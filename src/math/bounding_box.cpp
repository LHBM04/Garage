#include "garage/math/bounding_box.hpp"

#include <algorithm>
#include <cmath>
#include <compare>

#include "garage/core/common.hpp"
#include "garage/core/common/containers.hpp"

#include "garage/math/bounding_oriented_box.hpp"
#include "garage/math/bounding_sphere.hpp"
#include "garage/math/mathf.hpp"
#include "garage/math/matrix4x4.hpp"
#include "garage/math/quaternion.hpp"
#include "garage/math/vector3d.hpp"
#include "garage/math/vector4d.hpp"

namespace Garage
{
    namespace
    {
        [[nodiscard]] bool RayIntersectsAABB(const Vector3D<Float32>& origin,
                                             const Vector3D<Float32>& dir,
                                             const Vector3D<Float32>& boxMin,
                                             const Vector3D<Float32>& boxMax,
                                             Float32&                 outDist) noexcept
        {
            Float32 tMin = 0.0f;
            Float32 tMax = Mathf::MAX_VALUE;

            for (USize axis = 0; axis < 3; ++axis)
            {
                const Float32 O     = origin[axis];
                const Float32 D     = dir[axis];
                const Float32 B_MIN = boxMin[axis];
                const Float32 B_MAX = boxMax[axis];

                if (std::abs(D) <= Mathf::EPSILON)
                {
                    if (O < B_MIN || O > B_MAX)
                    {
                        return false;
                    }
                }
                else
                {
                    const Float32 INV_D = 1.0f / D;
                    Float32       t1    = (B_MIN - O) * INV_D;
                    Float32       t2    = (B_MAX - O) * INV_D;
                    if (t1 > t2)
                    {
                        std::swap(t1, t2);
                    }
                    tMin = std::max(tMin, t1);
                    tMax = std::min(tMax, t2);
                    if (tMin > tMax)
                    {
                        return false;
                    }
                }
            }

            outDist = tMin;
            return true;
        }

        [[nodiscard]] Vector4D<Float32> NormalizePlane(const Vector4D<Float32>& plane) noexcept
        {
            const Vector3D<Float32> NORMAL(plane.GetX(), plane.GetY(), plane.GetZ());
            ASSERT(plane.IsFinite());
            const Float32 LEN_SQ = NORMAL.GetSqrMagnitude();
            ASSERT(LEN_SQ > Mathf::EPSILON);

            const Float32 INV_LEN = 1.0f / std::sqrt(LEN_SQ);
            return plane * INV_LEN;
        }

        [[nodiscard]] Vector3D<Float32> NormalizeDirection(const Vector3D<Float32>& direction) noexcept
        {
            ASSERT(direction.IsFinite());
            ASSERT(direction.GetSqrMagnitude() > Mathf::EPSILON);

            return Vector3D<Float32>::Normalize(direction);
        }

        [[nodiscard]] Quaternion<Float32> NormalizeRotation(const Quaternion<Float32>& rotation) noexcept
        {
            ASSERT(rotation.IsFinite());
            ASSERT(rotation.GetSqrMagnitude() > Mathf::EPSILON);

            return Quaternion<Float32>::Normalize(rotation);
        }

        void AssertNonDegenerateTriangle(const Vector3D<Float32>& v0,
                                         const Vector3D<Float32>& v1,
                                         const Vector3D<Float32>& v2) noexcept
        {
            const Vector3D<Float32> NORMAL = Vector3D<Float32>::Cross(v1 - v0, v2 - v0);
            ASSERT(NORMAL.GetSqrMagnitude() > 0.0f);
        }

        [[nodiscard]] Float32 PlaneSignedDistance(const Vector4D<Float32>& normalizedPlane,
                                                  const Vector3D<Float32>& point) noexcept
        {
            return normalizedPlane.GetX() * point.GetX() + normalizedPlane.GetY() * point.GetY() +
                    normalizedPlane.GetZ() * point.GetZ() + normalizedPlane.GetW();
        }
    } // namespace

    BoundingBox::BoundingBox() noexcept = default;

    BoundingBox::BoundingBox(const Vector3D<Float32>& center, const Vector3D<Float32>& extents) noexcept
        : center(center)
        , extents(extents)
    {
    }

    bool BoundingBox::operator==(const BoundingBox& other) const noexcept
    {
        return center == other.center && extents == other.extents;
    }

    bool BoundingBox::operator!=(const BoundingBox& other) const noexcept
    {
        return !(*this == other);
    }

    std::partial_ordering BoundingBox::operator<=>(const BoundingBox& other) const noexcept
    {
        if (const auto CMP = center <=> other.center; CMP != 0)
        {
            return CMP;
        }
        return extents <=> other.extents;
    }

    Vector3D<Float32> BoundingBox::GetCenter() const noexcept
    {
        return center;
    }

    void BoundingBox::SetCenter(const Vector3D<Float32>& centerValue) noexcept
    {
        center = centerValue;
    }

    Vector3D<Float32> BoundingBox::GetExtents() const noexcept
    {
        return extents;
    }

    void BoundingBox::SetExtents(const Vector3D<Float32>& extentsValue) noexcept
    {
        extents = extentsValue;
    }

    void BoundingBox::Set(const Vector3D<Float32>& centerValue, const Vector3D<Float32>& extentsValue) noexcept
    {
        center  = centerValue;
        extents = extentsValue;
    }

    Vector3D<Float32> BoundingBox::GetMin() const noexcept
    {
        return center - extents;
    }

    Vector3D<Float32> BoundingBox::GetMax() const noexcept
    {
        return center + extents;
    }

    Vector3D<Float32> BoundingBox::GetSize() const noexcept
    {
        return extents * 2.0f;
    }

    void BoundingBox::SetMinMax(const Vector3D<Float32>& min, const Vector3D<Float32>& max) noexcept
    {
        center  = (min + max) * 0.5f;
        extents = (max - min) * 0.5f;
    }

    bool BoundingBox::IsFinite() const noexcept
    {
        return center.IsFinite() && extents.IsFinite();
    }

    bool BoundingBox::IsValid() const noexcept
    {
        return IsFinite() && extents.GetX() >= 0.0f && extents.GetY() >= 0.0f && extents.GetZ() >= 0.0f;
    }

    BoundingBox::CornerArray BoundingBox::GetCorners() const noexcept
    {
        CornerArray corners = {};
        GetCorners(corners);
        return corners;
    }

    void BoundingBox::GetCorners(CornerSpan corners) const noexcept
    {
        static constexpr Array<Array<Float32, 3>, CORNER_COUNT> OFFSETS = {
                Array<Float32, 3>{-1.0f, -1.0f, 1.0f},
                Array<Float32, 3>{1.0f, -1.0f, 1.0f},
                Array<Float32, 3>{1.0f, 1.0f, 1.0f},
                Array<Float32, 3>{-1.0f, 1.0f, 1.0f},
                Array<Float32, 3>{-1.0f, -1.0f, -1.0f},
                Array<Float32, 3>{1.0f, -1.0f, -1.0f},
                Array<Float32, 3>{1.0f, 1.0f, -1.0f},
                Array<Float32, 3>{-1.0f, 1.0f, -1.0f},
        };

        for (USize i = 0; i < CORNER_COUNT; ++i)
        {
            const Vector3D<Float32> OFFSET(OFFSETS[i][0], OFFSETS[i][1], OFFSETS[i][2]);
            corners[i] = center + Vector3D<Float32>::Scale(extents, OFFSET);
        }
    }

    BoundingBox BoundingBox::GetTransformed(const Matrix4x4<Float32>& matrix) const noexcept
    {
        BoundingBox out{};
        Transform(out, matrix);
        return out;
    }

    BoundingBox BoundingBox::GetTransformed(Float32                    scale,
                                            const Quaternion<Float32>& rotation,
                                            const Vector3D<Float32>&   translation) const noexcept
    {
        BoundingBox out{};
        Transform(out, scale, rotation, translation);
        return out;
    }

    void BoundingBox::Transform(const Matrix4x4<Float32>& matrix) noexcept
    {
        Transform(*this, matrix);
    }

    void BoundingBox::Transform(Float32                    scale,
                                const Quaternion<Float32>& rotation,
                                const Vector3D<Float32>&   translation) noexcept
    {
        Transform(*this, scale, rotation, translation);
    }

    void BoundingBox::Transform(BoundingBox& out, const Matrix4x4<Float32>& matrix) const noexcept
    {
        CornerArray corners = GetCorners();
        for (Vector3D<Float32>& corner : corners)
        {
            corner = matrix.MultiplyPoint3x4(corner);
        }
        out = CreateFromPoints(corners);
    }

    void BoundingBox::Transform(BoundingBox&               out,
                                Float32                    scale,
                                const Quaternion<Float32>& rotation,
                                const Vector3D<Float32>&   translation) const noexcept
    {
        const Quaternion<Float32> NORM_ROT = NormalizeRotation(rotation);
        CornerArray               corners  = GetCorners();
        for (Vector3D<Float32>& corner : corners)
        {
            corner = (NORM_ROT * (corner * scale)) + translation;
        }
        out = CreateFromPoints(corners);
    }

    bool BoundingBox::Contains(const Vector3D<Float32>& point) const noexcept
    {
        const Vector3D<Float32> B_MIN = GetMin();
        const Vector3D<Float32> B_MAX = GetMax();
        return point.GetX() >= B_MIN.GetX() && point.GetX() <= B_MAX.GetX() && point.GetY() >= B_MIN.GetY() &&
                point.GetY() <= B_MAX.GetY() && point.GetZ() >= B_MIN.GetZ() && point.GetZ() <= B_MAX.GetZ();
    }

    bool BoundingBox::Contains(const Vector3D<Float32>& v0,
                               const Vector3D<Float32>& v1,
                               const Vector3D<Float32>& v2) const noexcept
    {
        AssertNonDegenerateTriangle(v0, v1, v2);
        return Contains(v0) && Contains(v1) && Contains(v2);
    }

    bool BoundingBox::Contains(const BoundingSphere& sphere) const noexcept
    {
        const Vector3D<Float32> B_MIN = GetMin();
        const Vector3D<Float32> B_MAX = GetMax();
        const Vector3D<Float32> SC    = sphere.GetCenter();
        const Float32           R     = sphere.GetRadius();

        return SC.GetX() - R >= B_MIN.GetX() && SC.GetX() + R <= B_MAX.GetX() && SC.GetY() - R >= B_MIN.GetY() &&
                SC.GetY() + R <= B_MAX.GetY() && SC.GetZ() - R >= B_MIN.GetZ() && SC.GetZ() + R <= B_MAX.GetZ();
    }

    bool BoundingBox::Contains(const BoundingBox& box) const noexcept
    {
        const Vector3D<Float32> A_MIN = GetMin();
        const Vector3D<Float32> A_MAX = GetMax();
        const Vector3D<Float32> B_MIN = box.GetMin();
        const Vector3D<Float32> B_MAX = box.GetMax();

        return A_MIN.GetX() <= B_MIN.GetX() && B_MAX.GetX() <= A_MAX.GetX() && A_MIN.GetY() <= B_MIN.GetY() &&
                B_MAX.GetY() <= A_MAX.GetY() && A_MIN.GetZ() <= B_MIN.GetZ() && B_MAX.GetZ() <= A_MAX.GetZ();
    }

    bool BoundingBox::Contains(const BoundingOrientedBox& box) const noexcept
    {
        const CornerArray CORNERS = box.GetCorners();
        for (const Vector3D<Float32>& corner : CORNERS)
        {
            if (!Contains(corner))
            {
                return false;
            }
        }
        return true;
    }

    bool BoundingBox::Intersects(const BoundingSphere& sphere) const noexcept
    {
        const Vector3D<Float32> SC      = sphere.GetCenter();
        const Vector3D<Float32> CLOSEST = Vector3D<Float32>::Clamp(SC, GetMin(), GetMax());
        const Float32           DIST_SQ = (SC - CLOSEST).GetSqrMagnitude();
        const Float32           R       = sphere.GetRadius();
        return DIST_SQ <= R * R;
    }

    bool BoundingBox::Intersects(const BoundingBox& box) const noexcept
    {
        const Vector3D<Float32> A_MIN = GetMin();
        const Vector3D<Float32> A_MAX = GetMax();
        const Vector3D<Float32> B_MIN = box.GetMin();
        const Vector3D<Float32> B_MAX = box.GetMax();

        return A_MAX.GetX() >= B_MIN.GetX() && A_MIN.GetX() <= B_MAX.GetX() && A_MAX.GetY() >= B_MIN.GetY() &&
                A_MIN.GetY() <= B_MAX.GetY() && A_MAX.GetZ() >= B_MIN.GetZ() && A_MIN.GetZ() <= B_MAX.GetZ();
    }

    bool BoundingBox::Intersects(const BoundingOrientedBox& box) const noexcept
    {
        return box.Intersects(*this);
    }

    bool BoundingBox::Intersects(const Vector3D<Float32>& v0,
                                 const Vector3D<Float32>& v1,
                                 const Vector3D<Float32>& v2) const noexcept
    {
        AssertNonDegenerateTriangle(v0, v1, v2);
        const BoundingBox TRI_BOX = CreateFromPoints(Array<Vector3D<Float32>, 3>{v0, v1, v2});
        if (!Intersects(TRI_BOX))
        {
            return false;
        }

        const Vector3D<Float32> P0 = v0 - center;
        const Vector3D<Float32> P1 = v1 - center;
        const Vector3D<Float32> P2 = v2 - center;
        const Vector3D<Float32> N  = Vector3D<Float32>::Cross(P1 - P0, P2 - P0);
        const Float32           R  = extents.GetX() * std::abs(N.GetX()) + extents.GetY() * std::abs(N.GetY()) +
                extents.GetZ() * std::abs(N.GetZ());
        const Float32 S = Vector3D<Float32>::Dot(N, P0);
        return std::abs(S) <= R;
    }

    bool BoundingBox::Intersects(const Vector4D<Float32>& plane) const noexcept
    {
        const Vector4D<Float32> P = NormalizePlane(plane);
        const Float32           R = extents.GetX() * std::abs(P.GetX()) + extents.GetY() * std::abs(P.GetY()) +
                extents.GetZ() * std::abs(P.GetZ());
        const Float32 S = PlaneSignedDistance(P, center);
        return std::abs(S) <= R;
    }

    bool BoundingBox::Intersects(const Vector3D<Float32>& origin,
                                 const Vector3D<Float32>& direction,
                                 Float32&                 distance) const noexcept
    {
        const Vector3D<Float32> DIR = NormalizeDirection(direction);
        return RayIntersectsAABB(origin, DIR, GetMin(), GetMax(), distance);
    }

    bool BoundingBox::ContainedBy(const Vector4D<Float32>& plane0,
                                  const Vector4D<Float32>& plane1,
                                  const Vector4D<Float32>& plane2,
                                  const Vector4D<Float32>& plane3,
                                  const Vector4D<Float32>& plane4,
                                  const Vector4D<Float32>& plane5) const noexcept
    {
        const Array<Vector4D<Float32>, 6> PLANES = {plane0, plane1, plane2, plane3, plane4, plane5};
        for (const Vector4D<Float32>& plane : PLANES)
        {
            const Vector4D<Float32> P = NormalizePlane(plane);
            const Float32           R = extents.GetX() * std::abs(P.GetX()) + extents.GetY() * std::abs(P.GetY()) +
                    extents.GetZ() * std::abs(P.GetZ());
            const Float32 S = PlaneSignedDistance(P, center);
            if (S > -R)
            {
                return false;
            }
        }
        return true;
    }

    bool BoundingBox::IsApproximately(const BoundingBox& lhs, const BoundingBox& rhs, Float32 epsilon) noexcept
    {
        return Vector3D<Float32>::IsApproximately(lhs.center, rhs.center, epsilon) &&
                Vector3D<Float32>::IsApproximately(lhs.extents, rhs.extents, epsilon);
    }

    BoundingBox BoundingBox::CreateMerged(const BoundingBox& b1, const BoundingBox& b2) noexcept
    {
        const Vector3D<Float32> MIN_PT = Vector3D<Float32>::Min(b1.GetMin(), b2.GetMin());
        const Vector3D<Float32> MAX_PT = Vector3D<Float32>::Max(b1.GetMax(), b2.GetMax());
        return CreateFromMinMax(MIN_PT, MAX_PT);
    }

    BoundingBox BoundingBox::CreateFromSphere(const BoundingSphere& sphere) noexcept
    {
        const Float32 R = sphere.GetRadius();
        return BoundingBox(sphere.GetCenter(), Vector3D<Float32>(R, R, R));
    }

    BoundingBox BoundingBox::CreateFromMinMax(const Vector3D<Float32>& min, const Vector3D<Float32>& max) noexcept
    {
        BoundingBox box{};
        box.SetMinMax(min, max);
        return box;
    }

    BoundingBox BoundingBox::CreateFromPoints(const Vector3D<Float32>& point1, const Vector3D<Float32>& point2) noexcept
    {
        return CreateFromMinMax(Vector3D<Float32>::Min(point1, point2), Vector3D<Float32>::Max(point1, point2));
    }

    BoundingBox BoundingBox::CreateFromPoints(PointSpan points) noexcept
    {
        ASSERT(!points.empty());
        Vector3D<Float32> minPt = points[0];
        Vector3D<Float32> maxPt = points[0];
        for (const Vector3D<Float32>& pt : points)
        {
            minPt = Vector3D<Float32>::Min(minPt, pt);
            maxPt = Vector3D<Float32>::Max(maxPt, pt);
        }
        return CreateFromMinMax(minPt, maxPt);
    }
} // namespace Garage
