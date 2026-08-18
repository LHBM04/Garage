#pragma once

#include <vector>

#include "garage/platform/window_subsystem.hpp"

namespace Garage
{
    class WindowSubsystem_Impl : public WindowSubsystem
    {
    public:
        ~WindowSubsystem_Impl() noexcept override;
        
        void AddWindow(Window* window_) override;
        void RemoveWindow(Window* window_) override;
        void PollEvents() override;
        
    private:
        /**
         * @brief 관리할 모든 창.
         */
        std::pmr::vector<Window_Impl> windows;
    };
}
