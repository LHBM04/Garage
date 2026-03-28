#ifndef __GARAGE_ENTITY_HPP__
#define __GARAGE_ENTITY_HPP__

#include <vector>
#include <memory>

#include "garage/base/define.hpp"
#include "garage/base/types.hpp"

#include "object.hpp""

namespace Garage
{
	class Component;

	class GARAGE_API Entity : public Object
	{
    public:
		/**
		 * @brief 지정한 타입의 컴포넌트를 가지고 있는지 여부를 반환합니다.
		 * 
		 * @tparam TComponent 지정할 컴포넌트의 타입
		 * 
		 * @return bool 해당 엔티티가 지정한 타입의 컴포넌트를 가지고 있는지 여부
		 */
		template <class TComponent>
		bool HasComponent() const;

		/**
         * @brief 지정한 타입의 컴포넌트를 해당 엔티티에게 추가합니다.
		 * 
		 * @tparam TComponent 지정할 컴포넌트의 타입
		 */
		template <class TComponent>
		void AddComponent();

		/**
		 * @brief 지정한 타입의 컴포넌트를 반환합니다.
		 * 
		 * @tparam TComponent 지정할 컴포넌트의 타입
		 * 
		 * @return TComponent* 지정한 타입의 컴포넌트
		 */
		template <class TComponent>
        TComponent* GetComponent();

	private:
		/**
		 * @brief 해당 엔티티가 가지고 있는 컴포넌트들.
		 */
		std::vector<std::unique_ptr<Component>> components;
	};
}

#include "entity.inl"

#endif