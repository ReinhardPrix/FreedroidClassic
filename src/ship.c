/*----------------------------------------------------------------------
 *
 * Desc: the konsole- and elevator functions
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
#define _ship_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"
#include "paratext.h"
#include "ship.h"


int NoKeyPressed (void);

void GreatDruidShow (void);

void ShowElevators (void);

void AlleElevatorsGleichFaerben (void);

void PaintConsoleMenu (void);

int WaitElevatorCounter = 0;



/*-----------------------------------------------------------------
 * @Desc: does all the work when we enter an elevator
 * 
 *-----------------------------------------------------------------*/
void
EnterElevator (void)
{
  int i;
  int curLevel;
  int curElev, upElev, downElev, row;

  DebugPrintf ("\nvoid EnterElevator(void): Function call confirmed.");

  /* Prevent distortion of framerate by the delay coming from 
   * the time spend in the menu. */
  Activate_Conservative_Frame_Computation();

  /* Prevent the influ from coming out of the elevator in transfer mode
   * by turning off transfer mode as soon as the influ enters the elevator */
  Me.status= ELEVATOR;

  UpdateInfoline();
  curLevel = CurLevel->levelnum;

  if ((curElev = GetCurrentElevator ()) == -1)
    {
      printf ("Elevator out of order, I'm so sorry !");
      return;
    }

  EnterElevatorSound ();
  FadeLevel ();

  upElev = curShip.AllElevators[curElev].up;
  downElev = curShip.AllElevators[curElev].down;

  row = curShip.AllElevators[curElev].elevator_row;

  ShowElevators ();
  HilightElevator (row);

  PrepareScaledSurface(TRUE);

  /* Warten, bis User Feuer auslaesst */
  while (SpacePressed ())
    {
      JoystickControl ();
      keyboard_update ();
    }

  while (!SpacePressed ())
    {
      JoystickControl ();	/* Falls vorhanden einlesen */
      keyboard_update ();
      // if( kbhit() ) getchar();         /* Tastaturuffer leeren */

      if (UpPressed () && !DownPressed ())
	if (upElev != -1)
	  {			/* gibt es noch einen Lift hoeher ? */
	    if (curShip.AllElevators[upElev].x == 99)
	      {
		gotoxy (1, 1);
		printf ("Out of order, so sorry ..");
	      }
	    else
	      {
		downElev = curElev;
		curElev = upElev;
		curLevel = curShip.AllElevators[curElev].level;
		upElev = curShip.AllElevators[curElev].up;

		AlleLevelsGleichFaerben ();
		HilightLevel (curLevel);	/* highlight new level */

		/* Warten, bis user Taste auslaesst */
		WaitElevatorCounter = WAIT_ELEVATOR;
		MoveElevatorSound ();
		//PORT while( UpPressed() && WaitElevatorCounter ) {
		while (UpPressed ())
		  {
		    JoystickControl ();
		    keyboard_update ();
		  }
	      }
	  }			/* if uplevel */


      if (DownPressed () && !UpPressed ())
	if (downElev != -1)
	  {			/* gibt es noch einen Lift tiefer ? */
	    if (curShip.AllElevators[downElev].x == 99)
	      {
		gotoxy (1, 1);
		printf ("Out of order, so sorry ..");
	      }
	    else
	      {
		upElev = curElev;
		curElev = downElev;
		curLevel = curShip.AllElevators[curElev].level;
		downElev = curShip.AllElevators[curElev].down;

		AlleLevelsGleichFaerben ();
		HilightLevel (curLevel);

		/* Warten, bis User Taste auslaesst */
		WaitElevatorCounter = WAIT_ELEVATOR;
		MoveElevatorSound ();
		// PORT while( DownPressed() && WaitElevatorCounter ) {
		while (DownPressed ())
		  {
		    JoystickControl ();
		    keyboard_update ();
		  }
	      }
	  }			/* if downlevel */
    }				/* while !SpaceReleased */

  /* Neuen Level und Position setzen */
  if (curLevel != CurLevel->levelnum)
    {				/* wirklich neu ??? */
      int array_num = 0;
      Level tmp;

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
      Me.pos.x =
	curShip.AllElevators[curElev].x * BLOCKBREITE + BLOCKBREITE / 2;
      Me.pos.y =
	curShip.AllElevators[curElev].y * BLOCKHOEHE + BLOCKHOEHE / 2;

      /* Alle Blasts und Bullets loeschen */
      for (i = 0; i < MAXBLASTS; i++)
	AllBlasts[i].type = OUT;
      for (i = 0; i < MAXBULLETS; i++)
	{
	  AllBullets[i].type = OUT;
	  AllBullets[i].mine = FALSE;
	}

    }				/* if neuer Level */

  LeaveElevatorSound ();
  ClearGraphMem (RealScreen);
  DisplayRahmen (RealScreen);

  UnfadeLevel ();

  /* Wenn Level leer: grau faerben */
  if (CurLevel->empty)
    LevelGrauFaerben ();

  InitBars = TRUE;

  while (SpacePressed ())
    {
      JoystickControl ();
      keyboard_update ();
    }

  Me.status = MOBILE;

  DebugPrintf ("\nvoid EnterElevator(void): Usual end of function reached.");
}				/* EnterElevator */

/*-----------------------------------------------------------------
 * @Desc: show elevator view of the ship, and hightlight the current 
 *        level + lift
 *
 *
 *-----------------------------------------------------------------*/
void
ShowElevators (void)
{
  int curLevel = CurLevel->levelnum;

  DebugPrintf ("\nvoid ShowElevators(void): real function call confirmed.");

  /* Zuerst Screen loeschen (InternalScreen) */
  ClearGraphMem (RealScreen);
  
  /* Userfenster faerben */
  SetUserfenster (EL_BG_COLOR, RealScreen);
  DisplayRahmen (RealScreen);	/* Rahmen dazu */
  SayLeftInfo (LeftInfo, RealScreen);
  SayRightInfo (RightInfo, RealScreen);

  DisplayBlock (USERFENSTERPOSX, USERFENSTERPOSY, ElevatorPicture, 
		USERFENSTERBREITE, USERFENSTERHOEHE, RealScreen);

  AlleLevelsGleichFaerben ();
  AlleElevatorsGleichFaerben ();

  HilightLevel (curLevel);

  PrepareScaledSurface(TRUE);

  DebugPrintf ("\nvoid ShowElevators(void): end of function reached.");
  return;
} /* ShowElevators() */


/*@Function============================================================
@Desc: EnterKonsole(): does all konsole- duties
This function runs the consoles. This means the following duties:
	2	* Show a small-scale plan of the current deck
	3	* Show a side-elevation on the ship
	1	* Give all available data on lower druid types
	0	* Reenter the game without squashing the colortable
@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
EnterKonsole (void)
{
  int MenuPoint = 0;
  int ReenterGame = 0;
  int TasteOK;

  DebugPrintf ("\nvoid EnterKonsole(void): real function call confirmed.");

  // Prevent distortion of framerate by the delay coming from 
  // the time spend in the menu.
  Activate_Conservative_Frame_Computation();

  Me.status = CONSOLE;

  Switch_Background_Music_To (CONSOLE_BACKGROUND_MUSIC_SOUND);
  DebugPrintf
    ("\nvoid EnterKonsole(void):  Console background music started.");

  /* Initialisierung der Konsole */

  // InterruptInfolineUpdate = FALSE;
  ClearGraphMem (RealScreen);
  KillTastaturPuffer ();

  while (SpacePressed ())
    {
      keyboard_update ();
      JoystickControl ();
    }

  /* Gleich zu Beginn die Farben richtig setzten */
  if (MenuPoint == 0)
    SetPalCol (M1C, HR, HG, HB);
  else
    SetPalCol (M1C, GR, GG, GB);
  if (MenuPoint == 1)
    SetPalCol (M2C, HR, HG, HB);
  else
    SetPalCol (M2C, GR, GG, GB);
  if (MenuPoint == 2)
    SetPalCol (M3C, HR, HG, HB);
  else
    SetPalCol (M3C, GR, GG, GB);
  if (MenuPoint == 3)
    SetPalCol (M4C, HR, HG, HB);
  else
    SetPalCol (M4C, GR, GG, GB);

  /* Gesamtkonsolenschleife */

  while (!ReenterGame)
    {

      PaintConsoleMenu ();
      PrepareScaledSurface(TRUE);

      /* Nichts tun bis eine vern"unftige Taste gedr"uckt wurde */
      TasteOK = 0;
      while (!TasteOK)
	{
	  JoystickControl ();
	  if (UpPressed ())
	    {
	      MenuPoint--;
	      TasteOK = 1;
	    }
	  if (DownPressed ())
	    {
	      MenuPoint++;
	      TasteOK = 1;
	    }
	  if (SpacePressed ())
	    TasteOK = 1;
	}

      /* Verhindern, da"s der Menucursor das Menu verl"a"st */
      if (MenuPoint < 0)
	MenuPoint = 0;
      if (MenuPoint > 3)
	MenuPoint = 3;

      /* Anzeigen des aktuellen Menupunktes durch Palettenwertsetzen */
      if (MenuPoint == 0)
	SetPalCol (M1C, HR, HG, HB);
      else
	SetPalCol (M1C, GR, GG, GB);
      if (MenuPoint == 1)
	SetPalCol (M2C, HR, HG, HB);
      else
	SetPalCol (M2C, GR, GG, GB);
      if (MenuPoint == 2)
	SetPalCol (M3C, HR, HG, HB);
      else
	SetPalCol (M3C, GR, GG, GB);
      if (MenuPoint == 3)
	SetPalCol (M4C, HR, HG, HB);
      else
	SetPalCol (M4C, GR, GG, GB);

      /* gew"ahlte Menupunkte betreten */
      if ((MenuPoint == 0) & (SpacePressed ()))
	ReenterGame = TRUE;
      if ((MenuPoint == 1) & (SpacePressed ()))
	GreatDruidShow ();
      if ((MenuPoint == 2) & (SpacePressed ()))
	{
	  ShowDeckMap (CurLevel);
	  /* this is not very elegant at the moment, but it works ok.. */
	  while ( SpacePressed() );  /* wait for space-release */
	  while (!SpacePressed () ); /* and wait for another space before leaving */
	  while ( SpacePressed() ); /* but also wait for the release before going on..*/
	}
      if ((MenuPoint == 3) & (SpacePressed ()))
	{
	  while (SpacePressed ())
	    JoystickControl ();
	  ShowElevators ();
	  while (!SpacePressed ())
	    JoystickControl ();
	  while (SpacePressed ())
	    JoystickControl ();
	  KillTastaturPuffer ();
	}

      while (DownPressed ())
	JoystickControl ();
      while (UpPressed ())
	JoystickControl ();

    }				/* (while !ReenterGane) */

  KillTastaturPuffer ();
  Me.status = MOBILE;
  /* Die Textfarben wieder setzen wie sie vorher waren */
  SetTextColor (FONT_WHITE, FONT_RED);	/* BG: Rahmenwei"s FG: FONT_RED */
  UpdateInfoline ();
  // InterruptInfolineUpdate = TRUE;
  // InitBars=TRUE;

  while (SpacePressed ())
    {
      keyboard_update ();
      JoystickControl ();
    }

  Switch_Background_Music_To ( COMBAT_BACKGROUND_MUSIC_SOUND );

  DebugPrintf
	("\nvoid EnterKonsole(void):  Console background replaced by combat background music.");
  
  DebugPrintf
    ("\nvoid EnterKonsole(void): Normal end of function reached.\n\n");

} // void EnterKonsole(void)

/*@Function============================================================
@Desc: diese Funktion zeigt die m"oglichen Auswahlpunkte des Menus
       Sie soll die Schriftfarben nicht ver"andern

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
PaintConsoleMenu (void)
{
  char MenuText[200];
  unsigned int fg, bg;

  DebugPrintf ("\nvoid PaintConsoleMenu(void): Real function called.");

  ClearGraphMem (InternalScreen);
  GetTextColor (&bg, &fg);	/* Diese Funktion soll die Schriftfarben nicht ver"andern */

  DisplayRahmen (InternalScreen);

  /* Darstellung des oberen Rahmen mit Inhalt */
  //   SetInfoline();
  // strcpy(LeftInfo, "Console");

  /* Userfenster faerben */
  SetUserfenster (KON_BG_COLOR, InternalScreen);
  // ClearUserFenster();

  SetTextColor (RAHMEN_BG_COLOR, FONT_RED);	/* BG: Rahmenwei"s FG: FONT_RED */
  SayLeftInfo (LeftInfo, InternalScreen);
  SayRightInfo (RightInfo, InternalScreen);

  /* Konsolen-Menue Farbe setzen */
  SetTextColor (KON_BG_COLOR, KON_TEXT_COLOR);

  DebugPrintf
    ("\nvoid PaintConsoleMenu(void): Now the internals of the console are written to the screen....");

  SetTextBorder (MENUTEXT_X, USERFENSTERPOSY,
		 USERFENSTERPOSX + USERFENSTERBREITE,
		 USERFENSTERPOSY + USERFENSTERHOEHE, 30);

  // ClearUserFenster();
  // SetUserfenster(KON_BG_COLOR, InternalScreen);

  strcpy (MenuText, "Unit type ");
  strcat (MenuText, Druidmap[Me.type].druidname);
  strcat (MenuText, " - ");
  strcat (MenuText, Classname[Druidmap[Me.type].class]);
  DisplayText (MenuText, USERFENSTERPOSX, USERFENSTERPOSY, InternalScreen,
	       FALSE);


  DebugPrintf
    ("\nvoid PaintConsoleMenu(void): Now the screen will be prepared for the real menu....");

  strcpy (MenuText, "\nAccess granted.\nShip : ");
  strcat (MenuText, Shipnames[ThisShip]);
  strcat (MenuText, "\nDeck : ");
  strcat (MenuText, Decknames[CurLevel->levelnum]);
  strcat (MenuText, "\n\nAlert: ");
  strcat (MenuText, Alertcolor[Alert]);

  DisplayText (MenuText, MENUTEXT_X, USERFENSTERPOSY + 15, InternalScreen,
	       FALSE);

  DebugPrintf
    ("\nvoid PaintConsoleMenu(void): Now the imaged are about to be displayed....");

  SetTextBorder (0, 0, SCREENBREITE, SCREENHOEHE, 40);

  /*
   * Hier werden die Icons des Menus ausgegeben
   *
   */

  DisplayBlock (MENUITEMPOSX, MENUITEMPOSY + FONTHOEHE + BLOCKHOEHE - 4,
		MenuItemPointer,
		MENUITEMLENGTH, MENUITEMHEIGHT, InternalScreen);

  DebugPrintf
    ("\nvoid PaintConsoleMenu(void): Now the Influence will be drawn to the menu....");

  DisplayMergeBlock (MENUITEMPOSX + 10, MENUITEMPOSY + FONTHOEHE,
		     Influencepointer + BLOCKMEM * ((int) rintf (Me.phase)),
		     BLOCKBREITE, BLOCKHOEHE, InternalScreen);

  SwapScreen ();

  DebugPrintf
    ("\nvoid PaintConsoleMenu(void): Now the Info-Line will be updated....");

  UpdateInfoline ();
  SetTextColor (bg, fg);

  DebugPrintf
    ("\nvoid PaintConsoleMenu(void): Usual end of function reached.");
}				// void PaintConsoleMenu(void)

/*@Function============================================================
@Desc: Zeigt alle erlaubten Roboter.

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
GreatDruidShow (void)
{
  char InfoText[10000];
  int Infodroid;
  char PassOn = 0;

  DebugPrintf ("\nvoid GreadDruidShow(void): Function call confirmed.");

  /* Warte, bis User Space auslaesst */
  while (SpacePressed ())
    {
      JoystickControl ();
      keyboard_update ();
    }

  SetUserfenster (KON_BG_COLOR, InternalScreen);

  SetTextColor (KON_BG_COLOR, FONT_BLUE);	// RED // YELLOW

  /*
   * Beginn der gro"sen "Ubersicht "uber alle Roboter
   *
   */

  for (Infodroid = Me.type; Infodroid > -1;)
    {

      ClearUserFenster ();

      /*
       * Ausgabe der ersten Zeile, die den Druidtyp beschreibt
       *
       */

      // PORT KillTastaturPuffer();
      PassOn = 0;
      while (!PassOn)
	{

	  // Am Bildschirm anzeigen
	  ClearUserFenster ();
	  strcpy (InfoText, "Unit type ");
	  strcat (InfoText, Druidmap[Infodroid].druidname);
	  strcat (InfoText, " - ");
	  strcat (InfoText, Classname[Druidmap[Infodroid].class]);

	  SetTextBorder (USERFENSTERPOSX, USERFENSTERPOSY, USERFENSTERPOSX +
			 USERFENSTERBREITE,
			 USERFENSTERPOSY + USERFENSTERHOEHE, 36);
	  DisplayText (InfoText, USERFENSTERPOSX, USERFENSTERPOSY, RealScreen,
		       FALSE);
	  ShowRobotPicture (USERFENSTERPOSX, USERFENSTERPOSY + 2 * FONTHOEHE,
			    Infodroid, RealScreen);

	  Update_SDL_Screen();
	  PrepareScaledSurface(TRUE);

	  KillTastaturPuffer ();
	  while (!LeftPressed () && !UpPressed () && !DownPressed ()
		 && !RightPressed () && !SpacePressed ())
	    JoystickControl ();

	  if (UpPressed ())
	    {
	      Infodroid--;
	      // Einem zu schnellen Weiterbl"attern vorbeugen
	      while (UpPressed () || DownPressed ())
		{
		  JoystickControl ();
		  keyboard_update ();
		}
	    }

	  if (DownPressed ())
	    {
	      Infodroid++;
	      // Einem zu schnellen Weiterbl"attern vorbeugen
	      while (UpPressed () || DownPressed ())
		{
		  JoystickControl ();
		  keyboard_update ();
		}
	    }

	  if ((RightPressed ()) || (LeftPressed ()))
	    PassOn = 1;

	  if (Infodroid > Me.type)
	    Infodroid = DRUID001;
	  if (Infodroid < DRUID001)
	    Infodroid = Me.type;
	  if (SpacePressed ())
	    {
	      KillTastaturPuffer ();
	      while (SpacePressed ())
		{
		  JoystickControl ();
		  keyboard_update ();
		}
	      SetTextBorder (0, 0, SCREENBREITE, SCREENHOEHE, 40);
	      DebugPrintf
		("\nvoid GreadDruidShow(void): Alternate end of function reached via Space0.");
	      return;
	    }

	}
      while (LeftPressed () || UpPressed () || DownPressed ()
	     || RightPressed () || SpacePressed ())
	JoystickControl ();

      /*
       * Ausgabe der Liste von Werten dieses Druids
       *
       */

      ClearUserFenster ();
      strcpy (InfoText, "Unit type ");
      strcat (InfoText, Druidmap[Infodroid].druidname);
      strcat (InfoText, " - ");
      strcat (InfoText, Classname[Druidmap[Infodroid].class]);

      SetTextBorder (USERFENSTERPOSX, USERFENSTERPOSY, USERFENSTERPOSX +
		     USERFENSTERBREITE, USERFENSTERPOSY + USERFENSTERHOEHE,
		     36);
      DisplayText (InfoText, USERFENSTERPOSX, USERFENSTERPOSY, RealScreen,
		   FALSE);
      ShowRobotPicture (USERFENSTERPOSX, USERFENSTERPOSY + 2 * FONTHOEHE,
			Infodroid, RealScreen);

      strcpy (InfoText, "Entry : ");
      strcat (InfoText, Entry[Infodroid]);
      strcat (InfoText, "\nClass : ");
      strcat (InfoText, Classes[Druidmap[Infodroid].class]);
      strcat (InfoText, "\nHeight: ");
      strcat (InfoText, Height[Infodroid]);
      strcat (InfoText, "\nWeight: ");
      strcat (InfoText, Weight[Infodroid]);
      strcat (InfoText, "\nDrive : ");
      strcat (InfoText, Drivenames[Drive[Infodroid]]);
      strcat (InfoText, "\nBrain : ");
      strcat (InfoText, Brainnames[Brain[Infodroid]]);

      SetTextBorder (MENUTEXT_X, USERFENSTERPOSX, USERFENSTERPOSX +
		     USERFENSTERBREITE, USERFENSTERHOEHE + USERFENSTERPOSY,
		     30);
      DisplayText (InfoText, MENUTEXT_X, USERFENSTERPOSY + 17, RealScreen,
		   FALSE);

      Update_SDL_Screen();
      PrepareScaledSurface(TRUE);

      KillTastaturPuffer ();
      PassOn = 0;
      while (!PassOn)
	{
	  JoystickControl ();
	  if ((RightPressed ()) || (LeftPressed ()) || (UpPressed ())
	      || (DownPressed ()))
	    {
	      PassOn = 1;
	      while ((RightPressed ()) || (LeftPressed ()) || (UpPressed ())
		     || (DownPressed ()))
		JoystickControl ();
	    }
	  if (SpacePressed ())
	    {
	      KillTastaturPuffer ();
	      while (SpacePressed ())
		JoystickControl ();
	      SetTextBorder (0, 0, SCREENBREITE, SCREENHOEHE, 40);
	      DebugPrintf
		("\nvoid GreadDruidShow(void): Alternate end of function reached via Space1.");
	      return;
	    }
	}


      /*
       * Ausgabe der Liste von Ausr"ustung dieses Druids
       *
       */

      ClearUserFenster ();
      strcpy (InfoText, "Unit type ");
      strcat (InfoText, Druidmap[Infodroid].druidname);
      strcat (InfoText, " - ");
      strcat (InfoText, Classname[Druidmap[Infodroid].class]);

      SetTextBorder (USERFENSTERPOSX, USERFENSTERPOSY, USERFENSTERPOSX +
		     USERFENSTERBREITE, USERFENSTERPOSY + USERFENSTERHOEHE,
		     36);
      DisplayText (InfoText, USERFENSTERPOSX, USERFENSTERPOSY, RealScreen,
		   FALSE);
      ShowRobotPicture (USERFENSTERPOSX, USERFENSTERPOSY + 2 * FONTHOEHE,
			Infodroid, RealScreen);

      strcpy (InfoText, "Armamant : ");
      strcat (InfoText, Weaponnames[Armament[Infodroid]]);
      strcat (InfoText, "\nSensors  1: ");
      strcat (InfoText, Sensornames[Sensor1[Infodroid]]);
      strcat (InfoText, "\n          2: ");
      strcat (InfoText, Sensornames[Sensor2[Infodroid]]);
      strcat (InfoText, "\n          3: ");
      strcat (InfoText, Sensornames[Sensor3[Infodroid]]);

      SetTextBorder (MENUTEXT_X, USERFENSTERPOSX, USERFENSTERPOSX +
		     USERFENSTERBREITE, USERFENSTERHOEHE + USERFENSTERPOSY,
		     30);
      DisplayText (InfoText, MENUTEXT_X, USERFENSTERPOSY + 17, RealScreen,
		   FALSE);

      Update_SDL_Screen();
      PrepareScaledSurface(TRUE);

      KillTastaturPuffer ();
      PassOn = 0;
      while (!PassOn)
	{
	  JoystickControl ();
	  if ((RightPressed ()) || (LeftPressed ()) || (UpPressed ())
	      || (DownPressed ()))
	    {
	      PassOn = 1;
	      while ((RightPressed ()) || (LeftPressed ()) || (UpPressed ())
		     || (DownPressed ()))
		JoystickControl ();
	    }
	  if (SpacePressed ())
	    {
	      KillTastaturPuffer ();
	      while (SpacePressed ())
		JoystickControl ();
	      SetTextBorder (0, 0, SCREENBREITE, SCREENHOEHE, 40);
	      DebugPrintf
		("\nvoid GreadDruidShow(void): Alternate end of function reached via Space2.");
	      return;
	    }
	}


      /*
       * Ausgabe der Informationen bezuglich des Druidhintergrundes
       *
       */

      ClearUserFenster ();
      strcpy (InfoText, "Unit type ");
      strcat (InfoText, Druidmap[Infodroid].druidname);
      strcat (InfoText, " - ");
      strcat (InfoText, Classname[Druidmap[Infodroid].class]);

      SetTextBorder (USERFENSTERPOSX, USERFENSTERPOSY, USERFENSTERPOSX +
		     USERFENSTERBREITE, USERFENSTERPOSY + USERFENSTERHOEHE,
		     36);
      DisplayText (InfoText, USERFENSTERPOSX, USERFENSTERPOSY, RealScreen,
		   FALSE);
      ShowRobotPicture (USERFENSTERPOSX, USERFENSTERPOSY + 2 * FONTHOEHE,
			Infodroid, RealScreen);

      strcpy (InfoText, "Notes: ");
      strcat (InfoText, Druidmap[Infodroid].notes);

      SetTextBorder (MENUTEXT_X, USERFENSTERPOSX,
		     USERFENSTERPOSX + USERFENSTERBREITE,
		     USERFENSTERHOEHE + USERFENSTERPOSY, 30);
      DisplayText (InfoText, MENUTEXT_X, USERFENSTERPOSY + 17, RealScreen,
		   FALSE);

      Update_SDL_Screen();
      PrepareScaledSurface(TRUE);

      KillTastaturPuffer ();
      PassOn = 0;
      while (!PassOn)
	{
	  JoystickControl ();
	  if ((RightPressed ()) || (LeftPressed ()) || (UpPressed ())
	      || (DownPressed ()))
	    {
	      PassOn = 1;
	      while ((RightPressed ()) || (LeftPressed ()) || (UpPressed ())
		     || (DownPressed ()))
		JoystickControl ();
	    }
	  if (SpacePressed ())
	    {
	      KillTastaturPuffer ();
	      while (SpacePressed ())
		JoystickControl ();
	      SetTextBorder (0, 0, SCREENBREITE, SCREENHOEHE, 40);
	      DebugPrintf
		("\nvoid GreadDruidShow(void): End of function reached.");
	      return;
	    }
	}
    }				/* for */

  DebugPrintf ("\nvoid GreadDruidShow(void): End of function reached.");
}				/* GreatDruidShow() */


/*-----------------------------------------------------------------
 * @Desc: Displays the concept view of Level "deck" in Userfenster
 * 	  
 *	Note: we no longer wait here for a key-press, but return
 *            immediately 
 *-----------------------------------------------------------------*/
void
ShowDeckMap (Level deck)
{
  int i, j;
  int LX;
  int LY;

  LX = USERFENSTERPOSX;
  LY = USERFENSTERPOSY;

  ClearUserFenster ();
  for (i = 0; i < deck->ylen; i++)
    {
      for (j = 0; j < deck->xlen; j++)
	{
	  SmallBlock (LX, LY,
		      GetMapBrick (deck, j * BLOCKBREITE, i * BLOCKHOEHE),
		      RealScreen, SCREENBREITE);
	  LX += 8;  /* looks like that's our "small block" width */
	} /* for (j<xlen) */

      LX = USERFENSTERPOSX;
      LY += 8;
    } /* for (i<xlen) */

  PrepareScaledSurface(TRUE);

} /* ShowDeckMap() */


/*@Function============================================================
@Desc: Diese Funktion dient dazu, wenn die Seitenansicht des Schiffes
	gezeigt wird, alle Levels gleich zu faerben

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
AlleLevelsGleichFaerben (void)
{
  int i;

  for (i = 0; i < curShip.num_levels; i++)
    SetPalCol (EL_FIRST_LEVEL_COLOR + i, 0, 0, 30);

  return;
} /* void AlleLevelsGleichFaerben() */


/*-----------------------------------------------------------------
 * @Desc: Alle Farben der Elevators in der Seitenansicht gleich setzen 
 *
 *
 *-----------------------------------------------------------------*/
void
AlleElevatorsGleichFaerben (void)
{
  int i;
  curShip.num_lift_rows = 8;
  for (i=0; i<curShip.num_lift_rows; i++)
    SetPalCol (EL_FIRST_ROW_COLOR + i, 0, 0, 40);

  return;

} /* AlleElevatorsGleichFaerben() */



/*@Function============================================================
@Desc: Diese Funktion dient dazu, wenn die Seitenansicht des aktuellen
	Schiffes gezeigt ist, den momentanen Level als solchen zu kennzeichnen

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
HilightLevel (int Levelnummer)
{
  int rot = 47;
  int gruen = 63;
  int blau = 33;

  SetPalCol ( EL_FIRST_LEVEL_COLOR + Levelnummer, rot, gruen, blau);
}

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
HilightElevator (int ElevatorRow)
{
  int rot = 63;
  int gruen = 63;
  int blau = 63;

  SetPalCol (ElevatorRow + EL_FIRST_ROW_COLOR, rot, gruen, blau);
}

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
int
LevelEmpty (void)
{
  int i;
  int levelnum = CurLevel->levelnum;

  if (CurLevel->empty)
    return TRUE;

  for (i = 0; i < NumEnemys; i++)
    {
      if ((Feindesliste[i].Status != OUT)
	  && (Feindesliste[i].levelnum == levelnum))
	return FALSE;
    }

  CurLevel->empty = TRUE;
  RealScore += DECKCOMPLETEBONUS;
  ShowScore += DECKCOMPLETEBONUS;

  if (ShipEmpty ())
    ThouArtVictorious ();

  return TRUE;
}

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/

int
ShipEmpty (void)
{
  int i;

  for (i = 0; i < curShip.num_levels; i++)
    {
      if (curShip.AllLevels[i] == NULL)
	continue;

      if (!((curShip.AllLevels[i])->empty))
	return (FALSE);
    }
  return (TRUE);
}


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
int
NoKeyPressed (void)
{
  if (SpacePressed ())
    return (FALSE);
  if (LeftPressed ())
    return (FALSE);
  if (RightPressed ())
    return (FALSE);
  if (UpPressed ())
    return (FALSE);
  if (DownPressed ())
    return (FALSE);
  return (TRUE);
}				// int NoKeyPressed(void)


/*@Function============================================================
@Desc: l"oscht das Userfenster

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
ClearUserFenster (void)
{
  SDL_Rect ThisRectangle;

  DebugPrintf ("\nvoid ClearUserFenster(void): Real function called.");

  ThisRectangle.x=USERFENSTERPOSX;
  ThisRectangle.y=USERFENSTERPOSY;
  ThisRectangle.w=USERFENSTERBREITE;
  ThisRectangle.h=USERFENSTERHOEHE;


  SDL_FillRect( screen , & ThisRectangle , 0 );

  /*
  for (i = USERFENSTERPOSY; i < (USERFENSTERPOSY + USERFENSTERHOEHE); i++)
    {
      gl_hline (USERFENSTERPOSX, i, USERFENSTERPOSX + USERFENSTERBREITE,
		KON_BG_COLOR);
      // memset(RealScreen+i*SCREENBREITE+USERFENSTERPOSX,USERFENSTERBREITE,KON_BG_COLOR);
    }
  */

  DebugPrintf ("\nvoid ClearUserFenster(void): End of function reached.");

} // void ClearUserFenster(void)

#undef _ship_c
