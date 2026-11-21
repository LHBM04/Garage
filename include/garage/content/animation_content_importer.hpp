#ifndef GARAGE_CONTENT_ANIMATION_CONTENT_IMPORTER_HPP
#define GARAGE_CONTENT_ANIMATION_CONTENT_IMPORTER_HPP

#include <filesystem>

#include "../core/common.hpp"
#include "../core/common/containers.hpp"
#include "../core/memory.hpp"

#include "animation_content.hpp"
#include "content_importer.hpp"

namespace Garage
{
    class GARAGE_API AnimationContentImporter final : public ContentImporter<AnimationContent>
    {
    public:
        using ContentImporter<AnimationContent>::ContentImporter;

        [[nodiscard]] Result<UniquePtr<AnimationContent>, String>
                Import(const std::filesystem::path& path) const override;
    };
} // namespace Garage

#endif // GARAGE_CONTENT_ANIMATION_CONTENT_IMPORTER_HPP
