/*-----------------------------------------------------------------
 * highscore list mangagement 
 *-----------------------------------------------------------------*/

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
#define _highscore_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"

/*-----------------------------------------------------------------
 *
 * set up a new highscore list: currently empty, but could be
 * read from disk, or even loaded from net in the future...
 *
 *-----------------------------------------------------------------*/
void 
Init_Highscores (void)
{
  int i;

  num_highscores = MAX_HIGHSCORES;  /* hardcoded for now... */
  Highscores = MyMalloc ( num_highscores * sizeof(Highscore_entry) + 10);
  
  for (i=0; i< num_highscores; i++)
    {
      Highscores[i] = MyMalloc (sizeof(highscore_entry));
      strcpy (Highscores[i]->name, HS_EMPTY_ENTRY);
      strcpy (Highscores[i]->date, " --- ");
      Highscores[i]->score = -1;
      Highscores[i]->mission = -1;
    }

  return;
} /* Init_Highscores */

/*----------------------------------------------------------------------
 * managing of highscore entries: check if user has entered the list, 
 * and if so, insert a new entry
 *
 *----------------------------------------------------------------------*/
void
update_highscores (void)
{
  int i, entry;
  Highscore_entry new_entry;
  char *tmp_name;
  BFont_Info *prev_font;
  struct tm *timeinfo;
  time_t tsec;

  if (RealScore <= 0)  /* don't even bother.. */
    return;

  Me.status = DEBRIEFING;

  /* now find out the position of player's score in list */
  entry = 0;
  while (Highscores[entry]->score >= RealScore )
    entry ++;

  if (entry == num_highscores) /* sorry, you didnt' make it */
    return;

  /* ok, the last one always has to go... */
  free (Highscores[num_highscores-1]);

  /* now shuffle down the lower scores to make space */
  for (i=num_highscores-1; i> entry; i--)
    Highscores[i] = Highscores[i-1];

  /* get the new entry */
  new_entry = MyMalloc (sizeof(highscore_entry));

  prev_font = GetCurrentFont();
  SetCurrentFont (Highscore_BFont);

  Assemble_Combat_Picture ( 0);
  MakeGridOnScreen (&User_Rect);
  ShowRobotPicture (UserCenter_x -70, UserCenter_y - 80, DRUID999);
  DisplayText ("Great Score !",  UserCenter_x -90, UserCenter_y - 100, &User_Rect);
  DisplayText ("Enter your name: ",  UserCenter_x -160, UserCenter_y + 100, &User_Rect);
  SDL_Flip (ne_screen);

  tmp_name = GetString (MAX_NAME_LEN, 2);
  strcpy (new_entry->name, tmp_name);
  free (tmp_name);

  tsec = time (NULL);
  timeinfo = gmtime (&tsec);
  sprintf (new_entry->date, "[%d/%d/%02d]", timeinfo->tm_mday, timeinfo->tm_mon,
	   timeinfo->tm_year-100); 

  new_entry->score = RealScore;
  Highscores[entry] = new_entry;

  SetCurrentFont (prev_font);

  return;

} /* update_highscores */


/*-----------------------------------------------------------------
 *
 * Display the high scores of the single player game.  
 * This function is actually a submenu of the big EscapeMenu.
 *
 *-----------------------------------------------------------------*/
void
Show_Highscores (void)
{
  int x0, x1, x2, x3;
  int y0, height, len;
  int i;
  BFont_Info *prev_font;

  DisplayImage (find_file (HS_BACKGROUND_FILE, GRAPHICS_DIR, FALSE));
  MakeGridOnScreen ( (SDL_Rect*) & Full_Screen_Rect );
  DisplayBanner( NULL , NULL , BANNER_FORCE_UPDATE );

  prev_font = GetCurrentFont();
  SetCurrentFont (Highscore_BFont);  

  len = CharWidth (GetCurrentFont(), '9'); 

  x0 = SCREENLEN/8;
  x1 = x0 + 3*len;
  x2 = x1 + 9*len;
  x3 = x2 + MAX_NAME_LEN*len;
  y0 = 80;  
  height = FontHeight (GetCurrentFont());

  CenteredPrintString (ne_screen, y0, "Top %d  freedom fighters\n", num_highscores);
  
  for (i=0; i<num_highscores; i++)
    {
      PrintString (ne_screen, x0, y0 + (i+2)*height, "%d", i+1);
      if (Highscores[i]->score >= 0)
	PrintString (ne_screen, x1, y0 + (i+2)*height, "%s", Highscores[i]->date);
      PrintString (ne_screen, x2, y0 + (i+2)*height,  "%s", Highscores[i]->name);
      if (Highscores[i]->score >= 0)
	PrintString (ne_screen, x3, y0 + (i+2)*height, "%ld", Highscores[i]->score);
    }
  SDL_Flip (ne_screen);
  
  while (!SpacePressed() && !EscapePressed());
  while (SpacePressed() || EscapePressed());

  SetCurrentFont (prev_font);

  return;
} // Show_Highscores



#undef _highscore_c

