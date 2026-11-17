#include "garage/platform/input_subsystem.hpp"

#include <array>
#include <ranges>

#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_scancode.h>

#include "garage/core/defines.hpp"
#include "garage/core/engine.hpp"

#include "garage/platform/display.hpp"
#include "garage/platform/event_subsystem.hpp"

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
                KeyMapping{EKeyCode::BACKSPACE, SDL_SCANCODE_BACKSPACE},
                KeyMapping{EKeyCode::TAB, SDL_SCANCODE_TAB},
                KeyMapping{EKeyCode::ENTER, SDL_SCANCODE_RETURN},
                KeyMapping{EKeyCode::PAUSE, SDL_SCANCODE_PAUSE},
                KeyMapping{EKeyCode::CAPS_LOCK, SDL_SCANCODE_CAPSLOCK},
                KeyMapping{EKeyCode::ESCAPE, SDL_SCANCODE_ESCAPE},
                KeyMapping{EKeyCode::SPACE, SDL_SCANCODE_SPACE},
                KeyMapping{EKeyCode::PAGE_UP, SDL_SCANCODE_PAGEUP},
                KeyMapping{EKeyCode::PAGE_DOWN, SDL_SCANCODE_PAGEDOWN},
                KeyMapping{EKeyCode::END, SDL_SCANCODE_END},
                KeyMapping{EKeyCode::HOME, SDL_SCANCODE_HOME},
                KeyMapping{EKeyCode::LEFT, SDL_SCANCODE_LEFT},
                KeyMapping{EKeyCode::UP, SDL_SCANCODE_UP},
                KeyMapping{EKeyCode::RIGHT, SDL_SCANCODE_RIGHT},
                KeyMapping{EKeyCode::DOWN, SDL_SCANCODE_DOWN},
                KeyMapping{EKeyCode::PRINT_SCREEN, SDL_SCANCODE_PRINTSCREEN},
                KeyMapping{EKeyCode::INSERT, SDL_SCANCODE_INSERT},
                KeyMapping{EKeyCode::DELETE_KEY, SDL_SCANCODE_DELETE},

                KeyMapping{EKeyCode::D0, SDL_SCANCODE_0},
                KeyMapping{EKeyCode::D1, SDL_SCANCODE_1},
                KeyMapping{EKeyCode::D2, SDL_SCANCODE_2},
                KeyMapping{EKeyCode::D3, SDL_SCANCODE_3},
                KeyMapping{EKeyCode::D4, SDL_SCANCODE_4},
                KeyMapping{EKeyCode::D5, SDL_SCANCODE_5},
                KeyMapping{EKeyCode::D6, SDL_SCANCODE_6},
                KeyMapping{EKeyCode::D7, SDL_SCANCODE_7},
                KeyMapping{EKeyCode::D8, SDL_SCANCODE_8},
                KeyMapping{EKeyCode::D9, SDL_SCANCODE_9},

                KeyMapping{EKeyCode::A, SDL_SCANCODE_A},
                KeyMapping{EKeyCode::B, SDL_SCANCODE_B},
                KeyMapping{EKeyCode::C, SDL_SCANCODE_C},
                KeyMapping{EKeyCode::D, SDL_SCANCODE_D},
                KeyMapping{EKeyCode::E, SDL_SCANCODE_E},
                KeyMapping{EKeyCode::F, SDL_SCANCODE_F},
                KeyMapping{EKeyCode::G, SDL_SCANCODE_G},
                KeyMapping{EKeyCode::H, SDL_SCANCODE_H},
                KeyMapping{EKeyCode::I, SDL_SCANCODE_I},
                KeyMapping{EKeyCode::J, SDL_SCANCODE_J},
                KeyMapping{EKeyCode::K, SDL_SCANCODE_K},
                KeyMapping{EKeyCode::L, SDL_SCANCODE_L},
                KeyMapping{EKeyCode::M, SDL_SCANCODE_M},
                KeyMapping{EKeyCode::N, SDL_SCANCODE_N},
                KeyMapping{EKeyCode::O, SDL_SCANCODE_O},
                KeyMapping{EKeyCode::P, SDL_SCANCODE_P},
                KeyMapping{EKeyCode::Q, SDL_SCANCODE_Q},
                KeyMapping{EKeyCode::R, SDL_SCANCODE_R},
                KeyMapping{EKeyCode::S, SDL_SCANCODE_S},
                KeyMapping{EKeyCode::T, SDL_SCANCODE_T},
                KeyMapping{EKeyCode::U, SDL_SCANCODE_U},
                KeyMapping{EKeyCode::V, SDL_SCANCODE_V},
                KeyMapping{EKeyCode::W, SDL_SCANCODE_W},
                KeyMapping{EKeyCode::X, SDL_SCANCODE_X},
                KeyMapping{EKeyCode::Y, SDL_SCANCODE_Y},
                KeyMapping{EKeyCode::Z, SDL_SCANCODE_Z},

                KeyMapping{EKeyCode::LEFT_SUPER, SDL_SCANCODE_LGUI},
                KeyMapping{EKeyCode::RIGHT_SUPER, SDL_SCANCODE_RGUI},

                KeyMapping{EKeyCode::NUMPAD0, SDL_SCANCODE_KP_0},
                KeyMapping{EKeyCode::NUMPAD1, SDL_SCANCODE_KP_1},
                KeyMapping{EKeyCode::NUMPAD2, SDL_SCANCODE_KP_2},
                KeyMapping{EKeyCode::NUMPAD3, SDL_SCANCODE_KP_3},
                KeyMapping{EKeyCode::NUMPAD4, SDL_SCANCODE_KP_4},
                KeyMapping{EKeyCode::NUMPAD5, SDL_SCANCODE_KP_5},
                KeyMapping{EKeyCode::NUMPAD6, SDL_SCANCODE_KP_6},
                KeyMapping{EKeyCode::NUMPAD7, SDL_SCANCODE_KP_7},
                KeyMapping{EKeyCode::NUMPAD8, SDL_SCANCODE_KP_8},
                KeyMapping{EKeyCode::NUMPAD9, SDL_SCANCODE_KP_9},
                KeyMapping{EKeyCode::MULTIPLY, SDL_SCANCODE_KP_MULTIPLY},
                KeyMapping{EKeyCode::ADD, SDL_SCANCODE_KP_PLUS},
                KeyMapping{EKeyCode::SEPARATOR, SDL_SCANCODE_SEPARATOR},
                KeyMapping{EKeyCode::SUBTRACT, SDL_SCANCODE_KP_MINUS},
                KeyMapping{EKeyCode::DECIMAL, SDL_SCANCODE_KP_PERIOD},
                KeyMapping{EKeyCode::DIVIDE, SDL_SCANCODE_KP_DIVIDE},

                KeyMapping{EKeyCode::F1, SDL_SCANCODE_F1},
                KeyMapping{EKeyCode::F2, SDL_SCANCODE_F2},
                KeyMapping{EKeyCode::F3, SDL_SCANCODE_F3},
                KeyMapping{EKeyCode::F4, SDL_SCANCODE_F4},
                KeyMapping{EKeyCode::F5, SDL_SCANCODE_F5},
                KeyMapping{EKeyCode::F6, SDL_SCANCODE_F6},
                KeyMapping{EKeyCode::F7, SDL_SCANCODE_F7},
                KeyMapping{EKeyCode::F8, SDL_SCANCODE_F8},
                KeyMapping{EKeyCode::F9, SDL_SCANCODE_F9},
                KeyMapping{EKeyCode::F10, SDL_SCANCODE_F10},
                KeyMapping{EKeyCode::F11, SDL_SCANCODE_F11},
                KeyMapping{EKeyCode::F12, SDL_SCANCODE_F12},

                KeyMapping{EKeyCode::NUM_LOCK, SDL_SCANCODE_NUMLOCKCLEAR},
                KeyMapping{EKeyCode::SCROLL_LOCK, SDL_SCANCODE_SCROLLLOCK},

                KeyMapping{EKeyCode::LEFT_SHIFT, SDL_SCANCODE_LSHIFT},
                KeyMapping{EKeyCode::RIGHT_SHIFT, SDL_SCANCODE_RSHIFT},
                KeyMapping{EKeyCode::LEFT_CONTROL, SDL_SCANCODE_LCTRL},
                KeyMapping{EKeyCode::RIGHT_CONTROL, SDL_SCANCODE_RCTRL},
                KeyMapping{EKeyCode::LEFT_ALT, SDL_SCANCODE_LALT},
                KeyMapping{EKeyCode::RIGHT_ALT, SDL_SCANCODE_RALT},
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
        const std::string key(name);
        auto [it, inserted] = actions.try_emplace(key, nullptr);
        if (inserted)
        {
            it->second = std::make_unique<InputAction>(key);
        }

        return it->second.get();
    }

    void InputSubsystem::RemoveAction(std::string_view name) noexcept
    {
        const auto it = actions.find(std::string(name));
        if (it != actions.end())
        {
            it->second->Reset();
            actions.erase(it);
        }
    }

    InputAction* InputSubsystem::GetAction(std::string_view name) noexcept
    {
        const auto it = actions.find(std::string(name));
        return it != actions.end() ? it->second.get() : nullptr;
    }

    const InputAction* InputSubsystem::GetAction(std::string_view name) const noexcept
    {
        const auto it = actions.find(std::string(name));
        return it != actions.end() ? it->second.get() : nullptr;
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
            const int scancodeIndex = static_cast<int>(mapping.scancode);
            if (scancodeIndex > 0 && scancodeIndex < numKeys && keyboardState[scancodeIndex])
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
        float                      posX        = 0.0f;
        float                      posY        = 0.0f;
        const SDL_MouseButtonFlags buttonFlags = SDL_GetMouseState(&posX, &posY);

        targetProvider.SetMouseButtons(static_cast<UInt32>(buttonFlags));
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
