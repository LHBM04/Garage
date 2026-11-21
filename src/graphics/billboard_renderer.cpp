#include "garage/graphics/billboard_renderer.hpp"

#include <algorithm>
#include <cmath>

#include "garage/actors/actor.hpp"

#include "garage/content/texture_content.hpp"

#include "garage/core/common.hpp"
#include "garage/core/common/containers.hpp"
#include "garage/core/system_context.hpp"

#include "garage/graphics/gpu_command_buffer.hpp"
#include "garage/graphics/gpu_constants.hpp"
#include "garage/graphics/gpu_graphics_pipeline_state.hpp"
#include "garage/graphics/gpu_texture2d.hpp"
#include "garage/graphics/render_subsystem.hpp"

#include "garage/math/vector3d.hpp"

#include "garage/platform/time_subsystem.hpp"

#include "garage/utilities/logger.hpp"

namespace Garage
{
    void BillboardRenderer::Play(
            Float32 newDuration, UInt32 newColumns, UInt32 newRows, bool isLoop, bool isAutoDestroy) noexcept
    {
        duration              = std::max(0.0f, newDuration);
        columns               = std::max(1u, newColumns);
        rows                  = std::max(1u, newRows);
        isLooping             = isLoop;
        isAutoDestroyOnFinish = isAutoDestroy;
        elapsedTime           = 0.0f;
        frameIndex            = 0;
        isPlaying             = (duration > 0.0f);
    }

    void BillboardRenderer::Stop() noexcept
    {
        isPlaying = false;
    }

    void BillboardRenderer::OnUpdate(SystemContext& context)
    {
        if (!isPlaying || duration <= 0.0f)
        {
            return;
        }

        const TimeSubsystem* timeSubsystem = context.GetEngineSubsystem<TimeSubsystem>();
        const Float32        DELTA_TIME = (timeSubsystem != nullptr) ? timeSubsystem->GetDeltaTime() : (1.0f / 60.0f);

        elapsedTime += DELTA_TIME;
        const UInt32 TOTAL_FRAMES = columns * rows;
        if (TOTAL_FRAMES == 0)
        {
            return;
        }

        if (elapsedTime >= duration)
        {
            if (isLooping)
            {
                elapsedTime            = std::fmod(elapsedTime, duration);
                const Float32 PROGRESS = elapsedTime / duration;
                frameIndex =
                        std::min(static_cast<UInt32>(PROGRESS * static_cast<Float32>(TOTAL_FRAMES)), TOTAL_FRAMES - 1);
            }
            else
            {
                isPlaying  = false;
                frameIndex = TOTAL_FRAMES - 1;
                if (isAutoDestroyOnFinish)
                {
                    if (ActorBase* owner = GetOwner(); owner != nullptr)
                    {
                        owner->Destroy();
                    }
                }
            }
        }
        else
        {
            const Float32 PROGRESS = std::clamp(elapsedTime / duration, 0.0f, 1.0f);
            frameIndex = std::min(static_cast<UInt32>(PROGRESS * static_cast<Float32>(TOTAL_FRAMES)), TOTAL_FRAMES - 1);
        }
    }

    void BillboardRenderer::OnRender(SystemContext& context)
    {
        RenderSubsystem*   renderSubsystem = context.GetEngineSubsystem<RenderSubsystem>();
        IGPUCommandBuffer* commandBuffer =
                (renderSubsystem != nullptr) ? renderSubsystem->GetGPUCommandBuffer() : nullptr;
        if (commandBuffer == nullptr)
        {
            return;
        }

        const IGPUGraphicsPipelineState* pipelineState = GetPipelineState();
        const IGPUTexture2D*             gpuTexture    = (texture != nullptr) ? texture->GetTexture() : nullptr;
        if (pipelineState == nullptr || gpuTexture == nullptr)
        {
            return;
        }

        if (!BindObjectConstants(*commandBuffer, GetWorldMatrix()))
        {
            return;
        }

        const BillboardConstants CONSTANTS{
                .size       = size,
                .frameIndex = frameIndex,
                .columns    = columns,
                .rows       = rows,
                .padding    = Vector3D<Float32>{},
                .color      = GetColor(),
        };

        if (const auto RESULT = commandBuffer->SetGraphicsConstants(CONSTANTS); !RESULT)
        {
            LOG_ERROR("빌보드 상수를 바인딩하지 못했습니다: {}", RESULT.GetError());
            return;
        }

        Array<const IGPUTexture2D*, IGPUCommandBuffer::TEXTURE_SLOT_COUNT> textures{};
        textures[0] = gpuTexture;
        if (const auto RESULT = commandBuffer->BindTextures(textures); !RESULT)
        {
            LOG_ERROR("빌보드 텍스처를 바인딩하지 못했습니다: {}", RESULT.GetError());
            return;
        }

        ApplyPipelineState(*commandBuffer, *pipelineState);

        constexpr UInt32 QUAD_VERTEX_COUNT = 4;
        commandBuffer->Draw(QUAD_VERTEX_COUNT);
    }
} // namespace Garage
