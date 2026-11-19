#ifndef GARAGE_CORE_MEMORY_MEMORY_RESOURCE_HPP
#define GARAGE_CORE_MEMORY_MEMORY_RESOURCE_HPP

#include <memory_resource>
#include <string>
#include <string_view>

#include "../common/defines.hpp"

namespace Garage
{
    namespace Memory
    {
        [[nodiscard]] GARAGE_API std::pmr::memory_resource* GetMemoryResource() noexcept;
        [[nodiscard]] inline std::pmr::string               MakeString(std::string_view text);

        inline std::pmr::string MakeString(std::string_view text)
        {
            return std::pmr::string(text, GetMemoryResource());
        }
    } // namespace Memory
} // namespace Garage

#endif // GARAGE_CORE_MEMORY_MEMORY_RESOURCE_HPP
