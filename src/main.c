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


/* Schalter fuer die bedingte Kompilierung */

#define MOVEENEMYSOFF 		FALSE
#define ANIMATEENEMYSOFF 	FALSE

#define ENERGIEBALKENOFF 	TRUE

int ThisMessageTime;
float LastGotIntoBlastSound = 2;
float LastRefreshSound = 2;

int card = 0;
int device = 0;
void *handle;

void Debriefing (void);
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
  int Robot_Counter;
  int AllRobotsDead;

  GameOver = FALSE;
  QuitProgram = FALSE;

  sound_on = TRUE;	 /* default value, can be overridden by command-line */
  debug_level = 0;       /* 0=no debug 1=first debug level (at the moment=all) */
  fullscreen_on = FALSE; /* use X11-window or full screen */
  joy_sensitivity = 1;

  /*
   *  Parse command line and set global switches 
   *  this function exits program when error, so we don't need to 
   *  check its success  (dunno if that's good design?)
   */
  parse_command_line (argc, argv);

  /* Initialisieren der globalen Variablen und Arrays */
  InitFreedroid ();

  // NONSENSE FROM THE OLD ENGINE RedrawInfluenceNumber ();

  while (!QuitProgram)
    {

      InitNewGame ();

      GameOver = FALSE;

      while (!GameOver && !QuitProgram)
	{

	  StartTakingTimeForFPSCalculation();

	  UpdateCountersForThisFrame ();

	  ReactToSpecialKeys();

	  if (ShipEmptyCounter == 1)
	    GameOver = TRUE;

	  LastBlastHit++;

	  MoveLevelDoors ();	/* Die Tueren im Level auf und zu bewegen */
	  AnimateRefresh ();	/* Refreshes Animieren */

	  for (i = 0; i < NumEnemys; i++)
	    {
	      if (Feindesliste[i].warten > 0) 
		{
		  Feindesliste[i].warten -= Frame_Time() ;
		  if (Feindesliste[i].warten < 0) Feindesliste[i].warten = 0;
		}
	      if (Feindesliste[i].firewait > 0) 
		{
		  Feindesliste[i].firewait -= Frame_Time() ;
		  if (Feindesliste[i].firewait <= 0) Feindesliste[i].firewait=0;
		}
	    }

	  MoveBullets ();   

	  ExplodeBlasts ();	/* Blasts in der Explosionsphase weiterbewegen */

	  DisplayRahmen( 0 );

	  Assemble_Combat_Picture ( DO_SCREEN_UPDATE ); 

	  for (i = 0; i < MAXBULLETS; i++)
	    CheckBulletCollisions (i);

	  PutMessages ();

	  MoveInfluence ();	// change Influ-speed depending on keys pressed

	  MoveEnemys ();	// Auch die Feinde bewegen 

	  AnimateInfluence ();	// Bei animierten Influencer die Phasen weiterzaehlen 

	  AnimateEnemys ();	// Bei den Feinden auch Phasen weiterzaehlen 

	  InfluenceFrictionWithAir ();

	  /* Influencedruid nach der momentanen Geschwindigkeit weiterbewegen */
	  Me.pos.x += Me.speed.x * Frame_Time ();
	  Me.pos.y += Me.speed.y * Frame_Time ();
	  AdjustSpeed ();

	  CheckInfluenceWallCollisions ();	/* Testen ob der Weg nicht durch Mauern verstellt ist */

	  CheckInfluenceEnemyCollision ();

	  RotateBulletColor ();

	  AllRobotsDead=TRUE;
	  for ( Robot_Counter=0 ; Robot_Counter < MAX_ENEMYS_ON_SHIP ; Robot_Counter++ )
	    {
	      if (Feindesliste[Robot_Counter].energy) AllRobotsDead=FALSE;
	    }
	  if (AllRobotsDead) 
	    {
	      EndTitle();
	      GameOver=TRUE;
	    }
	  
	  if (CurLevel->empty == 2)
	    {
	      LevelGrauFaerben ();
	      CurLevel->empty = TRUE;
	    }			/* if */

	  ComputeFPSForThisFrame();

	} /* while !GameOver */
    } /* while !QuitProgram */
  Terminate (0);
  return (0);
}				// void main(void)

/*@Function============================================================
@Desc: Diese Funktion Sprengt den Influencer und beendet das Programm

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
ThouArtDefeated (void)
{
  int j;
  int now;

  DebugPrintf ("\nvoid ThouArtDefeated(void): Real function call confirmed.");
  Me.status = TERMINATED;
  ThouArtDefeatedSound ();
  ExplodeInfluencer ();

  now=SDL_GetTicks();

  while ( SDL_GetTicks()-now < 1000 * WAIT_AFTER_KILLED )
    {
      Assemble_Combat_Picture ( DO_SCREEN_UPDATE );
      DisplayRahmen( 0 );
      ExplodeBlasts ();
      MoveBullets ();
      MoveEnemys ();
      AnimateEnemys ();
      for (j = 0; j < MAXBULLETS; j++)
	CheckBulletCollisions (j);
      RotateBulletColor ();
    }

#ifdef NEW_ENGINE

#else
  /* Ein Flimmer zieht "uber den Schirm */
  Flimmern (4);  /* type 4 flimmer */
#endif

  Debriefing ();

  /* Soundblaster soll keine Toene mehr spucken */
  //PORT sbfm_silence();

  GameOver = TRUE;

  DebugPrintf
    ("\nvoid ThouArtDefeated(void): Usual end of function reached.");
  printf("\n\n DefeatedDone \n\n");
}				// void ThouArtDefeated(void)

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
ThouArtVictorious (void)
{
  ShipEmptyCounter = WAIT_SHIPEMPTY;
  GameOver = TRUE;		/*  */

  ClearUserFenster ();
  getchar_raw ();
}

/*-----------------------------------------------------------------
 * 
 * 
 * 
 *-----------------------------------------------------------------*/
void
UpdateCountersForThisFrame (void)
{
  static long Overall_Frames_Displayed=0;

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

  if ( SkipAFewFrames ) SkipAFewFrames--;

  if ( Me.firewait > 0 )
    {
      Me.firewait-=Frame_Time()*20;
      if (Me.firewait < 0) Me.firewait=0;
    }
  if (ShipEmptyCounter > 1)
    ShipEmptyCounter--;
  if (WaitElevatorCounter > 0)
    WaitElevatorCounter--;
  if (CurLevel->empty > 2)
    CurLevel->empty--;
  if (RealScore > ShowScore)
    ShowScore++;
  if (RealScore < ShowScore)
    ShowScore--;

} /* UpdateCountersForThisFrame() */


#undef _main_c
