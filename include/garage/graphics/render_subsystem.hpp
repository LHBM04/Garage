#ifndef GARAGE_GRAPHICS_RENDER_SUBSYSTEM_HPP
#define GARAGE_GRAPHICS_RENDER_SUBSYSTEM_HPP

#include <memory>
#include <string>
#include <unordered_map>

#include "../core/defines.hpp"
#include "../core/engine_subsystem.hpp"
#include "../core/system_status.hpp"
#include "../core/types.hpp"

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

    class GARAGE_API RenderSubsystem final : public EngineSubsystemBase
    {
    public:
        RenderSubsystem() noexcept;
        ~RenderSubsystem() noexcept override;

        [[nodiscard]] ESystemStatus OnStartup() override;
        [[nodiscard]] ESystemStatus OnPreTick() override;
        [[nodiscard]] ESystemStatus OnTick() override;
        [[nodiscard]] ESystemStatus OnPostTick() override;
        void                        OnShutdown() override;

        [[nodiscard]] Delegate<IGPUCommandBuffer&>& OnRender() noexcept;

        void OnDisplayCreate(const DisplayCreateEvent& event);
        void OnDisplayClose(const DisplayCloseEvent& event);
        void OnDisplayDestroy(const DisplayDestroyEvent& event);

        [[nodiscard]] IGPUDevice*       GetDevice() noexcept;
        [[nodiscard]] const IGPUDevice* GetDevice() const noexcept;

    private:
        [[nodiscard]] Result<std::unique_ptr<IGPUDevice>, std::string> CreateDevice() const;
        void ReleaseDisplaySwapChain(const IDisplay* display) noexcept;

        std::unique_ptr<IGPUDevice>                       device                   = nullptr;
        IGPUCommandBuffer*                                commandBuffer            = nullptr;
        IGPUFence*                                        fence                    = nullptr;
        std::unordered_map<IDisplay::EId, IGPUSwapChain*> swapChains               = {};
        Delegate<IGPUCommandBuffer&>                      onRender                 = {};
        Int32                                             displayCreateListenerId  = 0;
        Int32                                             displayCloseListenerId   = 0;
        Int32                                             displayDestroyListenerId = 0;
    };

    inline RenderSubsystem::RenderSubsystem() noexcept
        : EngineSubsystemBase(20)
    {
    }

    inline Delegate<IGPUCommandBuffer&>& RenderSubsystem::OnRender() noexcept
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
} // namespace Garage

#endif // GARAGE_GRAPHICS_RENDER_SUBSYSTEM_HPP
