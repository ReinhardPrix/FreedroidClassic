/*----------------------------------------------------------------------
 *
 * Desc: Graphics primitived, such as functions to load LBM or PCX images,
 * 	 to change the vga color table, to activate or deachtivate monitor
 *	 signal, to set video modes etc.
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
#define _graphics_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"
#include "map.h"
#include "text.h"
#include "SDL_rotozoom.h"

void PutPixel (SDL_Surface * surface, int x, int y, Uint32 pixel);
int Load_Fonts (void);
SDL_Surface *Load_Block (char *fpath, int line, int col, SDL_Rect * block);

/* XPM */
static const char *crosshair_xpm[] = {
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


/* XPM */
static const char *arrow_xpm[] = {
  /* width height num_colors chars_per_pixel */
  "    32    32        3            1",
  /* colors */
  "X c #000000",
  ". c #ffffff",
  "  c None",
  /* pixels */
  "X                               ",
  "XX                              ",
  "X.X                             ",
  "X..X                            ",
  "X...X                           ",
  "X....X                          ",
  "X.....X                         ",
  "X......X                        ",
  "X.......X                       ",
  "X........X                      ",
  "X.....XXXXX                     ",
  "X..X..X                         ",
  "X.X X..X                        ",
  "XX  X..X                        ",
  "X    X..X                       ",
  "     X..X                       ",
  "      X..X                      ",
  "      X..X                      ",
  "       XX                       ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "0,0"
};


/* ----------------------------------------------------------------------
 * This function applies a color filter to a given surface
 * ---------------------------------------------------------------------- */
int
ApplyFilter (SDL_Surface *surf, float fred, float fgreen, float fblue)
{
  int x , y ; // for processing through the surface...
  Uint8 red, green, blue, alpha;

  //--------------------
  // First we check for null surfaces given...
  //
  if ( surf == NULL )
    {
      DebugPrintf (0 , "\nERROR: ApplyFilter called with NULL pointer\n" );
      return (ERR);
    }

  //--------------------
  // Now we start to process through the whole surface and examine each
  // pixel.
  //
  for ( y = 0 ; y < surf -> h ; y ++ )
    {
      for ( x = 0 ; x < surf -> w ; x ++ )
	{
	  GetRGBA (surf, x, y, &red, &green, &blue, &alpha);

	  if (alpha == SDL_ALPHA_TRANSPARENT) 
	    continue;

	  red *= fred;
	  green *= fgreen;
	  blue *= fblue;

	  putpixel (surf, x, y, SDL_MapRGBA (surf->format, red, green, blue, alpha) ) ;
	}
    }

  return (OK);

} // Apply_Filter

/* ----------------------------------------------------------------------
 * This function gives the green component of a pixel, using a value of
 * 255 for the most green pixel and 0 for the least green pixel.
 * ---------------------------------------------------------------------- */
void
GetRGBA ( SDL_Surface* surface, int x, int y, Uint8 *r, Uint8 *g, Uint8 *b, Uint8 *a)
{
  SDL_PixelFormat *fmt;
  Uint32 pixel;

  //--------------------
  // First we extract the pixel itself and the
  // format information we need.
  //
  fmt = surface -> format ;
  pixel = * ( ( ( Uint32* ) surface -> pixels ) + x + y * surface->w )  ;

  SDL_GetRGBA (pixel, fmt, r, g, b, a);

}; // int GetRGBA


/*----------------------------------------------------------------------
This function was taken directly from the example in the SDL docu.
Even there they say they have stolen if from the mailing list.
Anyway it should create a new mouse cursor from an XPM.
The XPM is defined above and not read in from disk or something.
----------------------------------------------------------------------*/
static SDL_Cursor *init_system_cursor(const char *image[])
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

/*
----------------------------------------------------------------------
----------------------------------------------------------------------
*/

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
	  pixx = User_Rect.x + User_Rect.w/2 - Block_Width * (Me.pos.x - x1 );
	  pixy = USER_FENSTER_CENTER_Y - Block_Height * (Me.pos.y - y1 ) + i ;
	  if ( (pixx <= User_Rect.x) || 
	       (pixx >= User_Rect.x + User_Rect.w -1) || 
	       (pixy <= User_Rect.y ) || 
	       (pixy >= User_Rect.y + User_Rect.h -1) ) continue; 
	  putpixel( ne_screen , pixx , pixy , Color );
	  putpixel( ne_screen , pixx-1 , pixy , Color );
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
  // SDL_LockSurface( ne_screen );

  slope = ( y2 - y1 ) / (x2 - x1) ;
  for ( i=0 ; i<(x2-x1)*Block_Width ; i++ )
    {
      pixx=User_Rect.x + User_Rect.w/2 - Block_Width * (Me.pos.x - x1 ) + i;
      pixy= USER_FENSTER_CENTER_Y - Block_Height * (Me.pos.y - y1 ) + i * slope ;
      if ( (pixx <= User_Rect.x) || 
	   (pixx >= User_Rect.x + User_Rect.w -1) || 
	   (pixy <= User_Rect.y ) || 
	   (pixy >= User_Rect.y + User_Rect.h -1) ) continue; 
      putpixel( ne_screen , pixx , pixy , Color );
      putpixel( ne_screen , pixx , pixy -1 , Color );
    }

  // SDL_UnlockSurface( ne_screen );

} // void DrawLineBetweenTiles


/*-----------------------------------------------------------------
 * This function saves a screenshot to disk.
 * The screenshots are names "Screenshot_XX.bmp" where XX is a
 * running number.  
 *
 * NOTE:  This function does NOT check for existing screenshots,
 *        but will silently overwrite them.  No problem in most
 *        cases I think.
 *
 *-----------------------------------------------------------------*/
void
TakeScreenshot(void)
{
  static int Number_Of_Screenshot=0;
  char *Screenshoot_Filename;

  Screenshoot_Filename=malloc(100);
  DebugPrintf (1, "\n\nScreenshoot function called.\n\n");
  sprintf( Screenshoot_Filename , "Screenshot_%d.bmp", Number_Of_Screenshot );
  DebugPrintf(1, "\n\nScreenshoot function: The Filename is: %s.\n\n" , Screenshoot_Filename );
  SDL_SaveBMP( ne_screen , Screenshoot_Filename );
  Number_Of_Screenshot++;
  free(Screenshoot_Filename);

} // void TakeScreenshot(void)

/*
----------------------------------------------------------------------
@Desc: This function draws a "grid" on the screen, that means every
       "second" pixel is blacked out, thereby generation a fading 
       effect.  This function was created to fade the background of the 
       Escape menu and its submenus.

@Ret: none
----------------------------------------------------------------------
*/
void 
MakeGridOnScreen( SDL_Rect* Grid_Rectangle )
{
  int x,y;

  if ( Grid_Rectangle == NULL ) Grid_Rectangle = & User_Rect ;

  DebugPrintf (2, "\nvoid MakeGridOnScreen(...): real function call confirmed.");
  SDL_LockSurface( ne_screen );
  for ( y = Grid_Rectangle->y ; y < (Grid_Rectangle->h + Grid_Rectangle->y) ; y++) 
    {
      for ( x = Grid_Rectangle->x ; x < (Grid_Rectangle->x + Grid_Rectangle->w) ; x++ ) 
	{
	  if ((x+y)%2 == 0) 
	    {
	      putpixel( ne_screen, x, y, 0 );
	    }
	}
    }
  
  SDL_UnlockSurface( ne_screen );
  DebugPrintf (2, "\nvoid MakeGridOnScreen(...): end of function reached.");
} // void MakeGridOnSchreen(void)


/*----------------------------------------------------------------------
 * This function load an image and displays it directly to the ne_screen
 * but without updating it.
 * This might be very handy, especially in the Title() function to 
 * display the title image and perhaps also for displaying the ship
 * and that.
 *
 ----------------------------------------------------------------------*/
void DisplayImage( char *datafile )
{
  SDL_Surface *image;
  
  image = IMG_Load(datafile);
  if ( image == NULL ) {
    fprintf(stderr, "Couldn't load image %s: %s\n",
	    datafile, IMG_GetError());
    Terminate(ERR);
  }

  SDL_BlitSurface(image, NULL, ne_screen, NULL);

  SDL_FreeSurface(image);
}

/*----------------------------------------------------------------------
 * This function resizes all blocks and structures involved in assembling
 * the combat picture to a new scale.  The new scale is relative to the
 * standard scale with means scale=1 is 64x64 tile size.
 *
 ----------------------------------------------------------------------*/
void 
SetCombatScaleTo(float scale)
{
  int i, j;
  SDL_Surface *tmp;

  for ( j=0 ; j < NUM_COLORS ; j++ )
    for ( i = 0 ; i < NUM_MAP_BLOCKS ; i++ )
      {
	// if there's already a rescaled version, free it
	if (MapBlockSurfacePointer[j][i] != OrigMapBlockSurfacePointer[j][i])
	  SDL_FreeSurface (MapBlockSurfacePointer[j][i]);
	// then zoom..
	tmp = zoomSurface(OrigMapBlockSurfacePointer[j][i], scale, scale, 0);  
	// and optimize
	MapBlockSurfacePointer[j][i]=SDL_DisplayFormat (tmp);
	SDL_FreeSurface(tmp); // free the old surface
      }


  Block_Width = INITIAL_BLOCK_WIDTH * scale;
  Block_Height= INITIAL_BLOCK_HEIGHT* scale;

  return;

} // void SetCombatScaleTo(float new_scale);

/*
----------------------------------------------------------------------
----------------------------------------------------------------------
*/
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
		       &First_Digit_Pos_X , EndOfThemesDigitData );
  ReadValueFromString( Data , DIGIT_ONE_POSITION_Y_STRING , "%d" , 
		       &First_Digit_Pos_Y , EndOfThemesDigitData );

  ReadValueFromString( Data , DIGIT_TWO_POSITION_X_STRING , "%d" , 
		       &Second_Digit_Pos_X , EndOfThemesDigitData );
  ReadValueFromString( Data , DIGIT_TWO_POSITION_Y_STRING , "%d" , 
		       &Second_Digit_Pos_Y , EndOfThemesDigitData );

  ReadValueFromString( Data , DIGIT_THREE_POSITION_X_STRING , "%d" , 
		       &Third_Digit_Pos_X , EndOfThemesDigitData );
  ReadValueFromString( Data , DIGIT_THREE_POSITION_Y_STRING , "%d" , 
		       &Third_Digit_Pos_Y , EndOfThemesDigitData );

}; // void LoadThemeConfigurationFile ( void )


/*-----------------------------------------------------------------
 * @Desc: get the pics for: druids, bullets, blasts
 * 				
 * 	reads all blocks and puts the right pointers into
 * 	the various structs
 *
 * @Ret: TRUE/FALSE
 *
 *-----------------------------------------------------------------*/
int
InitPictures (void)
{
  static bool first_call= TRUE;
  char *fpath;
  int line, col;
  BFont_Info *oldfont;
  SDL_Rect StdBlock, DigitBlock;
  SDL_Surface *tmp_surf;

  // Loading all these pictures might take a while...
  // and we do not want do deal with huge frametimes, which
  // could box the influencer out of the ship....
  Activate_Conservative_Frame_Computation();

  oldfont = GetCurrentFont ();

  // Important: Global variable containing the _actual_ block-sizes
  Block_Width = INITIAL_BLOCK_WIDTH;
  Block_Height = INITIAL_BLOCK_HEIGHT;

  Set_Rect (StdBlock, 0, 0, Block_Width, Block_Height);
  Set_Rect (DigitBlock, 0,0,INITIAL_DIGIT_WIDTH, INITIAL_DIGIT_HEIGHT);

  Load_Fonts ();

  SetCurrentFont (FPS_Display_BFont);
  printf_SDL (ne_screen, User_Rect.x + 50, SCREENHEIGHT - 100, "Loading Theme config ...");

  LoadThemeConfigurationFile();

  printf_SDL (ne_screen, -1, -1, " ok\n");

  printf_SDL (ne_screen, User_Rect.x + 50, -1, "Loading image data ");
  //---------- get Map blocks
  fpath = find_file (MAP_BLOCK_FILE, GRAPHICS_DIR, TRUE);
  Load_Block (fpath, 0, 0, NULL);	/* init function */
  for (line = 0; line < NUM_COLORS; line ++)
    for (col = 0; col < NUM_MAP_BLOCKS; col ++)
      {
	FreeIfUsed (MapBlockSurfacePointer[line][col]);
	MapBlockSurfacePointer[line][col] = Load_Block (NULL, line, col, &StdBlock);
	OrigMapBlockSurfacePointer[line][col] = MapBlockSurfacePointer[line][col];
      }
  printf_SDL (ne_screen, -1, -1, ".");
  //---------- get Droid-model  blocks
  fpath = find_file (DROID_BLOCK_FILE, GRAPHICS_DIR, TRUE);
  Load_Block (fpath, 0, 0, NULL);
  for (col = 0; col < DROID_PHASES; col ++) 
    {
      FreeIfUsed (InfluencerSurfacePointer[col]);
      FreeIfUsed (EnemySurfacePointer[col]);
      InfluencerSurfacePointer[col] = Load_Block (NULL, 0, col, &StdBlock);
      /* Influence pics are only used in _internal_ blits ==> clear per-surf alpha */
      SDL_SetAlpha (InfluencerSurfacePointer[col], 0, 0); 
      EnemySurfacePointer[col] = Load_Block (NULL, 1, col, &StdBlock);
    }
  // now create the local influ-pic storage by reading a  dummy-block 
  tmp_surf = SDL_CreateRGBSurface( 0 , Block_Width, Block_Height, screen_bpp, 0, 0, 0, 0);
  Me.pic = SDL_DisplayFormatAlpha( tmp_surf ); 
  //  SDL_SetAlpha( Me.pic, SDL_SRCALPHA, SDL_ALPHA_OPAQUE);

  printf_SDL (ne_screen, -1, -1, ".");
  //---------- get Bullet blocks
  fpath = find_file (BULLET_BLOCK_FILE, GRAPHICS_DIR, TRUE);
  Load_Block (fpath, 0, 0, NULL);
  for (line = 0; line < Number_Of_Bullet_Types; line ++)
    for (col = 0; col < Bulletmap[line].phases; col ++)
      {
	FreeIfUsed (Bulletmap[line].SurfacePointer[col]);
	Bulletmap[line].SurfacePointer[col] = Load_Block (NULL, line, col, &StdBlock);
      }
  printf_SDL (ne_screen, -1, -1, ".");

  //---------- get Blast blocks
  fpath = find_file (BLAST_BLOCK_FILE, GRAPHICS_DIR, TRUE);
  Load_Block (fpath, 0, 0, NULL);	
  for (line = 0; line <  ALLBLASTTYPES; line ++)
    for (col = 0; col < Blastmap[line].phases; col ++)
      {
	FreeIfUsed (Blastmap[line].SurfacePointer[col]);
	Blastmap[line].SurfacePointer[col] = Load_Block (NULL, line, col, &StdBlock);
      }
  printf_SDL (ne_screen, -1, -1, ".");
  //---------- get Digit blocks
  fpath = find_file (DIGIT_BLOCK_FILE, GRAPHICS_DIR, TRUE);
  Load_Block (fpath, 0, 0, NULL);
  for (col = 0; col < 10; col++)
    {
      FreeIfUsed (InfluDigitSurfacePointer[col]);
      InfluDigitSurfacePointer[col] = Load_Block (NULL, 0, col, &DigitBlock);
      FreeIfUsed (EnemyDigitSurfacePointer[col]);
      EnemyDigitSurfacePointer[col] = Load_Block (NULL, 0, col + 10, &DigitBlock);
    }
  printf_SDL (ne_screen, -1, -1, ".");

  //---------- get Takeover pics
  GetTakeoverGraphics ();
  printf_SDL (ne_screen, -1, -1, ".");

  FreeIfUsed(ship_on_pic);
  ship_on_pic = IMG_Load (find_file (SHIP_ON_PIC_FILE, GRAPHICS_DIR, TRUE));
  FreeIfUsed(ship_off_pic);
  ship_off_pic= IMG_Load (find_file (SHIP_OFF_PIC_FILE, GRAPHICS_DIR, TRUE));

  // the following are not theme-specific and are therefore only loaded once!
  if (first_call)
    {
      // cursor shapes
      arrow_cursor = init_system_cursor (arrow_xpm);
      crosshair_cursor = init_system_cursor (crosshair_xpm);
      //---------- get Console pictures
      fpath = find_file (CONSOLE_PIC_FILE, GRAPHICS_DIR, FALSE);
      console_pic = Load_Block (fpath, 0, 0, NULL);
      fpath = find_file (CONSOLE_BG_PIC1_FILE, GRAPHICS_DIR, FALSE);
      console_bg_pic1 = Load_Block (fpath, 0, 0, NULL);
      fpath = find_file (CONSOLE_BG_PIC2_FILE, GRAPHICS_DIR, FALSE);
      console_bg_pic2 = Load_Block (fpath, 0, 0, NULL);
      printf_SDL (ne_screen, -1, -1, ".");
      arrow_up = IMG_Load (find_file ("arrow_up.png", GRAPHICS_DIR, FALSE) );
      arrow_down = IMG_Load (find_file ("arrow_down.png", GRAPHICS_DIR, FALSE) );
      arrow_right = IMG_Load (find_file ("arrow_right.png", GRAPHICS_DIR, FALSE) );
      arrow_left = IMG_Load (find_file ("arrow_left.png", GRAPHICS_DIR, FALSE) );
      //---------- get Banner
      fpath = find_file (BANNER_BLOCK_FILE, GRAPHICS_DIR, FALSE);
      banner_pic = Load_Block (fpath, 0, 0, NULL);

      printf_SDL (ne_screen, -1, -1, ".");
    }
  
  printf_SDL (ne_screen, -1, -1, " ok\n");

  SetCurrentFont (oldfont);

  first_call = FALSE;

  return (TRUE);
}				// InitPictures



/*------------------------------------------------------------
 * General block-reading routine: get block from pic-file
 * 
 * fpath: full pathname of picture-file; if NULL: use previous SDL-surf
 * line, col: block-position in pic-file to read block from
 * block: dimension of blocks to consider: if NULL: copy whole pic
 *
 *------------------------------------------------------------*/
SDL_Surface *
Load_Block (char *fpath, int line, int col, SDL_Rect * block)
{
  static SDL_Surface *pic = NULL;
  SDL_Surface *tmp;
  SDL_Rect src, dim;
  SDL_Surface *ret;
  int usealpha;

  if (!fpath && !pic)		/* we need some info.. */
    return (NULL);

  if (fpath)
    {
      if (pic)
	free (pic);
      pic = IMG_Load (fpath);
    }

  if (!block)
    {
      Set_Rect (dim, 0, 0, pic->w, pic->h);
    }
  else
    {
      Set_Rect (dim, 0, 0, block->w, block->h);
    }

  if (pic->format->Amask != 0)
    usealpha = TRUE;
  else
    usealpha = FALSE;

  if (usealpha)
    SDL_SetAlpha (pic, 0, 0);	/* clear per-surf alpha for internal blit */
  tmp = SDL_CreateRGBSurface (0, dim.w, dim.h, screen_bpp, 0, 0, 0, 0);
  if (usealpha)
    ret = SDL_DisplayFormatAlpha (tmp);
  else
    ret = SDL_DisplayFormat (tmp);
  SDL_FreeSurface (tmp);

  Set_Rect (src, col * (dim.w + 2), line * (dim.h + 2), dim.w, dim.h);
  SDL_BlitSurface (pic, &src, ret, NULL);
  if (usealpha)
    SDL_SetAlpha (ret, SDL_SRCALPHA | SDL_RLEACCEL, SDL_ALPHA_OPAQUE);

  return (ret);

}				/* Load_Block() */


/*-----------------------------------------------------------------
 * Initialise the Video display and graphics engine
 *
 *
 *-----------------------------------------------------------------*/
void
Init_Video (void)
{
  const SDL_VideoInfo *vid_info;
  SDL_Rect **vid_modes;
  char vid_driver[81];
  Uint32 flags;  /* flags for SDL video mode */
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


  vid_info = SDL_GetVideoInfo (); /* just curious */
  SDL_VideoDriverName (vid_driver, 80);
  
  flags = SDL_SWSURFACE | SDL_HWPALETTE ;
  if (GameConfig.UseFullscreen) flags |= SDL_FULLSCREEN;

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
  screen_bpp = 16; /* start with the simplest */

  #define SCALE_FACTOR 2

  if( !(ne_screen = SDL_SetVideoMode ( SCREENLEN, SCREENHEIGHT , 0 , flags)) )
    {
      fprintf(stderr, "Couldn't set (2*) 320x240*SCALE_FACTOR video mode: %s\n",
	      SDL_GetError()); 
      exit(-1);
    }

  ne_vid_info = SDL_GetVideoInfo (); /* info about current video mode */

  SDL_SetGamma( 1 , 1 , 1 );
  GameConfig.Current_Gamma_Correction=1;

  return;

} /* InitVideo () */

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
ClearGraphMem ( void )
{
  // One this function is done, the rahmen at the
  // top of the screen surely is destroyed.  We inform the
  // DisplayBanner function of the matter...
  BannerIsDestroyed=TRUE;

  // 
  SDL_SetClipRect( ne_screen, NULL );

  // Now we fill the screen with black color...
  SDL_FillRect( ne_screen , NULL , 0 );

  return;
} // ClearGraphMem( void )


/*----------------------------------------------------------------------
 * Return the pixel value at (x, y)
 * NOTE: The surface must be locked before calling this!
 *----------------------------------------------------------------------*/
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


int 
Load_Fonts (void)
{
  char *fpath;

  // make sure this function is only called once, even if we do ReInit():
  if (Para_BFont) 
    return (OK);

  fpath = find_file (PARA_FONT_FILE, GRAPHICS_DIR, FALSE);
  if ( ( Para_BFont = LoadFont (fpath) ) == NULL )
    {
      DebugPrintf (0, "ERROR: font file named %s was not found.\n", PARA_FONT_FILE );
      Terminate(ERR);
    } else
      DebugPrintf(1, "\nSDL Para Font initialisation successful.\n");

  Menu_BFont = Para_BFont;

  fpath = find_file (FPS_FONT_FILE, GRAPHICS_DIR, FALSE);
  if ( ( FPS_Display_BFont = LoadFont (fpath) ) == NULL )
    {
      DebugPrintf (0, "ERROR: font file named %s was not found.\n", FPS_FONT_FILE);
      Terminate(ERR);
    } else
      DebugPrintf(1, "\nSDL FPS Display Font initialisation successful.\n");

  /* choose a font for highscore displaying... */
  Highscore_BFont = Para_BFont;

  return (OK);
} // Load_Fonts ()

//------------------------------------------------------------
// display "white noise" effect in Rect.
// algorith basically stolen from 
// Greg Knauss's "xteevee" hack in xscreensavers.
//
// timeout is in ms
//------------------------------------------------------------
#define NOISE_COLORS 6
#define NOISE_TILES 8

void
white_noise (SDL_Surface *bitmap, SDL_Rect *rect, int timeout)
{
  int i;
  int x, y;
  int signal_strengh = 60;
  Uint32 grey[NOISE_COLORS];
  Uint8 color;
  SDL_Surface *tmp, *tmp2;
  SDL_Surface *noise_tiles[NOISE_TILES];
  SDL_Rect clip_rect;
  char used_tiles[NOISE_TILES/2+1];
  int next_tile;
  int now;

  for (i=0; i< NOISE_COLORS; i++)
    { 
      color = (Uint8)(((double)(i+1.0)/NOISE_COLORS)*255.0);
      grey[i] = SDL_MapRGB(ne_screen->format, color, color, color);
    }
  
  // produce the tiles
  tmp = SDL_CreateRGBSurface(0, rect->w, rect->h, screen_bpp, 0, 0, 0, 0);
  tmp2 = SDL_DisplayFormat (tmp);
  free(tmp);
  SDL_BlitSurface (bitmap, rect, tmp2, NULL);
  //  printf_SDL (ne_screen, rect->x + 10, rect->y + rect->h/2, "Preparing noise-tiles ");
  for (i=0; i< NOISE_TILES; i++)
    {
      noise_tiles[i] = SDL_DisplayFormat(tmp2);

      for (x = 0; x < rect->w; x++)
	for (y = 0; y < rect->h; y++)
	  if (random()%100 > signal_strengh)
	    PutPixel (noise_tiles[i], x, y, grey[random()%NOISE_COLORS]);

      //      printf_SDL (ne_screen, -1, -1, " %d", i+1);
      //      SDL_BlitSurface (noise_tiles[i], NULL, ne_screen, rect);
      //      SDL_UpdateRect (ne_screen, rect->x, rect->y, rect->w, rect->h);
    }
  free(tmp2);

  memset(used_tiles,-1, sizeof(used_tiles));
  // let's go
  now = SDL_GetTicks();
  
  while (1)
    {
      // pick an old enough tile
      do
	{
	  next_tile = random()%NOISE_TILES;
	  for (i = 0; i < sizeof(used_tiles); i++)
	    {
	      if (next_tile == used_tiles[i])
		{
		  next_tile = -1;
		  break;
		}
	    }
	} while (next_tile == -1);
      memmove(used_tiles,used_tiles+1,sizeof(used_tiles)-1);
      used_tiles[sizeof(used_tiles)-1] = next_tile;
      
      // make sure we can blit the full rect without clipping! (would change *rect!)
      SDL_GetClipRect (ne_screen, &clip_rect);
      SDL_SetClipRect (ne_screen, NULL);
      // set it
      SDL_BlitSurface (noise_tiles[next_tile], NULL, ne_screen, rect);
      SDL_UpdateRect (ne_screen, rect->x, rect->y, rect->w, rect->h);
      usleep(25000);

      if ( (timeout && (SDL_GetTicks()-now > timeout)))
	break;

    } // while (! finished)

  //restore previous clip-rectange
  SDL_SetClipRect (ne_screen, &clip_rect);

  for (i=0; i<NOISE_TILES; i++)
    free(noise_tiles[i]);

  return;
}


#undef _graphics_c
