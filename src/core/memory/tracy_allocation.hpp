#ifndef GARAGE_CORE_MEMORY_TRACY_ALLOCATION_HPP
#define GARAGE_CORE_MEMORY_TRACY_ALLOCATION_HPP

#if defined(TRACY_ENABLE)

#    include <cstddef>

namespace Garage::Memory::Detail
{
    inline constexpr int ALLOCATION_STACK_DEPTH = 16;

    // Named Tracy pools are keyed by the address of their name. Use one stable name per allocator type.
    // Track ownership separately so resetting an arena only clears allocations from that instance.
    void ProfileSuballocation(const void* owner, const void* pointer, std::size_t size, const char* name);
    void ProfileSuballocationFree(const void* owner, const void* pointer, const char* name) noexcept;
    void ProfileSuballocationReset(const void* owner, const char* name) noexcept;
} // namespace Garage::Memory::Detail

#endif

#endif // GARAGE_CORE_MEMORY_TRACY_ALLOCATION_HPP
