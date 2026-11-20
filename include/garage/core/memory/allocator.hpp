#ifndef GARAGE_CORE_MEMORY_ALLOCATOR_HPP
#define GARAGE_CORE_MEMORY_ALLOCATOR_HPP

#include <cstddef>

#include "../common/defines.hpp"

namespace Garage::Memory
{
    inline constexpr std::size_t DEFAULT_ALIGNMENT = alignof(std::max_align_t);

    class IAllocatable
    {
    public:
        IAllocatable() noexcept          = default;
        virtual ~IAllocatable() noexcept = default;

        IAllocatable(const IAllocatable&) noexcept            = delete;
        IAllocatable& operator=(const IAllocatable&) noexcept = delete;

        IAllocatable(IAllocatable&&) noexcept            = delete;
        IAllocatable& operator=(IAllocatable&&) noexcept = delete;

        [[nodiscard]] virtual void* Allocate(std::size_t size, std::size_t alignment = DEFAULT_ALIGNMENT) = 0;
        virtual void
                Deallocate(void* pointer, std::size_t size, std::size_t alignment = DEFAULT_ALIGNMENT) noexcept = 0;
    };

    [[nodiscard]] GARAGE_API IAllocatable* GetDefaultAllocator() noexcept;
} // namespace Garage::Memory

#endif // GARAGE_CORE_MEMORY_ALLOCATOR_HPP
