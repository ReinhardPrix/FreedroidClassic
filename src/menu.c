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

int New_Game_Requested=FALSE;

void Single_Player_Menu (void);
void Multi_Player_Menu (void);
void Credits_Menu (void);
void Options_Menu (void);
void Show_Mission_Log_Menu (void);
void Show_Waypoints(void);
void Level_Editor(void);

EXTERN int MyCursorX;
EXTERN int MyCursorY;
EXTERN char Previous_Mission_Name[1000];

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
  SDL_SetClipRect( ne_screen, NULL );
  ClearGraphMem();
  DisplayBanner (NULL, NULL,  BANNER_NO_SDL_UPDATE | BANNER_FORCE_UPDATE );
  Assemble_Combat_Picture ( 0 );
  SDL_SetClipRect( ne_screen, NULL );
  MakeGridOnScreen( NULL );
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
MissionSelectMenu (void)
{
#define FIRST_MIS_SELECT_ITEM_POS_X (0.0*Block_Width)
#define FIRST_MIS_SELECT_ITEM_POS_Y (USERFENSTERPOSY + FontHeight(Menu_BFont))
enum
  { 
    CLASSIC_PARADROID_MISSION_POSITION=1, 
    NEW_MISSION_POSITION,
    RESTART_PREVIOUS_MISSION
  };
  int Weiter = 0;
  int MenuPosition=1;
  int key;
  static int NoMissionLoadedEver=TRUE;

  Me.status=MENU;

  DebugPrintf (2, "\nvoid MissionSelectMenu(void): real function call confirmed."); 

  SDL_SetClipRect( ne_screen , NULL );

  // Prevent distortion of framerate by the delay coming from 
  // the time spend in the menu.
  Activate_Conservative_Frame_Computation();

  // This is not some Debug Menu but an optically impressive 
  // menu for the player.  Therefore I suggest we just fade out
  // the game screen a little bit.

  SetCurrentFont( Para_BFont );
  
  while ( EscapePressed() );

  while (!Weiter)
    {

      // InitiateMenu();
      DisplayImage (find_file (NE_TITLE_PIC_FILE, GRAPHICS_DIR, FALSE));

      // 
      // we highlight the currently selected option with an 
      // influencer to the left before it
      // PutInfluence( FIRST_MENU_ITEM_POS_X , 
      // FIRST_MENU_ITEM_POS_Y + (MenuPosition-1) * (FontHeight(Menu_BFont)) - Block_Width/4 );
      SetCurrentFont ( Menu_BFont );
      PutInfluence( FIRST_MIS_SELECT_ITEM_POS_X , FIRST_MIS_SELECT_ITEM_POS_Y + ( MenuPosition - 1.5 ) * (FontHeight( Menu_BFont )) );

      CenteredPutString (ne_screen ,  FIRST_MIS_SELECT_ITEM_POS_Y -2*FontHeight(GetCurrentFont()), "Mission Selection Menu");
      CenteredPutString (ne_screen ,  FIRST_MIS_SELECT_ITEM_POS_Y ,    "Classic Paradroid");
      CenteredPutString (ne_screen ,  FIRST_MIS_SELECT_ITEM_POS_Y +1*FontHeight(GetCurrentFont()), "Asteroid Research");
      CenteredPutString (ne_screen ,  FIRST_MIS_SELECT_ITEM_POS_Y +2*FontHeight(GetCurrentFont()), "Restart Previous Mission");

      SDL_Flip( ne_screen );

      // Wait until the user does SOMETHING

      key = getchar_raw ();
      if ( (key == SDLK_RETURN) || (key == SDLK_SPACE))
	{
	  MenuItemSelectedSound();
	  switch (MenuPosition) 
	    {

	    case CLASSIC_PARADROID_MISSION_POSITION:
	      InitNewMissionList ( STANDARD_MISSION );
	      NoMissionLoadedEver = FALSE;
	      Weiter = TRUE;   
	      break;
	    case NEW_MISSION_POSITION:
	      InitNewMissionList ( NEW_MISSION );
	      NoMissionLoadedEver = FALSE;
	      Weiter = TRUE;   /* jp forgot this... ;) */
	      break;
	    case RESTART_PREVIOUS_MISSION:
	      if ( NoMissionLoadedEver )
		{
		  CenteredPutString (ne_screen ,  FIRST_MIS_SELECT_ITEM_POS_Y +5*FontHeight(GetCurrentFont()), "No previous mission known.");
		  SDL_Flip( ne_screen );
		  while ( EnterPressed() );
		  while ( (!EnterPressed()) && (!SpacePressed()) );
		}
	      else
		{
		  InitNewMissionList ( Previous_Mission_Name );
		  Weiter = TRUE;   /* jp forgot this... ;) */
		}
	      break;
	    default: 
	      break;
	    } 
	  // Weiter=!Weiter;
	}
      if ( key == SDLK_UP )
	{
	  if (MenuPosition > 1) MenuPosition--;
	  MoveMenuPositionSound();
	}
      if ( key == SDLK_DOWN )
	{
	  if ( MenuPosition < RESTART_PREVIOUS_MISSION ) MenuPosition++;
	  MoveMenuPositionSound();
	}
      if ( key == SDLK_ESCAPE )
	{
	  Terminate( OK );
	}
    }

  ClearGraphMem();
  // Since we've faded out the whole scren, it can't hurt
  // to have the top status bar redrawn...
  BannerIsDestroyed=TRUE;
  Me.status=MOBILE;

  return;

} // MissionSelectMenu

/*@Function============================================================
@Desc: This function provides a the big escape menu from where you can
       get into different submenus.

@Ret:  none
* $Function----------------------------------------------------------*/
void
EscapeMenu (void)
{
#define FIRST_MENU_ITEM_POS_X (2*Block_Width)
#define FIRST_MENU_ITEM_POS_Y (USERFENSTERPOSY + FontHeight(Menu_BFont))
enum
  { 
    SINGLE_PLAYER_POSITION=1, 
    MULTI_PLAYER_POSITION, 
    OPTIONS_POSITION, 
    SET_THEME,
    LEVEL_EDITOR_POSITION, 
    CREDITS_POSITION,
    QUIT_POSITION
  };

  int Weiter = 0;
  int MenuPosition=1;
  int h;
  char theme_string[40];
  int i;

  Me.status=MENU;

  DebugPrintf (2, "\nvoid EscapeMenu(void): real function call confirmed."); 

  // Prevent distortion of framerate by the delay coming from 
  // the time spend in the menu.
  Activate_Conservative_Frame_Computation();

  // This is not some Debug Menu but an optically impressive 
  // menu for the player.  Therefore I suggest we just fade out
  // the game screen a little bit.

  SetCurrentFont( Para_BFont );
  
  while ( EscapePressed() );

  while (!Weiter)
    {

      InitiateMenu();

      // 
      // we highlight the currently selected option with an 
      // influencer to the left before it
      // PutInfluence( FIRST_MENU_ITEM_POS_X , 
      // FIRST_MENU_ITEM_POS_Y + (MenuPosition-1) * (FontHeight(Menu_BFont)) - Block_Width/4 );
      SetCurrentFont ( Menu_BFont );
      h = FontHeight (GetCurrentFont());
      PutInfluence( FIRST_MENU_ITEM_POS_X , 
		    FIRST_MENU_ITEM_POS_Y +
		    ( MenuPosition - 1.5 ) * h );

      strcpy (theme_string, "Theme: ");
      if (strstr (GameConfig.Theme_SubPath, "default"))
	strcat (theme_string, "default");
      else if (strstr (GameConfig.Theme_SubPath, "lanzz"))
	strcat (theme_string, "lanzz");
      else
	strcat (theme_string, "unknown");

      CenteredPutString (ne_screen ,  FIRST_MENU_ITEM_POS_Y ,    "Single Player");
      CenteredPutString (ne_screen ,  FIRST_MENU_ITEM_POS_Y +1*h,    "Multi Player");
      CenteredPutString (ne_screen ,  FIRST_MENU_ITEM_POS_Y +2*h,    "Options");
      CenteredPutString (ne_screen ,  FIRST_MENU_ITEM_POS_Y +3*h,  theme_string);
      CenteredPutString (ne_screen ,  FIRST_MENU_ITEM_POS_Y +4*h,    "Level Editor");
      CenteredPutString (ne_screen ,  FIRST_MENU_ITEM_POS_Y +5*h,    "Credits");
      CenteredPutString (ne_screen ,  FIRST_MENU_ITEM_POS_Y +6*h,    "Quit Game");

      SDL_Flip( ne_screen );

      // Wait until the user does SOMETHING

      while( !SpacePressed() && !EnterPressed() && !UpPressed()
	     && !DownPressed() && !EscapePressed() ) ;

      if ( EscapePressed() )
	{
	  while ( EscapePressed() );
	  Weiter=!Weiter;
	}
      if (EnterPressed() || SpacePressed() ) 
	{
	  MenuItemSelectedSound();
	  switch (MenuPosition) 
	    {

	    case SINGLE_PLAYER_POSITION:
	      while (EnterPressed() || SpacePressed() );
	      New_Game_Requested=FALSE;
	      Single_Player_Menu();
	      if (New_Game_Requested) Weiter = TRUE;   /* jp forgot this... ;) */
	      break;
	    case MULTI_PLAYER_POSITION:
	      while (EnterPressed() || SpacePressed() );
	      Multi_Player_Menu();
	      // Weiter = TRUE;   /* jp forgot this... ;) */
	      break;
	    case OPTIONS_POSITION:
	      while (EnterPressed() || SpacePressed() );
	      Options_Menu();
	      // Weiter = TRUE;   /* jp forgot this... ;) */
	      break;
	    case SET_THEME:
	      while (EnterPressed() || SpacePressed() );
	      if ( !strcmp ( GameConfig.Theme_SubPath , "default_theme/" ) )
		{
		  GameConfig.Theme_SubPath="lanzz_theme/";
		}
	      else
		{
		  GameConfig.Theme_SubPath="default_theme/";
		}
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
		{
		  AllBullets[i].Surfaces_were_generated = FALSE ;
		}
	      break;
	    case LEVEL_EDITOR_POSITION:
	      while (EnterPressed() || SpacePressed() );
	      Level_Editor();
	      // Weiter = TRUE;   /* jp forgot this... ;) */
	      break;
	    case CREDITS_POSITION:
	      while (EnterPressed() || SpacePressed() );
	      Credits_Menu();
	      // Weiter = TRUE;   /* jp forgot this... ;) */
	      break;
	    case QUIT_POSITION:
	      DebugPrintf (2, "\nvoid Options_Menu(void): Quit Requested by user.  Terminating...");
	      Terminate(0);
	      break;
	    default: 
	      break;
	    } 
	  // Weiter=!Weiter;
	}
      if (UpPressed()) 
	{
	  if (MenuPosition > 1) MenuPosition--;
	  MoveMenuPositionSound();
	  while (UpPressed());
	}
      if (DownPressed()) 
	{
	  if ( MenuPosition < QUIT_POSITION ) MenuPosition++;
	  MoveMenuPositionSound();
	  while (DownPressed());
	}
    }

  ClearGraphMem();
  // Since we've faded out the whole scren, it can't hurt
  // to have the top status bar redrawn...
  BannerIsDestroyed=TRUE;
  Me.status=MOBILE;

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
  int Weiter = 0;
  int MenuPosition=1;

#define OPTIONS_MENU_ITEM_POS_X (Block_Width/2)
enum
  { SET_BG_MUSIC_VOLUME=1, 
    SET_SOUND_FX_VOLUME, 
    SET_GAMMA_CORRECTION, 
    SET_FULLSCREEN_FLAG, 
    CW_WIDTH,
    CW_HEIGHT,
    LEAVE_OPTIONS_MENU };

  // This is not some Debug Menu but an optically impressive 
  // menu for the player.  Therefore I suggest we just fade out
  // the game screen a little bit.

  while ( EscapePressed() );

  while (!Weiter)
    {

      SDL_SetClipRect( ne_screen, NULL );
      ClearGraphMem();
      DisplayBanner (NULL, NULL,  BANNER_NO_SDL_UPDATE | BANNER_FORCE_UPDATE );
      Assemble_Combat_Picture ( 0 );
      SDL_SetClipRect( ne_screen, NULL );
      MakeGridOnScreen( NULL );

      // 
      // we highlight the currently selected option with an 
      // influencer to the left before it
      // PutInfluence( FIRST_MENU_ITEM_POS_X , 
      // FIRST_MENU_ITEM_POS_Y + (MenuPosition-1) * (FontHeight(Menu_BFont)) - Block_Width/4 );
      PutInfluence( OPTIONS_MENU_ITEM_POS_X - Block_Width/2, 
		    FIRST_MENU_ITEM_POS_Y + ( MenuPosition - 1.5 ) * (FontHeight( Menu_BFont )) );


      PrintStringFont (ne_screen , Menu_BFont, OPTIONS_MENU_ITEM_POS_X , FIRST_MENU_ITEM_POS_Y+0*FontHeight(Menu_BFont),
		       "Background Music Volume: %1.2f" , GameConfig.Current_BG_Music_Volume );
      PrintStringFont (ne_screen , Menu_BFont, OPTIONS_MENU_ITEM_POS_X , FIRST_MENU_ITEM_POS_Y+1*FontHeight(Menu_BFont), 
		       "Sound Effects Volume: %1.2f", GameConfig.Current_Sound_FX_Volume );
      PrintStringFont (ne_screen , Menu_BFont, OPTIONS_MENU_ITEM_POS_X , FIRST_MENU_ITEM_POS_Y+2*FontHeight(Menu_BFont), 
		       "Gamma Correction: %1.2f", GameConfig.Current_Gamma_Correction );
      PrintStringFont (ne_screen , Menu_BFont, OPTIONS_MENU_ITEM_POS_X , FIRST_MENU_ITEM_POS_Y+3*FontHeight(Menu_BFont), 
		       "Fullscreen Mode: %s", fullscreen_on ? "ON" : "OFF");
      PrintStringFont (ne_screen , Menu_BFont, OPTIONS_MENU_ITEM_POS_X , FIRST_MENU_ITEM_POS_Y+4*FontHeight(Menu_BFont), 
		       "Combat Window Width: %s", User_Rect.x ? "CLASSIC" : "FULL" );
      PrintStringFont (ne_screen , Menu_BFont, OPTIONS_MENU_ITEM_POS_X , FIRST_MENU_ITEM_POS_Y+5*FontHeight(Menu_BFont), 
		       "Combat Window Height: %s", (User_Rect.y - BANNER_HEIGHT ) ? "CLASSIC" : "FULL" );
      //PrintStringFont (ne_screen , Menu_BFont, OPTIONS_MENU_ITEM_POS_X , FIRST_MENU_ITEM_POS_Y+4*FontHeight(Menu_BFont),
      //"Show Framerate: %s", GameConfig.Draw_Framerate? "ON" : "OFF");
      //PrintStringFont (ne_screen , Menu_BFont, OPTIONS_MENU_ITEM_POS_X , FIRST_MENU_ITEM_POS_Y+5*FontHeight(Menu_BFont),
      //"Show Energy: %s", GameConfig.Draw_Energy? "ON" : "OFF");
      PrintStringFont (ne_screen , Menu_BFont, OPTIONS_MENU_ITEM_POS_X , FIRST_MENU_ITEM_POS_Y+6*FontHeight(Menu_BFont), 
		       "Back");

      SDL_Flip( ne_screen );

      // Wait until the user does SOMETHING

      while( !SpacePressed() && !EnterPressed() && !UpPressed()
	     && !DownPressed() && !LeftPressed() && !RightPressed() && !EscapePressed() ) ;

      if ( EscapePressed() )
	{
	  while ( EscapePressed() );
	  Weiter=!Weiter;
	}

      // Some menu options can be controlled by pressing right or left
      // These options are gamma corrections, sound volume and the like
      // Therefore left and right key must be resprected.  This is done here:
      if (RightPressed() || LeftPressed() ) 
	{
	  if (MenuPosition == SET_BG_MUSIC_VOLUME ) 
	    {
	      if (RightPressed()) 
		{
		  while (RightPressed());
		  if ( GameConfig.Current_BG_Music_Volume < 1 ) GameConfig.Current_BG_Music_Volume += 0.05;
		  Set_BG_Music_Volume( GameConfig.Current_BG_Music_Volume );
		}
	      if (LeftPressed()) 
		{
		  while (LeftPressed());
		  if ( GameConfig.Current_BG_Music_Volume > 0 ) GameConfig.Current_BG_Music_Volume -= 0.05;
		  Set_BG_Music_Volume( GameConfig.Current_BG_Music_Volume );
		}
	    }
	  if (MenuPosition == SET_SOUND_FX_VOLUME ) 
	    {
	      if (RightPressed()) 
		{
		  while (RightPressed());
		  if ( GameConfig.Current_Sound_FX_Volume < 1 ) GameConfig.Current_Sound_FX_Volume += 0.05;
		  Set_Sound_FX_Volume( GameConfig.Current_Sound_FX_Volume );
		}
	      if (LeftPressed()) 
		{
		  while (LeftPressed());
		  if ( GameConfig.Current_Sound_FX_Volume > 0 ) GameConfig.Current_Sound_FX_Volume -= 0.05;
		  Set_Sound_FX_Volume( GameConfig.Current_Sound_FX_Volume );
		}
	    }
	  if (MenuPosition == SET_GAMMA_CORRECTION ) 
	    {
	      if (RightPressed()) 
		{
		  while (RightPressed());
		  GameConfig.Current_Gamma_Correction+=0.05;
		  SDL_SetGamma( GameConfig.Current_Gamma_Correction , GameConfig.Current_Gamma_Correction , GameConfig.Current_Gamma_Correction );
		}
	      if (LeftPressed()) 
		{
		  while (LeftPressed());
		  GameConfig.Current_Gamma_Correction-=0.05;
		  SDL_SetGamma( GameConfig.Current_Gamma_Correction , GameConfig.Current_Gamma_Correction , GameConfig.Current_Gamma_Correction );
		}
	    }
	}


      if (EnterPressed() || SpacePressed() ) 
	{
	  MenuItemSelectedSound();
	  switch (MenuPosition) 
	    {
	    case SET_FULLSCREEN_FLAG:
	      while (EnterPressed() || SpacePressed() );
	      SDL_WM_ToggleFullScreen (ne_screen);
	      fullscreen_on = !fullscreen_on;
	      break;

	    case CW_WIDTH:
	      while (EnterPressed() || SpacePressed() );
	      if (User_Rect.x == 0) 
		{
		  User_Rect.x=USERFENSTERPOSX;
		  User_Rect.w=USERFENSTERBREITE;
		  ClearGraphMem();
		  DisplayBanner( NULL , NULL , BANNER_FORCE_UPDATE );
		  SDL_Flip( ne_screen );
		}
	      else
		{
		  User_Rect.x=0;
		  User_Rect.w=640;
		}
	      break;

	    case CW_HEIGHT:
	      while (EnterPressed() || SpacePressed() );
	      if ( User_Rect.y == BANNER_HEIGHT ) 
		{
		  User_Rect.y=USERFENSTERPOSY;
		  User_Rect.h=USERFENSTERHOEHE;
		  ClearGraphMem();
		  DisplayBanner( NULL , NULL , BANNER_FORCE_UPDATE );
		  SDL_Flip( ne_screen );
		}
	      else
		{
		  User_Rect.y = BANNER_HEIGHT;
		  User_Rect.h = 480 - BANNER_HEIGHT;
		}
	      break;

	    case LEAVE_OPTIONS_MENU:
	      while (EnterPressed() || SpacePressed() );
	      Weiter=TRUE;
	      break;
	    default: 
	      break;
	    } 
	  // Weiter=!Weiter;
	}
      if (UpPressed()) 
	{
	  if ( MenuPosition > 1 ) MenuPosition--;
	  MoveMenuPositionSound();
	  while (UpPressed());
	}
      if (DownPressed()) 
	{
	  if ( MenuPosition < LEAVE_OPTIONS_MENU ) MenuPosition++;
	  MoveMenuPositionSound();
	  while (DownPressed());
	}
    }

  ClearGraphMem ();
  DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE );
  InitBars = TRUE;

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

  int Weiter = 0;
  int MenuPosition=1;

#define OPTIONS_MENU_ITEM_POS_X (Block_Width/2)
enum
  { //SET_BG_MUSIC_VOLUME=1, 
    //SET_SOUND_FX_VOLUME, 
    //SET_GAMMA_CORRECTION, 
    SHOW_POSITION=1, 
    SHOW_FRAMERATE, 
    SHOW_ENERGY,
    LEAVE_OPTIONS_MENU };

  // This is not some Debug Menu but an optically impressive 
  // menu for the player.  Therefore I suggest we just fade out
  // the game screen a little bit.

  while ( EscapePressed() );

  while (!Weiter)
    {

      SDL_SetClipRect( ne_screen, NULL );
      ClearGraphMem();
      DisplayBanner (NULL, NULL,  BANNER_NO_SDL_UPDATE | BANNER_FORCE_UPDATE );
      Assemble_Combat_Picture ( 0 );
      SDL_SetClipRect( ne_screen, NULL );
      MakeGridOnScreen( NULL );

      // 
      // we highlight the currently selected option with an 
      // influencer to the left before it
      // PutInfluence( FIRST_MENU_ITEM_POS_X , 
      // FIRST_MENU_ITEM_POS_Y + (MenuPosition-1) * (FontHeight(Menu_BFont)) - Block_Width/4 );
      PutInfluence( OPTIONS_MENU_ITEM_POS_X - Block_Width/2, 
		    FIRST_MENU_ITEM_POS_Y + ( MenuPosition - 1.5 ) * (FontHeight( Menu_BFont )) );


      //PrintStringFont (ne_screen , Menu_BFont, OPTIONS_MENU_ITEM_POS_X , FIRST_MENU_ITEM_POS_Y+0*FontHeight(Menu_BFont)
      //"Background Music Volume: %1.2f" , GameConfig.Current_BG_Music_Volume );
      //PrintStringFont (ne_screen , Menu_BFont, OPTIONS_MENU_ITEM_POS_X , FIRST_MENU_ITEM_POS_Y+1*FontHeight(Menu_BFont),
      //"Sound Effects Volume: %1.2f", GameConfig.Current_Sound_FX_Volume );
      //PrintStringFont (ne_screen , Menu_BFont, OPTIONS_MENU_ITEM_POS_X , FIRST_MENU_ITEM_POS_Y+2*FontHeight(Menu_BFont),
      //"Gamma Correction: %1.2f", GameConfig.Current_Gamma_Correction );
      PrintStringFont (ne_screen , Menu_BFont, OPTIONS_MENU_ITEM_POS_X , FIRST_MENU_ITEM_POS_Y+0*FontHeight(Menu_BFont), 
		       "Show Position: %s", GameConfig.Draw_Position ? "ON" : "OFF");
      PrintStringFont (ne_screen , Menu_BFont, OPTIONS_MENU_ITEM_POS_X , FIRST_MENU_ITEM_POS_Y+1*FontHeight(Menu_BFont), 
		       "Show Framerate: %s", GameConfig.Draw_Framerate? "ON" : "OFF");
      PrintStringFont (ne_screen , Menu_BFont, OPTIONS_MENU_ITEM_POS_X , FIRST_MENU_ITEM_POS_Y+2*FontHeight(Menu_BFont), 
		       "Show Energy: %s", GameConfig.Draw_Energy? "ON" : "OFF");
      PrintStringFont (ne_screen , Menu_BFont, OPTIONS_MENU_ITEM_POS_X , FIRST_MENU_ITEM_POS_Y+3*FontHeight(Menu_BFont), 
		       "Back");

      SDL_Flip( ne_screen );

      // Wait until the user does SOMETHING

      while( !SpacePressed() && !EnterPressed() && !UpPressed()
	     && !DownPressed() && !LeftPressed() && !RightPressed() && !EscapePressed() ) ;

      if ( EscapePressed() )
	{
	  while ( EscapePressed() );
	  Weiter=!Weiter;
	}

      if (EnterPressed() || SpacePressed() ) 
	{
	  MenuItemSelectedSound();
	  switch (MenuPosition) 
	    {
	    case SHOW_POSITION:
	      while (EnterPressed() || SpacePressed() );
	      GameConfig.Draw_Position=!GameConfig.Draw_Position;
	      break;
	    case SHOW_FRAMERATE:
	      while (EnterPressed() || SpacePressed() );
	      GameConfig.Draw_Framerate=!GameConfig.Draw_Framerate;
	      break;
	    case SHOW_ENERGY:
	      while (EnterPressed() || SpacePressed() );
	      GameConfig.Draw_Energy=!GameConfig.Draw_Energy;
	      break;
	    case LEAVE_OPTIONS_MENU:
	      while (EnterPressed() || SpacePressed() );
	      Weiter=TRUE;
	      break;
	    default: 
	      break;
	    } 
	  // Weiter=!Weiter;
	}
      if (UpPressed()) 
	{
	  if ( MenuPosition > 1 ) MenuPosition--;
	  MoveMenuPositionSound();
	  while (UpPressed());
	}
      if (DownPressed()) 
	{
	  if ( MenuPosition < LEAVE_OPTIONS_MENU ) MenuPosition++;
	  MoveMenuPositionSound();
	  while (DownPressed());
	}
    }

  ClearGraphMem ();
  DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE );
  InitBars = TRUE;

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

  int Weiter = 0;
  int MenuPosition=1;

#define OPTIONS_MENU_ITEM_POS_X (Block_Width/2)
enum
  { 
    INFLU_REFRESH_TEXT=1,
    INFLU_BLAST_TEXT,
    ENEMY_HIT_TEXT,
    ENEMY_BUMP_TEXT,
    ENEMY_AIM_TEXT,
    ALL_TEXTS,
    // SHOW_ENERGY,
    LEAVE_DROID_TALK_OPTIONS_MENU };

  // This is not some Debug Menu but an optically impressive 
  // menu for the player.  Therefore I suggest we just fade out
  // the game screen a little bit.

  while ( EscapePressed() );

  while (!Weiter)
    {

      SDL_SetClipRect( ne_screen, NULL );
      ClearGraphMem();
      DisplayBanner (NULL, NULL,  BANNER_NO_SDL_UPDATE | BANNER_FORCE_UPDATE );
      Assemble_Combat_Picture ( 0 );
      SDL_SetClipRect( ne_screen, NULL );
      MakeGridOnScreen( NULL );

      // 
      // we highlight the currently selected option with an 
      // influencer to the left before it
      // PutInfluence( FIRST_MENU_ITEM_POS_X , 
      // FIRST_MENU_ITEM_POS_Y + (MenuPosition-1) * (FontHeight(Menu_BFont)) - Block_Width/4 );
      PutInfluence( OPTIONS_MENU_ITEM_POS_X - Block_Width/2, 
		    FIRST_MENU_ITEM_POS_Y + ( MenuPosition - 1.5 ) * (FontHeight( Menu_BFont )) );


      PrintStringFont (ne_screen , Menu_BFont, OPTIONS_MENU_ITEM_POS_X , FIRST_MENU_ITEM_POS_Y+0*FontHeight(Menu_BFont), 
		       "Influencer Refresh Texts: %s", GameConfig.Influencer_Refresh_Text ? "ON" : "OFF");
      PrintStringFont (ne_screen , Menu_BFont, OPTIONS_MENU_ITEM_POS_X , FIRST_MENU_ITEM_POS_Y+1*FontHeight(Menu_BFont), 
		       "Influencer Blast Texts: %s", GameConfig.Influencer_Blast_Text ? "ON" : "OFF");
      PrintStringFont (ne_screen , Menu_BFont, OPTIONS_MENU_ITEM_POS_X , FIRST_MENU_ITEM_POS_Y+2*FontHeight(Menu_BFont), 
		       "Enemy Hit Texts: %s", GameConfig.Enemy_Hit_Text ? "ON" : "OFF");
      PrintStringFont (ne_screen , Menu_BFont, OPTIONS_MENU_ITEM_POS_X , FIRST_MENU_ITEM_POS_Y+3*FontHeight(Menu_BFont), 
		       "Enemy Bumped Texts: %s", GameConfig.Enemy_Bump_Text ? "ON" : "OFF");
      PrintStringFont (ne_screen , Menu_BFont, OPTIONS_MENU_ITEM_POS_X , FIRST_MENU_ITEM_POS_Y+4*FontHeight(Menu_BFont), 
		       "Enemy Aim Texts: %s", GameConfig.Enemy_Aim_Text ? "ON" : "OFF");
      PrintStringFont (ne_screen , Menu_BFont, OPTIONS_MENU_ITEM_POS_X , FIRST_MENU_ITEM_POS_Y+5*FontHeight(Menu_BFont), 
		       "All in-game Speech: %s", GameConfig.All_Texts_Switch ? "ON" : "OFF");
      PrintStringFont (ne_screen , Menu_BFont, OPTIONS_MENU_ITEM_POS_X , FIRST_MENU_ITEM_POS_Y+6*FontHeight(Menu_BFont), 
		       "Back");

      SDL_Flip( ne_screen );

      // Wait until the user does SOMETHING

      while( !SpacePressed() && !EnterPressed() && !UpPressed()
	     && !DownPressed() && !LeftPressed() && !RightPressed() && !EscapePressed() ) ;

      if ( EscapePressed() )
	{
	  while ( EscapePressed() );
	  Weiter=!Weiter;
	}

      if (EnterPressed() || SpacePressed() ) 
	{
	  MenuItemSelectedSound();
	  switch (MenuPosition) 
	    {
	    case INFLU_REFRESH_TEXT:
	      while (EnterPressed() || SpacePressed() );
	      GameConfig.Influencer_Refresh_Text=!GameConfig.Influencer_Refresh_Text;
	      break;
	    case INFLU_BLAST_TEXT:
	      while (EnterPressed() || SpacePressed() );
	      GameConfig.Influencer_Blast_Text=!GameConfig.Influencer_Blast_Text;
	      break;
	    case ENEMY_HIT_TEXT:
	      while (EnterPressed() || SpacePressed() );
	      GameConfig.Enemy_Hit_Text=!GameConfig.Enemy_Hit_Text;
	      break;
	    case ENEMY_BUMP_TEXT:
	      while (EnterPressed() || SpacePressed() );
	      GameConfig.Enemy_Bump_Text=!GameConfig.Enemy_Bump_Text;
	      break;
	    case ENEMY_AIM_TEXT:
	      while (EnterPressed() || SpacePressed() );
	      GameConfig.Enemy_Aim_Text=!GameConfig.Enemy_Aim_Text;
	      break;
	    case ALL_TEXTS:
	      while (EnterPressed() || SpacePressed() );
	      GameConfig.All_Texts_Switch=!GameConfig.All_Texts_Switch;
	      break;
	    case LEAVE_DROID_TALK_OPTIONS_MENU:
	      while (EnterPressed() || SpacePressed() );
	      Weiter=TRUE;
	      break;
	    default: 
	      break;
	    } 
	  // Weiter=!Weiter;
	}
      if (UpPressed()) 
	{
	  if ( MenuPosition > 1 ) MenuPosition--;
	  MoveMenuPositionSound();
	  while (UpPressed());
	}
      if (DownPressed()) 
	{
	  if ( MenuPosition < LEAVE_DROID_TALK_OPTIONS_MENU ) MenuPosition++;
	  MoveMenuPositionSound();
	  while (DownPressed());
	}
    }

  ClearGraphMem ();
  DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE );
  InitBars = TRUE;

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

  int Weiter = 0;
  int MenuPosition=1;

#define OPTIONS_MENU_ITEM_POS_X (Block_Width/2)
enum
  { GRAPHICS_SOUND_OPTIONS=1, 
    DROID_TALK_OPTIONS,
    ON_SCREEN_DISPLAYS,
    SAVE_OPTIONS, 
    //    TOGGLE_FRAMERATE, 
    // SHOW_ENERGY,
    LEAVE_OPTIONS_MENU };

  // This is not some Debug Menu but an optically impressive 
  // menu for the player.  Therefore I suggest we just fade out
  // the game screen a little bit.

  while ( EscapePressed() );

  while (!Weiter)
    {

      SDL_SetClipRect( ne_screen, NULL );
      ClearGraphMem();
      DisplayBanner (NULL, NULL,  BANNER_NO_SDL_UPDATE | BANNER_FORCE_UPDATE );
      Assemble_Combat_Picture ( 0 );
      SDL_SetClipRect( ne_screen, NULL );
      MakeGridOnScreen( NULL );

      // 
      // we highlight the currently selected option with an 
      // influencer to the left before it
      // PutInfluence( FIRST_MENU_ITEM_POS_X , 
      // FIRST_MENU_ITEM_POS_Y + (MenuPosition-1) * (FontHeight(Menu_BFont)) - Block_Width/4 );
      PutInfluence( OPTIONS_MENU_ITEM_POS_X - Block_Width/2, 
		    FIRST_MENU_ITEM_POS_Y + ( MenuPosition - 1.5 ) * (FontHeight( Menu_BFont )) );


      PrintStringFont (ne_screen , Menu_BFont, OPTIONS_MENU_ITEM_POS_X , FIRST_MENU_ITEM_POS_Y+0*FontHeight(Menu_BFont),
		       "Graphics & Sound" );
      PrintStringFont (ne_screen , Menu_BFont, OPTIONS_MENU_ITEM_POS_X , FIRST_MENU_ITEM_POS_Y+1*FontHeight(Menu_BFont), 
		       "Droid Talk" );
      PrintStringFont (ne_screen , Menu_BFont, OPTIONS_MENU_ITEM_POS_X , FIRST_MENU_ITEM_POS_Y+2*FontHeight(Menu_BFont), 
		       "On-Screen Displays" );
      PrintStringFont (ne_screen , Menu_BFont, OPTIONS_MENU_ITEM_POS_X , FIRST_MENU_ITEM_POS_Y+3*FontHeight(Menu_BFont), 
		       "Save Options");
      //PrintStringFont (ne_screen , Menu_BFont, OPTIONS_MENU_ITEM_POS_X , FIRST_MENU_ITEM_POS_Y+4*FontHeight(Menu_BFont),
      //"Show Framerate: %s", GameConfig.Draw_Framerate? "ON" : "OFF");
      //PrintStringFont (ne_screen , Menu_BFont, OPTIONS_MENU_ITEM_POS_X , FIRST_MENU_ITEM_POS_Y+5*FontHeight(Menu_BFont),
      //"Show Energy: %s", GameConfig.Draw_Energy? "ON" : "OFF");
      PrintStringFont (ne_screen , Menu_BFont, OPTIONS_MENU_ITEM_POS_X , FIRST_MENU_ITEM_POS_Y+4*FontHeight(Menu_BFont), 
		       "Back");

      SDL_Flip( ne_screen );

      // Wait until the user does SOMETHING

      while( !SpacePressed() && !EnterPressed() && !UpPressed()
	     && !DownPressed() && !LeftPressed() && !RightPressed() && !EscapePressed() ) ;

      if ( EscapePressed() )
	{
	  while ( EscapePressed() );
	  Weiter=!Weiter;
	}

      if (EnterPressed() || SpacePressed() ) 
	{
	  MenuItemSelectedSound();
	  switch (MenuPosition) 
	    {
	    case GRAPHICS_SOUND_OPTIONS:
	      while (EnterPressed() || SpacePressed() );
	      GraphicsSound_Options_Menu();
	      break;
	    case DROID_TALK_OPTIONS:
	      while (EnterPressed() || SpacePressed() );
	      Droid_Talk_Options_Menu();
	      break;
	    case ON_SCREEN_DISPLAYS:
	      while (EnterPressed() || SpacePressed() );
	      On_Screen_Display_Options_Menu();
	      break;
	    case SAVE_OPTIONS:
	      while (EnterPressed() || SpacePressed() );
	      break;
	    case LEAVE_OPTIONS_MENU:
	      while (EnterPressed() || SpacePressed() );
	      Weiter=TRUE;
	      break;
	    default: 
	      break;
	    } 
	  // Weiter=!Weiter;
	}
      if (UpPressed()) 
	{
	  if ( MenuPosition > 1 ) MenuPosition--;
	  MoveMenuPositionSound();
	  while (UpPressed());
	}
      if (DownPressed()) 
	{
	  if ( MenuPosition < LEAVE_OPTIONS_MENU ) MenuPosition++;
	  MoveMenuPositionSound();
	  while (DownPressed());
	}
    }

  ClearGraphMem ();
  DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE );
  InitBars = TRUE;

  return;

} // Options_Menu

/*@Function============================================================
@Desc: This function provides the single player menu.  This menu is a 
       submenu of the big EscapeMenu.  Here you can restart a new game,
       see the highscore list, see mission instructions and such 
       things.

@Ret:  none
* $Function----------------------------------------------------------*/
void
Single_Player_Menu (void)
{
  int Weiter = 0;
  int MenuPosition=1;

  #define SINGLE_PLAYER_MENU_ITEM_POS_X (Block_Width*1.2)

  while (!Weiter)
    {

      InitiateMenu();

      // 
      // we highlight the currently selected option with an 
      // influencer to the left before it
      // PutInfluence( FIRST_MENU_ITEM_POS_X , 
      // FIRST_MENU_ITEM_POS_Y + (MenuPosition-1) * (FontHeight(Menu_BFont)) - Block_Width/4 );
      PutInfluence( SINGLE_PLAYER_MENU_ITEM_POS_X - Block_Width/2, 
		    (MenuPosition+3) * (FontHeight(Menu_BFont)) - Block_Width/4 );

      CenteredPutString ( ne_screen ,  4*FontHeight(Menu_BFont),    "New Game");
      CenteredPutString ( ne_screen ,  5*FontHeight(Menu_BFont),    "Show Hiscore List");
      CenteredPutString ( ne_screen ,  6*FontHeight(Menu_BFont),    "Show Mission Log");
      CenteredPutString ( ne_screen ,  7*FontHeight(Menu_BFont),    "Back");

      SDL_Flip( ne_screen );

      // Wait until the user does SOMETHING

      while( !SpacePressed() && !EnterPressed() && !UpPressed() && !DownPressed() && !EscapePressed() )  
	keyboard_update();

      if ( EscapePressed() )
	{
	  while (EscapePressed());
	  Weiter=!Weiter;
	}

      if (EnterPressed() || SpacePressed() ) 
	{
	  MenuItemSelectedSound();
	  while (EnterPressed() || SpacePressed() );
	  switch (MenuPosition) 
	    {

	    case NEW_GAME_POSITION:
	      while (EnterPressed() || SpacePressed() ) ;
	      New_Game_Requested=TRUE;
	      InitNewMissionList ( STANDARD_MISSION );
	      Weiter=!Weiter;
	      break;
	    case SHOW_HISCORE_POSITION: 
	      while (EnterPressed() || SpacePressed() ) ;
	      Show_Highscores();
	      break;
	    case SHOW_MISSION_POSITION:
	      while (EnterPressed() || SpacePressed() ) ;
	      Show_Mission_Log_Menu();
	      break;
	    case BACK_POSITION:
	      while (EnterPressed() || SpacePressed() ) ;
	      Weiter=!Weiter;
	      break;
	    default: 
	      break;
	    }
	}
      if (UpPressed()) 
	{
	  if (MenuPosition > 1) MenuPosition--;
	  MoveMenuPositionSound();
	  while (UpPressed());
	}
      if (DownPressed()) 
	{
	  if ( MenuPosition < BACK_POSITION ) MenuPosition++;
	  MoveMenuPositionSound();
	  while (DownPressed());
	}
    }
} // Single_Player_Menu


/*@Function============================================================
@Desc: This function provides the multi player menu.  It is a submenu
       of the big EscapeMenu.  Instead of connecting to a server or 
       something it simply displayes the nonchalant message, that 
       nothing is implemented yet, but sooner or later it will be.

@Ret:  none
* $Function----------------------------------------------------------*/
void
Multi_Player_Menu (void)
{
  int Weiter = 0;

  enum { NEW_GAME_POSITION=1, SHOW_HISCORE_POSITION=2, SHOW_MISSION_POSITION=3, BACK_POSITION=4 };

  // while( !SpacePressed() && !EnterPressed() ) keyboard_update(); 
  while( SpacePressed() || EnterPressed() ) keyboard_update(); 

  while (!Weiter)
    {

      InitiateMenu();

      CenteredPutString ( ne_screen , 1*FontHeight(Menu_BFont), "MULTI PLAYER" );
      LeftPutString ( ne_screen , 3*FontHeight(Menu_BFont), "We are sorry, but a multi player");
      LeftPutString ( ne_screen , 4*FontHeight(Menu_BFont), "mode has not yet been implemented.");
      LeftPutString ( ne_screen , 5*FontHeight(Menu_BFont), "There are plans to do this, but");
      LeftPutString ( ne_screen , 6*FontHeight(Menu_BFont), "currently it is not a priority.");
      LeftPutString ( ne_screen , 8*FontHeight(Menu_BFont), "If you feel like setting something");
      LeftPutString ( ne_screen , 9*FontHeight(Menu_BFont), "up, please contact the developers.");

      SDL_Flip( ne_screen );

      // Wait until the user does SOMETHING

      if ( EscapePressed() || EnterPressed() || SpacePressed() )
	{
	  Weiter=!Weiter;
	}
    }
  while ( EscapePressed() || EnterPressed() || SpacePressed() );

} // Multi_Player_Menu

/*@Function============================================================
@Desc: This function provides the credits screen.  It is a submenu of
       the big EscapeMenu.  Here you can see who helped developing the
       game, currently jp, rp and bastian.

@Ret:  none
* $Function----------------------------------------------------------*/
void
Credits_Menu (void)
{
  while( SpacePressed() || EnterPressed() ) ; /* wait for key release */

  InitiateMenu();
      
  DisplayImage ( find_file(NE_CREDITS_PIC_FILE,GRAPHICS_DIR,FALSE) );

  CenteredPutString ( ne_screen , 1*FontHeight(Menu_BFont), "CREDITS" );
  LeftPutString ( ne_screen , 3*FontHeight(Menu_BFont), "   PROGRAMMING:");
  RightPutString ( ne_screen , 4*FontHeight(Menu_BFont), "Johannes Prix   ");
  RightPutString ( ne_screen , 5*FontHeight(Menu_BFont), "Reinhard Prix   ");
  LeftPutString ( ne_screen , 7*FontHeight(Menu_BFont), "   ARTWORK:");
  RightPutString ( ne_screen , 8*FontHeight(Menu_BFont), "Bastian Salmela   ");
  RightPutString ( ne_screen , 9*FontHeight(Menu_BFont), "Lanzz   ");

  SDL_Flip( ne_screen );

  // Wait until the user does SOMETHING
  getchar_raw();

} // Credits_Menu

/*@Function============================================================
@Desc: This function provides the details of a mission that has been
       assigned to the player or has been solved perhaps too

@Ret:  none
* $Function----------------------------------------------------------*/
void
Show_Mission_Details ( int MissionNumber )
{
  int Weiter = 0;
  // int i;
  // SDL_Rect* Mission_Window_Pointer=&User_Rect;

  while( SpacePressed() || EnterPressed() ) keyboard_update(); 

  while (!Weiter)
    {

      DisplayImage (find_file (HS_BACKGROUND_FILE, GRAPHICS_DIR, FALSE));
      MakeGridOnScreen ( (SDL_Rect*) & Full_Screen_Rect );
      DisplayBanner( NULL , NULL , BANNER_FORCE_UPDATE );
      //InitiateMenu();

      CenteredPutString ( ne_screen ,  1*FontHeight(Menu_BFont),    "MISSION DETAILS");

      printf_SDL ( ne_screen , User_Rect.x , 3 *FontHeight(Menu_BFont) , "Kill all droids : "  );
      if ( Me.AllMissions[ MissionNumber ].KillAll != (-1) ) printf_SDL( ne_screen , -1 , -1 , "YES" ); 
      else printf_SDL( ne_screen , -1 , -1 , "NO" );

      printf_SDL ( ne_screen , User_Rect.x , 4 *FontHeight(Menu_BFont) , "Kill special : "  );
      if ( Me.AllMissions[ MissionNumber ].KillOne != (-1) ) printf_SDL( ne_screen , -1 , -1 , "YES" ); 
      else printf_SDL( ne_screen , -1 , -1 , "NO" );
      printf_SDL ( ne_screen , -1 , -1 , "   ReachLevel : "  );
      if ( Me.AllMissions[ MissionNumber ].MustReachLevel != (-1) ) printf_SDL( ne_screen , -1 , -1 , "%d\n" , Me.AllMissions[ MissionNumber ].MustReachLevel ); 
      else printf_SDL( ne_screen , -1 , -1 , "NONE\n" );

      printf_SDL ( ne_screen , User_Rect.x , 5 *FontHeight(Menu_BFont) , "Reach X= : "  );
      if ( Me.AllMissions[ MissionNumber ].MustReachPoint.x != (-1) ) printf_SDL( ne_screen , -1 , -1 , "%d" , Me.AllMissions[ MissionNumber ].MustReachPoint.x ); 
      else printf_SDL( ne_screen , -1 , -1 , "NONE" );
      printf_SDL ( ne_screen , -1 , -1 , "   Reach Y= : "  );
      if ( Me.AllMissions[ MissionNumber ].MustReachPoint.y != (-1) ) printf_SDL( ne_screen , -1 , -1 , "%d\n" , Me.AllMissions[ MissionNumber ].MustReachPoint.y );
      else printf_SDL( ne_screen , -1 , -1 , "NONE\n" );

      printf_SDL ( ne_screen , User_Rect.x , 6 *FontHeight(Menu_BFont) , "Live Time : "  );
      if ( Me.AllMissions[ MissionNumber ].MustLiveTime != (-1) ) printf_SDL( ne_screen , -1 , -1 , "%4.0f" , Me.AllMissions[ MissionNumber ].MustLiveTime ); 
      else printf_SDL( ne_screen , -1 , -1 , "NONE" );
      printf_SDL ( ne_screen , User_Rect.x , 7 *FontHeight(Menu_BFont) , "Must be class : "  );
      if ( Me.AllMissions[ MissionNumber ].MustBeClass != (-1) ) printf_SDL( ne_screen , -1 , -1 , "%d\n" , Me.AllMissions[ MissionNumber ].MustBeClass );
      else printf_SDL( ne_screen , -1 , -1 , "NONE\n" );

      printf_SDL ( ne_screen , User_Rect.x , 8 *FontHeight(Menu_BFont) , "Must be type : "  );
      if ( Me.AllMissions[ MissionNumber ].MustBeType != (-1) ) printf_SDL( ne_screen , -1 , -1 , "%d" , Me.AllMissions[ MissionNumber ].MustBeType ); 
      else printf_SDL( ne_screen , -1 , -1 , "NONE" );
      printf_SDL ( ne_screen , User_Rect.x , 9*FontHeight(Menu_BFont) , "Must be special : "  );
      if ( Me.AllMissions[ MissionNumber ].MustBeOne != (-1) ) printf_SDL( ne_screen , -1 , -1 , "YES" );
      else printf_SDL( ne_screen , -1 , -1 , "NO\n" );

      printf_SDL ( ne_screen , User_Rect.x , 10 * FontHeight(Menu_BFont) , "Kill Class : "  );
      if ( Me.AllMissions[ MissionNumber ].KillClass != (-1) ) printf_SDL( ne_screen , -1 , -1 , "%s" , Classname[Me.AllMissions[ MissionNumber ].KillClass] ); 
      else printf_SDL( ne_screen , -1 , -1 , "NONE\n" );

      
      //      LeftPutString ( ne_screen , 3*FontHeight(Menu_BFont), "This is the first mission.  It is");
      //LeftPutString ( ne_screen , 4*FontHeight(Menu_BFont), "identical to the original Paradroid");
      //LeftPutString ( ne_screen , 5*FontHeight(Menu_BFont), "mission from the Commodore C64.");
      //LeftPutString ( ne_screen , 6*FontHeight(Menu_BFont), "So the mission is:");
      //LeftPutString ( ne_screen , 7*FontHeight(Menu_BFont), "Destroy all robots on the ship.");
      //LeftPutString ( ne_screen , 9*FontHeight(Menu_BFont), "If you have some new and good");
      //LeftPutString ( ne_screen ,10*FontHeight(Menu_BFont), "ideas, why not tell us?");

      SDL_Flip( ne_screen );

      while ( (!EscapePressed()) && (!EnterPressed()) && (!SpacePressed()) );
      // Wait until the user does SOMETHING

      if ( EscapePressed() || EnterPressed() || SpacePressed() )
	{
	  Weiter=!Weiter;
	}
    }
  while ( EscapePressed() || EnterPressed() || SpacePressed() );

  

}; // void Show_Mission_Details (void)

/*@Function============================================================
@Desc: This function provides an overview over the missions currently
       assigned to the player

@Ret:  none
* $Function----------------------------------------------------------*/
void
Show_Mission_Log_Menu (void)
{
  int Weiter = 0;
  int i;
  int NoOfActiveMissions;
  int MenuPosition=1;
  int InterLineSpace=60;
  SDL_Rect* Mission_Window_Pointer=&User_Rect;

#define MISSION_NAME_POS_X 230
#define FIRST_MISSION_POS_Y 50


  while( SpacePressed() || EnterPressed() ) keyboard_update(); 

  while (!Weiter)
    {

      DisplayImage (find_file (HS_BACKGROUND_FILE, GRAPHICS_DIR, FALSE));
      MakeGridOnScreen ( (SDL_Rect*) & Full_Screen_Rect );
      DisplayBanner( NULL , NULL , BANNER_FORCE_UPDATE );
      // InitiateMenu();

      // CenteredPutString ( ne_screen ,  1*FontHeight(Menu_BFont),    "MISSION LOG");
      // DisplayText ( " " , 1 , 3*FontHeight( Menu_BFont ) , Mission_Window_Pointer );

      // SetCurrentFont( Menu_BFont );
      SetCurrentFont( Para_BFont );

      DisplayText ( "This is the record of all missions you have been assigned:\n\n" , 
		    0 , FIRST_MISSION_POS_Y - 2 * InterLineSpace , Mission_Window_Pointer );

      NoOfActiveMissions=0;
      for ( i = 0 ; i < MAX_MISSIONS_IN_GAME ; i ++ )
	{

	  if ( Me.AllMissions[i].MissionExistsAtAll != TRUE ) continue;

	  NoOfActiveMissions++;

	  // DisplayText ( "\nMission status: " , -1 , -1 , Mission_Window_Pointer );

	  if ( Me.AllMissions[i].MissionIsComplete == TRUE )
	    {
	      DisplayText ( "SOLVED: " , 0 , FIRST_MISSION_POS_Y + NoOfActiveMissions * InterLineSpace , Mission_Window_Pointer );
	    }
	  else if ( Me.AllMissions[i].MissionWasFailed == TRUE )
	    {
	      DisplayText ( "FAILED: " , 0 , FIRST_MISSION_POS_Y + NoOfActiveMissions * InterLineSpace , Mission_Window_Pointer );
	    }
	  else if ( Me.AllMissions[i].MissionWasAssigned == TRUE ) 
	    {
	      DisplayText ( "ASSIGNED: " , 0 , FIRST_MISSION_POS_Y + NoOfActiveMissions * InterLineSpace , Mission_Window_Pointer );
	    }
	  else
	    {
	      DisplayText ( "UNASSIGNED: " , 0 , FIRST_MISSION_POS_Y +  NoOfActiveMissions * InterLineSpace , Mission_Window_Pointer );
	    }

	  DisplayText ( Me.AllMissions[i].MissionName , MISSION_NAME_POS_X , 
			FIRST_MISSION_POS_Y + NoOfActiveMissions * InterLineSpace ,  Mission_Window_Pointer );

	}

      DisplayText ( "\n\n--- Currently no missions beyond that ---" , 
		    -1 , -1 , Mission_Window_Pointer );

      // Highlight currently selected option with an influencer before it
      PutInfluence( MISSION_NAME_POS_X , FIRST_MISSION_POS_Y + (MenuPosition) * InterLineSpace - Block_Width/4 );

      // If the user pressed up or down, the cursor within
      // the level editor menu has to be moved, which is done here:
      if (UpPressed()) 
	{
	  if (MenuPosition > 1) MenuPosition--;
	  MoveMenuPositionSound();
	  while (UpPressed());
	}
      if (DownPressed()) 
	{
	  if ( MenuPosition < NoOfActiveMissions ) MenuPosition++;
	  MoveMenuPositionSound();
	  while (DownPressed());
	}

      if ( EnterPressed() || SpacePressed() )
	{
	  Show_Mission_Details ( MenuPosition-1 );
	  while ( EnterPressed() || SpacePressed() );
	}




      SDL_Flip( ne_screen );

      if ( EscapePressed() || EnterPressed() || SpacePressed() )
	{
	  Weiter=!Weiter;
	}
    } // end of while loop

  // Wait until the user does SOMETHING
  //while ( (!EscapePressed()) && (!EnterPressed()) && (!SpacePressed()) );


  while ( EscapePressed() || EnterPressed() || SpacePressed() );
} // void Show_Mission_Log_Menu ( void )

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
  char ConnectionText[5000];
  char TextAddition[1000];

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
      strcpy( ConnectionText , "List of connection for this wp:\n" );
      for ( i=0; i<MAX_WP_CONNECTIONS; i++ )
	{
	  if ( CurLevel->AllWaypoints[wp].connections[i] != (-1) )
	    {
	      if ( ( BlockX == CurLevel->AllWaypoints[wp].x ) && ( BlockY == CurLevel->AllWaypoints[wp].y ) )
		{
		  // color = ACTIVE_WP_COLOR ;
		  // else color = HIGHLIGHTCOLOR ; 
		  // printf(" Found a connection!! ");
		  // printf_SDL ( ne_screen  , 100 , 100 , "Waypoint connection to: " );
		  
		  SDL_UnlockSurface( ne_screen );

		  sprintf ( TextAddition , "To: X=%d Y=%d    " , 
			    CurLevel->AllWaypoints[CurLevel->AllWaypoints[wp].connections[i]].x , 
			    CurLevel->AllWaypoints[CurLevel->AllWaypoints[wp].connections[i]].y 
			    );
		  strcat ( ConnectionText , TextAddition );

		  DisplayText ( ConnectionText , User_Rect.x , User_Rect.y , &User_Rect );

		  SDL_LockSurface( ne_screen );

		  DrawLineBetweenTiles( CurLevel->AllWaypoints[wp].x , CurLevel->AllWaypoints[wp].y , 
					CurLevel->AllWaypoints[CurLevel->AllWaypoints[wp].connections[i]].x , 
					CurLevel->AllWaypoints[CurLevel->AllWaypoints[wp].connections[i]].y ,
					color );
		}
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

  enum
    { SAVE_LEVEL_POSITION=1, CHANGE_LEVEL_POSITION, CHANGE_TILE_SET_POSITION, CHANGE_SIZE_X, CHANGE_SIZE_Y, SET_LEVEL_NAME , SET_BACKGROUND_SONG_NAME , SET_LEVEL_COMMENT, QUIT_LEVEL_EDITOR_POSITION };

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
	      if ( Shift_Was_Pressed() )
		CurLevel->map[BlockY][BlockX]=BOX_1;
	      else
		CurLevel->map[BlockY][BlockX]=BLOCK1;
	    }
	  if (Number2Pressed()) 
	    {
	      if ( Shift_Was_Pressed() )
		CurLevel->map[BlockY][BlockX]=BOX_2;
	      else
		CurLevel->map[BlockY][BlockX]=BLOCK2;
	    }
	  if (Number3Pressed()) 
	    {
	      if ( Shift_Was_Pressed() )
		CurLevel->map[BlockY][BlockX]=BOX_3;
	      else
		CurLevel->map[BlockY][BlockX]=BLOCK3;
	    }
	  if (Number4Pressed()) 
	    {
	      if ( Shift_Was_Pressed() )
		CurLevel->map[BlockY][BlockX]=BOX_4;
	      else
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
	      if ( Shift_Was_Pressed() )
		CurLevel->map[BlockY][BlockX]=KONSOLE_U;
	      else if ( Ctrl_Was_Pressed() ) 
		CurLevel->map[BlockY][BlockX]=CODEPANEL_D;
	      else if ( Alt_Was_Pressed() ) 
		CurLevel->map[BlockY][BlockX]=CONVEY_D;
	      else CurLevel->map[BlockY][BlockX]=T_U;
	    }
	  if (KP3Pressed()) 
	    {
	      CurLevel->map[BlockY][BlockX]=ECK_RU;
	    }
	  if (KP4Pressed()) 
	    {
	      if ( Shift_Was_Pressed() )
		CurLevel->map[BlockY][BlockX]=KONSOLE_L;
	      else if ( Ctrl_Was_Pressed() ) 
		CurLevel->map[BlockY][BlockX]=CODEPANEL_L;
	      else if ( Alt_Was_Pressed() ) 
		CurLevel->map[BlockY][BlockX]=CONVEY_R;
	      else CurLevel->map[BlockY][BlockX]=T_L;
	    }
	  if (KP5Pressed()) 
	    {
	      if (!Shift_Was_Pressed())
		CurLevel->map[BlockY][BlockX]=KREUZ;
	      else CurLevel->map[BlockY][BlockX]=VOID;
	    }
	  if (KP6Pressed()) 
	    {
	      if ( Shift_Was_Pressed() )
		CurLevel->map[BlockY][BlockX]=KONSOLE_R;
	      else if ( Ctrl_Was_Pressed() ) 
		CurLevel->map[BlockY][BlockX]=CODEPANEL_R;
	      else if ( Alt_Was_Pressed() ) 
		CurLevel->map[BlockY][BlockX]=CONVEY_L;
	      else CurLevel->map[BlockY][BlockX]=T_R;
	    }
	  if (KP7Pressed()) 
	    {
	      CurLevel->map[BlockY][BlockX]=ECK_LO;
	    }
	  if ( KP8Pressed() ) 
	    {
	      if ( Shift_Was_Pressed() )
		CurLevel->map[BlockY][BlockX]=KONSOLE_O;
	      else if ( Ctrl_Was_Pressed() ) 
		CurLevel->map[BlockY][BlockX]=CODEPANEL_U;
	      else if ( Alt_Was_Pressed() ) 
		CurLevel->map[BlockY][BlockX]=CONVEY_U;
	      else CurLevel->map[BlockY][BlockX]=T_O;
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
	      if ( !Ctrl_Was_Pressed())
		{
		  if (Shift_Was_Pressed())
		    CurLevel->map[BlockY][BlockX]=V_ZUTUERE;	            	      
		  else CurLevel->map[BlockY][BlockX]=H_ZUTUERE;	            	      
		}
	      else
		{
		  if (Shift_Was_Pressed())
		    CurLevel->map[BlockY][BlockX]=LOCKED_V_ZUTUERE;	            	      
		  else CurLevel->map[BlockY][BlockX]=LOCKED_H_ZUTUERE;	            	      
		}
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

	  InitiateMenu();

	  // Highlight currently selected option with an influencer before it
	  PutInfluence( SINGLE_PLAYER_MENU_POINTER_POS_X, (MenuPosition+3) * (FontHeight(Menu_BFont)) - Block_Width/4 );

	  CenteredPutString   ( ne_screen ,  4*FontHeight(Menu_BFont),    
				"Save whole ship to 'Testship.shp'");
	  CenteredPrintString ( ne_screen ,  5*FontHeight(Menu_BFont),    
				"Current: %d.  Level Up/Down" , CurLevel->levelnum );
	  CenteredPutString   ( ne_screen ,  6*FontHeight(Menu_BFont),    
				"Change tile set");
	  CenteredPrintString ( ne_screen ,  7*FontHeight(Menu_BFont),    
				"Levelsize in X: %d.  Shrink/Enlarge" , CurLevel->xlen );
	  CenteredPrintString ( ne_screen ,  8*FontHeight(Menu_BFont),    
				"Levelsize in Y: %d.  Shrink/Enlarge" , CurLevel->ylen );
	  CenteredPrintString ( ne_screen ,  9*FontHeight(Menu_BFont),    
				"Level name: %s" , CurLevel->Levelname );
	  CenteredPrintString ( ne_screen ,  10*FontHeight(Menu_BFont),    
				"Background music file name: %s" , CurLevel->Background_Song_Name );
	  CenteredPrintString ( ne_screen ,  11*FontHeight(Menu_BFont),    
				"Set Level Comment: %s" , CurLevel->Level_Enter_Comment );
	  CenteredPutString   ( ne_screen ,  12*FontHeight(Menu_BFont),    
				"Quit Level Editor");
	  
	  SDL_Flip ( ne_screen );
	  
	  // Wait until the user does SOMETHING
	  
	  while( !SpacePressed() && !EnterPressed() && !UpPressed() && !DownPressed() && !EscapePressed() && !LeftPressed() && !RightPressed())  
	    keyboard_update();
	  
	  if ( EscapePressed() )
	    {
	      while (EscapePressed());
	      Weiter=!Weiter;
	    }
	  
	  if (EnterPressed() || SpacePressed() ) 
	    {
	      MenuItemSelectedSound();
	      while (EnterPressed() || SpacePressed() );
	      switch (MenuPosition) 
		{
		  
		case SAVE_LEVEL_POSITION:
		  while (EnterPressed() || SpacePressed() ) ;
		  SaveShip("Testship");
		  CenteredPutString ( ne_screen ,  11*FontHeight(Menu_BFont),    "Your ship was saved...");
		  SDL_Flip ( ne_screen );
		  while (!EnterPressed() && !SpacePressed() ) ;
		  while (EnterPressed() || SpacePressed() ) ;
		  // Weiter=!Weiter;
		  break;
		case CHANGE_LEVEL_POSITION: 
		  // if ( CurLevel->levelnum ) Teleport ( CurLevel->levelnum-1 , Me.pos.x , Me.pos.y ); 
		  while (EnterPressed() || SpacePressed() ) ;
		  break;
		case CHANGE_TILE_SET_POSITION: 
		  while (EnterPressed() || SpacePressed() ) ;
		  break;
		case SET_LEVEL_NAME:
		  while (EnterPressed() || SpacePressed() ) ;
		  CenteredPutString ( ne_screen ,  12*FontHeight(Menu_BFont), "Please enter new level name:");
		  SDL_Flip( ne_screen );
		  CurLevel->Levelname=GetString( 100 , FALSE );
		  Weiter=!Weiter;
		  break;
		case SET_BACKGROUND_SONG_NAME:
		  while (EnterPressed() || SpacePressed() ) ;
		  CenteredPutString ( ne_screen ,  12*FontHeight(Menu_BFont), "Please enter new music file name:");
		  SDL_Flip( ne_screen );
		  CurLevel->Background_Song_Name=GetString( 100 , FALSE );
		  Weiter=!Weiter;
		  break;
		case SET_LEVEL_COMMENT:
		  while (EnterPressed() || SpacePressed() ) ;
		  CenteredPutString ( ne_screen ,  12*FontHeight(Menu_BFont), "Please enter new level comment:\n");
		  SDL_Flip( ne_screen );
		  MyCursorX=15; MyCursorY=440;
		  CurLevel->Level_Enter_Comment=GetString( 100 , FALSE );
		  Weiter=!Weiter;
		  break;
		case QUIT_LEVEL_EDITOR_POSITION:
		  while (EnterPressed() || SpacePressed() ) ;
		  Weiter=!Weiter;
		  Done=TRUE;
		  SetCombatScaleTo( 1 );
		  break;
		default: 
		  break;

		} // switch
	    } // if EnterPressed or SpacePressed

	  // If the user of the level editor pressed left or right, that should have
	  // an effect IF he/she is a the change level menu point

	  if (LeftPressed() || RightPressed() ) 
	    {
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
	  if (UpPressed()) 
	    {
	      if (MenuPosition > 1) MenuPosition--;
	      MoveMenuPositionSound();
	      while (UpPressed());
	    }
	  if (DownPressed()) 
	    {
	      if ( MenuPosition < QUIT_LEVEL_EDITOR_POSITION ) MenuPosition++;
	      MoveMenuPositionSound();
	      while (DownPressed());
	    }

	}
      
    } // while (!Done)

} // void Level_Editor(void)




#undef _menu_c
