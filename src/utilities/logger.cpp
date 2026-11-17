#include "garage/utilities/logger.hpp"

#include <cstddef>
#include <memory>
#include <source_location>
#include <string>
#include <string_view>
#include <vector>

#if !defined(_WIN32)
#    include <cstdint>
#endif

#include <spdlog/logger.h>
#include <spdlog/common.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#if defined(_WIN32)
#    ifndef WIN32_LEAN_AND_MEAN
#        define WIN32_LEAN_AND_MEAN
#    endif
#    ifndef NOMINMAX
#        define NOMINMAX
#    endif
#    include <Windows.h>
#    include <stringapiset.h>
#    include <winnls.h>
#    include <spdlog/sinks/msvc_sink.h>
#endif

namespace Garage::Logger
{
    namespace
    {
        [[nodiscard]] constexpr spdlog::level::level_enum ToSpdlogLevel(ECategory level) noexcept
        {
            switch (level)
            {
                case ECategory::TRACE:
                    return spdlog::level::trace;
                case ECategory::INFO:
                    return spdlog::level::info;
                case ECategory::WARNING:
                    return spdlog::level::warn;
                case ECategory::ERROR_LEVEL:
                    return spdlog::level::err;
                case ECategory::CRITICAL:
                    return spdlog::level::critical;
                default:
                    return spdlog::level::info;
            }
        }

        [[nodiscard]] std::shared_ptr<spdlog::logger> CreateLogger()
        {
            std::vector<spdlog::sink_ptr> sinks = {};
            sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
#if defined(_WIN32)
            sinks.push_back(std::make_shared<spdlog::sinks::msvc_sink_mt>());
#endif

            auto logger = std::make_shared<spdlog::logger>("Garage", sinks.begin(), sinks.end());
            logger->set_level(spdlog::level::trace);
            logger->set_pattern("[%T] [%^%l%$] %v (%s:%#)");
            return logger;
        }

        [[nodiscard]] spdlog::logger& GetLogger()
        {
            static std::shared_ptr<spdlog::logger> const LOGGER = CreateLogger();
            return *LOGGER;
        }

        [[nodiscard]] std::string WideToUtf8(std::wstring_view wide)
        {
            if (wide.empty())
            {
                return {};
            }

#if defined(_WIN32)
            const int SIZE = static_cast<int>(wide.size());
            const int LENGTH =
                    WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wide.data(), SIZE, nullptr, 0, nullptr, nullptr);
            if (LENGTH <= 0)
            {
                return {};
            }

            std::string utf8(static_cast<std::size_t>(LENGTH), '\0');
            WideCharToMultiByte(
                    CP_UTF8, WC_ERR_INVALID_CHARS, wide.data(), SIZE, utf8.data(), LENGTH, nullptr, nullptr);
            return utf8;
#else
            std::string utf8 = {};
            utf8.reserve(wide.size());
            for (const wchar_t WC : wide)
            {
                const auto CODEPOINT = static_cast<std::uint32_t>(WC);
                if (CODEPOINT <= 0x7FU)
                {
                    utf8.push_back(static_cast<char>(CODEPOINT));
                }
                else if (CODEPOINT <= 0x7FFU)
                {
                    utf8.push_back(static_cast<char>(0xC0U | ((CODEPOINT >> 6U) & 0x1FU)));
                    utf8.push_back(static_cast<char>(0x80U | (CODEPOINT & 0x3FU)));
                }
                else if (CODEPOINT <= 0xFFFFU)
                {
                    utf8.push_back(static_cast<char>(0xE0U | ((CODEPOINT >> 12U) & 0x0FU)));
                    utf8.push_back(static_cast<char>(0x80U | ((CODEPOINT >> 6U) & 0x3FU)));
                    utf8.push_back(static_cast<char>(0x80U | (CODEPOINT & 0x3FU)));
                }
                else if (CODEPOINT <= 0x10FFFFU)
                {
                    utf8.push_back(static_cast<char>(0xF0U | ((CODEPOINT >> 18U) & 0x07U)));
                    utf8.push_back(static_cast<char>(0x80U | ((CODEPOINT >> 12U) & 0x3FU)));
                    utf8.push_back(static_cast<char>(0x80U | ((CODEPOINT >> 6U) & 0x3FU)));
                    utf8.push_back(static_cast<char>(0x80U | (CODEPOINT & 0x3FU)));
                }
            }
            return utf8;
#endif
        }
    } // namespace

    void Log(ECategory level, std::string_view message, const std::source_location& location)
    {
        const spdlog::source_loc SOURCE_LOC{
                location.file_name(), static_cast<int>(location.line()), location.function_name()};
        GetLogger().log(SOURCE_LOC, ToSpdlogLevel(level), message);
    }

    void Log(ECategory level, std::wstring_view message, const std::source_location& location)
    {
        Log(level, WideToUtf8(message), location);
    }
} // namespace Garage::Logger
