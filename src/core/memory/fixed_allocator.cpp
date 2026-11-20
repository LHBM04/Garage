#include "garage/core/memory/fixed_allocator.hpp"

#include <algorithm>
#include <bit>
#include <memory>

#include "garage/core/memory/allocation_trace.hpp"

#include "tracy_allocation.hpp"

namespace Garage::Memory
{
#if defined(TRACY_ENABLE) || defined(GARAGE_DEBUG)
    namespace
    {
        constexpr char MEMORY_POOL_NAME[] = "FixedAllocator";
    } // namespace
#endif

    FixedAllocator::FixedAllocator(void* buffer, std::size_t capacity) noexcept
        : buffer(static_cast<std::byte*>(buffer))
        , capacity(capacity)
    {
        ASSERT(buffer != nullptr || capacity == 0);
    }

    FixedAllocator::~FixedAllocator() noexcept
    {
        Reset();
    }

    void* FixedAllocator::Allocate(std::size_t size, std::size_t alignment)
    {
#if defined(GARAGE_DEBUG)
        Detail::RecordAllocationRequest(this, MEMORY_POOL_NAME, size, alignment);
#endif
        ASSERT(std::has_single_bit(alignment));
        if (offset == capacity)
        {
            return nullptr;
        }

        void*             candidate      = buffer + offset;
        std::size_t       remaining      = capacity - offset;
        const std::size_t allocationSize = std::max(size, std::size_t{1});
        if (std::align(alignment, allocationSize, candidate, remaining) == nullptr)
        {
            return nullptr;
        }

#if defined(TRACY_ENABLE)
        Detail::ProfileSuballocation(this, candidate, allocationSize, MEMORY_POOL_NAME);
#endif
        offset = capacity - remaining + allocationSize;
        return candidate;
    }

    void FixedAllocator::Deallocate([[maybe_unused]] void*       pointer,
                                    [[maybe_unused]] std::size_t size,
                                    [[maybe_unused]] std::size_t alignment) noexcept
    {
    }

    void FixedAllocator::Reset() noexcept
    {
#if defined(TRACY_ENABLE)
        if (offset != 0)
        {
            Detail::ProfileSuballocationReset(this, MEMORY_POOL_NAME);
        }
#endif
        offset = 0;
    }
} // namespace Garage::Memory
