#ifndef GARAGE_RESOURCES_MODEL_ASSET_IMPORTER_HPP
#define GARAGE_RESOURCES_MODEL_ASSET_IMPORTER_HPP

#include <filesystem>
#include <string>

#include "../core/defines.hpp"
#include "../core/memory.hpp"
#include "../core/types.hpp"

#include "asset_importer.hpp"
#include "model_asset.hpp"

namespace Garage
{
    class GARAGE_API ModelAssetImporter final : public AssetImporter<ModelAsset>
    {
    public:
        using AssetImporter<ModelAsset>::AssetImporter;

        [[nodiscard]] Result<UniquePtr<ModelAsset>, std::pmr::string>
                Import(const std::filesystem::path& path) const override;
    };
} // namespace Garage

#endif // GARAGE_RESOURCES_MODEL_ASSET_IMPORTER_HPP
