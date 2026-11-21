#ifndef GARAGE_CONTENT_BITMAP_FONT_CONTENT_IMPORTER_HPP
#define GARAGE_CONTENT_BITMAP_FONT_CONTENT_IMPORTER_HPP

#include <filesystem>

#include "../core/common.hpp"
#include "../core/common/containers.hpp"
#include "../core/memory.hpp"

#include "bitmap_font_content.hpp"
#include "content_importer.hpp"

namespace Garage
{
    class GARAGE_API BitmapFontContentImporter final : public ContentImporter<BitmapFontContent>
    {
    public:
        using ContentImporter<BitmapFontContent>::ContentImporter;

        [[nodiscard]] Result<UniquePtr<BitmapFontContent>, String>
                Import(const std::filesystem::path& path) const override;
    };
} // namespace Garage

#endif // GARAGE_CONTENT_BITMAP_FONT_CONTENT_IMPORTER_HPP
