#include "garage/graphics/render_subsystem.hpp"

#include <ranges>
#include <string>
#include <utility>

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_gpu.h>

#include "garage/core/allocator.hpp"
#include "garage/core/engine.hpp"
#include "garage/core/system_status.hpp"
#include "garage/core/types.hpp"

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
#include "gpu_sdl_conversions.hpp"

namespace Garage
{
    RenderSubsystem::~RenderSubsystem() noexcept = default;

    ESystemStatus RenderSubsystem::OnStartup()
    {
        auto deviceResult = CreateDevice();
        if (!deviceResult)
        {
            LOG_CRITICAL("{}", deviceResult.GetError());
            return ESystemStatus::FAILURE;
        }

        device = std::move(deviceResult).GetValue();

        const auto COMMAND_BUFFER_RESULT = device->CreateCommandBuffer();
        if (!COMMAND_BUFFER_RESULT)
        {
            LOG_CRITICAL("{}", COMMAND_BUFFER_RESULT.GetError());
            device.reset();
            return ESystemStatus::FAILURE;
        }

        commandBuffer = COMMAND_BUFFER_RESULT.GetValue();

        const auto FENCE_RESULT = device->CreateFence();
        if (!FENCE_RESULT)
        {
            LOG_CRITICAL("{}", FENCE_RESULT.GetError());
            commandBuffer = nullptr;
            device.reset();
            return ESystemStatus::FAILURE;
        }

        fence = FENCE_RESULT.GetValue();

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

        const auto RESET_RESULT = commandBuffer->Reset();
        if (!RESET_RESULT)
        {
            LOG_ERROR("{}", RESET_RESULT.GetError());
            return ESystemStatus::FAILURE;
        }

        for (IGPUSwapChain* swapChain : swapChains | std::views::values)
        {
            if (swapChain == nullptr)
            {
                continue;
            }

            const auto TARGET_RESULT = swapChain->AcquireBackBufferTarget(*commandBuffer);
            if (!TARGET_RESULT)
            {
                LOG_ERROR("{}", TARGET_RESULT.GetError());
                continue;
            }

            const GPUTarget* target = TARGET_RESULT.GetValue();
            if (target == nullptr)
            {
                continue;
            }

            commandBuffer->SetGPUTarget(*target);
            onRender.Broadcast(*commandBuffer);
            commandBuffer->EndRenderPass();
        }

        const auto SUBMIT_RESULT = commandBuffer->Submit(fence);
        if (!SUBMIT_RESULT)
        {
            LOG_CRITICAL("{}", SUBMIT_RESULT.GetError());
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

        const auto SWAP_CHAIN_RESULT = device->CreateSwapChain(display);
        if (!SWAP_CHAIN_RESULT)
        {
            LOG_ERROR("{}", SWAP_CHAIN_RESULT.GetError());
            return;
        }

        swapChains.emplace(display->GetID(), SWAP_CHAIN_RESULT.GetValue());
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

    Result<Allocator::UniquePtr<IGPUDevice>, std::pmr::string> RenderSubsystem::CreateDevice() const
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
            return Result<Allocator::UniquePtr<IGPUDevice>, std::pmr::string>::Err(
                    GraphicsDetail::FormatError("SDL_GPU 디바이스 생성에 실패했습니다: {}", SDL_GetError()));
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
            return Result<Allocator::UniquePtr<IGPUDevice>, std::pmr::string>::Err(
                    GraphicsDetail::FormatError("기본 GPU 샘플러 생성에 실패했습니다: {}", SDL_GetError()));
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
            return Result<Allocator::UniquePtr<IGPUDevice>, std::pmr::string>::Err(
                    GraphicsDetail::FormatError("기본 폴백 GPU 텍스처 생성에 실패했습니다: {}", SDL_GetError()));
        }

        return Result<Allocator::UniquePtr<IGPUDevice>, std::pmr::string>::Ok(
                Allocator::MakeUnique<GPUDeviceImpl>(sdlDevice, defaultSampler, fallbackTexture));
    }
} // namespace Garage
