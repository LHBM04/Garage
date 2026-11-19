#include "garage/core/app.hpp"

#include "garage/core/app_subsystem.hpp"
#include "garage/core/system_context.hpp"
#include "garage/core/system_status.hpp"

#include "garage/platform/time_subsystem.hpp"

namespace Garage
{
    ESystemStatus App::Startup(SystemContext& context)
    {
        return Base::Startup(context);
    }

    ESystemStatus App::Routine(SystemContext& context)
    {
        if (const ESystemStatus STATUS = EarlyUpdate(context); STATUS != ESystemStatus::CONTINUE)
        {
            return STATUS;
        }

        TimeSubsystem* timeSubsystem = context.GetEngineSubsystem<TimeSubsystem>();
        if (timeSubsystem != nullptr)
        {
            while (timeSubsystem->ShouldFixedStep())
            {
                if (const ESystemStatus STATUS = FixedUpdate(context); STATUS != ESystemStatus::CONTINUE)
                {
                    return STATUS;
                }
            }
        }
        else
        {
            if (const ESystemStatus STATUS = FixedUpdate(context); STATUS != ESystemStatus::CONTINUE)
            {
                return STATUS;
            }
        }

        if (const ESystemStatus STATUS = Update(context); STATUS != ESystemStatus::CONTINUE)
        {
            return STATUS;
        }

        if (const ESystemStatus STATUS = LateUpdate(context); STATUS != ESystemStatus::CONTINUE)
        {
            return STATUS;
        }

        return ESystemStatus::CONTINUE;
    }

    void App::Shutdown(SystemContext& context)
    {
        Base::Shutdown(context);
    }

    ESystemStatus App::EarlyUpdate(SystemContext& context)
    {
        for (AppSubsystemBase* subsystem : GetSubsystems())
        {
            if (const ESystemStatus STATUS = subsystem->OnEarlyUpdate(context); STATUS != ESystemStatus::CONTINUE)
            {
                return STATUS;
            }
        }

        return ESystemStatus::CONTINUE;
    }

    ESystemStatus App::FixedUpdate(SystemContext& context)
    {
        for (AppSubsystemBase* subsystem : GetSubsystems())
        {
            if (const ESystemStatus STATUS = subsystem->OnFixedUpdate(context); STATUS != ESystemStatus::CONTINUE)
            {
                return STATUS;
            }
        }

        return ESystemStatus::CONTINUE;
    }

    ESystemStatus App::Update(SystemContext& context)
    {
        for (AppSubsystemBase* subsystem : GetSubsystems())
        {
            if (const ESystemStatus STATUS = subsystem->OnUpdate(context); STATUS != ESystemStatus::CONTINUE)
            {
                return STATUS;
            }
        }

        return ESystemStatus::CONTINUE;
    }

    ESystemStatus App::LateUpdate(SystemContext& context)
    {
        for (AppSubsystemBase* subsystem : GetSubsystems())
        {
            if (const ESystemStatus STATUS = subsystem->OnLateUpdate(context); STATUS != ESystemStatus::CONTINUE)
            {
                return STATUS;
            }
        }

        return ESystemStatus::CONTINUE;
    }
} // namespace Garage
