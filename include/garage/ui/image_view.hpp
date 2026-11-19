#ifndef GARAGE_UI_IMAGE_VIEW_HPP
#define GARAGE_UI_IMAGE_VIEW_HPP

#include "../actors/component.hpp"

#include "../core/common.hpp"

#include "../math/color_rgba.hpp"

namespace Garage
{
    class MaterialAsset;
    class SystemContext;
    class TextureAsset;

    class GARAGE_API ImageView final : public ComponentBase
    {
    public:
        using ComponentBase::ComponentBase;
        ~ImageView() noexcept override = default;

        void OnPostRender(SystemContext& context) override;

        [[nodiscard]] const TextureAsset* GetImage() const noexcept;
        void                              SetImage(const TextureAsset* value) noexcept;

        [[nodiscard]] const MaterialAsset* GetMaterial() const noexcept;
        void                               SetMaterial(const MaterialAsset* value) noexcept;

        [[nodiscard]] const ColorRGBA<Float32>& GetColor() const noexcept;
        void                                    SetColor(const ColorRGBA<Float32>& value) noexcept;

    private:
        ColorRGBA<Float32>   color    = ColorRGBA<Float32>::GetWhite();
        const TextureAsset*  texture  = nullptr;
        const MaterialAsset* material = nullptr;
    };

    inline const TextureAsset* ImageView::GetImage() const noexcept
    {
        return texture;
    }

    inline void ImageView::SetImage(const TextureAsset* value) noexcept
    {
        texture = value;
    }

    inline const MaterialAsset* ImageView::GetMaterial() const noexcept
    {
        return material;
    }

    inline void ImageView::SetMaterial(const MaterialAsset* value) noexcept
    {
        material = value;
    }

    inline const ColorRGBA<Float32>& ImageView::GetColor() const noexcept
    {
        return color;
    }

    inline void ImageView::SetColor(const ColorRGBA<Float32>& value) noexcept
    {
        color = value;
    }
} // namespace Garage

#endif // GARAGE_UI_IMAGE_VIEW_HPP
