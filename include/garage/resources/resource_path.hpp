#ifndef GARAGE_RESOURCES_RESOURCE_PATH_HPP
#define GARAGE_RESOURCES_RESOURCE_PATH_HPP

#include <filesystem>
#include <string>
#include <string_view>

#include "../core/common.hpp"
#include "../core/memory.hpp"

namespace Garage
{
    [[nodiscard]] inline Result<std::filesystem::path, std::pmr::string>
            FindResourceRoot(const std::filesystem::path& resourcePath, const std::filesystem::path& category);
    [[nodiscard]] inline Result<std::filesystem::path, std::pmr::string>
            ResolveResourcePath(const std::filesystem::path& root, std::string_view relativePath);

    inline Result<std::filesystem::path, std::pmr::string> FindResourceRoot(const std::filesystem::path& resourcePath,
                                                                            const std::filesystem::path& category)
    {
        for (std::filesystem::path directory = resourcePath.parent_path(); !directory.empty();
             directory                       = directory.parent_path())
        {
            if (directory.filename() == category)
            {
                return Result<std::filesystem::path, std::pmr::string>::Ok(directory.parent_path());
            }
            if (directory == directory.parent_path())
            {
                break;
            }
        }

        std::pmr::string error = Memory::MakeString("리소스 분류 디렉터리를 찾을 수 없습니다: ");
        error.append(resourcePath.generic_string());
        return Result<std::filesystem::path, std::pmr::string>::Err(std::move(error));
    }

    inline Result<std::filesystem::path, std::pmr::string> ResolveResourcePath(const std::filesystem::path& root,
                                                                               std::string_view relativePath)
    {
        if (relativePath.empty() || relativePath.find('\0') != std::string_view::npos)
        {
            return Result<std::filesystem::path, std::pmr::string>::Err(
                    Memory::MakeString("리소스 참조 경로가 비어 있거나 유효하지 않습니다."));
        }

        const std::pmr::u8string U8_PATH(relativePath.begin(), relativePath.end(), Memory::GetMemoryResource());
        std::filesystem::path    path = std::filesystem::path(U8_PATH).lexically_normal();
        if (root.filename() == "Resources" && !path.empty() && *path.begin() == "Resources")
        {
            path = path.lexically_relative("Resources");
        }

        if (path.empty() || path.has_root_path() || *path.begin() == "..")
        {
            std::pmr::string error = Memory::MakeString("리소스 참조 경로는 루트 디렉터리 내부여야 합니다: ");
            error.append(relativePath);
            return Result<std::filesystem::path, std::pmr::string>::Err(std::move(error));
        }

        return Result<std::filesystem::path, std::pmr::string>::Ok((root / path).lexically_normal());
    }
} // namespace Garage

#endif // GARAGE_RESOURCES_RESOURCE_PATH_HPP
