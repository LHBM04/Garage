#include "garage/actors/world_subsystem.hpp"

#include <algorithm>
#include <utility>
#include <vector>

#include "garage/actors/actor.hpp"
#include "garage/actors/world.hpp"

#include "garage/core/allocator.hpp"
#include "garage/core/defines.hpp"
#include "garage/core/engine.hpp"
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

    void WorldSubsystem::ExitWorld()
    {
        nextWorld = nullptr;
        ExitCurrentWorld();
        nextWorld = nullptr;
    }

    void WorldSubsystem::PreRender(IGPUCommandBuffer& commandBuffer)
    {
        if (currentWorld != nullptr)
        {
            currentWorld->PreRender(commandBuffer);
        }
    }

    void WorldSubsystem::Render(IGPUCommandBuffer& commandBuffer)
    {
        if (currentWorld != nullptr)
        {
            currentWorld->Render(commandBuffer);
        }
    }

    void WorldSubsystem::PostRender(IGPUCommandBuffer& commandBuffer)
    {
        if (currentWorld != nullptr)
        {
            currentWorld->PostRender(commandBuffer);
        }
    }

    ESystemStatus WorldSubsystem::OnStartup()
    {
        if (worlds.empty())
        {
            return ESystemStatus::FAILURE;
        }

        currentWorld = worlds.front().get();

        if (Engine* engine = Engine::GetInstance(); engine != nullptr)
        {
            if (RenderSubsystem* renderSubsystem = engine->GetSubsystem<RenderSubsystem>(); renderSubsystem != nullptr)
            {
                renderListenerId = renderSubsystem->OnRender().AddListener(
                        [this](IGPUCommandBuffer& commandBuffer)
                        {
                            PreRender(commandBuffer);
                            Render(commandBuffer);
                            PostRender(commandBuffer);
                        });
            }
        }

        return ESystemStatus::CONTINUE;
    }

    ESystemStatus WorldSubsystem::OnEarlyUpdate()
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
                ExitCurrentWorld();
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
            enteringWorld->Enter();
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
            updatingWorld->EarlyUpdate();
            NotifyIfWorldExited(updatingWorld);
        }
        return ESystemStatus::CONTINUE;
    }

    ESystemStatus WorldSubsystem::OnFixedUpdate()
    {
        if (currentWorld != nullptr)
        {
            WorldBase* updatingWorld = currentWorld;
            updatingWorld->FixedUpdate();
            NotifyIfWorldExited(updatingWorld);
        }
        return ESystemStatus::CONTINUE;
    }

    ESystemStatus WorldSubsystem::OnUpdate()
    {
        if (currentWorld != nullptr)
        {
            WorldBase* updatingWorld = currentWorld;
            updatingWorld->Update();
            NotifyIfWorldExited(updatingWorld);
        }
        return ESystemStatus::CONTINUE;
    }

    ESystemStatus WorldSubsystem::OnLateUpdate()
    {
        if (currentWorld != nullptr)
        {
            WorldBase* updatingWorld = currentWorld;
            updatingWorld->LateUpdate();
            NotifyIfWorldExited(updatingWorld);
        }
        return (currentWorld != nullptr || nextWorld != nullptr) ? ESystemStatus::CONTINUE : ESystemStatus::SUCCESS;
    }

    void WorldSubsystem::OnShutdown()
    {
        if (renderListenerId != 0)
        {
            if (Engine* engine = Engine::GetInstance(); engine != nullptr)
            {
                if (RenderSubsystem* renderSubsystem = engine->GetSubsystem<RenderSubsystem>();
                    renderSubsystem != nullptr)
                {
                    renderSubsystem->OnRender().RemoveListener(renderListenerId);
                }
            }
            renderListenerId = 0;
        }

        ExitWorld();
        onEnterWorld.Clear();
        onExitWorld.Clear();
        worlds.clear();
    }

    void WorldSubsystem::ExitCurrentWorld()
    {
        WorldBase* exitingWorld = std::exchange(currentWorld, nullptr);
        if (exitingWorld != nullptr && exitingWorld->IsEntered())
        {
            exitingWorld->Exit();
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
