#include "system.h"

static SDL_Window *fd_window = NULL;
static char fd_title[128] = "Freedroid";
static SDL_Surface *fd_icon = NULL;

static void
FD_ApplyWindowState(void)
{
	if (!fd_window)
		return;

	SDL_SetWindowTitle(fd_window, fd_title);
	if (fd_icon)
		SDL_SetWindowIcon(fd_window, fd_icon);
}

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

	fd_window = SDL_CreateWindow(fd_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
					 width, height, window_flags);
	if (!fd_window)
		return NULL;

	FD_ApplyWindowState();
	return SDL_GetWindowSurface(fd_window);
}

void
FD_UpdateRect(SDL_Surface *screen, Sint32 x, Sint32 y, Uint32 w, Uint32 h)
{
	SDL_Rect rect;

	(void)screen;
	if (!fd_window)
		return;

	if (w == 0 || h == 0)
	{
		SDL_UpdateWindowSurface(fd_window);
		return;
	}

	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;
	SDL_UpdateWindowSurfaceRects(fd_window, &rect, 1);
}

void
FD_UpdateRects(SDL_Surface *screen, int numrects, SDL_Rect *rects)
{
	(void)screen;
	if (!fd_window)
		return;
	if (numrects <= 0 || !rects)
	{
		SDL_UpdateWindowSurface(fd_window);
		return;
	}
	SDL_UpdateWindowSurfaceRects(fd_window, rects, numrects);
}

void
FD_WM_SetCaption(const char *title, const char *icon)
{
	(void)icon;
	if (title)
	{
		snprintf(fd_title, sizeof(fd_title), "%s", title);
		fd_title[sizeof(fd_title) - 1] = '\0';
	}
	if (fd_window)
		SDL_SetWindowTitle(fd_window, fd_title);
}

void
FD_WM_SetIcon(SDL_Surface *icon, Uint8 *mask)
{
	(void)mask;
	if (fd_icon)
	{
		SDL_FreeSurface(fd_icon);
		fd_icon = NULL;
	}
	if (icon)
		fd_icon = SDL_ConvertSurface(icon, icon->format, 0);
	if (fd_window && fd_icon)
		SDL_SetWindowIcon(fd_window, fd_icon);
}

int
FD_SetGamma(float red, float green, float blue)
{
	float avg;

	if (!fd_window)
		return -1;

	avg = (red + green + blue) / 3.0f;
	if (avg < 0.0f)
		avg = 0.0f;
	return SDL_SetWindowBrightness(fd_window, avg);
}

void
FD_WarpMouse(Uint16 x, Uint16 y)
{
	if (!fd_window)
		return;
	SDL_WarpMouseInWindow(fd_window, x, y);
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

	if (fd_icon)
	{
		SDL_FreeSurface(fd_icon);
		fd_icon = NULL;
	}
}
