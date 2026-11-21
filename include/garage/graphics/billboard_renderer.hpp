#ifndef GARAGE_GRAPHICS_BILLBOARD_RENDERER_HPP
#define GARAGE_GRAPHICS_BILLBOARD_RENDERER_HPP

#include "../core/common.hpp"

#include "../math/vector2d.hpp"

#include "renderer.hpp"

namespace Garage
{
    class SystemContext;
    class TextureContent;

    class GARAGE_API BillboardRenderer final : public RendererBase
    {
    public:
        using RendererBase::RendererBase;
        ~BillboardRenderer() noexcept override = default;

        void OnUpdate(SystemContext& context) override;
        void OnRender(SystemContext& context) override;

        void Play(Float32 duration,
                  UInt32  columns               = 1,
                  UInt32  rows                  = 1,
                  bool    isLooping             = false,
                  bool    isAutoDestroyOnFinish = true) noexcept;
        void Stop() noexcept;

        [[nodiscard]] const TextureContent* GetTexture() const noexcept;
        void                                SetTexture(const TextureContent* value) noexcept;

        [[nodiscard]] const Vector2D<Float32>& GetSize() const noexcept;
        void                                   SetSize(const Vector2D<Float32>& value) noexcept;

        [[nodiscard]] UInt32 GetFrameIndex() const noexcept;
        void                 SetFrameIndex(UInt32 value) noexcept;

        [[nodiscard]] UInt32 GetColumns() const noexcept;
        void                 SetColumns(UInt32 value) noexcept;

        [[nodiscard]] UInt32 GetRows() const noexcept;
        void                 SetRows(UInt32 value) noexcept;

        [[nodiscard]] Float32 GetDuration() const noexcept;
        void                  SetDuration(Float32 value) noexcept;

        [[nodiscard]] bool IsPlaying() const noexcept;
        [[nodiscard]] bool IsLooping() const noexcept;
        void               SetLooping(bool value) noexcept;

        [[nodiscard]] bool IsAutoDestroyOnFinish() const noexcept;
        void               SetAutoDestroyOnFinish(bool value) noexcept;

    private:
        Vector2D<Float32> size{1.0f, 1.0f};
        UInt32            frameIndex = 0;
        UInt32            columns    = 1;
        UInt32            rows       = 1;

        Float32 duration              = 0.0f;
        Float32 elapsedTime           = 0.0f;
        bool    isPlaying             = false;
        bool    isLooping             = false;
        bool    isAutoDestroyOnFinish = false;

        const TextureContent* texture = nullptr;
    };

    inline const TextureContent* BillboardRenderer::GetTexture() const noexcept
    {
        return texture;
    }

    inline void BillboardRenderer::SetTexture(const TextureContent* value) noexcept
    {
        texture = value;
    }

    inline const Vector2D<Float32>& BillboardRenderer::GetSize() const noexcept
    {
        return size;
    }

    inline void BillboardRenderer::SetSize(const Vector2D<Float32>& value) noexcept
    {
        size = value;
    }

    inline UInt32 BillboardRenderer::GetFrameIndex() const noexcept
    {
        return frameIndex;
    }

    inline void BillboardRenderer::SetFrameIndex(UInt32 value) noexcept
    {
        frameIndex = value;
    }

    inline UInt32 BillboardRenderer::GetColumns() const noexcept
    {
        return columns;
    }

    inline void BillboardRenderer::SetColumns(UInt32 value) noexcept
    {
        columns = value;
    }

    inline UInt32 BillboardRenderer::GetRows() const noexcept
    {
        return rows;
    }

    inline void BillboardRenderer::SetRows(UInt32 value) noexcept
    {
        rows = value;
    }

    inline Float32 BillboardRenderer::GetDuration() const noexcept
    {
        return duration;
    }

    inline void BillboardRenderer::SetDuration(Float32 value) noexcept
    {
        duration = value;
    }

    inline bool BillboardRenderer::IsPlaying() const noexcept
    {
        return isPlaying;
    }

    inline bool BillboardRenderer::IsLooping() const noexcept
    {
        return isLooping;
    }

    inline void BillboardRenderer::SetLooping(bool value) noexcept
    {
        isLooping = value;
    }

    inline bool BillboardRenderer::IsAutoDestroyOnFinish() const noexcept
    {
        return isAutoDestroyOnFinish;
    }

    inline void BillboardRenderer::SetAutoDestroyOnFinish(bool value) noexcept
    {
        isAutoDestroyOnFinish = value;
    }
} // namespace Garage

#endif // GARAGE_GRAPHICS_BILLBOARD_RENDERER_HPP
