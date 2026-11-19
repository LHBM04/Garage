#ifndef GARAGE_CORE_APP_SUBSYSTEM_HPP
#define GARAGE_CORE_APP_SUBSYSTEM_HPP

#include "defines.hpp"
#include "subsystem.hpp"
#include "system_status.hpp"

namespace Garage
{
    class SystemContext;

    class GARAGE_API AppSubsystemBase : public SubsystemBase
    {
    public:
        using SubsystemBase::SubsystemBase;

        [[nodiscard]] virtual ESystemStatus OnEarlyUpdate(SystemContext& context) = 0;
        [[nodiscard]] virtual ESystemStatus OnFixedUpdate(SystemContext& context) = 0;
        [[nodiscard]] virtual ESystemStatus OnUpdate(SystemContext& context)      = 0;
        [[nodiscard]] virtual ESystemStatus OnLateUpdate(SystemContext& context)  = 0;
    };
} // namespace Garage

#endif // GARAGE_CORE_APP_SUBSYSTEM_HPP
