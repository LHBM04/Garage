#ifndef GARAGE_PLATFORM_DISPLAY_OPTIONS_HPP
#define GARAGE_PLATFORM_DISPLAY_OPTIONS_HPP

#include <string>

#include "garage/core/defines.hpp"
#include "garage/core/types.hpp"
#include "garage/math/vector2d.hpp"

namespace Garage
{
    struct GARAGE_API DisplayOptions final
    {
        std::string title = "Garage";
        Vector2DInt position{0, 0};
        Vector2DInt size{1280, 720};
        UInt64      flags    = 0;
        bool        centered = true;
        bool        visible  = true;
    };
} // namespace Garage

#endif // GARAGE_PLATFORM_DISPLAY_OPTIONS_HPP
