#include "garage/actors/collider.hpp"

#include <cmath>

#include "garage/actors/actor.hpp"
#include "garage/actors/box_collider.hpp"
#include "garage/actors/sphere_collider.hpp"
#include "garage/actors/transform.hpp"

#include "garage/core/common.hpp"

#include "garage/math/bounding_box.hpp"
#include "garage/math/matrix4x4.hpp"
#include "garage/math/vector3d.hpp"

namespace Garage
{
    bool Collider::IsQueryEnabled() const noexcept
    {
        const ActorBase* owner = GetOwner();
        return IsEnabled() && !IsDestroyed() && owner != nullptr && owner->IsEnabled() && !owner->IsDestroyed() &&
                IsValid();
    }

    bool Collider::Intersects(const Collider* other) const noexcept
    {
        if (other == nullptr || other == this || !IsQueryEnabled() || !other->IsQueryEnabled())
        {
            return false;
        }

        if (!GetWorldBounds().Intersects(other->GetWorldBounds()))
        {
            return false;
        }

        if (const auto* sphere = dynamic_cast<const SphereCollider*>(other))
        {
            return Intersects(sphere->GetWorldSphere());
        }
        if (const auto* box = dynamic_cast<const BoxCollider*>(other))
        {
            return Intersects(box->GetWorldBox());
        }

        if (const auto* sphere = dynamic_cast<const SphereCollider*>(this))
        {
            return other->Intersects(sphere->GetWorldSphere());
        }
        if (const auto* box = dynamic_cast<const BoxCollider*>(this))
        {
            return other->Intersects(box->GetWorldBox());
        }

        return false;
    }

    Matrix4x4<Float32> Collider::GetWorldMatrix() const noexcept
    {
        const ActorBase* owner     = GetOwner();
        const Transform* transform = (owner != nullptr) ? owner->GetComponent<Transform>() : nullptr;
        return (transform != nullptr) ? transform->GetWorldMatrix() : Matrix4x4<Float32>::GetIdentity();
    }

    bool Collider::HasValidTransform() const noexcept
    {
        const Matrix4x4<Float32> WORLD = GetWorldMatrix();
        Matrix4x4<Float32>       inverse{};
        return WORLD[3] == 0.0f && WORLD[7] == 0.0f && WORLD[11] == 0.0f && WORLD[15] == 1.0f &&
                WORLD.TryGetInverse(inverse);
    }

    bool Collider::TryNormalizeRay(const Vector3D<Float32>& origin,
                                   const Vector3D<Float32>& direction,
                                   Float32                  maxDistance,
                                   Vector3D<Float32>&       normalizedDirection) noexcept
    {
        const Float32 LENGTH_SQUARED = direction.GetSqrMagnitude();
        if (!origin.IsFinite() || !direction.IsFinite() || !std::isfinite(LENGTH_SQUARED) || LENGTH_SQUARED <= 0.0f ||
            !std::isfinite(maxDistance) || maxDistance < 0.0f)
        {
            return false;
        }

        normalizedDirection = direction / std::sqrt(LENGTH_SQUARED);
        return true;
    }
} // namespace Garage
