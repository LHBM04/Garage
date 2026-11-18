#ifndef GARAGE_GRAPHICS_GPU_BUFFER_IMPL_HPP
#define GARAGE_GRAPHICS_GPU_BUFFER_IMPL_HPP

#include <span>
#include <string>

#include <SDL3/SDL_gpu.h>

#include "garage/core/types.hpp"

#include "garage/graphics/gpu_buffer.hpp"
#include "garage/graphics/gpu_types.hpp"

namespace Garage
{
    class GPUDeviceImpl;

    class GPUBufferImpl final : public IGPUBuffer
    {
    public:
        GPUBufferImpl(const GPUDeviceImpl*   device,
                      EGPUBufferUsage        usage,
                      UInt64                 sizeInBytes,
                      SDL_GPUBuffer*         buffer,
                      SDL_GPUTransferBuffer* transferBuffer) noexcept;
        ~GPUBufferImpl() noexcept override;

        [[nodiscard]] Result<void, std::string> Write(std::span<const UInt8> data, UInt64 offset = 0) override;
        [[nodiscard]] Result<void, std::string> Write(std::span<const Byte> data, UInt64 offset = 0) override;
        [[nodiscard]] Result<void, std::string> Read(std::span<UInt8> data, UInt64 offset = 0) const override;
        [[nodiscard]] Result<void, std::string> Read(std::span<Byte> data, UInt64 offset = 0) const override;

        void Release() noexcept;

        [[nodiscard]] const IGPUDevice*      GetDevice() const noexcept override;
        [[nodiscard]] UInt64                 GetSizeInBytes() const noexcept override;
        [[nodiscard]] EGPUBufferUsage        GetUsage() const noexcept override;
        [[nodiscard]] SDL_GPUBuffer*         GetSDLBuffer() const noexcept;
        [[nodiscard]] SDL_GPUTransferBuffer* GetSDLTransferBuffer() const noexcept;

    private:
        const GPUDeviceImpl*   device         = nullptr;
        SDL_GPUBuffer*         buffer         = nullptr;
        SDL_GPUTransferBuffer* transferBuffer = nullptr;
        UInt64                 sizeInBytes    = 0;
        EGPUBufferUsage        usage          = EGPUBufferUsage::UPLOAD;
    };

    inline UInt64 GPUBufferImpl::GetSizeInBytes() const noexcept
    {
        return sizeInBytes;
    }

    inline EGPUBufferUsage GPUBufferImpl::GetUsage() const noexcept
    {
        return usage;
    }

    inline SDL_GPUBuffer* GPUBufferImpl::GetSDLBuffer() const noexcept
    {
        return buffer;
    }

    inline SDL_GPUTransferBuffer* GPUBufferImpl::GetSDLTransferBuffer() const noexcept
    {
        return transferBuffer;
    }
} // namespace Garage

#endif // GARAGE_GRAPHICS_GPU_BUFFER_IMPL_HPP
