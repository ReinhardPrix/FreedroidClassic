/*
 *
 *   Copyright (c) 1994, 2002, 2003  Johannes Prix
 *   Copyright (c) 1994, 2002, 2003  Reinhard Prix
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

/*----------------------------------------------------------------------
 *
 * Desc: Graphics primitived, such as functions to load LBM or PCX images,
 * 	 to change the vga color table, to activate or deachtivate monitor
 *	 signal, to set video modes etc.
 *
 *----------------------------------------------------------------------*/

#define _graphics_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"
#include "map.h"
#include "text.h"
#include "takeover.h"

const SDL_VideoInfo *vid_info;	/* info about current video mode */
int vid_bpp;

int fonts_loaded = FALSE;
char *portrait_raw_mem[NUM_DROIDS];

void PutPixel (SDL_Surface * surface, int x, int y, Uint32 pixel);
int Load_Fonts (void);
SDL_Surface *Load_Block (char *fpath, int line, int col, SDL_Rect * block, int flags);
SDL_RWops *load_raw_pic (const char *fpath, char **raw_mem );
BFont_Info *Duplicate_Font ( const BFont_Info * in_font );

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

      for ( i=0 ; i < (y2 - y1) * Block_Rect.w ; i++ )
	{
	  pixx = User_Rect.x + User_Rect.w/2 - Block_Rect.w * (Me.pos.x - x1 );
	  pixy = UserCenter_y - Block_Rect.h * (Me.pos.y - y1 ) + i ;
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
  for ( i=0 ; i<(x2-x1)*Block_Rect.w ; i++ )
    {
      pixx=User_Rect.x + User_Rect.w/2 - Block_Rect.w * (Me.pos.x - x1 ) + i;
      pixy= UserCenter_y - Block_Rect.h * (Me.pos.y - y1 ) + i * slope ;
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
  char Screenshoot_Filename[100];

  Activate_Conservative_Frame_Computation();

  sprintf( Screenshoot_Filename , "Screenshot_%d.bmp", Number_Of_Screenshot );
  SDL_SaveBMP( ne_screen , Screenshoot_Filename );
  Number_Of_Screenshot++;
  DisplayBanner ("Screenshot", NULL,  BANNER_NO_SDL_UPDATE | BANNER_FORCE_UPDATE );
  MakeGridOnScreen(NULL);
  SDL_Flip (ne_screen);
  Play_Sound (SCREENSHOT_SOUND);

  while (cmd_is_active(CMD_SCREENSHOT)) SDL_Delay(1);

  DisplayBanner (NULL, NULL, BANNER_FORCE_UPDATE );

  return;

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
void
DisplayImage(char *datafile)
{
  SDL_Surface *image;

  image = IMG_Load(datafile);
  if ( image == NULL ) {
    DebugPrintf(0, "ERROR: Couldn't load image %s: %s\n", datafile, IMG_GetError());
    Terminate(ERR);
  }

  if (GameConfig.scale != 1.0)
    ScalePic (&image, GameConfig.scale);

  SDL_BlitSurface(image, NULL, ne_screen, NULL);

  SDL_FreeSurface(image);

  return;

} // DisplayImage()

/*----------------------------------------------------------------------
 * This function resizes all blocks and structures involved in assembling
 * the combat picture to a new scale.  The new scale is relative to the
 * standard scale with means scale=1 is 64x64 tile size.
 *
 * in the first call we assume the Block_Rect to be the original game-size
 * and store this value for future rescalings
 ----------------------------------------------------------------------*/
void
SetCombatScaleTo(float scale)
{
  int i, j;
  static SDL_Rect origBlock;
  static bool firstcall = TRUE;
  SDL_Surface *tmp;

  if (firstcall) Copy_Rect (Block_Rect, origBlock);   // keep that as a backup
  firstcall = FALSE;

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

  Copy_Rect (origBlock, Block_Rect);   // always scale with respect to original size!
  ScaleRect(Block_Rect, scale);

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
  int BulletIndex;

#define END_OF_THEME_DATA_STRING "**** End of theme data section ****"
#define END_OF_THEME_BLAST_DATA_STRING "*** End of themes blast data section ***"
#define END_OF_THEME_BULLET_DATA_STRING "*** End of themes bullet data section ***"
#define END_OF_THEME_DIGIT_DATA_STRING "*** End of themes digit data section ***"

  fpath = find_file ("config.theme", GRAPHICS_DIR, USE_THEME, CRITICAL);

  Data = ReadAndMallocAndTerminateFile( fpath , END_OF_THEME_DATA_STRING ) ;

  //--------------------
  // Now the file is read in entirely and
  // we can start to analyze its content,
  //
#define BLAST_ONE_NUMBER_OF_PHASES_STRING "How many phases in Blast one :"
#define BLAST_TWO_NUMBER_OF_PHASES_STRING "How many phases in Blast two :"

  ReadValueFromString (Data, BLAST_ONE_NUMBER_OF_PHASES_STRING, "%d", &Blastmap[0].phases);

  ReadValueFromString (Data, BLAST_TWO_NUMBER_OF_PHASES_STRING, "%d", &Blastmap[1].phases);

  //--------------------
  // Next we read in the number of phases that are to be used for each bullet type
  ReadPointer = Data ;
  while ( ( ReadPointer = strstr ( ReadPointer , "For Bullettype Nr.=" ) ) != NULL )
    {
      ReadValueFromString (ReadPointer, "For Bullettype Nr.=", "%d", &BulletIndex);
      if ( BulletIndex >= Number_Of_Bullet_Types )
	{
	  DebugPrintf (0, "\n\n\
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
      ReadValueFromString (ReadPointer, "we will use number of phases=", "%d", &Bulletmap[BulletIndex].phases);
      ReadValueFromString (ReadPointer, "and number of phase changes per second=", "%f",
			   &Bulletmap[BulletIndex].phase_changes_per_second);
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

  ReadValueFromString (Data, DIGIT_ONE_POSITION_X_STRING, "%hd", &FirstDigit_Rect.x);
  ReadValueFromString (Data, DIGIT_ONE_POSITION_Y_STRING, "%hd", &FirstDigit_Rect.y);

  ReadValueFromString (Data, DIGIT_TWO_POSITION_X_STRING, "%hd", &SecondDigit_Rect.x);
  ReadValueFromString (Data, DIGIT_TWO_POSITION_Y_STRING, "%hd", &SecondDigit_Rect.y);

  ReadValueFromString (Data, DIGIT_THREE_POSITION_X_STRING, "%hd", &ThirdDigit_Rect.x);
  ReadValueFromString (Data, DIGIT_THREE_POSITION_Y_STRING, "%hd", &ThirdDigit_Rect.y);

  free (Data);

  return;

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
  int line, col, i;
  BFont_Info *oldfont;
  SDL_Surface *tmp;
  char fname[500];

  // Loading all these pictures might take a while...
  // and we do not want do deal with huge frametimes, which
  // could box the influencer out of the ship....
  Activate_Conservative_Frame_Computation();

  oldfont = GetCurrentFont ();

  if (!fonts_loaded)
    Load_Fonts ();

  SetCurrentFont (Font0_BFont);

  init_progress ("Loading pictures");

  LoadThemeConfigurationFile();

  update_progress (15);

  //---------- get Map blocks
  fpath = find_file (MAP_BLOCK_FILE, GRAPHICS_DIR, USE_THEME, CRITICAL);
  Load_Block (fpath, 0, 0, NULL, INIT_ONLY);	/* init function */
  for (line = 0; line < NUM_COLORS; line ++)
    for (col = 0; col < NUM_MAP_BLOCKS; col ++)
      {
	FreeIfUsed (OrigMapBlockSurfacePointer[line][col]);
	OrigMapBlockSurfacePointer[line][col] = Load_Block (NULL, line, col, &OrigBlock_Rect,0);
	MapBlockSurfacePointer[line][col] = OrigMapBlockSurfacePointer[line][col];
      }
  update_progress (20);
  //---------- get Droid-model  blocks
  fpath = find_file (DROID_BLOCK_FILE, GRAPHICS_DIR, USE_THEME, CRITICAL);
  Load_Block (fpath, 0, 0, NULL, INIT_ONLY);
  for (col = 0; col < DROID_PHASES; col ++)
    {
      FreeIfUsed (InfluencerSurfacePointer[col]);
      FreeIfUsed (EnemySurfacePointer[col]);
      InfluencerSurfacePointer[col] = Load_Block (NULL, 0, col, &OrigBlock_Rect, 0);
      EnemySurfacePointer[col] = Load_Block (NULL, 1, col, &OrigBlock_Rect, 0);
      /* Droid pics are only used in _internal_ blits ==> clear per-surf alpha */
      SDL_SetAlpha (InfluencerSurfacePointer[col], 0, 0);
      SDL_SetAlpha (EnemySurfacePointer[col], 0, 0);
    }

  //  SDL_SetAlpha( Me.pic, SDL_SRCALPHA, SDL_ALPHA_OPAQUE);

  update_progress (30);
  //---------- get Bullet blocks
  fpath = find_file (BULLET_BLOCK_FILE, GRAPHICS_DIR, USE_THEME, CRITICAL);
  Load_Block (fpath, 0, 0, NULL, INIT_ONLY);
  for (line = 0; line < Number_Of_Bullet_Types; line ++)
    for (col = 0; col < Bulletmap[line].phases; col ++)
      {
	FreeIfUsed (Bulletmap[line].SurfacePointer[col]);
	Bulletmap[line].SurfacePointer[col] = Load_Block (NULL, line, col, &OrigBlock_Rect, 0);
      }

  update_progress (35);

  //---------- get Blast blocks
  fpath = find_file (BLAST_BLOCK_FILE, GRAPHICS_DIR, USE_THEME, CRITICAL);
  Load_Block (fpath, 0, 0, NULL, INIT_ONLY);
  for (line = 0; line <  ALLBLASTTYPES; line ++)
    for (col = 0; col < Blastmap[line].phases; col ++)
      {
	FreeIfUsed (Blastmap[line].SurfacePointer[col]);
	Blastmap[line].SurfacePointer[col] = Load_Block (NULL, line, col, &OrigBlock_Rect, 0);
      }

  update_progress (45);

  //---------- get Digit blocks
  fpath = find_file (DIGIT_BLOCK_FILE, GRAPHICS_DIR, USE_THEME, CRITICAL);
  Load_Block (fpath, 0, 0, NULL, INIT_ONLY);
  for (col = 0; col < 10; col++)
    {
      FreeIfUsed (InfluDigitSurfacePointer[col]);
      InfluDigitSurfacePointer[col] = Load_Block (NULL, 0, col, &OrigDigit_Rect, 0);
      FreeIfUsed (EnemyDigitSurfacePointer[col]);
      EnemyDigitSurfacePointer[col] = Load_Block (NULL, 0, col + 10, &OrigDigit_Rect, 0);
    }
  update_progress(50);

  //---------- get Takeover pics
  FreeIfUsed(to_blocks);   /* this happens when we do theme-switching */
  fpath = find_file (TO_BLOCK_FILE, GRAPHICS_DIR, USE_THEME, CRITICAL);
  to_blocks = Load_Block (fpath, 0, 0, NULL, 0);

  update_progress (60);

  FreeIfUsed(ship_on_pic);
  ship_on_pic = IMG_Load (find_file (SHIP_ON_PIC_FILE, GRAPHICS_DIR, USE_THEME, CRITICAL));
  FreeIfUsed(ship_off_pic);
  ship_off_pic= IMG_Load (find_file (SHIP_OFF_PIC_FILE, GRAPHICS_DIR, USE_THEME, CRITICAL));

  // the following are not theme-specific and are therefore only loaded once!
  if (first_call)
    {
      //  create the tmp block-build storage
      tmp = SDL_CreateRGBSurface( 0 , Block_Rect.w, Block_Rect.h, vid_bpp, 0, 0, 0, 0);
      BuildBlock = SDL_DisplayFormatAlpha (tmp);
      SDL_FreeSurface (tmp);

      // takeover background pics
      fpath = find_file (TAKEOVER_BG_PIC_FILE, GRAPHICS_DIR, NO_THEME, CRITICAL);
      takeover_bg_pic = Load_Block (fpath, 0, 0, NULL, 0);
      set_takeover_rects (); // setup takeover rectangles

      // cursor shapes
      arrow_cursor = init_system_cursor (arrow_xpm);
      crosshair_cursor = init_system_cursor (crosshair_xpm);
      //---------- get Console pictures
      fpath = find_file (CONSOLE_PIC_FILE, GRAPHICS_DIR, NO_THEME, CRITICAL);
      console_pic = Load_Block (fpath, 0, 0, NULL, 0);
      fpath = find_file (CONSOLE_BG_PIC1_FILE, GRAPHICS_DIR, NO_THEME, CRITICAL);
      console_bg_pic1 = Load_Block (fpath, 0, 0, NULL, 0);
      fpath = find_file (CONSOLE_BG_PIC2_FILE, GRAPHICS_DIR, NO_THEME, CRITICAL);
      console_bg_pic2 = Load_Block (fpath, 0, 0, NULL, 0);

      update_progress (80);

      arrow_up = IMG_Load (find_file ("arrow_up.png", GRAPHICS_DIR, NO_THEME, CRITICAL) );
      arrow_down = IMG_Load (find_file ("arrow_down.png", GRAPHICS_DIR, NO_THEME, CRITICAL) );
      arrow_right = IMG_Load (find_file ("arrow_right.png", GRAPHICS_DIR, NO_THEME, CRITICAL) );
      arrow_left = IMG_Load (find_file ("arrow_left.png", GRAPHICS_DIR, NO_THEME, CRITICAL) );
      //---------- get Banner
      fpath = find_file (BANNER_BLOCK_FILE, GRAPHICS_DIR, NO_THEME, CRITICAL);
      banner_pic = Load_Block (fpath, 0, 0, NULL, 0);

      update_progress (90);
      //---------- get Droid images ----------
      for (i=0; i<NUM_DROIDS; i++)
	{
	  // first check if we find a file with rotation-frames: first try .jpg
	  strcpy( fname, Druidmap[i].druidname );
	  strcat( fname , ".jpg" );
	  fpath = find_file (fname, GRAPHICS_DIR, NO_THEME, IGNORE);
	  // then try with .png
	  if (!fpath)
	    {
	      strcpy( fname, Druidmap[i].druidname );
	      strcat( fname , ".png" );
	      fpath = find_file (fname, GRAPHICS_DIR, NO_THEME, CRITICAL);
	    }

	  packed_portraits[i] = load_raw_pic (fpath, &portrait_raw_mem[i] );
	}

      update_progress (95);
      // we need the 999.png in any case for transparency!
      strcpy( fname, Druidmap[DRUID999].druidname );
      strcat( fname , ".png" );
      fpath = find_file (fname, GRAPHICS_DIR, NO_THEME, CRITICAL);
      pic999 = Load_Block (fpath, 0, 0, NULL, 0);

      // get the Ashes pics
      strcpy (fname, "Ashes.png");
      fpath = find_file (fname, GRAPHICS_DIR, NO_THEME, WARNONLY);
      if (!fpath)
	{
	  DebugPrintf (0, "WARNING: deactivated display of droid-decals\n");
	  GameConfig.ShowDecals = FALSE;
      }
      else
	{
	  Load_Block (fpath, 0, 0, NULL, INIT_ONLY);
	  Decal_pics[0] = Load_Block (NULL, 0, 0, &OrigBlock_Rect, 0);
	  Decal_pics[1] = Load_Block (NULL, 0, 1, &OrigBlock_Rect, 0);
	}

    } // if first_call

  update_progress (96);
  // if scale != 1 then we need to rescale everything now
  ScaleGraphics (GameConfig.scale);

  update_progress (98);

  // make sure bullet-surfaces get re-generated!
  for ( i = 0 ; i < MAXBULLETS ; i++ )
    AllBullets[i].Surfaces_were_generated = FALSE ;

  SetCurrentFont (oldfont);

  first_call = FALSE;

  return (TRUE);

}  // InitPictures


/*----------------------------------------------------------------------
 * load a pic into memory and return the SDL_RWops pointer to it
 *----------------------------------------------------------------------*/
SDL_RWops *
load_raw_pic (const char *fpath, char **raw_mem )
{
    FILE *fp;
    off_t size;

    if ( raw_mem == NULL || (*raw_mem) != NULL ) {
      DebugPrintf ( 0, "Invalid input 'raw_mem': must be pointing to NULL pointer\n");
      Terminate ( ERR );
    }

    // sanity check
    if (!fpath)
      {
	DebugPrintf (0, "ERROR: load_raw_pic() called with NULL argument!\n");
	Terminate (ERR);
      }

    fp = fopen (fpath, "rb");
    if (!fp)
      {
	DebugPrintf (0, "ERROR: could not open file %s. Giving up\n", fpath);
	Terminate (ERR);
      }

    size = FS_filelength (fp);
    (*raw_mem) = MyMalloc (size);
    if (fread ( (*raw_mem), 1, size, fp) != size)
      {
	DebugPrintf (0, "ERROR reading file %s. Giving up...\n", fpath);
	Terminate (ERR);
      }
    fclose (fp);


    return (SDL_RWFromMem( (*raw_mem), size) );

}


/*------------------------------------------------------------
 * General block-reading routine: get block from pic-file
 *
 * fpath: full pathname of picture-file; if NULL: use previous SDL-surf
 * line, col: block-position in pic-file to read block from
 * block: dimension of blocks to consider: if NULL: copy whole pic
 * 	  NOTE: only w and h of block are used!!
 *
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * NOTE: to avoid memory-leaks, use (flags | INIT_ONLY) if you only
 *       call this function to set up a new pic-file to be read.
 *       This will avoid copying & mallocing a new pic, NULL will be returned
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 *------------------------------------------------------------*/
SDL_Surface *
Load_Block (char *fpath, int line, int col, SDL_Rect * block, int flags)
{
  static SDL_Surface *pic = NULL;
  SDL_Surface *tmp;
  SDL_Rect src, dim;
  SDL_Surface *ret;
  int usealpha;

  if (!fpath && !pic)		/* we need some info.. */
    return (NULL);

  if ( (pic != NULL) && (flags == FREE_ONLY) ) {
    SDL_FreeSurface (pic);
    return NULL;
  }

  if (fpath) // initialize: read & malloc new pic, dont' return a copy!!
    {
      if (pic)  // previous pic?
	SDL_FreeSurface (pic);
      pic = IMG_Load (fpath);

    }

  if ( (flags & INIT_ONLY) != FALSE )
    return (NULL); // that's it guys, only initialzing...

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
  tmp = SDL_CreateRGBSurface (0, dim.w, dim.h, vid_bpp, 0, 0, 0, 0);
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

} // Load_Block()


/*-----------------------------------------------------------------
 * Initialise the Video display and graphics engine
 *
 *
 *-----------------------------------------------------------------*/
void
Init_Video (void)
{
  char vid_driver[81];
  Uint32 vid_flags;  		/* flags for SDL video mode */
  char *fpath;
  char *YN[2] = {"no", "yes"};

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
#ifdef ANDROID
  vid_bpp = 16; // Hardcoded Android default
#else
  vid_bpp = vid_info->vfmt->BitsPerPixel;
#endif

  DebugPrintf (0, "Video info summary from SDL:\n");
  DebugPrintf (0, "----------------------------------------------------------------------\n");
  DebugPrintf (0, "Is it possible to create hardware surfaces: %s\n" , YN[vid_info->hw_available]);
  DebugPrintf (0, "Is there a window manager available: %s\n", YN[vid_info->wm_available]);
  DebugPrintf (0, "Are hardware to hardware blits accelerated: %s\n", YN[vid_info->blit_hw]);
  DebugPrintf (0, "Are hardware to hardware colorkey blits accelerated: %s\n", YN[vid_info->blit_hw_CC]);
  DebugPrintf (0, "Are hardware to hardware alpha blits accelerated: %s\n", YN[vid_info->blit_hw_A]);
  DebugPrintf (0, "Are software to hardware blits accelerated: %s\n", YN[vid_info->blit_sw]);
  DebugPrintf (0, "Are software to hardware colorkey blits accelerated: %s\n", YN[vid_info->blit_sw_CC]);
  DebugPrintf (0, "Are software to hardware alpha blits accelerated: %s\n", YN[vid_info->blit_sw_A]);
  DebugPrintf (0, "Are color fills accelerated: %s\n", YN[vid_info->blit_fill]);
  DebugPrintf (0, "Total amount of video memory in Kilobytes: %d\n", vid_info->video_mem);
  DebugPrintf (0, "Pixel format of the video device: bpp = %d, bytes/pixel = %d\n",
	       vid_bpp, vid_info->vfmt->BytesPerPixel);
  DebugPrintf (0, "Video Driver Name: %s\n", vid_driver);
  DebugPrintf (0, "----------------------------------------------------------------------\n");


  //  flags = SDL_HWSURFACE | SDL_DOUBLEBUF;
  vid_flags = 0;
  if (GameConfig.UseFullscreen) vid_flags |= SDL_FULLSCREEN;

  if (vid_info->wm_available)  /* if there's a window-manager */
    {
      SDL_WM_SetCaption ("Freedroid", "");
      fpath = find_file (ICON_FILE, GRAPHICS_DIR, NO_THEME, WARNONLY);
      if ( fpath == NULL ) {
        DebugPrintf ( 0, "Could not find icon file '%s'\n", ICON_FILE );
      } else {
        SDL_Surface *img = IMG_Load (fpath);
        if ( img == NULL ) {
          DebugPrintf ( 0, "IMG_Load failed for icon file '%s'\n", fpath );
        } else {
          SDL_WM_SetIcon( img, NULL);
          SDL_FreeSurface ( img );
        }
      }
    }

  if( !(ne_screen = SDL_SetVideoMode ( Screen_Rect.w, Screen_Rect.h , 0 , vid_flags)) )
    {
      DebugPrintf (0, "ERORR: Couldn't set %d x %d video mode. SDL: %s\n",
		   Screen_Rect.w, Screen_Rect.h, SDL_GetError());
      exit(-1);
    }

  vid_info = SDL_GetVideoInfo (); /* info about current video mode */

  DebugPrintf(1, "Got video mode: ");

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
  SDL_Flip (ne_screen);

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

    /* no valid point to set: return */
    if ( !surface || (x<0) || (x >= surface->w) || (y<0) || (y >= surface->h) )
      return;

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

/*----------------------------------------------------------------------
 *
 *----------------------------------------------------------------------*/
int
Load_Fonts (void)
{
  char *fpath;

  fpath = find_file (PARA_FONT_FILE, GRAPHICS_DIR, NO_THEME, CRITICAL);
  if ( ( Para_BFont = LoadFont (fpath, GameConfig.scale) ) == NULL )
    {
      DebugPrintf (0, "ERROR: font file named %s was not found.\n", PARA_FONT_FILE );
      Terminate(ERR);
    }

  fpath = find_file (FONT0_FILE, GRAPHICS_DIR, NO_THEME, CRITICAL);
  if ( ( Font0_BFont = LoadFont (fpath, GameConfig.scale) ) == NULL )
    {
      DebugPrintf (0, "ERROR: font file named %s was not found.\n", FONT0_FILE);
      Terminate(ERR);
    }

  fpath = find_file (FONT1_FILE, GRAPHICS_DIR, NO_THEME, CRITICAL);
  if ( ( Font1_BFont = LoadFont (fpath, GameConfig.scale) ) == NULL )
    {
      DebugPrintf (0, "ERROR: font file named %s was not found.\n", FONT1_FILE);
      Terminate(ERR);
    }

  fpath = find_file (FONT2_FILE, GRAPHICS_DIR, NO_THEME, CRITICAL);
  if ( ( Font2_BFont = LoadFont (fpath, GameConfig.scale) ) == NULL )
    {
      DebugPrintf (0, "ERROR: font file named %s was not found.\n", FONT2_FILE);
      Terminate(ERR);
    }


  Menu_BFont      = Duplicate_Font ( Para_BFont );
  Highscore_BFont = Duplicate_Font ( Para_BFont );

  fonts_loaded = TRUE;

  return (OK);
} // Load_Fonts ()

BFont_Info *
Duplicate_Font ( const BFont_Info * in_font )
{
  BFont_Info *out_font = MyMalloc ( sizeof(out_font[0]) );

  memcpy ( out_font, in_font, sizeof(out_font[0]) );
  out_font->Surface = SDL_ConvertSurface ( in_font->Surface, in_font->Surface->format, in_font->Surface->flags);
  if ( out_font->Surface == NULL ) {
    DebugPrintf ( 0, "Duplicate_Font: failed to copy SDL_Surface using SDL_ConvertSurface()\n");
    Terminate ( ERR );
  }

  return out_font;
}

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
  tmp = SDL_CreateRGBSurface(0, rect->w, rect->h, vid_bpp, 0, 0, 0, 0);
  tmp2 = SDL_DisplayFormat (tmp);
  SDL_FreeSurface (tmp);
  SDL_BlitSurface (bitmap, rect, tmp2, NULL);
  //  printf_SDL (ne_screen, rect->x + 10, rect->y + rect->h/2, "Preparing noise-tiles ");
  for (i=0; i< NOISE_TILES; i++)
    {
      noise_tiles[i] = SDL_DisplayFormat(tmp2);

      for (x = 0; x < rect->w; x++)
	for (y = 0; y < rect->h; y++)
	  if (rand()%100 > signal_strengh)
	    PutPixel (noise_tiles[i], x, y, grey[rand()%NOISE_COLORS]);

      //      printf_SDL (ne_screen, -1, -1, " %d", i+1);
      //      SDL_BlitSurface (noise_tiles[i], NULL, ne_screen, rect);
      //      SDL_UpdateRect (ne_screen, rect->x, rect->y, rect->w, rect->h);
    }
  SDL_FreeSurface (tmp2);

  memset(used_tiles,-1, sizeof(used_tiles));
  // let's go
  Play_Sound (WHITE_NOISE);

  now = SDL_GetTicks();

  while (1)
    {
      // pick an old enough tile
      do
	{
	  next_tile = rand()%NOISE_TILES;
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
      SDL_Delay(25);

      if ( (timeout && (SDL_GetTicks()-now > timeout)))
	break;

    } // while (! finished)

  //restore previous clip-rectange
  SDL_SetClipRect (ne_screen, &clip_rect);

  for (i=0; i<NOISE_TILES; i++)
    SDL_FreeSurface (noise_tiles[i]);

  return;
}

/*----------------------------------------------------------------------
 * ScaleGraphics ()
 *----------------------------------------------------------------------*/
void
ScaleGraphics (float scale)
{
  static bool first_call = TRUE;
  SDL_Surface *tmp;
  int line, col;

/* For some reason we need to SetAlpha every time on OS X */
  for (col = 0; col < 10; col++)
    {
      /* Digits are only used in _internal_ blits ==> clear per-surf alpha */
      SDL_SetAlpha (InfluDigitSurfacePointer[col], 0, 0);
      SDL_SetAlpha (EnemyDigitSurfacePointer[col], 0, 0);
    }
  if (scale == 1.0)
    return;

  // these are reset in a theme-change by the theme-config-file
  // therefore we need to rescale them each time again
  ScaleRect (FirstDigit_Rect, scale);
  ScaleRect (SecondDigit_Rect, scale);
  ScaleRect (ThirdDigit_Rect, scale);


  // note: only rescale these rects the first time!!
  if (first_call)
    ScaleStatRects (scale);

  //  printf_SDL (ne_screen, User_Rect.x + 50, -1, "Rescaling graphics ...");

  //---------- rescale Map blocks
  for (line = 0; line < NUM_COLORS; line ++)
    for (col = 0; col < NUM_MAP_BLOCKS; col ++)
      {
	ScalePic( &OrigMapBlockSurfacePointer[line][col], scale);
	MapBlockSurfacePointer[line][col] = OrigMapBlockSurfacePointer[line][col];
      }
  //  printf_SDL (ne_screen, -1, -1, ".");
  //---------- rescale Droid-model  blocks
  for (col = 0; col < DROID_PHASES; col ++)
    {
      ScalePic (&InfluencerSurfacePointer[col], scale);
      ScalePic (&EnemySurfacePointer[col], scale);
      /* Droid pics are only used in _internal_ blits ==> clear per-surf alpha */
      SDL_SetAlpha (InfluencerSurfacePointer[col], 0, 0);
      SDL_SetAlpha (EnemySurfacePointer[col], 0, 0);
    }

  //  printf_SDL (ne_screen, -1, -1, ".");
  //---------- rescale Bullet blocks
  for (line = 0; line < Number_Of_Bullet_Types; line ++)
    for (col = 0; col < Bulletmap[line].phases; col ++)
      ScalePic( &Bulletmap[line].SurfacePointer[col], scale);

  //  printf_SDL (ne_screen, -1, -1, ".");

  //---------- rescale Blast blocks
  for (line = 0; line <  ALLBLASTTYPES; line ++)
    for (col = 0; col < Blastmap[line].phases; col ++)
      ScalePic (&Blastmap[line].SurfacePointer[col], scale);

  //  printf_SDL (ne_screen, -1, -1, ".");
  //---------- rescale Digit blocks
  for (col = 0; col < 10; col++)
    {
      ScalePic (&InfluDigitSurfacePointer[col], scale);
      ScalePic (&EnemyDigitSurfacePointer[col], scale);
      /* Digits are only used in _internal_ blits ==> clear per-surf alpha */
      SDL_SetAlpha (InfluDigitSurfacePointer[col], 0, 0);
      SDL_SetAlpha (EnemyDigitSurfacePointer[col], 0, 0);
    }
  //  printf_SDL (ne_screen, -1, -1, ".");

  //---------- rescale Takeover pics
  ScalePic (&to_blocks, scale);
  //  printf_SDL (ne_screen, -1, -1, ".");

  ScalePic (&ship_on_pic, scale);
  ScalePic (&ship_off_pic, scale);

  // the following are not theme-specific and are therefore only loaded once!
  if (first_call)
    {
      //  create a new tmp block-build storage
      FreeIfUsed (BuildBlock);
      tmp = SDL_CreateRGBSurface( 0 , Block_Rect.w, Block_Rect.h, vid_bpp, 0, 0, 0, 0);
      BuildBlock = SDL_DisplayFormatAlpha (tmp);
      SDL_FreeSurface (tmp);

      // takeover pics
      ScalePic (&takeover_bg_pic, scale);

      //---------- Console pictures
      ScalePic (&console_pic, scale);
      ScalePic (&console_bg_pic1, scale);
      ScalePic (&console_bg_pic2, scale);
      ScalePic (&arrow_up, scale);
      ScalePic (&arrow_down, scale);
      ScalePic (&arrow_right, scale);
      ScalePic (&arrow_left, scale);
      //---------- Banner
      ScalePic (&banner_pic, scale);

      ScalePic (&pic999, scale);

      // get the Ashes pics
      if (Decal_pics[0]) ScalePic (&Decal_pics[0], scale);
      if (Decal_pics[1]) ScalePic (&Decal_pics[1], scale);

    } // if first_call

  printf_SDL (ne_screen, -1, -1, " ok\n");

  first_call = FALSE;

  return;

} // ScaleGraphics()

/*----------------------------------------------------------------------
 * scales pic by scale: frees old pic and replaces it by new one!
 *----------------------------------------------------------------------*/
void
ScalePic (SDL_Surface **pic, float scale)
{
  SDL_Surface *tmp;

  if (scale == 1.0)
    return;

  tmp = *pic;
  *pic = zoomSurface (tmp, scale, scale, 0);
  SDL_FreeSurface (tmp);

  return;

} // ScalePic ()

/*----------------------------------------------------------------------
 * scale all "static" rectangles, which are theme-independent
 *----------------------------------------------------------------------*/
void
ScaleStatRects (float scale)
{
  int i, j;

  ScaleRect (Block_Rect, scale);
  ScaleRect (User_Rect, scale);
  ScaleRect (Classic_User_Rect, scale);
  ScaleRect (Full_User_Rect, scale);
  ScaleRect (Banner_Rect, scale);
  ScaleRect (Portrait_Rect, scale);
  ScaleRect (Cons_Droid_Rect, scale);
  ScaleRect (Menu_Rect, scale);
  ScaleRect (OptionsMenu_Rect, scale);
  ScaleRect (Digit_Rect, scale);
  ScaleRect (Cons_Header_Rect, scale);
  ScaleRect (Cons_Menu_Rect, scale);
  ScaleRect (Cons_Text_Rect, scale);

  ScaleRect (Cons_Menu_Rects[0], scale);
  ScaleRect (Cons_Menu_Rects[1], scale);
  ScaleRect (Cons_Menu_Rects[2], scale);
  ScaleRect (Cons_Menu_Rects[3], scale);

  ScaleRect (ConsMenuItem_Rect, scale);

  ScaleRect (LeftInfo_Rect, scale);
  ScaleRect (RightInfo_Rect, scale);

  for (i=0; i<NUM_FILL_BLOCKS; i++)
    ScaleRect (FillBlocks[i], scale);

  for (i = 0; i < NUM_CAPS_BLOCKS; i++)
    ScaleRect (CapsuleBlocks[i], scale);

  for (j = 0; j < 2*NUM_PHASES; j++)
    for (i = 0; i < TO_BLOCKS; i++)
      ScaleRect (ToGameBlocks[j*TO_BLOCKS+i], scale);

  for (i = 0; i < NUM_GROUND_BLOCKS; i++)
    ScaleRect (ToGroundBlocks[i], scale);

  ScaleRect (ToColumnBlock, scale);
  ScaleRect (ToLeaderBlock, scale);


  for (i=0; i < TO_COLORS; i++)
    {
      ScalePoint (LeftCapsulesStart[i],scale);
      ScalePoint (CurCapsuleStart[i],scale);
      ScalePoint (PlaygroundStart[i],scale);
      ScalePoint (DruidStart[i],scale);
    }
  ScalePoint (TO_LeftGroundStart, scale);
  ScalePoint (TO_ColumnStart, scale);
  ScalePoint (TO_RightGroundStart, scale);
  ScalePoint( TO_LeaderBlockStart, scale);

  ScaleRect (TO_FillBlock, scale);
  ScaleRect (TO_ElementRect, scale);
  ScaleRect (TO_CapsuleRect, scale);
  ScaleRect (TO_LeaderLed, scale);
  ScaleRect (TO_GroundRect, scale);
  ScaleRect (TO_ColumnRect, scale);

  return;

} // ScaleStatRects()


/*----------------------------------------------------------------------
 * toggle windowed/fullscreen modes
 *----------------------------------------------------------------------*/
void
toggle_fullscreen (void)
{
  Uint32 vid_flags = ne_screen->flags;

  //  SDL_WM_ToggleFullScreen (ne_screen);

  if (GameConfig.UseFullscreen)
    vid_flags &= ~SDL_FULLSCREEN;
  else
    vid_flags |= SDL_FULLSCREEN;

  if( !(ne_screen = SDL_SetVideoMode ( Screen_Rect.w, Screen_Rect.h, 0, vid_flags)) )
    {
      DebugPrintf (0, "ERORR occured when trying ot toggle windowed/fullscreen %d x %d video mode.\n",
		   Screen_Rect.w, Screen_Rect.h);
      DebugPrintf (0, "SDL-Error: %s\n", SDL_GetError() );
      Terminate (ERR);
    }

  if ( ne_screen->flags != vid_flags )
    {
      DebugPrintf (0, "WARNING: Failed to toggle windowed/fullscreen mode!\n");
    }
  else
    GameConfig.UseFullscreen = !GameConfig.UseFullscreen;

  return;
}

#define FreeSurfaceArrary(arr) do{   for ( i = 0; i < sizeof(arr)/sizeof(arr[0]); i++) {   SDL_FreeSurface ( arr[i] ); } } while(0)
void
FreeGraphics ( void )
{
  int i;

  // free RWops structures
  for ( i = 0; i < sizeof(packed_portraits)/sizeof(packed_portraits[0]); i ++ ) {
    SDL_RWclose( packed_portraits[i] );
  }

  for ( i = 0; i < sizeof(portrait_raw_mem)/sizeof(portrait_raw_mem[0]); i++) {
    free ( portrait_raw_mem[i] );
  }

  SDL_FreeSurface ( ne_screen );

  FreeSurfaceArrary ( EnemySurfacePointer );
  FreeSurfaceArrary ( InfluencerSurfacePointer );
  FreeSurfaceArrary ( InfluDigitSurfacePointer );
  FreeSurfaceArrary ( EnemyDigitSurfacePointer );
  FreeSurfaceArrary ( Decal_pics );

  int j;
  for ( i = 0; i < NUM_COLORS; i ++ ) {
    for ( j = 0; j < NUM_MAP_BLOCKS; j ++ ) {
      SDL_FreeSurface ( OrigMapBlockSurfacePointer[i][j] );
    }
  }

  SDL_FreeSurface ( BuildBlock );
  SDL_FreeSurface ( banner_pic );
  SDL_FreeSurface ( pic999 );
  // SDL_RWops *packed_portraits[NUM_DROIDS];
  SDL_FreeSurface ( takeover_bg_pic );
  SDL_FreeSurface ( console_pic );
  SDL_FreeSurface ( console_bg_pic1 );
  SDL_FreeSurface ( console_bg_pic2 );

  SDL_FreeSurface ( arrow_up );
  SDL_FreeSurface ( arrow_down );
  SDL_FreeSurface ( arrow_right );
  SDL_FreeSurface ( arrow_left );

  SDL_FreeSurface ( ship_off_pic );
  SDL_FreeSurface ( ship_on_pic );
  SDL_FreeSurface ( progress_meter_pic );
  SDL_FreeSurface ( progress_filler_pic );
  SDL_FreeSurface ( to_blocks );

  // free fonts
  BFont_Info *fonts[] = { Menu_BFont, Para_BFont, Highscore_BFont, Font0_BFont, Font1_BFont, Font2_BFont };
  for ( i = 0; i < sizeof(fonts)/sizeof(fonts[0]); i ++ ) {
    SDL_FreeSurface ( fonts[i]->Surface );
    free ( fonts[i] );
  }

  // free Load_Block()-internal buffer
  Load_Block (NULL, 0, 0, NULL, FREE_ONLY);

  // free cursors
  SDL_FreeCursor ( crosshair_cursor );
  SDL_FreeCursor ( arrow_cursor );

  return;
}

#undef _graphics_c
