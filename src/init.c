/*----------------------------------------------------------------------
 *
 * Desc: the paraplus initialisation routines
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
#define _init_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"
#include "text.h"
#include "ship.h"


#define FENSTEROFF 		FALSE
#define USEINTSOFF 		FALSE

/* Scroll- Fenster */
#define SCROLLSTARTX		USERFENSTERPOSX
#define SCROLLSTARTY		SCREENHOEHE
#define CHARSPERLINE		((int)(USERFENSTERBREITE/FontHeight(Menu_BFont))*2)

/* -----------------------------------------------------------------
 * This function is for stability while working with the SVGALIB, which otherwise would
 * be inconvenient if not dangerous in the following respect:  When SVGALIB has switched to
 * graphic mode and has grabbed the keyboard in raw mode and the program gets stuck, the 
 * console will NOT be returned to normal, the keyboard will remain useless and login from
 * outside and shutting down or reseting the console will be the only way to avoid a hard
 * reset!
 * Therefore this function is introduced.  When Paradroid starts up, the operating system is
 * instructed to generate a signal ALARM after a specified time has passed.  This signal will
 * be handled by this function, which in turn restores to console to normal and resets the
 * yiff sound server access if applicable. (All this is done via calling Terminate
 * of course.) 
 * -----------------------------------------------------------------*/
static void
timeout (int sig)
{
  DebugPrintf
    ("\n\nstatic void timeout(int sig): Automatic termination NOW!!");
  Terminate (0);
}				/* timeout */

char copyright[] = "\nCopyright (C) 2002 Johannes Prix, Reinhard Prix\n\
Freedroid comes with NO WARRANTY to the extent permitted by law.\n\
You may redistribute copies of Freedroid\n\
under the terms of the GNU General Public License.\n\
For more information about these matters, see the file named COPYING.\n";


char usage_string[] =
  "Usage: freedroid [-v|--version] \n\
                    [-q|--nosound] \n\
                    [-s|--sound] \n\
                    [-t|--timeout=SECONDS] \n\
                    [-f|--fullscreen] [-w|--window]\n\
                    [-j|--sensitivity]\n\
                    [-d|--debug=LEVEL]\n\
\n\
Report bugs to freedroid@??? (sorry, havent got one yet ;)\n";

/* -----------------------------------------------------------------
 *  parse command line arguments and set global switches 
 *  exit on error, so we don't need to return success status
 * -----------------------------------------------------------------*/
void
parse_command_line (int argc, char *const argv[])
{
  int c;
  int timeout_time;		/* timeout to restore text-mode */

  static struct option long_options[] = {
    {"version", 0, 0, 'v'},
    {"help", 	0, 0, 'h'},
    {"nosound", 0, 0, 'q'},
    {"sound", 	0, 0, 's'},
    {"timeout", 1, 0, 't'},
    {"debug", 	2, 0, 'd'},
    {"window",  0, 0, 'w'},
    {"fullscreen",0,0,'f'},
    {"sensitivity",1,0,'j'},
    { 0, 	0, 0,  0}
  };

  //   sound_on=TRUE;

  while (1)
    {
      c = getopt_long (argc, argv, "vqst:h?d::wfj:", long_options, NULL);
      if (c == -1)
	break;

      switch (c)
	{
	  /* version statement -v or --version
	   * following gnu-coding standards for command line interfaces */
	case 'v':
	  printf ("\n%s %s  \n", PACKAGE, VERSION); 
	  printf (copyright);
	  exit (0);
	  break;

	case 'h':
	case '?':
	  printf (usage_string);
	  exit (0);
	  break;

	case 'q':
	  sound_on = FALSE;
	  break;

	case 's':
	  sound_on = TRUE;
	  break;

	case 't':
	  timeout_time = atoi (optarg);
	  if (timeout_time > 0)
	    {
	      signal (SIGALRM, timeout);
	      alarm (timeout_time);	/* Terminate after some seconds for safety. */
	    }
	  break;
	case 'j':
	  joy_sensitivity = atoi (optarg);
	  if (joy_sensitivity < 0 || joy_sensitivity > 32)
	    {
	      printf ("\nJoystick sensitivity must lie in the range [0;32]\n");
	      Terminate(ERR);
	    }
	  break;

	case 'd':
	  if (!optarg) 
	    debug_level = 1;
	  else
	    debug_level = atoi (optarg);
	  break;

	case 'f':
	  fullscreen_on = TRUE;
	  break;
	case 'w':
	  fullscreen_on = FALSE;
	  break;

	default:
	  printf ("\nOption %c not implemented yet! Ignored.", c);
	  break;
	}			/* switch(c) */
    }				/* while(1) */
}				/* parse_command_line */


/*-----------------------------------------------------------------
 * @Desc: Startwerte fuer neues Spiel einstellen 
 * 
 * @Ret: 
 *
 *-----------------------------------------------------------------*/
void
InitNewGame (void)
{
  int i;

  DebugPrintf ("\nvoid InitNewGame(void): real function call confirmed...");

  Activate_Conservative_Frame_Computation();

  LastBlastHit = 0;
  LastGotIntoBlastSound = 2;
  LastRefreshSound = 2;
  PlusExtentionsOn = FALSE;
  ThisMessageTime = 0;
  Draw_Framerate=FALSE;
  Draw_Energy=FALSE;


  /*
   * Die Punkte wieder auf 0 setzen
   */
  RealScore = 0;
  ShowScore = 0;

  /* L"oschen der Messagequeue */
  KillQueue ();
  InsertMessage (" Game on!  Good Luck,,.");

  /* Alle Bullets und Blasts loeschen */
  for (i = 0; i < MAXBULLETS; i++)
    {
      AllBullets[i].type = OUT;
      AllBullets[i].mine = FALSE;
    }

  DebugPrintf ("\nvoid InitNewGame(void): All bullets have been deleted...");

  for (i = 0; i < MAXBLASTS; i++)
    {
      AllBlasts[i].phase = OUT;
      AllBlasts[i].type = OUT;
    }

  DebugPrintf ("\nvoid InitNewGame(void): All blasts have been deleted...");

  /* Alle Levels wieder aktivieren */
  for (i = 0; i < curShip.num_levels; i++)
    curShip.AllLevels[i]->empty = FALSE;

  DebugPrintf
    ("\nvoid InitNewGame(void): All levels have been set to 'active'...");

  // NORMALISATION Me.pos.x = 4*INITIAL_BLOCK_WIDTH;
  // NORMALISATION Me.pos.y = 1.5*INITIAL_BLOCK_HEIGHT;
  // Me.pos.x = 4;
  // Me.pos.y = 1.5;
  Me.pos.x = 1;
  Me.pos.y = 1;
  
  i = MyRandom (3);  /* chose one out of 4 possible start positions */
  i = 2;
  switch (i)
    {
    case 0:
      CurLevel = curShip.AllLevels[4];
      break;

    case 1:
      CurLevel = curShip.AllLevels[5];
      break;

    case 2:
      CurLevel = curShip.AllLevels[6];
      break;

    case 3:
      CurLevel = curShip.AllLevels[7];
      break;

    default:
      printf
	("\n InitNewGame(): MyRandom() failed  Terminating...\n");
      Terminate (ERR);
      break;
    } /* switch */

  printf
    ("\nvoid InitNewGame(void): Starting point for the influencer has been set...:\n");
  printf ("%f,%f", Me.pos.x, Me.pos.y);

  /* Alertcolor auf Gruen zurueckschalten */
  Alert = 0;

  /* Enemys initialisieren */
  if (GetCrew (SHIPNAME) == ERR)
    {
      printf
	("\nInitNewGame(): ERROR: Initialization of enemys failed...");
      Terminate (-1);
    }


  /* Introduction und Title */
  Title ();

  DebugPrintf
    ("\nvoid InitNewGame(void): The title signaton has been successfully displayed...:");

  /* Farben des aktuellen Levels einstellen */
  SetLevelColor (CurLevel->color); 

  /* set correct Influ color */
  SetPalCol (INFLUENCEFARBWERT, Mobilecolor.rot, Mobilecolor.gruen,
	       Mobilecolor.blau);

  /* Den Rahmen fuer das Spiel anzeigen */
  ClearGraphMem();
  DisplayRahmen ( RAHMEN_FORCE_UPDATE );

  SetTextColor (FONT_WHITE, FONT_RED);
  InitBars = TRUE;

  Switch_Background_Music_To (COMBAT_BACKGROUND_MUSIC_SOUND);

  // Now that the briefing and all that is done,
  // the influence structure can be initialized for
  // the new mission:
  Me.type = DRUID001;
  Me.speed.x = 0;
  Me.speed.y = 0;
  Me.energy = Druidmap[DRUID001].maxenergy;
  Me.health = Me.energy;	/* start with max. health */
  Me.autofire = FALSE;
  Me.status = MOBILE;
  Me.phase = 0;
  
  return;

} /* InitNewGame */

/*
-----------------------------------------------------------------
@Desc: InitFreedroid(): 
This function initializes the whole Freedroid game.

THIS MUST NOT BE CONFUSED WITH INITNEWGAME, WHICH
ONLY INITIALIZES A NEW MISSION FOR THE GAME.
 
@Ret: none
 
*-----------------------------------------------------------------*/
void
InitFreedroid (void)
{
  struct timeval timestamp;

  Init_Video ();

  Init_Audio ();
  
  Init_Joy ();

  // The default should be, that no rescaling of the
  // combat window at all is done.
  CurrentCombatScaleFactor = 1;

  /* 
   * Initialise random-number generator in order to make 
   * level-start etc really different at each program start
   */
  gettimeofday(&timestamp, NULL);
  srand((unsigned int) timestamp.tv_sec); /* yes, we convert long->int here! */

  /* Initialisierung der Highscorewerte */
  LowestName = MyMalloc (200);
  HighestName = MyMalloc (200);
  GreatScoreName = MyMalloc (200);
  strcpy (LowestName, "Brian Mill Waits");
  strcpy (HighestName, "Linus Gunar");
  strcpy (GreatScoreName, "Mister X");
  Hallptr = MyMalloc (sizeof (HallElement) + 1);
  Hallptr->PlayerScore = 0;
  Hallptr->PlayerName = MyMalloc (10);
  strcpy (Hallptr->PlayerName, " dummy ");
  Hallptr->NextPlayer = NULL;

  LowestScoreOfDay = 1;
  HighestScoreOfDay = 1000;
  GreatScore = 101;

  Draw_Framerate=FALSE;
  HideInvisibleMap = FALSE;	/* Hide invisible map-parts. Para-extension!! */

  Init_Druidmap ();   /* initialise some global text variables */

  if ( InitLevelColorTable () == FALSE)
    {
      printf (" Kann Farben nicht initialisieren !\n");
      getchar_raw ();
      Terminate (ERR);
    }


  MinMessageTime = 55;
  MaxMessageTime = 850;
  /* Farbwerte fuer die Funktion SetColors */
  Transfercolor.gruen = 13;
  Transfercolor.blau = 13;
  Transfercolor.rot = 63;
  Mobilecolor.gruen = 63;
  Mobilecolor.blau = 63;
  Mobilecolor.rot = 63;

  GameAdapterPresent = FALSE;	/* start with this */
  taste = 255;

  if (LoadShip (SHIPNAME) == ERR)
    {
      printf ("Error in LoadShip\n");
      Terminate (ERR);
    }

  CurLevel = NULL; // please leave this here BEFORE InitPictures
  /* Now fill the pictures correctly to the structs */
  if (!InitPictures ())
    {		
      printf("\n Error in InitPictures reported back...\n");
      Terminate(ERR);
    }

  /* Init the Takeover- Game */
  InitTakeover ();

  /* InternWindow */
  InternWindow = MyMalloc (INTERNBREITE * INTERNHOEHE * BLOCKMEM + 100);

  // Initialisieren der Schildbilder
  //  GetShieldBlocks ();

  return;
} /* InitFreedroid() */

/*-----------------------------------------------------------------
 *
 * a bit stupid: here we initialise the global druid-descriptions
 *
 *-----------------------------------------------------------------*/
void
Init_Druidmap (void)
{

  Druidmap[DRUID001].notes =
    "robot activity influence device. This helmet is self-powered and will control any robot for a short time. Lasers are turret-mounted. ";
  Druidmap[DRUID123].notes =
    "simpe rubbish diposal robot. Common device in most space craft to maintain a clean ship. ";
  Druidmap[DRUID139].notes =
    "created by Dr. Masternak to clean up large heaps of rubbish. Its large scoop is used to collect rubbish. It is then crushed internally. ";
  Druidmap[DRUID247].notes =
    "light duty servant robot. One of the first to use the anti-grav system. ";
  Druidmap[DRUID249].notes =
    "cheaper version of the anti-grav servant robot. ";
  Druidmap[DRUID296].notes =
    "this robot is used mainly for serving drinks. A tray is mounted on the head. Built by Orchard and Marsden Enterprises. ";
  Druidmap[DRUID302].notes =
    "common device for moving small packages. Clamp is mounted on the lower body. ";
  Druidmap[DRUID329].notes =
    "early type messenger robot. Large wheels impede motion on small craft.an";
  Druidmap[DRUID420].notes =
    "slow maintenance robot. Confined to drive maintenance during flight. ";
  Druidmap[DRUID476].notes =
    "ship maintenance robot. Fitted with multiple arms to carry out repairs to the ship efficiently. All craft built after the Jupiter-incident are supplied with a team of these. ";
  Druidmap[DRUID493].notes =
    "slave maintenance droid. Standard version will carry its own toolbox. ";
  Druidmap[DRUID516].notes =
    "early crew droid. Able to carry out simple flight checks only. No longer supplied. ";
  Druidmap[DRUID571].notes =
    "standard crew droid. Supplied with the ship. ";
  Druidmap[DRUID598].notes =
    "a highly sophisticated device. Able to control the Robo-Freighter on its own. ";
  Druidmap[DRUID614].notes =
    "low security sentinel droid. Used to protect areas of the ship from intruders. A slow but sure device. ";
  Druidmap[DRUID615].notes =
    "sophisticated sentinel droid. Only 2000 built by the Nicholson Company. these are now very rare.";
  Druidmap[DRUID629].notes =
    "low sentinel droid. Lasers are built into the turret. These are mounted on a small tank body. May be fitted with an auto-cannon on-the Gillen version. ";
  Druidmap[DRUID711].notes =
    "heavy duty battle droid. Disruptor is built into the head. One of the first in service with the Military. ";
  Druidmap[DRUID742].notes =
    "this version is the one mainly used by the Military. ";
  Druidmap[DRUID751].notes =
    "very heavy duty battle droid. Only a few have so far entered service. These are the most powerful battle units ever built. ";
  Druidmap[DRUID821].notes =
    "a very reliable anti-grav unit is fitted into this droid. It will patrol the ship and eliminate intruders as soon as detected by powerful sensors. ";
  Druidmap[DRUID834].notes =
    "early type anti-grav security droid. Fitted with an over-driven anti-grav unit. This droid is very fast but is not reliable. ";
  Druidmap[DRUID883].notes =
    "this droid was designed from archive data. For some unknown reason it instils great fear in Human adversaries. ";
  Druidmap[DRUID999].notes =
    "experimental command cyborg. Fitted with a new tipe of brain. Mounted on a security droid anti-grav unit for convenience. warning: the influence device may not control a primode brain for long. ";

  return;

} /* Init_Textvars () */



/*-----------------------------------------------------------------
 * @Desc: Diese Prozedur ist fuer die Introduction in das Spiel
 *  verantwortlich. Im Moment beschrankt sich ihre Funktion auf das
 *  Laden und anzeigen eines Titelbildes, das dann ausgeblendet wird.
 * 
 *-----------------------------------------------------------------*/
void
Title (void)
{
  int ScrollEndLine = USERFENSTERPOSY;	/* Endpunkt des Scrollens */

  Switch_Background_Music_To (CLASSICAL_BEEP_BEEP_BACKGROUND_MUSIC);

  DisplayImage ( NE_TITLE_PIC_FILE );

  PrepareScaledSurface(TRUE);

  while (!SpacePressed ());
  while (SpacePressed());

  ClearGraphMem ();

  Me.status=BRIEFING;

  DisplayRahmen( RAHMEN_FORCE_UPDATE ); 

  SetTextColor (FONT_BLACK, FONT_RED);

  ScrollText (TitleText1, SCROLLSTARTX, SCROLLSTARTY, ScrollEndLine);
  ScrollText (TitleText2, SCROLLSTARTX, SCROLLSTARTY, ScrollEndLine);
  ScrollText (TitleText3, SCROLLSTARTX, SCROLLSTARTY, ScrollEndLine);
  ScrollText (TitleText4, SCROLLSTARTX, SCROLLSTARTY, ScrollEndLine);

  return;

} /* Title() */

/*@Function============================================================
@Desc: Diese Prozedur ist fuer die Introduction in das Spiel verantwortlich. Im
   Moment beschr„nkt sich ihre Funktion auf das Laden und anzeigen eines
   Titelbildes, das dann ausgeblendet wird.

@Ret: keiner
@Int: keiner
* $Function----------------------------------------------------------*/
void
EndTitle (void)
{
  int ScrollEndLine = USERFENSTERPOSY;	/* Endpunkt des Scrollens */

  DebugPrintf ("\nvoid EndTitle(void): real function call confirmed...:");

  Switch_Background_Music_To (CLASSICAL_BEEP_BEEP_BACKGROUND_MUSIC);

  DisplayRahmen( RAHMEN_FORCE_UPDATE );

  SetTextColor (FONT_BLACK, FONT_RED);

  ScrollText (EndTitleText1, SCROLLSTARTX, SCROLLSTARTY, ScrollEndLine);
  ScrollText (EndTitleText2, SCROLLSTARTX, SCROLLSTARTY, ScrollEndLine);

  while ( SpacePressed() );

} /* EndTitle() */

/* 
----------------------------------------------------------------------
@Desc: This function does the mission debriefing.  If the score was
very good or very bad, player will be asked for this name and the 
highscore list will be updated.

@Ret: 
@Int:
----------------------------------------------------------------------
*/
void
Debriefing (void)
{
  int DebriefColor;

  DebriefColor = FONT_WHITE;

  Me.status = DEBRIEFING;

  SetUserfenster ( DebriefColor );	// KON_BG_COLOR

  if (RealScore > GreatScore)
    {
      PrintStringFont ( ne_screen , Menu_BFont, USERFENSTERPOSX, USERFENSTERPOSY, 
			"    Great Score !");
      PrintStringFont ( ne_screen , Menu_BFont, USERFENSTERPOSX,
			FontHeight(Menu_BFont)+USERFENSTERPOSY,
			"    Enter your name: ");
      PrepareScaledSurface(TRUE);

      GreatScoreName = GetString (10, 2);
      GreatScore = RealScore;
    }
  else if (RealScore < LowestScoreOfDay)
    {
      // strcpy (Scoretext, "\n   Lowest Score of Day! \n Enter your name:");
      PrintStringFont ( ne_screen , Menu_BFont, USERFENSTERPOSX, USERFENSTERPOSY, 
			    "\n   Lowest Score of Day!");
      PrintStringFont ( ne_screen , Menu_BFont, USERFENSTERPOSX, FontHeight(Menu_BFont)+USERFENSTERPOSY, 
			    "    Enter your name: ");
      PrepareScaledSurface(TRUE);
      LowestName = GetString (10, 2);
      LowestScoreOfDay = RealScore;
    }
  else if (RealScore > HighestScoreOfDay)
    {
      // strcpy (Scoretext, "\n   Highest Score of Day! \n Enter your name:");
      PrintStringFont ( ne_screen , Menu_BFont, USERFENSTERPOSX, USERFENSTERPOSY, 
			"\n   Highest Score of Day!" );
      PrintStringFont ( ne_screen , Menu_BFont, USERFENSTERPOSX, FontHeight(Menu_BFont)+USERFENSTERPOSY, 
			    "    Enter your name: ");
      PrepareScaledSurface(TRUE);
      HighestName = GetString (10, 2);
      HighestScoreOfDay = RealScore;
    }
  // free (Scoretext);

  printf ("\nSurvived Debriefing! \n");

  return;

} /* Debriefing() */

#undef _init_c



