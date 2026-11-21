#ifndef GARAGE_CONTENT_TERRAIN_CONTENT_IMPORTER_HPP
#define GARAGE_CONTENT_TERRAIN_CONTENT_IMPORTER_HPP

#include <filesystem>

#include "../core/common.hpp"
#include "../core/common/containers.hpp"
#include "../core/memory.hpp"

#include "content_importer.hpp"
#include "terrain_content.hpp"

namespace Garage
{
    class ContentSubsystem;
    class IGPUDevice;

    class GARAGE_API TerrainContentImporter final : public ContentImporter<TerrainContent>
    {
    public:
        explicit TerrainContentImporter(ContentSubsystem* owner, IGPUDevice& device) noexcept;

        [[nodiscard]] Result<UniquePtr<TerrainContent>, String>
                Import(const std::filesystem::path& path) const override;

    private:
        IGPUDevice& device;
    };

    inline TerrainContentImporter::TerrainContentImporter(ContentSubsystem* owner, IGPUDevice& device) noexcept
        : ContentImporter<TerrainContent>(owner)
        , device(device)
    {
    }
} // namespace Garage

#endif // GARAGE_CONTENT_TERRAIN_CONTENT_IMPORTER_HPP
