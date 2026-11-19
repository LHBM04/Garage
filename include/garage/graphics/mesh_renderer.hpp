#ifndef GARAGE_GRAPHICS_MESH_RENDERER_HPP
#define GARAGE_GRAPHICS_MESH_RENDERER_HPP

#include "../core/defines.hpp"

#include "renderer.hpp"

namespace Garage
{
    class MeshAsset;
    class SystemContext;

    class GARAGE_API MeshRenderer final : public RendererBase
    {
    public:
        using RendererBase::RendererBase;
        ~MeshRenderer() noexcept override = default;

        void OnRender(SystemContext& context) override;

        [[nodiscard]] const MeshAsset* GetMesh() const noexcept;
        void                           SetMesh(const MeshAsset* value) noexcept;

    private:
        const MeshAsset* mesh = nullptr;
    };

    inline const MeshAsset* MeshRenderer::GetMesh() const noexcept
    {
        return mesh;
    }

    inline void MeshRenderer::SetMesh(const MeshAsset* value) noexcept
    {
        mesh = value;
    }
} // namespace Garage

#endif // GARAGE_GRAPHICS_MESH_RENDERER_HPP
