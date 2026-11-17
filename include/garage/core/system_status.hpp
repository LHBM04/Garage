#ifndef GARAGE_CORE_SYSTEM_STATUS_HPP
#define GARAGE_CORE_SYSTEM_STATUS_HPP

#include "types.hpp"

namespace Garage
{
    enum class ESystemStatus : UInt8
    {
        SUCCESS,
        FAILURE,
        CONTINUE,
    };
} // namespace Garage

#endif // GARAGE_CORE_SYSTEM_STATUS_HPP
