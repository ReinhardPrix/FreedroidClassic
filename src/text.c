/*----------------------------------------------------------------------
 *
 * Desc: contains all functions dealing with the HUGE, BIG font used for
 *	the top status line, the score and the text displayed during briefing
 *	and highscore inverview.  This has NOTHING to do with the fonts
 *	of the SVGALIB or the fonts used for the horizontal srolling
 *      message line!
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
#define _text_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "proto.h"
#include "global.h"
#include "text.h"

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

int CharsPerLine;		/* Zeilenlaenge: veraltet */

/* Aktuelle Text-Einfuege-Position: */
int MyCursorX;
int MyCursorY;

/* Buffer fuer Text-Environment */
int StoreCursorX;
int StoreCursorY;

unsigned int StoreTextBG;
unsigned int StoreTextFG;


extern int TimerFlag;		/* Timerflag- wer haette das gedacht ! */

/*-----------------------------------------------------------------
 * @Desc: Setzt die Palettenwerten (und nicht die RGB-Werte) der
 * 	Proportionalschrift Null als Farbwert bewirkt keinen Effekt
 * 	Sicherheitsabfrage, ob die schrift nicht durch
 * 	Kontrastzerst"orung vernichtet wird
 * 
 * sets only, if color != 0 and other then old color
 * 
 *
 *-----------------------------------------------------------------*/
void
SetTextColor (unsigned char bg, unsigned char fg)
{

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

/*-----------------------------------------------------------------
 *  scrolls a given text down inside the User-window, 
 *  defined by the global SDL_Rect User_Rect
 *
 *  startx/y give the Start-position, 
 *  EndLine is the last line (?)
 *
 *-----------------------------------------------------------------*/
int
ScrollText (char *Text, int startx, int starty, int EndLine)
{
  int Number_Of_Line_Feeds = 0;		/* Anzahl der Textzeilen */
  char *textpt;			/* bewegl. Textpointer */
  int InsertLine = starty;
  int speed = +2;
  int maxspeed = 4;

  printf("\nScrollTest should be starting to scroll now...");

   /* Zeilen zaehlen */
  textpt = Text;
  while (*textpt++)
    if (*textpt == '\n')
      Number_Of_Line_Feeds++;

  while (!SpacePressed () )
    {
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

      ClearUserFenster(); 

      if (!DisplayText (Text, startx, InsertLine, &User_Rect))
	break;  /* Text has been scrolled outside User_Rect */

      InsertLine -= speed;

      PrepareScaledSurface(TRUE);

      /* Nicht bel. nach unten wegscrollen */
      if (InsertLine > SCREENHOEHE - 10 && (speed < 0))
	{
	  InsertLine = SCREENHOEHE - 10;
	  speed = 0;
	}

    } /* while !Space_Pressed */

  return OK;
}				// void ScrollText(void)

/*-----------------------------------------------------------------
 * @Desc: prints *Text beginning at positions startx/starty, 
 * 
 *	and respecting the text-borders set by clip_rect
 *      -> this includes clipping but also automatic line-breaks
 *      when end-of-line is reached
 * 
 *      if clip_rect==NULL, no clipping is performed
 *      
 *      NOTE: the previous clip-rectange is restored before
 *            the function returns!
 *
 *      NOTE2: this function _does not_ update the screen
 *
 * @Ret: TRUE if some characters where written inside the clip rectangle
 *       FALSE if not (used by ScrollText to know if Text has been scrolled
 *             out of clip-rect completely)
 *-----------------------------------------------------------------*/
int
DisplayText (char *Text, int startx, int starty, const SDL_Rect *clip)
{
  char *tmp;	/* Beweg. Zeiger auf aktuelle Position im Ausgabe-Text */

  SDL_Rect store_clip;

  MyCursorX = startx;		
  MyCursorY = starty;


  SDL_GetClipRect (ne_screen, &store_clip);  /* store previous clip-rect */
  if (clip)
    SDL_SetClipRect (ne_screen, clip);

  tmp = Text;			/* running text-pointer */

  while ( *tmp && (MyCursorY < clip->y + clip->h) )
    {
      if ( *tmp == '\n' )
	{
	  MyCursorX = clip->x;
	  MyCursorY += FontHeight (Menu_BFont) * TEXT_STRETCH;
	}
      else
	DisplayChar (*tmp);

      tmp++;

      ImprovedCheckUmbruch(tmp, clip);   /* dont write over right border */

    } // while !FensterVoll()

   SDL_SetClipRect (ne_screen, &store_clip); /* restore previous clip-rect */

  /*
   * ScrollText() wants to know if we still wrote something inside the
   * clip-rectangle, of if the Text has been scrolled out
   */
  if ( (MyCursorY < clip->y) || (starty > clip->y + clip->h) )
    return FALSE;  /* no text was written inside clip */
  else
    return TRUE; 

} // DisplayText(...)

/*-----------------------------------------------------------------
 * @Desc: This function displays a char.  It uses Menu_BFont now
 * to do this.  MyCursorX is  updated to new position.
 *
 *
 -----------------------------------------------------------------*/
void
DisplayChar (unsigned char c)
{

  if ( !isprint(c) ) // don't accept non-printable characters
    {
      printf ("Illegal char passed to DisplayChar(): %d \n", c);
      Terminate(ERR);
    }

  PutCharFont (ne_screen, Menu_BFont, MyCursorX, MyCursorY, c);

  // After the char has been displayed, we must move the cursor to its
  // new position.  That depends of course on the char displayed.
  //
  MyCursorX += CharWidth (Menu_BFont, c);
  
} // void DisplayChar(...)


/*@Function============================================================
  @Desc: This function checks if the next word still fits in this line
  of text and initiates a carriage return/line feed if not.
  Very handy and convenient, for that means it is no longer nescessary
  to enter \n in the text every time its time for a newline. cool.
  
  The function could perhaps still need a little improvement.  But for
  now its good enough and improvement enough in comparison to the old
  CheckUmbruch function.

  rp: added argument clip, which contains the text-window we're writing in
       (formerly known as "TextBorder")

  @Ret: 
  @Int:
* $Function----------------------------------------------------------*/
void
ImprovedCheckUmbruch (char* Resttext, const SDL_Rect *clip)
{
  int i;
  int NeededSpace=0;
#define MAX_WORD_LENGTH 100

  // In case of a space, see if the next word will still fit on the line
  // and do a carriage return/line feed if not
  if ( *Resttext == ' ' ) {
    for (i=1;i<MAX_WORD_LENGTH;i++) 
      {
	if ( (Resttext[i] != ' ') && (Resttext[i] != 0) )
	  { 
	    NeededSpace+=CharWidth( Menu_BFont , Resttext[i] );
	    if ( MyCursorX+NeededSpace > clip->x + clip->w - 10 )
	      {
		MyCursorX = clip->x;
		MyCursorY += FontHeight (Menu_BFont) * TEXT_STRETCH;
		return;
	      }
	  }
	else 
	  return;
      }
  }
} // void ImprovedCheckUmbruch(void)


/*-----------------------------------------------------------------
 * @Desc: reads a string of "MaxLen" from User-input, and echos it 
 *        either to stdout or using graphics-text, depending on the
 *        parameter "echo":	echo=0    no echo
 * 		               	echo=1    print using printf
 *         			echo=2    print using graphics-text
 *
 *     values of echo > 2 are ignored and treated like echo=0
 *
 *  NOTE: MaxLen is the maximal _strlen_ of the string (excl. \0 !)
 * 
 * @Ret: char *: String wird HIER reserviert !!! 
 *       (dont forget to free it !)
 * 
 *-----------------------------------------------------------------*/
char *
GetString (int MaxLen, int echo)
{
  char *input;		/* Pointer auf eingegebenen String */
  char *clear_str;     /* string used to clear echo-arear with DisplayText */
  int key;             /* last 'character' entered */
  int curpos;		/* zaehlt eingeg. Zeichen mit */
  int finished;
  int TextOutX, TextOutY;	/* Einfuegepunkt zum Darstellen der Eingabe */

  /* Texteingabe an momentaner Cursor-Pos. */
  TextOutX = MyCursorX;
  TextOutY = MyCursorY;

  /* Speicher fuer Eingabe reservieren */
  input     = MyMalloc (MaxLen + 5);
  clear_str = MyMalloc (MaxLen + 5);

  memset (clear_str, ' ', MaxLen);
  clear_str[MaxLen] = 0;
  
  finished = FALSE;
  curpos = 0;
  
  while ( !finished  )
    {
      input[curpos] = '\0';  /* terminate current input-string correctly for echo */

      if (echo == 1)		/* echo to stdout */
	{
	  if (curpos>0) 
	    putchar (input[curpos-1]);  
	  fflush (stdout);	/* here we need to flush manually (ask rp why) */
	}
      else if (echo == 2)   	/* or use graphics-text */
	{
	  DisplayText (clear_str, TextOutX, TextOutY, &User_Rect);
	  DisplayText (input,     TextOutX, TextOutY, &User_Rect);
	  PrepareScaledSurface(TRUE);
	} /* if echo */

      key = getchar_raw ();  
      
      if (key == SDLK_RETURN) 
	finished = TRUE;

      else if (isprint (key) && (curpos < MaxLen) )  
	input[curpos ++] = (char) key;   /* printable characters are entered in string */

      else if (key == SDLK_BACKSPACE)
	if (curpos > 0) curpos --;
      
    } /* while(!finished) */

  DebugPrintf("\n\nchar *GetString(..):  The final string is:\n");
  DebugPrintf( input );
  DebugPrintf("\n\n");


  return (input);

} /* GetString() */


#undef _text_c
