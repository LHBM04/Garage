if(NOT CMAKE_GENERATOR STREQUAL "Ninja")
    message(FATAL_ERROR "[Garage] Ninja generator is required. Configure using a CMake preset or -G Ninja.")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_custom_target(garage-clangd
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
        "${CMAKE_BINARY_DIR}/compile_commands.json"
        "${PROJECT_SOURCE_DIR}/compile_commands.json"
    COMMENT "Updating clangd compilation database"
    VERBATIM
)
