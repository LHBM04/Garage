#include "garage/platform/window_subsystem.h"

#include <vector>
#include <SDL3/SDL_events.h>

#include "window_impl.hpp"

namespace Garage
{
    namespace
    {
        class WindowSubsystem_Impl : public WindowSubsystem
        {
        public:
            ~WindowSubsystem_Impl() noexcept override
            {
                
            }
            
            void AddWindow(Window* window_) override
            {
                windows.push_back(window_);
            }
            
            void PollEvents()
            {
                SDL_Event event;
                while (SDL_PollEvent(&event))
                {
                    switch (event.type)
                    {
                        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                        {
                            auto result = std::ranges::find_if(windows, [&event](Window* window) {
                                return SDL_GetWindowID(dynamic_cast<Window_Impl*>(window)->GetNativeHandle()) == event.window.windowID;
                            });
                            if (result == windows.end())
                            {
                                continue;
                            }
                            
                            Window* window = *result;
                            
                            break;
                        }
                        default:
                            break;
                    }
                }
            }
            
        private:
            /**
             * @brief 관리할 모든 창.
             */
            std::pmr::vector<Window*> windows;
        };
    }
}
