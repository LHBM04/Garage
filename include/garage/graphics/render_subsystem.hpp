#ifndef GARAGE_GRAPHICS_RENDER_SUBSYSTEM_HPP
#define GARAGE_GRAPHICS_RENDER_SUBSYSTEM_HPP

#include <memory>
#include <unordered_map>

#include "../core/defines.hpp"
#include "../core/engine_subsystem.hpp"
#include "../core/system_status.hpp"
#include "../core/types.hpp"

#include "../platform/display.hpp"

#include "../utilities/delegate.hpp"

#include "gpu_device.hpp"

namespace Garage
{
    struct DisplayCreateEvent;
    struct DisplayCloseEvent;
    struct DisplayDestroyEvent;

    class GPUCommandBuffer;
    class GPUFence;
    class GPUSwapChain;

    class GARAGE_API RenderSubsystem final : public EngineSubsystemBase
    {
    public:
        RenderSubsystem() noexcept;

        [[nodiscard]] ESystemStatus OnStartup() override;
        [[nodiscard]] ESystemStatus OnPreTick() override;
        [[nodiscard]] ESystemStatus OnTick() override;
        [[nodiscard]] ESystemStatus OnPostTick() override;
        void                        OnShutdown() override;

        [[nodiscard]] Delegate<GPUCommandBuffer&>& OnRender() noexcept;

        void OnDisplayCreate(const DisplayCreateEvent& event);
        void OnDisplayClose(const DisplayCloseEvent& event);
        void OnDisplayDestroy(const DisplayDestroyEvent& event);

        [[nodiscard]] GPUDevice*       GetDevice() noexcept;
        [[nodiscard]] const GPUDevice* GetDevice() const noexcept;

    private:
        [[nodiscard]] std::unique_ptr<GPUDevice> CreateDevice() const;
        void                                     ReleaseDisplaySwapChain(const IDisplay* display) noexcept;

        std::unique_ptr<GPUDevice>                       device                   = nullptr;
        GPUCommandBuffer*                                commandBuffer            = nullptr;
        GPUFence*                                        fence                    = nullptr;
        std::unordered_map<IDisplay::EId, GPUSwapChain*> swapChains               = {};
        Delegate<GPUCommandBuffer&>                      onRender                 = {};
        Int32                                            displayCreateListenerId  = 0;
        Int32                                            displayCloseListenerId   = 0;
        Int32                                            displayDestroyListenerId = 0;
    };

    inline RenderSubsystem::RenderSubsystem() noexcept
        : EngineSubsystemBase(20)
    {
    }

    inline Delegate<GPUCommandBuffer&>& RenderSubsystem::OnRender() noexcept
    {
        return onRender;
    }

    inline GPUDevice* RenderSubsystem::GetDevice() noexcept
    {
        return device.get();
    }

    inline const GPUDevice* RenderSubsystem::GetDevice() const noexcept
    {
        return device.get();
    }
} // namespace Garage

#endif // GARAGE_GRAPHICS_RENDER_SUBSYSTEM_HPP
