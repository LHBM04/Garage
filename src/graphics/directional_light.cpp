#include "garage/graphics/directional_light.hpp"

#include "garage/actors/actor.hpp"
#include "garage/actors/transform.hpp"
#include "garage/actors/world.hpp"

#include "garage/core/types.hpp"

#include "garage/graphics/gpu_constants.hpp"

#include "garage/math/quaternion.hpp"
#include "garage/math/vector3d.hpp"

namespace Garage
{
    void DirectionalLight::OnEnable()
    {
        if (ActorBase* owner = GetOwner(); owner != nullptr)
        {
            if (WorldBase* world = owner->GetWorld(); world != nullptr)
            {
                world->AddDirectionalLight(this);
            }
        }
    }

    void DirectionalLight::OnDisable()
    {
        if (ActorBase* owner = GetOwner(); owner != nullptr)
        {
            if (WorldBase* world = owner->GetWorld(); world != nullptr)
            {
                world->RemoveDirectionalLight(this);
            }
        }
    }

    Vector3D<Float32> DirectionalLight::GetDirection() const noexcept
    {
        if (useTransformRotation)
        {
            const ActorBase* owner = GetOwner();
            if (owner != nullptr)
            {
                const Transform* transform = owner->GetComponent<Transform>();
                if (transform != nullptr)
                {
                    const Quaternion<Float32> ROTATION          = transform->GetWorldRotation();
                    const Vector3D<Float32>   ROTATED_DIRECTION = ROTATION * direction;
                    if (!ROTATED_DIRECTION.IsZero())
                    {
                        return ROTATED_DIRECTION.GetNormalized();
                    }
                }
            }
        }

        if (!direction.IsZero())
        {
            return direction.GetNormalized();
        }

        return Vector3D<Float32>::GetDown();
    }

    DirectionalLightConstants DirectionalLight::GetConstants() const noexcept
    {
        return DirectionalLightConstants{
                .direction    = GetDirection(),
                .intensity    = intensity,
                .color        = GetEffectiveColor(),
                .ambientColor = GetEffectiveAmbientColor(),
        };
    }
} // namespace Garage
