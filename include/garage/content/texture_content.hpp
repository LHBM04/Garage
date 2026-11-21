#ifndef GARAGE_CONTENT_TEXTURE_CONTENT_HPP
#define GARAGE_CONTENT_TEXTURE_CONTENT_HPP

#include <filesystem>

#include "../core/common.hpp"

#include "content.hpp"

namespace Garage
{
    class IGPUDevice;
    class IGPUTexture2D;

    class GARAGE_API TextureContent final : public ContentBase
    {
    public:
        explicit TextureContent(std::filesystem::path path, IGPUDevice& device, IGPUTexture2D* texture) noexcept;
        ~TextureContent() noexcept override;

        [[nodiscard]] const IGPUTexture2D* GetTexture() const noexcept;

    private:
        IGPUDevice*    device  = nullptr;
        IGPUTexture2D* texture = nullptr;
    };

    inline const IGPUTexture2D* TextureContent::GetTexture() const noexcept
    {
        return texture;
    }
} // namespace Garage

#endif // GARAGE_CONTENT_TEXTURE_CONTENT_HPP
