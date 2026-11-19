#include "garage/platform/display_subsystem.hpp"

#include <algorithm>
#include <utility>

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_properties.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_video.h>

#include "garage/core/allocator.hpp"
#include "garage/core/defines.hpp"
#include "garage/core/system_context.hpp"
#include "garage/core/system_status.hpp"
#include "garage/core/types.hpp"

#include "garage/platform/display_events.hpp"
#include "garage/platform/display_options.hpp"
#include "garage/platform/event_subsystem.hpp"
#include "platform/display_impl.hpp"

#include "garage/utilities/logger.hpp"

namespace Garage
{
    ESystemStatus DisplaySubsystem::OnStartup(SystemContext& context)
    {
        if (!SDL_InitSubSystem(SDL_INIT_VIDEO))
        {
            LOG_ERROR("SDL 비디오 서브시스템 초기화에 실패했습니다: {}", SDL_GetError());
            return ESystemStatus::FAILURE;
        }

        eventSubsystem = context.GetEngineSubsystem<EventSubsystem>();
        return ESystemStatus::CONTINUE;
    }

    ESystemStatus DisplaySubsystem::OnPreTick([[maybe_unused]] SystemContext& context)
    {
        return ESystemStatus::CONTINUE;
    }

    ESystemStatus DisplaySubsystem::OnTick([[maybe_unused]] SystemContext& context)
    {
        return ESystemStatus::CONTINUE;
    }

    ESystemStatus DisplaySubsystem::OnPostTick(SystemContext& context)
    {
        eventSubsystem = context.GetEngineSubsystem<EventSubsystem>();

        for (const Allocator::UniquePtr<IDisplay>& display : displays)
        {
            if (display->ShouldClose() && eventSubsystem != nullptr)
            {
                eventSubsystem->DispatchEvent(DisplayDestroyEvent{display.get()});
            }
        }

        const USize REMOVED = std::erase_if(displays,
                                            [](const Allocator::UniquePtr<IDisplay>& display)
                                            {
                                                return display->ShouldClose();
                                            });

        if (REMOVED != 0 && displays.empty())
        {
            return ESystemStatus::SUCCESS;
        }

        return ESystemStatus::CONTINUE;
    }

    void DisplaySubsystem::OnShutdown([[maybe_unused]] SystemContext& context)
    {
        displays.clear();
        eventSubsystem = nullptr;
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
    }

    IDisplay* DisplaySubsystem::CreateDisplay(const DisplayOptions& options) noexcept
    {
        const SDL_PropertiesID PROPERTIES = SDL_CreateProperties();
        if (PROPERTIES == 0)
        {
            LOG_ERROR("SDL 윈도우 속성 생성에 실패했습니다: {}", SDL_GetError());
            return nullptr;
        }

        UInt64 windowFlags = options.flags;
        if (!options.visible)
        {
            windowFlags |= SDL_WINDOW_HIDDEN;
        }

        const Int32 POS_X = options.centered ? SDL_WINDOWPOS_CENTERED : options.position.GetX();
        const Int32 POS_Y = options.centered ? SDL_WINDOWPOS_CENTERED : options.position.GetY();

        SDL_SetStringProperty(PROPERTIES, SDL_PROP_WINDOW_CREATE_TITLE_STRING, options.title.c_str());
        SDL_SetNumberProperty(PROPERTIES, SDL_PROP_WINDOW_CREATE_X_NUMBER, POS_X);
        SDL_SetNumberProperty(PROPERTIES, SDL_PROP_WINDOW_CREATE_Y_NUMBER, POS_Y);
        SDL_SetNumberProperty(PROPERTIES, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, options.size.GetX());
        SDL_SetNumberProperty(PROPERTIES, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, options.size.GetY());
        SDL_SetNumberProperty(PROPERTIES, SDL_PROP_WINDOW_CREATE_FLAGS_NUMBER, static_cast<Sint64>(windowFlags));

        SDL_Window* sdlWindow = SDL_CreateWindowWithProperties(PROPERTIES);
        SDL_DestroyProperties(PROPERTIES);

        if (sdlWindow == nullptr)
        {
            LOG_ERROR("SDL 윈도우 생성에 실패했습니다: {}", SDL_GetError());
            return nullptr;
        }

        auto      newDisplay = Allocator::MakeUnique<DisplayImpl>(sdlWindow);
        IDisplay* pointer    = newDisplay.get();
        displays.push_back(std::move(newDisplay));

        if (eventSubsystem != nullptr)
        {
            eventSubsystem->PushEvent(DisplayCreateEvent{pointer});
        }

        return pointer;
    }

    void DisplaySubsystem::DestroyDisplay(IDisplay* display) noexcept
    {
        ASSERT(display != nullptr);

        if (eventSubsystem != nullptr)
        {
            eventSubsystem->DispatchEvent(DisplayDestroyEvent{display});
        }

        std::erase_if(displays,
                      [display](const Allocator::UniquePtr<IDisplay>& item)
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

    IDisplay* DisplaySubsystem::FindDisplayByID(IDisplay::EId id) noexcept
    {
        const auto RESULT = std::ranges::find_if(displays,
                                                 [id](const auto& display)
                                                 {
                                                     return display->GetID() == id;
                                                 });
        return RESULT != displays.end() ? RESULT->get() : nullptr;
    }

    const IDisplay* DisplaySubsystem::FindDisplayByID(IDisplay::EId id) const noexcept
    {
        const auto RESULT = std::ranges::find_if(displays,
                                                 [id](const auto& display)
                                                 {
                                                     return display->GetID() == id;
                                                 });
        return RESULT != displays.end() ? RESULT->get() : nullptr;
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

    bool DisplaySubsystem::IsDisplayValid(IDisplay::EId id) const noexcept
    {
        return FindDisplayByID(id) != nullptr;
    }
} // namespace Garage
