#include "garage/graphics/skybox_renderer.hpp"

#include <array>

#include "garage/core/types.hpp"

#include "garage/graphics/gpu_command_buffer.hpp"
#include "garage/graphics/gpu_constants.hpp"
#include "garage/graphics/gpu_graphics_pipeline_state.hpp"
#include "garage/graphics/gpu_texture2d.hpp"

#include "garage/resources/cubemap_asset.hpp"

#include "garage/utilities/logger.hpp"

namespace Garage
{
    void SkyboxRenderer::OnRender(IGPUCommandBuffer& commandBuffer)
    {
        const IGPUGraphicsPipelineState* pipelineState = GetPipelineState();
        const IGPUTexture2D*             texture       = (cubemap != nullptr) ? cubemap->GetTexture() : nullptr;
        if (pipelineState == nullptr || texture == nullptr)
        {
            return;
        }

        const SkyboxConstants CONSTANTS{
                .color = GetColor(),
        };
        if (const auto RESULT = commandBuffer.SetGraphicsConstants(CONSTANTS); !RESULT)
        {
            LOG_ERROR("스카이박스 색상 상수를 바인딩하지 못했습니다: {}", RESULT.GetError());
            return;
        }

        const std::array<const IGPUTexture2D*, IGPUCommandBuffer::TEXTURE_SLOT_COUNT> TEXTURES{texture};
        if (const auto BOUND = commandBuffer.BindTextures(TEXTURES); !BOUND)
        {
            LOG_ERROR("스카이박스 텍스처를 바인딩하지 못했습니다: {}", BOUND.GetError());
            return;
        }

        ApplyPipelineState(commandBuffer, *pipelineState);

        constexpr UInt32 FULLSCREEN_VERTEX_COUNT = 3;
        commandBuffer.Draw(FULLSCREEN_VERTEX_COUNT);
    }
} // namespace Garage
