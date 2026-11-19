#include "garage/core/engine.hpp"

#include "garage/core/app.hpp"
#include "garage/core/defines.hpp"
#include "garage/core/engine_subsystem.hpp"
#include "garage/core/system_context.hpp"
#include "garage/core/system_status.hpp"
#include "garage/core/types.hpp"

namespace Garage
{
    Int32 Engine::Run(App* target)
    {
        ASSERT(target != nullptr);

        app = target;
        SystemContext context(this, app);

        if (const ESystemStatus STATUS = Startup(context); STATUS != ESystemStatus::CONTINUE)
        {
            Shutdown(context);
            app = nullptr;
            return static_cast<Int32>(STATUS);
        }

        if (const ESystemStatus STATUS = app->Startup(context); STATUS != ESystemStatus::CONTINUE)
        {
            app->Shutdown(context);
            Shutdown(context);
            app = nullptr;
            return static_cast<Int32>(STATUS);
        }

        while (true)
        {
            if (const ESystemStatus STATUS = Routine(context); STATUS != ESystemStatus::CONTINUE)
            {
                app->Shutdown(context);
                Shutdown(context);
                app = nullptr;

                return static_cast<Int32>(STATUS);
            }
        }
    }

    ESystemStatus Engine::Startup(SystemContext& context)
    {
        return Base::Startup(context);
    }

    ESystemStatus Engine::Routine(SystemContext& context)
    {
        if (const ESystemStatus STATUS = PreTick(context); STATUS != ESystemStatus::CONTINUE)
        {
            return STATUS;
        }

        if (const ESystemStatus STATUS = Tick(context); STATUS != ESystemStatus::CONTINUE)
        {
            return STATUS;
        }

        if (const ESystemStatus STATUS = app->Routine(context); STATUS != ESystemStatus::CONTINUE)
        {
            return STATUS;
        }

        if (const ESystemStatus STATUS = PostTick(context); STATUS != ESystemStatus::CONTINUE)
        {
            return STATUS;
        }

        return ESystemStatus::CONTINUE;
    }

    void Engine::Shutdown(SystemContext& context)
    {
        Base::Shutdown(context);
    }

    ESystemStatus Engine::PreTick(SystemContext& context)
    {
        for (EngineSubsystemBase* subsystem : GetSubsystems())
        {
            if (const ESystemStatus STATUS = subsystem->OnPreTick(context); STATUS != ESystemStatus::CONTINUE)
            {
                return STATUS;
            }
        }

        return ESystemStatus::CONTINUE;
    }

    ESystemStatus Engine::Tick(SystemContext& context)
    {
        for (EngineSubsystemBase* subsystem : GetSubsystems())
        {
            if (const ESystemStatus STATUS = subsystem->OnTick(context); STATUS != ESystemStatus::CONTINUE)
            {
                return STATUS;
            }
        }

        return ESystemStatus::CONTINUE;
    }

    ESystemStatus Engine::PostTick(SystemContext& context)
    {
        for (EngineSubsystemBase* subsystem : GetSubsystems())
        {
            if (const ESystemStatus STATUS = subsystem->OnPostTick(context); STATUS != ESystemStatus::CONTINUE)
            {
                return STATUS;
            }
        }

        return ESystemStatus::CONTINUE;
    }
} // namespace Garage
