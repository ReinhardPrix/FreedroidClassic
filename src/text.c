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
 * Desc: contains all functions dealing with the HUGE, BIG font used for
 *	the top status line, the score and the text displayed during briefing
 *	and highscore inverview.  This has NOTHING to do with the fonts
 *	of the SVGALIB or the fonts used for the horizontal srolling message line!
 *
 *----------------------------------------------------------------------*/
#include <config.h>

#define _paratext_c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <vga.h>
// #include <vgagl.h>
#include <vgakeyboard.h>

#include "defs.h"
#include "struct.h"
#include "proto.h"
#include "global.h"
#include "paratext.h"

#define WAITCHAR 220

int CharLenList[100] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* ' !"#$%&'()' */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* '*+,-./0123' */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* '456789:;<=' */
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1,	/* '>?@ABCDEFG' */
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	/* 'HIJKLMNOPQ' */
  1, 1, 1, 1, 1, 1, 1, 1, 1, 0,	/* 'RSTUVWXYZ[' */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* '\]^_`abcde' */
  0, 0, 0, 0, 0, 0, 0, 1, 0, 0,	/* 'fghijklmno' */
  0, 0, 0, 0, 0, 0, 0, 1, 0, 0,	/* 'pqrstuvwxy' */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 1	/* 'z{|}~ ... ' */
};

char *Wordpointer;
unsigned char *Fontpointer;
unsigned char *Zeichenpointer[110];	/* Pointer-Feld auf Buchstaben-Icons */
unsigned int CurrentFontFG = FIRST_FONT_FG;	/* Momentane Schrift-Farben */
unsigned int CurrentFontBG = FIRST_FONT_BG;

/* Text-Fenster */
int LeftTextBorder;
int RightTextBorder;
int LowerTextBorder;
int UpperTextBorder;

int CharsPerLine;		/* Zeilenlaenge: veraltet */

/* Aktuelle Text-Einfuege-Position: */
int MyCursorX;
int MyCursorY;

/* Buffer fuer Text-Environment */
int StoreCursorX;
int StoreCursorY;
int StoreLeftTextBorder;
int StoreRightTextBorder;
int StoreLowerTextBorder;
int StoreUpperTextBorder;
unsigned int StoreTextBG;
unsigned int StoreTextFG;


extern int TimerFlag;		/* Timerflag- wer haette das gedacht ! */


/*@Function============================================================
@Desc:  This function stores the textenvironment

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
StoreTextEnvironment (void)
{
  StoreLeftTextBorder = LeftTextBorder;
  StoreRightTextBorder = RightTextBorder;
  StoreLowerTextBorder = LowerTextBorder;
  StoreUpperTextBorder = UpperTextBorder;
  StoreCursorX = MyCursorX;
  StoreCursorY = MyCursorY;
  GetTextColor (&StoreTextBG, &StoreTextFG);
}				// void StoreTextEnvironment(void)

/*@Function============================================================
@Desc:  This function restores the textenvironment

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
RestoreTextEnvironment (void)
{
  LeftTextBorder = StoreLeftTextBorder;
  RightTextBorder = StoreRightTextBorder;
  LowerTextBorder = StoreLowerTextBorder;
  UpperTextBorder = StoreUpperTextBorder;
  MyCursorX = StoreCursorX;
  MyCursorY = StoreCursorY;
  SetTextColor (StoreTextBG, StoreTextFG);
}

/*@Function============================================================
@Desc: Initialisierne des Text-Moduls: Buchstaben-Icons einlesen

@Ret: OK/Exit
@Int:
* $Function----------------------------------------------------------*/
int
InitParaplusFont (void)
{
  int i, j;
  int FontXPos = 0;
  unsigned char *FontMem;

  if (RealScreen == NULL)
    // PORT RealScreen = MK_FP(SCREENADDRESS,0000);     
    RealScreen = malloc (64010);
  if (InternalScreen == NULL)
    InternalScreen = (unsigned char *) MyMalloc (SCREENHOEHE * SCREENBREITE);
  if (InternalScreen == NULL)
    {
      DebugPrintf ("No memory left !!");
      Terminate (-1);
    }

  Load_PCX_Image (FONTBILD_PCX, InternalScreen, FALSE);

  FontMem = (unsigned char *) MyMalloc (FONTANZAHL * FONTMEM * 2 + 10);

  for (j = 0; j < 10; j++)
    {
      FontXPos = 0;

      for (i = 0; i < 10; i++)
	{
	  Zeichenpointer[j * 10 + i] = FontMem + (j * 10 + i) * FONTMEM * 2;
	  IsolateBlock (InternalScreen, Zeichenpointer[j * 10 + i], FontXPos,
			j * (FONTHOEHE + 1),
			(CharLenList[j * 10 + i] ==
			 0) ? FONTBREITE : 2 * FONTBREITE, FONTHOEHE);
	  FontXPos +=
	    FONTBREITE + 1 + (CharLenList[j * 10 + i] * (FONTBREITE + 1));
	}
    }

  return (OK);
}				// int InitParaplusFont(void)

/*@Function============================================================
@Desc: SetTextColor(unsigned bg, unsigned fg)
Setzt die Palettenwerten (und nicht die RGB-Werte) der Proportionalschrift
Null als Farbwert bewirkt keinen Effekt
Sicherheitsabfrage, ob die schrift nicht durch Kontrastzerst"orung vernichtet
wird

				sets only, if color != 0 and other then old color
@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void
SetTextColor (unsigned bg, unsigned fg)
{
  register unsigned int i;
  static unsigned LastBg = FIRST_FONT_BG;
  static unsigned LastFg = FIRST_FONT_FG;
  register unsigned char *source;

  /* Sicherheitsabrage bez. Schriftzerst"orung durch Kontrastausl"oschung */
  if ((bg == LastFg) || (bg == fg))
    {
      DebugPrintf
	("\nvoid SetTextColor(...): WARNING ! Die Schrift wird durch diesen Aufruf vernichtet !\n");
      getchar ();
      Terminate (ERR);
    }
  CurrentFontFG = fg;
  CurrentFontBG = bg;

  source = Zeichenpointer[0];
  if ((bg != 0) && (bg != LastBg))
    {
      for (i = 0; i < FONTANZAHL * FONTMEM * 2; i++, source++)
	if (*source == LastBg)
	  *source = bg;
      LastBg = bg;
    }

  source = Zeichenpointer[0];
  if (fg && (fg != LastFg))
    {
      for (i = 0; i < FONTANZAHL * FONTMEM * 2; i++, source++)
	if (*source == LastFg)
	  *source = fg;
      LastFg = fg;
    }

  return;
}				/* SetTextcolor */

/* ====================================================================== 
   Diese Funktion soll die momentane Farbsituation des Textes wiedergeben.
	Dazu werden zwei Pointer "ubergeben, damit sie auch ver"andert werden.
		
	Grund f"ur die Funktion: so sparen wir zwei weitere globale Variablen
   ---------------------------------------------------------------------- */
void
GetTextColor (unsigned int *bg, unsigned int *fg)
{
  *bg = CurrentFontBG;
  *fg = CurrentFontFG;
}

/*@Function============================================================
@Desc: SetTextBorder(): setzt die Bezugs-daten fuer die folgenden
Text-ausgaben
RightTextBorder:
LeftTextBorder:
UpperTextBorder:
LowerTextBorder:

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
SetTextBorder (int left, int upper, int right, int lower, int chars_per_line)
{
  LeftTextBorder = left;
  RightTextBorder = right;
  UpperTextBorder = upper;
  LowerTextBorder = lower;

  CharsPerLine = chars_per_line;

  return;
}				/* SetTextBorder */

/*@Function============================================================
@Desc: ClearTextBorder(*screen): Loescht am Screen den Bereich des
					gesetzten Textfensters (textborder)

@Ret: voidN
@Int:
* $Function----------------------------------------------------------*/
void
ClearTextBorder (unsigned char *screen, int color)
{
  register int i;
  register int height = LowerTextBorder - UpperTextBorder;
  register int xlen = RightTextBorder - LeftTextBorder;
  unsigned char *target;

  target = screen + UpperTextBorder * SCREENBREITE + LeftTextBorder;

  for (i = 0; i < height; i++)
    {
      memset (target, color, xlen);
      target += SCREENBREITE;
    }
  return;
}

/*@Function============================================================
@Desc: SetTextCursor(x, y): Setzt Cursor fuer folgende Textausgaben

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
SetTextCursor (int x, int y)
{
  MyCursorX = x;
  MyCursorY = y;

  return;
}

/*@Function============================================================
@Desc: ScrollText(Text, startx, starty):

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
int
ScrollText (char *Text, int startx, int starty, int EndLine)
{
  int Number_Of_Line_Feeds = 0;		/* Anzahl der Textzeilen */
  long TextHeight;		/* Hoehe des Gesamt-Textes in Pixel */
  char *textpt;			/* bewegl. Textpointer */
  int InsertLine = starty;
  int speed = +2;
  int maxspeed = 4;


  ClearGraphMem (InternalScreen);

  /* Zeilen zaehlen */
  textpt = Text;
  while (*textpt++)
    if (*textpt == '\n')
      Number_Of_Line_Feeds++;

  /* Texthoehe berechnen */
  TextHeight = ( Number_Of_Line_Feeds+ strlen(Text)/CharsPerLine ) * (FONTHOEHE + ZEILENABSTAND);

  while (!SpacePressed () && ((InsertLine + TextHeight) > EndLine))
    {

      JoystickControl ();

      if (UpPressed ())
	{
	  speed--;
	  if (speed < -maxspeed)
	    speed = -maxspeed;
	}
      if (DownPressed ())
	{
	  speed++;
	  if (speed > maxspeed)
	    speed = maxspeed;
	}

      usleep (30000);

      PrepareScaledSurface();

      ClearTextBorder (InternalScreen, CurrentFontBG);
      DisplayText (Text, startx, InsertLine, InternalScreen, FALSE);
      InsertLine -= speed;

      /* Nicht bel. nach unten wegscrollen */
      if (InsertLine > SCREENHOEHE - 10 && (speed < 0))
	{
	  InsertLine = SCREENHOEHE - 10;
	  speed = 0;
	}

      SwapScreen ();		/* Show it */
    }

  // PORT SpacePressed = FALSE;
  return OK;
}				// void ScrollText(void)

/*@Function============================================================
@Desc: void DisplayText(char *Text, int startx, int starty):
gibt Text beginnend bei startx/starty aus
Zeilenumbruch bei voller Zeile oder bei '\n'

*screen: Pointer to the screen to use
EnterCursor: TRUE/FALSE Cursor darstellen ja/nein.
@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void
DisplayText (char *Text,
	     int startx, int starty, unsigned char *screen, int EnterCursor)
{
  char *tmp;			/* Beweg. Zeiger auf aktuelle Position im Ausgabe-Text */

  DebugPrintf ("\nvoid DisplayText(...): Funktion echt aufgerufen.");
  DebugPrintf ("\nvoid DisplayText(...): Der Text lautet:\n");
  DebugPrintf (Text);

  MyCursorX = startx;		/* akt. Schreib-Position */
  MyCursorY = starty;

  tmp = Text;			/* Auf Text-Begin setzen */

  while (!FensterVoll ())
    {

      if (*tmp == '\0')
	{			/* Textende erreicht */
	  /* Cursor darstellen, falls erwuenscht */
	  if (EnterCursor)
	    DisplayChar (CURSOR_ICON, screen);
	  break;
	}

      if (*tmp == '\n')
	{			/* Zeilenende erreicht */
	  MyCursorX = startx;	/* "Wagenruecklauf" */
	  MyCursorY += FONTHOEHE + ZEILENABSTAND;	/* naechste Zeile */
	  tmp++;		/* skip the newline-char !! */
	  continue;
	}

      if ((unsigned char) *tmp == WAITCHAR)
	{
	  Update_SDL_Screen();
	  tmp++;
	  while (UpPressed () || DownPressed () || LeftPressed ()
		 || RightPressed ())
	    JoystickControl ();
	  while (!UpPressed () && !DownPressed () && !LeftPressed ()
		 && !RightPressed () && !SpacePressed ())
	    JoystickControl ();
	  if (SpacePressed ())
	    continue;
	  while (UpPressed () || DownPressed () || LeftPressed ()
		 || RightPressed ())
	    JoystickControl ();
	  MyCursorX = startx;
	  MyCursorY = starty;
	  ClearUserFenster ();
	  continue;
	}

      /* Normales Zeichen ausgeben: */
      DisplayChar (*tmp, screen);
      tmp++;
      
      // CheckUmbruch ();		/* dont write over RightBorder */
      ImprovedCheckUmbruch(tmp);

    } // while !FensterVoll()
  DebugPrintf
    ("\nvoid DisplayText(...): Funktionsende ordnungsgemaess erreicht.");
} // void DisplayText(...)

/*@Function============================================================
@Desc: 
			Wird nicht gebraucht ???
@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
DisplayWord (char *Worttext)
{
  int i;

  gotoxy (1, 1);
  DebugPrintf ("\nDisplayWord() called !!!");
  getchar ();

  if (FensterVoll ())
    return;

  for (i = 0; i < strlen (Worttext); i++)
    {
      DisplayChar (*(Worttext + i), RealScreen);
    }
  MyCursorX += FONTBREITE;
  CheckUmbruch ();
}

/*@Function============================================================
@Desc: Stellt ein Zeichen an der Pos MyCursorX/Y INNERHALB des
Text-Borders dar. Hinausragende Teile des Zeichens werden
"abgeschnitten".
Der Einfuegepunkt MyCursorX/Y wird nach der Ausgabe um die
Zeichenbreite nach rechts verschoben.

Einfuegepunkt: LINKS OBEN

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void
DisplayChar (unsigned char Zeichen, unsigned char *screen)
{
  int i;
  int ZNum = Zeichen - ' ';
  int ZLen = CharLenList[ZNum];

  unsigned char *target;	/* Pointer auf Ausgabeposition am Screen */

  DebugPrintf("\nvoid DisplayChar(...): Real function call confirmed.");

  if (Zeichen == '\n')
    {
      MakeUmbruch ();
      return;
    }

  if ( (Zeichen < ' ') || (Zeichen > 131) )
    {
      DebugPrintf ("Illegal Char an DisplayChar() uebergeben!");
      Terminate(ERR);
    }

  //PORT: SINCE REALSCREEN NO LONGER OF MUCH INPORTANCE FIND SOME WORKAROUND:
  if (screen == RealScreen)
    {
      DisplayBlock (MyCursorX, MyCursorY, Zeichenpointer[ZNum],
		    FONTBREITE * (1 + ZLen), FONTHOEHE, RealScreen);
    }
  else
    {
      for (i = 0; i < FONTHOEHE; i++)
	{
	  if (MyCursorY + FONTHOEHE <= UpperTextBorder)
	    break;
	  if ((MyCursorY + i) <= UpperTextBorder)
	    continue;
	  if ((MyCursorY + i) >= LowerTextBorder)
	    break;
	  if ((MyCursorX > RightTextBorder) ||
	      (MyCursorX + FONTBREITE < LeftTextBorder))
	    break;

	  target =
	    screen + MyCursorX + MyCursorY * SCREENBREITE + i * SCREENBREITE;
	  memcpy (target, Zeichenpointer[ZNum] + i * (1 + ZLen) * FONTBREITE,
		  FONTBREITE * (1 + ZLen));
	}			// for(i=0;i<FONT...
    }				// if (screen==RealScreen)

  MyCursorX += FONTBREITE * (1 + ZLen);	/* Intern-Cursor weiterbewegen */

  DebugPrintf("\nvoid DisplayChar(...): Usual end of function reached.");

  return;
}				// void DisplayChar(...)

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
CheckUmbruch (void)
{
  if (MyCursorX > LeftTextBorder + (CharsPerLine-2) * FONTBREITE)
    MakeUmbruch ();
} // void CheckUmbruch(void)


/*@Function============================================================
  @Desc: This function checks if the next word still fits in this line
  of text and initiates a carriage return/line feed if not.
  Very handy and convenient, for that means it is no longer nescessary
  to enter \n in the text every time its time for a newline. cool.
  
  The function could perhaps still need a little improvement.  But for
  now its good enough and improvement enough in comparison to the old
  CheckUmbruch function above.

  @Ret: 
  @Int:
* $Function----------------------------------------------------------*/
void
ImprovedCheckUmbruch (char* Resttext)
{
  int i;
#define MAX_WORD_LENGTH 100

  if (MyCursorX > LeftTextBorder + (CharsPerLine-2) * FONTBREITE)
    MakeUmbruch ();

  // In case of a space, see if the next word will still fit on the line
  // and do a carriage return/line feed if not
  if ( *Resttext == ' ' ) {
    for (i=1;i<MAX_WORD_LENGTH;i++) 
      {
	if ( (Resttext[i] != ' ') && (Resttext[i] != 0) )
	  { 
	    if ( MyCursorX+i*FONTBREITE > LeftTextBorder + (CharsPerLine-3) * FONTBREITE ) 
	      {
		MakeUmbruch();
		return;
	      }
	  }
	else return;
      }
  }

} // void CheckUmbruch(void)


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
MakeUmbruch (void)
{
  DebugPrintf("\nvoid MakeUmbruch(void): real function call confirmed.");
  MyCursorX = LeftTextBorder;
  MyCursorY += FONTHOEHE + ZEILENABSTAND;
  DebugPrintf("\nvoid MakeUmbruch(void): end of function reached.");
} // void MakeUmbruch(void)


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
int
FensterVoll (void)
{
  if (MyCursorY > LowerTextBorder)
    return (TRUE);
  return (FALSE);
}

/*@Function============================================================
@Desc: GetString(): Liest einen String vom User ein
MaxLen: max. Laenge des Strings

@Ret: char *: String wird HIER reserviert !!! (dont forget to free it !)
@Int:
* $Function----------------------------------------------------------*/
char *
GetString (int MaxLen)
{
  char *instring;		/* Pointer auf eingegebenen String */
  char *loeschstring;		/* String zum Loeschen der Eingabe-Zeile */
  // PORT char taste;                           /* eingeg. Zeichen */
  int charcounter = 0;		/* zaehlt eingeg. Zeichen mit */
  int TextOutX, TextOutY;	/* Einfuegepunkt zum Darstellen der Eingabe */

  // The following line calls the LINUX SVGALIB and sets the keyboard mode from
  // raw mode back to normal mode!  Then the old code can be processed as is (I hope)
  // keyboard_close();

  /* Texteingabe an momentaner Cursor-Pos. */
  TextOutX = MyCursorX;
  TextOutY = MyCursorY;

  /* Speicher fuer Eingabe reservieren */
  if ((instring = MyMalloc (MaxLen + 10)) == NULL)
    {
      DebugPrintf ("\nNo Memory left !!");
      getchar ();
      Terminate (-1);
    }
  /* LoeschString reservieren */
  if ((loeschstring = MyMalloc (2 * MaxLen)) == NULL)
    {
      DebugPrintf ("\nNo Memory left !!");
      getchar ();
      Terminate (-1);
    }
  memset (loeschstring, ' ', 2 * MaxLen - 1);	/* Loeschstring mit SPACE fuellen */
  loeschstring[MaxLen - 1] = '\0';	/* Loeschstring abschliessen */

  instring[0] = '\0';		/* sicherheitshalber abschliessen */

  /* Leeren String mit Cursor ausgeben */
  DisplayText (instring, TextOutX, TextOutY, RealScreen, TRUE);

  /* Zeichen einlesen und anzeigen, bis RET gedrueckt */
  while (!keyboard_keypressed (SCANCODE_ENTER))
    {

      keyboard_update ();
      /* Sondertasten mit ext. Code ignorieren: */
      // if( taste == 0 ) {getchar(); continue;}

      /* Backspace: ausfuehren */
      if (keyboard_keypressed (SCANCODE_BACKSPACE))
	{
	  /* Wenn nicht schon am Beginn des STrings: */
	  if (charcounter)
	    {
	      charcounter--;	/* Zeichen zurueck */
	      instring[charcounter] = '\0';	/* und abschliessen */
	      DisplayText (loeschstring, TextOutX, TextOutY, RealScreen,
			   FALSE);
	      while (keyboard_keypressed (SCANCODE_BACKSPACE))
		keyboard_update ();
	    }
	}

      /* Regulaeres Zeichen: aufnehmen, falls MaxLen noch nicht erreicht */

      // PORT if( (taste >= ' ') && (taste <= 'z') && (charcounter < MaxLen) ) {
      // PORT instring[charcounter ++] = taste;   /* Zeichen aufnehmen */
      // PORT instring[charcounter] = '\0';                       /* und abschliessen */
      // }

      if (keyboard_keypressed (SCANCODE_A) && charcounter < MaxLen)
	{
	  if (keyboard_keypressed (SCANCODE_LEFTSHIFT)
	      || keyboard_keypressed (SCANCODE_RIGHTSHIFT))
	    {
	      instring[charcounter++] = 'A';
	    }
	  else
	    {
	      instring[charcounter++] = 'a';
	    }
	  instring[charcounter] = '\0';	/* und abschliessen */
	  while (keyboard_keypressed (SCANCODE_A))
	    keyboard_update ();
	}

      if (keyboard_keypressed (SCANCODE_B) && charcounter < MaxLen)
	{
	  if (keyboard_keypressed (SCANCODE_LEFTSHIFT)
	      || keyboard_keypressed (SCANCODE_RIGHTSHIFT))
	    {
	      instring[charcounter++] = 'B';
	    }
	  else
	    {
	      instring[charcounter++] = 'b';
	    }
	  instring[charcounter] = '\0';	/* und abschliessen */
	  while (keyboard_keypressed (SCANCODE_B))
	    keyboard_update ();
	}

      if (keyboard_keypressed (SCANCODE_C) && charcounter < MaxLen)
	{
	  if (keyboard_keypressed (SCANCODE_LEFTSHIFT)
	      || keyboard_keypressed (SCANCODE_RIGHTSHIFT))
	    {
	      instring[charcounter++] = 'C';
	    }
	  else
	    {
	      instring[charcounter++] = 'c';
	    }
	  instring[charcounter] = '\0';	/* und abschliessen */
	  while (keyboard_keypressed (SCANCODE_C))
	    keyboard_update ();
	}

      if (keyboard_keypressed (SCANCODE_D) && charcounter < MaxLen)
	{
	  if (keyboard_keypressed (SCANCODE_LEFTSHIFT)
	      || keyboard_keypressed (SCANCODE_RIGHTSHIFT))
	    {
	      instring[charcounter++] = 'D';
	    }
	  else
	    {
	      instring[charcounter++] = 'd';
	    }
	  instring[charcounter] = '\0';	/* und abschliessen */
	  while (keyboard_keypressed (SCANCODE_D))
	    keyboard_update ();
	}

      if (keyboard_keypressed (SCANCODE_E) && charcounter < MaxLen)
	{
	  if (keyboard_keypressed (SCANCODE_LEFTSHIFT)
	      || keyboard_keypressed (SCANCODE_RIGHTSHIFT))
	    {
	      instring[charcounter++] = 'E';
	    }
	  else
	    {
	      instring[charcounter++] = 'e';
	    }
	  instring[charcounter] = '\0';	/* und abschliessen */
	  while (keyboard_keypressed (SCANCODE_E))
	    keyboard_update ();
	}

      if (keyboard_keypressed (SCANCODE_F) && charcounter < MaxLen)
	{
	  if (keyboard_keypressed (SCANCODE_LEFTSHIFT)
	      || keyboard_keypressed (SCANCODE_RIGHTSHIFT))
	    {
	      instring[charcounter++] = 'F';
	    }
	  else
	    {
	      instring[charcounter++] = 'f';
	    }
	  instring[charcounter] = '\0';	/* und abschliessen */
	  while (keyboard_keypressed (SCANCODE_F))
	    keyboard_update ();
	}

      if (keyboard_keypressed (SCANCODE_G) && charcounter < MaxLen)
	{
	  if (keyboard_keypressed (SCANCODE_LEFTSHIFT)
	      || keyboard_keypressed (SCANCODE_RIGHTSHIFT))
	    {
	      instring[charcounter++] = 'G';
	    }
	  else
	    {
	      instring[charcounter++] = 'g';
	    }
	  instring[charcounter] = '\0';	/* und abschliessen */
	  while (keyboard_keypressed (SCANCODE_G))
	    keyboard_update ();
	}

      if (keyboard_keypressed (SCANCODE_H) && charcounter < MaxLen)
	{
	  if (keyboard_keypressed (SCANCODE_LEFTSHIFT)
	      || keyboard_keypressed (SCANCODE_RIGHTSHIFT))
	    {
	      instring[charcounter++] = 'H';
	    }
	  else
	    {
	      instring[charcounter++] = 'h';
	    }
	  instring[charcounter] = '\0';	/* und abschliessen */
	  while (keyboard_keypressed (SCANCODE_H))
	    keyboard_update ();
	}

      if (keyboard_keypressed (SCANCODE_I) && charcounter < MaxLen)
	{
	  if (keyboard_keypressed (SCANCODE_LEFTSHIFT)
	      || keyboard_keypressed (SCANCODE_RIGHTSHIFT))
	    {
	      instring[charcounter++] = 'I';
	    }
	  else
	    {
	      instring[charcounter++] = 'i';
	    }
	  instring[charcounter] = '\0';	/* und abschliessen */
	  while (keyboard_keypressed (SCANCODE_I))
	    keyboard_update ();
	}

      if (keyboard_keypressed (SCANCODE_J) && charcounter < MaxLen)
	{
	  if (keyboard_keypressed (SCANCODE_LEFTSHIFT)
	      || keyboard_keypressed (SCANCODE_RIGHTSHIFT))
	    {
	      instring[charcounter++] = 'J';
	    }
	  else
	    {
	      instring[charcounter++] = 'j';
	    }
	  instring[charcounter] = '\0';	/* und abschliessen */
	  while (keyboard_keypressed (SCANCODE_J))
	    keyboard_update ();
	}

      if (keyboard_keypressed (SCANCODE_K) && charcounter < MaxLen)
	{
	  if (keyboard_keypressed (SCANCODE_LEFTSHIFT)
	      || keyboard_keypressed (SCANCODE_RIGHTSHIFT))
	    {
	      instring[charcounter++] = 'K';
	    }
	  else
	    {
	      instring[charcounter++] = 'k';
	    }
	  instring[charcounter] = '\0';	/* und abschliessen */
	  while (keyboard_keypressed (SCANCODE_K))
	    keyboard_update ();
	}

      if (keyboard_keypressed (SCANCODE_A) && charcounter < MaxLen)
	{
	  if (keyboard_keypressed (SCANCODE_LEFTSHIFT)
	      || keyboard_keypressed (SCANCODE_RIGHTSHIFT))
	    {
	      instring[charcounter++] = 'A';
	    }
	  else
	    {
	      instring[charcounter++] = 'a';
	    }
	  instring[charcounter] = '\0';	/* und abschliessen */
	  while (keyboard_keypressed (SCANCODE_A))
	    keyboard_update ();
	}

      if (keyboard_keypressed (SCANCODE_L) && charcounter < MaxLen)
	{
	  if (keyboard_keypressed (SCANCODE_LEFTSHIFT)
	      || keyboard_keypressed (SCANCODE_RIGHTSHIFT))
	    {
	      instring[charcounter++] = 'L';
	    }
	  else
	    {
	      instring[charcounter++] = 'l';
	    }
	  instring[charcounter] = '\0';	/* und abschliessen */
	  while (keyboard_keypressed (SCANCODE_L))
	    keyboard_update ();
	}

      if (keyboard_keypressed (SCANCODE_M) && charcounter < MaxLen)
	{
	  if (keyboard_keypressed (SCANCODE_LEFTSHIFT)
	      || keyboard_keypressed (SCANCODE_RIGHTSHIFT))
	    {
	      instring[charcounter++] = 'M';
	    }
	  else
	    {
	      instring[charcounter++] = 'm';
	    }
	  instring[charcounter] = '\0';	/* und abschliessen */
	  while (keyboard_keypressed (SCANCODE_M))
	    keyboard_update ();
	}

      if (keyboard_keypressed (SCANCODE_N) && charcounter < MaxLen)
	{
	  if (keyboard_keypressed (SCANCODE_LEFTSHIFT)
	      || keyboard_keypressed (SCANCODE_RIGHTSHIFT))
	    {
	      instring[charcounter++] = 'N';
	    }
	  else
	    {
	      instring[charcounter++] = 'n';
	    }
	  instring[charcounter] = '\0';	/* und abschliessen */
	  while (keyboard_keypressed (SCANCODE_N))
	    keyboard_update ();
	}

      if (keyboard_keypressed (SCANCODE_O) && charcounter < MaxLen)
	{
	  if (keyboard_keypressed (SCANCODE_LEFTSHIFT)
	      || keyboard_keypressed (SCANCODE_RIGHTSHIFT))
	    {
	      instring[charcounter++] = 'O';
	    }
	  else
	    {
	      instring[charcounter++] = 'o';
	    }
	  instring[charcounter] = '\0';	/* und abschliessen */
	  while (keyboard_keypressed (SCANCODE_O))
	    keyboard_update ();
	}

      if (keyboard_keypressed (SCANCODE_P) && charcounter < MaxLen)
	{
	  if (keyboard_keypressed (SCANCODE_LEFTSHIFT)
	      || keyboard_keypressed (SCANCODE_RIGHTSHIFT))
	    {
	      instring[charcounter++] = 'P';
	    }
	  else
	    {
	      instring[charcounter++] = 'p';
	    }
	  instring[charcounter] = '\0';	/* und abschliessen */
	  while (keyboard_keypressed (SCANCODE_P))
	    keyboard_update ();
	}

      if (keyboard_keypressed (SCANCODE_Q) && charcounter < MaxLen)
	{
	  if (keyboard_keypressed (SCANCODE_LEFTSHIFT)
	      || keyboard_keypressed (SCANCODE_RIGHTSHIFT))
	    {
	      instring[charcounter++] = 'Q';
	    }
	  else
	    {
	      instring[charcounter++] = 'q';
	    }
	  instring[charcounter] = '\0';	/* und abschliessen */
	  while (keyboard_keypressed (SCANCODE_Q))
	    keyboard_update ();
	}

      if (keyboard_keypressed (SCANCODE_R) && charcounter < MaxLen)
	{
	  if (keyboard_keypressed (SCANCODE_LEFTSHIFT)
	      || keyboard_keypressed (SCANCODE_RIGHTSHIFT))
	    {
	      instring[charcounter++] = 'R';
	    }
	  else
	    {
	      instring[charcounter++] = 'r';
	    }
	  instring[charcounter] = '\0';	/* und abschliessen */
	  while (keyboard_keypressed (SCANCODE_R))
	    keyboard_update ();
	}

      if (keyboard_keypressed (SCANCODE_S) && charcounter < MaxLen)
	{
	  if (keyboard_keypressed (SCANCODE_LEFTSHIFT)
	      || keyboard_keypressed (SCANCODE_RIGHTSHIFT))
	    {
	      instring[charcounter++] = 'S';
	    }
	  else
	    {
	      instring[charcounter++] = 's';
	    }
	  instring[charcounter] = '\0';	/* und abschliessen */
	  while (keyboard_keypressed (SCANCODE_S))
	    keyboard_update ();
	}

      if (keyboard_keypressed (SCANCODE_T) && charcounter < MaxLen)
	{
	  if (keyboard_keypressed (SCANCODE_LEFTSHIFT)
	      || keyboard_keypressed (SCANCODE_RIGHTSHIFT))
	    {
	      instring[charcounter++] = 'T';
	    }
	  else
	    {
	      instring[charcounter++] = 't';
	    }
	  instring[charcounter] = '\0';	/* und abschliessen */
	  while (keyboard_keypressed (SCANCODE_T))
	    keyboard_update ();
	}

      if (keyboard_keypressed (SCANCODE_U) && charcounter < MaxLen)
	{
	  if (keyboard_keypressed (SCANCODE_LEFTSHIFT)
	      || keyboard_keypressed (SCANCODE_RIGHTSHIFT))
	    {
	      instring[charcounter++] = 'U';
	    }
	  else
	    {
	      instring[charcounter++] = 'u';
	    }
	  instring[charcounter] = '\0';	/* und abschliessen */
	  while (keyboard_keypressed (SCANCODE_U))
	    keyboard_update ();
	}

      if (keyboard_keypressed (SCANCODE_V) && charcounter < MaxLen)
	{
	  if (keyboard_keypressed (SCANCODE_LEFTSHIFT)
	      || keyboard_keypressed (SCANCODE_RIGHTSHIFT))
	    {
	      instring[charcounter++] = 'V';
	    }
	  else
	    {
	      instring[charcounter++] = 'v';
	    }
	  instring[charcounter] = '\0';	/* und abschliessen */
	  while (keyboard_keypressed (SCANCODE_V))
	    keyboard_update ();
	}

      if (keyboard_keypressed (SCANCODE_W) && charcounter < MaxLen)
	{
	  if (keyboard_keypressed (SCANCODE_LEFTSHIFT)
	      || keyboard_keypressed (SCANCODE_RIGHTSHIFT))
	    {
	      instring[charcounter++] = 'W';
	    }
	  else
	    {
	      instring[charcounter++] = 'w';
	    }
	  instring[charcounter] = '\0';	/* und abschliessen */
	  while (keyboard_keypressed (SCANCODE_W))
	    keyboard_update ();
	}

      if (keyboard_keypressed (SCANCODE_X) && charcounter < MaxLen)
	{
	  if (keyboard_keypressed (SCANCODE_LEFTSHIFT)
	      || keyboard_keypressed (SCANCODE_RIGHTSHIFT))
	    {
	      instring[charcounter++] = 'X';
	    }
	  else
	    {
	      instring[charcounter++] = 'x';
	    }
	  instring[charcounter] = '\0';	/* und abschliessen */
	  while (keyboard_keypressed (SCANCODE_X))
	    keyboard_update ();
	}

      if (keyboard_keypressed (SCANCODE_Y) && charcounter < MaxLen)
	{
	  if (keyboard_keypressed (SCANCODE_LEFTSHIFT)
	      || keyboard_keypressed (SCANCODE_RIGHTSHIFT))
	    {
	      instring[charcounter++] = 'Y';
	    }
	  else
	    {
	      instring[charcounter++] = 'y';
	    }
	  instring[charcounter] = '\0';	/* und abschliessen */
	  while (keyboard_keypressed (SCANCODE_Y))
	    keyboard_update ();
	}

      if (keyboard_keypressed (SCANCODE_Z) && charcounter < MaxLen)
	{
	  if (keyboard_keypressed (SCANCODE_LEFTSHIFT)
	      || keyboard_keypressed (SCANCODE_RIGHTSHIFT))
	    {
	      instring[charcounter++] = 'Z';
	    }
	  else
	    {
	      instring[charcounter++] = 'z';
	    }
	  instring[charcounter] = '\0';	/* und abschliessen */
	  while (keyboard_keypressed (SCANCODE_Z))
	    keyboard_update ();
	}

      /* alte eingabe-Zeile loeschen: */
      /* es wird eine Laenge MaxLen -1 geloescht */
      // DisplayText(loeschstring, TextOutX, TextOutY, RealScreen, FALSE);                


      /* Eingabe mit Cursor ausgeben */
      // DisplayText(instring, TextOutX, TextOutY, RealScreen, TRUE);
      DisplayText (instring, TextOutX, TextOutY, RealScreen, TRUE);

    }				/* While nicht RET gedrueckt */

  // After the GetString operation is complete the keyboard can be set to raw mode again
  // for further play.
  // keyboard_init();

  return (instring);

}				/* GetString() */


#undef _paratext_c
