#ifndef GARAGE_CORE_COMMON_DELEGATE_HPP
#define GARAGE_CORE_COMMON_DELEGATE_HPP

#include <algorithm>
#include <functional>
#include <memory_resource>
#include <type_traits>
#include <utility>
#include <vector>

#include "../memory.hpp"
#include "types.hpp"

namespace Garage
{
    template <class... TArgs>
    class Delegate final
    {
    public:
        using Listener   = std::function<void(TArgs...)>;
        using ListenerId = Int32;

    private:
        struct Subscription final
        {
            ListenerId id          = 0;
            Listener   callback    = {};
            bool       isConnected = true;
        };

        using SubscriptionPtr  = SharedPtr<Subscription>;
        using SubscriptionList = std::pmr::vector<SubscriptionPtr>;

    public:
        Delegate() noexcept  = default;
        ~Delegate() noexcept = default;

        Delegate(const Delegate&) noexcept            = delete;
        Delegate& operator=(const Delegate&) noexcept = delete;

        Delegate(Delegate&&) noexcept                                                                 = default;
        Delegate& operator=(Delegate&&) noexcept(std::is_nothrow_move_assignable_v<SubscriptionList>) = default;

        Delegate& operator+=(Listener callback) noexcept;

        ListenerId AddListener(Listener callback);
        void       RemoveListener(ListenerId id) noexcept;

        void Broadcast(TArgs... args) const;

        void Clear() noexcept;

    private:
        SubscriptionList subscriptions{Memory::GetMemoryResource()};
        ListenerId       nextListenerId = 1;
    };

    template <class... TArgs>
    Delegate<TArgs...>& Delegate<TArgs...>::operator+=(Listener callback) noexcept
    {
        AddListener(std::move(callback));
        return *this;
    }

    template <class... TArgs>
    typename Delegate<TArgs...>::ListenerId Delegate<TArgs...>::AddListener(Listener callback)
    {
        const ListenerId ID = nextListenerId++;
        subscriptions.push_back(MakeShared<Subscription>(Subscription{ID, std::move(callback), true}));
        return ID;
    }

    template <class... TArgs>
    void Delegate<TArgs...>::RemoveListener(ListenerId id) noexcept
    {
        const auto SUBSCRIPTION = std::ranges::find(subscriptions, id, &Subscription::id);
        if (SUBSCRIPTION != subscriptions.end())
        {
            (*SUBSCRIPTION)->isConnected = false;
            subscriptions.erase(SUBSCRIPTION);
        }
    }

    template <class... TArgs>
    void Delegate<TArgs...>::Broadcast(TArgs... args) const
    {
        const SubscriptionList SNAPSHOT(subscriptions, Memory::GetMemoryResource());
        for (const SubscriptionPtr& subscription : SNAPSHOT)
        {
            if (subscription->isConnected && subscription->callback)
            {
                subscription->callback(args...);
            }
        }
    }

    template <class... TArgs>
    void Delegate<TArgs...>::Clear() noexcept
    {
        for (const SubscriptionPtr& subscription : subscriptions)
        {
            subscription->isConnected = false;
        }
        subscriptions.clear();
    }
} // namespace Garage

#endif // GARAGE_CORE_COMMON_DELEGATE_HPP
