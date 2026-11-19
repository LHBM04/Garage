#ifndef GARAGE_MATH_QUATERNION_HPP
#define GARAGE_MATH_QUATERNION_HPP

#include <compare>

#include "../core/common.hpp"
#include "../core/common/containers.hpp"

#include "mathf.hpp"
#include "vector3d.hpp"

namespace Garage
{
    template <Scalarable TScalar>
    class Quaternion final
    {
    public:
        using ValueType = TScalar;

        Quaternion() noexcept;
        explicit Quaternion(TScalar value) noexcept;
        Quaternion(TScalar x, TScalar y, TScalar z, TScalar w) noexcept;

        Quaternion(const Quaternion& other) noexcept = default;
        Quaternion(Quaternion&& other) noexcept      = default;

        ~Quaternion() noexcept = default;

        Quaternion& operator=(const Quaternion& other) noexcept = default;
        Quaternion& operator=(Quaternion&& other) noexcept      = default;

        [[nodiscard]] explicit operator const TScalar*() const noexcept;
        [[nodiscard]] explicit operator TScalar*() noexcept;

        [[nodiscard]] TScalar  operator[](USize index) const noexcept;
        [[nodiscard]] TScalar& operator[](USize index) noexcept;

        [[nodiscard]] Quaternion operator+(const Quaternion& other) const noexcept;
        Quaternion&              operator+=(const Quaternion& other) noexcept;

        [[nodiscard]] Quaternion operator-(const Quaternion& other) const noexcept;
        Quaternion&              operator-=(const Quaternion& other) noexcept;

        [[nodiscard]] Quaternion        operator*(TScalar scalar) const noexcept;
        [[nodiscard]] Quaternion        operator*(const Quaternion& other) const noexcept;
        [[nodiscard]] Vector3D<Float32> operator*(const Vector3D<Float32>& vector) const noexcept
            requires Floatable<TScalar>;
        Quaternion& operator*=(TScalar scalar) noexcept;
        Quaternion& operator*=(const Quaternion& other) noexcept;

        [[nodiscard]] Quaternion operator/(TScalar scalar) const noexcept;
        Quaternion&              operator/=(TScalar scalar) noexcept;

        [[nodiscard]] bool                  operator==(const Quaternion& other) const noexcept;
        [[nodiscard]] bool                  operator!=(const Quaternion& other) const noexcept;
        [[nodiscard]] std::partial_ordering operator<=>(const Quaternion& other) const noexcept;

        [[nodiscard]] TScalar GetX() const noexcept;
        void                  SetX(TScalar component) noexcept;

        [[nodiscard]] TScalar GetY() const noexcept;
        void                  SetY(TScalar component) noexcept;

        [[nodiscard]] TScalar GetZ() const noexcept;
        void                  SetZ(TScalar component) noexcept;

        [[nodiscard]] TScalar GetW() const noexcept;
        void                  SetW(TScalar component) noexcept;

        void Set(TScalar xValue, TScalar yValue, TScalar zValue, TScalar wValue) noexcept;

        [[nodiscard]] Vector3D<Float32> GetEulerAngles() const noexcept
            requires Floatable<TScalar>;
        void SetEulerAngles(const Vector3D<Float32>& eulerDegrees) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] Float32 GetMagnitude() const noexcept
            requires Floatable<TScalar>;
        [[nodiscard]] Float32 GetSqrMagnitude() const noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] Quaternion GetNormalized() const noexcept
            requires Floatable<TScalar>;

        void SetFromToRotation(const Vector3D<Float32>& from, const Vector3D<Float32>& to) noexcept
            requires Floatable<TScalar>;
        void SetLookRotation(const Vector3D<Float32>& view) noexcept
            requires Floatable<TScalar>;
        void SetLookRotation(const Vector3D<Float32>& view, const Vector3D<Float32>& up) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] bool IsFinite() const noexcept;
        [[nodiscard]] bool IsNormalized(Float32 epsilon = Mathf::EPSILON) const noexcept
            requires Floatable<TScalar>;

        void Normalize() noexcept
            requires Floatable<TScalar>;

        void ToAngleAxis(Float32& angleDegrees, Vector3D<Float32>& axis) const noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static Quaternion Normalize(const Quaternion& rotation) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static Quaternion GetIdentity() noexcept;

        [[nodiscard]] static bool
                IsApproximately(const Quaternion& lhs, const Quaternion& rhs, Float32 epsilon = Mathf::EPSILON) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static Float32 Angle(const Quaternion& a, const Quaternion& b) noexcept
            requires Floatable<TScalar>;
        [[nodiscard]] static Quaternion AngleAxis(Float32 angleDegrees, Vector3D<Float32> axis) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static Quaternion Euler(Float32 xDegrees, Float32 yDegrees, Float32 zDegrees) noexcept
            requires Floatable<TScalar>;
        [[nodiscard]] static Quaternion Euler(const Vector3D<Float32>& eulerDegrees) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static Quaternion Inverse(const Quaternion& rotation) noexcept
            requires Floatable<TScalar>;
        [[nodiscard]] static Quaternion Conjugate(const Quaternion& rotation) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static Float32 Dot(const Quaternion& a, const Quaternion& b) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static Quaternion Lerp(const Quaternion& a, const Quaternion& b, Float32 t) noexcept
            requires Floatable<TScalar>;
        [[nodiscard]] static Quaternion LerpUnclamped(const Quaternion& a, const Quaternion& b, Float32 t) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static Quaternion Slerp(const Quaternion& a, const Quaternion& b, Float32 t) noexcept
            requires Floatable<TScalar>;
        [[nodiscard]] static Quaternion SlerpUnclamped(const Quaternion& a, const Quaternion& b, Float32 t) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static Quaternion FromToRotation(const Vector3D<Float32>& fromDirection,
                                                       const Vector3D<Float32>& toDirection) noexcept
            requires Floatable<TScalar>;
        [[nodiscard]] static Quaternion LookRotation(const Vector3D<Float32>& forward,
                                                     const Vector3D<Float32>& up) noexcept
            requires Floatable<TScalar>;
        [[nodiscard]] static Quaternion
                RotateTowards(const Quaternion& from, const Quaternion& to, Float32 maxDegreesDelta) noexcept
            requires Floatable<TScalar>;

    private:
        Array<TScalar, 4> elements = {TScalar{0}, TScalar{0}, TScalar{0}, static_cast<TScalar>(1)};
    };

    using QuaternionInt   = Quaternion<Int32>;
    using QuaternionFloat = Quaternion<Float32>;

#if !defined(Garage_EXPORTS)
    extern template class GARAGE_API Quaternion<Int32>;
    extern template class GARAGE_API Quaternion<Float32>;
#endif
} // namespace Garage

#endif // GARAGE_MATH_QUATERNION_HPP
