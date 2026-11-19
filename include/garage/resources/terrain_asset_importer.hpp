#ifndef GARAGE_RESOURCES_TERRAIN_ASSET_IMPORTER_HPP
#define GARAGE_RESOURCES_TERRAIN_ASSET_IMPORTER_HPP

#include <filesystem>
#include <string>

#include "../core/common.hpp"
#include "../core/memory.hpp"

#include "asset_importer.hpp"
#include "terrain_asset.hpp"

namespace Garage
{
    class AssetSubsystem;
    class IGPUDevice;

    class GARAGE_API TerrainAssetImporter final : public AssetImporter<TerrainAsset>
    {
    public:
        explicit TerrainAssetImporter(AssetSubsystem* owner, IGPUDevice& device) noexcept;

        [[nodiscard]] Result<UniquePtr<TerrainAsset>, std::pmr::string>
                Import(const std::filesystem::path& path) const override;

    private:
        IGPUDevice& device;
    };

    inline TerrainAssetImporter::TerrainAssetImporter(AssetSubsystem* owner, IGPUDevice& device) noexcept
        : AssetImporter<TerrainAsset>(owner)
        , device(device)
    {
    }
} // namespace Garage

#endif // GARAGE_RESOURCES_TERRAIN_ASSET_IMPORTER_HPP
