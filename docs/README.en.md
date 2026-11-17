# Garage

**English** | [한국어](README.ko.md) | [Home](../README.md)

> **Garage - The portable C++ game engine for 2D & 3D games**

![C++20](https://img.shields.io/badge/C%2B%2B-20-00599C?logo=cplusplus&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-3.22%2B-064F8C?logo=cmake&logoColor=white)
![Platform](https://img.shields.io/badge/Platform-Windows-0078D6?logo=windows&logoColor=white)
![License](https://img.shields.io/badge/License-MIT-green.svg)

[![Build (Windows)](https://github.com/LHBM04/Garage/actions/workflows/build-windows.yml/badge.svg?branch=develop)](https://github.com/LHBM04/Garage/actions/workflows/build-windows.yml)
[![Build (Linux)](https://github.com/LHBM04/Garage/actions/workflows/build-linux.yml/badge.svg?branch=develop)](https://github.com/LHBM04/Garage/actions/workflows/build-linux.yml)
[![Build (macOS)](https://github.com/LHBM04/Garage/actions/workflows/build-macos.yml/badge.svg?branch=develop)](https://github.com/LHBM04/Garage/actions/workflows/build-macos.yml)
[![Format Check (clang-format)](https://github.com/LHBM04/Garage/actions/workflows/clang-format.yml/badge.svg?branch=develop)](https://github.com/LHBM04/Garage/actions/workflows/clang-format.yml)
[![Static Analysis (clang-tidy)](https://github.com/LHBM04/Garage/actions/workflows/clang-tidy.yml/badge.svg?branch=develop)](https://github.com/LHBM04/Garage/actions/workflows/clang-tidy.yml)

---

## Overview

**Garage** is an open-source, cross-platform C++ game framework and editor designed for developing 2D and 3D games.

## Supported Platforms

| Platform | Status |
| :--- | :--- |
| **Windows** | Supported |
| **Linux** | Planned |
| **macOS** | Planned |

## Features

Garage is designed around a modular 2D & 3D game framework and editor architecture:

- **Core & Subsystems**: Application/Engine lifecycle management with priority-ordered subsystems, delegates, and event dispatch
- **Platform, Input & Timing**: Multi-window display management, action-based input handling, and frame/fixed-timestep timing
- **Scene & Object Model**: Scene lifecycle (`FixedUpdate`, `Update`, `LateUpdate`, `Render`), persistent object transfer, hierarchical `Transform`, and `Component`-based objects
- **Graphics & RHI (`SDL_GPU`)**: Hardware-abstracted GPU device/command buffer/swapchain/pipeline architecture with camera, directional lighting, mesh, billboard, skybox, and terrain rendering
- **Math & Collision**: Vectors (`2D`/`3D`/`4D`), matrices, quaternions, bounding volumes (`AABB`, `OBB`, `Sphere`), raycasting, and box/sphere/terrain colliders with debug visualization
- **Asset Pipeline**: Extensible importer and asset management for models, meshes, materials, textures, cubemaps, shaders, animations, fonts, and terrains
- **UI & Editor**: UI components alongside `ImGui`, `ImGuizmo`, and `ImPlot` integration for editor and debugging tools

## Getting Started

Configure and build using the presets defined in [`CMakePresets.json`](../CMakePresets.json) (`debug`, `debug-static`, `release`, `release-static`):

```bash
# Configure & Build (Debug - Dynamic)
cmake --preset debug
cmake --build --preset debug

# Configure & Build (Debug - Static)
cmake --preset debug-static
cmake --build --preset debug-static

# Configure & Build (Release - Dynamic)
cmake --preset release
cmake --build --preset release

# Configure & Build (Release - Static)
cmake --preset release-static
cmake --build --preset release-static
```

## Dependencies

| Library | Description |
| :--- | :--- |
| [SDL](https://github.com/libsdl-org/SDL) | Window, input, GPU rendering (`SDL_GPU`), and platform abstraction |
| [imgui](https://github.com/ocornut/imgui) | Immediate mode GUI |
| [ImGuizmo](https://github.com/CedricGuillemet/ImGuizmo) | 3D transformation gizmo for ImGui |
| [implot](https://github.com/epezent/implot) | Plotting library for ImGui |
| [spdlog](https://github.com/gabime/spdlog) | Fast C++ logging library |
| [fmt](https://github.com/fmtlib/fmt) | Modern string formatting library |
| [nameof](https://github.com/Neargye/nameof) | Compile-time name reflection for C++ |
| [rapidjson](https://github.com/Tencent/rapidjson) | Fast JSON parser and generator |
| [freetype](https://github.com/freetype/freetype) | Font rendering engine |

## License

This project is licensed under the MIT License — see the [LICENSE](../LICENSE) file for details.
