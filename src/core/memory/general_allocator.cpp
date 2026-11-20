#include "garage/core/memory/general_allocator.hpp"

#include <algorithm>
#include <bit>
#include <new>

#if defined(TRACY_ENABLE)
#    include <tracy/Tracy.hpp>
#endif

#include "garage/core/memory/allocation_trace.hpp"

#include "tracy_allocation.hpp"

namespace Garage::Memory
{
#if defined(TRACY_ENABLE) || defined(GARAGE_DEBUG)
    namespace
    {
        constexpr char MEMORY_POOL_NAME[] = "GeneralAllocator";
    } // namespace
#endif

    GeneralAllocator::GeneralAllocator() noexcept  = default;
    GeneralAllocator::~GeneralAllocator() noexcept = default;

    void* GeneralAllocator::Allocate(std::size_t size, std::size_t alignment)
    {
#if defined(GARAGE_DEBUG)
        Detail::RecordAllocationRequest(this, MEMORY_POOL_NAME, size, alignment);
#endif
        ASSERT(std::has_single_bit(alignment));
        const std::size_t allocationSize = std::max(size, std::size_t{1});
        void*             pointer =
                ::operator new(allocationSize, static_cast<std::align_val_t>(std::max(alignment, DEFAULT_ALIGNMENT)));
#if defined(TRACY_ENABLE)
        TracyAllocNS(pointer, allocationSize, Detail::ALLOCATION_STACK_DEPTH, MEMORY_POOL_NAME);
#endif
        return pointer;
    }

    void GeneralAllocator::Deallocate(void* pointer, [[maybe_unused]] std::size_t size, std::size_t alignment) noexcept
    {
        ASSERT(std::has_single_bit(alignment));
#if defined(TRACY_ENABLE)
        if (pointer != nullptr)
        {
            TracyFreeN(pointer, MEMORY_POOL_NAME);
        }
#endif
        ::operator delete(pointer, static_cast<std::align_val_t>(std::max(alignment, DEFAULT_ALIGNMENT)));
    }

    IAllocatable* GetDefaultAllocator() noexcept
    {
        static GeneralAllocator allocator;
        return &allocator;
    }
} // namespace Garage::Memory
