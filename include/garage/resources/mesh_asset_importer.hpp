#ifndef GARAGE_RESOURCES_MESH_ASSET_IMPORTER_HPP
#define GARAGE_RESOURCES_MESH_ASSET_IMPORTER_HPP

#include <filesystem>
#include <string>

#include "../core/allocator.hpp"
#include "../core/defines.hpp"
#include "../core/types.hpp"

#include "asset_importer.hpp"
#include "mesh_asset.hpp"

namespace Garage
{
    class AssetSubsystem;
    class IGPUDevice;

    class GARAGE_API MeshAssetImporter final : public AssetImporter<MeshAsset>
    {
    public:
        explicit MeshAssetImporter(AssetSubsystem* owner, IGPUDevice& device) noexcept;

        [[nodiscard]] Result<Allocator::UniquePtr<MeshAsset>, std::pmr::string>
                Import(const std::filesystem::path& path) const override;

    private:
        IGPUDevice& device;
    };

    inline MeshAssetImporter::MeshAssetImporter(AssetSubsystem* owner, IGPUDevice& device) noexcept
        : AssetImporter<MeshAsset>(owner)
        , device(device)
    {
    }
} // namespace Garage

#endif // GARAGE_RESOURCES_MESH_ASSET_IMPORTER_HPP
