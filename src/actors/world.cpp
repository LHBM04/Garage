#include "garage/actors/world.hpp"

#include <algorithm>
#include <ranges>
#include <string_view>
#include <utility>
#include <vector>

#include "garage/actors/actor.hpp"
#include "garage/actors/transform.hpp"

#include "garage/core/allocator.hpp"
#include "garage/core/defines.hpp"
#include "garage/core/types.hpp"

#include "garage/graphics/gpu_command_buffer.hpp"

#include "garage/math/quaternion.hpp"
#include "garage/math/vector3d.hpp"

namespace Garage
{
    template <class TCallback>
    void WorldBase::RunPhase(TCallback callback)
    {
        if (isRunningPhase)
        {
            return;
        }

        isRunningPhase = true;
        callback();

        if (!isEntered)
        {
            ClearActors();
        }
        isRunningPhase = false;
    }

    template <class TCallback>
    void WorldBase::ForEachActor(TCallback callback)
    {
        const USize COUNT = actors.size();
        for (USize index = 0; index < COUNT && isEntered; ++index)
        {
            ActorBase& actor = *actors[index];
            if (!actor.IsDestroyed())
            {
                callback(actor);
            }
        }
    }

    void WorldBase::Enter()
    {
        if (isEntered)
        {
            return;
        }

        RunPhase(
                [this]()
                {
                    isEntered = true;
                    OnEnter();
                });
    }

    void WorldBase::EarlyUpdate()
    {
        if (isEntered)
        {
            RunPhase(
                    [this]()
                    {
                        DestroyActors();
                    });
        }
    }

    void WorldBase::FixedUpdate()
    {
        UpdateActors(&WorldBase::OnFixedUpdate, &ActorBase::FixedUpdate);
    }

    void WorldBase::Update()
    {
        UpdateActors(&WorldBase::OnUpdate, &ActorBase::Update);
    }

    void WorldBase::LateUpdate()
    {
        UpdateActors(&WorldBase::OnLateUpdate, &ActorBase::LateUpdate);
    }

    void WorldBase::PreRender(IGPUCommandBuffer& commandBuffer)
    {
        if (isEntered)
        {
            RunPhase(
                    [this, &commandBuffer]()
                    {
                        RenderActors(&ActorBase::PreRender, commandBuffer);
                    });
        }
    }

    void WorldBase::Render(IGPUCommandBuffer& commandBuffer)
    {
        if (!isEntered)
        {
            return;
        }

        RunPhase(
                [this, &commandBuffer]()
                {
                    RenderActors(&ActorBase::Render, commandBuffer);
#if defined(GARAGE_DEBUG) || defined(_DEBUG)
                    RenderActors(&ActorBase::DrawGizmos, commandBuffer);
#endif
                });
    }

    void WorldBase::PostRender(IGPUCommandBuffer& commandBuffer)
    {
        if (isEntered)
        {
            RunPhase(
                    [this, &commandBuffer]()
                    {
                        RenderActors(&ActorBase::PostRender, commandBuffer);
                    });
        }
    }

    void WorldBase::Exit()
    {
        const auto EXIT = [this]()
        {
            if (isEntered)
            {
                isEntered = false;
                OnExit();
            }
        };

        if (isRunningPhase)
        {
            EXIT();
        }
        else
        {
            RunPhase(EXIT);
        }
    }

    void WorldBase::UpdateActors(WorldCallback worldCallback, ActorCallback actorCallback)
    {
        ASSERT(worldCallback != nullptr);
        ASSERT(actorCallback != nullptr);

        if (!isEntered)
        {
            return;
        }

        RunPhase(
                [this, worldCallback, actorCallback]()
                {
                    DestroyActors();
                    if (!isEntered)
                    {
                        return;
                    }

                    (this->*worldCallback)();
                    ForEachActor(
                            [actorCallback](ActorBase& actor)
                            {
                                (actor.*actorCallback)();
                            });
                });
    }

    void WorldBase::RenderActors(RenderCallback callback, IGPUCommandBuffer& commandBuffer)
    {
        ASSERT(callback != nullptr);

        ForEachActor(
                [callback, &commandBuffer](ActorBase& actor)
                {
                    (actor.*callback)(commandBuffer);
                });
    }

    ActorBase* WorldBase::Instantiate()
    {
        Allocator::UniquePtr<ActorBase> actor  = Allocator::MakeUnique<ActorBase>(this);
        ActorBase*                      result = actor.get();
        actors.push_back(std::move(actor));
        return result;
    }

    ActorBase* WorldBase::Instantiate(const Vector3D<Float32>& position, const Quaternion<Float32>& rotation)
    {
        return Instantiate(position, rotation, nullptr);
    }

    ActorBase* WorldBase::Instantiate(const Vector3D<Float32>& position)
    {
        ActorBase* actor = Instantiate(nullptr);
        actor->GetComponent<Transform>()->SetLocalPosition(position);
        return actor;
    }

    ActorBase* WorldBase::Instantiate(const Quaternion<Float32>& rotation)
    {
        ActorBase* actor = Instantiate(nullptr);
        actor->GetComponent<Transform>()->SetLocalRotation(rotation);
        return actor;
    }

    ActorBase* WorldBase::Instantiate(Transform* parent)
    {
        ActorBase* actor = Instantiate();
        actor->AddComponent<Transform>()->SetParent(parent);
        return actor;
    }

    ActorBase* WorldBase::Instantiate(const Vector3D<Float32>&   position,
                                      const Quaternion<Float32>& rotation,
                                      Transform*                 parent)
    {
        ActorBase* actor     = Instantiate(parent);
        Transform* transform = actor->GetComponent<Transform>();
        transform->SetLocalPosition(position);
        transform->SetLocalRotation(rotation);
        return actor;
    }

    void WorldBase::Destroy(ActorBase* actor) noexcept
    {
        ASSERT(actor != nullptr);

        if (actor != nullptr)
        {
            actor->Destroy();
        }
    }

    void WorldBase::DestroyActors()
    {
        if (!isEntered)
        {
            return;
        }

        const auto DESTROYED_COUNT = std::ranges::count_if(actors,
                                                           [](const Allocator::UniquePtr<ActorBase>& actor)
                                                           {
                                                               return actor->IsDestroyed();
                                                           });
        if (DESTROYED_COUNT == 0)
        {
            return;
        }

        std::pmr::vector<Allocator::UniquePtr<ActorBase>> destroyed{Allocator::GetMemoryResource()};
        destroyed.reserve(static_cast<USize>(DESTROYED_COUNT));

        USize remainingCount = 0;
        for (USize index = 0; index < actors.size(); ++index)
        {
            if (actors[index]->IsDestroyed())
            {
                destroyed.push_back(std::move(actors[index]));
            }
            else
            {
                if (remainingCount != index)
                {
                    actors[remainingCount] = std::move(actors[index]);
                }
                ++remainingCount;
            }
        }
        actors.resize(remainingCount);
    }

    void WorldBase::ClearActors()
    {
        if (actors.empty())
        {
            return;
        }

        std::pmr::vector<Allocator::UniquePtr<ActorBase>> removing(std::move(actors), Allocator::GetMemoryResource());
        actors.clear();

        for (Allocator::UniquePtr<ActorBase>& actor : removing | std::views::reverse)
        {
            actor->Destroy();
            actor.reset();
        }
    }

    std::pmr::vector<Allocator::UniquePtr<ActorBase>> WorldBase::ExtractPersistentActors()
    {
        std::pmr::vector<Allocator::UniquePtr<ActorBase>> extracted{Allocator::GetMemoryResource()};
        for (auto actorIt = actors.begin(); actorIt != actors.end();)
        {
            ActorBase* actor = actorIt->get();
            if (actor->ShouldDontDestroyOnLoad() && !actor->IsDestroyed())
            {
                extracted.push_back(std::move(*actorIt));
                actorIt = actors.erase(actorIt);
            }
            else
            {
                ++actorIt;
            }
        }

        return extracted;
    }

    void WorldBase::AdoptActors(std::pmr::vector<Allocator::UniquePtr<ActorBase>>&& adopted)
    {
        for (Allocator::UniquePtr<ActorBase>& actor : adopted)
        {
            if (actor == nullptr)
            {
                continue;
            }

            actor->world = this;
            actors.push_back(std::move(actor));
        }
    }

    ActorBase* WorldBase::FindActorByName(std::string_view name) noexcept
    {
        for (const Allocator::UniquePtr<ActorBase>& actor : actors)
        {
            if (!actor->IsDestroyed() && actor->GetName() == name)
            {
                return actor.get();
            }
        }

        return nullptr;
    }

    const ActorBase* WorldBase::FindActorByName(std::string_view name) const noexcept
    {
        for (const Allocator::UniquePtr<ActorBase>& actor : actors)
        {
            if (!actor->IsDestroyed() && actor->GetName() == name)
            {
                return actor.get();
            }
        }

        return nullptr;
    }

    std::pmr::vector<ActorBase*> WorldBase::FindActorsByName(std::string_view name)
    {
        std::pmr::vector<ActorBase*> result{Allocator::GetMemoryResource()};
        for (const Allocator::UniquePtr<ActorBase>& actor : actors)
        {
            if (!actor->IsDestroyed() && actor->GetName() == name)
            {
                result.push_back(actor.get());
            }
        }

        return result;
    }

    std::pmr::vector<const ActorBase*> WorldBase::FindActorsByName(std::string_view name) const
    {
        std::pmr::vector<const ActorBase*> result{Allocator::GetMemoryResource()};
        for (const Allocator::UniquePtr<ActorBase>& actor : actors)
        {
            if (!actor->IsDestroyed() && actor->GetName() == name)
            {
                result.push_back(actor.get());
            }
        }

        return result;
    }

    ActorBase* WorldBase::FindActorByTag(std::string_view tag) noexcept
    {
        for (const Allocator::UniquePtr<ActorBase>& actor : actors)
        {
            if (!actor->IsDestroyed() && actor->CompareTag(tag))
            {
                return actor.get();
            }
        }

        return nullptr;
    }

    const ActorBase* WorldBase::FindActorByTag(std::string_view tag) const noexcept
    {
        for (const Allocator::UniquePtr<ActorBase>& actor : actors)
        {
            if (!actor->IsDestroyed() && actor->CompareTag(tag))
            {
                return actor.get();
            }
        }

        return nullptr;
    }

    std::pmr::vector<ActorBase*> WorldBase::FindActorsByTag(std::string_view tag)
    {
        std::pmr::vector<ActorBase*> result{Allocator::GetMemoryResource()};
        for (const Allocator::UniquePtr<ActorBase>& actor : actors)
        {
            if (!actor->IsDestroyed() && actor->CompareTag(tag))
            {
                result.push_back(actor.get());
            }
        }

        return result;
    }

    std::pmr::vector<const ActorBase*> WorldBase::FindActorsByTag(std::string_view tag) const
    {
        std::pmr::vector<const ActorBase*> result{Allocator::GetMemoryResource()};
        for (const Allocator::UniquePtr<ActorBase>& actor : actors)
        {
            if (!actor->IsDestroyed() && actor->CompareTag(tag))
            {
                result.push_back(actor.get());
            }
        }

        return result;
    }
} // namespace Garage
