#include "garage/content/mesh_content.hpp"

#include <filesystem>
#include <utility>

#include "garage/graphics/gpu_device.hpp"
#include "garage/graphics/gpu_mesh.hpp"

namespace Garage
{
    MeshContent::MeshContent(std::filesystem::path path, IGPUDevice& device, IGPUMesh* gpuMesh) noexcept
        : ContentBase(std::move(path))
        , device(&device)
        , gpuMesh(gpuMesh)
    {
    }

    MeshContent::~MeshContent() noexcept
    {
        if (device != nullptr && gpuMesh != nullptr)
        {
            device->DestroyMesh(gpuMesh);
            gpuMesh = nullptr;
        }
    }
} // namespace Garage
