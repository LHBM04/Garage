#include "terrain_camera_controller.hpp"

#include <algorithm>

void TerrainCameraController::SetTerrain(const Terrain* value) noexcept
{
    terrain = value;
}

void TerrainCameraController::OnUpdate()
{
    const App*            app   = App::GetInstance();
    const InputSubsystem* input = (app != nullptr) ? app->GetSubsystem<InputSubsystem>() : nullptr;
    if (input == nullptr)
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

    if (const InputAction* lookAction = input->GetAction("Look"); lookAction != nullptr)
    {
        const Vector2DFloat LOOK = lookAction->GetValue();
        if (LOOK.GetX() != 0.0f || LOOK.GetY() != 0.0f)
        {
            constexpr Float32 LOOK_SPEED_DEGREES = 75.0f;
            yawDegrees += LOOK.GetX() * LOOK_SPEED_DEGREES * DELTA_TIME;
            pitchDegrees = std::clamp(pitchDegrees - LOOK.GetY() * LOOK_SPEED_DEGREES * DELTA_TIME, -80.0f, 80.0f);

            const Quaternion<Float32> YAW_ROTATION =
                    Quaternion<Float32>::AngleAxis(yawDegrees, Vector3D<Float32>::GetUp());
            const Quaternion<Float32> PITCH_ROTATION =
                    Quaternion<Float32>::AngleAxis(pitchDegrees, Vector3D<Float32>::GetRight());
            transform->SetLocalRotation(Quaternion<Float32>::Normalize(YAW_ROTATION * PITCH_ROTATION));
        }
    }

    if (const InputAction* moveAction = input->GetAction("Move"); moveAction != nullptr)
    {
        const Vector2DFloat MOVE = moveAction->GetValue();
        if (MOVE.GetX() != 0.0f || MOVE.GetY() != 0.0f)
        {
            constexpr Float32         MOVE_SPEED = 40.0f;
            const Quaternion<Float32> ROTATION   = transform->GetLocalRotation();
            const Vector3D<Float32>   FORWARD    = ROTATION * Vector3D<Float32>::GetForward();
            const Vector3D<Float32>   RIGHT      = ROTATION * Vector3D<Float32>::GetRight();

            Vector3D<Float32> nextPosition = transform->GetLocalPosition() +
                    (FORWARD * MOVE.GetY() + RIGHT * MOVE.GetX()) * (MOVE_SPEED * DELTA_TIME);

            if (terrain != nullptr && terrain->IsValid() && terrain->ContainsWorldPosition(nextPosition))
            {
                constexpr Float32 MIN_HEIGHT_OFFSET = 3.0f;
                const Float32     MIN_Y = terrain->SampleHeightAtWorldPosition(nextPosition) + MIN_HEIGHT_OFFSET;
                if (nextPosition.GetY() < MIN_Y)
                {
                    nextPosition.SetY(MIN_Y);
                }
            }

            transform->SetLocalPosition(nextPosition);
        }
    }
}
