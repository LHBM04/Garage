#include "garage/platform/input_subsystem.hpp"

#include <array>
#include <memory>
#include <ranges>
#include <string_view>
#include <string>

#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_scancode.h>

#include "garage/core/defines.hpp"
#include "garage/core/engine.hpp"
#include "garage/core/system_status.hpp"
#include "garage/core/types.hpp"

#include "garage/platform/display.hpp"
#include "garage/platform/display_events.hpp"
#include "garage/platform/event_subsystem.hpp"
#include "garage/platform/input_events.hpp"
#include "garage/platform/input_states.hpp"
#include "platform/display_impl.hpp"

namespace Garage
{
    namespace
    {
        struct KeyMapping final
        {
            EKeyCode     key      = EKeyCode::NONE;
            SDL_Scancode scancode = SDL_SCANCODE_UNKNOWN;
        };

        constexpr std::array KEY_MAPPINGS = {
                KeyMapping{.key = EKeyCode::BACKSPACE, .scancode = SDL_SCANCODE_BACKSPACE},
                KeyMapping{.key = EKeyCode::TAB, .scancode = SDL_SCANCODE_TAB},
                KeyMapping{.key = EKeyCode::ENTER, .scancode = SDL_SCANCODE_RETURN},
                KeyMapping{.key = EKeyCode::PAUSE, .scancode = SDL_SCANCODE_PAUSE},
                KeyMapping{.key = EKeyCode::CAPS_LOCK, .scancode = SDL_SCANCODE_CAPSLOCK},
                KeyMapping{.key = EKeyCode::ESCAPE, .scancode = SDL_SCANCODE_ESCAPE},
                KeyMapping{.key = EKeyCode::SPACE, .scancode = SDL_SCANCODE_SPACE},
                KeyMapping{.key = EKeyCode::PAGE_UP, .scancode = SDL_SCANCODE_PAGEUP},
                KeyMapping{.key = EKeyCode::PAGE_DOWN, .scancode = SDL_SCANCODE_PAGEDOWN},
                KeyMapping{.key = EKeyCode::END, .scancode = SDL_SCANCODE_END},
                KeyMapping{.key = EKeyCode::HOME, .scancode = SDL_SCANCODE_HOME},
                KeyMapping{.key = EKeyCode::LEFT, .scancode = SDL_SCANCODE_LEFT},
                KeyMapping{.key = EKeyCode::UP, .scancode = SDL_SCANCODE_UP},
                KeyMapping{.key = EKeyCode::RIGHT, .scancode = SDL_SCANCODE_RIGHT},
                KeyMapping{.key = EKeyCode::DOWN, .scancode = SDL_SCANCODE_DOWN},
                KeyMapping{.key = EKeyCode::PRINT_SCREEN, .scancode = SDL_SCANCODE_PRINTSCREEN},
                KeyMapping{.key = EKeyCode::INSERT, .scancode = SDL_SCANCODE_INSERT},
                KeyMapping{.key = EKeyCode::DELETE_KEY, .scancode = SDL_SCANCODE_DELETE},

                KeyMapping{.key = EKeyCode::D0, .scancode = SDL_SCANCODE_0},
                KeyMapping{.key = EKeyCode::D1, .scancode = SDL_SCANCODE_1},
                KeyMapping{.key = EKeyCode::D2, .scancode = SDL_SCANCODE_2},
                KeyMapping{.key = EKeyCode::D3, .scancode = SDL_SCANCODE_3},
                KeyMapping{.key = EKeyCode::D4, .scancode = SDL_SCANCODE_4},
                KeyMapping{.key = EKeyCode::D5, .scancode = SDL_SCANCODE_5},
                KeyMapping{.key = EKeyCode::D6, .scancode = SDL_SCANCODE_6},
                KeyMapping{.key = EKeyCode::D7, .scancode = SDL_SCANCODE_7},
                KeyMapping{.key = EKeyCode::D8, .scancode = SDL_SCANCODE_8},
                KeyMapping{.key = EKeyCode::D9, .scancode = SDL_SCANCODE_9},

                KeyMapping{.key = EKeyCode::A, .scancode = SDL_SCANCODE_A},
                KeyMapping{.key = EKeyCode::B, .scancode = SDL_SCANCODE_B},
                KeyMapping{.key = EKeyCode::C, .scancode = SDL_SCANCODE_C},
                KeyMapping{.key = EKeyCode::D, .scancode = SDL_SCANCODE_D},
                KeyMapping{.key = EKeyCode::E, .scancode = SDL_SCANCODE_E},
                KeyMapping{.key = EKeyCode::F, .scancode = SDL_SCANCODE_F},
                KeyMapping{.key = EKeyCode::G, .scancode = SDL_SCANCODE_G},
                KeyMapping{.key = EKeyCode::H, .scancode = SDL_SCANCODE_H},
                KeyMapping{.key = EKeyCode::I, .scancode = SDL_SCANCODE_I},
                KeyMapping{.key = EKeyCode::J, .scancode = SDL_SCANCODE_J},
                KeyMapping{.key = EKeyCode::K, .scancode = SDL_SCANCODE_K},
                KeyMapping{.key = EKeyCode::L, .scancode = SDL_SCANCODE_L},
                KeyMapping{.key = EKeyCode::M, .scancode = SDL_SCANCODE_M},
                KeyMapping{.key = EKeyCode::N, .scancode = SDL_SCANCODE_N},
                KeyMapping{.key = EKeyCode::O, .scancode = SDL_SCANCODE_O},
                KeyMapping{.key = EKeyCode::P, .scancode = SDL_SCANCODE_P},
                KeyMapping{.key = EKeyCode::Q, .scancode = SDL_SCANCODE_Q},
                KeyMapping{.key = EKeyCode::R, .scancode = SDL_SCANCODE_R},
                KeyMapping{.key = EKeyCode::S, .scancode = SDL_SCANCODE_S},
                KeyMapping{.key = EKeyCode::T, .scancode = SDL_SCANCODE_T},
                KeyMapping{.key = EKeyCode::U, .scancode = SDL_SCANCODE_U},
                KeyMapping{.key = EKeyCode::V, .scancode = SDL_SCANCODE_V},
                KeyMapping{.key = EKeyCode::W, .scancode = SDL_SCANCODE_W},
                KeyMapping{.key = EKeyCode::X, .scancode = SDL_SCANCODE_X},
                KeyMapping{.key = EKeyCode::Y, .scancode = SDL_SCANCODE_Y},
                KeyMapping{.key = EKeyCode::Z, .scancode = SDL_SCANCODE_Z},

                KeyMapping{.key = EKeyCode::LEFT_SUPER, .scancode = SDL_SCANCODE_LGUI},
                KeyMapping{.key = EKeyCode::RIGHT_SUPER, .scancode = SDL_SCANCODE_RGUI},

                KeyMapping{.key = EKeyCode::NUMPAD0, .scancode = SDL_SCANCODE_KP_0},
                KeyMapping{.key = EKeyCode::NUMPAD1, .scancode = SDL_SCANCODE_KP_1},
                KeyMapping{.key = EKeyCode::NUMPAD2, .scancode = SDL_SCANCODE_KP_2},
                KeyMapping{.key = EKeyCode::NUMPAD3, .scancode = SDL_SCANCODE_KP_3},
                KeyMapping{.key = EKeyCode::NUMPAD4, .scancode = SDL_SCANCODE_KP_4},
                KeyMapping{.key = EKeyCode::NUMPAD5, .scancode = SDL_SCANCODE_KP_5},
                KeyMapping{.key = EKeyCode::NUMPAD6, .scancode = SDL_SCANCODE_KP_6},
                KeyMapping{.key = EKeyCode::NUMPAD7, .scancode = SDL_SCANCODE_KP_7},
                KeyMapping{.key = EKeyCode::NUMPAD8, .scancode = SDL_SCANCODE_KP_8},
                KeyMapping{.key = EKeyCode::NUMPAD9, .scancode = SDL_SCANCODE_KP_9},
                KeyMapping{.key = EKeyCode::MULTIPLY, .scancode = SDL_SCANCODE_KP_MULTIPLY},
                KeyMapping{.key = EKeyCode::ADD, .scancode = SDL_SCANCODE_KP_PLUS},
                KeyMapping{.key = EKeyCode::SEPARATOR, .scancode = SDL_SCANCODE_SEPARATOR},
                KeyMapping{.key = EKeyCode::SUBTRACT, .scancode = SDL_SCANCODE_KP_MINUS},
                KeyMapping{.key = EKeyCode::DECIMAL, .scancode = SDL_SCANCODE_KP_PERIOD},
                KeyMapping{.key = EKeyCode::DIVIDE, .scancode = SDL_SCANCODE_KP_DIVIDE},

                KeyMapping{.key = EKeyCode::F1, .scancode = SDL_SCANCODE_F1},
                KeyMapping{.key = EKeyCode::F2, .scancode = SDL_SCANCODE_F2},
                KeyMapping{.key = EKeyCode::F3, .scancode = SDL_SCANCODE_F3},
                KeyMapping{.key = EKeyCode::F4, .scancode = SDL_SCANCODE_F4},
                KeyMapping{.key = EKeyCode::F5, .scancode = SDL_SCANCODE_F5},
                KeyMapping{.key = EKeyCode::F6, .scancode = SDL_SCANCODE_F6},
                KeyMapping{.key = EKeyCode::F7, .scancode = SDL_SCANCODE_F7},
                KeyMapping{.key = EKeyCode::F8, .scancode = SDL_SCANCODE_F8},
                KeyMapping{.key = EKeyCode::F9, .scancode = SDL_SCANCODE_F9},
                KeyMapping{.key = EKeyCode::F10, .scancode = SDL_SCANCODE_F10},
                KeyMapping{.key = EKeyCode::F11, .scancode = SDL_SCANCODE_F11},
                KeyMapping{.key = EKeyCode::F12, .scancode = SDL_SCANCODE_F12},

                KeyMapping{.key = EKeyCode::NUM_LOCK, .scancode = SDL_SCANCODE_NUMLOCKCLEAR},
                KeyMapping{.key = EKeyCode::SCROLL_LOCK, .scancode = SDL_SCANCODE_SCROLLLOCK},

                KeyMapping{.key = EKeyCode::LEFT_SHIFT, .scancode = SDL_SCANCODE_LSHIFT},
                KeyMapping{.key = EKeyCode::RIGHT_SHIFT, .scancode = SDL_SCANCODE_RSHIFT},
                KeyMapping{.key = EKeyCode::LEFT_CONTROL, .scancode = SDL_SCANCODE_LCTRL},
                KeyMapping{.key = EKeyCode::RIGHT_CONTROL, .scancode = SDL_SCANCODE_RCTRL},
                KeyMapping{.key = EKeyCode::LEFT_ALT, .scancode = SDL_SCANCODE_LALT},
                KeyMapping{.key = EKeyCode::RIGHT_ALT, .scancode = SDL_SCANCODE_RALT},
        };

        static_assert(static_cast<UInt32>(EButtonCode::LEFT) == SDL_BUTTON_LMASK);
        static_assert(static_cast<UInt32>(EButtonCode::MIDDLE) == SDL_BUTTON_MMASK);
        static_assert(static_cast<UInt32>(EButtonCode::RIGHT) == SDL_BUTTON_RMASK);
        static_assert(static_cast<UInt32>(EButtonCode::X1) == SDL_BUTTON_X1MASK);
        static_assert(static_cast<UInt32>(EButtonCode::X2) == SDL_BUTTON_X2MASK);
    } // namespace

    ESystemStatus InputSubsystem::OnStartup()
    {
        Engine* engine = Engine::GetInstance();
        if (engine != nullptr)
        {
            if (EventSubsystem* eventSubsystem = engine->GetSubsystem<EventSubsystem>(); eventSubsystem != nullptr)
            {
                displayCreateListenerId =
                        eventSubsystem->Subscribe<DisplayCreateEvent>(this, &InputSubsystem::OnDisplayCreate);
                displayCloseListenerId =
                        eventSubsystem->Subscribe<DisplayCloseEvent>(this, &InputSubsystem::OnDisplayClose);
                displayFocusListenerId =
                        eventSubsystem->Subscribe<DisplayFocusEvent>(this, &InputSubsystem::OnDisplayFocus);
                inputActivationListenerId =
                        eventSubsystem->Subscribe<InputActivationEvent>(this, &InputSubsystem::OnInputActivation);
                mouseWheelListenerId = eventSubsystem->Subscribe<MouseWheelEvent>(this, &InputSubsystem::OnMouseWheel);
            }
        }

        return ESystemStatus::CONTINUE;
    }

    ESystemStatus InputSubsystem::OnEarlyUpdate()
    {
        if (!isInputEnabled || activeDisplay == nullptr)
        {
            return ESystemStatus::CONTINUE;
        }

        const auto* displayImpl = static_cast<const DisplayImpl*>(activeDisplay);
        if (SDL_GetKeyboardFocus() != displayImpl->GetSDLWindow())
        {
            ResetInputState();
            return ESystemStatus::CONTINUE;
        }

        provider.Update();
        PollKeyboard(provider);
        PollMouse(provider);

        if (needsInputSync)
        {
            provider.SynchronizeHistory();
            needsInputSync = false;
        }

        for (const auto& action : actions | std::views::values)
        {
            action->Evaluate(provider);
        }

        return ESystemStatus::CONTINUE;
    }

    ESystemStatus InputSubsystem::OnFixedUpdate()
    {
        return ESystemStatus::CONTINUE;
    }

    ESystemStatus InputSubsystem::OnUpdate()
    {
        return ESystemStatus::CONTINUE;
    }

    ESystemStatus InputSubsystem::OnLateUpdate()
    {
        return ESystemStatus::CONTINUE;
    }

    void InputSubsystem::OnShutdown()
    {
        Engine* engine = Engine::GetInstance();
        if (engine != nullptr)
        {
            if (EventSubsystem* eventSubsystem = engine->GetSubsystem<EventSubsystem>(); eventSubsystem != nullptr)
            {
                eventSubsystem->Unsubscribe<DisplayCreateEvent>(displayCreateListenerId);
                eventSubsystem->Unsubscribe<DisplayCloseEvent>(displayCloseListenerId);
                eventSubsystem->Unsubscribe<DisplayFocusEvent>(displayFocusListenerId);
                eventSubsystem->Unsubscribe<InputActivationEvent>(inputActivationListenerId);
                eventSubsystem->Unsubscribe<MouseWheelEvent>(mouseWheelListenerId);
            }
        }

        displayCreateListenerId   = 0;
        displayCloseListenerId    = 0;
        displayFocusListenerId    = 0;
        inputActivationListenerId = 0;
        mouseWheelListenerId      = 0;

        ResetInputState();
        actions.clear();
        activeDisplay = nullptr;
    }

    void InputSubsystem::SetInputEnabled(bool value) noexcept
    {
        if (isInputEnabled == value)
        {
            return;
        }

        isInputEnabled = value;
        needsInputSync = value;
        ResetInputState();
    }

    InputAction* InputSubsystem::AddAction(std::string_view name)
    {
        const std::string KEY(name);
        auto [it, inserted] = actions.try_emplace(KEY, nullptr);
        if (inserted)
        {
            it->second = std::make_unique<InputAction>(KEY);
        }

        return it->second.get();
    }

    void InputSubsystem::RemoveAction(std::string_view name) noexcept
    {
        const auto IT = actions.find(std::string(name));
        if (IT != actions.end())
        {
            IT->second->Reset();
            actions.erase(IT);
        }
    }

    InputAction* InputSubsystem::GetAction(std::string_view name) noexcept
    {
        const auto IT = actions.find(std::string(name));
        return IT != actions.end() ? IT->second.get() : nullptr;
    }

    const InputAction* InputSubsystem::GetAction(std::string_view name) const noexcept
    {
        const auto IT = actions.find(std::string(name));
        return IT != actions.end() ? IT->second.get() : nullptr;
    }

    void InputSubsystem::OnDisplayCreate(const DisplayCreateEvent& event)
    {
        ASSERT(event.display != nullptr);

        if (activeDisplay == nullptr || event.display->IsFocused())
        {
            activeDisplay   = event.display;
            hasMouseHistory = false;
        }
    }

    void InputSubsystem::OnDisplayClose(const DisplayCloseEvent& event)
    {
        if (activeDisplay == event.display)
        {
            ResetInputState();
            activeDisplay = nullptr;
        }
    }

    void InputSubsystem::OnDisplayFocus(const DisplayFocusEvent& event)
    {
        if (event.display == nullptr)
        {
            return;
        }

        ResetInputState();

        if (!event.isFocused)
        {
            if (activeDisplay == event.display)
            {
                activeDisplay = nullptr;
            }
            return;
        }

        activeDisplay = event.display;
    }

    void InputSubsystem::OnInputActivation(const InputActivationEvent& event)
    {
        if (!event.isActive)
        {
            ResetInputState();
        }
    }

    void InputSubsystem::OnMouseWheel(const MouseWheelEvent& event) noexcept
    {
        if (event.display == nullptr || event.display == activeDisplay)
        {
            pendingWheelDelta += event.delta;
        }
    }

    void InputSubsystem::PollKeyboard(InputProvider& targetProvider) noexcept
    {
        int         numKeys       = 0;
        const bool* keyboardState = SDL_GetKeyboardState(&numKeys);
        if (keyboardState == nullptr || numKeys <= 0)
        {
            return;
        }

        for (const KeyMapping& mapping : KEY_MAPPINGS)
        {
            const int SCANCODE_INDEX = static_cast<int>(mapping.scancode);
            if (SCANCODE_INDEX > 0 && SCANCODE_INDEX < numKeys && keyboardState[SCANCODE_INDEX])
            {
                targetProvider.SetKey(mapping.key, true);
            }
        }

        targetProvider.SetKey(EKeyCode::SHIFT,
                              targetProvider.IsKeyHeld(EKeyCode::LEFT_SHIFT) ||
                                      targetProvider.IsKeyHeld(EKeyCode::RIGHT_SHIFT));
        targetProvider.SetKey(EKeyCode::CONTROL,
                              targetProvider.IsKeyHeld(EKeyCode::LEFT_CONTROL) ||
                                      targetProvider.IsKeyHeld(EKeyCode::RIGHT_CONTROL));
        targetProvider.SetKey(EKeyCode::ALT,
                              targetProvider.IsKeyHeld(EKeyCode::LEFT_ALT) ||
                                      targetProvider.IsKeyHeld(EKeyCode::RIGHT_ALT));
    }

    void InputSubsystem::PollMouse(InputProvider& targetProvider) noexcept
    {
        float                      posX         = 0.0f;
        float                      posY         = 0.0f;
        const SDL_MouseButtonFlags BUTTON_FLAGS = SDL_GetMouseState(&posX, &posY);

        targetProvider.SetMouseButtons(static_cast<UInt32>(BUTTON_FLAGS));
        targetProvider.SetMousePosition(Vector2DInt{static_cast<Int32>(posX), static_cast<Int32>(posY)});

        float relX = 0.0f;
        float relY = 0.0f;
        SDL_GetRelativeMouseState(&relX, &relY);

        if (hasMouseHistory)
        {
            targetProvider.AddMousePositionDelta(Vector2DInt{static_cast<Int32>(relX), static_cast<Int32>(relY)});
        }

        hasMouseHistory = true;

        if (pendingWheelDelta.GetX() != 0 || pendingWheelDelta.GetY() != 0)
        {
            targetProvider.AddMouseWheelDelta(pendingWheelDelta);
            pendingWheelDelta = {};
        }
    }

    void InputSubsystem::ResetInputState() noexcept
    {
        for (const auto& action : actions | std::views::values)
        {
            action->Reset();
        }

        provider.Reset();
        hasMouseHistory   = false;
        pendingWheelDelta = {};
    }
} // namespace Garage
