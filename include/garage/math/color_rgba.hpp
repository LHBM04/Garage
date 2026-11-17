#ifndef GARAGE_MATH_COLOR_RGBA_HPP
#define GARAGE_MATH_COLOR_RGBA_HPP

#include <array>

#include "garage/core/defines.hpp"
#include "garage/core/types.hpp"
#include "garage/math/concepts.hpp"
#include "garage/math/mathf.hpp"
#include "garage/math/vector3d.hpp"
#include "garage/math/vector4d.hpp"

namespace Garage
{
    template <Scalarable TScalar>
    class ColorRGB;

    template <Scalarable TScalar>
    class GARAGE_API ColorRGBA final
    {
    public:
        using ValueType = TScalar;

        ColorRGBA() noexcept;
        explicit ColorRGBA(TScalar value) noexcept;
        ColorRGBA(TScalar r,
                  TScalar g,
                  TScalar b,
                  TScalar a = static_cast<TScalar>(Integral<TScalar> ? 255 : 1)) noexcept;
        explicit ColorRGBA(const TScalar* values) noexcept;

        ColorRGBA(const ColorRGBA& other) noexcept = default;
        ColorRGBA(ColorRGBA&& other) noexcept      = default;

        explicit ColorRGBA(const ColorRGB<TScalar>& rgb,
                           TScalar                  a = static_cast<TScalar>(Integral<TScalar> ? 255 : 1)) noexcept;
        explicit ColorRGBA(const Vector3D<TScalar>& vector,
                           TScalar                  alpha = static_cast<TScalar>(Integral<TScalar> ? 255 : 1)) noexcept;
        explicit ColorRGBA(const Vector4D<TScalar>& vector) noexcept;

        ~ColorRGBA() noexcept = default;

        ColorRGBA& operator=(const ColorRGBA& other) noexcept = default;
        ColorRGBA& operator=(ColorRGBA&& other) noexcept      = default;
        ColorRGBA& operator=(const Vector4D<TScalar>& other) noexcept;

        [[nodiscard]] explicit operator Vector4D<TScalar>() const noexcept;

        [[nodiscard]] explicit operator const TScalar*() const noexcept;
        [[nodiscard]] explicit operator TScalar*() noexcept;

        [[nodiscard]] TScalar  operator[](USize index) const noexcept;
        [[nodiscard]] TScalar& operator[](USize index) noexcept;

        [[nodiscard]] ColorRGBA operator+(const ColorRGBA& other) const noexcept;
        ColorRGBA&              operator+=(const ColorRGBA& other) noexcept;

        [[nodiscard]] ColorRGBA operator-(const ColorRGBA& other) const noexcept;
        ColorRGBA&              operator-=(const ColorRGBA& other) noexcept;

        [[nodiscard]] ColorRGBA operator*(const ColorRGBA& other) const noexcept;
        [[nodiscard]] ColorRGBA operator*(TScalar scalar) const noexcept;
        ColorRGBA&              operator*=(const ColorRGBA& other) noexcept;
        ColorRGBA&              operator*=(TScalar scalar) noexcept;

        [[nodiscard]] ColorRGBA operator/(TScalar scalar) const noexcept;
        ColorRGBA&              operator/=(TScalar scalar) noexcept;

        [[nodiscard]] bool operator==(const ColorRGBA& other) const noexcept;
        [[nodiscard]] bool operator!=(const ColorRGBA& other) const noexcept;

        [[nodiscard]] TScalar GetR() const noexcept;
        void                  SetR(TScalar component) noexcept;

        [[nodiscard]] TScalar GetG() const noexcept;
        void                  SetG(TScalar component) noexcept;

        [[nodiscard]] TScalar GetB() const noexcept;
        void                  SetB(TScalar component) noexcept;

        [[nodiscard]] TScalar GetA() const noexcept;
        void                  SetA(TScalar component) noexcept;

        void Set(TScalar rValue, TScalar gValue, TScalar bValue, TScalar aValue) noexcept;

        [[nodiscard]] bool IsTransparent(Float32 epsilon = Mathf::EPSILON) const noexcept;
        [[nodiscard]] bool IsOpaque(Float32 epsilon = Mathf::EPSILON) const noexcept;

        [[nodiscard]] Vector4D<TScalar> ToVector4D() const noexcept;
        [[nodiscard]] ColorRGB<TScalar> ToColorRGB() const noexcept;

        [[nodiscard]] static ColorRGBA GetBlack() noexcept;
        [[nodiscard]] static ColorRGBA GetWhite() noexcept;

        [[nodiscard]] static ColorRGBA GetRed() noexcept;
        [[nodiscard]] static ColorRGBA GetGreen() noexcept;
        [[nodiscard]] static ColorRGBA GetBlue() noexcept;

        [[nodiscard]] static ColorRGBA GetYellow() noexcept;
        [[nodiscard]] static ColorRGBA GetCyan() noexcept;
        [[nodiscard]] static ColorRGBA GetMagenta() noexcept;

        [[nodiscard]] static ColorRGBA GetClear() noexcept;

        [[nodiscard]] static bool
                IsApproximately(const ColorRGBA& lhs, const ColorRGBA& rhs, Float32 epsilon = Mathf::EPSILON) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static ColorRGBA Lerp(const ColorRGBA& start, const ColorRGBA& end, Float32 t) noexcept
            requires Floatable<TScalar>;

    private:
        std::array<TScalar, 4> elements = {};
    };

    using ColorRGBAInt   = ColorRGBA<Int32>;
    using ColorRGBAFloat = ColorRGBA<Float32>;

    extern template class ColorRGBA<Int32>;
    extern template class ColorRGBA<Float32>;
} // namespace Garage

#endif // GARAGE_MATH_COLOR_RGBA_HPP
