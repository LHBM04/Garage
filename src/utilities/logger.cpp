#include "garage/utilities/logger.hpp"

#include <cstdint>
#include <memory>
#include <source_location>
#include <string>
#include <string_view>
#include <vector>

#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#if defined(_WIN32)
#    ifndef WIN32_LEAN_AND_MEAN
#        define WIN32_LEAN_AND_MEAN
#    endif
#    ifndef NOMINMAX
#        define NOMINMAX
#    endif
#    include <Windows.h>
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
            static std::shared_ptr<spdlog::logger> logger = CreateLogger();
            return *logger;
        }

        [[nodiscard]] std::string WideToUtf8(std::wstring_view wide)
        {
            if (wide.empty())
            {
                return {};
            }

#if defined(_WIN32)
            const int size = static_cast<int>(wide.size());
            const int length =
                    WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wide.data(), size, nullptr, 0, nullptr, nullptr);
            if (length <= 0)
            {
                return {};
            }

            std::string utf8(static_cast<std::size_t>(length), '\0');
            WideCharToMultiByte(
                    CP_UTF8, WC_ERR_INVALID_CHARS, wide.data(), size, utf8.data(), length, nullptr, nullptr);
            return utf8;
#else
            std::string utf8 = {};
            utf8.reserve(wide.size());
            for (const wchar_t wc : wide)
            {
                const auto codepoint = static_cast<std::uint32_t>(wc);
                if (codepoint <= 0x7FU)
                {
                    utf8.push_back(static_cast<char>(codepoint));
                }
                else if (codepoint <= 0x7FFU)
                {
                    utf8.push_back(static_cast<char>(0xC0U | ((codepoint >> 6U) & 0x1FU)));
                    utf8.push_back(static_cast<char>(0x80U | (codepoint & 0x3FU)));
                }
                else if (codepoint <= 0xFFFFU)
                {
                    utf8.push_back(static_cast<char>(0xE0U | ((codepoint >> 12U) & 0x0FU)));
                    utf8.push_back(static_cast<char>(0x80U | ((codepoint >> 6U) & 0x3FU)));
                    utf8.push_back(static_cast<char>(0x80U | (codepoint & 0x3FU)));
                }
                else if (codepoint <= 0x10FFFFU)
                {
                    utf8.push_back(static_cast<char>(0xF0U | ((codepoint >> 18U) & 0x07U)));
                    utf8.push_back(static_cast<char>(0x80U | ((codepoint >> 12U) & 0x3FU)));
                    utf8.push_back(static_cast<char>(0x80U | ((codepoint >> 6U) & 0x3FU)));
                    utf8.push_back(static_cast<char>(0x80U | (codepoint & 0x3FU)));
                }
            }
            return utf8;
#endif
        }
    } // namespace

    void Log(ECategory level, std::string_view message, const std::source_location& location)
    {
        const spdlog::source_loc sourceLoc{
                location.file_name(), static_cast<int>(location.line()), location.function_name()};
        GetLogger().log(sourceLoc, ToSpdlogLevel(level), message);
    }

    void Log(ECategory level, std::wstring_view message, const std::source_location& location)
    {
        Log(level, WideToUtf8(message), location);
    }
} // namespace Garage::Logger
