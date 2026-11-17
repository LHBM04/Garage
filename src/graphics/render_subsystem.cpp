#include "garage/graphics/render_subsystem.hpp"

#include <memory>
#include <ranges>
#include <utility>

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_gpu.h>

#include "garage/core/engine.hpp"
#include "garage/core/system_status.hpp"

#include "garage/graphics/gpu_command_buffer.hpp"
#include "garage/graphics/gpu_device.hpp"
#include "garage/graphics/gpu_fence.hpp"
#include "garage/graphics/gpu_swap_chain.hpp"
#include "garage/graphics/gpu_target.hpp"

#include "garage/platform/display.hpp"
#include "garage/platform/display_events.hpp"
#include "garage/platform/display_subsystem.hpp"
#include "garage/platform/event_subsystem.hpp"

#include "garage/utilities/logger.hpp"

#include "gpu_device_impl.hpp"

namespace Garage
{
    RenderSubsystem::~RenderSubsystem() noexcept = default;

    ESystemStatus RenderSubsystem::OnStartup()
    {
        device = CreateDevice();
        if (device == nullptr)
        {
            return ESystemStatus::FAILURE;
        }

        commandBuffer = device->CreateCommandBuffer();
        if (commandBuffer == nullptr)
        {
            device.reset();
            return ESystemStatus::FAILURE;
        }

        fence = device->CreateFence();
        if (fence == nullptr)
        {
            commandBuffer = nullptr;
            device.reset();
            return ESystemStatus::FAILURE;
        }

        Engine* engine = Engine::GetInstance();
        if (engine != nullptr)
        {
            if (EventSubsystem* eventSubsystem = engine->GetSubsystem<EventSubsystem>(); eventSubsystem != nullptr)
            {
                displayCreateListenerId =
                        eventSubsystem->Subscribe<DisplayCreateEvent>(this, &RenderSubsystem::OnDisplayCreate);
                displayCloseListenerId =
                        eventSubsystem->Subscribe<DisplayCloseEvent>(this, &RenderSubsystem::OnDisplayClose);
                displayDestroyListenerId =
                        eventSubsystem->Subscribe<DisplayDestroyEvent>(this, &RenderSubsystem::OnDisplayDestroy);
            }

            if (DisplaySubsystem* displaySubsystem = engine->GetSubsystem<DisplaySubsystem>();
                displaySubsystem != nullptr)
            {
                for (const IDisplay* display : displaySubsystem->GetDisplays())
                {
                    OnDisplayCreate(DisplayCreateEvent{display});
                }
            }
        }

        return ESystemStatus::CONTINUE;
    }

    ESystemStatus RenderSubsystem::OnPreTick()
    {
        return ESystemStatus::CONTINUE;
    }

    ESystemStatus RenderSubsystem::OnTick()
    {
        return ESystemStatus::CONTINUE;
    }

    ESystemStatus RenderSubsystem::OnPostTick()
    {
        if (device == nullptr || commandBuffer == nullptr || fence == nullptr)
        {
            return ESystemStatus::FAILURE;
        }

        if (swapChains.empty())
        {
            return ESystemStatus::CONTINUE;
        }

        if (!commandBuffer->Reset())
        {
            return ESystemStatus::FAILURE;
        }

        for (IGPUSwapChain* swapChain : swapChains | std::views::values)
        {
            if (swapChain == nullptr)
            {
                continue;
            }

            const GPUTarget* target = swapChain->AcquireBackBufferTarget(*commandBuffer);
            if (target == nullptr)
            {
                continue;
            }

            commandBuffer->SetGPUTarget(*target);
            onRender.Broadcast(*commandBuffer);
            commandBuffer->EndRenderPass();
        }

        if (!commandBuffer->Submit(fence))
        {
            LOG_CRITICAL("GPU 명령 버퍼 제출에 실패했습니다.");
            return ESystemStatus::FAILURE;
        }

        for (const IGPUSwapChain* swapChain : swapChains | std::views::values)
        {
            if (swapChain != nullptr)
            {
                swapChain->Present();
            }
        }

        fence->Wait();
        return ESystemStatus::CONTINUE;
    }

    void RenderSubsystem::OnShutdown()
    {
        Engine* engine = Engine::GetInstance();
        if (engine != nullptr)
        {
            if (EventSubsystem* eventSubsystem = engine->GetSubsystem<EventSubsystem>(); eventSubsystem != nullptr)
            {
                eventSubsystem->Unsubscribe<DisplayCreateEvent>(displayCreateListenerId);
                eventSubsystem->Unsubscribe<DisplayCloseEvent>(displayCloseListenerId);
                eventSubsystem->Unsubscribe<DisplayDestroyEvent>(displayDestroyListenerId);
            }
        }

        displayCreateListenerId  = 0;
        displayCloseListenerId   = 0;
        displayDestroyListenerId = 0;

        onRender.Clear();
        swapChains.clear();
        commandBuffer = nullptr;
        fence         = nullptr;
        device.reset();
    }

    void RenderSubsystem::OnDisplayCreate(const DisplayCreateEvent& event)
    {
        const IDisplay* display = event.display;
        if (display == nullptr || device == nullptr)
        {
            return;
        }

        if (swapChains.contains(display->GetID()))
        {
            return;
        }

        IGPUSwapChain* swapChain = device->CreateSwapChain(display);
        if (swapChain == nullptr)
        {
            LOG_ERROR("GPU 스왑체인 생성에 실패했습니다.");
            return;
        }

        swapChains.emplace(display->GetID(), swapChain);
    }

    void RenderSubsystem::OnDisplayClose(const DisplayCloseEvent& event)
    {
        ReleaseDisplaySwapChain(event.display);
    }

    void RenderSubsystem::OnDisplayDestroy(const DisplayDestroyEvent& event)
    {
        ReleaseDisplaySwapChain(event.display);
    }

    void RenderSubsystem::ReleaseDisplaySwapChain(const IDisplay* display) noexcept
    {
        if (display == nullptr || device == nullptr)
        {
            return;
        }

        const auto IT = swapChains.find(display->GetID());
        if (IT != swapChains.end())
        {
            device->DestroySwapChain(IT->second);
            swapChains.erase(IT);
        }
    }

    std::unique_ptr<IGPUDevice> RenderSubsystem::CreateDevice() const
    {
        constexpr SDL_GPUShaderFormat SHADER_FORMATS =
                SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_MSL;

#if defined(GARAGE_DEBUG) || defined(_DEBUG)
        constexpr bool DEBUG_MODE = true;
#else
        constexpr bool DEBUG_MODE = false;
#endif

        SDL_GPUDevice* sdlDevice = SDL_CreateGPUDevice(SHADER_FORMATS, DEBUG_MODE, nullptr);
        if (sdlDevice == nullptr)
        {
            LOG_CRITICAL("SDL_GPU 디바이스 생성에 실패했습니다: {}", SDL_GetError());
            return nullptr;
        }

        SDL_GPUSamplerCreateInfo samplerInfo{};
        samplerInfo.min_filter     = SDL_GPU_FILTER_LINEAR;
        samplerInfo.mag_filter     = SDL_GPU_FILTER_LINEAR;
        samplerInfo.mipmap_mode    = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR;
        samplerInfo.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
        samplerInfo.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
        samplerInfo.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
        samplerInfo.max_lod        = 1000.0f;

        SDL_GPUSampler* defaultSampler = SDL_CreateGPUSampler(sdlDevice, &samplerInfo);
        if (defaultSampler == nullptr)
        {
            SDL_DestroyGPUDevice(sdlDevice);
            LOG_ERROR("기본 GPU 샘플러 생성에 실패했습니다: {}", SDL_GetError());
            return nullptr;
        }

        SDL_GPUTextureCreateInfo fallbackTextureInfo{};
        fallbackTextureInfo.type                 = SDL_GPU_TEXTURETYPE_2D;
        fallbackTextureInfo.format               = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
        fallbackTextureInfo.usage                = SDL_GPU_TEXTUREUSAGE_SAMPLER;
        fallbackTextureInfo.width                = 1;
        fallbackTextureInfo.height               = 1;
        fallbackTextureInfo.layer_count_or_depth = 1;
        fallbackTextureInfo.num_levels           = 1;
        fallbackTextureInfo.sample_count         = SDL_GPU_SAMPLECOUNT_1;

        SDL_GPUTexture* fallbackTexture = SDL_CreateGPUTexture(sdlDevice, &fallbackTextureInfo);
        if (fallbackTexture == nullptr)
        {
            SDL_ReleaseGPUSampler(sdlDevice, defaultSampler);
            SDL_DestroyGPUDevice(sdlDevice);
            LOG_ERROR("기본 폴백 GPU 텍스처 생성에 실패했습니다: {}", SDL_GetError());
            return nullptr;
        }

        return std::make_unique<GPUDeviceImpl>(sdlDevice, defaultSampler, fallbackTexture);
    }
} // namespace Garage
