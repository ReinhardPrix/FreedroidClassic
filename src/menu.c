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
void Show_Highscore_Menu (void);
void Show_Mission_Instructions_Menu (void);
void Show_Waypoints(void);
void Level_Editor(void);

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
      //Here comes the standard initializer for all the menus and submenus
      //of the big escape menu.  This prepares the screen, so that we can
      //write on it further down.
      SDL_SetClipRect( ne_screen, NULL );
      ClearGraphMem();
      DisplayBanner (NULL, NULL,  BANNER_NO_SDL_UPDATE | BANNER_FORCE_UPDATE );
      Assemble_Combat_Picture ( 0 );
      SDL_SetClipRect( ne_screen, NULL );
      MakeGridOnScreen();
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
  int LNum, X, Y;
  int i, l;
  int x0, y0;
  Waypoint WpList;      /* pointer on current waypoint-list  */

  // Prevent distortion of framerate by the delay coming from 
  // the time spend in the menu.
  Activate_Conservative_Frame_Computation();

  x0 = User_Rect.x + 100;
  y0 = User_Rect.y + 100;

  Weiter = FALSE;
  while (!Weiter)
    {
      PrintString (ne_screen, x0, y0,
		   "\n\nCurrent position: Level=%d, X=%d, Y=%d\n\n",
		   CurLevel->levelnum, (int)Me.pos.x, (int)Me.pos.y);
      printf (" a. Armageddon (alle Robots sprengen)\n");
      printf (" l. robot list of current level\n");
      printf (" g. complete robot list\n");
      printf (" d. destroy robots on current level\n");
      printf (" t. Teleportation\n");
      printf (" r. change to new robot type\n");
      printf (" i. Invinciblemode: %s\n",
		 InvincibleMode ? "ON" : "OFF");
      printf (" f. full energy\n");
      printf (" b. blink-energy\n");
      printf (" h. Hide invisible map parts: %s\n",
	      HideInvisibleMap ? "ON" : "OFF" );
      printf (" m. Map of Deck xy\n");
      printf (" s. Sound: %s\n", sound_on ? "ON" : "OFF");
      printf (" x. Fullscreen : %s\n", fullscreen_on ? "ON" : "OFF");
      printf (" w. Print current waypoints\n");
      printf ("\n q. RESUME game\n");

      printf ("\n[ Input in Freedroid window please! ]");
      fflush (stdout);

      switch (getchar_raw ())
	{
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
		      printf ("\n --- MORE --- \n");
		      if( getchar_raw () == 'q')
			break;
		    }
		  if (!(l % 20) )  
		    {
		      //		      printf (clearscr);
		      printf ("\n\n NR.\tID\tX\tY\tENERGY\tspeedX\n");
		      printf ("---------------------------------------------\n");
		    }
		  
		  l ++;
		  printf ("%d.\t%s\t%f\t%f\t%d\t%g.\n", i,
			  Druidmap[AllEnemys[i].type].druidname,
			  AllEnemys[i].pos.x,
			  AllEnemys[i].pos.y,
			  (int)AllEnemys[i].energy,
			  AllEnemys[i].speed.x);
		} /* if (enemy on current level)  */
	    } /* for (i<NumEnemys) */

	  printf (" --- END --- \n");
	  getchar_raw ();
	  break;

	case 'g': /* complete robot list of this ship */
	  for (i = 0; i < NumEnemys; i++)
	    {
	      if (i && !(i%20)) 
		{
		  printf (" --- MORE --- \n");
		  if( getchar_raw () == 'q')
		    break;
		}
	      if ( !(i % 20) )
		{
		  //		  printf (clearscr);
		  printf ("\n\nNr.\tLev.\tID\tEnergy\nSpeed.x\n");
		  printf ("------------------------------\n");
		}
	      
	      printf ("%d\t%d\t%s\t%d\t%g\n",
		      i, AllEnemys[i].levelnum,
		      Druidmap[AllEnemys[i].type].druidname,
		      (int)AllEnemys[i].energy,
		      AllEnemys[i].speed.x);
	    } /* for (i<NumEnemys) */

	  printf (" --- END --- \n");
	  getchar_raw ();
	  break;


	case 'd': /* destroy all robots on this level, haha */
	  for (i = 0; i < NumEnemys; i++)
	    {
	      if (AllEnemys[i].levelnum == CurLevel->levelnum)
		AllEnemys[i].energy = -100;
	    }
	  printf ("\n\nAll robots on this deck killed! \n");
	  getchar_raw ();
	  break;


	case 't': /* Teleportation */
	  printf ("\n Enter Levelnummer, X-Pos, Y-Pos: ");
	  input = GetString (40, 1);
	  sscanf (input, "\n%d, %d, %d", &LNum, &X, &Y);
	  free (input);
	  Teleport (LNum, X, Y);
	  printf ("\nThis is your position on level %d.\n", LNum);
	  printf ("\nPress key to continue \n");
	  getchar_raw ();
	  break;

	case 'r': /* change to new robot type */
	  printf ("\nType number of new robot: ");
	  input = GetString (40, 1);
	  for (i = 0; i < Number_Of_Droid_Types ; i++)
	    if (!strcmp (Druidmap[i].druidname, input))
	      break;

	  if ( i == Number_Of_Droid_Types )
	    {
	      printf ("\nUnrecognized robot-type: %s\n", input);
	      getchar_raw ();
	    }
	  else
	    {
	      Me.type = i;
	      Me.energy = Druidmap[Me.type].maxenergy;
	      Me.health = Me.energy;
	      printf ("\nYou are now a %s. Have fun!\n", input);
	      getchar_raw ();
	    }
	  free (input);
	  break;

	case 'i': /* togge Invincible mode */
	  InvincibleMode = !InvincibleMode;
	  break;

	case 'f': /* complete heal */
	  Me.energy = Druidmap[Me.type].maxenergy;
	  Me.health = Me.energy;
	  printf ("\nSie sind wieder gesund! \n");
	  getchar_raw ();
	  break;

	case 'b': /* minimal energy */
	  Me.energy = 1;
	  printf ("\nSie sind jetzt ziemlich schwach! \n");
	  getchar_raw ();
	  break;

	case 'h': /* toggle hide invisible map */
	  HideInvisibleMap = !HideInvisibleMap;
	  break;

	case 's': /* toggle sound on/off */
	  sound_on = !sound_on;
	  break;

	case 'm': /* Show deck map in Concept view */
	  printf ("\nLevelnum: ");
	  input = GetString (40, 1);
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
		  printf (" ---- MORE -----\n");
		  if (getchar_raw () == 'q')
		    break;
		}
	      if ( !(i%20) )
		{
		  //		  printf (clearscr);
		  printf ("\n\nNr.   X   Y      C1  C2  C3  C4 \n");
		  printf ("------------------------------------\n");
		}
	      printf ("%2d   %2d  %2d      %2d  %2d  %2d  %2d\n",
		      i, WpList[i].x, WpList[i].y,
		      WpList[i].connections[0],
		      WpList[i].connections[1],
		      WpList[i].connections[2],
		      WpList[i].connections[3]);

	    } /* for (all waypoints) */
	  printf (" --- END ---\n");
	  getchar_raw ();
	  break;

	case ' ':
	case 'q':
	  Weiter = 1;
	  break;
	} /* switch (getchar_raw()) */
    } /* while (!Weiter) */

  InitBars = TRUE;
  
  keyboard_update (); /* treat all pending keyboard events */
  /* 
   * when changing windows etc, sometimes a key-release event gets 
   * lost, so we have to make sure that CPressed is no longer set
   * or we stay here for ever...
   */
  CurrentlyCPressed = FALSE;

  /* clear terminal (if it's not a terminal: tant pis) */
  //  printf (clearscr);
  printf ("Back in the game!\n");

  /* toggle back to fullscreen, if that's where we came from */
#ifndef NEW_ENGINE
  if (fullscreen_on)
    SDL_WM_ToggleFullScreen (ScaledSurface);
#endif

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
#define FIRST_MIS_SELECT_ITEM_POS_X (1.5*Block_Width)
#define FIRST_MIS_SELECT_ITEM_POS_Y (USERFENSTERPOSY + FontHeight(Menu_BFont))
enum
  { 
    CLASSIC_PARADROID_MISSION_POSITION=1, 
    NEW_MISSION_POSITION
  };

  int Weiter = 0;
  int MenuPosition=1;


  Me.status=MENU;

  DebugPrintf("\nvoid MissionSelectMenu(void): real function call confirmed."); 

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
      
      DisplayImage ( NE_TITLE_PIC_FILE );

      // 
      // we highlight the currently selected option with an 
      // influencer to the left before it
      // PutInfluence( FIRST_MENU_ITEM_POS_X , 
      // FIRST_MENU_ITEM_POS_Y + (MenuPosition-1) * (FontHeight(Menu_BFont)) - Block_Width/4 );
      SetCurrentFont ( Menu_BFont );
      PutInfluence( FIRST_MIS_SELECT_ITEM_POS_X , FIRST_MIS_SELECT_ITEM_POS_Y + ( MenuPosition - 1.5 ) * (FontHeight( Menu_BFont )) );

      CenteredPutString (ne_screen ,  FIRST_MIS_SELECT_ITEM_POS_Y -2*FontHeight(GetCurrentFont()), "Mission Selection Menu");
      CenteredPutString (ne_screen ,  FIRST_MIS_SELECT_ITEM_POS_Y ,    "Classical Paradroid");
      CenteredPutString (ne_screen ,  FIRST_MIS_SELECT_ITEM_POS_Y +1*FontHeight(GetCurrentFont()), "New Episode");

      // SDL_UpdateRect(ne_screen, 0, 0, SCREENBREITE*SCALE_FACTOR, SCREENHOEHE*SCALE_FACTOR);
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

	    case CLASSIC_PARADROID_MISSION_POSITION:
	      while (EnterPressed() || SpacePressed() );
	      InitNewMission ( STANDARD_MISSION );
	      // New_Game_Requested=FALSE;
	      // Single_Player_Menu();
	      Weiter = TRUE;   
	      break;
	    case NEW_MISSION_POSITION:
	      while (EnterPressed() || SpacePressed() );
	      InitNewMission ( NEW_MISSION );
	      Weiter = TRUE;   /* jp forgot this... ;) */
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
	  if ( MenuPosition < NEW_MISSION_POSITION ) MenuPosition++;
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
    LEVEL_EDITOR_POSITION, 
    CREDITS_POSITION,
    QUIT_POSITION
  };

  int Weiter = 0;
  int MenuPosition=1;


  Me.status=MENU;

  DebugPrintf("\nvoid EscapeMenu(void): real function call confirmed."); 

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
      PutInfluence( FIRST_MENU_ITEM_POS_X , 
		    FIRST_MENU_ITEM_POS_Y + ( MenuPosition - 1.5 ) * (FontHeight( Menu_BFont )) );

      CenteredPutString (ne_screen ,  FIRST_MENU_ITEM_POS_Y ,    "Single Player");
      CenteredPutString (ne_screen ,  FIRST_MENU_ITEM_POS_Y +1*FontHeight(GetCurrentFont()),    "Multi Player");
      CenteredPutString (ne_screen ,  FIRST_MENU_ITEM_POS_Y +2*FontHeight(GetCurrentFont()),    "Options");
      CenteredPutString (ne_screen ,  FIRST_MENU_ITEM_POS_Y +3*FontHeight(GetCurrentFont()),    "Level Editor");
      CenteredPutString (ne_screen ,  FIRST_MENU_ITEM_POS_Y +4*FontHeight(GetCurrentFont()),    "Credits");
      CenteredPutString (ne_screen ,  FIRST_MENU_ITEM_POS_Y +5*FontHeight(GetCurrentFont()),    "Quit Game");

      // SDL_UpdateRect(ne_screen, 0, 0, SCREENBREITE*SCALE_FACTOR, SCREENHOEHE*SCALE_FACTOR);
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
	      DebugPrintf("\nvoid Options_Menu(void): Quit Requested by user.  Terminating...");
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
Options_Menu (void)
{

  int Weiter = 0;
  int MenuPosition=1;

#define OPTIONS_MENU_ITEM_POS_X (Block_Width/2)
enum
  { SET_BG_MUSIC_VOLUME=1, 
    SET_SOUND_FX_VOLUME, 
    SET_GAMMA_CORRECTION, 
    SET_FULLSCREEN_FLAG, 
    TOGGLE_FRAMERATE, 
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
      MakeGridOnScreen();

      // 
      // we highlight the currently selected option with an 
      // influencer to the left before it
      // PutInfluence( FIRST_MENU_ITEM_POS_X , 
      // FIRST_MENU_ITEM_POS_Y + (MenuPosition-1) * (FontHeight(Menu_BFont)) - Block_Width/4 );
      PutInfluence( OPTIONS_MENU_ITEM_POS_X - Block_Width/2, 
		    FIRST_MENU_ITEM_POS_Y + ( MenuPosition - 1.5 ) * (FontHeight( Menu_BFont )) );


      PrintStringFont (ne_screen , Menu_BFont, OPTIONS_MENU_ITEM_POS_X , FIRST_MENU_ITEM_POS_Y+0*FontHeight(Menu_BFont),
		       "Background Music Volume: %1.2f" , Current_BG_Music_Volume );
      PrintStringFont (ne_screen , Menu_BFont, OPTIONS_MENU_ITEM_POS_X , FIRST_MENU_ITEM_POS_Y+1*FontHeight(Menu_BFont), 
		       "Sound Effects Volume: %1.2f", Current_Sound_FX_Volume );
      PrintStringFont (ne_screen , Menu_BFont, OPTIONS_MENU_ITEM_POS_X , FIRST_MENU_ITEM_POS_Y+2*FontHeight(Menu_BFont), 
		       "Gamma Correction: %1.2f", Current_Gamma_Correction );
      PrintStringFont (ne_screen , Menu_BFont, OPTIONS_MENU_ITEM_POS_X , FIRST_MENU_ITEM_POS_Y+3*FontHeight(Menu_BFont), 
		       "Fullscreen Mode: %s", fullscreen_on ? "ON" : "OFF");
      PrintStringFont (ne_screen , Menu_BFont, OPTIONS_MENU_ITEM_POS_X , FIRST_MENU_ITEM_POS_Y+4*FontHeight(Menu_BFont), 
		       "Show Framerate: %s", Draw_Framerate? "ON" : "OFF");
      PrintStringFont (ne_screen , Menu_BFont, OPTIONS_MENU_ITEM_POS_X , FIRST_MENU_ITEM_POS_Y+5*FontHeight(Menu_BFont), 
		       "Show Energy: %s", Draw_Energy? "ON" : "OFF");
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
		  if ( Current_BG_Music_Volume < 1 ) Current_BG_Music_Volume += 0.05;
		  Set_BG_Music_Volume( Current_BG_Music_Volume );
		}
	      if (LeftPressed()) 
		{
		  while (LeftPressed());
		  if ( Current_BG_Music_Volume > 0 ) Current_BG_Music_Volume -= 0.05;
		  Set_BG_Music_Volume( Current_BG_Music_Volume );
		}
	    }
	  if (MenuPosition == SET_SOUND_FX_VOLUME ) 
	    {
	      if (RightPressed()) 
		{
		  while (RightPressed());
		  if ( Current_Sound_FX_Volume < 1 ) Current_Sound_FX_Volume += 0.05;
		  Set_Sound_FX_Volume( Current_Sound_FX_Volume );
		}
	      if (LeftPressed()) 
		{
		  while (LeftPressed());
		  if ( Current_Sound_FX_Volume > 0 ) Current_Sound_FX_Volume -= 0.05;
		  Set_Sound_FX_Volume( Current_Sound_FX_Volume );
		}
	    }
	  if (MenuPosition == SET_GAMMA_CORRECTION ) 
	    {
	      if (RightPressed()) 
		{
		  while (RightPressed());
		  Current_Gamma_Correction+=0.05;
		  SDL_SetGamma( Current_Gamma_Correction , Current_Gamma_Correction , Current_Gamma_Correction );
		}
	      if (LeftPressed()) 
		{
		  while (LeftPressed());
		  Current_Gamma_Correction-=0.05;
		  SDL_SetGamma( Current_Gamma_Correction , Current_Gamma_Correction , Current_Gamma_Correction );
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
	    case TOGGLE_FRAMERATE:
	      while (EnterPressed() || SpacePressed() );
	      Draw_Framerate=!Draw_Framerate;
	      break;
	    case SHOW_ENERGY:
	      while (EnterPressed() || SpacePressed() );
	      Draw_Energy=!Draw_Energy;
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
  Update_SDL_Screen();
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
      CenteredPutString ( ne_screen ,  6*FontHeight(Menu_BFont),    "Show Mission Instructions");
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
	      InitNewMission( STANDARD_MISSION );
	      Weiter=!Weiter;
	      break;
	    case SHOW_HISCORE_POSITION: 
	      while (EnterPressed() || SpacePressed() ) ;
	      Show_Highscore_Menu();
	      break;
	    case SHOW_MISSION_POSITION:
	      while (EnterPressed() || SpacePressed() ) ;
	      Show_Mission_Instructions_Menu();
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
@Desc: This function displayes the high scores of the single player
       game.  This function is actually a submenu of the big 
       EscapeMenu.  The function is currently disabled, since rp is
       rewriting the high score administration routines.

@Ret:  none
* $Function----------------------------------------------------------*/
void
Show_Highscore_Menu (void)
{

  /*

  int Weiter = 0;

  enum { NEW_GAME_POSITION=1, SHOW_HISCORE_POSITION=2, SHOW_MISSION_POSITION=3, BACK_POSITION=4 };

  // while( !SpacePressed() && !EnterPressed() ) keyboard_update(); 
  while( SpacePressed() || EnterPressed() ) keyboard_update(); 

  while (!Weiter)
    {

      InitiateMenu();

      CenteredPutString (ScaledSurface, 1*FontHeight(Menu_BFont), "Highscore list:" );

      PrintStringFont (ScaledSurface , Menu_BFont, 2*Block_Width , 4*FontHeight(Menu_BFont),    
		       "Highest score: %10s : %6.2f" , HighestName, HighestScoreOfDay );

      PrintStringFont (ScaledSurface , Menu_BFont, 2*Block_Width , 5*FontHeight(Menu_BFont),
		       "Great score: %10s : %6.2f" , GreatScoreName,  GreatScore);

      PrintStringFont (ScaledSurface , Menu_BFont, 2*Block_Width , 6*FontHeight(Menu_BFont),
		       " Lowest Score:  %10s : %6.2f", LowestName,   LowestScoreOfDay);

      // LeftPutString (ScaledSurface , 9*FontHeight(Menu_BFont), "We are looking forward so seeing");
      // LeftPutString (ScaledSurface ,10*FontHeight(Menu_BFont), "new missions and levels from you!");

      SDL_Flip ( ne_screen );

      // Wait until the user does SOMETHING

      if ( EscapePressed() || EnterPressed() || SpacePressed() )
	{
	  Weiter=!Weiter;
	}
    }

  while ( EscapePressed() || EnterPressed() || SpacePressed() );

  ClearGraphMem ( );
  DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE );
  InitBars = TRUE;

  */ 

  return;

} // Show_Highscore_Menu

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
  int Weiter = 0;

  // while( !SpacePressed() && !EnterPressed() ) keyboard_update(); 
  while( SpacePressed() || EnterPressed() ) keyboard_update(); 

  while (!Weiter)
    {

      InitiateMenu();
      
      DisplayImage ( NE_CREDITS_PIC_FILE );

      CenteredPutString ( ne_screen , 1*FontHeight(Menu_BFont), "CREDITS" );
      LeftPutString ( ne_screen , 3*FontHeight(Menu_BFont), "PROGRAMMING:");
      LeftPutString ( ne_screen , 4*FontHeight(Menu_BFont), "Johannes Prix and Reinhard Prix");
      LeftPutString ( ne_screen , 5*FontHeight(Menu_BFont), "");
      LeftPutString ( ne_screen , 6*FontHeight(Menu_BFont), "ARTWORK:");
      LeftPutString ( ne_screen , 7*FontHeight(Menu_BFont), "Bastian Salmela");
      LeftPutString ( ne_screen , 8*FontHeight(Menu_BFont), "");

      SDL_Flip( ne_screen );

      // Wait until the user does SOMETHING

      if ( EscapePressed() || EnterPressed() || SpacePressed() )
	{
	  Weiter=!Weiter;
	}
    }
  while ( EscapePressed() || EnterPressed() || SpacePressed() );

} // Credits_Menu

/*@Function============================================================
@Desc: This function provides the mission instructions.  It is a 
       submenu of the single player menu.

@Ret:  none
* $Function----------------------------------------------------------*/
void
Show_Mission_Instructions_Menu (void)
{
  int Weiter = 0;

  enum { NEW_GAME_POSITION=1, SHOW_HISCORE_POSITION=2, SHOW_MISSION_POSITION=3, BACK_POSITION=4 };

  while( SpacePressed() || EnterPressed() ) keyboard_update(); 

  while (!Weiter)
    {

      InitiateMenu();

      CenteredPutString ( ne_screen ,  1*FontHeight(Menu_BFont),    "MISSION INSTRUCTIONS");
      LeftPutString ( ne_screen , 3*FontHeight(Menu_BFont), "This is the first mission.  It is");
      LeftPutString ( ne_screen , 4*FontHeight(Menu_BFont), "identical to the original Paradroid");
      LeftPutString ( ne_screen , 5*FontHeight(Menu_BFont), "mission from the Commodore C64.");
      LeftPutString ( ne_screen , 6*FontHeight(Menu_BFont), "So the mission is:");
      LeftPutString ( ne_screen , 7*FontHeight(Menu_BFont), "Destroy all robots on the ship.");
      LeftPutString ( ne_screen , 9*FontHeight(Menu_BFont), "If you have some new and good");
      LeftPutString ( ne_screen ,10*FontHeight(Menu_BFont), "ideas, why not tell us?");

      SDL_Flip( ne_screen );

      // Wait until the user does SOMETHING

      if ( EscapePressed() || EnterPressed() || SpacePressed() )
	{
	  Weiter=!Weiter;
	}
    }
  while ( EscapePressed() || EnterPressed() || SpacePressed() );

} // ShowMissionInstructionsMenu

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
  int OriginWaypoint = (-1);

  enum
    { SAVE_LEVEL_POSITION=1, SET_LEVEL_NAME_POSITION=2, BACK_TO_LEVEL_EDITING=3, QUIT_LEVEL_EDITOR_POSITION=4 };

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
	  //If the person using the level editor decides he/she wants a different
	  //scale for the editing process, he/she may say so by using the O/I keys.
	  //
	  if ( OPressed () )
	    {
	      if (CurrentCombatScaleFactor > 0.5 )
		CurrentCombatScaleFactor -= 0.5;
	      SetCombatScaleTo (CurrentCombatScaleFactor);
	      while (OPressed());
	    }
	  if ( IPressed () )
	    {
	      CurrentCombatScaleFactor += 0.5;
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

	  InitiateMenu();

	  // Highlight currently selected option with an influencer before it
	  PutInfluence( SINGLE_PLAYER_MENU_POINTER_POS_X, (MenuPosition+3) * (FontHeight(Menu_BFont)) - Block_Width/4 );

	  CenteredPutString   ( ne_screen ,  4*FontHeight(Menu_BFont),    
				"Save Level:");
	  CenteredPrintString ( ne_screen ,  5*FontHeight(Menu_BFont),    
				"Current: %d.  Level Up/Down" , CurLevel->levelnum );
	  CenteredPutString   ( ne_screen ,  6*FontHeight(Menu_BFont),    
				"Back to Level editing");
	  CenteredPutString   ( ne_screen ,  7*FontHeight(Menu_BFont),    
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
		  CenteredPutString ( ne_screen ,  9*FontHeight(Menu_BFont),    "Your ship was saved...");
		  SDL_Flip ( ne_screen );
		  while (!EnterPressed() && !SpacePressed() ) ;
		  while (EnterPressed() || SpacePressed() ) ;
		  // Weiter=!Weiter;
		  break;
		case SET_LEVEL_NAME_POSITION: 
		  while (EnterPressed() || SpacePressed() ) ;
		  break;
		case BACK_TO_LEVEL_EDITING:
		  while (EnterPressed() || SpacePressed() ) ;
		  Weiter=!Weiter;
		  break;
		case QUIT_LEVEL_EDITOR_POSITION:
		  while (EnterPressed() || SpacePressed() ) ;
		  Weiter=!Weiter;
		  Done=TRUE;
		  break;
		default: 
		  break;
		}
	    }

	  // If the user of the level editor pressed left or right, that should have
	  // an effect IF he/she is a the change level menu point
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
	      if (MenuPosition < 4) MenuPosition++;
	      MoveMenuPositionSound();
	      while (DownPressed());
	    }

	}
      
    } // while (!Done)

} // void Level_Editor(void)



#undef _menu_c
