#ifndef GARAGE_GRAPHICS_MESH_VERTEX_HPP
#define GARAGE_GRAPHICS_MESH_VERTEX_HPP

#include <array>
#include <cstddef>

#include "../core/defines.hpp"
#include "../core/types.hpp"

#include "../math/color_rgba.hpp"
#include "../math/vector2d.hpp"
#include "../math/vector3d.hpp"

#include "gpu_types.hpp"

namespace Garage
{
    struct GARAGE_API MeshVertex final
    {
        Vector3DFloat  position{};
        Vector3DFloat  normal{};
        ColorRGBAFloat color{};
        Vector2DFloat  textureCoordinate{};
    };

    inline constexpr std::array<GPUVertexAttribute, 4> MESH_VERTEX_ATTRIBUTES = {
            GPUVertexAttribute{
                    .location   = 0,
                    .bufferSlot = 0,
                    .format     = EGPUVertexElementFormat::FLOAT3,
                    .offset     = static_cast<UInt32>(offsetof(MeshVertex, position)),
            },
            GPUVertexAttribute{
                    .location   = 1,
                    .bufferSlot = 0,
                    .format     = EGPUVertexElementFormat::FLOAT3,
                    .offset     = static_cast<UInt32>(offsetof(MeshVertex, normal)),
            },
            GPUVertexAttribute{
                    .location   = 2,
                    .bufferSlot = 0,
                    .format     = EGPUVertexElementFormat::FLOAT4,
                    .offset     = static_cast<UInt32>(offsetof(MeshVertex, color)),
            },
            GPUVertexAttribute{
                    .location   = 3,
                    .bufferSlot = 0,
                    .format     = EGPUVertexElementFormat::FLOAT2,
                    .offset     = static_cast<UInt32>(offsetof(MeshVertex, textureCoordinate)),
            },
    };
} // namespace Garage

#endif // GARAGE_GRAPHICS_MESH_VERTEX_HPP
