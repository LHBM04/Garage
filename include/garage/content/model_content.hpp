#ifndef GARAGE_CONTENT_MODEL_CONTENT_HPP
#define GARAGE_CONTENT_MODEL_CONTENT_HPP

#include <filesystem>
#include <utility>

#include "../core/common.hpp"
#include "../core/common/containers.hpp"

#include "../math/quaternion.hpp"
#include "../math/vector3d.hpp"

#include "content.hpp"

namespace Garage
{
    class MaterialContent;
    class MeshContent;

    struct GARAGE_API Submodel final
    {
        Int32  parentIndex = -1;
        String name;

        Vector3D<Float32>   localPosition{0.0f, 0.0f, 0.0f};
        Quaternion<Float32> localRotation{0.0f, 0.0f, 0.0f, 1.0f};
        Vector3D<Float32>   localScale{1.0f, 1.0f, 1.0f};

        const MeshContent*     mesh     = nullptr;
        const MaterialContent* material = nullptr;
    };

    class GARAGE_API ModelContent final : public ContentBase
    {
    public:
        explicit ModelContent(std::filesystem::path path, Vector<Submodel>&& submodels) noexcept;

        [[nodiscard]] Span<const Submodel> GetSubmodels() const noexcept;

    private:
        Vector<Submodel> submodels;
    };

    inline ModelContent::ModelContent(std::filesystem::path path, Vector<Submodel>&& submodels) noexcept
        : ContentBase(std::move(path))
        , submodels(std::move(submodels))
    {
    }

    inline Span<const Submodel> ModelContent::GetSubmodels() const noexcept
    {
        return submodels;
    }
} // namespace Garage

#endif // GARAGE_CONTENT_MODEL_CONTENT_HPP
