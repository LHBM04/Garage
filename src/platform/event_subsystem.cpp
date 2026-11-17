#include "garage/platform/event_subsystem.hpp"

#include <tuple>
#include <variant>

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>

#include "garage/core/engine.hpp"

#include "garage/math/vector2d.hpp"

#include "garage/platform/display.hpp"
#include "garage/platform/display_events.hpp"
#include "garage/platform/display_subsystem.hpp"
#include "garage/platform/input_events.hpp"

#include "garage/utilities/logger.hpp"

#include "platform/display_impl.hpp"

namespace Garage
{
    ESystemStatus EventSubsystem::OnStartup()
    {
        if (!SDL_InitSubSystem(SDL_INIT_EVENTS))
        {
            LOG_ERROR("SDL 이벤트 서브시스템 초기화에 실패했습니다: {}", SDL_GetError());
            return ESystemStatus::FAILURE;
        }

        return ESystemStatus::CONTINUE;
    }

    ESystemStatus EventSubsystem::OnPreTick()
    {
        Engine*           engine           = Engine::GetInstance();
        DisplaySubsystem* displaySubsystem = (engine != nullptr) ? engine->GetSubsystem<DisplaySubsystem>() : nullptr;

        auto findDisplay = [displaySubsystem](UInt32 windowId) noexcept -> IDisplay*
        {
            if (displaySubsystem == nullptr || windowId == 0)
            {
                return nullptr;
            }

            for (IDisplay* display : displaySubsystem->GetDisplays())
            {
                const auto* displayImpl = static_cast<const DisplayImpl*>(display);
                if (displayImpl->GetSDLWindowID() == windowId)
                {
                    return display;
                }
            }

            return nullptr;
        };

        SDL_Event sdlEvent{};
        while (SDL_PollEvent(&sdlEvent))
        {
            switch (sdlEvent.type)
            {
                case SDL_EVENT_QUIT:
                {
                    return ESystemStatus::SUCCESS;
                }
                case SDL_EVENT_DID_ENTER_FOREGROUND:
                {
                    PushEvent(InputActivationEvent{nullptr, true});
                    break;
                }
                case SDL_EVENT_WILL_ENTER_BACKGROUND:
                {
                    PushEvent(InputActivationEvent{nullptr, false});
                    break;
                }
                case SDL_EVENT_WINDOW_MOVED:
                {
                    if (IDisplay* display = findDisplay(sdlEvent.window.windowID); display != nullptr)
                    {
                        PushEvent(DisplayMoveEvent{display, Vector2DInt{sdlEvent.window.data1, sdlEvent.window.data2}});
                    }
                    break;
                }
                case SDL_EVENT_WINDOW_RESIZED:
                {
                    if (IDisplay* display = findDisplay(sdlEvent.window.windowID); display != nullptr)
                    {
                        PushEvent(
                                DisplayResizeEvent{display, Vector2DInt{sdlEvent.window.data1, sdlEvent.window.data2}});
                    }
                    break;
                }
                case SDL_EVENT_WINDOW_FOCUS_GAINED:
                {
                    if (IDisplay* display = findDisplay(sdlEvent.window.windowID); display != nullptr)
                    {
                        PushEvent(DisplayFocusEvent{display, true});
                    }
                    break;
                }
                case SDL_EVENT_WINDOW_FOCUS_LOST:
                {
                    if (IDisplay* display = findDisplay(sdlEvent.window.windowID); display != nullptr)
                    {
                        PushEvent(DisplayFocusEvent{display, false});
                    }
                    break;
                }
                case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                {
                    if (IDisplay* display = findDisplay(sdlEvent.window.windowID); display != nullptr)
                    {
                        display->SetShouldClose(true);
                        PushEvent(DisplayCloseEvent{display});
                    }
                    break;
                }
                case SDL_EVENT_MOUSE_WHEEL:
                {
                    IDisplay* display = findDisplay(sdlEvent.wheel.windowID);
                    PushEvent(
                            MouseWheelEvent{display, Vector2DInt{sdlEvent.wheel.integer_x, sdlEvent.wheel.integer_y}});
                    break;
                }
                default:
                {
                    break;
                }
            }
        }

        return ESystemStatus::CONTINUE;
    }

    ESystemStatus EventSubsystem::OnTick()
    {
        readBuffer.swap(writeBuffer);

        for (const Event& event : readBuffer)
        {
            std::visit(
                    [this](const auto& concreteEvent)
                    {
                        DispatchEvent(concreteEvent);
                    },
                    event);
        }

        readBuffer.clear();
        return ESystemStatus::CONTINUE;
    }

    ESystemStatus EventSubsystem::OnPostTick()
    {
        return ESystemStatus::CONTINUE;
    }

    void EventSubsystem::OnShutdown()
    {
        writeBuffer.clear();
        readBuffer.clear();
        std::apply(
                [](auto&... delegate) noexcept
                {
                    (delegate.Clear(), ...);
                },
                listeners);
        SDL_QuitSubSystem(SDL_INIT_EVENTS);
    }
} // namespace Garage
