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

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"
#include "map.h"

extern int key_cmds[CMD_LAST][3]; 
extern char *cmd_strings[CMD_LAST];
extern char *keystr[INPUT_LAST];

#define HIGHLIGHTCOLOR 255
#define HIGHLIGHTCOLOR2 100
#define ACTIVE_WP_COLOR 0x0FFFFFFFF

void Single_Player_Menu (void);
void Multi_Player_Menu (void);
void Credits_Menu (void);
void Options_Menu (void);
void Show_Mission_Instructions_Menu (void);
void Show_Waypoints(void);
void LevelEditor(void);
bool LevelEditMenu (void);
void DeleteWaypoint (level *level, int num); 
void CreateWaypoint (level *level, int BlockX, int BlockY);
void GraphicsSound_Options_Menu (void);
void On_Screen_Display_Options_Menu (void);
void Key_Config_Menu (void);
void Display_Key_Config (int selx, int sely);

char *key2str(int key);

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
  InitiateMenu (TRUE);

  PutString (ne_screen, User_Rect.x + User_Rect.w/10, 
	      User_Rect.y + User_Rect.h/2, "Do you really want to quit? (y/n) ");
  SDL_Flip (ne_screen);

  while ( (!KeyIsPressed('n')) && (!KeyIsPressed('y')) ) SDL_Delay(1);
  if (KeyIsPressed('y'))
    Terminate (OK);
}



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
      BACK2GAME=1, 
      POS_GRAPHICS_SOUND_OPTIONS,
      POS_LEGACY_OPTIONS,
      POS_ON_SCREEN_DISPLAYS,
      POS_LEVEL_EDITOR,
      POS_HIGHSCORES,
      POS_CREDITS,
      POS_KEYCONFIG,
      POS_QUIT
    };
  
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

      PutString (ne_screen, OptionsMenu_Rect.x,Menu_Rect.y+(pos++)*fheight, "Back to Game");
      PutString (ne_screen, OptionsMenu_Rect.x,Menu_Rect.y+(pos++)*fheight,"Graphics & Sound" );
      PutString (ne_screen, OptionsMenu_Rect.x,Menu_Rect.y+(pos++)*fheight,"Legacy Options");
      PutString (ne_screen, OptionsMenu_Rect.x,Menu_Rect.y+(pos++)*fheight,"On-Screen Displays" );
      PutString (ne_screen, OptionsMenu_Rect.x,Menu_Rect.y+(pos++)*fheight, "Level Editor");
      PutString (ne_screen, OptionsMenu_Rect.x,Menu_Rect.y+(pos++)*fheight, "Highscores");
      PutString (ne_screen, OptionsMenu_Rect.x,Menu_Rect.y+(pos++)*fheight, "Credits");
      PutString (ne_screen, OptionsMenu_Rect.x,Menu_Rect.y+(pos++)*fheight, "Configure Keys");
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
	  

	  if (FirePressedR()||ReturnPressedR())
	    {
	      MenuItemSelectedSound();
	      key = TRUE;
	      switch (MenuPosition) 
		{
		case BACK2GAME:
		  finished = TRUE;
		  break;
		case POS_GRAPHICS_SOUND_OPTIONS:
		  GraphicsSound_Options_Menu();
		  break;
		case POS_ON_SCREEN_DISPLAYS:
		  On_Screen_Display_Options_Menu();
		  break;
		case POS_LEGACY_OPTIONS:
		  Options_Menu();
		  break;
		case POS_LEVEL_EDITOR:
		  LevelEditor();
		  finished = TRUE;
		  break;
		case POS_HIGHSCORES:
		  ShowHighscores();
		  break;
		case POS_CREDITS:
		  Credits_Menu();
		  break;
		case POS_KEYCONFIG:
		  Key_Config_Menu();
		  break;
		case POS_QUIT:
		  QuitGameMenu ();
		  break;
		default: 
		  break;
		}
	    }

	  if (UpPressedR () || WheelUpPressed() ) 
	    {
	      key = TRUE;
	      if (MenuPosition > 1) MenuPosition--;
	      else MenuPosition = POS_QUIT;
	      MoveMenuPositionSound();
	    }
	  if (DownPressedR() || WheelDownPressed() ) 
	    {
	      key = TRUE;
	      if ( MenuPosition < POS_QUIT ) MenuPosition++;
	      else MenuPosition = 1;
	      MoveMenuPositionSound();
      
	    }


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

	  if (FirePressedR()||ReturnPressed())
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
		  newkey = getchar_raw();
		  if (newkey == SDLK_ESCAPE)
		    key_cmds[sely-2][selx-1] = oldkey;
		  else
		    key_cmds[sely-2][selx-1] = newkey;
		}

	    } // if FirePressed()

	  if (UpPressedR() || WheelUpPressed ()) 
	    {
	      if ( sely > 1 ) sely--;
	      else sely = LastMenuPos;
	      MoveMenuPositionSound();
	      key = TRUE;
	    }
	  if (DownPressedR() || WheelDownPressed ()) 
	    {
	      if ( sely < LastMenuPos ) sely++;
	      else sely = 1;
	      MoveMenuPositionSound();
	      key = TRUE;
	    }
	  if (RightPressedR())
	    {
	      if ( selx < 3 ) selx++;
	      else selx = 1;
	      MoveMenuPositionSound();
	      key = TRUE;
	    }
	  if (LeftPressedR())
	    {
	      if ( selx > 1 ) selx--;
	      else selx = 3;
	      MoveMenuPositionSound();
	      key = TRUE;
	    }

	} // while !key

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
      
  for (i=0; i < CMD_LAST; i++)
    {
      PrintStringFont (ne_screen, Font1_BFont, startx, starty+(posy)*fheight, cmd_strings[i]);
      PrintStringFont (ne_screen, PosFont(1,2+i), col1, starty+(posy)*fheight, keystr[key_cmds[i][0]]);
      PrintStringFont (ne_screen, PosFont(2,2+i), col2, starty+(posy)*fheight, keystr[key_cmds[i][1]]);
      PrintStringFont (ne_screen, PosFont(3,2+i), col3, starty+(posy)*fheight, keystr[key_cmds[i][2]]);
      posy ++;
    }
  
  SDL_Flip( ne_screen );
  
  return;
} // Display_Key_Config

// ----------------------------------------------------------------------
// translate an SDL-key into a string
// ----------------------------------------------------------------------
char *
key2str (int key)
{
  static char result[100];

  if ( (key < 128) && (isprint(key)) )
    {
      result[0] = key;
      result[1] = 0;
    }
  else
    switch (key) 
      {
      case SDLK_RETURN:
	strcpy (result, "Return");
	break;
      case SDLK_PAUSE:
	strcpy (result, "Pause");
	break;

      default: 
	strcpy (result, "<??>");
	break;

      }
  


} // key2str


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
    POS_TAKEOVER_IS_ACTIVATE,
    POS_BACK 
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

      PrintString (ne_screen, OptionsMenu_Rect.x, Menu_Rect.y+(pos++)*fheight,
		   "Transfer = Activate: %s", GameConfig.TakeoverActivates ? "YES":"NO" );

      PrintString (ne_screen, OptionsMenu_Rect.x, Menu_Rect.y+(pos++)*fheight, 
		   "Back");

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

	  if (FirePressedR()||ReturnPressedR())
	    {
	      MenuItemSelectedSound();
	      key = TRUE;
	      switch (MenuPosition) 
		{
		case POS_RESET:
		  GameConfig.Droid_Talk = FALSE;
		  GameConfig.ShowDecals = FALSE;
		  GameConfig.TakeoverActivates = TRUE;
		  GameConfig.AllMapVisible = TRUE;
		  GameConfig.FullUserRect = FALSE;
		  Copy_Rect (Classic_User_Rect, User_Rect);
		  new_tnum = 0;
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
		case POS_TAKEOVER_IS_ACTIVATE:
		  GameConfig.TakeoverActivates = !GameConfig.TakeoverActivates;
		  break;

		case POS_BACK:
		  finished = TRUE;
		  break;
		default: 
		  break;
		} 
	    } // if FirePressed

	  if (UpPressedR() || WheelUpPressed()) 
	    {
	      if ( MenuPosition > 1 ) MenuPosition--;
	      else MenuPosition = POS_BACK;
	      MoveMenuPositionSound();
	      key = TRUE;
	      ReleaseKey (SDLK_RIGHT); // clear any r-l movement
	      ReleaseKey (SDLK_LEFT);
	    }
	  if (DownPressedR() || WheelDownPressed()) 
	    {
	      if ( MenuPosition < POS_BACK ) MenuPosition++;
	      else MenuPosition = 1;
	      MoveMenuPositionSound();
	      key = TRUE;
	      ReleaseKey (SDLK_RIGHT); // clear any r-l movement
	      ReleaseKey (SDLK_LEFT);
	    }


	  if (LeftPressedR() )
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
	  if (RightPressedR() || MouseRightPressedR() )
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
    SET_FULLSCREEN_FLAG, 
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
      PrintString (ne_screen, OptionsMenu_Rect.x, Menu_Rect.y+(pos++)*fheight, 
		   "Fullscreen Mode: %s", GameConfig.UseFullscreen ? "ON" : "OFF");
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
	  if (RightPressed() || LeftPressed() || MouseLeftPressed() || 
	      FirePressed() || ReturnPressed()|| MouseRightPressed()) 
	    key = TRUE;

      
	  switch (MenuPosition)
	    {
	    case SET_FULLSCREEN_FLAG:
	      if (FirePressedR()||ReturnPressedR())
		{
		  toggle_fullscreen();
		  MenuItemSelectedSound();
		}
	      break;


	    case SET_HOG_CPU:
	      if (FirePressedR()||ReturnPressedR())
		{
		  GameConfig.HogCPU = !GameConfig.HogCPU;
		  MenuItemSelectedSound();
		}
	      break;

	    case BACK:
	      if (FirePressedR()||ReturnPressedR())
		{
		  MenuItemSelectedSound();
		  finished=TRUE;
		}
	      break;
	      
	    case SET_BG_MUSIC_VOLUME: 
	      if (RightPressedR()||MouseRightPressedR()) 
		{
		  if ( GameConfig.Current_BG_Music_Volume < 1 ) 
		    GameConfig.Current_BG_Music_Volume += 0.05;
		  Set_BG_Music_Volume( GameConfig.Current_BG_Music_Volume );
		  MoveMenuPositionSound();
		}
	      if (LeftPressedR()||MouseLeftPressedR()) 
		{
		  if ( GameConfig.Current_BG_Music_Volume > 0 ) 
		    GameConfig.Current_BG_Music_Volume -= 0.05;
		  Set_BG_Music_Volume( GameConfig.Current_BG_Music_Volume );
		  MoveMenuPositionSound();
		}
	      break;

	      case SET_SOUND_FX_VOLUME:
		if (RightPressedR()||MouseRightPressedR()) 
		  {
		    if ( GameConfig.Current_Sound_FX_Volume < 1 ) 
		      GameConfig.Current_Sound_FX_Volume += 0.05;
		    Set_Sound_FX_Volume( GameConfig.Current_Sound_FX_Volume );
		    MoveMenuPositionSound();
		  }
		if (LeftPressedR()||MouseLeftPressedR()) 
		  {
		    if ( GameConfig.Current_Sound_FX_Volume > 0 ) 
		      GameConfig.Current_Sound_FX_Volume -= 0.05;
		    Set_Sound_FX_Volume( GameConfig.Current_Sound_FX_Volume );
		    MoveMenuPositionSound();
		  }
		break;

	      case SET_GAMMA_CORRECTION:
		if (RightPressedR()||MouseRightPressedR()) 
		  {
		    GameConfig.Current_Gamma_Correction+=0.05;
		    SDL_SetGamma( GameConfig.Current_Gamma_Correction , 
				  GameConfig.Current_Gamma_Correction , 
				  GameConfig.Current_Gamma_Correction );
		    MoveMenuPositionSound();
		  }
		if (LeftPressedR()||MouseLeftPressedR()) 
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


	  if (UpPressedR() || WheelUpPressed ()) 
	    {
	      key = TRUE;
	      if ( MenuPosition > 1 ) MenuPosition--;
	      else MenuPosition = BACK;
	      MoveMenuPositionSound();
	    }
	  if (DownPressedR() || WheelDownPressed()) 
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
  { SHOW_POSITION=1, 
    SHOW_FRAMERATE, 
    SHOW_ENERGY,
    SHOW_DEATHCOUNT,
    BACK };

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

	  if (FirePressedR()||ReturnPressed())
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

	  if (UpPressedR() || WheelUpPressed ()) 
	    {
	      if ( MenuPosition > 1 ) MenuPosition--;
	      else MenuPosition = BACK;
	      MoveMenuPositionSound();
	      key = TRUE;
	    }
	  if (DownPressedR() || WheelDownPressed ()) 
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

  SDL_LockSurface( ne_screen );

  for (i=0; i<Block_Rect.w; i++)
    {
      // This draws a (double) line at the upper border of the current block
      putpixel( ne_screen , 
		i + User_Rect.x + (User_Rect.w/2) + (rintf(Me.pos.x)-Me.pos.x - 0.5) * Block_Rect.w , 
		UserCenter_y + ( rintf(Me.pos.y)-Me.pos.y - 0.5 ) * Block_Rect.h , 
		HIGHLIGHTCOLOR );
      putpixel( ne_screen , 
		i + User_Rect.x + (User_Rect.w/2) + (rintf(Me.pos.x)-Me.pos.x - 0.5) * Block_Rect.w , 
		UserCenter_y + ( rintf(Me.pos.y)-Me.pos.y - 0.5 ) * Block_Rect.h + 1 , 
		HIGHLIGHTCOLOR );

      // This draws a line at the lower border of the current block
      putpixel( ne_screen , 
		i + User_Rect.x + (User_Rect.w/2) + (rintf(Me.pos.x)-Me.pos.x - 0.5) * Block_Rect.w , 
		UserCenter_y + ( rintf(Me.pos.y)-Me.pos.y + 0.5 ) * Block_Rect.h - 1, 
		HIGHLIGHTCOLOR );
      putpixel( ne_screen , 
		i + User_Rect.x + (User_Rect.w/2) + (rintf(Me.pos.x)-Me.pos.x - 0.5) * Block_Rect.w , 
		UserCenter_y + ( rintf(Me.pos.y)-Me.pos.y + 0.5 ) * Block_Rect.h - 2, 
		HIGHLIGHTCOLOR );

      // This draws a line at the left border of the current block
      putpixel( ne_screen , 
		0 + User_Rect.x + (User_Rect.w/2) + (rintf(Me.pos.x)-Me.pos.x - 0.5) * Block_Rect.w , 
		UserCenter_y + ( rintf(Me.pos.y)-Me.pos.y - 0.5 ) * Block_Rect.h + i , 
		// User_Rect.y + User_Rect.h/2 + (rintf(Me.pos.y)-Me.pos.y - 0.5 ) * Block_Rect.h + i , 
		HIGHLIGHTCOLOR );
      putpixel( ne_screen , 
		1 + User_Rect.x + (User_Rect.w/2) + (rintf(Me.pos.x)-Me.pos.x - 0.5) * Block_Rect.w , 
		UserCenter_y + ( rintf(Me.pos.y)-Me.pos.y - 0.5 ) * Block_Rect.h + i , 
		// User_Rect.y + User_Rect.h/2 + (rintf(Me.pos.y)-Me.pos.y - 0.5 ) * Block_Rect.h + i , 
		HIGHLIGHTCOLOR );

      // This draws a line at the right border of the current block
      putpixel( ne_screen , 
		-1 + User_Rect.x + (User_Rect.w/2) + (rintf(Me.pos.x)-Me.pos.x + 0.5) * Block_Rect.w , 
		UserCenter_y + ( rintf(Me.pos.y)-Me.pos.y - 0.5 ) * Block_Rect.h + i , 
		// User_Rect.y + User_Rect.h/2 + (rintf(Me.pos.y)-Me.pos.y - 0.5 ) * Block_Rect.h + i , 
		HIGHLIGHTCOLOR );
      putpixel( ne_screen , 
		-2 + User_Rect.x + (User_Rect.w/2) + (rintf(Me.pos.x)-Me.pos.x + 0.5) * Block_Rect.w , 
		UserCenter_y + ( rintf(Me.pos.y)-Me.pos.y - 0.5 ) * Block_Rect.h + i , 
		// User_Rect.y + User_Rect.h/2 + (rintf(Me.pos.y)-Me.pos.y - 0.5 ) * Block_Rect.h + i , 
		HIGHLIGHTCOLOR );

      /*
	      TargetRectangle.x = UserCenter_x 
		+ ( -Me.pos.x+col-0.5 )*Block_Rect.w;
	      TargetRectangle.y = UserCenter_y
		+ ( -Me.pos.y+line-0.5 )*Block_Rect.h;
	      SDL_BlitSurface( MapBlockSurfacePointer[ CurLevel->color ][MapBrick] , NULL ,
 			       ne_screen, &TargetRectangle);
      */


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
  waypoint *this_wp;


  BlockX=rintf(Me.pos.x);
  BlockY=rintf(Me.pos.y);
	  
  SDL_LockSurface( ne_screen );

  for (wp=0; wp<CurLevel->num_waypoints; wp++)
    {
      this_wp = &CurLevel->AllWaypoints[wp];
      //--------------------
      // Draw the cross in the middle of the middle of the tile
      //
      for (i= Block_Rect.w/4; i<3 * Block_Rect.w / 4; i++)
	{
	  // This draws a (double) line at the upper border of the current block
	  x = i + User_Rect.x+(User_Rect.w/2)- (( Me.pos.x)-this_wp->x + 0.5) * Block_Rect.w;
	  y = i + UserCenter_y - (( Me.pos.y)-this_wp->y + 0.5) * Block_Rect.h;
	  if ( ( x < User_Rect.x ) || ( x > User_Rect.x + User_Rect.w ) || ( y < User_Rect. y) || ( y > User_Rect.y + User_Rect.h ) ) continue;
	  putpixel( ne_screen , x , y , HIGHLIGHTCOLOR );

		    
	  x = i + User_Rect.x + (User_Rect.w/2) - (( Me.pos.x )-this_wp->x + 0.5) * Block_Rect.w;
	  y = i + UserCenter_y - (( Me.pos.y)-this_wp->y + 0.5) * Block_Rect.h + 1;
	  if ( ( x < User_Rect.x ) || ( x > User_Rect.x + User_Rect.w ) || ( y < User_Rect. y) || ( y > User_Rect.y + User_Rect.h ) ) continue;
	  putpixel( ne_screen , x , y , HIGHLIGHTCOLOR );
	  
	  // This draws a line at the lower border of the current block
	  x = i + User_Rect.x + (User_Rect.w/2) - (( Me.pos.x)-this_wp->x + 0.5) * Block_Rect.w;
	  y = -i + UserCenter_y - (( Me.pos.y )-this_wp->y - 0.5 ) * Block_Rect.h -1;
	  if ( ( x < User_Rect.x ) || ( x > User_Rect.x + User_Rect.w ) || ( y < User_Rect. y) || ( y > User_Rect.y + User_Rect.h ) ) continue;
	  putpixel( ne_screen , x , y , HIGHLIGHTCOLOR );

	  x = i + User_Rect.x + (User_Rect.w/2) - (( Me.pos.x)-this_wp->x + 0.5) * Block_Rect.w;
	  y = -i + UserCenter_y - ((Me.pos.y)-this_wp->y - 0.5 ) * Block_Rect.h -2;
	  if ( ( x < User_Rect.x ) || ( x > User_Rect.x + User_Rect.w ) || ( y < User_Rect. y) || ( y > User_Rect.y + User_Rect.h ) ) continue;
	  putpixel( ne_screen , x , y , HIGHLIGHTCOLOR );
	  
	}

      //--------------------
      // Draw the connections to other waypoints, BUT ONLY FOR THE WAYPOINT CURRENTLY TARGETED
      //
      if ( (BlockX == this_wp->x) && (BlockY == this_wp->y) )
	for ( i=0; i<this_wp->num_connections; i++ )
	  {
	    DrawLineBetweenTiles( this_wp->x , this_wp->y , 
				  CurLevel->AllWaypoints[this_wp->connections[i]].x , 
				  CurLevel->AllWaypoints[this_wp->connections[i]].y ,
				  HIGHLIGHTCOLOR );
	  }
    }

  SDL_UnlockSurface( ne_screen );

  return;

} // void Show_Waypoints(void);

/*@Function============================================================
@Desc: This function is provides the Level Editor integrated into 
       freedroid.  Actually this function is a submenu of the big
       Escape Menu.  In here you can edit the level and upon pressing
       escape enter a further submenu where you can save the level,
       change level name and quit from level editing.

@Ret:  none
* $Function----------------------------------------------------------*/
void 
LevelEditor(void)
{
  int BlockX=rintf(Me.pos.x);
  int BlockY=rintf(Me.pos.y);
  int Done=FALSE;
  int i,k;
  int SpecialMapValue;
  int OriginWaypoint = (-1);
  char* NumericInputString;
  SDL_Rect rect;
  waypoint *SrcWp;

  int KeymapOffset = 15;
  
  Copy_Rect (User_Rect, rect);
  Copy_Rect (Screen_Rect, User_Rect);  /// level editor can use the full screen!

  while ( !Done )
    {
      SDL_Delay(1);

      BlockX=rintf(Me.pos.x);
      BlockY=rintf(Me.pos.y);
	  
      Fill_Rect (User_Rect, Black);
      Assemble_Combat_Picture ( ONLY_SHOW_MAP );
      Highlight_Current_Block();
      Show_Waypoints();

      // show line between a selected connection-origin and the current block
      if (OriginWaypoint != (-1) )
	DrawLineBetweenTiles( BlockX, BlockY, 
			      CurLevel->AllWaypoints[OriginWaypoint].x, 
			      CurLevel->AllWaypoints[OriginWaypoint].y,
			      HIGHLIGHTCOLOR2 );

      
      PrintStringFont (ne_screen, FPS_Display_BFont, Full_User_Rect.x+Full_User_Rect.w/3 , 
		       Full_User_Rect.y+Full_User_Rect.h - FontHeight(FPS_Display_BFont), 
		       "Press F1 for keymap");

      SDL_Flip( ne_screen );

      //--------------------
      // If the user of the Level editor pressed some cursor keys, move the
      // highlited filed (that is Me.pos) accordingly. This is done here:
      //
      if (LeftPressedR()) 
	if ( rintf(Me.pos.x) > 0 ) Me.pos.x-=1;

      if (RightPressedR()) 
	if ( rintf(Me.pos.x) < CurLevel->xlen-1 ) Me.pos.x+=1;

      if (UpPressedR()) 
	if ( rintf(Me.pos.y) > 0 ) Me.pos.y-=1;

      if (DownPressedR()) 
	if ( rintf(Me.pos.y) < CurLevel->ylen-1 ) Me.pos.y+=1;


      if ( KeyIsPressedR (SDLK_F1) )
	{
	  k=3;
	  //	  SDL_BlitSurface ( console_bg_pic2 , NULL, ne_screen, NULL);
	  MakeGridOnScreen (NULL);
	  CenteredPutString   ( ne_screen ,  (k)*FontHeight(Menu_BFont), "Level Editor Keymap"); k+=2;
	  // DisplayText ("Use cursor keys to move around.", 1, 2 *FontHeight(Menu_BFont), NULL );
	  PutString ( ne_screen , KeymapOffset , (k) * FontHeight(Menu_BFont)  , "Use cursor keys to move around." ); k++;
	  PutString ( ne_screen , KeymapOffset , (k) * FontHeight(Menu_BFont)  , "Use number pad to plant walls." ); k++;
	  PutString ( ne_screen , KeymapOffset , (k) * FontHeight(Menu_BFont)  , "Use shift and number pad to plant extras." ); k++;
	  PutString ( ne_screen , KeymapOffset , (k) * FontHeight(Menu_BFont)  , "R...Refresh, 1-5...Blocktype 1-5, L...Lift" ); k++;
	  PutString ( ne_screen , KeymapOffset , (k) * FontHeight(Menu_BFont)  , "F...Fine grid, T/SHIFT + T...Doors" ); k++;
	  PutString ( ne_screen , KeymapOffset , (k) * FontHeight(Menu_BFont)  , "M...Alert, E...Enter tile by number" ); k++; 
	  PutString ( ne_screen , KeymapOffset , (k) * FontHeight(Menu_BFont)  , "Space/Enter...Floor" ); k+=2;
	  
	  PutString ( ne_screen , KeymapOffset , (k) * FontHeight(Menu_BFont)  , "I/O...zoom INTO/OUT OF the map" ); k+=2;
	  PutString ( ne_screen , KeymapOffset , (k) * FontHeight(Menu_BFont)  , "P...toggle wayPOINT on/off" ); k++;
	  PutString ( ne_screen , KeymapOffset , (k) * FontHeight(Menu_BFont)  , "C...start/end waypoint CONNECTION" ); k++;

	  SDL_Flip ( ne_screen );
	  while (!FirePressedR() && !EscapePressedR() && !ReturnPressedR() ) SDL_Delay(1);
	}
      
      //--------------------
      // Since the level editor will not always be able to
      // immediately feature all the the map tiles that might
      // have been added recently, we should offer a feature, so that you can
      // specify the value of a map piece just numerically.  This will be
      // done upon pressing the 'e' key.
	  //
      if ( KeyIsPressedR ('e') )
	{
	  CenteredPutString   ( ne_screen ,  6*FontHeight(Menu_BFont), "Please enter new value: ");
	  SDL_Flip( ne_screen );
	  NumericInputString = GetString (10, 2); 
	  sscanf( NumericInputString , "%d" , &SpecialMapValue );
	  if ( SpecialMapValue >= NUM_MAP_BLOCKS ) SpecialMapValue=0;
	  CurLevel->map[BlockY][BlockX]=SpecialMapValue;
	}

      //--------------------
      //If the person using the level editor decides he/she wants a different
      //scale for the editing process, he/she may say so by using the O/I keys.
      //
      if ( KeyIsPressedR ('o') )
	{
	  if (CurrentCombatScaleFactor > 0.25 )
	    CurrentCombatScaleFactor -= 0.25;
	  SetCombatScaleTo (CurrentCombatScaleFactor);
	}
      if ( KeyIsPressedR ('i') )
	{
	  CurrentCombatScaleFactor += 0.25;
	  SetCombatScaleTo (CurrentCombatScaleFactor);
	}
      
      // toggle waypoint on current square.  That means either removed or added.
      // And in case of removal, also the connections must be removed.
      if (KeyIsPressedR('p'))
	{
	  // find out if there is a waypoint on the current square
	  for (i=0 ; i < CurLevel->num_waypoints; i++)
	    {
	      if ( ( CurLevel->AllWaypoints[i].x == BlockX ) &&
		   ( CurLevel->AllWaypoints[i].y == BlockY ) ) break;
	    }
	  
	  // if its waypoint already, this waypoint must be deleted.
	  if (i < CurLevel->num_waypoints)
	    DeleteWaypoint (CurLevel, i); 
	  else // if its not a waypoint already, it must be made into one
	    CreateWaypoint (CurLevel, BlockX, BlockY);

	} // if 'p' pressed (toggle waypoint)

      // create a connection between waypoints.  If this is the first selected waypoint, its
      // an origin and the second "C"-pressed waypoint will be used a target.
      // If origin and destination are the same, the operation is cancelled.
      if (KeyIsPressedR ('c'))
	{
	  // Determine which waypoint is currently targeted
	  for (i=0 ; i < CurLevel->num_waypoints ; i++)
	    {
	      if ( ( CurLevel->AllWaypoints[i].x == BlockX ) &&
		   ( CurLevel->AllWaypoints[i].y == BlockY ) ) break;
	    }
	  
	  if ( i == CurLevel->num_waypoints )
	    DebugPrintf(0, "\nSorry, no waypoint here to connect...\n");
	  else
	    {

	      if ( OriginWaypoint == (-1) )
		{
		  OriginWaypoint = i;
		  SrcWp = &(CurLevel->AllWaypoints[OriginWaypoint]);
		  if (SrcWp->num_connections < MAX_WP_CONNECTIONS)
		    DebugPrintf (0, "\nWaypoint nr. %d. selected as origin\n", i);
		  else
		    {
		      DebugPrintf (0, "\nSorry, maximal number of waypoint-connections (%d) reached!\n",
				   MAX_WP_CONNECTIONS);
		      DebugPrintf (0, "Operation not possible\n");
		      OriginWaypoint = (-1);
		      SrcWp = NULL;
		    }
		}
	      else
		{
		  if ( OriginWaypoint == i )
		    {
		      DebugPrintf(0, "\nOrigin==Target --> Connection Operation cancelled.\n");
		      OriginWaypoint = (-1);
		      SrcWp = NULL;
		    }
		  else
		    {
		      DebugPrintf(0, "\nTarget-waypoint %d selected\n Connection established!\n", i );
		      SrcWp->connections[SrcWp->num_connections] = i;
		      SrcWp->num_connections ++;
		      OriginWaypoint = (-1);
		      SrcWp = NULL;
		    }
		}
	    }

	}
      
      // If the person using the level editor pressed some editing keys, insert the
      // corresponding map tile.  This is done here:
      if (KeyIsPressedR ('f')) 
	CurLevel->map[BlockY][BlockX]=FINE_GRID;
      if (KeyIsPressedR ('1')) 
	CurLevel->map[BlockY][BlockX]=BLOCK1;
      if (KeyIsPressedR ('2')) 
	CurLevel->map[BlockY][BlockX]=BLOCK2;
      if (KeyIsPressedR ('3'))
	CurLevel->map[BlockY][BlockX]=BLOCK3;
      if (KeyIsPressedR ('4'))
	CurLevel->map[BlockY][BlockX]=BLOCK4;
      if (KeyIsPressedR ('5'))
	CurLevel->map[BlockY][BlockX]=BLOCK5;
      if (KeyIsPressedR ('l'))
	CurLevel->map[BlockY][BlockX]=LIFT;
      if (KeyIsPressedR (SDLK_KP_PLUS))
	CurLevel->map[BlockY][BlockX]=V_WALL;
      if (KeyIsPressedR (SDLK_KP0))
	CurLevel->map[BlockY][BlockX]=H_WALL;
      if (KeyIsPressedR(SDLK_KP1))
	CurLevel->map[BlockY][BlockX]=ECK_LU;
      if (KeyIsPressedR (SDLK_KP2))
	{
	  if (!ShiftPressed())
	    CurLevel->map[BlockY][BlockX]=T_U;
	  else CurLevel->map[BlockY][BlockX]=KONSOLE_U;
	    }
      if (KeyIsPressedR (SDLK_KP3))
	CurLevel->map[BlockY][BlockX]=ECK_RU;
      if (KeyIsPressedR (SDLK_KP4))
	{
	  if (!ShiftPressed())
	    CurLevel->map[BlockY][BlockX]=T_L;
	  else CurLevel->map[BlockY][BlockX]=KONSOLE_L;
	}
      if (KeyIsPressedR (SDLK_KP5))
	{
	  if (!ShiftPressed())
	    CurLevel->map[BlockY][BlockX]=KREUZ;
	  else CurLevel->map[BlockY][BlockX]=VOID;
	}
      if (KeyIsPressedR (SDLK_KP6))
	{
	  if (!ShiftPressed())
	    CurLevel->map[BlockY][BlockX]=T_R;
	  else CurLevel->map[BlockY][BlockX]=KONSOLE_R;
	}
      if (KeyIsPressedR (SDLK_KP7))
	CurLevel->map[BlockY][BlockX]=ECK_LO;
      if (KeyIsPressedR (SDLK_KP8))
	{
	  if (!ShiftPressed())
	    CurLevel->map[BlockY][BlockX]=T_O;
	  else CurLevel->map[BlockY][BlockX]=KONSOLE_O;
	}
      if (KeyIsPressedR (SDLK_KP9))
	CurLevel->map[BlockY][BlockX]=ECK_RO;
      if ( KeyIsPressedR ('m'))
	CurLevel->map[BlockY][BlockX]=ALERT_GREEN;	      
      if (KeyIsPressedR ('r'))
	CurLevel->map[BlockY][BlockX]=REFRESH1;	            
      if (KeyIsPressedR('t'))
	{
	  if (ShiftPressed())
	    CurLevel->map[BlockY][BlockX]=V_ZUTUERE;	            	      
	  else CurLevel->map[BlockY][BlockX]=H_ZUTUERE;	            	      
	}
      if ((SpacePressed() || MouseLeftPressed()))
	CurLevel->map[BlockY][BlockX]=FLOOR;	            	      	    

      // After Level editing is done and escape has been pressed, 
      // display the Menu with level save options and all that.
      
      if (EscapePressedR())
	Done = LevelEditMenu();
      
    } // while (!Done)

  ShuffleEnemys ();  // now make sure droids get redestributed correctly!

  Copy_Rect (rect, User_Rect);

  ClearGraphMem();
  return;

} // void LevelEditor(void)


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

// delete given waypoint num (and all its connections) on level Lev
void
DeleteWaypoint (level *Lev, int num)
{
  int i, j;
  waypoint *WpList, *ThisWp;
  int wpmax;

  WpList = Lev->AllWaypoints;
  wpmax = Lev->num_waypoints - 1;
  
  // is this the last one? then just delete
  if (num == wpmax)
    WpList[num].num_connections = 0;
  else // otherwise shift down all higher waypoints
    memcpy (&WpList[num], &WpList[num+1], (wpmax - num) * sizeof(waypoint) );

  // now there's one less:
  Lev->num_waypoints --;
  wpmax --;

  // now adjust the remaining wp-list to the changes:
  ThisWp = WpList;
  for (i=0; i < Lev->num_waypoints; i++, ThisWp++)
    for (j=0; j < ThisWp->num_connections; j++)
      {
	// eliminate all references to this waypoint
	if (ThisWp->connections[j] == num)
	  {
	    // move all connections after this one down
	    memcpy (&(ThisWp->connections[j]), &(ThisWp->connections[j+1]), 
		    (ThisWp->num_connections-1 - j)*sizeof(int));
	    ThisWp->num_connections --;
	    j --;  // just to be sure... check the next connection as well...(they have been shifted!)
	    continue;
	  }
	// adjust all connections to the shifted waypoint-numbers
	else if (ThisWp->connections[j] > num)
	  ThisWp->connections[j] --;
	
      } // for j < num_connections

} // DeleteWaypoint()

/*----------------------------------------------------------------------
 * create a new empty waypoint on position x/y
 *----------------------------------------------------------------------*/
void
CreateWaypoint (level *Lev, int x, int y)
{
  int num;

  if (Lev->num_waypoints == MAXWAYPOINTS)
    {
      DebugPrintf (0, "WARNING: maximal number of waypoints (%d) reached on this level!!\n",
		   MAXWAYPOINTS);
      DebugPrintf (0, "... cannot insert any more, sorry!\n");
      return;
    }

  num = Lev->num_waypoints;
  Lev->num_waypoints ++;

  Lev->AllWaypoints[num].x = x;
  Lev->AllWaypoints[num].y = y;
  Lev->AllWaypoints[num].num_connections = 0;

  return;
} // CreateWaypoint()



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
