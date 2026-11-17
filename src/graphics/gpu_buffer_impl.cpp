#include "gpu_buffer_impl.hpp"

#include <cstring>
#include <limits>
#include <span>

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_gpu.h>

#include "garage/core/types.hpp"

#include "garage/graphics/gpu_device.hpp"
#include "garage/graphics/gpu_types.hpp"

#include "garage/utilities/logger.hpp"

#include "gpu_device_impl.hpp"

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

    bool GPUBufferImpl::Write(std::span<const UInt8> data, UInt64 offset) noexcept
    {
        if (device == nullptr || device->GetSDLDevice() == nullptr)
        {
            return false;
        }

        if (data.empty())
        {
            return offset <= sizeInBytes;
        }

        if (offset > sizeInBytes || data.size() > sizeInBytes - offset)
        {
            LOG_ERROR("GPU 버퍼 쓰기 범위가 버퍼 크기를 초과했습니다.");
            return false;
        }

        if (usage == EGPUBufferUsage::READBACK)
        {
            LOG_ERROR("READBACK 전용 GPU 버퍼에는 쓸 수 없습니다.");
            return false;
        }

        if (data.size() > std::numeric_limits<UInt32>::max() || offset > std::numeric_limits<UInt32>::max())
        {
            LOG_ERROR("GPU 버퍼 쓰기 크기 또는 오프셋이 32비트 범위를 초과했습니다.");
            return false;
        }

        SDL_GPUDevice* sdlDevice = device->GetSDLDevice();

        if (transferBuffer != nullptr)
        {
            void* mapped = SDL_MapGPUTransferBuffer(sdlDevice, transferBuffer, false);
            if (mapped == nullptr)
            {
                LOG_ERROR("GPU 전송 버퍼 매핑에 실패했습니다: {}", SDL_GetError());
                return false;
            }

            std::memcpy(static_cast<UInt8*>(mapped) + offset, data.data(), data.size());
            SDL_UnmapGPUTransferBuffer(sdlDevice, transferBuffer);
            return true;
        }

        if (buffer == nullptr)
        {
            return false;
        }

        SDL_GPUTransferBufferCreateInfo stagingInfo{};
        stagingInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
        stagingInfo.size  = static_cast<UInt32>(data.size());

        SDL_GPUTransferBuffer* stagingBuffer = SDL_CreateGPUTransferBuffer(sdlDevice, &stagingInfo);
        if (stagingBuffer == nullptr)
        {
            LOG_ERROR("임시 GPU 업로드 버퍼 생성에 실패했습니다: {}", SDL_GetError());
            return false;
        }

        void* mapped = SDL_MapGPUTransferBuffer(sdlDevice, stagingBuffer, false);
        if (mapped == nullptr)
        {
            SDL_ReleaseGPUTransferBuffer(sdlDevice, stagingBuffer);
            LOG_ERROR("임시 GPU 업로드 버퍼 매핑에 실패했습니다: {}", SDL_GetError());
            return false;
        }

        std::memcpy(mapped, data.data(), data.size());
        SDL_UnmapGPUTransferBuffer(sdlDevice, stagingBuffer);

        SDL_GPUCommandBuffer* sdlCommandBuffer = SDL_AcquireGPUCommandBuffer(sdlDevice);
        if (sdlCommandBuffer == nullptr)
        {
            SDL_ReleaseGPUTransferBuffer(sdlDevice, stagingBuffer);
            LOG_ERROR("GPU 업로드용 명령 버퍼 획득에 실패했습니다: {}", SDL_GetError());
            return false;
        }

        SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(sdlCommandBuffer);
        if (copyPass == nullptr)
        {
            SDL_CancelGPUCommandBuffer(sdlCommandBuffer);
            SDL_ReleaseGPUTransferBuffer(sdlDevice, stagingBuffer);
            LOG_ERROR("GPU 복사 패스 시작에 실패했습니다: {}", SDL_GetError());
            return false;
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
            LOG_ERROR("GPU 버퍼 업로드 제출에 실패했습니다: {}", SDL_GetError());
            return false;
        }

        SDL_WaitForGPUFences(sdlDevice, true, &uploadFence, 1);
        SDL_ReleaseGPUFence(sdlDevice, uploadFence);
        SDL_ReleaseGPUTransferBuffer(sdlDevice, stagingBuffer);
        return true;
    }

    bool GPUBufferImpl::Write(std::span<const Byte> data, UInt64 offset) noexcept
    {
        return Write(std::span<const UInt8>{reinterpret_cast<const UInt8*>(data.data()), data.size()}, offset);
    }

    bool GPUBufferImpl::Read(std::span<UInt8> data, UInt64 offset) const noexcept
    {
        if (device == nullptr || device->GetSDLDevice() == nullptr)
        {
            return false;
        }

        if (data.empty())
        {
            return offset <= sizeInBytes;
        }

        if (offset > sizeInBytes || data.size() > sizeInBytes - offset)
        {
            LOG_ERROR("GPU 버퍼 읽기 범위가 버퍼 크기를 초과했습니다.");
            return false;
        }

        if (data.size() > std::numeric_limits<UInt32>::max() || offset > std::numeric_limits<UInt32>::max())
        {
            LOG_ERROR("GPU 버퍼 읽기 크기 또는 오프셋이 32비트 범위를 초과했습니다.");
            return false;
        }

        SDL_GPUDevice* sdlDevice = device->GetSDLDevice();

        if (transferBuffer != nullptr)
        {
            void* mapped = SDL_MapGPUTransferBuffer(sdlDevice, transferBuffer, false);
            if (mapped == nullptr)
            {
                LOG_ERROR("GPU 전송 버퍼 매핑에 실패했습니다: {}", SDL_GetError());
                return false;
            }

            std::memcpy(data.data(), static_cast<const UInt8*>(mapped) + offset, data.size());
            SDL_UnmapGPUTransferBuffer(sdlDevice, transferBuffer);
            return true;
        }

        if (buffer == nullptr)
        {
            return false;
        }

        SDL_GPUTransferBufferCreateInfo stagingInfo{};
        stagingInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_DOWNLOAD;
        stagingInfo.size  = static_cast<UInt32>(data.size());

        SDL_GPUTransferBuffer* stagingBuffer = SDL_CreateGPUTransferBuffer(sdlDevice, &stagingInfo);
        if (stagingBuffer == nullptr)
        {
            LOG_ERROR("임시 GPU 다운로드 버퍼 생성에 실패했습니다: {}", SDL_GetError());
            return false;
        }

        SDL_GPUCommandBuffer* sdlCommandBuffer = SDL_AcquireGPUCommandBuffer(sdlDevice);
        if (sdlCommandBuffer == nullptr)
        {
            SDL_ReleaseGPUTransferBuffer(sdlDevice, stagingBuffer);
            LOG_ERROR("GPU 다운로드용 명령 버퍼 획득에 실패했습니다: {}", SDL_GetError());
            return false;
        }

        SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(sdlCommandBuffer);
        if (copyPass == nullptr)
        {
            SDL_CancelGPUCommandBuffer(sdlCommandBuffer);
            SDL_ReleaseGPUTransferBuffer(sdlDevice, stagingBuffer);
            LOG_ERROR("GPU 복사 패스 시작에 실패했습니다: {}", SDL_GetError());
            return false;
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
            LOG_ERROR("GPU 버퍼 다운로드 제출에 실패했습니다: {}", SDL_GetError());
            return false;
        }

        SDL_WaitForGPUFences(sdlDevice, true, &downloadFence, 1);
        SDL_ReleaseGPUFence(sdlDevice, downloadFence);

        void* mapped = SDL_MapGPUTransferBuffer(sdlDevice, stagingBuffer, false);
        if (mapped == nullptr)
        {
            SDL_ReleaseGPUTransferBuffer(sdlDevice, stagingBuffer);
            LOG_ERROR("임시 GPU 다운로드 버퍼 매핑에 실패했습니다: {}", SDL_GetError());
            return false;
        }

        std::memcpy(data.data(), mapped, data.size());
        SDL_UnmapGPUTransferBuffer(sdlDevice, stagingBuffer);
        SDL_ReleaseGPUTransferBuffer(sdlDevice, stagingBuffer);
        return true;
    }

    bool GPUBufferImpl::Read(std::span<Byte> data, UInt64 offset) const noexcept
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
