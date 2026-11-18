#ifndef MY_ENGINE_HPP
#define MY_ENGINE_HPP

#include "garage.hpp"

using namespace Garage;

class MyEngine : public Engine
{
    using Engine::Engine;

public:
    [[nodiscard]] ESystemStatus Startup() override;

private:
    DisplaySubsystem* displays = nullptr;
    EventSubsystem*   events   = nullptr;
};

#endif // MY_ENGINE_HPP