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

/* ----------------------------------------------------------------------
 * The following function is NOT 'standard' C but rather a GNU extention
 * to the C standards.  We *DON'T* want to use such things, but in this
 * case it helps debugging purposes of floating point operations just so
 * much, that it's really worth using it (in development versions, not in
 * releases).  But to avoid warnings from GCC (which we always set to not
 * allow gnu extentions to the C standard by default), we declare the
 * prototype of this function here.  If you don't use GCC or this 
 * function should give you portability problems, it's ABSOLUTELY SAFE
 * to just remove all instances of it, since it really only helps 
 * debugging.  Proper documentation can be found in the GNU C Library,
 * section about 'Arithmethic', subsection on floating point control
 * functions.
 * ---------------------------------------------------------------------- */
int feenableexcept (int excepts);

/* -----------------------------------------------------------------
 * This function is the heart of the game.  It contains the main
 * game loop.
 * ----------------------------------------------------------------- */
int
main (int argc, char * argv[])
{
    int i;
    int PlayerNum;
    
    //--------------------
    // First we issue some message, that should appear in the debug log for
    // windows users.
    //
#ifdef __WIN32__
    fprintf ( stderr , "\n\
Hello!  This window contains the DEBUG OUTPUT of FreedroidRPG\n\
\n\
Normally you would not see this message or this window, but apparently\n\
FreedroidRPG has terminated because of an error of some sort.\n\
\n\
You might wish to inspect the debug output below.  Maybe sending the\n\
debug output (or at least the lower end of the debug output) to the\n\
FreedroidRPG developers could help them to track down the problem.\n\
\n\
Well, it's no guarantee that we can solve any bug, but it's certainly\n\
better than nothing.  Thanks anyway for you interest in FreedroidRPG.\n\
\n\n\n\
--start of real debug log--\n\n" );
#endif
    
    DebugPrintf ( 1 , "\nNumber of obstacles: %d." , NUMBER_OF_OBSTACLE_TYPES ) ;
    DebugPrintf ( 1 , "\nNumber of ISO_V_BATHTUB : %d." , ISO_V_BATHTUB ) ;
    DebugPrintf ( 1 , "\nNumber of ISO_ROOM_WALL_V_RED : %d." , ISO_ROOM_WALL_V_RED ) ;
    DebugPrintf ( 1 , "\nNumber of  ISO_OUTER_DOOR_V_00 : %d." , ISO_OUTER_DOOR_V_00 ) ;
    
    GameOver = FALSE;
    QuitProgram = FALSE;
    
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
		for ( i = 0 ; i < MAX_PLAYERS ; i ++ ) MoveInfluence ( i );	

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

/* ----------------------------------------------------------------------
 * Some bots might be frozen and some might be poisoned, some might still 
 * have a 'firewait' or a normal wait or a paralysation.  Other bots have
 * a text, that is being displayed and that will timeout at some point.
 *
 * In any case those counteres must be updated, which is what this 
 * function is supposed to do.
 *
 * NOTE:  This whole updating business is a bit in-efficient.  It might 
 *        be better to use some sort of 'game_time' for this and then
 *        not use 'duration left' but rather 'end time' for all these 
 *        poison, paralysation, etc. effects.  That way, we would be able
 *        be skip this whole counter advancement here...
 *
 *        Maybe later it will finally be implemented this way...
 *
 * ---------------------------------------------------------------------- */
void
update_timeouts_for_bots_on_level ( int level_num , float latest_frame_time ) 
{
    int i;
    enemy* this_bot;

    occasionally_update_first_and_last_bot_indices ( );

    for ( i  = first_index_of_bot_on_level [ level_num ] ; 
	  i <=  last_index_of_bot_on_level [ level_num ] ; i++ )
    {
	this_bot = & ( AllEnemys [ i ] );
	
	if ( this_bot -> Status == OUT ) continue;
	
	if ( this_bot -> warten > 0 ) 
	{
	    this_bot -> warten -= latest_frame_time ;
	    if ( this_bot -> warten < 0 ) this_bot -> warten = 0;
	}
	
	if ( this_bot -> frozen > 0 ) 
	{
	    this_bot -> frozen -= latest_frame_time ;
	    if ( this_bot -> frozen < 0 ) this_bot -> frozen = 0;
	}
	
	if ( this_bot -> poison_duration_left > 0 ) 
	{
	    this_bot -> poison_duration_left -= latest_frame_time ;
	    if ( this_bot -> poison_duration_left < 0 ) this_bot -> poison_duration_left = 0 ;
	    this_bot -> energy -= latest_frame_time * this_bot -> poison_damage_per_sec ;
	}
	
	if ( this_bot -> paralysation_duration_left > 0 ) 
	{
	    this_bot -> paralysation_duration_left -= latest_frame_time ;
	    if ( this_bot -> paralysation_duration_left < 0 ) this_bot -> paralysation_duration_left = 0 ;
	    // this_bot -> energy -= latest_frame_time * this_bot -> paralysation_damage_per_sec ;
	}
	
	if ( this_bot -> firewait > 0 ) 
	{
	    this_bot -> firewait -= latest_frame_time ;
	    if ( this_bot -> firewait <= 0 ) this_bot -> firewait = 0 ;
	}
	
	this_bot -> TextVisibleTime += latest_frame_time;
    } 
    
}; // void update_timeouts_for_bots_on_level ( int level_num ) 

/* -----------------------------------------------------------------
 * This function updates counters and is called ONCE every frame.
 * The counters include timers, but framerate-independence of game speed
 * is preserved because everything is weighted with the Frame_Time()
 * function.
 * ----------------------------------------------------------------- */
void
UpdateCountersForThisFrame ( int player_num )
{
    static long Overall_Frames_Displayed=0;
    int i;
    Level item_level = curShip . AllLevels [ Me [ 0 ] . pos . z ] ;
    float my_speed ;
    float latest_frame_time = Frame_Time();

    //--------------------
    // First we do all the updated, that need to be done only once
    // indepentent of the player number...  These are typically some
    // things, that the client can do without any info from the 
    // server.
    //
    if ( player_num == 0 )
    {
	GameConfig . Mission_Log_Visible_Time += latest_frame_time;
	GameConfig . Inventory_Visible_Time += latest_frame_time;
	
	// The next couter counts the frames displayed by freedroid during this
	// whole run!!  DO NOT RESET THIS COUNTER WHEN THE GAME RESTARTS!!
	Overall_Frames_Displayed++;
	Overall_Average = ( Overall_Average * ( Overall_Frames_Displayed - 1 )
			   + latest_frame_time ) / Overall_Frames_Displayed ;
	
	// Here are some things, that were previously done by some periodic */
	// interrupt function
	ThisMessageTime++;
	
	LastGotIntoBlastSound += latest_frame_time ;
	LastRefreshSound += latest_frame_time ;
	
	LevelDoorsNotMovedTime += latest_frame_time;
	LevelGunsNotFiredTime += latest_frame_time;
	if ( SkipAFewFrames ) SkipAFewFrames--;

	//--------------------
	// This is the timeout, that the tux should not start a movement
	// some fraction of a second after an item drop.
	//
	if ( timeout_from_item_drop > 0 )
	{
	    timeout_from_item_drop -= latest_frame_time ;
	    if ( timeout_from_item_drop < 0 ) timeout_from_item_drop = 0 ; 
	}

	
	for ( i = 0 ; i < MAXBULLETS ; i ++ )
	{
	    if ( AllBullets [ i ] . time_to_hide_still > 0 )
	    {
		AllBullets [ i ] . time_to_hide_still -= latest_frame_time;
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
		item_level -> ItemList [ i ] . throw_time += latest_frame_time;
	    if ( item_level -> ItemList [ i ] . throw_time > ( M_PI / 3.0 ) ) 
		item_level -> ItemList [ i ] . throw_time = 0 ;
	}

	//--------------------
	// Some bots might be frozen and some might be poisoned, some
	// might still have a 'firewait' or a normal wait or a paralysation.
	// In any case those counteres must be updated, but we'll only to 
	// that for the Tux current level (at present).
	//
	update_timeouts_for_bots_on_level ( Me [ 0 ] . pos . z , latest_frame_time ) ;

    }; // things that need to be done only once per program, not per player
    
    //--------------------
    // Now we do all the things, that need to be updated for each connected
    // player separatedly.
    //
    Me [ player_num ] . FramesOnThisLevel++;

    Me [ player_num ] . LastCrysoundTime += latest_frame_time ;
    Me [ player_num ] . MissionTimeElapsed += latest_frame_time;
    Me [ player_num ] . LastTransferSoundTime += latest_frame_time;
    Me [ player_num ] . TextVisibleTime += latest_frame_time;
    
    //--------------------
    // We take care of the running stamina...
    //
    my_speed = sqrt ( Me [ player_num ] . speed . x * Me [ player_num ] . speed . x +
		      Me [ player_num ] . speed . y * Me [ player_num ] . speed . y ) ;
    if ( my_speed >= ( TUX_WALKING_SPEED + TUX_RUNNING_SPEED ) * 0.5 )
    {
	Me [ player_num ] . running_power -= latest_frame_time * 3.0 ;
    }
    else
    {
	Me [ player_num ] . running_power += latest_frame_time * 3.0 ;
	if ( Me [ player_num ] . running_power > Me [ player_num ] . max_running_power )
	    Me [ player_num ] . running_power = Me [ player_num ] . max_running_power ;

	if ( Me [ player_num ] . running_power >= 20 )
	    Me [ player_num ] . running_must_rest = FALSE ;
    }
    

    if ( Me [ player_num ] . weapon_swing_time != (-1) ) Me [ player_num ] . weapon_swing_time += latest_frame_time;
    if ( Me [ player_num ] . got_hit_time != (-1) ) Me [ player_num ] . got_hit_time += latest_frame_time;
    
    if ( Me [ player_num ] . firewait > 0 )
    {
	Me [ player_num ] . firewait -= latest_frame_time ;
	if ( Me [ player_num ] . firewait < 0 ) Me [ player_num ] . firewait = 0 ;
    }
    // DebugPrintf ( -1000 , "\nfirewait; %f." , Me [ 0 ] . firewait );
    
    //--------------------
    // In order to know when a level can finally be respawned with
    // enemies, we keep track to the time spent actually in the game, i.e.
    // time actually spent passing frames...
    //
    for ( i = 0 ; i < MAX_LEVELS ; i ++ )
    {
	if ( Me [ player_num ] . pos . z != i )
	{
	    if ( Me [ player_num ] . time_since_last_visit_or_respawn [ i ] > (-1) )
	    {
		Me [ player_num ] . time_since_last_visit_or_respawn [ i ] += latest_frame_time ;
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
