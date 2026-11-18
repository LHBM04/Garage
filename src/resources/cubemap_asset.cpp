#include "garage/resources/cubemap_asset.hpp"

#include <filesystem>
#include <utility>

#include "garage/graphics/gpu_device.hpp"
#include "garage/graphics/gpu_texture2d.hpp"

namespace Garage
{
    CubemapAsset::CubemapAsset(std::filesystem::path path, IGPUDevice& device, IGPUTexture2D* texture) noexcept
        : AssetBase(std::move(path))
        , device(&device)
        , texture(texture)
    {
    }

    CubemapAsset::~CubemapAsset() noexcept
    {
        if (device != nullptr && texture != nullptr)
        {
            device->DestroyTexture2D(texture);
            texture = nullptr;
        }
    }
} // namespace Garage
