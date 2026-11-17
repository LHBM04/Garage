#ifndef GARAGE_MATH_VECTOR4D_HPP
#define GARAGE_MATH_VECTOR4D_HPP

#include <array>
#include <compare>

#include "../core/defines.hpp"
#include "../core/types.hpp"

#include "mathf.hpp"

namespace Garage
{
    template <Scalarable TScalar>
    class Vector2D;

    template <Scalarable TScalar>
    class Vector3D;

    template <Scalarable TScalar>
    class GARAGE_API Vector4D final
    {
    public:
        using ValueType = TScalar;

        Vector4D() noexcept;
        explicit Vector4D(TScalar scalar) noexcept;
        Vector4D(TScalar x, TScalar y, TScalar z, TScalar w) noexcept;

        Vector4D(const Vector4D& other) noexcept = default;
        Vector4D(Vector4D&& other) noexcept      = default;

        explicit Vector4D(const Vector2D<TScalar>& vector, TScalar z = TScalar{}, TScalar w = TScalar{}) noexcept;
        explicit Vector4D(const Vector3D<TScalar>& vector, TScalar w = TScalar{}) noexcept;

        ~Vector4D() noexcept = default;

        Vector4D& operator=(const Vector4D& other) noexcept = default;
        Vector4D& operator=(Vector4D&& other) noexcept      = default;

        [[nodiscard]] explicit operator Vector2D<TScalar>() const noexcept;
        [[nodiscard]] explicit operator Vector3D<TScalar>() const noexcept;

        [[nodiscard]] explicit operator const TScalar*() const noexcept;
        [[nodiscard]] explicit operator TScalar*() noexcept;

        [[nodiscard]] TScalar  operator[](USize index) const noexcept;
        [[nodiscard]] TScalar& operator[](USize index) noexcept;

        [[nodiscard]] Vector4D operator+() const noexcept;
        [[nodiscard]] Vector4D operator+(const Vector4D& other) const noexcept;
        Vector4D&              operator+=(const Vector4D& other) noexcept;

        [[nodiscard]] Vector4D operator-() const noexcept;
        [[nodiscard]] Vector4D operator-(const Vector4D& other) const noexcept;
        Vector4D&              operator-=(const Vector4D& other) noexcept;

        [[nodiscard]] Vector4D operator*(TScalar scalar) const noexcept;
        Vector4D&              operator*=(TScalar scalar) noexcept;

        [[nodiscard]] Vector4D operator/(TScalar scalar) const noexcept;
        Vector4D&              operator/=(TScalar scalar) noexcept;

        [[nodiscard]] bool                  operator==(const Vector4D& other) const noexcept;
        [[nodiscard]] bool                  operator!=(const Vector4D& other) const noexcept;
        [[nodiscard]] std::partial_ordering operator<=>(const Vector4D& other) const noexcept;

        [[nodiscard]] TScalar GetX() const noexcept;
        void                  SetX(TScalar xValue) noexcept;

        [[nodiscard]] TScalar GetY() const noexcept;
        void                  SetY(TScalar yValue) noexcept;

        [[nodiscard]] TScalar GetZ() const noexcept;
        void                  SetZ(TScalar zValue) noexcept;

        [[nodiscard]] TScalar GetW() const noexcept;
        void                  SetW(TScalar wValue) noexcept;

        void Set(TScalar xValue, TScalar yValue, TScalar zValue, TScalar wValue) noexcept;

        [[nodiscard]] Float32 GetMagnitude() const noexcept
            requires Floatable<TScalar>;
        [[nodiscard]] Float32 GetSqrMagnitude() const noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] Float32 GetLength() const noexcept
            requires Floatable<TScalar>;
        [[nodiscard]] Float32 GetLengthSquared() const noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] Vector4D GetNormalized() const noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] bool IsZero(Float32 epsilon = Mathf::EPSILON) const noexcept;
        [[nodiscard]] bool IsFinite() const noexcept;

        void Normalize() noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] Float32 Dot(const Vector4D& other) const noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static Vector4D GetZero() noexcept;
        [[nodiscard]] static Vector4D GetOne() noexcept;

        [[nodiscard]] static Vector4D GetPositiveInfinity() noexcept
            requires Floatable<TScalar>;
        [[nodiscard]] static Vector4D GetNegativeInfinity() noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static bool
                IsApproximately(const Vector4D& lhs, const Vector4D& rhs, Float32 epsilon = Mathf::EPSILON) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static Vector4D Max(const Vector4D& a, const Vector4D& b) noexcept
            requires Floatable<TScalar>;
        [[nodiscard]] static Vector4D Min(const Vector4D& a, const Vector4D& b) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static Vector4D Clamp(const Vector4D& value, const Vector4D& min, const Vector4D& max) noexcept
            requires Floatable<TScalar>;
        [[nodiscard]] static Vector4D ClampMagnitude(const Vector4D& vector, Float32 maxLength) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static Vector4D Scale(const Vector4D& vector, const Vector4D& scale) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static Vector4D Normalize(const Vector4D& value) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static Float32 Dot(const Vector4D& a, const Vector4D& b) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static Float32 Distance(const Vector4D& a, const Vector4D& b) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static Float32 Angle(const Vector4D& a, const Vector4D& b) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static Vector4D Project(const Vector4D& vector, const Vector4D& onNormal) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static Vector4D Reflect(const Vector4D& vector, const Vector4D& normal) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static Vector4D Lerp(const Vector4D& a, const Vector4D& b, Float32 t) noexcept
            requires Floatable<TScalar>;
        [[nodiscard]] static Vector4D LerpUnclamped(const Vector4D& a, const Vector4D& b, Float32 t) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static Vector4D Slerp(const Vector4D& a, const Vector4D& b, Float32 t) noexcept
            requires Floatable<TScalar>;
        [[nodiscard]] static Vector4D SlerpUnclamped(const Vector4D& a, const Vector4D& b, Float32 t) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static Vector4D
                MoveTowards(const Vector4D& current, const Vector4D& target, Float32 maxDistanceDelta) noexcept
            requires Floatable<TScalar>;

    private:
        std::array<TScalar, 4> elements = {};
    };

    using Vector4DInt   = Vector4D<Int32>;
    using Vector4DFloat = Vector4D<Float32>;

    extern template class Vector4D<Int32>;
    extern template class Vector4D<Float32>;

    [[nodiscard]] GARAGE_API Vector4D<Int32> operator*(Int32 scalar, const Vector4D<Int32>& vector) noexcept;
    [[nodiscard]] GARAGE_API Vector4D<Float32> operator*(Float32 scalar, const Vector4D<Float32>& vector) noexcept;
} // namespace Garage

#endif // GARAGE_MATH_VECTOR4D_HPP
