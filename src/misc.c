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

#undef DIAGONAL_KEYS_AUS

#define MESPOSX 0
#define MESPOSY 64
#define MESHOEHE 8
#define MESBARBREITE 320

#define MAX_MESSAGE_LEN 100
#define MESBAR_MEM	MESBARBREITE*MESHOEHE+1000

// Die Definition f"ur eine Message kann ruhig lokal hier stehen, da sie
// nur innerhalb des Moduls gebraucht wird.

typedef struct
{
  void *NextMessage;
  int MessageCreated;
  char *MessageText;
}
message, Message;

void CreateMessageBar (char *MText);
void CleanMessageLine (void);
void AdvanceQueue (void);
void Single_Player_Menu (void);
void Multi_Player_Menu (void);
void Options_Menu (void);
void Show_Highscore_Menu (void);
void Show_Mission_Instructions_Menu (void);
void Level_Editor(void);

int New_Game_Requested=FALSE;
int VectsHaveBeenTurned = 0;
unsigned char *MessageBar;
message *Queue = NULL;
// int ThisMessageTime=0;               /* Counter fuer Message-Timing */

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

void
DebugPrintfFloat (float Print_Float)
{
  FILE *debugfile;

  if (debug_level == 0) return;

  debugfile = fopen ("DEBUG.OUT", "a");

  fprintf (debugfile, "%f", Print_Float);
  fclose (debugfile);
};

void
DebugPrintfInt (int Print_Int)
{
  FILE *debugfile;

  if (debug_level == 0) return;

  debugfile = fopen ("DEBUG.OUT", "a");

  fprintf (debugfile, "%d", Print_Int);
  fclose (debugfile);
};

// This Function is for the PORT!!!!
// Replacing all MyRandom-calls with MyMyRandom-calls

/*-----------------------------------------------------------------
 * Desc: return an integer-random number in the range [0,Obergrenze]
 * 
 *-----------------------------------------------------------------*/
int
MyRandom (int Obergrenze)
{
  float Zwisch;
  int ReinerZufall;
  int dice_val;    /* the result in [0, Obergrenze] */

  ReinerZufall = rand ();
  Zwisch = 1.0*ReinerZufall/RAND_MAX; /* random number in [0;1] */

  /* 
   * we always round OFF for the resulting int, therefore
   * we first add 0.99999 to make sure that Obergrenze has
   * roughly the same probablity as the other numbers 
   */
  dice_val = (int)( Zwisch * (1.0 * Obergrenze + 0.99999) );
  return (dice_val);
} /* MyRandom () */


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

// This Function is for the PORT!!!!
// Replacing all MyRandom-calls with MyMyRandom-calls

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

/* **********************************************************************
   Diese Funktion l"oscht alle Roboter auf dem momentanen Level
**********************************************************************/
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
	Feindesliste[i].energy = 0;
	Feindesliste[i].Status = OUT;
      }
} // void Armageddon(void)

/* **********************************************************************
 *	Diese Funktion teleportiert an einen beliebigen Ort im Schiff.
 *	Es werden Levelnummer und Grobkoordinaten "ubergeben.
 *
 **********************************************************************/

void
Teleport (int LNum, int X, int Y)
{
  int curLevel = LNum;
  int array_num = 0;
  Level tmp;
  int i;

  /* Neuen Level und Position setzen */

  if (curLevel != CurLevel->levelnum)
    {				/* wirklich neu ??? */
      /* Aktuellen Level setzen */
      while ((tmp = curShip.AllLevels[array_num]) != NULL)
	{
	  if (tmp->levelnum == curLevel)
	    break;
	  else
	    array_num++;
	}

      CurLevel = curShip.AllLevels[array_num];

      /* Enemys gut verteilen: */
      ShuffleEnemys ();

      /* Position des Influencer richtig setzen */
      Me.pos.x = Grob2Fein(X);   /* Macro to convert from grob to fein */
      Me.pos.y = Grob2Fein(Y);

      /* Alle Blasts und Bullets loeschen */
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
      Me.pos.x = Grob2Fein(X);
      Me.pos.y = Grob2Fein(Y);
    }

  LeaveElevatorSound ();

  UnfadeLevel ();

} /* Teleport() */


/* **********************************************************************
 *  Diese Funktion stellt ein praktisches Cheatmenu zur Verf"ugung
 *
 ***********************************************************************/
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
	      if (Feindesliste[i].levelnum == CurLevel->levelnum) 
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
			  Druidmap[Feindesliste[i].type].druidname,
			  Feindesliste[i].pos.x,
			  Feindesliste[i].pos.y,
			  (int)Feindesliste[i].energy,
			  Feindesliste[i].speed.x);
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
		      i, Feindesliste[i].levelnum,
		      Druidmap[Feindesliste[i].type].druidname,
		      (int)Feindesliste[i].energy,
		      Feindesliste[i].speed.x);
	    } /* for (i<NumEnemys) */

	  printf (" --- END --- \n");
	  getchar_raw ();
	  break;


	case 'd': /* destroy all robots on this level */
	  for (i = 0; i < NumEnemys; i++)
	    {
	      if (Feindesliste[i].levelnum == CurLevel->levelnum)
		Feindesliste[i].energy = -100;
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
	      // NONSENSE FROM THE OLD ENGINE RedrawInfluenceNumber ();
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

/* -----------------------------------------------------------------
 *-----------------------------------------------------------------*/
void
EscapeMenu (void)
{
#define FIRST_MENU_ITEM_POS_X (2*Block_Width)
#define FIRST_MENU_ITEM_POS_Y (USERFENSTERPOSY + FontHeight(Menu_BFont))
enum
  { 
    SINGLE_PLAYER_POSITION=1, 
    MULTI_PLAYER_POSITION=2, 
    OPTIONS_POSITION=3, 
    LEVEL_EDITOR_POSITION=4, 
    QUIT_POSITION=5 
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

      SDL_SetClipRect( ne_screen, NULL );

      DisplayRahmen( 0 );
      Assemble_Combat_Picture ( 0 );

      MakeGridOnScreen();

      // 
      // we highlight the currently selected option with an 
      // influencer to the left before it
      // PutInfluence( FIRST_MENU_ITEM_POS_X , 
      // FIRST_MENU_ITEM_POS_Y + (MenuPosition-1) * (FontHeight(Menu_BFont)) - Block_Width/4 );
      PutInfluence( FIRST_MENU_ITEM_POS_X , 
		    FIRST_MENU_ITEM_POS_Y + ( MenuPosition - 1.5 ) * (FontHeight( Menu_BFont )) );

      CenteredPutString (ne_screen ,  FIRST_MENU_ITEM_POS_Y ,    "Single Player");
      CenteredPutString (ne_screen ,  FIRST_MENU_ITEM_POS_Y +1*FontHeight(Para_BFont),    "Multi Player");
      CenteredPutString (ne_screen ,  FIRST_MENU_ITEM_POS_Y +2*FontHeight(Para_BFont),    "Options");
      CenteredPutString (ne_screen ,  FIRST_MENU_ITEM_POS_Y +3*FontHeight(Para_BFont),    "Level Editor");
      CenteredPutString (ne_screen ,  FIRST_MENU_ITEM_POS_Y +4*FontHeight(Para_BFont),    "Quit Game");

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

  // Since we've faded out the whole scren, it can't hurt
  // to have the top status bar redrawn...
  RahmenIsDestroyed=TRUE;
  Me.status=MOBILE;

  return;

} // EscapeMenu

/* 
-----------------------------------------------------------------
-----------------------------------------------------------------
*/
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

      Assemble_Combat_Picture ( 0 );
      MakeGridOnScreen();
      SDL_SetClipRect( ne_screen, NULL );

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
  DisplayRahmen ( RAHMEN_FORCE_UPDATE );
  InitBars = TRUE;

  return;

} // Options_Menu

/* -----------------------------------------------------------------
  This function prints the actual highscore list.
  It is called from the single player submenu of the escape menu.
  ------------------------------------------------------------------*/
void
Single_Player_Menu (void)
{
#ifdef NEW_ENGINE
  return;
#else
  int Weiter = 0;
  int MenuPosition=1;

  while (!Weiter)
    {

      MakeGridOnScreen( Outline320x200 );

      // Highlight currently selected option with an influencer before it
      DisplayMergeBlock( SINGLE_PLAYER_MENU_POINTER_POS_X, (MenuPosition+3) * (FontHeight(Menu_BFont)/2) - Block_Width/4, 
			 Influencepointer, Block_Width, Block_Height, RealScreen );

      PrepareScaledSurface(FALSE);

      CenteredPutString (ScaledSurface ,  4*FontHeight(Menu_BFont),    "New Game");
      CenteredPutString (ScaledSurface ,  5*FontHeight(Menu_BFont),    "Show Hiscore List");
      CenteredPutString (ScaledSurface ,  6*FontHeight(Menu_BFont),    "Show Mission Instructions");
      CenteredPutString (ScaledSurface ,  7*FontHeight(Menu_BFont),    "Back");

      SDL_UpdateRect(ScaledSurface, 0, 0, SCREENBREITE*SCALE_FACTOR, SCREENHOEHE*SCALE_FACTOR);

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
  ClearGraphMem ( );
  Update_SDL_Screen();
  DisplayRahmen ( RAHMEN_FORCE_UPDATE );
  InitBars = TRUE;

  return;
#endif // !NEW_ENGINE
} // Single_Player_Menu

/*-----------------------------------------------------------------
 *-----------------------------------------------------------------*/
void
Show_Highscore_Menu (void)
{
#ifdef NEW_ENGINE
  return;
#else
  int Weiter = 0;

  enum { NEW_GAME_POSITION=1, SHOW_HISCORE_POSITION=2, SHOW_MISSION_POSITION=3, BACK_POSITION=4 };

  // while( !SpacePressed() && !EnterPressed() ) keyboard_update(); 
  while( SpacePressed() || EnterPressed() ) keyboard_update(); 

  while (!Weiter)
    {

      MakeGridOnScreen( Outline320x200 );

      PrepareScaledSurface(FALSE);

      CenteredPutString (ScaledSurface, 1*FontHeight(Menu_BFont), "Highscore list:" );

      PrintStringFont (ScaledSurface , Menu_BFont, 2*Block_Width , 4*FontHeight(Menu_BFont),    
		       "Highest score: %10s : %6.2f" , HighestName, HighestScoreOfDay );

      PrintStringFont (ScaledSurface , Menu_BFont, 2*Block_Width , 5*FontHeight(Menu_BFont),
		       "Great score: %10s : %6.2f" , GreatScoreName,  GreatScore);

      PrintStringFont (ScaledSurface , Menu_BFont, 2*Block_Width , 6*FontHeight(Menu_BFont),
		       " Lowest Score:  %10s : %6.2f", LowestName,   LowestScoreOfDay);

      // LeftPutString (ScaledSurface , 9*FontHeight(Menu_BFont), "We are looking forward so seeing");
      // LeftPutString (ScaledSurface ,10*FontHeight(Menu_BFont), "new missions and levels from you!");

      SDL_UpdateRect(ScaledSurface, 0, 0, SCREENBREITE*SCALE_FACTOR, SCREENHOEHE*SCALE_FACTOR);

      // Wait until the user does SOMETHING

      if ( EscapePressed() || EnterPressed() || SpacePressed() )
	{
	  Weiter=!Weiter;
	}
    }

  while ( EscapePressed() || EnterPressed() || SpacePressed() );

  ClearGraphMem ( );
  DisplayRahmen ( RAHMEN_FORCE_UPDATE );
  InitBars = TRUE;

  return;
#endif // !NEW_ENGINE
} // Show_Highscore_Menu

/*-----------------------------------------------------------------
 *-----------------------------------------------------------------*/
void
Multi_Player_Menu (void)
{
#ifdef NEW_ENGINE
  return;
#else
  int Weiter = 0;

  enum { NEW_GAME_POSITION=1, SHOW_HISCORE_POSITION=2, SHOW_MISSION_POSITION=3, BACK_POSITION=4 };

  // while( !SpacePressed() && !EnterPressed() ) keyboard_update(); 
  while( SpacePressed() || EnterPressed() ) keyboard_update(); 

  while (!Weiter)
    {

      MakeGridOnScreen( Outline320x200 );

      PrepareScaledSurface(FALSE);

      CenteredPutString (ScaledSurface, 1*FontHeight(Menu_BFont), "MULTI PLAYER" );

      
      LeftPutString (ScaledSurface , 3*FontHeight(Menu_BFont), "We are sorry, but a multi player");
      LeftPutString (ScaledSurface , 4*FontHeight(Menu_BFont), "mode has not yet been implemented.");
      LeftPutString (ScaledSurface , 5*FontHeight(Menu_BFont), "There are plans to do this, but");
      LeftPutString (ScaledSurface , 6*FontHeight(Menu_BFont), "currently it is not a priority.");
      LeftPutString (ScaledSurface , 8*FontHeight(Menu_BFont), "If you feel like setting something");
      LeftPutString (ScaledSurface , 9*FontHeight(Menu_BFont), "up, please contact the developers.");

      SDL_UpdateRect(ScaledSurface, 0, 0, SCREENBREITE*SCALE_FACTOR, SCREENHOEHE*SCALE_FACTOR);

      // Wait until the user does SOMETHING

      if ( EscapePressed() || EnterPressed() || SpacePressed() )
	{
	  Weiter=!Weiter;
	}
    }
  while ( EscapePressed() || EnterPressed() || SpacePressed() );

  ClearGraphMem ( );
  // Update_SDL_Screen();
  DisplayRahmen ( RAHMEN_FORCE_UPDATE );
  InitBars = TRUE;

  return;
#endif // !NEW_ENGINE

} // Multi_Player_Menu

void
Show_Mission_Instructions_Menu (void)
{
#ifdef NEW_ENGINE
  return;
#else
  int Weiter = 0;

  enum { NEW_GAME_POSITION=1, SHOW_HISCORE_POSITION=2, SHOW_MISSION_POSITION=3, BACK_POSITION=4 };

  // while( !SpacePressed() && !EnterPressed() ) keyboard_update(); 
  while( SpacePressed() || EnterPressed() ) keyboard_update(); 

  while (!Weiter)
    {

      MakeGridOnScreen( Outline320x200 );

      PrepareScaledSurface(FALSE);

      CenteredPutString (ScaledSurface ,  1*FontHeight(Menu_BFont),    "MISSION INSTRUCTIONS");
      LeftPutString (ScaledSurface , 3*FontHeight(Menu_BFont), "This is the first mission.  It is");
      LeftPutString (ScaledSurface , 4*FontHeight(Menu_BFont), "identical to the original Paradroid");
      LeftPutString (ScaledSurface , 5*FontHeight(Menu_BFont), "mission from the Commodore C64.");
      LeftPutString (ScaledSurface , 6*FontHeight(Menu_BFont), "So the mission is:");
      LeftPutString (ScaledSurface , 7*FontHeight(Menu_BFont), "Destroy all robots on the ship.");
      LeftPutString (ScaledSurface , 9*FontHeight(Menu_BFont), "If you have some new and good");
      LeftPutString (ScaledSurface ,10*FontHeight(Menu_BFont), "ideas, why not tell us?");

      SDL_UpdateRect(ScaledSurface, 0, 0, SCREENBREITE*SCALE_FACTOR, SCREENHOEHE*SCALE_FACTOR);

      // Wait until the user does SOMETHING

      if ( EscapePressed() || EnterPressed() || SpacePressed() )
	{
	  Weiter=!Weiter;
	}
    }
  while ( EscapePressed() || EnterPressed() || SpacePressed() );
  ClearGraphMem ( );
  DisplayRahmen ( RAHMEN_FORCE_UPDATE );
  InitBars = TRUE;

  return;
#endif
} // ShowMissionInstructionsMenu

void 
Highlight_Current_Block(void)
{
  int i;

  for (i=0; i<Block_Width; i++)
    {
      // This draws a line at the upper border of the current block
      InternWindow[(INTERNHOEHE-1)*(Block_Height/2)*INTERNBREITE*Block_Width+
		   (INTERNBREITE-1)*(Block_Width/2)+i]=BULLETCOLOR;
      // This draws a line at the lower border of the current block
      InternWindow[(INTERNHOEHE+1)*(Block_Height/2)*INTERNBREITE*Block_Width+
		   (INTERNBREITE-1)*(Block_Width/2)+i]=BULLETCOLOR;
      // This draws a line at the left border of the current block
      InternWindow[(INTERNHOEHE-1)*(Block_Height/2)*INTERNBREITE*Block_Width+
		   (INTERNBREITE-1)*(Block_Width/2)+i*INTERNBREITE*Block_Width]=BULLETCOLOR;
      // This draws a line at the right border of the current block
      InternWindow[(INTERNHOEHE-1)*(Block_Height/2)*INTERNBREITE*Block_Width+
		   (INTERNBREITE+1)*(Block_Width/2)+i*INTERNBREITE*Block_Width]=BULLETCOLOR;
    }
}

void 
Level_Editor(void)
{
#ifdef NEW_ENGINE
  return;
#else
  int BlockX, BlockY;
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
	  BlockX=(int)(floor(Me.pos.x/Block_Width));
	  BlockY=(int)(floor(Me.pos.y/Block_Height));
	  
	  GetView();
	  Assemble_Combat_Picture ( SHOW_MAP );
	  Highlight_Current_Block();

	  PrepareScaledSurface( FALSE );
	  CenteredPutString (ScaledSurface ,  1*FontHeight(Menu_BFont),    "LEVEL EDITOR");
	  SDL_UpdateRect(ScaledSurface, 0, 0, SCREENBREITE*SCALE_FACTOR, SCREENHOEHE*SCALE_FACTOR);

	  // If the Level editor pressed some cursor keys, move the
	  // highlited filed (that is Me.pos) accordingly. This is done here:
	  if (LeftPressed()) 
	    {
	      Me.pos.x-=Block_Width;
	      while (LeftPressed());
	    }
	  if (RightPressed()) 
	    {
	      Me.pos.x+=Block_Width;
	      while (RightPressed());
	    }
	  if (UpPressed()) 
	    {
	      Me.pos.y-=Block_Height;
	      while (UpPressed());
	    }
	  if (DownPressed()) 
	    {
	      Me.pos.y+=Block_Height;
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

	  AssembleCombatWindow( 0 );

	  MakeGridOnScreen( Outline320x200 );

	  // Highlight currently selected option with an influencer before it
	  DisplayMergeBlock( SINGLE_PLAYER_MENU_POINTER_POS_X, (MenuPosition+3) * (FontHeight(Menu_BFont)/2) - Block_Width/4, 
			     Influencepointer, Block_Width, Block_Height, RealScreen );
	  
	  PrepareScaledSurface(FALSE);

	  CenteredPutString (ScaledSurface ,  4*FontHeight(Menu_BFont),    "Save Level:");
	  CenteredPutString (ScaledSurface ,  5*FontHeight(Menu_BFont),    "Set Level name:");
	  CenteredPutString (ScaledSurface ,  6*FontHeight(Menu_BFont),    "Back to Level editing");
	  CenteredPutString (ScaledSurface ,  7*FontHeight(Menu_BFont),    "Quit Level Editor");
	  
	  SDL_UpdateRect(ScaledSurface, 0, 0, SCREENBREITE*SCALE_FACTOR, SCREENHOEHE*SCALE_FACTOR);
	  
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
		  CenteredPutString (ScaledSurface ,  9*FontHeight(Menu_BFont),    "Your ship was saved...");
		  SDL_UpdateRect(ScaledSurface, 0, 0, SCREENBREITE*SCALE_FACTOR, SCREENHOEHE*SCALE_FACTOR);
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

#endif // !NEW_ENGINE
} // void Level_Editor(void)

/*@Function============================================================
@Desc: Testfunktion fuer InsertMessage()

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
@Desc: 	Diese Funktion beendet das Programm und setzt alle notwendigen Dinge
	auf ihre Ausgangswerte zur"uck.
	Wenn ein ExitCode != 0 angegeben wurde wartet er noch auf eine Taste.

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
Terminate (int ExitCode)
{
  DebugPrintf ("\nvoid Terminate(int ExitStatus) wurde aufgerufen....\n");
  printf("\n\n\nTermination of Freedroid initiated... \nUnallocation all resouces...\n\n");
  

  SDL_Quit();
  exit (ExitCode);
  return;
}				// void Terminate(int ExitCode)


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
KillQueue (void)
{
  while (Queue)
    AdvanceQueue ();
}

/*@Function============================================================
@Desc: AdvanceQueue(): rueckt in der Queue eins vor und loescht Vorgaenger.

@Ret:
@Int:
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
}				// void AdvanceQueue(void)


/*@Function============================================================
@Desc: Handles all Screenmessages

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
PutMessages (void)
{
  static int MesPos = 0;	/* X-Position der Message-Bar */
  static int Working = FALSE;	/* wird gerade eine Message bearbeitet ?? */
  message *LQueue;		/* Bewegl. Queue-Pointer */
  int i;

  if (!PlusExtentionsOn)
    return;

  DebugPrintf ("\nvoid PutMessages(void): Funktion wurde echt aufgerufen.");

  if (!Queue)
    return;			/* nichts liegt an */
  if (!Working)
    ThisMessageTime = 0;	/* inaktiv, aber Queue->reset time */

  printf ("Time: %d", ThisMessageTime);

// Ausgabe der momentanen Liste:

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

  // Wenn die Nachricht schon sehr alt ist, wird sie gel"oscht. 
  if (Working && (ThisMessageTime > MaxMessageTime))
    {
      AdvanceQueue ();
      CleanMessageLine ();
      Working = FALSE;		// inaktiv
      ThisMessageTime = 0;	// Counter init.
      return;
    }


  /* Alte Mes. hat MinTime gelebt, neue wartet */
  if ((ThisMessageTime > MinMessageTime) && (Queue->NextMessage))
    {
      AdvanceQueue ();		/* Queue weiterbewegen */
      Working = FALSE;		/* inaktiv setzen */
      ThisMessageTime = 0;	/* counter neu init. */
      return;
    }

  // Modul inaktiv und neue Message da --> aktivieren
  if ((!Working) && Queue)
    {

      // Wenn die neue Nachricht noch nicht erzeugt ist, dann erzeuge sie
      if (!Queue->MessageCreated)
	{
	  CreateMessageBar (Queue->MessageText);
	  Queue->MessageCreated = TRUE;
	}

      ThisMessageTime = 0;	/* counter init. */
      CleanMessageLine ();	/* Zeile loeschen */
      Working = TRUE;		/* aktiviert */
    }


  // Modul ist gerade aktiv --> bewegen und anzeigen
  if (Working && Queue)
    {

      MesPos = 10 * ThisMessageTime;	/* zeitl. synchronisierte Bewegung */

      /* nicht ueber linken Rand fahren !! */
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
	  // PORT REINHARDS NEUE ENGINE vga_drawscansegment (MessageBar + i * MESBARBREITE, MESPOSX - MesPos, MESPOSY + i, MesPos);
	}
    }				/* if aktiv + Message da */

}				/* Put Messages */

/*@Function============================================================
@Desc: CleanMessageLine: l"oscht die Nachrichtenzeile am Bildschrim

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
CleanMessageLine (void)
{
  memset (RealScreen + MESPOSX + MESPOSY * SCREENBREITE, 0,
	  SCREENBREITE * MESHOEHE);
}

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
CreateMessageBar (char *MText)
{
  char Worktext[42];
  int i, j;

  DebugPrintf
    ("\nvoid CreateMessageBar(char* MText): Funktion echt aufgerufen.");

  // "Uberl"angen checken
  if (strlen (MText) > 40)
    {
      DebugPrintf
	("\nvoid CreateMessageBar(char* MText): Message hat mehr als 40 Zeichen !.\n");
      getchar ();
      Terminate (ERR);
    }

  // Speicher anfordern, wenn noch keiner da
  if (MessageBar == NULL)
    if ((MessageBar = MyMalloc (MESBAR_MEM)) == NULL)
      {
	DebugPrintf
	  ("\nvoid CreateMessageBar(char* MText): Bekomme keinen Speicher fuer MessageBar !!\n");
	getchar ();
	Terminate (ERR);
      }

  // Message auf 40 Zeichen erg"anzen
  strcpy (Worktext, MText);
  while (strlen (Worktext) < 40)
    strcat (Worktext, " ");

  // Im internen Screen die Nachricht anzeigen und dann ausschneiden
  for (i = 0; i < 40; i++)
    {
      for (j = 0; j < 8; j++)
	{
	  memcpy ((MessageBar + i * 8 + j * SCREENBREITE),
		  (Data70Pointer + Worktext[i] * 8 * 8 + j * 8), 8);
	}
    }

// Am unteren Rand die Nachricht anzeigen und dann ausschneiden
//      gotoxy(1,4);
//      printf("%s",MText);
//      memcpy(MessageBar,RealScreen+3*8*SCREENBREITE,8*SCREENBREITE);

  DebugPrintf
    ("\nvoid CreateMessageBar(char* MText): Funktion hat ihr natuerliches Ende erreicht.");
}				// void CreateMessageBar(char* MText)

/*@Function============================================================
@Desc: 

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
      // Bis vor die n"achste freie Position vorr"ucken
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

  printf ("\nvoid InsertMessage(char* MText): A message has been added:%s",
	  MText);

}				// void InsertMessage(char* MText)

/* **********************************************************************
 *	Diese Funktion erledigt ein normales Malloc, trifft
 *      zuerst aber ein paar Sicherheitsvorkehrungen.
 * 
 *   NOTE: this exits on error, so we don't need to check success!
 * 
 **********************************************************************/
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

/*@Function============================================================
@Desc: DirToVect
			int direction: a number representing a direction
			Vect vector: 	a pointer to a vector for the result
								(must be already allocated !)

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void
DirToVect (int dir, Vect vector)
{

  switch (dir)
    {
    case OBEN:
      vector->x = 0;
      vector->y = -1;
      break;

    case RECHTSOBEN:
      vector->x = 1;
      vector->y = -1;
      break;

    case RECHTS:
      vector->x = 1;
      vector->y = 0;
      break;

    case RECHTSUNTEN:
      vector->x = 1;
      vector->y = 1;
      break;

    case UNTEN:
      vector->x = 0;
      vector->y = 1;
      break;

    case LINKSUNTEN:
      vector->x = -1;
      vector->y = 1;
      break;

    case LINKS:
      vector->x = -1;
      vector->y = 0;
      break;

    case LINKSOBEN:
      vector->x = -1;
      vector->y = -1;
      break;


    default:
      DebugPrintf ("illegal direction in VectToDir() !");
      vector->x = vector->y = 0;
      return;
    }				/* switch */

}				/* DirToVect */

/*@Function============================================================
@Desc: my_sqrt:		Quadrat-Wurzel

@Ret: int
@Int:
* $Function----------------------------------------------------------*/
long
my_sqrt (long wert)
{
  long base = (long) wert - 1;
  long x;
  long res;
  long tmp = wert;
  int prec = 0;			/* Groessenordnung der Zahl */
  long verschiebung = 1;	/* Verschiebung zur Erhoehung der Genauigkeit */
  int counter;

  if (wert < 1)
    return (long) 1;

  while (tmp /= 10)
    prec++;
  prec = 4 - prec;		/* verschiebe auf mind. 4 stellen */
  if (prec < 0)
    prec = 0;

  counter = prec;
  while (counter--)
    verschiebung *= 10;

  x = base;
  x *= verschiebung;

  res = verschiebung + x / 2;
  x *= base;
  res -= x / 8;
  x *= base;
  res += x / 16;

  while (counter--)
    {
      if ((counter == 0) && ((res % 10) >= 5))
	res += 10;
      res /= 10;
    }


  return res;
}

/*@Function============================================================
@Desc: my_abs

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
int
my_abs (int wert)
{
  return (wert < 0) ? -wert : wert;

}

#undef _misc_c
