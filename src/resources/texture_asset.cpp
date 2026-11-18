#include "garage/resources/texture_asset.hpp"

#include <filesystem>
#include <utility>

#include "garage/graphics/gpu_device.hpp"
#include "garage/graphics/gpu_texture2d.hpp"

namespace Garage
{
    TextureAsset::TextureAsset(std::filesystem::path path, IGPUDevice& device, IGPUTexture2D* texture) noexcept
        : AssetBase(std::move(path))
        , device(&device)
        , texture(texture)
    {
    }

    TextureAsset::~TextureAsset() noexcept
    {
        if (device != nullptr && texture != nullptr)
        {
            device->DestroyTexture2D(texture);
            texture = nullptr;
        }
    }
} // namespace Garage
