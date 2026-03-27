/**
 * @file defines.hpp
 *
 * @brief 
 */
#ifndef __GARAGE_DEFINES_HPP__
#define __GARAGE_DEFINES_HPP__

#if !defined(GARAGE_PLATFORM_WINDOWS) && !defined(GARAGE_PLATFORM_LINUX) && !defined(GARAGE_PLATFORM_MACOS)
    #if defined(_WIN32) || defined(_WIN64)
        #define GARAGE_PLATFORM_WINDOWS 1
    #elif defined(__linux__)
        #define GARAGE_PLATFORM_LINUX 1
    #elif defined(__APPLE__) || defined(__MACH__)
        #define GARAGE_PLATFORM_MACOS 1
    #else
        #error "Unknown platform."
#endif

#if defined(GARAGE_BUILD_SHARED)
    #if defined(GARAGE_COMPILER_MSVC)
        #define GARAGE_API __declspec(dllexport)
    #elif defined(GARAGE_COMPILER_GCC) || defined(GARAGE_COMPILER_CLANG)
        #define GARAGE_API __attribute__((visibility("default")))
    #endif
#else
    #define GARAGE_API
#endif

#endif