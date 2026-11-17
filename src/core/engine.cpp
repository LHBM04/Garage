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

        if (const ESystemStatus STATUS = Startup(); STATUS != ESystemStatus::CONTINUE)
        {
            Shutdown();
            return static_cast<Int32>(STATUS);
        }

        if (const ESystemStatus STATUS = app->Startup(); STATUS != ESystemStatus::CONTINUE)
        {
            app->Shutdown();
            Shutdown();
            return static_cast<Int32>(STATUS);
        }

        while (true)
        {
            if (const ESystemStatus STATUS = Routine(); STATUS != ESystemStatus::CONTINUE)
            {
                app->Shutdown();
                Shutdown();

                return static_cast<Int32>(STATUS);
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
        if (const ESystemStatus STATUS = PreTick(); STATUS != ESystemStatus::CONTINUE)
        {
            return STATUS;
        }

        if (const ESystemStatus STATUS = Tick(); STATUS != ESystemStatus::CONTINUE)
        {
            return STATUS;
        }

        if (const ESystemStatus STATUS = app->Routine(); STATUS != ESystemStatus::CONTINUE)
        {
            return STATUS;
        }

        if (const ESystemStatus STATUS = PostTick(); STATUS != ESystemStatus::CONTINUE)
        {
            return STATUS;
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
            if (const ESystemStatus STATUS = subsystem->OnPreTick(); STATUS != ESystemStatus::CONTINUE)
            {
                return STATUS;
            }
        }

        return ESystemStatus::CONTINUE;
    }

    ESystemStatus Engine::Tick()
    {
        for (EngineSubsystemBase* subsystem : GetSubsystems())
        {
            if (const ESystemStatus STATUS = subsystem->OnTick(); STATUS != ESystemStatus::CONTINUE)
            {
                return STATUS;
            }
        }

        return ESystemStatus::CONTINUE;
    }

    ESystemStatus Engine::PostTick()
    {
        for (EngineSubsystemBase* subsystem : GetSubsystems())
        {
            if (const ESystemStatus STATUS = subsystem->OnPostTick(); STATUS != ESystemStatus::CONTINUE)
            {
                return STATUS;
            }
        }

        return ESystemStatus::CONTINUE;
    }
} // namespace Garage
