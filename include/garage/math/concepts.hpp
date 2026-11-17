#ifndef GARAGE_MATH_CONCEPTS_HPP
#define GARAGE_MATH_CONCEPTS_HPP

#include <concepts>

#include "garage/core/types.hpp"

namespace Garage
{
    template <class TValue>
    concept Integral = std::same_as<TValue, Int32>;

    template <class TValue>
    concept Floatable = std::same_as<TValue, Float32>;

    template <class TValue>
    concept Scalarable = Integral<TValue> || Floatable<TValue>;
} // namespace Garage

#endif // GARAGE_MATH_CONCEPTS_HPP
