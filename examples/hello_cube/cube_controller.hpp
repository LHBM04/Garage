#ifndef CUBE_CONTROLLER_HPP
#define CUBE_CONTROLLER_HPP

#include "garage.hpp"

using namespace Garage;

class CubeController final : public ComponentBase
{
    using ComponentBase::ComponentBase;

public:
    void OnUpdate(SystemContext& context) override;
};

#endif // CUBE_CONTROLLER_HPP
