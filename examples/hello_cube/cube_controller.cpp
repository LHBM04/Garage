#include "cube_controller.hpp"

void CubeController::OnUpdate(SystemContext& context)
{
    const InputSubsystem* input = context.GetAppSubsystem<InputSubsystem>();
    if (input == nullptr)
    {
        return;
    }

    const InputAction* rotateAction = input->GetAction("Rotate");
    if (rotateAction == nullptr)
    {
        return;
    }

    const Vector2DFloat INPUT = rotateAction->GetValue();
    if (INPUT.GetX() == 0.0f && INPUT.GetY() == 0.0f)
    {
        return;
    }

    ActorBase* owner     = GetOwner();
    Transform* transform = (owner != nullptr) ? owner->GetComponent<Transform>() : nullptr;
    if (transform == nullptr)
    {
        return;
    }

    const TimeSubsystem* timeSubsystem = context.GetEngineSubsystem<TimeSubsystem>();
    const Float32        DELTA_TIME    = (timeSubsystem != nullptr) ? timeSubsystem->GetDeltaTime() : (1.0f / 60.0f);

    constexpr Float32 ROTATION_SPEED_DEGREES = 90.0f;
    const Float32     PITCH_DEGREES          = INPUT.GetY() * ROTATION_SPEED_DEGREES * DELTA_TIME;
    const Float32     YAW_DEGREES            = -INPUT.GetX() * ROTATION_SPEED_DEGREES * DELTA_TIME;

    const Quaternion<Float32> PITCH_DELTA =
            Quaternion<Float32>::AngleAxis(PITCH_DEGREES, Vector3D<Float32>::GetRight());
    const Quaternion<Float32> YAW_DELTA = Quaternion<Float32>::AngleAxis(YAW_DEGREES, Vector3D<Float32>::GetUp());
    const Quaternion<Float32> DELTA     = PITCH_DELTA * YAW_DELTA;

    transform->SetLocalRotation(Quaternion<Float32>::Normalize(transform->GetLocalRotation() * DELTA));
}
