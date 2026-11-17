#include "garage/math/color_rgba.hpp"

#include <algorithm>
#include <cmath>

#include "garage/core/defines.hpp"

#include "garage/math/color_rgb.hpp"
#include "garage/math/mathf.hpp"

namespace Garage
{
    template <Scalarable TScalar>
    ColorRGBA<TScalar>::ColorRGBA() noexcept
        : elements{TScalar{0}, TScalar{0}, TScalar{0}, static_cast<TScalar>(Integral<TScalar> ? 255 : 1)}
    {
    }

    template <Scalarable TScalar>
    ColorRGBA<TScalar>::ColorRGBA(TScalar value) noexcept
        : elements{value, value, value, value}
    {
    }

    template <Scalarable TScalar>
    ColorRGBA<TScalar>::ColorRGBA(TScalar r, TScalar g, TScalar b, TScalar a) noexcept
        : elements{r, g, b, a}
    {
    }

    template <Scalarable TScalar>
    ColorRGBA<TScalar>::ColorRGBA(const TScalar* values) noexcept
    {
        ASSERT(values != nullptr);
        elements[0] = values[0];
        elements[1] = values[1];
        elements[2] = values[2];
        elements[3] = values[3];
    }

    template <Scalarable TScalar>
    ColorRGBA<TScalar>::ColorRGBA(const ColorRGB<TScalar>& rgb, TScalar a) noexcept
        : elements{rgb.GetR(), rgb.GetG(), rgb.GetB(), a}
    {
    }

    template <Scalarable TScalar>
    ColorRGBA<TScalar>::ColorRGBA(const Vector3D<TScalar>& vector, TScalar alpha) noexcept
        : elements{vector.GetX(), vector.GetY(), vector.GetZ(), alpha}
    {
    }

    template <Scalarable TScalar>
    ColorRGBA<TScalar>::ColorRGBA(const Vector4D<TScalar>& vector) noexcept
        : elements{vector.GetX(), vector.GetY(), vector.GetZ(), vector.GetW()}
    {
    }

    template <Scalarable TScalar>
    ColorRGBA<TScalar>& ColorRGBA<TScalar>::operator=(const Vector4D<TScalar>& other) noexcept
    {
        elements[0] = other.GetX();
        elements[1] = other.GetY();
        elements[2] = other.GetZ();
        elements[3] = other.GetW();
        return *this;
    }

    template <Scalarable TScalar>
    ColorRGBA<TScalar>::operator Vector4D<TScalar>() const noexcept
    {
        return Vector4D<TScalar>(elements[0], elements[1], elements[2], elements[3]);
    }

    template <Scalarable TScalar>
    ColorRGBA<TScalar>::operator const TScalar*() const noexcept
    {
        return elements.data();
    }

    template <Scalarable TScalar>
    ColorRGBA<TScalar>::operator TScalar*() noexcept
    {
        return elements.data();
    }

    template <Scalarable TScalar>
    TScalar ColorRGBA<TScalar>::operator[](USize index) const noexcept
    {
        ASSERT(index < 4);
        return elements[index];
    }

    template <Scalarable TScalar>
    TScalar& ColorRGBA<TScalar>::operator[](USize index) noexcept
    {
        ASSERT(index < 4);
        return elements[index];
    }

    template <Scalarable TScalar>
    ColorRGBA<TScalar> ColorRGBA<TScalar>::operator+(const ColorRGBA& other) const noexcept
    {
        return ColorRGBA(elements[0] + other.elements[0],
                         elements[1] + other.elements[1],
                         elements[2] + other.elements[2],
                         elements[3] + other.elements[3]);
    }

    template <Scalarable TScalar>
    ColorRGBA<TScalar>& ColorRGBA<TScalar>::operator+=(const ColorRGBA& other) noexcept
    {
        elements[0] += other.elements[0];
        elements[1] += other.elements[1];
        elements[2] += other.elements[2];
        elements[3] += other.elements[3];
        return *this;
    }

    template <Scalarable TScalar>
    ColorRGBA<TScalar> ColorRGBA<TScalar>::operator-(const ColorRGBA& other) const noexcept
    {
        return ColorRGBA(elements[0] - other.elements[0],
                         elements[1] - other.elements[1],
                         elements[2] - other.elements[2],
                         elements[3] - other.elements[3]);
    }

    template <Scalarable TScalar>
    ColorRGBA<TScalar>& ColorRGBA<TScalar>::operator-=(const ColorRGBA& other) noexcept
    {
        elements[0] -= other.elements[0];
        elements[1] -= other.elements[1];
        elements[2] -= other.elements[2];
        elements[3] -= other.elements[3];
        return *this;
    }

    template <Scalarable TScalar>
    ColorRGBA<TScalar> ColorRGBA<TScalar>::operator*(const ColorRGBA& other) const noexcept
    {
        return ColorRGBA(elements[0] * other.elements[0],
                         elements[1] * other.elements[1],
                         elements[2] * other.elements[2],
                         elements[3] * other.elements[3]);
    }

    template <Scalarable TScalar>
    ColorRGBA<TScalar> ColorRGBA<TScalar>::operator*(TScalar scalar) const noexcept
    {
        return ColorRGBA(elements[0] * scalar, elements[1] * scalar, elements[2] * scalar, elements[3] * scalar);
    }

    template <Scalarable TScalar>
    ColorRGBA<TScalar>& ColorRGBA<TScalar>::operator*=(const ColorRGBA& other) noexcept
    {
        elements[0] *= other.elements[0];
        elements[1] *= other.elements[1];
        elements[2] *= other.elements[2];
        elements[3] *= other.elements[3];
        return *this;
    }

    template <Scalarable TScalar>
    ColorRGBA<TScalar>& ColorRGBA<TScalar>::operator*=(TScalar scalar) noexcept
    {
        elements[0] *= scalar;
        elements[1] *= scalar;
        elements[2] *= scalar;
        elements[3] *= scalar;
        return *this;
    }

    template <Scalarable TScalar>
    ColorRGBA<TScalar> ColorRGBA<TScalar>::operator/(TScalar scalar) const noexcept
    {
        ASSERT(scalar != TScalar{});
        return ColorRGBA(elements[0] / scalar, elements[1] / scalar, elements[2] / scalar, elements[3] / scalar);
    }

    template <Scalarable TScalar>
    ColorRGBA<TScalar>& ColorRGBA<TScalar>::operator/=(TScalar scalar) noexcept
    {
        ASSERT(scalar != TScalar{});
        elements[0] /= scalar;
        elements[1] /= scalar;
        elements[2] /= scalar;
        elements[3] /= scalar;
        return *this;
    }

    template <Scalarable TScalar>
    bool ColorRGBA<TScalar>::operator==(const ColorRGBA& other) const noexcept
    {
        if constexpr (Integral<TScalar>)
        {
            return elements[0] == other.elements[0] && elements[1] == other.elements[1] &&
                    elements[2] == other.elements[2] && elements[3] == other.elements[3];
        }
        else
        {
            return IsApproximately(*this, other);
        }
    }

    template <Scalarable TScalar>
    bool ColorRGBA<TScalar>::operator!=(const ColorRGBA& other) const noexcept
    {
        return !(*this == other);
    }

    template <Scalarable TScalar>
    TScalar ColorRGBA<TScalar>::GetR() const noexcept
    {
        return elements[0];
    }

    template <Scalarable TScalar>
    void ColorRGBA<TScalar>::SetR(TScalar component) noexcept
    {
        elements[0] = component;
    }

    template <Scalarable TScalar>
    TScalar ColorRGBA<TScalar>::GetG() const noexcept
    {
        return elements[1];
    }

    template <Scalarable TScalar>
    void ColorRGBA<TScalar>::SetG(TScalar component) noexcept
    {
        elements[1] = component;
    }

    template <Scalarable TScalar>
    TScalar ColorRGBA<TScalar>::GetB() const noexcept
    {
        return elements[2];
    }

    template <Scalarable TScalar>
    void ColorRGBA<TScalar>::SetB(TScalar component) noexcept
    {
        elements[2] = component;
    }

    template <Scalarable TScalar>
    TScalar ColorRGBA<TScalar>::GetA() const noexcept
    {
        return elements[3];
    }

    template <Scalarable TScalar>
    void ColorRGBA<TScalar>::SetA(TScalar component) noexcept
    {
        elements[3] = component;
    }

    template <Scalarable TScalar>
    void ColorRGBA<TScalar>::Set(TScalar rValue, TScalar gValue, TScalar bValue, TScalar aValue) noexcept
    {
        elements[0] = rValue;
        elements[1] = gValue;
        elements[2] = bValue;
        elements[3] = aValue;
    }

    template <Scalarable TScalar>
    bool ColorRGBA<TScalar>::IsTransparent(Float32 epsilon) const noexcept
    {
        if constexpr (Integral<TScalar>)
        {
            return elements[3] == 0;
        }
        else
        {
            return std::abs(elements[3]) <= epsilon;
        }
    }

    template <Scalarable TScalar>
    bool ColorRGBA<TScalar>::IsOpaque(Float32 epsilon) const noexcept
    {
        if constexpr (Integral<TScalar>)
        {
            return elements[3] == 255;
        }
        else
        {
            return std::abs(elements[3] - 1.0f) <= epsilon;
        }
    }

    template <Scalarable TScalar>
    Vector4D<TScalar> ColorRGBA<TScalar>::ToVector4D() const noexcept
    {
        return Vector4D<TScalar>(elements[0], elements[1], elements[2], elements[3]);
    }

    template <Scalarable TScalar>
    ColorRGB<TScalar> ColorRGBA<TScalar>::ToColorRGB() const noexcept
    {
        return ColorRGB<TScalar>(elements[0], elements[1], elements[2]);
    }

    template <Scalarable TScalar>
    ColorRGBA<TScalar> ColorRGBA<TScalar>::GetBlack() noexcept
    {
        return ColorRGBA(TScalar{0}, TScalar{0}, TScalar{0});
    }

    template <Scalarable TScalar>
    ColorRGBA<TScalar> ColorRGBA<TScalar>::GetWhite() noexcept
    {
        constexpr TScalar maxVal = static_cast<TScalar>(Integral<TScalar> ? 255 : 1);
        return ColorRGBA(maxVal, maxVal, maxVal, maxVal);
    }

    template <Scalarable TScalar>
    ColorRGBA<TScalar> ColorRGBA<TScalar>::GetRed() noexcept
    {
        constexpr TScalar maxVal = static_cast<TScalar>(Integral<TScalar> ? 255 : 1);
        return ColorRGBA(maxVal, TScalar{0}, TScalar{0}, maxVal);
    }

    template <Scalarable TScalar>
    ColorRGBA<TScalar> ColorRGBA<TScalar>::GetGreen() noexcept
    {
        constexpr TScalar maxVal = static_cast<TScalar>(Integral<TScalar> ? 255 : 1);
        return ColorRGBA(TScalar{0}, maxVal, TScalar{0}, maxVal);
    }

    template <Scalarable TScalar>
    ColorRGBA<TScalar> ColorRGBA<TScalar>::GetBlue() noexcept
    {
        constexpr TScalar maxVal = static_cast<TScalar>(Integral<TScalar> ? 255 : 1);
        return ColorRGBA(TScalar{0}, TScalar{0}, maxVal, maxVal);
    }

    template <Scalarable TScalar>
    ColorRGBA<TScalar> ColorRGBA<TScalar>::GetYellow() noexcept
    {
        if constexpr (Integral<TScalar>)
        {
            return ColorRGBA(255, 235, 4, 255);
        }
        else
        {
            return ColorRGBA(1.0f, 0.92156863f, 0.015686275f, 1.0f);
        }
    }

    template <Scalarable TScalar>
    ColorRGBA<TScalar> ColorRGBA<TScalar>::GetCyan() noexcept
    {
        constexpr TScalar maxVal = static_cast<TScalar>(Integral<TScalar> ? 255 : 1);
        return ColorRGBA(TScalar{0}, maxVal, maxVal, maxVal);
    }

    template <Scalarable TScalar>
    ColorRGBA<TScalar> ColorRGBA<TScalar>::GetMagenta() noexcept
    {
        constexpr TScalar maxVal = static_cast<TScalar>(Integral<TScalar> ? 255 : 1);
        return ColorRGBA(maxVal, TScalar{0}, maxVal, maxVal);
    }

    template <Scalarable TScalar>
    ColorRGBA<TScalar> ColorRGBA<TScalar>::GetClear() noexcept
    {
        return ColorRGBA(TScalar{0}, TScalar{0}, TScalar{0}, TScalar{0});
    }

    template <Scalarable TScalar>
    bool ColorRGBA<TScalar>::IsApproximately(const ColorRGBA& lhs, const ColorRGBA& rhs, Float32 epsilon) noexcept
        requires Floatable<TScalar>
    {
        return std::abs(lhs.elements[0] - rhs.elements[0]) <= epsilon &&
                std::abs(lhs.elements[1] - rhs.elements[1]) <= epsilon &&
                std::abs(lhs.elements[2] - rhs.elements[2]) <= epsilon &&
                std::abs(lhs.elements[3] - rhs.elements[3]) <= epsilon;
    }

    template <Scalarable TScalar>
    ColorRGBA<TScalar> ColorRGBA<TScalar>::Lerp(const ColorRGBA& start, const ColorRGBA& end, Float32 t) noexcept
        requires Floatable<TScalar>
    {
        return ColorRGBA(Mathf::Lerp(start.elements[0], end.elements[0], t),
                         Mathf::Lerp(start.elements[1], end.elements[1], t),
                         Mathf::Lerp(start.elements[2], end.elements[2], t),
                         Mathf::Lerp(start.elements[3], end.elements[3], t));
    }

    template class ColorRGBA<Int32>;
    template class ColorRGBA<Float32>;
} // namespace Garage
