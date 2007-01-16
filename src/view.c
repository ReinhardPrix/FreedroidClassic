/* 
 *
 *   Copyright (c) 1994, 2002, 2003, 2004 Johannes Prix
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

/* ----------------------------------------------------------------------
 * This file contains all the functions managing the things one gets to see.
 * That includes assembling of enemys, assembling the currently
 * relevant porting of the map (the bricks I mean), drawing all visible
 * elements like bullets, blasts, enemys or influencer in a nonvisible
 * place in memory at first, and finally drawing them to the visible
 * screen for the user.
 * ---------------------------------------------------------------------- */

#define _view_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "map.h"
#include "proto.h"
#include "colodefs.h"
#include "SDL_rotozoom.h"

#define PUT_ONLY_THROWN_ITEMS 3
#define PUT_NO_THROWN_ITEMS 4

char* 
part_group_strings [ ALL_PART_GROUPS ] = 
  {
    "" ,
    "shield_items/" ,
    "" ,
    "" ,
    "" ,
    "" 
  };

#define ALL_TUX_MOTION_CLASSES 2
iso_image loaded_tux_images [ ALL_PART_GROUPS ] [ TUX_TOTAL_PHASES ] [ MAX_TUX_DIRECTIONS ] ;

char* motion_class_string [ ALL_TUX_MOTION_CLASSES ] = { "sword_motion" , "gun_motion" } ;
int previously_used_motion_class = -4 ; // something we'll never really use...

extern int level_editor_mouse_move_mode;

void FdFlashWindow (SDL_Color Flashcolor);
void RecFlashFill (int LX, int LY, int Color, unsigned char *Parameter_Screen,
		   int SBreite);
int Cent (int);
void PutRadialBlueSparks( float PosX, float PosY , float Radius , int SparkType , int active_directions [ RADIAL_SPELL_DIRECTIONS ] ) ;
void insert_new_element_into_blitting_list ( float new_element_norm , int new_element_type , 
					     void* new_element_pointer , int code_number );

char *Affected;
EXTERN int MyCursorX;
EXTERN int MyCursorY;

EXTERN char *PrefixToFilename[ ENEMY_ROTATION_MODELS_AVAILABLE ];

SDL_Color flashcolor1 = {100, 100, 100};
SDL_Color flashcolor2 = {0, 0, 0};

#define MAX_ELEMENTS_IN_BLITTING_LIST (MAX_OBSTACLES_ON_MAP+100) // some enemies might there too

typedef struct
{
  int element_type;
  void *element_pointer;
  float norm_of_elements_position;
  int code_number;
}
blitting_list_element, *Blitting_list_element;

blitting_list_element blitting_list [ MAX_ELEMENTS_IN_BLITTING_LIST + 10 ] ;
int number_of_objects_currently_in_blitting_list ;

enum
{
    BLITTING_TYPE_NONE = 0 ,
    BLITTING_TYPE_OBSTACLE = 1 ,
    BLITTING_TYPE_ENEMY = 2 ,
    BLITTING_TYPE_TUX = 3 ,
    BLITTING_TYPE_BULLET = 4 ,
    BLITTING_TYPE_BLAST = 5 ,
    BLITTING_TYPE_THROWN_ITEM = 6
};


/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
blit_tux_status_flags ( void )
{
    static int first_call = TRUE ;
    int i;
    static iso_image power_flags [ 16 ] ;
    static iso_image dexterity_flags [ 16 ] ;
    int target_time;
    char constructed_filename[2000];
    char* fpath;

    //--------------------
    // On the first function call ever, we load the surfaces for the
    // flags into memory.
    //
    if ( first_call )
    {
	for ( i = 0 ; i < 16 ; i ++ )
	{
	    sprintf ( constructed_filename , "tux_flags/flag_power_%04d.png" , i + 1 );
	    fpath = find_file ( constructed_filename , GRAPHICS_DIR, FALSE );
	    get_iso_image_from_file_and_path ( fpath , & ( power_flags [ i ] ) , TRUE ) ;
	    if ( power_flags [ i ] . surface == NULL ) 
	    {
		fprintf ( stderr , "\nFull path used: %s." , fpath );
		GiveStandardErrorMessage ( __FUNCTION__ , "\
Error loading flag image.",
					   PLEASE_INFORM, IS_FATAL );
	    }

	    if ( use_open_gl )
	    {
		DebugPrintf ( 2 , "\nTexture made from flag surface..." );
		make_texture_out_of_surface ( & ( power_flags [ i ] ) ) ;
	    }
	}	

	for ( i = 0 ; i < 16 ; i ++ )
	{
	    sprintf ( constructed_filename , "tux_flags/flag_dexterity_%04d.png" , i + 1 );
	    fpath = find_file ( constructed_filename , GRAPHICS_DIR, FALSE );
	    get_iso_image_from_file_and_path ( fpath , & ( dexterity_flags [ i ] ) , TRUE ) ;
	    if ( dexterity_flags [ i ] . surface == NULL ) 
	    {
		fprintf ( stderr , "\nFull path used: %s." , fpath );
		GiveStandardErrorMessage ( __FUNCTION__ , "\
Error loading flag image.",
					   PLEASE_INFORM, IS_FATAL );
	    }

	    if ( use_open_gl )
	    {
		DebugPrintf ( 2 , "\nTexture made from flag surface..." );
		make_texture_out_of_surface ( & ( dexterity_flags [ i ] ) ) ;
	    }
	}	

	first_call = FALSE ;
    }

    //--------------------
    // We can now blit the flags...
    //
    target_time = ( (int) (Me [ 0 ] . current_game_date * 10 ) ) % 16 ;
    if ( target_time < 0 ) target_time = 0 ;
    if ( target_time >= 16 ) target_time = 0 ;

    if ( Me [ 0 ] . power_bonus_end_date > Me [ 0 ] . current_game_date )
    {
	if ( use_open_gl )
	{
	    blit_open_gl_texture_to_map_position ( power_flags [ target_time ] , 
						   Me [ 0 ] . pos . x , Me [ 0 ] . pos . y , 
						   1.0 , 1.0 , 1.0 ,
						   FALSE , FALSE );
	}
	else
	{
	    blit_iso_image_to_map_position ( power_flags [ target_time ] , 
					     Me [ 0 ] . pos . x , Me [ 0 ] . pos . y );
	}
    }
    else if ( Me [ 0 ] . dexterity_bonus_end_date > Me [ 0 ] . current_game_date )
    {
	if ( use_open_gl )
	{
	    blit_open_gl_texture_to_map_position ( dexterity_flags [ target_time ] , 
						   Me [ 0 ] . pos . x , Me [ 0 ] . pos . y , 
						   1.0 , 1.0 , 1.0 ,
						   FALSE , FALSE );
	}
	else
	{
	    blit_iso_image_to_map_position ( dexterity_flags [ target_time ] , 
					     Me [ 0 ] . pos . x , Me [ 0 ] . pos . y );
	}
    }

}; // void blit_tux_status_flags ( void )

/* ----------------------------------------------------------------------
 * This function displays an item at the current mouse cursor position.
 * The typical crosshair cursor is assumed.  The item is centered around
 * this crosshair cursor, depending on item size.
 * ---------------------------------------------------------------------- */
void
DisplayItemImageAtMouseCursor( int ItemImageCode )
{
    SDL_Rect TargetRect;

    if ( ItemImageCode == (-1) )
    {
	DebugPrintf( 2 , "\nCurrently no (-1 code) item held in hand.");
	return;
    }
    
    //--------------------
    // We define the target location for the item.  This will be the current
    // mouse cursor position of course, but -16 for the crosshair center, 
    // which is somewhat (16) to the lower right of the cursor top left 
    // corner.
    //
    // And then of course we also have to take into account the size of the
    // item, wich is also not always the same.
    //
    TargetRect.x = GetMousePos_x()  - ItemMap [ ItemImageCode ] . inv_image . inv_size . x * 16;
    TargetRect.y = GetMousePos_y()  - ItemMap [ ItemImageCode ] . inv_image . inv_size . y * 16;
    
    our_SDL_blit_surface_wrapper( ItemMap [ ItemImageCode ] . inv_image . Surface , 
				  NULL , Screen , &TargetRect );

}; // void DisplayItemImageAtMouseCursor( int ItemImageCode )

/* ----------------------------------------------------------------------
 * This function displays (several) blinking warning signs as soon as item
 * durations reach critical (<5) duration level.
 * ---------------------------------------------------------------------- */
void
ShowOneItemAlarm( item* AlarmItem , int Position )
{
    SDL_Rect TargetRect;
    int ItemImageCode;
    
    if ( AlarmItem->type == ( -1 ) ) return;
    if ( AlarmItem->max_duration == ( -1 ) ) return;
    
    ItemImageCode = AlarmItem -> type ;
    
    TargetRect . x = GameConfig . screen_width - 64 * Position ;
    TargetRect . y = 10 ;
    
    if ( AlarmItem->current_duration <= 5 )
    {
	if (  AlarmItem->current_duration < 3 )
	    if ( ( ( int ) ( Me[0].MissionTimeElapsed * 2 ) ) % 2 == 1 ) return;

	if(use_open_gl)
		{
		glPixelTransferf(GL_BLUE_SCALE, 0);
		glPixelTransferf(GL_GREEN_SCALE, (float)( AlarmItem->current_duration - 1 ) / ( 4 ) );
		glPixelTransferf(GL_RED_SCALE, 1);
		}
	our_SDL_blit_surface_wrapper( ItemMap [ ItemImageCode ] . inv_image . Surface , NULL , Screen , &TargetRect );
	if(use_open_gl)
		{
		glPixelTransferf(GL_BLUE_SCALE, 1);
		glPixelTransferf(GL_GREEN_SCALE, 1);
		glPixelTransferf(GL_RED_SCALE, 1);
		}

    }
}; // void ShowOneItemAlarm( item* AlarmItem )

/* ----------------------------------------------------------------------
 * This function displays (several) blinking warning signs as soon as item
 * durations reach critical (<5) duration level.
 * ---------------------------------------------------------------------- */
void
ShowItemAlarm( void )
{

    ShowOneItemAlarm( & Me[0].weapon_item , 1 );
    ShowOneItemAlarm( & Me[0].drive_item , 2 );
    ShowOneItemAlarm( & Me[0].shield_item , 3 );
    ShowOneItemAlarm( & Me[0].armour_item , 4 );
    ShowOneItemAlarm( & Me[0].special_item , 5 );
    
}; // void ShowItemAlarm( void )

/* ----------------------------------------------------------------------
 * Now it's time to blit all the spell effects.
 * ---------------------------------------------------------------------- */
void
PutMiscellaneousSpellEffects ( void )
{
    int i;

    //--------------------
    // This is here for some debugging/testing purpose
    //
    // long Ticks = SDL_GetTicks ( );
    // PutRadialBlueSparks( 15.0 , 15.0 , (float) ( Ticks % 10000 ) / 500.0 );
    
    //--------------------
    // Now we put all the spells in the list of active spells
    //
    for ( i = 0 ; i < MAX_ACTIVE_SPELLS; i ++ )
    {
	if ( AllActiveSpells [ i ] . type == (-1) ) continue;
	else if ( AllActiveSpells [ i ] . type == SPELL_RADIAL_EMP_WAVE ) 
	{
	    PutRadialBlueSparks( AllActiveSpells [ i ] . spell_center . x , 
				 AllActiveSpells [ i ] . spell_center . y , 
				 AllActiveSpells [ i ] . spell_radius , 0 ,
				 AllActiveSpells [ i ] . active_directions );
	}
	else if ( AllActiveSpells [ i ] . type == SPELL_RADIAL_VMX_WAVE ) 
	{
	    PutRadialBlueSparks( AllActiveSpells [ i ] . spell_center . x , 
				 AllActiveSpells [ i ] . spell_center . y , 
				 AllActiveSpells [ i ] . spell_radius , 1 ,
				 AllActiveSpells [ i ] . active_directions );
	}
	else if ( AllActiveSpells [ i ] . type == SPELL_RADIAL_FIRE_WAVE ) 
	{
	    PutRadialBlueSparks( AllActiveSpells [ i ] . spell_center . x , 
				 AllActiveSpells [ i ] . spell_center . y , 
				 AllActiveSpells [ i ] . spell_radius , 2 ,
				 AllActiveSpells [ i ] . active_directions );
	}
	else
	{
	    fprintf( stderr, "\n\nAllActiveSpells [ i ] . type: '%d'\n" , AllActiveSpells [ i ] . type );
	    GiveStandardErrorMessage ( __FUNCTION__ , "\
There was a bogus spell type entry found in the active spell list.",
				       PLEASE_INFORM, IS_FATAL );
	}
    }
    
}; // void PutMiscellaneousSpellEffects ( void )

/* ----------------------------------------------------------------------
 * The combat window can contain also some written text, displaying things
 * like the current energy level, current position and that.  This function
 * puts exactly those texts in fine print onto the Screen.
 * ---------------------------------------------------------------------- */
void
ShowCombatScreenTexts ( int mask )
{
    static float TimeSinceLastFPSUpdate=10;
    static int FPS_Displayed=1;
#define UPDATE_FPS_HOW_OFTEN 0.75
    Level DisplayLevel = curShip.AllLevels [ Me [ 0 ] . pos . z ] ;
    int minutes;
    int seconds;
    int i;
    int k;
    int remaining_bots;
    
    if ( GameConfig.Draw_Framerate )
    {
	TimeSinceLastFPSUpdate += Frame_Time();
	if ( TimeSinceLastFPSUpdate > UPDATE_FPS_HOW_OFTEN )
	{
	    if ( Frame_Time() > 0 )
		FPS_Displayed=(int)(1.0/Frame_Time());
	    else
		FPS_Displayed=(int)9999;
	    TimeSinceLastFPSUpdate=0;
	    
	    // DebugPrintf ( -2 , "\nFPS_Displayed: %d. " , FPS_Displayed );
	    
	}
      
	PrintStringFont( Screen , FPS_Display_BFont , User_Rect.x , 1,
			 "FPS: %d " , FPS_Displayed );
	
	// PrintStringFont( Screen , FPS_Display_BFont , User_Rect.x + 100, 
	// User_Rect.y+User_Rect.h - FontHeight( FPS_Display_BFont ), 
	// "Axis: %d %d" , input_axis.x, input_axis.y);
    }
    
    if ( GameConfig.Draw_Energy )
    {
	PrintStringFont( Screen , FPS_Display_BFont , User_Rect.x , 1 + 1 * FontHeight( FPS_Display_BFont ), 
			 "Energy: %d " , (int) (Me[0].energy) );
	// PrintStringFont( Screen , FPS_Display_BFont , User_Rect.x+User_Rect.w/2 , 
	// User_Rect.y+User_Rect.h - 2 * FontHeight( FPS_Display_BFont ), 
	// "Resistance: %f " , (Me[0].Current_Victim_Resistance_Factor) );
    }
    
    if ( GameConfig.Draw_Position || ( mask & ONLY_SHOW_MAP_AND_TEXT ) )
    {
	PrintStringFont( Screen , FPS_Display_BFont , User_Rect.x , 
			 1 + 2 * FontHeight( FPS_Display_BFont ), 
			 "GPS: X=%3.1f Y=%3.1f Lev=%d" , ( Me [ 0 ] . pos . x ) , ( Me [ 0 ] . pos . y ) , DisplayLevel->levelnum );
    }
    
    for ( i = 0 ; i < MAX_MISSIONS_IN_GAME ; i ++ )
    {
	if ( ! Me [ 0 ] . AllMissions [ i ] . MissionWasAssigned ) continue;
	
	DebugPrintf ( 0 , "\nYES, Something was assigned at all...." );
	
	if ( Me [ 0 ] . AllMissions [ i ] . MustLiveTime != (-1) )
	{
	    minutes = floor ( ( Me [ 0 ] . AllMissions [ i ] . MustLiveTime - Me [ 0 ] . MissionTimeElapsed ) / 60 );
	    seconds = rintf ( Me [ 0 ] . AllMissions [ i ] . MustLiveTime - Me [ 0 ] . MissionTimeElapsed ) - 60 * minutes;
	    if ( minutes < 0 ) 
	    {
		minutes = 0;
		seconds = 0;
	    }
	    PrintStringFont( Screen , FPS_Display_BFont , User_Rect.x , 
			     User_Rect.y + 0*FontHeight( FPS_Display_BFont ), 
			     "Time to hold out still: %2d:%2d " , minutes , seconds );
	}
	
	if ( ( Me [ 0 ] . AllMissions [ i ] . must_clear_first_level == Me [ 0 ] . pos . z ) ||
	     ( Me [ 0 ] . AllMissions [ i ] . must_clear_second_level == Me [ 0 ] . pos . z ) )
	{
	    remaining_bots = 0 ;
	    
	    // for ( k = 0 ; k < Number_Of_Droids_On_Ship ; k ++ )
	    for ( k = 0 ; k < MAX_ENEMYS_ON_SHIP ; k ++ )
	    {
		if ( ( AllEnemys [ k ] . pos . z == Me [ 0 ] . pos . z ) &&
		     ( AllEnemys [ k ] . Status != INFOUT ) &&
		     ( AllEnemys [ k ] . energy > 0 ) &&
		     ( ! AllEnemys [ k ] . is_friendly ) )
		    remaining_bots ++ ;
	    }
	    PrintStringFont( Screen , FPS_Display_BFont , User_Rect.x , 
			     User_Rect.y + 0*FontHeight( FPS_Display_BFont ), 
			     "Bots remaining on level: %d" , remaining_bots );
	    DebugPrintf ( 0 , "\nYES, this is the level...." );
	}
    }
    
    classic_show_mission_list ( );

    DisplayBigScreenMessage( );

}; // void ShowCombatScreenTexts ( int mask )

/* ----------------------------------------------------------------------
 * When blitting the floor to the screen, we can of course use the map
 * position of each map tile to compute the right position.  That is what
 * the blit_this_floor_tile_to_screen(...) function does.
 * ---------------------------------------------------------------------- */
void
blit_this_floor_tile_to_screen ( iso_image our_floor_iso_image ,
				 float our_col, float our_line )
{
    if ( use_open_gl )
    {
	blit_open_gl_texture_to_map_position ( our_floor_iso_image , our_col , our_line , 1.0 , 1.0 , 1.0 , FALSE , FALSE) ;
    }
    else
    {
	blit_iso_image_to_map_position ( our_floor_iso_image , our_col , our_line ) ;
    }
}; // void blit_this_floor_tile_to_screen ( iso_image our_floor_iso_image , float our_col, float our_line )

/* ----------------------------------------------------------------------
 * This function should assemble the pure floor tiles that will be visible
 * around the Tux or in the console map view.  Big map inserts and all that
 * will be handled later...
 * ---------------------------------------------------------------------- */
void
isometric_show_floor_around_tux_without_doublebuffering ( int mask )
{
    int LineStart, LineEnd, ColStart, ColEnd , line, col, MapBrick;
    Level DisplayLevel = curShip.AllLevels [ Me [ 0 ] . pos . z ] ;

    //--------------------
    // Maybe we should be using a more elegant function here, that will automatically
    // compute the right amount of squares to blit in each direction from the known amount
    // of pixel one floor tile takes...  But that must follow later...
    if ( mask & ZOOM_OUT )
    {
	LineStart = Me [ 0 ] . pos . y - FLOOR_TILES_VISIBLE_AROUND_TUX * LEVEL_EDITOR_ZOOM_OUT_FACT ;
	LineEnd = Me [ 0 ] . pos . y + FLOOR_TILES_VISIBLE_AROUND_TUX * LEVEL_EDITOR_ZOOM_OUT_FACT ;
	ColStart = Me [ 0 ] . pos . x - FLOOR_TILES_VISIBLE_AROUND_TUX * LEVEL_EDITOR_ZOOM_OUT_FACT ;
	ColEnd = Me [ 0 ] . pos . x + FLOOR_TILES_VISIBLE_AROUND_TUX * LEVEL_EDITOR_ZOOM_OUT_FACT ;
    }
    else
    {
	LineStart = Me [ 0 ] . pos . y - FLOOR_TILES_VISIBLE_AROUND_TUX ;
	LineEnd = Me [ 0 ] . pos . y + FLOOR_TILES_VISIBLE_AROUND_TUX ;
	ColStart = Me [ 0 ] . pos . x - FLOOR_TILES_VISIBLE_AROUND_TUX ;
	ColEnd = Me [ 0 ] . pos . x + FLOOR_TILES_VISIBLE_AROUND_TUX ;
    }
    
    SDL_SetClipRect (Screen , &User_Rect);
    
    if ( mask & ZOOM_OUT )
    {
	for (line = LineStart; line < LineEnd; line++)
	{
	    for (col = ColStart; col < ColEnd; col++)
	    {
		MapBrick = GetMapBrick ( DisplayLevel, col , line ) ;
		
		if ( use_open_gl )
		{
		    blit_zoomed_open_gl_texture_to_map_position ( floor_iso_images [ MapBrick % ALL_ISOMETRIC_FLOOR_TILES ] , 
								  ((float)col)+0.5 , ((float)line) +0.5 , 1.0 , 1.0 , 1.0 , FALSE , FALSE);
		}
		else
		{
		    blit_zoomed_iso_image_to_map_position ( & ( floor_iso_images [ MapBrick % ALL_ISOMETRIC_FLOOR_TILES ] ) , ((float)col)+0.5 , ((float)line) +0.5 ) ;
		}
	    }		// for(col) 
	}		// for(line) 
    }
    else
    {
	for ( line = LineStart ; line < LineEnd ; line++ )
	{
	    for ( col = ColStart ; col < ColEnd ; col++ )
	    {
		MapBrick = GetMapBrick ( DisplayLevel, col , line ) ;
		blit_this_floor_tile_to_screen ( floor_iso_images [ MapBrick % ALL_ISOMETRIC_FLOOR_TILES ] , 
		((float)col)+0.5 , ((float)line) +0.5 );
	    }		// for(col) 
	}		// for(line) 
    }
}; // void isometric_show_floor_around_tux_without_doublebuffering ( int mask )


/* ----------------------------------------------------------------------
 * More for debugging purposes than for real gameplay, we add some 
 * function to illustrate the collision rectangle of a certain obstacle
 * on the floor via a bright ugly distorted rectangular shape.
 * ---------------------------------------------------------------------- */
void 
blit_obstacle_collision_rectangle ( obstacle* our_obstacle )
{
    int x1, x2, x3, x4, y1, y2, y3, y4 ;

    //--------------------
    // If collision rectangles are turned off, then we need not do 
    // anything more here...
    //
    if ( ! draw_collision_rectangles ) return;

    //--------------------
    // If there is no collision rectangle to draw, we are done
    //
    if ( obstacle_map [ our_obstacle -> type ] . block_area_type == COLLISION_TYPE_NONE )
	return;

    //--------------------
    // Now we draw the collision rectangle.  We use the same parameters
    // of the obstacle spec, that are also used for the collision checks.
    //
    x1 = translate_map_point_to_screen_pixel ( 
	our_obstacle -> pos . x + obstacle_map [ our_obstacle -> type ] . upper_border , 
	our_obstacle -> pos . y + obstacle_map [ our_obstacle -> type ] . left_border , TRUE );
    y1 = translate_map_point_to_screen_pixel ( 
	our_obstacle -> pos . x + obstacle_map [ our_obstacle -> type ] . upper_border , 
	our_obstacle -> pos . y + obstacle_map [ our_obstacle -> type ] . left_border , FALSE );
    x2 = translate_map_point_to_screen_pixel ( 
	our_obstacle -> pos . x + obstacle_map [ our_obstacle -> type ] . upper_border , 
	our_obstacle -> pos . y + obstacle_map [ our_obstacle -> type ] . right_border , TRUE );
    y2 = translate_map_point_to_screen_pixel ( 
	our_obstacle -> pos . x + obstacle_map [ our_obstacle -> type ] . upper_border , 
	our_obstacle -> pos . y + obstacle_map [ our_obstacle -> type ] . right_border , FALSE );
    x3 = translate_map_point_to_screen_pixel ( 
	our_obstacle -> pos . x + obstacle_map [ our_obstacle -> type ] . lower_border , 
	our_obstacle -> pos . y + obstacle_map [ our_obstacle -> type ] . right_border , TRUE );
    y3 = translate_map_point_to_screen_pixel ( 
	our_obstacle -> pos . x + obstacle_map [ our_obstacle -> type ] . lower_border , 
	our_obstacle -> pos . y + obstacle_map [ our_obstacle -> type ] . right_border , FALSE );
    x4 = translate_map_point_to_screen_pixel ( 
	our_obstacle -> pos . x + obstacle_map [ our_obstacle -> type ] . lower_border , 
	our_obstacle -> pos . y + obstacle_map [ our_obstacle -> type ] . left_border , TRUE );
    y4 = translate_map_point_to_screen_pixel ( 
	our_obstacle -> pos . x + obstacle_map [ our_obstacle -> type ] . lower_border , 
	our_obstacle -> pos . y + obstacle_map [ our_obstacle -> type ] . left_border , FALSE );
    blit_quad ( x1, y1, x2, y2, x3, y3, x4, y4, 0x00FEEAA );
}; // void blit_obstacle_collision_rectangle ( obstacle* our_obstacle )

/* ----------------------------------------------------------------------
 * This function should blit an obstacle, that is given via it's address
 * in the parameter
 * ---------------------------------------------------------------------- */
void
blit_one_obstacle ( obstacle* our_obstacle )
{
    iso_image tmp;
    double darkness ;
    moderately_finepoint obs_onscreen_position;

    if ( ( our_obstacle-> type <= (-1) ) || ( our_obstacle-> type >= NUMBER_OF_OBSTACLE_TYPES ) )
    {
	fprintf ( stderr , "\nobstacle_type found=%d." , our_obstacle-> type ) ;
	GiveStandardErrorMessage ( __FUNCTION__  , "\
There was an obstacle type given, that exceeds the number of\n\
 obstacle types allowed and loaded in Freedroid.",
				   PLEASE_INFORM, IS_FATAL );
	
    }

    //--------------------
    // Maybe the children friendly version is desired.  Then the blood on the floor
    // will not be blitted to the screen.
    //
    if ( ( ! GameConfig . show_blood ) && 
	 ( our_obstacle-> type >= ISO_BLOOD_1 ) && 
	 ( our_obstacle -> type <= ISO_BLOOD_8 ) ) 
	return;

    // blit_obstacle_collision_rectangle ( our_obstacle );


    //--------------------
    // We blit the obstacle in question, but if we're in the level editor and this
    // obstacle has been marked, we apply a color filter to it.  Otherwise we blit
    // it just so.
    //
    if ( our_obstacle == level_editor_marked_obstacle )
    {
	//--------------------
	// Maybe the obstacle in question is also subject to the mouse_move_mode
	// of the level editor.  In this case we blit the obstacle on the current
	// mouse cursor location instead.
	//
	if ( level_editor_mouse_move_mode )
	{
	    obs_onscreen_position . x = 
		translate_pixel_to_map_location ( 0 ,
						  (float) ServerThinksInputAxisX ( 0 ) , 
						  (float) ServerThinksInputAxisY ( 0 ) , TRUE ) ;
	    obs_onscreen_position . y = 
		translate_pixel_to_map_location ( 0 ,
						  (float) ServerThinksInputAxisX ( 0 ) , 
						  (float) ServerThinksInputAxisY ( 0 ) , FALSE ) ;
	}
	else
	{
	    obs_onscreen_position . x = our_obstacle -> pos . x ;
	    obs_onscreen_position . y = our_obstacle -> pos . y ;
	}

	if ( use_open_gl )
	{
	    blit_open_gl_texture_to_map_position ( obstacle_map [ our_obstacle -> type ] . image , 
						   obs_onscreen_position . x , obs_onscreen_position . y , 
						   ( SDL_GetTicks() % 3) / 2.0  , 
						   ( ( SDL_GetTicks() + 1 ) % 3) / 2.0 , 
						   ( ( SDL_GetTicks() + 2 ) % 3) / 2.0 , TRUE , FALSE) ;
	}
	else
	{
	    DebugPrintf ( 1 , "\nColor filter for level editor invoked (via SDL!) for marked obstacle!" );
	    tmp . surface = our_SDL_display_format_wrapperAlpha ( obstacle_map [ our_obstacle -> type ] . image . surface );
	    tmp . surface -> format -> Bmask = 0x0 ; // 0FFFFFFFF ;
	    tmp . surface -> format -> Rmask = 0x0 ; // FFFFFFFF ;
	    tmp . surface -> format -> Gmask = 0x0FFFFFFFF ;
	    tmp . offset_x = obstacle_map [ our_obstacle -> type ] . image . offset_x ;
	    tmp . offset_y = obstacle_map [ our_obstacle -> type ] . image . offset_y ;
	    blit_iso_image_to_map_position ( tmp , obs_onscreen_position . x , obs_onscreen_position . y );
	    SDL_FreeSurface ( tmp . surface );
	}
    }
    else
    {
	if ( use_open_gl )
	{
	    //--------------------
	    // Not in all cases does it make sense to make the walls transparent.
	    // Only those walls, that are really blocking the Tux from view should
	    // be made transparent.
	    //
	    if ( obstacle_map [ our_obstacle -> type ] . transparent == TRANSPARENCY_FOR_WALLS ) 
	    {
		if ( ( our_obstacle -> pos . x > Me [ 0 ] . pos . x - 1.0 ) &&
		     ( our_obstacle -> pos . y > Me [ 0 ] . pos . y - 1.0 ) &&
		     ( our_obstacle -> pos . x < 
		       Me [ 0 ] . pos . x + 1.5 ) &&
		     ( our_obstacle -> pos . y < 
		       Me [ 0 ] . pos . y + 1.5 ) )
		{
				float locx, locy;
		float endlocx, endlocy;
		locx = translate_map_point_to_screen_pixel_deviation_tracking( our_obstacle -> pos . x, our_obstacle->pos.y, TRUE);
		locy = translate_map_point_to_screen_pixel_deviation_tracking( our_obstacle -> pos . x, our_obstacle->pos.y, FALSE);
		endlocx = (locx - UserCenter_x) / (float) iso_floor_tile_width + Me[0].pos.x + (locy - UserCenter_y ) / (float) iso_floor_tile_height;
		endlocy = (- locx + UserCenter_x) / (float) iso_floor_tile_width + Me[0].pos.y + (locy - UserCenter_y) / (float) iso_floor_tile_height;
		blit_open_gl_texture_to_map_position ( 
		    obstacle_map [ our_obstacle -> type ] . image , endlocx, endlocy, 1,1,1 , FALSE, 
		    obstacle_map [ our_obstacle -> type ] . transparent ) ;

		}
		else
		{
		float locx, locy;
		float endlocx, endlocy;
		locx = translate_map_point_to_screen_pixel_deviation_tracking( our_obstacle -> pos . x, our_obstacle->pos.y, TRUE);
		locy = translate_map_point_to_screen_pixel_deviation_tracking( our_obstacle -> pos . x, our_obstacle->pos.y, FALSE);
		endlocx = (locx - UserCenter_x) / (float) iso_floor_tile_width + Me[0].pos.x + (locy - UserCenter_y ) / (float) iso_floor_tile_height;
		endlocy = (- locx + UserCenter_x) / (float) iso_floor_tile_width + Me[0].pos.y + (locy - UserCenter_y) / (float) iso_floor_tile_height;
		blit_open_gl_texture_to_map_position ( 
		    obstacle_map [ our_obstacle -> type ] . image , endlocx, endlocy, 1,1,1 , FALSE, 
		    0 ) ;

		}
	    }
	    else
	    {
		float locx, locy;
		float endlocx, endlocy;
		locx = translate_map_point_to_screen_pixel_deviation_tracking( our_obstacle -> pos . x, our_obstacle->pos.y, TRUE);
		locy = translate_map_point_to_screen_pixel_deviation_tracking( our_obstacle -> pos . x, our_obstacle->pos.y, FALSE);
		endlocx = (locx - UserCenter_x) / (float) iso_floor_tile_width + Me[0].pos.x + (locy - UserCenter_y ) / (float) iso_floor_tile_height;
		endlocy = (- locx + UserCenter_x) / (float) iso_floor_tile_width + Me[0].pos.y + (locy - UserCenter_y) / (float) iso_floor_tile_height;
		blit_open_gl_texture_to_map_position ( 
		    obstacle_map [ our_obstacle -> type ] . image , endlocx, endlocy, 1,1,1 , FALSE, 
		    obstacle_map [ our_obstacle -> type ] . transparent ) ;
	    }
	}
	else
	{
	    float locx, locy;
            float endlocx, endlocy;
            locx = translate_map_point_to_screen_pixel_deviation_tracking( our_obstacle -> pos . x, our_obstacle->pos.y, TRUE);
            locy = translate_map_point_to_screen_pixel_deviation_tracking( our_obstacle -> pos . x, our_obstacle->pos.y, FALSE);
            endlocx = (locx - UserCenter_x) / (float) iso_floor_tile_width + Me[0].pos.x + (locy - UserCenter_y ) / (float) iso_floor_tile_height;
            endlocy = (- locx + UserCenter_x) / (float) iso_floor_tile_width + Me[0].pos.y + (locy - UserCenter_y) / (float) iso_floor_tile_height;

	    blit_iso_image_to_map_position ( obstacle_map [ our_obstacle -> type ] . image , 
					     endlocx , endlocy );
	}
    }


}; // blit_one_obstacle ( obstacle* our_obstacle )

/* ----------------------------------------------------------------------
 * This function should blit an obstacle, that is given via it's address
 * in the parameter
 * ---------------------------------------------------------------------- */
void
blit_one_obstacle_highlighted ( obstacle* our_obstacle )
{

    if ( ( our_obstacle-> type <= (-1) ) || ( our_obstacle-> type >= NUMBER_OF_OBSTACLE_TYPES ) )
    {
	GiveStandardErrorMessage ( __FUNCTION__  , "\
There was an obstacle type given, that exceeds the number of\n\
 obstacle types allowed and loaded in Freedroid.",
				   PLEASE_INFORM, IS_FATAL );
	
    }
    
    //--------------------
    // Maybe the children friendly version is desired.  Then the blood on the floor
    // will not be blitted to the screen.
    //
    if ( ( ! GameConfig . show_blood ) && 
	 ( our_obstacle-> type >= ISO_BLOOD_1 ) && 
	 ( our_obstacle -> type <= ISO_BLOOD_8 ) ) 
	return;
    
    if ( use_open_gl )
    {
	blit_open_gl_texture_to_map_position ( obstacle_map [ our_obstacle -> type ] . image , 
					       our_obstacle -> pos . x , our_obstacle -> pos . y , 1.0 , 1.0 , 1.0 , TRUE, FALSE ) ;
    }
    else
    {
	DebugPrintf ( 0 , "\nNormal in-game SDL highlight invoked for marked obstacle!" );
	blit_iso_image_to_map_position ( obstacle_map [ our_obstacle -> type ] . image , 
					 our_obstacle -> pos . x , our_obstacle -> pos . y );
	blit_outline_of_iso_image_to_map_position ( obstacle_map [ our_obstacle -> type ] . image , 
						    our_obstacle -> pos . x , our_obstacle -> pos . y );
    }
    
}; // blit_one_obstacle_highlighted ( obstacle* our_obstacle )

/* ----------------------------------------------------------------------
 * This function should blit an obstacle, that is given via it's address
 * in the parameter
 * ---------------------------------------------------------------------- */
void
blit_one_obstacle_zoomed ( obstacle* our_obstacle )
{
    iso_image tmp;
    // DebugPrintf ( 0 , "\nObstacle to be blitted: type=%d x=%f y=%f." , our_obstacle -> type ,
    // our_obstacle -> pos . x , our_obstacle -> pos . y );
    
    if ( ( our_obstacle-> type <= (-1) ) || ( our_obstacle-> type >= NUMBER_OF_OBSTACLE_TYPES ) )
    {
	GiveStandardErrorMessage ( __FUNCTION__  , "\
There was an obstacle type given, that exceeds the number of\n\
 obstacle types allowed and loaded in Freedroid.",
				   PLEASE_INFORM, IS_FATAL );
    }
    
    if ( ! use_open_gl )
	make_sure_zoomed_surface_is_there ( & ( obstacle_map [ our_obstacle -> type ] . image ) );

    //--------------------
    // We blit the obstacle in question, but if we're in the level editor and this
    // obstacle has been marked, we apply a color filter to it.  Otherwise we blit
    // it just so.
    //
    if ( our_obstacle == level_editor_marked_obstacle )
    {
	
	if ( use_open_gl )
	{
	    blit_zoomed_open_gl_texture_to_map_position ( obstacle_map [ our_obstacle -> type ] . image ,
							  our_obstacle -> pos . x , our_obstacle -> pos . y , 
							  1.0 , 1.0, 1.0 , 0.25, FALSE );
	}
	else
	{
	    DebugPrintf ( 0 , "\nCOLOR FILTER INVOKED FOR MARKED OBSTACLE!" );
	    tmp . surface = our_SDL_display_format_wrapperAlpha ( obstacle_map [ our_obstacle -> type ] . image . surface );
	    tmp . surface -> format -> Bmask = 0x0 ; // 0FFFFFFFF ;
	    tmp . surface -> format -> Rmask = 0x0 ; // FFFFFFFF ;
	    tmp . surface -> format -> Gmask = 0x0FFFFFFFF ;
	    tmp . offset_x = obstacle_map [ our_obstacle -> type ] . image . offset_x ;
	    tmp . offset_y = obstacle_map [ our_obstacle -> type ] . image . offset_y ;
	    tmp . zoomed_out_surface = NULL ;
	    // obstacle_map [ our_obstacle -> type ] . image . surface -> format -> Gmask = 0x0FFFFFFFF ;
	    // SDL_UnlockSurface ( obstacle_map [ our_obstacle -> type ] . image . surface );
	    blit_zoomed_iso_image_to_map_position ( & ( tmp ) , 
						    our_obstacle -> pos . x , our_obstacle -> pos . y );
	    SDL_FreeSurface ( tmp . surface );
	    SDL_FreeSurface ( tmp . zoomed_out_surface );
	    // SDL_LockSurface ( obstacle_map [ our_obstacle -> type ] . image . surface );
	    // obstacle_map [ our_obstacle -> type ] . image . surface -> format -> Bmask = temp_Bmask ; 
	    // obstacle_map [ our_obstacle -> type ] . image . surface -> format -> Rmask = temp_Rmask ; 
	    // SDL_UnlockSurface ( obstacle_map [ our_obstacle -> type ] . image . surface );
	}
    }
    else
    {
	if ( use_open_gl )
	{
	    blit_zoomed_open_gl_texture_to_map_position ( obstacle_map [ our_obstacle -> type ] . image ,
							  our_obstacle -> pos . x , our_obstacle -> pos . y , 1.0 , 1.0, 1.0 , 0.25, obstacle_map[our_obstacle->type].transparent  );
	}
	else
	{
	    blit_zoomed_iso_image_to_map_position ( & ( obstacle_map [ our_obstacle -> type ] . image ) , 
						    our_obstacle -> pos . x , our_obstacle -> pos . y );
	}
    }
}; // blit_one_obstacle_zoomed ( obstacle* our_obstacle )

/* ----------------------------------------------------------------------
 * In order for the obstacles to be blitted, they must first be inserted
 * into the correctly ordered list of objects to be blitted this frame.
 * ---------------------------------------------------------------------- */
void
insert_obstacles_into_blitting_list ( int mask )
{
    int i;
    level* obstacle_level = curShip . AllLevels [ Me [ 0 ] . pos . z ];
    int LineStart, LineEnd, ColStart, ColEnd , line, col;
    obstacle* OurObstacle;
    
    //--------------------
    // There are literally THOUSANDS of obstacles on some levels.
    // Therefore we will not blit each and every one of them, but only those
    // that are glued to one of the map tiles in the local area...
    // That should give us some better performance...
    //
    
    //--------------------
    // We select the following area to be the map excerpt, that can be
    // visible at most.  This is nescessare now that the Freedroid RPG is
    // going to have larger levels and we don't want to do 100x100 cyles
    // for nothing each frame.
    //
    if ( mask & ZOOM_OUT )
    {
	LineStart = Me [ 0 ] . pos . y - FLOOR_TILES_VISIBLE_AROUND_TUX * LEVEL_EDITOR_ZOOM_OUT_FACT ;
	LineEnd = Me [ 0 ] . pos . y + FLOOR_TILES_VISIBLE_AROUND_TUX * LEVEL_EDITOR_ZOOM_OUT_FACT ;
	ColStart = Me [ 0 ] . pos . x - FLOOR_TILES_VISIBLE_AROUND_TUX * LEVEL_EDITOR_ZOOM_OUT_FACT ;
	ColEnd = Me [ 0 ] . pos . x + FLOOR_TILES_VISIBLE_AROUND_TUX * LEVEL_EDITOR_ZOOM_OUT_FACT ;
    }
    else
    {
	LineStart = Me [ 0 ] . pos . y - FLOOR_TILES_VISIBLE_AROUND_TUX ;
	LineEnd = Me [ 0 ] . pos . y + FLOOR_TILES_VISIBLE_AROUND_TUX ;
	ColStart = Me [ 0 ] . pos . x - FLOOR_TILES_VISIBLE_AROUND_TUX ;
	ColEnd = Me [ 0 ] . pos . x + FLOOR_TILES_VISIBLE_AROUND_TUX ;
    }
    if ( LineStart < 0 ) LineStart = 0 ;
    if ( ColStart < 0 ) ColStart = 0 ;
    if ( LineEnd >= obstacle_level -> ylen ) LineEnd = obstacle_level -> ylen - 1 ;
    if ( ColEnd >= obstacle_level -> xlen ) ColEnd = obstacle_level -> xlen - 1 ;
    
    for (line = LineStart; line < LineEnd; line++)
    {
	for (col = ColStart; col < ColEnd; col++)
	{
	    for ( i = 0 ; i < MAX_OBSTACLES_GLUED_TO_ONE_MAP_TILE ; i ++ )
	    {
		if ( obstacle_level -> map [ line ] [ col ] . obstacles_glued_to_here [ i ] != (-1) )
		{
		    if ( i >= MAX_ELEMENTS_IN_BLITTING_LIST )
		    {
			GiveStandardErrorMessage ( __FUNCTION__  , "\
The blitting list size was exceeded!",
						   PLEASE_INFORM, IS_FATAL );
		    }
		    
		    //--------------------
		    // Now we have to insert this obstacle.  We do this of course respecting
		    // the blitting order, as always...
		    //
		    OurObstacle = & ( obstacle_level -> obstacle_list [ obstacle_level -> map [ line ] [ col ] . obstacles_glued_to_here [ i ] ] ) ;
		    insert_new_element_into_blitting_list ( OurObstacle -> pos . x + OurObstacle -> pos . y , 
							    BLITTING_TYPE_OBSTACLE , OurObstacle , obstacle_level -> map [ line ] [ col ] . obstacles_glued_to_here [ i ] ) ;
		}
		else
		    break;
	    }
	}
    }
    
}; // void insert_obstacles_into_blitting_list ( void )

/* ----------------------------------------------------------------------
 * Several different things must be inserted into the blitting list.
 * Therefore this function is an abstraction, that will insert a generic
 * object into the blitting list.
 * ---------------------------------------------------------------------- */
void
insert_new_element_into_blitting_list ( float new_element_norm , 
					int new_element_type , 
					void* new_element_pointer , 
					int code_number )
{
    int i;

    for ( i = 0 ; i < MAX_ELEMENTS_IN_BLITTING_LIST ; i ++ )
    {

	//--------------------
	// Maybe the new element is the last entry that will be added.  This case
	// must be handled specially.
	//
	if ( i == number_of_objects_currently_in_blitting_list )
	{
	    blitting_list [ number_of_objects_currently_in_blitting_list ] . norm_of_elements_position =
		new_element_norm ;
	    blitting_list [ number_of_objects_currently_in_blitting_list ] . element_type = new_element_type ;
	    blitting_list [ number_of_objects_currently_in_blitting_list ] . element_pointer = new_element_pointer ;
	    blitting_list [ number_of_objects_currently_in_blitting_list ] . code_number = code_number ;
	    number_of_objects_currently_in_blitting_list ++ ;
	    return;
	}

	//--------------------
	// In this case we know, that the New_Element insertion position is not the last
	// one and that therefore the other elements behind this position must be
	// moved along by one.
	//
	if ( new_element_norm < blitting_list [ i ] . norm_of_elements_position )
	{
	    //--------------------
	    // Before we can move the memory here, we must make sure that there
	    // is room for our shift.  So we'll check that now...
	    //
	    if ( i >= MAX_ELEMENTS_IN_BLITTING_LIST - 1 )
	    {
		GiveStandardErrorMessage ( __FUNCTION__  , "\
The blitting list size was exceeded!",
					   PLEASE_INFORM, IS_FATAL );
	    }
	    
	    //--------------------
	    // Note that we MUST NOT USE MEMCPY HERE BUT RATHER MUST USE MEM-MOVE!!
	    // See the GNU C Manual for details!
	    //
	    memmove ( & ( blitting_list [ i + 1 ] ) , & ( blitting_list [ i ] ) , 
		      sizeof ( blitting_list_element ) * ( number_of_objects_currently_in_blitting_list - i + 1 ) );
	    
	    //--------------------
	    // Now we can insert the New_Element in the new free position that we have 
	    // created.
	    //
	    blitting_list [ i ] . norm_of_elements_position =
		new_element_norm ;
	    blitting_list [ i ] . element_type = new_element_type ;
	    blitting_list [ i ] . element_pointer = new_element_pointer ;
	    blitting_list [ i ] . code_number = code_number ;
	    number_of_objects_currently_in_blitting_list ++ ;	  
	    return;
	}
    }
}; // void insert_new_element_into_blitting_list ( ... )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
insert_tux_into_blitting_list ( void )
{
    float tux_norm = Me [ 0 ] . pos . x + Me [ 0 ] . pos . y ;
    
    insert_new_element_into_blitting_list ( tux_norm , BLITTING_TYPE_TUX , NULL , -1 );
    
}; // void insert_tux_into_blitting_list ( void )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
insert_one_enemy_into_blitting_list ( int enemy_num )
{
    float enemy_norm;

    enemy_norm = AllEnemys [ enemy_num ] . virt_pos . x + AllEnemys [ enemy_num ] . virt_pos . y ;
    
    insert_new_element_into_blitting_list ( enemy_norm , BLITTING_TYPE_ENEMY , & ( AllEnemys [ enemy_num ] ) , enemy_num );
    
}; // void insert_one_enemy_into_blitting_list ( int enemy_num )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
insert_one_thrown_item_into_blitting_list ( int item_num )
{
    float item_norm;
    Level ItemLevel = curShip . AllLevels [ Me [ 0 ] . pos . z ] ;
    Item CurItem = &ItemLevel -> ItemList [ item_num ] ;
    
    item_norm = CurItem -> pos . x + CurItem -> pos . y ;
    
    insert_new_element_into_blitting_list ( item_norm , BLITTING_TYPE_THROWN_ITEM , CurItem , item_num );
    
    // fprintf ( stderr , "\nOne thrown item now inserted into blitting list. " );
    
}; // void insert_one_item_into_blitting_list ( int enemy_num )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
insert_one_bullet_into_blitting_list ( int bullet_num )
{
    float bullet_norm = AllBullets [ bullet_num ] . pos . x + AllBullets [ bullet_num ] . pos . y ;
    
    insert_new_element_into_blitting_list ( bullet_norm , BLITTING_TYPE_BULLET , 
					    & ( AllBullets [ bullet_num ] ) , bullet_num );
    
}; // void insert_one_bullet_into_blitting_list ( int enemy_num )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
insert_one_blast_into_blitting_list ( int blast_num )
{
    float blast_norm = AllBlasts [ blast_num ] . pos . x + AllBlasts [ blast_num ] . pos . y ;
    
    insert_new_element_into_blitting_list ( blast_norm , BLITTING_TYPE_BLAST , 
					    & ( AllBlasts [ blast_num ] ) , blast_num );
    
}; // void insert_one_blast_into_blitting_list ( int enemy_num )

/* ----------------------------------------------------------------------
 * We need to display bots that are on the current level or on one of the
 * levels glued to this one.
 * ---------------------------------------------------------------------- */
int
level_is_partly_visible ( int level_num )
{
    int current_tux_level = Me [ 0 ] . pos . z ;

    if ( level_num == current_tux_level ) return ( TRUE );
    if ( level_num == curShip . AllLevels [ current_tux_level ] -> jump_target_north ) return ( TRUE );
    if ( level_num == curShip . AllLevels [ current_tux_level ] -> jump_target_south ) return ( TRUE );
    if ( level_num == curShip . AllLevels [ current_tux_level ] -> jump_target_east ) return ( TRUE );
    if ( level_num == curShip . AllLevels [ current_tux_level ] -> jump_target_west ) return ( TRUE );

    return ( FALSE );

}; // int level_is_partly_visible ( int level_num )

/* ----------------------------------------------------------------------
 * The Tux can change onto other levels via jump thresholds.  This was an
 * important step for gluing together several maps into one big map.
 *
 * However, enemies will want to follow the Tux.  They should become 
 * visible when they are technically still on other levels.  Later they
 * should even become clickable and even later they might even move and
 * react to the Tux.
 *
 * However, from a technical point of view, this is becoming increasingly
 * unconvenient to handle.  Therefore we introduce 'virtual' positions,
 * i.e. the position the bot would have, if the bot were in fact counted
 * as part of a neighbouring level, mostly the level of the Tux.  Using
 * this concept, we can more easily compute distances and compare 
 * positions.
 *
 * This function is an abstract approach to this problem, working with
 * the 'gps' notion, such that later it might also be used in conjunction
 * with items or other stuff...
 *
 * ---------------------------------------------------------------------- */
void
update_virtual_position ( gps* target_pos , gps* source_pos , int level_num )
{
    int north_level, south_level, east_level, west_level;

    //--------------------
    // The case where the position in question is already directy on 
    // the virtual level, things are really simple and we can quit
    // almost immediately...
    //
    if ( source_pos -> z == level_num )
    {
	target_pos -> x = source_pos -> x ;
	target_pos -> y = source_pos -> y ;
	target_pos -> z = source_pos -> z ;
	return;
    }

    //--------------------
    // However, in case of a remote level, we need to compute a 
    // bit more...
    //
    north_level = curShip . AllLevels [ level_num ] -> jump_target_north ;
    south_level = curShip . AllLevels [ level_num ] -> jump_target_south ;
    east_level =  curShip . AllLevels [ level_num ] -> jump_target_east ;
    west_level =  curShip . AllLevels [ level_num ] -> jump_target_west ;
    
    if ( source_pos -> z == north_level )
    {
	target_pos -> z = level_num ;
	target_pos -> x = source_pos -> x ;
	target_pos -> y = source_pos -> y - curShip . AllLevels [ north_level ] -> ylen 
	    + curShip . AllLevels [ north_level ] -> jump_threshold_south ;
    }
    else if ( source_pos -> z == south_level )
    {
	target_pos -> z = level_num ;
	target_pos -> x = source_pos -> x ;
	target_pos -> y = source_pos -> y + curShip . AllLevels [ level_num ] -> ylen 
	    - curShip . AllLevels [ level_num ] -> jump_threshold_south ;

	// DebugPrintf ( -4 , "\n%s(): assigning virtual position to bot on southern map." , __FUNCTION__ );
    }
    else if ( source_pos -> z == east_level )
    {
	target_pos -> z = level_num ;
	target_pos -> y = source_pos -> y ;
	target_pos -> x = source_pos -> x + curShip . AllLevels [ level_num ] -> xlen 
	    - curShip . AllLevels [ level_num ] -> jump_threshold_east ;
    }
    else if ( source_pos -> z == west_level )
    {
	target_pos -> z = level_num ;
	target_pos -> y = source_pos -> y ;
	target_pos -> x = source_pos -> x - curShip . AllLevels [ west_level ] -> xlen 
	    + curShip . AllLevels [ west_level ] -> jump_threshold_east ;
    }
    else
    {
	//--------------------
	// In this case, we've reached the conclusion, that the position
	// in question cannot be expressed in terms of the virtual level.
	// That means we'll best 'erase' the virtual positions, so that
	// no 'phantoms' will occur...
	//
	target_pos -> x = (-1) ;
	target_pos -> y = (-1) ;
	target_pos -> z = (-1) ;
	return;
    }
    
}; // void update_virtual_position ( gps* target_pos , gps* source_pos , int level_num )

/* ----------------------------------------------------------------------
 * The blitting list must contain the enemies too.  This function is 
 * responsible for inserting the enemies at the right positions.
 * ---------------------------------------------------------------------- */
void
insert_enemies_into_blitting_list ( void )
{
    int i;
    enemy* ThisRobot;
    int level_num ;

    //--------------------
    // Now that we plan to also show bots on other levels, we must be
    // a bit more general and proceed through all the levels...
    //
    // Those levels not in question will be filtered out anyway inside
    // the loop...
    //
    for ( level_num = 0 ; level_num < MAX_LEVELS ; level_num ++ )
    {

	if ( ! level_is_partly_visible ( level_num ) ) continue ;

	for ( i = first_index_of_bot_on_level [ level_num ] ; 
	      i <= last_index_of_bot_on_level [ level_num ] ; i ++ )
	{
	    ThisRobot = & ( AllEnemys [ i ] ) ; 
	    
	    if ( ! level_is_partly_visible ( ThisRobot -> pos . z ) ) continue;

	    // if ( ThisRobot -> pos . z != Me [ 0 ] . pos . z )
	    // DebugPrintf ( -4 , "\n%s(): (possibly) inserting truly virtual bot..." , __FUNCTION__ );

	    if ( ( ThisRobot -> Status == INFOUT ) && ( ! Druidmap [ ThisRobot -> type ] . use_image_archive_file ) ) 
	    {
		// DebugPrintf ( -4 , "\n%s():  enemy blitting suppressed because of status and no animation..." , __FUNCTION__ );
		// continue;
	    }
	    
	    //--------------------
	    // We update the virtual position of this bot, such that we can handle it 
	    // with easier expressions later...
	    //
	    update_virtual_position ( & ( ThisRobot -> virt_pos ) ,
				      & ( ThisRobot -> pos ) , Me [ 0 ] . pos . z );

	    if ( fabsf ( ThisRobot -> virt_pos . x - Me [ 0 ] . pos . x ) > 
		 FLOOR_TILES_VISIBLE_AROUND_TUX + FLOOR_TILES_VISIBLE_AROUND_TUX ) continue;
	    if ( fabsf ( ThisRobot -> virt_pos . y - Me [ 0 ] . pos . y ) > 
		 FLOOR_TILES_VISIBLE_AROUND_TUX + FLOOR_TILES_VISIBLE_AROUND_TUX ) continue;

	    // if ( ThisRobot -> pos . z != Me [ 0 ] . pos . z )
	    // DebugPrintf ( -4 , "\n%s(): (possibly) inserting truly virtual bot..." , __FUNCTION__ );
	    
	    insert_one_enemy_into_blitting_list ( i );
	}
    }

}; // void insert_enemies_into_blitting_list ( void )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
insert_bullets_into_blitting_list ( void )
{
  int i;

  for ( i = 0 ; i < MAXBULLETS ; i ++ )
    {
      if ( AllBullets [ i ] . type != INFOUT )
	insert_one_bullet_into_blitting_list ( i );
    }
      
}; // void insert_bullets_into_blitting_list ( void )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
insert_blasts_into_blitting_list ( void )
{
    int i;
    
    for ( i = 0 ; i < MAXBLASTS ; i ++ )
    {
	if ( AllBlasts [ i ] . type != INFOUT )
	    insert_one_blast_into_blitting_list ( i );
    }
    
}; // void insert_enemies_into_blitting_list ( void )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
insert_thrown_items_into_blitting_list ( void )
{
    int i;
    Level ItemLevel = curShip . AllLevels [ Me [ 0 ] . pos . z ] ;
    Item CurItem = &ItemLevel -> ItemList [ 0 ] ;
    
    for ( i = 0 ; i < MAX_ITEMS_PER_LEVEL ; i ++ )
    {
	if ( CurItem -> throw_time > 0 )
	    insert_one_thrown_item_into_blitting_list ( i );
	CurItem ++ ;
    }
    
}; // void insert_enemies_into_blitting_list ( void )

/* ----------------------------------------------------------------------
 * In isometric viewpoint setting, we need to respect visibility when
 * considering the order of things to blit.  Therefore we will first set
 * up a list of the things to be blitted for this frame.  Then we can
 * later use this list to fill in objects into the picture, automatically
 * having the right order.
 * ---------------------------------------------------------------------- */
void
set_up_ordered_blitting_list ( int mask )
{
    //--------------------
    // First we need to clear out the blitting list.  We do this using
    // memset for optimal performance...
    //
    memset ( & ( blitting_list [ 0 ] ) , 0 , sizeof ( blitting_list_element ) * MAX_ELEMENTS_IN_BLITTING_LIST );
    number_of_objects_currently_in_blitting_list = 0 ;
    
    //--------------------
    // Now we can start to fill in the obstacles around the
    // tux...
    //
    insert_obstacles_into_blitting_list ( mask );
    
    insert_tux_into_blitting_list ();
    
    insert_enemies_into_blitting_list ( );
    
    insert_bullets_into_blitting_list ( ); 
    
    insert_blasts_into_blitting_list ( ); 
    
    insert_thrown_items_into_blitting_list ( ); 

}; // void set_up_ordered_blitting_list ( void )

/* ----------------------------------------------------------------------
 * Now that the blitting list has finally been assembled, we can start to
 * blit all the objects according to the blitting list set up.
 * ---------------------------------------------------------------------- */
void
blit_preput_objects_according_to_blitting_list ( int mask )
{
    int i;
    obstacle* our_obstacle = NULL;

    for ( i = 0 ; i < MAX_ELEMENTS_IN_BLITTING_LIST ; i ++ )
    {
	if ( blitting_list [ i ] . element_type == BLITTING_TYPE_NONE ) break;
	if ( blitting_list [ i ] . element_type == BLITTING_TYPE_OBSTACLE )
	{

	    //--------------------
	    // We do some sanity checking for illegal obstacle types.
	    // Can't hurt to do that so as to be on the safe side.
	    //
	    if ( ( ( (obstacle*)  blitting_list [ i ] . element_pointer ) -> type <= (-1) ) ||
		 ( (obstacle*)  blitting_list [ i ] . element_pointer ) -> type >= NUMBER_OF_OBSTACLE_TYPES )
	    {
		fprintf ( stderr , "\nerrorneous obstacle type to blit: %d." , 
			  ( (obstacle*)  blitting_list [ i ] . element_pointer ) -> type );
		GiveStandardErrorMessage ( __FUNCTION__  , 
					   "The blitting list contained an illegal blitting object type.",
					   PLEASE_INFORM, IS_FATAL );
	    }
	    
	    //--------------------
	    // If the obstacle has a shadow, it seems like now would be a good time
	    // to blit it.
	    //
	    if ( ! GameConfig . skip_shadow_blitting )
	    {
		our_obstacle = blitting_list [ i ] . element_pointer ;
		if ( use_open_gl )
		{
		    if ( obstacle_map [ our_obstacle -> type ] . shadow_image . texture_has_been_created )
		    {
			if ( mask & ZOOM_OUT )
			    blit_zoomed_open_gl_texture_to_map_position (   
                            obstacle_map [ our_obstacle -> type ] . shadow_image ,
                            our_obstacle -> pos . x , our_obstacle -> pos . y ,   
                            1.0 , 1.0, 1.0 , FALSE, TRANSPARENCY_FOR_SEE_THROUGH_OBJECTS );

			else blit_open_gl_texture_to_map_position ( 
			    obstacle_map [ our_obstacle -> type ] . shadow_image , 
			    our_obstacle -> pos . x , our_obstacle -> pos . y , 
			    1.0 , 1.0, 1.0 , FALSE, TRANSPARENCY_FOR_SEE_THROUGH_OBJECTS );
			// DebugPrintf ( -4 , "\n%s(): shadow has been drawn." , __FUNCTION__ );
		    }
		}
		else
		{
		    if ( obstacle_map [ our_obstacle -> type ] . shadow_image . surface != NULL )
		    {
			if ( mask & ZOOM_OUT )      blit_zoomed_iso_image_to_map_position ( & (obstacle_map [ our_obstacle -> type ] . shadow_image) ,
                                                         our_obstacle -> pos . x , our_obstacle -> pos . y );
			else blit_iso_image_to_map_position ( obstacle_map [ our_obstacle -> type ] . shadow_image , 
							 our_obstacle -> pos . x , our_obstacle -> pos . y );
			// DebugPrintf ( -4 , "\n%s(): shadow has been drawn." , __FUNCTION__ );
		    }
		}
	    }

	    //--------------------
	    // If the obstacle in question does have a collision rectangle, then we
	    // draw that on the floor now.
	    //
	    blit_obstacle_collision_rectangle ( our_obstacle );

	    //--------------------
	    // If the obstacle isn't otherwise a preput obstacle, we're done here and can 
	    // move on to the next list element
	    //
	    if ( ! obstacle_map [ ( (obstacle*)  blitting_list [ i ] . element_pointer ) -> type ] . needs_pre_put ) continue ;
	    
	    //--------------------
	    // So now we know that we must blit this one obstacle...
	    //
	    if ( ! ( mask & OMIT_OBSTACLES ) ) 
	    {
		if ( mask & ZOOM_OUT )
		    blit_one_obstacle_zoomed ( (obstacle*)  blitting_list [ i ] . element_pointer );
		else
		    blit_one_obstacle ( (obstacle*)  blitting_list [ i ] . element_pointer );
	    }
	}
	//--------------------
	// Enemies, which are dead already become like decoration on the floor.  
	// They should never obscur the Tux, so we blit them beforehand and not
	// again later from the list.
	//
	if ( ( blitting_list [ i ] . element_type == BLITTING_TYPE_ENEMY ) &&
	     // ( ( ( enemy* ) blitting_list [ i ] . element_pointer ) -> energy < 0 ) )
	     ( ( ( enemy* ) blitting_list [ i ] . element_pointer ) -> animation_type == DEATH_ANIMATION ) )
	{
	    if ( ! ( mask & OMIT_ENEMIES ) ) 
	    {
		PutEnemy ( blitting_list [ i ] . code_number , -1 , -1 , mask , FALSE ); 
	    }
	}
    }
    
}; // void blit_preput_objects_according_to_blitting_list ( ... )

/* ----------------------------------------------------------------------
 * Now that the blitting list has finally been assembled, we can start to
 * blit all the objects according to the blitting list set up.
 * ---------------------------------------------------------------------- */
void
blit_nonpreput_objects_according_to_blitting_list ( int mask )
{
    int i;
    int enemy_under_cursor = -1;
    int barrel_under_cursor = -1;
    int chest_under_cursor = -1;
    int item_under_cursor = -1; 
    
    //--------------------
    // We memorize which 'enemy' is currently under the mouse target, so that we
    // can properly highlight this enemy...
    //
    enemy_under_cursor = GetLivingDroidBelowMouseCursor ( 0 ) ;
    barrel_under_cursor = smashable_barrel_below_mouse_cursor ( 0 ) ;
    chest_under_cursor = closed_chest_below_mouse_cursor ( 0 ) ;
    item_under_cursor = get_floor_item_index_under_mouse_cursor ( 0 );
    
    //--------------------
    // Now it's time to blit all the elements from the list...
    //
    for ( i = 0 ; i < MAX_ELEMENTS_IN_BLITTING_LIST ; i ++ )
    {
	if ( blitting_list [ i ] . element_type == BLITTING_TYPE_NONE ) break;
	switch ( blitting_list [ i ] . element_type )
	{
	    case BLITTING_TYPE_OBSTACLE:
		
		//--------------------
		// We do some sanity checking for illegal obstacle types.
		// Can't hurt to do that so as to be on the safe side.
		//
		if ( ( ( (obstacle*)  blitting_list [ i ] . element_pointer ) -> type <= (-1) ) ||
		     ( (obstacle*)  blitting_list [ i ] . element_pointer ) -> type >= NUMBER_OF_OBSTACLE_TYPES )
		{
		    fprintf ( stderr , "\nerrorneous obstacle type to blit: %d." , 
			      ( (obstacle*)  blitting_list [ i ] . element_pointer ) -> type );
		    GiveStandardErrorMessage ( __FUNCTION__  , 
					       "The blitting list contained an illegal blitting object type.",
					       PLEASE_INFORM, IS_FATAL );
		}

		if ( obstacle_map [ ( (obstacle*)  blitting_list [ i ] . element_pointer ) -> type ] . needs_pre_put ) break ;

		if ( ! ( mask & OMIT_OBSTACLES ) ) 
		{
		    if ( mask & ZOOM_OUT )
			blit_one_obstacle_zoomed ( (obstacle*)  blitting_list [ i ] . element_pointer );
		    else
		    {
			if ( ( blitting_list [ i ] . code_number == barrel_under_cursor )  ||
			     ( blitting_list [ i ] . code_number == chest_under_cursor ) )
			    blit_one_obstacle_highlighted ( (obstacle*)  blitting_list [ i ] . element_pointer );
			else
			    blit_one_obstacle ( (obstacle*)  blitting_list [ i ] . element_pointer );
		    }
		}
		break;
	    case BLITTING_TYPE_TUX:
		if ( ! ( mask & OMIT_TUX ) ) 
		{
		    if ( Me [ 0 ] . energy > 0 )
			blit_tux ( -1 , -1 , 0 ); // this blits player 0 
		}
		break;
	    case BLITTING_TYPE_ENEMY:
		if ( ! ( mask & OMIT_ENEMIES ) ) 
		{
		    if ( ( ( enemy* ) blitting_list [ i ] . element_pointer ) -> energy < 0 )
			continue;
		    if ( ( ( enemy* ) blitting_list [ i ] . element_pointer ) -> animation_type == DEATH_ANIMATION )
			continue;
		    
		    //--------------------
		    // A droid can either be rendered in normal mode or in highlighted
		    // mode, depending in whether the mouse cursor is right over it or not.
		    //
		    if ( blitting_list [ i ] . code_number == enemy_under_cursor )
			PutEnemy ( blitting_list [ i ] . code_number , -1 , -1 , mask , TRUE ); 
		    else
			PutEnemy ( blitting_list [ i ] . code_number , -1 , -1 , mask , FALSE ); 
		}
		break;
	    case BLITTING_TYPE_BULLET:
		// DebugPrintf ( -1000 , "Bullet code_number: %d. " , blitting_list [ i ] . code_number );
		PutBullet ( blitting_list [ i ] . code_number , mask ); 
		break;
	    case BLITTING_TYPE_BLAST:
		if ( ! ( mask & OMIT_BLASTS ) )
		    PutBlast ( blitting_list [ i ] . code_number ); 
		break;
	    case BLITTING_TYPE_THROWN_ITEM:
		if ( item_under_cursor == blitting_list [ i ] . code_number )
		    PutItem ( blitting_list [ i ] . code_number , mask , PUT_ONLY_THROWN_ITEMS , TRUE ); 
		else
		    PutItem ( blitting_list [ i ] . code_number , mask , PUT_ONLY_THROWN_ITEMS , FALSE ); 
		
		// DebugPrintf ( -1 , "\nThrown item now blitted..." );
		break;
	    default:
		GiveStandardErrorMessage ( __FUNCTION__  , "\
The blitting list contained an illegal blitting object type.",
					   PLEASE_INFORM, IS_FATAL );
		break;
	}
    }
    
}; // void blit_nonpreput_objects_according_to_blitting_list ( ... )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void 
show_obstacle_labels ( int mask )
{
    int i;
    Level EditLevel = curShip . AllLevels [ Me [ 0 ] . pos . z ] ;
    
    if ( ! ( mask & SHOW_OBSTACLE_NAMES ) ) return;
    
    for ( i = 0 ; i < MAX_OBSTACLES_ON_MAP ; i ++ )
    {
	if ( EditLevel -> obstacle_list [ i ] . name_index >= 0 )
	{
	    show_backgrounded_label_at_map_position ( EditLevel -> obstacle_name_list [ EditLevel -> obstacle_list [ i ] . name_index ]  ,
						      0 , EditLevel -> obstacle_list [ i ] . pos . x , 
						      EditLevel -> obstacle_list [ i ] . pos . y ,
						      mask & ZOOM_OUT );
	}
    }

    //--------------------
    // Now that the obstacles labels are all displayed, we can start to 
    // display the obstacle descriptions.  Since those are larger and would
    // clutter up the screen pretty much if we drew them all, we'll just
    // confine ourselves to the currently marked obstacle and blit the
    // description of that one.
    //
    if ( level_editor_marked_obstacle != NULL )
    {
	if ( level_editor_marked_obstacle -> description_index >= 0 )
	{
	    //--------------------
	    // We do some extra security checks against non-present descriptions
	    //
	    if ( EditLevel -> obstacle_description_list [ level_editor_marked_obstacle -> description_index ] == NULL )
	    {
		GiveStandardErrorMessage ( __FUNCTION__  , "\
WARNING!  Null string for description found.  Deleting description index in question.",
					   NO_NEED_TO_INFORM, IS_WARNING_ONLY );
		level_editor_marked_obstacle -> description_index = (-1) ;
		return;
	    }
	    
	    show_backgrounded_text_rectangle ( EditLevel -> obstacle_description_list [ level_editor_marked_obstacle -> description_index ]  , 
					       translate_map_point_to_screen_pixel ( level_editor_marked_obstacle -> pos . x , level_editor_marked_obstacle -> pos . y , TRUE ) ,
					       translate_map_point_to_screen_pixel ( level_editor_marked_obstacle -> pos . x , level_editor_marked_obstacle -> pos . y , FALSE ) ,
					       320 , 240 ) ;
	    /*
	    show_backgrounded_label_at_map_position ( EditLevel -> obstacle_description_list [ level_editor_marked_obstacle -> description_index ]  ,
						      0 , level_editor_marked_obstacle -> pos . x , 
						      level_editor_marked_obstacle -> pos . y ,
						      mask & ZOOM_OUT );
	    */
	}
    }
    
    // show_backgrounded_label_at_map_position ( "This is a test" , 0 , Me [ 0 ] . pos . x + 1 , Me [ 0 ] . pos . y + 1 , mask & ZOOM_OUT );
    
}; // void show_obstacle_labels ( int mask )

/* ----------------------------------------------------------------------
 * Each item is lying on the floor.  But that means some of the items,
 * especially the smaller but not nescessary less valuable items will not
 * be easy to make out under all the bushed, trees, rubble and stuff.
 * So the solution is to offer a special key that when pressed will make
 * all item names flash up, so that you can't possibly miss an item that
 * you're standing close to.
 *
 * This function blits all the item names to the screen on the exact
 * positions that have been computed before (hopefully!) in other 
 * functions like update_item_text_slot_positions ( ... ) or so.
 * ---------------------------------------------------------------------- */
void
blit_all_item_slots ( void )
{
    int i;
    level* item_level = curShip . AllLevels [ Me [ 0 ] . pos . z ] ;

    for ( i = 0 ; i < MAX_ITEMS_PER_LEVEL ; i ++ )
    {
	//--------------------
	// We don't work with unused item slots...
	//
	if ( item_level -> ItemList [ i ] . type == (-1) ) continue;

	//--------------------
	// Now we check if the cursor is on that slot, because then the
	// background of the slot will be highlighted...
	//
	if ( MouseCursorIsInRect ( & ( item_level -> ItemList [ i ] . text_slot_rectangle ) , 
				   GetMousePos_x ( )  , 
				   GetMousePos_y ( )  ) )
	    our_SDL_fill_rect_wrapper ( Screen , & ( item_level -> ItemList [ i ] . text_slot_rectangle ) , 
					SDL_MapRGB ( Screen->format , 0x000 , 0x000 , 0x099 ) );
	else
	{
	    if ( use_open_gl )
	    {
		GL_HighlightRectangle ( Screen , item_level -> ItemList [ i ] . text_slot_rectangle , 0 , 0 , 0 , BACKGROUND_TEXT_RECT_ALPHA );
	    }
	    else
	    {
		our_SDL_fill_rect_wrapper ( Screen , & ( item_level -> ItemList [ i ] . text_slot_rectangle ) , 
					    SDL_MapRGB ( Screen->format , 0x000 , 0x000 , 0x000 ) );
	    }
	}

	//--------------------
	// Finally it's time to insert the font into the item slot.  We
	// use the item name, but currently font color is not adapted for
	// special item properties...
	//
	PutStringFont ( Screen , FPS_Display_BFont , item_level -> ItemList [ i ] . text_slot_rectangle . x , 
			item_level -> ItemList [ i ] . text_slot_rectangle . y , 
			ItemMap [ item_level -> ItemList [ i ] . type ] . item_name );

    }
    
}; // void blit_all_item_slots ( void )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
int
item_slot_position_blocked ( item* given_item , int last_slot_to_check )
{
    int i;
    item* cur_item;
    level* item_level = curShip . AllLevels [ Me [ 0 ] . pos . z ] ;

    for ( i = 0 ; i < last_slot_to_check + 1 ; i ++ )
    {
	cur_item = & ( item_level -> ItemList [ i ] ) ;

	if ( cur_item -> type == (-1) )
	    continue;
	
	if ( MouseCursorIsInRect ( & ( cur_item -> text_slot_rectangle ) , 
				   given_item -> text_slot_rectangle . x ,
				   given_item -> text_slot_rectangle . y ) )
	{
	    return ( TRUE );
	}
	if ( MouseCursorIsInRect ( & ( cur_item -> text_slot_rectangle ) , 
				   given_item -> text_slot_rectangle . x ,
				   given_item -> text_slot_rectangle . y + FontHeight ( FPS_Display_BFont ) ) )
	{
	    return ( TRUE );
	}
	if ( MouseCursorIsInRect ( & ( cur_item -> text_slot_rectangle ) , 
				   given_item -> text_slot_rectangle . x + 
				   given_item -> text_slot_rectangle . w ,
				   given_item -> text_slot_rectangle . y ) )
	{
	    return ( TRUE );
	}
	if ( MouseCursorIsInRect ( & ( cur_item -> text_slot_rectangle ) , 
				   given_item -> text_slot_rectangle . x + 
				   given_item -> text_slot_rectangle . w,
				   given_item -> text_slot_rectangle . y + FontHeight ( FPS_Display_BFont ) ) )
	{
	    return ( TRUE );
	}
	if ( MouseCursorIsInRect ( & ( cur_item -> text_slot_rectangle ) , 
				   given_item -> text_slot_rectangle . x + 
				   given_item -> text_slot_rectangle . w / 2  ,
				   given_item -> text_slot_rectangle . y ) )
	{
	    return ( TRUE );
	}
	if ( MouseCursorIsInRect ( & ( cur_item -> text_slot_rectangle ) , 
				   given_item -> text_slot_rectangle . x + 
				   given_item -> text_slot_rectangle . w / 2 ,
				   given_item -> text_slot_rectangle . y + FontHeight ( FPS_Display_BFont ) ) )
	{
	    return ( TRUE );
	}
    }
    
    return ( FALSE );
}; // void item_slot_position_blocked ( int x , int y , int last_slot_to_check )

/* ----------------------------------------------------------------------
 * Each item is lying on the floor.  But that means some of the items,
 * especially the smaller but not nescessary less valuable items will not
 * be easy to make out under all the bushed, trees, rubble and stuff.
 * So the solution is to offer a special key that when pressed will make
 * all item names flash up, so that you can't possibly miss an item that
 * you're standing close to.
 *
 * This function computes the best rectangles and positions for such 
 * item names to flash up.
 * ---------------------------------------------------------------------- */
void
update_item_text_slot_positions ( void )
{
    int i;
    level* item_level = curShip . AllLevels [ Me [ 0 ] . pos . z ] ;
    BFont_Info* BFont_to_use = FPS_Display_BFont ;
    item* cur_item;

    for ( i = 0 ; i < MAX_ITEMS_PER_LEVEL ; i ++ )
    {
	cur_item = & ( item_level -> ItemList [ i ] ) ;

	if ( cur_item -> type == (-1) )
	    continue;
	
	//--------------------
	// We try to use a text rectangle that is close to the
	// actual item...
	//
	cur_item -> text_slot_rectangle . h = FontHeight ( BFont_to_use ) ;
	cur_item -> text_slot_rectangle . w = 
	    TextWidthFont ( BFont_to_use , ItemMap [ cur_item -> type ] . item_name );
	cur_item -> text_slot_rectangle . x = 
	    translate_map_point_to_screen_pixel ( 
		cur_item -> pos . x , 
		cur_item -> pos . y , TRUE ) - cur_item -> text_slot_rectangle . w / 2 ;
	cur_item -> text_slot_rectangle . y = 
	    translate_map_point_to_screen_pixel ( 
		cur_item -> pos . x , 
		cur_item -> pos . y , FALSE ) - cur_item -> text_slot_rectangle . h / 2 ;

	//--------------------
	// But maybe the situation is already very crowded, i.e. maybe there are
	// already (a lot of) items there with slot positions conflicting...
	// Well, what to do?  If there is already an item there, we try to escape,
	// that's it.
	//
	if ( ( item_slot_position_blocked ( cur_item , i - 1 ) ) )
	{
	    while ( item_slot_position_blocked ( cur_item , i - 1 ) )
	    {
		if ( i % 2 )
		    cur_item -> text_slot_rectangle . y += 2 ;
		else
		    cur_item -> text_slot_rectangle . y -= 2 ;

		//--------------------
		// Maybe just a hundred left or right would also do...  but if it
		// doesn't, we'll undo the changes made.
		//
		cur_item -> text_slot_rectangle . x += 50 ;
		if ( item_slot_position_blocked ( cur_item , i - 1 ) ) 
		    cur_item -> text_slot_rectangle . x -= 50 ;
		cur_item -> text_slot_rectangle . x -= 50 ;
		if ( item_slot_position_blocked ( cur_item , i - 1 ) ) 
		    cur_item -> text_slot_rectangle . x += 50 ;
	    }
	}
    }
}; // void update_item_text_slot_positions ( void )

/* -----------------------------------------------------------------
 * This function assembles the contents of the combat window 
 * in Screen.
 *
 * Several FLAGS can be used to control its behaviour:
 *
 * (*) ONLY_SHOW_MAP = 1:  This flag indicates not do draw any
 *     game elements but the map blocks
 *
 * (*) DO_SCREEN_UPDATE = 2: This flag indicates for the function
 *     to also cause an SDL_Update of the portion of the screen
 *     that has been modified
 *
 * (*) ONLY_SHOW_MAP_AND_TEXT = 4: This flag indicates, that only
 *     the map and also info like the current coordinate position
 *     should be entered into the Screen.  This flag is mainly
 *     used for the level editor.
 *
 * ----------------------------------------------------------------- */
void
AssembleCombatPicture ( int mask )
{
    int i;
    int item_under_cursor = get_floor_item_index_under_mouse_cursor ( 0 );

    DebugPrintf ( 2 , "\n%s(): inside display code now." , __FUNCTION__ );
    
    clear_screen() ;
    
    if ( mask & USE_OWN_MOUSE_CURSOR )
	make_sure_system_mouse_cursor_is_turned_off();
    else
	make_sure_system_mouse_cursor_is_turned_on();

    //--------------------
    // We generate a list of obstacles (and other stuff) that might
    // emitt some light.  It should be sufficient to establish this
    // list once in the code and the to use it for all light computations
    // of this frame.
    //
    update_light_list ( 0 );
    
    //--------------------
    // Within all of this display code, we only check for LIGHT as far
    // as 'passability' is concerned.  Outside of this function, we'll
    // always check for real material collisions.  Accordingly we set
    // and unset the following flag at the beginning/ending of this
    // function respectively.
    // (I know that this is not considered a perfectly 'clean' method,
    //  but since collision checks are _most_ time-critical, I think
    //  we can live with it this time...)
    //
    global_check_for_light_only_collisions_flag = TRUE ;

    isometric_show_floor_around_tux_without_doublebuffering ( mask );
    
    set_up_ordered_blitting_list ( mask );
    
    blit_preput_objects_according_to_blitting_list ( mask );
    
    PutMouseMoveCursor ( );
    
    if ( mask & SHOW_ITEMS )
    {
	for ( i = 0 ; i < MAX_ITEMS_PER_LEVEL ; i ++ )
	{
	    if ( i == item_under_cursor )
		PutItem ( i , mask , PUT_NO_THROWN_ITEMS , TRUE ); 
	    else
		PutItem ( i , mask , PUT_NO_THROWN_ITEMS , FALSE ); 
	}
    }

    if ( use_open_gl )
    {
	blit_nonpreput_objects_according_to_blitting_list ( mask );
	if ( ( ! GameConfig . skip_light_radius ) &&
	     ( ! ( mask & SKIP_LIGHT_RADIUS ) ) ) blit_light_radius();

    }
    else
    {
	blit_nonpreput_objects_according_to_blitting_list ( mask );
	if ( ( ! GameConfig . skip_light_radius )  &&
	     ( ! ( mask & SKIP_LIGHT_RADIUS ) ) ) blit_light_radius();

    }
  
    PutMiscellaneousSpellEffects ( );
    
    if (mask & ONLY_SHOW_MAP) 
    {
	// in case we only draw the map, we are done here.  But
	// of course we must check if we should update the screen too.
	if ( mask & DO_SCREEN_UPDATE ) 
	    our_SDL_update_rect_wrapper( Screen , User_Rect.x , User_Rect.y , User_Rect.w , User_Rect.h );
	
	//--------------------
	// Within all of this display code, we only check for LIGHT as far
	// as 'passability' is concerned.  Outside of this function, we'll
	// always check for real material collisions.  Accordingly we set
	// and unset the following flag at the beginning/ending AND OF COURSE
	// AT ANY RETURN COMMAND of this
	// function respectively.
	// (I know that this is not considered a perfectly 'clean' method,
	//  but since collision checks are _most_ time-critical, I think
	//  we can live with it this time...)
	//
	global_check_for_light_only_collisions_flag = FALSE ;
	return;
    }
    
    show_obstacle_labels ( mask );

    if ( ! use_open_gl )
	show_automap_data_sdl ( ) ;
    else
	show_automap_data_ogl ( GameConfig . automap_display_scale ) ; 
    
    ShowCombatScreenTexts ( mask );
    
    if ( Shift_Is_Pressed() )
    {
	update_item_text_slot_positions ( );
	blit_all_item_slots ( );
    }

    //--------------------
    // Here are some more things, that are not needed in the level editor
    // view...
    //
    if ( ! ( mask & ONLY_SHOW_MAP_AND_TEXT ) )
    {
	ShowItemAlarm();
	// ShowQuickInventory ();
	display_current_game_message_window ( ) ;
	ShowCharacterScreen ( 0 );
	ShowSkillsScreen ( );
	ShowInventoryScreen ( );
	DisplayButtons( );
	if ( ! GameOver )
	    DisplayBanner ( );
	ShowQuickInventory ();
    }
    
    if ( ServerMode )
	CenteredPrintStringFont ( Screen , Menu_BFont , GameConfig . screen_height/2 , " S E R V E R ! ! ! " );
    
    if ( GameConfig.Inventory_Visible ) 
    {
	User_Rect.x = 320;
	User_Rect.w = GameConfig . screen_width - 320;
    }
    else if ( GameConfig . CharacterScreen_Visible || GameConfig . SkillScreen_Visible ) 
    {
	User_Rect.x = 0;
	User_Rect.w = GameConfig . screen_width - 320; 
    }
    else
    {
	User_Rect.x = 0;
	User_Rect.w = GameConfig . screen_width;
    }

    if ( mask & USE_OWN_MOUSE_CURSOR )
    {
	blit_our_own_mouse_cursor ();
	blit_mouse_cursor_corona ();
    }

    //--------------------
    // At this point we are done with the drawing procedure
    // and all that remains to be done is updating the screen.
    // Depending on where we did our modifications, we update
    // an according portion of the screen.
    //
    if ( mask & DO_SCREEN_UPDATE )
    {
	our_SDL_update_rect_wrapper( Screen , User_Rect.x , User_Rect.y , User_Rect.w , User_Rect.h );
    }

    //--------------------
    // Within all of this display code, we only check for LIGHT as far
    // as 'passability' is concerned.  Outside of this function, we'll
    // always check for real material collisions.  Accordingly we set
    // and unset the following flag at the beginning/ending AND OF COURSE
    // AT ANY RETURN COMMAND of this
    // function respectively.
    // (I know that this is not considered a perfectly 'clean' method,
    //  but since collision checks are _most_ time-critical, I think
    //  we can live with it this time...)
    //
    global_check_for_light_only_collisions_flag = FALSE ;

    DebugPrintf ( 2 , "\n%s(): done with display code again." , __FUNCTION__ );

}; // void AssembleCombatPicture(...)

/* -----------------------------------------------------------------
 * This function draws the mouse move cursor.
 * ----------------------------------------------------------------- */
void
PutMouseMoveCursor ( void )
{
    SDL_Rect TargetRectangle;
    
    if ( ( Me [ 0 ] . mouse_move_target . x == (-1) ) &&
	 ( Me [ 0 ] . current_enemy_target == (-1) ) )
    {
	return;
    }
    
    if ( Me [ 0 ] . mouse_move_target.x != (-1) )
    {
	TargetRectangle . x = 
	    translate_map_point_to_screen_pixel ( Me [ 0 ] . mouse_move_target . x , Me [ 0 ] . mouse_move_target . y , TRUE );
	TargetRectangle . y = 
	    translate_map_point_to_screen_pixel ( Me [ 0 ] . mouse_move_target . x , Me [ 0 ] . mouse_move_target . y , FALSE );
	if ( use_open_gl )
	{
	    TargetRectangle . x -= MouseCursorImageList [ 0 ] . original_image_width / 2 ;
	    TargetRectangle . y -= MouseCursorImageList [ 0 ] . original_image_height / 2 ;
	    blit_open_gl_texture_to_screen_position ( MouseCursorImageList [ 0 ] , 
						      TargetRectangle . x , TargetRectangle . y , TRUE );
	}
	else
	{
	    TargetRectangle . x -= MouseCursorImageList [ 0 ] . surface -> w / 2 ;
	    TargetRectangle . y -= MouseCursorImageList [ 0 ] . surface -> h / 2 ;
	    our_SDL_blit_surface_wrapper ( MouseCursorImageList [ 0 ] . surface , NULL , Screen , &TargetRectangle);
	}
    }
    
    if ( Me [ 0 ] . current_enemy_target != (-1) ) 
    {
	// translate_map_point_to_screen_pixel ( float x_map_pos , float y_map_pos , int give_x )
	
	TargetRectangle . x = 
	    translate_map_point_to_screen_pixel ( AllEnemys [ Me [ 0 ] . current_enemy_target ] . pos . x , 
						  AllEnemys [ Me [ 0 ] . current_enemy_target ] . pos . y , TRUE );
	TargetRectangle . y = 
	    translate_map_point_to_screen_pixel ( AllEnemys [ Me [ 0 ] . current_enemy_target ] . pos . x , 
						  AllEnemys [ Me [ 0 ] . current_enemy_target ] . pos . y , FALSE );
	if ( use_open_gl )
	{
	    TargetRectangle . x -= MouseCursorImageList [ 1 ] . original_image_width / 2 ;
	    TargetRectangle . y -= MouseCursorImageList [ 1 ] . original_image_height / 2 ;
	    blit_open_gl_texture_to_screen_position ( MouseCursorImageList [ 1 ] , 
						      TargetRectangle . x , TargetRectangle . y , TRUE );
	}
	else
	{
	    TargetRectangle . x -= MouseCursorImageList [ 1 ] . surface -> w / 2 ;
	    TargetRectangle . y -= MouseCursorImageList [ 1 ] . surface -> h / 2 ;
	    our_SDL_blit_surface_wrapper ( MouseCursorImageList [ 1 ] . surface , NULL , Screen , &TargetRectangle);
	}

    }
    
}; // void PutMouseMoveCursor ( void )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
/*
void 
free_single_tux_image ( tux_part_group , our_phase , rotation_index )
{
    // if ( loaded_tux_images [ tux_part_group ] [ our_phase ] [ rotation_index ] . surface != NULL )
    SDL_FreeSurface ( loaded_tux_images [ tux_part_group ] [ our_phase ] [ rotation_index ] . surface ) ;
    loaded_tux_images [ tux_part_group ] [ our_phase ] [ rotation_index ] . surface = NULL ;
}; // free_single_tux_image ( tux_part_group , our_phase , rotation_index )
*/

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
free_one_loaded_tux_image_series ( int tux_part_group )
{
    int j;
    int k;
    
    //--------------------
    if ( strcmp ( previous_part_strings [ tux_part_group ] , NOT_LOADED_MARKER ) == 0 )
    {
	DebugPrintf ( 1 , "\n%s(): refusing to free group %d because it's free already." , 
		      __FUNCTION__ , tux_part_group ) ;
	return;
    }

    DebugPrintf ( 1 , "\n%s():  part_group = %d." , __FUNCTION__ , tux_part_group );

    strcpy ( previous_part_strings [ tux_part_group ] , NOT_LOADED_MARKER );

    for ( j = 0 ; j < TUX_TOTAL_PHASES ; j ++ )
    {
	for ( k = 0 ; k < MAX_TUX_DIRECTIONS ; k ++ )
	{
	    // if ( loaded_tux_images [ tux_part_group ] [ j ] [ k ] . surface != NULL )
	    // SDL_FreeSurface ( loaded_tux_images [ tux_part_group ] [ j ] [ k ] . surface ) ;
	    // loaded_tux_images [ tux_part_group ] [ j ] [ k ] . surface = NULL ;
	    // free_single_tux_image ( tux_part_group , j , k );

	    SDL_FreeSurface ( 
		loaded_tux_images [ tux_part_group ] [ j ] [ k ] . surface ) ;

	    free ( loaded_tux_images [ tux_part_group ] [ j ] [ k ] . attached_pixel_data );

	    loaded_tux_images [ tux_part_group ] [ j ] [ k ] . surface = NULL ;
	}
    }
    
    DebugPrintf ( 1 , "...done freeing group." ) ;

}; // void free_one_loaded_tux_image_series ( int tux_part_group )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
clear_all_loaded_tux_images ( int with_free )
{
    int i , j , k;

    //--------------------
    // Some more debug output...
    //
    DebugPrintf ( 1 , "\n%s(): clearing tux surfaces.  with_free=%d." , __FUNCTION__ , with_free );

    if ( with_free )
    {
	for ( i = 0 ; i < ALL_PART_GROUPS ; i ++ )
	{
	    free_one_loaded_tux_image_series ( i );
	}
    }
    else
    {
	for ( i = 0 ; i < ALL_PART_GROUPS ; i ++ )
	{
	    strcpy ( previous_part_strings [ i ] , NOT_LOADED_MARKER );
	    for ( j = 0 ; j < TUX_TOTAL_PHASES ; j ++ )
	    {
		for ( k = 0 ; k < MAX_TUX_DIRECTIONS ; k ++ )
		{
		    loaded_tux_images [ i ] [ j ] [ k ] . surface = NULL ;
		}
	    }
	}
    }

}; // void clear_all_loaded_tux_images ( int force_free )

/* ----------------------------------------------------------------------
 * Now we determine the phase to use.  This is not all the same phase any 
 * more for all tux parts now that we've introduced a walk cycle.
 * ---------------------------------------------------------------------- */
int
get_current_phase ( int tux_part_group , int player_num , int motion_class ) 
{
    int our_phase = (int) Me [ player_num ] . phase ;
    float my_speed;
    
    if ( Me [ player_num ] . weapon_swing_time < 0 )
    {
	our_phase = (int) Me [ player_num ] . walk_cycle_phase ;
	
	//--------------------
	// Maybe the Tux speed is so high, that he should be considered running...
	// But then we should use the running motion, which is just 10 frames shifted
	// but otherwise in sync with the normal walkcycle...
	//
	my_speed = sqrt ( Me [ player_num ] . speed . x * Me [ player_num ] . speed . x +
			  Me [ player_num ] . speed . y * Me [ player_num ] . speed . y ) ;
	if ( my_speed > ( TUX_WALKING_SPEED + TUX_RUNNING_SPEED ) * 0.5 )
	    our_phase += TUX_WALK_CYCLE_PHASES ;
	
	// our_phase = ( ( ( int ) SDL_GetTicks()/1000) % 6 ) + 16 ;
    }
    
    return ( our_phase );

}; // int get_current_phase ( int tux_part_group , int player_num ) 

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
int
get_motion_class ( int player_num ) 
{
    int weapon_type = Me [ player_num ] . weapon_item . type ;
    int motion_class;
    
    if ( weapon_type == (-1) )
    {
	motion_class = 0 ;
    }
    else
    {
	if ( ItemMap [ weapon_type ] . item_gun_angle_change != 0 )
	    motion_class = 0 ;
	else
	    motion_class = 1 ;
    }
    
    //--------------------
    // If the motion class has changed, then everything needs to be reloaded...
    //
    if ( motion_class != previously_used_motion_class )
    {
	previously_used_motion_class = motion_class ;
	clear_all_loaded_tux_images ( TRUE ) ;
    }
    
    return ( motion_class );
    
}; // int get_motion_class ( player_num ) 

/* ----------------------------------------------------------------------
 * We open a tux image archive file corresponding to the currently needed
 * tux image series.
 * ---------------------------------------------------------------------- */
FILE*
open_tux_image_archive_file ( int tux_part_group , int motion_class , char* part_string )
{
    char constructed_filename[10000];
    char* fpath;
    FILE *DataFile;

    //--------------------
    // We need a file name!
    //
    sprintf ( constructed_filename , "tux_motion_parts/%s/%s%s.tux_image_archive" , 
	      motion_class_string [ motion_class ] , part_group_strings [ tux_part_group ] , 
	      part_string );
    fpath = find_file ( constructed_filename , GRAPHICS_DIR, FALSE );
    
    //--------------------
    // First we need to open the file
    //
    if ( ( DataFile = fopen ( fpath , "rb" ) ) == NULL )
    {
	fprintf( stderr, "\n\nfilename: '%s'\n" , fpath );
	
	GiveStandardErrorMessage ( __FUNCTION__  , "\
Freedroid was unable to open a given tux image archive.\n\
This indicates a serious bug in this installation of Freedroid.",
				   PLEASE_INFORM, IS_FATAL );
    }
    else
    {
	DebugPrintf ( 1 , "\n%s(): Opening file succeeded..." , __FUNCTION__ );
    }

    return ( DataFile );

}; // FILE* open_tux_image_archive_file ( int tux_part_group , int motion_class , char* part_string )

/* ----------------------------------------------------------------------
 * While earlier we used lots and lots of isolated .png and .offset files
 * to store the information about the Tux, we've now moved over to using
 * a single archive file that holds all the image and all the offset 
 * information, even in uncompressed form, making access at runtime even
 * *much* faster than it was before.  This file grabs one tux part from
 * such an archive file.  It's typically called once or twice whenever 
 * either a fresh game is started/loaded or when the Tux is changing
 * equipment.
 * ---------------------------------------------------------------------- */
void
grab_tux_images_from_archive ( int tux_part_group , int motion_class , char* part_string )
{
    int rotation_index;
    int our_phase ;
    FILE *DataFile;
    char* tmp_buff;
    char archive_type_string [ 5 ] = { 0 , 0 , 0 , 0 , 0 } ;
    char ogl_support_string [ 5 ] = { 0 , 0 , 0 , 0 , 0 } ;
    char *DataBuffer, *ptr;
    int filelen, tmplen;

    Sint16 cooked_walk_object_phases;
    Sint16 cooked_attack_object_phases;
    Sint16 cooked_gethit_object_phases;
    Sint16 cooked_death_object_phases;
    Sint16 cooked_stand_object_phases;

    Sint16 img_xlen;
    Sint16 img_ylen;
    Sint16 img_x_offs;
    Sint16 img_y_offs;

    //--------------------
    // A short message for debug purposes
    //
    DebugPrintf ( 1 , "\n%s():  grabbing new image series..." , __FUNCTION__ );

    // reading binary-files requires endian swapping depending on platform
    // Therefore we read the whole file into memory first then read out the 
    // numbers using SDLNet_Read..(). The file have to be written using SDLNet_Write..()
    DataFile = open_tux_image_archive_file ( tux_part_group , motion_class , part_string );
    filelen = FS_filelength (DataFile);
    DataBuffer = MyMalloc(filelen);
    fread ( DataBuffer, filelen, 1, DataFile );
    fclose ( DataFile );

    ptr = DataBuffer;
    //--------------------
    // We store the currently loaded part string, so that we can later
    // decide if we need to do something upon an equipment change or
    // not.
    //
    strcpy ( previous_part_strings [ tux_part_group ] , part_string );
    DebugPrintf ( 1 , "\n%s(): getting image series for group %d." , __FUNCTION__ , tux_part_group );

    //--------------------
    // Now we assume, that this is an image collection file for tux
    // and therefore it should have the right header bytes (keyword tuxX)
    // and it also should be suitable for pure SDL (keyword sdlX)
    //
    memcpy ( archive_type_string , ptr, 4 );
    ptr += 4;
    memcpy ( ogl_support_string ,  ptr, 4 );
    ptr += 4;

    //--------------------
    // We check if this is really an image archive of ENEMY type...
    //
    if ( strncmp ( "tuxX" , archive_type_string , 4 ) )
    {
	GiveStandardErrorMessage ( __FUNCTION__  , "\
Initial archive type string doesn't look like it's from an image archive of TUX type.\n\
This indicates a serious bug in this installation of Freedroid.",
				   PLEASE_INFORM, IS_FATAL );
    }
    //--------------------
    // We check if this is really an image archive of ENEMY type...
    //
    if ( strncmp ( "sdlX" , ogl_support_string , 4 ) )
    {
	GiveStandardErrorMessage ( __FUNCTION__  , "\
Initial archive type string doesn't look like this is a pure-SDL\n\
arranged image archive.  While this is not impossible to use, it's\n\
still quite inefficient, and I can only recommend to use sdl-sized\n\
images.  Therefore I refuse to process this file any further here.",
				   PLEASE_INFORM, IS_FATAL );
    }

    //--------------------
    // Now we know that this is an archive of tux type.  Therefore
    // we can start to read out some entries, that are only found in
    // enemy image collections and then disregard them, because for
    // tux, we don't need this kind of information anyway.
    //

    cooked_walk_object_phases = ReadSint16 ( ptr );
    ptr += sizeof ( Sint16 );
    cooked_attack_object_phases = ReadSint16 ( ptr );
    ptr += sizeof ( Sint16 );
    cooked_gethit_object_phases  = ReadSint16 ( ptr );
    ptr += sizeof ( Sint16 );
    cooked_death_object_phases = ReadSint16 ( ptr );
    ptr += sizeof ( Sint16 );
    cooked_stand_object_phases = ReadSint16 ( ptr );
    ptr += sizeof ( Sint16 );

    //--------------------
    // Now we can start to really load the images.
    //
    for ( rotation_index = 0 ; rotation_index < MAX_TUX_DIRECTIONS ; rotation_index ++ )
      {
	for ( our_phase = 0 ; our_phase < TUX_TOTAL_PHASES ; our_phase ++ )
	  {	    
	    //--------------------
	    // Now if the iso_image we want to blit right now has not yet been loaded,
	    // then we need to do something about is and at least attempt to load the
	    // surface
	    //
	    if ( loaded_tux_images [ tux_part_group ] [ our_phase ] [ rotation_index ] . surface == NULL )
	      {
		img_xlen = ReadSint16 ( ptr );
		ptr += sizeof ( Sint16 );
		img_ylen = ReadSint16 ( ptr );
		ptr += sizeof ( Sint16 );
		img_x_offs = ReadSint16 ( ptr );
		ptr += sizeof ( Sint16 );
		img_y_offs = ReadSint16 ( ptr );
		ptr += sizeof ( Sint16 );
		
		//--------------------
		// Some extra checks against illegal values for the length and height
		// of the tux images.
		//
		if ( ( img_xlen <= 0 ) || ( img_ylen <= 0 ) )
		  {
		    GiveStandardErrorMessage ( __FUNCTION__  , "\
Received some non-positive Tux surface dimensions.  That's a bug for sure!",
					       PLEASE_INFORM, IS_FATAL );
		}
		//--------------------
		// New code:  read data into some area.  Have SDL make a surface around the
		// loaded data.  That is much cleaner than hard-writing the data into the 
		// memory, that SDL has prepared internally.
		//
		tmplen = 4 * img_xlen * img_ylen;
		tmp_buff = MyMalloc ( tmplen ) ;
		memcpy ( tmp_buff , ptr, tmplen );
		ptr += tmplen;
#               ifdef __APPLE_CC__
		endian_swap ( tmp_buff, 4, img_xlen * img_ylen);
#               endif

		
		if ( ptr - DataBuffer > filelen )
		  {
		    GiveStandardErrorMessage ( __FUNCTION__  , "\
Datafile-length seems inconsistent with size of contained graphics-data", 
					       PLEASE_INFORM, IS_FATAL );
		  }
	
		loaded_tux_images [ tux_part_group ] [ our_phase ] [ rotation_index ] . attached_pixel_data = tmp_buff ;
		loaded_tux_images [ tux_part_group ] [ our_phase ] [ rotation_index ] . surface = 
		    SDL_CreateRGBSurfaceFrom ( tmp_buff , img_xlen , img_ylen , 32, 4 * img_xlen , 
					       0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000 ) ;
		
		if ( loaded_tux_images [ tux_part_group ] [ our_phase ] [ rotation_index ] . surface == NULL )
		{
		    DebugPrintf ( -1000 , "\n\nError code from SDL: %s." , SDL_GetError() );
		    GiveStandardErrorMessage ( __FUNCTION__  , "\
Creation of an Tux SDL software surface from pixel data failed.",
					       PLEASE_INFORM, IS_FATAL );
		}

		//--------------------
		// Depending on whether this is supposed to work with faster but less
		// quality color key or slower but more quality alpha channel, we set
		// appropriate parameters in the SDL surfaces and also a reminder flag
		// in the iso_image structure.
		//
		loaded_tux_images [ tux_part_group ] [ our_phase ] [ rotation_index ] . force_color_key = FALSE ;

		//--------------------
		// This might be useful later, when using only SDL output...
		//
		// SDL_SetAlpha( Whole_Image , 0 , SDL_ALPHA_OPAQUE );
		// our_iso_image -> surface = our_SDL_display_format_wrapperAlpha( Whole_Image );
		// now we have an alpha-surf of right size
		loaded_tux_images [ tux_part_group ] [ our_phase ] [ rotation_index ] . zoomed_out_surface = NULL ;
		loaded_tux_images [ tux_part_group ] [ our_phase ] [ rotation_index ] . texture_has_been_created = FALSE ;
		loaded_tux_images [ tux_part_group ] [ our_phase ] [ rotation_index ] . offset_x = img_x_offs ;
		loaded_tux_images [ tux_part_group ] [ our_phase ] [ rotation_index ] . offset_y = img_y_offs ;
		
		// this should clear any color key in the dest surface
		SDL_SetColorKey( loaded_tux_images [ tux_part_group ] [ our_phase ] [ rotation_index ] . surface , 0 , 0 );
		

		if ( ! use_open_gl ) 		  
		  flip_image_horizontally ( loaded_tux_images[tux_part_group][our_phase][rotation_index].surface ) ;
	      }
	    else
	      {
		//--------------------
		// If the surface pointer hasn't been NULL in the first place, then
		// obviously something with the initialisation was wrong in the first
		// place...
		//
		GiveStandardErrorMessage ( __FUNCTION__  , "\
Surface to be loaded didn't have empty (NULL) pointer in the first place.",
					   PLEASE_INFORM, IS_FATAL );

	      }
	  }
      } /* for rotation_index < MAX_TUX_DIRECTIONS */

    
    /* ok, we're done reading. Don't forget to free data-file */
    free ( DataBuffer );

    return;

}; // void grab_tux_images_from_archive ( ... )

/* ----------------------------------------------------------------------
 * While earlier we used lots and lots of isolated .png and .offset files
 * to store the information about an emey, we've now moved over to using
 * a single archive file that holds all the image and all the offset 
 * information, even in uncompressed form, making access at runtime even
 * *much* faster than it was before.  This file grabs one enemy from
 * such an archive file.  It's typically called once whenever the enemy
 * type is first encountered in one run of the engine.
 * ---------------------------------------------------------------------- */
void
grab_enemy_images_from_archive ( int enemy_model_nr )
{
    int rotation_index;
    int enemy_phase ;
    FILE *DataFile;
    char constructed_filename [ 10000 ] ;
    char* fpath;
    char archive_type_string [ 5 ] = { 0 , 0 , 0 , 0 , 0 } ;
    char ogl_support_string [ 5 ] = { 0 , 0 , 0 , 0 , 0 } ;
    char *DataBuffer, *ptr, *dest;
    int filelen, tmplen;

    Sint16 img_xlen;
    Sint16 img_ylen;
    Sint16 img_x_offs;
    Sint16 img_y_offs;
    Sint16 orig_img_xlen;
    Sint16 orig_img_ylen;

    Sint16 cooked_walk_object_phases;
    Sint16 cooked_attack_object_phases;
    Sint16 cooked_gethit_object_phases;
    Sint16 cooked_death_object_phases;
    Sint16 cooked_stand_object_phases;

    //--------------------
    // A short message for debug purposes
    //
    DebugPrintf ( 1 , "\n%s:  grabbing new image series..." , __FUNCTION__ );

    //--------------------
    // We need a file name!
    //
    sprintf ( constructed_filename , "droids/%s/%s.tux_image_archive" , 
	      PrefixToFilename [ enemy_model_nr ] ,
	      PrefixToFilename [ enemy_model_nr ] );
    fpath = find_file ( constructed_filename , GRAPHICS_DIR, FALSE );
    
    //--------------------
    // First we need to open the file
    //
    if ( ( DataFile = fopen ( fpath , "rb" ) ) == NULL )
    {
	fprintf( stderr, "\n\nfilename: '%s'\n" , fpath );
	
	GiveStandardErrorMessage ( __FUNCTION__  , "\
Freedroid was unable to open a given enemy image archive.\n\
This indicates a serious bug in this installation of Freedroid.",
				   PLEASE_INFORM, IS_FATAL );
    }
    else
    {
	DebugPrintf ( 1 , "\n%s() : Opening file succeeded..." , __FUNCTION__ );
    }

    /* read the whole file into memory, then use ReadSint16() for correct
     * endianness of byte-reading 
     */
    filelen = FS_filelength (DataFile);
    DataBuffer = MyMalloc(filelen);
    fread ( DataBuffer, filelen, 1, DataFile );
    fclose ( DataFile );

    ptr = DataBuffer;    

    //--------------------
    // Now we assume, that this is an image collection file for an enemy
    // and therefore it should have the right header bytes (keyword eneX)
    // and it also should be suitable for use with OpenGl (keyword oglX)
    //
    memcpy ( archive_type_string , ptr , 4 );
    ptr += 4;
    memcpy ( ogl_support_string ,  ptr, 4 );
    ptr += 4;

    //--------------------
    // We check if this is really an image archive of ENEMY type...
    //
    if ( strncmp ( "eneX" , archive_type_string , 4 ) )
    {
	GiveStandardErrorMessage ( __FUNCTION__  , "\
Initial archive type string doesn't look like it's from an image archive of ENEMY type.\n\
This indicates a serious bug in this installation of Freedroid.",
				   PLEASE_INFORM, IS_FATAL );
    }

    //--------------------
    // Now we know that this is an archive of enemy type.  Therefore
    // we can start to read out some entries, that are only found in
    // enemy image collections.
    //
    cooked_walk_object_phases = ReadSint16 ( ptr );
    ptr += sizeof ( Sint16 );
    cooked_attack_object_phases = ReadSint16 ( ptr );
    ptr += sizeof ( Sint16 );
    cooked_gethit_object_phases = ReadSint16 ( ptr );
    ptr += sizeof ( Sint16 );
    cooked_death_object_phases = ReadSint16 ( ptr );
    ptr += sizeof ( Sint16 );
    cooked_stand_object_phases = ReadSint16 ( ptr );
    ptr += sizeof ( Sint16 );

    //--------------------
    // The information about cycle length needs to be entered into the 
    // corresponding arrays (usually initialized in blocks.c, for those
    // series, that don't have an image archive yet...)
    //
    first_walk_animation_image [ enemy_model_nr ] = 1 ;
    last_walk_animation_image [ enemy_model_nr ] = cooked_walk_object_phases ;
    first_attack_animation_image [ enemy_model_nr ] = last_walk_animation_image [ enemy_model_nr ] + 1 ; 
    last_attack_animation_image [ enemy_model_nr ] = last_walk_animation_image [ enemy_model_nr ] + cooked_attack_object_phases ;
    first_gethit_animation_image [ enemy_model_nr ] = last_attack_animation_image [ enemy_model_nr ] + 1 ;
    last_gethit_animation_image [ enemy_model_nr ] = last_attack_animation_image [ enemy_model_nr ] + cooked_gethit_object_phases ;
    first_death_animation_image [ enemy_model_nr ] = last_gethit_animation_image [ enemy_model_nr ] + 1 ;
    last_death_animation_image [ enemy_model_nr ] = last_gethit_animation_image [ enemy_model_nr ] + cooked_death_object_phases ;
    first_stand_animation_image [ enemy_model_nr ] = last_death_animation_image [ enemy_model_nr ] + 1 ;
    last_stand_animation_image [ enemy_model_nr ] = last_death_animation_image [ enemy_model_nr ] + cooked_stand_object_phases ;

    //--------------------
    // Now some error checking against more phases in this enemy animation than
    // currently allowed from the array size...
    //
    if ( last_stand_animation_image [ enemy_model_nr ] >= MAX_ENEMY_MOVEMENT_PHASES )
    {
	DebugPrintf( -4 , "\nenemy_model_nr=%d." , enemy_model_nr );
	GiveStandardErrorMessage ( __FUNCTION__  , "\
The number of images found in the image collection is bigger than currently allowed.",
				   PLEASE_INFORM, IS_FATAL );
    }

    //--------------------
    // Now we can proceed to read in the pure image data from the image
    // collection archive file
    //
    for ( rotation_index = 0 ; rotation_index < ROTATION_ANGLES_PER_ROTATION_MODEL ; rotation_index ++ )
      {
	for ( enemy_phase = 0 ; enemy_phase < last_stand_animation_image [ enemy_model_nr ] ; enemy_phase ++ )
	  {	
	    //--------------------
	    // We read the image parameters.  We need those to construct the
	    // surface.  Therefore this must come first.
	    //
	    img_xlen = ReadSint16 ( ptr );
	    ptr += sizeof(Sint16);
	    img_ylen = ReadSint16 ( ptr );
	    ptr += sizeof(Sint16);
	    img_x_offs = ReadSint16 ( ptr );
	    ptr += sizeof(Sint16);
	    img_y_offs = ReadSint16 ( ptr );
	    ptr += sizeof(Sint16);
	    orig_img_xlen = ReadSint16 ( ptr );
	    ptr += sizeof(Sint16);
	    orig_img_ylen = ReadSint16 ( ptr );
	    ptr += sizeof(Sint16);

	    enemy_iso_images [ enemy_model_nr ] [ rotation_index ] [ enemy_phase ] . surface = 
	      SDL_CreateRGBSurface ( SDL_SWSURFACE , img_xlen , img_ylen, 32, 
				     0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000 ) ;

	    dest = enemy_iso_images[enemy_model_nr ] [ rotation_index ] [ enemy_phase ] . surface -> pixels;
	    tmplen = 4 * img_xlen * img_ylen;
	    memcpy ( dest, ptr, tmplen );
	    ptr += tmplen;
#           ifdef __APPLE_CC__
	    endian_swap ( dest, 4, img_xlen * img_ylen);
#           endif


	    //--------------------
	    // Depending on whether this is supposed to work with faster but less
	    // quality color key or slower but more quality alpha channel, we set
	    // appropriate parameters in the SDL surfaces and also a reminder flag
	    // in the iso_image structure.
	    //
	    enemy_iso_images [ enemy_model_nr ] [ rotation_index ] [ enemy_phase ] . force_color_key = FALSE ;

	    //--------------------
	    // This might be useful later, when using only SDL output...
	    //
	    // SDL_SetAlpha( Whole_Image , 0 , SDL_ALPHA_OPAQUE );
	    // our_iso_image -> surface = our_SDL_display_format_wrapperAlpha( Whole_Image ); 
	    // now we have an alpha-surf of right size
	    enemy_iso_images [ enemy_model_nr ] [ rotation_index ] [ enemy_phase ] . zoomed_out_surface = NULL ;
	    enemy_iso_images [ enemy_model_nr ] [ rotation_index ] [ enemy_phase ] . texture_has_been_created = FALSE ;
	    enemy_iso_images [ enemy_model_nr ] [ rotation_index ] [ enemy_phase ] . offset_x = img_x_offs ;
	    enemy_iso_images [ enemy_model_nr ] [ rotation_index ] [ enemy_phase ] . offset_y = img_y_offs ;
	    enemy_iso_images [ enemy_model_nr ] [ rotation_index ] [ enemy_phase ] . original_image_width = 
		orig_img_xlen ;
	    enemy_iso_images [ enemy_model_nr ] [ rotation_index ] [ enemy_phase ] . original_image_height = 
		orig_img_ylen ;
	    enemy_iso_images [ enemy_model_nr ] [ rotation_index ] [ enemy_phase ] . texture_width = 
		img_xlen ;
	    enemy_iso_images [ enemy_model_nr ] [ rotation_index ] [ enemy_phase ] . texture_height = 
		img_ylen ;
	    
	    SDL_SetColorKey( enemy_iso_images [ enemy_model_nr ] [ rotation_index ] [ enemy_phase ] . surface , 0 , 0 ); // this should clear any color key in the dest surface
	    
	    if ( ! use_open_gl ) 		  
	    {
		flip_image_horizontally ( 
		    enemy_iso_images [ enemy_model_nr ] [ rotation_index ] [ enemy_phase ] . surface ) ;
	    }
	    else
	    {
		if ( ! strncmp ( "oglX" , ogl_support_string , 4 ) )
		{
		    make_texture_out_of_prepadded_image ( 
			& ( enemy_iso_images [ enemy_model_nr ] [ rotation_index ] [ enemy_phase ] ) ) ;
		}
		else
		{
		    //--------------------
		    // Of course we could handle the case on non-open-gl optimized image
		    // collection files used with OpenGL output.  But that would be a
		    // sign of a bug, so we don't properly handle it (like below) but
		    // rather give out a fatal error message, just to be safe against
		    // non-open-gl-optimized image archives slipping undetected into some
		    // release or something...
		    //
		    // make_texture_out_of_surface ( 
		    // & ( enemy_iso_images [ enemy_model_nr ] [ rotation_index ] [ enemy_phase ] ) ) ;
		    //
		    GiveStandardErrorMessage ( __FUNCTION__  , "\
This image collection archive is not optimized for OpenGL usage\n\
but still used in conjunction with OpenGL graphics output.\n\
This is strange.  While of course we could handle this (a bit)\n\
slower than optimized archive, it's an indication that something\n\
is wrong with this installation of FreedroidRPG.  So we terminate\n\
to draw attention to the possible problem...",
				   PLEASE_INFORM, IS_FATAL );
		}
	    }
	}
    }

    free (DataBuffer);
    
    DebugPrintf ( 1 , "\n%s: grabbing new image series DONE." , __FUNCTION__ );

    return;

}; // void grab_enemy_images_from_archive ( ... )

/* ----------------------------------------------------------------------
 * When the Tux changes equipment and ONE NEW PART IS EQUIPPED, then
 * ALL THE IMAGES FOR THAT PART IN ALL DIRECTIONS AND ALL PHASES must
 * get loaded and that's what is done here...
 * ---------------------------------------------------------------------- */
void
make_sure_whole_part_group_is_ready ( int tux_part_group , int motion_class , char* part_string )
{
    grab_tux_images_from_archive ( tux_part_group , motion_class , part_string );

    //--------------------
    // It can be expected, that this operation HAS TAKEN CONSIDERABLE TIME!
    // Therefore we must activate the conservative frame time compution now,
    // so as to prevent any unwanted jumps right now...
    //
    Activate_Conservative_Frame_Computation ();
  
}; // void make_sure_whole_part_group_is_ready ( int tux_part_group , int motion_class , char* part_string )

/*----------------------------------------------------------------------
 * This function should blit the isometric version of the Tux to the
 * screen.
 *----------------------------------------------------------------------*/
void
iso_put_tux_part ( int tux_part_group , char* part_string , int x , int y , int player_num , int rotation_index )
{
    int i;
    int our_phase = 0 ;
    int motion_class;
    
    //--------------------
    // Now we find out which weapon class to use in this case.
    //
    motion_class = get_motion_class ( player_num ) ;
    
    //--------------------
    // Now we need to resolve the part_string given as parameter
    //
    if ( strlen ( part_string ) == 0 )
    {
	GiveStandardErrorMessage ( __FUNCTION__  , "\
Empty part string received!",
				   PLEASE_INFORM, IS_FATAL );
    }
    
    //--------------------
    // Now we determine the phase to use.  This is not all the same
    // phase any more for all tux parts now that we've introduced a walk cycle.
    //
    our_phase = get_current_phase ( tux_part_group , player_num , motion_class ) ;
    
    //--------------------
    // If some part string given is unlike the part string we were using so
    // far, then we'll need to free that old part and (later) load the new
    // part.
    //
    for ( i = 0 ; i < ALL_PART_GROUPS ; i ++ )
    {
	if ( strcmp ( previous_part_strings [ tux_part_group ] , part_string ) ) 
	{
	    free_one_loaded_tux_image_series ( tux_part_group );
	    make_sure_whole_part_group_is_ready ( tux_part_group , motion_class , part_string );
	}
    }
    
    //--------------------
    // Now everything should be loaded correctly and we just need to blit the Tux.  Anything
    // that isn't loaded yet should be considered a serious bug and a reason to terminate 
    // immediately...
    //
    if ( loaded_tux_images [ tux_part_group ] [ our_phase ] [ rotation_index ] . surface != NULL )
    {
	if ( x == (-1) )
	{
	    blit_iso_image_to_map_position ( loaded_tux_images [ tux_part_group ] [ our_phase ] [ rotation_index ] , 
					     Me [ player_num ] . pos . x , Me [ player_num ] . pos . y );
	}
	else
	{
	    blit_iso_image_to_screen_position ( loaded_tux_images [ tux_part_group ] [ our_phase ] [ rotation_index ] , 
						x + loaded_tux_images [ tux_part_group ] [ our_phase ] [ rotation_index ] . offset_x , y + loaded_tux_images [ tux_part_group ] [ our_phase ] [ rotation_index ] . offset_y );
	}
    }
    else
    {
	GiveStandardErrorMessage ( __FUNCTION__  , "Unable to load tux part!", PLEASE_INFORM, IS_FATAL );
    }
    
}; // void iso_put_tux_part ( char* part_string , int x , int y , int player_num )

/* ----------------------------------------------------------------------
 * This function will put the Tux torso, i.e. it will put some torso with
 * the currently equipped armour on it.  Of course we can't have a unique
 * ingame representation of the Tux torso for every type of armour inside
 * the game.  Therefore several types of armour will each be mapped upon
 * the same ingame representation.  Typically the types of armour mapping
 * to the same ingame representation will be so similar, that you can
 * easily tell them apart in inventory, but it will be more or less ok to
 * use the very same ingame representation, because they are rather 
 * similar after all.
 * ---------------------------------------------------------------------- */
void
iso_put_tux_torso ( int x , int y , int player_num , int rotation_index )
{
    switch ( Me [ player_num ] . armour_item . type )
    {
	case -1 :
	    iso_put_tux_part ( PART_GROUP_TORSO , "iso_torso" , x , y , player_num , rotation_index );
	    break;
	case ITEM_ARMOR_SIMPLE_JACKET:
	case ITEM_ARMOR_REINFORCED_JACKET:
	case ITEM_ARMOR_PROTECTIVE_JACKET:
	    iso_put_tux_part ( PART_GROUP_TORSO , "iso_armour1" , x , y , player_num , rotation_index );
	    break;
	case ITEM_RED_GUARD_LIGHT_ARMOUR:
	case ITEM_RED_GUARD_HEAVY_ARMOUR:
	    iso_put_tux_part ( PART_GROUP_TORSO , "iso_robe" , x , y , player_num , rotation_index );
	    break;
	default:
	    iso_put_tux_part ( PART_GROUP_TORSO , "iso_armour1" , x , y , player_num , rotation_index );
	    break;
    }
    
}; // void iso_put_tux_torso ( int x , int y , int player_num , int rotation_index )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
iso_put_tux_shieldarm ( int x , int y , int player_num , int rotation_index )
{
    //--------------------
    // In case of no shielditem present at all, it's clear that we'll just
    // display the empty shieldarm.
    //
    if ( Me [ player_num ] . shield_item . type == (-1) )
    {
	iso_put_tux_part ( PART_GROUP_SHIELD , "iso_shieldarm" , x , y , player_num , rotation_index );
	return;
    }
    
    //--------------------
    // If there is no weapon item present, we just need to blit the shield, cause
    // it's 'sword motion class' then.
    //
    
    if ( Me [ player_num ] . weapon_item . type != (-1) )
    {
	//--------------------
	// In case of a weapon item present, we need to look up the weapon item motion class
	// and then decide which shield to use.
	//
	if ( ItemMap [ Me [ player_num ] . weapon_item . type ] . item_gun_angle_change == 0 )
	{
	    iso_put_tux_part ( PART_GROUP_SHIELD , "iso_shieldarm" , x , y , player_num , rotation_index );
	    return;
	}
    }
    
    //--------------------
    // Now at this point we know, that a 'sword motion class' item is present, and that
    // we therefore need to blit the shield details.
    //
    switch ( Me [ player_num ] . shield_item . type )
    {
	case ITEM_BUCKLER:
	    iso_put_tux_part ( PART_GROUP_SHIELD , "iso_buckler" , x , y , player_num , rotation_index );
	    break;
	case ITEM_SMALL_SHIELD:
	case ITEM_MEDIUM_SHIELD:
	    iso_put_tux_part ( PART_GROUP_SHIELD , "iso_small_shield" , x , y , player_num , rotation_index );
	    break;
	case ITEM_STANDARD_SHIELD:
	    iso_put_tux_part ( PART_GROUP_SHIELD , "iso_standard_shield" , x , y , player_num , rotation_index );
	    break;
	case ITEM_LARGE_SHIELD:
	    iso_put_tux_part ( PART_GROUP_SHIELD , "iso_large_shield" , x , y , player_num , rotation_index );
	    break;
	default:
	    fprintf ( stderr , "Shield item code: %d " , Me [ player_num ] . shield_item . type ) ;
	    GiveStandardErrorMessage ( __FUNCTION__  , "This shield type is not yet rendered for Tux." ,
				       PLEASE_INFORM, IS_FATAL );
	    break;
    }

}; // void iso_put_tux_shieldarm ( int x , int y , int player_num , int rotation_index )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
iso_put_tux_head ( int x , int y , int player_num , int rotation_index )
{
    if ( Me [ player_num ] . special_item . type == (-1) )
	iso_put_tux_part ( PART_GROUP_HEAD , "iso_head" , x , y , player_num , rotation_index );
    else
	iso_put_tux_part ( PART_GROUP_HEAD , "iso_helm1" , x , y , player_num , rotation_index );
    
}; // void iso_put_tux_head ( int x , int y , int player_num , int rotation_index )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
iso_put_tux_feet ( int x , int y , int player_num , int rotation_index )
{
    if ( Me [ player_num ] . drive_item . type == (-1) )
	iso_put_tux_part ( PART_GROUP_FEET , "iso_feet" , x , y , player_num , rotation_index );
    else
	iso_put_tux_part ( PART_GROUP_FEET , "iso_boots1" , x , y , player_num , rotation_index );
    
}; // void iso_put_tux_feet ( int x , int y , int player_num , int rotation_index )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
iso_put_tux_weapon ( int x , int y , int player_num , int rotation_index )
{
    if ( Me [ player_num ] . weapon_item . type != (-1) )
    {
	if ( ItemMap [ Me [ player_num ] . weapon_item . type ] . item_gun_angle_change != 0 )
	{
	    if ( Me [ player_num ] . weapon_item . type == ITEM_MACE )
		iso_put_tux_part ( PART_GROUP_WEAPON , "iso_mace" , x , y , player_num , rotation_index );
	    else
		iso_put_tux_part ( PART_GROUP_WEAPON , "iso_sword" , x , y , player_num , rotation_index );
	}
	else
	{
	    iso_put_tux_part ( PART_GROUP_WEAPON , "iso_gun1" , x , y , player_num , rotation_index );
	}
    }
    
}; // void iso_put_tux_weapon ( int x , int y , int player_num , int rotation_index )

/* ----------------------------------------------------------------------
 * This function is intended to bring the Tux parts to the screen 
 * directly, respecting the correct order of blitting for all the parts
 * of the Tux in the sword motion.  Of course, the blitting order stongly
 * depends on the direction the Tux is facing.  Therefore a lot of cases
 * have to be separated.
 * ---------------------------------------------------------------------- */
void
iso_put_all_tux_parts_for_sword_motion ( int x , int y , int player_num , int rotation_index )
{

    DebugPrintf ( 2 , "\nDirection given: %d." , rotation_index );
    // DebugPrintf ( 0 , "\nphase: %d." , (int) Me [ player_num ] . phase );

    //--------------------
    // The correct order of blitting for all the Tux parts stongly depends
    // on the direction the Tux is facing, therefore we need to do careful
    // case separation for the tux direction.
    //
    switch ( rotation_index )
    {
	case 0:
	    iso_put_tux_feet ( x , y , player_num , rotation_index );
	    iso_put_tux_torso ( x , y , player_num , rotation_index );
	    iso_put_tux_head ( x , y , player_num , rotation_index );
	    iso_put_tux_part ( PART_GROUP_WEAPONARM , "iso_weaponarm" , x , y , player_num , rotation_index );
	    iso_put_tux_shieldarm ( x , y , player_num , rotation_index );
	    iso_put_tux_weapon ( x , y , player_num , rotation_index );
	    break;
	case 8:
	    iso_put_tux_weapon ( x , y , player_num , rotation_index );
	    iso_put_tux_feet ( x , y , player_num , rotation_index );
	    iso_put_tux_part ( PART_GROUP_WEAPONARM , "iso_weaponarm" , x , y , player_num , rotation_index );
	    iso_put_tux_shieldarm ( x , y , player_num , rotation_index );
	    iso_put_tux_torso ( x , y , player_num , rotation_index );
	    iso_put_tux_head ( x , y , player_num , rotation_index );
	    break;
	    
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	    iso_put_tux_feet ( x , y , player_num , rotation_index );
	    iso_put_tux_weapon ( x , y , player_num , rotation_index );
	    iso_put_tux_part ( PART_GROUP_WEAPONARM , "iso_weaponarm" , x , y , player_num , rotation_index );
	    iso_put_tux_torso ( x , y , player_num , rotation_index );
	    iso_put_tux_shieldarm ( x , y , player_num , rotation_index );
	    iso_put_tux_head ( x , y , player_num , rotation_index );
	    break;
	    
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	    iso_put_tux_feet ( x , y , player_num , rotation_index );
	    iso_put_tux_shieldarm ( x , y , player_num , rotation_index );
	    iso_put_tux_torso ( x , y , player_num , rotation_index );
	    iso_put_tux_head ( x , y , player_num , rotation_index );
	    iso_put_tux_weapon ( x , y , player_num , rotation_index );
	    iso_put_tux_part ( PART_GROUP_WEAPONARM , "iso_weaponarm" , x , y , player_num , rotation_index );
	    break;
	    
	default:
	    fprintf ( stderr , "Suspicious rotation index: %d " , rotation_index );
	    GiveStandardErrorMessage ( __FUNCTION__  , "\
Suspicious rotation index encountered!",
				       PLEASE_INFORM, IS_FATAL );
	    break;
    }
    
}; // void iso_put_all_tux_parts_for_sword_motion ( int x , int y , int player_num , int rotation_index )

/* ----------------------------------------------------------------------
 * This function is intended to bring the Tux parts to the screen 
 * directly, respecting the correct order of blitting for all the parts
 * of the Tux in the gun motion.  Of course, the blitting order stongly
 * depends on the direction the Tux is facing.  Therefore a lot of cases
 * have to be separated.
 * ---------------------------------------------------------------------- */
void
iso_put_all_tux_parts_for_gun_motion ( int x , int y , int player_num , int rotation_index )
{

    DebugPrintf ( 2 , "\nDirection given: %d." , rotation_index );
    // DebugPrintf ( 1 , "\nphase: %d." , (int) Me [ player_num ] . phase );
    
    //--------------------
    // The correct order of blitting for all the Tux parts stongly depends
    // on the direction the Tux is facing, therefore we need to do careful
    // case separation for the tux direction and sometimes even handle
    // some subcases because of the changes during the full motion.
    //
    switch ( rotation_index )
    {
	case 0:
	    iso_put_tux_feet ( x , y , player_num , rotation_index );
	    iso_put_tux_torso ( x , y , player_num , rotation_index );
	    iso_put_tux_head ( x , y , player_num , rotation_index );
	    iso_put_tux_shieldarm ( x , y , player_num , rotation_index );
	    iso_put_tux_weapon ( x , y , player_num , rotation_index );
	    iso_put_tux_part ( PART_GROUP_WEAPONARM , "iso_weaponarm" , x , y , player_num , rotation_index );
	    break;
	case 8:
	    iso_put_tux_feet ( x , y , player_num , rotation_index );
	    iso_put_tux_shieldarm ( x , y , player_num , rotation_index );
	    if ( ( (int) Me [ player_num ] . phase >= 3 ) && ( (int) Me [ player_num ] . phase <= 12 ) )
	    {
		iso_put_tux_weapon ( x , y , player_num , rotation_index );
		iso_put_tux_torso ( x , y , player_num , rotation_index );
		iso_put_tux_part ( PART_GROUP_WEAPONARM , "iso_weaponarm" , x , y , player_num , rotation_index );
	    }
	    else
	    {
		iso_put_tux_part ( PART_GROUP_WEAPONARM , "iso_weaponarm" , x , y , player_num , rotation_index );
		iso_put_tux_weapon ( x , y , player_num , rotation_index );
		iso_put_tux_torso ( x , y , player_num , rotation_index );
	    }
	    iso_put_tux_head ( x , y , player_num , rotation_index );
	    break;
	    
	case 9:
	    iso_put_tux_feet ( x , y , player_num , rotation_index );
	    iso_put_tux_weapon ( x , y , player_num , rotation_index );
	    if ( ( (int) Me [ player_num ] . phase >= 3 ) && ( (int) Me [ player_num ] . phase <= 12 ) )
	    {
		iso_put_tux_shieldarm ( x , y , player_num , rotation_index );
		iso_put_tux_torso ( x , y , player_num , rotation_index );
		iso_put_tux_part ( PART_GROUP_WEAPONARM , "iso_weaponarm" , x , y , player_num , rotation_index );
	    }
	    else
	    {
		iso_put_tux_part ( PART_GROUP_WEAPONARM , "iso_weaponarm" , x , y , player_num , rotation_index );
		iso_put_tux_torso ( x , y , player_num , rotation_index );
		iso_put_tux_shieldarm ( x , y , player_num , rotation_index );
	    }
	    iso_put_tux_head ( x , y , player_num , rotation_index );
	    break;
	    
	case 10:
	    iso_put_tux_feet ( x , y , player_num , rotation_index );
	    iso_put_tux_weapon ( x , y , player_num , rotation_index );
	    iso_put_tux_part ( PART_GROUP_WEAPONARM , "iso_weaponarm" , x , y , player_num , rotation_index );
	    if ( ( (int) Me [ player_num ] . phase >= 3 ) && ( (int) Me [ player_num ] . phase <= 12 ) )
	    {
		iso_put_tux_shieldarm ( x , y , player_num , rotation_index );
		iso_put_tux_torso ( x , y , player_num , rotation_index );
	    }
	    else
	    {
		iso_put_tux_torso ( x , y , player_num , rotation_index );
		iso_put_tux_shieldarm ( x , y , player_num , rotation_index );
	    }
	    iso_put_tux_head ( x , y , player_num , rotation_index );
	    break;
	case 11:
	    iso_put_tux_feet ( x , y , player_num , rotation_index );
	    iso_put_tux_part ( PART_GROUP_WEAPONARM , "iso_weaponarm" , x , y , player_num , rotation_index );
	    iso_put_tux_weapon ( x , y , player_num , rotation_index );
	    iso_put_tux_torso ( x , y , player_num , rotation_index );
	    iso_put_tux_shieldarm ( x , y , player_num , rotation_index );
	    iso_put_tux_head ( x , y , player_num , rotation_index );
	    break;
	    
	case 12:
	    iso_put_tux_feet ( x , y , player_num , rotation_index );
	    iso_put_tux_torso ( x , y , player_num , rotation_index );
	    iso_put_tux_part ( PART_GROUP_WEAPONARM , "iso_weaponarm" , x , y , player_num , rotation_index );
	    iso_put_tux_weapon ( x , y , player_num , rotation_index );
	    iso_put_tux_shieldarm ( x , y , player_num , rotation_index );
	    iso_put_tux_head ( x , y , player_num , rotation_index );
	    break;
	    
	case 13:
	case 14:
	case 15:
	    iso_put_tux_feet ( x , y , player_num , rotation_index );
	    iso_put_tux_part ( PART_GROUP_WEAPONARM , "iso_weaponarm" , x , y , player_num , rotation_index );
	    if ( ( (int) Me [ player_num ] . phase >= 4 ) && ( (int) Me [ player_num ] . phase <= 11 ) )
	    {
		iso_put_tux_weapon ( x , y , player_num , rotation_index );
		iso_put_tux_torso ( x , y , player_num , rotation_index );
	    }
	    else
	    {
		iso_put_tux_torso ( x , y , player_num , rotation_index );
		iso_put_tux_weapon ( x , y , player_num , rotation_index );
	    }
	    iso_put_tux_shieldarm ( x , y , player_num , rotation_index );
	    iso_put_tux_head ( x , y , player_num , rotation_index );
	    break;
	    
	case 1:
	case 2:
	    iso_put_tux_feet ( x , y , player_num , rotation_index );
	    
	    if ( ( (int) Me [ player_num ] . phase >= 4 ) && ( (int) Me [ player_num ] . phase <= 11 ) )
	    {
		iso_put_tux_torso ( x , y , player_num , rotation_index );
		iso_put_tux_head ( x , y , player_num , rotation_index );
		iso_put_tux_shieldarm ( x , y , player_num , rotation_index );
	    }
	    else
	    {
		iso_put_tux_shieldarm ( x , y , player_num , rotation_index );
		iso_put_tux_torso ( x , y , player_num , rotation_index );
		iso_put_tux_head ( x , y , player_num , rotation_index );
	    }
	    
	    iso_put_tux_weapon ( x , y , player_num , rotation_index );
	    iso_put_tux_part ( PART_GROUP_WEAPONARM , "iso_weaponarm" , x , y , player_num , rotation_index );
	    break;
	    
	    
	case 3:
	case 4:
	    iso_put_tux_feet ( x , y , player_num , rotation_index );
	    iso_put_tux_shieldarm ( x , y , player_num , rotation_index );
	    iso_put_tux_torso ( x , y , player_num , rotation_index );
	    iso_put_tux_weapon ( x , y , player_num , rotation_index );
	    iso_put_tux_part ( PART_GROUP_WEAPONARM , "iso_weaponarm" , x , y , player_num , rotation_index );
	    iso_put_tux_head ( x , y , player_num , rotation_index );
	    break;
	    
	case 5:
	case 6:
	case 7:
	    iso_put_tux_feet ( x , y , player_num , rotation_index );
	    iso_put_tux_shieldarm ( x , y , player_num , rotation_index );
	    iso_put_tux_weapon ( x , y , player_num , rotation_index );
	    iso_put_tux_torso ( x , y , player_num , rotation_index );
	    iso_put_tux_part ( PART_GROUP_WEAPONARM , "iso_weaponarm" , x , y , player_num , rotation_index );
	    iso_put_tux_head ( x , y , player_num , rotation_index );
	    break;
	    
	default:
	    fprintf ( stderr , "Suspicious rotation index: %d " , rotation_index );
	    GiveStandardErrorMessage ( __FUNCTION__  , "\
Suspicious rotation index encountered!",
				       PLEASE_INFORM, IS_FATAL );
	    break;
    }
    
}; // void iso_put_all_tux_parts_for_gun_motion ( x , y , player_num , rotation_index )

/*----------------------------------------------------------------------
 * This function should blit the isometric version of the Tux to the
 * screen.
 *----------------------------------------------------------------------*/
void
iso_put_tux ( int x , int y , int player_num )
{
    int rotation_index;
    float angle;
    
    //--------------------
    // In case there is no weapon swing going on, we can select the direction
    // of facing by examining the current speed.
    //
    if ( ( Me [ player_num ] . phase > 0 ) && ( Me [ player_num ] . phase <= TUX_SWING_PHASES ) )
    {
	//--------------------
	// Don't touch the direction of heading here, cause it's set correctly
	// within the raw tux attack function anyway.
	//
	angle = Me [ player_num ] . angle ;
    }
    else
    {
	//--------------------
	// We make the angle dependent upon direction of movement, but only if there really is
	// at least some movement.
	//
	if ( fabsf ( Me [ player_num ] . speed . x ) + fabsf ( Me [ player_num ] . speed . y ) > 0.1 )
	{
	    angle = - ( atan2 (Me [ player_num ].speed.y,  Me [ player_num ].speed.x) * 180 / M_PI - 45 -180 );
	    angle += 360 / ( 2 * MAX_TUX_DIRECTIONS );
	    while ( angle < 0 ) angle += 360;
	    Me [ player_num ] . angle = angle ;
	}
	else
	{ 
	    angle = Me [ player_num ] . angle ;
	}
    }
    
    //--------------------
    // From the angle we can compute the index to use...
    //
    rotation_index = ( angle * MAX_TUX_DIRECTIONS ) / 360.0 + ( MAX_TUX_DIRECTIONS / 2 )  ;
    while ( rotation_index >= MAX_TUX_DIRECTIONS ) rotation_index -= MAX_TUX_DIRECTIONS;
    while ( rotation_index < 0 ) rotation_index += MAX_TUX_DIRECTIONS;
    
    if ( Me [ player_num ] . weapon_item . type == (-1) )
    {
	iso_put_all_tux_parts_for_sword_motion ( x , y , player_num , rotation_index );
    }
    else
    {
	if ( ItemMap [ Me [ player_num ] . weapon_item . type ] . item_gun_angle_change > 0 )
	    iso_put_all_tux_parts_for_sword_motion ( x , y , player_num , rotation_index );
	else
	    iso_put_all_tux_parts_for_gun_motion ( x , y , player_num , rotation_index );
    }
    
}; // void iso_put_tux ( int x , int y , int player_num )

/* -----------------------------------------------------------------
 * This function draws the influencer to the screen, either
 * to the center of the combat window if (-1,-1) was specified, or
 * to the specified coordinates anywhere on the screen, useful e.g.
 * for using the influencer as a cursor in the menus.
 *
 * The given coordinates then indicate the UPPER LEFT CORNER for
 * the blit.
 * ----------------------------------------------------------------- */
void
blit_tux ( int x , int y , int player_num )
{
    SDL_Rect TargetRectangle;
    SDL_Rect Text_Rect;
    int alpha_value;
    point UpperLeftBlitCorner;
    
    Text_Rect . x = UserCenter_x + 21 ;
    Text_Rect . y = UserCenter_y  - 32 ;
    Text_Rect . w = ( User_Rect . w / 2 ) - 21 ;
    Text_Rect . h = ( User_Rect . h / 2 );
    
    DebugPrintf ( 2 , "\n%s(): real function call confirmed." , __FUNCTION__ ) ;

    if ( x == -1 ) 
    {
	//--------------------
	// The (-1) parameter indicates, that the tux should be drawn right 
	// into the game field at it's apropriate location.
	//
	// Well, for game purposes, we do not need to blit anything if the
	// tux is out, so we'll query for that first, as well as for the case
	// of other players that are not on this level.
	//
	if ( Me [ player_num ] . status == INFOUT ) return;
	if ( Me [ player_num ] . pos . z != Me [ 0 ] . pos . z ) return;
	
	UpperLeftBlitCorner.x = UserCenter_x - 32 ;
	UpperLeftBlitCorner.y = UserCenter_y - 32 ;
	
    }
    else
    {
	//--------------------
	// The not (-1) parameter indicates, that the tux should be drawn 
	// for cursor purposes.  This will be done anyway, regardless of
	// whether the tux is currently out or not.
	//
	UpperLeftBlitCorner.x=x ;
	UpperLeftBlitCorner.y=y ;
    }
    
    TargetRectangle.x = UpperLeftBlitCorner.x ;
    TargetRectangle.y = UpperLeftBlitCorner.y ;
    
    
    //--------------------
    // Maybe the influencer is fading due to low energy?
    // to achive this, is might be nescessary to add some 
    // alpha to the surface, that will later be
    // removed again.  We do this here:
    //
    
#define alpha_offset 80

    if ( ( ( Me [ player_num ] . energy * 100 / Me [ player_num ] . maxenergy ) <= BLINKENERGY ) && ( x == (-1) ) ) 
    {
	
	// In case of low energy, do the fading effect...
	alpha_value = (int) ( ( 256 - alpha_offset ) * 
			      fabsf( 0.5 * Me [ player_num ].MissionTimeElapsed - floor( 0.5 * Me [ player_num ].MissionTimeElapsed ) - 0.5 ) + 
			      ( alpha_offset ) );
	
	// ... and also maybe start a new cry-sound
	
	if ( Me [ player_num ].LastCrysoundTime > CRY_SOUND_INTERVAL )
	{
	    Me [ player_num ].LastCrysoundTime = 0;
	    CrySound();
	}
    }
    
    //--------------------
    // In case of transfer mode, we produce the transfer mode sound
    // but of course only in some periodic intervall...
    
    if ( Me [ player_num ].status == TRANSFERMODE )
    {
	if ( Me [ player_num ].LastTransferSoundTime > TRANSFER_SOUND_INTERVAL )
	{
	    Me [ player_num ].LastTransferSoundTime = 0;
	    TransferSound();
	}
    }
    
    //--------------------
    // Either we draw the classical influencer or we draw the more modern
    // tux, a descendant of the influencer :)
    //
    iso_put_tux ( x , y , player_num );
    
    //--------------------
    // Now that all fading effects are done, we can restore the blocks surface to OPAQUE,
    // which is the oposite of TRANSPARENT :)
    //
    
    //--------------------
    // Maybe the influencer has something to say :)
    // so let him say it..
    //
    if ( ( x == (-1) ) && ( Me [ player_num ].TextVisibleTime < GameConfig.WantedTextVisibleTime ) && GameConfig.All_Texts_Switch )
    {
	SetCurrentFont( FPS_Display_BFont );
	DisplayText( Me [ player_num ] . TextToBeDisplayed , UserCenter_x + 21 ,
		     UserCenter_y - 32 , &Text_Rect , TEXT_STRETCH );
    }

    //--------------------
    // Maybe there are some status flags active associated with
    // the Tux...
    //
    blit_tux_status_flags ( );
    
    DebugPrintf (2, "\n%s(): enf of function reached." , __FUNCTION__ );

}; // void blit_tux( int x , int y )

/* ----------------------------------------------------------------------
 * If the corresponding configuration flag is enabled, enemies might 'say'
 * some text comment on the screen, like 'ouch' or 'i'll getch' or 
 * something else more sensible.  This function is here to blit these
 * comments, that must have been set before, to the screen.
 * ---------------------------------------------------------------------- */
void
PrintCommentOfThisEnemy ( int Enum )
{
    int x_pos, y_pos;
    char phase_text[200];
    
    //--------------------
    // At this point we can assume, that the enemys has been blittet to the
    // screen, whether it's a friendly enemy or not.
    // 
    // So now we can add some text the enemys says.  That might be fun.
    //
    if ( ( AllEnemys [ Enum ] . TextVisibleTime < GameConfig . WantedTextVisibleTime )
	 && GameConfig . All_Texts_Switch )
    {
	x_pos = translate_map_point_to_screen_pixel ( AllEnemys[ Enum ] . virt_pos . x , AllEnemys [ Enum ] . virt_pos . y , TRUE );
	y_pos = translate_map_point_to_screen_pixel ( AllEnemys[ Enum ] . virt_pos . x , AllEnemys [ Enum ] . virt_pos . y , FALSE )
	    - 100 ;
	
	//--------------------
	// First we display the normal text to be displayed...
	//
	PutStringFont ( Screen , FPS_Display_BFont , 
			x_pos , y_pos ,  
			AllEnemys[Enum].TextToBeDisplayed );
	
	//--------------------
	// Now we add some more debug info here...
	//
	sprintf ( phase_text , "a-phase: %3.3f a-type: %d" , AllEnemys [ Enum ] . animation_phase , AllEnemys [ Enum ] . animation_type );
	/*
	  PutStringFont ( Screen , FPS_Display_BFont , 
	  x_pos , y_pos + FontHeight ( FPS_Display_BFont ) ,  
	  phase_text );
	  sprintf ( phase_text , "speed: %3.3fx %3.3fy" , AllEnemys [ Enum ] . speed . x , AllEnemys [ Enum ] . speed . y );
	  PutStringFont ( Screen , FPS_Display_BFont , 
	  x_pos , y_pos + 2 * FontHeight ( FPS_Display_BFont ) ,  
	  phase_text );
	*/
    }
    
}; // void PrintCommentOfThisEnemy ( int Enum, int x, int y )

/* ----------------------------------------------------------------------
 * Not every enemy has to be blitted onto the combat screen every time.
 * This function is here to find out whether this enemy has to be blitted
 * or whether we can skip it.
 * ---------------------------------------------------------------------- */
int
ThisEnemyNeedsToBeBlitted ( int Enum , int x , int y )
{
    // if enemy is on other level, return 
    if ( AllEnemys [ Enum ] . virt_pos . z != Me [ 0 ] . pos . z )
    {
	return FALSE;
    }
    
    // if enemy is of type (-1), return 
    if ( AllEnemys[Enum].type == ( -1 ) )
    {
	return FALSE ;
    }
    
    if ( ! MakeSureEnemyIsInsideHisLevel ( &(AllEnemys[Enum] ) ) ) return ( FALSE );
    
    // if the enemy is out of sight, we need not do anything more here
    if ( ( ! show_all_droids ) && ( ! IsVisible ( & AllEnemys [ Enum ] . virt_pos , 0 ) ) )
    {
	return FALSE ;
    }
    
    return TRUE;
    
}; // int ThisEnemyNeedsToBeBlitted ( int Enum , int x , int y )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
PutEnemyEnergyBar ( int Enum , SDL_Rect TargetRectangle )
{
    float Percentage;
    SDL_Rect FillRect;
    static Uint32 full_color_enemy ;
    static Uint32 full_color_friend ;
    static Uint32 energy_empty_color ;
   
    int x, y , w ,h;
    myColor c1 = {0,0,0,255} ;
    myColor c2 = {0,0,0,255} ;

    #define ENEMY_ENERGY_BAR_OFFSET_X 0
    #define ENEMY_ENERGY_BAR_OFFSET_Y (-20)
    #define ENEMY_ENERGY_BAR_LENGTH 65

    #define ENEMY_ENERGY_BAR_WIDTH 7 

    //--------------------
    // If the enemy is dead already, there's nothing to do here...
    //
    if ( AllEnemys [ Enum ] . Status == INFOUT ) return;
    if ( AllEnemys [ Enum ] . energy <= 0 ) return;
    
    //--------------------
    // Now we need to find the right colors to fill our bars with...
    //
    full_color_enemy = SDL_MapRGB( Screen->format, 255 , 0 , 0 ) ; 
    full_color_friend = SDL_MapRGB( Screen->format, 0 , 255 , 0 ) ; 
    energy_empty_color = SDL_MapRGB( Screen->format, 0 , 0 , 0 ) ; 
    
    //--------------------
    // work out the percentage health
    //
    Percentage = ( AllEnemys [ Enum ] . energy ) / Druidmap [ AllEnemys [ Enum ] . type ] . maxenergy ;
    
    if ( use_open_gl ) {
	
#ifdef HAVE_LIBGL
	float PercentageDone = 0;
	int barnum = 0;
	for ( ; Percentage > 0; Percentage -= PercentageDone, barnum ++)
		{
		if ( Percentage >= 1 ) PercentageDone = 1;
		else PercentageDone = Percentage;
		// draw cool bars here
		x = TargetRectangle . x ;
		y = TargetRectangle . y - 10 * barnum;
		w = TargetRectangle . w ;
		h = TargetRectangle . h ;
	
		if ( AllEnemys [ Enum ] . is_friendly ) 
		    c1.g = 255;
		else
		    c1.r = 255;
	
		// tweak as needed, this alters the transparency
		c1.a = 140 ;
		drawIsoEnergyBar( Z_DIR, x, y, 1, 5, 5, w, PercentageDone, &c1, &c2 ) ;
		}
	
#endif
	
    } 
    else 
    {
	//sdl stuff here
	
	FillRect . x = TargetRectangle . x ;
	FillRect . y = TargetRectangle . y - ENEMY_ENERGY_BAR_WIDTH - ENEMY_ENERGY_BAR_OFFSET_Y ;
	FillRect . h = ENEMY_ENERGY_BAR_WIDTH ; 
	FillRect . w = Percentage * TargetRectangle . w ;
	
	//--------------------
	// If the enemy is friendly, then we needn't display his health, right?
	// Or better yet, we might show a green energy bar instead.  That's even
	// better!
	if ( AllEnemys [ Enum ] . is_friendly ) 
	    our_SDL_fill_rect_wrapper ( Screen , &FillRect , full_color_friend ) ;
	else
	    our_SDL_fill_rect_wrapper ( Screen , &FillRect , full_color_enemy ) ;
	
	//--------------------
	// Now after the energy bar has been drawn, we can start to draw the
	// empty part of the energy bar (but only of course, if there is some
	// empty part at all!  (Otherwise we get indefinately large energy
	// bars...
	FillRect . x += (Percentage * TargetRectangle . w) ;
	FillRect . w = (1-Percentage) * TargetRectangle . w ;
	
	if ( Percentage < 1.0 )
	    our_SDL_fill_rect_wrapper ( Screen , &FillRect , energy_empty_color ) ;
    }
    
}; // void PutEnemyEnergyBar ( Enum , TargetRectangle )

/* ----------------------------------------------------------------------
 * The direction this robot should be facing right now is determined and
 * properly set in this function.
 * ---------------------------------------------------------------------- */
int
set_rotation_index_for_this_robot ( enemy* ThisRobot ) 
{
    int RotationIndex;

    //--------------------
    // By now the angle the robot is facing is determined, so we just need to
    // translate this angle into an index within the image series, i.e. into 
    // a 'phase' of rotation. 
    //
    RotationIndex = ( ( ThisRobot -> current_angle - 45.0 + 360.0 + 360 / 
			( 2 * ROTATION_ANGLES_PER_ROTATION_MODEL ) ) * ROTATION_ANGLES_PER_ROTATION_MODEL / 360 ) ;

    //--------------------
    // But it might happen, that the angle of rotation is 'out of scale' i.e.
    // it's more than 360 degree or less than 0 degree.  Therefore, we need to
    // be especially careful to generate only proper indices for our arrays.
    // Some would say, we identify the remainder classes with integers in the
    // range [ 0 - (rotation_angles-1) ], which is what's happening here.
    //
    while ( RotationIndex < 0  ) 
	RotationIndex += ROTATION_ANGLES_PER_ROTATION_MODEL ;
    while ( RotationIndex >= ROTATION_ANGLES_PER_ROTATION_MODEL ) 
	RotationIndex -= ROTATION_ANGLES_PER_ROTATION_MODEL ; 
    
    //--------------------
    // Now to prevent some jittering in some cases, where the droid uses an angle that is
    // right at the borderline between two possible 8-way directions, we introduce some
    // enforced consistency onto the droid...
    //
    if ( RotationIndex == ThisRobot -> previous_phase )
    {
	ThisRobot -> last_phase_change += Frame_Time ();
    }
    else
    {
	if ( ThisRobot -> last_phase_change >= 0.7 )
	{
	    ThisRobot -> last_phase_change = 0.0 ;
	    ThisRobot -> previous_phase = RotationIndex ;
	}
	else
	{
	    //--------------------
	    // In this case we don't permit to use a new 8-way direction now...
	    //
	    RotationIndex = ThisRobot -> previous_phase ;
	    ThisRobot -> last_phase_change += Frame_Time ();
	}
    }
    
    // DebugPrintf ( 0 , "\nCurrent angle: %f Current RotationIndex: %d. " , angle, RotationIndex );
    
    return ( RotationIndex );
  
}; // int set_rotation_index_for_this_robot ( enemy* ThisRobot ) 

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
int
set_rotation_model_for_this_robot ( enemy* ThisRobot ) 
{
    int RotationModel = Druidmap [ ThisRobot -> type ] . individual_shape_nr ;
    
    //--------------------
    // A sanity check for roation model to use can never hurt...
    //
    if ( ( RotationModel < 0 ) || ( RotationModel >= ENEMY_ROTATION_MODELS_AVAILABLE ) )
    {
	GiveStandardErrorMessage ( __FUNCTION__  , "\
There was a rotation model type given, that exceeds the number of rotation models allowed and loaded in Freedroid.",
				   PLEASE_INFORM, IS_FATAL );
    }
    
    return ( RotationModel );
    
}; // int set_rotation_model_for_this_robot ( enemy* ThisRobot ) 

/* ----------------------------------------------------------------------
 * This function is here to blit the 'body' of a droid to the screen, 
 * but the 'body' in the new and more modern sense with the 3d models
 * in various rotated forms as they are provided by Bastian.
 * This shape now depends upon the behaviour of the droid, which makes
 * everthing a little bit more complicated.
 * ---------------------------------------------------------------------- */
void
PutIndividuallyShapedDroidBody ( int Enum , SDL_Rect TargetRectangle , int mask , int highlight )
{
    int phase = AllEnemys [ Enum ] . phase;
    int RotationModel;
    int RotationIndex;
    float darkness ;
    enemy* ThisRobot = & ( AllEnemys [ Enum ] ) ;
    moderately_finepoint bot_pos;

    // if ( ThisRobot -> pos . z != Me [ 0 ] . pos . z )
    // DebugPrintf ( -4 , "\n%s(): Now attempting to blit bot on truly virtual position..." , __FUNCTION__ );

    //--------------------
    // We properly set the direction this robot is facing.
    //
    RotationIndex = set_rotation_index_for_this_robot ( ThisRobot ) ;
    
    //--------------------
    // We properly set the rotation model number for this robot, i.e.
    // which shape (like 302, 247 or proffa) to use for drawing this bot.
    //
    RotationModel = set_rotation_model_for_this_robot ( ThisRobot ) ;
    
    //--------------------
    // If the robot is dead and doesn't have a dead image, then we need not
    // do anything else here...
    //
    if ( ( phase == DROID_PHASES ) &&
	 ( ! Druidmap [ ThisRobot -> type ] . use_image_archive_file ) )
    {
	// asdf
	// DebugPrintf ( -4 , "\n%s(): Droid blitting omitted because of no death and no animation." , __FUNCTION__ );
	// return;
    }
    
    //--------------------
    // Some extra security against strange or undefined animation phases
    //
    

    //--------------------
    // Maybe the rotation model we're going to use now isn't yet loaded. 
    // Now in this case, we must load it immediately, or a segfault may
    // result...
    //
    LoadAndPrepareEnemyRotationModelNr ( RotationModel );
    
    //--------------------
    // Maybe we don't have an enemy here that would really stick to the 
    // exact size of a block but be somewhat bigger or smaller instead.
    // In this case, we'll just adapt the given target rectangle a little
    // bit, cause this rectangle assumes exactly the same size as a map 
    // block and has the origin shifted accordingly.
    //
    if ( ( TargetRectangle . x != 0 ) && ( TargetRectangle . y != 0 ) )
    {
	if ( use_open_gl )
	{
	    TargetRectangle.x -= ( enemy_iso_images[ RotationModel ] [ RotationIndex ] [ 0 ] . original_image_width ) / 2 ;
	    TargetRectangle.y -= ( enemy_iso_images[ RotationModel ] [ RotationIndex ] [ 0 ] . original_image_height ) / 2 ;
	    TargetRectangle.w = enemy_iso_images[ RotationModel ] [ RotationIndex ] [ 0 ] . original_image_width ;
	    TargetRectangle.h = enemy_iso_images[ RotationModel ] [ RotationIndex ] [ 0 ] . original_image_height ;
	}
	else
	{
	    TargetRectangle.x -= ( enemy_iso_images[ RotationModel ] [ RotationIndex ] [ 0 ] . surface -> w ) / 2 ;
	    TargetRectangle.y -= ( enemy_iso_images[ RotationModel ] [ RotationIndex ] [ 0 ] . surface -> h ) / 2 ;
	    TargetRectangle.w = enemy_iso_images[ RotationModel ] [ RotationIndex ] [ 0 ] . surface -> w;
	    TargetRectangle.h = enemy_iso_images[ RotationModel ] [ RotationIndex ] [ 0 ] . surface -> h;
	}
    }
    
    //--------------------
    // Maybe the enemy is desired e.g. for the takeover game, so a pixel position on
    // the screen is given and we blit the enemy to that position, not taking into 
    // account any map coordinates or stuff like that...
    //
    if ( ( TargetRectangle . x != 0 ) && ( TargetRectangle . y != 0 ) )
    {
	if ( use_open_gl )
	{
	    blit_open_gl_texture_to_screen_position ( enemy_iso_images [ RotationModel ] [ RotationIndex ] [ 0 ] , TargetRectangle . x , TargetRectangle . y , TRUE );
	}
	else
	{
	    our_SDL_blit_surface_wrapper ( 
		enemy_iso_images [ RotationModel ] [ RotationIndex ] [ 0 ] . surface , 
		NULL , Screen, &TargetRectangle );
	}
	return;
    }

    //--------------------
    // But here we know, that the enemy is desired inside the game, so we need to
    // taking into account map coordinates and all that stuff...
    //
    else
    {
	
	if ( mask & ZOOM_OUT )
	{
	    if ( use_open_gl )
	    {
		if ( ThisRobot -> paralysation_duration_left != 0 ) 
		{
		    blit_zoomed_open_gl_texture_to_map_position ( enemy_iso_images[ RotationModel ] [ RotationIndex ] [ 0 ] , ThisRobot -> virt_pos . x , ThisRobot -> virt_pos . y , 1.0 , 0.2 , 0.2 , highlight, FALSE ) ;
		}
		else if ( ThisRobot -> poison_duration_left != 0 ) 
		{
		    blit_zoomed_open_gl_texture_to_map_position ( enemy_iso_images[ RotationModel ] [ RotationIndex ] [ 0 ] , ThisRobot -> virt_pos . x , ThisRobot -> virt_pos . y , 0.2 , 1.0 , 0.2 , highlight, FALSE ) ;
		}
		else if ( ThisRobot -> frozen != 0 ) 
		{
		    blit_zoomed_open_gl_texture_to_map_position ( enemy_iso_images[ RotationModel ] [ RotationIndex ] [ 0 ] , ThisRobot -> virt_pos . x , ThisRobot -> virt_pos . y , 0.2 , 0.2 , 1.0 , highlight, FALSE ) ;
		}
		else
		{
		    blit_zoomed_open_gl_texture_to_map_position ( enemy_iso_images[ RotationModel ] [ RotationIndex ] [ 0 ] , ThisRobot -> virt_pos . x , ThisRobot -> virt_pos . y , 1.0 , 1.0 , 1.0 , highlight, FALSE ) ;
		}
	    }
	    else
	    {
		//--------------------
		// When no OpenGL is used, we need to proceed with SDL for
		// blitting the small enemies...
		//
		blit_zoomed_iso_image_to_map_position ( & ( enemy_iso_images[ RotationModel ] [ RotationIndex ] [ 0 ] ) , 
							ThisRobot -> virt_pos . x , ThisRobot -> virt_pos . y );
	    }
	}
	else
	{
	    
	    //--------------------
	    // Maybe we've got to do with some old bots, that don't have any movement
	    // animation phases yet, until Basse will provide them at some later point.
	    // This case must be handled separatedly...
	    //
	    if ( ! Druidmap [ ThisRobot -> type ] . use_image_archive_file )
	    {
		if ( use_open_gl )
		{
		    
		    if ( ThisRobot -> paralysation_duration_left != 0 ) 
		    {
			blit_open_gl_texture_to_map_position ( enemy_iso_images[ RotationModel ] [ RotationIndex ] [ 0 ] , ThisRobot -> virt_pos . x , ThisRobot -> virt_pos . y , 1.0 , 0.2 , 0.2 , highlight , FALSE) ;
		    }
		    else if ( ThisRobot -> poison_duration_left != 0 ) 
		    {
			blit_open_gl_texture_to_map_position ( enemy_iso_images[ RotationModel ] [ RotationIndex ] [ 0 ] , ThisRobot -> virt_pos . x , ThisRobot -> virt_pos . y , 0.2 , 1.0 , 0.2 , highlight , FALSE) ;
		    }
		    else if ( ThisRobot -> frozen != 0 ) 
		    {
			blit_open_gl_texture_to_map_position ( enemy_iso_images[ RotationModel ] [ RotationIndex ] [ 0 ] , ThisRobot -> virt_pos . x , ThisRobot -> virt_pos . y , 0.2 , 0.2 , 1.0 , highlight , FALSE) ;
		    }
		    else
		    {
			
			//--------------------
			// If we're using OpenGL, we can as well apply the darkness to the droids
			// we're about to blit...
			//
			bot_pos . x = ThisRobot -> virt_pos . x ;
			bot_pos . y = ThisRobot -> virt_pos . y ;
			
			darkness = 1.5 - 2.0 * ( ( (float) get_light_strength ( bot_pos ) ) / ( (float) NUMBER_OF_SHADOW_IMAGES ) ) ;
			if ( darkness > 1 ) darkness = 1.0 ;
			if ( darkness < 0 ) darkness = 0 ;
			
			blit_open_gl_texture_to_map_position ( enemy_iso_images[ RotationModel ] [ RotationIndex ] [ 0 ] , ThisRobot -> virt_pos . x , ThisRobot -> virt_pos . y , darkness , darkness , darkness , highlight , FALSE) ;
		    }		  
		    
		}
		else // no open_gl
		{
		    
		    //--------------------
		    // First we catch the case of dead bots, then we can separate the
		    // right color filter type, provided that the droid is still alive...
		    //
		    if ( ( ThisRobot -> energy <= 0 ) || ( ThisRobot -> Status ==  INFOUT ) )
		    {
			blit_iso_image_to_map_position ( enemy_iso_images [ RotationModel ] [ RotationIndex ] [ (int) ThisRobot -> animation_phase ] , 
							 ThisRobot -> virt_pos . x , ThisRobot -> virt_pos . y );
			
		    }
		    else if ( ThisRobot -> paralysation_duration_left != 0 ) 
		    {
			LoadAndPrepareRedEnemyRotationModelNr ( RotationModel );
			blit_iso_image_to_map_position ( RedEnemyRotationSurfacePointer [ RotationModel ] [ RotationIndex ] [ 0 ] , 
							 ThisRobot -> virt_pos . x , ThisRobot -> virt_pos . y );
		    }
		    else if ( ThisRobot -> poison_duration_left != 0 ) 
		    {
			LoadAndPrepareGreenEnemyRotationModelNr ( RotationModel );
			blit_iso_image_to_map_position ( GreenEnemyRotationSurfacePointer [ RotationModel ] [ RotationIndex ] [ 0 ] , 
							 ThisRobot -> virt_pos . x , ThisRobot -> virt_pos . y );
		    }
		    else if ( ThisRobot -> frozen != 0 ) 
		    {
			LoadAndPrepareBlueEnemyRotationModelNr ( RotationModel );
			blit_iso_image_to_map_position ( BlueEnemyRotationSurfacePointer [ RotationModel ] [ RotationIndex ] [ 0 ] , 
							 ThisRobot -> virt_pos . x , ThisRobot -> virt_pos . y );
		    }
		    else
		    {
			blit_iso_image_to_map_position ( enemy_iso_images [ RotationModel ] [ RotationIndex ] [ (int) ThisRobot -> animation_phase ] , ThisRobot -> virt_pos . x , ThisRobot -> virt_pos . y );
			if ( highlight )
			    blit_outline_of_iso_image_to_map_position ( enemy_iso_images [ RotationModel ] [ RotationIndex ] [ (int) ThisRobot -> animation_phase ] , ThisRobot -> virt_pos . x , ThisRobot -> virt_pos . y );
		    }
		    
		    // blit_iso_image_to_map_position ( enemy_iso_images[ RotationModel ] [ RotationIndex ] [ 0 ] , ThisRobot -> virt_pos . x , ThisRobot -> virt_pos . y );
		    
		}
	    }
	    //--------------------
	    // So here we got some animation phases, not only one image.  That's cool,
	    // especially when using OpenGL for graphics output, since then we can have
	    // fine color-filteres animation without extra effort.  With SDL output, the
	    // color-filteres surfaces must be reduced to the first cycle image.
	    //
	    else
	    {
		if ( use_open_gl )
		{
		    if ( ThisRobot -> paralysation_duration_left != 0 ) 
		    {
			blit_open_gl_texture_to_map_position ( enemy_iso_images[ RotationModel ] [ RotationIndex ] [ (int) ThisRobot -> animation_phase ] , 
							       ThisRobot -> virt_pos . x , ThisRobot -> virt_pos . y , 1.0 , 0.2 , 0.2 , highlight, FALSE ) ;
		    }
		    else if ( ThisRobot -> poison_duration_left != 0 ) 
		    {
			blit_open_gl_texture_to_map_position ( enemy_iso_images[ RotationModel ] [ RotationIndex ] [ (int) ThisRobot -> animation_phase ] , 
							       ThisRobot -> virt_pos . x , ThisRobot -> virt_pos . y , 0.2 , 1.0 , 0.2 , highlight, FALSE ) ;
		    }
		    else if ( ThisRobot -> frozen != 0 ) 
		    {
			blit_open_gl_texture_to_map_position ( enemy_iso_images[ RotationModel ] [ RotationIndex ] [ (int) ThisRobot -> animation_phase ] , 
							       ThisRobot -> virt_pos . x , ThisRobot -> virt_pos . y , 0.2 , 0.2 , 1.0 , highlight , FALSE) ;
		    }
		    else
		    {
			//--------------------
			// If we're using OpenGL, we can as well apply the darkness to the droids
			// we're about to blit...
			//
			bot_pos . x = ThisRobot -> virt_pos . x ;
			bot_pos . y = ThisRobot -> virt_pos . y ;
			
			darkness = 1.5 - 2.0 * ( ( (float) get_light_strength ( bot_pos ) ) / ( (float) NUMBER_OF_SHADOW_IMAGES ) ) ;
			if ( darkness > 1 ) darkness = 1.0 ;
			if ( darkness < 0 ) darkness = 0 ;
			float locx, locy;
	                float endlocx, endlocy;
        	        locx = translate_map_point_to_screen_pixel_deviation_tracking( bot_pos . x, bot_pos.y, TRUE);
	                locy = translate_map_point_to_screen_pixel_deviation_tracking( bot_pos . x, bot_pos.y, FALSE);
        	        endlocx = (locx - UserCenter_x) / (float) iso_floor_tile_width + Me[0].pos.x + (locy - UserCenter_y ) / (float) iso_floor_tile_height;
	                endlocy = (- locx + UserCenter_x) / (float) iso_floor_tile_width + Me[0].pos.y + (locy - UserCenter_y) / (float) iso_floor_tile_height;

			blit_open_gl_texture_to_map_position ( 
			    enemy_iso_images [ RotationModel ] [ RotationIndex ] [ (int) ThisRobot -> animation_phase ] , 
			    endlocx , endlocy , 
			    darkness , darkness , darkness , highlight , FALSE) ;
		    }
		}
		else // no OpenGL
		{
		    
		    //--------------------
		    // First we catch the case of a dead bot (no color filteres SDL surfaces
		    // availabe for that case).  In the other cases, we use the prepared color-
		    // filtered stuff...
		    // 
		    if ( ( ThisRobot -> energy <= 0 ) || ( ThisRobot -> Status == INFOUT ) )
		    {
			blit_iso_image_to_map_position ( enemy_iso_images [ RotationModel ] [ RotationIndex ] [ (int) ThisRobot -> animation_phase ] , ThisRobot -> virt_pos . x , ThisRobot -> virt_pos . y );
		    }
		    else if ( ThisRobot -> paralysation_duration_left != 0 ) 
		    {
			LoadAndPrepareRedEnemyRotationModelNr ( RotationModel );
			blit_iso_image_to_map_position ( RedEnemyRotationSurfacePointer [ RotationModel ] [ RotationIndex ] [ 0 ] , 
							 ThisRobot -> virt_pos . x , ThisRobot -> virt_pos . y );
		    }
		    else if ( ThisRobot -> poison_duration_left != 0 ) 
		    {
			LoadAndPrepareGreenEnemyRotationModelNr ( RotationModel );
			blit_iso_image_to_map_position ( GreenEnemyRotationSurfacePointer [ RotationModel ] [ RotationIndex ] [ 0 ] , 
							 ThisRobot -> virt_pos . x , ThisRobot -> virt_pos . y );
		    }
		    else if ( ThisRobot -> frozen != 0 ) 
		    {
			LoadAndPrepareBlueEnemyRotationModelNr ( RotationModel );
			blit_iso_image_to_map_position ( BlueEnemyRotationSurfacePointer [ RotationModel ] [ RotationIndex ] [ 0 ] , 
							 ThisRobot -> virt_pos . x , ThisRobot -> virt_pos . y );
		    }
		    else
		    {
			blit_iso_image_to_map_position ( enemy_iso_images [ RotationModel ] [ RotationIndex ] [ (int) ThisRobot -> animation_phase ] , ThisRobot -> virt_pos . x , ThisRobot -> virt_pos . y );
			if ( highlight )
			    blit_outline_of_iso_image_to_map_position ( enemy_iso_images [ RotationModel ] [ RotationIndex ] [ (int) ThisRobot -> animation_phase ] , ThisRobot -> virt_pos . x , ThisRobot -> virt_pos . y );
		    }
		    
		}
	    }
	}
	
	TargetRectangle . x = 
	    translate_map_point_to_screen_pixel ( ThisRobot -> virt_pos.x , ThisRobot -> virt_pos.y , TRUE );
	TargetRectangle . y = 
	    translate_map_point_to_screen_pixel ( ThisRobot -> virt_pos.x , ThisRobot -> virt_pos.y , FALSE ) ;
	// - ENEMY_ENERGY_BAR_OFFSET_Y ;
	  
	if ( use_open_gl )
	{
	    //--------------------
	    // Newly, we also make textures out of all enemy surfaces...
	    // This will prove to be very handy for purposes of color filtered
	    // output and such things...
	    //
	    TargetRectangle.x -= ( enemy_iso_images[ RotationModel ] [ RotationIndex ] [ 0 ] . original_image_width ) / 2 ;
	    TargetRectangle.y -= ( enemy_iso_images[ RotationModel ] [ RotationIndex ] [ 0 ] . original_image_height ) / 1 ;
	    TargetRectangle.w = enemy_iso_images[ RotationModel ] [ RotationIndex ] [ 0 ] . original_image_width ;
	    TargetRectangle.h = enemy_iso_images[ RotationModel ] [ RotationIndex ] [ 0 ] . original_image_height ;
	}
	else
	{
	    TargetRectangle.x -= ( enemy_iso_images[ RotationModel ] [ RotationIndex ] [ 0 ] . surface -> w ) / 2 ;
	    TargetRectangle.y -= ( enemy_iso_images[ RotationModel ] [ RotationIndex ] [ 0 ] . surface -> h ) / 1 ;
	    TargetRectangle.w = enemy_iso_images[ RotationModel ] [ RotationIndex ] [ 0 ] . surface -> w;
	    TargetRectangle.h = enemy_iso_images[ RotationModel ] [ RotationIndex ] [ 0 ] . surface -> h;
	}
	
	if ( GameConfig . enemy_energy_bars_visible )
	    PutEnemyEnergyBar ( Enum , TargetRectangle );
	return;
    }
    
}; // void PutIndividuallyShapedDroidBody ( int Enum , SDL_Rect TargetRectangle );

/* ----------------------------------------------------------------------
 * This function draws an enemy into the combat window.
 * The only parameter given is the number of the enemy within the
 * AllEnemys array. Everything else is computed in here.
 * ---------------------------------------------------------------------- */
void
PutEnemy ( int Enum , int x , int y , int mask , int highlight )
{
    SDL_Rect TargetRectangle;
    
    //--------------------
    // We check for things like visibility and distance and the like,
    // so that we know whether to consider this enemy for blitting to
    // the screen or not.  Since there are many things to consider, we
    // got a special function for this job.
    //
    if ( ( ! ThisEnemyNeedsToBeBlitted ( Enum , x , y ) ) && ( !xray_vision_for_tux ) ) return;
    
    //--------------------
    // We check for incorrect droid types, which sometimes might occor, especially after
    // heavy editing of the crew initialisation functions ;)
    //
    if ( AllEnemys[Enum].type >= Number_Of_Droid_Types )
    {
	GiveStandardErrorMessage ( __FUNCTION__  , "\
There was a droid type on this level, that does not really exist.",
				   PLEASE_INFORM, IS_FATAL );
	AllEnemys[Enum].type = 0;
    }
    
    //--------------------
    // Since we will need that several times in the sequel, we find out the correct
    // target location on the screen for our surface blit once and remember it for
    // later.  ( THE TARGET RECTANGLE GETS MODIFIED IN THE SDL BLIT!!! )
    //
    if ( x == (-1) ) 
    {
	TargetRectangle.x = 0 ;
	TargetRectangle.y = 0 ;
    }
    else
    {
	TargetRectangle.x = x ;
	TargetRectangle.y = y ;
    }
    
    PutIndividuallyShapedDroidBody ( Enum , TargetRectangle , mask , highlight );
    
    //--------------------
    // Only if this robot is not dead, we consider printing the comments
    // this robot might have to make on the current situation.
    //
    if ( AllEnemys [ Enum ] . Status != INFOUT) PrintCommentOfThisEnemy ( Enum );
    
}; // void PutEnemy(int Enum , int x , int y) 

/* ----------------------------------------------------------------------
 * This function draws a Bullet into the combat window.  The only 
 * parameter given is the number of the bullet in the AllBullets 
 * array. Everything else is computed in here.
 * ---------------------------------------------------------------------- */
void
PutBullet ( int bullet_index , int mask )
{
    bullet* CurBullet = & ( AllBullets [ bullet_index ] ) ;
    int PhaseOfBullet;
    int direction_index;
    
    if ( CurBullet -> time_to_hide_still > 0 ) 
	return ;
    
    //--------------------
    // in case our bullet is of the type "FLASH", we only
    // draw a big white or black rectangle right over the 
    // combat window, white for even frames and black for 
    // odd frames.
    if ( CurBullet -> type == FLASH )
    {
	// Now the whole window will be filled with either white
	// or black each frame until the flash is over.  (Flash 
	// deletion after some time is done in CheckBulletCollisions.)
	if ( ( CurBullet -> time_in_frames % 2 ) == 1)
	{
	    FdFlashWindow ( flashcolor1 );
	    return;
	}
	if ( ( CurBullet -> time_in_frames % 2 ) == 0)
	{
	    FdFlashWindow ( flashcolor2 );
	    return;
	}
    } // if type == FLASH
    
    // DebugPrintf( 0 , "\nBulletType before calculating phase : %d." , CurBullet->type );
    if ( ( CurBullet -> type >= Number_Of_Bullet_Types ) ||
	 ( CurBullet -> type <  0                      ) )
    {
	fprintf ( stderr , "\nPutBullet:  bullet type received: %d." , CurBullet -> type );
	fflush ( stderr );
	GiveStandardErrorMessage ( __FUNCTION__  , "\
There was a bullet to be blitted of a type that does not really exist.",
				   PLEASE_INFORM, IS_FATAL );
    };
    
    PhaseOfBullet = CurBullet -> time_in_seconds * Bulletmap [ CurBullet -> type ] . phase_changes_per_second ;
    
    PhaseOfBullet = PhaseOfBullet % Bulletmap [ CurBullet -> type ] . phases ;
    // DebugPrintf( 0 , "\nPhaseOfBullet: %d.", PhaseOfBullet );
    
    direction_index = ( ( CurBullet -> angle + 360.0 + 360 / ( 2 * BULLET_DIRECTIONS ) ) * BULLET_DIRECTIONS / 360 ) ;
    while ( direction_index < 0  ) direction_index += BULLET_DIRECTIONS ; // just to make sure... a modulo ROTATION_ANGLES_PER_ROTATION_MODEL operation can't hurt
    while ( direction_index >= BULLET_DIRECTIONS ) direction_index -= BULLET_DIRECTIONS ; // just to make sure... a modulo ROTATION_ANGLES_PER_ROTATION_MODEL operation can't hurt
    
    if ( mask & ZOOM_OUT )
    {
	// blit_zoomed_iso_image_to_map_position ( & ( Bulletmap [ CurBullet -> type ] . image [ direction_index ] [ PhaseOfBullet ] ) , CurBullet -> pos . x , CurBullet -> pos . y );
    }
    else
	blit_iso_image_to_map_position ( Bulletmap [ CurBullet -> type ] . image [ direction_index ] [ PhaseOfBullet ] , CurBullet -> pos . x , CurBullet -> pos . y );
    
}; // void PutBullet (int Bulletnumber )

/* ----------------------------------------------------------------------
 * This function draws an item into the combat window.
 * The only given parameter is the number of the item within
 * the AllItems array.
 * ---------------------------------------------------------------------- */
void
PutItem( int ItemNumber , int mask , int put_thrown_items_flag , int highlight_item )
{
    Level ItemLevel = curShip . AllLevels [ Me [ 0 ] . pos . z ] ;
    Item CurItem = &ItemLevel -> ItemList [ ItemNumber ] ;

    //--------------------
    // The unwanted cases MUST be handled first...
    //
    if ( CurItem->type == ( -1 ) ) 
    {
	return;
	fprintf( stderr, "\n\nItemNumber '%d'\n" , ItemNumber );
	GiveStandardErrorMessage ( __FUNCTION__  , "\
There was -1 item type given to blit.  This must be a mistake! ",
				   PLEASE_INFORM, IS_FATAL );
    }
    // We don't blit any item, that we're currently holding in our hand, do we?
    if ( CurItem->currently_held_in_hand == TRUE ) return;
    
    //--------------------
    // In case the flag filters this item, we don't blit it
    //
    if ( ( put_thrown_items_flag == PUT_ONLY_THROWN_ITEMS ) &&
	 ( CurItem -> throw_time <= 0 ) ) 
	return;
    if ( ( put_thrown_items_flag == PUT_NO_THROWN_ITEMS ) &&
	 ( CurItem -> throw_time > 0 ) ) 
	return;
    
    //--------------------
    // Now we can go take a look if maybe there is an ingame surface 
    // for this item available.  If not, the function will automatically
    // load the inventory surface instead, so we really can assume that
    // we have something to use afterwards.
    //
    if ( ( ItemMap [ CurItem -> type ] . inv_image . ingame_iso_image . surface == NULL ) &&
	 ( ! ItemMap [ CurItem -> type ] . inv_image . ingame_iso_image . texture_has_been_created ) )
	try_to_load_ingame_item_surface ( CurItem -> type );
    
    //--------------------
    // When zoomed out, you can't see any items clearly anyway...
    //
    if ( mask & ZOOM_OUT )
    {
	if ( use_open_gl )
	{
	    blit_zoomed_open_gl_texture_to_map_position ( ItemMap [ CurItem -> type ] . inv_image . ingame_iso_image , 
							  CurItem -> pos . x , CurItem -> pos . y , 1.0 , 1.0 , 1.0 , 0.25, FALSE );
	}
	else
	{
	    blit_zoomed_iso_image_to_map_position ( & ( ItemMap [ CurItem -> type ] . inv_image . ingame_iso_image ) , 
						    CurItem -> pos . x , CurItem -> pos . y );
	}
    }
    else
    {
	if ( use_open_gl )
	{
	    blit_open_gl_texture_to_map_position ( ItemMap [ CurItem -> type ] . inv_image . ingame_iso_image , 
						   CurItem -> pos . x - 3.0 * sinf ( CurItem -> throw_time * 3.0 ) , 
						   CurItem -> pos . y - 3.0 * sinf ( CurItem -> throw_time * 3.0 ) , 
						   1.0 , 1.0 , 1.0 , highlight_item , FALSE);
	}
	else
	{
	    blit_iso_image_to_map_position ( ItemMap [ CurItem->type ] . inv_image . ingame_iso_image , 
					     CurItem -> pos . x - 3.0 * sinf ( CurItem -> throw_time * 3.0 ) , 
					     CurItem -> pos . y - 3.0 * sinf ( CurItem -> throw_time * 3.0 ) );
	    if ( highlight_item )
		blit_outline_of_iso_image_to_map_position ( ItemMap [ CurItem->type ] . inv_image . ingame_iso_image , 
							    CurItem -> pos . x - 3.0 * sinf ( CurItem -> throw_time * 3.0 ) , 
							    CurItem -> pos . y - 3.0 * sinf ( CurItem -> throw_time * 3.0 ) );
	}
    }
    
}; // void PutItem( int ItemNumber );

/* ----------------------------------------------------------------------
 * This function draws an item into the combat window.
 * The only given parameter is the number of the item within
 * the AllItems array.
 * ---------------------------------------------------------------------- */
void
PutRadialBlueSparks( float PosX, float PosY , float Radius , int SparkType , int active_direction [ RADIAL_SPELL_DIRECTIONS ] )
{
#define FIXED_NUMBER_OF_SPARK_ANGLES 12
#define FIXED_NUMBER_OF_PROTOTYPES 4
#define NUMBER_OF_SPARK_TYPES 3
    
    SDL_Rect TargetRectangle;
    static SDL_Surface* SparkPrototypeSurface [ NUMBER_OF_SPARK_TYPES ] [ FIXED_NUMBER_OF_PROTOTYPES ] = { { NULL , NULL , NULL , NULL } , { NULL , NULL , NULL , NULL } } ;
    static iso_image PrerotatedSparkSurfaces [ NUMBER_OF_SPARK_TYPES ] [ FIXED_NUMBER_OF_PROTOTYPES ] [ FIXED_NUMBER_OF_SPARK_ANGLES ];
    SDL_Surface* tmp_surf;
    char* fpath;
    int NumberOfPicturesToUse;
    int i , k ;
    float Angle;
    int PrerotationIndex;
    moderately_finepoint Displacement;
    int PictureType;
    char ConstructedFilename[5000];
    int current_active_direction ;

    //--------------------
    // We do some sanity check against too small a radius
    // given as parameter.  This can be loosened later.
    //
    if ( Radius <= 1.0 ) return;
    
    //--------------------
    // We do some phase rotation for now.  Later we might leave this again...
    //
    PictureType = 1;
    
    //--------------------
    // Now if we do not yet have all the prototype images in memory,
    // we need to load them now and for once...
    //
    if ( SparkPrototypeSurface [ SparkType ] [0] == NULL )
    {
	for ( k = 0 ; k < FIXED_NUMBER_OF_PROTOTYPES ; k ++ )
	{
	    //--------------------
	    // First a sanity check against illegal spark types, and 
	    // ILLEGAL in this case means BIGGER THAN THE CONSTANT OF THE
	    // STATIC ARRAY ABOVE!!!  Otherwise no segfault but crazy 
	    // behaviour may follow....
	    //
	    if ( SparkType >= NUMBER_OF_SPARK_TYPES )
	    {
		fprintf( stderr, "\n\nSparkType: %d\n" , SparkType );
		GiveStandardErrorMessage ( __FUNCTION__  , "\
Freedroid encountered a radial wave type that exceeds the CONSTANT for wave types.",
					   PLEASE_INFORM, IS_FATAL );
	    }
	    
	    switch ( SparkType )
	    {
		case 0:
		    sprintf( ConstructedFilename , "blue_sparks_%d.png" , k );
		    break;
		case 1:
		    sprintf( ConstructedFilename , "green_mist_%d.png" , k );
		    break;
		case 2:
		    sprintf( ConstructedFilename , "red_fire_%d.png" , 1 );
		    break;
		default:
		    fprintf( stderr, "\n\nSparkType: %d\n" , SparkType );
		    GiveStandardErrorMessage ( __FUNCTION__  , "\
Freedroid encountered a radial wave type that does not exist in Freedroid.",
					       PLEASE_INFORM, IS_FATAL );
	    }	      
	    
	    fpath = find_file ( ConstructedFilename , GRAPHICS_DIR, FALSE );
	    
	    tmp_surf = our_IMG_load_wrapper( fpath );
	    if ( tmp_surf == NULL )
	    {
		fprintf( stderr, "\n\nfpath: '%s'\n" , fpath );
		GiveStandardErrorMessage ( __FUNCTION__  , "\
Freedroid wanted to load a certain image file into memory, but the SDL\n\
function used for this did not succeed.",
					   PLEASE_INFORM, IS_FATAL );
	    }
	    
	    // SDL_SetColorKey( tmp_surf , 0 , 0 ); 
	    SparkPrototypeSurface [ SparkType ] [ k ] = our_SDL_display_format_wrapperAlpha ( tmp_surf );
	    SDL_FreeSurface( tmp_surf );
	    
	    //--------------------
	    // Now that the loading is successfully done, we can do the
	    // prerotation of the images...using a constant for simplicity...
	    //
	    for ( i = 0 ; i < FIXED_NUMBER_OF_SPARK_ANGLES ; i++ )
	    {
		Angle = +45 - 360.0 * (float)i / (float)FIXED_NUMBER_OF_SPARK_ANGLES ;
		
		tmp_surf = 
		    rotozoomSurface( SparkPrototypeSurface [ SparkType ] [ k ] , Angle , 1.0 , FALSE );
		
		PrerotatedSparkSurfaces [ SparkType ] [ k ] [ i ] . surface = our_SDL_display_format_wrapperAlpha ( tmp_surf );
		
		//--------------------
		// Maybe opengl is in use.  Then we need to prepare some textures too...
		//
		if ( use_open_gl )
		{
		    flip_image_horizontally ( PrerotatedSparkSurfaces [ SparkType ] [ k ] [ i ] . surface ) ;
		    make_texture_out_of_surface ( & ( PrerotatedSparkSurfaces [ SparkType ] [ k ] [ i ] ) ) ;
		}
		
		SDL_FreeSurface ( tmp_surf );
	    }
	}
	
    }
    
    NumberOfPicturesToUse = 2 * ( 2 * Radius * 64 * 3.14 ) / (float) SparkPrototypeSurface[ SparkType ] [ PictureType ] -> w;
    NumberOfPicturesToUse += 3 ; // we want some overlap
    
    //--------------------
    // Now we blit all the pictures we like to use...in this case using
    // multiple dynamic rotations (oh god!)...
    //
    for ( i = 0 ; i < NumberOfPicturesToUse ; i++ )
    {
	Angle = 360.0 * (float)i / (float)NumberOfPicturesToUse ;
	
	Displacement . x = 0 ; Displacement . y = - Radius ;
	
	RotateVectorByAngle ( &Displacement , Angle );

	PrerotationIndex = rintf ( ( Angle  ) * (float)FIXED_NUMBER_OF_SPARK_ANGLES / 360.0 ); 
	if ( PrerotationIndex >= FIXED_NUMBER_OF_SPARK_ANGLES ) PrerotationIndex = 0 ;

	current_active_direction = rintf ( ( Angle  ) * (float) RADIAL_SPELL_DIRECTIONS / 360.0 ); 
	if ( ! active_direction [ current_active_direction ] ) continue ;
	
	if ( use_open_gl )
	{
	    TargetRectangle . x = translate_map_point_to_screen_pixel ( PosX + Displacement . x , PosY + Displacement . y , TRUE ) - ( ( PrerotatedSparkSurfaces [ SparkType ] [ PictureType ] [ PrerotationIndex ] . original_image_width ) / 2 );
	    TargetRectangle . y = translate_map_point_to_screen_pixel ( PosX + Displacement . x , PosY + Displacement . y , FALSE ) - ( ( PrerotatedSparkSurfaces [ SparkType ] [ PictureType ] [ PrerotationIndex ] . original_image_height ) / 2 );
	}
	else
	{
	    TargetRectangle . x = translate_map_point_to_screen_pixel ( PosX + Displacement . x , PosY + Displacement . y , TRUE ) - ( ( PrerotatedSparkSurfaces [ SparkType ] [ PictureType ] [ PrerotationIndex ] . surface -> w ) / 2 );
	    TargetRectangle . y = translate_map_point_to_screen_pixel ( PosX + Displacement . x , PosY + Displacement . y , FALSE ) - ( ( PrerotatedSparkSurfaces [ SparkType ] [ PictureType ] [ PrerotationIndex ] . surface -> h ) / 2 );
	}
	
	if ( use_open_gl )
	{
	    blit_open_gl_texture_to_screen_position ( PrerotatedSparkSurfaces [ SparkType ] [ PictureType ] [ PrerotationIndex ] , 
						      TargetRectangle . x , 
						      TargetRectangle . y , TRUE ) ;
	}
	else
	{
	    our_SDL_blit_surface_wrapper( PrerotatedSparkSurfaces [ SparkType ] [ PictureType ] [ PrerotationIndex ] . surface , NULL , Screen , &TargetRectangle);
	}
	
    }
    
}; // void PutRadialBlueSparks( float PosX, float PosY , float Radius )

/* ----------------------------------------------------------------------
 * This function draws an item into the combat window.
 * The only given parameter is the number of the item within
 * the AllItems array.
 * ---------------------------------------------------------------------- */
void
PutRadialBlueSparksBestQuality( float PosX, float PosY , float Radius )
{
    SDL_Rect TargetRectangle;
    static SDL_Surface* SparkPrototypeSurface=NULL;
    SDL_Surface* tmp_surf;
    char* fpath;
    int NumberOfPicturesToUse;
    int i;
    float Angle;
    moderately_finepoint Displacement;
    
    //--------------------
    // We do some sanity check against too small a radius
    // given as parameter.  This can be loosened later.
    //
    if ( Radius <= 1.0 ) return;
    
    //--------------------
    // Now if we do not yet have all the prototype images in memory,
    // we need to load them now and for once...
    //
    if ( SparkPrototypeSurface == NULL )
    {
	fpath = find_file ( "blue_sparks_0.png" , GRAPHICS_DIR, FALSE );
	
	tmp_surf = our_IMG_load_wrapper( fpath );
	if ( tmp_surf == NULL )
	{
	    fprintf( stderr, "\n\nfpath: '%s'\n" , fpath );
	    GiveStandardErrorMessage ( __FUNCTION__  , "\
Freedroid wanted to load a certain image file into memory, but the SDL\n\
function used for this did not succeed.",
				       PLEASE_INFORM, IS_FATAL );
	}
	// SDL_SetColorKey( tmp_surf , 0 , 0 ); 
	
	SparkPrototypeSurface = our_SDL_display_format_wrapperAlpha ( tmp_surf );
	
	SDL_FreeSurface( tmp_surf );
    }
    
    NumberOfPicturesToUse = ( 2 * Radius * 64 * 3.14 ) / (float) SparkPrototypeSurface -> w;
    NumberOfPicturesToUse += 3 ; // we want some overlap
    
    //--------------------
    // Now we blit all the pictures we like to use...in this case using
    // multiple dynamic rotations (oh god!)...
    //
    for ( i = 0 ; i < NumberOfPicturesToUse ; i++ )
    {
	Angle = 360.0 * (float)i / (float)NumberOfPicturesToUse ;
	
	Displacement . x = 0 ; Displacement . y = - Radius * 64 ;
	
	RotateVectorByAngle ( &Displacement , Angle );
	
	tmp_surf = 
	    rotozoomSurface( SparkPrototypeSurface , Angle , 1.0 , FALSE );
	
	TargetRectangle . x = UserCenter_x - ( Me [ 0 ] . pos . x - PosX ) * 64 + Displacement . x - ( (tmp_surf -> w) / 2 );
	TargetRectangle . y = UserCenter_y - ( Me [ 0 ] . pos . y - PosY ) * 64 + Displacement . y - ( (tmp_surf -> h) / 2 );
	
	our_SDL_blit_surface_wrapper( tmp_surf , NULL , Screen , &TargetRectangle);
	
	SDL_FreeSurface ( tmp_surf );
    }
    
    // DebugPrintf ( 0 , "\nSparks drawn!! " );
    
}; // void PutRadialBlueSparksBestQuality( float PosX, float PosY , float Radius )

/* ----------------------------------------------------------------------
 * This function draws a blast into the combat window.
 * The only given parameter is the number of the blast within
 * the AllBlasts array.
 * ---------------------------------------------------------------------- */
void
PutBlast (int Blast_number)
{
    Blast CurBlast = &AllBlasts[Blast_number];
    
    // If the blast is already long dead, we need not do anything else here
    if ( CurBlast -> type == INFOUT )
	return;

    int phase = (int)floorf(CurBlast->phase);
    if(phase >= 20)
        {
	DeleteBlast(Blast_number);
        return;
        }
    
    // DebugPrintf( 0 , "\nBulletType before calculating phase : %d." , CurBullet->type );
    if ( CurBlast->type >= ALLBLASTTYPES ) 
    {
	GiveStandardErrorMessage ( __FUNCTION__  , "\
The PutBlast function should blit a blast of a type that does not\n\
exist at all.",
				   PLEASE_INFORM, IS_FATAL );
    };
    
    blit_iso_image_to_map_position ( Blastmap [ CurBlast -> type ] . image [ phase ] , 
				     CurBlast -> pos . x , CurBlast -> pos . y  );
    
};  // void PutBlast(int Blast_number)

/* ----------------------------------------------------------------------
 * This function fills the combat window with one single color, given as
 * the only parameter to the function.
 * ---------------------------------------------------------------------- */
void
FdFlashWindow (SDL_Color Flashcolor)
{
    FdFillRect( User_Rect, Flashcolor);
}; // void FlashWindow(int Flashcolor)


/* -----------------------------------------------------------------
 * Fill given rectangle with given RBG color
 * ----------------------------------------------------------------- */
void
FdFillRect (SDL_Rect rect, SDL_Color color)
{
    Uint32 pixcolor;
    SDL_Rect tmp;
    
    Set_Rect (tmp, rect.x, rect.y, rect.w, rect.h);
    
    pixcolor = SDL_MapRGB (Screen->format, color.r, color.g, color.b);
    
    our_SDL_fill_rect_wrapper (Screen, &tmp, pixcolor);
    
}; // void FillRect (SDL_Rect rect, SDL_Color color)

/* ----------------------------------------------------------------------
 * This function displays a robot picture.  This does NOT mean a
 * robot picture like in combat but this means a finely renderd
 * artwork by bastian, that is displayed in the console if info
 * about a robot is requested.  The only parameters to this 
 * function are the position on the screen where to blit the 
 * picture and the number of the robot in the Druidmap *NOT*
 * in AllEnemys!!
 *
 * ---------------------------------------------------------------------- */
void
ShowRobotPicture (int PosX, int PosY, int Number )
{
    SDL_Surface *tmp;
    SDL_Rect target;
    char *fpath;
    char fname[500];
    
    DebugPrintf ( 2 , "\n%s(): Function call confirmed." , __FUNCTION__ );
    
    // strcpy( fname, Druidmap[Number].druidname );
    strcpy( fname, "droids/" );
    strcat( fname, Druidmap[Number].portrait_filename_without_ext );
    strcat( fname , "/portrait.png" );
    DebugPrintf ( 2 , "\ntrying to load this: %s" , fname );
    fpath = find_file (fname, GRAPHICS_DIR, FALSE);
    
    if ( (tmp=our_IMG_load_wrapper (fpath)) == NULL )
    {
	fprintf( stderr, "\n\nfpath '%s' SDL_GetError(): %s. \n" , fpath, SDL_GetError() );
	GiveStandardErrorMessage ( __FUNCTION__  , "\
A droid portrait failed to load.",
				   PLEASE_INFORM, IS_FATAL );
    }
    
    SDL_SetClipRect( Screen , NULL );
    Set_Rect (target, PosX, PosY, GameConfig . screen_width, GameConfig . screen_height);
    our_SDL_blit_surface_wrapper( tmp , NULL, Screen , &target);
    
    SDL_FreeSurface(tmp);
    
    DebugPrintf ( 2 , "\n%s(): Usual end of function reached." , __FUNCTION__ );

}; // void ShowRobotPicture ( ... )

/* ----------------------------------------------------------------------
 * When the inventory screen is visible, we do not only show the items
 * present in inventory, but we also show the inventory squares, that each
 * item in the item pool takes away for storage.  This function blits a
 * part-transparent colored shadow under the item, such that the inventory
 * dimensions become apparent to the player immediately.
 * ---------------------------------------------------------------------- */
void
draw_inventory_occupied_rectangle ( SDL_Rect TargetRect , int bgcolor )
{
#define RED_INVENTORY_SQUARE_OCCUPIED_FILE "backgrounds/TransparentRedPlate.png"
#define BLUE_INVENTORY_SQUARE_OCCUPIED_FILE "backgrounds/TransparentBluePlate.png"
#define GREY_INVENTORY_SQUARE_OCCUPIED_FILE "backgrounds/TransparentGreyPlate.png"
#define REQUIREMENTS_NOT_MET 1
#define IS_MAGICAL 2
    
    static SDL_Surface *TransparentRedPlateImage = NULL;
    static SDL_Surface *TransparentBluePlateImage = NULL;
    static SDL_Surface *TransparentGreyPlateImage = NULL;
    SDL_Surface *tmp;
    char *fpath;
    char fname1 [ ] = RED_INVENTORY_SQUARE_OCCUPIED_FILE;
    char fname2 [ ] = BLUE_INVENTORY_SQUARE_OCCUPIED_FILE;
    char fname3 [ ] = GREY_INVENTORY_SQUARE_OCCUPIED_FILE;
    
    if ( use_open_gl )
    {
	if ( ! bgcolor )
	    GL_HighlightRectangle ( Screen , TargetRect , 127 , 127 , 127 , 100 );
	if ( bgcolor & IS_MAGICAL ) 
	    GL_HighlightRectangle ( Screen , TargetRect , 0 , 0 , 255, 100 );
	if ( bgcolor & REQUIREMENTS_NOT_MET )
	    GL_HighlightRectangle ( Screen , TargetRect , 255 , 0 , 0 , 100 );
    }
    else
    {
	// --------------------
	// Some things like the loading of the inventory and initialisation of the
	// inventory rectangle need to be done only once at the first call of this
	// function. 
	//
	if ( TransparentRedPlateImage == NULL )
	{
	    //--------------------
	    // Now we load the red intentory plate
	    //
	    fpath = find_file ( fname1 , GRAPHICS_DIR, FALSE);
	    tmp = our_IMG_load_wrapper( fpath );
	    if ( !tmp )
	    {
		fprintf( stderr, "\n\nfname1: '%s'\n" , fname1 );
		GiveStandardErrorMessage ( __FUNCTION__  , "\
The red transparent plate for the inventory could not be loaded.  This is a fatal error.",
					   PLEASE_INFORM, IS_FATAL );
	    }
	    TransparentRedPlateImage = our_SDL_display_format_wrapperAlpha ( tmp );
	    SDL_FreeSurface ( tmp );
	    
	    //--------------------
	    // Now we load the blue inventory plate
	    //
	    fpath = find_file ( fname2 , GRAPHICS_DIR, FALSE);
	    tmp = our_IMG_load_wrapper( fpath );
	    if ( !tmp )
	    {
		fprintf( stderr, "\n\nfname2: '%s'\n" , fname2 );
		GiveStandardErrorMessage ( __FUNCTION__  , "\
The blue transparent plate for the inventory could not be loaded.  This is a fatal error.",
					   PLEASE_INFORM, IS_FATAL );
	    }
	    TransparentBluePlateImage = our_SDL_display_format_wrapperAlpha ( tmp );
	    SDL_FreeSurface ( tmp );

	    //--------------------
	    // Now we load the grey inventory plate
	    //
	    fpath = find_file ( fname3 , GRAPHICS_DIR, FALSE);
	    tmp = our_IMG_load_wrapper( fpath );
	    if ( !tmp )
	    {
		fprintf( stderr, "\n\nfname3: '%s'\n" , fname3 );
		GiveStandardErrorMessage ( __FUNCTION__  , "\
The grey transparent plate for the inventory could not be loaded.  This is a fatal error.",
					   PLEASE_INFORM, IS_FATAL );
	    }
	    TransparentGreyPlateImage = our_SDL_display_format_wrapperAlpha ( tmp );
	    SDL_FreeSurface ( tmp );
	    
	}
	
	if ( !bgcolor)
	    our_SDL_blit_surface_wrapper( TransparentGreyPlateImage , NULL , Screen , &TargetRect );
	if ( bgcolor & IS_MAGICAL )
	    our_SDL_blit_surface_wrapper( TransparentBluePlateImage , NULL , Screen , &TargetRect );
	if ( bgcolor & REQUIREMENTS_NOT_MET ) 
	    our_SDL_blit_surface_wrapper( TransparentRedPlateImage , NULL , Screen , &TargetRect );
    }
    
    
}; // void draw_inventory_occupied_rectangle ( SDL_Rect TargetRect )

/* ----------------------------------------------------------------------
 * This function displays the inventory screen and also fills in all the
 * items the influencer is carrying in his inventory and also all the 
 * items the influencer is fitted with.
 * ---------------------------------------------------------------------- */
void
ShowInventoryScreen( void )
{
    SDL_Rect TargetRect;
    int SlotNum;
    int i , j ;
    
    //--------------------
    // We define the left side of the user screen as the rectangle
    // for our inventory screen.
    //
    InventoryRect.x = 0;
    InventoryRect.y = User_Rect.y;
    InventoryRect.w = 320;
    InventoryRect.h = 480;

    if ( GameConfig.Inventory_Visible == FALSE ) return;
    
    //--------------------
    // At this point we know, that the inventory screen is desired and must be
    // displayed in-game:
    //
    blit_special_background ( INVENTORY_SCREEN_BACKGROUND_CODE );
    
    //--------------------
    // Now we display the item in the influencer drive slot
    //
    TargetRect.x = InventoryRect.x + DRIVE_RECT_X;
    TargetRect.y = InventoryRect.y + DRIVE_RECT_Y;
    if ( ( ! Me[0].drive_item.currently_held_in_hand ) && ( Me[0].drive_item.type != (-1) ) )
    {
	our_SDL_blit_surface_wrapper( ItemMap [ Me [ 0 ] . drive_item . type ] . inv_image . Surface , NULL , Screen , &TargetRect );
    }
    
    //--------------------
    // Now we display the item in the influencer weapon slot
    // At this point we have to pay extra care, cause the weapons in Freedroid
    // really come in many different sizes.
    //
    TargetRect.x = InventoryRect.x + WEAPON_RECT_X;
    TargetRect.y = InventoryRect.y + WEAPON_RECT_Y;
    if ( ( ! Me [ 0 ] . weapon_item.currently_held_in_hand ) && ( Me [ 0 ] . weapon_item.type != (-1) ) )
    {
	TargetRect.x += INV_SUBSQUARE_WIDTH * 0.5 * ( 2 - ItemMap [ Me [ 0 ] . weapon_item . type ] . inv_image . inv_size . x ) ;
	TargetRect.y += INV_SUBSQUARE_HEIGHT * 0.5 * ( 3 - ItemMap [ Me [ 0 ] . weapon_item . type ] . inv_image . inv_size . y ) ;
	our_SDL_blit_surface_wrapper( ItemMap [ Me [ 0 ] . weapon_item . type ] . inv_image . Surface , NULL , Screen , &TargetRect );
	
	//--------------------
	// Maybe this is also a 2-handed weapon.  In this case we need to blit the
	// weapon a second time, this time in the center of the shield rectangle to
	// visibly reflect the fact, that the shield hand is required too for this
	// weapon.
	//
	if ( ItemMap [ Me [ 0 ] . weapon_item . type ] . item_gun_requires_both_hands )
	{
	    TargetRect.x = InventoryRect.x + SHIELD_RECT_X;
	    TargetRect.y = InventoryRect.y + SHIELD_RECT_Y;
	    TargetRect.x += INV_SUBSQUARE_WIDTH * 0.5 * ( 2 - ItemMap [ Me [ 0 ] . weapon_item . type ] . inv_image . inv_size . x ) ;
	    TargetRect.y += INV_SUBSQUARE_HEIGHT * 0.5 * ( 3 - ItemMap [ Me [ 0 ] . weapon_item . type ] . inv_image . inv_size . y ) ;
	    our_SDL_blit_surface_wrapper( ItemMap [ Me [ 0 ] . weapon_item . type ] . inv_image . Surface , NULL , Screen , &TargetRect );
	}
    }
    
    //--------------------
    // Now we display the item in the influencer armour slot
    //
    TargetRect.x = InventoryRect.x + ARMOUR_RECT_X ;
    TargetRect.y = InventoryRect.y + ARMOUR_RECT_Y ;
    if ( ( ! Me[0].armour_item.currently_held_in_hand ) && ( Me[0].armour_item.type != (-1) ) )
    {
	our_SDL_blit_surface_wrapper( ItemMap [ Me [ 0 ] . armour_item . type ] . inv_image . Surface , NULL , Screen , &TargetRect );
    }
    
    //--------------------
    // Now we display the item in the influencer shield slot
    //
    TargetRect.x = InventoryRect.x + SHIELD_RECT_X ;
    TargetRect.y = InventoryRect.y + SHIELD_RECT_Y ;
    if ( ( ! Me [ 0 ] . shield_item . currently_held_in_hand ) && ( Me [ 0 ] . shield_item . type != (-1) ) )
    {
	//--------------------
	// Not all shield have the same height, therefore we do a little safety
	// correction here, so that the shield will always appear in the center
	// of the shield slot
	//
	TargetRect.y += INV_SUBSQUARE_HEIGHT * 0.5 * ( 3 - ItemMap [ Me [ 0 ] . shield_item . type ] . inv_image . inv_size . y ) ;
	our_SDL_blit_surface_wrapper( ItemMap [ Me [ 0 ] . shield_item . type ] . inv_image . Surface , NULL , Screen , &TargetRect );
    }
    
    //--------------------
    // Now we display the item in the influencer special slot
    //
    TargetRect.x = InventoryRect.x + HELMET_RECT_X ;
    TargetRect.y = InventoryRect.y + HELMET_RECT_Y ;
    if ( ( ! Me[0].special_item.currently_held_in_hand ) && ( Me[0].special_item.type != (-1) ) )
    {
	our_SDL_blit_surface_wrapper( ItemMap [ Me [ 0 ] . special_item . type ] . inv_image . Surface , NULL , Screen , &TargetRect );
    }
    
    //--------------------
    // Now we display the item in the influencers aux1 slot
    //
    TargetRect.x = InventoryRect.x + AUX1_RECT_X ;
    TargetRect.y = InventoryRect.y + AUX1_RECT_Y ;
    if ( ( ! Me[0].aux1_item.currently_held_in_hand ) && ( Me[0].aux1_item.type != (-1) ) )
    {
	our_SDL_blit_surface_wrapper( ItemMap [ Me [ 0 ] . aux1_item . type ] . inv_image . Surface , NULL , Screen , &TargetRect );
    }
    
    //--------------------
    // Now we display the item in the influencers aux2 slot
    //
    TargetRect.x = InventoryRect.x + AUX2_RECT_X ;
    TargetRect.y = InventoryRect.y + AUX2_RECT_Y ;
    if ( ( ! Me[0].aux2_item.currently_held_in_hand ) && ( Me[0].aux2_item.type != (-1) ) )
    {
	our_SDL_blit_surface_wrapper( ItemMap [ Me [ 0 ] . aux2_item . type ] . inv_image . Surface , NULL , Screen , &TargetRect );
    }
    
    //--------------------
    // Now we display all the items the influencer is carrying with him
    //
    for ( SlotNum = 0 ; SlotNum < MAX_ITEMS_IN_INVENTORY -1 ; SlotNum ++ )
    {
	// In case the item does not exist at all, we need not do anything more...
	if ( Me[0].Inventory[ SlotNum ].type == ( -1 ) ) 
	{
	    // DisplayText( "\n--- Slot empty ---" , -1 , -1 , &InventoryRect );
	    continue;
	}
	
	// In case the item is currently held in hand, we need not do anything more HERE ...
	if ( Me[0].Inventory[ SlotNum ].currently_held_in_hand == TRUE )
	{
	    continue;
	}
	
	for ( i = 0 ; i < ItemMap [ Me [ 0 ] . Inventory [ SlotNum ] . type ] . inv_image . inv_size . y ; i++ )
	{
	    for ( j = 0 ; j < ItemMap [ Me [ 0 ] . Inventory [ SlotNum ] . type ] . inv_image . inv_size . x ; j++ )
	    {
		TargetRect.x = INVENTORY_RECT_X - 1 + INV_SUBSQUARE_WIDTH * ( Me[0].Inventory[ SlotNum ].inventory_position.x + j );
		TargetRect.y = User_Rect.y + INVENTORY_RECT_Y + INV_SUBSQUARE_HEIGHT * ( Me[0].Inventory[ SlotNum ].inventory_position.y + i );
		TargetRect.w = INV_SUBSQUARE_WIDTH ;
		TargetRect.h = INV_SUBSQUARE_HEIGHT ;
		if ( ItemUsageRequirementsMet ( & ( Me [ 0 ] . Inventory [ SlotNum ] ) , FALSE ) )
		    draw_inventory_occupied_rectangle ( TargetRect , 0 | ( Me [ 0 ] . Inventory [ SlotNum ] . prefix_code == -1 ? 0 : 2) |  ( Me [ 0 ] . Inventory [ SlotNum ] . suffix_code == -1 ? 0 : 2) );
		else
		    draw_inventory_occupied_rectangle ( TargetRect , 1 );
	    }
	}
	
	TargetRect.x = INVENTORY_RECT_X - 1 + INV_SUBSQUARE_WIDTH * Me[0].Inventory[ SlotNum ].inventory_position.x;
	TargetRect.y = User_Rect.y + INVENTORY_RECT_Y + INV_SUBSQUARE_HEIGHT * Me[0].Inventory[ SlotNum ].inventory_position.y;
	
	our_SDL_blit_surface_wrapper( ItemMap [ Me [ 0 ] . Inventory [ SlotNum ] . type ] . inv_image . Surface , NULL , Screen , &TargetRect );
	
    }

  if ( Item_Held_In_Hand != (-1) )
    {
        DisplayItemImageAtMouseCursor( Item_Held_In_Hand );
    }
  else
    {
        // In case the player does not have anything in his hand, then of course we need to
        // unset everything as 'not in his hand'.
        //
        // printf("\n Mouse button should cause no image now."); 
    }

}; // void ShowInventoryScreen( void )


#undef _view_c
