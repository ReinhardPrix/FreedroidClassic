/*----------------------------------------------------------------------
 *
 * Desc: all menu functions and their subfunctions for Freedroid
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
#define _menu_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"

void Single_Player_Menu (void);
void Multi_Player_Menu (void);
void Credits_Menu (void);
void Options_Menu (void);
void Show_Mission_Instructions_Menu (void);
void Show_Waypoints(void);
void Level_Editor(void);

#define FIRST_MENU_ITEM_POS_X (2*Block_Width)
#define FIRST_MENU_ITEM_POS_Y (USERFENSTERPOSY + FontHeight(Menu_BFont))
#define OPTIONS_MENU_ITEM_POS_X (UserCenter_x - 120)
#define FIRST_MIS_SELECT_ITEM_POS_X (0.0*Block_Width)
#define FIRST_MIS_SELECT_ITEM_POS_Y (USERFENSTERPOSY + FontHeight(Menu_BFont))

EXTERN int MyCursorX;
EXTERN int MyCursorY;

SDL_Surface *Menu_Background = NULL;
int fheight;  // font height of Menu-font

/*@Function============================================================
@Desc: This function prepares the screen for the big Escape menu and 
       its submenus.  This means usual content of the screen, i.e. the 
       combat screen and top status bar, is "faded out", the rest of 
       the screen is cleared.  This function resolves some redundance 
       that occured since there are so many submenus needing this.

@Ret: none
* $Function----------------------------------------------------------*/
void 
InitiateMenu( void )
{
  //--------------------
  // Here comes the standard initializer for all the menus and submenus
  // of the big escape menu.  This prepares the screen, so that we can
  // write on it further down.
  //
  Activate_Conservative_Frame_Computation();

  SDL_SetClipRect( ne_screen, NULL );
  Me.status=MENU;
  ClearGraphMem();
  DisplayBanner (NULL, NULL,  BANNER_NO_SDL_UPDATE | BANNER_FORCE_UPDATE );
  Assemble_Combat_Picture ( 0 );
  SDL_SetClipRect( ne_screen, NULL );
  MakeGridOnScreen( NULL );
  
  if (Menu_Background) SDL_FreeSurface (Menu_Background);
  Menu_Background = SDL_DisplayFormat (ne_screen);  // keep a global copy of background 
 
  ResetMouseWheel ();
  
  SDL_ShowCursor (SDL_DISABLE);  // deactivate mouse-cursor in menus
  SetCurrentFont ( Menu_BFont );
  fheight = FontHeight (GetCurrentFont()) + 2;

  return;

} // void InitiateMenu(void)

/*@Function============================================================
@Desc: This function provides a convenient cheat menu, so that any 
       tester does not have to play all through the game again and again
       to see if a bug in a certain position has been removed or not.

@Ret:  none
* $Function----------------------------------------------------------*/
extern int CurrentlyCPressed; 	/* the key that brought as in here */
				/* we need to make sure it is set as released */
				/* before we leave ...*/
void
Cheatmenu (void)
{
  char *input;		/* string input from user */
  int Weiter;
  int LNum, X, Y, num;
  int i, l;
  int x0, y0, line;
  Waypoint WpList;      /* pointer on current waypoint-list  */
  BFont_Info *font;

  // Prevent distortion of framerate by the delay coming from 
  // the time spend in the menu.
  Activate_Conservative_Frame_Computation();

  font =  FPS_Display_BFont;


  SetCurrentFont (font);  /* not the ideal one, but there's currently */
				/* no other it seems.. */
  x0 = 50;
  y0 = 20;
  line = 0;

  Weiter = FALSE;
  while (!Weiter)
    {
      ClearGraphMem ();
      printf_SDL (ne_screen, x0, y0, "Current position: Level=%d, X=%d, Y=%d\n",
		   CurLevel->levelnum, (int)Me.pos.x, (int)Me.pos.y);
      printf_SDL (ne_screen, -1, -1, " a. Armageddon (alle Robots sprengen)\n");
      printf_SDL (ne_screen, -1, -1, " l. robot list of current level\n");
      printf_SDL (ne_screen, -1, -1, " g. complete robot list\n");
      printf_SDL (ne_screen, -1, -1, " d. destroy robots on current level\n");
      printf_SDL (ne_screen, -1, -1, " t. Teleportation\n");
      printf_SDL (ne_screen, -1, -1, " r. change to new robot type\n");
      printf_SDL (ne_screen, -1, -1, " i. Invinciblemode: %s",
		  InvincibleMode ? "ON\n" : "OFF\n");
      printf_SDL (ne_screen, -1, -1, " e. set energy\n");
      printf_SDL (ne_screen, -1, -1, " h. Hide invisible map parts: %s",
		  HideInvisibleMap ? "ON\n" : "OFF\n" );
      printf_SDL (ne_screen, -1, -1, " n. No hidden droids: %s",
		  show_all_droids ? "ON\n" : "OFF\n" );
      printf_SDL (ne_screen, -1, -1, " m. Map of Deck xy\n");
      printf_SDL (ne_screen, -1, -1, " s. Sound: %s",
		  sound_on ? "ON\n" : "OFF\n");
      printf_SDL (ne_screen, -1, -1, " x. Fullscreen : %s",
		  fullscreen_on ? "ON\n" : "OFF\n");
      printf_SDL (ne_screen, -1, -1, " w. Print current waypoints\n");
      printf_SDL (ne_screen, -1, -1, " z. change Zoom factor\n");
      printf_SDL (ne_screen, -1, -1, " f. Freeze on this positon: %s",
		  stop_influencer ? "ON\n" : "OFF\n");
      printf_SDL (ne_screen, -1, -1, " q. RESUME game\n");

      switch (getchar_raw ())
	{
	case 'f':
	  stop_influencer = !stop_influencer;
	  break;

	case 'z':
	  ClearGraphMem();
	  printf_SDL (ne_screen, x0, y0, "Current Zoom factor: %f\n",
		      CurrentCombatScaleFactor); 
	  printf_SDL (ne_screen, -1, -1, "New zoom factor: ");
	  input = GetString (40, 2);
	  sscanf (input, "%f", &CurrentCombatScaleFactor);
	  free (input);
	  SetCombatScaleTo (CurrentCombatScaleFactor);
	  break;

	case 'a': /* armageddon */
	  Weiter = 1;
	  Armageddon ();
	  break;

	case 'l': /* robot list of this deck */
	  l = 0; /* line counter for enemy output */
	  for (i = 0; i < NumEnemys; i++)
	    {
	      if (AllEnemys[i].levelnum == CurLevel->levelnum) 
		{
		  if (l && !(l%20)) 
		    {
		      printf_SDL (ne_screen, -1, -1, " --- MORE --- \n");
		      if( getchar_raw () == 'q')
			break;
		    }
		  if (!(l % 20) )  
		    {
		      ClearGraphMem ();
		      printf_SDL (ne_screen, x0, y0,
				   " NR.   ID  X    Y   ENERGY   speedX\n");
		      printf_SDL (ne_screen, -1, -1,
				  "---------------------------------------------\n");
		    }
		  
		  l ++;
		  printf_SDL (ne_screen, -1, -1,
			      "%d.   %s   %d   %d   %d    %g.\n", i,
			       Druidmap[AllEnemys[i].type].druidname,
			       (int)AllEnemys[i].pos.x,
			       (int)AllEnemys[i].pos.y,
			       (int)AllEnemys[i].energy,
			       AllEnemys[i].speed.x);
		} /* if (enemy on current level)  */
	    } /* for (i<NumEnemys) */

	  printf_SDL (ne_screen, -1, -1," --- END --- \n");
	  getchar_raw ();
	  break;

	case 'g': /* complete robot list of this ship */
	  for (i = 0; i < MAX_ENEMYS_ON_SHIP ; i++)
	    {
	      if ( AllEnemys[i].type == (-1) ) continue;

	      if (i && !(i%13)) 
		{
		  printf_SDL (ne_screen, -1, -1, " --- MORE --- ('q' to quit)\n");
		  if (getchar_raw () == 'q')
		    break;
		}
	      if ( !(i % 13) )
		{
		  ClearGraphMem ();
		  printf_SDL (ne_screen, x0, y0, "Nr.  Lev. ID  Energy  Speed.x\n");
		  printf_SDL (ne_screen, -1, -1, "------------------------------\n");
		}
	      
	      printf_SDL (ne_screen, -1, -1, "%d  %d  %s  %d  %g\n",
			  i, AllEnemys[i].levelnum,
			  Druidmap[AllEnemys[i].type].druidname,
			  (int)AllEnemys[i].energy,
			  AllEnemys[i].speed.x);
	    } /* for (i<NumEnemys) */

	  printf_SDL (ne_screen, -1, -1, " --- END ---\n");
	  getchar_raw ();
	  break;


	case 'd': /* destroy all robots on this level, haha */
	  for (i = 0; i < NumEnemys; i++)
	    {
	      if (AllEnemys[i].levelnum == CurLevel->levelnum)
		AllEnemys[i].energy = -100;
	    }
	  printf_SDL (ne_screen, -1, -1, "All robots on this deck killed!\n");
	  getchar_raw ();
	  break;


	case 't': /* Teleportation */
	  ClearGraphMem ();
	  printf_SDL (ne_screen, x0, y0, "Enter Level, X, Y: ");
	  input = GetString (40, 2);
	  sscanf (input, "%d, %d, %d\n", &LNum, &X, &Y);
	  free (input);
	  Teleport (LNum, X, Y);
	  break;

	case 'r': /* change to new robot type */
	  ClearGraphMem ();
	  printf_SDL (ne_screen, x0, y0, "Type number of new robot: ");
	  input = GetString (40, 2);
	  for (i = 0; i < Number_Of_Droid_Types ; i++)
	    if (!strcmp (Druidmap[i].druidname, input))
	      break;

	  if ( i == Number_Of_Droid_Types )
	    {
	      printf_SDL (ne_screen, x0, y0+20,
			  "Unrecognized robot-type: %s", input);
	      getchar_raw ();
	      ClearGraphMem();
	    }
	  else
	    {
	      Me.type = i;
	      Me.energy = Druidmap[Me.type].maxenergy;
	      Me.health = Me.energy;
	      printf_SDL (ne_screen, x0, y0+20, "You are now a %s. Have fun!\n", input);
	      getchar_raw ();
	    }
	  free (input);
	  break;

	case 'i': /* togge Invincible mode */
	  InvincibleMode = !InvincibleMode;
	  break;

	case 'e': /* complete heal */
	  ClearGraphMem();
	  printf_SDL (ne_screen, x0, y0, "Current energy: %f\n", Me.energy);
	  printf_SDL (ne_screen, -1, -1, "Enter your new energy: ");
	  input = GetString (40, 2);
	  sscanf (input, "%d", &num);
	  free (input);
	  Me.energy = (double) num;
	  if (Me.energy > Me.health) Me.health = Me.energy;
	  break;

	case 'h': /* toggle hide invisible map */
	  HideInvisibleMap = !HideInvisibleMap;
	  break;

	case 'n': /* toggle display of all droids */
	  show_all_droids = !show_all_droids;
	  break;

	case 's': /* toggle sound on/off */
	  sound_on = !sound_on;
	  break;

	case 'm': /* Show deck map in Concept view */
	  printf_SDL (ne_screen, -1, -1, "\nLevelnum: ");
	  input = GetString (40, 2);
	  sscanf (input, "%d", &LNum);
	  free (input);
	  ShowDeckMap (curShip.AllLevels[LNum]);
	  getchar_raw ();
	  break;

	case 'x': /* toggle fullscreen - mode */
	  fullscreen_on = !fullscreen_on;
	  break;
	  
	case 'w':  /* print waypoint info of current level */
	  WpList = CurLevel->AllWaypoints;
	  for (i=0; i<MAXWAYPOINTS && WpList[i].x; i++)
	    {
	      if (i && !(i%20))
		{
		  printf_SDL (ne_screen, -1, -1, " ---- MORE -----\n");
		  if (getchar_raw () == 'q')
		    break;
		}
	      if ( !(i%20) )
		{
		  ClearGraphMem ();
		  printf_SDL (ne_screen, x0, y0, "Nr.   X   Y      C1  C2  C3  C4\n");
		  printf_SDL (ne_screen, -1, -1, "------------------------------------\n");
		}
	      printf_SDL (ne_screen, -1, -1, "%2d   %2d  %2d      %2d  %2d  %2d  %2d\n",
			  i, WpList[i].x, WpList[i].y,
			  WpList[i].connections[0],
			  WpList[i].connections[1],
			  WpList[i].connections[2],
			  WpList[i].connections[3]);

	    } /* for (all waypoints) */
	  printf_SDL (ne_screen, -1, -1, " --- END ---\n");
	  getchar_raw ();
	  break;

	case ' ':
	case 'q':
	  Weiter = 1;
	  break;
	} /* switch (getchar_raw()) */
    } /* while (!Weiter) */

  InitBars = TRUE;

  ClearGraphMem ();
  SDL_Flip (ne_screen);

  keyboard_update (); /* treat all pending keyboard events */
  /* 
   * when changing windows etc, sometimes a key-release event gets 
   * lost, so we have to make sure that CPressed is no longer set
   * or we stay here for ever...
   */
  CurrentlyCPressed = FALSE;

  return;
} /* Cheatmenu() */


/*@Function============================================================
@Desc: This function provides a the big escape menu from where you can
       get into different submenus.

@Ret:  none
* $Function----------------------------------------------------------*/
void
EscapeMenu (void)
{
  enum
    { 
      NEW_GAME=1, 
      FULL_WINDOW,
      SET_THEME,
      OPTIONS,
      LEVEL_EDITOR,
      HIGHSCORES,
      CREDITS,
      QUIT
    };
  
  bool finished = FALSE;
  bool key_pressed = FALSE;
  int MenuPosition=1;
  char theme_string[40];
  char window_string[40];
  int i;

  InitiateMenu();
  
  while ( EscapePressed() );

  while (!finished)
    {
      key_pressed = FALSE;

      SDL_BlitSurface (Menu_Background, NULL, ne_screen, NULL);
    
      PutInfluence (FIRST_MENU_ITEM_POS_X, FIRST_MENU_ITEM_POS_Y + (MenuPosition-1.5)*fheight);

      strcpy (theme_string, "Tileset: ");
      if (strstr (GameConfig.Theme_SubPath, "classic"))
	strcat (theme_string, "Classic");
      else if (strstr (GameConfig.Theme_SubPath, "lanzz"))
	strcat (theme_string, "Lanzz");
      else
	strcat (theme_string, "unknown");

      strcpy (window_string, "Combat window: ");
      if (GameConfig.FullUserRect) strcat (window_string, "Full");
      else strcat (window_string, "Classic");

      PutString (ne_screen, OPTIONS_MENU_ITEM_POS_X, FIRST_MENU_ITEM_POS_Y, "New Game");
      PutString (ne_screen, OPTIONS_MENU_ITEM_POS_X, FIRST_MENU_ITEM_POS_Y+1*fheight, window_string);
      PutString (ne_screen, OPTIONS_MENU_ITEM_POS_X, FIRST_MENU_ITEM_POS_Y+2*fheight, theme_string);
      PutString (ne_screen, OPTIONS_MENU_ITEM_POS_X,FIRST_MENU_ITEM_POS_Y+3*fheight,"Further Options");
      PutString (ne_screen, OPTIONS_MENU_ITEM_POS_X, FIRST_MENU_ITEM_POS_Y +4*fheight, "Level Editor");
      PutString (ne_screen, OPTIONS_MENU_ITEM_POS_X, FIRST_MENU_ITEM_POS_Y +5*fheight, "Highscores");
      PutString (ne_screen, OPTIONS_MENU_ITEM_POS_X, FIRST_MENU_ITEM_POS_Y +6*fheight, "Credits");
      PutString (ne_screen, OPTIONS_MENU_ITEM_POS_X, FIRST_MENU_ITEM_POS_Y +7*fheight, "Quit Game");

      SDL_Flip( ne_screen );

      while (!key_pressed)
	{
	  if ( EscapePressed() )
	    {
	      while ( EscapePressed() );
	      finished = TRUE;
	      key_pressed = TRUE;
	    }
	  if (SpacePressed() ) 
	    {
	      key_pressed = TRUE;
	      MenuItemSelectedSound();
	      while (SpacePressed()) ;

	      switch (MenuPosition) 
		{
		case NEW_GAME:
		  // just let influ die, that's enough to start a new game... ;)
		  Me.energy = -1;
		  finished = TRUE;
		  break;
		case FULL_WINDOW:
		  GameConfig.FullUserRect = ! GameConfig.FullUserRect;
		  if (GameConfig.FullUserRect)
		    Copy_Rect (Full_User_Rect, User_Rect);
		  else
		    Copy_Rect (Classic_User_Rect, User_Rect);

		  InitiateMenu ();
		  break;

		case SET_THEME:
		  if ( !strcmp (GameConfig.Theme_SubPath , "classic_theme/" ) )
		    sprintf (GameConfig.Theme_SubPath, "lanzz_theme/");
		  else
		    sprintf (GameConfig.Theme_SubPath, "classic_theme/");

		  ReInitPictures();
		  
		  //--------------------
		  // Now we have loaded a new theme with new images!!  It might however be the
		  // case, that also the number of phases per bullet, which is specific to each
		  // theme, has been changed!!! THIS MUST NOT BE IGNORED, OR WE'LL SEGFAULT!!!!
		  // Because the old number of phases is still attached to living bullets, it
		  // might try to blit a new (higher) number of phases although there are only
		  // less Surfaces generated for the bullet in the old theme.  The solution seems
		  // to be simply to request new graphics to be attached to each bullet, which
		  // should be simply setting a flag for each of the bullets:
		  for ( i = 0 ; i < MAXBULLETS ; i++ )
		    AllBullets[i].Surfaces_were_generated = FALSE ;
		  InitiateMenu();
		  break;

		case OPTIONS:
		  Options_Menu();
		  break;

		case LEVEL_EDITOR:
		  Level_Editor();
		  break;

		case HIGHSCORES:
		  ShowHighscores();
		  break;

		case CREDITS:
		  Credits_Menu();
		  break;
		case QUIT:
		  Terminate(0);
		  break;
		default: 
		  break;
		} // switch 
	    } // if SpacePressed()
	  if (UpPressed() || WheelUpPressed() ) 
	    {
	      while (UpPressed());
	      key_pressed = TRUE;
	      if (MenuPosition > 1) MenuPosition--;
	      else MenuPosition = QUIT;
	      MoveMenuPositionSound();
	    }
	  if (DownPressed() || WheelDownPressed() ) 
	    {
	      while (DownPressed());
	      key_pressed = TRUE;
	      if ( MenuPosition < QUIT ) MenuPosition++;
	      else MenuPosition = 1;
	      MoveMenuPositionSound();
	    }
	} // while !key_pressed

    } // while !finished

  ClearGraphMem();
  // Since we've faded out the whole scren, it can't hurt
  // to have the top status bar redrawn...
  BannerIsDestroyed=TRUE;
  Me.status=MOBILE;

  SDL_ShowCursor (SDL_ENABLE);  // reactivate mouse-cursor for game

  return;

} // EscapeMenu

/*@Function============================================================
@Desc: This function provides a the options menu.  This menu is a 
       submenu of the big EscapeMenu.  Here you can change sound vol.,
       gamma correction, fullscreen mode, display of FPS and such
       things.

@Ret:  none
* $Function----------------------------------------------------------*/
void
GraphicsSound_Options_Menu (void)
{
  int MenuPosition=1;
  bool finished = FALSE;
  bool key_pressed = FALSE;

enum
  { SET_BG_MUSIC_VOLUME=1, 
    SET_SOUND_FX_VOLUME, 
    SET_GAMMA_CORRECTION, 
    SET_FULLSCREEN_FLAG, 
    BACK };

  while ( EscapePressed() );

  while (!finished)
    {
      key_pressed = FALSE;
      SDL_BlitSurface (Menu_Background, NULL, ne_screen, NULL);

      PutInfluence (FIRST_MENU_ITEM_POS_X, FIRST_MENU_ITEM_POS_Y+ (MenuPosition-1.5)*fheight);

      PrintString (ne_screen, OPTIONS_MENU_ITEM_POS_X, FIRST_MENU_ITEM_POS_Y+0*fheight, 
		   "Background Music: %1.2f" , GameConfig.Current_BG_Music_Volume );
      PrintString (ne_screen, OPTIONS_MENU_ITEM_POS_X, FIRST_MENU_ITEM_POS_Y+1*fheight,  
		   "Sound Effects: %1.2f", GameConfig.Current_Sound_FX_Volume );
      PrintString (ne_screen, OPTIONS_MENU_ITEM_POS_X, FIRST_MENU_ITEM_POS_Y+2*fheight,  
		   "Gamma: %1.2f", GameConfig.Current_Gamma_Correction );
      PrintString (ne_screen, OPTIONS_MENU_ITEM_POS_X, FIRST_MENU_ITEM_POS_Y+3*fheight, 
		   "Fullscreen Mode: %s", fullscreen_on ? "ON" : "OFF");
      PrintString (ne_screen, OPTIONS_MENU_ITEM_POS_X, FIRST_MENU_ITEM_POS_Y+4*fheight, "Back");
      SDL_Flip( ne_screen );

      while (!key_pressed)
	{
	  if ( EscapePressed() )
	    {
	      while ( EscapePressed() );
	      finished = TRUE;
	      key_pressed = TRUE;
	    }
	  
	  // Some menu options can be controlled by pressing right or left
	  // These options are gamma corrections, sound volume and the like
	  // Therefore left and right key must be resprected.  This is done here:
	  if (RightPressed() || LeftPressed() ) 
	    {
	      key_pressed = TRUE;
	      if (MenuPosition == SET_BG_MUSIC_VOLUME ) 
		{
		  if (RightPressed()) 
		    {
		      while (RightPressed());
		      if ( GameConfig.Current_BG_Music_Volume < 1 ) 
			GameConfig.Current_BG_Music_Volume += 0.05;
		      Set_BG_Music_Volume( GameConfig.Current_BG_Music_Volume );
		    }
		  if (LeftPressed()) 
		    {
		      while (LeftPressed());
		      if ( GameConfig.Current_BG_Music_Volume > 0 ) 
			GameConfig.Current_BG_Music_Volume -= 0.05;
		      Set_BG_Music_Volume( GameConfig.Current_BG_Music_Volume );
		    }
		}
	      if (MenuPosition == SET_SOUND_FX_VOLUME ) 
		{
		  if (RightPressed()) 
		    {
		      while (RightPressed());
		      if ( GameConfig.Current_Sound_FX_Volume < 1 ) 
			GameConfig.Current_Sound_FX_Volume += 0.05;
		      Set_Sound_FX_Volume( GameConfig.Current_Sound_FX_Volume );
		    }
		  if (LeftPressed()) 
		    {
		      while (LeftPressed());
		      if ( GameConfig.Current_Sound_FX_Volume > 0 ) 
			GameConfig.Current_Sound_FX_Volume -= 0.05;
		      Set_Sound_FX_Volume( GameConfig.Current_Sound_FX_Volume );
		    }
		}
	      if (MenuPosition == SET_GAMMA_CORRECTION ) 
		{
		  if (RightPressed()) 
		    {
		      while (RightPressed());
		      GameConfig.Current_Gamma_Correction+=0.05;
		      SDL_SetGamma( GameConfig.Current_Gamma_Correction , 
				    GameConfig.Current_Gamma_Correction , 
				    GameConfig.Current_Gamma_Correction );
		    }
		  if (LeftPressed()) 
		    {
		      while (LeftPressed());
		      GameConfig.Current_Gamma_Correction-=0.05;
		      SDL_SetGamma( GameConfig.Current_Gamma_Correction , 
				    GameConfig.Current_Gamma_Correction , 
				    GameConfig.Current_Gamma_Correction );
		    }
		}
	    }

	  if (SpacePressed() ) 
	    {
	      MenuItemSelectedSound();
	      while (SpacePressed());
	      key_pressed = TRUE;
	      switch (MenuPosition) 
		{
		case SET_FULLSCREEN_FLAG:
		  SDL_WM_ToggleFullScreen (ne_screen);
		  fullscreen_on = !fullscreen_on;
		  break;

		case BACK:
		  while (EnterPressed() || SpacePressed() );
		  finished=TRUE;
		  break;
		default: 
		  break;
		} 
	    } // if SpacePressed()

	  if (UpPressed() || WheelUpPressed ()) 
	    {
	      while (UpPressed());
	      key_pressed = TRUE;
	      if ( MenuPosition > 1 ) MenuPosition--;
	      else MenuPosition = BACK;
	      MoveMenuPositionSound();
	    }
	  if (DownPressed() || WheelDownPressed()) 
	    {
	      while (DownPressed());
	      key_pressed = TRUE;
	      if ( MenuPosition < BACK ) MenuPosition++;
	      else MenuPosition = 1;
	      MoveMenuPositionSound();
	    }
	} // while !key_pressed

    } // while !finished

  return;

}; // GraphicsSound_Options_Menu

/*@Function============================================================
@Desc: This function provides a the options menu.  This menu is a 
       submenu of the big EscapeMenu.  Here you can change sound vol.,
       gamma correction, fullscreen mode, display of FPS and such
       things.

@Ret:  none
* $Function----------------------------------------------------------*/
void
On_Screen_Display_Options_Menu (void)
{
  int MenuPosition=1;
  bool finished = FALSE;
  bool key_pressed = FALSE;

enum
  { SHOW_POSITION=1, 
    SHOW_FRAMERATE, 
    SHOW_ENERGY,
    BACK };

  while ( EscapePressed() );

  while (!finished)
    {
      key_pressed = FALSE;
      SDL_BlitSurface (Menu_Background, NULL, ne_screen, NULL);
      
      PutInfluence (FIRST_MENU_ITEM_POS_X, FIRST_MENU_ITEM_POS_Y + (MenuPosition-1.5)*fheight);

      PrintString (ne_screen, OPTIONS_MENU_ITEM_POS_X, FIRST_MENU_ITEM_POS_Y+0*fheight, 
		   "Show Position: %s", GameConfig.Draw_Position ? "ON" : "OFF");
      PrintString (ne_screen, OPTIONS_MENU_ITEM_POS_X, FIRST_MENU_ITEM_POS_Y+1*fheight,
		   "Show Framerate: %s", GameConfig.Draw_Framerate? "ON" : "OFF");
      PrintString (ne_screen, OPTIONS_MENU_ITEM_POS_X, FIRST_MENU_ITEM_POS_Y+2*fheight,
		   "Show Energy: %s", GameConfig.Draw_Energy? "ON" : "OFF");
      PrintString (ne_screen, OPTIONS_MENU_ITEM_POS_X, FIRST_MENU_ITEM_POS_Y+3*fheight, "Back");

      SDL_Flip( ne_screen );

      while (!key_pressed)
	{
	  if ( EscapePressed() )
	    {
	      while ( EscapePressed() );
	      finished = TRUE;
	      key_pressed = TRUE;
	    }

	  if (SpacePressed() ) 
	    {
	      MenuItemSelectedSound();
	      while (SpacePressed());
	      key_pressed = TRUE;
	      switch (MenuPosition) 
		{
		case SHOW_POSITION:
		  GameConfig.Draw_Position=!GameConfig.Draw_Position;
		  InitiateMenu();
		  break;
		case SHOW_FRAMERATE:
		  GameConfig.Draw_Framerate=!GameConfig.Draw_Framerate;
		  InitiateMenu();
		  break;
		case SHOW_ENERGY:
		  GameConfig.Draw_Energy=!GameConfig.Draw_Energy;
		  InitiateMenu();
		  break;
		case BACK:
		  finished = TRUE;
		  break;
		default: 
		  break;
		} 
	    } // if SpacePressed()

	  if (UpPressed() || WheelUpPressed ()) 
	    {
	      if ( MenuPosition > 1 ) MenuPosition--;
	      else MenuPosition = BACK;
	      MoveMenuPositionSound();
	      while (UpPressed());
	      key_pressed = TRUE;
	    }
	  if (DownPressed() || WheelDownPressed ()) 
	    {
	      if ( MenuPosition < BACK ) MenuPosition++;
	      else MenuPosition = 1;
	      MoveMenuPositionSound();
	      while (DownPressed());
	      key_pressed = TRUE;
	    }
	} // while !key_pressed

    } // while !finished

  return;

}; // On_Screen_Display_Options_Menu

/*@Function============================================================
@Desc: This function provides a the options menu.  This menu is a 
       submenu of the big EscapeMenu.  Here you can change sound vol.,
       gamma correction, fullscreen mode, display of FPS and such
       things.

@Ret:  none
* $Function----------------------------------------------------------*/
void
Droid_Talk_Options_Menu (void)
{
  int MenuPosition=1;

  bool finished = FALSE;
  bool key_pressed = FALSE;

  enum
    { 
    INFLU_REFRESH_TEXT=1,
    INFLU_BLAST_TEXT,
    ENEMY_HIT_TEXT,
    ENEMY_BUMP_TEXT,
    ENEMY_AIM_TEXT,
    ALL_TEXTS,
    BACK 
  };

  while ( EscapePressed() );

  while (!finished)
    {
      key_pressed = FALSE;
      SDL_BlitSurface (Menu_Background, NULL, ne_screen, NULL);

      PutInfluence(FIRST_MENU_ITEM_POS_X, FIRST_MENU_ITEM_POS_Y + (MenuPosition-1.5)*fheight);

      PrintString (ne_screen, OPTIONS_MENU_ITEM_POS_X, FIRST_MENU_ITEM_POS_Y+0*fheight, 
		   "Player Refresh Texts: %s", 
		   GameConfig.Influencer_Refresh_Text ? "ON" : "OFF");
      PrintString (ne_screen, OPTIONS_MENU_ITEM_POS_X, FIRST_MENU_ITEM_POS_Y+1*fheight, 
		   "Player Blast Texts: %s", 
		   GameConfig.Influencer_Blast_Text ? "ON" : "OFF");
      PrintString (ne_screen, OPTIONS_MENU_ITEM_POS_X, FIRST_MENU_ITEM_POS_Y+2*fheight,
		   "Enemy Hit Texts: %s", GameConfig.Enemy_Hit_Text ? "ON" : "OFF");
      PrintString (ne_screen, OPTIONS_MENU_ITEM_POS_X, FIRST_MENU_ITEM_POS_Y+3*fheight,
		   "Enemy Bumped Texts: %s", GameConfig.Enemy_Bump_Text ? "ON" : "OFF");
      PrintString (ne_screen, OPTIONS_MENU_ITEM_POS_X, FIRST_MENU_ITEM_POS_Y+4*fheight,
		   "Enemy Aim Texts: %s", GameConfig.Enemy_Aim_Text ? "ON" : "OFF");
      PrintString (ne_screen, OPTIONS_MENU_ITEM_POS_X, FIRST_MENU_ITEM_POS_Y+5*fheight,
		   "All in-game Speech: %s", GameConfig.All_Texts_Switch ? "ON" : "OFF");
      PrintString (ne_screen, OPTIONS_MENU_ITEM_POS_X, FIRST_MENU_ITEM_POS_Y+6*fheight, "Back");

      SDL_Flip( ne_screen );

      while (!key_pressed)
	{
	  if ( EscapePressed() )
	    {
	      while ( EscapePressed() );
	      finished = TRUE;
	      key_pressed = TRUE;
	    }

	  if (SpacePressed() ) 
	    {
	      MenuItemSelectedSound();
	      while (SpacePressed());
	      key_pressed = TRUE;
	      switch (MenuPosition) 
		{
		case INFLU_REFRESH_TEXT:
		  GameConfig.Influencer_Refresh_Text=!GameConfig.Influencer_Refresh_Text;
		  break;
		case INFLU_BLAST_TEXT:
		  GameConfig.Influencer_Blast_Text=!GameConfig.Influencer_Blast_Text;
		  break;
		case ENEMY_HIT_TEXT:
		  GameConfig.Enemy_Hit_Text=!GameConfig.Enemy_Hit_Text;
		  break;
		case ENEMY_BUMP_TEXT:
		  GameConfig.Enemy_Bump_Text=!GameConfig.Enemy_Bump_Text;
		  break;
		case ENEMY_AIM_TEXT:
		  GameConfig.Enemy_Aim_Text=!GameConfig.Enemy_Aim_Text;
		  break;
		case ALL_TEXTS:
		  GameConfig.All_Texts_Switch=!GameConfig.All_Texts_Switch;
		  break;
		case BACK:
		  finished=TRUE;
		  break;
		default: 
		  break;
		} 
	    } // if SpacePressed

	  if (UpPressed() || WheelUpPressed ()) 
	    {
	      if ( MenuPosition > 1 ) MenuPosition--;
	      else MenuPosition = BACK;
	      MoveMenuPositionSound();
	      while (UpPressed());
	      key_pressed = TRUE;
	    }
	  if (DownPressed()) 
	    {
	      if ( MenuPosition < BACK ) MenuPosition++;
	      MoveMenuPositionSound();
	      while (DownPressed());
	      key_pressed = TRUE;
	    }
	} // while !key_pressed
    } // while !finished

  return;

}; // Droid_Talk_Options_Menu

/*@Function============================================================
@Desc: This function provides a the options menu.  This menu is a 
       submenu of the big EscapeMenu.  Here you can change sound vol.,
       gamma correction, fullscreen mode, display of FPS and such
       things.

@Ret:  none
* $Function----------------------------------------------------------*/
void
Options_Menu (void)
{
  int MenuPosition=1;
  bool finished = FALSE;
  bool key_pressed = FALSE;

enum
  { GRAPHICS_SOUND_OPTIONS=1, 
    DROID_TALK_OPTIONS,
    ON_SCREEN_DISPLAYS,
    EXP_MISSION,
    BACK };

  while ( EscapePressed() );

  while (!finished)
    {
      SDL_BlitSurface (Menu_Background, NULL, ne_screen, NULL);
      key_pressed = FALSE;

      PutInfluence( FIRST_MENU_ITEM_POS_X,
		    FIRST_MENU_ITEM_POS_Y + ( MenuPosition - 1.5 ) *fheight);

      PrintString (ne_screen, OPTIONS_MENU_ITEM_POS_X, FIRST_MENU_ITEM_POS_Y+0*fheight,
		   "Graphics & Sound" );
      PrintString (ne_screen, OPTIONS_MENU_ITEM_POS_X, FIRST_MENU_ITEM_POS_Y+1*fheight,
		   "Droid Talk" );
      PrintString (ne_screen, OPTIONS_MENU_ITEM_POS_X, FIRST_MENU_ITEM_POS_Y+2*fheight,
		   "On-Screen Displays" );
      PrintString (ne_screen, OPTIONS_MENU_ITEM_POS_X, FIRST_MENU_ITEM_POS_Y+3*fheight,
		   "Asteroid Mission (unfinished)");
      PrintString (ne_screen, OPTIONS_MENU_ITEM_POS_X, FIRST_MENU_ITEM_POS_Y+4*fheight, 
		   "Back");

      SDL_Flip( ne_screen );

      while (!key_pressed)
	{
	  if ( EscapePressed() )
	    {
	      while ( EscapePressed() );
	      finished = TRUE;
	      key_pressed = TRUE;
	    }

	  if (SpacePressed() ) 
	    {
	      MenuItemSelectedSound();
	      while (SpacePressed());
	      key_pressed = TRUE;
	      switch (MenuPosition) 
		{
		case GRAPHICS_SOUND_OPTIONS:
		  GraphicsSound_Options_Menu();
		  break;
		case DROID_TALK_OPTIONS:
		  Droid_Talk_Options_Menu();
		  break;
		case ON_SCREEN_DISPLAYS:
		  On_Screen_Display_Options_Menu();
		  break;
		case EXP_MISSION:
		  InitNewMission (NEW_MISSION);
		  break;
		case BACK:
		  finished = TRUE;
		  break;
		default: 
		  break;
		} 
	    } // if SpacePressed

	  if (UpPressed() || WheelUpPressed()) 
	    {
	      if ( MenuPosition > 1 ) MenuPosition--;
	      else MenuPosition = BACK;
	      MoveMenuPositionSound();
	      while (UpPressed());
	      key_pressed = TRUE;
	    }
	  if (DownPressed() || WheelDownPressed()) 
	    {
	      if ( MenuPosition < BACK ) MenuPosition++;
	      else MenuPosition = 1;
	      MoveMenuPositionSound();
	      while (DownPressed());
	      key_pressed = TRUE;
	    }
	} // while !key_pressed

    } // while !finished

  return;

} // Options_Menu


/*@Function============================================================
@Desc: This function provides the credits screen.  It is a submenu of
       the big EscapeMenu.  Here you can see who helped developing the
       game, currently jp, rp and bastian.

@Ret:  none
* $Function----------------------------------------------------------*/
void
Credits_Menu (void)
{
  int h; 
  SDL_Rect screen;
  h = FontHeight(Menu_BFont);
  
  Copy_Rect (Full_Screen_Rect, screen);
  SDL_SetClipRect( ne_screen, NULL );
  DisplayImage ( find_file(NE_CREDITS_PIC_FILE,GRAPHICS_DIR,FALSE) );
  MakeGridOnScreen (&screen);


  printf_SDL (ne_screen, User_Rect.w/2 - 20, 20, "CREDITS\n");

  printf_SDL (ne_screen, 20, -1, "PROGRAMMING:");
  printf_SDL (ne_screen, 2*User_Rect.w/3, -1, "Johannes Prix\n");
  printf_SDL (ne_screen, -1, -1, "Reinhard Prix\n");
  printf_SDL (ne_screen, -1, -1, "\n");

  printf_SDL (ne_screen, 20, -1, "ARTWORK:");
  printf_SDL (ne_screen, 2*User_Rect.w/3, -1, "Bastian Salmela\n");
  printf_SDL (ne_screen, -1 , -1, "Lanzz\n");
  printf_SDL (ne_screen, -1, -1, "\n");

  printf_SDL (ne_screen, 20, -1, "C64 LEGACY MUSIC mods:\n");

  printf_SDL (ne_screen, 20, -1, "#dreamfish/trsi:");
  printf_SDL (ne_screen, User_Rect.w/2 +40, -1, "Green Beret, Sanxion,\n");
  printf_SDL (ne_screen, User_Rect.w/2 +40, -1, "Uridium2\n");

  printf_SDL (ne_screen, 20, -1, "4-mat:");
  printf_SDL (ne_screen, User_Rect.w/2 +40, -1, "The last V8, Anarchy\n");

  printf_SDL (ne_screen, 20, -1, "Kollaps:");
  printf_SDL (ne_screen, User_Rect.w/2 +40, -1, "Tron\n");

  printf_SDL (ne_screen, 20, -1, "Nashua:");
  printf_SDL (ne_screen, User_Rect.w/2 +40, -1, "Starpaws\n");
  
  printf_SDL (ne_screen, 20, -1, "Android:");
  printf_SDL (ne_screen, User_Rect.w/2 +40, -1, "Commando\n");

  SDL_Flip( ne_screen );

  while (!SpacePressed() && !EscapePressed());
  while (SpacePressed() || EscapePressed());

  return;

} // Credits_Menu

/*@Function============================================================
@Desc: This function is used by the Level Editor integrated into 
       freedroid.  It highlights the map position that is currently 
       edited or would be edited, if the user pressed something.  I.e. 
       it provides a "cursor" for the Level Editor.

@Ret:  none
* $Function----------------------------------------------------------*/
void 
Highlight_Current_Block(void)
{
  int i;
#define HIGHLIGHTCOLOR 255

  SDL_LockSurface( ne_screen );

  for (i=0; i<Block_Width; i++)
    {
      // This draws a (double) line at the upper border of the current block
      putpixel( ne_screen , i + User_Rect.x + (User_Rect.w/2) + (rintf(Me.pos.x)-Me.pos.x - 0.5) * Block_Width , User_Rect.y + User_Rect.h/2 + (rintf(Me.pos.y)-Me.pos.y - 0.5 ) * Block_Height , HIGHLIGHTCOLOR );
      putpixel( ne_screen , i + User_Rect.x + (User_Rect.w/2) + (rintf(Me.pos.x)-Me.pos.x - 0.5) * Block_Width , User_Rect.y + User_Rect.h/2 + (rintf(Me.pos.y)-Me.pos.y - 0.5 ) * Block_Height + 1, HIGHLIGHTCOLOR );

      // This draws a line at the lower border of the current block
      putpixel( ne_screen , i + User_Rect.x + (User_Rect.w/2) + (rintf(Me.pos.x)-Me.pos.x - 0.5) * Block_Width , User_Rect.y + User_Rect.h/2 + (rintf(Me.pos.y)-Me.pos.y + 0.5 ) * Block_Height -1, HIGHLIGHTCOLOR );
      putpixel( ne_screen , i + User_Rect.x + (User_Rect.w/2) + (rintf(Me.pos.x)-Me.pos.x - 0.5) * Block_Width , User_Rect.y + User_Rect.h/2 + (rintf(Me.pos.y)-Me.pos.y + 0.5 ) * Block_Height -2, HIGHLIGHTCOLOR );

      // This draws a line at the left border of the current block
      putpixel( ne_screen , 0 + User_Rect.x + (User_Rect.w/2) + (rintf(Me.pos.x)-Me.pos.x - 0.5) * Block_Width , User_Rect.y + User_Rect.h/2 + (rintf(Me.pos.y)-Me.pos.y - 0.5 ) * Block_Height + i , HIGHLIGHTCOLOR );
      putpixel( ne_screen , 1 + User_Rect.x + (User_Rect.w/2) + (rintf(Me.pos.x)-Me.pos.x - 0.5) * Block_Width , User_Rect.y + User_Rect.h/2 + (rintf(Me.pos.y)-Me.pos.y - 0.5 ) * Block_Height + i , HIGHLIGHTCOLOR );

      // This draws a line at the right border of the current block
      putpixel( ne_screen , -1 + User_Rect.x + (User_Rect.w/2) + (rintf(Me.pos.x)-Me.pos.x + 0.5) * Block_Width , User_Rect.y + User_Rect.h/2 + (rintf(Me.pos.y)-Me.pos.y - 0.5 ) * Block_Height + i , HIGHLIGHTCOLOR );
      putpixel( ne_screen , -2 + User_Rect.x + (User_Rect.w/2) + (rintf(Me.pos.x)-Me.pos.x + 0.5) * Block_Width , User_Rect.y + User_Rect.h/2 + (rintf(Me.pos.y)-Me.pos.y - 0.5 ) * Block_Height + i , HIGHLIGHTCOLOR );

    }

  SDL_UnlockSurface( ne_screen );
} // void Highlight_Current_Block(void)

/*@Function============================================================
@Desc: This function is used by the Level Editor integrated into 
       freedroid.  It marks all waypoints with a cross.

@Ret:  none
* $Function----------------------------------------------------------*/
void 
Show_Waypoints(void)
{
  int wp;
  int i;
  int x;
  int y;
  int BlockX, BlockY;
  int color;
#define ACTIVE_WP_COLOR 0x0FFFFFFFF

  BlockX=rintf(Me.pos.x);
  BlockY=rintf(Me.pos.y);
	  
  SDL_LockSurface( ne_screen );

  for (wp=0; wp<MAXWAYPOINTS; wp++)
    {

      if ( CurLevel->AllWaypoints[wp].x == 0) continue;

      //--------------------
      // Draw the cross in the middle of the middle of the tile
      //
      for (i= Block_Width/4; i<3 * Block_Width / 4; i++)
	{
	  // This draws a (double) line at the upper border of the current block
	  x = i + User_Rect.x+(User_Rect.w/2)- (( Me.pos.x)-CurLevel->AllWaypoints[wp].x + 0.5) * Block_Width;
	  y = i + User_Rect.y+User_Rect.h/2 - (( Me.pos.y)-CurLevel->AllWaypoints[wp].y + 0.5) * Block_Height;
	  if ( ( x < User_Rect.x ) || ( x > User_Rect.x + User_Rect.w ) || ( y < User_Rect. y) || ( y > User_Rect.y + User_Rect.h ) ) continue;
	  putpixel( ne_screen , x , y , HIGHLIGHTCOLOR );

		    
	  x = i + User_Rect.x + (User_Rect.w/2) - (( Me.pos.x )-CurLevel->AllWaypoints[wp].x + 0.5) * Block_Width;
	  y = i + User_Rect.y+User_Rect.h/2- (( Me.pos.y)-CurLevel->AllWaypoints[wp].y + 0.5) * Block_Height + 1;
	  if ( ( x < User_Rect.x ) || ( x > User_Rect.x + User_Rect.w ) || ( y < User_Rect. y) || ( y > User_Rect.y + User_Rect.h ) ) continue;
	  putpixel( ne_screen , x , y , HIGHLIGHTCOLOR );
	  
	  // This draws a line at the lower border of the current block
	  x = i + User_Rect.x + (User_Rect.w/2) - (( Me.pos.x)-CurLevel->AllWaypoints[wp].x + 0.5) * Block_Width;
	  y = -i + User_Rect.y + User_Rect.h/2 - (( Me.pos.y )-CurLevel->AllWaypoints[wp].y - 0.5 ) * Block_Height -1;
	  if ( ( x < User_Rect.x ) || ( x > User_Rect.x + User_Rect.w ) || ( y < User_Rect. y) || ( y > User_Rect.y + User_Rect.h ) ) continue;
	  putpixel( ne_screen , x , y , HIGHLIGHTCOLOR );

	  x = i + User_Rect.x + (User_Rect.w/2) - (( Me.pos.x)-CurLevel->AllWaypoints[wp].x + 0.5) * Block_Width;
	  y = -i + User_Rect.y + User_Rect.h/2 - ((Me.pos.y)-CurLevel->AllWaypoints[wp].y - 0.5 ) * Block_Height -2;
	  if ( ( x < User_Rect.x ) || ( x > User_Rect.x + User_Rect.w ) || ( y < User_Rect. y) || ( y > User_Rect.y + User_Rect.h ) ) continue;
	  putpixel( ne_screen , x , y , HIGHLIGHTCOLOR );
	  
	}

      //--------------------
      // Draw the connections to other waypoints, BUT ONLY FOR THE WAYPOINT CURRENTLY TARGETED
      //
      for ( i=0; i<MAX_WP_CONNECTIONS; i++ )
	{
	  if ( CurLevel->AllWaypoints[wp].connections[i] != (-1) )
	    {
	       if ( ( BlockX == CurLevel->AllWaypoints[wp].x ) && ( BlockY == CurLevel->AllWaypoints[wp].y ) )
		 // color = ACTIVE_WP_COLOR ;
		 // else color = HIGHLIGHTCOLOR ; 
		 // printf(" Found a connection!! ");
		 DrawLineBetweenTiles( CurLevel->AllWaypoints[wp].x , CurLevel->AllWaypoints[wp].y , 
				       CurLevel->AllWaypoints[CurLevel->AllWaypoints[wp].connections[i]].x , 
				       CurLevel->AllWaypoints[CurLevel->AllWaypoints[wp].connections[i]].y ,
				       color );
	    }
	}
    }
  SDL_UnlockSurface( ne_screen );

} // void Show_Waypoints(void);

/*@Function============================================================
@Desc: This function is provides the Level Editor integrated into 
       freedroid.  Actually this function is a submenu of the big
       Escape Menu.  In here you can edit the level and upon pressing
       escape enter a further submenu where you can save the level,
       change level name and quit from level editing.

       NOTE: SAVING CURRENTLY DOES NOT WORK!  DONT WORK TOO MUCH WITH
             THIS IF YOU CANT SAVE YOUR LEVELS LATER!!!

@Ret:  none
* $Function----------------------------------------------------------*/
void 
Level_Editor(void)
{
  int BlockX=rintf(Me.pos.x);
  int BlockY=rintf(Me.pos.y);
  int Done=FALSE;
  int Weiter=FALSE;
  int MenuPosition=1;
  int i,j,k;
  int SpecialMapValue;
  int OriginWaypoint = (-1);
  char* NumericInputString;
  char* OldMapPointer;
  bool key_pressed;

  enum { 
    SAVE_LEVEL_POSITION=1, 
    CHANGE_LEVEL_POSITION, 
    CHANGE_TILE_SET_POSITION, 
    CHANGE_SIZE_X, 
    CHANGE_SIZE_Y, 
    SET_LEVEL_NAME, 
    SET_BACKGROUND_SONG_NAME, 
    SET_LEVEL_COMMENT, 
    BACK};

  while ( !Done )
    {
      Weiter=FALSE;
      while (!EscapePressed())
	{
	  BlockX=rintf(Me.pos.x);
	  BlockY=rintf(Me.pos.y);
	  
	  ClearUserFenster();
	  Assemble_Combat_Picture ( ONLY_SHOW_MAP );
	  Highlight_Current_Block();
	  Show_Waypoints();

	  CenteredPutString ( ne_screen ,  1*FontHeight(Menu_BFont),    "LEVEL EDITOR");
	  SDL_Flip( ne_screen );

	  //--------------------
	  // If the user of the Level editor pressed some cursor keys, move the
	  // highlited filed (that is Me.pos) accordingly. This is done here:
	  if (LeftPressed()) 
	    {
	      if ( rintf(Me.pos.x) > 0 ) Me.pos.x-=1;
	      while (LeftPressed());
	    }
	  if (RightPressed()) 
	    {
	      if ( rintf(Me.pos.x) < CurLevel->xlen-1 ) Me.pos.x+=1;
	      while (RightPressed());
	    }
	  if (UpPressed()) 
	    {
	      if ( rintf(Me.pos.y) > 0 ) Me.pos.y-=1;
	      while (UpPressed());
	    }
	  if (DownPressed()) 
	    {
	      if ( rintf(Me.pos.y) < CurLevel->ylen-1 ) Me.pos.y+=1;
	      while (DownPressed());
	    }

	  //--------------------
	  // Since the level editor will not always be able to
	  // immediately feature all the the map tiles that might
	  // have been added recently, we should offer a feature, so that you can
	  // specify the value of a map piece just numerically.  This will be
	  // done upon pressing the 'e' key.
	  //
	  if ( EPressed () )
	    {
	      while (EPressed());
	      CenteredPutString   ( ne_screen ,  6*FontHeight(Menu_BFont), "Please enter new value (blindly):");
	      SDL_Flip( ne_screen );
	      NumericInputString=GetString( 10, FALSE );  // TRUE currently not implemented
	      sscanf( NumericInputString , "%d" , &SpecialMapValue );
	      if ( SpecialMapValue >= NUM_MAP_BLOCKS ) SpecialMapValue=0;
	      CurLevel->map[BlockY][BlockX]=SpecialMapValue;
	    }

	  //--------------------
	  //If the person using the level editor decides he/she wants a different
	  //scale for the editing process, he/she may say so by using the O/I keys.
	  //
	  if ( OPressed () )
	    {
	      if (CurrentCombatScaleFactor > 0.25 )
		CurrentCombatScaleFactor -= 0.25;
	      SetCombatScaleTo (CurrentCombatScaleFactor);
	      while (OPressed());
	    }
	  if ( IPressed () )
	    {
	      CurrentCombatScaleFactor += 0.25;
	      SetCombatScaleTo (CurrentCombatScaleFactor);
	      while (IPressed());
	    }
  
	  // If the person using the level editor pressed w, the waypoint is
	  // toggled on the current square.  That means either removed or added.
	  // And in case of removal, also the connections must be removed.
	  if (WPressed())
	    {
	      // find out if there is a waypoint on the current square
	      for (i=0 ; i < MAXWAYPOINTS ; i++)
		{
		  if ( ( CurLevel->AllWaypoints[i].x == BlockX ) &&
		       ( CurLevel->AllWaypoints[i].y == BlockY ) ) break;
		}
	      
	      // if its waypoint already, this waypoint must be deleted.
	      if ( i != MAXWAYPOINTS )
		{
		  // Eliminate the waypoint itself
		  CurLevel->AllWaypoints[i].x = 0;
		  CurLevel->AllWaypoints[i].y = 0;
		  for ( k = 0; k < MAX_WP_CONNECTIONS ; k++) 
		    CurLevel->AllWaypoints[i].connections[k] = (-1) ;

		  
		  // Eliminate all connections pointing to this waypoint
		  for ( j = 0; j < MAXWAYPOINTS ; j++ )
		    {
		      for ( k = 0; k < MAX_WP_CONNECTIONS ; k++) 
			if ( CurLevel->AllWaypoints[j].connections[k] == i )
			  CurLevel->AllWaypoints[j].connections[k] = (-1) ;
		    }
		}
	      else // if its not a waypoint already, it must be made into one
		{
		  // seek a free position
		  for ( i = 0 ; i < MAXWAYPOINTS ; i++ )
		    {
		      if ( CurLevel->AllWaypoints[i].x == 0 ) break;
		    }
		  if ( i == MAXWAYPOINTS )
		    {
		      printf("\n\nSorry, no free waypoint available.  Using the first one.");
		      i = 0;
		    }

		  // Now make the new entry into the waypoint list
		  CurLevel->AllWaypoints[i].x = BlockX;
		  CurLevel->AllWaypoints[i].y = BlockY;

		  // delete all old connection information from the new waypoint
		  for ( k = 0; k < MAX_WP_CONNECTIONS ; k++ ) 
		    CurLevel->AllWaypoints[i].connections[k] = (-1) ;

		}

	      printf("\n\n  i is now: %d ", i ); fflush(stdout);

	      while ( WPressed() );
	    }

	  // If the person using the level editor presses C that indicated he/she wants
	  // a connection between waypoints.  If this is the first selected waypoint, its
	  // an origin and the second "C"-pressed waypoint will be used a target.
	  // If origin and destination are the same, the operation is cancelled.
	  if (CPressed())
	    {
	      // Determine which waypoint is currently targeted
	      for (i=0 ; i < MAXWAYPOINTS ; i++)
		{
		  if ( ( CurLevel->AllWaypoints[i].x == BlockX ) &&
		       ( CurLevel->AllWaypoints[i].y == BlockY ) ) break;
		}

	      if ( i == MAXWAYPOINTS )
		{
		  printf("\n\nSorry, don't know which waypoint you mean.");
		}
	      else
		{
		  printf("\n\nYou specified waypoint nr. %d.",i);
		  if ( OriginWaypoint== (-1) )
		    {
		      printf("\nIt has been marked as the origin of the next connection.");
		      OriginWaypoint = i;
		    }
		  else
		    {
		      if ( OriginWaypoint == i )
			{
			  printf("\n\nOrigin==Target --> Connection Operation cancelled.");
			  OriginWaypoint = (-1);
			}
		      else
			{
			  printf("\n\nOrigin: %d Target: %d. Operation makes sense.", OriginWaypoint , i );
			  for ( k = 0; k < MAX_WP_CONNECTIONS ; k++ ) 
			    {
			      if (CurLevel->AllWaypoints[ OriginWaypoint ].connections[k] == (-1) ) break;
			    }
			  if ( k == MAX_WP_CONNECTIONS ) 
			    {
			      printf("\nSORRY. NO MORE CONNECTIONS AVAILABLE FROM THERE.");
			    }
			  else
			    {
			      CurLevel->AllWaypoints[ OriginWaypoint ].connections[k] = i;
			      printf("\nOPERATION DONE!! CONNECTION SHOULD BE THERE.");
			    }
			  OriginWaypoint = (-1);
			}
		    }
		}

	      while (CPressed());
	      fflush(stdout);
	    }

	  // If the person using the level editor pressed some editing keys, insert the
	  // corresponding map tile.  This is done here:
	  if (Number1Pressed()) 
	    {
	      CurLevel->map[BlockY][BlockX]=BLOCK1;
	    }
	  if (Number2Pressed()) 
	    {
	      CurLevel->map[BlockY][BlockX]=BLOCK2;
	    }
	  if (Number3Pressed()) 
	    {
	      CurLevel->map[BlockY][BlockX]=BLOCK3;
	    }
	  if (Number4Pressed()) 
	    {
	      CurLevel->map[BlockY][BlockX]=BLOCK4;
	    }
	  if (Number5Pressed()) 
	    {
	      CurLevel->map[BlockY][BlockX]=BLOCK5;
	    }
	  if (LPressed()) 
	    {
	      CurLevel->map[BlockY][BlockX]=LIFT;
	    }
	  if (KP_PLUS_Pressed()) 
	    {
	      CurLevel->map[BlockY][BlockX]=V_WALL;
	    }
	  if (KP0Pressed()) 
	    {
	      CurLevel->map[BlockY][BlockX]=H_WALL;
	    }
	  if (KP1Pressed()) 
	    {
	      CurLevel->map[BlockY][BlockX]=ECK_LU;
	    }
	  if (KP2Pressed()) 
	    {
	      if (!Shift_Was_Pressed())
		CurLevel->map[BlockY][BlockX]=T_U;
	      else CurLevel->map[BlockY][BlockX]=KONSOLE_U;
	    }
	  if (KP3Pressed()) 
	    {
	      CurLevel->map[BlockY][BlockX]=ECK_RU;
	    }
	  if (KP4Pressed()) 
	    {
	      if (!Shift_Was_Pressed())
		CurLevel->map[BlockY][BlockX]=T_L;
	      else CurLevel->map[BlockY][BlockX]=KONSOLE_L;
	    }
	  if (KP5Pressed()) 
	    {
	      if (!Shift_Was_Pressed())
		CurLevel->map[BlockY][BlockX]=KREUZ;
	      else CurLevel->map[BlockY][BlockX]=VOID;
	    }
	  if (KP6Pressed()) 
	    {
	      if (!Shift_Was_Pressed())
		CurLevel->map[BlockY][BlockX]=T_R;
	      else CurLevel->map[BlockY][BlockX]=KONSOLE_R;
	    }
	  if (KP7Pressed()) 
	    {
	      CurLevel->map[BlockY][BlockX]=ECK_LO;
	    }
	  if (KP8Pressed()) 
	    {
	      if (!Shift_Was_Pressed())
		CurLevel->map[BlockY][BlockX]=T_O;
	      else CurLevel->map[BlockY][BlockX]=KONSOLE_O;
	    }
	  if (KP9Pressed()) 
	    {
	      CurLevel->map[BlockY][BlockX]=ECK_RO;
	    }
	  if (APressed())
	    {
	      CurLevel->map[BlockY][BlockX]=ALERT;	      
	    }
	  if (RPressed())
	    {
	      CurLevel->map[BlockY][BlockX]=REFRESH1;	            
	    }
	  if (DPressed())
	    {
	      if (Shift_Was_Pressed())
		CurLevel->map[BlockY][BlockX]=V_ZUTUERE;	            	      
	      else CurLevel->map[BlockY][BlockX]=H_ZUTUERE;	            	      
	    }
	  if (SpacePressed())
	    CurLevel->map[BlockY][BlockX]=FLOOR;	            	      	    
	  if (QPressed())
	    {
	      Terminate(0);
	    }

	} // while (!EscapePressed())
      while( EscapePressed() );

      // After Level editing is done and escape has been pressed, 
      // display the Menu with level save options and all that.

      while (!Weiter)
	{
	  int xoffs = 110;
	  key_pressed = FALSE;
	  SDL_BlitSurface (Menu_Background, NULL, ne_screen, NULL);

	  // Highlight currently selected option with an influencer before it
	  PutInfluence( FIRST_MENU_ITEM_POS_X-xoffs, FIRST_MENU_ITEM_POS_Y+(MenuPosition-1.5)*fheight);

	  PrintString (ne_screen, OPTIONS_MENU_ITEM_POS_X-xoffs, FIRST_MENU_ITEM_POS_Y + 0*fheight, 
		       "Save ship as  'Testship.shp'");
	  PrintString (ne_screen, OPTIONS_MENU_ITEM_POS_X-xoffs, FIRST_MENU_ITEM_POS_Y + 1*fheight, 
		       "Current: %d.  Level +/-" , CurLevel->levelnum );
	  PrintString (ne_screen, OPTIONS_MENU_ITEM_POS_X-xoffs, FIRST_MENU_ITEM_POS_Y + 2*fheight, 
		       "Change tile set (color)");
	  PrintString (ne_screen, OPTIONS_MENU_ITEM_POS_X-xoffs, FIRST_MENU_ITEM_POS_Y + 3*fheight, 
		       "Levelsize in X: %d.  -/+" , CurLevel->xlen );
	  PrintString (ne_screen, OPTIONS_MENU_ITEM_POS_X-xoffs, FIRST_MENU_ITEM_POS_Y + 4*fheight, 
		       "Levelsize in Y: %d.  -/+" , CurLevel->ylen );
	  PrintString (ne_screen, OPTIONS_MENU_ITEM_POS_X-xoffs, FIRST_MENU_ITEM_POS_Y + 5*fheight, 
		       "Level name: %s" , CurLevel->Levelname );
	  PrintString (ne_screen, OPTIONS_MENU_ITEM_POS_X-xoffs, FIRST_MENU_ITEM_POS_Y + 6*fheight, 
		       "Background music: %s" , CurLevel->Background_Song_Name );
	  PrintString (ne_screen, OPTIONS_MENU_ITEM_POS_X-xoffs, FIRST_MENU_ITEM_POS_Y + 7*fheight, 
		       "Level Comment: %s" , CurLevel->Level_Enter_Comment );
	  PrintString (ne_screen, OPTIONS_MENU_ITEM_POS_X-xoffs, FIRST_MENU_ITEM_POS_Y + 8*fheight, 
		       "Quit Level Editor");
	  
	  SDL_Flip ( ne_screen );
	  
	  while (!key_pressed)
	    {
	      if ( EscapePressed() )
		{
		  while (EscapePressed());
		  Weiter=TRUE;
		  key_pressed = TRUE;
		}
	  
	      if (SpacePressed() ) 
		{
		  MenuItemSelectedSound();
		  while (SpacePressed() );
		  key_pressed = TRUE;
		  switch (MenuPosition) 
		    {
		  
		    case SAVE_LEVEL_POSITION:
		      SaveShip("Testship");
		      CenteredPutString ( ne_screen ,  11*FontHeight(Menu_BFont),    "Your ship was saved...");
		      SDL_Flip ( ne_screen );
		      Wait4Fire();
		      break;
		    case CHANGE_LEVEL_POSITION: 
		      break;
		    case CHANGE_TILE_SET_POSITION: 
		      break;
		    case SET_LEVEL_NAME:
		      ClearUserFenster();
		      Assemble_Combat_Picture ( ONLY_SHOW_MAP );
		      DisplayText ("New level name: ",
				   FIRST_MENU_ITEM_POS_X-50, FIRST_MENU_ITEM_POS_X+ 5*fheight, 
				   &Full_User_Rect);
		      SDL_Flip( ne_screen );
		      CurLevel->Levelname=GetString(15, FALSE );
		      Weiter=!Weiter;
		      break;
		    case SET_BACKGROUND_SONG_NAME:
		      ClearUserFenster();
		      Assemble_Combat_Picture ( ONLY_SHOW_MAP );
		      DisplayText ("Bg music filename: ", 
				   FIRST_MENU_ITEM_POS_X-50, FIRST_MENU_ITEM_POS_X+ 5*fheight, 
				   &Full_User_Rect);
		      SDL_Flip( ne_screen );
		      CurLevel->Background_Song_Name=GetString(20 , FALSE );
		      break;
		    case SET_LEVEL_COMMENT:
		      ClearUserFenster();
		      Assemble_Combat_Picture ( ONLY_SHOW_MAP );
		      DisplayText ("New level-comment :",
				   FIRST_MENU_ITEM_POS_X-50, FIRST_MENU_ITEM_POS_X+ 5*fheight, 
				   &Full_User_Rect);
		      SDL_Flip( ne_screen );
		      CurLevel->Level_Enter_Comment=GetString(15 , FALSE );
		      break;

		    case BACK:
		      Weiter=!Weiter;
		      Done=TRUE;
		      SetCombatScaleTo( 1 );
		      break;
		    default: 
		      break;

		    } // switch
		} // if SpacePressed
	      
	      // If the user of the level editor pressed left or right, that should have
	      // an effect IF he/she is a the change level menu point

	      if (LeftPressed() || RightPressed() ) 
		{
		  key_pressed = TRUE;
		  switch (MenuPosition)
		    {

		    case CHANGE_LEVEL_POSITION:
		      if ( LeftPressed() )
			{
			  if ( CurLevel->levelnum > 0 )
			    Teleport ( CurLevel->levelnum -1 , 3 , 3 );
			  while (LeftPressed());
			}

		      if ( RightPressed() )
			{
			  if ( CurLevel->levelnum < curShip.num_levels -1 )
			    Teleport ( CurLevel->levelnum +1 , 3 , 3 );
			  while (RightPressed());
			}
		      
		      SetCombatScaleTo ( CurrentCombatScaleFactor );
		      break;
		  
		    case CHANGE_TILE_SET_POSITION:
		      if ( RightPressed() && (CurLevel->color  < 6 ) )
			{
			  CurLevel->color++;
			  while (RightPressed());
			}
		      if ( LeftPressed() && (CurLevel->color > 0) )
			{
			  CurLevel->color--;
			  while (LeftPressed());
			}
		      Teleport ( CurLevel->levelnum , Me.pos.x , Me.pos.y ); 
		      break;
		    case CHANGE_SIZE_X:
		      if ( RightPressed() )
			{
			  CurLevel->xlen++;
			  // In case of enlargement, we need to do more:
			  for ( i = 0 ; i < CurLevel->ylen ; i++ )
			    {
			      OldMapPointer=CurLevel->map[i];
			      CurLevel->map[i] = MyMalloc( CurLevel->xlen +1) ;
			      memcpy( CurLevel->map[i] , OldMapPointer , CurLevel->xlen-1 );
			      // We don't want to fill the new area with junk, do we? So we set it VOID
			      CurLevel->map[ i ] [ CurLevel->xlen-1 ] = VOID;  
			    }
			  while (RightPressed());
			}
		      if ( LeftPressed() )
			{
			  CurLevel->xlen--; // making it smaller is always easy:  just modify the value for size
			  // allocation of new memory or things like that are not nescessary.
			  while (LeftPressed());
			}
		      break;
		  
		    case CHANGE_SIZE_Y:
		      if ( RightPressed() )
			{
			  CurLevel->ylen++;
		      
			  // In case of enlargement, we need to do more:
			  CurLevel->map[ CurLevel->ylen-1 ] = MyMalloc( CurLevel->xlen +1) ;
			  
			  // We don't want to fill the new area with junk, do we? So we set it VOID
			  memset( CurLevel->map[ CurLevel->ylen-1 ] , VOID , CurLevel->xlen );
			  
			  while (RightPressed());
		      
			}

		      if ( LeftPressed() )
			{
			  CurLevel->ylen--; // making it smaller is always easy:  just modify the value for size
			  // allocation of new memory or things like that are not nescessary.
			  while (LeftPressed());
			}
		      break;
		  
		    }
		} // if LeftPressed || RightPressed

	      // If the user pressed up or down, the cursor within
	      // the level editor menu has to be moved, which is done here:
	      if (UpPressed() || WheelUpPressed ()) 
		{
		  if (MenuPosition > 1) MenuPosition--;
		  else MenuPosition = BACK;
		  MoveMenuPositionSound();
		  while (UpPressed());
		  key_pressed = TRUE;
		}
	      if (DownPressed() || WheelDownPressed ()) 
		{
		  if ( MenuPosition < BACK ) MenuPosition++;
		  else MenuPosition = 1;
		  MoveMenuPositionSound();
		  while (DownPressed());
		  key_pressed = TRUE;
		}

	    } // while !key_pressed

	} // while !Weiter
      
    } // while (!Done)

  ClearGraphMem();
  return;

} // void Level_Editor(void)




#undef _menu_c
