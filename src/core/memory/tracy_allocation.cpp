#include "tracy_allocation.hpp"

#if defined(TRACY_ENABLE)

#    include <atomic>
#    include <mutex>
#    include <unordered_map>
#    include <unordered_set>

#    include <tracy/Tracy.hpp>

namespace Garage::Memory::Detail
{
    namespace
    {
        struct AllocationRegistry
        {
            std::mutex                                                       mutex;
            std::unordered_map<const void*, std::unordered_set<const void*>> allocations;
        };

        constinit std::atomic<AllocationRegistry*> registryInstance{nullptr};

        AllocationRegistry& GetOrCreateRegistry()
        {
            // Profiling metadata uses the standard heap and remains available during static teardown.
            static AllocationRegistry* const REGISTRY = []
            {
                auto* registry = new AllocationRegistry;
                registryInstance.store(registry, std::memory_order_release);
                return registry;
            }();
            return *REGISTRY;
        }
    } // namespace

    void ProfileSuballocation(const void* owner, const void* pointer, std::size_t size, const char* name)
    {
        auto&                 registry = GetOrCreateRegistry();
        const std::lock_guard lock(registry.mutex);
        registry.allocations[owner].insert(pointer);
        TracyAllocNS(pointer, size, ALLOCATION_STACK_DEPTH, name);
    }

    void ProfileSuballocationFree(const void* owner, const void* pointer, const char* name) noexcept
    {
        auto* registry = registryInstance.load(std::memory_order_acquire);
        if (registry == nullptr)
        {
            return;
        }

        const std::lock_guard lock(registry->mutex);
        const auto            entry = registry->allocations.find(owner);
        if (entry == registry->allocations.end() || entry->second.erase(pointer) == 0)
        {
            return;
        }

        TracyFreeN(pointer, name);
        if (entry->second.empty())
        {
            registry->allocations.erase(entry);
        }
    }

    void ProfileSuballocationReset(const void* owner, const char* name) noexcept
    {
        auto* registry = registryInstance.load(std::memory_order_acquire);
        if (registry == nullptr)
        {
            return;
        }

        const std::lock_guard lock(registry->mutex);
        const auto            entry = registry->allocations.find(owner);
        if (entry == registry->allocations.end())
        {
            return;
        }

        for (const void* pointer : entry->second)
        {
            TracyFreeN(pointer, name);
        }
        registry->allocations.erase(entry);
    }
} // namespace Garage::Memory::Detail

#endif
