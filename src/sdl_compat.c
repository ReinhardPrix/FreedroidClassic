#include "system.h"

static SDL_Window *fd_window = NULL;

SDL_Surface *
FD_SetVideoMode(int width, int height, int bpp, Uint32 flags)
{
	Uint32 window_flags = SDL_WINDOW_SHOWN;

	(void)bpp;

	if (flags & SDL_FULLSCREEN)
		window_flags |= SDL_WINDOW_FULLSCREEN;

	if (fd_window)
	{
		SDL_DestroyWindow(fd_window);
		fd_window = NULL;
	}

	fd_window = SDL_CreateWindow("Freedroid", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
					 width, height, window_flags);
	if (!fd_window)
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
