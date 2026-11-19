#ifndef GARAGE_RESOURCES_TERRAIN_ASSET_HPP
#define GARAGE_RESOURCES_TERRAIN_ASSET_HPP

#include <filesystem>
#include <span>
#include <vector>

#include "../core/common.hpp"
#include "../core/memory.hpp"

#include "../math/vector2d.hpp"
#include "../math/vector3d.hpp"

#include "asset.hpp"

namespace Garage
{
    class IGPUDevice;
    class IGPUMesh;

    class GARAGE_API TerrainAsset final : public AssetBase
    {
    public:
        explicit TerrainAsset(std::filesystem::path       path,
                              UInt32                      width,
                              UInt32                      height,
                              bool                        is16Bit,
                              Float32                     cellSize,
                              Float32                     heightScale,
                              const Vector2D<Float32>&    uvTiling,
                              std::pmr::vector<Float32>&& normalizedHeights,
                              IGPUDevice&                 device,
                              IGPUMesh*                   gpuMesh) noexcept;
        ~TerrainAsset() noexcept override;

        [[nodiscard]] UInt32 GetWidth() const noexcept;
        [[nodiscard]] UInt32 GetHeight() const noexcept;
        [[nodiscard]] bool   Is16Bit() const noexcept;

        [[nodiscard]] Float32                  GetCellSize() const noexcept;
        [[nodiscard]] Float32                  GetHeightScale() const noexcept;
        [[nodiscard]] const Vector2D<Float32>& GetUVTiling() const noexcept;

        [[nodiscard]] Float32 GetLocalWidth() const noexcept;
        [[nodiscard]] Float32 GetLocalDepth() const noexcept;

        [[nodiscard]] std::span<const Float32> GetNormalizedHeights() const noexcept;
        [[nodiscard]] Float32                  GetNormalizedHeightAt(UInt32 x, UInt32 z) const noexcept;
        [[nodiscard]] Float32                  SampleNormalizedHeight(Float32 gridX, Float32 gridZ) const noexcept;
        [[nodiscard]] Vector3D<Float32>        CalculateGridNormal(UInt32 x, UInt32 z) const noexcept;
        [[nodiscard]] static Vector3D<Float32> CalculateGridNormal(std::span<const Float32> normalizedHeights,
                                                                   UInt32                   width,
                                                                   UInt32                   height,
                                                                   UInt32                   x,
                                                                   UInt32                   z,
                                                                   Float32                  cellSize,
                                                                   Float32                  heightScale) noexcept;

        [[nodiscard]] const IGPUMesh* GetGPUMesh() const noexcept;

    private:
        IGPUDevice*               device      = nullptr;
        UInt32                    width       = 0;
        UInt32                    height      = 0;
        bool                      is16Bit     = false;
        Float32                   cellSize    = 4.0f;
        Float32                   heightScale = 120.0f;
        Vector2D<Float32>         uvTiling{1.0f, 1.0f};
        std::pmr::vector<Float32> normalizedHeights{Memory::GetMemoryResource()};
        IGPUMesh*                 gpuMesh = nullptr;
    };

    inline UInt32 TerrainAsset::GetWidth() const noexcept
    {
        return width;
    }

    inline UInt32 TerrainAsset::GetHeight() const noexcept
    {
        return height;
    }

    inline bool TerrainAsset::Is16Bit() const noexcept
    {
        return is16Bit;
    }

    inline Float32 TerrainAsset::GetCellSize() const noexcept
    {
        return cellSize;
    }

    inline Float32 TerrainAsset::GetHeightScale() const noexcept
    {
        return heightScale;
    }

    inline const Vector2D<Float32>& TerrainAsset::GetUVTiling() const noexcept
    {
        return uvTiling;
    }

    inline Float32 TerrainAsset::GetLocalWidth() const noexcept
    {
        if (width < 2 || cellSize <= 0.0f)
        {
            return 0.0f;
        }

        return static_cast<Float32>(width - 1) * cellSize;
    }

    inline Float32 TerrainAsset::GetLocalDepth() const noexcept
    {
        if (height < 2 || cellSize <= 0.0f)
        {
            return 0.0f;
        }

        return static_cast<Float32>(height - 1) * cellSize;
    }

    inline std::span<const Float32> TerrainAsset::GetNormalizedHeights() const noexcept
    {
        return normalizedHeights;
    }

    inline const IGPUMesh* TerrainAsset::GetGPUMesh() const noexcept
    {
        return gpuMesh;
    }
} // namespace Garage

#endif // GARAGE_RESOURCES_TERRAIN_ASSET_HPP
