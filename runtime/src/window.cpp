#include "garage/platform/window.hpp"

#include <SDL3/SDL.h>

#include "window_impl.hpp"
#include "garage/platform/window_options.hpp"

namespace Garage
{
	Window* Window::Create(const WindowOptions& options)
	{
		const SDL_PropertiesID props = SDL_CreateProperties();
		
		SDL_SetStringProperty(props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, options.title.c_str());

		SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_X_NUMBER, SDL_WINDOWPOS_CENTERED + options.positionX);
		SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_Y_NUMBER, SDL_WINDOWPOS_CENTERED + options.positionY);
		SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, options.sizeX);
		SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, options.sizeY);
	    
	    SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_FULLSCREEN_BOOLEAN, options.flags & Fullscreen);
	    SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_BORDERLESS_BOOLEAN, options.flags & Borderless);
	    SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_RESIZABLE_BOOLEAN, options.flags & Resizable);
		SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_HIGH_PIXEL_DENSITY_BOOLEAN, true);

		SDL_Window* window = SDL_CreateWindowWithProperties(props);
		if (!window)
		{
			SDL_DestroyProperties(props);
			return nullptr;
		}

		SDL_DestroyProperties(props);

		return new Window_Impl(window);
	}
}
