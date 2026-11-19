#include "garage/core/memory/memory_resource.hpp"

#include <bit>
#include <cstddef>
#include <memory_resource>

#include "garage/core/defines.hpp"
#include "garage/core/memory/allocator.hpp"

namespace Garage::Memory
{
    namespace
    {
        class DefaultMemoryResource final : public std::pmr::memory_resource
        {
        public:
            DefaultMemoryResource() noexcept           = default;
            ~DefaultMemoryResource() noexcept override = default;

            DefaultMemoryResource(const DefaultMemoryResource&)            = delete;
            DefaultMemoryResource& operator=(const DefaultMemoryResource&) = delete;

            DefaultMemoryResource(DefaultMemoryResource&&)            = delete;
            DefaultMemoryResource& operator=(DefaultMemoryResource&&) = delete;

        private:
            [[nodiscard]] void* do_allocate(std::size_t bytes, std::size_t alignment) override;
            void                do_deallocate(void* pointer, std::size_t bytes, std::size_t alignment) override;
            [[nodiscard]] bool  do_is_equal(const std::pmr::memory_resource& other) const noexcept override;
        };

        void* DefaultMemoryResource::do_allocate(std::size_t bytes, std::size_t alignment)
        {
            const std::size_t VALID_ALIGNMENT = std::has_single_bit(alignment) ? alignment : DEFAULT_ALIGNMENT;
            const std::size_t ALLOC_SIZE      = (bytes == 0) ? 1 : bytes;
            void*             memory          = Allocate(ALLOC_SIZE, VALID_ALIGNMENT);
            ASSERT(memory != nullptr);
            return memory;
        }

        void DefaultMemoryResource::do_deallocate(void*                        pointer,
                                                  [[maybe_unused]] std::size_t bytes,
                                                  [[maybe_unused]] std::size_t alignment)
        {
            Deallocate(pointer);
        }

        bool DefaultMemoryResource::do_is_equal(const std::pmr::memory_resource& other) const noexcept
        {
            return this == &other;
        }
    } // namespace

    std::pmr::memory_resource* GetMemoryResource() noexcept
    {
        static DefaultMemoryResource resource;
        return &resource;
    }
} // namespace Garage::Memory
