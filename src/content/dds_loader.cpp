#include "dds_loader.hpp"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <ios>
#include <limits>
#include <utility>

#include "garage/core/common.hpp"
#include "garage/core/common/containers.hpp"

#include "garage/graphics/gpu_device.hpp"
#include "garage/graphics/gpu_texture2d.hpp"
#include "garage/graphics/gpu_types.hpp"

namespace Garage::ContentDetail
{
    namespace
    {
        constexpr UInt32 DDS_MAGIC                 = 0x20534444u; // "DDS "
        constexpr UInt32 DDS_HEADER_SIZE           = 124u;
        constexpr UInt32 DDS_PIXELFORMAT_SIZE      = 32u;
        constexpr UInt32 DDS_FLAG_MIPMAPCOUNT      = 0x00020000u;
        constexpr UInt32 DDS_FLAG_DEPTH            = 0x00800000u;
        constexpr UInt32 DDS_CAPS2_CUBEMAP         = 0x00000200u;
        constexpr UInt32 DDS_CAPS2_CUBEMAP_ALLFACE = 0x0000FC00u;
        constexpr UInt32 DDS_CAPS2_VOLUME          = 0x00200000u;

        constexpr UInt32 DDPF_ALPHAPIXELS = 0x00000001u;
        constexpr UInt32 DDPF_ALPHA       = 0x00000002u;
        constexpr UInt32 DDPF_FOURCC      = 0x00000004u;
        constexpr UInt32 DDPF_RGB         = 0x00000040u;
        constexpr UInt32 DDPF_LUMINANCE   = 0x00020000u;

        constexpr UInt32 DDS_RESOURCE_DIMENSION_TEXTURE2D = 3u;
        constexpr UInt32 DDS_RESOURCE_MISC_TEXTURECUBE    = 0x4u;

        consteval UInt32 MakeFourCC(char ch0, char ch1, char ch2, char ch3) noexcept
        {
            return static_cast<UInt32>(static_cast<UInt8>(ch0)) | (static_cast<UInt32>(static_cast<UInt8>(ch1)) << 8u) |
                    (static_cast<UInt32>(static_cast<UInt8>(ch2)) << 16u) |
                    (static_cast<UInt32>(static_cast<UInt8>(ch3)) << 24u);
        }

        constexpr UInt32 FOURCC_DXT1 = MakeFourCC('D', 'X', 'T', '1');
        constexpr UInt32 FOURCC_DXT2 = MakeFourCC('D', 'X', 'T', '2');
        constexpr UInt32 FOURCC_DXT3 = MakeFourCC('D', 'X', 'T', '3');
        constexpr UInt32 FOURCC_DXT4 = MakeFourCC('D', 'X', 'T', '4');
        constexpr UInt32 FOURCC_DXT5 = MakeFourCC('D', 'X', 'T', '5');
        constexpr UInt32 FOURCC_ATI1 = MakeFourCC('A', 'T', 'I', '1');
        constexpr UInt32 FOURCC_BC4U = MakeFourCC('B', 'C', '4', 'U');
        constexpr UInt32 FOURCC_ATI2 = MakeFourCC('A', 'T', 'I', '2');
        constexpr UInt32 FOURCC_BC5U = MakeFourCC('B', 'C', '5', 'U');
        constexpr UInt32 FOURCC_DX10 = MakeFourCC('D', 'X', '1', '0');

        struct DDSPixelFormat final
        {
            UInt32 size        = 0;
            UInt32 flags       = 0;
            UInt32 fourCC      = 0;
            UInt32 rgbBitCount = 0;
            UInt32 rBitMask    = 0;
            UInt32 gBitMask    = 0;
            UInt32 bBitMask    = 0;
            UInt32 aBitMask    = 0;
        };

        struct DDSHeader final
        {
            UInt32            size              = 0;
            UInt32            flags             = 0;
            UInt32            height            = 0;
            UInt32            width             = 0;
            UInt32            pitchOrLinearSize = 0;
            UInt32            depth             = 0;
            UInt32            mipMapCount       = 0;
            Array<UInt32, 11> reserved1{};
            DDSPixelFormat    pixelFormat{};
            UInt32            caps      = 0;
            UInt32            caps2     = 0;
            UInt32            caps3     = 0;
            UInt32            caps4     = 0;
            UInt32            reserved2 = 0;
        };

        struct DDSHeaderDXT10 final
        {
            UInt32 dxgiFormat        = 0;
            UInt32 resourceDimension = 0;
            UInt32 miscFlag          = 0;
            UInt32 arraySize         = 0;
            UInt32 miscFlags2        = 0;
        };

        static_assert(sizeof(DDSPixelFormat) == DDS_PIXELFORMAT_SIZE);
        static_assert(sizeof(DDSHeader) == DDS_HEADER_SIZE);
        static_assert(sizeof(DDSHeaderDXT10) == 20u);

        enum class ESourcePixelConversion : UInt8
        {
            NONE,
            FORCE_ALPHA_4BYTE,
            EXPAND_RGB24_TO_RGBA32,
            EXPAND_BGR24_TO_RGBA32,
        };

        struct ResolvedDDSFormat final
        {
            EGPUTextureFormat      gpuFormat         = EGPUTextureFormat::UNKNOWN;
            bool                   isBlockCompressed = false;
            UInt32                 bytesPerUnit      = 0; // 블록 압축이면 블록 바이트, 아니면 픽셀 바이트
            UInt32                 sourceBytesPerPx  = 0;
            ESourcePixelConversion conversion        = ESourcePixelConversion::NONE;
        };

        [[nodiscard]] bool MapDXGIFormat(UInt32 dxgiFormat, ResolvedDDSFormat& outFormat) noexcept
        {
            switch (dxgiFormat)
            {
                case 2u: // DXGI_FORMAT_R32G32B32A32_FLOAT
                    outFormat = {EGPUTextureFormat::R32G32B32A32_FLOAT, false, 16u, 16u, ESourcePixelConversion::NONE};
                    return true;
                case 10u: // DXGI_FORMAT_R16G16B16A16_FLOAT
                    outFormat = {EGPUTextureFormat::R16G16B16A16_FLOAT, false, 8u, 8u, ESourcePixelConversion::NONE};
                    return true;
                case 28u: // DXGI_FORMAT_R8G8B8A8_UNORM
                    outFormat = {EGPUTextureFormat::R8G8B8A8_UNORM, false, 4u, 4u, ESourcePixelConversion::NONE};
                    return true;
                case 29u: // DXGI_FORMAT_R8G8B8A8_UNORM_SRGB
                    outFormat = {EGPUTextureFormat::R8G8B8A8_UNORM_SRGB, false, 4u, 4u, ESourcePixelConversion::NONE};
                    return true;
                case 35u: // DXGI_FORMAT_R16G16_UNORM
                    outFormat = {EGPUTextureFormat::R16G16_UNORM, false, 4u, 4u, ESourcePixelConversion::NONE};
                    return true;
                case 49u: // DXGI_FORMAT_R8G8_UNORM
                    outFormat = {EGPUTextureFormat::R8G8_UNORM, false, 2u, 2u, ESourcePixelConversion::NONE};
                    return true;
                case 61u: // DXGI_FORMAT_R8_UNORM
                    outFormat = {EGPUTextureFormat::R8_UNORM, false, 1u, 1u, ESourcePixelConversion::NONE};
                    return true;
                case 71u: // DXGI_FORMAT_BC1_UNORM
                    outFormat = {EGPUTextureFormat::BC1_RGBA_UNORM, true, 8u, 0u, ESourcePixelConversion::NONE};
                    return true;
                case 72u: // DXGI_FORMAT_BC1_UNORM_SRGB
                    outFormat = {EGPUTextureFormat::BC1_RGBA_UNORM_SRGB, true, 8u, 0u, ESourcePixelConversion::NONE};
                    return true;
                case 74u: // DXGI_FORMAT_BC2_UNORM
                    outFormat = {EGPUTextureFormat::BC2_RGBA_UNORM, true, 16u, 0u, ESourcePixelConversion::NONE};
                    return true;
                case 75u: // DXGI_FORMAT_BC2_UNORM_SRGB
                    outFormat = {EGPUTextureFormat::BC2_RGBA_UNORM_SRGB, true, 16u, 0u, ESourcePixelConversion::NONE};
                    return true;
                case 77u: // DXGI_FORMAT_BC3_UNORM
                    outFormat = {EGPUTextureFormat::BC3_RGBA_UNORM, true, 16u, 0u, ESourcePixelConversion::NONE};
                    return true;
                case 78u: // DXGI_FORMAT_BC3_UNORM_SRGB
                    outFormat = {EGPUTextureFormat::BC3_RGBA_UNORM_SRGB, true, 16u, 0u, ESourcePixelConversion::NONE};
                    return true;
                case 80u: // DXGI_FORMAT_BC4_UNORM
                    outFormat = {EGPUTextureFormat::BC4_R_UNORM, true, 8u, 0u, ESourcePixelConversion::NONE};
                    return true;
                case 83u: // DXGI_FORMAT_BC5_UNORM
                    outFormat = {EGPUTextureFormat::BC5_RG_UNORM, true, 16u, 0u, ESourcePixelConversion::NONE};
                    return true;
                case 87u: // DXGI_FORMAT_B8G8R8A8_UNORM
                    outFormat = {EGPUTextureFormat::B8G8R8A8_UNORM, false, 4u, 4u, ESourcePixelConversion::NONE};
                    return true;
                case 88u: // DXGI_FORMAT_B8G8R8X8_UNORM
                    outFormat = {EGPUTextureFormat::B8G8R8A8_UNORM,
                                 false,
                                 4u,
                                 4u,
                                 ESourcePixelConversion::FORCE_ALPHA_4BYTE};
                    return true;
                case 91u: // DXGI_FORMAT_B8G8R8A8_UNORM_SRGB
                    outFormat = {EGPUTextureFormat::B8G8R8A8_UNORM_SRGB, false, 4u, 4u, ESourcePixelConversion::NONE};
                    return true;
                case 93u: // DXGI_FORMAT_B8G8R8X8_UNORM_SRGB
                    outFormat = {EGPUTextureFormat::B8G8R8A8_UNORM_SRGB,
                                 false,
                                 4u,
                                 4u,
                                 ESourcePixelConversion::FORCE_ALPHA_4BYTE};
                    return true;
                case 95u: // DXGI_FORMAT_BC6H_UF16
                    outFormat = {EGPUTextureFormat::BC6H_RGB_UFLOAT, true, 16u, 0u, ESourcePixelConversion::NONE};
                    return true;
                case 96u: // DXGI_FORMAT_BC6H_SF16
                    outFormat = {EGPUTextureFormat::BC6H_RGB_FLOAT, true, 16u, 0u, ESourcePixelConversion::NONE};
                    return true;
                case 98u: // DXGI_FORMAT_BC7_UNORM
                    outFormat = {EGPUTextureFormat::BC7_RGBA_UNORM, true, 16u, 0u, ESourcePixelConversion::NONE};
                    return true;
                case 99u: // DXGI_FORMAT_BC7_UNORM_SRGB
                    outFormat = {EGPUTextureFormat::BC7_RGBA_UNORM_SRGB, true, 16u, 0u, ESourcePixelConversion::NONE};
                    return true;
                default:
                    return false;
            }
        }

        [[nodiscard]] bool ResolveLegacyFormat(const DDSPixelFormat& pf, ResolvedDDSFormat& outFormat) noexcept
        {
            if ((pf.flags & DDPF_FOURCC) != 0u)
            {
                switch (pf.fourCC)
                {
                    case FOURCC_DXT1:
                        outFormat = {EGPUTextureFormat::BC1_RGBA_UNORM, true, 8u, 0u, ESourcePixelConversion::NONE};
                        return true;
                    case FOURCC_DXT2:
                    case FOURCC_DXT3:
                        outFormat = {EGPUTextureFormat::BC2_RGBA_UNORM, true, 16u, 0u, ESourcePixelConversion::NONE};
                        return true;
                    case FOURCC_DXT4:
                    case FOURCC_DXT5:
                        outFormat = {EGPUTextureFormat::BC3_RGBA_UNORM, true, 16u, 0u, ESourcePixelConversion::NONE};
                        return true;
                    case FOURCC_ATI1:
                    case FOURCC_BC4U:
                        outFormat = {EGPUTextureFormat::BC4_R_UNORM, true, 8u, 0u, ESourcePixelConversion::NONE};
                        return true;
                    case FOURCC_ATI2:
                    case FOURCC_BC5U:
                        outFormat = {EGPUTextureFormat::BC5_RG_UNORM, true, 16u, 0u, ESourcePixelConversion::NONE};
                        return true;
                    case 113u: // D3DFMT_A16B16G16R16F
                        outFormat = {
                                EGPUTextureFormat::R16G16B16A16_FLOAT, false, 8u, 8u, ESourcePixelConversion::NONE};
                        return true;
                    case 116u: // D3DFMT_A32B32G32R32F
                        outFormat = {
                                EGPUTextureFormat::R32G32B32A32_FLOAT, false, 16u, 16u, ESourcePixelConversion::NONE};
                        return true;
                    default:
                        return false;
                }
            }

            if ((pf.flags & DDPF_RGB) != 0u)
            {
                if (pf.rgbBitCount == 32u)
                {
                    if (pf.rBitMask == 0x000000FFu && pf.gBitMask == 0x0000FF00u && pf.bBitMask == 0x00FF0000u)
                    {
                        const ESourcePixelConversion CONVERSION = (pf.aBitMask == 0xFF000000u)
                                ? ESourcePixelConversion::NONE
                                : ESourcePixelConversion::FORCE_ALPHA_4BYTE;
                        outFormat = {EGPUTextureFormat::R8G8B8A8_UNORM, false, 4u, 4u, CONVERSION};
                        return true;
                    }

                    if (pf.rBitMask == 0x00FF0000u && pf.gBitMask == 0x0000FF00u && pf.bBitMask == 0x000000FFu)
                    {
                        const ESourcePixelConversion CONVERSION = (pf.aBitMask == 0xFF000000u)
                                ? ESourcePixelConversion::NONE
                                : ESourcePixelConversion::FORCE_ALPHA_4BYTE;
                        outFormat = {EGPUTextureFormat::B8G8R8A8_UNORM, false, 4u, 4u, CONVERSION};
                        return true;
                    }
                }
                else if (pf.rgbBitCount == 24u)
                {
                    if (pf.rBitMask == 0x00FF0000u && pf.gBitMask == 0x0000FF00u && pf.bBitMask == 0x000000FFu)
                    {
                        outFormat = {EGPUTextureFormat::R8G8B8A8_UNORM,
                                     false,
                                     4u,
                                     3u,
                                     ESourcePixelConversion::EXPAND_BGR24_TO_RGBA32};
                        return true;
                    }

                    if (pf.rBitMask == 0x000000FFu && pf.gBitMask == 0x0000FF00u && pf.bBitMask == 0x00FF0000u)
                    {
                        outFormat = {EGPUTextureFormat::R8G8B8A8_UNORM,
                                     false,
                                     4u,
                                     3u,
                                     ESourcePixelConversion::EXPAND_RGB24_TO_RGBA32};
                        return true;
                    }
                }
            }

            if ((pf.flags & (DDPF_LUMINANCE | DDPF_ALPHA)) != 0u && pf.rgbBitCount == 8u)
            {
                outFormat = {EGPUTextureFormat::R8_UNORM, false, 1u, 1u, ESourcePixelConversion::NONE};
                return true;
            }

            return false;
        }

        [[nodiscard]] UInt64
                CalculateSourceSubresourceSize(UInt32 width, UInt32 height, const ResolvedDDSFormat& format) noexcept
        {
            if (format.isBlockCompressed)
            {
                const UInt64 BLOCKS_WIDE = std::max<UInt64>(1u, (static_cast<UInt64>(width) + 3u) / 4u);
                const UInt64 BLOCKS_HIGH = std::max<UInt64>(1u, (static_cast<UInt64>(height) + 3u) / 4u);
                return BLOCKS_WIDE * BLOCKS_HIGH * format.bytesPerUnit;
            }

            return static_cast<UInt64>(width) * height * format.sourceBytesPerPx;
        }

        [[nodiscard]] UInt64 CalculateDestinationSubresourceSize(UInt32                   width,
                                                                 UInt32                   height,
                                                                 const ResolvedDDSFormat& format) noexcept
        {
            if (format.isBlockCompressed)
            {
                return CalculateSourceSubresourceSize(width, height, format);
            }

            return static_cast<UInt64>(width) * height * format.bytesPerUnit;
        }

        void ConvertSubresourcePixels(Span<const Byte>         src,
                                      Span<Byte>               dst,
                                      UInt32                   width,
                                      UInt32                   height,
                                      const ResolvedDDSFormat& format) noexcept
        {
            switch (format.conversion)
            {
                case ESourcePixelConversion::FORCE_ALPHA_4BYTE:
                {
                    std::memcpy(dst.data(), src.data(), dst.size());
                    const USize PIXEL_COUNT = static_cast<USize>(width) * height;
                    for (USize i = 0; i < PIXEL_COUNT; ++i)
                    {
                        dst[i * 4u + 3u] = static_cast<Byte>(0xFFu);
                    }
                    break;
                }
                case ESourcePixelConversion::EXPAND_RGB24_TO_RGBA32:
                {
                    const USize PIXEL_COUNT = static_cast<USize>(width) * height;
                    for (USize i = 0; i < PIXEL_COUNT; ++i)
                    {
                        dst[i * 4u + 0u] = src[i * 3u + 0u];
                        dst[i * 4u + 1u] = src[i * 3u + 1u];
                        dst[i * 4u + 2u] = src[i * 3u + 2u];
                        dst[i * 4u + 3u] = static_cast<Byte>(0xFFu);
                    }
                    break;
                }
                case ESourcePixelConversion::EXPAND_BGR24_TO_RGBA32:
                {
                    const USize PIXEL_COUNT = static_cast<USize>(width) * height;
                    for (USize i = 0; i < PIXEL_COUNT; ++i)
                    {
                        dst[i * 4u + 0u] = src[i * 3u + 2u];
                        dst[i * 4u + 1u] = src[i * 3u + 1u];
                        dst[i * 4u + 2u] = src[i * 3u + 0u];
                        dst[i * 4u + 3u] = static_cast<Byte>(0xFFu);
                    }
                    break;
                }
                case ESourcePixelConversion::NONE:
                default:
                {
                    std::memcpy(dst.data(), src.data(), dst.size());
                    break;
                }
            }
        }
    } // namespace

    Result<IGPUTexture2D*, String>
            LoadDDSTexture(const std::filesystem::path& path, bool expectCubemap, IGPUDevice& device)
    {
        String extension(path.extension().string());
        for (char& character : extension)
        {
            character = static_cast<char>(std::tolower(static_cast<unsigned char>(character)));
        }

        if (extension != ".dds")
        {
            String error("지원하지 않는 이미지 파일 확장자입니다: ");
            error.append(extension);
            return Result<IGPUTexture2D*, String>::Err(std::move(error));
        }

        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file.is_open())
        {
            String error("이미지 파일을 열 수 없습니다: ");
            error.append(path.generic_string());
            return Result<IGPUTexture2D*, String>::Err(std::move(error));
        }

        const std::streamsize FILE_SIZE = file.tellg();
        constexpr USize       MIN_SIZE  = sizeof(UInt32) + sizeof(DDSHeader);
        if (FILE_SIZE < static_cast<std::streamsize>(MIN_SIZE))
        {
            String error("DDS 헤더 크기가 유효하지 않습니다: ");
            error.append(path.generic_string());
            return Result<IGPUTexture2D*, String>::Err(std::move(error));
        }

        file.seekg(0, std::ios::beg);
        Vector<Byte> fileBytes(static_cast<USize>(FILE_SIZE));
        if (!file.read(reinterpret_cast<char*>(fileBytes.data()), FILE_SIZE))
        {
            String error("이미지 파일 읽기에 실패했습니다: ");
            error.append(path.generic_string());
            return Result<IGPUTexture2D*, String>::Err(std::move(error));
        }

        UInt32 magic = 0u;
        std::memcpy(&magic, fileBytes.data(), sizeof(magic));
        if (magic != DDS_MAGIC)
        {
            return Result<IGPUTexture2D*, String>::Err(String("유효한 DDS 시그니처가 아닙니다."));
        }

        DDSHeader header{};
        std::memcpy(&header, fileBytes.data() + sizeof(UInt32), sizeof(DDSHeader));
        if (header.size != DDS_HEADER_SIZE || header.pixelFormat.size != DDS_PIXELFORMAT_SIZE || header.width == 0u ||
            header.height == 0u)
        {
            return Result<IGPUTexture2D*, String>::Err(String("DDS 헤더 정보가 유효하지 않습니다."));
        }

        USize             dataOffset     = MIN_SIZE;
        bool              isCubemap      = false;
        UInt32            layerCount     = 1u;
        ResolvedDDSFormat resolvedFormat = {};

        const bool HAS_DX10 =
                ((header.pixelFormat.flags & DDPF_FOURCC) != 0u) && (header.pixelFormat.fourCC == FOURCC_DX10);
        if (HAS_DX10)
        {
            if (fileBytes.size() < dataOffset + sizeof(DDSHeaderDXT10))
            {
                return Result<IGPUTexture2D*, String>::Err(String("DDS DX10 확장 헤더가 손상되었습니다."));
            }

            DDSHeaderDXT10 dx10{};
            std::memcpy(&dx10, fileBytes.data() + dataOffset, sizeof(DDSHeaderDXT10));
            dataOffset += sizeof(DDSHeaderDXT10);

            if (dx10.resourceDimension != DDS_RESOURCE_DIMENSION_TEXTURE2D || dx10.arraySize == 0u)
            {
                return Result<IGPUTexture2D*, String>::Err(String("DDS 파일에 2D 텍스처 리소스가 필요합니다."));
            }

            isCubemap  = (dx10.miscFlag & DDS_RESOURCE_MISC_TEXTURECUBE) != 0u;
            layerCount = isCubemap ? (dx10.arraySize * 6u) : dx10.arraySize;
            if (!MapDXGIFormat(dx10.dxgiFormat, resolvedFormat))
            {
                return Result<IGPUTexture2D*, String>::Err(String("지원하지 않는 DXGI 텍스처 포맷입니다."));
            }
        }
        else
        {
            if ((header.caps2 & DDS_CAPS2_VOLUME) != 0u || ((header.flags & DDS_FLAG_DEPTH) != 0u && header.depth > 1u))
            {
                return Result<IGPUTexture2D*, String>::Err(String("볼륨 텍스처 DDS는 지원하지 않습니다."));
            }

            isCubemap = (header.caps2 & DDS_CAPS2_CUBEMAP) != 0u;
            if (isCubemap)
            {
                if ((header.caps2 & DDS_CAPS2_CUBEMAP_ALLFACE) != DDS_CAPS2_CUBEMAP_ALLFACE)
                {
                    return Result<IGPUTexture2D*, String>::Err(String("큐브맵 DDS에 6개 면이 모두 포함되어야 합니다."));
                }
                layerCount = 6u;
            }

            if (!ResolveLegacyFormat(header.pixelFormat, resolvedFormat))
            {
                return Result<IGPUTexture2D*, String>::Err(String("지원하지 않는 DDS 픽셀 포맷입니다."));
            }
        }

        if (expectCubemap)
        {
            if (!isCubemap || layerCount != 6u || header.width != header.height)
            {
                return Result<IGPUTexture2D*, String>::Err(String("DDS 파일에 단일 큐브맵이 필요합니다."));
            }
        }
        else
        {
            if (isCubemap || layerCount != 1u)
            {
                return Result<IGPUTexture2D*, String>::Err(String("DDS 파일에 일반 2D 텍스처가 필요합니다."));
            }
        }

        const UInt32 MIP_LEVELS =
                ((header.flags & DDS_FLAG_MIPMAPCOUNT) != 0u && header.mipMapCount > 0u) ? header.mipMapCount : 1u;

        UInt64 expectedSourceBytes = 0u;
        UInt64 totalDestBytes      = 0u;
        for (UInt32 layer = 0u; layer < layerCount; ++layer)
        {
            for (UInt32 mip = 0u; mip < MIP_LEVELS; ++mip)
            {
                const UInt32 MIP_WIDTH  = std::max(1u, header.width >> mip);
                const UInt32 MIP_HEIGHT = std::max(1u, header.height >> mip);
                expectedSourceBytes += CalculateSourceSubresourceSize(MIP_WIDTH, MIP_HEIGHT, resolvedFormat);
                totalDestBytes += CalculateDestinationSubresourceSize(MIP_WIDTH, MIP_HEIGHT, resolvedFormat);
            }
        }

        if (fileBytes.size() - dataOffset < expectedSourceBytes || totalDestBytes > std::numeric_limits<USize>::max())
        {
            return Result<IGPUTexture2D*, String>::Err(String("DDS 서브리소스 데이터가 헤더 크기보다 작습니다."));
        }

        Vector<Byte>                      pixelStorage;
        Vector<GPUTextureSubresourceData> subresources;
        subresources.reserve(static_cast<USize>(layerCount) * MIP_LEVELS);

        const bool NEEDS_CONVERSION = resolvedFormat.conversion != ESourcePixelConversion::NONE;
        if (NEEDS_CONVERSION)
        {
            pixelStorage.resize(static_cast<USize>(totalDestBytes));
        }

        USize srcOffset = dataOffset;
        USize dstOffset = 0u;
        for (UInt32 layer = 0u; layer < layerCount; ++layer)
        {
            for (UInt32 mip = 0u; mip < MIP_LEVELS; ++mip)
            {
                const UInt32 MIP_WIDTH  = std::max(1u, header.width >> mip);
                const UInt32 MIP_HEIGHT = std::max(1u, header.height >> mip);
                const USize  SRC_SIZE =
                        static_cast<USize>(CalculateSourceSubresourceSize(MIP_WIDTH, MIP_HEIGHT, resolvedFormat));
                const USize DST_SIZE =
                        static_cast<USize>(CalculateDestinationSubresourceSize(MIP_WIDTH, MIP_HEIGHT, resolvedFormat));

                const Span<const Byte> SRC_SPAN(fileBytes.data() + srcOffset, SRC_SIZE);
                if (NEEDS_CONVERSION)
                {
                    const Span<Byte> DST_SPAN(pixelStorage.data() + dstOffset, DST_SIZE);
                    ConvertSubresourcePixels(SRC_SPAN, DST_SPAN, MIP_WIDTH, MIP_HEIGHT, resolvedFormat);
                    subresources.push_back(GPUTextureSubresourceData{
                            .data     = DST_SPAN,
                            .mipLevel = mip,
                            .layer    = layer,
                    });
                    dstOffset += DST_SIZE;
                }
                else
                {
                    subresources.push_back(GPUTextureSubresourceData{
                            .data     = SRC_SPAN,
                            .mipLevel = mip,
                            .layer    = layer,
                    });
                }

                srcOffset += SRC_SIZE;
            }
        }

        const GPUTexture2DCreateInfo CREATE_INFO{
                .type       = expectCubemap ? EGPUTextureType::TEXTURE_CUBE : EGPUTextureType::TEXTURE_2D,
                .format     = resolvedFormat.gpuFormat,
                .usage      = static_cast<UInt32>(EGPUTextureUsage::SAMPLER),
                .width      = header.width,
                .height     = header.height,
                .layerCount = layerCount,
                .mipLevels  = MIP_LEVELS,
        };

        return device.CreateTexture2D(CREATE_INFO, subresources);
    }
} // namespace Garage::ContentDetail
