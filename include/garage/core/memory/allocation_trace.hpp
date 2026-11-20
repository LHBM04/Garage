#ifndef GARAGE_CORE_MEMORY_ALLOCATION_TRACE_HPP
#define GARAGE_CORE_MEMORY_ALLOCATION_TRACE_HPP

#include <cstddef>
#include <source_location>

#include "allocator.hpp"

namespace Garage::Memory
{
    // Assigns a source location to the next recorded allocation on this thread.
    class GARAGE_API AllocationTraceScope final
    {
    public:
        explicit AllocationTraceScope(std::source_location location = std::source_location::current()) noexcept;
        ~AllocationTraceScope() noexcept;

        AllocationTraceScope(const AllocationTraceScope&) noexcept            = delete;
        AllocationTraceScope& operator=(const AllocationTraceScope&) noexcept = delete;
        AllocationTraceScope(AllocationTraceScope&&) noexcept                 = delete;
        AllocationTraceScope& operator=(AllocationTraceScope&&) noexcept      = delete;

    private:
        std::source_location previousLocation{};
    };

    // Logs a snapshot of recent allocation requests at trace level. Release builds record nothing.
    GARAGE_API void DumpAllocationTrace();

    [[nodiscard]] GARAGE_API void* AllocateAt(IAllocatable*        allocator,
                                              std::size_t          size,
                                              std::size_t          alignment = DEFAULT_ALIGNMENT,
                                              std::source_location location  = std::source_location::current());

    namespace Detail
    {
        // allocatorName must have static storage duration.
        void RecordAllocationRequest(const IAllocatable* allocator,
                                     const char*         allocatorName,
                                     std::size_t         size,
                                     std::size_t         alignment) noexcept;
    } // namespace Detail
} // namespace Garage::Memory

#endif // GARAGE_CORE_MEMORY_ALLOCATION_TRACE_HPP
