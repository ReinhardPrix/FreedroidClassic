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
 * Desc: the main program
 * ---------------------------------------------------------------------- */
/*
 * This file has been checked for remains of german comments in the code
 * I you still find some, please just kill it mercilessly.
 */

#define _main_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"
#include "text.h"
#include "vars.h"
#include "ship.h"

int ThisMessageTime;
float LastGotIntoBlastSound = 2;
float LastRefreshSound = 2;

void UpdateCountersForThisFrame ( int PlayerNum ) ;

/* -----------------------------------------------------------------
 * This function is the heart of the game.  It contains the main
 * game loop.
 * ----------------------------------------------------------------- */
int
main (int argc, char *const argv[])
{
  int i;
  int PlayerNum;

  GameOver = FALSE;
  QuitProgram = FALSE;

  /*
   *  Parse command line and set global switches 
   *  this function exits program when error, so we don't need to 
   *  check its success  (dunno if that's good design?)
   */
  sound_on = TRUE;	 /* default value, can be overridden by command-line */
  debug_level = 0;       /* 0=no debug 1=first debug level (at the moment=all) */
  fullscreen_on = TRUE; /* use X11-window or full screen */
  joy_sensitivity = 1;
  mouse_control = TRUE;
  classic_user_rect = FALSE;

  ParseCommandLine (argc, argv); 

  InitFreedroid ();   // Initialisation of global variables and arrays

  while (!QuitProgram)
    {

      StartupMenu ( );
      // MissionSelectMenu ( );
      // InitNewMission ( STANDARD_MISSION );
      // InitNewMission ( NEW_MISSION );

      GameOver = FALSE;

      while ( (!GameOver && !QuitProgram) || ServerMode )
	{

	  CurLevel = curShip.AllLevels [ Me [ 0 ] . pos . z ];

	  if ( ServerMode ) AcceptConnectionsFromClients ( ) ;

	  if ( ServerMode ) ListenToAllRemoteClients ( ) ; 

	  if ( ClientMode ) ListenForServerMessages ( ) ;

	  if ( ServerMode ) SendPeriodicServerMessagesToAllClients (  ) ;

	  StartTakingTimeForFPSCalculation(); 

	  for ( PlayerNum = 0 ; PlayerNum < MAX_PLAYERS ; PlayerNum ++ ) 
	    UpdateCountersForThisFrame ( PlayerNum ) ;

	  CollectAutomapData (); // this is a pure client issue.  Only do it for the main player...

	  ReactToSpecialKeys();

	  for ( PlayerNum = 0 ; PlayerNum < MAX_PLAYERS ; PlayerNum ++ ) 
	      MoveLevelDoors ( PlayerNum ) ; // this is also a pure client issue, but done for all players...

	  for ( PlayerNum = 0 ; PlayerNum < MAX_PLAYERS ; PlayerNum ++ ) 
	    WorkLevelGuns( PlayerNum ); // this should fire all autocannons on this level

	  for ( PlayerNum = 0 ; PlayerNum < MAX_PLAYERS ; PlayerNum ++ ) 
	    CheckForTriggeredEventsAndStatements ( PlayerNum ) ;

	  AnimateRefresh (); // this is a pure client issue.  Not dependent upon the players.

	  AnimateConsumer (); // this is a pure client issue.  Not dependent upon the players.

	  AnimateTeleports (); // this is a pure client issue.  Not dependent upon the players.

	  ExplodeBlasts ();	// move blasts to the right current "phase" of the blast

	  MoveActiveSpells (); // move moving spells currently active...

	  //--------------------
	  // Now, for multiplayer, we update all player images...
	  //
	  for ( PlayerNum = 0 ; PlayerNum < MAX_PLAYERS ; PlayerNum ++ )
	    {
	      // Update_Tux_Working_Copy ( PlayerNum ); // do this for player Nr. 
	      Homemade_Update_Tux_Working_Copy ( PlayerNum ); // do this for player Nr. 
	    }

	  // AssembleCombatPicture ( DO_SCREEN_UPDATE ); 
	  AssembleCombatPicture ( 0 ); 

	  if ( !ClientMode ) MoveBullets ();   // please leave this in front of graphics output, so that time_in_frames always starts with 1

	  DisplayBanner (NULL, NULL,  0 ); // this is a pure client issue

	  SDL_Flip ( Screen );

	  for (i = 0; i < MAXBULLETS; i++) CheckBulletCollisions (i);

	  if ( ! ClientMode )
	    for ( i = 0 ; i < MAX_PLAYERS ; i ++ ) MoveInfluence ( i );	// change Influ-speed depending on keys pressed, but
	                        // also change his status and position and "phase" of rotation

	  UpdateAllCharacterStats ( 0 );

	  // Move_Influencers_Friends (); // Transport followers to next level

	  if ( ! ClientMode ) MoveEnemys ();	// move all the enemys:
	                        // also do attacks on influ and also move "phase" or their rotation

	  for ( i = 0 ; i < MAX_PLAYERS ; i ++ ) CheckInfluenceWallCollisions ( i );	// test if influs way is blocked by walls...

	  CheckInfluenceEnemyCollision ();

	  CheckForJumpThresholds( 0  ); // maybe the Tux is so close to the border of one map, that
	                                // he should be put into the next one already, to link them smoothly


	  /*
	  if (CurLevel->empty == 2)
	    {
	      LevelGrauFaerben ();
	      CurLevel->empty = TRUE;
	    }		    
	  */

	  CheckIfMissionIsComplete (); 

	  ComputeFPSForThisFrame();

	} /* while !GameOver */
    } /* while !QuitProgram */
  Terminate (0);
  return (0);
}; // void main ( void )

/* -----------------------------------------------------------------
 * This function updates counters and is called ONCE every frame.
 * The counters include timers, but framerate-independence of game speed
 * is preserved because everything is weighted with the Frame_Time()
 * function.
 * ----------------------------------------------------------------- */
void
UpdateCountersForThisFrame ( int PlayerNum )
{
  static long Overall_Frames_Displayed=0;
  int i;
  Level UpdateLevel = curShip.AllLevels [ Me [ PlayerNum ] . pos . z ] ;

  //--------------------
  // First we do all the updated, that need to be done only once
  // indepentent of the player number...  These are typically some
  // things, that the client can do without any info from the 
  // server.
  //
  if ( PlayerNum == 0 )
    {
      GameConfig.Mission_Log_Visible_Time += Frame_Time();
      GameConfig.Inventory_Visible_Time += Frame_Time();
      // if (ShipEmptyCounter == 1) GameOver = TRUE;
      LastBlastHit++;
      Total_Frames_Passed_In_Mission++;

      // The next couter counts the frames displayed by freedroid during this
      // whole run!!  DO NOT RESET THIS COUNTER WHEN THE GAME RESTARTS!!
      Overall_Frames_Displayed++;
      Overall_Average = (Overall_Average*(Overall_Frames_Displayed-1)
			 + Frame_Time()) / Overall_Frames_Displayed;

      // Here are some things, that were previously done by some periodic */
      // interrupt function
      ThisMessageTime++;

      LastGotIntoBlastSound += Frame_Time ();
      LastRefreshSound += Frame_Time ();

      LevelDoorsNotMovedTime += Frame_Time();
      LevelGunsNotFiredTime += Frame_Time();
      if ( SkipAFewFrames ) SkipAFewFrames--;

      if (ShipEmptyCounter > 1)
	ShipEmptyCounter--;
      if ( UpdateLevel->empty > 2)
	UpdateLevel->empty--;

      for (i = 0; i < MAX_ENEMYS_ON_SHIP ; i++)
	{
	  
	  if (AllEnemys[i].Status == OUT ) continue;
	  
	  if (AllEnemys[i].warten > 0) 
	    {
	      AllEnemys[i].warten -= Frame_Time() ;
	      if (AllEnemys[i].warten < 0) AllEnemys[i].warten = 0;
	    }
	  
	  if (AllEnemys[i].frozen > 0) 
	    {
	      AllEnemys[i].frozen -= Frame_Time() ;
	      if (AllEnemys[i].frozen < 0) AllEnemys[i].frozen = 0;
	    }
	  
	  if (AllEnemys[i].poison_duration_left > 0) 
	    {
	      AllEnemys[i].poison_duration_left -= Frame_Time() ;
	      if (AllEnemys[i].poison_duration_left < 0) AllEnemys[i].poison_duration_left = 0;
	      AllEnemys[i].energy -= Frame_Time() * AllEnemys[i].poison_damage_per_sec ;
	    }
	  
	  if (AllEnemys[i].paralysation_duration_left > 0) 
	    {
	      AllEnemys[i].paralysation_duration_left -= Frame_Time() ;
	      if (AllEnemys[i].paralysation_duration_left < 0) AllEnemys[i].paralysation_duration_left = 0;
	      // AllEnemys[i].energy -= Frame_Time() * AllEnemys[i].paralysation_damage_per_sec ;
	    }
	  
	  if (AllEnemys[i].firewait > 0) 
	    {
	      AllEnemys[i].firewait -= Frame_Time() ;
	      if (AllEnemys[i].firewait <= 0) AllEnemys[i].firewait=0;
	    }
	  
	  AllEnemys[i].TextVisibleTime += Frame_Time();
	} // for (i=0;...

    }; // things that need to be done only once per program, not per player

  //--------------------
  // Now we do all the things, that need to be updated for each connected
  // player separatedly.
  //
  Me [ PlayerNum ] .FramesOnThisLevel++;

  Me [ PlayerNum ] .LastCrysoundTime += Frame_Time ();
  Me [ PlayerNum ] .MissionTimeElapsed += Frame_Time();
  Me [ PlayerNum ] .LastTransferSoundTime += Frame_Time();
  Me [ PlayerNum ] .TextVisibleTime += Frame_Time();

  if ( Me [ PlayerNum ] .weapon_swing_time != (-1) ) Me [ PlayerNum ] .weapon_swing_time += Frame_Time();
  if ( Me [ PlayerNum ] .got_hit_time != (-1) ) Me [ PlayerNum ] .got_hit_time += Frame_Time();

  if ( Me [ PlayerNum ] .firewait > 0 )
    {
      Me [ PlayerNum ] .firewait-=Frame_Time();
      if (Me [ PlayerNum ] .firewait < 0) Me [ PlayerNum ] .firewait=0;
    }

  if (Me [ PlayerNum ] .Experience > ShowScore)
    ShowScore++;
  if (Me [ PlayerNum ] .Experience < ShowScore)
    ShowScore--;

} /* UpdateCountersForThisFrame() */


#undef _main_c
