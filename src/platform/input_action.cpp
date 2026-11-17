#include "garage/platform/input_action.hpp"

#include <algorithm>
#include <cmath>

#include "garage/math/mathf.hpp"

#include "garage/platform/input_provider.hpp"

namespace Garage
{
    InputAction::InputAction(std::string_view name) noexcept
        : name(name)
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
                    const Float32 axis = static_cast<Float32>(provider.IsKeyHeld(binding.secondaryKey)) -
                            static_cast<Float32>(provider.IsKeyHeld(binding.primaryKey));
                    x += axis;
                    break;
                }
                case EBindingKind::VECTOR_2D:
                {
                    const Float32 horizontal = static_cast<Float32>(provider.IsKeyHeld(binding.rightKey)) -
                            static_cast<Float32>(provider.IsKeyHeld(binding.leftKey));
                    const Float32 vertical = static_cast<Float32>(provider.IsKeyHeld(binding.primaryKey)) -
                            static_cast<Float32>(provider.IsKeyHeld(binding.secondaryKey));
                    x += horizontal;
                    y += vertical;
                    break;
                }
            }
        }

        x = std::clamp(x, -1.0f, 1.0f);
        y = std::clamp(y, -1.0f, 1.0f);

        const bool activeNow = (std::abs(x) > Mathf::EPSILON) || (std::abs(y) > Mathf::EPSILON);
        const bool wasActive = isActive;

        currentValue = activeNow ? Vector2DFloat{x, y} : Vector2DFloat{};
        isActive     = activeNow;

        if (!wasActive && activeNow)
        {
            onStarted.Broadcast(InputActionContext{EInputPhase::STARTED, currentValue});
            onPerformed.Broadcast(InputActionContext{EInputPhase::PERFORMED, currentValue});
        }
        else if (wasActive && activeNow)
        {
            onPerformed.Broadcast(InputActionContext{EInputPhase::PERFORMED, currentValue});
        }
        else if (wasActive && !activeNow)
        {
            onCanceled.Broadcast(InputActionContext{EInputPhase::CANCELED, Vector2DFloat{}});
        }
    }

    void InputAction::Reset() noexcept
    {
        if (isActive)
        {
            isActive     = false;
            currentValue = {};
            onCanceled.Broadcast(InputActionContext{EInputPhase::CANCELED, Vector2DFloat{}});
        }
    }

    const std::string& InputAction::GetName() const noexcept
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
