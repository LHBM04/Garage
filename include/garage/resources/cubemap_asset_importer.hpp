#ifndef GARAGE_RESOURCES_CUBEMAP_ASSET_IMPORTER_HPP
#define GARAGE_RESOURCES_CUBEMAP_ASSET_IMPORTER_HPP

#include <filesystem>
#include <string>

#include "../core/common.hpp"
#include "../core/memory.hpp"

#include "asset_importer.hpp"
#include "cubemap_asset.hpp"

namespace Garage
{
    class AssetSubsystem;
    class IGPUDevice;

    class GARAGE_API CubemapAssetImporter final : public AssetImporter<CubemapAsset>
    {
    public:
        explicit CubemapAssetImporter(AssetSubsystem* owner, IGPUDevice& device) noexcept;

        [[nodiscard]] Result<UniquePtr<CubemapAsset>, std::pmr::string>
                Import(const std::filesystem::path& path) const override;

    private:
        IGPUDevice& device;
    };

    inline CubemapAssetImporter::CubemapAssetImporter(AssetSubsystem* owner, IGPUDevice& device) noexcept
        : AssetImporter<CubemapAsset>(owner)
        , device(device)
    {
    }
} // namespace Garage

#endif // GARAGE_RESOURCES_CUBEMAP_ASSET_IMPORTER_HPP
