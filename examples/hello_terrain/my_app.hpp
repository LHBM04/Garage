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
    void OnInputSubsystemAdded(InputSubsystem* subsystem);
    void OnWorldSubsystemAdded(WorldSubsystem* subsystem);
    void OnAssetSubsystemAdded(AssetSubsystem* subsystem);

    EventSubsystem*   eventSubsystem   = nullptr;
    DisplaySubsystem* displaySubsystem = nullptr;
    RenderSubsystem*  renderSubsystem  = nullptr;
    InputSubsystem*   inputSubsystem   = nullptr;
    WorldSubsystem*   worldSubsystem   = nullptr;
    AssetSubsystem*   assetSubsystem   = nullptr;
};

#endif // MY_APP_HPP
