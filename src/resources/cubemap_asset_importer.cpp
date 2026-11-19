#include "garage/resources/cubemap_asset_importer.hpp"

#include <filesystem>
#include <utility>

#include "garage/core/common.hpp"
#include "garage/core/common/containers.hpp"
#include "garage/core/memory.hpp"

#include "garage/graphics/gpu_texture2d.hpp"

#include "garage/resources/cubemap_asset.hpp"

#include "dds_loader.hpp"

namespace Garage
{
    Result<UniquePtr<CubemapAsset>, String> CubemapAssetImporter::Import(const std::filesystem::path& path) const
    {
        auto textureResult = ResourcesDetail::LoadDDSTexture(path, true, device);
        if (!textureResult)
        {
            return Result<UniquePtr<CubemapAsset>, String>::Err(std::move(textureResult).GetError());
        }

        return Result<UniquePtr<CubemapAsset>, String>::Ok(
                MakeUnique<CubemapAsset>(path, device, textureResult.GetValue()));
    }
} // namespace Garage
