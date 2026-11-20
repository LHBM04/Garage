#ifndef GARAGE_CORE_MEMORY_GENERAL_ALLOCATOR_HPP
#define GARAGE_CORE_MEMORY_GENERAL_ALLOCATOR_HPP

#include "allocator.hpp"

namespace Garage::Memory
{
    class GeneralAllocator final : public IAllocatable
    {
    public:
        GARAGE_API GeneralAllocator() noexcept;
        GARAGE_API ~GeneralAllocator() noexcept override;

        [[nodiscard]] GARAGE_API void* Allocate(std::size_t size, std::size_t alignment = DEFAULT_ALIGNMENT) override;
        GARAGE_API void                Deallocate(void*       pointer,
                                                  std::size_t size,
                                                  std::size_t alignment = DEFAULT_ALIGNMENT) noexcept override;
    };
} // namespace Garage::Memory

#endif // GARAGE_CORE_MEMORY_GENERAL_ALLOCATOR_HPP
