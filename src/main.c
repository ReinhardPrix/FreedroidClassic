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
 * Desc: the main program
 * ---------------------------------------------------------------------- */

#define _main_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"
#include "vars.h"
#include "ship.h"
#include "fenv.h"

int ThisMessageTime;
float LastGotIntoBlastSound = 2;
float LastRefreshSound = 2;

void UpdateCountersForThisFrame ( int PlayerNum ) ;

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
DoAllMovementAndAnimations ( void )
{
  int PlayerNum;

  for ( PlayerNum = 0 ; PlayerNum < MAX_PLAYERS ; PlayerNum ++ ) 
    MoveLevelDoors ( PlayerNum ) ; // this is also a pure client issue, but done for all players...
  
  for ( PlayerNum = 0 ; PlayerNum < MAX_PLAYERS ; PlayerNum ++ ) 
    WorkLevelGuns( PlayerNum ); // this should fire all autocannons on this level
  
  for ( PlayerNum = 0 ; PlayerNum < MAX_PLAYERS ; PlayerNum ++ ) 
    CheckForTriggeredEventsAndStatements ( PlayerNum ) ;
  
  AnimateCyclingMapTiles (); // this is a pure client issue.  Not dependent upon the players.
  
  animate_blasts ();	// move blasts to the right current "phase" of the blast
  
  MoveActiveSpells (); // move moving spells currently active...
  
  if ( !ClientMode ) MoveBullets ();   
  
}; // void DoAllMovementAndAnimations ( void )
  
/* -----------------------------------------------------------------
 * This function is the heart of the game.  It contains the main
 * game loop.
 * ----------------------------------------------------------------- */
int
main (int argc, char * argv[])
{
  int i;
  int PlayerNum;
  float test_float_1, test_float_2, test_float_3;

  //--------------------
  // WARNING!  We're using a GNU extension of standard (ANSI?) C here.  That
  //           means the following 'feenableexcept' might not be completely
  //           portable (and also cause a warning about missing prototype at
  //           compile time).  However, this is not severe.  The feenableexcept
  //           just helps for debugging the code.  Feel free to slimply cut away
  //           these instructions in case they give you any porting problems...
  //--------------------
  // 
  // feenableexcept ( FE_ALL_EXCEPT );
  // feenableexcept ( FE_INEXACT ) ;
  feenableexcept ( FE_DIVBYZERO ) ;
  // feenableexcept ( FE_UNDERFLOW ) ;
  // feenableexcept ( FE_OVERFLOW ) ;
  feenableexcept ( FE_INVALID ) ;

  /*
  if ( feraiseexcept ( FE_ALL_EXCEPT ) != 0 )
    {
      DebugPrintf ( -100 , "\nCouldn't set floating point exceptions to be raised...\nTerminating..." );
      exit ( 0 );
    }
  else
    {
      DebugPrintf ( -100 , "\nFloating point exceptions to be raised set successfully!\n" );
    }
  */

  GameOver = FALSE;
  QuitProgram = FALSE;


  test_float_1 = 3.1 ;
  test_float_2 = 0.0 ; 
  test_float_3 = test_float_1 / test_float_2 ;

  /*
   *  Parse command line and set global switches 
   *  this function exits program when error, so we don't need to 
   *  check its success  (dunno if that's good design?)
   */
  sound_on = TRUE;	 // default value, can be overridden by command-line 
  use_open_gl = TRUE;	 // default value, can be overridden by command-line 
  debug_level = -1;      // -1: shut up all debug ... 0=no debug 1=first debug level (at the moment=all) 
  fullscreen_on = TRUE;  // use X11-window or full screen 
  joy_sensitivity = 1;
  mouse_control = TRUE;
  classic_user_rect = FALSE;

  ParseCommandLine (argc, argv); 

  InitFreedroid ();   // Initialisation of global variables and arrays

  while (!QuitProgram)
    {

      StartupMenu ( );
      GameOver = FALSE;

      while ( (!GameOver && !QuitProgram) || ServerMode )
	{
	  CurLevel = curShip.AllLevels [ Me [ 0 ] . pos . z ];

	  if ( ServerMode ) AcceptConnectionsFromClients ( ) ;

	  if ( ServerMode ) ListenToAllRemoteClients ( ) ; 

	  if ( ClientMode ) ListenForServerMessages ( ) ;

	  if ( ServerMode ) SendPeriodicServerMessagesToAllClients (  ) ;

	  StartTakingTimeForFPSCalculation(); 

	  ReactToSpecialKeys();

	  for ( PlayerNum = 0 ; PlayerNum < MAX_PLAYERS ; PlayerNum ++ ) 
	    UpdateCountersForThisFrame ( PlayerNum ) ;

	  CollectAutomapData (); // this is a pure client issue.  Only do it for the main player...

	  DoAllMovementAndAnimations();

	  if ( ! GameOver ) 
	    {
	      AssembleCombatPicture ( SHOW_ITEMS ); 
	      our_SDL_flip_wrapper ( Screen );
	    }

	  //--------------------
	  // This will now be done inside the move bullets function, because there
	  // might be need to check for collisions on several positions whenever a
	  // bullet is moving very fast...
	  //
	  // for (i = 0; i < MAXBULLETS; i++) CheckBulletCollisions (i);

	  if ( ! ClientMode )
	    for ( i = 0 ; i < MAX_PLAYERS ; i ++ ) MoveInfluence ( i );	// change Influ-speed depending on keys pressed, but
	                        // also change his status and position and "phase" of rotation

	  UpdateAllCharacterStats ( 0 );

	  // Move_Influencers_Friends (); // Transport followers to next level

	  if ( ! ClientMode ) MoveEnemys ();	// move all the enemys:
	                        // also do attacks on influ and also move "phase" or their rotation

	  CheckInfluenceEnemyCollision ();

	  CheckForJumpThresholds( 0  ); // maybe the Tux is so close to the border of one map, that
	                                // he should be put into the next one already, to link them smoothly

	  CheckIfMissionIsComplete (); 

	  if ( ! GameConfig.hog_CPU ) SDL_Delay (1); // we allow the CPU to also do something else..

	  ComputeFPSForThisFrame();

	} // while !GameOver 
    } // while !QuitProgram 
  Terminate (0);
  return (0);
}; // int main ( void )

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
  Level item_level = curShip . AllLevels [ Me [ 0 ] . pos . z ] ;

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

      //--------------------
      // This is the timeout, that the tux should not start a movement
      // some fraction of a second after an item drop.
      //
      if ( timeout_from_item_drop > 0 )
	{
	  timeout_from_item_drop -= Frame_Time ();
	  if ( timeout_from_item_drop < 0 ) timeout_from_item_drop = 0 ; 
	}


      for ( i = 0 ; i < MAXBULLETS ; i ++ )
	{
	  if ( AllBullets [ i ] . time_to_hide_still > 0 )
	    {
	      AllBullets [ i ] . time_to_hide_still -= Frame_Time();
	      if ( AllBullets [ i ] . time_to_hide_still < 0 )
		AllBullets [ i ] . time_to_hide_still = 0 ; 
	    }
	}

      //--------------------
      // Maybe some items are just thrown in the air and still in the air.
      // We need to keep track of the time the item has spent in the air so far.
      //
      for ( i = 0 ; i < MAX_ITEMS_PER_LEVEL ; i ++ )
	{
	  if ( item_level -> ItemList [ i ] . type == ( -1 ) ) continue;
	  if ( item_level -> ItemList [ i ] . throw_time > 0 ) 
	    item_level -> ItemList [ i ] . throw_time += Frame_Time();
	  if ( item_level -> ItemList [ i ] . throw_time > ( M_PI / 3.0 ) ) 
	    item_level -> ItemList [ i ] . throw_time = 0 ;
	}

      for ( i = 0 ; i < MAX_ENEMYS_ON_SHIP ; i++ )
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
  Me [ PlayerNum ] . FramesOnThisLevel++;

  Me [ PlayerNum ] . LastCrysoundTime += Frame_Time ();
  Me [ PlayerNum ] . MissionTimeElapsed += Frame_Time();
  Me [ PlayerNum ] . LastTransferSoundTime += Frame_Time();
  Me [ PlayerNum ] . TextVisibleTime += Frame_Time();

  if ( Me [ PlayerNum ] . weapon_swing_time != (-1) ) Me [ PlayerNum ] . weapon_swing_time += Frame_Time();
  if ( Me [ PlayerNum ] . got_hit_time != (-1) ) Me [ PlayerNum ] . got_hit_time += Frame_Time();

  if ( Me [ PlayerNum ] . firewait > 0 )
    {
      Me [ PlayerNum ] . firewait -= Frame_Time ( ) ;
      if ( Me [ PlayerNum ] . firewait < 0 ) Me [ PlayerNum ] . firewait = 0 ;
    }
  // DebugPrintf ( -1000 , "\nfirewait; %f." , Me [ 0 ] . firewait );

  //--------------------
  // In order to know when a level can finally be respawned with
  // enemies, we keep track to the time spent actually in the game, i.e.
  // time actually spent passing frames...
  //
  for ( i = 0 ; i < MAX_LEVELS ; i ++ )
    {
      if ( Me [ PlayerNum ] . pos . z != i )
	{
	  if ( Me [ PlayerNum ] . time_since_last_visit_or_respawn [ i ] > (-1) )
	    {
	      Me [ PlayerNum ] . time_since_last_visit_or_respawn [ i ] += Frame_Time() ;
	    }

	  //--------------------
	  // Now maybe it's time to respawn?  If we really have multiple
	  // players of course, this check would have to look a bit different...
	  //
	  if ( Me [ 0 ] . time_since_last_visit_or_respawn [ i ] > 600 )
	    {
	      DebugPrintf ( -10 , "\nNow respawning all bots on level : %d. " , i ) ;
	      Me [ 0 ] . time_since_last_visit_or_respawn [ i ] = 0 ;
	      respawn_level ( i ) ;
	    }
	  
	}
      else
	{
	  //--------------------
	  // When the Tux is right on this level, there is absolutely no need 
	  // for respawning anything...
	  //
	  Me [ 0 ] . time_since_last_visit_or_respawn [ i ] = 0 ;
	}
    }

}; // void UpdateCountersForThisFrame(...) 


#undef _main_c
