#ifndef GARAGE_CORE_SYSTEM_CONTEXT_HPP
#define GARAGE_CORE_SYSTEM_CONTEXT_HPP

#include <concepts>
#include <utility>

#include "app.hpp"
#include "app_subsystem.hpp"
#include "common.hpp"
#include "engine.hpp"
#include "engine_subsystem.hpp"

namespace Garage
{
    class SystemContext
    {
    public:
        using EngineListenerId = Engine::ListenerId;
        using AppListenerId    = App::ListenerId;

        template <std::derived_from<EngineSubsystemBase> TSubsystem>
        using EngineSubsystemCallback = Engine::SubsystemCallback<TSubsystem>;

        template <std::derived_from<EngineSubsystemBase> TSubsystem, class TObject>
        using EngineSubsystemMethod = Engine::SubsystemMethod<TSubsystem, TObject>;

        template <std::derived_from<AppSubsystemBase> TSubsystem>
        using AppSubsystemCallback = App::SubsystemCallback<TSubsystem>;

        template <std::derived_from<AppSubsystemBase> TSubsystem, class TObject>
        using AppSubsystemMethod = App::SubsystemMethod<TSubsystem, TObject>;

        explicit SystemContext(Engine* engine, App* app) noexcept;
        ~SystemContext() noexcept = default;

        SystemContext(const SystemContext&) noexcept            = delete;
        SystemContext& operator=(const SystemContext&) noexcept = delete;

        SystemContext(SystemContext&&) noexcept            = delete;
        SystemContext& operator=(SystemContext&&) noexcept = delete;

        template <std::derived_from<EngineSubsystemBase> TSubsystem>
        TSubsystem* AddEngineSubsystem();

        template <std::derived_from<EngineSubsystemBase> TSubsystem>
        void RemoveEngineSubsystem();

        template <std::derived_from<EngineSubsystemBase> TSubsystem>
        [[nodiscard]] TSubsystem* GetEngineSubsystem() noexcept;

        template <std::derived_from<EngineSubsystemBase> TSubsystem>
        [[nodiscard]] const TSubsystem* GetEngineSubsystem() const noexcept;

        template <std::derived_from<EngineSubsystemBase> TSubsystem>
        EngineListenerId OnEngineSubsystemAdded(EngineSubsystemCallback<TSubsystem> callback);

        template <std::derived_from<EngineSubsystemBase> TSubsystem, class TObject>
        EngineListenerId OnEngineSubsystemAdded(TObject* object, EngineSubsystemMethod<TSubsystem, TObject> method);

        template <std::derived_from<EngineSubsystemBase> TSubsystem>
        EngineListenerId OnEngineSubsystemRemoved(EngineSubsystemCallback<TSubsystem> callback);

        template <std::derived_from<EngineSubsystemBase> TSubsystem, class TObject>
        EngineListenerId OnEngineSubsystemRemoved(TObject* object, EngineSubsystemMethod<TSubsystem, TObject> method);

        template <std::derived_from<AppSubsystemBase> TSubsystem>
        TSubsystem* AddAppSubsystem();

        template <std::derived_from<AppSubsystemBase> TSubsystem>
        void RemoveAppSubsystem();

        template <std::derived_from<AppSubsystemBase> TSubsystem>
        [[nodiscard]] TSubsystem* GetAppSubsystem() noexcept;

        template <std::derived_from<AppSubsystemBase> TSubsystem>
        [[nodiscard]] const TSubsystem* GetAppSubsystem() const noexcept;

        template <std::derived_from<AppSubsystemBase> TSubsystem>
        AppListenerId OnAppSubsystemAdded(AppSubsystemCallback<TSubsystem> callback);

        template <std::derived_from<AppSubsystemBase> TSubsystem, class TObject>
        AppListenerId OnAppSubsystemAdded(TObject* object, AppSubsystemMethod<TSubsystem, TObject> method);

        template <std::derived_from<AppSubsystemBase> TSubsystem>
        AppListenerId OnAppSubsystemRemoved(AppSubsystemCallback<TSubsystem> callback);

        template <std::derived_from<AppSubsystemBase> TSubsystem, class TObject>
        AppListenerId OnAppSubsystemRemoved(TObject* object, AppSubsystemMethod<TSubsystem, TObject> method);

    private:
        Engine* engine = nullptr;
        App*    app    = nullptr;
    };

    inline SystemContext::SystemContext(Engine* engine, App* app) noexcept
        : engine(engine)
        , app(app)
    {
        ASSERT(engine != nullptr);
        ASSERT(app != nullptr);
    }

    template <std::derived_from<EngineSubsystemBase> TSubsystem>
    TSubsystem* SystemContext::AddEngineSubsystem()
    {
        return (engine != nullptr) ? engine->AddSubsystem<TSubsystem>() : nullptr;
    }

    template <std::derived_from<EngineSubsystemBase> TSubsystem>
    void SystemContext::RemoveEngineSubsystem()
    {
        if (engine != nullptr)
        {
            engine->RemoveSubsystem<TSubsystem>(*this);
        }
    }

    template <std::derived_from<EngineSubsystemBase> TSubsystem>
    TSubsystem* SystemContext::GetEngineSubsystem() noexcept
    {
        return (engine != nullptr) ? engine->GetSubsystem<TSubsystem>() : nullptr;
    }

    template <std::derived_from<EngineSubsystemBase> TSubsystem>
    const TSubsystem* SystemContext::GetEngineSubsystem() const noexcept
    {
        return (engine != nullptr) ? static_cast<const Engine*>(engine)->GetSubsystem<TSubsystem>() : nullptr;
    }

    template <std::derived_from<EngineSubsystemBase> TSubsystem>
    SystemContext::EngineListenerId SystemContext::OnEngineSubsystemAdded(EngineSubsystemCallback<TSubsystem> callback)
    {
        return (engine != nullptr) ? engine->OnSubsystemAdded<TSubsystem>(std::move(callback)) : 0;
    }

    template <std::derived_from<EngineSubsystemBase> TSubsystem, class TObject>
    SystemContext::EngineListenerId
            SystemContext::OnEngineSubsystemAdded(TObject* object, EngineSubsystemMethod<TSubsystem, TObject> method)
    {
        return (engine != nullptr) ? engine->OnSubsystemAdded<TSubsystem>(object, method) : 0;
    }

    template <std::derived_from<EngineSubsystemBase> TSubsystem>
    SystemContext::EngineListenerId
            SystemContext::OnEngineSubsystemRemoved(EngineSubsystemCallback<TSubsystem> callback)
    {
        return (engine != nullptr) ? engine->OnSubsystemRemoved<TSubsystem>(std::move(callback)) : 0;
    }

    template <std::derived_from<EngineSubsystemBase> TSubsystem, class TObject>
    SystemContext::EngineListenerId
            SystemContext::OnEngineSubsystemRemoved(TObject* object, EngineSubsystemMethod<TSubsystem, TObject> method)
    {
        return (engine != nullptr) ? engine->OnSubsystemRemoved<TSubsystem>(object, method) : 0;
    }

    template <std::derived_from<AppSubsystemBase> TSubsystem>
    TSubsystem* SystemContext::AddAppSubsystem()
    {
        return (app != nullptr) ? app->AddSubsystem<TSubsystem>() : nullptr;
    }

    template <std::derived_from<AppSubsystemBase> TSubsystem>
    void SystemContext::RemoveAppSubsystem()
    {
        if (app != nullptr)
        {
            app->RemoveSubsystem<TSubsystem>(*this);
        }
    }

    template <std::derived_from<AppSubsystemBase> TSubsystem>
    TSubsystem* SystemContext::GetAppSubsystem() noexcept
    {
        return (app != nullptr) ? app->GetSubsystem<TSubsystem>() : nullptr;
    }

    template <std::derived_from<AppSubsystemBase> TSubsystem>
    const TSubsystem* SystemContext::GetAppSubsystem() const noexcept
    {
        return (app != nullptr) ? static_cast<const App*>(app)->GetSubsystem<TSubsystem>() : nullptr;
    }

    template <std::derived_from<AppSubsystemBase> TSubsystem>
    SystemContext::AppListenerId SystemContext::OnAppSubsystemAdded(AppSubsystemCallback<TSubsystem> callback)
    {
        return (app != nullptr) ? app->OnSubsystemAdded<TSubsystem>(std::move(callback)) : 0;
    }

    template <std::derived_from<AppSubsystemBase> TSubsystem, class TObject>
    SystemContext::AppListenerId SystemContext::OnAppSubsystemAdded(TObject*                                object,
                                                                    AppSubsystemMethod<TSubsystem, TObject> method)
    {
        return (app != nullptr) ? app->OnSubsystemAdded<TSubsystem>(object, method) : 0;
    }

    template <std::derived_from<AppSubsystemBase> TSubsystem>
    SystemContext::AppListenerId SystemContext::OnAppSubsystemRemoved(AppSubsystemCallback<TSubsystem> callback)
    {
        return (app != nullptr) ? app->OnSubsystemRemoved<TSubsystem>(std::move(callback)) : 0;
    }

    template <std::derived_from<AppSubsystemBase> TSubsystem, class TObject>
    SystemContext::AppListenerId SystemContext::OnAppSubsystemRemoved(TObject*                                object,
                                                                      AppSubsystemMethod<TSubsystem, TObject> method)
    {
        return (app != nullptr) ? app->OnSubsystemRemoved<TSubsystem>(object, method) : 0;
    }
} // namespace Garage

#endif // GARAGE_CORE_SYSTEM_CONTEXT_HPP
