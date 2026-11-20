#ifndef GARAGE_CORE_MEMORY_FIXED_ALLOCATOR_HPP
#define GARAGE_CORE_MEMORY_FIXED_ALLOCATOR_HPP

#include "allocator.hpp"

namespace Garage::Memory
{
    // The caller owns the buffer. Destroy all allocations and their owners before Reset() or buffer destruction.
    class FixedAllocator final : public IAllocatable
    {
    public:
        GARAGE_API FixedAllocator(void* buffer, std::size_t capacity) noexcept;
        GARAGE_API ~FixedAllocator() noexcept override;

        [[nodiscard]] GARAGE_API void* Allocate(std::size_t size, std::size_t alignment = DEFAULT_ALIGNMENT) override;
        GARAGE_API void                Deallocate(void*       pointer,
                                                  std::size_t size,
                                                  std::size_t alignment = DEFAULT_ALIGNMENT) noexcept override;
        GARAGE_API void                Reset() noexcept;

    private:
        std::byte*  buffer   = nullptr;
        std::size_t capacity = 0;
        std::size_t offset   = 0;
    };
} // namespace Garage::Memory

#endif // GARAGE_CORE_MEMORY_FIXED_ALLOCATOR_HPP
