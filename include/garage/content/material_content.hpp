#ifndef GARAGE_CONTENT_MATERIAL_CONTENT_HPP
#define GARAGE_CONTENT_MATERIAL_CONTENT_HPP

#include <filesystem>

#include "../core/common.hpp"
#include "../core/common/containers.hpp"

#include "../graphics/gpu_types.hpp"

#include "../math/color_rgba.hpp"

#include "content.hpp"

namespace Garage
{
    class IGPUDevice;
    class IGPUGraphicsPipelineState;
    class ShaderContent;
    class TextureContent;

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

    using MaterialTextures = Array<const TextureContent*, static_cast<USize>(EMaterialTextureSlot::COUNT)>;

    class GARAGE_API MaterialContent final : public ContentBase
    {
    public:
        explicit MaterialContent(std::filesystem::path      path,
                                 IGPUDevice&                device,
                                 const ShaderContent*       vertexShader,
                                 const ShaderContent*       fragmentShader,
                                 IGPUGraphicsPipelineState* pipelineState) noexcept;
        ~MaterialContent() noexcept override;

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

        [[nodiscard]] const ShaderContent* GetVertexShader() const noexcept;
        [[nodiscard]] const ShaderContent* GetFragmentShader() const noexcept;

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
        const ShaderContent*       vertexShader   = nullptr;
        const ShaderContent*       fragmentShader = nullptr;
        IGPUGraphicsPipelineState* pipelineState  = nullptr;
        EGPUTextureFormat          targetFormat   = EGPUTextureFormat::UNKNOWN;
        EGPUPrimitiveTopology      primitive      = EGPUPrimitiveTopology::TRIANGLE_LIST;
    };

    inline EGPUCullMode MaterialContent::GetCullMode() const noexcept
    {
        return cullMode;
    }

    inline void MaterialContent::SetCullMode(EGPUCullMode value) noexcept
    {
        cullMode = value;
    }

    inline EGPUFillMode MaterialContent::GetFillMode() const noexcept
    {
        return fillMode;
    }

    inline void MaterialContent::SetFillMode(EGPUFillMode value) noexcept
    {
        fillMode = value;
    }

    inline const ColorRGBA<Float32>& MaterialContent::GetBaseColor() const noexcept
    {
        return baseColor;
    }

    inline void MaterialContent::SetBaseColor(const ColorRGBA<Float32>& value) noexcept
    {
        baseColor = value;
    }

    inline const ColorRGBA<Float32>& MaterialContent::GetEmissiveColor() const noexcept
    {
        return emissiveColor;
    }

    inline void MaterialContent::SetEmissiveColor(const ColorRGBA<Float32>& value) noexcept
    {
        emissiveColor = value;
    }

    inline const ColorRGBA<Float32>& MaterialContent::GetSpecularColor() const noexcept
    {
        return specularColor;
    }

    inline void MaterialContent::SetSpecularColor(const ColorRGBA<Float32>& value) noexcept
    {
        specularColor = value;
    }

    inline Float32 MaterialContent::GetMetallic() const noexcept
    {
        return metallic;
    }

    inline void MaterialContent::SetMetallic(Float32 value) noexcept
    {
        metallic = value;
    }

    inline Float32 MaterialContent::GetRoughness() const noexcept
    {
        return roughness;
    }

    inline void MaterialContent::SetRoughness(Float32 value) noexcept
    {
        roughness = value;
    }

    inline Float32 MaterialContent::GetSmoothness() const noexcept
    {
        return 1.0f - roughness;
    }

    inline void MaterialContent::SetSmoothness(Float32 value) noexcept
    {
        roughness = 1.0f - value;
    }

    inline const MaterialTextures& MaterialContent::GetTextures() const noexcept
    {
        return textures;
    }

    inline void MaterialContent::SetTextures(const MaterialTextures& value) noexcept
    {
        textures = value;
    }

    inline EGPUPrimitiveTopology MaterialContent::GetPrimitive() const noexcept
    {
        return primitive;
    }

    inline void MaterialContent::SetPrimitive(EGPUPrimitiveTopology value) noexcept
    {
        primitive = value;
    }

    inline const ShaderContent* MaterialContent::GetVertexShader() const noexcept
    {
        return vertexShader;
    }

    inline const ShaderContent* MaterialContent::GetFragmentShader() const noexcept
    {
        return fragmentShader;
    }

    inline const IGPUGraphicsPipelineState* MaterialContent::GetGraphicsPipelineState() const noexcept
    {
        return pipelineState;
    }

    inline EGPUTextureFormat MaterialContent::GetTargetFormat() const noexcept
    {
        return targetFormat;
    }

    inline void MaterialContent::SetTargetFormat(EGPUTextureFormat value) noexcept
    {
        targetFormat = value;
    }
} // namespace Garage

#endif // GARAGE_CONTENT_MATERIAL_CONTENT_HPP
