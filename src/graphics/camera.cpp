#include "garage/graphics/camera.hpp"

#include "garage/actors/actor.hpp"
#include "garage/actors/transform.hpp"
#include "garage/actors/world.hpp"

#include "garage/core/defines.hpp"
#include "garage/core/types.hpp"

#include "garage/math/matrix4x4.hpp"
#include "garage/math/quaternion.hpp"
#include "garage/math/vector3d.hpp"

namespace Garage
{
    void Camera::OnEnable()
    {
        if (ActorBase* owner = GetOwner(); owner != nullptr)
        {
            if (WorldBase* world = owner->GetWorld(); world != nullptr)
            {
                world->AddCamera(this);
            }
        }
    }

    void Camera::OnDisable()
    {
        if (ActorBase* owner = GetOwner(); owner != nullptr)
        {
            if (WorldBase* world = owner->GetWorld(); world != nullptr)
            {
                world->RemoveCamera(this);
            }
        }
    }

    Matrix4x4<Float32> Camera::GetViewMatrix() const noexcept
    {
        const ActorBase* owner     = GetOwner();
        const Transform* transform = (owner != nullptr) ? owner->GetComponent<Transform>() : nullptr;
        if (transform == nullptr)
        {
            return Matrix4x4<Float32>::GetIdentity();
        }

        const Vector3D<Float32>   POSITION = transform->GetWorldPosition();
        const Quaternion<Float32> ROTATION = transform->GetWorldRotation();

        const Matrix4x4<Float32> TRANSLATION_MATRIX = Matrix4x4<Float32>::Translate(-POSITION);
        const Matrix4x4<Float32> ROTATION_MATRIX = Matrix4x4<Float32>::Rotate(Quaternion<Float32>::Inverse(ROTATION));

        return TRANSLATION_MATRIX * ROTATION_MATRIX;
    }

    Matrix4x4<Float32> Camera::GetProjectionMatrix() const noexcept
    {
        ASSERT(viewport.width > 0.0f);
        ASSERT(viewport.height > 0.0f);

        const Float32 ASPECT_RATIO = viewport.width / viewport.height;

        switch (projectionType)
        {
            case ECameraProjectionType::ORTHOGRAPHIC:
            {
                const Float32 HALF_HEIGHT = orthoSize * 0.5f;
                const Float32 HALF_WIDTH  = HALF_HEIGHT * ASPECT_RATIO;

                return Matrix4x4<Float32>::Ortho(
                        -HALF_WIDTH, HALF_WIDTH, -HALF_HEIGHT, HALF_HEIGHT, nearPlane, farPlane);
            }
            case ECameraProjectionType::PERSPECTIVE:
            {
                return Matrix4x4<Float32>::Perspective(fieldOfView, ASPECT_RATIO, nearPlane, farPlane);
            }
            default:
            {
                return Matrix4x4<Float32>::GetIdentity();
            }
        }
    }
} // namespace Garage
