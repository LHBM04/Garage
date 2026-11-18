#ifndef MY_APP_HPP
#define MY_APP_HPP

#include "garage.hpp"

using namespace Garage;

class MyApp : public App
{
    using App::App;

public:
    [[nodiscard]] ESystemStatus Startup() override;

private:
    EventSubsystem*   eventSubsystem   = nullptr;
    DisplaySubsystem* displaySubsystem = nullptr;
};

#endif // MY_APP_HPP
