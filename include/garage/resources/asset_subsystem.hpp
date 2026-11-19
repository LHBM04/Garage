#ifndef GARAGE_RESOURCES_ASSET_SUBSYSTEM_HPP
#define GARAGE_RESOURCES_ASSET_SUBSYSTEM_HPP

#include <concepts>
#include <filesystem>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../core/allocator.hpp"
#include "../core/app_subsystem.hpp"
#include "../core/defines.hpp"
#include "../core/system_status.hpp"
#include "../core/types.hpp"

#include "../utilities/logger.hpp"

#include "asset.hpp"
#include "asset_importer.hpp"

namespace Garage
{
    class SystemContext;
    class WorldBase;

    class GARAGE_API AssetSubsystem final : public AppSubsystemBase
    {
    public:
        AssetSubsystem() noexcept;
        ~AssetSubsystem() noexcept override = default;

        template <std::derived_from<AssetImporter<>> TAssetImporter, class... TArgs>
        void AddImporter(TArgs&&... args);

        template <std::derived_from<AssetBase> TAsset>
        [[nodiscard]] const TAsset* LoadAsset(const std::filesystem::path& path);

        void UnloadAsset(const std::filesystem::path& path);
        void UnloadAllAssets() noexcept;

        void OnWorldEntered(const WorldBase* world);
        void OnWorldExited(const WorldBase* world);

        [[nodiscard]] ESystemStatus OnStartup(SystemContext& context) override;
        [[nodiscard]] ESystemStatus OnEarlyUpdate(SystemContext& context) override;
        [[nodiscard]] ESystemStatus OnFixedUpdate(SystemContext& context) override;
        [[nodiscard]] ESystemStatus OnUpdate(SystemContext& context) override;
        [[nodiscard]] ESystemStatus OnLateUpdate(SystemContext& context) override;
        void                        OnShutdown(SystemContext& context) override;

    private:
        struct PathHash final
        {
            [[nodiscard]] USize operator()(const std::filesystem::path& path) const noexcept;
        };

        template <std::derived_from<AssetBase> TAsset>
        [[nodiscard]] const TAsset* GetLoadedAsset(const std::filesystem::path& path) const noexcept;

        using AssetImporterList = std::pmr::vector<Allocator::UniquePtr<AssetImporter<>>>;
        using AssetImporterDict = std::pmr::unordered_map<std::type_index, AssetImporter<>*>;

        using AssetList = std::pmr::vector<Allocator::UniquePtr<AssetBase>>;
        using AssetDict = std::pmr::unordered_map<std::filesystem::path, const AssetBase*, PathHash>;

        AssetImporterList assetImporters{Allocator::GetMemoryResource()};
        AssetImporterDict assetImportersByType{Allocator::GetMemoryResource()};

        AssetList assets{Allocator::GetMemoryResource()};
        AssetDict assetsByPath{Allocator::GetMemoryResource()};

        Int32 worldExitListenerId = 0;
    };

    inline AssetSubsystem::AssetSubsystem() noexcept
        : AppSubsystemBase(10)
    {
    }

    inline USize AssetSubsystem::PathHash::operator()(const std::filesystem::path& path) const noexcept
    {
        return std::filesystem::hash_value(path);
    }

    template <std::derived_from<AssetImporter<>> TAssetImporter, class... TArgs>
    void AssetSubsystem::AddImporter(TArgs&&... args)
    {
        const auto GET_ASSET_TYPE = []<class TImporter, std::derived_from<AssetBase> TAsset>(
                                            Result<Allocator::UniquePtr<TAsset>, std::pmr::string> (TImporter::*)(
                                                    const std::filesystem::path&) const) noexcept
        {
            return std::type_index(typeid(TAsset));
        };

        const std::type_index ASSET_TYPE = GET_ASSET_TYPE(&TAssetImporter::Import);
        if (assetImportersByType.contains(ASSET_TYPE))
        {
            return;
        }

        Allocator::UniquePtr<TAssetImporter> importer =
                Allocator::MakeUnique<TAssetImporter>(this, std::forward<TArgs>(args)...);
        AssetImporter<>* pointer = importer.get();
        assetImporters.emplace_back(std::move(importer));
        assetImportersByType.emplace(ASSET_TYPE, pointer);
    }

    template <std::derived_from<AssetBase> TAsset>
    const TAsset* AssetSubsystem::GetLoadedAsset(const std::filesystem::path& path) const noexcept
    {
        const auto CACHED = assetsByPath.find(path.lexically_normal());
        if (CACHED == assetsByPath.end())
        {
            return nullptr;
        }

        return dynamic_cast<const TAsset*>(CACHED->second);
    }

    template <std::derived_from<AssetBase> TAsset>
    const TAsset* AssetSubsystem::LoadAsset(const std::filesystem::path& path)
    {
        const std::filesystem::path NORMALIZED_PATH = path.lexically_normal();
        if (const auto CACHED = assetsByPath.find(NORMALIZED_PATH); CACHED != assetsByPath.end())
        {
            return dynamic_cast<const TAsset*>(CACHED->second);
        }

        const auto ASSET_IMPORTER = assetImportersByType.find(std::type_index(typeid(TAsset)));
        if (ASSET_IMPORTER == assetImportersByType.end())
        {
            return nullptr;
        }

        const auto* importer = static_cast<const AssetImporter<TAsset>*>(ASSET_IMPORTER->second);
        auto        asset    = importer->Import(NORMALIZED_PATH);
        if (!asset)
        {
            LOG_ERROR("에셋 로드에 실패했습니다: {}: {}", NORMALIZED_PATH.generic_string(), asset.GetError());
            return nullptr;
        }

        const TAsset* result = asset.GetValue().get();
        if (result == nullptr)
        {
            return nullptr;
        }

        assets.emplace_back(std::move(*asset));
        assetsByPath.emplace(NORMALIZED_PATH, result);
        LOG_TRACE("에셋을 로드했습니다: {}", NORMALIZED_PATH.generic_string());
        return result;
    }
} // namespace Garage

#endif // GARAGE_RESOURCES_ASSET_SUBSYSTEM_HPP
