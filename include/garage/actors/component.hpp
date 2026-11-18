#ifndef GARAGE_ACTORS_COMPONENT_HPP
#define GARAGE_ACTORS_COMPONENT_HPP

#include "../core/defines.hpp"

namespace Garage
{
    class ActorBase;
    class IGPUCommandBuffer;

    class GARAGE_API ComponentBase
    {
    public:
        explicit ComponentBase(ActorBase* owner) noexcept;
        virtual ~ComponentBase() noexcept = default;

        ComponentBase(const ComponentBase&)            = delete;
        ComponentBase& operator=(const ComponentBase&) = delete;

        ComponentBase(ComponentBase&&)            = delete;
        ComponentBase& operator=(ComponentBase&&) = delete;

        [[nodiscard]] ActorBase*       GetOwner() noexcept;
        [[nodiscard]] const ActorBase* GetOwner() const noexcept;

        [[nodiscard]] bool IsStarted() const noexcept;

        [[nodiscard]] bool IsEnabled() const noexcept;
        void               SetEnabled(bool enabled) noexcept;

        [[nodiscard]] bool IsDestroyed() const noexcept;
        void               Destroy() noexcept;

    protected:
        virtual void OnAwake();
        virtual void OnStart();
        virtual void OnEnable();
        virtual void OnDisable();
        virtual void OnFixedUpdate();
        virtual void OnUpdate();
        virtual void OnLateUpdate();
        virtual void OnPreRender(IGPUCommandBuffer& commandBuffer);
        virtual void OnRender(IGPUCommandBuffer& commandBuffer);
        virtual void OnPostRender(IGPUCommandBuffer& commandBuffer);
#if defined(GARAGE_DEBUG) || defined(_DEBUG)
        virtual void OnDrawGizmos(IGPUCommandBuffer& commandBuffer);
#endif
        virtual void OnDestroy();

    private:
        friend class ActorBase;

        void SetStarted(bool started) noexcept;

        ActorBase* owner       = nullptr;
        bool       isStarted   = false;
        bool       isEnabled   = true;
        bool       isDestroyed = false;
    };

    inline ComponentBase::ComponentBase(ActorBase* owner) noexcept
        : owner(owner)
    {
        ASSERT(owner != nullptr);
    }

    inline ActorBase* ComponentBase::GetOwner() noexcept
    {
        return owner;
    }

    inline const ActorBase* ComponentBase::GetOwner() const noexcept
    {
        return owner;
    }

    inline bool ComponentBase::IsStarted() const noexcept
    {
        return isStarted;
    }

    inline bool ComponentBase::IsEnabled() const noexcept
    {
        return isEnabled;
    }

    inline void ComponentBase::SetEnabled(bool enabled) noexcept
    {
        if (isEnabled == enabled)
        {
            return;
        }

        isEnabled = enabled;
        if (!isDestroyed)
        {
            if (isEnabled)
            {
                OnEnable();
            }
            else
            {
                OnDisable();
            }
        }
    }

    inline bool ComponentBase::IsDestroyed() const noexcept
    {
        return isDestroyed;
    }

    inline void ComponentBase::Destroy() noexcept
    {
        isDestroyed = true;
    }

    inline void ComponentBase::OnAwake()
    {
    }

    inline void ComponentBase::OnStart()
    {
    }

    inline void ComponentBase::OnEnable()
    {
    }

    inline void ComponentBase::OnDisable()
    {
    }

    inline void ComponentBase::OnFixedUpdate()
    {
    }

    inline void ComponentBase::OnUpdate()
    {
    }

    inline void ComponentBase::OnLateUpdate()
    {
    }

    inline void ComponentBase::OnPreRender(IGPUCommandBuffer& /*commandBuffer*/)
    {
    }

    inline void ComponentBase::OnRender(IGPUCommandBuffer& /*commandBuffer*/)
    {
    }

    inline void ComponentBase::OnPostRender(IGPUCommandBuffer& /*commandBuffer*/)
    {
    }

#if defined(GARAGE_DEBUG) || defined(_DEBUG)
    inline void ComponentBase::OnDrawGizmos(IGPUCommandBuffer& /*commandBuffer*/)
    {
    }
#endif

    inline void ComponentBase::OnDestroy()
    {
    }

    inline void ComponentBase::SetStarted(bool started) noexcept
    {
        isStarted = started;
    }
} // namespace Garage

#endif // GARAGE_ACTORS_COMPONENT_HPP
