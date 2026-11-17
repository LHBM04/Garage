#ifndef GARAGE_UTILITIES_DELEGATE_HPP
#define GARAGE_UTILITIES_DELEGATE_HPP

#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <utility>
#include <vector>

namespace Garage
{
    template <class... TArgs>
    class Delegate final
    {
    public:
        using Listener   = std::function<void(TArgs...)>;
        using ListenerId = std::int32_t;

        Delegate() noexcept  = default;
        ~Delegate() noexcept = default;

        Delegate(const Delegate&) noexcept            = delete;
        Delegate& operator=(const Delegate&) noexcept = delete;

        Delegate(Delegate&&) noexcept            = default;
        Delegate& operator=(Delegate&&) noexcept = default;

        Delegate& operator+=(Listener callback) noexcept;

        ListenerId AddListener(Listener callback);
        void       RemoveListener(ListenerId id) noexcept;

        void Broadcast(TArgs... args) const;

        void Clear() noexcept;

    private:
        struct Subscription final
        {
            ListenerId id          = 0;
            Listener   callback    = {};
            bool       isConnected = true;
        };

        using SubscriptionPtr  = std::shared_ptr<Subscription>;
        using SubscriptionList = std::vector<SubscriptionPtr>;

        SubscriptionList subscriptions  = {};
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
        subscriptions.push_back(std::make_shared<Subscription>(Subscription{ID, std::move(callback), true}));
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
        const SubscriptionList SNAPSHOT = subscriptions;
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

#endif // GARAGE_UTILITIES_DELEGATE_HPP
