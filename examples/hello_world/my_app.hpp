#ifndef MY_APP_HPP
#define MY_APP_HPP

#include "garage.hpp"

using namespace Garage;

class MyApp final : public App
{
    using App::App;

public:
    [[nodiscard]] ESystemStatus Startup(SystemContext& context) override;

private:
    void OnWorldSubsystemAdded(WorldSubsystem* subsystem);
    void OnContentSubsystemAdded(ContentSubsystem* subsystem);

    EventSubsystem*   eventSubsystem   = nullptr;
    DisplaySubsystem* displaySubsystem = nullptr;
    RenderSubsystem*  renderSubsystem  = nullptr;
    WorldSubsystem*   worldSubsystem   = nullptr;
    ContentSubsystem* contentSubsystem = nullptr;
};

#endif // MY_APP_HPP
