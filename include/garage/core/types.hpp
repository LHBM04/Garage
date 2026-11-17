#ifndef GARAGE_CORE_TYPES_HPP
#define GARAGE_CORE_TYPES_HPP

#include <cstddef>
#include <cstdint>
#include <functional>

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

    using Float32 = float;
    using Float64 = double;

    using USize = std::size_t;
    using ISize = std::ptrdiff_t;

    using Byte = std::byte;

    template <class TResult, class... TArgs>
    using Func = std::function<TResult(TArgs...)>;

    template <class... TArgs>
    using Action = std::function<void(TArgs...)>;

    template <class... TArgs>
    using Predicate = std::function<bool(TArgs...)>;
} // namespace Garage

#endif // GARAGE_CORE_TYPES_HPP
