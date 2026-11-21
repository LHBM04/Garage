#ifndef GARAGE_CONTENT_CUBEMAP_CONTENT_IMPORTER_HPP
#define GARAGE_CONTENT_CUBEMAP_CONTENT_IMPORTER_HPP

#include <filesystem>

#include "../core/common.hpp"
#include "../core/common/containers.hpp"
#include "../core/memory.hpp"

#include "content_importer.hpp"
#include "cubemap_content.hpp"

namespace Garage
{
    class ContentSubsystem;
    class IGPUDevice;

    class GARAGE_API CubemapContentImporter final : public ContentImporter<CubemapContent>
    {
    public:
        explicit CubemapContentImporter(ContentSubsystem* owner, IGPUDevice& device) noexcept;

        [[nodiscard]] Result<UniquePtr<CubemapContent>, String>
                Import(const std::filesystem::path& path) const override;

    private:
        IGPUDevice& device;
    };

    inline CubemapContentImporter::CubemapContentImporter(ContentSubsystem* owner, IGPUDevice& device) noexcept
        : ContentImporter<CubemapContent>(owner)
        , device(device)
    {
    }
} // namespace Garage

#endif // GARAGE_CONTENT_CUBEMAP_CONTENT_IMPORTER_HPP
