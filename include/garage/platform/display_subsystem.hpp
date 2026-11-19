#ifndef GARAGE_PLATFORM_DISPLAY_SUBSYSTEM_HPP
#define GARAGE_PLATFORM_DISPLAY_SUBSYSTEM_HPP

#include <memory_resource>
#include <ranges>
#include <vector>

#include "../core/allocator.hpp"
#include "../core/defines.hpp"
#include "../core/engine_subsystem.hpp"
#include "../core/system_status.hpp"
#include "../core/types.hpp"

#include "display.hpp"
#include "display_options.hpp"

namespace Garage
{
    class EventSubsystem;
    class SystemContext;

    class GARAGE_API DisplaySubsystem final : public EngineSubsystemBase
    {
    public:
        DisplaySubsystem() noexcept;

        [[nodiscard]] ESystemStatus OnStartup(SystemContext& context) override;
        [[nodiscard]] ESystemStatus OnPreTick(SystemContext& context) override;
        [[nodiscard]] ESystemStatus OnTick(SystemContext& context) override;
        [[nodiscard]] ESystemStatus OnPostTick(SystemContext& context) override;
        void                        OnShutdown(SystemContext& context) override;

        [[nodiscard]] IDisplay* CreateDisplay(const DisplayOptions& options) noexcept;
        void                    DestroyDisplay(IDisplay* display) noexcept;

        [[nodiscard]] auto GetDisplays() noexcept;
        [[nodiscard]] auto GetDisplays() const noexcept;

        [[nodiscard]] USize GetDisplayCount() const noexcept;

        [[nodiscard]] IDisplay*       GetDisplay(USize index) noexcept;
        [[nodiscard]] const IDisplay* GetDisplay(USize index) const noexcept;

        [[nodiscard]] IDisplay*       FindDisplayByID(IDisplay::EId id) noexcept;
        [[nodiscard]] const IDisplay* FindDisplayByID(IDisplay::EId id) const noexcept;

        [[nodiscard]] bool IsDisplayValid(const IDisplay* display) const noexcept;
        [[nodiscard]] bool IsDisplayValid(IDisplay::EId id) const noexcept;

    private:
        EventSubsystem*                                  eventSubsystem = nullptr;
        std::pmr::vector<Allocator::UniquePtr<IDisplay>> displays{Allocator::GetMemoryResource()};
    };

    inline DisplaySubsystem::DisplaySubsystem() noexcept
        : EngineSubsystemBase(10)
    {
    }

    inline auto DisplaySubsystem::GetDisplays() noexcept
    {
        return displays |
                std::views::transform(
                        [](const Allocator::UniquePtr<IDisplay>& displayPtr) noexcept -> IDisplay*
                        {
                            return displayPtr.get();
                        });
    }

    inline auto DisplaySubsystem::GetDisplays() const noexcept
    {
        return displays |
                std::views::transform(
                        [](const Allocator::UniquePtr<IDisplay>& displayPtr) noexcept -> const IDisplay*
                        {
                            return displayPtr.get();
                        });
    }
} // namespace Garage

#endif // GARAGE_PLATFORM_DISPLAY_SUBSYSTEM_HPP
