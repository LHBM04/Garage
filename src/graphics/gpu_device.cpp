#include "garage/graphics/gpu_device.hpp"

#include <algorithm>
#include <limits>
#include <memory>
#include <span>
#include <string>
#include <utility>
#include <vector>

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_video.h>

#include "garage/core/defines.hpp"
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
#include "graphics/gpu_sdl_conversions.hpp"

#include "garage/platform/display.hpp"
#include "platform/display_impl.hpp"

#include "garage/utilities/logger.hpp"

namespace Garage
{
    GPUDevice::GPUDevice(const GPUDeviceCreateInfo& createInfo) noexcept
        : deviceHandle(createInfo.deviceHandle)
        , defaultSampler(createInfo.defaultSampler)
        , fallbackTexture(createInfo.fallbackTexture)
    {
        ASSERT(this->deviceHandle != nullptr);
    }

    GPUDevice::~GPUDevice() noexcept
    {
        Release();
    }

    GPUDevice::GPUDevice(GPUDevice&& other) noexcept
        : deviceHandle(std::exchange(other.deviceHandle, nullptr))
        , defaultSampler(std::exchange(other.defaultSampler, nullptr))
        , fallbackTexture(std::exchange(other.fallbackTexture, nullptr))
        , buffers(std::move(other.buffers))
        , shaders(std::move(other.shaders))
        , textures(std::move(other.textures))
        , commandBuffers(std::move(other.commandBuffers))
        , fences(std::move(other.fences))
        , swapChains(std::move(other.swapChains))
        , pipelineStates(std::move(other.pipelineStates))
        , meshes(std::move(other.meshes))
    {
    }

    GPUDevice& GPUDevice::operator=(GPUDevice&& other) noexcept
    {
        if (this != &other)
        {
            Release();
            deviceHandle    = std::exchange(other.deviceHandle, nullptr);
            defaultSampler  = std::exchange(other.defaultSampler, nullptr);
            fallbackTexture = std::exchange(other.fallbackTexture, nullptr);
            buffers         = std::move(other.buffers);
            shaders         = std::move(other.shaders);
            textures        = std::move(other.textures);
            commandBuffers  = std::move(other.commandBuffers);
            fences          = std::move(other.fences);
            swapChains      = std::move(other.swapChains);
            pipelineStates  = std::move(other.pipelineStates);
            meshes          = std::move(other.meshes);
        }

        return *this;
    }

    GPUBuffer* GPUDevice::CreateBuffer(EGPUBufferUsage usage, UInt64 sizeInBytes)
    {
        if (deviceHandle == nullptr || sizeInBytes == 0 || sizeInBytes > std::numeric_limits<UInt32>::max())
        {
            LOG_ERROR("GPU 버퍼 크기가 유효하지 않습니다.");
            return nullptr;
        }

        auto*                  sdlDevice      = static_cast<SDL_GPUDevice*>(deviceHandle);
        SDL_GPUBuffer*         sdlBuffer      = nullptr;
        SDL_GPUTransferBuffer* sdlTransfer    = nullptr;
        const UInt32           BUFFER_SIZE_32 = static_cast<UInt32>(sizeInBytes);

        if (usage == EGPUBufferUsage::VERTEX || usage == EGPUBufferUsage::INDEX)
        {
            SDL_GPUBufferCreateInfo bufferCreateInfo{};
            bufferCreateInfo.usage =
                    (usage == EGPUBufferUsage::INDEX) ? SDL_GPU_BUFFERUSAGE_INDEX : SDL_GPU_BUFFERUSAGE_VERTEX;
            bufferCreateInfo.size = BUFFER_SIZE_32;

            sdlBuffer = SDL_CreateGPUBuffer(sdlDevice, &bufferCreateInfo);
            if (sdlBuffer == nullptr)
            {
                LOG_ERROR("GPU 버퍼 리소스 생성에 실패했습니다: {}", SDL_GetError());
                return nullptr;
            }
        }

        SDL_GPUTransferBufferCreateInfo transferCreateInfo{};
        transferCreateInfo.usage = (usage == EGPUBufferUsage::READBACK) ? SDL_GPU_TRANSFERBUFFERUSAGE_DOWNLOAD
                                                                        : SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
        transferCreateInfo.size  = BUFFER_SIZE_32;

        sdlTransfer = SDL_CreateGPUTransferBuffer(sdlDevice, &transferCreateInfo);

        if (sdlTransfer == nullptr)
        {
            if (sdlBuffer != nullptr)
            {
                SDL_ReleaseGPUBuffer(sdlDevice, sdlBuffer);
            }
            LOG_ERROR("GPU 전송 버퍼 생성에 실패했습니다: {}", SDL_GetError());
            return nullptr;
        }

        auto       newBuffer = std::make_unique<GPUBuffer>(GPUBufferCreateInfo{
                .deviceHandle   = deviceHandle,
                .bufferHandle   = sdlBuffer,
                .transferHandle = sdlTransfer,
                .usage          = usage,
                .sizeInBytes    = sizeInBytes,
        });
        GPUBuffer* pointer   = newBuffer.get();
        buffers.push_back(std::move(newBuffer));
        return pointer;
    }

    GPUBuffer* GPUDevice::CreateUploadBuffer(std::span<const Byte> data)
    {
        GPUBuffer* buffer = CreateBuffer(EGPUBufferUsage::UPLOAD, data.size());
        if (buffer == nullptr)
        {
            return nullptr;
        }

        if (!buffer->Write(data))
        {
            DestroyBuffer(buffer);
            return nullptr;
        }

        return buffer;
    }

    void GPUDevice::DestroyBuffer(GPUBuffer* buffer) noexcept
    {
        std::erase_if(buffers,
                      [buffer](const std::unique_ptr<GPUBuffer>& item)
                      {
                          return item.get() == buffer;
                      });
    }

    GPUShader* GPUDevice::CreateShader(const GPUShaderCreateInfo& createInfo)
    {
        if (deviceHandle == nullptr || createInfo.code.empty() || createInfo.format == EGPUShaderFormat::INVALID)
        {
            LOG_ERROR("GPU 셰이더 생성 정보가 유효하지 않습니다.");
            return nullptr;
        }

        const std::string ENTRY_POINT(createInfo.entryPoint);

        SDL_GPUShaderCreateInfo sdlCreateInfo{};
        sdlCreateInfo.code                 = reinterpret_cast<const UInt8*>(createInfo.code.data());
        sdlCreateInfo.code_size            = createInfo.code.size();
        sdlCreateInfo.entrypoint           = ENTRY_POINT.c_str();
        sdlCreateInfo.format               = GraphicsDetail::ToSDLShaderFormat(createInfo.format);
        sdlCreateInfo.stage                = GraphicsDetail::ToSDLShaderStage(createInfo.stage);
        sdlCreateInfo.num_samplers         = createInfo.samplerCount;
        sdlCreateInfo.num_storage_textures = createInfo.storageTextureCount;
        sdlCreateInfo.num_storage_buffers  = createInfo.storageBufferCount;
        sdlCreateInfo.num_uniform_buffers  = createInfo.uniformBufferCount;

        SDL_GPUShader* sdlShader = SDL_CreateGPUShader(static_cast<SDL_GPUDevice*>(deviceHandle), &sdlCreateInfo);
        if (sdlShader == nullptr)
        {
            LOG_ERROR("GPU 셰이더 생성에 실패했습니다: {}", SDL_GetError());
            return nullptr;
        }

        auto newShader     = std::make_unique<GPUShader>(deviceHandle, sdlShader, createInfo.stage, createInfo.format);
        GPUShader* pointer = newShader.get();
        shaders.push_back(std::move(newShader));
        return pointer;
    }

    void GPUDevice::DestroyShader(GPUShader* shader) noexcept
    {
        std::erase_if(shaders,
                      [shader](const std::unique_ptr<GPUShader>& item)
                      {
                          return item.get() == shader;
                      });
    }

    GPUTexture2D* GPUDevice::CreateTexture2D(const GPUTexture2DCreateInfo& createInfo)
    {
        if (deviceHandle == nullptr || createInfo.width == 0 || createInfo.height == 0)
        {
            LOG_ERROR("GPU 텍스처 생성 정보가 유효하지 않습니다.");
            return nullptr;
        }

        SDL_GPUTextureCreateInfo sdlCreateInfo{};
        sdlCreateInfo.type                 = GraphicsDetail::ToSDLTextureType(createInfo.type);
        sdlCreateInfo.format               = GraphicsDetail::ToSDLTextureFormat(createInfo.format);
        sdlCreateInfo.usage                = createInfo.usage;
        sdlCreateInfo.width                = createInfo.width;
        sdlCreateInfo.height               = createInfo.height;
        sdlCreateInfo.layer_count_or_depth = createInfo.layerCount;
        sdlCreateInfo.num_levels           = createInfo.mipLevels;
        sdlCreateInfo.sample_count         = SDL_GPU_SAMPLECOUNT_1;

        SDL_GPUTexture* sdlTexture = SDL_CreateGPUTexture(static_cast<SDL_GPUDevice*>(deviceHandle), &sdlCreateInfo);
        if (sdlTexture == nullptr)
        {
            LOG_ERROR("GPU 텍스처 생성에 실패했습니다: {}", SDL_GetError());
            return nullptr;
        }

        auto          newTexture = std::make_unique<GPUTexture2D>(GPUTexture2DHandleInfo{
                .deviceHandle  = deviceHandle,
                .textureHandle = sdlTexture,
                .type          = createInfo.type,
                .format        = createInfo.format,
                .width         = createInfo.width,
                .height        = createInfo.height,
                .layerCount    = createInfo.layerCount,
                .mipLevels     = createInfo.mipLevels,
                .ownsHandle    = true,
        });
        GPUTexture2D* pointer    = newTexture.get();
        textures.push_back(std::move(newTexture));
        return pointer;
    }

    void GPUDevice::DestroyTexture2D(GPUTexture2D* texture) noexcept
    {
        std::erase_if(textures,
                      [texture](const std::unique_ptr<GPUTexture2D>& item)
                      {
                          return item.get() == texture;
                      });
    }

    GPUCommandBuffer* GPUDevice::CreateCommandBuffer()
    {
        auto              newCommandBuffer = std::make_unique<GPUCommandBuffer>(GPUCommandBufferCreateInfo{
                .device = this,
        });
        GPUCommandBuffer* pointer          = newCommandBuffer.get();
        commandBuffers.push_back(std::move(newCommandBuffer));
        return pointer;
    }

    void GPUDevice::DestroyCommandBuffer(GPUCommandBuffer* commandBuffer) noexcept
    {
        std::erase_if(commandBuffers,
                      [commandBuffer](const std::unique_ptr<GPUCommandBuffer>& item)
                      {
                          return item.get() == commandBuffer;
                      });
    }

    GPUFence* GPUDevice::CreateFence()
    {
        auto      newFence = std::make_unique<GPUFence>(deviceHandle);
        GPUFence* pointer  = newFence.get();
        fences.push_back(std::move(newFence));
        return pointer;
    }

    void GPUDevice::DestroyFence(GPUFence* fence) noexcept
    {
        std::erase_if(fences,
                      [fence](const std::unique_ptr<GPUFence>& item)
                      {
                          return item.get() == fence;
                      });
    }

    GPUSwapChain* GPUDevice::CreateSwapChain(const IDisplay* display)
    {
        if (deviceHandle == nullptr || display == nullptr)
        {
            LOG_ERROR("스왑체인을 생성할 디스플레이가 유효하지 않습니다.");
            return nullptr;
        }

        const auto* displayImpl = static_cast<const DisplayImpl*>(display);
        SDL_Window* sdlWindow   = displayImpl->GetSDLWindow();
        if (sdlWindow == nullptr)
        {
            LOG_ERROR("디스플레이의 SDL 윈도우가 유효하지 않습니다.");
            return nullptr;
        }

        auto* sdlDevice = static_cast<SDL_GPUDevice*>(deviceHandle);
        if (!SDL_ClaimWindowForGPUDevice(sdlDevice, sdlWindow))
        {
            LOG_ERROR("GPU 디바이스에 윈도우 스왑체인을 연결하지 못했습니다: {}", SDL_GetError());
            return nullptr;
        }

        const EGPUTextureFormat BACK_BUFFER_FORMAT =
                GraphicsDetail::FromSDLTextureFormat(SDL_GetGPUSwapchainTextureFormat(sdlDevice, sdlWindow));

        EGPUTextureFormat depthFormat = EGPUTextureFormat::D32_FLOAT;
        if (!SDL_GPUTextureSupportsFormat(sdlDevice,
                                          SDL_GPU_TEXTUREFORMAT_D32_FLOAT,
                                          SDL_GPU_TEXTURETYPE_2D,
                                          SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET))
        {
            if (SDL_GPUTextureSupportsFormat(sdlDevice,
                                             SDL_GPU_TEXTUREFORMAT_D24_UNORM,
                                             SDL_GPU_TEXTURETYPE_2D,
                                             SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET))
            {
                depthFormat = EGPUTextureFormat::D24_UNORM;
            }
            else
            {
                depthFormat = EGPUTextureFormat::D16_UNORM;
            }
        }

        const Vector2DInt PIXEL_SIZE   = display->GetPixelSize();
        auto              newSwapChain = std::make_unique<GPUSwapChain>(GPUSwapChainCreateInfo{
                .deviceHandle       = deviceHandle,
                .windowHandle       = sdlWindow,
                .display            = display,
                .backBufferFormat   = BACK_BUFFER_FORMAT,
                .depthStencilFormat = depthFormat,
                .width              = static_cast<UInt32>(std::max(0, PIXEL_SIZE.GetX())),
                .height             = static_cast<UInt32>(std::max(0, PIXEL_SIZE.GetY())),
        });
        GPUSwapChain*     pointer      = newSwapChain.get();
        swapChains.push_back(std::move(newSwapChain));
        return pointer;
    }

    void GPUDevice::DestroySwapChain(GPUSwapChain* swapChain) noexcept
    {
        std::erase_if(swapChains,
                      [swapChain](const std::unique_ptr<GPUSwapChain>& item)
                      {
                          return item.get() == swapChain;
                      });
    }

    GPUGraphicsPipelineState* GPUDevice::CreateGraphicsPipelineState(const GPUGraphicsPipelineDesc& description)
    {
        if (deviceHandle == nullptr || description.vertexShader == nullptr || description.fragmentShader == nullptr)
        {
            LOG_ERROR("그래픽스 파이프라인 상태 생성에 필요한 셰이더가 누락되었습니다.");
            return nullptr;
        }

        SDL_GPUVertexBufferDescription vertexBufferDesc{};
        vertexBufferDesc.slot               = 0;
        vertexBufferDesc.pitch              = description.vertexStride;
        vertexBufferDesc.input_rate         = SDL_GPU_VERTEXINPUTRATE_VERTEX;
        vertexBufferDesc.instance_step_rate = 0;

        std::vector<SDL_GPUVertexAttribute> sdlAttributes = {};
        sdlAttributes.reserve(description.vertexAttributes.size());
        for (const GPUVertexAttribute& attribute : description.vertexAttributes)
        {
            sdlAttributes.push_back(SDL_GPUVertexAttribute{
                    .location    = attribute.location,
                    .buffer_slot = attribute.bufferSlot,
                    .format      = GraphicsDetail::ToSDLVertexElementFormat(attribute.format),
                    .offset      = attribute.offset,
            });
        }

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

        SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo{};
        pipelineCreateInfo.vertex_shader   = static_cast<SDL_GPUShader*>(description.vertexShader->GetNativeHandle());
        pipelineCreateInfo.fragment_shader = static_cast<SDL_GPUShader*>(description.fragmentShader->GetNativeHandle());
        pipelineCreateInfo.primitive_type  = GraphicsDetail::ToSDLPrimitiveType(description.primitiveTopology);

        if (description.vertexStride > 0 && !sdlAttributes.empty())
        {
            pipelineCreateInfo.vertex_input_state.vertex_buffer_descriptions = &vertexBufferDesc;
            pipelineCreateInfo.vertex_input_state.num_vertex_buffers         = 1;
            pipelineCreateInfo.vertex_input_state.vertex_attributes          = sdlAttributes.data();
            pipelineCreateInfo.vertex_input_state.num_vertex_attributes = static_cast<UInt32>(sdlAttributes.size());
        }

        pipelineCreateInfo.rasterizer_state.fill_mode         = GraphicsDetail::ToSDLFillMode(description.fillMode);
        pipelineCreateInfo.rasterizer_state.cull_mode         = GraphicsDetail::ToSDLCullMode(description.cullMode);
        pipelineCreateInfo.rasterizer_state.front_face        = GraphicsDetail::ToSDLFrontFace(description.frontFace);
        pipelineCreateInfo.rasterizer_state.enable_depth_clip = description.enableDepthClip;

        pipelineCreateInfo.multisample_state.sample_count = SDL_GPU_SAMPLECOUNT_1;

        pipelineCreateInfo.depth_stencil_state.compare_op = GraphicsDetail::ToSDLCompareOp(description.depthCompareOp);
        pipelineCreateInfo.depth_stencil_state.enable_depth_test  = description.enableDepthTest;
        pipelineCreateInfo.depth_stencil_state.enable_depth_write = description.enableDepthWrite;

        pipelineCreateInfo.target_info.color_target_descriptions = &colorTargetDesc;
        pipelineCreateInfo.target_info.num_color_targets         = 1;
        pipelineCreateInfo.target_info.has_depth_stencil_target  = description.hasDepthStencilTarget;
        pipelineCreateInfo.target_info.depth_stencil_format =
                GraphicsDetail::ToSDLTextureFormat(description.depthStencilFormat);

        SDL_GPUGraphicsPipeline* sdlPipeline =
                SDL_CreateGPUGraphicsPipeline(static_cast<SDL_GPUDevice*>(deviceHandle), &pipelineCreateInfo);
        if (sdlPipeline == nullptr)
        {
            LOG_ERROR("그래픽스 파이프라인 상태 생성에 실패했습니다: {}", SDL_GetError());
            return nullptr;
        }

        auto                      newPipeline = std::make_unique<GPUGraphicsPipelineState>(deviceHandle, sdlPipeline);
        GPUGraphicsPipelineState* pointer     = newPipeline.get();
        pipelineStates.push_back(std::move(newPipeline));
        return pointer;
    }

    void GPUDevice::DestroyGraphicsPipelineState(GPUGraphicsPipelineState* pipelineState) noexcept
    {
        std::erase_if(pipelineStates,
                      [pipelineState](const std::unique_ptr<GPUGraphicsPipelineState>& item)
                      {
                          return item.get() == pipelineState;
                      });
    }

    GPUMesh* GPUDevice::CreateMesh(const GPUBuffer*      vertexBuffer,
                                   UInt32                vertexStride,
                                   const GPUBuffer*      indexBuffer,
                                   EGPUIndexFormat       indexFormat,
                                   EGPUPrimitiveTopology primitive)
    {
        if (vertexBuffer == nullptr || indexBuffer == nullptr || vertexStride == 0 ||
            vertexBuffer->GetSizeInBytes() == 0 ||
            vertexBuffer->GetSizeInBytes() > std::numeric_limits<UInt32>::max() ||
            vertexBuffer->GetSizeInBytes() % vertexStride != 0)
        {
            LOG_ERROR("정점 버퍼 크기 또는 스트라이드가 유효하지 않습니다.");
            return nullptr;
        }

        const UInt32 INDEX_SIZE = indexFormat == EGPUIndexFormat::UINT16 ? 2u : 4u;
        if (indexBuffer->GetSizeInBytes() == 0 || indexBuffer->GetSizeInBytes() > std::numeric_limits<UInt32>::max() ||
            indexBuffer->GetSizeInBytes() % INDEX_SIZE != 0)
        {
            LOG_ERROR("인덱스 버퍼 크기가 유효하지 않습니다.");
            return nullptr;
        }

        const GPUVertexBufferView VERTEX_VIEW{
                .buffer        = vertexBuffer,
                .offset        = 0,
                .sizeInBytes   = static_cast<UInt32>(vertexBuffer->GetSizeInBytes()),
                .strideInBytes = vertexStride,
        };
        const GPUIndexBufferView INDEX_VIEW{
                .buffer      = indexBuffer,
                .offset      = 0,
                .sizeInBytes = static_cast<UInt32>(indexBuffer->GetSizeInBytes()),
                .format      = indexFormat,
        };

        auto     newMesh = std::make_unique<GPUMesh>(GPUMeshCreateInfo{
                .vertexBuffer     = vertexBuffer,
                .vertexBufferView = VERTEX_VIEW,
                .indexBuffer      = indexBuffer,
                .indexBufferView  = INDEX_VIEW,
                .primitive        = primitive,
        });
        GPUMesh* pointer = newMesh.get();
        meshes.push_back(std::move(newMesh));
        return pointer;
    }

    GPUMesh* GPUDevice::CreateMesh(std::span<const Byte> vertexData,
                                   UInt32                vertexStride,
                                   std::span<const Byte> indexData,
                                   EGPUIndexFormat       indexFormat,
                                   EGPUPrimitiveTopology primitive)
    {
        GPUBuffer* vertexBuffer = CreateBuffer(EGPUBufferUsage::VERTEX, vertexData.size());
        if (vertexBuffer == nullptr || !vertexBuffer->Write(vertexData))
        {
            DestroyBuffer(vertexBuffer);
            LOG_ERROR("메시의 정점 GPU 버퍼 생성에 실패했습니다.");
            return nullptr;
        }

        GPUBuffer* indexBuffer = CreateBuffer(EGPUBufferUsage::INDEX, indexData.size());
        if (indexBuffer == nullptr || !indexBuffer->Write(indexData))
        {
            DestroyBuffer(vertexBuffer);
            DestroyBuffer(indexBuffer);
            LOG_ERROR("메시의 인덱스 GPU 버퍼 생성에 실패했습니다.");
            return nullptr;
        }

        GPUMesh* mesh = CreateMesh(vertexBuffer, vertexStride, indexBuffer, indexFormat, primitive);
        if (mesh == nullptr)
        {
            DestroyBuffer(vertexBuffer);
            DestroyBuffer(indexBuffer);
        }

        return mesh;
    }

    void GPUDevice::DestroyMesh(GPUMesh* mesh) noexcept
    {
        std::erase_if(meshes,
                      [mesh](const std::unique_ptr<GPUMesh>& item)
                      {
                          return item.get() == mesh;
                      });
    }

    void GPUDevice::Release() noexcept
    {
        auto* sdlDevice = static_cast<SDL_GPUDevice*>(deviceHandle);
        if (sdlDevice != nullptr)
        {
            SDL_WaitForGPUIdle(sdlDevice);
        }

        meshes.clear();
        pipelineStates.clear();
        swapChains.clear();
        fences.clear();
        commandBuffers.clear();
        textures.clear();
        shaders.clear();
        buffers.clear();

        if (sdlDevice != nullptr)
        {
            if (fallbackTexture != nullptr)
            {
                SDL_ReleaseGPUTexture(sdlDevice, static_cast<SDL_GPUTexture*>(fallbackTexture));
                fallbackTexture = nullptr;
            }
            if (defaultSampler != nullptr)
            {
                SDL_ReleaseGPUSampler(sdlDevice, static_cast<SDL_GPUSampler*>(defaultSampler));
                defaultSampler = nullptr;
            }

            SDL_DestroyGPUDevice(sdlDevice);
            deviceHandle = nullptr;
        }
    }
} // namespace Garage
