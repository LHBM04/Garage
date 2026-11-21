#ifndef GARAGE_CONTENT_MESH_CONTENT_IMPORTER_HPP
#define GARAGE_CONTENT_MESH_CONTENT_IMPORTER_HPP

#include <filesystem>

#include "../core/common.hpp"
#include "../core/common/containers.hpp"
#include "../core/memory.hpp"

#include "content_importer.hpp"
#include "mesh_content.hpp"

namespace Garage
{
    class ContentSubsystem;
    class IGPUDevice;

    class GARAGE_API MeshContentImporter final : public ContentImporter<MeshContent>
    {
    public:
        explicit MeshContentImporter(ContentSubsystem* owner, IGPUDevice& device) noexcept;

        [[nodiscard]] Result<UniquePtr<MeshContent>, String> Import(const std::filesystem::path& path) const override;

    private:
        IGPUDevice& device;
    };

    inline MeshContentImporter::MeshContentImporter(ContentSubsystem* owner, IGPUDevice& device) noexcept
        : ContentImporter<MeshContent>(owner)
        , device(device)
    {
    }
} // namespace Garage

#endif // GARAGE_CONTENT_MESH_CONTENT_IMPORTER_HPP
