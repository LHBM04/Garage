set(GARAGE_CLANGD_QUERY_DIR "${CMAKE_BINARY_DIR}/.cmake/api/v1/query/client-garage-clangd")
file(MAKE_DIRECTORY "${GARAGE_CLANGD_QUERY_DIR}")
file(WRITE "${GARAGE_CLANGD_QUERY_DIR}/codemodel-v2" "")
file(WRITE "${GARAGE_CLANGD_QUERY_DIR}/toolchains-v1" "")

set(GARAGE_CLANGD_NATIVE_DATABASE "")
if(NOT CMAKE_CONFIGURATION_TYPES)
    set(GARAGE_CLANGD_NATIVE_DATABASE "${CMAKE_BINARY_DIR}/compile_commands.json")
endif()

set(GARAGE_CLANGD_TARGET "${CMAKE_CXX_COMPILER_TARGET}")
if(NOT GARAGE_CLANGD_TARGET AND MSVC AND WIN32)
    string(TOLOWER "${CMAKE_CXX_COMPILER_ARCHITECTURE_ID}" GARAGE_CLANGD_ARCHITECTURE)
    if(GARAGE_CLANGD_ARCHITECTURE MATCHES "^(x64|amd64)$")
        set(GARAGE_CLANGD_TARGET "x86_64-pc-windows-msvc")
    elseif(GARAGE_CLANGD_ARCHITECTURE MATCHES "^(arm64|aarch64)$")
        set(GARAGE_CLANGD_TARGET "aarch64-pc-windows-msvc")
    elseif(GARAGE_CLANGD_ARCHITECTURE MATCHES "^(x86|win32)$")
        set(GARAGE_CLANGD_TARGET "i686-pc-windows-msvc")
    endif()
endif()

add_custom_target(garage-clangd
    COMMAND "${CMAKE_COMMAND}"
        "-DGARAGE_BUILD_DIR=${CMAKE_BINARY_DIR}"
        "-DGARAGE_CONFIG=$<CONFIG>"
        "-DGARAGE_DATABASE=${PROJECT_SOURCE_DIR}/compile_commands.json"
        "-DGARAGE_NATIVE_DATABASE=${GARAGE_CLANGD_NATIVE_DATABASE}"
        "-DGARAGE_COMPILER_TARGET=${GARAGE_CLANGD_TARGET}"
        -P "${CMAKE_CURRENT_LIST_DIR}/export_clangd.cmake"
    COMMENT "Updating clangd compilation database for $<CONFIG>"
    VERBATIM
)
