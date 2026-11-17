#ifndef GARAGE_GRAPHICS_GPU_FENCE_HPP
#define GARAGE_GRAPHICS_GPU_FENCE_HPP

#include "../core/defines.hpp"
#include "../core/types.hpp"

namespace Garage
{
    class GPUDevice;

    class GARAGE_API GPUFence final
    {
    public:
        explicit GPUFence(void* deviceHandle, void* fenceHandle = nullptr) noexcept;
        ~GPUFence() noexcept;

        GPUFence(const GPUFence&)            = delete;
        GPUFence& operator=(const GPUFence&) = delete;

        GPUFence(GPUFence&& other) noexcept;
        GPUFence& operator=(GPUFence&& other) noexcept;

        void Wait() const noexcept;
        void Signal(const GPUDevice& device) noexcept;
        void BindNativeFence(void* newFenceHandle) noexcept;

        [[nodiscard]] bool  IsCompleted() const noexcept;
        [[nodiscard]] void* GetNativeHandle() const noexcept;

    private:
        void Release() noexcept;

        void*  deviceHandle = nullptr;
        void*  fenceHandle  = nullptr;
        UInt64 fenceValue   = 0;
    };

    inline void* GPUFence::GetNativeHandle() const noexcept
    {
        return fenceHandle;
    }
} // namespace Garage

#endif // GARAGE_GRAPHICS_GPU_FENCE_HPP
