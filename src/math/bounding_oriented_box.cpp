#include "garage/math/bounding_oriented_box.hpp"

#include <algorithm>
#include <array>
#include <cmath>

#include "garage/core/defines.hpp"
#include "garage/math/bounding_box.hpp"
#include "garage/math/bounding_sphere.hpp"
#include "garage/math/mathf.hpp"

namespace Garage
{
    namespace
    {
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

        [[nodiscard]] Float32 PlaneSignedDistance(const Vector4D<Float32>& normalizedPlane,
                                                  const Vector3D<Float32>& point) noexcept
        {
            return normalizedPlane.GetX() * point.GetX() + normalizedPlane.GetY() * point.GetY() +
                    normalizedPlane.GetZ() * point.GetZ() + normalizedPlane.GetW();
        }
    } // namespace

    BoundingOrientedBox::BoundingOrientedBox() noexcept = default;

    BoundingOrientedBox::BoundingOrientedBox(const Vector3D<Float32>&   center,
                                             const Vector3D<Float32>&   extents,
                                             const Quaternion<Float32>& orientation) noexcept
        : center(center)
        , extents(extents)
        , orientation(orientation)
    {
    }

    bool BoundingOrientedBox::operator==(const BoundingOrientedBox& other) const noexcept
    {
        return center == other.center && extents == other.extents &&
                Quaternion<Float32>::IsApproximately(orientation, other.orientation);
    }

    bool BoundingOrientedBox::operator!=(const BoundingOrientedBox& other) const noexcept
    {
        return !(*this == other);
    }

    std::partial_ordering BoundingOrientedBox::operator<=>(const BoundingOrientedBox& other) const noexcept
    {
        if (const auto cmp = center <=> other.center; cmp != 0)
        {
            return cmp;
        }
        if (const auto cmp = extents <=> other.extents; cmp != 0)
        {
            return cmp;
        }
        return orientation <=> other.orientation;
    }

    Vector3D<Float32> BoundingOrientedBox::GetCenter() const noexcept
    {
        return center;
    }

    void BoundingOrientedBox::SetCenter(const Vector3D<Float32>& centerValue) noexcept
    {
        center = centerValue;
    }

    Vector3D<Float32> BoundingOrientedBox::GetExtents() const noexcept
    {
        return extents;
    }

    void BoundingOrientedBox::SetExtents(const Vector3D<Float32>& extentsValue) noexcept
    {
        extents = extentsValue;
    }

    Quaternion<Float32> BoundingOrientedBox::GetOrientation() const noexcept
    {
        return orientation;
    }

    void BoundingOrientedBox::SetOrientation(const Quaternion<Float32>& orientationValue) noexcept
    {
        orientation = orientationValue;
    }

    void BoundingOrientedBox::Set(const Vector3D<Float32>&   centerValue,
                                  const Vector3D<Float32>&   extentsValue,
                                  const Quaternion<Float32>& orientationValue) noexcept
    {
        center      = centerValue;
        extents     = extentsValue;
        orientation = orientationValue;
    }

    Vector3D<Float32> BoundingOrientedBox::GetSize() const noexcept
    {
        return extents * 2.0f;
    }

    bool BoundingOrientedBox::IsFinite() const noexcept
    {
        return center.IsFinite() && extents.IsFinite() && orientation.IsFinite();
    }

    bool BoundingOrientedBox::IsValid() const noexcept
    {
        return IsFinite() && extents.GetX() >= 0.0f && extents.GetY() >= 0.0f && extents.GetZ() >= 0.0f &&
                orientation.GetSqrMagnitude() > Mathf::EPSILON;
    }

    BoundingOrientedBox::CornerArray BoundingOrientedBox::GetCorners() const noexcept
    {
        CornerArray corners = {};
        GetCorners(corners);
        return corners;
    }

    void BoundingOrientedBox::GetCorners(CornerSpan corners) const noexcept
    {
        static constexpr std::array<Vector3D<Float32>, CORNER_COUNT> OFFSETS = {Vector3D<Float32>(-1.0f, -1.0f, 1.0f),
                                                                                Vector3D<Float32>(1.0f, -1.0f, 1.0f),
                                                                                Vector3D<Float32>(1.0f, 1.0f, 1.0f),
                                                                                Vector3D<Float32>(-1.0f, 1.0f, 1.0f),
                                                                                Vector3D<Float32>(-1.0f, -1.0f, -1.0f),
                                                                                Vector3D<Float32>(1.0f, -1.0f, -1.0f),
                                                                                Vector3D<Float32>(1.0f, 1.0f, -1.0f),
                                                                                Vector3D<Float32>(-1.0f, 1.0f, -1.0f)};

        const Quaternion<Float32> normRot = NormalizeRotation(orientation);
        for (USize i = 0; i < CORNER_COUNT; ++i)
        {
            corners[i] = center + (normRot * Vector3D<Float32>::Scale(extents, OFFSETS[i]));
        }
    }

    BoundingOrientedBox BoundingOrientedBox::GetTransformed(const Matrix4x4<Float32>& matrix) const noexcept
    {
        BoundingOrientedBox out{};
        Transform(out, matrix);
        return out;
    }

    BoundingOrientedBox BoundingOrientedBox::GetTransformed(Float32                    scale,
                                                            const Quaternion<Float32>& rotation,
                                                            const Vector3D<Float32>&   translation) const noexcept
    {
        BoundingOrientedBox out{};
        Transform(out, scale, rotation, translation);
        return out;
    }

    void BoundingOrientedBox::Transform(const Matrix4x4<Float32>& matrix) noexcept
    {
        Transform(*this, matrix);
    }

    void BoundingOrientedBox::Transform(Float32                    scale,
                                        const Quaternion<Float32>& rotation,
                                        const Vector3D<Float32>&   translation) noexcept
    {
        Transform(*this, scale, rotation, translation);
    }

    void BoundingOrientedBox::Transform(BoundingOrientedBox& out, const Matrix4x4<Float32>& matrix) const noexcept
    {
        const Vector3D<Float32>   newCenter   = matrix.MultiplyPoint3x4(center);
        const Vector3D<Float32>   matrixScale = matrix.GetScale();
        const Quaternion<Float32> matrixRot   = matrix.GetRotation();
        const Quaternion<Float32> normSelfRot = NormalizeRotation(orientation);
        out.center                            = newCenter;
        out.extents                           = Vector3D<Float32>::Scale(extents, matrixScale);
        out.orientation                       = Quaternion<Float32>::Normalize(normSelfRot * matrixRot);
    }

    void BoundingOrientedBox::Transform(BoundingOrientedBox&       out,
                                        Float32                    scale,
                                        const Quaternion<Float32>& rotation,
                                        const Vector3D<Float32>&   translation) const noexcept
    {
        const Quaternion<Float32> normRot     = NormalizeRotation(rotation);
        const Quaternion<Float32> normSelfRot = NormalizeRotation(orientation);
        out.center                            = (normRot * (center * scale)) + translation;
        out.extents                           = extents * std::abs(scale);
        out.orientation                       = Quaternion<Float32>::Normalize(normSelfRot * normRot);
    }

    bool BoundingOrientedBox::Contains(const Vector3D<Float32>& point) const noexcept
    {
        const Quaternion<Float32> invRot  = Quaternion<Float32>::Conjugate(NormalizeRotation(orientation));
        const Vector3D<Float32>   localPt = invRot * (point - center);
        const BoundingBox         localBox(Vector3D<Float32>::GetZero(), extents);
        return localBox.Contains(localPt);
    }

    bool BoundingOrientedBox::Contains(const Vector3D<Float32>& v0,
                                       const Vector3D<Float32>& v1,
                                       const Vector3D<Float32>& v2) const noexcept
    {
        const Quaternion<Float32> invRot = Quaternion<Float32>::Conjugate(NormalizeRotation(orientation));
        const BoundingBox         localBox(Vector3D<Float32>::GetZero(), extents);
        return localBox.Contains(invRot * (v0 - center), invRot * (v1 - center), invRot * (v2 - center));
    }

    bool BoundingOrientedBox::Contains(const BoundingSphere& sphere) const noexcept
    {
        const Quaternion<Float32> invRot = Quaternion<Float32>::Conjugate(NormalizeRotation(orientation));
        const BoundingSphere      localSphere(invRot * (sphere.GetCenter() - center), sphere.GetRadius());
        const BoundingBox         localBox(Vector3D<Float32>::GetZero(), extents);
        return localBox.Contains(localSphere);
    }

    bool BoundingOrientedBox::Contains(const BoundingBox& box) const noexcept
    {
        return Contains(CreateFromBoundingBox(box));
    }

    bool BoundingOrientedBox::Contains(const BoundingOrientedBox& box) const noexcept
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

    bool BoundingOrientedBox::Intersects(const BoundingSphere& sphere) const noexcept
    {
        const Quaternion<Float32> invRot = Quaternion<Float32>::Conjugate(NormalizeRotation(orientation));
        const BoundingSphere      localSphere(invRot * (sphere.GetCenter() - center), sphere.GetRadius());
        const BoundingBox         localBox(Vector3D<Float32>::GetZero(), extents);
        return localBox.Intersects(localSphere);
    }

    bool BoundingOrientedBox::Intersects(const BoundingBox& box) const noexcept
    {
        return Intersects(CreateFromBoundingBox(box));
    }

    bool BoundingOrientedBox::Intersects(const BoundingOrientedBox& box) const noexcept
    {
        const Quaternion<Float32> rotA = NormalizeRotation(orientation);
        const Quaternion<Float32> rotB = NormalizeRotation(box.orientation);

        const std::array<Vector3D<Float32>, 3> axesA = {rotA * Vector3D<Float32>::GetRight(),
                                                        rotA * Vector3D<Float32>::GetUp(),
                                                        rotA * Vector3D<Float32>::GetForward()};
        const std::array<Vector3D<Float32>, 3> axesB = {rotB * Vector3D<Float32>::GetRight(),
                                                        rotB * Vector3D<Float32>::GetUp(),
                                                        rotB * Vector3D<Float32>::GetForward()};

        const Vector3D<Float32> diff = box.center - center;

        auto testAxis = [&](const Vector3D<Float32>& axis) noexcept -> bool
        {
            if (axis.GetSqrMagnitude() <= Mathf::EPSILON)
            {
                return true;
            }
            const Float32 rA = extents.GetX() * std::abs(Vector3D<Float32>::Dot(axesA[0], axis)) +
                    extents.GetY() * std::abs(Vector3D<Float32>::Dot(axesA[1], axis)) +
                    extents.GetZ() * std::abs(Vector3D<Float32>::Dot(axesA[2], axis));
            const Float32 rB = box.extents.GetX() * std::abs(Vector3D<Float32>::Dot(axesB[0], axis)) +
                    box.extents.GetY() * std::abs(Vector3D<Float32>::Dot(axesB[1], axis)) +
                    box.extents.GetZ() * std::abs(Vector3D<Float32>::Dot(axesB[2], axis));
            const Float32 dist = std::abs(Vector3D<Float32>::Dot(diff, axis));
            return dist <= (rA + rB);
        };

        for (USize i = 0; i < 3; ++i)
        {
            if (!testAxis(axesA[i]) || !testAxis(axesB[i]))
            {
                return false;
            }
        }

        for (USize i = 0; i < 3; ++i)
        {
            for (USize j = 0; j < 3; ++j)
            {
                if (!testAxis(Vector3D<Float32>::Cross(axesA[i], axesB[j])))
                {
                    return false;
                }
            }
        }

        return true;
    }

    bool BoundingOrientedBox::Intersects(const Vector3D<Float32>& v0,
                                         const Vector3D<Float32>& v1,
                                         const Vector3D<Float32>& v2) const noexcept
    {
        const Quaternion<Float32> invRot = Quaternion<Float32>::Conjugate(NormalizeRotation(orientation));
        const BoundingBox         localBox(Vector3D<Float32>::GetZero(), extents);
        return localBox.Intersects(invRot * (v0 - center), invRot * (v1 - center), invRot * (v2 - center));
    }

    bool BoundingOrientedBox::Intersects(const Vector4D<Float32>& plane) const noexcept
    {
        const Vector4D<Float32>   p         = NormalizePlane(plane);
        const Vector3D<Float32>   planeNorm = Vector3D<Float32>(p.GetX(), p.GetY(), p.GetZ());
        const Quaternion<Float32> normRot   = NormalizeRotation(orientation);
        const Vector3D<Float32>   axisX     = normRot * Vector3D<Float32>::GetRight();
        const Vector3D<Float32>   axisY     = normRot * Vector3D<Float32>::GetUp();
        const Vector3D<Float32>   axisZ     = normRot * Vector3D<Float32>::GetForward();

        const Float32 r = extents.GetX() * std::abs(Vector3D<Float32>::Dot(planeNorm, axisX)) +
                extents.GetY() * std::abs(Vector3D<Float32>::Dot(planeNorm, axisY)) +
                extents.GetZ() * std::abs(Vector3D<Float32>::Dot(planeNorm, axisZ));
        const Float32 s = PlaneSignedDistance(p, center);
        return std::abs(s) <= r;
    }

    bool BoundingOrientedBox::Intersects(const Vector3D<Float32>& origin,
                                         const Vector3D<Float32>& direction,
                                         Float32&                 distance) const noexcept
    {
        const Vector3D<Float32>   dir         = NormalizeDirection(direction);
        const Quaternion<Float32> invRot      = Quaternion<Float32>::Conjugate(NormalizeRotation(orientation));
        const Vector3D<Float32>   localOrigin = invRot * (origin - center);
        const Vector3D<Float32>   localDir    = invRot * dir;
        const BoundingBox         localBox(Vector3D<Float32>::GetZero(), extents);
        return localBox.Intersects(localOrigin, localDir, distance);
    }

    bool BoundingOrientedBox::ContainedBy(const Vector4D<Float32>& plane0,
                                          const Vector4D<Float32>& plane1,
                                          const Vector4D<Float32>& plane2,
                                          const Vector4D<Float32>& plane3,
                                          const Vector4D<Float32>& plane4,
                                          const Vector4D<Float32>& plane5) const noexcept
    {
        const std::array<Vector4D<Float32>, 6> planes  = {plane0, plane1, plane2, plane3, plane4, plane5};
        const Quaternion<Float32>              normRot = NormalizeRotation(orientation);
        const Vector3D<Float32>                axisX   = normRot * Vector3D<Float32>::GetRight();
        const Vector3D<Float32>                axisY   = normRot * Vector3D<Float32>::GetUp();
        const Vector3D<Float32>                axisZ   = normRot * Vector3D<Float32>::GetForward();

        for (const Vector4D<Float32>& plane : planes)
        {
            const Vector4D<Float32> p         = NormalizePlane(plane);
            const Vector3D<Float32> planeNorm = Vector3D<Float32>(p.GetX(), p.GetY(), p.GetZ());
            const Float32           r         = extents.GetX() * std::abs(Vector3D<Float32>::Dot(planeNorm, axisX)) +
                    extents.GetY() * std::abs(Vector3D<Float32>::Dot(planeNorm, axisY)) +
                    extents.GetZ() * std::abs(Vector3D<Float32>::Dot(planeNorm, axisZ));
            const Float32 s = PlaneSignedDistance(p, center);
            if (s > -r)
            {
                return false;
            }
        }
        return true;
    }

    bool BoundingOrientedBox::IsApproximately(const BoundingOrientedBox& lhs,
                                              const BoundingOrientedBox& rhs,
                                              Float32                    epsilon) noexcept
    {
        return Vector3D<Float32>::IsApproximately(lhs.center, rhs.center, epsilon) &&
                Vector3D<Float32>::IsApproximately(lhs.extents, rhs.extents, epsilon) &&
                Quaternion<Float32>::IsApproximately(lhs.orientation, rhs.orientation, epsilon);
    }

    BoundingOrientedBox BoundingOrientedBox::CreateFromBoundingBox(const BoundingBox& box) noexcept
    {
        return BoundingOrientedBox(box.GetCenter(), box.GetExtents(), Quaternion<Float32>::GetIdentity());
    }

    BoundingOrientedBox BoundingOrientedBox::CreateFromPoints(PointSpan points) noexcept
    {
        return CreateFromBoundingBox(BoundingBox::CreateFromPoints(points));
    }
} // namespace Garage
