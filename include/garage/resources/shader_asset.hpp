#ifndef GARAGE_RESOURCES_SHADER_ASSET_HPP
#define GARAGE_RESOURCES_SHADER_ASSET_HPP

#include <filesystem>
#include <span>
#include <utility>
#include <vector>

#include "../core/common.hpp"
#include "../core/memory.hpp"

#include "../graphics/gpu_graphics_pipeline_state.hpp"
#include "../graphics/gpu_target.hpp"
#include "../graphics/gpu_types.hpp"

#include "asset.hpp"

namespace Garage
{
    class IGPUDevice;
    class IGPUShader;

    class GARAGE_API ShaderAsset final : public AssetBase
    {
    public:
        explicit ShaderAsset(std::filesystem::path path, IGPUDevice& device, IGPUShader* shader) noexcept;
        ~ShaderAsset() noexcept override;

        [[nodiscard]] const IGPUShader* GetShader() const noexcept;

        [[nodiscard]] std::span<const GPUVertexAttribute> GetVertexAttributes() const noexcept;
        void SetVertexAttributes(std::pmr::vector<GPUVertexAttribute>&& value) noexcept;

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
                const ShaderAsset& fragmentShader,
                EGPUTextureFormat  colorTargetFormat  = EGPUTextureFormat::R8G8B8A8_UNORM,
                EGPUTextureFormat  depthStencilFormat = GPUTarget::DEPTH_FORMAT) const noexcept;

    private:
        IGPUDevice*                          device = nullptr;
        IGPUShader*                          shader = nullptr;
        std::pmr::vector<GPUVertexAttribute> vertexAttributes{Memory::GetMemoryResource()};
        UInt32                               vertexStride        = 0u;
        EGPUPrimitiveTopology                primitive           = EGPUPrimitiveTopology::TRIANGLE_LIST;
        EGPUFillMode                         fillMode            = EGPUFillMode::FILL;
        EGPUCullMode                         cullMode            = EGPUCullMode::BACK;
        EGPUFrontFace                        frontFace           = EGPUFrontFace::CLOCKWISE;
        bool                                 enableDepthTest     = true;
        bool                                 enableDepthWrite    = true;
        EGPUCompareOp                        depthCompareOp      = EGPUCompareOp::LESS_OR_EQUAL;
        bool                                 enableBlend         = false;
        EGPUBlendFactor                      srcColorBlendFactor = EGPUBlendFactor::ONE;
        EGPUBlendFactor                      dstColorBlendFactor = EGPUBlendFactor::ZERO;
        EGPUBlendOp                          colorBlendOp        = EGPUBlendOp::ADD;
        EGPUBlendFactor                      srcAlphaBlendFactor = EGPUBlendFactor::ONE;
        EGPUBlendFactor                      dstAlphaBlendFactor = EGPUBlendFactor::ZERO;
        EGPUBlendOp                          alphaBlendOp        = EGPUBlendOp::ADD;
    };

    inline const IGPUShader* ShaderAsset::GetShader() const noexcept
    {
        return shader;
    }

    inline std::span<const GPUVertexAttribute> ShaderAsset::GetVertexAttributes() const noexcept
    {
        return vertexAttributes;
    }

    inline void ShaderAsset::SetVertexAttributes(std::pmr::vector<GPUVertexAttribute>&& value) noexcept
    {
        vertexAttributes = std::move(value);
    }

    inline UInt32 ShaderAsset::GetVertexStride() const noexcept
    {
        return vertexStride;
    }

    inline void ShaderAsset::SetVertexStride(UInt32 value) noexcept
    {
        vertexStride = value;
    }

    inline EGPUFillMode ShaderAsset::GetFillMode() const noexcept
    {
        return fillMode;
    }

    inline void ShaderAsset::SetFillMode(EGPUFillMode value) noexcept
    {
        fillMode = value;
    }

    inline EGPUCullMode ShaderAsset::GetCullMode() const noexcept
    {
        return cullMode;
    }

    inline void ShaderAsset::SetCullMode(EGPUCullMode value) noexcept
    {
        cullMode = value;
    }

    inline EGPUFrontFace ShaderAsset::GetFrontFace() const noexcept
    {
        return frontFace;
    }

    inline void ShaderAsset::SetFrontFace(EGPUFrontFace value) noexcept
    {
        frontFace = value;
    }

    inline bool ShaderAsset::IsDepthTestEnabled() const noexcept
    {
        return enableDepthTest;
    }

    inline void ShaderAsset::SetDepthTestEnabled(bool value) noexcept
    {
        enableDepthTest = value;
    }

    inline bool ShaderAsset::IsDepthWriteEnabled() const noexcept
    {
        return enableDepthWrite;
    }

    inline void ShaderAsset::SetDepthWriteEnabled(bool value) noexcept
    {
        enableDepthWrite = value;
    }

    inline EGPUCompareOp ShaderAsset::GetDepthCompareOp() const noexcept
    {
        return depthCompareOp;
    }

    inline void ShaderAsset::SetDepthCompareOp(EGPUCompareOp value) noexcept
    {
        depthCompareOp = value;
    }

    inline bool ShaderAsset::IsBlendEnabled() const noexcept
    {
        return enableBlend;
    }

    inline void ShaderAsset::SetBlendEnabled(bool value) noexcept
    {
        enableBlend = value;
    }

    inline void ShaderAsset::SetAlphaBlend() noexcept
    {
        enableBlend         = true;
        srcColorBlendFactor = EGPUBlendFactor::SRC_ALPHA;
        dstColorBlendFactor = EGPUBlendFactor::ONE_MINUS_SRC_ALPHA;
        colorBlendOp        = EGPUBlendOp::ADD;
        srcAlphaBlendFactor = EGPUBlendFactor::ONE;
        dstAlphaBlendFactor = EGPUBlendFactor::ONE_MINUS_SRC_ALPHA;
        alphaBlendOp        = EGPUBlendOp::ADD;
    }

    inline void ShaderAsset::ResetOpaqueBlend() noexcept
    {
        enableBlend         = false;
        srcColorBlendFactor = EGPUBlendFactor::ONE;
        dstColorBlendFactor = EGPUBlendFactor::ZERO;
        colorBlendOp        = EGPUBlendOp::ADD;
        srcAlphaBlendFactor = EGPUBlendFactor::ONE;
        dstAlphaBlendFactor = EGPUBlendFactor::ZERO;
        alphaBlendOp        = EGPUBlendOp::ADD;
    }

    inline EGPUPrimitiveTopology ShaderAsset::GetPrimitive() const noexcept
    {
        return primitive;
    }

    inline void ShaderAsset::SetPrimitive(EGPUPrimitiveTopology value) noexcept
    {
        primitive = value;
    }
} // namespace Garage

#endif // GARAGE_RESOURCES_SHADER_ASSET_HPP
