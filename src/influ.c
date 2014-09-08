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
 * Desc: all features, movement, fireing, collision and extras of the
 *	influencer are done in here.
 *
 *----------------------------------------------------------------------*/

#define _influ_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"

#define TIMETILLNEXTBULLET 14

#define REFRESH_ENERGY		3
// NORMALISATION #define COLLISION_PUSHSPEED	70
#define COLLISION_PUSHSPEED	2.0

#define BOUNCE_LOSE_ENERGY 3	/* amount of lose-energy at enemy-collisions */
#define BOUNCE_LOSE_FACT 1
void InfluEnemyCollisionLoseEnergy (int enemynum);	/* influ can lose energy on coll. */
void PermanentLoseEnergy (void);	/* influ permanently loses energy */
int NoInfluBulletOnWay (void);

#define max(x,y) ((x) < (y) ? (y) : (x) )
#define MAXIMAL_STEP_SIZE ( 7.0/20 )

int CurrentZeroRingIndex=0;


void
InitInfluPositionHistory( void )
{
  int RingPosition;

  for ( RingPosition = 0 ; RingPosition < MAX_INFLU_POSITION_HISTORY ; RingPosition ++ )
    {
      Me.Position_History_Ring_Buffer[ RingPosition ].x = Me.pos.x ;
      Me.Position_History_Ring_Buffer[ RingPosition ].y = Me.pos.y ;
      Me.Position_History_Ring_Buffer[ RingPosition ].z = CurLevel->levelnum ;
    }
} // void InitInfluPositionHistory( void )

float
GetInfluPositionHistoryX( int HowLongPast )
{
  int RingPosition;

  RingPosition = CurrentZeroRingIndex - HowLongPast;

  RingPosition += MAX_INFLU_POSITION_HISTORY; // We don't want any negative values, for safety

  RingPosition %= MAX_INFLU_POSITION_HISTORY; // We do MODULO for the Ring buffer length

  return Me.Position_History_Ring_Buffer[ RingPosition ].x;
}

float
GetInfluPositionHistoryY( int HowLongPast )
{
  int RingPosition;

  RingPosition = CurrentZeroRingIndex - HowLongPast;

  RingPosition += MAX_INFLU_POSITION_HISTORY; // We don't want any negative values, for safety

  RingPosition %= MAX_INFLU_POSITION_HISTORY; // We do MODULO for the Ring buffer length

  return Me.Position_History_Ring_Buffer[ RingPosition ].y;
}

float
GetInfluPositionHistoryZ( int HowLongPast )
{
  int RingPosition;

  RingPosition = CurrentZeroRingIndex - HowLongPast;

  RingPosition += MAX_INFLU_POSITION_HISTORY; // We don't want any negative values, for safety

  RingPosition %= MAX_INFLU_POSITION_HISTORY; // We do MODULO for the Ring buffer length

  return Me.Position_History_Ring_Buffer[ RingPosition ].z;
}



/*@Function============================================================
@Desc: This function moves the influencer, adjusts his speed according to
keys pressed and also adjusts his status and current "phase" of his rotation.

@Ret: none
* $Function----------------------------------------------------------*/
void
MoveInfluence (void)
{
  float accel = Druidmap[Me.type].accel;
  float planned_step_x;
  float planned_step_y;
  static float TransferCounter = 0;

  accel *= Frame_Time();

  DebugPrintf (2, "\nvoid MoveInfluence(void):  Real function call confirmed.");

  //--------------------
  // We store the influencers position for the history record and so that others
  // can follow his trail.
  //

  CurrentZeroRingIndex++;
  CurrentZeroRingIndex %= MAX_INFLU_POSITION_HISTORY;
  Me.Position_History_Ring_Buffer [CurrentZeroRingIndex].x = Me.pos.x;
  Me.Position_History_Ring_Buffer [CurrentZeroRingIndex].y = Me.pos.y;
  Me.Position_History_Ring_Buffer [CurrentZeroRingIndex].z = CurLevel->levelnum ;

  PermanentLoseEnergy ();	/* influ permanently loses energy */

  // check, if the influencer is still ok
  if (Me.energy <= 0)
    {
      if (Me.type != DRUID001)
	{
	  Me.type = DRUID001;
	  Me.energy = BLINKENERGY;
	  Me.health = BLINKENERGY;
	  StartBlast (Me.pos.x, Me.pos.y, REJECTBLAST);
	}
      else
	{
	  Me.status = TERMINATED;
	  ThouArtDefeated ();
	  DebugPrintf (2, "\nvoid MoveInfluence(void):  Alternate end of function reached.");
	  return;
	}
    }

  /* Time passed before entering Transfermode ?? */
  if ( TransferCounter >= WAIT_TRANSFERMODE )
    {
      Me.status = TRANSFERMODE;
      TransferCounter=0;
    }

  if (UpPressed())
    Me.speed.y -= accel;
  if (DownPressed ())
    Me.speed.y += accel;
  if (LeftPressed())
    Me.speed.x -= accel;
  if (RightPressed())
    Me.speed.x += accel;

//  We only need this check if we want held fire to cause activate 
  if (!AnyCmdActive()) // Used to be !SpacePressed, which causes any fire button != SPACE behave differently than space
     Me.status = MOBILE;

  if (TransferCounter == 1)
    {
      Me.status = TRANSFERMODE;
      TransferCounter = 0;
    }

  if (cmd_is_active(CMD_ACTIVATE)) // activate mode for Konsole and Lifts
    Me.status = ACTIVATE;

   if ( GameConfig.FireHoldTakeover && (FirePressed ()) && (NoDirectionPressed ()) &&(Me.status != WEAPON) && (Me.status != TRANSFERMODE) ) // Proposed FireActivatePressed here...
   TransferCounter += Frame_Time(); // Or make it an option!

  if ( (FirePressed() ) && (!NoDirectionPressed () ) && (Me.status != TRANSFERMODE) )
    Me.status = WEAPON;

  if (FirePressed() && (!NoDirectionPressed())&&(Me.status==WEAPON)&&(Me.firewait == 0) )
    FireBullet ();

  if ( (Me.status != WEAPON) && (cmd_is_active(CMD_TAKEOVER)) )
    Me.status = TRANSFERMODE;


  InfluenceFrictionWithAir (); // The influ should lose some of his speed when no key is pressed

  AdjustSpeed ();  // If the influ is faster than allowed for his type, slow him

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
  planned_step_x = Me.speed.x * Frame_Time ();
  planned_step_y = Me.speed.y * Frame_Time ();
  if ( fabsf(planned_step_x) >= MAXIMAL_STEP_SIZE )
    {
      planned_step_x = copysignf( MAXIMAL_STEP_SIZE , planned_step_x );
    }
  if ( fabsf(planned_step_y) >= MAXIMAL_STEP_SIZE )
    {
      planned_step_y = copysignf( MAXIMAL_STEP_SIZE , planned_step_y );
    }
  Me.pos.x += planned_step_x;
  Me.pos.y += planned_step_y;

  //--------------------
  // Check it the influ is on a special field like a lift, a console or a refresh
  ActSpecialField ( Me.pos.x , Me.pos.y );

  AnimateInfluence ();	// move the "phase" of influencers rotation

  DebugPrintf (2, "\nvoid MoveInfluence(void):  Usual end of function reached.");

} /* MoveInfluence */


/*@Function============================================================
@Desc:

@Ret:
@Int:
* $Function----------------------------------------------------------*/
int
NoInfluBulletOnWay (void)
{
  int i;

  for (i = 0; i < MAXBULLETS; i++)
    {
      if ((AllBullets[i].type != OUT) && (AllBullets[i].mine))
	return FALSE;
    }

  return TRUE;
} // NoInfluBulletOnWay

/*@Function============================================================
@Desc: AnimateInfluence: zaehlt die Phasen weiter, falls der Roboter
							mehrphasig ist
@Ret:
@Int:
* $Function----------------------------------------------------------*/
void
AnimateInfluence (void)
{

  /*
   * Phase des Influencers in fein gestuften Schritten weiterz"ahlen
   */

  if (Me.type != DRUID001)
    {
      Me.phase +=
	(Me.energy / (Druidmap[Me.type].maxenergy + Druidmap[DRUID001].maxenergy)) * Frame_Time () *
	ENEMYPHASES * 3;
    }
  else
    {
      Me.phase +=
	(Me.energy / (Druidmap[DRUID001].maxenergy)) * Frame_Time () *
	ENEMYPHASES * 3;
    }

  if (((int) rintf (Me.phase)) >= ENEMYPHASES)
    {
      Me.phase = 0;
    }


}				// void AnimateInfluence(void)


/*@Function============================================================
@Desc: This function checks for collisions of the influencer with walls,
doors, consoles, boxes and all other map elements.
In case of a collision, the position and speed of the influencer are
adapted accordingly.
NOTE: Of course this functions HAS to take into account the current framerate!

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void
CheckInfluenceWallCollisions (void)
{
  int sign;
  float SX = Me.speed.x * Frame_Time ();
  float SY = Me.speed.y * Frame_Time ();
  finepoint lastpos;
  int res;
  int NumberOfShifts=0;
  int safty_sx = 0, safty_sy = 0;	/* wegstoss - Geschwindigkeiten (falls noetig) */
  int NorthSouthAxisBlocked=FALSE;
  int EastWestAxisBlocked=FALSE;
  int H_Door_Sliding_Active = FALSE;


  int crashx = FALSE, crashy = FALSE;	/* Merker wo kollidiert wurde */

  lastpos.x = Me.pos.x - SX;
  lastpos.y = Me.pos.y - SY;

  // lastpos.x = Me.Position_History[0].x;
  // lastpos.y = Me.Position_History[0].y;

  res = DruidPassable (Me.pos.x, Me.pos.y);

#define NEW_BOUNCE_CHECK
#ifdef NEW_BOUNCE_CHECK

  // Influence-Wall-Collision only has to be checked in case of
  // a collision of course, which is indicated by res not CENTER.
  if (res != CENTER )
    {

      //--------------------
      // At first we just check in which directions (from the last position)
      // the ways are blocked and in which directions the ways are open.
      //
      if ( ! ( ( DruidPassable(lastpos.x , lastpos.y + Druidmap[Me.type].maxspeed * Frame_Time() ) != CENTER ) ||
	       ( DruidPassable(lastpos.x , lastpos.y - Druidmap[Me.type].maxspeed * Frame_Time() ) != CENTER ) ) )
	{
	  DebugPrintf(1, "\nNorth-south-Axis seems to be free.");
	  NorthSouthAxisBlocked = FALSE;
	}
      else
	{
	  NorthSouthAxisBlocked = TRUE;
	}

      if ( ( DruidPassable(lastpos.x + Druidmap[Me.type].maxspeed * Frame_Time() , lastpos.y ) == CENTER ) &&
	   ( DruidPassable(lastpos.x - Druidmap[Me.type].maxspeed * Frame_Time() , lastpos.y ) == CENTER ) )
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
	  Me.pos.y = lastpos.y;
	  Me.speed.y = 0;

	  // if its an open door, we also correct the east-west position, in the
	  // sense that we move thowards the middle
	  if ( (GetMapBrick(CurLevel, Me.pos.x , Me.pos.y - 0.5 ) == H_GANZTUERE ) ||
	       (GetMapBrick(CurLevel, Me.pos.x , Me.pos.y + 0.5 ) == H_GANZTUERE ) )
	    {
	      Me.pos.x += copysignf ( PUSHSPEED * Frame_Time() , ( rintf(Me.pos.x) - Me.pos.x ));
	      H_Door_Sliding_Active = TRUE;
	    }
	}

      if ( EastWestAxisBlocked )
	{
	  // EastWestCorrectionDone=TRUE;
	  if ( !H_Door_Sliding_Active ) Me.pos.x = lastpos.x;
	  Me.speed.x = 0;

	  // if its an open door, we also correct the north-south position, in the
	  // sense that we move thowards the middle
	  if ( (GetMapBrick(CurLevel, Me.pos.x +0.5 , Me.pos.y) == V_GANZTUERE ) ||
	       (GetMapBrick(CurLevel, Me.pos.x -0.5 , Me.pos.y) == V_GANZTUERE ) )
	    Me.pos.y += copysignf (PUSHSPEED * Frame_Time() , ( rintf(Me.pos.y) - Me.pos.y ));
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
	  if ( DruidPassable( Me.pos.x + SX , Me.pos.y ) == CENTER ) Me.pos.x += SX;
	  if ( DruidPassable( Me.pos.x , Me.pos.y +SY ) == CENTER ) Me.pos.y += SY;
	}

      // Here I introduce some extra security as a fallback:  Obviously
      // if the influencer is blocked FOR THE SECOND TIME, then the throw-back-algorithm
      // above HAS FAILED.  The absolutely fool-proof and secure handling is now done by
      // simply reverting to the last influ coordinated, where influ was NOT BLOCKED.
      // For this reason, a history of influ-coordinates has been introduced.  This will all
      // be done here and now:

      if ( (DruidPassable (Me.pos.x, Me.pos.y) != CENTER) &&
	   (DruidPassable ( GetInfluPositionHistoryX( 0 ) , GetInfluPositionHistoryY( 0 ) ) != CENTER) &&
	   (DruidPassable ( GetInfluPositionHistoryX( 1 ) , GetInfluPositionHistoryY( 1 ) ) != CENTER) )
	{
	  Me.pos.x = GetInfluPositionHistoryX( 2 );
	  Me.pos.y = GetInfluPositionHistoryY( 2 );
	  DebugPrintf(1, "\nATTENTION! CheckInfluenceWallCollsision FALLBACK ACTIVATED!!");
	}

    }

  return;

#endif

  switch (res)
    {
      // In this case, the influencer is (completely?) blocked.
    case -1:
      // --------------------
      // We handle here the case, that the influencer is completely blocked.
      // WHAT DO WE DO?  ---  The new algorithm proceeds as follows:
      // 1. Check if the north-south axis would be free FROM THE PREVIOUS POSITION
      // 2. Check if the east-west axis would be free FROM THE PREVIOUS POSITION
      //    and both of the above under the assumption of full speed.
      // 3. If the north south axis is free, it must have been the east-west movement
      //    otherwise it must have been the north-west movement, which caused the
      //    collision.
      // 4. Therefore restore the last position and move from there, but only in
      //    the free direction and not in the other.
      //

      /*
      if ( ( DruidPassable(lastpos.x , lastpos.y + Druidmap[Me.type].maxspeed * Frame_Time() ) == CENTER ) &&
	   ( DruidPassable(lastpos.x , lastpos.y - Druidmap[Me.type].maxspeed * Frame_Time() ) == CENTER ) )
	{
	  printf("\nNorth-south-Axis seems to be free.");
	}
      else
	{
	  printf("\nNorth-south-Axis seems NOT to be free.");
	  printf("\nCorrection movement and position in this direction...");
	  Me.pos.y = lastpos.y;
	  Me.speed.y = 0;
	  // return;
	}

      if ( ( DruidPassable(lastpos.x + Druidmap[Me.type].maxspeed * Frame_Time() , lastpos.y ) == CENTER ) &&
	   ( DruidPassable(lastpos.x - Druidmap[Me.type].maxspeed * Frame_Time() , lastpos.y ) == CENTER ) )
	{
	  printf("\nEast-west-Axis seems to be free.");
	}
      else
	{
	  printf("\nEast-west-Axis seems NOT to be free.");
	  printf("\nCorrection movement and position in this direction...");
	  Me.pos.x = lastpos.x;
	  Me.speed.x = 0;
	  //return;
	}

      return;

      */

      /* Festellen, in welcher Richtung die Mauer lag,
         und den Influencer entsprechend stoppen */
      if ( rintf(SX) && (DruidPassable (lastpos.x + SX, lastpos.y) != CENTER))
	{
	  crashx = TRUE;	/* In X wurde gecrasht */
	  sign = (SX < 0) ? -1 : 1;
	  SX = abs (SX);
	  NumberOfShifts=0;
	  while (--SX
		 && (DruidPassable (lastpos.x + sign * SX, lastpos.y) !=
		     CENTER) && (NumberOfShifts++ < 4));
	  Me.pos.x = lastpos.x + SX * sign;
	  Me.speed.x = 0;

	  /* falls Influencer weggestossen werden muss ! */
	  safty_sx = (-1) * sign * PUSHSPEED;
	}

      if (rintf(SY) && (DruidPassable (lastpos.x, lastpos.y + SY) != CENTER))
	{
	  crashy = TRUE;	/* in Y wurde gecrasht */
	  sign = (SY < 0) ? -1 : 1;
	  SY = abs (SY);
	  NumberOfShifts=0;
	  while (--SY
		 && (DruidPassable (lastpos.x, lastpos.y + sign * SY) !=
		     CENTER) && (NumberOfShifts++ < 4));
	  Me.pos.y = lastpos.y + SY * sign;
	  Me.speed.y = 0;

	  /* Falls Influencer weggestossen werden muss */
	  safty_sy = (-1) * sign * PUSHSPEED;
	}

      /* Hat das nichts geholfen, noch etwas wegschubsen */
      if (DruidPassable (Me.pos.x, Me.pos.y) != CENTER)
	{
	  if (crashx)
	    {
	      Me.speed.x = safty_sx;
	      Me.pos.x += Me.speed.x * Frame_Time() ;
	    }

	  if (crashy)
	    {
	      Me.speed.y = safty_sy;
	      Me.pos.y += Me.speed.y * Frame_Time() ;
	    }
	}

      break;

      /* Von Tuerrand wegschubsen */
    case OBEN:
      Me.speed.y = -PUSHSPEED;
      Me.pos.y += Me.speed.y; // * Frame_Time();
      break;

    case UNTEN:
      Me.speed.y = PUSHSPEED;
      Me.pos.y += Me.speed.y; // * Frame_Time() ;
      break;

    case RECHTS:
      Me.speed.x = PUSHSPEED;
      Me.pos.x += Me.speed.x; // * Frame_Time() ;
      break;

    case LINKS:
      Me.speed.x = -PUSHSPEED;
      Me.pos.x += Me.speed.x; // * Frame_Time() ;
      break;

      /* Not blocked at all ! */
    case CENTER:
      break;

    default:
      DebugPrintf (2, "Illegal return value from DruidPassable() ");
      Terminate (-1);
      break;

    } /* switch */

  // This old bouncing code is no longer working in all cases due to bigger numbers
  // and frame_rate dependence.  I therefore introduce some extra security:  Obviously
  // if the influencer is blocked FOR THE SECOND TIME, then the throw-back-algorithm
  // above HAS FAILED.  The absolutely fool-proof and secure handling is now done by
  // simply reverting to the last influ coordinated, where influ was NOT BLOCKED.
  // For this reason, a history of influ-coordinates has been introduced.  This will all
  // be done here and now:

  if ( (DruidPassable (Me.pos.x, Me.pos.y) != CENTER) &&
       (DruidPassable ( GetInfluPositionHistoryX ( 0 ) , GetInfluPositionHistoryY ( 0 ) != CENTER ) ) &&
       (DruidPassable ( GetInfluPositionHistoryX ( 1 ) , GetInfluPositionHistoryY ( 1 ) != CENTER ) ) )
    {
      Me.pos.x = GetInfluPositionHistoryX ( 2 );
      Me.pos.y = GetInfluPositionHistoryY ( 2 );
    }

} /* CheckInfluenceWallCollisions */

/*@Function============================================================
@Desc: Dies Prozedur passt die momentane Geschwindigkeit an die Hoechst-
	geschwindigkeit an.
@Ret: keiner
@Int: keiner
* $Function----------------------------------------------------------*/
void
AdjustSpeed (void)
{
  float maxspeed = Druidmap[Me.type].maxspeed;
  if (Me.speed.x > maxspeed)
    Me.speed.x = maxspeed;
  if (Me.speed.x < (-maxspeed))
    Me.speed.x = (-maxspeed);

  if (Me.speed.y > maxspeed)
    Me.speed.y = maxspeed;
  if (Me.speed.y < (-maxspeed))
    Me.speed.y = (-maxspeed);
}				// void AdjustSpeed(void)


/*@Function============================================================
@Desc: Diese Funktion reduziert die Fahrt des Influencers sobald keine
	Taste Richtungstaste mehr gedrueckt ist

@Ret: keiner

* $Function----------------------------------------------------------*/
void
InfluenceFrictionWithAir (void)
{
  float deccel = 7.0;
  float slowdown;
  int oldsign;

  if (!RightPressed () && !LeftPressed ())
    {
      oldsign = sign(Me.speed.x);
      slowdown = 1.0 * oldsign * deccel * Frame_Time();
      Me.speed.x -= slowdown;
      if (sign(Me.speed.x) != oldsign)  // changed direction -> vel=0
	Me.speed.x = 0.0;
    }

  if (!UpPressed() && !DownPressed())
    {
      oldsign = sign(Me.speed.y);
      slowdown = 1.0 * oldsign * deccel * Frame_Time();
      Me.speed.y -= slowdown;
      if (sign(Me.speed.y) != oldsign)  // changed direction -> vel=0
	Me.speed.y = 0.0;
    }

  return;

} // InfluenceFrictionWithAir (void)

/*@Function============================================================
@Desc: ExplodeInfluencer(): generiert eine grosse Explosion an
				der Position des Influencers

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void
ExplodeInfluencer (void)
{
  int i;
  int counter;

  Me.status = TERMINATED;

  DebugPrintf (2, "\nvoid ExplodeInfluencer(void): Real function call confirmed.");

  /* ein paar versetze Explosionen */
  for (i = 0; i < 10; i++)
    {
      /* freien Blast finden */
      counter = 0;
      while (AllBlasts[counter++].type != OUT);
      counter -= 1;
      if (counter >= MAXBLASTS)
	{
	  DebugPrintf(1, "\n\nWent out of blasts in ExplodeInfluencer...\n\n");
	  Terminate(ERR);
	}
      AllBlasts[counter].type = DRUIDBLAST;
      AllBlasts[counter].PX = Me.pos.x - Droid_Radius / 2 + MyRandom (10)*0.05;
      AllBlasts[counter].PY = Me.pos.y - Droid_Radius / 2 + MyRandom (10)*0.05;
      AllBlasts[counter].phase = 0.2*i;
    }

  Play_Sound (INFLUEXPLOSION_SOUND);

  return;

  DebugPrintf (2, "\nvoid ExplodeInfluencer(void): Usual end of function reached.");
}				/* ExplodeInfluencer */

/*@Function============================================================
@Desc:

@Ret:
@Int:
* $Function----------------------------------------------------------*/
void
CheckInfluenceEnemyCollision (void)
{
  int i;
  float xdist;
  float ydist;
  float dist2;
  float max_step_size;
  int swap;

  // return;

  for (i = 0; i < NumEnemys ; i++)
    {
      /* ignore enemy that are not on this level or dead */
      if (AllEnemys[i].levelnum != CurLevel->levelnum)
	continue;
      if (AllEnemys[i].status == OUT || AllEnemys[i].status == TERMINATED)
	continue;

      xdist = Me.pos.x - AllEnemys[i].pos.x;
      ydist = Me.pos.y - AllEnemys[i].pos.y;

      if (abs (xdist) > 1)
	continue;
      if (abs (ydist) > 1)
	continue;

      dist2 = sqrt( (xdist * xdist) + (ydist * ydist) );
      if ( dist2 > 2 * Droid_Radius )
	continue;


      if (Me.status != TRANSFERMODE)
	{

	  Me.speed.x = -Me.speed.x;
	  Me.speed.y = -Me.speed.y;

	  if (Me.speed.x != 0)
	    Me.speed.x +=
	      COLLISION_PUSHSPEED * (Me.speed.x / fabsf (Me.speed.x));
	  else if (xdist)
	    Me.speed.x = COLLISION_PUSHSPEED * (xdist / fabsf (xdist));
	  if (Me.speed.y != 0)
	    Me.speed.y +=
	      COLLISION_PUSHSPEED * (Me.speed.y / fabsf (Me.speed.y));
	  else if (ydist)
	    Me.speed.y = COLLISION_PUSHSPEED * (ydist / fabsf (ydist));

	  // move the influencer a little bit out of the enemy AND the enemy a little bit out of the influ
	  max_step_size = ((Frame_Time()) < ( MAXIMAL_STEP_SIZE ) ? (Frame_Time()) : ( MAXIMAL_STEP_SIZE )) ;
	  Me.pos.x += copysignf( max_step_size , Me.pos.x - AllEnemys[i].pos.x ) ;
	  Me.pos.y += copysignf( max_step_size , Me.pos.y - AllEnemys[i].pos.y ) ;
	  AllEnemys[i].pos.x -= copysignf( Frame_Time() , Me.pos.x - AllEnemys[i].pos.x ) ;
	  AllEnemys[i].pos.y -= copysignf( Frame_Time() , Me.pos.y - AllEnemys[i].pos.y ) ;
	  // Me.pos.x += Me.speed.x * Frame_Time ();
	  // Me.pos.y += Me.speed.y * Frame_Time ();

	  // there might be walls close too, so lets check again for collisions with them
	  CheckInfluenceWallCollisions ();

	  BounceSound ();

	  // shortly stop this enemy, then send him back to previous waypoint
	  if (!AllEnemys[i].warten)
	    {
	      AllEnemys[i].warten = WAIT_COLLISION;
	      swap = AllEnemys[i].nextwaypoint;
	      AllEnemys[i].nextwaypoint = AllEnemys[i].lastwaypoint;
	      AllEnemys[i].lastwaypoint = swap;

	      // Add some funny text!
	      EnemyInfluCollisionText ( i );

	    }
	  InfluEnemyCollisionLoseEnergy (i);	/* someone loses energy ! */

	}
      else
	{
	  Takeover (i);

	  if (LevelEmpty ())
	    CurLevel->empty = WAIT_LEVELEMPTY;

	}			/* if !Transfer else .. */

    }				/* for */

} // CheckInfluenceEnemyCollision

/*@Function============================================================
@Desc: Fire-Routine for the Influencer only !! (should be changed)

@Ret:
@Int:
* $Function----------------------------------------------------------*/
void
FireBullet (void)
{
  int i = 0;
  Bullet CurBullet = NULL;	/* das Bullet, um das es jetzt geht */
  int guntype = Druidmap[Me.type].gun;	/* which gun do we have ? */
  float BulletSpeed = Bulletmap[guntype].speed;
  float speed_norm;
  finepoint speed;
  int max_val;

  /* Wenn noch kein Schuss loesbar ist sofort zurueck */
  if (Me.firewait > 0)
    return;
  Me.firewait = Bulletmap[guntype].recharging_time;

  /* Geraeusch eines geloesten Schusses fabrizieren */
  Fire_Bullet_Sound ( guntype );

  /* Naechste Freie Bulletposition suchen */
  for (i = 0; i < (MAXBULLETS); i++)
    {
      if (AllBullets[i].type == OUT)
	{
	  CurBullet = &AllBullets[i];
	  break;
	}
    }

  /* Kein freies Bullet gefunden: Nimm das erste */
  if (CurBullet == NULL)
    CurBullet = &AllBullets[0];

  CurBullet->pos.x = Me.pos.x;
  CurBullet->pos.y = Me.pos.y;
  CurBullet->type = guntype;
  CurBullet->mine = TRUE;
  CurBullet->owner = -1;

  speed.x = 0.0;
  speed.y = 0.0;

  if (DownPressed ())
    speed.y = 1.0;
  if (UpPressed ())
    speed.y = -1.0;
  if (LeftPressed ())
    speed.x = -1.0;
  if (RightPressed ())
    speed.x = 1.0;

  /* if using a joystick/mouse, allow exact directional shots! */
  if ( axis_is_active )
    {
      max_val = max (abs(input_axis.x), abs(input_axis.y));
      speed.x = 1.0*input_axis.x/max_val;
      speed.y = 1.0*input_axis.y/max_val;
    }

  speed_norm = sqrt (speed.x * speed.x + speed.y * speed.y);
  CurBullet->speed.x = (speed.x/speed_norm);
  CurBullet->speed.y = (speed.y/speed_norm);

  // now determine the angle of the shot
  CurBullet->angle= - ( atan2 (speed.y,  speed.x) * 180 / M_PI + 90 );

  DebugPrintf( 1 , "\nFireBullet(...) : Phase of bullet=%d." , CurBullet->phase );
  DebugPrintf( 1 , "\nFireBullet(...) : angle of bullet=%f." , CurBullet->angle );

  //  printf_SDL(ne_screen, User_Rect.x, User_Rect.y, "Bullet speed: %g %g ",
  //	     CurBullet->speed.x, CurBullet->speed.y);
  //  getchar_raw();

  CurBullet->speed.x *= BulletSpeed;
  CurBullet->speed.y *= BulletSpeed;

  // To prevent influ from hitting himself with his own bullets,
  // move them a bit..
  CurBullet->pos.x += 0.5 * (CurBullet->speed.x/BulletSpeed);
  CurBullet->pos.y += 0.5 * (CurBullet->speed.y/BulletSpeed);

  CurBullet->time_in_frames = 0;
  CurBullet->time_in_seconds=0;

  return;

}; // FireBullet

/*@Function============================================================
@Desc: RefreshInfluencer(): Refresh fields can be used to regain energy
lost due to bullets or collisions, but not energy lost due to permanent
loss of health in PermanentLoseEnergy.

NEW: this function now takes into account the framerates.

@Ret: void
@In
* $Function----------------------------------------------------------*/
void
RefreshInfluencer (void)
{
  static int timecounter = 3;	/* to slow down healing process */

  if (--timecounter)
    return;
  if (timecounter == 0)
    timecounter = 3;

  if (Me.energy < Me.health)
    {
      Me.energy += REFRESH_ENERGY * Frame_Time () * 5;
      RealScore -= REFRESH_ENERGY * Frame_Time () * 10;

      if (RealScore < 0) RealScore = 0; // don't go negative...

      if (Me.energy > Me.health)
	Me.energy = Me.health;

      if (LastRefreshSound > 0.6)
	{
	  RefreshSound ();
	  LastRefreshSound = 0;
	}

      //--------------------
      // since robots like the refresh, the influencer might also say so...
      //
      if ( GameConfig.Droid_Talk)
	{
	  Me.TextToBeDisplayed="Ahhh, that feels so good...";
	  Me.TextVisibleTime=0;
	}
    }
  else
    {
      //--------------------
      // If nothing more is to be had, the influencer might also say so...
      if ( GameConfig.Droid_Talk)
	{
	  Me.TextToBeDisplayed="Oh, it seems that was it again.";
	  Me.TextVisibleTime=0;
	}
    }

  return;
}				/* RefreshInfluence */

/*@Function============================================================
@Desc: influ-enemy collisions are sucking someones
       energy, depending no longer on classes of the colliding parties,
       but on their weights

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void
InfluEnemyCollisionLoseEnergy (int enemynum)
{
  int enemytype = AllEnemys[enemynum].type;

  if (Me.type <= enemytype)
    {
      if (InvincibleMode)
	return;

      Me.energy -=
	(Druidmap[enemytype].class - Druidmap[Me.type].class) * collision_lose_energy_calibrator;


      /*      Me.energy -=
	(Druidmap[enemytype].weight -
	Druidmap[Me.type].weight ) * collision_lose_energy_calibrator * 0.01 ; */
    }
  else
    AllEnemys[enemynum].energy -=
	 - (Druidmap[enemytype].class - Druidmap[Me.type].class) * collision_lose_energy_calibrator;
  //      (Druidmap[Me.type].weight -
  //       Druidmap[enemytype].weight ) * collision_lose_energy_calibrator * 0.01;
  //    else AllEnemys[enemynum].energy -= BOUNCE_LOSE_ENERGY;

  return;
}; // void InfluEnemyCollisionLoseEnergy(int enemynum)

/*@Function============================================================
@Desc: PermanentLoseEnergy(): In the classic paradroid game, the influencer
continuously lost energy.  This loss was, in contrast to damage from fighting
and collisions, NOT regainable by using refresh fields.

NEW: this function now takes into account the framerate.

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void
PermanentLoseEnergy (void)
{
  // Of course if in invincible mode, no energy will ever be lost...
  if (InvincibleMode) return;

  /* health decreases with time */
  Me.health -= Druidmap[Me.type].lose_health * Frame_Time();

  /* you cant have more energy than health */
  if (Me.energy > Me.health) Me.energy = Me.health;

} // void PermanentLoseEnergy(void)

#undef _influ_c
