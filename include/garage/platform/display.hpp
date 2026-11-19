#ifndef GARAGE_PLATFORM_DISPLAY_HPP
#define GARAGE_PLATFORM_DISPLAY_HPP

#include <string_view>

#include "../core/common.hpp"

#include "../math/vector2d.hpp"

namespace Garage
{
    class GARAGE_API IDisplay
    {
    public:
        enum class EId : UInt32;

        IDisplay() noexcept          = default;
        virtual ~IDisplay() noexcept = default;

        IDisplay(const IDisplay&)            = delete;
        IDisplay& operator=(const IDisplay&) = delete;

        IDisplay(IDisplay&&)            = delete;
        IDisplay& operator=(IDisplay&&) = delete;

        [[nodiscard]] virtual EId GetID() const noexcept = 0;

        [[nodiscard]] virtual std::string_view GetCaption() const noexcept                 = 0;
        virtual void                           SetCaption(std::string_view value) noexcept = 0;

        [[nodiscard]] virtual Vector2DInt GetPosition() const noexcept                      = 0;
        virtual void                      SetPosition(const Vector2DInt& position) noexcept = 0;

        [[nodiscard]] virtual Vector2DInt GetSize() const noexcept                  = 0;
        virtual void                      SetSize(const Vector2DInt& size) noexcept = 0;

        [[nodiscard]] virtual Vector2DInt GetClientSize() const noexcept                  = 0;
        virtual void                      SetClientSize(const Vector2DInt& size) noexcept = 0;

        [[nodiscard]] virtual Vector2DInt GetPixelSize() const noexcept = 0;

        [[nodiscard]] virtual bool ShouldClose() const noexcept        = 0;
        virtual void               SetShouldClose(bool value) noexcept = 0;

        [[nodiscard]] virtual bool IsVisible() const noexcept = 0;
        virtual void               Show() noexcept            = 0;
        virtual void               Hide() noexcept            = 0;

        [[nodiscard]] virtual bool   IsFocused() const noexcept = 0;
        [[nodiscard]] virtual UInt64 GetFlags() const noexcept  = 0;
    };
} // namespace Garage

#endif // GARAGE_PLATFORM_DISPLAY_HPP
