#ifndef GARAGE_RESOURCES_ASSET_HPP
#define GARAGE_RESOURCES_ASSET_HPP

#include <filesystem>
#include <string>
#include <string_view>
#include <utility>

#include "../core/allocator.hpp"
#include "../core/defines.hpp"

namespace Garage
{
    class GARAGE_API AssetBase
    {
    public:
        explicit AssetBase(std::filesystem::path path) noexcept;
        virtual ~AssetBase() noexcept = default;

        AssetBase(const AssetBase&)            = delete;
        AssetBase& operator=(const AssetBase&) = delete;

        AssetBase(AssetBase&&)            = delete;
        AssetBase& operator=(AssetBase&&) = delete;

        [[nodiscard]] std::string_view             GetName() const noexcept;
        [[nodiscard]] const std::filesystem::path& GetPath() const noexcept;

    private:
        std::pmr::string      name{Allocator::GetMemoryResource()};
        std::filesystem::path path = {};
    };

    inline AssetBase::AssetBase(std::filesystem::path path) noexcept
        : name(path.stem().string(), Allocator::GetMemoryResource())
        , path(std::move(path))
    {
    }

    inline std::string_view AssetBase::GetName() const noexcept
    {
        return name;
    }

    inline const std::filesystem::path& AssetBase::GetPath() const noexcept
    {
        return path;
    }
} // namespace Garage

#endif // GARAGE_RESOURCES_ASSET_HPP
