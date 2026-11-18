#include "garage/graphics/renderer.hpp"

#include "garage/actors/actor.hpp"
#include "garage/actors/transform.hpp"

#include "garage/core/defines.hpp"
#include "garage/core/types.hpp"

#include "garage/graphics/gpu_command_buffer.hpp"
#include "garage/graphics/gpu_constants.hpp"
#include "garage/graphics/gpu_graphics_pipeline_state.hpp"

#include "garage/math/matrix4x4.hpp"

#include "garage/resources/material_asset.hpp"

#include "garage/utilities/logger.hpp"

namespace Garage
{
    Matrix4x4<Float32> RendererBase::GetWorldMatrix() const noexcept
    {
        const ActorBase* owner     = GetOwner();
        const Transform* transform = (owner != nullptr) ? owner->GetComponent<Transform>() : nullptr;
        return (transform != nullptr) ? transform->GetWorldMatrix() : Matrix4x4<Float32>::GetIdentity();
    }

    const IGPUGraphicsPipelineState* RendererBase::GetPipelineState() const noexcept
    {
        return (material != nullptr) ? material->GetGraphicsPipelineState() : nullptr;
    }

    bool RendererBase::BindObjectConstants(IGPUCommandBuffer&        commandBuffer,
                                           const Matrix4x4<Float32>& worldMatrix) const
    {
        const ObjectConstants OBJECT_CONSTANTS{
                .world = worldMatrix,
        };

        if (const auto RESULT = commandBuffer.SetGraphicsConstants(OBJECT_CONSTANTS); !RESULT)
        {
            LOG_ERROR("오브젝트 변환을 바인딩하지 못했습니다: {}", RESULT.GetError());
            return false;
        }

        return true;
    }

    void RendererBase::ApplyPipelineState(IGPUCommandBuffer&               commandBuffer,
                                          const IGPUGraphicsPipelineState& pipelineState) const noexcept
    {
        ASSERT(material != nullptr);

        commandBuffer.SetGPUGraphicsPipelineState(pipelineState);
        commandBuffer.SetPrimitiveTopology(material->GetPrimitive());
    }
} // namespace Garage
