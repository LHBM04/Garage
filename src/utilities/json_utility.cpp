#include "garage/utilities/json_utility.hpp"

#include <cctype>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <ios>
#include <string>
#include <string_view>
#include <utility>

#include <rapidjson/document.h>

#include "garage/core/common.hpp"
#include "garage/core/memory.hpp"

namespace Garage::JsonUtility
{
    bool HasJsonExtension(const std::filesystem::path& path) noexcept
    {
        const std::string          EXTENSION_TEXT = path.extension().string();
        const std::string_view     EXTENSION      = EXTENSION_TEXT;
        constexpr std::string_view TARGET         = ".json";
        if (EXTENSION.size() != TARGET.size())
        {
            return false;
        }

        for (USize index = 0; index < TARGET.size(); ++index)
        {
            const unsigned char LHS = static_cast<unsigned char>(EXTENSION[index]);
            const unsigned char RHS = static_cast<unsigned char>(TARGET[index]);
            if (std::tolower(LHS) != std::tolower(RHS))
            {
                return false;
            }
        }

        return true;
    }

    Result<rapidjson::Document, std::pmr::string> LoadDocument(const std::filesystem::path& path)
    {
        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file.is_open())
        {
            std::pmr::string error = Memory::MakeString("리소스를 열 수 없습니다: ");
            error.append(path.generic_string());
            return Result<rapidjson::Document, std::pmr::string>::Err(std::move(error));
        }

        const std::streamsize FILE_SIZE = file.tellg();
        if (FILE_SIZE <= 0 || static_cast<USize>(FILE_SIZE) > MAX_FILE_SIZE)
        {
            std::pmr::string error = Memory::MakeString("JSON 파일 크기가 유효하지 않습니다: ");
            error.append(path.generic_string());
            return Result<rapidjson::Document, std::pmr::string>::Err(std::move(error));
        }

        file.seekg(0, std::ios::beg);
        std::pmr::string jsonText(static_cast<USize>(FILE_SIZE), '\0', Memory::GetMemoryResource());
        if (!file.read(jsonText.data(), FILE_SIZE))
        {
            std::pmr::string error = Memory::MakeString("JSON 파일을 읽지 못했습니다: ");
            error.append(path.generic_string());
            return Result<rapidjson::Document, std::pmr::string>::Err(std::move(error));
        }

        const char* parseBegin  = jsonText.data();
        USize       parseLength = jsonText.size();
        if (parseLength >= 3 && static_cast<unsigned char>(parseBegin[0]) == 0xEF &&
            static_cast<unsigned char>(parseBegin[1]) == 0xBB && static_cast<unsigned char>(parseBegin[2]) == 0xBF)
        {
            parseBegin += 3;
            parseLength -= 3;
        }

        rapidjson::Document document;
        document.Parse(parseBegin, parseLength);
        if (document.HasParseError() || !document.IsObject())
        {
            std::pmr::string error = Memory::MakeString("JSON 구문 분석에 실패했습니다: ");
            error.append(path.generic_string());
            return Result<rapidjson::Document, std::pmr::string>::Err(std::move(error));
        }

        return Result<rapidjson::Document, std::pmr::string>::Ok(std::move(document));
    }

    const rapidjson::Value* FindMember(const rapidjson::Value& object, const char* key) noexcept
    {
        if (!object.IsObject() || key == nullptr)
        {
            return nullptr;
        }

        const auto ITERATOR = object.FindMember(key);
        if (ITERATOR == object.MemberEnd())
        {
            return nullptr;
        }

        return &ITERATOR->value;
    }

    bool TryReadString(const rapidjson::Value& value, std::pmr::string& out)
    {
        if (!value.IsString())
        {
            return false;
        }

        out.assign(value.GetString(), value.GetStringLength());
        return true;
    }

    bool TryReadBool(const rapidjson::Value& value, bool& out) noexcept
    {
        if (!value.IsBool())
        {
            return false;
        }

        out = value.GetBool();
        return true;
    }

    bool TryReadInt32(const rapidjson::Value& value, Int32& out) noexcept
    {
        if (!value.IsInt())
        {
            return false;
        }

        out = value.GetInt();
        return true;
    }

    bool TryReadUInt32(const rapidjson::Value& value, UInt32& out) noexcept
    {
        if (!value.IsUint())
        {
            return false;
        }

        out = value.GetUint();
        return true;
    }

    bool TryReadFloat32(const rapidjson::Value& value, Float32& out) noexcept
    {
        if (!value.IsNumber())
        {
            return false;
        }

        const Float32 PARSED = static_cast<Float32>(value.GetDouble());
        if (!std::isfinite(PARSED))
        {
            return false;
        }

        out = PARSED;
        return true;
    }

    bool ReadString(const rapidjson::Value& object, const char* key, std::pmr::string& out)
    {
        const rapidjson::Value* member = FindMember(object, key);
        return member != nullptr && TryReadString(*member, out);
    }

    bool ReadBool(const rapidjson::Value& object, const char* key, bool& out) noexcept
    {
        const rapidjson::Value* member = FindMember(object, key);
        return member != nullptr && TryReadBool(*member, out);
    }

    bool ReadInt32(const rapidjson::Value& object, const char* key, Int32& out) noexcept
    {
        const rapidjson::Value* member = FindMember(object, key);
        return member != nullptr && TryReadInt32(*member, out);
    }

    bool ReadUInt32(const rapidjson::Value& object, const char* key, UInt32& out) noexcept
    {
        const rapidjson::Value* member = FindMember(object, key);
        return member != nullptr && TryReadUInt32(*member, out);
    }

    bool ReadFloat32(const rapidjson::Value& object, const char* key, Float32& out) noexcept
    {
        const rapidjson::Value* member = FindMember(object, key);
        return member != nullptr && TryReadFloat32(*member, out);
    }

    bool ReadArray(const rapidjson::Value& object, const char* key, const rapidjson::Value*& outArray) noexcept
    {
        const rapidjson::Value* member = FindMember(object, key);
        if (member == nullptr || !member->IsArray() || member->Size() > MAX_ELEMENTS)
        {
            return false;
        }

        outArray = member;
        return true;
    }

    bool ReadOptionalString(const rapidjson::Value& object, const char* key, std::pmr::string& out)
    {
        const rapidjson::Value* member = FindMember(object, key);
        if (member == nullptr)
        {
            return true;
        }

        return TryReadString(*member, out);
    }

    bool ReadOptionalUInt32(const rapidjson::Value& object, const char* key, UInt32& out) noexcept
    {
        const rapidjson::Value* member = FindMember(object, key);
        if (member == nullptr)
        {
            return true;
        }

        return TryReadUInt32(*member, out);
    }

    bool ReadOptionalFloat32(const rapidjson::Value& object, const char* key, Float32& out) noexcept
    {
        const rapidjson::Value* member = FindMember(object, key);
        if (member == nullptr)
        {
            return true;
        }

        return TryReadFloat32(*member, out);
    }

    bool ReadOptionalArray(const rapidjson::Value& object, const char* key, const rapidjson::Value*& outArray) noexcept
    {
        outArray                       = nullptr;
        const rapidjson::Value* member = FindMember(object, key);
        if (member == nullptr)
        {
            return true;
        }

        if (!member->IsArray() || member->Size() > MAX_ELEMENTS)
        {
            return false;
        }

        outArray = member;
        return true;
    }
} // namespace Garage::JsonUtility
