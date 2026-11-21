#ifndef GARAGE_CONTENT_CONTENT_HPP
#define GARAGE_CONTENT_CONTENT_HPP

#include <filesystem>
#include <utility>

#include "../core/common.hpp"
#include "../core/common/containers.hpp"
#include "../core/objectable.hpp"

namespace Garage
{
    class GARAGE_API ContentBase : public IObjectable
    {
    public:
        explicit ContentBase(std::filesystem::path path) noexcept;
        ~ContentBase() noexcept override = default;

        ContentBase(const ContentBase&)            = delete;
        ContentBase& operator=(const ContentBase&) = delete;

        ContentBase(ContentBase&&)            = delete;
        ContentBase& operator=(ContentBase&&) = delete;

        [[nodiscard]] StringView                   GetName() const noexcept;
        [[nodiscard]] const std::filesystem::path& GetPath() const noexcept;

    private:
        String                name;
        std::filesystem::path path = {};
    };

    inline ContentBase::ContentBase(std::filesystem::path path) noexcept
        : name(path.stem().string())
        , path(std::move(path))
    {
    }

    inline StringView ContentBase::GetName() const noexcept
    {
        return name;
    }

    inline const std::filesystem::path& ContentBase::GetPath() const noexcept
    {
        return path;
    }
} // namespace Garage

#endif // GARAGE_CONTENT_CONTENT_HPP
