#ifndef GARAGE_CONTENT_CUBEMAP_CONTENT_HPP
#define GARAGE_CONTENT_CUBEMAP_CONTENT_HPP

#include <filesystem>

#include "../core/common.hpp"

#include "content.hpp"

namespace Garage
{
    class IGPUDevice;
    class IGPUTexture2D;

    class GARAGE_API CubemapContent final : public ContentBase
    {
    public:
        explicit CubemapContent(std::filesystem::path path, IGPUDevice& device, IGPUTexture2D* texture) noexcept;
        ~CubemapContent() noexcept override;

        [[nodiscard]] const IGPUTexture2D* GetTexture() const noexcept;

    private:
        IGPUDevice*    device  = nullptr;
        IGPUTexture2D* texture = nullptr;
    };

    inline const IGPUTexture2D* CubemapContent::GetTexture() const noexcept
    {
        return texture;
    }
} // namespace Garage

#endif // GARAGE_CONTENT_CUBEMAP_CONTENT_HPP
