#ifndef GARAGE_CORE_SYSTEM_HPP
#define GARAGE_CORE_SYSTEM_HPP

#include <algorithm>
#include <concepts>
#include <functional>
#include <memory>
#include <ranges>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../utilities/delegate.hpp"

#include "subsystem.hpp"
#include "system_status.hpp"
#include "types.hpp"

namespace Garage
{
    template <std::derived_from<SubsystemBase> TSubsystem>
    class SystemBase
    {
    public:
        using SubsystemPtr      = TSubsystem*;
        using ConstSubsystemPtr = const TSubsystem*;
        using SubsystemOwner    = std::unique_ptr<TSubsystem>;
        using SubsystemList     = std::vector<SubsystemOwner>;
        using SubsystemMap      = std::unordered_map<std::type_index, SubsystemPtr>;
        using SubsystemDelegate = Delegate<SubsystemPtr>;
        using ListenerId        = typename SubsystemDelegate::ListenerId;
        using DelegateMap       = std::unordered_map<std::type_index, SubsystemDelegate>;

        template <std::derived_from<TSubsystem> TResult>
        using SubsystemCallback = Action<TResult*>;

        template <std::derived_from<TSubsystem> TResult, class TObject>
        using SubsystemMethod = void (TObject::*)(TResult*);

        SystemBase() noexcept          = default;
        virtual ~SystemBase() noexcept = default;

        SystemBase(const SystemBase&)            = delete;
        SystemBase& operator=(const SystemBase&) = delete;

        SystemBase(SystemBase&&)            = delete;
        SystemBase& operator=(SystemBase&&) = delete;

        [[nodiscard]] virtual ESystemStatus Startup();
        [[nodiscard]] virtual ESystemStatus Routine();
        virtual void                        Shutdown();

    protected:
        template <std::derived_from<TSubsystem> TResult>
        TResult* AddSubsystem();

        template <std::derived_from<TSubsystem> TResult>
        void RemoveSubsystem();

        template <std::derived_from<TSubsystem> TResult>
        [[nodiscard]] TResult* GetSubsystem() noexcept;

        template <std::derived_from<TSubsystem> TResult>
        [[nodiscard]] const TResult* GetSubsystem() const noexcept;

        [[nodiscard]] auto GetSubsystems() noexcept;
        [[nodiscard]] auto GetSubsystems() const noexcept;

        template <std::derived_from<TSubsystem> TResult>
        ListenerId OnSubsystemAdded(SubsystemCallback<TResult> callback);

        template <std::derived_from<TSubsystem> TResult, class TObject>
        ListenerId OnSubsystemAdded(TObject* object, SubsystemMethod<TResult, TObject> method);

        template <std::derived_from<TSubsystem> TResult>
        ListenerId OnSubsystemRemoved(SubsystemCallback<TResult> callback);

        template <std::derived_from<TSubsystem> TResult, class TObject>
        ListenerId OnSubsystemRemoved(TObject* object, SubsystemMethod<TResult, TObject> method);

    private:
        SubsystemList subsystems         = {};
        SubsystemMap  subsystemsByType   = {};
        DelegateMap   onSubsystemAdded   = {};
        DelegateMap   onSubsystemRemoved = {};
    };

    template <std::derived_from<SubsystemBase> TSubsystem>
    ESystemStatus SystemBase<TSubsystem>::Startup()
    {
        std::ranges::sort(subsystems, std::ranges::less{}, &SubsystemBase::GetPriority);

        for (const SubsystemOwner& subsystem : subsystems)
        {
            if (subsystem->OnStartup() == ESystemStatus::FAILURE)
            {
                return ESystemStatus::FAILURE;
            }
        }

        return ESystemStatus::CONTINUE;
    }

    template <std::derived_from<SubsystemBase> TSubsystem>
    ESystemStatus SystemBase<TSubsystem>::Routine()
    {
        return ESystemStatus::CONTINUE;
    }

    template <std::derived_from<SubsystemBase> TSubsystem>
    void SystemBase<TSubsystem>::Shutdown()
    {
        for (const SubsystemOwner& subsystem : subsystems | std::views::reverse)
        {
            SubsystemPtr          pointer = subsystem.get();
            const std::type_index type(typeid(*pointer));
            if (const auto result = onSubsystemRemoved.find(type); result != onSubsystemRemoved.end())
            {
                result->second.Broadcast(pointer);
            }
            pointer->OnShutdown();
        }

        for (auto& [type, delegate] : onSubsystemAdded)
        {
            delegate.Clear();
        }
        for (auto& [type, delegate] : onSubsystemRemoved)
        {
            delegate.Clear();
        }
        onSubsystemAdded.clear();
        onSubsystemRemoved.clear();
        subsystems.clear();
        subsystemsByType.clear();
    }

    template <std::derived_from<SubsystemBase> TSubsystem>
    template <std::derived_from<TSubsystem> TResult>
    TResult* SystemBase<TSubsystem>::AddSubsystem()
    {
        const std::type_index type(typeid(TResult));
        const auto            result = subsystemsByType.find(type);
        if (result != subsystemsByType.end())
        {
            return static_cast<TResult*>(result->second);
        }

        auto  subsystem = std::make_unique<TResult>();
        auto* pointer   = subsystem.get();
        subsystems.push_back(std::move(subsystem));
        subsystemsByType.emplace(type, pointer);

        if (const auto found = onSubsystemAdded.find(type); found != onSubsystemAdded.end())
        {
            found->second.Broadcast(pointer);
        }
        return pointer;
    }

    template <std::derived_from<SubsystemBase> TSubsystem>
    template <std::derived_from<TSubsystem> TResult>
    void SystemBase<TSubsystem>::RemoveSubsystem()
    {
        const std::type_index type(typeid(TResult));
        const auto            result = subsystemsByType.find(type);
        if (result == subsystemsByType.end())
        {
            return;
        }

        SubsystemPtr pointer = result->second;
        if (const auto found = onSubsystemRemoved.find(type); found != onSubsystemRemoved.end())
        {
            found->second.Broadcast(pointer);
        }
        pointer->OnShutdown();

        subsystemsByType.erase(result);
        std::erase_if(subsystems,
                      [pointer](const SubsystemOwner& subsystem)
                      {
                          return subsystem.get() == pointer;
                      });
    }

    template <std::derived_from<SubsystemBase> TSubsystem>
    template <std::derived_from<TSubsystem> TResult>
    TResult* SystemBase<TSubsystem>::GetSubsystem() noexcept
    {
        const auto result = subsystemsByType.find(std::type_index(typeid(TResult)));
        if (result != subsystemsByType.end())
        {
            return static_cast<TResult*>(result->second);
        }

        return nullptr;
    }

    template <std::derived_from<SubsystemBase> TSubsystem>
    template <std::derived_from<TSubsystem> TResult>
    const TResult* SystemBase<TSubsystem>::GetSubsystem() const noexcept
    {
        const auto result = subsystemsByType.find(std::type_index(typeid(TResult)));
        if (result != subsystemsByType.end())
        {
            return static_cast<const TResult*>(result->second);
        }

        return nullptr;
    }

    template <std::derived_from<SubsystemBase> TSubsystem>
    auto SystemBase<TSubsystem>::GetSubsystems() noexcept
    {
        return subsystems |
                std::views::transform(
                        [](const SubsystemOwner& subsystem) -> SubsystemPtr
                        {
                            return subsystem.get();
                        });
    }

    template <std::derived_from<SubsystemBase> TSubsystem>
    auto SystemBase<TSubsystem>::GetSubsystems() const noexcept
    {
        return subsystems |
                std::views::transform(
                        [](const SubsystemOwner& subsystem) -> ConstSubsystemPtr
                        {
                            return subsystem.get();
                        });
    }

    template <std::derived_from<SubsystemBase> TSubsystem>
    template <std::derived_from<TSubsystem> TResult>
    typename SystemBase<TSubsystem>::ListenerId
            SystemBase<TSubsystem>::OnSubsystemAdded(SubsystemCallback<TResult> callback)
    {
        return onSubsystemAdded[std::type_index(typeid(TResult))].AddListener(
                [callback = std::move(callback)](SubsystemPtr subsystem)
                {
                    if (callback)
                    {
                        callback(static_cast<TResult*>(subsystem));
                    }
                });
    }

    template <std::derived_from<SubsystemBase> TSubsystem>
    template <std::derived_from<TSubsystem> TResult, class TObject>
    typename SystemBase<TSubsystem>::ListenerId
            SystemBase<TSubsystem>::OnSubsystemAdded(TObject* object, SubsystemMethod<TResult, TObject> method)
    {
        return OnSubsystemAdded<TResult>(std::bind_front(method, object));
    }

    template <std::derived_from<SubsystemBase> TSubsystem>
    template <std::derived_from<TSubsystem> TResult>
    typename SystemBase<TSubsystem>::ListenerId
            SystemBase<TSubsystem>::OnSubsystemRemoved(SubsystemCallback<TResult> callback)
    {
        return onSubsystemRemoved[std::type_index(typeid(TResult))].AddListener(
                [callback = std::move(callback)](SubsystemPtr subsystem)
                {
                    if (callback)
                    {
                        callback(static_cast<TResult*>(subsystem));
                    }
                });
    }

    template <std::derived_from<SubsystemBase> TSubsystem>
    template <std::derived_from<TSubsystem> TResult, class TObject>
    typename SystemBase<TSubsystem>::ListenerId
            SystemBase<TSubsystem>::OnSubsystemRemoved(TObject* object, SubsystemMethod<TResult, TObject> method)
    {
        return OnSubsystemRemoved<TResult>(std::bind_front(method, object));
    }
} // namespace Garage

#endif // GARAGE_CORE_SYSTEM_HPP
