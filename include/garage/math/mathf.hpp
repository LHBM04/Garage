#ifndef GARAGE_MATH_MATHF_HPP
#define GARAGE_MATH_MATHF_HPP

#include <algorithm>
#include <cmath>
#include <concepts>
#include <limits>

#include "../core/types.hpp"

namespace Garage
{
    template <class TValue>
    concept Integral = std::same_as<TValue, Int32>;

    template <class TValue>
    concept Floatable = std::same_as<TValue, Float32>;

    template <class TValue>
    concept Scalarable = Integral<TValue> || Floatable<TValue>;
} // namespace Garage

namespace Garage::Mathf
{
    inline constexpr Float32 EPSILON = 0.0001f;

    inline constexpr Float32 PI                   = 3.1415926f;
    inline constexpr Float32 HALF_PI              = 1.5707963f;
    inline constexpr Float32 DOUBLE_PI            = 6.283185f;
    inline constexpr Float32 ONE_THREE_FOURTHS_PI = 5.4977871f;

    inline constexpr Float32 DEG_TO_RAD = PI / 180.0f;
    inline constexpr Float32 RAD_TO_DEG = 180.0f / PI;

    inline constexpr Float32 POSITIVE_INF = std::numeric_limits<Float32>::infinity();
    inline constexpr Float32 NEGATIVE_INF = -std::numeric_limits<Float32>::infinity();
    inline constexpr Float32 MAX_VALUE    = std::numeric_limits<Float32>::max();
    inline constexpr Float32 MIN_VALUE    = std::numeric_limits<Float32>::lowest();

    [[nodiscard]] constexpr Float32 Sign(Float32 value) noexcept
    {
        return (value >= 0.0f) ? 1.0f : -1.0f;
    }

    [[nodiscard]] constexpr Float32 Proportion(Float32 a, Float32 b, Float32 d) noexcept
    {
        return (a / b) * d;
    }

    [[nodiscard]] constexpr Float32 Flip(Float32 value, Float32 over = 0.0f) noexcept
    {
        return -(value - over) + over;
    }

    [[nodiscard]] inline bool IsZero(Float32 value, Float32 epsilon = EPSILON) noexcept
    {
        return std::abs(value) <= epsilon;
    }

    [[nodiscard]] inline bool IsApproximately(Float32 a, Float32 b, Float32 epsilon = EPSILON) noexcept
    {
        return std::abs(a - b) <= epsilon;
    }

    [[nodiscard]] inline Float32 Floor(Float32 value, Float32 interval = 1.0f, Float32 offset = 0.0f) noexcept
    {
        if (IsZero(interval))
        {
            interval = 1.0f;
        }
        return std::floor((value - offset) / interval) * interval + offset;
    }

    [[nodiscard]] inline Float32 Ceil(Float32 value, Float32 interval = 1.0f, Float32 offset = 0.0f) noexcept
    {
        if (IsZero(interval))
        {
            interval = 1.0f;
        }
        return std::ceil((value - offset) / interval) * interval + offset;
    }

    [[nodiscard]] inline Float32 Round(Float32 value, Float32 interval = 1.0f, Float32 offset = 0.0f) noexcept
    {
        if (IsZero(interval))
        {
            interval = 1.0f;
        }
        return std::round((value - offset) / interval) * interval + offset;
    }

    [[nodiscard]] constexpr Int32 Modp(Int32 dividend, Int32 divisor) noexcept
    {
        return ((dividend % divisor) + divisor) % divisor;
    }

    [[nodiscard]] inline Float32 Modp(Float32 dividend, Float32 divisor) noexcept
    {
        return std::fmod(std::fmod(dividend, divisor) + divisor, divisor);
    }

    [[nodiscard]] constexpr Float32 Clamp(Float32 value, Float32 min, Float32 max) noexcept
    {
        return std::max(min, std::min(max, value));
    }

    [[nodiscard]] constexpr Float32 Clamp01(Float32 value) noexcept
    {
        return Clamp(value, 0.0f, 1.0f);
    }

    [[nodiscard]] constexpr Float32 LerpUnclamped(Float32 start, Float32 end, Float32 t) noexcept
    {
        return start + t * (end - start);
    }

    [[nodiscard]] constexpr Float32 Lerp(Float32 start, Float32 end, Float32 t) noexcept
    {
        return LerpUnclamped(start, end, Clamp01(t));
    }

    [[nodiscard]] inline Float32 InverseLerp(Float32 start, Float32 end, Float32 value) noexcept
    {
        if (IsApproximately(start, end))
        {
            return 0.0f;
        }
        return Clamp01((value - start) / (end - start));
    }

    [[nodiscard]] inline Float32
            Remap(Float32 value, Float32 inMin, Float32 inMax, Float32 outMin, Float32 outMax) noexcept
    {
        return Lerp(outMin, outMax, InverseLerp(inMin, inMax, value));
    }

    [[nodiscard]] constexpr Float32 SmoothStep(Float32 edge0, Float32 edge1, Float32 x) noexcept
    {
        const Float32 t = Clamp01((x - edge0) / (edge1 - edge0));
        return t * t * (3.0f - 2.0f * t);
    }

    [[nodiscard]] constexpr Float32 SmootherStep(Float32 edge0, Float32 edge1, Float32 x) noexcept
    {
        const Float32 t = Clamp01((x - edge0) / (edge1 - edge0));
        return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
    }

    [[nodiscard]] inline Float32 MoveTowards(Float32 current, Float32 target, Float32 maxDelta) noexcept
    {
        if (std::abs(target - current) <= maxDelta)
        {
            return target;
        }
        return current + Sign(target - current) * maxDelta;
    }

    [[nodiscard]] inline Float32 SmoothDamp(Float32  current,
                                            Float32  target,
                                            Float32& currentVelocity,
                                            Float32  smoothTime,
                                            Float32  maxSpeed,
                                            Float32  deltaTime) noexcept
    {
        smoothTime          = std::max(0.0001f, smoothTime);
        const Float32 omega = 2.0f / smoothTime;
        const Float32 x     = omega * deltaTime;
        const Float32 exp   = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);

        Float32       change     = current - target;
        const Float32 originalTo = target;
        const Float32 maxChange  = maxSpeed * smoothTime;
        change                   = Clamp(change, -maxChange, maxChange);
        target                   = current - change;

        const Float32 temp = (currentVelocity + omega * change) * deltaTime;
        currentVelocity    = (currentVelocity - omega * temp) * exp;
        Float32 output     = target + (change + temp) * exp;

        if ((originalTo - current > 0.0f) == (output > originalTo))
        {
            output          = originalTo;
            currentVelocity = (deltaTime > EPSILON) ? ((output - originalTo) / deltaTime) : 0.0f;
        }

        return output;
    }

    [[nodiscard]] inline Float32 Repeat(Float32 t, Float32 length) noexcept
    {
        return Clamp(t - std::floor(t / length) * length, 0.0f, length);
    }

    [[nodiscard]] inline Float32 PingPong(Float32 t, Float32 length) noexcept
    {
        t = Repeat(t, length * 2.0f);
        return length - std::abs(t - length);
    }

    [[nodiscard]] inline Float32 PositiveAngle(Float32 angleRadians) noexcept
    {
        return Modp(angleRadians, DOUBLE_PI);
    }

    [[nodiscard]] inline Float32 PositiveAngleDegrees(Float32 angleDegrees) noexcept
    {
        return Modp(angleDegrees, 360.0f);
    }

    [[nodiscard]] inline Float32 ShortestArc(Float32 aRadians, Float32 bRadians) noexcept
    {
        return Modp(bRadians - aRadians + PI, DOUBLE_PI) - PI;
    }

    [[nodiscard]] inline Float32 ShortestArcDegrees(Float32 aDegrees, Float32 bDegrees) noexcept
    {
        return Modp(bDegrees - aDegrees + 180.0f, 360.0f) - 180.0f;
    }

    [[nodiscard]] inline Float32 DeltaAngle(Float32 currentDegrees, Float32 targetDegrees) noexcept
    {
        return ShortestArcDegrees(currentDegrees, targetDegrees);
    }

    [[nodiscard]] inline Float32 LerpAngle(Float32 startDegrees, Float32 endDegrees, Float32 t) noexcept
    {
        return startDegrees + ShortestArcDegrees(startDegrees, endDegrees) * Clamp01(t);
    }

    [[nodiscard]] inline Float32
            MoveTowardsAngle(Float32 currentDegrees, Float32 targetDegrees, Float32 maxDeltaDegrees) noexcept
    {
        const Float32 deltaAngle = DeltaAngle(currentDegrees, targetDegrees);
        if (-maxDeltaDegrees < deltaAngle && deltaAngle < maxDeltaDegrees)
        {
            return targetDegrees;
        }
        return MoveTowards(currentDegrees, currentDegrees + deltaAngle, maxDeltaDegrees);
    }

    [[nodiscard]] inline Float32 PositiveArc(Float32 aRadians, Float32 bRadians) noexcept
    {
        const Float32 diff = PositiveAngle(bRadians) - PositiveAngle(aRadians);
        if (diff < 0.0f)
        {
            return diff + DOUBLE_PI;
        }
        return diff;
    }

    [[nodiscard]] inline Float32 PositiveArcDegrees(Float32 aDegrees, Float32 bDegrees) noexcept
    {
        const Float32 diff = PositiveAngleDegrees(bDegrees) - PositiveAngleDegrees(aDegrees);
        if (diff < 0.0f)
        {
            return diff + 360.0f;
        }
        return diff;
    }

    [[nodiscard]] inline bool IsAngleInRange(Float32 angleRadians, Float32 aRadians, Float32 bRadians) noexcept
    {
        return PositiveArc(aRadians, angleRadians) <= PositiveArc(aRadians, bRadians);
    }

    [[nodiscard]] inline bool IsAngleInRangeDegrees(Float32 angleDegrees, Float32 aDegrees, Float32 bDegrees) noexcept
    {
        return PositiveArcDegrees(aDegrees, angleDegrees) <= PositiveArcDegrees(aDegrees, bDegrees);
    }
} // namespace Garage::Mathf

#endif // GARAGE_MATH_MATHF_HPP
