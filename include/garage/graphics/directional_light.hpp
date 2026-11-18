#ifndef GARAGE_GRAPHICS_DIRECTIONAL_LIGHT_HPP
#define GARAGE_GRAPHICS_DIRECTIONAL_LIGHT_HPP

#include <algorithm>

#include "../actors/component.hpp"

#include "../core/defines.hpp"
#include "../core/types.hpp"

#include "../math/color_rgb.hpp"
#include "../math/color_rgba.hpp"
#include "../math/vector3d.hpp"

#include "gpu_constants.hpp"

namespace Garage
{
    class GARAGE_API DirectionalLight final : public ComponentBase
    {
    public:
        using ComponentBase::ComponentBase;
        ~DirectionalLight() noexcept override = default;

        void OnEnable() override;
        void OnDisable() override;

        [[nodiscard]] const ColorRGBA<Float32>& GetColor() const noexcept;
        void                                    SetColor(const ColorRGBA<Float32>& value) noexcept;

        [[nodiscard]] ColorRGB<Float32> GetColorRGB() const noexcept;
        void                            SetColor(const ColorRGB<Float32>& value) noexcept;

        [[nodiscard]] Float32 GetIntensity() const noexcept;
        void                  SetIntensity(Float32 value) noexcept;

        [[nodiscard]] const ColorRGBA<Float32>& GetAmbientColor() const noexcept;
        void                                    SetAmbientColor(const ColorRGBA<Float32>& value) noexcept;

        [[nodiscard]] ColorRGB<Float32> GetAmbientColorRGB() const noexcept;
        void                            SetAmbientColor(const ColorRGB<Float32>& value) noexcept;

        [[nodiscard]] Vector3D<Float32> GetDirection() const noexcept;
        void                            SetDirection(const Vector3D<Float32>& value) noexcept;

        [[nodiscard]] Vector3D<Float32> GetToLightDirection() const noexcept;

        [[nodiscard]] bool IsUsingTransformRotation() const noexcept;
        void               SetUseTransformRotation(bool value) noexcept;

        [[nodiscard]] ColorRGBA<Float32> GetEffectiveColor() const noexcept;
        [[nodiscard]] ColorRGBA<Float32> GetEffectiveAmbientColor() const noexcept;

        [[nodiscard]] DirectionalLightConstants GetConstants() const noexcept;

    private:
        ColorRGBA<Float32> color = ColorRGBA<Float32>::GetWhite();
        ColorRGBA<Float32> ambientColor{0.15f, 0.15f, 0.15f, 1.0f};

        Vector3D<Float32> direction{-0.5f, -1.0f, 0.5f};
        Float32           intensity = 1.0f;

        bool useTransformRotation = true;
    };

    inline const ColorRGBA<Float32>& DirectionalLight::GetColor() const noexcept
    {
        return color;
    }

    inline void DirectionalLight::SetColor(const ColorRGBA<Float32>& value) noexcept
    {
        color = value;
    }

    inline ColorRGB<Float32> DirectionalLight::GetColorRGB() const noexcept
    {
        return color.ToColorRGB();
    }

    inline void DirectionalLight::SetColor(const ColorRGB<Float32>& value) noexcept
    {
        color = value.ToColorRGBA(1.0f);
    }

    inline Float32 DirectionalLight::GetIntensity() const noexcept
    {
        return intensity;
    }

    inline void DirectionalLight::SetIntensity(Float32 value) noexcept
    {
        intensity = std::max(0.0f, value);
    }

    inline const ColorRGBA<Float32>& DirectionalLight::GetAmbientColor() const noexcept
    {
        return ambientColor;
    }

    inline void DirectionalLight::SetAmbientColor(const ColorRGBA<Float32>& value) noexcept
    {
        ambientColor = value;
    }

    inline ColorRGB<Float32> DirectionalLight::GetAmbientColorRGB() const noexcept
    {
        return ambientColor.ToColorRGB();
    }

    inline void DirectionalLight::SetAmbientColor(const ColorRGB<Float32>& value) noexcept
    {
        ambientColor = value.ToColorRGBA(1.0f);
    }

    inline void DirectionalLight::SetDirection(const Vector3D<Float32>& value) noexcept
    {
        direction = value;
    }

    inline Vector3D<Float32> DirectionalLight::GetToLightDirection() const noexcept
    {
        return -GetDirection();
    }

    inline bool DirectionalLight::IsUsingTransformRotation() const noexcept
    {
        return useTransformRotation;
    }

    inline void DirectionalLight::SetUseTransformRotation(bool value) noexcept
    {
        useTransformRotation = value;
    }

    inline ColorRGBA<Float32> DirectionalLight::GetEffectiveColor() const noexcept
    {
        return color * intensity;
    }

    inline ColorRGBA<Float32> DirectionalLight::GetEffectiveAmbientColor() const noexcept
    {
        return ambientColor * intensity;
    }
} // namespace Garage

#endif // GARAGE_GRAPHICS_DIRECTIONAL_LIGHT_HPP
