#ifndef GARAGE_GRAPHICS_SKYBOX_RENDERER_HPP
#define GARAGE_GRAPHICS_SKYBOX_RENDERER_HPP

#include "../core/defines.hpp"

#include "renderer.hpp"

namespace Garage
{
    class CubemapAsset;
    class IGPUCommandBuffer;
    class SystemContext;

    class GARAGE_API SkyboxRenderer final : public RendererBase
    {
    public:
        using RendererBase::RendererBase;
        ~SkyboxRenderer() noexcept override = default;

        void OnRender(SystemContext& context, IGPUCommandBuffer& commandBuffer) override;

        [[nodiscard]] const CubemapAsset* GetCubemap() const noexcept;
        void                              SetCubemap(const CubemapAsset* value) noexcept;

    private:
        const CubemapAsset* cubemap = nullptr;
    };

    inline const CubemapAsset* SkyboxRenderer::GetCubemap() const noexcept
    {
        return cubemap;
    }

    inline void SkyboxRenderer::SetCubemap(const CubemapAsset* value) noexcept
    {
        cubemap = value;
    }
} // namespace Garage

#endif // GARAGE_GRAPHICS_SKYBOX_RENDERER_HPP
