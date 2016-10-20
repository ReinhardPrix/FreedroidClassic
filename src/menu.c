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
 * Desc: all menu functions and their subfunctions for Freedroid
 *
 *----------------------------------------------------------------------*/

#define _menu_c

// ----- includes --------------------
#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"
#include "map.h"

// ----- global variables --------------------
extern int key_cmds[CMD_LAST][3];
extern char *cmd_strings[CMD_LAST];
extern char *keystr[INPUT_LAST];


SDL_Surface *Menu_Background = NULL;
int fheight;  // font height of Menu-font

// ----- Macros --------------------
#define HANDLE_MOVEMENT_KEYS \
  if (MenuUpR()) { \
     key = TRUE; \
     if (MenuPosition > 1) { MenuPosition--; } \
     else { MenuPosition = (END-1); } \
     ReleaseKey (SDLK_RIGHT); ReleaseKey (SDLK_LEFT); \
     MoveMenuPositionSound(); \
  } \
  if (MenuDownR()) { \
     key = TRUE; \
     if ( MenuPosition < (END-1) ) { MenuPosition++; } \
     else { MenuPosition = 1; } \
     ReleaseKey (SDLK_RIGHT); \
     ReleaseKey (SDLK_LEFT); \
     MoveMenuPositionSound(); \
  }

// ----- local prototypes ----------
void Credits_Menu (void);
void Options_Menu (void);

bool LevelEditMenu (void);
void GraphicsSound_Options_Menu (void);
void On_Screen_Display_Options_Menu (void);
void Key_Config_Menu (void);
void Display_Key_Config (int selx, int sely);

/*@Function============================================================
@Desc: This function prepares the screen for the big Escape menu and
       its submenus.  This means usual content of the screen, i.e. the
       combat screen and top status bar, is "faded out", the rest of
       the screen is cleared.  This function resolves some redundance
       that occured since there are so many submenus needing this.

@Ret: none
* $Function----------------------------------------------------------*/
void
InitiateMenu (bool with_droids)
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
  if (with_droids)
    Assemble_Combat_Picture (0);
  else
    Assemble_Combat_Picture (ONLY_SHOW_MAP);

  SDL_SetClipRect( ne_screen, NULL );
  MakeGridOnScreen( NULL );

  if (Menu_Background) SDL_FreeSurface (Menu_Background);
  Menu_Background = SDL_DisplayFormat (ne_screen);  // keep a global copy of background

  ResetMouseWheel ();

  SDL_ShowCursor (SDL_DISABLE);  // deactivate mouse-cursor in menus
  SetCurrentFont ( Menu_BFont );
  fheight = FontHeight (GetCurrentFont()) + 2;

  return;

} // void InitiateMenu (bool with_droids)


// ----------------------------------------------------------------------
void
QuitGameMenu (void)
{
#ifdef ANDROID
  Terminate (OK);
#endif

  InitiateMenu (TRUE);

#ifdef GCW0
  PutString (ne_screen, User_Rect.x + User_Rect.w/3,
	      User_Rect.y + User_Rect.h/2, "Press A to quit");
#else
  PutString (ne_screen, User_Rect.x + User_Rect.w/10,
	      User_Rect.y + User_Rect.h/2, "Do you really want to quit? (y/n) ");
#endif
  SDL_Flip (ne_screen);
#ifdef GCW0
  while ( (!Gcw0AnyButtonPressed()) ) SDL_Delay(1);
  if ( (Gcw0APressed()) ) {
    while ( (!Gcw0AnyButtonPressedR()) ) SDL_Delay(1); // In case FirePressed && !Gcw0APressed() -> would cause a loop otherwise in the menu...
    Terminate (OK);
  }
#else
  while ( (!KeyIsPressed('n')) && (!KeyIsPressed('y')) ) SDL_Delay(1);
  if (KeyIsPressed('y'))
    Terminate (OK);
#endif

  return;

} // QuitGameMenu()



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
      START = 0,
#ifndef ANDROID
      BACK2GAME,
      POS_GRAPHICS_SOUND_OPTIONS,
#endif
      POS_LEGACY_OPTIONS,
      POS_ON_SCREEN_DISPLAYS,
#if !defined ANDROID && !defined GCW0 // Haven't looked at level editor keys, if they are feasibly re-defined for GCW0 it could be enabled...
      POS_LEVEL_EDITOR,
#endif
      POS_HIGHSCORES,
      POS_CREDITS,
#ifndef ANDROID
      POS_KEYCONFIG,
#endif
      POS_QUIT,
      END
    };

  static const struct {
    const char *const name;	/**< coordinate name */
  const double scale;		/**< multiplicative scaling factor of the coordinate */
  const char *const help;	/**< help string explaining the coordinate's meaning and units */
} DopplerCoordinates[DOPPLERCOORD_LAST] = {

  [DOPPLERCOORD_FREQ]     = {"freq",    SCALE_T,          "Frequency [Units: Hz]."},
  [DOPPLERCOORD_F1DOT]    = {"f1dot",   POW2(SCALE_T),    "First spindown [Units: Hz/s]."},
  [DOPPLERCOORD_F2DOT]    = {"f2dot",   POW3(SCALE_T),    "Second spindown [Units: Hz/s^2]."},
  [DOPPLERCOORD_F3DOT]    = {"f3dot",   POW4(SCALE_T),    "Third spindown [Units: Hz/s^3]."},
    

  bool key=FALSE;
  bool finished = FALSE;

  int pos=0;
  int MenuPosition=1;

  InitiateMenu(TRUE);

  while (!finished)
    {
      key = FALSE;
      SDL_BlitSurface (Menu_Background, NULL, ne_screen, NULL);

      PutInfluence (Menu_Rect.x, Menu_Rect.y + (MenuPosition-1.5)*fheight);

      pos = 0;

#ifndef ANDROID
      PutString (ne_screen, OptionsMenu_Rect.x,Menu_Rect.y+(pos++)*fheight, "Back to Game");
      PutString (ne_screen, OptionsMenu_Rect.x,Menu_Rect.y+(pos++)*fheight, "Graphics & Sound" );
#endif
      PutString (ne_screen, OptionsMenu_Rect.x,Menu_Rect.y+(pos++)*fheight, "Legacy Options");
      PutString (ne_screen, OptionsMenu_Rect.x,Menu_Rect.y+(pos++)*fheight, "On-Screen Displays" );
#if !defined ANDROID && !defined GCW0
      PutString (ne_screen, OptionsMenu_Rect.x,Menu_Rect.y+(pos++)*fheight, "Level Editor");
#endif
      PutString (ne_screen, OptionsMenu_Rect.x,Menu_Rect.y+(pos++)*fheight, "Highscores");
      PutString (ne_screen, OptionsMenu_Rect.x,Menu_Rect.y+(pos++)*fheight, "Credits");
#ifndef ANDROID
      PutString (ne_screen, OptionsMenu_Rect.x,Menu_Rect.y+(pos++)*fheight, "Configure Keys");
#endif
      PutString (ne_screen, OptionsMenu_Rect.x,Menu_Rect.y+(pos++)*fheight, "Quit Game");

      SDL_Flip( ne_screen );

      while (!key)
	{
	  SDL_Delay (1);

	  if (EscapePressedR() )
	    {
	      finished = TRUE;
	      key = TRUE;
	    }


	  if (MenuChooseR())
	    {
	      MenuItemSelectedSound();
	      key = TRUE;
	      switch (MenuPosition)
		{
#ifndef ANDROID
		case BACK2GAME:
		  finished = TRUE;
		  break;
		case POS_GRAPHICS_SOUND_OPTIONS:
		  GraphicsSound_Options_Menu();
		  break;
#endif
		case POS_ON_SCREEN_DISPLAYS:
		  On_Screen_Display_Options_Menu();
		  break;
		case POS_LEGACY_OPTIONS:
		  Options_Menu();
		  break;
#if !defined ANDROID && !defined GCW0
                case POS_LEVEL_EDITOR:
                  LevelEditor();
                  finished = TRUE;
                  break;
#endif
		case POS_HIGHSCORES:
		  ShowHighscores();
		  break;
		case POS_CREDITS:
		  Credits_Menu();
		  break;
#ifndef ANDROID
                case POS_KEYCONFIG:
                  Key_Config_Menu();
                  break;
#endif
		case POS_QUIT:
		  QuitGameMenu ();
		  break;
		default:
		  break;
		}
	    }

          HANDLE_MOVEMENT_KEYS();

	} // while !key


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
Options_Menu (void)
{
  int MenuPosition=1;
  bool finished = FALSE;
  bool key = FALSE;
  int pos;
  bool reload_theme = FALSE;
  bool toggle_window = FALSE;
  char theme_string[40];
  char window_string[40];
  int new_tnum = AllThemes.cur_tnum;

enum
  {
    POS_RESET=1,
    POS_FULL_WINDOW,
    POS_SET_THEME,
    POS_DROID_TALK,
    POS_SHOW_DECALS,
    POS_MAP_VISIBLE,
#ifndef ANDROID
    POS_TAKEOVER_IS_ACTIVATE,
    POS_FIRE_HOLD_TAKEOVER,
    POS_BACK
#endif
  };


  while (!finished)
    {
      SDL_BlitSurface (Menu_Background, NULL, ne_screen, NULL);
      key = FALSE;

      sprintf (theme_string, "Graphics theme: %s", AllThemes.theme_name[new_tnum]);

      strcpy (window_string, "Combat window: ");
      if (GameConfig.FullUserRect) strcat (window_string, "Full");
      else strcat (window_string, "Classic");



      PutInfluence( Menu_Rect.x,
		    Menu_Rect.y + ( MenuPosition - 1.5 ) *fheight);
      pos = 0;

      PutString (ne_screen, OptionsMenu_Rect.x, Menu_Rect.y+(pos++)*fheight,
		 "Set to Strictly Classic");

      PutString (ne_screen, OptionsMenu_Rect.x, Menu_Rect.y+(pos++)*fheight, window_string);
      PutString (ne_screen, OptionsMenu_Rect.x, Menu_Rect.y+(pos++)*fheight, theme_string);

      PrintString (ne_screen, OptionsMenu_Rect.x, Menu_Rect.y+(pos++)*fheight,
		   "Droid Talk : %s", GameConfig.Droid_Talk ? "ON" : "OFF");
      PrintString (ne_screen, OptionsMenu_Rect.x, Menu_Rect.y+(pos++)*fheight,
		   "Show Decals : %s", GameConfig.ShowDecals ? "ON" : "OFF");

      PrintString (ne_screen, OptionsMenu_Rect.x, Menu_Rect.y+(pos++)*fheight,
		   "All Map Visible: %s", GameConfig.AllMapVisible ? "ON" : "OFF");
#ifndef ANDROID
      PrintString (ne_screen, OptionsMenu_Rect.x, Menu_Rect.y+(pos++)*fheight,
		   "Transfer = Activate: %s", GameConfig.TakeoverActivates ? "YES":"NO" );

      PrintString (ne_screen, OptionsMenu_Rect.x, Menu_Rect.y+(pos++)*fheight,
		   "Hold Fire to Transfer: %s", GameConfig.FireHoldTakeover ? "YES":"NO" );

      PrintString (ne_screen, OptionsMenu_Rect.x, Menu_Rect.y+(pos++)*fheight,
		   "Back");
#endif
      SDL_Flip( ne_screen );

      while (!key)
	{

	  SDL_Delay(1);

	  if ( EscapePressedR () )
	    {
	      finished = TRUE;
	      key = TRUE;
	      if (MenuPosition == POS_SET_THEME)
		reload_theme = TRUE;
	    }

	  if (MenuChooseR())
	    {
	      MenuItemSelectedSound();
	      key = TRUE;
	      switch (MenuPosition)
		{
		case POS_RESET:
		  GameConfig.Droid_Talk = FALSE;
		  GameConfig.ShowDecals = FALSE;
		  GameConfig.TakeoverActivates = TRUE;
		  GameConfig.FireHoldTakeover = TRUE;
		  GameConfig.AllMapVisible = TRUE;
		  GameConfig.FullUserRect = FALSE;
		  Copy_Rect (Classic_User_Rect, User_Rect);
		  new_tnum = classic_theme_index;
		  reload_theme = TRUE;
		  break;


		case POS_FULL_WINDOW:
		  toggle_window = TRUE;
		  break;
		case POS_SET_THEME:
		  if (!MouseLeftPressed() )MoveMenuPositionSound();
		  new_tnum--;
		  if (new_tnum < 0)
		    new_tnum = AllThemes.num_themes - 1;
		  reload_theme = TRUE;
		  break;

		case POS_DROID_TALK:
		  GameConfig.Droid_Talk = !GameConfig.Droid_Talk;
		  break;
		case POS_SHOW_DECALS:
		  GameConfig.ShowDecals = !GameConfig.ShowDecals;
		  InitiateMenu (TRUE);
		  break;
		case POS_MAP_VISIBLE:
		  GameConfig.AllMapVisible = !GameConfig.AllMapVisible;
		  InitiateMenu (TRUE);
		  break;
#ifndef ANDROID
		case POS_TAKEOVER_IS_ACTIVATE:
		  GameConfig.TakeoverActivates = !GameConfig.TakeoverActivates;
		  break;
		case POS_FIRE_HOLD_TAKEOVER:
		  GameConfig.FireHoldTakeover = !GameConfig.FireHoldTakeover;
		  break;
		case POS_BACK:
		  finished = TRUE;
		  break;
#endif
		default:
		  break;
		}
	    } // if FirePressed

	  if (MenuUpR())
	    {
	      if ( MenuPosition > 1 ) MenuPosition--;
	      else MenuPosition = POS_BACK;
	      MoveMenuPositionSound();
	      key = TRUE;
	      ReleaseKey (SDLK_RIGHT); // clear any r-l movement
	      ReleaseKey (SDLK_LEFT);
	    }
	  if (MenuDownR())
	    {
	      if ( MenuPosition < POS_BACK ) MenuPosition++;
	      else MenuPosition = 1;
	      MoveMenuPositionSound();
	      key = TRUE;
	      ReleaseKey (SDLK_RIGHT); // clear any r-l movement
	      ReleaseKey (SDLK_LEFT);
	    }


	  if (MenuLeftR() )
	    {
	      switch (MenuPosition)
		{
		case POS_SET_THEME:
		  key = TRUE;
		  if (!MouseLeftPressed() ) MoveMenuPositionSound();

		  new_tnum--;
		  if (new_tnum < 0)
		    new_tnum = AllThemes.num_themes - 1;
		  reload_theme = TRUE;
		  break;

		default:
		  break;
		}
	    }
	  if (MenuRightR() )
	    {
	      switch (MenuPosition)
		{
		case POS_SET_THEME:
		  key = TRUE;
		  MenuItemSelectedSound();

		  new_tnum++;
		  if (new_tnum > AllThemes.num_themes -1)
		    new_tnum = 0;
		  reload_theme = TRUE;
		  break;

		default:
		  break;
		}
	    }


	} // while !key


      if (reload_theme)
	{
	  if (new_tnum != AllThemes.cur_tnum)
	    {
	      AllThemes.cur_tnum = new_tnum;
	      strcpy (GameConfig.Theme_Name, AllThemes.theme_name[AllThemes.cur_tnum]);
	      InitPictures();
	    }
	  reload_theme = FALSE;
	  InitiateMenu (TRUE);
	}
      if (toggle_window)
	{
	  GameConfig.FullUserRect = ! GameConfig.FullUserRect;
	  if (GameConfig.FullUserRect)
	    Copy_Rect (Full_User_Rect, User_Rect);
	  else
	    Copy_Rect (Classic_User_Rect, User_Rect);

	  InitiateMenu (TRUE);
	  toggle_window = FALSE;
	}



    } // while !finished

  return;

} // Options_Menu



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
  bool key = FALSE;
  int pos;


enum
  { SET_BG_MUSIC_VOLUME=1,
    SET_SOUND_FX_VOLUME,
    SET_GAMMA_CORRECTION,
#ifndef GCW0
    SET_FULLSCREEN_FLAG,
#endif
    SET_HOG_CPU,
    BACK
  };



  while (!finished)
    {
      key = FALSE;

      pos = 0;

      SDL_BlitSurface (Menu_Background, NULL, ne_screen, NULL);

      PutInfluence (Menu_Rect.x, Menu_Rect.y+ (MenuPosition-1.5)*fheight);

      PrintString (ne_screen, OptionsMenu_Rect.x, Menu_Rect.y+(pos++)*fheight,
		   "Background Music: %1.2f" , GameConfig.Current_BG_Music_Volume );
      PrintString (ne_screen, OptionsMenu_Rect.x, Menu_Rect.y+(pos++)*fheight,
		   "Sound Effects: %1.2f", GameConfig.Current_Sound_FX_Volume );
      PrintString (ne_screen, OptionsMenu_Rect.x, Menu_Rect.y+(pos++)*fheight,
		   "Gamma: %1.2f", GameConfig.Current_Gamma_Correction );
#ifndef GCW0
      PrintString (ne_screen, OptionsMenu_Rect.x, Menu_Rect.y+(pos++)*fheight,
		   "Fullscreen Mode: %s", GameConfig.UseFullscreen ? "ON" : "OFF");
#endif
      PrintString (ne_screen, OptionsMenu_Rect.x, Menu_Rect.y+(pos++)*fheight,
		   "Use 100%% CPU: %s", GameConfig.HogCPU ? "ON" : "OFF");
      PrintString (ne_screen, OptionsMenu_Rect.x, Menu_Rect.y+(pos++)*fheight, "Back");
      SDL_Flip( ne_screen );

      while (!key)
	{
	  SDL_Delay (1);

	  if ( EscapePressedR () )
	    {
	      finished = TRUE;
	      key = TRUE;
	    }
          if (MenuLeft() || MenuRight() || MenuChoose())
//	  if (RightPressed() || LeftPressed() || MouseLeftPressed() ||
//	      FirePressed() || ReturnPressed()|| MouseRightPressed())
	    key = TRUE;


	  switch (MenuPosition)
	    {
#ifndef GCW0
	    case SET_FULLSCREEN_FLAG:
	      if (MenuChooseR())
		{
		  toggle_fullscreen();
		  MenuItemSelectedSound();
		}
	      break;
#endif

	    case SET_HOG_CPU:
	      if (MenuChooseR())
		{
		  GameConfig.HogCPU = !GameConfig.HogCPU;
		  MenuItemSelectedSound();
		}
	      break;

	    case BACK:
	      if (MenuChooseR())
		{
		  MenuItemSelectedSound();
		  finished=TRUE;
		}
	      break;

	    case SET_BG_MUSIC_VOLUME:
	      if (MenuRightR())
		{
		  if ( GameConfig.Current_BG_Music_Volume < 1 )
		    GameConfig.Current_BG_Music_Volume += 0.05;
		  Set_BG_Music_Volume( GameConfig.Current_BG_Music_Volume );
		  MoveMenuPositionSound();
		}
	      if (MenuLeftR())
		{
		  if ( GameConfig.Current_BG_Music_Volume > 0 )
		    GameConfig.Current_BG_Music_Volume -= 0.05;
		  Set_BG_Music_Volume( GameConfig.Current_BG_Music_Volume );
		  MoveMenuPositionSound();
		}
	      break;

	      case SET_SOUND_FX_VOLUME:
		if (MenuRightR())
		  {
		    if ( GameConfig.Current_Sound_FX_Volume < 1 )
		      GameConfig.Current_Sound_FX_Volume += 0.05;
		    Set_Sound_FX_Volume( GameConfig.Current_Sound_FX_Volume );
		    MoveMenuPositionSound();
		  }
		if (MenuLeftR())
		  {
		    if ( GameConfig.Current_Sound_FX_Volume > 0 )
		      GameConfig.Current_Sound_FX_Volume -= 0.05;
		    Set_Sound_FX_Volume( GameConfig.Current_Sound_FX_Volume );
		    MoveMenuPositionSound();
		  }
		break;

	      case SET_GAMMA_CORRECTION:
		if (MenuRightR())
		  {
		    GameConfig.Current_Gamma_Correction+=0.05;
		    SDL_SetGamma( GameConfig.Current_Gamma_Correction ,
				  GameConfig.Current_Gamma_Correction ,
				  GameConfig.Current_Gamma_Correction );
		    MoveMenuPositionSound();
		  }
		if (MenuLeftR())
		  {
		    GameConfig.Current_Gamma_Correction-=0.05;
		    SDL_SetGamma( GameConfig.Current_Gamma_Correction ,
				  GameConfig.Current_Gamma_Correction ,
				  GameConfig.Current_Gamma_Correction );
		    MoveMenuPositionSound();
		  }
		break;
	    default:
	      DebugPrintf (0, "WARNING: illegal menu selection: %d\n", MenuPosition);
	      break;

	    } // switch MenuPosition


	  if (MenuUpR())
	    {
	      key = TRUE;
	      if ( MenuPosition > 1 ) MenuPosition--;
	      else MenuPosition = BACK;
	      MoveMenuPositionSound();
	    }
	  if (MenuDownR())
	    {
	      key = TRUE;
	      if ( MenuPosition < BACK ) MenuPosition++;
	      else MenuPosition = 1;
	      MoveMenuPositionSound();
	    }
	} // while !key

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
  bool key = FALSE;

enum
  {
    SHOW_POSITION=1,
    SHOW_FRAMERATE,
    SHOW_ENERGY,
    SHOW_DEATHCOUNT,
    BACK
  };

  while (!finished)
    {
      key = FALSE;
      SDL_BlitSurface (Menu_Background, NULL, ne_screen, NULL);

      PutInfluence (Menu_Rect.x, Menu_Rect.y + (MenuPosition-1.5)*fheight);

      PrintString (ne_screen, OptionsMenu_Rect.x, Menu_Rect.y+0*fheight,
		   "Show Position: %s", GameConfig.Draw_Position ? "ON" : "OFF");
      PrintString (ne_screen, OptionsMenu_Rect.x, Menu_Rect.y+1*fheight,
		   "Show Framerate: %s", GameConfig.Draw_Framerate? "ON" : "OFF");
      PrintString (ne_screen, OptionsMenu_Rect.x, Menu_Rect.y+2*fheight,
		   "Show Energy: %s", GameConfig.Draw_Energy? "ON" : "OFF");
      PrintString (ne_screen, OptionsMenu_Rect.x, Menu_Rect.y+3*fheight,
		   "Show DeathCount: %s", GameConfig.Draw_DeathCount ? "ON" : "OFF");

      PrintString (ne_screen, OptionsMenu_Rect.x, Menu_Rect.y+4*fheight, "Back");

      SDL_Flip( ne_screen );

      while (!key)
	{

	  SDL_Delay(1);

	  if ( EscapePressedR() )
	    {
	      finished = TRUE;
	      key = TRUE;
	    }

	  if (MenuChooseR())
	    {
	      MenuItemSelectedSound();
	      key = TRUE;
	      switch (MenuPosition)
		{
		case SHOW_POSITION:
		  GameConfig.Draw_Position=!GameConfig.Draw_Position;
		  InitiateMenu (TRUE);
		  break;
		case SHOW_FRAMERATE:
		  GameConfig.Draw_Framerate=!GameConfig.Draw_Framerate;
		  InitiateMenu (TRUE);
		  break;
		case SHOW_ENERGY:
		  GameConfig.Draw_Energy=!GameConfig.Draw_Energy;
		  InitiateMenu (TRUE);
		  break;
		case SHOW_DEATHCOUNT:
		  GameConfig.Draw_DeathCount = !GameConfig.Draw_DeathCount;
		  InitiateMenu (TRUE);
		  break;
		case BACK:
		  finished = TRUE;
		  break;
		default:
		  break;
		}
	    } // if FirePressed()

	  if (MenuUpR())
	    {
	      if ( MenuPosition > 1 ) MenuPosition--;
	      else MenuPosition = BACK;
	      MoveMenuPositionSound();
	      key = TRUE;
	    }
	  if (MenuDownR())
	    {
	      if ( MenuPosition < BACK ) MenuPosition++;
	      else MenuPosition = 1;
	      MoveMenuPositionSound();
	      key = TRUE;
	    }
	} // while !key

    } // while !finished

  return;

}; // On_Screen_Display_Options_Menu


/*@Function============================================================
@Desc: This function provides the credits screen.  It is a submenu of
       the big EscapeMenu.  Here you can see who helped developing the
       game, currently jp, rp and bastian.

@Ret:  none
* $Function----------------------------------------------------------*/
void
Credits_Menu (void)
{
  int h, em;
  SDL_Rect screen;
  BFont_Info *oldfont;
  int col2 = 2*User_Rect.w/3;

  h = FontHeight(Menu_BFont);
  em = CharWidth (Menu_BFont, 'm');

  Copy_Rect (Screen_Rect, screen);
  SDL_SetClipRect( ne_screen, NULL );
  DisplayImage ( find_file(CREDITS_PIC_FILE, GRAPHICS_DIR, NO_THEME, CRITICAL));
  MakeGridOnScreen (&screen);

  oldfont = GetCurrentFont();
  SetCurrentFont (Font1_BFont);

  printf_SDL (ne_screen, UserCenter_x - 2*em, h, "CREDITS\n");

  printf_SDL (ne_screen, em, -1, "PROGRAMMING:");
  printf_SDL (ne_screen, col2, -1, "Johannes Prix\n");
  printf_SDL (ne_screen, -1, -1, "Reinhard Prix\n");
  printf_SDL (ne_screen, -1, -1, "\n");

  printf_SDL (ne_screen, em, -1, "ARTWORK:");
  printf_SDL (ne_screen, col2, -1, "Bastian Salmela\n");
  printf_SDL (ne_screen, -1, -1, "\n");
  printf_SDL (ne_screen, em, -1, "ADDITIONAL THEMES:\n");
  printf_SDL (ne_screen, 2*em, -1, "Lanzz-theme");
  printf_SDL (ne_screen, col2, -1, "Lanzz\n");
  printf_SDL (ne_screen, 2*em, -1, "Para90-theme");
  printf_SDL (ne_screen, col2, -1, "Andreas Wedemeyer\n");

  printf_SDL (ne_screen, -1, -1, "\n");
  printf_SDL (ne_screen, em, -1, "C64 LEGACY MODS:\n");


  printf_SDL (ne_screen, 2*em, -1, "Green Beret, Sanxion, Uridium2");
  printf_SDL (ne_screen, col2, -1, "#dreamfish/trsi\n");

  printf_SDL (ne_screen, 2*em, -1, "The last V8, Anarchy");
  printf_SDL (ne_screen, col2, -1, "4-mat\n");

  printf_SDL (ne_screen, 2*em, -1, "Tron");
  printf_SDL (ne_screen, col2, -1, "Kollaps\n");


  printf_SDL (ne_screen, 2*em, -1, "Starpaws");
  printf_SDL (ne_screen, col2, -1, "Nashua\n");


  printf_SDL (ne_screen, 2*em, -1, "Commando");
  printf_SDL (ne_screen, col2, -1, "Android");


  SDL_Flip( ne_screen );

  wait4key();

  SetCurrentFont (oldfont);

  return;

} // Credits_Menu



//----------------------------------------------------------------------
// returns FALSE if level-editing should continue, TRUE if user chose 'back'
//----------------------------------------------------------------------

bool
LevelEditMenu (void)
{
  bool key = FALSE;
  int xoffs = 0;
  int Weiter=FALSE;
  char* OldMapPointer;
  int MenuPosition=1;
  bool Done = FALSE;
  int i;


  enum {
    BACK = 1,
    CHANGE_LEVEL_POSITION,
    CHANGE_COLOR,
    CHANGE_SIZE_X,
    CHANGE_SIZE_Y,
    SET_LEVEL_NAME,
    SET_BACKGROUND_SONG_NAME,
    SET_LEVEL_COMMENT,
    SAVE_LEVEL_POSITION,
    LAST
    };


  InitiateMenu (FALSE);
  while (!Weiter)
    {
      SDL_BlitSurface (Menu_Background, NULL, ne_screen, NULL);
      SDL_Delay(1);

      PutInfluence (Menu_Rect.x, Menu_Rect.y + (MenuPosition-1.5)*fheight);

      CenteredPutString ( ne_screen ,  1*FontHeight(Menu_BFont),    "LEVEL EDITOR");

      PrintString (ne_screen, OptionsMenu_Rect.x-xoffs, Menu_Rect.y + 0*fheight,
		   "Quit Level Editor");

      PrintString (ne_screen, OptionsMenu_Rect.x-xoffs, Menu_Rect.y + 1*fheight,
		   "Current: %d.  Level +/-" , CurLevel->levelnum );
      PrintString (ne_screen, OptionsMenu_Rect.x-xoffs, Menu_Rect.y + 2*fheight,
		   "Change level color: %s", ColorNames[CurLevel->color]);
      PrintString (ne_screen, OptionsMenu_Rect.x-xoffs, Menu_Rect.y + 3*fheight,
		   "Levelsize in X: %d.  -/+" , CurLevel->xlen );
      PrintString (ne_screen, OptionsMenu_Rect.x-xoffs, Menu_Rect.y + 4*fheight,
		   "Levelsize in Y: %d.  -/+" , CurLevel->ylen );
      PrintString (ne_screen, OptionsMenu_Rect.x-xoffs, Menu_Rect.y + 5*fheight,
		   "Level name: %s" , CurLevel->Levelname );
      PrintString (ne_screen, OptionsMenu_Rect.x-xoffs, Menu_Rect.y + 6*fheight,
		   "Background music: %s" , CurLevel->Background_Song_Name );
      PrintString (ne_screen, OptionsMenu_Rect.x-xoffs, Menu_Rect.y + 7*fheight,
		   "Level Comment: %s" , CurLevel->Level_Enter_Comment );
      PrintString (ne_screen, OptionsMenu_Rect.x-xoffs, Menu_Rect.y + 8*fheight,
		   "Save ship as  'Testship.shp'");

      SDL_Flip ( ne_screen );

      key = FALSE;
      while (!key)
	{
	  SDL_Delay(1);

	  if (LeftPressed()||RightPressed()||MouseLeftPressed()||MouseRightPressed()||SpacePressed())
	    key = TRUE;

	  if ( EscapePressedR() )
	    {
	      Weiter=TRUE;
	      key=TRUE;
	    }


	  switch (MenuPosition)
	    {
	    case SAVE_LEVEL_POSITION:
	      if (FirePressedR()||ReturnPressedR())
		{
		  MenuItemSelectedSound();
		  SaveShip("Testship");
		  CenteredPutString (ne_screen, 3*FontHeight(Menu_BFont),"Ship saved as 'Testship.shp'\n");
		  SDL_Flip ( ne_screen );
		  while ( !FirePressedR() && !EscapePressedR() && !ReturnPressedR() ) SDL_Delay(1);
		}
	      break;
	    case SET_LEVEL_NAME:
	      if (FirePressedR()||ReturnPressedR())
		{
		  MenuItemSelectedSound();
		  DisplayText ("New level name: ",
			       Menu_Rect.x-50, Menu_Rect.x+ 5*fheight,
			       &Full_User_Rect);
		  SDL_Flip( ne_screen );
		  CurLevel->Levelname = GetString(15, 2);
		  Weiter=!Weiter;
		}
	      break;
	    case SET_BACKGROUND_SONG_NAME:
	      if (FirePressedR()||ReturnPressedR())
		{
		  MenuItemSelectedSound();
		  DisplayText ("Bg music filename: ",
			       Menu_Rect.x-50, Menu_Rect.x+ 5*fheight,
			       &Full_User_Rect);
		  SDL_Flip( ne_screen );
		  CurLevel->Background_Song_Name=GetString(20, 2);
		}
	      break;
	    case SET_LEVEL_COMMENT:
	      if (FirePressedR()||ReturnPressedR())
		{
		  MenuItemSelectedSound();
		  DisplayText ("New level-comment :",
			       Menu_Rect.x-50, Menu_Rect.x+ 5*fheight,
			       &Full_User_Rect);
		  SDL_Flip( ne_screen );
		  CurLevel->Level_Enter_Comment=GetString(15 , FALSE );
		}
	      break;
	    case BACK:
	      if (FirePressedR()||ReturnPressedR())
		{
		  MenuItemSelectedSound();
		  Weiter=!Weiter;
		  Done = TRUE;
		  {
		    int i;
		    for (i = 0; i < curShip.num_levels; i++)
		      {
			ResetLevelMap (curShip.AllLevels[i]);	// close all doors
			InterpretMap (curShip.AllLevels[i]); // initialize doors, refreshes and lifts
		      }
		  }

		  SetCombatScaleTo( 1 );
		}
	      break;

	    case CHANGE_LEVEL_POSITION:
	      if (LeftPressedR()||MouseLeftPressedR())
		{
		  if ( CurLevel->levelnum > 0 )
		    Teleport ( CurLevel->levelnum -1 , 3 , 3 );
		  InitiateMenu(FALSE);
		  MoveMenuPositionSound();
		}
	      if ( RightPressedR()||MouseRightPressedR() )
		{
		  if ( CurLevel->levelnum < curShip.num_levels -1 )
		    Teleport ( CurLevel->levelnum +1 , 3 , 3 );
		  InitiateMenu(FALSE);
		  MoveMenuPositionSound();
		}
	      break;

	    case CHANGE_COLOR:
	      if ( (RightPressedR()||MouseRightPressedR()) && (CurLevel->color  < 6 ) )
		{
		  CurLevel->color++;
		  InitiateMenu(FALSE);
		  MoveMenuPositionSound();
		}

	      if ( (LeftPressedR()||MouseLeftPressedR()) && (CurLevel->color > 0) )
		{
		  CurLevel->color--;
		  InitiateMenu(FALSE);
		  MoveMenuPositionSound();
		}
	      break;

	    case CHANGE_SIZE_X:
	      if ( RightPressedR()||MouseRightPressedR() )
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
		  InitiateMenu (FALSE);
		  MoveMenuPositionSound();
		}
	      if (LeftPressedR()||MouseLeftPressedR())
		{
		  CurLevel->xlen--; // making it smaller is always easy:  just modify the value for size
		  // allocation of new memory or things like that are not nescessary.
		  InitiateMenu (FALSE);
		  MoveMenuPositionSound();
		}
	      break;

	    case CHANGE_SIZE_Y:
	      if ( RightPressedR()||MouseRightPressedR() )
		{
		  CurLevel->ylen++;

		  // In case of enlargement, we need to do more:
		  CurLevel->map[ CurLevel->ylen-1 ] = MyMalloc( CurLevel->xlen +1) ;

		  // We don't want to fill the new area with junk, do we? So we set it VOID
		  memset( CurLevel->map[ CurLevel->ylen-1 ] , VOID , CurLevel->xlen );
		  InitiateMenu (FALSE);
		  MoveMenuPositionSound();
		}

	      if (LeftPressedR()||MouseLeftPressedR())
		{
		  CurLevel->ylen--; // making it smaller is always easy:  just modify the value for size
		  // allocation of new memory or things like that are not nescessary.
		  InitiateMenu (FALSE);
		  MoveMenuPositionSound();
		}
	      break;

	    default:
	      DebugPrintf(0, "WARNING: nonexistant Menu selection: %d\n", MenuPosition);
	      break;

	    } // switch MenuPosition

	      // If the user pressed up or down, the cursor within
	      // the level editor menu has to be moved, which is done here:
	  if (UpPressedR() || WheelUpPressed ())
	    {
	      key = TRUE;
	      if (MenuPosition > 1) MenuPosition--;
	      else MenuPosition = LAST-1;
	      MoveMenuPositionSound();
	    }
	  if (DownPressedR() || WheelDownPressed ())
	    {
	      key = TRUE;
	      if ( MenuPosition < LAST-1 ) MenuPosition++;
	      else MenuPosition = 1;
	      MoveMenuPositionSound();
	    }

	} // while !key

    } // while !Weiter

  return (Done);
} // LevelEditMenu

// ======================================================================

// ------------------------------------------------------------
// show/edit keyboard-config
// ------------------------------------------------------------
void
Key_Config_Menu (void)
{
  int LastMenuPos = 1 + CMD_LAST;
  int selx = 1, sely = 1;   // currently selected menu-position
  bool finished = FALSE;
  bool key = FALSE;
  int posy = 0;
  int i;
  int oldkey, newkey = -1;

  enum { BACK};

  while (!finished)
    {
      key = FALSE;

      while (!key)
	{
	  Display_Key_Config (selx, sely);
	  SDL_Delay(1);

	  if ( EscapePressedR() )
	    {
	      finished = TRUE;
	      key = TRUE;
	    }

	  if (MenuChooseR())
	    {
	      MenuItemSelectedSound();
	      key = TRUE;

	      if (sely == 1)
		finished = TRUE;
	      else
		{
		  oldkey = key_cmds[sely-2][selx-1];
		  key_cmds[sely-2][selx-1] = '_';
		  Display_Key_Config (selx, sely);
		  newkey = getchar_raw(); // || joystick input!
		  if (newkey == SDLK_ESCAPE)
		    key_cmds[sely-2][selx-1] = oldkey;
		  else
		    key_cmds[sely-2][selx-1] = newkey;
		}

	    } // if FirePressed()

	  if (MenuUpR())
	    {
	      if ( sely > 1 ) sely--;
	      else sely = LastMenuPos;
	      MoveMenuPositionSound();
	      key = TRUE;
	    }
	  if (MenuDownR())
	    {
	      if ( sely < LastMenuPos ) sely++;
	      else sely = 1;
	      MoveMenuPositionSound();
	      key = TRUE;
	    }
	  if (MenuRightR())
	    {
	      if ( selx < 3 ) selx++;
	      else selx = 1;
	      MoveMenuPositionSound();
	      key = TRUE;
	    }
	  if (MenuLeftR())
	    {
	      if ( selx > 1 ) selx--;
	      else selx = 3;
	      MoveMenuPositionSound();
	      key = TRUE;
	    }
           /* There should really be a way to clear a key; this is dirty... 
	    * On a PC, one could just set a "junk" key, but not on a device with 
	    * limited buttons */ 
	  if (ClearBoundKeyR()) // Currently this = backspace, but in the future...
	    {
		    key_cmds[sely-2][selx-1] = 0;
	    } // Hmm, hopefully nothing nasty happens if back is selected... it doesn't seem to do anything

	} // while !key /* TODO: A user can't add joystick axises trough this menu! */

    } // while !finished

  return;

} // Key_Config_Menu()

// ------------------------------------------------------------
// subroutine to display the current key-config and highlight
//  current selection
// ------------------------------------------------------------
#define PosFont(x,y) ( (((x)!=selx)||((y)!=sely)) ? Font1_BFont : Font2_BFont )
void
Display_Key_Config (int selx, int sely)
{
  int startx = Full_User_Rect.x + 1.2*Block_Rect.w;
  int starty = Full_User_Rect.y + FontHeight(GetCurrentFont());
  int col1 = startx + 7.5 * CharWidth(GetCurrentFont(), 'O');
  int col2 = col1 + 6.5 * CharWidth(GetCurrentFont(), 'O');
  int col3 = col2 + 6.5 * CharWidth(GetCurrentFont(), 'O');
  int posy = 0;
  int i;

  SDL_BlitSurface (Menu_Background, NULL, ne_screen, NULL);

  //      PutInfluence (startx - 1.1*Block_Rect.w, starty + (MenuPosition-1.5)*fheight);

  PrintStringFont (ne_screen, (sely==1)? Font2_BFont:Font1_BFont, startx, starty+(posy++)*fheight, "Back");
#ifdef GCW0
  PrintStringFont (ne_screen, Font0_BFont, col1, starty, "(RShldr to clear an entry)");
#else
  PrintStringFont (ne_screen, Font0_BFont, col1, starty, "(Backspace to clear an entry)");
#endif

  PrintStringFont (ne_screen, Font0_BFont, startx, starty + (posy)*fheight, "Command");
  PrintStringFont (ne_screen, Font0_BFont, col1, starty + (posy)*fheight, "Key1");
  PrintStringFont (ne_screen, Font0_BFont, col2, starty + (posy)*fheight, "Key2");
  PrintStringFont (ne_screen, Font0_BFont, col3, starty + (posy)*fheight, "Key3");
  posy ++;

  for (i=0; i < CMD_LAST; i++)
    {
      PrintStringFont (ne_screen, Font0_BFont, startx, starty+(posy)*fheight, cmd_strings[i]);
      PrintStringFont (ne_screen, PosFont(1,2+i), col1, starty+(posy)*fheight, keystr[key_cmds[i][0]]);
      PrintStringFont (ne_screen, PosFont(2,2+i), col2, starty+(posy)*fheight, keystr[key_cmds[i][1]]);
      PrintStringFont (ne_screen, PosFont(3,2+i), col3, starty+(posy)*fheight, keystr[key_cmds[i][2]]);
      posy ++;
    }

  SDL_Flip( ne_screen );

  return;
} // Display_Key_Config


// ======================================================================

// ----------------------------------------------------------------------
// Cheat menu
// ----------------------------------------------------------------------
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
  char *status;

  // Prevent distortion of framerate by the delay coming from
  // the time spend in the menu.
  Activate_Conservative_Frame_Computation();

  font =  Font0_BFont;


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
      printf_SDL (ne_screen, -1, -1, " n. No hidden droids: %s",
		  show_all_droids ? "ON\n" : "OFF\n" );
      printf_SDL (ne_screen, -1, -1, " m. Map of Deck xy\n");
      printf_SDL (ne_screen, -1, -1, " s. Sound: %s",
		  sound_on ? "ON\n" : "OFF\n");
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
				   " NR.   ID  X    Y   ENERGY   Status\n");
		      printf_SDL (ne_screen, -1, -1,
				  "---------------------------------------------\n");
		    }

		  l ++;
		  if (AllEnemys[i].status == OUT)
		    status = "OUT";
		  else if (AllEnemys[i].status == TERMINATED)
		    status = "DEAD";
		  else
		    status = "ACTIVE";

		  printf_SDL (ne_screen, -1, -1,
			      "%d.   %s   %d   %d   %d    %s.\n", i,
			      Druidmap[AllEnemys[i].type].druidname,
			      (int)AllEnemys[i].pos.x,
			      (int)AllEnemys[i].pos.y,
			      (int)AllEnemys[i].energy,
			      status);
		} /* if (enemy on current level)  */
	    } /* for (i<NumEnemys) */

	  printf_SDL (ne_screen, -1, -1," --- END --- \n");
	  getchar_raw ();
	  break;

	case 'g': /* complete robot list of this ship */
	  for (i = 0; i < NumEnemys ; i++)
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
		  printf_SDL (ne_screen, x0, y0, "Nr.  Lev. ID  Energy  Status.\n");
		  printf_SDL (ne_screen, -1, -1, "------------------------------\n");
		}

	      printf_SDL (ne_screen, -1, -1, "%d  %d  %s  %d  %s\n",
			  i, AllEnemys[i].levelnum,
			  Druidmap[AllEnemys[i].type].druidname,
			  (int)AllEnemys[i].energy,
			  InfluenceModeNames[AllEnemys[i].status]);
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
	  Me.energy = (float) num;
	  if (Me.energy > Me.health) Me.health = Me.energy;
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

	case 'w':  /* print waypoint info of current level */
	  WpList = CurLevel->AllWaypoints;
	  for (i=0; i<CurLevel->num_waypoints; i++)
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

  ClearGraphMem ();

  update_input (); /* treat all pending keyboard events */

  return;
} /* Cheatmenu() */


#undef _menu_c
