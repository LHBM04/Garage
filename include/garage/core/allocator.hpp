#ifndef GARAGE_CORE_ALLOCATOR_HPP
#define GARAGE_CORE_ALLOCATOR_HPP

#include <cstddef>
#include <limits>
#include <memory>
#include <memory_resource>
#include <type_traits>
#include <utility>

#include "defines.hpp"
#include "types.hpp"

namespace Garage::Allocator
{
    inline constexpr USize DEFAULT_ALIGNMENT = alignof(std::max_align_t);

    template <class T>
    struct Deleter final
    {
        void operator()(T* pointer) const noexcept;
    };

    template <class T>
    using UniquePtr = std::unique_ptr<T, Deleter<T>>;

    [[nodiscard]] GARAGE_API void* Allocate(USize size, USize alignment = DEFAULT_ALIGNMENT) noexcept;
    [[nodiscard]] GARAGE_API void* AllocateZeroed(USize size, USize alignment = DEFAULT_ALIGNMENT) noexcept;
    [[nodiscard]] GARAGE_API void*
            Reallocate(void* pointer, USize oldSize, USize newSize, USize alignment = DEFAULT_ALIGNMENT) noexcept;
    GARAGE_API void Deallocate(void* pointer) noexcept;

    [[nodiscard]] GARAGE_API USize GetActiveAllocationCount() noexcept;
    [[nodiscard]] GARAGE_API std::pmr::memory_resource* GetMemoryResource() noexcept;

    template <class T>
        requires(!std::is_void_v<T> && !std::is_reference_v<T> && !std::is_array_v<T> && !std::is_function_v<T>)
    [[nodiscard]] T* Allocate(USize count = 1, USize alignment = alignof(T)) noexcept;

    template <class T>
        requires(!std::is_void_v<T> && !std::is_reference_v<T> && !std::is_array_v<T> && !std::is_function_v<T>)
    [[nodiscard]] T* AllocateZeroed(USize count = 1, USize alignment = alignof(T)) noexcept;

    template <class T>
        requires(!std::is_void_v<T> && !std::is_reference_v<T> && !std::is_array_v<T> && !std::is_function_v<T>)
    [[nodiscard]] T* Reallocate(T* pointer, USize oldCount, USize newCount, USize alignment = alignof(T)) noexcept;

    template <class T, class... TArgs>
        requires(!std::is_array_v<T> && std::is_constructible_v<T, TArgs...>)
    [[nodiscard]] T* New(TArgs&&... args) noexcept(std::is_nothrow_constructible_v<T, TArgs...>);

    template <class T>
        requires(!std::is_array_v<T>)
    void Delete(T* pointer) noexcept;

    template <class T, class... TArgs>
        requires(!std::is_array_v<T> && std::is_constructible_v<T, TArgs...>)
    [[nodiscard]] UniquePtr<T> MakeUnique(TArgs&&... args) noexcept(std::is_nothrow_constructible_v<T, TArgs...>);

    template <class T>
    void Deleter<T>::operator()(T* pointer) const noexcept
    {
        Delete(pointer);
    }

    template <class T>
        requires(!std::is_void_v<T> && !std::is_reference_v<T> && !std::is_array_v<T> && !std::is_function_v<T>)
    T* Allocate(USize count, USize alignment) noexcept
    {
        if (count == 0 || count > std::numeric_limits<USize>::max() / sizeof(T))
        {
            return nullptr;
        }

        return static_cast<T*>(Allocate(sizeof(T) * count, alignment));
    }

    template <class T>
        requires(!std::is_void_v<T> && !std::is_reference_v<T> && !std::is_array_v<T> && !std::is_function_v<T>)
    T* AllocateZeroed(USize count, USize alignment) noexcept
    {
        if (count == 0 || count > std::numeric_limits<USize>::max() / sizeof(T))
        {
            return nullptr;
        }

        return static_cast<T*>(AllocateZeroed(sizeof(T) * count, alignment));
    }

    template <class T>
        requires(!std::is_void_v<T> && !std::is_reference_v<T> && !std::is_array_v<T> && !std::is_function_v<T>)
    T* Reallocate(T* pointer, USize oldCount, USize newCount, USize alignment) noexcept
    {
        constexpr USize MAX_COUNT = std::numeric_limits<USize>::max() / sizeof(T);
        if (oldCount > MAX_COUNT || newCount > MAX_COUNT)
        {
            return nullptr;
        }

        return static_cast<T*>(
                Reallocate(static_cast<void*>(pointer), sizeof(T) * oldCount, sizeof(T) * newCount, alignment));
    }

    template <class T, class... TArgs>
        requires(!std::is_array_v<T> && std::is_constructible_v<T, TArgs...>)
    T* New(TArgs&&... args) noexcept(std::is_nothrow_constructible_v<T, TArgs...>)
    {
        void* memory = Allocate(sizeof(T), alignof(T));
        if (memory == nullptr)
        {
            return nullptr;
        }

        return std::construct_at(static_cast<T*>(memory), std::forward<TArgs>(args)...);
    }

    template <class T>
        requires(!std::is_array_v<T>)
    void Delete(T* pointer) noexcept
    {
        if (pointer == nullptr)
        {
            return;
        }

        std::destroy_at(pointer);
        Deallocate(pointer);
    }

    template <class T, class... TArgs>
        requires(!std::is_array_v<T> && std::is_constructible_v<T, TArgs...>)
    UniquePtr<T> MakeUnique(TArgs&&... args) noexcept(std::is_nothrow_constructible_v<T, TArgs...>)
    {
        return UniquePtr<T>(New<T>(std::forward<TArgs>(args)...));
    }
} // namespace Garage::Allocator

#endif // GARAGE_CORE_ALLOCATOR_HPP
