#ifndef GARAGE_GRAPHICS_GPU_DEVICE_HPP
#define GARAGE_GRAPHICS_GPU_DEVICE_HPP

#include <memory>
#include <span>
#include <vector>

#include "../core/defines.hpp"
#include "../core/types.hpp"

#include "gpu_buffer.hpp"
#include "gpu_command_buffer.hpp"
#include "gpu_fence.hpp"
#include "gpu_graphics_pipeline_state.hpp"
#include "gpu_mesh.hpp"
#include "gpu_shader.hpp"
#include "gpu_swap_chain.hpp"
#include "gpu_texture2d.hpp"
#include "gpu_types.hpp"

namespace Garage
{
    class IDisplay;

    struct GARAGE_API GPUDeviceCreateInfo final
    {
        void* deviceHandle    = nullptr;
        void* defaultSampler  = nullptr;
        void* fallbackTexture = nullptr;
    };

    class GARAGE_API GPUDevice final
    {
    public:
        explicit GPUDevice(const GPUDeviceCreateInfo& createInfo) noexcept;
        ~GPUDevice() noexcept;

        GPUDevice(const GPUDevice&)            = delete;
        GPUDevice& operator=(const GPUDevice&) = delete;

        GPUDevice(GPUDevice&& other) noexcept;
        GPUDevice& operator=(GPUDevice&& other) noexcept;

        [[nodiscard]] GPUBuffer* CreateBuffer(EGPUBufferUsage usage, UInt64 sizeInBytes);
        [[nodiscard]] GPUBuffer* CreateUploadBuffer(std::span<const Byte> data);
        void                     DestroyBuffer(GPUBuffer* buffer) noexcept;

        [[nodiscard]] GPUShader* CreateShader(const GPUShaderCreateInfo& createInfo);
        void                     DestroyShader(GPUShader* shader) noexcept;

        [[nodiscard]] GPUTexture2D* CreateTexture2D(const GPUTexture2DCreateInfo& createInfo);
        void                        DestroyTexture2D(GPUTexture2D* texture) noexcept;

        [[nodiscard]] GPUCommandBuffer* CreateCommandBuffer();
        void                            DestroyCommandBuffer(GPUCommandBuffer* commandBuffer) noexcept;

        [[nodiscard]] GPUFence* CreateFence();
        void                    DestroyFence(GPUFence* fence) noexcept;

        [[nodiscard]] GPUSwapChain* CreateSwapChain(const IDisplay* display);
        void                        DestroySwapChain(GPUSwapChain* swapChain) noexcept;

        [[nodiscard]] GPUGraphicsPipelineState* CreateGraphicsPipelineState(const GPUGraphicsPipelineDesc& description);
        void DestroyGraphicsPipelineState(GPUGraphicsPipelineState* pipelineState) noexcept;

        [[nodiscard]] GPUMesh* CreateMesh(const GPUBuffer*      vertexBuffer,
                                          UInt32                vertexStride,
                                          const GPUBuffer*      indexBuffer,
                                          EGPUIndexFormat       indexFormat,
                                          EGPUPrimitiveTopology primitive);
        [[nodiscard]] GPUMesh* CreateMesh(std::span<const Byte> vertexData,
                                          UInt32                vertexStride,
                                          std::span<const Byte> indexData,
                                          EGPUIndexFormat       indexFormat,
                                          EGPUPrimitiveTopology primitive);
        void                   DestroyMesh(GPUMesh* mesh) noexcept;

        [[nodiscard]] void* GetNativeDevice() const noexcept;
        [[nodiscard]] void* GetDefaultSampler() const noexcept;
        [[nodiscard]] void* GetFallbackTexture() const noexcept;

    private:
        void Release() noexcept;

        void* deviceHandle    = nullptr;
        void* defaultSampler  = nullptr;
        void* fallbackTexture = nullptr;

        std::vector<std::unique_ptr<GPUBuffer>>                buffers        = {};
        std::vector<std::unique_ptr<GPUShader>>                shaders        = {};
        std::vector<std::unique_ptr<GPUTexture2D>>             textures       = {};
        std::vector<std::unique_ptr<GPUCommandBuffer>>         commandBuffers = {};
        std::vector<std::unique_ptr<GPUFence>>                 fences         = {};
        std::vector<std::unique_ptr<GPUSwapChain>>             swapChains     = {};
        std::vector<std::unique_ptr<GPUGraphicsPipelineState>> pipelineStates = {};
        std::vector<std::unique_ptr<GPUMesh>>                  meshes         = {};
    };

    inline void* GPUDevice::GetNativeDevice() const noexcept
    {
        return deviceHandle;
    }

    inline void* GPUDevice::GetDefaultSampler() const noexcept
    {
        return defaultSampler;
    }

    inline void* GPUDevice::GetFallbackTexture() const noexcept
    {
        return fallbackTexture;
    }
} // namespace Garage

#endif // GARAGE_GRAPHICS_GPU_DEVICE_HPP
