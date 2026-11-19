#ifndef GARAGE_GRAPHICS_GPU_GRAPHICS_PIPELINE_STATE_HPP
#define GARAGE_GRAPHICS_GPU_GRAPHICS_PIPELINE_STATE_HPP

#include <span>

#include "../core/common.hpp"

#include "gpu_types.hpp"

namespace Garage
{
    class IGPUDevice;
    class IGPUShader;

    struct GARAGE_API GPUGraphicsPipelineDesc final
    {
        const IGPUShader*                   vertexShader          = nullptr;
        const IGPUShader*                   fragmentShader        = nullptr;
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

    class GARAGE_API IGPUGraphicsPipelineState
    {
    public:
        IGPUGraphicsPipelineState() noexcept          = default;
        virtual ~IGPUGraphicsPipelineState() noexcept = default;

        IGPUGraphicsPipelineState(const IGPUGraphicsPipelineState&)            = delete;
        IGPUGraphicsPipelineState& operator=(const IGPUGraphicsPipelineState&) = delete;

        IGPUGraphicsPipelineState(IGPUGraphicsPipelineState&&)            = delete;
        IGPUGraphicsPipelineState& operator=(IGPUGraphicsPipelineState&&) = delete;

        [[nodiscard]] virtual const IGPUDevice* GetDevice() const noexcept = 0;
    };
} // namespace Garage

#endif // GARAGE_GRAPHICS_GPU_GRAPHICS_PIPELINE_STATE_HPP
