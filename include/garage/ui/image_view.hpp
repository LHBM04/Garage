#ifndef GARAGE_UI_IMAGE_VIEW_HPP
#define GARAGE_UI_IMAGE_VIEW_HPP

#include "../actors/component.hpp"

#include "../core/common.hpp"

#include "../math/color_rgba.hpp"

namespace Garage
{
    class MaterialContent;
    class SystemContext;
    class TextureContent;

    class GARAGE_API ImageView final : public ComponentBase
    {
    public:
        using ComponentBase::ComponentBase;
        ~ImageView() noexcept override = default;

        void OnPostRender(SystemContext& context) override;

        [[nodiscard]] const TextureContent* GetImage() const noexcept;
        void                                SetImage(const TextureContent* value) noexcept;

        [[nodiscard]] const MaterialContent* GetMaterial() const noexcept;
        void                                 SetMaterial(const MaterialContent* value) noexcept;

        [[nodiscard]] const ColorRGBA<Float32>& GetColor() const noexcept;
        void                                    SetColor(const ColorRGBA<Float32>& value) noexcept;

    private:
        ColorRGBA<Float32>     color    = ColorRGBA<Float32>::GetWhite();
        const TextureContent*  texture  = nullptr;
        const MaterialContent* material = nullptr;
    };

    inline const TextureContent* ImageView::GetImage() const noexcept
    {
        return texture;
    }

    inline void ImageView::SetImage(const TextureContent* value) noexcept
    {
        texture = value;
    }

    inline const MaterialContent* ImageView::GetMaterial() const noexcept
    {
        return material;
    }

    inline void ImageView::SetMaterial(const MaterialContent* value) noexcept
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
