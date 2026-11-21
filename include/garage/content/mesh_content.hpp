#ifndef GARAGE_CONTENT_MESH_CONTENT_HPP
#define GARAGE_CONTENT_MESH_CONTENT_HPP

#include <filesystem>
#include <utility>

#include "../core/common.hpp"
#include "../core/common/containers.hpp"

#include "../graphics/mesh_vertex.hpp"

#include "../math/color_rgba.hpp"
#include "../math/vector2d.hpp"
#include "../math/vector3d.hpp"

#include "content.hpp"

namespace Garage
{
    class IGPUDevice;
    class IGPUMesh;

    class GARAGE_API MeshContent final : public ContentBase
    {
    public:
        explicit MeshContent(std::filesystem::path path, IGPUDevice& device, IGPUMesh* gpuMesh) noexcept;
        ~MeshContent() noexcept override;

        [[nodiscard]] const Vector3D<Float32>& GetBoundsCenter() const noexcept;
        void                                   SetBoundsCenter(const Vector3D<Float32>& value) noexcept;

        [[nodiscard]] const Vector3D<Float32>& GetBoundsExtents() const noexcept;
        void                                   SetBoundsExtents(const Vector3D<Float32>& value) noexcept;

        [[nodiscard]] Span<const Vector3D<Float32>> GetPositions() const noexcept;
        void                                        SetPositions(Vector<Vector3D<Float32>>&& value) noexcept;

        [[nodiscard]] Span<const ColorRGBA<Float32>> GetColors() const noexcept;
        void                                         SetColors(Vector<ColorRGBA<Float32>>&& value) noexcept;

        [[nodiscard]] Span<const Vector3D<Float32>> GetNormals() const noexcept;
        void                                        SetNormals(Vector<Vector3D<Float32>>&& value) noexcept;

        [[nodiscard]] Span<const Vector2D<Float32>> GetTextureCoords() const noexcept;
        void                                        SetTextureCoords(Vector<Vector2D<Float32>>&& value) noexcept;

        [[nodiscard]] Span<const MeshVertex> GetVertices() const noexcept;
        void                                 SetVertices(Vector<MeshVertex>&& value) noexcept;

        [[nodiscard]] Span<const UInt32> GetIndices() const noexcept;
        void                             SetIndices(Vector<UInt32>&& value) noexcept;

        [[nodiscard]] const IGPUMesh* GetGPUMesh() const noexcept;

    private:
        IGPUDevice*                device = nullptr;
        Vector3D<Float32>          boundsCenter{};
        Vector3D<Float32>          boundsExtents{};
        Vector<Vector3D<Float32>>  positions;
        Vector<ColorRGBA<Float32>> colors;
        Vector<Vector3D<Float32>>  normals;
        Vector<Vector2D<Float32>>  textureCoords;
        Vector<MeshVertex>         vertices;
        Vector<UInt32>             indices;
        IGPUMesh*                  gpuMesh = nullptr;
    };

    inline const Vector3D<Float32>& MeshContent::GetBoundsCenter() const noexcept
    {
        return boundsCenter;
    }

    inline void MeshContent::SetBoundsCenter(const Vector3D<Float32>& value) noexcept
    {
        boundsCenter = value;
    }

    inline const Vector3D<Float32>& MeshContent::GetBoundsExtents() const noexcept
    {
        return boundsExtents;
    }

    inline void MeshContent::SetBoundsExtents(const Vector3D<Float32>& value) noexcept
    {
        boundsExtents = value;
    }

    inline Span<const Vector3D<Float32>> MeshContent::GetPositions() const noexcept
    {
        return positions;
    }

    inline void MeshContent::SetPositions(Vector<Vector3D<Float32>>&& value) noexcept
    {
        positions = std::move(value);
    }

    inline Span<const ColorRGBA<Float32>> MeshContent::GetColors() const noexcept
    {
        return colors;
    }

    inline void MeshContent::SetColors(Vector<ColorRGBA<Float32>>&& value) noexcept
    {
        colors = std::move(value);
    }

    inline Span<const Vector3D<Float32>> MeshContent::GetNormals() const noexcept
    {
        return normals;
    }

    inline void MeshContent::SetNormals(Vector<Vector3D<Float32>>&& value) noexcept
    {
        normals = std::move(value);
    }

    inline Span<const Vector2D<Float32>> MeshContent::GetTextureCoords() const noexcept
    {
        return textureCoords;
    }

    inline void MeshContent::SetTextureCoords(Vector<Vector2D<Float32>>&& value) noexcept
    {
        textureCoords = std::move(value);
    }

    inline Span<const MeshVertex> MeshContent::GetVertices() const noexcept
    {
        return vertices;
    }

    inline void MeshContent::SetVertices(Vector<MeshVertex>&& value) noexcept
    {
        vertices = std::move(value);
    }

    inline Span<const UInt32> MeshContent::GetIndices() const noexcept
    {
        return indices;
    }

    inline void MeshContent::SetIndices(Vector<UInt32>&& value) noexcept
    {
        indices = std::move(value);
    }

    inline const IGPUMesh* MeshContent::GetGPUMesh() const noexcept
    {
        return gpuMesh;
    }
} // namespace Garage

#endif // GARAGE_CONTENT_MESH_CONTENT_HPP
