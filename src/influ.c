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

#define FRICTION_CONSTANT (0.02)

#define BEST_MELEE_DISTANCE_IN_SQUARES (1.0)
#define BEST_CHAT_DISTANCE_IN_SQUARES ( BEST_MELEE_DISTANCE_IN_SQUARES + 0.2 )
#define DISTANCE_TOLERANCE (0.2)
#define FORCE_FIRE_DISTANCE (1.5)
#define DROID_SELECTION_TOLERANCE (0.9)

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
InitInfluPositionHistory( int PlayerNum )
{
  int RingPosition;

  for ( RingPosition = 0 ; RingPosition < MAX_INFLU_POSITION_HISTORY ; RingPosition ++ )
    {
      Me [ PlayerNum ] . Position_History_Ring_Buffer [ RingPosition ] . x = Me [ PlayerNum ] . pos . x ;
      Me [ PlayerNum ] . Position_History_Ring_Buffer [ RingPosition ] . y = Me [ PlayerNum ] . pos . y ;
      // Me [ PlayerNum ] . Position_History_Ring_Buffer [ RingPosition ] . z = CurLevel->levelnum ;
      Me [ PlayerNum ] . Position_History_Ring_Buffer [ RingPosition ] . z = Me [ PlayerNum ] . pos . z ;
    }
} // void InitInfluPositionHistory( void )

float 
GetInfluPositionHistoryX( int HowLongPast )
{
  int RingPosition;

  RingPosition = CurrentZeroRingIndex - HowLongPast;

  RingPosition += MAX_INFLU_POSITION_HISTORY; // We don't want any negative values, for safety

  RingPosition %= MAX_INFLU_POSITION_HISTORY; // We do MODULO for the Ring buffer length 

  return Me[0].Position_History_Ring_Buffer[ RingPosition ].x;
}

float 
GetInfluPositionHistoryY( int HowLongPast )
{
  int RingPosition;

  RingPosition = CurrentZeroRingIndex - HowLongPast;

  RingPosition += MAX_INFLU_POSITION_HISTORY; // We don't want any negative values, for safety

  RingPosition %= MAX_INFLU_POSITION_HISTORY; // We do MODULO for the Ring buffer length 

  return Me[0].Position_History_Ring_Buffer[ RingPosition ].y;
}

float 
GetInfluPositionHistoryZ( int HowLongPast )
{
  int RingPosition;

  RingPosition = CurrentZeroRingIndex - HowLongPast;

  RingPosition += MAX_INFLU_POSITION_HISTORY; // We don't want any negative values, for safety

  RingPosition %= MAX_INFLU_POSITION_HISTORY; // We do MODULO for the Ring buffer length 

  return Me[0].Position_History_Ring_Buffer[ RingPosition ].z;
}

/* ----------------------------------------------------------------------
 * T
 *
 * ---------------------------------------------------------------------- */
void 
CheckIfCharacterIsStillOk ( int PlayerNum ) 
{

  //--------------------
  // This is something, that needs to be done ONLY for the 
  // one character of this client programm!!!!
  //
  if ( PlayerNum != 0 ) return;

  //------------------------------
  // Now we check if the main character is really still ok.
  //
  if ( Me [ PlayerNum ] .energy <= 0 )
    {
      if ( Me [ PlayerNum ] .type != DRUID001 )
	{
	  Me [ PlayerNum ] .type = DRUID001;
	  Me [ PlayerNum ] .speed.x = 0;
	  Me [ PlayerNum ] .speed.y = 0;
	  Me [ PlayerNum ] .energy = PreTakeEnergy;
	  Me [ PlayerNum ] .health = BLINKENERGY;
	  StartBlast ( Me [ PlayerNum ] .pos.x, Me [ PlayerNum ] .pos.y, Me [ PlayerNum ] .pos.z , DRUIDBLAST );
	}
      else
	{
	  Me [ PlayerNum ] .status = OUT;

	  if ( !ServerMode ) ThouArtDefeated ();
	  
	  DebugPrintf (2, "\nvoid MoveInfluence(void):  Alternate end of function reached.");
	  return;
	}
    }

}; // void CheckIfCharacterIsStillOk ( int PlayerNum ) 


/* ----------------------------------------------------------------------
 * This function moves the influencer, adjusts his speed according to
 * keys pressed and also adjusts his status and current "phase" of his 
 * rotation.
 * ---------------------------------------------------------------------- */
void
MoveInfluence ( int PlayerNum )
{
  float accel;
  float planned_step_x;
  float planned_step_y;
  static float TransferCounter = 0;
  moderately_finepoint RemainingWay;
  moderately_finepoint MinimalWayAtThisSpeed;
  float RemainingWayLength;

  //--------------------
  // We do not move any players, who's statuses are 'OUT'.
  //
  if ( Me [ PlayerNum ] . status == OUT ) return;
  // if ( Me [ PlayerNum ] . energy <= 0 ) return;

  accel = ItemMap[ Me [ PlayerNum ] .drive_item.type ].item_drive_accel * Frame_Time( ) ;

  //--------------------
  // We store the influencers position for the history record and so that others
  // can follow his trail.
  //
  CurrentZeroRingIndex++;
  CurrentZeroRingIndex %= MAX_INFLU_POSITION_HISTORY;
  Me [ PlayerNum ] . Position_History_Ring_Buffer [CurrentZeroRingIndex].x = Me [ PlayerNum ] .pos.x;
  Me [ PlayerNum ] . Position_History_Ring_Buffer [CurrentZeroRingIndex].y = Me [ PlayerNum ] .pos.y;
  Me [ PlayerNum ] . Position_History_Ring_Buffer [CurrentZeroRingIndex].z = CurLevel->levelnum ;

  PermanentLoseEnergy ();	/* influ permanently loses energy */

  // check, if the influencer is still ok
  CheckIfCharacterIsStillOk ( PlayerNum ) ;
  
  // Time passed before entering Transfermode ??
  if ( TransferCounter >= WAIT_TRANSFERMODE )
    {
      Me [ PlayerNum ] .status = TRANSFERMODE;
      TransferCounter=0;
    }


  //--------------------
  // Acceleration occurs, but only if there is at least some
  // drive unit present!!!  Otherwise only a comment will be
  // printed out!!
  //
  if ( Me [ PlayerNum ] . drive_item . type != (-1) )
    {

      //--------------------
      // As a preparation for the later operations, we see if there is
      // a living droid set as a target, and if yes, we correct the move
      // target to something suiting that new droids position.
      //
      if ( Me [ PlayerNum ] . mouse_move_target_is_enemy != (-1) )
	{
	  //--------------------
	  // If the position is close already, we hold the position.
	  // Otherwise we select some position suitably close.
	  //
	  if ( ( AllEnemys [ Me [ PlayerNum ] . mouse_move_target_is_enemy ] . Status == OUT ) ||
	       ( AllEnemys [ Me [ PlayerNum ] . mouse_move_target_is_enemy ] . pos . z != 
		 Me [ PlayerNum ] . pos . z ) )
	    {
	      Me [ PlayerNum ] . mouse_move_target_is_enemy = ( -1 ) ;
	      Me [ PlayerNum ] . mouse_move_target . x = ( -1 ) ;
	      Me [ PlayerNum ] . mouse_move_target . y = ( -1 ) ;
	      Me [ PlayerNum ] . mouse_move_target . z = ( -1 ) ;
	    }
	  else
	    {
	      Me [ PlayerNum ] . mouse_move_target . x = 
		AllEnemys [ Me [ PlayerNum ] . mouse_move_target_is_enemy ] . pos . x ;
	      Me [ PlayerNum ] . mouse_move_target . y = 
		AllEnemys [ Me [ PlayerNum ] . mouse_move_target_is_enemy ] . pos . y ;
	      // }

	      RemainingWay . x = Me [ PlayerNum ] . pos . x - Me [ PlayerNum ] . mouse_move_target . x ;
	      RemainingWay . y = Me [ PlayerNum ] . pos . y - Me [ PlayerNum ] . mouse_move_target . y ;

	      RemainingWayLength = sqrtf ( ( RemainingWay . x ) * ( RemainingWay . x ) +
					   ( RemainingWay . y ) * ( RemainingWay . y ) ) ;

	      RemainingWay . x = ( RemainingWay . x / RemainingWayLength ) * 
		( RemainingWayLength - BEST_MELEE_DISTANCE_IN_SQUARES ) ;
	      RemainingWay . y = ( RemainingWay . y / RemainingWayLength ) * 
		( RemainingWayLength - BEST_MELEE_DISTANCE_IN_SQUARES ) ;


	      Me [ PlayerNum ] . mouse_move_target . x = Me [ PlayerNum ] . pos . x - RemainingWay . x ;
	      Me [ PlayerNum ] . mouse_move_target . y = Me [ PlayerNum ] . pos . y - RemainingWay . y ;


	  //--------------------
	  // Now we stop one square before crashing into our target...
	  //
	  // if ( fabsf( RemainingWay . x ) > BEST_MELEE_DISTANCE_IN_SQUARES )
	  // {
	      // Me [ PlayerNum ] . mouse_move_target . x = Me [ PlayerNum ] . pos . x - copysignf ( fabsf ( RemainingWay . x ) - BEST_MELEE_DISTANCE_IN_SQUARES + 0.0 , RemainingWay . x ) ;
	      // }

	      // if ( fabsf( RemainingWay . y ) > BEST_MELEE_DISTANCE_IN_SQUARES )
	      // {
	      // Me [ PlayerNum ] . mouse_move_target . y = Me [ PlayerNum ] . pos . y - copysignf ( fabsf ( RemainingWay . y ) - BEST_MELEE_DISTANCE_IN_SQUARES + 0.0 , RemainingWay . y ) ;
	      // }

	    }

	}
      
      //--------------------
      // Now maybe there isn't any mouse move target present or mouse move
      // is somehow disabled.  Then we can just adapt the current speed 
      // according to the acceleration possible with the current drive unit.
      //
      if ( Me [ PlayerNum ] . mouse_move_target . x == ( -1 ) )
	{
	  if ( ServerThinksUpPressed ( PlayerNum ) )
	    Me [ PlayerNum ] .speed.y -= accel;
	  if ( ServerThinksDownPressed ( PlayerNum ) )
	    Me [ PlayerNum ] .speed.y += accel;
	  if ( ServerThinksLeftPressed ( PlayerNum ) )
	    Me [ PlayerNum ] .speed.x -= accel;
	  if ( ServerThinksRightPressed ( PlayerNum ) )
	    Me [ PlayerNum ] .speed.x += accel;
	}
      else
      //--------------------
      // But in case of some mouse move target present, we proceed to move
      // thowards this mouse move target.
      //
	{
	  //--------------------
	  // Let's do some mathematics:  We compute how far we have to go still
	  // and we also compute how far we will inevitably go even if we pull the breakes
	  // or even better use the usual friction with air to stop our motion immediately.
	  // Once we know that, we can simply decide if we still have to build up speed or
	  // if it's time to slow down again and so finally we will slide to a stop exactly
	  // at the place where we intend to be.  So:  Mathematics is always helpful. :)
	  //
	  RemainingWay . x = Me [ PlayerNum ] . pos . x - Me [ PlayerNum ] . mouse_move_target . x ;
	  RemainingWay . y = Me [ PlayerNum ] . pos . y - Me [ PlayerNum ] . mouse_move_target . y ;

	  MinimalWayAtThisSpeed . x = Me [ PlayerNum ] . speed . x / log ( FRICTION_CONSTANT ) ;
	  MinimalWayAtThisSpeed . y = Me [ PlayerNum ] . speed . y / log ( FRICTION_CONSTANT ) ;

	  if ( fabsf ( MinimalWayAtThisSpeed . x ) < fabsf ( RemainingWay . x ) )
	    {
	      if ( RemainingWay.x > 0 ) Me [ PlayerNum ] .speed.x -= accel;
	      else Me [ PlayerNum ] .speed.x += accel;
	    }
	  else
	    {
	      Me [ PlayerNum ] . speed . x *= exp ( log ( FRICTION_CONSTANT ) * Frame_Time ( ) );
	    }
	  if ( fabsf ( MinimalWayAtThisSpeed . y ) < fabsf ( RemainingWay . y ) )
	    {
	      if ( RemainingWay.y > 0 ) Me [ PlayerNum ] .speed.y -= accel;
	      else Me [ PlayerNum ] .speed.y += accel;
	    }
	  else
	    {
	      Me [ PlayerNum ] . speed . y *= exp ( log ( FRICTION_CONSTANT ) * Frame_Time ( ) );
	    }
	    
	  //--------------------
	  // In case we have reached our target, we can remove this mouse_move_target again,
	  // but also if we have been thrown onto a different level, we cancel our current
	  // mouse move target...
	  //
	  if ( ( ( fabsf ( RemainingWay.y ) <= DISTANCE_TOLERANCE ) && 
		 ( fabsf ( RemainingWay.x ) <= DISTANCE_TOLERANCE )     ) ||
	       ( Me [ PlayerNum ] . mouse_move_target . z != Me [ PlayerNum ] . pos . z ) )
	    {
	      Me [ PlayerNum ] . mouse_move_target . x = ( -1 ) ;
	      Me [ PlayerNum ] . mouse_move_target . y = ( -1 ) ;
	      Me [ PlayerNum ] . mouse_move_target . z = ( -1 ) ;

	      // Me [ PlayerNum ] . mouse_move_target_is_enemy = ( -1 ) ;
	    }

	}
    }
  else
    {
      Me [ PlayerNum ] .TextVisibleTime = 0;
      Me [ PlayerNum ] .TextToBeDisplayed = "Can't go anywhere far without at least some drive! Sorry...";
    }

  //--------------------
  // As long as the tux is still alive, his status will be either
  // in MOBILE mode or in WEAPON mode or in TRANSFER mode.
  //
  if ( Me [ PlayerNum ] . energy >= 0 )
    {
      if ( ! ServerThinksSpacePressed ( PlayerNum ) )
	{
	  Me [ PlayerNum ] .status = MOBILE;
	}
      if (TransferCounter == 1)
	{
	  Me [ PlayerNum ] .status = TRANSFERMODE;
	  TransferCounter = 0;
	}
      if ( ( ServerThinksSpacePressed ( PlayerNum ) ) && ( ServerThinksNoDirectionPressed ( PlayerNum ) ) &&
	   ( Me [ PlayerNum ] .status != WEAPON ) && ( Me [ PlayerNum ] .status != TRANSFERMODE ) )
	TransferCounter += Frame_Time();

      if ( ( ServerThinksSpacePressed ( PlayerNum ) || ServerThinksAxisIsActive ( PlayerNum ) ) && 
	   ( ! ServerThinksNoDirectionPressed ( PlayerNum ) ) &&
	   ( Me [ PlayerNum ] .status != TRANSFERMODE ) )
	Me [ PlayerNum ] .status = WEAPON ;
    }

  //--------------------
  // Perhaps the player has pressed the right mouse button, indicating the use
  // of the currently selected special function or spell.
  //
  HandleCurrentlyActivatedSkill();

  // --------------------
  // What is this code good for??
  // stop_influencer = FALSE ;
  //
  if ( stop_influencer )
    {
      Me [ PlayerNum ] . speed.x = 0.0;
      Me [ PlayerNum ] . speed.y = 0.0;
      if ( ServerThinksSpacePressed ( PlayerNum ) || ServerThinksAxisIsActive ( PlayerNum ) )
	{
	  Me [ PlayerNum ] . firewait = 0;
	  FireBullet ( PlayerNum );
	}
    }
  else
    {
      if ( ( ServerThinksSpacePressed ( PlayerNum ) || ServerThinksAxisIsActive ( PlayerNum ) ) && 
	   ( ! ServerThinksNoDirectionPressed ( PlayerNum ) ) && 
	   ( Me [ PlayerNum ] . status == WEAPON ) && 
	   ( Me [ PlayerNum ] . firewait == 0 ) && 
	   ( NoInfluBulletOnWay ( ) ) )
	FireBullet ( PlayerNum );
    }

  //--------------------
  // The influ should lose some of his speed when no key is pressed and
  // also no mouse move target is set.
  //
  if ( Me [ PlayerNum ] . mouse_move_target . x == (-1) ) InfluenceFrictionWithAir ( PlayerNum ) ; 

  AdjustSpeed ( PlayerNum ) ;  // If the influ is faster than allowed for his type, slow him

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
  planned_step_x = Me [ PlayerNum ] .speed.x * Frame_Time ();
  planned_step_y = Me [ PlayerNum ] .speed.y * Frame_Time ();
  if ( fabsf(planned_step_x) >= MAXIMAL_STEP_SIZE )
    {
      planned_step_x = copysignf( MAXIMAL_STEP_SIZE , planned_step_x );
    }
  if ( fabsf(planned_step_y) >= MAXIMAL_STEP_SIZE )
    {
      planned_step_y = copysignf( MAXIMAL_STEP_SIZE , planned_step_y );
    }

  //--------------------
  // Now perhaps the influencer is out of bounds, i.e. outside of the map.
  // This would cause a segfault immediately afterwards, when checking
  // for the current map tile to be conveyor or not.  Therefore we add some
  // extra security against segfaults and increased diagnosis functionality
  // here.
  //
  if ( ( (int) rintf( Me [ PlayerNum ] . pos.y ) >= CurLevel->ylen ) ||
       ( (int) rintf( Me [ PlayerNum ] . pos.x ) >= CurLevel->xlen ) ||
       ( (int) rintf( Me [ PlayerNum ] . pos.y ) <  0              ) ||
       ( (int) rintf( Me [ PlayerNum ] . pos.x ) <  0              ) )
    {
      fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
A Player (influencer/tux) was found outside the map in MoveInfluence.\n\
\n\
This indicates a bug or unimplemented functionality in Freedroid.\n\
The PlayerNum of the offending player on this machine was: %d.\n\
\n\
Freedroid will terminate now to draw attention \n\
to the internal problem it could not resolve.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" , PlayerNum );
      Terminate(ERR);
    }

  //--------------------
  // Now if the influencer is on some form of conveyor belt, we adjust the planned step
  // accoringly
  //
  switch ( CurLevel->map[ (int) rintf( Me [ PlayerNum ] . pos.y) ] [ (int) rintf( Me [ PlayerNum ] . pos.x ) ] )
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

  Me [ PlayerNum ] .pos.x += planned_step_x;
  Me [ PlayerNum ] .pos.y += planned_step_y;

  //--------------------
  // Check it the influ is on a special field like a lift, a console or a refresh or a conveyor belt
  //
  ActSpecialField ( PlayerNum ) ;

  AnimateInfluence ( PlayerNum ) ;	// move the "phase" of influencers rotation

  //--------------------
  // Now we check if perhaps the influencer is close to some friendly droid
  // and this friendly droid is also target of the mouse move.  Then of course
  // a chat with this droid must be initiated.
  //
  if ( ( Me [ PlayerNum ] . mouse_move_target_is_enemy != (-1) ) &&
       ( fabsf( AllEnemys [ Me [ PlayerNum ] . mouse_move_target_is_enemy ] . pos . x -
		Me [ PlayerNum ] . pos . x ) < BEST_CHAT_DISTANCE_IN_SQUARES ) &&  
       ( fabsf( AllEnemys [ Me [ PlayerNum ] . mouse_move_target_is_enemy ] . pos . y -
		Me [ PlayerNum ] . pos . y ) < BEST_CHAT_DISTANCE_IN_SQUARES ) &&  
       ( ( AllEnemys [ Me [ PlayerNum ] . mouse_move_target_is_enemy ] . pos . z -
	   Me [ PlayerNum ] . pos . z ) == 0 ) )
    {
      //--------------------
      // This whole action only makes sence for FRIENDLY droids of course!
      //
      if ( AllEnemys [ Me [ PlayerNum ] . mouse_move_target_is_enemy ] . is_friendly )
	{
	  //--------------------
	  // We chat with the friendly droid
	  ChatWithFriendlyDroid ( Me [ PlayerNum ] . mouse_move_target_is_enemy ) ;
	  
	  //--------------------
	  // and then we deactivate this mouse_move_target_is_enemy to prevent
	  // immediate recurrence of the very same chat.
	  Me [ PlayerNum ] . mouse_move_target_is_enemy = (-1) ;
	}
    }

  //--------------------
  // Finally we check if perhaps the influencer is close to some targeted enemy droid
  // and the takeover skill is activated as well.
  // Then of course, takeover process must be initiated.
  //
  if ( ( Me [ PlayerNum ] . mouse_move_target_is_enemy != (-1) ) &&
       ( fabsf( AllEnemys [ Me [ PlayerNum ] . mouse_move_target_is_enemy ] . pos . x -
		Me [ PlayerNum ] . pos . x ) < BEST_CHAT_DISTANCE_IN_SQUARES ) &&  
       ( fabsf( AllEnemys [ Me [ PlayerNum ] . mouse_move_target_is_enemy ] . pos . y -
		Me [ PlayerNum ] . pos . y ) < BEST_CHAT_DISTANCE_IN_SQUARES ) &&  
       ( ( AllEnemys [ Me [ PlayerNum ] . mouse_move_target_is_enemy ] . pos . z -
	   Me [ PlayerNum ] . pos . z ) == 0 ) &&
       ( Me [ PlayerNum ] . readied_skill == SKILL_TAKEOVER ) &&
       ( MouseRightPressed() )
       )
    {
      //--------------------
      // This whole action only makes sence for ENEMY droids of course!
      //
      if ( ! AllEnemys [ Me [ PlayerNum ] . mouse_move_target_is_enemy ] . is_friendly )
	{
	  //--------------------
	  // We chat with the friendly droid
	  Takeover ( Me [ PlayerNum ] . mouse_move_target_is_enemy ) ;
	  
	  //--------------------
	  // and then we deactivate this mouse_move_target_is_enemy to prevent
	  // immediate recurrence of the very same chat.
	  Me [ PlayerNum ] . mouse_move_target_is_enemy = (-1) ;
	}
    }


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
AnimateInfluence ( int PlayerNum )
{
#define TOTAL_SWING_TIME 0.35
#define FULL_BREATHE_TIME 3
#define TOTAL_STUNNED_TIME 0.35

  if ( Me [ PlayerNum ] .got_hit_time != (-1) )
    {
      Me [ PlayerNum ] .phase = TUX_SWING_PHASES + TUX_BREATHE_PHASES + 
	( Me [ PlayerNum ] .got_hit_time * TUX_GOT_HIT_PHASES * 1.0 / TOTAL_STUNNED_TIME ) ;
      if ( Me [ PlayerNum ] .got_hit_time > TOTAL_STUNNED_TIME ) Me [ PlayerNum ] .got_hit_time = (-1) ;
    }
  else if ( Me [ PlayerNum ] .weapon_swing_time == (-1) )
    {
      Me [ PlayerNum ] .phase = ( (int) ( Me [ PlayerNum ] .MissionTimeElapsed * TUX_BREATHE_PHASES / FULL_BREATHE_TIME ) ) % TUX_BREATHE_PHASES ;
    }
  else
    {
      Me [ PlayerNum ] .phase = ( TUX_BREATHE_PHASES + ( Me [ PlayerNum ] .weapon_swing_time * TUX_SWING_PHASES * 1.0 / TOTAL_SWING_TIME ) ) ;
      if ( Me [ PlayerNum ] .weapon_swing_time > TOTAL_SWING_TIME ) Me [ PlayerNum ] .weapon_swing_time = (-1) ;
      if (((int) (Me [ PlayerNum ] .phase)) >= TUX_SWING_PHASES + TUX_BREATHE_PHASES )
	{
	  Me [ PlayerNum ] .phase = 0;
	}

    }

  if (((int) (Me [ PlayerNum ] .phase)) >= TUX_SWING_PHASES + TUX_BREATHE_PHASES + TUX_GOT_HIT_PHASES )
    {
      Me [ PlayerNum ] .phase = 0;
    }

  /*
  Me [ PlayerNum ] .phase +=
    (Me [ PlayerNum ] .energy / ( Me [ PlayerNum ] .maxenergy)) * Frame_Time () *
    DROID_PHASES * 3;
  */

  /*
  if (Me [ PlayerNum ] .type != DRUID001)
    {
      Me [ PlayerNum ] .phase +=
	(Me [ PlayerNum ] .energy / (Druidmap[Me [ PlayerNum ] .type].maxenergy + Druidmap[DRUID001].maxenergy)) * Frame_Time () *
	DROID_PHASES * 3;
    }
  else
    {
      Me [ PlayerNum ] .phase +=
	(Me [ PlayerNum ] .energy / (Druidmap[DRUID001].maxenergy)) * Frame_Time () *
	DROID_PHASES * 3;
    }
  */

  /*
  if (((int) rintf (Me [ PlayerNum ] .phase)) >= DROID_PHASES)
    {
      Me [ PlayerNum ] .phase = 0;
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
CheckInfluenceWallCollisions ( int PlayerNum )
{
  double SX = Me [ PlayerNum ] .speed.x * Frame_Time ();
  double SY = Me [ PlayerNum ] .speed.y * Frame_Time ();
  finepoint lastpos;
  int res; 
  int NorthSouthAxisBlocked=FALSE;
  int EastWestAxisBlocked=FALSE;
  int H_Door_Sliding_Active = FALSE;
  double maxspeed;
  Level InfluencerLevel = curShip . AllLevels [ Me [ PlayerNum ] . pos . z ] ;

  //--------------------
  // First we introduce some security against later segfaults
  // due to calculation with properties of items of type -1.
  //
  if ( Me [ PlayerNum ] . status == OUT ) return;
  if ( Me [ PlayerNum ] . energy <= 0   ) return;

  maxspeed = ItemMap [ Me [ PlayerNum ] .drive_item.type ].item_drive_maxspeed ;

  lastpos.x = Me [ PlayerNum ] .pos.x - SX;
  lastpos.y = Me [ PlayerNum ] .pos.y - SY;

  res = DruidPassable ( Me [ PlayerNum ] .pos.x , Me [ PlayerNum ] .pos.y , Me [ PlayerNum ] . pos . z );

  //--------------------
  // Influence-Wall-Collision only has to be checked in case of
  // a collision of course, which is indicated by res not CENTER.
  if (res != CENTER )
    {

      //--------------------
      // At first we just check in which directions (from the last position)
      // the ways are blocked and in which directions the ways are open.
      //
      if ( ! ( ( DruidPassable ( lastpos.x , lastpos.y + maxspeed * Frame_Time() , Me [ PlayerNum ] . pos . z ) != CENTER ) ||
	       ( DruidPassable ( lastpos.x , lastpos.y - maxspeed * Frame_Time() , Me [ PlayerNum ] . pos . z ) != CENTER ) ) )
	{
	  DebugPrintf(1, "\nNorth-south-Axis seems to be free.");
	  NorthSouthAxisBlocked = FALSE;
	}
      else
	{
	  NorthSouthAxisBlocked = TRUE;
	}

      if ( ( DruidPassable(lastpos.x + maxspeed * Frame_Time() , lastpos.y , Me [ PlayerNum ] . pos . z ) == CENTER ) &&
	   ( DruidPassable(lastpos.x - maxspeed * Frame_Time() , lastpos.y , Me [ PlayerNum ] . pos . z ) == CENTER ) )
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
	  Me [ PlayerNum ] .pos.y = lastpos.y;
	  Me [ PlayerNum ] .speed.y = 0;
	  
	  // if its an open door, we also correct the east-west position, in the
	  // sense that we move thowards the middle
	  if ( ( GetMapBrick ( InfluencerLevel , Me [ PlayerNum ] .pos.x , Me [ PlayerNum ] .pos.y - 0.5  ) == H_GANZTUERE ) || 
	       ( GetMapBrick ( InfluencerLevel , Me [ PlayerNum ] .pos.x , Me [ PlayerNum ] .pos.y + 0.5  ) == H_GANZTUERE ) )
	    {
	      Me [ PlayerNum ] .pos.x += copysignf ( PUSHSPEED * Frame_Time() , ( rintf(Me [ PlayerNum ] .pos.x) - Me [ PlayerNum ] .pos.x ));
	      H_Door_Sliding_Active = TRUE;
	    }
	}

      if ( EastWestAxisBlocked )
	{
	  // EastWestCorrectionDone=TRUE;
	  if ( !H_Door_Sliding_Active ) Me [ PlayerNum ] .pos.x = lastpos.x;
	  Me [ PlayerNum ] .speed.x = 0;

	  // if its an open door, we also correct the north-south position, in the
	  // sense that we move thowards the middle
	  if ( ( GetMapBrick ( InfluencerLevel , Me [ PlayerNum ] . pos . x + 0.5 , Me [ PlayerNum ] . pos . y ) == V_GANZTUERE ) || 
	       ( GetMapBrick ( InfluencerLevel , Me [ PlayerNum ] . pos . x - 0.5 , Me [ PlayerNum ] . pos . y ) == V_GANZTUERE ) )
	    Me [ PlayerNum ] .pos.y += copysignf (PUSHSPEED * Frame_Time() , ( rintf(Me [ PlayerNum ] .pos.y) - Me [ PlayerNum ] .pos.y ));
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
	  if ( DruidPassable( Me [ PlayerNum ] . pos . x + SX , 
			      Me [ PlayerNum ] . pos . y ,
			      Me [ PlayerNum ] . pos . z ) == CENTER ) Me [ PlayerNum ] .pos.x += SX;
	  if ( DruidPassable( Me [ PlayerNum ] . pos . x      , 
			      Me [ PlayerNum ] . pos . y + SY ,
			      Me [ PlayerNum ] . pos . z ) == CENTER ) Me [ PlayerNum ] .pos.y += SY;
	}

      //--------------------
      // Here I introduce some extra security as a fallback:  Obviously
      // if the influencer is blocked FOR THE SECOND TIME, then the throw-back-algorithm
      // above HAS FAILED.  The absolutely fool-proof and secure handling is now done by
      // simply reverting to the last influ coordinated, where influ was NOT BLOCKED.
      // For this reason, a history of influ-coordinates has been introduced.  This will all
      // be done here and now:
      
      if ( ( DruidPassable ( Me [ PlayerNum ] .pos.x, Me [ PlayerNum ] .pos.y , Me [ PlayerNum ] . pos . z ) != CENTER) && 
	   ( DruidPassable ( GetInfluPositionHistoryX( 0 ) , GetInfluPositionHistoryY( 0 ) , GetInfluPositionHistoryZ( 0 ) ) != CENTER) &&
	   ( DruidPassable ( GetInfluPositionHistoryX( 1 ) , GetInfluPositionHistoryY( 1 ) , GetInfluPositionHistoryZ( 1 ) ) != CENTER) )
	{
	  Me [ PlayerNum ] .pos.x = GetInfluPositionHistoryX( 2 );
	  Me [ PlayerNum ] .pos.y = GetInfluPositionHistoryY( 2 );
	  DebugPrintf ( 0, "\nATTENTION! CheckInfluenceWallCollsision FALLBACK ACTIVATED!!");
	}

    }

} /* CheckInfluenceWallCollisions */

/* ----------------------------------------------------------------------
 * This function adapts the influencers current speed to the maximal speed
 * possible for the influencer (determined by the currely used drive type).
 * ---------------------------------------------------------------------- */
void
AdjustSpeed ( int PlayerNum )
{
  double maxspeed = ItemMap [ Me [ PlayerNum ] .drive_item.type ].item_drive_maxspeed ;

  if (Me [ PlayerNum ] .speed.x > maxspeed)
    Me [ PlayerNum ] .speed.x = maxspeed;
  if (Me [ PlayerNum ] .speed.x < (-maxspeed))
    Me [ PlayerNum ] .speed.x = (-maxspeed);

  if (Me [ PlayerNum ] .speed.y > maxspeed)
    Me [ PlayerNum ] .speed.y = maxspeed;
  if (Me [ PlayerNum ] .speed.y < (-maxspeed))
    Me [ PlayerNum ] .speed.y = (-maxspeed);

}; // void AdjustSpeed ( int PlayerNum ) 

/* ----------------------------------------------------------------------
 * This function reduces the influencers speed as long as no direction 
 * key of any form is pressed and also no mouse move target is set.
 * ---------------------------------------------------------------------- */
void
InfluenceFrictionWithAir ( int PlayerNum )
{

  if ( ! ServerThinksUpPressed ( PlayerNum ) && ! ServerThinksDownPressed ( PlayerNum ) )
    {
      Me [ PlayerNum ] . speed . y *= exp ( log ( FRICTION_CONSTANT ) * Frame_Time ( ) );
    }
  if ( ! ServerThinksRightPressed ( PlayerNum ) && ! ServerThinksLeftPressed ( PlayerNum ) )
    {
      Me [ PlayerNum ] . speed . x *= exp ( log ( FRICTION_CONSTANT ) * Frame_Time ( ) );
    }

}; // InfluenceFrictionWithAir ( int PlayerNum )

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
  for (i = 0; i < MAX_ENEMYS_ON_SHIP ; i++)
    {

      //--------------------
      // ignore enemy that are not on this level or dead 
      //
      if ( AllEnemys[i].pos.z != CurLevel->levelnum )
	continue;
      if ( AllEnemys[i].Status == OUT )
	continue;
      if ( AllEnemys[i].type == ( -1 ) )
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
	  Me[0].speed.x = -Me[0].speed.x;
	  Me[0].speed.y = -Me[0].speed.y;
	  
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
 * This function checks if there is some living droid below the current
 * mouse cursor. 
 * This function is useful for determining if a mouse-button-press was
 * meant as a mouse-indicated move instruction was given or rather a 
 * weapon swing/weapon fire command was meant by the player.
 * ---------------------------------------------------------------------- */
int 
LivingDroidBelowMouseCursor ( int PlayerNum )
{
  int i;
  float Mouse_Blocks_X, Mouse_Blocks_Y;

  Mouse_Blocks_X = ((float)ServerThinksInputAxisX ( PlayerNum )) / ((float)Block_Width  ) ;
  Mouse_Blocks_Y = ((float)ServerThinksInputAxisY ( PlayerNum )) / ((float)Block_Height ) ;

  // for (i = 0; i < MAX_ENEMYS_ON_SHIP; i++)
  for (i = 0; i < Number_Of_Droids_On_Ship; i++)
    {
      if (AllEnemys[i].Status == OUT)
	continue;
      if (AllEnemys[i].pos.z != Me[ PlayerNum ] . pos . z )
	continue;
      if ( fabsf (AllEnemys[i].pos.x - ( Me[ PlayerNum ] . pos . x + Mouse_Blocks_X ) ) >= DROID_SELECTION_TOLERANCE )
	continue;
      if ( fabsf (AllEnemys[i].pos.y - ( Me[ PlayerNum ] . pos . y + Mouse_Blocks_Y ) ) >= DROID_SELECTION_TOLERANCE )
	continue;
      


      //--------------------
      // So this must be a possible target for the next weapon swing.  Yes, there
      // is some living droid beneath the mouse cursor.
      //
      return ( TRUE );
    }

  //--------------------
  // It seems that we were unable to locate a living droid under the mouse 
  // cursor.  So we return, giving this very same message.
  //
  return ( FALSE );

}; // int LivingDroidBelowMouseCursor ( int PlayerNum )

/* ----------------------------------------------------------------------
 * This function checks if there is a crushable box below the mouse 
 * cursor or not.
 * This function is useful for determining if a mouse-button-press was
 * meant as a mouse-indicated move instruction was given or rather a 
 * weapon swing/weapon fire command was meant by the player.
 * ---------------------------------------------------------------------- */
int
CrushableBoxBelowMouseCursor ( int PlayerNum )
{
  Level SpecialFieldLevel;
  unsigned char MapBrick;
  float Mouse_Blocks_X, Mouse_Blocks_Y;

  Mouse_Blocks_X = ((float)ServerThinksInputAxisX ( PlayerNum )) / ((float)Block_Width  ) ;
  Mouse_Blocks_Y = ((float)ServerThinksInputAxisY ( PlayerNum )) / ((float)Block_Height ) ;

  //--------------------
  // Now we get the brick code at our corrent location.
  //
  SpecialFieldLevel = curShip . AllLevels [ Me [ PlayerNum ] . pos . z ] ;
  MapBrick = GetMapBrick ( SpecialFieldLevel , Me [ PlayerNum ] . pos . x + Mouse_Blocks_X , 
			   Me [ PlayerNum ] . pos . y + Mouse_Blocks_Y ) ;

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

}; // int CrushableBoxBelowMouseCursor ( int PlayerNum )


/* ----------------------------------------------------------------------
 * This function checks if there is some living droid below the current
 * mouse cursor and returns the index number of this droid in the array.
 * ---------------------------------------------------------------------- */
int 
GetLivingDroidBelowMouseCursor ( int PlayerNum )
{
  int i;
  float Mouse_Blocks_X, Mouse_Blocks_Y;
  int TargetFound = (-1);
  float DistanceFound = 1000;
  float CurrentDistance;

  Mouse_Blocks_X = (float)ServerThinksInputAxisX ( PlayerNum ) / (float)Block_Width ;
  Mouse_Blocks_Y = (float)ServerThinksInputAxisY ( PlayerNum ) / (float)Block_Height ;

  // for (i = 0; i < MAX_ENEMYS_ON_SHIP; i++)
  for (i = 0; i < Number_Of_Droids_On_Ship; i++)
    {
      if (AllEnemys[i].Status == OUT)
	continue;
      if (AllEnemys[i].pos.z != Me[ PlayerNum ] . pos . z )
	continue;
      if ( fabsf (AllEnemys[i].pos.x - ( Me[ PlayerNum ] . pos . x + Mouse_Blocks_X ) ) >= DROID_SELECTION_TOLERANCE )
	continue;
      if ( fabsf (AllEnemys[i].pos.y - ( Me[ PlayerNum ] . pos . y + Mouse_Blocks_Y ) ) >= DROID_SELECTION_TOLERANCE )
	continue;

      CurrentDistance = 
	( fabsf (AllEnemys[i].pos.x - ( Me[ PlayerNum ] . pos . x + Mouse_Blocks_X ) ) ) *
	( fabsf (AllEnemys[i].pos.x - ( Me[ PlayerNum ] . pos . x + Mouse_Blocks_X ) ) ) +
	( fabsf (AllEnemys[i].pos.y - ( Me[ PlayerNum ] . pos . y + Mouse_Blocks_Y ) ) ) *
	( fabsf (AllEnemys[i].pos.y - ( Me[ PlayerNum ] . pos . y + Mouse_Blocks_Y ) ) ) ;


      if ( CurrentDistance < DistanceFound )
	{
	  DistanceFound = CurrentDistance ;
	  TargetFound = i;
	}      

      //--------------------
      // So this must be a possible target for the next weapon swing.  Yes, there
      // is some living droid beneath the mouse cursor.
      //
      // return ( i );
    }

  //--------------------
  // It seems that we were unable to locate a living droid under the mouse 
  // cursor.  So we return, giving this very same message.
  //
  return ( TargetFound );

}; // int GetLivingDroidBelowMouseCursor ( int PlayerNum )


/* ----------------------------------------------------------------------
 * This function fires a bullet from the influencer in some direction, 
 * no matter whether this is 'allowed' or not, not questioning anything
 * and SILENTLY TRUSTING THAT THIS TUX HAS A RANGED WEAPON EQUIPPED.
 * ---------------------------------------------------------------------- */
void
FireTuxRangedWeaponRaw ( int PlayerNum , int weapon_item_type , int ForceMouseUse ) 
{
  int i = 0;
  Bullet CurBullet = NULL;  // the bullet we're currentl dealing with
  int guntype = ItemMap[ weapon_item_type ].item_gun_bullet_image_type;   // which gun do we have ? 
  double BulletSpeed = ItemMap[ weapon_item_type ].item_gun_speed;
  double speed_norm;
  moderately_finepoint speed;
  int max_val;
  float OffsetFactor;

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

  // didn't find any free bullet entry? --> take the first
  if (CurBullet == NULL)
    CurBullet = &AllBullets[0];

  CurBullet->pos.x = Me [ PlayerNum ] .pos.x;
  CurBullet->pos.y = Me [ PlayerNum ] .pos.y;
  CurBullet->pos.z = Me [ PlayerNum ] .pos.z;
  CurBullet->type = guntype;

  //--------------------
  // Previously, we had the damage done only dependant upon the weapon used.  Now
  // the damage value is taken directly from the character stats, and the UpdateAll...stats
  // has to do the right computation and updating of this value.  hehe. very conventient.
  CurBullet->damage = Me [ PlayerNum ] .base_damage + MyRandom( Me [ PlayerNum ] .damage_modifier);
  CurBullet->mine = TRUE;
  CurBullet->owner = -1;
  CurBullet->bullet_lifetime        = ItemMap[ weapon_item_type ].item_gun_bullet_lifetime;
  CurBullet->angle_change_rate      = ItemMap[ weapon_item_type ].item_gun_angle_change;
  CurBullet->fixed_offset           = ItemMap[ weapon_item_type ].item_gun_fixed_offset;
  CurBullet->ignore_wall_collisions = ItemMap[ weapon_item_type ].item_gun_bullet_ignore_wall_collisions;
  CurBullet->owner_pos = & ( Me [ PlayerNum ] .pos );
  CurBullet->time_in_frames = 0;
  CurBullet->time_in_seconds = 0;
  CurBullet->was_reflected = FALSE;
  CurBullet->reflect_other_bullets   = ItemMap[ weapon_item_type ].item_gun_bullet_reflect_other_bullets;
  CurBullet->pass_through_explosions = ItemMap[ weapon_item_type ].item_gun_bullet_pass_through_explosions;
  CurBullet->pass_through_hit_bodies = ItemMap[ weapon_item_type ].item_gun_bullet_pass_through_hit_bodies;
  CurBullet->miss_hit_influencer = UNCHECKED ;
  memset( CurBullet->total_miss_hit , UNCHECKED , MAX_ENEMYS_ON_SHIP );
  CurBullet->to_hit = Me [ PlayerNum ] .to_hit;

  Me [ PlayerNum ] . firewait = ItemMap[ weapon_item_type ].item_gun_recharging_time;

  speed.x = 0.0;
  speed.y = 0.0;

  if ( ServerThinksDownPressed ( PlayerNum ) )
    speed.y = 1.0;
  if ( ServerThinksUpPressed ( PlayerNum ) )
    speed.y = -1.0;
  if ( ServerThinksLeftPressed ( PlayerNum ) )
    speed.x = -1.0;
  if ( ServerThinksRightPressed ( PlayerNum ) )
    speed.x = 1.0;

  /* if using a joystick/mouse, allow exact directional shots! */
  if ( ServerThinksAxisIsActive ( PlayerNum ) || ForceMouseUse )
    {
      max_val = max ( abs( ServerThinksInputAxisX ( PlayerNum ) ) , 
		      abs( ServerThinksInputAxisY ( PlayerNum ) ) );
      speed.x = 1.0 * ServerThinksInputAxisX ( PlayerNum ) / max_val ;
      speed.y = 1.0 * ServerThinksInputAxisY ( PlayerNum ) / max_val ;
    }

  //--------------------
  // It might happen, that this is not a normal shot, but rather the
  // swing of a melee weapon.  Then of course, we should make a swing
  // and not start in this direction, but rather somewhat 'before' it,
  // so that the rotation will hit the target later.
  //
  RotateVectorByAngle ( & speed , ItemMap[ weapon_item_type ].item_gun_start_angle_modifier );

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
  
  CurBullet->speed.x *= BulletSpeed;
  CurBullet->speed.y *= BulletSpeed;

  //--------------------
  // To prevent influ from hitting himself with his own bullets,
  // move them a bit..
  //
  if ( CurBullet->angle_change_rate == 0 ) OffsetFactor = 0.5; else OffsetFactor = 1;
  CurBullet->pos.x += OffsetFactor * (CurBullet->speed.x/BulletSpeed);
  CurBullet->pos.y += OffsetFactor * (CurBullet->speed.y/BulletSpeed);
  // CurBullet->pos.x += 0.5 ;
  // CurBullet->pos.y += 0.5 ;

}; // void FireTuxRangedWeaponRaw ( PlayerNum ) 


/* ----------------------------------------------------------------------
 * This function fires a bullet from the influencer in some direction, or
 * at least it TRIES to fire a bullet from the influencer, cause maybe
 * the influencer can't fire for this reason or another right now...
 * ---------------------------------------------------------------------- */
void
FireBullet ( int PlayerNum )
{
  int i = 0;
  int guntype = ItemMap[ Me [ PlayerNum ] . weapon_item.type ].item_gun_bullet_image_type;   // which gun do we have ? 
  moderately_finepoint Weapon_Target_Vector;
  float angle;

  DebugPrintf ( 2 , "\n===> void FireBullet ( int PlayerNum ) : real function call confirmed. " ) ;

  // If the influencer is holding something from the inventory
  // menu via the mouse, also just return
  // if ( Item_Held_In_Hand != (-1) ) return;

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
  if ( ServerThinksAxisIsActive ( PlayerNum ) && 
       ( GameConfig.Inventory_Visible || GameConfig.CharacterScreen_Visible || GameConfig.SkillScreen_Visible ) && 
       ! CursorIsInUserRect( User_Rect.x + User_Rect.w/2 + ServerThinksInputAxisX ( PlayerNum ) , User_Rect.y + User_Rect.h/2 + ServerThinksInputAxisY ( PlayerNum ) ) )
    { 
      DebugPrintf( 0 , "\nCursor outside user-rect:\n  User_Rect.x=%d, User_Rect.w=%d, User_Rect.y=%d, User_Rect.h=%d." ,
		   User_Rect.x , User_Rect.w , User_Rect.y , User_Rect.h );
      DebugPrintf( 0 , "\nCursor position: X=%d, Y=%d." ,
		   ServerThinksInputAxisX ( PlayerNum ) , ServerThinksInputAxisY ( PlayerNum ) );
      return;
    }

  //--------------------
  // And also if the whole screen is filled with inventory or other screens, then we will
  // of course not fire any weapon or something but rather return immediately.
  //
  if ( ( GameConfig.CharacterScreen_Visible || GameConfig.SkillScreen_Visible ) && GameConfig.Inventory_Visible ) return;

  //--------------------
  // Now the new mouse move:  If there is no enemy below the mouse cursor, we interpret
  // a move to that location, not a fire command.
  //
  // Also if the target is pretty close, we interpret a fireing command.
  //
  if ( 
      ( ( ! LivingDroidBelowMouseCursor ( PlayerNum ) ) && 
	( ! CrushableBoxBelowMouseCursor ( PlayerNum ) ) && 
	( ! ServerThinksShiftWasPressed ( PlayerNum ) ) &&
	( Me [ PlayerNum ] . mouse_move_target_is_enemy == (-1) ) )
      )
    {
      //--------------------
      // Later, we will add the new mouse move intention at this point
      //
      Me [ PlayerNum ] . mouse_move_target . x = 
	Me [ PlayerNum ] . pos . x + ( (float) ServerThinksInputAxisX ( PlayerNum ) ) / (float) Block_Width ;
      Me [ PlayerNum ] . mouse_move_target . y = 
	Me [ PlayerNum ] . pos . y + ( (float) ServerThinksInputAxisY ( PlayerNum ) ) / (float) Block_Width ;
      Me [ PlayerNum ] . mouse_move_target . z = Me [ PlayerNum ] . pos . z ;

      // Me [ PlayerNum ] . mouse_move_target_is_enemy = (-1) ;

      return;

    }

  if ( ( ! CrushableBoxBelowMouseCursor ( PlayerNum ) ) && 
       ( ! LivingDroidBelowMouseCursor ( PlayerNum ) ) && 
       ( ! ServerThinksShiftWasPressed ( PlayerNum ) ) )
    {
      //--------------------
      // Later, we will add the new mouse move intention at this point
      //
      Me [ PlayerNum ] . mouse_move_target . x = 
	Me [ PlayerNum ] . pos . x + ( (float) ServerThinksInputAxisX ( PlayerNum ) ) / (float) Block_Width ;
      Me [ PlayerNum ] . mouse_move_target . y = 
	Me [ PlayerNum ] . pos . y + ( (float) ServerThinksInputAxisY ( PlayerNum ) ) / (float) Block_Width ;
      Me [ PlayerNum ] . mouse_move_target . z = Me [ PlayerNum ] . pos . z ;

      Me [ PlayerNum ] . mouse_move_target_is_enemy = (-1) ;

    }

  if ( ( CrushableBoxBelowMouseCursor ( PlayerNum ) ) && ( ! ServerThinksShiftWasPressed ( PlayerNum ) ) )
    {
      //--------------------
      // Perhaps the player is just targeting a crushable box now.
      // Then of course we must not return, but execute the stroke!!
      //
      if ( ( ( fabsf ( Me [ PlayerNum ] . mouse_move_target . x - 
		     Me [ PlayerNum ] . pos . x ) < FORCE_FIRE_DISTANCE ) &&
	     ( fabsf ( Me [ PlayerNum ] . mouse_move_target . y - 
		       Me [ PlayerNum ] . pos . y ) < FORCE_FIRE_DISTANCE  ) ) ||
	   ( ItemMap [ Me [ PlayerNum ] . weapon_item . type ] . item_gun_angle_change == 0 ) )
	{
	  // don't return, but do the attack...
	  //
	}
      else
	{
	  return;
	}
    }


  if ( ( LivingDroidBelowMouseCursor ( PlayerNum ) ) && ( ! ServerThinksShiftWasPressed ( PlayerNum ) ) ) 
    {
      //--------------------
      // We assign the target robot of the coming attack operation.
      // In case of no robot, we should get (-1), which is also serving us well.
      //
      Me [ PlayerNum ] . mouse_move_target_is_enemy = GetLivingDroidBelowMouseCursor ( PlayerNum ) ;
      
      //--------------------
      // It would be tempting to return now, but perhaps the player is just targeting and fighting a robot.
      // Then of course we must not return, but execute the stroke!!
      //
      if ( ( Me [ PlayerNum ] . mouse_move_target_is_enemy != (-1) ) &&
	   ( fabsf ( AllEnemys [ Me [ PlayerNum ] . mouse_move_target_is_enemy ] . pos . x - 
		     Me [ PlayerNum ] . pos . x ) < FORCE_FIRE_DISTANCE ) &&
	   ( fabsf ( AllEnemys [ Me [ PlayerNum ] . mouse_move_target_is_enemy ] . pos . y - 
		     Me [ PlayerNum ] . pos . y ) < FORCE_FIRE_DISTANCE  ) )
	{
	  // don't return, but do the attack...
	  //
	}
      else
	{
	  return;
	}

    }

  // If influencer hasn't recharged yet, fireing is impossible, we're done here and return
  if ( Me [ PlayerNum ] .firewait > 0 )
    return;


  //--------------------
  // We should always make the sound of a fired bullet (or weapon swing)
  // and then of course also subtract a certain fee from the remaining weapon
  // duration in the course of the swing/hit
  //
  Fire_Bullet_Sound ( guntype );
  DamageItem ( & ( Me [ PlayerNum ] . weapon_item  ) );

  //--------------------
  // We always start the weapon application cycle, i.e. change of tux
  // motion phases
  //
  Me [ PlayerNum ] . weapon_swing_time = 0;

  //--------------------
  // But if the currently used weapon is a melee weapon, the tux no longer
  // generates a bullet, but rather does his weapon swinging motion and
  // only the damage is done to the robots in the area of effect
  //
  if ( ( ItemMap [ Me [ PlayerNum ] . weapon_item . type ] . item_gun_angle_change != 0 ) ||
       ( Me [ PlayerNum ] . weapon_item . type == (-1) ) )
    {
      //--------------------
      // Since a melee weapon is swung, which may be only influencers fists,
      // we calculate where the point
      // of the weapon should be finally hitting and do some damage
      // to all the enemys in that area.
      //
      angle = - ( atan2 ( ServerThinksInputAxisY ( PlayerNum ) + 16 ,  
			  ServerThinksInputAxisX ( PlayerNum ) + 16 ) * 180 / M_PI + 90 );
      DebugPrintf( 0 , "\n===> Fire Bullet: angle=%f. " , angle ) ;
      DebugPrintf( 0 , "\n===> Fire Bullet: InpAxis: X=%d Y=%d . " , 
		   ServerThinksInputAxisX ( PlayerNum ) , 
		   ServerThinksInputAxisY ( PlayerNum ) ) ;
      Weapon_Target_Vector.x = 0 ;
      Weapon_Target_Vector.y = - 1.0 ;
      RotateVectorByAngle ( & Weapon_Target_Vector , angle );
      Weapon_Target_Vector.x += Me [ PlayerNum ] . pos . x;
      Weapon_Target_Vector.y += Me [ PlayerNum ] . pos . y;
      DebugPrintf( 0 , "\n===> Fire Bullet target: x=%f, y=%f. " , Weapon_Target_Vector.x , Weapon_Target_Vector.y ) ;
      
      for ( i = 0 ; i < Number_Of_Droids_On_Ship ; i ++ )
	{
	  if ( AllEnemys [ i ] . Status == OUT ) continue;
	  if ( AllEnemys [ i ] . pos . z != Me [ PlayerNum ] . pos . z ) continue;
	  if ( fabsf ( AllEnemys [ i ] . pos . x - Weapon_Target_Vector.x ) > 0.5 ) continue;
	  if ( fabsf ( AllEnemys [ i ] . pos . y - Weapon_Target_Vector.y ) > 0.5 ) continue;
	  AllEnemys[ i ] . energy -= Me [ PlayerNum ] .base_damage + MyRandom( Me [ PlayerNum ] .damage_modifier );
	  
	  //--------------------
	  // War tux freezes enemys with the appropriate plugin...
	  AllEnemys[ i ] . frozen += Me [ PlayerNum ] . freezing_melee_targets ; 

	  AllEnemys[ i ] . firewait = 
	    2 * ItemMap [ Druidmap [ AllEnemys [ i ] . type ] . weapon_item.type ] . item_gun_recharging_time ;
	  PlayEnemyGotHitSound ( Druidmap [ AllEnemys [ i ] . type ] . got_hit_sound_type );
	  DebugPrintf( 0 , "\n===> Fire Bullet hit something.... melee ... " ) ;
	}

      //--------------------
      // Also, we should check if there was perhaps a chest or box
      // or something that can be smashed up, cause in this case, we
      // must open pendoras box now.
      //
      Smash_Box ( Weapon_Target_Vector.x , Weapon_Target_Vector.y );
      
      //--------------------
      // Finally we add a new wait-counter, so that bullets or swings
      // cannot be started in too rapid succession.  
      // 
      // And then we can return, for real bullet generation isn't required in
      // our case here.
      //
      if ( Me [ PlayerNum ] .weapon_item.type != ( -1 ) )
	Me [ PlayerNum ] .firewait = ItemMap[ Me [ PlayerNum ] .weapon_item.type ].item_gun_recharging_time;
      else
	Me [ PlayerNum ] .firewait = 0.5;

      return;
    }

  FireTuxRangedWeaponRaw ( PlayerNum , Me [ PlayerNum ] .weapon_item.type , FALSE ) ;


  return;
}; // void FireBullet ( int PlayerNum )

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

  if ( AllEnemys[enemynum].is_friendly ) return;

  if (Me[0].type <= enemytype)
    {
      if (InvincibleMode)
	return;
      
      /* This old code used class difference to determine collision damage.
	 But now we use Weight-Difference. 
      Me[0].energy -=
	(Druidmap[enemytype].class -
	 Druidmap[Me[0].type].class) * BOUNCE_LOSE_FACT;
      */
      Me[0].energy -=
	(Druidmap[enemytype].weight -
	 Druidmap[Me[0].type].weight ) * collision_lose_energy_calibrator * 0.01 ;
    }
  else
    AllEnemys[enemynum].energy -=
      (Druidmap[Me[0].type].weight -
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
  // Me[0].health -= Druidmap[Me[0].type].lose_health * Frame_Time () * Me[0].Current_Victim_Resistance_Factor;

  /* you cant have more energy than health */
  // if (Me[0].energy > Me[0].health) Me[0].energy = Me[0].health;

} // void PermanentLoseEnergy(void)

#undef _influ_c
