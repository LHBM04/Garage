cmake_minimum_required(VERSION 3.22)

project(Garage VERSION 0.1.0 LANGUAGES CXX)

add_library(garage
    src/garage.cpp
)

target_compile_features(garage PUBLIC cxx_std_20)

target_precompile_headers(garage PRIVATE
    src/pch.hpp
)
