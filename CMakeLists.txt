cmake_minimum_required(VERSION 3.21)

project(
  garage
  VERSION 0.1 
  DESCRIPTION "Garage - The portable C++ framework for 2D/3D games"
  LANGUAGES CXX)

if (BUILD_SHARED_LIBS)
  add_library(garage SHARED
    src/garage.cpp)
else()
  add_library(garage STATIC
    src/garage.cpp)
endif()

target_include_directories(garage
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include/garage/
)
