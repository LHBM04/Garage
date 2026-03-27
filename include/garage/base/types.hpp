#ifndef __GARAGE_TYPES_HPP__
#define __GARAGE_TYPES_HPP__

#include <cstddef>
#include <cstdint>
#if defined(GARAGE_HAS_CXX26)
    #include <stdfloat>
#else
    #include <cmath>
#endif

namespace Garage
{
    using Int8  = std::int8_t;
    using Int16 = std::int16_t;
    using Int32 = std::int32_t;
    using Int64 = std::int64_t;
    
    using UInt8  = std::uint8_t;
    using UInt16 = std::uint16_t;
    using UInt32 = std::uint32_t;
    using UInt64 = std::uint64_t;

#if defined(GARAGE_HAS_CXX26)
    using float32 = std::float32_t;
    using float64 = std::float64_t;
#else
    using float32 = float;
    using float64 = double;
#endif
}

#endif