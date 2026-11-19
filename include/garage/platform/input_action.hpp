#ifndef GARAGE_PLATFORM_INPUT_ACTION_HPP
#define GARAGE_PLATFORM_INPUT_ACTION_HPP

#include <type_traits>

#include "../core/common.hpp"
#include "../core/common/containers.hpp"

#include "../math/vector2d.hpp"

#include "input_states.hpp"

namespace Garage
{
    class InputProvider;

    enum class EInputPhase : UInt8
    {
        STARTED,
        PERFORMED,
        CANCELED,
    };

    struct GARAGE_API InputActionContext final
    {
        EInputPhase   phase = EInputPhase::STARTED;
        Vector2DFloat value{};
    };

    class GARAGE_API InputAction final
    {
    public:
        using ActionCallback = Delegate<const InputActionContext&>;

    private:
        enum class EBindingKind : UInt8
        {
            KEY_BUTTON,
            MOUSE_BUTTON,
            AXIS_1D,
            VECTOR_2D,
        };

        struct Binding final
        {
            EBindingKind kind         = EBindingKind::KEY_BUTTON;
            EKeyCode     primaryKey   = EKeyCode::NONE;
            EKeyCode     secondaryKey = EKeyCode::NONE;
            EKeyCode     leftKey      = EKeyCode::NONE;
            EKeyCode     rightKey     = EKeyCode::NONE;
            EButtonCode  mouseButton  = EButtonCode::NONE;
        };

    public:
        explicit InputAction(StringView name) noexcept;
        ~InputAction() noexcept = default;

        InputAction(const InputAction&)            = delete;
        InputAction& operator=(const InputAction&) = delete;

        InputAction(InputAction&&) noexcept                                                                = default;
        InputAction& operator=(InputAction&&) noexcept(std::is_nothrow_move_assignable_v<String> &&
                                                       std::is_nothrow_move_assignable_v<Vector<Binding>>) = default;

        void AddButtonBinding(EKeyCode key);
        void AddMouseButtonBinding(EButtonCode button);
        void AddAxis1DBinding(EKeyCode negative, EKeyCode positive);
        void AddVector2DBinding(EKeyCode up, EKeyCode down, EKeyCode left, EKeyCode right);
        void ClearBindings() noexcept;

        void Evaluate(const InputProvider& provider);
        void Reset() noexcept;

        [[nodiscard]] const String&        GetName() const noexcept;
        [[nodiscard]] bool                 IsActive() const noexcept;
        [[nodiscard]] const Vector2DFloat& GetValue() const noexcept;

        [[nodiscard]] ActionCallback& OnStarted() noexcept;
        [[nodiscard]] ActionCallback& OnPerformed() noexcept;
        [[nodiscard]] ActionCallback& OnCanceled() noexcept;

    private:
        String          name;
        Vector<Binding> bindings;
        Vector2DFloat   currentValue = {};
        bool            isActive     = false;

        ActionCallback onStarted   = {};
        ActionCallback onPerformed = {};
        ActionCallback onCanceled  = {};
    };
} // namespace Garage

#endif // GARAGE_PLATFORM_INPUT_ACTION_HPP
