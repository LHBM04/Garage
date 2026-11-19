#ifndef GARAGE_RESOURCES_RESOURCE_PATH_HPP
#define GARAGE_RESOURCES_RESOURCE_PATH_HPP

#include <filesystem>

#include "../core/common.hpp"
#include "../core/common/containers.hpp"

namespace Garage
{
    [[nodiscard]] inline Result<std::filesystem::path, String>
            FindResourceRoot(const std::filesystem::path& resourcePath, const std::filesystem::path& category);
    [[nodiscard]] inline Result<std::filesystem::path, String> ResolveResourcePath(const std::filesystem::path& root,
                                                                                   StringView relativePath);

    inline Result<std::filesystem::path, String> FindResourceRoot(const std::filesystem::path& resourcePath,
                                                                  const std::filesystem::path& category)
    {
        for (std::filesystem::path directory = resourcePath.parent_path(); !directory.empty();
             directory                       = directory.parent_path())
        {
            if (directory.filename() == category)
            {
                return Result<std::filesystem::path, String>::Ok(directory.parent_path());
            }
            if (directory == directory.parent_path())
            {
                break;
            }
        }

        String error("리소스 분류 디렉터리를 찾을 수 없습니다: ");
        error.append(resourcePath.generic_string());
        return Result<std::filesystem::path, String>::Err(std::move(error));
    }

    inline Result<std::filesystem::path, String> ResolveResourcePath(const std::filesystem::path& root,
                                                                     StringView                   relativePath)
    {
        if (relativePath.empty() || relativePath.find('\0') != StringView::npos)
        {
            return Result<std::filesystem::path, String>::Err(
                    String("리소스 참조 경로가 비어 있거나 유효하지 않습니다."));
        }

        const U8String        U8_PATH(relativePath.begin(), relativePath.end());
        std::filesystem::path path = std::filesystem::path(U8_PATH).lexically_normal();
        if (root.filename() == "Resources" && !path.empty() && *path.begin() == "Resources")
        {
            path = path.lexically_relative("Resources");
        }

        if (path.empty() || path.has_root_path() || *path.begin() == "..")
        {
            String error("리소스 참조 경로는 루트 디렉터리 내부여야 합니다: ");
            error.append(relativePath);
            return Result<std::filesystem::path, String>::Err(std::move(error));
        }

        return Result<std::filesystem::path, String>::Ok((root / path).lexically_normal());
    }
} // namespace Garage

#endif // GARAGE_RESOURCES_RESOURCE_PATH_HPP
