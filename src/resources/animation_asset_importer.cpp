#include "garage/resources/animation_asset_importer.hpp"

#include <cmath>
#include <filesystem>
#include <fstream>
#include <ios>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include "garage/core/allocator.hpp"
#include "garage/core/binary_reader.hpp"
#include "garage/core/types.hpp"

#include "garage/resources/animation_asset.hpp"

namespace Garage
{
    Result<Allocator::UniquePtr<AnimationAsset>, std::pmr::string>
            AnimationAssetImporter::Import(const std::filesystem::path& path) const
    {
        if (path.extension() != ".tukanim")
        {
            return Result<Allocator::UniquePtr<AnimationAsset>, std::pmr::string>::Err(
                    Allocator::MakeString("애니메이션 로더에는 .tukanim 파일이 필요합니다."));
        }

        std::ifstream file(path, std::ios::binary);
        if (!file)
        {
            std::pmr::string error = Allocator::MakeString("리소스를 열 수 없습니다: ");
            error.append(path.generic_string());
            return Result<Allocator::UniquePtr<AnimationAsset>, std::pmr::string>::Err(std::move(error));
        }

        BinaryReader     reader(file);
        std::pmr::string name{Allocator::GetMemoryResource()};
        Float32          length    = 0.0f;
        Float32          frameRate = 0.0f;
        if (!reader.Read(name, length, frameRate))
        {
            return Result<Allocator::UniquePtr<AnimationAsset>, std::pmr::string>::Err(
                    Allocator::MakeString("애니메이션 헤더가 유효하지 않습니다."));
        }

        Int32 count = 0;
        if (!reader.ReadCount(count, sizeof(AnimationTransform)))
        {
            return Result<Allocator::UniquePtr<AnimationAsset>, std::pmr::string>::Err(
                    Allocator::MakeString("애니메이션 트랜스폼 개수가 유효하지 않습니다."));
        }

        std::pmr::vector<AnimationTransform> transforms(static_cast<USize>(count), Allocator::GetMemoryResource());
        for (AnimationTransform& transform : transforms)
        {
            if (!reader.Read(transform.path) || !reader.ReadCount(count, sizeof(AnimationFrame)))
            {
                return Result<Allocator::UniquePtr<AnimationAsset>, std::pmr::string>::Err(
                        Allocator::MakeString("애니메이션 트랜스폼 데이터가 유효하지 않습니다."));
            }

            transform.frames.resize(static_cast<USize>(count));
            for (AnimationFrame& frame : transform.frames)
            {
                if (!reader.Read(frame.time, frame.position, frame.rotation, frame.scale))
                {
                    return Result<Allocator::UniquePtr<AnimationAsset>, std::pmr::string>::Err(
                            Allocator::MakeString("애니메이션 프레임 데이터가 유효하지 않습니다."));
                }
            }
        }

        if (!reader.IsAtEnd())
        {
            return Result<Allocator::UniquePtr<AnimationAsset>, std::pmr::string>::Err(
                    Allocator::MakeString("애니메이션 파일 끝에 예상치 못한 데이터가 남아 있습니다."));
        }

        if (length < 0.0f || frameRate <= 0.0f || transforms.empty())
        {
            return Result<Allocator::UniquePtr<AnimationAsset>, std::pmr::string>::Err(
                    Allocator::MakeString("애니메이션 시간 정보 또는 트랜스폼 개수가 유효하지 않습니다."));
        }

        std::pmr::unordered_set<std::pmr::string> paths{Allocator::GetMemoryResource()};
        for (const AnimationTransform& transform : transforms)
        {
            if (!paths.insert(transform.path).second || transform.frames.empty())
            {
                return Result<Allocator::UniquePtr<AnimationAsset>, std::pmr::string>::Err(
                        Allocator::MakeString("중복된 애니메이션 대상이 있거나 프레임이 비어 있습니다."));
            }

            Float32 previous = -1.0f;
            for (const AnimationFrame& frame : transform.frames)
            {
                Float32 magnitude = 0.0f;
                for (const Float32 COMPONENT : frame.rotation)
                {
                    magnitude += COMPONENT * COMPONENT;
                }
                if (frame.time < 0.0f || frame.time < previous || frame.time > length || !std::isfinite(magnitude) ||
                    magnitude < 0.000001f)
                {
                    return Result<Allocator::UniquePtr<AnimationAsset>, std::pmr::string>::Err(
                            Allocator::MakeString("애니메이션 프레임 시간 또는 회전값이 유효하지 않습니다."));
                }
                previous = frame.time;
            }
        }

        return Result<Allocator::UniquePtr<AnimationAsset>, std::pmr::string>::Ok(
                Allocator::MakeUnique<AnimationAsset>(path, std::move(name), length, frameRate, std::move(transforms)));
    }
} // namespace Garage
