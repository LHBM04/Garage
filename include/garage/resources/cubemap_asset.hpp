#ifndef GARAGE_RESOURCES_CUBEMAP_ASSET_HPP
#define GARAGE_RESOURCES_CUBEMAP_ASSET_HPP

#include <filesystem>

#include "../core/common.hpp"

#include "asset.hpp"

namespace Garage
{
    class IGPUDevice;
    class IGPUTexture2D;

    class GARAGE_API CubemapAsset final : public AssetBase
    {
    public:
        explicit CubemapAsset(std::filesystem::path path, IGPUDevice& device, IGPUTexture2D* texture) noexcept;
        ~CubemapAsset() noexcept override;

        [[nodiscard]] const IGPUTexture2D* GetTexture() const noexcept;

    private:
        IGPUDevice*    device  = nullptr;
        IGPUTexture2D* texture = nullptr;
    };

    inline const IGPUTexture2D* CubemapAsset::GetTexture() const noexcept
    {
        return texture;
    }
} // namespace Garage

#endif // GARAGE_RESOURCES_CUBEMAP_ASSET_HPP
