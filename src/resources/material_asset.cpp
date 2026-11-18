#include "garage/resources/material_asset.hpp"

#include <filesystem>
#include <utility>

#include "garage/core/defines.hpp"

#include "garage/graphics/gpu_device.hpp"
#include "garage/graphics/gpu_graphics_pipeline_state.hpp"

namespace Garage
{
    MaterialAsset::MaterialAsset(std::filesystem::path      path,
                                 IGPUDevice&                device,
                                 const ShaderAsset*         vertexShader,
                                 const ShaderAsset*         fragmentShader,
                                 IGPUGraphicsPipelineState* pipelineState) noexcept
        : AssetBase(std::move(path))
        , device(&device)
        , vertexShader(vertexShader)
        , fragmentShader(fragmentShader)
        , pipelineState(pipelineState)
    {
        ASSERT(this->vertexShader != nullptr);
        ASSERT(this->fragmentShader != nullptr);
        ASSERT(this->pipelineState != nullptr);
    }

    MaterialAsset::~MaterialAsset() noexcept
    {
        if (device != nullptr && pipelineState != nullptr)
        {
            device->DestroyGraphicsPipelineState(pipelineState);
            pipelineState = nullptr;
        }
    }
} // namespace Garage
