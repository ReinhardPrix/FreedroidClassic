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
 * Desc: the main program
 *
 *----------------------------------------------------------------------*/

#define _main_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"
#include "text.h"
#include "vars.h"
#include "map.h"

int ThisMessageTime;
float LastGotIntoBlastSound = 2;
float LastRefreshSound = 2;

extern bool show_cursor;
void UpdateCountersForThisFrame (void);

/*-----------------------------------------------------------------
 * @Desc: the heart of the Game 
 *
 * @Ret: void
 *
 *-----------------------------------------------------------------*/
int
main (int argc, char * argv[])
{
  int i;
  Uint32 now;

  GameOver = FALSE;
  QuitProgram = FALSE;

  debug_level = 0;       /* 0=no debug 1=first debug level (at the moment=all) */

  joy_sensitivity = 1;
  sound_on = TRUE;	 /* default value, can be overridden by command-line */

  now = SDL_GetTicks();
  InitFreedroid (argc, argv);   // Initialisation of global variables and arrays

  SDL_ShowCursor (SDL_DISABLE);

  while (!QuitProgram)
    {
      InitNewMission ( STANDARD_MISSION );

      // release fire-keys
      if (FirePressedR()) ;
      
      show_droid_info (Me.type, -3, 0);  // show unit-intro page
      show_droid_portrait (Cons_Droid_Rect, Me.type, DROID_ROTATION_TIME, RESET);
      now=SDL_GetTicks();
      while (  (SDL_GetTicks() - now < SHOW_WAIT) && (!FirePressedR())) 
	show_droid_portrait (Cons_Droid_Rect, Me.type, DROID_ROTATION_TIME, 0);

      ClearGraphMem();
      DisplayBanner (NULL, NULL, BANNER_FORCE_UPDATE |BANNER_NO_SDL_UPDATE);
      SDL_Flip(ne_screen);

      GameOver = FALSE;

      SDL_SetCursor (crosshair_cursor); // default cursor is a crosshair
      SDL_ShowCursor (SDL_ENABLE);

      while (!GameOver && !QuitProgram)
	{
	  StartTakingTimeForFPSCalculation(); 

	  UpdateCountersForThisFrame ();

	  ReactToSpecialKeys();

	  if (show_cursor) SDL_ShowCursor(SDL_ENABLE);
	  else SDL_ShowCursor(SDL_DISABLE);

	  MoveLevelDoors ();	

	  AnimateRefresh ();	

	  ExplodeBlasts ();	// move blasts to the right current "phase" of the blast

	  AlertLevelWarning ();  // tout tout, blink blink... Alert!!

	  DisplayBanner (NULL, NULL,  0 );

	  MoveBullets ();   // leave this in front of graphics output: time_in_frames should start with 1

	  Assemble_Combat_Picture ( DO_SCREEN_UPDATE ); 

	  for (i = 0; i < MAXBULLETS; i++) CheckBulletCollisions (i);

	  MoveInfluence ();	// change Influ-speed depending on keys pressed, but
	                        // also change his status and position and "phase" of rotation


	  MoveEnemys ();	// move all the enemys:
	                        // also do attacks on influ and also move "phase" or their rotation


	  CheckInfluenceWallCollisions ();	/* Testen ob der Weg nicht durch Mauern verstellt ist */
	  CheckInfluenceEnemyCollision ();


	  if (CurLevel->empty == TRUE && CurLevel->timer <= 0.0 && CurLevel->color != PD_DARK)
	    {
	      CurLevel->color = PD_DARK;
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
  int i;

  // Here are some things, that were previously done by some periodic */
  // interrupt function
  ThisMessageTime++;

  LastGotIntoBlastSound += Frame_Time ();
  LastRefreshSound += Frame_Time ();
  Me.LastCrysoundTime += Frame_Time ();
  Me.timer += Frame_Time();
  if (CurLevel->timer >= 0.0) CurLevel->timer -= Frame_Time ();
    
  Me.LastTransferSoundTime += Frame_Time();
  Me.TextVisibleTime += Frame_Time();
  LevelDoorsNotMovedTime += Frame_Time();
  if (SkipAFewFrames) SkipAFewFrames = FALSE;

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

  // drain Death-count, responsible for Alert-state
  if (DeathCount > 0)
    DeathCount -= DeathCountDrainSpeed * Frame_Time();
  if (DeathCount < 0) DeathCount = 0;
  // and switch Alert-level according to DeathCount
  AlertLevel = (int)(DeathCount / AlertThreshold);
  if (AlertLevel > AL_RED) AlertLevel = AL_RED;
  // player gets a bonus/second in AlertLevel
  RealScore += AlertLevel * AlertBonusPerSec * Frame_Time();
  

  for (i = 0; i < MAX_ENEMYS_ON_SHIP ; i++)
    {

      if (AllEnemys[i].status == OUT ) continue;

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
