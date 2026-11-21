#ifndef GARAGE_CONTENT_BITMAP_FONT_CONTENT_HPP
#define GARAGE_CONTENT_BITMAP_FONT_CONTENT_HPP

#include <filesystem>
#include <utility>

#include "../core/common.hpp"
#include "../core/common/containers.hpp"

#include "content.hpp"

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

    class GARAGE_API BitmapFontContent final : public ContentBase
    {
    public:
        explicit BitmapFontContent(std::filesystem::path path) noexcept;

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

    inline BitmapFontContent::BitmapFontContent(std::filesystem::path path) noexcept
        : ContentBase(std::move(path))
    {
    }

    inline const String& BitmapFontContent::GetName() const noexcept
    {
        return name;
    }

    inline void BitmapFontContent::SetName(String&& value) noexcept
    {
        name = std::move(value);
    }

    inline const String& BitmapFontContent::GetRenderMode() const noexcept
    {
        return renderMode;
    }

    inline void BitmapFontContent::SetRenderMode(String&& value) noexcept
    {
        renderMode = std::move(value);
    }

    inline bool BitmapFontContent::IsDistanceField() const noexcept
    {
        return isDistanceField;
    }

    inline void BitmapFontContent::SetDistanceField(bool value) noexcept
    {
        isDistanceField = value;
    }

    inline Float32 BitmapFontContent::GetPointSize() const noexcept
    {
        return pointSize;
    }

    inline void BitmapFontContent::SetPointSize(Float32 value) noexcept
    {
        pointSize = value;
    }

    inline Float32 BitmapFontContent::GetFaceScale() const noexcept
    {
        return faceScale;
    }

    inline void BitmapFontContent::SetFaceScale(Float32 value) noexcept
    {
        faceScale = value;
    }

    inline Float32 BitmapFontContent::GetLineHeight() const noexcept
    {
        return lineHeight;
    }

    inline void BitmapFontContent::SetLineHeight(Float32 value) noexcept
    {
        lineHeight = value;
    }

    inline Float32 BitmapFontContent::GetAscentLine() const noexcept
    {
        return ascentLine;
    }

    inline void BitmapFontContent::SetAscentLine(Float32 value) noexcept
    {
        ascentLine = value;
    }

    inline Float32 BitmapFontContent::GetDescentLine() const noexcept
    {
        return descentLine;
    }

    inline void BitmapFontContent::SetDescentLine(Float32 value) noexcept
    {
        descentLine = value;
    }

    inline Int32 BitmapFontContent::GetAtlasPadding() const noexcept
    {
        return atlasPadding;
    }

    inline void BitmapFontContent::SetAtlasPadding(Int32 value) noexcept
    {
        atlasPadding = value;
    }

    inline Span<const FontAtlasData> BitmapFontContent::GetAtlasData() const noexcept
    {
        return atlasData;
    }

    inline void BitmapFontContent::SetAtlasData(Vector<FontAtlasData>&& value) noexcept
    {
        atlasData = std::move(value);
    }

    inline Span<const FontCharacterData> BitmapFontContent::GetCharacters() const noexcept
    {
        return characters;
    }

    inline void BitmapFontContent::SetCharacters(Vector<FontCharacterData>&& value) noexcept
    {
        characters = std::move(value);
    }
} // namespace Garage

#endif // GARAGE_CONTENT_BITMAP_FONT_CONTENT_HPP
