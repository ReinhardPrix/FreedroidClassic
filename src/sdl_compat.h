#ifndef _sdl_compat_h
#define _sdl_compat_h

#include <SDL3/SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

SDL_Surface *FD_SetVideoMode(int width, int height, int bpp, Uint32 flags);
SDL_Window *FD_GetWindow(void);
void FD_DestroyWindow(void);

#ifdef __cplusplus
}
#endif

#ifndef SDL_FULLSCREEN
#define SDL_FULLSCREEN 0x80000000u
#endif

#ifndef SDL_SRCCOLORKEY
#define SDL_SRCCOLORKEY 0x00001000u
#endif

#ifndef SDL_SRCALPHA
#define SDL_SRCALPHA 0x00010000u
#endif

#ifndef SDLK_LAST
#define SDLK_LAST 512
#endif

#ifndef SDLK_KP0
#define SDLK_KP0 SDLK_KP_0
#define SDLK_KP1 SDLK_KP_1
#define SDLK_KP2 SDLK_KP_2
#define SDLK_KP3 SDLK_KP_3
#define SDLK_KP4 SDLK_KP_4
#define SDLK_KP5 SDLK_KP_5
#define SDLK_KP6 SDLK_KP_6
#define SDLK_KP7 SDLK_KP_7
#define SDLK_KP8 SDLK_KP_8
#define SDLK_KP9 SDLK_KP_9
#endif

#ifndef SDLK_NUMLOCK
#define SDLK_NUMLOCK SDLK_NUMLOCKCLEAR
#endif

#ifndef SDLK_SCROLLOCK
#define SDLK_SCROLLOCK SDLK_SCROLLLOCK
#endif

#ifndef SDLK_RMETA
#define SDLK_RMETA SDLK_RGUI
#endif

#ifndef SDLK_LMETA
#define SDLK_LMETA SDLK_LGUI
#endif

#ifndef SDLK_LSUPER
#define SDLK_LSUPER SDLK_LGUI
#endif

#ifndef SDLK_RSUPER
#define SDLK_RSUPER SDLK_RGUI
#endif

#ifndef SDLK_COMPOSE
#define SDLK_COMPOSE SDLK_APPLICATION
#endif

#ifndef SDLK_PRINT
#define SDLK_PRINT SDLK_PRINTSCREEN
#endif

#ifndef SDLK_BREAK
#define SDLK_BREAK SDLK_PAUSE
#endif

#ifndef SDLK_EURO
#define SDLK_EURO SDLK_CURRENCYUNIT
#endif

typedef SDL_Keymod SDLMod;

#endif
