#include "gpu_device_impl.hpp"

#include <algorithm>
#include <cstring>
#include <limits>
#include <memory_resource>
#include <span>
#include <string>
#include <utility>
#include <vector>

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_video.h>

#include "garage/core/memory.hpp"
#include "garage/core/types.hpp"

#include "garage/graphics/gpu_buffer.hpp"
#include "garage/graphics/gpu_command_buffer.hpp"
#include "garage/graphics/gpu_device.hpp"
#include "garage/graphics/gpu_fence.hpp"
#include "garage/graphics/gpu_graphics_pipeline_state.hpp"
#include "garage/graphics/gpu_mesh.hpp"
#include "garage/graphics/gpu_shader.hpp"
#include "garage/graphics/gpu_swap_chain.hpp"
#include "garage/graphics/gpu_texture2d.hpp"
#include "garage/graphics/gpu_types.hpp"

#include "garage/platform/display.hpp"

#include "../platform/display_impl.hpp"

#include "gpu_buffer_impl.hpp"
#include "gpu_command_buffer_impl.hpp"
#include "gpu_fence_impl.hpp"
#include "gpu_graphics_pipeline_state_impl.hpp"
#include "gpu_mesh_impl.hpp"
#include "gpu_sdl_conversions.hpp"
#include "gpu_shader_impl.hpp"
#include "gpu_swap_chain_impl.hpp"
#include "gpu_texture2d_impl.hpp"

namespace Garage
{
    namespace
    {
        template <typename TImpl, typename TInterface>
        void DestroyOwnedResource(std::pmr::vector<UniquePtr<TImpl>>& resources, const TInterface* target) noexcept
        {
            if (target == nullptr)
            {
                return;
            }

            std::erase_if(resources,
                          [target](const UniquePtr<TImpl>& item) noexcept
                          {
                              return item.get() == target;
                          });
        }
    } // namespace

    GPUDeviceImpl::GPUDeviceImpl(SDL_GPUDevice*  device,
                                 SDL_GPUSampler* defaultSampler,
                                 SDL_GPUTexture* fallbackTexture) noexcept
        : device(device)
        , defaultSampler(defaultSampler)
        , fallbackTexture(fallbackTexture)
    {
    }

    GPUDeviceImpl::~GPUDeviceImpl() noexcept
    {
        Release();
    }

    Result<IGPUBuffer*, std::pmr::string> GPUDeviceImpl::CreateBuffer(EGPUBufferUsage usage, UInt64 sizeInBytes)
    {
        if (device == nullptr)
        {
            return Result<IGPUBuffer*, std::pmr::string>::Err(
                    GraphicsDetail::MakeError("유효한 SDL_GPU 디바이스가 필요합니다."));
        }

        if (sizeInBytes == 0 || sizeInBytes > std::numeric_limits<UInt32>::max())
        {
            return Result<IGPUBuffer*, std::pmr::string>::Err(
                    GraphicsDetail::MakeError("GPU 버퍼 크기가 유효하지 않습니다."));
        }

        const UInt32 SIZE_U32 = static_cast<UInt32>(sizeInBytes);

        if (usage == EGPUBufferUsage::UPLOAD || usage == EGPUBufferUsage::READBACK)
        {
            SDL_GPUTransferBufferCreateInfo transferInfo{};
            transferInfo.usage = (usage == EGPUBufferUsage::UPLOAD) ? SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD
                                                                    : SDL_GPU_TRANSFERBUFFERUSAGE_DOWNLOAD;
            transferInfo.size  = SIZE_U32;

            SDL_GPUTransferBuffer* transferBuffer = SDL_CreateGPUTransferBuffer(device, &transferInfo);
            if (transferBuffer == nullptr)
            {
                return Result<IGPUBuffer*, std::pmr::string>::Err(
                        GraphicsDetail::FormatError("SDL_GPU 전송 버퍼 생성에 실패했습니다: {}", SDL_GetError()));
            }

            auto           bufferImpl = MakeUnique<GPUBufferImpl>(this, usage, sizeInBytes, nullptr, transferBuffer);
            GPUBufferImpl* result     = bufferImpl.get();
            buffers.push_back(std::move(bufferImpl));
            return Result<IGPUBuffer*, std::pmr::string>::Ok(result);
        }

        SDL_GPUBufferCreateInfo bufferInfo{};
        bufferInfo.usage = (usage == EGPUBufferUsage::INDEX) ? SDL_GPU_BUFFERUSAGE_INDEX : SDL_GPU_BUFFERUSAGE_VERTEX;
        bufferInfo.size  = SIZE_U32;

        SDL_GPUBuffer* sdlBuffer = SDL_CreateGPUBuffer(device, &bufferInfo);
        if (sdlBuffer == nullptr)
        {
            return Result<IGPUBuffer*, std::pmr::string>::Err(
                    GraphicsDetail::FormatError("SDL_GPU 버퍼 생성에 실패했습니다: {}", SDL_GetError()));
        }

        auto           bufferImpl = MakeUnique<GPUBufferImpl>(this, usage, sizeInBytes, sdlBuffer, nullptr);
        GPUBufferImpl* result     = bufferImpl.get();
        buffers.push_back(std::move(bufferImpl));
        return Result<IGPUBuffer*, std::pmr::string>::Ok(result);
    }

    Result<IGPUBuffer*, std::pmr::string> GPUDeviceImpl::CreateUploadBuffer(std::span<const Byte> data)
    {
        if (data.empty())
        {
            return Result<IGPUBuffer*, std::pmr::string>::Err(
                    GraphicsDetail::MakeError("업로드 버퍼에 기록할 데이터가 비어 있습니다."));
        }

        auto bufferResult = CreateBuffer(EGPUBufferUsage::UPLOAD, data.size());
        if (!bufferResult)
        {
            return Result<IGPUBuffer*, std::pmr::string>::Err(std::move(bufferResult).GetError());
        }

        IGPUBuffer* uploadBuffer = bufferResult.GetValue();
        if (auto writeResult = uploadBuffer->Write(data, 0); !writeResult)
        {
            DestroyBuffer(uploadBuffer);
            return Result<IGPUBuffer*, std::pmr::string>::Err(std::move(writeResult).GetError());
        }

        return Result<IGPUBuffer*, std::pmr::string>::Ok(uploadBuffer);
    }

    void GPUDeviceImpl::DestroyBuffer(IGPUBuffer* buffer) noexcept
    {
        DestroyOwnedResource(buffers, buffer);
    }

    Result<IGPUShader*, std::pmr::string> GPUDeviceImpl::CreateShader(const GPUShaderCreateInfo& createInfo)
    {
        if (device == nullptr)
        {
            return Result<IGPUShader*, std::pmr::string>::Err(
                    GraphicsDetail::MakeError("유효한 SDL_GPU 디바이스가 필요합니다."));
        }

        if (createInfo.code.empty() || createInfo.format == EGPUShaderFormat::INVALID)
        {
            return Result<IGPUShader*, std::pmr::string>::Err(
                    GraphicsDetail::MakeError("셰이더 바이트코드 또는 형식이 유효하지 않습니다."));
        }

        const std::pmr::string ENTRY_POINT(createInfo.entryPoint, Memory::GetMemoryResource());

        SDL_GPUShaderCreateInfo sdlInfo{};
        sdlInfo.code                 = reinterpret_cast<const UInt8*>(createInfo.code.data());
        sdlInfo.code_size            = createInfo.code.size();
        sdlInfo.entrypoint           = ENTRY_POINT.c_str();
        sdlInfo.format               = GraphicsDetail::ToSDLShaderFormat(createInfo.format);
        sdlInfo.stage                = GraphicsDetail::ToSDLShaderStage(createInfo.stage);
        sdlInfo.num_samplers         = createInfo.samplerCount;
        sdlInfo.num_storage_textures = createInfo.storageTextureCount;
        sdlInfo.num_storage_buffers  = createInfo.storageBufferCount;
        sdlInfo.num_uniform_buffers  = createInfo.uniformBufferCount;

        SDL_GPUShader* sdlShader = SDL_CreateGPUShader(device, &sdlInfo);
        if (sdlShader == nullptr)
        {
            return Result<IGPUShader*, std::pmr::string>::Err(
                    GraphicsDetail::FormatError("SDL_GPU 셰이더 생성에 실패했습니다: {}", SDL_GetError()));
        }

        auto           shaderImpl = MakeUnique<GPUShaderImpl>(this, sdlShader, createInfo.stage, createInfo.format);
        GPUShaderImpl* result     = shaderImpl.get();
        shaders.push_back(std::move(shaderImpl));
        return Result<IGPUShader*, std::pmr::string>::Ok(result);
    }

    void GPUDeviceImpl::DestroyShader(IGPUShader* shader) noexcept
    {
        DestroyOwnedResource(shaders, shader);
    }

    Result<IGPUTexture2D*, std::pmr::string> GPUDeviceImpl::CreateTexture2D(const GPUTexture2DCreateInfo& createInfo)
    {
        if (device == nullptr)
        {
            return Result<IGPUTexture2D*, std::pmr::string>::Err(
                    GraphicsDetail::MakeError("유효한 SDL_GPU 디바이스가 필요합니다."));
        }

        if (createInfo.width == 0 || createInfo.height == 0)
        {
            return Result<IGPUTexture2D*, std::pmr::string>::Err(
                    GraphicsDetail::MakeError("텍스처 가로·세로 크기는 0보다 커야 합니다."));
        }

        SDL_GPUTextureUsageFlags sdlUsage = 0;
        if ((createInfo.usage & static_cast<UInt32>(EGPUTextureUsage::SAMPLER)) != 0u)
        {
            sdlUsage |= SDL_GPU_TEXTUREUSAGE_SAMPLER;
        }
        if ((createInfo.usage & static_cast<UInt32>(EGPUTextureUsage::COLOR_TARGET)) != 0u)
        {
            sdlUsage |= SDL_GPU_TEXTUREUSAGE_COLOR_TARGET;
        }
        if ((createInfo.usage & static_cast<UInt32>(EGPUTextureUsage::DEPTH_STENCIL_TARGET)) != 0u)
        {
            sdlUsage |= SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET;
        }

        SDL_GPUTextureCreateInfo sdlInfo{};
        sdlInfo.type                 = GraphicsDetail::ToSDLTextureType(createInfo.type);
        sdlInfo.format               = GraphicsDetail::ToSDLTextureFormat(createInfo.format);
        sdlInfo.usage                = sdlUsage;
        sdlInfo.width                = createInfo.width;
        sdlInfo.height               = createInfo.height;
        sdlInfo.layer_count_or_depth = createInfo.layerCount;
        sdlInfo.num_levels           = createInfo.mipLevels;
        sdlInfo.sample_count         = SDL_GPU_SAMPLECOUNT_1;

        SDL_GPUTexture* sdlTexture = SDL_CreateGPUTexture(device, &sdlInfo);
        if (sdlTexture == nullptr)
        {
            return Result<IGPUTexture2D*, std::pmr::string>::Err(
                    GraphicsDetail::FormatError("SDL_GPU 텍스처 생성에 실패했습니다: {}", SDL_GetError()));
        }

        auto              textureImpl = MakeUnique<GPUTexture2DImpl>(this,
                                                                     sdlTexture,
                                                                     createInfo.width,
                                                                     createInfo.height,
                                                                     createInfo.layerCount,
                                                                     createInfo.mipLevels,
                                                                     createInfo.format,
                                                                     createInfo.type,
                                                                     true);
        GPUTexture2DImpl* result      = textureImpl.get();
        textures.push_back(std::move(textureImpl));
        return Result<IGPUTexture2D*, std::pmr::string>::Ok(result);
    }

    Result<IGPUTexture2D*, std::pmr::string>
            GPUDeviceImpl::CreateTexture2D(const GPUTexture2DCreateInfo&              createInfo,
                                           std::span<const GPUTextureSubresourceData> subresources)
    {
        if (device == nullptr)
        {
            return Result<IGPUTexture2D*, std::pmr::string>::Err(
                    GraphicsDetail::MakeError("유효한 SDL_GPU 디바이스가 필요합니다."));
        }

        if (subresources.empty())
        {
            return Result<IGPUTexture2D*, std::pmr::string>::Err(
                    GraphicsDetail::MakeError("텍스처 업로드에 사용할 서브리소스 데이터가 비어 있습니다."));
        }

        constexpr UInt64         UPLOAD_OFFSET_ALIGNMENT = 512u;
        UInt64                   totalUploadSize         = 0u;
        std::pmr::vector<UInt32> offsets(Memory::GetMemoryResource());
        offsets.reserve(subresources.size());

        for (const GPUTextureSubresourceData& subresource : subresources)
        {
            if (subresource.data.empty() || subresource.mipLevel >= createInfo.mipLevels ||
                subresource.layer >= createInfo.layerCount)
            {
                return Result<IGPUTexture2D*, std::pmr::string>::Err(
                        GraphicsDetail::MakeError("텍스처 서브리소스 범위 또는 데이터가 유효하지 않습니다."));
            }

            const UInt64 ALIGNED_OFFSET =
                    (totalUploadSize + UPLOAD_OFFSET_ALIGNMENT - 1u) & ~(UPLOAD_OFFSET_ALIGNMENT - 1u);
            if (ALIGNED_OFFSET > std::numeric_limits<UInt32>::max() ||
                subresource.data.size() > std::numeric_limits<UInt32>::max() - ALIGNED_OFFSET)
            {
                return Result<IGPUTexture2D*, std::pmr::string>::Err(
                        GraphicsDetail::MakeError("텍스처 업로드 크기가 32비트 범위를 초과했습니다."));
            }

            offsets.push_back(static_cast<UInt32>(ALIGNED_OFFSET));
            totalUploadSize = ALIGNED_OFFSET + subresource.data.size();
        }

        auto textureResult = CreateTexture2D(createInfo);
        if (!textureResult)
        {
            return Result<IGPUTexture2D*, std::pmr::string>::Err(std::move(textureResult).GetError());
        }

        IGPUTexture2D*  texture    = textureResult.GetValue();
        SDL_GPUTexture* sdlTexture = static_cast<const GPUTexture2DImpl*>(texture)->GetSDLTexture();

        SDL_GPUTransferBufferCreateInfo transferInfo{};
        transferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
        transferInfo.size  = static_cast<UInt32>(totalUploadSize);

        SDL_GPUTransferBuffer* stagingBuffer = SDL_CreateGPUTransferBuffer(device, &transferInfo);
        if (stagingBuffer == nullptr)
        {
            DestroyTexture2D(texture);
            return Result<IGPUTexture2D*, std::pmr::string>::Err(
                    GraphicsDetail::FormatError("텍스처 업로드용 전송 버퍼 생성에 실패했습니다: {}", SDL_GetError()));
        }

        void* mapped = SDL_MapGPUTransferBuffer(device, stagingBuffer, false);
        if (mapped == nullptr)
        {
            SDL_ReleaseGPUTransferBuffer(device, stagingBuffer);
            DestroyTexture2D(texture);
            return Result<IGPUTexture2D*, std::pmr::string>::Err(
                    GraphicsDetail::FormatError("텍스처 업로드용 전송 버퍼 매핑에 실패했습니다: {}", SDL_GetError()));
        }

        auto* mappedBytes = static_cast<Byte*>(mapped);
        for (USize index = 0; index < subresources.size(); ++index)
        {
            const std::span<const Byte> SRC = subresources[index].data;
            std::memcpy(mappedBytes + offsets[index], SRC.data(), SRC.size());
        }
        SDL_UnmapGPUTransferBuffer(device, stagingBuffer);

        SDL_GPUCommandBuffer* sdlCommandBuffer = SDL_AcquireGPUCommandBuffer(device);
        if (sdlCommandBuffer == nullptr)
        {
            SDL_ReleaseGPUTransferBuffer(device, stagingBuffer);
            DestroyTexture2D(texture);
            return Result<IGPUTexture2D*, std::pmr::string>::Err(
                    GraphicsDetail::FormatError("텍스처 업로드용 명령 버퍼 획득에 실패했습니다: {}", SDL_GetError()));
        }

        SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(sdlCommandBuffer);
        if (copyPass == nullptr)
        {
            SDL_CancelGPUCommandBuffer(sdlCommandBuffer);
            SDL_ReleaseGPUTransferBuffer(device, stagingBuffer);
            DestroyTexture2D(texture);
            return Result<IGPUTexture2D*, std::pmr::string>::Err(
                    GraphicsDetail::FormatError("텍스처 업로드용 복사 패스 시작에 실패했습니다: {}", SDL_GetError()));
        }

        for (USize index = 0; index < subresources.size(); ++index)
        {
            const GPUTextureSubresourceData& SUBRESOURCE = subresources[index];

            SDL_GPUTextureTransferInfo sourceInfo{};
            sourceInfo.transfer_buffer = stagingBuffer;
            sourceInfo.offset          = offsets[index];
            sourceInfo.pixels_per_row  = 0u;
            sourceInfo.rows_per_layer  = 0u;

            SDL_GPUTextureRegion destinationRegion{};
            destinationRegion.texture   = sdlTexture;
            destinationRegion.mip_level = SUBRESOURCE.mipLevel;
            destinationRegion.layer     = SUBRESOURCE.layer;
            destinationRegion.x         = 0u;
            destinationRegion.y         = 0u;
            destinationRegion.z         = 0u;
            destinationRegion.w         = std::max(1u, createInfo.width >> SUBRESOURCE.mipLevel);
            destinationRegion.h         = std::max(1u, createInfo.height >> SUBRESOURCE.mipLevel);
            destinationRegion.d         = 1u;

            SDL_UploadToGPUTexture(copyPass, &sourceInfo, &destinationRegion, false);
        }
        SDL_EndGPUCopyPass(copyPass);

        SDL_GPUFence* uploadFence = SDL_SubmitGPUCommandBufferAndAcquireFence(sdlCommandBuffer);
        if (uploadFence == nullptr)
        {
            SDL_ReleaseGPUTransferBuffer(device, stagingBuffer);
            DestroyTexture2D(texture);
            return Result<IGPUTexture2D*, std::pmr::string>::Err(
                    GraphicsDetail::FormatError("텍스처 업로드 제출에 실패했습니다: {}", SDL_GetError()));
        }

        SDL_WaitForGPUFences(device, true, &uploadFence, 1u);
        SDL_ReleaseGPUFence(device, uploadFence);
        SDL_ReleaseGPUTransferBuffer(device, stagingBuffer);
        return Result<IGPUTexture2D*, std::pmr::string>::Ok(texture);
    }

    void GPUDeviceImpl::DestroyTexture2D(IGPUTexture2D* texture) noexcept
    {
        DestroyOwnedResource(textures, texture);
    }

    Result<IGPUCommandBuffer*, std::pmr::string> GPUDeviceImpl::CreateCommandBuffer()
    {
        if (device == nullptr)
        {
            return Result<IGPUCommandBuffer*, std::pmr::string>::Err(
                    GraphicsDetail::MakeError("유효한 SDL_GPU 디바이스가 필요합니다."));
        }

        auto                  commandBufferImpl = MakeUnique<GPUCommandBufferImpl>(this);
        GPUCommandBufferImpl* result            = commandBufferImpl.get();
        commandBuffers.push_back(std::move(commandBufferImpl));
        return Result<IGPUCommandBuffer*, std::pmr::string>::Ok(result);
    }

    void GPUDeviceImpl::DestroyCommandBuffer(IGPUCommandBuffer* commandBuffer) noexcept
    {
        DestroyOwnedResource(commandBuffers, commandBuffer);
    }

    Result<IGPUFence*, std::pmr::string> GPUDeviceImpl::CreateFence()
    {
        if (device == nullptr)
        {
            return Result<IGPUFence*, std::pmr::string>::Err(
                    GraphicsDetail::MakeError("유효한 SDL_GPU 디바이스가 필요합니다."));
        }

        auto          fenceImpl = MakeUnique<GPUFenceImpl>(this);
        GPUFenceImpl* result    = fenceImpl.get();
        fences.push_back(std::move(fenceImpl));
        return Result<IGPUFence*, std::pmr::string>::Ok(result);
    }

    void GPUDeviceImpl::DestroyFence(IGPUFence* fence) noexcept
    {
        DestroyOwnedResource(fences, fence);
    }

    Result<IGPUSwapChain*, std::pmr::string> GPUDeviceImpl::CreateSwapChain(const IDisplay* display)
    {
        if (device == nullptr || display == nullptr)
        {
            return Result<IGPUSwapChain*, std::pmr::string>::Err(
                    GraphicsDetail::MakeError("스왑체인 생성에 필요한 GPU 디바이스 또는 디스플레이가 없습니다."));
        }

        const auto* displayImpl = static_cast<const DisplayImpl*>(display);
        SDL_Window* sdlWindow   = displayImpl->GetSDLWindow();
        if (sdlWindow == nullptr)
        {
            return Result<IGPUSwapChain*, std::pmr::string>::Err(
                    GraphicsDetail::MakeError("디스플레이의 SDL 윈도우 핸들이 유효하지 않습니다."));
        }

        if (!SDL_ClaimWindowForGPUDevice(device, sdlWindow))
        {
            return Result<IGPUSwapChain*, std::pmr::string>::Err(
                    GraphicsDetail::FormatError("SDL_GPU 디바이스에 윈도우 연결을 실패했습니다: {}", SDL_GetError()));
        }

        auto              swapChainImpl = MakeUnique<GPUSwapChainImpl>(this, display);
        GPUSwapChainImpl* result        = swapChainImpl.get();
        swapChains.push_back(std::move(swapChainImpl));
        return Result<IGPUSwapChain*, std::pmr::string>::Ok(result);
    }

    void GPUDeviceImpl::DestroySwapChain(IGPUSwapChain* swapChain) noexcept
    {
        DestroyOwnedResource(swapChains, swapChain);
    }

    Result<IGPUGraphicsPipelineState*, std::pmr::string>
            GPUDeviceImpl::CreateGraphicsPipelineState(const GPUGraphicsPipelineDesc& description)
    {
        if (device == nullptr)
        {
            return Result<IGPUGraphicsPipelineState*, std::pmr::string>::Err(
                    GraphicsDetail::MakeError("유효한 SDL_GPU 디바이스가 필요합니다."));
        }

        if (description.vertexShader == nullptr || description.fragmentShader == nullptr)
        {
            return Result<IGPUGraphicsPipelineState*, std::pmr::string>::Err(GraphicsDetail::MakeError(
                    "그래픽스 파이프라인 생성에는 정점 셰이더와 프래그먼트 셰이더가 필요합니다."));
        }

        const auto*    vertexShaderImpl   = static_cast<const GPUShaderImpl*>(description.vertexShader);
        const auto*    fragmentShaderImpl = static_cast<const GPUShaderImpl*>(description.fragmentShader);
        SDL_GPUShader* sdlVertexShader    = vertexShaderImpl->GetSDLShader();
        SDL_GPUShader* sdlFragmentShader  = fragmentShaderImpl->GetSDLShader();
        if (sdlVertexShader == nullptr || sdlFragmentShader == nullptr)
        {
            return Result<IGPUGraphicsPipelineState*, std::pmr::string>::Err(
                    GraphicsDetail::MakeError("셰이더의 SDL_GPU 핸들이 유효하지 않습니다."));
        }

        std::pmr::vector<SDL_GPUVertexAttribute> sdlAttributes(Memory::GetMemoryResource());
        sdlAttributes.reserve(description.vertexAttributes.size());
        for (const GPUVertexAttribute& attribute : description.vertexAttributes)
        {
            SDL_GPUVertexAttribute sdlAttribute{};
            sdlAttribute.location    = attribute.location;
            sdlAttribute.buffer_slot = attribute.bufferSlot;
            sdlAttribute.format      = GraphicsDetail::ToSDLVertexElementFormat(attribute.format);
            sdlAttribute.offset      = attribute.offset;
            sdlAttributes.push_back(sdlAttribute);
        }

        SDL_GPUVertexBufferDescription vertexBufferDesc{};
        vertexBufferDesc.slot               = 0u;
        vertexBufferDesc.pitch              = description.vertexStride;
        vertexBufferDesc.input_rate         = SDL_GPU_VERTEXINPUTRATE_VERTEX;
        vertexBufferDesc.instance_step_rate = 0u;

        SDL_GPUColorTargetDescription colorTargetDesc{};
        colorTargetDesc.format                   = GraphicsDetail::ToSDLTextureFormat(description.colorTargetFormat);
        colorTargetDesc.blend_state.enable_blend = description.enableBlend;
        colorTargetDesc.blend_state.src_color_blendfactor =
                GraphicsDetail::ToSDLBlendFactor(description.srcColorBlendFactor);
        colorTargetDesc.blend_state.dst_color_blendfactor =
                GraphicsDetail::ToSDLBlendFactor(description.dstColorBlendFactor);
        colorTargetDesc.blend_state.color_blend_op = GraphicsDetail::ToSDLBlendOp(description.colorBlendOp);
        colorTargetDesc.blend_state.src_alpha_blendfactor =
                GraphicsDetail::ToSDLBlendFactor(description.srcAlphaBlendFactor);
        colorTargetDesc.blend_state.dst_alpha_blendfactor =
                GraphicsDetail::ToSDLBlendFactor(description.dstAlphaBlendFactor);
        colorTargetDesc.blend_state.alpha_blend_op = GraphicsDetail::ToSDLBlendOp(description.alphaBlendOp);

        SDL_GPUGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.vertex_shader   = sdlVertexShader;
        pipelineInfo.fragment_shader = sdlFragmentShader;
        pipelineInfo.primitive_type  = GraphicsDetail::ToSDLPrimitiveType(description.primitiveTopology);

        if (!sdlAttributes.empty() && description.vertexStride > 0u)
        {
            pipelineInfo.vertex_input_state.vertex_buffer_descriptions = &vertexBufferDesc;
            pipelineInfo.vertex_input_state.num_vertex_buffers         = 1u;
            pipelineInfo.vertex_input_state.vertex_attributes          = sdlAttributes.data();
            pipelineInfo.vertex_input_state.num_vertex_attributes      = static_cast<UInt32>(sdlAttributes.size());
        }

        pipelineInfo.rasterizer_state.fill_mode         = GraphicsDetail::ToSDLFillMode(description.fillMode);
        pipelineInfo.rasterizer_state.cull_mode         = GraphicsDetail::ToSDLCullMode(description.cullMode);
        pipelineInfo.rasterizer_state.front_face        = GraphicsDetail::ToSDLFrontFace(description.frontFace);
        pipelineInfo.rasterizer_state.enable_depth_clip = description.enableDepthClip;

        pipelineInfo.multisample_state.sample_count = SDL_GPU_SAMPLECOUNT_1;

        pipelineInfo.depth_stencil_state.enable_depth_test  = description.enableDepthTest;
        pipelineInfo.depth_stencil_state.enable_depth_write = description.enableDepthWrite;
        pipelineInfo.depth_stencil_state.compare_op = GraphicsDetail::ToSDLCompareOp(description.depthCompareOp);

        pipelineInfo.target_info.color_target_descriptions = &colorTargetDesc;
        pipelineInfo.target_info.num_color_targets         = 1u;
        pipelineInfo.target_info.has_depth_stencil_target  = description.hasDepthStencilTarget;
        pipelineInfo.target_info.depth_stencil_format      = description.hasDepthStencilTarget
                ? GraphicsDetail::ToSDLTextureFormat(description.depthStencilFormat)
                : SDL_GPU_TEXTUREFORMAT_INVALID;

        SDL_GPUGraphicsPipeline* sdlPipeline = SDL_CreateGPUGraphicsPipeline(device, &pipelineInfo);
        if (sdlPipeline == nullptr)
        {
            return Result<IGPUGraphicsPipelineState*, std::pmr::string>::Err(
                    GraphicsDetail::FormatError("SDL_GPU 그래픽스 파이프라인 생성에 실패했습니다: {}", SDL_GetError()));
        }

        auto                          pipelineImpl = MakeUnique<GPUGraphicsPipelineStateImpl>(this, sdlPipeline);
        GPUGraphicsPipelineStateImpl* result       = pipelineImpl.get();
        pipelineStates.push_back(std::move(pipelineImpl));
        return Result<IGPUGraphicsPipelineState*, std::pmr::string>::Ok(result);
    }

    void GPUDeviceImpl::DestroyGraphicsPipelineState(IGPUGraphicsPipelineState* pipelineState) noexcept
    {
        DestroyOwnedResource(pipelineStates, pipelineState);
    }

    Result<IGPUMesh*, std::pmr::string> GPUDeviceImpl::CreateMesh(const IGPUBuffer*     vertexBuffer,
                                                                  UInt32                vertexStride,
                                                                  const IGPUBuffer*     indexBuffer,
                                                                  EGPUIndexFormat       indexFormat,
                                                                  EGPUPrimitiveTopology primitive)
    {
        if (vertexBuffer == nullptr || vertexStride == 0)
        {
            return Result<IGPUMesh*, std::pmr::string>::Err(
                    GraphicsDetail::MakeError("정점 버퍼 크기 또는 스트라이드가 유효하지 않습니다."));
        }

        auto meshImpl       = MakeUnique<GPUMeshImpl>(vertexBuffer, vertexStride, indexBuffer, indexFormat, primitive);
        GPUMeshImpl* result = meshImpl.get();
        meshes.push_back(std::move(meshImpl));
        return Result<IGPUMesh*, std::pmr::string>::Ok(result);
    }

    Result<IGPUMesh*, std::pmr::string> GPUDeviceImpl::CreateMesh(std::span<const Byte> vertexData,
                                                                  UInt32                vertexStride,
                                                                  std::span<const Byte> indexData,
                                                                  EGPUIndexFormat       indexFormat,
                                                                  EGPUPrimitiveTopology primitive)
    {
        if (vertexData.empty() || vertexStride == 0)
        {
            return Result<IGPUMesh*, std::pmr::string>::Err(
                    GraphicsDetail::MakeError("메시 정점 데이터 또는 스트라이드가 유효하지 않습니다."));
        }

        auto vertexBufferResult = CreateBuffer(EGPUBufferUsage::VERTEX, vertexData.size());
        if (!vertexBufferResult)
        {
            return Result<IGPUMesh*, std::pmr::string>::Err(std::move(vertexBufferResult).GetError());
        }

        IGPUBuffer* vertexBuffer = vertexBufferResult.GetValue();
        if (auto writeResult = vertexBuffer->Write(vertexData, 0); !writeResult)
        {
            DestroyBuffer(vertexBuffer);
            return Result<IGPUMesh*, std::pmr::string>::Err(std::move(writeResult).GetError());
        }

        IGPUBuffer* indexBuffer = nullptr;
        if (!indexData.empty())
        {
            auto indexBufferResult = CreateBuffer(EGPUBufferUsage::INDEX, indexData.size());
            if (!indexBufferResult)
            {
                DestroyBuffer(vertexBuffer);
                return Result<IGPUMesh*, std::pmr::string>::Err(std::move(indexBufferResult).GetError());
            }

            indexBuffer = indexBufferResult.GetValue();
            if (auto writeResult = indexBuffer->Write(indexData, 0); !writeResult)
            {
                DestroyBuffer(indexBuffer);
                DestroyBuffer(vertexBuffer);
                return Result<IGPUMesh*, std::pmr::string>::Err(std::move(writeResult).GetError());
            }
        }

        auto meshResult = CreateMesh(vertexBuffer, vertexStride, indexBuffer, indexFormat, primitive);
        if (!meshResult)
        {
            DestroyBuffer(indexBuffer);
            DestroyBuffer(vertexBuffer);
            return Result<IGPUMesh*, std::pmr::string>::Err(std::move(meshResult).GetError());
        }

        IGPUMesh* mesh = meshResult.GetValue();
        static_cast<GPUMeshImpl*>(mesh)->SetOwnedBuffers(vertexBuffer, indexBuffer);
        return Result<IGPUMesh*, std::pmr::string>::Ok(mesh);
    }

    void GPUDeviceImpl::DestroyMesh(IGPUMesh* mesh) noexcept
    {
        if (mesh == nullptr)
        {
            return;
        }

        const auto* meshImpl    = static_cast<const GPUMeshImpl*>(mesh);
        IGPUBuffer* ownedVertex = meshImpl->OwnsBuffers() ? meshImpl->GetOwnedVertexBuffer() : nullptr;
        IGPUBuffer* ownedIndex  = meshImpl->OwnsBuffers() ? meshImpl->GetOwnedIndexBuffer() : nullptr;

        DestroyOwnedResource(meshes, mesh);
        DestroyBuffer(ownedIndex);
        DestroyBuffer(ownedVertex);
    }

    void GPUDeviceImpl::Release() noexcept
    {
        if (device != nullptr)
        {
            SDL_WaitForGPUIdle(device);
        }

        meshes.clear();
        pipelineStates.clear();
        swapChains.clear();
        fences.clear();
        commandBuffers.clear();
        textures.clear();
        shaders.clear();
        buffers.clear();

        if (device != nullptr)
        {
            if (fallbackTexture != nullptr)
            {
                SDL_ReleaseGPUTexture(device, fallbackTexture);
            }

            if (defaultSampler != nullptr)
            {
                SDL_ReleaseGPUSampler(device, defaultSampler);
            }

            SDL_DestroyGPUDevice(device);
        }

        fallbackTexture = nullptr;
        defaultSampler  = nullptr;
        device          = nullptr;
    }
} // namespace Garage
