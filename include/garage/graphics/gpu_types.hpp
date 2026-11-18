#ifndef GARAGE_GRAPHICS_GPU_TYPES_HPP
#define GARAGE_GRAPHICS_GPU_TYPES_HPP

#include <span>

#include "../core/defines.hpp"
#include "../core/types.hpp"

namespace Garage
{
    class IGPUBuffer;

    enum class EGPUBufferUsage : UInt8
    {
        VERTEX,
        INDEX,
        UPLOAD,
        READBACK,
    };

    enum class EGPUTextureFormat : UInt8
    {
        UNKNOWN,
        R8_UNORM,
        R8G8_UNORM,
        R8G8B8A8_UNORM,
        B8G8R8A8_UNORM,
        R8G8B8A8_UNORM_SRGB,
        B8G8R8A8_UNORM_SRGB,
        R16G16_UNORM,
        R16G16B16A16_FLOAT,
        R32G32B32A32_FLOAT,
        BC1_RGBA_UNORM,
        BC1_RGBA_UNORM_SRGB,
        BC2_RGBA_UNORM,
        BC2_RGBA_UNORM_SRGB,
        BC3_RGBA_UNORM,
        BC3_RGBA_UNORM_SRGB,
        BC4_R_UNORM,
        BC5_RG_UNORM,
        BC6H_RGB_FLOAT,
        BC6H_RGB_UFLOAT,
        BC7_RGBA_UNORM,
        BC7_RGBA_UNORM_SRGB,
        D16_UNORM,
        D24_UNORM,
        D32_FLOAT,
        D24_UNORM_S8_UINT,
        D32_FLOAT_S8_UINT,
    };

    enum class EGPUTextureType : UInt8
    {
        TEXTURE_2D,
        TEXTURE_CUBE,
    };

    enum class EGPUTextureUsage : UInt32
    {
        NONE                 = 0u,
        SAMPLER              = 1u << 0u,
        COLOR_TARGET         = 1u << 1u,
        DEPTH_STENCIL_TARGET = 1u << 2u,
    };

    enum class EGPUIndexFormat : UInt8
    {
        UINT16,
        UINT32,
    };

    enum class EGPUPrimitiveTopology : UInt8
    {
        TRIANGLE_LIST,
        TRIANGLE_STRIP,
        LINE_LIST,
        LINE_STRIP,
        POINT_LIST,
    };

    enum class EGPUShaderStage : UInt8
    {
        VERTEX,
        FRAGMENT,
    };

    enum class EGPUShaderFormat : UInt32
    {
        INVALID  = 0u,
        SPIRV    = 1u << 1u,
        DXBC     = 1u << 2u,
        DXIL     = 1u << 3u,
        MSL      = 1u << 4u,
        METALLIB = 1u << 5u,
    };

    enum class EGPUVertexElementFormat : UInt8
    {
        FLOAT,
        FLOAT2,
        FLOAT3,
        FLOAT4,
        INT,
        INT2,
        INT3,
        INT4,
        UINT,
        UINT2,
        UINT3,
        UINT4,
        UBYTE4_NORM,
    };

    enum class EGPUFillMode : UInt8
    {
        FILL,
        LINE,
    };

    enum class EGPUCullMode : UInt8
    {
        NONE,
        FRONT,
        BACK,
    };

    enum class EGPUFrontFace : UInt8
    {
        COUNTER_CLOCKWISE,
        CLOCKWISE,
    };

    enum class EGPUCompareOp : UInt8
    {
        NEVER,
        LESS,
        EQUAL,
        LESS_OR_EQUAL,
        GREATER,
        NOT_EQUAL,
        GREATER_OR_EQUAL,
        ALWAYS,
    };

    enum class EGPUBlendFactor : UInt8
    {
        ZERO,
        ONE,
        SRC_COLOR,
        ONE_MINUS_SRC_COLOR,
        DST_COLOR,
        ONE_MINUS_DST_COLOR,
        SRC_ALPHA,
        ONE_MINUS_SRC_ALPHA,
        DST_ALPHA,
        ONE_MINUS_DST_ALPHA,
    };

    enum class EGPUBlendOp : UInt8
    {
        ADD,
        SUBTRACT,
        REVERSE_SUBTRACT,
        MIN,
        MAX,
    };

    struct GARAGE_API GPUViewport final
    {
        Float32 x        = 0.0f;
        Float32 y        = 0.0f;
        Float32 width    = 0.0f;
        Float32 height   = 0.0f;
        Float32 minDepth = 0.0f;
        Float32 maxDepth = 1.0f;
    };

    struct GARAGE_API GPURect final
    {
        Int32 x      = 0;
        Int32 y      = 0;
        Int32 width  = 0;
        Int32 height = 0;
    };

    struct GARAGE_API GPUVertexAttribute final
    {
        UInt32                  location   = 0;
        UInt32                  bufferSlot = 0;
        EGPUVertexElementFormat format     = EGPUVertexElementFormat::FLOAT3;
        UInt32                  offset     = 0;
    };

    struct GARAGE_API GPUVertexBufferView final
    {
        const IGPUBuffer* buffer        = nullptr;
        UInt32            offset        = 0;
        UInt32            sizeInBytes   = 0;
        UInt32            strideInBytes = 0;
    };

    struct GARAGE_API GPUIndexBufferView final
    {
        const IGPUBuffer* buffer      = nullptr;
        UInt32            offset      = 0;
        UInt32            sizeInBytes = 0;
        EGPUIndexFormat   format      = EGPUIndexFormat::UINT32;
    };

    struct GARAGE_API GPUTextureSubresourceData final
    {
        std::span<const Byte> data     = {};
        UInt32                mipLevel = 0;
        UInt32                layer    = 0;
    };
} // namespace Garage

#endif // GARAGE_GRAPHICS_GPU_TYPES_HPP
