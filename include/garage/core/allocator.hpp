#ifndef GARAGE_CORE_ALLOCATOR_HPP
#define GARAGE_CORE_ALLOCATOR_HPP

#include <concepts>
#include <cstddef>
#include <limits>
#include <memory>
#include <memory_resource>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#include "defines.hpp"

namespace Garage::Allocator
{
    inline constexpr std::size_t DEFAULT_ALIGNMENT = alignof(std::max_align_t);

    template <class T>
    struct Deleter final
    {
        constexpr Deleter() noexcept = default;

        template <class U>
            requires std::convertible_to<U*, T*>
        // NOLINTNEXTLINE(hicpp-explicit-conversions)
        Deleter(const Deleter<U>&) noexcept;

        void operator()(T* pointer) const noexcept;
    };

    template <class T>
    using UniquePtr = std::unique_ptr<T, Deleter<T>>;

    [[nodiscard]] GARAGE_API void* Allocate(std::size_t size, std::size_t alignment = DEFAULT_ALIGNMENT) noexcept;
    [[nodiscard]] GARAGE_API void* AllocateZeroed(std::size_t size, std::size_t alignment = DEFAULT_ALIGNMENT) noexcept;
    [[nodiscard]] GARAGE_API void* Reallocate(void*       pointer,
                                              std::size_t oldSize,
                                              std::size_t newSize,
                                              std::size_t alignment = DEFAULT_ALIGNMENT) noexcept;
    GARAGE_API void                Deallocate(void* pointer) noexcept;

    [[nodiscard]] GARAGE_API std::size_t GetActiveAllocationCount() noexcept;
    [[nodiscard]] GARAGE_API std::pmr::memory_resource* GetMemoryResource() noexcept;
    [[nodiscard]] inline std::pmr::string               MakeString(std::string_view text);

    template <class T>
        requires(!std::is_void_v<T> && !std::is_reference_v<T> && !std::is_array_v<T> && !std::is_function_v<T>)
    [[nodiscard]] T* Allocate(std::size_t count = 1, std::size_t alignment = alignof(T)) noexcept;

    template <class T>
        requires(!std::is_void_v<T> && !std::is_reference_v<T> && !std::is_array_v<T> && !std::is_function_v<T>)
    [[nodiscard]] T* AllocateZeroed(std::size_t count = 1, std::size_t alignment = alignof(T)) noexcept;

    template <class T>
        requires(!std::is_void_v<T> && !std::is_reference_v<T> && !std::is_array_v<T> && !std::is_function_v<T>)
    [[nodiscard]] T* Reallocate(T*          pointer,
                                std::size_t oldCount,
                                std::size_t newCount,
                                std::size_t alignment = alignof(T)) noexcept;

    template <class T, class... TArgs>
        requires(!std::is_array_v<T> && std::is_constructible_v<T, TArgs...>)
    [[nodiscard]] T* New(TArgs&&... args) noexcept(std::is_nothrow_constructible_v<T, TArgs...>);

    template <class T>
        requires(!std::is_array_v<T>)
    void Delete(T* pointer) noexcept;

    template <class T, class... TArgs>
        requires(!std::is_array_v<T> && std::is_constructible_v<T, TArgs...>)
    [[nodiscard]] UniquePtr<T> MakeUnique(TArgs&&... args) noexcept(std::is_nothrow_constructible_v<T, TArgs...>);

    template <class T, class... TArgs>
        requires(!std::is_array_v<T> && std::is_constructible_v<T, TArgs...>)
    [[nodiscard]] std::shared_ptr<T> MakeShared(TArgs&&... args);

    inline std::pmr::string MakeString(std::string_view text)
    {
        return std::pmr::string(text, GetMemoryResource());
    }

    template <class T>
    template <class U>
        requires std::convertible_to<U*, T*>
    Deleter<T>::Deleter([[maybe_unused]] const Deleter<U>& other) noexcept
    {
    }

    template <class T>
    void Deleter<T>::operator()(T* pointer) const noexcept
    {
        Delete(pointer);
    }

    template <class T>
        requires(!std::is_void_v<T> && !std::is_reference_v<T> && !std::is_array_v<T> && !std::is_function_v<T>)
    T* Allocate(std::size_t count, std::size_t alignment) noexcept
    {
        if (count == 0 || count > std::numeric_limits<std::size_t>::max() / sizeof(T))
        {
            return nullptr;
        }

        return static_cast<T*>(Allocate(sizeof(T) * count, alignment));
    }

    template <class T>
        requires(!std::is_void_v<T> && !std::is_reference_v<T> && !std::is_array_v<T> && !std::is_function_v<T>)
    T* AllocateZeroed(std::size_t count, std::size_t alignment) noexcept
    {
        if (count == 0 || count > std::numeric_limits<std::size_t>::max() / sizeof(T))
        {
            return nullptr;
        }

        return static_cast<T*>(AllocateZeroed(sizeof(T) * count, alignment));
    }

    template <class T>
        requires(!std::is_void_v<T> && !std::is_reference_v<T> && !std::is_array_v<T> && !std::is_function_v<T>)
    T* Reallocate(T* pointer, std::size_t oldCount, std::size_t newCount, std::size_t alignment) noexcept
    {
        constexpr std::size_t MAX_COUNT = std::numeric_limits<std::size_t>::max() / sizeof(T);
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

    template <class T, class... TArgs>
        requires(!std::is_array_v<T> && std::is_constructible_v<T, TArgs...>)
    std::shared_ptr<T> MakeShared(TArgs&&... args)
    {
        return std::allocate_shared<T>(std::pmr::polymorphic_allocator<T>(GetMemoryResource()),
                                       std::forward<TArgs>(args)...);
    }
} // namespace Garage::Allocator

#endif // GARAGE_CORE_ALLOCATOR_HPP
