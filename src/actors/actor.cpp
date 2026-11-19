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

    void ActorBase::FixedUpdate(SystemContext& context)
    {
        UpdateComponents(&ComponentBase::OnFixedUpdate, context);
    }

    void ActorBase::Update(SystemContext& context)
    {
        UpdateComponents(&ComponentBase::OnUpdate, context);
    }

    void ActorBase::LateUpdate(SystemContext& context)
    {
        UpdateComponents(&ComponentBase::OnLateUpdate, context);
    }

    void ActorBase::PreRender(SystemContext& context, IGPUCommandBuffer& commandBuffer)
    {
        RenderComponents(&ComponentBase::OnPreRender, context, commandBuffer);
    }

    void ActorBase::Render(SystemContext& context, IGPUCommandBuffer& commandBuffer)
    {
        RenderComponents(&ComponentBase::OnRender, context, commandBuffer);
    }

    void ActorBase::PostRender(SystemContext& context, IGPUCommandBuffer& commandBuffer)
    {
        RenderComponents(&ComponentBase::OnPostRender, context, commandBuffer);
    }

#if defined(GARAGE_DEBUG) || defined(_DEBUG)
    void ActorBase::DrawGizmos(SystemContext& context, IGPUCommandBuffer& commandBuffer)
    {
        RenderComponents(&ComponentBase::OnDrawGizmos, context, commandBuffer);
    }
#endif

    void ActorBase::UpdateComponents(ComponentCallback callback, SystemContext& context)
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
                component->OnStart(context);
                if (!world->IsEntered() || !isEnabled || isDestroyed || !component->IsEnabled() ||
                    component->IsDestroyed())
                {
                    continue;
                }
            }

            (component->*callback)(context);
        }
    }

    void ActorBase::RenderComponents(RenderCallback     callback,
                                     SystemContext&     context,
                                     IGPUCommandBuffer& commandBuffer)
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
                (component->*callback)(context, commandBuffer);
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
