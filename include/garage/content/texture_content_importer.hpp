#ifndef GARAGE_CONTENT_TEXTURE_CONTENT_IMPORTER_HPP
#define GARAGE_CONTENT_TEXTURE_CONTENT_IMPORTER_HPP

#include <filesystem>

#include "../core/common.hpp"
#include "../core/common/containers.hpp"
#include "../core/memory.hpp"

#include "content_importer.hpp"
#include "texture_content.hpp"

namespace Garage
{
    class ContentSubsystem;
    class IGPUDevice;

    class GARAGE_API TextureContentImporter final : public ContentImporter<TextureContent>
    {
    public:
        explicit TextureContentImporter(ContentSubsystem* owner, IGPUDevice& device) noexcept;

        [[nodiscard]] Result<UniquePtr<TextureContent>, String>
                Import(const std::filesystem::path& path) const override;

    private:
        IGPUDevice& device;
    };

    inline TextureContentImporter::TextureContentImporter(ContentSubsystem* owner, IGPUDevice& device) noexcept
        : ContentImporter<TextureContent>(owner)
        , device(device)
    {
    }
} // namespace Garage

#endif // GARAGE_CONTENT_TEXTURE_CONTENT_IMPORTER_HPP
