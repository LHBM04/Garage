#include "garage/math/bounding_box.hpp"

#include <algorithm>
#include <array>
#include <cmath>

#include "garage/core/defines.hpp"
#include "garage/math/bounding_oriented_box.hpp"
#include "garage/math/bounding_sphere.hpp"
#include "garage/math/mathf.hpp"

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
                const Float32 o    = origin[axis];
                const Float32 d    = dir[axis];
                const Float32 bMin = boxMin[axis];
                const Float32 bMax = boxMax[axis];

                if (std::abs(d) <= Mathf::EPSILON)
                {
                    if (o < bMin || o > bMax)
                    {
                        return false;
                    }
                }
                else
                {
                    const Float32 invD = 1.0f / d;
                    Float32       t1   = (bMin - o) * invD;
                    Float32       t2   = (bMax - o) * invD;
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
            const Vector3D<Float32> normal(plane.GetX(), plane.GetY(), plane.GetZ());
            ASSERT(plane.IsFinite());
            const Float32 lenSq = normal.GetSqrMagnitude();
            ASSERT(lenSq > Mathf::EPSILON);

            const Float32 invLen = 1.0f / std::sqrt(lenSq);
            return plane * invLen;
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
            const Vector3D<Float32> normal = Vector3D<Float32>::Cross(v1 - v0, v2 - v0);
            ASSERT(normal.GetSqrMagnitude() > 0.0f);
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
        if (const auto cmp = center <=> other.center; cmp != 0)
        {
            return cmp;
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
        static constexpr std::array<Vector3D<Float32>, CORNER_COUNT> OFFSETS = {Vector3D<Float32>(-1.0f, -1.0f, 1.0f),
                                                                                Vector3D<Float32>(1.0f, -1.0f, 1.0f),
                                                                                Vector3D<Float32>(1.0f, 1.0f, 1.0f),
                                                                                Vector3D<Float32>(-1.0f, 1.0f, 1.0f),
                                                                                Vector3D<Float32>(-1.0f, -1.0f, -1.0f),
                                                                                Vector3D<Float32>(1.0f, -1.0f, -1.0f),
                                                                                Vector3D<Float32>(1.0f, 1.0f, -1.0f),
                                                                                Vector3D<Float32>(-1.0f, 1.0f, -1.0f)};

        for (USize i = 0; i < CORNER_COUNT; ++i)
        {
            corners[i] = center + Vector3D<Float32>::Scale(extents, OFFSETS[i]);
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
        const Quaternion<Float32> normRot = NormalizeRotation(rotation);
        CornerArray               corners = GetCorners();
        for (Vector3D<Float32>& corner : corners)
        {
            corner = (normRot * (corner * scale)) + translation;
        }
        out = CreateFromPoints(corners);
    }

    bool BoundingBox::Contains(const Vector3D<Float32>& point) const noexcept
    {
        const Vector3D<Float32> bMin = GetMin();
        const Vector3D<Float32> bMax = GetMax();
        return point.GetX() >= bMin.GetX() && point.GetX() <= bMax.GetX() && point.GetY() >= bMin.GetY() &&
                point.GetY() <= bMax.GetY() && point.GetZ() >= bMin.GetZ() && point.GetZ() <= bMax.GetZ();
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
        const Vector3D<Float32> bMin = GetMin();
        const Vector3D<Float32> bMax = GetMax();
        const Vector3D<Float32> sc   = sphere.GetCenter();
        const Float32           r    = sphere.GetRadius();

        return sc.GetX() - r >= bMin.GetX() && sc.GetX() + r <= bMax.GetX() && sc.GetY() - r >= bMin.GetY() &&
                sc.GetY() + r <= bMax.GetY() && sc.GetZ() - r >= bMin.GetZ() && sc.GetZ() + r <= bMax.GetZ();
    }

    bool BoundingBox::Contains(const BoundingBox& box) const noexcept
    {
        const Vector3D<Float32> aMin = GetMin();
        const Vector3D<Float32> aMax = GetMax();
        const Vector3D<Float32> bMin = box.GetMin();
        const Vector3D<Float32> bMax = box.GetMax();

        return aMin.GetX() <= bMin.GetX() && bMax.GetX() <= aMax.GetX() && aMin.GetY() <= bMin.GetY() &&
                bMax.GetY() <= aMax.GetY() && aMin.GetZ() <= bMin.GetZ() && bMax.GetZ() <= aMax.GetZ();
    }

    bool BoundingBox::Contains(const BoundingOrientedBox& box) const noexcept
    {
        const CornerArray corners = box.GetCorners();
        for (const Vector3D<Float32>& corner : corners)
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
        const Vector3D<Float32> sc      = sphere.GetCenter();
        const Vector3D<Float32> closest = Vector3D<Float32>::Clamp(sc, GetMin(), GetMax());
        const Float32           distSq  = (sc - closest).GetSqrMagnitude();
        const Float32           r       = sphere.GetRadius();
        return distSq <= r * r;
    }

    bool BoundingBox::Intersects(const BoundingBox& box) const noexcept
    {
        const Vector3D<Float32> aMin = GetMin();
        const Vector3D<Float32> aMax = GetMax();
        const Vector3D<Float32> bMin = box.GetMin();
        const Vector3D<Float32> bMax = box.GetMax();

        return aMax.GetX() >= bMin.GetX() && aMin.GetX() <= bMax.GetX() && aMax.GetY() >= bMin.GetY() &&
                aMin.GetY() <= bMax.GetY() && aMax.GetZ() >= bMin.GetZ() && aMin.GetZ() <= bMax.GetZ();
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
        const BoundingBox triBox = CreateFromPoints(std::array<Vector3D<Float32>, 3>{v0, v1, v2});
        if (!Intersects(triBox))
        {
            return false;
        }

        const Vector3D<Float32> p0 = v0 - center;
        const Vector3D<Float32> p1 = v1 - center;
        const Vector3D<Float32> p2 = v2 - center;
        const Vector3D<Float32> n  = Vector3D<Float32>::Cross(p1 - p0, p2 - p0);
        const Float32           r  = extents.GetX() * std::abs(n.GetX()) + extents.GetY() * std::abs(n.GetY()) +
                extents.GetZ() * std::abs(n.GetZ());
        const Float32 s = Vector3D<Float32>::Dot(n, p0);
        return std::abs(s) <= r;
    }

    bool BoundingBox::Intersects(const Vector4D<Float32>& plane) const noexcept
    {
        const Vector4D<Float32> p = NormalizePlane(plane);
        const Float32           r = extents.GetX() * std::abs(p.GetX()) + extents.GetY() * std::abs(p.GetY()) +
                extents.GetZ() * std::abs(p.GetZ());
        const Float32 s = PlaneSignedDistance(p, center);
        return std::abs(s) <= r;
    }

    bool BoundingBox::Intersects(const Vector3D<Float32>& origin,
                                 const Vector3D<Float32>& direction,
                                 Float32&                 distance) const noexcept
    {
        const Vector3D<Float32> dir = NormalizeDirection(direction);
        return RayIntersectsAABB(origin, dir, GetMin(), GetMax(), distance);
    }

    bool BoundingBox::ContainedBy(const Vector4D<Float32>& plane0,
                                  const Vector4D<Float32>& plane1,
                                  const Vector4D<Float32>& plane2,
                                  const Vector4D<Float32>& plane3,
                                  const Vector4D<Float32>& plane4,
                                  const Vector4D<Float32>& plane5) const noexcept
    {
        const std::array<Vector4D<Float32>, 6> planes = {plane0, plane1, plane2, plane3, plane4, plane5};
        for (const Vector4D<Float32>& plane : planes)
        {
            const Vector4D<Float32> p = NormalizePlane(plane);
            const Float32           r = extents.GetX() * std::abs(p.GetX()) + extents.GetY() * std::abs(p.GetY()) +
                    extents.GetZ() * std::abs(p.GetZ());
            const Float32 s = PlaneSignedDistance(p, center);
            if (s > -r)
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
        const Vector3D<Float32> minPt = Vector3D<Float32>::Min(b1.GetMin(), b2.GetMin());
        const Vector3D<Float32> maxPt = Vector3D<Float32>::Max(b1.GetMax(), b2.GetMax());
        return CreateFromMinMax(minPt, maxPt);
    }

    BoundingBox BoundingBox::CreateFromSphere(const BoundingSphere& sphere) noexcept
    {
        const Float32 r = sphere.GetRadius();
        return BoundingBox(sphere.GetCenter(), Vector3D<Float32>(r, r, r));
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
