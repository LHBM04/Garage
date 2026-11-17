#include "garage/math/bounding_sphere.hpp"

#include <algorithm>
#include <array>
#include <cmath>

#include "garage/core/defines.hpp"

#include "garage/math/bounding_box.hpp"
#include "garage/math/bounding_oriented_box.hpp"
#include "garage/math/mathf.hpp"

namespace Garage
{
    namespace
    {
        [[nodiscard]] Vector3D<Float32> ClosestPointOnTriangle(const Vector3D<Float32>& p,
                                                               const Vector3D<Float32>& a,
                                                               const Vector3D<Float32>& b,
                                                               const Vector3D<Float32>& c) noexcept
        {
            const Vector3D<Float32> ab = b - a;
            const Vector3D<Float32> ac = c - a;
            const Vector3D<Float32> ap = p - a;

            const Float32 d1 = Vector3D<Float32>::Dot(ab, ap);
            const Float32 d2 = Vector3D<Float32>::Dot(ac, ap);
            if (d1 <= 0.0f && d2 <= 0.0f)
            {
                return a;
            }

            const Vector3D<Float32> bp = p - b;
            const Float32           d3 = Vector3D<Float32>::Dot(ab, bp);
            const Float32           d4 = Vector3D<Float32>::Dot(ac, bp);
            if (d3 >= 0.0f && d4 <= d3)
            {
                return b;
            }

            const Float32 vc = d1 * d4 - d3 * d2;
            if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f)
            {
                const Float32 v = d1 / (d1 - d3);
                return a + ab * v;
            }

            const Vector3D<Float32> cp = p - c;
            const Float32           d5 = Vector3D<Float32>::Dot(ab, cp);
            const Float32           d6 = Vector3D<Float32>::Dot(ac, cp);
            if (d6 >= 0.0f && d5 <= d6)
            {
                return c;
            }

            const Float32 vb = d5 * d2 - d1 * d6;
            if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f)
            {
                const Float32 w = d2 / (d2 - d6);
                return a + ac * w;
            }

            const Float32 va = d3 * d6 - d5 * d4;
            if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f)
            {
                const Float32 w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
                return b + (c - b) * w;
            }

            const Float32 denom = 1.0f / (va + vb + vc);
            const Float32 v     = vb * denom;
            const Float32 w     = vc * denom;
            return a + ab * v + ac * w;
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

    BoundingSphere::BoundingSphere() noexcept = default;

    BoundingSphere::BoundingSphere(const Vector3D<Float32>& center, Float32 radius) noexcept
        : center(center)
        , radius(radius)
    {
    }

    bool BoundingSphere::operator==(const BoundingSphere& other) const noexcept
    {
        return center == other.center && std::abs(radius - other.radius) <= Mathf::EPSILON;
    }

    bool BoundingSphere::operator!=(const BoundingSphere& other) const noexcept
    {
        return !(*this == other);
    }

    std::partial_ordering BoundingSphere::operator<=>(const BoundingSphere& other) const noexcept
    {
        if (const auto cmp = center <=> other.center; cmp != 0)
        {
            return cmp;
        }
        return radius <=> other.radius;
    }

    Vector3D<Float32> BoundingSphere::GetCenter() const noexcept
    {
        return center;
    }

    void BoundingSphere::SetCenter(const Vector3D<Float32>& centerValue) noexcept
    {
        center = centerValue;
    }

    Float32 BoundingSphere::GetRadius() const noexcept
    {
        return radius;
    }

    void BoundingSphere::SetRadius(Float32 radiusValue) noexcept
    {
        radius = radiusValue;
    }

    void BoundingSphere::Set(const Vector3D<Float32>& centerValue, Float32 radiusValue) noexcept
    {
        center = centerValue;
        radius = radiusValue;
    }

    bool BoundingSphere::IsFinite() const noexcept
    {
        return center.IsFinite() && std::isfinite(radius);
    }

    bool BoundingSphere::IsValid() const noexcept
    {
        return IsFinite() && radius >= 0.0f;
    }

    BoundingSphere BoundingSphere::GetTransformed(const Matrix4x4<Float32>& matrix) const noexcept
    {
        BoundingSphere out{};
        Transform(out, matrix);
        return out;
    }

    BoundingSphere BoundingSphere::GetTransformed(Float32                    scale,
                                                  const Quaternion<Float32>& rotation,
                                                  const Vector3D<Float32>&   translation) const noexcept
    {
        BoundingSphere out{};
        Transform(out, scale, rotation, translation);
        return out;
    }

    void BoundingSphere::Transform(const Matrix4x4<Float32>& matrix) noexcept
    {
        Transform(*this, matrix);
    }

    void BoundingSphere::Transform(Float32                    scale,
                                   const Quaternion<Float32>& rotation,
                                   const Vector3D<Float32>&   translation) noexcept
    {
        Transform(*this, scale, rotation, translation);
    }

    void BoundingSphere::Transform(BoundingSphere& out, const Matrix4x4<Float32>& matrix) const noexcept
    {
        const Vector3D<Float32> newCenter = matrix.MultiplyPoint3x4(center);
        const Vector3D<Float32> scl       = matrix.GetScale();
        const Float32           maxScale  = std::max(scl.GetX(), std::max(scl.GetY(), scl.GetZ()));
        out.center                        = newCenter;
        out.radius                        = radius * maxScale;
    }

    void BoundingSphere::Transform(BoundingSphere&            out,
                                   Float32                    scale,
                                   const Quaternion<Float32>& rotation,
                                   const Vector3D<Float32>&   translation) const noexcept
    {
        const Quaternion<Float32> normRot = NormalizeRotation(rotation);
        out.center                        = (normRot * (center * scale)) + translation;
        out.radius                        = radius * std::abs(scale);
    }

    bool BoundingSphere::Contains(const Vector3D<Float32>& point) const noexcept
    {
        const Float32 distSq = (point - center).GetSqrMagnitude();
        return distSq <= radius * radius;
    }

    bool BoundingSphere::Contains(const Vector3D<Float32>& v0,
                                  const Vector3D<Float32>& v1,
                                  const Vector3D<Float32>& v2) const noexcept
    {
        AssertNonDegenerateTriangle(v0, v1, v2);
        return Contains(v0) && Contains(v1) && Contains(v2);
    }

    bool BoundingSphere::Contains(const BoundingSphere& sphere) const noexcept
    {
        const Float32 dist = Vector3D<Float32>::Distance(center, sphere.center);
        return dist + sphere.radius <= radius;
    }

    bool BoundingSphere::Contains(const BoundingBox& box) const noexcept
    {
        const BoundingBox::CornerArray corners = box.GetCorners();
        for (const Vector3D<Float32>& corner : corners)
        {
            if (!Contains(corner))
            {
                return false;
            }
        }
        return true;
    }

    bool BoundingSphere::Contains(const BoundingOrientedBox& box) const noexcept
    {
        const BoundingOrientedBox::CornerArray corners = box.GetCorners();
        for (const Vector3D<Float32>& corner : corners)
        {
            if (!Contains(corner))
            {
                return false;
            }
        }
        return true;
    }

    bool BoundingSphere::Intersects(const BoundingSphere& sphere) const noexcept
    {
        const Float32 distSq = (sphere.center - center).GetSqrMagnitude();
        const Float32 rSum   = radius + sphere.radius;
        return distSq <= rSum * rSum;
    }

    bool BoundingSphere::Intersects(const BoundingBox& box) const noexcept
    {
        return box.Intersects(*this);
    }

    bool BoundingSphere::Intersects(const BoundingOrientedBox& box) const noexcept
    {
        return box.Intersects(*this);
    }

    bool BoundingSphere::Intersects(const Vector3D<Float32>& v0,
                                    const Vector3D<Float32>& v1,
                                    const Vector3D<Float32>& v2) const noexcept
    {
        AssertNonDegenerateTriangle(v0, v1, v2);
        const Vector3D<Float32> closest = ClosestPointOnTriangle(center, v0, v1, v2);
        return (closest - center).GetSqrMagnitude() <= radius * radius;
    }

    bool BoundingSphere::Intersects(const Vector4D<Float32>& plane) const noexcept
    {
        const Vector4D<Float32> p    = NormalizePlane(plane);
        const Float32           dist = PlaneSignedDistance(p, center);
        return std::abs(dist) <= radius;
    }

    bool BoundingSphere::Intersects(const Vector3D<Float32>& origin,
                                    const Vector3D<Float32>& direction,
                                    Float32&                 distance) const noexcept
    {
        const Vector3D<Float32> dir = NormalizeDirection(direction);
        const Vector3D<Float32> m   = origin - center;
        const Float32           b   = Vector3D<Float32>::Dot(m, dir);
        const Float32           c   = Vector3D<Float32>::Dot(m, m) - radius * radius;

        if (c > 0.0f && b > 0.0f)
        {
            return false;
        }

        const Float32 discr = b * b - c;
        if (discr < 0.0f)
        {
            return false;
        }

        Float32 t = -b - std::sqrt(discr);
        if (t < 0.0f)
        {
            t = 0.0f;
        }
        distance = t;
        return true;
    }

    bool BoundingSphere::ContainedBy(const Vector4D<Float32>& plane0,
                                     const Vector4D<Float32>& plane1,
                                     const Vector4D<Float32>& plane2,
                                     const Vector4D<Float32>& plane3,
                                     const Vector4D<Float32>& plane4,
                                     const Vector4D<Float32>& plane5) const noexcept
    {
        const std::array<Vector4D<Float32>, 6> planes = {plane0, plane1, plane2, plane3, plane4, plane5};
        for (const Vector4D<Float32>& plane : planes)
        {
            const Vector4D<Float32> p    = NormalizePlane(plane);
            const Float32           dist = PlaneSignedDistance(p, center);
            if (dist > -radius)
            {
                return false;
            }
        }
        return true;
    }

    bool BoundingSphere::IsApproximately(const BoundingSphere& lhs, const BoundingSphere& rhs, Float32 epsilon) noexcept
    {
        return Vector3D<Float32>::IsApproximately(lhs.center, rhs.center, epsilon) &&
                std::abs(lhs.radius - rhs.radius) <= epsilon;
    }

    BoundingSphere BoundingSphere::CreateMerged(const BoundingSphere& s1, const BoundingSphere& s2) noexcept
    {
        const Vector3D<Float32> diff = s2.center - s1.center;
        const Float32           dist = diff.GetMagnitude();

        if (dist + s2.radius <= s1.radius)
        {
            return s1;
        }
        if (dist + s1.radius <= s2.radius)
        {
            return s2;
        }

        const Float32           newRadius = (s1.radius + s2.radius + dist) * 0.5f;
        const Vector3D<Float32> dir       = (dist > Mathf::EPSILON) ? (diff / dist) : Vector3D<Float32>::GetZero();
        const Vector3D<Float32> newC      = s1.center + dir * (newRadius - s1.radius);
        return BoundingSphere(newC, newRadius);
    }

    BoundingSphere BoundingSphere::CreateFromBoundingBox(const BoundingBox& box) noexcept
    {
        return BoundingSphere(box.GetCenter(), box.GetExtents().GetMagnitude());
    }

    BoundingSphere BoundingSphere::CreateFromBoundingBox(const BoundingOrientedBox& box) noexcept
    {
        return BoundingSphere(box.GetCenter(), box.GetExtents().GetMagnitude());
    }

    BoundingSphere BoundingSphere::CreateFromPoints(PointSpan points) noexcept
    {
        return CreateFromBoundingBox(BoundingBox::CreateFromPoints(points));
    }
} // namespace Garage
