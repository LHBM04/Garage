#include "component.hpp"

namespace Garage
{
    template <class TComponent>
    bool Entity::HasComponent() const
    {
        for (Component* component : components)
        {
            if (dynamic_cast<TComponent*>(component))
            {
                return true;
            }
        }
    
        return false;
    }
    
    template <class TComponent>
    void Entity::AddComponent()
    {
    }
    
    template <class TComponent>
    TComponent* Entity::GetComponent()
    {
        for (Component* component : components)
        {
            if (TComponent* casted = dynamic_cast<TComponent*>(component))
            {
                return casted;
            }
        }
    
        return nullptr;
    }
}  // namespace Garage