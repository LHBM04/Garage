cmake_minimum_required(VERSION 3.19)

project(garage
    LANGUAGES C CXX
    VERSION   1.0.0
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(garage STATIC)
target_sources(garage
    PRIVATE
        src/rendering/gl/render_device_gl.cpp
        src/rendering/gl/shader_gl.cpp
        src/rendering/renderer.cpp
        src/application.cpp
)
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