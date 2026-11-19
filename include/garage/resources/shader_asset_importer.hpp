#ifndef GARAGE_RESOURCES_SHADER_ASSET_IMPORTER_HPP
#define GARAGE_RESOURCES_SHADER_ASSET_IMPORTER_HPP

#include <filesystem>
#include <string>

#include "../core/common.hpp"
#include "../core/memory.hpp"

#include "asset_importer.hpp"
#include "shader_asset.hpp"

namespace Garage
{
    class AssetSubsystem;
    class IGPUDevice;

    class GARAGE_API ShaderAssetImporter final : public AssetImporter<ShaderAsset>
    {
    public:
        explicit ShaderAssetImporter(AssetSubsystem* owner, IGPUDevice& device) noexcept;

        [[nodiscard]] Result<UniquePtr<ShaderAsset>, std::pmr::string>
                Import(const std::filesystem::path& path) const override;

    private:
        IGPUDevice& device;
    };

    inline ShaderAssetImporter::ShaderAssetImporter(AssetSubsystem* owner, IGPUDevice& device) noexcept
        : AssetImporter<ShaderAsset>(owner)
        , device(device)
    {
    }
} // namespace Garage

#endif // GARAGE_RESOURCES_SHADER_ASSET_IMPORTER_HPP
