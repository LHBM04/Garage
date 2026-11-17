#ifndef GARAGE_GRAPHICS_GPU_GRAPHICS_PIPELINE_STATE_IMPL_HPP
#define GARAGE_GRAPHICS_GPU_GRAPHICS_PIPELINE_STATE_IMPL_HPP

#include <SDL3/SDL_gpu.h>

#include "garage/graphics/gpu_graphics_pipeline_state.hpp"

namespace Garage
{
    class GPUDeviceImpl;

    class GPUGraphicsPipelineStateImpl final : public IGPUGraphicsPipelineState
    {
    public:
        GPUGraphicsPipelineStateImpl(const GPUDeviceImpl* device, SDL_GPUGraphicsPipeline* pipelineState) noexcept;
        ~GPUGraphicsPipelineStateImpl() noexcept override;

        void Release() noexcept;

        [[nodiscard]] const IGPUDevice*        GetDevice() const noexcept override;
        [[nodiscard]] SDL_GPUGraphicsPipeline* GetSDLPipelineState() const noexcept;

    private:
        const GPUDeviceImpl*     device        = nullptr;
        SDL_GPUGraphicsPipeline* pipelineState = nullptr;
    };

    inline SDL_GPUGraphicsPipeline* GPUGraphicsPipelineStateImpl::GetSDLPipelineState() const noexcept
    {
        return pipelineState;
    }
} // namespace Garage

#endif // GARAGE_GRAPHICS_GPU_GRAPHICS_PIPELINE_STATE_IMPL_HPP
