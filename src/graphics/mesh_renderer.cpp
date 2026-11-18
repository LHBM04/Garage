#include "garage/graphics/mesh_renderer.hpp"

#include <array>

#include "garage/core/types.hpp"

#include "garage/graphics/gpu_command_buffer.hpp"
#include "garage/graphics/gpu_constants.hpp"
#include "garage/graphics/gpu_graphics_pipeline_state.hpp"
#include "garage/graphics/gpu_mesh.hpp"
#include "garage/graphics/gpu_texture2d.hpp"

#include "garage/resources/material_asset.hpp"
#include "garage/resources/mesh_asset.hpp"
#include "garage/resources/texture_asset.hpp"

#include "garage/utilities/logger.hpp"

namespace Garage
{
    void MeshRenderer::OnRender(IGPUCommandBuffer& commandBuffer)
    {
        const MaterialAsset*             material      = GetMaterial();
        const IGPUGraphicsPipelineState* pipelineState = GetPipelineState();
        const IGPUMesh*                  gpuMesh       = (mesh != nullptr) ? mesh->GetGPUMesh() : nullptr;
        if (material == nullptr || pipelineState == nullptr || gpuMesh == nullptr)
        {
            return;
        }

        if (material->GetTargetFormat() != commandBuffer.GetActiveTargetFormat())
        {
            LOG_ERROR("메시 머티리얼이 현재 렌더 타깃과 호환되지 않습니다.");
            return;
        }

        MaterialConstants constants{
                .baseColor     = material->GetBaseColor() * GetColor(),
                .emissiveColor = material->GetEmissiveColor(),
                .metallic      = material->GetMetallic(),
                .roughness     = material->GetRoughness(),
                .specularColor = material->GetSpecularColor(),
        };

        static_assert(IGPUCommandBuffer::TEXTURE_SLOT_COUNT == static_cast<UInt32>(EMaterialTextureSlot::COUNT));
        std::array<const IGPUTexture2D*, IGPUCommandBuffer::TEXTURE_SLOT_COUNT> textures{};
        const MaterialTextures& MATERIAL_TEXTURES = material->GetTextures();
        for (UInt32 index = 0; index < textures.size(); ++index)
        {
            if (const TextureAsset* textureAsset = MATERIAL_TEXTURES[index]; textureAsset != nullptr)
            {
                if (const IGPUTexture2D* texture = textureAsset->GetTexture(); texture != nullptr)
                {
                    textures[index] = texture;
                    constants.textureMask |= 1u << index;
                }
            }
        }

        if (const auto RESULT = commandBuffer.SetGraphicsConstants(constants); !RESULT)
        {
            LOG_ERROR("메시 머티리얼 상수를 바인딩하지 못했습니다: {}", RESULT.GetError());
            return;
        }

        if (const auto RESULT = commandBuffer.BindTextures(textures); !RESULT)
        {
            LOG_ERROR("메시 텍스처를 바인딩하지 못했습니다: {}", RESULT.GetError());
            return;
        }

        if (!BindObjectConstants(commandBuffer, GetWorldMatrix()))
        {
            return;
        }

        ApplyPipelineState(commandBuffer, *pipelineState);
        commandBuffer.SetVertexBuffer(0, gpuMesh->GetVertexBufferView());
        commandBuffer.SetIndexBuffer(gpuMesh->GetIndexBufferView());
        commandBuffer.DrawIndexedInstanced(gpuMesh->GetIndexCount(), 1);
    }
} // namespace Garage
