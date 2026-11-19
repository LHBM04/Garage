#include "gpu_texture2d_impl.hpp"

#include <SDL3/SDL_gpu.h>

#include "garage/core/common.hpp"

#include "garage/graphics/gpu_device.hpp"
#include "garage/graphics/gpu_types.hpp"

#include "gpu_device_impl.hpp"

namespace Garage
{
    GPUTexture2DImpl::GPUTexture2DImpl(const GPUDeviceImpl* device,
                                       SDL_GPUTexture*      texture,
                                       UInt32               width,
                                       UInt32               height,
                                       UInt32               layerCount,
                                       UInt32               mipLevels,
                                       EGPUTextureFormat    format,
                                       EGPUTextureType      type,
                                       bool                 ownsHandle) noexcept
        : device(device)
        , texture(texture)
        , width(width)
        , height(height)
        , layerCount(layerCount)
        , mipLevels(mipLevels)
        , format(format)
        , type(type)
        , ownsHandle(ownsHandle)
    {
    }

    GPUTexture2DImpl::~GPUTexture2DImpl() noexcept
    {
        Release();
    }

    void GPUTexture2DImpl::ResetBorrowedHandle(SDL_GPUTexture*   newTexture,
                                               UInt32            newWidth,
                                               UInt32            newHeight,
                                               EGPUTextureFormat newFormat) noexcept
    {
        ASSERT(!ownsHandle);

        texture = newTexture;
        width   = newWidth;
        height  = newHeight;
        format  = newFormat;
    }

    void GPUTexture2DImpl::Release() noexcept
    {
        if (ownsHandle && texture != nullptr && device != nullptr && device->GetSDLDevice() != nullptr)
        {
            SDL_ReleaseGPUTexture(device->GetSDLDevice(), texture);
        }

        texture = nullptr;
        device  = nullptr;
    }

    const IGPUDevice* GPUTexture2DImpl::GetDevice() const noexcept
    {
        return device;
    }
} // namespace Garage
