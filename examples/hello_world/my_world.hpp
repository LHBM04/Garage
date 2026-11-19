#ifndef MY_WORLD_HPP
#define MY_WORLD_HPP

#include "garage.hpp"

using namespace Garage;

class MyWorld final : public WorldBase
{
    GARAGE_WORLD;

protected:
    void OnEnter(SystemContext& context) override;
};

#endif // MY_WORLD_HPP
