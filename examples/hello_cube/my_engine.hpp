#ifndef MY_ENGINE_HPP
#define MY_ENGINE_HPP

#include "garage.hpp"

using namespace Garage;

class MyEngine final : public Engine
{
    using Engine::Engine;

public:
    [[nodiscard]] ESystemStatus Startup(SystemContext& context) override;

private:
    DisplaySubsystem* displays  = nullptr;
    EventSubsystem*   events    = nullptr;
    RenderSubsystem*  rendering = nullptr;
};

#endif // MY_ENGINE_HPP
