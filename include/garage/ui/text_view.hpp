#ifndef GARAGE_UI_TEXT_VIEW_HPP
#define GARAGE_UI_TEXT_VIEW_HPP

#include "../actors/component.hpp"

#include "../core/common.hpp"
#include "../core/common/containers.hpp"

#include "../math/color_rgba.hpp"

namespace Garage
{
    class BitmapFontAsset;
    struct FontCharacterData;
    class MaterialAsset;
    class SystemContext;
    class TextureAsset;

    enum class ETextHorizontalAlignment : UInt8
    {
        LEFT,
        CENTER,
        RIGHT,
    };

    class GARAGE_API TextView final : public ComponentBase
    {
    public:
        using ComponentBase::ComponentBase;
        ~TextView() noexcept override = default;

        void OnPostRender(SystemContext& context) override;

        [[nodiscard]] const TextureAsset* GetTexture() const noexcept;
        void                              SetTexture(const TextureAsset* value) noexcept;

        [[nodiscard]] const BitmapFontAsset* GetFont() const noexcept;
        void                                 SetFont(const BitmapFontAsset* value) noexcept;

        [[nodiscard]] const MaterialAsset* GetMaterial() const noexcept;
        void                               SetMaterial(const MaterialAsset* value) noexcept;

        [[nodiscard]] const String& GetText() const noexcept;
        void                        SetText(StringView value) noexcept;

        [[nodiscard]] Float32 GetFontSize() const noexcept;
        void                  SetFontSize(Float32 value) noexcept;

        [[nodiscard]] const ColorRGBA<Float32>& GetColor() const noexcept;
        void                                    SetColor(const ColorRGBA<Float32>& value) noexcept;

        [[nodiscard]] ETextHorizontalAlignment GetHorizontalAlignment() const noexcept;
        void                                   SetHorizontalAlignment(ETextHorizontalAlignment value) noexcept;

    private:
        [[nodiscard]] const FontCharacterData* GetCharacter(UInt32 unicode) const noexcept;

        const TextureAsset*    texture  = nullptr;
        const BitmapFontAsset* font     = nullptr;
        const MaterialAsset*   material = nullptr;

        String                                         text{"New Text!"};
        Float32                                        fontSize            = 32.0f;
        ETextHorizontalAlignment                       horizontalAlignment = ETextHorizontalAlignment::LEFT;
        ColorRGBA<Float32>                             color               = ColorRGBA<Float32>::GetWhite();
        UnorderedMap<UInt32, const FontCharacterData*> characterMap;
    };

    inline const TextureAsset* TextView::GetTexture() const noexcept
    {
        return texture;
    }

    inline void TextView::SetTexture(const TextureAsset* value) noexcept
    {
        texture = value;
    }

    inline const BitmapFontAsset* TextView::GetFont() const noexcept
    {
        return font;
    }

    inline const MaterialAsset* TextView::GetMaterial() const noexcept
    {
        return material;
    }

    inline void TextView::SetMaterial(const MaterialAsset* value) noexcept
    {
        material = value;
    }

    inline const String& TextView::GetText() const noexcept
    {
        return text;
    }

    inline void TextView::SetText(StringView value) noexcept
    {
        text = value;
    }

    inline Float32 TextView::GetFontSize() const noexcept
    {
        return fontSize;
    }

    inline void TextView::SetFontSize(Float32 value) noexcept
    {
        fontSize = value;
    }

    inline const ColorRGBA<Float32>& TextView::GetColor() const noexcept
    {
        return color;
    }

    inline void TextView::SetColor(const ColorRGBA<Float32>& value) noexcept
    {
        color = value;
    }

    inline ETextHorizontalAlignment TextView::GetHorizontalAlignment() const noexcept
    {
        return horizontalAlignment;
    }

    inline void TextView::SetHorizontalAlignment(ETextHorizontalAlignment value) noexcept
    {
        horizontalAlignment = value;
    }
} // namespace Garage

#endif // GARAGE_UI_TEXT_VIEW_HPP
