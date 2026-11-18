#include "garage/resources/mesh_asset.hpp"

#include <filesystem>
#include <utility>

#include "garage/graphics/gpu_device.hpp"
#include "garage/graphics/gpu_mesh.hpp"

namespace Garage
{
    MeshAsset::MeshAsset(std::filesystem::path path, IGPUDevice& device, IGPUMesh* gpuMesh) noexcept
        : AssetBase(std::move(path))
        , device(&device)
        , gpuMesh(gpuMesh)
    {
    }

    MeshAsset::~MeshAsset() noexcept
    {
        if (device != nullptr && gpuMesh != nullptr)
        {
            device->DestroyMesh(gpuMesh);
            gpuMesh = nullptr;
        }
    }
} // namespace Garage
