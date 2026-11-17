#ifndef GARAGE_GRAPHICS_GPU_FENCE_HPP
#define GARAGE_GRAPHICS_GPU_FENCE_HPP

#include "../core/defines.hpp"

namespace Garage
{
    class IGPUDevice;

    class GARAGE_API IGPUFence
    {
    public:
        IGPUFence() noexcept          = default;
        virtual ~IGPUFence() noexcept = default;

        IGPUFence(const IGPUFence&)            = delete;
        IGPUFence& operator=(const IGPUFence&) = delete;

        IGPUFence(IGPUFence&&)            = delete;
        IGPUFence& operator=(IGPUFence&&) = delete;

        virtual void Wait() noexcept                           = 0;
        virtual void Signal(const IGPUDevice& device) noexcept = 0;

        [[nodiscard]] virtual const IGPUDevice* GetDevice() const noexcept   = 0;
        [[nodiscard]] virtual bool              IsCompleted() const noexcept = 0;
    };
} // namespace Garage

#endif // GARAGE_GRAPHICS_GPU_FENCE_HPP
