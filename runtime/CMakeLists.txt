set(GARAGE_RUNTIME_SOURCES
  src/entry.cpp
  src/window.cpp
  src/window_subsystem_impl.cpp
)

add_library(runtime ${GARAGE_RUNTIME_SOURCES})
add_library(garage::runtime ALIAS runtime)

if(MSVC)
  add_compile_options(/utf-8)
endif()

target_include_directories(runtime
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include  
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

find_package(SDL3 CONFIG REQUIRED)
target_link_libraries(runtime
  PUBLIC
    SDL3::SDL3
)
