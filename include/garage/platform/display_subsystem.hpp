#ifndef GARAGE_PLATFORM_DISPLAY_SUBSYSTEM_HPP
#define GARAGE_PLATFORM_DISPLAY_SUBSYSTEM_HPP

#include <memory>
#include <ranges>
#include <vector>

#include "garage/core/defines.hpp"
#include "garage/core/engine_subsystem.hpp"
#include "garage/core/system_status.hpp"
#include "garage/core/types.hpp"
#include "garage/platform/display.hpp"
#include "garage/platform/display_options.hpp"

namespace Garage
{
    class GARAGE_API DisplaySubsystem final : public EngineSubsystemBase
    {
    public:
        DisplaySubsystem() noexcept;

        [[nodiscard]] ESystemStatus OnStartup() override;
        [[nodiscard]] ESystemStatus OnPreTick() override;
        [[nodiscard]] ESystemStatus OnTick() override;
        [[nodiscard]] ESystemStatus OnPostTick() override;
        void                        OnShutdown() override;

        [[nodiscard]] IDisplay* CreateDisplay(const DisplayOptions& options) noexcept;
        void                    DestroyDisplay(IDisplay* display) noexcept;

        [[nodiscard]] auto GetDisplays() noexcept;
        [[nodiscard]] auto GetDisplays() const noexcept;

        [[nodiscard]] USize GetDisplayCount() const noexcept;

        [[nodiscard]] IDisplay*       GetDisplay(USize index) noexcept;
        [[nodiscard]] const IDisplay* GetDisplay(USize index) const noexcept;

        [[nodiscard]] IDisplay*       FindDisplayByID(IDisplay::ID id) noexcept;
        [[nodiscard]] const IDisplay* FindDisplayByID(IDisplay::ID id) const noexcept;

        [[nodiscard]] bool IsDisplayValid(const IDisplay* display) const noexcept;
        [[nodiscard]] bool IsDisplayValid(IDisplay::ID id) const noexcept;

    private:
        std::vector<std::unique_ptr<IDisplay>> displays = {};
    };

    inline DisplaySubsystem::DisplaySubsystem() noexcept
        : EngineSubsystemBase(10)
    {
    }

    inline auto DisplaySubsystem::GetDisplays() noexcept
    {
        return displays |
                std::views::transform(
                        [](const std::unique_ptr<IDisplay>& displayPtr) noexcept -> IDisplay*
                        {
                            return displayPtr.get();
                        });
    }

    inline auto DisplaySubsystem::GetDisplays() const noexcept
    {
        return displays |
                std::views::transform(
                        [](const std::unique_ptr<IDisplay>& displayPtr) noexcept -> const IDisplay*
                        {
                            return displayPtr.get();
                        });
    }
} // namespace Garage

#endif // GARAGE_PLATFORM_DISPLAY_SUBSYSTEM_HPP
