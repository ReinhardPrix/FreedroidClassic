/*----------------------------------------------------------------------
 *
 * Desc: the main program
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
#define _main_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"
#include "text.h"
#include "vars.h"
#include "ship.h"
#include "map.h"

#define CURSOR_KEEP_VISIBLE  3000   // ticks to keep mouse-cursor visible without mouse-input


int ThisMessageTime;
float LastGotIntoBlastSound = 2;
float LastRefreshSound = 2;

void UpdateCountersForThisFrame (void);

/*-----------------------------------------------------------------
 * @Desc: the heart of the Game 
 *
 * @Ret: void
 *
 *-----------------------------------------------------------------*/
int
main (int argc, char *const argv[])
{
  int i;
  Uint32 now;
  bool first_time = TRUE;

  GameOver = FALSE;
  QuitProgram = FALSE;

  sound_on = TRUE;	 /* default value, can be overridden by command-line */
  debug_level = 0;       /* 0=no debug 1=first debug level (at the moment=all) */
  fullscreen_on = TRUE; /* use X11-window or full screen */
  joy_sensitivity = 1;
  mouse_control = TRUE;
  /*
   *  Parse command line and set global switches 
   *  this function exits program when error, so we don't need to 
   *  check its success  (dunno if that's good design?)
   */
  parse_command_line (argc, argv);


  now = SDL_GetTicks();
  InitFreedroid ();   // Initialisation of global variables and arrays

  while (!QuitProgram)
    {
      InitNewMission ( STANDARD_MISSION );
      if (first_time)   // make sure title is displayed long enough on fast machines.
	{
	  while (!SpacePressed() && (SDL_GetTicks() - now < SHOW_WAIT)) ; 
	  first_time = FALSE;
	}

      show_droid_info (Me.type, -3);  // show unit-intro page
      ClearGraphMem();
      DisplayBanner (NULL, NULL,  BANNER_NO_SDL_UPDATE | BANNER_FORCE_UPDATE );
      now=SDL_GetTicks();
      while (  (SDL_GetTicks() - now < SHOW_WAIT) && (!SpacePressed()) );

      GameOver = FALSE;

      while (!GameOver && !QuitProgram)
	{

	  StartTakingTimeForFPSCalculation(); 

	  if (SDL_GetTicks () - last_mouse_event > CURSOR_KEEP_VISIBLE)
	    SDL_ShowCursor (SDL_DISABLE);
	  else
	    SDL_ShowCursor (SDL_ENABLE);

	  UpdateCountersForThisFrame ();

	  ReactToSpecialKeys();

	  MoveLevelDoors ();	

	  CheckForTriggeredEvents();

	  AnimateRefresh ();	

	  AnimateTeleports ();	

	  ExplodeBlasts ();	// move blasts to the right current "phase" of the blast

	  DisplayBanner (NULL, NULL,  0 );

	  MoveBullets ();   // please leave this in front of graphics output, so that time_in_frames always starts with 1

	  Assemble_Combat_Picture ( DO_SCREEN_UPDATE ); 

	  for (i = 0; i < MAXBULLETS; i++) CheckBulletCollisions (i);

	  MoveInfluence ();	// change Influ-speed depending on keys pressed, but
	                        // also change his status and position and "phase" of rotation

	  // Move_Influencers_Friends (); // Transport followers to next level

	  MoveEnemys ();	// move all the enemys:
	                        // also do attacks on influ and also move "phase" or their rotation

	  CheckInfluenceWallCollisions ();	/* Testen ob der Weg nicht durch Mauern verstellt ist */

	  CheckInfluenceEnemyCollision ();

	  if (CurLevel->empty == 2)
	    {
	      CurLevel->color = PD_DARK;
	      CurLevel->empty = TRUE;
	      Switch_Background_Music_To (BYCOLOR);  // start new background music
	    }			/* if */

	  CheckIfMissionIsComplete ();

	  ComputeFPSForThisFrame();

	} /* while !GameOver */
    } /* while !QuitProgram */
  Terminate (0);
  return (0);
}				// void main(void)

/*-----------------------------------------------------------------
@Desc: This function updates counters and is called ONCE every frame.
The counters include timers, but framerate-independence of game speed
is preserved because everything is weighted with the Frame_Time()
function.

@Ret: none
 *-----------------------------------------------------------------*/
void
UpdateCountersForThisFrame (void)
{
  static long Overall_Frames_Displayed=0;
  int i;


  // if (ShipEmptyCounter == 1) GameOver = TRUE;

  Total_Frames_Passed_In_Mission++;
  Me.FramesOnThisLevel++;
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
  Me.LastCrysoundTime += Frame_Time ();
  Me.MissionTimeElapsed += Frame_Time();
  Me.LastTransferSoundTime += Frame_Time();
  Me.TextVisibleTime += Frame_Time();
  LevelDoorsNotMovedTime += Frame_Time();

  if ( SkipAFewFrames ) SkipAFewFrames--;

  if ( Me.firewait > 0 )
    {
      Me.firewait-=Frame_Time();
      if (Me.firewait < 0) Me.firewait=0;
    }
  if (ShipEmptyCounter > 1)
    ShipEmptyCounter--;
  if (CurLevel->empty > 2)
    CurLevel->empty--;
  if (RealScore > ShowScore)
    ShowScore++;
  if (RealScore < ShowScore)
    ShowScore--;

  for (i = 0; i < MAX_ENEMYS_ON_SHIP ; i++)
    {

      if (AllEnemys[i].Status == OUT ) continue;

      if (AllEnemys[i].warten > 0) 
	{
	  AllEnemys[i].warten -= Frame_Time() ;
	  if (AllEnemys[i].warten < 0) AllEnemys[i].warten = 0;
	}

      if (AllEnemys[i].firewait > 0) 
	{
	  AllEnemys[i].firewait -= Frame_Time() ;
	  if (AllEnemys[i].firewait <= 0) AllEnemys[i].firewait=0;
	}

      AllEnemys[i].TextVisibleTime += Frame_Time();
    } // for (i=0;...

} /* UpdateCountersForThisFrame() */


#undef _main_c
