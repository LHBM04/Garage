#include "garage/core/memory/general_allocator.hpp"

#include <algorithm>
#include <bit>
#include <new>

namespace Garage::Memory
{
    GeneralAllocator::GeneralAllocator() noexcept  = default;
    GeneralAllocator::~GeneralAllocator() noexcept = default;

    void* GeneralAllocator::Allocate(std::size_t size, std::size_t alignment)
    {
        ASSERT(std::has_single_bit(alignment));
        return ::operator new(std::max(size, std::size_t{1}),
                              static_cast<std::align_val_t>(std::max(alignment, DEFAULT_ALIGNMENT)));
    }

    void GeneralAllocator::Deallocate(void* pointer, [[maybe_unused]] std::size_t size, std::size_t alignment) noexcept
    {
        ASSERT(std::has_single_bit(alignment));
        ::operator delete(pointer, static_cast<std::align_val_t>(std::max(alignment, DEFAULT_ALIGNMENT)));
    }

    IAllocatable* GetDefaultAllocator() noexcept
    {
        static GeneralAllocator allocator;
        return &allocator;
    }
} // namespace Garage::Memory
