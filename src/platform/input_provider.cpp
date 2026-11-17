#include "garage/platform/input_provider.hpp"

namespace Garage
{
    bool InputProvider::IsKeyPressed(EKeyCode key) const noexcept
    {
        const USize index = static_cast<USize>(key);
        return index < currentKeys.size() && currentKeys[index] && !previousKeys[index];
    }

    bool InputProvider::IsKeyHeld(EKeyCode key) const noexcept
    {
        const USize index = static_cast<USize>(key);
        return index < currentKeys.size() && currentKeys[index];
    }

    bool InputProvider::IsKeyReleased(EKeyCode key) const noexcept
    {
        const USize index = static_cast<USize>(key);
        return index < currentKeys.size() && !currentKeys[index] && previousKeys[index];
    }

    bool InputProvider::HasMouseButtonState() const noexcept
    {
        return hasMouseButtonState;
    }

    bool InputProvider::IsMouseButtonPressed(EButtonCode button) const noexcept
    {
        const UInt32 mask = static_cast<UInt32>(button);
        return (currentButtons & mask) != 0 && (previousButtons & mask) == 0;
    }

    bool InputProvider::IsMouseButtonHeld(EButtonCode button) const noexcept
    {
        const UInt32 mask = static_cast<UInt32>(button);
        return (currentButtons & mask) != 0;
    }

    bool InputProvider::IsMouseButtonReleased(EButtonCode button) const noexcept
    {
        const UInt32 mask = static_cast<UInt32>(button);
        return (currentButtons & mask) == 0 && (previousButtons & mask) != 0;
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
        const USize index = static_cast<USize>(key);
        if (index < currentKeys.size())
        {
            currentKeys.set(index, isDown);
        }
    }

    void InputProvider::SetMouseButton(EButtonCode button, bool isDown) noexcept
    {
        hasMouseButtonState = true;
        const UInt32 mask   = static_cast<UInt32>(button);
        if (isDown)
        {
            currentButtons |= mask;
        }
        else
        {
            currentButtons &= ~mask;
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
