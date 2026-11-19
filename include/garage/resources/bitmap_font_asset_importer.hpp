#ifndef GARAGE_RESOURCES_BITMAP_FONT_ASSET_IMPORTER_HPP
#define GARAGE_RESOURCES_BITMAP_FONT_ASSET_IMPORTER_HPP

#include <filesystem>

#include "../core/common.hpp"
#include "../core/common/containers.hpp"
#include "../core/memory.hpp"

#include "asset_importer.hpp"
#include "bitmap_font_asset.hpp"

namespace Garage
{
    class GARAGE_API BitmapFontAssetImporter final : public AssetImporter<BitmapFontAsset>
    {
    public:
        using AssetImporter<BitmapFontAsset>::AssetImporter;

        [[nodiscard]] Result<UniquePtr<BitmapFontAsset>, String>
                Import(const std::filesystem::path& path) const override;
    };
} // namespace Garage

#endif // GARAGE_RESOURCES_BITMAP_FONT_ASSET_IMPORTER_HPP
