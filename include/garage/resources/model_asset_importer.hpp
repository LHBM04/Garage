#ifndef GARAGE_RESOURCES_MODEL_ASSET_IMPORTER_HPP
#define GARAGE_RESOURCES_MODEL_ASSET_IMPORTER_HPP

#include <filesystem>

#include "../core/common.hpp"
#include "../core/common/containers.hpp"
#include "../core/memory.hpp"

#include "asset_importer.hpp"
#include "model_asset.hpp"

namespace Garage
{
    class GARAGE_API ModelAssetImporter final : public AssetImporter<ModelAsset>
    {
    public:
        using AssetImporter<ModelAsset>::AssetImporter;

        [[nodiscard]] Result<UniquePtr<ModelAsset>, String> Import(const std::filesystem::path& path) const override;
    };
} // namespace Garage

#endif // GARAGE_RESOURCES_MODEL_ASSET_IMPORTER_HPP
