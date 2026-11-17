#include "garage/math/vector4d.hpp"

#include <algorithm>
#include <cmath>

#include "garage/core/defines.hpp"

#include "garage/math/mathf.hpp"
#include "garage/math/vector2d.hpp"
#include "garage/math/vector3d.hpp"

namespace Garage
{
    template <Scalarable TScalar>
    Vector4D<TScalar>::Vector4D() noexcept = default;

    template <Scalarable TScalar>
    Vector4D<TScalar>::Vector4D(TScalar scalar) noexcept
        : elements{scalar, scalar, scalar, scalar}
    {
    }

    template <Scalarable TScalar>
    Vector4D<TScalar>::Vector4D(TScalar x, TScalar y, TScalar z, TScalar w) noexcept
        : elements{x, y, z, w}
    {
    }

    template <Scalarable TScalar>
    Vector4D<TScalar>::Vector4D(const Vector2D<TScalar>& vector, TScalar z, TScalar w) noexcept
        : elements{vector.GetX(), vector.GetY(), z, w}
    {
    }

    template <Scalarable TScalar>
    Vector4D<TScalar>::Vector4D(const Vector3D<TScalar>& vector, TScalar w) noexcept
        : elements{vector.GetX(), vector.GetY(), vector.GetZ(), w}
    {
    }

    template <Scalarable TScalar>
    Vector4D<TScalar>::operator Vector2D<TScalar>() const noexcept
    {
        return Vector2D<TScalar>(elements[0], elements[1]);
    }

    template <Scalarable TScalar>
    Vector4D<TScalar>::operator Vector3D<TScalar>() const noexcept
    {
        return Vector3D<TScalar>(elements[0], elements[1], elements[2]);
    }

    template <Scalarable TScalar>
    Vector4D<TScalar>::operator const TScalar*() const noexcept
    {
        return elements.data();
    }

    template <Scalarable TScalar>
    Vector4D<TScalar>::operator TScalar*() noexcept
    {
        return elements.data();
    }

    template <Scalarable TScalar>
    TScalar Vector4D<TScalar>::operator[](USize index) const noexcept
    {
        ASSERT(index < 4);
        return elements[index];
    }

    template <Scalarable TScalar>
    TScalar& Vector4D<TScalar>::operator[](USize index) noexcept
    {
        ASSERT(index < 4);
        return elements[index];
    }

    template <Scalarable TScalar>
    Vector4D<TScalar> Vector4D<TScalar>::operator+() const noexcept
    {
        return *this;
    }

    template <Scalarable TScalar>
    Vector4D<TScalar> Vector4D<TScalar>::operator+(const Vector4D& other) const noexcept
    {
        return Vector4D(elements[0] + other.elements[0],
                        elements[1] + other.elements[1],
                        elements[2] + other.elements[2],
                        elements[3] + other.elements[3]);
    }

    template <Scalarable TScalar>
    Vector4D<TScalar>& Vector4D<TScalar>::operator+=(const Vector4D& other) noexcept
    {
        elements[0] += other.elements[0];
        elements[1] += other.elements[1];
        elements[2] += other.elements[2];
        elements[3] += other.elements[3];
        return *this;
    }

    template <Scalarable TScalar>
    Vector4D<TScalar> Vector4D<TScalar>::operator-() const noexcept
    {
        return Vector4D(-elements[0], -elements[1], -elements[2], -elements[3]);
    }

    template <Scalarable TScalar>
    Vector4D<TScalar> Vector4D<TScalar>::operator-(const Vector4D& other) const noexcept
    {
        return Vector4D(elements[0] - other.elements[0],
                        elements[1] - other.elements[1],
                        elements[2] - other.elements[2],
                        elements[3] - other.elements[3]);
    }

    template <Scalarable TScalar>
    Vector4D<TScalar>& Vector4D<TScalar>::operator-=(const Vector4D& other) noexcept
    {
        elements[0] -= other.elements[0];
        elements[1] -= other.elements[1];
        elements[2] -= other.elements[2];
        elements[3] -= other.elements[3];
        return *this;
    }

    template <Scalarable TScalar>
    Vector4D<TScalar> Vector4D<TScalar>::operator*(TScalar scalar) const noexcept
    {
        return Vector4D(elements[0] * scalar, elements[1] * scalar, elements[2] * scalar, elements[3] * scalar);
    }

    template <Scalarable TScalar>
    Vector4D<TScalar>& Vector4D<TScalar>::operator*=(TScalar scalar) noexcept
    {
        elements[0] *= scalar;
        elements[1] *= scalar;
        elements[2] *= scalar;
        elements[3] *= scalar;
        return *this;
    }

    template <Scalarable TScalar>
    Vector4D<TScalar> Vector4D<TScalar>::operator/(TScalar scalar) const noexcept
    {
        ASSERT(scalar != TScalar{});
        return Vector4D(elements[0] / scalar, elements[1] / scalar, elements[2] / scalar, elements[3] / scalar);
    }

    template <Scalarable TScalar>
    Vector4D<TScalar>& Vector4D<TScalar>::operator/=(TScalar scalar) noexcept
    {
        ASSERT(scalar != TScalar{});
        elements[0] /= scalar;
        elements[1] /= scalar;
        elements[2] /= scalar;
        elements[3] /= scalar;
        return *this;
    }

    template <Scalarable TScalar>
    bool Vector4D<TScalar>::operator==(const Vector4D& other) const noexcept
    {
        if constexpr (Floatable<TScalar>)
        {
            return IsApproximately(*this, other);
        }
        else
        {
            return elements[0] == other.elements[0] && elements[1] == other.elements[1] &&
                    elements[2] == other.elements[2] && elements[3] == other.elements[3];
        }
    }

    template <Scalarable TScalar>
    bool Vector4D<TScalar>::operator!=(const Vector4D& other) const noexcept
    {
        return !(*this == other);
    }

    template <Scalarable TScalar>
    std::partial_ordering Vector4D<TScalar>::operator<=>(const Vector4D& other) const noexcept
    {
        if (const auto cmp = elements[0] <=> other.elements[0]; cmp != 0)
        {
            return cmp;
        }
        if (const auto cmp = elements[1] <=> other.elements[1]; cmp != 0)
        {
            return cmp;
        }
        if (const auto cmp = elements[2] <=> other.elements[2]; cmp != 0)
        {
            return cmp;
        }
        return elements[3] <=> other.elements[3];
    }

    template <Scalarable TScalar>
    TScalar Vector4D<TScalar>::GetX() const noexcept
    {
        return elements[0];
    }

    template <Scalarable TScalar>
    void Vector4D<TScalar>::SetX(TScalar xValue) noexcept
    {
        elements[0] = xValue;
    }

    template <Scalarable TScalar>
    TScalar Vector4D<TScalar>::GetY() const noexcept
    {
        return elements[1];
    }

    template <Scalarable TScalar>
    void Vector4D<TScalar>::SetY(TScalar yValue) noexcept
    {
        elements[1] = yValue;
    }

    template <Scalarable TScalar>
    TScalar Vector4D<TScalar>::GetZ() const noexcept
    {
        return elements[2];
    }

    template <Scalarable TScalar>
    void Vector4D<TScalar>::SetZ(TScalar zValue) noexcept
    {
        elements[2] = zValue;
    }

    template <Scalarable TScalar>
    TScalar Vector4D<TScalar>::GetW() const noexcept
    {
        return elements[3];
    }

    template <Scalarable TScalar>
    void Vector4D<TScalar>::SetW(TScalar wValue) noexcept
    {
        elements[3] = wValue;
    }

    template <Scalarable TScalar>
    void Vector4D<TScalar>::Set(TScalar xValue, TScalar yValue, TScalar zValue, TScalar wValue) noexcept
    {
        elements[0] = xValue;
        elements[1] = yValue;
        elements[2] = zValue;
        elements[3] = wValue;
    }

    template <Scalarable TScalar>
    Float32 Vector4D<TScalar>::GetMagnitude() const noexcept
        requires Floatable<TScalar>
    {
        return std::sqrt(GetSqrMagnitude());
    }

    template <Scalarable TScalar>
    Float32 Vector4D<TScalar>::GetSqrMagnitude() const noexcept
        requires Floatable<TScalar>
    {
        return elements[0] * elements[0] + elements[1] * elements[1] + elements[2] * elements[2] +
                elements[3] * elements[3];
    }

    template <Scalarable TScalar>
    Float32 Vector4D<TScalar>::GetLength() const noexcept
        requires Floatable<TScalar>
    {
        return GetMagnitude();
    }

    template <Scalarable TScalar>
    Float32 Vector4D<TScalar>::GetLengthSquared() const noexcept
        requires Floatable<TScalar>
    {
        return GetSqrMagnitude();
    }

    template <Scalarable TScalar>
    Vector4D<TScalar> Vector4D<TScalar>::GetNormalized() const noexcept
        requires Floatable<TScalar>
    {
        return Normalize(*this);
    }

    template <Scalarable TScalar>
    bool Vector4D<TScalar>::IsZero(Float32 epsilon) const noexcept
    {
        if constexpr (Floatable<TScalar>)
        {
            return std::abs(elements[0]) <= epsilon && std::abs(elements[1]) <= epsilon &&
                    std::abs(elements[2]) <= epsilon && std::abs(elements[3]) <= epsilon;
        }
        else
        {
            return elements[0] == 0 && elements[1] == 0 && elements[2] == 0 && elements[3] == 0;
        }
    }

    template <Scalarable TScalar>
    bool Vector4D<TScalar>::IsFinite() const noexcept
    {
        if constexpr (Floatable<TScalar>)
        {
            return std::isfinite(elements[0]) && std::isfinite(elements[1]) && std::isfinite(elements[2]) &&
                    std::isfinite(elements[3]);
        }
        else
        {
            return true;
        }
    }

    template <Scalarable TScalar>
    void Vector4D<TScalar>::Normalize() noexcept
        requires Floatable<TScalar>
    {
        *this = Normalize(*this);
    }

    template <Scalarable TScalar>
    Float32 Vector4D<TScalar>::Dot(const Vector4D& other) const noexcept
        requires Floatable<TScalar>
    {
        return Dot(*this, other);
    }

    template <Scalarable TScalar>
    Vector4D<TScalar> Vector4D<TScalar>::GetZero() noexcept
    {
        return Vector4D(TScalar{0}, TScalar{0}, TScalar{0}, TScalar{0});
    }

    template <Scalarable TScalar>
    Vector4D<TScalar> Vector4D<TScalar>::GetOne() noexcept
    {
        return Vector4D(TScalar{1}, TScalar{1}, TScalar{1}, TScalar{1});
    }

    template <Scalarable TScalar>
    Vector4D<TScalar> Vector4D<TScalar>::GetPositiveInfinity() noexcept
        requires Floatable<TScalar>
    {
        return Vector4D(Mathf::POSITIVE_INF, Mathf::POSITIVE_INF, Mathf::POSITIVE_INF, Mathf::POSITIVE_INF);
    }

    template <Scalarable TScalar>
    Vector4D<TScalar> Vector4D<TScalar>::GetNegativeInfinity() noexcept
        requires Floatable<TScalar>
    {
        return Vector4D(Mathf::NEGATIVE_INF, Mathf::NEGATIVE_INF, Mathf::NEGATIVE_INF, Mathf::NEGATIVE_INF);
    }

    template <Scalarable TScalar>
    bool Vector4D<TScalar>::IsApproximately(const Vector4D& lhs, const Vector4D& rhs, Float32 epsilon) noexcept
        requires Floatable<TScalar>
    {
        return std::abs(lhs.elements[0] - rhs.elements[0]) <= epsilon &&
                std::abs(lhs.elements[1] - rhs.elements[1]) <= epsilon &&
                std::abs(lhs.elements[2] - rhs.elements[2]) <= epsilon &&
                std::abs(lhs.elements[3] - rhs.elements[3]) <= epsilon;
    }

    template <Scalarable TScalar>
    Vector4D<TScalar> Vector4D<TScalar>::Max(const Vector4D& a, const Vector4D& b) noexcept
        requires Floatable<TScalar>
    {
        return Vector4D(std::max(a.elements[0], b.elements[0]),
                        std::max(a.elements[1], b.elements[1]),
                        std::max(a.elements[2], b.elements[2]),
                        std::max(a.elements[3], b.elements[3]));
    }

    template <Scalarable TScalar>
    Vector4D<TScalar> Vector4D<TScalar>::Min(const Vector4D& a, const Vector4D& b) noexcept
        requires Floatable<TScalar>
    {
        return Vector4D(std::min(a.elements[0], b.elements[0]),
                        std::min(a.elements[1], b.elements[1]),
                        std::min(a.elements[2], b.elements[2]),
                        std::min(a.elements[3], b.elements[3]));
    }

    template <Scalarable TScalar>
    Vector4D<TScalar> Vector4D<TScalar>::Clamp(const Vector4D& value, const Vector4D& min, const Vector4D& max) noexcept
        requires Floatable<TScalar>
    {
        return Vector4D(std::clamp(value.elements[0], min.elements[0], max.elements[0]),
                        std::clamp(value.elements[1], min.elements[1], max.elements[1]),
                        std::clamp(value.elements[2], min.elements[2], max.elements[2]),
                        std::clamp(value.elements[3], min.elements[3], max.elements[3]));
    }

    template <Scalarable TScalar>
    Vector4D<TScalar> Vector4D<TScalar>::ClampMagnitude(const Vector4D& vector, Float32 maxLength) noexcept
        requires Floatable<TScalar>
    {
        const Float32 sqrMag = vector.GetSqrMagnitude();
        const Float32 maxSqr = maxLength * maxLength;
        if (sqrMag <= maxSqr)
        {
            return vector;
        }
        return Normalize(vector) * maxLength;
    }

    template <Scalarable TScalar>
    Vector4D<TScalar> Vector4D<TScalar>::Scale(const Vector4D& vector, const Vector4D& scale) noexcept
        requires Floatable<TScalar>
    {
        return Vector4D(vector.elements[0] * scale.elements[0],
                        vector.elements[1] * scale.elements[1],
                        vector.elements[2] * scale.elements[2],
                        vector.elements[3] * scale.elements[3]);
    }

    template <Scalarable TScalar>
    Vector4D<TScalar> Vector4D<TScalar>::Normalize(const Vector4D& value) noexcept
        requires Floatable<TScalar>
    {
        const Float32 mag = value.GetMagnitude();
        if (mag <= Mathf::EPSILON)
        {
            return GetZero();
        }
        return value / mag;
    }

    template <Scalarable TScalar>
    Float32 Vector4D<TScalar>::Dot(const Vector4D& a, const Vector4D& b) noexcept
        requires Floatable<TScalar>
    {
        return a.elements[0] * b.elements[0] + a.elements[1] * b.elements[1] + a.elements[2] * b.elements[2] +
                a.elements[3] * b.elements[3];
    }

    template <Scalarable TScalar>
    Float32 Vector4D<TScalar>::Distance(const Vector4D& a, const Vector4D& b) noexcept
        requires Floatable<TScalar>
    {
        return (a - b).GetMagnitude();
    }

    template <Scalarable TScalar>
    Float32 Vector4D<TScalar>::Angle(const Vector4D& a, const Vector4D& b) noexcept
        requires Floatable<TScalar>
    {
        const Vector4D from = Normalize(a);
        const Vector4D to   = Normalize(b);
        const Float32  dot  = std::clamp(Dot(from, to), -1.0f, 1.0f);
        return std::acos(dot);
    }

    template <Scalarable TScalar>
    Vector4D<TScalar> Vector4D<TScalar>::Project(const Vector4D& vector, const Vector4D& onNormal) noexcept
        requires Floatable<TScalar>
    {
        const Float32 denom = Dot(onNormal, onNormal);
        if (denom <= Mathf::EPSILON)
        {
            return GetZero();
        }
        return onNormal * (Dot(vector, onNormal) / denom);
    }

    template <Scalarable TScalar>
    Vector4D<TScalar> Vector4D<TScalar>::Reflect(const Vector4D& vector, const Vector4D& normal) noexcept
        requires Floatable<TScalar>
    {
        return vector - normal * (2.0f * Dot(vector, normal));
    }

    template <Scalarable TScalar>
    Vector4D<TScalar> Vector4D<TScalar>::Lerp(const Vector4D& a, const Vector4D& b, Float32 t) noexcept
        requires Floatable<TScalar>
    {
        return LerpUnclamped(a, b, std::clamp(t, 0.0f, 1.0f));
    }

    template <Scalarable TScalar>
    Vector4D<TScalar> Vector4D<TScalar>::LerpUnclamped(const Vector4D& a, const Vector4D& b, Float32 t) noexcept
        requires Floatable<TScalar>
    {
        return Vector4D(Mathf::LerpUnclamped(a.elements[0], b.elements[0], t),
                        Mathf::LerpUnclamped(a.elements[1], b.elements[1], t),
                        Mathf::LerpUnclamped(a.elements[2], b.elements[2], t),
                        Mathf::LerpUnclamped(a.elements[3], b.elements[3], t));
    }

    template <Scalarable TScalar>
    Vector4D<TScalar> Vector4D<TScalar>::Slerp(const Vector4D& a, const Vector4D& b, Float32 t) noexcept
        requires Floatable<TScalar>
    {
        return SlerpUnclamped(a, b, std::clamp(t, 0.0f, 1.0f));
    }

    template <Scalarable TScalar>
    Vector4D<TScalar> Vector4D<TScalar>::SlerpUnclamped(const Vector4D& a, const Vector4D& b, Float32 t) noexcept
        requires Floatable<TScalar>
    {
        const Float32 aMag = a.GetMagnitude();
        const Float32 bMag = b.GetMagnitude();

        if (aMag <= Mathf::EPSILON || bMag <= Mathf::EPSILON)
        {
            return LerpUnclamped(a, b, t);
        }

        const Vector4D from = a / aMag;
        const Vector4D to   = b / bMag;

        const Float32 dot   = std::clamp(Dot(from, to), -1.0f, 1.0f);
        const Float32 theta = std::acos(dot) * t;

        Vector4D      relative    = to - from * dot;
        const Float32 relativeMag = relative.GetMagnitude();
        if (relativeMag <= Mathf::EPSILON)
        {
            return LerpUnclamped(a, b, t);
        }

        relative /= relativeMag;
        const Vector4D direction = from * std::cos(theta) + relative * std::sin(theta);
        const Float32  magnitude = Mathf::LerpUnclamped(aMag, bMag, t);

        return direction * magnitude;
    }

    template <Scalarable TScalar>
    Vector4D<TScalar> Vector4D<TScalar>::MoveTowards(const Vector4D& current,
                                                     const Vector4D& target,
                                                     Float32         maxDistanceDelta) noexcept
        requires Floatable<TScalar>
    {
        const Vector4D delta    = target - current;
        const Float32  distance = delta.GetMagnitude();

        if (distance <= maxDistanceDelta || distance <= Mathf::EPSILON)
        {
            return target;
        }

        return current + (delta / distance) * maxDistanceDelta;
    }

    template class Vector4D<Int32>;
    template class Vector4D<Float32>;

    Vector4D<Int32> operator*(Int32 scalar, const Vector4D<Int32>& vector) noexcept
    {
        return vector * scalar;
    }

    Vector4D<Float32> operator*(Float32 scalar, const Vector4D<Float32>& vector) noexcept
    {
        return vector * scalar;
    }
} // namespace Garage
