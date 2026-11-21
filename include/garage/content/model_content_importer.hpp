#ifndef GARAGE_CONTENT_MODEL_CONTENT_IMPORTER_HPP
#define GARAGE_CONTENT_MODEL_CONTENT_IMPORTER_HPP

#include <filesystem>

#include "../core/common.hpp"
#include "../core/common/containers.hpp"
#include "../core/memory.hpp"

#include "content_importer.hpp"
#include "model_content.hpp"

namespace Garage
{
    class GARAGE_API ModelContentImporter final : public ContentImporter<ModelContent>
    {
    public:
        using ContentImporter<ModelContent>::ContentImporter;

        [[nodiscard]] Result<UniquePtr<ModelContent>, String> Import(const std::filesystem::path& path) const override;
    };
} // namespace Garage

#endif // GARAGE_CONTENT_MODEL_CONTENT_IMPORTER_HPP
