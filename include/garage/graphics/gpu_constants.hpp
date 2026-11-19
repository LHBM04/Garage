#ifndef GARAGE_GRAPHICS_GPU_CONSTANTS_HPP
#define GARAGE_GRAPHICS_GPU_CONSTANTS_HPP

#include "../core/common.hpp"

#include "../math/color_rgba.hpp"
#include "../math/matrix4x4.hpp"
#include "../math/vector2d.hpp"
#include "../math/vector3d.hpp"
#include "../math/vector4d.hpp"

namespace Garage
{
    struct alignas(16) GARAGE_API CameraConstants final
    {
        static constexpr UInt32 ROOT_SLOT = 0;

        Matrix4x4Float view{};
        Matrix4x4Float projection{};
        Vector3DFloat  lightDirection{-0.5f, -1.0f, 0.5f};
        Float32        lightIntensity = 1.0f;
        ColorRGBAFloat lightColor{1.0f, 1.0f, 1.0f, 1.0f};
        ColorRGBAFloat ambientLightColor{0.15f, 0.15f, 0.15f, 1.0f};
    };

    struct alignas(16) GARAGE_API ObjectConstants final
    {
        static constexpr UInt32 ROOT_SLOT = 1;

        Matrix4x4Float world{};
    };

    struct alignas(16) GARAGE_API MaterialConstants final
    {
        static constexpr UInt32 ROOT_SLOT = 2;

        ColorRGBAFloat baseColor{};
        ColorRGBAFloat emissiveColor{};
        Float32        metallic    = 0.0f;
        Float32        roughness   = 0.0f;
        UInt32         textureMask = 0;
        Float32        padding     = 0.0f;
        ColorRGBAFloat specularColor{};
    };

    struct alignas(16) GARAGE_API SkyboxConstants final
    {
        static constexpr UInt32 ROOT_SLOT = 2;

        ColorRGBAFloat color{1.0f, 1.0f, 1.0f, 1.0f};
    };

    struct alignas(16) GARAGE_API ImageUIConstants final
    {
        static constexpr UInt32 ROOT_SLOT = 2;

        Vector2DFloat  viewportSize{};
        Vector2DFloat  padding{};
        ColorRGBAFloat color{1.0f, 1.0f, 1.0f, 1.0f};
    };

    struct alignas(16) GARAGE_API TextUIConstants final
    {
        static constexpr UInt32 ROOT_SLOT = 2;

        Vector2DFloat  viewportSize{};
        Vector2DFloat  padding{};
        Vector4DFloat  glyphRect{};
        Vector4DFloat  glyphUV{};
        ColorRGBAFloat textColor{};
    };

    struct alignas(16) GARAGE_API TerrainConstants final
    {
        static constexpr UInt32 ROOT_SLOT = 2;

        Vector4DFloat padding{};
    };

    struct alignas(16) GARAGE_API DirectionalLightConstants final
    {
        static constexpr UInt32 ROOT_SLOT = 2;

        Vector3DFloat  direction{-0.5f, -1.0f, 0.5f};
        Float32        intensity = 1.0f;
        ColorRGBAFloat color{1.0f, 1.0f, 1.0f, 1.0f};
        ColorRGBAFloat ambientColor{0.15f, 0.15f, 0.15f, 1.0f};
    };

    struct alignas(16) GARAGE_API BillboardConstants final
    {
        static constexpr UInt32 ROOT_SLOT = 2;

        Vector2DFloat  size{1.0f, 1.0f};
        UInt32         frameIndex = 0;
        UInt32         columns    = 1;
        UInt32         rows       = 1;
        Vector3DFloat  padding{};
        ColorRGBAFloat color{1.0f, 1.0f, 1.0f, 1.0f};
    };
} // namespace Garage

#endif // GARAGE_GRAPHICS_GPU_CONSTANTS_HPP
