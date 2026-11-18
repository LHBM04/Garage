#ifndef GARAGE_ACTORS_TERRAIN_COLLIDER_HPP
#define GARAGE_ACTORS_TERRAIN_COLLIDER_HPP

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
    class TerrainAsset;

    class GARAGE_API TerrainCollider final : public Collider
    {
    public:
        using Collider::Collider;
        using Collider::Intersects;
        ~TerrainCollider() noexcept override = default;

        [[nodiscard]] const TerrainAsset* GetTerrain() const noexcept;
        void                              SetTerrain(const TerrainAsset* value) noexcept;

        [[nodiscard]] bool IsValid() const noexcept override;

        [[nodiscard]] BoundingBox GetWorldBounds() const noexcept override;

        [[nodiscard]] bool Intersects(const BoundingSphere& sphere) const noexcept override;
        [[nodiscard]] bool Intersects(const BoundingOrientedBox& box) const noexcept override;

        [[nodiscard]] bool Raycast(const Vector3D<Float32>& origin,
                                   const Vector3D<Float32>& direction,
                                   Float32&                 distance,
                                   Float32 maxDistance = std::numeric_limits<Float32>::max()) const noexcept override;

    private:
        const TerrainAsset* asset = nullptr;
    };
} // namespace Garage

#endif // GARAGE_ACTORS_TERRAIN_COLLIDER_HPP
