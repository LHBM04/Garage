#include "garage/core/app.hpp"

#include "garage/core/app_subsystem.hpp"
#include "garage/core/engine.hpp"
#include "garage/core/system_status.hpp"

#include "garage/platform/time_subsystem.hpp"

namespace Garage
{
    ESystemStatus App::Startup()
    {
        instance = this;
        return Base::Startup();
    }

    ESystemStatus App::Routine()
    {
        if (const ESystemStatus STATUS = EarlyUpdate(); STATUS != ESystemStatus::CONTINUE)
        {
            return STATUS;
        }

        Engine*        engine        = Engine::GetInstance();
        TimeSubsystem* timeSubsystem = (engine != nullptr) ? engine->GetSubsystem<TimeSubsystem>() : nullptr;
        if (timeSubsystem != nullptr)
        {
            while (timeSubsystem->ShouldFixedStep())
            {
                if (const ESystemStatus STATUS = FixedUpdate(); STATUS != ESystemStatus::CONTINUE)
                {
                    return STATUS;
                }
            }
        }
        else
        {
            if (const ESystemStatus STATUS = FixedUpdate(); STATUS != ESystemStatus::CONTINUE)
            {
                return STATUS;
            }
        }

        if (const ESystemStatus STATUS = Update(); STATUS != ESystemStatus::CONTINUE)
        {
            return STATUS;
        }

        if (const ESystemStatus STATUS = LateUpdate(); STATUS != ESystemStatus::CONTINUE)
        {
            return STATUS;
        }

        return ESystemStatus::CONTINUE;
    }

    void App::Shutdown()
    {
        Base::Shutdown();
        instance = nullptr;
    }

    ESystemStatus App::EarlyUpdate()
    {
        for (AppSubsystemBase* subsystem : GetSubsystems())
        {
            if (const ESystemStatus STATUS = subsystem->OnEarlyUpdate(); STATUS != ESystemStatus::CONTINUE)
            {
                return STATUS;
            }
        }

        return ESystemStatus::CONTINUE;
    }

    ESystemStatus App::FixedUpdate()
    {
        for (AppSubsystemBase* subsystem : GetSubsystems())
        {
            if (const ESystemStatus STATUS = subsystem->OnFixedUpdate(); STATUS != ESystemStatus::CONTINUE)
            {
                return STATUS;
            }
        }

        return ESystemStatus::CONTINUE;
    }

    ESystemStatus App::Update()
    {
        for (AppSubsystemBase* subsystem : GetSubsystems())
        {
            if (const ESystemStatus STATUS = subsystem->OnUpdate(); STATUS != ESystemStatus::CONTINUE)
            {
                return STATUS;
            }
        }

        return ESystemStatus::CONTINUE;
    }

    ESystemStatus App::LateUpdate()
    {
        for (AppSubsystemBase* subsystem : GetSubsystems())
        {
            if (const ESystemStatus STATUS = subsystem->OnLateUpdate(); STATUS != ESystemStatus::CONTINUE)
            {
                return STATUS;
            }
        }

        return ESystemStatus::CONTINUE;
    }
} // namespace Garage
