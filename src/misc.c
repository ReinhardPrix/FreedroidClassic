/*----------------------------------------------------------------------
 *
 * Desc: miscellaeous helpful functions for Freedroid
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
#define _misc_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"


// The definition of the message structure can stay here,
// because its only needed in this module.
typedef struct
{
  void *NextMessage;
  int MessageCreated;
  char *MessageText;
}
message, Message;
#define MESPOSX 0
#define MESPOSY 64
#define MESHOEHE 8
#define MESBARBREITE 320
#define MAX_MESSAGE_LEN 100
#define MESBAR_MEM	MESBARBREITE*MESHOEHE+1000



void CreateMessageBar (char *MText);
void CleanMessageLine (void);
void AdvanceQueue (void);
void Single_Player_Menu (void);
void Multi_Player_Menu (void);
void Credits_Menu (void);
void Options_Menu (void);
void Show_Highscore_Menu (void);
void Show_Mission_Instructions_Menu (void);
void Level_Editor(void);

int New_Game_Requested=FALSE;
unsigned char *MessageBar;
message *Queue = NULL;
// int ThisMessageTime=0;               /* Counter fuer Message-Timing */

struct timeval now, oneframetimestamp, tenframetimestamp,
  onehundredframetimestamp, differenz;
long oneframedelay = 0;
long tenframedelay = 0;
long onehundredframedelay = 0;
float FPSover1 = 10;
float FPSover10 = 10;
float FPSover100 = 10;
Uint32 Now_SDL_Ticks;
Uint32 One_Frame_SDL_Ticks;
Uint32 Ten_Frame_SDL_Ticks;
Uint32 Onehundred_Frame_SDL_Ticks;
int framenr = 0;

/*@Function============================================================
@Desc: realise Pause-Mode: the game process is halted,
       while the graphics and animations are not.  This mode 
       can further be toggled from PAUSE to CHEESE, which is
       a feature from the original program that should probably
       allow for better screenshots.
       
@Ret: 
* $Function----------------------------------------------------------*/
void
Pause (void)
{
  int Pause = TRUE;

  Activate_Conservative_Frame_Computation();

  Me.status = PAUSE;
  Assemble_Combat_Picture ( DO_SCREEN_UPDATE );

  while ( Pause )
    {
      // usleep(10);
      AnimateInfluence ();
      AnimateRefresh ();
      RotateBulletColor ();
      AnimateEnemys ();
      DisplayBanner(0);
      Assemble_Combat_Picture ( DO_SCREEN_UPDATE );
      
      if (CPressed ())
	{
	  Me.status = CHEESE;
	  DisplayBanner( 0 );
	  Assemble_Combat_Picture ( DO_SCREEN_UPDATE );

	  while (!SpacePressed ()); /* stay CHEESE until Space pressed */
	  while ( SpacePressed() ); /* then wait for Space released */
	  
	  Me.status = PAUSE;       /* return to normal PAUSE */
	} /* if (CPressed) */

      if ( SpacePressed() )
	{
	  Pause = FALSE;
	  while ( SpacePressed() );  /* wait for release */
	}

    } /* while (Pause) */

  return;

} /* Pause () */


/*@Function============================================================
@Desc: This function starts the time-taking process.  Later the results
       of this function will be used to calculate the current framerate

       Two methods of time-taking are available.  One uses the SDL 
       ticks.  This seems LESS ACCURATE.  The other one uses the
       standard ansi c gettimeofday functions and are MORE ACCURATE
       but less convenient to use.
@Ret: 
* $Function----------------------------------------------------------*/
void 
StartTakingTimeForFPSCalculation(void)
{
	  /* This ensures, that 0 is never an encountered framenr,
	   * therefore count to 100 here
	   * Take the time now for calculating the frame rate
	   * (DO NOT MOVE THIS COMMAND PLEASE!) */
	  framenr++;

#ifdef USE_SDL_FRAMERATE
	  One_Frame_SDL_Ticks=SDL_GetTicks();
	  if (framenr % 10 == 1)
	    Ten_Frame_SDL_Ticks=SDL_GetTicks();
	  if (framenr % 100 == 1)
	    {
	      Onehundred_Frame_SDL_Ticks=SDL_GetTicks();
	      // printf("\n%f",1/Frame_Time());
	      // printf("Me.pos.x: %g Me.pos.y: %g Me.speed.x: %g Me.speed.y: %g \n",
	      //Me.pos.x, Me.pos.y, Me.speed.x, Me.speed.y );
	      //printf("Me.maxspeed.x: %g \n",
	      //	     Druidmap[Me.type].maxspeed );
	    }
#else
	  gettimeofday (&oneframetimestamp, NULL);
	  if (framenr % 10 == 1)
	    gettimeofday (&tenframetimestamp, NULL);
	  if (framenr % 100 == 1)
	    {
	      gettimeofday (&onehundredframetimestamp, NULL);
	      printf("\n%f",1/Frame_Time());
	    }
#endif

} // void StartTakingTimeForFPSCalculation(void)


/*@Function============================================================
@Desc: This function computes the framerate that has been experienced
       in this frame.  It will be used to correctly calibrate all 
       movements of game objects.

       NOTE:  To query the actual framerate a DIFFERENT function must
       be used, namely Frame_Time().

       Two methods of time-taking are available.  One uses the SDL 
       ticks.  This seems LESS ACCURATE.  The other one uses the
       standard ansi c gettimeofday functions and are MORE ACCURATE
       but less convenient to use.
@Ret: 
* $Function----------------------------------------------------------*/
void 
ComputeFPSForThisFrame(void)
{

	  // In the following paragraph the framerate calculation is done.
	  // There are basically two ways to do this:
	  // The first way is to use SDL_GetTicks(), a function measuring milliseconds
	  // since the initialisation of the SDL.
	  // The second way is to use gettimeofday, a standard ANSI C function I guess,
	  // defined in time.h or so.
	  // 
	  // I have arranged for a definition set in defs.h to switch between the two
	  // methods of ramerate calculation.  THIS MIGHT INDEED MAKE SENSE, SINCE THERE
	  // ARE SOME UNEXPLAINED FRAMERATE PHENOMENA WHICH HAVE TO TO WITH KEYBOARD
	  // SPACE KEY, SO PLEASE DO NOT ERASE EITHER METHOD.  PLEASE ASK JP FIRST.
	  //

#ifdef USE_SDL_FRAMERATE

	  Now_SDL_Ticks=SDL_GetTicks();
	  oneframedelay=Now_SDL_Ticks-One_Frame_SDL_Ticks;
	  tenframedelay=Now_SDL_Ticks-Ten_Frame_SDL_Ticks;
	  onehundredframedelay=Now_SDL_Ticks-Onehundred_Frame_SDL_Ticks;

	  FPSover1 = 1000 * 1 / (float) oneframedelay;
	  FPSover10 = 1000 * 10 / (float) tenframedelay;
	  FPSover100 = 1000 * 100 / (float) onehundredframedelay;

#else

	  gettimeofday (&now, NULL);

	  oneframedelay =
	    (now.tv_usec - oneframetimestamp.tv_usec) + (now.tv_sec -
							 oneframetimestamp.
							 tv_sec) * 1000000;
	  if (framenr % 10 == 0)
	    tenframedelay =
	      ((now.tv_usec - tenframetimestamp.tv_usec)) + (now.tv_sec -
							     tenframetimestamp.
							     tv_sec) *
	      1000000;
	  if ((framenr % 100) == 0)
	    {
	      onehundredframedelay =
		(now.tv_sec - onehundredframetimestamp.tv_sec) * 1000000 +
		(now.tv_usec - onehundredframetimestamp.tv_usec);
	      framenr = 0;
	    }

	  FPSover1 = 1000000 * 1 / (float) oneframedelay;
	  FPSover10 = 1000000 * 10 / (float) tenframedelay;
	  FPSover100 = 1000000 * 100 / (float) onehundredframedelay;

#endif


} // void ComputeFPSForThisFrame(void)

/*@Function============================================================
@Desc: 

 * This function is the key to independence of the framerate for various game elements.
 * It returns the average time needed to draw one frame.
 * Other functions use this to calculate new positions of moving objects, etc..
 *

 * Also there is of course a serious problem when some interuption occurs, like e.g.
 * the options menu is called or the debug menu is called or the console or the elevator
 * is entered or a takeover game takes place.  This might cause HUGE framerates, that could
 * box the influencer out of the ship if used to calculate the new position.

 * To counter unwanted effects after such events we have the SkipAFewFramerates counter,
 * which instructs Rate_To_Be_Returned to return only the overall default framerate since
 * no better substitute exists at this moment.  But on the other hand, this seems to
 * work REALLY well this way.

 * This counter is most conveniently set via the function Activate_Conservative_Frame_Computation,
 * which can be conveniently called from eveywhere.

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
float
Frame_Time (void)
{
  float Rate_To_Be_Returned;
  
  if ( SkipAFewFrames ) 
    {
      Rate_To_Be_Returned = Overall_Average;
      return Rate_To_Be_Returned;
    }

  Rate_To_Be_Returned = (1.0 / FPSover1);

  return Rate_To_Be_Returned;

} // float Frame_Time(void)

/*@Function============================================================
@Desc: 

 * With framerate computation, there is a problem when some interuption occurs, like e.g.
 * the options menu is called or the debug menu is called or the console or the elevator
 * is entered or a takeover game takes place.  This might cause HUGE framerates, that could
 * box the influencer out of the ship if used to calculate the new position.

 * To counter unwanted effects after such events we have the SkipAFewFramerates counter,
 * which instructs Rate_To_Be_Returned to return only the overall default framerate since
 * no better substitute exists at this moment.

 * This counter is most conveniently set via the function Activate_Conservative_Frame_Computation,
 * which can be conveniently called from eveywhere.

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void 
Activate_Conservative_Frame_Computation(void)
{
  // SkipAFewFrames=212;
  // SkipAFewFrames=22;
  SkipAFewFrames=3;

  // Now we are in some form of pause.  It can't
  // hurt to have the top status bar redrawn after that,
  // so we set this variable...
  BannerIsDestroyed=TRUE;

} // void Activate_Conservative_Frame_Computation(void)


/*@Function============================================================
@Desc: This function prepares the screen for the big Escape menu and 
       its submenus.  This means usual content of the screen, i.e. the 
       combat screen and top status bar, is "faded out", the rest of 
       the screen is cleared.  This function resolves some redundance 
       that occured since there are so many submenus needing this.

@Ret: none
* $Function----------------------------------------------------------*/
void 
InitiateMenu( void )
{
      //--------------------
      //Here comes the standard initializer for all the menus and submenus
      //of the big escape menu.  This prepares the screen, so that we can
      //write on it further down.
      SDL_SetClipRect( ne_screen, NULL );
      ClearGraphMem();
      DisplayBanner( RAHMEN_NO_SDL_UPDATE | RAHMEN_FORCE_UPDATE );
      Assemble_Combat_Picture ( 0 );
      SDL_SetClipRect( ne_screen, NULL );
      MakeGridOnScreen();
} // void InitiateMenu(void)

/*@Function============================================================
@Desc: This function is used for debugging purposes.  It writes the
       given string either into a file, on the screen, or simply does
       nothing according to currently set debug level.

@Ret: none
* $Function----------------------------------------------------------*/
void
DebugPrintf (char *Print_String)
{
  static int first_time = TRUE;
  FILE *debugfile;

  if (debug_level == 0) return;

  if (first_time)		/* make sure the first call deletes previous log-file */
    {
      debugfile = fopen ("DEBUG.OUT", "w");
      first_time = FALSE;
    }
  else
    debugfile = fopen ("DEBUG.OUT", "a");

  fprintf (debugfile, Print_String);
  fclose (debugfile);
};

/*@Function============================================================
@Desc: This function is used for debugging purposes.  It writes the
       given float either into a file, on the screen, or simply does
       nothing according to currently set debug level.

@Ret: none
* $Function----------------------------------------------------------*/
void
DebugPrintfFloat (float Print_Float)
{
  FILE *debugfile;

  if (debug_level == 0) return;

  debugfile = fopen ("DEBUG.OUT", "a");

  fprintf (debugfile, "%f", Print_Float);
  fclose (debugfile);
};

/*@Function============================================================
@Desc: This function is used for debugging purposes.  It writes the
       given int either into a file, on the screen, or simply does
       nothing according to currently set debug level.

@Ret: none
* $Function----------------------------------------------------------*/
void
DebugPrintfInt (int Print_Int)
{
  FILE *debugfile;

  if (debug_level == 0) return;

  debugfile = fopen ("DEBUG.OUT", "a");

  fprintf (debugfile, "%d", Print_Int);
  fclose (debugfile);
};

/*@Function============================================================
@Desc: This function is used to generate an integer in range of all
       numbers from 0 to UpperBound.

@Ret:  the generated integer
* $Function----------------------------------------------------------*/
int
MyRandom (int UpperBound)
{
  float tmp;
  int PureRandom;
  int dice_val;    /* the result in [0, Obergrenze] */

  PureRandom = rand ();
  tmp = 1.0*PureRandom/RAND_MAX; /* random number in [0;1] */

  /* 
   * we always round OFF for the resulting int, therefore
   * we first add 0.99999 to make sure that Obergrenze has
   * roughly the same probablity as the other numbers 
   */
  dice_val = (int)( tmp * (1.0 * UpperBound + 0.99999) );
  return (dice_val);
} /* MyRandom () */


/*@Function============================================================
@Desc: This function is used to revers the order of the chars in a
       given string.

@Ret:  none
* $Function----------------------------------------------------------*/
void
reverse (char s[])
{
  int c, i, j;
  for (i = 0, j = strlen (s) - 1; i < j; i++, j--)
    {
      c = s[i];
      s[i] = s[j];
      s[j] = c;
    }
}/* void reverse(char s[]) siehe Kernighan&Ritchie! */


/*@Function============================================================
@Desc: This function is used to transform an integer into an ascii
       string that can then be written to a file.

@Ret:  the given pointer to the string.
* $Function----------------------------------------------------------*/
char *
itoa (int n, char s[], int Dummy)
{
  int i, sign;

  if ((sign = n) < 0)
    n = -n;
  i = 0;
  do
    {
      s[i++] = n % 10 + '0';
    }
  while ((n /= 10) > 0);
  if (sign < 0)
    s[i++] = '-';
  s[i] = '\0';
  reverse (s);
  return s;
}// void itoa(int n, char s[]) siehe Kernighan&Ritchie!

/*@Function============================================================
@Desc: This function is used to transform a long into an ascii
       string that can then be written to a file.

@Ret:  the given pointer to the string.
* $Function----------------------------------------------------------*/
char *
ltoa (long n, char s[], int Dummy)
{
  int i, sign;

  if ((sign = n) < 0)
    n = -n;
  i = 0;
  do
    {
      s[i++] = n % 10 + '0';
    }
  while ((n /= 10) > 0);
  if (sign < 0)
    s[i++] = '-';
  s[i] = '\0';
  reverse (s);
  return s;
} // void ltoa(long n, char s[]) angelehnt an itoa!

/*@Function============================================================
@Desc: This function is kills all enemy robots on the whole ship.
       It querys the user once for safety.

@Ret:  none
* $Function----------------------------------------------------------*/
void
Armageddon (void)
{
  char key =' ';
  int i;

  printf ("\nKill all droids on ship (y/n) ? \n");
  while ((key != 'y') && (key != 'n'))
    key = getchar_raw ();
  if (key == 'n')
    return;
  else
    for (i = 0; i < NumEnemys; i++)
      {
	AllEnemys[i].energy = 0;
	AllEnemys[i].Status = OUT;
      }
} // void Armageddon(void)

/*@Function============================================================
@Desc: This function teleports the influencer to a new position on the
       ship.  THIS CAN BE A POSITION ON A DIFFERENT LEVEL.

@Ret:  none
* $Function----------------------------------------------------------*/
void
Teleport (int LNum, int X, int Y)
{
  int curLevel = LNum;
  int array_num = 0;
  Level tmp;
  int i;

  if (curLevel != CurLevel->levelnum)
    {	

      //--------------------
      // In case a real level change has happend,
      // we need to do a lot of work:

      while ((tmp = curShip.AllLevels[array_num]) != NULL)
	{
	  if (tmp->levelnum == curLevel)
	    break;
	  else
	    array_num++;
	}

      CurLevel = curShip.AllLevels[array_num];

      ShuffleEnemys ();

      Me.pos.x = X;
      Me.pos.y = Y;

      // turn off all blasts and bullets from the old level
      for (i = 0; i < MAXBLASTS; i++)
	AllBlasts[i].type = OUT;
      for (i = 0; i < MAXBULLETS; i++)
	{
	  AllBullets[i].type = OUT;
	  AllBullets[i].mine = FALSE;
	}
    }
  else
    {
      //--------------------
      // If no real level change has occured, everything
      // is simple and we just need to set the new coordinates, haha
      //
      Me.pos.x = X;
      Me.pos.y = Y;
    }

  LeaveElevatorSound ();

  UnfadeLevel ();

} /* Teleport() */


/*@Function============================================================
@Desc: This function provides a convenient cheat menu, so that any 
       tester does not have to play all through the game again and again
       to see if a bug in a certain position has been removed or not.

@Ret:  none
* $Function----------------------------------------------------------*/
extern int CurrentlyCPressed; 	/* the key that brought as in here */
				/* we need to make sure it is set as released */
				/* before we leave ...*/
void
Cheatmenu (void)
{
  char *input;		/* string input from user */
  const char clearscr[] =
    "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
  int Weiter;
  int LNum, X, Y;
  int i, l;
  Waypoint WpList;      /* pointer on current waypoint-list  */

  // Prevent distortion of framerate by the delay coming from 
  // the time spend in the menu.
  Activate_Conservative_Frame_Computation();

  /* 
   * this cheat-menu currently uses the text-window for output
   * therefore, if we're in fullscreen-mode: change to window-mode
   */
#ifndef NEW_ENGINE
  if (fullscreen_on)
    SDL_WM_ToggleFullScreen (ScaledSurface);
#endif

  Weiter = FALSE;
  while (!Weiter)
    {
      printf (clearscr);
      printf ("\n\nCurrent position: Level=%d, X=%d, Y=%d\n\n",
	      CurLevel->levelnum, (int)Me.pos.x, (int)Me.pos.y);
      printf (" a. Armageddon (alle Robots sprengen)\n");
      printf (" l. robot list of current level\n");
      printf (" g. complete robot list\n");
      printf (" d. destroy robots on current level\n");
      printf (" t. Teleportation\n");
      printf (" r. change to new robot type\n");
      printf (" i. Invinciblemode: %s\n",
		 InvincibleMode ? "ON" : "OFF");
      printf (" f. full energy\n");
      printf (" b. blink-energy\n");
      printf (" h. Hide invisible map parts: %s\n",
	      HideInvisibleMap ? "ON" : "OFF" );
      printf (" m. Map of Deck xy\n");
      printf (" s. Sound: %s\n", sound_on ? "ON" : "OFF");
      printf (" x. Fullscreen : %s\n", fullscreen_on ? "ON" : "OFF");
      printf (" w. Print current waypoints\n");
      printf ("\n q. RESUME game\n");

      printf ("\n[ Input in Freedroid window please! ]");
      fflush (stdout);

      switch (getchar_raw ())
	{
	case 'a': /* armageddon */
	  Weiter = 1;
	  Armageddon ();
	  break;

	case 'l': /* robot list of this deck */
	  l = 0; /* line counter for enemy output */
	  for (i = 0; i < NumEnemys; i++)
	    {
	      if (AllEnemys[i].levelnum == CurLevel->levelnum) 
		{
		  if (l && !(l%20)) 
		    {
		      printf ("\n --- MORE --- \n");
		      if( getchar_raw () == 'q')
			break;
		    }
		  if (!(l % 20) )  
		    {
		      printf (clearscr);
		      printf ("\n\n NR.\tID\tX\tY\tENERGY\tspeedX\n");
		      printf ("---------------------------------------------\n");
		    }
		  
		  l ++;
		  printf ("%d.\t%s\t%f\t%f\t%d\t%g.\n", i,
			  Druidmap[AllEnemys[i].type].druidname,
			  AllEnemys[i].pos.x,
			  AllEnemys[i].pos.y,
			  (int)AllEnemys[i].energy,
			  AllEnemys[i].speed.x);
		} /* if (enemy on current level)  */
	    } /* for (i<NumEnemys) */

	  printf (" --- END --- \n");
	  getchar_raw ();
	  break;

	case 'g': /* complete robot list of this ship */
	  for (i = 0; i < NumEnemys; i++)
	    {
	      if (i && !(i%20)) 
		{
		  printf (" --- MORE --- \n");
		  if( getchar_raw () == 'q')
		    break;
		}
	      if ( !(i % 20) )
		{
		  printf (clearscr);
		  printf ("\n\nNr.\tLev.\tID\tEnergy\nSpeed.x\n");
		  printf ("------------------------------\n");
		}
	      
	      printf ("%d\t%d\t%s\t%d\t%g\n",
		      i, AllEnemys[i].levelnum,
		      Druidmap[AllEnemys[i].type].druidname,
		      (int)AllEnemys[i].energy,
		      AllEnemys[i].speed.x);
	    } /* for (i<NumEnemys) */

	  printf (" --- END --- \n");
	  getchar_raw ();
	  break;


	case 'd': /* destroy all robots on this level, haha */
	  for (i = 0; i < NumEnemys; i++)
	    {
	      if (AllEnemys[i].levelnum == CurLevel->levelnum)
		AllEnemys[i].energy = -100;
	    }
	  printf ("\n\nAll robots on this deck killed! \n");
	  getchar_raw ();
	  break;


	case 't': /* Teleportation */
	  printf ("\n Enter Levelnummer, X-Pos, Y-Pos: ");
	  input = GetString (40, 1);
	  sscanf (input, "\n%d, %d, %d", &LNum, &X, &Y);
	  free (input);
	  Teleport (LNum, X, Y);
	  printf ("\nThis is your position on level %d.\n", LNum);
	  printf ("\nPress key to continue \n");
	  getchar_raw ();
	  break;

	case 'r': /* change to new robot type */
	  printf ("\nType number of new robot: ");
	  input = GetString (40, 1);
	  for (i = 0; i < ALLDRUIDTYPES; i++)
	    if (!strcmp (Druidmap[i].druidname, input))
	      break;

	  if (i == ALLDRUIDTYPES)
	    {
	      printf ("\nUnrecognized robot-type: %s\n", input);
	      getchar_raw ();
	    }
	  else
	    {
	      Me.type = i;
	      Me.energy = Druidmap[Me.type].maxenergy;
	      Me.health = Me.energy;
	      printf ("\nYou are now a %s. Have fun!\n", input);
	      getchar_raw ();
	    }
	  free (input);
	  break;

	case 'i': /* togge Invincible mode */
	  InvincibleMode = !InvincibleMode;
	  break;

	case 'f': /* complete heal */
	  Me.energy = Druidmap[Me.type].maxenergy;
	  Me.health = Me.energy;
	  printf ("\nSie sind wieder gesund! \n");
	  getchar_raw ();
	  break;

	case 'b': /* minimal energy */
	  Me.energy = 1;
	  printf ("\nSie sind jetzt ziemlich schwach! \n");
	  getchar_raw ();
	  break;

	case 'h': /* toggle hide invisible map */
	  HideInvisibleMap = !HideInvisibleMap;
	  break;

	case 's': /* toggle sound on/off */
	  sound_on = !sound_on;
	  break;

	case 'm': /* Show deck map in Concept view */
	  printf ("\nLevelnum: ");
	  input = GetString (40, 1);
	  sscanf (input, "%d", &LNum);
	  free (input);
	  ShowDeckMap (curShip.AllLevels[LNum]);
	  getchar_raw ();
	  break;

	case 'x': /* toggle fullscreen - mode */
	  fullscreen_on = !fullscreen_on;
	  break;
	  
	case 'w':  /* print waypoint info of current level */
	  WpList = CurLevel->AllWaypoints;
	  for (i=0; i<MAXWAYPOINTS && WpList[i].x; i++)
	    {
	      if (i && !(i%20))
		{
		  printf (" ---- MORE -----\n");
		  if (getchar_raw () == 'q')
		    break;
		}
	      if ( !(i%20) )
		{
		  printf (clearscr);
		  printf ("\n\nNr.   X   Y      C1  C2  C3  C4 \n");
		  printf ("------------------------------------\n");
		}
	      printf ("%2d   %2d  %2d      %2d  %2d  %2d  %2d\n",
		      i, WpList[i].x, WpList[i].y,
		      WpList[i].connections[0],
		      WpList[i].connections[1],
		      WpList[i].connections[2],
		      WpList[i].connections[3]);

	    } /* for (all waypoints) */
	  printf (" --- END ---\n");
	  getchar_raw ();
	  break;

	case ' ':
	case 'q':
	  Weiter = 1;
	  break;
	} /* switch (getchar_raw()) */
    } /* while (!Weiter) */

  InitBars = TRUE;
  
  keyboard_update (); /* treat all pending keyboard events */
  /* 
   * when changing windows etc, sometimes a key-release event gets 
   * lost, so we have to make sure that CPressed is no longer set
   * or we stay here for ever...
   */
  CurrentlyCPressed = FALSE;

  /* clear terminal (if it's not a terminal: tant pis) */
  printf (clearscr);
  printf ("Back in the game!\n");

  /* toggle back to fullscreen, if that's where we came from */
#ifndef NEW_ENGINE
  if (fullscreen_on)
    SDL_WM_ToggleFullScreen (ScaledSurface);
#endif

  return;
} /* Cheatmenu() */

/*@Function============================================================
@Desc: This function provides a the big escape menu from where you can
       get into different submenus.

@Ret:  none
* $Function----------------------------------------------------------*/
void
EscapeMenu (void)
{
#define FIRST_MENU_ITEM_POS_X (2*Block_Width)
#define FIRST_MENU_ITEM_POS_Y (USERFENSTERPOSY + FontHeight(Menu_BFont))
enum
  { 
    SINGLE_PLAYER_POSITION=1, 
    MULTI_PLAYER_POSITION, 
    OPTIONS_POSITION, 
    LEVEL_EDITOR_POSITION, 
    CREDITS_POSITION,
    QUIT_POSITION
  };

  int Weiter = 0;
  int MenuPosition=1;


  Me.status=MENU;

  DebugPrintf("\nvoid EscapeMenu(void): real function call confirmed."); 

  // Prevent distortion of framerate by the delay coming from 
  // the time spend in the menu.
  Activate_Conservative_Frame_Computation();

  // This is not some Debug Menu but an optically impressive 
  // menu for the player.  Therefore I suggest we just fade out
  // the game screen a little bit.

  SetCurrentFont( Para_BFont );
  
  while ( EscapePressed() );

  while (!Weiter)
    {

      InitiateMenu();

      // 
      // we highlight the currently selected option with an 
      // influencer to the left before it
      // PutInfluence( FIRST_MENU_ITEM_POS_X , 
      // FIRST_MENU_ITEM_POS_Y + (MenuPosition-1) * (FontHeight(Menu_BFont)) - Block_Width/4 );
      SetCurrentFont ( Menu_BFont );
      PutInfluence( FIRST_MENU_ITEM_POS_X , 
		    FIRST_MENU_ITEM_POS_Y + ( MenuPosition - 1.5 ) * (FontHeight( Menu_BFont )) );

      CenteredPutString (ne_screen ,  FIRST_MENU_ITEM_POS_Y ,    "Single Player");
      CenteredPutString (ne_screen ,  FIRST_MENU_ITEM_POS_Y +1*FontHeight(GetCurrentFont()),    "Multi Player");
      CenteredPutString (ne_screen ,  FIRST_MENU_ITEM_POS_Y +2*FontHeight(GetCurrentFont()),    "Options");
      CenteredPutString (ne_screen ,  FIRST_MENU_ITEM_POS_Y +3*FontHeight(GetCurrentFont()),    "Level Editor");
      CenteredPutString (ne_screen ,  FIRST_MENU_ITEM_POS_Y +4*FontHeight(GetCurrentFont()),    "Credits");
      CenteredPutString (ne_screen ,  FIRST_MENU_ITEM_POS_Y +5*FontHeight(GetCurrentFont()),    "Quit Game");

      // SDL_UpdateRect(ne_screen, 0, 0, SCREENBREITE*SCALE_FACTOR, SCREENHOEHE*SCALE_FACTOR);
      SDL_Flip( ne_screen );

      // Wait until the user does SOMETHING

      while( !SpacePressed() && !EnterPressed() && !UpPressed()
	     && !DownPressed() && !EscapePressed() ) ;

      if ( EscapePressed() )
	{
	  while ( EscapePressed() );
	  Weiter=!Weiter;
	}
      if (EnterPressed() || SpacePressed() ) 
	{
	  MenuItemSelectedSound();
	  switch (MenuPosition) 
	    {

	    case SINGLE_PLAYER_POSITION:
	      while (EnterPressed() || SpacePressed() );
	      New_Game_Requested=FALSE;
	      Single_Player_Menu();
	      if (New_Game_Requested) Weiter = TRUE;   /* jp forgot this... ;) */
	      break;
	    case MULTI_PLAYER_POSITION:
	      while (EnterPressed() || SpacePressed() );
	      Multi_Player_Menu();
	      // Weiter = TRUE;   /* jp forgot this... ;) */
	      break;
	    case OPTIONS_POSITION:
	      while (EnterPressed() || SpacePressed() );
	      Options_Menu();
	      // Weiter = TRUE;   /* jp forgot this... ;) */
	      break;
	    case LEVEL_EDITOR_POSITION:
	      while (EnterPressed() || SpacePressed() );
	      Level_Editor();
	      // Weiter = TRUE;   /* jp forgot this... ;) */
	      break;
	    case CREDITS_POSITION:
	      while (EnterPressed() || SpacePressed() );
	      Credits_Menu();
	      // Weiter = TRUE;   /* jp forgot this... ;) */
	      break;
	    case QUIT_POSITION:
	      DebugPrintf("\nvoid Options_Menu(void): Quit Requested by user.  Terminating...");
	      Terminate(0);
	      break;
	    default: 
	      break;
	    } 
	  // Weiter=!Weiter;
	}
      if (UpPressed()) 
	{
	  if (MenuPosition > 1) MenuPosition--;
	  MoveMenuPositionSound();
	  while (UpPressed());
	}
      if (DownPressed()) 
	{
	  if ( MenuPosition < QUIT_POSITION ) MenuPosition++;
	  MoveMenuPositionSound();
	  while (DownPressed());
	}
    }

  ClearGraphMem();
  // Since we've faded out the whole scren, it can't hurt
  // to have the top status bar redrawn...
  BannerIsDestroyed=TRUE;
  Me.status=MOBILE;

  return;

} // EscapeMenu

/*@Function============================================================
@Desc: This function provides a the options menu.  This menu is a 
       submenu of the big EscapeMenu.  Here you can change sound vol.,
       gamma correction, fullscreen mode, display of FPS and such
       things.

@Ret:  none
* $Function----------------------------------------------------------*/
void
Options_Menu (void)
{

  int Weiter = 0;
  int MenuPosition=1;

#define OPTIONS_MENU_ITEM_POS_X (Block_Width/2)
enum
  { SET_BG_MUSIC_VOLUME=1, 
    SET_SOUND_FX_VOLUME, 
    SET_GAMMA_CORRECTION, 
    SET_FULLSCREEN_FLAG, 
    TOGGLE_FRAMERATE, 
    SHOW_ENERGY,
    LEAVE_OPTIONS_MENU };

  // This is not some Debug Menu but an optically impressive 
  // menu for the player.  Therefore I suggest we just fade out
  // the game screen a little bit.

  while ( EscapePressed() );

  while (!Weiter)
    {

      SDL_SetClipRect( ne_screen, NULL );
      ClearGraphMem();
      DisplayBanner( RAHMEN_NO_SDL_UPDATE | RAHMEN_FORCE_UPDATE );
      Assemble_Combat_Picture ( 0 );
      SDL_SetClipRect( ne_screen, NULL );
      MakeGridOnScreen();

      // 
      // we highlight the currently selected option with an 
      // influencer to the left before it
      // PutInfluence( FIRST_MENU_ITEM_POS_X , 
      // FIRST_MENU_ITEM_POS_Y + (MenuPosition-1) * (FontHeight(Menu_BFont)) - Block_Width/4 );
      PutInfluence( OPTIONS_MENU_ITEM_POS_X - Block_Width/2, 
		    FIRST_MENU_ITEM_POS_Y + ( MenuPosition - 1.5 ) * (FontHeight( Menu_BFont )) );


      PrintStringFont (ne_screen , Menu_BFont, OPTIONS_MENU_ITEM_POS_X , FIRST_MENU_ITEM_POS_Y+0*FontHeight(Menu_BFont),
		       "Background Music Volume: %1.2f" , Current_BG_Music_Volume );
      PrintStringFont (ne_screen , Menu_BFont, OPTIONS_MENU_ITEM_POS_X , FIRST_MENU_ITEM_POS_Y+1*FontHeight(Menu_BFont), 
		       "Sound Effects Volume: %1.2f", Current_Sound_FX_Volume );
      PrintStringFont (ne_screen , Menu_BFont, OPTIONS_MENU_ITEM_POS_X , FIRST_MENU_ITEM_POS_Y+2*FontHeight(Menu_BFont), 
		       "Gamma Correction: %1.2f", Current_Gamma_Correction );
      PrintStringFont (ne_screen , Menu_BFont, OPTIONS_MENU_ITEM_POS_X , FIRST_MENU_ITEM_POS_Y+3*FontHeight(Menu_BFont), 
		       "Fullscreen Mode: %s", fullscreen_on ? "ON" : "OFF");
      PrintStringFont (ne_screen , Menu_BFont, OPTIONS_MENU_ITEM_POS_X , FIRST_MENU_ITEM_POS_Y+4*FontHeight(Menu_BFont), 
		       "Show Framerate: %s", Draw_Framerate? "ON" : "OFF");
      PrintStringFont (ne_screen , Menu_BFont, OPTIONS_MENU_ITEM_POS_X , FIRST_MENU_ITEM_POS_Y+5*FontHeight(Menu_BFont), 
		       "Show Energy: %s", Draw_Energy? "ON" : "OFF");
      PrintStringFont (ne_screen , Menu_BFont, OPTIONS_MENU_ITEM_POS_X , FIRST_MENU_ITEM_POS_Y+6*FontHeight(Menu_BFont), 
		       "Back");

      SDL_Flip( ne_screen );

      // Wait until the user does SOMETHING

      while( !SpacePressed() && !EnterPressed() && !UpPressed()
	     && !DownPressed() && !LeftPressed() && !RightPressed() && !EscapePressed() ) ;

      if ( EscapePressed() )
	{
	  while ( EscapePressed() );
	  Weiter=!Weiter;
	}

      // Some menu options can be controlled by pressing right or left
      // These options are gamma corrections, sound volume and the like
      // Therefore left and right key must be resprected.  This is done here:
      if (RightPressed() || LeftPressed() ) 
	{
	  if (MenuPosition == SET_BG_MUSIC_VOLUME ) 
	    {
	      if (RightPressed()) 
		{
		  while (RightPressed());
		  if ( Current_BG_Music_Volume < 1 ) Current_BG_Music_Volume += 0.05;
		  Set_BG_Music_Volume( Current_BG_Music_Volume );
		}
	      if (LeftPressed()) 
		{
		  while (LeftPressed());
		  if ( Current_BG_Music_Volume > 0 ) Current_BG_Music_Volume -= 0.05;
		  Set_BG_Music_Volume( Current_BG_Music_Volume );
		}
	    }
	  if (MenuPosition == SET_SOUND_FX_VOLUME ) 
	    {
	      if (RightPressed()) 
		{
		  while (RightPressed());
		  if ( Current_Sound_FX_Volume < 1 ) Current_Sound_FX_Volume += 0.05;
		  Set_Sound_FX_Volume( Current_Sound_FX_Volume );
		}
	      if (LeftPressed()) 
		{
		  while (LeftPressed());
		  if ( Current_Sound_FX_Volume > 0 ) Current_Sound_FX_Volume -= 0.05;
		  Set_Sound_FX_Volume( Current_Sound_FX_Volume );
		}
	    }
	  if (MenuPosition == SET_GAMMA_CORRECTION ) 
	    {
	      if (RightPressed()) 
		{
		  while (RightPressed());
		  Current_Gamma_Correction+=0.05;
		  SDL_SetGamma( Current_Gamma_Correction , Current_Gamma_Correction , Current_Gamma_Correction );
		}
	      if (LeftPressed()) 
		{
		  while (LeftPressed());
		  Current_Gamma_Correction-=0.05;
		  SDL_SetGamma( Current_Gamma_Correction , Current_Gamma_Correction , Current_Gamma_Correction );
		}
	    }
	}


      if (EnterPressed() || SpacePressed() ) 
	{
	  MenuItemSelectedSound();
	  switch (MenuPosition) 
	    {
	    case SET_FULLSCREEN_FLAG:
	      while (EnterPressed() || SpacePressed() );
	      SDL_WM_ToggleFullScreen (ne_screen);
	      fullscreen_on = !fullscreen_on;
	      break;
	    case TOGGLE_FRAMERATE:
	      while (EnterPressed() || SpacePressed() );
	      Draw_Framerate=!Draw_Framerate;
	      break;
	    case SHOW_ENERGY:
	      while (EnterPressed() || SpacePressed() );
	      Draw_Energy=!Draw_Energy;
	      break;
	    case LEAVE_OPTIONS_MENU:
	      while (EnterPressed() || SpacePressed() );
	      Weiter=TRUE;
	      break;
	    default: 
	      break;
	    } 
	  // Weiter=!Weiter;
	}
      if (UpPressed()) 
	{
	  if ( MenuPosition > 1 ) MenuPosition--;
	  MoveMenuPositionSound();
	  while (UpPressed());
	}
      if (DownPressed()) 
	{
	  if ( MenuPosition < LEAVE_OPTIONS_MENU ) MenuPosition++;
	  MoveMenuPositionSound();
	  while (DownPressed());
	}
    }

  ClearGraphMem ();
  Update_SDL_Screen();
  DisplayBanner ( RAHMEN_FORCE_UPDATE );
  InitBars = TRUE;

  return;

} // Options_Menu

/*@Function============================================================
@Desc: This function provides the single player menu.  This menu is a 
       submenu of the big EscapeMenu.  Here you can restart a new game,
       see the highscore list, see mission instructions and such 
       things.

@Ret:  none
* $Function----------------------------------------------------------*/
void
Single_Player_Menu (void)
{
  int Weiter = 0;
  int MenuPosition=1;

  #define SINGLE_PLAYER_MENU_ITEM_POS_X (Block_Width*1.2)

  while (!Weiter)
    {

      InitiateMenu();

      // 
      // we highlight the currently selected option with an 
      // influencer to the left before it
      // PutInfluence( FIRST_MENU_ITEM_POS_X , 
      // FIRST_MENU_ITEM_POS_Y + (MenuPosition-1) * (FontHeight(Menu_BFont)) - Block_Width/4 );
      PutInfluence( SINGLE_PLAYER_MENU_ITEM_POS_X - Block_Width/2, 
		    (MenuPosition+3) * (FontHeight(Menu_BFont)) - Block_Width/4 );

      CenteredPutString ( ne_screen ,  4*FontHeight(Menu_BFont),    "New Game");
      CenteredPutString ( ne_screen ,  5*FontHeight(Menu_BFont),    "Show Hiscore List");
      CenteredPutString ( ne_screen ,  6*FontHeight(Menu_BFont),    "Show Mission Instructions");
      CenteredPutString ( ne_screen ,  7*FontHeight(Menu_BFont),    "Back");

      SDL_Flip( ne_screen );

      // Wait until the user does SOMETHING

      while( !SpacePressed() && !EnterPressed() && !UpPressed() && !DownPressed() && !EscapePressed() )  
	keyboard_update();

      if ( EscapePressed() )
	{
	  while (EscapePressed());
	  Weiter=!Weiter;
	}

      if (EnterPressed() || SpacePressed() ) 
	{
	  MenuItemSelectedSound();
	  while (EnterPressed() || SpacePressed() );
	  switch (MenuPosition) 
	    {

	    case NEW_GAME_POSITION:
	      while (EnterPressed() || SpacePressed() ) ;
	      New_Game_Requested=TRUE;
	      InitNewGame();
	      Weiter=!Weiter;
	      break;
	    case SHOW_HISCORE_POSITION: 
	      while (EnterPressed() || SpacePressed() ) ;
	      Show_Highscore_Menu();
	      break;
	    case SHOW_MISSION_POSITION:
	      while (EnterPressed() || SpacePressed() ) ;
	      Show_Mission_Instructions_Menu();
	      break;
	    case BACK_POSITION:
	      while (EnterPressed() || SpacePressed() ) ;
	      Weiter=!Weiter;
	      break;
	    default: 
	      break;
	    }
	}
      if (UpPressed()) 
	{
	  if (MenuPosition > 1) MenuPosition--;
	  MoveMenuPositionSound();
	  while (UpPressed());
	}
      if (DownPressed()) 
	{
	  if ( MenuPosition < BACK_POSITION ) MenuPosition++;
	  MoveMenuPositionSound();
	  while (DownPressed());
	}
    }
} // Single_Player_Menu

/*@Function============================================================
@Desc: This function displayes the high scores of the single player
       game.  This function is actually a submenu of the big 
       EscapeMenu.  The function is currently disabled, since rp is
       rewriting the high score administration routines.

@Ret:  none
* $Function----------------------------------------------------------*/
void
Show_Highscore_Menu (void)
{

  /*

  int Weiter = 0;

  enum { NEW_GAME_POSITION=1, SHOW_HISCORE_POSITION=2, SHOW_MISSION_POSITION=3, BACK_POSITION=4 };

  // while( !SpacePressed() && !EnterPressed() ) keyboard_update(); 
  while( SpacePressed() || EnterPressed() ) keyboard_update(); 

  while (!Weiter)
    {

      InitiateMenu();

      CenteredPutString (ScaledSurface, 1*FontHeight(Menu_BFont), "Highscore list:" );

      PrintStringFont (ScaledSurface , Menu_BFont, 2*Block_Width , 4*FontHeight(Menu_BFont),    
		       "Highest score: %10s : %6.2f" , HighestName, HighestScoreOfDay );

      PrintStringFont (ScaledSurface , Menu_BFont, 2*Block_Width , 5*FontHeight(Menu_BFont),
		       "Great score: %10s : %6.2f" , GreatScoreName,  GreatScore);

      PrintStringFont (ScaledSurface , Menu_BFont, 2*Block_Width , 6*FontHeight(Menu_BFont),
		       " Lowest Score:  %10s : %6.2f", LowestName,   LowestScoreOfDay);

      // LeftPutString (ScaledSurface , 9*FontHeight(Menu_BFont), "We are looking forward so seeing");
      // LeftPutString (ScaledSurface ,10*FontHeight(Menu_BFont), "new missions and levels from you!");

      SDL_Flip ( ne_screen );

      // Wait until the user does SOMETHING

      if ( EscapePressed() || EnterPressed() || SpacePressed() )
	{
	  Weiter=!Weiter;
	}
    }

  while ( EscapePressed() || EnterPressed() || SpacePressed() );

  ClearGraphMem ( );
  DisplayBanner ( RAHMEN_FORCE_UPDATE );
  InitBars = TRUE;

  */ 

  return;

} // Show_Highscore_Menu

/*@Function============================================================
@Desc: This function provides the multi player menu.  It is a submenu
       of the big EscapeMenu.  Instead of connecting to a server or 
       something it simply displayes the nonchalant message, that 
       nothing is implemented yet, but sooner or later it will be.

@Ret:  none
* $Function----------------------------------------------------------*/
void
Multi_Player_Menu (void)
{
  int Weiter = 0;

  enum { NEW_GAME_POSITION=1, SHOW_HISCORE_POSITION=2, SHOW_MISSION_POSITION=3, BACK_POSITION=4 };

  // while( !SpacePressed() && !EnterPressed() ) keyboard_update(); 
  while( SpacePressed() || EnterPressed() ) keyboard_update(); 

  while (!Weiter)
    {

      InitiateMenu();

      CenteredPutString ( ne_screen , 1*FontHeight(Menu_BFont), "MULTI PLAYER" );
      LeftPutString ( ne_screen , 3*FontHeight(Menu_BFont), "We are sorry, but a multi player");
      LeftPutString ( ne_screen , 4*FontHeight(Menu_BFont), "mode has not yet been implemented.");
      LeftPutString ( ne_screen , 5*FontHeight(Menu_BFont), "There are plans to do this, but");
      LeftPutString ( ne_screen , 6*FontHeight(Menu_BFont), "currently it is not a priority.");
      LeftPutString ( ne_screen , 8*FontHeight(Menu_BFont), "If you feel like setting something");
      LeftPutString ( ne_screen , 9*FontHeight(Menu_BFont), "up, please contact the developers.");

      SDL_Flip( ne_screen );

      // Wait until the user does SOMETHING

      if ( EscapePressed() || EnterPressed() || SpacePressed() )
	{
	  Weiter=!Weiter;
	}
    }
  while ( EscapePressed() || EnterPressed() || SpacePressed() );

} // Multi_Player_Menu

/*@Function============================================================
@Desc: This function provides the credits screen.  It is a submenu of
       the big EscapeMenu.  Here you can see who helped developing the
       game, currently jp, rp and bastian.

@Ret:  none
* $Function----------------------------------------------------------*/
void
Credits_Menu (void)
{
  int Weiter = 0;

  // while( !SpacePressed() && !EnterPressed() ) keyboard_update(); 
  while( SpacePressed() || EnterPressed() ) keyboard_update(); 

  while (!Weiter)
    {

      InitiateMenu();
      
      DisplayImage ( NE_TITLE_PIC_FILE );

      CenteredPutString ( ne_screen , 1*FontHeight(Menu_BFont), "CREDITS" );
      LeftPutString ( ne_screen , 3*FontHeight(Menu_BFont), "PROGRAMMING:");
      LeftPutString ( ne_screen , 4*FontHeight(Menu_BFont), "Johannes Prix and Reinhard Prix");
      LeftPutString ( ne_screen , 5*FontHeight(Menu_BFont), "");
      LeftPutString ( ne_screen , 6*FontHeight(Menu_BFont), "ARTWORK:");
      LeftPutString ( ne_screen , 7*FontHeight(Menu_BFont), "Bastian Salmela");
      LeftPutString ( ne_screen , 8*FontHeight(Menu_BFont), "");

      SDL_Flip( ne_screen );

      // Wait until the user does SOMETHING

      if ( EscapePressed() || EnterPressed() || SpacePressed() )
	{
	  Weiter=!Weiter;
	}
    }
  while ( EscapePressed() || EnterPressed() || SpacePressed() );

} // Credits_Menu

/*@Function============================================================
@Desc: This function provides the mission instructions.  It is a 
       submenu of the single player menu.

@Ret:  none
* $Function----------------------------------------------------------*/
void
Show_Mission_Instructions_Menu (void)
{
  int Weiter = 0;

  enum { NEW_GAME_POSITION=1, SHOW_HISCORE_POSITION=2, SHOW_MISSION_POSITION=3, BACK_POSITION=4 };

  while( SpacePressed() || EnterPressed() ) keyboard_update(); 

  while (!Weiter)
    {

      InitiateMenu();

      CenteredPutString ( ne_screen ,  1*FontHeight(Menu_BFont),    "MISSION INSTRUCTIONS");
      LeftPutString ( ne_screen , 3*FontHeight(Menu_BFont), "This is the first mission.  It is");
      LeftPutString ( ne_screen , 4*FontHeight(Menu_BFont), "identical to the original Paradroid");
      LeftPutString ( ne_screen , 5*FontHeight(Menu_BFont), "mission from the Commodore C64.");
      LeftPutString ( ne_screen , 6*FontHeight(Menu_BFont), "So the mission is:");
      LeftPutString ( ne_screen , 7*FontHeight(Menu_BFont), "Destroy all robots on the ship.");
      LeftPutString ( ne_screen , 9*FontHeight(Menu_BFont), "If you have some new and good");
      LeftPutString ( ne_screen ,10*FontHeight(Menu_BFont), "ideas, why not tell us?");

      SDL_Flip( ne_screen );

      // Wait until the user does SOMETHING

      if ( EscapePressed() || EnterPressed() || SpacePressed() )
	{
	  Weiter=!Weiter;
	}
    }
  while ( EscapePressed() || EnterPressed() || SpacePressed() );

} // ShowMissionInstructionsMenu

/*@Function============================================================
@Desc: This function is used by the Level Editor integrated into 
       freedroid.  It highlights the map position that is currently 
       edited or would be edited, if the user pressed something.  I.e. 
       it provides a "cursor" for the Level Editor.

@Ret:  none
* $Function----------------------------------------------------------*/
void 
Highlight_Current_Block(void)
{
  int i;
#define HIGHLIGHTCOLOR 255

  SDL_LockSurface( ne_screen );

  for (i=0; i<Block_Width; i++)
    {
      // This draws a (double) line at the upper border of the current block
      putpixel( ne_screen , i + User_Rect.x + (User_Rect.w/2) + (rintf(Me.pos.x)-Me.pos.x - 0.5) * Block_Width , User_Rect.y + User_Rect.h/2 + (rintf(Me.pos.y)-Me.pos.y - 0.5 ) * Block_Height , HIGHLIGHTCOLOR );
      putpixel( ne_screen , i + User_Rect.x + (User_Rect.w/2) + (rintf(Me.pos.x)-Me.pos.x - 0.5) * Block_Width , User_Rect.y + User_Rect.h/2 + (rintf(Me.pos.y)-Me.pos.y - 0.5 ) * Block_Height + 1, HIGHLIGHTCOLOR );

      // This draws a line at the lower border of the current block
      putpixel( ne_screen , i + User_Rect.x + (User_Rect.w/2) + (rintf(Me.pos.x)-Me.pos.x - 0.5) * Block_Width , User_Rect.y + User_Rect.h/2 + (rintf(Me.pos.y)-Me.pos.y + 0.5 ) * Block_Height -1, HIGHLIGHTCOLOR );
      putpixel( ne_screen , i + User_Rect.x + (User_Rect.w/2) + (rintf(Me.pos.x)-Me.pos.x - 0.5) * Block_Width , User_Rect.y + User_Rect.h/2 + (rintf(Me.pos.y)-Me.pos.y + 0.5 ) * Block_Height -2, HIGHLIGHTCOLOR );

      // This draws a line at the left border of the current block
      putpixel( ne_screen , 0 + User_Rect.x + (User_Rect.w/2) + (rintf(Me.pos.x)-Me.pos.x - 0.5) * Block_Width , User_Rect.y + User_Rect.h/2 + (rintf(Me.pos.y)-Me.pos.y - 0.5 ) * Block_Height + i , HIGHLIGHTCOLOR );
      putpixel( ne_screen , 1 + User_Rect.x + (User_Rect.w/2) + (rintf(Me.pos.x)-Me.pos.x - 0.5) * Block_Width , User_Rect.y + User_Rect.h/2 + (rintf(Me.pos.y)-Me.pos.y - 0.5 ) * Block_Height + i , HIGHLIGHTCOLOR );

      // This draws a line at the right border of the current block
      putpixel( ne_screen , -1 + User_Rect.x + (User_Rect.w/2) + (rintf(Me.pos.x)-Me.pos.x + 0.5) * Block_Width , User_Rect.y + User_Rect.h/2 + (rintf(Me.pos.y)-Me.pos.y - 0.5 ) * Block_Height + i , HIGHLIGHTCOLOR );
      putpixel( ne_screen , -2 + User_Rect.x + (User_Rect.w/2) + (rintf(Me.pos.x)-Me.pos.x + 0.5) * Block_Width , User_Rect.y + User_Rect.h/2 + (rintf(Me.pos.y)-Me.pos.y - 0.5 ) * Block_Height + i , HIGHLIGHTCOLOR );

    }

  SDL_UnlockSurface( ne_screen );
} // void Highlight_Current_Block(void)


/*@Function============================================================
@Desc: This function is provides the Level Editor integrated into 
       freedroid.  Actually this function is a submenu of the big
       Escape Menu.  In here you can edit the level and upon pressing
       escape enter a further submenu where you can save the level,
       change level name and quit from level editing.

       NOTE: SAVING CURRENTLY DOES NOT WORK!  DONT WORK TOO MUCH WITH
             THIS IF YOU CANT SAVE YOUR LEVELS LATER!!!

@Ret:  none
* $Function----------------------------------------------------------*/
void 
Level_Editor(void)
{
  int BlockX=rintf(Me.pos.x);
  int BlockY=rintf(Me.pos.y);
  int Done=FALSE;
  int Weiter=FALSE;
  int MenuPosition=1;

  enum
    { SAVE_LEVEL_POSITION=1, SET_LEVEL_NAME_POSITION=2, BACK_TO_LEVEL_EDITING=3, QUIT_LEVEL_EDITOR_POSITION=4 };

  while ( !Done )
    {
      Weiter=FALSE;
      while (!EscapePressed())
	{
	  BlockX=rintf(Me.pos.x);
	  BlockY=rintf(Me.pos.y);
	  
	  ClearUserFenster();
	  Assemble_Combat_Picture ( ONLY_SHOW_MAP );
	  Highlight_Current_Block();

	  CenteredPutString ( ne_screen ,  1*FontHeight(Menu_BFont),    "LEVEL EDITOR");
	  SDL_Flip( ne_screen );

	  //--------------------
	  // If the user of the Level editor pressed some cursor keys, move the
	  // highlited filed (that is Me.pos) accordingly. This is done here:
	  if (LeftPressed()) 
	    {
	      Me.pos.x-=1;
	      while (LeftPressed());
	    }
	  if (RightPressed()) 
	    {
	      Me.pos.x+=1;
	      while (RightPressed());
	    }
	  if (UpPressed()) 
	    {
	      Me.pos.y-=1;
	      while (UpPressed());
	    }
	  if (DownPressed()) 
	    {
	      Me.pos.y+=1;
	      while (DownPressed());
	    }
	  
	  // If the level editor pressed some editing keys, insert the
	  // corresponding map tile.  This is done here:
	  if (KP1Pressed()) 
	    {
	      CurLevel->map[BlockY][BlockX]=ECK_LU;
	    }
	  if (KP2Pressed()) 
	    {
	      if (!Shift_Was_Pressed())
		CurLevel->map[BlockY][BlockX]=T_U;
	      else CurLevel->map[BlockY][BlockX]=KONSOLE_U;
	    }
	  if (KP3Pressed()) 
	    {
	      CurLevel->map[BlockY][BlockX]=ECK_RU;
	    }
	  if (KP4Pressed()) 
	    {
	      if (!Shift_Was_Pressed())
		CurLevel->map[BlockY][BlockX]=T_L;
	      else CurLevel->map[BlockY][BlockX]=KONSOLE_L;
	    }
	  if (KP5Pressed()) 
	    {
	      CurLevel->map[BlockY][BlockX]=KREUZ;
	    }
	  if (KP6Pressed()) 
	    {
	      if (!Shift_Was_Pressed())
		CurLevel->map[BlockY][BlockX]=T_R;
	      else CurLevel->map[BlockY][BlockX]=KONSOLE_R;
	    }
	  if (KP7Pressed()) 
	    {
	      CurLevel->map[BlockY][BlockX]=ECK_RO;
	    }
	  if (KP8Pressed()) 
	    {
	      if (!Shift_Was_Pressed())
		CurLevel->map[BlockY][BlockX]=T_O;
	      else CurLevel->map[BlockY][BlockX]=KONSOLE_O;
	    }
	  if (KP9Pressed()) 
	    {
	      CurLevel->map[BlockY][BlockX]=ECK_RO;
	    }
	  if (APressed())
	    {
	      CurLevel->map[BlockY][BlockX]=ALERT;	      
	    }
	  if (RPressed())
	    {
	      CurLevel->map[BlockY][BlockX]=REFRESH1;	            
	    }
	  if (DPressed())
	    {
	      if (Shift_Was_Pressed())
		CurLevel->map[BlockY][BlockX]=V_ZUTUERE;	            	      
	      else CurLevel->map[BlockY][BlockX]=H_ZUTUERE;	            	      
	    }
	  if (SpacePressed())
	    CurLevel->map[BlockY][BlockX]=FLOOR;	            	      	    
	  if (QPressed())
	    {
	      Terminate(0);
	    }

	} // while (!EscapePressed())
      while( EscapePressed() );

      // After Level editing is done and escape has been pressed, 
      // display the Menu with level save options and all that.

      while (!Weiter)
	{

	  InitiateMenu();

	  // Highlight currently selected option with an influencer before it
	  PutInfluence( SINGLE_PLAYER_MENU_POINTER_POS_X, (MenuPosition+3) * (FontHeight(Menu_BFont)) - Block_Width/4 );

	  CenteredPutString ( ne_screen ,  4*FontHeight(Menu_BFont),    "Save Level:");
	  CenteredPutString ( ne_screen ,  5*FontHeight(Menu_BFont),    "Set Level name:");
	  CenteredPutString ( ne_screen ,  6*FontHeight(Menu_BFont),    "Back to Level editing");
	  CenteredPutString ( ne_screen ,  7*FontHeight(Menu_BFont),    "Quit Level Editor");
	  
	  SDL_Flip ( ne_screen );
	  
	  // Wait until the user does SOMETHING
	  
	  while( !SpacePressed() && !EnterPressed() && !UpPressed() && !DownPressed() )  keyboard_update();
	  
	  if ( EscapePressed() )
	    {
	      while (EscapePressed());
	      Weiter=!Weiter;
	    }
	  
	  if (EnterPressed() || SpacePressed() ) 
	    {
	      MenuItemSelectedSound();
	      while (EnterPressed() || SpacePressed() );
	      switch (MenuPosition) 
		{
		  
		case SAVE_LEVEL_POSITION:
		  while (EnterPressed() || SpacePressed() ) ;
		  SaveShip("Testship");
		  CenteredPutString ( ne_screen ,  9*FontHeight(Menu_BFont),    "Your ship was saved...");
		  SDL_Flip ( ne_screen );
		  while (!EnterPressed() && !SpacePressed() ) ;
		  while (EnterPressed() || SpacePressed() ) ;
		  // Weiter=!Weiter;
		  break;
		case SET_LEVEL_NAME_POSITION: 
		  while (EnterPressed() || SpacePressed() ) ;
		  break;
		case BACK_TO_LEVEL_EDITING:
		  while (EnterPressed() || SpacePressed() ) ;
		  Weiter=!Weiter;
		  break;
		case QUIT_LEVEL_EDITOR_POSITION:
		  while (EnterPressed() || SpacePressed() ) ;
		  Weiter=!Weiter;
		  Done=TRUE;
		  break;
		default: 
		  break;
		}
	    }

	  // If the user pressed up or down, the cursor within
	  // the level editor menu has to be moved, which is done here:
	  if (UpPressed()) 
	    {
	      if (MenuPosition > 1) MenuPosition--;
	      MoveMenuPositionSound();
	      while (UpPressed());
	    }
	  if (DownPressed()) 
	    {
	      if (MenuPosition < 4) MenuPosition++;
	      MoveMenuPositionSound();
	      while (DownPressed());
	    }

	}
      
    } // while (!Done)

} // void Level_Editor(void)

/*@Function============================================================
@Desc: This is a test function for InsertMessage()

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
InsertNewMessage (void)
{
  static int counter = 0;
  char testmessage[100];

  DebugPrintf
    ("\nvoid InsertNewMessage(void): real function call confirmed...");

  counter++;
  sprintf (testmessage, "Das ist die %d .te Message !!", counter);
  InsertMessage (testmessage);

  DebugPrintf ("\nvoid InsertNewMessage(void): end of function reached...");
  return;
}				// void InsertNewMessage(void)

/*@Function============================================================
@Desc: 	This function is used for terminating freedroid.  It will close
        the SDL submodules and exit.

@Ret: 
* $Function----------------------------------------------------------*/
void
Terminate (int ExitCode)
{
  DebugPrintf ("\nvoid Terminate(int ExitStatus) wurde aufgerufen....");
  printf("\n----------------------------------------------------------------------\nTermination of Freedroid initiated... \nUnallocation all resouces...");

  // free the allocated surfaces...
  // SDL_FreeSurface( ne_blocks );
  // SDL_FreeSurface( ne_static );

  // free the mixer channels...
  // Mix_CloseAudio();

  printf("\nAnd now the final step...\n\n");
  SDL_Quit();
  exit (ExitCode);
  return;
}  // void Terminate(int ExitCode)


/*@Function============================================================
@Desc: This function empties the message queue of messages to be
       displayed in a moving font on screen.

@Ret: none
* $Function----------------------------------------------------------*/
void
KillQueue (void)
{
  while (Queue)
    AdvanceQueue ();
}

/*@Function============================================================
@Desc: This functin deletes the currently displayed message and
       advances to the next message.

@Ret: none
* $Function----------------------------------------------------------*/
void
AdvanceQueue (void)
{
  message *tmp;

  DebugPrintf ("\nvoid AdvanceQueue(void): Funktion wurde echt aufgerufen.");

  if (Queue == NULL)
    return;

  if (Queue->MessageText)
    free (Queue->MessageText);
  tmp = Queue;

  Queue = Queue->NextMessage;

  free (tmp);

  DebugPrintf
    ("\nvoid AdvanceQueue(void): Funktion hat ihr natuerliches Ende erfolgreich erreicht....");
} // void AdvanceQueue(void)


/*@Function============================================================
@Desc: This function should put a message from the queue to the scren.
       It surely does not work now and it also needs not work now, 
       since this is a feature not incorporated into the original game
       from the C64 and therefore has less priority.

@Ret: 
* $Function----------------------------------------------------------*/
void
PutMessages (void)
{
  static int MesPos = 0;	// X-position of the message bar 
  static int Working = FALSE;	// is a message beeing processed? 
  message *LQueue;		// mobile queue pointer
  int i;

  if (!PlusExtentionsOn)
    return;

  DebugPrintf ("\nvoid PutMessages(void): Funktion wurde echt aufgerufen.");

  if (!Queue)
    return;			// nothing to be done
  if (!Working)
    ThisMessageTime = 0;	// inactive, but Queue->reset time

  printf ("Time: %d", ThisMessageTime);

// display the current list:

  LQueue = Queue;
  i = 0;
  DebugPrintf ("\nvoid PutMessages(void): This is the Queue of Messages:\n");
  while (LQueue != NULL)
    {
      if ((LQueue->MessageText) == NULL)
	{
	  DebugPrintf
	    ("\nvoid PutMessages(void): ERROR: Textpointer is NULL !!!!!!\n");
	  getchar ();
	}
      printf ("%d. '%s' %d\n", i, LQueue->MessageText,
	      LQueue->MessageCreated);
      i++;
      LQueue = LQueue->NextMessage;
    }
  DebugPrintf (" NULL reached !\n");

  // if the message is very old, it can be deleted...
  if (Working && (ThisMessageTime > MaxMessageTime))
    {
      AdvanceQueue ();
      CleanMessageLine ();
      Working = FALSE;		// inactive
      ThisMessageTime = 0;	// Counter init.
      return;
    }


  // old message has lived for MinTime, new one is waiting
  if ((ThisMessageTime > MinMessageTime) && (Queue->NextMessage))
    {
      AdvanceQueue ();		/* Queue weiterbewegen */
      Working = FALSE;		/* inaktiv setzen */
      ThisMessageTime = 0;	/* counter neu init. */
      return;
    }

  // function currenlty inactive and new message waiting --> activate it
  if ((!Working) && Queue)
    {

      // if message not yet generated, generate it
      if (!Queue->MessageCreated)
	{
	  CreateMessageBar (Queue->MessageText);
	  Queue->MessageCreated = TRUE;
	}

      ThisMessageTime = 0;	/* initialize counter  */
      CleanMessageLine ();	/* delete line */
      Working = TRUE;		/* activated */
    }

  // function currently inactive --> move and display
  if (Working && Queue)
    {

      MesPos = 10 * ThisMessageTime;	/* move synchronized this time */

      /* don't go beyond the left border!! */
      if (MesPos > (MESBARBREITE - 2))
	MesPos = MESBARBREITE - 2;

      // 
      // SINCE 'REALSCREEN' NO LONGER RELEVANT, A NEW ROUTINE HAS TO BE FOUND FOR THE PORT
      //
      //    for(i=0;i<MESHOEHE;i++) {
      //      memcpy(
      //             RealScreen+MESPOSX+MESBARBREITE-MesPos+(MESPOSY+i)*SCREENBREITE,
      //             MessageBar+i*MESBARBREITE,
      //             MesPos
      //            );
      //    } /* for */

      //
      // THIS IS THE NEW ROUTINE.
      //
      for (i = 0; i < MESHOEHE; i++)
	{
	  // PORT REINHARDS NEW GRAPHICS ENGINE vga_drawscansegment (MessageBar + i * MESBARBREITE, MESPOSX - MesPos, MESPOSY + i, MesPos);
	}
    }	/* if aktiv + Message there */
}	/* Put Messages */

/*@Function============================================================
@Desc: CleanMessageLine: deleted the message line on screen

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
CleanMessageLine (void)
{
  // memset (RealScreen + MESPOSX + MESPOSY * SCREENBREITE, 0, SCREENBREITE * MESHOEHE);
}

/*@Function============================================================
@Desc: This function prepares the graphics for the next message to
       be displayed

@Ret: 
* $Function----------------------------------------------------------*/
void
CreateMessageBar (char *MText)
{
  char Worktext[42];
  int i, j;

  DebugPrintf
    ("\nvoid CreateMessageBar(char* MText): real function call confirmed.");

  // check for too long message
  if (strlen (MText) > 40)
    {
      DebugPrintf
	("\nvoid CreateMessageBar(char* MText): Message hat mehr als 40 Zeichen !.\n");
      getchar ();
      Terminate (ERR);
    }

  // allocate memory if this hasn't happened yet
  if (MessageBar == NULL)
    if ((MessageBar = MyMalloc (MESBAR_MEM)) == NULL)
      {
	DebugPrintf
	  ("\nvoid CreateMessageBar(char* MText): Bekomme keinen Speicher fuer MessageBar !!\n");
	getchar ();
	Terminate (ERR);
      }

  // fill in spaces to get 40 chars as message length
  strcpy (Worktext, MText);
  while (strlen (Worktext) < 40)
    strcat (Worktext, " ");

  // display the current message to the internal screen and then cut it out from there
  for (i = 0; i < 40; i++)
    {
      for (j = 0; j < 8; j++)
	{
	  memcpy ((MessageBar + i * 8 + j * SCREENBREITE),
		  (Data70Pointer + Worktext[i] * 8 * 8 + j * 8), 8);
	}
    }

// display the message at the lower border and then cut it
//      gotoxy(1,4);
//      printf("%s",MText);
//      memcpy(MessageBar,RealScreen+3*8*SCREENBREITE,8*SCREENBREITE);

  DebugPrintf
    ("\nvoid CreateMessageBar(char* MText): end of function reached.");
} // void CreateMessageBar(char* MText)

/*@Function============================================================
@Desc: This function insers a new message for the user to be 
       displayed into the message queue

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
InsertMessage (char *MText)
{
  message *LQueue = Queue;

  DebugPrintf
    ("\nvoid InsertMessage(char* MText): real function call confirmed...");

  if (LQueue)
    {
      // move to the next free position in the message queue
      while (LQueue->NextMessage != NULL)
	LQueue = LQueue->NextMessage;
      LQueue->NextMessage = MyMalloc (sizeof (message) + 1);
      LQueue = LQueue->NextMessage;
    }
  else
    {
      Queue = MyMalloc (sizeof (message) + 1);
      LQueue = Queue;
    }

  LQueue->MessageText = MyMalloc (MAX_MESSAGE_LEN + 1);
  strcpy (LQueue->MessageText, MText);
  LQueue->NextMessage = NULL;
  LQueue->MessageCreated = FALSE;
} // void InsertMessage(char* MText)

/*@Function============================================================
@Desc: This function works a malloc, except that it also checks for
       success and terminates in case of "out of memory", so we dont
       need to do this always in the code.

@Ret: 
* $Function----------------------------------------------------------*/
void *
MyMalloc (long Mamount)
{
  void *Mptr = NULL;

  if ((Mptr = malloc ((size_t) Mamount)) == NULL)
    {
      printf (" MyMalloc(%ld) did not succeed!\n", Mamount);
      Terminate(ERR);
    }

  return Mptr;
}				// void* MyMalloc(long Mamount)

#undef _misc_c
