cmake_minimum_required(VERSION 3.19)

project(garage
    LANGUAGES C CXX
    VERSION   1.0.0
)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_compile_definitions(GARAGE_PLATFORM_WINDOWS)
    message(STATUS "Garage in the Windows")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_compile_definitions(GARAGE_PLATFORM_LINUX)
    message(STATUS "Garage in the Linux")
else()
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

if (GARAGE_BUILD_SHARED)
    add_library(garage SHARED)
    add_compile_definitions(GARAGE_BUILD_SHARED)
else()
    add_library(garage STATIC)
    add_compile_definitions(GARAGE_BUILD_STATIC)
endif()

if (GARAGE_PLATFORM_WINDOWS)
    target_sources(garage
        PRIVATE
            src/rendering/gl/render_device_gl.cpp
            src/rendering/gl/shader_gl.cpp
            src/rendering/renderer.cpp
            src/application.cpp
    )
else()
    target_sources(garage
        PRIVATE
            src/rendering/gl/render_device_gl.cpp
            src/rendering/gl/shader_gl.cpp
            src/rendering/renderer.cpp
            src/application.cpp
    )
endif()

target_include_directories(garage
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include
)

add_subdirectory(external/glfw)
target_link_libraries(garage PRIVATE glfw)

target_include_directories(garage PRIVATE external/glad/include)
target_sources(garage
    PUBLIC
        external/glad/include/glad/glad.h
    PRIVATE
        external/glad/src/glad.c
)

set_target_properties(garage
    PROPERTIES
        OUTPUT_NAME "libgarage"
        DEBUG_POSTFIX "d"
)