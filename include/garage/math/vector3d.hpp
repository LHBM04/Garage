#ifndef GARAGE_MATH_VECTOR3D_HPP
#define GARAGE_MATH_VECTOR3D_HPP

#include <array>
#include <compare>

#include "../core/common.hpp"

#include "mathf.hpp"

namespace Garage
{
    template <Scalarable TScalar>
    class Vector2D;

    template <Scalarable TScalar>
    class Vector4D;

    template <Scalarable TScalar>
    class Vector3D final
    {
    public:
        using ValueType = TScalar;

        Vector3D() noexcept;
        explicit Vector3D(TScalar scalar) noexcept;
        Vector3D(TScalar x, TScalar y, TScalar z) noexcept;

        Vector3D(const Vector3D& other) noexcept = default;
        Vector3D(Vector3D&& other) noexcept      = default;

        explicit Vector3D(const Vector2D<TScalar>& vector, TScalar z = TScalar{}) noexcept;
        explicit Vector3D(const Vector4D<TScalar>& vector) noexcept;

        ~Vector3D() noexcept = default;

        Vector3D& operator=(const Vector3D& other) noexcept = default;
        Vector3D& operator=(Vector3D&& other) noexcept      = default;

        [[nodiscard]] explicit operator Vector2D<TScalar>() const noexcept;
        [[nodiscard]] explicit operator Vector4D<TScalar>() const noexcept;

        [[nodiscard]] explicit operator const TScalar*() const noexcept;
        [[nodiscard]] explicit operator TScalar*() noexcept;

        [[nodiscard]] TScalar  operator[](USize index) const noexcept;
        [[nodiscard]] TScalar& operator[](USize index) noexcept;

        [[nodiscard]] Vector3D operator+() const noexcept;
        [[nodiscard]] Vector3D operator+(const Vector3D& other) const noexcept;
        Vector3D&              operator+=(const Vector3D& other) noexcept;

        [[nodiscard]] Vector3D operator-() const noexcept;
        [[nodiscard]] Vector3D operator-(const Vector3D& other) const noexcept;
        Vector3D&              operator-=(const Vector3D& other) noexcept;

        [[nodiscard]] Vector3D operator*(TScalar scalar) const noexcept;
        Vector3D&              operator*=(TScalar scalar) noexcept;

        [[nodiscard]] Vector3D operator/(TScalar scalar) const noexcept;
        Vector3D&              operator/=(TScalar scalar) noexcept;

        [[nodiscard]] bool                  operator==(const Vector3D& other) const noexcept;
        [[nodiscard]] bool                  operator!=(const Vector3D& other) const noexcept;
        [[nodiscard]] std::partial_ordering operator<=>(const Vector3D& other) const noexcept;

        [[nodiscard]] TScalar GetX() const noexcept;
        void                  SetX(TScalar xValue) noexcept;

        [[nodiscard]] TScalar GetY() const noexcept;
        void                  SetY(TScalar yValue) noexcept;

        [[nodiscard]] TScalar GetZ() const noexcept;
        void                  SetZ(TScalar zValue) noexcept;

        void Set(TScalar xValue, TScalar yValue, TScalar zValue) noexcept;

        [[nodiscard]] Float32 GetMagnitude() const noexcept
            requires Floatable<TScalar>;
        [[nodiscard]] Float32 GetSqrMagnitude() const noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] Float32 GetLength() const noexcept
            requires Floatable<TScalar>;
        [[nodiscard]] Float32 GetLengthSquared() const noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] Vector3D GetNormalized() const noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] bool IsZero(Float32 epsilon = Mathf::EPSILON) const noexcept;
        [[nodiscard]] bool IsFinite() const noexcept;

        void Normalize() noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] Float32 Dot(const Vector3D& other) const noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static Vector3D GetZero() noexcept;
        [[nodiscard]] static Vector3D GetOne() noexcept;

        [[nodiscard]] static Vector3D GetUp() noexcept;
        [[nodiscard]] static Vector3D GetDown() noexcept;

        [[nodiscard]] static Vector3D GetLeft() noexcept;
        [[nodiscard]] static Vector3D GetRight() noexcept;

        [[nodiscard]] static Vector3D GetForward() noexcept;
        [[nodiscard]] static Vector3D GetBack() noexcept;

        [[nodiscard]] static Vector3D GetPositiveInfinity() noexcept
            requires Floatable<TScalar>;
        [[nodiscard]] static Vector3D GetNegativeInfinity() noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static bool
                IsApproximately(const Vector3D& lhs, const Vector3D& rhs, Float32 epsilon = Mathf::EPSILON) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static Vector3D Max(const Vector3D& a, const Vector3D& b) noexcept
            requires Floatable<TScalar>;
        [[nodiscard]] static Vector3D Min(const Vector3D& a, const Vector3D& b) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static Vector3D Clamp(const Vector3D& value, const Vector3D& min, const Vector3D& max) noexcept
            requires Floatable<TScalar>;
        [[nodiscard]] static Vector3D ClampMagnitude(const Vector3D& vector, Float32 maxLength) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static Vector3D Scale(const Vector3D& vector, const Vector3D& scale) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static Vector3D Normalize(const Vector3D& value) noexcept
            requires Floatable<TScalar>;
        static void OrthoNormalize(Vector3D& normal, Vector3D& tangent) noexcept
            requires Floatable<TScalar>;
        static void OrthoNormalize(Vector3D& normal, Vector3D& tangent, Vector3D& binormal) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static Float32 Dot(const Vector3D& a, const Vector3D& b) noexcept
            requires Floatable<TScalar>;
        [[nodiscard]] static Vector3D Cross(const Vector3D& a, const Vector3D& b) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static Float32 Distance(const Vector3D& a, const Vector3D& b) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static Float32 Angle(const Vector3D& a, const Vector3D& b) noexcept
            requires Floatable<TScalar>;
        [[nodiscard]] static Float32
                SignedAngle(const Vector3D& from, const Vector3D& to, const Vector3D& axis) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static Vector3D Project(const Vector3D& vector, const Vector3D& onNormal) noexcept
            requires Floatable<TScalar>;
        [[nodiscard]] static Vector3D ProjectOnPlane(const Vector3D& vector, const Vector3D& planeNormal) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static Vector3D Reflect(const Vector3D& vector, const Vector3D& normal) noexcept
            requires Floatable<TScalar>;
        [[nodiscard]] static Vector3D Refract(const Vector3D& vector, const Vector3D& normal, Float32 eta) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static Vector3D Lerp(const Vector3D& a, const Vector3D& b, Float32 t) noexcept
            requires Floatable<TScalar>;
        [[nodiscard]] static Vector3D LerpUnclamped(const Vector3D& a, const Vector3D& b, Float32 t) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static Vector3D Slerp(const Vector3D& a, const Vector3D& b, Float32 t) noexcept
            requires Floatable<TScalar>;
        [[nodiscard]] static Vector3D SlerpUnclamped(const Vector3D& a, const Vector3D& b, Float32 t) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static Vector3D
                MoveTowards(const Vector3D& current, const Vector3D& target, Float32 maxDistanceDelta) noexcept
            requires Floatable<TScalar>;
        [[nodiscard]] static Vector3D RotateTowards(const Vector3D& current,
                                                    const Vector3D& target,
                                                    Float32         maxRadiansDelta,
                                                    Float32         maxMagnitudeDelta) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static Vector3D SmoothDamp(const Vector3D& current,
                                                 const Vector3D& target,
                                                 Vector3D&       currentVelocity,
                                                 Float32         smoothTime,
                                                 Float32         maxSpeed,
                                                 Float32         deltaTime) noexcept
            requires Floatable<TScalar>;

    private:
        std::array<TScalar, 3> elements = {};
    };

    using Vector3DInt   = Vector3D<Int32>;
    using Vector3DFloat = Vector3D<Float32>;

#if !defined(Garage_EXPORTS)
    extern template class GARAGE_API Vector3D<Int32>;
    extern template class GARAGE_API Vector3D<Float32>;
#endif

    [[nodiscard]] GARAGE_API Vector3D<Int32> operator*(Int32 scalar, const Vector3D<Int32>& vector) noexcept;
    [[nodiscard]] GARAGE_API Vector3D<Float32> operator*(Float32 scalar, const Vector3D<Float32>& vector) noexcept;
} // namespace Garage

#endif // GARAGE_MATH_VECTOR3D_HPP
