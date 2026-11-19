#include "garage/ui/image_view.hpp"

#include <array>

#include "garage/actors/actor.hpp"
#include "garage/actors/transform.hpp"

#include "garage/core/system_context.hpp"
#include "garage/core/types.hpp"

#include "garage/graphics/gpu_command_buffer.hpp"
#include "garage/graphics/gpu_constants.hpp"
#include "garage/graphics/gpu_graphics_pipeline_state.hpp"
#include "garage/graphics/gpu_texture2d.hpp"
#include "garage/graphics/gpu_types.hpp"
#include "garage/graphics/render_subsystem.hpp"

#include "garage/math/vector2d.hpp"

#include "garage/resources/material_asset.hpp"
#include "garage/resources/texture_asset.hpp"

#include "garage/utilities/logger.hpp"

namespace Garage
{
    void ImageView::OnPostRender(SystemContext& context)
    {
        RenderSubsystem* renderSubsystem = context.GetEngineSubsystem<RenderSubsystem>();
        IGPUCommandBuffer* commandBuffer =
                (renderSubsystem != nullptr) ? renderSubsystem->GetGPUCommandBuffer() : nullptr;
        if (commandBuffer == nullptr || texture == nullptr || material == nullptr)
        {
            return;
        }

        const IGPUTexture2D*             gpuTexture    = texture->GetTexture();
        const IGPUGraphicsPipelineState* pipelineState = material->GetGraphicsPipelineState();
        if (gpuTexture == nullptr || pipelineState == nullptr)
        {
            return;
        }

        const ActorBase* owner     = GetOwner();
        const Transform* transform = (owner != nullptr) ? owner->GetComponent<Transform>() : nullptr;
        if (transform == nullptr)
        {
            LOG_ERROR("UI 이미지 액터에 Transform 컴포넌트가 필요합니다.");
            return;
        }

        const ObjectConstants OBJECT_CONSTANTS{
                .world = transform->GetWorldMatrix(),
        };
        if (const auto RESULT = commandBuffer->SetGraphicsConstants(OBJECT_CONSTANTS); !RESULT)
        {
            LOG_ERROR("UI 이미지 변환을 바인딩하지 못했습니다: {}", RESULT.GetError());
            return;
        }

        const GPUViewport&  VIEWPORT      = commandBuffer->GetActiveViewport();
        const Vector2DFloat VIEWPORT_SIZE = (VIEWPORT.width > 0.0f && VIEWPORT.height > 0.0f)
                ? Vector2DFloat{VIEWPORT.width, VIEWPORT.height}
                : Vector2DFloat{1280.0f, 960.0f};

        const ImageUIConstants CONSTANTS{
                .viewportSize = VIEWPORT_SIZE,
                .padding      = Vector2DFloat{},
                .color        = color,
        };
        if (const auto RESULT = commandBuffer->SetGraphicsConstants(CONSTANTS); !RESULT)
        {
            LOG_ERROR("UI 뷰포트 상수를 바인딩하지 못했습니다: {}", RESULT.GetError());
            return;
        }

        std::array<const IGPUTexture2D*, IGPUCommandBuffer::TEXTURE_SLOT_COUNT> textures{};
        textures[0] = gpuTexture;
        if (const auto RESULT = commandBuffer->BindTextures(textures); !RESULT)
        {
            LOG_ERROR("UI 이미지 텍스처를 바인딩하지 못했습니다: {}", RESULT.GetError());
            return;
        }

        commandBuffer->SetGPUGraphicsPipelineState(*pipelineState);
        commandBuffer->SetPrimitiveTopology(material->GetPrimitive());
        constexpr UInt32 IMAGE_VERTEX_COUNT = 4;
        commandBuffer->Draw(IMAGE_VERTEX_COUNT);
    }
} // namespace Garage
