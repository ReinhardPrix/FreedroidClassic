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

#define NOT_LOADED_MARKER "nothing_loaded"
enum
  {
    PART_GROUP_HEAD = 0 ,
    PART_GROUP_SHIELD = 1 , 
    PART_GROUP_TORSO = 2 ,
    PART_GROUP_FEET = 3 ,
    PART_GROUP_WEAPON = 4 ,
    PART_GROUP_WEAPONARM = 5 ,
    ALL_PART_GROUPS = 6
  };

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

#define ALL_TUX_PARTS 12
#define ALL_TUX_MOTION_CLASSES 2
iso_image loaded_tux_images [ ALL_TUX_PARTS ] [ TUX_TOTAL_PHASES ] [ MAX_TUX_DIRECTIONS ] ;

int use_walk_cycle_for_part [ ALL_PART_GROUPS ] [ ALL_TUX_MOTION_CLASSES ] = 
  { 
    { 1 , 0 } , // head
    { 1 , 0 } , // shield
    { 1 , 1 } , // torso
    { 1 , 1 } , // feet
    { 1 , 0 } , // sword
    { 1 , 0 } , // weaponarm
  } ;

char previously_used_part_strings [ ALL_PART_GROUPS ] [ 2000 ] ;

char* motion_class_string [ ALL_TUX_MOTION_CLASSES ] = { "sword_motion" , "gun_motion" } ;
int previously_used_motion_class = -4 ; // something we'll never really use...


void FlashWindow (SDL_Color Flashcolor);
void RecFlashFill (int LX, int LY, int Color, unsigned char *Parameter_Screen,
		   int SBreite);
int Cent (int);
void PutRadialBlueSparks( float PosX, float PosY , float Radius , int SparkType );
void insert_new_element_into_blitting_list ( float new_element_norm , int new_element_type , void* new_element_pointer , int code_number );

char *Affected;
EXTERN int MyCursorX;
EXTERN int MyCursorY;

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

blitting_list_element blitting_list [ MAX_ELEMENTS_IN_BLITTING_LIST ] ;
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

//
// POSSIBLY OUTDATED AND UNUSED FUNCTION
// PLEASE CHECK FOR REMOVAL POSSIBLE
//
int
Cent (int Val)
{
  Val = Val - (Val % Block_Width) + Block_Width / 2;
  return Val;
}

/* ----------------------------------------------------------------------
 * This function should display the automap data, that was collected so
 * far, by the tux.
 * ---------------------------------------------------------------------- */
void
ShowAutomapData( void )
{
  int x , y ;
#define AUTOMAP_SQUARE_SIZE 3
#define AUTOMAP_COLOR 0x0FFFF
  int i;
  int TuxColor = SDL_MapRGB( Screen->format, 0 , 0 , 255 ); 
  int FriendColor = SDL_MapRGB( Screen->format, 0 , 255 , 0 ); 
  int BoogyColor = SDL_MapRGB( Screen->format, 255 , 0 , 0 ); 
  int ItemColor = SDL_MapRGB( Screen->format, 255 , 255 , 0 ); 
  Level automap_level = curShip . AllLevels [ Me [ 0 ] . pos . z ] ;
  int level = Me [ 0 ] . pos . z ;

  //--------------------
  // Of course we only display the automap on demand of the user...
  //
  if ( GameConfig.Automap_Visible == FALSE ) return;

  //--------------------
  // Also if there is no map-maker present in inventory, then we need not
  // do a thing here...
  //
  if ( ! CountItemtypeInInventory( ITEM_MAP_MAKER_SIMPLE , 0 ) ) return;

  //--------------------
  // At first, we only blit the known data about the pure wall-type
  // obstacles on this level
  //
  for ( y = 0 ; y < automap_level->ylen ; y ++ )
    {
      for ( x = 0 ; x < automap_level->xlen ; x ++ )
	{
	  if ( Me [ 0 ] . Automap [ level ] [ y ] [ x ] & RIGHT_WALL_BIT )
	    {
	      PutPixel ( Screen , 
			 2+AUTOMAP_SQUARE_SIZE * x + AUTOMAP_SQUARE_SIZE * ( automap_level -> ylen - y ) , 
			 0+AUTOMAP_SQUARE_SIZE * x + AUTOMAP_SQUARE_SIZE * y , AUTOMAP_COLOR );
	      PutPixel ( Screen , 
			 2+AUTOMAP_SQUARE_SIZE * x + AUTOMAP_SQUARE_SIZE * ( automap_level -> ylen - y ) , 
			 1+AUTOMAP_SQUARE_SIZE * x + AUTOMAP_SQUARE_SIZE * y , AUTOMAP_COLOR );
	      PutPixel ( Screen , 
			 2+AUTOMAP_SQUARE_SIZE * x + AUTOMAP_SQUARE_SIZE * ( automap_level -> ylen - y ) , 
			 1+AUTOMAP_SQUARE_SIZE * x + AUTOMAP_SQUARE_SIZE * y , AUTOMAP_COLOR );
	      // putpixel ( Screen , 3*x+2 , 3*y+0 , AUTOMAP_COLOR );
	      // putpixel ( Screen , 3*x+2 , 3*y+1 , AUTOMAP_COLOR );
	      // putpixel ( Screen , 3*x+2 , 3*y+2 , AUTOMAP_COLOR );
	    }
	  if ( Me [ 0 ] . Automap [ level ] [ y ] [ x ] & LEFT_WALL_BIT )
	    {
	      PutPixel ( Screen , 
			 0+AUTOMAP_SQUARE_SIZE * x + AUTOMAP_SQUARE_SIZE * ( automap_level -> ylen - y ) , 
			 0+AUTOMAP_SQUARE_SIZE * x + AUTOMAP_SQUARE_SIZE * y , AUTOMAP_COLOR );
	      PutPixel ( Screen , 
			 0+AUTOMAP_SQUARE_SIZE * x + AUTOMAP_SQUARE_SIZE * ( automap_level -> ylen - y ) , 
			 1+AUTOMAP_SQUARE_SIZE * x + AUTOMAP_SQUARE_SIZE * y , AUTOMAP_COLOR );
	      PutPixel ( Screen , 
			 0+AUTOMAP_SQUARE_SIZE * x + AUTOMAP_SQUARE_SIZE * ( automap_level -> ylen - y ) , 
			 1+AUTOMAP_SQUARE_SIZE * x + AUTOMAP_SQUARE_SIZE * y , AUTOMAP_COLOR );
	      // putpixel ( Screen , 3*x , 3*y+0 , AUTOMAP_COLOR );
	      // putpixel ( Screen , 3*x , 3*y+1 , AUTOMAP_COLOR );
	      // putpixel ( Screen , 3*x , 3*y+2 , AUTOMAP_COLOR );
	    }
	  if ( Me [ 0 ] . Automap [ level ] [ y ] [ x ] & UP_WALL_BIT )
	    {
	      PutPixel ( Screen , 3*x+0 , 3*y , AUTOMAP_COLOR );
	      PutPixel ( Screen , 3*x+1 , 3*y , AUTOMAP_COLOR );
	      PutPixel ( Screen , 3*x+2 , 3*y , AUTOMAP_COLOR );
	    }
	  if ( Me [ 0 ] . Automap [ level ] [ y ] [ x ] & DOWN_WALL_BIT )
	    {
	      PutPixel ( Screen , 3*x+0 , 3*y+2 , AUTOMAP_COLOR );
	      PutPixel ( Screen , 3*x+1 , 3*y+2 , AUTOMAP_COLOR );
	      PutPixel ( Screen , 3*x+2 , 3*y+2 , AUTOMAP_COLOR );
	    }
	}
    }

  //--------------------
  // Now that the pure map data has been drawn, we add yellow dots for 
  // the items, that have been detected on this level.
  //
  for ( i = 0 ; i < MAX_ITEMS_PER_LEVEL ; i ++ )
    {
      //--------------------
      // This would be the most accurate information, but this is not granted
      // without casting a spell and then only a copy is made and only updated
      // once, so the player must really get on with much less information.
      //
      // if ( automap_level -> ItemList [ i ] . type == (-1) ) continue;
      if ( Me [ 0 ] . DetectedItemList [ i ] . x == 0 ) continue;

      for ( x = 0 ; x < AUTOMAP_SQUARE_SIZE ; x ++ )
	{
	  for ( y = 0 ; y < AUTOMAP_SQUARE_SIZE ; y ++ )
	    {
	      PutPixel ( Screen , AUTOMAP_SQUARE_SIZE * automap_level -> ItemList [ i ].pos.x + x , 
			 AUTOMAP_SQUARE_SIZE * automap_level -> ItemList [ i ].pos.y + y , ItemColor );
	    }
	}
    }

  //--------------------
  // Now that the pure map data has been drawn, we add red dots for 
  // the ememys around.
  //
  for ( i = 0 ; i < Number_Of_Droids_On_Ship ; i ++ )
    {
      if ( AllEnemys [ i ] . Status  == OUT ) continue;
      if ( AllEnemys [ i ] . type == (-1) ) continue;
      if ( AllEnemys [ i ] . pos . z != automap_level -> levelnum ) continue;

      for ( x = 0 ; x < AUTOMAP_SQUARE_SIZE ; x ++ )
	{
	  for ( y = 0 ; y < AUTOMAP_SQUARE_SIZE ; y ++ )
	    {
	      if ( AllEnemys [ i ] . is_friendly )
		{
		  PutPixel ( Screen , AUTOMAP_SQUARE_SIZE * AllEnemys[i].pos.x + AUTOMAP_SQUARE_SIZE * ( automap_level -> ylen - AllEnemys[i].pos.y ) + x , 
			     AUTOMAP_SQUARE_SIZE * AllEnemys[i].pos.x + AUTOMAP_SQUARE_SIZE * AllEnemys[i].pos.y + y , FriendColor );
		}
	      else
		{
		  PutPixel ( Screen , AUTOMAP_SQUARE_SIZE * AllEnemys[i].pos.x + AUTOMAP_SQUARE_SIZE * ( automap_level -> ylen - AllEnemys[i].pos.y ) + x , 
			     AUTOMAP_SQUARE_SIZE * AllEnemys[i].pos.x + AUTOMAP_SQUARE_SIZE * AllEnemys[i].pos.y + y , BoogyColor );
		}
	    }
	}
    }

  //--------------------
  // Now that the automap is drawn so far, we add a blue dot for the
  // tux himself and also for colleagues, that are on this level and alive.
  //
  for ( x = 0 ; x < AUTOMAP_SQUARE_SIZE ; x ++ )
    {
      for ( y = 0 ; y < AUTOMAP_SQUARE_SIZE ; y ++ )
	{
	  PutPixel ( Screen , AUTOMAP_SQUARE_SIZE * Me [ 0 ] . pos . x + AUTOMAP_SQUARE_SIZE * ( automap_level -> ylen - Me [ 0 ] . pos . y ) + x , 
		     AUTOMAP_SQUARE_SIZE * Me [ 0 ] . pos . x + AUTOMAP_SQUARE_SIZE * Me [ 0 ] . pos . y + y , TuxColor );

	  
	  for ( i = 1 ; i < MAX_PLAYERS ; i ++ )
	    {
	      //--------------------
	      // We don't blit other players, that are either dead or not
	      // on this level...
	      //
	      if ( Me [ i ] . pos . z != Me [ 0 ] . pos . z ) continue;
	      if ( Me [ i ] . status == OUT  ) continue;

	      PutPixel ( Screen , AUTOMAP_SQUARE_SIZE * Me [ i ] . pos . x + x , AUTOMAP_SQUARE_SIZE * Me [ i ] . pos . y + y , FriendColor );
	    }
	}
    }

}; // void ShowAutomapData( void )

/* ----------------------------------------------------------------------
 * This function should display the currently assigned/unassigned mission
 * and all that directly over the combat screen without interrupting the
 * game in any other way.
 * ---------------------------------------------------------------------- */
void 
ShowMissionCompletitionMessages( void )
{
  int MissNum;

  //--------------------
  // If the log is not set to visible right now, we do not need to 
  // do anything more
  //
  if ( GameConfig.Mission_Log_Visible == FALSE ) return;
  if ( GameConfig.Mission_Log_Visible_Time >= GameConfig.Mission_Log_Visible_Max_Time ) return;

  //--------------------
  // At this point we know, that the quest log is desired and
  // therefore we display it in-game:
  //
  SDL_SetClipRect( Screen , NULL );
  DisplayText( "See quest log: \n" , User_Rect.x , User_Rect.y , &User_Rect );

  for ( MissNum = 0 ; MissNum < MAX_MISSIONS_IN_GAME; MissNum ++ )
    {
      // In case the mission does not exist at all, we need not do anything more...
      if ( Me[0].AllMissions[ MissNum ].MissionExistsAtAll != TRUE ) continue;

      // In case the mission was not yet assigned, we need not do anything more...
      // if ( Me[0].AllMissions[ MissNum ].MissionWasAssigned != TRUE ) continue;

      // In case the message is rather old, we need not do anything more...
      // if ( Me[0].AllMissions[ MissNum ].MissionLastStatusChangeTime > 1000 ) continue;

      // At this point we know, that the mission has recently been completed or failed

      if ( Me[0].AllMissions[ MissNum ].MissionIsComplete == TRUE )
	{
	  DisplayText( "\n* Mission completed: " , -1 , -1 , &User_Rect );
	}
      else if ( Me[0].AllMissions[ MissNum ].MissionWasFailed == TRUE )
	{
	  DisplayText( "\n* Mission failed: " , -1 , -1 , &User_Rect );
	}
      else if ( ! Me[0].AllMissions[ MissNum ].MissionWasAssigned == TRUE )
	{
	  DisplayText( "\n* Mission not yet assigned: " , -1 , -1 , &User_Rect );
	}
      else 
	DisplayText( "\n* Mission assigned: " , -1 , -1 , &User_Rect );

      DisplayText( Me[0].AllMissions[ MissNum ].MissionName , -1 , -1 , &User_Rect );

    }
};

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
  TargetRect.x = GetMousePos_x() + 16 - ItemImageList[ ItemImageCode ].inv_size.x * 16;
  TargetRect.y = GetMousePos_y() + 16 - ItemImageList[ ItemImageCode ].inv_size.y * 16;

  our_SDL_blit_surface_wrapper( ItemImageList[ ItemImageCode ].Surface , NULL , Screen , &TargetRect );
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

  ItemImageCode = ItemMap [ AlarmItem->type ].picture_number ;

  TargetRect.x = 60 * Position ;
  TargetRect.y = 400;

  if ( AlarmItem->current_duration < 5 )
    {
      our_SDL_blit_surface_wrapper( ItemImageList[ ItemImageCode ].Surface , NULL , Screen , &TargetRect );
    }
}; // void ShowOneItemAlarm( item* AlarmItem )

/* ----------------------------------------------------------------------
 * This function displays (several) blinking warning signs as soon as item
 * durations reach critical (<5) duration level.
 * ---------------------------------------------------------------------- */
void
ShowItemAlarm( void )
{

  if ( ( ( int ) ( Me[0].MissionTimeElapsed * 2 ) ) % 2 == 1 ) return;

  ShowOneItemAlarm( & Me[0].weapon_item , 1 );
  ShowOneItemAlarm( & Me[0].drive_item , 2 );
  ShowOneItemAlarm( & Me[0].shield_item , 3 );
  ShowOneItemAlarm( & Me[0].armour_item , 4 );

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
			       AllActiveSpells [ i ] . spell_radius , 0 );
	}
      else if ( AllActiveSpells [ i ] . type == SPELL_RADIAL_VMX_WAVE ) 
	{
	  PutRadialBlueSparks( AllActiveSpells [ i ] . spell_center . x , 
			       AllActiveSpells [ i ] . spell_center . y , 
			       AllActiveSpells [ i ] . spell_radius , 1 );
	}
      else if ( AllActiveSpells [ i ] . type == SPELL_RADIAL_FIRE_WAVE ) 
	{
	  PutRadialBlueSparks( AllActiveSpells [ i ] . spell_center . x , 
			       AllActiveSpells [ i ] . spell_center . y , 
			       AllActiveSpells [ i ] . spell_radius , 2 );
	}
      else
	{
	  fprintf( stderr, "\n\nAllActiveSpells [ i ] . type: '%d'\n" , AllActiveSpells [ i ] . type );
	  GiveStandardErrorMessage ( "PutMiscellaneousSpellEffects(...)" , "\
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
      
      PrintStringFont( Screen , FPS_Display_BFont , User_Rect.x , 
		       User_Rect.y+User_Rect.h - FontHeight( FPS_Display_BFont ), 
		       "FPS: %d " , FPS_Displayed );

      // PrintStringFont( Screen , FPS_Display_BFont , User_Rect.x + 100, 
      // User_Rect.y+User_Rect.h - FontHeight( FPS_Display_BFont ), 
      // "Axis: %d %d" , input_axis.x, input_axis.y);
    }

  if ( GameConfig.Draw_Energy )
    {
      PrintStringFont( Screen , FPS_Display_BFont , User_Rect.x+User_Rect.w/2 , 
		       User_Rect.y+User_Rect.h - FontHeight( FPS_Display_BFont ), 
		       "Energy: %d " , (int) (Me[0].energy) );
      // PrintStringFont( Screen , FPS_Display_BFont , User_Rect.x+User_Rect.w/2 , 
      // User_Rect.y+User_Rect.h - 2 * FontHeight( FPS_Display_BFont ), 
      // "Resistance: %f " , (Me[0].Current_Victim_Resistance_Factor) );
    }

  if ( GameConfig.Draw_Position || ( mask & ONLY_SHOW_MAP_AND_TEXT ) )
    {
      PrintStringFont( Screen , FPS_Display_BFont , User_Rect.x+1.0*User_Rect.w/3 , 
		       User_Rect.y+User_Rect.h - FontHeight( FPS_Display_BFont ), 
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

	  for ( k = 0 ; k < Number_Of_Droids_On_Ship ; k ++ )
	    {
	      if ( ( AllEnemys [ k ] . pos . z == Me [ 0 ] . pos . z ) &&
		   ( AllEnemys [ k ] . Status != OUT ) &&
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

  ShowMissionCompletitionMessages();

}; // void ShowCombatScreenTexts ( int mask )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
blit_this_floor_tile_to_screen ( iso_image our_floor_iso_image ,
				 float our_col, float our_line )
{
  if ( use_open_gl )
    {
      blit_open_gl_texture_to_map_position ( our_floor_iso_image , our_col , our_line , 1.0 , 1.0 , 1.0 ) ;
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
      LineStart = Me [ 0 ] . pos . y - FLOOR_TILES_VISIBLE_AROUND_TUX * FIXED_ZOOM_OUT_FACT ;
      LineEnd = Me [ 0 ] . pos . y + FLOOR_TILES_VISIBLE_AROUND_TUX * FIXED_ZOOM_OUT_FACT ;
      ColStart = Me [ 0 ] . pos . x - FLOOR_TILES_VISIBLE_AROUND_TUX * FIXED_ZOOM_OUT_FACT ;
      ColEnd = Me [ 0 ] . pos . x + FLOOR_TILES_VISIBLE_AROUND_TUX * FIXED_ZOOM_OUT_FACT ;
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
	      if ((MapBrick = GetMapBrick( DisplayLevel, col , line )) != INVISIBLE_BRICK)
		{
		  blit_zoomed_iso_image_to_map_position ( & ( floor_iso_images [ MapBrick % ALL_ISOMETRIC_FLOOR_TILES ] ) , 
							  ((float)col)+0.5 , ((float)line) +0.5 );
		  
		}	// if !INVISIBLE_BRICK 
	    }		// for(col) 
	}		// for(line) 
    }
  else
    {
      for (line = LineStart; line < LineEnd; line++)
	{
	  for (col = ColStart; col < ColEnd; col++)
	    {
	      if ((MapBrick = GetMapBrick( DisplayLevel, col , line )) != INVISIBLE_BRICK)
		{
		  blit_this_floor_tile_to_screen ( floor_iso_images [ MapBrick % ALL_ISOMETRIC_FLOOR_TILES ] , 
						   ((float)col)+0.5 , ((float)line) +0.5 );
		}	// if !INVISIBLE_BRICK 
	    }		// for(col) 
	}		// for(line) 
    }
}; // void isometric_show_floor_around_tux_without_doublebuffering ( int mask )

/* ----------------------------------------------------------------------
 * This function should blit an obstacle, that is given via it's address
 * in the parameter
 * ---------------------------------------------------------------------- */
void
blit_one_obstacle ( obstacle* our_obstacle )
{
  iso_image tmp;
  float darkness ;
  // DebugPrintf ( 0 , "\nObstacle to be blitted: type=%d x=%f y=%f." , our_obstacle -> type ,
  // our_obstacle -> pos . x , our_obstacle -> pos . y );

  if ( ( our_obstacle-> type <= (-1) ) || ( our_obstacle-> type >= NUMBER_OF_OBSTACLE_TYPES ) )
    {
      GiveStandardErrorMessage ( "blit_one_obstacle(...)" , "\
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

  //--------------------
  // We blit the obstacle in question, but if we're in the level editor and this
  // obstacle has been marked, we apply a color filter to it.  Otherwise we blit
  // it just so.
  //
  if ( our_obstacle == level_editor_marked_obstacle )
    {
      if ( use_open_gl )
	{
	  blit_open_gl_texture_to_map_position ( obstacle_map [ our_obstacle -> type ] . image , 
						 our_obstacle -> pos . x , our_obstacle -> pos . y , 1.0 , 0, ( SDL_GetTicks() % 2 ) * 1.0 ) ;
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
	  blit_iso_image_to_map_position ( tmp , our_obstacle -> pos . x , our_obstacle -> pos . y );
	  SDL_FreeSurface ( tmp . surface );
	}
    }
  else
    {
      if ( use_open_gl )
	{
	  darkness = 2.0 - 2.0 * ( ( (float) get_light_strength ( our_obstacle -> pos ) ) / ( (float) NUMBER_OF_SHADOW_IMAGES ) ) ;
	  if ( darkness > 1 ) darkness = 1.0 ;
	  if ( darkness < 0 ) darkness = 0 ;
	  blit_open_gl_texture_to_map_position ( obstacle_map [ our_obstacle -> type ] . image , 
						 our_obstacle -> pos . x , our_obstacle -> pos . y , darkness , darkness, darkness ) ;
	}
      else
	{
	  blit_iso_image_to_map_position ( obstacle_map [ our_obstacle -> type ] . image , 
					   our_obstacle -> pos . x , our_obstacle -> pos . y );
	}
    }
}; // blit_one_obstacle ( obstacle* our_obstacle )

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
      GiveStandardErrorMessage ( "blit_one_obstacle(...)" , "\
There was an obstacle type given, that exceeds the number of\n\
 obstacle types allowed and loaded in Freedroid.",
				 PLEASE_INFORM, IS_FATAL );
    }

  make_sure_zoomed_surface_is_there ( & ( obstacle_map [ our_obstacle -> type ] . image ) );

  //--------------------
  // We blit the obstacle in question, but if we're in the level editor and this
  // obstacle has been marked, we apply a color filter to it.  Otherwise we blit
  // it just so.
  //
  if ( our_obstacle == level_editor_marked_obstacle )
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
  else
    blit_zoomed_iso_image_to_map_position ( & ( obstacle_map [ our_obstacle -> type ] . image ) , 
					    our_obstacle -> pos . x , our_obstacle -> pos . y );
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
      LineStart = Me [ 0 ] . pos . y - FLOOR_TILES_VISIBLE_AROUND_TUX * FIXED_ZOOM_OUT_FACT ;
      LineEnd = Me [ 0 ] . pos . y + FLOOR_TILES_VISIBLE_AROUND_TUX * FIXED_ZOOM_OUT_FACT ;
      ColStart = Me [ 0 ] . pos . x - FLOOR_TILES_VISIBLE_AROUND_TUX * FIXED_ZOOM_OUT_FACT ;
      ColEnd = Me [ 0 ] . pos . x + FLOOR_TILES_VISIBLE_AROUND_TUX * FIXED_ZOOM_OUT_FACT ;
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
		      GiveStandardErrorMessage ( "insert_obstacles_into_blitting_list (...)" , "\
The blitting list size was exceeded!",
						 PLEASE_INFORM, IS_FATAL );
		    }

		  //--------------------
		  // Now we have to insert this obstacle.  We do this of course respecting
		  // the blitting order, as always...
		  //
		  OurObstacle = & ( obstacle_level -> obstacle_list [ obstacle_level -> map [ line ] [ col ] . obstacles_glued_to_here [ i ] ] ) ;
		  insert_new_element_into_blitting_list ( OurObstacle -> pos . x + OurObstacle -> pos . y , 
							  BLITTING_TYPE_OBSTACLE , OurObstacle , -1 );
		}
	      else
		break;
	    }
	}
    }

}; // void insert_obstacles_into_blitting_list ( void )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
insert_new_element_into_blitting_list ( float new_element_norm , int new_element_type , void* new_element_pointer , int code_number )
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
	      GiveStandardErrorMessage ( "insert_new_element_into_blitting_list (...)" , "\
The blitting list size was exceeded!",
					 PLEASE_INFORM, IS_FATAL );
	    }

	  //--------------------
	  // Note that we MUST NOT USE MEMCPY HERE BUT RATHER MUST USE MEM-MOVE!!
	  // See the GNU C Manual for details!
	  //
	  memmove ( & ( blitting_list [ i + 1 ] ) , & ( blitting_list [ i ] ) , 
		    sizeof ( blitting_list_element ) * ( number_of_objects_currently_in_blitting_list - i ) );

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

  enemy_norm = AllEnemys [ enemy_num ] . pos . x + AllEnemys [ enemy_num ] . pos . y ;

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
 *
 *
 * ---------------------------------------------------------------------- */
void
insert_enemies_into_blitting_list ( void )
{
  int i;
  float enemy_norm;
  float tux_norm = Me [ 0 ] . pos . x + Me [ 0 ] . pos . y ;
  enemy* ThisRobot = & ( AllEnemys [ 0 ] ) ;
  
  ThisRobot -- ;
  for ( i = 0 ; i < Number_Of_Droids_On_Ship ; i ++ )
    {
      ThisRobot ++ ;
      if ( ( ThisRobot -> Status == OUT ) && ( last_death_animation_image [ ThisRobot -> type ] - first_walk_animation_image [ ThisRobot -> type ] == 0 ) ) continue;
      if ( ThisRobot -> pos . z != Me [ 0 ] . pos . z ) continue;
      enemy_norm = ThisRobot -> pos . x + ThisRobot -> pos . y ;
      
      if ( fabsf ( enemy_norm - tux_norm ) > FLOOR_TILES_VISIBLE_AROUND_TUX + FLOOR_TILES_VISIBLE_AROUND_TUX ) continue;

      insert_one_enemy_into_blitting_list ( i );
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
      if (AllBullets[i].type != OUT)
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
      if (AllBlasts[i].type != OUT)
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

  for ( i = 0 ; i < MAX_ELEMENTS_IN_BLITTING_LIST ; i ++ )
    {
      if ( blitting_list [ i ] . element_type == BLITTING_TYPE_NONE ) break;
      if ( blitting_list [ i ] . element_type == BLITTING_TYPE_OBSTACLE )
	{
	  if ( ( (obstacle*)  blitting_list [ i ] . element_pointer ) -> type <= (-1) )
	    {
	      GiveStandardErrorMessage ( "blit_preput_objects_according_to_blitting_list (...)" , 
					 "The blitting list contained an illegal blitting object type.",
					 PLEASE_INFORM, IS_FATAL );
	    }
	  
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
	   ( ( ( enemy* ) blitting_list [ i ] . element_pointer ) -> energy < 0 ) )
	{
	  if ( ! ( mask & OMIT_ENEMIES ) ) 
	    {
	      PutEnemy ( blitting_list [ i ] . code_number , -1 , -1 , mask ); // this blits player 0 
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

  for ( i = 0 ; i < MAX_ELEMENTS_IN_BLITTING_LIST ; i ++ )
    {
      if ( blitting_list [ i ] . element_type == BLITTING_TYPE_NONE ) break;
      switch ( blitting_list [ i ] . element_type )
	{
	case BLITTING_TYPE_OBSTACLE:

	  if ( obstacle_map [ ( (obstacle*)  blitting_list [ i ] . element_pointer ) -> type ] . needs_pre_put ) break ;
	  if ( ! ( mask & OMIT_OBSTACLES ) ) 
	    {
	      if ( mask & ZOOM_OUT )
		blit_one_obstacle_zoomed ( (obstacle*)  blitting_list [ i ] . element_pointer );
	      else
		blit_one_obstacle ( (obstacle*)  blitting_list [ i ] . element_pointer );
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
	      PutEnemy ( blitting_list [ i ] . code_number , -1 , -1 , mask ); // this blits player 0 
	    }
	  break;
	case BLITTING_TYPE_BULLET:
	  PutBullet ( blitting_list [ i ] . code_number , mask ); 
	  break;
	case BLITTING_TYPE_BLAST:
	  if ( ! ( mask & OMIT_BLASTS ) )
	    PutBlast ( blitting_list [ i ] . code_number ); 
	  break;
	case BLITTING_TYPE_THROWN_ITEM:
	  PutItem ( blitting_list [ i ] . code_number , mask , PUT_ONLY_THROWN_ITEMS ); 
	  // DebugPrintf ( -1 , "\nThrown item now blitted..." );
	  break;
	default:
	  GiveStandardErrorMessage ( "blit_all_objects_according_to_blitting_list (...)" , "\
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

  // show_backgrounded_label_at_map_position ( "This is a test" , 0 , Me [ 0 ] . pos . x + 1 , Me [ 0 ] . pos . y + 1 , mask & ZOOM_OUT );

}; // void show_obstacle_labels ( int mask )

/* ----------------------------------------------------------------------
 * This function is used to find the light intensity at any given point
 * on the map.
 * ---------------------------------------------------------------------- */
int 
get_light_strength ( moderately_finepoint target_pos )
{
#define MAX_NUMBER_OF_LIGHT_SOURCES 10
  int final_darkness = NUMBER_OF_SHADOW_IMAGES;
  moderately_finepoint light_sources [ MAX_NUMBER_OF_LIGHT_SOURCES ] ;
  int light_source_strengthes [ MAX_NUMBER_OF_LIGHT_SOURCES ] ;
  int i;
  Level light_level = curShip . AllLevels [ Me [ 0 ] . pos . z ] ;

  //--------------------
  // At first we fill out the light sources array with 'empty' information,
  // i.e. such positions, that won't affect our location for sure.
  //
  for ( i = 0 ; i < MAX_NUMBER_OF_LIGHT_SOURCES ; i ++ )
    {
      light_sources [ i ] . x = -200 ;
      light_sources [ i ] . y = -200 ;
      light_source_strengthes [ i ] = 0 ;
    }

  //--------------------
  // Now we fill in the Tux position as the very first light source, that will
  // always be present.
  //
  light_sources [ 0 ] . x = Me [ 0 ] . pos . x ;
  light_sources [ 0 ] . y = Me [ 0 ] . pos . y ;
  light_source_strengthes [ 0 ] = light_level -> light_radius_bonus ;

  //--------------------
  // Now we can fill in the remaining light sources of this level
  //
  for ( i = 1 ; i < MAX_NUMBER_OF_LIGHT_SOURCES ; i ++ )
    {
      if ( light_level -> teleporter_obstacle_indices [ i-1 ] != (-1) )
	{
	  light_sources [ i ] . x = light_level -> obstacle_list [ light_level -> teleporter_obstacle_indices [ i-1 ] ] . pos . x ;
	  light_sources [ i ] . y = light_level -> obstacle_list [ light_level -> teleporter_obstacle_indices [ i-1 ] ] . pos . y ;
	  light_source_strengthes [ i ] = 10 ;
	}
    }

  //--------------------
  // Now that the light sources array is fully set up, we can start
  // to compute the individual light strength at any given position
  //
  for ( i = 0 ; i < MAX_NUMBER_OF_LIGHT_SOURCES ; i ++ )
    {
      //--------------------
      // We could of course use a maximum function to find out the proper light at
      // any place.  But maybe addition of light would be better, so we use the latter
      // code.
      //
      if ( ( (int) ( sqrt ( ( light_sources [ i ] . x - target_pos . x ) * 
			    ( light_sources [ i ] . x - target_pos . x ) + 
			    ( light_sources [ i ] . y - target_pos . y ) * 
			    ( light_sources [ i ] . y - target_pos . y ) ) * 4.0 ) 
	     - light_source_strengthes [ i ] ) < final_darkness )
	final_darkness = (int) ( sqrt ( ( light_sources [ i ] . x - target_pos . x ) * 
					( light_sources [ i ] . x - target_pos . x ) + 
					( light_sources [ i ] . y - target_pos . y ) * 
					( light_sources [ i ] . y - target_pos . y ) ) * 4.0 ) 
	  - light_source_strengthes [ i ] ;
      
      /*
      if ( ( (int) ( sqrt ( ( light_sources [ i ] . x - target_pos . x ) * 
			    ( light_sources [ i ] . x - target_pos . x ) + 
			    ( light_sources [ i ] . y - target_pos . y ) * 
			    ( light_sources [ i ] . y - target_pos . y ) ) * 4.0 ) 
	     - light_bonus ) < NUMBER_OF_SHADOW_IMAGES )
	final_darkness -= ( NUMBER_OF_SHADOW_IMAGES -  ( (int) ( sqrt ( ( light_sources [ i ] . x - target_pos . x ) * 
									( light_sources [ i ] . x - target_pos . x ) + 
									( light_sources [ i ] . y - target_pos . y ) * 
									( light_sources [ i ] . y - target_pos . y ) ) * 
								 4.0 )
							 - light_bonus ) ) ;
      */
    }

  return ( final_darkness );

}; // int get_light_strength ( moderately_finepoint target_pos )

/* ----------------------------------------------------------------------
 * This function should blit the shadows on the floor, that are used to
 * generate the impression of a 'light radius' around the players 
 * character.
 * ---------------------------------------------------------------------- */
void
blit_classic_SDL_light_radius( void )
{
  static int first_call = TRUE ;
  int i, j ;
  char* fpath;
  char constructed_file_name[2000];
  int our_height, our_width, our_max_height, our_max_width;
  int light_strength;
  moderately_finepoint target_pos;
  static int pos_x_grid [ (int)(FLOOR_TILES_VISIBLE_AROUND_TUX * ( 1.0 / LIGHT_RADIUS_CHUNK_SIZE ) * 2) ] [ (int)(FLOOR_TILES_VISIBLE_AROUND_TUX * ( 1.0 / LIGHT_RADIUS_CHUNK_SIZE ) * 2 ) ] ;
  static int pos_y_grid [ (int)(FLOOR_TILES_VISIBLE_AROUND_TUX * ( 1.0 / LIGHT_RADIUS_CHUNK_SIZE ) * 2) ] [ (int)(FLOOR_TILES_VISIBLE_AROUND_TUX * ( 1.0 / LIGHT_RADIUS_CHUNK_SIZE ) * 2 ) ] ;
  static SDL_Rect target_rectangle;
  int chunk_size_x;
  int chunk_size_y;
  int window_offset_x;
  SDL_Surface* tmp;

  //--------------------
  // If the darkenss chunks have not yet been loaded, we load them...
  //
  if ( first_call )
    {
      first_call = FALSE;
      for ( i = 0 ; i < NUMBER_OF_SHADOW_IMAGES ; i ++ )
	{
	  sprintf ( constructed_file_name , "light_radius_chunks/iso_light_radius_darkness_%04d.png" , i + 1 );
	  fpath = find_file ( constructed_file_name , GRAPHICS_DIR , FALSE );
	  get_iso_image_from_file_and_path ( fpath , & ( light_radius_chunk [ i ] ) ) ;
	  tmp = light_radius_chunk [ i ] . surface ;
	  light_radius_chunk [ i ] . surface = SDL_DisplayFormatAlpha ( light_radius_chunk [ i ] . surface ) ; 
	  SDL_FreeSurface ( tmp ) ;
	}

      pos_x_grid [ 0 ] [ 0 ] = translate_map_point_to_screen_pixel ( Me [ 0 ] . pos . x - ( FLOOR_TILES_VISIBLE_AROUND_TUX ) , Me [ 0 ] . pos . y - ( FLOOR_TILES_VISIBLE_AROUND_TUX ) , TRUE ) - 10 ;
      pos_y_grid [ 0 ] [ 0 ] = translate_map_point_to_screen_pixel ( Me [ 0 ] . pos . x - ( FLOOR_TILES_VISIBLE_AROUND_TUX ) , Me [ 0 ] . pos . y - ( FLOOR_TILES_VISIBLE_AROUND_TUX ) , FALSE ) - 42 ;

      chunk_size_x = 26 /2 + 1 ;
      chunk_size_y = 14 /2 ; 

      for ( i = 0 ; i < (int)(FLOOR_TILES_VISIBLE_AROUND_TUX * ( 1.0 / LIGHT_RADIUS_CHUNK_SIZE ) * 2) ; i ++ )
	{
	  for ( j = 0 ; j < (int)(FLOOR_TILES_VISIBLE_AROUND_TUX * ( 1.0 / LIGHT_RADIUS_CHUNK_SIZE ) * 2) ; j ++ )
	    {
	      pos_x_grid [ i ] [ j ] = pos_x_grid [ 0 ] [ 0 ] + ( i - j ) * chunk_size_x ;
	      pos_y_grid [ i ] [ j ] = pos_y_grid [ 0 ] [ 0 ] + ( i + j ) * chunk_size_y ;
	    }
	}

    }

  //--------------------
  // Now it's time to apply the light radius
  //
  our_max_width = FLOOR_TILES_VISIBLE_AROUND_TUX * ( 1.0 / LIGHT_RADIUS_CHUNK_SIZE ) * 2 ;
  our_max_height = our_max_width;

  window_offset_x = - ( SCREEN_WIDTH / 2 ) + UserCenter_x ;

  for ( our_height = 0 ; our_height < our_max_height ; our_height ++ )
    {
      for ( our_width = 0 ; our_width < our_max_width ; our_width ++ )
	{
	  if ( our_width % LIGHT_RADIUS_CRUDENESS_FACTOR ) continue;
	  if ( our_height % LIGHT_RADIUS_CRUDENESS_FACTOR ) continue;

	  target_pos . x = Me [ 0 ] . pos . x - ( FLOOR_TILES_VISIBLE_AROUND_TUX ) + our_width * LIGHT_RADIUS_CHUNK_SIZE ;
	  target_pos . y = Me [ 0 ] . pos . y - ( FLOOR_TILES_VISIBLE_AROUND_TUX ) + our_height * LIGHT_RADIUS_CHUNK_SIZE;
	  light_strength = get_light_strength ( target_pos ) ;

	  if ( light_strength >= NUMBER_OF_SHADOW_IMAGES ) light_strength = NUMBER_OF_SHADOW_IMAGES -1 ;
	  if ( light_strength <= 0 ) continue ;

	  // blit_iso_image_to_map_position ( light_radius_chunk [ light_strength ] , target_pos . x , target_pos . y );
	  target_rectangle . x = pos_x_grid [ our_width ] [ our_height ] + window_offset_x ;
	  target_rectangle . y = pos_y_grid [ our_width ] [ our_height ] ;

	  our_SDL_blit_surface_wrapper( light_radius_chunk [ light_strength ] . surface , NULL , Screen, &target_rectangle );
	}
    }
}; // void blit_classic_SDL_light_radius( void )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
blit_light_radius ( void )
{

  if ( use_open_gl )
    {
      // blit_open_gl_light_radius ();
      blit_open_gl_cheap_light_radius ();
    }
  else
    {
      blit_classic_SDL_light_radius();
    }

}; // void blit_light_radius ( void )



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
AssembleCombatPicture (int mask)
{
  int i;
  // SDL_Surface* right_sized_image ;

  isometric_show_floor_around_tux_without_doublebuffering ( mask );

  set_up_ordered_blitting_list ( mask );

  blit_preput_objects_according_to_blitting_list ( mask );

  if ( mask & SHOW_ITEMS )
    {
      for ( i = 0 ; i < MAX_ITEMS_PER_LEVEL ; i ++ )
	{
	  PutItem( i , mask , PUT_NO_THROWN_ITEMS );
	}
    }

  if ( ! GameConfig . skip_light_radius ) blit_light_radius();

  PutMiscellaneousSpellEffects ( );
      
  PutMouseMoveCursor ( );

  set_up_ordered_blitting_list ( mask );

  blit_nonpreput_objects_according_to_blitting_list ( mask );

  if (mask & ONLY_SHOW_MAP) 
    {
      // in case we only draw the map, we are done here.  But
      // of course we must check if we should update the screen too.
      if ( mask & DO_SCREEN_UPDATE ) 
	our_SDL_update_rect_wrapper( Screen , User_Rect.x , User_Rect.y , User_Rect.w , User_Rect.h );
      return;
    }

  show_obstacle_labels ( mask );

  ShowAutomapData();

  ShowCombatScreenTexts ( mask );

  //--------------------
  // Here are some more things, that are not needed in the level editor
  // view...
  //
  if ( ! ( mask & ONLY_SHOW_MAP_AND_TEXT ) )
    {
      ShowItemAlarm();
      ShowCharacterScreen ( );
      ShowSkillsScreen ( );
      ManageInventoryScreen ( );
      ShowQuickInventory ();
      DisplayButtons( );
      if ( ! GameOver )
	DisplayBanner (NULL, NULL,  0 ); // this is a pure client issue
    }

  if ( ServerMode )
    CenteredPrintStringFont ( Screen , Menu_BFont , SCREEN_HEIGHT/2 , " S E R V E R ! ! ! " );

  if ( GameConfig.Inventory_Visible ) 
    {
      User_Rect.x = SCREEN_WIDTH/2;
      User_Rect.w = SCREEN_WIDTH/2;
    }
  else if ( GameConfig.CharacterScreen_Visible || GameConfig.SkillScreen_Visible ) 
    {
      User_Rect.x = 0; // SCREEN_WIDTH/2;
      User_Rect.w = SCREEN_WIDTH/2;
    }
  else
    {
      User_Rect.x = 0;
      User_Rect.w = SCREEN_WIDTH;
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

}; // void AssembleCombatPicture(...)

/* -----------------------------------------------------------------
 * This function draws the mouse move cursor.
 * ----------------------------------------------------------------- */
void
PutMouseMoveCursor ( void )
{
  SDL_Rect TargetRectangle;

  if ( ( Me [ 0 ] . mouse_move_target . x == (-1) ) &&
       ( Me [ 0 ] . mouse_move_target_is_enemy == (-1) ) )
    {
      // DebugPrintf ( 0 , "\nMouse Move Target: x == (-1) ! and NO ENEMY TARGETED!! " );
      return;
    }

  if ( Me [ 0 ] . mouse_move_target_is_enemy == (-1) )
    {
      TargetRectangle . x = - Block_Width  / 2 +
	translate_map_point_to_screen_pixel ( Me [ 0 ] . mouse_move_target . x , Me [ 0 ] . mouse_move_target . y , TRUE );
      TargetRectangle . y = - Block_Height  / 2 +
	translate_map_point_to_screen_pixel ( Me [ 0 ] . mouse_move_target . x , Me [ 0 ] . mouse_move_target . y , FALSE );

    }
  else
    {
      // translate_map_point_to_screen_pixel ( float x_map_pos , float y_map_pos , int give_x )

      TargetRectangle . x = - Block_Width  / 2 +
	translate_map_point_to_screen_pixel ( AllEnemys [ Me [ 0 ] . mouse_move_target_is_enemy ] . pos . x , 
					      AllEnemys [ Me [ 0 ] . mouse_move_target_is_enemy ] . pos . y , TRUE );
      TargetRectangle . y = - Block_Height  / 2 +
	translate_map_point_to_screen_pixel ( AllEnemys [ Me [ 0 ] . mouse_move_target_is_enemy ] . pos . x , 
					      AllEnemys [ Me [ 0 ] . mouse_move_target_is_enemy ] . pos . y , FALSE );
    }

  if ( Me [ 0 ] . mouse_move_target_is_enemy == (-1) )
    our_SDL_blit_surface_wrapper ( MouseCursorImageList[ 0 ] , NULL , Screen , &TargetRectangle);
  else
    our_SDL_blit_surface_wrapper ( MouseCursorImageList[ 1 ] , NULL , Screen , &TargetRectangle);

}; // void PutMouseMoveCursor ( void )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
clear_all_loaded_tux_images ( int with_free )
{
  int i;
  int j;
  int k;

  strcpy ( previously_used_part_strings [ 0 ] , NOT_LOADED_MARKER );
  strcpy ( previously_used_part_strings [ 1 ] , NOT_LOADED_MARKER );
  strcpy ( previously_used_part_strings [ 2 ] , NOT_LOADED_MARKER );
  strcpy ( previously_used_part_strings [ 3 ] , NOT_LOADED_MARKER );
  strcpy ( previously_used_part_strings [ 4 ] , NOT_LOADED_MARKER );
  strcpy ( previously_used_part_strings [ 5 ] , NOT_LOADED_MARKER );
  
  for ( i = 0 ; i < ALL_TUX_PARTS ; i ++ )
    {
      for ( j = 0 ; j < TUX_TOTAL_PHASES ; j ++ )
	{
	  for ( k = 0 ; k < MAX_TUX_DIRECTIONS ; k ++ )
	    {
	      if ( ( with_free ) && ( loaded_tux_images [ i ] [ j ] [ k ] . surface != NULL ) )
		SDL_FreeSurface ( loaded_tux_images [ i ] [ j ] [ k ] . surface ) ;
	      loaded_tux_images [ i ] [ j ] [ k ] . surface = NULL ;
	    }
	}
    }
  
}; // void clear_all_loaded_tux_images ( int force_free )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
free_one_loaded_tux_image_series ( int tux_part_group )
{
  int j;
  int k;

  for ( j = 0 ; j < TUX_TOTAL_PHASES ; j ++ )
    {
      for ( k = 0 ; k < MAX_TUX_DIRECTIONS ; k ++ )
	{
	  if ( loaded_tux_images [ tux_part_group ] [ j ] [ k ] . surface != NULL )
	    SDL_FreeSurface ( loaded_tux_images [ tux_part_group ] [ j ] [ k ] . surface ) ;
	  loaded_tux_images [ tux_part_group ] [ j ] [ k ] . surface = NULL ;
	}
    }
  
}; // void free_one_loaded_tux_image_series ( int tux_part_group )

/* ----------------------------------------------------------------------
 * Now we determine the phase to use.  This is not all the same phase any 
 * more for all tux parts now that we've introduced a walk cycle.
 * ---------------------------------------------------------------------- */
int
get_current_phase ( int tux_part_group , int player_num , int motion_class ) 
{
  int our_phase = (int) Me [ player_num ] . phase ;
  float my_speed;

  if ( ( use_walk_cycle_for_part [ tux_part_group ] [ motion_class ] ) && ( Me [ player_num ] . weapon_swing_time < 0 ) )
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
get_motion_class ( player_num ) 
{
  int weapon_type = Me [ player_num ] . weapon_item . type ;
  int motion_class;
  int i;

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
      for ( i = 0 ; i < ALL_PART_GROUPS ; i ++ )
	{
	  clear_all_loaded_tux_images ( TRUE ) ;
	}
    }

  return ( motion_class );

}; // int get_motion_class ( player_num ) 

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
make_sure_tux_image_is_loaded ( int tux_part_group , int our_phase , int rotation_index , int motion_class , char* part_string )
{
  static char constructed_filename[5000];
  char* fpath;

  //--------------------
  // Now if the iso_image we want to blit right now has not yet been loaded,
  // then we need to do something about is and at least attempt to load the
  // surface
  //
  if ( loaded_tux_images [ tux_part_group ] [ our_phase ] [ rotation_index ] . surface == NULL )
    {
      //--------------------
      // Either we load all missing images at once or we just load the one missing
      // image right now and the next one when we come to it.
      //
      sprintf ( constructed_filename , "tux_motion_parts/%s/%s%s_%02d_%04d.png" , motion_class_string[motion_class] , part_group_strings [ tux_part_group ] , part_string , rotation_index , our_phase + 1 );
      fpath = find_file ( constructed_filename , GRAPHICS_DIR, FALSE );
      get_iso_image_from_file_and_path ( fpath , & ( loaded_tux_images [ tux_part_group ] [ our_phase ] [ rotation_index ] ) ) ;
      strcpy ( previously_used_part_strings [ tux_part_group ] , part_string );
    }

}; // void make_sure_tux_image_is_loaded ( int tux_part_group , int our_phase , int rotation_index )

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
  // If some part string given is unlike the part string we were using so
  // far, then we'll need to free that old part and (later) load the new
  // part.
  //
  for ( i = 0 ; i < ALL_PART_GROUPS ; i ++ )
    {
      if ( ! strcmp ( previously_used_part_strings [ tux_part_group ] , part_string ) ) 
	free_one_loaded_tux_image_series ( tux_part_group );
    }

  //--------------------
  // Now we need to resolve the part_string given as parameter
  //
  if ( strlen ( part_string ) == 0 )
    {
      GiveStandardErrorMessage ( "iso_put_tux(...)" , "\
Empty part string received!",
				 PLEASE_INFORM, IS_FATAL );
    }

  //--------------------
  // Now we determine the phase to use.  This is not all the same
  // phase any more for all tux parts now that we've introduced a walk cycle.
  //
  our_phase = get_current_phase ( tux_part_group , player_num , motion_class ) ;

  make_sure_tux_image_is_loaded ( tux_part_group , our_phase , rotation_index , motion_class , part_string );

  //--------------------
  // Now everything should be loaded correctly and we just need to blit the Tux.  Anything
  // that isn't loaded yet should be considered a serious bug and a reason to terminate 
  // immediately...
  //
  if ( loaded_tux_images [ tux_part_group ] [ our_phase ] [ rotation_index ] . surface != NULL )
    {
      blit_iso_image_to_map_position ( loaded_tux_images [ tux_part_group ] [ our_phase ] [ rotation_index ] , 
				       Me [ player_num ] . pos . x , Me [ player_num ] . pos . y );
    }
  else
    {
      GiveStandardErrorMessage ( "iso_put_tux(...)" , "Unable to load tux part!", PLEASE_INFORM, IS_FATAL );
    }

}; // void iso_put_tux_part ( char* part_string , int x , int y , int player_num )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
iso_put_tux_torso ( int x , int y , int player_num , int rotation_index )
{
  switch ( Me [ player_num ] . armour_item . type )
    {
    case -1 :
      iso_put_tux_part ( PART_GROUP_TORSO , "iso_torso" , x , y , player_num , rotation_index );
      break;
    case 29:
    case 30:
    case 31:
      iso_put_tux_part ( PART_GROUP_TORSO , "iso_robe" , x , y , player_num , rotation_index );
      break;
    default:
      iso_put_tux_part ( PART_GROUP_TORSO , "iso_armour1" , x , y , player_num , rotation_index );
      break;
    }

}; // void iso_put_tux_head ( int x , int y , int player_num , int rotation_index )

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
  if ( Me [ player_num ] . weapon_item . type == (-1) )
    {
      iso_put_tux_part ( PART_GROUP_SHIELD , "iso_buckler" , x , y , player_num , rotation_index );
      return;
    }

  //--------------------
  // In case of a weapon item present, we need to look up the weapon item motion class
  // and then decide which shield to use.
  //
  if ( ItemMap [ Me [ player_num ] . weapon_item . type ] . item_gun_angle_change == 0 )
    {
      iso_put_tux_part ( PART_GROUP_SHIELD , "iso_shieldarm" , x , y , player_num , rotation_index );
      return;
    }

  //--------------------
  // Now at this point we know, that a 'sword motion class' item is present, and that
  // we therefore need to blit the shield details.
  //
  if ( Me [ player_num ] . shield_item . type == ITEM_BUCKLER )
    iso_put_tux_part ( PART_GROUP_SHIELD , "iso_buckler" , x , y , player_num , rotation_index );
  else if ( Me [ player_num ] . shield_item . type == ITEM_SMALL_SHIELD )
    iso_put_tux_part ( PART_GROUP_SHIELD , "iso_small_shield" , x , y , player_num , rotation_index );
  else if ( Me [ player_num ] . shield_item . type == ITEM_STANDARD_SHIELD )
    iso_put_tux_part ( PART_GROUP_SHIELD , "iso_standard_shield" , x , y , player_num , rotation_index );
  else
    {
      fprintf ( stderr , "Shield item code: %d " , Me [ player_num ] . shield_item . type ) ;
      GiveStandardErrorMessage ( "iso_put_tux_shieldarm (...)" , "This shield type is not yet rendered for Tux." ,
				 PLEASE_INFORM, IS_FATAL );
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

}; // void iso_put_tux_head ( int x , int y , int player_num , int rotation_index )

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
	  iso_put_tux_part ( PART_GROUP_WEAPON , "iso_sword" , x , y , player_num , rotation_index );
	}
      else
	{
	  iso_put_tux_part ( PART_GROUP_WEAPON , "iso_gun1" , x , y , player_num , rotation_index );
	}
    }

}; // void iso_put_tux_weapon ( int x , int y , int player_num , int rotation_index )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
iso_put_all_tux_parts_for_sword_motion ( int x , int y , int player_num , int rotation_index )
{

  DebugPrintf ( 1 , "\nDirection given: %d." , rotation_index );
  // DebugPrintf ( 0 , "\nphase: %d." , (int) Me [ player_num ] . phase );

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
      GiveStandardErrorMessage ( "iso_put_all_tux_parts_in_direction (...)" , "\
Suspicious rotation index encountered!",
				 PLEASE_INFORM, IS_FATAL );
      break;
    }

}; // void iso_put_all_tux_parts_in_direction ( x , y , player_num , rotation_index )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
iso_put_all_tux_parts_for_gun_motion ( int x , int y , int player_num , int rotation_index )
{

  DebugPrintf ( 1 , "\nDirection given: %d." , rotation_index );
  DebugPrintf ( 1 , "\nphase: %d." , (int) Me [ player_num ] . phase );

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
      iso_put_tux_head ( x , y , player_num , rotation_index );
      iso_put_tux_weapon ( x , y , player_num , rotation_index );
      iso_put_tux_part ( PART_GROUP_WEAPONARM , "iso_weaponarm" , x , y , player_num , rotation_index );
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
      GiveStandardErrorMessage ( "iso_put_all_tux_parts_in_direction (...)" , "\
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

  Text_Rect.x=UserCenter_x + Block_Width/3;
  Text_Rect.y=UserCenter_y  - Block_Height/2;
  Text_Rect.w=User_Rect.w/2 - Block_Width/3;
  Text_Rect.h=User_Rect.h/2;

  DebugPrintf ( 2 , "\nvoid blit_tux(void): real function call confirmed." ) ;

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
      if ( Me [ player_num ] . status == OUT ) return;
      if ( Me [ player_num ] . pos . z != Me [ 0 ] . pos . z ) return;
      
      UpperLeftBlitCorner.x = UserCenter_x - Block_Width  / 2 ;
      UpperLeftBlitCorner.y = UserCenter_y - Block_Height / 2 ;

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
  if ( ( ( Me [ player_num ].energy * 100 / Me [ player_num ].maxenergy ) <= BLINKENERGY ) && ( x == (-1) ) ) 
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
      //      PutStringFont ( Screen , FPS_Display_BFont , 
      //		      User_Rect.x+(User_Rect.w/2) + Block_Width/3 , 
      //		      User_Rect.y+(User_Rect.h/2) - Block_Height/2 ,  
      //		      Me [ player_num ].TextToBeDisplayed );
      SetCurrentFont( FPS_Display_BFont );
      DisplayText( Me [ player_num ].TextToBeDisplayed , UserCenter_x + Block_Width/3,
		   UserCenter_y - Block_Height/2 , &Text_Rect );
    }

  DebugPrintf (2, "\nvoid blit_tux(void): enf of function reached.");

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

  //--------------------
  // At this point we can assume, that the enemys has been blittet to the
  // screen, whether it's a friendly enemy or not.
  // 
  // So now we can add some text the enemys says.  That might be fun.
  //
  if ( ( AllEnemys[Enum].TextVisibleTime < GameConfig.WantedTextVisibleTime )
       && GameConfig.All_Texts_Switch )
    {
      x_pos = translate_map_point_to_screen_pixel ( AllEnemys[ Enum ] . pos . x , AllEnemys [ Enum ] . pos . y , TRUE );
      y_pos = translate_map_point_to_screen_pixel ( AllEnemys[ Enum ] . pos . x , AllEnemys [ Enum ] . pos . y , FALSE )
	- 100 ;
      PutStringFont ( Screen , FPS_Display_BFont , 
		      x_pos ,  
		      y_pos ,  
		      AllEnemys[Enum].TextToBeDisplayed );
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

  /*
  // if enemy is on other level, return 
  if ( AllEnemys[Enum].pos.z != CurLevel->levelnum )
    {
      // DebugPrintf (3, "\nvoid PutEnemy(int Enum): DIFFERENT LEVEL-->usual end of function reached.\n");
      return;
    }
  */

  // if enemy is on other level, return 
  if ( AllEnemys[Enum].pos.z != Me [ 0 ] . pos . z )
    {
      // DebugPrintf (3, "\nvoid PutEnemy(int Enum): DIFFERENT LEVEL-->usual end of function reached.\n");
      return FALSE;
    }

  // if enemy is of type (-1), return 
  if ( AllEnemys[Enum].type == ( -1 ) )
    {
      // DebugPrintf (3, "\nvoid PutEnemy(int Enum): DIFFERENT LEVEL-->usual end of function reached.\n");
      return FALSE ;
    }

  if ( ! MakeSureEnemyIsInsideThisLevel ( &(AllEnemys[Enum] ) ) ) return ( FALSE );

  // if the enemy is out of sight, we need not do anything more here
  if ( ( ! show_all_droids ) && ( ! IsVisible ( & AllEnemys [ Enum ] . pos , 0 ) ) )
    {
      // DebugPrintf (3, "\nvoid PutEnemy(int Enum): ONSCREEN=FALSE --> usual end of function reached.\n");
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
  int Percentage;
  SDL_Rect FillRect;
  static Uint32 FullColor ;
  static Uint32 EmptyColor ;
#define ENEMY_ENERGY_BAR_OFFSET_X 0
#define ENEMY_ENERGY_BAR_OFFSET_Y (-20)
#define ENEMY_ENERGY_BAR_LENGTH 65

  return;

  //--------------------
  // If the enemy is dead already, there's nothing to do here...
  //
  if ( AllEnemys [ Enum ] . Status == OUT ) return;
  if ( AllEnemys [ Enum ] . energy <= 0 ) return;

  //--------------------
  // If the enemy is friendly, then we needn't display his health, right?
  //
  if ( AllEnemys [ Enum ] . is_friendly ) return;

  //--------------------
  // Now we need to find the right colors to fill our bars with...
  //
  FullColor = SDL_MapRGB( Screen->format, 255 , 0 , 0 ) ; 
  EmptyColor = SDL_MapRGB( Screen->format, 0 , 0 , 0 ) ; 

  //--------------------
  // Now we fill our bars...
  //
  Percentage = ( ENEMY_ENERGY_BAR_LENGTH * AllEnemys [ Enum ] . energy ) / Druidmap [ AllEnemys [ Enum ] . type ] . maxenergy ; 
  
  FillRect . x = TargetRectangle . x + ENEMY_ENERGY_BAR_OFFSET_X ;
  FillRect . y = TargetRectangle . y + ENEMY_ENERGY_BAR_OFFSET_Y ;
  FillRect . h = 7 ; 
  FillRect . w = Percentage ;

  our_SDL_fill_rect_wrapper ( Screen , &FillRect , FullColor ) ;

  FillRect . x = TargetRectangle . x + Percentage + ENEMY_ENERGY_BAR_OFFSET_X ;
  FillRect . y = TargetRectangle . y + ENEMY_ENERGY_BAR_OFFSET_Y ;
  FillRect . h = 7 ; 
  FillRect . w = ENEMY_ENERGY_BAR_LENGTH - Percentage ;
  
  our_SDL_fill_rect_wrapper ( Screen , &FillRect , EmptyColor ) ;

}; // void PutEnemyEnergyBar ( Enum , TargetRectangle )

/* ----------------------------------------------------------------------
 * This function is here to blit the 'body' of a droid to the screen, 
 * but the 'body' in the new and more modern sense with the 3d models
 * in various rotated forms as they are provided by Bastian.
 * This shape now depends upon the behaviour of the droid, which makes
 * everthing a little bit more complicated.
 * ---------------------------------------------------------------------- */
void
PutIndividuallyShapedDroidBody ( int Enum , SDL_Rect TargetRectangle , int mask )
{
  int phase = AllEnemys[Enum].phase;
  int RotationModel;
  int RotationIndex;
  enemy* ThisRobot = & ( AllEnemys [ Enum ] ) ;

  //--------------------
  // Now that the angle the robot is facing is determined, we just need to
  // translate this angle into an index within the image series, i.e. into a 'phase'
  // of rotation. 
  //
  // For this, several 'rounding' issues have to be taken into account!
  // But now it has optimal performance.
  //
  RotationIndex = ( ( ThisRobot -> current_angle - 45.0 + 360.0 + 360 / ( 2 * ROTATION_ANGLES_PER_ROTATION_MODEL ) ) * ROTATION_ANGLES_PER_ROTATION_MODEL / 360 ) ;
  while ( RotationIndex < 0  ) RotationIndex += ROTATION_ANGLES_PER_ROTATION_MODEL ; // just to make sure... a modulo ROTATION_ANGLES_PER_ROTATION_MODEL operation can't hurt
  while ( RotationIndex >= ROTATION_ANGLES_PER_ROTATION_MODEL ) RotationIndex -= ROTATION_ANGLES_PER_ROTATION_MODEL ; // just to make sure... a modulo ROTATION_ANGLES_PER_ROTATION_MODEL operation can't hurt

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
      if ( ThisRobot -> last_phase_change >= 0.33 )
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
  RotationModel = Druidmap [ ThisRobot -> type ] . individual_shape_nr ;
  
  //--------------------
  // A sanity check for roation model to use can never hurt...
  //
  if ( ( RotationModel < 0 ) || ( RotationModel >= ENEMY_ROTATION_MODELS_AVAILABLE ) )
    {
      GiveStandardErrorMessage ( "PutIndividuallyShapedDroidBody(...)" , "\
There was a rotation model type given, that exceeds the number of rotation models allowed and loaded in Freedroid.",
				     PLEASE_INFORM, IS_FATAL );
    }

  //--------------------
  // First we check if the robot is still alive.  If it isn't, 
  // then we can use the explosion dust from the classic ball-shaped
  // version.
  //
  if ( ( phase != DROID_PHASES ) || ( last_death_animation_image [ ThisRobot -> type ] - first_death_animation_image [ ThisRobot -> type ] > 0 ) )
    {
      
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
      //      if ( enemy_iso_images[ RotationModel ] [ RotationIndex ] -> w != Block_Width )
      // {
      if ( ( TargetRectangle . x != 0 ) && ( TargetRectangle . y != 0 ) )
	{
	  TargetRectangle.x -= ( enemy_iso_images[ RotationModel ] [ RotationIndex ] [ 0 ] . surface -> w ) / 2 ;
	  TargetRectangle.y -= ( enemy_iso_images[ RotationModel ] [ RotationIndex ] [ 0 ] . surface -> h ) / 2 ;
	  TargetRectangle.w = enemy_iso_images[ RotationModel ] [ RotationIndex ] [ 0 ] . surface -> w;
	  TargetRectangle.h = enemy_iso_images[ RotationModel ] [ RotationIndex ] [ 0 ] . surface -> h;
	}

      
      if ( AllEnemys[Enum].paralysation_duration_left != 0 ) 
	{
	  LoadAndPrepareRedEnemyRotationModelNr ( RotationModel );
	  // our_SDL_blit_surface_wrapper( RedEnemyRotationSurfacePointer [ RotationModel ] [ RotationIndex ] , NULL , Screen, &TargetRectangle);
	  blit_iso_image_to_map_position ( RedEnemyRotationSurfacePointer [ RotationModel ] [ RotationIndex ] [ 0 ] , 
					   ThisRobot -> pos . x , ThisRobot -> pos . y );
	}
      else if ( AllEnemys[Enum].poison_duration_left != 0 ) 
	{
	  LoadAndPrepareGreenEnemyRotationModelNr ( RotationModel );
	  // our_SDL_blit_surface_wrapper( GreenEnemyRotationSurfacePointer [ RotationModel ] [ RotationIndex ] , NULL , Screen, &TargetRectangle);
	  blit_iso_image_to_map_position ( GreenEnemyRotationSurfacePointer [ RotationModel ] [ RotationIndex ] [ 0 ] , 
					   ThisRobot -> pos . x , ThisRobot -> pos . y );

	}
      else if ( AllEnemys[Enum].frozen != 0 ) 
	{
	  LoadAndPrepareBlueEnemyRotationModelNr ( RotationModel );
	  // our_SDL_blit_surface_wrapper( BlueEnemyRotationSurfacePointer [ RotationModel ] [ RotationIndex ] , NULL , Screen, &TargetRectangle);
	  blit_iso_image_to_map_position ( BlueEnemyRotationSurfacePointer [ RotationModel ] [ RotationIndex ] [ 0 ] , 
					   ThisRobot -> pos . x , ThisRobot -> pos . y );
	}
      else
	{
	  if ( ( TargetRectangle . x != 0 ) && ( TargetRectangle . y != 0 ) )
	    {
	      our_SDL_blit_surface_wrapper( enemy_iso_images[ RotationModel ] [ RotationIndex ] [ 0 ] . surface , NULL , Screen, &TargetRectangle);
	      if ( GameConfig . enemy_energy_bars_visible )
		PutEnemyEnergyBar ( Enum , TargetRectangle );
	      return;
	    }
	  else
	    {
	      if ( mask & ZOOM_OUT )
		blit_zoomed_iso_image_to_map_position ( & ( enemy_iso_images[ RotationModel ] [ RotationIndex ] [ 0 ] ) , ThisRobot -> pos . x , ThisRobot -> pos . y );
	      else
		{
		  if ( last_death_animation_image [ RotationModel ] - first_walk_animation_image [ RotationModel ] == 0 )
		    {
		      blit_iso_image_to_map_position ( enemy_iso_images[ RotationModel ] [ RotationIndex ] [ 0 ] , ThisRobot -> pos . x , ThisRobot -> pos . y );
		    }
		  else
		    {
		      // blit_iso_image_to_map_position ( enemy_iso_images[ RotationModel ] [ RotationIndex ] [ ( SDL_GetTicks() / 100 ) % phases_in_enemy_animation [ RotationModel ] ] , ThisRobot -> pos . x , ThisRobot -> pos . y );
		      blit_iso_image_to_map_position ( enemy_iso_images [ RotationModel ] [ RotationIndex ] [ (int) ThisRobot -> animation_phase ] , ThisRobot -> pos . x , ThisRobot -> pos . y );
		    }
		}

	      TargetRectangle . x = 
		translate_map_point_to_screen_pixel ( AllEnemys[Enum].pos.x , AllEnemys[Enum].pos.y , TRUE );
	      TargetRectangle . y = 
		translate_map_point_to_screen_pixel ( AllEnemys[Enum].pos.x , AllEnemys[Enum].pos.y , FALSE )
		- ENEMY_ENERGY_BAR_OFFSET_Y ;

	      TargetRectangle.x -= ( enemy_iso_images[ RotationModel ] [ RotationIndex ] [ 0 ] . surface -> w ) / 2 ;
	      TargetRectangle.y -= ( enemy_iso_images[ RotationModel ] [ RotationIndex ] [ 0 ] . surface -> h ) / 1 ;
	      TargetRectangle.w = enemy_iso_images[ RotationModel ] [ RotationIndex ] [ 0 ] . surface -> w;
	      TargetRectangle.h = enemy_iso_images[ RotationModel ] [ RotationIndex ] [ 0 ] . surface -> h;


	      if ( GameConfig . enemy_energy_bars_visible )
		PutEnemyEnergyBar ( Enum , TargetRectangle );
	      return;
	    }
	}
    }
  
}; // void PutIndividuallyShapedDroidBody ( int Enum , SDL_Rect TargetRectangle );

/* ----------------------------------------------------------------------
 * This function draws an enemy into the combat window.
 * The only parameter given is the number of the enemy within the
 * AllEnemys array. Everything else is computed in here.
 * ---------------------------------------------------------------------- */
void
PutEnemy ( int Enum , int x , int y , int mask )
{
  char *druidname;	// the number-name of the Enemy 
  SDL_Rect TargetRectangle;
  point UpperLeftBlitCorner;

  if ( ! ThisEnemyNeedsToBeBlitted ( Enum , x , y ) ) return;

  //--------------------
  // We check for incorrect droid types, which sometimes might occor, especially after
  // heavy editing of the crew initialisation functions ;)
  //
  if ( AllEnemys[Enum].type >= Number_Of_Droid_Types )
    {
      GiveStandardErrorMessage ( "PutEnemy(...)" , "\
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
      UpperLeftBlitCorner.x = 0 ;
      UpperLeftBlitCorner.y = 0 ;
    }
  else
    {
      UpperLeftBlitCorner.x = x ;
      UpperLeftBlitCorner.y = y ;
    }

  //--------------------
  // First blit just the enemy hat and shoes.
  // The number will be blittet later
  //
  druidname = Druidmap[AllEnemys[Enum].type].druidname;

  TargetRectangle.x = UpperLeftBlitCorner.x ;
  TargetRectangle.y = UpperLeftBlitCorner.y ;
  // DebugPrintf( 0 , "X: %d." , TargetRectangle.x ); fflush(stdout);

  PutIndividuallyShapedDroidBody ( Enum , TargetRectangle , mask );

  // if this enemy is dead, we need not do anything more here
  if (AllEnemys[Enum].Status == OUT)
    {
      // DebugPrintf (3, "\nvoid PutEnemy(int Enum): STATUS==OUT --> usual end of function reached.\n");
      return;
    }

  PrintCommentOfThisEnemy ( Enum );

}; // void PutEnemy(int Enum , int x , int y) 

/* ----------------------------------------------------------------------
 * This function draws a Bullet into the combat window.  The only 
 * parameter given is the number of the bullet in the AllBullets 
 * array. Everything else is computed in here.
 * ---------------------------------------------------------------------- */
void
PutBullet ( int Bullet_number , int mask )
{
  Bullet CurBullet = &AllBullets[Bullet_number];
  int PhaseOfBullet;
  int direction_index;

  if ( CurBullet -> time_to_hide_still > 0 ) return ;

  //--------------------
  // in case our bullet is of the type "FLASH", we only
  // draw a big white or black rectangle right over the 
  // combat window, white for even frames and black for 
  // odd frames.
  if (CurBullet->type == FLASH)
    {
      // Now the whole window will be filled with either white
      // or black each frame until the flash is over.  (Flash 
      // deletion after some time is done in CheckBulletCollisions.)
      if ( (CurBullet->time_in_frames % 2) == 1)
	{
	  FlashWindow (flashcolor1);
	  return;
	}
      if ( (CurBullet->time_in_frames % 2) == 0)
	{
	  FlashWindow (flashcolor2);
	  return;
	}
    } // if type == FLASH

  // DebugPrintf( 0 , "\nBulletType before calculating phase : %d." , CurBullet->type );
  if ( CurBullet->type >= Number_Of_Bullet_Types ) 
    {
      GiveStandardErrorMessage ( "PutBullet(...)" , "\
There was a bullet to be blitted of a type that does not really exist.",
				 PLEASE_INFORM, IS_FATAL );
    };

  PhaseOfBullet = (CurBullet->time_in_seconds * Bulletmap[ CurBullet->type ].phase_changes_per_second );

  PhaseOfBullet = PhaseOfBullet % Bulletmap[CurBullet->type].phases ;
  // DebugPrintf( 0 , "\nPhaseOfBullet: %d.", PhaseOfBullet );

  direction_index = ( ( CurBullet -> angle + 360.0 + 360 / ( 2 * BULLET_DIRECTIONS ) ) * BULLET_DIRECTIONS / 360 ) ;
  while ( direction_index < 0  ) direction_index += BULLET_DIRECTIONS ; // just to make sure... a modulo ROTATION_ANGLES_PER_ROTATION_MODEL operation can't hurt
  while ( direction_index >= BULLET_DIRECTIONS ) direction_index -= BULLET_DIRECTIONS ; // just to make sure... a modulo ROTATION_ANGLES_PER_ROTATION_MODEL operation can't hurt

  if ( mask & ZOOM_OUT )
    blit_zoomed_iso_image_to_map_position ( & ( Bulletmap [ CurBullet -> type ] . image [ direction_index ] [ PhaseOfBullet ] ) , CurBullet -> pos . x , CurBullet -> pos . y );
  else
    blit_iso_image_to_map_position ( Bulletmap [ CurBullet -> type ] . image [ direction_index ] [ PhaseOfBullet ] , CurBullet -> pos . x , CurBullet -> pos . y );

}; // void PutBullet (int Bulletnumber )

/* ----------------------------------------------------------------------
 * This function draws an item into the combat window.
 * The only given parameter is the number of the item within
 * the AllItems array.
 * ---------------------------------------------------------------------- */
void
PutItem( int ItemNumber , int mask , int put_thrown_items_flag )
{
  Level ItemLevel = curShip . AllLevels [ Me [ 0 ] . pos . z ] ;
  Item CurItem = &ItemLevel -> ItemList [ ItemNumber ] ;
  // gps ItemGPS;

  //--------------------
  // The unwanted cases MUST be handled first...
  //
  if ( CurItem->type == ( -1 ) ) 
    {
      return;
      fprintf( stderr, "\n\nItemNumber '%d'\n" , ItemNumber );
      GiveStandardErrorMessage ( "PutItem(...)" , "\
There was -1 item type given to blit.  This must be a mistake! ",
				 PLEASE_INFORM, IS_FATAL );
    }
  if ( ItemMap[ CurItem->type ].picture_number >= NUMBER_OF_ITEM_PICTURES ) 
    {
      fprintf( stderr, "\n\nItemMap[ CurItem->type ].picture_number '%d'\n" , ItemMap[ CurItem->type ].picture_number );
      GiveStandardErrorMessage ( "PutItem(...)" , "\
There was an item type given, that exceeds the range of item images loaded.",
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
  if ( ItemImageList[ ItemMap[ CurItem->type ] . picture_number ] . ingame_iso_image . surface == NULL )
    try_to_load_ingame_item_surface ( CurItem -> type );

  if ( mask & ZOOM_OUT )
    blit_zoomed_iso_image_to_map_position ( & ( ItemImageList[ ItemMap[ CurItem->type ] . picture_number ] . ingame_iso_image ) , 
					    CurItem -> pos . x , CurItem -> pos . y );
  else
    {
      blit_iso_image_to_map_position ( ItemImageList[ ItemMap[ CurItem->type ] . picture_number ] . ingame_iso_image , 
				       CurItem -> pos . x - 3.0 * sinf ( CurItem -> throw_time * 3.0 ) , 
				       CurItem -> pos . y - 3.0 * sinf ( CurItem -> throw_time * 3.0 ) );
    }

}; // void PutItem( int ItemNumber );

/* ----------------------------------------------------------------------
 * This function draws an item into the combat window.
 * The only given parameter is the number of the item within
 * the AllItems array.
 * ---------------------------------------------------------------------- */
void
PutRadialBlueSparks( float PosX, float PosY , float Radius , int SparkType )
{
#define FIXED_NUMBER_OF_SPARK_ANGLES 12
#define FIXED_NUMBER_OF_PROTOTYPES 4
#define NUMBER_OF_SPARK_TYPES 3

  SDL_Rect TargetRectangle;
  static SDL_Surface* SparkPrototypeSurface [ NUMBER_OF_SPARK_TYPES ] [ FIXED_NUMBER_OF_PROTOTYPES ] = { { NULL , NULL , NULL , NULL } , { NULL , NULL , NULL , NULL } } ;
  static SDL_Surface* PrerotatedSparkSurfaces [ NUMBER_OF_SPARK_TYPES ] [ FIXED_NUMBER_OF_PROTOTYPES ] [ FIXED_NUMBER_OF_SPARK_ANGLES ];
  SDL_Surface* tmp_surf;
  char* fpath;
  int NumberOfPicturesToUse;
  int i , k ;
  float Angle;
  int PrerotationIndex;
  moderately_finepoint Displacement;
  int PictureType;
  char ConstructedFilename[5000];

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
	      GiveStandardErrorMessage ( "PutRadialBlueSparks(...)" , "\
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
	      GiveStandardErrorMessage ( "PutRadialBlueSparks(...)" , "\
Freedroid encountered a radial wave type that does not exist in Freedroid.",
					 PLEASE_INFORM, IS_FATAL );
	    }	      
	  
	  fpath = find_file ( ConstructedFilename , GRAPHICS_DIR, FALSE );

	  tmp_surf = our_IMG_load_wrapper( fpath );
	  if ( tmp_surf == NULL )
	    {
	      fprintf( stderr, "\n\nfpath: '%s'\n" , fpath );
	      GiveStandardErrorMessage ( "PutRadialBlueSparks(...)" , "\
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
	      Angle = -45 + 360.0 * (float)i / (float)FIXED_NUMBER_OF_SPARK_ANGLES ;
	      
	      tmp_surf = 
		rotozoomSurface( SparkPrototypeSurface [ SparkType ] [ k ] , Angle , 1.0 , FALSE );
	      
	      PrerotatedSparkSurfaces [ SparkType ] [ k ] [ i ] = our_SDL_display_format_wrapperAlpha ( tmp_surf );
	      
	      SDL_FreeSurface ( tmp_surf );
	    }
	}

    }

  NumberOfPicturesToUse = 2 * ( 2 * Radius * Block_Width * 3.14 ) / (float) SparkPrototypeSurface[ SparkType ] [ PictureType ] -> w;
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

      PrerotationIndex = rintf ( Angle * (float)FIXED_NUMBER_OF_SPARK_ANGLES / 360.0 ); 
      if ( PrerotationIndex >= FIXED_NUMBER_OF_SPARK_ANGLES ) PrerotationIndex = 0 ;

      TargetRectangle . x = translate_map_point_to_screen_pixel ( PosX + Displacement . x , PosY + Displacement . y , TRUE ) - ( ( PrerotatedSparkSurfaces [ SparkType ] [ PictureType ] [ PrerotationIndex ] -> w ) / 2 );
      TargetRectangle . y = translate_map_point_to_screen_pixel ( PosX + Displacement . x , PosY + Displacement . y , FALSE ) - ( ( PrerotatedSparkSurfaces [ SparkType ] [ PictureType ] [ PrerotationIndex ] -> h ) / 2 );

      our_SDL_blit_surface_wrapper( PrerotatedSparkSurfaces [ SparkType ] [ PictureType ] [ PrerotationIndex ] , NULL , Screen , &TargetRectangle);

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
	  GiveStandardErrorMessage ( "PutRadialBlueSparksBestQuality(...)" , "\
Freedroid wanted to load a certain image file into memory, but the SDL\n\
function used for this did not succeed.",
				     PLEASE_INFORM, IS_FATAL );
	}
      // SDL_SetColorKey( tmp_surf , 0 , 0 ); 

      SparkPrototypeSurface = our_SDL_display_format_wrapperAlpha ( tmp_surf );

      SDL_FreeSurface( tmp_surf );
    }

  NumberOfPicturesToUse = ( 2 * Radius * Block_Width * 3.14 ) / (float) SparkPrototypeSurface -> w;
  NumberOfPicturesToUse += 3 ; // we want some overlap

  //--------------------
  // Now we blit all the pictures we like to use...in this case using
  // multiple dynamic rotations (oh god!)...
  //
  for ( i = 0 ; i < NumberOfPicturesToUse ; i++ )
    {
      Angle = 360.0 * (float)i / (float)NumberOfPicturesToUse ;
      
      Displacement . x = 0 ; Displacement . y = - Radius * Block_Height ;

      RotateVectorByAngle ( &Displacement , Angle );

      tmp_surf = 
	rotozoomSurface( SparkPrototypeSurface , Angle , 1.0 , FALSE );

      TargetRectangle . x = UserCenter_x - ( Me [ 0 ] . pos . x - PosX ) * Block_Width  + Displacement . x - ( (tmp_surf -> w) / 2 );
      TargetRectangle . y = UserCenter_y - ( Me [ 0 ] . pos . y - PosY ) * Block_Height + Displacement . y - ( (tmp_surf -> h) / 2 );
      
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
  // SDL_Rect TargetRectangle;

  // If the blast is already long dead, we need not do anything else here
  if (CurBlast->type == OUT)
    return;

  // DebugPrintf( 0 , "\nBulletType before calculating phase : %d." , CurBullet->type );
  if ( CurBlast->type >= ALLBLASTTYPES ) 
    {
      GiveStandardErrorMessage ( "PutBlast(...)" , "\
The PutBlast function should blit a blast of a type that does not\n\
exist at all.",
				 PLEASE_INFORM, IS_FATAL );
    };
  
  blit_iso_image_to_map_position ( Blastmap [ CurBlast -> type ] . image [ (int)floorf(CurBlast->phase) ] , 
				   CurBlast -> pos . x , CurBlast -> pos . y  );

};  // void PutBlast(int Blast_number)

/* ----------------------------------------------------------------------
 * This function fills the combat window with one single color, given as
 * the only parameter to the function.
 * ---------------------------------------------------------------------- */
void
FlashWindow (SDL_Color Flashcolor)
{
  FillRect( User_Rect, Flashcolor);
}; // void FlashWindow(int Flashcolor)


/* -----------------------------------------------------------------
 * Fill given rectangle with given RBG color
 * ----------------------------------------------------------------- */
void
FillRect (SDL_Rect rect, SDL_Color color)
{
  Uint32 pixcolor;
  SDL_Rect tmp;

  Set_Rect (tmp, rect.x, rect.y, rect.w, rect.h);

  pixcolor = SDL_MapRGB (Screen->format, color.r, color.g, color.b);

  our_SDL_fill_rect_wrapper (Screen, &tmp, pixcolor);
  
  return;
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

  DebugPrintf (2, "\nvoid ShowRobotPicture(...): Function call confirmed.");

  // strcpy( fname, Druidmap[Number].druidname );
  strcpy( fname, "droids/" );
  strcat( fname, Druidmap[Number].portrait_filename_without_ext );
  strcat( fname , "/portrait.png" );
  DebugPrintf (2, "\ntrying to load this: $fname");
  fpath = find_file (fname, GRAPHICS_DIR, FALSE);

  if ( (tmp=our_IMG_load_wrapper (fpath)) == NULL )
    {
      fprintf( stderr, "\n\nfpath '%s' SDL_GetError(): %s. \n" , fpath, SDL_GetError() );
      GiveStandardErrorMessage ( "ShowRobotPicture(...)" , "\
A droid portrait failed to load.",
				 PLEASE_INFORM, IS_FATAL );
    }
  

  SDL_SetClipRect( Screen , NULL );
  Set_Rect (target, PosX, PosY, SCREEN_WIDTH, SCREEN_HEIGHT);
  our_SDL_blit_surface_wrapper( tmp , NULL, Screen , &target);

  SDL_FreeSurface(tmp);

  DebugPrintf (2, "\nvoid ShowRobotPicture(...): Usual end of function reached.");
}; // void ShowRobotPicture ( ... )


/* ----------------------------------------------------------------------
 * This function displays the inventory screen and also fills in all the
 * items the influencer is carrying in his inventory and also all the 
 * items the influencer is fitted with.
 * ---------------------------------------------------------------------- */
void
ShowInventoryScreen( void )
{
  static SDL_Surface *TransparentPlateImage = NULL;
  SDL_Surface *tmp;
  char *fpath;
  char fname2 [ ] = INVENTORY_SQUARE_OCCUPIED_FILE;
  SDL_Rect TargetRect;
  int SlotNum;
  int i , j ;

  // --------------------
  // Some things like the loading of the inventory and initialisation of the
  // inventory rectangle need to be done only once at the first call of this
  // function. 
  //
  if ( TransparentPlateImage == NULL )
    {
      fpath = find_file ( fname2 , GRAPHICS_DIR, FALSE);
      tmp = our_IMG_load_wrapper( fpath );
      if ( !tmp )
	{
	  fprintf( stderr, "\n\nfname2: '%s'\n" , fname2 );
	  GiveStandardErrorMessage ( "ShowInventoryScreen(...)" , "\
The transparent plate for the inventory could not be loaded.  This is a fatal error.",
				     PLEASE_INFORM, IS_FATAL );
	}
      TransparentPlateImage = our_SDL_display_format_wrapperAlpha ( tmp );
      SDL_FreeSurface ( tmp );

      //--------------------
      // We define the right side of the user screen as the rectangle
      // for our inventory screen.
      //
      InventoryRect.x = 0;
      // InventoryRect.y = SCREEN_HEIGHT - InventoryImage->h;
      InventoryRect.y = User_Rect.y;
      InventoryRect.w = SCREEN_WIDTH/2;
      InventoryRect.h = User_Rect.h;

    }

  //--------------------
  // At this point we know, that the inventory screen is desired and must be
  // displayed in-game:
  //
  blit_special_background ( 0 );

  //--------------------
  // Now we display the item in the influencer drive slot
  //
  TargetRect.x = InventoryRect.x + DRIVE_RECT_X;
  TargetRect.y = InventoryRect.y + DRIVE_RECT_Y;
  if ( ( ! Me[0].drive_item.currently_held_in_hand ) && ( Me[0].drive_item.type != (-1) ) )
    {
      our_SDL_blit_surface_wrapper( ItemImageList[ ItemMap[ Me[0].drive_item.type ].picture_number ].Surface , NULL , Screen , &TargetRect );
    }

  //--------------------
  // Now we display the item in the influencer weapon slot
  // At this point we have to pay extra care, cause the weapons in Freedroid
  // really come in many different sizes.
  //
  TargetRect.x = InventoryRect.x + WEAPON_RECT_X;
  TargetRect.y = InventoryRect.y + WEAPON_RECT_Y;
  if ( ( ! Me[0].weapon_item.currently_held_in_hand ) && ( Me[0].weapon_item.type != (-1) ) )
    {
      TargetRect.x += 32 * 0.5 * ( 2 - ItemImageList [ ItemMap[ Me[0].weapon_item.type ] . picture_number ] . inv_size . x ) ;
      TargetRect.y += 32 * 0.5 * ( 3 - ItemImageList [ ItemMap[ Me[0].weapon_item.type ] . picture_number ] . inv_size . y ) ;
      our_SDL_blit_surface_wrapper( ItemImageList[ ItemMap[ Me[0].weapon_item.type ].picture_number ] . Surface , NULL , Screen , &TargetRect );
    }

  //--------------------
  // Now we display the item in the influencer armour slot
  //
  TargetRect.x = InventoryRect.x + ARMOUR_POS_X ;
  TargetRect.y = InventoryRect.y + ARMOUR_POS_Y ;
  if ( ( ! Me[0].armour_item.currently_held_in_hand ) && ( Me[0].armour_item.type != (-1) ) )
    {
      our_SDL_blit_surface_wrapper( ItemImageList[ ItemMap[ Me[0].armour_item.type ].picture_number ].Surface , NULL , Screen , &TargetRect );
    }

  //--------------------
  // Now we display the item in the influencer shield slot
  //
  TargetRect.x = InventoryRect.x + SHIELD_POS_X ;
  TargetRect.y = InventoryRect.y + SHIELD_POS_Y ;
  if ( ( ! Me[0].shield_item.currently_held_in_hand ) && ( Me[0].shield_item.type != (-1) ) )
    {
      our_SDL_blit_surface_wrapper( ItemImageList[ ItemMap[ Me[0].shield_item.type ].picture_number ].Surface , NULL , Screen , &TargetRect );
    }
  
  //--------------------
  // Now we display the item in the influencer special slot
  //
  TargetRect.x = InventoryRect.x + SPECIAL_POS_X ;
  TargetRect.y = InventoryRect.y + SPECIAL_POS_Y ;
  if ( ( ! Me[0].special_item.currently_held_in_hand ) && ( Me[0].special_item.type != (-1) ) )
    {
      our_SDL_blit_surface_wrapper( ItemImageList[ ItemMap[ Me[0].special_item.type ].picture_number ].Surface , NULL , Screen , &TargetRect );
    }

  //--------------------
  // Now we display the item in the influencers aux1 slot
  //
  TargetRect.x = InventoryRect.x + AUX1_POS_X ;
  TargetRect.y = InventoryRect.y + AUX1_POS_Y ;
  if ( ( ! Me[0].aux1_item.currently_held_in_hand ) && ( Me[0].aux1_item.type != (-1) ) )
    {
      our_SDL_blit_surface_wrapper( ItemImageList[ ItemMap[ Me[0].aux1_item.type ].picture_number ].Surface , NULL , Screen , &TargetRect );
    }

  //--------------------
  // Now we display the item in the influencers aux2 slot
  //
  TargetRect.x = InventoryRect.x + AUX2_POS_X ;
  TargetRect.y = InventoryRect.y + AUX2_POS_Y ;
  if ( ( ! Me[0].aux2_item.currently_held_in_hand ) && ( Me[0].aux2_item.type != (-1) ) )
    {
      our_SDL_blit_surface_wrapper( ItemImageList[ ItemMap[ Me[0].aux2_item.type ].picture_number ].Surface , NULL , Screen , &TargetRect );
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

      for ( i = 0 ; i < ItemImageList[ ItemMap[ Me[0].Inventory[ SlotNum ].type ].picture_number ].inv_size.y ; i++ )
	{
	  for ( j = 0 ; j < ItemImageList[ ItemMap[ Me[0].Inventory[ SlotNum ].type ].picture_number ].inv_size.x ; j++ )
	    {
	      TargetRect.x = INVENTORY_RECT_X + 32 * ( Me[0].Inventory[ SlotNum ].inventory_position.x + j );
	      TargetRect.y = User_Rect.y + INVENTORY_RECT_Y + 32 * ( Me[0].Inventory[ SlotNum ].inventory_position.y + i );
	    
	      our_SDL_blit_surface_wrapper( TransparentPlateImage , NULL , Screen , &TargetRect );
	    }
	}

      TargetRect.x = INVENTORY_RECT_X + 32 * Me[0].Inventory[ SlotNum ].inventory_position.x;
      TargetRect.y = User_Rect.y +INVENTORY_RECT_Y + 32 * Me[0].Inventory[ SlotNum ].inventory_position.y;
      
      our_SDL_blit_surface_wrapper( ItemImageList[ ItemMap[ Me[0].Inventory[ SlotNum ].type ].picture_number ].Surface , NULL , Screen , &TargetRect );

    }
}; // void ShowInventoryScreen( void )


#undef _view_c
