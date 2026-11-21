#include "garage/content/content_subsystem.hpp"

#include <filesystem>
#include <vector>

#include "garage/actors/world.hpp"
#include "garage/actors/world_subsystem.hpp"

#include "garage/content/content.hpp"

#include "garage/core/memory.hpp"
#include "garage/core/system_context.hpp"
#include "garage/core/system_status.hpp"

#include "garage/utilities/logger.hpp"

namespace Garage
{
    void ContentSubsystem::UnloadContent(const std::filesystem::path& path)
    {
        const std::filesystem::path NORMALIZED_PATH = path.lexically_normal();
        const auto                  CACHED          = contentByPath.find(NORMALIZED_PATH);
        if (CACHED == contentByPath.end())
        {
            return;
        }

        const ContentBase* target = CACHED->second;
        contentByPath.erase(CACHED);
        std::erase_if(loadedContent,
                      [target](const UniquePtr<ContentBase>& content) noexcept
                      {
                          return content.get() == target;
                      });
        LOG_TRACE("콘텐츠를 해제했습니다: {}", NORMALIZED_PATH.generic_string());
    }

    void ContentSubsystem::UnloadAllContent() noexcept
    {
        for (const auto& [path, content] : contentByPath)
        {
            LOG_TRACE("콘텐츠를 해제했습니다: {}", path.generic_string());
        }

        contentByPath.clear();
        loadedContent.clear();
    }

    void ContentSubsystem::OnWorldEntered([[maybe_unused]] const WorldBase* world)
    {
    }

    void ContentSubsystem::OnWorldExited([[maybe_unused]] const WorldBase* world)
    {
        UnloadAllContent();
    }

    ESystemStatus ContentSubsystem::OnStartup(SystemContext& context)
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

    ESystemStatus ContentSubsystem::OnEarlyUpdate([[maybe_unused]] SystemContext& context)
    {
        return ESystemStatus::CONTINUE;
    }

    ESystemStatus ContentSubsystem::OnFixedUpdate([[maybe_unused]] SystemContext& context)
    {
        return ESystemStatus::CONTINUE;
    }

    ESystemStatus ContentSubsystem::OnUpdate([[maybe_unused]] SystemContext& context)
    {
        return ESystemStatus::CONTINUE;
    }

    ESystemStatus ContentSubsystem::OnLateUpdate([[maybe_unused]] SystemContext& context)
    {
        return ESystemStatus::CONTINUE;
    }

    void ContentSubsystem::OnShutdown(SystemContext& context)
    {
        if (worldExitListenerId != 0)
        {
            if (WorldSubsystem* worldSubsystem = context.GetAppSubsystem<WorldSubsystem>(); worldSubsystem != nullptr)
            {
                worldSubsystem->OnExitWorld().RemoveListener(worldExitListenerId);
            }
            worldExitListenerId = 0;
        }

        UnloadAllContent();
        contentImportersByType.clear();
        contentImporters.clear();
    }
} // namespace Garage
