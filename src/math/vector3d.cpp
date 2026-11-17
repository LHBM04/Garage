#include "garage/math/vector3d.hpp"

#include <algorithm>
#include <cmath>

#include "garage/core/defines.hpp"

#include "garage/math/mathf.hpp"
#include "garage/math/vector2d.hpp"
#include "garage/math/vector4d.hpp"

namespace Garage
{
    template <Scalarable TScalar>
    Vector3D<TScalar>::Vector3D() noexcept = default;

    template <Scalarable TScalar>
    Vector3D<TScalar>::Vector3D(TScalar scalar) noexcept
        : elements{scalar, scalar, scalar}
    {
    }

    template <Scalarable TScalar>
    Vector3D<TScalar>::Vector3D(TScalar x, TScalar y, TScalar z) noexcept
        : elements{x, y, z}
    {
    }

    template <Scalarable TScalar>
    Vector3D<TScalar>::Vector3D(const Vector2D<TScalar>& vector, TScalar z) noexcept
        : elements{vector.GetX(), vector.GetY(), z}
    {
    }

    template <Scalarable TScalar>
    Vector3D<TScalar>::Vector3D(const Vector4D<TScalar>& vector) noexcept
        : elements{vector.GetX(), vector.GetY(), vector.GetZ()}
    {
    }

    template <Scalarable TScalar>
    Vector3D<TScalar>::operator Vector2D<TScalar>() const noexcept
    {
        return Vector2D<TScalar>(elements[0], elements[1]);
    }

    template <Scalarable TScalar>
    Vector3D<TScalar>::operator Vector4D<TScalar>() const noexcept
    {
        return Vector4D<TScalar>(elements[0], elements[1], elements[2], TScalar{});
    }

    template <Scalarable TScalar>
    Vector3D<TScalar>::operator const TScalar*() const noexcept
    {
        return elements.data();
    }

    template <Scalarable TScalar>
    Vector3D<TScalar>::operator TScalar*() noexcept
    {
        return elements.data();
    }

    template <Scalarable TScalar>
    TScalar Vector3D<TScalar>::operator[](USize index) const noexcept
    {
        ASSERT(index < 3);
        return elements[index];
    }

    template <Scalarable TScalar>
    TScalar& Vector3D<TScalar>::operator[](USize index) noexcept
    {
        ASSERT(index < 3);
        return elements[index];
    }

    template <Scalarable TScalar>
    Vector3D<TScalar> Vector3D<TScalar>::operator+() const noexcept
    {
        return *this;
    }

    template <Scalarable TScalar>
    Vector3D<TScalar> Vector3D<TScalar>::operator+(const Vector3D& other) const noexcept
    {
        return Vector3D(
                elements[0] + other.elements[0], elements[1] + other.elements[1], elements[2] + other.elements[2]);
    }

    template <Scalarable TScalar>
    Vector3D<TScalar>& Vector3D<TScalar>::operator+=(const Vector3D& other) noexcept
    {
        elements[0] += other.elements[0];
        elements[1] += other.elements[1];
        elements[2] += other.elements[2];
        return *this;
    }

    template <Scalarable TScalar>
    Vector3D<TScalar> Vector3D<TScalar>::operator-() const noexcept
    {
        return Vector3D(-elements[0], -elements[1], -elements[2]);
    }

    template <Scalarable TScalar>
    Vector3D<TScalar> Vector3D<TScalar>::operator-(const Vector3D& other) const noexcept
    {
        return Vector3D(
                elements[0] - other.elements[0], elements[1] - other.elements[1], elements[2] - other.elements[2]);
    }

    template <Scalarable TScalar>
    Vector3D<TScalar>& Vector3D<TScalar>::operator-=(const Vector3D& other) noexcept
    {
        elements[0] -= other.elements[0];
        elements[1] -= other.elements[1];
        elements[2] -= other.elements[2];
        return *this;
    }

    template <Scalarable TScalar>
    Vector3D<TScalar> Vector3D<TScalar>::operator*(TScalar scalar) const noexcept
    {
        return Vector3D(elements[0] * scalar, elements[1] * scalar, elements[2] * scalar);
    }

    template <Scalarable TScalar>
    Vector3D<TScalar>& Vector3D<TScalar>::operator*=(TScalar scalar) noexcept
    {
        elements[0] *= scalar;
        elements[1] *= scalar;
        elements[2] *= scalar;
        return *this;
    }

    template <Scalarable TScalar>
    Vector3D<TScalar> Vector3D<TScalar>::operator/(TScalar scalar) const noexcept
    {
        ASSERT(scalar != TScalar{});
        return Vector3D(elements[0] / scalar, elements[1] / scalar, elements[2] / scalar);
    }

    template <Scalarable TScalar>
    Vector3D<TScalar>& Vector3D<TScalar>::operator/=(TScalar scalar) noexcept
    {
        ASSERT(scalar != TScalar{});
        elements[0] /= scalar;
        elements[1] /= scalar;
        elements[2] /= scalar;
        return *this;
    }

    template <Scalarable TScalar>
    bool Vector3D<TScalar>::operator==(const Vector3D& other) const noexcept
    {
        if constexpr (Floatable<TScalar>)
        {
            return IsApproximately(*this, other);
        }
        else
        {
            return elements[0] == other.elements[0] && elements[1] == other.elements[1] &&
                    elements[2] == other.elements[2];
        }
    }

    template <Scalarable TScalar>
    bool Vector3D<TScalar>::operator!=(const Vector3D& other) const noexcept
    {
        return !(*this == other);
    }

    template <Scalarable TScalar>
    std::partial_ordering Vector3D<TScalar>::operator<=>(const Vector3D& other) const noexcept
    {
        if (const auto cmp = elements[0] <=> other.elements[0]; cmp != 0)
        {
            return cmp;
        }
        if (const auto cmp = elements[1] <=> other.elements[1]; cmp != 0)
        {
            return cmp;
        }
        return elements[2] <=> other.elements[2];
    }

    template <Scalarable TScalar>
    TScalar Vector3D<TScalar>::GetX() const noexcept
    {
        return elements[0];
    }

    template <Scalarable TScalar>
    void Vector3D<TScalar>::SetX(TScalar xValue) noexcept
    {
        elements[0] = xValue;
    }

    template <Scalarable TScalar>
    TScalar Vector3D<TScalar>::GetY() const noexcept
    {
        return elements[1];
    }

    template <Scalarable TScalar>
    void Vector3D<TScalar>::SetY(TScalar yValue) noexcept
    {
        elements[1] = yValue;
    }

    template <Scalarable TScalar>
    TScalar Vector3D<TScalar>::GetZ() const noexcept
    {
        return elements[2];
    }

    template <Scalarable TScalar>
    void Vector3D<TScalar>::SetZ(TScalar zValue) noexcept
    {
        elements[2] = zValue;
    }

    template <Scalarable TScalar>
    void Vector3D<TScalar>::Set(TScalar xValue, TScalar yValue, TScalar zValue) noexcept
    {
        elements[0] = xValue;
        elements[1] = yValue;
        elements[2] = zValue;
    }

    template <Scalarable TScalar>
    Float32 Vector3D<TScalar>::GetMagnitude() const noexcept
        requires Floatable<TScalar>
    {
        return std::sqrt(GetSqrMagnitude());
    }

    template <Scalarable TScalar>
    Float32 Vector3D<TScalar>::GetSqrMagnitude() const noexcept
        requires Floatable<TScalar>
    {
        return elements[0] * elements[0] + elements[1] * elements[1] + elements[2] * elements[2];
    }

    template <Scalarable TScalar>
    Float32 Vector3D<TScalar>::GetLength() const noexcept
        requires Floatable<TScalar>
    {
        return GetMagnitude();
    }

    template <Scalarable TScalar>
    Float32 Vector3D<TScalar>::GetLengthSquared() const noexcept
        requires Floatable<TScalar>
    {
        return GetSqrMagnitude();
    }

    template <Scalarable TScalar>
    Vector3D<TScalar> Vector3D<TScalar>::GetNormalized() const noexcept
        requires Floatable<TScalar>
    {
        return Normalize(*this);
    }

    template <Scalarable TScalar>
    bool Vector3D<TScalar>::IsZero(Float32 epsilon) const noexcept
    {
        if constexpr (Floatable<TScalar>)
        {
            return std::abs(elements[0]) <= epsilon && std::abs(elements[1]) <= epsilon &&
                    std::abs(elements[2]) <= epsilon;
        }
        else
        {
            return elements[0] == 0 && elements[1] == 0 && elements[2] == 0;
        }
    }

    template <Scalarable TScalar>
    bool Vector3D<TScalar>::IsFinite() const noexcept
    {
        if constexpr (Floatable<TScalar>)
        {
            return std::isfinite(elements[0]) && std::isfinite(elements[1]) && std::isfinite(elements[2]);
        }
        else
        {
            return true;
        }
    }

    template <Scalarable TScalar>
    void Vector3D<TScalar>::Normalize() noexcept
        requires Floatable<TScalar>
    {
        *this = Normalize(*this);
    }

    template <Scalarable TScalar>
    Float32 Vector3D<TScalar>::Dot(const Vector3D& other) const noexcept
        requires Floatable<TScalar>
    {
        return Dot(*this, other);
    }

    template <Scalarable TScalar>
    Vector3D<TScalar> Vector3D<TScalar>::GetZero() noexcept
    {
        return Vector3D(TScalar{0}, TScalar{0}, TScalar{0});
    }

    template <Scalarable TScalar>
    Vector3D<TScalar> Vector3D<TScalar>::GetOne() noexcept
    {
        return Vector3D(TScalar{1}, TScalar{1}, TScalar{1});
    }

    template <Scalarable TScalar>
    Vector3D<TScalar> Vector3D<TScalar>::GetUp() noexcept
    {
        return Vector3D(TScalar{0}, TScalar{1}, TScalar{0});
    }

    template <Scalarable TScalar>
    Vector3D<TScalar> Vector3D<TScalar>::GetDown() noexcept
    {
        return Vector3D(TScalar{0}, TScalar{-1}, TScalar{0});
    }

    template <Scalarable TScalar>
    Vector3D<TScalar> Vector3D<TScalar>::GetLeft() noexcept
    {
        return Vector3D(TScalar{-1}, TScalar{0}, TScalar{0});
    }

    template <Scalarable TScalar>
    Vector3D<TScalar> Vector3D<TScalar>::GetRight() noexcept
    {
        return Vector3D(TScalar{1}, TScalar{0}, TScalar{0});
    }

    template <Scalarable TScalar>
    Vector3D<TScalar> Vector3D<TScalar>::GetForward() noexcept
    {
        return Vector3D(TScalar{0}, TScalar{0}, TScalar{1});
    }

    template <Scalarable TScalar>
    Vector3D<TScalar> Vector3D<TScalar>::GetBack() noexcept
    {
        return Vector3D(TScalar{0}, TScalar{0}, TScalar{-1});
    }

    template <Scalarable TScalar>
    Vector3D<TScalar> Vector3D<TScalar>::GetPositiveInfinity() noexcept
        requires Floatable<TScalar>
    {
        return Vector3D(Mathf::POSITIVE_INF, Mathf::POSITIVE_INF, Mathf::POSITIVE_INF);
    }

    template <Scalarable TScalar>
    Vector3D<TScalar> Vector3D<TScalar>::GetNegativeInfinity() noexcept
        requires Floatable<TScalar>
    {
        return Vector3D(Mathf::NEGATIVE_INF, Mathf::NEGATIVE_INF, Mathf::NEGATIVE_INF);
    }

    template <Scalarable TScalar>
    bool Vector3D<TScalar>::IsApproximately(const Vector3D& lhs, const Vector3D& rhs, Float32 epsilon) noexcept
        requires Floatable<TScalar>
    {
        return std::abs(lhs.elements[0] - rhs.elements[0]) <= epsilon &&
                std::abs(lhs.elements[1] - rhs.elements[1]) <= epsilon &&
                std::abs(lhs.elements[2] - rhs.elements[2]) <= epsilon;
    }

    template <Scalarable TScalar>
    Vector3D<TScalar> Vector3D<TScalar>::Max(const Vector3D& a, const Vector3D& b) noexcept
        requires Floatable<TScalar>
    {
        return Vector3D(std::max(a.elements[0], b.elements[0]),
                        std::max(a.elements[1], b.elements[1]),
                        std::max(a.elements[2], b.elements[2]));
    }

    template <Scalarable TScalar>
    Vector3D<TScalar> Vector3D<TScalar>::Min(const Vector3D& a, const Vector3D& b) noexcept
        requires Floatable<TScalar>
    {
        return Vector3D(std::min(a.elements[0], b.elements[0]),
                        std::min(a.elements[1], b.elements[1]),
                        std::min(a.elements[2], b.elements[2]));
    }

    template <Scalarable TScalar>
    Vector3D<TScalar> Vector3D<TScalar>::Clamp(const Vector3D& value, const Vector3D& min, const Vector3D& max) noexcept
        requires Floatable<TScalar>
    {
        return Vector3D(std::clamp(value.elements[0], min.elements[0], max.elements[0]),
                        std::clamp(value.elements[1], min.elements[1], max.elements[1]),
                        std::clamp(value.elements[2], min.elements[2], max.elements[2]));
    }

    template <Scalarable TScalar>
    Vector3D<TScalar> Vector3D<TScalar>::ClampMagnitude(const Vector3D& vector, Float32 maxLength) noexcept
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
    Vector3D<TScalar> Vector3D<TScalar>::Scale(const Vector3D& vector, const Vector3D& scale) noexcept
        requires Floatable<TScalar>
    {
        return Vector3D(vector.elements[0] * scale.elements[0],
                        vector.elements[1] * scale.elements[1],
                        vector.elements[2] * scale.elements[2]);
    }

    template <Scalarable TScalar>
    Vector3D<TScalar> Vector3D<TScalar>::Normalize(const Vector3D& value) noexcept
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
    void Vector3D<TScalar>::OrthoNormalize(Vector3D& normal, Vector3D& tangent) noexcept
        requires Floatable<TScalar>
    {
        normal  = Normalize(normal);
        tangent = Normalize(tangent - Project(tangent, normal));
    }

    template <Scalarable TScalar>
    void Vector3D<TScalar>::OrthoNormalize(Vector3D& normal, Vector3D& tangent, Vector3D& binormal) noexcept
        requires Floatable<TScalar>
    {
        OrthoNormalize(normal, tangent);
        binormal = binormal - Project(binormal, normal) - Project(binormal, tangent);
        binormal = Normalize(binormal);
    }

    template <Scalarable TScalar>
    Float32 Vector3D<TScalar>::Dot(const Vector3D& a, const Vector3D& b) noexcept
        requires Floatable<TScalar>
    {
        return a.elements[0] * b.elements[0] + a.elements[1] * b.elements[1] + a.elements[2] * b.elements[2];
    }

    template <Scalarable TScalar>
    Vector3D<TScalar> Vector3D<TScalar>::Cross(const Vector3D& a, const Vector3D& b) noexcept
        requires Floatable<TScalar>
    {
        return Vector3D(a.elements[1] * b.elements[2] - a.elements[2] * b.elements[1],
                        a.elements[2] * b.elements[0] - a.elements[0] * b.elements[2],
                        a.elements[0] * b.elements[1] - a.elements[1] * b.elements[0]);
    }

    template <Scalarable TScalar>
    Float32 Vector3D<TScalar>::Distance(const Vector3D& a, const Vector3D& b) noexcept
        requires Floatable<TScalar>
    {
        return (a - b).GetMagnitude();
    }

    template <Scalarable TScalar>
    Float32 Vector3D<TScalar>::Angle(const Vector3D& a, const Vector3D& b) noexcept
        requires Floatable<TScalar>
    {
        const Vector3D from = Normalize(a);
        const Vector3D to   = Normalize(b);
        const Float32  dot  = std::clamp(Dot(from, to), -1.0f, 1.0f);
        return std::acos(dot);
    }

    template <Scalarable TScalar>
    Float32 Vector3D<TScalar>::SignedAngle(const Vector3D& from, const Vector3D& to, const Vector3D& axis) noexcept
        requires Floatable<TScalar>
    {
        const Float32 unsignedAngle = Angle(from, to);
        const Float32 sign          = Dot(axis, Cross(from, to));
        return (sign >= 0.0f) ? unsignedAngle : -unsignedAngle;
    }

    template <Scalarable TScalar>
    Vector3D<TScalar> Vector3D<TScalar>::Project(const Vector3D& vector, const Vector3D& onNormal) noexcept
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
    Vector3D<TScalar> Vector3D<TScalar>::ProjectOnPlane(const Vector3D& vector, const Vector3D& planeNormal) noexcept
        requires Floatable<TScalar>
    {
        return vector - Project(vector, planeNormal);
    }

    template <Scalarable TScalar>
    Vector3D<TScalar> Vector3D<TScalar>::Reflect(const Vector3D& vector, const Vector3D& normal) noexcept
        requires Floatable<TScalar>
    {
        return vector - normal * (2.0f * Dot(vector, normal));
    }

    template <Scalarable TScalar>
    Vector3D<TScalar> Vector3D<TScalar>::Refract(const Vector3D& vector, const Vector3D& normal, Float32 eta) noexcept
        requires Floatable<TScalar>
    {
        const Float32 cosI  = Dot(normal, vector);
        const Float32 sinT2 = eta * eta * (1.0f - cosI * cosI);
        if (sinT2 > 1.0f)
        {
            return GetZero();
        }
        const Float32 cosT = std::sqrt(1.0f - sinT2);
        return vector * eta - normal * (eta * cosI + cosT);
    }

    template <Scalarable TScalar>
    Vector3D<TScalar> Vector3D<TScalar>::Lerp(const Vector3D& a, const Vector3D& b, Float32 t) noexcept
        requires Floatable<TScalar>
    {
        return LerpUnclamped(a, b, std::clamp(t, 0.0f, 1.0f));
    }

    template <Scalarable TScalar>
    Vector3D<TScalar> Vector3D<TScalar>::LerpUnclamped(const Vector3D& a, const Vector3D& b, Float32 t) noexcept
        requires Floatable<TScalar>
    {
        return Vector3D(Mathf::LerpUnclamped(a.elements[0], b.elements[0], t),
                        Mathf::LerpUnclamped(a.elements[1], b.elements[1], t),
                        Mathf::LerpUnclamped(a.elements[2], b.elements[2], t));
    }

    template <Scalarable TScalar>
    Vector3D<TScalar> Vector3D<TScalar>::Slerp(const Vector3D& a, const Vector3D& b, Float32 t) noexcept
        requires Floatable<TScalar>
    {
        return SlerpUnclamped(a, b, std::clamp(t, 0.0f, 1.0f));
    }

    template <Scalarable TScalar>
    Vector3D<TScalar> Vector3D<TScalar>::SlerpUnclamped(const Vector3D& a, const Vector3D& b, Float32 t) noexcept
        requires Floatable<TScalar>
    {
        const Float32 aMag = a.GetMagnitude();
        const Float32 bMag = b.GetMagnitude();

        if (aMag <= Mathf::EPSILON || bMag <= Mathf::EPSILON)
        {
            return LerpUnclamped(a, b, t);
        }

        const Vector3D from = a / aMag;
        const Vector3D to   = b / bMag;

        const Float32 dot   = std::clamp(Dot(from, to), -1.0f, 1.0f);
        const Float32 theta = std::acos(dot) * t;

        Vector3D      relative    = to - from * dot;
        const Float32 relativeMag = relative.GetMagnitude();
        if (relativeMag <= Mathf::EPSILON)
        {
            return LerpUnclamped(a, b, t);
        }

        relative /= relativeMag;
        const Vector3D direction = from * std::cos(theta) + relative * std::sin(theta);
        const Float32  magnitude = Mathf::LerpUnclamped(aMag, bMag, t);

        return direction * magnitude;
    }

    template <Scalarable TScalar>
    Vector3D<TScalar> Vector3D<TScalar>::MoveTowards(const Vector3D& current,
                                                     const Vector3D& target,
                                                     Float32         maxDistanceDelta) noexcept
        requires Floatable<TScalar>
    {
        const Vector3D delta    = target - current;
        const Float32  distance = delta.GetMagnitude();

        if (distance <= maxDistanceDelta || distance <= Mathf::EPSILON)
        {
            return target;
        }

        return current + (delta / distance) * maxDistanceDelta;
    }

    template <Scalarable TScalar>
    Vector3D<TScalar> Vector3D<TScalar>::RotateTowards(const Vector3D& current,
                                                       const Vector3D& target,
                                                       Float32         maxRadiansDelta,
                                                       Float32         maxMagnitudeDelta) noexcept
        requires Floatable<TScalar>
    {
        const Float32 currentMag = current.GetMagnitude();
        const Float32 targetMag  = target.GetMagnitude();

        if (currentMag <= Mathf::EPSILON || targetMag <= Mathf::EPSILON)
        {
            return MoveTowards(current, target, maxMagnitudeDelta);
        }

        const Vector3D currentDir = current / currentMag;
        const Vector3D targetDir  = target / targetMag;

        const Float32  angle    = Angle(currentDir, targetDir);
        const Float32  t        = (angle <= Mathf::EPSILON) ? 1.0f : std::min(1.0f, maxRadiansDelta / angle);
        const Vector3D newDir   = SlerpUnclamped(currentDir, targetDir, t).GetNormalized();
        const Float32  deltaMag = std::clamp(targetMag - currentMag, -maxMagnitudeDelta, maxMagnitudeDelta);

        return newDir * (currentMag + deltaMag);
    }

    template <Scalarable TScalar>
    Vector3D<TScalar> Vector3D<TScalar>::SmoothDamp(const Vector3D& current,
                                                    const Vector3D& target,
                                                    Vector3D&       currentVelocity,
                                                    Float32         smoothTime,
                                                    Float32         maxSpeed,
                                                    Float32         deltaTime) noexcept
        requires Floatable<TScalar>
    {
        smoothTime          = std::max(0.0001f, smoothTime);
        const Float32 omega = 2.0f / smoothTime;
        const Float32 xVal  = omega * deltaTime;
        const Float32 exp   = 1.0f / (1.0f + xVal + 0.48f * xVal * xVal + 0.235f * xVal * xVal * xVal);

        Vector3D       change         = current - target;
        const Vector3D originalTarget = target;

        const Float32 maxChange       = maxSpeed * smoothTime;
        change                        = ClampMagnitude(change, maxChange);
        const Vector3D adjustedTarget = current - change;

        const Vector3D temp = (currentVelocity + change * omega) * deltaTime;
        currentVelocity     = (currentVelocity - temp * omega) * exp;
        Vector3D output     = adjustedTarget + (change + temp) * exp;

        if (Dot(originalTarget - current, output - originalTarget) > 0.0f)
        {
            output          = originalTarget;
            currentVelocity = Vector3D(0.0f, 0.0f, 0.0f);
        }

        return output;
    }

    template class GARAGE_API Vector3D<Int32>;
    template class GARAGE_API Vector3D<Float32>;

    Vector3D<Int32> operator*(Int32 scalar, const Vector3D<Int32>& vector) noexcept
    {
        return vector * scalar;
    }

    Vector3D<Float32> operator*(Float32 scalar, const Vector3D<Float32>& vector) noexcept
    {
        return vector * scalar;
    }
} // namespace Garage
