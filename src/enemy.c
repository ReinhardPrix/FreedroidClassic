/* 
 *
 *   Copyright (c) 1994, 2002 Johannes Prix
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
/*
 * This file has been checked for remains of german comments in the code
 * I you still find some, please just kill it mercilessly.
 */
#define _enemy_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"

#define COL_SPEED		3	

#define FIREDIST2	8 // according to the intro, the laser can be "focused on any target
                          // within a range of eight metres"

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
	  switch ( ThisBotsLevel->map[j][i] )
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
      if (AllEnemys[i].Status == OUT)
	continue;
      if (AllEnemys[i].energy < Druidmap[AllEnemys[i].type].maxenergy)
	AllEnemys[i].energy += Druidmap[AllEnemys[i].type].lose_health * Frame_Time();
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
      AllEnemys[i].Marker = 0;
      AllEnemys[i].is_friendly = 0;
      AllEnemys[i].TextVisibleTime = 0;
      AllEnemys[i].TextToBeDisplayed = "";
      AllEnemys[i].persuing_given_course = FALSE;
      AllEnemys[i].FollowingInflusTail = FALSE;
      AllEnemys[i].StayHowManySecondsBehind = 5;
      
      for ( j=0 ; j < MAX_STEPS_IN_GIVEN_COURSE ; j++ )
	{
	  AllEnemys[i].PrivatePathway[j].x=0;
	  AllEnemys[i].PrivatePathway[j].y=0;
	}

      for ( j = 0 ; j < MAX_CHAT_KEYWORDS_PER_DROID * 2 ; j++ )
	{
	  AllEnemys[i].QuestionResponseList[j]="";
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
  // int curlevel = CurLevel->levelnum;
  int i, j;
  int nth_enemy;
  int wp_num;
  int wp = 0;
  int BestWaypoint;
  finepoint influ_coord;
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
      if ( AllEnemys[i].SpecialForce )
	{
	  BestWaypoint = 0;
	  for ( j=0 ; j<MAXWAYPOINTS ; j ++ )
	    {
	      if ( abs ( ( ShuffleLevel -> AllWaypoints[j].x) - AllEnemys[i].pos.x ) < 
		   abs ( ShuffleLevel -> AllWaypoints[ BestWaypoint ].x - AllEnemys[i].pos.x ) )
		BestWaypoint = j;
	    }
	  AllEnemys[i].nextwaypoint = BestWaypoint;
	  AllEnemys[i].lastwaypoint = BestWaypoint;
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

    }/* for (MAX_ENEMYS_ON_SHIP) */

  /* leave the enemys alone for some time.... */

  /* we shortly remove the influencer, so that it does not interfere with the movement */
  influ_coord.x = Me[0].pos.x;
  influ_coord.y = Me[0].pos.y;
  Me[0].pos.x = Me[0].pos.y = 0;

  for (i = 0; i < 30; i++)     MoveEnemys ();

  /* now we can put the influencer back in */
  Me[0].pos.x = influ_coord.x;
  Me[0].pos.y = influ_coord.y;

}; // void ShuffleEnemys ( void ) 

/* ----------------------------------------------------------------------
 * This function selects the next waypoint for a droid, with a random
 * principle, not taking anything but existing waypoint connections into
 * account.
 * ---------------------------------------------------------------------- */
void 
SelectNextWaypointClassical( int EnemyNum )
{
  int j;
  finepoint Restweg;
  Waypoint WpList;		/* Pointer to waypoint-liste */
  int nextwp;
  finepoint nextwp_pos;
  int trywp;
  Enemy ThisRobot=&AllEnemys[ EnemyNum ];
  Level WaypointLevel = curShip.AllLevels[ AllEnemys[ EnemyNum ].pos.z ];

  // We do some definitions to save us some more typing later...
  WpList = WaypointLevel->AllWaypoints;
  nextwp = ThisRobot->nextwaypoint;
  // maxspeed = Druidmap[ ThisRobot->type ].maxspeed;
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
      ThisRobot->lastwaypoint = ThisRobot->nextwaypoint;
      ThisRobot->warten = MyRandom (ENEMYMAXWAIT);
      
      // search for possible connections from here
      // but only if there are connections possible 
      for ( j=0; j<MAX_WP_CONNECTIONS; j++ )
	if ( WpList[nextwp].connections[j] != -1 )
	  break;
      if ( j < MAX_WP_CONNECTIONS )
	while ( (trywp = WpList[nextwp].
		 connections[MyRandom (MAX_WP_CONNECTIONS - 1)]) == -1);
      else
	{
	  DebugPrintf ( 0, "\nvoid MoveThisRobotClassical ( int Enemynum ) : Weird waypoint %d has no connections!\n", nextwp);
	  Terminate(ERR);
	}
      
      /* setze neuen Waypoint */
      ThisRobot->nextwaypoint = trywp;
    }			/* if */

}; // void MoveThisRobotClassical ( int Enemynum );

/* ----------------------------------------------------------------------
 * This function checks if the connection between two points is free of
 * droids.  
 *
 * MAP TILES ARE NOT TAKEN INTO CONSIDERATION, ONLY DROIDS!!!
 *
 * ---------------------------------------------------------------------- */
int 
CheckIfWayIsFreeOfDroids ( float x1 , float y1 , float x2 , float y2 , int OurLevel , int ExceptedDroid )
{
  float LargerDistance;
  int Steps;
  int i, j;
  finepoint step;
  finepoint CheckPosition;

  DebugPrintf( 2, "\nint CheckIfWayIsFreeOfDroids (...) : Checking from %d-%d to %d-%d.", (int) x1, (int) y1 , (int) x2, (int) y2 );
  fflush(stdout);

  if ( abs(x1-x2) > abs (y1-y2) ) LargerDistance=fabsf(x1-x2);
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
	  if ( j == ExceptedDroid ) continue;

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
 * This function moves one robot thowards his next waypoint.  If already
 * there, the function does nothing more.
 * ---------------------------------------------------------------------- */
void 
MoveThisRobotThowardsHisWaypoint ( int EnemyNum )
{
  finepoint Restweg;
  Waypoint WpList;		/* Pointer to waypoint-list */
  int nextwp;
  finepoint nextwp_pos;
  float maxspeed;
  Enemy ThisRobot=&AllEnemys[ EnemyNum ];
  int HistoryIndex;
  Level WaypointLevel = curShip.AllLevels[ AllEnemys[ EnemyNum ].pos.z ];

  DebugPrintf( 2 , "\n void MoveThisRobotThowardsHisWaypoint ( int EnemyNum ) : real function call confirmed. ");

  // We do some definitions to save us some more typing later...
  WpList = WaypointLevel->AllWaypoints;
  nextwp = ThisRobot->nextwaypoint;

  //--------------------
  // According to properties of the robot like being frozen or not,
  // we define the maximum speed of this machine for later use...
  //
  // maxspeed = Druidmap[ ThisRobot->type ].maxspeed;
  if ( ThisRobot -> paralysation_duration_left != 0 )
    {
      return;
      // maxspeed = ItemMap[ Druidmap[ ThisRobot->type ].drive_item.type ].item_drive_maxspeed;
    }
  if ( ThisRobot -> frozen == 0 )
    {
      maxspeed = ItemMap[ Druidmap[ ThisRobot->type ].drive_item.type ].item_drive_maxspeed;
    }
  else 
    {
      maxspeed = 0.2 * ItemMap[ Druidmap[ ThisRobot->type ].drive_item.type ].item_drive_maxspeed;
    }

  nextwp_pos.x = WpList[nextwp].x;
  nextwp_pos.y = WpList[nextwp].y;

  if ( ThisRobot->persuing_given_course )
    {
      nextwp_pos.x = ThisRobot->PrivatePathway[0].x;
      nextwp_pos.y = ThisRobot->PrivatePathway[0].y;
    }

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

  // determine the remaining way until the target point is reached
  Restweg.x = nextwp_pos.x - ThisRobot->pos.x;
  Restweg.y = nextwp_pos.y - ThisRobot->pos.y;

  // --------------------
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
      // ThisRobot->pos.y += (nextwp_pos.y-ThisRobot->pos.y)*Frame_Time();
      ThisRobot->pos.y = nextwp_pos.y;
      ThisRobot->speed.y = 0;
    }

}; // void MoveThisRobotThowardsHisWaypoint ( int EnemyNum )

/* ----------------------------------------------------------------------
 * This function selects the next waypoints or 'parawaypoints' for the
 * droid along some predefined course.
 * 
 ----------------------------------------------------------------------*/
void 
Persue_Given_Course ( int EnemyNum )
{
  moderately_finepoint Restweg;
  Waypoint WpList;		/* Pointer to waypoint-liste */
  int nextwp;
  finepoint nextwp_pos;
  float maxspeed;
  Enemy ThisRobot=&AllEnemys[ EnemyNum ];
  Level WaypointLevel = curShip.AllLevels[ AllEnemys[ EnemyNum ].pos.z ];

  DebugPrintf( 2 , "\n void MoveThisRobotAdvanced ( int EnemyNum ) : real function call confirmed. ");

  if ( ThisRobot->persuing_given_course == FALSE ) return;

  DebugPrintf( 2 , "\nvoid MoveThisRobotAdvanced ( int EnemyNum ) : Robot now on given course!!!. ");

  // We do some definitions to save us some more typing later...
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

  ThisRobot->TextVisibleTime = 0;
  ThisRobot->TextToBeDisplayed = "Persuing given course!!";

}; // Persue_Given_Waypoint_Course


/* ----------------------------------------------------------------------
 * This function moves one robot in an advanced way, that hasn't been
 * present within the classical paradroid game.
 * ---------------------------------------------------------------------- */
void 
SelectNextWaypointAdvanced ( int EnemyNum )
{
  int i,j;
  finepoint Restweg;
  Waypoint WpList;		/* Pointer to waypoint-liste */
  int nextwp;
  finepoint nextwp_pos;
  int trywp;
  float maxspeed;
  Enemy ThisRobot=&AllEnemys[ EnemyNum ];
  int FreeWays[ MAX_WP_CONNECTIONS ];
  int SolutionFound;
  int TestConnection;
  Level WaypointLevel = curShip.AllLevels[ AllEnemys[ EnemyNum ].pos.z ];

  DebugPrintf( 2 , "\n void MoveThisRobotAdvanced ( int EnemyNum ) : real function call confirmed. ");

  if ( ThisRobot->persuing_given_course == TRUE ) return;

  // We do some definitions to save us some more typing later...
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
      ThisRobot->lastwaypoint = ThisRobot->nextwaypoint;
      ThisRobot->warten = MyRandom (ENEMYMAXWAIT);

      //--------------------
      // This statement should make hostile droids with aggresssion
      // and a ranged weapon
      // wait, if they see the influencer and are at their waypoint now.
      // Then they (in some other function) open fire and should do
      // that, until the influencer vanishes out of sight, which should cause them
      // to go into a hunting mode. (to be implemented later).
      //
      if ( ( !( ItemMap [ Druidmap [ ThisRobot -> type ] . weapon_item . type ] . item_gun_angle_change > 0 ) ) &&
	   Druidmap[ThisRobot->type].aggression &&
	   IsVisible ( & ( ThisRobot -> pos ) , 0 ) && // WARNING!  Player 0 here always is wrong
	   // ! ItemMap [ Druidmap [ ThisRobot->type ].weapon_item.type ].item_gun_angle_change  &&
	   ! ThisRobot->is_friendly &&
	   ( sqrt ( ( ThisRobot->pos.x - Me[0].pos.x ) * ( ThisRobot->pos.x - Me[0].pos.x ) +
		    ( ThisRobot->pos.y - Me[0].pos.y ) * ( ThisRobot->pos.y - Me[0].pos.y ) ) > 1.5 ) )
	{
	  // But now that the enemy is are almost ready to fire, it just
	  // might also say something.  But of course it will not repeat this
	  // sentence every frame.  Therefore we may only say something when
	  // this happens the first time i.e. when the robot was not waiting
	  // already.
	  //
	  if ( ThisRobot->warten == 0)
	    {
	      AddStandingAndAimingText( EnemyNum );
	    }


	  ThisRobot->warten=2;
	  return;
	}

						   
      
      // search for possible connections from here...
      DebugPrintf (2, "\nMoveRobotAdvanced: searching for possible connections...");

      // search for the first connection, that doesn't exist any more, so
      // that we know, which connections surely do exist
      for ( j=0; j<MAX_WP_CONNECTIONS; j++ )
	if ( WpList[nextwp].connections[j] == -1 )
	  break;

      // Of course, only if such connections exist at all, we do the
      // following change of target waypoint procedure
      if ( j < MAX_WP_CONNECTIONS )
	{

	  // At this point, we should check, if there is another waypoint 
	  // and also if the way there is free of other droids
	  for ( i = 0; i < j ; i++ )
	    {
	      FreeWays[i] = CheckIfWayIsFreeOfDroids ( WpList[ThisRobot->lastwaypoint].x , WpList[ThisRobot->lastwaypoint].y , WpList[WpList[ThisRobot->lastwaypoint].connections[i]].x , WpList[WpList[ThisRobot->lastwaypoint].connections[i]].y , ThisRobot->pos.z , EnemyNum );
	    }

	  // Now see whether any way point at all is free in that sense
	  // otherwise we set this robot to waiting and return;
	  for ( i = 0 ; i < j ; i++ )
	    {
	      if ( FreeWays[i] ) break;
	    }

	  if ( i == j )
	    {
	      DebugPrintf( 2 , "\n Sorry, there seems no free way out.  I'll wait then... , j was : %d ." , j);
	      ThisRobot->warten = 1;
	      return;
	    }

	  // Now that we know, there is some way out of this, we can test around
	  // and around randomly until we finally find some solution.
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
	  DebugPrintf (1, "\nWeird waypoint %d has no connections!\n", nextwp);
	  // Terminate(ERR);
	}
      
      // set new waypoint...
      ThisRobot->nextwaypoint = trywp;
    }			/* if */
}; // void MoveThisRobotAdvanced ( int EnemyNum )

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
  int i;

  DropRandomItem ( ThisRobot->pos.x , ThisRobot->pos.y , Druidmap [ ThisRobot->type ].monster_level , 
		   ! ( MyRandom ( 6 ) ) , FALSE ) ;
  for ( i = 0 ; i < Druidmap [ ThisRobot->type ].forced_magic_items ; i ++ )
    {
      switch ( i )
	{
	case 0:
	  DropRandomItem ( ThisRobot->pos.x , ThisRobot->pos.y + 0.5 , 
			   Druidmap [ ThisRobot->type ].monster_level , TRUE , TRUE ) ;
	  break;
	case 1:
	  DropRandomItem ( ThisRobot->pos.x , ThisRobot->pos.y - 0.5 , 
			   Druidmap [ ThisRobot->type ].monster_level , TRUE , TRUE ) ;
	  break;
	case 2:
	  DropRandomItem ( ThisRobot->pos.x + 0.5 , ThisRobot->pos.y , 
			   Druidmap [ ThisRobot->type ].monster_level , TRUE , TRUE ) ;
	  break;
	case 3:
	  DropRandomItem ( ThisRobot->pos.x - 0.5 , ThisRobot->pos.y , 
			   Druidmap [ ThisRobot->type ].monster_level , TRUE , TRUE ) ;
	  break;
	default:
	  DropRandomItem ( ThisRobot->pos.x -0.7 + 0.07 * MyRandom(20) , 
			   ThisRobot->pos.y -0.7 + 0.07 * MyRandom(20) , 
			   Druidmap [ ThisRobot->type ].monster_level , TRUE , TRUE ) ;
	  break;
	}
    }
}; // void DropEnemyTreasure ( Enemy ThisRobot )

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
  //--------------------
  // The dead enemy will now explode.
  //
  ThisRobot->Status = OUT;
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
  
  if (LevelEmpty ())
    curShip.AllLevels[ Me[ 0 ] . pos . z ] -> empty = WAIT_LEVELEMPTY;

  if ( !ClientMode ) SwapThisRobotToFrontPosition ( ThisRobot );

}; // void InitiateDeathOfEnemy ( Enemy ThisRobot )

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
  // ignore robots on other levels, except, it it's following influ's trail
  // if ( ( ThisRobot->pos.z != CurLevel->levelnum) && (!ThisRobot->FollowingInflusTail) ) return;
  if ( ( ! IsActiveLevel ( ThisRobot->pos.z ) )  && ( ! ThisRobot -> FollowingInflusTail ) ) return;

  // ignore dead robots as well...
  if ( ThisRobot->Status == OUT ) return;

  //--------------------
  // Now check if the robot is still alive
  // if the robot just got killed, initiate the
  // explosion and all that...
  if ( ThisRobot->energy <= 1)
    {
      InitiateDeathOfEnemy ( ThisRobot );

      return;	// this one's down, so we can move on to the next
    }
  
  //--------------------
  // ignore all enemys with CompletelyFixed flag set...
  //
  if ( ThisRobot->CompletelyFixed ) return;

  //--------------------
  // robots that still have to wait also do not need to
  // be processed for movement
  //
  if ( ThisRobot->warten > 0) return;

  //--------------------
  // Now check for collisions of this enemy with his colleagues
  //
  CheckEnemyEnemyCollision ( EnemyNum );

  //--------------------
  // Now comes the real movement part
  //
  MoveThisRobotThowardsHisWaypoint( EnemyNum );

  if ( Druidmap[ThisRobot->type].advanced_behaviour )
    SelectNextWaypointAdvanced( EnemyNum );
  else
    SelectNextWaypointClassical( EnemyNum );

  Persue_Given_Course( EnemyNum );

}; // void MoveThisEnemy ( int EnemyNum )

/* ----------------------------------------------------------------------
 * This function moves all enemys individually, using MoveThisEnemy(i)
 * and it also initiates the robots fireing behaviour via 
 * AttackInfluence (i) all individually.
 * ---------------------------------------------------------------------- */
void
MoveEnemys (void)
{
  int i;

  PermanentHealRobots ();  // enemy robots heal as time passes...

  AnimateEnemys ();	// move the "phase" of the rotation of enemys

  for (i = 0; i < Number_Of_Droids_On_Ship ; i++)
     {

       MoveThisEnemy(i);

       // If its a combat droid, then if might attack...
       if (Druidmap[AllEnemys[i].type].aggression)
	 AttackInfluence (i);

    }	/* for (MAX_ENEMYS_ON_SHIP) */

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
  
  
  NewBullet->angle = - ( 90 + 180 * atan2 ( NewBullet->speed.y,  NewBullet->speed.x ) / M_PI );  
  
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
 * determine the distance vector to the target of this shot.  The target
 * depends of course on wheter it's a friendly device or a hostile device.
 * ---------------------------------------------------------------------- */
void
DetermineVectorToShotTarget( enemy* ThisRobot , moderately_finepoint* vect_to_target )
{
  int j;
  int TargetPlayerNum;

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

	  // At this point we have found our target
	  vect_to_target -> x = AllEnemys [ j ] . pos . x - ThisRobot -> pos . x ;
	  vect_to_target -> y = AllEnemys [ j ] . pos . y - ThisRobot -> pos . y ;
	  DebugPrintf( 0 , "\nPOSSIBLE TARGET FOR FRIENDLY DROID FOUND!!!\n");
	  break;
	}
      // Maybe we havn't found a single target.  Then we don't attack anything of course.
      if ( j >= Number_Of_Droids_On_Ship ) 
	{
	  ThisRobot->firewait = 1.0 ;
	  return; 
	}
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

}; // void DetermineVectorToShotTarget( enemy* ThisRobot , & vect_to_target )

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
				    ThisRobot->pos.z , enemynum ) ) )
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
 * MoveThisRobotThowardsHisWaypoint or so, cause what this function does
 * is setting some new course parameters, NOT really alter the position
 * of this robot directly.
 * ---------------------------------------------------------------------- */
void
MoveInCloserForMeleeCombat ( Enemy ThisRobot , int TargetPlayer , int enemynum )
{
  float StepSize;
  finepoint VictimPosition;
  finepoint ConsideredGoodPosition;
  finepoint CurrentPosition;
  moderately_finepoint StepVector;
  float StepVectorLen;

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
  StepSize = 0.5;

  VictimPosition . x = Me [ TargetPlayer ] . pos . x ;
  VictimPosition . y = Me [ TargetPlayer ] . pos . y ;
  CurrentPosition . x = ThisRobot -> pos . x ;
  CurrentPosition . y = ThisRobot -> pos . y ;

  StepVector . x = VictimPosition . x - CurrentPosition . x ;
  StepVector . y = VictimPosition . y - CurrentPosition . y ;
  StepVectorLen = sqrt ( ( StepVector . x ) * ( StepVector . x ) + ( StepVector . y ) * ( StepVector . y ) );

  StepVector . x /= ( 2 * StepVectorLen ) ;
  StepVector . y /= ( 2 * StepVectorLen ) ;

  //--------------------
  // Now we have assembled the simplest of ideas:  Try to move directly
  // thowards the Tux.  We just need to check if that does make some
  // sense to move there.  Otherwise we can still consider some variations
  // to the left or right.
  //
  ConsideredGoodPosition . x = CurrentPosition . x + StepVector . x ;
  ConsideredGoodPosition . y = CurrentPosition . y + StepVector . y ;

  if ( ConsideredMoveIsFeasible ( ThisRobot , StepVector , enemynum ) )
    {
      ThisRobot -> PrivatePathway [ 0 ] . x = ThisRobot -> pos.x + StepVector . x ;
      ThisRobot -> PrivatePathway [ 0 ] . y = ThisRobot -> pos.y + StepVector . y ;
    }
  else
    {
      //--------------------
      // If we didn't have any luck with the direct vector, we may have
      // more luck with a 45 degree rotated vector.  So let's try that.
      //
      RotateVectorByAngle ( & StepVector , 45 ) ;
      if ( ConsideredMoveIsFeasible ( ThisRobot , StepVector , enemynum ) )
	{
	  ThisRobot -> PrivatePathway [ 0 ] . x = ThisRobot -> pos.x + StepVector . x ;
	  ThisRobot -> PrivatePathway [ 0 ] . y = ThisRobot -> pos.y + StepVector . y ;
	}
      else
	{
	  //--------------------
	  // And if all else has failed, we could still try rotating (the meanwhile
	  // 45 degree rotated vector) into the other direction, which is the same
	  // as rotating is again by (360-90) degrees.
	  //
	  RotateVectorByAngle ( & StepVector , 360 - 90 ) ;
	  if ( ConsideredMoveIsFeasible ( ThisRobot , StepVector , enemynum ) )
	    {
	      ThisRobot -> PrivatePathway [ 0 ] . x = ThisRobot -> pos.x + StepVector . x ;
	      ThisRobot -> PrivatePathway [ 0 ] . y = ThisRobot -> pos.y + StepVector . y ;
	    }
	}
    }

}; // void MoveInCloserForMeleeCombat ( Enemy ThisRobot , int TargetPlayer , int enemynum )

/* ----------------------------------------------------------------------
 * This function sometimes fires a bullet from enemy number enemynum 
 * directly into the direction of the influencer, but of course only if 
 * the odds are good i.e. requirements are met.
 * ---------------------------------------------------------------------- */
void
AttackInfluence (int enemynum)
{
  moderately_finepoint vect_to_target;
  float dist2;
  Enemy ThisRobot = & AllEnemys[ enemynum ] ;
  float StepSize;
  float TargetRange;
  int TargetPlayer;

  //--------------------
  // At first, we check for a lot of cases in which we do not
  // need to move anything for this reason or for that
  //

  // ignore robots on other levels 
  // if ( ThisRobot->pos.z != CurLevel->levelnum) return;
  if ( ! IsActiveLevel ( ThisRobot -> pos . z ) ) return;

  // ignore dead robots as well...
  if ( ThisRobot -> Status == OUT ) return;

  // ignore paralyzed robots as well...
  if ( ThisRobot -> paralysation_duration_left != 0 ) return;

  // ignore robots, that don't have any weapon
  if ( Druidmap [ ThisRobot -> type ] . weapon_item . type == ( -1 ) ) return;

  //--------------------
  // If some special command was given, like 
  // ATTACK_FIXED_MAP_POSITION=1, then we IGNORE EVERYTHING AND JUST FIRE OUR BULLETS,
  // NO MATTER WHOS IN THE LINE OF FIRE, HOW FAR WE ARE AWAY OR ANYTHING, JUST BLUNTLY
  // DO THAT.
  //
  if ( ThisRobot->AdvancedCommand == 1 )
    {
      if (ThisRobot->firewait) return;  // can't fire:  gun not yet reloaded...
      vect_to_target.x = ThisRobot->Parameter1 - ThisRobot->pos.x;
      vect_to_target.y = ThisRobot->Parameter2 - ThisRobot->pos.y;
      RawStartEnemysShot( ThisRobot , vect_to_target.x , vect_to_target.y );
      return;
    }

  //--------------------
  // determine the distance vector to the target of this shot.  The target
  // depends of course on wheter it's a friendly device or a hostile device.
  //
  DetermineVectorToShotTarget ( ThisRobot , & vect_to_target ) ;
  // vect_to_target.x = 1;
  // vect_to_target.y = 1;

  dist2 = sqrt( vect_to_target.x * vect_to_target.x + vect_to_target.y * vect_to_target.y );

  //====================
  //
  // From here on, it's classical Paradroid robot behaviour concerning fireing....
  //

  if ( ( dist2 >= FIREDIST2 ) && ( ThisRobot->is_friendly == FALSE ) ) return; // distance limitation only for MS mechs

  TargetPlayer = ClosestVisiblePlayer ( ThisRobot ) ;

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

  //--------------------
  // At this point we know, that the influencer is visible!  Perhaps we have
  // not yet 'greeted' him!  If that is so, we need to do that now:
  //
  if ( ThisRobot->has_greeted_influencer == FALSE )
    {
      ThisRobot->has_greeted_influencer = TRUE;
      if ( Druidmap[ ThisRobot->type ].greeting_sound_type != (-1) )
	{
	  PlayGreetingSound( Druidmap[ ThisRobot->type ].greeting_sound_type );
	}
    }


  //--------------------
  // For melee weapons, we can't just stand anywhere and try to
  // hit the influencer,  In most cases, we will have to move thowards
  // our target.  Here, this need is hopefully satisfied....
  //
  if ( ItemMap [ Druidmap [ ThisRobot -> type ] . weapon_item . type ] . item_gun_angle_change != 0 )
    {

      MoveInCloserForMeleeCombat ( ThisRobot , TargetPlayer , enemynum );

    } // if a melee weapon is given.
  else if (dist2 < 1.5)
    {
      //--------------------
      // Even if no melee weapon is used by this droid, there might still
      // be some reason to move (regardless of waypoints):  This is the case
      // if the influencer/tux gets too close.  Then the droid should try to
      // take advantage of his greater range and back of somewhat, so that
      // the tux does not just need to kill the sitting duck.  This shall be
      // accomplished here.
      //
      
      //--------------------
      // If the distance is not yet right, we find a new location to move to.  We
      // do this WITHOUT consulting the waypoints, so that the robots become more
      // 'intelligent' in their movement.
      //
      // ThisRobot->TextVisibleTime = 0 ;
      // ThisRobot->TextToBeDisplayed = "Seeking to farther away from target...";
      //
      ThisRobot -> persuing_given_course = TRUE;
      ThisRobot -> PrivatePathway [ 0 ] . x = ThisRobot -> pos.x ;
      ThisRobot -> PrivatePathway [ 0 ] . y = ThisRobot -> pos.y ;
      
      //--------------------
      // Now we check if it's perhaps time to make a step to the left/right in 
      // order to get close enough to the target tux for a successful melee weapon
      // swing.
      //
      TargetRange = 1.5;
      StepSize = -1.0;
      if ( fabsf ( Me [ TargetPlayer ] . pos . x - ThisRobot -> pos . x ) < TargetRange )
	{
	  if ( ( Me [ TargetPlayer ] . pos . x - ThisRobot -> pos . x ) > 0 )
	    {
	      if ( ( DirectLineWalkable ( ThisRobot -> PrivatePathway [ 0 ] . x + StepSize , 
					  ThisRobot -> PrivatePathway [ 0 ] . y ,
					  ThisRobot -> PrivatePathway [ 0 ] . x , 
					  ThisRobot -> PrivatePathway [ 0 ] . y ,
					  ThisRobot -> pos.z ) == TRUE ) &&
		   ( CheckIfWayIsFreeOfDroids ( ThisRobot->pos.x , ThisRobot->pos.y , 
						ThisRobot->PrivatePathway[ 0 ].x + StepSize , ThisRobot->PrivatePathway[ 0 ].y,
						ThisRobot->pos.z , enemynum ) ) )
		{
		  ThisRobot->PrivatePathway[ 0 ].x = ThisRobot->pos.x + StepSize ;
		}
	    }
	  else
	    {
	      if ( ( DirectLineWalkable( ThisRobot->PrivatePathway[ 0 ].x - StepSize , 
					 ThisRobot->PrivatePathway[ 0 ].y ,
					 ThisRobot->PrivatePathway[ 0 ].x , 
					 ThisRobot->PrivatePathway[ 0 ].y ,
					 ThisRobot->pos.z ) == TRUE ) &&
		   ( CheckIfWayIsFreeOfDroids ( ThisRobot->pos.x , ThisRobot->pos.y , 
						ThisRobot->PrivatePathway [ 0 ] . x - StepSize , ThisRobot->PrivatePathway[ 0 ].y,
						ThisRobot->pos.z , enemynum ) ) )
		{
		  ThisRobot->PrivatePathway [ 0 ] . x = ThisRobot->pos.x - StepSize;
		}
	    }
	}
      
      //--------------------
      // Now we check if it's perhaps time to make a step up/down in order to
      // get close enough for a successful melee weapon swing at a tux.
      //
      if ( fabsf ( Me [ TargetPlayer ] . pos . y - ThisRobot -> pos . y ) < TargetRange )
	{
	  if ( ( Me [ TargetPlayer ] . pos . y - ThisRobot -> pos . y ) > 0 )
	    {
	      if ( ( DirectLineWalkable ( ThisRobot->PrivatePathway[ 0 ].x , 
					  ThisRobot->PrivatePathway[ 0 ].y + StepSize ,
					  ThisRobot->PrivatePathway[ 0 ].x , 
					  ThisRobot->PrivatePathway[ 0 ].y ,
					  ThisRobot->pos.z ) == TRUE ) &&
		   ( CheckIfWayIsFreeOfDroids ( ThisRobot->pos.x , ThisRobot->pos.y , 
						ThisRobot->PrivatePathway[ 0 ].x , ThisRobot->PrivatePathway[ 0 ].y + StepSize ,
						ThisRobot->pos.z , enemynum ) ) )
		ThisRobot->PrivatePathway[ 0 ].y = ThisRobot->pos.y + StepSize ;
	    }
	  else
	    {
	      if ( ( DirectLineWalkable ( ThisRobot->PrivatePathway[ 0 ].x , 
					  ThisRobot->PrivatePathway[ 0 ].y - StepSize ,
					  ThisRobot->PrivatePathway[ 0 ].x , 
					  ThisRobot->PrivatePathway[ 0 ].y ,
					  ThisRobot->pos.z ) == TRUE ) &&
		   ( CheckIfWayIsFreeOfDroids ( ThisRobot->pos.x , ThisRobot->pos.y , 
						ThisRobot->PrivatePathway[ 0 ].x , ThisRobot->PrivatePathway[ 0 ].y - StepSize ,
						ThisRobot->pos.z , enemynum ) ) )
		{
		  ThisRobot->PrivatePathway[ 0 ].y = ThisRobot->pos.y - StepSize;
		}
	    }
	} // the case, that with no melee weapon distance is very short
    } // else the case, that no melee weapon 

  //--------------------
  // Melee weapons have a certain limited range.  If such a weapon is used,
  // don't fire if the influencer is several squares away!
  //
  if ( ( ItemMap [ Druidmap [ ThisRobot->type ].weapon_item.type ] .item_gun_angle_change != 0 ) && 
       ( dist2 > 1.5 ) ) return;
  
  if ( ThisRobot->firewait ) return;
  
  if ( ( MyRandom ( AGGRESSIONMAX ) >= Druidmap[ThisRobot->type].aggression ) &&
       ( ThisRobot->is_friendly == FALSE ) )
    {
      ThisRobot->firewait += drand48()* ROBOT_MAX_WAIT_BETWEEN_SHOTS; //MyRandom (Druidmap[ThisRobot->type].firewait);
      return;
    }
  
  
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
	  DebugPrintf( 0 , "\nWARNING:  Enemy with negative energy encountered.  Phase correction forced..." );
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

/* ----------------------------------------------------------------------
 * This function returns the class a robot belongs to.
 * ---------------------------------------------------------------------- */
int
ClassOfDruid (int druidtype)
{
  /*
    This does not work with characters being used also as droid digits.

    char classnumber[2];

    classnumber[0] = Druidmap[druidtype].druidname[0];	// first digit is class 
    classnumber[1] = '\0';
  
    return (atoi (classnumber));
  */

  return ( Druidmap [ druidtype ] . class ) ;

}; // ClassOfDruid 

#undef _enemy_c
