#ifndef GARAGE_RESOURCES_ASSET_IMPORTER_HPP
#define GARAGE_RESOURCES_ASSET_IMPORTER_HPP

#include <concepts>
#include <filesystem>
#include <string>

#include "../core/common.hpp"
#include "../core/memory.hpp"

#include "asset.hpp"

namespace Garage
{
    class AssetSubsystem;

    template <std::derived_from<AssetBase> TAsset = AssetBase>
    class AssetImporter;

    template <>
    class GARAGE_API AssetImporter<AssetBase>
    {
    public:
        explicit AssetImporter(AssetSubsystem* owner) noexcept;
        virtual ~AssetImporter() noexcept = default;

        AssetImporter(const AssetImporter&)            = delete;
        AssetImporter& operator=(const AssetImporter&) = delete;

        AssetImporter(AssetImporter&&)            = delete;
        AssetImporter& operator=(AssetImporter&&) = delete;

        [[nodiscard]] AssetSubsystem* GetOwner() const noexcept;

    private:
        AssetSubsystem* owner = nullptr;
    };

    using AssetImporterBase = AssetImporter<AssetBase>;

    template <std::derived_from<AssetBase> TAsset>
    class AssetImporter : public AssetImporter<>
    {
    public:
        using AssetImporter<>::AssetImporter;

        [[nodiscard]] virtual Result<UniquePtr<TAsset>, std::pmr::string>
                Import(const std::filesystem::path& path) const = 0;
    };

    inline AssetImporter<AssetBase>::AssetImporter(AssetSubsystem* owner) noexcept
        : owner(owner)
    {
        ASSERT(owner != nullptr);
    }

    inline AssetSubsystem* AssetImporter<AssetBase>::GetOwner() const noexcept
    {
        return owner;
    }
} // namespace Garage

#endif // GARAGE_RESOURCES_ASSET_IMPORTER_HPP
