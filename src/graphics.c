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
 */
/* ----------------------------------------------------------------------
 * This file contains graphics primitives, such as initialisation of SDL
 * and video modes and fonts.
 * ---------------------------------------------------------------------- */
/*
 * This file has been checked for remains of german comments in the code
 * I you still find some, please just kill it mercilessly.
 */
#define _graphics_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"
#include "map.h"
#include "text.h"
#include "colodefs.h"
#include "SDL_rotozoom.h"

/* XPM */
static const char *arrow[] = {
  /* width height num_colors chars_per_pixel */
  "    32    32        3            1",
  /* colors */
  "X c #000000",
  ". c #ffffff",
  "  c None",
  /* pixels */
  "                                ",
  "                                ",
  "               XXXX             ",
  "               X..X             ",
  "               X..X             ",
  "               X..X             ",
  "               X..X             ",
  "               X..X             ",
  "               X..X             ",
  "               X..X             ",
  "               X..X             ",
  "               XXXX             ",
  "                                ",
  "   XXXXXXXXXXX      XXXXXXXXXX  ",
  "   X.........X      X........X  ",
  "   X.........X      X........X  ",
  "   XXXXXXXXXXX      XXXXXXXXXX  ",
  "                                ",
  "               XXXX             ",
  "               X..X             ",
  "               X..X             ",
  "               X..X             ",
  "               X..X             ",
  "               X..X             ",
  "               X..X             ",
  "               X..X             ",
  "               X..X             ",
  "               X..X             ",
  "               X..X             ",
  "               XXXX             ",
  "                                ",
  "                                ",
  "0,0"
};

SDL_Surface *BackupMapBlockSurfacePointer[ NUM_COLORS ][ NUM_MAP_BLOCKS ] = { { NULL , NULL } , { NULL , NULL } } ; 

/* ----------------------------------------------------------------------
 * This function was taken directly from the example in the SDL docu.
 * Even there they say they have stolen if from the mailing list.
 * Anyway it should create a new mouse cursor from an XPM.
 * The XPM is defined above and not read in from disk or something.
 * ---------------------------------------------------------------------- */
static SDL_Cursor *
init_system_cursor(const char *image[])
{
  int i, row, col;
  Uint8 data[4*32];
  Uint8 mask[4*32];
  int hot_x, hot_y;

  i = -1;
  for ( row=0; row<32; ++row ) {
    for ( col=0; col<32; ++col ) {
      if ( col % 8 ) {
        data[i] <<= 1;
        mask[i] <<= 1;
      } else {
        ++i;
        data[i] = mask[i] = 0;
      }
      switch (image[4+row][col]) {
        case 'X':
          data[i] |= 0x01;
          mask[i] |= 0x01;
          break;
        case '.':
          mask[i] |= 0x01;
          break;
        case ' ':
          break;
      }
    }
  }
  sscanf(image[4+row], "%d,%d", &hot_x, &hot_y);
  return SDL_CreateCursor(data, mask, 32, 32, hot_x, hot_y);
};

/* ----------------------------------------------------------------------
 * This function gives the green component of a pixel, using a value of
 * 255 for the most green pixel and 0 for the least green pixel.
 * ---------------------------------------------------------------------- */
Uint8
GetGreenComponent ( SDL_Surface* surface , int x , int y )
{
  SDL_PixelFormat *fmt;
  Uint32 temp, pixel;
  Uint8 green;
  int bpp = surface->format->BytesPerPixel;

  //--------------------
  // First we extract the pixel itself and the
  // format information we need.
  //
  fmt = surface -> format ;
  SDL_LockSurface ( surface ) ;
  // pixel = * ( ( Uint32* ) surface -> pixels ) ;
  //
  //--------------------
  // Now for the longest time we had this command here (which can actually segfault!!)
  //
  // pixel = * ( ( ( Uint32* ) surface -> pixels ) + x + y * surface->w )  ;
  // 
  pixel = * ( ( Uint32* ) ( ( (Uint8*) ( surface -> pixels ) ) + ( x + y * surface->w ) * bpp ) ) ;

  SDL_UnlockSurface ( surface ) ;

  //--------------------
  // Now we can extract the green component
  //
  temp = pixel&fmt->Gmask; /* Isolate green component */
  temp = temp>>fmt->Gshift;/* Shift it down to 8-bit */
  temp = temp<<fmt->Gloss; /* Expand to a full 8-bit number */
  green = (Uint8)temp;

  return ( green ) ;

}; // int GetGreenComponent ( SDL_Surface* SourceSurface , int x , int y )

/* ----------------------------------------------------------------------
 * This function gives the red component of a pixel, using a value of
 * 255 for the most red pixel and 0 for the least red pixel.
 * ---------------------------------------------------------------------- */
Uint8
GetRedComponent ( SDL_Surface* surface , int x , int y )
{
  SDL_PixelFormat *fmt;
  Uint32 temp, pixel;
  Uint8 red;
  int bpp = surface->format->BytesPerPixel;

  //--------------------
  // First we extract the pixel itself and the
  // format information we need.
  //
  fmt = surface -> format ;
  SDL_LockSurface ( surface ) ;
  // pixel = * ( ( Uint32* ) surface -> pixels ) ;
  //--------------------
  // Now for the longest time we had this command here (which can actually segfault!!)
  //
  // pixel = * ( ( ( Uint32* ) surface -> pixels ) + x + y * surface->w )  ;
  // 
  pixel = * ( ( Uint32* ) ( ( (Uint8*) ( surface -> pixels ) ) + ( x + y * surface->w ) * bpp ) ) ;
  SDL_UnlockSurface ( surface ) ;

  //--------------------
  // Now we can extract the red component
  //
  temp = pixel&fmt->Rmask; /* Isolate red component */
  temp = temp>>fmt->Rshift;/* Shift it down to 8-bit */
  temp = temp<<fmt->Rloss; /* Expand to a full 8-bit number */
  red = ( Uint8 ) temp ;

  return ( red ) ;

}; // int GetRedComponent ( SDL_Surface* SourceSurface , int x , int y )

/* ----------------------------------------------------------------------
 * This function gives the blue component of a pixel, using a value of
 * 255 for the most blue pixel and 0 for the least blue pixel.
 * ---------------------------------------------------------------------- */
Uint8
GetBlueComponent ( SDL_Surface* surface , int x , int y )
{
  SDL_PixelFormat *fmt;
  Uint32 temp, pixel;
  Uint8 blue;
  int bpp = surface->format->BytesPerPixel;

  //--------------------
  // First we extract the pixel itself and the
  // format information we need.
  //
  fmt = surface -> format ;
  SDL_LockSurface ( surface ) ;
  // pixel = * ( ( Uint32* ) surface -> pixels ) ;
  //--------------------
  // Now for the longest time we had this command here (which can actually segfault!!)
  //
  // pixel = * ( ( ( Uint32* ) surface -> pixels ) + x + y * surface->w )  ;
  // 
  pixel = * ( ( Uint32* ) ( ( (Uint8*) ( surface -> pixels ) ) + ( x + y * surface->w ) * bpp ) ) ;
  SDL_UnlockSurface ( surface ) ;

  //--------------------
  // Now we can extract the blue component
  //
  temp = pixel&fmt->Bmask;  /* Isolate blue component */
  temp = temp>>fmt->Bshift; /* Shift it down to 8-bit */
  temp = temp<<fmt->Bloss;  /* Expand to a full 8-bit number */
  blue = ( Uint8 ) temp ;

  return ( blue ) ;

}; // int GetBlueComponent ( SDL_Surface* SourceSurface , int x , int y )

/* ----------------------------------------------------------------------
 * This function gives the alpha component of a pixel, using a value of
 * 255 for the most opaque pixel and 0 for the least opaque pixel.
 * ---------------------------------------------------------------------- */
Uint8
GetAlphaComponent ( SDL_Surface* surface , int x , int y )
{
  SDL_PixelFormat *fmt;
  Uint32 temp, pixel;
  Uint8 alpha;
  int bpp = surface->format->BytesPerPixel;

  //--------------------
  // First we extract the pixel itself and the
  // format information we need.
  //
  fmt = surface -> format ;
  SDL_LockSurface ( surface ) ;
  //--------------------
  // Now for the longest time we had this command here (which can actually segfault!!)
  //
  // pixel = * ( ( ( Uint32* ) surface -> pixels ) + x + y * surface->w )  ;
  // 
  pixel = * ( ( Uint32* ) ( ( (Uint8*) ( surface -> pixels ) ) + ( x + y * surface->w ) * bpp ) ) ;
  SDL_UnlockSurface ( surface ) ;

  //--------------------
  // Now we can extract the alpha component
  //
  temp = pixel&fmt->Amask;  /* Isolate alpha component */
  temp = temp>>fmt->Ashift; /* Shift it down to 8-bit */
  temp = temp<<fmt->Aloss;  /* Expand to a full 8-bit number */
  alpha = ( Uint8 ) temp ;

  return ( alpha ) ;

}; // int GetAlphaComponent ( SDL_Surface* SourceSurface , int x , int y )

/* ----------------------------------------------------------------------
 * If you have two SDL surfaces with alpha channel (i.e. each pixel has
 * it's own alpha value) and you blit one surface over some background
 * and then the other surface over that, the result is the same, as if
 * you merge the two alpha surfaces with this function and then blit it
 * once over the background.
 *
 * This function will be very useful for pre-assembling the tux with all
 * it's equippment out of alpha channeled surfaces only.
 *
 * ---------------------------------------------------------------------- */
SDL_Surface* 
Slow_CreateAlphaCombinedSurface ( SDL_Surface* FirstSurface , SDL_Surface* SecondSurface )
{
  SDL_Surface* ThirdSurface; // this will be the surface we return to the calling function.
  int x , y ; // for processing through the surface...
  Uint8 red, green, blue;
  float alpha1, alpha2, alpha3 ;

  //--------------------
  // First we check if the two surfaces have the same size.  If not, an
  // error message will be generated and the program will halt.
  //
  if ( ( FirstSurface -> w != SecondSurface -> w ) ||
       ( FirstSurface -> w != SecondSurface -> w ) )
    {
      DebugPrintf ( 0 , "\nERROR in SDL_Surface* CreateAlphaCombinedSurface ( SDL_Surface* FirstBlit , SDL_Surface* SecondBlit ):  Surface sizes do not match.... " );
      Terminate ( ERR );
    }

  //--------------------
  // Now we create a new surface, best in display format with alpha channel
  // ready to be blitted.
  //
  ThirdSurface = SDL_DisplayFormatAlpha ( FirstSurface );

  //--------------------
  // Now we start to process through the whole surface and examine each
  // pixel.
  //
  for ( y = 0 ; y < FirstSurface -> h ; y ++ )
    {
      for ( x = 0 ; x < FirstSurface -> w ; x ++ )
	{

	  alpha1 = ( ( float ) GetAlphaComponent (  FirstSurface , x , y ) ) / 255.0 ;
	  alpha2 = ( ( float ) GetAlphaComponent ( SecondSurface , x , y ) ) / 255.0 ;
	  alpha3 = 1 - ( 1 - alpha1 ) * ( 1 - alpha2 ) ;

	  //--------------------
	  // In some cases we give exact alpha values...
	  //
	  /*
	  if ( ( x == 64 ) && ( y == 96 ) )
	    {
	      DebugPrintf( 0 , "\nOldAlphaValue 1: %d OldAlphaValue 2: %d " , 
			   GetAlphaComponent (  FirstSurface , x , y ) ,
			   GetAlphaComponent ( SecondSurface , x , y ) 
			   ) ;
	    }
	  */

	  red =  ( alpha2 * GetRedComponent ( SecondSurface , x , y ) +
		   ( 1 - alpha2 ) * alpha1 * GetRedComponent ( FirstSurface , x , y ) ) 
	    / alpha3 ;

	  green =  ( alpha2 * GetGreenComponent ( SecondSurface , x , y ) +
		   ( 1 - alpha2 ) * alpha1 * GetGreenComponent ( FirstSurface , x , y ) ) 
	    / alpha3 ;

	  blue =  ( alpha2 * GetBlueComponent ( SecondSurface , x , y ) +
		   ( 1 - alpha2 ) * alpha1 * GetBlueComponent ( FirstSurface , x , y ) ) 
	    / alpha3 ;

	  putpixel ( ThirdSurface , x , y , 
		     SDL_MapRGBA ( ThirdSurface -> format , red , green , blue , 255.0 * alpha3 ) ) ;

	}
    }

  return ( ThirdSurface );

}; // SDL_Surface* Slow_CreateAlphaCombinedSurface ( SDL_Surface* FirstBlit , SDL_Surface* SecondBlit )

/* ----------------------------------------------------------------------
 * This function can be used to create a new surface that has a certain
 * color filter applied to it.  The default so far will be that the blue
 * color filter will be applied.
 * ---------------------------------------------------------------------- */
SDL_Surface* 
CreateColorFilteredSurface ( SDL_Surface* FirstSurface , int FilterType )
{
  SDL_Surface* ThirdSurface; // this will be the surface we return to the calling function.
  int x , y ; // for processing through the surface...
  Uint8 red, green, blue;
  float alpha3 ;

  //--------------------
  // First we check for null surfaces given...
  //
  if ( FirstSurface == NULL )
    {
      DebugPrintf ( 0 , "\nERROR in SDL_Surface* CreateBlueColorFilteredSurface ( ... ) : NULL PARAMETER GIVEN.\n" );
      Terminate ( ERR );
    }

  //--------------------
  // Now we create a new surface, best in display format with alpha channel
  // ready to be blitted.
  //
  ThirdSurface = SDL_DisplayFormatAlpha ( FirstSurface );

  //--------------------
  // Now we start to process through the whole surface and examine each
  // pixel.
  //
  for ( y = 0 ; y < FirstSurface -> h ; y ++ )
    {
      for ( x = 0 ; x < FirstSurface -> w ; x ++ )
	{

	  alpha3 = GetAlphaComponent ( FirstSurface , x , y ); 

	  if ( FilterType == FILTER_BLUE )
	    {
	      red =  0;
	      green = 0;
	      blue =  ( GetBlueComponent ( FirstSurface , x , y ) + 
			GetRedComponent ( FirstSurface , x , y ) + 
			GetGreenComponent ( FirstSurface , x , y ) ) / 3 ;
	    }
	  else if ( FilterType == FILTER_GREEN )
	    {
	      red =  0;
	      blue = 0;
	      green =  ( GetBlueComponent ( FirstSurface , x , y ) + 
			 GetRedComponent ( FirstSurface , x , y ) + 
			 GetGreenComponent ( FirstSurface , x , y ) ) / 3 ;
	    } 
	  else if ( FilterType == FILTER_RED )
	    {
	      green =  0;
	      blue = 0;
	      red =  ( GetBlueComponent ( FirstSurface , x , y ) + 
		       GetRedComponent ( FirstSurface , x , y ) + 
		       GetGreenComponent ( FirstSurface , x , y ) ) / 3 ;
	    } 

	  putpixel ( ThirdSurface , x , y , 
		     SDL_MapRGBA ( ThirdSurface -> format , red , green , blue , alpha3 ) ) ;

	}
    }

  return ( ThirdSurface );

}; // SDL_Surface* CreateBlueColorFilteredSurface ( SDL_Surface* FirstSurface )

/* ----------------------------------------------------------------------
 * If you have two SDL surfaces with alpha channel (i.e. each pixel has
 * it's own alpha value) and you blit one surface over some background
 * and then the other surface over that, the result is the same, as if
 * you merge the two alpha surfaces with this function and then blit it
 * once over the background.
 *
 * This function will be very useful for pre-assembling the tux with all
 * it's equippment out of alpha channeled surfaces only.
 *
 * ---------------------------------------------------------------------- */
SDL_Surface* 
CreateAlphaCombinedSurface ( SDL_Surface* FirstSurface , SDL_Surface* SecondSurface )
{
  SDL_Surface* ThirdSurface; // this will be the surface we return to the calling function.
  int x , y ; // for processing through the surface...
  Uint8 new_red, new_green, new_blue, red1, red2, green1, green2, blue1 , blue2, raw_alpha1, raw_alpha2 ;
  float alpha1, alpha2, alpha3 ;
  SDL_PixelFormat *fmt1;
  SDL_PixelFormat *fmt2;
  Uint32 temp, pixel1 , pixel2 ;
  Uint32 AMask_1 , AMask_2 , RMask_1, RMask_2, GMask_1, GMask_2, BMask_1, BMask_2;
  Uint8 AShift_1 , AShift_2 , RShift_1, RShift_2, GShift_1, GShift_2, BShift_1, BShift_2;
  Uint8 ALoss_1 , ALoss_2 , RLoss_1, RLoss_2, GLoss_1, GLoss_2, BLoss_1, BLoss_2;
  Uint32* pixel_pointer1;
  Uint32* pixel_pointer2;
  Uint32 pixel;
  // Uint8 blue;
  int bpp;
  Uint8 *p;


  //--------------------
  // First we check if the two surfaces have the same size.  If not, an
  // error message will be generated and the program will halt.
  //
  if ( ( FirstSurface -> w != SecondSurface -> w ) ||
       ( FirstSurface -> w != SecondSurface -> w ) )
    {
      DebugPrintf ( 0 , "\nERROR in SDL_Surface* CreateAlphaCombinedSurface ( SDL_Surface* FirstBlit , SDL_Surface* SecondBlit ):  Surface sizes do not match.... " );
      Terminate ( ERR );
    }

  //--------------------
  // Now we create a new surface, best in display format with alpha channel
  // ready to be blitted.
  //
  ThirdSurface = SDL_DisplayFormatAlpha ( FirstSurface );
  bpp = ThirdSurface->format->BytesPerPixel;

  //--------------------
  // Now we prepare some pointer, so that we only have to increase it later on...
  //
  fmt1 = FirstSurface -> format ;
  fmt2 = SecondSurface -> format ;
  SDL_LockSurface ( FirstSurface ) ;
  SDL_LockSurface ( SecondSurface ) ;
  SDL_LockSurface ( ThirdSurface ) ;
  pixel_pointer1 = ( ( ( Uint32* ) FirstSurface -> pixels ) + 0 + 0 * FirstSurface->w )  ;
  pixel_pointer2 = ( ( ( Uint32* ) SecondSurface -> pixels ) + 0 + 0 * SecondSurface->w )  ;
  p = (Uint8 *) ThirdSurface -> pixels + 0 * ThirdSurface -> pitch + 0 * bpp;

  //--------------------
  // Now we prepare some surface properties, so that we don't have to retrieve
  // them again and again later...
  //
  BMask_1 = fmt1->Bmask;  /* Isolate blue component */
  BMask_2 = fmt2->Bmask;  /* Isolate blue component */
  RMask_1 = fmt1->Rmask;  /* Isolate blue component */
  RMask_2 = fmt2->Rmask;  /* Isolate blue component */
  GMask_1 = fmt1->Gmask;  /* Isolate blue component */
  GMask_2 = fmt2->Gmask;  /* Isolate blue component */
  AMask_1 = fmt1->Amask;  /* Isolate blue component */
  AMask_2 = fmt2->Amask;  /* Isolate blue component */

  BLoss_1 = fmt1->Bloss;  /* Isolate blue component */
  BLoss_2 = fmt2->Bloss;  /* Isolate blue component */
  RLoss_1 = fmt1->Rloss;  /* Isolate blue component */
  RLoss_2 = fmt2->Rloss;  /* Isolate blue component */
  GLoss_1 = fmt1->Gloss;  /* Isolate blue component */
  GLoss_2 = fmt2->Gloss;  /* Isolate blue component */
  ALoss_1 = fmt1->Aloss;  /* Isolate blue component */
  ALoss_2 = fmt2->Aloss;  /* Isolate blue component */

  BShift_1 = fmt1->Bshift;  /* Isolate blue component */
  BShift_2 = fmt2->Bshift;  /* Isolate blue component */
  RShift_1 = fmt1->Rshift;  /* Isolate blue component */
  RShift_2 = fmt2->Rshift;  /* Isolate blue component */
  GShift_1 = fmt1->Gshift;  /* Isolate blue component */
  GShift_2 = fmt2->Gshift;  /* Isolate blue component */
  AShift_1 = fmt1->Ashift;  /* Isolate blue component */
  AShift_2 = fmt2->Ashift;  /* Isolate blue component */


  //--------------------
  // Now we start to process through the whole surface and examine each
  // pixel.
  //
  for ( y = 0 ; y < FirstSurface -> h ; y ++ )
    {
      for ( x = 0 ; x < FirstSurface -> w ; x ++ )
	{

	  pixel1 = * pixel_pointer1;
	  pixel2 = * pixel_pointer2;

	  pixel_pointer1 ++;
	  pixel_pointer2 ++;

	  //--------------------
	  // Now we can extract the blue component
	  //
	  temp = pixel1  & BMask_1;  /* Isolate blue component */
	  temp = temp   >> BShift_1; /* Shift it down to 8-bit */
	  temp = temp   << BLoss_1;  /* Expand to a full 8-bit number */
	  blue1 = ( Uint8 ) temp ;

	  temp = pixel2 &  BMask_2;  /* Isolate blue component */
	  temp = temp   >> BShift_2; /* Shift it down to 8-bit */
	  temp = temp   << BLoss_2;  /* Expand to a full 8-bit number */
	  blue2 = ( Uint8 ) temp ;

	  //--------------------
	  // Now we can extract the red component
	  //
	  temp = pixel1& RMask_1; /* Isolate red component */
	  temp = temp>>fmt1->Rshift;/* Shift it down to 8-bit */
	  temp = temp<<fmt1->Rloss; /* Expand to a full 8-bit number */
	  red1 = ( Uint8 ) temp ;

	  temp = pixel2& RMask_2; /* Isolate red component */
	  temp = temp>>fmt2->Rshift;/* Shift it down to 8-bit */
	  temp = temp<<fmt2->Rloss; /* Expand to a full 8-bit number */
	  red2 = ( Uint8 ) temp ;

	  //--------------------
	  // Now we can extract the green component
	  //
	  temp = pixel1 &  GMask_1; /* Isolate green component */
	  temp = temp   >> GShift_1; /* Shift it down to 8-bit */
	  temp = temp   << GLoss_1; /* Expand to a full 8-bit number */
	  green1 = (Uint8)temp;

	  temp = pixel2 &  GMask_2; /* Isolate green component */
	  temp = temp   >> GShift_2;/* Shift it down to 8-bit */
	  temp = temp   << GLoss_2; /* Expand to a full 8-bit number */
	  green2 = (Uint8)temp;

	  //--------------------
	  // Now we can extract the alpha component
	  //
	  temp = pixel1 &  AMask_1;  /* Isolate alpha component */
	  temp = temp   >> AShift_1; /* Shift it down to 8-bit */
	  temp = temp   << ALoss_1;  /* Expand to a full 8-bit number */
	  raw_alpha1 = ( Uint8 ) temp ;

	  temp = pixel2 &  AMask_2;  /* Isolate alpha component */
	  temp = temp   >> AShift_2; /* Shift it down to 8-bit */
	  temp = temp   << ALoss_2;  /* Expand to a full 8-bit number */
	  raw_alpha2 = ( Uint8 ) temp ;



	  //--------------------
	  // Now we can start to do our normal operation like in the
	  // well working but too slow slow variant...
	  //
	  alpha1 = ( ( float ) raw_alpha1 ) / 255.0 ;
	  alpha2 = ( ( float ) raw_alpha2 ) / 255.0 ;
	  alpha3 = 1 - ( 1 - alpha1 ) * ( 1 - alpha2 ) ;

	  // alpha3 = ( 255 * 255  - ( 255 - raw_alpha1 ) * ( 255 - raw_alpha2 ) ) / ( 255.0 * 255.0 )  ;

	  //--------------------
	  // In some cases we give exact alpha values...
	  //
	  if ( alpha3 )
	    {
	      new_red =  ( alpha2 * red2 +
			   ( 1.0 - alpha2 ) * alpha1 * red1 ) 
		/ alpha3 ;

	      new_green =  ( alpha2 * green2 +
			     ( 1.0 - alpha2 ) * alpha1 * green1 ) 
		/ alpha3 ;

	      new_blue =  ( alpha2 * blue2 +
			    ( 1.0 - alpha2 ) * alpha1 * blue1 ) 
		/ alpha3 ;
	    }

	  alpha3 *= 255.0 ; 

	  //--------------------
	  // This putpixel must go out and be replaced by something with
	  // less overhead...
	  //
	  // putpixel ( ThirdSurface , x , y , 
	  // SDL_MapRGBA ( ThirdSurface -> format , new_red , new_green , new_blue , alpha3 ) ) ;

	  switch (bpp)
	    {
	    case 1:
	      // *p = pixel;
	      *p = SDL_MapRGBA ( ThirdSurface -> format , new_red , new_green , new_blue , alpha3 ) ;
	      break;
	      
	    case 2:
	      // *(Uint16 *) p = pixel;
	      *(Uint16 *) p = SDL_MapRGBA ( ThirdSurface -> format , new_red , new_green , new_blue , alpha3 ) ;
	      break;
	      
	    case 3:
	      pixel = SDL_MapRGBA ( ThirdSurface -> format , new_red , new_green , new_blue , alpha3 ) ;

	      if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
		{
		  p[0] = (pixel >> 16) & 0xff;
		  p[1] = (pixel >> 8) & 0xff;
		  p[2] = pixel & 0xff;
		}
	      else
		{
		  p[0] = pixel & 0xff;
		  p[1] = (pixel >> 8) & 0xff;
		  p[2] = (pixel >> 16) & 0xff;
		}
	      break;

	    case 4:
	      *(Uint32 *) p = SDL_MapRGBA ( ThirdSurface -> format , new_red , new_green , new_blue , alpha3 ) ;
	      break;
	    }
	  
	  p += bpp;
	  
	}
    }


  SDL_UnlockSurface ( FirstSurface ) ;
  SDL_UnlockSurface ( SecondSurface ) ;
  SDL_UnlockSurface ( ThirdSurface ) ;

  return ( ThirdSurface );

}; // SDL_Surface* CreateAlphaCombinedSurface ( SDL_Surface* FirstBlit , SDL_Surface* SecondBlit )

/* ----------------------------------------------------------------------
 * This function is used to draw a line between given map tiles.  It is
 * mainly used for the map editor to highlight connections and the 
 * current map tile target.
 * ---------------------------------------------------------------------- */
void 
DrawLineBetweenTiles( float x1 , float y1 , float x2 , float y2 , int Color )
{
  int i;
  int pixx;
  int pixy;
  float tmp;
  float slope;

  if ( (x1 == x2) && (y1 == y2) ) return; // nothing is to be done here


  if (x1 == x2) // infinite slope!! special case, that must be caught!
    {

      if (y1 > y2) // in this case, just interchange 1 and 2
	{
	  tmp = y1;
	  y1=y2;
	  y2=tmp;
	}

      for ( i=0 ; i < (y2 - y1) * Block_Width ; i++ )
	{
	  pixx=User_Rect.x + User_Rect.w/2 - Block_Width * (Me[0].pos.x - x1 );
	  pixy=User_Rect.y + User_Rect.h/2 - Block_Height * (Me[0].pos.y - y1 ) + i ;
	  if ( (pixx <= User_Rect.x) || 
	       (pixx >= User_Rect.x + User_Rect.w -1) || 
	       (pixy <= User_Rect.y ) || 
	       (pixy >= User_Rect.y + User_Rect.h -1) ) continue; 
	  putpixel( Screen , pixx , pixy , Color );
	  putpixel( Screen , pixx-1 , pixy , Color );
	}
      return; 
    }

  if (x1 > x2) // in this case, just interchange 1 and 2
    {
      tmp = x1;
      x1=x2;
      x2=tmp;
      tmp = y1;
      y1=y2;
      y2=tmp;
    }

  //--------------------
  // Now we start the drawing process
  //
  // SDL_LockSurface( Screen );
  //

  slope = ( y2 - y1 ) / (x2 - x1) ;
  for ( i=0 ; i<(x2-x1)*Block_Width ; i++ )
    {
      pixx=User_Rect.x + User_Rect.w/2 - Block_Width * (Me[0].pos.x - x1 ) + i;
      pixy=User_Rect.y + User_Rect.h/2 - Block_Height * (Me[0].pos.y - y1 ) + i * slope ;
      if ( (pixx <= User_Rect.x) || 
	   (pixx >= User_Rect.x + User_Rect.w -1) || 
	   (pixy <= User_Rect.y ) || 
	   (pixy >= User_Rect.y + User_Rect.h -1) ) continue; 
      putpixel( Screen , pixx , pixy , Color );
      putpixel( Screen , pixx , pixy -1 , Color );
    }
  // SDL_UnlockSurface( Screen );
}; // void DrawLineBetweenTiles

/* -----------------------------------------------------------------
 * This function saves a screenshot to disk.
 * The screenshots are names "Screenshot_XX.bmp" where XX is a
 * running number.  
 *
 * NOTE:  This function does NOT check for existing screenshots,
 *        but will silently overwrite them.  No problem in most
 *        cases I think.
 *
 * ----------------------------------------------------------------- */
void
TakeScreenshot( void )
{
  static int Number_Of_Screenshot=0;
  char *Screenshoot_Filename;

  Screenshoot_Filename=malloc(100);
  DebugPrintf (1, "\n\nScreenshoot function called.\n\n");
  sprintf( Screenshoot_Filename , "Screenshot_%02d.bmp", Number_Of_Screenshot );
  DebugPrintf(1, "\n\nScreenshoot function: The Filename is: %s.\n\n" , Screenshoot_Filename );
  SDL_SaveBMP( Screen , Screenshoot_Filename );
  Number_Of_Screenshot++;
  free(Screenshoot_Filename);

}; // void TakeScreenshot(void)

/* ----------------------------------------------------------------------
 * This function draws a "grid" on the screen, that means every
 * "second" pixel is blacked out, thereby generation a fading 
 * effect.  This function was created to fade the background of the 
 * Escape menu and its submenus.
 * ---------------------------------------------------------------------- */
void 
MakeGridOnScreen( SDL_Rect* GridRectangle )
{
  int x,y;
  SDL_Rect TempRect;

  if ( GridRectangle == NULL ) GridRectangle = & User_Rect ;

  DebugPrintf (2, "\nvoid MakeGridOnScreen(...): real function call confirmed.");

  //--------------------
  // We store the grid rectangle for later restoration.
  //
  Copy_Rect ( *GridRectangle , TempRect );

  //--------------------
  // At first we do some sanity check to see if this rectangle does really
  // make sense or not.
  //
  if ( GridRectangle->x < 0 )
    {
      if ( GridRectangle->w >= -GridRectangle->x ) GridRectangle->w += GridRectangle->w ;
      GridRectangle->x = 0;
    }
  if ( GridRectangle->y < 0 )
    {
      if ( GridRectangle->h >= -GridRectangle->y ) GridRectangle->h += GridRectangle->y ;
      GridRectangle->y = 0;
    }
  if ( ( GridRectangle->h <= 0 ) || 
       ( GridRectangle->w <= 0 ) || 
       ( GridRectangle->x >= SCREEN_WIDTH ) ||
       ( GridRectangle->y >= SCREEN_HEIGHT ) )
    {
      Copy_Rect ( TempRect , *GridRectangle );
      return;
    }
  if ( ( GridRectangle->x + GridRectangle->w ) > SCREEN_WIDTH )
    {
      GridRectangle->w = SCREEN_WIDTH - GridRectangle->x ;
    }
  if ( ( GridRectangle->y + GridRectangle->h ) > SCREEN_HEIGHT )
    {
      GridRectangle->h = SCREEN_HEIGHT - GridRectangle->y ;
    }

  //--------------------
  // Now we can start to draw the actual grid rectangle.
  // We do so completely correctly with locked surfaces and everything.
  //
  SDL_LockSurface( Screen );
  for ( y = GridRectangle->y ; y < (GridRectangle->h + GridRectangle->y) ; y++) 
    {
      for ( x = GridRectangle->x ; x < (GridRectangle->x + GridRectangle->w) ; x++ ) 
	{
	  if ((x+y)%2 == 0) 
	    {
	      putpixel( Screen, x, y, 0 );
	    }
	}
    }
  SDL_UnlockSurface( Screen );

  //--------------------
  // We restore the original rectangle..
  //
  Copy_Rect ( TempRect , *GridRectangle );

  DebugPrintf (2, "\nvoid MakeGridOnScreen(...): end of function reached.");

}; // void MakeGridOnSchreen( SDL_Rect* GridRectangle )

/* ----------------------------------------------------------------------
 * This function load an image and displays it directly to the Screen
 * but without updating it.
 * This might be very handy, especially in the Title() function to 
 * display the title image and perhaps also for displaying the ship
 * and that.
 * ---------------------------------------------------------------------- */
void DisplayImage( char *datafile )
{
  SDL_Surface *image;
  
  image = IMG_Load(datafile);
  if ( image == NULL ) {
    fprintf(stderr, "Couldn't load image %s: %s\n",
	    datafile, IMG_GetError());
    Terminate(ERR);
  }

  SDL_BlitSurface(image, NULL, Screen, NULL);

  SDL_FreeSurface(image);

}; // void DisplayImage( char *datafile )


/*
 * replace every occurance of color src by dst in Surface surf
 */
void replace_color (SDL_Surface *surf, SDL_Color src, SDL_Color dst)
{
  int i, j;
    
  for (i=0; i < surf->w; i++)
    for (j=0; j < surf->h; i++)
      ; /* ok, I'll do that later ; */

  return;
}; // void replace_color (SDL_Surface *surf, SDL_Color src, SDL_Color dst)

/* ----------------------------------------------------------------------
 * This function initializes ALL the graphics again, propably after 
 * they have been destroyed by resizing operations.
 * This is done via freeing the old structures and starting the classical
 * allocations routine again.
 * ---------------------------------------------------------------------- */
int
ReInitPictures (void)
{
  int i;
  int j;

  for ( j=0 ; j < NUM_COLORS ; j++ )
    {
      for ( i = 0 ; i < NUM_MAP_BLOCKS ; i++ )
	{
	  SDL_FreeSurface( MapBlockSurfacePointer[j][i] );
	}
    }

  for ( j=0 ; j < DROID_PHASES ; j++ )
    {
      SDL_FreeSurface( InfluencerSurfacePointer[j] ); 
      SDL_FreeSurface( EnemySurfacePointer[j] ); 
    }

  for ( j=0 ; j < DIGITNUMBER ; j++ )
    {
      SDL_FreeSurface( InfluDigitSurfacePointer[j] ); 
      SDL_FreeSurface( EnemyDigitSurfacePointer[j] ); 
    }

  for ( j=0 ; j < NUMBER_OF_ITEM_PICTURES ; j++ )
    {
      SDL_FreeSurface( ItemImageList[j].Surface ); 
    }

  for ( j=0 ; j < ALLBLASTTYPES ; j++ )
    {
      for ( i = 0 ; i < Blastmap[j].phases ; i++ )
	{
	  SDL_FreeSurface( Blastmap[j].SurfacePointer[i] );
	}
    }

  for ( j=0 ; j < Number_Of_Bullet_Types ; j++ )
    {
      for ( i = 0 ; i < Bulletmap[j].phases ; i++ )
	{
	  SDL_FreeSurface( Bulletmap[j].SurfacePointer[i] );
	}
    }

  SDL_FreeSurface ( banner_pic );
  // SDL_FreeSurface ( console_pic );
  SDL_FreeSurface ( static_blocks );

  return (InitPictures());
}; // int ReInitPictures(void)


/* ----------------------------------------------------------------------
 * This function resizes all blocks and structures involved in assembling
 * the combat picture to a new scale.  The new scale is relative to the
 * standard scale with means 64x64 tile size.
 * ---------------------------------------------------------------------- */
void 
SetCombatScaleTo(float ResizeFactor)
{
  int i, j;
  SDL_Surface *tmp;
  // static SDL_Surface *BackupMapBlockSurfacePointer[ NUM_COLORS ][ NUM_MAP_BLOCKS ] = { { NULL , NULL } , { NULL , NULL } } ; 

  // just to be sure, reset the size of the graphics
  // ReInitPictures();

  //--------------------
  // If a backup does not yet exist, we'll make one of each map block
  //
  if ( BackupMapBlockSurfacePointer [ 0 ] [ 0 ] == NULL )
    {
      for ( j=0 ; j < NUM_COLORS ; j++ )
	{
	  for ( i = 0 ; i < NUM_MAP_BLOCKS ; i++ )
	    {
	      BackupMapBlockSurfacePointer [ j ] [ i ] = SDL_DisplayFormat( MapBlockSurfacePointer [ j ] [ i ] );
	    }
	}
    }

  //--------------------
  // Now that we are sure we have a backup available, we can start
  // to resize everything.
  //
  for ( j=0 ; j < NUM_COLORS ; j++ )
    {
      for ( i = 0 ; i < NUM_MAP_BLOCKS ; i++ )
	{
	  //--------------------
	  // Now we resize the map blocks to fullfill the requirements of the
	  // currently set ResizeFactor.
	  //
	  SDL_FreeSurface( MapBlockSurfacePointer[j][i] ); // free the old surface
	  MapBlockSurfacePointer [ j ] [ i ] = 
	    zoomSurface ( BackupMapBlockSurfacePointer [ j ] [ i ] , ResizeFactor , ResizeFactor , 0 );

	  //--------------------
	  // Now we convert all the new map blocks to the current display format,
	  // so that blitting and game performance is normal afterwards.
	  //
	  tmp = MapBlockSurfacePointer[j][i]; // store the surface pointer for freeing it soon
	  MapBlockSurfacePointer[j][i]=SDL_DisplayFormat( MapBlockSurfacePointer[j][i] );
	  SDL_FreeSurface( tmp ); // free the old surface
	}
    }

  Block_Width  = INITIAL_BLOCK_WIDTH * ResizeFactor ;
  Block_Height = INITIAL_BLOCK_HEIGHT * ResizeFactor ;

} // void SetCombatScaleTo(float new_scale);

/* ----------------------------------------------------------------------
 * This function loads the configuration file for a theme, containing 
 * such things as the number of bullet phases and the position for droid
 * digits in that theme.
 * ---------------------------------------------------------------------- */
void 
LoadThemeConfigurationFile(void)
{
  char *Data;
  char *ReadPointer;
  char *fpath;
  char *EndOfThemesBulletData;
  char *EndOfThemesBlastData;
  char *EndOfThemesDigitData;
  int BulletIndex;
  int NumberOfBulletTypesInConfigFile=0;
  
#define END_OF_THEME_DATA_STRING "**** End of theme data section ****"
#define END_OF_THEME_BLAST_DATA_STRING "*** End of themes blast data section ***" 
#define END_OF_THEME_BULLET_DATA_STRING "*** End of themes bullet data section ***" 
#define END_OF_THEME_DIGIT_DATA_STRING "*** End of themes digit data section ***" 


  fpath = find_file ("config.theme", GRAPHICS_DIR, TRUE);

  DebugPrintf ( 0 , "\nvoid LoadThemeConfigurationFile: Data will be taken from file %s.  Commencing...\n " 
		, fpath );

  Data = ReadAndMallocAndTerminateFile( fpath , END_OF_THEME_DATA_STRING ) ;

  EndOfThemesBulletData = LocateStringInData ( Data , END_OF_THEME_BULLET_DATA_STRING );
  EndOfThemesBlastData  = LocateStringInData ( Data , END_OF_THEME_BLAST_DATA_STRING  );
  EndOfThemesDigitData  = LocateStringInData ( Data , END_OF_THEME_DIGIT_DATA_STRING  );

  //--------------------
  // Now the file is read in entirely and
  // we can start to analyze its content, 
  //
#define BLAST_ONE_NUMBER_OF_PHASES_STRING "How many phases in Blast one :"
#define BLAST_TWO_NUMBER_OF_PHASES_STRING "How many phases in Blast two :"

  ReadValueFromString( Data , BLAST_ONE_NUMBER_OF_PHASES_STRING , "%d" , 
		       &Blastmap[0].phases , EndOfThemesBlastData );

  ReadValueFromString( Data , BLAST_TWO_NUMBER_OF_PHASES_STRING , "%d" , 
		       &Blastmap[1].phases , EndOfThemesBlastData );

  //--------------------
  // Now we read in the total time amount for each animation
  //
#define BLAST_ONE_TOTAL_AMOUNT_OF_TIME_STRING "Time in seconds for the hole animation of blast one :"
#define BLAST_TWO_TOTAL_AMOUNT_OF_TIME_STRING "Time in seconds for the hole animation of blast two :"

  ReadValueFromString( Data , BLAST_ONE_TOTAL_AMOUNT_OF_TIME_STRING , "%lf" , 
		       &Blastmap[0].total_animation_time , EndOfThemesBlastData );

  ReadValueFromString( Data , BLAST_TWO_TOTAL_AMOUNT_OF_TIME_STRING , "%lf" , 
		       &Blastmap[1].total_animation_time , EndOfThemesBlastData );

  //--------------------
  // Next we read in the number of phases that are to be used for each bullet type
  ReadPointer = Data ;
  while ( ( ReadPointer = strstr ( ReadPointer , "For Bullettype Nr.=" ) ) != NULL )
    {
      ReadValueFromString( ReadPointer , "For Bullettype Nr.=" , "%d" , &BulletIndex , EndOfThemesBulletData );
      if ( BulletIndex >= Number_Of_Bullet_Types )
	{
	  GiveStandardErrorMessage ( "LoadThemeConfigurationFile(...)" , "\
A BLAST WAS FOUND TO EXIST OUTSIDE THE BOUNDS OF THE MAP.\n\
The theme configuration file seems to be BOGUS!!!\n\
The number of bullettypes mentioned therein does not match the number\n\
of bullettypes mentioned in the freedroid.ruleset file!!!\n\
\n\
Either there was a specification for the number of phases in a bullet type\n\
that does not at all exist in the ruleset or an existing bullettype has not\n\
been assigned a number of phases in the themes config file.\n\
\n\
This might indicate that either the ruleset file is corrupt or the \n\
theme.config configuration file is corrupt or (less likely) that there\n\
is a severe bug in the reading function.",
				     NO_NEED_TO_INFORM, IS_FATAL );
	}
      ReadValueFromString( ReadPointer , "we will use number of phases=" , "%d" , 
			   &Bulletmap[BulletIndex].phases , EndOfThemesBulletData );
      ReadValueFromString( ReadPointer , "and number of phase changes per second=" , "%lf" , 
			   &Bulletmap[BulletIndex].phase_changes_per_second , EndOfThemesBulletData );
      ReadPointer++;
      NumberOfBulletTypesInConfigFile ++;
    }
  //--------------------
  // LEAVE THIS SECURITY CHECK IN HERE!!!! IT IS VITAL!!!
  // *ALSO* LEAVE IN THE SECURITY CHECK ABOVE!!! PLEASE!!!
  //
  if ( NumberOfBulletTypesInConfigFile != Number_Of_Bullet_Types )
    {
      GiveStandardErrorMessage ( "LoadThemeConfigurationFile(...)" , "\
The theme configuration file seems to be COMPLETELY BOGUS!!!\n\
The number of bullettypes mentioned therein does not match the number\n\
of bullettypes mentioned in the freedroid.ruleset file!!!\n\
\n\
Either there was a specification for the number of phases in a bullet type\n\
that does not at all exist in the ruleset or an existing bullettype has not\n\
been assigned a number of phases in the themes config file.\n\
\n\
This might indicate that either the ruleset file is corrupt or the \n\
theme.config configuration file is corrupt or (less likely) that there\n\
is a severe bug in the reading function.",
				 NO_NEED_TO_INFORM, IS_FATAL );
	}


  
  // --------------------
  // Also decidable from the theme is where in the robot to
  // display the digits.  This must also be read from the configuration
  // file of the theme
  //
#define DIGIT_ONE_POSITION_X_STRING "First digit x :"
#define DIGIT_ONE_POSITION_Y_STRING "First digit y :"
#define DIGIT_TWO_POSITION_X_STRING "Second digit x :"
#define DIGIT_TWO_POSITION_Y_STRING "Second digit y :"
#define DIGIT_THREE_POSITION_X_STRING "Third digit x :"
#define DIGIT_THREE_POSITION_Y_STRING "Third digit y :"

  ReadValueFromString( Data , DIGIT_ONE_POSITION_X_STRING , "%d" , 
		       & ( Digit_Pos[0].x ) , EndOfThemesDigitData );
  ReadValueFromString( Data , DIGIT_ONE_POSITION_Y_STRING , "%d" , 
		       & ( Digit_Pos[0].y ) , EndOfThemesDigitData );

  ReadValueFromString( Data , DIGIT_TWO_POSITION_X_STRING , "%d" , 
		       & ( Digit_Pos[1].x ) , EndOfThemesDigitData );
  ReadValueFromString( Data , DIGIT_TWO_POSITION_Y_STRING , "%d" , 
		       & ( Digit_Pos[1].y ) , EndOfThemesDigitData );

  ReadValueFromString( Data , DIGIT_THREE_POSITION_X_STRING , "%d" , 
		       & ( Digit_Pos[2].x ) , EndOfThemesDigitData );
  ReadValueFromString( Data , DIGIT_THREE_POSITION_Y_STRING , "%d" , 
		       & ( Digit_Pos[2].y ) , EndOfThemesDigitData );

}; // void LoadThemeConfigurationFile ( void )

/* -----------------------------------------------------------------
 * This function does all the bitmap initialisation, so that you
 * later have the bitmaps in perfect form in memory, ready for blitting
 * them to the screen.
 * ----------------------------------------------------------------- */
int
InitPictures (void)
{
  SDL_Surface *tmp;

  Block_Width=INITIAL_BLOCK_WIDTH;
  Block_Height=INITIAL_BLOCK_HEIGHT;
  
  // Loading all these pictures might take a while...
  // and we do not want do deal with huge frametimes, which
  // could box the influencer out of the ship....
  Activate_Conservative_Frame_Computation();

  ShowStartupPercentage ( 18 ) ; 

  // In the following we will be reading in image information.  But the number
  // of images to read in and the way they are displayed might be strongly dependant
  // on the theme.  That is not at all a problem.  We just got to read in the
  // theme configuration file again.  After that is done, the following reading
  // commands will do the right thing...
  LoadThemeConfigurationFile();

  ShowStartupPercentage ( 20 ) ; 

  SDL_SetCursor( init_system_cursor( arrow ) );

  //--------------------
  // Now we create the internal storage for all our blocks 
  //
  tmp = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, vid_bpp, 0, 0, 0, 0);
  if (tmp == NULL) 
    {
      DebugPrintf (1, "\nCould not create static_blocks surface: %s\n", SDL_GetError());
      return (FALSE);
    }
  static_blocks = SDL_DisplayFormat(tmp);  /* the second surface is copied !*/
  SDL_FreeSurface( tmp );
  if (static_blocks == NULL) 
    {
      DebugPrintf (1, "\nSDL_DisplayFormat() has failed: %s\n", SDL_GetError());
      return (FALSE);
    }
  if (SDL_SetColorKey(static_blocks, SDL_SRCCOLORKEY, transp_key) == -1 )
    {
      fprintf (stderr, "Transp setting by SDL_SetColorKey() failed: %s \n",
	       SDL_GetError());
      return (FALSE);
    }

  ShowStartupPercentage ( 22 ) ; 

  //--------------------
  // And now we read in the blocks from various files 
  //
  Load_MapBlock_Surfaces();

  ShowStartupPercentage ( 24 ) ; 

  DebugPrintf( 1 , "\nvoid InitPictures(void): preparing to load droids." );

  Load_Influencer_Surfaces();

  ShowStartupPercentage ( 26 ) ; 

  Load_Enemy_Surfaces();

  ShowStartupPercentage ( 38 ) ; 

  Load_Tux_Surfaces();

  ShowStartupPercentage ( 55 ) ;
  
  Load_Big_Map_Insert_Surfaces();

  ShowStartupPercentage ( 60 ) ; 

  Load_Digit_Surfaces();

  ShowStartupPercentage ( 65 ) ; 

  DebugPrintf( 1 , "\nvoid InitPictures(void): preparing to load blast image file." );
  Load_Blast_Surfaces();

  ShowStartupPercentage ( 70 ) ; 

  DebugPrintf( 1 , "\nvoid InitPictures(void): preparing to load items image file." );
  Load_Item_Surfaces();

  ShowStartupPercentage ( 75 ) ; 

  Load_Mouse_Move_Cursor_Surfaces( );

  // Load_Skill_Level_Button_Surfaces( );

  ShowStartupPercentage ( 80 ) ; 

  DebugPrintf( 1 , "\nvoid InitPictures(void): preparing to load bullet file." );
  DebugPrintf( 1 , "\nvoid InitPictures(void): Number_Of_Bullet_Types : %d." , Number_Of_Bullet_Types );
  Load_Bullet_Surfaces();

  ShowStartupPercentage ( 92 ) ; 
  
  return (TRUE);
};  // int InitPictures ( void )

/* ----------------------------------------------------------------------
 * This function should load all the fonts we'll be using via the SDL
 * BFont library in Freedroid.
 * ---------------------------------------------------------------------- */
void
InitOurBFonts ( void )
{
#define ALL_BFONTS_WE_LOAD 6
  char* fpath;
  int i;
  char* MenuFontFiles[ALL_BFONTS_WE_LOAD] =
    {
      MENU_FONT_FILE,
      MENU_FILLED_FONT_FILE,
      PARA_FONT_FILE,
      FPS_FONT_FILE,
      RED_FONT_FILE,
      BLUE_FONT_FILE,
      
    };
  BFont_Info** MenuFontPointers[ALL_BFONTS_WE_LOAD] =
    {
      &Menu_BFont,
      &Menu_Filled_BFont,
      &Para_BFont,
      &FPS_Display_BFont,
      &Red_BFont,
      &Blue_BFont
    };

  for ( i = 0 ; i < ALL_BFONTS_WE_LOAD ; i ++ )
    {
      fpath = find_file ( MenuFontFiles [ i ] , GRAPHICS_DIR , FALSE);
      if ( ( *MenuFontPointers [ i ] = LoadFont ( fpath ) ) == NULL )
	{
	  fprintf (stderr, "\n\nFont file: '%s'.\n" , MenuFontFiles [ i ] );
	  GiveStandardErrorMessage ( "InitOurBFonts(...)" , "\
A font file for the BFont library was not found.",
				     PLEASE_INFORM, IS_FATAL );
	} 
      else
	{
	  DebugPrintf(1, "\nSDL Menu Font initialisation successful.\n");
	}
    }

}; // InitOutBFonts ( void )

/* -----------------------------------------------------------------
 * This funciton initialises the video display and opens up a 
 * window for graphics display.
 * -----------------------------------------------------------------*/
void
InitVideo (void)
{
  const SDL_VideoInfo *vid_info;
  SDL_Rect **vid_modes;
  char vid_driver[81];
  Uint32 flags;  // flags for SDL video mode 
  char *fpath;

  /* Initialize the SDL library */
  // if ( SDL_Init (SDL_INIT_VIDEO | SDL_INIT_TIMER) == -1 ) 

  if ( SDL_Init (SDL_INIT_VIDEO) == -1 ) 
    {
      fprintf(stderr, "Couldn't initialize SDL: %s\n",SDL_GetError());
      Terminate(ERR);
    } else
      DebugPrintf(1, "\nSDL Video initialisation successful.\n");

  // Now SDL_TIMER is initialized here:

  if ( SDL_InitSubSystem ( SDL_INIT_TIMER ) == -1 ) 
    {
      fprintf(stderr, "Couldn't initialize SDL: %s\n",SDL_GetError());
      Terminate(ERR);
    } else
      DebugPrintf(1, "\nSDL Timer initialisation successful.\n");

  /* clean up on exit */
  atexit (SDL_Quit);

  InitOurBFonts ( );


  vid_info = SDL_GetVideoInfo (); /* just curious */
  SDL_VideoDriverName (vid_driver, 80);
  
  flags = SDL_SWSURFACE | SDL_HWPALETTE ;
  if (fullscreen_on) flags |= SDL_FULLSCREEN;

  vid_modes = SDL_ListModes (NULL, SDL_SWSURFACE);

  if (vid_info->wm_available)  /* if there's a window-manager */
    {
      SDL_WM_SetCaption ("Freedroid", "");
      fpath = find_file (ICON_FILE, GRAPHICS_DIR, FALSE);
      SDL_WM_SetIcon( IMG_Load (fpath), NULL);
    }


  /* 
   * currently only the simple 320x200 mode is supported for 
   * simplicity, as all our graphics are in this format
   * once this is up and running, we'll provide others modes
   * as well.
   */
  vid_bpp = 16; /* start with the simplest */

  #define SCALE_FACTOR 2

  if( !(Screen = SDL_SetVideoMode ( SCREEN_WIDTH, SCREEN_HEIGHT , 0 , flags)) )
    {
      fprintf(stderr, "Couldn't set (2*) 320x240*SCALE_FACTOR video mode: %s\n",
	      SDL_GetError()); 
      exit(-1);
    }

  DisplayImage ( find_file( FREEDROID_LOADING_PICTURE_NAME , GRAPHICS_DIR, FALSE) );
  SDL_Flip ( Screen ) ;

  ShowStartupPercentage ( 10 ) ; 

  if (!mouse_control)  /* hide mouse pointer if not needed */
    SDL_ShowCursor (SDL_DISABLE);

  vid_info = SDL_GetVideoInfo (); /* info about current video mode */

  // RGB of transparent color in our pics 
  transp_rgb.rot   = 199; 
  transp_rgb.gruen =  43; 
  transp_rgb.blau  =  43; 
  // and corresponding key: 
  transp_key = SDL_MapRGB(Screen->format, transp_rgb.rot,
			     transp_rgb.gruen, transp_rgb.blau);

  SDL_SetGamma( 1 , 1 , 1 );
  GameConfig.Current_Gamma_Correction=1;

  return;

}; // InitVideo () 

/* ----------------------------------------------------------------------
 * This function changes the level color to grey.  It is used mainly
 * after a level has been cleared of all other robots, as it was the
 * case in the classical paradroid game.
 * ---------------------------------------------------------------------- */
void
LevelGrauFaerben (void)
{
  CurLevel->color = PD_DARK;
}; // void LevelGrauFaerben (void)

/* ----------------------------------------------------------------------
 * This function fills all the screen or the freedroid window with a 
 * black color.  The name of the function originates from earlier, when
 * we still wrote directly to the vga memory using memset under ms-dos.
 * ---------------------------------------------------------------------- */
void
ClearGraphMem ( void )
{
  // One this function is done, the rahmen at the
  // top of the screen surely is destroyed.  We inform the
  // DisplayBanner function of the matter...
  BannerIsDestroyed=TRUE;

  // 
  SDL_SetClipRect( Screen, NULL );

  // Now we fill the screen with black color...
  SDL_FillRect( Screen , NULL , 0 );
}; // void ClearGraphMem( void )

/* ----------------------------------------------------------------------
 * This function was taken directly from the SDL documentation.  It 
 * returns the pixel value at a given pixel coordinate (x, y).
 *
 * NOTE: The surface must be locked before calling this!
 * ---------------------------------------------------------------------- */
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

}; // Uint32 getpixel(...)


/* ----------------------------------------------------------------------
 * This function was taken directly from the SDL documentation.
 * It sets the pixel in a given surface at pixel coordinates (x, y) to the 
 * given value.
 *
 * NOTE: The surface must be locked before calling this!
 *
 * ---------------------------------------------------------------------- */
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
}; // void putpixel(...)

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
HighlightRectangle ( SDL_Surface* Surface , SDL_Rect Area )
{
  int x , y ;
  unsigned char red, green, blue;
  int max;
  float EnhancementFactor;

  //--------------------
  // First some sanity checks.. one can never now...
  //
  if ( Area.x < 0 ) Area.x = 0;
  if ( Area.x + Area.w >= Surface->w ) Area.w = Surface->w - Area.x - 1;
  if ( Area.y < 0 ) Area.y = 0;
  if ( Area.y + Area.h >= Surface->h ) Area.h = Surface->h - Area.y - 1;

  //--------------------
  // Now we start to process through the whole surface and examine each
  // pixel.
  //
  SDL_LockSurface ( Surface );

  for ( y = Area.y ; y < Area.y+Area.h ; y ++ )
    {
      for ( x = Area.x ; x < Area.x+Area.w ; x ++ )
	{

	  green = GetGreenComponent ( Surface , x , y ) ;
	  red = GetRedComponent ( Surface , x , y ) ;
	  blue = GetBlueComponent ( Surface , x , y ) ;

	  if ( green < red ) max = red; else max = green ; 
	  if ( max < blue ) max = blue;

	  /*
	  EnhancementFactor = 254.0 / (float)max ;
	  // EnhancementFactor = 0 ;
	  red = (float)red * EnhancementFactor ;
	  green = (float)green * EnhancementFactor ;
	  blue = (float)blue * EnhancementFactor ;
	  */
	  EnhancementFactor = 90;
	  
	  if ( red < 255 - EnhancementFactor ) red += EnhancementFactor ;
	  else red = 255;
	  if ( green < 255 - EnhancementFactor ) green += EnhancementFactor ;
	  else green = 255;
	  if ( blue < 255 - EnhancementFactor ) blue += EnhancementFactor ;
	  else green = 255;

	  putpixel ( Surface , x , y , 
		     SDL_MapRGB ( Surface -> format , red , green , blue ) ) ;

	}
    }

  SDL_UnlockSurface ( Surface );

}; // void HighlightRectangle ( SDL_Surface* Surface , SDL_Rect Area )


#undef _graphics_c
