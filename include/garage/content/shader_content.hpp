#ifndef GARAGE_CONTENT_SHADER_CONTENT_HPP
#define GARAGE_CONTENT_SHADER_CONTENT_HPP

#include <filesystem>
#include <utility>

#include "../core/common.hpp"
#include "../core/common/containers.hpp"

#include "../graphics/gpu_graphics_pipeline_state.hpp"
#include "../graphics/gpu_target.hpp"
#include "../graphics/gpu_types.hpp"

#include "content.hpp"

namespace Garage
{
    class IGPUDevice;
    class IGPUShader;

    class GARAGE_API ShaderContent final : public ContentBase
    {
    public:
        explicit ShaderContent(std::filesystem::path path, IGPUDevice& device, IGPUShader* shader) noexcept;
        ~ShaderContent() noexcept override;

        [[nodiscard]] const IGPUShader* GetShader() const noexcept;

        [[nodiscard]] Span<const GPUVertexAttribute> GetVertexAttributes() const noexcept;
        void                                         SetVertexAttributes(Vector<GPUVertexAttribute>&& value) noexcept;

        [[nodiscard]] UInt32 GetVertexStride() const noexcept;
        void                 SetVertexStride(UInt32 value) noexcept;

        [[nodiscard]] EGPUFillMode GetFillMode() const noexcept;
        void                       SetFillMode(EGPUFillMode value) noexcept;

        [[nodiscard]] EGPUCullMode GetCullMode() const noexcept;
        void                       SetCullMode(EGPUCullMode value) noexcept;

        [[nodiscard]] EGPUFrontFace GetFrontFace() const noexcept;
        void                        SetFrontFace(EGPUFrontFace value) noexcept;

        [[nodiscard]] bool IsDepthTestEnabled() const noexcept;
        void               SetDepthTestEnabled(bool value) noexcept;

        [[nodiscard]] bool IsDepthWriteEnabled() const noexcept;
        void               SetDepthWriteEnabled(bool value) noexcept;

        [[nodiscard]] EGPUCompareOp GetDepthCompareOp() const noexcept;
        void                        SetDepthCompareOp(EGPUCompareOp value) noexcept;

        [[nodiscard]] bool IsBlendEnabled() const noexcept;
        void               SetBlendEnabled(bool value) noexcept;

        void SetAlphaBlend() noexcept;
        void ResetOpaqueBlend() noexcept;

        [[nodiscard]] EGPUPrimitiveTopology GetPrimitive() const noexcept;
        void                                SetPrimitive(EGPUPrimitiveTopology value) noexcept;

        [[nodiscard]] GPUGraphicsPipelineDesc CreatePipelineDescription(
                const ShaderContent& fragmentShader,
                EGPUTextureFormat    colorTargetFormat  = EGPUTextureFormat::R8G8B8A8_UNORM,
                EGPUTextureFormat    depthStencilFormat = GPUTarget::DEPTH_FORMAT) const noexcept;

    private:
        IGPUDevice*                device = nullptr;
        IGPUShader*                shader = nullptr;
        Vector<GPUVertexAttribute> vertexAttributes;
        UInt32                     vertexStride        = 0u;
        EGPUPrimitiveTopology      primitive           = EGPUPrimitiveTopology::TRIANGLE_LIST;
        EGPUFillMode               fillMode            = EGPUFillMode::FILL;
        EGPUCullMode               cullMode            = EGPUCullMode::BACK;
        EGPUFrontFace              frontFace           = EGPUFrontFace::CLOCKWISE;
        bool                       enableDepthTest     = true;
        bool                       enableDepthWrite    = true;
        EGPUCompareOp              depthCompareOp      = EGPUCompareOp::LESS_OR_EQUAL;
        bool                       enableBlend         = false;
        EGPUBlendFactor            srcColorBlendFactor = EGPUBlendFactor::ONE;
        EGPUBlendFactor            dstColorBlendFactor = EGPUBlendFactor::ZERO;
        EGPUBlendOp                colorBlendOp        = EGPUBlendOp::ADD;
        EGPUBlendFactor            srcAlphaBlendFactor = EGPUBlendFactor::ONE;
        EGPUBlendFactor            dstAlphaBlendFactor = EGPUBlendFactor::ZERO;
        EGPUBlendOp                alphaBlendOp        = EGPUBlendOp::ADD;
    };

    inline const IGPUShader* ShaderContent::GetShader() const noexcept
    {
        return shader;
    }

    inline Span<const GPUVertexAttribute> ShaderContent::GetVertexAttributes() const noexcept
    {
        return vertexAttributes;
    }

    inline void ShaderContent::SetVertexAttributes(Vector<GPUVertexAttribute>&& value) noexcept
    {
        vertexAttributes = std::move(value);
    }

    inline UInt32 ShaderContent::GetVertexStride() const noexcept
    {
        return vertexStride;
    }

    inline void ShaderContent::SetVertexStride(UInt32 value) noexcept
    {
        vertexStride = value;
    }

    inline EGPUFillMode ShaderContent::GetFillMode() const noexcept
    {
        return fillMode;
    }

    inline void ShaderContent::SetFillMode(EGPUFillMode value) noexcept
    {
        fillMode = value;
    }

    inline EGPUCullMode ShaderContent::GetCullMode() const noexcept
    {
        return cullMode;
    }

    inline void ShaderContent::SetCullMode(EGPUCullMode value) noexcept
    {
        cullMode = value;
    }

    inline EGPUFrontFace ShaderContent::GetFrontFace() const noexcept
    {
        return frontFace;
    }

    inline void ShaderContent::SetFrontFace(EGPUFrontFace value) noexcept
    {
        frontFace = value;
    }

    inline bool ShaderContent::IsDepthTestEnabled() const noexcept
    {
        return enableDepthTest;
    }

    inline void ShaderContent::SetDepthTestEnabled(bool value) noexcept
    {
        enableDepthTest = value;
    }

    inline bool ShaderContent::IsDepthWriteEnabled() const noexcept
    {
        return enableDepthWrite;
    }

    inline void ShaderContent::SetDepthWriteEnabled(bool value) noexcept
    {
        enableDepthWrite = value;
    }

    inline EGPUCompareOp ShaderContent::GetDepthCompareOp() const noexcept
    {
        return depthCompareOp;
    }

    inline void ShaderContent::SetDepthCompareOp(EGPUCompareOp value) noexcept
    {
        depthCompareOp = value;
    }

    inline bool ShaderContent::IsBlendEnabled() const noexcept
    {
        return enableBlend;
    }

    inline void ShaderContent::SetBlendEnabled(bool value) noexcept
    {
        enableBlend = value;
    }

    inline void ShaderContent::SetAlphaBlend() noexcept
    {
        enableBlend         = true;
        srcColorBlendFactor = EGPUBlendFactor::SRC_ALPHA;
        dstColorBlendFactor = EGPUBlendFactor::ONE_MINUS_SRC_ALPHA;
        colorBlendOp        = EGPUBlendOp::ADD;
        srcAlphaBlendFactor = EGPUBlendFactor::ONE;
        dstAlphaBlendFactor = EGPUBlendFactor::ONE_MINUS_SRC_ALPHA;
        alphaBlendOp        = EGPUBlendOp::ADD;
    }

    inline void ShaderContent::ResetOpaqueBlend() noexcept
    {
        enableBlend         = false;
        srcColorBlendFactor = EGPUBlendFactor::ONE;
        dstColorBlendFactor = EGPUBlendFactor::ZERO;
        colorBlendOp        = EGPUBlendOp::ADD;
        srcAlphaBlendFactor = EGPUBlendFactor::ONE;
        dstAlphaBlendFactor = EGPUBlendFactor::ZERO;
        alphaBlendOp        = EGPUBlendOp::ADD;
    }

    inline EGPUPrimitiveTopology ShaderContent::GetPrimitive() const noexcept
    {
        return primitive;
    }

    inline void ShaderContent::SetPrimitive(EGPUPrimitiveTopology value) noexcept
    {
        primitive = value;
    }
} // namespace Garage

#endif // GARAGE_CONTENT_SHADER_CONTENT_HPP
