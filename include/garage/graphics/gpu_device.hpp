#ifndef GARAGE_GRAPHICS_GPU_DEVICE_HPP
#define GARAGE_GRAPHICS_GPU_DEVICE_HPP

#include "../core/common.hpp"
#include "../core/common/containers.hpp"

#include "gpu_types.hpp"

namespace Garage
{
    struct GPUGraphicsPipelineDesc;
    struct GPUShaderCreateInfo;
    struct GPUTexture2DCreateInfo;

    class IDisplay;
    class IGPUBuffer;
    class IGPUCommandBuffer;
    class IGPUFence;
    class IGPUGraphicsPipelineState;
    class IGPUMesh;
    class IGPUShader;
    class IGPUSwapChain;
    class IGPUTexture2D;

    class GARAGE_API IGPUDevice
    {
    public:
        IGPUDevice() noexcept          = default;
        virtual ~IGPUDevice() noexcept = default;

        IGPUDevice(const IGPUDevice&)            = delete;
        IGPUDevice& operator=(const IGPUDevice&) = delete;

        IGPUDevice(IGPUDevice&&)            = delete;
        IGPUDevice& operator=(IGPUDevice&&) = delete;

        [[nodiscard]] virtual Result<IGPUBuffer*, String> CreateBuffer(EGPUBufferUsage usage, UInt64 sizeInBytes) = 0;
        [[nodiscard]] virtual Result<IGPUBuffer*, String> CreateUploadBuffer(Span<const Byte> data)               = 0;
        virtual void                                      DestroyBuffer(IGPUBuffer* buffer) noexcept              = 0;

        [[nodiscard]] virtual Result<IGPUShader*, String> CreateShader(const GPUShaderCreateInfo& createInfo) = 0;
        virtual void                                      DestroyShader(IGPUShader* shader) noexcept          = 0;

        [[nodiscard]] virtual Result<IGPUTexture2D*, String>
                CreateTexture2D(const GPUTexture2DCreateInfo& createInfo) = 0;
        [[nodiscard]] virtual Result<IGPUTexture2D*, String>
                     CreateTexture2D(const GPUTexture2DCreateInfo&         createInfo,
                                     Span<const GPUTextureSubresourceData> subresources) = 0;
        virtual void DestroyTexture2D(IGPUTexture2D* texture) noexcept                   = 0;

        [[nodiscard]] virtual Result<IGPUCommandBuffer*, String> CreateCommandBuffer() = 0;
        virtual void DestroyCommandBuffer(IGPUCommandBuffer* commandBuffer) noexcept   = 0;

        [[nodiscard]] virtual Result<IGPUFence*, String> CreateFence()                           = 0;
        virtual void                                     DestroyFence(IGPUFence* fence) noexcept = 0;

        [[nodiscard]] virtual Result<IGPUSwapChain*, String> CreateSwapChain(const IDisplay* display)            = 0;
        virtual void                                         DestroySwapChain(IGPUSwapChain* swapChain) noexcept = 0;

        [[nodiscard]] virtual Result<IGPUGraphicsPipelineState*, String>
                     CreateGraphicsPipelineState(const GPUGraphicsPipelineDesc& description)         = 0;
        virtual void DestroyGraphicsPipelineState(IGPUGraphicsPipelineState* pipelineState) noexcept = 0;

        [[nodiscard]] virtual Result<IGPUMesh*, String> CreateMesh(const IGPUBuffer*     vertexBuffer,
                                                                   UInt32                vertexStride,
                                                                   const IGPUBuffer*     indexBuffer,
                                                                   EGPUIndexFormat       indexFormat,
                                                                   EGPUPrimitiveTopology primitive) = 0;
        [[nodiscard]] virtual Result<IGPUMesh*, String> CreateMesh(Span<const Byte>      vertexData,
                                                                   UInt32                vertexStride,
                                                                   Span<const Byte>      indexData,
                                                                   EGPUIndexFormat       indexFormat,
                                                                   EGPUPrimitiveTopology primitive) = 0;
        virtual void                                    DestroyMesh(IGPUMesh* mesh) noexcept        = 0;
    };
} // namespace Garage

#endif // GARAGE_GRAPHICS_GPU_DEVICE_HPP
