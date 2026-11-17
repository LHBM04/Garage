#include "garage/graphics/gpu_buffer.hpp"

#include <cstring>
#include <span>
#include <utility>

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_gpu.h>

#include "garage/core/defines.hpp"
#include "garage/core/types.hpp"

#include "garage/graphics/gpu_types.hpp"

#include "garage/utilities/logger.hpp"

namespace Garage
{
    GPUBuffer::GPUBuffer(const GPUBufferCreateInfo& createInfo) noexcept
        : deviceHandle(createInfo.deviceHandle)
        , bufferHandle(createInfo.bufferHandle)
        , transferHandle(createInfo.transferHandle)
        , usage(createInfo.usage)
        , sizeInBytes(createInfo.sizeInBytes)
    {
        ASSERT(this->deviceHandle != nullptr);
        ASSERT(this->bufferHandle != nullptr || this->transferHandle != nullptr);
    }

    GPUBuffer::~GPUBuffer() noexcept
    {
        Release();
    }

    GPUBuffer::GPUBuffer(GPUBuffer&& other) noexcept
        : deviceHandle(std::exchange(other.deviceHandle, nullptr))
        , bufferHandle(std::exchange(other.bufferHandle, nullptr))
        , transferHandle(std::exchange(other.transferHandle, nullptr))
        , usage(other.usage)
        , sizeInBytes(std::exchange(other.sizeInBytes, 0))
    {
    }

    GPUBuffer& GPUBuffer::operator=(GPUBuffer&& other) noexcept
    {
        if (this != &other)
        {
            Release();
            deviceHandle   = std::exchange(other.deviceHandle, nullptr);
            bufferHandle   = std::exchange(other.bufferHandle, nullptr);
            transferHandle = std::exchange(other.transferHandle, nullptr);
            usage          = other.usage;
            sizeInBytes    = std::exchange(other.sizeInBytes, 0);
        }

        return *this;
    }

    bool GPUBuffer::Write(std::span<const UInt8> data, UInt64 offset) noexcept
    {
        if (deviceHandle == nullptr || transferHandle == nullptr)
        {
            LOG_ERROR("쓰기에 유효한 전송 버퍼가 필요합니다.");
            return false;
        }

        if (offset > sizeInBytes || data.size() > sizeInBytes - offset)
        {
            LOG_ERROR("GPU 버퍼 쓰기 범위가 유효 범위를 벗어났습니다.");
            return false;
        }

        if (data.empty())
        {
            return true;
        }

        auto* sdlDevice   = static_cast<SDL_GPUDevice*>(deviceHandle);
        auto* sdlTransfer = static_cast<SDL_GPUTransferBuffer*>(transferHandle);

        void* mappedData = SDL_MapGPUTransferBuffer(sdlDevice, sdlTransfer, false);
        if (mappedData == nullptr)
        {
            LOG_ERROR("쓰기를 위한 GPU 버퍼 매핑에 실패했습니다: {}", SDL_GetError());
            return false;
        }

        std::memcpy(static_cast<UInt8*>(mappedData) + offset, data.data(), data.size());
        SDL_UnmapGPUTransferBuffer(sdlDevice, sdlTransfer);

        if (bufferHandle != nullptr)
        {
            SDL_GPUCommandBuffer* copyCmd = SDL_AcquireGPUCommandBuffer(sdlDevice);
            if (copyCmd == nullptr)
            {
                LOG_ERROR("버퍼 업로드를 위한 GPU 명령 버퍼 획득에 실패했습니다: {}", SDL_GetError());
                return false;
            }

            SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(copyCmd);
            if (copyPass == nullptr)
            {
                SDL_CancelGPUCommandBuffer(copyCmd);
                LOG_ERROR("버퍼 업로드를 위한 복사 패스 시작에 실패했습니다: {}", SDL_GetError());
                return false;
            }

            const SDL_GPUTransferBufferLocation SOURCE{
                    .transfer_buffer = sdlTransfer,
                    .offset          = static_cast<UInt32>(offset),
            };
            const SDL_GPUBufferRegion DESTINATION{
                    .buffer = static_cast<SDL_GPUBuffer*>(bufferHandle),
                    .offset = static_cast<UInt32>(offset),
                    .size   = static_cast<UInt32>(data.size()),
            };

            SDL_UploadToGPUBuffer(copyPass, &SOURCE, &DESTINATION, false);
            SDL_EndGPUCopyPass(copyPass);

            if (!SDL_SubmitGPUCommandBuffer(copyCmd))
            {
                LOG_ERROR("버퍼 업로드 명령 버퍼 제출에 실패했습니다: {}", SDL_GetError());
                return false;
            }
        }

        return true;
    }

    bool GPUBuffer::Write(std::span<const Byte> data, UInt64 offset) noexcept
    {
        return Write(std::span<const UInt8>(reinterpret_cast<const UInt8*>(data.data()), data.size()), offset);
    }

    bool GPUBuffer::Read(std::span<UInt8> data, UInt64 offset) const noexcept
    {
        if (deviceHandle == nullptr || transferHandle == nullptr || usage != EGPUBufferUsage::READBACK)
        {
            LOG_ERROR("읽기에 유효한 READBACK 버퍼가 필요합니다.");
            return false;
        }

        if (offset > sizeInBytes || data.size() > sizeInBytes - offset)
        {
            LOG_ERROR("GPU 버퍼 읽기 범위가 유효 범위를 벗어났습니다.");
            return false;
        }

        if (data.empty())
        {
            return true;
        }

        auto* sdlDevice   = static_cast<SDL_GPUDevice*>(deviceHandle);
        auto* sdlTransfer = static_cast<SDL_GPUTransferBuffer*>(transferHandle);

        const void* mappedData = SDL_MapGPUTransferBuffer(sdlDevice, sdlTransfer, false);
        if (mappedData == nullptr)
        {
            LOG_ERROR("읽기를 위한 GPU 버퍼 매핑에 실패했습니다: {}", SDL_GetError());
            return false;
        }

        std::memcpy(data.data(), static_cast<const UInt8*>(mappedData) + offset, data.size());
        SDL_UnmapGPUTransferBuffer(sdlDevice, sdlTransfer);
        return true;
    }

    bool GPUBuffer::Read(std::span<Byte> data, UInt64 offset) const noexcept
    {
        return Read(std::span<UInt8>(reinterpret_cast<UInt8*>(data.data()), data.size()), offset);
    }

    void GPUBuffer::Release() noexcept
    {
        if (deviceHandle != nullptr)
        {
            auto* sdlDevice = static_cast<SDL_GPUDevice*>(deviceHandle);
            if (bufferHandle != nullptr)
            {
                SDL_ReleaseGPUBuffer(sdlDevice, static_cast<SDL_GPUBuffer*>(bufferHandle));
                bufferHandle = nullptr;
            }
            if (transferHandle != nullptr)
            {
                SDL_ReleaseGPUTransferBuffer(sdlDevice, static_cast<SDL_GPUTransferBuffer*>(transferHandle));
                transferHandle = nullptr;
            }
            deviceHandle = nullptr;
        }
        sizeInBytes = 0;
    }
} // namespace Garage
