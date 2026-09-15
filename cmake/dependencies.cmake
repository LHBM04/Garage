include (FetchContent)

FetchContent_Declare(
    sdl3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL
    GIT_TAG        release-3.4.16
)

FetchContent_MakeAvailable(sdl3)

target_link_libraries(garage PRIVATE SDL3::SDL3)