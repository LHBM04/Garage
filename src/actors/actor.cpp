#include "garage/actors/actor.hpp"

#include <algorithm>
#include <ranges>
#include <typeindex>
#include <utility>
#include <vector>

#include "garage/actors/component.hpp"
#include "garage/actors/world.hpp"

#include "garage/core/common.hpp"
#include "garage/core/memory.hpp"

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

    void ActorBase::PreRender(SystemContext& context)
    {
        RenderComponents(&ComponentBase::OnPreRender, context);
    }

    void ActorBase::Render(SystemContext& context)
    {
        RenderComponents(&ComponentBase::OnRender, context);
    }

    void ActorBase::PostRender(SystemContext& context)
    {
        RenderComponents(&ComponentBase::OnPostRender, context);
    }

#if defined(GARAGE_DEBUG) || defined(_DEBUG)
    void ActorBase::DrawGizmos(SystemContext& context)
    {
        RenderComponents(&ComponentBase::OnDrawGizmos, context);
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

    void ActorBase::RenderComponents(ComponentCallback callback, SystemContext& context)
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
                (component->*callback)(context);
            }
        }
    }

    void ActorBase::DestroyComponents()
    {
        const auto DESTROYED_COUNT = std::ranges::count_if(components,
                                                           [](const UniquePtr<ComponentBase>& component)
                                                           {
                                                               return component->IsDestroyed();
                                                           });
        if (DESTROYED_COUNT == 0)
        {
            return;
        }

        std::pmr::vector<UniquePtr<ComponentBase>> destroyed{Memory::GetMemoryResource()};
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

        for (const UniquePtr<ComponentBase>& component : components)
        {
            componentsByType[std::type_index(typeid(*component))] = component.get();
        }

        for (const UniquePtr<ComponentBase>& component : destroyed)
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
        std::pmr::vector<UniquePtr<ComponentBase>> removing(std::move(components), Memory::GetMemoryResource());
        components.clear();

        for (const UniquePtr<ComponentBase>& component : removing | std::views::reverse)
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
