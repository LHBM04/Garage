#include "garage/core/allocator.hpp"

#include <atomic>
#include <bit>
#include <cstring>
#include <memory_resource>

#include <SDL3/SDL_stdinc.h>

#include "garage/core/defines.hpp"
#include "garage/core/types.hpp"

namespace Garage::Allocator
{
    namespace
    {
        std::atomic<USize> activeAllocationCount{0};

        class AllocatorMemoryResource final : public std::pmr::memory_resource
        {
        public:
            AllocatorMemoryResource() noexcept           = default;
            ~AllocatorMemoryResource() noexcept override = default;

            AllocatorMemoryResource(const AllocatorMemoryResource&)            = delete;
            AllocatorMemoryResource& operator=(const AllocatorMemoryResource&) = delete;

            AllocatorMemoryResource(AllocatorMemoryResource&&)            = delete;
            AllocatorMemoryResource& operator=(AllocatorMemoryResource&&) = delete;

        private:
            [[nodiscard]] void* do_allocate(USize bytes, USize alignment) override;
            void                do_deallocate(void* pointer, USize bytes, USize alignment) override;
            [[nodiscard]] bool  do_is_equal(const std::pmr::memory_resource& other) const noexcept override;
        };

        void* AllocatorMemoryResource::do_allocate(USize bytes, USize alignment)
        {
            const USize VALID_ALIGNMENT = std::has_single_bit(alignment) ? alignment : DEFAULT_ALIGNMENT;
            const USize ALLOC_SIZE      = (bytes == 0) ? 1 : bytes;
            void*       memory          = Allocate(ALLOC_SIZE, VALID_ALIGNMENT);
            ASSERT(memory != nullptr);
            return memory;
        }

        void AllocatorMemoryResource::do_deallocate(void*                  pointer,
                                                    [[maybe_unused]] USize bytes,
                                                    [[maybe_unused]] USize alignment)
        {
            Deallocate(pointer);
        }

        bool AllocatorMemoryResource::do_is_equal(const std::pmr::memory_resource& other) const noexcept
        {
            return this == &other;
        }
    } // namespace

    void* Allocate(USize size, USize alignment) noexcept
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

    void* AllocateZeroed(USize size, USize alignment) noexcept
    {
        void* pointer = Allocate(size, alignment);
        if (pointer != nullptr)
        {
            std::memset(pointer, 0, size);
        }

        return pointer;
    }

    void* Reallocate(void* pointer, USize oldSize, USize newSize, USize alignment) noexcept
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

        const USize COPY_SIZE = (oldSize < newSize) ? oldSize : newSize;
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

    USize GetActiveAllocationCount() noexcept
    {
        return activeAllocationCount.load(std::memory_order_relaxed);
    }

    std::pmr::memory_resource* GetMemoryResource() noexcept
    {
        static AllocatorMemoryResource resource;
        return &resource;
    }
} // namespace Garage::Allocator
