#include "garage/graphics/gpu_swap_chain.hpp"

#include <utility>

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_video.h>

#include "garage/core/defines.hpp"
#include "garage/core/types.hpp"

#include "garage/graphics/gpu_command_buffer.hpp"
#include "garage/graphics/gpu_target.hpp"
#include "garage/graphics/gpu_texture2d.hpp"
#include "garage/graphics/gpu_types.hpp"
#include "graphics/gpu_sdl_conversions.hpp"

#include "garage/utilities/logger.hpp"

namespace Garage
{
    GPUSwapChain::GPUSwapChain(const GPUSwapChainCreateInfo& createInfo) noexcept
        : deviceHandle(createInfo.deviceHandle)
        , windowHandle(createInfo.windowHandle)
        , display(createInfo.display)
        , backBufferFormat(createInfo.backBufferFormat)
        , depthStencilFormat(createInfo.depthStencilFormat)
    {
        ASSERT(this->deviceHandle != nullptr);
        ASSERT(this->windowHandle != nullptr);
        ASSERT(this->display != nullptr);

        if (createInfo.width > 0 && createInfo.height > 0)
        {
            static_cast<void>(RecreateDepthStencil(createInfo.width, createInfo.height));
        }
    }

    GPUSwapChain::~GPUSwapChain() noexcept
    {
        Release();
    }

    GPUSwapChain::GPUSwapChain(GPUSwapChain&& other) noexcept
        : deviceHandle(std::exchange(other.deviceHandle, nullptr))
        , windowHandle(std::exchange(other.windowHandle, nullptr))
        , display(std::exchange(other.display, nullptr))
        , backBufferFormat(other.backBufferFormat)
        , depthStencilFormat(other.depthStencilFormat)
        , backBufferTexture(std::move(other.backBufferTexture))
        , depthStencil(std::move(other.depthStencil))
        , hasValidTarget(std::exchange(other.hasValidTarget, false))
    {
        if (hasValidTarget)
        {
            currentTarget = GPUTarget(GPUTargetCreateInfo{
                    .texture             = &backBufferTexture,
                    .depthStencilTexture = depthStencil.GetNativeHandle() != nullptr ? &depthStencil : nullptr,
                    .format              = backBufferFormat,
            });
        }
    }

    GPUSwapChain& GPUSwapChain::operator=(GPUSwapChain&& other) noexcept
    {
        if (this != &other)
        {
            Release();
            deviceHandle       = std::exchange(other.deviceHandle, nullptr);
            windowHandle       = std::exchange(other.windowHandle, nullptr);
            display            = std::exchange(other.display, nullptr);
            backBufferFormat   = other.backBufferFormat;
            depthStencilFormat = other.depthStencilFormat;
            backBufferTexture  = std::move(other.backBufferTexture);
            depthStencil       = std::move(other.depthStencil);
            hasValidTarget     = std::exchange(other.hasValidTarget, false);

            if (hasValidTarget)
            {
                currentTarget = GPUTarget(GPUTargetCreateInfo{
                        .texture             = &backBufferTexture,
                        .depthStencilTexture = depthStencil.GetNativeHandle() != nullptr ? &depthStencil : nullptr,
                        .format              = backBufferFormat,
                });
            }
            else
            {
                currentTarget = GPUTarget{};
            }
        }

        return *this;
    }

    void GPUSwapChain::Present() const noexcept
    {
        ASSERT(deviceHandle != nullptr);
        ASSERT(windowHandle != nullptr);
    }

    const GPUTarget* GPUSwapChain::AcquireBackBufferTarget(GPUCommandBuffer& commandBuffer) noexcept
    {
        hasValidTarget = false;

        if (deviceHandle == nullptr || windowHandle == nullptr || commandBuffer.GetNativeCommandBuffer() == nullptr)
        {
            return nullptr;
        }

        auto* sdlCmd    = static_cast<SDL_GPUCommandBuffer*>(commandBuffer.GetNativeCommandBuffer());
        auto* sdlWindow = static_cast<SDL_Window*>(windowHandle);

        SDL_GPUTexture* swapchainTexture = nullptr;
        UInt32          width            = 0;
        UInt32          height           = 0;
        if (!SDL_WaitAndAcquireGPUSwapchainTexture(sdlCmd, sdlWindow, &swapchainTexture, &width, &height))
        {
            LOG_ERROR("GPU 스왑체인 텍스처 획득에 실패했습니다: {}", SDL_GetError());
            return nullptr;
        }

        if (swapchainTexture == nullptr || width == 0 || height == 0)
        {
            return nullptr;
        }

        if (depthStencil.GetNativeHandle() == nullptr || depthStencil.GetWidth() != width ||
            depthStencil.GetHeight() != height)
        {
            if (!RecreateDepthStencil(width, height))
            {
                return nullptr;
            }
        }

        backBufferTexture.ResetBorrowedHandle(swapchainTexture, backBufferFormat, width, height);
        currentTarget  = GPUTarget(GPUTargetCreateInfo{
                .texture             = &backBufferTexture,
                .depthStencilTexture = depthStencil.GetNativeHandle() != nullptr ? &depthStencil : nullptr,
                .format              = backBufferFormat,
        });
        hasValidTarget = true;

        return &currentTarget;
    }

    void GPUSwapChain::Release() noexcept
    {
        hasValidTarget    = false;
        currentTarget     = GPUTarget{};
        backBufferTexture = GPUTexture2D{};
        depthStencil      = GPUTexture2D{};

        if (deviceHandle != nullptr && windowHandle != nullptr)
        {
            SDL_ReleaseWindowFromGPUDevice(static_cast<SDL_GPUDevice*>(deviceHandle),
                                           static_cast<SDL_Window*>(windowHandle));
        }

        deviceHandle = nullptr;
        windowHandle = nullptr;
        display      = nullptr;
    }

    bool GPUSwapChain::RecreateDepthStencil(UInt32 newWidth, UInt32 newHeight) noexcept
    {
        if (deviceHandle == nullptr || newWidth == 0 || newHeight == 0)
        {
            return false;
        }

        auto* sdlDevice = static_cast<SDL_GPUDevice*>(deviceHandle);

        SDL_GPUTextureCreateInfo depthCreateInfo{};
        depthCreateInfo.type                 = SDL_GPU_TEXTURETYPE_2D;
        depthCreateInfo.format               = GraphicsDetail::ToSDLTextureFormat(depthStencilFormat);
        depthCreateInfo.usage                = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET;
        depthCreateInfo.width                = newWidth;
        depthCreateInfo.height               = newHeight;
        depthCreateInfo.layer_count_or_depth = 1;
        depthCreateInfo.num_levels           = 1;
        depthCreateInfo.sample_count         = SDL_GPU_SAMPLECOUNT_1;

        SDL_GPUTexture* depthTexture = SDL_CreateGPUTexture(sdlDevice, &depthCreateInfo);
        if (depthTexture == nullptr)
        {
            LOG_ERROR("깊이 버퍼 생성에 실패했습니다: {}", SDL_GetError());
            return false;
        }

        depthStencil = GPUTexture2D(GPUTexture2DHandleInfo{
                .deviceHandle  = deviceHandle,
                .textureHandle = depthTexture,
                .type          = EGPUTextureType::TEXTURE_2D,
                .format        = depthStencilFormat,
                .width         = newWidth,
                .height        = newHeight,
                .layerCount    = 1,
                .mipLevels     = 1,
                .ownsHandle    = true,
        });

        return true;
    }
} // namespace Garage
