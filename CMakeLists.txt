cmake_minimum_required(VERSION 3.21)

project(
  garage
  VERSION 0.1 
  DESCRIPTION "Garage - The portable C++ framework for 2D/3D games"
  LANGUAGES CXX)

# 런타임 빌드
if (GARAGE_BUILD_RUNTIME)
  add_subdirectory(runtime)
endif()

# 예제 빌드
if (GARAGE_BUILD_EXAMPLE)
  add_subdirectory(example)
endif()
