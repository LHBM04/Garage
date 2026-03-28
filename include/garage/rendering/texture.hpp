#ifndef __GARAGE_TEXTURE_HPP__
#define __GARAGE_TEXTURE_HPP__

#include "garage/base/defines.hpp"
#include "garage/base/types.hpp"

#include "garage/resource/resource.hpp"

namespace Garage
{
	/**
	 * @class Texture
	 * 
	 * @brief 텍스처를 정의합니다.
	 */
	class Texture : public Resource
	{
	public:
		/**
		 * @brief 소멸자.
		 */
		virtual ~Texture() = default;

		/**
		 * @brief 해당 텍스처를 바인딩합니다.
		 */
		virtual void Bind() const = 0;

		/**
		 * @brief 해당 텍스처를 언바인딩합니다.
		 */
		virtual void Unbind() const = 0;

	protected:
        /**
         * @brief 해당 텍스쳐의 핸들.
         */
        UInt32 handle{ 0 };

        /**
         * @brief 해당 텍스쳐의 너비.
         */
        Int32 width{ 0 };

        /**
         * @brief 해당 텍스쳐의 높이.
         */
        Int32 height{0};
	};
}

#endif