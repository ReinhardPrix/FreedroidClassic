/* 
 *
 *   Copyright (c) 1994, 2002 Johannes Prix
 *   Copyright (c) 1994, 2002 Reinhard Prix
 *
 *
 *  This file is part of FreeDroid
 *
 *  FreeDroid is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  FreeDroid is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with FreeDroid; see the file COPYING. If not, write to the 
 *  Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
 *  MA  02111-1307  USA
 *
 */

/*----------------------------------------------------------------------
 *
 * Desc: functions to make keyboard access via svgalib somewhat easyer.
 *
 *----------------------------------------------------------------------*/
#include <config.h>

#define _svgaemu_c

#undef DIAGONAL_KEYS_AUS

#include <stdlib.h>
#include <stdio.h>
#include <vga.h>
// #include <vgagl.h>
#include <vgakeyboard.h>

#include "SDL.h"
// #include "SDL_mixer.h"
#include "SDL_image.h"


#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"

// SDL_Surface *screen;
// SDL_Surface *ScaledSurface;

void 
PrepareScaledSurface()
{
  int x;
  int y;
  byte SourcePixel;

  Lock_SDL_Screen();
  SDL_LockSurface( ScaledSurface );

  for ( y = 0; y < SCREENHOEHE; y++ )
    {
      for ( x = 0; x< SCREENBREITE; x++ )
	{
	  SourcePixel=getpixel(screen, x, y);
	  putpixel ( ScaledSurface , x*2   , y*2   , SourcePixel );
	  putpixel ( ScaledSurface , x*2+1 , y*2   , SourcePixel );
	  putpixel ( ScaledSurface , x*2   , y*2+1 , SourcePixel );
	  putpixel ( ScaledSurface , x*2+1 , y*2+1 , SourcePixel );
	}
    }

  Unlock_SDL_Screen();
  SDL_UnlockSurface( ScaledSurface );

  Update_SDL_Screen();
}

void 
Lock_SDL_Screen(void)
{
  /* Lock the screen for direct access to the pixels */
  if ( SDL_MUSTLOCK(screen) ) {
    if ( SDL_LockSurface(screen) < 0 ) {
      fprintf(stderr, "Can't lock screen: %s\n", SDL_GetError());
      return;
    }
  }
} // void Lock_SDL_Screen

void
Unlock_SDL_Screen(void)
{
  if ( SDL_MUSTLOCK(screen) ) {
    SDL_UnlockSurface(screen);
  }
} // void Unlock_SDL_Screen

void
Update_SDL_Screen(void)
{
  /* Update just the part of the display that we've changed */
  // SDL_UpdateRect( screen , 0, 0, SCREENBREITE, SCREENHOEHE);
  SDL_UpdateRect(ScaledSurface, 0, 0, SCREENBREITE*2, SCREENHOEHE*2);
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

void 
gl_printf(int x, int y, const char *fmt,...)
{

}

void gl_expandfont(int fw, int fh, int c, void *sfdp, void *dfdp)
{

}

void gl_setfont(int fw, int fh, void *fdp)
{

}

void gl_colorfont(int fw, int fh, int c, void *fdp)
{

}

void gl_setwritemode(int wm)
{

}

void gl_write(int x, int y, char *s)
{

}

void gl_writen(int x, int y, int n, char *s)
{

}

void gl_setfontcolors(int bg, int fg)
{

}

void gl_setpalettecolor(int c, int r, int b, int g)
{

}

void gl_getpalettecolor(int c, int *r, int *b, int *g)
{

}

void gl_setpalettecolors(int s, int n, void *dp)
{

}

void gl_getpalettecolors(int s, int n, void *dp)
{

}

void 
gl_setpalette(void *p)
{

}

void gl_getpalette(void *p)
{

}

void gl_setrgbpalette(void)
{

}

void gl_clearscreen(int c)
{

}

void gl_scalebox(int w1, int h1, void *sb, int w2, int h2, void *db)
{

}

void gl_setdisplaystart(int x, int y)
{

}

void gl_enableclipping(void)
{

}

void gl_setclippingwindow(int x1, int y1, int x2, int y2)
{

}

void gl_disableclipping(void)
{

}

void gl_putbox(int x, int y, int w, int h, void *dp)
{

}

int gl_setcontextvga(int m)
{
  return (OK);
}

void gl_hline(int x1, int y, int x2, int c)
{

}


int keyboard_init(void)
{
  return 0;
}

int keyboard_init_return_fd(void)
{
  return 0;
}

void keyboard_close(void)
{

}

void keyboard_setdefaulteventhandler(void)
{

}

char *keyboard_getstate(void)
{
  return NULL;
}

void keyboard_clearstate(void)
{

}

void keyboard_translatekeys(int mask)
{

}

int keyboard_keypressed(int scancode)
{
  return 0;
}

int vga_setmode(int mode)
{
  /* Open the display device */
  // screen = SDL_SetVideoMode(320, 200, 0, SDL_SWSURFACE|SDL_FULLSCREEN|SDL_HWPALETTE );
  // screen = SDL_SetVideoMode(320, 200, 0, SDL_SWSURFACE|SDL_FULLSCREEN );
  // screen = SDL_SetVideoMode(320 , 200, 8, SDL_SWSURFACE | SDL_HWPALETTE );
  ScaledSurface = SDL_SetVideoMode(320*2 , 200*2, 8, SDL_SWSURFACE | SDL_HWPALETTE | SDL_RESIZABLE );
  // ScaledSurface = SDL_SetVideoMode(320*2 , 200*2, 8, SDL_SWSURFACE | SDL_HWPALETTE | SDL_RESIZABLE | SDL_FULLSCREEN );
  if ( ScaledSurface == NULL ) {
    fprintf(stderr, "Couldn't set 320x200 video mode: %s\n",
	    SDL_GetError());
    exit(2);
  }

  // SDL_SetGamma( 1.4 , 2.5 , 2.5 );
  SDL_SetGamma( 2 , 2 , 2 );

  // SDL_CreateRGBSurface( SDL_SWSURFACE , 640, 480, 8, screen->Rmask, screen->Gmask, screen->Bmask, screen->Amask);
  screen = SDL_CreateRGBSurface( SDL_SWSURFACE , 320, 200, 8, 0, 0, 0, 0 );

  /*
  if ( SDL_SetColorKey(screen, 0xFFFFFFFF, 254) == (-1) )
    {
      printf("\n\nint vga_setmode(int mode): ERROR in SDL_SetColorKey.\n\nTerminating...\n\n");
      Terminate(ERR);
    }
  */

  return (OK);
}

int vga_hasmode(int mode)
{
  return 1;
}

int vga_setflipchar(int c)
{
  return 1;
}


int vga_clear(void)
{
  return 1;
}

int vga_flip(void)
{
  return 1;

}


int vga_getxdim(void)
{
  return 1;

}

int vga_getydim(void)
{
  return 1;

}

int vga_getcolors(void)
{
  return 1;

}


int vga_setpalette(int index, int red, int green, int blue)
{
  return 1;

}

int vga_getpalette(int index, int *red, int *green, int *blue)
{
  return 1;

}

int vga_setpalvec(int start, int num, int *pal)
{
  return 1;
}

int vga_getpalvec(int start, int num, int *pal)
{
  return 1;
}


int vga_screenoff(void)
{
  return 1;
}

int vga_screenon(void)
{
  return 1;
}


int vga_setcolor(int color)
{
  return 1;
}

// int vga_drawpixel(int x, int y)
//{
//   return 1;
// }

int vga_drawline(int x1, int y1, int x2, int y2)
{
  return 1;
}

int vga_drawscanline(int line, unsigned char *colors)
{
  return 1;
}

int vga_drawscansegment(unsigned char *colors, int x, int y, int length)
{
  return 1;
}

// int vga_getpixel(int x, int y)
// {
//   return 1;
// }

int vga_getscansegment(unsigned char *colors, int x, int y, int length)
{
  return 1;
}


int vga_getch(void)
{
  return 1;
}


int vga_dumpregs(void)
{
  return 1;
}

int 
vga_init(void)
{
  /* Initialize the SDL library */
  if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
    fprintf(stderr, "Couldn't initialize SDL: %s\n",SDL_GetError());
    Terminate(ERR);
  }

  return 0;
}

int 
vga_white(void)
{
  return 0;
}

void 
vga_waitretrace(void)
{

};
    
int 
vga_getdefaultmode(void)
{
  return 0;
};

#undef _svgaemu_c
