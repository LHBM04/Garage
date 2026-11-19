#include "garage/actors/box_collider.hpp"

#include "garage/core/common.hpp"

#include "garage/math/bounding_box.hpp"
#include "garage/math/bounding_oriented_box.hpp"
#include "garage/math/bounding_sphere.hpp"
#include "garage/math/vector3d.hpp"

namespace Garage
{
    Vector3D<Float32> BoxCollider::GetCenter() const noexcept
    {
        return box.GetCenter();
    }

    void BoxCollider::SetCenter(const Vector3D<Float32>& value) noexcept
    {
        ASSERT(value.IsFinite());
        box.SetCenter(value);
    }

    Vector3D<Float32> BoxCollider::GetSize() const noexcept
    {
        return box.GetSize();
    }

    void BoxCollider::SetSize(const Vector3D<Float32>& value) noexcept
    {
        ASSERT(value.IsFinite());
        ASSERT(value.GetX() >= 0.0f && value.GetY() >= 0.0f && value.GetZ() >= 0.0f);
        box.SetExtents(value * 0.5f);
    }

    bool BoxCollider::IsValid() const noexcept
    {
        return box.IsValid() && HasValidTransform();
    }

    BoundingOrientedBox BoxCollider::GetWorldBox() const noexcept
    {
        ASSERT(IsValid());
        return box.GetTransformed(GetWorldMatrix());
    }

    BoundingBox BoxCollider::GetWorldBounds() const noexcept
    {
        const BoundingOrientedBox::CornerArray CORNERS = GetWorldBox().GetCorners();
        return BoundingBox::CreateFromPoints(CORNERS);
    }

    bool BoxCollider::Intersects(const BoundingSphere& sphere) const noexcept
    {
        return IsQueryEnabled() && sphere.IsValid() && GetWorldBox().Intersects(sphere);
    }

    bool BoxCollider::Intersects(const BoundingOrientedBox& other) const noexcept
    {
        return IsQueryEnabled() && other.IsValid() && GetWorldBox().Intersects(other);
    }

    bool BoxCollider::Raycast(const Vector3D<Float32>& origin,
                              const Vector3D<Float32>& direction,
                              Float32&                 distance,
                              Float32                  maxDistance) const noexcept
    {
        Vector3D<Float32> normalizedDirection{};
        if (!IsQueryEnabled() || !TryNormalizeRay(origin, direction, maxDistance, normalizedDirection))
        {
            return false;
        }

        Float32 hitDistance = 0.0f;
        if (!GetWorldBox().Intersects(origin, normalizedDirection, hitDistance) || hitDistance > maxDistance)
        {
            return false;
        }

        distance = hitDistance;
        return true;
    }
} // namespace Garage
