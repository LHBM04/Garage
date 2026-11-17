#ifndef GARAGE_MATH_MATRIX4X4_HPP
#define GARAGE_MATH_MATRIX4X4_HPP

#include <array>
#include <compare>

#include "../core/defines.hpp"
#include "../core/types.hpp"

#include "mathf.hpp"
#include "vector3d.hpp"
#include "vector4d.hpp"

namespace Garage
{
    template <Scalarable TScalar>
    class Quaternion;

    template <Scalarable TScalar>
    class GARAGE_API Matrix4x4 final
    {
    public:
        using ValueType = TScalar;

        Matrix4x4() noexcept;
        explicit Matrix4x4(TScalar value) noexcept;
        explicit Matrix4x4(const TScalar values[16]) noexcept;
        Matrix4x4(TScalar m00,
                  TScalar m01,
                  TScalar m02,
                  TScalar m03,
                  TScalar m10,
                  TScalar m11,
                  TScalar m12,
                  TScalar m13,
                  TScalar m20,
                  TScalar m21,
                  TScalar m22,
                  TScalar m23,
                  TScalar m30,
                  TScalar m31,
                  TScalar m32,
                  TScalar m33) noexcept;

        Matrix4x4(const Matrix4x4& other) noexcept = default;
        Matrix4x4(Matrix4x4&& other) noexcept      = default;

        ~Matrix4x4() noexcept = default;

        Matrix4x4& operator=(const Matrix4x4& other) noexcept = default;
        Matrix4x4& operator=(Matrix4x4&& other) noexcept      = default;

        [[nodiscard]] explicit operator const TScalar*() const noexcept;
        [[nodiscard]] explicit operator TScalar*() noexcept;

        [[nodiscard]] TScalar  operator[](USize index) const noexcept;
        [[nodiscard]] TScalar& operator[](USize index) noexcept;

        [[nodiscard]] Matrix4x4 operator*(const Matrix4x4& other) const noexcept;
        Matrix4x4&              operator*=(const Matrix4x4& other) noexcept;

        [[nodiscard]] bool                  operator==(const Matrix4x4& other) const noexcept;
        [[nodiscard]] bool                  operator!=(const Matrix4x4& other) const noexcept;
        [[nodiscard]] std::partial_ordering operator<=>(const Matrix4x4& other) const noexcept;

        [[nodiscard]] Vector4D<TScalar> GetRow(USize index) const noexcept;
        void                            SetRow(USize index, const Vector4D<TScalar>& row) noexcept;

        [[nodiscard]] Vector4D<TScalar> GetColumn(USize index) const noexcept;
        void                            SetColumn(USize index, const Vector4D<TScalar>& column) noexcept;

        [[nodiscard]] Float32 GetDeterminant() const noexcept
            requires Floatable<TScalar>;
        [[nodiscard]] Matrix4x4 GetTranspose() const noexcept;
        [[nodiscard]] Matrix4x4 GetInverse() const noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] Vector3D<TScalar>   GetWorldPosition() const noexcept;
        [[nodiscard]] Quaternion<TScalar> GetRotation() const noexcept
            requires Floatable<TScalar>;
        [[nodiscard]] Vector3D<Float32> GetScale() const noexcept
            requires Floatable<TScalar>;
        [[nodiscard]] Vector3D<Float32> GetLossyScale() const noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] Vector3D<Float32> GetForward() const noexcept
            requires Floatable<TScalar>;
        [[nodiscard]] Vector3D<Float32> GetUp() const noexcept
            requires Floatable<TScalar>;
        [[nodiscard]] Vector3D<Float32> GetRight() const noexcept
            requires Floatable<TScalar>;

        void SetTRS(const Vector3D<Float32>&   position,
                    const Quaternion<TScalar>& rotation,
                    const Vector3D<Float32>&   scale) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] Vector3D<TScalar> MultiplyPoint3x4(const Vector3D<TScalar>& point) const noexcept;
        [[nodiscard]] Vector3D<Float32> MultiplyPoint(const Vector3D<Float32>& point) const noexcept
            requires Floatable<TScalar>;
        [[nodiscard]] Vector3D<TScalar> MultiplyVector(const Vector3D<TScalar>& vector) const noexcept;

        [[nodiscard]] bool TryDecompose(Vector3D<Float32>&   position,
                                        Quaternion<TScalar>& rotation,
                                        Vector3D<Float32>&   scale) const noexcept
            requires Floatable<TScalar>;

        bool TryGetInverse(Matrix4x4& result) const noexcept
            requires Floatable<TScalar>;
        [[nodiscard]] bool CanInverse() const noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] bool IsValidTRS() const noexcept
            requires Floatable<TScalar>;
        [[nodiscard]] bool IsIdentity(Float32 epsilon = Mathf::EPSILON) const noexcept;

        [[nodiscard]] static Matrix4x4 GetIdentity() noexcept;
        [[nodiscard]] static Matrix4x4 GetZero() noexcept;

        [[nodiscard]] static bool
                IsApproximately(const Matrix4x4& lhs, const Matrix4x4& rhs, Float32 epsilon = Mathf::EPSILON) noexcept;

        [[nodiscard]] static Matrix4x4 Translate(const Vector3D<TScalar>& translation) noexcept;
        [[nodiscard]] static Matrix4x4 Rotate(const Quaternion<TScalar>& rotation) noexcept
            requires Floatable<TScalar>;
        [[nodiscard]] static Matrix4x4 Scale(const Vector3D<TScalar>& scale) noexcept;
        [[nodiscard]] static Matrix4x4 TRS(const Vector3D<Float32>&   translation,
                                           const Quaternion<TScalar>& rotation,
                                           const Vector3D<Float32>&   scale) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static Matrix4x4
                LookAt(const Vector3D<Float32>& from, const Vector3D<Float32>& to, const Vector3D<Float32>& up) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static Matrix4x4
                Perspective(Float32 fovYDegrees, Float32 aspect, Float32 nearZ, Float32 farZ) noexcept
            requires Floatable<TScalar>;
        [[nodiscard]] static Matrix4x4
                Frustum(Float32 left, Float32 right, Float32 bottom, Float32 top, Float32 nearZ, Float32 farZ) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static Matrix4x4
                Ortho(Float32 left, Float32 right, Float32 bottom, Float32 top, Float32 nearZ, Float32 farZ) noexcept
            requires Floatable<TScalar>;
        [[nodiscard]] static Matrix4x4 Ortho(Float32 width, Float32 height, Float32 nearZ, Float32 farZ) noexcept
            requires Floatable<TScalar>;

        static bool TryInverse3DAffine(const Matrix4x4& input, Matrix4x4& result) noexcept
            requires Floatable<TScalar>;

    private:
        std::array<TScalar, 16> elements = {};
    };

    using Matrix4x4Int   = Matrix4x4<Int32>;
    using Matrix4x4Float = Matrix4x4<Float32>;

    extern template class Matrix4x4<Int32>;
    extern template class Matrix4x4<Float32>;
} // namespace Garage

#endif // GARAGE_MATH_MATRIX4X4_HPP
