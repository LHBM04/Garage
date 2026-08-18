#pragma once

#include "garage/platform/window.hpp"

#include <SDL3/SDL_video.h>

namespace Garage
{
    class Window_Impl final : public Window
	    {
	    public:
	        explicit Window_Impl(SDL_Window* handle) noexcept;
	        ~Window_Impl() noexcept override;
        
	        [[nodiscard]] std::string GetTitle() const noexcept override;
	        Window& SetTitle(std::string_view title) noexcept override;

	        [[nodiscard]] int GetPositionX() const noexcept override;
	        Window& SetPositionX(int x_) noexcept override;
        
	        [[nodiscard]] int GetPositionY() const noexcept override;
	        Window& SetPositionY(int y_) noexcept override;
        
	        [[nodiscard]] int GetSizeX() const noexcept override;
	        Window& SetSizeX(int x_) noexcept override;
        
	        [[nodiscard]] int GetSizeY() const noexcept override;
	        Window& SetSizeY(int y_) noexcept override;
        
	        SDL_Window* GetNativeHandle();

	    private:
	        /**
             * @brief SDL 창.
             */
	        SDL_Window* handle;
	    };
}
