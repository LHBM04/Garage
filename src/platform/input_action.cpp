#include "garage/platform/input_action.hpp"

#include <algorithm>
#include <cmath>
#include <string>
#include <string_view>

#include "garage/core/common.hpp"
#include "garage/core/memory.hpp"

#include "garage/math/mathf.hpp"

#include "garage/platform/input_provider.hpp"
#include "garage/platform/input_states.hpp"

namespace Garage
{
    InputAction::InputAction(std::string_view name) noexcept
        : name(name, Memory::GetMemoryResource())
    {
    }

    void InputAction::AddButtonBinding(EKeyCode key)
    {
        bindings.push_back(Binding{
                .kind       = EBindingKind::KEY_BUTTON,
                .primaryKey = key,
        });
    }

    void InputAction::AddMouseButtonBinding(EButtonCode button)
    {
        bindings.push_back(Binding{
                .kind        = EBindingKind::MOUSE_BUTTON,
                .mouseButton = button,
        });
    }

    void InputAction::AddAxis1DBinding(EKeyCode negative, EKeyCode positive)
    {
        bindings.push_back(Binding{
                .kind         = EBindingKind::AXIS_1D,
                .primaryKey   = negative,
                .secondaryKey = positive,
        });
    }

    void InputAction::AddVector2DBinding(EKeyCode up, EKeyCode down, EKeyCode left, EKeyCode right)
    {
        bindings.push_back(Binding{
                .kind         = EBindingKind::VECTOR_2D,
                .primaryKey   = up,
                .secondaryKey = down,
                .leftKey      = left,
                .rightKey     = right,
        });
    }

    void InputAction::ClearBindings() noexcept
    {
        bindings.clear();
    }

    void InputAction::Evaluate(const InputProvider& provider)
    {
        Float32 x = 0.0f;
        Float32 y = 0.0f;

        for (const Binding& binding : bindings)
        {
            switch (binding.kind)
            {
                case EBindingKind::KEY_BUTTON:
                {
                    if (provider.IsKeyHeld(binding.primaryKey))
                    {
                        x = 1.0f;
                    }
                    break;
                }
                case EBindingKind::MOUSE_BUTTON:
                {
                    if (provider.HasMouseButtonState() && provider.IsMouseButtonHeld(binding.mouseButton))
                    {
                        x = 1.0f;
                    }
                    break;
                }
                case EBindingKind::AXIS_1D:
                {
                    const Float32 AXIS = static_cast<Float32>(provider.IsKeyHeld(binding.secondaryKey)) -
                            static_cast<Float32>(provider.IsKeyHeld(binding.primaryKey));
                    x += AXIS;
                    break;
                }
                case EBindingKind::VECTOR_2D:
                {
                    const Float32 HORIZONTAL = static_cast<Float32>(provider.IsKeyHeld(binding.rightKey)) -
                            static_cast<Float32>(provider.IsKeyHeld(binding.leftKey));
                    const Float32 VERTICAL = static_cast<Float32>(provider.IsKeyHeld(binding.primaryKey)) -
                            static_cast<Float32>(provider.IsKeyHeld(binding.secondaryKey));
                    x += HORIZONTAL;
                    y += VERTICAL;
                    break;
                }
            }
        }

        x = std::clamp(x, -1.0f, 1.0f);
        y = std::clamp(y, -1.0f, 1.0f);

        const bool ACTIVE_NOW = (std::abs(x) > Mathf::EPSILON) || (std::abs(y) > Mathf::EPSILON);
        const bool WAS_ACTIVE = isActive;

        currentValue = ACTIVE_NOW ? Vector2DFloat{x, y} : Vector2DFloat{};
        isActive     = ACTIVE_NOW;

        if (!WAS_ACTIVE && ACTIVE_NOW)
        {
            onStarted.Broadcast(InputActionContext{.phase = EInputPhase::STARTED, .value = currentValue});
            onPerformed.Broadcast(InputActionContext{.phase = EInputPhase::PERFORMED, .value = currentValue});
        }
        else if (WAS_ACTIVE && ACTIVE_NOW)
        {
            onPerformed.Broadcast(InputActionContext{.phase = EInputPhase::PERFORMED, .value = currentValue});
        }
        else if (WAS_ACTIVE && !ACTIVE_NOW)
        {
            onCanceled.Broadcast(InputActionContext{.phase = EInputPhase::CANCELED, .value = Vector2DFloat{}});
        }
    }

    void InputAction::Reset() noexcept
    {
        if (isActive)
        {
            isActive     = false;
            currentValue = {};
            onCanceled.Broadcast(InputActionContext{.phase = EInputPhase::CANCELED, .value = Vector2DFloat{}});
        }
    }

    const std::pmr::string& InputAction::GetName() const noexcept
    {
        return name;
    }

    bool InputAction::IsActive() const noexcept
    {
        return isActive;
    }

    const Vector2DFloat& InputAction::GetValue() const noexcept
    {
        return currentValue;
    }

    InputAction::ActionCallback& InputAction::OnStarted() noexcept
    {
        return onStarted;
    }

    InputAction::ActionCallback& InputAction::OnPerformed() noexcept
    {
        return onPerformed;
    }

    InputAction::ActionCallback& InputAction::OnCanceled() noexcept
    {
        return onCanceled;
    }
} // namespace Garage
