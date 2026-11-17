#ifndef GARAGE_CORE_APP_SUBSYSTEM_HPP
#define GARAGE_CORE_APP_SUBSYSTEM_HPP

#include "defines.hpp"
#include "subsystem.hpp"
#include "system_status.hpp"

namespace Garage
{
    class GARAGE_API AppSubsystemBase : public SubsystemBase
    {
    public:
        using SubsystemBase::SubsystemBase;

        [[nodiscard]] virtual ESystemStatus OnEarlyUpdate() = 0;
        [[nodiscard]] virtual ESystemStatus OnFixedUpdate() = 0;
        [[nodiscard]] virtual ESystemStatus OnUpdate()      = 0;
        [[nodiscard]] virtual ESystemStatus OnLateUpdate()  = 0;
    };
} // namespace Garage

#endif // GARAGE_CORE_APP_SUBSYSTEM_HPP
