#include "garage/math/vector2d.hpp"

#include <algorithm>
#include <cmath>
#include <compare>

#include "garage/core/common.hpp"

#include "garage/math/mathf.hpp"
#include "garage/math/vector3d.hpp"
#include "garage/math/vector4d.hpp"

namespace Garage
{
    template <Scalarable TScalar>
    Vector2D<TScalar>::Vector2D() noexcept = default;

    template <Scalarable TScalar>
    Vector2D<TScalar>::Vector2D(TScalar scalar) noexcept
        : elements{scalar, scalar}
    {
    }

    template <Scalarable TScalar>
    Vector2D<TScalar>::Vector2D(TScalar x, TScalar y) noexcept
        : elements{x, y}
    {
    }

    template <Scalarable TScalar>
    Vector2D<TScalar>::Vector2D(const Vector3D<TScalar>& vector) noexcept
        : elements{vector.GetX(), vector.GetY()}
    {
    }

    template <Scalarable TScalar>
    Vector2D<TScalar>::Vector2D(const Vector4D<TScalar>& vector) noexcept
        : elements{vector.GetX(), vector.GetY()}
    {
    }

    template <Scalarable TScalar>
    Vector2D<TScalar>::operator Vector3D<TScalar>() const noexcept
    {
        return Vector3D<TScalar>(elements[0], elements[1], TScalar{});
    }

    template <Scalarable TScalar>
    Vector2D<TScalar>::operator Vector4D<TScalar>() const noexcept
    {
        return Vector4D<TScalar>(elements[0], elements[1], TScalar{}, TScalar{});
    }

    template <Scalarable TScalar>
    Vector2D<TScalar>::operator const TScalar*() const noexcept
    {
        return elements.data();
    }

    template <Scalarable TScalar>
    Vector2D<TScalar>::operator TScalar*() noexcept
    {
        return elements.data();
    }

    template <Scalarable TScalar>
    TScalar Vector2D<TScalar>::operator[](USize index) const noexcept
    {
        ASSERT(index < 2);
        return elements[index];
    }

    template <Scalarable TScalar>
    TScalar& Vector2D<TScalar>::operator[](USize index) noexcept
    {
        ASSERT(index < 2);
        return elements[index];
    }

    template <Scalarable TScalar>
    Vector2D<TScalar> Vector2D<TScalar>::operator+() const noexcept
    {
        return *this;
    }

    template <Scalarable TScalar>
    Vector2D<TScalar> Vector2D<TScalar>::operator+(const Vector2D& other) const noexcept
    {
        return Vector2D(elements[0] + other.elements[0], elements[1] + other.elements[1]);
    }

    template <Scalarable TScalar>
    Vector2D<TScalar>& Vector2D<TScalar>::operator+=(const Vector2D& other) noexcept
    {
        elements[0] += other.elements[0];
        elements[1] += other.elements[1];
        return *this;
    }

    template <Scalarable TScalar>
    Vector2D<TScalar> Vector2D<TScalar>::operator-() const noexcept
    {
        return Vector2D(-elements[0], -elements[1]);
    }

    template <Scalarable TScalar>
    Vector2D<TScalar> Vector2D<TScalar>::operator-(const Vector2D& other) const noexcept
    {
        return Vector2D(elements[0] - other.elements[0], elements[1] - other.elements[1]);
    }

    template <Scalarable TScalar>
    Vector2D<TScalar>& Vector2D<TScalar>::operator-=(const Vector2D& other) noexcept
    {
        elements[0] -= other.elements[0];
        elements[1] -= other.elements[1];
        return *this;
    }

    template <Scalarable TScalar>
    Vector2D<TScalar> Vector2D<TScalar>::operator*(TScalar scalar) const noexcept
    {
        return Vector2D(elements[0] * scalar, elements[1] * scalar);
    }

    template <Scalarable TScalar>
    Vector2D<TScalar>& Vector2D<TScalar>::operator*=(TScalar scalar) noexcept
    {
        elements[0] *= scalar;
        elements[1] *= scalar;
        return *this;
    }

    template <Scalarable TScalar>
    Vector2D<TScalar> Vector2D<TScalar>::operator/(TScalar scalar) const noexcept
    {
        ASSERT(scalar != TScalar{});
        return Vector2D(elements[0] / scalar, elements[1] / scalar);
    }

    template <Scalarable TScalar>
    Vector2D<TScalar>& Vector2D<TScalar>::operator/=(TScalar scalar) noexcept
    {
        ASSERT(scalar != TScalar{});
        elements[0] /= scalar;
        elements[1] /= scalar;
        return *this;
    }

    template <Scalarable TScalar>
    bool Vector2D<TScalar>::operator==(const Vector2D& other) const noexcept
    {
        if constexpr (Floatable<TScalar>)
        {
            return IsApproximately(*this, other);
        }
        else
        {
            return elements[0] == other.elements[0] && elements[1] == other.elements[1];
        }
    }

    template <Scalarable TScalar>
    bool Vector2D<TScalar>::operator!=(const Vector2D& other) const noexcept
    {
        return !(*this == other);
    }

    template <Scalarable TScalar>
    std::partial_ordering Vector2D<TScalar>::operator<=>(const Vector2D& other) const noexcept
    {
        if (const auto CMP = elements[0] <=> other.elements[0]; CMP != 0)
        {
            return CMP;
        }
        return elements[1] <=> other.elements[1];
    }

    template <Scalarable TScalar>
    TScalar Vector2D<TScalar>::GetX() const noexcept
    {
        return elements[0];
    }

    template <Scalarable TScalar>
    void Vector2D<TScalar>::SetX(TScalar xValue) noexcept
    {
        elements[0] = xValue;
    }

    template <Scalarable TScalar>
    TScalar Vector2D<TScalar>::GetY() const noexcept
    {
        return elements[1];
    }

    template <Scalarable TScalar>
    void Vector2D<TScalar>::SetY(TScalar yValue) noexcept
    {
        elements[1] = yValue;
    }

    template <Scalarable TScalar>
    void Vector2D<TScalar>::Set(TScalar xValue, TScalar yValue) noexcept
    {
        elements[0] = xValue;
        elements[1] = yValue;
    }

    template <Scalarable TScalar>
    Float32 Vector2D<TScalar>::GetMagnitude() const noexcept
        requires Floatable<TScalar>
    {
        return std::sqrt(GetSqrMagnitude());
    }

    template <Scalarable TScalar>
    Float32 Vector2D<TScalar>::GetSqrMagnitude() const noexcept
        requires Floatable<TScalar>
    {
        return elements[0] * elements[0] + elements[1] * elements[1];
    }

    template <Scalarable TScalar>
    Float32 Vector2D<TScalar>::GetLength() const noexcept
        requires Floatable<TScalar>
    {
        return GetMagnitude();
    }

    template <Scalarable TScalar>
    Float32 Vector2D<TScalar>::GetLengthSquared() const noexcept
        requires Floatable<TScalar>
    {
        return GetSqrMagnitude();
    }

    template <Scalarable TScalar>
    Vector2D<TScalar> Vector2D<TScalar>::GetNormalized() const noexcept
        requires Floatable<TScalar>
    {
        return Normalize(*this);
    }

    template <Scalarable TScalar>
    bool Vector2D<TScalar>::IsZero(Float32 epsilon) const noexcept
    {
        if constexpr (Floatable<TScalar>)
        {
            return std::abs(elements[0]) <= epsilon && std::abs(elements[1]) <= epsilon;
        }
        else
        {
            return elements[0] == 0 && elements[1] == 0;
        }
    }

    template <Scalarable TScalar>
    bool Vector2D<TScalar>::IsFinite() const noexcept
    {
        if constexpr (Floatable<TScalar>)
        {
            return std::isfinite(elements[0]) && std::isfinite(elements[1]);
        }
        else
        {
            return true;
        }
    }

    template <Scalarable TScalar>
    void Vector2D<TScalar>::Normalize() noexcept
        requires Floatable<TScalar>
    {
        *this = Normalize(*this);
    }

    template <Scalarable TScalar>
    Float32 Vector2D<TScalar>::Dot(const Vector2D& other) const noexcept
        requires Floatable<TScalar>
    {
        return Dot(*this, other);
    }

    template <Scalarable TScalar>
    Vector2D<TScalar> Vector2D<TScalar>::GetZero() noexcept
    {
        return Vector2D(TScalar{0}, TScalar{0});
    }

    template <Scalarable TScalar>
    Vector2D<TScalar> Vector2D<TScalar>::GetOne() noexcept
    {
        return Vector2D(TScalar{1}, TScalar{1});
    }

    template <Scalarable TScalar>
    Vector2D<TScalar> Vector2D<TScalar>::GetUp() noexcept
    {
        return Vector2D(TScalar{0}, TScalar{1});
    }

    template <Scalarable TScalar>
    Vector2D<TScalar> Vector2D<TScalar>::GetDown() noexcept
    {
        return Vector2D(TScalar{0}, TScalar{-1});
    }

    template <Scalarable TScalar>
    Vector2D<TScalar> Vector2D<TScalar>::GetLeft() noexcept
    {
        return Vector2D(TScalar{-1}, TScalar{0});
    }

    template <Scalarable TScalar>
    Vector2D<TScalar> Vector2D<TScalar>::GetRight() noexcept
    {
        return Vector2D(TScalar{1}, TScalar{0});
    }

    template <Scalarable TScalar>
    Vector2D<TScalar> Vector2D<TScalar>::GetPositiveInfinity() noexcept
        requires Floatable<TScalar>
    {
        return Vector2D(Mathf::POSITIVE_INF, Mathf::POSITIVE_INF);
    }

    template <Scalarable TScalar>
    Vector2D<TScalar> Vector2D<TScalar>::GetNegativeInfinity() noexcept
        requires Floatable<TScalar>
    {
        return Vector2D(Mathf::NEGATIVE_INF, Mathf::NEGATIVE_INF);
    }

    template <Scalarable TScalar>
    bool Vector2D<TScalar>::IsApproximately(const Vector2D& lhs, const Vector2D& rhs, Float32 epsilon) noexcept
        requires Floatable<TScalar>
    {
        return std::abs(lhs.elements[0] - rhs.elements[0]) <= epsilon &&
                std::abs(lhs.elements[1] - rhs.elements[1]) <= epsilon;
    }

    template <Scalarable TScalar>
    Vector2D<TScalar> Vector2D<TScalar>::Max(const Vector2D& a, const Vector2D& b) noexcept
        requires Floatable<TScalar>
    {
        return Vector2D(std::max(a.elements[0], b.elements[0]), std::max(a.elements[1], b.elements[1]));
    }

    template <Scalarable TScalar>
    Vector2D<TScalar> Vector2D<TScalar>::Min(const Vector2D& a, const Vector2D& b) noexcept
        requires Floatable<TScalar>
    {
        return Vector2D(std::min(a.elements[0], b.elements[0]), std::min(a.elements[1], b.elements[1]));
    }

    template <Scalarable TScalar>
    Vector2D<TScalar> Vector2D<TScalar>::Clamp(const Vector2D& value, const Vector2D& min, const Vector2D& max) noexcept
        requires Floatable<TScalar>
    {
        return Vector2D(std::clamp(value.elements[0], min.elements[0], max.elements[0]),
                        std::clamp(value.elements[1], min.elements[1], max.elements[1]));
    }

    template <Scalarable TScalar>
    Vector2D<TScalar> Vector2D<TScalar>::ClampMagnitude(const Vector2D& vector, Float32 maxLength) noexcept
        requires Floatable<TScalar>
    {
        const Float32 SQR_MAG = vector.GetSqrMagnitude();
        const Float32 MAX_SQR = maxLength * maxLength;
        if (SQR_MAG <= MAX_SQR)
        {
            return vector;
        }
        return Normalize(vector) * maxLength;
    }

    template <Scalarable TScalar>
    Vector2D<TScalar> Vector2D<TScalar>::Scale(const Vector2D& vector, const Vector2D& scale) noexcept
        requires Floatable<TScalar>
    {
        return Vector2D(vector.elements[0] * scale.elements[0], vector.elements[1] * scale.elements[1]);
    }

    template <Scalarable TScalar>
    Vector2D<TScalar> Vector2D<TScalar>::Normalize(const Vector2D& value) noexcept
        requires Floatable<TScalar>
    {
        const Float32 MAG = value.GetMagnitude();
        if (MAG <= Mathf::EPSILON)
        {
            return GetZero();
        }
        return value / MAG;
    }

    template <Scalarable TScalar>
    Float32 Vector2D<TScalar>::Dot(const Vector2D& a, const Vector2D& b) noexcept
        requires Floatable<TScalar>
    {
        return a.elements[0] * b.elements[0] + a.elements[1] * b.elements[1];
    }

    template <Scalarable TScalar>
    Vector2D<TScalar> Vector2D<TScalar>::Cross(const Vector2D& a, const Vector2D& b) noexcept
        requires Floatable<TScalar>
    {
        const Float32 Z = a.elements[0] * b.elements[1] - a.elements[1] * b.elements[0];
        return Vector2D(0.0f, Z);
    }

    template <Scalarable TScalar>
    Vector2D<TScalar> Vector2D<TScalar>::Perpendicular(const Vector2D& vector) noexcept
        requires Floatable<TScalar>
    {
        return Vector2D(-vector.elements[1], vector.elements[0]);
    }

    template <Scalarable TScalar>
    Float32 Vector2D<TScalar>::Distance(const Vector2D& a, const Vector2D& b) noexcept
        requires Floatable<TScalar>
    {
        return (a - b).GetMagnitude();
    }

    template <Scalarable TScalar>
    Float32 Vector2D<TScalar>::Angle(const Vector2D& a, const Vector2D& b) noexcept
        requires Floatable<TScalar>
    {
        const Vector2D FROM = Normalize(a);
        const Vector2D TO   = Normalize(b);
        const Float32  DOT  = std::clamp(Dot(FROM, TO), -1.0f, 1.0f);
        return std::acos(DOT);
    }

    template <Scalarable TScalar>
    Float32 Vector2D<TScalar>::SignedAngle(const Vector2D& from, const Vector2D& to) noexcept
        requires Floatable<TScalar>
    {
        const Float32 ANGLE = Angle(from, to);
        const Float32 DET   = from.elements[0] * to.elements[1] - from.elements[1] * to.elements[0];
        return (DET >= 0.0f) ? ANGLE : -ANGLE;
    }

    template <Scalarable TScalar>
    Vector2D<TScalar> Vector2D<TScalar>::Reflect(const Vector2D& vector, const Vector2D& normal) noexcept
        requires Floatable<TScalar>
    {
        return vector - normal * (2.0f * Dot(vector, normal));
    }

    template <Scalarable TScalar>
    Vector2D<TScalar> Vector2D<TScalar>::Lerp(const Vector2D& a, const Vector2D& b, Float32 t) noexcept
        requires Floatable<TScalar>
    {
        return LerpUnclamped(a, b, std::clamp(t, 0.0f, 1.0f));
    }

    template <Scalarable TScalar>
    Vector2D<TScalar> Vector2D<TScalar>::LerpUnclamped(const Vector2D& a, const Vector2D& b, Float32 t) noexcept
        requires Floatable<TScalar>
    {
        return Vector2D(Mathf::LerpUnclamped(a.elements[0], b.elements[0], t),
                        Mathf::LerpUnclamped(a.elements[1], b.elements[1], t));
    }

    template <Scalarable TScalar>
    Vector2D<TScalar> Vector2D<TScalar>::Slerp(const Vector2D& a, const Vector2D& b, Float32 t) noexcept
        requires Floatable<TScalar>
    {
        return SlerpUnclamped(a, b, std::clamp(t, 0.0f, 1.0f));
    }

    template <Scalarable TScalar>
    Vector2D<TScalar> Vector2D<TScalar>::SlerpUnclamped(const Vector2D& a, const Vector2D& b, Float32 t) noexcept
        requires Floatable<TScalar>
    {
        const Float32 A_MAG = a.GetMagnitude();
        const Float32 B_MAG = b.GetMagnitude();

        if (A_MAG <= Mathf::EPSILON || B_MAG <= Mathf::EPSILON)
        {
            return LerpUnclamped(a, b, t);
        }

        const Vector2D FROM = a / A_MAG;
        const Vector2D TO   = b / B_MAG;

        const Float32 DOT   = std::clamp(Dot(FROM, TO), -1.0f, 1.0f);
        const Float32 THETA = std::acos(DOT) * t;

        Vector2D      relative     = TO - FROM * DOT;
        const Float32 RELATIVE_MAG = relative.GetMagnitude();
        if (RELATIVE_MAG <= Mathf::EPSILON)
        {
            return LerpUnclamped(a, b, t);
        }

        relative /= RELATIVE_MAG;
        const Vector2D DIRECTION = FROM * std::cos(THETA) + relative * std::sin(THETA);
        const Float32  MAGNITUDE = Mathf::LerpUnclamped(A_MAG, B_MAG, t);

        return DIRECTION * MAGNITUDE;
    }

    template <Scalarable TScalar>
    Vector2D<TScalar> Vector2D<TScalar>::MoveTowards(const Vector2D& current,
                                                     const Vector2D& target,
                                                     Float32         maxDistanceDelta) noexcept
        requires Floatable<TScalar>
    {
        const Vector2D DELTA    = target - current;
        const Float32  DISTANCE = DELTA.GetMagnitude();

        if (DISTANCE <= maxDistanceDelta || DISTANCE <= Mathf::EPSILON)
        {
            return target;
        }

        return current + (DELTA / DISTANCE) * maxDistanceDelta;
    }

    template <Scalarable TScalar>
    Vector2D<TScalar> Vector2D<TScalar>::SmoothDamp(const Vector2D& current,
                                                    const Vector2D& target,
                                                    Vector2D&       currentVelocity,
                                                    Float32         smoothTime,
                                                    Float32         maxSpeed,
                                                    Float32         deltaTime) noexcept
        requires Floatable<TScalar>
    {
        smoothTime          = std::max(0.0001f, smoothTime);
        const Float32 OMEGA = 2.0f / smoothTime;
        const Float32 X_VAL = OMEGA * deltaTime;
        const Float32 EXP   = 1.0f / (1.0f + X_VAL + 0.48f * X_VAL * X_VAL + 0.235f * X_VAL * X_VAL * X_VAL);

        Vector2D       change          = current - target;
        const Vector2D ORIGINAL_TARGET = target;

        const Float32 MAX_CHANGE       = maxSpeed * smoothTime;
        change                         = ClampMagnitude(change, MAX_CHANGE);
        const Vector2D ADJUSTED_TARGET = current - change;

        const Vector2D TEMP = (currentVelocity + change * OMEGA) * deltaTime;
        currentVelocity     = (currentVelocity - TEMP * OMEGA) * EXP;
        Vector2D output     = ADJUSTED_TARGET + (change + TEMP) * EXP;

        if (Dot(ORIGINAL_TARGET - current, output - ORIGINAL_TARGET) > 0.0f)
        {
            output          = ORIGINAL_TARGET;
            currentVelocity = Vector2D(0.0f, 0.0f);
        }

        return output;
    }

    template class GARAGE_API Vector2D<Int32>;
    template class GARAGE_API Vector2D<Float32>;

    Vector2D<Int32> operator*(Int32 scalar, const Vector2D<Int32>& vector) noexcept
    {
        return vector * scalar;
    }

    Vector2D<Float32> operator*(Float32 scalar, const Vector2D<Float32>& vector) noexcept
    {
        return vector * scalar;
    }
} // namespace Garage
