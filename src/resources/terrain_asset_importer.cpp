#include "garage/resources/terrain_asset_importer.hpp"

#include <filesystem>
#include <source_location>
#include <span>
#include <utility>

#include <rapidjson/document.h>

#include "garage/core/common.hpp"
#include "garage/core/common/containers.hpp"
#include "garage/core/memory.hpp"

#include "garage/graphics/gpu_device.hpp"
#include "garage/graphics/gpu_mesh.hpp"
#include "garage/graphics/gpu_types.hpp"
#include "garage/graphics/mesh_vertex.hpp"

#include "garage/math/color_rgba.hpp"
#include "garage/math/vector2d.hpp"
#include "garage/math/vector3d.hpp"

#include "garage/resources/terrain_asset.hpp"

#include "garage/utilities/json_utility.hpp"

namespace Garage
{
    namespace
    {
        struct TerrainImportSettings final
        {
            UInt32            width       = 0;
            UInt32            height      = 0;
            UInt32            bitDepth    = 16;
            Float32           cellSize    = 4.0f;
            Float32           heightScale = 120.0f;
            Vector2D<Float32> uvTiling{1.0f, 1.0f};
        };

        void BuildTerrainMeshData(Span<const Float32>      normalizedHeights,
                                  UInt32                   width,
                                  UInt32                   height,
                                  Float32                  cellSize,
                                  Float32                  heightScale,
                                  const Vector2D<Float32>& uvTiling,
                                  Vector<MeshVertex>&      outVertices,
                                  Vector<UInt32>&          outIndices)
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

    Result<UniquePtr<TerrainAsset>, String> TerrainAssetImporter::Import(const std::filesystem::path& path) const
    {
        if (!JsonUtility::HasJsonExtension(path))
        {
            return Result<UniquePtr<TerrainAsset>, String>::Err(
                    String("지형 로더에는 .json 높이맵 파일이 필요합니다."));
        }

        auto documentResult = JsonUtility::LoadDocument(path);
        if (!documentResult)
        {
            return Result<UniquePtr<TerrainAsset>, String>::Err(std::move(documentResult).GetError());
        }

        const rapidjson::Value& ROOT = *documentResult;

        TerrainImportSettings settings{};
        Array<Float32, 2>     uvTiling{1.0f, 1.0f};
        if (!JsonUtility::ReadUInt32(ROOT, "width", settings.width) ||
            !JsonUtility::ReadUInt32(ROOT, "height", settings.height) ||
            !JsonUtility::ReadOptionalUInt32(ROOT, "bitDepth", settings.bitDepth) ||
            !JsonUtility::ReadOptionalFloat32(ROOT, "cellSize", settings.cellSize) ||
            !JsonUtility::ReadOptionalFloat32(ROOT, "heightScale", settings.heightScale) ||
            !JsonUtility::ReadOptionalFloatArray(ROOT, "uvTiling", uvTiling))
        {
            String error("지형 헤더를 읽지 못했습니다: ");
            error.append(path.generic_string());
            return Result<UniquePtr<TerrainAsset>, String>::Err(std::move(error));
        }

        if (settings.width < 2 || settings.height < 2 || (settings.bitDepth != 8 && settings.bitDepth != 16) ||
            settings.cellSize <= 0.0f || settings.heightScale < 0.0f || uvTiling[0] <= 0.0f || uvTiling[1] <= 0.0f)
        {
            String error("지형 헤더의 설정값이 유효하지 않습니다: ");
            error.append(path.generic_string());
            return Result<UniquePtr<TerrainAsset>, String>::Err(std::move(error));
        }
        settings.uvTiling = Vector2D<Float32>{uvTiling[0], uvTiling[1]};

        const UInt32 WIDTH           = settings.width;
        const UInt32 HEIGHT          = settings.height;
        const bool   IS_16_BIT       = settings.bitDepth == 16;
        const UInt64 SAMPLE_COUNT_64 = static_cast<UInt64>(WIDTH) * HEIGHT;

        const rapidjson::Value* heightsArray = nullptr;
        if (!JsonUtility::ReadArray(ROOT, "heights", heightsArray) || SAMPLE_COUNT_64 > JsonUtility::MAX_ELEMENTS ||
            heightsArray->Size() != static_cast<USize>(SAMPLE_COUNT_64))
        {
            String error("지형 높이 데이터 개수가 해상도와 일치하지 않습니다: ");
            error.append(path.generic_string());
            return Result<UniquePtr<TerrainAsset>, String>::Err(std::move(error));
        }

        Vector<Float32> normalizedHeights;
        normalizedHeights.reserve(static_cast<USize>(SAMPLE_COUNT_64));
        for (const rapidjson::Value& sampleValue : heightsArray->GetArray())
        {
            Float32 heightSample = 0.0f;
            if (!JsonUtility::TryReadFloat32(sampleValue, heightSample) || heightSample < 0.0f || heightSample > 1.0f)
            {
                String error("지형 높이 샘플 값이 유효하지 않습니다: ");
                error.append(path.generic_string());
                return Result<UniquePtr<TerrainAsset>, String>::Err(std::move(error));
            }

            normalizedHeights.push_back(heightSample);
        }

        Vector<MeshVertex> vertices;
        Vector<UInt32>     indices;
        BuildTerrainMeshData(normalizedHeights,
                             WIDTH,
                             HEIGHT,
                             settings.cellSize,
                             settings.heightScale,
                             settings.uvTiling,
                             vertices,
                             indices);

        auto gpuMeshResult = device.CreateMesh(std::as_bytes(Span<const MeshVertex>(vertices)),
                                               sizeof(MeshVertex),
                                               std::as_bytes(Span<const UInt32>(indices)),
                                               EGPUIndexFormat::UINT32,
                                               EGPUPrimitiveTopology::TRIANGLE_LIST);
        if (!gpuMeshResult)
        {
            return Result<UniquePtr<TerrainAsset>, String>::Err(std::move(gpuMeshResult).GetError());
        }

        return Result<UniquePtr<TerrainAsset>, String>::Ok(MakeUniqueAt<TerrainAsset>(std::source_location::current(),
                                                                                      path,
                                                                                      WIDTH,
                                                                                      HEIGHT,
                                                                                      IS_16_BIT,
                                                                                      settings.cellSize,
                                                                                      settings.heightScale,
                                                                                      settings.uvTiling,
                                                                                      std::move(normalizedHeights),
                                                                                      device,
                                                                                      gpuMeshResult.GetValue()));
    }
} // namespace Garage
