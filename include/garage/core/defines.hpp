#ifndef GARAGE_CORE_DEFINES_HPP
#define GARAGE_CORE_DEFINES_HPP

#include <cstdlib>

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

#if !defined(GARAGE_DEBUG_BREAK)
#    if defined(_MSC_VER)
#        define GARAGE_DEBUG_BREAK() __debugbreak()
#    elif defined(__clang__) || defined(__GNUC__)
#        define GARAGE_DEBUG_BREAK() __builtin_trap()
#    else
#        define GARAGE_DEBUG_BREAK() std::abort()
#    endif
#endif

#ifndef ABORT
#    define ABORT()                                                                                                    \
        do                                                                                                             \
        {                                                                                                              \
            GARAGE_DEBUG_BREAK();                                                                                      \
            std::exit(EXIT_FAILURE);                                                                                   \
        }                                                                                                              \
        while (false)
#endif

#ifndef ASSERT
#    define ASSERT(condition)                                                                                          \
        do                                                                                                             \
        {                                                                                                              \
            if (!(condition))                                                                                          \
            {                                                                                                          \
                ABORT();                                                                                               \
            }                                                                                                          \
        }                                                                                                              \
        while (false)
#endif

#endif // GARAGE_CORE_DEFINES_HPP
