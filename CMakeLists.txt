cmake_minimum_required(VERSION 3.21)

project(
  garage
  VERSION 0.1 
  DESCRIPTION "Garage - The portable C++ framework for 2D/3D games"
  LANGUAGES CXX)

set(GARAGE_RUNTIME_SOURCES
  src/window.cpp
)

add_library(garage ${GARAGE_RUNTIME_SOURCES})

if(MSVC)
  add_compile_options(/utf-8)
endif()

target_include_directories(garage
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

find_package(SDL3 CONFIG REQUIRED)
target_link_libraries(garage
  PRIVATE
    SDL3::SDL3
)
