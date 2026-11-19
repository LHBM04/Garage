#include "garage/core/memory/allocator.hpp"

#include <atomic>
#include <bit>
#include <cstddef>
#include <cstring>

#include <SDL3/SDL_stdinc.h>

#include "garage/core/common/defines.hpp"

namespace Garage::Memory
{
    namespace
    {
        std::atomic<std::size_t> activeAllocationCount{0};
    } // namespace

    void* Allocate(std::size_t size, std::size_t alignment) noexcept
    {
        if (size == 0)
        {
            return nullptr;
        }

        ASSERT(std::has_single_bit(alignment));

        void* pointer = SDL_aligned_alloc(alignment, size);
        if (pointer != nullptr)
        {
            activeAllocationCount.fetch_add(1, std::memory_order_relaxed);
        }

        return pointer;
    }

    void* AllocateZeroed(std::size_t size, std::size_t alignment) noexcept
    {
        void* pointer = Allocate(size, alignment);
        if (pointer != nullptr)
        {
            std::memset(pointer, 0, size);
        }

        return pointer;
    }

    void* Reallocate(void* pointer, std::size_t oldSize, std::size_t newSize, std::size_t alignment) noexcept
    {
        if (pointer == nullptr)
        {
            return Allocate(newSize, alignment);
        }

        if (newSize == 0)
        {
            Deallocate(pointer);
            return nullptr;
        }

        void* newPointer = Allocate(newSize, alignment);
        if (newPointer == nullptr)
        {
            return nullptr;
        }

        const std::size_t COPY_SIZE = (oldSize < newSize) ? oldSize : newSize;
        if (COPY_SIZE > 0)
        {
            std::memcpy(newPointer, pointer, COPY_SIZE);
        }

        Deallocate(pointer);
        return newPointer;
    }

    void Deallocate(void* pointer) noexcept
    {
        if (pointer == nullptr)
        {
            return;
        }

        activeAllocationCount.fetch_sub(1, std::memory_order_relaxed);
        SDL_aligned_free(pointer);
    }

    std::size_t GetActiveAllocationCount() noexcept
    {
        return activeAllocationCount.load(std::memory_order_relaxed);
    }
} // namespace Garage::Memory
