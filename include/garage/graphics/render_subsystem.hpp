#ifndef GARAGE_GRAPHICS_RENDER_SUBSYSTEM_HPP
#define GARAGE_GRAPHICS_RENDER_SUBSYSTEM_HPP

#include <memory_resource>
#include <string>
#include <unordered_map>

#include "../core/common.hpp"
#include "../core/engine_subsystem.hpp"
#include "../core/memory.hpp"
#include "../core/system_status.hpp"

#include "../platform/display.hpp"

namespace Garage
{
    struct DisplayCreateEvent;
    struct DisplayCloseEvent;
    struct DisplayDestroyEvent;

    class IGPUCommandBuffer;
    class IGPUDevice;
    class IGPUFence;
    class IGPUSwapChain;
    class SystemContext;

    class GARAGE_API RenderSubsystem final : public EngineSubsystemBase
    {
    public:
        RenderSubsystem() noexcept;
        ~RenderSubsystem() noexcept override;

        [[nodiscard]] ESystemStatus OnStartup(SystemContext& context) override;
        [[nodiscard]] ESystemStatus OnPreTick(SystemContext& context) override;
        [[nodiscard]] ESystemStatus OnTick(SystemContext& context) override;
        [[nodiscard]] ESystemStatus OnPostTick(SystemContext& context) override;
        void                        OnShutdown(SystemContext& context) override;

        [[nodiscard]] Delegate<SystemContext&>& OnRender() noexcept;

        void OnDisplayCreate(const DisplayCreateEvent& event);
        void OnDisplayClose(const DisplayCloseEvent& event);
        void OnDisplayDestroy(const DisplayDestroyEvent& event);

        [[nodiscard]] IGPUDevice*       GetDevice() noexcept;
        [[nodiscard]] const IGPUDevice* GetDevice() const noexcept;

        [[nodiscard]] IGPUCommandBuffer*       GetGPUCommandBuffer() noexcept;
        [[nodiscard]] const IGPUCommandBuffer* GetGPUCommandBuffer() const noexcept;

    private:
        [[nodiscard]] Result<UniquePtr<IGPUDevice>, std::pmr::string> CreateDevice() const;
        void ReleaseDisplaySwapChain(const IDisplay* display) noexcept;

        UniquePtr<IGPUDevice>                                  device;
        IGPUCommandBuffer*                                     commandBuffer = nullptr;
        IGPUFence*                                             fence         = nullptr;
        std::pmr::unordered_map<IDisplay::EId, IGPUSwapChain*> swapChains{Memory::GetMemoryResource()};
        Delegate<SystemContext&>                               onRender                 = {};
        Int32                                                  displayCreateListenerId  = 0;
        Int32                                                  displayCloseListenerId   = 0;
        Int32                                                  displayDestroyListenerId = 0;
    };

    inline Delegate<SystemContext&>& RenderSubsystem::OnRender() noexcept
    {
        return onRender;
    }

    inline IGPUDevice* RenderSubsystem::GetDevice() noexcept
    {
        return device.get();
    }

    inline const IGPUDevice* RenderSubsystem::GetDevice() const noexcept
    {
        return device.get();
    }

    inline IGPUCommandBuffer* RenderSubsystem::GetGPUCommandBuffer() noexcept
    {
        return commandBuffer;
    }

    inline const IGPUCommandBuffer* RenderSubsystem::GetGPUCommandBuffer() const noexcept
    {
        return commandBuffer;
    }
} // namespace Garage

#endif // GARAGE_GRAPHICS_RENDER_SUBSYSTEM_HPP
