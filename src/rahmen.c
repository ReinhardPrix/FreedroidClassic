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

}				// void DrawBar(...)

/*-----------------------------------------------------------------
 * @Desc: well, do just that
 *
 * NOTE: this function used to update also the infoline display,
 *    this is now down by a separate call to SetInfoline(left,right)!!
 *
 *-----------------------------------------------------------------*/
void
DisplayRahmen (unsigned char *Parameter_Screen)
{
#ifdef NEW_ENGINE
  return;
#endif
  DisplayMergeBlock(0, 0, RahmenPicture, RAHMENBREITE, RAHMENHOEHE, Parameter_Screen);

  return;

} /* DisplayRahmen() */


/*-----------------------------------------------------------------
 * @Desc: setzt Infos im Rahmen neu BUT does NOT update screen!!
 * 
 *  *left, *right: pointer to strings to display left and right
 *                 ! strings longer than LEFT/RIGHT_TEXT_LEN get cut
 *
 * NULL-pointer indicates to display default: left=MODE, right=SCORE 
 * 
 *-----------------------------------------------------------------*/
void
SetInfoline (const char *left, const char *right)
{
  char dummy[80];
  char left_box [LEFT_TEXT_LEN + 10];
  char right_box[RIGHT_TEXT_LEN + 10];
  int left_len, right_len;   /* the actualy string-lens */

#ifdef NEW_ENGINE
  return;
#endif
  if (left == NULL)       /* Left-DEFAULT: Mode */
    left = InfluenceModeNames[Me.status];

  if ( right == NULL )  /* Right-DEFAULT: Score */
    right =  ltoa (ShowScore, dummy, 10);


  left_len = strlen (left);
  if( left_len > LEFT_TEXT_LEN )
    {
      printf ("\nWarning: String %s too long for Left Infoline!!",left);
      left_len = LEFT_TEXT_LEN;  /* too long, so we cut it! */
    }
  right_len = strlen (right);
  if( right_len > RIGHT_TEXT_LEN )
    {
      printf ("\nWarning: String %s too long for Right Infoline!!", right);
      right_len = RIGHT_TEXT_LEN;  /* too long, so we cut it! */
    }

  /* Now prepare the left/right text-boxes */
  memset (left_box,  ' ', LEFT_TEXT_LEN);  /* pad with spaces */
  memset (right_box, ' ', RIGHT_TEXT_LEN);  

  strncpy (left_box,  left, left_len);  /* this drops terminating \0 ! */
  strncpy (right_box, right, left_len);  /* this drops terminating \0 ! */

  left_box [LEFT_TEXT_LEN]  = '\0';     /* that's right, we want padding! */
  right_box[RIGHT_TEXT_LEN] = '\0';

  /* Hintergrund Textfarbe setzen */
  SetTextColor (RAHMEN_WHITE, RAHMEN_VIOLETT);	// FONT_RED, 0

  /* Text ausgeben */
  DisplayText (left_box, LEFTINFO_X, LEFTINFO_Y, Outline320x200, FALSE);
  DisplayText (right_box, RIGHTINFO_X, RIGHTINFO_Y, Outline320x200, FALSE);

  return;

} /* SetInfoline () */

#undef _rahmen_c
