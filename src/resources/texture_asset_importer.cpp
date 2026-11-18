#include "garage/resources/texture_asset_importer.hpp"

#include <filesystem>
#include <string>
#include <utility>

#include "garage/core/allocator.hpp"
#include "garage/core/types.hpp"

#include "garage/graphics/gpu_texture2d.hpp"

#include "garage/resources/texture_asset.hpp"

#include "dds_loader.hpp"

namespace Garage
{
    Result<Allocator::UniquePtr<TextureAsset>, std::pmr::string>
            TextureAssetImporter::Import(const std::filesystem::path& path) const
    {
        auto textureResult = ResourcesDetail::LoadDDSTexture(path, false, device);
        if (!textureResult)
        {
            return Result<Allocator::UniquePtr<TextureAsset>, std::pmr::string>::Err(
                    std::move(textureResult).GetError());
        }

        return Result<Allocator::UniquePtr<TextureAsset>, std::pmr::string>::Ok(
                Allocator::MakeUnique<TextureAsset>(path, device, textureResult.GetValue()));
    }
} // namespace Garage
