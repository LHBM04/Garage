#ifndef GARAGE_GRAPHICS_SKYBOX_RENDERER_HPP
#define GARAGE_GRAPHICS_SKYBOX_RENDERER_HPP

#include "../core/common.hpp"

#include "renderer.hpp"

namespace Garage
{
    class CubemapContent;
    class SystemContext;

    class GARAGE_API SkyboxRenderer final : public RendererBase
    {
    public:
        using RendererBase::RendererBase;
        ~SkyboxRenderer() noexcept override = default;

        void OnRender(SystemContext& context) override;

        [[nodiscard]] const CubemapContent* GetCubemap() const noexcept;
        void                                SetCubemap(const CubemapContent* value) noexcept;

    private:
        const CubemapContent* cubemap = nullptr;
    };

    inline const CubemapContent* SkyboxRenderer::GetCubemap() const noexcept
    {
        return cubemap;
    }

    inline void SkyboxRenderer::SetCubemap(const CubemapContent* value) noexcept
    {
        cubemap = value;
    }
} // namespace Garage

#endif // GARAGE_GRAPHICS_SKYBOX_RENDERER_HPP
