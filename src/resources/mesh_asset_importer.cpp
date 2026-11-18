#include "garage/resources/mesh_asset_importer.hpp"

#include <array>
#include <filesystem>
#include <span>
#include <string>
#include <utility>
#include <vector>

#include <rapidjson/document.h>

#include "garage/core/allocator.hpp"
#include "garage/core/types.hpp"

#include "garage/graphics/gpu_device.hpp"
#include "garage/graphics/gpu_mesh.hpp"
#include "garage/graphics/gpu_types.hpp"
#include "garage/graphics/mesh_vertex.hpp"

#include "garage/math/color_rgba.hpp"
#include "garage/math/vector2d.hpp"
#include "garage/math/vector3d.hpp"

#include "garage/resources/mesh_asset.hpp"

#include "garage/utilities/json_utility.hpp"

namespace Garage
{
    namespace
    {
        template <USize N, class TElement, class TFactory>
        [[nodiscard]] bool ReadComponentVectorArray(const rapidjson::Value&     arrayValue,
                                                    std::pmr::vector<TElement>& out,
                                                    TFactory                    factory)
        {
            out.clear();
            out.reserve(arrayValue.Size());
            for (const rapidjson::Value& item : arrayValue.GetArray())
            {
                std::array<Float32, N> components{};
                if (!JsonUtility::TryReadFloatArray(item, components))
                {
                    return false;
                }

                out.push_back(factory(components));
            }

            return true;
        }
    } // namespace

    Result<Allocator::UniquePtr<MeshAsset>, std::pmr::string>
            MeshAssetImporter::Import(const std::filesystem::path& path) const
    {
        if (!JsonUtility::HasJsonExtension(path))
        {
            return Result<Allocator::UniquePtr<MeshAsset>, std::pmr::string>::Err(
                    Allocator::MakeString("메시 로더에는 .json 파일이 필요합니다."));
        }

        auto documentResult = JsonUtility::LoadDocument(path);
        if (!documentResult)
        {
            return Result<Allocator::UniquePtr<MeshAsset>, std::pmr::string>::Err(std::move(documentResult).GetError());
        }

        const rapidjson::Value& ROOT           = *documentResult;
        const rapidjson::Value* positionsArray = nullptr;
        const rapidjson::Value* normalsArray   = nullptr;
        const rapidjson::Value* uvsArray       = nullptr;
        const rapidjson::Value* colorsArray    = nullptr;
        const rapidjson::Value* indicesArray   = nullptr;

        if (!JsonUtility::ReadArray(ROOT, "positions", positionsArray) ||
            !JsonUtility::ReadArray(ROOT, "normals", normalsArray) ||
            !JsonUtility::ReadOptionalArray(ROOT, "uvs", uvsArray) ||
            !JsonUtility::ReadOptionalArray(ROOT, "colors", colorsArray) ||
            !JsonUtility::ReadArray(ROOT, "indices", indicesArray))
        {
            return Result<Allocator::UniquePtr<MeshAsset>, std::pmr::string>::Err(
                    Allocator::MakeString("메시 속성 데이터가 유효하지 않습니다."));
        }

        auto makeVector3 = [](const std::array<Float32, 3>& components) noexcept
        {
            return Vector3D<Float32>{components[0], components[1], components[2]};
        };
        auto makeVector2 = [](const std::array<Float32, 2>& components) noexcept
        {
            return Vector2D<Float32>{components[0], components[1]};
        };
        auto makeColor = [](const std::array<Float32, 4>& components) noexcept
        {
            return ColorRGBA<Float32>{components[0], components[1], components[2], components[3]};
        };

        std::pmr::vector<Vector3D<Float32>>  parsedPositions{Allocator::GetMemoryResource()};
        std::pmr::vector<Vector3D<Float32>>  parsedNormals{Allocator::GetMemoryResource()};
        std::pmr::vector<Vector2D<Float32>>  parsedUVs{Allocator::GetMemoryResource()};
        std::pmr::vector<ColorRGBA<Float32>> parsedColors{Allocator::GetMemoryResource()};
        std::pmr::vector<UInt32>             parsedIndices{Allocator::GetMemoryResource()};

        if (!ReadComponentVectorArray<3>(*positionsArray, parsedPositions, makeVector3) ||
            !ReadComponentVectorArray<3>(*normalsArray, parsedNormals, makeVector3) ||
            (uvsArray != nullptr && !ReadComponentVectorArray<2>(*uvsArray, parsedUVs, makeVector2)) ||
            (colorsArray != nullptr && !ReadComponentVectorArray<4>(*colorsArray, parsedColors, makeColor)))
        {
            return Result<Allocator::UniquePtr<MeshAsset>, std::pmr::string>::Err(
                    Allocator::MakeString("메시 속성 데이터가 유효하지 않습니다."));
        }

        parsedIndices.reserve(indicesArray->Size());
        for (const rapidjson::Value& indexValue : indicesArray->GetArray())
        {
            UInt32 index = 0;
            if (!JsonUtility::TryReadUInt32(indexValue, index) || index >= parsedPositions.size())
            {
                return Result<Allocator::UniquePtr<MeshAsset>, std::pmr::string>::Err(
                        Allocator::MakeString("메시 인덱스가 정점 범위를 벗어났습니다."));
            }

            parsedIndices.push_back(index);
        }

        if (parsedPositions.empty() || parsedNormals.size() != parsedPositions.size() ||
            (!parsedUVs.empty() && parsedUVs.size() != parsedPositions.size()) ||
            (!parsedColors.empty() && parsedColors.size() != parsedPositions.size()) || parsedIndices.empty() ||
            parsedIndices.size() % 3 != 0)
        {
            return Result<Allocator::UniquePtr<MeshAsset>, std::pmr::string>::Err(
                    Allocator::MakeString("메시 정점 속성 또는 인덱스 개수가 유효하지 않습니다."));
        }

        Vector3D<Float32> boundsMin = parsedPositions.front();
        Vector3D<Float32> boundsMax = boundsMin;
        for (const Vector3D<Float32>& position : parsedPositions)
        {
            boundsMin = Vector3D<Float32>::Min(boundsMin, position);
            boundsMax = Vector3D<Float32>::Max(boundsMax, position);
        }

        const Vector3D<Float32> PARSED_BOUNDS_CENTER  = boundsMin * 0.5f + boundsMax * 0.5f;
        const Vector3D<Float32> PARSED_BOUNDS_EXTENTS = boundsMax * 0.5f - boundsMin * 0.5f;
        const USize             VERTEX_COUNT          = parsedPositions.size();

        std::pmr::vector<MeshVertex> builtVertices(VERTEX_COUNT, Allocator::GetMemoryResource());
        for (USize i = 0; i < VERTEX_COUNT; ++i)
        {
            const Vector3D<Float32>& POS   = parsedPositions[i];
            const Vector3D<Float32>& NORM  = parsedNormals[i];
            const ColorRGBA<Float32> COLOR = parsedColors.empty() ? ColorRGBA<Float32>::GetWhite() : parsedColors[i];

            const Vector2D<Float32> UV = parsedUVs.empty()
                    ? Vector2D<Float32>::GetZero()
                    : Vector2D<Float32>{parsedUVs[i].GetX(), 1.0f - parsedUVs[i].GetY()};

            builtVertices[i] = MeshVertex{
                    .position          = POS,
                    .normal            = NORM,
                    .color             = COLOR,
                    .textureCoordinate = UV,
            };
        }

        auto gpuMeshResult = device.CreateMesh(std::as_bytes(std::span<const MeshVertex>(builtVertices)),
                                               sizeof(MeshVertex),
                                               std::as_bytes(std::span<const UInt32>(parsedIndices)),
                                               EGPUIndexFormat::UINT32,
                                               EGPUPrimitiveTopology::TRIANGLE_LIST);
        if (!gpuMeshResult)
        {
            return Result<Allocator::UniquePtr<MeshAsset>, std::pmr::string>::Err(std::move(gpuMeshResult).GetError());
        }

        Allocator::UniquePtr<MeshAsset> mesh = Allocator::MakeUnique<MeshAsset>(path, device, gpuMeshResult.GetValue());
        mesh->SetBoundsCenter(PARSED_BOUNDS_CENTER);
        mesh->SetBoundsExtents(PARSED_BOUNDS_EXTENTS);
        mesh->SetPositions(std::move(parsedPositions));
        mesh->SetColors(std::move(parsedColors));
        mesh->SetNormals(std::move(parsedNormals));
        mesh->SetTextureCoords(std::move(parsedUVs));
        mesh->SetVertices(std::move(builtVertices));
        mesh->SetIndices(std::move(parsedIndices));

        return Result<Allocator::UniquePtr<MeshAsset>, std::pmr::string>::Ok(std::move(mesh));
    }
} // namespace Garage
