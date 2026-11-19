#ifndef GARAGE_CORE_MEMORY_SMART_PTR_HPP
#define GARAGE_CORE_MEMORY_SMART_PTR_HPP

#include <concepts>
#include <memory>
#include <memory_resource>
#include <type_traits>
#include <utility>

#include "allocator.hpp"
#include "memory_resource.hpp"

namespace Garage
{
    namespace Memory
    {
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
    } // namespace Memory

    template <class T>
    using UniquePtr = std::unique_ptr<T, Memory::Deleter<T>>;

    template <class T>
    using SharedPtr = std::shared_ptr<T>;

    template <class T>
    using WeakPtr = std::weak_ptr<T>;

    template <class T, class... TArgs>
        requires(!std::is_array_v<T> && std::is_constructible_v<T, TArgs...>)
    [[nodiscard]] UniquePtr<T> MakeUnique(TArgs&&... args) noexcept(std::is_nothrow_constructible_v<T, TArgs...>);

    template <class T, class... TArgs>
        requires(!std::is_array_v<T> && std::is_constructible_v<T, TArgs...>)
    [[nodiscard]] SharedPtr<T> MakeShared(TArgs&&... args);

    template <class T, class... TArgs>
        requires(!std::is_array_v<T> && std::is_constructible_v<T, TArgs...>)
    UniquePtr<T> MakeUnique(TArgs&&... args) noexcept(std::is_nothrow_constructible_v<T, TArgs...>)
    {
        return UniquePtr<T>(Memory::New<T>(std::forward<TArgs>(args)...));
    }

    template <class T, class... TArgs>
        requires(!std::is_array_v<T> && std::is_constructible_v<T, TArgs...>)
    SharedPtr<T> MakeShared(TArgs&&... args)
    {
        return std::allocate_shared<T>(std::pmr::polymorphic_allocator<T>(Memory::GetMemoryResource()),
                                       std::forward<TArgs>(args)...);
    }
} // namespace Garage

#endif // GARAGE_CORE_MEMORY_SMART_PTR_HPP
