set(GARAGE_RUNTIME_SOURCES
  src/window.cpp
  src/window_subsystem.cpp
)

add_library(runtime ${GARAGE_RUNTIME_SOURCES})
add_library(garage::runtime ALIAS runtime)

if(MSVC)
  add_compile_options(/utf-8)
endif()

target_include_directories(runtime
  PRIVATE
    include/garage
)

find_package(SDL3 CONFIG REQUIRED)
target_link_libraries(runtime
  PRIVATE
    SDL3::SDL3
)
