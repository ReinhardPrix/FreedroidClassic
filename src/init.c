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
 *  This file is part of FreeDroid
 *
 *  FreeDroid is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  FreeDroid is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with FreeDroid; see the file COPYING. If not, write to the 
 *  Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
 *  MA  02111-1307  USA
 *
 */

#include <config.h>

#define _parainit_c

#define TITLE_EIN

#undef MODSCHASEIN
#undef NOJUNKWHILEINIT


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
// #include <vga.h>
// #include <vgagl.h>
// #include <vgakeyboard.h>
#include <sys/stat.h>
#include <getopt.h>

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"
#include "paratext.h"

// #include "paravars.h"
#include "ship.h"

#define FENSTEROFF 		FALSE
#define USEINTSOFF 		FALSE

/* Scroll- Fenster */
#define SCROLLSTARTX		USERFENSTERPOSX
#define SCROLLSTARTY		SCREENHOEHE
#define CHARSPERLINE		(int)(USERFENSTERBREITE/FONTBREITE)


char EndTitleText1[] =
"Congratulations!!\n\nIt seems that you have made it!  The Ship is clear.\n\n At the moment, this is the end of FreeDroid.  However we are looking forward to writing a new and different story line, introduce more new concepts, features and sounds.\n\n If you enjoyed the game and would like to contribute, contact one of the developers. \n\n Also if you would like to donate something to help keep alive the FreeDroid development, please also contact the developers.\n\n  Since we have not yet written something new, we can not launch the second part of the game now.\n\n What we can do is inform you about the concept of the new story-line and the features we plan to introduce sooner or later:\n\n After this preview of the coming storyline is over, the game will be restarted.\n You however have made it, but if you want, you can restart from the beginning nevertheless.\n\n  Press Space Bar to\nrestart Freedroid from the beginning. \n \n \n ";

char EndTitleText2[] =
"In the year 2400 in a far distant galaxy strange civilisation has been enslaved by an all-mighty Software Corporation named 'MegaSoft' and called MS for short.  This came to be as follows:  At first all the druids of that civilisation were running a MS Operating System which turned out to be a trojan horse and led to the revolution and the enslavement of men.  By virtue of the tremendous wealth the Corporation had accumulated it was able to break all resistances and ban all other operating systems forever, using the druids with their MS Operating System to enforce the oppression with military strength.\n\n  However not all is yet lost:\nA small group of rebels has managed to create a new 'influence device' running the latest Linux 28.32.199 kernel.  The rebels will soon be spotted and can not hope to withstand an open attack.  The Influence device must be beamed aboard an unmanned transport ship that could evacuate the rebel group.  In the first episode, the rebel influence device is beamed aboard the robo freighter and must eliminate all of the robot crew running the MS operating system.  After this is done, the rebels can beam aboard and make their escape to some distant planet from where they can prepare their next steps to free the world from oppression.\n\n Press Space Bar to relauch the original Freedroid game \n \n \n ";

char TitleText1[] =
  "A fleet of Robo-freighters on its way to the Beta Ceti system reported entering an uncharted field of asteroids. Each ship carries a cargo of battle droids to reinforce the outworld defences. Two distress beacons have been discovered. Similar Messages were stored on each. The ships had been bombarded by a powerful radionic beam from one of the asteroids. All of the robots on the ships, including those in storage, became hyper-active. The crews report an attack by droids, isolating them on the bridge. They cannot reach the shuttle and can hold out for only a couple more hours.  Since these beacons were located two days ago, we can only fear the worst.  Some of the fleet was last seen heading for enemy space. In enemy hands the droids can be used against our forces. Docking would be impossible but we can beam aboard a prototype Influence Device. \n \n ";


char TitleText2[] =
  "The 001 Influence Device consists of a helmet, which, when placed over a robots control unit can halt the normal activities of that robot for a short time. The helmet has its own energy supply and powers the robot itself, at an upgraded capability. The helmet also uses an energy cloak for protection of the host. The helmet is fitted with twin lasers mounted in a turret. These can be focussed on any target inside a range of eight metres. Most of the device's resources are channelled towards holding control of the host robot, as it attempts to resume 'normal' operation. It is therefore necessary to change the host robot often to prevent the device from burning out. Transfer to a new robot requires the device to drain its host of energy in order to take ist over. Failure to achieve transfer results in the device being a free agent once more.\n\n        Press space bar to skip instructions\n \n ";

char TitleText3[] =
  "An Influence Device can transmitt data to your console.  A small-scale plan of the whole deck is available, as well as a side elevation of the ship. Robots are represented on-screen as a symbol showing a three-digit number. The first digit shown is the important one, the class of the robot. It denotes the strength also. To find out more about any given robot, use the robot enquiry system at a console. Only data about units of a lower class than your current host is available, since it is the host's security clearance which is used to acces the console. \n  \n  \n  \n Press space bar to skip instructions\n\n\n";

char TitleText4[] =
  "Controls\n\
\n\
The game is controlled via keyboard input.\n\
\n\
Use cursor keys to move around.  The speed you can go \
depends on the druid you currently control.\n\
\n\
If you press space bar in -addition- to a cursor key, this fires \
the weapon of the druid you currently control.\n\n\
If you press space bar whilst NOT pressing a cursor key, \
this will enter transfer mode.  You will notice your robot to \
take on a flashy red color.  \
Now if you touch some other druid, this will initiate the takeover \
process in which you have to win a small game of logical curcuits \
within the given time.\n\
If you succeed, you thereafter can control this new droid and for game \
purposes, it is as if you were him.\n
If you loose, you either are destroyed if you didn't control an enemy \
druid at that time, or the host you controlled is destroyed together with \
the unit you wished to control.\n\
\n\
Watch out for energy refreshing fields and elevators.\n
\n\
Elevators are also entered via transfer mode if you stand still on the elevaor.\n\
\n\
    Press Fire to Play\n \n \n \n \n \n";


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

char copyright[] = "\nCopyright (C) 2002 Johannes Prix, Reinhard Prix
FreeDroid comes with NO WARRANTY to the extent permitted by law.
You may redistribute copies of FreeDroid
under the terms of the GNU General Public License.
For more information about these matters, see the file named COPYING.\n";


char usage_string[] =
  "Usage: freedroid [-v|--version] [-q|--nosound] [-s|--sound] 
			[-t|--timeout=SECONDS]
			[-f|--fullscreen] [-w|--window]
	      		[-d|--debug=LEVEL]\n
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
    { 0, 	0, 0,  0}
  };

  while (1)
    {
      c = getopt_long (argc, argv, "vqst:h?d::wf", long_options, NULL);
      if (c == -1)
	break;

      switch (c)
	{
	  /* version statement -v or --version
	   * following gnu-coding standards for command line interfaces */
	case 'v':
	  // printf (PACKAGE_STRING); 
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

	case 'd':
	  if (!optarg) 
	    debug_level = 1;
	  else
	    debug_level = atoi (optarg);
	  break;

	case 'f':
	  use_fullscreen = TRUE;
	  break;
	case 'w':
	  use_fullscreen = FALSE;
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

  InterruptInfolineUpdate = TRUE;
  LastBlastHit = 0;
  LastGotIntoBlastSound = 2;
  LastRefreshSound = 2;
  PlusExtentionsOn = FALSE;
  ThisMessageTime = 0;

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

  i = MyRandom (3);
  switch (i)
    {
    case 0:
      Me.pos.x = 120;
      Me.pos.y = 48;
      CurLevel = curShip.AllLevels[4];
      break;

    case 1:
      Me.pos.x = 120;
      Me.pos.y = 48;
      CurLevel = curShip.AllLevels[5];
      break;

    case 2:
      Me.pos.x = 120;
      Me.pos.y = 48;
      CurLevel = curShip.AllLevels[6];
      break;

    case 3:
      Me.pos.x = 120;
      Me.pos.y = 48;
      CurLevel = curShip.AllLevels[7];
      break;

    default:
      DebugPrintf
	("\n InitNewGame(): MyRandom() failed  Terminating...");
      Terminate (ERR);
      break;
    } /* switch */

  DebugPrintf
    ("\nvoid InitNewGame(void): Starting point for the influencer has been set...:");
  printf ("%f,%f", Me.pos.x, Me.pos.y);

  /* Alertcolor auf Gruen zurueckschalten */
  Alert = 0;

  /* Enemys initialisieren */
  if (GetCrew (SHIPNAME) == ERR)
    {
      DebugPrintf
	("\nvoid InitNewGame(void): ERROR: Initialization of enemys failed... Terminating...");
      Terminate (-1);
    }

  DebugPrintf ("\nvoid InitNewGame(void): Enemys have been initialized...:");

  /* Influ initialisieren */
  Me.type = DRUID001;
  Me.speed.x = 0;
  Me.speed.y = 0;
  Me.energy = Druidmap[DRUID001].maxenergy;
  Me.health = Me.energy;	/* start with max. health */
  Me.autofire = FALSE;
  Me.status = MOBILE;
  Me.phase = 0;

  DebugPrintf
    ("\nvoid InitNewGame(void): The status of the influencer has been initialized...:");

  RedrawInfluenceNumber ();

  DebugPrintf
    ("\nvoid InitNewGame(void): The influence number has been redrawn...:");

  /* Introduction und Title */
#ifdef TITLE_EIN
  Title ();
#endif

  DebugPrintf
    ("\nvoid InitNewGame(void): The title signaton has been successfully displayed...:");

  /* Farben des aktuellen Levels einstellen */
  SetLevelColor (CurLevel->color);

  LeftInfo[0] = '\0';
  RightInfo[0] = '\0';

  /* Den Rahmen fuer das Spiel anzeigen */
  ClearVGAScreen ();
  DisplayRahmen (RealScreen);
  // DisplayRahmen(NULL);

  SetTextBorder (0, 0, SCREENBREITE, SCREENHOEHE, 40);

  SetTextColor (FONT_WHITE, FONT_RED);
  //    InitPalette();
#ifdef NOJUNKWHILEINIT
  Monitorsignalunterbrechung (0);
#endif
  InitBars = TRUE;

  Switch_Background_Music_To (COMBAT_BACKGROUND_MUSIC_SOUND);

  DebugPrintf ("\nvoid InitNewGame(void): end of function reached.");

}				/* InitNewGame */

/*@Function============================================================
@Desc: InitParaplus(): initialisiert das Spiel beim Programmstart

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void
InitParaplus (void)
{
  struct timeval timestamp;

  DebugPrintf ("\nvoid InitParaplus(void) wurde echt aufgerufen....\n");

  Set_SVGALIB_Video_ON ();

  Init_YIFF_Sound_Server ();

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
  strcpy (LowestName, " Bill Gates");
  strcpy (HighestName, "Linus");
  strcpy (GreatScoreName, "Mister X");
  Hallptr = MyMalloc (sizeof (HallElement) + 1);
  Hallptr->PlayerScore = 0;
  Hallptr->PlayerName = MyMalloc (10);
  strcpy (Hallptr->PlayerName, " dummy ");
  Hallptr->NextPlayer = NULL;

  LowestScoreOfDay = 1;
  HighestScoreOfDay = 1000;
  GreatScore = 101;

  HideInvisibleMap = FALSE;	/* Hide invisible map-parts. Para-extension!! */

  DebugPrintf
    ("\nvoid InitParaplus(void): Highscorevariablen wurden erfolgreich initialisiert...");

  if (InitLevelColorTable () == FALSE)
    {
      DebugPrintf (" Kann Farben nicht initialisieren !");
      getchar ();
      Terminate (0);
    }

  if (InitParaplusFont () == ERR)
    {
      DebugPrintf (" Kann Schrift nicht initialisieren !");
      getchar ();
      Terminate (ERR);
    }

  DebugPrintf
    ("\nvoid InitParaplus(void): Farben- und Fontinitialisierung zumindest fehlerfrei ueberwunden....");

  MinMessageTime = 55;
  MaxMessageTime = 850;
  /* Farbwerte fuer die Funktion SetColors */
  Transfercolor.gruen = 13;
  Transfercolor.blau = 13;
  Transfercolor.rot = 63;
  Mobilecolor.gruen = 63;
  Mobilecolor.blau = 63;
  Mobilecolor.rot = 63;

  Druidmap[DRUID001].notes =
    "robot activity influence device. This helmet is self-powered and will control any robot for a short time. Lasers are turret‹mounted. ";
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
    "ship maintenance robot. Fitted with multiple arms to carry out repairs to the ship efficiently. All craft built after the Jupiter‹incident are supplied with a team of these. ";
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
    "low sentinel droid. Lasers are built into the turret. These are mounted on a small tank body. May be fitted with an auto-cannon on‹the Gillen version. ";
  Druidmap[DRUID711].notes =
    "heavy duty battle droid. Disruptor is built into the head. One of the first in service with the Military. ";
  Druidmap[DRUID742].notes =
    "this version is the one mainly used by the Military. ";
  Druidmap[DRUID751].notes =
    "very heavy duty battle droid. Only a few have so far entered service. These are the most powerful battle units ever built. ";
  Druidmap[DRUID821].notes =
    "a very reliable anti-grav unit is fitted into this droid. It will patrol the ship and eliminate intruders as soon as detected by‹powerful sensors. ";
  Druidmap[DRUID834].notes =
    "early type anti-grav security droid. Fitted with an over-driven anti-grav unit. This droid is very fast but is not reliable. ";
  Druidmap[DRUID883].notes =
    "this droid was designed from archive data. For some unknown reason it instils great fear in Human adversaries. ";
  Druidmap[DRUID999].notes =
    "experimental command cyborg. Fitted with a new tipe of brain. Mounted on a security droid anti-grav unit for convenience.‹warning: the influence device may not control a primode brain for long. ";
  IntroMSG1 =
    "Dies symbolisiert den Text, der zu Beginn des Spiels ausgegeben werden soll. Nach einer gewissen Zeit soll der Text nach unten weiterscrolloen und zwar in einer moeglichst fliessenden Form.";

  GameAdapterPresent = FALSE;	/* start with this */
  taste = 255;

  /* Sounds on/off */
  ModPlayerOn = FALSE;

  DebugPrintf
    ("\nvoid InitParaplus(void): Textmeldungsvariablen wurden erfolgreich initialisiert....");

  /* ScreenPointer setzen */
  RealScreen = malloc (SCREENBREITE * SCREENHOEHE + 10);
  Outline320x200 = malloc (SCREENBREITE * SCREENHOEHE + 10);
  InternalScreen = (unsigned char *) malloc (SCREENHOEHE * SCREENBREITE + 10);

  DebugPrintf
    ("\nvoid InitParaplus(void): Realscreen und Internalscreen haben erfolgreich Speicher erhalten....");

  if (LoadShip (SHIPNAME) == ERR)
    {
      DebugPrintf ("Error in LoadShip");
      Terminate (-1);
    }

  DebugPrintf
    ("\nvoid InitParaplus(void): LoadShip(...) ist erfolgreich zurueckgekehrt....");

  /* Now fill the pictures correctly to the structs */
  if (!InitPictures ())
    {				/* Fehler aufgetreten */
      DebugPrintf("\n Error in InitPictures reported back... Terminating....");
      Terminate(ERR);
      return;
    }

  DebugPrintf
    ("\nvoid InitParaplus(void): InitPictures(void) ist erfolgreich zurueckgekehrt....");

  /* Init the Takeover- Game */
  InitTakeover ();

  DebugPrintf
    ("\nvoid InitParaplus(void): InitTakeover(void) ist erfolgreich zurueckgekehrt....");

  /* Die Zahlen, mit denen die Robotkennungen erzeugt werden einlesen */
  GetDigits ();

  DebugPrintf
    ("\nvoid InitParaplus(void): GetDigits(void) ist erfolgreich zurueckgekehrt....");

  /* InternWindow */
  /* wenn moeglich: Speicher sparen und mit InternalScreen ueberlappen: */
  if (INTERNHOEHE * INTERNBREITE * BLOCKMEM <= SCREENHOEHE * SCREENBREITE)
    {
      InternWindow = InternalScreen;
    }
  else
    {
      if ((InternWindow =
	   (unsigned char *)
	   MyMalloc (INTERNBREITE * INTERNHOEHE * BLOCKMEM + 100)) == NULL)
	{
	  DebugPrintf ("\nFatal: Out of Memory for InternWindow.");
	  getchar ();
	  Terminate (-1);
	}
    }

  DebugPrintf
    ("\nvoid InitParaplus(void): InternWindow wurde erfolgreich initialisiert....");

  /* eigenen Zeichensatz installieren */
  LadeZeichensatz (DATA70ZEICHENSATZ);

  DebugPrintf
    ("\nvoid InitParaplus(void): Zeichensatz wurde erfolgreich geladen....");

  // Initialisieren der Schildbilder
  GetShieldBlocks ();

  DebugPrintf
    ("\nvoid InitParaplus(void): GetShieldBlocks(void) ist fehlerfrei zurueckgekehrt....");

  /* richtige Paletten-Werte einstellen */
  InitPalette ();

  /* Tastaturwiederholrate auf den geringsten Wert setzen */
  SetTypematicRate (TYPEMATIC_SLOW);

  /* Initialisierung beendet. Monitor wird aktiviert. */
  DebugPrintf
    ("\nvoid InitParaplus(void): Funktionsende fehlerfrei erreicht....");

}				// void InitParaplus(void)


/*@Function============================================================
@Desc: Diese Prozedur ist fuer die Introduction in das Spiel verantwortlich. Im
   Moment beschrÑnkt sich ihre Funktion auf das Laden und anzeigen eines
   Titelbildes, das dann ausgeblendet wird.

@Ret: keiner
@Int: keiner
* $Function----------------------------------------------------------*/
void
Title (void)
{
  int ScrollEndLine = USERFENSTERPOSY;	/* Endpunkt des Scrollens */
  int OldUpdateStatus = InterruptInfolineUpdate;

  DebugPrintf ("\nvoid Title(void): real function call confirmed...:");

  Switch_Background_Music_To (CLASSICAL_BEEP_BEEP_BACKGROUND_MUSIC);

  InterruptInfolineUpdate = FALSE;
#ifdef NOJUNKWHILEINIT
  Monitorsignalunterbrechung (0);
#endif

  // LadeLBMBild(TITELBILD1,RealScreen,FALSE);  /* Titelbild laden */
  Load_PCX_Image (TITELBILD1_PCX, RealScreen, TRUE);	/* Titelbild laden */

  PrepareScaledSurface();

  while (!SpacePressed ());
  while (SpacePressed());


  FadeColors1 ();		/* Titelbild langsam ausblenden */

  InitPalette ();		/* This function writes into InternalScreen ! */

  // ClearGraphMem(RealScreen);
  // DisplayRahmen(RealScreen);

  Load_PCX_Image (RAHMENBILD1_PCX, RealScreen, FALSE);	/* Titelbild laden */


  SetTextBorder (USERFENSTERPOSX, USERFENSTERPOSY,
		 USERFENSTERPOSX + USERFENSTERBREITE,
		 USERFENSTERPOSY + USERFENSTERHOEHE, CHARSPERLINE);

  SetTextColor (FONT_BLACK, FONT_RED);

  // *          Auskommentiert zu Testzwecken
  // *

  ScrollText (TitleText1, SCROLLSTARTX, SCROLLSTARTY, ScrollEndLine);
  ScrollText (TitleText2, SCROLLSTARTX, SCROLLSTARTY, ScrollEndLine);
  ScrollText (TitleText3, SCROLLSTARTX, SCROLLSTARTY, ScrollEndLine);
  ScrollText (TitleText4, SCROLLSTARTX, SCROLLSTARTY, ScrollEndLine);

  SetTextBorder (0, 0, SCREENBREITE, SCREENHOEHE, 40);

  SetTypematicRate (TYPEMATIC_SLOW);
  InterruptInfolineUpdate = OldUpdateStatus;

  DebugPrintf ("\nvoid Title(void): end of function reached...:");

}				// void Title(void)

/*@Function============================================================
@Desc: Diese Prozedur ist fuer die Introduction in das Spiel verantwortlich. Im
   Moment beschrÑnkt sich ihre Funktion auf das Laden und anzeigen eines
   Titelbildes, das dann ausgeblendet wird.

@Ret: keiner
@Int: keiner
* $Function----------------------------------------------------------*/
void
EndTitle (void)
{
  int ScrollEndLine = USERFENSTERPOSY;	/* Endpunkt des Scrollens */
  int OldUpdateStatus = InterruptInfolineUpdate;

  DebugPrintf ("\nvoid EndTitle(void): real function call confirmed...:");

  Switch_Background_Music_To (CLASSICAL_BEEP_BEEP_BACKGROUND_MUSIC);

  InterruptInfolineUpdate = FALSE;
#ifdef NOJUNKWHILEINIT
  Monitorsignalunterbrechung (0);
#endif

  // LadeLBMBild(TITELBILD1,RealScreen,FALSE);  /* Titelbild laden */
  // Load_PCX_Image (TITELBILD1_PCX, RealScreen, TRUE);	/* Titelbild laden */

  // while (!SpacePressed ())
  //JoystickControl ();

  // FadeColors1 ();		/* Titelbild langsam ausblenden */

  // InitPalette ();		/* This function writes into InternalScreen ! */

  // ClearGraphMem(RealScreen);
  // DisplayRahmen(RealScreen);

  Load_PCX_Image (RAHMENBILD1_PCX, RealScreen, FALSE);	/* Titelbild laden */

  SetTextBorder (USERFENSTERPOSX, USERFENSTERPOSY,
		 USERFENSTERPOSX + USERFENSTERBREITE,
		 USERFENSTERPOSY + USERFENSTERHOEHE, CHARSPERLINE);

  SetTextColor (FONT_BLACK, FONT_RED);

  // *          Auskommentiert zu Testzwecken
  // *

  ScrollText (EndTitleText1, SCROLLSTARTX, SCROLLSTARTY, ScrollEndLine);
  ScrollText (EndTitleText2, SCROLLSTARTX, SCROLLSTARTY, ScrollEndLine);
  // ScrollText (TitleText3, SCROLLSTARTX, SCROLLSTARTY, ScrollEndLine);

  SetTextBorder (0, 0, SCREENBREITE, SCREENHOEHE, 40);

  SetTypematicRate (TYPEMATIC_SLOW);
  InterruptInfolineUpdate = OldUpdateStatus;

  DebugPrintf ("\nvoid EndTitle(void): end of function reached...:");

} // void EndTitle(void)

#undef _parainit_c
