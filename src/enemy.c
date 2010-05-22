/*
 *
 *   Copyright (c) 1994, 2002, 2003  Johannes Prix
 *   Copyright (c) 1994, 2002, 2003  Reinhard Prix
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

/*----------------------------------------------------------------------
 *
 * Desc: All enemy - realted functions.  This includes their whole behaviour,
 *	healing, initialization, shuffling them around after evevator-transitions
 *	of the paradroid, deleting them, collisions of enemys among themselves,
 *	their fireing, animation and such.
 *
 *----------------------------------------------------------------------*/

#define _enemy_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"

#define COL_SPEED		3	/* wegstossen bei enemy-enemy collision */

#define FIREDIST2	8 // according to the intro, the laser can be "focused on any target
                          // within a range of eight metres"


//----------------------------------------------------------------------
// for debug purposes: check if there are any droid "piles" on this level
//----------------------------------------------------------------------
void
CheckDroidDistribution (int level)
{
  int i;
  bool ok = TRUE;

  for (i=0; i< NumEnemys; i++)
    if ( (AllEnemys[i].levelnum == level) && CheckEnemyEnemyCollision (i) )
      {
	DebugPrintf (0, "We found a droid collision of droid Nr: %d\n", i);
	ok = FALSE;
      }

  if (ok)
    DebugPrintf (0, "Seems ok.\n");


  DebugPrintf (0, "\n NR.   ID  X    Y   ENERGY   speedX\n");
  for (i = 0; i < NumEnemys; i++)
    {
      if ( (AllEnemys[i].levelnum == CurLevel->levelnum)  && (AllEnemys[i].energy > 0) )
	DebugPrintf (0, "%d.   %s   %d   %d   %d    %g.\n", i,
		     Druidmap[AllEnemys[i].type].druidname,
		     (int)AllEnemys[i].pos.x,
		     (int)AllEnemys[i].pos.y,
		     (int)AllEnemys[i].energy,
		     AllEnemys[i].speed.x);
    } /* for (i<NumEnemys) */


  return;
}



/* ----------------------------------------------------------------------
 * This function tests, if a Robot can go a direct straigt line from
 * x1 y1 to x2 y2 without hitting a wall or another obstacle.
 *
 * The return value is TRUE or FALSE accoringly.
 * ----------------------------------------------------------------------*/
int
DirectLineWalkable( float x1 , float y1 , float x2 , float y2 )
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

      if ( IsPassable ( CheckPosition.x , CheckPosition.y , CENTER ) != CENTER )
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


/*@Function============================================================
@Desc:

@Ret:
@Int:
* $Function----------------------------------------------------------*/
void
PermanentHealRobots (void)
{
  int i;

  for (i = 0; i < NumEnemys; i++)
    {
      if (AllEnemys[i].status == OUT || AllEnemys[i].energy <= 0.0)
	continue;
      if (AllEnemys[i].energy < Druidmap[AllEnemys[i].type].maxenergy)
	AllEnemys[i].energy += Druidmap[AllEnemys[i].type].lose_health * Frame_Time();
    }
} // void PermanentHealRobots(void)


/*-----------------------------------------------------------------
 * @Desc: setzt AllEnemys - Array auf 0
 *
 *
 *-----------------------------------------------------------------*/
void
ClearEnemys (void)
{
  int i;

  for (i = 0; i < MAX_ENEMYS_ON_SHIP; i++)
    {
      AllEnemys[i].type = -1;
      AllEnemys[i].levelnum = -1;
      AllEnemys[i].phase = 0;
      AllEnemys[i].nextwaypoint = AllEnemys[i].lastwaypoint = 0;
      AllEnemys[i].status = OUT;
      AllEnemys[i].warten = 0;
      AllEnemys[i].firewait = 0;
      AllEnemys[i].energy = -1;
      AllEnemys[i].TextVisibleTime = 0;
      AllEnemys[i].TextToBeDisplayed = "";
    }

  NumEnemys = 0;

  return;

} /*  ClearEnemys() */


/*-----------------------------------------------------------------
 * @Desc: Vermischt enemys in CurLevel auf die Waypoints
 *
 *
 *-----------------------------------------------------------------*/
void
ShuffleEnemys (void)
{
  int curlevel = CurLevel->levelnum;
  int i;
  int nth_enemy;
  int wp, num_wp;
  bool used_wp[MAXWAYPOINTS];
  bool warned = FALSE;

  num_wp = CurLevel->num_waypoints;

  // keep a little list of which waypoints have already been used
  for ( i=0 ; i < num_wp; i++)
    used_wp[i] = FALSE;

  nth_enemy = 0;
  for (i = 0; i < NumEnemys ; i++)
    {
      if (AllEnemys[i].status == OUT || AllEnemys[i].levelnum != curlevel)
	continue;		/* dont handle dead enemys or on other level */

      nth_enemy++;
      if (nth_enemy > num_wp)
	{
	  if (!warned)
	    {
	      DebugPrintf (0, "\nWARNING: Less waypoints (%d) than enemys on level %d? !\n",
			   num_wp, CurLevel->levelnum );
	      DebugPrintf (0, "...cannot insert all droids on this level!\n");
	    }

	  warned = TRUE;
	  AllEnemys[i].status = OUT;
	  continue;
	}

      do { wp = MyRandom(num_wp-1);} while(used_wp[wp]);

      used_wp[wp] = TRUE;
      AllEnemys[i].pos.x = CurLevel->AllWaypoints[wp].x;
      AllEnemys[i].pos.y = CurLevel->AllWaypoints[wp].y;

      AllEnemys[i].lastwaypoint = wp;
      AllEnemys[i].nextwaypoint = wp;

    }/* for NumEnemys */

  return;

}	/* ShuffleEnemys() */

/* ----------------------------------------------------------------------
 * This function checks if the connection between two points is free of
 * droids.
 *
 * MAP TILES ARE NOT TAKEN INTO CONSIDERATION, ONLY DROIDS!!!
 *
 ----------------------------------------------------------------------*/
void
SelectNextWaypointClassical( int EnemyNum )
{
  int num_con;
  finepoint Restweg;
  Waypoint WpList;		/* Pointer to waypoint-liste */
  int nextwp;
  Enemy ThisRobot=&AllEnemys[EnemyNum];

  // We do some definitions to save us some more typing later...
  WpList = CurLevel->AllWaypoints;
  nextwp = ThisRobot->nextwaypoint;

  // determine the remaining way until the target point is reached
  Restweg.x = WpList[nextwp].x - ThisRobot->pos.x;
  Restweg.y = WpList[nextwp].y - ThisRobot->pos.y;

  //--------------------
  // Now we can see if we are perhaps already there?
  // then it might be time to set a new waypoint.
  //
  if ((Restweg.x == 0) && (Restweg.y == 0))
    {
      ThisRobot->lastwaypoint = ThisRobot->nextwaypoint;
      ThisRobot->warten = MyRandom (ENEMYMAXWAIT);

      /* suche moegliche Verbindung von hier */
      DebugPrintf (2, "/* suche moegliche Verbindung von hier */\n");

      num_con = WpList[nextwp].num_connections;
      if ( num_con > 0)
	ThisRobot->nextwaypoint = WpList[nextwp].connections[MyRandom(num_con-1)];
    }			/* if */

  return;

} // void MoveThisRobotClassical ( int Enemynum );

/* ----------------------------------------------------------------------
 * This function checks if the connection between two points is free of
 * droids.
 *
 * MAP TILES ARE NOT TAKEN INTO CONSIDERATION, ONLY DROIDS!!!
 *
 ----------------------------------------------------------------------*/
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

  for ( i = 0 ; i < Steps ; i++ )
    {
      for ( j = 0 ; j < NumEnemys ; j ++ )
	{
	  if ( AllEnemys[j].levelnum != OurLevel ) continue;
	  if ( AllEnemys[j].status == OUT ) continue;
	  if ( AllEnemys[j].energy <= 0 ) continue;
	  if ( j == ExceptedDroid ) continue;

	  // so it seems that we need to test this one!!
	  if ( ( fabsf(AllEnemys[j].pos.x - CheckPosition.x ) < 2*Droid_Radius ) &&
	       ( fabsf(AllEnemys[j].pos.y - CheckPosition.y ) < 2*Droid_Radius ) )
	    {
	      DebugPrintf( 2, "\nCheckIfWayIsFreeOfDroids (...) : Connection analysis revealed : TRAFFIC-BLOCKED !");
	      return FALSE;
	    }
	}

      if ( ( fabsf( Me.pos.x - CheckPosition.x ) < 2*Droid_Radius ) &&
	   ( fabsf( Me.pos.y - CheckPosition.y ) < 2*Droid_Radius ) )
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
 *
 ----------------------------------------------------------------------*/
void
MoveThisRobotThowardsHisWaypoint ( int EnemyNum )
{
  finepoint Restweg;
  Waypoint WpList;		/* Pointer to waypoint-liste */
  int nextwp;
  finepoint nextwp_pos;
  float maxspeed, steplen, dist;
  Enemy ThisRobot=&AllEnemys[ EnemyNum ];

  DebugPrintf( 2 , "\n void MoveThisRobotThowardsHisWaypoint ( int EnemyNum ) : real function call confirmed. ");

  // We do some definitions to save us some more typing later...
  WpList = CurLevel->AllWaypoints;
  nextwp = ThisRobot->nextwaypoint;
  maxspeed = Druidmap[ ThisRobot->type ].maxspeed;

  nextwp_pos.x = WpList[nextwp].x;
  nextwp_pos.y = WpList[nextwp].y;

  // determine the remaining way until the target point is reached
  Restweg.x = nextwp_pos.x - ThisRobot->pos.x;
  Restweg.y = nextwp_pos.y - ThisRobot->pos.y;

  steplen = Frame_Time() * maxspeed;
  // --------------------
  // As long a the distance from the current position of the enemy
  // to its next wp is large, movement is rather simple:

  dist = sqrt(Restweg.x*Restweg.x + Restweg.y*Restweg.y);
  if ( dist > steplen )
    {
      ThisRobot->speed.x = (Restweg.x/dist) * maxspeed;
      ThisRobot->speed.y = (Restweg.y/dist) * maxspeed;
      ThisRobot->pos.x += ThisRobot->speed.x * Frame_Time ();
      ThisRobot->pos.y += ThisRobot->speed.y * Frame_Time ();
    }
  else
    {
      // --------------------
      // If this enemy is just one step ahead of his target, we just put him there now
      ThisRobot->pos.x = nextwp_pos.x;
      ThisRobot->pos.y = nextwp_pos.y;
      ThisRobot->speed.x = 0;
      ThisRobot->speed.y = 0;
    }

  return;

}; // void MoveThisRobotThowardsHisWaypoint ( int EnemyNum )

/* ----------------------------------------------------------------------
 * This function moves one robot in an advanced way, that hasn't been
 * present within the classical paradroid game.
 *
 ----------------------------------------------------------------------*/
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

  DebugPrintf( 2 , "\n void MoveThisRobotAdvanced ( int EnemyNum ) : real function call confirmed. ");


  // We do some definitions to save us some more typing later...
  WpList = CurLevel->AllWaypoints;
  nextwp = ThisRobot->nextwaypoint;
  maxspeed = Druidmap[ ThisRobot->type ].maxspeed;
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

      // This statement should make hostile droids with aggresssion
      // wait, if they see the influencer and are at their waypoint now.
      // Then they (in some other function) open fire and should do
      // that, until the influencer vanishes out of sight, which should cause them
      // to go into a hunting mode. (to be implemented later).
      //
      if ( Druidmap[ThisRobot->type].aggression &&  IsVisible ( &(ThisRobot->pos)))
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
	      FreeWays[i] = CheckIfWayIsFreeOfDroids ( WpList[ThisRobot->lastwaypoint].x , WpList[ThisRobot->lastwaypoint].y , WpList[WpList[ThisRobot->lastwaypoint].connections[i]].x , WpList[WpList[ThisRobot->lastwaypoint].connections[i]].y , ThisRobot->levelnum , EnemyNum );
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

      /* setze neuen Waypoint */
      ThisRobot->nextwaypoint = trywp;
    }			/* if */
} // void MoveThisRobotAdvanced ( int EnemyNum )


void
MoveThisEnemy( int EnemyNum )
{
  Enemy ThisRobot=&AllEnemys[ EnemyNum ];

  // Now check if the robot is still alive
  // if the robot just got killed, initiate the
  // explosion and all that...
  if ( ThisRobot->energy <= 0 && (ThisRobot->status != TERMINATED) )
    {
      ThisRobot->status = TERMINATED;
      RealScore += Druidmap[ ThisRobot->type ].score;

      DeathCount += ThisRobot->type * ThisRobot->type;   // quadratic "importance", max=529

      StartBlast ( ThisRobot->pos.x, ThisRobot->pos.y, DRUIDBLAST);
      if (LevelEmpty ())
	{

	  RealScore += DECKCOMPLETEBONUS;

	  CurLevel->empty = TRUE;
	  CurLevel->timer = WAIT_LEVELEMPTY;
	}
      return;	// this one's down, so we can move on to the next
    }

  // robots that still have to wait also do not need to
  // be processed for movement
  if ( ThisRobot->warten > 0)
    return;

  // Now check for collisions of this enemy with his colleagues
  CheckEnemyEnemyCollision ( EnemyNum );

  //--------------------
  // Now comes the real movement part
  MoveThisRobotThowardsHisWaypoint( EnemyNum );

  SelectNextWaypointClassical( EnemyNum );

} // void MoveThisEnemy ( int EnemyNum )

/*@Function============================================================
@Desc: This is the function, that move each of the enemys according to
their orders and their program

@Ret: none
* $Function----------------------------------------------------------*/
void
MoveEnemys (void)
{
  int i;

  PermanentHealRobots ();  // enemy robots heal as time passes...

  AnimateEnemys ();	// move the "phase" of the rotation of enemys

  for (i = 0; i < NumEnemys; i++)
     {

       if ( (AllEnemys[i].status == OUT) || (AllEnemys[i].status == TERMINATED) ||
	    (AllEnemys[i].levelnum != CurLevel->levelnum) )
	 continue;

       MoveThisEnemy(i);

       // If its a combat droid, then if might attack...
       if (Druidmap[AllEnemys[i].type].aggression)
	 AttackInfluence (i);

     }	/* for NumEnemys */

} /* MoveEnemys() */

/*@Function============================================================
@Desc: AttackInfluence(): This function sometimes fires a bullet from
enemy number enemynum directly into the direction of the influencer,
but of course only if the odds are good i.e. requirements are met.

@Ret: void
* $Function----------------------------------------------------------*/
void
AttackInfluence (int enemynum)
{
  int j;
  Bullet CurBullet;
  int guntype;
  float xdist, ydist;
  float dist2;
  Enemy ThisRobot=&AllEnemys[ enemynum ];


  //--------------------
  // At first, we check for a lot of cases in which we do not
  // need to move anything for this reason or for that
  //

  // ignore robots on other levels
  if ( ThisRobot->levelnum != CurLevel->levelnum) return;

  // ignore dead robots as well...
  if ( ThisRobot->status == OUT ) return;

  xdist = Me.pos.x - ThisRobot->pos.x;
  ydist = Me.pos.y - ThisRobot->pos.y;

  // Add some security against division by zero
  if (xdist == 0) xdist = 0.01;
  if (ydist == 0) ydist = 0.01;

  // if odds are good, make a shot at your target
  guntype = Druidmap[ThisRobot->type].gun;

  dist2 = sqrt(xdist * xdist + ydist * ydist);

  //--------------------
  //
  // From here on, it's classical Paradroid robot behaviour concerning fireing....
  //

  if ( dist2 >= FIREDIST2 ) return; // distance limitation only for MS mechs

  if ( ThisRobot->firewait ) return;

  if ( !IsVisible (&ThisRobot->pos))
    return;

  if ( ( MyRandom (AGGRESSIONMAX) >= Druidmap[ThisRobot->type].aggression ))
    {
      ThisRobot->firewait += 1.0*MyRandom(1000)* ROBOT_MAX_WAIT_BETWEEN_SHOTS / 1000.0;
      return;
    }


  Fire_Bullet_Sound ( guntype );

  // find a bullet entry, that isn't currently used...
  for (j = 0; j < MAXBULLETS; j++)
    {
      if (AllBullets[j].type == OUT)
	break;
    }
  if (j == MAXBULLETS)
    {
      DebugPrintf (2, "\nvoid AttackInfluencer(void):  no free bullets... giving up\n");
      return ;
    }

  CurBullet = &AllBullets[j];
  // determine the direction of the shot, so that it will go into the direction of
  // the target

  if (fabsf (xdist) > fabsf (ydist))
    {
      CurBullet->speed.x = Bulletmap[guntype].speed;
      CurBullet->speed.y = ydist * CurBullet->speed.x / xdist;
      if (xdist < 0)
	{
	  CurBullet->speed.x = -CurBullet->speed.x;
	  CurBullet->speed.y = -CurBullet->speed.y;
	}
    }

  if (fabsf (xdist) < fabsf (ydist))
    {
      CurBullet->speed.y = Bulletmap[guntype].speed;
      CurBullet->speed.x = xdist * CurBullet->speed.y / ydist;
      if (ydist < 0)
	{
	  CurBullet->speed.x = -CurBullet->speed.x;
	  CurBullet->speed.y = -CurBullet->speed.y;
	}
    }

  CurBullet->angle = - ( 90 + 180 * atan2 ( CurBullet->speed.y,  CurBullet->speed.x ) / M_PI );

  /* Bullets im Zentrum des Schuetzen starten */
  CurBullet->pos.x = ThisRobot->pos.x;
  CurBullet->pos.y = ThisRobot->pos.y;

  /* Bullets so abfeuern, dass sie nicht den Schuetzen treffen */
  CurBullet->pos.x +=
    (CurBullet->speed.x) / fabsf (Bulletmap[guntype].speed) * 0.5;
  CurBullet->pos.y +=
    (CurBullet->speed.y) / fabsf (Bulletmap[guntype].speed) * 0.5;

  /* Dem Bullettype entsprechend lange warten vor naechstem Schuss */

  ThisRobot->firewait = Bulletmap[Druidmap[ThisRobot->type].gun].recharging_time ;

  /* Bullettype gemaess dem ueblichen guntype fuer den robottyp setzen */
  CurBullet->type = guntype;
  CurBullet->time_in_frames = 0;
  CurBullet->time_in_seconds = 0;

  //}	/* if */

}   /* AttackInfluence */

/*@Function============================================================
@Desc: CheckEnemyEnemyCollision()

@Ret: 	TRUE: if enemy Nr. Enemynum collided with another enemy
@Int:
* $Function----------------------------------------------------------*/
int
CheckEnemyEnemyCollision (int enemynum)
{
  int i;
  int curlev = CurLevel->levelnum;
  float check_x, check_y;
  int swap;
  float xdist, ydist;
  float dist;
  float speed_x, speed_y;

  check_x = AllEnemys[enemynum].pos.x;
  check_y = AllEnemys[enemynum].pos.y;

  for (i = 0; i < NumEnemys ; i++)
    {
      // check only collisions of LIVING enemys on this level
      if ( (AllEnemys[i].status == OUT) || (AllEnemys[i].status == TERMINATED)
	   || AllEnemys[i].levelnum != curlev)
	continue;
      // dont check yourself...
      if (i == enemynum)
	continue;

      /* get distance between enemy i and enemynum */
      xdist = check_x - AllEnemys[i].pos.x;
      ydist = check_y - AllEnemys[i].pos.y;

      dist = sqrt(xdist * xdist + ydist * ydist);

      // Is there a Collision?
      if ( dist <= 2*Droid_Radius)
	{

	  // am I waiting already?  If so, keep waiting...
	  if (AllEnemys[enemynum].warten)
	    {
	      /* weiter warten */
	      AllEnemys[enemynum].warten = MyRandom(2*WAIT_COLLISION);
	      continue;
	    }

	  /* Sonst: Feind stoppen und selbst umdrehen */
	  AllEnemys[i].warten = MyRandom(2*WAIT_COLLISION);

	  /* gestoppten gegner ein wenig zurueckstossen */
	  if (xdist)
	    AllEnemys[i].pos.x -= xdist / fabsf (xdist) * Frame_Time();
	  if (ydist)
	    AllEnemys[i].pos.y -= ydist / fabsf (ydist) * Frame_Time();

	  swap = AllEnemys[enemynum].nextwaypoint;
	  AllEnemys[enemynum].nextwaypoint =
	    AllEnemys[enemynum].lastwaypoint;
	  AllEnemys[enemynum].lastwaypoint = swap;

	  /* Etwas aus Gegner herausbewegen !! */
	  speed_x = AllEnemys[enemynum].speed.x;
	  speed_y = AllEnemys[enemynum].speed.y;

	  if (speed_x)
	    AllEnemys[enemynum].pos.x -=
	      Frame_Time() * COL_SPEED * (speed_x) / fabsf (speed_x);
	  if (speed_y)
	    AllEnemys[enemynum].pos.y -=
	      Frame_Time() * COL_SPEED * (speed_y) / fabsf (speed_y);

	  return TRUE;

	}			/* if dist zu klein */

    }				/* for */

  return FALSE;
} // int CheckEnemyEnemyCollision


/*@Function============================================================
@Desc:

@Ret:
@Int:
* $Function----------------------------------------------------------*/
void
AnimateEnemys (void)
{
  int i;

  for (i = 0; i < NumEnemys; i++)
    {
      //      if (AllEnemys[i].type == DRUID598)
      //	{
	  //   AllEnemys[i].feindrehcode,
	  //   Druidmap[AllEnemys[i].type].maxenergy,
	  //   AllEnemys[i].energy,
	  //  AllEnemys[i].phase);
      //}

      /* ignore enemys that are dead or on other levels or dummys */
      if (AllEnemys[i].levelnum != CurLevel->levelnum)
	continue;
      if (AllEnemys[i].status == OUT)
	continue;

      // AllEnemys[i].feindrehcode+=AllEnemys[i].energy;
      AllEnemys[i].phase +=
	(AllEnemys[i].energy / Druidmap[AllEnemys[i].type].maxenergy) *
	Frame_Time () * ENEMYPHASES * 2.5;

      if (AllEnemys[i].phase >= ENEMYPHASES)
	{
	  AllEnemys[i].phase = 0;
	}
    }
} // void AnimateEnemys(void)

/*@Function============================================================
@Desc: ClassOfDruid(druidtype): liefert die Classe des Druidtypes type

@Ret: int class: 0-9
@Int:
* $Function----------------------------------------------------------*/
int
ClassOfDruid (int druidtype)
{
  char classnumber[2];

  classnumber[0] = Druidmap[druidtype].druidname[0];	/* first digit is class */
  classnumber[1] = '\0';

  return (atoi (classnumber));
}				/* ClassOfDruid */

#undef _enemy_c
