#include "garage/core/objectable.hpp"

#include "garage/core/memory/allocator.hpp"

namespace Garage
{
    void* IObjectable::operator new(std::size_t size) noexcept
    {
        return Memory::GetDefaultAllocator()->Allocate(size);
    }

    void* IObjectable::operator new(std::size_t size, std::align_val_t alignment) noexcept
    {
        return Memory::GetDefaultAllocator()->Allocate(size, static_cast<std::size_t>(alignment));
    }

    void IObjectable::operator delete(void* pointer, std::size_t size) noexcept
    {
        Memory::GetDefaultAllocator()->Deallocate(pointer, size);
    }

    void IObjectable::operator delete(void* pointer, std::size_t size, std::align_val_t alignment) noexcept
    {
        Memory::GetDefaultAllocator()->Deallocate(pointer, size, static_cast<std::size_t>(alignment));
    }
} // namespace Garage
