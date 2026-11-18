#ifndef GARAGE_ACTORS_ACTOR_HPP
#define GARAGE_ACTORS_ACTOR_HPP

#include <concepts>
#include <ranges>
#include <string>
#include <string_view>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../core/allocator.hpp"
#include "../core/defines.hpp"

#include "component.hpp"

namespace Garage
{
    class IGPUCommandBuffer;
    class WorldBase;

    class GARAGE_API ActorBase
    {
    public:
        explicit ActorBase(WorldBase* world) noexcept;
        explicit ActorBase(WorldBase* world, std::string_view name) noexcept;
        explicit ActorBase(WorldBase* world, std::string_view name, std::string_view tag) noexcept;
        virtual ~ActorBase() noexcept;

        ActorBase(const ActorBase&)            = delete;
        ActorBase& operator=(const ActorBase&) = delete;

        ActorBase(ActorBase&&)            = delete;
        ActorBase& operator=(ActorBase&&) = delete;

        void FixedUpdate();
        void Update();
        void LateUpdate();
        void PreRender(IGPUCommandBuffer& commandBuffer);
        void Render(IGPUCommandBuffer& commandBuffer);
        void PostRender(IGPUCommandBuffer& commandBuffer);
#if defined(GARAGE_DEBUG) || defined(_DEBUG)
        void DrawGizmos(IGPUCommandBuffer& commandBuffer);
#endif

        [[nodiscard]] const std::pmr::string& GetName() const noexcept;
        void                                  SetName(std::string_view value) noexcept;

        [[nodiscard]] const std::pmr::string& GetTag() const noexcept;
        void                                  SetTag(std::string_view value) noexcept;

        [[nodiscard]] bool CompareTag(std::string_view value) const noexcept;

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

        using ComponentCallback = void (ComponentBase::*)();
        using RenderCallback    = void (ComponentBase::*)(IGPUCommandBuffer&);

        void UpdateComponents(ComponentCallback callback);
        void RenderComponents(RenderCallback callback, IGPUCommandBuffer& commandBuffer);
        void DestroyComponents();
        void DestroyAllComponents();

        std::pmr::string name{"New Actor", Allocator::GetMemoryResource()};
        std::pmr::string tag{"Untagged", Allocator::GetMemoryResource()};

        std::pmr::vector<Allocator::UniquePtr<ComponentBase>>    components{Allocator::GetMemoryResource()};
        std::pmr::unordered_map<std::type_index, ComponentBase*> componentsByType{Allocator::GetMemoryResource()};

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

    inline ActorBase::ActorBase(WorldBase* world, std::string_view name) noexcept
        : name(name, Allocator::GetMemoryResource())
        , world(world)
    {
        ASSERT(world != nullptr);
    }

    inline ActorBase::ActorBase(WorldBase* world, std::string_view name, std::string_view tag) noexcept
        : name(name, Allocator::GetMemoryResource())
        , tag(tag, Allocator::GetMemoryResource())
        , world(world)
    {
        ASSERT(world != nullptr);
    }

    inline const std::pmr::string& ActorBase::GetName() const noexcept
    {
        return name;
    }

    inline void ActorBase::SetName(std::string_view value) noexcept
    {
        name = value;
    }

    inline const std::pmr::string& ActorBase::GetTag() const noexcept
    {
        return tag;
    }

    inline void ActorBase::SetTag(std::string_view value) noexcept
    {
        tag = value;
    }

    inline bool ActorBase::CompareTag(std::string_view value) const noexcept
    {
        return std::string_view(tag) == value;
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
        Allocator::UniquePtr<TComponent> component    = Allocator::MakeUnique<TComponent>(this);
        TComponent*                      componentPtr = component.get();

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
                        [](const Allocator::UniquePtr<ComponentBase>& componentPtr) -> ComponentBase*
                        {
                            return componentPtr.get();
                        });
    }

    inline auto ActorBase::GetComponents() const noexcept
    {
        return components |
                std::views::transform(
                        [](const Allocator::UniquePtr<ComponentBase>& componentPtr) -> const ComponentBase*
                        {
                            return componentPtr.get();
                        });
    }
} // namespace Garage

#endif // GARAGE_ACTORS_ACTOR_HPP
