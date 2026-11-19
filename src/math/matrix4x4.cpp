#include "garage/math/matrix4x4.hpp"

#include <array>
#include <cmath>
#include <compare>

#include "garage/core/common.hpp"

#include "garage/math/mathf.hpp"
#include "garage/math/quaternion.hpp"
#include "garage/math/vector3d.hpp"
#include "garage/math/vector4d.hpp"

namespace Garage
{
    namespace
    {
        [[nodiscard]] bool ComputeInverse4x4(const std::array<Float32, 16>& m,
                                             std::array<Float32, 16>&       invOut,
                                             Float32&                       detOut) noexcept
        {
            std::array<Float32, 16> inv = {};

            inv[0] = m[5] * m[10] * m[15] - m[5] * m[11] * m[14] - m[9] * m[6] * m[15] + m[9] * m[7] * m[14] +
                    m[13] * m[6] * m[11] - m[13] * m[7] * m[10];
            inv[4] = -m[4] * m[10] * m[15] + m[4] * m[11] * m[14] + m[8] * m[6] * m[15] - m[8] * m[7] * m[14] -
                    m[12] * m[6] * m[11] + m[12] * m[7] * m[10];
            inv[8] = m[4] * m[9] * m[15] - m[4] * m[11] * m[13] - m[8] * m[5] * m[15] + m[8] * m[7] * m[13] +
                    m[12] * m[5] * m[11] - m[12] * m[7] * m[9];
            inv[12] = -m[4] * m[9] * m[14] + m[4] * m[10] * m[13] + m[8] * m[5] * m[14] - m[8] * m[6] * m[13] -
                    m[12] * m[5] * m[10] + m[12] * m[6] * m[9];

            inv[1] = -m[1] * m[10] * m[15] + m[1] * m[11] * m[14] + m[9] * m[2] * m[15] - m[9] * m[3] * m[14] -
                    m[13] * m[2] * m[11] + m[13] * m[3] * m[10];
            inv[5] = m[0] * m[10] * m[15] - m[0] * m[11] * m[14] - m[8] * m[2] * m[15] + m[8] * m[3] * m[14] +
                    m[12] * m[2] * m[11] - m[12] * m[3] * m[10];
            inv[9] = -m[0] * m[9] * m[15] + m[0] * m[11] * m[13] + m[8] * m[1] * m[15] - m[8] * m[3] * m[13] -
                    m[12] * m[1] * m[11] + m[12] * m[3] * m[9];
            inv[13] = m[0] * m[9] * m[14] - m[0] * m[10] * m[13] - m[8] * m[1] * m[14] + m[8] * m[2] * m[13] +
                    m[12] * m[1] * m[10] - m[12] * m[2] * m[9];

            inv[2] = m[1] * m[6] * m[15] - m[1] * m[7] * m[14] - m[5] * m[2] * m[15] + m[5] * m[3] * m[14] +
                    m[13] * m[2] * m[7] - m[13] * m[3] * m[6];
            inv[6] = -m[0] * m[6] * m[15] + m[0] * m[7] * m[14] + m[4] * m[2] * m[15] - m[4] * m[3] * m[14] -
                    m[12] * m[2] * m[7] + m[12] * m[3] * m[6];
            inv[10] = m[0] * m[5] * m[15] - m[0] * m[7] * m[13] - m[4] * m[1] * m[15] + m[4] * m[3] * m[13] +
                    m[12] * m[1] * m[7] - m[12] * m[3] * m[5];
            inv[14] = -m[0] * m[5] * m[14] + m[0] * m[6] * m[13] + m[4] * m[1] * m[14] - m[4] * m[2] * m[13] -
                    m[12] * m[1] * m[6] + m[12] * m[2] * m[5];

            inv[3] = -m[1] * m[6] * m[11] + m[1] * m[7] * m[10] + m[5] * m[2] * m[11] - m[5] * m[3] * m[10] -
                    m[9] * m[2] * m[7] + m[9] * m[3] * m[6];
            inv[7] = m[0] * m[6] * m[11] - m[0] * m[7] * m[10] - m[4] * m[2] * m[11] + m[4] * m[3] * m[10] +
                    m[8] * m[2] * m[7] - m[8] * m[3] * m[6];
            inv[11] = -m[0] * m[5] * m[11] + m[0] * m[7] * m[9] + m[4] * m[1] * m[11] - m[4] * m[3] * m[9] -
                    m[8] * m[1] * m[7] + m[8] * m[3] * m[5];
            inv[15] = m[0] * m[5] * m[10] - m[0] * m[6] * m[9] - m[4] * m[1] * m[10] + m[4] * m[2] * m[9] +
                    m[8] * m[1] * m[6] - m[8] * m[2] * m[5];

            detOut = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];
            if (!std::isfinite(detOut) || std::abs(detOut) <= Mathf::EPSILON)
            {
                return false;
            }

            const Float32 INV_DET = 1.0f / detOut;
            for (USize i = 0; i < 16; ++i)
            {
                invOut[i] = inv[i] * INV_DET;
                if (!std::isfinite(invOut[i]))
                {
                    return false;
                }
            }

            return true;
        }
    } // namespace

    template <Scalarable TScalar>
    Matrix4x4<TScalar>::Matrix4x4() noexcept
    {
        for (USize diagonal = 0; diagonal < 4; ++diagonal)
        {
            elements[diagonal * 5] = static_cast<TScalar>(1);
        }
    }

    template <Scalarable TScalar>
    Matrix4x4<TScalar>::Matrix4x4(TScalar value) noexcept
    {
        for (USize diagonal = 0; diagonal < 4; ++diagonal)
        {
            elements[diagonal * 5] = value;
        }
    }

    template <Scalarable TScalar>
    Matrix4x4<TScalar>::Matrix4x4(const TScalar values[16]) noexcept
    {
        ASSERT(values != nullptr);
        for (USize index = 0; index < 16; ++index)
        {
            elements[index] = values[index];
        }
    }

    template <Scalarable TScalar>
    Matrix4x4<TScalar>::Matrix4x4(TScalar m00,
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
                                  TScalar m33) noexcept
        : elements{m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23, m30, m31, m32, m33}
    {
    }

    template <Scalarable TScalar>
    Matrix4x4<TScalar>::operator const TScalar*() const noexcept
    {
        return elements.data();
    }

    template <Scalarable TScalar>
    Matrix4x4<TScalar>::operator TScalar*() noexcept
    {
        return elements.data();
    }

    template <Scalarable TScalar>
    TScalar Matrix4x4<TScalar>::operator[](USize index) const noexcept
    {
        ASSERT(index < 16);
        return elements[index];
    }

    template <Scalarable TScalar>
    TScalar& Matrix4x4<TScalar>::operator[](USize index) noexcept
    {
        ASSERT(index < 16);
        return elements[index];
    }

    template <Scalarable TScalar>
    Matrix4x4<TScalar> Matrix4x4<TScalar>::operator*(const Matrix4x4& other) const noexcept
    {
        Matrix4x4 result(TScalar{0});
        for (USize row = 0; row < 4; ++row)
        {
            for (USize col = 0; col < 4; ++col)
            {
                TScalar sum = TScalar{0};
                for (USize k = 0; k < 4; ++k)
                {
                    sum += elements[row * 4 + k] * other.elements[k * 4 + col];
                }
                result.elements[row * 4 + col] = sum;
            }
        }
        return result;
    }

    template <Scalarable TScalar>
    Matrix4x4<TScalar>& Matrix4x4<TScalar>::operator*=(const Matrix4x4& other) noexcept
    {
        *this = *this * other;
        return *this;
    }

    template <Scalarable TScalar>
    bool Matrix4x4<TScalar>::operator==(const Matrix4x4& other) const noexcept
    {
        return (*this <=> other) == std::partial_ordering::equivalent;
    }

    template <Scalarable TScalar>
    bool Matrix4x4<TScalar>::operator!=(const Matrix4x4& other) const noexcept
    {
        return !(*this == other);
    }

    template <Scalarable TScalar>
    std::partial_ordering Matrix4x4<TScalar>::operator<=>(const Matrix4x4& other) const noexcept
    {
        for (USize index = 0; index < 16; ++index)
        {
            if (const auto ORDER = elements[index] <=> other.elements[index]; ORDER != 0)
            {
                return ORDER;
            }
        }
        return std::partial_ordering::equivalent;
    }

    template <Scalarable TScalar>
    Vector4D<TScalar> Matrix4x4<TScalar>::GetRow(USize index) const noexcept
    {
        ASSERT(index < 4);
        const USize BASE = index * 4;
        return Vector4D<TScalar>(elements[BASE], elements[BASE + 1], elements[BASE + 2], elements[BASE + 3]);
    }

    template <Scalarable TScalar>
    void Matrix4x4<TScalar>::SetRow(USize index, const Vector4D<TScalar>& row) noexcept
    {
        ASSERT(index < 4);
        for (USize col = 0; col < 4; ++col)
        {
            elements[index * 4 + col] = row[col];
        }
    }

    template <Scalarable TScalar>
    Vector4D<TScalar> Matrix4x4<TScalar>::GetColumn(USize index) const noexcept
    {
        ASSERT(index < 4);
        return Vector4D<TScalar>(elements[index], elements[index + 4], elements[index + 8], elements[index + 12]);
    }

    template <Scalarable TScalar>
    void Matrix4x4<TScalar>::SetColumn(USize index, const Vector4D<TScalar>& column) noexcept
    {
        ASSERT(index < 4);
        for (USize row = 0; row < 4; ++row)
        {
            elements[row * 4 + index] = column[row];
        }
    }

    template <Scalarable TScalar>
    Float32 Matrix4x4<TScalar>::GetDeterminant() const noexcept
        requires Floatable<TScalar>
    {
        std::array<Float32, 16> ignored = {};
        Float32                 det     = 0.0f;
        (void)ComputeInverse4x4(elements, ignored, det);
        return det;
    }

    template <Scalarable TScalar>
    Matrix4x4<TScalar> Matrix4x4<TScalar>::GetTranspose() const noexcept
    {
        Matrix4x4 result(TScalar{0});
        for (USize row = 0; row < 4; ++row)
        {
            for (USize col = 0; col < 4; ++col)
            {
                result.elements[row * 4 + col] = elements[col * 4 + row];
            }
        }
        return result;
    }

    template <Scalarable TScalar>
    Matrix4x4<TScalar> Matrix4x4<TScalar>::GetInverse() const noexcept
        requires Floatable<TScalar>
    {
        Matrix4x4  result{};
        const bool OK = TryGetInverse(result);
        ASSERT(OK);
        return result;
    }

    template <Scalarable TScalar>
    Vector3D<TScalar> Matrix4x4<TScalar>::GetWorldPosition() const noexcept
    {
        return Vector3D<TScalar>(elements[12], elements[13], elements[14]);
    }

    template <Scalarable TScalar>
    Quaternion<TScalar> Matrix4x4<TScalar>::GetRotation() const noexcept
        requires Floatable<TScalar>
    {
        Vector3D<Float32>   pos{};
        Quaternion<TScalar> rot{};
        Vector3D<Float32>   scl{};
        if (TryDecompose(pos, rot, scl))
        {
            return rot;
        }
        return Quaternion<TScalar>::GetIdentity();
    }

    template <Scalarable TScalar>
    Vector3D<Float32> Matrix4x4<TScalar>::GetScale() const noexcept
        requires Floatable<TScalar>
    {
        const Vector3D<Float32> X_AXIS(elements[0], elements[1], elements[2]);
        const Vector3D<Float32> Y_AXIS(elements[4], elements[5], elements[6]);
        const Vector3D<Float32> Z_AXIS(elements[8], elements[9], elements[10]);
        return Vector3D<Float32>(X_AXIS.GetMagnitude(), Y_AXIS.GetMagnitude(), Z_AXIS.GetMagnitude());
    }

    template <Scalarable TScalar>
    Vector3D<Float32> Matrix4x4<TScalar>::GetLossyScale() const noexcept
        requires Floatable<TScalar>
    {
        return GetScale();
    }

    template <Scalarable TScalar>
    Vector3D<Float32> Matrix4x4<TScalar>::GetForward() const noexcept
        requires Floatable<TScalar>
    {
        return Vector3D<Float32>(elements[8], elements[9], elements[10]).GetNormalized();
    }

    template <Scalarable TScalar>
    Vector3D<Float32> Matrix4x4<TScalar>::GetUp() const noexcept
        requires Floatable<TScalar>
    {
        return Vector3D<Float32>(elements[4], elements[5], elements[6]).GetNormalized();
    }

    template <Scalarable TScalar>
    Vector3D<Float32> Matrix4x4<TScalar>::GetRight() const noexcept
        requires Floatable<TScalar>
    {
        return Vector3D<Float32>(elements[0], elements[1], elements[2]).GetNormalized();
    }

    template <Scalarable TScalar>
    void Matrix4x4<TScalar>::SetTRS(const Vector3D<Float32>&   position,
                                    const Quaternion<TScalar>& rotation,
                                    const Vector3D<Float32>&   scale) noexcept
        requires Floatable<TScalar>
    {
        *this = TRS(position, rotation, scale);
    }

    template <Scalarable TScalar>
    Vector3D<TScalar> Matrix4x4<TScalar>::MultiplyPoint3x4(const Vector3D<TScalar>& point) const noexcept
    {
        return Vector3D<TScalar>(
                point.GetX() * elements[0] + point.GetY() * elements[4] + point.GetZ() * elements[8] + elements[12],
                point.GetX() * elements[1] + point.GetY() * elements[5] + point.GetZ() * elements[9] + elements[13],
                point.GetX() * elements[2] + point.GetY() * elements[6] + point.GetZ() * elements[10] + elements[14]);
    }

    template <Scalarable TScalar>
    Vector3D<Float32> Matrix4x4<TScalar>::MultiplyPoint(const Vector3D<Float32>& point) const noexcept
        requires Floatable<TScalar>
    {
        const Float32 X =
                point.GetX() * elements[0] + point.GetY() * elements[4] + point.GetZ() * elements[8] + elements[12];
        const Float32 Y =
                point.GetX() * elements[1] + point.GetY() * elements[5] + point.GetZ() * elements[9] + elements[13];
        const Float32 Z =
                point.GetX() * elements[2] + point.GetY() * elements[6] + point.GetZ() * elements[10] + elements[14];
        const Float32 W =
                point.GetX() * elements[3] + point.GetY() * elements[7] + point.GetZ() * elements[11] + elements[15];

        if (std::abs(W) <= Mathf::EPSILON)
        {
            return Vector3D<Float32>(X, Y, Z);
        }
        return Vector3D<Float32>(X / W, Y / W, Z / W);
    }

    template <Scalarable TScalar>
    Vector3D<TScalar> Matrix4x4<TScalar>::MultiplyVector(const Vector3D<TScalar>& vector) const noexcept
    {
        return Vector3D<TScalar>(
                vector.GetX() * elements[0] + vector.GetY() * elements[4] + vector.GetZ() * elements[8],
                vector.GetX() * elements[1] + vector.GetY() * elements[5] + vector.GetZ() * elements[9],
                vector.GetX() * elements[2] + vector.GetY() * elements[6] + vector.GetZ() * elements[10]);
    }

    template <Scalarable TScalar>
    bool Matrix4x4<TScalar>::TryGetInverse(Matrix4x4& result) const noexcept
        requires Floatable<TScalar>
    {
        Float32 det = 0.0f;
        return ComputeInverse4x4(elements, result.elements, det);
    }

    template <Scalarable TScalar>
    bool Matrix4x4<TScalar>::CanInverse() const noexcept
        requires Floatable<TScalar>
    {
        return std::abs(GetDeterminant()) > Mathf::EPSILON;
    }

    template <Scalarable TScalar>
    bool Matrix4x4<TScalar>::TryDecompose(Vector3D<Float32>&   position,
                                          Quaternion<TScalar>& rotation,
                                          Vector3D<Float32>&   scale) const noexcept
        requires Floatable<TScalar>
    {
        position = GetWorldPosition();
        scale    = GetScale();

        if (scale.GetX() <= Mathf::EPSILON || scale.GetY() <= Mathf::EPSILON || scale.GetZ() <= Mathf::EPSILON)
        {
            return false;
        }

        const Vector3D<Float32> R0 = Vector3D<Float32>(elements[0], elements[1], elements[2]) / scale.GetX();
        const Vector3D<Float32> R1 = Vector3D<Float32>(elements[4], elements[5], elements[6]) / scale.GetY();
        const Vector3D<Float32> R2 = Vector3D<Float32>(elements[8], elements[9], elements[10]) / scale.GetZ();

        rotation = Quaternion<TScalar>::LookRotation(R2, R1);
        (void)R0;
        return true;
    }

    template <Scalarable TScalar>
    bool Matrix4x4<TScalar>::IsValidTRS() const noexcept
        requires Floatable<TScalar>
    {
        if (std::abs(elements[3]) > Mathf::EPSILON || std::abs(elements[7]) > Mathf::EPSILON ||
            std::abs(elements[11]) > Mathf::EPSILON || std::abs(elements[15] - 1.0f) > Mathf::EPSILON)
        {
            return false;
        }

        for (USize index = 0; index < 16; ++index)
        {
            if (!std::isfinite(elements[index]))
            {
                return false;
            }
        }

        Vector3D<Float32>   pos{};
        Quaternion<TScalar> rot{};
        Vector3D<Float32>   scl{};
        return TryDecompose(pos, rot, scl);
    }

    template <Scalarable TScalar>
    bool Matrix4x4<TScalar>::IsIdentity(Float32 epsilon) const noexcept
    {
        return IsApproximately(*this, GetIdentity(), epsilon);
    }

    template <Scalarable TScalar>
    Matrix4x4<TScalar> Matrix4x4<TScalar>::GetIdentity() noexcept
    {
        return Matrix4x4();
    }

    template <Scalarable TScalar>
    Matrix4x4<TScalar> Matrix4x4<TScalar>::GetZero() noexcept
    {
        return Matrix4x4(TScalar{0});
    }

    template <Scalarable TScalar>
    bool Matrix4x4<TScalar>::IsApproximately(const Matrix4x4& lhs, const Matrix4x4& rhs, Float32 epsilon) noexcept
    {
        ASSERT(epsilon >= 0.0f);
        for (USize index = 0; index < 16; ++index)
        {
            if (!(std::abs(static_cast<Float64>(lhs.elements[index]) - static_cast<Float64>(rhs.elements[index])) <=
                  epsilon))
            {
                return false;
            }
        }
        return true;
    }

    template <Scalarable TScalar>
    Matrix4x4<TScalar> Matrix4x4<TScalar>::Translate(const Vector3D<TScalar>& translation) noexcept
    {
        Matrix4x4 result{};
        result.elements[12] = translation.GetX();
        result.elements[13] = translation.GetY();
        result.elements[14] = translation.GetZ();
        return result;
    }

    template <Scalarable TScalar>
    Matrix4x4<TScalar> Matrix4x4<TScalar>::Rotate(const Quaternion<TScalar>& rotation) noexcept
        requires Floatable<TScalar>
    {
        const Quaternion<TScalar> Q  = Quaternion<TScalar>::Normalize(rotation);
        const Float32             XX = Q.GetX() * Q.GetX();
        const Float32             YY = Q.GetY() * Q.GetY();
        const Float32             ZZ = Q.GetZ() * Q.GetZ();
        const Float32             XY = Q.GetX() * Q.GetY();
        const Float32             XZ = Q.GetX() * Q.GetZ();
        const Float32             YZ = Q.GetY() * Q.GetZ();
        const Float32             WX = Q.GetW() * Q.GetX();
        const Float32             WY = Q.GetW() * Q.GetY();
        const Float32             WZ = Q.GetW() * Q.GetZ();

        return Matrix4x4(1.0f - 2.0f * (YY + ZZ),
                         2.0f * (XY + WZ),
                         2.0f * (XZ - WY),
                         0.0f,
                         2.0f * (XY - WZ),
                         1.0f - 2.0f * (XX + ZZ),
                         2.0f * (YZ + WX),
                         0.0f,
                         2.0f * (XZ + WY),
                         2.0f * (YZ - WX),
                         1.0f - 2.0f * (XX + YY),
                         0.0f,
                         0.0f,
                         0.0f,
                         0.0f,
                         1.0f);
    }

    template <Scalarable TScalar>
    Matrix4x4<TScalar> Matrix4x4<TScalar>::Scale(const Vector3D<TScalar>& scale) noexcept
    {
        Matrix4x4 result{};
        result.elements[0]  = scale.GetX();
        result.elements[5]  = scale.GetY();
        result.elements[10] = scale.GetZ();
        return result;
    }

    template <Scalarable TScalar>
    Matrix4x4<TScalar> Matrix4x4<TScalar>::TRS(const Vector3D<Float32>&   translation,
                                               const Quaternion<TScalar>& rotation,
                                               const Vector3D<Float32>&   scale) noexcept
        requires Floatable<TScalar>
    {
        return Scale(scale) * Rotate(rotation) * Translate(translation);
    }

    template <Scalarable TScalar>
    Matrix4x4<TScalar> Matrix4x4<TScalar>::LookAt(const Vector3D<Float32>& from,
                                                  const Vector3D<Float32>& to,
                                                  const Vector3D<Float32>& up) noexcept
        requires Floatable<TScalar>
    {
        const Vector3D<Float32> Z_AXIS = Vector3D<Float32>::Normalize(to - from);
        const Vector3D<Float32> X_AXIS = Vector3D<Float32>::Normalize(Vector3D<Float32>::Cross(up, Z_AXIS));
        const Vector3D<Float32> Y_AXIS = Vector3D<Float32>::Cross(Z_AXIS, X_AXIS);

        return Matrix4x4(X_AXIS.GetX(),
                         Y_AXIS.GetX(),
                         Z_AXIS.GetX(),
                         0.0f,
                         X_AXIS.GetY(),
                         Y_AXIS.GetY(),
                         Z_AXIS.GetY(),
                         0.0f,
                         X_AXIS.GetZ(),
                         Y_AXIS.GetZ(),
                         Z_AXIS.GetZ(),
                         0.0f,
                         -Vector3D<Float32>::Dot(X_AXIS, from),
                         -Vector3D<Float32>::Dot(Y_AXIS, from),
                         -Vector3D<Float32>::Dot(Z_AXIS, from),
                         1.0f);
    }

    template <Scalarable TScalar>
    Matrix4x4<TScalar>
            Matrix4x4<TScalar>::Perspective(Float32 fovYDegrees, Float32 aspect, Float32 nearZ, Float32 farZ) noexcept
        requires Floatable<TScalar>
    {
        const Float32 HALF_FOV = (fovYDegrees * Mathf::DEG_TO_RAD) * 0.5f;
        const Float32 HEIGHT   = 1.0f / std::tan(HALF_FOV);
        const Float32 WIDTH    = HEIGHT / aspect;
        const Float32 RANGE    = farZ / (farZ - nearZ);

        return Matrix4x4(WIDTH,
                         0.0f,
                         0.0f,
                         0.0f,
                         0.0f,
                         HEIGHT,
                         0.0f,
                         0.0f,
                         0.0f,
                         0.0f,
                         RANGE,
                         1.0f,
                         0.0f,
                         0.0f,
                         -RANGE * nearZ,
                         0.0f);
    }

    template <Scalarable TScalar>
    Matrix4x4<TScalar> Matrix4x4<TScalar>::Frustum(
            Float32 left, Float32 right, Float32 bottom, Float32 top, Float32 nearZ, Float32 farZ) noexcept
        requires Floatable<TScalar>
    {
        const Float32 RCP_WIDTH  = 1.0f / (right - left);
        const Float32 RCP_HEIGHT = 1.0f / (top - bottom);
        const Float32 RANGE      = farZ / (farZ - nearZ);

        return Matrix4x4(2.0f * nearZ * RCP_WIDTH,
                         0.0f,
                         0.0f,
                         0.0f,
                         0.0f,
                         2.0f * nearZ * RCP_HEIGHT,
                         0.0f,
                         0.0f,
                         -(left + right) * RCP_WIDTH,
                         -(top + bottom) * RCP_HEIGHT,
                         RANGE,
                         1.0f,
                         0.0f,
                         0.0f,
                         -RANGE * nearZ,
                         0.0f);
    }

    template <Scalarable TScalar>
    Matrix4x4<TScalar> Matrix4x4<TScalar>::Ortho(
            Float32 left, Float32 right, Float32 bottom, Float32 top, Float32 nearZ, Float32 farZ) noexcept
        requires Floatable<TScalar>
    {
        const Float32 RCP_WIDTH  = 1.0f / (right - left);
        const Float32 RCP_HEIGHT = 1.0f / (top - bottom);
        const Float32 RANGE      = 1.0f / (farZ - nearZ);

        return Matrix4x4(2.0f * RCP_WIDTH,
                         0.0f,
                         0.0f,
                         0.0f,
                         0.0f,
                         2.0f * RCP_HEIGHT,
                         0.0f,
                         0.0f,
                         0.0f,
                         0.0f,
                         RANGE,
                         0.0f,
                         -(left + right) * RCP_WIDTH,
                         -(top + bottom) * RCP_HEIGHT,
                         -RANGE * nearZ,
                         1.0f);
    }

    template <Scalarable TScalar>
    Matrix4x4<TScalar> Matrix4x4<TScalar>::Ortho(Float32 width, Float32 height, Float32 nearZ, Float32 farZ) noexcept
        requires Floatable<TScalar>
    {
        const Float32 RANGE = 1.0f / (farZ - nearZ);

        return Matrix4x4(2.0f / width,
                         0.0f,
                         0.0f,
                         0.0f,
                         0.0f,
                         2.0f / height,
                         0.0f,
                         0.0f,
                         0.0f,
                         0.0f,
                         RANGE,
                         0.0f,
                         0.0f,
                         0.0f,
                         -RANGE * nearZ,
                         1.0f);
    }

    template <Scalarable TScalar>
    bool Matrix4x4<TScalar>::TryInverse3DAffine(const Matrix4x4& input, Matrix4x4& result) noexcept
        requires Floatable<TScalar>
    {
        if (input.elements[3] != 0.0f || input.elements[7] != 0.0f || input.elements[11] != 0.0f ||
            input.elements[15] != 1.0f)
        {
            return false;
        }
        return input.TryGetInverse(result);
    }

    template class GARAGE_API Matrix4x4<Int32>;
    template class GARAGE_API Matrix4x4<Float32>;
} // namespace Garage
