#ifndef GARAGE_PLATFORM_INPUT_PROVIDER_HPP
#define GARAGE_PLATFORM_INPUT_PROVIDER_HPP

#include <bitset>

#include "../core/common.hpp"

#include "../math/vector2d.hpp"

#include "input_states.hpp"

namespace Garage
{
    class InputSubsystem;

    class GARAGE_API InputProvider final
    {
        friend class InputSubsystem;

    public:
        static constexpr USize KEY_STATE_COUNT = 128;

        InputProvider() noexcept  = default;
        ~InputProvider() noexcept = default;

        InputProvider(const InputProvider&)            = delete;
        InputProvider& operator=(const InputProvider&) = delete;

        InputProvider(InputProvider&&) noexcept            = default;
        InputProvider& operator=(InputProvider&&) noexcept = default;

        [[nodiscard]] bool IsKeyPressed(EKeyCode key) const noexcept;
        [[nodiscard]] bool IsKeyHeld(EKeyCode key) const noexcept;
        [[nodiscard]] bool IsKeyReleased(EKeyCode key) const noexcept;

        [[nodiscard]] bool HasMouseButtonState() const noexcept;
        [[nodiscard]] bool IsMouseButtonPressed(EButtonCode button) const noexcept;
        [[nodiscard]] bool IsMouseButtonHeld(EButtonCode button) const noexcept;
        [[nodiscard]] bool IsMouseButtonReleased(EButtonCode button) const noexcept;

        [[nodiscard]] bool        HasMousePosition() const noexcept;
        [[nodiscard]] Vector2DInt GetMousePosition() const noexcept;
        [[nodiscard]] Vector2DInt GetMousePositionDelta() const noexcept;
        [[nodiscard]] Vector2DInt GetMouseWheelDelta() const noexcept;

    private:
        void Reset() noexcept;
        void Update() noexcept;
        void SynchronizeHistory() noexcept;

        void SetKey(EKeyCode key, bool isDown) noexcept;
        void SetMouseButton(EButtonCode button, bool isDown) noexcept;
        void SetMouseButtons(UInt32 buttons) noexcept;
        void SetMousePosition(const Vector2DInt& position) noexcept;
        void AddMousePositionDelta(const Vector2DInt& positionDelta) noexcept;
        void AddMouseWheelDelta(const Vector2DInt& wheelDelta) noexcept;

        std::bitset<KEY_STATE_COUNT> currentKeys{};
        std::bitset<KEY_STATE_COUNT> previousKeys{};

        UInt32 currentButtons      = 0;
        UInt32 previousButtons     = 0;
        bool   hasMouseButtonState = false;

        bool        hasMousePosition = false;
        Vector2DInt mousePosition{};
        Vector2DInt mousePositionDelta{};
        Vector2DInt mouseWheelDelta{};
    };
} // namespace Garage

#endif // GARAGE_PLATFORM_INPUT_PROVIDER_HPP
