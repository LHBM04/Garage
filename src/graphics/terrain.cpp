#include "garage/graphics/terrain.hpp"

#include <algorithm>
#include <cmath>

#include "garage/actors/actor.hpp"
#include "garage/actors/transform.hpp"

#include "garage/content/terrain_content.hpp"

#include "garage/core/common.hpp"

#include "garage/math/matrix4x4.hpp"
#include "garage/math/vector2d.hpp"
#include "garage/math/vector3d.hpp"

namespace Garage
{
    Float32 Terrain::GetCellSize() const noexcept
    {
        return (content != nullptr) ? content->GetCellSize() : 0.0f;
    }

    Float32 Terrain::GetHeightScale() const noexcept
    {
        return (content != nullptr) ? content->GetHeightScale() : 0.0f;
    }

    const Vector2D<Float32>& Terrain::GetUVTiling() const noexcept
    {
        static const Vector2D<Float32> DEFAULT_UV_TILING{1.0f, 1.0f};
        return (content != nullptr) ? content->GetUVTiling() : DEFAULT_UV_TILING;
    }

    bool Terrain::IsValid() const noexcept
    {
        return content != nullptr && content->GetWidth() >= 2 && content->GetHeight() >= 2 &&
                content->GetCellSize() > 0.0f;
    }

    Float32 Terrain::GetLocalWidth() const noexcept
    {
        return IsValid() ? content->GetLocalWidth() : 0.0f;
    }

    Float32 Terrain::GetLocalDepth() const noexcept
    {
        return IsValid() ? content->GetLocalDepth() : 0.0f;
    }

    bool Terrain::ContainsLocalPosition(Float32 localX, Float32 localZ) const noexcept
    {
        if (!IsValid())
        {
            return false;
        }

        const Float32 HALF_WIDTH = GetLocalWidth() * 0.5f;
        const Float32 HALF_DEPTH = GetLocalDepth() * 0.5f;

        return localX >= -HALF_WIDTH && localX <= HALF_WIDTH && localZ >= -HALF_DEPTH && localZ <= HALF_DEPTH;
    }

    Float32 Terrain::SampleHeightAtLocalPosition(Float32 localX, Float32 localZ) const noexcept
    {
        if (!IsValid())
        {
            return 0.0f;
        }

        const Float32 CELL_SIZE  = content->GetCellSize();
        const Float32 HALF_WIDTH = GetLocalWidth() * 0.5f;
        const Float32 HALF_DEPTH = GetLocalDepth() * 0.5f;
        const Float32 GRID_X     = (localX + HALF_WIDTH) / CELL_SIZE;
        const Float32 GRID_Z     = (localZ + HALF_DEPTH) / CELL_SIZE;

        return content->SampleNormalizedHeight(GRID_X, GRID_Z) * content->GetHeightScale();
    }

    Vector3D<Float32> Terrain::SampleNormalAtLocalPosition(Float32 localX, Float32 localZ) const noexcept
    {
        if (!IsValid())
        {
            return Vector3D<Float32>::GetUp();
        }

        const UInt32  WIDTH      = content->GetWidth();
        const UInt32  HEIGHT     = content->GetHeight();
        const Float32 CELL_SIZE  = content->GetCellSize();
        const Float32 HALF_WIDTH = GetLocalWidth() * 0.5f;
        const Float32 HALF_DEPTH = GetLocalDepth() * 0.5f;

        const Float32 GRID_X = std::clamp((localX + HALF_WIDTH) / CELL_SIZE, 0.0f, static_cast<Float32>(WIDTH - 1));
        const Float32 GRID_Z = std::clamp((localZ + HALF_DEPTH) / CELL_SIZE, 0.0f, static_cast<Float32>(HEIGHT - 1));

        const UInt32 X0 = static_cast<UInt32>(std::floor(GRID_X));
        const UInt32 Z0 = static_cast<UInt32>(std::floor(GRID_Z));
        const UInt32 X1 = std::min(X0 + 1, WIDTH - 1);
        const UInt32 Z1 = std::min(Z0 + 1, HEIGHT - 1);

        const Float32 TX = GRID_X - static_cast<Float32>(X0);
        const Float32 TZ = GRID_Z - static_cast<Float32>(Z0);

        const Vector3D<Float32> N00 = content->CalculateGridNormal(X0, Z0);
        const Vector3D<Float32> N10 = content->CalculateGridNormal(X1, Z0);
        const Vector3D<Float32> N01 = content->CalculateGridNormal(X0, Z1);
        const Vector3D<Float32> N11 = content->CalculateGridNormal(X1, Z1);

        const Vector3D<Float32> N0     = Vector3D<Float32>::Lerp(N00, N10, TX);
        const Vector3D<Float32> N1     = Vector3D<Float32>::Lerp(N01, N11, TX);
        Vector3D<Float32>       normal = Vector3D<Float32>::Lerp(N0, N1, TZ);
        normal.Normalize();
        return normal;
    }

    bool Terrain::ContainsWorldPosition(const Vector3D<Float32>& worldPosition) const noexcept
    {
        if (!IsValid())
        {
            return false;
        }

        Matrix4x4<Float32> worldMatrix{};
        Matrix4x4<Float32> inverseWorld{};
        if (!TryGetWorldMatrices(worldMatrix, inverseWorld))
        {
            return ContainsLocalPosition(worldPosition.GetX(), worldPosition.GetZ());
        }

        const Vector3D<Float32> LOCAL_POSITION = inverseWorld.MultiplyPoint3x4(worldPosition);
        return ContainsLocalPosition(LOCAL_POSITION.GetX(), LOCAL_POSITION.GetZ());
    }

    Float32 Terrain::SampleHeightAtWorldPosition(const Vector3D<Float32>& worldPosition) const noexcept
    {
        if (!IsValid())
        {
            return 0.0f;
        }

        Matrix4x4<Float32> worldMatrix{};
        Matrix4x4<Float32> inverseWorld{};
        if (!TryGetWorldMatrices(worldMatrix, inverseWorld))
        {
            return SampleHeightAtLocalPosition(worldPosition.GetX(), worldPosition.GetZ());
        }

        const Vector3D<Float32> LOCAL_POSITION = inverseWorld.MultiplyPoint3x4(worldPosition);
        const Float32 LOCAL_HEIGHT = SampleHeightAtLocalPosition(LOCAL_POSITION.GetX(), LOCAL_POSITION.GetZ());
        const Vector3D<Float32> WORLD_SURFACE = worldMatrix.MultiplyPoint3x4(
                Vector3D<Float32>(LOCAL_POSITION.GetX(), LOCAL_HEIGHT, LOCAL_POSITION.GetZ()));
        return WORLD_SURFACE.GetY();
    }

    Vector3D<Float32> Terrain::SampleNormalAtWorldPosition(const Vector3D<Float32>& worldPosition) const noexcept
    {
        if (!IsValid())
        {
            return Vector3D<Float32>::GetUp();
        }

        Matrix4x4<Float32> worldMatrix{};
        Matrix4x4<Float32> inverseWorld{};
        if (!TryGetWorldMatrices(worldMatrix, inverseWorld))
        {
            return SampleNormalAtLocalPosition(worldPosition.GetX(), worldPosition.GetZ());
        }

        const Vector3D<Float32> LOCAL_POSITION = inverseWorld.MultiplyPoint3x4(worldPosition);
        const Vector3D<Float32> LOCAL_NORMAL =
                SampleNormalAtLocalPosition(LOCAL_POSITION.GetX(), LOCAL_POSITION.GetZ());
        Vector3D<Float32> worldNormal = inverseWorld.GetTranspose().MultiplyVector(LOCAL_NORMAL);
        worldNormal.Normalize();
        return worldNormal;
    }

    bool Terrain::TryGetWorldMatrices(Matrix4x4<Float32>& outWorld, Matrix4x4<Float32>& outInverseWorld) const noexcept
    {
        const ActorBase* owner     = GetOwner();
        const Transform* transform = (owner != nullptr) ? owner->GetComponent<Transform>() : nullptr;
        if (transform == nullptr)
        {
            return false;
        }

        outWorld = transform->GetWorldMatrix();
        return outWorld.TryGetInverse(outInverseWorld);
    }
} // namespace Garage
