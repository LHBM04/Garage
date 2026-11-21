#ifndef GARAGE_CONTENT_MATERIAL_CONTENT_IMPORTER_HPP
#define GARAGE_CONTENT_MATERIAL_CONTENT_IMPORTER_HPP

#include <filesystem>

#include "../core/common.hpp"
#include "../core/common/containers.hpp"
#include "../core/memory.hpp"

#include "../graphics/gpu_target.hpp"
#include "../graphics/gpu_types.hpp"

#include "content_importer.hpp"
#include "material_content.hpp"

namespace Garage
{
    class ContentSubsystem;
    class IGPUDevice;

    class GARAGE_API MaterialContentImporter final : public ContentImporter<MaterialContent>
    {
    public:
        explicit MaterialContentImporter(ContentSubsystem* owner,
                                         IGPUDevice&       device,
                                         EGPUTextureFormat targetFormat = EGPUTextureFormat::R8G8B8A8_UNORM,
                                         EGPUTextureFormat depthFormat  = GPUTarget::DEPTH_FORMAT) noexcept;

        [[nodiscard]] Result<UniquePtr<MaterialContent>, String>
                Import(const std::filesystem::path& path) const override;

    private:
        IGPUDevice&       device;
        EGPUTextureFormat targetFormat = EGPUTextureFormat::R8G8B8A8_UNORM;
        EGPUTextureFormat depthFormat  = GPUTarget::DEPTH_FORMAT;
    };

    inline MaterialContentImporter::MaterialContentImporter(ContentSubsystem* owner,
                                                            IGPUDevice&       device,
                                                            EGPUTextureFormat targetFormat,
                                                            EGPUTextureFormat depthFormat) noexcept
        : ContentImporter<MaterialContent>(owner)
        , device(device)
        , targetFormat(targetFormat)
        , depthFormat(depthFormat)
    {
    }
} // namespace Garage

#endif // GARAGE_CONTENT_MATERIAL_CONTENT_IMPORTER_HPP
