/*
 *
 *   Copyright (c) 1994, 2002, 2003  Johannes Prix
 *   Copyright (c) 1994, 2002, 2003  Reinhard Prix
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

/*-----------------------------------------------------------------
 * highscore list mangagement
 *-----------------------------------------------------------------*/

#define _highscore_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"

/*-----------------------------------------------------------------
 *
 * set up a new highscore list: load from disk if found
 *
 *-----------------------------------------------------------------*/
void
InitHighscores (void)
{
  int i;
  char fname[255];
  FILE *file = NULL;

  if (ConfigDir[0] != '\0')
    {
      sprintf (fname, "%s/highscores", ConfigDir);
      if ( (file = fopen (fname, "r")) == NULL)
	DebugPrintf (0, "WARNING: no highscores file found... \n");
      else
        DebugPrintf (0, "Found highscore file '%s'\n", fname );
    }


  num_highscores = MAX_HIGHSCORES;  /* hardcoded for now... */
  Highscores = MyMalloc (num_highscores * sizeof(Highscore_entry) + 10);

  for (i=0; i< num_highscores; i++)
    {
      Highscores[i] = MyMalloc (sizeof(highscore_entry));
      if (file)
	fread (Highscores[i], sizeof(highscore_entry), sizeof(char), file);
      else
	{
	  strcpy (Highscores[i]->name, HS_EMPTY_ENTRY);
	  strcpy (Highscores[i]->date, " --- ");
	  Highscores[i]->score = -1;
	}
    }

  if (file) fclose (file);

  return;
} /* InitHighscores */


/*----------------------------------------------------------------------
 * Save highscores to disk
 *
 *----------------------------------------------------------------------*/
int
SaveHighscores (void)
{
  int i;
  char fname[255];
  FILE *file = NULL;

  if (ConfigDir[0] == '\0')
    {
      DebugPrintf (0, "WARNING: no config-dir found, cannot save highscores!\n");
      return (ERR);
    }

  sprintf (fname, "%s/highscores", ConfigDir);
  if ( (file = fopen (fname, "w")) == NULL)
    {
      DebugPrintf (0, "WARNING: failed to create highscores file. Giving up... \n");
      return (ERR);
    }

  for (i=0; i < MAX_HIGHSCORES;  i++)
    fwrite (Highscores[i], sizeof(highscore_entry), sizeof(char), file);

  fclose (file);
  DebugPrintf (0, "Successfully updated highscores file '%s'\n", fname );

  return (OK);

} // SaveHighscores

/*----------------------------------------------------------------------
 * managing of highscore entries: check if user has entered the list,
 * and if so, insert a new entry
 *
 *----------------------------------------------------------------------*/
void
UpdateHighscores (void)
{
  int i, entry;
  Highscore_entry new_entry;
  char *tmp_name;
  BFont_Info *prev_font;
  struct tm *timeinfo;
  time_t tsec;
  float score;
  SDL_Rect dst;
  int h;

  score = RealScore;

  // reset score counters
  RealScore = 0.0;
  ShowScore = 0.0;


  if (score <= 0)  /* don't even bother.. */
    return;

  Me.status = DEBRIEFING;

  /* now find out the position of player's score in list */
  entry = 0;
  while ( (entry < num_highscores) && (Highscores[entry]->score >= score) )
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
  Set_Rect (dst, UserCenter_x - Portrait_Rect.w/2, UserCenter_y - Portrait_Rect.h/2,
	    Portrait_Rect.w, Portrait_Rect.h);
  SDL_BlitSurface (pic999, NULL, ne_screen, &dst);
  h = FontHeight (Para_BFont);
  DisplayText ("Great Score !",  dst.x - h, dst.y - h, &User_Rect);
#if !defined ANDROID && !defined ARCADEINPUT
  DisplayText ("Enter your name: ",  dst.x - 5*h, dst.y + dst.h, &User_Rect);
#endif
#ifdef ARCADEINPUT 
  DisplayText ("Enter with U/D, (L/R skip 5 chars),",  dst.x - 6*h, dst.y + dst.h, &User_Rect);
  DisplayText ("Act = toggle case, Fire to enter,",  dst.x - 5*h, dst.y + dst.h + h, &User_Rect);
  DisplayText ("Start when ready: ",  dst.x - 5*h, dst.y + dst.h + h*2, &User_Rect);
#endif
  SDL_Flip (ne_screen);

  SDL_SetClipRect (ne_screen, NULL);
#if !defined ANDROID
  tmp_name = GetString (MAX_NAME_LEN, 2);
  strcpy (new_entry->name, tmp_name);
  free (tmp_name);
#else
  strcpy (new_entry->name, "Player");
#endif
  printf_SDL(ne_screen, -1, -1, "\n");

  tsec = time (NULL);
  timeinfo = gmtime (&tsec);
  sprintf (new_entry->date, "20%02d/%02d/%02d", timeinfo->tm_year - 100, timeinfo->tm_mon +1, timeinfo->tm_mday );

  new_entry->score = score;
  Highscores[entry] = new_entry;

  SetCurrentFont (prev_font);

  return;

} /* UpdateHighscores */


/*-----------------------------------------------------------------
 *
 * Display the high scores of the single player game.
 * This function is actually a submenu of the MainMenu.
 *
 *-----------------------------------------------------------------*/
void
ShowHighscores (void)
{
  int x0, x1, x2, x3;
  int y0, height, len;
  int i;
  BFont_Info *prev_font;
  char *fpath;

  fpath = find_file (HS_BACKGROUND_FILE, GRAPHICS_DIR, NO_THEME, WARNONLY);
  if(fpath) DisplayImage (fpath);
  MakeGridOnScreen ( (SDL_Rect*) & Screen_Rect );
  DisplayBanner( NULL , NULL , BANNER_FORCE_UPDATE );

  prev_font = GetCurrentFont();
  SetCurrentFont (Highscore_BFont);

  len = CharWidth (GetCurrentFont(), '9');

  x0 = Screen_Rect.w/8;
  x1 = x0 + 2*len;
  x2 = x1 + 11*len;
  x3 = x2 + MAX_NAME_LEN*len;

  height = FontHeight (GetCurrentFont());

  y0 = Full_User_Rect.y + height;


  CenteredPrintString (ne_screen, y0, "Top %d  scores\n", num_highscores);

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

  wait_for_key_pressed();

  SetCurrentFont (prev_font);

  return;
} // ShowHighscores



#undef _highscore_c

