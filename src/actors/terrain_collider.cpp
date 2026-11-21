#include "garage/actors/terrain_collider.hpp"

#include <algorithm>
#include <cmath>

#include "garage/actors/actor.hpp"

#include "garage/content/terrain_content.hpp"

#include "garage/core/common.hpp"

#include "garage/graphics/terrain.hpp"

#include "garage/math/bounding_box.hpp"
#include "garage/math/bounding_oriented_box.hpp"
#include "garage/math/bounding_sphere.hpp"
#include "garage/math/matrix4x4.hpp"
#include "garage/math/vector3d.hpp"

namespace Garage
{
    namespace
    {
        [[nodiscard]] bool IntersectsRayTriangle(const Vector3D<Float32>& origin,
                                                 const Vector3D<Float32>& direction,
                                                 const Vector3D<Float32>& v0,
                                                 const Vector3D<Float32>& v1,
                                                 const Vector3D<Float32>& v2,
                                                 Float32&                 distance) noexcept
        {
            constexpr Float32 DETERMINANT_EPSILON = 1e-6f;

            const Vector3D<Float32> EDGE1       = v1 - v0;
            const Vector3D<Float32> EDGE2       = v2 - v0;
            const Vector3D<Float32> P_VEC       = Vector3D<Float32>::Cross(direction, EDGE2);
            const Float32           DETERMINANT = Vector3D<Float32>::Dot(EDGE1, P_VEC);
            if (std::abs(DETERMINANT) < DETERMINANT_EPSILON)
            {
                return false;
            }

            const Float32           INV_DETERMINANT = 1.0f / DETERMINANT;
            const Vector3D<Float32> T_VEC           = origin - v0;
            const Float32           U               = Vector3D<Float32>::Dot(T_VEC, P_VEC) * INV_DETERMINANT;
            if (U < 0.0f || U > 1.0f)
            {
                return false;
            }

            const Vector3D<Float32> Q_VEC = Vector3D<Float32>::Cross(T_VEC, EDGE1);
            const Float32           V     = Vector3D<Float32>::Dot(direction, Q_VEC) * INV_DETERMINANT;
            if (V < 0.0f || U + V > 1.0f)
            {
                return false;
            }

            const Float32 CANDIDATE = Vector3D<Float32>::Dot(EDGE2, Q_VEC) * INV_DETERMINANT;
            if (CANDIDATE < 0.0f)
            {
                return false;
            }

            distance = CANDIDATE;
            return true;
        }

        template <class TCallback>
        bool VisitTerrainTriangles(const TerrainContent*     content,
                                   const Matrix4x4<Float32>& world,
                                   const BoundingBox&        worldRegion,
                                   TCallback&&               callback) noexcept
        {
            if (content == nullptr || content->GetWidth() < 2 || content->GetHeight() < 2 ||
                content->GetCellSize() <= 0.0f)
            {
                return false;
            }

            Matrix4x4<Float32> inverse{};
            if (!world.TryGetInverse(inverse))
            {
                return false;
            }

            const BoundingBox       REGION = worldRegion.GetTransformed(inverse);
            const Vector3D<Float32> MIN    = REGION.GetMin();
            const Vector3D<Float32> MAX    = REGION.GetMax();

            const Float32 HALF_WIDTH = content->GetLocalWidth() * 0.5f;
            const Float32 HALF_DEPTH = content->GetLocalDepth() * 0.5f;
            if (MAX.GetX() < -HALF_WIDTH || MIN.GetX() > HALF_WIDTH || MAX.GetZ() < -HALF_DEPTH ||
                MIN.GetZ() > HALF_DEPTH)
            {
                return false;
            }

            const Float32 CELL_SIZE = content->GetCellSize();
            const auto    CELL_INDEX =
                    [](Float32 coordinate, Float32 halfSize, Float32 spacing, UInt32 maxCell, bool lower) -> UInt32
            {
                const Float32 GRID  = (coordinate + halfSize) / spacing;
                const Float32 INDEX = lower ? std::floor(GRID) - 1.0f : std::floor(GRID) + 1.0f;
                return static_cast<UInt32>(std::clamp(INDEX, 0.0f, static_cast<Float32>(maxCell)));
            };

            const UInt32 MIN_X = CELL_INDEX(MIN.GetX(), HALF_WIDTH, CELL_SIZE, content->GetWidth() - 2, true);
            const UInt32 MAX_X = CELL_INDEX(MAX.GetX(), HALF_WIDTH, CELL_SIZE, content->GetWidth() - 2, false);
            const UInt32 MIN_Z = CELL_INDEX(MIN.GetZ(), HALF_DEPTH, CELL_SIZE, content->GetHeight() - 2, true);
            const UInt32 MAX_Z = CELL_INDEX(MAX.GetZ(), HALF_DEPTH, CELL_SIZE, content->GetHeight() - 2, false);

            const auto VERTEX = [&](UInt32 x, UInt32 z) -> Vector3D<Float32>
            {
                return world.MultiplyPoint3x4(Vector3D<Float32>{
                        static_cast<Float32>(x) * CELL_SIZE - HALF_WIDTH,
                        content->GetNormalizedHeightAt(x, z) * content->GetHeightScale(),
                        static_cast<Float32>(z) * CELL_SIZE - HALF_DEPTH,
                });
            };

            for (UInt32 z = MIN_Z; z <= MAX_Z; ++z)
            {
                for (UInt32 x = MIN_X; x <= MAX_X; ++x)
                {
                    const Vector3D<Float32> TOP_LEFT     = VERTEX(x, z);
                    const Vector3D<Float32> TOP_RIGHT    = VERTEX(x + 1, z);
                    const Vector3D<Float32> BOTTOM_LEFT  = VERTEX(x, z + 1);
                    const Vector3D<Float32> BOTTOM_RIGHT = VERTEX(x + 1, z + 1);

                    if (callback(TOP_LEFT, BOTTOM_LEFT, TOP_RIGHT) || callback(TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT))
                    {
                        return true;
                    }
                }
            }

            return false;
        }
    } // namespace

    const TerrainContent* TerrainCollider::GetTerrain() const noexcept
    {
        if (content != nullptr)
        {
            return content;
        }

        const ActorBase* owner   = GetOwner();
        const Terrain*   terrain = (owner != nullptr) ? owner->GetComponent<Terrain>() : nullptr;
        return (terrain != nullptr) ? terrain->GetTerrain() : nullptr;
    }

    void TerrainCollider::SetTerrain(const TerrainContent* value) noexcept
    {
        content = value;
    }

    bool TerrainCollider::IsValid() const noexcept
    {
        const TerrainContent* terrain = GetTerrain();
        return terrain != nullptr && terrain->GetWidth() >= 2 && terrain->GetHeight() >= 2 &&
                std::isfinite(terrain->GetCellSize()) && terrain->GetCellSize() > 0.0f &&
                std::isfinite(terrain->GetHeightScale()) &&
                terrain->GetNormalizedHeights().size() ==
                static_cast<USize>(terrain->GetWidth()) * terrain->GetHeight() &&
                HasValidTransform();
    }

    BoundingBox TerrainCollider::GetWorldBounds() const noexcept
    {
        ASSERT(IsValid());

        const TerrainContent* terrain = GetTerrain();
        ASSERT(terrain != nullptr);

        const Float32 HALF_WIDTH   = terrain->GetLocalWidth() * 0.5f;
        const Float32 HALF_DEPTH   = terrain->GetLocalDepth() * 0.5f;
        const Float32 HEIGHT_SCALE = terrain->GetHeightScale();

        return BoundingBox::CreateFromMinMax(Vector3D<Float32>{-HALF_WIDTH, std::min(0.0f, HEIGHT_SCALE), -HALF_DEPTH},
                                             Vector3D<Float32>{HALF_WIDTH, std::max(0.0f, HEIGHT_SCALE), HALF_DEPTH})
                .GetTransformed(GetWorldMatrix());
    }

    bool TerrainCollider::Intersects(const BoundingSphere& sphere) const noexcept
    {
        if (!IsQueryEnabled() || !sphere.IsValid())
        {
            return false;
        }

        return VisitTerrainTriangles(
                GetTerrain(),
                GetWorldMatrix(),
                BoundingBox::CreateFromSphere(sphere),
                [&](const Vector3D<Float32>& v0, const Vector3D<Float32>& v1, const Vector3D<Float32>& v2) noexcept
                {
                    return sphere.Intersects(v0, v1, v2);
                });
    }

    bool TerrainCollider::Intersects(const BoundingOrientedBox& box) const noexcept
    {
        if (!IsQueryEnabled() || !box.IsValid())
        {
            return false;
        }

        const BoundingOrientedBox::CornerArray CORNERS = box.GetCorners();
        return VisitTerrainTriangles(
                GetTerrain(),
                GetWorldMatrix(),
                BoundingBox::CreateFromPoints(CORNERS),
                [&](const Vector3D<Float32>& v0, const Vector3D<Float32>& v1, const Vector3D<Float32>& v2) noexcept
                {
                    return box.Intersects(v0, v1, v2);
                });
    }

    bool TerrainCollider::Raycast(const Vector3D<Float32>& origin,
                                  const Vector3D<Float32>& direction,
                                  Float32&                 distance,
                                  Float32                  maxDistance) const noexcept
    {
        Vector3D<Float32> normalizedDirection{};
        if (!IsQueryEnabled() || !TryNormalizeRay(origin, direction, maxDistance, normalizedDirection))
        {
            return false;
        }

        const BoundingBox       BOUNDS = GetWorldBounds();
        const Vector3D<Float32> MIN    = BOUNDS.GetMin();
        const Vector3D<Float32> MAX    = BOUNDS.GetMax();

        Float32 enter = 0.0f;
        Float32 exit  = maxDistance;
        for (USize axis = 0; axis < 3; ++axis)
        {
            if (normalizedDirection[axis] == 0.0f)
            {
                if (origin[axis] < MIN[axis] || origin[axis] > MAX[axis])
                {
                    return false;
                }
                continue;
            }

            const Float32 FIRST  = (MIN[axis] - origin[axis]) / normalizedDirection[axis];
            const Float32 SECOND = (MAX[axis] - origin[axis]) / normalizedDirection[axis];
            enter                = std::max(enter, std::min(FIRST, SECOND));
            exit                 = std::min(exit, std::max(FIRST, SECOND));

            if (enter > exit)
            {
                return false;
            }
        }

        const BoundingBox REGION = BoundingBox::CreateFromPoints(origin + normalizedDirection * enter,
                                                                 origin + normalizedDirection * exit);

        bool    hit     = false;
        Float32 closest = maxDistance;
        VisitTerrainTriangles(
                GetTerrain(),
                GetWorldMatrix(),
                REGION,
                [&](const Vector3D<Float32>& v0, const Vector3D<Float32>& v1, const Vector3D<Float32>& v2) noexcept
                {
                    Float32 candidate = 0.0f;
                    if (IntersectsRayTriangle(origin, normalizedDirection, v0, v1, v2, candidate) &&
                        candidate <= closest)
                    {
                        closest = candidate;
                        hit     = true;
                    }
                    return false;
                });

        if (hit)
        {
            distance = closest;
        }

        return hit;
    }
} // namespace Garage
