#include "garage/content/model_content_importer.hpp"

#include <filesystem>
#include <string>
#include <utility>

#include <fmt/format.h>
#include <rapidjson/document.h>

#include "garage/content/content_path.hpp"
#include "garage/content/content_subsystem.hpp"
#include "garage/content/material_content.hpp"
#include "garage/content/mesh_content.hpp"
#include "garage/content/model_content.hpp"

#include "garage/core/common.hpp"
#include "garage/core/common/containers.hpp"
#include "garage/core/memory.hpp"

#include "garage/utilities/json_utility.hpp"

namespace Garage
{
    Result<UniquePtr<ModelContent>, String> ModelContentImporter::Import(const std::filesystem::path& path) const
    {
        ContentSubsystem* contentSubsystem = GetOwner();
        if (contentSubsystem == nullptr)
        {
            return Result<UniquePtr<ModelContent>, String>::Err(
                    String("모델 로더의 콘텐츠 서브시스템이 nullptr입니다."));
        }

        if (!JsonUtility::HasJsonExtension(path))
        {
            return Result<UniquePtr<ModelContent>, String>::Err(String("모델 로더에는 .json 파일이 필요합니다."));
        }

        auto documentResult = JsonUtility::LoadDocument(path);
        if (!documentResult)
        {
            return Result<UniquePtr<ModelContent>, String>::Err(std::move(documentResult).GetError());
        }

        const auto ROOT_DIR = FindContentRoot(path, "Models");
        if (!ROOT_DIR)
        {
            return Result<UniquePtr<ModelContent>, String>::Err(ROOT_DIR.GetError());
        }

        struct PendingNode final
        {
            Int32                   parentIndex = -1;
            const rapidjson::Value* jsonNode    = nullptr;
            USize                   depth       = 0;
        };

        Vector<PendingNode> pending;
        pending.push_back(PendingNode{.parentIndex = -1, .jsonNode = &(*documentResult), .depth = 1u});
        Vector<Submodel> nodes;

        while (!pending.empty())
        {
            const PendingNode CURRENT = pending.back();
            pending.pop_back();

            if (nodes.size() >= JsonUtility::MAX_ELEMENTS || CURRENT.depth > JsonUtility::MAX_DEPTH)
            {
                return Result<UniquePtr<ModelContent>, String>::Err(
                        String("모델 노드 수 또는 계층 깊이가 제한을 초과했습니다."));
            }

            if (CURRENT.jsonNode == nullptr || !CURRENT.jsonNode->IsObject())
            {
                String error("모델 노드 데이터가 유효하지 않습니다: ");
                error.append(path.generic_string());
                return Result<UniquePtr<ModelContent>, String>::Err(std::move(error));
            }

            const rapidjson::Value& NODE_VALUE = *CURRENT.jsonNode;

            Submodel node{};
            node.parentIndex = CURRENT.parentIndex;

            Array<Float32, 3>       position{0.0f, 0.0f, 0.0f};
            Array<Float32, 4>       rotation{0.0f, 0.0f, 0.0f, 1.0f};
            Array<Float32, 3>       scale{1.0f, 1.0f, 1.0f};
            String                  meshReference;
            String                  materialReference;
            const rapidjson::Value* childrenArray = nullptr;

            if (!JsonUtility::ReadOptionalString(NODE_VALUE, "name", node.name) ||
                !JsonUtility::ReadOptionalFloatArray(NODE_VALUE, "position", position) ||
                !JsonUtility::ReadOptionalFloatArray(NODE_VALUE, "rotation", rotation) ||
                !JsonUtility::ReadOptionalFloatArray(NODE_VALUE, "scale", scale) ||
                !JsonUtility::ReadOptionalString(NODE_VALUE, "mesh", meshReference) ||
                !JsonUtility::ReadOptionalString(NODE_VALUE, "material", materialReference) ||
                !JsonUtility::ReadOptionalArray(NODE_VALUE, "children", childrenArray))
            {
                String error("모델 노드 데이터가 유효하지 않습니다: ");
                error.append(path.generic_string());
                return Result<UniquePtr<ModelContent>, String>::Err(std::move(error));
            }

            node.localPosition.Set(position[0], position[1], position[2]);
            node.localRotation.Set(rotation[0], rotation[1], rotation[2], rotation[3]);
            node.localScale.Set(scale[0], scale[1], scale[2]);

            if (meshReference.empty() != materialReference.empty())
            {
                String error("모델 노드의 메시와 머티리얼 참조는 함께 지정해야 합니다: ");
                error.append(node.name);
                return Result<UniquePtr<ModelContent>, String>::Err(std::move(error));
            }

            if (!meshReference.empty() && !materialReference.empty())
            {
                const auto MESH_PATH     = ResolveContentPath(*ROOT_DIR, meshReference);
                const auto MATERIAL_PATH = ResolveContentPath(*ROOT_DIR, materialReference);
                if (!MESH_PATH || !MATERIAL_PATH)
                {
                    return Result<UniquePtr<ModelContent>, String>::Err(
                            String("모델 콘텐츠 참조 경로가 유효하지 않습니다."));
                }

                node.mesh     = contentSubsystem->LoadContent<MeshContent>(*MESH_PATH);
                node.material = contentSubsystem->LoadContent<MaterialContent>(*MATERIAL_PATH);
                if (node.mesh == nullptr || node.material == nullptr)
                {
                    const std::string MSG = fmt::format("모델 콘텐츠를 로드하지 못했습니다: {}, {}",
                                                        MESH_PATH->generic_string(),
                                                        MATERIAL_PATH->generic_string());
                    return Result<UniquePtr<ModelContent>, String>::Err(String(MSG));
                }
            }

            const Int32 INDEX = static_cast<Int32>(nodes.size());
            nodes.push_back(std::move(node));

            if (childrenArray != nullptr)
            {
                const auto CHILDREN = childrenArray->GetArray();
                for (USize childOffset = CHILDREN.Size(); childOffset > 0; --childOffset)
                {
                    const rapidjson::Value& CHILD = CHILDREN[static_cast<unsigned int>(childOffset - 1)];
                    pending.push_back(
                            PendingNode{.parentIndex = INDEX, .jsonNode = &CHILD, .depth = CURRENT.depth + 1u});
                }
            }
        }

        return Result<UniquePtr<ModelContent>, String>::Ok(MakeUnique<ModelContent>(path, std::move(nodes)));
    }
} // namespace Garage
