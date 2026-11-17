#include "gpu_device_impl.hpp"

#include <limits>
#include <memory>
#include <span>
#include <string>
#include <vector>

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_video.h>

#include "garage/core/types.hpp"

#include "garage/graphics/gpu_buffer.hpp"
#include "garage/graphics/gpu_command_buffer.hpp"
#include "garage/graphics/gpu_fence.hpp"
#include "garage/graphics/gpu_graphics_pipeline_state.hpp"
#include "garage/graphics/gpu_mesh.hpp"
#include "garage/graphics/gpu_shader.hpp"
#include "garage/graphics/gpu_swap_chain.hpp"
#include "garage/graphics/gpu_texture2d.hpp"
#include "garage/graphics/gpu_types.hpp"

#include "garage/platform/display.hpp"

#include "garage/utilities/logger.hpp"

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
        void DestroyOwnedResource(std::vector<std::unique_ptr<TImpl>>& resources, const TInterface* target) noexcept
        {
            if (target == nullptr)
            {
                return;
            }

            std::erase_if(resources,
                          [target](const std::unique_ptr<TImpl>& item) noexcept
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

    IGPUBuffer* GPUDeviceImpl::CreateBuffer(EGPUBufferUsage usage, UInt64 sizeInBytes)
    {
        if (device == nullptr || sizeInBytes == 0 || sizeInBytes > std::numeric_limits<UInt32>::max())
        {
            return nullptr;
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
                LOG_ERROR("SDL_GPU 전송 버퍼 생성에 실패했습니다: {}", SDL_GetError());
                return nullptr;
            }

            auto bufferImpl       = std::make_unique<GPUBufferImpl>(this, usage, sizeInBytes, nullptr, transferBuffer);
            GPUBufferImpl* result = bufferImpl.get();
            buffers.push_back(std::move(bufferImpl));
            return result;
        }

        SDL_GPUBufferCreateInfo bufferInfo{};
        bufferInfo.usage = (usage == EGPUBufferUsage::INDEX) ? SDL_GPU_BUFFERUSAGE_INDEX : SDL_GPU_BUFFERUSAGE_VERTEX;
        bufferInfo.size  = SIZE_U32;

        SDL_GPUBuffer* sdlBuffer = SDL_CreateGPUBuffer(device, &bufferInfo);
        if (sdlBuffer == nullptr)
        {
            LOG_ERROR("SDL_GPU 버퍼 생성에 실패했습니다: {}", SDL_GetError());
            return nullptr;
        }

        auto           bufferImpl = std::make_unique<GPUBufferImpl>(this, usage, sizeInBytes, sdlBuffer, nullptr);
        GPUBufferImpl* result     = bufferImpl.get();
        buffers.push_back(std::move(bufferImpl));
        return result;
    }

    IGPUBuffer* GPUDeviceImpl::CreateUploadBuffer(std::span<const Byte> data)
    {
        if (data.empty())
        {
            return nullptr;
        }

        IGPUBuffer* uploadBuffer = CreateBuffer(EGPUBufferUsage::UPLOAD, data.size());
        if (uploadBuffer == nullptr)
        {
            return nullptr;
        }

        if (!uploadBuffer->Write(data, 0))
        {
            DestroyBuffer(uploadBuffer);
            return nullptr;
        }

        return uploadBuffer;
    }

    void GPUDeviceImpl::DestroyBuffer(IGPUBuffer* buffer) noexcept
    {
        DestroyOwnedResource(buffers, buffer);
    }

    IGPUShader* GPUDeviceImpl::CreateShader(const GPUShaderCreateInfo& createInfo)
    {
        if (device == nullptr || createInfo.code.empty() || createInfo.format == EGPUShaderFormat::INVALID)
        {
            return nullptr;
        }

        const std::string ENTRY_POINT(createInfo.entryPoint);

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
            LOG_ERROR("SDL_GPU 셰이더 생성에 실패했습니다: {}", SDL_GetError());
            return nullptr;
        }

        auto shaderImpl       = std::make_unique<GPUShaderImpl>(this, sdlShader, createInfo.stage, createInfo.format);
        GPUShaderImpl* result = shaderImpl.get();
        shaders.push_back(std::move(shaderImpl));
        return result;
    }

    void GPUDeviceImpl::DestroyShader(IGPUShader* shader) noexcept
    {
        DestroyOwnedResource(shaders, shader);
    }

    IGPUTexture2D* GPUDeviceImpl::CreateTexture2D(const GPUTexture2DCreateInfo& createInfo)
    {
        if (device == nullptr || createInfo.width == 0 || createInfo.height == 0)
        {
            return nullptr;
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
            LOG_ERROR("SDL_GPU 텍스처 생성에 실패했습니다: {}", SDL_GetError());
            return nullptr;
        }

        auto              textureImpl = std::make_unique<GPUTexture2DImpl>(this,
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
        return result;
    }

    void GPUDeviceImpl::DestroyTexture2D(IGPUTexture2D* texture) noexcept
    {
        DestroyOwnedResource(textures, texture);
    }

    IGPUCommandBuffer* GPUDeviceImpl::CreateCommandBuffer()
    {
        if (device == nullptr)
        {
            return nullptr;
        }

        auto                  commandBufferImpl = std::make_unique<GPUCommandBufferImpl>(this);
        GPUCommandBufferImpl* result            = commandBufferImpl.get();
        commandBuffers.push_back(std::move(commandBufferImpl));
        return result;
    }

    void GPUDeviceImpl::DestroyCommandBuffer(IGPUCommandBuffer* commandBuffer) noexcept
    {
        DestroyOwnedResource(commandBuffers, commandBuffer);
    }

    IGPUFence* GPUDeviceImpl::CreateFence()
    {
        if (device == nullptr)
        {
            return nullptr;
        }

        auto          fenceImpl = std::make_unique<GPUFenceImpl>(this);
        GPUFenceImpl* result    = fenceImpl.get();
        fences.push_back(std::move(fenceImpl));
        return result;
    }

    void GPUDeviceImpl::DestroyFence(IGPUFence* fence) noexcept
    {
        DestroyOwnedResource(fences, fence);
    }

    IGPUSwapChain* GPUDeviceImpl::CreateSwapChain(const IDisplay* display)
    {
        if (device == nullptr || display == nullptr)
        {
            return nullptr;
        }

        const auto* displayImpl = static_cast<const DisplayImpl*>(display);
        SDL_Window* sdlWindow   = displayImpl->GetSDLWindow();
        if (sdlWindow == nullptr)
        {
            return nullptr;
        }

        if (!SDL_ClaimWindowForGPUDevice(device, sdlWindow))
        {
            LOG_ERROR("SDL_GPU 디바이스에 윈도우 연결을 실패했습니다: {}", SDL_GetError());
            return nullptr;
        }

        auto              swapChainImpl = std::make_unique<GPUSwapChainImpl>(this, display);
        GPUSwapChainImpl* result        = swapChainImpl.get();
        swapChains.push_back(std::move(swapChainImpl));
        return result;
    }

    void GPUDeviceImpl::DestroySwapChain(IGPUSwapChain* swapChain) noexcept
    {
        DestroyOwnedResource(swapChains, swapChain);
    }

    IGPUGraphicsPipelineState* GPUDeviceImpl::CreateGraphicsPipelineState(const GPUGraphicsPipelineDesc& description)
    {
        if (device == nullptr || description.vertexShader == nullptr || description.fragmentShader == nullptr)
        {
            return nullptr;
        }

        const auto*    vertexShaderImpl   = static_cast<const GPUShaderImpl*>(description.vertexShader);
        const auto*    fragmentShaderImpl = static_cast<const GPUShaderImpl*>(description.fragmentShader);
        SDL_GPUShader* sdlVertexShader    = vertexShaderImpl->GetSDLShader();
        SDL_GPUShader* sdlFragmentShader  = fragmentShaderImpl->GetSDLShader();
        if (sdlVertexShader == nullptr || sdlFragmentShader == nullptr)
        {
            return nullptr;
        }

        std::vector<SDL_GPUVertexAttribute> sdlAttributes;
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
            LOG_ERROR("SDL_GPU 그래픽스 파이프라인 생성에 실패했습니다: {}", SDL_GetError());
            return nullptr;
        }

        auto                          pipelineImpl = std::make_unique<GPUGraphicsPipelineStateImpl>(this, sdlPipeline);
        GPUGraphicsPipelineStateImpl* result       = pipelineImpl.get();
        pipelineStates.push_back(std::move(pipelineImpl));
        return result;
    }

    void GPUDeviceImpl::DestroyGraphicsPipelineState(IGPUGraphicsPipelineState* pipelineState) noexcept
    {
        DestroyOwnedResource(pipelineStates, pipelineState);
    }

    IGPUMesh* GPUDeviceImpl::CreateMesh(const IGPUBuffer*     vertexBuffer,
                                        UInt32                vertexStride,
                                        const IGPUBuffer*     indexBuffer,
                                        EGPUIndexFormat       indexFormat,
                                        EGPUPrimitiveTopology primitive)
    {
        if (vertexBuffer == nullptr || vertexStride == 0)
        {
            return nullptr;
        }

        auto meshImpl = std::make_unique<GPUMeshImpl>(vertexBuffer, vertexStride, indexBuffer, indexFormat, primitive);
        GPUMeshImpl* result = meshImpl.get();
        meshes.push_back(std::move(meshImpl));
        return result;
    }

    IGPUMesh* GPUDeviceImpl::CreateMesh(std::span<const Byte> vertexData,
                                        UInt32                vertexStride,
                                        std::span<const Byte> indexData,
                                        EGPUIndexFormat       indexFormat,
                                        EGPUPrimitiveTopology primitive)
    {
        if (vertexData.empty() || vertexStride == 0)
        {
            return nullptr;
        }

        IGPUBuffer* vertexBuffer = CreateBuffer(EGPUBufferUsage::VERTEX, vertexData.size());
        if (vertexBuffer == nullptr)
        {
            return nullptr;
        }

        if (!vertexBuffer->Write(vertexData, 0))
        {
            DestroyBuffer(vertexBuffer);
            return nullptr;
        }

        IGPUBuffer* indexBuffer = nullptr;
        if (!indexData.empty())
        {
            indexBuffer = CreateBuffer(EGPUBufferUsage::INDEX, indexData.size());
            if (indexBuffer == nullptr)
            {
                DestroyBuffer(vertexBuffer);
                return nullptr;
            }

            if (!indexBuffer->Write(indexData, 0))
            {
                DestroyBuffer(indexBuffer);
                DestroyBuffer(vertexBuffer);
                return nullptr;
            }
        }

        IGPUMesh* mesh = CreateMesh(vertexBuffer, vertexStride, indexBuffer, indexFormat, primitive);
        if (mesh == nullptr)
        {
            DestroyBuffer(indexBuffer);
            DestroyBuffer(vertexBuffer);
            return nullptr;
        }

        static_cast<GPUMeshImpl*>(mesh)->SetOwnedBuffers(vertexBuffer, indexBuffer);
        return mesh;
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
