/*----------------------------------------------------------------------
 *
 * Desc: contains functions to update and draw the top status line with
 *	score and status etc...
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
#define _rahmen_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "proto.h"
#include "global.h"
#include "text.h"

extern char *InfluenceModeNames[];

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

} // void DrawBar(...)

/*
-----------------------------------------------------------------
@Desc: This function updates the top status bar.
To save framerate on slow machines however it will only work
if it thinks that work needs to be done. 
You can however force update if you say so with a flag.

BANNER_FORCE_UPDATE=1: Forces the redrawing of the title bar

BANNER_DONT_TOUCH_TEXT=2: Prevents DisplayBanner from touching the
text, i.e. calling SetInfoline            

BANNER_NO_SDL_UPDATE=4: Prevents any SDL_Update calls.

To update the information in the top status line only, you can
as well use the function SetInfoline.

-----------------------------------------------------------------
*/
void
DisplayBanner ( int flags )
{
  SDL_Rect TargetRectangle;
  char dummy[80];
  char left_box [LEFT_TEXT_LEN + 10];
  char right_box[RIGHT_TEXT_LEN + 10];
  static char previous_left_box [LEFT_TEXT_LEN + 10]="NOUGHT";
  static char previous_right_box[RIGHT_TEXT_LEN + 10]="NOUGHT";
  int left_len, right_len;   /* the actualy string-lens */
  char *left;
  char *right;


  // --------------------
  // At first the text is prepared.  This can't hurt.
  // we will decide whether to dispaly it or not later...
  //
  left = InfluenceModeNames[Me.status];
  right =  ltoa (ShowScore, dummy, 10);
  
  // Now fill in the text
  left_len = strlen (left);
  if( left_len > LEFT_TEXT_LEN )
    {
      printf ("\nWarning: String %s too long for Left Infoline!!",left);
      left_len = LEFT_TEXT_LEN;  /* too long, so we cut it! */
      Terminate(ERR);
    }
  right_len = strlen (right);
  if( right_len > RIGHT_TEXT_LEN )
    {
      printf ("\nWarning: String %s too long for Right Infoline!!", right);
      right_len = RIGHT_TEXT_LEN;  /* too long, so we cut it! */
      Terminate(ERR);
    }
  
  /* Now prepare the left/right text-boxes */
  memset (left_box,  ' ', LEFT_TEXT_LEN);  /* pad with spaces */
  memset (right_box, ' ', RIGHT_TEXT_LEN);  
  
  strncpy (left_box,  left, left_len);  /* this drops terminating \0 ! */
  strncpy (right_box, right, left_len);  /* this drops terminating \0 ! */
  
  left_box [LEFT_TEXT_LEN]  = '\0';     /* that's right, we want padding! */
  right_box[RIGHT_TEXT_LEN] = '\0';
  
  // --------------------
  // No we see if the screen need an update...

  if ( BannerIsDestroyed || 
       (flags & BANNER_FORCE_UPDATE ) || 
       (strcmp( left_box , previous_left_box )) || 
       (strcmp( right_box , previous_right_box )) )
    {
      // Redraw the whole background of the top status bar
      TargetRectangle.x=0;
      TargetRectangle.y=0;
      SDL_SetClipRect( ne_screen , NULL );  // this unsets the clipping rectangle
      SDL_BlitSurface( ne_static , ne_rahmen_block , ne_screen , &TargetRectangle );

      // Now the text should be ready and its
      // time to display it...
      if ( (strcmp( left_box , previous_left_box )) || 
	   (strcmp( right_box , previous_right_box )) ||
	   ( flags & BANNER_FORCE_UPDATE ) )
	{
	  PrintStringFont ( ne_screen , Menu_BFont, LEFT_INFO_X , LEFT_INFO_Y , left_box );
	  strcpy( previous_left_box , left_box );
	  PrintStringFont ( ne_screen , Menu_BFont, RIGHT_INFO_X , RIGHT_INFO_Y , right_box );
	  strcpy( previous_right_box , right_box );
	  // printf("\nHad to update top status line box...");
	}

      // finally update the whole top status box
      // printf("\nHad to update whole top status line box...");
      if ( !(flags & BANNER_NO_SDL_UPDATE ) )SDL_UpdateRect( ne_screen, 0, 0, BANNER_WIDTH , BANNER_HEIGHT );
      BannerIsDestroyed=FALSE;
      return;
    }

} /* DisplayBanner() */


/*-----------------------------------------------------------------
 * @Desc: setzt Infos im Banner neu BUT does NOT update screen!!
 * 
 *  *left, *right: pointer to strings to display left and right
 *                 ! strings longer than LEFT/RIGHT_TEXT_LEN get cut
 *

 * NULL-pointer indicates to display default: left=MODE, right=SCORE 
 * 
 *-----------------------------------------------------------------*/
void
SetInfoline (const char *left, const char *right , int flags )
{
  char dummy[80];
  char left_box [LEFT_TEXT_LEN + 10];
  char right_box[RIGHT_TEXT_LEN + 10];
  static char previous_left_box [LEFT_TEXT_LEN + 10]="NOUGHT";
  static char previous_right_box[RIGHT_TEXT_LEN + 10]="NOUGHT";
  int left_len, right_len;   /* the actualy string-lens */

  if (left == NULL)       /* Left-DEFAULT: Mode */
    left = InfluenceModeNames[Me.status];

  if ( right == NULL )  /* Right-DEFAULT: Score */
    right =  ltoa (ShowScore, dummy, 10);


  left_len = strlen (left);
  if( left_len > LEFT_TEXT_LEN )
    {
      printf ("\nWarning: String %s too long for Left Infoline!!",left);
      left_len = LEFT_TEXT_LEN;  /* too long, so we cut it! */
      Terminate(ERR);
    }
  right_len = strlen (right);
  if( right_len > RIGHT_TEXT_LEN )
    {
      printf ("\nWarning: String %s too long for Right Infoline!!", right);
      right_len = RIGHT_TEXT_LEN;  /* too long, so we cut it! */
      Terminate(ERR);
    }

  /* Now prepare the left/right text-boxes */
  memset (left_box,  ' ', LEFT_TEXT_LEN);  /* pad with spaces */
  memset (right_box, ' ', RIGHT_TEXT_LEN);  

  strncpy (left_box,  left, left_len);  /* this drops terminating \0 ! */
  strncpy (right_box, right, left_len);  /* this drops terminating \0 ! */

  left_box [LEFT_TEXT_LEN]  = '\0';     /* that's right, we want padding! */
  right_box[RIGHT_TEXT_LEN] = '\0';

  /* Hintergrund Textfarbe setzen */
  SetTextColor (BANNER_WHITE, BANNER_VIOLETT);	// FONT_RED, 0
  
  SDL_SetClipRect( ne_screen , NULL );
  // Now the text should be ready and its
  // time to display it...
  if ( (strcmp( left_box , previous_left_box )) || 
       (strcmp( right_box , previous_right_box )) ||
       ( flags & BANNER_FORCE_UPDATE ) )
    {
      SetCurrentFont(Para_BFont);
      DisplayBanner( BANNER_FORCE_UPDATE | BANNER_DONT_TOUCH_TEXT );
      PrintStringFont ( ne_screen , Para_BFont, LEFT_INFO_X , LEFT_INFO_Y , left_box );
      // SDL_UpdateRect( ne_screen, LEFT_INFO_X, LEFT_INFO_Y, FontHeight(Menu_BFont)*8, FontHeight(Menu_BFont) );
      strcpy( previous_left_box , left_box );
      PrintStringFont ( ne_screen , Para_BFont, RIGHT_INFO_X , RIGHT_INFO_Y , right_box );
      // SDL_UpdateRect( ne_screen, RIGHT_INFO_X, RIGHT_INFO_Y, FontHeight(Menu_BFont)*4, FontHeight(Menu_BFont) );
      strcpy( previous_right_box , right_box );
      printf("\nHad to update top status line box...");
    }

  return;

} /* SetInfoline () */

#undef _rahmen_c
