#include "garage/actors/world.hpp"

#include <algorithm>
#include <cmath>
#include <ranges>
#include <string_view>
#include <utility>
#include <vector>

#include "garage/actors/actor.hpp"
#include "garage/actors/transform.hpp"

#include "garage/core/common.hpp"
#include "garage/core/memory.hpp"
#include "garage/core/system_context.hpp"

#include "garage/graphics/camera.hpp"
#include "garage/graphics/directional_light.hpp"
#include "garage/graphics/gpu_command_buffer.hpp"
#include "garage/graphics/gpu_constants.hpp"
#include "garage/graphics/gpu_target.hpp"
#include "garage/graphics/gpu_types.hpp"
#include "garage/graphics/render_subsystem.hpp"

#include "garage/math/color_rgba.hpp"
#include "garage/math/quaternion.hpp"
#include "garage/math/vector3d.hpp"

#include "garage/utilities/logger.hpp"

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

    void WorldBase::Enter(SystemContext& context)
    {
        if (isEntered)
        {
            return;
        }

        RunPhase(
                [this, &context]()
                {
                    isEntered = true;
                    OnEnter(context);
                });
    }

    void WorldBase::EarlyUpdate([[maybe_unused]] SystemContext& context)
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

    void WorldBase::FixedUpdate(SystemContext& context)
    {
        UpdateActors(&WorldBase::OnFixedUpdate, &ActorBase::FixedUpdate, context);
    }

    void WorldBase::Update(SystemContext& context)
    {
        UpdateActors(&WorldBase::OnUpdate, &ActorBase::Update, context);
    }

    void WorldBase::LateUpdate(SystemContext& context)
    {
        UpdateActors(&WorldBase::OnLateUpdate, &ActorBase::LateUpdate, context);
    }

    void WorldBase::PreRender(SystemContext& context)
    {
        if (isEntered)
        {
            RunPhase(
                    [this, &context]()
                    {
                        RenderActors(&ActorBase::PreRender, context);
                    });
        }
    }

    void WorldBase::Render(SystemContext& context)
    {
        if (!isEntered)
        {
            return;
        }

        RenderSubsystem*   renderSubsystem = context.GetEngineSubsystem<RenderSubsystem>();
        IGPUCommandBuffer* commandBuffer =
                (renderSubsystem != nullptr) ? renderSubsystem->GetGPUCommandBuffer() : nullptr;
        if (commandBuffer == nullptr)
        {
            return;
        }

        RunPhase(
                [this, &context, commandBuffer]()
                {
                    for (const Camera* activeCamera : activeCameras)
                    {
                        ASSERT(activeCamera != nullptr);

                        if (!isEntered)
                        {
                            break;
                        }

                        if (!activeCamera->IsEnabled() || activeCamera->IsDestroyed())
                        {
                            continue;
                        }

                        const ActorBase* owner = activeCamera->GetOwner();
                        ASSERT(owner != nullptr);

                        if (!owner->IsEnabled() || owner->IsDestroyed())
                        {
                            continue;
                        }

                        if (commandBuffer->GetActiveTargetFormat() == EGPUTextureFormat::UNKNOWN)
                        {
                            LOG_ERROR("카메라를 적용하기 전에 렌더 타깃을 설정해야 합니다.");
                            continue;
                        }

                        const GPUViewport& VIEWPORT = activeCamera->GetViewport();
                        const GPURect&     SCISSOR  = activeCamera->GetScissorRect();
                        const GPURect&     BOUNDS   = commandBuffer->GetActiveTargetBounds();
                        if (VIEWPORT.width <= 0.0f || VIEWPORT.height <= 0.0f)
                        {
                            continue;
                        }

                        const Float32 TARGET_LEFT   = static_cast<Float32>(BOUNDS.x);
                        const Float32 TARGET_TOP    = static_cast<Float32>(BOUNDS.y);
                        const Float32 TARGET_RIGHT  = static_cast<Float32>(BOUNDS.x + BOUNDS.width);
                        const Float32 TARGET_BOTTOM = static_cast<Float32>(BOUNDS.y + BOUNDS.height);

                        const Int32 CLIPPED_LEFT = std::max(
                                SCISSOR.x,
                                static_cast<Int32>(std::floor(std::clamp(VIEWPORT.x, TARGET_LEFT, TARGET_RIGHT))));
                        const Int32 CLIPPED_TOP = std::max(
                                SCISSOR.y,
                                static_cast<Int32>(std::floor(std::clamp(VIEWPORT.y, TARGET_TOP, TARGET_BOTTOM))));
                        const Int32 CLIPPED_RIGHT =
                                std::min(SCISSOR.x + SCISSOR.width,
                                         static_cast<Int32>(std::ceil(
                                                 std::clamp(VIEWPORT.x + VIEWPORT.width, TARGET_LEFT, TARGET_RIGHT))));
                        const Int32 CLIPPED_BOTTOM =
                                std::min(SCISSOR.y + SCISSOR.height,
                                         static_cast<Int32>(std::ceil(
                                                 std::clamp(VIEWPORT.y + VIEWPORT.height, TARGET_TOP, TARGET_BOTTOM))));
                        if (CLIPPED_LEFT >= CLIPPED_RIGHT || CLIPPED_TOP >= CLIPPED_BOTTOM)
                        {
                            continue;
                        }

                        Vector3DFloat  lightDir       = Vector3DFloat{-0.5f, -1.0f, 0.5f};
                        Float32        lightIntensity = 1.0f;
                        ColorRGBAFloat lightColor     = ColorRGBAFloat::GetWhite();
                        ColorRGBAFloat ambientColor{0.15f, 0.15f, 0.15f, 1.0f};

                        if (const DirectionalLight* mainLight = GetMainLight(); mainLight != nullptr)
                        {
                            lightDir       = mainLight->GetDirection();
                            lightIntensity = mainLight->GetIntensity();
                            lightColor     = mainLight->GetColor();
                            ambientColor   = mainLight->GetAmbientColor();
                        }

                        const CameraConstants CONSTANTS{
                                .view              = activeCamera->GetViewMatrix(),
                                .projection        = activeCamera->GetProjectionMatrix(),
                                .lightDirection    = lightDir,
                                .lightIntensity    = lightIntensity,
                                .lightColor        = lightColor,
                                .ambientLightColor = ambientColor,
                        };
                        if (const auto UPLOADED = commandBuffer->SetGraphicsConstants(CONSTANTS); !UPLOADED)
                        {
                            LOG_ERROR("월드의 카메라 상수 설정에 실패했습니다: {}", UPLOADED.GetError());
                            continue;
                        }

                        commandBuffer->EndRenderPass();
                        commandBuffer->SetViewport(VIEWPORT);
                        commandBuffer->SetScissorRect(SCISSOR);
                        commandBuffer->ClearRenderTarget(activeCamera->GetClearColor());
                        commandBuffer->ClearDepthStencil(GPUTarget::DEPTH_CLEAR_VALUE);

                        RenderActors(&ActorBase::Render, context);
#if defined(GARAGE_DEBUG) || defined(_DEBUG)
                        RenderActors(&ActorBase::DrawGizmos, context);
#endif
                    }
                });
    }

    void WorldBase::PostRender(SystemContext& context)
    {
        if (isEntered)
        {
            RunPhase(
                    [this, &context]()
                    {
                        RenderActors(&ActorBase::PostRender, context);
                    });
        }
    }

    void WorldBase::Exit(SystemContext& context)
    {
        const auto EXIT = [this, &context]()
        {
            if (isEntered)
            {
                isEntered = false;
                OnExit(context);
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

    void WorldBase::UpdateActors(WorldCallback worldCallback, ActorCallback actorCallback, SystemContext& context)
    {
        ASSERT(worldCallback != nullptr);
        ASSERT(actorCallback != nullptr);

        if (!isEntered)
        {
            return;
        }

        RunPhase(
                [this, worldCallback, actorCallback, &context]()
                {
                    DestroyActors();
                    if (!isEntered)
                    {
                        return;
                    }

                    (this->*worldCallback)(context);
                    ForEachActor(
                            [actorCallback, &context](ActorBase& actor)
                            {
                                (actor.*actorCallback)(context);
                            });
                });
    }

    void WorldBase::RenderActors(ActorCallback callback, SystemContext& context)
    {
        ASSERT(callback != nullptr);

        ForEachActor(
                [callback, &context](ActorBase& actor)
                {
                    (actor.*callback)(context);
                });
    }

    ActorBase* WorldBase::Instantiate()
    {
        UniquePtr<ActorBase> actor  = MakeUnique<ActorBase>(this);
        ActorBase*           result = actor.get();
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
                                                           [](const UniquePtr<ActorBase>& actor)
                                                           {
                                                               return actor->IsDestroyed();
                                                           });
        if (DESTROYED_COUNT == 0)
        {
            return;
        }

        std::pmr::vector<UniquePtr<ActorBase>> destroyed{Memory::GetMemoryResource()};
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
            activeCameras.clear();
            activeDirectionalLights.clear();
            return;
        }

        std::pmr::vector<UniquePtr<ActorBase>> removing(std::move(actors), Memory::GetMemoryResource());
        actors.clear();

        for (UniquePtr<ActorBase>& actor : removing | std::views::reverse)
        {
            actor->Destroy();
            actor.reset();
        }

        activeCameras.clear();
        activeDirectionalLights.clear();
    }

    void WorldBase::AddCamera(Camera* camera)
    {
        ASSERT(camera != nullptr);

        if (camera != nullptr && std::ranges::find(activeCameras, camera) == activeCameras.end())
        {
            activeCameras.push_back(camera);
        }
    }

    void WorldBase::RemoveCamera(const Camera* camera)
    {
        ASSERT(camera != nullptr);

        std::erase(activeCameras, camera);
    }

    void WorldBase::AddDirectionalLight(DirectionalLight* light)
    {
        ASSERT(light != nullptr);

        if (light != nullptr && std::ranges::find(activeDirectionalLights, light) == activeDirectionalLights.end())
        {
            activeDirectionalLights.push_back(light);
        }
    }

    void WorldBase::RemoveDirectionalLight(const DirectionalLight* light)
    {
        ASSERT(light != nullptr);

        std::erase(activeDirectionalLights, light);
    }

    DirectionalLight* WorldBase::GetMainLight() noexcept
    {
        for (DirectionalLight* light : activeDirectionalLights)
        {
            if (light != nullptr && light->IsEnabled() && !light->IsDestroyed())
            {
                const ActorBase* owner = light->GetOwner();
                if (owner != nullptr && owner->IsEnabled() && !owner->IsDestroyed())
                {
                    return light;
                }
            }
        }

        return nullptr;
    }

    const DirectionalLight* WorldBase::GetMainLight() const noexcept
    {
        for (const DirectionalLight* light : activeDirectionalLights)
        {
            if (light != nullptr && light->IsEnabled() && !light->IsDestroyed())
            {
                const ActorBase* owner = light->GetOwner();
                if (owner != nullptr && owner->IsEnabled() && !owner->IsDestroyed())
                {
                    return light;
                }
            }
        }

        return nullptr;
    }

    std::pmr::vector<UniquePtr<ActorBase>> WorldBase::ExtractPersistentActors()
    {
        std::pmr::vector<UniquePtr<ActorBase>> extracted{Memory::GetMemoryResource()};
        for (auto actorIt = actors.begin(); actorIt != actors.end();)
        {
            ActorBase* actor = actorIt->get();
            if (actor->ShouldDontDestroyOnLoad() && !actor->IsDestroyed())
            {
                if (Camera* camera = actor->GetComponent<Camera>(); camera != nullptr)
                {
                    RemoveCamera(camera);
                }
                if (DirectionalLight* light = actor->GetComponent<DirectionalLight>(); light != nullptr)
                {
                    RemoveDirectionalLight(light);
                }

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

    void WorldBase::AdoptActors(std::pmr::vector<UniquePtr<ActorBase>>&& adopted)
    {
        for (UniquePtr<ActorBase>& actor : adopted)
        {
            if (actor == nullptr)
            {
                continue;
            }

            actor->world = this;
            if (Camera* camera = actor->GetComponent<Camera>(); camera != nullptr && camera->IsEnabled())
            {
                AddCamera(camera);
            }
            if (DirectionalLight* light = actor->GetComponent<DirectionalLight>();
                light != nullptr && light->IsEnabled())
            {
                AddDirectionalLight(light);
            }

            actors.push_back(std::move(actor));
        }
    }

    ActorBase* WorldBase::FindActorByName(std::string_view name) noexcept
    {
        for (const UniquePtr<ActorBase>& actor : actors)
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
        for (const UniquePtr<ActorBase>& actor : actors)
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
        std::pmr::vector<ActorBase*> result{Memory::GetMemoryResource()};
        for (const UniquePtr<ActorBase>& actor : actors)
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
        std::pmr::vector<const ActorBase*> result{Memory::GetMemoryResource()};
        for (const UniquePtr<ActorBase>& actor : actors)
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
        for (const UniquePtr<ActorBase>& actor : actors)
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
        for (const UniquePtr<ActorBase>& actor : actors)
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
        std::pmr::vector<ActorBase*> result{Memory::GetMemoryResource()};
        for (const UniquePtr<ActorBase>& actor : actors)
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
        std::pmr::vector<const ActorBase*> result{Memory::GetMemoryResource()};
        for (const UniquePtr<ActorBase>& actor : actors)
        {
            if (!actor->IsDestroyed() && actor->CompareTag(tag))
            {
                result.push_back(actor.get());
            }
        }

        return result;
    }
} // namespace Garage
