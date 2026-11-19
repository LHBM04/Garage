#ifndef GARAGE_RESOURCES_MATERIAL_ASSET_IMPORTER_HPP
#define GARAGE_RESOURCES_MATERIAL_ASSET_IMPORTER_HPP

#include <filesystem>

#include "../core/common.hpp"
#include "../core/common/containers.hpp"
#include "../core/memory.hpp"

#include "../graphics/gpu_target.hpp"
#include "../graphics/gpu_types.hpp"

#include "asset_importer.hpp"
#include "material_asset.hpp"

namespace Garage
{
    class AssetSubsystem;
    class IGPUDevice;

    class GARAGE_API MaterialAssetImporter final : public AssetImporter<MaterialAsset>
    {
    public:
        explicit MaterialAssetImporter(AssetSubsystem*   owner,
                                       IGPUDevice&       device,
                                       EGPUTextureFormat targetFormat = EGPUTextureFormat::R8G8B8A8_UNORM,
                                       EGPUTextureFormat depthFormat  = GPUTarget::DEPTH_FORMAT) noexcept;

        [[nodiscard]] Result<UniquePtr<MaterialAsset>, String> Import(const std::filesystem::path& path) const override;

    private:
        IGPUDevice&       device;
        EGPUTextureFormat targetFormat = EGPUTextureFormat::R8G8B8A8_UNORM;
        EGPUTextureFormat depthFormat  = GPUTarget::DEPTH_FORMAT;
    };

    inline MaterialAssetImporter::MaterialAssetImporter(AssetSubsystem*   owner,
                                                        IGPUDevice&       device,
                                                        EGPUTextureFormat targetFormat,
                                                        EGPUTextureFormat depthFormat) noexcept
        : AssetImporter<MaterialAsset>(owner)
        , device(device)
        , targetFormat(targetFormat)
        , depthFormat(depthFormat)
    {
    }
} // namespace Garage

#endif // GARAGE_RESOURCES_MATERIAL_ASSET_IMPORTER_HPP
