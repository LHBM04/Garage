#include "garage/core/memory/pool_allocator.hpp"

#include <algorithm>
#include <bit>
#include <cstdint>
#include <limits>
#include <memory>

#include "garage/core/memory/allocation_trace.hpp"

#include "tracy_allocation.hpp"

namespace Garage::Memory
{
#if defined(TRACY_ENABLE) || defined(GARAGE_DEBUG)
    namespace
    {
        constexpr char MEMORY_POOL_NAME[] = "PoolAllocator";
    } // namespace
#endif

    PoolAllocator::PoolAllocator(std::size_t blockSize, std::size_t blockCount, std::size_t blockAlignment) noexcept
        : blockSize(blockSize)
        , blockAlignment(blockAlignment)
    {
        ASSERT(blockSize != 0 && blockCount != 0 && std::has_single_bit(blockAlignment));

        const std::size_t storageAlignment = std::max(blockAlignment, alignof(FreeBlock));
        const std::size_t storageSize      = std::max(blockSize, sizeof(FreeBlock));
        const std::size_t paddingMask      = storageAlignment - 1;
        ASSERT(storageSize <= std::numeric_limits<std::size_t>::max() - paddingMask);
        stride = (storageSize + paddingMask) & ~paddingMask;
        ASSERT(blockCount <= std::numeric_limits<std::size_t>::max() / stride);
        capacity = stride * blockCount;

        buffer = static_cast<std::byte*>(GetDefaultAllocator()->Allocate(capacity, storageAlignment));
        for (std::size_t remaining = blockCount; remaining != 0; --remaining)
        {
            auto* block = reinterpret_cast<FreeBlock*>(buffer + (remaining - 1) * stride);
            freeList    = std::construct_at(block, FreeBlock{freeList});
        }
    }

    PoolAllocator::~PoolAllocator() noexcept
    {
#if defined(TRACY_ENABLE)
        Detail::ProfileSuballocationReset(this, MEMORY_POOL_NAME);
#endif
        GetDefaultAllocator()->Deallocate(buffer, capacity, std::max(blockAlignment, alignof(FreeBlock)));
    }

    void* PoolAllocator::Allocate(std::size_t size, std::size_t alignment)
    {
#if defined(GARAGE_DEBUG)
        Detail::RecordAllocationRequest(this, MEMORY_POOL_NAME, size, alignment);
#endif
        ASSERT(size <= blockSize);
        ASSERT(std::has_single_bit(alignment) && alignment <= blockAlignment);
        if (freeList == nullptr)
        {
            return nullptr;
        }

        FreeBlock* block = freeList;
#if defined(TRACY_ENABLE)
        Detail::ProfileSuballocation(this, block, size, MEMORY_POOL_NAME);
#endif
        freeList = block->next;
        std::destroy_at(block);
        return block;
    }

    void PoolAllocator::Deallocate(void* pointer, std::size_t size, std::size_t alignment) noexcept
    {
        if (pointer == nullptr)
        {
            return;
        }

        ASSERT(size <= blockSize);
        ASSERT(std::has_single_bit(alignment) && alignment <= blockAlignment);
        const auto address = reinterpret_cast<std::uintptr_t>(pointer);
        const auto start   = reinterpret_cast<std::uintptr_t>(buffer);
        ASSERT(address >= start && address - start < capacity && (address - start) % stride == 0);
#if defined(TRACY_ENABLE)
        Detail::ProfileSuballocationFree(this, pointer, MEMORY_POOL_NAME);
#endif
        freeList = std::construct_at(static_cast<FreeBlock*>(pointer), FreeBlock{freeList});
    }
} // namespace Garage::Memory
