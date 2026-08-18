#pragma once

namespace Garage
{
    /**
     * @enum WindowFlags
     * @brief 애플리케이션 내 창이 가지는 플래그를 정의합니다.
     */
    enum WindowFlags : unsigned char
    {
        /**
         * @brief 일반.
         */
        None,

        /**
         * @brief 전체 화면.
         */
        Fullscreen,

        /**
         * @brief 크기 조절 가능.
         */
        Resizable,

        /**
         * @brief 테두리 여부.
         */
        Borderless,
    };
}
