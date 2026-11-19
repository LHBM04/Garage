#ifndef GARAGE_GRAPHICS_GPU_DEVICE_IMPL_HPP
#define GARAGE_GRAPHICS_GPU_DEVICE_IMPL_HPP

#include <SDL3/SDL_gpu.h>

#include "garage/core/common.hpp"
#include "garage/core/common/containers.hpp"
#include "garage/core/memory.hpp"

#include "garage/graphics/gpu_device.hpp"
#include "garage/graphics/gpu_types.hpp"

#include "gpu_buffer_impl.hpp"
#include "gpu_command_buffer_impl.hpp"
#include "gpu_fence_impl.hpp"
#include "gpu_graphics_pipeline_state_impl.hpp"
#include "gpu_mesh_impl.hpp"
#include "gpu_shader_impl.hpp"
#include "gpu_swap_chain_impl.hpp"
#include "gpu_texture2d_impl.hpp"

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

    class GPUDeviceImpl final : public IGPUDevice
    {
    public:
        GPUDeviceImpl(SDL_GPUDevice* device, SDL_GPUSampler* defaultSampler, SDL_GPUTexture* fallbackTexture) noexcept;
        ~GPUDeviceImpl() noexcept override;

        [[nodiscard]] Result<IGPUBuffer*, String> CreateBuffer(EGPUBufferUsage usage, UInt64 sizeInBytes) override;
        [[nodiscard]] Result<IGPUBuffer*, String> CreateUploadBuffer(Span<const Byte> data) override;
        void                                      DestroyBuffer(IGPUBuffer* buffer) noexcept override;

        [[nodiscard]] Result<IGPUShader*, String> CreateShader(const GPUShaderCreateInfo& createInfo) override;
        void                                      DestroyShader(IGPUShader* shader) noexcept override;

        [[nodiscard]] Result<IGPUTexture2D*, String> CreateTexture2D(const GPUTexture2DCreateInfo& createInfo) override;
        [[nodiscard]] Result<IGPUTexture2D*, String>
                CreateTexture2D(const GPUTexture2DCreateInfo&         createInfo,
                                Span<const GPUTextureSubresourceData> subresources) override;
        void    DestroyTexture2D(IGPUTexture2D* texture) noexcept override;

        [[nodiscard]] Result<IGPUCommandBuffer*, String> CreateCommandBuffer() override;
        void DestroyCommandBuffer(IGPUCommandBuffer* commandBuffer) noexcept override;

        [[nodiscard]] Result<IGPUFence*, String> CreateFence() override;
        void                                     DestroyFence(IGPUFence* fence) noexcept override;

        [[nodiscard]] Result<IGPUSwapChain*, String> CreateSwapChain(const IDisplay* display) override;
        void                                         DestroySwapChain(IGPUSwapChain* swapChain) noexcept override;

        [[nodiscard]] Result<IGPUGraphicsPipelineState*, String>
                CreateGraphicsPipelineState(const GPUGraphicsPipelineDesc& description) override;
        void    DestroyGraphicsPipelineState(IGPUGraphicsPipelineState* pipelineState) noexcept override;

        [[nodiscard]] Result<IGPUMesh*, String> CreateMesh(const IGPUBuffer*     vertexBuffer,
                                                           UInt32                vertexStride,
                                                           const IGPUBuffer*     indexBuffer,
                                                           EGPUIndexFormat       indexFormat,
                                                           EGPUPrimitiveTopology primitive) override;
        [[nodiscard]] Result<IGPUMesh*, String> CreateMesh(Span<const Byte>      vertexData,
                                                           UInt32                vertexStride,
                                                           Span<const Byte>      indexData,
                                                           EGPUIndexFormat       indexFormat,
                                                           EGPUPrimitiveTopology primitive) override;
        void                                    DestroyMesh(IGPUMesh* mesh) noexcept override;

        void Release() noexcept;

        [[nodiscard]] SDL_GPUDevice*  GetSDLDevice() const noexcept;
        [[nodiscard]] SDL_GPUSampler* GetDefaultSampler() const noexcept;
        [[nodiscard]] SDL_GPUTexture* GetFallbackTexture() const noexcept;

    private:
        SDL_GPUDevice*                                  device          = nullptr;
        SDL_GPUSampler*                                 defaultSampler  = nullptr;
        SDL_GPUTexture*                                 fallbackTexture = nullptr;
        Vector<UniquePtr<GPUBufferImpl>>                buffers;
        Vector<UniquePtr<GPUShaderImpl>>                shaders;
        Vector<UniquePtr<GPUTexture2DImpl>>             textures;
        Vector<UniquePtr<GPUCommandBufferImpl>>         commandBuffers;
        Vector<UniquePtr<GPUFenceImpl>>                 fences;
        Vector<UniquePtr<GPUSwapChainImpl>>             swapChains;
        Vector<UniquePtr<GPUGraphicsPipelineStateImpl>> pipelineStates;
        Vector<UniquePtr<GPUMeshImpl>>                  meshes;
    };

    inline SDL_GPUDevice* GPUDeviceImpl::GetSDLDevice() const noexcept
    {
        return device;
    }

    inline SDL_GPUSampler* GPUDeviceImpl::GetDefaultSampler() const noexcept
    {
        return defaultSampler;
    }

    inline SDL_GPUTexture* GPUDeviceImpl::GetFallbackTexture() const noexcept
    {
        return fallbackTexture;
    }
} // namespace Garage

#endif // GARAGE_GRAPHICS_GPU_DEVICE_IMPL_HPP
