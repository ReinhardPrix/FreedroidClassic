/*----------------------------------------------------------------------
 *
 * Desc: functions to make keyboard access via svgalib somewhat easyer.
 *
 *----------------------------------------------------------------------*/

/* 
 *
 *   Copyright (c) 1994, 2002 Johannes Prix
 *   Copyright (c) 1994, 2002 Reinhard Prix
 *
 *
 *  This file is part of Freedroid
 *
 *  Freedroid is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Freedroid is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Freedroid; see the file COPYING. If not, write to the 
 *  Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
 *  MA  02111-1307  USA
 *
 */
#define _svgaemu_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"


#undef DIAGONAL_KEYS_AUS



// SDL_Surface *screen;
// SDL_Surface *ScaledSurface;

/*-----------------------------------------------------------------
 * please document !!
 * 
 *-----------------------------------------------------------------*/
void 
PrepareScaledSurface(int With_Screen_Update)
{
#ifdef NEW_ENGINE
  return;
#else
  int bpp;
  int i,j;
  unsigned char *s;
  unsigned char *p;
  unsigned char *pSecond;
  unsigned char pix;

  Lock_SDL_Screen();
  SDL_LockSurface( ScaledSurface );

  bpp  = screen->format->BytesPerPixel;

  /* Here s is the address to the pixel source */
  // s = (Uint8 *)screen->pixels;
  s = Outline320x200;

  /* Here p is the address to the pixel we want to set */
  p = (Uint8 *)ScaledSurface->pixels;
  pSecond=p+SCREENBREITE*SCALE_FACTOR*bpp;
  
  switch(bpp) 
    {
    case 1:
      switch ( SCALE_FACTOR ) 
	{
	case 1:
	  printf
	    ("\n\nPrepareScaledSurface(): Unhandled SCALE_FACTOR...\n");
	  Terminate(ERR);
	  break;

	case 2:
	  for (j=0; j<SCREENHOEHE; j++)
	    {
	      for (i=0; i<SCREENBREITE; i++)
		{
		  pix=*s;
		  
		  *p=pix;
		  *pSecond=pix;
		  p++;
		  pSecond++;
		  
		  *p=pix;
		  *pSecond=pix;
		  p++;
		  pSecond++;
		  
		  s++;
		}
	      p+=SCREENBREITE*SCALE_FACTOR*bpp;
	      pSecond+=SCREENBREITE*SCALE_FACTOR*bpp;
	    }
	  break;

	case 3:
	  printf("\n\nPrepareScaledSurface(): Unhandled SCALE_FACTOR...\n");
	  Terminate(ERR);
	  break;
	  
	default:
	  printf("\n\nPrepareScaledSurface(): Unhandled SCALE_FACTOR...\n");
	  Terminate(ERR);
	  break;
	} /* switch (SCALE_FACTOR) */
      
      break;
      
    case 2:
      printf("\n\nPrepareScaledSurface(void): Unhandled bpp!...\n");
      Terminate(ERR);
      break;
      
    case 3:
      printf("\n\nPrepareScaledSurface(void):  Unhandled bpp!...\n");
      Terminate(ERR);
      break;
      
    case 4:
      printf("\n\nPrepareScaledSurface(void):  Unhandled bpp!...\n");
      Terminate(ERR);
      break;
    } /* switch (bpp) */

  Unlock_SDL_Screen();
  SDL_UnlockSurface( ScaledSurface );

  if (Draw_Framerate)
    PrintStringFont (ScaledSurface , Font1, 0, RAHMENHOEHE*2 , "FPS: %d", 
		     5*(int)(rintf(0.2/Frame_Time())) );
  if (Draw_Energy)
    PrintStringFont (ScaledSurface , Font1, 0, RAHMENHOEHE*2+FontHeight(Font1) , "Energy: %d", 
		     (int)(rintf(Me.energy)) );


  if (With_Screen_Update) Update_SDL_Screen();


  return;
#endif // !NEW_ENGINE
} // PrepareScaledSurface()


/*-----------------------------------------------------------------
 * 
 * 
 *-----------------------------------------------------------------*/
void 
Lock_SDL_Screen(void)
{
#ifdef NEW_ENGINE
  return;
#else
  /* Lock the screen for direct access to the pixels */
  if ( SDL_MUSTLOCK(screen) ) {
    if ( SDL_LockSurface(screen) < 0 ) {
      fprintf(stderr, "Can't lock screen: %s\n", SDL_GetError());
      return;
    }
  }
#endif
} // Lock_SDL_Screen ()

void
Unlock_SDL_Screen(void)
{
#ifdef NEW_ENGINE
  return;
#else
  if ( SDL_MUSTLOCK(screen) ) {
    SDL_UnlockSurface(screen);
  }
#endif
} // void Unlock_SDL_Screen

void
Update_SDL_Screen(void)
{
#ifdef NEW_ENGINE
  return;
#else
  /* Update just the part of the display that we've changed */
  // SDL_UpdateRect( screen , 0, 0, SCREENBREITE, SCREENHOEHE);
  SDL_UpdateRect(ScaledSurface, 0, 0, SCREENBREITE*SCALE_FACTOR, SCREENHOEHE*SCALE_FACTOR);
#endif
} // void Update_SDL_Screen

/*
 * Return the pixel value at (x, y)
 * NOTE: The surface must be locked before calling this!
 */
Uint32 
getpixel(SDL_Surface *surface, int x, int y)
{
  int bpp = surface->format->BytesPerPixel;
  /* Here p is the address to the pixel we want to retrieve */
  Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
  
  switch(bpp) 
    {
    case 1:
      return *p;
      
    case 2:
      return *(Uint16 *)p;
      
    case 3:
      if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
	return p[0] << 16 | p[1] << 8 | p[2];
      else
	return p[0] | p[1] << 8 | p[2] << 16;
      
    case 4:
      return *(Uint32 *)p;
      
    default:
      return 0;       /* shouldn't happen, but avoids warnings */
    }

} // Uint32 getpixel(...)


/*
 * Set the pixel at (x, y) to the given value
 * NOTE: The surface must be locked before calling this!
 */
void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
} // void putpixel(...)

#undef _svgaemu_c
