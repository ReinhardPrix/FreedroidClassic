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
#include "SDL_rotozoom.h"

int DisplayTextWithScrolling (char *Text, int startx, int starty, const SDL_Rect *clip , SDL_Surface* Background );

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

void 
EnemyHitByBulletText( int Enum )
{
  Enemy ThisRobot=&AllEnemys[ Enum ];

  if ( !GameConfig.Enemy_Hit_Text ) return;
  
  ThisRobot->TextVisibleTime=0;

  switch (MyRandom(4))
    {
    case 0:
      ThisRobot->TextToBeDisplayed="Unhandled exception fault.  Press ok to reboot.";
      break;
    case 1:
      ThisRobot->TextToBeDisplayed="System fault. Please buy a newer version.";
      break;
    case 2:
      ThisRobot->TextToBeDisplayed="System error. Might be a virus.";
      break;
    case 3:
      ThisRobot->TextToBeDisplayed="System error. Pleae buy an upgrade from MS.";
      break;
    case 4:
      ThisRobot->TextToBeDisplayed="System error. Press any key to reboot.";
      break;
    }

  return;

}; // void EnemyHitByBullet( int Enum );

void 
EnemyInfluCollisionText ( int Enum )
{
  Enemy ThisRobot=&AllEnemys[ Enum ];

  if ( !GameConfig.Enemy_Bump_Text ) return;
  
  ThisRobot->TextVisibleTime=0;
	      
  switch (MyRandom(1))
    {
    case 0:
      ThisRobot->TextToBeDisplayed="Hey, I'm from MS! Walk outa my way!";
      break;
    case 1:
      ThisRobot->TextToBeDisplayed="Hey, I know the big MS boss! You better go.";
      break;
    }

  return;

} // void AddStandingAndAimingText( int Enum )


void 
AddStandingAndAimingText ( int Enum )
{
  Enemy ThisRobot=&AllEnemys[ Enum ];

  if ( !GameConfig.Enemy_Aim_Text ) return;
  
  ThisRobot->TextVisibleTime=0;
	      
  if ( ( fabsf (Me.speed.x) < 1 ) && ( fabsf (Me.speed.y) < 1 ) )
    {
      ThisRobot->TextToBeDisplayed="Yeah, stay like that, haha.";
    }
  else
    {
      ThisRobot->TextToBeDisplayed="Stand still while I aim at you.";
    }

} // void AddStandingAndAimingText( int Enum )


void
AddInfluBurntText( void )
{
  int FinalTextNr;

  if ( !GameConfig.Influencer_Blast_Text ) return;
  
  Me.TextVisibleTime=0;
  
  FinalTextNr=MyRandom ( 6 );
  switch ( FinalTextNr )
    {
    case 0:
      Me.TextToBeDisplayed="Aaarrgh, aah, that burnt me!";
      break;
    case 1:
      Me.TextToBeDisplayed="Hell, that blast was hot!";
      break;
    case 2:
      Me.TextToBeDisplayed="Ghaart, I hate to stain my chassis like that.";
      break;
    case 3:
      Me.TextToBeDisplayed="Oh no!  I think I've burnt a cable!";
      break;
    case 4:
      Me.TextToBeDisplayed="Oh no, my poor transfer connectors smolder!";
      break;
    case 5:
      Me.TextToBeDisplayed="I hope that didn't melt any circuits!";
      break;
    case 6:
      Me.TextToBeDisplayed="So that gives some more black scars on me ol' dented chassis!";
      break;
    default:
      printf("\nError in AddInfluBurntText! That shouldn't be happening.");
      Terminate(ERR);
      break;
    }
} // void AddInfluBurntText

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
 *  scrolls a given text down inside the given rect
 *
 * returns : 0  if end of text was scolled out
 *           1  if user pressed space
 *-----------------------------------------------------------------*/
int
ScrollText (char *Text, SDL_Rect *rect)
{
  int Number_Of_Line_Feeds = 0;		/* Anzahl der Textzeilen */
  char *textpt;			/* bewegl. Textpointer */
  float InsertLine = 1.0*rect->y;
  int speed = 30;   // in pixel / sec
  int maxspeed = 150;
  SDL_Surface* Background;
  int ret = 0;
  Uint32 prev_tick, now;
  bool just_started = TRUE;

  Background = SDL_DisplayFormat( ne_screen );

  // count the number of lines in the text
  textpt = Text;
  while (*textpt++)
    if (*textpt == '\n')
      Number_Of_Line_Feeds++;

  while (1)
    {
      prev_tick = SDL_GetTicks ();
      SDL_BlitSurface ( Background , NULL , ne_screen , NULL );
      if (!DisplayText (Text, rect->x, (int)InsertLine, rect))
	{
	  ret = 0;  /* Text has been scrolled outside Rect */
	  break;  
	}
      SDL_Flip (ne_screen);


      if (just_started)
	{
	  just_started = FALSE;
	  now = SDL_GetTicks();
	  while ( !SpacePressed() && (SDL_GetTicks() - now < SHOW_WAIT)) ;  // wait before scrolling
	  if (SpacePressed())
	    {
	      ret = 1;
	      break;
	    }
	  prev_tick = SDL_GetTicks ();
	}


      if (UpPressed () || WheelUpPressed())
	{
	  speed -= 5;
	  if (speed < -maxspeed)
	    speed = -maxspeed;
	}
      if (DownPressed () || WheelDownPressed ())
	{
	  speed +=5;
	  if (speed > maxspeed)
	    speed = maxspeed;
	}
      if (SpacePressed())
	{
	  ret = 1;
	  break;
	}

      InsertLine -= 1.0 * (SDL_GetTicks() - prev_tick) * speed /1000.0;

      /* Nicht bel. nach unten wegscrollen */
      if (InsertLine > rect->y + rect->h)
	{
	  InsertLine = rect->y + rect->h;
	  if (speed < 0) speed = 0;
	}

    } /* while 1 */

  SDL_BlitSurface (Background, NULL, ne_screen, NULL);
  SDL_Flip (ne_screen);
  SDL_FreeSurface( Background );

  return (ret);
}				// void ScrollText(void)

/*-----------------------------------------------------------------
 * This function is much like DisplayText, but with the main difference,
 * that in case of the whole clipping window filled, the function will
 * display a ---more--- line, wait for a key and then scroll the text
 * further up.
 *
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
DisplayTextWithScrolling (char *Text, int startx, int starty, const SDL_Rect *clip , SDL_Surface* Background )
{
  char *tmp;	/* Beweg. Zeiger auf aktuelle Position im Ausgabe-Text */
  // SDL_Rect Temp_Clipping_Rect; // adding this to prevent segfault in case of NULL as parameter

  SDL_Rect store_clip;

  if ( startx != -1 ) MyCursorX = startx;		
  if ( starty != -1 ) MyCursorY = starty;

  SDL_GetClipRect (ne_screen, &store_clip);  /* store previous clip-rect */
  if (clip)
    SDL_SetClipRect (ne_screen, clip);
  else
    {
      clip = & User_Rect;
    }


  tmp = Text;			/* running text-pointer */

  while ( *tmp && (MyCursorY < clip->y + clip->h) )
    {
      if ( *tmp == '\n' )
	{
	  MyCursorX = clip->x;
	  MyCursorY += FontHeight ( GetCurrentFont() ) * TEXT_STRETCH;

	}
      else
	DisplayChar (*tmp);
      tmp++;

      //--------------------
      // Here we plant in the question for ---more--- and a key to be pressed,
      // before we clean the screen and restart displaying text from the top
      // of the given Clipping rectangle
      //
      // if ( ( clip->h + clip->y - MyCursorY ) <= 2 * FontHeight ( GetCurrentFont() ) * TEXT_STRETCH )
      if ( ( clip->h + clip->y - MyCursorY ) <= 1 * FontHeight ( GetCurrentFont() ) * TEXT_STRETCH )
	{
	  DisplayText( "--- more --- more --- \n" , MyCursorX , MyCursorY , clip );
	  SDL_Flip( ne_screen );
	  while ( !SpacePressed() );
	  while (  SpacePressed() );
	  SDL_BlitSurface( Background , NULL , ne_screen , NULL );
	  MyCursorY = clip->y;
	  SDL_Flip( ne_screen );
	};
      
      if (clip)
	ImprovedCheckUmbruch(tmp, clip);   /* dont write over right border */

    } // while !FensterVoll()

   SDL_SetClipRect (ne_screen, &store_clip); /* restore previous clip-rect */

  /*
   * ScrollText() wants to know if we still wrote something inside the
   * clip-rectangle, of if the Text has been scrolled out
   */
   if ( clip && ((MyCursorY < clip->y) || (starty > clip->y + clip->h) ))
     return FALSE;  /* no text was written inside clip */
   else
     return TRUE; 

};

/*-----------------------------------------------------------------
 * @Desc: prints *Text beginning at positions startx/starty, 
 * 
 *	and respecting the text-borders set by clip_rect
 *      -> this includes clipping but also automatic line-breaks
 *      when end-of-line is reached
 * 
 *    if startx/y == -1, write at current position, given by MyCursorX/Y.
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
  SDL_Rect Temp_Clipping_Rect; // adding this to prevent segfault in case of NULL as parameter

  SDL_Rect store_clip;

  if ( startx != -1 ) MyCursorX = startx;		
  if ( starty != -1 ) MyCursorY = starty;


  SDL_GetClipRect (ne_screen, &store_clip);  /* store previous clip-rect */
  if (clip)
    SDL_SetClipRect (ne_screen, clip);
  else
    {
      clip = & Temp_Clipping_Rect;
      Temp_Clipping_Rect.x=0;
      Temp_Clipping_Rect.y=0;
      Temp_Clipping_Rect.w=SCREENLEN;
      Temp_Clipping_Rect.h=SCREENHEIGHT;
    }


  tmp = Text;			/* running text-pointer */

  while ( *tmp && (MyCursorY < clip->y + clip->h) )
    {
      if ( *tmp == '\n' )
	{
	  MyCursorX = clip->x;
	  MyCursorY += FontHeight ( GetCurrentFont() ) * TEXT_STRETCH;
	}
      else
	DisplayChar (*tmp);

      tmp++;

      if (clip)
	ImprovedCheckUmbruch(tmp, clip);   /* dont write over right border */

    } // while !FensterVoll()

   SDL_SetClipRect (ne_screen, &store_clip); /* restore previous clip-rect */

  /*
   * ScrollText() wants to know if we still wrote something inside the
   * clip-rectangle, of if the Text has been scrolled out
   */
   if ( clip && ((MyCursorY < clip->y) || (starty > clip->y + clip->h) ))
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

  PutChar (ne_screen, MyCursorX, MyCursorY, c);

  // After the char has been displayed, we must move the cursor to its
  // new position.  That depends of course on the char displayed.
  //
  MyCursorX += CharWidth ( GetCurrentFont() , c);
  
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
	    NeededSpace+=CharWidth( GetCurrentFont() , Resttext[i] );
	    if ( MyCursorX+NeededSpace > clip->x + clip->w - 10 )
	      {
		MyCursorX = clip->x;
		MyCursorY += FontHeight ( GetCurrentFont() ) * TEXT_STRETCH;
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
 * @Ret: char *: String is allocated _here_!!!
 *       (dont forget to free it !)
 * 
 *-----------------------------------------------------------------*/
char *
GetString (int MaxLen, int echo)
{
  char *input;		/* Pointer auf eingegebenen String */
  int key;             /* last 'character' entered */
  int curpos;		/* zaehlt eingeg. Zeichen mit */
  int finished;
  int x0, y0, height;
  SDL_Rect store_rect, tmp_rect;
  SDL_Surface *store = NULL;

  if (echo == 1)		/* echo to stdout */
    {
      printf ("\nGetString(): sorry, echo=1 currently not implemented!\n");
      return NULL;
    }

  x0 = MyCursorX;
  y0 = MyCursorY;
  height = FontHeight (GetCurrentFont());
  
  store = SDL_CreateRGBSurface(0, SCREENLEN, height, ne_bpp, 0, 0, 0, 0);
  Set_Rect (store_rect, x0, y0, SCREENLEN, height);
  SDL_BlitSurface (ne_screen, &store_rect, store, NULL);

  /* Speicher fuer Eingabe reservieren */
  input     = MyMalloc (MaxLen + 5);

  memset (input, '.', MaxLen);
  input[MaxLen] = 0;
  
  finished = FALSE;
  curpos = 0;

  while ( !finished  )
    {
      Copy_Rect( store_rect, tmp_rect);
      SDL_BlitSurface (store, NULL, ne_screen, &tmp_rect);
      PutString (ne_screen, x0, y0, input);
      SDL_Flip (ne_screen);
      
      key = getchar_raw ();  
      
      if (key == SDLK_RETURN) 
	{
	  input[curpos] = 0;
	  finished = TRUE;
	}
      else if (isprint (key) && (curpos < MaxLen) )  
	{
	  /* printable characters are entered in string */
	  input[curpos] = (char) key;   
	  curpos ++;
	}
      else if (key == SDLK_BACKSPACE)
	{
	  if ( curpos > 0 ) curpos --;
	  input[curpos] = '.';
	}
      
    } /* while(!finished) */

  DebugPrintf (2, "\n\nchar *GetString(..):  The final string is:\n");
  DebugPrintf (2,  input );
  DebugPrintf (2, "\n\n");


  return (input);

} /* GetString() */

/*-----------------------------------------------------------------
 *
 * similar to putchar(), using SDL via the BFont-fct PutChar().
 *
 * sets MyCursor[XY], and allows passing (-1,-1) as coords to indicate
 *  using the current cursor position.
 *
 *-----------------------------------------------------------------*/
int
putchar_SDL (SDL_Surface *Surface, int x, int y, int c)
{
  int ret;
  if (x == -1) x = MyCursorX;
  if (y == -1) y = MyCursorY;
  
  MyCursorX = x + CharWidth (GetCurrentFont(), c);
  MyCursorY = y;

  ret = PutChar (Surface, x, y, c);

  SDL_Flip (Surface);

  return (ret);
}


/*-----------------------------------------------------------------
 * behaves similarly as gl_printf() of svgalib, using the BFont
 * print function PrintString().
 *  
 *  sets current position of MyCursor[XY],  
 *     if last char is '\n': to same x, next line y
 *     to end of string otherwise
 *
 * Added functionality to PrintString() is: 
 *  o) passing -1 as coord uses previous x and next-line y for printing
 *  o) Screen is updated immediatly after print, using SDL_flip()                       
 *
 *-----------------------------------------------------------------*/
void
printf_SDL (SDL_Surface *screen, int x, int y, char *fmt, ...)
{
  va_list args;
  int i, h;

  char *tmp;
  va_start (args, fmt);

  if (x == -1) x = MyCursorX;
  else MyCursorX = x;

  if (y == -1) y = MyCursorY;
  else MyCursorY = y;

  tmp = (char *) MyMalloc (10000 + 1);
  vsprintf (tmp, fmt, args);
  PutString (screen, x, y, tmp);
  h = FontHeight (GetCurrentFont()) + 2;
  SDL_UpdateRect (screen, x, y, SCREENLEN - x, h);  // update the relevant line

  if (tmp[strlen(tmp)-1] == '\n')
    {
      MyCursorX = x;
      MyCursorY = y+ 1.1*h;
    }
  else
    {
      for (i=0; i < strlen(tmp); i++)
	MyCursorX += CharWidth (GetCurrentFont(), tmp[i]);
      MyCursorY = y;
    }

  free (tmp);
  va_end (args);
}


#undef _text_c
