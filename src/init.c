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
#include <vga.h>
#include <vgagl.h>
#include <vgakeyboard.h>
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


char TitleText1[] =
  "A fleet of Robo-freighters on\nits way to the Beta Ceti system\nreported entering an uncharted field\nof asteroids. Each ship carries a\ncargo of battle droids to reinforce\nthe outworld defences.\nTwo distress beacons have been\ndiscovered. Similar Messages were\nstored on each. The ships had been\nbombarded by a powerful radionic\nbeam from one of the asteroids.\nAll of the robots on the ships,\nincluding those in storage, became\nhyper-active. The crews report an\nattack by droids, isolating them on\nthe bridge. They cannot reach the\nshuttle and can hold out for only a\ncouple more hours.\n Since these beacons were located\ntwo days ago, we can only fear the\nworst.\n Some of the fleet was last seen\nheading for enemy space. In enemy\nhands the droids can be used against\nour forces.\nDocking would be impossible but\nwe can beam aboard a prototype\nInfluence Device.";


char TitleText2[] =
  "The 001 Influence Device consists\nof a helmet, which, when placed\nover a robots control unit can halt\nthe normal activities of that robot\nfor a short time. The helmet has\nits own energy supply and powers\nthe robot itself, at an upgraded\ncapability. The helmet also uses\nan energy cloak for protection of\nthe host.\nThe helmet is fitted with twin\nlasers mounted in a turret. These\ncan be focussed on any target inside\na range of eight metres.\nMost of the device's resources are\nchannelled towards holding control\nof the host robot, as it attempts\nto resume 'normal' operation.\nIt is therefore necessary to change\nthe host robot often to prevent the\ndevice from burning out. Transfer\nto a new robot requires the device\nto drain its host of energy in order\nto take ist over. Failure to achieve\ntransfer results in the device being\na free agent once more.\n\n        Press fire to play";

char TitleText3[] =
  "An Influence Device can transmitt\nconsole. A small-scale plan of the\nwhole deck is available, as well\nas a side elevation of the ship.\nRobots are represented on-screen\nas a symbol showing a three-digit\nnumber. The first digit shown is\nthe important one, the class of the\nrobot. It denotes the strength also.\nTo find out more about any given\nrobot, use the robot enquiry system\nat a console. Only data about units\nof a lower class than your current\nhost is available, since it is the\nhost's security clearance which is\nused to acces the console. \n  \n  \n  \n Press fire to play";


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
			[-t|--timeout=SECONDS]\n
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
    {"help", 0, 0, 'h'},
    {"nosound", 0, 0, 'q'},
    {"sound", 0, 0, 's'},
    {"timeout", 1, 0, 't'},
    {0, 0, 0, 0}
  };

  while (1)
    {
      c = getopt_long (argc, argv, "vqst:h?", long_options, NULL);
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

  InterruptInfolineUpdate = TRUE;
  LastBlastHit = 0;
  LastGotIntoBlastSound = 2;
  LastRefreshSound = 2;
  BeamLine = BLOCKBREITE;
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
  for (i = 0; i < curShip.LevelsOnShip; i++)
    curShip.AllLevels[i]->empty = FALSE;

  DebugPrintf
    ("\nvoid InitNewGame(void): All levels have been set to 'active'...");

  i = MyRandom (3);
  switch (i)
    {
    case 0:
      {
	Me.pos.x = 120;
	Me.pos.y = 48;
	CurLevel = curShip.AllLevels[4];
	break;
      }
    case 1:
      {
	Me.pos.x = 120;
	Me.pos.y = 48;
	CurLevel = curShip.AllLevels[5];
	break;
      }
    case 2:
      {
	Me.pos.x = 120;
	Me.pos.y = 48;
	CurLevel = curShip.AllLevels[6];
	break;
      }
    case 3:
      {
	Me.pos.x = 120;
	Me.pos.y = 48;
	CurLevel = curShip.AllLevels[7];
	break;
      }
    case 4:
      {
	DebugPrintf
	  ("\nvoid InitNewGame(void): ERROR: Impossible case 4 encountered.  Terminating...");
	Terminate (ERR);
	break;
      }
    }

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

  DebugPrintf ("\nvoid InitParaplus(void) wurde echt aufgerufen....\n");

  Set_SVGALIB_Video_ON ();

  Init_YIFF_Sound_Server ();

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
    "robot activity\ninfluence device. This\nhelmet is self-powered\nand will control any\nrobot for a short time.\nLasers are turret‹mounted.\n";
  Druidmap[DRUID123].notes =
    "simpe rubbish\ndiposal robot. Common\ndevice in most space\ncraft to maintain a clean\nship.\n";
  Druidmap[DRUID139].notes =
    "created by Dr.\nMasternak to clean up\nlarge heaps of rubbish.\nIts large scoop is used to\ncollect rubbish. It is\nthen crushed internally.\n";
  Druidmap[DRUID247].notes =
    "light duty servant\nrobot. One of the first\nto use the anti-grav\nsystem.\n";
  Druidmap[DRUID249].notes =
    "cheaper version of\nthe anti-grav servant\nrobot.\n";
  Druidmap[DRUID296].notes =
    "this robot is used\nmainly for serving drinks.\nA tray is mounted on the\nhead. Built by Orchard\nand Marsden Enterprises.\n";
  Druidmap[DRUID302].notes =
    "common device\nfor moving small\npackages. Clamp is\nmounted on the lower\nbody.\n";
  Druidmap[DRUID329].notes =
    "early type\nmessenger robot. Large\nwheels impede motion on\nsmall craft.an";
  Druidmap[DRUID420].notes =
    "slow maintenance\nrobot. Confined to drive\nmaintenance during flight.\n";
  Druidmap[DRUID476].notes =
    "ship maintenance\nrobot. Fitted with\nmultiple arms to carry\nout repairs to the ship\nefficiently. All craft\nbuilt after the Jupiter‹incident are supplied\nwith a team of these.\n";
  Druidmap[DRUID493].notes =
    "slave maintenance\ndroid. Standard version\nwill carry its own\ntoolbox.\n";
  Druidmap[DRUID516].notes =
    "early crew droid.\nAble to carry out simple\nflight checks only. No\nlonger supplied.\n";
  Druidmap[DRUID571].notes =
    "standard crew\ndroid. Supplied with the\nship.\n";
  Druidmap[DRUID598].notes =
    "a highly\nsophisticated device.\nAble to control the\nRobo-Freighter on its\nown.\n";
  Druidmap[DRUID614].notes =
    "low security\nsentinel droid. Used to\nprotect areas of the ship\nfrom intruders. A slow\nbut sure device.\n";
  Druidmap[DRUID615].notes =
    "sophisticated\nsentinel droid. Only 2000\nbuilt by the Nicholson\nCompany. these are now\nvery rare.";
  Druidmap[DRUID629].notes =
    "low sentinel\ndroid. Lasers are built\ninto the turret. These\nare mounted on a small\ntank body. May be fitted\nwith an auto-cannon on‹the Gillen version.\n";
  Druidmap[DRUID711].notes =
    "heavy duty battle\ndroid. Disruptor is built\ninto the head. One of the\nfirst in service with the\nMilitary.\n";
  Druidmap[DRUID742].notes =
    "this version is\nthe one mainly used by\nthe Military.\n";
  Druidmap[DRUID751].notes =
    "very heavy duty\nbattle droid. Only a few\nhave so far entered\nservice. These are the\nmost powerful battle\nunits ever built.\n";
  Druidmap[DRUID821].notes =
    "a very reliable\nanti-grav unit is fitted\ninto this droid. It will\npatrol the ship and\neliminate intruders as\nsoon as detected by‹powerful sensors.\n";
  Druidmap[DRUID834].notes =
    "early type\nanti-grav security droid.\nFitted with an\nover-driven anti-grav unit.\nThis droid is very fast\nbut is not reliable.\n";
  Druidmap[DRUID883].notes =
    "this droid was\ndesigned from archive\ndata. For some unknown\nreason it instils great\nfear in Human\nadversaries.\n";
  Druidmap[DRUID999].notes =
    "experimental\ncommand cyborg. Fitted\nwith a new tipe of\nbrain. Mounted on a\nsecurity droid anti-grav\nunit for convenience.‹warning: the influence\ndevice may not control a\nprimode brain for long.\n";
  IntroMSG1 =
    "Dies symbolisiert den Text, der zu Beginn des Spiels ausgegeben werden soll. Nach einer gewissen Zeit soll der Text nach unten weiterscrolloen und zwar in einer moeglichst fliessenden Form.";

  GameAdapterPresent = FALSE;	/* start with this */
  taste = 255;

  /* Sounds on/off */
  ModPlayerOn = FALSE;

  DebugPrintf
    ("\nvoid InitParaplus(void): Textmeldungsvariablen wurden erfolgreich initialisiert....");

  /* ScreenPointer setzen */
  // PORT RealScreen = MK_FP(SCREENADDRESS, 0);
  RealScreen = malloc (SCREENBREITE * SCREENHOEHE + 10);
  InternalScreen = (unsigned char *) malloc (SCREENHOEHE * SCREENBREITE + 10);

  DebugPrintf
    ("\nvoid InitParaplus(void): Realscreen und Internalscreen haben erfolgreich Speicher erhalten....");

  /* Zufallsgenerator initialisieren */
  //PORT MyRandomize();

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

  while (!SpacePressed ())
    JoystickControl ();

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

  SetTextBorder (0, 0, SCREENBREITE, SCREENHOEHE, 40);

  SetTypematicRate (TYPEMATIC_SLOW);
  InterruptInfolineUpdate = OldUpdateStatus;

  DebugPrintf ("\nvoid Title(void): end of function reached...:");

}				// void Title(void)

#undef _parainit_c
