#include "garage/math/bounding_sphere.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <compare>

#include "garage/core/common.hpp"

#include "garage/math/bounding_box.hpp"
#include "garage/math/bounding_oriented_box.hpp"
#include "garage/math/mathf.hpp"
#include "garage/math/matrix4x4.hpp"
#include "garage/math/quaternion.hpp"
#include "garage/math/vector3d.hpp"
#include "garage/math/vector4d.hpp"

namespace Garage
{
    namespace
    {
        [[nodiscard]] Vector3D<Float32> ClosestPointOnTriangle(const Vector3D<Float32>& p,
                                                               const Vector3D<Float32>& a,
                                                               const Vector3D<Float32>& b,
                                                               const Vector3D<Float32>& c) noexcept
        {
            const Vector3D<Float32> AB = b - a;
            const Vector3D<Float32> AC = c - a;
            const Vector3D<Float32> AP = p - a;

            const Float32 D1 = Vector3D<Float32>::Dot(AB, AP);
            const Float32 D2 = Vector3D<Float32>::Dot(AC, AP);
            if (D1 <= 0.0f && D2 <= 0.0f)
            {
                return a;
            }

            const Vector3D<Float32> BP = p - b;
            const Float32           D3 = Vector3D<Float32>::Dot(AB, BP);
            const Float32           D4 = Vector3D<Float32>::Dot(AC, BP);
            if (D3 >= 0.0f && D4 <= D3)
            {
                return b;
            }

            const Float32 VC = D1 * D4 - D3 * D2;
            if (VC <= 0.0f && D1 >= 0.0f && D3 <= 0.0f)
            {
                const Float32 V = D1 / (D1 - D3);
                return a + AB * V;
            }

            const Vector3D<Float32> CP = p - c;
            const Float32           D5 = Vector3D<Float32>::Dot(AB, CP);
            const Float32           D6 = Vector3D<Float32>::Dot(AC, CP);
            if (D6 >= 0.0f && D5 <= D6)
            {
                return c;
            }

            const Float32 VB = D5 * D2 - D1 * D6;
            if (VB <= 0.0f && D2 >= 0.0f && D6 <= 0.0f)
            {
                const Float32 W = D2 / (D2 - D6);
                return a + AC * W;
            }

            const Float32 VA = D3 * D6 - D5 * D4;
            if (VA <= 0.0f && (D4 - D3) >= 0.0f && (D5 - D6) >= 0.0f)
            {
                const Float32 W = (D4 - D3) / ((D4 - D3) + (D5 - D6));
                return b + (c - b) * W;
            }

            const Float32 DENOM = 1.0f / (VA + VB + VC);
            const Float32 V     = VB * DENOM;
            const Float32 W     = VC * DENOM;
            return a + AB * V + AC * W;
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
        if (const auto CMP = center <=> other.center; CMP != 0)
        {
            return CMP;
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
        const Vector3D<Float32> NEW_CENTER = matrix.MultiplyPoint3x4(center);
        const Vector3D<Float32> SCL        = matrix.GetScale();
        const Float32           MAX_SCALE  = std::max(SCL.GetX(), std::max(SCL.GetY(), SCL.GetZ()));
        out.center                         = NEW_CENTER;
        out.radius                         = radius * MAX_SCALE;
    }

    void BoundingSphere::Transform(BoundingSphere&            out,
                                   Float32                    scale,
                                   const Quaternion<Float32>& rotation,
                                   const Vector3D<Float32>&   translation) const noexcept
    {
        const Quaternion<Float32> NORM_ROT = NormalizeRotation(rotation);
        out.center                         = (NORM_ROT * (center * scale)) + translation;
        out.radius                         = radius * std::abs(scale);
    }

    bool BoundingSphere::Contains(const Vector3D<Float32>& point) const noexcept
    {
        const Float32 DIST_SQ = (point - center).GetSqrMagnitude();
        return DIST_SQ <= radius * radius;
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
        const Float32 DIST = Vector3D<Float32>::Distance(center, sphere.center);
        return DIST + sphere.radius <= radius;
    }

    bool BoundingSphere::Contains(const BoundingBox& box) const noexcept
    {
        const BoundingBox::CornerArray CORNERS = box.GetCorners();
        for (const Vector3D<Float32>& corner : CORNERS)
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
        const BoundingOrientedBox::CornerArray CORNERS = box.GetCorners();
        for (const Vector3D<Float32>& corner : CORNERS)
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
        const Float32 DIST_SQ = (sphere.center - center).GetSqrMagnitude();
        const Float32 R_SUM   = radius + sphere.radius;
        return DIST_SQ <= R_SUM * R_SUM;
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
        const Vector3D<Float32> CLOSEST = ClosestPointOnTriangle(center, v0, v1, v2);
        return (CLOSEST - center).GetSqrMagnitude() <= radius * radius;
    }

    bool BoundingSphere::Intersects(const Vector4D<Float32>& plane) const noexcept
    {
        const Vector4D<Float32> P    = NormalizePlane(plane);
        const Float32           DIST = PlaneSignedDistance(P, center);
        return std::abs(DIST) <= radius;
    }

    bool BoundingSphere::Intersects(const Vector3D<Float32>& origin,
                                    const Vector3D<Float32>& direction,
                                    Float32&                 distance) const noexcept
    {
        const Vector3D<Float32> DIR = NormalizeDirection(direction);
        const Vector3D<Float32> M   = origin - center;
        const Float32           B   = Vector3D<Float32>::Dot(M, DIR);
        const Float32           C   = Vector3D<Float32>::Dot(M, M) - radius * radius;

        if (C > 0.0f && B > 0.0f)
        {
            return false;
        }

        const Float32 DISCR = B * B - C;
        if (DISCR < 0.0f)
        {
            return false;
        }

        Float32 t = -B - std::sqrt(DISCR);
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
        const std::array<Vector4D<Float32>, 6> PLANES = {plane0, plane1, plane2, plane3, plane4, plane5};
        for (const Vector4D<Float32>& plane : PLANES)
        {
            const Vector4D<Float32> P    = NormalizePlane(plane);
            const Float32           DIST = PlaneSignedDistance(P, center);
            if (DIST > -radius)
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
        const Vector3D<Float32> DIFF = s2.center - s1.center;
        const Float32           DIST = DIFF.GetMagnitude();

        if (DIST + s2.radius <= s1.radius)
        {
            return s1;
        }
        if (DIST + s1.radius <= s2.radius)
        {
            return s2;
        }

        const Float32           NEW_RADIUS = (s1.radius + s2.radius + DIST) * 0.5f;
        const Vector3D<Float32> DIR        = (DIST > Mathf::EPSILON) ? (DIFF / DIST) : Vector3D<Float32>::GetZero();
        const Vector3D<Float32> NEW_C      = s1.center + DIR * (NEW_RADIUS - s1.radius);
        return BoundingSphere(NEW_C, NEW_RADIUS);
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
