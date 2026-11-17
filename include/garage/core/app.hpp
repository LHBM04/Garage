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
    class GARAGE_API App : public SystemBase<AppSubsystemBase>
    {
    public:
        using Base       = SystemBase<AppSubsystemBase>;
        using ListenerId = typename Base::ListenerId;

        template <std::derived_from<AppSubsystemBase> TSubsystem>
        using SubsystemCallback = typename Base::template SubsystemCallback<TSubsystem>;

        template <std::derived_from<AppSubsystemBase> TSubsystem, class TObject>
        using SubsystemMethod = typename Base::template SubsystemMethod<TSubsystem, TObject>;

        [[nodiscard]] ESystemStatus Startup() override;
        [[nodiscard]] ESystemStatus Routine() override;
        void                        Shutdown() override;

        template <std::derived_from<AppSubsystemBase> TSubsystem>
        TSubsystem* AddSubsystem();

        template <std::derived_from<AppSubsystemBase> TSubsystem>
        void RemoveSubsystem();

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

        [[nodiscard]] static App* GetInstance() noexcept;

    private:
        [[nodiscard]] ESystemStatus EarlyUpdate();
        [[nodiscard]] ESystemStatus FixedUpdate();
        [[nodiscard]] ESystemStatus Update();
        [[nodiscard]] ESystemStatus LateUpdate();

        inline static App* instance = nullptr;
    };

    template <std::derived_from<AppSubsystemBase> TSubsystem>
    TSubsystem* App::AddSubsystem()
    {
        return Base::AddSubsystem<TSubsystem>();
    }

    template <std::derived_from<AppSubsystemBase> TSubsystem>
    void App::RemoveSubsystem()
    {
        Base::RemoveSubsystem<TSubsystem>();
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

    inline App* App::GetInstance() noexcept
    {
        ASSERT(instance != nullptr);

        return instance;
    }
} // namespace Garage

#endif // GARAGE_CORE_APP_HPP
