#ifndef GARAGE_MATH_BOUNDING_ORIENTED_BOX_HPP
#define GARAGE_MATH_BOUNDING_ORIENTED_BOX_HPP

#include <array>
#include <compare>
#include <span>

#include "../core/defines.hpp"
#include "../core/types.hpp"

#include "mathf.hpp"
#include "matrix4x4.hpp"
#include "quaternion.hpp"
#include "vector3d.hpp"
#include "vector4d.hpp"

namespace Garage
{
    class BoundingBox;
    class BoundingSphere;

    class GARAGE_API BoundingOrientedBox final
    {
    public:
        static constexpr USize CORNER_COUNT = 8;

        using CornerArray = std::array<Vector3D<Float32>, CORNER_COUNT>;
        using CornerSpan  = std::span<Vector3D<Float32>, CORNER_COUNT>;
        using PointSpan   = std::span<const Vector3D<Float32>>;

        BoundingOrientedBox() noexcept;
        BoundingOrientedBox(const Vector3D<Float32>&   center,
                            const Vector3D<Float32>&   extents,
                            const Quaternion<Float32>& orientation) noexcept;

        BoundingOrientedBox(const BoundingOrientedBox& other) noexcept = default;
        BoundingOrientedBox(BoundingOrientedBox&& other) noexcept      = default;

        ~BoundingOrientedBox() noexcept = default;

        BoundingOrientedBox& operator=(const BoundingOrientedBox& other) noexcept = default;
        BoundingOrientedBox& operator=(BoundingOrientedBox&& other) noexcept      = default;

        [[nodiscard]] bool                  operator==(const BoundingOrientedBox& other) const noexcept;
        [[nodiscard]] bool                  operator!=(const BoundingOrientedBox& other) const noexcept;
        [[nodiscard]] std::partial_ordering operator<=>(const BoundingOrientedBox& other) const noexcept;

        [[nodiscard]] Vector3D<Float32> GetCenter() const noexcept;
        void                            SetCenter(const Vector3D<Float32>& centerValue) noexcept;

        [[nodiscard]] Vector3D<Float32> GetExtents() const noexcept;
        void                            SetExtents(const Vector3D<Float32>& extentsValue) noexcept;

        [[nodiscard]] Quaternion<Float32> GetOrientation() const noexcept;
        void                              SetOrientation(const Quaternion<Float32>& orientationValue) noexcept;

        void Set(const Vector3D<Float32>&   centerValue,
                 const Vector3D<Float32>&   extentsValue,
                 const Quaternion<Float32>& orientationValue) noexcept;

        [[nodiscard]] Vector3D<Float32> GetSize() const noexcept;

        [[nodiscard]] bool IsFinite() const noexcept;
        [[nodiscard]] bool IsValid() const noexcept;

        [[nodiscard]] CornerArray GetCorners() const noexcept;
        void                      GetCorners(CornerSpan corners) const noexcept;

        [[nodiscard]] BoundingOrientedBox GetTransformed(const Matrix4x4<Float32>& matrix) const noexcept;
        [[nodiscard]] BoundingOrientedBox GetTransformed(Float32                    scale,
                                                         const Quaternion<Float32>& rotation,
                                                         const Vector3D<Float32>&   translation) const noexcept;

        void Transform(const Matrix4x4<Float32>& matrix) noexcept;
        void Transform(Float32                    scale,
                       const Quaternion<Float32>& rotation,
                       const Vector3D<Float32>&   translation) noexcept;
        void Transform(BoundingOrientedBox& out, const Matrix4x4<Float32>& matrix) const noexcept;
        void Transform(BoundingOrientedBox&       out,
                       Float32                    scale,
                       const Quaternion<Float32>& rotation,
                       const Vector3D<Float32>&   translation) const noexcept;

        [[nodiscard]] bool Contains(const Vector3D<Float32>& point) const noexcept;
        [[nodiscard]] bool Contains(const Vector3D<Float32>& v0,
                                    const Vector3D<Float32>& v1,
                                    const Vector3D<Float32>& v2) const noexcept;
        [[nodiscard]] bool Contains(const BoundingSphere& sphere) const noexcept;
        [[nodiscard]] bool Contains(const BoundingBox& box) const noexcept;
        [[nodiscard]] bool Contains(const BoundingOrientedBox& box) const noexcept;

        [[nodiscard]] bool Intersects(const BoundingSphere& sphere) const noexcept;
        [[nodiscard]] bool Intersects(const BoundingBox& box) const noexcept;
        [[nodiscard]] bool Intersects(const BoundingOrientedBox& box) const noexcept;
        [[nodiscard]] bool Intersects(const Vector3D<Float32>& v0,
                                      const Vector3D<Float32>& v1,
                                      const Vector3D<Float32>& v2) const noexcept;
        [[nodiscard]] bool Intersects(const Vector4D<Float32>& plane) const noexcept;
        [[nodiscard]] bool Intersects(const Vector3D<Float32>& origin,
                                      const Vector3D<Float32>& direction,
                                      Float32&                 distance) const noexcept;

        [[nodiscard]] bool ContainedBy(const Vector4D<Float32>& plane0,
                                       const Vector4D<Float32>& plane1,
                                       const Vector4D<Float32>& plane2,
                                       const Vector4D<Float32>& plane3,
                                       const Vector4D<Float32>& plane4,
                                       const Vector4D<Float32>& plane5) const noexcept;

        [[nodiscard]] static bool IsApproximately(const BoundingOrientedBox& lhs,
                                                  const BoundingOrientedBox& rhs,
                                                  Float32                    epsilon = Mathf::EPSILON) noexcept;

        [[nodiscard]] static BoundingOrientedBox CreateFromBoundingBox(const BoundingBox& box) noexcept;
        [[nodiscard]] static BoundingOrientedBox CreateFromPoints(PointSpan points) noexcept;

    private:
        Vector3D<Float32>   center      = Vector3D<Float32>(0.0f, 0.0f, 0.0f);
        Vector3D<Float32>   extents     = Vector3D<Float32>(1.0f, 1.0f, 1.0f);
        Quaternion<Float32> orientation = Quaternion<Float32>::GetIdentity();
    };
} // namespace Garage

#endif // GARAGE_MATH_BOUNDING_ORIENTED_BOX_HPP
