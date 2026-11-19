#ifndef GARAGE_ACTORS_WORLD_HPP
#define GARAGE_ACTORS_WORLD_HPP

#include <ranges>

#include "../core/common.hpp"
#include "../core/common/containers.hpp"
#include "../core/memory.hpp"

#include "../math/quaternion.hpp"
#include "../math/vector3d.hpp"

#include "actor.hpp"

#define GARAGE_WORLD using WorldBase::WorldBase

namespace Garage
{
    class Camera;
    class DirectionalLight;
    class SystemContext;
    class Transform;

    class GARAGE_API WorldBase
    {
    public:
        WorldBase() noexcept          = default;
        virtual ~WorldBase() noexcept = default;

        WorldBase(const WorldBase&)            = delete;
        WorldBase& operator=(const WorldBase&) = delete;

        WorldBase(WorldBase&&)            = delete;
        WorldBase& operator=(WorldBase&&) = delete;

        void Enter(SystemContext& context);
        void EarlyUpdate(SystemContext& context);
        void FixedUpdate(SystemContext& context);
        void Update(SystemContext& context);
        void LateUpdate(SystemContext& context);
        void PreRender(SystemContext& context);
        void Render(SystemContext& context);
        void PostRender(SystemContext& context);
        void Exit(SystemContext& context);

        ActorBase* Instantiate();
        ActorBase* Instantiate(const Vector3D<Float32>& position, const Quaternion<Float32>& rotation);
        ActorBase* Instantiate(const Vector3D<Float32>& position);
        ActorBase* Instantiate(const Quaternion<Float32>& rotation);
        ActorBase* Instantiate(Transform* parent);
        ActorBase*
                Instantiate(const Vector3D<Float32>& position, const Quaternion<Float32>& rotation, Transform* parent);

        void Destroy(ActorBase* actor) noexcept;

        void AddCamera(Camera* camera);
        void RemoveCamera(const Camera* camera);

        void AddDirectionalLight(DirectionalLight* light);
        void RemoveDirectionalLight(const DirectionalLight* light);

        [[nodiscard]] DirectionalLight*       GetMainLight() noexcept;
        [[nodiscard]] const DirectionalLight* GetMainLight() const noexcept;

        [[nodiscard]] Vector<UniquePtr<ActorBase>> ExtractPersistentActors();
        void                                       AdoptActors(Vector<UniquePtr<ActorBase>>&& adopted);

        [[nodiscard]] bool IsEntered() const noexcept;

        [[nodiscard]] ActorBase*               FindActorByName(StringView name) noexcept;
        [[nodiscard]] const ActorBase*         FindActorByName(StringView name) const noexcept;
        [[nodiscard]] Vector<ActorBase*>       FindActorsByName(StringView name);
        [[nodiscard]] Vector<const ActorBase*> FindActorsByName(StringView name) const;

        [[nodiscard]] ActorBase*               FindActorByTag(StringView tag) noexcept;
        [[nodiscard]] const ActorBase*         FindActorByTag(StringView tag) const noexcept;
        [[nodiscard]] Vector<ActorBase*>       FindActorsByTag(StringView tag);
        [[nodiscard]] Vector<const ActorBase*> FindActorsByTag(StringView tag) const;

        [[nodiscard]] auto GetActors() noexcept;
        [[nodiscard]] auto GetActors() const noexcept;

        [[nodiscard]] Span<Camera* const>       GetCameras() noexcept;
        [[nodiscard]] Span<const Camera* const> GetCameras() const noexcept;

        [[nodiscard]] Span<DirectionalLight* const>       GetDirectionalLights() noexcept;
        [[nodiscard]] Span<const DirectionalLight* const> GetDirectionalLights() const noexcept;

    protected:
        virtual void OnEnter(SystemContext& context);
        virtual void OnExit(SystemContext& context);

        virtual void OnFixedUpdate(SystemContext& context);
        virtual void OnUpdate(SystemContext& context);
        virtual void OnLateUpdate(SystemContext& context);

    private:
        using WorldCallback = void (WorldBase::*)(SystemContext&);
        using ActorCallback = void (ActorBase::*)(SystemContext&);

        template <class TCallback>
        void RunPhase(TCallback callback);

        template <class TCallback>
        void ForEachActor(TCallback callback);

        void UpdateActors(WorldCallback worldCallback, ActorCallback actorCallback, SystemContext& context);
        void RenderActors(ActorCallback callback, SystemContext& context);
        void DestroyActors();
        void ClearActors();

        bool isEntered      = false;
        bool isRunningPhase = false;

        Vector<UniquePtr<ActorBase>> actors;
        Vector<Camera*>              activeCameras;
        Vector<DirectionalLight*>    activeDirectionalLights;
    };

    inline bool WorldBase::IsEntered() const noexcept
    {
        return isEntered;
    }

    inline auto WorldBase::GetActors() noexcept
    {
        return actors |
                std::views::transform(
                        [](const UniquePtr<ActorBase>& actorPtr) -> ActorBase*
                        {
                            return actorPtr.get();
                        });
    }

    inline auto WorldBase::GetActors() const noexcept
    {
        return actors |
                std::views::transform(
                        [](const UniquePtr<ActorBase>& actorPtr) -> const ActorBase*
                        {
                            return actorPtr.get();
                        });
    }

    inline Span<Camera* const> WorldBase::GetCameras() noexcept
    {
        return activeCameras;
    }

    inline Span<const Camera* const> WorldBase::GetCameras() const noexcept
    {
        return activeCameras;
    }

    inline Span<DirectionalLight* const> WorldBase::GetDirectionalLights() noexcept
    {
        return activeDirectionalLights;
    }

    inline Span<const DirectionalLight* const> WorldBase::GetDirectionalLights() const noexcept
    {
        return activeDirectionalLights;
    }

    inline void WorldBase::OnEnter([[maybe_unused]] SystemContext& context)
    {
    }

    inline void WorldBase::OnExit([[maybe_unused]] SystemContext& context)
    {
    }

    inline void WorldBase::OnFixedUpdate([[maybe_unused]] SystemContext& context)
    {
    }

    inline void WorldBase::OnUpdate([[maybe_unused]] SystemContext& context)
    {
    }

    inline void WorldBase::OnLateUpdate([[maybe_unused]] SystemContext& context)
    {
    }
} // namespace Garage

#endif // GARAGE_ACTORS_WORLD_HPP
