#include "garage/content/shader_content.hpp"

#include <filesystem>
#include <utility>

#include "garage/graphics/gpu_device.hpp"
#include "garage/graphics/gpu_graphics_pipeline_state.hpp"
#include "garage/graphics/gpu_shader.hpp"
#include "garage/graphics/gpu_types.hpp"

namespace Garage
{
    ShaderContent::ShaderContent(std::filesystem::path path, IGPUDevice& device, IGPUShader* shader) noexcept
        : ContentBase(std::move(path))
        , device(&device)
        , shader(shader)
    {
    }

    ShaderContent::~ShaderContent() noexcept
    {
        if (device != nullptr && shader != nullptr)
        {
            device->DestroyShader(shader);
            shader = nullptr;
        }
    }

    GPUGraphicsPipelineDesc
            ShaderContent::CreatePipelineDescription(const ShaderContent& fragmentShader,
                                                     EGPUTextureFormat    colorTargetFormat,
                                                     EGPUTextureFormat    depthStencilFormat) const noexcept
    {
        return GPUGraphicsPipelineDesc{
                .vertexShader          = shader,
                .fragmentShader        = fragmentShader.GetShader(),
                .vertexAttributes      = vertexAttributes,
                .vertexStride          = vertexStride,
                .primitiveTopology     = primitive,
                .fillMode              = fillMode,
                .cullMode              = cullMode,
                .frontFace             = frontFace,
                .enableDepthClip       = true,
                .enableDepthTest       = enableDepthTest,
                .enableDepthWrite      = enableDepthWrite,
                .depthCompareOp        = depthCompareOp,
                .enableBlend           = enableBlend,
                .srcColorBlendFactor   = srcColorBlendFactor,
                .dstColorBlendFactor   = dstColorBlendFactor,
                .colorBlendOp          = colorBlendOp,
                .srcAlphaBlendFactor   = srcAlphaBlendFactor,
                .dstAlphaBlendFactor   = dstAlphaBlendFactor,
                .alphaBlendOp          = alphaBlendOp,
                .colorTargetFormat     = colorTargetFormat,
                .depthStencilFormat    = depthStencilFormat,
                .hasDepthStencilTarget = (depthStencilFormat != EGPUTextureFormat::UNKNOWN),
        };
    }
} // namespace Garage
