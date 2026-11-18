#include "garage/actors/transform.hpp"

#include <algorithm>
#include <cmath>
#include <vector>

#include "garage/core/defines.hpp"
#include "garage/core/types.hpp"

namespace Garage
{
    void Transform::OnDestroy()
    {
        if (parent != nullptr)
        {
            std::erase(parent->children, this);
            parent = nullptr;
        }

        for (Transform* child : children)
        {
            child->parent = nullptr;
            child->UpdateMatrices();
        }

        children.clear();
        UpdateMatrices();
    }

    const Vector3D<Float32>& Transform::GetLocalPosition() const noexcept
    {
        return localPosition;
    }

    void Transform::SetLocalPosition(const Vector3D<Float32>& position) noexcept
    {
        if (!position.IsFinite())
        {
            ABORT();
        }

        localPosition = position;
        UpdateMatrices();
    }

    const Quaternion<Float32>& Transform::GetLocalRotation() const noexcept
    {
        return localRotation;
    }

    void Transform::SetLocalRotation(const Quaternion<Float32>& rotation) noexcept
    {
        const Float32 LENGTH_SQUARED = rotation.GetSqrMagnitude();
        if (!rotation.IsFinite() || !std::isfinite(LENGTH_SQUARED) || LENGTH_SQUARED <= 0.0f)
        {
            ABORT();
        }

        localRotation = rotation.GetNormalized();
        UpdateMatrices();
    }

    const Vector3D<Float32>& Transform::GetLocalScale() const noexcept
    {
        return localScale;
    }

    void Transform::SetLocalScale(const Vector3D<Float32>& scale) noexcept
    {
        if (!scale.IsFinite())
        {
            ABORT();
        }

        localScale = scale;
        UpdateMatrices();
    }

    Matrix4x4<Float32> Transform::GetLocalMatrix() const noexcept
    {
        return Matrix4x4<Float32>::TRS(localPosition, localRotation, localScale);
    }

    void Transform::SetLocalMatrix(const Matrix4x4<Float32>& matrix) noexcept
    {
        constexpr Float32 TOLERANCE = 0.0001f;
        for (USize index = 0; index < 16; ++index)
        {
            if (!std::isfinite(matrix[index]))
            {
                ABORT();
            }
        }

        if (std::abs(matrix[3]) > TOLERANCE || std::abs(matrix[7]) > TOLERANCE || std::abs(matrix[11]) > TOLERANCE ||
            std::abs(matrix[15] - 1.0f) > TOLERANCE)
        {
            ABORT();
        }

        Vector3D<Float32>   position{};
        Quaternion<Float32> rotation{};
        Vector3D<Float32>   scale{};
        if (!matrix.TryDecompose(position, rotation, scale))
        {
            ABORT();
        }

        const Matrix4x4<Float32> RECOMPOSED = Matrix4x4<Float32>::TRS(position, rotation, scale);
        for (USize index = 0; index < 16; ++index)
        {
            if (!std::isfinite(RECOMPOSED[index]) ||
                std::abs(RECOMPOSED[index] - matrix[index]) > TOLERANCE * std::max(1.0f, std::abs(matrix[index])))
            {
                ABORT();
            }
        }

        localPosition = position;
        localRotation = rotation;
        localScale    = scale;
        UpdateMatrices();
    }

    Vector3D<Float32> Transform::GetWorldPosition() const noexcept
    {
        return cachedWorldMatrix.GetWorldPosition();
    }

    void Transform::SetWorldPosition(const Vector3D<Float32>& position) noexcept
    {
        if (parent != nullptr)
        {
            Matrix4x4<Float32> inverse{};
            if (!parent->GetWorldMatrix().TryGetInverse(inverse))
            {
                ABORT();
            }
            SetLocalPosition(inverse.MultiplyPoint3x4(position));
        }
        else
        {
            SetLocalPosition(position);
        }
    }

    Quaternion<Float32> Transform::GetWorldRotation() const noexcept
    {
        return (parent != nullptr) ? (localRotation * parent->GetWorldRotation()).GetNormalized() : localRotation;
    }

    void Transform::SetWorldRotation(const Quaternion<Float32>& rotation) noexcept
    {
        if (parent != nullptr)
        {
            SetLocalRotation(rotation * Quaternion<Float32>::Inverse(parent->GetWorldRotation()));
        }
        else
        {
            SetLocalRotation(rotation);
        }
    }

    Vector3D<Float32> Transform::GetWorldScale() const noexcept
    {
        if (parent == nullptr)
        {
            return localScale;
        }

        const Vector3D<Float32> PARENT_SCALE = parent->GetWorldScale();
        return Vector3D<Float32>(localScale.GetX() * PARENT_SCALE.GetX(),
                                 localScale.GetY() * PARENT_SCALE.GetY(),
                                 localScale.GetZ() * PARENT_SCALE.GetZ());
    }

    void Transform::SetWorldScale(const Vector3D<Float32>& scale) noexcept
    {
        if (parent != nullptr)
        {
            const Vector3D<Float32> PARENT_SCALE = parent->GetWorldScale();
            if (!PARENT_SCALE.IsFinite() || PARENT_SCALE.GetX() == 0.0f || PARENT_SCALE.GetY() == 0.0f ||
                PARENT_SCALE.GetZ() == 0.0f)
            {
                ABORT();
            }
            SetLocalScale(Vector3D<Float32>(scale.GetX() / PARENT_SCALE.GetX(),
                                            scale.GetY() / PARENT_SCALE.GetY(),
                                            scale.GetZ() / PARENT_SCALE.GetZ()));
        }
        else
        {
            SetLocalScale(scale);
        }
    }

    const Matrix4x4<Float32>& Transform::GetWorldMatrix() const noexcept
    {
        return cachedWorldMatrix;
    }

    void Transform::SetWorldMatrix(const Matrix4x4<Float32>& matrix) noexcept
    {
        if (parent != nullptr)
        {
            Matrix4x4<Float32> inverse{};
            if (!parent->GetWorldMatrix().TryGetInverse(inverse))
            {
                ABORT();
            }
            SetLocalMatrix(matrix * inverse);
        }
        else
        {
            SetLocalMatrix(matrix);
        }
    }

    Transform* Transform::GetParent() noexcept
    {
        return parent;
    }

    const Transform* Transform::GetParent() const noexcept
    {
        return parent;
    }

    std::span<Transform* const> Transform::GetChildren() noexcept
    {
        return children;
    }

    std::span<const Transform* const> Transform::GetChildren() const noexcept
    {
        return children;
    }

    void Transform::SetParent(Transform* nextParent) noexcept
    {
        if (parent == nextParent)
        {
            return;
        }

        for (const Transform* ancestor = nextParent; ancestor != nullptr; ancestor = ancestor->parent)
        {
            if (ancestor == this)
            {
                ABORT();
            }
        }

        if (nextParent != nullptr)
        {
            nextParent->children.push_back(this);
        }

        if (parent != nullptr)
        {
            std::erase(parent->children, this);
        }

        parent = nextParent;
        UpdateMatrices();
    }

    void Transform::UpdateMatrices() noexcept
    {
        cachedWorldMatrix = (parent != nullptr) ? (GetLocalMatrix() * parent->GetWorldMatrix()) : GetLocalMatrix();
        for (Transform* child : children)
        {
            child->UpdateMatrices();
        }
    }
} // namespace Garage
