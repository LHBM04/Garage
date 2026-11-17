# Garage

[English](README.en.md) | **한국어** | [홈](../README.md)

> **Garage - 2D 및 3D 게임을 위한 크로스 플랫폼 C++ 게임 엔진**

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

## 개요

**Garage**는 2D 및 3D 게임 개발을 위해 설계된 오픈 소스 크로스 플랫폼 C++ 게임 프레임워크 및 에디터입니다.

## 지원 플랫폼

| 플랫폼 | 지원 상태 |
| :--- | :--- |
| **Windows** | 지원 |
| **Linux** | 지원 예정 |
| **macOS** | 지원 예정 |

## 주요 기능

Garage는 모듈식 2D 및 3D 게임 프레임워크와 에디터 아키텍처를 기반으로 설계됩니다:

- **코어 및 서브시스템**: 우선순위 기반 애플리케이션/엔진 서브시스템 생명주기 관리, 델리게이트 및 이벤트 디스패치
- **플랫폼, 입력 및 타이밍**: 멀티 윈도우 디스플레이 관리, 액션 기반 입력 처리, 프레임 및 고정 타임스텝 타이밍
- **씬 및 오브젝트 모델**: 씬 생명주기(`FixedUpdate`, `Update`, `LateUpdate`, `Render`), 영속 오브젝트 이관, 계층형 `Transform` 및 `Component` 기반 오브젝트 구조
- **그래픽스 및 RHI (`SDL_GPU`)**: GPU 디바이스·커맨드 버퍼·스왑체인·파이프라인 추상화 계층과 카메라, 방향성 조명, 메시, 빌보드, 스카이박스, 지형 렌더링
- **수학 및 충돌**: 벡터(`2D`/`3D`/`4D`), 행렬, 사원수, 경계 볼륨(`AABB`, `OBB`, `Sphere`), 레이캐스트, 박스/구/지형 콜라이더 및 디버그 시각화
- **에셋 파이프라인**: 모델, 메시, 머티리얼, 텍스처, 큐브맵, 셰이더, 애니메이션, 폰트, 지형 에셋을 위한 확장 가능한 임포터 및 리소스 관리
- **UI 및 에디터**: 기본 UI 구성 요소와 함께 `ImGui`, `ImGuizmo`, `ImPlot` 기반의 에디터 및 디버깅 도구 통합

## 시작하기

[`CMakePresets.json`](../CMakePresets.json)에 정의된 프리셋(`debug`, `debug-static`, `release`, `release-static`)을 사용해 구성 및 빌드할 수 있습니다:

```bash
# 구성 및 빌드 (Debug - Dynamic)
cmake --preset debug
cmake --build --preset debug

# 구성 및 빌드 (Debug - Static)
cmake --preset debug-static
cmake --build --preset debug-static

# 구성 및 빌드 (Release - Dynamic)
cmake --preset release
cmake --build --preset release

# 구성 및 빌드 (Release - Static)
cmake --preset release-static
cmake --build --preset release-static
```

## 의존성

| 라이브러리 | 설명 |
| :--- | :--- |
| [SDL](https://github.com/libsdl-org/SDL) | 윈도우, 입력, GPU 렌더링(`SDL_GPU`) 및 플랫폼 추상화 |
| [imgui](https://github.com/ocornut/imgui) | 즉시 모드 GUI |
| [ImGuizmo](https://github.com/CedricGuillemet/ImGuizmo) | ImGui용 3D 변환 기즈모 |
| [implot](https://github.com/epezent/implot) | ImGui용 플로팅 라이브러리 |
| [spdlog](https://github.com/gabime/spdlog) | 고속 C++ 로깅 라이브러리 |
| [fmt](https://github.com/fmtlib/fmt) | 모던 문자열 포맷팅 라이브러리 |
| [nameof](https://github.com/Neargye/nameof) | C++ 컴파일 타임 이름 리플렉션 |
| [rapidjson](https://github.com/Tencent/rapidjson) | 고속 JSON 파서 및 생성기 |
| [freetype](https://github.com/freetype/freetype) | 폰트 렌더링 엔진 |

## 라이선스

이 프로젝트는 MIT 라이선스를 따릅니다 — 자세한 내용은 [LICENSE](../LICENSE) 파일을 참고하세요.
