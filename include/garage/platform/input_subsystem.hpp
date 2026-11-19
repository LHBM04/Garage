#ifndef GARAGE_PLATFORM_INPUT_SUBSYSTEM_HPP
#define GARAGE_PLATFORM_INPUT_SUBSYSTEM_HPP

#include "../core/app_subsystem.hpp"
#include "../core/common.hpp"
#include "../core/common/containers.hpp"
#include "../core/memory.hpp"
#include "../core/system_status.hpp"

#include "../math/vector2d.hpp"

#include "display_events.hpp"
#include "input_action.hpp"
#include "input_events.hpp"
#include "input_provider.hpp"

namespace Garage
{
    class IDisplay;
    class SystemContext;

    class GARAGE_API InputSubsystem final : public AppSubsystemBase
    {
    public:
        InputSubsystem() noexcept;

        [[nodiscard]] ESystemStatus OnStartup(SystemContext& context) override;
        [[nodiscard]] ESystemStatus OnEarlyUpdate(SystemContext& context) override;
        [[nodiscard]] ESystemStatus OnFixedUpdate(SystemContext& context) override;
        [[nodiscard]] ESystemStatus OnUpdate(SystemContext& context) override;
        [[nodiscard]] ESystemStatus OnLateUpdate(SystemContext& context) override;
        void                        OnShutdown(SystemContext& context) override;

        [[nodiscard]] bool IsInputEnabled() const noexcept;
        void               SetInputEnabled(bool value) noexcept;

        InputAction*                     AddAction(StringView name);
        void                             RemoveAction(StringView name) noexcept;
        [[nodiscard]] InputAction*       GetAction(StringView name) noexcept;
        [[nodiscard]] const InputAction* GetAction(StringView name) const noexcept;

        void OnDisplayCreate(const DisplayCreateEvent& event);
        void OnDisplayClose(const DisplayCloseEvent& event);
        void OnDisplayFocus(const DisplayFocusEvent& event);
        void OnInputActivation(const InputActivationEvent& event);
        void OnMouseWheel(const MouseWheelEvent& event) noexcept;

        [[nodiscard]] const IDisplay*      GetActiveDisplay() const noexcept;
        [[nodiscard]] const InputProvider* GetProvider() const noexcept;

    private:
        void PollKeyboard(InputProvider& targetProvider) noexcept;
        void PollMouse(InputProvider& targetProvider) noexcept;
        void ResetInputState() noexcept;

        const IDisplay*                              activeDisplay = nullptr;
        InputProvider                                provider{};
        UnorderedMap<String, UniquePtr<InputAction>> actions;

        Delegate<const DisplayCreateEvent&>::ListenerId   displayCreateListenerId   = 0;
        Delegate<const DisplayCloseEvent&>::ListenerId    displayCloseListenerId    = 0;
        Delegate<const DisplayFocusEvent&>::ListenerId    displayFocusListenerId    = 0;
        Delegate<const InputActivationEvent&>::ListenerId inputActivationListenerId = 0;
        Delegate<const MouseWheelEvent&>::ListenerId      mouseWheelListenerId      = 0;

        bool        isInputEnabled    = true;
        bool        needsInputSync    = false;
        bool        hasMouseHistory   = false;
        Vector2DInt pendingWheelDelta = {};
    };

    inline InputSubsystem::InputSubsystem() noexcept
        : AppSubsystemBase(0)
    {
    }

    inline bool InputSubsystem::IsInputEnabled() const noexcept
    {
        return isInputEnabled;
    }

    inline const IDisplay* InputSubsystem::GetActiveDisplay() const noexcept
    {
        return activeDisplay;
    }

    inline const InputProvider* InputSubsystem::GetProvider() const noexcept
    {
        return (activeDisplay != nullptr) ? &provider : nullptr;
    }
} // namespace Garage

#endif // GARAGE_PLATFORM_INPUT_SUBSYSTEM_HPP
