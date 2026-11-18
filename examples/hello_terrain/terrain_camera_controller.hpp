#ifndef TERRAIN_CAMERA_CONTROLLER_HPP
#define TERRAIN_CAMERA_CONTROLLER_HPP

#include "garage.hpp"

using namespace Garage;

class TerrainCameraController final : public ComponentBase
{
    using ComponentBase::ComponentBase;

public:
    void OnUpdate() override;

    void SetTerrain(const Terrain* value) noexcept;

private:
    const Terrain* terrain      = nullptr;
    Float32        yawDegrees   = 0.0f;
    Float32        pitchDegrees = 10.0f;
};

#endif // TERRAIN_CAMERA_CONTROLLER_HPP
