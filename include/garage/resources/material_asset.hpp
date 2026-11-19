#ifndef GARAGE_RESOURCES_MATERIAL_ASSET_HPP
#define GARAGE_RESOURCES_MATERIAL_ASSET_HPP

#include <filesystem>

#include "../core/common.hpp"
#include "../core/common/containers.hpp"

#include "../graphics/gpu_types.hpp"

#include "../math/color_rgba.hpp"

#include "asset.hpp"

namespace Garage
{
    class IGPUDevice;
    class IGPUGraphicsPipelineState;
    class ShaderAsset;
    class TextureAsset;

    enum class EMaterialTextureSlot : UInt32
    {
        BASE_COLOR,
        NORMAL,
        METALLIC,
        ROUGHNESS,
        OCCLUSION,
        EMISSIVE,
        SPECULAR,
        MASK,
        COUNT,
    };

    using MaterialTextures = Array<const TextureAsset*, static_cast<USize>(EMaterialTextureSlot::COUNT)>;

    class GARAGE_API MaterialAsset final : public AssetBase
    {
    public:
        explicit MaterialAsset(std::filesystem::path      path,
                               IGPUDevice&                device,
                               const ShaderAsset*         vertexShader,
                               const ShaderAsset*         fragmentShader,
                               IGPUGraphicsPipelineState* pipelineState) noexcept;
        ~MaterialAsset() noexcept override;

        [[nodiscard]] EGPUCullMode GetCullMode() const noexcept;
        void                       SetCullMode(EGPUCullMode value) noexcept;

        [[nodiscard]] EGPUFillMode GetFillMode() const noexcept;
        void                       SetFillMode(EGPUFillMode value) noexcept;

        [[nodiscard]] const ColorRGBA<Float32>& GetBaseColor() const noexcept;
        void                                    SetBaseColor(const ColorRGBA<Float32>& value) noexcept;

        [[nodiscard]] const ColorRGBA<Float32>& GetEmissiveColor() const noexcept;
        void                                    SetEmissiveColor(const ColorRGBA<Float32>& value) noexcept;

        [[nodiscard]] const ColorRGBA<Float32>& GetSpecularColor() const noexcept;
        void                                    SetSpecularColor(const ColorRGBA<Float32>& value) noexcept;

        [[nodiscard]] Float32 GetMetallic() const noexcept;
        void                  SetMetallic(Float32 value) noexcept;

        [[nodiscard]] Float32 GetRoughness() const noexcept;
        void                  SetRoughness(Float32 value) noexcept;

        [[nodiscard]] Float32 GetSmoothness() const noexcept;
        void                  SetSmoothness(Float32 value) noexcept;

        [[nodiscard]] const MaterialTextures& GetTextures() const noexcept;
        void                                  SetTextures(const MaterialTextures& value) noexcept;

        [[nodiscard]] EGPUPrimitiveTopology GetPrimitive() const noexcept;
        void                                SetPrimitive(EGPUPrimitiveTopology value) noexcept;

        [[nodiscard]] const ShaderAsset* GetVertexShader() const noexcept;
        [[nodiscard]] const ShaderAsset* GetFragmentShader() const noexcept;

        [[nodiscard]] const IGPUGraphicsPipelineState* GetGraphicsPipelineState() const noexcept;

        [[nodiscard]] EGPUTextureFormat GetTargetFormat() const noexcept;
        void                            SetTargetFormat(EGPUTextureFormat value) noexcept;

    private:
        IGPUDevice*  device   = nullptr;
        EGPUCullMode cullMode = EGPUCullMode::BACK;
        EGPUFillMode fillMode = EGPUFillMode::FILL;

        ColorRGBA<Float32> baseColor{1.0f, 1.0f, 1.0f, 1.0f};
        ColorRGBA<Float32> emissiveColor{0.0f, 0.0f, 0.0f, 1.0f};
        ColorRGBA<Float32> specularColor{1.0f, 1.0f, 1.0f, 1.0f};

        Float32 metallic  = 0.0f;
        Float32 roughness = 0.5f;

        MaterialTextures           textures{};
        const ShaderAsset*         vertexShader   = nullptr;
        const ShaderAsset*         fragmentShader = nullptr;
        IGPUGraphicsPipelineState* pipelineState  = nullptr;
        EGPUTextureFormat          targetFormat   = EGPUTextureFormat::UNKNOWN;
        EGPUPrimitiveTopology      primitive      = EGPUPrimitiveTopology::TRIANGLE_LIST;
    };

    inline EGPUCullMode MaterialAsset::GetCullMode() const noexcept
    {
        return cullMode;
    }

    inline void MaterialAsset::SetCullMode(EGPUCullMode value) noexcept
    {
        cullMode = value;
    }

    inline EGPUFillMode MaterialAsset::GetFillMode() const noexcept
    {
        return fillMode;
    }

    inline void MaterialAsset::SetFillMode(EGPUFillMode value) noexcept
    {
        fillMode = value;
    }

    inline const ColorRGBA<Float32>& MaterialAsset::GetBaseColor() const noexcept
    {
        return baseColor;
    }

    inline void MaterialAsset::SetBaseColor(const ColorRGBA<Float32>& value) noexcept
    {
        baseColor = value;
    }

    inline const ColorRGBA<Float32>& MaterialAsset::GetEmissiveColor() const noexcept
    {
        return emissiveColor;
    }

    inline void MaterialAsset::SetEmissiveColor(const ColorRGBA<Float32>& value) noexcept
    {
        emissiveColor = value;
    }

    inline const ColorRGBA<Float32>& MaterialAsset::GetSpecularColor() const noexcept
    {
        return specularColor;
    }

    inline void MaterialAsset::SetSpecularColor(const ColorRGBA<Float32>& value) noexcept
    {
        specularColor = value;
    }

    inline Float32 MaterialAsset::GetMetallic() const noexcept
    {
        return metallic;
    }

    inline void MaterialAsset::SetMetallic(Float32 value) noexcept
    {
        metallic = value;
    }

    inline Float32 MaterialAsset::GetRoughness() const noexcept
    {
        return roughness;
    }

    inline void MaterialAsset::SetRoughness(Float32 value) noexcept
    {
        roughness = value;
    }

    inline Float32 MaterialAsset::GetSmoothness() const noexcept
    {
        return 1.0f - roughness;
    }

    inline void MaterialAsset::SetSmoothness(Float32 value) noexcept
    {
        roughness = 1.0f - value;
    }

    inline const MaterialTextures& MaterialAsset::GetTextures() const noexcept
    {
        return textures;
    }

    inline void MaterialAsset::SetTextures(const MaterialTextures& value) noexcept
    {
        textures = value;
    }

    inline EGPUPrimitiveTopology MaterialAsset::GetPrimitive() const noexcept
    {
        return primitive;
    }

    inline void MaterialAsset::SetPrimitive(EGPUPrimitiveTopology value) noexcept
    {
        primitive = value;
    }

    inline const ShaderAsset* MaterialAsset::GetVertexShader() const noexcept
    {
        return vertexShader;
    }

    inline const ShaderAsset* MaterialAsset::GetFragmentShader() const noexcept
    {
        return fragmentShader;
    }

    inline const IGPUGraphicsPipelineState* MaterialAsset::GetGraphicsPipelineState() const noexcept
    {
        return pipelineState;
    }

    inline EGPUTextureFormat MaterialAsset::GetTargetFormat() const noexcept
    {
        return targetFormat;
    }

    inline void MaterialAsset::SetTargetFormat(EGPUTextureFormat value) noexcept
    {
        targetFormat = value;
    }
} // namespace Garage

#endif // GARAGE_RESOURCES_MATERIAL_ASSET_HPP
