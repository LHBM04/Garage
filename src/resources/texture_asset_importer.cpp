#include "garage/resources/texture_asset_importer.hpp"

#include <filesystem>
#include <utility>

#include "garage/core/common.hpp"
#include "garage/core/common/containers.hpp"
#include "garage/core/memory.hpp"

#include "garage/graphics/gpu_texture2d.hpp"

#include "garage/resources/texture_asset.hpp"

#include "dds_loader.hpp"

namespace Garage
{
    Result<UniquePtr<TextureAsset>, String> TextureAssetImporter::Import(const std::filesystem::path& path) const
    {
        auto textureResult = ResourcesDetail::LoadDDSTexture(path, false, device);
        if (!textureResult)
        {
            return Result<UniquePtr<TextureAsset>, String>::Err(std::move(textureResult).GetError());
        }

        return Result<UniquePtr<TextureAsset>, String>::Ok(
                MakeUnique<TextureAsset>(path, device, textureResult.GetValue()));
    }
} // namespace Garage
