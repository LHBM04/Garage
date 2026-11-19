#ifndef GARAGE_RESOURCES_DDS_LOADER_HPP
#define GARAGE_RESOURCES_DDS_LOADER_HPP

#include <filesystem>

#include "garage/core/common.hpp"
#include "garage/core/common/containers.hpp"

namespace Garage
{
    class IGPUDevice;
    class IGPUTexture2D;

    namespace ResourcesDetail
    {
        [[nodiscard]] Result<IGPUTexture2D*, String>
                LoadDDSTexture(const std::filesystem::path& path, bool expectCubemap, IGPUDevice& device);
    } // namespace ResourcesDetail
} // namespace Garage

#endif // GARAGE_RESOURCES_DDS_LOADER_HPP
