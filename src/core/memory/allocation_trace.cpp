#include "garage/core/memory/allocation_trace.hpp"

#include <array>
#include <mutex>
#include <utility>

#include "garage/utilities/logger.hpp"

namespace Garage::Memory
{
#if defined(GARAGE_DEBUG)
    namespace
    {
        constexpr std::size_t TRACE_CAPACITY = 512;

        struct AllocationRequest
        {
            const IAllocatable*  allocator = nullptr;
            const char*          name      = nullptr;
            std::size_t          size      = 0;
            std::size_t          alignment = 0;
            std::source_location location{};
        };

        constinit std::mutex                                    traceMutex;
        constinit std::array<AllocationRequest, TRACE_CAPACITY> requests{};
        constinit std::size_t                                   nextIndex        = 0;
        constinit std::size_t                                   requestCount     = 0;
        constinit std::size_t                                   overwrittenCount = 0;
        constinit thread_local std::source_location             pendingLocation{};
    } // namespace
#endif

    AllocationTraceScope::AllocationTraceScope([[maybe_unused]] std::source_location location) noexcept
    {
#if defined(GARAGE_DEBUG)
        previousLocation = std::exchange(pendingLocation, location);
#endif
    }

    AllocationTraceScope::~AllocationTraceScope() noexcept
    {
#if defined(GARAGE_DEBUG)
        pendingLocation = previousLocation;
#endif
    }

    void* AllocateAt(IAllocatable* allocator, std::size_t size, std::size_t alignment, std::source_location location)
    {
        ASSERT(allocator != nullptr);
        const AllocationTraceScope scope(location);
        return allocator->Allocate(size, alignment);
    }

    void DumpAllocationTrace()
    {
#if defined(GARAGE_DEBUG)
        std::array<AllocationRequest, TRACE_CAPACITY> snapshot{};
        std::size_t                                   count       = 0;
        std::size_t                                   overwritten = 0;
        {
            const std::lock_guard lock(traceMutex);
            count                        = requestCount;
            overwritten                  = overwrittenCount;
            const std::size_t firstIndex = (nextIndex + TRACE_CAPACITY - count) % TRACE_CAPACITY;
            for (std::size_t index = 0; index < count; ++index)
            {
                snapshot[index] = requests[(firstIndex + index) % TRACE_CAPACITY];
            }
        }

        if (overwritten != 0)
        {
            LOG_TRACE("[Memory] {} older requests overwritten", overwritten);
        }
        for (std::size_t index = 0; index < count; ++index)
        {
            const auto& request = snapshot[index];
            LOG_TRACE("[Memory] {}:{} | {} bytes | align={} | {} ({})",
                      request.location.line() == 0 ? "<unknown>" : request.location.file_name(),
                      request.location.line(),
                      request.size,
                      request.alignment,
                      request.name,
                      static_cast<const void*>(request.allocator));
        }
#endif
    }

    namespace Detail
    {
        void RecordAllocationRequest([[maybe_unused]] const IAllocatable* allocator,
                                     [[maybe_unused]] const char*         allocatorName,
                                     [[maybe_unused]] std::size_t         size,
                                     [[maybe_unused]] std::size_t         alignment) noexcept
        {
#if defined(GARAGE_DEBUG)
            const std::lock_guard lock(traceMutex);
            requests[nextIndex] = {allocator, allocatorName, size, alignment, std::exchange(pendingLocation, {})};
            nextIndex           = (nextIndex + 1) % TRACE_CAPACITY;
            if (requestCount < TRACE_CAPACITY)
            {
                ++requestCount;
            }
            else
            {
                ++overwrittenCount;
            }
#endif
        }
    } // namespace Detail
} // namespace Garage::Memory
