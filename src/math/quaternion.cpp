#include "garage/math/quaternion.hpp"

#include <algorithm>
#include <cmath>

#include "garage/core/defines.hpp"

#include "garage/math/mathf.hpp"
#include "garage/math/matrix4x4.hpp"

namespace Garage
{
    namespace
    {
        [[nodiscard]] Quaternion<Float32> QuaternionFromBasis(const Vector3D<Float32>& r0,
                                                              const Vector3D<Float32>& r1,
                                                              const Vector3D<Float32>& r2) noexcept
        {
            const Float32 m00   = r0.GetX();
            const Float32 m01   = r0.GetY();
            const Float32 m02   = r0.GetZ();
            const Float32 m10   = r1.GetX();
            const Float32 m11   = r1.GetY();
            const Float32 m12   = r1.GetZ();
            const Float32 m20   = r2.GetX();
            const Float32 m21   = r2.GetY();
            const Float32 m22   = r2.GetZ();
            const Float32 trace = m00 + m11 + m22;

            if (trace > 0.0f)
            {
                const Float32 s = std::sqrt(trace + 1.0f) * 2.0f;
                return Quaternion<Float32>::Normalize(
                        Quaternion<Float32>((m12 - m21) / s, (m20 - m02) / s, (m01 - m10) / s, 0.25f * s));
            }
            if (m00 > m11 && m00 > m22)
            {
                const Float32 s = std::sqrt(1.0f + m00 - m11 - m22) * 2.0f;
                return Quaternion<Float32>::Normalize(
                        Quaternion<Float32>(0.25f * s, (m01 + m10) / s, (m20 + m02) / s, (m12 - m21) / s));
            }
            if (m11 > m22)
            {
                const Float32 s = std::sqrt(1.0f + m11 - m00 - m22) * 2.0f;
                return Quaternion<Float32>::Normalize(
                        Quaternion<Float32>((m01 + m10) / s, 0.25f * s, (m12 + m21) / s, (m20 - m02) / s));
            }

            const Float32 s = std::sqrt(1.0f + m22 - m00 - m11) * 2.0f;
            return Quaternion<Float32>::Normalize(
                    Quaternion<Float32>((m20 + m02) / s, (m12 + m21) / s, 0.25f * s, (m01 - m10) / s));
        }
    } // namespace

    template <Scalarable TScalar>
    Quaternion<TScalar>::Quaternion() noexcept = default;

    template <Scalarable TScalar>
    Quaternion<TScalar>::Quaternion(TScalar value) noexcept
        : elements{value, value, value, value}
    {
    }

    template <Scalarable TScalar>
    Quaternion<TScalar>::Quaternion(TScalar x, TScalar y, TScalar z, TScalar w) noexcept
        : elements{x, y, z, w}
    {
    }

    template <Scalarable TScalar>
    Quaternion<TScalar>::operator const TScalar*() const noexcept
    {
        return elements.data();
    }

    template <Scalarable TScalar>
    Quaternion<TScalar>::operator TScalar*() noexcept
    {
        return elements.data();
    }

    template <Scalarable TScalar>
    TScalar Quaternion<TScalar>::operator[](USize index) const noexcept
    {
        ASSERT(index < 4);
        return elements[index];
    }

    template <Scalarable TScalar>
    TScalar& Quaternion<TScalar>::operator[](USize index) noexcept
    {
        ASSERT(index < 4);
        return elements[index];
    }

    template <Scalarable TScalar>
    Quaternion<TScalar> Quaternion<TScalar>::operator+(const Quaternion& other) const noexcept
    {
        return Quaternion(elements[0] + other.elements[0],
                          elements[1] + other.elements[1],
                          elements[2] + other.elements[2],
                          elements[3] + other.elements[3]);
    }

    template <Scalarable TScalar>
    Quaternion<TScalar>& Quaternion<TScalar>::operator+=(const Quaternion& other) noexcept
    {
        *this = *this + other;
        return *this;
    }

    template <Scalarable TScalar>
    Quaternion<TScalar> Quaternion<TScalar>::operator-(const Quaternion& other) const noexcept
    {
        return Quaternion(elements[0] - other.elements[0],
                          elements[1] - other.elements[1],
                          elements[2] - other.elements[2],
                          elements[3] - other.elements[3]);
    }

    template <Scalarable TScalar>
    Quaternion<TScalar>& Quaternion<TScalar>::operator-=(const Quaternion& other) noexcept
    {
        *this = *this - other;
        return *this;
    }

    template <Scalarable TScalar>
    Quaternion<TScalar> Quaternion<TScalar>::operator*(TScalar scalar) const noexcept
    {
        return Quaternion(elements[0] * scalar, elements[1] * scalar, elements[2] * scalar, elements[3] * scalar);
    }

    template <Scalarable TScalar>
    Quaternion<TScalar> Quaternion<TScalar>::operator*(const Quaternion& other) const noexcept
    {
        return Quaternion(other.elements[3] * elements[0] + other.elements[0] * elements[3] +
                                  other.elements[1] * elements[2] - other.elements[2] * elements[1],
                          other.elements[3] * elements[1] - other.elements[0] * elements[2] +
                                  other.elements[1] * elements[3] + other.elements[2] * elements[0],
                          other.elements[3] * elements[2] + other.elements[0] * elements[1] -
                                  other.elements[1] * elements[0] + other.elements[2] * elements[3],
                          other.elements[3] * elements[3] - other.elements[0] * elements[0] -
                                  other.elements[1] * elements[1] - other.elements[2] * elements[2]);
    }

    template <Scalarable TScalar>
    Vector3D<Float32> Quaternion<TScalar>::operator*(const Vector3D<Float32>& vector) const noexcept
        requires Floatable<TScalar>
    {
        const Vector3D<Float32> qVec(elements[0], elements[1], elements[2]);
        const Vector3D<Float32> t = Vector3D<Float32>::Cross(qVec, vector) * 2.0f;
        return vector + t * elements[3] + Vector3D<Float32>::Cross(qVec, t);
    }

    template <Scalarable TScalar>
    Quaternion<TScalar>& Quaternion<TScalar>::operator*=(TScalar scalar) noexcept
    {
        *this = *this * scalar;
        return *this;
    }

    template <Scalarable TScalar>
    Quaternion<TScalar>& Quaternion<TScalar>::operator*=(const Quaternion& other) noexcept
    {
        *this = *this * other;
        return *this;
    }

    template <Scalarable TScalar>
    Quaternion<TScalar> Quaternion<TScalar>::operator/(TScalar scalar) const noexcept
    {
        ASSERT(scalar != TScalar{});
        return Quaternion(elements[0] / scalar, elements[1] / scalar, elements[2] / scalar, elements[3] / scalar);
    }

    template <Scalarable TScalar>
    Quaternion<TScalar>& Quaternion<TScalar>::operator/=(TScalar scalar) noexcept
    {
        *this = *this / scalar;
        return *this;
    }

    template <Scalarable TScalar>
    bool Quaternion<TScalar>::operator==(const Quaternion& other) const noexcept
    {
        return (*this <=> other) == std::partial_ordering::equivalent;
    }

    template <Scalarable TScalar>
    bool Quaternion<TScalar>::operator!=(const Quaternion& other) const noexcept
    {
        return !(*this == other);
    }

    template <Scalarable TScalar>
    std::partial_ordering Quaternion<TScalar>::operator<=>(const Quaternion& other) const noexcept
    {
        if (const auto order = elements[0] <=> other.elements[0]; order != 0)
        {
            return order;
        }
        if (const auto order = elements[1] <=> other.elements[1]; order != 0)
        {
            return order;
        }
        if (const auto order = elements[2] <=> other.elements[2]; order != 0)
        {
            return order;
        }
        return elements[3] <=> other.elements[3];
    }

    template <Scalarable TScalar>
    TScalar Quaternion<TScalar>::GetX() const noexcept
    {
        return elements[0];
    }

    template <Scalarable TScalar>
    void Quaternion<TScalar>::SetX(TScalar component) noexcept
    {
        elements[0] = component;
    }

    template <Scalarable TScalar>
    TScalar Quaternion<TScalar>::GetY() const noexcept
    {
        return elements[1];
    }

    template <Scalarable TScalar>
    void Quaternion<TScalar>::SetY(TScalar component) noexcept
    {
        elements[1] = component;
    }

    template <Scalarable TScalar>
    TScalar Quaternion<TScalar>::GetZ() const noexcept
    {
        return elements[2];
    }

    template <Scalarable TScalar>
    void Quaternion<TScalar>::SetZ(TScalar component) noexcept
    {
        elements[2] = component;
    }

    template <Scalarable TScalar>
    TScalar Quaternion<TScalar>::GetW() const noexcept
    {
        return elements[3];
    }

    template <Scalarable TScalar>
    void Quaternion<TScalar>::SetW(TScalar component) noexcept
    {
        elements[3] = component;
    }

    template <Scalarable TScalar>
    void Quaternion<TScalar>::Set(TScalar xValue, TScalar yValue, TScalar zValue, TScalar wValue) noexcept
    {
        elements[0] = xValue;
        elements[1] = yValue;
        elements[2] = zValue;
        elements[3] = wValue;
    }

    template <Scalarable TScalar>
    Vector3D<Float32> Quaternion<TScalar>::GetEulerAngles() const noexcept
        requires Floatable<TScalar>
    {
        const Matrix4x4<Float32> matrix = Matrix4x4<Float32>::Rotate(Normalize(*this));
        const Float32            pitch  = std::asin(std::clamp(-matrix[9], -1.0f, 1.0f));
        Float32                  yaw    = 0.0f;
        Float32                  roll   = 0.0f;
        if (std::abs(std::cos(pitch)) > 0.00001f)
        {
            yaw  = std::atan2(matrix[8], matrix[10]);
            roll = std::atan2(matrix[1], matrix[5]);
        }
        else
        {
            yaw  = std::atan2(-matrix[2], matrix[0]);
            roll = 0.0f;
        }
        return Vector3D<Float32>(pitch, yaw, roll) * Mathf::RAD_TO_DEG;
    }

    template <Scalarable TScalar>
    void Quaternion<TScalar>::SetEulerAngles(const Vector3D<Float32>& eulerDegrees) noexcept
        requires Floatable<TScalar>
    {
        *this = Euler(eulerDegrees);
    }

    template <Scalarable TScalar>
    Float32 Quaternion<TScalar>::GetMagnitude() const noexcept
        requires Floatable<TScalar>
    {
        return std::sqrt(GetSqrMagnitude());
    }

    template <Scalarable TScalar>
    Float32 Quaternion<TScalar>::GetSqrMagnitude() const noexcept
        requires Floatable<TScalar>
    {
        return Dot(*this, *this);
    }

    template <Scalarable TScalar>
    Quaternion<TScalar> Quaternion<TScalar>::GetNormalized() const noexcept
        requires Floatable<TScalar>
    {
        return Normalize(*this);
    }

    template <Scalarable TScalar>
    void Quaternion<TScalar>::SetFromToRotation(const Vector3D<Float32>& from, const Vector3D<Float32>& to) noexcept
        requires Floatable<TScalar>
    {
        *this = FromToRotation(from, to);
    }

    template <Scalarable TScalar>
    void Quaternion<TScalar>::SetLookRotation(const Vector3D<Float32>& view) noexcept
        requires Floatable<TScalar>
    {
        *this = LookRotation(view, Vector3D<Float32>::GetUp());
    }

    template <Scalarable TScalar>
    void Quaternion<TScalar>::SetLookRotation(const Vector3D<Float32>& view, const Vector3D<Float32>& up) noexcept
        requires Floatable<TScalar>
    {
        *this = LookRotation(view, up);
    }

    template <Scalarable TScalar>
    bool Quaternion<TScalar>::IsFinite() const noexcept
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
    bool Quaternion<TScalar>::IsNormalized(Float32 epsilon) const noexcept
        requires Floatable<TScalar>
    {
        return std::abs(GetSqrMagnitude() - 1.0f) <= epsilon;
    }

    template <Scalarable TScalar>
    void Quaternion<TScalar>::Normalize() noexcept
        requires Floatable<TScalar>
    {
        *this = GetNormalized();
    }

    template <Scalarable TScalar>
    void Quaternion<TScalar>::ToAngleAxis(Float32& angleDegrees, Vector3D<Float32>& axis) const noexcept
        requires Floatable<TScalar>
    {
        const Quaternion q         = Normalize(*this);
        const Float32    clampedW  = std::clamp(q.elements[3], -1.0f, 1.0f);
        const Float32    angleRad  = 2.0f * std::acos(clampedW);
        const Float32    sinHalfSq = 1.0f - clampedW * clampedW;
        if (sinHalfSq <= Mathf::EPSILON)
        {
            axis = Vector3D<Float32>::GetRight();
        }
        else
        {
            const Float32 invSinHalf = 1.0f / std::sqrt(sinHalfSq);
            axis                     = Vector3D<Float32>(
                    q.elements[0] * invSinHalf, q.elements[1] * invSinHalf, q.elements[2] * invSinHalf);
        }
        angleDegrees = angleRad * Mathf::RAD_TO_DEG;
    }

    template <Scalarable TScalar>
    Quaternion<TScalar> Quaternion<TScalar>::Normalize(const Quaternion& rotation) noexcept
        requires Floatable<TScalar>
    {
        const Float32 mag = rotation.GetMagnitude();
        if (mag <= Mathf::EPSILON)
        {
            return GetIdentity();
        }
        return rotation / mag;
    }

    template <Scalarable TScalar>
    Quaternion<TScalar> Quaternion<TScalar>::GetIdentity() noexcept
    {
        return Quaternion(TScalar{0}, TScalar{0}, TScalar{0}, static_cast<TScalar>(1));
    }

    template <Scalarable TScalar>
    bool Quaternion<TScalar>::IsApproximately(const Quaternion& lhs, const Quaternion& rhs, Float32 epsilon) noexcept
        requires Floatable<TScalar>
    {
        return std::abs(Dot(lhs, rhs)) >= (1.0f - epsilon);
    }

    template <Scalarable TScalar>
    Float32 Quaternion<TScalar>::Angle(const Quaternion& a, const Quaternion& b) noexcept
        requires Floatable<TScalar>
    {
        const Float32 d = std::clamp(std::abs(Dot(a, b)), 0.0f, 1.0f);
        return (2.0f * std::acos(d)) * Mathf::RAD_TO_DEG;
    }

    template <Scalarable TScalar>
    Quaternion<TScalar> Quaternion<TScalar>::AngleAxis(Float32 angleDegrees, Vector3D<Float32> axis) noexcept
        requires Floatable<TScalar>
    {
        const Vector3D<Float32> normAxis = Vector3D<Float32>::Normalize(axis);
        const Float32           halfRad  = (angleDegrees * Mathf::DEG_TO_RAD) * 0.5f;
        const Float32           s        = std::sin(halfRad);
        const Float32           c        = std::cos(halfRad);
        return Quaternion(normAxis.GetX() * s, normAxis.GetY() * s, normAxis.GetZ() * s, c);
    }

    template <Scalarable TScalar>
    Quaternion<TScalar> Quaternion<TScalar>::Euler(Float32 xDegrees, Float32 yDegrees, Float32 zDegrees) noexcept
        requires Floatable<TScalar>
    {
        const Float32 halfPitch = (xDegrees * Mathf::DEG_TO_RAD) * 0.5f;
        const Float32 halfYaw   = (yDegrees * Mathf::DEG_TO_RAD) * 0.5f;
        const Float32 halfRoll  = (zDegrees * Mathf::DEG_TO_RAD) * 0.5f;

        const Float32 sp = std::sin(halfPitch);
        const Float32 cp = std::cos(halfPitch);
        const Float32 sy = std::sin(halfYaw);
        const Float32 cy = std::cos(halfYaw);
        const Float32 sr = std::sin(halfRoll);
        const Float32 cr = std::cos(halfRoll);

        return Quaternion(cy * sp * cr + sy * cp * sr,
                          sy * cp * cr - cy * sp * sr,
                          cy * cp * sr - sy * sp * cr,
                          cy * cp * cr + sy * sp * sr);
    }

    template <Scalarable TScalar>
    Quaternion<TScalar> Quaternion<TScalar>::Euler(const Vector3D<Float32>& eulerDegrees) noexcept
        requires Floatable<TScalar>
    {
        return Euler(eulerDegrees.GetX(), eulerDegrees.GetY(), eulerDegrees.GetZ());
    }

    template <Scalarable TScalar>
    Quaternion<TScalar> Quaternion<TScalar>::Inverse(const Quaternion& rotation) noexcept
        requires Floatable<TScalar>
    {
        const Float32 lenSq = rotation.GetSqrMagnitude();
        if (lenSq <= Mathf::EPSILON)
        {
            return Quaternion(0.0f, 0.0f, 0.0f, 0.0f);
        }
        return Conjugate(rotation) / lenSq;
    }

    template <Scalarable TScalar>
    Quaternion<TScalar> Quaternion<TScalar>::Conjugate(const Quaternion& rotation) noexcept
        requires Floatable<TScalar>
    {
        return Quaternion(-rotation.elements[0], -rotation.elements[1], -rotation.elements[2], rotation.elements[3]);
    }

    template <Scalarable TScalar>
    Float32 Quaternion<TScalar>::Dot(const Quaternion& a, const Quaternion& b) noexcept
        requires Floatable<TScalar>
    {
        return a.elements[0] * b.elements[0] + a.elements[1] * b.elements[1] + a.elements[2] * b.elements[2] +
                a.elements[3] * b.elements[3];
    }

    template <Scalarable TScalar>
    Quaternion<TScalar> Quaternion<TScalar>::Lerp(const Quaternion& a, const Quaternion& b, Float32 t) noexcept
        requires Floatable<TScalar>
    {
        return LerpUnclamped(a, b, std::clamp(t, 0.0f, 1.0f));
    }

    template <Scalarable TScalar>
    Quaternion<TScalar> Quaternion<TScalar>::LerpUnclamped(const Quaternion& a, const Quaternion& b, Float32 t) noexcept
        requires Floatable<TScalar>
    {
        return Normalize(Quaternion(Mathf::LerpUnclamped(a.elements[0], b.elements[0], t),
                                    Mathf::LerpUnclamped(a.elements[1], b.elements[1], t),
                                    Mathf::LerpUnclamped(a.elements[2], b.elements[2], t),
                                    Mathf::LerpUnclamped(a.elements[3], b.elements[3], t)));
    }

    template <Scalarable TScalar>
    Quaternion<TScalar> Quaternion<TScalar>::Slerp(const Quaternion& a, const Quaternion& b, Float32 t) noexcept
        requires Floatable<TScalar>
    {
        return SlerpUnclamped(a, b, std::clamp(t, 0.0f, 1.0f));
    }

    template <Scalarable TScalar>
    Quaternion<TScalar>
            Quaternion<TScalar>::SlerpUnclamped(const Quaternion& a, const Quaternion& b, Float32 t) noexcept
        requires Floatable<TScalar>
    {
        Float32    cosOmega = Dot(a, b);
        Quaternion end      = b;
        if (cosOmega < 0.0f)
        {
            cosOmega = -cosOmega;
            end      = Quaternion(-b.elements[0], -b.elements[1], -b.elements[2], -b.elements[3]);
        }

        Float32 k0 = 0.0f;
        Float32 k1 = 0.0f;
        if (cosOmega > 0.9999f)
        {
            k0 = 1.0f - t;
            k1 = t;
        }
        else
        {
            const Float32 sinOmega = std::sqrt(1.0f - cosOmega * cosOmega);
            const Float32 omega    = std::atan2(sinOmega, cosOmega);
            const Float32 invSin   = 1.0f / sinOmega;
            k0                     = std::sin((1.0f - t) * omega) * invSin;
            k1                     = std::sin(t * omega) * invSin;
        }

        return Quaternion(a.elements[0] * k0 + end.elements[0] * k1,
                          a.elements[1] * k0 + end.elements[1] * k1,
                          a.elements[2] * k0 + end.elements[2] * k1,
                          a.elements[3] * k0 + end.elements[3] * k1);
    }

    template <Scalarable TScalar>
    Quaternion<TScalar> Quaternion<TScalar>::FromToRotation(const Vector3D<Float32>& fromDirection,
                                                            const Vector3D<Float32>& toDirection) noexcept
        requires Floatable<TScalar>
    {
        const Vector3D<Float32> from = Vector3D<Float32>::Normalize(fromDirection);
        const Vector3D<Float32> to   = Vector3D<Float32>::Normalize(toDirection);
        const Float32           dot  = std::clamp(Vector3D<Float32>::Dot(from, to), -1.0f, 1.0f);

        if (dot > 1.0f - Mathf::EPSILON)
        {
            return GetIdentity();
        }

        if (dot < -1.0f + Mathf::EPSILON)
        {
            Vector3D<Float32> axis = Vector3D<Float32>::Cross(Vector3D<Float32>::GetRight(), from);
            if (axis.GetSqrMagnitude() <= Mathf::EPSILON)
            {
                axis = Vector3D<Float32>::Cross(Vector3D<Float32>::GetUp(), from);
            }
            return AngleAxis(180.0f, axis);
        }

        const Vector3D<Float32> axis         = Vector3D<Float32>::Cross(from, to);
        const Float32           angleDegrees = std::acos(dot) * Mathf::RAD_TO_DEG;
        return AngleAxis(angleDegrees, axis);
    }

    template <Scalarable TScalar>
    Quaternion<TScalar> Quaternion<TScalar>::LookRotation(const Vector3D<Float32>& forward,
                                                          const Vector3D<Float32>& up) noexcept
        requires Floatable<TScalar>
    {
        const Vector3D<Float32> normForward = Vector3D<Float32>::Normalize(forward);
        if (normForward.GetSqrMagnitude() <= Mathf::EPSILON)
        {
            return GetIdentity();
        }

        Vector3D<Float32> right = Vector3D<Float32>::Normalize(Vector3D<Float32>::Cross(up, normForward));
        if (right.GetSqrMagnitude() <= Mathf::EPSILON)
        {
            right = Vector3D<Float32>::Normalize(Vector3D<Float32>::Cross(Vector3D<Float32>::GetUp(), normForward));
            if (right.GetSqrMagnitude() <= Mathf::EPSILON)
            {
                right = Vector3D<Float32>::Normalize(
                        Vector3D<Float32>::Cross(Vector3D<Float32>::GetRight(), normForward));
            }
        }

        const Vector3D<Float32> orthoUp = Vector3D<Float32>::Cross(normForward, right);
        return QuaternionFromBasis(right, orthoUp, normForward);
    }

    template <Scalarable TScalar>
    Quaternion<TScalar> Quaternion<TScalar>::RotateTowards(const Quaternion& from,
                                                           const Quaternion& to,
                                                           Float32           maxDegreesDelta) noexcept
        requires Floatable<TScalar>
    {
        const Float32 angleDegrees = Angle(from, to);
        if (angleDegrees <= Mathf::EPSILON)
        {
            return to;
        }

        const Float32 t = std::min(1.0f, maxDegreesDelta / angleDegrees);
        return SlerpUnclamped(from, to, t);
    }

    template class Quaternion<Int32>;
    template class Quaternion<Float32>;
} // namespace Garage
