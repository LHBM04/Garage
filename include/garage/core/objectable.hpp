#ifndef GARAGE_CORE_OBJECTABLE_HPP
#define GARAGE_CORE_OBJECTABLE_HPP

#include <cstddef>
#include <new>

#include "common/defines.hpp"

namespace Garage
{
    class GARAGE_API IObjectable
    {
    public:
        virtual ~IObjectable() noexcept = default;

        IObjectable(const IObjectable&) noexcept            = delete;
        IObjectable& operator=(const IObjectable&) noexcept = delete;
        IObjectable(IObjectable&&) noexcept                 = delete;
        IObjectable& operator=(IObjectable&&) noexcept      = delete;

        [[nodiscard]] void* operator new(std::size_t size) noexcept;
        [[nodiscard]] void* operator new(std::size_t size, std::align_val_t alignment) noexcept;
        void                operator delete(void* pointer, std::size_t size) noexcept;
        void                operator delete(void* pointer, std::size_t size, std::align_val_t alignment) noexcept;

    protected:
        IObjectable() noexcept = default;
    };
} // namespace Garage

#endif // GARAGE_CORE_OBJECTABLE_HPP
