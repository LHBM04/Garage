#ifndef GARAGE_PLATFORM_TIME_SUBSYSTEM_HPP
#define GARAGE_PLATFORM_TIME_SUBSYSTEM_HPP

#include "garage/core/defines.hpp"
#include "garage/core/engine_subsystem.hpp"
#include "garage/core/system_status.hpp"
#include "garage/core/types.hpp"

namespace Garage
{
    class App;

    class GARAGE_API TimeSubsystem final : public EngineSubsystemBase
    {
        friend class App;

    public:
        TimeSubsystem() noexcept;

        [[nodiscard]] Float32 GetTotalTime() const noexcept;

        [[nodiscard]] Float32 GetTimeScale() const noexcept;
        void                  SetTimeScale(Float32 value) noexcept;

        [[nodiscard]] UInt32 GetFixedStep() const noexcept;
        void                 SetFixedStep(UInt32 value) noexcept;

        [[nodiscard]] Float32 GetDeltaTime() const noexcept;
        [[nodiscard]] Float32 GetUnscaledDeltaTime() const noexcept;

        [[nodiscard]] Float32 GetFixedDeltaTime() const noexcept;
        [[nodiscard]] Float32 GetUnscaledFixedDeltaTime() const noexcept;

        [[nodiscard]] UInt32 GetFrameCount() const noexcept;

        [[nodiscard]] ESystemStatus OnStartup() noexcept override;
        [[nodiscard]] ESystemStatus OnPreTick() noexcept override;
        [[nodiscard]] ESystemStatus OnTick() noexcept override;
        [[nodiscard]] ESystemStatus OnPostTick() noexcept override;
        void                        OnShutdown() noexcept override;

    private:
        [[nodiscard]] bool ShouldFixedStep() noexcept;

        UInt64 startTime    = 0;
        UInt64 previousTime = 0;

        Float64 totalTime       = 0.0;
        Float64 secondsPerCount = 0.0;

        Float64 deltaTime = 0.0;
        Float32 timeScale = 1.0f;

        Float64 fixedDeltaTime   = 1.0 / 60.0;
        Float64 fixedAccumulator = 0.0;

        UInt32 maxFixedStepsPerFrame = 5;
        UInt32 fixedSteps            = 0;
        UInt32 frameCount            = 0;
    };
} // namespace Garage

#endif // GARAGE_PLATFORM_TIME_SUBSYSTEM_HPP
