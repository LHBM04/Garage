#include "garage/content/cubemap_content_importer.hpp"

#include <filesystem>
#include <utility>

#include "garage/content/cubemap_content.hpp"

#include "garage/core/common.hpp"
#include "garage/core/common/containers.hpp"
#include "garage/core/memory.hpp"

#include "garage/graphics/gpu_texture2d.hpp"

#include "dds_loader.hpp"

namespace Garage
{
    Result<UniquePtr<CubemapContent>, String> CubemapContentImporter::Import(const std::filesystem::path& path) const
    {
        auto textureResult = ContentDetail::LoadDDSTexture(path, true, device);
        if (!textureResult)
        {
            return Result<UniquePtr<CubemapContent>, String>::Err(std::move(textureResult).GetError());
        }

        return Result<UniquePtr<CubemapContent>, String>::Ok(
                MakeUnique<CubemapContent>(path, device, textureResult.GetValue()));
    }
} // namespace Garage
