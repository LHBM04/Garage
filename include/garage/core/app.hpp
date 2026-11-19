#ifndef GARAGE_CORE_APP_HPP
#define GARAGE_CORE_APP_HPP

#include <concepts>
#include <utility>

#include "app_subsystem.hpp"
#include "defines.hpp"
#include "system.hpp"
#include "system_status.hpp"

namespace Garage
{
    class SystemContext;

    class GARAGE_API App : public SystemBase<AppSubsystemBase>
    {
    public:
        using Base       = SystemBase<AppSubsystemBase>;
        using ListenerId = typename Base::ListenerId;

        template <std::derived_from<AppSubsystemBase> TSubsystem>
        using SubsystemCallback = typename Base::template SubsystemCallback<TSubsystem>;

        template <std::derived_from<AppSubsystemBase> TSubsystem, class TObject>
        using SubsystemMethod = typename Base::template SubsystemMethod<TSubsystem, TObject>;

        [[nodiscard]] ESystemStatus Startup(SystemContext& context) override;
        [[nodiscard]] ESystemStatus Routine(SystemContext& context) override;
        void                        Shutdown(SystemContext& context) override;

        template <std::derived_from<AppSubsystemBase> TSubsystem>
        TSubsystem* AddSubsystem();

        template <std::derived_from<AppSubsystemBase> TSubsystem>
        void RemoveSubsystem(SystemContext& context);

        template <std::derived_from<AppSubsystemBase> TSubsystem>
        [[nodiscard]] TSubsystem* GetSubsystem() noexcept;

        template <std::derived_from<AppSubsystemBase> TSubsystem>
        [[nodiscard]] const TSubsystem* GetSubsystem() const noexcept;

        template <std::derived_from<AppSubsystemBase> TSubsystem>
        ListenerId OnSubsystemAdded(SubsystemCallback<TSubsystem> callback);

        template <std::derived_from<AppSubsystemBase> TSubsystem, class TObject>
        ListenerId OnSubsystemAdded(TObject* object, SubsystemMethod<TSubsystem, TObject> method);

        template <std::derived_from<AppSubsystemBase> TSubsystem>
        ListenerId OnSubsystemRemoved(SubsystemCallback<TSubsystem> callback);

        template <std::derived_from<AppSubsystemBase> TSubsystem, class TObject>
        ListenerId OnSubsystemRemoved(TObject* object, SubsystemMethod<TSubsystem, TObject> method);

    private:
        [[nodiscard]] ESystemStatus EarlyUpdate(SystemContext& context);
        [[nodiscard]] ESystemStatus FixedUpdate(SystemContext& context);
        [[nodiscard]] ESystemStatus Update(SystemContext& context);
        [[nodiscard]] ESystemStatus LateUpdate(SystemContext& context);
    };

    template <std::derived_from<AppSubsystemBase> TSubsystem>
    TSubsystem* App::AddSubsystem()
    {
        return Base::AddSubsystem<TSubsystem>();
    }

    template <std::derived_from<AppSubsystemBase> TSubsystem>
    void App::RemoveSubsystem(SystemContext& context)
    {
        Base::RemoveSubsystem<TSubsystem>(context);
    }

    template <std::derived_from<AppSubsystemBase> TSubsystem>
    TSubsystem* App::GetSubsystem() noexcept
    {
        return Base::GetSubsystem<TSubsystem>();
    }

    template <std::derived_from<AppSubsystemBase> TSubsystem>
    const TSubsystem* App::GetSubsystem() const noexcept
    {
        return Base::GetSubsystem<TSubsystem>();
    }

    template <std::derived_from<AppSubsystemBase> TSubsystem>
    App::ListenerId App::OnSubsystemAdded(SubsystemCallback<TSubsystem> callback)
    {
        return Base::OnSubsystemAdded<TSubsystem>(std::move(callback));
    }

    template <std::derived_from<AppSubsystemBase> TSubsystem, class TObject>
    App::ListenerId App::OnSubsystemAdded(TObject* object, SubsystemMethod<TSubsystem, TObject> method)
    {
        return Base::OnSubsystemAdded<TSubsystem>(object, method);
    }

    template <std::derived_from<AppSubsystemBase> TSubsystem>
    App::ListenerId App::OnSubsystemRemoved(SubsystemCallback<TSubsystem> callback)
    {
        return Base::OnSubsystemRemoved<TSubsystem>(std::move(callback));
    }

    template <std::derived_from<AppSubsystemBase> TSubsystem, class TObject>
    App::ListenerId App::OnSubsystemRemoved(TObject* object, SubsystemMethod<TSubsystem, TObject> method)
    {
        return Base::OnSubsystemRemoved<TSubsystem>(object, method);
    }
} // namespace Garage

#endif // GARAGE_CORE_APP_HPP
