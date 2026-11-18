#ifndef TERRAIN_WORLD_HPP
#define TERRAIN_WORLD_HPP

#include "garage.hpp"

using namespace Garage;

class TerrainWorld final : public WorldBase
{
    GARAGE_WORLD;

protected:
    void OnEnter() override;
};

#endif // TERRAIN_WORLD_HPP
