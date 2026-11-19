#ifndef GARAGE_RESOURCES_TEXTURE_ASSET_IMPORTER_HPP
#define GARAGE_RESOURCES_TEXTURE_ASSET_IMPORTER_HPP

#include <filesystem>

#include "../core/common.hpp"
#include "../core/common/containers.hpp"
#include "../core/memory.hpp"

#include "asset_importer.hpp"
#include "texture_asset.hpp"

namespace Garage
{
    class AssetSubsystem;
    class IGPUDevice;

    class GARAGE_API TextureAssetImporter final : public AssetImporter<TextureAsset>
    {
    public:
        explicit TextureAssetImporter(AssetSubsystem* owner, IGPUDevice& device) noexcept;

        [[nodiscard]] Result<UniquePtr<TextureAsset>, String> Import(const std::filesystem::path& path) const override;

    private:
        IGPUDevice& device;
    };

    inline TextureAssetImporter::TextureAssetImporter(AssetSubsystem* owner, IGPUDevice& device) noexcept
        : AssetImporter<TextureAsset>(owner)
        , device(device)
    {
    }
} // namespace Garage

#endif // GARAGE_RESOURCES_TEXTURE_ASSET_IMPORTER_HPP
