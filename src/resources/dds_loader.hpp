#ifndef GARAGE_RESOURCES_DDS_LOADER_HPP
#define GARAGE_RESOURCES_DDS_LOADER_HPP

#include <filesystem>
#include <string>

#include "garage/core/common.hpp"
#include "garage/core/memory.hpp"

namespace Garage
{
    class IGPUDevice;
    class IGPUTexture2D;

    namespace ResourcesDetail
    {
        [[nodiscard]] Result<IGPUTexture2D*, std::pmr::string>
                LoadDDSTexture(const std::filesystem::path& path, bool expectCubemap, IGPUDevice& device);
    } // namespace ResourcesDetail
} // namespace Garage

#endif // GARAGE_RESOURCES_DDS_LOADER_HPP
