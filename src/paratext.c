/*----------------------------------------------------------------------
 *
 * Desc: contains all functions dealing with the HUGE, BIG font used for
 *	the top status line, the score and the text displayed during briefing
 *	and highscore inverview.  This has NOTHING to do with the fonts
 *	of the SVGALIB or the fonts used for the horizontal srolling message line!
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
#define _paratext_c

#include "system.h"

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
@Desc: ClearTextBorder(*Paramter_Screen): Loescht am Screen den Bereich des
					gesetzten Textfensters (textborder)

@Ret: voidN
@Int:
* $Function----------------------------------------------------------*/
void
ClearTextBorder (unsigned char *Parameter_Screen, int color)
{
  register int i;
  register int height = LowerTextBorder - UpperTextBorder;
  register int xlen = RightTextBorder - LeftTextBorder;
  unsigned char *target;

  target = Parameter_Screen + UpperTextBorder * SCREENBREITE + LeftTextBorder;

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

      PrepareScaledSurface(TRUE);

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

/*-----------------------------------------------------------------
 * @Desc: gibt Text beginnend bei startx/starty aus Zeilenumbruch bei 
 *        voller Zeile oder bei '\n' 
 * 
 * Params: 	*screen: Pointer to the screen to use
 * EnterCursor: TRUE/FALSE Cursor darstellen ja/nein.
 * 
 * @Ret: void
 *-----------------------------------------------------------------*/
void
DisplayText (char *Text,
	     int startx, int starty, unsigned char *screen, int EnterCursor)
{
  char *tmp;	/* Beweg. Zeiger auf aktuelle Position im Ausgabe-Text */

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
	  PrepareScaledSurface(TRUE);

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

/*-----------------------------------------------------------------
 * @Desc: reads a string of "MaxLen" from User-input, and echos it 
 *        either to stdout or using graphics-text, depending on the
 *        parameter "echo":	echo=0    no echo
 * 		               	echo=1    print using printf
 *         			echo=2    print using graphics-text
 *
 *     values of echo > 2 are ignored and treated like echo=0
 *
 * @Ret: char *: String wird HIER reserviert !!! 
 *       (dont forget to free it !)
 * 
 *-----------------------------------------------------------------*/
char *
GetString (int MaxLen, int echo)
{
  char *input;		/* Pointer auf eingegebenen String */
  int curpos;		/* zaehlt eingeg. Zeichen mit */
  int finished;
  int TextOutX, TextOutY;	/* Einfuegepunkt zum Darstellen der Eingabe */

  /* Texteingabe an momentaner Cursor-Pos. */
  TextOutX = MyCursorX;
  TextOutY = MyCursorY;

  /* Speicher fuer Eingabe reservieren */
  if ((input = MyMalloc (MaxLen + 10)) == NULL)
    {
      DebugPrintf ("\nNo Memory left !!");
      Terminate (-1);
    }

  finished = FALSE;
  curpos = 0;
  input[0] = '\0'; 
  
  while ( !finished  )
    {
      if (echo == 1)		/* echo to stdout */
	{
	  if (curpos>0) 
	    putchar (input[curpos-1]);  
	  fflush (stdout);	/* here we need to flush manually (ask rp why) */
	}
      else if (echo == 2)   	/* or use graphics-text */
	{
	  DisplayText (input, TextOutX, TextOutY, RealScreen, TRUE);
	  PrepareScaledSurface(TRUE);
	} /* if echo */

      input[curpos] = (char) getchar_raw (); 	/* read char from raw kbd */
      if (input[curpos] == '\r')  		/* return pressed */
	finished = TRUE;
      else
	curpos ++;

      if (curpos == MaxLen) 
	finished = TRUE;

      input[curpos] = '\0'; 	/* terminate string. this must be inside */
				/* the loop for DisplayText() ! */

    } /* while(!finished) */

  return (input);

} /* GetString() */


#undef _paratext_c
