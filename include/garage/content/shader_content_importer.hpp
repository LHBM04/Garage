#ifndef GARAGE_CONTENT_SHADER_CONTENT_IMPORTER_HPP
#define GARAGE_CONTENT_SHADER_CONTENT_IMPORTER_HPP

#include <filesystem>

#include "../core/common.hpp"
#include "../core/common/containers.hpp"
#include "../core/memory.hpp"

#include "content_importer.hpp"
#include "shader_content.hpp"

namespace Garage
{
    class ContentSubsystem;
    class IGPUDevice;

    class GARAGE_API ShaderContentImporter final : public ContentImporter<ShaderContent>
    {
    public:
        explicit ShaderContentImporter(ContentSubsystem* owner, IGPUDevice& device) noexcept;

        [[nodiscard]] Result<UniquePtr<ShaderContent>, String> Import(const std::filesystem::path& path) const override;

    private:
        IGPUDevice& device;
    };

    inline ShaderContentImporter::ShaderContentImporter(ContentSubsystem* owner, IGPUDevice& device) noexcept
        : ContentImporter<ShaderContent>(owner)
        , device(device)
    {
    }
} // namespace Garage

#endif // GARAGE_CONTENT_SHADER_CONTENT_IMPORTER_HPP
