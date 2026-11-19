#ifndef GARAGE_ACTORS_COLLIDER_HPP
#define GARAGE_ACTORS_COLLIDER_HPP

#include <limits>

#include "../core/common.hpp"

#include "../math/bounding_box.hpp"
#include "../math/bounding_oriented_box.hpp"
#include "../math/bounding_sphere.hpp"
#include "../math/matrix4x4.hpp"
#include "../math/vector3d.hpp"

#include "component.hpp"

namespace Garage
{
    class GARAGE_API Collider : public ComponentBase
    {
    public:
        using ComponentBase::ComponentBase;
        ~Collider() noexcept override = default;

        [[nodiscard]] bool         IsQueryEnabled() const noexcept;
        [[nodiscard]] virtual bool IsValid() const noexcept = 0;

        [[nodiscard]] virtual BoundingBox GetWorldBounds() const noexcept = 0;

        [[nodiscard]] bool         Intersects(const Collider* other) const noexcept;
        [[nodiscard]] virtual bool Intersects(const BoundingSphere& sphere) const noexcept   = 0;
        [[nodiscard]] virtual bool Intersects(const BoundingOrientedBox& box) const noexcept = 0;

        [[nodiscard]] virtual bool
                Raycast(const Vector3D<Float32>& origin,
                        const Vector3D<Float32>& direction,
                        Float32&                 distance,
                        Float32                  maxDistance = std::numeric_limits<Float32>::max()) const noexcept = 0;

    protected:
        [[nodiscard]] Matrix4x4<Float32> GetWorldMatrix() const noexcept;
        [[nodiscard]] bool               HasValidTransform() const noexcept;
        [[nodiscard]] static bool        TryNormalizeRay(const Vector3D<Float32>& origin,
                                                         const Vector3D<Float32>& direction,
                                                         Float32                  maxDistance,
                                                         Vector3D<Float32>&       normalizedDirection) noexcept;
    };
} // namespace Garage

#endif // GARAGE_ACTORS_COLLIDER_HPP
