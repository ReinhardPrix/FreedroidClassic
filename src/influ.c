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
 * This file contains all features, movement, fireing, collision and 
 * extras of the influencer.
 * ---------------------------------------------------------------------- */

#define _influ_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"

#define TIMETILLNEXTBULLET 14

#define FRICTION_CONSTANT (0.02)

#define BEST_MELEE_DISTANCE (1.0)
#define BEST_CHAT_DISTANCE (BEST_MELEE_DISTANCE+0.2)
// #define DISTANCE_TOLERANCE (0.2)
#define DISTANCE_TOLERANCE (0.00002)

#define FORCE_FIRE_DISTANCE (1.5)
#define ATTACK_BOXES_DISTANCE (2.0)
#define DROID_SELECTION_TOLERANCE (0.5)

#define REFRESH_ENERGY		3
#define COLLISION_PUSHSPEED	7

#define BOUNCE_LOSE_ENERGY 3	/* amount of lose-energy at enemy-collisions */
#define BOUNCE_LOSE_FACT 1

#define MAXIMAL_STEP_SIZE ( 7.0/20.0 )

#define DEBUG_TUX_PATHFINDING 1  // debug level for tux pathfinding...

void InfluEnemyCollisionLoseEnergy (int enemynum);	/* influ can lose energy on coll. */
int NoInfluBulletOnWay (void);
void limit_tux_speed_to_a_maximum ( int player_num );
void set_up_intermediate_course_for_tux ( int player_num );
void clear_out_intermediate_points ( int player_num );
void check_for_chests_to_open ( int player_num , int chest_index ) ;
void check_for_barrels_to_smash ( int player_num , int index_of_barrel_below_mouse_cursor ) ;

char recursion_grid[ MAX_MAP_LINES ][ MAX_MAP_LINES ] ;
// moderately_finepoint first_found_walkable_point;
moderately_finepoint last_sight_contact;
int next_index_to_set_up = 30000 ;
int bad_luck_in_4_directions_counter = 0;
int no_left_button_press_in_previous_analyze_mouse_click = FALSE ;

#define TILE_IS_UNPROCESSED 3
#define TILE_IS_PROCESSED 4

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
float
calc_euklid_distance ( float pos1_x , float pos1_y , float pos2_x , float pos2_y ) 
{
  return sqrt ( (pos1_x-pos2_x)*(pos1_x-pos2_x) + (pos1_y-pos2_y)*(pos1_y-pos2_y) ) ;
};

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
float
vect_len ( moderately_finepoint our_vector )
{
  return ( sqrt ( ( our_vector . x * our_vector . x ) + ( our_vector . y * our_vector . y ) ) ) ;
}; // float vect_len ( moderately_finepoint our_vector )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
int
find_free_floor_items_index ( int levelnum ) 
{
  int i;
  Level DropLevel = curShip . AllLevels [ levelnum ] ;

  for ( i = 0 ; i < MAX_ITEMS_PER_LEVEL ; i ++ )
    {
      if ( DropLevel -> ItemList [ i ] . type == ( -1 ) ) return ( i ) ;
    }

  GiveStandardErrorMessage ( "find_free_floor_items_index(...)" , "FreedroidRPG failed to find a free items index for an item it wanted to put on the floor.\nThis case means that there are too many items on the floor of this level for the current game engine.\nA constant needs to be raised or the engine improved.",
			     PLEASE_INFORM, IS_FATAL );

  return ( 0 ) ;
  
}; // int find_free_floor_items_index ( int levelnum ) 

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
throw_out_all_chest_content ( int obst_index )
{
  Level chest_level;
  int i;
  int j;
  moderately_finepoint throw_out_offset_vector = { 0 , 0.9 } ;

  chest_level = curShip . AllLevels [ Me [ 0 ] . pos . z ] ;

  DebugPrintf ( 0 , "\nthrow_out_all_chest_content: call confimed." );

  //--------------------
  // First some check if the given obstacle is really a closed chest.
  //
  switch ( chest_level -> obstacle_list [ obst_index ] . type )
    {
    case ISO_H_CHEST_CLOSED:
    case ISO_V_CHEST_CLOSED:
      // all is ok in this case.  it's really a chest.  fine.
      break;
    default: 
      // no chest handed as the chest obstacle!  Clearly a severe error.!
      GiveStandardErrorMessage ( "throw_out_all_chest_content(...)" , "Obstacle given to empty is not really a chest!" ,
				 PLEASE_INFORM, IS_FATAL );
      break;
    }

  //--------------------
  // Now we can throw out all the items from inside the chest and maybe
  // (later) also play a 'chest opening' sound.
  //
  for ( i = 0 ; i < MAX_CHEST_ITEMS_PER_LEVEL ; i ++ )
    {
      if ( chest_level -> ChestItemList [ i ] . type == (-1) ) continue;
      if ( fabsf ( chest_level -> obstacle_list [ obst_index ] . pos . x - chest_level -> ChestItemList [ i ] . pos . x ) > 0.1 ) continue ;
      if ( fabsf ( chest_level -> obstacle_list [ obst_index ] . pos . y - chest_level -> ChestItemList [ i ] . pos . y ) > 0.1 ) continue ;
      
      //--------------------
      // So this item is one of the right one and will now get thrown out of the chest:
      // 
      // First we find a free items index on this level.
      //
      DebugPrintf ( 0 , "\nOne item now thrown out of the chest..." );
      j = find_free_floor_items_index ( Me [ 0 ] . pos . z ) ;
      MoveItem ( & ( chest_level -> ChestItemList [ i ] ) , & ( chest_level -> ItemList [ j ] ) ) ;

      chest_level -> ItemList [ j ] . pos . x += throw_out_offset_vector . x ;
      chest_level -> ItemList [ j ] . pos . y += throw_out_offset_vector . y ;
      chest_level -> ItemList [ j ] . throw_time = 0.01 ;
      RotateVectorByAngle ( & throw_out_offset_vector , 45 );
    }

  //--------------------
  // We play the sound, now that the chest is really opened...
  //
  play_open_chest_sound ( ) ;

}; // void throw_out_all_chest_content ( int obst_index )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
int
closed_chest_below_mouse_cursor ( int player_num ) 
{
  finepoint MapPositionOfMouse;
  int i;
  int obst_index ;

  if ( CursorIsInUserRect( GetMousePos_x() + MOUSE_CROSSHAIR_OFFSET_X , GetMousePos_y() + MOUSE_CROSSHAIR_OFFSET_Y ) && ( CurLevel != NULL ) )
    {
      MapPositionOfMouse.x = translate_pixel_to_map_location ( player_num , 
							       (float) ServerThinksInputAxisX ( player_num ) , 
							       (float) ServerThinksInputAxisY ( player_num ) , TRUE ) ;
      MapPositionOfMouse.y = translate_pixel_to_map_location ( player_num , 
							       (float) ServerThinksInputAxisX ( player_num ) , 
							       (float) ServerThinksInputAxisY ( player_num ) , FALSE ) ;

      for ( i = 0 ; i < MAX_OBSTACLES_GLUED_TO_ONE_MAP_TILE ; i++ )
	{
	  if ( ( ( (int) MapPositionOfMouse . x ) < 0 ) ||
	       ( ( (int) MapPositionOfMouse . y ) < 0 ) ||
	       ( ( (int) MapPositionOfMouse . x ) >= CurLevel -> xlen ) ||
	       ( ( (int) MapPositionOfMouse . y ) >= CurLevel -> ylen ) ) return ( -1 ) ;

	  obst_index = CurLevel -> map [ (int) MapPositionOfMouse . y ] [ (int) MapPositionOfMouse . x ] . obstacles_glued_to_here [ i ] ;

	  if ( obst_index == (-1) ) continue;

	  switch ( CurLevel -> obstacle_list [ obst_index ] . type )
	    {
	    case ISO_H_CHEST_CLOSED:
	    case ISO_V_CHEST_CLOSED:
	      // DebugPrintf ( 0 , "\nBANNER: Cursor is now on closed chest!!!" );
	      // strcpy ( ItemDescText , "  C  H  E  S  T  ! ! ! " ) ;
	      return ( obst_index ) ;
	      break;
		
	    default: 
	      break;
	    }
	}
    }

  return ( -1 ) ;

}; // int closed_chest_below_mouse_cursor ( int player_num ) 

/* ----------------------------------------------------------------------
 * This function checks if there is a smashable barrel under the mouse
 * cursor and if this is so, then the obstacle index of that barrel will
 * be returned.  Else -1 is returned for no barrel.
 * ---------------------------------------------------------------------- */
int
smashable_barred_below_mouse_cursor ( int player_num ) 
{
  finepoint MapPositionOfMouse;
  int i;
  int obst_index ;

  if ( CursorIsInUserRect( GetMousePos_x() + MOUSE_CROSSHAIR_OFFSET_X , 
			   GetMousePos_y() + MOUSE_CROSSHAIR_OFFSET_Y ) && ( CurLevel != NULL ) )
    {
      MapPositionOfMouse.x = translate_pixel_to_map_location ( player_num , 
							       (float) ServerThinksInputAxisX ( player_num ) , 
							       (float) ServerThinksInputAxisY ( player_num ) , TRUE ) ;
      MapPositionOfMouse.y = translate_pixel_to_map_location ( player_num , 
							       (float) ServerThinksInputAxisX ( player_num ) , 
							       (float) ServerThinksInputAxisY ( player_num ) , FALSE ) ;

      for ( i = 0 ; i < MAX_OBSTACLES_GLUED_TO_ONE_MAP_TILE ; i++ )
	{
	  if ( ( ( (int) MapPositionOfMouse . x ) < 0 ) ||
	       ( ( (int) MapPositionOfMouse . y ) < 0 ) ||
	       ( ( (int) MapPositionOfMouse . x ) >= CurLevel -> xlen ) ||
	       ( ( (int) MapPositionOfMouse . y ) >= CurLevel -> ylen ) ) return ( -1 ) ;

	  obst_index = CurLevel -> map [ (int) MapPositionOfMouse . y ] [ (int) MapPositionOfMouse . x ] . obstacles_glued_to_here [ i ] ;

	  if ( obst_index == (-1) ) continue;

	  switch ( CurLevel -> obstacle_list [ obst_index ] . type )
	    {
	    case ISO_BARREL_1:
	    case ISO_BARREL_2:
	    case ISO_BARREL_3:
	    case ISO_BARREL_4:
	      return ( obst_index ) ;
	      break;
		
	    default: 
	      break;
	    }
	}
    }

  return ( -1 ) ;

}; // int smashable_barred_below_mouse_cursor ( int player_num ) 

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
teleporter_square_below_mouse_cursor ( int player_num , char* ItemDescText )
{
  finepoint MapPositionOfMouse;
  int i;
  int action_number;

  if ( CursorIsInUserRect( GetMousePos_x() + MOUSE_CROSSHAIR_OFFSET_X , 
			   GetMousePos_y() + MOUSE_CROSSHAIR_OFFSET_Y ) && ( CurLevel != NULL ) )
    {
      MapPositionOfMouse.x = translate_pixel_to_map_location ( player_num , 
							       (float) ServerThinksInputAxisX ( player_num ) , 
							       (float) ServerThinksInputAxisY ( player_num ) , TRUE ) ;
      MapPositionOfMouse.y = translate_pixel_to_map_location ( player_num , 
							       (float) ServerThinksInputAxisX ( player_num ) , 
							       (float) ServerThinksInputAxisY ( player_num ) , FALSE ) ;

      for ( i = 0 ; i < MAX_EVENT_TRIGGERS ; i++ )
	{
	  if ( ( ( (int) MapPositionOfMouse . x ) != AllEventTriggers [ i ] . Influ_Must_Be_At_Point . x ) )
	    continue;
	  if ( ( ( (int) MapPositionOfMouse . y ) != AllEventTriggers [ i ] . Influ_Must_Be_At_Point . y ) )
	    continue;
	  if ( Me [ player_num ] . pos . z != AllEventTriggers [ i ] . Influ_Must_Be_At_Level )
	    continue;
	  
	  // DebugPrintf ( -1000 , "\nSome trigger seems to be here..." );
	  
	  //--------------------
	  // Now we know, that the mouse is currently exactly over an event trigger.  The
	  // question to be answered still is whether this trigger also triggers a teleporter
	  // action or not and if yes, where the connection leads to...
	  //
	  action_number = GiveNumberToThisActionLabel ( AllEventTriggers [ i ] . TargetActionLabel ) ;

	  if ( AllTriggeredActions [ action_number ] . TeleportTargetLevel != (-1) )
	    {
	      sprintf ( ItemDescText , "To %s...." , curShip . AllLevels [ AllTriggeredActions [ action_number ] . TeleportTargetLevel ] -> Levelname ) ;
	    }
	}
    }	      
}; // void teleporter_square_below_mouse_cursor ( int player_num , char* ItemDescText )

/* ----------------------------------------------------------------------
 *
 * 
 * ---------------------------------------------------------------------- */
void
tux_wants_to_attack_now ( int player_num ) 
{

  if ( Me [ 0 ] . firewait > 0 ) return; 
  
  //--------------------
  // If the Tux has a weapon and this weapon requires some ammunition, then
  // we have to check for enough ammunition first...
  //
  if ( Me [ 0 ] . weapon_item . type >= 0 )
    {
      if ( ItemMap [ Me [ 0 ] . weapon_item . type ] . item_gun_use_ammunition )
	{
	  if ( ! CountItemtypeInInventory ( ItemMap [ Me [ 0 ] . weapon_item . type ] . item_gun_use_ammunition , 
					   0 ) )
	    {
	      No_Ammo_Sound( );

	      //--------------------
	      // So no ammunition... We should say so and return...
	      //
	      return ;
	    }
	  else
	    DeleteOneInventoryItemsOfType( ItemMap [ Me [ 0 ] . weapon_item . type ] . item_gun_use_ammunition , 0 );
	}
    }
  
  PerformTuxAttackRaw ( 0 ) ;      

}; // void tux_wants_to_attack_now ( int player_num ) 

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
CheckForJumpThresholds ( int player_num )
{
  int JumpTarget;
  float JumpThreshold;
  float SafetyBonus = 0.0 ;
  float JumpStartThreshold;

#define SHUFFLE_WHEN_CROSSING TRUE
#define LEVEL_JUMP_DEBUG 1

  //--------------------
  // First we check for the northern threshold
  //
  JumpThreshold = curShip.AllLevels [ Me [ player_num ] . pos . z ] -> jump_threshold_north ;
  JumpStartThreshold = JumpThreshold / 2.0 ;

  if ( Me [ player_num ] . pos . y < JumpStartThreshold )
    {

      JumpTarget = curShip.AllLevels [ Me [ player_num ] . pos . z ] -> jump_target_north ; 

      DebugPrintf ( LEVEL_JUMP_DEBUG , "\nJUMP TO THE NORTH CONSIDERED!!" );
      DebugPrintf ( LEVEL_JUMP_DEBUG , "\nJumpStartThreshold was: %f. " , JumpStartThreshold ); 

      if ( JumpTarget <= -1 ) return;

      Teleport ( JumpTarget , 
		 Me [ player_num ] . pos . x ,
		 curShip.AllLevels [ JumpTarget ] -> ylen - 0 - JumpStartThreshold - SafetyBonus ,
		 player_num , 
		 SHUFFLE_WHEN_CROSSING , FALSE ) ; 

      return;

    }

  //--------------------
  // Now we check for the southern threshold
  //
  JumpThreshold = curShip.AllLevels [ Me [ player_num ] . pos . z ] -> jump_threshold_south ;
  JumpStartThreshold = JumpThreshold / 2.0 ;

  if ( Me [ player_num ] . pos . y > 
       curShip.AllLevels [ Me [ player_num ] . pos . z ] -> ylen - 
       JumpStartThreshold ) 
    {

      JumpTarget = curShip.AllLevels [ Me [ player_num ] . pos . z ] -> jump_target_south ; 

      DebugPrintf ( LEVEL_JUMP_DEBUG , "\nJUMP TO THE SOUTH CONSIDERED!!" );
      DebugPrintf ( LEVEL_JUMP_DEBUG , "\nJumpStartThreshold was: %f. " , JumpStartThreshold ); 
      DebugPrintf ( LEVEL_JUMP_DEBUG , "\nCurrent Level Y-len: %d. " , curShip.AllLevels [ Me [ player_num ] . pos . z ] -> ylen );
      DebugPrintf ( LEVEL_JUMP_DEBUG , "\nCurrent Y-pos: %f. " , Me [ player_num ] . pos . y );

      if ( JumpTarget <= -1 ) return;

      Teleport ( JumpTarget , 
		 Me [ player_num ] . pos . x ,
		 0 + JumpStartThreshold + SafetyBonus ,
		 player_num , 
		 SHUFFLE_WHEN_CROSSING , FALSE ) ; 
      return;
    }

  //--------------------
  // Now we check for the eastern threshold
  //
  JumpThreshold = curShip.AllLevels [ Me [ player_num ] . pos . z ] -> jump_threshold_east ;
  JumpStartThreshold = JumpThreshold / 2.0 ;

  if ( Me [ player_num ] . pos . x > 
       curShip.AllLevels [ Me [ player_num ] . pos . z ] -> xlen - 
       JumpStartThreshold ) 
    {

      JumpTarget = curShip.AllLevels [ Me [ player_num ] . pos . z ] -> jump_target_east ; 

      DebugPrintf ( LEVEL_JUMP_DEBUG , "\nJUMP TO THE EAST CONSIDERED!!" );
      DebugPrintf ( LEVEL_JUMP_DEBUG , "\nJumpStartThreshold was: %f. " , JumpStartThreshold ); 
      DebugPrintf ( LEVEL_JUMP_DEBUG , "\nCurrent Level X-len: %d. " , curShip.AllLevels [ Me [ player_num ] . pos . z ] -> xlen );
      DebugPrintf ( LEVEL_JUMP_DEBUG , "\nCurrent X-pos: %f. " , Me [ player_num ] . pos . x );

      if ( JumpTarget <= -1 ) return;

      Teleport ( JumpTarget , 
		 0 + JumpStartThreshold + SafetyBonus ,
		 Me [ player_num ] . pos . y ,
		 player_num , 
		 SHUFFLE_WHEN_CROSSING , FALSE ) ; 
      return;
    }

  //--------------------
  // Now we check for the western threshold
  //
  JumpThreshold = curShip.AllLevels [ Me [ player_num ] . pos . z ] -> jump_threshold_west ;
  JumpStartThreshold = JumpThreshold / 2.0 ;

  if ( Me [ player_num ] . pos . x < JumpStartThreshold )
    {

      JumpTarget = curShip.AllLevels [ Me [ player_num ] . pos . z ] -> jump_target_west ; 

      DebugPrintf ( LEVEL_JUMP_DEBUG , "\nJUMP TO THE WEST CONSIDERED!!" );
      DebugPrintf ( LEVEL_JUMP_DEBUG , "\nJumpStartThreshold was: %f. " , JumpStartThreshold ); 

      if ( JumpTarget <= -1 ) return;

      Teleport ( JumpTarget , 
		 curShip.AllLevels [ JumpTarget ] -> xlen - 0 - JumpStartThreshold - SafetyBonus ,
		 Me [ player_num ] . pos . y ,
		 player_num , 
		 SHUFFLE_WHEN_CROSSING , FALSE ) ; 

      return;

    }

}; // void CheckForJumpThresholds ( int player_num )

/* ----------------------------------------------------------------------
 * This function initializes the influencers position history, which is
 * a ring buffer and is needed for throwing the influencer back (only one
 * or two positions would be needed for that) and for influencers followers
 * to be able to track the influencers path (10000 or so positions are used
 * for that, and that's why it is a ring buffer).
 * ---------------------------------------------------------------------- */
void 
InitInfluPositionHistory( int player_num )
{
  int RingPosition;

  for ( RingPosition = 0 ; RingPosition < MAX_INFLU_POSITION_HISTORY ; RingPosition ++ )
    {
      Me [ player_num ] . Position_History_Ring_Buffer [ RingPosition ] . x = Me [ player_num ] . pos . x ;
      Me [ player_num ] . Position_History_Ring_Buffer [ RingPosition ] . y = Me [ player_num ] . pos . y ;
      Me [ player_num ] . Position_History_Ring_Buffer [ RingPosition ] . z = Me [ player_num ] . pos . z ;
    }
} // void InitInfluPositionHistory( void )

float 
GetInfluPositionHistoryX( int HowLongPast )
{
  int RingPosition;

  RingPosition = Me [ 0 ] . current_zero_ring_index - HowLongPast;

  RingPosition += MAX_INFLU_POSITION_HISTORY; // We don't want any negative values, for safety

  RingPosition %= MAX_INFLU_POSITION_HISTORY; // We do MODULO for the Ring buffer length 

  return Me[0].Position_History_Ring_Buffer[ RingPosition ].x;
}

float 
GetInfluPositionHistoryY( int HowLongPast )
{
  int RingPosition;

  RingPosition = Me [ 0 ] . current_zero_ring_index - HowLongPast;

  RingPosition += MAX_INFLU_POSITION_HISTORY; // We don't want any negative values, for safety

  RingPosition %= MAX_INFLU_POSITION_HISTORY; // We do MODULO for the Ring buffer length 

  return Me[0].Position_History_Ring_Buffer[ RingPosition ].y;
}

float 
GetInfluPositionHistoryZ( int HowLongPast )
{
  int RingPosition;

  RingPosition = Me [ 0 ] . current_zero_ring_index - HowLongPast;

  RingPosition += MAX_INFLU_POSITION_HISTORY; // We don't want any negative values, for safety

  RingPosition %= MAX_INFLU_POSITION_HISTORY; // We do MODULO for the Ring buffer length 

  return Me[0].Position_History_Ring_Buffer[ RingPosition ].z;
}

/* ----------------------------------------------------------------------
 * This function should check if the Tux is still ok, i.e. if he is still
 * alive or if the death sequence should be initiated.
 * ---------------------------------------------------------------------- */
void 
CheckIfCharacterIsStillOk ( int player_num ) 
{

  //--------------------
  // This is something, that needs to be done ONLY for the 
  // one character of this client programm!!!!
  //
  if ( player_num != 0 ) return;

  //------------------------------
  // Now we check if the main character is really still ok.
  //
  if ( Me [ player_num ] .energy <= 0 )
    {
      if ( Me [ player_num ] .type != DRUID001 )
	{
	  Me [ player_num ] .type = DRUID001;
	  Me [ player_num ] .speed.x = 0;
	  Me [ player_num ] .speed.y = 0;
	  Me [ player_num ] .energy = PreTakeEnergy;
	  Me [ player_num ] .health = BLINKENERGY;
	  StartBlast ( Me [ player_num ] .pos.x, Me [ player_num ] .pos.y, Me [ player_num ] .pos.z , DRUIDBLAST );
	}
      else
	{
	  Me [ player_num ] .status = OUT;

	  if ( !ServerMode ) ThouArtDefeated ();
	  
	  DebugPrintf (2, "\nvoid CheckIfCharacterIsStillOk( int player_num ):  Alternate end of function reached.");
	  return;
	}
    }

}; // void CheckIfCharacterIsStillOk ( int player_num ) 

/* ----------------------------------------------------------------------
 * Even the Tux must not leave the map!  A sanity check is done here...
 * ---------------------------------------------------------------------- */
void
CheckForTuxOutOfMap ( player_num )
{
  Level MoveLevel = curShip.AllLevels[ Me [ player_num ] . pos . z ] ;

  //--------------------
  // Now perhaps the influencer is out of bounds, i.e. outside of the map.
  // This would cause a segfault immediately afterwards, when checking
  // for the current map tile to be conveyor or not.  Therefore we add some
  // extra security against segfaults and increased diagnosis functionality
  // here.
  //
  if ( ( (int) rintf( Me [ player_num ] . pos.y ) >= MoveLevel->ylen ) ||
       ( (int) rintf( Me [ player_num ] . pos.x ) >= MoveLevel->xlen ) ||
       ( (int) rintf( Me [ player_num ] . pos.y ) <  0              ) ||
       ( (int) rintf( Me [ player_num ] . pos.x ) <  0              ) )
    {
      fprintf ( stderr, "\n\nplayer_num: '%d'.\n" , player_num );
      fprintf ( stderr, "\n\nplayer_num's last position: X=%f, Y=%f, Z=%d.\n" , 
		Me [ player_num ] . pos . x ,
		Me [ player_num ] . pos . y ,
		Me [ player_num ] . pos . z );
      GiveStandardErrorMessage ( "CheckForTuxOutOfMap(...)" , "\
A player's Tux was found outside the map in MoveInfluence.\n\
This indicates either a bug in the Freedroid RPG code or\n\
a bug in the currently used map system of Freedroid RPG.",
				 PLEASE_INFORM, IS_FATAL );
    }
}; // void CheckForTuxOutOfMap ( player_num )

/* ----------------------------------------------------------------------
 * If an enemy was specified as the mouse move target, this enemy will
 * maybe move here and there.  But this means that also the mouse move
 * target of the influencer must adapt, which is done in this function.
 * ---------------------------------------------------------------------- */
void
UpdateMouseMoveTargetAccordingToEnemy ( int player_num )
{
  moderately_finepoint RemainingWay;
  float RemainingWayLength;

  //--------------------
  // If the mouse move target got destroyed, there's no reason
  // to move toward it any more.  The use can request a new move
  // if that should be done.
  //
  if ( ( AllEnemys [ Me [ player_num ] . mouse_move_target_is_enemy ] . Status == OUT ) ||
       ( AllEnemys [ Me [ player_num ] . mouse_move_target_is_enemy ] . pos . z != 
	 Me [ player_num ] . pos . z ) )
    {
      Me [ player_num ] . mouse_move_target_is_enemy = ( -1 ) ;
      Me [ player_num ] . mouse_move_target . x = ( -1 ) ;
      Me [ player_num ] . mouse_move_target . y = ( -1 ) ;
      Me [ player_num ] . mouse_move_target . z = ( -1 ) ;
    }
  else
    {
      //--------------------
      // But now that there is a an enemy selected as mouse move target, the Tux 
      // use the coordiantes of this enemy (or a position directly in front of it)
      // as the new mouse move target coordinates.
      //
      Me [ player_num ] . mouse_move_target . x = 
	AllEnemys [ Me [ player_num ] . mouse_move_target_is_enemy ] . pos . x ;
      Me [ player_num ] . mouse_move_target . y = 
	AllEnemys [ Me [ player_num ] . mouse_move_target_is_enemy ] . pos . y ;
      Me [ player_num ] . mouse_move_target . z = 
	AllEnemys [ Me [ player_num ] . mouse_move_target_is_enemy ] . pos . z ;
      
      RemainingWay . x = Me [ player_num ] . pos . x - Me [ player_num ] . mouse_move_target . x ;
      RemainingWay . y = Me [ player_num ] . pos . y - Me [ player_num ] . mouse_move_target . y ;
      
      RemainingWayLength = sqrtf ( ( RemainingWay . x ) * ( RemainingWay . x ) +
				   ( RemainingWay . y ) * ( RemainingWay . y ) ) ;
      
      RemainingWay . x = ( RemainingWay . x / RemainingWayLength ) * 
	( RemainingWayLength - BEST_MELEE_DISTANCE ) ;
      RemainingWay . y = ( RemainingWay . y / RemainingWayLength ) * 
	( RemainingWayLength - BEST_MELEE_DISTANCE ) ;
      
      Me [ player_num ] . mouse_move_target . x = Me [ player_num ] . pos . x - RemainingWay . x ;
      Me [ player_num ] . mouse_move_target . y = Me [ player_num ] . pos . y - RemainingWay . y ;

      //--------------------
      // Now that the mouse move target has implicitly affected the recursive
      // waypoint stuff, we might need to establish a new waypoint route, so we
      // do this here as well...
      //
      // (redundancy will be caught inside that function anyway...)
      //
      set_up_intermediate_course_for_tux ( player_num ) ;

      // DebugPrintf ( 0 , "\nRemaining way: %f %f." , RemainingWay . x , RemainingWay . y );

    }
}; // void UpdateMouseMoveTargetAccoringToEnemy ( int player_num )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
round_tux_position ( int player_num )
{
  // Me [ player_num ] . pos . x = ( (int) ( Me [ player_num ] . pos . x * 1024.0 ) ) / 1024.0 ;
  // Me [ player_num ] . pos . y = ( (int) ( Me [ player_num ] . pos . y * 1024.0 ) ) / 1024.0 ;
  // Me [ player_num ] . pos . x = ( (int) ( Me [ player_num ] . pos . x * 5000.0 * LIGHT_RADIUS_CHUNK_SIZE) ) / ( 5000.0 * LIGHT_RADIUS_CHUNK_SIZE ) ;
  // Me [ player_num ] . pos . y = ( (int) ( Me [ player_num ] . pos . y * 5000.0 * LIGHT_RADIUS_CHUNK_SIZE) ) / ( 5000.0 * LIGHT_RADIUS_CHUNK_SIZE ) ;

  // Me [ player_num ] . pos . x = ( (int) ( Me [ player_num ] . pos . x * ( iso_floor_tile_width / 10.0 * iso_floor_tile_height / 10.0 ) ) ) / ( iso_floor_tile_width / 10.0 * iso_floor_tile_height / 10.0 ) ;
  // Me [ player_num ] . pos . y = ( (int) ( Me [ player_num ] . pos . y * ( iso_floor_tile_width / 10.0 * iso_floor_tile_height / 10.0 ) ) ) / ( iso_floor_tile_width / 10.0 * iso_floor_tile_height / 10.0 ) ;

  // Me [ player_num ] . pos . x = ( (int) ( Me [ player_num ] . pos . x * 24.0 ) ) / 24.0 ;
  // Me [ player_num ] . pos . y = ( (int) ( Me [ player_num ] . pos . y * 24.0 ) ) / 24.0 ;

  // DebugPrintf ( 0 , "\nwidth: %d 

};

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
move_tux_out_of_obstacle ( int player_num , obstacle* ThisObstacle )
{
  moderately_finepoint out_vect;
  float v_len;

  //--------------------
  // We find the vector pointing outwards from the obstacle center,
  // so we know in which direction to move the Tux
  //
  out_vect . x = Me [ player_num ] . pos . x - ThisObstacle -> pos . x ;
  out_vect . y = Me [ player_num ] . pos . y - ThisObstacle -> pos . y ;

  //--------------------
  // Determining vector length and taking precautions against
  // division by zero
  //
  v_len = vect_len ( out_vect );
  if ( v_len < 0.01 ) v_len = 0.01;
  
  //--------------------
  // We norm the outward vector length to 1.0
  //
  out_vect . x = out_vect . x / v_len ;
  out_vect . y = out_vect . y / v_len ;

  //--------------------
  // We set the outwards speed to 1/2
  //
  out_vect . x *= 0.5 ;
  out_vect . y *= 0.5 ;

  //--------------------
  // Now we can fix the Tux position.  Of course this again
  // has to take into account the current framerate...
  //
  Me [ player_num ] . pos . x += out_vect . x * Frame_Time();
  Me [ player_num ] . pos . y += out_vect . y * Frame_Time();

}; // void move_tux_out_of_obstacle ( player_num , & ( ThisLevel -> ObstacleList [ obst_index ] ) );

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
move_tux_out_of_obstacles_on_square ( int x , int y , int player_num )
{
  Level ThisLevel = curShip . AllLevels [ Me [ player_num ] . pos . z ] ;
  int obst_index;
  int i;
  
  for ( i = 0 ; i < MAX_OBSTACLES_GLUED_TO_ONE_MAP_TILE ; i ++ )
    {
      if ( ThisLevel -> map [ y ] [ x ] . obstacles_glued_to_here [ i ] == (-1) )
	{
	  DebugPrintf ( -3 , "\nFALLBACK:  Last obstacle on correction-square reached...breaking..." );
	  break;
	}
      obst_index = ThisLevel -> map [ y ] [ x ] . obstacles_glued_to_here [ i ] ;

      if ( position_collides_with_this_obstacle ( Me [ player_num ] . pos . x , 
						  Me [ player_num ] . pos . y , 
						  & ( ThisLevel -> obstacle_list [ obst_index ] ) ) )
	{
	  DebugPrintf ( -3 , "\nFALLBACK:  One offending obstacle found.  Fixing Tux position..." );
	  move_tux_out_of_obstacle ( player_num , & ( ThisLevel -> obstacle_list [ obst_index ] ) );
	}
    }
  
}; // void move_tux_out_of_obstacles_on_square ( int x , int y , int player_num )

/* ----------------------------------------------------------------------
 * Self-explanatory.
 * This function also takes into account any conveyor belts the Tux might
 * be standing on.
 * ---------------------------------------------------------------------- */
void
MoveTuxAccordingToHisSpeed ( int player_num )
{
  float planned_step_x;
  float planned_step_y;
  int start_x, end_x, start_y, end_y;
  int x , y ;
  Level ThisLevel;

  //--------------------
  // Now we move influence according to current speed.  But there has been a problem
  // reported from people, that the influencer would (*very* rarely) jump throught walls
  // and even out of the ship.  This has *never* occured on my fast machine.  Therefore
  // I assume that the problem is related to sometimes very low framerates on these machines.
  // So, we do a sanity check not to make steps too big.
  //
  // So what do we do?  We allow a maximum step of exactly that, what the 302 (with a speed
  // of 7) could get when the framerate is as low as 20 FPS.  This should be sufficient to
  // prevent the influencer from *ever* leaving the ship.  I hope this really does work.
  // The definition of that speed is made in MAXIMAL_STEP_SIZE at the top of this file.
  //
  // And on machines with FPS << 20, it will certainly alter the game behaviour, so people
  // should really start using a pentium or better machine.
  //
  // NOTE:  PLEASE LEAVE THE .0 in the code or gcc will round it down to 0 like an integer.
  //
  planned_step_x = Me [ player_num ] . speed . x * Frame_Time ();
  planned_step_y = Me [ player_num ] . speed . y * Frame_Time ();

  //--------------------
  // Maybe the Tux is just executing a weapon strike.  In this case, there should
  // be no movement at all, so in this case we'll just not go anywhere...
  //
  if ( Me [ player_num ] . weapon_swing_time > 0 )
    {
      planned_step_x = 0 ;
      planned_step_y = 0 ;
    }

  //--------------------
  // Now we can make the actual move, AND WE DO SO REGARDLESS
  // OF ANY BLOCKING BY OBSTACLES, SINCE THE POSSIBILITY OF GOING
  // THROUGH WALLS HAS BEEN RULES OUT BY THE PATHFINDING FUNCTION
  // ANYWAY.
  //
  Me [ player_num ] . pos . x += planned_step_x;
  Me [ player_num ] . pos . y += planned_step_y;

  //--------------------
  // If the Tux got stuck, i.e. if he got no speed at all and still is 
  // currently not in a 'passable' position, the fallback handling needs
  // to be applied to move the Tux out of the offending obstacle (i.e. 
  // simply away from the offending obstacles center)
  //
  if ( ( fabsf ( Me [ player_num ] . speed . x ) < 0.1 ) &&
       ( fabsf ( Me [ player_num ] . speed . y ) < 0.1 ) )
    {
      //--------------------
      // So there is no speed, so we check for passability...
      //
      if ( ! IsPassable ( Me [ player_num ] . pos . x , Me [ player_num ] . pos . y , Me [ player_num ] . pos . z ) )
	{
	  //--------------------
	  // Now it's time to launch the stuck-fallback handling...
	  //
	  DebugPrintf ( -3 , "\nTux looks stuck...ENABLING FALLBACK just for this frame..." );

	  start_x = (int) Me [ player_num ] . pos . x - 2 ;
	  start_y = (int) Me [ player_num ] . pos . y - 2 ;
	  end_x = start_x + 4 ;
	  end_y = start_y + 4 ;

	  ThisLevel = curShip . AllLevels [ Me [ player_num ] . pos . z ] ;

	  if ( start_x < 0 ) start_x = 0 ;
	  if ( start_y < 0 ) start_y = 0 ;
	  if ( end_x >= ThisLevel -> xlen ) end_x = ThisLevel -> xlen - 1 ;
	  if ( end_y >= ThisLevel -> ylen ) end_y = ThisLevel -> ylen - 1 ;

	  //--------------------
	  //
	  for ( x = start_x ; x < end_x ; x ++ )
	    {
	      for ( y = start_y ; y < end_y ; y ++ )
		{
		  if ( position_collides_with_obstacles_on_square ( Me [ player_num ] . pos . x , 
								    Me [ player_num ] . pos . y , 
								    x , y , ThisLevel ) )
		    {
		      DebugPrintf ( -3, "\nFALLBACK: It seems like we got the offending square.  Starting check..." );
		      
		      move_tux_out_of_obstacles_on_square ( x , y , player_num );
		    }
		}
	    }
	}
    }

  //--------------------
  // Even the Tux must not leave the map!  A sanity check is done
  // here...
  //
  CheckForTuxOutOfMap ( player_num );

  round_tux_position ( player_num );

}; // void MoveTuxAccordingToHisSpeed ( player_num )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
update_intermediate_tux_waypoints ( int player_num )
{
  if ( tux_can_walk_this_line ( player_num , Me [ 0 ] . pos . x , Me [ 0 ] . pos . y , 
				Me [ 0 ] . mouse_move_target . x , Me [ 0 ] . mouse_move_target . y ) )
    {
      Me [ player_num ] . next_intermediate_point [ 0 ] . x =
	Me [ player_num ] . mouse_move_target . x ;
      Me [ player_num ] . next_intermediate_point [ 0 ] . y =
	Me [ player_num ] . mouse_move_target . y ;

      DebugPrintf ( DEBUG_TUX_PATHFINDING , "\nThis point can be reached directly, so no need to set up waypoints..." );
    }
  else
    {
      // find_new_intermediate_point ( player_num );
    }

}; // void update_intermediate_tux_waypoints ( int player_num )

/* ----------------------------------------------------------------------
 * This function contains the final dumb movement code, that, without
 * any checks and any refinement, just moves the tux thowards the given
 * target position.
 *
 * The return value indicates, if the target has been sufficiently 
 * approximated (TRUE) already or not (FALSE) .
 *
 * ---------------------------------------------------------------------- */
int
move_tux_thowards_raw_position ( int player_num , float x , float y )
{
  moderately_finepoint RemainingWay;
  moderately_finepoint planned_step;
  float length;

  //--------------------
  // We do not move any players, who's statuses are 'OUT'.
  //
  if ( Me [ player_num ] . status == OUT ) return ( FALSE ) ;

  //--------------------
  // Now for a change, we try to implement some movement code,
  // that doesn't rely on any acceleration any more, but just
  // gives full speed immediately
  //

  RemainingWay . x = - Me [ player_num ] . pos . x + x ;
  RemainingWay . y = - Me [ player_num ] . pos . y + y ;

  length = vect_len ( RemainingWay );

  //--------------------
  // Maybe the remaining way is VERY!! small!  Then we must not do
  // a division at all.  We also need not do any movement, so the
  // speed can be eliminated and we're done here.
  //
  if ( length < 0.05 )
    {
      Me [ player_num ] . speed . x = 0 ;
      Me [ player_num ] . speed . y = 0 ;
      return ( TRUE ) ;
    }

  //--------------------
  // Now depending on whether the running key is pressed or not,
  // we have the Tux go on running speed or on walking speed.
  //
  if ( Me [ player_num ] . running_power <= 0 ) Me [ player_num ] . running_must_rest = TRUE ;
  if ( LeftCtrlPressed() && ( ! Me [ player_num ] . running_must_rest ) )
    { 
      planned_step . x = RemainingWay . x * TUX_RUNNING_SPEED / length ;
      planned_step . y = RemainingWay . y * TUX_RUNNING_SPEED / length ;
      // DebugPrintf ( -2 , "\nNow running..." );
    }
  else
    {
      planned_step . x = RemainingWay . x * TUX_WALKING_SPEED / length ;
      planned_step . y = RemainingWay . y * TUX_WALKING_SPEED / length ;
      // DebugPrintf ( -2 , "\nNow walking..." );
    }

  //--------------------
  // Now that the speed is set, we can start to make the step
  //
  Me [ player_num ] . speed . x = planned_step . x ;
  Me [ player_num ] . speed . y = planned_step . y ;

  //--------------------
  // If speed is so high, that we might step over the target,
  // we reduce the speed.
  //
  if ( ( Frame_Time() > 0.001 ) && ( length > 0.05 ) )
    {
      if ( fabsf ( planned_step . x * Frame_Time() ) >= fabsf ( RemainingWay .x  ) )
	Me [ player_num ] . speed . x = RemainingWay . x / Frame_Time() ;
      if ( fabsf ( planned_step . y * Frame_Time() ) >= fabsf ( RemainingWay .y  ) )
	Me [ player_num ] . speed . y = RemainingWay . y / Frame_Time() ;
    }

  //--------------------
  // In case we have reached our target, we can remove this mouse_move_target again,
  // but also if we have been thrown onto a different level, we cancel our current
  // mouse move target...
  //
  if ( ( ( fabsf ( RemainingWay.y ) <= DISTANCE_TOLERANCE ) && 
	 ( fabsf ( RemainingWay.x ) <= DISTANCE_TOLERANCE )     ) ||
       ( Me [ player_num ] . mouse_move_target . z != Me [ player_num ] . pos . z ) )
    {
      return ( TRUE );
    }

  return ( FALSE );

}; // int move_tux_thowards_raw_position ( int player_num , float x , float y )

/* ----------------------------------------------------------------------
 * This function is supposed to find out if a given line on the current
 * map of this player is walkable for the tux or not.
 *
 * ---------------------------------------------------------------------- */
int
tux_can_walk_this_line ( int player_num , float x1, float y1 , float x2 , float y2 )
{
  global_ignore_doors_for_collisions_flag = TRUE ;
  if ( DirectLineWalkable ( x1 , y1 , x2 , y2 , Me [ player_num ] . pos . z ) )
    return ( TRUE );
  else
    return ( FALSE ); 
  global_ignore_doors_for_collisions_flag = FALSE ;
}; // int tux_can_walk_this_line ( float x1, float y1 , float x2 , float y2 )

/* ----------------------------------------------------------------------
 * After a course for the Tux has been set up, the Tux can start to 
 * proceed thowards his target.  However, the unmodified recursive course
 * is often a bit awakward and goes back and forth a lot.
 * 
 * Therefore it will be a good idea to streamline the freshly set up
 * course first, once and for all, before the tux is finally set in
 * motion.
 *
 * ---------------------------------------------------------------------- */
void
streamline_tux_intermediate_course ( int player_num )
{
  int start_index ;
  int last_index = -10;
  int scan_index;
  int cut_away;

  DebugPrintf ( DEBUG_TUX_PATHFINDING , "\nOPTIMISATION --> streamline_tux_intermediate_course: starting..." );

  //--------------------
  // We process each index position of the course, starting with the point
  // where the tux will be starting.
  //
  for ( start_index = 0 ; start_index < MAX_INTERMEDIATE_WAYPOINTS_FOR_TUX ; start_index ++ )
    {
      //--------------------
      // If the end of the course is reached in the outer loop, then we're done indeed 
      // with the streamlining process and therefore can go home now...
      //
      if ( Me [ player_num ] . next_intermediate_point [ start_index ] . x == (-1) )
	break;

      //--------------------
      // Start of inner streamlining loop:
      // We eliminate every point from here on up to the last point in the
      // course, that can still be reached from here.
      //
      last_index = (-1) ;
      for ( scan_index = start_index + 1 ; scan_index < MAX_INTERMEDIATE_WAYPOINTS_FOR_TUX ; scan_index ++ )
	{

	  //--------------------
	  // If we've reached the end of the course this way, then we know how much
	  // we can cut away and can quit the inner loop here.
	  //
	  if ( Me [ player_num ] . next_intermediate_point [ scan_index ] . x == (-1) )
	    break;

	  //--------------------
	  // Otherwise we check if maybe this is (another) reachable intermediate point (AGAIN?)
	  //
	  if ( tux_can_walk_this_line ( player_num , 
					Me [ player_num ] . next_intermediate_point [ start_index ] . x ,
					Me [ player_num ] . next_intermediate_point [ start_index ] . y ,
					Me [ player_num ] . next_intermediate_point [ scan_index ] . x ,
					Me [ player_num ] . next_intermediate_point [ scan_index ] . y ) )
	    {
	      if ( CheckIfWayIsFreeOfDroidsWithoutTuxchecking ( Me [ player_num ] . next_intermediate_point [ start_index ] . x ,
								Me [ player_num ] . next_intermediate_point [ start_index ] . y ,
								Me [ player_num ] . next_intermediate_point [ scan_index ] . x ,
								Me [ player_num ] . next_intermediate_point [ scan_index ] . y , 
								Me [ 0 ] . pos . z , ( enemy* ) NULL ) )
		{
		  last_index = scan_index ;
		}
	    }
	}

      //--------------------
      // Maybe the result of the scan indicated, that there is nothing to cut away at this
      // point.  Then we must contine right after this point.
      //
      if ( last_index == (-1) ) continue;
      // if ( last_index == start_index + 1 ) continue; // nothing to cut away...
      
      //--------------------
      // Now we know how much to cut away.  So we'll do it.
      //
      for ( cut_away = 0 ; cut_away < MAX_INTERMEDIATE_WAYPOINTS_FOR_TUX ; cut_away ++ )
	{

	  if ( last_index + cut_away < MAX_INTERMEDIATE_WAYPOINTS_FOR_TUX )
	    {
	      Me [ player_num ] . next_intermediate_point [ start_index + 1 + cut_away ] . x =
		Me [ player_num ] . next_intermediate_point [ last_index + 0 + cut_away ] . x ;
	      Me [ player_num ] . next_intermediate_point [ start_index + 1 + cut_away ] . y =
		Me [ player_num ] . next_intermediate_point [ last_index + 0 + cut_away ] . y ;
	    }
	  else
	    {
	      Me [ player_num ] . next_intermediate_point [ start_index + 1 + cut_away ] . x = (-1) ;
	      Me [ player_num ] . next_intermediate_point [ start_index + 1 + cut_away ] . y = (-1) ;
	    }
	}
    }

  //--------------------
  // At this point the waypoint history is fairly good.  However, it might
  // be, that the very first waypoint entry (index 0) is not nescessary and
  // did not get overwritten in the process above, because the original course
  // setup function does not usually include the current position of the Tux
  // as the very first entry.
  //
  // Therefore we do some extra optimisation check here for this special case...
  //
  if ( tux_can_walk_this_line ( player_num , 
				Me [ player_num ] . pos . x ,
				Me [ player_num ] . pos . y ,
				Me [ player_num ] . next_intermediate_point [ 1 ] . x ,
				Me [ player_num ] . next_intermediate_point [ 1 ] . y ) )
    {
      if ( CheckIfWayIsFreeOfDroidsWithoutTuxchecking ( Me [ player_num ] . pos . x ,
							Me [ player_num ] . pos . y ,
							Me [ player_num ] . next_intermediate_point [ 1 ] . x ,
							Me [ player_num ] . next_intermediate_point [ 1 ] . y , 
							Me [ 0 ] . pos . z , (enemy*) NULL ) )
	{
	  DebugPrintf ( DEBUG_TUX_PATHFINDING , "\nVERY FIRST INTERMEDIATE POINT CUT MANUALLY!!!!" );
	  for ( cut_away = 1 ; cut_away < MAX_INTERMEDIATE_WAYPOINTS_FOR_TUX ; cut_away ++ )
	    {
	      Me [ player_num ] . next_intermediate_point [ cut_away - 1 ] . x =
		Me [ player_num ] . next_intermediate_point [ cut_away ] . x ;
	      Me [ player_num ] . next_intermediate_point [ cut_away - 1 ] . y =
		Me [ player_num ] . next_intermediate_point [ cut_away ] . y ;
	    }
	}
    }
  else
    {
      DebugPrintf ( DEBUG_TUX_PATHFINDING , "\nOPTIMISATION --> streamline_tux_intermediate_course: no final shortcut." );

    }

}; // void streamline_tux_intermediate_course ( player_num )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
int
recursive_find_walkable_point ( float x1 , float y1 , float x2 , float y2 , int recursion_depth ) 
{
  moderately_finepoint ordered_moves[4];
  int i;

#define MAX_RECUSION_DEPTH 50

  //--------------------
  // At first we mark the current position as processed...
  //
  recursion_grid [ (int) x1 ] [ (int) y1 ] = TILE_IS_PROCESSED ;
  
  //--------------------
  // Maybe the recursion is too deep already.  Then we just return and report
  // failure.
  //
  if ( recursion_depth > MAX_RECUSION_DEPTH )
    return ( FALSE );

  //--------------------
  // If we can reach the final destination from here, then there is no need to
  // go any further, but instead we select the current position as the preliminary
  // walkable target for the Tux.
  //
  if ( ( tux_can_walk_this_line ( 0 , x1, y1 , x2 , y2 ) ) &&
       ( CheckIfWayIsFreeOfDroidsWithoutTuxchecking ( x1 , y1 , x2 , y2 , Me [ 0 ] . pos . z , (enemy*) NULL ) ) ) 
    {
      //--------------------
      // If the current position is still directly reachable for the Tux, we set it
      // as our target and return
      //
      DebugPrintf ( DEBUG_TUX_PATHFINDING , "\nRecursion has found the final target! --> start to set up course..." );
      Me [ 0 ] . next_intermediate_point [ 0 ] . x = x2 ;
      Me [ 0 ] . next_intermediate_point [ 0 ] . y = y2 ;
      Me [ 0 ] . next_intermediate_point [ 1 ] . x = x1 ;
      Me [ 0 ] . next_intermediate_point [ 1 ] . y = y1 ;
      next_index_to_set_up = 2 ;
      return ( TRUE ) ;
    }

  //--------------------
  // So at this point we know, that the current position is not one from where
  // we would be able to reach our goal.
  //
  // Therefore we will try other positions that might bring us more luck, but 
  // we only try such positions, as we can reach from here...
  //
  // And also we will try the 'more promising' directions before the 'less promising'
  // ones...
  //
  if ( fabsf ( x1-x2 ) >= fabsf ( y1-y2 ) )
    {
      //--------------------
      // More prority on x move into the right direction, least
      // priority on x move into the wrong direction.
      //
      if ( x1 <= x2 )
	{
	  ordered_moves [ 0 ] . x =  1.0 ;
	  ordered_moves [ 0 ] . y =  0.0 ;
	  ordered_moves [ 3 ] . x = -1.0 ;
	  ordered_moves [ 3 ] . y =  0.0 ;
	}
      else
	{
	  ordered_moves [ 3 ] . x =  1.0 ;
	  ordered_moves [ 3 ] . y =  0.0 ;
	  ordered_moves [ 0 ] . x = -1.0 ;
	  ordered_moves [ 0 ] . y =  0.0 ;
	}
      if ( y1 <= y2 )
	{
	  ordered_moves [ 2 ] . x =  0.0 ;
	  ordered_moves [ 2 ] . y =  1.0 ;
	  ordered_moves [ 1 ] . x =  0.0 ;
	  ordered_moves [ 1 ] . y = -1.0 ;
	}
      else
	{
	  ordered_moves [ 1 ] . x =  0.0 ;
	  ordered_moves [ 1 ] . y =  1.0 ;
	  ordered_moves [ 2 ] . x =  0.0 ;
	  ordered_moves [ 2 ] . y = -1.0 ;
	}
    }
  else
    {
      //--------------------
      // More prority on x move into the right direction, least
      // priority on x move into the wrong direction.
      //
      if ( x1 <= x2 )
	{
	  ordered_moves [ 1 ] . x =  1.0 ;
	  ordered_moves [ 1 ] . y =  0.0 ;
	  ordered_moves [ 2 ] . x = -1.0 ;
	  ordered_moves [ 2 ] . y =  0.0 ;
	}
      else
	{
	  ordered_moves [ 2 ] . x =  1.0 ;
	  ordered_moves [ 2 ] . y =  0.0 ;
	  ordered_moves [ 1 ] . x = -1.0 ;
	  ordered_moves [ 1 ] . y =  0.0 ;
	}
      if ( y1 <= y2 )
	{
	  ordered_moves [ 0 ] . x =  0.0 ;
	  ordered_moves [ 0 ] . y =  1.0 ;
	  ordered_moves [ 3 ] . x =  0.0 ;
	  ordered_moves [ 3 ] . y = -1.0 ;
	}
      else
	{
	  ordered_moves [ 3 ] . x =  0.0 ;
	  ordered_moves [ 3 ] . y =  1.0 ;
	  ordered_moves [ 0 ] . x =  0.0 ;
	  ordered_moves [ 0 ] . y = -1.0 ;
	}
    }


  //--------------------
  // Now that we have set up our walk preferences, we can start to try out the directions we have...
  //

  for ( i = 0 ; i < 4 ; i ++ )
    {
      if ( ( recursion_grid 
	     [ (int) ( x1 + ordered_moves [ i ] . x ) ] 
	     [ (int) ( y1 + ordered_moves [ i ] . y ) ] == TILE_IS_UNPROCESSED ) &&
	   ( tux_can_walk_this_line ( 0 , x1, y1 , 
				      x1 + ordered_moves [ i ] . x , 
				      y1 + ordered_moves [ i ] . y ) ) )
	{
	  if ( ( CheckIfWayIsFreeOfDroidsWithoutTuxchecking ( x1 , y1 , 
							  x1 + ordered_moves [ i ] . x , 
							  y1 + ordered_moves [ i ] . y , 
							  Me [ 0 ] . pos . z , (enemy*) NULL ) ) )
	    {
	  
	      last_sight_contact . x = x1 ;
	      last_sight_contact . y = y1 ;
	  
	      if ( recursive_find_walkable_point ( rintf ( x1 + ordered_moves [ i ] . x + 0.5 ) - 0.5 , 
						   rintf ( y1 + ordered_moves [ i ] . y + 0.5 ) - 0.5 , x2 , y2 , recursion_depth + 1 ) )
		{
	      
		  //--------------------
		  // If there is still sight contact to the waypoint closer to the target, we just set this
		  // waypoint.
		  // Otherwise we set THE NEXT WAYPOINT.
		  //
		  Me [ 0 ] . next_intermediate_point [ next_index_to_set_up ] . x = x1 + ordered_moves [ i ] . x ;
		  Me [ 0 ] . next_intermediate_point [ next_index_to_set_up ] . y = y1 + ordered_moves [ i ] . y ;
		  
		  DebugPrintf ( DEBUG_TUX_PATHFINDING , "\nAdded another Tux waypoint entry..." );
		  next_index_to_set_up++;
		  
		  if ( next_index_to_set_up >= MAX_INTERMEDIATE_WAYPOINTS_FOR_TUX )
		    {
		      DebugPrintf ( DEBUG_TUX_PATHFINDING , "\nERROR!  Ran out of tux waypoints even with solutionfound!" );
		      clear_out_intermediate_points ( 0 ) ;
		      return ( FALSE );
		    }
		  
		  return ( TRUE ) ;
		  
		}
	    }
	}
    }


  // DebugPrintf ( DEBUG_TUX_PATHFINDING , "\nBad luck in all 4 directions!" );
  bad_luck_in_4_directions_counter ++ ;

  //--------------------
  // Here we know, that we didn't have any success finding some possible point...
  //
  return ( FALSE );

}; // int recursive_find_walkable_point ( float x1 , float y1 , float x2 , float y2 ) 

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
clear_out_intermediate_points ( int player_num )
{
  int i;

  //--------------------
  // We clear out the waypoint list for the Tux and initialize the 
  // very first entry.
  //
  for ( i = 0 ; i < MAX_INTERMEDIATE_WAYPOINTS_FOR_TUX ; i ++ )
    {
      Me [ player_num ] . next_intermediate_point [ i ] . x = (-1) ;
      Me [ player_num ] . next_intermediate_point [ i ] . y = (-1) ;
    }
  Me [ player_num ] . next_intermediate_point [ 0 ] . x = Me [ 0 ] . pos . x ;
  Me [ player_num ] . next_intermediate_point [ 0 ] . y = Me [ 0 ] . pos . y ;

}; // void clear_out_intermediate_points ( int player_num )

/* ----------------------------------------------------------------------
 * In case that the Tux cannot walk the direct line from his current 
 * position to the mouse move target, we must set up an alternative target
 * that should sooner or later lead to a news position that is finally
 * so good that the Tux has better chances of walking to the final mouse
 * move target.
 * ---------------------------------------------------------------------- */
void
set_up_intermediate_course_for_tux ( int player_num )
{
  int i;
  moderately_finepoint tmp;
  static moderately_finepoint last_given_course_target = { -2 , -2 };

  //--------------------
  // For the protocol, we want to know how many cases of the cursion
  // ending unresolved have occured.  Therefore we initialize a counter
  // here and give out the result later after the recursion.
  //
  bad_luck_in_4_directions_counter = 0;

  //--------------------
  // For optimisation purposes, we'll not do anything unless a new target
  // has been given.
  //
  if ( ( fabsf ( Me [ player_num ] . mouse_move_target . x - last_given_course_target . x ) < 0.3 ) &&
       ( fabsf ( Me [ player_num ] . mouse_move_target . y - last_given_course_target . y ) < 0.3 ) )
    {
      DebugPrintf ( DEBUG_TUX_PATHFINDING , "\nSKIPPING RECURSION BECAUSE OF REDUNDANCY!" );
      return;
    }

  //--------------------
  // If the target position cannot be reached at all, because of being inside an obstacle
  // for example, then we know what to do:  Set up one waypoint to the target and that's it.
  //
  if ( ! IsPassable ( Me [ player_num ] . mouse_move_target . x ,
		    Me [ player_num ] . mouse_move_target . y ,
		    Me [ player_num ] . mouse_move_target . z ) )
    {
      DebugPrintf ( DEBUG_TUX_PATHFINDING , "\nSKIPPING RECURSION BECAUSE OF UNREACHABLENESS!" );
      return;
    }

  //--------------------
  // If the target position cannot be reached at all, because of being inside an obstacle
  // for example, then we know what to do:  Set up one waypoint to the target and that's it.
  //
  if ( ! IsPassable ( Me [ player_num ] . pos . x ,
		      Me [ player_num ] . pos . y ,
		      Me [ player_num ] . pos . z ) )
    {
      DebugPrintf ( 0 , "\nSkipping recursion because of passability reasons from current position..." );
      return;
    }

  //--------------------
  // By default, we clear out any combo action for the target position.
  // The calling function must set the combo action it has in mind.
  //
  // Me [ player_num ] . mouse_move_target_is_enemy = ( -1 ) ;
  Me [ player_num ] . mouse_move_target_combo_action_type = NO_COMBO_ACTION_SET ;
  Me [ player_num ] . mouse_move_target_combo_action_parameter = -1 ;

  //--------------------
  // We give out a well visible debug message, so that the heavy process
  // can easily be seen as redundant if that's really the case.
  //
  DebugPrintf ( DEBUG_TUX_PATHFINDING , 
"\n\n\n\n\
*******************************************\n\
*** Setting up new intermediate course using recursion...\n\
*******************************************\n" );

  //--------------------
  // First we clear out the position grid and initialize the target
  // point, which will be the result of the recursion.
  //
  memset ( & ( recursion_grid [ 0 ] ) , TILE_IS_UNPROCESSED , sizeof ( char ) * MAX_MAP_LINES * MAX_MAP_LINES );
  
  clear_out_intermediate_points ( player_num );

  next_index_to_set_up = 0 ;

  recursive_find_walkable_point ( Me [ player_num ] . pos . x , Me [ player_num ] . pos . y , Me [ player_num ] . mouse_move_target . x , Me [ player_num ] . mouse_move_target . y , 0 ) ;

  //--------------------
  // We delete the current position from the courseway, cause this position
  // would only lead to jittering in the process of the walk.
  //
  next_index_to_set_up -- ;
  if ( next_index_to_set_up > 0 )
    {
      Me [ player_num ] . next_intermediate_point [ next_index_to_set_up ] . x = (-1) ;
      Me [ player_num ] . next_intermediate_point [ next_index_to_set_up ] . y = (-1) ;
    }

  //--------------------
  // We print out the final result for debug purposes
  //
  DebugPrintf ( DEBUG_TUX_PATHFINDING , "\nTHE FINAL WAYPOINT HISTORY LOOKS LIKE THIS:" );
  for ( i = 0 ; i < MAX_INTERMEDIATE_WAYPOINTS_FOR_TUX ; i ++ )
    {
      if ( Me [ player_num ] . next_intermediate_point [ i ] . x != (-1) )
	{
	  DebugPrintf ( DEBUG_TUX_PATHFINDING , "\nIndex: %d.  Position: (%f,%f)." , i , 
			Me [ player_num ] . next_intermediate_point [ i ] . x ,	
			Me [ player_num ] . next_intermediate_point [ i ] . y );
	}
    }

  //--------------------
  // We invert the intermediate waypoint list, because the Tux is coming from the
  // other side of course...
  //
  for ( i = 0 ; i < next_index_to_set_up / 2 ; i ++ )
    {
      tmp . x = Me [ player_num ] . next_intermediate_point [ i ] . x ;
      tmp . y = Me [ player_num ] . next_intermediate_point [ i ] . y ;

      Me [ player_num ] . next_intermediate_point [ i ] . x = 
	Me [ player_num ] . next_intermediate_point [ next_index_to_set_up -1 - i ] . x;
      Me [ player_num ] . next_intermediate_point [ i ] . y = 
	Me [ player_num ] . next_intermediate_point [ next_index_to_set_up -1 - i ] . y;

      Me [ player_num ] . next_intermediate_point [ next_index_to_set_up -1 - i ] . x =
	tmp . x ;
      Me [ player_num ] . next_intermediate_point [ next_index_to_set_up -1 - i ] . y =
	tmp . y ;
      
    }

  //--------------------
  // We print out the final result for debug purposes
  //
  DebugPrintf ( DEBUG_TUX_PATHFINDING , "\nAFTER INVERSION OF THE LIST, THIS LOOKS LIKE THIS:" );
  for ( i = 0 ; i < MAX_INTERMEDIATE_WAYPOINTS_FOR_TUX ; i ++ )
    {
      if ( Me [ player_num ] . next_intermediate_point [ i ] . x != (-1) )
	{
	  DebugPrintf ( DEBUG_TUX_PATHFINDING , "\nIndex: %d.  Position: (%f,%f)." , i , 
			Me [ player_num ] . next_intermediate_point [ i ] . x ,	
			Me [ player_num ] . next_intermediate_point [ i ] . y );
	}
    }

  streamline_tux_intermediate_course ( player_num ) ;

  //--------------------
  // We print out the final result for debug purposes
  //
  DebugPrintf ( DEBUG_TUX_PATHFINDING , "\nAFTER STREAMLINING THE LIST, THIS LOOKS LIKE THIS:" );
  for ( i = 0 ; i < MAX_INTERMEDIATE_WAYPOINTS_FOR_TUX ; i ++ )
    {
      if ( Me [ player_num ] . next_intermediate_point [ i ] . x != (-1) )
	{
	  DebugPrintf ( DEBUG_TUX_PATHFINDING , "\nIndex: %d.  Position: (%f,%f)." , i , 
			Me [ player_num ] . next_intermediate_point [ i ] . x ,	
			Me [ player_num ] . next_intermediate_point [ i ] . y );
	}
    }


  //--------------------
  // Finally, we set the reminder what the last given target was, so that
  // we'll be able to identify redundant orders later, which is IMPORTANT
  // for performance!
  //
  last_given_course_target . x = Me [ player_num ] . mouse_move_target . x ;
  last_given_course_target . y = Me [ player_num ] . mouse_move_target . y ;

  //--------------------
  // We give the number of 4-way-unresolved situations here.
  //
  DebugPrintf ( DEBUG_TUX_PATHFINDING , "\nFinal value of bad_luck_in_4_directions_counter after recursion: %d. " , 
		bad_luck_in_4_directions_counter );

}; // void set_up_intermediate_course_for_tux ( int player_num )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
move_tux_thowards_intermediate_point ( int player_num )
{
  int i;

  //--------------------
  // If there is no intermediate course, we don't need to do anything
  // in this function.
  //
  if ( Me [ player_num ] . next_intermediate_point [ 0 ] . x == (-1) )
    {
      //--------------------
      // The fact that there is no more intermediate course can mean, that
      // there never has been any intermediate course or we have now arrived
      // at the end of the previous intermediate course.
      //
      // But that maybe means, that it is now time for the combo_action, that
      // can occur on the end of any intermediate course, like e.g. open a
      // chest or pick up some item.
      //
      // DebugPrintf ( 2 , "\nAm I now at the last intermediate point???" );
      
      switch ( Me [ player_num ] . mouse_move_target_combo_action_type )
	{
	case NO_COMBO_ACTION_SET:
	  break;
	case COMBO_ACTION_OPEN_CHEST:
	  check_for_chests_to_open ( player_num , Me [ player_num ] . mouse_move_target_combo_action_parameter ) ;
	  break;
	case COMBO_ACTION_SMASH_BARREL:
	  check_for_barrels_to_smash ( player_num , Me [ player_num ] . mouse_move_target_combo_action_parameter ) ;
	  break;
	case COMBO_ACTION_PICK_UP_ITEM:
	  break;
	default:
	  GiveStandardErrorMessage ( "move_tux_thowards_intermediate_point(...)" , 
				     "Unhandled combo action for intermediate course encountered!" ,
				     PLEASE_INFORM, IS_FATAL );
	  break;
	}
      return;
    }

  //--------------------
  // Now we move the Tux thowards the next intermediate course point
  //
  if ( move_tux_thowards_raw_position ( player_num , Me [ player_num ] . next_intermediate_point [ 0 ] . x , 
					Me [ player_num ] . next_intermediate_point [ 0 ] . y ) )
    {

      if ( Me [ player_num ] . next_intermediate_point [ 0 ] . x == (-1) )
	{
	  // find_new_intermediate_point ( player_num );
	}
      else // if ( Me [ player_num ] . next_intermediate_point [ 1 ] . x != (-1) )
	{
	  DebugPrintf ( DEBUG_TUX_PATHFINDING , "\nMOVING ON TO NEXT INTERMEDIATE WAYPOINT! " );
	  for ( i = 1 ; i < MAX_INTERMEDIATE_WAYPOINTS_FOR_TUX ; i ++ )
	    {
	      Me [ player_num ] . next_intermediate_point [ i-1 ] . x = 
		Me [ player_num ] . next_intermediate_point [ i ] . x ;
	      Me [ player_num ] . next_intermediate_point [ i-1 ] . y = 
		Me [ player_num ] . next_intermediate_point [ i ] . y ;
	    }
	}
      /*
      else
	{
	  DebugPrintf ( DEBUG_TUX_PATHFINDING , "\nLAST INTERMEDIATE WAYPOINT HAS BEEN REACHED! --> clearing setup." );
	  clear_out_intermediate_points ( player_num ) ;
	  // find_new_intermediate_point ( player_num );
	}
      */
	// find_new_intermediate_point ( player_num );
    }

}; // void move_tux_thowards_intermediate_point ( int player_num )

/* ----------------------------------------------------------------------
 * This function moves the influencer, adjusts his speed according to
 * keys pressed and also adjusts his status and current "phase" of his 
 * rotation.
 * ---------------------------------------------------------------------- */
void
MoveInfluence ( int player_num )
{
  Level MoveLevel = curShip.AllLevels[ Me [ player_num ] . pos . z ] ;

  //--------------------
  // We store the influencers position for the history record and so that others
  // can follow his trail.
  //
  Me [ player_num ] . current_zero_ring_index++;
  Me [ player_num ] . current_zero_ring_index %= MAX_INFLU_POSITION_HISTORY;
  Me [ player_num ] . Position_History_Ring_Buffer [ Me [ player_num ] . current_zero_ring_index ] . x = Me [ player_num ] .pos.x;
  Me [ player_num ] . Position_History_Ring_Buffer [ Me [ player_num ] . current_zero_ring_index ] . y = Me [ player_num ] .pos.y;
  Me [ player_num ] . Position_History_Ring_Buffer [ Me [ player_num ] . current_zero_ring_index ] . z = MoveLevel->levelnum ;

  // check, if the influencer is still ok
  CheckIfCharacterIsStillOk ( player_num ) ;
  
  //--------------------
  // As a preparation for the later operations, we see if there is
  // a living droid set as a target, and if yes, we correct the move
  // target to something suiting that new droids position.
  //
  if ( Me [ player_num ] . mouse_move_target_is_enemy != (-1) )
    UpdateMouseMoveTargetAccordingToEnemy ( player_num );

  //--------------------
  // But in case of some mouse move target present, we proceed to move
  // thowards this mouse move target.
  //
  move_tux_thowards_intermediate_point ( player_num );
 
  //--------------------
  // As long as the Tux is still alive, his status will be either
  // in MOBILE mode or in WEAPON mode or in TRANSFER mode.
  //
  if ( Me [ player_num ] . energy >= 0 )
    {
      if ( ! ServerThinksSpacePressed ( player_num ) )
	{
	  Me [ player_num ] .status = MOBILE;
	}

      if ( ( ServerThinksSpacePressed ( player_num ) ) && ( ServerThinksNoDirectionPressed ( player_num ) ) &&
	   ( Me [ player_num ] .status != WEAPON ) )
	Me [ player_num ] . status = TRANSFERMODE ;

      if ( ( ServerThinksAxisIsActive ( player_num ) ) && 
	   ( ! ServerThinksNoDirectionPressed ( player_num ) ) &&
	   ( Me [ player_num ] .status != TRANSFERMODE ) )
	Me [ player_num ] .status = WEAPON ;
    }

  //--------------------
  // Perhaps the player has pressed the right mouse button, indicating the use
  // of the currently selected special function or spell.
  //
  HandleCurrentlyActivatedSkill();

  // --------------------
  // Maybe we need to fire a bullet or set a new mouse move target
  // for the new move-to location
  //
  if ( ( ServerThinksSpacePressed ( player_num ) || ServerThinksAxisIsActive ( player_num ) ) && 
       ( ! ServerThinksNoDirectionPressed ( player_num ) ) && 
       ( Me [ player_num ] . status == WEAPON ) )
    AnalyzePlayersMouseClick ( player_num );

  if ( ServerThinksSpacePressed ( player_num ) || ServerThinksAxisIsActive ( player_num ) )
    no_left_button_press_in_previous_analyze_mouse_click = FALSE ;
  else
    no_left_button_press_in_previous_analyze_mouse_click = TRUE ;

  //--------------------
  // During inventory operations, there should not be any (new) movement
  //
  if ( Item_Held_In_Hand != (-1) )
    {
      Me [ player_num ] . mouse_move_target . x = Me [ player_num ] . pos . x ;
      Me [ player_num ] . mouse_move_target . y = Me [ player_num ] . pos . y ;
      Me [ player_num ] . mouse_move_target . z = Me [ player_num ] . pos . z ;
      Me [ player_num ] . mouse_move_target_is_enemy = (-1) ;
      // clear_out_intermediate_points ( player_num );
      return; 
    }

  //--------------------
  // The influ should lose some of his speed when no key is pressed and
  // also no mouse move target is set.
  //
  InfluenceFrictionWithAir ( player_num ) ; 

  limit_tux_speed_to_a_maximum ( player_num ) ;  

  MoveTuxAccordingToHisSpeed ( player_num );

  //--------------------
  // Check it the influ is on a special field like a lift, a console or a refresh or a conveyor belt
  //
  ActSpecialField ( player_num ) ;

  AnimateInfluence ( player_num ) ;	// move the "phase" of influencers rotation

  //--------------------
  // Now we check if perhaps the influencer is close to some targeted enemy droid
  // and the takeover skill is activated as well.
  // Then of course, takeover process must be initiated.
  //
  if ( ( Me [ player_num ] . mouse_move_target_is_enemy != (-1) ) &&
       ( fabsf( AllEnemys [ Me [ player_num ] . mouse_move_target_is_enemy ] . pos . x -
		Me [ player_num ] . pos . x ) < BEST_CHAT_DISTANCE ) &&  
       ( fabsf( AllEnemys [ Me [ player_num ] . mouse_move_target_is_enemy ] . pos . y -
		Me [ player_num ] . pos . y ) < BEST_CHAT_DISTANCE ) &&  
       ( ( AllEnemys [ Me [ player_num ] . mouse_move_target_is_enemy ] . pos . z -
	   Me [ player_num ] . pos . z ) == 0 ) &&
       ( Me [ player_num ] . readied_skill == SPELL_TRANSFERMODE ) &&
       ( MouseRightPressed() )
       )
    {
      //--------------------
      // This whole action only makes sence for ENEMY droids of course!
      //
      if ( ! AllEnemys [ Me [ player_num ] . mouse_move_target_is_enemy ] . is_friendly )
	{
	  //--------------------
	  // We chat with the friendly droid
	  Takeover ( Me [ player_num ] . mouse_move_target_is_enemy ) ;
	  
	  //--------------------
	  // and then we deactivate this mouse_move_target_is_enemy to prevent
	  // immediate recurrence of the very same chat.
	  Me [ player_num ] . mouse_move_target_is_enemy = (-1) ;
	}
    }

}; // void MoveInfluence( int player_num );


/* ----------------------------------------------------------------------
 * This function checks if there is a bullet from the influencer still
 * flying around somewhere.  This is needed in case no new shot can be
 * made until the old one has passed out, a feature which is currently
 * not used anywhere in the code I think.
 * ---------------------------------------------------------------------- */
int
NoInfluBulletOnWay (void)
{
  int i;

  if ( ! ItemMap[ Me[0].weapon_item.type ].item_gun_oneshotonly )
    return TRUE;

  for (i = 0; i < MAXBULLETS; i++)
    {
      if ((AllBullets[i].type != OUT) && (AllBullets[i].mine))
	return FALSE;
    }

  return TRUE;
}; // int NoInfluBulletOnWay( void )

/* ----------------------------------------------------------------------
 * This function does the 'rotation' of the influencer, according to the
 * current energy level of the influencer.  If his energy is low, the
 * rotation will also go slow, if his energy is high, rotation will go
 * fast. 
 * ---------------------------------------------------------------------- */
void
AnimateInfluence ( int player_num )
{
  float my_speed;
#define TOTAL_SWING_TIME 0.55
#define FULL_BREATHE_TIME 3
#define TOTAL_STUNNED_TIME 0.35
#define STEP_TIME (0.28)
  static float step_countdown = 0;

  //--------------------
  // First we handle the case of just getting hit...
  //
  if ( Me [ player_num ] .got_hit_time != (-1) )
    {
      Me [ player_num ] . phase = TUX_SWING_PHASES + TUX_BREATHE_PHASES + 
	( Me [ player_num ] . got_hit_time * TUX_GOT_HIT_PHASES * 1.0 / TOTAL_STUNNED_TIME ) ;
      if ( Me [ player_num ] . got_hit_time > TOTAL_STUNNED_TIME ) Me [ player_num ] .got_hit_time = (-1) ;

      Me [ player_num ] . walk_cycle_phase = 17 ;
    }

  //--------------------
  // Now we handle the case of nothing going on and the Tux just standing around...
  // or moving to some place.
  //
  else if ( Me [ player_num ] . weapon_swing_time == (-1) )
    {
      Me [ player_num ] . phase = ( (int) ( Me [ player_num ] . MissionTimeElapsed * TUX_BREATHE_PHASES / FULL_BREATHE_TIME ) ) % TUX_BREATHE_PHASES ;

      if ( fabsf ( Me [ player_num ] . speed . x ) + fabsf ( Me [ player_num ] . speed . y ) < 0.3 )
	Me [ player_num ] . walk_cycle_phase = 17 ;
      else
	{
	  my_speed = sqrt ( Me [ player_num ] . speed . x * Me [ player_num ] . speed . x +
			    Me [ player_num ] . speed . y * Me [ player_num ] . speed . y ) ;
	  if ( my_speed <= ( TUX_WALKING_SPEED + TUX_RUNNING_SPEED ) * 0.5 )
	    Me [ player_num ] . walk_cycle_phase += Frame_Time() * 10.0 * my_speed ;
	  else
	    {
	      Me [ player_num ] . walk_cycle_phase += Frame_Time() * 3.0 * my_speed ;

	      step_countdown += Frame_Time() ;
	      if ( step_countdown > STEP_TIME )
		{
		  play_sample_using_WAV_cache( "../effects/tux_footstep.wav" , FALSE , FALSE );
		  step_countdown -= STEP_TIME ;
		  // Me [ player_num ] . running_power -= STEP_TIME * 2.0 ;
		}
	    }

	  if ( Me [ player_num ] . walk_cycle_phase > 25.0 ) Me [ player_num ] . walk_cycle_phase = 15.0 ;
	}

    }

  //--------------------
  // Now we handle the case of a weapon swing just going on...
  //
  else
    {
      Me [ player_num ] . phase = ( TUX_BREATHE_PHASES + ( Me [ player_num ] . weapon_swing_time * TUX_SWING_PHASES * 1.0 / TOTAL_SWING_TIME ) ) ;
      if ( Me [ player_num ] . weapon_swing_time > TOTAL_SWING_TIME ) Me [ player_num ] . weapon_swing_time = (-1) ;
      if ( ( (int) Me [ player_num ] . phase ) > TUX_SWING_PHASES + TUX_BREATHE_PHASES )
	{
	  Me [ player_num ] .phase = 0;
	}
      Me [ player_num ] . walk_cycle_phase = 17 ;
      // DebugPrintf ( 0 , "\nphase = %d. " , (int) Me [ player_num ] . phase );
    }

  
  if (((int) (Me [ player_num ] .phase)) >= TUX_SWING_PHASES + TUX_BREATHE_PHASES + TUX_GOT_HIT_PHASES )
    {
      Me [ player_num ] .phase = 0;
    }

  // Me [ player_num ] . walk_cycle_phase = Me [ player_num ] . phase ;

}; // void AnimateInfluence ( void )

/* ----------------------------------------------------------------------
 * This function adapts the influencers current speed to the maximal speed
 * possible for the influencer (determined by the currely used drive type).
 * ---------------------------------------------------------------------- */
void
limit_tux_speed_to_a_maximum ( int player_num )
{
  double maxspeed = TUX_RUNNING_SPEED ;

  //--------------------
  // First we adjust the speed, so that the Tux can never go too fast
  // in any direction.
  //
  if (Me [ player_num ] .speed.x > maxspeed)
    Me [ player_num ] .speed.x = maxspeed;
  if (Me [ player_num ] .speed.x < (-maxspeed))
    Me [ player_num ] .speed.x = (-maxspeed);

  if (Me [ player_num ] .speed.y > maxspeed)
    Me [ player_num ] .speed.y = maxspeed;
  if (Me [ player_num ] .speed.y < (-maxspeed))
    Me [ player_num ] .speed.y = (-maxspeed);

}; // void limit_tux_speed_to_a_maximum ( int player_num ) 

/* ----------------------------------------------------------------------
 * This function reduces the influencers speed as long as no direction 
 * key of any form is pressed and also no mouse move target is set.
 * ---------------------------------------------------------------------- */
void
InfluenceFrictionWithAir ( int player_num )
{
  //--------------------
  // Maybe the Tux is justified on his way.  Then we don't apply
  // any friction, since there is intended movement.
  //
  // if ( Me [ player_num ] . mouse_move_target . x == (-1) ) 
  //
  if ( Me [ player_num ] . next_intermediate_point [ 0 ] . x != (-1) ) 
    return;

  Me [ player_num ] . speed . x = 0 ;
  Me [ player_num ] . speed . y = 0 ;

}; // InfluenceFrictionWithAir ( int player_num )

/* ----------------------------------------------------------------------
 * This function creates several exprosions around the location where the
 * influencer is (was) positioned.  It is used after the influencers 
 * death to make his death more spectacular.
 * ---------------------------------------------------------------------- */
void
ExplodeInfluencer (void)
{
  int i;
  int counter;

  Me[0].status = OUT;

  DebugPrintf (2, "\nvoid ExplodeInfluencer(void): Real function call confirmed.");

  // create a few shifted explosions...
  for (i = 0; i < 10; i++)
    {

      // find a free blast
      counter = 0;
      while (AllBlasts[counter++].type != OUT);
      counter -= 1;
      if (counter >= MAXBLASTS)
	{
	  DebugPrintf(1, "\n\nWent out of blasts in ExplodeInfluencer...\n\n");
	  Terminate(ERR);
	}
      AllBlasts[counter].type = DRUIDBLAST;
      AllBlasts[counter].pos.x =
	Me[0].pos.x - Druid_Radius_X / 2 + MyRandom (10)*0.05;
      AllBlasts[counter].pos.y =
	Me[0].pos.y - Druid_Radius_Y / 2 + MyRandom (10)*0.05;
      AllBlasts[counter].phase = i;
    }

  DebugPrintf (2, "\nvoid ExplodeInfluencer(void): Usual end of function reached.");
}; // void ExplodeInfluencer ( void )

/* ----------------------------------------------------------------------
 * This function checks if the influencer is currently colliding with an
 * enemys and throws him back in that case.
 * ---------------------------------------------------------------------- */
void
CheckInfluenceEnemyCollision (void)
{
  int i;
  float xdist;
  float ydist;
  // float dist2;
  float max_step_size;
  // int swap;

  //--------------------
  // We need to go through the whole list of enemys...
  //
  for (i = 0; i < Number_Of_Droids_On_Ship ; i++)
    {

      //--------------------
      // ignore enemy that are not on this level or dead 
      //
      if ( AllEnemys[i].pos.z != CurLevel->levelnum )
	continue;
      if ( AllEnemys[i].type == ( -1 ) )
	continue;
      if ( AllEnemys[i].Status == OUT )
	continue;

      //--------------------
      // We determine the distance and back out immediately if there
      // is still one whole square distance or even more...
      //
      xdist = Me [ 0 ] . pos . x - AllEnemys [ i ] . pos . x;
      ydist = Me [ 0 ] . pos . y - AllEnemys [ i ] . pos . y;
      if (abs (xdist) > 1)
	continue;
      if (abs (ydist) > 1)
	continue;

      //--------------------
      // Now at this point we know, that we are pretty close.  It is time
      // to calculate the exact distance and to see if the exact distance
      // indicates a collision or not, in which case we can again back out
      //
      // dist2 = sqrt( (xdist * xdist) + (ydist * ydist) );
      // if ( dist2 > 2 * Druid_Radius_X )
      // continue;
      //
      if ( ( fabsf( xdist ) >= 2.0*Druid_Radius_X ) ||
	   ( fabsf( ydist ) >= 2.0*Druid_Radius_Y ) ) 
	continue;

      //--------------------
      // At this point we know, that the influencer *has* collided with some
      // form of 'enemy' robot.  In case of the influencer being in transfer
      // mode, we just either start the transfer subgame or we start the
      // chat interface, but after that we're sure to back out, since collsions
      // after either of that don't interest us, at least not in this frame
      // any more.
      //
      if ( Me[0].status == TRANSFERMODE )
	{
	  if ( ! AllEnemys[i].is_friendly ) Takeover (i);
	  else ChatWithFriendlyDroid( & ( AllEnemys [ i ] ) );

	  return;
	}

      // move the influencer a little bit out of the enemy AND the enemy a little bit out of the influ
      max_step_size = ((Frame_Time()) < ( MAXIMAL_STEP_SIZE ) ? (Frame_Time()) : ( MAXIMAL_STEP_SIZE )) ; 
      // Me[0].pos.x += copysignf( max_step_size , Me[0].pos.x - AllEnemys[i].pos.x ) ;
      // Me[0].pos.y += copysignf( max_step_size , Me[0].pos.y - AllEnemys[i].pos.y ) ;
      // evasion_vector . x = Me [ 0 ] . speed . x ;
      // evasion_vector . y = Me [ 0 ] . speed . y ;
      // evasion_vector . x *= Frame_Time();
      // evasion_vector . y *= Frame_Time();

      AllEnemys[i].pos.x -= copysignf( 0.6 * Frame_Time() , Me[0].pos.x - AllEnemys[i].pos.x ) ;
      AllEnemys[i].pos.y -= copysignf( 0.6 * Frame_Time() , Me[0].pos.y - AllEnemys[i].pos.y ) ;

      // AllEnemys[i].pos.x += evasion_vector . x ;
      // AllEnemys[i].pos.y += evasion_vector . y ;copysignf( 5.1 * Frame_Time() , Me[0].pos.y - AllEnemys[i].pos.y ) ;
	  
      // BounceSound ();
      
      //--------------------
      // shortly stop this enemy, then send him back to previous waypoint
      //
      if ( ! AllEnemys[i].warten )
	{
	  AllEnemys[i].warten = WAIT_COLLISION;
	  // swap = AllEnemys[i].nextwaypoint;
	  // AllEnemys[i].nextwaypoint = AllEnemys[i].lastwaypoint;
	  // AllEnemys[i].lastwaypoint = swap;

	  //--------------------
	  // Maybe we add some fun collision text, but only
	  // sometimes and only if configured to do so...
	  //
	  EnemyInfluCollisionText ( i );
	  
	}

      // InfluEnemyCollisionLoseEnergy (i);	/* someone loses energy ! */
      
    }				/* for */

}; // void CheckInfluenceEnemyCollision( void )

/* ----------------------------------------------------------------------
 * This function checks if there is some living droid below the current
 * mouse cursor and returns the index number of this droid in the array.
 * ---------------------------------------------------------------------- */
int 
GetLivingDroidBelowMouseCursor ( int player_num )
{
  int i;
  float Mouse_Blocks_X, Mouse_Blocks_Y;
  int TargetFound = (-1);
  float DistanceFound = 1000;
  float CurrentDistance;
  enemy* this_bot;

  // Mouse_Blocks_X = (float)ServerThinksInputAxisX ( player_num ) / (float)Block_Width ;
  // Mouse_Blocks_Y = (float)ServerThinksInputAxisY ( player_num ) / (float)Block_Height ;

  Mouse_Blocks_X = translate_pixel_to_map_location ( player_num , 
						     (float) ServerThinksInputAxisX ( player_num ) , 
						     (float) ServerThinksInputAxisY ( player_num ) , TRUE ) ;
  Mouse_Blocks_Y = translate_pixel_to_map_location ( player_num , 
						     (float) ServerThinksInputAxisX ( player_num ) , 
						     (float) ServerThinksInputAxisY ( player_num ) , FALSE ) ;

  //--------------------
  // We make sure the first and last but indices for the current 
  // level are at least halfway correct...
  //
  occasionally_update_first_and_last_bot_indices ( );

  // for (i = 0; i < MAX_ENEMYS_ON_SHIP; i++)
  // for (i = 0; i < Number_Of_Droids_On_Ship; i++)
  for ( i  = first_index_of_bot_on_level [ Me [ player_num ] . pos . z ] ; 
	i <=  last_index_of_bot_on_level [ Me [ player_num ] . pos . z ] ; i ++ )
    {
      this_bot = & ( AllEnemys [ i ] );

      if ( this_bot -> Status == OUT)
	continue;
      if ( this_bot -> pos . z != Me [ player_num ] . pos . z )
	continue;
      if ( fabsf ( this_bot -> pos . x - ( Mouse_Blocks_X ) ) >= DROID_SELECTION_TOLERANCE )
	continue;
      if ( fabsf ( this_bot -> pos . y - ( Mouse_Blocks_Y ) ) >= DROID_SELECTION_TOLERANCE )
	continue;

      CurrentDistance = 
	( this_bot -> pos . x - Mouse_Blocks_X ) *
	( this_bot -> pos . x - Mouse_Blocks_X ) +
	( this_bot -> pos . y - Mouse_Blocks_Y ) *
	( this_bot -> pos . y - Mouse_Blocks_Y ) ;

      if ( CurrentDistance < DistanceFound )
	{
	  DistanceFound = CurrentDistance ;
	  TargetFound = i;
	}      

    }

  //--------------------
  // It seems that we were unable to locate a living droid under the mouse 
  // cursor.  So we return, giving this very same message.
  //
  return ( TargetFound );

}; // int GetLivingDroidBelowMouseCursor ( int player_num )


/* ----------------------------------------------------------------------
 * This function fires a bullet from the influencer in some direction, 
 * no matter whether this is 'allowed' or not, not questioning anything
 * and SILENTLY TRUSTING THAT THIS TUX HAS A RANGED WEAPON EQUIPPED.
 * ---------------------------------------------------------------------- */
void
FireTuxRangedWeaponRaw ( int player_num , int weapon_item_type , int bullet_image_type, int ForceMouseUse , int FreezeSeconds , float PoisonDuration , float PoisonDamagePerSec , float ParalysationDuration , int HitPercentage ) 
{
  int i = 0;
  Bullet CurBullet = NULL;  // the bullet we're currentl dealing with
  // int bullet_image_type = ItemMap[ weapon_item_type ].item_gun_bullet_image_type;   // which gun do we have ? 
  double BulletSpeed = ItemMap [ weapon_item_type ] . item_gun_speed;
  double speed_norm;
  moderately_finepoint speed;
  float OffsetFactor;
  moderately_finepoint offset;

#define FIRE_TUX_RANGED_WEAPON_RAW_DEBUG 0 

  //--------------------
  // search for the next free bullet list entry
  //
  i = find_free_bullet_index ();
  CurBullet = & ( AllBullets [ i ] ) ;

  //--------------------
  // Now that we have found a fresh and new bullet entry, we can start
  // to fill in sensible values...
  //
  CurBullet->pos.x = Me [ player_num ] .pos.x;
  CurBullet->pos.y = Me [ player_num ] .pos.y;
  CurBullet->pos.z = Me [ player_num ] .pos.z;
  CurBullet->type = bullet_image_type;

  //--------------------
  // Previously, we had the damage done only dependant upon the weapon used.  Now
  // the damage value is taken directly from the character stats, and the UpdateAll...stats
  // has to do the right computation and updating of this value.  hehe. very conventient.
  CurBullet->damage = Me [ player_num ] . base_damage + MyRandom( Me [ player_num ] .damage_modifier);
  CurBullet->mine = TRUE;
  CurBullet->owner = -1;
  CurBullet -> time_to_hide_still = 0.3 ;
  CurBullet->bullet_lifetime        = ItemMap[ weapon_item_type ].item_gun_bullet_lifetime;
  CurBullet->angle_change_rate      = ItemMap[ weapon_item_type ].item_gun_angle_change;
  CurBullet->fixed_offset           = ItemMap[ weapon_item_type ].item_gun_fixed_offset;
  CurBullet->ignore_wall_collisions = ItemMap[ weapon_item_type ].item_gun_bullet_ignore_wall_collisions;
  CurBullet->owner_pos = & ( Me [ player_num ] .pos );
  CurBullet->time_in_frames = 0;
  CurBullet->time_in_seconds = 0;
  CurBullet->was_reflected = FALSE;
  CurBullet->reflect_other_bullets   = ItemMap[ weapon_item_type ].item_gun_bullet_reflect_other_bullets;
  CurBullet->pass_through_explosions = ItemMap[ weapon_item_type ].item_gun_bullet_pass_through_explosions;
  CurBullet->pass_through_hit_bodies = ItemMap[ weapon_item_type ].item_gun_bullet_pass_through_hit_bodies;
  CurBullet->miss_hit_influencer = UNCHECKED ;
  memset( CurBullet->total_miss_hit , UNCHECKED , MAX_ENEMYS_ON_SHIP );

  //--------------------
  // Depending on whether this is a real bullet (-1 given as parameter)
  // or not, we assign this bullet the appropriate to-hit propability
  //
  if ( HitPercentage == (-1) ) CurBullet->to_hit = Me [ player_num ] .to_hit;
  else CurBullet->to_hit = HitPercentage ;

  //--------------------
  // Maybe the bullet has some magic properties.  This is handled here.
  //
  CurBullet->freezing_level = FreezeSeconds;
  CurBullet->poison_duration = PoisonDuration;
  CurBullet->poison_damage_per_sec = PoisonDamagePerSec;
  CurBullet->paralysation_duration = ParalysationDuration;

  Me [ player_num ] . firewait = ItemMap[ weapon_item_type ].item_gun_recharging_time;

  //--------------------
  // The recharging time is now modified by the ranged weapon skill
  //
  Me [ player_num ] . firewait *= RangedRechargeMultiplierTable [ Me [ player_num ] . ranged_weapon_skill ] ;

  //--------------------
  // Use the map location to
  // pixel translation and vice versa to compute firing direction...
  //
  speed.x = translate_pixel_to_map_location ( player_num , ServerThinksInputAxisX ( player_num ) + 16 , ServerThinksInputAxisY ( player_num ) + 16 , TRUE ) - Me [ player_num ] . pos . x ;
  speed.y = translate_pixel_to_map_location ( player_num , ServerThinksInputAxisX ( player_num ) + 16 , ServerThinksInputAxisY ( player_num ) + 16 , FALSE ) - Me [ player_num ] . pos . y ;

  //--------------------
  // It might happen, that this is not a normal shot, but rather the
  // swing of a melee weapon.  Then of course, we should make a swing
  // and not start in this direction, but rather somewhat 'before' it,
  // so that the rotation will hit the target later.
  //
  // RotateVectorByAngle ( & speed , ItemMap[ weapon_item_type ] . item_gun_start_angle_modifier );

  speed_norm = sqrt ( speed . x * speed . x + speed . y * speed . y );
  CurBullet->speed.x = (speed.x/speed_norm);
  CurBullet->speed.y = (speed.y/speed_norm);

  DebugPrintf( FIRE_TUX_RANGED_WEAPON_RAW_DEBUG , 
	       "\nFireTuxRangedWeaponRaw(...) : speed_norm = %f." , speed_norm );

  //--------------------
  // Now we determine the angle of rotation to be used for
  // the picture of the bullet itself
  //
  
  CurBullet -> angle = -( atan2 (speed.y,  speed.x) * 180 / M_PI + 90 + 45 );

  DebugPrintf( FIRE_TUX_RANGED_WEAPON_RAW_DEBUG , 
	       "\nFireTuxRangedWeaponRaw(...) : Phase of bullet=%d." , CurBullet->phase );
  DebugPrintf( FIRE_TUX_RANGED_WEAPON_RAW_DEBUG , 
	       "\nFireTuxRangedWeaponRaw(...) : angle of bullet=%f." , CurBullet->angle );
  
  CurBullet -> speed . x *= BulletSpeed;
  CurBullet -> speed . y *= BulletSpeed;

  //--------------------
  // To prevent influ from hitting himself with his own bullets,
  // move them a bit..
  //
  if ( CurBullet -> angle_change_rate == 0 ) 
    OffsetFactor = 0.0; 
  else 
    OffsetFactor = 1;

  OffsetFactor = 0.25 ;

  offset . x = OffsetFactor * ( CurBullet -> speed . x / BulletSpeed );
  offset . y = OffsetFactor * ( CurBullet -> speed . y / BulletSpeed );
  RotateVectorByAngle ( & ( offset ) , -60 );
  CurBullet -> pos . x += offset . x ;
  CurBullet -> pos . y += offset . y ; 
  DebugPrintf ( 0 , "\nOffset:  x=%f y=%f." , offset . x , offset . y );

  DebugPrintf( 0 , // FIRE_TUX_RANGED_WEAPON_RAW_DEBUG , 
	       "\nFireTuxRangedWeaponRaw(...) : final position of bullet = (%f/%f)." , 
	       CurBullet->pos . x , CurBullet->pos . y );
  DebugPrintf( FIRE_TUX_RANGED_WEAPON_RAW_DEBUG , 
	       "\nFireTuxRangedWeaponRaw(...) : BulletSpeed=%f." , BulletSpeed );
  
  // CurBullet->pos.x += 0.5 ;
  // CurBullet->pos.y += 0.5 ;

}; // void FireTuxRangedWeaponRaw ( player_num ) 

/* ----------------------------------------------------------------------
 * In some cases, the mouse button will be pressed, but still some signs
 * might tell us, that this mouse button press was not intended as a move
 * or fire command to the Tux.  This function checks for these cases.
 * ---------------------------------------------------------------------- */
int 
ButtonPressWasNotMeantAsFire( player_num )
{
  //--------------------
  // If the influencer is holding something from the inventory
  // menu via the mouse, also just return
  //
  if ( Item_Held_In_Hand != (-1) ) return ( TRUE ) ;
  if ( timeout_from_item_drop > 0 ) return ( TRUE ) ;

  // If the influencer has pressed fire with the mouse cursor
  // and is in the inventory screen and inventory screen is 
  // active, then also just return
  //
  // And for the character screen, do a similar thing
  //

  //--------------------
  // Maybe the player just pressed the mouse button but INSIDE one of the character/skills/inventory
  // screens.  Then of course we will not interpret the intention to fire the weapon but rather 
  // return from here immediately.
  //
  if ( ServerThinksAxisIsActive ( player_num ) && 
       ( GameConfig.Inventory_Visible || GameConfig.CharacterScreen_Visible || GameConfig.SkillScreen_Visible ) && 
       ! CursorIsInUserRect( User_Rect.x + User_Rect.w/2 + ServerThinksInputAxisX ( player_num ) , User_Rect.y + User_Rect.h/2 + ServerThinksInputAxisY ( player_num ) ) )
    { 
      DebugPrintf( 0 , "\nCursor outside user-rect:\n  User_Rect.x=%d, User_Rect.w=%d, User_Rect.y=%d, User_Rect.h=%d." ,
		   User_Rect.x , User_Rect.w , User_Rect.y , User_Rect.h );
      DebugPrintf( 0 , "\nCursor position: X=%d, Y=%d." ,
		   ServerThinksInputAxisX ( player_num ) , ServerThinksInputAxisY ( player_num ) );
      return ( TRUE );
    }

  //--------------------
  // Also, if the cursor is right on one of the buttons, that open or close
  // e.g. the inventory screen or the skills screens or the like, we will also
  // not interpret this as a common in-game movement or firing command.
  //
  if ( ServerThinksAxisIsActive ( player_num ) && 
       ( CursorIsOnButton( INV_BUTTON , GetMousePos_x() + MOUSE_CROSSHAIR_OFFSET_X , GetMousePos_y() + MOUSE_CROSSHAIR_OFFSET_Y ) ||
	 CursorIsOnButton( SKI_BUTTON , GetMousePos_x() + MOUSE_CROSSHAIR_OFFSET_X , GetMousePos_y() + MOUSE_CROSSHAIR_OFFSET_Y ) ||
	 CursorIsOnButton( CHA_BUTTON , GetMousePos_x() + MOUSE_CROSSHAIR_OFFSET_X , GetMousePos_y() + MOUSE_CROSSHAIR_OFFSET_Y ) ) )
    {
      DebugPrintf( 0 , "\n Cursor is on a button, therefore this press will be ignored." );
      return (TRUE) ;
    }

  //--------------------
  // And also if the whole screen is filled with inventory or other screens, then we will
  // of course not fire any weapon or something but rather return immediately.
  //
  if ( ( GameConfig.CharacterScreen_Visible || GameConfig.SkillScreen_Visible ) && GameConfig.Inventory_Visible ) return (TRUE) ;

  return ( FALSE );

}; // void ButtonPressWasNotMeantAsFire( player_num )

/* ----------------------------------------------------------------------
 * At some point in the analysis of the users mouse click, we'll be 
 * certain, that a fireing/weapon swing was meant with the click.  Once
 * this is knows, this function can be called to do the mechanics of the
 * weapon use.
 * ---------------------------------------------------------------------- */
void
PerformTuxAttackRaw ( int player_num ) 
{
  int guntype = ItemMap[ Me [ player_num ] . weapon_item.type ].item_gun_bullet_image_type;   // which gun do we have ? 
  float angle;
  moderately_finepoint Weapon_Target_Vector;
  int i;
  int melee_weapon_hit_something = FALSE ;

#define PERFORM_TUX_ATTACK_RAW_DEBUG 1

  //--------------------
  // We should always make the sound of a fired bullet (or weapon swing)
  // and then of course also subtract a certain fee from the remaining weapon
  // duration in the course of the swing/hit
  //
  DebugPrintf ( PERFORM_TUX_ATTACK_RAW_DEBUG , "\nWeapon_item: %d guntype: %d . " ,  Me [ player_num ] . weapon_item . type , guntype );

  //--------------------
  // The weapon was used and therefore looses some of it's durability
  //
  DamageItem ( & ( Me [ player_num ] . weapon_item  ) );

  //--------------------
  // We always start the weapon application cycle, i.e. change of Tux
  // motion phases
  //
  Me [ player_num ] . weapon_swing_time = 0;

  //--------------------
  // Now that an attack is being made, the Tux must turn thowards the direction
  // of the attack, no matter what.
  //
  Me [ player_num ] . angle = - ( atan2 ( ServerThinksInputAxisY ( player_num ) + 16 ,  
					 ServerThinksInputAxisX ( player_num ) + 16 ) * 180 / M_PI + 90 );
  Me [ player_num ] . speed . x = 0 ;
  Me [ player_num ] . speed . y = 0 ;
  Me [ player_num ] . mouse_move_target . x = Me [ player_num ] . pos . x ;
  Me [ player_num ] . mouse_move_target . y = Me [ player_num ] . pos . y ;
  Me [ player_num ] . mouse_move_target . z = Me [ player_num ] . pos . z; 
  Me [ player_num ] . mouse_move_target_is_enemy = -1 ;

  //--------------------
  // But if the currently used weapon is a melee weapon, the tux no longer
  // generates a bullet, but rather does his weapon swinging motion and
  // only the damage is done to the robots in the area of effect
  //
  if ( ( ItemMap [ Me [ player_num ] . weapon_item . type ] . item_gun_angle_change != 0 ) ||
       ( Me [ player_num ] . weapon_item . type == (-1) ) )
    {
      //--------------------
      // Since a melee weapon is swung, which may be only influencers fists,
      // we calculate where the point
      // of the weapon should be finally hitting and do some damage
      // to all the enemys in that area.
      //
      angle = - ( atan2 ( ServerThinksInputAxisY ( player_num ) + 16 ,  
			  ServerThinksInputAxisX ( player_num ) + 16 ) * 180 / M_PI + 90 - 45 );
      DebugPrintf( PERFORM_TUX_ATTACK_RAW_DEBUG , "\n===> Fire Bullet: angle=%f. " , angle ) ;
      DebugPrintf( PERFORM_TUX_ATTACK_RAW_DEBUG , "\n===> Fire Bullet: InpAxis: X=%d Y=%d . " , 
		   ServerThinksInputAxisX ( player_num ) , 
		   ServerThinksInputAxisY ( player_num ) ) ;
      Weapon_Target_Vector.x = 0 ;
      Weapon_Target_Vector.y = - 0.8 ;
      RotateVectorByAngle ( & Weapon_Target_Vector , angle );
      Weapon_Target_Vector.x += Me [ player_num ] . pos . x;
      Weapon_Target_Vector.y += Me [ player_num ] . pos . y;
      DebugPrintf( PERFORM_TUX_ATTACK_RAW_DEBUG , "\n===> Fire Bullet target: x=%f, y=%f. " , Weapon_Target_Vector.x , Weapon_Target_Vector.y ) ;
      
      for ( i = 0 ; i < Number_Of_Droids_On_Ship ; i ++ )
	{
	  if ( AllEnemys [ i ] . Status == OUT ) continue;
	  if ( AllEnemys [ i ] . pos . z != Me [ player_num ] . pos . z ) continue;
	  if ( fabsf ( AllEnemys [ i ] . pos . x - Weapon_Target_Vector.x ) > 0.5 ) continue;
	  if ( fabsf ( AllEnemys [ i ] . pos . y - Weapon_Target_Vector.y ) > 0.5 ) continue;

	  //--------------------
	  // So here we know, that the Tux weapon swing might actually hit something
	  // as far as only 'area of attack' and position of possible target is 
	  // concerned.  So now we check, whether this weapon swing really is a 'hit'
	  // in the sense of AD&D games, that something can either hit or miss.
	  //
	  if ( MyRandom ( 100 ) > Me [ player_num ] . to_hit ) continue ; 

	  AllEnemys [ i ] . energy -= Me [ player_num ] . base_damage + MyRandom ( Me [ player_num ] . damage_modifier );
	  enemy_spray_blood ( & ( AllEnemys [ i ] ) ) ;

	  melee_weapon_hit_something = TRUE;

	  start_gethit_animation_if_applicable ( & ( AllEnemys [ i ] ) ) ; 

	  // AllEnemys[ i ] . is_friendly = 0 ;
	  // AllEnemys[ i ] . combat_state = MAKE_ATTACK_RUN ;
	  robot_group_turn_hostile ( i );
	  SetRestOfGroupToState ( & ( AllEnemys[i] ) , MAKE_ATTACK_RUN );

	  //--------------------
	  // We'll launch the attack cry of this bot...
	  //
	  if ( Druidmap[ AllEnemys[ i ] . type ].greeting_sound_type != (-1) )
	    {
	      play_enter_attack_run_state_sound ( Druidmap[ AllEnemys[ i ] . type ].greeting_sound_type );
	    }

	  //--------------------
	  // War tux freezes enemys with the appropriate plugin...
	  AllEnemys[ i ] . frozen += Me [ player_num ] . freezing_melee_targets ; 

	  AllEnemys[ i ] . firewait = 
	    1 * ItemMap [ Druidmap [ AllEnemys [ i ] . type ] . weapon_item.type ] . item_gun_recharging_time ;
	    // 2 * ItemMap [ Druidmap [ AllEnemys [ i ] . type ] . weapon_item.type ] . item_gun_recharging_time ;

	  //--------------------
	  // Only if the Tux didn't kill the poor bot, we'll play the 'got-hit-sound' of
	  // that enemy, otherwise the InitaiteDeathOfThisEnemy function will play the 
	  // death sound for this enemy anyway.
	  //
	  if ( AllEnemys[ i ] . energy > 0 )
	    PlayEnemyGotHitSound ( Druidmap [ AllEnemys [ i ] . type ] . got_hit_sound_type );

	  DebugPrintf( PERFORM_TUX_ATTACK_RAW_DEBUG , "\n===> Fire Bullet hit something.... melee ... " ) ;
	}

      //--------------------
      // Also, we should check if there was perhaps a chest or box
      // or something that can be smashed up, cause in this case, we
      // must open pendoras box now.
      //
      // SmashBox ( Weapon_Target_Vector.x , Weapon_Target_Vector.y );
      if ( smash_obstacle ( Weapon_Target_Vector.x , Weapon_Target_Vector.y ) )
	melee_weapon_hit_something = TRUE;
      
      //--------------------
      // Finally we add a new wait-counter, so that bullets or swings
      // cannot be started in too rapid succession.  
      // 
      // And then we can return, for real bullet generation in the sense that
      // we would have to enter something into the AllBullets array or that
      // isn't required in our case here.
      //
      if ( Me [ player_num ] . weapon_item . type != ( -1 ) )
	Me [ player_num ] . firewait = ItemMap [ Me [ player_num ] . weapon_item.type ] . item_gun_recharging_time;
      else
	Me [ player_num ] . firewait = 0.5;

      // Now we modify for melee weapon skill...
      Me [ player_num ] . firewait *= MeleeRechargeMultiplierTable [ Me [ player_num ] . melee_weapon_skill ] ;


      if ( melee_weapon_hit_something ) play_melee_weapon_hit_something_sound();
      else play_melee_weapon_missed_sound();

      return;
    }

  //--------------------
  //
  if ( Me [ player_num ] . weapon_item . type != (-1) ) Fire_Bullet_Sound ( guntype );
  else Fire_Bullet_Sound ( LASER_SWORD_1 );

  FireTuxRangedWeaponRaw ( player_num , Me [ player_num ] . weapon_item . type , guntype, FALSE , 0 , 0 , 0 , 0 , -1 ) ;

}; // void PerformTuxAttackRaw ( int player_num ) ;

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
float
translate_pixel_to_map_location ( int player_num , float axis_x , float axis_y , int give_x ) 
{

  //--------------------
  // NOTE:  This function does not expect absolute screen coordinates but rather coordinates relative
  // to the center of the screen.
  //
  // That's also why it's 'axis' rather than 'pos' or 'point'.
  //
  // That is because mouse clicks can best be analyzed this way.
  //

  if ( give_x )
    {
      return ( Me [ player_num ] . pos . x + ( axis_x / ( (float) iso_floor_tile_width ) ) + 
	       ( axis_y / ( (float) iso_floor_tile_height) ) ) ;
    }
  else
    {
      return ( Me [ player_num ] . pos . y - ( axis_x / ( (float) iso_floor_tile_width ) ) + 
	       ( axis_y / ( (float) iso_floor_tile_height) ) ) ;
    }
	      
}; // int translate_pixel_to_map_location ( int player_num , int axis_x , int axis_y , int give_x ) 

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
float
translate_pixel_to_zoomed_map_location ( int player_num , float axis_x , float axis_y , int give_x ) 
{
  if ( give_x )
    {
      return ( Me [ player_num ] . pos . x + ( FIXED_ZOOM_OUT_FACT * axis_x / ((float)iso_floor_tile_width) ) + ( FIXED_ZOOM_OUT_FACT * axis_y / ((float)iso_floor_tile_height) ) ) ;
      // return ( ( axis_x / ISO_WIDTH ) + ( axis_y / ISO_HEIGHT ) ) ;
    }
  else
    {
      return ( Me [ player_num ] . pos . y - ( FIXED_ZOOM_OUT_FACT * axis_x / ((float)iso_floor_tile_width) ) + ( FIXED_ZOOM_OUT_FACT * axis_y / ((float)iso_floor_tile_height) ) ) ;
      // return ( - ( axis_x / ISO_WIDTH ) + ( axis_y / ISO_HEIGHT ) ) ;
    }
	      
}; // int translate_pixel_to_zoomed_map_location ( int player_num , int axis_x , int axis_y , int give_x ) 

/* ----------------------------------------------------------------------
 *
 * 
 * ---------------------------------------------------------------------- */
int
translate_map_point_to_screen_pixel ( float x_map_pos , float y_map_pos , int give_x )
{
  if ( give_x )
    {
      // return ( UserCenter_x + ( ( x_map_pos - Me [ 0 ] . pos . x ) + ( Me [ 0 ] . pos . y - y_map_pos ) ) * iso_floor_tile_width / 2 ) ; 
      //--------------------
      // Now we attempt the same with less deletion of digits...
      return ( rintf ( UserCenter_x + ( x_map_pos + Me [ 0 ] . pos . y - Me [ 0 ] . pos . x - y_map_pos ) * iso_floor_tile_width / 2 ) ) ; 
    }
  else
    {
      // return ( UserCenter_y + ( ( x_map_pos + y_map_pos - Me [ 0 ] . pos . x - Me [ 0 ] . pos . y ) * iso_floor_tile_height / 2 ) ) ;
      return ( rintf ( ( UserCenter_y + ( x_map_pos + y_map_pos - Me [ 0 ] . pos . x - Me [ 0 ] . pos . y ) * iso_floor_tile_height / 2 ) ) ) ;
    }
}; // int translate_map_point_to_screen_pixel ( float x_map_pos , float y_map_pos , int give_x )

/* ----------------------------------------------------------------------
 *
 * 
 * ---------------------------------------------------------------------- */
int
translate_map_point_to_zoomed_screen_pixel ( float x_map_pos , float y_map_pos , int give_x )
{
  if ( give_x )
    {
      return ( UserCenter_x + ( ( x_map_pos - Me [ 0 ] . pos . x ) + ( Me [ 0 ] . pos . y - y_map_pos ) ) * iso_floor_tile_width / (2 * FIXED_ZOOM_OUT_FACT) ) ; 
    }
  else
    {
      return ( UserCenter_y + ( ( x_map_pos + y_map_pos - Me [ 0 ] . pos . x - Me [ 0 ] . pos . y ) * iso_floor_tile_height / ( 2 * FIXED_ZOOM_OUT_FACT ) ) ) ;
    }
}; // int translate_map_point_to_zoomed_screen_pixel ( float x_map_pos , float y_map_pos , int give_x )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
check_for_chests_to_open ( int player_num , int chest_index ) 
{
  Level our_level = curShip . AllLevels [ Me [ player_num ] . pos . z ] ;

  if ( chest_index != (-1) )
    {
      //--------------------
      // So the player clicked on a chest.  Well, if the chest is already close
      // enough, it should be sufficient to just spill out the contents of the
      // chest and then return.  However, if the player was not yet close enough
      // to the chest, we need to SET A COMBINED_ACTION, i.e. first set the walk
      // thowards the chest and then set the open_chest action, which is more
      // complicated of course.
      //
      if ( fabsf ( Me [ player_num ] . pos . x - our_level -> obstacle_list [ chest_index ] . pos . x ) +
	   fabsf ( Me [ player_num ] . pos . y - our_level -> obstacle_list [ chest_index ] . pos . y ) < 1.1 )
	{
	  throw_out_all_chest_content ( chest_index ) ;

	  if ( our_level -> obstacle_list [ chest_index ] . type == ISO_H_CHEST_CLOSED )
	    our_level -> obstacle_list [ chest_index ] . type = ISO_H_CHEST_OPEN  ;
	  if ( our_level -> obstacle_list [ chest_index ] . type == ISO_V_CHEST_CLOSED )
	    our_level -> obstacle_list [ chest_index ] . type = ISO_V_CHEST_OPEN  ;

	  //--------------------
	  // Maybe a combo_action has made us come here and open the chest.  Then of
	  // course we can remove the combo action setting now...
	  //
	  Me [ player_num ] . mouse_move_target_combo_action_type = NO_COMBO_ACTION_SET ;
	  Me [ player_num ] . mouse_move_target_combo_action_parameter = ( -1 ) ;

	  //--------------------
	  // Now that the chest has been opend, we don't need to do anything more
	  //
	  return;
	}
      else
	{
	  //--------------------
	  // So here we know, that we must set the course thowards the chest.  We
	  // do so first.
	  //
	  DebugPrintf ( 2 , "\ncheck_for_chests_to_open:  setting up combined mouse move target!" );

	  switch ( our_level -> obstacle_list [ chest_index ] . type )
	    {
	    case ISO_V_CHEST_CLOSED:
	    case ISO_V_CHEST_OPEN:
	      Me [ player_num ] . mouse_move_target . x = our_level -> obstacle_list [ chest_index ] . pos . x ;
	      Me [ player_num ] . mouse_move_target . y = our_level -> obstacle_list [ chest_index ] . pos . y ;
	      Me [ player_num ] . mouse_move_target . x += 0.8 ;
	      set_up_intermediate_course_for_tux ( player_num ) ;

	      Me [ player_num ] . mouse_move_target_is_enemy = ( -1 ) ;
	      Me [ player_num ] . mouse_move_target_combo_action_type = COMBO_ACTION_OPEN_CHEST ;
	      Me [ player_num ] . mouse_move_target_combo_action_parameter = chest_index ;
	      
	      break;
	    case ISO_H_CHEST_CLOSED:
	    case ISO_H_CHEST_OPEN:
	      Me [ player_num ] . mouse_move_target . x = our_level -> obstacle_list [ chest_index ] . pos . x ;
	      Me [ player_num ] . mouse_move_target . y = our_level -> obstacle_list [ chest_index ] . pos . y ;
	      Me [ player_num ] . mouse_move_target . y += 0.8 ;
	      set_up_intermediate_course_for_tux ( player_num ) ;

	      Me [ player_num ] . mouse_move_target_is_enemy = ( -1 ) ;
	      Me [ player_num ] . mouse_move_target_combo_action_type = COMBO_ACTION_OPEN_CHEST ;
	      Me [ player_num ] . mouse_move_target_combo_action_parameter = chest_index ;

	      break;
	    default:
	      GiveStandardErrorMessage ( "check_for_chests_to_open(...)" , 
					 "chest to be approached is not a chest obstacle!!" ,
					 PLEASE_INFORM, IS_FATAL );
	      break;
	    }
	}
    }

}; // void check_for_chests_to_open ( int player_num ) 

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
check_for_barrels_to_smash ( int player_num , int barrel_index ) 
{
  Level our_level = curShip . AllLevels [ Me [ player_num ] . pos . z ] ;
  int i;
  moderately_finepoint step_vector;
  float vec_len;

  if ( barrel_index != (-1) )
    {
      //--------------------
      // If the smash distance for a barrel is not yet reached, then we must set up
      // a course that will lead us to the barrel and also a combo_action specification,
      // that will cause the corresponding barrel to be smashed upon arrival.
      //
      if ( calc_euklid_distance ( Me [ player_num ] . pos . x , Me [ player_num ] . pos . y , 
				  our_level -> obstacle_list [ barrel_index ] . pos . x ,
				  our_level -> obstacle_list [ barrel_index ] . pos . y ) 
	   > ( obstacle_map [ ISO_BARREL_1 ] . block_area_parm_1 * sqrt(2) ) / 2.0 + 0.5 )
	{
	  //--------------------
	  // We set up a course, that will lead us directly to the barrel, that we are
	  // supposed to smash (upon arrival, later).
	  //
	  // For this purpose, we take a vector and rotate it around the barrel center to
	  // find the 'best' location to go to for the smashing motion...
	  //
	  step_vector . x = Me [ player_num ] . pos . x - our_level -> obstacle_list [ barrel_index ] . pos . x ;
	  step_vector . y = Me [ player_num ] . pos . y - our_level -> obstacle_list [ barrel_index ] . pos . y ;
	  vec_len = vect_len ( step_vector );

	  //--------------------
	  // We normalize the distance of the final walk-point to the barrel center just
	  // so, that it will be within the 'strike_distance' we have used just above in
	  // the 'distance-met' query.
	  //
	  step_vector . x *= ( ( obstacle_map [ ISO_BARREL_1 ] . block_area_parm_1 * sqrt(2) ) / 2.0 + 0.05 ) / vec_len ;
	  step_vector . y *= ( ( obstacle_map [ ISO_BARREL_1 ] . block_area_parm_1 * sqrt(2) ) / 2.0 + 0.05 ) / vec_len ;

	  for ( i = 0 ; i < 8 ; i ++ )
	    {
	      if ( IsPassable ( our_level -> obstacle_list [ barrel_index ] . pos . x + step_vector . x ,
				our_level -> obstacle_list [ barrel_index ] . pos . y + step_vector . y ,
				Me [ player_num ] . pos . z ) )
		{
		  //--------------------
		  // The obstacle plus the step vector give us the position to move the
		  // Tux to for the optimal strike...
		  //
		  Me [ player_num ] . mouse_move_target . x = our_level -> obstacle_list [ barrel_index ] . pos . x + 
		    step_vector . x ;
		  Me [ player_num ] . mouse_move_target . y = our_level -> obstacle_list [ barrel_index ] . pos . y + 
		    step_vector . y ;
		  set_up_intermediate_course_for_tux ( player_num ) ;

		  //--------------------
		  // We set up the combo_action, so that the barrel can be smashed later...
		  //
		  Me [ player_num ] . mouse_move_target_is_enemy = ( -1 ) ;
		  Me [ player_num ] . mouse_move_target_combo_action_type = COMBO_ACTION_SMASH_BARREL ;
		  Me [ player_num ] . mouse_move_target_combo_action_parameter = barrel_index ;
		  break;
		}

	      //--------------------
	      // If this vector didn't bring us any luck, we rotate by 45 degrees and try anew...
	      //
	      RotateVectorByAngle ( & ( step_vector ) , 45.0 ) ;
	    }

	  //--------------------
	  // since we cannot smash immediately, we must return now.  Later, on the 
	  // second call once we've reached the barrel, this will be different.
	  //
	  return;
	}

      //--------------------
      // Before the barrel can get destroyed and we loose the position information,
      // we record the vector of the Tux strike direction...
      //
      step_vector . x = - Me [ player_num ] . pos . x + our_level -> obstacle_list [ barrel_index ] . pos . x ;
      step_vector . y = - Me [ player_num ] . pos . y + our_level -> obstacle_list [ barrel_index ] . pos . y ;

      //--------------------
      // We make sure the barrel gets smashed, eben if the strike made by the
      // Tux would be otherwise a miss...
      //
      smash_obstacle ( our_level -> obstacle_list [ barrel_index ] . pos . x , 
		       our_level -> obstacle_list [ barrel_index ] . pos . y );

      //--------------------
      // We start an attack motion...
      //
      tux_wants_to_attack_now ( player_num ) ;

      //--------------------
      // We set a direction of facing directly thowards the barrel in question
      // so that the strike motion looks authentic...
      //
      Me [ player_num ] . angle = - ( atan2 ( step_vector . y ,  step_vector . x ) * 180 / M_PI - 180 - 45 );
      Me [ player_num ] . angle += 360 / ( 2 * MAX_TUX_DIRECTIONS );
      while ( Me [ player_num ] . angle < 0 ) Me [ player_num ] . angle += 360;

      //--------------------
      // Maybe the barrel smashing came from a combo_action, i.e. the click made
      // the tux come here and the same click should now also make the Tux smash
      // the barrel.  So we smash the barrel. but that also means, that we can 
      // now unset the combo_action back to normal state again.
      //
      Me [ player_num ] . mouse_move_target_combo_action_type = NO_COMBO_ACTION_SET ;
      Me [ player_num ] . mouse_move_target_combo_action_parameter = ( -1 ) ;
      DebugPrintf ( 2 , "\ncheck_for_barrels_to_smash(...):  combo_action now unset." );
    }
}; // void check_for_barrels_to_smash ( int player_num , int barrel_index ) 

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
check_for_droids_to_attack_or_talk_with ( int player_num ) 
{
  int index_of_droid_below_mouse_cursor = GetLivingDroidBelowMouseCursor ( player_num ) ;

  //--------------------
  // Now the new mouse move: If there is 
  //
  // * NO enemy below the mouse cursor, 
  // * NO box below the mouse cursor
  // * AND also shift was NOT pressed
  //
  // a move to that location, not a fire command, 
  // so only new target will be set and return without attack motion.
  //
  if ( index_of_droid_below_mouse_cursor == (-1) )
    {
      Me [ player_num ] . mouse_move_target . x = 
	translate_pixel_to_map_location ( player_num , ServerThinksInputAxisX ( player_num ) , ServerThinksInputAxisY ( player_num ) , TRUE ) ;
      // Me [ player_num ] . pos . x + ( (float) ServerThinksInputAxisX ( player_num ) ) / (float) Block_Width ;
      Me [ player_num ] . mouse_move_target . y = 
	translate_pixel_to_map_location ( player_num , ServerThinksInputAxisX ( player_num ) , ServerThinksInputAxisY ( player_num ) , FALSE ) ;
	// Me [ player_num ] . pos . y + ( (float) ServerThinksInputAxisY ( player_num ) ) / (float) Block_Width ;
      Me [ player_num ] . mouse_move_target . z = Me [ player_num ] . pos . z ;

      Me [ player_num ] . mouse_move_target_is_enemy = (-1) ;

      // clear_out_intermediate_points ( player_num ) ;

      set_up_intermediate_course_for_tux ( player_num ) ;

      return; // no attack motion since no target given!!
    }

  if ( index_of_droid_below_mouse_cursor != (-1) )
    {
      //--------------------
      // We assign the target robot of the coming attack operation.
      // In case of no robot, we should get (-1), which is also serving us well.
      //
      Me [ player_num ] . mouse_move_target_is_enemy = index_of_droid_below_mouse_cursor ;

      //--------------------
      // If the click was onto a friendly droid, we initiate talk, no matter if the
      // TAKEOVER skill was actiavted or if the TRANSFERMODE state of the tux was 
      // present.  Left click should do this, other methods for talking are depreciated.
      //
      // However, we do not want an accidential mouse move while button is held down
      // to always initiate talk.  This should require a true click.  Therefore we
      // check if the button has been released just before it came to this check.
      //
      if ( AllEnemys [ Me [ player_num ] . mouse_move_target_is_enemy ] . is_friendly ) 
	{

	  if ( no_left_button_press_in_previous_analyze_mouse_click )
	    {
	      ChatWithFriendlyDroid ( & ( AllEnemys [ Me [ player_num ] . mouse_move_target_is_enemy ] ) ) ;
	  
	      //--------------------
	      // and then we deactivate this mouse_move_target_is_enemy to prevent
	      // immediate recurrence of the very same chat.
	      //
	      Me [ player_num ] . mouse_move_target_is_enemy = (-1) ;
	    }

	  return ;
	}

      if ( Me [ 0 ] . weapon_item . type >= 0 )
	{
	  if ( ( ItemMap [ Me [ 0 ] . weapon_item . type ] . item_gun_angle_change ) &&
	       ( calc_euklid_distance ( Me [ player_num ] . pos . x , Me [ player_num ] . pos . y , 
					AllEnemys [ index_of_droid_below_mouse_cursor ] . pos . x ,
					AllEnemys [ index_of_droid_below_mouse_cursor ] . pos . y ) 
		 > BEST_MELEE_DISTANCE+0.1 ) )
	    return;
	}
      else if ( calc_euklid_distance ( Me [ player_num ] . pos . x , Me [ player_num ] . pos . y , 
				       AllEnemys [ index_of_droid_below_mouse_cursor ] . pos . x ,
				       AllEnemys [ index_of_droid_below_mouse_cursor ] . pos . y ) 
		> BEST_MELEE_DISTANCE+0.1 )
	return;

      tux_wants_to_attack_now ( player_num ) ;
    }
}; // void check_for_droids_to_attack ( int player_num ) 

/* ----------------------------------------------------------------------
 * If the user clicked his mouse, this might have several reasons.  It 
 * might happen to open some windows, pick up some stuff, smash a box,
 * move somewhere or fire a shot or make a weapon swing.
 * 
 * Therefore it is not so easy to decide what to do upon a users mouse
 * click and so this function analyzes the situation and decides what to
 * do.
 * ---------------------------------------------------------------------- */
void
AnalyzePlayersMouseClick ( int player_num )
{
  DebugPrintf ( 2 , "\n===> void AnalyzePlayersMouseClick ( int player_num ) : real function call confirmed. " ) ;

  if ( ButtonPressWasNotMeantAsFire( player_num ) ) return;

  check_for_chests_to_open ( player_num , closed_chest_below_mouse_cursor ( player_num ) ) ;

  check_for_barrels_to_smash ( player_num , smashable_barred_below_mouse_cursor ( player_num ) ) ;

  check_for_droids_to_attack_or_talk_with ( player_num ) ;

}; // void AnalyzePlayersMouseClick ( int player_num )

/* ----------------------------------------------------------------------
 * influ-enemy collisions are sucking someones
 * energy, depending no longer on classes of the colliding parties,
 * but on their weights
 * ---------------------------------------------------------------------- */
void
InfluEnemyCollisionLoseEnergy (int enemynum)
{
  int enemytype = AllEnemys[enemynum].type;

  //--------------------
  // This will not be done any more...
  //

  return;

  if ( AllEnemys[enemynum].is_friendly ) return;

  if (Me[0].type <= enemytype)
    {
      if (InvincibleMode)
	return;
      
      Me[0].energy -=
	(Druidmap[enemytype].weight -
	 Druidmap[Me[0].type].weight ) * collision_lose_energy_calibrator * 0.01 ;
    }
  else
    AllEnemys[enemynum].energy -=
      (Druidmap[Me[0].type].weight -
       Druidmap[enemytype].weight ) * collision_lose_energy_calibrator * 0.01;

}; // void InfluEnemyCollisionLoseEnergy(int enemynum)

#undef _influ_c
