/* 
 *   Copyright (c) 1994, 2002, 2003 Johannes Prix
 *   Copyright (c) 1994, 2002 Reinhard Prix
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
 * This file contains all enemy realted functions.  This includes their 
 * whole behaviour, healing, initialization, shuffling them around after 
 * evevator-transitions, deleting them, collisions of enemys among 
 * themselves, their fireing, animation and such.
 * ---------------------------------------------------------------------- */

#define _enemy_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"

#define COL_SPEED		3

#define IS_FRIENDLY_EYE_DISTANCE (2.0)

void ProcessAttackStateMachine ( int enemynum );
void AnimateEnemys ( void );

/* ----------------------------------------------------------------------
 * In the very beginning of each game, it is not enough to just place the
 * bots onto the right locations.  They must also be integrated into the
 * waypoint system, i.e. current waypoint and next waypoint initialized.
 * This is what this function is supposed to do.
 * ---------------------------------------------------------------------- */
void
TeleportToClosestWaypoint ( Enemy ThisRobot )
{
  int i;
  float BestDistance = 10000;
  float NewDistance;
  Level ThisLevel = curShip . AllLevels [ ThisRobot->pos.z ] ;
  int BestWaypoint = ( -1 );

  DebugPrintf ( 1 , "\nAdvancedCommand == 2 encountered --> teleporting to closest wp." );
  ThisRobot->AdvancedCommand = 0 ;
  
  for ( i = 0 ; i < ThisLevel->num_waypoints ; i ++ )
    {
      if ( ThisLevel -> AllWaypoints [ i ] . x <= 0 ) continue;
      
      NewDistance = sqrt ( ( ThisRobot -> pos . x - ThisLevel -> AllWaypoints [ i ] . x ) *
			   ( ThisRobot -> pos . x - ThisLevel -> AllWaypoints [ i ] . x ) +
			   ( ThisRobot -> pos . y - ThisLevel -> AllWaypoints [ i ] . y ) *
			   ( ThisRobot -> pos . y - ThisLevel -> AllWaypoints [ i ] . y ) ) ;

      if ( NewDistance <= BestDistance )
	{
	  BestDistance = NewDistance;
	  BestWaypoint = i ;
	}
    }

  //--------------------
  // Now we have found a global minimum.  So we 'teleport' there.
  //
  ThisRobot -> pos . x = ThisLevel -> AllWaypoints [ BestWaypoint ] . x ;
  ThisRobot -> pos . y = ThisLevel -> AllWaypoints [ BestWaypoint ] . y ;
  ThisRobot -> nextwaypoint = BestWaypoint ;
  ThisRobot -> lastwaypoint = BestWaypoint ;

}; // void TeleportToClosestWaypoint ( Enemy ThisRobot )

/* ----------------------------------------------------------------------
 * This function tests, if a Robot can go a direct straigt line from
 * x1 y1 to x2 y2 without hitting a wall or another obstacle.
 * 
 * The return value is TRUE or FALSE accoringly.
 * ----------------------------------------------------------------------*/
int 
DirectLineWalkable( float x1 , float y1 , float x2 , float y2 , int z )
{
    float LargerDistance;
    int Steps;
    int i;
    finepoint step;
    finepoint CheckPosition;
    static int first_call = TRUE;
    static int step_multiplier = -1 ; // something completely absurd...
    int steps_for_this_obstacle;
    static int key_obstacle_type = -1 ;

    //--------------------
    // On the very first call of this function, we find out how much of
    // a stepsize will be possible for the purpose of finding out, if any
    // given line is directly walkable or not.  Depending on the collision
    // rectangles defined for each obstacle, we'll find out the maximum
    // step distance we can take.
    //
    if ( first_call )
    {
	first_call = FALSE;
	
	DebugPrintf ( 1 , "\n%s():Now calibrating passability check maximum stepsize..." , __FUNCTION__ );
	
	for ( i = 0 ; i < NUMBER_OF_OBSTACLE_TYPES ; i ++ )
	{
	    if ( obstacle_map [ i ] . block_area_type == COLLISION_TYPE_RECTANGLE )
	    {
		steps_for_this_obstacle = ( 1.0 / obstacle_map [ i ] . block_area_parm_1 ) + 1 ;
		if ( steps_for_this_obstacle > step_multiplier )
		{
		    step_multiplier = steps_for_this_obstacle;
		    key_obstacle_type = i ;
		}
		
		steps_for_this_obstacle = ( 1.0 / obstacle_map [ i ] . block_area_parm_2 ) + 1 ;
		if ( steps_for_this_obstacle > step_multiplier )
		{
		    step_multiplier = steps_for_this_obstacle;
		    key_obstacle_type = i ;
		}
	    }
	}
	
	DebugPrintf ( 1 , "\n%s(): Final calibration for passablilit check produced multiplier : %d." , 
		      __FUNCTION__ , step_multiplier );
	DebugPrintf ( 1 , "\n%s(): The key obstacle type for this calibration was : %d." , 
		      __FUNCTION__ , key_obstacle_type );
	
    }
  

    //--------------------
    // First we determine the amount of steps we need to take
    // such that we can't oversee any walls or something.
    //
    if ( fabsf(x1-x2) > fabsf (y1-y2) ) LargerDistance = fabsf(x1-x2);
    else LargerDistance=fabsf(y1-y2);
    
    //--------------------
    // The larger distance must be used to compute the steps nescessary
    // for good passability check. i.e. passability check such that no
    // movement completely through an obstacle will be possible.
    //
    // The number of steps must of course be multiplied with the minimum
    // number of steps for one floor tile, which has been calibrated
    // (hopefully sensibly) above.
    //
    Steps = LargerDistance * step_multiplier ; 
    if ( Steps <= 1 ) Steps = 2 ; // return TRUE;
    
    //--------------------
    // We determine the step size when walking from (x1,y1) to (x2,y2) in Steps number of steps
    //
    step.x = (x2 - x1) / ( float ) Steps;
    step.y = (y2 - y1) / ( float ) Steps;
    
    DebugPrintf( 2 , "\n%s():  step.x=%f step.y=%f." , __FUNCTION__ , step.x , step.y );
    
    //--------------------
    // We start from position (x1, y1)
    //
    CheckPosition . x = x1;
    CheckPosition . y = y1;
    
    for ( i = 0 ; i < Steps + 1 ; i++ )
    {
	if ( ! IsPassable ( CheckPosition . x , CheckPosition . y , z ) ) 
	{
	    DebugPrintf( 1 , "\n%s(): Connection analysis revealed : OBSTACLES!! NO WAY!!!" , __FUNCTION__ );
	    return FALSE;
	}	

	CheckPosition.x += step.x;
	CheckPosition.y += step.y;
    }

    DebugPrintf( 1 , "\n%s(): Connection analysis revealed : FREE!" , __FUNCTION__ );

    return TRUE;

}; // int DirectLineWalkable( float x1 , float y1 , float x2 , float y2 )


/* ----------------------------------------------------------------------
 * This function tests, if the given Robot can go a direct straigt line 
 * to the next console.  If so, that way is set as his next 'parawaypoint'
 * and TRUE is returned.
 * Else nothing is done and FALSE is returned.
 * ----------------------------------------------------------------------*/
int 
SetDirectCourseToConsole( int EnemyNum )
{
  int i, j;
  long TicksBefore;
  Level ThisBotsLevel = curShip.AllLevels[ AllEnemys[ EnemyNum ].pos.z ];

  TicksBefore = SDL_GetTicks();

  DebugPrintf( 1 , "\nSetDirectCourseToConsole( int EnemyNum ): real function call confirmed.");
  DebugPrintf( 1 , "\nSetDirectCourseToConsole( int EnemyNum ): Trying to find direct line to console...");

  for ( i = 0 ; i < ThisBotsLevel->xlen ; i ++ )
    {
      for ( j = 0 ; j < ThisBotsLevel->ylen ; j ++ )
	{
	  switch ( ThisBotsLevel->map[j][i]  . floor_value )
	    {
	    case CONSOLE_D:
	    case CONSOLE_U:
	    case CONSOLE_R:
	    case CONSOLE_L:
	      DebugPrintf( 1 , "\nSetDirectCourseToConsole( int EnemyNum ): Console found: %d-%d.", i , j );
	      if ( DirectLineWalkable( AllEnemys[EnemyNum].pos.x , AllEnemys[EnemyNum].pos.y , i , j , AllEnemys [ EnemyNum ] . pos . z ) )
		{
		  DebugPrintf( 1 , "\nSetDirectCourseToConsole( int EnemyNum ): Walkable is: %d-%d.",
			       i , j );

		  AllEnemys[ EnemyNum ].PrivatePathway[0].x = i;
		  AllEnemys[ EnemyNum ].PrivatePathway[0].y = j;

		  return TRUE;

		  // i=ThisBotsLevel->xlen;
		  // j=ThisBotsLevel->ylen;
		  // break;
		}
	      break;
	      // default:
	      // break;
	    }
	}
    }

  DebugPrintf( 1 , "\nSetDirectCourseToConsole: Ticks used: %d." , SDL_GetTicks() - TicksBefore );

  return FALSE;

}; // int SetDirectCourseToConsole ( int Enemynum )

/* ----------------------------------------------------------------------
 * After an enemy gets hit, it might perform some special behaviour as
 * reaction to the hit.  Perhaps it might just say ouch, perhaps it
 * might do more.  This is done here.
 * ----------------------------------------------------------------------*/
void 
Enemy_Post_Bullethit_Behaviour( int EnemyNum )
{
  Enemy ThisRobot=&AllEnemys[ EnemyNum ];

  DebugPrintf( 1 , "\nEnemy_Post_Bullethit_Behaviour( int EnemyNum ): real function call confirmed.");

  // Since the enemy just got hit, it might as well say so :)
  EnemyHitByBulletText( EnemyNum );

  //--------------------
  // It that is an enemy, who can go and tell his mommy MS at the next console,
  // we will establish a route to the next best console and set the droid to
  // persue this route and make his report.
  //
  if ( Druidmap[ ThisRobot->type ].call_for_help_after_getting_hit ) 
    {
      DebugPrintf( 1 , "\nEnemy_Post_Bullethit_Behaviour( int EnemyNum ): starting to set up special course.");
      
      ThisRobot->persuing_given_course = TRUE;

      if ( SetDirectCourseToConsole( EnemyNum ) == TRUE ) return;
      else 
	{
	  DebugPrintf( 1 , "\nEnemy_Post_Bullethit_Behaviour( int EnemyNum ): giving up way for console....");
	  ThisRobot->persuing_given_course = FALSE;
	}
    }
}; // void Enemy_Post_Bullethit_Behaviour( int Enemynum )


/* ----------------------------------------------------------------------
 * Enemys recover with time, just so.  This is done in this function, and
 * it is of course independent of the current framerate.
 * ---------------------------------------------------------------------- */
void
PermanentHealRobots (void)
{
  int i;
  static float time_since_last_heal=0;
#define HEAL_INTERVAL (3.0)

  //--------------------
  // For performance issues, we won't heal each robot every frame.  Instead it
  // will be enough to heal the bots every HEAL_INTERVAL seconds or something like that.
  //
  time_since_last_heal += Frame_Time() ;
  if ( time_since_last_heal < HEAL_INTERVAL ) return;
  time_since_last_heal = 0 ;

  // for (i = 0; i < MAX_ENEMYS_ON_SHIP; i++)
  for (i = 0; i < Number_Of_Droids_On_Ship; i++)
    {
      if ( AllEnemys [ i ] . Status == OUT)
	continue;
      if ( AllEnemys [ i ] . energy < Druidmap [ AllEnemys [ i ] . type ] . maxenergy )
	AllEnemys[i].energy += Druidmap[AllEnemys [ i ] . type ] . lose_health * HEAL_INTERVAL ;
    }

} // void PermanentHealRobots(void)

/* -----------------------------------------------------------------
 * This function removes all enemy entries from the list of the
 * enemys.
 * ----------------------------------------------------------------- */
void
ClearEnemys ( void )
{
    int i , j;
    enemy* our_bot;

    for ( i = 0 ; i < MAX_ENEMYS_ON_SHIP ; i++ )
    {
	our_bot = & ( AllEnemys [ i ] ) ;

	our_bot -> type = -1;
	our_bot -> pos . z = our_bot -> virt_pos . z = our_bot -> energy = 0;
	our_bot -> nextwaypoint = our_bot -> lastwaypoint = 0;
	our_bot -> Status = OUT;
	our_bot -> pure_wait = 0;
	our_bot -> frozen = 0;
	our_bot -> poison_duration_left = 0;
	our_bot -> poison_damage_per_sec = 0;
	our_bot -> firewait = 0;
	our_bot -> energy = 0;
	our_bot -> SpecialForce = 0;
	our_bot -> AdvancedCommand = 0;
	our_bot -> CompletelyFixed = 0;
	our_bot -> Parameter1 = 0;
	our_bot -> Parameter2 = 0;
	our_bot -> marker = 0;
	our_bot -> is_friendly = 0;
	our_bot -> attack_target_type = ATTACK_TARGET_IS_NOTHING ;
	our_bot -> attack_target_index = (-1) ;
	our_bot -> TextVisibleTime = 0;
	our_bot -> TextToBeDisplayed = "";
	our_bot -> persuing_given_course = FALSE;
	our_bot -> FollowingInflusTail = FALSE;
	our_bot -> StayHowManySecondsBehind = 5;
	
	our_bot -> phase = 0;
	our_bot -> animation_type = WALK_ANIMATION ;
	our_bot -> animation_phase = 0.0 ;
	
	our_bot -> previous_angle = 0 ;         // which angle has this robot been facing the frame before?
	our_bot -> current_angle = 0 ;          // which angle will the robot be facing now?
	our_bot -> last_phase_change = 100 ;      // when did the robot last change his (8-way-)direction of facing
	our_bot -> previous_phase = 0 ;         // which (8-way) direction did the robot face before?
	our_bot -> has_greeted_influencer = FALSE ;   // has this robot issued his first-time-see-the-Tux message?
	our_bot -> will_rush_tux = FALSE ; 
	our_bot -> last_combat_step = 100 ;       // when did this robot last make a step to move in closer or farther away from Tux in combat?      
	for ( j = 0 ; j < MAX_STEPS_IN_GIVEN_COURSE ; j++ )
	{
	    our_bot -> PrivatePathway [ j ] . x = 0 ;
	    our_bot -> PrivatePathway [ j ] . y = 0 ;
	}
    }
}; // void ClearEnemys ( void ) 


/* -----------------------------------------------------------------
 * After a teleporter or lift transition but also after the ship
 * is loaded, the enemys on each deck get shuffled around.
 *
 * WARNING!! IT SHOULD BE NOTED THAT THIS FUNCTION REQUIRES THE
 * CURLEVEL STRUCTURE TO BE SET ALREADY, OR IT WILL SEGFAULT,
 * EVEN WHEN A RETURN IS PLACED AT THE START OF THE FUNCTION!!!!
 *
 * -----------------------------------------------------------------*/
void
ShuffleEnemys ( int LevelNum )
{
  int i, j;
  int nth_enemy;
  int wp_num;
  int wp = 0;
  int BestWaypoint;
  Level ShuffleLevel = curShip.AllLevels[ LevelNum ];

  // get the number of waypoints on CurLevel
  wp_num = ShuffleLevel->num_waypoints;;
  nth_enemy = 0;
  for (i = 0; i < MAX_ENEMYS_ON_SHIP ; i++)
    {
      if ( ( AllEnemys [ i ] . Status == OUT ) || ( AllEnemys [ i ] . pos . z != LevelNum ) )
	continue;		// dont handle dead enemys or on other level 

      AllEnemys [ i ] . persuing_given_course = FALSE; // since position is now completely mixed up,
                                                    // the robot needs to forget about any previous given course.

      if ( AllEnemys [ i ] . CompletelyFixed ) continue;

      //--------------------
      // A special force, that is not completely fixed, needs to be integrated
      // into the waypoint system:  We find the closest waypoint for it and put
      // it simply there.  For simplicity we use sum norm as distance.
      //
      if ( AllEnemys [ i ] . AdvancedCommand == 2 ) TeleportToClosestWaypoint ( &(AllEnemys[i]) );
      if ( AllEnemys [ i ] . SpecialForce )
	{

	  //--------------------
	  // For every special force, that is exactly positioned in the map anyway,
	  // we find the waypoint he's standing on.  That will be his current target
	  // and source waypoint.  That's it for special forces.
	  //
	  BestWaypoint = 0;
	  for ( j=0 ; j<wp_num ; j ++ )
	    {
	      if ( fabsf ( ( ShuffleLevel -> AllWaypoints[j].x  - AllEnemys[i].pos.x ) *
			   ( ShuffleLevel -> AllWaypoints[j].x  - AllEnemys[i].pos.x ) +
		           ( ShuffleLevel -> AllWaypoints[j].y - AllEnemys[i].pos.y ) *
			   ( ShuffleLevel -> AllWaypoints[j].y - AllEnemys[i].pos.y ) ) < 
		   fabsf ( ( ShuffleLevel -> AllWaypoints[ BestWaypoint ].x - AllEnemys[i].pos.x ) *
			   ( ShuffleLevel -> AllWaypoints[ BestWaypoint ].x - AllEnemys[i].pos.x ) +
			   ( ShuffleLevel -> AllWaypoints[ BestWaypoint ].y - AllEnemys[i].pos.y ) *
			   ( ShuffleLevel -> AllWaypoints[ BestWaypoint ].y - AllEnemys[i].pos.y ) ) )
		BestWaypoint = j;
	    }

	  AllEnemys[i].nextwaypoint = BestWaypoint;
	  AllEnemys[i].lastwaypoint = BestWaypoint;
	  
	  AllEnemys[i].pos.x = ShuffleLevel->AllWaypoints[ BestWaypoint ].x;
	  AllEnemys[i].pos.y = ShuffleLevel->AllWaypoints[ BestWaypoint ].y;

	  continue;
	}

      nth_enemy++ ;
      if ( nth_enemy < wp_num )
      {
	  //--------------------
	  // Now if that waypoint is already useful for random-spawning
	  // bots, that's good.  Otherwise we need to skip this waypoint
	  // and try again...
	  //
	  if ( ! ShuffleLevel -> AllWaypoints [ nth_enemy ] . suppress_random_spawn )
	  {
	      wp = nth_enemy;
	  }
	  else
	  {
	      i -- ;
	      continue ;
	  }
      }
      else
	{
	  DebugPrintf (0, "\nNumber of waypoints found: %d." , wp_num );
	  DebugPrintf (0, "\nLess waypoints than enemys on level %d? !", ShuffleLevel->levelnum );
	  Terminate (ERR);
	}

      AllEnemys[i].pos.x = ShuffleLevel->AllWaypoints[wp].x;
      AllEnemys[i].pos.y = ShuffleLevel->AllWaypoints[wp].y;

      AllEnemys[i].lastwaypoint = wp;
      AllEnemys[i].nextwaypoint = wp;

    } // for (MAX_ENEMYS_ON_SHIP) 

}; // void ShuffleEnemys ( void ) 

/* ----------------------------------------------------------------------
 *
 * There are some rather time-consuming functions and loops dealing with
 * the enemies on one level.  However, stepping through *all* the bots
 * in *all* the game can be rather cumbersome, especially if repeated so
 * often.  Therefore it might be sensible to keep some information on
 * where inside the AllEnemys array the bots of one particular level are
 * located.  
 * 
 * Bots don't change level very much, so it should also be sufficient to
 * update this list of array locations only now and then.
 *
 * ---------------------------------------------------------------------- */
void
occasionally_update_first_and_last_bot_indices ( void )
{
    static int first_call = TRUE ;
    static int last_update_ticks ;
    int current_ticks;
    int level_num;
    int i;
    
    if ( first_call )
    {
	first_call = FALSE ;
	last_update_ticks = SDL_GetTicks() - 1000 ;
    }
    
    current_ticks = SDL_GetTicks();
    
    if ( current_ticks - last_update_ticks >= 1000 )
    {
	last_update_ticks = current_ticks ;
	DebugPrintf ( 2 , "\nNow updating the first and last indices of bots from each level." );
	
	for ( level_num = 0 ; level_num < MAX_LEVELS ; level_num ++ )
	{
	    //--------------------
	    // Some entries, that will be overridden shortly...
	    first_index_of_bot_on_level [ level_num ] = MAX_ENEMYS_ON_SHIP - 2 ;
	    last_index_of_bot_on_level [ level_num ] = 0 ;
	    
	    for ( i = 0 ; i < MAX_ENEMYS_ON_SHIP ; i ++ )
	    {
		if ( AllEnemys [ i ] . type == (-1) ) continue ;
		if ( AllEnemys [ i ] . pos . z != level_num ) continue ;
		
		if ( first_index_of_bot_on_level [ level_num ] >= i )
		    first_index_of_bot_on_level [ level_num ] = i ;
		if ( last_index_of_bot_on_level [ level_num ] <= i )
		    last_index_of_bot_on_level [ level_num ] = i ;
	    }
	    
	    DebugPrintf ( 2 , "\nFirst index on lv. %d is %d, last index is %d. " , level_num , 
			  first_index_of_bot_on_level [ level_num ] , last_index_of_bot_on_level [ level_num ] );
	}
    }
    
}; // occasionally_update_first_and_last_bot_indices ( void )

/* ----------------------------------------------------------------------
 * This function checks if the connection between two points is free of
 * droids.  
 *
 * MAP TILES ARE NOT TAKEN INTO CONSIDERATION, ONLY DROIDS!!!
 *
 * ---------------------------------------------------------------------- */
int 
CheckIfWayIsFreeOfDroidsWithTuxchecking ( float x1 , float y1 , float x2 , float y2 , int OurLevel , 
					  Enemy ExceptedRobot ) 
{
    float LargerDistance;
    int Steps;
    int i, j;
    moderately_finepoint step;
    moderately_finepoint CheckPosition;
    enemy* this_enemy;
    static int first_call = TRUE ;
    static float steps_per_square;

    //--------------------
    // Upon the very first function call, we calibrate the density of steps, so that 
    // we cannot miss out a droid by stepping right over it.
    //
    if ( first_call )
    {
	first_call = FALSE ;
	steps_per_square = 1 / ( 2.0 * sqrt(2.0) * Druid_Radius_X );
    }
    
    // DebugPrintf( 2, "\nint CheckIfWayIsFreeOfDroids (...) : Checking from %d-%d to %d-%d.", (int) x1, (int) y1 , (int) x2, (int) y2 );
    // fflush(stdout);
    
    if ( fabsf ( x1 - x2 ) > fabsf ( y1 - y2 ) ) LargerDistance = fabsf ( x1 - x2 );
    else LargerDistance = fabsf ( y1 - y2 );
    
    Steps = LargerDistance * steps_per_square + 1 ;   // We check four times on each map tile...
    // if ( Steps == 0 ) return TRUE;
    
    // We determine the step size when walking from (x1,y1) to (x2,y2) in Steps number of steps
    step.x = ( x2 - x1 ) / ((float)Steps) ;
    step.y = ( y2 - y1 ) / ((float)Steps) ;
    
    // DebugPrintf( 2 , "\nint CheckIfWayIsFreeOfDroids (...) :  step.x=%f step.y=%f." , step.x , step.y );
    
    // We start from position (x1, y1)
    CheckPosition . x = x1;
    CheckPosition . y = y1;
    
    for ( i = 0 ; i < Steps + 1 ; i++ )
    {
	// for ( j = 0 ; j < MAX_ENEMYS_ON_SHIP ; j ++ )
	// for ( j = 0 ; j < Number_Of_Droids_On_Ship ; j ++ )
	for ( j  = first_index_of_bot_on_level [ OurLevel ] ; 
	      j <=  last_index_of_bot_on_level [ OurLevel ] ; j ++ )
	{
	    this_enemy = & ( AllEnemys [ j ] ) ;
	    if ( this_enemy -> pos.z != OurLevel ) continue;
	    if ( this_enemy -> Status == OUT ) continue;
	    if ( this_enemy -> energy <= 0 ) continue;
	    if ( this_enemy -> pure_wait > 0 ) continue;
	    if ( this_enemy == ExceptedRobot ) continue;
	    
	    // so it seems that we need to test this one!!
	    if ( ( fabsf ( this_enemy -> pos . x - CheckPosition . x ) < 2.0 * Druid_Radius_X ) &&
		 ( fabsf ( this_enemy -> pos . y - CheckPosition . y ) < 2.0 * Druid_Radius_Y ) ) 
	    {
		// DebugPrintf( 2, "\nCheckIfWayIsFreeOfDroids (...) : Connection analysis revealed : TRAFFIC-BLOCKED !");
		return FALSE;
	    }
	}
	
	//--------------------
	// Now we check for collisions with the Tux himself
	//
	if ( ( fabsf ( Me [ 0 ] . pos.x - CheckPosition.x ) < 2 * Druid_Radius_X ) &&
	     ( fabsf ( Me [ 0 ] . pos.y - CheckPosition.y ) < 2 * Druid_Radius_Y ) ) 
	{
	    // DebugPrintf( 2 , "\nCheckIfWayIsFreeOfDroids (...) : Connection analysis revealed : TRAFFIC-BLOCKED-INFLUENCER !");
	    return FALSE;
	}
	
	CheckPosition . x += step . x;
	CheckPosition . y += step . y;
    }
    
    return TRUE;

}; // CheckIfWayIsFreeOfDroids ( float x1 , float y1 , float x2 , float y2 , int OurLevel , int ExceptedDroid )


/* ----------------------------------------------------------------------
 * This function checks if the connection between two points is free of
 * droids.  
 *
 * MAP TILES ARE NOT TAKEN INTO CONSIDERATION, ONLY DROIDS!!!
 *
 * ---------------------------------------------------------------------- */
int 
CheckIfWayIsFreeOfDroidsWithoutTuxchecking ( float x1 , float y1 , float x2 , float y2 , int OurLevel , 
					     Enemy ExceptedRobot ) 
{
  float LargerDistance;
  int Steps;
  int i, j;
  moderately_finepoint step;
  moderately_finepoint CheckPosition;
  enemy* this_enemy;
  static int first_call = TRUE ;
  static float steps_per_square;

  //--------------------
  // Upon the very first function call, we calibrate the density of steps, so that 
  // we cannot miss out a droid by stepping right over it.
  //
  if ( first_call )
    {
      first_call = FALSE ;
      steps_per_square = 1 / ( 2.0 * sqrt(2.0) * Druid_Radius_X );
    }

  // DebugPrintf( 2, "\nint CheckIfWayIsFreeOfDroids (...) : Checking from %d-%d to %d-%d.", (int) x1, (int) y1 , (int) x2, (int) y2 );
  // fflush(stdout);

  if ( fabsf ( x1 - x2 ) > fabsf ( y1 - y2 ) ) LargerDistance = fabsf ( x1 - x2 );
  else LargerDistance = fabsf ( y1 - y2 );

  Steps = LargerDistance * steps_per_square + 1 ;   // We check four times on each map tile...
  // if ( Steps == 0 ) return TRUE;

  // We determine the step size when walking from (x1,y1) to (x2,y2) in Steps number of steps
  step.x = ( x2 - x1 ) / ((float)Steps) ;
  step.y = ( y2 - y1 ) / ((float)Steps) ;

  // DebugPrintf( 2 , "\nint CheckIfWayIsFreeOfDroids (...) :  step.x=%f step.y=%f." , step.x , step.y );

  // We start from position (x1, y1)
  CheckPosition . x = x1;
  CheckPosition . y = y1;

  for ( i = 0 ; i < Steps + 1 ; i++ )
    {
      // for ( j = 0 ; j < MAX_ENEMYS_ON_SHIP ; j ++ )
      // for ( j = 0 ; j < Number_Of_Droids_On_Ship ; j ++ )
      for ( j  = first_index_of_bot_on_level [ OurLevel ] ; 
	    j <=  last_index_of_bot_on_level [ OurLevel ] ; j ++ )
	{
	  this_enemy = & ( AllEnemys [ j ] ) ;
	  if ( this_enemy -> pos.z != OurLevel ) continue;
	  if ( this_enemy -> Status == OUT ) continue;
	  if ( this_enemy -> energy <= 0 ) continue;
	  if ( this_enemy -> pure_wait > 0 ) continue;
	  if ( this_enemy == ExceptedRobot ) continue;

	  // so it seems that we need to test this one!!
	  if ( ( fabsf ( this_enemy -> pos . x - CheckPosition . x ) < 2.0 * Druid_Radius_X ) &&
	       ( fabsf ( this_enemy -> pos . y - CheckPosition . y ) < 2.0 * Druid_Radius_Y ) ) 
	    {
	      // DebugPrintf( 2, "\nCheckIfWayIsFreeOfDroids (...) : Connection analysis revealed : TRAFFIC-BLOCKED !");
	      return FALSE;
	    }
	}

      CheckPosition . x += step . x;
      CheckPosition . y += step . y;
    }

  return TRUE;
}; // CheckIfWayIsFreeOfDroidsWithoutTuxchecking ( float x1 , float y1 , float x2 , float y2 , int OurLevel , ExceptedDroid )


/* ----------------------------------------------------------------------
 * Once the next waypoint or the next private pathway point has been 
 * selected, this generic low_level movement function can be called to
 * approach either the waypoint or seletect private pathway point.
 * ---------------------------------------------------------------------- */
void
RawEnemyApproachPosition ( Enemy ThisRobot , finepoint next_target_spot )
{
    finepoint remaining_way;
    float maxspeed;
    
    //--------------------
    // According to properties of the robot like being frozen or not,
    // we define the maximum speed of this machine for later use...
    // A frozen robot is slow while a paralyzed robot can do absolutely nothing.
    //
    // if ( ThisRobot -> paralysation_duration_left != 0 ) return;
    
    if ( ThisRobot -> frozen == 0 )
	maxspeed = Druidmap [ ThisRobot -> type ] . maxspeed;
    else 
	maxspeed = 0.2 * Druidmap [ ThisRobot->type ] . maxspeed;
    
    //--------------------
    // While getting hit, the bot or person shouldn't be running, but
    // when standing, it should move over to the 'walk' animation type...
    //
    if ( ThisRobot -> animation_type == GETHIT_ANIMATION ) return;
    if ( ThisRobot -> animation_type == STAND_ANIMATION ) 
    {
	ThisRobot -> animation_type = WALK_ANIMATION ;
	ThisRobot -> animation_phase = 0.0 ;

	if ( ( ThisRobot -> Status == OUT ) || ( ThisRobot -> animation_type == DEATH_ANIMATION ) )
	{
	    DebugPrintf ( -4 , "\n%s(): WARNING: animation phase reset for OUT bot... " , __FUNCTION__ );
	}
    }
    
    //--------------------
    // Now that we have found out where to go, we can start to determine the remaining 
    // way until the target point is reached.
    //
    remaining_way . x = next_target_spot . x - ThisRobot -> pos . x ;
    remaining_way . y = next_target_spot . y - ThisRobot -> pos . y ;
    
    if ( ( ThisRobot -> pos . z != Me [ 0 ] . pos . z ) &&
	 ( ThisRobot -> combat_state == MAKE_ATTACK_RUN ) )
    {
	// DebugPrintf ( -4 , "\n%s(): moving enemy on remote level... " , __FUNCTION__ );
	// DebugPrintf ( -4 , "\n%s(): remaining_way: x=%f, y=%f." , __FUNCTION__ ,
	// remaining_way . x , remaining_way . y );
    }


    //--------------------
    // As long a the distance from the current position of the enemy
    // to its next wp is large, movement is rather simple:
    //
    if ( fabsf ( remaining_way . x ) > Frame_Time() * maxspeed )
    {
	ThisRobot -> speed . x =
	    ( remaining_way . x / fabsf ( remaining_way . x ) ) * maxspeed;
	ThisRobot -> pos . x += ThisRobot -> speed . x * Frame_Time ();
    } 	 
    else
    {
	// --------------------
	// Once this enemy is close to his final destination waypoint, we have
	// to do some fine tuning, and then of course set the next waypoint.
	ThisRobot -> pos . x = next_target_spot . x;
	ThisRobot -> speed . x = 0;
    }
    
    if ( fabsf ( remaining_way . y )  > Frame_Time() * maxspeed )
    {
	ThisRobot -> speed . y =
	    ( remaining_way . y / fabsf ( remaining_way . y ) ) * maxspeed;
	ThisRobot -> pos . y += ThisRobot -> speed . y * Frame_Time ();
    }
    else
    {
	ThisRobot -> pos . y = next_target_spot . y ;
	ThisRobot -> speed . y = 0;
    }
    
    //--------------------
    // Now the bot is moving, so maybe it's moving over a jump threshold?
    // In any case, it might be best to check...
    //
    adapt_position_for_jump_thresholds ( & ( ThisRobot -> pos ) , & ( ThisRobot -> pos ) );
    


    //--------------------
    // Since this robot is moving, we set the animation type to 
    // walking...
    //
    // ThisRobot -> animation_type = WALK_ANIMATION ; 
    // if ( ThisRobot -> animation_phase == 0 )
    // ThisRobot -> animation_phase = 0.1 ;
    
}; // void RawEnemyApproachPosition ( Enemy ThisRobot , finepoint next_target_spot )

/* ----------------------------------------------------------------------
 * This function moves one robot thowards his next waypoint.  If already
 * there, the function does nothing more.
 * ---------------------------------------------------------------------- */
void 
MoveThisRobotThowardsHisCurrentTarget ( int EnemyNum )
{
    Waypoint WpList;		/* Pointer to waypoint-list */
    finepoint nextwp_pos;
    Enemy ThisRobot = & ( AllEnemys [ EnemyNum ] ) ;
    int HistoryIndex;
    Level WaypointLevel = curShip.AllLevels[ AllEnemys[ EnemyNum ].pos.z ];
    
    // DebugPrintf( 2 , "\n%s(): real function call confirmed. " , __FUNCTION__ );
    
    // We do some definitions to save us some more typing later...
    WpList = WaypointLevel -> AllWaypoints;
    
    //--------------------
    // According to properties of the robot like being frozen or not,
    // we define the maximum speed of this machine for later use...
    // A frozen robot is slow while a paralyzed robot can do absolutely nothing.
    //
    if ( ThisRobot -> paralysation_duration_left != 0 ) return;

    //--------------------
    // We determine our movement target, either the preset course or the 
    // current classical waypoint that has been set.
    //
    if ( ThisRobot -> persuing_given_course )
    {
	nextwp_pos . x = ThisRobot -> PrivatePathway [ 0 ] . x ;
	nextwp_pos . y = ThisRobot -> PrivatePathway [ 0 ] . y ;
    }
    else
    {
	nextwp_pos . x = WpList [ ThisRobot -> nextwaypoint ] . x + 0.5 ;
	nextwp_pos . y = WpList [ ThisRobot -> nextwaypoint ] . y + 0.5 ;
    }
    
    //--------------------
    // Maybe this robot is following behind influ.  Then of course we need to set
    // even different special target positions...
    //
    if ( ThisRobot -> FollowingInflusTail == TRUE )
    {
	if ( ( fabsf( ThisRobot->pos.x - Me[0].pos.x ) > 1 ) || 
	     ( fabsf( ThisRobot->pos.y - Me[0].pos.y ) > 1 ) )
	{
	    
	    HistoryIndex = ThisRobot->StayHowManyFramesBehind;
	    
	    nextwp_pos.y = GetInfluPositionHistoryY( HistoryIndex );
	    nextwp_pos.x = GetInfluPositionHistoryX( HistoryIndex );
	    // jump to the next level, if the influencer did
	    // that might cause some inconsistencies, but who cares right now?
	    if ( ThisRobot->pos.z != GetInfluPositionHistoryZ( HistoryIndex ) )
	    {
		ThisRobot->pos.x = GetInfluPositionHistoryX( HistoryIndex );
		ThisRobot->pos.y = GetInfluPositionHistoryY( HistoryIndex );
		ThisRobot->pos.z = GetInfluPositionHistoryZ( HistoryIndex );
	    }
	}
	else
	{
	    nextwp_pos.y = ThisRobot->pos.y;
	    nextwp_pos.x = ThisRobot->pos.x;
	}
    }
    
    RawEnemyApproachPosition ( ThisRobot , nextwp_pos );

}; // void MoveThisRobotThowardsHisCurrentTarget ( int EnemyNum )

/* ----------------------------------------------------------------------
 * When droids are walking along random WAYPOINTS (i.e. never leave the
 * system of wayponts an the connections between them, it may occasionally
 * become nescessary to select a new randomly chosen waypoint that is 
 * reachable directly from the current waypoint.
 * Doing this selection is what this function does.
 * ---------------------------------------------------------------------- */
void
RawSetNewRandomWaypoint ( Enemy ThisRobot )
{
    int i;
    Waypoint WpList;	
    int nextwp;
    finepoint nextwp_pos;
    waypoint *this_wp;
    int num_conn;
    int trywp = 0 ;
    int FreeWays[ MAX_WP_CONNECTIONS ];
    int SolutionFound;
    int TestConnection;
    Level WaypointLevel = curShip.AllLevels[ ThisRobot -> pos.z ];
    
    DebugPrintf( 2 , "\nvoid RawSetNewRandomWaypoint ( Enemy ThisRobot ): real function call confirmed. ");
    
    //--------------------
    // We do some definitions to save us some more typing later...
    //
    WpList = WaypointLevel->AllWaypoints;
    nextwp = ThisRobot->nextwaypoint;
    nextwp_pos.x = WpList[nextwp].x + 0.5 ;
    nextwp_pos.y = WpList[nextwp].y + 0.5 ;
    
    ThisRobot->lastwaypoint = ThisRobot->nextwaypoint;
    
    // search for possible connections from here...
    DebugPrintf ( 2 , "\nRawSetNewRandomWaypoint ( Enemy ThisRobot ): searching for possible connections...");
    
    // search for the first connection, that doesn't exist any more, so
    // that we know, which connections surely do exist
    this_wp = &WpList[nextwp];
    num_conn = this_wp->num_connections;
    
    // Of course, only if such connections exist at all, we do the
    // following change of target waypoint procedure
    if (  num_conn == 0 ) // no connections found!
    {
	fprintf ( stderr , "\nThe offending waypoint nr. is: %d.", nextwp );
	fprintf ( stderr , "\nThe map level in question got nr.: %d.", ThisRobot -> pos . z );
	if ( ThisRobot -> stick_to_waypoint_system_by_default )
	{
	    GiveStandardErrorMessage ( __FUNCTION__  , "\
There was a droid on a waypoint, that apparently has no connections to other waypoints...\n\
Since it was a waypoint-based bot, this is a fatal message in this case.",
				       NO_NEED_TO_INFORM, IS_FATAL );
	}
	else
	{
	    GiveStandardErrorMessage ( __FUNCTION__  , "\
There was a droid on a waypoint, that apparently has no connections to other waypoints...\n\
Since it was NOT a waypoint-based bot, this is NOT a fatal message in this case.",
				       NO_NEED_TO_INFORM, IS_WARNING_ONLY );
	}
    }
    
    //--------------------
    // At this point, we should check, if there is another waypoint 
    // and also if the way there is free of other droids
    //
    for ( i = 0; i < num_conn ; i++ )
    {
	FreeWays [ i ] = CheckIfWayIsFreeOfDroidsWithTuxchecking ( 
	    WpList [ ThisRobot -> lastwaypoint ] . x + 0.5 , 
	    WpList [ ThisRobot -> lastwaypoint ] . y + 0.5 , 
	    WpList [ WpList [ ThisRobot -> lastwaypoint ] . connections [ i ] ] . x + 0.5 , 
	    WpList [ WpList [ ThisRobot -> lastwaypoint ] . connections [ i ] ] . y + 0.5 , 
	    ThisRobot->pos.z , ThisRobot );
    }
    
    //--------------------
    // Now see whether any way point at all is free in that sense
    // otherwise we set this robot to waiting and return;
    //
    for ( i = 0 ; i < num_conn ; i++ )
    {
	if ( FreeWays[i] ) break;
    }
    if ( i == num_conn )
    {
	DebugPrintf( 2 , "\n%s(): Sorry, there seems no free way out.  I'll wait then... , num_conn was : %d ." , __FUNCTION__ , num_conn );
	ThisRobot->pure_wait = 1.5 ; // this makes this droid 'passable' for other droids for now...
	if ( ( ThisRobot -> combat_state == MOVE_ALONG_RANDOM_WAYPOINTS ) ||
	     ( ThisRobot -> combat_state == TURN_THOWARDS_NEXT_WAYPOINT ) )
	    ThisRobot -> combat_state = WAIT_AND_TURN_AROUND_AIMLESSLY ;
	return;
    }
    
    //--------------------
    // Now that we know, there is some way out of this, we can test around
    // and around randomly until we finally find some solution.
    //
    // ThisRobot->nextwaypoint = WpList [ nextwp ] . connections [ i ] ;
    // 
    SolutionFound = FALSE;
    while ( !SolutionFound )
    {
	TestConnection = MyRandom ( WpList [ nextwp ] . num_connections - 1 );
	
	if ( WpList[nextwp].connections[ TestConnection ] == (-1) ) continue;
	if ( !FreeWays[TestConnection] ) continue;
	
	trywp = WpList[nextwp].connections[ TestConnection ];
	SolutionFound = TRUE;
    }
    
    // set new waypoint...
    ThisRobot->nextwaypoint = trywp;
    
    DebugPrintf ( 2 , "\n%s():  A new waypoint has been set." , __FUNCTION__ );
  
}; // void RawSetNewRandomWaypoint ( Enemy ThisRobot )

/* ----------------------------------------------------------------------
 * This function moves one robot in an advanced way, that hasn't been
 * present within the classical paradroid game.
 * ---------------------------------------------------------------------- */
void 
SelectNextWaypointAdvanced ( int EnemyNum )
{
    Enemy ThisRobot=&AllEnemys[ EnemyNum ];

    DebugPrintf( 2 , "\nvoid SelectNextWaypointAdvanced ( int EnemyNum ) : real function call confirmed. ");

    //--------------------
    // Maybe currently we do not stick to the whole waypoint system but rather 
    // choose our course independently.  Then it's PersueGivenCourse.  Otherwise
    // we select waypoints as we're used to...
    //
    if ( ThisRobot->persuing_given_course == TRUE ) return;
    
    RawSetNewRandomWaypoint ( ThisRobot );

}; // void SelectNextWaypointAdvanced ( int EnemyNum )

/* ----------------------------------------------------------------------
 * This function is supposed to find out if a given line on an 
 * arbitrarily chosen map can be walked by a bot or not.
 * ---------------------------------------------------------------------- */
int
droid_can_walk_this_line ( int level_num , float x1, float y1 , float x2 , float y2 )
{
    global_ignore_doors_for_collisions_flag = TRUE ;
    if ( DirectLineWalkable ( x1 , y1 , x2 , y2 , level_num ) )
	return ( TRUE );
    else
	return ( FALSE ); 
    global_ignore_doors_for_collisions_flag = FALSE ;
}; // int droid_can_walk_this_line ( int level_num , float x1, float y1 , float x2 , float y2 )

/* ----------------------------------------------------------------------
 * If the droid in question is currently not following the waypoint system
 * but rather moving around on it's own and without any real destination,
 * this function sets up randomly chosen targets for the droid.
 * ---------------------------------------------------------------------- */
void 
select_new_waypointless_random_walk_target ( int EnemyNum )
{
    enemy* ThisRobot = & ( AllEnemys [ EnemyNum ] ) ;
    int i ;
    moderately_finepoint target_candidate;
    int success = FALSE ;

    // Level WaypointLevel = curShip.AllLevels[ ThisRobot -> pos.z ];
#define MAX_RANDOM_WALK_ATTEMPTS_BEFORE_GIVING_UP 4

    // DebugPrintf( -4 , "\n%s(): real function call confirmed. " , __FUNCTION__ );
    
    for ( i = 0 ; i < MAX_RANDOM_WALK_ATTEMPTS_BEFORE_GIVING_UP ; i ++ )
    {
	//--------------------
	// We select a possible new walktarget for this bot, not too
	// far away from the current position...
	//
	target_candidate . x = ThisRobot -> pos . x + ( MyRandom ( 600 ) - 300 ) / 100 ; 
	target_candidate . y = ThisRobot -> pos . y + ( MyRandom ( 600 ) - 300 ) / 100 ; 

	if ( droid_can_walk_this_line ( ThisRobot -> pos . z , ThisRobot -> pos . x , ThisRobot -> pos . y , 
					target_candidate . x , target_candidate . y ) )
	{
	    ThisRobot -> persuing_given_course = TRUE ;
	    ThisRobot -> PrivatePathway [ 0 ] . x = target_candidate . x ;
	    ThisRobot -> PrivatePathway [ 0 ] . y = target_candidate . y ;
	    success = TRUE ;
	    break;
	}
    }

    if ( ! success )
    {
	DebugPrintf ( 1 , "\n%s():  bad luck with random walk point this time..." , __FUNCTION__ );
	ThisRobot -> pure_wait = 1.6 ;
	
    }
    else
    {
	// DebugPrintf ( -4 , "\n%s():  A new waypoint has been set." , __FUNCTION__ );
    }
    
}; // void select_new_waypointless_random_walk_target ( int EnemyNum )

/* ----------------------------------------------------------------------
 * This function moves one robot in an advanced way, that hasn't been
 * present within the classical paradroid game.
 * ---------------------------------------------------------------------- */
float 
remaining_distance_to_current_walk_target ( Enemy ThisRobot )
{
    Waypoint WpList;
    int nextwp;
    finepoint nextwp_pos;
    Level WaypointLevel = curShip.AllLevels[ ThisRobot -> pos.z ];
    finepoint remaining_way;

    //--------------------
    // Maybe currently we do not stick to the whole waypoint system but rather 
    // choose our course independently.  Then it's PersueGivenCourse.  Otherwise
    // we select waypoints as we're used to...
    //
    if ( ThisRobot -> persuing_given_course )
    {
	remaining_way.x = ThisRobot -> PrivatePathway [ 0 ] . x - ThisRobot -> pos . x ;
	remaining_way.y = ThisRobot -> PrivatePathway [ 0 ] . y - ThisRobot -> pos . y ;
    }
    else
    {
	WpList = WaypointLevel -> AllWaypoints;
	nextwp = ThisRobot -> nextwaypoint;
	nextwp_pos.x = WpList[nextwp].x + 0.5 ;
	nextwp_pos.y = WpList[nextwp].y + 0.5 ;

	// determine the remaining way until the target point is reached
	remaining_way.x = nextwp_pos.x - ThisRobot->pos.x;
	remaining_way.y = nextwp_pos.y - ThisRobot->pos.y;
    }

  return ( sqrt ( remaining_way.x * remaining_way.x + remaining_way.y * remaining_way.y ) ) ;

}; // float remaining_distance_to_current_walk_target ( Enemy ThisRobot )

/* ----------------------------------------------------------------------
 * This function tells if a given level is active in the sence that there
 * is one ore more player character on the level, so that need exists to
 * move all the enemies on this level etc.
 * ---------------------------------------------------------------------- */
int 
IsActiveLevel ( int levelnum ) 
{
    int PlayerNum;

    //--------------------
    // Now we check for alive players on this level, and if we find
    // some, the level is an 'active' level.
    //
    for ( PlayerNum = 0 ; PlayerNum < MAX_PLAYERS ; PlayerNum ++ )
    {
	if ( Me [ PlayerNum ] . status == OUT ) continue;
	if ( Me [ PlayerNum ] . pos . z == levelnum ) return TRUE;
    }
    
    //--------------------
    // But if we didn't find any, then the level is not 'active'.
    //
    return FALSE;

}; // int IsActiveLevel ( int levelnum ) 

/* ----------------------------------------------------------------------
 * When a (hostile) robot is defeated and explodes, it will drop some 
 * treasure, i.e. stuff it had or parts that it consisted of or similar
 * things.  Maybe there will even be some extra magical treasures if the
 * robot in question was a 'boss monster'.  This function does the 
 * treasure dropping.
 * ---------------------------------------------------------------------- */
void
DropEnemyTreasure ( Enemy ThisRobot )
{

    //--------------------
    // If the Tux has the skill to extract certain components from dead bots,
    // these components will be thrown out automatically, when the bot is killed.
    //
    if ( Me [ 0 ] . base_skill_level [ SPELL_EXTRACT_PLASMA_TRANSISTORS ] )
    {
	if ( Druidmap [ ThisRobot->type ] . amount_of_plasma_transistors )
	    DropItemAt( ITEM_DROID_PART_1 , ThisRobot -> pos . z , 
			ThisRobot->virt_pos.x , ThisRobot->virt_pos.y , -1 , -1 , 2 , 1 );
    }
    if ( Me [ 0 ] . base_skill_level [ SPELL_EXTRACT_SUPERCONDUCTORS ] )
    {
	if ( Druidmap [ ThisRobot->type ] . amount_of_superconductors )
	    DropItemAt( ITEM_DROID_PART_2 , ThisRobot -> pos . z , 
			ThisRobot->virt_pos.x , ThisRobot->virt_pos.y , -1 , -1 , 2 , 1 );
    }
    if ( Me [ 0 ] . base_skill_level [ SPELL_EXTRACT_ANTIMATTER_CONVERTERS ] )
    {
	if ( Druidmap [ ThisRobot->type ] . amount_of_antimatter_converters )
	    DropItemAt( ITEM_DROID_PART_3 , ThisRobot -> pos . z , 
			ThisRobot->virt_pos.x , ThisRobot->virt_pos.y , -1 , -1 , 2 , 1 );
    }
    if ( Me [ 0 ] . base_skill_level [ SPELL_EXTRACT_ENTROPY_INVERTERS ] )
    {
	if ( Druidmap [ ThisRobot->type ] . amount_of_entropy_inverters )
	    DropItemAt( ITEM_DROID_PART_4 , ThisRobot -> pos . z , 
			ThisRobot->virt_pos.x , ThisRobot->virt_pos.y , -1 , -1 , 2 , 1 );
    }
    if ( Me [ 0 ] . base_skill_level [ SPELL_EXTRACT_TACHYON_CONDENSATORS ] ) 
    {
	if ( Druidmap [ ThisRobot->type ] . amount_of_tachyon_condensators )
	    DropItemAt( ITEM_DROID_PART_5 , ThisRobot -> pos . z , 
			ThisRobot->virt_pos.x , ThisRobot->virt_pos.y , -1 , -1 , 2 , 1 );
    }
    if ( ThisRobot -> on_death_drop_item_code != (-1) )
    {
	DropItemAt( ThisRobot -> on_death_drop_item_code , ThisRobot -> pos . z , 
		    ThisRobot -> virt_pos . x , 
		    ThisRobot -> virt_pos . y , -1 , -1 , 2 , 1 );
    }  
    
    //--------------------
    // Apart from the parts, that the Tux might be able to extract from the bot,
    // there is still some chance, that the enemy will have (and drop) some other
    // valuables, that the Tux can then collect afterwards.
    //
    DropRandomItem ( ThisRobot -> pos . z , ThisRobot -> virt_pos . x , ThisRobot -> virt_pos . y , Druidmap [ ThisRobot -> type ] . monster_level , FALSE , FALSE , FALSE );
    

}; // void DropEnemyTreasure ( Enemy ThisRobot )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
int
MakeSureEnemyIsInsideHisLevel ( Enemy ThisRobot )
{
    //--------------------
    // If the enemy is outside of the current map, 
    // that's an error and needs to be correted.
    //
    if ( ( ThisRobot -> pos . x <= 0 ) || 
	 ( ThisRobot -> pos . x >= curShip . AllLevels [ ThisRobot -> pos . z ] -> xlen ) ||
	 ( ThisRobot -> pos . y <= 0 ) || 
	 ( ThisRobot -> pos . y >= curShip . AllLevels [ ThisRobot -> pos . z ] -> ylen ) )
    {
	
	GiveStandardErrorMessage ( __FUNCTION__  , "\
There was a droid found outside the bounds of this level.\n\
This is an error and should not occur, but most likely it does since\n\
the bots are allowed some motion without respect to existing waypoints\n\
in Freedroid RPG.\n\
The offending bot will be deleted silently.",
				   NO_NEED_TO_INFORM, IS_WARNING_ONLY );
	ThisRobot -> type = (-1) ;
	ThisRobot -> Status = (OUT) ;
	
	//--------------------
	// This droid must not be blitted!!
	//
	return ( FALSE );
	// Terminate(ERR);
    }

    return ( TRUE );

}; // int MakeSureEnemyIsInsideThisLevel ( int Enum )

/* ----------------------------------------------------------------------
 * When a robot has reached energy <= 1, then this robot will explode and
 * die, lose some treasure and add up to the kill record of the Tux.  All
 * the things that should happen when energy is that low are handled here
 * while the check for low energy is done outside of this function namely
 * somewhere in the movement processing for this enemy.
 * ---------------------------------------------------------------------- */
void 
InitiateDeathOfEnemy ( Enemy ThisRobot )
{

    DebugPrintf ( 1 , "\n%s():  another death of a bot/human initiated..." , __FUNCTION__ );

    //--------------------
    // If the Tux has killed his friend, he will regret it, or at least
    // say so.
    //
    if ( ThisRobot->is_friendly )
    {
	Activate_Conservative_Frame_Computation();
	PlayOnceNeededSoundSample( "Tux_Why_Did_I_0.wav" , FALSE , TRUE );
    }
    else
    {
	//--------------------
	// The Tux gains experience from this, only for non-friends
	//
	Me [ 0 ] . Experience += Druidmap[ ThisRobot->type ].experience_reward;
    }
    
    ThisRobot->Status = OUT;
    
    //--------------------
    // The dead enemy will now explode and drop treasure, provided that 
    // it was still on this map
    //
    if ( MakeSureEnemyIsInsideHisLevel ( ThisRobot ) ) 
    {
	Me [ 0 ] . KillRecord [ ThisRobot -> type ] ++ ;
	//--------------------
	// Maybe that robot did have something with him?  The item should then
	// fall to the floor with it's clanc
	//
	// Maybe the robots was also a boss monster.  Then some additional items
	// must be dropped and they must always be magical.
	//
	DropEnemyTreasure ( ThisRobot ) ;

	//--------------------
	// Maybe this robot is already fully animated or has at least one
	// 'death' image.  Then we'll activate it.
	//
	if ( Druidmap [ ThisRobot -> type ] . use_image_archive_file )
	{

	    //--------------------
	    // NOTE:  We reset the animation phase to the first death animation image
	    //        here.  But this may be WRONG!  In the case that the enemy graphics
	    //        hasn't been loaded yet, this will result in '1' for the animation
	    //        phase.  That however is unlikely to happen unless the bot is killed
	    //        right now and hasn't been ever visible in the game yet.  Also it
	    //        will lead only to minor 'prior animation' before the real death
	    //        phase is reached and so serious bugs other than that, so I think it 
	    //        will be tolerable this way.
	    //
	    ThisRobot -> animation_phase = ( ( float ) first_death_animation_image [ ThisRobot -> type ] ) - 1 + 0.1 ;
	    ThisRobot -> animation_type = DEATH_ANIMATION;
	    play_death_sound_for_bot ( ThisRobot );
	}
	else
	{
	    StartBlast ( ThisRobot->pos.x , ThisRobot->pos.y , ThisRobot->pos.z , DRUIDBLAST );
	}
	
	//--------------------
	// And, not that the enemy is dead, some more blood will be spilled...
	//
	enemy_spray_blood ( ThisRobot ) ;
	enemy_spray_blood ( ThisRobot ) ;
	enemy_spray_blood ( ThisRobot ) ;
	
    }
  
}; // void InitiateDeathOfEnemy ( Enemy ThisRobot )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
DetermineAngleOfFacing ( int Enum )
{
  //--------------------
  // The phase now depends upon the direction this robot
  // is heading.
  //
  // We calsulate the angle of the vector, but only if the robot has at least
  // some minimal speed.  If not, simply the previous angle will be used again.
  //
  if ( ( fabsf ( AllEnemys[Enum].speed.y ) > 0.03 ) || ( fabsf ( AllEnemys[Enum].speed.x ) > 0.03 ) )
    {
      AllEnemys [ Enum ] . current_angle = 180 - ( atan2 ( AllEnemys[Enum].speed.y,  AllEnemys[Enum].speed.x) * 180 / M_PI + 90 );
      AllEnemys[Enum].previous_angle = AllEnemys [ Enum ] . current_angle ;
    }
  else
    {
      AllEnemys[ Enum ] . current_angle = AllEnemys[Enum].previous_angle ;
    }
}; // void DetermineAngleOfFacing ( int EnemyNum )

/* ----------------------------------------------------------------------
 * This function moves a single enemy.  It is used by MoveEnemys().
 * ---------------------------------------------------------------------- */
void 
MoveThisEnemy( int EnemyNum )
{
    Enemy ThisRobot = & AllEnemys[ EnemyNum ];

    //--------------------
    // At first, we check for a lot of cases in which we do not
    // need to move anything for this reason or for that
    //
    
    //--------------------
    // ignore all enemys with CompletelyFixed flag set...
    //
    if ( ThisRobot->CompletelyFixed ) return;
    
    //--------------------
    // robots that still have to wait also do not need to
    // be processed for movement
    //
    if ( ThisRobot->pure_wait > 0 ) return;
    
    if ( ThisRobot->AdvancedCommand == 2 ) 
    {
	TeleportToClosestWaypoint ( ThisRobot );
	ThisRobot->AdvancedCommand = 0;
    }
    
    if ( ThisRobot -> will_rush_tux )
    {
	if ( IsVisible ( & ( ThisRobot -> pos ) , 0 ) )
	{
	    ThisRobot -> persuing_given_course = TRUE ;
	    ThisRobot -> PrivatePathway [ 0 ] . x = Me [ 0 ] . pos . x ;
	    ThisRobot -> PrivatePathway [ 0 ] . y = Me [ 0 ] . pos . y ;
	    
	    if ( sqrt ( ( ThisRobot -> pos . x - Me [ 0 ] . pos . x ) * ( ThisRobot -> pos . x - Me [ 0 ] . pos . x ) +
			( ThisRobot -> pos . y - Me [ 0 ] . pos . y ) * ( ThisRobot -> pos . y - Me [ 0 ] . pos . y ) ) < 1 )
	    {
		ChatWithFriendlyDroid ( ThisRobot );
		ThisRobot -> will_rush_tux = FALSE ;
		ThisRobot -> persuing_given_course = FALSE ; 
	    }
	}
    }
    else
    {
	//--------------------
	// Checking collisions with colleagues is only nescessary for
	// those not busy rushing the Tux...
	//
	CheckEnemyEnemyCollision ( EnemyNum );
    }

    // if ( ThisRobot -> pos . z != Me [ 0 ] . pos . z )
    // DebugPrintf ( -4 , "\n%s(): moving enemy on remote level... " , __FUNCTION__ );

    //--------------------
    // Now comes the real movement part
    //
    MoveThisRobotThowardsHisCurrentTarget( EnemyNum );

    SelectNextWaypointAdvanced( EnemyNum );
    
    DetermineAngleOfFacing ( EnemyNum );

}; // void MoveThisEnemy ( int EnemyNum )

/* ----------------------------------------------------------------------
 * This function moves all enemys individually, using MoveThisEnemy(i)
 * and it also initiates the robots fireing behaviour via 
 * ProcessAttackStateMachine (i) all individually.
 * ---------------------------------------------------------------------- */
void
MoveEnemys ( void )
{
    int i;
    Enemy ThisRobot;
    
    //--------------------
    // We heal the robots again as time passes.  This function has been checked and
    // optimized for speed already....
    //
    PermanentHealRobots ();  // enemy robots heal as time passes...
    
    //--------------------
    // Some robots like 302 are already partly animated.  These robots
    // must be shifted in phase properly...
    //
    AnimateEnemys ();

    //--------------------
    // We make sure that the start and end indices for the bots of each level
    // are approximately correct, so that bots will (within a second or two)
    // be updated and taken into account correctly inside the movement code.
    //
    occasionally_update_first_and_last_bot_indices (  );
    
    //--------------------
    // Now the pure movement stuff..
    //
    for ( i = 0 ; i < Number_Of_Droids_On_Ship ; i++ )
    {
	ThisRobot = & AllEnemys[ i ];
	
	//--------------------
	// ignore dead robots as well...
	if ( ThisRobot -> Status == OUT ) continue;
	
	//--------------------
	// Now check if the robot is still alive if the robot just got killed, 
	// initiate the explosion and all that...
	//
	if ( ThisRobot->energy <= 1)
	{
	    InitiateDeathOfEnemy ( ThisRobot );
	    continue;
	}
	
	//--------------------
	// Ignore robots on other levels, except perhaps those, 
	// that are following Tux' trail
	// 
	// if ( ( ! IsActiveLevel ( ThisRobot->pos.z ) )  && 
	if ( ( ! level_is_partly_visible ( ThisRobot -> pos . z ) )  && 
	     ( ! ThisRobot -> FollowingInflusTail ) ) 
	    continue;
	
	//--------------------
	// Ignore robots, that are in the middle of their attack movement
	//
	// if ( ThisRobot -> animation_phase > 0 ) continue ;
	if ( ThisRobot -> animation_type == ATTACK_ANIMATION ) continue ;
	
	//--------------------
	// We set the speed to zero for a start.  Later it can be
	// set to something else...
	//
	ThisRobot -> speed . x = 0 ;
	ThisRobot -> speed . y = 0 ;
	
	//--------------------
	// Now we do the movement, either to the next waypoint OR if one
	// is set, we persue the given course, which allows complete control
	// of the bot moves (via the state machine for example)...
	//
	ProcessAttackStateMachine ( i );
	
    }	// for Number_Of_Droids_On_Ship
    
}; // MoveEnemys( void ) 

/* ----------------------------------------------------------------------
 * When an enemy is firing a shot, the newly created bullet must be 
 * assigned a speed, that would lead the bullet thowards the intended
 * target, which is done here.
 * ---------------------------------------------------------------------- */
void
set_bullet_speed_to_target_direction ( bullet* NewBullet , float bullet_speed , float xdist , float ydist )
{
  //--------------------
  // determine the direction of the shot, so that it will go into the direction of
  // the target
  //
  if (fabsf (xdist) > fabsf (ydist))
    {
      NewBullet->speed.x = bullet_speed;
      NewBullet->speed.y = ydist * NewBullet->speed.x / xdist;
      if (xdist < 0)
	{
	  NewBullet->speed.x = -NewBullet->speed.x;
	  NewBullet->speed.y = -NewBullet->speed.y;
	}
    }
  
  if (fabsf (xdist) < fabsf (ydist))
    {
      NewBullet->speed.y = bullet_speed;
      NewBullet->speed.x = xdist * NewBullet->speed.y / ydist;
      if (ydist < 0)
	{
	  NewBullet->speed.x = -NewBullet->speed.x;
	  NewBullet->speed.y = -NewBullet->speed.y;
	}
    }
}; // void set_bullet_speed_to_target_direction ( bullet* NewBullet , float bullet_speed , float xdist , float ydist )

/* ----------------------------------------------------------------------
 * 
 *
 * ---------------------------------------------------------------------- */
int
find_free_bullet_index ( void )
{
  int j;

  for ( j = 0 ; j < MAXBULLETS ; j ++ )
    {
      if ( AllBullets [ j ] . type == OUT )
	{
	  return ( j ) ;
	  break;
	}
    }

  //--------------------
  // If this point is ever reached, there's a severe bug in here...
  //
  GiveStandardErrorMessage ( __FUNCTION__  , "\
I seem to have run out of free bullet entries.  This can't normally happen.  --> some bug in here, oh no..." ,
			     PLEASE_INFORM, IS_FATAL );
  
  return ( -1 ) ; // can't happen.  just to make compilers happy (no warnings)

}; // void find_free_bullet_entry_pointer ( void )

/* ----------------------------------------------------------------------
 * This function is low-level:  It simply sets off a shot from enemy
 * through the pointer ThisRobot at the target VECTOR xdist ydist, which
 * is a DISTANCE VECTOR, NOT ABSOLUTE COORDINATES OF THE TARGET!!!
 * ---------------------------------------------------------------------- */
void 
RawStartEnemysShot( enemy* ThisRobot , float xdist , float ydist )
{
    int guntype = ItemMap[ Druidmap[ThisRobot->type].weapon_item.type ].item_gun_bullet_image_type;
    float bullet_speed = (float) ItemMap[ Druidmap[ ThisRobot->type ].weapon_item.type ].item_gun_speed;
    int j;
    float OffsetFactor;
    bullet* NewBullet=NULL;
    int bullet_index = 0 ;
    enemy* target_robot;
    
    //--------------------
    // If the robot is not in walk or stand animation, i.e. if it's in
    // gethit, death or attack animation, then we can't start another
    // shot/attack right now...
    //
    if ( ( ThisRobot -> animation_type != WALK_ANIMATION ) && 
	 ( ThisRobot -> animation_type != STAND_ANIMATION ) ) return ;
    
    //--------------------
    // find a bullet entry, that isn't currently used... 
    //
    bullet_index = find_free_bullet_index ();
    NewBullet = & ( AllBullets [ bullet_index ] );
    
    //--------------------
    // We send the bullet onto it's way thowards the given target
    //
    set_bullet_speed_to_target_direction ( NewBullet , bullet_speed , xdist , ydist );
  
    //--------------------
    // Newly, also enemys have to respect the angle modifier in their weapons...
    //
    RotateVectorByAngle ( & ( NewBullet->speed ) , ItemMap[ Druidmap[ ThisRobot->type ].weapon_item.type ].item_gun_start_angle_modifier );
    
    NewBullet->angle = - ( 90 + 45 + 180 * atan2 ( NewBullet->speed.y,  NewBullet->speed.x ) / M_PI );  
    
    //--------------------
    // At this point we mention, that when not moving anywhere, the robot should also
    // face into the direction of the shot
    //
    ThisRobot->previous_angle = NewBullet->angle + 180 ;
  
    // now we set the bullet type right
    // DebugPrintf( 0 , "Setting gun type : %d." , guntype );
    NewBullet->type = guntype;
    
    // Now we set the damage of this bullet to the correct value
    NewBullet->damage = ItemMap[ Druidmap[ ThisRobot->type ].weapon_item.type ].base_item_gun_damage;
    
    NewBullet->time_in_seconds = 0;
    NewBullet->time_in_frames = 0;
    
    //--------------------
    // Most enemy shots will not have any special 'magic' property...
    //
    NewBullet->poison_duration = 0;
    NewBullet->poison_damage_per_sec = 0;
    NewBullet->freezing_level = 0;
    NewBullet->paralysation_duration = 0;
    
    NewBullet->bullet_lifetime = ItemMap [ Druidmap[ThisRobot->type].weapon_item.type ].item_gun_bullet_lifetime;
    
    NewBullet->angle_change_rate = ItemMap[ Druidmap[ ThisRobot->type].weapon_item.type ].item_gun_angle_change;
    NewBullet->fixed_offset = ItemMap[ Druidmap[ ThisRobot->type].weapon_item.type ].item_gun_fixed_offset;
    NewBullet->owner_pos = & ( ThisRobot->pos );
    NewBullet->ignore_wall_collisions = 
	ItemMap[ Druidmap[ ThisRobot->type].weapon_item.type ].item_gun_bullet_ignore_wall_collisions;
    memset( NewBullet->total_miss_hit , UNCHECKED , MAX_ENEMYS_ON_SHIP );
    NewBullet->miss_hit_influencer = FALSE;
    NewBullet->to_hit = Druidmap [ ThisRobot->type ].to_hit ;
    NewBullet->was_reflected = FALSE;
    NewBullet->pass_through_explosions = 
	ItemMap[ Druidmap[ ThisRobot->type].weapon_item.type ].item_gun_bullet_pass_through_explosions;
    NewBullet->reflect_other_bullets = 
	ItemMap[ Druidmap[ ThisRobot->type].weapon_item.type ].item_gun_bullet_reflect_other_bullets;
    NewBullet->pass_through_hit_bodies = 
	ItemMap[ Druidmap[ ThisRobot->type].weapon_item.type ].item_gun_bullet_pass_through_hit_bodies;
    
    // start all bullets in the center of the shooter first...
    NewBullet -> pos . x = ThisRobot -> virt_pos . x;
    NewBullet -> pos . y = ThisRobot -> virt_pos . y;
    NewBullet -> pos . z = ThisRobot -> virt_pos . z;
    
    // fire bullets so, that they don't hit the shooter...
    if ( NewBullet->angle_change_rate == 0 ) OffsetFactor = 0.5; else OffsetFactor = 1;
    NewBullet->pos.x +=
	(NewBullet->speed.x) / (bullet_speed) * OffsetFactor ;
    NewBullet->pos.y +=
	(NewBullet->speed.y) / (bullet_speed) * OffsetFactor ;
    
    // wait for as long as is usual for this weapon type until making the next shot
    ThisRobot -> firewait = 
	ItemMap [ Druidmap [ ThisRobot -> type ] . weapon_item . type ] . item_gun_recharging_time ;
    
    
    //--------------------
    // Maybe this robot doesn't really generate any bullets, but rather the
    // attack is built into the droid animation itself.  Then of course we
    // can delete the bullet that might have been created and just apply some
    // damage to the Tux if the Tux was sufficiently close
    //
    if ( Druidmap [ ThisRobot -> type ] . use_image_archive_file )
    {
	
	if ( ( ThisRobot -> Status == OUT ) || ( ThisRobot -> animation_type == DEATH_ANIMATION ) )
	{
	    DebugPrintf ( -4 , "\n%s(): WARNING: animation phase reset for OUT bot... " , __FUNCTION__ );
	}
	
	ThisRobot -> animation_phase = ((float)first_attack_animation_image [ ThisRobot -> type ]) + 0.1 ;
	ThisRobot -> animation_type = ATTACK_ANIMATION;
	
	
	
	if ( Druidmap [ ThisRobot -> type ] . suppress_bullet_generation_when_attacking )
	{
	    DeleteBullet ( bullet_index , FALSE );
	    //--------------------
	    // Built-in attacks also don't use the recharge value of the
	    // weapon item.
	    //
	    ThisRobot -> firewait = 1.7 ;
	}
	
	ThisRobot -> current_angle = - ( - 90 + 180 * atan2 ( ydist ,  xdist ) / M_PI );  
	
	if ( ThisRobot -> is_friendly )
	{
	    DebugPrintf ( -3 , "\nATTACK OF A FRIENDLY DROID WITH BUILT-IN ATTACK ANIMATION DETECTED!-->hurting enemies..." );
	    for ( j = 0 , target_robot = & ( AllEnemys [ 0 ] ) ; 
		  j < Number_Of_Droids_On_Ship ; j ++ , target_robot ++ )
	    {
		if ( target_robot -> Status == OUT ) continue ;
		if ( target_robot -> pos . z != ThisRobot -> pos . z ) continue;
		if ( fabsf ( (float) ( target_robot -> pos . x - ThisRobot -> pos . x ) ) > 2.5 ) continue;
		if ( fabsf ( target_robot -> pos . y - ThisRobot -> pos . y ) > 2.5 ) continue;
		if ( target_robot == ThisRobot ) continue;
		DebugPrintf ( -3 , "\nATTACK OF A FRIENDLY DROID: NOW APPLYING DAMAGE!" );
		target_robot -> energy -= Druidmap [ ThisRobot -> type ] . physical_damage ; 
	    }
	}
	else
	{
	    //--------------------
	    // There can be two cases:  The hostile bot is attacking a player i.e. 
	    // a Tux or the hostile bot is attacking a friendly bot.
	    //
	    switch ( ThisRobot -> attack_target_type )
	    {
		case ATTACK_TARGET_IS_ENEMY:
		    DebugPrintf ( -4 , "\nNow hostile bot is trying to hurt friendly bot!" );
		    //--------------------
		    // For now we use a constant of 70% hit chance of hostile bots against
		    // any friendly bots.  This might be replaced by something more sophisticated
		    // some time later.
		    //
		    if ( MyRandom ( 100 ) <= 70 )
		    {
			//--------------------
			// If the bot hit, we reduce the energy of the other bot and maybe there
			// should also be some kind of scream of the victimized bot?
			//
			AllEnemys [ ThisRobot -> attack_target_index ] . energy -= 
			    Druidmap [ ThisRobot -> type ] . physical_damage ;
		  if ( MyRandom ( 100 ) <= 20 ) 
		      play_death_sound_for_bot ( & AllEnemys [ ThisRobot -> attack_target_index ] ) ;
		    }
		    else
		    {
			//--------------------
			// If the bot missed the other bot, we do nothing...
			//
		    }
		    break;
		    
		case ATTACK_TARGET_IS_PLAYER:
		    //--------------------
		    // For now, we just damage the Tux according to this enemys 'damage' value.  We
		    // don't fuss around whether the Tux is close at all or not.  
		    // In later releases, a more complex ruleset,
		    // taking into account position, maybe even bocks with the shield, should
		    // be implemented here.
		    //
		    if ( MyRandom ( 100 ) <= Me [ 0 ] . lv_1_bot_will_hit_percentage )
		    {
			//--------------------
			// If the bot hit, we reduce the energy of the Tux and maybe there
			// should also be some kind of scream of the Tux?
			//
			Me [ 0 ] . energy -= Druidmap [ ThisRobot -> type ] . physical_damage ;
			if ( MyRandom ( 100 ) <= 20 ) tux_scream_sound ( );
		    }
		    else
		    {
			//--------------------
			// If the bot missed, we do nothing...
			//
		    }
		    break;
		    
		case ATTACK_TARGET_IS_NOTHING:
		    DebugPrintf ( -4 , "\nSuspicious:  attack made while attack_target_type set to NOTHING." );
		    break;
		    
		default:
		    GiveStandardErrorMessage ( __FUNCTION__  , 
					       "unhandled attack_target_type encountered!",
					       PLEASE_INFORM, IS_FATAL );
		    break;
	    }
	}
	
	//--------------------
	// While we don't have sound samples for individual attack motions,
	// we'll use the death sound sample here too, even if noone is dying.
	// So far, that seems to work well, but it would be good, if sooner or
	// later bots could have separate attack and death sound samples, maybe
	// in some later release...
	//
	play_death_sound_for_bot ( ThisRobot );
    }
    else
    {
	//--------------------
	// Only in case of a conventional sword strike, i.e. no real animation
	// yet will we start the sound for that 'bullet'.
	//
	Fire_Bullet_Sound ( guntype );
    }
    
}; // void RawStartEnemysShot( enemy* ThisRobot , float xdist , float ydist )

/* ----------------------------------------------------------------------
 * This function should determine the closest visible player to this 
 * enemy droid.
 * ---------------------------------------------------------------------- */
int
ClosestVisiblePlayer ( Enemy ThisRobot ) 
{
  int PlayerNum;
  int BestTarget = 0 ;
  int BestDistance = 100000 ;
  int FoundDistance;

  //--------------------
  // For now we disable this function again, since there is no
  // need for multiplayer capabilities right now...
  //
  return ( 0 );

  for ( PlayerNum = 0 ; PlayerNum < MAX_PLAYERS ; PlayerNum ++ )
    {
      //--------------------
      // A player on a different level can never be the closest player.
      //
      if ( ThisRobot -> pos . z != Me [ PlayerNum ] . pos . z ) continue;

      //--------------------
      // A dead or deactivated player can never be the closest player.
      //
      if ( Me [ PlayerNum ] . status == OUT ) continue;

      //--------------------
      // Now we compute the distance and see if this robot is closer than
      // the previous best target.
      //
      FoundDistance = ( Me [ PlayerNum ] . pos . x - ThisRobot -> pos . x ) *
	( Me [ PlayerNum ] . pos . x - ThisRobot -> pos . x ) +
	( Me [ PlayerNum ] . pos . y - ThisRobot -> pos . y ) * 
	( Me [ PlayerNum ] . pos . y - ThisRobot -> pos . y ) ;

      if ( FoundDistance < BestDistance )
	{
	  BestDistance = FoundDistance ;
	  BestTarget = PlayerNum ;
	}
    }

  return BestTarget ;

}; // int ClosestVisiblePlayer ( Enemy ThisRobot ) 

/* ----------------------------------------------------------------------
 * This function should determine the closest visible player to this 
 * enemy droid.
 * ---------------------------------------------------------------------- */
int
ClosestOtherEnemyDroid ( Enemy ThisRobot ) 
{
  int BestTarget = -1 ;
  float BestDistance = 100000 ;
  float FoundDistance;
  int i;

  for ( i = 0 ; i < MAX_ENEMYS_ON_SHIP ; i ++ )
    {
      //--------------------
      // A dead or deactivated colleague can never be the closest enemy
      //
      if ( AllEnemys [ i ] . Status == OUT ) continue;

      //--------------------
      // A colleague on a different level can never be the closest enemy
      //
      if ( ThisRobot -> pos . z != AllEnemys [ i ] . pos . z ) continue;

      //--------------------
      // If we compare us with ourselves, this is also no good...
      //
      if ( ThisRobot == & (AllEnemys [ i ] ) ) continue;

      //--------------------
      // Now we compute the distance and see if this robot is closer than
      // the previous best target.
      //
      FoundDistance = ( AllEnemys [ i ] . pos . x - ThisRobot -> pos . x ) *
	( AllEnemys [ i ] . pos . x - ThisRobot -> pos . x ) +
	( AllEnemys [ i ] . pos . y - ThisRobot -> pos . y ) * 
	( AllEnemys [ i ] . pos . y - ThisRobot -> pos . y ) ;

      if ( FoundDistance < BestDistance )
	{
	  BestDistance = FoundDistance ;
	  BestTarget = i ;
	}
    }

  return BestTarget ;

}; // int ClosestOtherEnemyDroid ( Enemy ThisRobot ) 

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
float
DistanceToTux ( Enemy ThisRobot )
{

  return ( sqrt ( ( ThisRobot -> pos . x - Me [ 0 ] . pos . x ) * 
		  ( ThisRobot -> pos . x - Me [ 0 ] . pos . x ) + 
		  ( ThisRobot -> pos . y - Me [ 0 ] . pos . y ) * 
		  ( ThisRobot -> pos . y - Me [ 0 ] . pos . y ) ) );

}; // float DistanceToTux ( Enemy ThisRobot )

/* ----------------------------------------------------------------------
 *
 * ---------------------------------------------------------------------- */
int
EnemyOfTuxCloseToThisRobot ( Enemy ThisRobot , moderately_finepoint* vect_to_target )
{
  int j;
  float IgnoreRange = Druidmap [ ThisRobot -> type ] . minimal_range_hostile_bots_are_ignored;

  for ( j = 0 ; j < Number_Of_Droids_On_Ship ; j++ )
    {
      if ( AllEnemys[ j ].Status == OUT ) continue;
      if ( AllEnemys[ j ].is_friendly ) continue;
      if ( AllEnemys[ j ].pos.z != ThisRobot->pos.z ) continue;
      if ( DirectLineWalkable ( ThisRobot -> pos . x , ThisRobot -> pos . y , 
				AllEnemys [ j ] . pos . x , AllEnemys [ j ] . pos . y , 
				ThisRobot -> pos . z ) != TRUE ) continue;
      if ( sqrt ( ( ThisRobot -> pos . x - AllEnemys[ j ] . pos . x ) *
		  ( ThisRobot -> pos . x - AllEnemys[ j ] . pos . x ) +
		  ( ThisRobot -> pos . y - AllEnemys[ j ] . pos . y ) *
		  ( ThisRobot -> pos . y - AllEnemys[ j ] . pos . y ) ) > IgnoreRange ) continue;

      // At this point we have found our target
      vect_to_target -> x = AllEnemys [ j ] . pos . x - ThisRobot -> pos . x ;
      vect_to_target -> y = AllEnemys [ j ] . pos . y - ThisRobot -> pos . y ;
      DebugPrintf( 0 , "\nPOSSIBLE TARGET FOR FRIENDLY DROID FOUND!!!\n");
      DebugPrintf( 0 , "\nIt is: %s.\n", ThisRobot -> dialog_section_name );
      return ( TRUE );
    }
  return ( FALSE );

}; // int EnemyOfTuxCloseToThisRobot ( Enemy ThisRobot )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
update_vector_to_shot_target_for_friend ( enemy* ThisRobot , moderately_finepoint* vect_to_target )
{
  int j;
  float IgnoreRange = Druidmap [ ThisRobot -> type ] . minimal_range_hostile_bots_are_ignored;

  //--------------------
  // We set some default values, in case there isn't anything attackable
  // found below...
  //
  vect_to_target -> x = -1000;
  vect_to_target -> y = -1000;
  
  //--------------------
  // Since it's a friendly device in this case, it will aim at the (closest?) of
  // the MS bots.
  //
  // for ( j = 0 ; j < Number_Of_Droids_On_Ship ; j++ )
  for ( j  = first_index_of_bot_on_level [ ThisRobot -> pos . z ] ; 
	j <= last_index_of_bot_on_level [ ThisRobot -> pos . z ] ; j++ )
    {
      if ( AllEnemys [ j ] . Status == OUT ) continue;
      if ( AllEnemys [ j ] . is_friendly ) continue;
      if ( AllEnemys [ j ] . pos.z != ThisRobot->pos.z ) continue;
      /*
	This is MUCH TOO COSTLY!!!
	
	if ( DirectLineWalkable ( ThisRobot -> pos . x , ThisRobot -> pos . y , 
	AllEnemys [ j ] . pos . x , AllEnemys [ j ] . pos . y , 
	ThisRobot -> pos . z ) != TRUE ) continue;
      */
      
      if ( sqrt ( ( ThisRobot -> pos . x - AllEnemys[ j ] . pos . x ) *
		  ( ThisRobot -> pos . x - AllEnemys[ j ] . pos . x ) +
		  ( ThisRobot -> pos . y - AllEnemys[ j ] . pos . y ) *
		  ( ThisRobot -> pos . y - AllEnemys[ j ] . pos . y ) ) > IgnoreRange ) continue;
      
      // At this point we have found our target
      vect_to_target -> x = AllEnemys [ j ] . pos . x - ThisRobot -> pos . x ;
      vect_to_target -> y = AllEnemys [ j ] . pos . y - ThisRobot -> pos . y ;
      DebugPrintf( 0 , "\nPOSSIBLE TARGET FOR FRIENDLY DROID FOUND!!!\n");
      DebugPrintf( 0 , "\nIt is a good target for: %s.\n", ThisRobot -> dialog_section_name );
      break;
    }
  
  
  /*
  //--------------------
  // Maybe we havn't found a single target.  Then we don't attack anything of course.
  // But the target will be set to the Tux.
  //
  if ( j >= Number_Of_Droids_On_Ship ) 
  {
  ThisRobot->firewait = 1.0 ;
  TargetPlayerNum = ClosestVisiblePlayer ( ThisRobot ) ;
  vect_to_target -> x = Me [ TargetPlayerNum ] . pos . x - ThisRobot -> pos . x ;
  vect_to_target -> y = Me [ TargetPlayerNum ] . pos . y - ThisRobot -> pos . y ;
  return; 
  }
  */

  if ( j < Number_Of_Droids_On_Ship - 1 ) 
    {
      ThisRobot -> attack_target_type = ATTACK_TARGET_IS_ENEMY ;
      ThisRobot -> attack_target_index = j ;
    }
  else 
    ThisRobot -> attack_target_type = ATTACK_TARGET_IS_NOTHING ;

}; // void update_vector_to_shot_target_for_friend ( ThisRobot , moderately_finepoint* vect_to_target )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
update_vector_to_shot_target_for_enemy ( enemy* this_robot , moderately_finepoint* vect_to_target )
{
    int TargetPlayerNum;
    int j;
    int our_level = this_robot -> pos . z ;
    float best_dist, our_dist;
    
    //--------------------
    // By default, we set the target of this bot to the Tux himself
    // i.e. the closest (visible?) player.
    //
    TargetPlayerNum = ClosestVisiblePlayer ( this_robot ) ;
    vect_to_target -> x = Me [ TargetPlayerNum ] . pos . x - this_robot -> virt_pos . x ;
    vect_to_target -> y = Me [ TargetPlayerNum ] . pos . y - this_robot -> virt_pos . y ;
    this_robot -> attack_target_type = ATTACK_TARGET_IS_PLAYER ;
    this_robot -> attack_target_index = TargetPlayerNum ;
    
    best_dist = vect_len ( *vect_to_target );
    
    //--------------------
    // But maybe there is a friend of the Tux also close.  Then maybe we
    // should attack this one instead, since it's much closer anyway.
    // Let's see...
    //
    for ( j  = first_index_of_bot_on_level [ our_level ] ; 
	  j <=  last_index_of_bot_on_level [ our_level ] ; j ++ )
    {
	if ( AllEnemys [ j ] . pos . z != our_level ) continue ;
	if ( AllEnemys [ j ] . Status == OUT ) continue ;
	if ( ! AllEnemys [ j ] . is_friendly ) continue ;
	
	our_dist = sqrt ( powf ( this_robot -> pos . x - AllEnemys [ j ] . pos . x , 2 ) +
			  powf ( this_robot -> pos . y - AllEnemys [ j ] . pos . y , 2 )   );
	
	if ( our_dist > 5.0 ) continue ;
	
	if ( our_dist < best_dist )
	{
	    // DebugPrintf ( -4 , "\nThis hostile robot just switched to attacking a friendly droid." );
	    best_dist = our_dist ;
	    
	    vect_to_target -> x = AllEnemys [ j ] . pos . x - this_robot -> pos . x ;
	    vect_to_target -> y = AllEnemys [ j ] . pos . y - this_robot -> pos . y ;
	    this_robot -> attack_target_type = ATTACK_TARGET_IS_ENEMY ;
	    this_robot -> attack_target_index = j ;
	}
    }
    
}; // int update_vector_to_shot_target_for_enemy ( ThisRobot , moderately_finepoint* vect_to_target )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
occasionally_update_vector_and_shot_target ( enemy* ThisRobot , moderately_finepoint* vect_to_target ) 
{
    /* 
       static int first_call = FALSE ;
       static int last_time_bots_updated_target;
       
       if ( first_call )
       {
       last_time_bots_updated_target = SDL_GetTicks() - 10000 ;
       }
       if ( SDL_GetTicks() - last_time_bots_updated_target < 500 ) return;
       last_
    */
    
    if ( ThisRobot->is_friendly == TRUE )
    {
	update_vector_to_shot_target_for_friend ( ThisRobot , vect_to_target ) ;
    }
    else
    {
	update_vector_to_shot_target_for_enemy ( ThisRobot , vect_to_target ) ;
    }

}; // void give_occasionally_updated_vector_to_shot_target ( ThisRobot , & vect_to_target ) 

/* ----------------------------------------------------------------------
 * In some of the movement functions for enemy droids, we consider making
 * a step and move a bit into one direction or the other.  But not all
 * moves are really allowed and feasible.  Therefore we need a function
 * to check if a certain step makes sense or not, which is exactly what
 * this function is supposed to do.
 * ---------------------------------------------------------------------- */
int
ConsideredMoveIsFeasible ( Enemy ThisRobot , moderately_finepoint StepVector , int enemynum )
{
    if ( ( IsPassable ( ThisRobot -> pos.x + StepVector.x , 
			ThisRobot -> pos.y + StepVector.y ,
			ThisRobot -> pos.z ) ) &&
	 ( CheckIfWayIsFreeOfDroidsWithTuxchecking ( ThisRobot->pos.x , ThisRobot->pos.y , 
						     ThisRobot->pos.x + StepVector . x , 
						     ThisRobot->pos.y + StepVector . y ,
						     ThisRobot->pos.z , ThisRobot ) ) )
    {
	return TRUE;
    }
    
    return FALSE;

}; // int ConsideredMoveIsFeasible ( Enemy ThisRobot , finepoint StepVector )

/* ----------------------------------------------------------------------
 * Maybe there is a melee weapon using robot somewhere in the game.  In
 * this case it might come to be that this robot can't use his weapon
 * cause it is still too far away for a strike.  So the robot must move
 * in closer to the target.  This is what this function is for.  It is
 * called by the 'AttachInfluencer' functions and NOT directly by the
 * MoveThisRobotThowardsHisCurrentTarget or so, cause what this function does
 * is setting some new course parameters, NOT really alter the position
 * of this robot directly.
 *
 * Also in some cases, the robot will want to move farther away from the
 * melee combat range.  In this case sign (-1) will do the trick, while
 * otherwise sign (+1) must be given.
 * ---------------------------------------------------------------------- */
void
MoveInCloserForOrAwayFromMeleeCombat ( Enemy ThisRobot , int enemynum , int DirectionSign )
{
    finepoint VictimPosition;
    finepoint CurrentPosition;
    moderately_finepoint StepVector;
    moderately_finepoint RotatedStepVector;
    float StepVectorLen;
    int i , j ;
#define ANGLES_TO_TRY 7
    float RotationAngleTryList[ ANGLES_TO_TRY ] = { 0 , 30 , 360-30 , 60, 360-60, 90, 360-90 };
    
    //--------------------
    // When the robot is just getting hit, then there shouldn't be much
    // of a running motion, especially during the corresponding animaiton
    // phase...
    //
    if ( ThisRobot -> animation_type == GETHIT_ANIMATION ) 
	return;
    
    if ( DirectionSign < 0 )
    {
	if ( MyRandom ( 100 ) > 20 )
	{
	    // DebugPrintf ( 1 , "\nDROPPING MOVE OUT from failed random check." );
	    return;
	}
	DirectionSign = (-3) ;
    }

    //--------------------
    // If the distance is not yet right, we find a new location to move to.  We
    // do this WITHOUT consulting the waypoints, so that the robots become more
    // 'intelligent' in their movement.
    //
    // However great care must be taken so that the robot will not pass 
    // through walls, which is could, since there are no other checks for
    // enemy-wall collision and no corrects of any kind for this mistake.
    //
    // ThisRobot->TextVisibleTime = 0 ;
    // ThisRobot->TextToBeDisplayed = "Seeking to get closer to target...";
    //
    ThisRobot -> persuing_given_course = TRUE;
    ThisRobot -> PrivatePathway [ 0 ] . x = ThisRobot -> pos.x ;
    ThisRobot -> PrivatePathway [ 0 ] . y = ThisRobot -> pos.y ;
    
    //--------------------
    // Now we determine a probably better fighting position (not too far away
    // to save us from walking through walls) and see if it is perhaps reachable
    // without passing though walls and if it's also free of other droids so
    // that we won't bump into our colleagues as well.
    //
    if ( ThisRobot -> attack_target_type == ATTACK_TARGET_IS_PLAYER )
    {
	if ( ( ThisRobot -> attack_target_index < 0 ) ||
	     ( ThisRobot -> attack_target_index >= MAX_PLAYERS ) )
	{
	    fprintf ( stderr , "\nThisRobot -> attack_target_index=%d." , ThisRobot -> attack_target_index );
	    GiveStandardErrorMessage ( __FUNCTION__  , 
				       "attack_target_index pointing outside of player number!",
				       PLEASE_INFORM, IS_FATAL );
	}
	VictimPosition . x = Me [ ThisRobot -> attack_target_index ] . pos . x ;
	VictimPosition . y = Me [ ThisRobot -> attack_target_index ] . pos . y ;
    }
    else if ( ThisRobot -> attack_target_type == ATTACK_TARGET_IS_ENEMY )
    {
	VictimPosition . x = AllEnemys [ ThisRobot -> attack_target_index ] . virt_pos . x ;
	VictimPosition . y = AllEnemys [ ThisRobot -> attack_target_index ] . virt_pos . y ;
    }
    else if ( ThisRobot -> attack_target_type == ATTACK_TARGET_IS_NOTHING )
    {
	//--------------------
	// Well, if there is no target given, we don't do anything here in 
	// this function...
	//
	return;
    }
    else
    {
	GiveStandardErrorMessage ( __FUNCTION__  , 
				   "Unhandled attack_target_type encountered!",
				   PLEASE_INFORM, IS_FATAL );
    }
    
    CurrentPosition . x = ThisRobot -> virt_pos . x ;
    CurrentPosition . y = ThisRobot -> virt_pos . y ;
    
    StepVector . x = VictimPosition . x - CurrentPosition . x ;
    StepVector . y = VictimPosition . y - CurrentPosition . y ;
    
    //--------------------
    // Now some protection against division by zero when two bots
    // have _exactly_ the same position, i.e. are standing on top
    // of each other:
    //
    if ( ( fabsf ( StepVector . x ) < 0.01 ) &&
	 ( fabsf ( StepVector . y ) < 0.01 ) )
    {
	//--------------------
	// One droid must go to the left and up and one must go to
	// the right and down.  But who will go where?  --- We use
	// the index numbers to resolve the question...
	//
	if ( enemynum <= ThisRobot -> attack_target_index )
	{
	    StepVector . x = 3.0 ;
	    StepVector . y = 3.0 ;
	}
	else
	{
	    StepVector . x = -3.0 ;
	    StepVector . y = -3.0 ;
	}
    }
    
    
    StepVectorLen = sqrt ( ( StepVector . x ) * ( StepVector . x ) + ( StepVector . y ) * ( StepVector . y ) );

    StepVector . x /= ( DirectionSign * 2 * StepVectorLen ) ;
    StepVector . y /= ( DirectionSign * 2 * StepVectorLen ) ;
    
    //--------------------
    // Now we have assembled the simplest of ideas:  Try to move directly
    // thowards the Tux.  We just need to check if that does make some
    // sense to move there.  Otherwise we can still consider some variations
    // to the left or right.
    //
    for ( i = 0 ; i < ANGLES_TO_TRY ; i ++ )
    {
	RotatedStepVector.x = StepVector.x ;
	RotatedStepVector.y = StepVector.y ;
	RotateVectorByAngle ( & RotatedStepVector , RotationAngleTryList [ i ] ) ;
	
	// DebugPrintf ( -4 , "\n%s(): moving in/out for combat on remote level... " , __FUNCTION__ );

	//--------------------
	// Maybe we've found a solution, then we can take it and quit
	// trying around...
	//
	if ( ConsideredMoveIsFeasible ( ThisRobot , RotatedStepVector , enemynum ) )
	{
	    ThisRobot -> PrivatePathway [ 0 ] . x = ThisRobot -> pos.x + RotatedStepVector . x ;
	    ThisRobot -> PrivatePathway [ 0 ] . y = ThisRobot -> pos.y + RotatedStepVector . y ;
	    break;
	}
    }
    
    //--------------------
    // But if we didn't find anything, we'll just consider moving away from 
    // the robot that we got stuck into instead of making a step back.
    //
    if ( i >= ANGLES_TO_TRY ) 
    {
	//--------------------
	// Well, who is the closest (other) robot?
	//
	j = ClosestOtherEnemyDroid ( ThisRobot );
	
	StepVector . x = ThisRobot -> pos . x - AllEnemys [ j ] . pos . x ;
	StepVector . y = ThisRobot -> pos . y - AllEnemys [ j ] . pos . y ;
	StepVectorLen = sqrt ( ( StepVector . x ) * ( StepVector . x ) + ( StepVector . y ) * ( StepVector . y ) );
	
	//--------------------
	// If can happen, that two droids are EXACTLY on top of each other.  This
	// is possible by starting teleportation of a special force right on top
	// of a random bot for example.  But we should not cause a FLOATING POINT
	// EXCEPTION here!  AND we should also do a sensible handling...
	//
	if ( StepVectorLen )
	{
	    StepVector . x /= ( DirectionSign * 2 * StepVectorLen ) ;
	    StepVector . y /= ( DirectionSign * 2 * StepVectorLen ) ;
	}
	else
	{
	    StepVector . x = (float) MyRandom ( 100 ) / 200.0 ;
	    StepVector . y = (float) MyRandom ( 100 ) / 200.0 ;
	}
	
	//--------------------
	// Here, when eventually moving out of a colliding colleague,
	// we must not check for feasibility but only for wall collisions,
	// cause otherwise the move out of the colleague will never
	// be allowed.
	//
	if ( IsPassable ( ThisRobot -> pos.x + StepVector.x , 
			  ThisRobot -> pos.y + StepVector.y ,
			  ThisRobot -> pos.z ) )
	{
	    ThisRobot -> PrivatePathway [ 0 ] . x = ThisRobot -> pos.x + StepVector.x;
	    ThisRobot -> PrivatePathway [ 0 ] . y = ThisRobot -> pos.y + StepVector.y;
	}
	
    }

}; // void MoveInCloserForOrAwayFromMeleeCombat ( Enemy ThisRobot , int TargetPlayer , int enemynum )

/* ----------------------------------------------------------------------
 * At some points it may be nescessary, that an enemy turns around to
 * face the Tux.  This function does that, but only so much as can be
 * done in the current frame, i.e. NON-BLOCKING operation.
 *
 * The return value indicates, if the turning has already reached it's
 * target by now or not.
 * ---------------------------------------------------------------------- */
int
TurnABitThowardsPosition ( Enemy ThisRobot , float x , float y , float TurnSpeed )
{
  float RightAngle;
  float AngleInBetween;
  float TurningDirection;

  //--------------------
  // Now we find out what the final target direction of facing should
  // be.
  //
  // For this we use the atan2, which gives angles from -pi to +pi.
  // 
  // Attention must be paid, since 'y' in our coordinates ascends when
  // moving down and descends when moving 'up' on the scren.  So that
  // one sign must be corrected, so that everything is right again.
  //
  RightAngle = ( atan2 ( - ( y - ThisRobot -> pos . y ) ,  
			 + ( x - ThisRobot -> pos . x ) ) * 180.0 / M_PI ) ;
  //
  // Another thing there is, that must also be corrected:  '0' begins
  // with facing 'down' in the current rotation models.  Therefore angle
  // 0 corresponds to that.  We need to shift again...
  //
  RightAngle += 90 ;

  //--------------------
  // Now it's time do determine which direction to move, i.e. if to 
  // turn to the left or to turn to the right...  For this purpose
  // we convert the current angle, which is between 270 and -90 degrees
  // to one between -180 and +180 degrees...
  //
  if ( RightAngle > 180.0 ) RightAngle -= 360.0 ; 

  //--------------------
  // Having done these preparations, it's now easy to determine the right
  // direction of rotation...
  //
  AngleInBetween = RightAngle - ThisRobot -> current_angle ;
  if ( AngleInBetween > 180 ) AngleInBetween -= 360;
  if ( AngleInBetween <= -180 ) AngleInBetween += 360;

  if ( AngleInBetween > 0 )
    TurningDirection = +1 ; 
  else 
    TurningDirection = -1 ; 

  //--------------------
  // Now we turn and show the image until both chat partners are
  // facing each other, mostly the chat partner is facing the Tux,
  // since the Tux may still turn around to somewhere else all the 
  // while, if the chose so
  //
  ThisRobot -> current_angle += TurningDirection * TurnSpeed * Frame_Time() ;

  //--------------------
  // In case of positive turning direction, we wait, till our angle is greater
  // than the right angle.
  // Otherwise we wait till our angle is lower than the right angle.
  //
  AngleInBetween = RightAngle - ThisRobot -> current_angle ;
  if ( AngleInBetween > 180 ) AngleInBetween -= 360;
  if ( AngleInBetween <= -180 ) AngleInBetween += 360;
      
  if ( ( TurningDirection > 0 ) && ( AngleInBetween < 0 ) ) return ( TRUE );
  if ( ( TurningDirection < 0 ) && ( AngleInBetween > 0 ) ) return ( TRUE );

  return ( FALSE );

}; // int TurnABitThowardsTux ( Enemy ThisRobot , float TurnSpeed )

/* ----------------------------------------------------------------------
 * Enemies act as groups.  If one is hit, all will attack and the like.
 * This transmission of information is handled here.
 * ---------------------------------------------------------------------- */
void 
SetRestOfGroupToState ( Enemy ThisRobot , int NewState )
{
  int MarkerCode;
  int i;

  MarkerCode = ThisRobot -> marker ;

  if ( ( MarkerCode == 0 ) || ( MarkerCode == 101 ) )return ;

  for ( i = 0 ; i < MAX_ENEMYS_ON_SHIP ; i ++ )
    {
      if ( AllEnemys [ i ] . marker == MarkerCode )
	AllEnemys [ i ] . combat_state = NewState ;
    }

}; // void SetRestOfGroupToState ( Enemy ThisRobot , int NewState )

/* ----------------------------------------------------------------------
 * Enemies act as groups.  If one is hit, all will attack and the like.
 * Similarly, if you attack one peaceful guard, all other guards will be
 * pissed as well...
 * ---------------------------------------------------------------------- */
void 
robot_group_turn_hostile ( int enemy_num )
{
    int MarkerCode;
    int i;
    enemy* ThisRobot = & ( AllEnemys [ enemy_num ] ) ;

    MarkerCode = ThisRobot -> marker ;
    
    // if ( ( MarkerCode == 0 ) || ( MarkerCode == 101 ) ) return ;

    for ( i = 0 ; i < MAX_ENEMYS_ON_SHIP ; i ++ )
    {
	if ( AllEnemys [ i ] . marker == MarkerCode )
	    AllEnemys [ i ] . is_friendly = FALSE ;
    }

}; // void robot_group_turn_hostile ( int enemy_num )

/* ----------------------------------------------------------------------
 * This function sometimes fires a bullet from enemy number enemynum 
 * directly into the direction of the influencer, but of course only if 
 * the odds are good i.e. requirements are met.
 * ---------------------------------------------------------------------- */
void
ProcessAttackStateMachine ( int enemynum )
{
    moderately_finepoint vect_to_target;
    float dist2;
    Enemy ThisRobot = & AllEnemys[ enemynum ] ;
    int TargetPlayer;
    
    //--------------------
    // At first, we check for a lot of cases in which we do not
    // need to move anything for this reason or for that
    //
    // ignore robots on other levels 
    // if ( ThisRobot->pos.z != CurLevel->levelnum) return;
    // if ( ! IsActiveLevel ( ThisRobot -> pos . z ) ) return;
    
    // ignore dead robots as well...
    // if ( ThisRobot -> Status == OUT ) return;
    
    // ignore paralyzed robots as well...
    if ( ThisRobot -> paralysation_duration_left != 0 ) return;
    // ignore robots, that don't have any weapon
    if ( Druidmap [ ThisRobot -> type ] . weapon_item . type == ( -1 ) ) return;
    
    //--------------------
    // More for debugging purposes, we print out the current state of the
    // robot as his in-game text.
    //
    switch ( ThisRobot -> combat_state )
    {
	case MOVE_ALONG_RANDOM_WAYPOINTS:
	    ThisRobot->TextToBeDisplayed = "state:  Wandering along waypoints." ;
	    break;
	case TURN_THOWARDS_NEXT_WAYPOINT:
	    ThisRobot->TextToBeDisplayed = "state:  Turn thowards next WP." ;
	    break;
	case RUSH_TUX_ON_SIGHT_AND_OPEN_TALK:
	    ThisRobot->TextToBeDisplayed = "state:  Rush Tux on Sight and open Talk." ;
	    break;
	case STOP_AND_EYE_TUX:
	    ThisRobot->TextToBeDisplayed = "state:  Stop and Eye Tux." ;
	    break;
	case MAKE_ATTACK_RUN:
	    ThisRobot->TextToBeDisplayed = "state:  Make Attack Run." ;
	    break;
	case FIGHT_ON_TUX_SIDE:
	    ThisRobot->TextToBeDisplayed = "state:  Fight On Tux Side." ;
	    break;
	case RELENTLESS_FIRE_TO_GIVEN_POSITION:
	    ThisRobot->TextToBeDisplayed = "state:  RELENTLESS FIRE!" ;
	    break;
	case WAIT_AND_TURN_AROUND_AIMLESSLY:
	    ThisRobot->TextToBeDisplayed = "state:  Waiting, Turning aimlessly..." ;
	    break;
	case WAYPOINTLESS_WANDERING:
	    ThisRobot->TextToBeDisplayed = "state:  Waypointless wandering." ;
	    break;
	case TURN_THOWARDS_WAYPOINTLESS_SPOT:
	    ThisRobot->TextToBeDisplayed = "state:  Waypointless turning." ;
	    break;
	default:
	    ThisRobot->TextToBeDisplayed = "state:  UNHANDLED!!" ;
	    break;
    }      
    ThisRobot->TextVisibleTime = 0 ; 
    
    //--------------------
    // If some special command was given, like 
    // ATTACK_FIXED_MAP_POSITION=1, then we IGNORE EVERYTHING AND JUST FIRE OUR BULLETS,
    // NO MATTER WHOS IN THE LINE OF FIRE, HOW FAR WE ARE AWAY OR ANYTHING, JUST BLUNTLY
    // DO THAT.
    //
    if ( ThisRobot -> AdvancedCommand == 1 ) ThisRobot -> combat_state = RELENTLESS_FIRE_TO_GIVEN_POSITION ;
    
    if ( ThisRobot->AdvancedCommand == 2 ) 
    {
	TeleportToClosestWaypoint ( ThisRobot );
	ThisRobot->AdvancedCommand = 0;
	
    }
    
    if ( ThisRobot -> will_rush_tux ) 
    {
	if ( sqrt ( powf ( Me [ 0 ] . pos . x - ThisRobot -> pos . x , 2 ) +
		    powf ( Me [ 0 ] . pos . x - ThisRobot -> pos . x , 2 ) ) < 4 )
	{
	    ThisRobot -> combat_state = RUSH_TUX_ON_SIGHT_AND_OPEN_TALK ;
	    ThisRobot -> will_rush_tux = FALSE ;
	}
    }
    
    //--------------------
    // determine the distance vector to the target of this shot.  The target
    // depends of course on wheter it's a friendly device or a hostile device.
    //
    occasionally_update_vector_and_shot_target ( ThisRobot , & vect_to_target ) ;
    
    //--------------------
    // A friendly bot *MIGHT* help the tux in combat.  But this state must
    // not be entered too easily, as it might break some other things...
    //
    if ( ( ThisRobot -> is_friendly ) && 
	 ( ThisRobot -> attack_target_type == ATTACK_TARGET_IS_ENEMY ) && 
	 ( sqrt ( vect_to_target . x * vect_to_target.x + vect_to_target . y * vect_to_target . y  ) < Druidmap [ ThisRobot -> type ] . minimal_range_hostile_bots_are_ignored ) )
    {
	if ( DirectLineWalkable ( ThisRobot -> pos . x , ThisRobot -> pos . y , ThisRobot -> pos . x + vect_to_target . x ,
				  ThisRobot -> pos . y + vect_to_target . y , ThisRobot -> pos . z ) &&
	     ThisRobot -> combat_state != RUSH_TUX_ON_SIGHT_AND_OPEN_TALK )
	{
	    if ( ThisRobot -> combat_state != FIGHT_ON_TUX_SIDE )
		DebugPrintf ( 1 , "\nFriendly bot of type (%d) now switched to FIGHT_ON_TUX_SIDE." , ThisRobot -> type );
	    ThisRobot -> combat_state = FIGHT_ON_TUX_SIDE ;
	}
    }
    
    dist2 = sqrt( vect_to_target . x * vect_to_target . x + vect_to_target . y * vect_to_target . y );
    
    TargetPlayer = ClosestVisiblePlayer ( ThisRobot ) ;
    
    
    if ( ThisRobot -> combat_state == RUSH_TUX_ON_SIGHT_AND_OPEN_TALK )
    {
	
	if ( IsVisible ( & ( ThisRobot -> pos ) , 0 ) )
	{
	    ThisRobot -> persuing_given_course = TRUE ;
	    ThisRobot -> PrivatePathway [ 0 ] . x = Me [ 0 ] . pos . x ;
	    ThisRobot -> PrivatePathway [ 0 ] . y = Me [ 0 ] . pos . y ;
	    
	    if ( sqrt ( ( ThisRobot -> virt_pos . x - Me [ 0 ] . pos . x ) * ( ThisRobot -> virt_pos . x - Me [ 0 ] . pos . x ) +
			( ThisRobot -> virt_pos . y - Me [ 0 ] . pos . y ) * ( ThisRobot -> virt_pos . y - Me [ 0 ] . pos . y ) ) < 1 )
	    {
		ChatWithFriendlyDroid ( ThisRobot );
		ThisRobot -> will_rush_tux = FALSE ;
		ThisRobot -> persuing_given_course = FALSE ; 
		ThisRobot -> combat_state = TURN_THOWARDS_NEXT_WAYPOINT ;
	    }
	}
	MoveThisRobotThowardsHisCurrentTarget( enemynum );
	DetermineAngleOfFacing ( enemynum );
	return;
    }
    
    else if ( ThisRobot -> combat_state == MOVE_ALONG_RANDOM_WAYPOINTS )
    {
	MoveThisRobotThowardsHisCurrentTarget( enemynum );
	if ( remaining_distance_to_current_walk_target ( ThisRobot ) < 0.1 ) 
	{
	    SelectNextWaypointAdvanced( enemynum );
	    ThisRobot -> combat_state = TURN_THOWARDS_NEXT_WAYPOINT ;
	    return;
	}
	DetermineAngleOfFacing ( enemynum );
	
	// CheckEnemyEnemyCollision ( enemynum );
	
	//--------------------
	// In case that the enemy droid isn't even aware of Tux and
	// does not even see Tux now, there's nothing more to do here...
	// Not even the combat state will change.
	//
	if ( DistanceToTux( ThisRobot ) > Druidmap [ ThisRobot -> type ] . range_of_vision ) return;
	
	//--------------------
	// But if the Tux is now within range of vision, then it will be
	// 'greeted' and also the state will finally move to something more
	// interesting...
	//
	if ( ThisRobot -> has_greeted_influencer == FALSE )
	{
	    ThisRobot->has_greeted_influencer = TRUE;
	    if ( Druidmap[ ThisRobot->type ].greeting_sound_type != (-1) )
	    {
		PlayGreetingSound( Druidmap[ ThisRobot->type ].greeting_sound_type );
	    }
	}
	
	//--------------------
	// If the Tux isn't directly on it's way to the Tux, it might just
	// switch to take a closer look at the Tux.  But this will again
	// be different for hostile bots and for non-hostile bots...
	//
	if ( ! ThisRobot -> will_rush_tux )
	{
	    if ( ( ThisRobot -> is_friendly && DistanceToTux( ThisRobot ) < IS_FRIENDLY_EYE_DISTANCE ) ||
		 ( ! ThisRobot -> is_friendly ) )
	    {
		ThisRobot -> combat_state = STOP_AND_EYE_TUX ;
		ThisRobot -> state_timeout = Druidmap [ ThisRobot -> type ] . time_spent_eyeing_tux ;
	    }
	}
	
	return; 
    }
    else if ( ThisRobot -> combat_state == WAYPOINTLESS_WANDERING )
    {
	MoveThisRobotThowardsHisCurrentTarget( enemynum );
	if ( remaining_distance_to_current_walk_target ( ThisRobot ) < 0.1 ) 
	{
	    select_new_waypointless_random_walk_target ( enemynum );
	    ThisRobot -> combat_state = TURN_THOWARDS_WAYPOINTLESS_SPOT ;
	    return;
	}
	DetermineAngleOfFacing ( enemynum );
	
	// CheckEnemyEnemyCollision ( enemynum );
	
	//--------------------
	// In case that the enemy droid isn't even aware of Tux and
	// does not even see Tux now, there's nothing more to do here...
	// Not even the combat state will change.
	//
	if ( DistanceToTux( ThisRobot ) > Druidmap [ ThisRobot -> type ] . range_of_vision ) return;
	
	//--------------------
	// But if the Tux is now within range of vision, then it will be
	// 'greeted' and also the state will finally move to something more
	// interesting...
	//
	if ( ThisRobot -> has_greeted_influencer == FALSE )
	{
	    ThisRobot->has_greeted_influencer = TRUE;
	    if ( Druidmap[ ThisRobot->type ].greeting_sound_type != (-1) )
	    {
		PlayGreetingSound( Druidmap[ ThisRobot->type ].greeting_sound_type );
	    }
	}
	
	//--------------------
	// If the Tux isn't directly on it's way to the Tux, it might just
	// switch to take a closer look at the Tux.  But this will again
	// be different for hostile bots and for non-hostile bots...
	//
	if ( ! ThisRobot -> will_rush_tux )
	{
	    if ( ( ThisRobot -> is_friendly && DistanceToTux( ThisRobot ) < IS_FRIENDLY_EYE_DISTANCE ) ||
		 ( ! ThisRobot -> is_friendly ) )
	    {
		ThisRobot -> combat_state = STOP_AND_EYE_TUX ;
		ThisRobot -> state_timeout = Druidmap [ ThisRobot -> type ] . time_spent_eyeing_tux ;
	    }
	}
	
	return; 
    }
    else if ( ThisRobot -> combat_state == TURN_THOWARDS_NEXT_WAYPOINT )
    {
	//--------------------
	// We allow arbitrary turning speed in this case... so we disable
	// the turning control in display function...
	//
	ThisRobot -> last_phase_change = 100 ; 
	if ( 
	    TurnABitThowardsPosition ( ThisRobot , 
				       curShip . AllLevels [ Me [ 0 ] . pos . z ] -> 
				       AllWaypoints [ ThisRobot->nextwaypoint ] . x ,
				       curShip . AllLevels [ Me [ 0 ] . pos . z ] -> 
				       AllWaypoints [ ThisRobot->nextwaypoint ] . y ,
				       90 )
	    )
	{
	    //--------------------
	    // Maybe there should be some more waiting here, so that the bots and
	    // characters don't appear so very busy and moving so abruptly, but that
	    // may follow later...
	    //
	    ThisRobot -> combat_state = MOVE_ALONG_RANDOM_WAYPOINTS ;
	}
	return;
	
    }
    else if ( ThisRobot -> combat_state == TURN_THOWARDS_WAYPOINTLESS_SPOT )
    {
	//--------------------
	// We allow arbitrary turning speed in this case... so we disable
	// the turning control in display function...
	//
	ThisRobot -> last_phase_change = 100 ; 
	if ( 
	    TurnABitThowardsPosition ( ThisRobot , 
				       ThisRobot -> PrivatePathway [ 0 ] . x ,
				       ThisRobot -> PrivatePathway [ 0 ] . y ,
				       90 )
	    )
	{
	    //--------------------
	    // Maybe there should be some more waiting here, so that the bots and
	    // characters don't appear so very busy and moving so abruptly, but that
	    // may follow later...
	    //
	    ThisRobot -> combat_state = WAYPOINTLESS_WANDERING ;
	}
	return;
	
    }
    else if ( ThisRobot -> combat_state == WAIT_AND_TURN_AROUND_AIMLESSLY )
    {
	//--------------------
	// We allow arbitrary turning speed in this case... so we disable
	// the turning control in display function...
	//
	ThisRobot -> last_phase_change = 100 ; 
	
	TurnABitThowardsPosition ( ThisRobot , Me [ 0 ] . pos . x , Me [ 0 ] . pos . y , 30 );
	
	if ( ! ThisRobot -> pure_wait ) 
	    ThisRobot -> combat_state = TURN_THOWARDS_NEXT_WAYPOINT ;
	
	return;
	
    }
    else if ( ThisRobot -> combat_state == STOP_AND_EYE_TUX )
    {
	//--------------------
	// While eyeing the Tux, we stay right where we are, but
	// maybe we should turn to face right thowards the Tux..
	//
	TurnABitThowardsPosition ( ThisRobot , Me [ 0 ] . pos . x , Me [ 0 ] . pos . y , 120 );
	
	//--------------------
	// After some time, we'll no longer eye the Tux but rather do something,
	// like attack the Tux or maybe also return to 'normal' operation and do
	// nothing.  This timeout case is checked and handled here...  When the
	// Tux is still visible at timeout, then it will be attacked... otherwise
	// the robot resumes normal operation...
	//
	ThisRobot -> state_timeout -= Frame_Time();
	if ( ThisRobot -> state_timeout < 0 ) 
	{
	    
	    if ( ( DistanceToTux ( ThisRobot ) > Druidmap [ ThisRobot->type ] . range_of_vision ) ||
		 ( ( DistanceToTux ( ThisRobot ) > IS_FRIENDLY_EYE_DISTANCE ) &&ThisRobot->is_friendly ) )
	    {
		//--------------------
		// If the state has just timed out, we can return to 'normal' operation
		// which of course depends on the waypoint usage or not-waypoint usage
		// for this bot.
		//
		if ( ThisRobot -> stick_to_waypoint_system_by_default )
		{
		    ThisRobot -> combat_state = TURN_THOWARDS_NEXT_WAYPOINT ;
		    ThisRobot -> persuing_given_course = FALSE ;
		}
		else
		{
		    ThisRobot -> combat_state = WAYPOINTLESS_WANDERING ;
		    ThisRobot -> persuing_given_course = TRUE ;
		}
	    }
	    else
	    {
		if ( ThisRobot -> is_friendly )
		{
		    //--------------------
		    // No dramatic changes here... the bot will just not
		    // time out eyeing Tux unless the Tux moves away from him
		    // again...
		    //
		    ThisRobot -> state_timeout = 2.0 ; 
		}
		else
		{
		    if ( ThisRobot -> attack_run_only_when_direct_line )
		    {
			if ( DirectLineWalkable( ThisRobot -> pos . x , ThisRobot -> pos . y , ThisRobot -> pos . x + vect_to_target . x , ThisRobot -> pos . y + vect_to_target . y , ThisRobot -> pos . z ) )
			{
			    ThisRobot -> combat_state = MAKE_ATTACK_RUN ;
			    // not in this case.. SetRestOfGroupToState ( ThisRobot , MAKE_ATTACK_RUN );
			}
		    }
		    else
		    {
			ThisRobot -> combat_state = MAKE_ATTACK_RUN ;
			SetRestOfGroupToState ( ThisRobot , MAKE_ATTACK_RUN );
		    }
		    ThisRobot -> persuing_given_course = FALSE ;
		    
		    //--------------------
		    // We'll launch the attack cry of this bot...
		    //
		    if ( Druidmap[ ThisRobot->type ].greeting_sound_type != (-1) )
		    {
			// PlayStartAttackSound( Druidmap[ ThisRobot->type ].greeting_sound_type );
			play_enter_attack_run_state_sound ( Druidmap[ ThisRobot->type ].greeting_sound_type );
		    }
		    
		}
	    }
	}
	
	// if ( TargetIsEnemy ) ThisRobot -> combat_state = FIGHT_ON_TUX_SIDE ;
	
	//--------------------
	// Anyway, while in this state no more attacking is nescessary, so
	// we can return outright...
	//
	return;
    }
    else if ( ThisRobot -> combat_state == RELENTLESS_FIRE_TO_GIVEN_POSITION )
    {
	if (ThisRobot->firewait) return;  // can't fire:  gun not yet reloaded...
	vect_to_target.x = ThisRobot->Parameter1 - ThisRobot->pos.x;
	vect_to_target.y = ThisRobot->Parameter2 - ThisRobot->pos.y;
	RawStartEnemysShot( ThisRobot , vect_to_target.x , vect_to_target.y );
	return;
    }
    else if ( ThisRobot -> combat_state == MAKE_ATTACK_RUN )
    {
	MoveThisEnemy ( enemynum ); // this will now be done in the attack state machine...
	//--------------------
	// If the bot is now friendly, it should switch to 'normal'
	// operation again...
	//
	if ( ThisRobot -> is_friendly )
	{
	    if ( ThisRobot -> stick_to_waypoint_system_by_default )
		ThisRobot -> combat_state = MOVE_ALONG_RANDOM_WAYPOINTS ;
	    else
		ThisRobot -> combat_state = WAYPOINTLESS_WANDERING ;
	}
	
    }
    else if ( ThisRobot -> combat_state == FIGHT_ON_TUX_SIDE )
    {
	MoveThisEnemy ( enemynum ); // this will now be done in the attack state machine...
	
	if ( ! EnemyOfTuxCloseToThisRobot ( ThisRobot , & vect_to_target ) ) 
	{
	    //--------------------
	    // Once all the enemies of the Tux have been defeated, the
	    // bot can switch back to 'normal' state.  But that means
	    // different things for different bots...
	    //
	    if ( ThisRobot -> stick_to_waypoint_system_by_default )
	    {
		ThisRobot -> combat_state = TURN_THOWARDS_NEXT_WAYPOINT ;
		ThisRobot -> persuing_given_course = FALSE ;
	    }
	    else
	    {
		ThisRobot -> combat_state = WAYPOINTLESS_WANDERING ;
		ThisRobot -> persuing_given_course = TRUE ;
	    }
	    return;
	}
    }
    else
    {
	DebugPrintf ( -1000 , "\nWARNING:  Unidentified combat_state encountered: code=%d!" , ThisRobot -> combat_state );
	MoveThisEnemy ( enemynum ); // this will now be done in the attack state machine...
	return;
    }
    
    //--------------------
    // If the closes alive player is not alive at all, that's a sign
    // that there is nothing to attack any more, but just return.
    //
    if ( Me [ TargetPlayer ] . status == OUT ) return;
    
    //--------------------
    // If the closest alive player is not visible at all, then there is
    // nothing to do and we just return.
    //
    if ( ! IsVisible ( & ThisRobot -> virt_pos , TargetPlayer ) && 
	 ( ThisRobot -> is_friendly == FALSE ) ) 
	return; 
    
    // if ( ThisRobot -> is_friendly ) return; 
    
    //--------------------
    // For melee weapons, we can't just stand anywhere and try to
    // hit the influencer,  In most cases, we will have to move thowards
    // our target.  Here, this need is hopefully satisfied....
    //
    // But this moving around can lead to jittering of droids moving back and 
    // forth between two positions very rapidly.  Therefore we will not do this
    // movement thing every frame, but rather only sometimes
    //
    if ( ThisRobot -> last_combat_step > 0.20 )
    {
	ThisRobot -> last_combat_step = 0 ; 
	if ( ItemMap [ Druidmap [ ThisRobot -> type ] . weapon_item . type ] . item_gun_angle_change != 0 )
	{
	    MoveInCloserForOrAwayFromMeleeCombat ( ThisRobot , enemynum , (+1) );
	} // if a melee weapon is given.
	else if ( dist2 < 1.5 )
	{
	    MoveInCloserForOrAwayFromMeleeCombat ( ThisRobot , enemynum , (-1) );
	} // else the case, that no melee weapon 
    }
    else
    {
	ThisRobot -> last_combat_step += Frame_Time ();
    }
    
    //--------------------
    // Melee weapons have a certain limited range.  If such a weapon is used,
    // don't fire if the influencer is several squares away!
    //
    if ( ( ItemMap [ Druidmap [ ThisRobot -> type ] . weapon_item . type ] . item_gun_angle_change != 0 ) && 
	 ( dist2 > 1.5 ) ) return;
    
    if ( ThisRobot->firewait ) return;
    
    RawStartEnemysShot( ThisRobot , vect_to_target.x , vect_to_target.y );
  
}; // void ProcessAttackStateMachine ( int enemynum )

/* ----------------------------------------------------------------------
 * This function checks for enemy collsions and returns TRUE if enemy 
 * with number enemynum collided with another enemy from the list.
 * ---------------------------------------------------------------------- */
int
CheckEnemyEnemyCollision (int enemynum)
{
  int i;
  int curlev = Me [ 0 ] . pos . z ; // CurLevel->levelnum;
  float check_x, check_y;
  int swap;
  float xdist, ydist;
  float dist2;
  float speed_x, speed_y;
  enemy* ListEnemy;
  enemy* OurBot= & ( AllEnemys[ enemynum ] );

  // return ( FALSE );

  //--------------------
  // Enemys persuing a specific course may pass through other enerys
  // and are therefore exempted from the collision check
  //
  // if ( OurBot->persuing_given_course == TRUE ) return ( FALSE );

  check_x = OurBot->pos.x;
  check_y = OurBot->pos.y;

  //--------------------
  // Now we check through all the other enemys on the ship if there is
  // perhaps a collision with them...
  //

  // for (i = 0; i < MAX_ENEMYS_ON_SHIP	; i++)
  for (i = 0; i < Number_Of_Droids_On_Ship ; i++)
    {
      // check only collisions of LIVING enemys on this level
      if (AllEnemys[i].Status == OUT || AllEnemys[i].pos.z != curlev)
	continue;
      // dont check yourself...
      if (i == enemynum)
	continue;

      //--------------------
      // We set up a pointer to the next one in the list...
      //
      ListEnemy = & ( AllEnemys[ i ] );

      /* get distance between enemy i and enemynum */
      xdist = check_x - ListEnemy->pos.x;
      ydist = check_y - ListEnemy->pos.y;

      dist2 = sqrt(xdist * xdist + ydist * ydist);

      // Is there a Collision?
      if ( dist2 <= 2*DRUIDRADIUSXY )
	{

	  // am I waiting already?  If so, keep waiting... 
	  if ( OurBot->pure_wait)
	    {
	      // keep waiting
	      OurBot->pure_wait = WAIT_COLLISION;
	      continue;
	    }

	  // otherwise: stop this one enemy and go back youself
	  ListEnemy->pure_wait = WAIT_COLLISION;

	  swap = OurBot->nextwaypoint;
	  OurBot->nextwaypoint = OurBot->lastwaypoint;
	  OurBot->lastwaypoint = swap;

	  /*
	  if ( ( ListEnemy -> combat_state == MOVE_ALONG_RANDOM_WAYPOINTS ) ||
	       ( ListEnemy -> combat_state == TURN_THOWARDS_NEXT_WAYPOINT ) )
	    ListEnemy -> combat_state = WAIT_AND_TURN_AROUND_AIMLESSLY ;
	  */

	  if ( ListEnemy -> combat_state == MOVE_ALONG_RANDOM_WAYPOINTS )
	    ListEnemy -> combat_state = TURN_THOWARDS_NEXT_WAYPOINT ;

	  // push the stopped colleague a little bit backwards...
	  if (xdist)
	    ListEnemy->pos.x -= xdist / fabsf (xdist) * Frame_Time();
	  if (ydist)
	    ListEnemy->pos.y -= ydist / fabsf (ydist) * Frame_Time();

	  // Move a little bit out of the colleague yourself...
	  speed_x = OurBot->speed.x;
	  speed_y = OurBot->speed.y;

	  if (speed_x) OurBot->pos.x -= Frame_Time() * COL_SPEED * (speed_x) / fabsf (speed_x);
	  if (speed_y) OurBot->pos.y -= Frame_Time() * COL_SPEED * (speed_y) / fabsf (speed_y);

	  return TRUE;
	} // if collision distance reached
    } // for all the bots...

  return FALSE;
}; // int CheckEnemyEnemyCollision

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
start_gethit_animation_if_applicable ( enemy* ThisRobot ) 
{
    //--------------------
    // Maybe this robot is already fully animated.  In this case, after getting
    // hit, the gethit animation should be displayed, which we'll initiate here.
    //
    if ( ( last_gethit_animation_image [ ThisRobot -> type ] - first_gethit_animation_image [ ThisRobot -> type ] > 0 ) )
    {
	if ( ( ThisRobot -> Status == OUT ) || ( ThisRobot -> animation_type == DEATH_ANIMATION ) )
	{
	    DebugPrintf ( -4 , "\n%s(): WARNING: animation phase reset for OUT bot... " , __FUNCTION__ );
	}
	ThisRobot -> animation_phase = ((float)first_gethit_animation_image [ ThisRobot -> type ]) + 0.1 ;
	ThisRobot -> animation_type = GETHIT_ANIMATION;
    }

}; // void start_gethit_animation_if_applicable ( enemy* ThisRobot ) 

/* ----------------------------------------------------------------------
 * This function does the rotation of the enemys according to their 
 * current energy level.
 * ---------------------------------------------------------------------- */
void
AnimateEnemys (void)
{
    int i;
    enemy* our_enemy;
    
    // for (i = 0; i < MAX_ENEMYS_ON_SHIP ; i++)
    for ( i = 0; i < Number_Of_Droids_On_Ship ; i++ )
    {
      
	our_enemy = & ( AllEnemys [ i ] ) ;
	
	/* ignore enemys that are dead or on other levels or dummys */
	// if (AllEnemys[i].type == DEBUG_ENEMY) continue;
	// if (AllEnemys[i].pos.z != CurLevel->levelnum)
	if ( ! level_is_partly_visible ( our_enemy -> pos . z ) )
	    continue;

	//--------------------
	// While the old 'phase' doesn't have much meaning today any more, the
	// new animation phase does.
	//
	// First the walkphases,
	// Then the attack phases
	// then the gethit phases
	// finally the death phases
	//
	// if ( our_enemy -> animation_phase > 0 )
	// {
	if ( Druidmap [ our_enemy -> type ] . use_image_archive_file == TRUE )
	{
	    switch ( our_enemy -> animation_type )
	    {
		case WALK_ANIMATION:
		    our_enemy -> animation_phase += 
			Frame_Time() * droid_walk_animation_speed_factor [ our_enemy -> type ] ;
		    
		    //--------------------
		    // While we're in the walk animation cycle, we have the walk animation
		    // images cycle.
		    //
		    if ( our_enemy -> animation_phase >= last_walk_animation_image [ our_enemy -> type ] )
		    {
			our_enemy -> animation_phase = 0 ;
			our_enemy -> animation_type = WALK_ANIMATION;
		    }
		    //--------------------
		    // But as soon as the walk stops and the 'bot' is standing still, we switch
		    // to the standing cycle...
		    //
		    if ( ( fabsf ( our_enemy -> speed . x ) < 0.1 ) && ( fabsf ( our_enemy -> speed . y ) < 0.1 ) )
		    {
			our_enemy -> animation_type = STAND_ANIMATION ;
			our_enemy -> animation_phase = first_stand_animation_image [ our_enemy -> type ] - 1 ;
			// DebugPrintf ( -1000 , "\nSwitching to 'stand' now..." );
		    }

		    if ( our_enemy -> Status == OUT )
		    {
			DebugPrintf ( -4 , "\n%s(): WARNING: animation phase reset for OUT bot in WALK phase ... " , __FUNCTION__ );
		    }

		    break;
		case ATTACK_ANIMATION:
		    our_enemy -> animation_phase += 
			Frame_Time() * droid_attack_animation_speed_factor [ our_enemy -> type ] ;
		    
		    if ( our_enemy -> animation_phase >= last_attack_animation_image [ our_enemy -> type ] )
		    {
			our_enemy -> animation_phase = 0 ;
			our_enemy -> animation_type = WALK_ANIMATION;
		    }

		    if ( our_enemy -> Status == OUT )
		    {
			DebugPrintf ( -4 , "\n%s(): WARNING: animation phase reset for OUT bot in ATTACK phase... " , __FUNCTION__ );
		    }

		    break;
		case GETHIT_ANIMATION:
		    our_enemy -> animation_phase += 
			Frame_Time() * droid_gethit_animation_speed_factor [ our_enemy -> type ] ;
		    
		    if ( our_enemy -> animation_phase >= last_gethit_animation_image [ our_enemy -> type ] )
		    {
			our_enemy -> animation_phase = 0 ;
			our_enemy -> animation_type = WALK_ANIMATION;
		    }

		    if ( our_enemy -> Status == OUT )
		    {
			DebugPrintf ( -4 , "\n%s(): WARNING: animation phase reset for OUT bot in GETHIT phase ... " , __FUNCTION__ );
		    }

		    break;
		case DEATH_ANIMATION:
		    our_enemy -> animation_phase += 
			Frame_Time() * droid_death_animation_speed_factor [ our_enemy -> type ] ;

		    if ( our_enemy -> animation_phase >= last_death_animation_image [ our_enemy -> type ] - 1 )
		    {
			our_enemy -> animation_phase = last_death_animation_image [ our_enemy -> type ] - 1 ;
			our_enemy -> animation_type = DEATH_ANIMATION ;
		    }
		    break;
		case STAND_ANIMATION:
		    our_enemy -> animation_phase += 
			Frame_Time() * droid_stand_animation_speed_factor [ our_enemy -> type ] ;
		    
		    if ( our_enemy -> animation_phase >= last_stand_animation_image [ our_enemy -> type ] - 1 )
		    {
			our_enemy -> animation_phase = first_stand_animation_image [ our_enemy -> type ] - 1 ;
			our_enemy -> animation_type = STAND_ANIMATION;
		    }

		    if ( our_enemy -> Status == OUT )
		    {
			DebugPrintf ( -4 , "\n%s(): WARNING: animation phase reset for OUT bot in STAND phase... " , __FUNCTION__ );
		    }

		    break;
		    
		default:
		    fprintf ( stderr , "\nThe animation type found is: %d.", our_enemy -> animation_type );
		    GiveStandardErrorMessage ( __FUNCTION__  , "\
There was an animation type encountered that isn't defined in FreedroidRPG.\n\
That means:  Something is going *terribly* wrong!" ,
					       PLEASE_INFORM, IS_FATAL );
		    break;
	    }
	}
	
	
	if ( our_enemy -> Status == OUT)
	{
	    our_enemy -> phase = DROID_PHASES ;
	    continue;
	}
	
	if ( our_enemy -> energy <= 0 ) 
	{
	    DebugPrintf( 1 , "\n%s(): WARNING: Enemy with negative energy encountered.  Phase correction forced..." , __FUNCTION__ );
	    our_enemy -> phase = 0 ;
	}
	else
	{
	    our_enemy -> phase +=
		( our_enemy -> energy / Druidmap [ our_enemy -> type ] . maxenergy ) *
		Frame_Time () * DROID_PHASES * 2.5;
	}
	
	if ( our_enemy -> phase >= DROID_PHASES)
	{
	    our_enemy -> phase = 0;
	}
	
    }
}; // void AnimateEnemys ( void )

#undef _enemy_c
