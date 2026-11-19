#include "gpu_buffer_impl.hpp"

#include <cstring>
#include <limits>
#include <span>
#include <string>

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_gpu.h>

#include "garage/core/common.hpp"

#include "garage/graphics/gpu_device.hpp"
#include "garage/graphics/gpu_types.hpp"

#include "gpu_device_impl.hpp"
#include "gpu_sdl_conversions.hpp"

namespace Garage
{
    GPUBufferImpl::GPUBufferImpl(const GPUDeviceImpl*   device,
                                 EGPUBufferUsage        usage,
                                 UInt64                 sizeInBytes,
                                 SDL_GPUBuffer*         buffer,
                                 SDL_GPUTransferBuffer* transferBuffer) noexcept
        : device(device)
        , buffer(buffer)
        , transferBuffer(transferBuffer)
        , sizeInBytes(sizeInBytes)
        , usage(usage)
    {
    }

    GPUBufferImpl::~GPUBufferImpl() noexcept
    {
        Release();
    }

    Result<void, std::pmr::string> GPUBufferImpl::Write(std::span<const UInt8> data, UInt64 offset)
    {
        if (device == nullptr || device->GetSDLDevice() == nullptr)
        {
            return Result<void, std::pmr::string>::Err(GraphicsDetail::MakeError("유효한 GPU 디바이스가 필요합니다."));
        }

        if (offset > sizeInBytes || data.size() > sizeInBytes - offset)
        {
            return Result<void, std::pmr::string>::Err(
                    GraphicsDetail::MakeError("GPU 버퍼 쓰기 범위가 유효 범위를 벗어났습니다."));
        }

        if (data.empty())
        {
            return Result<void, std::pmr::string>::Ok();
        }

        if (usage == EGPUBufferUsage::READBACK)
        {
            return Result<void, std::pmr::string>::Err(
                    GraphicsDetail::MakeError("READBACK 전용 GPU 버퍼에는 쓸 수 없습니다."));
        }

        if (data.size() > std::numeric_limits<UInt32>::max() || offset > std::numeric_limits<UInt32>::max())
        {
            return Result<void, std::pmr::string>::Err(
                    GraphicsDetail::MakeError("GPU 버퍼 쓰기 크기 또는 오프셋이 32비트 범위를 초과했습니다."));
        }

        SDL_GPUDevice* sdlDevice = device->GetSDLDevice();

        if (transferBuffer != nullptr)
        {
            void* mapped = SDL_MapGPUTransferBuffer(sdlDevice, transferBuffer, false);
            if (mapped == nullptr)
            {
                return Result<void, std::pmr::string>::Err(
                        GraphicsDetail::FormatError("GPU 전송 버퍼 매핑에 실패했습니다: {}", SDL_GetError()));
            }

            std::memcpy(static_cast<UInt8*>(mapped) + offset, data.data(), data.size());
            SDL_UnmapGPUTransferBuffer(sdlDevice, transferBuffer);
            return Result<void, std::pmr::string>::Ok();
        }

        if (buffer == nullptr)
        {
            return Result<void, std::pmr::string>::Err(GraphicsDetail::MakeError("유효한 GPU 버퍼 핸들이 없습니다."));
        }

        SDL_GPUTransferBufferCreateInfo stagingInfo{};
        stagingInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
        stagingInfo.size  = static_cast<UInt32>(data.size());

        SDL_GPUTransferBuffer* stagingBuffer = SDL_CreateGPUTransferBuffer(sdlDevice, &stagingInfo);
        if (stagingBuffer == nullptr)
        {
            return Result<void, std::pmr::string>::Err(
                    GraphicsDetail::FormatError("임시 GPU 업로드 버퍼 생성에 실패했습니다: {}", SDL_GetError()));
        }

        void* mapped = SDL_MapGPUTransferBuffer(sdlDevice, stagingBuffer, false);
        if (mapped == nullptr)
        {
            SDL_ReleaseGPUTransferBuffer(sdlDevice, stagingBuffer);
            return Result<void, std::pmr::string>::Err(
                    GraphicsDetail::FormatError("임시 GPU 업로드 버퍼 매핑에 실패했습니다: {}", SDL_GetError()));
        }

        std::memcpy(mapped, data.data(), data.size());
        SDL_UnmapGPUTransferBuffer(sdlDevice, stagingBuffer);

        SDL_GPUCommandBuffer* sdlCommandBuffer = SDL_AcquireGPUCommandBuffer(sdlDevice);
        if (sdlCommandBuffer == nullptr)
        {
            SDL_ReleaseGPUTransferBuffer(sdlDevice, stagingBuffer);
            return Result<void, std::pmr::string>::Err(
                    GraphicsDetail::FormatError("GPU 업로드용 명령 버퍼 획득에 실패했습니다: {}", SDL_GetError()));
        }

        SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(sdlCommandBuffer);
        if (copyPass == nullptr)
        {
            SDL_CancelGPUCommandBuffer(sdlCommandBuffer);
            SDL_ReleaseGPUTransferBuffer(sdlDevice, stagingBuffer);
            return Result<void, std::pmr::string>::Err(
                    GraphicsDetail::FormatError("GPU 복사 패스 시작에 실패했습니다: {}", SDL_GetError()));
        }

        SDL_GPUTransferBufferLocation sourceLocation{};
        sourceLocation.transfer_buffer = stagingBuffer;
        sourceLocation.offset          = 0;

        SDL_GPUBufferRegion destinationRegion{};
        destinationRegion.buffer = buffer;
        destinationRegion.offset = static_cast<UInt32>(offset);
        destinationRegion.size   = static_cast<UInt32>(data.size());

        SDL_UploadToGPUBuffer(copyPass, &sourceLocation, &destinationRegion, false);
        SDL_EndGPUCopyPass(copyPass);

        SDL_GPUFence* uploadFence = SDL_SubmitGPUCommandBufferAndAcquireFence(sdlCommandBuffer);
        if (uploadFence == nullptr)
        {
            SDL_ReleaseGPUTransferBuffer(sdlDevice, stagingBuffer);
            return Result<void, std::pmr::string>::Err(
                    GraphicsDetail::FormatError("GPU 버퍼 업로드 제출에 실패했습니다: {}", SDL_GetError()));
        }

        SDL_WaitForGPUFences(sdlDevice, true, &uploadFence, 1);
        SDL_ReleaseGPUFence(sdlDevice, uploadFence);
        SDL_ReleaseGPUTransferBuffer(sdlDevice, stagingBuffer);
        return Result<void, std::pmr::string>::Ok();
    }

    Result<void, std::pmr::string> GPUBufferImpl::Write(std::span<const Byte> data, UInt64 offset)
    {
        return Write(std::span<const UInt8>{reinterpret_cast<const UInt8*>(data.data()), data.size()}, offset);
    }

    Result<void, std::pmr::string> GPUBufferImpl::Read(std::span<UInt8> data, UInt64 offset) const
    {
        if (device == nullptr || device->GetSDLDevice() == nullptr)
        {
            return Result<void, std::pmr::string>::Err(GraphicsDetail::MakeError("유효한 GPU 디바이스가 필요합니다."));
        }

        if (offset > sizeInBytes || data.size() > sizeInBytes - offset)
        {
            return Result<void, std::pmr::string>::Err(
                    GraphicsDetail::MakeError("GPU 버퍼 읽기 범위가 유효 범위를 벗어났습니다."));
        }

        if (data.empty())
        {
            return Result<void, std::pmr::string>::Ok();
        }

        if (data.size() > std::numeric_limits<UInt32>::max() || offset > std::numeric_limits<UInt32>::max())
        {
            return Result<void, std::pmr::string>::Err(
                    GraphicsDetail::MakeError("GPU 버퍼 읽기 크기 또는 오프셋이 32비트 범위를 초과했습니다."));
        }

        SDL_GPUDevice* sdlDevice = device->GetSDLDevice();

        if (transferBuffer != nullptr)
        {
            void* mapped = SDL_MapGPUTransferBuffer(sdlDevice, transferBuffer, false);
            if (mapped == nullptr)
            {
                return Result<void, std::pmr::string>::Err(
                        GraphicsDetail::FormatError("GPU 전송 버퍼 매핑에 실패했습니다: {}", SDL_GetError()));
            }

            std::memcpy(data.data(), static_cast<const UInt8*>(mapped) + offset, data.size());
            SDL_UnmapGPUTransferBuffer(sdlDevice, transferBuffer);
            return Result<void, std::pmr::string>::Ok();
        }

        if (buffer == nullptr)
        {
            return Result<void, std::pmr::string>::Err(GraphicsDetail::MakeError("유효한 GPU 버퍼 핸들이 없습니다."));
        }

        SDL_GPUTransferBufferCreateInfo stagingInfo{};
        stagingInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_DOWNLOAD;
        stagingInfo.size  = static_cast<UInt32>(data.size());

        SDL_GPUTransferBuffer* stagingBuffer = SDL_CreateGPUTransferBuffer(sdlDevice, &stagingInfo);
        if (stagingBuffer == nullptr)
        {
            return Result<void, std::pmr::string>::Err(
                    GraphicsDetail::FormatError("임시 GPU 다운로드 버퍼 생성에 실패했습니다: {}", SDL_GetError()));
        }

        SDL_GPUCommandBuffer* sdlCommandBuffer = SDL_AcquireGPUCommandBuffer(sdlDevice);
        if (sdlCommandBuffer == nullptr)
        {
            SDL_ReleaseGPUTransferBuffer(sdlDevice, stagingBuffer);
            return Result<void, std::pmr::string>::Err(
                    GraphicsDetail::FormatError("GPU 다운로드용 명령 버퍼 획득에 실패했습니다: {}", SDL_GetError()));
        }

        SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(sdlCommandBuffer);
        if (copyPass == nullptr)
        {
            SDL_CancelGPUCommandBuffer(sdlCommandBuffer);
            SDL_ReleaseGPUTransferBuffer(sdlDevice, stagingBuffer);
            return Result<void, std::pmr::string>::Err(
                    GraphicsDetail::FormatError("GPU 복사 패스 시작에 실패했습니다: {}", SDL_GetError()));
        }

        SDL_GPUBufferRegion sourceRegion{};
        sourceRegion.buffer = buffer;
        sourceRegion.offset = static_cast<UInt32>(offset);
        sourceRegion.size   = static_cast<UInt32>(data.size());

        SDL_GPUTransferBufferLocation destinationLocation{};
        destinationLocation.transfer_buffer = stagingBuffer;
        destinationLocation.offset          = 0;

        SDL_DownloadFromGPUBuffer(copyPass, &sourceRegion, &destinationLocation);
        SDL_EndGPUCopyPass(copyPass);

        SDL_GPUFence* downloadFence = SDL_SubmitGPUCommandBufferAndAcquireFence(sdlCommandBuffer);
        if (downloadFence == nullptr)
        {
            SDL_ReleaseGPUTransferBuffer(sdlDevice, stagingBuffer);
            return Result<void, std::pmr::string>::Err(
                    GraphicsDetail::FormatError("GPU 버퍼 다운로드 제출에 실패했습니다: {}", SDL_GetError()));
        }

        SDL_WaitForGPUFences(sdlDevice, true, &downloadFence, 1);
        SDL_ReleaseGPUFence(sdlDevice, downloadFence);

        void* mapped = SDL_MapGPUTransferBuffer(sdlDevice, stagingBuffer, false);
        if (mapped == nullptr)
        {
            SDL_ReleaseGPUTransferBuffer(sdlDevice, stagingBuffer);
            return Result<void, std::pmr::string>::Err(
                    GraphicsDetail::FormatError("임시 GPU 다운로드 버퍼 매핑에 실패했습니다: {}", SDL_GetError()));
        }

        std::memcpy(data.data(), mapped, data.size());
        SDL_UnmapGPUTransferBuffer(sdlDevice, stagingBuffer);
        SDL_ReleaseGPUTransferBuffer(sdlDevice, stagingBuffer);
        return Result<void, std::pmr::string>::Ok();
    }

    Result<void, std::pmr::string> GPUBufferImpl::Read(std::span<Byte> data, UInt64 offset) const
    {
        return Read(std::span<UInt8>{reinterpret_cast<UInt8*>(data.data()), data.size()}, offset);
    }

    void GPUBufferImpl::Release() noexcept
    {
        if (device != nullptr && device->GetSDLDevice() != nullptr)
        {
            SDL_GPUDevice* sdlDevice = device->GetSDLDevice();

            if (buffer != nullptr)
            {
                SDL_ReleaseGPUBuffer(sdlDevice, buffer);
            }

            if (transferBuffer != nullptr)
            {
                SDL_ReleaseGPUTransferBuffer(sdlDevice, transferBuffer);
            }
        }

        buffer         = nullptr;
        transferBuffer = nullptr;
        device         = nullptr;
    }

    const IGPUDevice* GPUBufferImpl::GetDevice() const noexcept
    {
        return device;
    }
} // namespace Garage
