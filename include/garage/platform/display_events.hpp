#ifndef GARAGE_PLATFORM_DISPLAY_EVENTS_HPP
#define GARAGE_PLATFORM_DISPLAY_EVENTS_HPP

#include "garage/core/defines.hpp"
#include "garage/math/vector2d.hpp"

namespace Garage
{
    class IDisplay;

    struct GARAGE_API DisplayCreateEvent final
    {
        const IDisplay* display = nullptr;
    };

    struct GARAGE_API DisplayCloseEvent final
    {
        const IDisplay* display = nullptr;
    };

    struct GARAGE_API DisplayDestroyEvent final
    {
        const IDisplay* display = nullptr;
    };

    struct GARAGE_API DisplayFocusEvent final
    {
        const IDisplay* display   = nullptr;
        bool            isFocused = false;
    };

    struct GARAGE_API DisplayMoveEvent final
    {
        const IDisplay* display = nullptr;
        Vector2DInt     position{0, 0};
    };

    struct GARAGE_API DisplayResizeEvent final
    {
        const IDisplay* display = nullptr;
        Vector2DInt     size{0, 0};
    };
} // namespace Garage

#endif // GARAGE_PLATFORM_DISPLAY_EVENTS_HPP
