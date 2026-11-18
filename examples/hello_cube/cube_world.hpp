#ifndef CUBE_WORLD_HPP
#define CUBE_WORLD_HPP

#include "garage.hpp"

using namespace Garage;

class CubeWorld final : public WorldBase
{
    GARAGE_WORLD;

protected:
    void OnEnter() override;
};

#endif // CUBE_WORLD_HPP
