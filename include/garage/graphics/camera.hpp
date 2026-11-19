#ifndef GARAGE_GRAPHICS_CAMERA_HPP
#define GARAGE_GRAPHICS_CAMERA_HPP

#include "../actors/component.hpp"

#include "../core/common.hpp"

#include "../math/color_rgba.hpp"
#include "../math/matrix4x4.hpp"

#include "gpu_types.hpp"

namespace Garage
{
    enum class ECameraProjectionType : UInt8
    {
        PERSPECTIVE,
        ORTHOGRAPHIC,
    };

    class GARAGE_API Camera final : public ComponentBase
    {
    public:
        using ComponentBase::ComponentBase;
        ~Camera() noexcept override = default;

        void OnEnable() override;
        void OnDisable() override;

        [[nodiscard]] const ColorRGBA<Float32>& GetClearColor() const noexcept;
        void                                    SetClearColor(const ColorRGBA<Float32>& value) noexcept;

        [[nodiscard]] ECameraProjectionType GetProjectionType() const noexcept;
        void                                SetProjectionType(ECameraProjectionType value) noexcept;

        [[nodiscard]] Float32 GetFieldOfView() const noexcept;
        void                  SetFieldOfView(Float32 value) noexcept;

        [[nodiscard]] Float32 GetNearPlane() const noexcept;
        void                  SetNearPlane(Float32 value) noexcept;

        [[nodiscard]] Float32 GetFarPlane() const noexcept;
        void                  SetFarPlane(Float32 value) noexcept;

        [[nodiscard]] Float32 GetOrthoSize() const noexcept;
        void                  SetOrthoSize(Float32 value) noexcept;

        [[nodiscard]] const GPUViewport& GetViewport() const noexcept;
        void                             SetViewport(const GPUViewport& value) noexcept;

        [[nodiscard]] const GPURect& GetScissorRect() const noexcept;
        void                         SetScissorRect(const GPURect& value) noexcept;

        [[nodiscard]] Matrix4x4<Float32> GetViewMatrix() const noexcept;
        [[nodiscard]] Matrix4x4<Float32> GetProjectionMatrix() const noexcept;

    private:
        ColorRGBA<Float32>    clearColor     = ColorRGBA<Float32>::GetBlack();
        ECameraProjectionType projectionType = ECameraProjectionType::PERSPECTIVE;

        Float32 fieldOfView = 60.0f;

        Float32 nearPlane = 0.1f;
        Float32 farPlane  = 1000.0f;

        GPUViewport viewport{0.0f, 0.0f, 800.0f, 600.0f, 0.0f, 1.0f};
        GPURect     scissorRect{0, 0, 800, 600};

        Float32 orthoSize = 10.0f;
    };

    inline const ColorRGBA<Float32>& Camera::GetClearColor() const noexcept
    {
        return clearColor;
    }

    inline void Camera::SetClearColor(const ColorRGBA<Float32>& value) noexcept
    {
        clearColor = value;
    }

    inline ECameraProjectionType Camera::GetProjectionType() const noexcept
    {
        return projectionType;
    }

    inline void Camera::SetProjectionType(ECameraProjectionType value) noexcept
    {
        projectionType = value;
    }

    inline Float32 Camera::GetFieldOfView() const noexcept
    {
        return fieldOfView;
    }

    inline void Camera::SetFieldOfView(Float32 value) noexcept
    {
        fieldOfView = value;
    }

    inline Float32 Camera::GetNearPlane() const noexcept
    {
        return nearPlane;
    }

    inline void Camera::SetNearPlane(Float32 value) noexcept
    {
        nearPlane = value;
    }

    inline Float32 Camera::GetFarPlane() const noexcept
    {
        return farPlane;
    }

    inline void Camera::SetFarPlane(Float32 value) noexcept
    {
        farPlane = value;
    }

    inline Float32 Camera::GetOrthoSize() const noexcept
    {
        return orthoSize;
    }

    inline void Camera::SetOrthoSize(Float32 value) noexcept
    {
        orthoSize = value;
    }

    inline const GPUViewport& Camera::GetViewport() const noexcept
    {
        return viewport;
    }

    inline void Camera::SetViewport(const GPUViewport& value) noexcept
    {
        viewport = value;
    }

    inline const GPURect& Camera::GetScissorRect() const noexcept
    {
        return scissorRect;
    }

    inline void Camera::SetScissorRect(const GPURect& value) noexcept
    {
        scissorRect = value;
    }
} // namespace Garage

#endif // GARAGE_GRAPHICS_CAMERA_HPP
