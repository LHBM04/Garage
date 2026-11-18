#include "garage/actors/actor.hpp"

#include <algorithm>
#include <ranges>
#include <typeindex>
#include <utility>
#include <vector>

#include "garage/actors/component.hpp"
#include "garage/actors/world.hpp"

#include "garage/core/allocator.hpp"
#include "garage/core/defines.hpp"
#include "garage/core/types.hpp"

#include "garage/graphics/gpu_command_buffer.hpp"

namespace Garage
{
    ActorBase::~ActorBase() noexcept
    {
        DestroyAllComponents();
    }

    void ActorBase::FixedUpdate()
    {
        UpdateComponents(&ComponentBase::OnFixedUpdate);
    }

    void ActorBase::Update()
    {
        UpdateComponents(&ComponentBase::OnUpdate);
    }

    void ActorBase::LateUpdate()
    {
        UpdateComponents(&ComponentBase::OnLateUpdate);
    }

    void ActorBase::PreRender(IGPUCommandBuffer& commandBuffer)
    {
        RenderComponents(&ComponentBase::OnPreRender, commandBuffer);
    }

    void ActorBase::Render(IGPUCommandBuffer& commandBuffer)
    {
        RenderComponents(&ComponentBase::OnRender, commandBuffer);
    }

    void ActorBase::PostRender(IGPUCommandBuffer& commandBuffer)
    {
        RenderComponents(&ComponentBase::OnPostRender, commandBuffer);
    }

#if defined(GARAGE_DEBUG) || defined(_DEBUG)
    void ActorBase::DrawGizmos(IGPUCommandBuffer& commandBuffer)
    {
        RenderComponents(&ComponentBase::OnDrawGizmos, commandBuffer);
    }
#endif

    void ActorBase::UpdateComponents(ComponentCallback callback)
    {
        ASSERT(callback != nullptr);

        if (world == nullptr || !world->IsEntered() || isDestroyed)
        {
            return;
        }

        DestroyComponents();

        const USize COUNT = components.size();
        for (USize index = 0; index < COUNT && world->IsEntered() && isEnabled && !isDestroyed; ++index)
        {
            ComponentBase* component = components[index].get();
            if (!component->IsEnabled() || component->IsDestroyed())
            {
                continue;
            }

            if (!component->IsStarted())
            {
                component->SetStarted(true);
                component->OnStart();
                if (!world->IsEntered() || !isEnabled || isDestroyed || !component->IsEnabled() ||
                    component->IsDestroyed())
                {
                    continue;
                }
            }

            (component->*callback)();
        }
    }

    void ActorBase::RenderComponents(RenderCallback callback, IGPUCommandBuffer& commandBuffer)
    {
        ASSERT(callback != nullptr);

        if (world == nullptr)
        {
            return;
        }

        const USize COUNT = components.size();
        for (USize index = 0; index < COUNT && world->IsEntered() && isEnabled && !isDestroyed; ++index)
        {
            ComponentBase* component = components[index].get();
            if (component->IsStarted() && component->IsEnabled() && !component->IsDestroyed())
            {
                (component->*callback)(commandBuffer);
            }
        }
    }

    void ActorBase::DestroyComponents()
    {
        const auto DESTROYED_COUNT = std::ranges::count_if(components,
                                                           [](const Allocator::UniquePtr<ComponentBase>& component)
                                                           {
                                                               return component->IsDestroyed();
                                                           });
        if (DESTROYED_COUNT == 0)
        {
            return;
        }

        std::pmr::vector<Allocator::UniquePtr<ComponentBase>> destroyed{Allocator::GetMemoryResource()};
        destroyed.reserve(static_cast<USize>(DESTROYED_COUNT));
        std::erase_if(componentsByType,
                      [](const auto& entry)
                      {
                          return entry.second->IsDestroyed();
                      });

        USize remainingCount = 0;
        for (USize index = 0; index < components.size(); ++index)
        {
            if (components[index]->IsDestroyed())
            {
                destroyed.push_back(std::move(components[index]));
            }
            else
            {
                if (remainingCount != index)
                {
                    components[remainingCount] = std::move(components[index]);
                }
                ++remainingCount;
            }
        }
        components.resize(remainingCount);

        for (const Allocator::UniquePtr<ComponentBase>& component : components)
        {
            componentsByType[std::type_index(typeid(*component))] = component.get();
        }

        for (const Allocator::UniquePtr<ComponentBase>& component : destroyed)
        {
            if (component->IsEnabled())
            {
                component->OnDisable();
            }
            component->OnDestroy();
        }
    }

    void ActorBase::DestroyAllComponents()
    {
        if (components.empty())
        {
            return;
        }

        componentsByType.clear();
        std::pmr::vector<Allocator::UniquePtr<ComponentBase>> removing(std::move(components),
                                                                       Allocator::GetMemoryResource());
        components.clear();

        for (const Allocator::UniquePtr<ComponentBase>& component : removing | std::views::reverse)
        {
            const bool WAS_ENABLED = component->IsEnabled();
            component->Destroy();
            if (WAS_ENABLED)
            {
                component->OnDisable();
            }
            component->OnDestroy();
        }
    }
} // namespace Garage
