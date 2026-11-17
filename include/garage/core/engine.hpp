#ifndef GARAGE_CORE_ENGINE_HPP
#define GARAGE_CORE_ENGINE_HPP

#include <concepts>
#include <utility>

#include "defines.hpp"
#include "engine_subsystem.hpp"
#include "system.hpp"
#include "system_status.hpp"
#include "types.hpp"

namespace Garage
{
    class App;

    class GARAGE_API Engine : public SystemBase<EngineSubsystemBase>
    {
    public:
        using Base       = SystemBase<EngineSubsystemBase>;
        using ListenerId = typename Base::ListenerId;

        template <std::derived_from<EngineSubsystemBase> TSubsystem>
        using SubsystemCallback = typename Base::template SubsystemCallback<TSubsystem>;

        template <std::derived_from<EngineSubsystemBase> TSubsystem, class TObject>
        using SubsystemMethod = typename Base::template SubsystemMethod<TSubsystem, TObject>;

        [[nodiscard]] ESystemStatus Startup() override;
        [[nodiscard]] ESystemStatus Routine() override;
        void                        Shutdown() override;

        template <std::derived_from<EngineSubsystemBase> TSubsystem>
        TSubsystem* AddSubsystem();

        template <std::derived_from<EngineSubsystemBase> TSubsystem>
        void RemoveSubsystem();

        template <std::derived_from<EngineSubsystemBase> TSubsystem>
        [[nodiscard]] TSubsystem* GetSubsystem() noexcept;

        template <std::derived_from<EngineSubsystemBase> TSubsystem>
        [[nodiscard]] const TSubsystem* GetSubsystem() const noexcept;

        template <std::derived_from<EngineSubsystemBase> TSubsystem>
        ListenerId OnSubsystemAdded(SubsystemCallback<TSubsystem> callback);

        template <std::derived_from<EngineSubsystemBase> TSubsystem, class TObject>
        ListenerId OnSubsystemAdded(TObject* object, SubsystemMethod<TSubsystem, TObject> method);

        template <std::derived_from<EngineSubsystemBase> TSubsystem>
        ListenerId OnSubsystemRemoved(SubsystemCallback<TSubsystem> callback);

        template <std::derived_from<EngineSubsystemBase> TSubsystem, class TObject>
        ListenerId OnSubsystemRemoved(TObject* object, SubsystemMethod<TSubsystem, TObject> method);

        [[nodiscard]] Int32 Run(App* target);

        [[nodiscard]] static Engine* GetInstance() noexcept;

    private:
        [[nodiscard]] ESystemStatus PreTick();
        [[nodiscard]] ESystemStatus Tick();
        [[nodiscard]] ESystemStatus PostTick();

        inline static Engine* instance = nullptr;

        App* app = nullptr;
    };

    template <std::derived_from<EngineSubsystemBase> TSubsystem>
    TSubsystem* Engine::AddSubsystem()
    {
        return Base::AddSubsystem<TSubsystem>();
    }

    template <std::derived_from<EngineSubsystemBase> TSubsystem>
    void Engine::RemoveSubsystem()
    {
        Base::RemoveSubsystem<TSubsystem>();
    }

    template <std::derived_from<EngineSubsystemBase> TSubsystem>
    TSubsystem* Engine::GetSubsystem() noexcept
    {
        return Base::GetSubsystem<TSubsystem>();
    }

    template <std::derived_from<EngineSubsystemBase> TSubsystem>
    const TSubsystem* Engine::GetSubsystem() const noexcept
    {
        return Base::GetSubsystem<TSubsystem>();
    }

    template <std::derived_from<EngineSubsystemBase> TSubsystem>
    Engine::ListenerId Engine::OnSubsystemAdded(SubsystemCallback<TSubsystem> callback)
    {
        return Base::OnSubsystemAdded<TSubsystem>(std::move(callback));
    }

    template <std::derived_from<EngineSubsystemBase> TSubsystem, class TObject>
    Engine::ListenerId Engine::OnSubsystemAdded(TObject* object, SubsystemMethod<TSubsystem, TObject> method)
    {
        return Base::OnSubsystemAdded<TSubsystem>(object, method);
    }

    template <std::derived_from<EngineSubsystemBase> TSubsystem>
    Engine::ListenerId Engine::OnSubsystemRemoved(SubsystemCallback<TSubsystem> callback)
    {
        return Base::OnSubsystemRemoved<TSubsystem>(std::move(callback));
    }

    template <std::derived_from<EngineSubsystemBase> TSubsystem, class TObject>
    Engine::ListenerId Engine::OnSubsystemRemoved(TObject* object, SubsystemMethod<TSubsystem, TObject> method)
    {
        return Base::OnSubsystemRemoved<TSubsystem>(object, method);
    }

    inline Engine* Engine::GetInstance() noexcept
    {
        ASSERT(instance != nullptr);

        return instance;
    }
} // namespace Garage

#endif // GARAGE_CORE_ENGINE_HPP
