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

void ProcessAttackStateMachine (int enemynum);

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
  
  for ( i = 0 ; i < MAXWAYPOINTS ; i ++ )
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

  DebugPrintf( 1 , "\nint DirectLineWalkable (...) : Checking from %d-%d to %d-%d.", (int) x1, (int) y1 , (int) x2, (int) y2 );
  fflush(stdout);

  if ( fabsf(x1-x2) > fabsf (y1-y2) ) LargerDistance=fabsf(x1-x2);
  else LargerDistance=fabsf(y1-y2);

  Steps=LargerDistance * 4 ;   // We check four times on each map tile...
  if ( Steps == 0 ) return TRUE;

  // We determine the step size when walking from (x1,y1) to (x2,y2) in Steps number of steps
  step.x = (x2 - x1) / Steps;
  step.y = (y2 - y1) / Steps;

  DebugPrintf( 2 , "\nint DirectLineWalkable (...) :  step.x=%f step.y=%f." , step.x , step.y );

  // We start from position (x1, y1)
  CheckPosition.x = x1;
  CheckPosition.y = y1;

  for ( i = 0 ; i < Steps ; i++ )
    {

      if ( IsPassable ( CheckPosition.x , CheckPosition.y , z , CENTER ) != CENTER ) 
	{
	  DebugPrintf( 1 , "\n DirectLineWalkable (...) : Connection analysis revealed : OBSTACLES!! NO WAY!!!");
	  return FALSE;
	}

      CheckPosition.x += step.x;
      CheckPosition.y += step.y;
    }

  DebugPrintf( 1 , "\n DirectLineWalkable (...) : Connection analysis revealed : FREE!");

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

  // for (i = 0; i < MAX_ENEMYS_ON_SHIP; i++)
  for (i = 0; i < Number_Of_Droids_On_Ship; i++)
    {
      if ( AllEnemys [ i ] . Status == OUT)
	continue;
      if ( AllEnemys [ i ] . energy < Druidmap [ AllEnemys [ i ] . type ] . maxenergy )
	AllEnemys[i].energy += Druidmap[AllEnemys [ i ] . type ] . lose_health * Frame_Time( );
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

  for (i = 0; i < MAX_ENEMYS_ON_SHIP; i++)
    {
      AllEnemys[i].type = -1;
      AllEnemys[i].pos.z = AllEnemys[i].energy = 0;
      AllEnemys[i].phase = 0;
      AllEnemys[i].nextwaypoint = AllEnemys[i].lastwaypoint = 0;
      AllEnemys[i].Status = OUT;
      AllEnemys[i].warten = 0;
      AllEnemys[i].frozen = 0;
      AllEnemys[i].poison_duration_left = 0;
      AllEnemys[i].poison_damage_per_sec = 0;
      AllEnemys[i].firewait = 0;
      AllEnemys[i].energy = 0;
      AllEnemys[i].SpecialForce = 0;
      AllEnemys[i].AdvancedCommand = 0;
      AllEnemys[i].CompletelyFixed = 0;
      AllEnemys[i].Parameter1 = 0;
      AllEnemys[i].Parameter2 = 0;
      AllEnemys[i] . marker = 0;
      AllEnemys[i].is_friendly = 0;
      AllEnemys[i].TextVisibleTime = 0;
      AllEnemys[i].TextToBeDisplayed = "";
      AllEnemys[i].persuing_given_course = FALSE;
      AllEnemys[i].FollowingInflusTail = FALSE;
      AllEnemys[i].StayHowManySecondsBehind = 5;

      AllEnemys[i].previous_angle = 0 ;         // which angle has this robot been facing the frame before?
      AllEnemys[i].current_angle = 0 ;          // which angle will the robot be facing now?
      AllEnemys[i].last_phase_change = 100 ;      // when did the robot last change his (8-way-)direction of facing
      AllEnemys[i].previous_phase = 0 ;         // which (8-way) direction did the robot face before?
      AllEnemys[i].has_greeted_influencer = FALSE ;   // has this robot issued his first-time-see-the-Tux message?
      AllEnemys[i].will_rush_tux = FALSE ; 
      AllEnemys[i].last_combat_step = 100 ;       // when did this robot last make a step to move in closer or farther away from Tux in combat?      
      for ( j=0 ; j < MAX_STEPS_IN_GIVEN_COURSE ; j++ )
	{
	  AllEnemys[i].PrivatePathway[j].x=0;
	  AllEnemys[i].PrivatePathway[j].y=0;
	}

    }

  return;

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

  // count the number of waypoints on CurLevel
  wp_num = 0;

  for ( i=0 ; i<MAXWAYPOINTS ; i++ )
    {
      if ( ShuffleLevel->AllWaypoints[i].x != 0 ) wp_num ++;
    }

  nth_enemy = 0;
  for (i = 0; i < MAX_ENEMYS_ON_SHIP ; i++)
    {
      if ( ( AllEnemys[i].Status == OUT ) || ( AllEnemys[i].pos.z != LevelNum ) )
	continue;		// dont handle dead enemys or on other level 

      AllEnemys[ i ].persuing_given_course = FALSE; // since position is now completely mixed up,
                                                    // the robot needs to forget about any previous given course.

      if (AllEnemys[i].CompletelyFixed) continue;

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
	  for ( j=0 ; j<MAXWAYPOINTS ; j ++ )
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

      nth_enemy++;
      if (nth_enemy < wp_num)
	wp = nth_enemy;
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
 * This function checks if the connection between two points is free of
 * droids.  
 *
 * MAP TILES ARE NOT TAKEN INTO CONSIDERATION, ONLY DROIDS!!!
 *
 * ---------------------------------------------------------------------- */
int 
CheckIfWayIsFreeOfDroids ( float x1 , float y1 , float x2 , float y2 , int OurLevel , Enemy ExceptedRobot ) // int ExceptedDroid )
{
  float LargerDistance;
  int Steps;
  int i, j;
  finepoint step;
  finepoint CheckPosition;

  DebugPrintf( 2, "\nint CheckIfWayIsFreeOfDroids (...) : Checking from %d-%d to %d-%d.", (int) x1, (int) y1 , (int) x2, (int) y2 );
  fflush(stdout);

  if ( fabsf(x1-x2) > fabsf (y1-y2) ) LargerDistance=fabsf(x1-x2);
  else LargerDistance=fabsf(y1-y2);

  Steps=LargerDistance * 4 ;   // We check four times on each map tile...
  if ( Steps == 0 ) return TRUE;

  // We determine the step size when walking from (x1,y1) to (x2,y2) in Steps number of steps
  step.x = (x2 - x1) / Steps;
  step.y = (y2 - y1) / Steps;

  DebugPrintf( 2 , "\nint CheckIfWayIsFreeOfDroids (...) :  step.x=%f step.y=%f." , step.x , step.y );

  // We start from position (x1, y1)
  CheckPosition.x = x1;
  CheckPosition.y = y1;

  for ( i = 0 ; i < Steps + 1 ; i++ )
    {
      // for ( j = 0 ; j < MAX_ENEMYS_ON_SHIP ; j ++ )
      for ( j = 0 ; j < Number_Of_Droids_On_Ship ; j ++ )
	{
	  if ( AllEnemys[j].pos.z != OurLevel ) continue;
	  if ( AllEnemys[j].Status == OUT ) continue;
	  if ( AllEnemys[j].energy <= 0 ) continue;

	  // if ( j == ExceptedDroid ) continue;
	  if ( & ( AllEnemys [ j ] ) == ExceptedRobot ) continue;

	  if ( AllEnemys[j].warten > 0 ) continue;

	  // so it seems that we need to test this one!!
	  if ( ( fabsf(AllEnemys[j].pos.x - CheckPosition.x ) < 2*Druid_Radius_X ) &&
	       ( fabsf(AllEnemys[j].pos.y - CheckPosition.y ) < 2*Druid_Radius_Y ) ) 
	    {
	      DebugPrintf( 2, "\nCheckIfWayIsFreeOfDroids (...) : Connection analysis revealed : TRAFFIC-BLOCKED !");
	      return FALSE;
	    }
	}

      if ( ( fabsf( Me[0].pos.x - CheckPosition.x ) < 2*Druid_Radius_X ) &&
	   ( fabsf( Me[0].pos.y - CheckPosition.y ) < 2*Druid_Radius_Y ) ) 
	{
	  DebugPrintf( 2 , "\nCheckIfWayIsFreeOfDroids (...) : Connection analysis revealed : TRAFFIC-BLOCKED-INFLUENCER !");
	  return FALSE;
	}


      CheckPosition.x += step.x;
      CheckPosition.y += step.y;
    }

  DebugPrintf( 2 , "\nCheckIfWayIsFreeOfDroids (...) : Connection analysis revealed : FREE!");
  return TRUE;
}; // CheckIfWayIsFreeOfDroids ( float x1 , float y1 , float x2 , float y2 , int OurLevel , int ExceptedDroid )


/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
RawEnemyApproachPosition ( Enemy ThisRobot , finepoint nextwp_pos )
{
  finepoint Restweg;
  float maxspeed;

  //--------------------
  // According to properties of the robot like being frozen or not,
  // we define the maximum speed of this machine for later use...
  // A frozen robot is slow while a paralyzed robot can do absolutely nothing.
  //
  // if ( ThisRobot -> paralysation_duration_left != 0 ) return;
  if ( ThisRobot -> frozen == 0 )
    maxspeed = ItemMap[ Druidmap[ ThisRobot->type ].drive_item.type ].item_drive_maxspeed;
  else 
    maxspeed = 0.2 * ItemMap[ Druidmap[ ThisRobot->type ].drive_item.type ].item_drive_maxspeed;

  //--------------------
  // Now that we have found out where to go, we can start to determine the remaining 
  // way until the target point is reached.
  //
  Restweg.x = nextwp_pos.x - ThisRobot->pos.x;
  Restweg.y = nextwp_pos.y - ThisRobot->pos.y;

  //--------------------
  // As long a the distance from the current position of the enemy
  // to its next wp is large, movement is rather simple:
  //
  if ( fabsf (Restweg.x)  > Frame_Time() * maxspeed )
    {
      ThisRobot->speed.x =
	(Restweg.x / fabsf (Restweg.x)) * maxspeed;
      ThisRobot->pos.x += ThisRobot->speed.x * Frame_Time ();
    } 	 
  else
    {
      // --------------------
      // Once this enemy is close to his final destination waypoint, we have
      // to do some fine tuning, and then of course set the next waypoint.
      ThisRobot->pos.x = nextwp_pos.x;
      ThisRobot->speed.x = 0;
    }

  if ( fabsf (Restweg.y)  > Frame_Time() * maxspeed )
    {
      ThisRobot->speed.y =
	(Restweg.y / fabsf (Restweg.y)) * maxspeed;
      ThisRobot->pos.y += ThisRobot->speed.y * Frame_Time ();
    }
  else
    {
      ThisRobot->pos.y = nextwp_pos.y;
      ThisRobot->speed.y = 0;
    }

}; // void

/* ----------------------------------------------------------------------
 * This function moves one robot thowards his next waypoint.  If already
 * there, the function does nothing more.
 * ---------------------------------------------------------------------- */
void 
MoveThisRobotThowardsHisCurrentTarget ( int EnemyNum )
{
  Waypoint WpList;		/* Pointer to waypoint-list */
  finepoint nextwp_pos;
  Enemy ThisRobot=&AllEnemys[ EnemyNum ];
  int HistoryIndex;
  Level WaypointLevel = curShip.AllLevels[ AllEnemys[ EnemyNum ].pos.z ];

  DebugPrintf( 2 , "\n void MoveThisRobotThowardsHisCurrentTarget ( int EnemyNum ) : real function call confirmed. ");

  // We do some definitions to save us some more typing later...
  WpList = WaypointLevel->AllWaypoints;

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
      nextwp_pos.x = ThisRobot->PrivatePathway[0].x;
      nextwp_pos.y = ThisRobot->PrivatePathway[0].y;
    }
  else
    {
      nextwp_pos.x = WpList[ ThisRobot->nextwaypoint ] . x ;
      nextwp_pos.y = WpList[ ThisRobot->nextwaypoint ] . y ;
    }

  //--------------------
  // Maybe this robot is following behind influ.  Then of course we need to set
  // even different special target positions...
  //
  if ( ThisRobot->FollowingInflusTail == TRUE )
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
 *
 *
 * ---------------------------------------------------------------------- */
void
RawSetNewRandomWaypoint ( Enemy ThisRobot )
{
  int i,j;
  Waypoint WpList;		/* Pointer to waypoint-liste */
  int nextwp;
  finepoint nextwp_pos;
  int trywp = 0 ;
  float maxspeed;
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
  maxspeed = ItemMap[ Druidmap[ ThisRobot->type ].drive_item.type ].item_drive_maxspeed;
  nextwp_pos.x = WpList[nextwp].x;
  nextwp_pos.y = WpList[nextwp].y;

  ThisRobot->lastwaypoint = ThisRobot->nextwaypoint;
  
  // search for possible connections from here...
  DebugPrintf (2, "\nRawSetNewRandomWaypoint ( Enemy ThisRobot ): searching for possible connections...");
  
  // search for the first connection, that doesn't exist any more, so
  // that we know, which connections surely do exist
  for ( j=0; j<MAX_WP_CONNECTIONS; j++ )
    if ( WpList[nextwp].connections[j] == -1 )
      break;
  
  DebugPrintf ( 2 , "\nRawSetNewRandomWaypoint ( Enemy ThisRobot ):  Found %d possible targets." , j );
  
  // Of course, only if such connections exist at all, we do the
  // following change of target waypoint procedure
  if ( j < MAX_WP_CONNECTIONS )
    {
      //--------------------
      // At this point, we should check, if there is another waypoint 
      // and also if the way there is free of other droids
      //
      for ( i = 0; i < j ; i++ )
	{
	  FreeWays[i] = CheckIfWayIsFreeOfDroids ( WpList[ThisRobot->lastwaypoint].x , WpList[ThisRobot->lastwaypoint].y , WpList[WpList[ThisRobot->lastwaypoint].connections[i]].x , WpList[WpList[ThisRobot->lastwaypoint].connections[i]].y , ThisRobot->pos.z , ThisRobot );
	}
      
      //--------------------
      // Now see whether any way point at all is free in that sense
      // otherwise we set this robot to waiting and return;
      //
      for ( i = 0 ; i < j ; i++ )
	{
	  if ( FreeWays[i] ) break;
	}
      if ( i == j )
	{
	  DebugPrintf( 2 , "\nRawSetNewRandomWaypoint ( Enemy ThisRobot ): Sorry, there seems no free way out.  I'll wait then... , j was : %d ." , j);
	  ThisRobot->warten = 1.5 ; // this makes this droid 'passable' for other droids for now...
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
      SolutionFound=FALSE;
      while ( !SolutionFound )
	{
	  TestConnection = MyRandom (MAX_WP_CONNECTIONS - 1);
	  
	  if ( WpList[nextwp].connections[ TestConnection ] == (-1) ) continue;
	  if ( !FreeWays[TestConnection] ) continue;
	  
	  trywp = WpList[nextwp].connections[ TestConnection ];
	  SolutionFound = TRUE;
	}
      
    }
  else
    {
      GiveStandardErrorMessage ( "RawSetNewRandomWaypoint ( ... )" , "\
There was a droid on a waypoint, that apparently has no connections to other waypoints...\n\
This is an error in the waypoint structure of this level.",
				 NO_NEED_TO_INFORM, IS_FATAL );
    }
  
  // set new waypoint...
  ThisRobot->nextwaypoint = trywp;
  
  DebugPrintf ( 2 , "\nRawSetNewRandomWaypoint ( Enemy ThisRobot ):  A new waypoint has been set." );
  
}; // void RawSetNewRandomWaypoint ( Enemy ThisRobot )

/* ----------------------------------------------------------------------
 * This function moves one robot in an advanced way, that hasn't been
 * present within the classical paradroid game.
 * ---------------------------------------------------------------------- */
void 
SelectNextWaypointAdvanced ( int EnemyNum )
{
  Waypoint WpList;		/* Pointer to waypoint-liste */
  int nextwp;
  finepoint nextwp_pos;
  float maxspeed;
  Enemy ThisRobot=&AllEnemys[ EnemyNum ];
  Level WaypointLevel = curShip.AllLevels[ AllEnemys[ EnemyNum ].pos.z ];
  finepoint Restweg;

  DebugPrintf( 2 , "\nvoid SelectNextWaypointAdvanced ( int EnemyNum ) : real function call confirmed. ");

  //--------------------
  // Maybe currently we do not stick to the whole waypoint system but rather 
  // choose our course independently.  Then it's PersueGivenCourse.  Otherwise
  // we select waypoints as we're used to...
  //
  if ( ThisRobot->persuing_given_course == TRUE ) return;

  //--------------------
  // We do some definitions to save us some more typing later...
  //
  WpList = WaypointLevel->AllWaypoints;
  nextwp = ThisRobot->nextwaypoint;
  // maxspeed = Druidmap[ ThisRobot->type ].maxspeed;
  maxspeed = ItemMap[ Druidmap[ ThisRobot->type ].drive_item.type ].item_drive_maxspeed;
  nextwp_pos.x = WpList[nextwp].x;
  nextwp_pos.y = WpList[nextwp].y;

  // determine the remaining way until the target point is reached
  Restweg.x = nextwp_pos.x - ThisRobot->pos.x;
  Restweg.y = nextwp_pos.y - ThisRobot->pos.y;

  //--------------------
  // Now we can see if we are perhaps already there?
  // then it might be time to set a new waypoint.
  //
  if ((Restweg.x == 0) && (Restweg.y == 0))
    {
      RawSetNewRandomWaypoint ( ThisRobot );
    } // if

}; // void SelectNextWaypointAdvanced ( int EnemyNum )

/* ----------------------------------------------------------------------
 * This function moves one robot in an advanced way, that hasn't been
 * present within the classical paradroid game.
 * ---------------------------------------------------------------------- */
float 
RemainingDistanceToNextWaypoint ( Enemy ThisRobot )
{
  Waypoint WpList;		/* Pointer to waypoint-liste */
  int nextwp;
  finepoint nextwp_pos;
  float maxspeed;
  Level WaypointLevel = curShip.AllLevels[ ThisRobot -> pos.z ];
  finepoint Restweg;

  DebugPrintf( 2 , "\nvoid SelectNextWaypointAdvanced ( int EnemyNum ) : real function call confirmed. ");

  //--------------------
  // Maybe currently we do not stick to the whole waypoint system but rather 
  // choose our course independently.  Then it's PersueGivenCourse.  Otherwise
  // we select waypoints as we're used to...
  //
  // if ( ThisRobot->persuing_given_course == TRUE ) return;

  //--------------------
  // We do some definitions to save us some more typing later...
  //
  WpList = WaypointLevel->AllWaypoints;
  nextwp = ThisRobot->nextwaypoint;
  // maxspeed = Druidmap[ ThisRobot->type ].maxspeed;
  maxspeed = ItemMap[ Druidmap[ ThisRobot->type ].drive_item.type ].item_drive_maxspeed;
  nextwp_pos.x = WpList[nextwp].x;
  nextwp_pos.y = WpList[nextwp].y;

  // determine the remaining way until the target point is reached
  Restweg.x = nextwp_pos.x - ThisRobot->pos.x;
  Restweg.y = nextwp_pos.y - ThisRobot->pos.y;

  return ( sqrt ( Restweg.x * Restweg.x + Restweg.y * Restweg.y ) ) ;

}; // float RemainingDistanceToNextWaypoint ( Enemy ThisRobot )

/* ----------------------------------------------------------------------
 * This function swaps two enemys in the AllEnemys array.
 * ---------------------------------------------------------------------- */
void
SwapEnemys ( int First , int Second ) 
{
  enemy Zwisch;
  int i;

  //--------------------
  // This function swaps two enemies.  However this can not be done
  // without taking also the target index of the mouse move for the
  // influecner into account.
  //
  for ( i = 0 ; i < MAX_PLAYERS ; i ++ )
    {
      if ( Me [ i ] . mouse_move_target_is_enemy == First )
	{
	  Me [ i ] .  mouse_move_target_is_enemy = Second ;
	}
      else if ( Me [ i ] . mouse_move_target_is_enemy == Second )
	{
	  Me [ i ] .  mouse_move_target_is_enemy = First ;
	}
    }
	   
  //--------------------
  // Now we can swap the two enemies as desired...
  //
  memcpy ( &Zwisch , & ( AllEnemys[ First ] ) , sizeof( enemy ) );
  memcpy ( & ( AllEnemys[ First ] ) , & ( AllEnemys [ Second ] ) , sizeof( enemy ) );
  memcpy ( & ( AllEnemys[ Second ] ) , & Zwisch , sizeof( enemy ) );

}; // void SwapEnemys ( int First , int Second ) 

/* ----------------------------------------------------------------------
 * In order to make sure that dead robots are blitted first, we swap them
 * to the beginning of the AllEnemys array.
 * ---------------------------------------------------------------------- */
void
SwapThisRobotToFrontPosition ( enemy* ThisRobot )
{
  int i, j;

  //--------------------
  // This function is only for servers and single player games...
  //
  if ( ClientMode ) return; 

  for ( i = 0 ; i < Number_Of_Droids_On_Ship ; i++ )
    {
      if ( AllEnemys[ i ] .Status != OUT )
	{

	  //--------------------
	  // Now we find out the number of the second robot for the
	  // swap operation, so that we can send it to the client later.
	  //
	  for ( j = 0 ; j < Number_Of_Droids_On_Ship ; j ++ )
	    {
	      if ( & ( AllEnemys [ j ] ) == ThisRobot ) break;
	    }
	  if ( i == j ) return ; // in this case, no swap is at all nescessary and no signal as well...
	  if ( j == Number_Of_Droids_On_Ship )
	    {
	      DebugPrintf ( 0 , "\nSEVERE ERROR IN SwapThisRobotToFrontPosition ( enemy* ThisRobot )! \nTerminatin..." );
	      Terminate ( ERR ) ;
	    }

	  //--------------------
	  // Now if this is the server, it must inform the clients
	  // about the recent swap in the enemy array.
	  //
	  if ( ServerMode && ( ! ClientMode ) ) 
	    {
	      SendEnemySwapSignalToClient ( 0 , i , j );
	    }

	  //--------------------
	  // Finally, the swap operation should be done on the server too...
	  //
	  SwapEnemys ( i , j ) ;

	  /*
	  memcpy ( &Zwisch , & ( AllEnemys[ i ] ) , sizeof( enemy ) );
	  memcpy ( & ( AllEnemys[ i ] ) , ThisRobot , sizeof( enemy ) );
	  memcpy ( ThisRobot , & Zwisch , sizeof( enemy ) );
	  */

	  //--------------------
	  // And of course we return, cause further swapping after that does
	  // not seem to make sense.
	  //
	  return;
	}
    }

}; // void SwapThisRobotToFrontPosition ( enemy* ThisRobot )


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

  if ( Me [ 0 ] . base_skill_level [ SPELL_EXTRACT_PLASMA_TRANSISTORS ] )
    {
      if ( Druidmap [ ThisRobot->type ] . amount_of_plasma_transistors )
	DropChestItemAt( ITEM_DROID_PART_1 , ThisRobot->pos.x , ThisRobot->pos.y , -1 , -1 , 2 );
    }
  if ( Me [ 0 ] . base_skill_level [ SPELL_EXTRACT_SUPERCONDUCTORS ] )
    {
      if ( Druidmap [ ThisRobot->type ] . amount_of_superconductors )
	DropChestItemAt( ITEM_DROID_PART_2 , ThisRobot->pos.x , ThisRobot->pos.y , -1 , -1 , 2 );
    }
  if ( Me [ 0 ] . base_skill_level [ SPELL_EXTRACT_ANTIMATTER_CONVERTERS ] )
    {
      if ( Druidmap [ ThisRobot->type ] . amount_of_antimatter_converters )
	DropChestItemAt( ITEM_DROID_PART_3 , ThisRobot->pos.x , ThisRobot->pos.y , -1 , -1 , 2 );
    }
  if ( Me [ 0 ] . base_skill_level [ SPELL_EXTRACT_ENTROPY_INVERTERS ] )
    {
      if ( Druidmap [ ThisRobot->type ] . amount_of_entropy_inverters )
	DropChestItemAt( ITEM_DROID_PART_4 , ThisRobot->pos.x , ThisRobot->pos.y , -1 , -1 , 2 );
    }
  if ( Me [ 0 ] . base_skill_level [ SPELL_EXTRACT_TACHYON_CONDENSATORS ] )
    {
      if ( Druidmap [ ThisRobot->type ] . amount_of_tachyon_condensators )
	DropChestItemAt( ITEM_DROID_PART_5 , ThisRobot->pos.x , ThisRobot->pos.y , -1 , -1 , 2 );
    }

}; // void DropEnemyTreasure ( Enemy ThisRobot )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
int
MakeSureEnemyIsInsideThisLevel ( Enemy ThisRobot )
{

  // if the enemy is outside of the current map, that's an error and needs to be correted.
  if ( ( ThisRobot -> pos . x <= 0 ) || 
       ( ThisRobot -> pos . x >= curShip.AllLevels[ ThisRobot -> pos . z ] -> xlen ) ||
       ( ThisRobot -> pos . y <= 0 ) || 
       ( ThisRobot -> pos . y >= curShip.AllLevels[ ThisRobot -> pos . z ] -> ylen ) )
    {

      GiveStandardErrorMessage ( "MakeSureEnemyIsInsideThisLevel(...)" , "\
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
  //--------------------
  // If the Tux has killed his friend, he will regret it, or at least
  // say so.
  //
  if ( ThisRobot->is_friendly )
    {
      Activate_Conservative_Frame_Computation();
      PlayOnceNeededSoundSample( "Tux_Why_Did_I_0.wav" , FALSE );
    }
  else
    {
      //--------------------
      // The Tux gains experience from this, only for non-friends
      //
      Me[0].Experience += Druidmap[ ThisRobot->type ].score;
    }

  ThisRobot->Status = OUT;
  //--------------------
  // The dead enemy will now explode and drop treasure, provided that 
  // it was still on this map
  //
  if ( MakeSureEnemyIsInsideThisLevel ( ThisRobot ) ) 
    {
      StartBlast ( ThisRobot->pos.x , ThisRobot->pos.y , ThisRobot->pos.z , DRUIDBLAST );
      Me [ 0 ] . KillRecord [ ThisRobot -> type ] ++ ;
      //--------------------
      // Maybe that robot did have something with him?  The item should then
      // fall to the floor with it's clanc
      //
      // Maybe the robots was also a boss monster.  Then some additional items
      // must be dropped and they must always be magical.
      //
      DropEnemyTreasure ( ThisRobot ) ;
    }
  
  if ( !ClientMode ) SwapThisRobotToFrontPosition ( ThisRobot );

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
  if ( ( fabsf ( AllEnemys[Enum].speed.y ) > 1 ) || ( fabsf ( AllEnemys[Enum].speed.x ) > 1 ) )
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
  if ( ThisRobot->warten > 0) return;

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

  PermanentHealRobots ();  // enemy robots heal as time passes...

  AnimateEnemys ();	// move the "phase" of the rotation of enemys

  for (i = 0; i < Number_Of_Droids_On_Ship ; i++)
     {
       ThisRobot = & AllEnemys[ i ];

       //--------------------
       // ignore dead robots as well...
       if ( ThisRobot->Status == OUT ) continue;

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
       if ( ( ! IsActiveLevel ( ThisRobot->pos.z ) )  && 
	    ( ! ThisRobot -> FollowingInflusTail ) ) 
	 continue;
  
       //--------------------
       // Now we do the movement, either to the next waypoint OR if one
       // is set, we persue the given course, which allows complete control
       // of the bot moves (via the state machine for example)...
       //
       ProcessAttackStateMachine ( i );

     }	// for Number_Of_Droids_On_Ship

}; // MoveEnemys( void ) 

/* ----------------------------------------------------------------------
 * This function is low-level:  It simply sets off a shot from enemy
 * through the pointer ThisRobot at the target VECTOR xdist ydist, which
 * is a DISTANCE VECTOR, NOT ABSOLUTE COORDINATES OF THE TARGET!!!
 * ---------------------------------------------------------------------- */
void 
RawStartEnemysShot( enemy* ThisRobot , float xdist , float ydist )
{
  int guntype = ItemMap[ Druidmap[ThisRobot->type].weapon_item.type ].item_gun_bullet_image_type;
  double bullet_speed = ItemMap[ Druidmap[ ThisRobot->type ].weapon_item.type ].item_gun_speed;
  int j;
  float OffsetFactor;
  bullet* NewBullet=NULL;

  Fire_Bullet_Sound ( guntype );
  
  // find a bullet entry, that isn't currently used... 
  for (j = 0; j < MAXBULLETS; j++)
    {
      if (AllBullets[ j ].type == OUT)
	{
	  NewBullet = & ( AllBullets[j] );
	  break;
	}
    }
  if ( NewBullet == NULL )
    {
      DebugPrintf ( 0 , "\nvoid RawStartEnemysShot ( ... ) :  Ran out of Bullets.... Terminating....");
      Terminate (ERR);
    }
  
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
  NewBullet->pos.x = ThisRobot->pos.x;
  NewBullet->pos.y = ThisRobot->pos.y;
  NewBullet->pos.z = ThisRobot->pos.z;
  
  // fire bullets so, that they don't hit the shooter...
  if ( NewBullet->angle_change_rate == 0 ) OffsetFactor = 0.5; else OffsetFactor = 1;
  NewBullet->pos.x +=
    (NewBullet->speed.x) / (bullet_speed) * OffsetFactor ;
  NewBullet->pos.y +=
    (NewBullet->speed.y) / (bullet_speed) * OffsetFactor ;
  
  // wait for as long as is usual for this weapon type until making the next shot
  ThisRobot->firewait = ItemMap [ Druidmap[ThisRobot->type].weapon_item.type ].item_gun_recharging_time ;
  
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
 * determine the distance vector to the target of this shot.  The target
 * depends of course on wheter it's a friendly device or a hostile device.
 * ---------------------------------------------------------------------- */
int
DetermineVectorToShotTarget( enemy* ThisRobot , moderately_finepoint* vect_to_target )
{
  int j = 0 ;
  int TargetPlayerNum;
  float IgnoreRange = Druidmap [ ThisRobot -> type ] . minimal_range_hostile_bots_are_ignored;

  if ( ThisRobot->is_friendly == TRUE )
    {
      // Since it's a friendly device in this case, it will aim at the (closest?) of
      // the MS bots.
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

    }
  else
    {

      TargetPlayerNum = ClosestVisiblePlayer ( ThisRobot ) ;
      vect_to_target -> x = Me [ TargetPlayerNum ] . pos . x - ThisRobot -> pos . x ;
      vect_to_target -> y = Me [ TargetPlayerNum ] . pos . y - ThisRobot -> pos . y ;

    }

  // Add some security against division by zero
  if ( vect_to_target->x == 0) vect_to_target->x = 0.1;
  if ( vect_to_target->y == 0) vect_to_target->y = 0.1;

  if ( j < Number_Of_Droids_On_Ship - 1 ) return ( TRUE );
  else return ( FALSE );

}; // int DetermineVectorToShotTarget( enemy* ThisRobot , & vect_to_target )

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
  if ( ( DruidPassable ( ThisRobot -> pos.x + StepVector.x , 
			 ThisRobot -> pos.y + StepVector.y ,
			 ThisRobot -> pos.z ) == CENTER ) &&
       ( CheckIfWayIsFreeOfDroids ( ThisRobot->pos.x , ThisRobot->pos.y , 
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
MoveInCloserForOrAwayFromMeleeCombat ( Enemy ThisRobot , int TargetPlayer , int enemynum , int DirectionSign )
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
  VictimPosition . x = Me [ TargetPlayer ] . pos . x ;
  VictimPosition . y = Me [ TargetPlayer ] . pos . y ;
  CurrentPosition . x = ThisRobot -> pos . x ;
  CurrentPosition . y = ThisRobot -> pos . y ;

  StepVector . x = VictimPosition . x - CurrentPosition . x ;
  StepVector . y = VictimPosition . y - CurrentPosition . y ;
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
      if ( DruidPassable ( ThisRobot -> pos.x + StepVector.x , 
			   ThisRobot -> pos.y + StepVector.y ,
			   ThisRobot -> pos.z ) == CENTER )
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
ProcessAttackStateMachine (int enemynum)
{
  moderately_finepoint vect_to_target;
  float dist2;
  Enemy ThisRobot = & AllEnemys[ enemynum ] ;
  int TargetPlayer;
  int TargetIsEnemy;

  //--------------------
  // At first, we check for a lot of cases in which we do not
  // need to move anything for this reason or for that
  //
  // ignore robots on other levels 
  // if ( ThisRobot->pos.z != CurLevel->levelnum) return;
  if ( ! IsActiveLevel ( ThisRobot -> pos . z ) ) return;

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
      ThisRobot->TextToBeDisplayed = "state:  Unaware of Tux." ;
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
      ThisRobot -> combat_state = RUSH_TUX_ON_SIGHT_AND_OPEN_TALK ;
      ThisRobot -> will_rush_tux = FALSE ;
    }

  //--------------------
  // determine the distance vector to the target of this shot.  The target
  // depends of course on wheter it's a friendly device or a hostile device.
  //
  TargetIsEnemy = DetermineVectorToShotTarget ( ThisRobot , & vect_to_target ) ;

  if ( ThisRobot -> is_friendly && TargetIsEnemy && ( sqrt ( vect_to_target.x * vect_to_target.x + vect_to_target.y * vect_to_target.x  ) < Druidmap [ ThisRobot -> type ] . minimal_range_hostile_bots_are_ignored ) )
    ThisRobot -> combat_state = FIGHT_ON_TUX_SIDE ;

  // vect_to_target.x = 1;
  // vect_to_target.y = 1;

  dist2 = sqrt( vect_to_target.x * vect_to_target.x + vect_to_target.y * vect_to_target.y );
  // dist2 = DistanceToTux ( ThisRobot );

  TargetPlayer = ClosestVisiblePlayer ( ThisRobot ) ;

  if ( ThisRobot -> combat_state == RUSH_TUX_ON_SIGHT_AND_OPEN_TALK )
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
      if ( RemainingDistanceToNextWaypoint ( ThisRobot ) < 0.1 ) 
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
      if ( ThisRobot->has_greeted_influencer == FALSE )
	{
	  ThisRobot->has_greeted_influencer = TRUE;
	  if ( Druidmap[ ThisRobot->type ].greeting_sound_type != (-1) )
	    {
	      PlayGreetingSound( Druidmap[ ThisRobot->type ].greeting_sound_type );
	    }
	}

      if ( ! ThisRobot -> will_rush_tux )
	{
	  ThisRobot -> combat_state = STOP_AND_EYE_TUX ;
	  ThisRobot -> state_timeout = Druidmap [ ThisRobot -> type ] . time_spent_eyeing_tux ;
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
				     240 )
	  )
	{
	  ThisRobot -> combat_state = MOVE_ALONG_RANDOM_WAYPOINTS ;
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

      if ( ! ThisRobot -> warten ) 
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
	  
	  if ( ( DistanceToTux ( ThisRobot ) > Druidmap [ ThisRobot->type ] . range_of_vision ) )
	    {
	      // ThisRobot -> combat_state = MOVE_ALONG_RANDOM_WAYPOINTS ;
	      ThisRobot -> combat_state = TURN_THOWARDS_NEXT_WAYPOINT ;
	      ThisRobot -> persuing_given_course = FALSE ;
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
		  ThisRobot -> combat_state = MAKE_ATTACK_RUN ;
		  ThisRobot -> persuing_given_course = FALSE ;
		  SetRestOfGroupToState ( ThisRobot , MAKE_ATTACK_RUN );

		  //--------------------
		  // We'll launch the attack cry of this bot...
		  //
		  if ( Druidmap[ ThisRobot->type ].greeting_sound_type != (-1) )
		    {
		      PlayStartAttackSound( Druidmap[ ThisRobot->type ].greeting_sound_type );
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
    }
  else if ( ThisRobot -> combat_state == FIGHT_ON_TUX_SIDE )
    {
      MoveThisEnemy ( enemynum ); // this will now be done in the attack state machine...

      if ( ! EnemyOfTuxCloseToThisRobot ( ThisRobot , & vect_to_target ) ) 
	{
	  ThisRobot -> combat_state = MOVE_ALONG_RANDOM_WAYPOINTS ;
	  return;
	}
    }
  else
    {
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
  if ( ! IsVisible ( &ThisRobot->pos , TargetPlayer ) && 
       ( ThisRobot->is_friendly == FALSE ) ) 
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
	  MoveInCloserForOrAwayFromMeleeCombat ( ThisRobot , TargetPlayer , enemynum , (+1) );
	} // if a melee weapon is given.
      else if (dist2 < 1.5)
	{
	  MoveInCloserForOrAwayFromMeleeCombat ( ThisRobot , TargetPlayer , enemynum , (-1) );
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
  
}; // void AttackInfluence ( int enemynum )

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
	  if ( OurBot->warten)
	    {
	      // keep waiting
	      OurBot->warten = WAIT_COLLISION;
	      continue;
	    }

	  // otherwise: stop this one enemy and go back youself
	  ListEnemy->warten = WAIT_COLLISION;

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
 * This function does the rotation of the enemys according to their 
 * current energy level.
 * ---------------------------------------------------------------------- */
void
AnimateEnemys (void)
{
  int i;

  // for (i = 0; i < MAX_ENEMYS_ON_SHIP ; i++)
  for (i = 0; i < Number_Of_Droids_On_Ship ; i++)
    {
      /* ignore enemys that are dead or on other levels or dummys */
      // if (AllEnemys[i].type == DEBUG_ENEMY) continue;
      // if (AllEnemys[i].pos.z != CurLevel->levelnum)
      if ( AllEnemys[i].pos.z != Me [ 0 ] . pos . z )
	continue;

      if (AllEnemys[i].Status == OUT)
	{
	  AllEnemys[i].phase = DROID_PHASES ;
	  continue;
	}

      if ( AllEnemys[i].energy <= 0 ) 
	{
	  DebugPrintf( 1 , "\nAnimateEnemys: WARNING: Enemy with negative energy encountered.  Phase correction forced..." );
	  AllEnemys[i].phase = 0 ;
	}
      else
	{
	  // AllEnemys[i].feindrehcode+=AllEnemys[i].energy;
	  AllEnemys[i].phase +=
	    (AllEnemys[i].energy / Druidmap[AllEnemys[i].type].maxenergy) *
	    Frame_Time () * DROID_PHASES * 2.5;
	}

      if (AllEnemys[i].phase >= DROID_PHASES)
	{
	  AllEnemys[i].phase = 0;
	}
    }
}; // void AnimateEnemys ( void )

#undef _enemy_c
