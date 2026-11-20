cmake_minimum_required(VERSION 3.22)

project(Garage VERSION 0.1.0 LANGUAGES CXX)

include(cmake/clangd.cmake)

set(GARAGE_DEBUG FALSE)
set(GARAGE_RELEASE FALSE)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(GARAGE_DEBUG TRUE)
elseif(CMAKE_BUILD_TYPE)
    set(GARAGE_RELEASE TRUE)
endif()

set(GARAGE_STATIC FALSE)
set(GARAGE_DYNAMIC FALSE)
if(BUILD_SHARED_LIBS)
    set(GARAGE_DYNAMIC TRUE)
    add_compile_definitions(GARAGE_DYNAMIC)
    message(STATUS "[Garage] Linkage: Dynamic (BUILD_SHARED_LIBS=ON)")
else()
    set(GARAGE_STATIC TRUE)
    add_compile_definitions(GARAGE_STATIC)
    message(STATUS "[Garage] Linkage: Static (BUILD_SHARED_LIBS=OFF)")
endif()

add_compile_definitions(
    $<$<CONFIG:Debug>:GARAGE_DEBUG>
    $<$<NOT:$<CONFIG:Debug>>:GARAGE_RELEASE>
)

if(CMAKE_BUILD_TYPE)
    message(STATUS "[Garage] Build Type: ${CMAKE_BUILD_TYPE}")
endif()

include(cmake/platform-detection.cmake)
include(cmake/architecture_detection.cmake)
include(cmake/compiler-detection.cmake)
option(GARAGE_ENABLE_TRACY "Enable the Tracy profiler client" OFF)
include(cmake/dependencies.cmake)

file(GLOB_RECURSE GARAGE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)
file(GLOB_RECURSE GARAGE_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
)

if(GARAGE_SOURCES)
    add_library(Garage ${GARAGE_SOURCES} ${GARAGE_HEADERS})
    add_library(Garage::Garage ALIAS Garage)
    add_dependencies(Garage garage-clangd)
    target_compile_definitions(Garage PUBLIC
        $<IF:$<BOOL:${BUILD_SHARED_LIBS}>,GARAGE_DYNAMIC,GARAGE_STATIC>
    )

    target_include_directories(Garage
        PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
    )

    target_link_libraries(Garage
        PUBLIC
        SDL3::SDL3
        imgui::imgui
        imguizmo::imguizmo
        implot::implot
        spdlog::spdlog
        fmt::fmt
        nameof::nameof
        rapidjson::rapidjson
        freetype
    )

    if(GARAGE_ENABLE_TRACY)
        target_link_libraries(Garage PRIVATE Tracy::TracyClient)
    endif()

    target_precompile_headers(Garage PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src/precompiled.hpp"
    )
else()
    message(STATUS "[Garage] No .cpp source files found in src/. Garage library target creation skipped.")
endif()

option(GARAGE_BUILD_EXAMPLES "Build Garage example applications" ON)
if(GARAGE_BUILD_EXAMPLES AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/examples/CMakeLists.txt")
    add_subdirectory(examples)
endif()
