#ifndef GARAGE_ACTORS_TRANSFORM_HPP
#define GARAGE_ACTORS_TRANSFORM_HPP

#include <span>
#include <vector>

#include "../core/defines.hpp"
#include "../core/memory.hpp"

#include "../math/matrix4x4.hpp"
#include "../math/quaternion.hpp"
#include "../math/vector3d.hpp"

#include "component.hpp"

namespace Garage
{
    class GARAGE_API Transform final : public ComponentBase
    {
    public:
        using ComponentBase::ComponentBase;
        ~Transform() noexcept override = default;

        [[nodiscard]] const Vector3D<Float32>& GetLocalPosition() const noexcept;
        void                                   SetLocalPosition(const Vector3D<Float32>& position) noexcept;

        [[nodiscard]] const Quaternion<Float32>& GetLocalRotation() const noexcept;
        void                                     SetLocalRotation(const Quaternion<Float32>& rotation) noexcept;

        [[nodiscard]] const Vector3D<Float32>& GetLocalScale() const noexcept;
        void                                   SetLocalScale(const Vector3D<Float32>& scale) noexcept;

        [[nodiscard]] Vector3D<Float32> GetWorldPosition() const noexcept;
        void                            SetWorldPosition(const Vector3D<Float32>& position) noexcept;

        [[nodiscard]] Quaternion<Float32> GetWorldRotation() const noexcept;
        void                              SetWorldRotation(const Quaternion<Float32>& rotation) noexcept;

        [[nodiscard]] Vector3D<Float32> GetWorldScale() const noexcept;
        void                            SetWorldScale(const Vector3D<Float32>& scale) noexcept;

        [[nodiscard]] Matrix4x4<Float32> GetLocalMatrix() const noexcept;
        void                             SetLocalMatrix(const Matrix4x4<Float32>& matrix) noexcept;

        [[nodiscard]] const Matrix4x4<Float32>& GetWorldMatrix() const noexcept;
        void                                    SetWorldMatrix(const Matrix4x4<Float32>& matrix) noexcept;

        [[nodiscard]] Transform*       GetParent() noexcept;
        [[nodiscard]] const Transform* GetParent() const noexcept;
        void                           SetParent(Transform* nextParent) noexcept;

        [[nodiscard]] std::span<Transform* const>       GetChildren() noexcept;
        [[nodiscard]] std::span<const Transform* const> GetChildren() const noexcept;

        void OnDestroy() override;

    private:
        void UpdateMatrices() noexcept;

        Vector3D<Float32>   localPosition = Vector3D<Float32>::GetZero();
        Quaternion<Float32> localRotation = Quaternion<Float32>::GetIdentity();
        Vector3D<Float32>   localScale    = Vector3D<Float32>::GetOne();

        Transform*                   parent = nullptr;
        std::pmr::vector<Transform*> children{Memory::GetMemoryResource()};

        Matrix4x4<Float32> cachedWorldMatrix = Matrix4x4<Float32>::GetIdentity();
    };
} // namespace Garage

#endif // GARAGE_ACTORS_TRANSFORM_HPP
