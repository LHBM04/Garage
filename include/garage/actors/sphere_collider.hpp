#ifndef GARAGE_ACTORS_SPHERE_COLLIDER_HPP
#define GARAGE_ACTORS_SPHERE_COLLIDER_HPP

#include <limits>

#include "../core/defines.hpp"
#include "../core/types.hpp"

#include "../math/bounding_box.hpp"
#include "../math/bounding_oriented_box.hpp"
#include "../math/bounding_sphere.hpp"
#include "../math/vector3d.hpp"

#include "collider.hpp"

namespace Garage
{
    class GARAGE_API SphereCollider final : public Collider
    {
    public:
        using Collider::Collider;
        using Collider::Intersects;
        ~SphereCollider() noexcept override = default;

        [[nodiscard]] Vector3D<Float32> GetCenter() const noexcept;
        void                            SetCenter(const Vector3D<Float32>& value) noexcept;

        [[nodiscard]] Float32 GetRadius() const noexcept;
        void                  SetRadius(Float32 value) noexcept;

        [[nodiscard]] bool IsValid() const noexcept override;

        [[nodiscard]] BoundingSphere GetWorldSphere() const noexcept;
        [[nodiscard]] BoundingBox    GetWorldBounds() const noexcept override;

        [[nodiscard]] bool Intersects(const BoundingSphere& other) const noexcept override;
        [[nodiscard]] bool Intersects(const BoundingOrientedBox& box) const noexcept override;

        [[nodiscard]] bool Raycast(const Vector3D<Float32>& origin,
                                   const Vector3D<Float32>& direction,
                                   Float32&                 distance,
                                   Float32 maxDistance = std::numeric_limits<Float32>::max()) const noexcept override;

    private:
        BoundingSphere sphere{Vector3D<Float32>{0.0f, 0.0f, 0.0f}, 0.5f};
    };
} // namespace Garage

#endif // GARAGE_ACTORS_SPHERE_COLLIDER_HPP
