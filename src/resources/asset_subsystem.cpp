#include "garage/resources/asset_subsystem.hpp"

#include <filesystem>
#include <vector>

#include "garage/actors/world.hpp"
#include "garage/actors/world_subsystem.hpp"

#include "garage/core/allocator.hpp"
#include "garage/core/system_context.hpp"
#include "garage/core/system_status.hpp"

#include "garage/resources/asset.hpp"

#include "garage/utilities/logger.hpp"

namespace Garage
{
    void AssetSubsystem::UnloadAsset(const std::filesystem::path& path)
    {
        const std::filesystem::path NORMALIZED_PATH = path.lexically_normal();
        const auto                  CACHED          = assetsByPath.find(NORMALIZED_PATH);
        if (CACHED == assetsByPath.end())
        {
            return;
        }

        const AssetBase* target = CACHED->second;
        assetsByPath.erase(CACHED);
        std::erase_if(assets,
                      [target](const Allocator::UniquePtr<AssetBase>& asset) noexcept
                      {
                          return asset.get() == target;
                      });
        LOG_TRACE("에셋을 해제했습니다: {}", NORMALIZED_PATH.generic_string());
    }

    void AssetSubsystem::UnloadAllAssets() noexcept
    {
        for (const auto& [path, asset] : assetsByPath)
        {
            LOG_TRACE("에셋을 해제했습니다: {}", path.generic_string());
        }

        assetsByPath.clear();
        assets.clear();
    }

    void AssetSubsystem::OnWorldEntered([[maybe_unused]] const WorldBase* world)
    {
    }

    void AssetSubsystem::OnWorldExited([[maybe_unused]] const WorldBase* world)
    {
        UnloadAllAssets();
    }

    ESystemStatus AssetSubsystem::OnStartup(SystemContext& context)
    {
        if (WorldSubsystem* worldSubsystem = context.GetAppSubsystem<WorldSubsystem>(); worldSubsystem != nullptr)
        {
            worldExitListenerId = worldSubsystem->OnExitWorld().AddListener(
                    [this](WorldBase* world)
                    {
                        OnWorldExited(world);
                    });
        }

        return ESystemStatus::CONTINUE;
    }

    ESystemStatus AssetSubsystem::OnEarlyUpdate([[maybe_unused]] SystemContext& context)
    {
        return ESystemStatus::CONTINUE;
    }

    ESystemStatus AssetSubsystem::OnFixedUpdate([[maybe_unused]] SystemContext& context)
    {
        return ESystemStatus::CONTINUE;
    }

    ESystemStatus AssetSubsystem::OnUpdate([[maybe_unused]] SystemContext& context)
    {
        return ESystemStatus::CONTINUE;
    }

    ESystemStatus AssetSubsystem::OnLateUpdate([[maybe_unused]] SystemContext& context)
    {
        return ESystemStatus::CONTINUE;
    }

    void AssetSubsystem::OnShutdown(SystemContext& context)
    {
        if (worldExitListenerId != 0)
        {
            if (WorldSubsystem* worldSubsystem = context.GetAppSubsystem<WorldSubsystem>(); worldSubsystem != nullptr)
            {
                worldSubsystem->OnExitWorld().RemoveListener(worldExitListenerId);
            }
            worldExitListenerId = 0;
        }

        UnloadAllAssets();
        assetImportersByType.clear();
        assetImporters.clear();
    }
} // namespace Garage
