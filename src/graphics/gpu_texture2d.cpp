#include "garage/graphics/gpu_texture2d.hpp"

#include <utility>

#include <SDL3/SDL_gpu.h>

#include "garage/core/defines.hpp"
#include "garage/core/types.hpp"

#include "garage/graphics/gpu_types.hpp"

namespace Garage
{
    GPUTexture2D::GPUTexture2D(const GPUTexture2DHandleInfo& handleInfo) noexcept
        : deviceHandle(handleInfo.deviceHandle)
        , textureHandle(handleInfo.textureHandle)
        , type(handleInfo.type)
        , format(handleInfo.format)
        , width(handleInfo.width)
        , height(handleInfo.height)
        , layerCount(handleInfo.layerCount)
        , mipLevels(handleInfo.mipLevels)
        , ownsHandle(handleInfo.ownsHandle)
    {
        ASSERT(this->textureHandle != nullptr);
    }

    GPUTexture2D::~GPUTexture2D() noexcept
    {
        Release();
    }

    GPUTexture2D::GPUTexture2D(GPUTexture2D&& other) noexcept
        : deviceHandle(std::exchange(other.deviceHandle, nullptr))
        , textureHandle(std::exchange(other.textureHandle, nullptr))
        , type(other.type)
        , format(other.format)
        , width(std::exchange(other.width, 0))
        , height(std::exchange(other.height, 0))
        , layerCount(std::exchange(other.layerCount, 1))
        , mipLevels(std::exchange(other.mipLevels, 1))
        , ownsHandle(std::exchange(other.ownsHandle, false))
    {
    }

    GPUTexture2D& GPUTexture2D::operator=(GPUTexture2D&& other) noexcept
    {
        if (this != &other)
        {
            Release();
            deviceHandle  = std::exchange(other.deviceHandle, nullptr);
            textureHandle = std::exchange(other.textureHandle, nullptr);
            type          = other.type;
            format        = other.format;
            width         = std::exchange(other.width, 0);
            height        = std::exchange(other.height, 0);
            layerCount    = std::exchange(other.layerCount, 1);
            mipLevels     = std::exchange(other.mipLevels, 1);
            ownsHandle    = std::exchange(other.ownsHandle, false);
        }

        return *this;
    }

    void GPUTexture2D::ResetBorrowedHandle(void*             newTextureHandle,
                                           EGPUTextureFormat newFormat,
                                           UInt32            newWidth,
                                           UInt32            newHeight) noexcept
    {
        Release();
        textureHandle = newTextureHandle;
        format        = newFormat;
        width         = newWidth;
        height        = newHeight;
        layerCount    = 1;
        mipLevels     = 1;
        ownsHandle    = false;
    }

    void GPUTexture2D::Release() noexcept
    {
        if (ownsHandle && deviceHandle != nullptr && textureHandle != nullptr)
        {
            SDL_ReleaseGPUTexture(static_cast<SDL_GPUDevice*>(deviceHandle),
                                  static_cast<SDL_GPUTexture*>(textureHandle));
        }

        textureHandle = nullptr;
        ownsHandle    = false;
    }
} // namespace Garage
