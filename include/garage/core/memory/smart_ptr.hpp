#ifndef GARAGE_CORE_MEMORY_SMART_PTR_HPP
#define GARAGE_CORE_MEMORY_SMART_PTR_HPP

#include <memory>
#include <source_location>
#include <type_traits>
#include <utility>

#include "allocation_trace.hpp"
#include "stl_allocator.hpp"

namespace Garage
{
    namespace Memory
    {
        template <class T>
        class Deleter
        {
        public:
            Deleter() noexcept = default;
            Deleter(IAllocatable* allocator, T* object) noexcept;

            template <class U> requires std::is_convertible_v<U*, T*>
            Deleter(const Deleter<U>& other) noexcept;

            void operator()(T* pointer) const noexcept;

        private:
            template <class U>
            friend class Deleter;

            using DestroyFunction = void (*)(void*) noexcept;

            IAllocatable*     allocator = nullptr;
            void*           object    = nullptr;
            std::size_t     size      = 0;
            std::size_t     alignment = 0;
            DestroyFunction destroy   = nullptr;
        };

        template <class T>
        Deleter<T>::Deleter(IAllocatable* allocator, T* object) noexcept
            : allocator(allocator)
            , object(object)
            , size(sizeof(T))
            , alignment(alignof(T))
            , destroy(
                      [](void* address) noexcept
                      {
                          std::destroy_at(static_cast<T*>(address));
                      })
        {
        }

        template <class T>
        template <class U>
            requires std::is_convertible_v<U*, T*>
        Deleter<T>::Deleter(const Deleter<U>& other) noexcept
            : allocator(other.allocator)
            , object(other.object)
            , size(other.size)
            , alignment(other.alignment)
            , destroy(other.destroy)
        {
        }

        template <class T>
        void Deleter<T>::operator()(T* pointer) const noexcept
        {
            if (pointer != nullptr)
            {
                ASSERT(destroy != nullptr);
                destroy(object);
                allocator->Deallocate(object, size, alignment);
            }
        }

        template <class... TArgs>
        inline constexpr bool FIRST_ARG_IS_ALLOCATOR = false;

        template <class TFirst, class... TRest>
        inline constexpr bool FIRST_ARG_IS_ALLOCATOR<TFirst, TRest...> = std::is_convertible_v<TFirst, IAllocatable*>;
    } // namespace Memory

    // Pair with MakeUnique. Replacing the raw pointer independently of its deleter is unsupported.
    template <class T>
    using UniquePtr = std::unique_ptr<T, Memory::Deleter<T>>;

    template <class T>
    using SharedPtr = std::shared_ptr<T>;

    template <class T>
    using WeakPtr = std::weak_ptr<T>;

    template <class T, class... TArgs>
        requires(!std::is_array_v<T> && std::is_constructible_v<T, TArgs...>)
    [[nodiscard]] UniquePtr<T> MakeUnique(Memory::IAllocatable* allocator, TArgs&&... args)
    {
        ASSERT(allocator != nullptr);
        void* storage = allocator->Allocate(sizeof(T), alignof(T));
        ASSERT(storage != nullptr);
        const auto deallocate = [allocator](void* pointer) noexcept
        {
            allocator->Deallocate(pointer, sizeof(T), alignof(T));
        };
        std::unique_ptr<void, decltype(deallocate)> guard(storage, deallocate);
        T* object = std::construct_at(static_cast<T*>(storage), std::forward<TArgs>(args)...);
        static_cast<void>(guard.release());
        return UniquePtr<T>(object, Memory::Deleter<T>(allocator, object));
    }

    template <class T, class... TArgs>
        requires(!std::is_array_v<T> && !Memory::FIRST_ARG_IS_ALLOCATOR<TArgs...> &&
                 std::is_constructible_v<T, TArgs...>)
    [[nodiscard]] UniquePtr<T> MakeUnique(TArgs&&... args)
    {
        return MakeUnique<T>(Memory::GetDefaultAllocator(), std::forward<TArgs>(args)...);
    }

    template <class T, class... TArgs>
        requires(!std::is_array_v<T> && std::is_constructible_v<T, TArgs...>)
    [[nodiscard]] SharedPtr<T> MakeShared(Memory::IAllocatable* allocator, TArgs&&... args)
    {
        return std::allocate_shared<T>(Memory::StlAllocator<T>(allocator), std::forward<TArgs>(args)...);
    }

    template <class T, class... TArgs>
        requires(!std::is_array_v<T> && !Memory::FIRST_ARG_IS_ALLOCATOR<TArgs...> &&
                 std::is_constructible_v<T, TArgs...>)
    [[nodiscard]] SharedPtr<T> MakeShared(TArgs&&... args)
    {
        return MakeShared<T>(Memory::GetDefaultAllocator(), std::forward<TArgs>(args)...);
    }
    template <class T, class... TArgs>
        requires(!std::is_array_v<T> && std::is_constructible_v<T, TArgs...>)
    [[nodiscard]] UniquePtr<T> MakeUniqueAt(std::source_location location, Memory::IAllocatable* allocator,
                                            TArgs&&... args)
    {
        const Memory::AllocationTraceScope scope(location);
        return MakeUnique<T>(allocator, std::forward<TArgs>(args)...);
    }

    template <class T, class... TArgs>
        requires(!std::is_array_v<T> && !Memory::FIRST_ARG_IS_ALLOCATOR<TArgs...> &&
                 std::is_constructible_v<T, TArgs...>)
    [[nodiscard]] UniquePtr<T> MakeUniqueAt(std::source_location location, TArgs&&... args)
    {
        return MakeUniqueAt<T>(location, Memory::GetDefaultAllocator(), std::forward<TArgs>(args)...);
    }

    template <class T, class... TArgs>
        requires(!std::is_array_v<T> && std::is_constructible_v<T, TArgs...>)
    [[nodiscard]] SharedPtr<T> MakeSharedAt(std::source_location location, Memory::IAllocatable* allocator,
                                            TArgs&&... args)
    {
        const Memory::AllocationTraceScope scope(location);
        return MakeShared<T>(allocator, std::forward<TArgs>(args)...);
    }

    template <class T, class... TArgs>
        requires(!std::is_array_v<T> && !Memory::FIRST_ARG_IS_ALLOCATOR<TArgs...> &&
                 std::is_constructible_v<T, TArgs...>)
    [[nodiscard]] SharedPtr<T> MakeSharedAt(std::source_location location, TArgs&&... args)
    {
        return MakeSharedAt<T>(location, Memory::GetDefaultAllocator(), std::forward<TArgs>(args)...);
    }
} // namespace Garage

#endif // GARAGE_CORE_MEMORY_SMART_PTR_HPP
