#include "garage/content/texture_content_importer.hpp"

#include <filesystem>
#include <utility>

#include "garage/content/texture_content.hpp"

#include "garage/core/common.hpp"
#include "garage/core/common/containers.hpp"
#include "garage/core/memory.hpp"

#include "garage/graphics/gpu_texture2d.hpp"

#include "dds_loader.hpp"

namespace Garage
{
    Result<UniquePtr<TextureContent>, String> TextureContentImporter::Import(const std::filesystem::path& path) const
    {
        auto textureResult = ContentDetail::LoadDDSTexture(path, false, device);
        if (!textureResult)
        {
            return Result<UniquePtr<TextureContent>, String>::Err(std::move(textureResult).GetError());
        }

        return Result<UniquePtr<TextureContent>, String>::Ok(
                MakeUnique<TextureContent>(path, device, textureResult.GetValue()));
    }
} // namespace Garage
