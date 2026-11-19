#include "garage/platform/time_subsystem.hpp"

#include <cmath>

#include <SDL3/SDL_timer.h>

#include "garage/core/common.hpp"
#include "garage/core/engine_subsystem.hpp"
#include "garage/core/system_status.hpp"

#include "garage/utilities/logger.hpp"

namespace Garage
{
    TimeSubsystem::TimeSubsystem() noexcept
        : EngineSubsystemBase(0)
    {
    }

    Float32 TimeSubsystem::GetTotalTime() const noexcept
    {
        return static_cast<Float32>(totalTime);
    }

    Float32 TimeSubsystem::GetTimeScale() const noexcept
    {
        return timeScale;
    }

    void TimeSubsystem::SetTimeScale(Float32 value) noexcept
    {
        timeScale = value;
    }

    UInt32 TimeSubsystem::GetFixedStep() const noexcept
    {
        return maxFixedStepsPerFrame;
    }

    void TimeSubsystem::SetFixedStep(UInt32 value) noexcept
    {
        maxFixedStepsPerFrame = value;
    }

    Float32 TimeSubsystem::GetDeltaTime() const noexcept
    {
        return static_cast<Float32>(deltaTime) * timeScale;
    }

    Float32 TimeSubsystem::GetUnscaledDeltaTime() const noexcept
    {
        return static_cast<Float32>(deltaTime);
    }

    Float32 TimeSubsystem::GetFixedDeltaTime() const noexcept
    {
        return static_cast<Float32>(fixedDeltaTime) * timeScale;
    }

    Float32 TimeSubsystem::GetUnscaledFixedDeltaTime() const noexcept
    {
        return static_cast<Float32>(fixedDeltaTime);
    }

    UInt32 TimeSubsystem::GetFrameCount() const noexcept
    {
        return frameCount;
    }

    ESystemStatus TimeSubsystem::OnStartup([[maybe_unused]] SystemContext& context) noexcept
    {
        const UInt64 FREQUENCY = static_cast<UInt64>(SDL_GetPerformanceFrequency());
        if (FREQUENCY == 0)
        {
            LOG_ERROR("SDL 고해상도 타이머 주기를 조회하지 못했습니다.");
            return ESystemStatus::FAILURE;
        }

        secondsPerCount = 1.0 / static_cast<Float64>(FREQUENCY);
        startTime       = static_cast<UInt64>(SDL_GetPerformanceCounter());
        previousTime    = startTime;

        deltaTime = 0.0;
        totalTime = 0.0;

        frameCount = 0;

        fixedAccumulator = 0.0;
        fixedSteps       = 0;

        return ESystemStatus::CONTINUE;
    }

    ESystemStatus TimeSubsystem::OnPreTick([[maybe_unused]] SystemContext& context) noexcept
    {
        const UInt64 NOW = static_cast<UInt64>(SDL_GetPerformanceCounter());

        deltaTime    = (frameCount == 0 || NOW < previousTime)
                ? 0.0
                : static_cast<Float64>(NOW - previousTime) * secondsPerCount;
        totalTime    = static_cast<Float64>(NOW - startTime) * secondsPerCount;
        previousTime = NOW;

        ++frameCount;

        if (timeScale > 0.0f)
        {
            fixedAccumulator += deltaTime;
        }
        fixedSteps = 0;

        return ESystemStatus::CONTINUE;
    }

    ESystemStatus TimeSubsystem::OnTick([[maybe_unused]] SystemContext& context) noexcept
    {
        return ESystemStatus::CONTINUE;
    }

    ESystemStatus TimeSubsystem::OnPostTick([[maybe_unused]] SystemContext& context) noexcept
    {
        return ESystemStatus::CONTINUE;
    }

    void TimeSubsystem::OnShutdown([[maybe_unused]] SystemContext& context) noexcept
    {
    }

    bool TimeSubsystem::ShouldFixedStep() noexcept
    {
        if (timeScale <= 0.0f)
        {
            return false;
        }

        if (fixedSteps >= maxFixedStepsPerFrame)
        {
            fixedAccumulator = std::fmod(fixedAccumulator, fixedDeltaTime);
            return false;
        }

        if (fixedAccumulator < fixedDeltaTime)
        {
            return false;
        }

        fixedAccumulator -= fixedDeltaTime;
        ++fixedSteps;

        return true;
    }
} // namespace Garage
