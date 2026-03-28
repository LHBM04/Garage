/**
 * @file Renderer.hpp
 * 
 * @brief 렌더러의 기본 동작을 정의합니다.
 */
#ifndef __GARAGE_RENDERDEVICE_HPP__
#define __GARAGE_RENDERDEVICE_HPP__

#include <cstdint>
#include <memory>

#include "garage/base/defines.hpp"
#include "garage/base/types.hpp"

namespace Garage
{
	/**
	 * @class RenderDevice
	 * 
	 * @brief 렌더 디바이스의 기본 동작을 정의합니다.
	 */
	class GARAGE_API RenderDevice
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
             * @brief 렌더러가 사용할 창의 핸들.
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
		virtual ~RenderDevice() = default;

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