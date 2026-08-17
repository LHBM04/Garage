cmake_minimum_required(VERSION 3.21)

project(
  garage
  VERSION 0.1 
  DESCRIPTION "Garage - The portable C++ framework for 2D/3D games"
  LANGUAGES CXX)

add_library(garage
  src/garage.cpp)

target_include_directories(garage
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include/garage/
)
