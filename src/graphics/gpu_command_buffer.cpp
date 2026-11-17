#include "garage/graphics/gpu_command_buffer.hpp"

#include <array>
#include <cstring>
#include <span>
#include <utility>

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>

#include "garage/core/defines.hpp"
#include "garage/core/types.hpp"

#include "garage/graphics/gpu_buffer.hpp"
#include "garage/graphics/gpu_device.hpp"
#include "garage/graphics/gpu_fence.hpp"
#include "garage/graphics/gpu_graphics_pipeline_state.hpp"
#include "garage/graphics/gpu_target.hpp"
#include "garage/graphics/gpu_texture2d.hpp"
#include "garage/graphics/gpu_types.hpp"
#include "graphics/gpu_sdl_conversions.hpp"

#include "garage/math/color_rgba.hpp"

#include "garage/utilities/logger.hpp"

namespace Garage
{
    GPUCommandBuffer::GPUCommandBuffer(const GPUCommandBufferCreateInfo& createInfo) noexcept
        : device(createInfo.device)
    {
        ASSERT(this->device != nullptr);
    }

    GPUCommandBuffer::~GPUCommandBuffer() noexcept
    {
        EndRenderPass();
        if (commandBufferHandle != nullptr)
        {
            SDL_CancelGPUCommandBuffer(static_cast<SDL_GPUCommandBuffer*>(commandBufferHandle));
            commandBufferHandle = nullptr;
        }
    }

    GPUCommandBuffer::GPUCommandBuffer(GPUCommandBuffer&& other) noexcept
        : device(std::exchange(other.device, nullptr))
        , commandBufferHandle(std::exchange(other.commandBufferHandle, nullptr))
        , renderPassHandle(std::exchange(other.renderPassHandle, nullptr))
        , activeTarget(std::exchange(other.activeTarget, nullptr))
        , activeTargetFormat(other.activeTargetFormat)
        , activeTargetBounds(other.activeTargetBounds)
        , activeViewport(other.activeViewport)
        , activeScissor(other.activeScissor)
        , activeTopology(other.activeTopology)
        , clearColor(other.clearColor)
        , clearDepth(other.clearDepth)
        , shouldClearColor(other.shouldClearColor)
        , shouldClearDepth(other.shouldClearDepth)
        , hasPendingViewport(other.hasPendingViewport)
        , hasPendingScissor(other.hasPendingScissor)
        , rootConstants(other.rootConstants)
    {
    }

    GPUCommandBuffer& GPUCommandBuffer::operator=(GPUCommandBuffer&& other) noexcept
    {
        if (this != &other)
        {
            EndRenderPass();
            if (commandBufferHandle != nullptr)
            {
                SDL_CancelGPUCommandBuffer(static_cast<SDL_GPUCommandBuffer*>(commandBufferHandle));
            }

            device              = std::exchange(other.device, nullptr);
            commandBufferHandle = std::exchange(other.commandBufferHandle, nullptr);
            renderPassHandle    = std::exchange(other.renderPassHandle, nullptr);
            activeTarget        = std::exchange(other.activeTarget, nullptr);
            activeTargetFormat  = other.activeTargetFormat;
            activeTargetBounds  = other.activeTargetBounds;
            activeViewport      = other.activeViewport;
            activeScissor       = other.activeScissor;
            activeTopology      = other.activeTopology;
            clearColor          = other.clearColor;
            clearDepth          = other.clearDepth;
            shouldClearColor    = other.shouldClearColor;
            shouldClearDepth    = other.shouldClearDepth;
            hasPendingViewport  = other.hasPendingViewport;
            hasPendingScissor   = other.hasPendingScissor;
            rootConstants       = other.rootConstants;
        }

        return *this;
    }

    bool GPUCommandBuffer::Reset() noexcept
    {
        ASSERT(device != nullptr);

        EndRenderPass();
        if (commandBufferHandle != nullptr)
        {
            SDL_CancelGPUCommandBuffer(static_cast<SDL_GPUCommandBuffer*>(commandBufferHandle));
            commandBufferHandle = nullptr;
        }

        auto* sdlDevice     = static_cast<SDL_GPUDevice*>(device->GetNativeDevice());
        commandBufferHandle = SDL_AcquireGPUCommandBuffer(sdlDevice);
        if (commandBufferHandle == nullptr)
        {
            LOG_ERROR("GPU 명령 버퍼 획득에 실패했습니다: {}", SDL_GetError());
            return false;
        }

        activeTarget       = nullptr;
        activeTargetFormat = EGPUTextureFormat::UNKNOWN;
        activeTargetBounds = {};
        activeViewport     = {};
        activeScissor      = {};
        activeTopology     = EGPUPrimitiveTopology::TRIANGLE_LIST;
        clearColor         = ColorRGBAFloat::GetBlack();
        clearDepth         = GPUTarget::DEPTH_CLEAR_VALUE;
        shouldClearColor   = false;
        shouldClearDepth   = false;
        hasPendingViewport = false;
        hasPendingScissor  = false;
        rootConstants.fill(0);

        return true;
    }

    bool GPUCommandBuffer::Submit(GPUFence* fence) noexcept
    {
        if (commandBufferHandle == nullptr)
        {
            LOG_ERROR("제출할 GPU 명령 버퍼가 없습니다.");
            return false;
        }

        EndRenderPass();

        auto* sdlCmd        = static_cast<SDL_GPUCommandBuffer*>(commandBufferHandle);
        commandBufferHandle = nullptr;

        if (fence != nullptr)
        {
            SDL_GPUFence* sdlFence = SDL_SubmitGPUCommandBufferAndAcquireFence(sdlCmd);
            if (sdlFence == nullptr)
            {
                LOG_ERROR("GPU 명령 버퍼 제출 및 펜스 획득에 실패했습니다: {}", SDL_GetError());
                return false;
            }

            fence->BindNativeFence(sdlFence);
            return true;
        }

        if (!SDL_SubmitGPUCommandBuffer(sdlCmd))
        {
            LOG_ERROR("GPU 명령 버퍼 제출에 실패했습니다: {}", SDL_GetError());
            return false;
        }

        return true;
    }

    bool GPUCommandBuffer::SetGraphicsConstants(UInt32 rootSlot, std::span<const Byte> bytes) noexcept
    {
        if (commandBufferHandle == nullptr || bytes.empty())
        {
            LOG_ERROR("GPU 명령 버퍼 또는 상수 버퍼 데이터가 유효하지 않습니다.");
            return false;
        }

        auto*        sdlCmd = static_cast<SDL_GPUCommandBuffer*>(commandBufferHandle);
        const UInt32 LENGTH = static_cast<UInt32>(bytes.size());

        SDL_PushGPUVertexUniformData(sdlCmd, rootSlot, bytes.data(), LENGTH);
        SDL_PushGPUFragmentUniformData(sdlCmd, rootSlot, bytes.data(), LENGTH);
        return true;
    }

    void GPUCommandBuffer::SetGraphicsRootConstants(UInt32                rootSlot,
                                                    std::span<const Byte> bytes,
                                                    UInt32                offset) noexcept
    {
        ASSERT(commandBufferHandle != nullptr);
        ASSERT(rootSlot == ROOT_CONSTANT_SLOT);
        ASSERT(bytes.size() % sizeof(UInt32) == 0);
        ASSERT(offset <= ROOT_CONSTANT_COUNT);
        ASSERT(bytes.size() / sizeof(UInt32) <= ROOT_CONSTANT_COUNT - offset);

        std::memcpy(rootConstants.data() + offset, bytes.data(), bytes.size());

        auto* sdlCmd = static_cast<SDL_GPUCommandBuffer*>(commandBufferHandle);
        SDL_PushGPUFragmentUniformData(
                sdlCmd, rootSlot, rootConstants.data(), static_cast<UInt32>(sizeof(rootConstants)));
    }

    void GPUCommandBuffer::SetGPUGraphicsPipelineState(const GPUGraphicsPipelineState& pipelineState) noexcept
    {
        if (!EnsureRenderPass())
        {
            return;
        }

        SDL_BindGPUGraphicsPipeline(static_cast<SDL_GPURenderPass*>(renderPassHandle),
                                    static_cast<SDL_GPUGraphicsPipeline*>(pipelineState.GetNativeHandle()));
    }

    bool GPUCommandBuffer::BindTextures(std::span<const GPUTexture2D* const, TEXTURE_SLOT_COUNT> textures) noexcept
    {
        ASSERT(device != nullptr);
        if (!EnsureRenderPass())
        {
            return false;
        }

        auto* defaultSampler  = static_cast<SDL_GPUSampler*>(device->GetDefaultSampler());
        auto* fallbackTexture = static_cast<SDL_GPUTexture*>(device->GetFallbackTexture());
        if (defaultSampler == nullptr || fallbackTexture == nullptr)
        {
            LOG_ERROR("기본 GPU 샘플러 또는 폴백 텍스처가 초기화되지 않았습니다.");
            return false;
        }

        std::array<SDL_GPUTextureSamplerBinding, TEXTURE_SLOT_COUNT> bindings{};
        for (USize index = 0; index < TEXTURE_SLOT_COUNT; ++index)
        {
            const GPUTexture2D* texture    = textures[index];
            auto*               sdlTexture = (texture != nullptr && texture->GetNativeHandle() != nullptr)
                    ? static_cast<SDL_GPUTexture*>(texture->GetNativeHandle())
                    : fallbackTexture;
            bindings[index]                = SDL_GPUTextureSamplerBinding{
                    .texture = sdlTexture,
                    .sampler = defaultSampler,
            };
        }

        SDL_BindGPUFragmentSamplers(static_cast<SDL_GPURenderPass*>(renderPassHandle),
                                    0,
                                    bindings.data(),
                                    static_cast<UInt32>(bindings.size()));
        return true;
    }

    void GPUCommandBuffer::CopyGPUBuffer(const GPUBuffer& destination,
                                         UInt64           destinationOffset,
                                         const GPUBuffer& source,
                                         UInt64           sourceOffset,
                                         UInt64           sizeInBytes) noexcept
    {
        ASSERT(commandBufferHandle != nullptr);
        ASSERT(destinationOffset <= destination.GetSizeInBytes());
        ASSERT(sizeInBytes <= destination.GetSizeInBytes() - destinationOffset);
        ASSERT(sourceOffset <= source.GetSizeInBytes());
        ASSERT(sizeInBytes <= source.GetSizeInBytes() - sourceOffset);

        if (sizeInBytes == 0)
        {
            return;
        }

        EndRenderPass();

        auto*            sdlCmd   = static_cast<SDL_GPUCommandBuffer*>(commandBufferHandle);
        SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(sdlCmd);
        if (copyPass == nullptr)
        {
            LOG_ERROR("버퍼 복사를 위한 복사 패스 시작에 실패했습니다: {}", SDL_GetError());
            return;
        }

        if (source.GetUsage() == EGPUBufferUsage::UPLOAD && source.GetNativeTransferBuffer() != nullptr &&
            destination.GetNativeBuffer() != nullptr)
        {
            const SDL_GPUTransferBufferLocation SRC_LOC{
                    .transfer_buffer = static_cast<SDL_GPUTransferBuffer*>(source.GetNativeTransferBuffer()),
                    .offset          = static_cast<UInt32>(sourceOffset),
            };
            const SDL_GPUBufferRegion DST_REGION{
                    .buffer = static_cast<SDL_GPUBuffer*>(destination.GetNativeBuffer()),
                    .offset = static_cast<UInt32>(destinationOffset),
                    .size   = static_cast<UInt32>(sizeInBytes),
            };
            SDL_UploadToGPUBuffer(copyPass, &SRC_LOC, &DST_REGION, false);
        }
        else if (source.GetNativeBuffer() != nullptr && destination.GetNativeBuffer() != nullptr)
        {
            const SDL_GPUBufferLocation SRC_LOC{
                    .buffer = static_cast<SDL_GPUBuffer*>(source.GetNativeBuffer()),
                    .offset = static_cast<UInt32>(sourceOffset),
            };
            const SDL_GPUBufferLocation DST_LOC{
                    .buffer = static_cast<SDL_GPUBuffer*>(destination.GetNativeBuffer()),
                    .offset = static_cast<UInt32>(destinationOffset),
            };
            SDL_CopyGPUBufferToBuffer(copyPass, &SRC_LOC, &DST_LOC, static_cast<UInt32>(sizeInBytes), false);
        }

        SDL_EndGPUCopyPass(copyPass);
    }

    void GPUCommandBuffer::SetPrimitiveTopology(EGPUPrimitiveTopology primitive) noexcept
    {
        activeTopology = primitive;
    }

    void GPUCommandBuffer::SetViewport(const GPUViewport& viewport) noexcept
    {
        activeViewport     = viewport;
        hasPendingViewport = true;

        if (renderPassHandle != nullptr)
        {
            const SDL_GPUViewport SDL_VIEWPORT{
                    .x         = viewport.x,
                    .y         = viewport.y,
                    .w         = viewport.width,
                    .h         = viewport.height,
                    .min_depth = viewport.minDepth,
                    .max_depth = viewport.maxDepth,
            };
            SDL_SetGPUViewport(static_cast<SDL_GPURenderPass*>(renderPassHandle), &SDL_VIEWPORT);
            hasPendingViewport = false;
        }
    }

    void GPUCommandBuffer::SetScissorRect(const GPURect& scissorRect) noexcept
    {
        activeScissor     = scissorRect;
        hasPendingScissor = true;

        if (renderPassHandle != nullptr)
        {
            const SDL_Rect SDL_SCISSOR{
                    .x = scissorRect.x,
                    .y = scissorRect.y,
                    .w = scissorRect.width,
                    .h = scissorRect.height,
            };
            SDL_SetGPUScissor(static_cast<SDL_GPURenderPass*>(renderPassHandle), &SDL_SCISSOR);
            hasPendingScissor = false;
        }
    }

    void GPUCommandBuffer::SetVertexBuffer(UInt32 slot, const GPUVertexBufferView& view) noexcept
    {
        if (view.buffer == nullptr || view.buffer->GetNativeBuffer() == nullptr || !EnsureRenderPass())
        {
            return;
        }

        const SDL_GPUBufferBinding BINDING{
                .buffer = static_cast<SDL_GPUBuffer*>(view.buffer->GetNativeBuffer()),
                .offset = view.offset,
        };
        SDL_BindGPUVertexBuffers(static_cast<SDL_GPURenderPass*>(renderPassHandle), slot, &BINDING, 1);
    }

    void GPUCommandBuffer::SetIndexBuffer(const GPUIndexBufferView& view) noexcept
    {
        if (view.buffer == nullptr || view.buffer->GetNativeBuffer() == nullptr || !EnsureRenderPass())
        {
            return;
        }

        const SDL_GPUBufferBinding BINDING{
                .buffer = static_cast<SDL_GPUBuffer*>(view.buffer->GetNativeBuffer()),
                .offset = view.offset,
        };
        SDL_BindGPUIndexBuffer(static_cast<SDL_GPURenderPass*>(renderPassHandle),
                               &BINDING,
                               GraphicsDetail::ToSDLIndexElementSize(view.format));
    }

    void GPUCommandBuffer::Draw(UInt32 vertexCount, UInt32 firstVertex) noexcept
    {
        DrawInstanced(vertexCount, 1, firstVertex, 0);
    }

    void GPUCommandBuffer::DrawIndexed(UInt32 indexCount, UInt32 firstIndex, Int32 baseVertex) noexcept
    {
        DrawIndexedInstanced(indexCount, 1, firstIndex, baseVertex, 0);
    }

    void GPUCommandBuffer::DrawInstanced(UInt32 vertexCountPerInstance,
                                         UInt32 instanceCount,
                                         UInt32 firstVertex,
                                         UInt32 firstInstance) noexcept
    {
        if (!EnsureRenderPass())
        {
            return;
        }

        SDL_DrawGPUPrimitives(static_cast<SDL_GPURenderPass*>(renderPassHandle),
                              vertexCountPerInstance,
                              instanceCount,
                              firstVertex,
                              firstInstance);
    }

    void GPUCommandBuffer::DrawIndexedInstanced(UInt32 indexCountPerInstance,
                                                UInt32 instanceCount,
                                                UInt32 firstIndex,
                                                Int32  baseVertex,
                                                UInt32 firstInstance) noexcept
    {
        if (!EnsureRenderPass())
        {
            return;
        }

        SDL_DrawGPUIndexedPrimitives(static_cast<SDL_GPURenderPass*>(renderPassHandle),
                                     indexCountPerInstance,
                                     instanceCount,
                                     firstIndex,
                                     baseVertex,
                                     firstInstance);
    }

    void GPUCommandBuffer::ClearRenderTarget(const ColorRGBAFloat& color) noexcept
    {
        clearColor       = color;
        shouldClearColor = true;
    }

    void GPUCommandBuffer::ClearDepthStencil(Float32 depth) noexcept
    {
        clearDepth       = depth;
        shouldClearDepth = true;
    }

    void GPUCommandBuffer::SetGPUTarget(const GPUTarget& target) noexcept
    {
        EndRenderPass();

        activeTarget       = &target;
        activeTargetFormat = target.GetFormat();

        const GPUTexture2D* colorTexture = target.GetTexture();
        if (colorTexture != nullptr)
        {
            activeTargetBounds = GPURect{
                    .x      = 0,
                    .y      = 0,
                    .width  = static_cast<Int32>(colorTexture->GetWidth()),
                    .height = static_cast<Int32>(colorTexture->GetHeight()),
            };
            activeViewport = GPUViewport{
                    .x        = 0.0f,
                    .y        = 0.0f,
                    .width    = static_cast<Float32>(colorTexture->GetWidth()),
                    .height   = static_cast<Float32>(colorTexture->GetHeight()),
                    .minDepth = 0.0f,
                    .maxDepth = 1.0f,
            };
        }
        else
        {
            activeTargetBounds = {};
            activeViewport     = {};
        }

        shouldClearColor   = false;
        shouldClearDepth   = false;
        hasPendingViewport = false;
        hasPendingScissor  = false;
    }

    void GPUCommandBuffer::EndRenderPass() noexcept
    {
        if (renderPassHandle != nullptr)
        {
            SDL_EndGPURenderPass(static_cast<SDL_GPURenderPass*>(renderPassHandle));
            renderPassHandle = nullptr;
        }
    }

    bool GPUCommandBuffer::EnsureRenderPass() noexcept
    {
        if (renderPassHandle != nullptr)
        {
            return true;
        }

        if (commandBufferHandle == nullptr || activeTarget == nullptr || activeTarget->GetTexture() == nullptr ||
            activeTarget->GetTexture()->GetNativeHandle() == nullptr)
        {
            return false;
        }

        SDL_GPUColorTargetInfo colorTargetInfo{};
        colorTargetInfo.texture     = static_cast<SDL_GPUTexture*>(activeTarget->GetTexture()->GetNativeHandle());
        colorTargetInfo.clear_color = SDL_FColor{
                .r = clearColor.GetR(),
                .g = clearColor.GetG(),
                .b = clearColor.GetB(),
                .a = clearColor.GetA(),
        };
        colorTargetInfo.load_op  = shouldClearColor ? SDL_GPU_LOADOP_CLEAR : SDL_GPU_LOADOP_LOAD;
        colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;

        SDL_GPUDepthStencilTargetInfo        depthTargetInfo{};
        const SDL_GPUDepthStencilTargetInfo* depthTargetPtr = nullptr;
        if (const GPUTexture2D* depthTexture = activeTarget->GetDepthStencilTexture();
            depthTexture != nullptr && depthTexture->GetNativeHandle() != nullptr)
        {
            depthTargetInfo.texture          = static_cast<SDL_GPUTexture*>(depthTexture->GetNativeHandle());
            depthTargetInfo.clear_depth      = clearDepth;
            depthTargetInfo.load_op          = shouldClearDepth ? SDL_GPU_LOADOP_CLEAR : SDL_GPU_LOADOP_LOAD;
            depthTargetInfo.store_op         = SDL_GPU_STOREOP_STORE;
            depthTargetInfo.stencil_load_op  = SDL_GPU_LOADOP_DONT_CARE;
            depthTargetInfo.stencil_store_op = SDL_GPU_STOREOP_DONT_CARE;
            depthTargetInfo.cycle            = shouldClearDepth;
            depthTargetPtr                   = &depthTargetInfo;
        }

        renderPassHandle = SDL_BeginGPURenderPass(
                static_cast<SDL_GPUCommandBuffer*>(commandBufferHandle), &colorTargetInfo, 1, depthTargetPtr);
        shouldClearColor = false;
        shouldClearDepth = false;

        if (renderPassHandle == nullptr)
        {
            return false;
        }

        if (hasPendingViewport)
        {
            const SDL_GPUViewport SDL_VIEWPORT{
                    .x         = activeViewport.x,
                    .y         = activeViewport.y,
                    .w         = activeViewport.width,
                    .h         = activeViewport.height,
                    .min_depth = activeViewport.minDepth,
                    .max_depth = activeViewport.maxDepth,
            };
            SDL_SetGPUViewport(static_cast<SDL_GPURenderPass*>(renderPassHandle), &SDL_VIEWPORT);
            hasPendingViewport = false;
        }

        if (hasPendingScissor)
        {
            const SDL_Rect SDL_SCISSOR{
                    .x = activeScissor.x,
                    .y = activeScissor.y,
                    .w = activeScissor.width,
                    .h = activeScissor.height,
            };
            SDL_SetGPUScissor(static_cast<SDL_GPURenderPass*>(renderPassHandle), &SDL_SCISSOR);
            hasPendingScissor = false;
        }

        return true;
    }
} // namespace Garage
