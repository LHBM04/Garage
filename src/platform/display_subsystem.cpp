#include "garage/platform/display_subsystem.hpp"

#include <algorithm>

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_properties.h>
#include <SDL3/SDL_video.h>

#include "garage/core/defines.hpp"
#include "garage/core/engine.hpp"
#include "garage/platform/display_events.hpp"
#include "garage/platform/event_subsystem.hpp"
#include "garage/utilities/logger.hpp"

#include "platform/display_impl.hpp"

namespace Garage
{
    ESystemStatus DisplaySubsystem::OnStartup()
    {
        if (!SDL_InitSubSystem(SDL_INIT_VIDEO))
        {
            LOG_ERROR("SDL 비디오 서브시스템 초기화에 실패했습니다: {}", SDL_GetError());
            return ESystemStatus::FAILURE;
        }

        return ESystemStatus::CONTINUE;
    }

    ESystemStatus DisplaySubsystem::OnPreTick()
    {
        return ESystemStatus::CONTINUE;
    }

    ESystemStatus DisplaySubsystem::OnTick()
    {
        return ESystemStatus::CONTINUE;
    }

    ESystemStatus DisplaySubsystem::OnPostTick()
    {
        Engine*         engine         = Engine::GetInstance();
        EventSubsystem* eventSubsystem = (engine != nullptr) ? engine->GetSubsystem<EventSubsystem>() : nullptr;

        for (const std::unique_ptr<IDisplay>& display : displays)
        {
            if (display->ShouldClose() && eventSubsystem != nullptr)
            {
                eventSubsystem->DispatchEvent(DisplayDestroyEvent{display.get()});
            }
        }

        const USize removed = std::erase_if(displays,
                                            [](const std::unique_ptr<IDisplay>& display)
                                            {
                                                return display->ShouldClose();
                                            });

        if (removed != 0 && displays.empty())
        {
            return ESystemStatus::SUCCESS;
        }

        return ESystemStatus::CONTINUE;
    }

    void DisplaySubsystem::OnShutdown()
    {
        displays.clear();
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
    }

    IDisplay* DisplaySubsystem::CreateDisplay(const DisplayOptions& options) noexcept
    {
        const SDL_PropertiesID properties = SDL_CreateProperties();
        if (properties == 0)
        {
            LOG_ERROR("SDL 윈도우 속성 생성에 실패했습니다: {}", SDL_GetError());
            return nullptr;
        }

        UInt64 windowFlags = options.flags;
        if (!options.visible)
        {
            windowFlags |= SDL_WINDOW_HIDDEN;
        }

        const Int32 posX = options.centered ? SDL_WINDOWPOS_CENTERED : options.position.GetX();
        const Int32 posY = options.centered ? SDL_WINDOWPOS_CENTERED : options.position.GetY();

        SDL_SetStringProperty(properties, SDL_PROP_WINDOW_CREATE_TITLE_STRING, options.title.c_str());
        SDL_SetNumberProperty(properties, SDL_PROP_WINDOW_CREATE_X_NUMBER, posX);
        SDL_SetNumberProperty(properties, SDL_PROP_WINDOW_CREATE_Y_NUMBER, posY);
        SDL_SetNumberProperty(properties, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, options.size.GetX());
        SDL_SetNumberProperty(properties, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, options.size.GetY());
        SDL_SetNumberProperty(properties, SDL_PROP_WINDOW_CREATE_FLAGS_NUMBER, static_cast<Sint64>(windowFlags));

        SDL_Window* sdlWindow = SDL_CreateWindowWithProperties(properties);
        SDL_DestroyProperties(properties);

        if (sdlWindow == nullptr)
        {
            LOG_ERROR("SDL 윈도우 생성에 실패했습니다: {}", SDL_GetError());
            return nullptr;
        }

        auto      newDisplay = std::make_unique<DisplayImpl>(sdlWindow);
        IDisplay* pointer    = newDisplay.get();
        displays.push_back(std::move(newDisplay));

        Engine* engine = Engine::GetInstance();
        if (engine != nullptr)
        {
            if (EventSubsystem* eventSubsystem = engine->GetSubsystem<EventSubsystem>(); eventSubsystem != nullptr)
            {
                eventSubsystem->PushEvent(DisplayCreateEvent{pointer});
            }
        }

        return pointer;
    }

    void DisplaySubsystem::DestroyDisplay(IDisplay* display) noexcept
    {
        ASSERT(display != nullptr);

        Engine* engine = Engine::GetInstance();
        if (engine != nullptr)
        {
            if (EventSubsystem* eventSubsystem = engine->GetSubsystem<EventSubsystem>(); eventSubsystem != nullptr)
            {
                eventSubsystem->DispatchEvent(DisplayDestroyEvent{display});
            }
        }

        std::erase_if(displays,
                      [display](const std::unique_ptr<IDisplay>& item)
                      {
                          return item.get() == display;
                      });
    }

    USize DisplaySubsystem::GetDisplayCount() const noexcept
    {
        return displays.size();
    }

    IDisplay* DisplaySubsystem::GetDisplay(USize index) noexcept
    {
        return index < displays.size() ? displays[index].get() : nullptr;
    }

    const IDisplay* DisplaySubsystem::GetDisplay(USize index) const noexcept
    {
        return index < displays.size() ? displays[index].get() : nullptr;
    }

    IDisplay* DisplaySubsystem::FindDisplayByID(IDisplay::ID id) noexcept
    {
        const auto result = std::ranges::find_if(displays,
                                                 [id](const auto& display)
                                                 {
                                                     return display->GetID() == id;
                                                 });
        return result != displays.end() ? result->get() : nullptr;
    }

    const IDisplay* DisplaySubsystem::FindDisplayByID(IDisplay::ID id) const noexcept
    {
        const auto result = std::ranges::find_if(displays,
                                                 [id](const auto& display)
                                                 {
                                                     return display->GetID() == id;
                                                 });
        return result != displays.end() ? result->get() : nullptr;
    }

    bool DisplaySubsystem::IsDisplayValid(const IDisplay* display) const noexcept
    {
        if (display == nullptr)
        {
            return false;
        }

        return std::ranges::any_of(displays,
                                   [display](const auto& item)
                                   {
                                       return item.get() == display;
                                   });
    }

    bool DisplaySubsystem::IsDisplayValid(IDisplay::ID id) const noexcept
    {
        return FindDisplayByID(id) != nullptr;
    }
} // namespace Garage
