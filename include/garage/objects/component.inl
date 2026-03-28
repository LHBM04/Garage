#include "entity.hpp"

namespace Garage
{
	Entity* Component::GetOwner() const
	{
        return owner;
	}
}