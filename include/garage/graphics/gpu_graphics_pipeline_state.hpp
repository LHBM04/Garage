#ifndef GARAGE_GRAPHICS_GPU_GRAPHICS_PIPELINE_STATE_HPP
#define GARAGE_GRAPHICS_GPU_GRAPHICS_PIPELINE_STATE_HPP

#include <span>

#include "../core/defines.hpp"
#include "../core/types.hpp"

#include "gpu_types.hpp"

namespace Garage
{
    class GPUShader;

    struct GARAGE_API GPUGraphicsPipelineDesc final
    {
        const GPUShader*                    vertexShader          = nullptr;
        const GPUShader*                    fragmentShader        = nullptr;
        std::span<const GPUVertexAttribute> vertexAttributes      = {};
        UInt32                              vertexStride          = 0;
        EGPUPrimitiveTopology               primitiveTopology     = EGPUPrimitiveTopology::TRIANGLE_LIST;
        EGPUFillMode                        fillMode              = EGPUFillMode::FILL;
        EGPUCullMode                        cullMode              = EGPUCullMode::BACK;
        EGPUFrontFace                       frontFace             = EGPUFrontFace::CLOCKWISE;
        bool                                enableDepthClip       = true;
        bool                                enableDepthTest       = true;
        bool                                enableDepthWrite      = true;
        EGPUCompareOp                       depthCompareOp        = EGPUCompareOp::LESS_OR_EQUAL;
        bool                                enableBlend           = false;
        EGPUBlendFactor                     srcColorBlendFactor   = EGPUBlendFactor::ONE;
        EGPUBlendFactor                     dstColorBlendFactor   = EGPUBlendFactor::ZERO;
        EGPUBlendOp                         colorBlendOp          = EGPUBlendOp::ADD;
        EGPUBlendFactor                     srcAlphaBlendFactor   = EGPUBlendFactor::ONE;
        EGPUBlendFactor                     dstAlphaBlendFactor   = EGPUBlendFactor::ZERO;
        EGPUBlendOp                         alphaBlendOp          = EGPUBlendOp::ADD;
        EGPUTextureFormat                   colorTargetFormat     = EGPUTextureFormat::R8G8B8A8_UNORM;
        EGPUTextureFormat                   depthStencilFormat    = EGPUTextureFormat::D32_FLOAT;
        bool                                hasDepthStencilTarget = true;
    };

    class GARAGE_API GPUGraphicsPipelineState final
    {
    public:
        GPUGraphicsPipelineState(void* deviceHandle, void* pipelineHandle) noexcept;
        ~GPUGraphicsPipelineState() noexcept;

        GPUGraphicsPipelineState(const GPUGraphicsPipelineState&)            = delete;
        GPUGraphicsPipelineState& operator=(const GPUGraphicsPipelineState&) = delete;

        GPUGraphicsPipelineState(GPUGraphicsPipelineState&& other) noexcept;
        GPUGraphicsPipelineState& operator=(GPUGraphicsPipelineState&& other) noexcept;

        [[nodiscard]] void* GetNativeHandle() const noexcept;

    private:
        void Release() noexcept;

        void* deviceHandle   = nullptr;
        void* pipelineHandle = nullptr;
    };

    inline void* GPUGraphicsPipelineState::GetNativeHandle() const noexcept
    {
        return pipelineHandle;
    }
} // namespace Garage

#endif // GARAGE_GRAPHICS_GPU_GRAPHICS_PIPELINE_STATE_HPP
