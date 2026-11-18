#ifndef GARAGE_RESOURCES_MESH_ASSET_HPP
#define GARAGE_RESOURCES_MESH_ASSET_HPP

#include <filesystem>
#include <span>
#include <utility>
#include <vector>

#include "../core/allocator.hpp"
#include "../core/defines.hpp"
#include "../core/types.hpp"

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

        [[nodiscard]] std::span<const Vector3D<Float32>> GetPositions() const noexcept;
        void SetPositions(std::pmr::vector<Vector3D<Float32>>&& value) noexcept;

        [[nodiscard]] std::span<const ColorRGBA<Float32>> GetColors() const noexcept;
        void SetColors(std::pmr::vector<ColorRGBA<Float32>>&& value) noexcept;

        [[nodiscard]] std::span<const Vector3D<Float32>> GetNormals() const noexcept;
        void SetNormals(std::pmr::vector<Vector3D<Float32>>&& value) noexcept;

        [[nodiscard]] std::span<const Vector2D<Float32>> GetTextureCoords() const noexcept;
        void SetTextureCoords(std::pmr::vector<Vector2D<Float32>>&& value) noexcept;

        [[nodiscard]] std::span<const MeshVertex> GetVertices() const noexcept;
        void                                      SetVertices(std::pmr::vector<MeshVertex>&& value) noexcept;

        [[nodiscard]] std::span<const UInt32> GetIndices() const noexcept;
        void                                  SetIndices(std::pmr::vector<UInt32>&& value) noexcept;

        [[nodiscard]] const IGPUMesh* GetGPUMesh() const noexcept;

    private:
        IGPUDevice*                          device = nullptr;
        Vector3D<Float32>                    boundsCenter{};
        Vector3D<Float32>                    boundsExtents{};
        std::pmr::vector<Vector3D<Float32>>  positions{Allocator::GetMemoryResource()};
        std::pmr::vector<ColorRGBA<Float32>> colors{Allocator::GetMemoryResource()};
        std::pmr::vector<Vector3D<Float32>>  normals{Allocator::GetMemoryResource()};
        std::pmr::vector<Vector2D<Float32>>  textureCoords{Allocator::GetMemoryResource()};
        std::pmr::vector<MeshVertex>         vertices{Allocator::GetMemoryResource()};
        std::pmr::vector<UInt32>             indices{Allocator::GetMemoryResource()};
        IGPUMesh*                            gpuMesh = nullptr;
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

    inline std::span<const Vector3D<Float32>> MeshAsset::GetPositions() const noexcept
    {
        return positions;
    }

    inline void MeshAsset::SetPositions(std::pmr::vector<Vector3D<Float32>>&& value) noexcept
    {
        positions = std::move(value);
    }

    inline std::span<const ColorRGBA<Float32>> MeshAsset::GetColors() const noexcept
    {
        return colors;
    }

    inline void MeshAsset::SetColors(std::pmr::vector<ColorRGBA<Float32>>&& value) noexcept
    {
        colors = std::move(value);
    }

    inline std::span<const Vector3D<Float32>> MeshAsset::GetNormals() const noexcept
    {
        return normals;
    }

    inline void MeshAsset::SetNormals(std::pmr::vector<Vector3D<Float32>>&& value) noexcept
    {
        normals = std::move(value);
    }

    inline std::span<const Vector2D<Float32>> MeshAsset::GetTextureCoords() const noexcept
    {
        return textureCoords;
    }

    inline void MeshAsset::SetTextureCoords(std::pmr::vector<Vector2D<Float32>>&& value) noexcept
    {
        textureCoords = std::move(value);
    }

    inline std::span<const MeshVertex> MeshAsset::GetVertices() const noexcept
    {
        return vertices;
    }

    inline void MeshAsset::SetVertices(std::pmr::vector<MeshVertex>&& value) noexcept
    {
        vertices = std::move(value);
    }

    inline std::span<const UInt32> MeshAsset::GetIndices() const noexcept
    {
        return indices;
    }

    inline void MeshAsset::SetIndices(std::pmr::vector<UInt32>&& value) noexcept
    {
        indices = std::move(value);
    }

    inline const IGPUMesh* MeshAsset::GetGPUMesh() const noexcept
    {
        return gpuMesh;
    }
} // namespace Garage

#endif // GARAGE_RESOURCES_MESH_ASSET_HPP
