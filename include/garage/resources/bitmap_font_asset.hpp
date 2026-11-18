#ifndef GARAGE_RESOURCES_BITMAP_FONT_ASSET_HPP
#define GARAGE_RESOURCES_BITMAP_FONT_ASSET_HPP

#include <array>
#include <filesystem>
#include <span>
#include <string>
#include <utility>
#include <vector>

#include "../core/allocator.hpp"
#include "../core/defines.hpp"
#include "../core/types.hpp"

#include "asset.hpp"

namespace Garage
{
    struct GARAGE_API FontAtlasData final
    {
        Int32            index = 0;
        std::pmr::string texture{Allocator::GetMemoryResource()};
        Int32            width  = 0;
        Int32            height = 0;
    };

    struct GARAGE_API FontCharacterData final
    {
        UInt32                 unicode        = 0;
        UInt32                 glyphIndex     = 0;
        Int32                  atlasIndex     = 0;
        Float32                characterScale = 0.0f;
        Float32                glyphScale     = 0.0f;
        std::array<Float32, 4> glyphRect{};
        std::array<Float32, 4> sampleRect{};
        std::array<Float32, 2> uvMin{};
        std::array<Float32, 2> uvMax{};
        Float32                width    = 0.0f;
        Float32                height   = 0.0f;
        Float32                bearingX = 0.0f;
        Float32                bearingY = 0.0f;
        Float32                advance  = 0.0f;
    };

    class GARAGE_API BitmapFontAsset final : public AssetBase
    {
    public:
        explicit BitmapFontAsset(std::filesystem::path path) noexcept;

        [[nodiscard]] const std::pmr::string& GetName() const noexcept;
        void                                  SetName(std::pmr::string&& value) noexcept;

        [[nodiscard]] const std::pmr::string& GetRenderMode() const noexcept;
        void                                  SetRenderMode(std::pmr::string&& value) noexcept;

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

        [[nodiscard]] std::span<const FontAtlasData> GetAtlasData() const noexcept;
        void                                         SetAtlasData(std::pmr::vector<FontAtlasData>&& value) noexcept;

        [[nodiscard]] std::span<const FontCharacterData> GetCharacters() const noexcept;
        void SetCharacters(std::pmr::vector<FontCharacterData>&& value) noexcept;

    private:
        std::pmr::string                    name{Allocator::GetMemoryResource()};
        std::pmr::string                    renderMode{Allocator::GetMemoryResource()};
        bool                                isDistanceField = false;
        Float32                             pointSize       = 0.0f;
        Float32                             faceScale       = 0.0f;
        Float32                             lineHeight      = 0.0f;
        Float32                             ascentLine      = 0.0f;
        Float32                             descentLine     = 0.0f;
        Int32                               atlasPadding    = 0;
        std::pmr::vector<FontAtlasData>     atlasData{Allocator::GetMemoryResource()};
        std::pmr::vector<FontCharacterData> characters{Allocator::GetMemoryResource()};
    };

    inline BitmapFontAsset::BitmapFontAsset(std::filesystem::path path) noexcept
        : AssetBase(std::move(path))
    {
    }

    inline const std::pmr::string& BitmapFontAsset::GetName() const noexcept
    {
        return name;
    }

    inline void BitmapFontAsset::SetName(std::pmr::string&& value) noexcept
    {
        name = std::move(value);
    }

    inline const std::pmr::string& BitmapFontAsset::GetRenderMode() const noexcept
    {
        return renderMode;
    }

    inline void BitmapFontAsset::SetRenderMode(std::pmr::string&& value) noexcept
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

    inline std::span<const FontAtlasData> BitmapFontAsset::GetAtlasData() const noexcept
    {
        return atlasData;
    }

    inline void BitmapFontAsset::SetAtlasData(std::pmr::vector<FontAtlasData>&& value) noexcept
    {
        atlasData = std::move(value);
    }

    inline std::span<const FontCharacterData> BitmapFontAsset::GetCharacters() const noexcept
    {
        return characters;
    }

    inline void BitmapFontAsset::SetCharacters(std::pmr::vector<FontCharacterData>&& value) noexcept
    {
        characters = std::move(value);
    }
} // namespace Garage

#endif // GARAGE_RESOURCES_BITMAP_FONT_ASSET_HPP
