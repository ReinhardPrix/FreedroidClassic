/* 
 *
 *  Copyright (c) 2003 Johannes Prix
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

#include "../src/system.h"
#include "../src/defs.h"
#include "../src/getopt.h"
#include "../src/struct.h"
#include "../src/proto.h"

void Terminate (int ExitCode);

iso_image offset_iso_image;
char* background_filename = "gluem_background_fill.png" ;
SDL_Surface *Screen;   // the graphics display 
SDL_Surface *input_surface;   // the graphics display 
// SDL_Surface *output_surface;   // the graphics display 
SDL_Surface *background_surface;   // the graphics display 
char* current_file_series_prefix = NULL ;
char copyright[] = "\nCopyright (C) 2004 Johannes Prix\n\
Gluem comes with NO WARRANTY to the extent permitted by law.\n\
You may redistribute copies of Gluem\n\
under the terms of the GNU General Public License.\n\
For more information about these matters, see the file named COPYING.\n";
FILE *output_file;

char usage_string[] =
  "Usage: gluem    [-v|--version] \n\
                   [-i|--input_file] \n\
                   [-n|--nographicsoutput] (default!)\n\
                   [-g|--graphicsoutput] \n\
                   [-d|--debug=LEVEL]\n\
                   [-x|--offset_x=OFFSET_X]\n\
                   [-y|--offset_y=OFFSET_Y]\n\
\n\
EXAMPLE:  gluem -i my_test_file.png\n\
\n\
Please report bugs by sending e-mail to:\n\n\
freedroid-discussion@lists.sourceforge.net\n\n\
Thanks a lot in advance, the Freedroid dev team.\n\n";

int debug_level = 0 ;
int vid_bpp;

//--------------------
// Now some dummy entries, such that the text_public modules
// will not cause undefined references (in parts of the code,
// that we won't be using anyway...
//
dialogue_option ChatRoster[MAX_DIALOGUE_OPTIONS_IN_ROSTER];
itemspec* ItemMap;
int Number_Of_Item_Types = 0 ; 

// int all_object_directions = 16 ;
// int max_object_phases = 35 ;
int all_object_directions = -1 ;
int max_object_phases = -1 ;
int walk_object_phases = -1 ;
int attack_object_phases = -1 ;
int gethit_object_phases = -1 ;
int death_object_phases = -1 ;
int stand_object_phases = -1 ;
Sint16 cooked_walk_object_phases;
Sint16 cooked_attack_object_phases;
Sint16 cooked_gethit_object_phases;
Sint16 cooked_death_object_phases;
Sint16 cooked_stand_object_phases;

int tux_direction_numbering = FALSE ;
int open_gl_sized_images = FALSE ;

//--------------------
// Another dummy function, such that the (unused) parts of the
// text_public module will not cause undefined references...
//
void
load_item_surfaces_for_item_type ( int item_type )
{
};

void
PutPixel (SDL_Surface * surface, int x, int y, Uint32 pixel)
{
  int bpp = surface->format->BytesPerPixel;
  Uint8 *p;

/*
  if ( use_open_gl )
    {
      if ( surface == Screen ) 
	{
	  PutPixel_open_gl ( x , y , pixel ) ;
	  return;
	}
    }

*/

  //--------------------
  // Here I add a security query against segfaults due to writing
  // perhaps even far outside of the surface pixmap data.
  //
  if ( ( x < 0 ) || ( y < 0 ) || ( x >= surface->w ) || ( y >= surface->h ) ) return;

  /* Here p is the address to the pixel we want to set */
  p = (Uint8 *) surface->pixels + y * surface->pitch + x * bpp;

  switch (bpp)
    {
    case 1:
      *p = pixel;
      break;

    case 2:
      *(Uint16 *) p = pixel;
      break;

    case 3:
      // pixel = pixel & 0x0ffffff ;
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
      *(Uint32 *) p = pixel;
      break;
    }

}; // void PutPixel ( ... )

/* ----------------------------------------------------------------------
 * NOTE:  I THINK THE SURFACE MUST BE LOCKED FOR THIS!
 *
 * ---------------------------------------------------------------------- */
Uint32
GetPixel (SDL_Surface * Surface, Sint32 X, Sint32 Y)
{

  Uint8 *bits;
  Uint32 Bpp;

  //--------------------
  // First some security checks against segfaulting due to
  // coordinates out of bounds...
  //
  if (X < 0)
    {
      DebugPrintf ( 1 , "x too small in GetPixel!" );
      return -1;
    }
  if (X >= Surface->w)
    {
      DebugPrintf ( 1 , "x too big in GetPixel!" );
      return -1;
    }
  if (Y < 0)
    {
      DebugPrintf ( 1 , "y too small in GetPixel!" );
      return -1;
    }
  if (Y >= Surface->h)
    {
      DebugPrintf ( 1 , "y too big in GetPixel!" );
      return -1;
    }

  Bpp = Surface->format->BytesPerPixel;

  bits = ((Uint8 *) Surface->pixels) + Y * Surface->pitch + X * Bpp;

  // Get the pixel
  switch (Bpp)
    {
    case 1:
      return *((Uint8 *) Surface->pixels + Y * Surface->pitch + X);
      break;
    case 2:
      return *((Uint16 *) Surface->pixels + Y * Surface->pitch / 2 + X);
      break;
    case 3:
      {				// Format/endian independent
	Uint8 r, g, b;
	r = *((bits) + Surface->format->Rshift / 8);
	g = *((bits) + Surface->format->Gshift / 8);
	b = *((bits) + Surface->format->Bshift / 8);
	return SDL_MapRGB (Surface->format, r, g, b);
      }
      break;
    case 4:
      return *((Uint32 *) Surface->pixels + Y * Surface->pitch / 4 + X);
      break;
    }

  return -1;
}

/* ----------------------------------------------------------------------
 * There is need to do some padding, cause OpenGL textures need to have
 * a format: width and length both each a power of two.  Therefore some
 * extra alpha to the sides must be inserted.  This is what this function
 * is supposed to do:  manually adding hte proper amount of padding to
 * the surface, so that the dimensions will reach the next biggest power
 * of two in both directions, width and length.
 * ---------------------------------------------------------------------- */
SDL_Surface*
pad_image_for_texture ( SDL_Surface* our_surface ) 
{
    int i ; 
    int x = 1 ;
    int y = 1 ;
    SDL_Surface* padded_surf;
    SDL_Surface* tmp_surf;
    SDL_Rect dest;
    Uint32 target_color;
    
    for ( i = 1 ; i < 100 ; i ++ )
    {
	if ( x >= our_surface -> w )
	    break;
	x = x * 2 ;
    }
    
    for ( i = 1 ; i < 100 ; i ++ )
    {
	if ( y >= our_surface -> h )
	    break;
	y = y * 2 ;
    }
    
    if ( x < 64 )
    {
	DebugPrintf ( 1 , "\nWARNING!  Texture x < 64 encountered.  Raising to 64 x." ) ;
	x = 64 ;
    }
    if ( y < 64 )
    {
	DebugPrintf ( 1 , "\nWARNING!  Texture y < 64 encountered.  Raising to 64 y." ) ;
	y = 64 ;
    }
    
    DebugPrintf ( 1 , "\nPadding image to texture size: final is x=%d, y=%d." , x , y );
    
    padded_surf = SDL_CreateRGBSurface( 0 , x , y , 32, 0x0FF000000 , 0x000FF0000  , 0x00000FF00 , 0x000FF );
    
    //--------------------
    // This might fail, since in gluem, there is no current video mode set...
    // --> disabling it...
    //
    // tmp_surf = SDL_DisplayFormatAlpha ( padded_surf ) ;
    // SDL_FreeSurface ( padded_surf );
    //
    
    tmp_surf = padded_surf ;
    
    SDL_SetAlpha( our_surface , 0 , 0 );
    SDL_SetColorKey( our_surface , 0 , 0x0FF );
    
    dest . x = 0;
    dest . y = y - our_surface -> h ;
    dest . w = our_surface -> w ;
    dest . h = our_surface -> h ;
    
    target_color = SDL_MapRGBA( tmp_surf -> format, 0, 0, 0, 0 );
    for ( x = 0 ; x < tmp_surf -> w ; x ++ )
    {
	for ( y = 0 ; y < tmp_surf -> h ; y ++ )
	{
	    PutPixel ( tmp_surf , x , y , target_color ) ;
	}
    }
    
    SDL_BlitSurface ( our_surface, NULL , tmp_surf , & dest );
    
    return ( tmp_surf );
    
}; // SDL_Surface* pad_image_for_texture ( SDL_Surface* our_surface ) 


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
  temp = pixel&fmt->Gmask;  /* Isolate green component */
  temp = temp>>fmt->Gshift; /* Shift it down to 8-bit */
  temp = temp<<fmt->Gloss;  /* Expand to a full 8-bit number */
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
 *
 *
 * ---------------------------------------------------------------------- */
void
MyWait ( float wait_time ) 
{
  long start_ticks = SDL_GetTicks();

  while ( ( SDL_GetTicks() - start_ticks ) < 1000.0 * wait_time );

}; // void MyWait ( float wait_time )

/* ----------------------------------------------------------------------
 * This function is used for terminating freedroid.  It will close
 * the SDL submodules and exit.
 * ---------------------------------------------------------------------- */
void
Terminate (int ExitCode)
{
  DebugPrintf (2, "\nvoid Terminate(int ExitStatus) was called....");
  printf("\n----------------------------------------------------------------------");
  printf("\nTermination of Gluem initiated...");

  printf("Thank you for using the FreedroidRPG Gluem Tool.\n\n");
  SDL_Quit();
  exit (ExitCode);
  return;
}; // void Terminate ( int ExitCode )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
int
probe_filename ( char* current_file_series_prefix )
{
    char temp_filename[10000];
    FILE* temp_file;
    int i = 0 ;

    while ( 1 ) 
    {
	sprintf ( temp_filename , "./%s_%02d_%04d.png" , current_file_series_prefix , 0 , i + 1 );
	
	//--------------------
	// Let's see if we can find an offset file...
	//
	if ( ( temp_file = fopen ( temp_filename , "rb") ) == NULL )
	{
	    // max_object_phases = i ;
	    DebugPrintf ( -4 , "\nThis file seems not to be there any more: %s." , temp_filename );
	    DebugPrintf ( -4 , "\nReturning phase amount: %d." , i );
	    // DebugPrintf ( -4 , "\nThat means the final max_object_phases=%d." , max_object_phases );
	    return ( i ) ;
	    break; 
	}
	else
	{
	    if ( fclose ( temp_file ) == EOF)
	    {
		fprintf( stderr, "\n\noffset_file_name: '%s'\n" , temp_filename );
		DebugPrintf ( -4 , "\nClosing the tested file failed!! ERROR!! STRANGE!!");
	    }
	    else
	    {
		DebugPrintf ( 1 , "\nThe tested file seems to be there at least.....");
		DebugPrintf ( 1 , "\nClosing again...");
	    }
	}
	i++ ;
    }
}; // int probe_filename ( char* current_file_series_prefix )

/* ----------------------------------------------------------------------
 * Maybe the user did not bother with specifying the right amount of 
 * phases to use for this animation.  Then the gluem tool is supposed to
 * find out the correct number completely on it's own.
 * ---------------------------------------------------------------------- */
void
auto_probe_max_object_phases ( void )
{
    
    DebugPrintf ( -4 , "\nNow auto-probing max_object_phases..." );
    
    if ( strcmp ( current_file_series_prefix , "AUTO" ) )
    {
	max_object_phases = probe_filename ( current_file_series_prefix ) ;
    }
    else
    {
	walk_object_phases = probe_filename ( "walk" ) ;
	attack_object_phases = probe_filename ( "attack" ) ;
	gethit_object_phases = probe_filename ( "gethit" ) ;
	death_object_phases = probe_filename ( "death" ) ;
	stand_object_phases = probe_filename ( "stand" ) ;

	max_object_phases = walk_object_phases + attack_object_phases + gethit_object_phases + death_object_phases + stand_object_phases ;
	if ( walk_object_phases == 0 ) max_object_phases ++ ;
	if ( attack_object_phases == 0 ) max_object_phases ++ ;
	if ( gethit_object_phases == 0 ) max_object_phases ++ ;
	if ( death_object_phases == 0 ) max_object_phases ++ ;
	if ( stand_object_phases == 0 ) max_object_phases ++ ;
    }

    cooked_walk_object_phases   = walk_object_phases   ;
    cooked_attack_object_phases = attack_object_phases ; 
    cooked_gethit_object_phases = gethit_object_phases ; 
    cooked_death_object_phases  = death_object_phases  ; 
    cooked_stand_object_phases  = stand_object_phases  ; 
    
    if ( ! cooked_walk_object_phases ) cooked_walk_object_phases = 1 ;
    if ( ! cooked_attack_object_phases ) cooked_attack_object_phases = 1 ;
    if ( ! cooked_gethit_object_phases ) cooked_gethit_object_phases = 1 ;
    if ( ! cooked_death_object_phases ) cooked_death_object_phases = 1 ;
    if ( ! cooked_stand_object_phases ) cooked_stand_object_phases = 1 ;

}; // void auto_probe_max_object_phases ( void )
    
/* -----------------------------------------------------------------
 * parse command line arguments and set global switches 
 * exit on error, so we don't need to return success status
 * -----------------------------------------------------------------*/
void
ParseCommandLine (int argc, char *const argv[])
{
    int c;

    static struct option long_options[] = 
	{
	    { "version",          0, 0,  'v' },
	    { "help",             0, 0,  'h' },
	    { "input_file",       required_argument , 0,  'i' },
	    { "max_object_phases",       required_argument , 0,  'p' },
	    { "all_object_directions",       required_argument , 0,  'd' },
	    { "tux_16_direction_numbering",      0 , 0,  't' },
	    { "open_gl_sized_images",      0 , 0,  'o' },
	    // { "debug",            2, 0,  'd' },
	    {  0,                 0, 0,   0  }
	};
    
    while (1)
    {
	c = getopt_long (argc, argv, "tovi:h?d:p:", long_options, NULL);
	if (c == -1)
	    break;
	
	switch (c)
	{
	    // version statement -v or --version
	    // following gnu-coding standards for command line interfaces 
	    case 'v':
		DebugPrintf ( 0 , "\nFreedroid Gluem Tool, Version 1.1.\n" );
		DebugPrintf ( 0 , copyright );
		exit (0);
		break;
		
	    case 'h':
	    case '?':
		printf (usage_string);
		exit (0);
		break;
		
	    case 't':
		tux_direction_numbering = TRUE ;
		break;
		
	    case 'o':
		open_gl_sized_images = TRUE ;
		break;
		
	    case 'i':
		if ( optarg )
		{
		    current_file_series_prefix = optarg;
		    DebugPrintf ( 1 , "\nInput file name set to : %s " , current_file_series_prefix );
		}
		else
		{
		    printf ("\nERROR! -i specified, but no input file given... Exiting.\n\n" );
		    exit ( 0 );
		}
		break;
		
	    case 'p':
		if ( optarg )
		{
		    sscanf ( optarg , "%d" , &max_object_phases ) ;
		    DebugPrintf ( 1 , "\nmax_object_phases set to: %d " , max_object_phases );
		}
		else
		{
		    printf ("\nERROR! -p specified, but no phases number given... Exiting.\n\n" );
		    exit ( 0 );
		}
		break;
		
	    case 'd':
		if ( optarg )
		{
		    sscanf ( optarg , "%d" , &all_object_directions ) ;
		    DebugPrintf ( 1 , "\nall_object_directions set to: %d " , all_object_directions );
		}
		else
		{
		    printf ("\nERROR! -d specified, but no directions number given... Exiting.\n\n" );
		    exit ( 0 );
		}
		break;
		
	    default:
		printf ("\nOption %c not implemented yet! Ignored.", c);
		break;
	} // switch(c) 
    }  // while(1) 
    
    if ( current_file_series_prefix == NULL )
    {
	DebugPrintf ( 0 , "\nERROR:  No current_file_series_prefix specified... Terminating... " );
	Terminate ( ERR );
    }
    
    if ( current_file_series_prefix == NULL )
    {
	DebugPrintf ( 0 , "\nERROR:  No current_file_series_prefix specified... Terminating... " );
	Terminate ( ERR );
    }
    
    if ( all_object_directions == (-1) )
    {
	DebugPrintf ( 0 , "\nERROR:  No all_object_directions specified... Terminating... " );
	Terminate ( ERR );
    }
    
    if ( max_object_phases == (-1) )
    {
	DebugPrintf ( 0 , "\nERROR:  No max_object_phases... Terminating... " );
	Terminate ( ERR );
    }

    if ( tux_direction_numbering )
	DebugPrintf ( 0 , "\nTux direction numbering ENABLED." );
    else
	DebugPrintf ( 0 , "\nTux direction numbering DISABLED." );

    if ( ( 16 % all_object_directions ) && tux_direction_numbering )
    {
	DebugPrintf ( 0 , "\nTux direction numbering enabled but directions don't divide 16!\nERROR!-->terminating.\n\n" );
	exit ( -1 ) ;
    }

    if ( open_gl_sized_images )
	DebugPrintf ( 0 , "\nOpen_gl_sized_images ENABLED.\n" );
    else
	DebugPrintf ( 0 , "\nOpen_gl_sized_images DISABLED.\n" );

    if ( max_object_phases == 0 )
    {
	DebugPrintf ( 0 , "\nZero value for max_object_phases detected --> will use auto-probing now...\n" );	
	auto_probe_max_object_phases();
    }

}; // void ParseCommandLine(...)

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
    Uint32 flags;  
    
    //--------------------
    // Initialize the SDL library 
    //
    if ( SDL_Init ( SDL_INIT_VIDEO ) == -1 ) 
    {
	fprintf( stderr , "Couldn't initialize SDL: %s\n" , SDL_GetError ( ) );
	Terminate ( ERR ) ;
    } 
    else
	DebugPrintf ( 1 , "\nSDL Video initialisation successful.\n" );
    
    //--------------------
    // Now SDL_TIMER is initialized here:
    //
    if ( SDL_InitSubSystem ( SDL_INIT_TIMER ) == -1 ) 
    {
	fprintf(stderr, "Couldn't initialize SDL: %s\n",SDL_GetError());
	Terminate(ERR);
    } else
	DebugPrintf(1, "\nSDL Timer initialisation successful.\n");
    
    //--------------------
    // We request automatic cleanup on program exit 
    //
    atexit (SDL_Quit);

    vid_info = SDL_GetVideoInfo (); /* just curious */
    SDL_VideoDriverName (vid_driver, 80);
  
    flags = SDL_SWSURFACE | SDL_HWPALETTE ;
    // if (fullscreen_on) flags |= SDL_FULLSCREEN;
    flags &= !SDL_FULLSCREEN;

    vid_modes = SDL_ListModes (NULL, SDL_SWSURFACE);

    /* 
     * currently only the simple 320x200 mode is supported for 
     * simplicity, as all our graphics are in this format
     * once this is up and running, we'll provide others modes
     * as well.
     */
    vid_bpp = 16; /* start with the simplest */
    
}; // InitVideo () 

/* ----------------------------------------------------------------------
 * This function was copied from the core FreedroidRPG modules.
 * ---------------------------------------------------------------------- */
void
get_offset_for_iso_image_from_file_and_path ( char* fpath , iso_image* our_iso_image )
{
  char offset_file_name[10000];
  FILE *OffsetFile; 
  char* offset_data;
  //--------------------
  // Now we try to load the associated offset file, that we'll be needing
  // in order to properly fine-position the image later when blitting is to
  // a map location.
  //
  strcpy ( offset_file_name , fpath );
  offset_file_name [ strlen ( offset_file_name ) - 4 ] = 0 ;
  strcat ( offset_file_name , ".offset" );

  //--------------------
  // Let's see if we can find an offset file...
  //
  if ( ( OffsetFile = fopen ( offset_file_name , "rb") ) == NULL )
    {
      fprintf (stderr, "\nObtaining offset failed with file name '%s'." , offset_file_name );
      GiveStandardErrorMessage ( __FUNCTION__ , "\
Freedroid was unable to open a given offset file for an isometric image.\n\
Since the offset could not be obtained from the offset file, some default\n\
values will be used instead.  This can lead to minor positioning pertubations\n\
in graphics displayed, but FreedroidRPG will continue to work.",
				 NO_NEED_TO_INFORM, IS_WARNING_ONLY );
      our_iso_image -> offset_x = - INITIAL_BLOCK_WIDTH/2 ;
      our_iso_image -> offset_y = - INITIAL_BLOCK_HEIGHT/2 ;
      return ;
    }
  else
    {
      DebugPrintf ( 1 , "\nThe offset file seems to be there at least.....");

      if ( fclose ( OffsetFile ) == EOF)
	{
	  fprintf( stderr, "\n\noffset_file_name: '%s'\n" , offset_file_name );
	  GiveStandardErrorMessage ( __FUNCTION__ , "\
Freedroid was unable to close an offset file.\nThis is a very strange occasion!",
				     PLEASE_INFORM, IS_FATAL );
	}
      else
	{
	  DebugPrintf( 1 , "\nThe offset file, that is there, was closed again successfully...");
	}

    }

  //--------------------
  // So at this point we can be certain, that the offset file is there.
  // That means, that we can now use the (otherwise terminating) read-and-malloc-...
  // functions.
  //
  offset_data = ReadAndMallocAndTerminateFile( offset_file_name , END_OF_OFFSET_FILE_STRING ) ;

  ReadValueFromString( offset_data ,  OFFSET_FILE_OFFSETX_STRING , "%d" , 
		       & ( our_iso_image -> offset_x ) , offset_data + 1000 );

  ReadValueFromString( offset_data ,  OFFSET_FILE_OFFSETY_STRING , "%d" , 
		       & ( our_iso_image -> offset_y ) , offset_data + 1000 );
  free ( offset_data );

}; // void get_offset_for_iso_image_from_file_and_path ( fpath , our_iso_image )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
add_loaded_image_to_output_file ( void )
{
    int x;
    int y;

    Sint16 img_xlen;
    Sint16 img_ylen;
    Sint16 img_x_offs;
    Sint16 img_y_offs;
    Sint16 orig_img_xlen;
    Sint16 orig_img_ylen;
    unsigned char r_comp, g_comp, b_comp, a_comp;

    img_xlen = input_surface -> w ;
    img_ylen = input_surface -> h ;
    img_x_offs = offset_iso_image . offset_x ;
    img_y_offs = offset_iso_image . offset_y ;
    orig_img_xlen = offset_iso_image . original_image_width ;
    orig_img_ylen = offset_iso_image . original_image_height ;

    fwrite ( & ( img_xlen ) , 1 , sizeof ( Sint16 ) , output_file );
    fwrite ( & ( img_ylen ) , 1 , sizeof ( Sint16 ) , output_file );
    fwrite ( & ( img_x_offs ) , 1 , sizeof ( Sint16 ) , output_file );
    fwrite ( & ( img_y_offs ) , 1 , sizeof ( Sint16 ) , output_file );

    //--------------------
    // If we've padded the image, we need to write down the original
    // image height and width in the image collection file too...
    //
    if ( open_gl_sized_images )
    {
	fwrite ( & ( orig_img_xlen ) , 1 , sizeof ( Sint16 ) , output_file );
	fwrite ( & ( orig_img_ylen ) , 1 , sizeof ( Sint16 ) , output_file );
    }

    //--------------------
    // Now we can save the image data...
    //
    for ( y = 0 ; y < input_surface -> h ; y ++ )
    {
	for ( x = 0 ; x < input_surface -> w ; x ++ )
	{
	    a_comp = GetAlphaComponent ( input_surface , x , input_surface -> h - y - 1 ) ;
	    b_comp = GetBlueComponent  ( input_surface , x , input_surface -> h - y - 1 ) ;
	    r_comp = GetRedComponent   ( input_surface , x , input_surface -> h - y - 1 ) ;
	    g_comp = GetGreenComponent ( input_surface , x , input_surface -> h - y - 1 ) ;

	    fwrite ( & ( b_comp ) , 1 , sizeof ( b_comp ) , output_file );
	    fwrite ( & ( g_comp ) , 1 , sizeof ( g_comp ) , output_file );
	    fwrite ( & ( r_comp ) , 1 , sizeof ( r_comp ) , output_file );
	    fwrite ( & ( a_comp ) , 1 , sizeof ( a_comp ) , output_file );
	}
    } 
    
}; // void add_loaded_image_to_output_file ( void )

/* ----------------------------------------------------------------------
 * This function flips a given SDL_Surface around the x-axis, i.e. up-down.
 * 
 * This is particularly nescessary, since OpenGL has a different native
 * coordinate system than SDL and therefore images often appear flipped
 * around if one doesn't counter this effect with OpenGL by flipping the
 * images just once more in the same fashion.  That is what this function
 * does.
 * ---------------------------------------------------------------------- */
void
flip_image_horizontally ( SDL_Surface* tmp1 ) 
{
  int x , y ;
  Uint32 temp;

  for ( y = 0 ; y < ( tmp1 -> h ) / 2 ; y ++ )
    {
      for ( x = 0 ; x < (tmp1 -> w ) ; x ++ )
	{
	  temp = GetPixel ( tmp1 , x , y ) ;
	  PutPixel ( tmp1 , x , y , GetPixel ( tmp1 , x , ( tmp1 -> h - y - 1 ) ) ) ;
	  PutPixel ( tmp1 , x , ( tmp1 -> h - y - 1 ) , temp ) ;
	}
    }
}; // void flip_image_horizontally ( SDL_Surface* tmp1 ) 


/* -----------------------------------------------------------------
 * This function is the heart of the game.  It contains the main
 * game loop.
 * ----------------------------------------------------------------- */
int
main (int argc, char *const argv[])
{
    char current_filename[10000];
    int i, j;
    char output_file_filename[10000]="/home/johannes/FreeDroid/gluem/test.tux_image_archive" ;
    char* local_prefix;
    int local_index;

#define MAIN_DEBUG 1 

    DebugPrintf ( MAIN_DEBUG , "\nFreedroidRPG 'Gluem' Tool, starting to read command line....\n" );

    ParseCommandLine (argc, argv); 

    DebugPrintf ( MAIN_DEBUG , "\nFreedroidRPG 'Gluem' Tool, initializing video....\n" );

    InitVideo () ;

    DebugPrintf ( MAIN_DEBUG , "\nFreedroidRPG 'Gluem' Tool, now loading input files...\n" );

    //--------------------
    // Now that we know which filename to use, we can open the save file for writing
    //
    sprintf ( output_file_filename , "./%s.tux_image_archive" , current_file_series_prefix );
    if ( ( output_file = fopen ( output_file_filename , "wb" ) ) == NULL ) 
    {
	printf( "\n\nError opening save game file for writing...\n\nTerminating...\n\n" );
	Terminate( ERR );
	// return ERR;
    }
    else
	DebugPrintf( 0 , "\nOpening output file successful...\n" );

    //--------------------
    // Now a new feature:  We write down some header information into
    // the newly created file.  This will help the main program to 
    // distinguish the image collection format and add security against
    // various errors.
    //
    if ( ! strncmp ( "iso" , current_file_series_prefix , 3 ) )
    {
	fwrite ( "tuxX" , 1 , strlen ( "tuxX" ) , output_file );
    }
    else
    {
	fwrite ( "eneX" , 1 , strlen ( "eneX" ) , output_file );
    }
    if ( open_gl_sized_images )
    {
	fwrite ( "oglX" , 1 , strlen ( "oglX" ) , output_file );
    }
    else
    {
	fwrite ( "sdlX" , 1 , strlen ( "sdlX" ) , output_file );
    }

    //--------------------
    // At this point (when reading the file in the game) the file format
    // is clear, so we may do some case-dependent writing into the post-header
    // area here already without getting into trouble...
    //
    // We write out the number of phases in each cycle, i.e. the number of
    // images in walk, attack, gethit, death and stand cycle.
    //
    fwrite ( & ( cooked_walk_object_phases ) , 1 , sizeof ( Sint16 ) , output_file );
    fwrite ( & ( cooked_attack_object_phases ) , 1 , sizeof ( Sint16 ) , output_file );
    fwrite ( & ( cooked_gethit_object_phases ) , 1 , sizeof ( Sint16 ) , output_file );
    fwrite ( & ( cooked_death_object_phases ) , 1 , sizeof ( Sint16 ) , output_file );
    fwrite ( & ( cooked_stand_object_phases ) , 1 , sizeof ( Sint16 ) , output_file );

    //--------------------
    // Now we can start to write out the raw image and offset information in one
    // long saucage...
    //
    for ( j = 0 ; j < all_object_directions ; j ++ )
    {
	for ( i = 0 ; i < max_object_phases ; i ++ )
	{

	    if ( strcmp ( current_file_series_prefix , "AUTO" ) )
	    {
		local_prefix = current_file_series_prefix ;
		local_index = i + 1 ;
	    }
	    else 
	    {
		if ( i < cooked_walk_object_phases )
		{
		    //--------------------
		    // Walk cycle must always be there and have at least 1 image!
		    // That's an assumption, because this is the fallback image to
		    // use if other cycles don't yet exist.
		    //
		    local_prefix = "walk" ;
		    local_index = i + 1 ;
		}
		else if ( i < cooked_walk_object_phases + cooked_attack_object_phases )
		{
		    if ( attack_object_phases == 0 )
		    {
			local_prefix = "walk" ;
			local_index = 1 ;
		    }
		    else
		    {
			local_prefix = "attack" ;
			local_index = i - cooked_walk_object_phases + 1 ;
		    }
		}
		else if ( i < cooked_walk_object_phases + cooked_attack_object_phases + cooked_gethit_object_phases)
		{
		    if ( gethit_object_phases == 0 )
		    {
			local_prefix = "walk" ;
			local_index = 1 ;
		    }
		    else
		    {
			local_prefix = "gethit" ;
			local_index = i - cooked_walk_object_phases - cooked_attack_object_phases + 1 ;
		    }
		}
		else if ( i < cooked_walk_object_phases + cooked_attack_object_phases + cooked_gethit_object_phases + cooked_death_object_phases )
		{
		    if ( death_object_phases == 0 )
		    {
			local_prefix = "../default_dead_body" ;
			local_index = 1 ;
		    }
		    else
		    {
			local_prefix = "death" ;
			local_index = i - cooked_walk_object_phases - cooked_attack_object_phases - cooked_gethit_object_phases + 1 ;
		    }
		}
		else if ( i < cooked_walk_object_phases + cooked_attack_object_phases + cooked_gethit_object_phases + cooked_death_object_phases + cooked_stand_object_phases )
		{
		    if ( stand_object_phases == 0 )
		    {
			local_prefix = "walk" ;
			local_index = 1 ;
		    }
		    else
		    {
			local_prefix = "stand" ;
			local_index = i - cooked_walk_object_phases - cooked_attack_object_phases - cooked_death_object_phases - cooked_gethit_object_phases + 1 ;
		    }
		}
		else
		{
		    local_prefix = "UNHANDLED_CASE" ;
		    local_index = 999 ;
		    GiveStandardErrorMessage ( __FUNCTION__ , "\
Unhandled case in the AUTO image prefix code encountered!",
					       PLEASE_INFORM, IS_FATAL );
		}

	    }

	    //--------------------
	    // Maybe this is a bot using the tux direction numbering convention
	    // for the files and file names on disk.  Then we need to take care
	    // of this...  --> we just advance the counter a bit in this case.
	    //
	    if ( tux_direction_numbering && ( all_object_directions < 16 ) )
	    {
		sprintf ( current_filename , "./%s_%02d_%04d.png" , local_prefix , j * ( 16 / all_object_directions ) , local_index );
	    }
	    else
		sprintf ( current_filename , "./%s_%02d_%04d.png" , local_prefix , j , local_index );

	    input_surface = IMG_Load ( current_filename ) ;
	    if ( input_surface == NULL )
	    {
		DebugPrintf ( 0 , "\n\nERROR:  Unable to load input file... " );
		DebugPrintf ( 0 , "\nFile name was : %s . " , current_filename );
		Terminate ( ERR );
	    }
	    else
	    {
		DebugPrintf ( 0 , "\nSuccessfully loaded input image %s." , current_filename );
	    }

	    //--------------------
	    // If this is supposed to become an open_gl prepadded image, we need
	    // to do the padding here...
	    //
	    if ( open_gl_sized_images )
	    {
		offset_iso_image . original_image_width = input_surface -> w ;
		offset_iso_image . original_image_height = input_surface -> h ;

		flip_image_horizontally ( input_surface ) ;
		input_surface = pad_image_for_texture ( input_surface ) ;
		flip_image_horizontally ( input_surface ) ;
		DebugPrintf ( 0 , "\nImage padded to match powers of two." );
	    }
	    
	    //--------------------
	    // Now that the image has been loaded successfully, we can start 
	    // to load the offset for this image file.
	    //
	    get_offset_for_iso_image_from_file_and_path ( current_filename , &offset_iso_image );
	    
	    add_loaded_image_to_output_file ( ) ;
	    
	    // DebugPrintf ( 0 , "\nSuccessfully loaded offset file for image %s." , current_filename );
	    
	}

    }

    //--------------------
    // The writing is now done, so we can close the output file now.
    // 
    if ( fclose ( output_file ) == EOF )
    {
	printf("\n\nClosing of output file failed...\n\nTerminating\n\n");
	Terminate(ERR);
	// return ERR;
    }
    else
	DebugPrintf( 0 , "\nSaving of output file successful.\n" );
    
    DebugPrintf ( MAIN_DEBUG , "\nGluem finished.  Exiting...\n\n" );
    
    // Terminate ( OK );
    
    DebugPrintf ( 0 , "\n" );
    
    return ( 0 );
    
}; // int main ( ... )
