#include "garage/core/app.hpp"

#include "garage/core/app_subsystem.hpp"
#include "garage/core/system_status.hpp"

namespace Garage
{
    ESystemStatus App::Startup()
    {
        instance = this;
        return Base::Startup();
    }

    ESystemStatus App::Routine()
    {
        if (const ESystemStatus status = EarlyUpdate(); status != ESystemStatus::CONTINUE)
        {
            return status;
        }

        if (const ESystemStatus status = FixedUpdate(); status != ESystemStatus::CONTINUE)
        {
            return status;
        }

        if (const ESystemStatus status = Update(); status != ESystemStatus::CONTINUE)
        {
            return status;
        }

        if (const ESystemStatus status = LateUpdate(); status != ESystemStatus::CONTINUE)
        {
            return status;
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
            if (const ESystemStatus status = subsystem->OnEarlyUpdate(); status != ESystemStatus::CONTINUE)
            {
                return status;
            }
        }

        return ESystemStatus::CONTINUE;
    }

    ESystemStatus App::FixedUpdate()
    {
        for (AppSubsystemBase* subsystem : GetSubsystems())
        {
            if (const ESystemStatus status = subsystem->OnFixedUpdate(); status != ESystemStatus::CONTINUE)
            {
                return status;
            }
        }

        return ESystemStatus::CONTINUE;
    }

    ESystemStatus App::Update()
    {
        for (AppSubsystemBase* subsystem : GetSubsystems())
        {
            if (const ESystemStatus status = subsystem->OnUpdate(); status != ESystemStatus::CONTINUE)
            {
                return status;
            }
        }

        return ESystemStatus::CONTINUE;
    }

    ESystemStatus App::LateUpdate()
    {
        for (AppSubsystemBase* subsystem : GetSubsystems())
        {
            if (const ESystemStatus status = subsystem->OnLateUpdate(); status != ESystemStatus::CONTINUE)
            {
                return status;
            }
        }

        return ESystemStatus::CONTINUE;
    }
} // namespace Garage
