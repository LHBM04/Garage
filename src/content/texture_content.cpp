#include "garage/content/texture_content.hpp"

#include <filesystem>
#include <utility>

#include "garage/graphics/gpu_device.hpp"
#include "garage/graphics/gpu_texture2d.hpp"

namespace Garage
{
    TextureContent::TextureContent(std::filesystem::path path, IGPUDevice& device, IGPUTexture2D* texture) noexcept
        : ContentBase(std::move(path))
        , device(&device)
        , texture(texture)
    {
    }

    TextureContent::~TextureContent() noexcept
    {
        if (device != nullptr && texture != nullptr)
        {
            device->DestroyTexture2D(texture);
            texture = nullptr;
        }
    }
} // namespace Garage
