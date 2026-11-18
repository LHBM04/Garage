#ifndef TRIANGLE_WORLD_HPP
#define TRIANGLE_WORLD_HPP

#include "garage.hpp"

using namespace Garage;

class TriangleWorld final : public WorldBase
{
    GARAGE_WORLD;

protected:
    void OnEnter() override;
};

#endif // TRIANGLE_WORLD_HPP
