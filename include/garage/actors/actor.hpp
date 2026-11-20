#ifndef GARAGE_ACTORS_ACTOR_HPP
#define GARAGE_ACTORS_ACTOR_HPP

#include <concepts>
#include <ranges>
#include <typeindex>
#include <utility>

#include "../core/common.hpp"
#include "../core/common/containers.hpp"
#include "../core/memory.hpp"
#include "../core/objectable.hpp"

#include "component.hpp"

namespace Garage
{
    class SystemContext;
    class WorldBase;

    class GARAGE_API ActorBase : public IObjectable
    {
    public:
        explicit ActorBase(WorldBase* world) noexcept;
        explicit ActorBase(WorldBase* world, StringView name) noexcept;
        explicit ActorBase(WorldBase* world, StringView name, StringView tag) noexcept;
        ~ActorBase() noexcept override;

        ActorBase(const ActorBase&)            = delete;
        ActorBase& operator=(const ActorBase&) = delete;

        ActorBase(ActorBase&&)            = delete;
        ActorBase& operator=(ActorBase&&) = delete;

        void FixedUpdate(SystemContext& context);
        void Update(SystemContext& context);
        void LateUpdate(SystemContext& context);
        void PreRender(SystemContext& context);
        void Render(SystemContext& context);
        void PostRender(SystemContext& context);
#if defined(GARAGE_DEBUG) || defined(_DEBUG)
        void DrawGizmos(SystemContext& context);
#endif

        [[nodiscard]] const String& GetName() const noexcept;
        void                        SetName(StringView value) noexcept;

        [[nodiscard]] const String& GetTag() const noexcept;
        void                        SetTag(StringView value) noexcept;

        [[nodiscard]] bool CompareTag(StringView value) const noexcept;

        [[nodiscard]] const WorldBase* GetWorld() const noexcept;
        [[nodiscard]] WorldBase*       GetWorld() noexcept;

        [[nodiscard]] bool IsEnabled() const noexcept;
        void               SetEnabled(bool value) noexcept;

        [[nodiscard]] bool ShouldDontDestroyOnLoad() const noexcept;
        void               SetDontDestroyOnLoad(bool value) noexcept;

        [[nodiscard]] bool IsDestroyed() const noexcept;
        void               Destroy() noexcept;

        template <std::derived_from<ComponentBase> TComponent>
        TComponent* AddComponent();

        template <std::derived_from<ComponentBase> TComponent>
        [[nodiscard]] TComponent* GetComponent() noexcept;

        template <std::derived_from<ComponentBase> TComponent>
        [[nodiscard]] const TComponent* GetComponent() const noexcept;

        template <std::derived_from<ComponentBase> TComponent>
        [[nodiscard]] bool HasComponent() const noexcept;

        template <std::derived_from<ComponentBase> TComponent>
        bool RemoveComponent() noexcept;

        [[nodiscard]] auto GetComponents() noexcept;
        [[nodiscard]] auto GetComponents() const noexcept;

    private:
        friend class WorldBase;

        using ComponentCallback = void (ComponentBase::*)(SystemContext&);

        void UpdateComponents(ComponentCallback callback, SystemContext& context);
        void RenderComponents(ComponentCallback callback, SystemContext& context);
        void DestroyComponents();
        void DestroyAllComponents();

        String name{"New Actor"};
        String tag{"Untagged"};

        Vector<UniquePtr<ComponentBase>>              components;
        UnorderedMap<std::type_index, ComponentBase*> componentsByType;

        WorldBase* world = nullptr;

        bool isEnabled         = true;
        bool dontDestroyOnLoad = false;
        bool isDestroyed       = false;
    };

    inline ActorBase::ActorBase(WorldBase* world) noexcept
        : world(world)
    {
        ASSERT(world != nullptr);
    }

    inline ActorBase::ActorBase(WorldBase* world, StringView name) noexcept
        : name(name)
        , world(world)
    {
        ASSERT(world != nullptr);
    }

    inline ActorBase::ActorBase(WorldBase* world, StringView name, StringView tag) noexcept
        : name(name)
        , tag(tag)
        , world(world)
    {
        ASSERT(world != nullptr);
    }

    inline const String& ActorBase::GetName() const noexcept
    {
        return name;
    }

    inline void ActorBase::SetName(StringView value) noexcept
    {
        name = value;
    }

    inline const String& ActorBase::GetTag() const noexcept
    {
        return tag;
    }

    inline void ActorBase::SetTag(StringView value) noexcept
    {
        tag = value;
    }

    inline bool ActorBase::CompareTag(StringView value) const noexcept
    {
        return StringView(tag) == value;
    }

    inline const WorldBase* ActorBase::GetWorld() const noexcept
    {
        return world;
    }

    inline WorldBase* ActorBase::GetWorld() noexcept
    {
        return world;
    }

    inline bool ActorBase::IsEnabled() const noexcept
    {
        return isEnabled;
    }

    inline void ActorBase::SetEnabled(bool value) noexcept
    {
        isEnabled = value;
    }

    inline bool ActorBase::ShouldDontDestroyOnLoad() const noexcept
    {
        return dontDestroyOnLoad;
    }

    inline void ActorBase::SetDontDestroyOnLoad(bool value) noexcept
    {
        dontDestroyOnLoad = value;
    }

    inline bool ActorBase::IsDestroyed() const noexcept
    {
        return isDestroyed;
    }

    inline void ActorBase::Destroy() noexcept
    {
        isDestroyed = true;
    }

    template <std::derived_from<ComponentBase> TComponent>
    TComponent* ActorBase::AddComponent()
    {
        UniquePtr<TComponent> component    = MakeUnique<TComponent>(this);
        TComponent*           componentPtr = component.get();

        components.push_back(std::move(component));
        componentsByType[std::type_index(typeid(TComponent))] = componentPtr;

        ComponentBase& addedComponent = *componentPtr;
        addedComponent.OnAwake();
        if (addedComponent.IsEnabled() && !addedComponent.IsDestroyed())
        {
            addedComponent.OnEnable();
        }

        return componentPtr;
    }

    template <std::derived_from<ComponentBase> TComponent>
    TComponent* ActorBase::GetComponent() noexcept
    {
        const auto RESULT = componentsByType.find(std::type_index(typeid(TComponent)));
        if (RESULT != componentsByType.end() && !RESULT->second->IsDestroyed())
        {
            return static_cast<TComponent*>(RESULT->second);
        }

        return nullptr;
    }

    template <std::derived_from<ComponentBase> TComponent>
    const TComponent* ActorBase::GetComponent() const noexcept
    {
        const auto RESULT = componentsByType.find(std::type_index(typeid(TComponent)));
        if (RESULT != componentsByType.end() && !RESULT->second->IsDestroyed())
        {
            return static_cast<const TComponent*>(RESULT->second);
        }

        return nullptr;
    }

    template <std::derived_from<ComponentBase> TComponent>
    bool ActorBase::HasComponent() const noexcept
    {
        return GetComponent<TComponent>() != nullptr;
    }

    template <std::derived_from<ComponentBase> TComponent>
    bool ActorBase::RemoveComponent() noexcept
    {
        TComponent* component = GetComponent<TComponent>();
        if (component == nullptr)
        {
            return false;
        }

        component->Destroy();
        return true;
    }

    inline auto ActorBase::GetComponents() noexcept
    {
        return components |
                std::views::transform(
                        [](const UniquePtr<ComponentBase>& componentPtr) -> ComponentBase*
                        {
                            return componentPtr.get();
                        });
    }

    inline auto ActorBase::GetComponents() const noexcept
    {
        return components |
                std::views::transform(
                        [](const UniquePtr<ComponentBase>& componentPtr) -> const ComponentBase*
                        {
                            return componentPtr.get();
                        });
    }
} // namespace Garage

#endif // GARAGE_ACTORS_ACTOR_HPP
