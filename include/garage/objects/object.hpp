#ifndef __GARAGE_OBJECT_HPP__
#define __GARAGE_OBJECT_HPP__

#include "garage/base/defines.hpp"
#include "garage/base/types.hpp"

namespace Garage
{
	/**
	 * @class Object
	 * 
	 * @brief 오브젝트를 정의합니다.
	 */
	class GARAGE_API Object
	{
	public:
        /**
         * @brief 기본 소멸자.
         */
        virtual ~Object() = default;

		/**
		 * @brief 
		 * 
		 * @param data 
		 */
		virtual void Serialize(const char* data) const = 0;

		/**
		 * @brief 
		 * 
		 * @return 
		 */
		virtual const char* Deserialize() = 0;
	};
}

#endif