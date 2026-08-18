#pragma once

#include "garage/platform/window.hpp"

#include <SDL3/SDL.h>

namespace Garage
{
    class Window_Impl final : public Window
    {
    public:
        explicit Window_Impl(SDL_Window* handle) noexcept
            : handle(handle)
        {
        }

        ~Window_Impl() noexcept override
        {
            if (handle)
            {
                SDL_DestroyWindow(handle);
                handle = nullptr;
            }
        }

        [[nodiscard]] std::string GetTitle() const noexcept override
        {
            return SDL_GetWindowTitle(handle);
        }

        Window& SetTitle(std::string_view title) noexcept override
        {
            SDL_SetWindowTitle(handle, title.data());
            return *this;
        }

        [[nodiscard]] int GetPositionX() const noexcept override
        {
            int x, y;
            SDL_GetWindowPosition(handle, &x, &y);
            return x;
        }

        Window& SetPositionX(int x_) noexcept override
        {
            int x, y;
            SDL_GetWindowPosition(handle, &x, &y);
            SDL_SetWindowPosition(handle, x_, y);
            return *this;
        }

        [[nodiscard]] int GetPositionY() const noexcept override
        {
            int x, y;
            SDL_GetWindowPosition(handle, &x, &y);
            return y;
        }

        Window& SetPositionY(int y_) noexcept override
        {
            int x, y;
            SDL_GetWindowPosition(handle, &x, &y);
            SDL_SetWindowPosition(handle, x, y_);
            return *this;
        }

        [[nodiscard]] int GetSizeX() const noexcept override
        {
            int w, h;
            SDL_GetWindowSize(handle, &w, &h);
            return w;
        }

        Window& SetSizeX(int x_) noexcept override
        {
            int w, h;
            SDL_GetWindowSize(handle, &w, &h);
            SDL_SetWindowSize(handle, x_, h);
            return *this;
        }

        [[nodiscard]] int GetSizeY() const noexcept override
        {
            int w, h;
            SDL_GetWindowSize(handle, &w, &h);
            return h;
        }

        Window& SetSizeY(int y_) noexcept override
        {
            int w, h;
            SDL_GetWindowSize(handle, &w, &h);
            SDL_SetWindowSize(handle, w, y_);
            return *this;
        }
        
        SDL_Window* GetNativeHandle()
        {
            return handle;
        }

    private:
        /**
         * @brief SDL 창.
         */
        SDL_Window* handle;
    };
}
