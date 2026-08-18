#pragma once

#include <string>

#include "window_flags.hpp"

namespace Garage
{
    /**
     * @struct WindowOptions
     * @brief 애플리케이션 내 창이 가지는 설정을 정의합니다.
     */
    struct WindowOptions final
    {
        /**
         * @brief 생성할 창의 제목.
         */
        std::string title;

        /**
         * @brief 생성할 창의 X 위치.
         */
        int positionX;

        /**
         * @brief 생성할 창의 Y 위치.
         */
        int positionY;

        /**
         * @brief 생성할 창의 너비.
         */
        int sizeX;

        /**
         * @brief 생성할 창의 높이.
         */
        int sizeY;

        /**
         * @brief 생성할 창의 플래그.
         */
        WindowFlags flags;
    };
}
