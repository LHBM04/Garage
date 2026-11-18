#include "cube_controller.hpp"

void CubeController::OnUpdate()
{
    const App*            app   = App::GetInstance();
    const InputSubsystem* input = (app != nullptr) ? app->GetSubsystem<InputSubsystem>() : nullptr;
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

    const Engine*        engine        = Engine::GetInstance();
    const TimeSubsystem* timeSubsystem = (engine != nullptr) ? engine->GetSubsystem<TimeSubsystem>() : nullptr;
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
