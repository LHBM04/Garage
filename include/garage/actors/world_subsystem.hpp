#ifndef GARAGE_ACTORS_WORLD_SUBSYSTEM_HPP
#define GARAGE_ACTORS_WORLD_SUBSYSTEM_HPP

#include <vector>

#include "../core/app_subsystem.hpp"
#include "../core/common.hpp"
#include "../core/memory.hpp"
#include "../core/system_status.hpp"

namespace Garage
{
    class SystemContext;
    class WorldBase;

    class GARAGE_API WorldSubsystem final : public AppSubsystemBase
    {
    public:
        WorldSubsystem() noexcept;
        ~WorldSubsystem() noexcept override;

        WorldBase* AddWorld(UniquePtr<WorldBase> world);
        void       ExitWorld(SystemContext& context);

        [[nodiscard]] bool RequestWorldChange(WorldBase* targetWorld) noexcept;

        [[nodiscard]] Delegate<WorldBase*>& OnEnterWorld() noexcept;
        [[nodiscard]] Delegate<WorldBase*>& OnExitWorld() noexcept;

        [[nodiscard]] WorldBase*       GetCurrentWorld() noexcept;
        [[nodiscard]] const WorldBase* GetCurrentWorld() const noexcept;

        void PreRender(SystemContext& context);
        void Render(SystemContext& context);
        void PostRender(SystemContext& context);

        [[nodiscard]] ESystemStatus OnStartup(SystemContext& context) override;
        [[nodiscard]] ESystemStatus OnEarlyUpdate(SystemContext& context) override;
        [[nodiscard]] ESystemStatus OnFixedUpdate(SystemContext& context) override;
        [[nodiscard]] ESystemStatus OnUpdate(SystemContext& context) override;
        [[nodiscard]] ESystemStatus OnLateUpdate(SystemContext& context) override;
        void                        OnShutdown(SystemContext& context) override;

    private:
        void ExitCurrentWorld(SystemContext& context);
        void NotifyIfWorldExited(WorldBase* world);

        std::pmr::vector<UniquePtr<WorldBase>> worlds{Memory::GetMemoryResource()};
        WorldBase*                             currentWorld = nullptr;
        WorldBase*                             nextWorld    = nullptr;

        Delegate<WorldBase*> onEnterWorld     = {};
        Delegate<WorldBase*> onExitWorld      = {};
        Int32                renderListenerId = 0;
    };

    inline WorldSubsystem::WorldSubsystem() noexcept
        : AppSubsystemBase(20)
    {
    }

    inline Delegate<WorldBase*>& WorldSubsystem::OnEnterWorld() noexcept
    {
        return onEnterWorld;
    }

    inline Delegate<WorldBase*>& WorldSubsystem::OnExitWorld() noexcept
    {
        return onExitWorld;
    }

    inline WorldBase* WorldSubsystem::GetCurrentWorld() noexcept
    {
        return currentWorld;
    }

    inline const WorldBase* WorldSubsystem::GetCurrentWorld() const noexcept
    {
        return currentWorld;
    }
} // namespace Garage

#endif // GARAGE_ACTORS_WORLD_SUBSYSTEM_HPP
