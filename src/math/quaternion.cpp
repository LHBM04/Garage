#include "garage/math/quaternion.hpp"

#include <algorithm>
#include <cmath>
#include <compare>

#include "garage/core/common.hpp"

#include "garage/math/mathf.hpp"
#include "garage/math/matrix4x4.hpp"
#include "garage/math/vector3d.hpp"

namespace Garage
{
    namespace
    {
        [[nodiscard]] Quaternion<Float32> QuaternionFromBasis(const Vector3D<Float32>& r0,
                                                              const Vector3D<Float32>& r1,
                                                              const Vector3D<Float32>& r2) noexcept
        {
            const Float32 M00   = r0.GetX();
            const Float32 M01   = r0.GetY();
            const Float32 M02   = r0.GetZ();
            const Float32 M10   = r1.GetX();
            const Float32 M11   = r1.GetY();
            const Float32 M12   = r1.GetZ();
            const Float32 M20   = r2.GetX();
            const Float32 M21   = r2.GetY();
            const Float32 M22   = r2.GetZ();
            const Float32 TRACE = M00 + M11 + M22;

            if (TRACE > 0.0f)
            {
                const Float32 S = std::sqrt(TRACE + 1.0f) * 2.0f;
                return Quaternion<Float32>::Normalize(
                        Quaternion<Float32>((M12 - M21) / S, (M20 - M02) / S, (M01 - M10) / S, 0.25f * S));
            }
            if (M00 > M11 && M00 > M22)
            {
                const Float32 S = std::sqrt(1.0f + M00 - M11 - M22) * 2.0f;
                return Quaternion<Float32>::Normalize(
                        Quaternion<Float32>(0.25f * S, (M01 + M10) / S, (M20 + M02) / S, (M12 - M21) / S));
            }
            if (M11 > M22)
            {
                const Float32 S = std::sqrt(1.0f + M11 - M00 - M22) * 2.0f;
                return Quaternion<Float32>::Normalize(
                        Quaternion<Float32>((M01 + M10) / S, 0.25f * S, (M12 + M21) / S, (M20 - M02) / S));
            }

            const Float32 S = std::sqrt(1.0f + M22 - M00 - M11) * 2.0f;
            return Quaternion<Float32>::Normalize(
                    Quaternion<Float32>((M20 + M02) / S, (M12 + M21) / S, 0.25f * S, (M01 - M10) / S));
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
        const Vector3D<Float32> Q_VEC(elements[0], elements[1], elements[2]);
        const Vector3D<Float32> T = Vector3D<Float32>::Cross(Q_VEC, vector) * 2.0f;
        return vector + T * elements[3] + Vector3D<Float32>::Cross(Q_VEC, T);
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
        if (const auto ORDER = elements[0] <=> other.elements[0]; ORDER != 0)
        {
            return ORDER;
        }
        if (const auto ORDER = elements[1] <=> other.elements[1]; ORDER != 0)
        {
            return ORDER;
        }
        if (const auto ORDER = elements[2] <=> other.elements[2]; ORDER != 0)
        {
            return ORDER;
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
        const Matrix4x4<Float32> MATRIX = Matrix4x4<Float32>::Rotate(Normalize(*this));
        const Float32            PITCH  = std::asin(std::clamp(-MATRIX[9], -1.0f, 1.0f));
        Float32                  yaw    = 0.0f;
        Float32                  roll   = 0.0f;
        if (std::abs(std::cos(PITCH)) > 0.00001f)
        {
            yaw  = std::atan2(MATRIX[8], MATRIX[10]);
            roll = std::atan2(MATRIX[1], MATRIX[5]);
        }
        else
        {
            yaw  = std::atan2(-MATRIX[2], MATRIX[0]);
            roll = 0.0f;
        }
        return Vector3D<Float32>(PITCH, yaw, roll) * Mathf::RAD_TO_DEG;
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
        const Quaternion Q           = Normalize(*this);
        const Float32    CLAMPED_W   = std::clamp(Q.elements[3], -1.0f, 1.0f);
        const Float32    ANGLE_RAD   = 2.0f * std::acos(CLAMPED_W);
        const Float32    SIN_HALF_SQ = 1.0f - CLAMPED_W * CLAMPED_W;
        if (SIN_HALF_SQ <= Mathf::EPSILON)
        {
            axis = Vector3D<Float32>::GetRight();
        }
        else
        {
            const Float32 INV_SIN_HALF = 1.0f / std::sqrt(SIN_HALF_SQ);
            axis                       = Vector3D<Float32>(
                    Q.elements[0] * INV_SIN_HALF, Q.elements[1] * INV_SIN_HALF, Q.elements[2] * INV_SIN_HALF);
        }
        angleDegrees = ANGLE_RAD * Mathf::RAD_TO_DEG;
    }

    template <Scalarable TScalar>
    Quaternion<TScalar> Quaternion<TScalar>::Normalize(const Quaternion& rotation) noexcept
        requires Floatable<TScalar>
    {
        const Float32 MAG = rotation.GetMagnitude();
        if (MAG <= Mathf::EPSILON)
        {
            return GetIdentity();
        }
        return rotation / MAG;
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
        const Float32 D = std::clamp(std::abs(Dot(a, b)), 0.0f, 1.0f);
        return (2.0f * std::acos(D)) * Mathf::RAD_TO_DEG;
    }

    template <Scalarable TScalar>
    Quaternion<TScalar> Quaternion<TScalar>::AngleAxis(Float32 angleDegrees, Vector3D<Float32> axis) noexcept
        requires Floatable<TScalar>
    {
        const Vector3D<Float32> NORM_AXIS = Vector3D<Float32>::Normalize(axis);
        const Float32           HALF_RAD  = (angleDegrees * Mathf::DEG_TO_RAD) * 0.5f;
        const Float32           S         = std::sin(HALF_RAD);
        const Float32           C         = std::cos(HALF_RAD);
        return Quaternion(NORM_AXIS.GetX() * S, NORM_AXIS.GetY() * S, NORM_AXIS.GetZ() * S, C);
    }

    template <Scalarable TScalar>
    Quaternion<TScalar> Quaternion<TScalar>::Euler(Float32 xDegrees, Float32 yDegrees, Float32 zDegrees) noexcept
        requires Floatable<TScalar>
    {
        const Float32 HALF_PITCH = (xDegrees * Mathf::DEG_TO_RAD) * 0.5f;
        const Float32 HALF_YAW   = (yDegrees * Mathf::DEG_TO_RAD) * 0.5f;
        const Float32 HALF_ROLL  = (zDegrees * Mathf::DEG_TO_RAD) * 0.5f;

        const Float32 SP = std::sin(HALF_PITCH);
        const Float32 CP = std::cos(HALF_PITCH);
        const Float32 SY = std::sin(HALF_YAW);
        const Float32 CY = std::cos(HALF_YAW);
        const Float32 SR = std::sin(HALF_ROLL);
        const Float32 CR = std::cos(HALF_ROLL);

        return Quaternion(CY * SP * CR + SY * CP * SR,
                          SY * CP * CR - CY * SP * SR,
                          CY * CP * SR - SY * SP * CR,
                          CY * CP * CR + SY * SP * SR);
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
        const Float32 LEN_SQ = rotation.GetSqrMagnitude();
        if (LEN_SQ <= Mathf::EPSILON)
        {
            return Quaternion(0.0f, 0.0f, 0.0f, 0.0f);
        }
        return Conjugate(rotation) / LEN_SQ;
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
            const Float32 SIN_OMEGA = std::sqrt(1.0f - cosOmega * cosOmega);
            const Float32 OMEGA     = std::atan2(SIN_OMEGA, cosOmega);
            const Float32 INV_SIN   = 1.0f / SIN_OMEGA;
            k0                      = std::sin((1.0f - t) * OMEGA) * INV_SIN;
            k1                      = std::sin(t * OMEGA) * INV_SIN;
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
        const Vector3D<Float32> FROM = Vector3D<Float32>::Normalize(fromDirection);
        const Vector3D<Float32> TO   = Vector3D<Float32>::Normalize(toDirection);
        const Float32           DOT  = std::clamp(Vector3D<Float32>::Dot(FROM, TO), -1.0f, 1.0f);

        if (DOT > 1.0f - Mathf::EPSILON)
        {
            return GetIdentity();
        }

        if (DOT < -1.0f + Mathf::EPSILON)
        {
            Vector3D<Float32> axis = Vector3D<Float32>::Cross(Vector3D<Float32>::GetRight(), FROM);
            if (axis.GetSqrMagnitude() <= Mathf::EPSILON)
            {
                axis = Vector3D<Float32>::Cross(Vector3D<Float32>::GetUp(), FROM);
            }
            return AngleAxis(180.0f, axis);
        }

        const Vector3D<Float32> AXIS          = Vector3D<Float32>::Cross(FROM, TO);
        const Float32           ANGLE_DEGREES = std::acos(DOT) * Mathf::RAD_TO_DEG;
        return AngleAxis(ANGLE_DEGREES, AXIS);
    }

    template <Scalarable TScalar>
    Quaternion<TScalar> Quaternion<TScalar>::LookRotation(const Vector3D<Float32>& forward,
                                                          const Vector3D<Float32>& up) noexcept
        requires Floatable<TScalar>
    {
        const Vector3D<Float32> NORM_FORWARD = Vector3D<Float32>::Normalize(forward);
        if (NORM_FORWARD.GetSqrMagnitude() <= Mathf::EPSILON)
        {
            return GetIdentity();
        }

        Vector3D<Float32> right = Vector3D<Float32>::Normalize(Vector3D<Float32>::Cross(up, NORM_FORWARD));
        if (right.GetSqrMagnitude() <= Mathf::EPSILON)
        {
            right = Vector3D<Float32>::Normalize(Vector3D<Float32>::Cross(Vector3D<Float32>::GetUp(), NORM_FORWARD));
            if (right.GetSqrMagnitude() <= Mathf::EPSILON)
            {
                right = Vector3D<Float32>::Normalize(
                        Vector3D<Float32>::Cross(Vector3D<Float32>::GetRight(), NORM_FORWARD));
            }
        }

        const Vector3D<Float32> ORTHO_UP = Vector3D<Float32>::Cross(NORM_FORWARD, right);
        return QuaternionFromBasis(right, ORTHO_UP, NORM_FORWARD);
    }

    template <Scalarable TScalar>
    Quaternion<TScalar> Quaternion<TScalar>::RotateTowards(const Quaternion& from,
                                                           const Quaternion& to,
                                                           Float32           maxDegreesDelta) noexcept
        requires Floatable<TScalar>
    {
        const Float32 ANGLE_DEGREES = Angle(from, to);
        if (ANGLE_DEGREES <= Mathf::EPSILON)
        {
            return to;
        }

        const Float32 T = std::min(1.0f, maxDegreesDelta / ANGLE_DEGREES);
        return SlerpUnclamped(from, to, T);
    }

    template class GARAGE_API Quaternion<Int32>;
    template class GARAGE_API Quaternion<Float32>;
} // namespace Garage
