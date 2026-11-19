#ifndef GARAGE_RESOURCES_ASSET_HPP
#define GARAGE_RESOURCES_ASSET_HPP

#include <filesystem>
#include <utility>

#include "../core/common.hpp"
#include "../core/common/containers.hpp"

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

        [[nodiscard]] StringView                   GetName() const noexcept;
        [[nodiscard]] const std::filesystem::path& GetPath() const noexcept;

    private:
        String                name;
        std::filesystem::path path = {};
    };

    inline AssetBase::AssetBase(std::filesystem::path path) noexcept
        : name(path.stem().string())
        , path(std::move(path))
    {
    }

    inline StringView AssetBase::GetName() const noexcept
    {
        return name;
    }

    inline const std::filesystem::path& AssetBase::GetPath() const noexcept
    {
        return path;
    }
} // namespace Garage

#endif // GARAGE_RESOURCES_ASSET_HPP
