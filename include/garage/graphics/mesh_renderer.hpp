#ifndef GARAGE_GRAPHICS_MESH_RENDERER_HPP
#define GARAGE_GRAPHICS_MESH_RENDERER_HPP

#include "../core/common.hpp"

#include "renderer.hpp"

namespace Garage
{
    class MeshContent;
    class SystemContext;

    class GARAGE_API MeshRenderer final : public RendererBase
    {
    public:
        using RendererBase::RendererBase;
        ~MeshRenderer() noexcept override = default;

        void OnRender(SystemContext& context) override;

        [[nodiscard]] const MeshContent* GetMesh() const noexcept;
        void                             SetMesh(const MeshContent* value) noexcept;

    private:
        const MeshContent* mesh = nullptr;
    };

    inline const MeshContent* MeshRenderer::GetMesh() const noexcept
    {
        return mesh;
    }

    inline void MeshRenderer::SetMesh(const MeshContent* value) noexcept
    {
        mesh = value;
    }
} // namespace Garage

#endif // GARAGE_GRAPHICS_MESH_RENDERER_HPP
