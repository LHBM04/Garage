#include "garage/graphics/skybox_renderer.hpp"

#include "garage/content/cubemap_content.hpp"

#include "garage/core/common.hpp"
#include "garage/core/common/containers.hpp"
#include "garage/core/system_context.hpp"

#include "garage/graphics/gpu_command_buffer.hpp"
#include "garage/graphics/gpu_constants.hpp"
#include "garage/graphics/gpu_graphics_pipeline_state.hpp"
#include "garage/graphics/gpu_texture2d.hpp"
#include "garage/graphics/render_subsystem.hpp"

#include "garage/utilities/logger.hpp"

namespace Garage
{
    void SkyboxRenderer::OnRender(SystemContext& context)
    {
        RenderSubsystem*   renderSubsystem = context.GetEngineSubsystem<RenderSubsystem>();
        IGPUCommandBuffer* commandBuffer =
                (renderSubsystem != nullptr) ? renderSubsystem->GetGPUCommandBuffer() : nullptr;
        if (commandBuffer == nullptr)
        {
            return;
        }

        const IGPUGraphicsPipelineState* pipelineState = GetPipelineState();
        const IGPUTexture2D*             texture       = (cubemap != nullptr) ? cubemap->GetTexture() : nullptr;
        if (pipelineState == nullptr || texture == nullptr)
        {
            return;
        }

        const SkyboxConstants CONSTANTS{
                .color = GetColor(),
        };
        if (const auto RESULT = commandBuffer->SetGraphicsConstants(CONSTANTS); !RESULT)
        {
            LOG_ERROR("스카이박스 색상 상수를 바인딩하지 못했습니다: {}", RESULT.GetError());
            return;
        }

        const Array<const IGPUTexture2D*, IGPUCommandBuffer::TEXTURE_SLOT_COUNT> TEXTURES{texture};
        if (const auto BOUND = commandBuffer->BindTextures(TEXTURES); !BOUND)
        {
            LOG_ERROR("스카이박스 텍스처를 바인딩하지 못했습니다: {}", BOUND.GetError());
            return;
        }

        ApplyPipelineState(*commandBuffer, *pipelineState);

        constexpr UInt32 FULLSCREEN_VERTEX_COUNT = 3;
        commandBuffer->Draw(FULLSCREEN_VERTEX_COUNT);
    }
} // namespace Garage
