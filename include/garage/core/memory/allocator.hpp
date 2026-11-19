#ifndef GARAGE_CORE_MEMORY_ALLOCATOR_HPP
#define GARAGE_CORE_MEMORY_ALLOCATOR_HPP

#include <cstddef>
#include <limits>
#include <memory>
#include <type_traits>
#include <utility>

#include "../common/defines.hpp"

namespace Garage
{
    namespace Memory
    {
        inline constexpr std::size_t DEFAULT_ALIGNMENT = alignof(std::max_align_t);

        [[nodiscard]] GARAGE_API void* Allocate(std::size_t size, std::size_t alignment = DEFAULT_ALIGNMENT) noexcept;
        [[nodiscard]] GARAGE_API void* AllocateZeroed(std::size_t size,
                                                      std::size_t alignment = DEFAULT_ALIGNMENT) noexcept;
        [[nodiscard]] GARAGE_API void* Reallocate(void*       pointer,
                                                  std::size_t oldSize,
                                                  std::size_t newSize,
                                                  std::size_t alignment = DEFAULT_ALIGNMENT) noexcept;
        GARAGE_API void                Deallocate(void* pointer) noexcept;

        [[nodiscard]] GARAGE_API std::size_t GetActiveAllocationCount() noexcept;

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
    } // namespace Memory
} // namespace Garage

#endif // GARAGE_CORE_MEMORY_ALLOCATOR_HPP
