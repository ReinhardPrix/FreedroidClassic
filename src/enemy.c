/*----------------------------------------------------------------------
 *
 * Desc: All enemy - realted functions.  This includes their whole behaviour,
 *	healing, initialization, shuffling them around after evevator-transitions
 *	of the paradroid, deleting them, collisions of enemys among themselves,
 *	their fireing, animation and such.
 *
 *----------------------------------------------------------------------*/

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
#define _enemy_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"



#define NOSTRAIGHTDIR 255

#define COL_SPEED		3	/* wegstossen bei enemy-enemy collision */

#define FIREDIST2	8 // according to the intro, the laser can be "focused on any target
                          // within a range of eight metres"

// void PermanentHealRobots (void);

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
PermanentHealRobots (void)
{
  int i;

  for (i = 0; i < MAX_ENEMYS_ON_SHIP; i++)
    {
      if (AllEnemys[i].Status == OUT)
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
      AllEnemys[i].levelnum = AllEnemys[i].energy = 0;
      AllEnemys[i].feindphase = 0;
      AllEnemys[i].nextwaypoint = AllEnemys[i].lastwaypoint = 0;
      AllEnemys[i].Status = OUT;
      AllEnemys[i].warten = 0;
      AllEnemys[i].firewait = 0;
      AllEnemys[i].energy = 0;
      AllEnemys[i].SpecialForce = 0;
      AllEnemys[i].AdvancedCommand = 0;
      AllEnemys[i].CompletelyFixed = 0;
      AllEnemys[i].Parameter1 = 0;
      AllEnemys[i].Parameter2 = 0;
      AllEnemys[i].Marker = 0;
      AllEnemys[i].Friendly = 0;
      AllEnemys[i].TextVisibleTime = 0;
      AllEnemys[i].TextToBeDisplayed = "";
    }

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
  // WARNING!! IT SHOULD BE NOTED THAT THIS FUNCTION REQUIRES THE
  // CURLEVEL STRUCTURE TO BE SET ALREADY, OR IT WILL SEGFAULT,
  // EVEN WHEN A RETURN IS PLACED AT THE START OF THE FUNCTION!!!!
  //
  int curlevel = CurLevel->levelnum;
  int i, j;
  int nth_enemy;
  int wp_num;
  int wp = 0;
  int BestWaypoint;
  finepoint influ_coord;

  /* Anzahl der Waypoints auf CurLevel abzaehlen */
  wp_num = 0;

  for ( i=0 ; i<MAXWAYPOINTS ; i++ )
    {
      if ( CurLevel->AllWaypoints[i].x != 0 ) wp_num ++;
    }

  nth_enemy = 0;
  for (i = 0; i < MAX_ENEMYS_ON_SHIP ; i++)
    {
      if (AllEnemys[i].Status == OUT
	  || AllEnemys[i].levelnum != curlevel)
	continue;		/* dont handle dead enemys or on other level */

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
	      if ( abs ( (CurLevel->AllWaypoints[j].x) - AllEnemys[i].pos.x ) < 
		   abs ( CurLevel->AllWaypoints[ BestWaypoint ].x - AllEnemys[i].pos.x ) )
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
	  DebugPrintf (1, "\nNumber of waypoints found: %d." , wp_num );
	  DebugPrintf (1, "\nLess waypoints than enemys on level %d? !", CurLevel->levelnum );
	  Terminate (ERR);
	}

      AllEnemys[i].pos.x = CurLevel->AllWaypoints[wp].x;
      AllEnemys[i].pos.y = CurLevel->AllWaypoints[wp].y;

      AllEnemys[i].lastwaypoint = wp;
      AllEnemys[i].nextwaypoint = wp;

    }/* for (MAX_ENEMYS_ON_SHIP) */

  /* enemys ein bisschen sich selbst ueberlassen */

  /* Influencer zuerst entfernen */
  influ_coord.x = Me.pos.x;
  influ_coord.y = Me.pos.y;
  Me.pos.x = Me.pos.y = 0;

  for (i = 0; i < 30; i++)     MoveEnemys ();

  /* influencer wieder her */
  Me.pos.x = influ_coord.x;
  Me.pos.y = influ_coord.y;

}	/* ShuffleEnemys() */

void 
MoveThisRobotClassical( int EnemyNum )
{
  int i,j;
  finepoint Restweg;
  Waypoint WpList;		/* Pointer to waypoint-liste */
  int nextwp;
  finepoint nextwp_pos;
  int trywp;
  float maxspeed;
  Enemy ThisRobot=&AllEnemys[ EnemyNum ];

  // We do some definitions to save us some more typing later...
  WpList = CurLevel->AllWaypoints;
  nextwp = ThisRobot->nextwaypoint;
  maxspeed = Druidmap[ ThisRobot->type ].maxspeed;
  nextwp_pos.x = WpList[nextwp].x;
  nextwp_pos.y = WpList[nextwp].y;


  // determine the remaining way until the target point is reached
  Restweg.x = nextwp_pos.x - ThisRobot->pos.x;
  Restweg.y = nextwp_pos.y - ThisRobot->pos.y;

  // --------------------
  // As long a the distance from the current position of the enemy
  // to its next wp is large, movement is rather sinple:

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
      /* but only if there are connections possible */
      for ( j=0; j<MAX_WP_CONNECTIONS; j++ )
	if ( WpList[nextwp].connections[j] != -1 )
	  break;
      if ( j < MAX_WP_CONNECTIONS )
	while ( (trywp = WpList[nextwp].
		 connections[MyRandom (MAX_WP_CONNECTIONS - 1)]) == -1);
      else
	{
	  DebugPrintf ( 2, "\nWeird waypoint %d has no connections!\n", nextwp);
	  Terminate(ERR);
	}
      
      /* setze neuen Waypoint */
      ThisRobot->nextwaypoint = trywp;
    }			/* if */

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
      for ( j = 0 ; j < MAX_ENEMYS_ON_SHIP ; j ++ )
	{
	  if ( AllEnemys[j].levelnum != OurLevel ) continue;
	  if ( AllEnemys[j].Status == OUT ) continue;
	  if ( AllEnemys[j].energy <= 0 ) continue;
	  if ( j == ExceptedDroid ) continue;

	  // so it seems that we need to test this one!!
	  if ( ( fabsf(AllEnemys[j].pos.x - CheckPosition.x ) < 2*DRUIDRADIUSX ) &&
	       ( fabsf(AllEnemys[j].pos.y - CheckPosition.y ) < 2*DRUIDRADIUSY ) ) 
	    {
	      DebugPrintf( 2, "\nCheckIfWayIsFreeOfDroids (...) : Connection analysis revealed : TRAFFIC-BLOCKED !");
	      return FALSE;
	    }
	}

      if ( ( fabsf( Me.pos.x - CheckPosition.x ) < 2*DRUIDRADIUSX ) &&
	   ( fabsf( Me.pos.y - CheckPosition.y ) < 2*DRUIDRADIUSY ) ) 
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
 * This function moves one robot in an advanced way, that hasn't been
 * present within the classical paradroid game.
 *
 ----------------------------------------------------------------------*/
void 
MoveThisRobotAdvanced ( int EnemyNum )
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

  // --------------------
  // As long a the distance from the current position of the enemy
  // to its next wp is large, movement is rather sinple:

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
      if ( IsVisible ( &(ThisRobot->pos) ) &&
	   Druidmap[ThisRobot->type].aggression &&
	   ! ThisRobot->Friendly )
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

  //--------------------
  // At first, we check for a lot of cases in which we do not
  // need to move anything for this reason or for that
  //

  // ignore robots on other levels 
  if ( ThisRobot->levelnum != CurLevel->levelnum) return;

  // ignore dead robots as well...
  if ( ThisRobot->Status == OUT ) return;

  // Now check if the robot is still alive
  // if the robot just got killed, initiate the
  // explosion and all that...
  if ( ThisRobot->energy <= 0)
    {
      ThisRobot->Status = OUT;
      RealScore += Druidmap[ ThisRobot->type ].score;
      StartBlast ( ThisRobot->pos.x, ThisRobot->pos.y,
 		  DRUIDBLAST);
      if (LevelEmpty ())
	CurLevel->empty = WAIT_LEVELEMPTY;
      return;	// this one's down, so we can move on to the next
    }
  
  // ignore all enemys with CompletelyFixed flag set...
  if ( ThisRobot->CompletelyFixed ) return;

  // robots that still have to wait also do not need to
  // be processed for movement
  if ( ThisRobot->warten > 0) return;

  // Now check for collisions of this enemy with his colleagues
  CheckEnemyEnemyCollision ( EnemyNum );

  //--------------------
  // Now comes the real movement part

  if ( Druidmap[ThisRobot->type].AdvancedBehaviour )
    MoveThisRobotAdvanced( EnemyNum );
  else
    MoveThisRobotClassical( EnemyNum );

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

  for (i = 0; i < MAX_ENEMYS_ON_SHIP ; i++)
     {

       MoveThisEnemy(i);

       // If its a combat droid, then if might attack...
       if (Druidmap[AllEnemys[i].type].aggression)
	 AttackInfluence (i);

    }	/* for (MAX_ENEMYS_ON_SHIP) */

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
  if ( ThisRobot->Status == OUT ) return;

  /* Ermittlung des Abstandsvektors zum Influencer */
  xdist = Me.pos.x - ThisRobot->pos.x;
  ydist = Me.pos.y - ThisRobot->pos.y;

  if (xdist == 0) xdist = 0.01;
  if (ydist == 0) ydist = 0.01;

  /* Sicherheit gegen Division durch 0 */
  //  if (fabsf (xdist) < 2)
  //    xdist = 2;
  //  if (abs (ydist) < 2)
  //    ydist = 2;

  /* wenn die Vorzeichen gut sind einen Schuss auf den 001 abgeben */
  guntype = Druidmap[ThisRobot->type].gun;

  dist2 = sqrt(xdist * xdist + ydist * ydist);

  //--------------------
  // If some special command was given, like 
  // ATTACK_FIXED_MAP_POSITION=1, then we do the following:
  //
  if ( ThisRobot->AdvancedCommand == 1 )
    {
      if (ThisRobot->firewait) return;  // can't fire:  gun not yet reloaded...


      xdist = ThisRobot->Parameter1 - ThisRobot->pos.x;
      ydist = ThisRobot->Parameter2 - ThisRobot->pos.y;

      Fire_Bullet_Sound ( guntype );

      // find a bullet entry, that isn't currently used... 
      for (j = 0; j < MAXBULLETS; j++)
	{
	  if (AllBullets[j].type == OUT)
	    break;
	}
      if (j == MAXBULLETS)
	{
	  DebugPrintf (2, "\nvoid AttackInfluencer(void):  Ran out of Bullets.... Terminating....");
	  Terminate (ERR);
	}

      /* Schussrichtung festlegen */
      if (fabsf (xdist) > fabsf (ydist))
	{
	  AllBullets[j].speed.x = Bulletmap[guntype].speed;
	  AllBullets[j].speed.y = ydist * AllBullets[j].speed.x / xdist;
	  if (xdist < 0)
	    {
	      AllBullets[j].speed.x = -AllBullets[j].speed.x;
	      AllBullets[j].speed.y = -AllBullets[j].speed.y;
	    }
	}

      if (fabsf (xdist) < fabsf (ydist))
	{
	  AllBullets[j].speed.y = Bulletmap[guntype].speed;
	  AllBullets[j].speed.x = xdist * AllBullets[j].speed.y / ydist;
	  if (ydist < 0)
	    {
	      AllBullets[j].speed.x = -AllBullets[j].speed.x;
	      AllBullets[j].speed.y = -AllBullets[j].speed.y;
	    }
	}

      AllBullets[j].angle = - ( 90 + 180 * atan2 ( AllBullets[j].speed.y ,  AllBullets[j].speed.x ) / M_PI ) ;  

      /* Bullets im Zentrum des Schuetzen starten */
      AllBullets[j].pos.x = ThisRobot->pos.x;
      AllBullets[j].pos.y = ThisRobot->pos.y;

      /* Bullets so abfeuern, dass sie nicht den Schuetzen treffen */
      AllBullets[j].pos.x +=
	(AllBullets[j].speed.x) / fabsf (Bulletmap[guntype].speed) * 0.5;
      AllBullets[j].pos.y +=
	(AllBullets[j].speed.y) / fabsf (Bulletmap[guntype].speed) * 0.5;

      // The following lines could be improved: Use not the sign, but only */
      // the fraction of the maxspeed times constant!
      // SINCE WE CAN ASSUME HIGH FRAMERATE DISABLE THIS CRAP! Within one */
      // frame, the robot cant move into its own bullet.
      // AllBullets[j].pos.x+=isignf(ThisRobot->speed.x)*Block_Width/2;      
      // AllBullets[j].pos.y+=isignf(ThisRobot->speed.y)*Block_Height/2;

      /* Dem Bullettype entsprechend lange warten vor naechstem Schuss */

      ThisRobot->firewait = Bulletmap[Druidmap[ThisRobot->type].gun].recharging_time ;

      /* Bullettype gemaes dem ueblichen guntype fuer den robottyp setzen */
      AllBullets[j].type = guntype;

      return;
    }

  //--------------------
  //
  // From here on, it's classical Paradroid robot behaviour concerning fireing....
  //

  /* Only fire, if the influencer is in range.... */
  if ((dist2 < FIREDIST2) &&
      (!ThisRobot->firewait) &&
      IsVisible (&ThisRobot->pos))
    {

      if ( MyRandom (AGGRESSIONMAX)  >=
 	  Druidmap[ThisRobot->type].aggression )
	{
	  ThisRobot->firewait += drand48()* ROBOT_MAX_WAIT_BETWEEN_SHOTS; //MyRandom (Druidmap[ThisRobot->type].firewait);
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
	  DebugPrintf (2, "\nvoid AttackInfluencer(void):  Ran out of Bullets.... Terminating....");
	  Terminate (ERR);
	}

      // determine the direction of the shot, so that it will go into the direction of
      // the influencer

      if (fabsf (xdist) > fabsf (ydist))
	{
	  AllBullets[j].speed.x = Bulletmap[guntype].speed;
	  AllBullets[j].speed.y = ydist * AllBullets[j].speed.x / xdist;
	  if (xdist < 0)
	    {
	      AllBullets[j].speed.x = -AllBullets[j].speed.x;
	      AllBullets[j].speed.y = -AllBullets[j].speed.y;
	    }
	}

      if (fabsf (xdist) < fabsf (ydist))
	{
	  AllBullets[j].speed.y = Bulletmap[guntype].speed;
	  AllBullets[j].speed.x = xdist * AllBullets[j].speed.y / ydist;
	  if (ydist < 0)
	    {
	      AllBullets[j].speed.x = -AllBullets[j].speed.x;
	      AllBullets[j].speed.y = -AllBullets[j].speed.y;
	    }
	}

      AllBullets[j].angle = - ( 90 + 180 * atan2 ( AllBullets[j].speed.y,  AllBullets[j].speed.x ) / M_PI );  

      /* Bullets im Zentrum des Schuetzen starten */
      AllBullets[j].pos.x = ThisRobot->pos.x;
      AllBullets[j].pos.y = ThisRobot->pos.y;

      /* Bullets so abfeuern, dass sie nicht den Schuetzen treffen */
      AllBullets[j].pos.x +=
	(AllBullets[j].speed.x) / fabsf (Bulletmap[guntype].speed) * 0.5;
      AllBullets[j].pos.y +=
	(AllBullets[j].speed.y) / fabsf (Bulletmap[guntype].speed) * 0.5;

      // The following lines could be improved: Use not the sign, but only */
      // the fraction of the maxspeed times constant!
      // SINCE WE CAN ASSUME HIGH FRAMERATE DISABLE THIS CRAP! Within one */
      // frame, the robot cant move into its own bullet.
      // AllBullets[j].pos.x+=isignf(ThisRobot->speed.x)*Block_Width/2;      
      // AllBullets[j].pos.y+=isignf(ThisRobot->speed.y)*Block_Height/2;

      /* Dem Bullettype entsprechend lange warten vor naechstem Schuss */

      ThisRobot->firewait = Bulletmap[Druidmap[ThisRobot->type].gun].recharging_time ;

      /* Bullettype gemaes dem ueblichen guntype fuer den robottyp setzen */
      AllBullets[j].type = guntype;

    }	/* if */

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
  float dist2;
  float speed_x, speed_y;

  check_x = AllEnemys[enemynum].pos.x;
  check_y = AllEnemys[enemynum].pos.y;

  for (i = 0; i < MAX_ENEMYS_ON_SHIP ; i++)
    {
      // check only collisions of LIVING enemys on this level
      if (AllEnemys[i].Status == OUT || AllEnemys[i].levelnum != curlev)
	continue;
      // dont check yourself...
      if (i == enemynum)
	continue;

      /* get distance between enemy i and enemynum */
      xdist = check_x - AllEnemys[i].pos.x;
      ydist = check_y - AllEnemys[i].pos.y;

      dist2 = sqrt(xdist * xdist + ydist * ydist);

      // Is there a Collision?
      if ( dist2 <= 2*DRUIDRADIUSXY )
	{

	  // am I waiting already?  If so, keep waiting... 
	  if (AllEnemys[enemynum].warten)
	    {
	      /* weiter warten */
	      AllEnemys[enemynum].warten = WAIT_COLLISION;
	      continue;
	    }

	  /* Sonst: Feind stoppen und selbst umdrehen */
	  AllEnemys[i].warten = WAIT_COLLISION;

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

  for (i = 0; i < MAX_ENEMYS_ON_SHIP ; i++)
    {
      if (AllEnemys[i].type == DRUID598)
	{
	  //   AllEnemys[i].feindrehcode,
	  //   Druidmap[AllEnemys[i].type].maxenergy,
	  //   AllEnemys[i].energy,
	  //  AllEnemys[i].feindphase);
	}

      /* ignore enemys that are dead or on other levels or dummys */
      if (AllEnemys[i].type == DEBUG_ENEMY)
	continue;
      if (AllEnemys[i].levelnum != CurLevel->levelnum)
	continue;
      if (AllEnemys[i].Status == OUT)
	continue;

      // AllEnemys[i].feindrehcode+=AllEnemys[i].energy;
      AllEnemys[i].feindphase +=
	(AllEnemys[i].energy / Druidmap[AllEnemys[i].type].maxenergy) *
	Frame_Time () * ENEMYPHASES * 2.5;

      if (AllEnemys[i].feindphase >= ENEMYPHASES)
	{
	  AllEnemys[i].feindphase = 0;
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
