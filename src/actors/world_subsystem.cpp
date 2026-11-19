#include "garage/actors/world_subsystem.hpp"

#include <algorithm>
#include <utility>
#include <vector>

#include "garage/actors/actor.hpp"
#include "garage/actors/world.hpp"

#include "garage/core/allocator.hpp"
#include "garage/core/defines.hpp"
#include "garage/core/system_context.hpp"
#include "garage/core/system_status.hpp"
#include "garage/core/types.hpp"

#include "garage/graphics/gpu_command_buffer.hpp"
#include "garage/graphics/render_subsystem.hpp"

namespace Garage
{
    WorldSubsystem::~WorldSubsystem() noexcept
    {
        worlds.clear();
    }

    WorldBase* WorldSubsystem::AddWorld(Allocator::UniquePtr<WorldBase> world)
    {
        ASSERT(world != nullptr);
        ASSERT(currentWorld == nullptr);

        WorldBase* result = world.get();
        worlds.push_back(std::move(world));
        return result;
    }

    bool WorldSubsystem::RequestWorldChange(WorldBase* targetWorld) noexcept
    {
        if (targetWorld == nullptr || targetWorld == currentWorld)
        {
            return false;
        }

        const bool IS_REGISTERED = std::ranges::any_of(worlds,
                                                       [targetWorld](const Allocator::UniquePtr<WorldBase>& world)
                                                       {
                                                           return world.get() == targetWorld;
                                                       });
        if (!IS_REGISTERED)
        {
            return false;
        }

        nextWorld = targetWorld;
        return true;
    }

    void WorldSubsystem::ExitWorld(SystemContext& context)
    {
        nextWorld = nullptr;
        ExitCurrentWorld(context);
        nextWorld = nullptr;
    }

    void WorldSubsystem::PreRender(SystemContext& context, IGPUCommandBuffer& commandBuffer)
    {
        if (currentWorld != nullptr)
        {
            currentWorld->PreRender(context, commandBuffer);
        }
    }

    void WorldSubsystem::Render(SystemContext& context, IGPUCommandBuffer& commandBuffer)
    {
        if (currentWorld != nullptr)
        {
            currentWorld->Render(context, commandBuffer);
        }
    }

    void WorldSubsystem::PostRender(SystemContext& context, IGPUCommandBuffer& commandBuffer)
    {
        if (currentWorld != nullptr)
        {
            currentWorld->PostRender(context, commandBuffer);
        }
    }

    ESystemStatus WorldSubsystem::OnStartup(SystemContext& context)
    {
        if (worlds.empty())
        {
            return ESystemStatus::FAILURE;
        }

        currentWorld = worlds.front().get();

        if (RenderSubsystem* renderSubsystem = context.GetEngineSubsystem<RenderSubsystem>();
            renderSubsystem != nullptr)
        {
            renderListenerId = renderSubsystem->OnRender().AddListener(
                    [this](SystemContext& renderContext, IGPUCommandBuffer& commandBuffer)
                    {
                        PreRender(renderContext, commandBuffer);
                        Render(renderContext, commandBuffer);
                        PostRender(renderContext, commandBuffer);
                    });
        }

        return ESystemStatus::CONTINUE;
    }

    ESystemStatus WorldSubsystem::OnEarlyUpdate(SystemContext& context)
    {
        std::pmr::vector<Allocator::UniquePtr<ActorBase>> persistentActors{Allocator::GetMemoryResource()};
        if (nextWorld != nullptr)
        {
            if (nextWorld != currentWorld)
            {
                if (currentWorld != nullptr)
                {
                    persistentActors = currentWorld->ExtractPersistentActors();
                }
                ExitCurrentWorld(context);
            }
            currentWorld = std::exchange(nextWorld, nullptr);
        }

        if (currentWorld == nullptr)
        {
            return ESystemStatus::SUCCESS;
        }

        if (!currentWorld->IsEntered())
        {
            WorldBase* enteringWorld = currentWorld;
            enteringWorld->Enter(context);
            if (currentWorld == enteringWorld && !persistentActors.empty())
            {
                enteringWorld->AdoptActors(std::move(persistentActors));
            }
            if (currentWorld == enteringWorld)
            {
                onEnterWorld.Broadcast(enteringWorld);
                NotifyIfWorldExited(enteringWorld);
            }
        }
        else if (!persistentActors.empty())
        {
            currentWorld->AdoptActors(std::move(persistentActors));
        }

        if (currentWorld != nullptr)
        {
            WorldBase* updatingWorld = currentWorld;
            updatingWorld->EarlyUpdate(context);
            NotifyIfWorldExited(updatingWorld);
        }
        return ESystemStatus::CONTINUE;
    }

    ESystemStatus WorldSubsystem::OnFixedUpdate(SystemContext& context)
    {
        if (currentWorld != nullptr)
        {
            WorldBase* updatingWorld = currentWorld;
            updatingWorld->FixedUpdate(context);
            NotifyIfWorldExited(updatingWorld);
        }
        return ESystemStatus::CONTINUE;
    }

    ESystemStatus WorldSubsystem::OnUpdate(SystemContext& context)
    {
        if (currentWorld != nullptr)
        {
            WorldBase* updatingWorld = currentWorld;
            updatingWorld->Update(context);
            NotifyIfWorldExited(updatingWorld);
        }
        return ESystemStatus::CONTINUE;
    }

    ESystemStatus WorldSubsystem::OnLateUpdate(SystemContext& context)
    {
        if (currentWorld != nullptr)
        {
            WorldBase* updatingWorld = currentWorld;
            updatingWorld->LateUpdate(context);
            NotifyIfWorldExited(updatingWorld);
        }
        return (currentWorld != nullptr || nextWorld != nullptr) ? ESystemStatus::CONTINUE : ESystemStatus::SUCCESS;
    }

    void WorldSubsystem::OnShutdown(SystemContext& context)
    {
        if (renderListenerId != 0)
        {
            if (RenderSubsystem* renderSubsystem = context.GetEngineSubsystem<RenderSubsystem>();
                renderSubsystem != nullptr)
            {
                renderSubsystem->OnRender().RemoveListener(renderListenerId);
            }
            renderListenerId = 0;
        }

        ExitWorld(context);
        onEnterWorld.Clear();
        onExitWorld.Clear();
        worlds.clear();
    }

    void WorldSubsystem::ExitCurrentWorld(SystemContext& context)
    {
        WorldBase* exitingWorld = std::exchange(currentWorld, nullptr);
        if (exitingWorld != nullptr && exitingWorld->IsEntered())
        {
            exitingWorld->Exit(context);
            onExitWorld.Broadcast(exitingWorld);
        }
    }

    void WorldSubsystem::NotifyIfWorldExited(WorldBase* world)
    {
        if (currentWorld == world && !world->IsEntered())
        {
            currentWorld = nullptr;
            onExitWorld.Broadcast(world);
        }
    }
} // namespace Garage
