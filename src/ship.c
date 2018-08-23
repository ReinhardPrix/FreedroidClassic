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

/*----------------------------------------------------------------------
 *
 * Desc: the konsole- and lift functions
 *
 *----------------------------------------------------------------------*/

#define _ship_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"
#include "text.h"

int CursorIsOnRect (SDL_Rect *rect);
SDL_Rect up_rect,down_rect,left_rect,right_rect;
extern bool show_cursor;
extern int vid_bpp;

#define UPDATE_ONLY 0x01

//--------------------
// Definitions for the menu inside the in-game console
//
#define CONS_MENU_HEIGHT 		256
#define CONS_MENU_LENGTH 		100

#define WAIT_ELEVATOR		9	/* warte, bevor Lift weitergeht */

#define MENUTEXT_X	(132 + USERFENSTERPOSX + 5 )


SDL_Rect up_rect;
SDL_Rect down_rect;
SDL_Rect left_rect;
SDL_Rect right_rect;


/*-----------------------------------------------------------------
 * @Desc: does all the work when we enter a lift
 *
 *-----------------------------------------------------------------*/
void
EnterLift (void)
{
  int i;
  int curLevel;
  int curLift, upLift, downLift, liftrow;

  DebugPrintf (2, "\nvoid EnterLift(void): Function call confirmed.");

  /* Prevent distortion of framerate by the delay coming from
   * the time spend in the menu. */
  Activate_Conservative_Frame_Computation();

  /* make sure to release the fire-key */
  SpacePressedR();
  MouseLeftPressedR();
  MouseRightPressedR();

  /* Prevent the influ from coming out of the lift in transfer mode
   * by turning off transfer mode as soon as the influ enters the lift */
  Me.status= ELEVATOR;

  ResetMouseWheel ();  // forget previous mouse-wheel action
  SDL_ShowCursor(SDL_DISABLE);

  curLevel = CurLevel->levelnum;

  if ((curLift = GetCurrentLift ()) == -1)
    {
      printf ("Lift out of order, I'm so sorry !");
      return;
    }

  EnterLiftSound ();
  Switch_Background_Music_To (NULL); // turn off Bg music

  upLift = curShip.AllLifts[curLift].up;
  downLift = curShip.AllLifts[curLift].down;

  liftrow = curShip.AllLifts[curLift].lift_row;

  // clear the whole screen
  ClearGraphMem();
  DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE );

  const Uint32 wait_move_ticks = 100;
  static Uint32 last_move_tick = 0;
  MenuAction_t action = ACTION_NONE;
  bool finished = FALSE;
  while ( !finished )
    {
      ShowLifts (curLevel, liftrow);

      action = getMenuAction( 500 );
      if ( SDL_GetTicks() - last_move_tick > wait_move_ticks )
        {
          switch ( action )
            {
            case ACTION_CLICK:
              finished = TRUE;
              break;

            case ACTION_UP:
              last_move_tick = SDL_GetTicks();
              if (upLift != -1)
                {			/* gibt es noch einen Lift hoeher ? */
                  if (curShip.AllLifts[upLift].x == 99)
                    {
                      DebugPrintf (0, "Lift out of order, so sorry ..");
                    }
                  else
                    {
                      downLift = curLift;
                      curLift = upLift;
                      curLevel = curShip.AllLifts[curLift].level;
                      upLift = curShip.AllLifts[curLift].up;
                      ShowLifts (curLevel, liftrow);
                      MoveLiftSound ();
                    }
                } /* if uplevel */
              break;

            case ACTION_DOWN:
              last_move_tick = SDL_GetTicks();
              if (downLift != -1)
                {			/* gibt es noch einen Lift tiefer ? */
                  if (curShip.AllLifts[downLift].x == 99)
                    {
                      DebugPrintf (0, "Lift Out of order, so sorry ..");
                    }
                  else
                    {
                      upLift = curLift;
                      curLift = downLift;
                      curLevel = curShip.AllLifts[curLift].level;
                      downLift = curShip.AllLifts[curLift].down;
                      ShowLifts (curLevel, liftrow);
                      MoveLiftSound ();
                    }
                } /* if downlevel */
              break;
            default:
              break;
            } // switch(action)
        }
      SDL_Delay(1);	// don't hog CPU
    } // while !finished

  //--------------------
  // It might happen, that the influencer enters the elevator, but then decides to
  // come out on the same level where he has been before.  In this case of course there
  // is no need to reshuffle enemys or to reset influencers position.  Therefore, only
  // when a real level change has occured, we need to do real changes as below, where
  // we set the new level and set new position and initiate timers and all that...
  //
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

      // redistribute the enemys around the level
      //      ShuffleEnemys ();

      // set the position of the influencer to the correct locatiohn
      Me.pos.x = curShip.AllLifts[curLift].x;
      Me.pos.y = curShip.AllLifts[curLift].y;

      for (i = 0; i < MAXBLASTS; i++)
	DeleteBlast( i );
      for (i = 0; i < MAXBULLETS; i++)
	DeleteBullet ( i ) ;
    } // if real level change has occured

  LeaveLiftSound ( );
  Switch_Background_Music_To (CurLevel->Background_Song_Name);
  ClearGraphMem ( );
  DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE );

  // UnfadeLevel ();

  Me.status = MOBILE;
  Me.TextVisibleTime=0;
  Me.TextToBeDisplayed=CurLevel->Level_Enter_Comment;

  DebugPrintf (2, "\nvoid EnterLift(void): Usual end of function reached.");

  return;
}	/* EnterLift */

/*-----------------------------------------------------------------
 * @Desc: show side-view of the ship, and hightlight the current
 *        level + lift
 *
 *  if level==-1: don't highlight any level
 *  if liftrow==-1: dont' highlight any liftrows
 *
 *-----------------------------------------------------------------*/
void
ShowLifts (int level, int liftrow)
{
  SDL_Rect src, dst;
  int i;
  SDL_Color lift_bg_color = {0,0,0};  /* black... */
  int xoffs = User_Rect.w/20;
  int yoffs = User_Rect.h/5;

  SDL_ShowCursor (SDL_DISABLE);
  // fill the user fenster with some color
  Fill_Rect (User_Rect, lift_bg_color);

  /* First blit ship "lights off" */
  Copy_Rect (User_Rect, dst);
  SDL_SetClipRect (ne_screen, &dst);
  Copy_Rect (User_Rect, dst);
  dst.x += xoffs;
  dst.y += yoffs;
  SDL_BlitSurface (ship_off_pic, NULL, ne_screen, &dst);

  if (level >= 0)
    for (i=0; i<curShip.num_level_rects[level]; i++)
      {
	Copy_Rect (curShip.Level_Rects[level][i], src);
	Copy_Rect (src, dst);
	dst.x += User_Rect.x + xoffs;   /* offset respective to User-Rectangle */
	dst.y += User_Rect.y + yoffs;
	SDL_BlitSurface (ship_on_pic, &src, ne_screen, &dst);
      }

  if (liftrow >=0)
    {
      Copy_Rect (curShip.LiftRow_Rect[liftrow], src);
      Copy_Rect (src, dst);
      dst.x += User_Rect.x + xoffs;   /* offset respective to User-Rectangle */
      dst.y += User_Rect.y + yoffs;
      SDL_BlitSurface (ship_on_pic, &src, ne_screen, &dst);
    }

  SDL_Flip (ne_screen);

  return;

} /* ShowLifts() */

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
  int i, pos, mousemove_buf;
  SDL_Rect TmpRect;
  // Prevent distortion of framerate by the delay coming from
  // the time spend in the menu.
  Activate_Conservative_Frame_Computation();

  Copy_Rect (User_Rect, TmpRect);
  Copy_Rect (Full_User_Rect, User_Rect);

  wait_for_all_keys_released();
  ResetMouseWheel ();

  Me.status = CONSOLE;

#ifdef ANDROID
  show_cursor = FALSE;
#endif

  SDL_SetCursor (arrow_cursor);

  SetCurrentFont( Para_BFont );

  pos = 0;   // starting menu position
  PaintConsoleMenu (pos, 0);

  /* Gesamtkonsolenschleife */
  const Uint32 wait_move_ticks = 100;
  static Uint32 last_move_tick = 0;
  MenuAction_t action = ACTION_NONE;
  bool finished = FALSE;
  bool need_update = TRUE;
  while ( !finished )
    {
      if (show_cursor) SDL_ShowCursor (SDL_ENABLE);
      else SDL_ShowCursor (SDL_DISABLE);

      // check if the mouse-cursor is on any of the console-menu points
      for (i=0; i < 4; i++)
	if (show_cursor && (pos != i) && CursorIsOnRect(&Cons_Menu_Rects[i]) )
	  {
	    MoveMenuPositionSound ();
	    pos = i;
            need_update = TRUE;
	  }
      action = getMenuAction( 250 );
      if ( SDL_GetTicks() - last_move_tick > wait_move_ticks )
        {
          switch ( action )
            {
            case ACTION_BACK:
	      finished = TRUE;
              break;

            case ACTION_UP:
              if (pos > 0) pos--;
              else pos = 3;
              // when warping the mouse-cursor: don't count that as a mouse-activity
              // this is a dirty hack, but that should be enough for here...
              if (show_cursor)
                {
                  mousemove_buf = last_mouse_event;
                  SDL_WarpMouse (Cons_Menu_Rects[pos].x+Cons_Menu_Rects[pos].w/2,
                                 Cons_Menu_Rects[pos].y+Cons_Menu_Rects[pos].h/2);
                  update_input ();  // this sets a new last_mouse_event
                  last_mouse_event = mousemove_buf; //... which we override.. ;)
                }
              MoveMenuPositionSound ();
              need_update = TRUE;
              last_move_tick = SDL_GetTicks();
              break;

            case ACTION_DOWN:
              if (pos < 3) pos++;
              else pos = 0;
              // when warping the mouse-cursor: don't count that as a mouse-activity
              // this is a dirty hack, but that should be enough for here...
              if (show_cursor)
                {
                  mousemove_buf = last_mouse_event;
                  SDL_WarpMouse (Cons_Menu_Rects[pos].x+Cons_Menu_Rects[pos].w/2,
                                 Cons_Menu_Rects[pos].y+Cons_Menu_Rects[pos].h/2);
                  update_input ();  // this sets a new last_mouse_event
                  last_mouse_event = mousemove_buf; //... which we override.. ;)
                }
              MoveMenuPositionSound ();
              need_update = TRUE;
              last_move_tick = SDL_GetTicks();
              break;

            case ACTION_CLICK:
              MenuItemSelectedSound();
              need_update = TRUE;
              switch (pos)
                {
                case 0:
                  finished = TRUE;
                  break;
                case 1:
                  GreatDruidShow ();
                  PaintConsoleMenu (pos, 0);
                  break;
                case 2:
                  ClearGraphMem();
                  DisplayBanner (NULL, NULL, BANNER_FORCE_UPDATE);
                  ShowDeckMap (CurLevel);
                  PaintConsoleMenu(pos, 0);
                  break;
                case 3:
                  ClearGraphMem();
                  DisplayBanner (NULL, NULL, BANNER_FORCE_UPDATE);
                  ShowLifts (CurLevel->levelnum, -1);
                  while (! (FirePressedR() || EscapePressedR() || MouseRightPressedR() ))
                    SDL_Delay(1);
                  PaintConsoleMenu(pos, 0);
                  break;
                default:
                  DebugPrintf (1, "Konsole menu out of bounds... pos = %d", pos);
                  pos = 0;
                  break;
                } // switch
              break;
            default:
              break;
            } // switch(action)
        } // if time-since-last move > wait

      if ( need_update ) {
        PaintConsoleMenu (pos, UPDATE_ONLY);
#ifndef ANDROID
        SDL_Flip (ne_screen);
#endif
        need_update = FALSE;
      }
#ifdef ANDROID
      SDL_Flip( ne_screen );	// for responsive input on Android, we need to run this every cycle
#endif
      SDL_Delay(1);	// don't hog CPU
    } // while(!finished)

  Copy_Rect (TmpRect, User_Rect);

  Me.status = MOBILE;

  ClearGraphMem();

  SDL_SetCursor (crosshair_cursor);
  if (!show_cursor)
    SDL_ShowCursor (SDL_DISABLE);

  return;

} // void EnterKonsole(void)

/*-----------------------------------------------------------------
 * @Desc: diese Funktion zeigt die m"oglichen Auswahlpunkte des Menus
 *    Sie soll die Schriftfarben nicht ver"andern
 *
 *  NOTE: this function does not actually _display_ anything yet,
 *        it just prepares the display, so you need
 *        to call SDL_Flip() to display the result!
 *  pos  : 0<=pos<=3: which menu-position is currently active?
 *  flag : UPDATE_ONLY  only update the console-menu bar, not text & background
 *-----------------------------------------------------------------*/
void
PaintConsoleMenu (int pos, int flag)
{
  char MenuText[200];
  SDL_Rect src;

  if ( !(flag & UPDATE_ONLY) )
    {
      ClearGraphMem ();
      SDL_SetClipRect ( ne_screen , NULL );
      SDL_BlitSurface( console_bg_pic1 , NULL , ne_screen , NULL );

      DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE );

      sprintf (MenuText, "Area : %s\nDeck : %s    Alert: %s",
	       curShip.AreaName, CurLevel->Levelname, Alertcolor[AlertLevel]);
      DisplayText (MenuText, Cons_Header_Rect.x, Cons_Header_Rect.y, &Cons_Header_Rect);

      sprintf (MenuText, "Logout from console\n\nDroid info\n\nDeck map\n\nShip map");
      DisplayText (MenuText, Cons_Text_Rect.x, Cons_Text_Rect.y+25, &Cons_Text_Rect);

    } // only if not UPDATE_ONLY was required

  src.x = Cons_Menu_Rects[0].w * pos + 2*pos*GameConfig.scale;
  src.y = 0;
  src.w = Cons_Menu_Rect.w;
  src.h = 4 * Cons_Menu_Rect.h;
  SDL_BlitSurface (console_pic, &src, ne_screen, &Cons_Menu_Rect);

  return;
}	// PaintConsoleMenu ()

/*-----------------------------------------------------------------
 * @Desc: Displays the concept view of Level "deck" in Userfenster
 *
 *	Note: we no longer wait here for a key-press, but return
 *            immediately
 *-----------------------------------------------------------------*/
void
ShowDeckMap (Level deck)
{
  finepoint tmp;
  tmp.x=Me.pos.x;
  tmp.y=Me.pos.y;

  Me.pos.x = CurLevel->xlen/2;
  Me.pos.y = CurLevel->ylen/2;

  SDL_ShowCursor (SDL_DISABLE);

  SetCombatScaleTo( 0.25 );

  Assemble_Combat_Picture( ONLY_SHOW_MAP|SHOW_FULL_MAP );

  SDL_Flip (ne_screen);

  Me.pos.x=tmp.x;
  Me.pos.y=tmp.y;

  while (! (FirePressedR() || EscapePressedR() || MouseRightPressedR() ))
    SDL_Delay(1);

  SetCombatScaleTo (1.0);

  return;
} /* ShowDeckMap() */

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
      if ((AllEnemys[i].levelnum == levelnum) &&
	  ( (AllEnemys[i].status != OUT) && (AllEnemys[i].status != TERMINATED) ) )
	return FALSE;
    }

  return TRUE;
}

/* ----------------------------------------------------------------------
 * This function should check if the mouse cursor is in the given Rectangle
 * ---------------------------------------------------------------------- */
int
CursorIsOnRect (SDL_Rect *rect)
{
  point CurPos;

  CurPos.x = input_axis.x + (UserCenter_x - 16);
  CurPos.y = input_axis.y + (UserCenter_y - 16);

  if ( (CurPos.x >= rect->x) && (CurPos.x <= rect->x + rect->w) )
    if ( (CurPos.y >= rect->y) && (CurPos.y <= rect->y + rect->h) )
      return (TRUE);

  return (FALSE);

}; // int CursorIsOnRect

/* ----------------------------------------------------------------------
 * This function does the robot show when the user has selected robot
 * show from the console menu.
 * ---------------------------------------------------------------------- */
void
GreatDruidShow (void)
{
  int droidtype;
  int page;
  bool finished = FALSE;

  droidtype = Me.type;
  page = 0;

  show_droid_info (droidtype, page, 0);
  show_droid_portrait (Cons_Droid_Rect, droidtype, 0.0, UPDATE|RESET);

  wait_for_all_keys_released();
  bool need_update = TRUE;
  while (!finished)
    {
      show_droid_portrait (Cons_Droid_Rect, droidtype, DROID_ROTATION_TIME, 0);

      if (show_cursor) SDL_ShowCursor (SDL_ENABLE);
      else SDL_ShowCursor (SDL_DISABLE);

      if ( need_update )
	{
	  show_droid_info (droidtype, page, UPDATE_ONLY);
	  need_update = FALSE;
	}

      MenuAction_t action = ACTION_NONE;
      // special handling of mouse-clicks: check if move-arrows were clicked on
      if (MouseLeftPressedR ()) {
        DebugPrintf ( 0, "MouseLeftPress registered\n");
        if ( CursorIsOnRect (&left_rect) ) {
          action = ACTION_LEFT;
        }
        else if (CursorIsOnRect (&right_rect) ) {
          action = ACTION_RIGHT;
        }
        else if (CursorIsOnRect (&up_rect) ) {
          action = ACTION_UP;
        }
        else if (CursorIsOnRect (&down_rect) ) {
          action = ACTION_DOWN;
        }
      } else {
        action = getMenuAction ( 250 );
      }

      switch ( action )
        {
        case ACTION_BACK:
          finished = TRUE;
          break;

        case ACTION_UP:
          if ( droidtype < Me.type ) {
            MoveMenuPositionSound();
            droidtype ++;
            need_update = TRUE;
          }
          break;

        case ACTION_DOWN:
          if ( droidtype > 0 ) {
            MoveMenuPositionSound();
            droidtype --;
            need_update = TRUE;
          }
          break;

        case ACTION_RIGHT:
          if ( page < 2 ) {
            MoveMenuPositionSound();
            page ++;
            need_update = TRUE;
          }
          break;

        case ACTION_LEFT:
          if ( page > 0 ) {
            MoveMenuPositionSound();
            page --;
            need_update = TRUE;
          }
        default:
          break;
        } // switch(action)

      SDL_Delay(1); // don't hog CPU
    } /* while !finished */

  return;
}; // void GreatDroidShow( void )

/*------------------------------------------------------------
 * display infopage page of droidtype
 *
 * if flags == UPDATE_ONLY : don't blit a new background&banner,
 *                           only  update the text-regions
 *
 *  does update the screen: all if flags=0, text-rect if flags=UPDATE_ONLY
 *
 *------------------------------------------------------------*/
void
show_droid_info (int droidtype, int page, int flags)
{
  char InfoText[1000];
  char DroidName[80];
  bool show_arrows = FALSE;
  int lineskip, lastline;

  SDL_SetClipRect ( ne_screen , NULL );
  SetCurrentFont( Para_BFont );

  lineskip = FontHeight (GetCurrentFont()) * TEXT_STRETCH;
  lastline = Cons_Header_Rect.y + Cons_Header_Rect.h;
  Set_Rect(up_rect,   Cons_Header_Rect.x, lastline - 1.0*lineskip, 25, 13);
  Set_Rect(down_rect, Cons_Header_Rect.x, lastline - 0.5*lineskip, 25, 13);
  Set_Rect(left_rect, Cons_Header_Rect.x + Cons_Header_Rect.w - 1.5*lineskip, lastline - 0.9*lineskip,13,25);
  Set_Rect(right_rect,Cons_Header_Rect.x + Cons_Header_Rect.w - 1.0*lineskip, lastline - 0.9*lineskip,13,25);

  //  Fill_Rect (Cons_Header_Rect, Black);  // for debugging menu-rects...

  sprintf (DroidName, "  Unit type %s - %s", Druidmap[droidtype].druidname,
	   Classname[Druidmap[droidtype].class]);

  switch (page)
    {
    case -3: // Title screen: intro unit
      sprintf (InfoText, "This is the unit that you currently control. Prepare to board Robo-frighter \
Paradroid to eliminate all rogue robots.");
      break;
    case -2: // Takeover: unit that you wish to control
      sprintf (InfoText, "This is the unit that you wish to control.\n\n Prepare to takeover.");
      break;
    case -1: // Takeover: unit that you control
      sprintf (InfoText, "This is the unit that you currently control.");
      break;
    case 0:
      show_arrows = TRUE;
      sprintf (InfoText, "\
Entry : %02d\n\
Class : %s\n\
Height : %5.2f m\n\
Weight: %d kg\n\
Drive : %s \n\
Brain : %s",   droidtype+1, Classes[Druidmap[droidtype].class],
	       Druidmap[droidtype].height, Druidmap[droidtype].weight,
	       Drivenames [ Druidmap[ droidtype].drive],
	       Brainnames[ Druidmap[droidtype].brain ]);
      break;
    case 1:
      show_arrows = TRUE;
      sprintf( InfoText , "\
Armament : %s\n\
Sensors  1: %s\n\
                    2: %s\n\
                    3: %s",
	       Weaponnames [ Druidmap[droidtype].gun],
	       Sensornames[ Druidmap[droidtype].sensor1 ],
	       Sensornames[ Druidmap[droidtype].sensor2 ],
	       Sensornames[ Druidmap[droidtype].sensor3 ]);
      break;
    case 2:
      show_arrows = TRUE;
      sprintf (InfoText, "Notes: %s", Druidmap[droidtype].notes);
      break;
    default:
      sprintf (InfoText, "ERROR: Page not implemented!! \nPlease report bug!");
      break;
    } /* switch (page) */



  // if UPDATE_ONLY then the background has not been cleared, so we have do it
  // it for each menu-rect:
  if (flags & UPDATE_ONLY)
    {
      SDL_SetClipRect (ne_screen, &Cons_Text_Rect);
      SDL_BlitSurface (console_bg_pic2, NULL, ne_screen, NULL);
      SDL_SetClipRect (ne_screen, &Cons_Header_Rect);
      SDL_BlitSurface (console_bg_pic2, NULL, ne_screen, NULL);
      SDL_SetClipRect (ne_screen, NULL);
    }
  else // otherwise we just redraw the whole screen
    {
      SDL_BlitSurface (console_bg_pic2, NULL, ne_screen, NULL);
      DisplayBanner (NULL, NULL,  BANNER_NO_SDL_UPDATE | BANNER_FORCE_UPDATE );
    }

  DisplayText (InfoText, Cons_Text_Rect.x, Cons_Text_Rect.y, &Cons_Text_Rect);

  DisplayText (DroidName, Cons_Header_Rect.x + lineskip , lastline - 0.9*lineskip, NULL);

  if (show_arrows)
    {
      if (Me.type >  droidtype)
	SDL_BlitSurface ( arrow_up, NULL, ne_screen, &up_rect);

      if (droidtype > 0)
	SDL_BlitSurface ( arrow_down, NULL, ne_screen, &down_rect);

      if (page > 0)
	SDL_BlitSurface ( arrow_left, NULL, ne_screen, &left_rect);

      if (page < 2)
	SDL_BlitSurface ( arrow_right, NULL, ne_screen, &right_rect);
    }

  if (flags & UPDATE_ONLY)
    {
      SDL_UpdateRects (ne_screen, 1, &Cons_Header_Rect);
      SDL_UpdateRects (ne_screen, 1, &Cons_Text_Rect);
    }
  else
    SDL_Flip (ne_screen);

  return;

} /* show_droid_info */


//----------------------------------------------------------------------
// show a an animated droid-pic: automatically counts frames and frametimes
// stored internally, so you just have to keep calling this function to get
// an animation. The target-rect dst is only updated when a new frame is set
// if flags & RESET: to restart a fresh animation at frame 0
// if flags & UPDATE: force a blit of droid-pic
//
// cycle_time is the time in seconds for a full animation-cycle,
// if cycle_time == 0 : display static pic, using only first frame
//
//----------------------------------------------------------------------
void
show_droid_portrait (SDL_Rect dst, int droid_type, float cycle_time, int flags)
{
  static SDL_Surface *background = NULL;
  static SDL_Surface *droid_pics = NULL;
  static int frame_num = 0;
  static int last_droid_type = -1;
  static Uint32 last_frame_time = 0;
  static SDL_Rect src_rect;
  SDL_Surface *tmp;
  Uint32 frame_duration;
  bool need_new_frame = FALSE;
  int num_frames;

  SDL_SetClipRect (ne_screen, &dst);

  if (!background) // first call
    {
      tmp = SDL_CreateRGBSurface (0, dst.w, dst.h, vid_bpp, 0, 0, 0, 0);
      background = SDL_DisplayFormat (tmp);
      SDL_FreeSurface (tmp);
      SDL_BlitSurface (ne_screen, &dst, background, NULL);
      Copy_Rect (Portrait_Rect, src_rect);
    }

  if (flags & RESET)
    {
      SDL_BlitSurface (ne_screen, &dst, background, NULL);
      frame_num  = 0;
      last_frame_time = SDL_GetTicks ();
    }

  if ( (droid_type != last_droid_type) || (droid_pics == NULL))
    { // we need to unpack the droid-pics into our local storage
      if (droid_pics) SDL_FreeSurface (droid_pics);
      droid_pics = NULL;
      tmp = IMG_Load_RW (packed_portraits[droid_type], 0);
      // important: return seek-position to beginning of RWops for next operation to succeed!
      SDL_RWseek (packed_portraits[droid_type], 0, SEEK_SET);
      if (!tmp)
	{
	  DebugPrintf (0, "ERROR: failed to unpack droid-portraits of droid-type %d\n", droid_type);
	  return; // ok, so no pic but we continue ;)
	}
      // now see if its a jpg, then we add some transparency by color-keying:
      if (IMG_isJPG(packed_portraits[droid_type]))
	{
	  droid_pics = SDL_DisplayFormat (tmp);
	} // else assume it's png ;)
      else
	{
	  droid_pics = SDL_DisplayFormatAlpha (tmp);
	}
      SDL_FreeSurface (tmp);
      SDL_RWseek (packed_portraits[droid_type], 0, SEEK_SET);


      // do we have to scale the droid pics
      if (GameConfig.scale != 1.0)
	ScalePic (&droid_pics, GameConfig.scale);

      last_droid_type = droid_type;
    }

  num_frames = droid_pics->w / Portrait_Rect.w;

  // sanity check
  if ( num_frames == 0)
    {
      DebugPrintf (0, "WARNING: Only one frame found. Width droid-pics=%d, Frame-width=%d\n",
		   droid_pics->w, Portrait_Rect.w);
      num_frames = 1;       // continue and hope for the best
    }

  frame_duration = SDL_GetTicks() - last_frame_time;

  if (cycle_time && (frame_duration >  1000.0*cycle_time/num_frames) )
    {
      need_new_frame = TRUE;
      frame_num ++;
    }

  if (frame_num >= num_frames)
    frame_num = 0;

  if ( (flags & (RESET|UPDATE)) || need_new_frame)
    {
      src_rect.x = frame_num*src_rect.w;

      SDL_BlitSurface (background, NULL, ne_screen, &dst);
      SDL_BlitSurface (droid_pics, &src_rect, ne_screen, &dst);

      SDL_UpdateRects (ne_screen, 1, &dst);

      last_frame_time = SDL_GetTicks();
    }

  SDL_SetClipRect (ne_screen, NULL);

  return;

} // show_droid_portrait

// ----------------------------------------------------------------------
// do all alert-related agitations: alert-sirens and alert-lights
// ----------------------------------------------------------------------
#define SIREN_WAIT 2.5
#define BLINK_WAIT 0.2
void
AlertLevelWarning (void)
{
  static Uint32 last_siren = 0;
  //  static Uint32 last_blink = 0;
  int i, posx, posy;
  int cur_alert = 0;


  switch (AlertLevel)
    {
    case AL_GREEN:
      break;
    case AL_YELLOW:
    case AL_AMBER:
    case AL_RED:
      if (SDL_GetTicks() - last_siren > SIREN_WAIT * 1000.0 / AlertLevel)  // higher alert-> faster sirens!
	{
	  Play_Sound (ALERT_SOUND);
	  last_siren = SDL_GetTicks ();
	}
      break;
    default:
      DebugPrintf (0, "WARNING: illegal AlertLevel = %d > %d.. something's gone wrong!!\n",
		   AlertLevel, AL_RED);
      break;
    }

  // so much to the sirens, now make sure the alert-tiles are updated correctly:
  posx = CurLevel->alerts[0].x;
  posy = CurLevel->alerts[0].y;
  if (posx == -1) return;  // no alerts here...


  cur_alert = ALERT_GREEN + AlertLevel;

  // check if alert-tiles are up-to-date
  if (GetMapBrick(CurLevel, posx, posy) == cur_alert)
    return; // ok

  for (i=0; i< MAX_ALERTS_ON_LEVEL; i++)
    {
      posx = CurLevel->alerts[i].x;
      posy = CurLevel->alerts[i].y;
      if ( posx == -1) 	break;

      CurLevel->map[posy][posx] = cur_alert;
    }

  return;
}

#undef _ship_c
