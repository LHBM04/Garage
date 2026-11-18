#include "gpu_command_buffer_impl.hpp"

#include <algorithm>
#include <array>
#include <cstring>
#include <limits>
#include <span>
#include <string>
#include <utility>

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>

#include "garage/core/allocator.hpp"
#include "garage/core/types.hpp"

#include "garage/graphics/gpu_buffer.hpp"
#include "garage/graphics/gpu_command_buffer.hpp"
#include "garage/graphics/gpu_device.hpp"
#include "garage/graphics/gpu_fence.hpp"
#include "garage/graphics/gpu_graphics_pipeline_state.hpp"
#include "garage/graphics/gpu_target.hpp"
#include "garage/graphics/gpu_texture2d.hpp"
#include "garage/graphics/gpu_types.hpp"

#include "garage/math/color_rgba.hpp"

#include "garage/utilities/logger.hpp"

#include "gpu_buffer_impl.hpp"
#include "gpu_device_impl.hpp"
#include "gpu_fence_impl.hpp"
#include "gpu_graphics_pipeline_state_impl.hpp"
#include "gpu_sdl_conversions.hpp"
#include "gpu_texture2d_impl.hpp"

namespace Garage
{
    GPUCommandBufferImpl::GPUCommandBufferImpl(const GPUDeviceImpl* device) noexcept
        : device(device)
    {
    }

    GPUCommandBufferImpl::~GPUCommandBufferImpl() noexcept
    {
        Release();
    }

    Result<void, std::pmr::string> GPUCommandBufferImpl::Reset()
    {
        if (device == nullptr || device->GetSDLDevice() == nullptr)
        {
            return Result<void, std::pmr::string>::Err(GraphicsDetail::MakeError("유효한 GPU 디바이스가 필요합니다."));
        }

        EndRenderPass();

        if (commandBuffer != nullptr)
        {
            SDL_CancelGPUCommandBuffer(commandBuffer);
            commandBuffer = nullptr;
        }

        hasActiveTarget    = false;
        activeTarget       = {};
        activeTargetBounds = {};
        activeViewport     = {};
        pendingScissorRect = {};
        hasPendingViewport = false;
        hasPendingScissor  = false;
        pendingClearColor  = false;
        pendingClearDepth  = false;
        boundPipeline      = nullptr;

        commandBuffer = SDL_AcquireGPUCommandBuffer(device->GetSDLDevice());
        if (commandBuffer == nullptr)
        {
            return Result<void, std::pmr::string>::Err(
                    GraphicsDetail::FormatError("SDL_GPU 명령 버퍼 획득에 실패했습니다: {}", SDL_GetError()));
        }

        return Result<void, std::pmr::string>::Ok();
    }

    Result<void, std::pmr::string> GPUCommandBufferImpl::Submit(IGPUFence* fence)
    {
        if (commandBuffer == nullptr)
        {
            return Result<void, std::pmr::string>::Err(GraphicsDetail::MakeError("제출할 GPU 명령 버퍼가 없습니다."));
        }

        EndRenderPass();

        std::pmr::string errorMessage(Allocator::GetMemoryResource());
        if (fence != nullptr)
        {
            auto*         fenceImpl = static_cast<GPUFenceImpl*>(fence);
            SDL_GPUFence* sdlFence  = SDL_SubmitGPUCommandBufferAndAcquireFence(commandBuffer);
            if (sdlFence != nullptr)
            {
                fenceImpl->SetSDLFence(sdlFence);
            }
            else
            {
                errorMessage = GraphicsDetail::FormatError("GPU 명령 버퍼 제출 및 펜스 획득에 실패했습니다: {}",
                                                           SDL_GetError());
            }
        }
        else if (!SDL_SubmitGPUCommandBuffer(commandBuffer))
        {
            errorMessage = GraphicsDetail::FormatError("GPU 명령 버퍼 제출에 실패했습니다: {}", SDL_GetError());
        }

        commandBuffer      = nullptr;
        hasActiveTarget    = false;
        activeTarget       = {};
        boundPipeline      = nullptr;
        hasPendingViewport = false;
        hasPendingScissor  = false;
        pendingClearColor  = false;
        pendingClearDepth  = false;

        if (!errorMessage.empty())
        {
            return Result<void, std::pmr::string>::Err(std::move(errorMessage));
        }

        return Result<void, std::pmr::string>::Ok();
    }

    Result<void, std::pmr::string> GPUCommandBufferImpl::SetGraphicsConstants(UInt32                rootSlot,
                                                                              std::span<const Byte> bytes)
    {
        if (commandBuffer == nullptr)
        {
            return Result<void, std::pmr::string>::Err(GraphicsDetail::MakeError("유효한 GPU 명령 버퍼가 필요합니다."));
        }

        if (bytes.empty())
        {
            return Result<void, std::pmr::string>::Err(
                    GraphicsDetail::MakeError("그래픽스 상수 버퍼 데이터가 비어 있습니다."));
        }

        if (bytes.size() > std::numeric_limits<UInt32>::max())
        {
            return Result<void, std::pmr::string>::Err(
                    GraphicsDetail::MakeError("그래픽스 상수 버퍼 크기가 32비트 범위를 초과했습니다."));
        }

        const UInt32 BYTE_SIZE = static_cast<UInt32>(bytes.size());
        SDL_PushGPUVertexUniformData(commandBuffer, rootSlot, bytes.data(), BYTE_SIZE);
        SDL_PushGPUFragmentUniformData(commandBuffer, rootSlot, bytes.data(), BYTE_SIZE);
        return Result<void, std::pmr::string>::Ok();
    }

    void GPUCommandBufferImpl::SetGraphicsRootConstants(UInt32                rootSlot,
                                                        std::span<const Byte> bytes,
                                                        UInt32                offset) noexcept
    {
        if (commandBuffer == nullptr || bytes.empty())
        {
            return;
        }

        if (offset > rootConstantsData.size() || bytes.size() > rootConstantsData.size() - offset)
        {
            LOG_ERROR("루트 상수 범위가 허용 크기({}바이트)를 초과했습니다.", rootConstantsData.size());
            return;
        }

        std::memcpy(rootConstantsData.data() + offset, bytes.data(), bytes.size());

        const UInt32 BYTE_SIZE = static_cast<UInt32>(rootConstantsData.size());
        SDL_PushGPUVertexUniformData(commandBuffer, rootSlot, rootConstantsData.data(), BYTE_SIZE);
        SDL_PushGPUFragmentUniformData(commandBuffer, rootSlot, rootConstantsData.data(), BYTE_SIZE);
    }

    void GPUCommandBufferImpl::SetGPUGraphicsPipelineState(const IGPUGraphicsPipelineState& pipelineState) noexcept
    {
        const auto& pipelineImpl = static_cast<const GPUGraphicsPipelineStateImpl&>(pipelineState);
        boundPipeline            = &pipelineImpl;

        if (renderPass != nullptr && pipelineImpl.GetSDLPipelineState() != nullptr)
        {
            SDL_BindGPUGraphicsPipeline(renderPass, pipelineImpl.GetSDLPipelineState());
        }
    }

    Result<void, std::pmr::string>
            GPUCommandBufferImpl::BindTextures(std::span<const IGPUTexture2D* const, TEXTURE_SLOT_COUNT> textures)
    {
        if (!EnsureRenderPass())
        {
            return Result<void, std::pmr::string>::Err(
                    GraphicsDetail::MakeError("텍스처 바인딩을 위한 렌더 패스를 시작할 수 없습니다."));
        }

        if (device == nullptr)
        {
            return Result<void, std::pmr::string>::Err(GraphicsDetail::MakeError("유효한 GPU 디바이스가 필요합니다."));
        }

        SDL_GPUSampler* defaultSampler  = device->GetDefaultSampler();
        SDL_GPUTexture* fallbackTexture = device->GetFallbackTexture();
        if (defaultSampler == nullptr || fallbackTexture == nullptr)
        {
            return Result<void, std::pmr::string>::Err(
                    GraphicsDetail::MakeError("텍스처 바인딩에 필요한 기본 샘플러 또는 폴백 텍스처가 없습니다."));
        }

        std::array<SDL_GPUTextureSamplerBinding, TEXTURE_SLOT_COUNT> bindings{};
        for (USize index = 0; index < TEXTURE_SLOT_COUNT; ++index)
        {
            const IGPUTexture2D* texture = textures[index];
            SDL_GPUTexture*      sdlTex  = fallbackTexture;
            if (texture != nullptr)
            {
                const auto* textureImpl = static_cast<const GPUTexture2DImpl*>(texture);
                if (textureImpl->GetSDLTexture() != nullptr)
                {
                    sdlTex = textureImpl->GetSDLTexture();
                }
            }

            bindings[index].texture = sdlTex;
            bindings[index].sampler = defaultSampler;
        }

        SDL_BindGPUFragmentSamplers(renderPass, 0u, bindings.data(), TEXTURE_SLOT_COUNT);
        return Result<void, std::pmr::string>::Ok();
    }

    void GPUCommandBufferImpl::CopyGPUBuffer(const IGPUBuffer& destination,
                                             UInt64            destinationOffset,
                                             const IGPUBuffer& source,
                                             UInt64            sourceOffset,
                                             UInt64            sizeInBytes) noexcept
    {
        if (commandBuffer == nullptr || sizeInBytes == 0)
        {
            return;
        }

        if (destinationOffset > std::numeric_limits<UInt32>::max() ||
            sourceOffset > std::numeric_limits<UInt32>::max() || sizeInBytes > std::numeric_limits<UInt32>::max())
        {
            LOG_ERROR("GPU 버퍼 복사 오프셋 또는 크기가 32비트 범위를 초과했습니다.");
            return;
        }

        const auto& destinationImpl = static_cast<const GPUBufferImpl&>(destination);
        const auto& sourceImpl      = static_cast<const GPUBufferImpl&>(source);
        if (destinationImpl.GetSDLBuffer() == nullptr || sourceImpl.GetSDLBuffer() == nullptr)
        {
            return;
        }

        EndRenderPass();

        SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(commandBuffer);
        if (copyPass == nullptr)
        {
            LOG_ERROR("GPU 복사 패스 시작에 실패했습니다: {}", SDL_GetError());
            return;
        }

        SDL_GPUBufferLocation sourceLocation{};
        sourceLocation.buffer = sourceImpl.GetSDLBuffer();
        sourceLocation.offset = static_cast<UInt32>(sourceOffset);

        SDL_GPUBufferLocation destinationLocation{};
        destinationLocation.buffer = destinationImpl.GetSDLBuffer();
        destinationLocation.offset = static_cast<UInt32>(destinationOffset);

        SDL_CopyGPUBufferToBuffer(
                copyPass, &sourceLocation, &destinationLocation, static_cast<UInt32>(sizeInBytes), false);
        SDL_EndGPUCopyPass(copyPass);
    }

    void GPUCommandBufferImpl::CopyBufferToTexture(const IGPUTexture2D& destination,
                                                   UInt32               mipLevel,
                                                   UInt32               layer,
                                                   const IGPUBuffer&    source,
                                                   UInt64               sourceOffset,
                                                   UInt32               pixelsPerRow,
                                                   UInt32               rowsPerLayer) noexcept
    {
        if (commandBuffer == nullptr || mipLevel >= destination.GetMipLevels() || layer >= destination.GetLayerCount())
        {
            return;
        }

        if (sourceOffset > std::numeric_limits<UInt32>::max() || sourceOffset >= source.GetSizeInBytes())
        {
            LOG_ERROR("텍스처 복사 소스 오프셋이 유효하지 않습니다.");
            return;
        }

        const auto&            destinationImpl = static_cast<const GPUTexture2DImpl&>(destination);
        const auto&            sourceImpl      = static_cast<const GPUBufferImpl&>(source);
        SDL_GPUTexture*        sdlTexture      = destinationImpl.GetSDLTexture();
        SDL_GPUTransferBuffer* sdlTransfer     = sourceImpl.GetSDLTransferBuffer();
        if (sdlTexture == nullptr || sdlTransfer == nullptr)
        {
            return;
        }

        EndRenderPass();

        SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(commandBuffer);
        if (copyPass == nullptr)
        {
            LOG_ERROR("GPU 복사 패스 시작에 실패했습니다: {}", SDL_GetError());
            return;
        }

        SDL_GPUTextureTransferInfo sourceInfo{};
        sourceInfo.transfer_buffer = sdlTransfer;
        sourceInfo.offset          = static_cast<UInt32>(sourceOffset);
        sourceInfo.pixels_per_row  = pixelsPerRow;
        sourceInfo.rows_per_layer  = rowsPerLayer;

        SDL_GPUTextureRegion destinationRegion{};
        destinationRegion.texture   = sdlTexture;
        destinationRegion.mip_level = mipLevel;
        destinationRegion.layer     = layer;
        destinationRegion.x         = 0u;
        destinationRegion.y         = 0u;
        destinationRegion.z         = 0u;
        destinationRegion.w         = std::max(1u, destination.GetWidth() >> mipLevel);
        destinationRegion.h         = std::max(1u, destination.GetHeight() >> mipLevel);
        destinationRegion.d         = 1u;

        SDL_UploadToGPUTexture(copyPass, &sourceInfo, &destinationRegion, false);
        SDL_EndGPUCopyPass(copyPass);
    }

    void GPUCommandBufferImpl::SetPrimitiveTopology([[maybe_unused]] EGPUPrimitiveTopology primitive) noexcept
    {
        // SDL_gpu에서는 프리미티브 토폴로지가 그래픽스 파이프라인 상태(PSO)에 포함됩니다.
    }

    void GPUCommandBufferImpl::SetViewport(const GPUViewport& viewport) noexcept
    {
        activeViewport = viewport;

        if (renderPass != nullptr)
        {
            const SDL_GPUViewport SDL_VIEWPORT{
                    .x         = viewport.x,
                    .y         = viewport.y,
                    .w         = viewport.width,
                    .h         = viewport.height,
                    .min_depth = viewport.minDepth,
                    .max_depth = viewport.maxDepth,
            };
            SDL_SetGPUViewport(renderPass, &SDL_VIEWPORT);
            hasPendingViewport = false;
        }
        else
        {
            hasPendingViewport = true;
        }
    }

    void GPUCommandBufferImpl::SetScissorRect(const GPURect& scissorRect) noexcept
    {
        pendingScissorRect = scissorRect;

        if (renderPass != nullptr)
        {
            const SDL_Rect SDL_SCISSOR{
                    .x = scissorRect.x,
                    .y = scissorRect.y,
                    .w = scissorRect.width,
                    .h = scissorRect.height,
            };
            SDL_SetGPUScissor(renderPass, &SDL_SCISSOR);
            hasPendingScissor = false;
        }
        else
        {
            hasPendingScissor = true;
        }
    }

    void GPUCommandBufferImpl::SetVertexBuffer(UInt32 slot, const GPUVertexBufferView& view) noexcept
    {
        if (view.buffer == nullptr || !EnsureRenderPass())
        {
            return;
        }

        const auto*    bufferImpl = static_cast<const GPUBufferImpl*>(view.buffer);
        SDL_GPUBuffer* sdlBuffer  = bufferImpl->GetSDLBuffer();
        if (sdlBuffer == nullptr)
        {
            return;
        }

        const SDL_GPUBufferBinding BINDING{
                .buffer = sdlBuffer,
                .offset = view.offset,
        };
        SDL_BindGPUVertexBuffers(renderPass, slot, &BINDING, 1u);
    }

    void GPUCommandBufferImpl::SetIndexBuffer(const GPUIndexBufferView& view) noexcept
    {
        if (view.buffer == nullptr || !EnsureRenderPass())
        {
            return;
        }

        const auto*    bufferImpl = static_cast<const GPUBufferImpl*>(view.buffer);
        SDL_GPUBuffer* sdlBuffer  = bufferImpl->GetSDLBuffer();
        if (sdlBuffer == nullptr)
        {
            return;
        }

        const SDL_GPUBufferBinding BINDING{
                .buffer = sdlBuffer,
                .offset = view.offset,
        };
        SDL_BindGPUIndexBuffer(renderPass, &BINDING, GraphicsDetail::ToSDLIndexElementSize(view.format));
    }

    void GPUCommandBufferImpl::Draw(UInt32 vertexCount, UInt32 firstVertex) noexcept
    {
        if (!EnsureRenderPass())
        {
            return;
        }

        SDL_DrawGPUPrimitives(renderPass, vertexCount, 1u, firstVertex, 0u);
    }

    void GPUCommandBufferImpl::DrawIndexed(UInt32 indexCount, UInt32 firstIndex, Int32 baseVertex) noexcept
    {
        if (!EnsureRenderPass())
        {
            return;
        }

        SDL_DrawGPUIndexedPrimitives(renderPass, indexCount, 1u, firstIndex, baseVertex, 0u);
    }

    void GPUCommandBufferImpl::DrawInstanced(UInt32 vertexCountPerInstance,
                                             UInt32 instanceCount,
                                             UInt32 firstVertex,
                                             UInt32 firstInstance) noexcept
    {
        if (!EnsureRenderPass())
        {
            return;
        }

        SDL_DrawGPUPrimitives(renderPass, vertexCountPerInstance, instanceCount, firstVertex, firstInstance);
    }

    void GPUCommandBufferImpl::DrawIndexedInstanced(UInt32 indexCountPerInstance,
                                                    UInt32 instanceCount,
                                                    UInt32 firstIndex,
                                                    Int32  baseVertex,
                                                    UInt32 firstInstance) noexcept
    {
        if (!EnsureRenderPass())
        {
            return;
        }

        SDL_DrawGPUIndexedPrimitives(
                renderPass, indexCountPerInstance, instanceCount, firstIndex, baseVertex, firstInstance);
    }

    void GPUCommandBufferImpl::ClearRenderTarget(const ColorRGBAFloat& color) noexcept
    {
        clearColor        = color;
        pendingClearColor = true;
    }

    void GPUCommandBufferImpl::ClearDepthStencil(Float32 depth) noexcept
    {
        clearDepth        = depth;
        pendingClearDepth = true;
    }

    void GPUCommandBufferImpl::SetGPUTarget(const GPUTarget& target) noexcept
    {
        EndRenderPass();

        activeTarget       = target;
        hasActiveTarget    = true;
        boundPipeline      = nullptr;
        hasPendingViewport = false;
        hasPendingScissor  = false;
        pendingClearColor  = false;
        pendingClearDepth  = false;

        if (const IGPUTexture2D* colorTexture = activeTarget.GetTexture(); colorTexture != nullptr)
        {
            const Int32 WIDTH  = static_cast<Int32>(colorTexture->GetWidth());
            const Int32 HEIGHT = static_cast<Int32>(colorTexture->GetHeight());

            activeTargetBounds = GPURect{
                    .x      = 0,
                    .y      = 0,
                    .width  = WIDTH,
                    .height = HEIGHT,
            };

            activeViewport = GPUViewport{
                    .x        = 0.0f,
                    .y        = 0.0f,
                    .width    = static_cast<Float32>(WIDTH),
                    .height   = static_cast<Float32>(HEIGHT),
                    .minDepth = 0.0f,
                    .maxDepth = 1.0f,
            };
        }
        else
        {
            activeTargetBounds = {};
            activeViewport     = {};
        }
    }

    void GPUCommandBufferImpl::EndRenderPass() noexcept
    {
        if (renderPass != nullptr)
        {
            SDL_EndGPURenderPass(renderPass);
            renderPass = nullptr;
        }
    }

    void GPUCommandBufferImpl::Release() noexcept
    {
        EndRenderPass();

        if (commandBuffer != nullptr)
        {
            SDL_CancelGPUCommandBuffer(commandBuffer);
            commandBuffer = nullptr;
        }

        hasActiveTarget = false;
        activeTarget    = {};
        boundPipeline   = nullptr;
        device          = nullptr;
    }

    const IGPUDevice* GPUCommandBufferImpl::GetDevice() const noexcept
    {
        return device;
    }

    EGPUTextureFormat GPUCommandBufferImpl::GetActiveTargetFormat() const noexcept
    {
        return hasActiveTarget ? activeTarget.GetFormat() : EGPUTextureFormat::UNKNOWN;
    }

    bool GPUCommandBufferImpl::EnsureRenderPass() noexcept
    {
        if (renderPass != nullptr)
        {
            return true;
        }

        if (commandBuffer == nullptr || !hasActiveTarget || activeTarget.GetTexture() == nullptr)
        {
            return false;
        }

        const auto*     colorTextureImpl = static_cast<const GPUTexture2DImpl*>(activeTarget.GetTexture());
        SDL_GPUTexture* sdlColorTexture  = colorTextureImpl->GetSDLTexture();
        if (sdlColorTexture == nullptr)
        {
            return false;
        }

        SDL_GPUColorTargetInfo colorTargetInfo{};
        colorTargetInfo.texture     = sdlColorTexture;
        colorTargetInfo.load_op     = pendingClearColor ? SDL_GPU_LOADOP_CLEAR : SDL_GPU_LOADOP_LOAD;
        colorTargetInfo.store_op    = SDL_GPU_STOREOP_STORE;
        colorTargetInfo.clear_color = SDL_FColor{
                clearColor.GetR(),
                clearColor.GetG(),
                clearColor.GetB(),
                clearColor.GetA(),
        };

        SDL_GPUDepthStencilTargetInfo depthTargetInfo{};
        bool                          hasDepthTarget = false;
        if (const IGPUTexture2D* depthTexture = activeTarget.GetDepthStencilTexture(); depthTexture != nullptr)
        {
            const auto*     depthTextureImpl = static_cast<const GPUTexture2DImpl*>(depthTexture);
            SDL_GPUTexture* sdlDepthTexture  = depthTextureImpl->GetSDLTexture();
            if (sdlDepthTexture != nullptr)
            {
                depthTargetInfo.texture          = sdlDepthTexture;
                depthTargetInfo.clear_depth      = clearDepth;
                depthTargetInfo.load_op          = pendingClearDepth ? SDL_GPU_LOADOP_CLEAR : SDL_GPU_LOADOP_LOAD;
                depthTargetInfo.store_op         = SDL_GPU_STOREOP_STORE;
                depthTargetInfo.stencil_load_op  = SDL_GPU_LOADOP_DONT_CARE;
                depthTargetInfo.stencil_store_op = SDL_GPU_STOREOP_DONT_CARE;
                depthTargetInfo.cycle            = pendingClearDepth;
                hasDepthTarget                   = true;
            }
        }

        renderPass = SDL_BeginGPURenderPass(
                commandBuffer, &colorTargetInfo, 1u, hasDepthTarget ? &depthTargetInfo : nullptr);
        if (renderPass == nullptr)
        {
            LOG_ERROR("SDL_GPU 렌더 패스 시작에 실패했습니다: {}", SDL_GetError());
            return false;
        }

        pendingClearColor = false;
        pendingClearDepth = false;

        if (boundPipeline != nullptr && boundPipeline->GetSDLPipelineState() != nullptr)
        {
            SDL_BindGPUGraphicsPipeline(renderPass, boundPipeline->GetSDLPipelineState());
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
            SDL_SetGPUViewport(renderPass, &SDL_VIEWPORT);
            hasPendingViewport = false;
        }

        if (hasPendingScissor)
        {
            const SDL_Rect SDL_SCISSOR{
                    .x = pendingScissorRect.x,
                    .y = pendingScissorRect.y,
                    .w = pendingScissorRect.width,
                    .h = pendingScissorRect.height,
            };
            SDL_SetGPUScissor(renderPass, &SDL_SCISSOR);
            hasPendingScissor = false;
        }

        return true;
    }
} // namespace Garage
