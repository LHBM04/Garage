cmake_minimum_required(VERSION 3.19)
project(Garage
    LANGUAGES CXX
    VERSION   1.0.0
)

if (GARAGE_BUILD_SHARED)
    add_library(Garage SHARED)
    target_compile_definitions(Garage PRIVATE GARAGE_BUILD_SHARED)
else()
    add_library(Garage STATIC)
    target_compile_definitions(Garage PRIVATE GARAGE_BUILD_STATIC)
endif()

target_sources(Garage
    PUBLIC
        Public/Garage/Test.hpp
    PRIVATE
        Private/Test.cpp
)

target_include_directories(Garage
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Public>
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Public/Garage>
)