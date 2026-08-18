#include "garage/entry.hpp"

#include <span>
#include <iostream>

#include "garage/engine.hpp"
#include "garage/platform/window_subsystem.hpp"

namespace Garage
{
    Engine* gEngine = nullptr;
}

extern "C" int main(int argc, char* argv[])
{
    Garage::gEngine = new Garage::Engine();
    
    std::span<char*> args = std::span(argv + 1, static_cast<size_t>(argc - 1));
    
    for (std::string_view arg : args)
    {
        if (arg == "-s" || arg == "--server")
        {
            std::cout << "Garage Server" << std::endl;
        }
        else if (arg == "-c" || arg == "--client")
        {
            std::cout << "Garage Client" << std::endl;
        }
        else
        {
            // 알 수 없는 인수
        }
    }
    
    delete Garage::gEngine;
    return 0;
}
