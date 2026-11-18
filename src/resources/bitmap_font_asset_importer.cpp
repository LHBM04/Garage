#include "garage/resources/bitmap_font_asset_importer.hpp"

#include <filesystem>
#include <fstream>
#include <ios>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "garage/core/allocator.hpp"
#include "garage/core/binary_reader.hpp"
#include "garage/core/types.hpp"

#include "garage/resources/bitmap_font_asset.hpp"

namespace Garage
{
    Result<Allocator::UniquePtr<BitmapFontAsset>, std::pmr::string>
            BitmapFontAssetImporter::Import(const std::filesystem::path& path) const
    {
        if (path.extension() != ".tukfont")
        {
            return Result<Allocator::UniquePtr<BitmapFontAsset>, std::pmr::string>::Err(
                    Allocator::MakeString("폰트 로더에는 .tukfont 파일이 필요합니다."));
        }

        std::ifstream file(path, std::ios::binary);
        if (!file)
        {
            std::pmr::string error = Allocator::MakeString("리소스를 열 수 없습니다: ");
            error.append(path.generic_string());
            return Result<Allocator::UniquePtr<BitmapFontAsset>, std::pmr::string>::Err(std::move(error));
        }

        BinaryReader     reader(file);
        std::pmr::string name{Allocator::GetMemoryResource()};
        std::pmr::string renderMode{Allocator::GetMemoryResource()};
        bool             isDistanceField = false;
        Float32          pointSize       = 0.0f;
        Float32          faceScale       = 0.0f;
        Float32          lineHeight      = 0.0f;
        Float32          ascentLine      = 0.0f;
        Float32          descentLine     = 0.0f;
        Int32            atlasPadding    = 0;
        if (!reader.Read(name,
                         renderMode,
                         isDistanceField,
                         pointSize,
                         faceScale,
                         lineHeight,
                         ascentLine,
                         descentLine,
                         atlasPadding))
        {
            return Result<Allocator::UniquePtr<BitmapFontAsset>, std::pmr::string>::Err(
                    Allocator::MakeString("폰트 헤더가 유효하지 않습니다."));
        }

        Int32 count = 0;
        if (!reader.ReadCount(count, sizeof(FontAtlasData)))
        {
            return Result<Allocator::UniquePtr<BitmapFontAsset>, std::pmr::string>::Err(
                    Allocator::MakeString("폰트 아틀라스 개수가 유효하지 않습니다."));
        }

        std::pmr::vector<FontAtlasData> parsedAtlases(static_cast<USize>(count), Allocator::GetMemoryResource());
        for (FontAtlasData& atlas : parsedAtlases)
        {
            if (!reader.Read(atlas.index, atlas.texture, atlas.width, atlas.height))
            {
                return Result<Allocator::UniquePtr<BitmapFontAsset>, std::pmr::string>::Err(
                        Allocator::MakeString("폰트 아틀라스 데이터가 유효하지 않습니다."));
            }
        }

        if (!reader.ReadCount(count, sizeof(FontCharacterData)))
        {
            return Result<Allocator::UniquePtr<BitmapFontAsset>, std::pmr::string>::Err(
                    Allocator::MakeString("폰트 문자 개수가 유효하지 않습니다."));
        }

        std::pmr::vector<FontCharacterData> parsedCharacters(static_cast<USize>(count), Allocator::GetMemoryResource());
        for (FontCharacterData& character : parsedCharacters)
        {
            if (!reader.Read(character.unicode,
                             character.glyphIndex,
                             character.atlasIndex,
                             character.characterScale,
                             character.glyphScale,
                             character.glyphRect,
                             character.sampleRect,
                             character.uvMin,
                             character.uvMax,
                             character.width,
                             character.height,
                             character.bearingX,
                             character.bearingY,
                             character.advance))
            {
                return Result<Allocator::UniquePtr<BitmapFontAsset>, std::pmr::string>::Err(
                        Allocator::MakeString("폰트 문자 데이터가 유효하지 않습니다."));
            }
        }

        if (!reader.IsAtEnd())
        {
            return Result<Allocator::UniquePtr<BitmapFontAsset>, std::pmr::string>::Err(
                    Allocator::MakeString("폰트 파일 끝에 예상치 못한 데이터가 남아 있습니다."));
        }

        if (pointSize <= 0.0f || faceScale <= 0.0f || atlasPadding < 0 || parsedAtlases.empty())
        {
            return Result<Allocator::UniquePtr<BitmapFontAsset>, std::pmr::string>::Err(
                    Allocator::MakeString("폰트 메트릭이 유효하지 않거나 아틀라스가 없습니다."));
        }

        std::pmr::unordered_map<Int32, const FontAtlasData*> atlases{Allocator::GetMemoryResource()};
        for (const FontAtlasData& atlas : parsedAtlases)
        {
            if (atlas.index < 0 || atlas.width <= 0 || atlas.height <= 0 ||
                !atlases.emplace(atlas.index, &atlas).second)
            {
                return Result<Allocator::UniquePtr<BitmapFontAsset>, std::pmr::string>::Err(
                        Allocator::MakeString("폰트 아틀라스가 유효하지 않거나 중복되었습니다."));
            }
        }

        std::pmr::unordered_set<UInt32> characters{Allocator::GetMemoryResource()};
        for (const FontCharacterData& character : parsedCharacters)
        {
            const auto PAGE = atlases.find(character.atlasIndex);
            if (PAGE == atlases.end() || character.unicode > 0x10FFFFu ||
                (character.unicode >= 0xD800u && character.unicode <= 0xDFFFu) ||
                !characters.insert(character.unicode).second)
            {
                return Result<Allocator::UniquePtr<BitmapFontAsset>, std::pmr::string>::Err(
                        Allocator::MakeString("폰트 문자 매핑이 유효하지 않습니다."));
            }

            const Float32 ATLAS_WIDTH  = static_cast<Float32>(PAGE->second->width);
            const Float32 ATLAS_HEIGHT = static_cast<Float32>(PAGE->second->height);
            for (const auto& rect : {character.glyphRect, character.sampleRect})
            {
                if (rect[0] < 0.0f || rect[1] < 0.0f || rect[2] < 0.0f || rect[3] < 0.0f ||
                    rect[0] + rect[2] > ATLAS_WIDTH || rect[1] + rect[3] > ATLAS_HEIGHT)
                {
                    return Result<Allocator::UniquePtr<BitmapFontAsset>, std::pmr::string>::Err(
                            Allocator::MakeString("폰트 글리프 영역이 아틀라스 범위를 벗어났습니다."));
                }
            }
        }

        Allocator::UniquePtr<BitmapFontAsset> font = Allocator::MakeUnique<BitmapFontAsset>(path);
        font->SetName(std::move(name));
        font->SetRenderMode(std::move(renderMode));
        font->SetDistanceField(isDistanceField);
        font->SetPointSize(pointSize);
        font->SetFaceScale(faceScale);
        font->SetLineHeight(lineHeight);
        font->SetAscentLine(ascentLine);
        font->SetDescentLine(descentLine);
        font->SetAtlasPadding(atlasPadding);
        font->SetAtlasData(std::move(parsedAtlases));
        font->SetCharacters(std::move(parsedCharacters));
        return Result<Allocator::UniquePtr<BitmapFontAsset>, std::pmr::string>::Ok(std::move(font));
    }
} // namespace Garage
