#ifndef GARAGE_ACTORS_BOX_COLLIDER_HPP
#define GARAGE_ACTORS_BOX_COLLIDER_HPP

#include <limits>

#include "../core/defines.hpp"
#include "../core/types.hpp"

#include "../math/bounding_box.hpp"
#include "../math/bounding_oriented_box.hpp"
#include "../math/bounding_sphere.hpp"
#include "../math/quaternion.hpp"
#include "../math/vector3d.hpp"

#include "collider.hpp"

namespace Garage
{
    class GARAGE_API BoxCollider final : public Collider
    {
    public:
        using Collider::Collider;
        using Collider::Intersects;
        ~BoxCollider() noexcept override = default;

        [[nodiscard]] Vector3D<Float32> GetCenter() const noexcept;
        void                            SetCenter(const Vector3D<Float32>& value) noexcept;

        [[nodiscard]] Vector3D<Float32> GetSize() const noexcept;
        void                            SetSize(const Vector3D<Float32>& value) noexcept;

        [[nodiscard]] bool IsValid() const noexcept override;

        [[nodiscard]] BoundingOrientedBox GetWorldBox() const noexcept;
        [[nodiscard]] BoundingBox         GetWorldBounds() const noexcept override;

        [[nodiscard]] bool Intersects(const BoundingSphere& sphere) const noexcept override;
        [[nodiscard]] bool Intersects(const BoundingOrientedBox& other) const noexcept override;

        [[nodiscard]] bool Raycast(const Vector3D<Float32>& origin,
                                   const Vector3D<Float32>& direction,
                                   Float32&                 distance,
                                   Float32 maxDistance = std::numeric_limits<Float32>::max()) const noexcept override;

    private:
        BoundingOrientedBox box{Vector3D<Float32>{0.0f, 0.0f, 0.0f},
                                Vector3D<Float32>{0.5f, 0.5f, 0.5f},
                                Quaternion<Float32>::GetIdentity()};
    };
} // namespace Garage

#endif // GARAGE_ACTORS_BOX_COLLIDER_HPP
