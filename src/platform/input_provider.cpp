#include "garage/platform/input_provider.hpp"

#include "garage/core/common.hpp"

#include "garage/math/vector2d.hpp"

#include "garage/platform/input_states.hpp"

namespace Garage
{
    bool InputProvider::IsKeyPressed(EKeyCode key) const noexcept
    {
        const USize INDEX = static_cast<USize>(key);
        return INDEX < currentKeys.size() && currentKeys[INDEX] && !previousKeys[INDEX];
    }

    bool InputProvider::IsKeyHeld(EKeyCode key) const noexcept
    {
        const USize INDEX = static_cast<USize>(key);
        return INDEX < currentKeys.size() && currentKeys[INDEX];
    }

    bool InputProvider::IsKeyReleased(EKeyCode key) const noexcept
    {
        const USize INDEX = static_cast<USize>(key);
        return INDEX < currentKeys.size() && !currentKeys[INDEX] && previousKeys[INDEX];
    }

    bool InputProvider::HasMouseButtonState() const noexcept
    {
        return hasMouseButtonState;
    }

    bool InputProvider::IsMouseButtonPressed(EButtonCode button) const noexcept
    {
        const UInt32 MASK = static_cast<UInt32>(button);
        return (currentButtons & MASK) != 0 && (previousButtons & MASK) == 0;
    }

    bool InputProvider::IsMouseButtonHeld(EButtonCode button) const noexcept
    {
        const UInt32 MASK = static_cast<UInt32>(button);
        return (currentButtons & MASK) != 0;
    }

    bool InputProvider::IsMouseButtonReleased(EButtonCode button) const noexcept
    {
        const UInt32 MASK = static_cast<UInt32>(button);
        return (currentButtons & MASK) == 0 && (previousButtons & MASK) != 0;
    }

    bool InputProvider::HasMousePosition() const noexcept
    {
        return hasMousePosition;
    }

    Vector2DInt InputProvider::GetMousePosition() const noexcept
    {
        return mousePosition;
    }

    Vector2DInt InputProvider::GetMousePositionDelta() const noexcept
    {
        return mousePositionDelta;
    }

    Vector2DInt InputProvider::GetMouseWheelDelta() const noexcept
    {
        return mouseWheelDelta;
    }

    void InputProvider::Reset() noexcept
    {
        currentKeys.reset();
        previousKeys.reset();

        currentButtons      = 0;
        previousButtons     = 0;
        hasMouseButtonState = false;

        hasMousePosition   = false;
        mousePosition      = {};
        mousePositionDelta = {};
        mouseWheelDelta    = {};
    }

    void InputProvider::Update() noexcept
    {
        previousKeys    = currentKeys;
        previousButtons = currentButtons;

        currentKeys.reset();
        currentButtons      = 0;
        hasMouseButtonState = false;
        hasMousePosition    = false;

        mousePositionDelta = {};
        mouseWheelDelta    = {};
    }

    void InputProvider::SynchronizeHistory() noexcept
    {
        previousKeys       = currentKeys;
        previousButtons    = currentButtons;
        mousePositionDelta = {};
        mouseWheelDelta    = {};
    }

    void InputProvider::SetKey(EKeyCode key, bool isDown) noexcept
    {
        const USize INDEX = static_cast<USize>(key);
        if (INDEX < currentKeys.size())
        {
            currentKeys.set(INDEX, isDown);
        }
    }

    void InputProvider::SetMouseButton(EButtonCode button, bool isDown) noexcept
    {
        hasMouseButtonState = true;
        const UInt32 MASK   = static_cast<UInt32>(button);
        if (isDown)
        {
            currentButtons |= MASK;
        }
        else
        {
            currentButtons &= ~MASK;
        }
    }

    void InputProvider::SetMouseButtons(UInt32 buttons) noexcept
    {
        currentButtons      = buttons;
        hasMouseButtonState = true;
    }

    void InputProvider::SetMousePosition(const Vector2DInt& position) noexcept
    {
        mousePosition    = position;
        hasMousePosition = true;
    }

    void InputProvider::AddMousePositionDelta(const Vector2DInt& positionDelta) noexcept
    {
        mousePositionDelta += positionDelta;
    }

    void InputProvider::AddMouseWheelDelta(const Vector2DInt& wheelDelta) noexcept
    {
        mouseWheelDelta += wheelDelta;
    }
} // namespace Garage
