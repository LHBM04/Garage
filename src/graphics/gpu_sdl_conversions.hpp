#ifndef GARAGE_GRAPHICS_GPU_SDL_CONVERSIONS_HPP
#define GARAGE_GRAPHICS_GPU_SDL_CONVERSIONS_HPP

#include <SDL3/SDL_gpu.h>

#include "garage/core/types.hpp"

#include "garage/graphics/gpu_types.hpp"

namespace Garage::GraphicsDetail
{
    [[nodiscard]] constexpr SDL_GPUTextureFormat ToSDLTextureFormat(EGPUTextureFormat format) noexcept
    {
        switch (format)
        {
            case EGPUTextureFormat::R8G8B8A8_UNORM:
            {
                return SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
            }
            case EGPUTextureFormat::B8G8R8A8_UNORM:
            {
                return SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM;
            }
            case EGPUTextureFormat::R8G8B8A8_UNORM_SRGB:
            {
                return SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM_SRGB;
            }
            case EGPUTextureFormat::B8G8R8A8_UNORM_SRGB:
            {
                return SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM_SRGB;
            }
            case EGPUTextureFormat::D16_UNORM:
            {
                return SDL_GPU_TEXTUREFORMAT_D16_UNORM;
            }
            case EGPUTextureFormat::D24_UNORM:
            {
                return SDL_GPU_TEXTUREFORMAT_D24_UNORM;
            }
            case EGPUTextureFormat::D32_FLOAT:
            {
                return SDL_GPU_TEXTUREFORMAT_D32_FLOAT;
            }
            case EGPUTextureFormat::D24_UNORM_S8_UINT:
            {
                return SDL_GPU_TEXTUREFORMAT_D24_UNORM_S8_UINT;
            }
            case EGPUTextureFormat::D32_FLOAT_S8_UINT:
            {
                return SDL_GPU_TEXTUREFORMAT_D32_FLOAT_S8_UINT;
            }
            case EGPUTextureFormat::UNKNOWN:
            default:
            {
                return SDL_GPU_TEXTUREFORMAT_INVALID;
            }
        }
    }

    [[nodiscard]] constexpr EGPUTextureFormat FromSDLTextureFormat(SDL_GPUTextureFormat format) noexcept
    {
        switch (format)
        {
            case SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM:
            {
                return EGPUTextureFormat::R8G8B8A8_UNORM;
            }
            case SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM:
            {
                return EGPUTextureFormat::B8G8R8A8_UNORM;
            }
            case SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM_SRGB:
            {
                return EGPUTextureFormat::R8G8B8A8_UNORM_SRGB;
            }
            case SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM_SRGB:
            {
                return EGPUTextureFormat::B8G8R8A8_UNORM_SRGB;
            }
            case SDL_GPU_TEXTUREFORMAT_D16_UNORM:
            {
                return EGPUTextureFormat::D16_UNORM;
            }
            case SDL_GPU_TEXTUREFORMAT_D24_UNORM:
            {
                return EGPUTextureFormat::D24_UNORM;
            }
            case SDL_GPU_TEXTUREFORMAT_D32_FLOAT:
            {
                return EGPUTextureFormat::D32_FLOAT;
            }
            case SDL_GPU_TEXTUREFORMAT_D24_UNORM_S8_UINT:
            {
                return EGPUTextureFormat::D24_UNORM_S8_UINT;
            }
            case SDL_GPU_TEXTUREFORMAT_D32_FLOAT_S8_UINT:
            {
                return EGPUTextureFormat::D32_FLOAT_S8_UINT;
            }
            default:
            {
                return EGPUTextureFormat::UNKNOWN;
            }
        }
    }

    [[nodiscard]] constexpr SDL_GPUTextureType ToSDLTextureType(EGPUTextureType type) noexcept
    {
        switch (type)
        {
            case EGPUTextureType::TEXTURE_CUBE:
            {
                return SDL_GPU_TEXTURETYPE_CUBE;
            }
            case EGPUTextureType::TEXTURE_2D:
            default:
            {
                return SDL_GPU_TEXTURETYPE_2D;
            }
        }
    }

    [[nodiscard]] constexpr SDL_GPUPrimitiveType ToSDLPrimitiveType(EGPUPrimitiveTopology topology) noexcept
    {
        switch (topology)
        {
            case EGPUPrimitiveTopology::TRIANGLE_STRIP:
            {
                return SDL_GPU_PRIMITIVETYPE_TRIANGLESTRIP;
            }
            case EGPUPrimitiveTopology::LINE_LIST:
            {
                return SDL_GPU_PRIMITIVETYPE_LINELIST;
            }
            case EGPUPrimitiveTopology::LINE_STRIP:
            {
                return SDL_GPU_PRIMITIVETYPE_LINESTRIP;
            }
            case EGPUPrimitiveTopology::POINT_LIST:
            {
                return SDL_GPU_PRIMITIVETYPE_POINTLIST;
            }
            case EGPUPrimitiveTopology::TRIANGLE_LIST:
            default:
            {
                return SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
            }
        }
    }

    [[nodiscard]] constexpr SDL_GPUIndexElementSize ToSDLIndexElementSize(EGPUIndexFormat format) noexcept
    {
        return format == EGPUIndexFormat::UINT16 ? SDL_GPU_INDEXELEMENTSIZE_16BIT : SDL_GPU_INDEXELEMENTSIZE_32BIT;
    }

    [[nodiscard]] constexpr SDL_GPUShaderStage ToSDLShaderStage(EGPUShaderStage stage) noexcept
    {
        return stage == EGPUShaderStage::FRAGMENT ? SDL_GPU_SHADERSTAGE_FRAGMENT : SDL_GPU_SHADERSTAGE_VERTEX;
    }

    [[nodiscard]] constexpr SDL_GPUShaderFormat ToSDLShaderFormat(EGPUShaderFormat format) noexcept
    {
        switch (format)
        {
            case EGPUShaderFormat::SPIRV:
            {
                return SDL_GPU_SHADERFORMAT_SPIRV;
            }
            case EGPUShaderFormat::DXBC:
            {
                return SDL_GPU_SHADERFORMAT_DXBC;
            }
            case EGPUShaderFormat::DXIL:
            {
                return SDL_GPU_SHADERFORMAT_DXIL;
            }
            case EGPUShaderFormat::MSL:
            {
                return SDL_GPU_SHADERFORMAT_MSL;
            }
            case EGPUShaderFormat::METALLIB:
            {
                return SDL_GPU_SHADERFORMAT_METALLIB;
            }
            case EGPUShaderFormat::INVALID:
            default:
            {
                return SDL_GPU_SHADERFORMAT_INVALID;
            }
        }
    }

    [[nodiscard]] constexpr SDL_GPUVertexElementFormat ToSDLVertexElementFormat(EGPUVertexElementFormat format) noexcept
    {
        switch (format)
        {
            case EGPUVertexElementFormat::FLOAT:
            {
                return SDL_GPU_VERTEXELEMENTFORMAT_FLOAT;
            }
            case EGPUVertexElementFormat::FLOAT2:
            {
                return SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
            }
            case EGPUVertexElementFormat::FLOAT3:
            {
                return SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
            }
            case EGPUVertexElementFormat::FLOAT4:
            {
                return SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4;
            }
            case EGPUVertexElementFormat::INT:
            {
                return SDL_GPU_VERTEXELEMENTFORMAT_INT;
            }
            case EGPUVertexElementFormat::INT2:
            {
                return SDL_GPU_VERTEXELEMENTFORMAT_INT2;
            }
            case EGPUVertexElementFormat::INT3:
            {
                return SDL_GPU_VERTEXELEMENTFORMAT_INT3;
            }
            case EGPUVertexElementFormat::INT4:
            {
                return SDL_GPU_VERTEXELEMENTFORMAT_INT4;
            }
            case EGPUVertexElementFormat::UINT:
            {
                return SDL_GPU_VERTEXELEMENTFORMAT_UINT;
            }
            case EGPUVertexElementFormat::UINT2:
            {
                return SDL_GPU_VERTEXELEMENTFORMAT_UINT2;
            }
            case EGPUVertexElementFormat::UINT3:
            {
                return SDL_GPU_VERTEXELEMENTFORMAT_UINT3;
            }
            case EGPUVertexElementFormat::UINT4:
            {
                return SDL_GPU_VERTEXELEMENTFORMAT_UINT4;
            }
            case EGPUVertexElementFormat::UBYTE4_NORM:
            {
                return SDL_GPU_VERTEXELEMENTFORMAT_UBYTE4_NORM;
            }
            default:
            {
                return SDL_GPU_VERTEXELEMENTFORMAT_INVALID;
            }
        }
    }

    [[nodiscard]] constexpr SDL_GPUFillMode ToSDLFillMode(EGPUFillMode mode) noexcept
    {
        return mode == EGPUFillMode::LINE ? SDL_GPU_FILLMODE_LINE : SDL_GPU_FILLMODE_FILL;
    }

    [[nodiscard]] constexpr SDL_GPUCullMode ToSDLCullMode(EGPUCullMode mode) noexcept
    {
        switch (mode)
        {
            case EGPUCullMode::NONE:
            {
                return SDL_GPU_CULLMODE_NONE;
            }
            case EGPUCullMode::FRONT:
            {
                return SDL_GPU_CULLMODE_FRONT;
            }
            case EGPUCullMode::BACK:
            default:
            {
                return SDL_GPU_CULLMODE_BACK;
            }
        }
    }

    [[nodiscard]] constexpr SDL_GPUFrontFace ToSDLFrontFace(EGPUFrontFace face) noexcept
    {
        return face == EGPUFrontFace::COUNTER_CLOCKWISE ? SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE
                                                        : SDL_GPU_FRONTFACE_CLOCKWISE;
    }

    [[nodiscard]] constexpr SDL_GPUCompareOp ToSDLCompareOp(EGPUCompareOp op) noexcept
    {
        switch (op)
        {
            case EGPUCompareOp::NEVER:
            {
                return SDL_GPU_COMPAREOP_NEVER;
            }
            case EGPUCompareOp::LESS:
            {
                return SDL_GPU_COMPAREOP_LESS;
            }
            case EGPUCompareOp::EQUAL:
            {
                return SDL_GPU_COMPAREOP_EQUAL;
            }
            case EGPUCompareOp::LESS_OR_EQUAL:
            {
                return SDL_GPU_COMPAREOP_LESS_OR_EQUAL;
            }
            case EGPUCompareOp::GREATER:
            {
                return SDL_GPU_COMPAREOP_GREATER;
            }
            case EGPUCompareOp::NOT_EQUAL:
            {
                return SDL_GPU_COMPAREOP_NOT_EQUAL;
            }
            case EGPUCompareOp::GREATER_OR_EQUAL:
            {
                return SDL_GPU_COMPAREOP_GREATER_OR_EQUAL;
            }
            case EGPUCompareOp::ALWAYS:
            default:
            {
                return SDL_GPU_COMPAREOP_ALWAYS;
            }
        }
    }

    [[nodiscard]] constexpr SDL_GPUBlendFactor ToSDLBlendFactor(EGPUBlendFactor factor) noexcept
    {
        switch (factor)
        {
            case EGPUBlendFactor::ZERO:
            {
                return SDL_GPU_BLENDFACTOR_ZERO;
            }
            case EGPUBlendFactor::ONE:
            {
                return SDL_GPU_BLENDFACTOR_ONE;
            }
            case EGPUBlendFactor::SRC_COLOR:
            {
                return SDL_GPU_BLENDFACTOR_SRC_COLOR;
            }
            case EGPUBlendFactor::ONE_MINUS_SRC_COLOR:
            {
                return SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_COLOR;
            }
            case EGPUBlendFactor::DST_COLOR:
            {
                return SDL_GPU_BLENDFACTOR_DST_COLOR;
            }
            case EGPUBlendFactor::ONE_MINUS_DST_COLOR:
            {
                return SDL_GPU_BLENDFACTOR_ONE_MINUS_DST_COLOR;
            }
            case EGPUBlendFactor::SRC_ALPHA:
            {
                return SDL_GPU_BLENDFACTOR_SRC_ALPHA;
            }
            case EGPUBlendFactor::ONE_MINUS_SRC_ALPHA:
            {
                return SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
            }
            case EGPUBlendFactor::DST_ALPHA:
            {
                return SDL_GPU_BLENDFACTOR_DST_ALPHA;
            }
            case EGPUBlendFactor::ONE_MINUS_DST_ALPHA:
            default:
            {
                return SDL_GPU_BLENDFACTOR_ONE_MINUS_DST_ALPHA;
            }
        }
    }

    [[nodiscard]] constexpr SDL_GPUBlendOp ToSDLBlendOp(EGPUBlendOp op) noexcept
    {
        switch (op)
        {
            case EGPUBlendOp::SUBTRACT:
            {
                return SDL_GPU_BLENDOP_SUBTRACT;
            }
            case EGPUBlendOp::REVERSE_SUBTRACT:
            {
                return SDL_GPU_BLENDOP_REVERSE_SUBTRACT;
            }
            case EGPUBlendOp::MIN:
            {
                return SDL_GPU_BLENDOP_MIN;
            }
            case EGPUBlendOp::MAX:
            {
                return SDL_GPU_BLENDOP_MAX;
            }
            case EGPUBlendOp::ADD:
            default:
            {
                return SDL_GPU_BLENDOP_ADD;
            }
        }
    }
} // namespace Garage::GraphicsDetail

#endif // GARAGE_GRAPHICS_GPU_SDL_CONVERSIONS_HPP
