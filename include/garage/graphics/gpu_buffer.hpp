#ifndef GARAGE_GRAPHICS_GPU_BUFFER_HPP
#define GARAGE_GRAPHICS_GPU_BUFFER_HPP

#include <span>
#include <string>

#include "../core/defines.hpp"
#include "../core/types.hpp"

#include "gpu_types.hpp"

namespace Garage
{
    class IGPUDevice;

    class GARAGE_API IGPUBuffer
    {
    public:
        IGPUBuffer() noexcept          = default;
        virtual ~IGPUBuffer() noexcept = default;

        IGPUBuffer(const IGPUBuffer&)            = delete;
        IGPUBuffer& operator=(const IGPUBuffer&) = delete;

        IGPUBuffer(IGPUBuffer&&)            = delete;
        IGPUBuffer& operator=(IGPUBuffer&&) = delete;

        [[nodiscard]] virtual Result<void, std::string> Write(std::span<const UInt8> data, UInt64 offset = 0) = 0;
        [[nodiscard]] virtual Result<void, std::string> Write(std::span<const Byte> data, UInt64 offset = 0)  = 0;
        [[nodiscard]] virtual Result<void, std::string> Read(std::span<UInt8> data, UInt64 offset = 0) const  = 0;
        [[nodiscard]] virtual Result<void, std::string> Read(std::span<Byte> data, UInt64 offset = 0) const   = 0;

        [[nodiscard]] virtual const IGPUDevice* GetDevice() const noexcept      = 0;
        [[nodiscard]] virtual UInt64            GetSizeInBytes() const noexcept = 0;
        [[nodiscard]] virtual EGPUBufferUsage   GetUsage() const noexcept       = 0;
    };
} // namespace Garage

#endif // GARAGE_GRAPHICS_GPU_BUFFER_HPP
