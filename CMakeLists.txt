cmake_minimum_required(VERSION 3.19)

if (NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

project(garage
    LANGUAGES C CXX
    VERSION   1.0.0
)

if (GARAGE_PLATFORM_WINDOWS AND GARAGE_BUILD_SHARED)
    add_library(garage SHARED)
    target_compile_definitions(garage PRIVATE GARAGE_BUILD_SHARED)
else()
    add_library(garage STATIC)
    target_compile_definitions(garage PRIVATE GARAGE_BUILD_STATIC)
endif()

target_include_directories(garage PUBLIC 
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
)

target_sources(garage
    PUBLIC
        include/garage/application.hpp
        include/garage/renderer.hpp
    PRIVATE
        src/application.cpp
)

find_package(assimp CONFIG REQUIRED)
target_link_libraries(garage PRIVATE assimp::assimp)

find_package(fmt CONFIG REQUIRED)
target_link_libraries(garage PRIVATE fmt::fmt-header-only)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(garage PRIVATE glfw)

find_package(imgui CONFIG REQUIRED)
target_link_libraries(garage PRIVATE imgui::imgui)

find_package(imguizmo CONFIG REQUIRED)
target_link_libraries(garage PRIVATE imguizmo::imguizmo)

find_package(implot CONFIG REQUIRED)
target_link_libraries(garage PRIVATE implot::implot)

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(garage PRIVATE spdlog::spdlog_header_only)

if (GARAGE_GRAPHICS_GL)
    target_compile_definitions(garage PRIVATE GARAGE_GRAPHICS_GL)

    target_sources(garage
        PUBLIC
            include/garage/gl/renderer_gl.hpp
        PRIVATE
            src/gl/renderer_gl.cpp
    )

    find_package(glad CONFIG REQUIRED)
    target_link_libraries(garage PRIVATE glad::glad)

    find_package(glm CONFIG REQUIRED)
    target_link_libraries(garage PRIVATE glm::glm-header-only)
elseif (GARAGE_GRAPHICS_DX12)
    target_compile_definitions(garage PRIVATE GARAGE_GRAPHICS_DX12)

    target_sources(garage
        PUBLIC
            include/garage/dx12/renderer_dx12.hpp
        PRIVATE
            src/dx12/renderer_dx12.cpp
    )
endif()

if (GARAGE_AUDIO_AL)
    target_compile_definitions(garage PRIVATE GARAGE_AUDIO_AL)
    
    find_package(OpenAL CONFIG REQUIRED)
    target_link_libraries(garage PRIVATE OpenAL::OpenAL)
elseif (GARAGE_AUDIO_XAUDIO)
    target_compile_definitions(garage PRIVATE GARAGE_AUDIO_XAUDIO)

    find_file(OpenXAudio2_INCLUDE_DIR xaudio2.h)
    target_link_directories(garage PRIVATE ${OpenXAudio2_INCLUDE_DIR})
endif()

if (GARAGE_INPUT_GAMEINPUT)
    target_compile_definitions(garage PRIVATE GARAGE_AUDIO_AL)
    
    find_package(OpenAL CONFIG REQUIRED)
    target_link_libraries(garage PRIVATE OpenAL::OpenAL)
endif()

set_target_properties(garage
    PROPERTIES
        OUTPUT_NAME "libgarage"
        DEBUG_POSTFIX "d"
)