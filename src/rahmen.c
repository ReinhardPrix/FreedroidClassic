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

/*----------------------------------------------------------------------
 *
 * Desc: contains functions to update and draw the top status line with
 *	score and status etc...
 *
 *----------------------------------------------------------------------*/
#include <config.h>

#define _rahmen_c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "defs.h"
#include "struct.h"
#include "proto.h"
#include "global.h"
#include "paratext.h"

extern char *InfluenceModeNames[];

/* Startpositionen fuer Rahmen-Texte */
#define LEFTINFO_X 	12
#define LEFTINFO_Y	8

#define RIGHTINFO_X	242
#define RIGHTINFO_Y	8


#define LEFT_TEXT_LEN 10
#define RIGHT_TEXT_LEN 6

// Dieses Array enth"alt die Datenstrukturen "uber alle im Spiel vorkommenden
// Anzeigebalken.
//
// { {point pos}, int len, int hgt, int oldval, int col }
//

bar AllBars[] = {
// Bar for the Energydisplay
  {{3, 52}, 300, 10, 0, FONT_RED},
// Bars for Shield1-4
  {{260, 5}, 50, 5, 0, FONT_GREEN},
  {{260, 12}, 50, 5, 0, FONT_GREEN},
  {{260, 19}, 50, 5, 0, FONT_GREEN},
  {{260, 26}, 50, 5, 0, FONT_GREEN}
};

void DrawBar (int, int, unsigned char *);

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
DrawBar (int BarCode, int Wert, unsigned char *Parameter_Screen)
{
  unsigned char *BarPoint = Parameter_Screen;
  int xlen;
  int barcol = 0;
  int i;

  DebugPrintf ("\nvoid DrawBar(...):  real function call confirmed.");

  if (Wert < 0)
    Wert = 0;
  BarPoint += AllBars[BarCode].pos.x + AllBars[BarCode].pos.y * SCREENBREITE;

  if (InitBars)
    {
      for (i = 0; i < AllBars[BarCode].hgt; i++)
	{
	  memset (BarPoint, AllBars[BarCode].col, Wert);
	  memset (BarPoint + Wert, 0, abs (AllBars[BarCode].len - Wert));
	  BarPoint += SCREENBREITE;
	}
      AllBars[BarCode].oldval = Wert;
      return;
    }

  if (Wert == AllBars[BarCode].oldval)
    {
      DebugPrintf ("\nvoid DrawBar(...):  end of function reached.");
      return;
    }

  xlen = abs (Wert - AllBars[BarCode].oldval);

  // Den Cursor an die Position stellen und rot oder schwarz einstellen.        
  if (Wert > AllBars[BarCode].oldval)
    {
      barcol = AllBars[BarCode].col;
      BarPoint += AllBars[BarCode].oldval;
    }
  else
    BarPoint += Wert;

  // Balken soweit zeichnen, wie die Ver"anderung ausmacht.
  for (i = 0; i < AllBars[BarCode].hgt; i++)
    {
      memset (BarPoint, barcol, xlen);
      BarPoint += SCREENBREITE;
    }

  AllBars[BarCode].oldval = Wert;

  DebugPrintf ("\nvoid DrawBar(...):  end of function reached.");

}				// void DrawBar(...)

/*@Function============================================================
@Desc: SayLeftInfo( char* text):
						gibt text oben links im Rahmen aus, loescht
						alten text zuerst

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void
SayLeftInfo (char *text, unsigned char *Parameter_Screen)
{
  char textbox[LEFT_TEXT_LEN + 10];

  if (!PlusExtentionsOn)
    {
      /* Hintergrund Textfarbe setzen */
      SetTextColor (FONT_WHITE, FONT_RED);	// FONT_RED, 0

      strncpy (textbox, text, LEFT_TEXT_LEN);
      if (strlen (text) < LEFT_TEXT_LEN)
	strncat (textbox, "           ", LEFT_TEXT_LEN - strlen (text));
      textbox[LEFT_TEXT_LEN] = '\0';	/* String abschliessen */

      /* Text ausgeben */
      DisplayText (textbox, LEFTINFO_X, LEFTINFO_Y, Parameter_Screen, FALSE);
      return;
    }
}				// void SayLeftInfo(...)

/*@Function============================================================
@Desc: SayRightInfo(char *text): wie SayLeftInfo()

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void
SayRightInfo (char *text, unsigned char *Parameter_Screen)
{
  char textbox[RIGHT_TEXT_LEN + 10];

  if (!PlusExtentionsOn)
    {
      /* Hintergrund Textfarbe richtig setzen */
      SetTextColor (FONT_WHITE, FONT_RED);

      strncpy (textbox, text, RIGHT_TEXT_LEN);
      if (strlen (text) < RIGHT_TEXT_LEN)
	strncat (textbox, "           ", RIGHT_TEXT_LEN - strlen (text));
      textbox[RIGHT_TEXT_LEN] = '\0';

      /* Text ausgeben */
      DisplayText (textbox, RIGHTINFO_X, RIGHTINFO_Y, Parameter_Screen, FALSE);
    }
  return;
}				/* SayRightInfo */


/*@Function============================================================
@Desc: DisplayRahmen(*Parameter_Screen):  gibt Rahmen mit Info-Texten auf screen aus
    Die Funktion sichert und restauriert jetzt auch die Schriftfarben

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void
DisplayRahmen (unsigned char *Parameter_Screen)
{
  // unsigned int bg;
  // unsigned int fg;

  // DisplayBlock(0, 0, RahmenPicture, RAHMENBREITE, RAHMENHOEHE, Parameter_Screen);
  DisplayMergeBlock (0, 0, RahmenPicture, RAHMENBREITE, RAHMENHOEHE, InternalScreen);

  /*    GetTextColor(&bg,&fg);
   SetTextColor(FONT_WHITE,FONT_RED);    *//* BG: Rahmenwei"s FG: FONT_RED */
  SayRightInfo (RightInfo, InternalScreen);
  SayLeftInfo (LeftInfo, InternalScreen);

  if ( screen == RealScreen )
    DisplayMergeBlock (0, 0, InternalScreen, RAHMENBREITE, RAHMENHOEHE, RealScreen);

  /*    SetTextColor(bg,fg); */
  return;
}				/* DisplayRahmen */


/*@Function============================================================
@Desc: SetInfoline(): schreibt Modus und Punkte-Werte in die
						globalen Infoline-Variable

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void
SetInfoline (void)
{
  char dummy[80];
  /* Modus des Influencers links angeben  */
  strncpy (LeftInfo, InfluenceModeNames[Me.status], LEFT_TEXT_LEN - 1);
  LeftInfo[LEFT_TEXT_LEN - 1] = '\0';

  /* Punkte des Users rechts ausgeben */
  strncpy (RightInfo, ltoa (ShowScore, dummy, 10), RIGHT_TEXT_LEN - 1);
  RightInfo[RIGHT_TEXT_LEN - 1] = '\0';

  return;
}				/* SetInfoline */


/*@Function============================================================
@Desc: UpdateInfoline(): 		setzt Infos im Rahmen neu (nur RealScreen),
								wenn sie sich geaendert haben: Flimmern 

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void
UpdateInfoline (void)
{
  static char LastLeft[50];	/* the change-detectors */
  static char LastRight[50];
  int NoNeedToSaveEnv = 1;

  DebugPrintf
    ("\nvoid UpdateInfoline(void): Real function call confirmed....");

  if ((Me.status == CONSOLE) || (Me.status == DEBRIEFING))
    NoNeedToSaveEnv = 0;

  if (!NoNeedToSaveEnv)
    StoreTextEnvironment ();

  if (strcmp (LastLeft, LeftInfo) != 0)
    {
      SetTextColor (FONT_WHITE, FONT_RED);
      // SayLeftInfo (LeftInfo, RealScreen);
      SayLeftInfo (LeftInfo, InternalScreen);
      strcpy (LastLeft, LeftInfo);
    }

  if (strcmp (LastRight, RightInfo) != 0)
    {
      // SayRightInfo (RightInfo, RealScreen);
      SayRightInfo (RightInfo, InternalScreen);
      strcpy (LastRight, RightInfo);
    }

  if (!NoNeedToSaveEnv)
    RestoreTextEnvironment ();

  DebugPrintf ("\nvoid UpdateInfoline(void): end of function reached.");

  return;
} // void UpdateInfoline(void)

#undef _rahmen_c
