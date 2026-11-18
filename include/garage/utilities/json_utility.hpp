#ifndef GARAGE_UTILITIES_JSON_UTILITY_HPP
#define GARAGE_UTILITIES_JSON_UTILITY_HPP

#include <array>
#include <cmath>
#include <filesystem>
#include <string>

#include <rapidjson/document.h>

#include "../core/defines.hpp"
#include "../core/types.hpp"

namespace Garage::JsonUtility
{
    inline constexpr USize MAX_FILE_SIZE = 64u * 1024u * 1024u;
    inline constexpr USize MAX_ELEMENTS  = 16777216u;
    inline constexpr USize MAX_DEPTH     = 1024u;

    [[nodiscard]] GARAGE_API bool HasJsonExtension(const std::filesystem::path& path) noexcept;

    [[nodiscard]] GARAGE_API Result<rapidjson::Document, std::pmr::string>
                             LoadDocument(const std::filesystem::path& path);

    [[nodiscard]] GARAGE_API const rapidjson::Value* FindMember(const rapidjson::Value& object,
                                                                const char*             key) noexcept;

    [[nodiscard]] GARAGE_API bool TryReadString(const rapidjson::Value& value, std::pmr::string& out);
    [[nodiscard]] GARAGE_API bool TryReadBool(const rapidjson::Value& value, bool& out) noexcept;
    [[nodiscard]] GARAGE_API bool TryReadInt32(const rapidjson::Value& value, Int32& out) noexcept;
    [[nodiscard]] GARAGE_API bool TryReadUInt32(const rapidjson::Value& value, UInt32& out) noexcept;
    [[nodiscard]] GARAGE_API bool TryReadFloat32(const rapidjson::Value& value, Float32& out) noexcept;

    template <USize N>
    [[nodiscard]] bool TryReadFloatArray(const rapidjson::Value& value, std::array<Float32, N>& out) noexcept;

    [[nodiscard]] GARAGE_API bool ReadString(const rapidjson::Value& object, const char* key, std::pmr::string& out);
    [[nodiscard]] GARAGE_API bool ReadBool(const rapidjson::Value& object, const char* key, bool& out) noexcept;
    [[nodiscard]] GARAGE_API bool ReadInt32(const rapidjson::Value& object, const char* key, Int32& out) noexcept;
    [[nodiscard]] GARAGE_API bool ReadUInt32(const rapidjson::Value& object, const char* key, UInt32& out) noexcept;
    [[nodiscard]] GARAGE_API bool ReadFloat32(const rapidjson::Value& object, const char* key, Float32& out) noexcept;
    [[nodiscard]] GARAGE_API bool
            ReadArray(const rapidjson::Value& object, const char* key, const rapidjson::Value*& outArray) noexcept;

    template <USize N>
    [[nodiscard]] bool
            ReadFloatArray(const rapidjson::Value& object, const char* key, std::array<Float32, N>& out) noexcept;

    [[nodiscard]] GARAGE_API bool
            ReadOptionalString(const rapidjson::Value& object, const char* key, std::pmr::string& out);
    [[nodiscard]] GARAGE_API bool
            ReadOptionalUInt32(const rapidjson::Value& object, const char* key, UInt32& out) noexcept;
    [[nodiscard]] GARAGE_API bool
            ReadOptionalFloat32(const rapidjson::Value& object, const char* key, Float32& out) noexcept;
    [[nodiscard]] GARAGE_API bool ReadOptionalArray(const rapidjson::Value&  object,
                                                    const char*              key,
                                                    const rapidjson::Value*& outArray) noexcept;

    template <USize N>
    [[nodiscard]] bool ReadOptionalFloatArray(const rapidjson::Value& object,
                                              const char*             key,
                                              std::array<Float32, N>& out) noexcept;

    template <USize N>
    bool TryReadFloatArray(const rapidjson::Value& value, std::array<Float32, N>& out) noexcept
    {
        using IndexType = decltype(value.Size());
        if (!value.IsArray() || value.Size() != static_cast<IndexType>(N))
        {
            return false;
        }

        std::array<Float32, N> parsed{};
        for (USize index = 0; index < N; ++index)
        {
            if (!TryReadFloat32(value[static_cast<IndexType>(index)], parsed[index]))
            {
                return false;
            }
        }

        out = parsed;
        return true;
    }

    template <USize N>
    bool ReadFloatArray(const rapidjson::Value& object, const char* key, std::array<Float32, N>& out) noexcept
    {
        const rapidjson::Value* member = FindMember(object, key);
        return member != nullptr && TryReadFloatArray(*member, out);
    }

    template <USize N>
    bool ReadOptionalFloatArray(const rapidjson::Value& object, const char* key, std::array<Float32, N>& out) noexcept
    {
        const rapidjson::Value* member = FindMember(object, key);
        if (member == nullptr)
        {
            return true;
        }

        return TryReadFloatArray(*member, out);
    }
} // namespace Garage::JsonUtility

#endif // GARAGE_UTILITIES_JSON_UTILITY_HPP
