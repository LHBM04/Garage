#ifndef GARAGE_CORE_SUBSYSTEM_HPP
#define GARAGE_CORE_SUBSYSTEM_HPP

#include "defines.hpp"
#include "system_status.hpp"
#include "types.hpp"

namespace Garage
{
    class SystemContext;

    class GARAGE_API SubsystemBase
    {
    public:
        using Priority = UInt8;

        explicit SubsystemBase(Priority priority) noexcept;
        virtual ~SubsystemBase() noexcept = default;

        SubsystemBase(const SubsystemBase&)            = delete;
        SubsystemBase& operator=(const SubsystemBase&) = delete;

        SubsystemBase(SubsystemBase&&)            = delete;
        SubsystemBase& operator=(SubsystemBase&&) = delete;

        [[nodiscard]] virtual ESystemStatus OnStartup(SystemContext& context)  = 0;
        virtual void                        OnShutdown(SystemContext& context) = 0;

        [[nodiscard]] Priority GetPriority() const noexcept;

    private:
        Priority priority = 0;
    };

    inline SubsystemBase::SubsystemBase(Priority priority) noexcept
        : priority(priority)
    {
    }

    inline SubsystemBase::Priority SubsystemBase::GetPriority() const noexcept
    {
        return priority;
    }
} // namespace Garage

#endif // GARAGE_CORE_SUBSYSTEM_HPP
