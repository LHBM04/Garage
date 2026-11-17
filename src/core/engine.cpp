#include "garage/core/engine.hpp"

#include "garage/core/app.hpp"
#include "garage/core/defines.hpp"
#include "garage/core/engine_subsystem.hpp"
#include "garage/core/system_status.hpp"
#include "garage/core/types.hpp"

namespace Garage
{
    Int32 Engine::Run(App* target)
    {
        ASSERT(target != nullptr);

        app = target;

        if (const ESystemStatus status = Startup(); status != ESystemStatus::CONTINUE)
        {
            Shutdown();
            return static_cast<Int32>(status);
        }

        if (const ESystemStatus status = app->Startup(); status != ESystemStatus::CONTINUE)
        {
            app->Shutdown();
            Shutdown();
            return static_cast<Int32>(status);
        }

        while (true)
        {
            if (const ESystemStatus status = Routine(); status != ESystemStatus::CONTINUE)
            {
                app->Shutdown();
                Shutdown();

                return static_cast<Int32>(status);
            }
        }
    }

    ESystemStatus Engine::Startup()
    {
        instance = this;
        return Base::Startup();
    }

    ESystemStatus Engine::Routine()
    {
        if (const ESystemStatus status = PreTick(); status != ESystemStatus::CONTINUE)
        {
            return status;
        }

        if (const ESystemStatus status = Tick(); status != ESystemStatus::CONTINUE)
        {
            return status;
        }

        if (const ESystemStatus status = app->Routine(); status != ESystemStatus::CONTINUE)
        {
            return status;
        }

        if (const ESystemStatus status = PostTick(); status != ESystemStatus::CONTINUE)
        {
            return status;
        }

        return ESystemStatus::CONTINUE;
    }

    void Engine::Shutdown()
    {
        Base::Shutdown();
        instance = nullptr;
    }

    ESystemStatus Engine::PreTick()
    {
        for (EngineSubsystemBase* subsystem : GetSubsystems())
        {
            if (const ESystemStatus status = subsystem->OnPreTick(); status != ESystemStatus::CONTINUE)
            {
                return status;
            }
        }

        return ESystemStatus::CONTINUE;
    }

    ESystemStatus Engine::Tick()
    {
        for (EngineSubsystemBase* subsystem : GetSubsystems())
        {
            if (const ESystemStatus status = subsystem->OnTick(); status != ESystemStatus::CONTINUE)
            {
                return status;
            }
        }

        return ESystemStatus::CONTINUE;
    }

    ESystemStatus Engine::PostTick()
    {
        for (EngineSubsystemBase* subsystem : GetSubsystems())
        {
            if (const ESystemStatus status = subsystem->OnPostTick(); status != ESystemStatus::CONTINUE)
            {
                return status;
            }
        }

        return ESystemStatus::CONTINUE;
    }
} // namespace Garage
