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
#define DISTANCE_TOLERANCE (0.2)
#define FORCE_FIRE_DISTANCE (1.5)
#define ATTACK_BOXES_DISTANCE (2.0)
// #define DROID_SELECTION_TOLERANCE (0.9)
// #define DROID_SELECTION_TOLERANCE (0.3)
#define DROID_SELECTION_TOLERANCE (0.5)

#define REFRESH_ENERGY		3
#define COLLISION_PUSHSPEED	7

#define BOUNCE_LOSE_ENERGY 3	/* amount of lose-energy at enemy-collisions */
#define BOUNCE_LOSE_FACT 1
void InfluEnemyCollisionLoseEnergy (int enemynum);	/* influ can lose energy on coll. */
int NoInfluBulletOnWay (void);
void limit_tux_speed_to_a_maximum ( int player_num );

#define MAXIMAL_STEP_SIZE ( 7.0/20.0 )

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

  if ( CursorIsInUserRect( GetMousePos_x()+16 , GetMousePos_y()+16 ) && ( CurLevel != NULL ) )
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
	  if ( !CountItemtypeInInventory ( ItemMap [ Me [ 0 ] . weapon_item . type ] . item_gun_use_ammunition , 
					   0 ) )
	    {
	      No_Ammo_Sound();
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
UpdateMouseMoveTargetAccoringToEnemy ( int player_num )
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

      DebugPrintf ( 0 , "\nRemaining way: %f %f." , RemainingWay . x , RemainingWay . y );

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
 * Self-explanatory.
 * This function also takes into account any conveyor belts the Tux might
 * be standing on.
 * ---------------------------------------------------------------------- */
void
MoveTuxAccordingToHisSpeed ( int player_num )
{
  float planned_step_x;
  float planned_step_y;


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
  if ( fabsf( planned_step_x ) >= MAXIMAL_STEP_SIZE )
    {
      planned_step_x = copysignf( MAXIMAL_STEP_SIZE , planned_step_x );
    }
  if ( fabsf( planned_step_y ) >= MAXIMAL_STEP_SIZE )
    {
      planned_step_y = copysignf( MAXIMAL_STEP_SIZE , planned_step_y );
    }

  Me [ player_num ] .pos.x += planned_step_x;
  Me [ player_num ] .pos.y += planned_step_y;

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
move_tux_thowards_mouse_move_target ( int player_num )
{
  moderately_finepoint RemainingWay;
  moderately_finepoint MinimalWayAtThisSpeed;
  float accel;

  //--------------------
  // We do not move any players, who's statuses are 'OUT'.
  //
  if ( Me [ player_num ] . status == OUT ) return;
  // if ( Me [ player_num ] . energy <= 0 ) return;

  // DebugPrintf ( 0 , "\nNow at:  move_tux_thowards_mouse_move_target." );

  accel = 5.0 * Frame_Time() ;

  //--------------------
  // Let's do some mathematics:  We compute how far we have to go still
  // and we also compute how far we will inevitably go even if we pull the breakes
  // or even better use the usual friction with air to stop our motion immediately.
  // Once we know that, we can simply decide if we still have to build up speed or
  // if it's time to slow down again and so finally we will slide to a stop exactly
  // at the place where we intend to be.  So:  Mathematics is always helpful. :)
  //
  RemainingWay . x = Me [ player_num ] . pos . x - Me [ player_num ] . mouse_move_target . x ;
  RemainingWay . y = Me [ player_num ] . pos . y - Me [ player_num ] . mouse_move_target . y ;
  
  MinimalWayAtThisSpeed . x = Me [ player_num ] . speed . x / log ( FRICTION_CONSTANT ) ;
  MinimalWayAtThisSpeed . y = Me [ player_num ] . speed . y / log ( FRICTION_CONSTANT ) ;
  
  if ( fabsf ( MinimalWayAtThisSpeed . x ) < fabsf ( RemainingWay . x ) )
    {
      if ( RemainingWay.x > 0 ) Me [ player_num ] .speed.x -= accel;
      else Me [ player_num ] .speed.x += accel;
    }
  else
    {
      Me [ player_num ] . speed . x *= exp ( log ( FRICTION_CONSTANT ) * Frame_Time ( ) );
      if ( fabsf ( Me [ player_num ] . speed . x ) < 0.3 ) Me [ player_num ] . speed . x = 0 ;
    }
  if ( fabsf ( MinimalWayAtThisSpeed . y ) < fabsf ( RemainingWay . y ) )
    {
      if ( RemainingWay.y > 0 ) Me [ player_num ] .speed.y -= accel;
      else Me [ player_num ] .speed.y += accel;
    }
  else
    {
      Me [ player_num ] . speed . y *= exp ( log ( FRICTION_CONSTANT ) * Frame_Time ( ) );
      if ( fabsf ( Me [ player_num ] . speed . y ) < 0.3 ) Me [ player_num ] . speed . y = 0 ;
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
      Me [ player_num ] . mouse_move_target . x = ( -1 ) ;
      Me [ player_num ] . mouse_move_target . y = ( -1 ) ;
      Me [ player_num ] . mouse_move_target . z = ( -1 ) ;
      
      // Me [ player_num ] . mouse_move_target_is_enemy = ( -1 ) ;
    }

}; // void move_tux_thowards_mouse_move_target ( int player_num )

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
    UpdateMouseMoveTargetAccoringToEnemy ( player_num );

  //--------------------
  // But in case of some mouse move target present, we proceed to move
  // thowards this mouse move target.
  //
  if ( Me [ player_num ] . mouse_move_target . x != ( -1 ) )
    {
      move_tux_thowards_mouse_move_target ( player_num );
    }
  
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

      // if ( ( ServerThinksSpacePressed ( player_num ) || ServerThinksAxisIsActive ( player_num ) ) && 
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

  //--------------------
  // During inventory operations, there should not be any (new) movement
  //
  if ( Item_Held_In_Hand != (-1) )
    {
      Me [ player_num ] . mouse_move_target . x = Me [ player_num ] . pos . x ;
      Me [ player_num ] . mouse_move_target . y = Me [ player_num ] . pos . y ;
      Me [ player_num ] . mouse_move_target . z = Me [ player_num ] . pos . z ;
      Me [ player_num ] . mouse_move_target_is_enemy = (-1) ;
      return; 
    }

  //--------------------
  // The influ should lose some of his speed when no key is pressed and
  // also no mouse move target is set.
  //
  if ( Me [ player_num ] . mouse_move_target . x == (-1) ) InfluenceFrictionWithAir ( player_num ) ; 

  limit_tux_speed_to_a_maximum ( player_num ) ;  

  MoveTuxAccordingToHisSpeed ( player_num );

  //--------------------
  // Check it the influ is on a special field like a lift, a console or a refresh or a conveyor belt
  //
  ActSpecialField ( player_num ) ;

  AnimateInfluence ( player_num ) ;	// move the "phase" of influencers rotation

  if ( ( Me [ player_num ] . mouse_move_target_is_enemy != (-1) ) &&
       ( ( AllEnemys [ Me [ player_num ] . mouse_move_target_is_enemy ] . pos . z -
	   Me [ player_num ] . pos . z ) == 0 ) &&
       ( AllEnemys [ Me [ player_num ] . mouse_move_target_is_enemy ] . is_friendly ) )
    {
      if ( Me [ player_num ] . readied_skill == SPELL_TRANSFERMODE )
	{
	  //--------------------
	  // We chat with the friendly droid
	  ChatWithFriendlyDroid ( & ( AllEnemys [ Me [ player_num ] . mouse_move_target_is_enemy ] ) ) ;
	  
	  //--------------------
	  // and then we deactivate this mouse_move_target_is_enemy to prevent
	  // immediate recurrence of the very same chat.
	  Me [ player_num ] . mouse_move_target_is_enemy = (-1) ;
	}
    }

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
      Me [ player_num ] .phase = ( (int) ( Me [ player_num ] .MissionTimeElapsed * TUX_BREATHE_PHASES / FULL_BREATHE_TIME ) ) % TUX_BREATHE_PHASES ;

      if ( fabsf ( Me [ player_num ] . speed . x ) + fabsf ( Me [ player_num ] . speed . y ) < 0.3 )
	Me [ player_num ] . walk_cycle_phase = 17 ;
      else
	{
	  my_speed = sqrt ( Me [ player_num ] . speed . x * Me [ player_num ] . speed . x +
			    Me [ player_num ] . speed . y * Me [ player_num ] . speed . y ) ;
	  if ( my_speed <= RUNNING_SPEED_THRESHOLD )
	    Me [ player_num ] . walk_cycle_phase += Frame_Time() * 10.0 * my_speed ;
	  else
	    Me [ player_num ] . walk_cycle_phase += Frame_Time() * 3.0 * my_speed ;

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
      DebugPrintf ( 0 , "\nphase = %d. " , (int) Me [ player_num ] . phase );
    }

  
  if (((int) (Me [ player_num ] .phase)) >= TUX_SWING_PHASES + TUX_BREATHE_PHASES + TUX_GOT_HIT_PHASES )
    {
      Me [ player_num ] .phase = 0;
    }

  // Me [ player_num ] . walk_cycle_phase = Me [ player_num ] . phase ;

}; // void AnimateInfluence ( void )

/* ----------------------------------------------------------------------
 * This function checks for collisions of the influencer with walls,
 * doors, consoles, boxes and all other map elements.
 * In case of a collision, the position and speed of the influencer are
 * adapted accordingly.
 * ---------------------------------------------------------------------- */
void
CheckInfluenceWallCollisions ( int player_num )
{
  double SX = Me [ player_num ] .speed.x * Frame_Time ();
  double SY = Me [ player_num ] .speed.y * Frame_Time ();
  finepoint lastpos;
  int res; 
  int NorthSouthAxisBlocked=FALSE;
  int EastWestAxisBlocked=FALSE;
  int H_Door_Sliding_Active = FALSE;
  double maxspeed;
  Level InfluencerLevel = curShip . AllLevels [ Me [ player_num ] . pos . z ] ;

  //--------------------
  // First we introduce some security against later segfaults
  // due to calculation with properties of items of type -1.
  //
  if ( Me [ player_num ] . status == OUT ) return;
  if ( Me [ player_num ] . energy <= 0   ) return;

  if ( Me [ player_num ] .drive_item.type != (-1) )
    maxspeed = ItemMap [ Me [ player_num ] .drive_item.type ].item_drive_maxspeed ;
  else maxspeed = TUX_MAXSPEED_WITHOUT_DRIVE_ITEM ;

  maxspeed = 7.0 ;

  lastpos.x = Me [ player_num ] .pos.x - SX;
  lastpos.y = Me [ player_num ] .pos.y - SY;

  res = DruidPassable ( Me [ player_num ] .pos.x , Me [ player_num ] .pos.y , Me [ player_num ] . pos . z );

  //--------------------
  // Influence-Wall-Collision only has to be checked in case of
  // a collision of course, which is indicated by res not CENTER.
  if (res != CENTER )
    {

      //--------------------
      // At first we just check in which directions (from the last position)
      // the ways are blocked and in which directions the ways are open.
      //
      if ( ! ( ( DruidPassable ( lastpos.x , lastpos.y + maxspeed * Frame_Time() , Me [ player_num ] . pos . z ) != CENTER ) ||
	       ( DruidPassable ( lastpos.x , lastpos.y - maxspeed * Frame_Time() , Me [ player_num ] . pos . z ) != CENTER ) ) )
	{
	  DebugPrintf(1, "\nNorth-south-Axis seems to be free.");
	  NorthSouthAxisBlocked = FALSE;
	}
      else
	{
	  NorthSouthAxisBlocked = TRUE;
	}

      if ( ( DruidPassable(lastpos.x + maxspeed * Frame_Time() , lastpos.y , Me [ player_num ] . pos . z ) == CENTER ) &&
	   ( DruidPassable(lastpos.x - maxspeed * Frame_Time() , lastpos.y , Me [ player_num ] . pos . z ) == CENTER ) )
	{
	  EastWestAxisBlocked = FALSE;
	}
      else 
	{
	  EastWestAxisBlocked = TRUE;
	}

      //--------------------
      // Now we try to handle the sitution:
      //

      if ( NorthSouthAxisBlocked )
	{
	  // NorthSouthCorrectionDone=TRUE;
	  Me [ player_num ] .pos.y = lastpos.y;
	  Me [ player_num ] .speed.y = 0;
	  
	  // if its an open door, we also correct the east-west position, in the
	  // sense that we move thowards the middle
	  if ( ( GetMapBrick ( InfluencerLevel , Me [ player_num ] .pos.x , Me [ player_num ] .pos.y - 0.5  ) == H_OPEN_DOOR ) || 
	       ( GetMapBrick ( InfluencerLevel , Me [ player_num ] .pos.x , Me [ player_num ] .pos.y + 0.5  ) == H_OPEN_DOOR ) )
	    {
	      Me [ player_num ] .pos.x += copysignf ( PUSHSPEED * Frame_Time() , ( rintf(Me [ player_num ] .pos.x) - Me [ player_num ] .pos.x ));
	      H_Door_Sliding_Active = TRUE;
	    }
	}

      if ( EastWestAxisBlocked )
	{
	  // EastWestCorrectionDone=TRUE;
	  if ( !H_Door_Sliding_Active ) Me [ player_num ] .pos.x = lastpos.x;
	  Me [ player_num ] .speed.x = 0;

	  // if its an open door, we also correct the north-south position, in the
	  // sense that we move thowards the middle
	  if ( ( GetMapBrick ( InfluencerLevel , Me [ player_num ] . pos . x + 0.5 , Me [ player_num ] . pos . y ) == V_OPEN_DOOR ) || 
	       ( GetMapBrick ( InfluencerLevel , Me [ player_num ] . pos . x - 0.5 , Me [ player_num ] . pos . y ) == V_OPEN_DOOR ) )
	    Me [ player_num ] .pos.y += copysignf (PUSHSPEED * Frame_Time() , ( rintf(Me [ player_num ] .pos.y) - Me [ player_num ] .pos.y ));
	}

      if ( EastWestAxisBlocked && NorthSouthAxisBlocked )
	{
	  // printf("\nBOTH AXES BLOCKED... Corner handling activated...");
	  // in case both axes were blocked, we must be at a corner.  
	  // both axis-blocked-routines have been executed, so the speed has
	  // been set to absolutely zero and we are at the previous position.
	  //
	  // But perhaps everything would be fine,
	  // if we just restricted ourselves to moving in only ONE direction.
	  // try if this would make sense...
	  // (Of course we may only move into the one direction that is free)
	  //
	  if ( DruidPassable( Me [ player_num ] . pos . x + SX , 
			      Me [ player_num ] . pos . y ,
			      Me [ player_num ] . pos . z ) == CENTER ) Me [ player_num ] .pos.x += SX;
	  if ( DruidPassable( Me [ player_num ] . pos . x      , 
			      Me [ player_num ] . pos . y + SY ,
			      Me [ player_num ] . pos . z ) == CENTER ) Me [ player_num ] .pos.y += SY;
	}

      //--------------------
      // Here I introduce some extra security as a fallback:  Obviously
      // if the influencer is blocked FOR THE SECOND TIME, then the throw-back-algorithm
      // above HAS FAILED.  The absolutely fool-proof and secure handling is now done by
      // simply reverting to the last influ coordinated, where influ was NOT BLOCKED.
      // For this reason, a history of influ-coordinates has been introduced.  This will all
      // be done here and now:
      
      if ( ( DruidPassable ( Me [ player_num ] .pos.x, Me [ player_num ] .pos.y , Me [ player_num ] . pos . z ) != CENTER) && 
	   ( DruidPassable ( GetInfluPositionHistoryX( 0 ) , GetInfluPositionHistoryY( 0 ) , GetInfluPositionHistoryZ( 0 ) ) != CENTER) &&
	   ( DruidPassable ( GetInfluPositionHistoryX( 1 ) , GetInfluPositionHistoryY( 1 ) , GetInfluPositionHistoryZ( 1 ) ) != CENTER) )
	{
	  Me [ player_num ] .pos.x = GetInfluPositionHistoryX( 2 );
	  Me [ player_num ] .pos.y = GetInfluPositionHistoryY( 2 );
	  DebugPrintf ( 0, "\nATTENTION! CheckInfluenceWallCollsision FALLBACK ACTIVATED!!");
	}

    }

} /* CheckInfluenceWallCollisions */

/* ----------------------------------------------------------------------
 * This function adapts the influencers current speed to the maximal speed
 * possible for the influencer (determined by the currely used drive type).
 * ---------------------------------------------------------------------- */
void
limit_tux_speed_to_a_maximum ( int player_num )
{
  double maxspeed = 7.0 ;

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

  if ( ! ServerThinksUpPressed ( player_num ) && ! ServerThinksDownPressed ( player_num ) )
    {
      Me [ player_num ] . speed . y *= exp ( log ( FRICTION_CONSTANT ) * Frame_Time ( ) );
    }
  if ( ! ServerThinksRightPressed ( player_num ) && ! ServerThinksLeftPressed ( player_num ) )
    {
      Me [ player_num ] . speed . x *= exp ( log ( FRICTION_CONSTANT ) * Frame_Time ( ) );
    }

  if ( fabsf ( Me [ player_num ] . speed . x ) < 0.3 ) Me [ player_num ] . speed . x = 0 ;
  if ( fabsf ( Me [ player_num ] . speed . y ) < 0.3 ) Me [ player_num ] . speed . y = 0 ;

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
  float dist2;
  float max_step_size;
  int swap;
  int first_collision = TRUE;	/* marker */

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
      xdist = Me[0].pos.x - AllEnemys[i].pos.x;
      ydist = Me[0].pos.y - AllEnemys[i].pos.y;
      if (abs (xdist) > 1)
	continue;
      if (abs (ydist) > 1)
	continue;

      //--------------------
      // Now at this point we know, that we are pretty close.  It is time
      // to calculate the exact distance and to see if the exact distance
      // indicates a collision or not, in which case we can again back out
      //
      dist2 = sqrt( (xdist * xdist) + (ydist * ydist) );
      if ( dist2 > 2 * Druid_Radius_X )
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

      //--------------------
      // Now we've arrived at a real classical 'boing'-influencer-enemy-collision
      //
      // THE FIRST_COLLSION VARIABLE IS CURRENTLY WITHOUT MEANING ALWAYS TRUE!!!
      //
      if (first_collision)
	{			
	  //--------------------
	  // we invert the speed vector of the influencer
	  //
	  Me[0].speed.x = -Me[0].speed.x * 0.25 ;
	  Me[0].speed.y = -Me[0].speed.y * 0.25 ;
	  
	  /*
	    This old code has cause the annoying collision behavious.  Maybe we
	    dont need this at all any more right now, so let's try without.  I hope
	    no occasions of getting 'sucked in' occur with it.

	  if (Me[0].speed.x != 0)
	    Me[0].speed.x +=
	      COLLISION_PUSHSPEED * (Me[0].speed.x / fabsf (Me[0].speed.x));
	  else if (xdist)
	    Me[0].speed.x = COLLISION_PUSHSPEED * (xdist / fabsf (xdist));
	  if (Me[0].speed.y != 0)
	    Me[0].speed.y +=
	      COLLISION_PUSHSPEED * (Me[0].speed.y / fabsf (Me[0].speed.y));
	  else if (ydist)
	    Me[0].speed.y = COLLISION_PUSHSPEED * (ydist / fabsf (ydist));
	  */

	  // move the influencer a little bit out of the enemy AND the enemy a little bit out of the influ
	  max_step_size = ((Frame_Time()) < ( MAXIMAL_STEP_SIZE ) ? (Frame_Time()) : ( MAXIMAL_STEP_SIZE )) ; 
	  Me[0].pos.x += copysignf( max_step_size , Me[0].pos.x - AllEnemys[i].pos.x ) ;
	  Me[0].pos.y += copysignf( max_step_size , Me[0].pos.y - AllEnemys[i].pos.y ) ;
	  AllEnemys[i].pos.x -= copysignf( Frame_Time() , Me[0].pos.x - AllEnemys[i].pos.x ) ;
	  AllEnemys[i].pos.y -= copysignf( Frame_Time() , Me[0].pos.y - AllEnemys[i].pos.y ) ;
	  
	  // there might be walls close too, so lets check again for collisions with them
	  CheckInfluenceWallCollisions ( 0 );
	  
	  BounceSound ();
	  
	} // if first_collision (ALWAYS TRUE ANYWAY...)
      
      //--------------------
      // shortly stop this enemy, then send him back to previous waypoint
      //
      if ( ! AllEnemys[i].warten )
	{
	  AllEnemys[i].warten = WAIT_COLLISION;
	  swap = AllEnemys[i].nextwaypoint;
	  AllEnemys[i].nextwaypoint = AllEnemys[i].lastwaypoint;
	  AllEnemys[i].lastwaypoint = swap;

	  //--------------------
	  // Maybe we add some fun collision text, but only
	  // sometimes and only if configured to do so...
	  //
	  EnemyInfluCollisionText ( i );
	  
	}
      InfluEnemyCollisionLoseEnergy (i);	/* someone loses energy ! */
      
    }				/* for */

}; // void CheckInfluenceEnemyCollision( void )

/* ----------------------------------------------------------------------
 * This function checks if there is a crushable box below the mouse 
 * cursor or not.
 * This function is useful for determining if a mouse-button-press was
 * meant as a mouse-indicated move instruction was given or rather a 
 * weapon swing/weapon fire command was meant by the player.
 * ---------------------------------------------------------------------- */
int
CrushableBoxBelowMouseCursor ( int player_num )
{
  Level SpecialFieldLevel;
  unsigned char MapBrick;
  float Mouse_Blocks_X, Mouse_Blocks_Y;

  Mouse_Blocks_X = ((float)ServerThinksInputAxisX ( player_num )) / ((float)Block_Width  ) ;
  Mouse_Blocks_Y = ((float)ServerThinksInputAxisY ( player_num )) / ((float)Block_Height ) ;

  //--------------------
  // Now we get the brick code at our corrent location.
  //
  SpecialFieldLevel = curShip . AllLevels [ Me [ player_num ] . pos . z ] ;
  MapBrick = GetMapBrick ( SpecialFieldLevel , Me [ player_num ] . pos . x + Mouse_Blocks_X , 
			   Me [ player_num ] . pos . y + Mouse_Blocks_Y ) ;

  switch ( MapBrick )
    {
    case BOX_1:
    case BOX_2:
    case BOX_3:
    case BOX_4:
      return ( TRUE );
      break;
    default:
      return ( FALSE );
      break;
    }

  //--------------------
  // It seems that we were unable to locate a box under the mouse 
  // cursor.  So we return, giving this very same message.
  //
  return ( FALSE );

}; // int CrushableBoxBelowMouseCursor ( int player_num )


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

  // Mouse_Blocks_X = (float)ServerThinksInputAxisX ( player_num ) / (float)Block_Width ;
  // Mouse_Blocks_Y = (float)ServerThinksInputAxisY ( player_num ) / (float)Block_Height ;

  Mouse_Blocks_X = translate_pixel_to_map_location ( player_num , 
						     (float) ServerThinksInputAxisX ( player_num ) , 
						     (float) ServerThinksInputAxisY ( player_num ) , TRUE ) ;
  Mouse_Blocks_Y = translate_pixel_to_map_location ( player_num , 
						     (float) ServerThinksInputAxisX ( player_num ) , 
						     (float) ServerThinksInputAxisY ( player_num ) , FALSE ) ;


  // for (i = 0; i < MAX_ENEMYS_ON_SHIP; i++)
  for (i = 0; i < Number_Of_Droids_On_Ship; i++)
    {
      if (AllEnemys[i].Status == OUT)
	continue;
      if (AllEnemys[i].pos.z != Me[ player_num ] . pos . z )
	continue;
      if ( fabsf ( AllEnemys[i].pos.x - ( Mouse_Blocks_X ) ) >= DROID_SELECTION_TOLERANCE )
	continue;
      if ( fabsf ( AllEnemys[i].pos.y - ( Mouse_Blocks_Y ) ) >= DROID_SELECTION_TOLERANCE )
	continue;

      CurrentDistance = 
	( fabsf (AllEnemys[i].pos.x - ( Mouse_Blocks_X ) ) ) *
	( fabsf (AllEnemys[i].pos.x - ( Mouse_Blocks_X ) ) ) +
	( fabsf (AllEnemys[i].pos.y - ( Mouse_Blocks_Y ) ) ) *
	( fabsf (AllEnemys[i].pos.y - ( Mouse_Blocks_Y ) ) ) ;


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
  for (i = 0; i < (MAXBULLETS); i++)
    {
      if (AllBullets[i].type == OUT)
	{
	  CurBullet = &AllBullets[i];
	  break;
	}
    }
  //--------------------
  // didn't find any free bullet entry? --> take the first and issue a warning...
  //
  if (CurBullet == NULL)
    {      
      CurBullet = &AllBullets[0];
      GiveStandardErrorMessage ( "FireTuxRangedWeaponRaw(...)" , "\
There seems to be no more free bullet entry to fire the shot of the Tux...\n\
This is very strange.  Well, we'll overwrite the first entry and continue.",
				 NO_NEED_TO_INFORM, IS_WARNING_ONLY );
    }

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

  /*
  speed.x = 0.0;
  speed.y = 0.0;

  if ( ServerThinksDownPressed ( player_num ) )
    speed.y = 1.0;
  if ( ServerThinksUpPressed ( player_num ) )
    speed.y = -1.0;
  if ( ServerThinksLeftPressed ( player_num ) )
    speed.x = -1.0;
  if ( ServerThinksRightPressed ( player_num ) )
    speed.x = 1.0;

    // if using a joystick/mouse, allow exact directional shots! 
  if ( ServerThinksAxisIsActive ( player_num ) || ForceMouseUse )
    {
      max_val = max ( abs( ServerThinksInputAxisX ( player_num ) ) , 
		      abs( ServerThinksInputAxisY ( player_num ) ) );
      speed.x = 1.0 * ServerThinksInputAxisX ( player_num ) / max_val ;
      speed.y = 1.0 * ServerThinksInputAxisY ( player_num ) / max_val ;

      DebugPrintf( FIRE_TUX_RANGED_WEAPON_RAW_DEBUG , 
      "\nFireTuxRangedWeaponRaw(...) : Server thinks axis: (%d/%d)." , ServerThinksInputAxisX ( player_num ) , ServerThinksInputAxisX ( player_num ) );
      }
  */

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
       ( CursorIsOnButton( INV_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) ||
	 CursorIsOnButton( SKI_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) ||
	 CursorIsOnButton( CHA_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) ) )
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

#define PERFORM_TUX_ATTACK_RAW_DEBUG 1

  //--------------------
  // We should always make the sound of a fired bullet (or weapon swing)
  // and then of course also subtract a certain fee from the remaining weapon
  // duration in the course of the swing/hit
  //
  DebugPrintf ( PERFORM_TUX_ATTACK_RAW_DEBUG , "\nWeapon_item: %d guntype: %d . " ,  Me [ player_num ] . weapon_item . type , guntype );

  if ( Me [ player_num ] . weapon_item . type != (-1) ) Fire_Bullet_Sound ( guntype );
  else Fire_Bullet_Sound ( LASER_SWORD_1 );
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
	  AllEnemys[ i ] . energy -= Me [ player_num ] .base_damage + MyRandom( Me [ player_num ] .damage_modifier );

	  // AllEnemys[ i ] . is_friendly = 0 ;
	  // AllEnemys[ i ] . combat_state = MAKE_ATTACK_RUN ;
	  robot_group_turn_hostile ( i );
	  SetRestOfGroupToState ( & ( AllEnemys[i] ) , MAKE_ATTACK_RUN );

	  //--------------------
	  // We'll launch the attack cry of this bot...
	  //
	  if ( Druidmap[ AllEnemys[ i ] . type ].greeting_sound_type != (-1) )
	    {
	      PlayStartAttackSound( Druidmap[ AllEnemys[ i ] . type ].greeting_sound_type );
	    }

	  //--------------------
	  // War tux freezes enemys with the appropriate plugin...
	  AllEnemys[ i ] . frozen += Me [ player_num ] . freezing_melee_targets ; 

	  AllEnemys[ i ] . firewait = 
	    1 * ItemMap [ Druidmap [ AllEnemys [ i ] . type ] . weapon_item.type ] . item_gun_recharging_time ;
	    // 2 * ItemMap [ Druidmap [ AllEnemys [ i ] . type ] . weapon_item.type ] . item_gun_recharging_time ;
	  PlayEnemyGotHitSound ( Druidmap [ AllEnemys [ i ] . type ] . got_hit_sound_type );
	  DebugPrintf( PERFORM_TUX_ATTACK_RAW_DEBUG , "\n===> Fire Bullet hit something.... melee ... " ) ;
	}

      //--------------------
      // Also, we should check if there was perhaps a chest or box
      // or something that can be smashed up, cause in this case, we
      // must open pendoras box now.
      //
      // SmashBox ( Weapon_Target_Vector.x , Weapon_Target_Vector.y );
      smash_obstacle ( Weapon_Target_Vector.x , Weapon_Target_Vector.y );
      
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

      return;
    }

  FireTuxRangedWeaponRaw ( player_num , Me [ player_num ] . weapon_item . type , guntype, FALSE , 0 , 0 , 0 , 0 , -1 ) ;

}; // void PerformTuxAttackRaw ( int player_num ) ;

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
float
translate_pixel_to_map_location ( int player_num , float axis_x , float axis_y , int give_x ) 
{
  if ( give_x )
    {
      return ( Me [ player_num ] . pos . x + ( axis_x / ((float)iso_floor_tile_width) ) + ( axis_y / ((float)iso_floor_tile_height) ) ) ;
    }
  else
    {
      return ( Me [ player_num ] . pos . y - ( axis_x / ((float)iso_floor_tile_width) ) + ( axis_y / ((float)iso_floor_tile_height) ) ) ;
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
  int chest_index ;
  int index_of_droid_below_mouse_cursor = (-1) ;

  DebugPrintf ( 2 , "\n===> void AnalyzePlayersMouseClick ( int player_num ) : real function call confirmed. " ) ;

  if ( ButtonPressWasNotMeantAsFire( player_num ) ) return;

  //--------------------
  // A chest, that has been clicked on, will be opened and the content thrown out...
  //
  chest_index = closed_chest_below_mouse_cursor ( player_num ) ;
  if ( chest_index != (-1) )
    {
      if ( fabsf ( Me [ player_num ] . pos . x - curShip . AllLevels [ Me [ player_num ] . pos . z ] -> obstacle_list [ chest_index ] . pos . x ) +
	   fabsf ( Me [ player_num ] . pos . y - curShip . AllLevels [ Me [ player_num ] . pos . z ] -> obstacle_list [ chest_index ] . pos . y ) < 1.1 )
	{
	  throw_out_all_chest_content ( chest_index ) ;

	  if ( curShip . AllLevels [ Me [ player_num ] . pos . z ] -> obstacle_list [ chest_index ] . type == ISO_H_CHEST_CLOSED )
	    curShip . AllLevels [ Me [ player_num ] . pos . z ] -> obstacle_list [ chest_index ] . type = ISO_H_CHEST_OPEN  ;
	  if ( curShip . AllLevels [ Me [ player_num ] . pos . z ] -> obstacle_list [ chest_index ] . type == ISO_V_CHEST_CLOSED )
	    curShip . AllLevels [ Me [ player_num ] . pos . z ] -> obstacle_list [ chest_index ] . type = ISO_V_CHEST_OPEN  ;
	}
    }

  index_of_droid_below_mouse_cursor = GetLivingDroidBelowMouseCursor ( player_num ) ;

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
  if  ( ( index_of_droid_below_mouse_cursor == (-1) ) && 
	( ! CrushableBoxBelowMouseCursor ( player_num ) ) )
    {
      Me [ player_num ] . mouse_move_target . x = 
	translate_pixel_to_map_location ( player_num , ServerThinksInputAxisX ( player_num ) , ServerThinksInputAxisY ( player_num ) , TRUE ) ;
      // Me [ player_num ] . pos . x + ( (float) ServerThinksInputAxisX ( player_num ) ) / (float) Block_Width ;
      Me [ player_num ] . mouse_move_target . y = 
	translate_pixel_to_map_location ( player_num , ServerThinksInputAxisX ( player_num ) , ServerThinksInputAxisY ( player_num ) , FALSE ) ;
	// Me [ player_num ] . pos . y + ( (float) ServerThinksInputAxisY ( player_num ) ) / (float) Block_Width ;
      Me [ player_num ] . mouse_move_target . z = Me [ player_num ] . pos . z ;

      Me [ player_num ] . mouse_move_target_is_enemy = (-1) ;
      return; // no attack motion since no target given!!
    }

  if ( CrushableBoxBelowMouseCursor ( player_num ) )
    {

      Me [ player_num ] . mouse_move_target . x = 
	Me [ player_num ] . pos . x + ( (float) ServerThinksInputAxisX ( player_num ) ) / (float) Block_Width ;
      Me [ player_num ] . mouse_move_target . y = 
	Me [ player_num ] . pos . y + ( (float) ServerThinksInputAxisY ( player_num ) ) / (float) Block_Width ;
      Me [ player_num ] . mouse_move_target . z = Me [ player_num ] . pos . z ;

      //--------------------
      // Only if the Tux wields a melee weapon and is too far away will we return 
      // instead of continuing for the attack move now...
      //
      if ( ! ( ( ( fabsf ( Me [ player_num ] . mouse_move_target . x - Me [ player_num ] . pos . x ) < ATTACK_BOXES_DISTANCE ) && 
		 ( fabsf ( Me [ player_num ] . mouse_move_target . y - Me [ player_num ] . pos . y ) < ATTACK_BOXES_DISTANCE ) ) ||
	       ( ItemMap [ Me [ player_num ] . weapon_item . type ] . item_gun_angle_change == 0 ) ) )
	return;
    }


  if ( index_of_droid_below_mouse_cursor != (-1) )
    {
      //--------------------
      // We assign the target robot of the coming attack operation.
      // In case of no robot, we should get (-1), which is also serving us well.
      //
      Me [ player_num ] . mouse_move_target_is_enemy = index_of_droid_below_mouse_cursor ;

      if ( AllEnemys [ Me [ player_num ] . mouse_move_target_is_enemy ] . is_friendly ) return ;

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

}; // void AnalyzePlayersMouseClick ( int player_num )

/* ----------------------------------------------------------------------
 * This function does all the things needed, when the influencer is on
 * some refresh field, i.e. it increases influencers current energy but
 * it also decreases his current score = experience points...
 * ---------------------------------------------------------------------- */
void
RefreshInfluencer (void)
{

  if ( Me[0].energy < Me[0].maxenergy )
    {
      Me[0].energy += REFRESH_ENERGY * Frame_Time () * 5;
      Me[0].Experience -= REFRESH_ENERGY * Frame_Time () * 10;
      if (Me[0].energy > Me[0].health)
	Me[0].energy = Me[0].health;

      RefreshSound ();
      
      //--------------------
      // since robots like the refresh, the influencer might also say so...
      //
      if ( GameConfig.Influencer_Refresh_Text )
	{
	  Me[0].TextToBeDisplayed="Ahhh, that feels so good...";
	  Me[0].TextVisibleTime=0;
	}
    }
  else
    {
      //--------------------
      // If nothing more is to be had, the influencer might also say so...
      if ( GameConfig.Influencer_Refresh_Text )
	{
	  Me[0].TextToBeDisplayed="Oh, it seems that was it again.";
	  Me[0].TextVisibleTime=0;
	}
    }

  return;
}; // void RefreshInfluence ( void )

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
