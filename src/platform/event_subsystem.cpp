#include "garage/platform/event_subsystem.hpp"

#include <tuple>
#include <variant>

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>

#include "garage/core/system_context.hpp"
#include "garage/core/system_status.hpp"
#include "garage/core/types.hpp"

#include "garage/math/vector2d.hpp"

#include "garage/platform/display.hpp"
#include "garage/platform/display_events.hpp"
#include "garage/platform/display_subsystem.hpp"
#include "garage/platform/event.hpp"
#include "garage/platform/input_events.hpp"
#include "platform/display_impl.hpp"

#include "garage/utilities/logger.hpp"

namespace Garage
{
    ESystemStatus EventSubsystem::OnStartup([[maybe_unused]] SystemContext& context)
    {
        if (!SDL_InitSubSystem(SDL_INIT_EVENTS))
        {
            LOG_ERROR("SDL 이벤트 서브시스템 초기화에 실패했습니다: {}", SDL_GetError());
            return ESystemStatus::FAILURE;
        }

        return ESystemStatus::CONTINUE;
    }

    ESystemStatus EventSubsystem::OnPreTick(SystemContext& context)
    {
        DisplaySubsystem* displaySubsystem = context.GetEngineSubsystem<DisplaySubsystem>();

        auto const FIND_DISPLAY = [displaySubsystem](UInt32 windowId) noexcept -> IDisplay*
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
                    PushEvent(InputActivationEvent{.display = nullptr, .isActive = true});
                    break;
                }
                case SDL_EVENT_WILL_ENTER_BACKGROUND:
                {
                    PushEvent(InputActivationEvent{.display = nullptr, .isActive = false});
                    break;
                }
                case SDL_EVENT_WINDOW_MOVED:
                {
                    if (IDisplay const* display = FIND_DISPLAY(sdlEvent.window.windowID); display != nullptr)
                    {
                        PushEvent(DisplayMoveEvent{.display = display,
                                                   .position =
                                                           Vector2DInt{sdlEvent.window.data1, sdlEvent.window.data2}});
                    }
                    break;
                }
                case SDL_EVENT_WINDOW_RESIZED:
                {
                    if (IDisplay const* display = FIND_DISPLAY(sdlEvent.window.windowID); display != nullptr)
                    {
                        PushEvent(DisplayResizeEvent{
                                .display = display, .size = Vector2DInt{sdlEvent.window.data1, sdlEvent.window.data2}});
                    }
                    break;
                }
                case SDL_EVENT_WINDOW_FOCUS_GAINED:
                {
                    if (IDisplay const* display = FIND_DISPLAY(sdlEvent.window.windowID); display != nullptr)
                    {
                        PushEvent(DisplayFocusEvent{.display = display, .isFocused = true});
                    }
                    break;
                }
                case SDL_EVENT_WINDOW_FOCUS_LOST:
                {
                    if (IDisplay const* display = FIND_DISPLAY(sdlEvent.window.windowID); display != nullptr)
                    {
                        PushEvent(DisplayFocusEvent{.display = display, .isFocused = false});
                    }
                    break;
                }
                case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                {
                    if (IDisplay* display = FIND_DISPLAY(sdlEvent.window.windowID); display != nullptr)
                    {
                        display->SetShouldClose(true);
                        PushEvent(DisplayCloseEvent{display});
                    }
                    break;
                }
                case SDL_EVENT_MOUSE_WHEEL:
                {
                    IDisplay const* display = FIND_DISPLAY(sdlEvent.wheel.windowID);
                    PushEvent(
                            MouseWheelEvent{.display = display,
                                            .delta = Vector2DInt{sdlEvent.wheel.integer_x, sdlEvent.wheel.integer_y}});
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

    ESystemStatus EventSubsystem::OnTick([[maybe_unused]] SystemContext& context)
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

    ESystemStatus EventSubsystem::OnPostTick([[maybe_unused]] SystemContext& context)
    {
        return ESystemStatus::CONTINUE;
    }

    void EventSubsystem::OnShutdown([[maybe_unused]] SystemContext& context)
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
