#include "garage/resources/model_asset_importer.hpp"

#include <array>
#include <filesystem>
#include <fstream>
#include <ios>
#include <string>
#include <utility>
#include <vector>

#include <fmt/format.h>

#include "garage/core/allocator.hpp"
#include "garage/core/binary_reader.hpp"
#include "garage/core/types.hpp"

#include "garage/resources/asset_subsystem.hpp"
#include "garage/resources/material_asset.hpp"
#include "garage/resources/mesh_asset.hpp"
#include "garage/resources/model_asset.hpp"
#include "garage/resources/resource_path.hpp"

namespace Garage
{
    Result<Allocator::UniquePtr<ModelAsset>, std::pmr::string>
            ModelAssetImporter::Import(const std::filesystem::path& path) const
    {
        AssetSubsystem* assets = GetOwner();
        if (assets == nullptr)
        {
            return Result<Allocator::UniquePtr<ModelAsset>, std::pmr::string>::Err(
                    Allocator::MakeString("모델 로더의 에셋 서브시스템이 nullptr입니다."));
        }

        if (path.extension() != ".tukmodel")
        {
            return Result<Allocator::UniquePtr<ModelAsset>, std::pmr::string>::Err(
                    Allocator::MakeString("모델 로더에는 .tukmodel 파일이 필요합니다."));
        }

        std::ifstream file(path, std::ios::binary);
        if (!file)
        {
            std::pmr::string error = Allocator::MakeString("리소스를 열 수 없습니다: ");
            error.append(path.generic_string());
            return Result<Allocator::UniquePtr<ModelAsset>, std::pmr::string>::Err(std::move(error));
        }

        BinaryReader reader(file);
        const auto   ROOT = FindResourceRoot(path, "Models");
        if (!ROOT)
        {
            return Result<Allocator::UniquePtr<ModelAsset>, std::pmr::string>::Err(ROOT.GetError());
        }

        struct PendingChildren final
        {
            Int32 parentIndex = -1;
            Int32 count       = 0;
        };

        std::pmr::vector<PendingChildren> pending{{-1, 1}, Allocator::GetMemoryResource()};
        std::pmr::vector<Submodel>        nodes{Allocator::GetMemoryResource()};
        while (!pending.empty())
        {
            if (pending.back().count == 0)
            {
                pending.pop_back();
                continue;
            }

            if (nodes.size() >= BinaryReader::MAX_ELEMENTS || pending.size() > BinaryReader::MAX_DEPTH)
            {
                return Result<Allocator::UniquePtr<ModelAsset>, std::pmr::string>::Err(
                        Allocator::MakeString("모델 노드 수 또는 계층 깊이가 제한을 초과했습니다."));
            }

            Submodel node{};
            node.parentIndex = pending.back().parentIndex;
            --pending.back().count;

            std::array<Float32, 3> position{};
            std::array<Float32, 4> rotation{};
            std::array<Float32, 3> scale{};
            std::pmr::string       meshReference{Allocator::GetMemoryResource()};
            std::pmr::string       materialReference{Allocator::GetMemoryResource()};
            Int32                  childCount = 0;
            if (!reader.Read(node.name, position, rotation, scale, meshReference, materialReference) ||
                !reader.ReadCount(childCount, sizeof(Submodel)))
            {
                std::pmr::string error = Allocator::MakeString("모델 노드 데이터가 유효하지 않습니다: ");
                error.append(path.generic_string());
                return Result<Allocator::UniquePtr<ModelAsset>, std::pmr::string>::Err(std::move(error));
            }

            node.localPosition.Set(position[0], position[1], position[2]);
            node.localRotation.Set(rotation[0], rotation[1], rotation[2], rotation[3]);
            node.localScale.Set(scale[0], scale[1], scale[2]);

            if (meshReference.empty() != materialReference.empty())
            {
                std::pmr::string error =
                        Allocator::MakeString("모델 노드의 메시와 머티리얼 참조는 함께 지정해야 합니다: ");
                error.append(node.name);
                return Result<Allocator::UniquePtr<ModelAsset>, std::pmr::string>::Err(std::move(error));
            }

            if (!meshReference.empty() && !materialReference.empty())
            {
                const auto MESH_PATH     = ResolveResourcePath(*ROOT, meshReference);
                const auto MATERIAL_PATH = ResolveResourcePath(*ROOT, materialReference);
                if (!MESH_PATH || !MATERIAL_PATH)
                {
                    return Result<Allocator::UniquePtr<ModelAsset>, std::pmr::string>::Err(
                            Allocator::MakeString("모델 리소스 참조 경로가 유효하지 않습니다."));
                }

                node.mesh     = assets->LoadAsset<MeshAsset>(*MESH_PATH);
                node.material = assets->LoadAsset<MaterialAsset>(*MATERIAL_PATH);
                if (node.mesh == nullptr || node.material == nullptr)
                {
                    const std::string MSG = fmt::format("모델 리소스를 로드하지 못했습니다: {}, {}",
                                                        MESH_PATH->generic_string(),
                                                        MATERIAL_PATH->generic_string());
                    return Result<Allocator::UniquePtr<ModelAsset>, std::pmr::string>::Err(Allocator::MakeString(MSG));
                }
            }

            const Int32 INDEX = static_cast<Int32>(nodes.size());
            nodes.push_back(std::move(node));
            if (childCount > 0)
            {
                pending.push_back({INDEX, childCount});
            }
        }

        if (!reader.IsAtEnd())
        {
            std::pmr::string error = Allocator::MakeString("모델 파일 끝에 예상치 못한 데이터가 남아 있습니다: ");
            error.append(path.generic_string());
            return Result<Allocator::UniquePtr<ModelAsset>, std::pmr::string>::Err(std::move(error));
        }

        return Result<Allocator::UniquePtr<ModelAsset>, std::pmr::string>::Ok(
                Allocator::MakeUnique<ModelAsset>(path, std::move(nodes)));
    }
} // namespace Garage
