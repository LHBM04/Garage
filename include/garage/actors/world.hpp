#ifndef GARAGE_ACTORS_WORLD_HPP
#define GARAGE_ACTORS_WORLD_HPP

#include <ranges>
#include <span>
#include <string_view>
#include <vector>

#include "../core/allocator.hpp"
#include "../core/defines.hpp"

#include "../math/quaternion.hpp"
#include "../math/vector3d.hpp"

#include "actor.hpp"

#define GARAGE_WORLD using WorldBase::WorldBase

namespace Garage
{
    class Camera;
    class DirectionalLight;
    class IGPUCommandBuffer;
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

        void Enter();
        void EarlyUpdate();
        void FixedUpdate();
        void Update();
        void LateUpdate();
        void PreRender(IGPUCommandBuffer& commandBuffer);
        void Render(IGPUCommandBuffer& commandBuffer);
        void PostRender(IGPUCommandBuffer& commandBuffer);
        void Exit();

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

        [[nodiscard]] std::pmr::vector<Allocator::UniquePtr<ActorBase>> ExtractPersistentActors();
        void AdoptActors(std::pmr::vector<Allocator::UniquePtr<ActorBase>>&& adopted);

        [[nodiscard]] bool IsEntered() const noexcept;

        [[nodiscard]] ActorBase*                         FindActorByName(std::string_view name) noexcept;
        [[nodiscard]] const ActorBase*                   FindActorByName(std::string_view name) const noexcept;
        [[nodiscard]] std::pmr::vector<ActorBase*>       FindActorsByName(std::string_view name);
        [[nodiscard]] std::pmr::vector<const ActorBase*> FindActorsByName(std::string_view name) const;

        [[nodiscard]] ActorBase*                         FindActorByTag(std::string_view tag) noexcept;
        [[nodiscard]] const ActorBase*                   FindActorByTag(std::string_view tag) const noexcept;
        [[nodiscard]] std::pmr::vector<ActorBase*>       FindActorsByTag(std::string_view tag);
        [[nodiscard]] std::pmr::vector<const ActorBase*> FindActorsByTag(std::string_view tag) const;

        [[nodiscard]] auto GetActors() noexcept;
        [[nodiscard]] auto GetActors() const noexcept;

        [[nodiscard]] std::span<Camera* const>       GetCameras() noexcept;
        [[nodiscard]] std::span<const Camera* const> GetCameras() const noexcept;

        [[nodiscard]] std::span<DirectionalLight* const>       GetDirectionalLights() noexcept;
        [[nodiscard]] std::span<const DirectionalLight* const> GetDirectionalLights() const noexcept;

    protected:
        virtual void OnEnter();
        virtual void OnExit();

        virtual void OnFixedUpdate();
        virtual void OnUpdate();
        virtual void OnLateUpdate();

    private:
        using WorldCallback  = void (WorldBase::*)();
        using ActorCallback  = void (ActorBase::*)();
        using RenderCallback = void (ActorBase::*)(IGPUCommandBuffer&);

        template <class TCallback>
        void RunPhase(TCallback callback);

        template <class TCallback>
        void ForEachActor(TCallback callback);

        void UpdateActors(WorldCallback worldCallback, ActorCallback actorCallback);
        void RenderActors(RenderCallback callback, IGPUCommandBuffer& commandBuffer);
        void DestroyActors();
        void ClearActors();

        bool isEntered      = false;
        bool isRunningPhase = false;

        std::pmr::vector<Allocator::UniquePtr<ActorBase>> actors{Allocator::GetMemoryResource()};
        std::pmr::vector<Camera*>                         activeCameras{Allocator::GetMemoryResource()};
        std::pmr::vector<DirectionalLight*>               activeDirectionalLights{Allocator::GetMemoryResource()};
    };

    inline bool WorldBase::IsEntered() const noexcept
    {
        return isEntered;
    }

    inline auto WorldBase::GetActors() noexcept
    {
        return actors |
                std::views::transform(
                        [](const Allocator::UniquePtr<ActorBase>& actorPtr) -> ActorBase*
                        {
                            return actorPtr.get();
                        });
    }

    inline auto WorldBase::GetActors() const noexcept
    {
        return actors |
                std::views::transform(
                        [](const Allocator::UniquePtr<ActorBase>& actorPtr) -> const ActorBase*
                        {
                            return actorPtr.get();
                        });
    }

    inline std::span<Camera* const> WorldBase::GetCameras() noexcept
    {
        return activeCameras;
    }

    inline std::span<const Camera* const> WorldBase::GetCameras() const noexcept
    {
        return activeCameras;
    }

    inline std::span<DirectionalLight* const> WorldBase::GetDirectionalLights() noexcept
    {
        return activeDirectionalLights;
    }

    inline std::span<const DirectionalLight* const> WorldBase::GetDirectionalLights() const noexcept
    {
        return activeDirectionalLights;
    }

    inline void WorldBase::OnEnter()
    {
    }

    inline void WorldBase::OnExit()
    {
    }

    inline void WorldBase::OnFixedUpdate()
    {
    }

    inline void WorldBase::OnUpdate()
    {
    }

    inline void WorldBase::OnLateUpdate()
    {
    }
} // namespace Garage

#endif // GARAGE_ACTORS_WORLD_HPP
