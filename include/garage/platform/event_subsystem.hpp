#ifndef GARAGE_PLATFORM_EVENT_SUBSYSTEM_HPP
#define GARAGE_PLATFORM_EVENT_SUBSYSTEM_HPP

#include <functional>
#include <tuple>
#include <utility>
#include <variant>

#include "../core/common.hpp"
#include "../core/common/containers.hpp"
#include "../core/engine_subsystem.hpp"
#include "../core/system_status.hpp"

#include "event.hpp"

namespace Garage
{
    class SystemContext;

    class GARAGE_API EventSubsystem final : public EngineSubsystemBase
    {
    public:
        using ListenerId = Int32;

        EventSubsystem() noexcept;

        [[nodiscard]] ESystemStatus OnStartup(SystemContext& context) override;
        [[nodiscard]] ESystemStatus OnPreTick(SystemContext& context) override;
        [[nodiscard]] ESystemStatus OnTick(SystemContext& context) override;
        [[nodiscard]] ESystemStatus OnPostTick(SystemContext& context) override;
        void                        OnShutdown(SystemContext& context) override;

        template <Eventable TEvent>
        void PushEvent(const TEvent& event);

        template <Eventable TEvent, class... TArgs>
        void PushEvent(TArgs&&... args);

        template <Eventable TEvent>
        void DispatchEvent(const TEvent& event);

        template <Eventable TEvent, class... TArgs>
        void DispatchEvent(TArgs&&... args);

        template <Eventable TEvent>
        ListenerId Subscribe(Action<const TEvent&> callback);

        template <Eventable TEvent, class TObject>
        ListenerId Subscribe(TObject* object, void (TObject::*method)(const TEvent&));

        template <Eventable TEvent, class TObject>
        ListenerId Subscribe(TObject* object, void (TObject::*method)(const TEvent&) noexcept);

        template <Eventable TEvent>
        void Unsubscribe(ListenerId id) noexcept;

    private:
        template <class TVariant>
        struct ListenerTuple;

        template <class... TEvents>
        struct ListenerTuple<std::variant<TEvents...>>
        {
            using Type = std::tuple<Delegate<const TEvents&>...>;
        };

        Vector<Event>              writeBuffer;
        Vector<Event>              readBuffer;
        ListenerTuple<Event>::Type listeners = {};
    };

    inline EventSubsystem::EventSubsystem() noexcept
        : EngineSubsystemBase(0)
    {
    }

    template <Eventable TEvent>
    void EventSubsystem::PushEvent(const TEvent& event)
    {
        writeBuffer.emplace_back(event);
    }

    template <Eventable TEvent, class... TArgs>
    void EventSubsystem::PushEvent(TArgs&&... args)
    {
        writeBuffer.emplace_back(std::in_place_type<TEvent>, TEvent{std::forward<TArgs>(args)...});
    }

    template <Eventable TEvent>
    void EventSubsystem::DispatchEvent(const TEvent& event)
    {
        std::get<Delegate<const TEvent&>>(listeners).Broadcast(event);
    }

    template <Eventable TEvent, class... TArgs>
    void EventSubsystem::DispatchEvent(TArgs&&... args)
    {
        const TEvent EVENT{std::forward<TArgs>(args)...};
        std::get<Delegate<const TEvent&>>(listeners).Broadcast(EVENT);
    }

    template <Eventable TEvent>
    EventSubsystem::ListenerId EventSubsystem::Subscribe(Action<const TEvent&> callback)
    {
        return std::get<Delegate<const TEvent&>>(listeners).AddListener(std::move(callback));
    }

    template <Eventable TEvent, class TObject>
    EventSubsystem::ListenerId EventSubsystem::Subscribe(TObject* object, void (TObject::*method)(const TEvent&))
    {
        return Subscribe<TEvent>(std::bind_front(method, object));
    }

    template <Eventable TEvent, class TObject>
    EventSubsystem::ListenerId EventSubsystem::Subscribe(TObject* object,
                                                         void (TObject::*method)(const TEvent&) noexcept)
    {
        return Subscribe<TEvent>(std::bind_front(method, object));
    }

    template <Eventable TEvent>
    void EventSubsystem::Unsubscribe(ListenerId id) noexcept
    {
        std::get<Delegate<const TEvent&>>(listeners).RemoveListener(id);
    }
} // namespace Garage

#endif // GARAGE_PLATFORM_EVENT_SUBSYSTEM_HPP
