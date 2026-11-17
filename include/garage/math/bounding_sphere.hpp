#ifndef GARAGE_MATH_BOUNDING_SPHERE_HPP
#define GARAGE_MATH_BOUNDING_SPHERE_HPP

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
    class BoundingOrientedBox;

    class GARAGE_API BoundingSphere final
    {
    public:
        using PointSpan = std::span<const Vector3D<Float32>>;

        BoundingSphere() noexcept;
        BoundingSphere(const Vector3D<Float32>& center, Float32 radius) noexcept;

        BoundingSphere(const BoundingSphere& other) noexcept            = default;
        BoundingSphere& operator=(const BoundingSphere& other) noexcept = default;

        BoundingSphere(BoundingSphere&& other) noexcept            = default;
        BoundingSphere& operator=(BoundingSphere&& other) noexcept = default;

        ~BoundingSphere() noexcept = default;

        [[nodiscard]] bool                  operator==(const BoundingSphere& other) const noexcept;
        [[nodiscard]] bool                  operator!=(const BoundingSphere& other) const noexcept;
        [[nodiscard]] std::partial_ordering operator<=>(const BoundingSphere& other) const noexcept;

        [[nodiscard]] Vector3D<Float32> GetCenter() const noexcept;
        void                            SetCenter(const Vector3D<Float32>& centerValue) noexcept;

        [[nodiscard]] Float32 GetRadius() const noexcept;
        void                  SetRadius(Float32 radiusValue) noexcept;

        void Set(const Vector3D<Float32>& centerValue, Float32 radiusValue) noexcept;

        [[nodiscard]] bool IsFinite() const noexcept;
        [[nodiscard]] bool IsValid() const noexcept;

        [[nodiscard]] BoundingSphere GetTransformed(const Matrix4x4<Float32>& matrix) const noexcept;
        [[nodiscard]] BoundingSphere GetTransformed(Float32                    scale,
                                                    const Quaternion<Float32>& rotation,
                                                    const Vector3D<Float32>&   translation) const noexcept;

        void Transform(const Matrix4x4<Float32>& matrix) noexcept;
        void Transform(Float32                    scale,
                       const Quaternion<Float32>& rotation,
                       const Vector3D<Float32>&   translation) noexcept;
        void Transform(BoundingSphere& out, const Matrix4x4<Float32>& matrix) const noexcept;
        void Transform(BoundingSphere&            out,
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

        [[nodiscard]] static bool IsApproximately(const BoundingSphere& lhs,
                                                  const BoundingSphere& rhs,
                                                  Float32               epsilon = Mathf::EPSILON) noexcept;

        [[nodiscard]] static BoundingSphere CreateMerged(const BoundingSphere& s1, const BoundingSphere& s2) noexcept;
        [[nodiscard]] static BoundingSphere CreateFromBoundingBox(const BoundingBox& box) noexcept;
        [[nodiscard]] static BoundingSphere CreateFromBoundingBox(const BoundingOrientedBox& box) noexcept;
        [[nodiscard]] static BoundingSphere CreateFromPoints(PointSpan points) noexcept;

    private:
        Vector3D<Float32> center = Vector3D<Float32>(0.0f, 0.0f, 0.0f);
        Float32           radius = 1.0f;
    };
} // namespace Garage

#endif // GARAGE_MATH_BOUNDING_SPHERE_HPP
