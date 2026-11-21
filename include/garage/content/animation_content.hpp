#ifndef GARAGE_CONTENT_ANIMATION_CONTENT_HPP
#define GARAGE_CONTENT_ANIMATION_CONTENT_HPP

#include <filesystem>
#include <utility>

#include "../core/common.hpp"
#include "../core/common/containers.hpp"

#include "content.hpp"

namespace Garage
{
    struct GARAGE_API AnimationFrame final
    {
        Float32           time = 0.0f;
        Array<Float32, 3> position{};
        Array<Float32, 4> rotation{};
        Array<Float32, 3> scale{};
    };

    struct GARAGE_API AnimationTransform final
    {
        String                 path;
        Vector<AnimationFrame> frames;
    };

    class GARAGE_API AnimationContent final : public ContentBase
    {
    public:
        explicit AnimationContent(std::filesystem::path        path,
                                  String                       name,
                                  Float32                      length,
                                  Float32                      frameRate,
                                  Vector<AnimationTransform>&& transforms) noexcept;

        [[nodiscard]] const String&                  GetName() const noexcept;
        [[nodiscard]] Float32                        GetLength() const noexcept;
        [[nodiscard]] Float32                        GetFrameRate() const noexcept;
        [[nodiscard]] Span<const AnimationTransform> GetTransforms() const noexcept;

    private:
        String                     name;
        Float32                    length    = 0.0f;
        Float32                    frameRate = 0.0f;
        Vector<AnimationTransform> transforms;
    };

    inline AnimationContent::AnimationContent(std::filesystem::path        path,
                                              String                       name,
                                              Float32                      length,
                                              Float32                      frameRate,
                                              Vector<AnimationTransform>&& transforms) noexcept
        : ContentBase(std::move(path))
        , name(std::move(name))
        , length(length)
        , frameRate(frameRate)
        , transforms(std::move(transforms))
    {
    }

    inline const String& AnimationContent::GetName() const noexcept
    {
        return name;
    }

    inline Float32 AnimationContent::GetLength() const noexcept
    {
        return length;
    }

    inline Float32 AnimationContent::GetFrameRate() const noexcept
    {
        return frameRate;
    }

    inline Span<const AnimationTransform> AnimationContent::GetTransforms() const noexcept
    {
        return transforms;
    }
} // namespace Garage

#endif // GARAGE_CONTENT_ANIMATION_CONTENT_HPP
