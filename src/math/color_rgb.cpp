#include "garage/math/color_rgb.hpp"

#include <algorithm>
#include <cmath>

#include "garage/core/common.hpp"

#include "garage/math/color_rgba.hpp"
#include "garage/math/mathf.hpp"
#include "garage/math/vector3d.hpp"
#include "garage/math/vector4d.hpp"

namespace Garage
{
    template <Scalarable TScalar>
    ColorRGB<TScalar>::ColorRGB() noexcept = default;

    template <Scalarable TScalar>
    ColorRGB<TScalar>::ColorRGB(TScalar value) noexcept
        : elements{value, value, value}
    {
    }

    template <Scalarable TScalar>
    ColorRGB<TScalar>::ColorRGB(TScalar r, TScalar g, TScalar b) noexcept
        : elements{r, g, b}
    {
    }

    template <Scalarable TScalar>
    ColorRGB<TScalar>::ColorRGB(const Vector3D<TScalar>& vector) noexcept
        : elements{vector.GetX(), vector.GetY(), vector.GetZ()}
    {
    }

    template <Scalarable TScalar>
    ColorRGB<TScalar>::ColorRGB(const Vector4D<TScalar>& vector) noexcept
        : elements{vector.GetX(), vector.GetY(), vector.GetZ()}
    {
    }

    template <Scalarable TScalar>
    ColorRGB<TScalar>::ColorRGB(const ColorRGBA<TScalar>& color) noexcept
        : elements{color.GetR(), color.GetG(), color.GetB()}
    {
    }

    template <Scalarable TScalar>
    ColorRGB<TScalar>::operator const TScalar*() const noexcept
    {
        return elements.data();
    }

    template <Scalarable TScalar>
    ColorRGB<TScalar>::operator TScalar*() noexcept
    {
        return elements.data();
    }

    template <Scalarable TScalar>
    TScalar ColorRGB<TScalar>::operator[](USize index) const noexcept
    {
        ASSERT(index < 3);
        return elements[index];
    }

    template <Scalarable TScalar>
    TScalar& ColorRGB<TScalar>::operator[](USize index) noexcept
    {
        ASSERT(index < 3);
        return elements[index];
    }

    template <Scalarable TScalar>
    bool ColorRGB<TScalar>::operator==(const ColorRGB& other) const noexcept
    {
        if constexpr (Integral<TScalar>)
        {
            return elements[0] == other.elements[0] && elements[1] == other.elements[1] &&
                    elements[2] == other.elements[2];
        }
        else
        {
            return IsApproximately(*this, other);
        }
    }

    template <Scalarable TScalar>
    bool ColorRGB<TScalar>::operator!=(const ColorRGB& other) const noexcept
    {
        return !(*this == other);
    }

    template <Scalarable TScalar>
    TScalar ColorRGB<TScalar>::GetR() const noexcept
    {
        return elements[0];
    }

    template <Scalarable TScalar>
    void ColorRGB<TScalar>::SetR(TScalar component) noexcept
    {
        elements[0] = component;
    }

    template <Scalarable TScalar>
    TScalar ColorRGB<TScalar>::GetG() const noexcept
    {
        return elements[1];
    }

    template <Scalarable TScalar>
    void ColorRGB<TScalar>::SetG(TScalar component) noexcept
    {
        elements[1] = component;
    }

    template <Scalarable TScalar>
    TScalar ColorRGB<TScalar>::GetB() const noexcept
    {
        return elements[2];
    }

    template <Scalarable TScalar>
    void ColorRGB<TScalar>::SetB(TScalar component) noexcept
    {
        elements[2] = component;
    }

    template <Scalarable TScalar>
    void ColorRGB<TScalar>::Set(TScalar rValue, TScalar gValue, TScalar bValue) noexcept
    {
        elements[0] = rValue;
        elements[1] = gValue;
        elements[2] = bValue;
    }

    template <Scalarable TScalar>
    ColorRGB<TScalar> ColorRGB<TScalar>::GetGamma() const noexcept
        requires Floatable<TScalar>
    {
        return ColorRGB(
                LinearToGammaSpace(elements[0]), LinearToGammaSpace(elements[1]), LinearToGammaSpace(elements[2]));
    }

    template <Scalarable TScalar>
    ColorRGB<TScalar> ColorRGB<TScalar>::GetLinear() const noexcept
        requires Floatable<TScalar>
    {
        return ColorRGB(
                GammaToLinearSpace(elements[0]), GammaToLinearSpace(elements[1]), GammaToLinearSpace(elements[2]));
    }

    template <Scalarable TScalar>
    Float32 ColorRGB<TScalar>::GetGrayscale() const noexcept
        requires Floatable<TScalar>
    {
        return 0.299f * elements[0] + 0.587f * elements[1] + 0.114f * elements[2];
    }

    template <Scalarable TScalar>
    TScalar ColorRGB<TScalar>::GetMaxColorComponent() const noexcept
    {
        return std::max(elements[0], std::max(elements[1], elements[2]));
    }

    template <Scalarable TScalar>
    bool ColorRGB<TScalar>::IsFinite() const noexcept
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
    bool ColorRGB<TScalar>::IsHDR() const noexcept
    {
        constexpr TScalar MAXIMUM = static_cast<TScalar>(Integral<TScalar> ? 255 : 1);
        return elements[0] > MAXIMUM || elements[1] > MAXIMUM || elements[2] > MAXIMUM;
    }

    template <Scalarable TScalar>
    Vector3D<TScalar> ColorRGB<TScalar>::ToVector3D() const noexcept
    {
        return Vector3D<TScalar>(elements[0], elements[1], elements[2]);
    }

    template <Scalarable TScalar>
    Vector4D<TScalar> ColorRGB<TScalar>::ToVector4D(TScalar alpha) const noexcept
    {
        return Vector4D<TScalar>(elements[0], elements[1], elements[2], alpha);
    }

    template <Scalarable TScalar>
    ColorRGBA<TScalar> ColorRGB<TScalar>::ToColorRGBA(TScalar alpha) const noexcept
    {
        return ColorRGBA<TScalar>(elements[0], elements[1], elements[2], alpha);
    }

    template <Scalarable TScalar>
    ColorRGB<TScalar> ColorRGB<TScalar>::GetBlack() noexcept
    {
        return ColorRGB(TScalar{0}, TScalar{0}, TScalar{0});
    }

    template <Scalarable TScalar>
    ColorRGB<TScalar> ColorRGB<TScalar>::GetWhite() noexcept
    {
        constexpr TScalar MAX_VAL = static_cast<TScalar>(Integral<TScalar> ? 255 : 1);
        return ColorRGB(MAX_VAL, MAX_VAL, MAX_VAL);
    }

    template <Scalarable TScalar>
    ColorRGB<TScalar> ColorRGB<TScalar>::GetRed() noexcept
    {
        constexpr TScalar MAX_VAL = static_cast<TScalar>(Integral<TScalar> ? 255 : 1);
        return ColorRGB(MAX_VAL, TScalar{0}, TScalar{0});
    }

    template <Scalarable TScalar>
    ColorRGB<TScalar> ColorRGB<TScalar>::GetGreen() noexcept
    {
        constexpr TScalar MAX_VAL = static_cast<TScalar>(Integral<TScalar> ? 255 : 1);
        return ColorRGB(TScalar{0}, MAX_VAL, TScalar{0});
    }

    template <Scalarable TScalar>
    ColorRGB<TScalar> ColorRGB<TScalar>::GetBlue() noexcept
    {
        constexpr TScalar MAX_VAL = static_cast<TScalar>(Integral<TScalar> ? 255 : 1);
        return ColorRGB(TScalar{0}, TScalar{0}, MAX_VAL);
    }

    template <Scalarable TScalar>
    ColorRGB<TScalar> ColorRGB<TScalar>::GetYellow() noexcept
    {
        if constexpr (Integral<TScalar>)
        {
            return ColorRGB(255, 235, 4);
        }
        else
        {
            return ColorRGB(1.0f, 0.92156863f, 0.015686275f);
        }
    }

    template <Scalarable TScalar>
    ColorRGB<TScalar> ColorRGB<TScalar>::GetCyan() noexcept
    {
        constexpr TScalar MAX_VAL = static_cast<TScalar>(Integral<TScalar> ? 255 : 1);
        return ColorRGB(TScalar{0}, MAX_VAL, MAX_VAL);
    }

    template <Scalarable TScalar>
    ColorRGB<TScalar> ColorRGB<TScalar>::GetMagenta() noexcept
    {
        constexpr TScalar MAX_VAL = static_cast<TScalar>(Integral<TScalar> ? 255 : 1);
        return ColorRGB(MAX_VAL, TScalar{0}, MAX_VAL);
    }

    template <Scalarable TScalar>
    ColorRGB<TScalar> ColorRGB<TScalar>::GetGray() noexcept
    {
        if constexpr (Integral<TScalar>)
        {
            return ColorRGB(128, 128, 128);
        }
        else
        {
            return ColorRGB(0.5f, 0.5f, 0.5f);
        }
    }

    template <Scalarable TScalar>
    ColorRGB<TScalar> ColorRGB<TScalar>::GetGrey() noexcept
    {
        return GetGray();
    }

    template <Scalarable TScalar>
    bool ColorRGB<TScalar>::IsApproximately(const ColorRGB& lhs, const ColorRGB& rhs, Float32 epsilon) noexcept
        requires Floatable<TScalar>
    {
        return std::abs(lhs.elements[0] - rhs.elements[0]) <= epsilon &&
                std::abs(lhs.elements[1] - rhs.elements[1]) <= epsilon &&
                std::abs(lhs.elements[2] - rhs.elements[2]) <= epsilon;
    }

    template <Scalarable TScalar>
    Float32 ColorRGB<TScalar>::LinearToGammaSpace(Float32 value) noexcept
        requires Floatable<TScalar>
    {
        if (value <= 0.0f)
        {
            return 0.0f;
        }
        if (value <= 0.0031308f)
        {
            return value * 12.92f;
        }
        if (value < 1.0f)
        {
            return 1.055f * std::pow(value, 0.4166667f) - 0.055f;
        }
        return std::pow(value, 0.45454545f);
    }

    template <Scalarable TScalar>
    Float32 ColorRGB<TScalar>::GammaToLinearSpace(Float32 value) noexcept
        requires Floatable<TScalar>
    {
        if (value <= 0.04045f)
        {
            return value / 12.92f;
        }
        if (value < 1.0f)
        {
            return std::pow((value + 0.055f) / 1.055f, 2.4f);
        }
        return std::pow(value, 2.2f);
    }

    template <Scalarable TScalar>
    ColorRGB<TScalar> ColorRGB<TScalar>::HSVToRGB(Float32 h, Float32 s, Float32 v) noexcept
        requires Floatable<TScalar>
    {
        return HSVToRGB(h, s, v, true);
    }

    template <Scalarable TScalar>
    ColorRGB<TScalar> ColorRGB<TScalar>::HSVToRGB(Float32 h, Float32 s, Float32 v, bool hdr) noexcept
        requires Floatable<TScalar>
    {
        ColorRGB retval = GetWhite();
        if (s == 0.0f)
        {
            retval.elements[0] = v;
            retval.elements[1] = v;
            retval.elements[2] = v;
        }
        else if (v == 0.0f)
        {
            retval.elements[0] = 0.0f;
            retval.elements[1] = 0.0f;
            retval.elements[2] = 0.0f;
        }
        else
        {
            retval.elements[0] = 0.0f;
            retval.elements[1] = 0.0f;
            retval.elements[2] = 0.0f;

            const Float32 H_SCALED = h * 6.0f;
            const auto    SECTOR   = static_cast<Int32>(std::floor(H_SCALED));
            const Float32 FRAC     = H_SCALED - static_cast<Float32>(SECTOR);
            const Float32 P        = v * (1.0f - s);
            const Float32 Q        = v * (1.0f - s * FRAC);
            const Float32 T        = v * (1.0f - s * (1.0f - FRAC));

            switch (SECTOR)
            {
                case 0:
                    retval.elements[0] = v;
                    retval.elements[1] = T;
                    retval.elements[2] = P;
                    break;
                case 1:
                    retval.elements[0] = Q;
                    retval.elements[1] = v;
                    retval.elements[2] = P;
                    break;
                case 2:
                    retval.elements[0] = P;
                    retval.elements[1] = v;
                    retval.elements[2] = T;
                    break;
                case 3:
                    retval.elements[0] = P;
                    retval.elements[1] = Q;
                    retval.elements[2] = v;
                    break;
                case 4:
                    retval.elements[0] = T;
                    retval.elements[1] = P;
                    retval.elements[2] = v;
                    break;
                case 5:
                    retval.elements[0] = v;
                    retval.elements[1] = P;
                    retval.elements[2] = Q;
                    break;
                case 6:
                    retval.elements[0] = v;
                    retval.elements[1] = T;
                    retval.elements[2] = P;
                    break;
                case -1:
                    retval.elements[0] = v;
                    retval.elements[1] = P;
                    retval.elements[2] = Q;
                    break;
                default:
                    break;
            }
        }

        if (!hdr)
        {
            retval.elements[0] = std::clamp(retval.elements[0], 0.0f, 1.0f);
            retval.elements[1] = std::clamp(retval.elements[1], 0.0f, 1.0f);
            retval.elements[2] = std::clamp(retval.elements[2], 0.0f, 1.0f);
        }

        return retval;
    }

    template <Scalarable TScalar>
    void ColorRGB<TScalar>::RGBToHSV(const ColorRGB& rgbColor, Float32& h, Float32& s, Float32& v) noexcept
        requires Floatable<TScalar>
    {
        const Float32 MAX_COMP = std::max(rgbColor.elements[0], std::max(rgbColor.elements[1], rgbColor.elements[2]));
        const Float32 MIN_COMP = std::min(rgbColor.elements[0], std::min(rgbColor.elements[1], rgbColor.elements[2]));
        const Float32 DELTA    = MAX_COMP - MIN_COMP;

        v = MAX_COMP;
        if (MAX_COMP <= 0.0f || DELTA <= 0.0f)
        {
            s = 0.0f;
            h = 0.0f;
            return;
        }

        s = DELTA / MAX_COMP;
        if (rgbColor.elements[0] >= MAX_COMP)
        {
            h = (rgbColor.elements[1] - rgbColor.elements[2]) / DELTA;
        }
        else if (rgbColor.elements[1] >= MAX_COMP)
        {
            h = 2.0f + (rgbColor.elements[2] - rgbColor.elements[0]) / DELTA;
        }
        else
        {
            h = 4.0f + (rgbColor.elements[0] - rgbColor.elements[1]) / DELTA;
        }

        h /= 6.0f;
        if (h < 0.0f)
        {
            h += 1.0f;
        }
    }

    template <Scalarable TScalar>
    ColorRGB<TScalar> ColorRGB<TScalar>::Lerp(const ColorRGB& a, const ColorRGB& b, Float32 t) noexcept
        requires Floatable<TScalar>
    {
        return LerpUnclamped(a, b, std::clamp(t, 0.0f, 1.0f));
    }

    template <Scalarable TScalar>
    ColorRGB<TScalar> ColorRGB<TScalar>::LerpUnclamped(const ColorRGB& a, const ColorRGB& b, Float32 t) noexcept
        requires Floatable<TScalar>
    {
        return ColorRGB(Mathf::LerpUnclamped(a.elements[0], b.elements[0], t),
                        Mathf::LerpUnclamped(a.elements[1], b.elements[1], t),
                        Mathf::LerpUnclamped(a.elements[2], b.elements[2], t));
    }

    template class GARAGE_API ColorRGB<Int32>;
    template class GARAGE_API ColorRGB<Float32>;
} // namespace Garage
