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
 * This file contains all features, movement, fireing, collision and 
 * extras of the influencer.
 * ---------------------------------------------------------------------- */

/*
 * This file has been checked for remains of german comments in the code
 * I you still find some, please let me know.
 */


#define _influ_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"

#define TIMETILLNEXTBULLET 14

#define REFRESH_ENERGY		3
#define COLLISION_PUSHSPEED	7

#define BOUNCE_LOSE_ENERGY 3	/* amount of lose-energy at enemy-collisions */
#define BOUNCE_LOSE_FACT 1
void InfluEnemyCollisionLoseEnergy (int enemynum);	/* influ can lose energy on coll. */
void PermanentLoseEnergy (void);	/* influ permanently loses energy */
int NoInfluBulletOnWay (void);

int CurrentZeroRingIndex=0;

#define max(x,y) ((x) < (y) ? (y) : (x) ) 
#define MAXIMAL_STEP_SIZE ( 7.0/20 )

/* ----------------------------------------------------------------------
 * This function initializes the influencers position history, which is
 * a ring buffer and is needed for throwing the influencer back (only one
 * or two positions would be needed for that) and for influencers followers
 * to be able to track the influencers path (10000 or so positions are used
 * for that, and that's why it is a ring buffer).
 * ---------------------------------------------------------------------- */
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
  // float accel = Druidmap[Me.type].accel;
  float accel = ItemMap[ Me.drive_item.type ].item_drive_accel;
  float planned_step_x;
  float planned_step_y;
  static float TransferCounter = 0;

  accel *= Frame_Time();

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
	  Me.speed.x = 0;
	  Me.speed.y = 0;
	  Me.energy = PreTakeEnergy;
	  Me.health = BLINKENERGY;
	  StartBlast (Me.pos.x, Me.pos.y, DRUIDBLAST);
	}
      else
	{
	  Me.status = OUT;
	  ThouArtDefeated ();
	  DebugPrintf (2, "\nvoid MoveInfluence(void):  Alternate end of function reached.");
	  return;
	}
    }

  
  // Time passed before entering Transfermode ??
  if ( TransferCounter >= WAIT_TRANSFERMODE )
    {
      Me.status = TRANSFERMODE;
      TransferCounter=0;
    }


  //--------------------
  // Acceleration occurs, but only if there is at least some
  // drive unit present!!!  Otherwise only a comment will be
  // printed out!!
  //
  if ( Me.drive_item.type != (-1) )
    {
      if (UpPressed ())
	Me.speed.y -= accel;
      if (DownPressed ())
	Me.speed.y += accel;
      if (LeftPressed ())
	Me.speed.x -= accel;
      if (RightPressed ())
	Me.speed.x += accel;
    }
  else
    {
      Me.TextVisibleTime = 0;
      Me.TextToBeDisplayed = "Can't go anywhere far without at least some drive! Sorry...";
    }

  if (!SpacePressed ())
    Me.status = MOBILE;

  if (TransferCounter == 1)
    {
      Me.status = TRANSFERMODE;
      TransferCounter = 0;
    }

  HandleCurrentlyActivatedSkill();
  /*
  if (MouseRightPressed() == 1)
    Me.status = TRANSFERMODE;
  */

  if ( (SpacePressed ()) && (NoDirectionPressed ()) &&
       (Me.status != WEAPON) && (Me.status != TRANSFERMODE) )
    TransferCounter += Frame_Time();

  if ( (SpacePressed ()) && (!NoDirectionPressed () ) &&
       (Me.status != TRANSFERMODE) )
    Me.status = WEAPON;


  if (stop_influencer)
    {
      Me.speed.x = 0.0;
      Me.speed.y = 0.0;
      if (SpacePressed())
	{
	  Me.firewait = 0;
	  FireBullet ();
	}
    }
  else
    {

      if ((SpacePressed ()) && (!NoDirectionPressed ()) && (Me.status == WEAPON)
	  && (Me.firewait == 0) && (NoInfluBulletOnWay ()))
	FireBullet ();
    }

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
  //--------------------
  // Now if the influencer is on some form of conveyor belt, we adjust the planned step
  // accoringly
  //
  switch ( CurLevel->map[ (int) rintf(Me.pos.y) ] [ (int) rintf( Me. pos.x ) ] )
    {
    case CONVEY_L:
      planned_step_x+=Conveyor_Belt_Speed*Frame_Time();
      break;
    case CONVEY_D:
      planned_step_y+=Conveyor_Belt_Speed*Frame_Time();
      break;
    case CONVEY_R:
      planned_step_x-=Conveyor_Belt_Speed*Frame_Time();
      break;
    case CONVEY_U:
      planned_step_y-=Conveyor_Belt_Speed*Frame_Time();
      break;
    default:
      break;
    }
  Me.pos.x += planned_step_x;
  Me.pos.y += planned_step_y;

  //--------------------
  // Check it the influ is on a special field like a lift, a console or a refresh or a conveyor belt
  //
  ActSpecialField ( Me.pos.x , Me.pos.y );

  AnimateInfluence ();	// move the "phase" of influencers rotation

}; // void MoveInfluence( void );


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

  if ( ! ItemMap[ Me.weapon_item.type ].item_gun_oneshotonly )
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
AnimateInfluence (void)
{
#define TOTAL_SWING_TIME 0.35
#define FULL_BREATHE_TIME 3
#define TOTAL_STUNNED_TIME 0.35

  if ( Me.got_hit_time != (-1) )
    {
      Me.phase = TUX_SWING_PHASES + TUX_BREATHE_PHASES + 
	( Me.got_hit_time * TUX_GOT_HIT_PHASES * 1.0 / TOTAL_STUNNED_TIME ) ;
      if ( Me.got_hit_time > TOTAL_STUNNED_TIME ) Me.got_hit_time = (-1) ;
    }
  else if ( Me.weapon_swing_time == (-1) )
    {
      Me.phase = ( (int) ( Me.MissionTimeElapsed * TUX_BREATHE_PHASES / FULL_BREATHE_TIME ) ) % TUX_BREATHE_PHASES ;
    }
  else
    {
      Me.phase = ( TUX_BREATHE_PHASES + ( Me.weapon_swing_time * TUX_SWING_PHASES * 1.0 / TOTAL_SWING_TIME ) ) ;
      if ( Me.weapon_swing_time > TOTAL_SWING_TIME ) Me.weapon_swing_time = (-1) ;
      if (((int) (Me.phase)) >= TUX_SWING_PHASES + TUX_BREATHE_PHASES )
	{
	  Me.phase = 0;
	}

    }

  if (((int) (Me.phase)) >= TUX_SWING_PHASES + TUX_BREATHE_PHASES + TUX_GOT_HIT_PHASES )
    {
      Me.phase = 0;
    }

  /*
  Me.phase +=
    (Me.energy / ( Me.maxenergy)) * Frame_Time () *
    DROID_PHASES * 3;
  */

  /*
  if (Me.type != DRUID001)
    {
      Me.phase +=
	(Me.energy / (Druidmap[Me.type].maxenergy + Druidmap[DRUID001].maxenergy)) * Frame_Time () *
	DROID_PHASES * 3;
    }
  else
    {
      Me.phase +=
	(Me.energy / (Druidmap[DRUID001].maxenergy)) * Frame_Time () *
	DROID_PHASES * 3;
    }
  */

  /*
  if (((int) rintf (Me.phase)) >= DROID_PHASES)
    {
      Me.phase = 0;
    }
  */
}; // void AnimateInfluence ( void )

/* ----------------------------------------------------------------------
 * This function checks for collisions of the influencer with walls,
 * doors, consoles, boxes and all other map elements.
 * In case of a collision, the position and speed of the influencer are
 * adapted accordingly.
 * ---------------------------------------------------------------------- */
void
CheckInfluenceWallCollisions (void)
{
  double SX = Me.speed.x * Frame_Time ();
  double SY = Me.speed.y * Frame_Time ();
  finepoint lastpos;
  int res; 
  int NorthSouthAxisBlocked=FALSE;
  int EastWestAxisBlocked=FALSE;
  int H_Door_Sliding_Active = FALSE;
  double maxspeed = ItemMap [ Me.drive_item.type ].item_drive_maxspeed ;

  lastpos.x = Me.pos.x - SX;
  lastpos.y = Me.pos.y - SY;

  res = DruidPassable (Me.pos.x, Me.pos.y);

  //--------------------
  // Influence-Wall-Collision only has to be checked in case of
  // a collision of course, which is indicated by res not CENTER.
  if (res != CENTER )
    {

      //--------------------
      // At first we just check in which directions (from the last position)
      // the ways are blocked and in which directions the ways are open.
      //
      if ( ! ( ( DruidPassable(lastpos.x , lastpos.y + maxspeed * Frame_Time() ) != CENTER ) ||
	       ( DruidPassable(lastpos.x , lastpos.y - maxspeed * Frame_Time() ) != CENTER ) ) )
	{
	  DebugPrintf(1, "\nNorth-south-Axis seems to be free.");
	  NorthSouthAxisBlocked = FALSE;
	}
      else
	{
	  NorthSouthAxisBlocked = TRUE;
	}

      if ( ( DruidPassable(lastpos.x + maxspeed * Frame_Time() , lastpos.y ) == CENTER ) &&
	   ( DruidPassable(lastpos.x - maxspeed * Frame_Time() , lastpos.y ) == CENTER ) )
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

} /* CheckInfluenceWallCollisions */

/* ----------------------------------------------------------------------
 * This function adapts the influencers current speed to the maximal speed
 * possible for the influencer (determined by the currely used drive type).
 * ---------------------------------------------------------------------- */
void
AdjustSpeed (void)
{
  double maxspeed = ItemMap [ Me.drive_item.type ].item_drive_maxspeed ;
  if (Me.speed.x > maxspeed)
    Me.speed.x = maxspeed;
  if (Me.speed.x < (-maxspeed))
    Me.speed.x = (-maxspeed);

  if (Me.speed.y > maxspeed)
    Me.speed.y = maxspeed;
  if (Me.speed.y < (-maxspeed))
    Me.speed.y = (-maxspeed);
}; // void AdjustSpeed ( void ) 

/* ----------------------------------------------------------------------
 * This function reduces the influencers speed as long as no direction 
 * key of any form is pressed.
 * ---------------------------------------------------------------------- */
void
InfluenceFrictionWithAir (void)
{

  if (!UpPressed () && !DownPressed ())
    {
      /*
	if (Me.speed.y < 0)
	Me.speed.y++;
	if (Me.speed.y > 0)
	Me.speed.y--;
      */
      Me.speed.y *= exp(log(0.02) * Frame_Time());
    }
  if (!RightPressed () && !LeftPressed ())
    {
      /*
	if (Me.speed.x < 0)
	Me.speed.x++;
	if (Me.speed.x > 0)
	Me.speed.x--;
      */
      Me.speed.x *= exp(log(0.02) * Frame_Time());
    }

}; // InfluenceFrictionWithAir (void)

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

  Me.status = OUT;

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
	Me.pos.x - Druid_Radius_X / 2 + MyRandom (10)*0.05;
      AllBlasts[counter].pos.y =
	Me.pos.y - Druid_Radius_Y / 2 + MyRandom (10)*0.05;
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
  for (i = 0; i < MAX_ENEMYS_ON_SHIP ; i++)
    {

      //--------------------
      // ignore enemy that are not on this level or dead 
      //
      if (AllEnemys[i].levelnum != CurLevel->levelnum)
	continue;
      if (AllEnemys[i].Status == OUT)
	continue;

      //--------------------
      // We determine the distance and back out immediately if there
      // is still one whole square distance or even more...
      //
      xdist = Me.pos.x - AllEnemys[i].pos.x;
      ydist = Me.pos.y - AllEnemys[i].pos.y;
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
      if ( Me.status == TRANSFERMODE )
	{
	  if ( ! AllEnemys[i].Friendly ) Takeover (i);
	  else ChatWithFriendlyDroid( i );

	  if (LevelEmpty ())
	    CurLevel->empty = WAIT_LEVELEMPTY;

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
	  
	  // there might be walls close too, so lets check again for collisions with them
	  CheckInfluenceWallCollisions ();
	  
	  BounceSound ();
	  
	} // if first_collision (ALWAYS TRUE ANYWAY...)
      
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
      
    }				/* for */

}; // void CheckInfluenceEnemyCollision( void )

/* ----------------------------------------------------------------------
 * This function fires a bullet from the influencer in some direction, or
 * at least it TRIES to fire a bullet from the influencer, cause maybe
 * the influencer can't fire for this reason or another right now...
 * ---------------------------------------------------------------------- */
void
FireBullet (void)
{
  int i = 0;
  Bullet CurBullet = NULL;  // the bullet we're currentl dealing with
  int guntype = ItemMap[ Me.weapon_item.type ].item_gun_bullet_image_type;   // which gun do we have ? 
  double BulletSpeed = ItemMap[ Me.weapon_item.type ].item_gun_speed;
  double speed_norm;
  moderately_finepoint speed;
  int max_val;
  float OffsetFactor;
  moderately_finepoint Weapon_Target_Vector;
  float angle;

  // If the current overtaken droid doesn't have a weapon at all, just return
  if ( Me.weapon_item.type == (-1) ) return;

  // If the influencer is holding something from the invenotry
  // menu via the mouse, also just return
  // if ( Item_Held_In_Hand != (-1) ) return;

  // If the influencer has pressed fire with the mouse cursor
  // and is in the inventory screen and inventory screen is 
  // active, then also just return
  //
  // And for the character screen, do a similar thing
  //
  if ( axis_is_active && GameConfig.Inventory_Visible && ! CursorIsInUserRect( GetMousePos_x() , GetMousePos_y() ) ) return;
  if ( axis_is_active && GameConfig.CharacterScreen_Visible && ! CursorIsInUserRect( GetMousePos_x() , GetMousePos_y() ) ) return;
  if ( GameConfig.CharacterScreen_Visible && GameConfig.Inventory_Visible ) return;

  // If influencer hasn't recharged yet, fireing is impossible, we're done here and return
  if (Me.firewait > 0)
    return;

  //--------------------
  // We should always make the sound of a fired bullet (or weapon swing)
  //
  Fire_Bullet_Sound ( guntype );

  //--------------------
  // We always start the weapon application cycle, i.e. change of tux
  // motion phases
  //
  // But it the currently used weapon is a melee weapon, the tux no longer
  // generates a bullet, but rather does his weapon swinging motion and
  // only the damage is done to the robots in the area of effect
  //
  Me.weapon_swing_time = 0;
  if ( ItemMap [ Me.weapon_item.type ].item_gun_angle_change != 0 )
    {
      //--------------------
      // Since a melee weapon is swung, we calculate where the point
      // of the weapon should be finally hitting and do some damage
      // to all the enemys in that area.
      //
      angle = - ( atan2 ( input_axis.y,  input_axis.x ) * 180 / M_PI + 90 );
      Weapon_Target_Vector.x = 0;
      Weapon_Target_Vector.y = - 1.0;
      RotateVectorByAngle ( & Weapon_Target_Vector , angle );
      Weapon_Target_Vector.x += Me.pos.x;
      Weapon_Target_Vector.y += Me.pos.y;
      
      for ( i = 0 ; i < Number_Of_Droids_On_Ship ; i ++ )
	{
	  if ( AllEnemys[i].Status == OUT ) continue;
	  if ( AllEnemys[i].levelnum != CurLevel->levelnum ) continue;
	  if ( fabsf ( AllEnemys[i].pos.x - Weapon_Target_Vector.x ) > 0.5 ) continue;
	  if ( fabsf ( AllEnemys[i].pos.y - Weapon_Target_Vector.y ) > 0.5 ) continue;
	  AllEnemys[ i ].energy -= Me.base_damage + MyRandom( Me.damage_modifier );
	  AllEnemys[ i ].firewait = 2 * ItemMap [ Druidmap [ AllEnemys[ i ].type ].weapon_item.type ].item_gun_recharging_time ;
	}
      
      //--------------------
      // Finally we add a new wait-counter, so that bullets or swings
      // cannot be started in too rapid succession.  
      // 
      // And then we can return, for real bullet generation isn't required in
      // our case here.
      //
      Me.firewait = ItemMap[ Me.weapon_item.type ].item_gun_recharging_time;
      return;
    }

  // search for the next free bullet list entry
  for (i = 0; i < (MAXBULLETS); i++)
    {
      if (AllBullets[i].type == OUT)
	{
	  CurBullet = &AllBullets[i];
	  break;
	}
    }

  // didn't find any free bullet entry? --> take the first
  if (CurBullet == NULL)
    CurBullet = &AllBullets[0];

  CurBullet->pos.x = Me.pos.x;
  CurBullet->pos.y = Me.pos.y;
  CurBullet->type = guntype;

  //--------------------
  // Previously, we had the damage done only dependant upon the weapon used.  Now
  // the damage value is taken directly from the character stats, and the UpdateAll...stats
  // has to do the right computation and updating of this value.  hehe. very conventient.
  CurBullet->damage = Me.base_damage + MyRandom( Me.damage_modifier);
  CurBullet->mine = TRUE;
  CurBullet->owner = -1;
  CurBullet->bullet_lifetime = ItemMap[ Me.weapon_item.type ].item_gun_bullet_lifetime;
  CurBullet->angle_change_rate = ItemMap[ Me.weapon_item.type ].item_gun_angle_change;
  CurBullet->fixed_offset = ItemMap[ Me.weapon_item.type ].item_gun_fixed_offset;
  CurBullet->ignore_wall_collisions = ItemMap[ Me.weapon_item.type ].item_gun_bullet_ignore_wall_collisions;
  CurBullet->owner_pos = & ( Me.pos );
  CurBullet->time_in_frames = 0;
  CurBullet->time_in_seconds = 0;
  CurBullet->was_reflected = FALSE;
  CurBullet->reflect_other_bullets = ItemMap[ Me.weapon_item.type ].item_gun_bullet_reflect_other_bullets;
  CurBullet->pass_through_explosions = ItemMap[ Me.weapon_item.type ].item_gun_bullet_pass_through_explosions;
  CurBullet->pass_through_hit_bodies = ItemMap[ Me.weapon_item.type ].item_gun_bullet_pass_through_hit_bodies;
  CurBullet->miss_hit_influencer = UNCHECKED ;
  memset( CurBullet->total_miss_hit , UNCHECKED , MAX_ENEMYS_ON_SHIP );
  CurBullet->to_hit = Me.to_hit;
  Me.firewait = ItemMap[ Me.weapon_item.type ].item_gun_recharging_time;

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

  //--------------------
  // It might happen, that this is not a normal shot, but rather the
  // swing of a melee weapon.  Then of course, we should make a swing
  // and not start in this direction, but rather somewhat 'before' it,
  // so that the rotation will hit the target later.
  //
  RotateVectorByAngle ( & speed , ItemMap[ Me.weapon_item.type ].item_gun_start_angle_modifier );

  speed_norm = sqrt (speed.x * speed.x + speed.y * speed.y);
  CurBullet->speed.x = (speed.x/speed_norm);
  CurBullet->speed.y = (speed.y/speed_norm);

  //--------------------
  // Now we determine the angle of rotation to be used for
  // the picture of the bullet itself
  //
  
  CurBullet->angle= - ( atan2 (speed.y,  speed.x) * 180 / M_PI + 90 );

  DebugPrintf( 1 , "\nFireBullet(...) : Phase of bullet=%d." , CurBullet->phase );
  DebugPrintf( 1 , "\nFireBullet(...) : angle of bullet=%f." , CurBullet->angle );
  
  //  printf_SDL(Screen, User_Rect.x, User_Rect.y, "Bullet speed: %g %g ",
  //	     CurBullet->speed.x, CurBullet->speed.y);
  //  getchar_raw();

  CurBullet->speed.x *= BulletSpeed;
  CurBullet->speed.y *= BulletSpeed;

  // To prevent influ from hitting himself with his own bullets,
  // move them a bit..
  if ( CurBullet->angle_change_rate == 0 ) OffsetFactor = 0.5; else OffsetFactor = 1;
  CurBullet->pos.x += OffsetFactor * (CurBullet->speed.x/BulletSpeed);
  CurBullet->pos.y += OffsetFactor * (CurBullet->speed.y/BulletSpeed);
  // CurBullet->pos.x += 0.5 ;
  // CurBullet->pos.y += 0.5 ;

  return;
}; // FireBullet 

/* ----------------------------------------------------------------------
 * This function does all the things needed, when the influencer is on
 * some refresh field, i.e. it increases influencers current energy but
 * it also decreases his current score = experience points...
 * ---------------------------------------------------------------------- */
void
RefreshInfluencer (void)
{

  if ( Me.energy < Me.maxenergy )
    {
      Me.energy += REFRESH_ENERGY * Frame_Time () * 5;
      Me.Experience -= REFRESH_ENERGY * Frame_Time () * 10;
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
      if ( GameConfig.Influencer_Refresh_Text )
	{
	  Me.TextToBeDisplayed="Ahhh, that feels so good...";
	  Me.TextVisibleTime=0;
	}
    }
  else
    {
      //--------------------
      // If nothing more is to be had, the influencer might also say so...
      if ( GameConfig.Influencer_Refresh_Text )
	{
	  Me.TextToBeDisplayed="Oh, it seems that was it again.";
	  Me.TextVisibleTime=0;
	}
    }

  return;
}; // void RefreshInfluence ( void )

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

  if ( AllEnemys[enemynum].Friendly ) return;

  if (Me.type <= enemytype)
    {
      if (InvincibleMode)
	return;
      
      /* This old code used class difference to determine collision damage.
	 But now we use Weight-Difference. 
      Me.energy -=
	(Druidmap[enemytype].class -
	 Druidmap[Me.type].class) * BOUNCE_LOSE_FACT;
      */
      Me.energy -=
	(Druidmap[enemytype].weight -
	 Druidmap[Me.type].weight ) * collision_lose_energy_calibrator * 0.01 ;
    }
  else
    AllEnemys[enemynum].energy -=
      (Druidmap[Me.type].weight -
       Druidmap[enemytype].weight ) * collision_lose_energy_calibrator * 0.01;

  //    else AllEnemys[enemynum].energy -= BOUNCE_LOSE_ENERGY;

  return;
}; // void InfluEnemyCollisionLoseEnergy(int enemynum)

/* ----------------------------------------------------------------------
 *
 * In the classic paradroid game, the influencer
 * continuously lost energy.  This loss was, in contrast to damage from fighting
 * and collisions, NOT regainable by using refresh fields.
 * 
 * NEW: this function now takes into account the framerate.
 *
 * ---------------------------------------------------------------------- */
void
PermanentLoseEnergy (void)
{
  // Of course if in invincible mode, no energy will ever be lost...
  if (InvincibleMode) return;

  /* health decreases with time */
  Me.health -= Druidmap[Me.type].lose_health * Frame_Time () * Me.Current_Victim_Resistance_Factor;

  /* you cant have more energy than health */
  if (Me.energy > Me.health) Me.energy = Me.health;

} // void PermanentLoseEnergy(void)

#undef _influ_c
