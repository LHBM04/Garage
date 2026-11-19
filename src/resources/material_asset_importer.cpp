#include "garage/resources/material_asset_importer.hpp"

#include <algorithm>
#include <array>
#include <filesystem>
#include <string>
#include <utility>

#include <fmt/format.h>
#include <rapidjson/document.h>

#include "garage/core/memory.hpp"
#include "garage/core/types.hpp"

#include "garage/graphics/gpu_device.hpp"
#include "garage/graphics/gpu_graphics_pipeline_state.hpp"

#include "garage/math/color_rgba.hpp"

#include "garage/resources/asset_subsystem.hpp"
#include "garage/resources/material_asset.hpp"
#include "garage/resources/resource_path.hpp"
#include "garage/resources/shader_asset.hpp"
#include "garage/resources/texture_asset.hpp"

#include "garage/utilities/json_utility.hpp"

namespace Garage
{
    Result<UniquePtr<MaterialAsset>, std::pmr::string>
            MaterialAssetImporter::Import(const std::filesystem::path& path) const
    {
        AssetSubsystem* assets = GetOwner();
        if (assets == nullptr)
        {
            return Result<UniquePtr<MaterialAsset>, std::pmr::string>::Err(
                    Memory::MakeString("머티리얼 로더의 에셋 서브시스템이 nullptr입니다."));
        }

        if (!JsonUtility::HasJsonExtension(path))
        {
            return Result<UniquePtr<MaterialAsset>, std::pmr::string>::Err(
                    Memory::MakeString("머티리얼 로더에는 .json 파일이 필요합니다."));
        }

        auto documentResult = JsonUtility::LoadDocument(path);
        if (!documentResult)
        {
            return Result<UniquePtr<MaterialAsset>, std::pmr::string>::Err(std::move(documentResult).GetError());
        }

        const rapidjson::Value& ROOT = *documentResult;

        std::array<Float32, 4> base{1.0f, 1.0f, 1.0f, 1.0f};
        std::array<Float32, 4> emission{0.0f, 0.0f, 0.0f, 1.0f};
        std::array<Float32, 4> specular{1.0f, 1.0f, 1.0f, 1.0f};
        Float32                metallic  = 0.0f;
        Float32                roughness = 0.5f;
        std::pmr::string       vertexShader{Memory::GetMemoryResource()};
        std::pmr::string       hullShader{Memory::GetMemoryResource()};
        std::pmr::string       domainShader{Memory::GetMemoryResource()};
        std::pmr::string       geometryShader{Memory::GetMemoryResource()};
        std::pmr::string       fragmentShader{Memory::GetMemoryResource()};

        if (!JsonUtility::ReadOptionalFloatArray(ROOT, "baseColor", base) ||
            !JsonUtility::ReadOptionalFloatArray(ROOT, "emissiveColor", emission) ||
            !JsonUtility::ReadOptionalFloatArray(ROOT, "specularColor", specular) ||
            !JsonUtility::ReadOptionalFloat32(ROOT, "metallic", metallic) ||
            !JsonUtility::ReadOptionalFloat32(ROOT, "roughness", roughness) ||
            !JsonUtility::ReadString(ROOT, "vertexShader", vertexShader) ||
            !JsonUtility::ReadString(ROOT, "fragmentShader", fragmentShader) ||
            !JsonUtility::ReadOptionalString(ROOT, "hullShader", hullShader) ||
            !JsonUtility::ReadOptionalString(ROOT, "domainShader", domainShader) ||
            !JsonUtility::ReadOptionalString(ROOT, "geometryShader", geometryShader))
        {
            std::pmr::string error = Memory::MakeString("머티리얼 데이터가 유효하지 않습니다: ");
            error.append(path.generic_string());
            return Result<UniquePtr<MaterialAsset>, std::pmr::string>::Err(std::move(error));
        }

        if (!hullShader.empty() || !domainShader.empty() || !geometryShader.empty())
        {
            return Result<UniquePtr<MaterialAsset>, std::pmr::string>::Err(
                    Memory::MakeString("SDL_GPU 백엔드는 헐/도메인/지오메트리 셰이더 스테이지를 지원하지 않습니다."));
        }

        std::array<std::pmr::string, static_cast<USize>(EMaterialTextureSlot::COUNT)> textureReferences{
                std::pmr::string{Memory::GetMemoryResource()},
                std::pmr::string{Memory::GetMemoryResource()},
                std::pmr::string{Memory::GetMemoryResource()},
                std::pmr::string{Memory::GetMemoryResource()},
                std::pmr::string{Memory::GetMemoryResource()},
                std::pmr::string{Memory::GetMemoryResource()},
                std::pmr::string{Memory::GetMemoryResource()},
                std::pmr::string{Memory::GetMemoryResource()},
        };

        const rapidjson::Value* texturesObject = JsonUtility::FindMember(ROOT, "textures");
        if (texturesObject != nullptr)
        {
            if (!texturesObject->IsObject())
            {
                std::pmr::string error = Memory::MakeString("머티리얼 텍스처 항목이 유효하지 않습니다: ");
                error.append(path.generic_string());
                return Result<UniquePtr<MaterialAsset>, std::pmr::string>::Err(std::move(error));
            }

            constexpr std::array<std::pair<const char*, EMaterialTextureSlot>, 8> TEXTURE_SLOTS{{
                    {"baseColor", EMaterialTextureSlot::BASE_COLOR},
                    {"normal", EMaterialTextureSlot::NORMAL},
                    {"metallic", EMaterialTextureSlot::METALLIC},
                    {"roughness", EMaterialTextureSlot::ROUGHNESS},
                    {"occlusion", EMaterialTextureSlot::OCCLUSION},
                    {"emissive", EMaterialTextureSlot::EMISSIVE},
                    {"specular", EMaterialTextureSlot::SPECULAR},
                    {"mask", EMaterialTextureSlot::MASK},
            }};

            for (const auto& [slotKey, slotType] : TEXTURE_SLOTS)
            {
                const USize SLOT_INDEX = static_cast<USize>(slotType);
                if (!JsonUtility::ReadOptionalString(*texturesObject, slotKey, textureReferences[SLOT_INDEX]))
                {
                    std::pmr::string error = Memory::MakeString("머티리얼 텍스처 참조가 유효하지 않습니다: ");
                    error.append(slotKey);
                    return Result<UniquePtr<MaterialAsset>, std::pmr::string>::Err(std::move(error));
                }
            }
        }

        const auto ROOT_DIR = FindResourceRoot(path, "Materials");
        if (!ROOT_DIR)
        {
            return Result<UniquePtr<MaterialAsset>, std::pmr::string>::Err(ROOT_DIR.GetError());
        }

        const auto VERTEX_PATH   = ResolveResourcePath(*ROOT_DIR, vertexShader);
        const auto FRAGMENT_PATH = ResolveResourcePath(*ROOT_DIR, fragmentShader);
        if (!VERTEX_PATH || !FRAGMENT_PATH)
        {
            return Result<UniquePtr<MaterialAsset>, std::pmr::string>::Err(
                    Memory::MakeString("머티리얼의 셰이더 참조 경로가 유효하지 않습니다."));
        }

        const ColorRGBA<Float32> PARSED_BASE_COLOR{base[0], base[1], base[2], base[3]};
        const ColorRGBA<Float32> PARSED_EMISSIVE_COLOR{emission[0], emission[1], emission[2], emission[3]};
        const ColorRGBA<Float32> PARSED_SPECULAR_COLOR{specular[0], specular[1], specular[2], specular[3]};
        const Float32            PARSED_METALLIC  = std::clamp(metallic, 0.0f, 1.0f);
        const Float32            PARSED_ROUGHNESS = std::clamp(roughness, 0.0f, 1.0f);

        MaterialTextures textures{};
        for (USize slot = 0u; slot < textures.size(); ++slot)
        {
            const std::pmr::string& REFERENCE = textureReferences[slot];
            if (REFERENCE.empty())
            {
                continue;
            }

            const auto TEXTURE_PATH = ResolveResourcePath(*ROOT_DIR, REFERENCE);
            if (!TEXTURE_PATH)
            {
                return Result<UniquePtr<MaterialAsset>, std::pmr::string>::Err(TEXTURE_PATH.GetError());
            }

            const TextureAsset* texture = assets->LoadAsset<TextureAsset>(*TEXTURE_PATH);
            if (texture == nullptr)
            {
                std::pmr::string error = Memory::MakeString("머티리얼 텍스처를 로드하지 못했습니다: ");
                error.append(TEXTURE_PATH->generic_string());
                return Result<UniquePtr<MaterialAsset>, std::pmr::string>::Err(std::move(error));
            }

            textures[slot] = texture;
        }

        const ShaderAsset* vertex = assets->LoadAsset<ShaderAsset>(*VERTEX_PATH);
        if (vertex == nullptr)
        {
            const std::string MSG = fmt::format("정점 셰이더를 로드하지 못했습니다: {}", VERTEX_PATH->generic_string());
            return Result<UniquePtr<MaterialAsset>, std::pmr::string>::Err(Memory::MakeString(MSG));
        }

        const ShaderAsset* fragment = assets->LoadAsset<ShaderAsset>(*FRAGMENT_PATH);
        if (fragment == nullptr)
        {
            const std::string MSG =
                    fmt::format("프래그먼트 셰이더를 로드하지 못했습니다: {}", FRAGMENT_PATH->generic_string());
            return Result<UniquePtr<MaterialAsset>, std::pmr::string>::Err(Memory::MakeString(MSG));
        }

        const GPUGraphicsPipelineDesc DESCRIPTION =
                vertex->CreatePipelineDescription(*fragment, targetFormat, depthFormat);

        auto pipelineResult = device.CreateGraphicsPipelineState(DESCRIPTION);
        if (!pipelineResult)
        {
            const std::string MSG =
                    fmt::format("그래픽스 파이프라인 상태 생성에 실패했습니다: {}", pipelineResult.GetError());
            return Result<UniquePtr<MaterialAsset>, std::pmr::string>::Err(Memory::MakeString(MSG));
        }

        UniquePtr<MaterialAsset> material =
                MakeUnique<MaterialAsset>(path, device, vertex, fragment, pipelineResult.GetValue());
        material->SetCullMode(DESCRIPTION.cullMode);
        material->SetFillMode(DESCRIPTION.fillMode);
        material->SetBaseColor(PARSED_BASE_COLOR);
        material->SetEmissiveColor(PARSED_EMISSIVE_COLOR);
        material->SetSpecularColor(PARSED_SPECULAR_COLOR);
        material->SetMetallic(PARSED_METALLIC);
        material->SetRoughness(PARSED_ROUGHNESS);
        material->SetTextures(textures);
        material->SetTargetFormat(DESCRIPTION.colorTargetFormat);
        material->SetPrimitive(DESCRIPTION.primitiveTopology);

        return Result<UniquePtr<MaterialAsset>, std::pmr::string>::Ok(std::move(material));
    }
} // namespace Garage
