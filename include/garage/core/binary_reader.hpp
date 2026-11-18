#ifndef GARAGE_CORE_BINARY_READER_HPP
#define GARAGE_CORE_BINARY_READER_HPP

#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <concepts>
#include <fstream>
#include <ios>
#include <ranges>
#include <string>
#include <string_view>

#include "defines.hpp"
#include "types.hpp"

namespace Garage
{
    class GARAGE_API BinaryReader final
    {
    public:
        static constexpr USize  MAX_BYTES    = 512 * 1024 * 1024;
        static constexpr UInt32 MAX_ELEMENTS = 16 * 1024 * 1024;
        static constexpr UInt32 MAX_DEPTH    = 128;

        explicit BinaryReader(std::ifstream& file) noexcept;
        ~BinaryReader() noexcept = default;

        BinaryReader(const BinaryReader&)            = delete;
        BinaryReader& operator=(const BinaryReader&) = delete;

        BinaryReader(BinaryReader&&)            = delete;
        BinaryReader& operator=(BinaryReader&&) = delete;

        template <class T>
            requires((std::integral<T> && !std::same_as<T, bool>) || std::floating_point<T>)
        [[nodiscard]] bool Read(T& value);

        [[nodiscard]] bool Read(bool& value);
        [[nodiscard]] bool Read(std::pmr::string& value);

        template <class T, USize N>
        [[nodiscard]] bool Read(std::array<T, N>& values);

        template <class T, class... TRest>
        [[nodiscard]] bool Read(T& first, TRest&... rest);

        [[nodiscard]] bool ReadCount(Int32& count, USize elementSize);

        [[nodiscard]] bool IsAtEnd() const noexcept;

    private:
        [[nodiscard]] bool        ReadBytes(char* destination, USize size);
        [[nodiscard]] static bool IsValidText(std::string_view text) noexcept;

        std::ifstream& file;
        USize          remainingBytes   = 0;
        USize          allocationBudget = MAX_BYTES;
    };

    inline BinaryReader::BinaryReader(std::ifstream& file) noexcept
        : file(file)
    {
        if (!file)
        {
            return;
        }

        file.seekg(0, std::ios::end);
        const std::streamoff SIZE = file.tellg();
        if (SIZE <= 0 || SIZE > static_cast<std::streamoff>(MAX_BYTES))
        {
            file.setstate(std::ios::failbit);
            return;
        }

        remainingBytes = static_cast<USize>(SIZE);
        file.seekg(0);
    }

    template <class T>
        requires((std::integral<T> && !std::same_as<T, bool>) || std::floating_point<T>)
    bool BinaryReader::Read(T& value)
    {
        std::array<char, sizeof(T)> encoded{};
        if (!ReadBytes(encoded.data(), encoded.size()))
        {
            return false;
        }

        if constexpr (std::endian::native == std::endian::big)
        {
            std::ranges::reverse(encoded);
        }

        value = std::bit_cast<T>(encoded);
        if constexpr (std::floating_point<T>)
        {
            return std::isfinite(value);
        }

        return true;
    }

    inline bool BinaryReader::Read(bool& value)
    {
        UInt8 encoded = 0;
        if (!Read(encoded) || encoded > 1)
        {
            return false;
        }

        value = (encoded != 0);
        return true;
    }

    inline bool BinaryReader::Read(std::pmr::string& value)
    {
        UInt32 length   = 0;
        bool   complete = false;
        for (UInt32 i = 0; i < 5; ++i)
        {
            UInt8 encoded = 0;
            if (!Read(encoded) || (i == 4 && encoded > 7))
            {
                return false;
            }

            length |= static_cast<UInt32>(encoded & 0x7Fu) << (i * 7u);
            if ((encoded & 0x80u) == 0u)
            {
                complete = true;
                break;
            }
        }

        if (!complete || length > MAX_ELEMENTS || length > remainingBytes || length > allocationBudget)
        {
            return false;
        }

        allocationBudget -= length;
        value.resize(length);

        return ReadBytes(value.data(), length) && IsValidText(value);
    }

    template <class T, USize N>
    bool BinaryReader::Read(std::array<T, N>& values)
    {
        for (T& value : values)
        {
            if (!Read(value))
            {
                return false;
            }
        }

        return true;
    }

    template <class T, class... TRest>
    bool BinaryReader::Read(T& first, TRest&... rest)
    {
        return Read(first) && (Read(rest) && ...);
    }

    inline bool BinaryReader::ReadCount(Int32& count, USize elementSize)
    {
        if (!Read(count) || count < 0 || static_cast<UInt32>(count) > MAX_ELEMENTS ||
            static_cast<USize>(count) > remainingBytes || elementSize == 0 ||
            static_cast<USize>(count) > allocationBudget / elementSize)
        {
            return false;
        }

        allocationBudget -= static_cast<USize>(count) * elementSize;
        return true;
    }

    inline bool BinaryReader::IsAtEnd() const noexcept
    {
        return file.good() && remainingBytes == 0;
    }

    inline bool BinaryReader::ReadBytes(char* destination, USize size)
    {
        if (!file || size > remainingBytes || !file.read(destination, static_cast<std::streamsize>(size)))
        {
            return false;
        }

        remainingBytes -= size;
        return true;
    }

    inline bool BinaryReader::IsValidText(std::string_view text) noexcept
    {
        for (USize i = 0; i < text.size();)
        {
            const UInt8 FIRST = static_cast<UInt8>(text[i++]);
            if (FIRST == 0)
            {
                return false;
            }

            if (FIRST < 0x80)
            {
                continue;
            }

            UInt32 codepoint         = 0;
            UInt32 minimum           = 0;
            USize  continuationCount = 0;
            if (FIRST >= 0xC2 && FIRST <= 0xDF)
            {
                codepoint         = FIRST & 0x1Fu;
                minimum           = 0x80u;
                continuationCount = 1;
            }
            else if (FIRST >= 0xE0 && FIRST <= 0xEF)
            {
                codepoint         = FIRST & 0x0Fu;
                minimum           = 0x800u;
                continuationCount = 2;
            }
            else if (FIRST >= 0xF0 && FIRST <= 0xF4)
            {
                codepoint         = FIRST & 0x07u;
                minimum           = 0x10000u;
                continuationCount = 3;
            }
            else
            {
                return false;
            }

            if (continuationCount > text.size() - i)
            {
                return false;
            }

            for (USize j = 0; j < continuationCount; ++j)
            {
                const UInt8 NEXT = static_cast<UInt8>(text[i++]);
                if ((NEXT & 0xC0u) != 0x80u)
                {
                    return false;
                }

                codepoint = (codepoint << 6u) | (NEXT & 0x3Fu);
            }

            if (codepoint < minimum || codepoint > 0x10FFFFu || (codepoint >= 0xD800u && codepoint <= 0xDFFFu))
            {
                return false;
            }
        }

        return true;
    }
} // namespace Garage

#endif // GARAGE_CORE_BINARY_READER_HPP
