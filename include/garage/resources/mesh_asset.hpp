#ifndef GARAGE_RESOURCES_MESH_ASSET_HPP
#define GARAGE_RESOURCES_MESH_ASSET_HPP

#include <filesystem>
#include <utility>

#include "../core/common.hpp"
#include "../core/common/containers.hpp"

#include "../graphics/mesh_vertex.hpp"

#include "../math/color_rgba.hpp"
#include "../math/vector2d.hpp"
#include "../math/vector3d.hpp"

#include "asset.hpp"

namespace Garage
{
    class IGPUDevice;
    class IGPUMesh;

    class GARAGE_API MeshAsset final : public AssetBase
    {
    public:
        explicit MeshAsset(std::filesystem::path path, IGPUDevice& device, IGPUMesh* gpuMesh) noexcept;
        ~MeshAsset() noexcept override;

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

    inline const Vector3D<Float32>& MeshAsset::GetBoundsCenter() const noexcept
    {
        return boundsCenter;
    }

    inline void MeshAsset::SetBoundsCenter(const Vector3D<Float32>& value) noexcept
    {
        boundsCenter = value;
    }

    inline const Vector3D<Float32>& MeshAsset::GetBoundsExtents() const noexcept
    {
        return boundsExtents;
    }

    inline void MeshAsset::SetBoundsExtents(const Vector3D<Float32>& value) noexcept
    {
        boundsExtents = value;
    }

    inline Span<const Vector3D<Float32>> MeshAsset::GetPositions() const noexcept
    {
        return positions;
    }

    inline void MeshAsset::SetPositions(Vector<Vector3D<Float32>>&& value) noexcept
    {
        positions = std::move(value);
    }

    inline Span<const ColorRGBA<Float32>> MeshAsset::GetColors() const noexcept
    {
        return colors;
    }

    inline void MeshAsset::SetColors(Vector<ColorRGBA<Float32>>&& value) noexcept
    {
        colors = std::move(value);
    }

    inline Span<const Vector3D<Float32>> MeshAsset::GetNormals() const noexcept
    {
        return normals;
    }

    inline void MeshAsset::SetNormals(Vector<Vector3D<Float32>>&& value) noexcept
    {
        normals = std::move(value);
    }

    inline Span<const Vector2D<Float32>> MeshAsset::GetTextureCoords() const noexcept
    {
        return textureCoords;
    }

    inline void MeshAsset::SetTextureCoords(Vector<Vector2D<Float32>>&& value) noexcept
    {
        textureCoords = std::move(value);
    }

    inline Span<const MeshVertex> MeshAsset::GetVertices() const noexcept
    {
        return vertices;
    }

    inline void MeshAsset::SetVertices(Vector<MeshVertex>&& value) noexcept
    {
        vertices = std::move(value);
    }

    inline Span<const UInt32> MeshAsset::GetIndices() const noexcept
    {
        return indices;
    }

    inline void MeshAsset::SetIndices(Vector<UInt32>&& value) noexcept
    {
        indices = std::move(value);
    }

    inline const IGPUMesh* MeshAsset::GetGPUMesh() const noexcept
    {
        return gpuMesh;
    }
} // namespace Garage

#endif // GARAGE_RESOURCES_MESH_ASSET_HPP
