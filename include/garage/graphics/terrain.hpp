#ifndef GARAGE_GRAPHICS_TERRAIN_HPP
#define GARAGE_GRAPHICS_TERRAIN_HPP

#include "../actors/component.hpp"

#include "../core/common.hpp"

#include "../math/mathf.hpp"
#include "../math/vector2d.hpp"
#include "../math/vector3d.hpp"

namespace Garage
{
    template <Scalarable TScalar>
    class Matrix4x4;

    class TerrainContent;

    class GARAGE_API Terrain final : public ComponentBase
    {
    public:
        using ComponentBase::ComponentBase;
        ~Terrain() noexcept override = default;

        [[nodiscard]] const TerrainContent* GetTerrain() const noexcept;
        void                                SetTerrain(const TerrainContent* value) noexcept;

        [[nodiscard]] Float32                  GetCellSize() const noexcept;
        [[nodiscard]] Float32                  GetHeightScale() const noexcept;
        [[nodiscard]] const Vector2D<Float32>& GetUVTiling() const noexcept;

        [[nodiscard]] bool IsValid() const noexcept;

        [[nodiscard]] Float32 GetLocalWidth() const noexcept;
        [[nodiscard]] Float32 GetLocalDepth() const noexcept;

        [[nodiscard]] bool              ContainsLocalPosition(Float32 localX, Float32 localZ) const noexcept;
        [[nodiscard]] Float32           SampleHeightAtLocalPosition(Float32 localX, Float32 localZ) const noexcept;
        [[nodiscard]] Vector3D<Float32> SampleNormalAtLocalPosition(Float32 localX, Float32 localZ) const noexcept;

        [[nodiscard]] bool    ContainsWorldPosition(const Vector3D<Float32>& worldPosition) const noexcept;
        [[nodiscard]] Float32 SampleHeightAtWorldPosition(const Vector3D<Float32>& worldPosition) const noexcept;
        [[nodiscard]] Vector3D<Float32>
                SampleNormalAtWorldPosition(const Vector3D<Float32>& worldPosition) const noexcept;

    private:
        [[nodiscard]] bool TryGetWorldMatrices(Matrix4x4<Float32>& outWorld,
                                               Matrix4x4<Float32>& outInverseWorld) const noexcept;

        const TerrainContent* content = nullptr;
    };

    inline const TerrainContent* Terrain::GetTerrain() const noexcept
    {
        return content;
    }

    inline void Terrain::SetTerrain(const TerrainContent* value) noexcept
    {
        content = value;
    }
} // namespace Garage

#endif // GARAGE_GRAPHICS_TERRAIN_HPP
