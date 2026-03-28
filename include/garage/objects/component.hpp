#ifndef __GARAGE_COMPONENT_HPP__
#define __GARAGE_COMPONENT_HPP__

#include "garage/base/defines.hpp"
#include "garage/base/types.hpp"

#include "object.hpp"

namespace Garage
{
	class Entity;

	/**
     * @class Component
	 * 
	 * @brief 컴포넌트.
	 */
	class GARAGE_API Component : public Object
	{
	public:
		/**
		 * @brief 기본 소멸자.
		 */
		virtual ~Component() = default;

		/**
		 * @brief 해당 컴포넌트의 오너를 반환합니다.
		 * 
		 * @return Entity* 해당 컴포넌트의 오너.
		 */
		inline Entity* GetOwner() const;

    private:
		/**
		 * @brief 해당 컴포넌트의 주인.
		 */
		Entity* owner{ nullptr };
	};
}

#include "component.inl"

#endif