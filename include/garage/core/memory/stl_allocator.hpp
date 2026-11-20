#ifndef GARAGE_CORE_MEMORY_STL_ALLOCATOR_HPP
#define GARAGE_CORE_MEMORY_STL_ALLOCATOR_HPP

#include <limits>
#include <type_traits>

#include "allocator.hpp"

namespace Garage::Memory
{
    template <class T>
    class StlAllocator
    {
    public:
        using value_type                             = T;
        using propagate_on_container_move_assignment = std::true_type;
        using is_always_equal                        = std::false_type;

        StlAllocator() noexcept;
        explicit StlAllocator(IAllocatable* allocator) noexcept;

        template <class U>
        StlAllocator(const StlAllocator<U>& other) noexcept;

        [[nodiscard]] IAllocatable* GetAllocator() const noexcept;

        [[nodiscard]] T* allocate(std::size_t count) const;
        void             deallocate(T* pointer, std::size_t count) const noexcept;

        template <class U>
        [[nodiscard]] bool operator==(const StlAllocator<U>& other) const noexcept;

    private:
        IAllocatable* allocator = nullptr;
    };

    template <class T>
    StlAllocator<T>::StlAllocator() noexcept
        : StlAllocator(GetDefaultAllocator())
    {
    }

    template <class T>
    StlAllocator<T>::StlAllocator(IAllocatable* allocator) noexcept
        : allocator(allocator)
    {
        ASSERT(allocator != nullptr);
    }

    template <class T>
    template <class U>
    StlAllocator<T>::StlAllocator(const StlAllocator<U>& other) noexcept
        : allocator(other.GetAllocator())
    {
    }

    template <class T>
    IAllocatable* StlAllocator<T>::GetAllocator() const noexcept
    {
        return allocator;
    }

    template <class T>
    T* StlAllocator<T>::allocate(std::size_t count) const
    {
        if (count == 0)
        {
            return nullptr;
        }

        ASSERT(count <= std::numeric_limits<std::size_t>::max() / sizeof(T));
        void* pointer = allocator->Allocate(count * sizeof(T), alignof(T));
        ASSERT(pointer != nullptr);
        return static_cast<T*>(pointer);
    }

    template <class T>
    void StlAllocator<T>::deallocate(T* pointer, std::size_t count) const noexcept
    {
        if (pointer != nullptr)
        {
            allocator->Deallocate(pointer, count * sizeof(T), alignof(T));
        }
    }

    template <class T>
    template <class U>
    bool StlAllocator<T>::operator==(const StlAllocator<U>& other) const noexcept
    {
        return allocator == other.GetAllocator();
    }
} // namespace Garage::Memory

#endif // GARAGE_CORE_MEMORY_STL_ALLOCATOR_HPP
