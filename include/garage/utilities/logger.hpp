#ifndef GARAGE_UTILITIES_LOGGER_HPP
#define GARAGE_UTILITIES_LOGGER_HPP

#include <cstdint>
#include <source_location>
#include <string_view>
#include <type_traits>
#include <utility>

#include <fmt/format.h>
#include <fmt/xchar.h>

#if !defined(GARAGE_API)
#    if defined(GARAGE_DYNAMIC)
#        if defined(_WIN32)
#            if defined(Garage_EXPORTS)
#                define GARAGE_API __declspec(dllexport)
#            else
#                define GARAGE_API __declspec(dllimport)
#            endif
#        elif defined(__GNUC__) || defined(__clang__)
#            define GARAGE_API __attribute__((visibility("default")))
#        else
#            define GARAGE_API
#        endif
#    else
#        define GARAGE_API
#    endif
#endif

namespace Garage::Logger
{
    enum class ECategory : std::uint8_t
    {
        TRACE,
        INFO,
        WARNING,
        ERROR_LEVEL,
        CRITICAL,
    };

    template <class TChar, class... TArgs>
    struct Format final
    {
        using StringView = std::basic_string_view<TChar>;
        using FormatStr  = fmt::basic_format_string<TChar, TArgs...>;

        template <std::convertible_to<StringView> TText>
        consteval Format(const TText& text, std::source_location loc = std::source_location::current()) noexcept
            : text(text)
            , location(loc)
        {
        }

        FormatStr            text;
        std::source_location location;
    };

    template <class... TArgs>
    using FormatAscii = Format<char, std::type_identity_t<TArgs>...>;

    template <class... TArgs>
    using FormatWide = Format<wchar_t, std::type_identity_t<TArgs>...>;

    GARAGE_API void Log(ECategory                   level,
                        std::string_view            message,
                        const std::source_location& location = std::source_location::current());
    GARAGE_API void Log(ECategory                   level,
                        std::wstring_view           message,
                        const std::source_location& location = std::source_location::current());

    template <class... TArgs>
    void LogTrace(FormatAscii<TArgs...> fmt, TArgs&&... args)
    {
        Log(ECategory::TRACE, fmt::format(fmt.text, std::forward<TArgs>(args)...), fmt.location);
    }

    template <class... TArgs>
    void LogTrace(FormatWide<TArgs...> fmt, TArgs&&... args)
    {
        Log(ECategory::TRACE, fmt::format(fmt.text, std::forward<TArgs>(args)...), fmt.location);
    }

    template <class... TArgs>
    void LogInfo(FormatAscii<TArgs...> fmt, TArgs&&... args)
    {
        Log(ECategory::INFO, fmt::format(fmt.text, std::forward<TArgs>(args)...), fmt.location);
    }

    template <class... TArgs>
    void LogInfo(FormatWide<TArgs...> fmt, TArgs&&... args)
    {
        Log(ECategory::INFO, fmt::format(fmt.text, std::forward<TArgs>(args)...), fmt.location);
    }

    template <class... TArgs>
    void LogWarning(FormatAscii<TArgs...> fmt, TArgs&&... args)
    {
        Log(ECategory::WARNING, fmt::format(fmt.text, std::forward<TArgs>(args)...), fmt.location);
    }

    template <class... TArgs>
    void LogWarning(FormatWide<TArgs...> fmt, TArgs&&... args)
    {
        Log(ECategory::WARNING, fmt::format(fmt.text, std::forward<TArgs>(args)...), fmt.location);
    }

    template <class... TArgs>
    void LogError(FormatAscii<TArgs...> fmt, TArgs&&... args)
    {
        Log(ECategory::ERROR_LEVEL, fmt::format(fmt.text, std::forward<TArgs>(args)...), fmt.location);
    }

    template <class... TArgs>
    void LogError(FormatWide<TArgs...> fmt, TArgs&&... args)
    {
        Log(ECategory::ERROR_LEVEL, fmt::format(fmt.text, std::forward<TArgs>(args)...), fmt.location);
    }

    template <class... TArgs>
    void LogCritical(FormatAscii<TArgs...> fmt, TArgs&&... args)
    {
        Log(ECategory::CRITICAL, fmt::format(fmt.text, std::forward<TArgs>(args)...), fmt.location);
    }

    template <class... TArgs>
    void LogCritical(FormatWide<TArgs...> fmt, TArgs&&... args)
    {
        Log(ECategory::CRITICAL, fmt::format(fmt.text, std::forward<TArgs>(args)...), fmt.location);
    }
} // namespace Garage::Logger

#if defined(GARAGE_DEBUG) || defined(_DEBUG)
#    define LOG_TRACE(...) Garage::Logger::LogTrace(__VA_ARGS__)
#    define LOG_INFO(...) Garage::Logger::LogInfo(__VA_ARGS__)
#    define LOG_WARNING(...) Garage::Logger::LogWarning(__VA_ARGS__)
#    define LOG_ERROR(...) Garage::Logger::LogError(__VA_ARGS__)
#    define LOG_CRITICAL(...) Garage::Logger::LogCritical(__VA_ARGS__)
#else
#    define LOG_TRACE(...) ((void)0)
#    define LOG_INFO(...) ((void)0)
#    define LOG_WARNING(...) ((void)0)
#    define LOG_ERROR(...) ((void)0)
#    define LOG_CRITICAL(...) ((void)0)
#endif

#endif // GARAGE_UTILITIES_LOGGER_HPP
