/**
 * @file Test.hpp
 * 
 * @brief Declares the test application interface.
 */
#ifndef __GARAGE_APPLICATION_HPP__
#define __GARAGE_APPLICATION_HPP__

#include <cstdint>
#include <string>
#include <string_view>

namespace Garage
{
    namespace Application
    {
        /**
         * @struct Specification 
         * 
         * @brief 생성할 창의 사양을 정의합니다.
         */
        struct Specification final
        {
            /**
             * @brief 창의 제목.
             */
            std::string title{ "New Window" };

            /**
             * @brief 창의 너비.
             */
            std::uint32_t width{ 800 };

            /**
             * @brief 창의 높이.
             */
            std::uint32_t height{ 600 };
        };

        /**
         * @brief 애플리케이션을 초기화합니다.
         *
         * @param specification 애플리케이션의 사양
         *
         * @return bool 초기화 성공 여부
         */
        bool Initialize(const Specification& specification) noexcept;

        /**
         * @brief 애플리케이션을 실행합니다.
         */
        void Run() noexcept;
    }
}

#endif