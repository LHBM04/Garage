#ifndef GARAGE_PLATFORM_INPUT_SUBSYSTEM_HPP
#define GARAGE_PLATFORM_INPUT_SUBSYSTEM_HPP

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

#include "../core/app_subsystem.hpp"
#include "../core/defines.hpp"
#include "../core/system_status.hpp"
#include "../core/types.hpp"

#include "../math/vector2d.hpp"

#include "display_events.hpp"
#include "input_action.hpp"
#include "input_events.hpp"
#include "input_provider.hpp"

namespace Garage
{
    class IDisplay;

    class GARAGE_API InputSubsystem final : public AppSubsystemBase
    {
    public:
        InputSubsystem() noexcept;

        [[nodiscard]] ESystemStatus OnStartup() override;
        [[nodiscard]] ESystemStatus OnEarlyUpdate() override;
        [[nodiscard]] ESystemStatus OnFixedUpdate() override;
        [[nodiscard]] ESystemStatus OnUpdate() override;
        [[nodiscard]] ESystemStatus OnLateUpdate() override;
        void                        OnShutdown() override;

        [[nodiscard]] bool IsInputEnabled() const noexcept;
        void               SetInputEnabled(bool value) noexcept;

        InputAction*                     AddAction(std::string_view name);
        void                             RemoveAction(std::string_view name) noexcept;
        [[nodiscard]] InputAction*       GetAction(std::string_view name) noexcept;
        [[nodiscard]] const InputAction* GetAction(std::string_view name) const noexcept;

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

        const IDisplay*                                               activeDisplay = nullptr;
        InputProvider                                                 provider{};
        std::unordered_map<std::string, std::unique_ptr<InputAction>> actions = {};

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
