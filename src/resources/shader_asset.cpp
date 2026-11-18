#include "garage/resources/shader_asset.hpp"

#include <filesystem>
#include <utility>

#include "garage/graphics/gpu_device.hpp"
#include "garage/graphics/gpu_graphics_pipeline_state.hpp"
#include "garage/graphics/gpu_shader.hpp"
#include "garage/graphics/gpu_types.hpp"

namespace Garage
{
    ShaderAsset::ShaderAsset(std::filesystem::path path, IGPUDevice& device, IGPUShader* shader) noexcept
        : AssetBase(std::move(path))
        , device(&device)
        , shader(shader)
    {
    }

    ShaderAsset::~ShaderAsset() noexcept
    {
        if (device != nullptr && shader != nullptr)
        {
            device->DestroyShader(shader);
            shader = nullptr;
        }
    }

    GPUGraphicsPipelineDesc ShaderAsset::CreatePipelineDescription(const ShaderAsset& fragmentShader,
                                                                   EGPUTextureFormat  colorTargetFormat,
                                                                   EGPUTextureFormat  depthStencilFormat) const noexcept
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
