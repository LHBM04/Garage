#ifndef GARAGE_CONTENT_CONTENT_IMPORTER_HPP
#define GARAGE_CONTENT_CONTENT_IMPORTER_HPP

#include <concepts>
#include <filesystem>

#include "../core/common.hpp"
#include "../core/common/containers.hpp"
#include "../core/memory.hpp"

#include "content.hpp"

namespace Garage
{
    class ContentSubsystem;

    template <std::derived_from<ContentBase> TContent = ContentBase>
    class ContentImporter;

    template <>
    class GARAGE_API ContentImporter<ContentBase>
    {
    public:
        explicit ContentImporter(ContentSubsystem* owner) noexcept;
        virtual ~ContentImporter() noexcept = default;

        ContentImporter(const ContentImporter&)            = delete;
        ContentImporter& operator=(const ContentImporter&) = delete;

        ContentImporter(ContentImporter&&)            = delete;
        ContentImporter& operator=(ContentImporter&&) = delete;

        [[nodiscard]] ContentSubsystem* GetOwner() const noexcept;

    private:
        ContentSubsystem* owner = nullptr;
    };

    using ContentImporterBase = ContentImporter<ContentBase>;

    template <std::derived_from<ContentBase> TContent>
    class ContentImporter : public ContentImporter<>
    {
    public:
        using ContentImporter<>::ContentImporter;

        [[nodiscard]] virtual Result<UniquePtr<TContent>, String> Import(const std::filesystem::path& path) const = 0;
    };

    inline ContentImporter<ContentBase>::ContentImporter(ContentSubsystem* owner) noexcept
        : owner(owner)
    {
        ASSERT(owner != nullptr);
    }

    inline ContentSubsystem* ContentImporter<ContentBase>::GetOwner() const noexcept
    {
        return owner;
    }
} // namespace Garage

#endif // GARAGE_CONTENT_CONTENT_IMPORTER_HPP
