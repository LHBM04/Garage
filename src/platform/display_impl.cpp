#include "platform/display_impl.hpp"

#include <SDL3/SDL_video.h>

#include "garage/core/common.hpp"
#include "garage/core/common/containers.hpp"

#include "garage/math/vector2d.hpp"

#include "garage/platform/display.hpp"

namespace Garage
{
    DisplayImpl::DisplayImpl(SDL_Window* window) noexcept
        : window(window)
    {
        nextId = static_cast<EId>(static_cast<UInt32>(nextId) + 1U);
        id     = nextId;
    }

    DisplayImpl::~DisplayImpl() noexcept
    {
        if (window != nullptr)
        {
            SDL_DestroyWindow(window);
            window = nullptr;
        }
    }

    SDL_Window* DisplayImpl::GetSDLWindow() const noexcept
    {
        return window;
    }

    UInt32 DisplayImpl::GetSDLWindowID() const noexcept
    {
        ASSERT(window != nullptr);
        return static_cast<UInt32>(SDL_GetWindowID(window));
    }

    IDisplay::EId DisplayImpl::GetID() const noexcept
    {
        return id;
    }

    StringView DisplayImpl::GetCaption() const noexcept
    {
        ASSERT(window != nullptr);

        const char* title = SDL_GetWindowTitle(window);
        if (title == nullptr)
        {
            return {};
        }

        return StringView{title};
    }

    void DisplayImpl::SetCaption(StringView value) noexcept
    {
        ASSERT(window != nullptr);

        const String TITLE(value);
        SDL_SetWindowTitle(window, TITLE.c_str());
    }

    Vector2DInt DisplayImpl::GetPosition() const noexcept
    {
        ASSERT(window != nullptr);

        Int32 x = 0;
        Int32 y = 0;
        SDL_GetWindowPosition(window, &x, &y);
        return Vector2DInt{x, y};
    }

    void DisplayImpl::SetPosition(const Vector2DInt& position) noexcept
    {
        ASSERT(window != nullptr);
        SDL_SetWindowPosition(window, position.GetX(), position.GetY());
    }

    Vector2DInt DisplayImpl::GetSize() const noexcept
    {
        return GetClientSize();
    }

    void DisplayImpl::SetSize(const Vector2DInt& size) noexcept
    {
        SetClientSize(size);
    }

    Vector2DInt DisplayImpl::GetClientSize() const noexcept
    {
        ASSERT(window != nullptr);

        Int32 width  = 0;
        Int32 height = 0;
        SDL_GetWindowSize(window, &width, &height);
        return Vector2DInt{width, height};
    }

    void DisplayImpl::SetClientSize(const Vector2DInt& size) noexcept
    {
        ASSERT(window != nullptr);
        SDL_SetWindowSize(window, size.GetX(), size.GetY());
    }

    Vector2DInt DisplayImpl::GetPixelSize() const noexcept
    {
        ASSERT(window != nullptr);

        Int32 width  = 0;
        Int32 height = 0;
        SDL_GetWindowSizeInPixels(window, &width, &height);
        return Vector2DInt{width, height};
    }

    bool DisplayImpl::ShouldClose() const noexcept
    {
        return shouldClose;
    }

    void DisplayImpl::SetShouldClose(bool value) noexcept
    {
        shouldClose = value;
    }

    bool DisplayImpl::IsVisible() const noexcept
    {
        return (GetFlags() & SDL_WINDOW_HIDDEN) == 0;
    }

    void DisplayImpl::Show() noexcept
    {
        ASSERT(window != nullptr);
        SDL_ShowWindow(window);
    }

    void DisplayImpl::Hide() noexcept
    {
        ASSERT(window != nullptr);
        SDL_HideWindow(window);
    }

    bool DisplayImpl::IsFocused() const noexcept
    {
        return (GetFlags() & SDL_WINDOW_INPUT_FOCUS) != 0;
    }

    UInt64 DisplayImpl::GetFlags() const noexcept
    {
        ASSERT(window != nullptr);
        return static_cast<UInt64>(SDL_GetWindowFlags(window));
    }
} // namespace Garage
