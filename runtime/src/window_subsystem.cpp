#include "garage/platform/window_subsystem.h"

#include <vector>
#include <SDL3/SDL_events.h>

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
            
            void RemoveWindow(Window* window_) override
            {
                auto result = std::ranges::find(windows, window_);
                if (result != windows.end())
                {
                    windows.erase(result);
                }
            }
            
            void PollEvents() override
            {
                SDL_Event event;
                while (SDL_PollEvent(&event))
                {
                    switch (event.type)
                    {
                        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                        {
                            // auto result = std::ranges::find(windows, window_);
                            // if (result != windows.end())
                            // {
                            //     windows.erase(result);
                            // }
                            
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
