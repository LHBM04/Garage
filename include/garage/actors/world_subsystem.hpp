#ifndef GARAGE_ACTORS_WORLD_SUBSYSTEM_HPP
#define GARAGE_ACTORS_WORLD_SUBSYSTEM_HPP

#include <vector>

#include "../core/allocator.hpp"
#include "../core/app_subsystem.hpp"
#include "../core/defines.hpp"
#include "../core/system_status.hpp"
#include "../core/types.hpp"

namespace Garage
{
    class IGPUCommandBuffer;
    class WorldBase;

    class GARAGE_API WorldSubsystem final : public AppSubsystemBase
    {
    public:
        WorldSubsystem() noexcept;
        ~WorldSubsystem() noexcept override;

        WorldBase* AddWorld(Allocator::UniquePtr<WorldBase> world);
        void       ExitWorld();

        [[nodiscard]] bool RequestWorldChange(WorldBase* targetWorld) noexcept;

        [[nodiscard]] Delegate<WorldBase*>& OnEnterWorld() noexcept;
        [[nodiscard]] Delegate<WorldBase*>& OnExitWorld() noexcept;

        [[nodiscard]] WorldBase*       GetCurrentWorld() noexcept;
        [[nodiscard]] const WorldBase* GetCurrentWorld() const noexcept;

        void PreRender(IGPUCommandBuffer& commandBuffer);
        void Render(IGPUCommandBuffer& commandBuffer);
        void PostRender(IGPUCommandBuffer& commandBuffer);

        [[nodiscard]] ESystemStatus OnStartup() override;
        [[nodiscard]] ESystemStatus OnEarlyUpdate() override;
        [[nodiscard]] ESystemStatus OnFixedUpdate() override;
        [[nodiscard]] ESystemStatus OnUpdate() override;
        [[nodiscard]] ESystemStatus OnLateUpdate() override;
        void                        OnShutdown() override;

    private:
        void ExitCurrentWorld();
        void NotifyIfWorldExited(WorldBase* world);

        std::pmr::vector<Allocator::UniquePtr<WorldBase>> worlds{Allocator::GetMemoryResource()};
        WorldBase*                                        currentWorld = nullptr;
        WorldBase*                                        nextWorld    = nullptr;

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
