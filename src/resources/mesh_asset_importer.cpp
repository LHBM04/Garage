#include "garage/resources/mesh_asset_importer.hpp"

#include <array>
#include <filesystem>
#include <fstream>
#include <ios>
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

#include "garage/resources/mesh_asset.hpp"

namespace Garage
{
    Result<Allocator::UniquePtr<MeshAsset>, std::pmr::string>
            MeshAssetImporter::Import(const std::filesystem::path& path) const
    {
        if (path.extension() != ".tukmesh")
        {
            return Result<Allocator::UniquePtr<MeshAsset>, std::pmr::string>::Err(
                    Allocator::MakeString("메시 로더에는 .tukmesh 파일이 필요합니다."));
        }

        std::ifstream file(path, std::ios::binary);
        if (!file)
        {
            std::pmr::string error = Allocator::MakeString("리소스를 열 수 없습니다: ");
            error.append(path.generic_string());
            return Result<Allocator::UniquePtr<MeshAsset>, std::pmr::string>::Err(std::move(error));
        }

        BinaryReader                        reader(file);
        std::pmr::vector<Vector3D<Float32>> parsedPositions{Allocator::GetMemoryResource()};
        std::pmr::vector<Vector3D<Float32>> parsedNormals{Allocator::GetMemoryResource()};
        std::pmr::vector<Vector2D<Float32>> parsedUVs{Allocator::GetMemoryResource()};
        std::pmr::vector<UInt32>            parsedIndices{Allocator::GetMemoryResource()};
        Int32                               count = 0;

        if (!reader.ReadCount(count, sizeof(Vector3D<Float32>)))
        {
            return Result<Allocator::UniquePtr<MeshAsset>, std::pmr::string>::Err(
                    Allocator::MakeString("메시 속성 개수가 유효하지 않습니다."));
        }
        parsedPositions.resize(static_cast<USize>(count));
        for (Vector3D<Float32>& value : parsedPositions)
        {
            std::array<Float32, 3> components{};
            if (!reader.Read(components))
            {
                return Result<Allocator::UniquePtr<MeshAsset>, std::pmr::string>::Err(
                        Allocator::MakeString("메시 속성 데이터가 유효하지 않습니다."));
            }
            value.Set(components[0], components[1], components[2]);
        }

        if (!reader.ReadCount(count, sizeof(Vector3D<Float32>)))
        {
            return Result<Allocator::UniquePtr<MeshAsset>, std::pmr::string>::Err(
                    Allocator::MakeString("메시 속성 개수가 유효하지 않습니다."));
        }
        parsedNormals.resize(static_cast<USize>(count));
        for (Vector3D<Float32>& value : parsedNormals)
        {
            std::array<Float32, 3> components{};
            if (!reader.Read(components))
            {
                return Result<Allocator::UniquePtr<MeshAsset>, std::pmr::string>::Err(
                        Allocator::MakeString("메시 속성 데이터가 유효하지 않습니다."));
            }
            value.Set(components[0], components[1], components[2]);
        }

        if (!reader.ReadCount(count, sizeof(Vector2D<Float32>)))
        {
            return Result<Allocator::UniquePtr<MeshAsset>, std::pmr::string>::Err(
                    Allocator::MakeString("메시 속성 개수가 유효하지 않습니다."));
        }
        parsedUVs.resize(static_cast<USize>(count));
        for (Vector2D<Float32>& value : parsedUVs)
        {
            std::array<Float32, 2> components{};
            if (!reader.Read(components))
            {
                return Result<Allocator::UniquePtr<MeshAsset>, std::pmr::string>::Err(
                        Allocator::MakeString("메시 속성 데이터가 유효하지 않습니다."));
            }
            value.Set(components[0], components[1]);
        }

        if (!reader.ReadCount(count, sizeof(UInt32)))
        {
            return Result<Allocator::UniquePtr<MeshAsset>, std::pmr::string>::Err(
                    Allocator::MakeString("메시 인덱스 개수가 유효하지 않습니다."));
        }
        parsedIndices.resize(static_cast<USize>(count));
        for (UInt32& index : parsedIndices)
        {
            if (!reader.Read(index) || index >= parsedPositions.size())
            {
                return Result<Allocator::UniquePtr<MeshAsset>, std::pmr::string>::Err(
                        Allocator::MakeString("메시 인덱스가 정점 범위를 벗어났습니다."));
            }
        }

        if (!reader.IsAtEnd() || parsedPositions.empty() || parsedNormals.size() != parsedPositions.size() ||
            (!parsedUVs.empty() && parsedUVs.size() != parsedPositions.size()) || parsedIndices.empty() ||
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
            const Vector3D<Float32>& POS  = parsedPositions[i];
            const Vector3D<Float32>& NORM = parsedNormals[i];

            const Vector2D<Float32> UV = parsedUVs.empty()
                    ? Vector2D<Float32>::GetZero()
                    : Vector2D<Float32>{parsedUVs[i].GetX(), 1.0f - parsedUVs[i].GetY()};

            builtVertices[i] = MeshVertex{
                    .position          = POS,
                    .normal            = NORM,
                    .color             = ColorRGBA<Float32>::GetWhite(),
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
        mesh->SetNormals(std::move(parsedNormals));
        mesh->SetTextureCoords(std::move(parsedUVs));
        mesh->SetVertices(std::move(builtVertices));
        mesh->SetIndices(std::move(parsedIndices));

        return Result<Allocator::UniquePtr<MeshAsset>, std::pmr::string>::Ok(std::move(mesh));
    }
} // namespace Garage
