#ifndef GARAGE_PLATFORM_EVENT_HPP
#define GARAGE_PLATFORM_EVENT_HPP

#include <concepts>
#include <type_traits>
#include <variant>

#include "display_events.hpp"
#include "input_events.hpp"

namespace Garage
{
    using Event = std::variant<DisplayCreateEvent,
                               DisplayCloseEvent,
                               DisplayDestroyEvent,
                               DisplayFocusEvent,
                               DisplayMoveEvent,
                               DisplayResizeEvent,
                               InputActivationEvent,
                               MouseWheelEvent>;

    template <class TEvent, class TVariant>
    struct IsEventMember : std::false_type
    {
    };

    template <class TEvent, class... TEvents>
    struct IsEventMember<TEvent, std::variant<TEvents...>>
        : std::bool_constant<(std::same_as<std::remove_cvref_t<TEvent>, TEvents> || ...)>
    {
    };

    template <class TEvent>
    concept Eventable = IsEventMember<TEvent, Event>::value;
} // namespace Garage

#endif // GARAGE_PLATFORM_EVENT_HPP
