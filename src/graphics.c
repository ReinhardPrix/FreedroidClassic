/* 
 *
 *   Copyright (c) 1994, 2002, 2003 Johannes Prix
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
#include "colodefs.h"
#include "SDL_rotozoom.h"

void swap_red_and_blue_for_open_gl ( SDL_Surface* FullView );

/* XPM */
static const char *crosshair_mouse_cursor[] = {
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
static const char *arrow_mouse_cursor[] = {
  /* width height num_colors chars_per_pixel */
  "    32    32        3            1",
  /* colors */
  "X c #000000",
  ". c #ffffff",
  "  c None",
  /* pixels */
  "                                ",
  "                                ",
  "   XXXXXXXXXXXXXXXX             ",
  "   X..............X             ",
  "   X..............X             ",
  "   X..............X             ",
  "   X...XXXXXXXXXXXX             ",
  "   X...X                        ",
  "   X...X XXXX                   ",
  "   X...X X...X                  ",
  "   X...X X....X                 ",
  "   X...X X.....X                ",
  "   X...X  X.....X               ",
  "   X...X   X.....X              ",
  "   X...X    X.....X             ",
  "   X...X     X.....X            ",
  "   XXXXX      X....X            ",
  "               X...X            ",
  "                XXX             ",
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
    return SDL_CreateCursor( data , mask , 32 , 32 , hot_x , hot_y );
}; // static SDL_Cursor *init_system_cursor(const char *image[])

/* ----------------------------------------------------------------------
 * The shape of the mouse cursor might be changed in the course of the
 * game.  Sometimes a crosshair might be good.  Sometimes it's rather an
 * arrow, that is suitable for the mouse cursor shape.  This function
 * should change the mouse cursor shape accordingly...
 * ---------------------------------------------------------------------- */
void
set_mouse_cursor_to_shape ( int given_shape ) 
{
    static void* current_cursor = NULL ;

    if ( current_cursor != NULL )
	SDL_FreeCursor ( current_cursor );

    switch ( given_shape )
    {
	case MOUSE_CURSOR_CROSSHAIR_SHAPE:
	    // SDL_SetCursor( init_system_cursor( crosshair_mouse_cursor ) );
	    // current_mouse_cursor_shape = MOUSE_CURSOR_CROSSHAIR_SHAPE ;
	    current_cursor = init_system_cursor( arrow_mouse_cursor ) ;
	    SDL_SetCursor( current_cursor ) ;
	    current_mouse_cursor_shape = MOUSE_CURSOR_ARROW_SHAPE ;
	    break;
	case MOUSE_CURSOR_ARROW_SHAPE:
	    current_cursor = init_system_cursor( arrow_mouse_cursor ) ;
	    SDL_SetCursor( current_cursor ) ;
	    current_mouse_cursor_shape = MOUSE_CURSOR_ARROW_SHAPE ;
	    break;
	default:
	    GiveStandardErrorMessage ( __FUNCTION__  , "\
ERROR: Unhandled mouse cursor shape type received.",
				       PLEASE_INFORM, IS_FATAL );
	    SDL_SetCursor( init_system_cursor( crosshair_mouse_cursor ) );
	    current_mouse_cursor_shape = MOUSE_CURSOR_CROSSHAIR_SHAPE ;
	    break;
    }
}; // void set_mouse_cursor_to_shape ( int given_shape ) 

/* ----------------------------------------------------------------------
 * Because we're using our own mouse cursor, it might be important to
 * make sure, that the system mouse cursor is hidden, so it will not show
 * and we can draw our own mouse cursor image right at that place safely.
 * ---------------------------------------------------------------------- */
void
make_sure_system_mouse_cursor_is_turned_off ( void )
{

    if ( SDL_ShowCursor( SDL_QUERY ) == SDL_ENABLE )
    {
	DebugPrintf ( 1 , "\n%s(): now hiding mouse cursor again, because it was shown." , __FUNCTION__ );
	SDL_ShowCursor( SDL_DISABLE );
    }

}; // void make_sure_system_mouse_cursor_is_turned_off ( void )

/* ----------------------------------------------------------------------
 * Because we're using our own mouse cursor, it might be important to
 * make sure, that the system mouse cursor is hidden, so it will not show
 * and we can draw our own mouse cursor image right at that place safely.
 * ---------------------------------------------------------------------- */
void
make_sure_system_mouse_cursor_is_turned_on ( void )
{

    if ( SDL_ShowCursor( SDL_QUERY ) == SDL_DISABLE )
    {
	DebugPrintf ( 1 , "\n%s(): now showing mouse cursor again, because it was hidden." , __FUNCTION__ );
	SDL_ShowCursor( SDL_ENABLE );
    }

}; // void make_sure_system_mouse_cursor_is_turned_off ( void )

/* ----------------------------------------------------------------------
 * When the system mouse cursor is now shown, we need to blit our own 
 * mouse cursor instead.  That is good, because that we we can even use
 * many colors and maybe also some small animation in a controlled way.
 * ---------------------------------------------------------------------- */
void
blit_our_own_mouse_cursor ( void )
{
    static int first_call = TRUE ;
    int i;
    static iso_image mouse_cursors [ 16 ] ;
    char constructed_filename[2000];
    char* fpath;
    int cursor_index = (-1) ;

    //--------------------
    // On the first function call ever, we load the surfaces for the
    // flags into memory.
    //
    if ( first_call )
    {
	for ( i = 0 ; i < 12 ; i ++ )
	{
	    sprintf ( constructed_filename , "mouse_cursor_%04d.png" , i );
	    fpath = find_file ( constructed_filename , GRAPHICS_DIR, FALSE );
	    get_iso_image_from_file_and_path ( fpath , & ( mouse_cursors [ i ] ) , FALSE ) ;
	    if ( mouse_cursors [ i ] . surface == NULL ) 
	    {
		fprintf ( stderr , "\nFull path used: %s." , fpath );
		GiveStandardErrorMessage ( __FUNCTION__ , "\
Error loading flag image.",
					   PLEASE_INFORM, IS_FATAL );
	    }
	    if ( use_open_gl )
	    {
		DebugPrintf ( 1 , "\n%s(): Texture made from mouse cursor surface..." );
		make_texture_out_of_surface ( & ( mouse_cursors [ i ] ) ) ;
	    }
	}

	first_call = FALSE ;
    }

    switch ( global_ingame_mode )
    {
	case GLOBAL_INGAME_MODE_SCROLL_UP :
	    cursor_index = 4 ;
	    break;
	case GLOBAL_INGAME_MODE_SCROLL_DOWN :
	    cursor_index = 5 ;
	    break;
	case GLOBAL_INGAME_MODE_IDENTIFY :
	    cursor_index = 1 ;
	    break;
	case GLOBAL_INGAME_MODE_NORMAL:
	    cursor_index = 0 ;
	    break;
	case GLOBAL_INGAME_MODE_EXAMINE:
	    cursor_index = 2 ;
	    break;
	case GLOBAL_INGAME_MODE_LOOT:
	    cursor_index = 3 ;
	    break;
	case GLOBAL_INGAME_MODE_REPAIR:
	    cursor_index = 6 ;
	    break;
	case GLOBAL_INGAME_MODE_UNLOCK:
	    cursor_index = 7 ;
	    break;
	case GLOBAL_INGAME_MODE_TALK:
	    cursor_index = 8 ;
	    break;
	case GLOBAL_INGAME_MODE_FIRST_AID:
	    cursor_index = 9 ;
	    break;
	case GLOBAL_INGAME_MODE_ATTACK:
	    cursor_index = 10 ;
	    break;
	case GLOBAL_INGAME_MODE_PICKPOCKET:
	    cursor_index = 11 ;
	    break;
	default:
	    DebugPrintf ( -4 , "\n%s(): global_ingame_mode: %d." , __FUNCTION__ , 
			  global_ingame_mode );
	    GiveStandardErrorMessage ( __FUNCTION__  , 
				       "Illegal global ingame mode encountered!" ,
				       PLEASE_INFORM, IS_FATAL );
	    break;
    }

    //--------------------
    // We can now blit the mouse cursor...
    //
    if ( use_open_gl )
    {
	blit_open_gl_texture_to_screen_position ( mouse_cursors [ cursor_index ] , 
						  GetMousePos_x () , GetMousePos_y () , TRUE );
    }
    else
    {
	blit_iso_image_to_screen_position ( mouse_cursors [ cursor_index ] , 
					    GetMousePos_x () , GetMousePos_y () );
    }

}; // void blit_our_own_mouse_cursor ( void )

/* ----------------------------------------------------------------------
 * When the system mouse cursor is now shown, we need to blit our own 
 * mouse cursor instead.  That is good, because that we we can even use
 * many colors and maybe also some small animation in a controlled way.
 * ---------------------------------------------------------------------- */
void
blit_mouse_cursor_corona ( void )
{
    static int first_call = TRUE ;
    int i;
    static iso_image mouse_cursor_coronas [ 16 ] ;
    char constructed_filename[2000];
    char* fpath;
    moderately_finepoint offset_vector;
    int our_obstacle_index ;
    int our_enemy_index;

    return ;

    //--------------------
    // In case of scroll modes, we don't have to blit any corona...
    //
    if ( ( global_ingame_mode == GLOBAL_INGAME_MODE_SCROLL_UP ) ||
	 ( global_ingame_mode == GLOBAL_INGAME_MODE_SCROLL_DOWN ) )
	return;

    //--------------------
    // On the first function call ever, we load the surfaces for the
    // flags into memory.
    //
    if ( first_call )
    {
	for ( i = 0 ; i < 8 ; i ++ )
	{
	    sprintf ( constructed_filename , "mouse_cursor_corona_%04d.png" , i );
	    fpath = find_file ( constructed_filename , GRAPHICS_DIR, FALSE );
	    get_iso_image_from_file_and_path ( fpath , & ( mouse_cursor_coronas [ i ] ) , FALSE ) ;
	    if ( mouse_cursor_coronas [ i ] . surface == NULL ) 
	    {
		fprintf ( stderr , "\nFull path used: %s." , fpath );
		GiveStandardErrorMessage ( __FUNCTION__ , "\
Error loading flag image.",
					   PLEASE_INFORM, IS_FATAL );
	    }
	    if ( use_open_gl )
	    {
		DebugPrintf ( 1 , "\n%s(): Texture made from mouse cursor corona surface..." );
		make_texture_out_of_surface ( & ( mouse_cursor_coronas [ i ] ) ) ;
	    }
	}

	first_call = FALSE ;
    }

    //--------------------
    // First we check to see if there is maybe a person underneath the current
    // mouse cursor.  If this is so, we'll use this person to associate all 
    // special functions.  Obstacles have a lower priority and only get taken
    // into account if there isn't any person present as well.
    //
    our_enemy_index = GetLivingDroidBelowMouseCursor ( 0 ) ;
    if ( our_enemy_index == (-1) )
    {
	our_obstacle_index = GetObstacleBelowMouseCursor ( 0 ) ;
	if ( our_obstacle_index == (-1) ) return;
    }
	
    //--------------------
    // Now we blit the full mouse cursor corona
    //
    for ( i = 0 ; i < 8 ; i ++ )
    {
	offset_vector . x = 0 ;
	offset_vector . y = -35 ;
	RotateVectorByAngle ( &offset_vector , -(i+1) * 35 );
	offset_vector . x += 7 ;
	offset_vector . y += 7 ;

	if ( use_open_gl )
	{
	    blit_open_gl_texture_to_screen_position ( mouse_cursor_coronas [ i ] , 
						      GetMousePos_x () + offset_vector . x , GetMousePos_y () + offset_vector . y , TRUE );
	}
	else
	{
	    blit_iso_image_to_map_position ( mouse_cursor_coronas [ i ] , 
					     GetMousePos_x () , GetMousePos_x () );
	}
    }

}; // void blit_mouse_cursor_corona ( void )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
get_obstacle_below_mouse_cursor ( void ) 
{
    
}; // void get_obstacle_below_mouse_cursor ( void ) 

/* ----------------------------------------------------------------------
 * Occasionally it might come in handly to have the whole image fading
 * out when something time-consuming is happening, which is not displayed.
 * This function is intended to provide a mechanism for this using the
 * gamma adjustment.  Note, that the SDL documentation clearly mentions,
 * that NOT ALL HARDWARE SUPPORTS THIS.  But if it isn't supported, we
 * should still be safe and it should just mean that nothing will happen
 * in here except for some (unexplained) delay.
 * ---------------------------------------------------------------------- */
void
fade_out_using_gamma_ramp ( void )
{
    int i;
    Activate_Conservative_Frame_Computation( );
    for ( i = 0 ; i < 100 ; i ++ ) 
    {
	SDL_SetGamma ( GameConfig . current_gamma_correction * 0.01 * ( (float) ( 100 - i ) ) , 
		       GameConfig . current_gamma_correction * 0.01 * ( (float) ( 100 - i ) ) , 
		       GameConfig . current_gamma_correction * 0.01 * ( (float) ( 100 - i ) ) );
	SDL_Delay ( 8 ) ;
    }
}; // void fade_out_using_gamma_ramp ( void )

/* ----------------------------------------------------------------------
 * Occasionally it might come in handly to have the whole image fading
 * out when something time-consuming is happening, which is not displayed.
 * This function is intended to provide a mechanism for this using the
 * gamma adjustment.  Note, that the SDL documentation clearly mentions,
 * that NOT ALL HARDWARE SUPPORTS THIS.  But if it isn't supported, we
 * should still be safe and it should just mean that nothing will happen
 * in here except for some (unexplained) delay.
 * ---------------------------------------------------------------------- */
void
fade_in_using_gamma_ramp ( void )
{
    int i;
    Activate_Conservative_Frame_Computation( );
    for ( i = 0 ; i < 100 ; i ++ ) 
    {
	SDL_SetGamma ( GameConfig . current_gamma_correction * 0.01 * ((float)i) , 
		       GameConfig . current_gamma_correction * 0.01 * ((float)i) , 
		       GameConfig . current_gamma_correction * 0.01 * ((float)i) );
	SDL_Delay ( 8 ) ;
    }
}; // void fade_in_using_gamma_ramp ( void )

/* ----------------------------------------------------------------------
 * There is need to do some padding, cause OpenGL textures need to have
 * a format: width and length both each a power of two.  Therefore some
 * extra alpha to the sides must be inserted.  This is what this function
 * is supposed to do:  manually adding hte proper amount of padding to
 * the surface, so that the dimensions will reach the next biggest power
 * of two in both directions, width and length.
 * ---------------------------------------------------------------------- */
SDL_Surface*
rip_rectangle_from_alpha_image ( SDL_Surface* our_surface , SDL_Rect our_rect ) 
{
  SDL_Surface* padded_surf;
  SDL_Surface* tmp_surf;
  SDL_Rect dest;

  our_rect.h -= 0 ;

  padded_surf = SDL_CreateRGBSurface( SDL_SWSURFACE | SDL_SRCALPHA , our_rect . w , our_rect . h , 32, 0x0FF000000 , 0x000FF0000  , 0x00000FF00 , 0x000FF );
  tmp_surf = SDL_DisplayFormatAlpha ( padded_surf ) ;
  SDL_FreeSurface ( padded_surf );

  // SDL_SetAlpha( our_surface , 0 , 0 );
  SDL_SetAlpha( our_surface , 0 , SDL_ALPHA_OPAQUE );
  SDL_SetAlpha( tmp_surf , 0 , SDL_ALPHA_OPAQUE );
  SDL_SetColorKey( our_surface , 0 , 0x0FF );

  dest . x = 0;
  dest . y = 0;
  dest . w = our_rect . w ;
  dest . h = our_rect . h ;

  our_SDL_blit_surface_wrapper ( our_surface, & our_rect , tmp_surf , NULL );

  SDL_SetAlpha( tmp_surf , SDL_SRCALPHA , SDL_ALPHA_OPAQUE );

  flip_image_horizontally ( tmp_surf ) ;

  return ( tmp_surf );

}; // SDL_Surface* rip_rectangle_from_alpha_image ( SDL_Surface* our_surface , SDL_Rect our_rect ) 

/* ----------------------------------------------------------------------
 * In the shop interface, when an item was selected that could be grouped
 * together in inventory, we showed three mouse buttons to either buy 1,
 * buy 10 or buy 100 or the similar thing for selling items.
 * But now Bastian has proposed a new number selector design with a scale
 * and a small knob to set the right number of items you wish to have and
 * also with small buttons left and right for some fine tuning. 
 * This function is intended to handle this number selection process.
 * It will accept the range allowed and do the complete selection process
 * with the user until he presses 'OK' on the scale screen.
 * ---------------------------------------------------------------------- */
int
do_graphical_number_selection_in_range ( int lower_range , int upper_range, int default_value )
{
    static SDL_Surface* SelectionKnob = NULL ;
    int ok_button_was_pressed = FALSE;
    int left_mouse_pressed_previous_frame = FALSE;
    int knob_start_x = UNIVERSAL_COORD_W(200);
    int knob_end_x = UNIVERSAL_COORD_W(390);
    if(!(upper_range - lower_range))
	return upper_range;
    int knob_offset_x = ceilf ( (float)(default_value * (knob_end_x - knob_start_x)) / (float)(upper_range - lower_range));
    int knob_is_grabbed = FALSE ;
    char number_text[1000];
    static SDL_Rect knob_target_rect;

    DebugPrintf ( 1 , "\n%s(): graphical number selection invoked." , __FUNCTION__ );
    
    // MakeGridOnScreen ( NULL );
    
    StoreMenuBackground ( 1 );

    //--------------------
    // Next we prepare the selection knob for all later operations
    //
    if ( SelectionKnob == NULL )
	SelectionKnob = our_IMG_load_wrapper( find_file ( "mouse_buttons/number_selector_selection_knob.png" , GRAPHICS_DIR, FALSE ) );
    if ( SelectionKnob == NULL )
    {
	fprintf( stderr, "\n\nSDL_GetError: %s \n" , SDL_GetError() );
	GiveStandardErrorMessage ( __FUNCTION__  , "\
ERROR LOADING SELECTION KNOB IMAGE FILE!",
				   PLEASE_INFORM, IS_FATAL );
    }
    
    knob_target_rect . w = SelectionKnob -> w;
    knob_target_rect . h = SelectionKnob -> h;
    
    while ( SpacePressed() );
    while ( ! ok_button_was_pressed )
    {
	//--------------------
	// Now we assemble and show the screen, which includes 
	// 1. the background
	// 2. the ok button
	// 3. the knob of the scale
	// 4. the writing in the number selector
	//
	// then: show it.
	//
	RestoreMenuBackground ( 1 );
	blit_special_background ( NUMBER_SELECTOR_BACKGROUND_CODE );
	ShowGenericButtonFromList ( NUMBER_SELECTOR_OK_BUTTON );
	knob_target_rect . x = knob_start_x + knob_offset_x - knob_target_rect . w / 2 ;
	knob_target_rect . y = UNIVERSAL_COORD_H(260) - knob_target_rect . h / 2 ;
	our_SDL_blit_surface_wrapper ( SelectionKnob , NULL , Screen , &knob_target_rect );
	sprintf ( number_text , "%d" , knob_offset_x * ( upper_range - lower_range ) / ( knob_end_x - knob_start_x ) )  ;
	PutStringFont( Screen , FPS_Display_BFont , UNIVERSAL_COORD_W(320) , UNIVERSAL_COORD_H(190) , number_text );
	blit_our_own_mouse_cursor ( );
	our_SDL_flip_wrapper ( Screen );
	
	if ( ( SpacePressed() && axis_is_active ) && ( ! left_mouse_pressed_previous_frame ) ) 
	{
	    //--------------------
	    // Maybe the user has just 'grabbed the knob?  Then we need to
	    // mark the knob as grabbed.
	    //
	    if ( ( abs ( GetMousePos_x ( )  - ( knob_target_rect . x + knob_target_rect . w / 2 ) ) < knob_target_rect . w ) &&
		 ( abs ( GetMousePos_y ( )  - ( knob_target_rect . y + knob_target_rect . h / 2 ) ) < knob_target_rect . h ) )
	    {
		knob_is_grabbed = TRUE ;
	    }
	    
	    //--------------------
	    // OK pressed?  Then we can return the current scale value and
	    // that's it...
	    //
	    if ( MouseCursorIsOnButton ( NUMBER_SELECTOR_OK_BUTTON , GetMousePos_x()  , GetMousePos_y()  ) )
		ok_button_was_pressed = TRUE ;
	    if ( MouseCursorIsOnButton ( NUMBER_SELECTOR_LEFT_BUTTON , GetMousePos_x()  , GetMousePos_y()  ) )
	    {
		if(knob_offset_x > ((knob_end_x - knob_start_x) / (upper_range - lower_range)))
			{
			knob_offset_x -= (knob_end_x - knob_start_x) / (upper_range - lower_range);
			}
		if(knob_offset_x > 0)
			knob_offset_x --;
	    }

	    if ( MouseCursorIsOnButton ( NUMBER_SELECTOR_RIGHT_BUTTON , GetMousePos_x()  , GetMousePos_y()  ) )
	    {
		if ( knob_offset_x < knob_end_x - knob_start_x )
			{
			if(((knob_end_x - knob_start_x) / (upper_range - lower_range)) > 0)
				knob_offset_x += (knob_end_x - knob_start_x) / (upper_range - lower_range);
			else knob_offset_x ++;
			}
	    }
	    
	}
	if ( ! ( SpacePressed() && axis_is_active ) ) knob_is_grabbed = FALSE ;
	
	if ( knob_is_grabbed )
	{
	    knob_offset_x = GetMousePos_x()  - knob_start_x ;
	    if ( knob_offset_x >= knob_end_x - knob_start_x ) knob_offset_x = knob_end_x - knob_start_x ;
	    if ( knob_offset_x <= 0 ) knob_offset_x = 0 ; 
	}
	
	
	left_mouse_pressed_previous_frame = axis_is_active ;
	SDL_Delay (1);
    }
    
    
    return ( knob_offset_x * ( upper_range - lower_range ) / ( knob_end_x - knob_start_x ) ) ;
    
}; // int do_graphical_number_selection_in_range ( int lower_range , int upper_range )

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

}; // Uint8 GetAlphaComponent ( SDL_Surface* SourceSurface , int x , int y )

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
  ThirdSurface = our_SDL_display_format_wrapperAlpha ( FirstSurface );

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

	  PutPixel ( ThirdSurface , x , y , 
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
  Uint8 red=0;
  Uint8 green=0;
  Uint8 blue=0;
  float alpha3 ;

  //--------------------
  // First we check for NULL surfaces given...
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
  ThirdSurface = our_SDL_display_format_wrapperAlpha ( FirstSurface );

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

	  PutPixel ( ThirdSurface , x , y , 
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
  Uint8 new_red=0, new_green=0, new_blue=0, red1, red2, green1, green2, blue1 , blue2, raw_alpha1, raw_alpha2 ;
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
  ThirdSurface = our_SDL_display_format_wrapperAlpha ( FirstSurface );
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
	  // This PutPixel must go out and be replaced by something with
	  // less overhead...
	  //
	  // PutPixel ( ThirdSurface , x , y , 
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
  SDL_Surface* FullView;

  Screenshoot_Filename=malloc(100);
  DebugPrintf (1, "\n\nScreenshoot function called.\n\n");
  sprintf( Screenshoot_Filename , "Screenshot_%02d.bmp", Number_Of_Screenshot );
  DebugPrintf(1, "\n\nScreenshoot function: The Filename is: %s.\n\n" , Screenshoot_Filename );

  if ( use_open_gl )
    {
      //--------------------
      // We need to make a copy in processor memory.  This has already
      // been implemented once, and we can just reuse the old code here.
      //
      StoreMenuBackground ( 1 ) ;

      //--------------------
      // Now we need to make a real SDL surface from the raw image data we
      // have just extracted.
      //
      FullView = SDL_CreateRGBSurfaceFrom( StoredMenuBackground [ 1 ] , GameConfig . screen_width , GameConfig . screen_height, 24, 3 * GameConfig . screen_width, 0x0FF0000, 0x0FF00, 0x0FF , 0 );

      flip_image_horizontally ( FullView );
      
      swap_red_and_blue_for_open_gl ( FullView );

      SDL_SaveBMP( FullView , Screenshoot_Filename );

      SDL_FreeSurface ( FullView ) ;

    }
  else
    {
      SDL_SaveBMP( Screen , Screenshoot_Filename );
    }

  Number_Of_Screenshot++;
  free(Screenshoot_Filename);

  //--------------------
  // Now that a screenshot has been taken, we might as well start
  // the sound of a camera taking a picture... :)
  //
  PlayOnceNeededSoundSample( "../effects/CameraTakesPicture.wav" , FALSE , TRUE ) ;

  //--------------------
  // Taking the screenshot, converting is and saving it, maybe also
  // flipping it around, all these things cost time, so in order not
  // to make to much of a jump after a screenshot has been made and
  // saved, we use the conservative frame time computation for this
  // next frame now.
  //
  Activate_Conservative_Frame_Computation();

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
  // It's not very convenient to put a real grid over the screen using 
  // OpenGL.  We do something similar but not quite as expensive in function
  // overhead here.
  //
  if ( use_open_gl )
    {
      // our_SDL_fill_rect_wrapper ( Screen , & User_Rect , SDL_MapRGBA( Screen -> format , 0, 0, 0, 0x050 ) ) ;
      return;
    }


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
       ( GridRectangle->x >= GameConfig . screen_width ) ||
       ( GridRectangle->y >= GameConfig . screen_height ) )
    {
      Copy_Rect ( TempRect , *GridRectangle );
      return;
    }
  if ( ( GridRectangle->x + GridRectangle->w ) > GameConfig . screen_width )
    {
      GridRectangle->w = GameConfig . screen_width - GridRectangle->x ;
    }
  if ( ( GridRectangle->y + GridRectangle->h ) > GameConfig . screen_height )
    {
      GridRectangle->h = GameConfig . screen_height - GridRectangle->y ;
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
	      PutPixel( Screen, x, y, 0 );
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
void 
DisplayImage( char *datafile )
{
  SDL_Surface *image;
  
  image = our_IMG_load_wrapper(datafile);
  if ( image == NULL ) {
    fprintf(stderr, "Couldn't load image %s: %s\n",
	    datafile, IMG_GetError());
    Terminate(ERR);
  }

  our_SDL_blit_surface_wrapper(image, NULL, Screen, NULL);

  SDL_FreeSurface(image);

}; // void DisplayImage( char *datafile )

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

  DebugPrintf ( 1 , "\nvoid LoadThemeConfigurationFile: Data will be taken from file %s.  Commencing...\n " , fpath );

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
	  GiveStandardErrorMessage ( __FUNCTION__  , "\
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
      GiveStandardErrorMessage ( __FUNCTION__  , "\
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

}; // void LoadThemeConfigurationFile ( void )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
get_standard_iso_floor_tile_size ( void )
{
    SDL_Surface *standard_floor_tile;

    standard_floor_tile = our_IMG_load_wrapper( find_file ( "floor_tiles/iso_miscellaneous_floor_0000.png" , GRAPHICS_DIR, FALSE ) );
    if ( standard_floor_tile == NULL )
    {
	fprintf( stderr, "\n\nSDL_GetError: %s \n" , SDL_GetError() );
	GiveStandardErrorMessage ( __FUNCTION__  , "\
UNABLE TO LOAD STANDARD TILE!",
				   PLEASE_INFORM, IS_FATAL );
    }
    
    //--------------------
    // Warning!  The standard tile sizes should be a multiple of 2 in
    //           both directions to prevent jittering from numerical 
    //           rounding.
    //
    //           They also should be a bit less than a real tile size
    //           to hide some of the gap from the shading and anti-aliasing
    //           of the rendering process.
    //
    if ( standard_floor_tile -> w % 2 )
	iso_floor_tile_width  = standard_floor_tile -> w - 3;
    else
	iso_floor_tile_width  = standard_floor_tile -> w - 2;
    if ( standard_floor_tile -> h % 2 )
	iso_floor_tile_height = standard_floor_tile -> h - 3;
    else
	iso_floor_tile_height = standard_floor_tile -> h - 2;

    SDL_FreeSurface ( standard_floor_tile );

}; // void get_standard_iso_floor_tile_size ( void )

/* -----------------------------------------------------------------
 * This function does all the bitmap initialisation, so that you
 * later have the bitmaps in perfect form in memory, ready for blitting
 * them to the screen.
 * ----------------------------------------------------------------- */
void
InitPictures (void)
{
    //--------------------
    // First thing to do is get the size of a typical isometric
    // floor tile, i.e. height and width of the corresponding graphics
    // bitmap
    //
    get_standard_iso_floor_tile_size ();
    
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
    
    set_mouse_cursor_to_shape ( MOUSE_CURSOR_CROSSHAIR_SHAPE ) ;
    
    ShowStartupPercentage ( 22 ) ; 
    
    load_all_isometric_floor_tiles (  );
    
    ShowStartupPercentage ( 25 ) ; 
    
    load_all_obstacles (  ) ;
    
    ShowStartupPercentage ( 26 ) ; 
    
    DebugPrintf( 1 , "\nvoid InitPictures(void): preparing to load droids." );

    Load_Enemy_Surfaces();
    
    ShowStartupPercentage ( 38 ) ; 
    
    ShowStartupPercentage ( 43 ) ; 
    
    clear_all_loaded_tux_images ( FALSE ) ;
    
    ShowStartupPercentage ( 65 ) ; 
    
    DebugPrintf( 1 , "\nvoid InitPictures(void): preparing to load blast image file." );
    Load_Blast_Surfaces();
    
    ShowStartupPercentage ( 70 ) ; 
    
    DebugPrintf( 1 , "\nvoid InitPictures(void): preparing to load items image file." );
    
    ShowStartupPercentage ( 75 ) ; 
    
    Load_Mouse_Move_Cursor_Surfaces( );
    
    ShowStartupPercentage ( 80 ) ; 
    
    DebugPrintf( 1 , "\nvoid InitPictures(void): preparing to load bullet file." );
    DebugPrintf( 1 , "\nvoid InitPictures(void): Number_Of_Bullet_Types : %d." , Number_Of_Bullet_Types );
    iso_load_bullet_surfaces();
    
    ShowStartupPercentage ( 92 ) ; 
  
};  // void InitPictures ( void )

/* ----------------------------------------------------------------------
 * This function should load all the fonts we'll be using via the SDL
 * BFont library in Freedroid.
 * ---------------------------------------------------------------------- */
void
InitOurBFonts ( void )
{
#define ALL_BFONTS_WE_LOAD 6

#define PARA_FONT_FILE 		"font/parafont.png"
#define MENU_FONT_FILE 		"font/cpuFont.png"
// #define MESSAGE_FONT_FILE 	"font/ArialGold.png"
// #define MESSAGE_FONT_FILE 	"font/SmallStone.png"
#define MESSAGE_FONT_FILE 	"font/small_white.png"
#define RED_FONT_FILE 		"font/font05_red.png"
#define BLUE_FONT_FILE 		"font/font05_white.png"
#define FPS_FONT_FILE 		"font/font05.png"
// #define FPS_FONT_FILE 		"font/SmallStone.png"

    char* fpath;
    int i;
    char* MenuFontFiles[ALL_BFONTS_WE_LOAD] =
	{
	    MENU_FONT_FILE,
	    MESSAGE_FONT_FILE,
	    PARA_FONT_FILE,
	    FPS_FONT_FILE,
	    RED_FONT_FILE,
	    BLUE_FONT_FILE,
	};
    BFont_Info** MenuFontPointers[ALL_BFONTS_WE_LOAD] =
	{
	    &Menu_BFont,
	    &Message_BFont,
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
	    GiveStandardErrorMessage ( __FUNCTION__  , "\
A font file for the BFont library was not found.",
				       PLEASE_INFORM, IS_FATAL );
	} 
	else
	{
	    DebugPrintf(1, "\nSDL Menu Font initialisation successful.\n");
	}
    }
    
}; // InitOurBFonts ( void )

/* -----------------------------------------------------------------
 * This funciton initialises the timer subsystem.
 * -----------------------------------------------------------------*/
void
InitTimer (void)
{
    //--------------------
    // Now SDL_TIMER is initialized here:
    //
    if ( SDL_InitSubSystem ( SDL_INIT_TIMER ) == -1 ) 
    {
	fprintf(stderr, "Couldn't initialize SDL: %s\n" , SDL_GetError ( ) ) ;
	Terminate(ERR);
    } 
    else
	DebugPrintf( 1 , "\nSDL Timer initialisation successful.\n");

}; // void InitTimer (void)

/* ----------------------------------------------------------------------
 * This function checks if the availability of OpenGL libraries (at compile
 * time) and request of OpenGL graphics output are compatible with each
 * other...  If not, we just disable OpenGL output method...
 * ---------------------------------------------------------------------- */
void
check_open_gl_libraries_present ( void )
{
    //--------------------
    // Here we introduce some warning output in case open_gl output is
    // requested while the game was compiled without having the GL libs...
    //
    // The solution in this case is to force open_gl switch off again and
    // to (forcefully) print out a warning message about this!
    //
    if ( use_open_gl )
    {
#ifndef HAVE_LIBGL
	DebugPrintf ( -100 , 
		      "\n**********************************************************************\
\n*\
\n*  W  A  R  N  I  N  G    !  !  ! \
\n*\
\n* You have requested OpenGL output via command line switch (-o parameter)\
\n* but you (or someone else) compiled this version of FreedroidRPG without\
\n* having the nescessary OpenGL libraries on your (his/her) system. \
\n*\
\n* FreedroidRPG will now fallback to normal SDL output (which might be a\
\n* lot slower than the OpenGL method.\n\
\n*\
\n* You might try setting appropriate speed optimisation parameters in the\
\n* 'performance tweaks' menu, in case you run into speed trouble.\
\n*\
\n* If you prefer to use OpenGL output, please make sure that you have \
\n* libGL installed on your system and recompile FreedroidRPG.\
\n*\
\n***********************************************************************\
\n" );
	use_open_gl = FALSE ;
#endif
    }
}; // void check_open_gl_libraries_present ( void )

/* ----------------------------------------------------------------------
 * This function should display the driver info obtained from the OpenGL
 * libraries.  This should be in a function of it's own (like now) to 
 * make sure that the OpenGL error checks in the video mode set functions
 * and that seem to be occuring so frequently are not coming from this 
 * chunk of code.
 * ---------------------------------------------------------------------- */
void
safely_show_open_gl_driver_info ( void )
{
#ifdef HAVE_LIBGL
    //--------------------
    // Since we want to use openGl, it might be good to check the OpenGL vendor string
    // provided by the graphics driver.  Let's see...
    //
    fprintf( stderr , "\n-OpenGL-------------------------------------------------------" );
    fprintf( stderr , "\nVendor     : %s", glGetString( GL_VENDOR ) );
    open_gl_check_error_status ( __FUNCTION__ );
    fprintf( stderr , "\nRenderer   : %s", glGetString( GL_RENDERER ) );
    open_gl_check_error_status ( __FUNCTION__ );
    fprintf( stderr , "\nVersion    : %s", glGetString( GL_VERSION ) );
    open_gl_check_error_status ( __FUNCTION__ );
    // fprintf( stderr , "\nExtentions : %s", glGetString( GL_EXTENSIONS ) );
    // open_gl_check_error_status ( __FUNCTION__ );
    fprintf( stderr , "\n\n" );
#endif
}; // void safely_show_open_gl_driver_info ( void )

/* ----------------------------------------------------------------------
 * This function gets the video info from OpenGL.  We do this
 * in a separate function, so that eventual errors (and bug reports) from
 * the OpenGL error checking can be attributed to a source more easily.
 * ---------------------------------------------------------------------- */
SDL_VideoInfo*
safely_get_SDL_video_info ( void )
{

    SDL_VideoInfo *vid_info;

    vid_info = (SDL_VideoInfo*) SDL_GetVideoInfo (); 
    if ( !vid_info )
    {
	fprintf(stderr, "Could not obtain video info via SDL: %s\n", SDL_GetError ( ) );
	Terminate(ERR);
    }
    
    //--------------------
    // Since the OpenGL stuff hasn't been initialized yet, it's normal
    // to get an GL_INVALID_OPERATION here, if we would really do the
    // check.  So better refrain from OpenGL error checking here...
    //
    // open_gl_check_error_status ( __FUNCTION__ );

    return ( vid_info );

}; // SDL_VideoInfo *safely_get_SDL_video_info ( void )

/* ----------------------------------------------------------------------
 * This function sets the OpenGL double buffering attribute.  We do this
 * in a separate function, so that eventual errors (and bug reports) from
 * the OpenGL error checking can be attributed to a source more easily.
 * ---------------------------------------------------------------------- */
void
safely_set_double_buffering_attribute ( void )
{
    
#ifdef HAVE_LIBGL

    if ( SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 ) )
    {
	GiveStandardErrorMessage ( __FUNCTION__  , "\
Unable to set SDL_GL_DOUBLEBUFFER attribute!",
				   PLEASE_INFORM, IS_FATAL );
    }
    
    //--------------------
    // Since the OpenGL stuff hasn't been initialized yet, it's normal
    // to get an GL_INVALID_OPERATION here, if we would really do the
    // check.  So better refrain from OpenGL error checking here...
    //
    // open_gl_check_error_status ( __FUNCTION__ );

#endif
    
}; // void safely_set_double_buffering_attribute ( void )

/* ----------------------------------------------------------------------
 * This function is supposed to set the video mode in the case that 
 * OpenGL is used for graphics output.  The function is highly split up
 * into subfunctions now, so that the OpenGL error checking will be more
 * precise.  Typically it's in here that most problems occur when there
 * is a peculiar OpenGL driver used, mostly under the Windows operating
 * system.
 * ---------------------------------------------------------------------- */
void
set_video_mode_for_open_gl ( void )
{
#ifdef HAVE_LIBGL
    SDL_VideoInfo *vid_info;
    Uint32 video_flags = 0 ;  // flags for SDL video mode 
    int video_mode_ok_check_result ;
    int buffer_size , depth_size, red_size, green_size, blue_size, alpha_size ;

    //--------------------
    // We query the available video configuration on this system.
    //
    vid_info = safely_get_SDL_video_info ();

    //--------------------
    // We need OpenGL double buffering, so we request it.  If we
    // can't get it, something must be wrong, maybe an extremely bad 
    // card/driver is present or some bad emulation.  Anyway, we'll
    // break off...
    //
    safely_set_double_buffering_attribute();

    //--------------------
    // Now we start setting up the proper OpenGL flags to pass to the
    // SDL for creating the initial output window...
    //
    video_flags  = SDL_OPENGL;          /* Enable OpenGL in SDL */
    
    //--------------------
    // Now according to the document http://sdldoc.csn.ul.ie/guidevideoopengl.php
    // we do need the SDL_GL_SetAttribute ( SDL_GL_DOUBLEBUFFER, 1 ) and NOT
    // this here...
    //
    // video_flags |= SDL_GL_DOUBLEBUFFER; /* Enable double buffering */
    
    // video_flags |= SDL_HWPALETTE;       /* Store the palette in hardware */
    video_flags |= SDL_RESIZABLE;       /* Enable window resizing */
    if (fullscreen_on) video_flags |= SDL_FULLSCREEN;
    if ( vid_info->hw_available )
	video_flags |= SDL_HWSURFACE;
  else
      video_flags |= SDL_SWSURFACE;
    if ( vid_info->blit_hw )
	video_flags |= SDL_HWACCEL;
    //--------------------
    // We have 24 bit (or 32 bit) color depth in some of the graphics used,
    // like e.g. backgrounds produced by Basse, so we try to get close to
    // a target color depth of 24, or 32.
    //
    vid_bpp = 32; 
    
    //--------------------
    // First we check to see if the mode we wish to set is really supported.  If it
    // isn't supported, then we cancel the whole operation...
    //
    video_mode_ok_check_result = SDL_VideoModeOK( GameConfig . screen_width, GameConfig . screen_height, vid_bpp , video_flags );
    switch ( video_mode_ok_check_result )
    {
	case 0:
	    GiveStandardErrorMessage ( __FUNCTION__  , "\
SDL reported, that the video mode mentioned above is not supported UNDER ANY BIT COLOR DEPTH!\nBreaking off...",
				       PLEASE_INFORM, IS_FATAL );
	    break;
	default:
	    DebugPrintf ( -4 , "\nTesting if color depth %d bits is available... " , vid_bpp );
	    if ( video_mode_ok_check_result == vid_bpp )
	    {
		DebugPrintf ( -4 , "YES." );
	    }
	    else
	    {
		DebugPrintf ( -4 , "NO! \nThe closest we will get is %d bits per pixel." , video_mode_ok_check_result );
		/*
		  GiveStandardErrorMessage ( __FUNCTION__  , "\
		  SDL reported, that the video mode mentioned \nabove is not supported UNDER THE COLOR DEPTH MENTIONED ABOVE!\n\
		  We'll be using the alternate color depth given above instead...",
		  PLEASE_INFORM, IS_WARNING_ONLY );
		*/
		vid_bpp = video_mode_ok_check_result ;
	    }
    }
    
    //--------------------
    // Now that we know which mode to go for, we can give it a try and get the
    // output surface we want.  Of course, some extra checking will be done, so
    // that we know that the surface we're expecting is really there...
    //
    Screen = SDL_SetVideoMode( GameConfig . screen_width, GameConfig . screen_height, vid_bpp , video_flags );
    if ( !Screen )
    {
	fprintf( stderr,  "Video mode set failed: %s\n", SDL_GetError( ) );
	Terminate ( ERR ) ;
    }
    else
    {
	//      open_gl_check_error_status ( __FUNCTION__ );
	SDL_GL_GetAttribute( SDL_GL_BUFFER_SIZE , & buffer_size);
	SDL_GL_GetAttribute( SDL_GL_RED_SIZE , & red_size);
	SDL_GL_GetAttribute( SDL_GL_GREEN_SIZE , & green_size);
	SDL_GL_GetAttribute( SDL_GL_BLUE_SIZE , & blue_size);
	SDL_GL_GetAttribute( SDL_GL_ALPHA_SIZE , & alpha_size);
	SDL_GL_GetAttribute( SDL_GL_DEPTH_SIZE , & depth_size);
	fprintf( stderr , "\n\nvideo mode set (bpp=%d RGBA=%d%d%d%d depth=%d)" ,
		 buffer_size , red_size, green_size, blue_size, alpha_size, depth_size );
    }
    
    //--------------------
    // Since the OpenGL stuff hasn't been initialized yet, it's normal
    // to get an GL_INVALID_OPERATION here, if we would really do the
    // check.  So better refrain from OpenGL error checking here...
    //
    // open_gl_check_error_status ( __FUNCTION__ );
    
    safely_show_open_gl_driver_info ( );
    
    safely_initialize_our_default_open_gl_parameters ( );
    
    //--------------------
    // Maybe resize the window to standard size?
    //
    // resizeWindow( GameConfig . screen_width, GameConfig . screen_height );
    //
    
    //--------------------
    // Now under win32 we're running into problems, because there seems to be some
    // garbage in ONE OF THE TWO BUFFERS from the double-buffering.  Maybe cleaning
    // that out solves part of the problem.  Well, not all, since there is still the
    // dialog background not visible.  But anyway, let's just clear the two buffers
    // for now...
    //
    our_SDL_fill_rect_wrapper ( Screen , NULL , 0 );
    our_SDL_flip_wrapper ( Screen ) ;
    our_SDL_fill_rect_wrapper ( Screen , NULL , 0 );
    our_SDL_flip_wrapper ( Screen ) ;
    
#endif // HAVE_LIBGL

}; // void set_video_mode_for_open_gl ( void )

/* -----------------------------------------------------------------
 * This funciton initialises the video display and opens up a 
 * window for graphics display.
 * -----------------------------------------------------------------*/
void
InitVideo (void)
{
    const SDL_VideoInfo *vid_info;
    char vid_driver[81];
    Uint32 video_flags = 0 ;  // flags for SDL video mode 
    char *fpath;
    char window_title_string[200];
    
    //--------------------
    // Initialize the SDL library 
    //
    if ( SDL_Init ( SDL_INIT_VIDEO ) == -1 ) 
    {
	fprintf(stderr, "Couldn't initialize SDL: %s\n",SDL_GetError());
	Terminate(ERR);
    } 
    else
    {
	DebugPrintf( 1, "\nSDL Video initialisation successful.\n");
	//--------------------
	// So the video library could be initialized.  So it should also be
	// cleaned up and closed once we're done and quit FreedroidRPG.
	//
	atexit (SDL_Quit);
    }

    //--------------------
    // Let's get some info about the whole system here.  Is this a windows or x11 or
    // mac or whatever graphical environment?
    //
    // NOTE:  This has got NOTHING to do with OpenGL and OpenGL venour or the like yet...
    //
    if ( SDL_VideoDriverName ( vid_driver , 80 ) )
    {
	DebugPrintf ( -4 , "\nVideo system type: %s." , vid_driver );
    }
    else
    {
	fprintf(stderr, "Video driver seems not to exist or initialisation failure!\nError code: %s\n",SDL_GetError());
	Terminate(ERR);
    }
    
    //--------------------
    // We check if the program has been compiled with OpenGL libraries present
    // and take care of the case OpenGL output requested when compiled without
    // those libs...
    //
    check_open_gl_libraries_present();
    
    //--------------------
    // We note the screen resolution used.
    //
    DebugPrintf ( -4 , "\nUsing screen resolution %d x %d." ,
		  GameConfig . screen_width , GameConfig . screen_height );
    
    if ( use_open_gl )
    {
	set_video_mode_for_open_gl();
    }
    else
    {
	// RP: let's try without those...
	// video_flags = SDL_SWSURFACE | SDL_HWPALETTE ;
	if (fullscreen_on) video_flags |= SDL_FULLSCREEN;
	
	/* 
	 * currently only the simple 320x200 mode is supported for 
	 * simplicity, as all our graphics are in this format
	 * once this is up and running, we'll provide others modes
	 * as well.
	 */
	if( !(Screen = SDL_SetVideoMode ( GameConfig . screen_width, GameConfig . screen_height , 0 , video_flags )) )
	{
	    fprintf(stderr, "Couldn't set (2*) 320x240*SCALE_FACTOR video mode: %s\n",
		    SDL_GetError()); 
	    Terminate ( ERR ) ; 
	}
    }
    
    //--------------------
    // We query the available video configuration on this system.
    //
    vid_info = SDL_GetVideoInfo (); 
    if ( !vid_info )
    {
	fprintf(stderr, "Could not obtain video info via SDL: %s\n",SDL_GetError());
	Terminate(ERR);
    }
    
    vid_bpp = 32; /* start with the simplest */
    
    //--------------------
    // End of possibly open-gl dependant initialisation stuff...
    //
    sprintf ( window_title_string , "FreedroidRPG %s" , VERSION );
    if ( vid_info->wm_available )  /* if there's a window-manager */
    {
	SDL_WM_SetCaption ( window_title_string , "" );
	fpath = find_file ( ICON_FILE , GRAPHICS_DIR , FALSE );
	SDL_WM_SetIcon( our_IMG_load_wrapper ( fpath ) , NULL );
    }
    
    InitOurBFonts ( );
    
    blit_special_background ( FREEDROID_LOADING_PICTURE_CODE );
    our_SDL_flip_wrapper ( Screen ) ;
    
    ShowStartupPercentage ( 1 ) ; 
    
    if ( use_open_gl )
	set_up_texture_for_automap ( );
    
    SDL_SetGamma( 1 , 1 , 1 );
    GameConfig.current_gamma_correction=1;
    
    if ( ! mouse_control )  // hide mouse pointer if not needed 
	SDL_ShowCursor (SDL_DISABLE);
    
}; // void InitVideo () 

/* ----------------------------------------------------------------------
 * This function fills all the screen or the freedroid window with a 
 * black color.  The name of the function originates from earlier, when
 * we still wrote directly to the vga memory using memset under ms-dos.
 * ---------------------------------------------------------------------- */
void
ClearGraphMem ( void )
{
  SDL_SetClipRect( Screen, NULL );

  // Now we fill the screen with black color...
  our_SDL_fill_rect_wrapper( Screen , NULL , 0 );
}; // void ClearGraphMem( void )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
SDL_HighlightRectangle ( SDL_Surface* Surface , SDL_Rect Area )
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
      
#define HIGHLIGHT_BLOCK_FRAME 2

      // if ( ! ( ( y < Area . y + HIGHLIGHT_BLOCK_FRAME ) || ( y + HIGHLIGHT_BLOCK_FRAME >= Area.y+Area.h ) ) && 
      // ! GameConfig.highlighting_mode_full ) continue ;

      for ( x = Area.x ; x < Area.x+Area.w ; x ++ )
	{

	  if ( ( ! ( ( x < Area . x + HIGHLIGHT_BLOCK_FRAME ) || ( x + HIGHLIGHT_BLOCK_FRAME >= Area.x+Area.w ) ) && 
		 ! GameConfig.highlighting_mode_full ) &&
	       ( ! ( ( y < Area . y + HIGHLIGHT_BLOCK_FRAME ) || ( y + HIGHLIGHT_BLOCK_FRAME >= Area.y+Area.h ) ) && 
		 ! GameConfig.highlighting_mode_full ) ) continue ;			       

	  green = GetGreenComponent ( Surface , x , y ) ;
	  red = GetRedComponent ( Surface , x , y ) ;
	  blue = GetBlueComponent ( Surface , x , y ) ;

	  if ( green < red ) max = red; else max = green ; 
	  if ( max < blue ) max = blue;

	  EnhancementFactor = 90;
	  
	  if ( red < 255 - EnhancementFactor ) red += EnhancementFactor ;
	  else red = 255;
	  if ( green < 255 - EnhancementFactor ) green += EnhancementFactor ;
	  else green = 255;
	  if ( blue < 255 - EnhancementFactor ) blue += EnhancementFactor ;
	  else green = 255;

	  PutPixel ( Surface , x , y , 
		     SDL_MapRGB ( Surface -> format , red , green , blue ) ) ;

	}
    }

  SDL_UnlockSurface ( Surface );

}; // void Old_SDL_HighlightRectangle ( SDL_Surface* Surface , SDL_Rect Area )


/* ----------------------------------------------------------------------
 * This function draws a transparent white rectangle over a specified
 * area on the screen.
 * ---------------------------------------------------------------------- */
void
HighlightRectangle ( SDL_Surface* Surface , SDL_Rect Area )
{

    DebugPrintf ( 2 , "\n%s(): x=%d, y=%d, w=%d, h=%d." , __FUNCTION__ ,
		  Area . x , Area . y , Area . w , Area . h );
    if ( use_open_gl )
	GL_HighlightRectangle (Surface, Area , 255 , 255 , 255 , 100 );
    else
	SDL_HighlightRectangle ( Surface , Area );

    return;

}; // void HighlightRectangle

/* ----------------------------------------------------------------------
 * OpenGL has a habit of using different order for RGB, namely BGR.  To
 * fix this when saving something e.g. as bmp, we need to reverse the 
 * order of red, green and blue again, which we do here.
 * ---------------------------------------------------------------------- */
void
swap_red_and_blue_for_open_gl ( SDL_Surface* FullView )
{
    int x , y ;
    Uint32 pixel;
    
    // DebugPrintf ( -1000 , "\nSurface bpp: %d." , FullView -> format -> BytesPerPixel  );
    
    SDL_LockSurface ( FullView );
    for ( x = 0 ; x < FullView -> w ; x ++ )
    {
	for ( y = 0 ; y < FullView -> h ; y ++ )
	{
	    // pixel = FdGetPixel ( FullView , x , y ) ;
	    pixel = SDL_MapRGBA ( FullView -> format , 
				  GetBlueComponent ( FullView, x , y ) , 
				  GetGreenComponent ( FullView, x , y ) , 
				  GetRedComponent ( FullView, x , y ) ,
				  GetAlphaComponent ( FullView , x , y ) );
	    PutPixel ( FullView , x , y , pixel);
	}
    }
    SDL_UnlockSurface ( FullView );
    
}; // void swap_red_and_blue_for_open_gl ( SDL_Surface* FullView )

#undef _graphics_c
