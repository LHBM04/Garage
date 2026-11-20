#include "garage/core/memory/fixed_allocator.hpp"

#include <algorithm>
#include <bit>
#include <memory>

#include "garage/core/memory/allocation_trace.hpp"

namespace Garage::Memory
{
    FixedAllocator::FixedAllocator(void* buffer, std::size_t capacity) noexcept
        : buffer(static_cast<std::byte*>(buffer))
        , capacity(capacity)
    {
        ASSERT(buffer != nullptr || capacity == 0);
    }

    FixedAllocator::~FixedAllocator() noexcept = default;

    void* FixedAllocator::Allocate(std::size_t size, std::size_t alignment)
    {
#if defined(GARAGE_DEBUG)
        Detail::RecordAllocationRequest(this, "FixedAllocator", size, alignment);
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
        offset = 0;
    }
} // namespace Garage::Memory
