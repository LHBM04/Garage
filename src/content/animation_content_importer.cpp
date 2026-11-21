#include "garage/content/animation_content_importer.hpp"

#include <array>
#include <cmath>
#include <filesystem>
#include <utility>

#include <rapidjson/document.h>

#include "garage/content/animation_content.hpp"

#include "garage/core/common.hpp"
#include "garage/core/common/containers.hpp"
#include "garage/core/memory.hpp"

#include "garage/utilities/json_utility.hpp"

namespace Garage
{
    Result<UniquePtr<AnimationContent>, String>
            AnimationContentImporter::Import(const std::filesystem::path& path) const
    {
        if (!JsonUtility::HasJsonExtension(path))
        {
            return Result<UniquePtr<AnimationContent>, String>::Err(
                    String("애니메이션 로더에는 .json 파일이 필요합니다."));
        }

        auto documentResult = JsonUtility::LoadDocument(path);
        if (!documentResult)
        {
            return Result<UniquePtr<AnimationContent>, String>::Err(std::move(documentResult).GetError());
        }

        const rapidjson::Value& ROOT = *documentResult;

        String  name;
        Float32 length    = 0.0f;
        Float32 frameRate = 0.0f;
        if (!JsonUtility::ReadString(ROOT, "name", name) || !JsonUtility::ReadFloat32(ROOT, "length", length) ||
            !JsonUtility::ReadFloat32(ROOT, "frameRate", frameRate))
        {
            return Result<UniquePtr<AnimationContent>, String>::Err(String("애니메이션 헤더가 유효하지 않습니다."));
        }

        const rapidjson::Value* transformsArray = nullptr;
        if (!JsonUtility::ReadArray(ROOT, "transforms", transformsArray) || transformsArray->Empty())
        {
            return Result<UniquePtr<AnimationContent>, String>::Err(
                    String("애니메이션 트랜스폼 개수가 유효하지 않습니다."));
        }

        Vector<AnimationTransform> transforms;
        transforms.reserve(transformsArray->Size());
        for (const rapidjson::Value& transformValue : transformsArray->GetArray())
        {
            const rapidjson::Value* framesArray = nullptr;
            AnimationTransform      transform{};
            if (!transformValue.IsObject() || !JsonUtility::ReadString(transformValue, "path", transform.path) ||
                !JsonUtility::ReadArray(transformValue, "frames", framesArray) || framesArray->Empty())
            {
                return Result<UniquePtr<AnimationContent>, String>::Err(
                        String("애니메이션 트랜스폼 데이터가 유효하지 않습니다."));
            }

            transform.frames.reserve(framesArray->Size());
            for (const rapidjson::Value& frameValue : framesArray->GetArray())
            {
                AnimationFrame frame{};
                if (!frameValue.IsObject() || !JsonUtility::ReadFloat32(frameValue, "time", frame.time) ||
                    !JsonUtility::ReadFloatArray(frameValue, "position", frame.position) ||
                    !JsonUtility::ReadFloatArray(frameValue, "rotation", frame.rotation) ||
                    !JsonUtility::ReadFloatArray(frameValue, "scale", frame.scale))
                {
                    return Result<UniquePtr<AnimationContent>, String>::Err(
                            String("애니메이션 프레임 데이터가 유효하지 않습니다."));
                }

                transform.frames.push_back(frame);
            }

            transforms.push_back(std::move(transform));
        }

        if (length < 0.0f || frameRate <= 0.0f || transforms.empty())
        {
            return Result<UniquePtr<AnimationContent>, String>::Err(
                    String("애니메이션 시간 정보 또는 트랜스폼 개수가 유효하지 않습니다."));
        }

        UnorderedSet<String> paths;
        for (const AnimationTransform& transform : transforms)
        {
            if (!paths.insert(transform.path).second || transform.frames.empty())
            {
                return Result<UniquePtr<AnimationContent>, String>::Err(
                        String("중복된 애니메이션 대상이 있거나 프레임이 비어 있습니다."));
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
                    return Result<UniquePtr<AnimationContent>, String>::Err(
                            String("애니메이션 프레임 시간 또는 회전값이 유효하지 않습니다."));
                }
                previous = frame.time;
            }
        }

        return Result<UniquePtr<AnimationContent>, String>::Ok(
                MakeUnique<AnimationContent>(path, std::move(name), length, frameRate, std::move(transforms)));
    }
} // namespace Garage
