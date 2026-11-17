#ifndef GARAGE_MATH_COLOR_RGB_HPP
#define GARAGE_MATH_COLOR_RGB_HPP

#include <array>

#include "../core/defines.hpp"
#include "../core/types.hpp"

#include "mathf.hpp"
#include "vector3d.hpp"
#include "vector4d.hpp"

namespace Garage
{
    template <Scalarable TScalar>
    class ColorRGBA;

    template <Scalarable TScalar>
    class ColorRGB final
    {
    public:
        using ValueType = TScalar;

        ColorRGB() noexcept;
        explicit ColorRGB(TScalar value) noexcept;
        ColorRGB(TScalar r, TScalar g, TScalar b) noexcept;

        ColorRGB(const ColorRGB& color) noexcept = default;
        ColorRGB(ColorRGB&& color) noexcept      = default;

        explicit ColorRGB(const Vector3D<TScalar>& vector) noexcept;
        explicit ColorRGB(const Vector4D<TScalar>& vector) noexcept;
        explicit ColorRGB(const ColorRGBA<TScalar>& color) noexcept;

        ~ColorRGB() noexcept = default;

        ColorRGB& operator=(const ColorRGB& other) noexcept = default;
        ColorRGB& operator=(ColorRGB&& other) noexcept      = default;

        [[nodiscard]] explicit operator const TScalar*() const noexcept;
        [[nodiscard]] explicit operator TScalar*() noexcept;

        [[nodiscard]] TScalar  operator[](USize index) const noexcept;
        [[nodiscard]] TScalar& operator[](USize index) noexcept;

        [[nodiscard]] bool operator==(const ColorRGB& other) const noexcept;
        [[nodiscard]] bool operator!=(const ColorRGB& other) const noexcept;

        [[nodiscard]] TScalar GetR() const noexcept;
        void                  SetR(TScalar component) noexcept;

        [[nodiscard]] TScalar GetG() const noexcept;
        void                  SetG(TScalar component) noexcept;

        [[nodiscard]] TScalar GetB() const noexcept;
        void                  SetB(TScalar component) noexcept;

        void Set(TScalar rValue, TScalar gValue, TScalar bValue) noexcept;

        [[nodiscard]] ColorRGB GetGamma() const noexcept
            requires Floatable<TScalar>;
        [[nodiscard]] ColorRGB GetLinear() const noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] Float32 GetGrayscale() const noexcept
            requires Floatable<TScalar>;
        [[nodiscard]] TScalar GetMaxColorComponent() const noexcept;

        [[nodiscard]] bool IsFinite() const noexcept;
        [[nodiscard]] bool IsHDR() const noexcept;

        [[nodiscard]] Vector3D<TScalar> ToVector3D() const noexcept;
        [[nodiscard]] Vector4D<TScalar> ToVector4D(TScalar alpha) const noexcept;
        [[nodiscard]] ColorRGBA<TScalar>
                ToColorRGBA(TScalar alpha = static_cast<TScalar>(Integral<TScalar> ? 255 : 1)) const noexcept;

        [[nodiscard]] static ColorRGB GetBlack() noexcept;
        [[nodiscard]] static ColorRGB GetWhite() noexcept;

        [[nodiscard]] static ColorRGB GetRed() noexcept;
        [[nodiscard]] static ColorRGB GetGreen() noexcept;
        [[nodiscard]] static ColorRGB GetBlue() noexcept;

        [[nodiscard]] static ColorRGB GetYellow() noexcept;
        [[nodiscard]] static ColorRGB GetCyan() noexcept;
        [[nodiscard]] static ColorRGB GetMagenta() noexcept;

        [[nodiscard]] static ColorRGB GetGray() noexcept;
        [[nodiscard]] static ColorRGB GetGrey() noexcept;

        [[nodiscard]] static bool
                IsApproximately(const ColorRGB& lhs, const ColorRGB& rhs, Float32 epsilon = Mathf::EPSILON) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static Float32 LinearToGammaSpace(Float32 value) noexcept
            requires Floatable<TScalar>;
        [[nodiscard]] static Float32 GammaToLinearSpace(Float32 value) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static ColorRGB HSVToRGB(Float32 h, Float32 s, Float32 v) noexcept
            requires Floatable<TScalar>;
        [[nodiscard]] static ColorRGB HSVToRGB(Float32 h, Float32 s, Float32 v, bool hdr) noexcept
            requires Floatable<TScalar>;
        static void RGBToHSV(const ColorRGB& rgbColor, Float32& h, Float32& s, Float32& v) noexcept
            requires Floatable<TScalar>;

        [[nodiscard]] static ColorRGB Lerp(const ColorRGB& a, const ColorRGB& b, Float32 t) noexcept
            requires Floatable<TScalar>;
        [[nodiscard]] static ColorRGB LerpUnclamped(const ColorRGB& a, const ColorRGB& b, Float32 t) noexcept
            requires Floatable<TScalar>;

    private:
        std::array<TScalar, 3> elements = {};
    };

    using ColorRGBInt   = ColorRGB<Int32>;
    using ColorRGBFloat = ColorRGB<Float32>;

#if !defined(Garage_EXPORTS)
    extern template class GARAGE_API ColorRGB<Int32>;
    extern template class GARAGE_API ColorRGB<Float32>;
#endif
} // namespace Garage

#endif // GARAGE_MATH_COLOR_RGB_HPP
