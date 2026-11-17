#ifndef GARAGE_GRAPHICS_GPU_DEVICE_HPP
#define GARAGE_GRAPHICS_GPU_DEVICE_HPP

#include <span>

#include "../core/defines.hpp"
#include "../core/types.hpp"

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

        [[nodiscard]] virtual IGPUBuffer* CreateBuffer(EGPUBufferUsage usage, UInt64 sizeInBytes) = 0;
        [[nodiscard]] virtual IGPUBuffer* CreateUploadBuffer(std::span<const Byte> data)          = 0;
        virtual void                      DestroyBuffer(IGPUBuffer* buffer) noexcept              = 0;

        [[nodiscard]] virtual IGPUShader* CreateShader(const GPUShaderCreateInfo& createInfo) = 0;
        virtual void                      DestroyShader(IGPUShader* shader) noexcept          = 0;

        [[nodiscard]] virtual IGPUTexture2D* CreateTexture2D(const GPUTexture2DCreateInfo& createInfo) = 0;
        virtual void                         DestroyTexture2D(IGPUTexture2D* texture) noexcept         = 0;

        [[nodiscard]] virtual IGPUCommandBuffer* CreateCommandBuffer()                                           = 0;
        virtual void                             DestroyCommandBuffer(IGPUCommandBuffer* commandBuffer) noexcept = 0;

        [[nodiscard]] virtual IGPUFence* CreateFence()                           = 0;
        virtual void                     DestroyFence(IGPUFence* fence) noexcept = 0;

        [[nodiscard]] virtual IGPUSwapChain* CreateSwapChain(const IDisplay* display)            = 0;
        virtual void                         DestroySwapChain(IGPUSwapChain* swapChain) noexcept = 0;

        [[nodiscard]] virtual IGPUGraphicsPipelineState*
                     CreateGraphicsPipelineState(const GPUGraphicsPipelineDesc& description)         = 0;
        virtual void DestroyGraphicsPipelineState(IGPUGraphicsPipelineState* pipelineState) noexcept = 0;

        [[nodiscard]] virtual IGPUMesh* CreateMesh(const IGPUBuffer*     vertexBuffer,
                                                   UInt32                vertexStride,
                                                   const IGPUBuffer*     indexBuffer,
                                                   EGPUIndexFormat       indexFormat,
                                                   EGPUPrimitiveTopology primitive) = 0;
        [[nodiscard]] virtual IGPUMesh* CreateMesh(std::span<const Byte> vertexData,
                                                   UInt32                vertexStride,
                                                   std::span<const Byte> indexData,
                                                   EGPUIndexFormat       indexFormat,
                                                   EGPUPrimitiveTopology primitive) = 0;
        virtual void                    DestroyMesh(IGPUMesh* mesh) noexcept        = 0;
    };
} // namespace Garage

#endif // GARAGE_GRAPHICS_GPU_DEVICE_HPP
