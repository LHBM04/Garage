#ifndef GARAGE_PLATFORM_DISPLAY_IMPL_HPP
#define GARAGE_PLATFORM_DISPLAY_IMPL_HPP

#include <string_view>

#include "garage/core/types.hpp"
#include "garage/math/vector2d.hpp"
#include "garage/platform/display.hpp"

struct SDL_Window;

namespace Garage
{
    class DisplayImpl final : public IDisplay
    {
    public:
        explicit DisplayImpl(SDL_Window* window = nullptr) noexcept;
        ~DisplayImpl() noexcept override;

        DisplayImpl(const DisplayImpl&)            = delete;
        DisplayImpl& operator=(const DisplayImpl&) = delete;

        DisplayImpl(DisplayImpl&&)            = delete;
        DisplayImpl& operator=(DisplayImpl&&) = delete;

        [[nodiscard]] SDL_Window* GetSDLWindow() const noexcept;
        [[nodiscard]] UInt32      GetSDLWindowID() const noexcept;

        [[nodiscard]] ID GetID() const noexcept override;

        [[nodiscard]] std::string_view GetCaption() const noexcept override;
        void                           SetCaption(std::string_view value) noexcept override;

        [[nodiscard]] Vector2DInt GetPosition() const noexcept override;
        void                      SetPosition(const Vector2DInt& position) noexcept override;

        [[nodiscard]] Vector2DInt GetSize() const noexcept override;
        void                      SetSize(const Vector2DInt& size) noexcept override;

        [[nodiscard]] Vector2DInt GetClientSize() const noexcept override;
        void                      SetClientSize(const Vector2DInt& size) noexcept override;

        [[nodiscard]] Vector2DInt GetPixelSize() const noexcept override;

        [[nodiscard]] bool ShouldClose() const noexcept override;
        void               SetShouldClose(bool value) noexcept override;

        [[nodiscard]] bool IsVisible() const noexcept override;
        void               Show() noexcept override;
        void               Hide() noexcept override;

        [[nodiscard]] bool   IsFocused() const noexcept override;
        [[nodiscard]] UInt64 GetFlags() const noexcept override;

    private:
        inline static ID nextId = static_cast<ID>(0);

        SDL_Window* window      = nullptr;
        bool        shouldClose = false;
        ID          id          = static_cast<ID>(0);
    };
} // namespace Garage

#endif // GARAGE_PLATFORM_DISPLAY_IMPL_HPP
