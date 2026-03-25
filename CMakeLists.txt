cmake_minimum_required(VERSION 3.19)
project(garage
    LANGUAGES C CXX
    VERSION   1.0.0
)

if (GARAGE_BUILD_SHARED)
    add_library(garage SHARED)
    target_compile_definitions(garage PRIVATE GARAGE_BUILD_SHARED)
else()
    add_library(garage STATIC)
    target_compile_definitions(garage PRIVATE GARAGE_BUILD_STATIC)
endif()

if (GARAGE_GRAPHICS_GL)
    target_compile_definitions(garage PRIVATE GARAGE_GRAPHICS_GL)
endif()

target_sources(garage
    PUBLIC
        src/dx12/renderer_dx12.cpp
        src/gl/renderer_gl.cpp
        src/application.cpp
    PRIVATE
        include/garage/dx12/renderer_dx12.hpp
        include/garage/gl/renderer_gl.hpp
        include/garage/application.hpp
        include/garage/renderer.hpp
)

target_include_directories(garage
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/garage>
)

find_package(glad CONFIG REQUIRED)
target_link_libraries(garage PRIVATE glad::glad)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(garage PRIVATE glfw)

set_target_properties(garage
    PROPERTIES
        OUTPUT_NAME "libgarage"
        DEBUG_POSTFIX "d"
)