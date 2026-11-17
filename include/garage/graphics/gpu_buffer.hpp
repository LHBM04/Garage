#ifndef GARAGE_GRAPHICS_GPU_BUFFER_HPP
#define GARAGE_GRAPHICS_GPU_BUFFER_HPP

#include <span>

#include "../core/defines.hpp"
#include "../core/types.hpp"

#include "gpu_types.hpp"

namespace Garage
{
    struct GARAGE_API GPUBufferCreateInfo final
    {
        void*           deviceHandle   = nullptr;
        void*           bufferHandle   = nullptr;
        void*           transferHandle = nullptr;
        EGPUBufferUsage usage          = EGPUBufferUsage::VERTEX;
        UInt64          sizeInBytes    = 0;
    };

    class GARAGE_API GPUBuffer final
    {
    public:
        explicit GPUBuffer(const GPUBufferCreateInfo& createInfo) noexcept;
        ~GPUBuffer() noexcept;

        GPUBuffer(const GPUBuffer&)            = delete;
        GPUBuffer& operator=(const GPUBuffer&) = delete;

        GPUBuffer(GPUBuffer&& other) noexcept;
        GPUBuffer& operator=(GPUBuffer&& other) noexcept;

        [[nodiscard]] bool Write(std::span<const UInt8> data, UInt64 offset = 0) noexcept;
        [[nodiscard]] bool Write(std::span<const Byte> data, UInt64 offset = 0) noexcept;
        [[nodiscard]] bool Read(std::span<UInt8> data, UInt64 offset = 0) const noexcept;
        [[nodiscard]] bool Read(std::span<Byte> data, UInt64 offset = 0) const noexcept;

        [[nodiscard]] UInt64          GetSizeInBytes() const noexcept;
        [[nodiscard]] EGPUBufferUsage GetUsage() const noexcept;
        [[nodiscard]] void*           GetNativeBuffer() const noexcept;
        [[nodiscard]] void*           GetNativeTransferBuffer() const noexcept;

    private:
        void Release() noexcept;

        void*           deviceHandle   = nullptr;
        void*           bufferHandle   = nullptr;
        void*           transferHandle = nullptr;
        EGPUBufferUsage usage          = EGPUBufferUsage::VERTEX;
        UInt64          sizeInBytes    = 0;
    };

    inline UInt64 GPUBuffer::GetSizeInBytes() const noexcept
    {
        return sizeInBytes;
    }

    inline EGPUBufferUsage GPUBuffer::GetUsage() const noexcept
    {
        return usage;
    }

    inline void* GPUBuffer::GetNativeBuffer() const noexcept
    {
        return bufferHandle;
    }

    inline void* GPUBuffer::GetNativeTransferBuffer() const noexcept
    {
        return transferHandle;
    }
} // namespace Garage

#endif // GARAGE_GRAPHICS_GPU_BUFFER_HPP
