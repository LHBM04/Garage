#ifndef GARAGE_CONTENT_CONTENT_PATH_HPP
#define GARAGE_CONTENT_CONTENT_PATH_HPP

#include <filesystem>

#include "../core/common.hpp"
#include "../core/common/containers.hpp"

namespace Garage
{
    [[nodiscard]] inline Result<std::filesystem::path, String> FindContentRoot(const std::filesystem::path& contentPath,
                                                                               const std::filesystem::path& category);
    [[nodiscard]] inline Result<std::filesystem::path, String> ResolveContentPath(const std::filesystem::path& root,
                                                                                  StringView relativePath);

    inline Result<std::filesystem::path, String> FindContentRoot(const std::filesystem::path& contentPath,
                                                                 const std::filesystem::path& category)
    {
        for (std::filesystem::path directory = contentPath.parent_path(); !directory.empty();
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

        String error("콘텐츠 분류 디렉터리를 찾을 수 없습니다: ");
        error.append(contentPath.generic_string());
        return Result<std::filesystem::path, String>::Err(std::move(error));
    }

    inline Result<std::filesystem::path, String> ResolveContentPath(const std::filesystem::path& root,
                                                                    StringView                   relativePath)
    {
        if (relativePath.empty() || relativePath.find('\0') != StringView::npos)
        {
            return Result<std::filesystem::path, String>::Err(
                    String("콘텐츠 참조 경로가 비어 있거나 유효하지 않습니다."));
        }

        const U8String        U8_PATH(relativePath.begin(), relativePath.end());
        std::filesystem::path path = std::filesystem::path(U8_PATH).lexically_normal();
        if (root.filename() == "content" && !path.empty() && *path.begin() == "content")
        {
            path = path.lexically_relative("content");
        }

        if (path.empty() || path.has_root_path() || *path.begin() == "..")
        {
            String error("콘텐츠 참조 경로는 루트 디렉터리 내부여야 합니다: ");
            error.append(relativePath);
            return Result<std::filesystem::path, String>::Err(std::move(error));
        }

        return Result<std::filesystem::path, String>::Ok((root / path).lexically_normal());
    }
} // namespace Garage

#endif // GARAGE_CONTENT_CONTENT_PATH_HPP
