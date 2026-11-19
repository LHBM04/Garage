#ifndef GARAGE_RESOURCES_ANIMATION_ASSET_IMPORTER_HPP
#define GARAGE_RESOURCES_ANIMATION_ASSET_IMPORTER_HPP

#include <filesystem>

#include "../core/common.hpp"
#include "../core/common/containers.hpp"
#include "../core/memory.hpp"

#include "animation_asset.hpp"
#include "asset_importer.hpp"

namespace Garage
{
    class GARAGE_API AnimationAssetImporter final : public AssetImporter<AnimationAsset>
    {
    public:
        using AssetImporter<AnimationAsset>::AssetImporter;

        [[nodiscard]] Result<UniquePtr<AnimationAsset>, String>
                Import(const std::filesystem::path& path) const override;
    };
} // namespace Garage

#endif // GARAGE_RESOURCES_ANIMATION_ASSET_IMPORTER_HPP
