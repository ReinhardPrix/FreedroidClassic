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
 * Maybe the user did not bother with specifying the right amount of 
 * phases to use for this animation.  Then the gluem tool is supposed to
 * find out the correct number completely on it's own.
 * ---------------------------------------------------------------------- */
void
auto_probe_max_object_phases ( void )
{
    char temp_filename[10000];
    FILE* temp_file;
    int i = 0 ;
    
    DebugPrintf ( -4 , "\nNow auto-probing max_object_phases..." );
    
    while ( 1 ) 
    {
	sprintf ( temp_filename , "./%s_%02d_%04d.png" , current_file_series_prefix , 0 , i + 1 );
	
	//--------------------
	// Let's see if we can find an offset file...
	//
	if ( ( temp_file = fopen ( temp_filename , "rb") ) == NULL )
	{
	    max_object_phases = i ;
	    DebugPrintf ( -4 , "\nThis file seems not to be there any more: %s." , temp_filename );
	    DebugPrintf ( -4 , "\nThat means the final max_object_phases=%d." , max_object_phases );
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
      GiveStandardErrorMessage ( "get_offset_for_iso_image_from_file_and_path(...)" , "\
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
	  GiveStandardErrorMessage ( "get_offset_for_iso_image_from_file_and_path(...)" , "\
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
    unsigned char r_comp, g_comp, b_comp, a_comp;

    img_xlen = input_surface -> w ;
    img_ylen = input_surface -> h ;
    img_x_offs = offset_iso_image . offset_x ;
    img_y_offs = offset_iso_image . offset_y ;

    fwrite ( & ( img_xlen ) , 1 , sizeof ( Sint16 ) , output_file );
    fwrite ( & ( img_ylen ) , 1 , sizeof ( Sint16 ) , output_file );
    fwrite ( & ( img_x_offs ) , 1 , sizeof ( Sint16 ) , output_file );
    fwrite ( & ( img_y_offs ) , 1 , sizeof ( Sint16 ) , output_file );

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

    for ( j = 0 ; j < all_object_directions ; j ++ )
    {
	for ( i = 0 ; i < max_object_phases ; i ++ )
	{
	    //--------------------
	    // Maybe this is a bot using the tux direction numbering convention
	    // for the files and file names on disk.  Then we need to take care
	    // of this...  --> we just advance the counter a bit in this case.
	    //
	    if ( tux_direction_numbering && ( all_object_directions < 16 ) )
	    {
		sprintf ( current_filename , "./%s_%02d_%04d.png" , current_file_series_prefix , j * ( 16 / all_object_directions ) , i + 1 );
	    }
	    else
		sprintf ( current_filename , "./%s_%02d_%04d.png" , current_file_series_prefix , j , i + 1 );

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
