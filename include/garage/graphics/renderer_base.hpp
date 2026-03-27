/**
 * @file Renderer.hpp
 * 
 * @brief 렌더러의 기본 동작을 정의합니다.
 */
#ifndef GARAGE_RENDERER_HPP
#define GARAGE_RENDERER_HPP

#include <cstdint>
#include <memory>

namespace Garage
{
	class RendererBase
	{
    public:
		/**
		 * @brief Specification
		 * 
	     * @brief 렌더러의 사양을 정의합니다.
		 */
		struct Specification final
		{
		    /**
		     
		     */
		    void* window{ nullptr };

			/**
			 * @brief 
			 */
			std::uint32_t x{ 0 };

			/**
			 * @brief 
			 */
			std::uint32_t y{ 0 };

			/**
			 * @brief 
			 */
			std::uint32_t width{ 800 };

			/**
			 * @brief 
			 */
			std::uint32_t height{ 600 };
		};

		/**
		 * @brief 소멸자.
		 */
		virtual ~RendererBase();

		/**
		 * @brief 렌더러를 초기화합니다.
		 * 
		 * @param specification 렌더러의 사양
		 * 
		 * @return bool 초기화 성공 여부
		 */
        virtual bool Initialize(const Specification& specification) = 0;

		/**
		 * @brief 렌더러를 해제합니다.
		 */
		virtual void Release() = 0;

		/**
		 * @brief 화면 지우기.
		 */
		virtual void Clear() = 0;
		
		/**
		 * @brief 화면 그리기.
		 */
		virtual void Present() = 0;
	};
}

#endif