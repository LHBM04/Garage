include(FetchContent)
set(FETCHCONTENT_QUIET OFF)

set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.4.20
    GIT_SHALLOW TRUE
)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG docking
    GIT_SHALLOW TRUE
)

FetchContent_Declare(
    imguizmo
    GIT_REPOSITORY https://github.com/CedricGuillemet/ImGuizmo.git
    GIT_TAG master
    GIT_SHALLOW TRUE
)

FetchContent_Declare(
    implot
    GIT_REPOSITORY https://github.com/epezent/implot.git
    GIT_TAG v1.1
    GIT_SHALLOW TRUE
)

set(FMT_DOC OFF CACHE BOOL "" FORCE)
set(FMT_TEST OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 12.2.1
    GIT_SHALLOW TRUE
)

set(SPDLOG_FMT_EXTERNAL ON CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_EXAMPLE OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_TESTS OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.17.0
    GIT_SHALLOW TRUE
)

set(NAMEOF_OPT_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(NAMEOF_OPT_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    nameof
    GIT_REPOSITORY https://github.com/Neargye/nameof.git
    GIT_TAG v0.11.0
    GIT_SHALLOW TRUE
)

FetchContent_Declare(
    rapidjson
    GIT_REPOSITORY https://github.com/Tencent/rapidjson.git
    GIT_TAG master
    GIT_SHALLOW TRUE
    SOURCE_SUBDIR cmake_skip
)

set(FT_DISABLE_HARFBUZZ ON CACHE BOOL "" FORCE)
FetchContent_Declare(
    freetype
    GIT_REPOSITORY https://github.com/freetype/freetype.git
    GIT_TAG VER-2-15-0
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(
    SDL3
    fmt
    spdlog
    nameof
    freetype
    imgui
    imguizmo
    implot
    rapidjson
)

add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
)
target_include_directories(imgui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)
target_link_libraries(imgui PUBLIC SDL3::SDL3)
add_library(imgui::imgui ALIAS imgui)

target_link_libraries(imguizmo PUBLIC imgui::imgui)

add_library(implot STATIC
    ${implot_SOURCE_DIR}/implot.cpp
    ${implot_SOURCE_DIR}/implot_items.cpp
)
target_include_directories(implot PUBLIC
    ${implot_SOURCE_DIR}
)
target_link_libraries(implot PUBLIC imgui::imgui)
add_library(implot::implot ALIAS implot)

add_library(rapidjson INTERFACE)
target_include_directories(rapidjson INTERFACE
    ${rapidjson_SOURCE_DIR}/include
)
add_library(rapidjson::rapidjson ALIAS rapidjson)
