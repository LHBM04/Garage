#ifndef GARAGE_MATH_VECTOR2D_HPP
#define GARAGE_MATH_VECTOR2D_HPP

#include <array>
#include <compare>

#include "../core/defines.hpp"
#include "../core/types.hpp"

#include "mathf.hpp"

namespace Garage
{
    template <Scalarable TScalar>
    class Vector3D;

    template <Scalarable TScalar>
    class Vector4D;

    template <Scalarable TScalar>
    class GARAGE_API Vector2D final
    {
    public:
        using ValueType = TScalar;

        Vector2D() noexcept;
        explicit Vector2D(TScalar scalar) noexcept;
        Vector2D(TScalar x, TScalar y) noexcept;

        Vector2D(const Vector2D& other) noexcept = default;
        Vector2D(Vector2D&& other) noexcept      = default;

        explicit Vector2D(const Vector3D<TScalar>& vector) noexcept;
        explicit Vector2D(const Vector4D<TScalar>& vector) noexcept;

        ~Vector2D() noexcept = default;

        Vector2D& operator=(const Vector2D& other) noexcept = default;
        Vector2D& operator=(Vector2D&& other) noexcept      = default;

        [[nodiscard]] explicit operator Vector3D<TScalar>() const noexcept;
        [[nodiscard]] explicit operator Vector4D<TScalar>() const noexcept;

        [[nodiscard]] explicit operator const TScalar*() const noexcept;
        [[nodiscard]] explicit operator TScalar*() noexcept;

        [[nodiscard]] TScalar  operator[](USize index) const noexcept;
        [[nodiscard]] TScalar& operator[](USize index) noexcept;

        [[nodiscard]] Vector2D operator+() const noexcept;
        [[nodiscard]] Vector2D operator+(const Vector2D& other) const noexcept;
        Vector2D&              operator+=(const Vector2D& other) noexcept;

        [[nodiscard]] Vector2D operator-() const noexcept;
        [[nodiscard]] Vector2D operator-(const Vector2D& other) const noexcept;
        Vector2D&              operator-=(const Vector2D& other) noexcept;

        [[nodiscard]] Vector2D operator*(TScalar scalar) const noexcept;
        Vector2D&              operator*=(TScalar scalar) noexcept;

        [[nodiscard]] Vector2D operator/(TScalar scalar) const noexcept;
        Vector2D&              operator/=(TScalar scalar) noexcept;

        [[nodiscard]] bool                  operator==(const Vector2D& other) const noexcept;
        [[nodiscard]] bool                  operator!=(const Vector2D& other) const noexcept;
        [[nodiscard]] std::partial_ordering operator<=>(const Vector2D& other) const noexcept;

        [[nodiscard]] TScalar GetX() const noexcept;
        void                  SetX(TScalar xValue) noexcept;

        [[nodiscard]] TScalar GetY() const noexcept;
        void                  SetY(TScalar yValue) noexcept;

        void Set(TScalar xValue, TScalar yValue) noexcept;

        [[nodiscard]] Float32 GetMagnitude() const noexcept
            requires Floatable<TScalar>;
        [[nodiscard]] Float32 GetSqrMagnitude() const noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] Float32 GetLength() const noexcept
            requires Floatable<TScalar>;
        [[nodiscard]] Float32 GetLengthSquared() const noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] Vector2D GetNormalized() const noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] bool IsZero(Float32 epsilon = Mathf::EPSILON) const noexcept;
        [[nodiscard]] bool IsFinite() const noexcept;

        void Normalize() noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] Float32 Dot(const Vector2D& other) const noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static Vector2D GetZero() noexcept;
        [[nodiscard]] static Vector2D GetOne() noexcept;

        [[nodiscard]] static Vector2D GetUp() noexcept;
        [[nodiscard]] static Vector2D GetDown() noexcept;

        [[nodiscard]] static Vector2D GetLeft() noexcept;
        [[nodiscard]] static Vector2D GetRight() noexcept;

        [[nodiscard]] static Vector2D GetPositiveInfinity() noexcept
            requires Floatable<TScalar>;
        [[nodiscard]] static Vector2D GetNegativeInfinity() noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static bool
                IsApproximately(const Vector2D& lhs, const Vector2D& rhs, Float32 epsilon = Mathf::EPSILON) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static Vector2D Max(const Vector2D& a, const Vector2D& b) noexcept
            requires Floatable<TScalar>;
        [[nodiscard]] static Vector2D Min(const Vector2D& a, const Vector2D& b) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static Vector2D Clamp(const Vector2D& value, const Vector2D& min, const Vector2D& max) noexcept
            requires Floatable<TScalar>;
        [[nodiscard]] static Vector2D ClampMagnitude(const Vector2D& vector, Float32 maxLength) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static Vector2D Scale(const Vector2D& vector, const Vector2D& scale) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static Vector2D Normalize(const Vector2D& value) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static Float32 Dot(const Vector2D& a, const Vector2D& b) noexcept
            requires Floatable<TScalar>;
        [[nodiscard]] static Vector2D Cross(const Vector2D& a, const Vector2D& b) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static Vector2D Perpendicular(const Vector2D& vector) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static Float32 Distance(const Vector2D& a, const Vector2D& b) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static Float32 Angle(const Vector2D& a, const Vector2D& b) noexcept
            requires Floatable<TScalar>;
        [[nodiscard]] static Float32 SignedAngle(const Vector2D& from, const Vector2D& to) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static Vector2D Reflect(const Vector2D& vector, const Vector2D& normal) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static Vector2D Lerp(const Vector2D& a, const Vector2D& b, Float32 t) noexcept
            requires Floatable<TScalar>;
        [[nodiscard]] static Vector2D LerpUnclamped(const Vector2D& a, const Vector2D& b, Float32 t) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static Vector2D Slerp(const Vector2D& a, const Vector2D& b, Float32 t) noexcept
            requires Floatable<TScalar>;
        [[nodiscard]] static Vector2D SlerpUnclamped(const Vector2D& a, const Vector2D& b, Float32 t) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static Vector2D
                MoveTowards(const Vector2D& current, const Vector2D& target, Float32 maxDistanceDelta) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static Vector2D SmoothDamp(const Vector2D& current,
                                                 const Vector2D& target,
                                                 Vector2D&       currentVelocity,
                                                 Float32         smoothTime,
                                                 Float32         maxSpeed,
                                                 Float32         deltaTime) noexcept
            requires Floatable<TScalar>;

    private:
        std::array<TScalar, 2> elements = {};
    };

    using Vector2DInt   = Vector2D<Int32>;
    using Vector2DFloat = Vector2D<Float32>;

    extern template class Vector2D<Int32>;
    extern template class Vector2D<Float32>;

    [[nodiscard]] GARAGE_API Vector2D<Int32> operator*(Int32 scalar, const Vector2D<Int32>& vector) noexcept;
    [[nodiscard]] GARAGE_API Vector2D<Float32> operator*(Float32 scalar, const Vector2D<Float32>& vector) noexcept;
} // namespace Garage

#endif // GARAGE_MATH_VECTOR2D_HPP
