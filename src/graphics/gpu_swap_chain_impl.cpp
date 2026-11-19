#include "gpu_swap_chain_impl.hpp"

#include <string>
#include <utility>

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_video.h>

#include "garage/core/memory.hpp"
#include "garage/core/types.hpp"

#include "garage/graphics/gpu_command_buffer.hpp"
#include "garage/graphics/gpu_device.hpp"
#include "garage/graphics/gpu_target.hpp"
#include "garage/graphics/gpu_texture2d.hpp"
#include "garage/graphics/gpu_types.hpp"

#include "garage/platform/display.hpp"

#include "../platform/display_impl.hpp"

#include "gpu_command_buffer_impl.hpp"
#include "gpu_device_impl.hpp"
#include "gpu_sdl_conversions.hpp"
#include "gpu_texture2d_impl.hpp"

namespace Garage
{
    GPUSwapChainImpl::GPUSwapChainImpl(GPUDeviceImpl* device, const IDisplay* display) noexcept
        : device(device)
        , display(display)
    {
        if (this->device != nullptr && this->device->GetSDLDevice() != nullptr && this->display != nullptr)
        {
            const auto* displayImpl = static_cast<const DisplayImpl*>(this->display);
            SDL_Window* sdlWindow   = displayImpl->GetSDLWindow();
            if (sdlWindow != nullptr)
            {
                backBufferFormat = GraphicsDetail::FromSDLTextureFormat(
                        SDL_GetGPUSwapchainTextureFormat(this->device->GetSDLDevice(), sdlWindow));
            }
        }
    }

    GPUSwapChainImpl::~GPUSwapChainImpl() noexcept
    {
        Release();
    }

    void GPUSwapChainImpl::Present() const noexcept
    {
        // SDL_gpu는 스왑체인 텍스처를 획득한 명령 버퍼를 제출할 때 자동으로 프레젠트를 수행합니다.
    }

    void GPUSwapChainImpl::Release() noexcept
    {
        if (device != nullptr)
        {
            if (depthStencilTexture != nullptr)
            {
                device->DestroyTexture2D(depthStencilTexture);
            }

            if (device->GetSDLDevice() != nullptr && display != nullptr)
            {
                const auto* displayImpl = static_cast<const DisplayImpl*>(display);
                SDL_Window* sdlWindow   = displayImpl->GetSDLWindow();
                if (sdlWindow != nullptr)
                {
                    SDL_ReleaseWindowFromGPUDevice(device->GetSDLDevice(), sdlWindow);
                }
            }
        }

        depthStencilTexture = nullptr;
        backBufferTexture.reset();
        currentBackBufferTarget    = {};
        hasCurrentBackBufferTarget = false;
        display                    = nullptr;
        device                     = nullptr;
    }

    Result<const GPUTarget*, std::pmr::string>
            GPUSwapChainImpl::AcquireBackBufferTarget(IGPUCommandBuffer& commandBuffer)
    {
        if (device == nullptr || device->GetSDLDevice() == nullptr || display == nullptr)
        {
            return Result<const GPUTarget*, std::pmr::string>::Err(
                    GraphicsDetail::MakeError("스왑체인의 GPU 디바이스 또는 디스플레이가 유효하지 않습니다."));
        }

        const auto* displayImpl = static_cast<const DisplayImpl*>(display);
        SDL_Window* sdlWindow   = displayImpl->GetSDLWindow();
        if (sdlWindow == nullptr)
        {
            return Result<const GPUTarget*, std::pmr::string>::Err(
                    GraphicsDetail::MakeError("스왑체인의 SDL 윈도우 핸들이 유효하지 않습니다."));
        }

        auto&                 commandBufferImpl = static_cast<GPUCommandBufferImpl&>(commandBuffer);
        SDL_GPUCommandBuffer* sdlCommandBuffer  = commandBufferImpl.GetSDLCommandBuffer();
        if (sdlCommandBuffer == nullptr)
        {
            return Result<const GPUTarget*, std::pmr::string>::Err(
                    GraphicsDetail::MakeError("스왑체인 텍스처 획득에 필요한 명령 버퍼가 유효하지 않습니다."));
        }

        SDL_GPUTexture* swapchainTexture = nullptr;
        UInt32          width            = 0;
        UInt32          height           = 0;

        if (!SDL_WaitAndAcquireGPUSwapchainTexture(sdlCommandBuffer, sdlWindow, &swapchainTexture, &width, &height))
        {
            hasCurrentBackBufferTarget = false;
            return Result<const GPUTarget*, std::pmr::string>::Err(
                    GraphicsDetail::FormatError("SDL_GPU 스왑체인 텍스처 획득에 실패했습니다: {}", SDL_GetError()));
        }

        if (swapchainTexture == nullptr || width == 0 || height == 0)
        {
            hasCurrentBackBufferTarget = false;
            return Result<const GPUTarget*, std::pmr::string>::Ok(nullptr);
        }

        backBufferFormat = GraphicsDetail::FromSDLTextureFormat(
                SDL_GetGPUSwapchainTextureFormat(device->GetSDLDevice(), sdlWindow));

        if (backBufferTexture == nullptr)
        {
            backBufferTexture = MakeUnique<GPUTexture2DImpl>(device,
                                                             swapchainTexture,
                                                             width,
                                                             height,
                                                             1u,
                                                             1u,
                                                             backBufferFormat,
                                                             EGPUTextureType::TEXTURE_2D,
                                                             false);
        }
        else
        {
            backBufferTexture->ResetBorrowedHandle(swapchainTexture, width, height, backBufferFormat);
        }

        if (depthStencilTexture == nullptr || depthStencilTexture->GetWidth() != width ||
            depthStencilTexture->GetHeight() != height)
        {
            if (depthStencilTexture != nullptr)
            {
                device->DestroyTexture2D(depthStencilTexture);
                depthStencilTexture = nullptr;
            }

            GPUTexture2DCreateInfo depthInfo{};
            depthInfo.type       = EGPUTextureType::TEXTURE_2D;
            depthInfo.format     = GPUTarget::DEPTH_FORMAT;
            depthInfo.usage      = static_cast<UInt32>(EGPUTextureUsage::DEPTH_STENCIL_TARGET);
            depthInfo.width      = width;
            depthInfo.height     = height;
            depthInfo.layerCount = 1u;
            depthInfo.mipLevels  = 1u;

            auto depthResult = device->CreateTexture2D(depthInfo);
            if (!depthResult)
            {
                hasCurrentBackBufferTarget = false;
                return Result<const GPUTarget*, std::pmr::string>::Err(std::move(depthResult).GetError());
            }

            depthStencilTexture = depthResult.GetValue();
        }

        currentBackBufferTarget    = GPUTarget(GPUTargetCreateInfo{
                .texture             = backBufferTexture.get(),
                .depthStencilTexture = depthStencilTexture,
                .format              = backBufferFormat,
        });
        hasCurrentBackBufferTarget = true;

        return Result<const GPUTarget*, std::pmr::string>::Ok(&currentBackBufferTarget);
    }

    const IGPUDevice* GPUSwapChainImpl::GetDevice() const noexcept
    {
        return device;
    }
} // namespace Garage
