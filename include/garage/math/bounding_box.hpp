#ifndef GARAGE_MATH_BOUNDING_BOX_HPP
#define GARAGE_MATH_BOUNDING_BOX_HPP

#include <array>
#include <compare>
#include <span>

#include "garage/core/defines.hpp"
#include "garage/core/types.hpp"
#include "garage/math/mathf.hpp"
#include "garage/math/matrix4x4.hpp"
#include "garage/math/quaternion.hpp"
#include "garage/math/vector3d.hpp"
#include "garage/math/vector4d.hpp"

namespace Garage
{
    class BoundingOrientedBox;
    class BoundingSphere;

    class GARAGE_API BoundingBox final
    {
    public:
        static constexpr USize CORNER_COUNT = 8;

        using CornerArray = std::array<Vector3D<Float32>, CORNER_COUNT>;
        using CornerSpan  = std::span<Vector3D<Float32>, CORNER_COUNT>;
        using PointSpan   = std::span<const Vector3D<Float32>>;

        BoundingBox() noexcept;
        BoundingBox(const Vector3D<Float32>& center, const Vector3D<Float32>& extents) noexcept;

        BoundingBox(const BoundingBox& other) noexcept = default;
        BoundingBox(BoundingBox&& other) noexcept      = default;

        ~BoundingBox() noexcept = default;

        BoundingBox& operator=(const BoundingBox& other) noexcept = default;
        BoundingBox& operator=(BoundingBox&& other) noexcept      = default;

        [[nodiscard]] bool                  operator==(const BoundingBox& other) const noexcept;
        [[nodiscard]] bool                  operator!=(const BoundingBox& other) const noexcept;
        [[nodiscard]] std::partial_ordering operator<=>(const BoundingBox& other) const noexcept;

        [[nodiscard]] Vector3D<Float32> GetCenter() const noexcept;
        void                            SetCenter(const Vector3D<Float32>& centerValue) noexcept;

        [[nodiscard]] Vector3D<Float32> GetExtents() const noexcept;
        void                            SetExtents(const Vector3D<Float32>& extentsValue) noexcept;

        void Set(const Vector3D<Float32>& centerValue, const Vector3D<Float32>& extentsValue) noexcept;

        [[nodiscard]] Vector3D<Float32> GetMin() const noexcept;
        [[nodiscard]] Vector3D<Float32> GetMax() const noexcept;
        [[nodiscard]] Vector3D<Float32> GetSize() const noexcept;
        void                            SetMinMax(const Vector3D<Float32>& min, const Vector3D<Float32>& max) noexcept;

        [[nodiscard]] bool IsFinite() const noexcept;
        [[nodiscard]] bool IsValid() const noexcept;

        [[nodiscard]] CornerArray GetCorners() const noexcept;
        void                      GetCorners(CornerSpan corners) const noexcept;

        [[nodiscard]] BoundingBox GetTransformed(const Matrix4x4<Float32>& matrix) const noexcept;
        [[nodiscard]] BoundingBox GetTransformed(Float32                    scale,
                                                 const Quaternion<Float32>& rotation,
                                                 const Vector3D<Float32>&   translation) const noexcept;

        void Transform(const Matrix4x4<Float32>& matrix) noexcept;
        void Transform(Float32                    scale,
                       const Quaternion<Float32>& rotation,
                       const Vector3D<Float32>&   translation) noexcept;
        void Transform(BoundingBox& out, const Matrix4x4<Float32>& matrix) const noexcept;
        void Transform(BoundingBox&               out,
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

        [[nodiscard]] static bool IsApproximately(const BoundingBox& lhs,
                                                  const BoundingBox& rhs,
                                                  Float32            epsilon = Mathf::EPSILON) noexcept;

        [[nodiscard]] static BoundingBox CreateMerged(const BoundingBox& b1, const BoundingBox& b2) noexcept;
        [[nodiscard]] static BoundingBox CreateFromSphere(const BoundingSphere& sphere) noexcept;
        [[nodiscard]] static BoundingBox CreateFromMinMax(const Vector3D<Float32>& min,
                                                          const Vector3D<Float32>& max) noexcept;
        [[nodiscard]] static BoundingBox CreateFromPoints(const Vector3D<Float32>& point1,
                                                          const Vector3D<Float32>& point2) noexcept;
        [[nodiscard]] static BoundingBox CreateFromPoints(PointSpan points) noexcept;

    private:
        Vector3D<Float32> center  = Vector3D<Float32>(0.0f, 0.0f, 0.0f);
        Vector3D<Float32> extents = Vector3D<Float32>(1.0f, 1.0f, 1.0f);
    };
} // namespace Garage

#endif // GARAGE_MATH_BOUNDING_BOX_HPP
