cmake_minimum_required(VERSION 3.22)

project(garage VERSION 1.0 LANGUAGES CXX)

add_library(garage 
    src/garage.cpp
        include/garage/platform/display.hpp
        src/platform/display_internal.hpp
        src/platform/display_internal.cpp)

target_include_directories(garage 
    PUBLIC 
        include)

include (cmake/dependencies.cmake)