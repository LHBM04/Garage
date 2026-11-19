#ifndef GARAGE_GRAPHICS_RENDERER_HPP
#define GARAGE_GRAPHICS_RENDERER_HPP

#include "../actors/component.hpp"

#include "../core/defines.hpp"
#include "../core/types.hpp"

#include "../math/color_rgba.hpp"
#include "../math/matrix4x4.hpp"

namespace Garage
{
    class IGPUCommandBuffer;
    class IGPUGraphicsPipelineState;
    class MaterialAsset;
    class SystemContext;

    class GARAGE_API RendererBase : public ComponentBase
    {
    public:
        using ComponentBase::ComponentBase;
        ~RendererBase() noexcept override = default;

        void OnRender(SystemContext& context) override = 0;

        [[nodiscard]] const MaterialAsset* GetMaterial() const noexcept;
        void                               SetMaterial(const MaterialAsset* value) noexcept;

        [[nodiscard]] const ColorRGBA<Float32>& GetColor() const noexcept;
        void                                    SetColor(const ColorRGBA<Float32>& value) noexcept;

    protected:
        [[nodiscard]] Matrix4x4<Float32>               GetWorldMatrix() const noexcept;
        [[nodiscard]] const IGPUGraphicsPipelineState* GetPipelineState() const noexcept;
        [[nodiscard]] bool                             BindObjectConstants(IGPUCommandBuffer&        commandBuffer,
                                                                           const Matrix4x4<Float32>& worldMatrix) const;
        void ApplyPipelineState(IGPUCommandBuffer&               commandBuffer,
                                const IGPUGraphicsPipelineState& pipelineState) const noexcept;

    private:
        ColorRGBA<Float32>   color    = ColorRGBA<Float32>::GetWhite();
        const MaterialAsset* material = nullptr;
    };

    inline const MaterialAsset* RendererBase::GetMaterial() const noexcept
    {
        return material;
    }

    inline void RendererBase::SetMaterial(const MaterialAsset* value) noexcept
    {
        material = value;
    }

    inline const ColorRGBA<Float32>& RendererBase::GetColor() const noexcept
    {
        return color;
    }

    inline void RendererBase::SetColor(const ColorRGBA<Float32>& value) noexcept
    {
        color = value;
    }
} // namespace Garage

#endif // GARAGE_GRAPHICS_RENDERER_HPP
