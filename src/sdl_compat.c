#include "system.h"

static SDL_Window *fd_window = NULL;

SDL_Surface *
FD_SetVideoMode(int width, int height, int bpp, Uint32 flags)
{
	bool want_fullscreen = ((flags & SDL_FULLSCREEN) != 0);
	SDL_DisplayMode fullscreen_mode;
	SDL_DisplayMode *mode = NULL;
	SDL_DisplayID display = 0;

	(void)bpp;

	if (!fd_window) {
		fd_window = SDL_CreateWindow("Freedroid", width, height, 0);
		if (!fd_window)
			return NULL;
	}

	display = SDL_GetDisplayForWindow(fd_window);
	if (!display)
		display = SDL_GetPrimaryDisplay();

	if (want_fullscreen && display) {
		if (SDL_GetClosestFullscreenDisplayMode(display, width, height, 0.0f, false, &fullscreen_mode))
			mode = &fullscreen_mode;
	}

	if (!SDL_SetWindowFullscreenMode(fd_window, mode))
		return NULL;

	if (!want_fullscreen && !SDL_SetWindowSize(fd_window, width, height))
		return NULL;

	if (!SDL_SetWindowFullscreen(fd_window, want_fullscreen))
		return NULL;

	if (!SDL_SyncWindow(fd_window))
		return NULL;

	return SDL_GetWindowSurface(fd_window);
}

SDL_Window *
FD_GetWindow(void)
{
	return fd_window;
}

void
FD_DestroyWindow(void)
{
	if (fd_window)
	{
		SDL_DestroyWindow(fd_window);
		fd_window = NULL;
	}
}
