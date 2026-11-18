#include "garage/resources/terrain_asset_importer.hpp"

#include <filesystem>
#include <fstream>
#include <ios>
#include <limits>
#include <span>
#include <string>
#include <utility>
#include <vector>

#include "garage/core/allocator.hpp"
#include "garage/core/binary_reader.hpp"
#include "garage/core/types.hpp"

#include "garage/graphics/gpu_device.hpp"
#include "garage/graphics/gpu_mesh.hpp"
#include "garage/graphics/gpu_types.hpp"
#include "garage/graphics/mesh_vertex.hpp"

#include "garage/math/color_rgba.hpp"
#include "garage/math/vector2d.hpp"
#include "garage/math/vector3d.hpp"

#include "garage/resources/terrain_asset.hpp"

namespace Garage
{
    namespace
    {
        struct TerrainImportSettings final
        {
            UInt32            width       = 0;
            UInt32            height      = 0;
            UInt32            bitDepth    = 0;
            Float32           cellSize    = 4.0f;
            Float32           heightScale = 120.0f;
            Vector2D<Float32> uvTiling{1.0f, 1.0f};
        };

        [[nodiscard]] bool HasTerrainExtension(const std::filesystem::path& path) noexcept
        {
            const std::pmr::string EXTENSION(path.extension().string(), Allocator::GetMemoryResource());
            return EXTENSION == ".tukterrain" || EXTENSION == ".TUKTERRAIN";
        }

        template <class TSample>
        [[nodiscard]] Result<std::pmr::vector<Float32>, std::pmr::string>
                ReadAndNormalizeSamples(std::ifstream& file, USize sampleCount, const std::filesystem::path& path)
        {
            std::pmr::vector<TSample> rawSamples(sampleCount, TSample{0}, Allocator::GetMemoryResource());
            if (!static_cast<bool>(file.read(reinterpret_cast<char*>(rawSamples.data()),
                                             static_cast<std::streamsize>(sampleCount * sizeof(TSample)))))
            {
                std::pmr::string error = Allocator::MakeString("높이맵 데이터를 읽지 못했습니다: ");
                error.append(path.generic_string());
                return Result<std::pmr::vector<Float32>, std::pmr::string>::Err(std::move(error));
            }

            constexpr Float32         MAX_SAMPLE_VALUE = static_cast<Float32>(std::numeric_limits<TSample>::max());
            std::pmr::vector<Float32> normalizedHeights(sampleCount, 0.0f, Allocator::GetMemoryResource());
            for (USize index = 0; index < sampleCount; ++index)
            {
                normalizedHeights[index] = static_cast<Float32>(rawSamples[index]) / MAX_SAMPLE_VALUE;
            }

            return Result<std::pmr::vector<Float32>, std::pmr::string>::Ok(std::move(normalizedHeights));
        }

        void BuildTerrainMeshData(std::span<const Float32>      normalizedHeights,
                                  UInt32                        width,
                                  UInt32                        height,
                                  Float32                       cellSize,
                                  Float32                       heightScale,
                                  const Vector2D<Float32>&      uvTiling,
                                  std::pmr::vector<MeshVertex>& outVertices,
                                  std::pmr::vector<UInt32>&     outIndices)
        {
            const USize VERTEX_COUNT = static_cast<USize>(width) * height;
            const USize INDEX_COUNT  = static_cast<USize>(width - 1) * (height - 1) * 6;

            outVertices.resize(VERTEX_COUNT);
            outIndices.clear();
            outIndices.reserve(INDEX_COUNT);

            const Float32 HALF_WIDTH = static_cast<Float32>(width - 1) * cellSize * 0.5f;
            const Float32 HALF_DEPTH = static_cast<Float32>(height - 1) * cellSize * 0.5f;
            const Float32 INV_MAX_X  = 1.0f / static_cast<Float32>(width - 1);
            const Float32 INV_MAX_Z  = 1.0f / static_cast<Float32>(height - 1);

            for (UInt32 z = 0; z < height; ++z)
            {
                for (UInt32 x = 0; x < width; ++x)
                {
                    const USize             INDEX  = static_cast<USize>(z) * width + x;
                    const Float32           POS_X  = static_cast<Float32>(x) * cellSize - HALF_WIDTH;
                    const Float32           POS_Y  = normalizedHeights[INDEX] * heightScale;
                    const Float32           POS_Z  = static_cast<Float32>(z) * cellSize - HALF_DEPTH;
                    const Vector3D<Float32> NORMAL = TerrainAsset::CalculateGridNormal(
                            normalizedHeights, width, height, x, z, cellSize, heightScale);

                    outVertices[INDEX] = MeshVertex{
                            .position = Vector3D<Float32>{POS_X, POS_Y, POS_Z},
                            .normal   = NORMAL,
                            .color    = ColorRGBA<Float32>::GetWhite(),
                            .textureCoordinate =
                                    Vector2D<Float32>{static_cast<Float32>(x) * INV_MAX_X * uvTiling.GetX(),
                                                      static_cast<Float32>(z) * INV_MAX_Z * uvTiling.GetY()},
                    };
                }
            }

            for (UInt32 z = 0; z < height - 1; ++z)
            {
                for (UInt32 x = 0; x < width - 1; ++x)
                {
                    const UInt32 TOP_LEFT     = z * width + x;
                    const UInt32 TOP_RIGHT    = TOP_LEFT + 1;
                    const UInt32 BOTTOM_LEFT  = (z + 1) * width + x;
                    const UInt32 BOTTOM_RIGHT = BOTTOM_LEFT + 1;

                    outIndices.emplace_back(TOP_LEFT);
                    outIndices.emplace_back(BOTTOM_LEFT);
                    outIndices.emplace_back(TOP_RIGHT);

                    outIndices.emplace_back(TOP_RIGHT);
                    outIndices.emplace_back(BOTTOM_LEFT);
                    outIndices.emplace_back(BOTTOM_RIGHT);
                }
            }
        }
    } // namespace

    Result<Allocator::UniquePtr<TerrainAsset>, std::pmr::string>
            TerrainAssetImporter::Import(const std::filesystem::path& path) const
    {
        if (!HasTerrainExtension(path))
        {
            return Result<Allocator::UniquePtr<TerrainAsset>, std::pmr::string>::Err(
                    Allocator::MakeString("지형 로더에는 .tukterrain 높이맵 파일이 필요합니다."));
        }

        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file.is_open())
        {
            std::pmr::string error = Allocator::MakeString("지형 높이맵 파일을 열 수 없습니다: ");
            error.append(path.generic_string());
            return Result<Allocator::UniquePtr<TerrainAsset>, std::pmr::string>::Err(std::move(error));
        }

        const std::streamsize FILE_SIZE = file.tellg();
        if (FILE_SIZE <= 0)
        {
            std::pmr::string error = Allocator::MakeString("지형 높이맵 파일이 비어 있습니다: ");
            error.append(path.generic_string());
            return Result<Allocator::UniquePtr<TerrainAsset>, std::pmr::string>::Err(std::move(error));
        }
        file.seekg(0, std::ios::beg);

        constexpr UInt64      HEADER_SIZE = 3 * sizeof(UInt32) + 4 * sizeof(Float32);
        BinaryReader          reader(file);
        TerrainImportSettings settings{};
        Float32               uvTilingX = 0.0f;
        Float32               uvTilingY = 0.0f;
        if (!reader.Read(settings.width,
                         settings.height,
                         settings.bitDepth,
                         settings.cellSize,
                         settings.heightScale,
                         uvTilingX,
                         uvTilingY))
        {
            std::pmr::string error = Allocator::MakeString("지형 헤더를 읽지 못했습니다: ");
            error.append(path.generic_string());
            return Result<Allocator::UniquePtr<TerrainAsset>, std::pmr::string>::Err(std::move(error));
        }

        if (settings.width < 2 || settings.height < 2 || (settings.bitDepth != 8 && settings.bitDepth != 16) ||
            settings.cellSize <= 0.0f || settings.heightScale < 0.0f || uvTilingX <= 0.0f || uvTilingY <= 0.0f)
        {
            std::pmr::string error = Allocator::MakeString("지형 헤더의 설정값이 유효하지 않습니다: ");
            error.append(path.generic_string());
            return Result<Allocator::UniquePtr<TerrainAsset>, std::pmr::string>::Err(std::move(error));
        }
        settings.uvTiling = Vector2D<Float32>{uvTilingX, uvTilingY};

        const UInt32 WIDTH           = settings.width;
        const UInt32 HEIGHT          = settings.height;
        const bool   IS_16_BIT       = settings.bitDepth == 16;
        const UInt64 SAMPLE_COUNT_64 = static_cast<UInt64>(WIDTH) * HEIGHT;
        const UInt64 SAMPLE_SIZE     = IS_16_BIT ? sizeof(UInt16) : sizeof(UInt8);
        if (SAMPLE_COUNT_64 > BinaryReader::MAX_ELEMENTS ||
            static_cast<UInt64>(FILE_SIZE) != HEADER_SIZE + SAMPLE_COUNT_64 * SAMPLE_SIZE)
        {
            std::pmr::string error =
                    Allocator::MakeString("지형 높이 데이터 크기가 해상도 및 샘플 형식과 일치하지 않습니다: ");
            error.append(path.generic_string());
            return Result<Allocator::UniquePtr<TerrainAsset>, std::pmr::string>::Err(std::move(error));
        }

        const USize SAMPLE_COUNT            = static_cast<USize>(SAMPLE_COUNT_64);
        auto        normalizedHeightsResult = IS_16_BIT ? ReadAndNormalizeSamples<UInt16>(file, SAMPLE_COUNT, path)
                                                        : ReadAndNormalizeSamples<UInt8>(file, SAMPLE_COUNT, path);
        if (!normalizedHeightsResult)
        {
            return Result<Allocator::UniquePtr<TerrainAsset>, std::pmr::string>::Err(
                    std::move(normalizedHeightsResult).GetError());
        }

        std::pmr::vector<MeshVertex> vertices{Allocator::GetMemoryResource()};
        std::pmr::vector<UInt32>     indices{Allocator::GetMemoryResource()};
        BuildTerrainMeshData(*normalizedHeightsResult,
                             WIDTH,
                             HEIGHT,
                             settings.cellSize,
                             settings.heightScale,
                             settings.uvTiling,
                             vertices,
                             indices);

        auto gpuMeshResult = device.CreateMesh(std::as_bytes(std::span<const MeshVertex>(vertices)),
                                               sizeof(MeshVertex),
                                               std::as_bytes(std::span<const UInt32>(indices)),
                                               EGPUIndexFormat::UINT32,
                                               EGPUPrimitiveTopology::TRIANGLE_LIST);
        if (!gpuMeshResult)
        {
            return Result<Allocator::UniquePtr<TerrainAsset>, std::pmr::string>::Err(
                    std::move(gpuMeshResult).GetError());
        }

        return Result<Allocator::UniquePtr<TerrainAsset>, std::pmr::string>::Ok(
                Allocator::MakeUnique<TerrainAsset>(path,
                                                    WIDTH,
                                                    HEIGHT,
                                                    IS_16_BIT,
                                                    settings.cellSize,
                                                    settings.heightScale,
                                                    settings.uvTiling,
                                                    std::move(*normalizedHeightsResult),
                                                    device,
                                                    gpuMeshResult.GetValue()));
    }
} // namespace Garage
