#ifndef GARAGE_CORE_MEMORY_POOL_ALLOCATOR_HPP
#define GARAGE_CORE_MEMORY_POOL_ALLOCATOR_HPP

#include "allocator.hpp"

namespace Garage::Memory
{
    class PoolAllocator final : public IAllocatable
    {
    public:
        GARAGE_API PoolAllocator(std::size_t blockSize,
                                 std::size_t blockCount,
                                 std::size_t blockAlignment = DEFAULT_ALIGNMENT) noexcept;
        GARAGE_API ~PoolAllocator() noexcept override;

        [[nodiscard]] GARAGE_API void* Allocate(std::size_t size, std::size_t alignment = DEFAULT_ALIGNMENT) override;
        GARAGE_API void                Deallocate(void*       pointer,
                                                  std::size_t size,
                                                  std::size_t alignment = DEFAULT_ALIGNMENT) noexcept override;

    private:
        struct FreeBlock
        {
            FreeBlock* next = nullptr;
        };

        std::byte*  buffer         = nullptr;
        FreeBlock*  freeList       = nullptr;
        std::size_t blockSize      = 0;
        std::size_t blockAlignment = 0;
        std::size_t stride         = 0;
        std::size_t capacity       = 0;
    };
} // namespace Garage::Memory

#endif // GARAGE_CORE_MEMORY_POOL_ALLOCATOR_HPP
