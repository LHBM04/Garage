#include "display_internal.hpp"

namespace Garage
{
    DisplayInternal::DisplayInternal(SDL_Window* window)
        : window(window)
    {
    }

    DisplayInternal::~DisplayInternal()
    {
        if (window != nullptr)
        {
            SDL_DestroyWindow(window);
            window = nullptr;
        }
    }

    DisplayInternal::DisplayInternal(DisplayInternal&& other) noexcept
        : window(other.window)
    {
        other.window = nullptr;
    }

    DisplayInternal& DisplayInternal::operator=(DisplayInternal&& other) noexcept
    {
        this->window = other.window;
        other.window = nullptr;
        return *this;
    }

    std::string DisplayInternal::GetCaption() const
    {
        return SDL_GetWindowTitle(window);
    }

    void DisplayInternal::SetCaption(std::string_view caption)
    {
        SDL_SetWindowTitle(window, caption.data());
    }

    int DisplayInternal::GetSizeX() const
    {
        int sizeX = 0;
        SDL_GetWindowSize(window, &sizeX, nullptr);
        return sizeX;
    }

    void DisplayInternal::SetSizeX(int sizeX)
    {
        int sizeY = 0;
        SDL_GetWindowSize(window, nullptr, &sizeY);
        SDL_SetWindowSize(window, sizeX, sizeY);
    }

    int DisplayInternal::GetSizeY() const
    {
        int sizeY = 0;
        SDL_GetWindowSize(window, nullptr, &sizeY);
        return sizeY;
    }

    void DisplayInternal::SetSizeY(int sizeY)
    {
        int sizeX = 0;
        SDL_GetWindowSize(window, &sizeX, nullptr);
        SDL_SetWindowSize(window, sizeX, sizeY);
    }

    int DisplayInternal::GetPositionX() const
    {
        int positionX = 0;
        SDL_GetWindowPosition(window, &positionX, nullptr);
        return positionX;
    }

    void DisplayInternal::SetPositionX(int positionX)
    {
        int positionY = 0;
        SDL_GetWindowPosition(window, nullptr, &positionY);
        SDL_SetWindowPosition(window, positionX, positionY);
    }

    int DisplayInternal::GetPositionY() const
    {
        int positionY = 0;
        SDL_GetWindowPosition(window, nullptr, &positionY);
        return positionY;
    }

    void DisplayInternal::SetPositionY(int positionY)
    {
        int positionX = 0;
        SDL_GetWindowPosition(window, &positionX, nullptr);
        SDL_SetWindowPosition(window, positionX, positionY);
    }
}
