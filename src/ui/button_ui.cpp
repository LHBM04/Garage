#include "garage/ui/button_ui.hpp"

#include <cmath>
#include <ranges>
#include <utility>

#include "garage/actors/actor.hpp"
#include "garage/actors/transform.hpp"
#include "garage/actors/world.hpp"

#include "garage/core/system_context.hpp"
#include "garage/core/types.hpp"

#include "garage/graphics/camera.hpp"
#if defined(GARAGE_DEBUG) || defined(_DEBUG)
#    include "garage/graphics/gpu_command_buffer.hpp"
#    include "garage/graphics/gpu_constants.hpp"
#    include "garage/graphics/gpu_graphics_pipeline_state.hpp"
#endif
#include "garage/graphics/gpu_types.hpp"
#if defined(GARAGE_DEBUG) || defined(_DEBUG)
#    include "garage/graphics/render_subsystem.hpp"
#endif

#if defined(GARAGE_DEBUG) || defined(_DEBUG)
#    include "garage/math/color_rgba.hpp"
#endif
#include "garage/math/matrix4x4.hpp"
#include "garage/math/vector2d.hpp"

#include "garage/platform/display.hpp"
#include "garage/platform/input_provider.hpp"
#include "garage/platform/input_states.hpp"
#include "garage/platform/input_subsystem.hpp"

#if defined(GARAGE_DEBUG) || defined(_DEBUG)
#    include "garage/resources/material_asset.hpp"
#endif

namespace Garage
{
    namespace
    {
        [[nodiscard]] const Camera* FindUICamera(const WorldBase* world) noexcept
        {
            if (world == nullptr)
            {
                return nullptr;
            }

            for (const Camera* camera : world->GetCameras() | std::views::reverse)
            {
                if (camera == nullptr)
                {
                    continue;
                }

                const ActorBase* owner = camera->GetOwner();
                if (owner != nullptr && camera->IsEnabled() && !camera->IsDestroyed() && owner->IsEnabled() &&
                    !owner->IsDestroyed())
                {
                    return camera;
                }
            }

            return nullptr;
        }

        [[nodiscard]] bool HitTestButton(const Transform*       transform,
                                         const Camera*          camera,
                                         const Vector2D<Int32>& cursor,
                                         const Vector2D<Int32>& clientSize) noexcept
        {
            const GPUViewport& VIEWPORT = camera->GetViewport();
            const GPURect&     SCISSOR  = camera->GetScissorRect();
            const Int32        CLIENT_X = cursor.GetX();
            const Int32        CLIENT_Y = cursor.GetY();
            if (CLIENT_X < 0 || CLIENT_Y < 0 || CLIENT_X >= clientSize.GetX() || CLIENT_Y >= clientSize.GetY() ||
                CLIENT_X < SCISSOR.x || CLIENT_Y < SCISSOR.y || CLIENT_X >= SCISSOR.x + SCISSOR.width ||
                CLIENT_Y >= SCISSOR.y + SCISSOR.height)
            {
                return false;
            }

            const Float32 X = static_cast<Float32>(CLIENT_X) - VIEWPORT.x;
            const Float32 Y = static_cast<Float32>(CLIENT_Y) - VIEWPORT.y;
            if (X < 0.0f || Y < 0.0f || X >= VIEWPORT.width || Y >= VIEWPORT.height)
            {
                return false;
            }

            const Matrix4x4<Float32>& WORLD           = transform->GetWorldMatrix();
            const Float32             DETERMINANT     = WORLD[0] * WORLD[5] - WORLD[1] * WORLD[4];
            constexpr Float32         MIN_DETERMINANT = 1.0e-6f;
            if (!std::isfinite(DETERMINANT) || std::abs(DETERMINANT) <= MIN_DETERMINANT)
            {
                return false;
            }

            const Float32 OFFSET_X = X - VIEWPORT.width * 0.5f - WORLD[12];
            const Float32 OFFSET_Y = Y - VIEWPORT.height * 0.5f - WORLD[13];
            const Float32 LOCAL_X  = (OFFSET_X * WORLD[5] - OFFSET_Y * WORLD[4]) / DETERMINANT;
            const Float32 LOCAL_Y  = (WORLD[0] * OFFSET_Y - WORLD[1] * OFFSET_X) / DETERMINANT;
            return std::abs(LOCAL_X) <= 0.5f && std::abs(LOCAL_Y) <= 0.5f;
        }
    } // namespace

    void ButtonUI::OnUpdate(SystemContext& context)
    {
        const ActorBase* owner = GetOwner();
        const WorldBase* world = (owner != nullptr) ? owner->GetWorld() : nullptr;
        if (owner == nullptr || world == nullptr)
        {
            return;
        }

        const auto IS_ACTIVE = [this, owner, world]()
        {
            return IsEnabled() && !IsDestroyed() && owner->IsEnabled() && !owner->IsDestroyed() && world->IsEntered();
        };

        const InputSubsystem* input     = context.GetAppSubsystem<InputSubsystem>();
        const InputProvider*  provider  = (input != nullptr) ? input->GetProvider() : nullptr;
        const IDisplay*       display   = (input != nullptr) ? input->GetActiveDisplay() : nullptr;
        const Transform*      transform = owner->GetComponent<Transform>();
        const Camera*         camera    = FindUICamera(world);
        if (provider == nullptr || !provider->HasMousePosition() || display == nullptr || transform == nullptr ||
            camera == nullptr)
        {
            isPressed = false;
            if (std::exchange(isHovered, false))
            {
                onExit.Broadcast();
            }
            return;
        }

        const bool HOVERED_NOW =
                HitTestButton(transform, camera, provider->GetMousePosition(), display->GetClientSize());
        const bool HAS_BUTTON_STATE = provider->HasMouseButtonState();
        const bool PRESSED_NOW      = HAS_BUTTON_STATE && provider->IsMouseButtonPressed(EButtonCode::LEFT);
        const bool RELEASED_NOW     = HAS_BUTTON_STATE && provider->IsMouseButtonReleased(EButtonCode::LEFT);
        const bool HELD_NOW         = HAS_BUTTON_STATE && provider->IsMouseButtonHeld(EButtonCode::LEFT);
        if (std::exchange(isHovered, HOVERED_NOW) != HOVERED_NOW)
        {
            (HOVERED_NOW ? onEnter : onExit).Broadcast();
            if (!IS_ACTIVE())
            {
                return;
            }
        }

        if (PRESSED_NOW && isHovered)
        {
            isPressed = true;
            onDown.Broadcast();
        }
        else if (isPressed && !HELD_NOW)
        {
            isPressed = false;
            if (RELEASED_NOW)
            {
                onUp.Broadcast();
                if (isHovered && IS_ACTIVE())
                {
                    onClick.Broadcast();
                }
            }
        }
    }

    void ButtonUI::OnDisable()
    {
        isPressed = false;
        if (std::exchange(isHovered, false) && !IsDestroyed())
        {
            onExit.Broadcast();
        }
    }

#if defined(GARAGE_DEBUG) || defined(_DEBUG)
    void ButtonUI::OnPostRender(SystemContext& context)
    {
        RenderSubsystem* renderSubsystem = context.GetEngineSubsystem<RenderSubsystem>();
        IGPUCommandBuffer* commandBuffer =
                (renderSubsystem != nullptr) ? renderSubsystem->GetGPUCommandBuffer() : nullptr;
        if (commandBuffer == nullptr || debugMaterial == nullptr)
        {
            return;
        }

        const IGPUGraphicsPipelineState* pipelineState = debugMaterial->GetGraphicsPipelineState();
        if (pipelineState == nullptr)
        {
            return;
        }

        const ActorBase* owner     = GetOwner();
        const Transform* transform = (owner != nullptr) ? owner->GetComponent<Transform>() : nullptr;
        if (transform == nullptr)
        {
            return;
        }

        const ObjectConstants OBJECT_CONSTANTS{
                .world = transform->GetWorldMatrix(),
        };
        if (const auto RESULT = commandBuffer->SetGraphicsConstants(OBJECT_CONSTANTS); !RESULT)
        {
            return;
        }

        const GPUViewport&  VIEWPORT      = commandBuffer->GetActiveViewport();
        const Vector2DFloat VIEWPORT_SIZE = (VIEWPORT.width > 0.0f && VIEWPORT.height > 0.0f)
                ? Vector2DFloat{VIEWPORT.width, VIEWPORT.height}
                : Vector2DFloat{1280.0f, 960.0f};

        const ColorRGBA<Float32> DEBUG_COLOR = isPressed
                ? ColorRGBA<Float32>::GetRed()
                : (isHovered ? ColorRGBA<Float32>::GetYellow() : ColorRGBA<Float32>::GetGreen());

        const ImageUIConstants CONSTANTS{
                .viewportSize = VIEWPORT_SIZE,
                .padding      = Vector2DFloat{},
                .color        = DEBUG_COLOR,
        };
        if (const auto RESULT = commandBuffer->SetGraphicsConstants(CONSTANTS); !RESULT)
        {
            return;
        }

        commandBuffer->SetGPUGraphicsPipelineState(*pipelineState);
        commandBuffer->SetPrimitiveTopology(debugMaterial->GetPrimitive());
        constexpr UInt32 DEBUG_BORDER_VERTEX_COUNT = 5;
        commandBuffer->Draw(DEBUG_BORDER_VERTEX_COUNT);
    }
#endif
} // namespace Garage
