#include "garage/resources/bitmap_font_asset_importer.hpp"

#include <array>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <rapidjson/document.h>

#include "garage/core/allocator.hpp"
#include "garage/core/types.hpp"

#include "garage/resources/bitmap_font_asset.hpp"

#include "garage/utilities/json_utility.hpp"

namespace Garage
{
    Result<Allocator::UniquePtr<BitmapFontAsset>, std::pmr::string>
            BitmapFontAssetImporter::Import(const std::filesystem::path& path) const
    {
        if (!JsonUtility::HasJsonExtension(path))
        {
            return Result<Allocator::UniquePtr<BitmapFontAsset>, std::pmr::string>::Err(
                    Allocator::MakeString("폰트 로더에는 .json 파일이 필요합니다."));
        }

        auto documentResult = JsonUtility::LoadDocument(path);
        if (!documentResult)
        {
            return Result<Allocator::UniquePtr<BitmapFontAsset>, std::pmr::string>::Err(
                    std::move(documentResult).GetError());
        }

        const rapidjson::Value& ROOT = *documentResult;

        std::pmr::string name{Allocator::GetMemoryResource()};
        std::pmr::string renderMode{Allocator::GetMemoryResource()};
        bool             isDistanceField = false;
        Float32          pointSize       = 0.0f;
        Float32          faceScale       = 0.0f;
        Float32          lineHeight      = 0.0f;
        Float32          ascentLine      = 0.0f;
        Float32          descentLine     = 0.0f;
        Int32            atlasPadding    = 0;
        if (!JsonUtility::ReadString(ROOT, "name", name) || !JsonUtility::ReadString(ROOT, "renderMode", renderMode) ||
            !JsonUtility::ReadBool(ROOT, "isDistanceField", isDistanceField) ||
            !JsonUtility::ReadFloat32(ROOT, "pointSize", pointSize) ||
            !JsonUtility::ReadFloat32(ROOT, "faceScale", faceScale) ||
            !JsonUtility::ReadFloat32(ROOT, "lineHeight", lineHeight) ||
            !JsonUtility::ReadFloat32(ROOT, "ascentLine", ascentLine) ||
            !JsonUtility::ReadFloat32(ROOT, "descentLine", descentLine) ||
            !JsonUtility::ReadInt32(ROOT, "atlasPadding", atlasPadding))
        {
            return Result<Allocator::UniquePtr<BitmapFontAsset>, std::pmr::string>::Err(
                    Allocator::MakeString("폰트 헤더가 유효하지 않습니다."));
        }

        const rapidjson::Value* atlasesArray = nullptr;
        if (!JsonUtility::ReadArray(ROOT, "atlases", atlasesArray) || atlasesArray->Empty())
        {
            return Result<Allocator::UniquePtr<BitmapFontAsset>, std::pmr::string>::Err(
                    Allocator::MakeString("폰트 아틀라스 개수가 유효하지 않습니다."));
        }

        std::pmr::vector<FontAtlasData> parsedAtlases{Allocator::GetMemoryResource()};
        parsedAtlases.reserve(atlasesArray->Size());
        for (const rapidjson::Value& atlasValue : atlasesArray->GetArray())
        {
            FontAtlasData atlas{};
            if (!atlasValue.IsObject() || !JsonUtility::ReadInt32(atlasValue, "index", atlas.index) ||
                !JsonUtility::ReadString(atlasValue, "texture", atlas.texture) ||
                !JsonUtility::ReadInt32(atlasValue, "width", atlas.width) ||
                !JsonUtility::ReadInt32(atlasValue, "height", atlas.height))
            {
                return Result<Allocator::UniquePtr<BitmapFontAsset>, std::pmr::string>::Err(
                        Allocator::MakeString("폰트 아틀라스 데이터가 유효하지 않습니다."));
            }

            parsedAtlases.push_back(std::move(atlas));
        }

        const rapidjson::Value* charactersArray = nullptr;
        if (!JsonUtility::ReadArray(ROOT, "characters", charactersArray))
        {
            return Result<Allocator::UniquePtr<BitmapFontAsset>, std::pmr::string>::Err(
                    Allocator::MakeString("폰트 문자 개수가 유효하지 않습니다."));
        }

        std::pmr::vector<FontCharacterData> parsedCharacters{Allocator::GetMemoryResource()};
        parsedCharacters.reserve(charactersArray->Size());
        for (const rapidjson::Value& characterValue : charactersArray->GetArray())
        {
            FontCharacterData character{};
            if (!characterValue.IsObject() || !JsonUtility::ReadUInt32(characterValue, "unicode", character.unicode) ||
                !JsonUtility::ReadUInt32(characterValue, "glyphIndex", character.glyphIndex) ||
                !JsonUtility::ReadInt32(characterValue, "atlasIndex", character.atlasIndex) ||
                !JsonUtility::ReadFloat32(characterValue, "characterScale", character.characterScale) ||
                !JsonUtility::ReadFloat32(characterValue, "glyphScale", character.glyphScale) ||
                !JsonUtility::ReadFloatArray(characterValue, "glyphRect", character.glyphRect) ||
                !JsonUtility::ReadFloatArray(characterValue, "sampleRect", character.sampleRect) ||
                !JsonUtility::ReadFloatArray(characterValue, "uvMin", character.uvMin) ||
                !JsonUtility::ReadFloatArray(characterValue, "uvMax", character.uvMax) ||
                !JsonUtility::ReadFloat32(characterValue, "width", character.width) ||
                !JsonUtility::ReadFloat32(characterValue, "height", character.height) ||
                !JsonUtility::ReadFloat32(characterValue, "bearingX", character.bearingX) ||
                !JsonUtility::ReadFloat32(characterValue, "bearingY", character.bearingY) ||
                !JsonUtility::ReadFloat32(characterValue, "advance", character.advance))
            {
                return Result<Allocator::UniquePtr<BitmapFontAsset>, std::pmr::string>::Err(
                        Allocator::MakeString("폰트 문자 데이터가 유효하지 않습니다."));
            }

            parsedCharacters.push_back(character);
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
