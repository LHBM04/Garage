#ifndef GARAGE_CORE_ENGINE_SUBSYSTEM_HPP
#define GARAGE_CORE_ENGINE_SUBSYSTEM_HPP

#include "defines.hpp"
#include "subsystem.hpp"
#include "system_status.hpp"

namespace Garage
{
    class SystemContext;

    class GARAGE_API EngineSubsystemBase : public SubsystemBase
    {
    public:
        using SubsystemBase::SubsystemBase;

        [[nodiscard]] virtual ESystemStatus OnPreTick(SystemContext& context)  = 0;
        [[nodiscard]] virtual ESystemStatus OnTick(SystemContext& context)     = 0;
        [[nodiscard]] virtual ESystemStatus OnPostTick(SystemContext& context) = 0;
    };
} // namespace Garage

#endif // GARAGE_CORE_ENGINE_SUBSYSTEM_HPP
