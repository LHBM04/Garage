#include "garage/core/common.hpp"

#include <cmath>
#include <random>

#include "garage/core/defines.hpp"
#include "garage/core/types.hpp"

namespace Garage
{
    namespace Random
    {
        namespace
        {
            [[nodiscard]] std::mt19937& GetGenerator()
            {
                static std::mt19937 generator{std::random_device{}()};
                return generator;
            }
        } // namespace

        Int32 RandInt(Int32 min, Int32 max)
        {
            ASSERT(min <= max);

            if (min == max)
            {
                return min;
            }

            std::uniform_int_distribution<Int32> distribution(min, max);
            return distribution(GetGenerator());
        }

        Float32 RandFloat(Float32 min, Float32 max)
        {
            ASSERT(std::isfinite(min));
            ASSERT(std::isfinite(max));
            ASSERT(min <= max);

            if (min == max)
            {
                return min;
            }

            std::uniform_real_distribution<Float64> distribution(min, max);
            return static_cast<Float32>(distribution(GetGenerator()));
        }
    } // namespace Random
} // namespace Garage
