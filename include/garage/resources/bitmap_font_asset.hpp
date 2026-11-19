#ifndef GARAGE_RESOURCES_BITMAP_FONT_ASSET_HPP
#define GARAGE_RESOURCES_BITMAP_FONT_ASSET_HPP

#include <filesystem>
#include <utility>

#include "../core/common.hpp"
#include "../core/common/containers.hpp"

#include "asset.hpp"

namespace Garage
{
    struct GARAGE_API FontAtlasData final
    {
        Int32  index = 0;
        String texture;
        Int32  width  = 0;
        Int32  height = 0;
    };

    struct GARAGE_API FontCharacterData final
    {
        UInt32            unicode        = 0;
        UInt32            glyphIndex     = 0;
        Int32             atlasIndex     = 0;
        Float32           characterScale = 0.0f;
        Float32           glyphScale     = 0.0f;
        Array<Float32, 4> glyphRect{};
        Array<Float32, 4> sampleRect{};
        Array<Float32, 2> uvMin{};
        Array<Float32, 2> uvMax{};
        Float32           width    = 0.0f;
        Float32           height   = 0.0f;
        Float32           bearingX = 0.0f;
        Float32           bearingY = 0.0f;
        Float32           advance  = 0.0f;
    };

    class GARAGE_API BitmapFontAsset final : public AssetBase
    {
    public:
        explicit BitmapFontAsset(std::filesystem::path path) noexcept;

        [[nodiscard]] const String& GetName() const noexcept;
        void                        SetName(String&& value) noexcept;

        [[nodiscard]] const String& GetRenderMode() const noexcept;
        void                        SetRenderMode(String&& value) noexcept;

        [[nodiscard]] bool IsDistanceField() const noexcept;
        void               SetDistanceField(bool value) noexcept;

        [[nodiscard]] Float32 GetPointSize() const noexcept;
        void                  SetPointSize(Float32 value) noexcept;

        [[nodiscard]] Float32 GetFaceScale() const noexcept;
        void                  SetFaceScale(Float32 value) noexcept;

        [[nodiscard]] Float32 GetLineHeight() const noexcept;
        void                  SetLineHeight(Float32 value) noexcept;

        [[nodiscard]] Float32 GetAscentLine() const noexcept;
        void                  SetAscentLine(Float32 value) noexcept;

        [[nodiscard]] Float32 GetDescentLine() const noexcept;
        void                  SetDescentLine(Float32 value) noexcept;

        [[nodiscard]] Int32 GetAtlasPadding() const noexcept;
        void                SetAtlasPadding(Int32 value) noexcept;

        [[nodiscard]] Span<const FontAtlasData> GetAtlasData() const noexcept;
        void                                    SetAtlasData(Vector<FontAtlasData>&& value) noexcept;

        [[nodiscard]] Span<const FontCharacterData> GetCharacters() const noexcept;
        void                                        SetCharacters(Vector<FontCharacterData>&& value) noexcept;

    private:
        String                    name;
        String                    renderMode;
        bool                      isDistanceField = false;
        Float32                   pointSize       = 0.0f;
        Float32                   faceScale       = 0.0f;
        Float32                   lineHeight      = 0.0f;
        Float32                   ascentLine      = 0.0f;
        Float32                   descentLine     = 0.0f;
        Int32                     atlasPadding    = 0;
        Vector<FontAtlasData>     atlasData;
        Vector<FontCharacterData> characters;
    };

    inline BitmapFontAsset::BitmapFontAsset(std::filesystem::path path) noexcept
        : AssetBase(std::move(path))
    {
    }

    inline const String& BitmapFontAsset::GetName() const noexcept
    {
        return name;
    }

    inline void BitmapFontAsset::SetName(String&& value) noexcept
    {
        name = std::move(value);
    }

    inline const String& BitmapFontAsset::GetRenderMode() const noexcept
    {
        return renderMode;
    }

    inline void BitmapFontAsset::SetRenderMode(String&& value) noexcept
    {
        renderMode = std::move(value);
    }

    inline bool BitmapFontAsset::IsDistanceField() const noexcept
    {
        return isDistanceField;
    }

    inline void BitmapFontAsset::SetDistanceField(bool value) noexcept
    {
        isDistanceField = value;
    }

    inline Float32 BitmapFontAsset::GetPointSize() const noexcept
    {
        return pointSize;
    }

    inline void BitmapFontAsset::SetPointSize(Float32 value) noexcept
    {
        pointSize = value;
    }

    inline Float32 BitmapFontAsset::GetFaceScale() const noexcept
    {
        return faceScale;
    }

    inline void BitmapFontAsset::SetFaceScale(Float32 value) noexcept
    {
        faceScale = value;
    }

    inline Float32 BitmapFontAsset::GetLineHeight() const noexcept
    {
        return lineHeight;
    }

    inline void BitmapFontAsset::SetLineHeight(Float32 value) noexcept
    {
        lineHeight = value;
    }

    inline Float32 BitmapFontAsset::GetAscentLine() const noexcept
    {
        return ascentLine;
    }

    inline void BitmapFontAsset::SetAscentLine(Float32 value) noexcept
    {
        ascentLine = value;
    }

    inline Float32 BitmapFontAsset::GetDescentLine() const noexcept
    {
        return descentLine;
    }

    inline void BitmapFontAsset::SetDescentLine(Float32 value) noexcept
    {
        descentLine = value;
    }

    inline Int32 BitmapFontAsset::GetAtlasPadding() const noexcept
    {
        return atlasPadding;
    }

    inline void BitmapFontAsset::SetAtlasPadding(Int32 value) noexcept
    {
        atlasPadding = value;
    }

    inline Span<const FontAtlasData> BitmapFontAsset::GetAtlasData() const noexcept
    {
        return atlasData;
    }

    inline void BitmapFontAsset::SetAtlasData(Vector<FontAtlasData>&& value) noexcept
    {
        atlasData = std::move(value);
    }

    inline Span<const FontCharacterData> BitmapFontAsset::GetCharacters() const noexcept
    {
        return characters;
    }

    inline void BitmapFontAsset::SetCharacters(Vector<FontCharacterData>&& value) noexcept
    {
        characters = std::move(value);
    }
} // namespace Garage

#endif // GARAGE_RESOURCES_BITMAP_FONT_ASSET_HPP
