#include <span>

#include "garage/engine.hpp"
#include "garage/platform/window_subsystem.h"

Garage::Engine* engine{ nullptr };

int main(int argc, char **argv)
{
    engine = new Garage::Engine();
    
    std::span<char*> args = std::span(argv + 1, static_cast<size_t>(argc - 1));
    
    for (char*& arg : args)
    {
        if (arg == "-s" || arg == "--server")
        {
            
        }
        else if (arg == "-c" || arg == "--client")
        {
            Garage::WindowSubsystem* subsystem = engine->GetSubsystem<Garage::WindowSubsystem>();
        }
        else
        {
            // 알 수 없는 인수
        }
    }
    
    Garage::Engine engine;
    return 0;
}
