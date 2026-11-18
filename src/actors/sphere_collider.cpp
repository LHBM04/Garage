#include "garage/actors/sphere_collider.hpp"

#include <algorithm>
#include <array>
#include <cmath>

#include "garage/core/defines.hpp"
#include "garage/core/types.hpp"

#include "garage/math/bounding_box.hpp"
#include "garage/math/bounding_oriented_box.hpp"
#include "garage/math/bounding_sphere.hpp"
#include "garage/math/matrix4x4.hpp"
#include "garage/math/vector3d.hpp"

namespace Garage
{
    Vector3D<Float32> SphereCollider::GetCenter() const noexcept
    {
        return sphere.GetCenter();
    }

    void SphereCollider::SetCenter(const Vector3D<Float32>& value) noexcept
    {
        ASSERT(value.IsFinite());
        sphere.SetCenter(value);
    }

    Float32 SphereCollider::GetRadius() const noexcept
    {
        return sphere.GetRadius();
    }

    void SphereCollider::SetRadius(Float32 value) noexcept
    {
        ASSERT(std::isfinite(value));
        ASSERT(value >= 0.0f);
        sphere.SetRadius(value);
    }

    BoundingSphere SphereCollider::GetWorldSphere() const noexcept
    {
        ASSERT(IsValid());
        const Matrix4x4<Float32>               WORLD = GetWorldMatrix();
        const std::array<Vector3D<Float32>, 3> AXES{WORLD.MultiplyVector(Vector3D<Float32>{1.0f, 0.0f, 0.0f}),
                                                    WORLD.MultiplyVector(Vector3D<Float32>{0.0f, 1.0f, 0.0f}),
                                                    WORLD.MultiplyVector(Vector3D<Float32>{0.0f, 0.0f, 1.0f})};

        Float32 scaleSquared = 0.0f;
        for (const Vector3D<Float32>& axis : AXES)
        {
            Float32 rowSum = 0.0f;
            for (const Vector3D<Float32>& other : AXES)
            {
                rowSum += std::abs(Vector3D<Float32>::Dot(axis, other));
            }
            scaleSquared = std::max(scaleSquared, rowSum);
        }

        return BoundingSphere{WORLD.MultiplyPoint3x4(sphere.GetCenter()), sphere.GetRadius() * std::sqrt(scaleSquared)};
    }

    bool SphereCollider::IsValid() const noexcept
    {
        return sphere.IsValid() && HasValidTransform();
    }

    BoundingBox SphereCollider::GetWorldBounds() const noexcept
    {
        return BoundingBox::CreateFromSphere(GetWorldSphere());
    }

    bool SphereCollider::Intersects(const BoundingSphere& other) const noexcept
    {
        return IsQueryEnabled() && other.IsValid() && GetWorldSphere().Intersects(other);
    }

    bool SphereCollider::Intersects(const BoundingOrientedBox& box) const noexcept
    {
        return IsQueryEnabled() && box.IsValid() && GetWorldSphere().Intersects(box);
    }

    bool SphereCollider::Raycast(const Vector3D<Float32>& origin,
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
        if (!GetWorldSphere().Intersects(origin, normalizedDirection, hitDistance) || hitDistance > maxDistance)
        {
            return false;
        }

        distance = hitDistance;
        return true;
    }
} // namespace Garage
