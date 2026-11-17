#include "garage/math/bounding_oriented_box.hpp"

#include <array>
#include <cmath>
#include <compare>

#include "garage/core/defines.hpp"
#include "garage/core/types.hpp"

#include "garage/math/bounding_box.hpp"
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
        if (const auto CMP = center <=> other.center; CMP != 0)
        {
            return CMP;
        }
        if (const auto CMP = extents <=> other.extents; CMP != 0)
        {
            return CMP;
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
        static constexpr std::array<std::array<Float32, 3>, CORNER_COUNT> OFFSETS = {
                std::array<Float32, 3>{-1.0f, -1.0f, 1.0f},
                std::array<Float32, 3>{1.0f, -1.0f, 1.0f},
                std::array<Float32, 3>{1.0f, 1.0f, 1.0f},
                std::array<Float32, 3>{-1.0f, 1.0f, 1.0f},
                std::array<Float32, 3>{-1.0f, -1.0f, -1.0f},
                std::array<Float32, 3>{1.0f, -1.0f, -1.0f},
                std::array<Float32, 3>{1.0f, 1.0f, -1.0f},
                std::array<Float32, 3>{-1.0f, 1.0f, -1.0f},
        };

        const Quaternion<Float32> NORM_ROT = NormalizeRotation(orientation);
        for (USize i = 0; i < CORNER_COUNT; ++i)
        {
            const Vector3D<Float32> OFFSET(OFFSETS[i][0], OFFSETS[i][1], OFFSETS[i][2]);
            corners[i] = center + (NORM_ROT * Vector3D<Float32>::Scale(extents, OFFSET));
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
        const Vector3D<Float32>   NEW_CENTER    = matrix.MultiplyPoint3x4(center);
        const Vector3D<Float32>   MATRIX_SCALE  = matrix.GetScale();
        const Quaternion<Float32> MATRIX_ROT    = matrix.GetRotation();
        const Quaternion<Float32> NORM_SELF_ROT = NormalizeRotation(orientation);
        out.center                              = NEW_CENTER;
        out.extents                             = Vector3D<Float32>::Scale(extents, MATRIX_SCALE);
        out.orientation                         = Quaternion<Float32>::Normalize(NORM_SELF_ROT * MATRIX_ROT);
    }

    void BoundingOrientedBox::Transform(BoundingOrientedBox&       out,
                                        Float32                    scale,
                                        const Quaternion<Float32>& rotation,
                                        const Vector3D<Float32>&   translation) const noexcept
    {
        const Quaternion<Float32> NORM_ROT      = NormalizeRotation(rotation);
        const Quaternion<Float32> NORM_SELF_ROT = NormalizeRotation(orientation);
        out.center                              = (NORM_ROT * (center * scale)) + translation;
        out.extents                             = extents * std::abs(scale);
        out.orientation                         = Quaternion<Float32>::Normalize(NORM_SELF_ROT * NORM_ROT);
    }

    bool BoundingOrientedBox::Contains(const Vector3D<Float32>& point) const noexcept
    {
        const Quaternion<Float32> INV_ROT  = Quaternion<Float32>::Conjugate(NormalizeRotation(orientation));
        const Vector3D<Float32>   LOCAL_PT = INV_ROT * (point - center);
        const BoundingBox         LOCAL_BOX(Vector3D<Float32>::GetZero(), extents);
        return LOCAL_BOX.Contains(LOCAL_PT);
    }

    bool BoundingOrientedBox::Contains(const Vector3D<Float32>& v0,
                                       const Vector3D<Float32>& v1,
                                       const Vector3D<Float32>& v2) const noexcept
    {
        const Quaternion<Float32> INV_ROT = Quaternion<Float32>::Conjugate(NormalizeRotation(orientation));
        const BoundingBox         LOCAL_BOX(Vector3D<Float32>::GetZero(), extents);
        return LOCAL_BOX.Contains(INV_ROT * (v0 - center), INV_ROT * (v1 - center), INV_ROT * (v2 - center));
    }

    bool BoundingOrientedBox::Contains(const BoundingSphere& sphere) const noexcept
    {
        const Quaternion<Float32> INV_ROT = Quaternion<Float32>::Conjugate(NormalizeRotation(orientation));
        const BoundingSphere      LOCAL_SPHERE(INV_ROT * (sphere.GetCenter() - center), sphere.GetRadius());
        const BoundingBox         LOCAL_BOX(Vector3D<Float32>::GetZero(), extents);
        return LOCAL_BOX.Contains(LOCAL_SPHERE);
    }

    bool BoundingOrientedBox::Contains(const BoundingBox& box) const noexcept
    {
        return Contains(CreateFromBoundingBox(box));
    }

    bool BoundingOrientedBox::Contains(const BoundingOrientedBox& box) const noexcept
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

    bool BoundingOrientedBox::Intersects(const BoundingSphere& sphere) const noexcept
    {
        const Quaternion<Float32> INV_ROT = Quaternion<Float32>::Conjugate(NormalizeRotation(orientation));
        const BoundingSphere      LOCAL_SPHERE(INV_ROT * (sphere.GetCenter() - center), sphere.GetRadius());
        const BoundingBox         LOCAL_BOX(Vector3D<Float32>::GetZero(), extents);
        return LOCAL_BOX.Intersects(LOCAL_SPHERE);
    }

    bool BoundingOrientedBox::Intersects(const BoundingBox& box) const noexcept
    {
        return Intersects(CreateFromBoundingBox(box));
    }

    bool BoundingOrientedBox::Intersects(const BoundingOrientedBox& box) const noexcept
    {
        const Quaternion<Float32> ROT_A = NormalizeRotation(orientation);
        const Quaternion<Float32> ROT_B = NormalizeRotation(box.orientation);

        const std::array<Vector3D<Float32>, 3> AXES_A = {ROT_A * Vector3D<Float32>::GetRight(),
                                                         ROT_A * Vector3D<Float32>::GetUp(),
                                                         ROT_A * Vector3D<Float32>::GetForward()};
        const std::array<Vector3D<Float32>, 3> AXES_B = {ROT_B * Vector3D<Float32>::GetRight(),
                                                         ROT_B * Vector3D<Float32>::GetUp(),
                                                         ROT_B * Vector3D<Float32>::GetForward()};

        const Vector3D<Float32> DIFF = box.center - center;

        auto const TEST_AXIS = [&](const Vector3D<Float32>& axis) noexcept -> bool
        {
            if (axis.GetSqrMagnitude() <= Mathf::EPSILON)
            {
                return true;
            }
            const Float32 R_A = extents.GetX() * std::abs(Vector3D<Float32>::Dot(AXES_A[0], axis)) +
                    extents.GetY() * std::abs(Vector3D<Float32>::Dot(AXES_A[1], axis)) +
                    extents.GetZ() * std::abs(Vector3D<Float32>::Dot(AXES_A[2], axis));
            const Float32 R_B = box.extents.GetX() * std::abs(Vector3D<Float32>::Dot(AXES_B[0], axis)) +
                    box.extents.GetY() * std::abs(Vector3D<Float32>::Dot(AXES_B[1], axis)) +
                    box.extents.GetZ() * std::abs(Vector3D<Float32>::Dot(AXES_B[2], axis));
            const Float32 DIST = std::abs(Vector3D<Float32>::Dot(DIFF, axis));
            return DIST <= (R_A + R_B);
        };

        for (USize i = 0; i < 3; ++i)
        {
            if (!TEST_AXIS(AXES_A[i]) || !TEST_AXIS(AXES_B[i]))
            {
                return false;
            }
        }

        for (USize i = 0; i < 3; ++i)
        {
            for (USize j = 0; j < 3; ++j)
            {
                if (!TEST_AXIS(Vector3D<Float32>::Cross(AXES_A[i], AXES_B[j])))
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
        const Quaternion<Float32> INV_ROT = Quaternion<Float32>::Conjugate(NormalizeRotation(orientation));
        const BoundingBox         LOCAL_BOX(Vector3D<Float32>::GetZero(), extents);
        return LOCAL_BOX.Intersects(INV_ROT * (v0 - center), INV_ROT * (v1 - center), INV_ROT * (v2 - center));
    }

    bool BoundingOrientedBox::Intersects(const Vector4D<Float32>& plane) const noexcept
    {
        const Vector4D<Float32>   P          = NormalizePlane(plane);
        const Vector3D<Float32>   PLANE_NORM = Vector3D<Float32>(P.GetX(), P.GetY(), P.GetZ());
        const Quaternion<Float32> NORM_ROT   = NormalizeRotation(orientation);
        const Vector3D<Float32>   AXIS_X     = NORM_ROT * Vector3D<Float32>::GetRight();
        const Vector3D<Float32>   AXIS_Y     = NORM_ROT * Vector3D<Float32>::GetUp();
        const Vector3D<Float32>   AXIS_Z     = NORM_ROT * Vector3D<Float32>::GetForward();

        const Float32 R = extents.GetX() * std::abs(Vector3D<Float32>::Dot(PLANE_NORM, AXIS_X)) +
                extents.GetY() * std::abs(Vector3D<Float32>::Dot(PLANE_NORM, AXIS_Y)) +
                extents.GetZ() * std::abs(Vector3D<Float32>::Dot(PLANE_NORM, AXIS_Z));
        const Float32 S = PlaneSignedDistance(P, center);
        return std::abs(S) <= R;
    }

    bool BoundingOrientedBox::Intersects(const Vector3D<Float32>& origin,
                                         const Vector3D<Float32>& direction,
                                         Float32&                 distance) const noexcept
    {
        const Vector3D<Float32>   DIR          = NormalizeDirection(direction);
        const Quaternion<Float32> INV_ROT      = Quaternion<Float32>::Conjugate(NormalizeRotation(orientation));
        const Vector3D<Float32>   LOCAL_ORIGIN = INV_ROT * (origin - center);
        const Vector3D<Float32>   LOCAL_DIR    = INV_ROT * DIR;
        const BoundingBox         LOCAL_BOX(Vector3D<Float32>::GetZero(), extents);
        return LOCAL_BOX.Intersects(LOCAL_ORIGIN, LOCAL_DIR, distance);
    }

    bool BoundingOrientedBox::ContainedBy(const Vector4D<Float32>& plane0,
                                          const Vector4D<Float32>& plane1,
                                          const Vector4D<Float32>& plane2,
                                          const Vector4D<Float32>& plane3,
                                          const Vector4D<Float32>& plane4,
                                          const Vector4D<Float32>& plane5) const noexcept
    {
        const std::array<Vector4D<Float32>, 6> PLANES   = {plane0, plane1, plane2, plane3, plane4, plane5};
        const Quaternion<Float32>              NORM_ROT = NormalizeRotation(orientation);
        const Vector3D<Float32>                AXIS_X   = NORM_ROT * Vector3D<Float32>::GetRight();
        const Vector3D<Float32>                AXIS_Y   = NORM_ROT * Vector3D<Float32>::GetUp();
        const Vector3D<Float32>                AXIS_Z   = NORM_ROT * Vector3D<Float32>::GetForward();

        for (const Vector4D<Float32>& plane : PLANES)
        {
            const Vector4D<Float32> P          = NormalizePlane(plane);
            const Vector3D<Float32> PLANE_NORM = Vector3D<Float32>(P.GetX(), P.GetY(), P.GetZ());
            const Float32           R          = extents.GetX() * std::abs(Vector3D<Float32>::Dot(PLANE_NORM, AXIS_X)) +
                    extents.GetY() * std::abs(Vector3D<Float32>::Dot(PLANE_NORM, AXIS_Y)) +
                    extents.GetZ() * std::abs(Vector3D<Float32>::Dot(PLANE_NORM, AXIS_Z));
            const Float32 S = PlaneSignedDistance(P, center);
            if (S > -R)
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
