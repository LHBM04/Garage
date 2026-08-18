#pragma once

#include "../subsystem.h"

namespace Garage
{
    class Window;
    
    /**
     * @class WindowSubsystem
     * @brief 애플리케이션 내 모든 창을 관리 및 제어하는 서브시스템을 정의합니다.
     */
    class WindowSubsystem : public Subsystem
    {
    public:
        /**
         * @brief 새 창을 추가합니다.
         * @param window_ 추가할 창
         */
        virtual void AddWindow(Window* window_);
    };
}
