#ifndef GARAGE_PLATFORM_INPUT_EVENTS_HPP
#define GARAGE_PLATFORM_INPUT_EVENTS_HPP

#include "garage/core/defines.hpp"
#include "garage/math/vector2d.hpp"

namespace Garage
{
    class IDisplay;

    struct GARAGE_API InputActivationEvent final
    {
        const IDisplay* display  = nullptr;
        bool            isActive = false;
    };

    struct GARAGE_API MouseWheelEvent final
    {
        const IDisplay* display = nullptr;
        Vector2DInt     delta{0, 0};
    };
} // namespace Garage

#endif // GARAGE_PLATFORM_INPUT_EVENTS_HPP
