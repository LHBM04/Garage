/**
 * @file Renderer.hpp
 * 
 * @brief 렌더러의 기본 동작을 정의합니다.
 */
#ifndef GARAGE_RENDERER_HPP
#define GARAGE_RENDERER_HPP

#include <cstdint>

namespace Garage
{
	class Renderer
	{
		/**
		 * @brief Specification
		 * 
	     * @brief 렌더러의 사양을 정의합니다.
		 */
		struct Specification final
		{
		    void* window{ nullptr };

			std::uint32_t x{ 0 };
			std::uint32_t y{ 0 };

			std::uint32_t width{ 800 };
			std::uint32_t height{ 600 };
		};

		/**
		 * @brief 렌더러를 초기화합니다.
		 * 
		 * @param specification 렌더러의 사양
		 * 
		 * @return bool 초기화 성공 여부
		 */
        bool Initialize(const Specification& specification) noexcept;

		/**
		 * @brief 
		 */
		void Release();

		/**
         * @brief 화면을 지웁니다.
		 */
		void Clear() noexcept;
	};
}

#endif