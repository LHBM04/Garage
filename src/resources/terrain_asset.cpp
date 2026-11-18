#include "garage/resources/terrain_asset.hpp"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <span>
#include <utility>
#include <vector>

#include "garage/core/types.hpp"

#include "garage/graphics/gpu_device.hpp"
#include "garage/graphics/gpu_mesh.hpp"

#include "garage/math/vector2d.hpp"
#include "garage/math/vector3d.hpp"

namespace Garage
{
    TerrainAsset::TerrainAsset(std::filesystem::path       path,
                               UInt32                      width,
                               UInt32                      height,
                               bool                        is16Bit,
                               Float32                     cellSize,
                               Float32                     heightScale,
                               const Vector2D<Float32>&    uvTiling,
                               std::pmr::vector<Float32>&& normalizedHeights,
                               IGPUDevice&                 device,
                               IGPUMesh*                   gpuMesh) noexcept
        : AssetBase(std::move(path))
        , device(&device)
        , width(width)
        , height(height)
        , is16Bit(is16Bit)
        , cellSize(cellSize)
        , heightScale(heightScale)
        , uvTiling(uvTiling)
        , normalizedHeights(std::move(normalizedHeights))
        , gpuMesh(gpuMesh)
    {
    }

    TerrainAsset::~TerrainAsset() noexcept
    {
        if (device != nullptr && gpuMesh != nullptr)
        {
            device->DestroyMesh(gpuMesh);
            gpuMesh = nullptr;
        }
    }

    Float32 TerrainAsset::GetNormalizedHeightAt(UInt32 x, UInt32 z) const noexcept
    {
        if (width == 0 || height == 0 || normalizedHeights.empty())
        {
            return 0.0f;
        }

        const UInt32 CLAMPED_X = std::min(x, width - 1);
        const UInt32 CLAMPED_Z = std::min(z, height - 1);
        const USize  INDEX     = static_cast<USize>(CLAMPED_Z) * width + CLAMPED_X;
        if (INDEX >= normalizedHeights.size())
        {
            return 0.0f;
        }

        return normalizedHeights[INDEX];
    }

    Float32 TerrainAsset::SampleNormalizedHeight(Float32 gridX, Float32 gridZ) const noexcept
    {
        if (width < 2 || height < 2 || normalizedHeights.empty())
        {
            return 0.0f;
        }

        const Float32 CLAMPED_X = std::clamp(gridX, 0.0f, static_cast<Float32>(width - 1));
        const Float32 CLAMPED_Z = std::clamp(gridZ, 0.0f, static_cast<Float32>(height - 1));

        const UInt32 X0 = static_cast<UInt32>(std::floor(CLAMPED_X));
        const UInt32 Z0 = static_cast<UInt32>(std::floor(CLAMPED_Z));
        const UInt32 X1 = std::min(X0 + 1, width - 1);
        const UInt32 Z1 = std::min(Z0 + 1, height - 1);

        const Float32 TX = CLAMPED_X - static_cast<Float32>(X0);
        const Float32 TZ = CLAMPED_Z - static_cast<Float32>(Z0);

        const Float32 H00 = GetNormalizedHeightAt(X0, Z0);
        const Float32 H10 = GetNormalizedHeightAt(X1, Z0);
        const Float32 H01 = GetNormalizedHeightAt(X0, Z1);
        const Float32 H11 = GetNormalizedHeightAt(X1, Z1);

        const Float32 H0 = H00 + (H10 - H00) * TX;
        const Float32 H1 = H01 + (H11 - H01) * TX;
        return H0 + (H1 - H0) * TZ;
    }

    Vector3D<Float32> TerrainAsset::CalculateGridNormal(UInt32 x, UInt32 z) const noexcept
    {
        return CalculateGridNormal(normalizedHeights, width, height, x, z, cellSize, heightScale);
    }

    Vector3D<Float32> TerrainAsset::CalculateGridNormal(std::span<const Float32> normalizedHeights,
                                                        UInt32                   width,
                                                        UInt32                   height,
                                                        UInt32                   x,
                                                        UInt32                   z,
                                                        Float32                  cellSize,
                                                        Float32                  heightScale) noexcept
    {
        if (width < 2 || height < 2 || cellSize <= 0.0f ||
            normalizedHeights.size() < static_cast<USize>(width) * height)
        {
            return Vector3D<Float32>::GetUp();
        }

        const auto SAMPLE_HEIGHT = [&](UInt32 sampleX, UInt32 sampleZ) noexcept -> Float32
        {
            const UInt32 CLAMPED_X = std::min(sampleX, width - 1);
            const UInt32 CLAMPED_Z = std::min(sampleZ, height - 1);
            return normalizedHeights[static_cast<USize>(CLAMPED_Z) * width + CLAMPED_X] * heightScale;
        };

        const UInt32 LEFT  = (x > 0) ? (x - 1) : x;
        const UInt32 RIGHT = (x + 1 < width) ? (x + 1) : x;
        const UInt32 DOWN  = (z > 0) ? (z - 1) : z;
        const UInt32 UP    = (z + 1 < height) ? (z + 1) : z;

        const Float32 HEIGHT_L = SAMPLE_HEIGHT(LEFT, z);
        const Float32 HEIGHT_R = SAMPLE_HEIGHT(RIGHT, z);
        const Float32 HEIGHT_D = SAMPLE_HEIGHT(x, DOWN);
        const Float32 HEIGHT_U = SAMPLE_HEIGHT(x, UP);

        const Float32 SPAN_X = static_cast<Float32>(RIGHT - LEFT) * cellSize;
        const Float32 SPAN_Z = static_cast<Float32>(UP - DOWN) * cellSize;

        const Vector3D<Float32> TANGENT_X{SPAN_X, HEIGHT_R - HEIGHT_L, 0.0f};
        const Vector3D<Float32> TANGENT_Z{0.0f, HEIGHT_U - HEIGHT_D, SPAN_Z};
        Vector3D<Float32>       normal = Vector3D<Float32>::Cross(TANGENT_Z, TANGENT_X);
        normal.Normalize();
        return normal;
    }
} // namespace Garage
