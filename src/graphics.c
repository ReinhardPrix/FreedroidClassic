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

  //--------------------
  // First we extract the pixel itself and the
  // format information we need.
  //
  fmt = surface -> format ;
  SDL_LockSurface ( surface ) ;
  pixel = * ( ( Uint32* ) surface -> pixels ) ;
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

  //--------------------
  // First we extract the pixel itself and the
  // format information we need.
  //
  fmt = surface -> format ;
  SDL_LockSurface ( surface ) ;
  pixel = * ( ( Uint32* ) surface -> pixels ) ;
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

  //--------------------
  // First we extract the pixel itself and the
  // format information we need.
  //
  fmt = surface -> format ;
  SDL_LockSurface ( surface ) ;
  pixel = * ( ( Uint32* ) surface -> pixels ) ;
  SDL_UnlockSurface ( surface ) ;

  //--------------------
  // Now we can extract the green component
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

  //--------------------
  // First we extract the pixel itself and the
  // format information we need.
  //
  fmt = surface -> format ;
  SDL_LockSurface ( surface ) ;
  pixel = * ( ( Uint32* ) surface -> pixels ) ;
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
CreateAlphaCombinedSurface ( SDL_Surface* FirstSurface , SDL_Surface* SecondSurface )
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
  DebugPrintf( 0 , "\nNewAlphaValue: " ) ;
  for ( y = 0 ; y < FirstSurface -> h ; y ++ )
    {
      for ( x = 0 ; x < FirstSurface -> w ; x ++ )
	{

	  alpha1 = ( ( float ) GetAlphaComponent (  FirstSurface , x , y ) ) / 255.0 ;
	  alpha2 = ( ( float ) GetAlphaComponent ( SecondSurface , x , y ) ) / 255.0 ;
	  alpha3 = 1 - ( 1 - alpha1 ) * ( 1 - alpha2 ) ;
	  
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
TakeScreenshot(void)
{
  static int Number_Of_Screenshot=0;
  char *Screenshoot_Filename;

  Screenshoot_Filename=malloc(100);
  DebugPrintf (1, "\n\nScreenshoot function called.\n\n");
  sprintf( Screenshoot_Filename , "Screenshot_%d.bmp", Number_Of_Screenshot );
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
MakeGridOnScreen( SDL_Rect* Grid_Rectangle )
{
  int x,y;

  if ( Grid_Rectangle == NULL ) Grid_Rectangle = & User_Rect ;

  DebugPrintf (2, "\nvoid MakeGridOnScreen(...): real function call confirmed.");
  SDL_LockSurface( Screen );
  for ( y = Grid_Rectangle->y ; y < (Grid_Rectangle->h + Grid_Rectangle->y) ; y++) 
    {
      for ( x = Grid_Rectangle->x ; x < Grid_Rectangle->w ; x++ ) 
	{
	  if ((x+y)%2 == 0) 
	    {
	      putpixel( Screen, x, y, 0 );
	    }
	}
    }
  
  SDL_UnlockSurface( Screen );
  DebugPrintf (2, "\nvoid MakeGridOnScreen(...): end of function reached.");
}; // void MakeGridOnSchreen(void)

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
  SDL_FreeSurface ( console_pic );
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

  // just to be sure, reset the size of the graphics
  ReInitPictures();

  for ( j=0 ; j < NUM_COLORS ; j++ )
    {
      for ( i = 0 ; i < NUM_MAP_BLOCKS ; i++ )
	{
	  //--------------------
	  // Now we resize the map blocks to fullfill the requirements of the
	  // currently set ResizeFactor.
	  //
	  tmp = MapBlockSurfacePointer[j][i]; // store the surface pointer for freeing it soon
	  MapBlockSurfacePointer[j][i]=zoomSurface( MapBlockSurfacePointer[j][i] , ResizeFactor , ResizeFactor , 0 );
	  SDL_FreeSurface( tmp ); // free the old surface

	  //--------------------
	  // Now we convert all the new map blocks to the current display format,
	  // so that blitting and game performance is normal afterwards.
	  //
	  tmp = MapBlockSurfacePointer[j][i]; // store the surface pointer for freeing it soon
	  MapBlockSurfacePointer[j][i]=SDL_DisplayFormat( MapBlockSurfacePointer[j][i] );
	  SDL_FreeSurface( tmp ); // free the old surface
	}
    }

  Block_Width *= ResizeFactor;
  Block_Height *= ResizeFactor;

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
  
#define END_OF_THEME_DATA_STRING "**** End of theme data section ****"
#define END_OF_THEME_BLAST_DATA_STRING "*** End of themes blast data section ***" 
#define END_OF_THEME_BULLET_DATA_STRING "*** End of themes bullet data section ***" 
#define END_OF_THEME_DIGIT_DATA_STRING "*** End of themes digit data section ***" 

  fpath = find_file ("config.theme", GRAPHICS_DIR, TRUE);

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
	  fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
In function 'char* LoadThemeConfigurationFile ( ... ):\n\
\n\
There was a specification for the number of phases in a bullet type\n\
that does not at all exist in the ruleset.\n\
\n\
This might indicate that either the ruleset file is corrupt or the \n\
theme.config configuration file is corrupt or (less likely) that there\n\
is a severe bug in the reading function.\n\
\n\
Please check that your theme and ruleset files are properly set up.\n\
\n\
Please also don't forget, that you might have to run 'make install'\n\
again after you've made modifications to the data files in the source tree.\n\
\n\
Freedroid will terminate now to draw attention to the data problem it could\n\
not resolve.... Sorry, if that interrupts a major game of yours.....\n\
----------------------------------------------------------------------\n\
\n" );
	  Terminate(ERR);
	}
      ReadValueFromString( ReadPointer , "we will use number of phases=" , "%d" , 
			   &Bulletmap[BulletIndex].phases , EndOfThemesBulletData );
      ReadValueFromString( ReadPointer , "and number of phase changes per second=" , "%lf" , 
			   &Bulletmap[BulletIndex].phase_changes_per_second , EndOfThemesBulletData );
      ReadPointer++;
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
  char *fpath;

  Block_Width=INITIAL_BLOCK_WIDTH;
  Block_Height=INITIAL_BLOCK_HEIGHT;
  
  // Loading all these pictures might take a while...
  // and we do not want do deal with huge frametimes, which
  // could box the influencer out of the ship....
  Activate_Conservative_Frame_Computation();

  ShowStartupPercentage ( 12 ) ; 

  // In the following we will be reading in image information.  But the number
  // of images to read in and the way they are displayed might be strongly dependant
  // on the theme.  That is not at all a problem.  We just got to read in the
  // theme configuration file again.  After that is done, the following reading
  // commands will do the right thing...
  LoadThemeConfigurationFile();

  ShowStartupPercentage ( 15 ) ; 

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

  ShowStartupPercentage ( 18 ) ; 

  //--------------------
  // And now we read in the blocks from various files 
  //
  Load_MapBlock_Surfaces();

  ShowStartupPercentage ( 20 ) ; 

  DebugPrintf( 2 , "\nvoid InitPictures(void): preparing to load droids." );

  Load_Influencer_Surfaces();

  ShowStartupPercentage ( 25 ) ; 

  Load_Enemy_Surfaces();

  ShowStartupPercentage ( 30 ) ; 

  Load_Tux_Surfaces();

  ShowStartupPercentage ( 60 ) ; 

  DebugPrintf( 2 , "\nvoid InitPictures(void): preparing to load bullet file." );
  DebugPrintf( 1 , "\nvoid InitPictures(void): Number_Of_Bullet_Types : %d." , Number_Of_Bullet_Types );

  Load_Bullet_Surfaces();

  ShowStartupPercentage ( 65 ) ; 

  DebugPrintf( 2 , "\nvoid InitPictures(void): preparing to load blast image file." );

  Load_Blast_Surfaces();

  ShowStartupPercentage ( 70 ) ; 

  DebugPrintf( 2 , "\nvoid InitPictures(void): preparing to load items image file." );

  Load_Item_Surfaces();

  ShowStartupPercentage ( 75 ) ; 

  DebugPrintf( 2 , "\nvoid InitPictures(void): preparing to load skill icon image file." );

  Load_SkillIcon_Surfaces();

  ShowStartupPercentage ( 80 ) ; 

  DebugPrintf( 2 , "\nvoid InitPictures(void): preparing to load blast image file." );

  Load_Digit_Surfaces();

  ShowStartupPercentage ( 84 ) ; 

  fpath = find_file (NE_BANNER_BLOCK_FILE, GRAPHICS_DIR, FALSE);
  tmp = IMG_Load (fpath); 

  ShowStartupPercentage ( 86 ) ; 

  banner_pic = SDL_DisplayFormat (tmp);
  SDL_FreeSurface (tmp);  

  ShowStartupPercentage ( 88 ) ; 

  // console picture need not be rendered fast or something.  This
  // really has time, so we load it as a surface and do not take the
  // elements apart (they dont have typical block format either)
  fpath = find_file (NE_CONSOLE_PIC_FILE, GRAPHICS_DIR, FALSE);
  console_pic = IMG_Load (fpath); 

  ShowStartupPercentage ( 90 ) ; 

  GetTakeoverGraphics();

  ShowStartupPercentage ( 92 ) ; 
  
  return (TRUE);
};  // int InitPictures ( void )

/* -----------------------------------------------------------------
 * This funciton initialises the video display and opens up a 
 * window for graphics display.
 * -----------------------------------------------------------------*/
void
Init_Video (void)
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

  //--------------------
  // Now we initialize the fonts needed by BFont functions
  fpath = find_file (MENU_FONT_FILE, GRAPHICS_DIR, FALSE);
  if ( ( Menu_BFont = LoadFont (fpath) ) == NULL )
      {
      fprintf (stderr,
	     "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
A font file named %s it wanted to load was not found.\n\
\n\
Please check that the file is present and not corrupted\n\
in your distribution of Freedroid.\n\
\n\
Freedroid will terminate now to point at the error.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" , MENU_FONT_FILE );
        Terminate(ERR);
  } else
  DebugPrintf(1, "\nSDL Menu Font initialisation successful.\n");
  
  fpath = find_file (PARA_FONT_FILE, GRAPHICS_DIR, FALSE);
  if ( ( Para_BFont = LoadFont (fpath) ) == NULL )
    {
      fprintf (stderr,
	     "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
A font file named %s it wanted to load was not found.\n\
\n\
Please check that the file is present and not corrupted\n\
in your distribution of Freedroid.\n\
\n\
Freedroid will terminate now to point at the error.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" , PARA_FONT_FILE );
      Terminate(ERR);
    } else
      DebugPrintf(1, "\nSDL Para Font initialisation successful.\n");

  fpath = find_file (FPS_FONT_FILE, GRAPHICS_DIR, FALSE);
  if ( ( FPS_Display_BFont = LoadFont (fpath) ) == NULL )
    {
      fprintf (stderr,
	     "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
A font file named %s it wanted to load was not found.\n\
\n\
Please check that the file is present and not corrupted\n\
in your distribution of Freedroid.\n\
\n\
Freedroid will terminate now to point at the error.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" , FPS_FONT_FILE );
      Terminate(ERR);
    } else
      DebugPrintf(1, "\nSDL FPS Display Font initialisation successful.\n");

  fpath = find_file ( RED_FONT_FILE, GRAPHICS_DIR, FALSE);
  if ( ( Red_BFont = LoadFont (fpath) ) == NULL )
    {
      fprintf (stderr,
	     "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
A font file named %s it wanted to load was not found.\n\
\n\
Please check that the file is present and not corrupted\n\
in your distribution of Freedroid.\n\
\n\
Freedroid will terminate now to point at the error.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" , RED_FONT_FILE );
      Terminate(ERR);
    } else
      DebugPrintf(1, "\nSDL Red Font initialisation successful.\n");

  fpath = find_file ( BLUE_FONT_FILE, GRAPHICS_DIR, FALSE);
  if ( ( Blue_BFont = LoadFont (fpath) ) == NULL )
    {
      fprintf (stderr,
	     "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
A font file named %s it wanted to load was not found.\n\
\n\
Please check that the file is present and not corrupted\n\
in your distribution of Freedroid.\n\
\n\
Freedroid will terminate now to point at the error.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" , BLUE_FONT_FILE );
      Terminate(ERR);
    } else
      DebugPrintf(1, "\nSDL Blue Font initialisation successful.\n");

  //  SetCurrentFont(Menu_BFont);

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
  /* RGB of transparent color in our pics */
  transp_rgb.rot   = 199; 
  transp_rgb.gruen =  43; 
  transp_rgb.blau  =  43; 
  /* and corresponding key: */
  transp_key = SDL_MapRGB(Screen->format, transp_rgb.rot,
			     transp_rgb.gruen, transp_rgb.blau);

  SDL_SetGamma( 1 , 1 , 1 );
  GameConfig.Current_Gamma_Correction=1;

  return;

}; /* InitVideo () */

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


#undef _graphics_c
