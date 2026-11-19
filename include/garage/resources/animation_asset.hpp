#ifndef GARAGE_RESOURCES_ANIMATION_ASSET_HPP
#define GARAGE_RESOURCES_ANIMATION_ASSET_HPP

#include <array>
#include <filesystem>
#include <span>
#include <string>
#include <utility>
#include <vector>

#include "../core/defines.hpp"
#include "../core/memory.hpp"
#include "../core/types.hpp"

#include "asset.hpp"

namespace Garage
{
    struct GARAGE_API AnimationFrame final
    {
        Float32                time = 0.0f;
        std::array<Float32, 3> position{};
        std::array<Float32, 4> rotation{};
        std::array<Float32, 3> scale{};
    };

    struct GARAGE_API AnimationTransform final
    {
        std::pmr::string                 path{Memory::GetMemoryResource()};
        std::pmr::vector<AnimationFrame> frames{Memory::GetMemoryResource()};
    };

    class GARAGE_API AnimationAsset final : public AssetBase
    {
    public:
        explicit AnimationAsset(std::filesystem::path                  path,
                                std::pmr::string                       name,
                                Float32                                length,
                                Float32                                frameRate,
                                std::pmr::vector<AnimationTransform>&& transforms) noexcept;

        [[nodiscard]] const std::pmr::string&             GetName() const noexcept;
        [[nodiscard]] Float32                             GetLength() const noexcept;
        [[nodiscard]] Float32                             GetFrameRate() const noexcept;
        [[nodiscard]] std::span<const AnimationTransform> GetTransforms() const noexcept;

    private:
        std::pmr::string                     name{Memory::GetMemoryResource()};
        Float32                              length    = 0.0f;
        Float32                              frameRate = 0.0f;
        std::pmr::vector<AnimationTransform> transforms{Memory::GetMemoryResource()};
    };

    inline AnimationAsset::AnimationAsset(std::filesystem::path                  path,
                                          std::pmr::string                       name,
                                          Float32                                length,
                                          Float32                                frameRate,
                                          std::pmr::vector<AnimationTransform>&& transforms) noexcept
        : AssetBase(std::move(path))
        , name(std::move(name))
        , length(length)
        , frameRate(frameRate)
        , transforms(std::move(transforms))
    {
    }

    inline const std::pmr::string& AnimationAsset::GetName() const noexcept
    {
        return name;
    }

    inline Float32 AnimationAsset::GetLength() const noexcept
    {
        return length;
    }

    inline Float32 AnimationAsset::GetFrameRate() const noexcept
    {
        return frameRate;
    }

    inline std::span<const AnimationTransform> AnimationAsset::GetTransforms() const noexcept
    {
        return transforms;
    }
} // namespace Garage

#endif // GARAGE_RESOURCES_ANIMATION_ASSET_HPP
