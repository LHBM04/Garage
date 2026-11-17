#ifndef GARAGE_CORE_COMMON_HPP
#define GARAGE_CORE_COMMON_HPP

#include "defines.hpp"
#include "types.hpp"

namespace Garage
{
    namespace Random
    {
        [[nodiscard]] GARAGE_API Int32   RandInt(Int32 min, Int32 max);
        [[nodiscard]] GARAGE_API Float32 RandFloat(Float32 min, Float32 max);
    } // namespace Random
} // namespace Garage

#endif // GARAGE_CORE_COMMON_HPP
