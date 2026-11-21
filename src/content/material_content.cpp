#include "garage/content/material_content.hpp"

#include <filesystem>
#include <utility>

#include "garage/core/common.hpp"

#include "garage/graphics/gpu_device.hpp"
#include "garage/graphics/gpu_graphics_pipeline_state.hpp"

namespace Garage
{
    MaterialContent::MaterialContent(std::filesystem::path      path,
                                     IGPUDevice&                device,
                                     const ShaderContent*       vertexShader,
                                     const ShaderContent*       fragmentShader,
                                     IGPUGraphicsPipelineState* pipelineState) noexcept
        : ContentBase(std::move(path))
        , device(&device)
        , vertexShader(vertexShader)
        , fragmentShader(fragmentShader)
        , pipelineState(pipelineState)
    {
        ASSERT(this->vertexShader != nullptr);
        ASSERT(this->fragmentShader != nullptr);
        ASSERT(this->pipelineState != nullptr);
    }

    MaterialContent::~MaterialContent() noexcept
    {
        if (device != nullptr && pipelineState != nullptr)
        {
            device->DestroyGraphicsPipelineState(pipelineState);
            pipelineState = nullptr;
        }
    }
} // namespace Garage
