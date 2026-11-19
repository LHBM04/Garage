#ifndef GARAGE_RESOURCES_MODEL_ASSET_HPP
#define GARAGE_RESOURCES_MODEL_ASSET_HPP

#include <filesystem>
#include <utility>

#include "../core/common.hpp"
#include "../core/common/containers.hpp"

#include "../math/quaternion.hpp"
#include "../math/vector3d.hpp"

#include "asset.hpp"

namespace Garage
{
    class MaterialAsset;
    class MeshAsset;

    struct GARAGE_API Submodel final
    {
        Int32  parentIndex = -1;
        String name;

        Vector3D<Float32>   localPosition{0.0f, 0.0f, 0.0f};
        Quaternion<Float32> localRotation{0.0f, 0.0f, 0.0f, 1.0f};
        Vector3D<Float32>   localScale{1.0f, 1.0f, 1.0f};

        const MeshAsset*     mesh     = nullptr;
        const MaterialAsset* material = nullptr;
    };

    class GARAGE_API ModelAsset final : public AssetBase
    {
    public:
        explicit ModelAsset(std::filesystem::path path, Vector<Submodel>&& submodels) noexcept;

        [[nodiscard]] Span<const Submodel> GetSubmodels() const noexcept;

    private:
        Vector<Submodel> submodels;
    };

    inline ModelAsset::ModelAsset(std::filesystem::path path, Vector<Submodel>&& submodels) noexcept
        : AssetBase(std::move(path))
        , submodels(std::move(submodels))
    {
    }

    inline Span<const Submodel> ModelAsset::GetSubmodels() const noexcept
    {
        return submodels;
    }
} // namespace Garage

#endif // GARAGE_RESOURCES_MODEL_ASSET_HPP
