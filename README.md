# Garage

> **Garage - 2D 및 3D 게임을 위한 오픈 소스 및 크로스 플랫폼 C++ 게임 프레임워크**

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

**Garage**는 2D 및 3D 게임 개발을 위해 설계된 오픈 소스 크로스 플랫폼 C++ 게임 런타임 프레임워크입니다.

## 지원 플랫폼

| 플랫폼 | 지원 상태 |
| :--- | :--- |
| **Windows** | 지원 |
| **Linux** | 지원 예정 |
| **macOS** | 지원 예정 |

## 주요 기능

**Garage**는 다음과 같은 기능(단위)로 구성됩니다.

| 모듈 | 주요 기능 | 구현 상태 |
| :--- | :--- | :--- |
| **코어 (`core`)** | 애플리케이션·엔진 실행 흐름, 우선순위 기반 서브시스템 생명주기 관리, 공통 타입 및 단언 | 구현 완료 |
| **플랫폼 (`platform`)** | 멀티 윈도우 디스플레이 관리, 이벤트 큐 및 디스패치, 액션 기반 입력 처리, 프레임·고정 타임스텝 타이밍 | 구현 완료 |
| **수학 (`math`)** | 2D/3D/4D 벡터, 행렬, 사원수, RGB/RGBA 색상, 경계 볼륨(`AABB`, `OBB`, `Sphere`), 충돌 판정 및 수학 유틸리티 | 구현 완료 |
| **유틸리티 (`utilities`)** | 멀티캐스트 델리게이트, `fmt`·`spdlog` 기반 로깅 | 구현 완료 |
| **그래픽스 (`graphics`)** | GPU 디바이스·커맨드 버퍼·스왑체인·파이프라인·리소스 추상화, 카메라, 방향성 조명, 메시·빌보드·스카이박스 렌더링 | 구현 예정 |
| **오브젝트 (`objects`)** | 씬 생명주기, `GameObject`·`Component`·`Transform` 계층 구조, 지형 및 콜라이더(`Box`, `Sphere`, `Terrain`) | 구현 예정 |
| **리소스 (`resources`)** | 에셋 서브시스템·임포터 기반 모델, 메시, 머티리얼, 텍스처, 큐브맵, 셰이더, 애니메이션, 비트맵 폰트, 지형 에셋 관리 | 구현 예정 |
| **UI (`ui`)** | 이미지(`ImageView`), 텍스트(`TextView`), 버튼(`ButtonUI`) 기반 런타임 UI 구성 요소 | 구현 예정 |

## 시작하기

시작하기에 앞서서 **Garage**는 다음과 같은 환경을 요구합니다.

| 항목 | 요구사항 |
| :--- | :--- |
| **CMake** | 3.22 이상 |
| **빌드 시스템** | Ninja |
| **컴파일러** | C++20 지원 컴파일러 (Windows: MSVC 19.29 이상 및 개발자 환경, Linux/macOS: Clang 14.0+ / AppleClang 14.0+ / GCC 11.0+) |
| **아키텍처** | 64비트 (`x64`, `ARM64`) |

환경이 갖추어졌다면 [`CMakePresets.json`](CMakePresets.json)에 정의된 프리셋(`debug`, `debug-static`, `release`, `release-static`)을 사용하여 다음과 같이 구성 및 빌드할 수 있습니다

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

**Garage**는 다음과 같은 라이브러리를 사용합니다.

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

**Garage**는 MIT 라이선스를 준수합니다 - 
자세한 내용은 [LICENSE](LICENSE) 파일을 참고하세요.
