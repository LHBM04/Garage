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
         * @brief 지정한 창을 관리 목록에 추가합니다.
         * @param window_ 지정할 창
         */
        virtual void AddWindow(Window* window_);

        /**
         * @brief 지정한 창을 제거합니다.
         * @param window_ 지정할 창
         */
        virtual void RemoveWindow(Window* window_);
        
        /**
         * @brief 모든 창의 이벤트를 폴링합니다.
         */
        virtual void PollEvents() = 0;
    };
}
