/*
 *
 *   Copyright (c) 2016  Reinhard Prix
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
// ----- local types ----------
typedef enum
{
  ACTION_NONE,
  ACTION_INFO,
  ACTION_BACK,
  ACTION_CLICK,
  ACTION_LEFT,
  ACTION_RIGHT,
  ACTION_UP,
  ACTION_DOWN
} MenuAction_t;

typedef struct MenuEntry_s
{
  const char *name;			/**< menu entry string */
  const char *(*handler)();		/**< handler & info function for this menu entry (none if NULL) */
  const struct MenuEntry_s *submenu; 	/**< enter this submenu (if non-NULL) */
} MenuEntry_t;


// ----- local prototypes ----------
void Credits_Menu (void);
void Key_Config_Menu (void);

void Display_Key_Config (int selx, int sely);
bool LevelEditMenu (void);

void ShowMenu ( const MenuEntry_t *menu );

const char *handle_StrictlyClassic ( MenuAction_t action );
const char *handle_WindowType ( MenuAction_t action );
const char *handle_Theme ( MenuAction_t action );
const char *handle_DroidTalk ( MenuAction_t action );
const char *handle_AllMapVisible ( MenuAction_t action );
const char *handle_ShowDecals ( MenuAction_t action );
const char *handle_TransferIsActivate ( MenuAction_t action );
const char *handle_FireIsTransfer ( MenuAction_t action );

const char *handle_MusicVolume ( MenuAction_t action );
const char *handle_SoundVolume ( MenuAction_t action );
const char *handle_Fullscreen ( MenuAction_t action );

const char *handle_ShowPosition ( MenuAction_t action );
const char *handle_ShowFramerate ( MenuAction_t action );
const char *handle_ShowEnergy ( MenuAction_t action );
const char *handle_ShowDeathCount ( MenuAction_t action );

const char *handle_LevelEditor ( MenuAction_t action );
const char *handle_ShowHighscores ( MenuAction_t action );
const char *handle_Credits_Menu ( MenuAction_t action );
const char *handle_Key_Config_Menu ( MenuAction_t action );
const char *handle_QuitGameMenu ( MenuAction_t action );

void setTheme ( int theme_index );
const char *isToggleOn ( int toggle );
void flipToggle ( int *toggle );
void menuChangeFloat ( MenuAction_t action, float *val, float step, float min_val, float max_val );

// ----- define menus ----------
MenuEntry_t LegacyMenu[] =
  {
    { "Back",			NULL,			NULL },
    { "Set Strictly Classic",	handle_StrictlyClassic,	NULL },
    { "Combat window: ", 	handle_WindowType, 	NULL },
    { "Graphics theme: ", 	handle_Theme,		NULL },
    { "Droid Talk: ", 		handle_DroidTalk,	NULL },
    { "Show Decals: ", 		handle_ShowDecals,	NULL },
    { "All Map Visible: ", 	handle_AllMapVisible,	NULL },
    { "Transfer = Activate: ", 	handle_TransferIsActivate, NULL },
    { "Hold Fire to Transfer: ",handle_FireIsTransfer,	NULL },
    { NULL,			NULL, 			NULL }
  };

MenuEntry_t GraphicsSoundMenu[] =
  {
    { "Back", 			NULL,			NULL },
    { "Music volume: ", 	handle_MusicVolume,	NULL },
    { "Sound volume: ", 	handle_SoundVolume,	NULL },
    { "Fullscreen Mode: ", 	handle_Fullscreen,	NULL },
    { NULL,			NULL, 			NULL }
  };

MenuEntry_t HUDMenu[] =
{
  { "Back", 			NULL,			NULL },
  { "Show Position: ", 		handle_ShowPosition,	NULL },
  { "Show Framerate: ",		handle_ShowFramerate,	NULL },
  { "Show Energy: ",		handle_ShowEnergy,	NULL },
//  { "Show DeathCount: ",	handle_ShowDeathCount,	NULL },
  { NULL,			NULL, 			NULL }
};


MenuEntry_t MainMenu[] =
  {
    { "Back to Game", 		NULL, 					NULL },
    { "Graphics & Sound", 	NULL, 					GraphicsSoundMenu },
    { "Legacy Options", 	NULL,					LegacyMenu },
    { "HUD Settings",		NULL, 					HUDMenu },
    { "Level Editor",		handle_LevelEditor,			NULL },
    { "Highscores", 		handle_ShowHighscores, 			NULL },
    { "Credits", 		handle_Credits_Menu, 			NULL },
    { "Configure Keys", 	handle_Key_Config_Menu, 		NULL },
    { "Quit Game",		handle_QuitGameMenu, 			NULL },
    { NULL,			NULL, 					NULL }
  };


const char *
isToggleOn ( int toggle )
{
  return toggle ? "YES" : "NO";
}
void
flipToggle ( int *toggle )
{
  if (toggle)
    {
      (*toggle) = !(*toggle);
      MenuItemSelectedSound();
    }
  return;
}

void
setTheme ( int theme_index )
{
  if ( (theme_index < 0) || ( theme_index > AllThemes.num_themes - 1 ) )
    {
      DebugPrintf ( 0, "%s: Coding error: invalid theme index '%s' requested, must be within [0, %d]\n", __func__, theme_index, AllThemes.num_themes - 1 );
      Terminate(ERR);
    }
  AllThemes.cur_tnum = theme_index;
  strcpy ( GameConfig.Theme_Name, AllThemes.theme_name [ AllThemes.cur_tnum ] );
  InitPictures();
  return;
} // setTheme()
void
menuChangeFloat ( MenuAction_t action, float *val, float step, float min_val, float max_val )
{
  if ( (action == ACTION_RIGHT) && ( (*val) < max_val ) )
    {
      (*val) += step;
      if ( (*val) > max_val ) (*val) = max_val;
      MoveLiftSound();
    }
  if ( (action == ACTION_LEFT) && ( (*val) > min_val ) )
    {
      (*val) -= step;
      if ( (*val) < min_val ) (*val) = min_val;
      MoveLiftSound();
    }
  return;
} // menuChangeFloat()


// ========== menu entry handler functions ====================
const char *
handle_StrictlyClassic ( MenuAction_t action )
{
  if ( action == ACTION_CLICK )
    {
      GameConfig.Droid_Talk = FALSE;
      GameConfig.ShowDecals = FALSE;
      GameConfig.TakeoverActivates = TRUE;
      GameConfig.FireHoldTakeover = TRUE;
      GameConfig.AllMapVisible = TRUE;

      // set window type
      GameConfig.FullUserRect = FALSE;
      Copy_Rect (Classic_User_Rect, User_Rect);
      // set theme
      setTheme ( classic_theme_index );
      InitiateMenu (TRUE);
      MenuItemSelectedSound();
    }

  return NULL;
}

const char *
handle_WindowType ( MenuAction_t action )
{
  if ( action == ACTION_INFO ) {
    return GameConfig.FullUserRect ? "Full" : "Classic";
  }

  if ( (action == ACTION_CLICK) || (action == ACTION_LEFT) || (action == ACTION_RIGHT) )
    {
      flipToggle ( &GameConfig.FullUserRect );
      if ( GameConfig.FullUserRect )
        Copy_Rect ( Full_User_Rect, User_Rect );
      else
        Copy_Rect ( Classic_User_Rect, User_Rect );

      InitiateMenu (TRUE);
    }
  return NULL;
} // handle_WindowType()

const char *
handle_Theme ( MenuAction_t action )
{
  if ( action == ACTION_INFO ) {
    return AllThemes.theme_name [ AllThemes.cur_tnum ];
  }

  if ( (action == ACTION_CLICK) || (action == ACTION_LEFT) || (action == ACTION_RIGHT) )
    {
      int tnum = AllThemes.cur_tnum;
      if ( (action == ACTION_CLICK) && (action == ACTION_RIGHT) )
        tnum ++;
      else
        tnum --;

      if ( tnum < 0 ) tnum = AllThemes.num_themes - 1;
      if ( tnum > AllThemes.num_themes - 1 ) tnum = 0;

      setTheme ( tnum );
      InitiateMenu (TRUE);
    }

  return NULL;
} // handle_Theme()

const char *
handle_DroidTalk ( MenuAction_t action )
{
  if ( action == ACTION_INFO ) {
    return isToggleOn ( GameConfig.Droid_Talk );
  }
  if ( (action == ACTION_CLICK) || (action == ACTION_LEFT) || (action == ACTION_RIGHT) ) {
    flipToggle ( &GameConfig.Droid_Talk );
  }
  return NULL;
}

const char *
isAllMapVisible ( void )
{

}
const char *
handle_AllMapVisible ( MenuAction_t action )
{
  if ( action == ACTION_INFO ) {
    return isToggleOn ( GameConfig.AllMapVisible );
  }
  if ( (action == ACTION_CLICK) || (action == ACTION_LEFT) || (action == ACTION_RIGHT) )
    {
      flipToggle ( &GameConfig.AllMapVisible );
      InitiateMenu (TRUE);
    }
  return NULL;
}

const char *
handle_ShowDecals ( MenuAction_t action )
{
  if ( action == ACTION_INFO ) {
    return isToggleOn ( GameConfig.ShowDecals );
  }
  if ( (action == ACTION_CLICK) || (action == ACTION_LEFT) || (action == ACTION_RIGHT) )
    {
      flipToggle ( &GameConfig.ShowDecals );
      InitiateMenu (TRUE);
    }
  return NULL;
}

const char *
handle_TransferIsActivate ( MenuAction_t action )
{
  if ( action == ACTION_INFO ) {
    return isToggleOn ( GameConfig.TakeoverActivates );
  }
  if ( (action == ACTION_CLICK) || (action == ACTION_LEFT) || (action == ACTION_RIGHT) ) {
    flipToggle ( &GameConfig.TakeoverActivates );
  }
  return NULL;
}

const char *
handle_FireIsTransfer ( MenuAction_t action )
{
  if ( action == ACTION_INFO ) {
    return isToggleOn ( GameConfig.FireHoldTakeover );
  }
  if ( (action == ACTION_CLICK) || (action == ACTION_LEFT) || (action == ACTION_RIGHT) ) {
    flipToggle ( &GameConfig.FireHoldTakeover );
  }
  return NULL;
}

const char *
handle_MusicVolume ( MenuAction_t action )
{
  static char buf[256];
  if ( action == ACTION_INFO ) {
    sprintf ( buf, "%1.2f" , GameConfig.Current_BG_Music_Volume );
    return buf;
  }

  menuChangeFloat ( action, &(GameConfig.Current_BG_Music_Volume), 0.05, 0, 1 );
  Set_BG_Music_Volume ( GameConfig.Current_BG_Music_Volume );
  return NULL;
}

const char *
handle_SoundVolume ( MenuAction_t action )
{
  static char buf[256];
  if ( action == ACTION_INFO ) {
    sprintf ( buf, "%1.2f" , GameConfig.Current_Sound_FX_Volume );
    return buf;
  }

  menuChangeFloat ( action, &(GameConfig.Current_Sound_FX_Volume), 0.05, 0, 1 );
  Set_Sound_FX_Volume( GameConfig.Current_Sound_FX_Volume );
  return NULL;
}

const char *
handle_Fullscreen ( MenuAction_t action )
{
  if ( action == ACTION_INFO ) {
    return isToggleOn ( GameConfig.UseFullscreen );
  }
  if ( (action == ACTION_CLICK) || (action == ACTION_LEFT) || (action == ACTION_RIGHT) ) {
      toggle_fullscreen();
      MenuItemSelectedSound();
  }
  return NULL;
}



const char *
handle_ShowPosition ( MenuAction_t action )
{
  if ( action == ACTION_INFO ) {
    return isToggleOn ( GameConfig.Draw_Position );
  }
  if ( (action == ACTION_CLICK) || (action == ACTION_LEFT) || (action == ACTION_RIGHT) ) {
    flipToggle ( &GameConfig.Draw_Position );
    InitiateMenu (TRUE);
  }
  return NULL;
}
const char *
handle_ShowFramerate ( MenuAction_t action )
{
  if ( action == ACTION_INFO ) {
    return isToggleOn ( GameConfig.Draw_Framerate );
  }
  if ( (action == ACTION_CLICK) || (action == ACTION_LEFT) || (action == ACTION_RIGHT) ) {
    flipToggle ( &GameConfig.Draw_Framerate );
    InitiateMenu (TRUE);
  }
  return NULL;
}
const char *
handle_ShowEnergy ( MenuAction_t action )
{
  if ( action == ACTION_INFO ) {
    return isToggleOn ( GameConfig.Draw_Energy );
  }
  if ( (action == ACTION_CLICK) || (action == ACTION_LEFT) || (action == ACTION_RIGHT) ) {
    flipToggle ( &GameConfig.Draw_Energy );
    InitiateMenu (TRUE);
  }
  return NULL;
}
const char *
handle_ShowDeathCount ( MenuAction_t action )
{
  if ( action == ACTION_INFO ) {
    return isToggleOn ( GameConfig.Draw_DeathCount );
  }
  if ( (action == ACTION_CLICK) || (action == ACTION_LEFT) || (action == ACTION_RIGHT) ) {
    flipToggle ( &GameConfig.Draw_DeathCount );
    InitiateMenu (TRUE);
  }
  return NULL;
}

const char *handle_LevelEditor ( MenuAction_t action )
{
  if ( action == ACTION_CLICK || action == ACTION_RIGHT ) {
    LevelEditor();
  }
  return NULL;
}
const char *handle_ShowHighscores ( MenuAction_t action )
{
  if ( action == ACTION_CLICK || action == ACTION_RIGHT ) {
    ShowHighscores();
  }
  return NULL;
}
const char *handle_Credits_Menu ( MenuAction_t action )
{
  if ( action == ACTION_CLICK || action == ACTION_RIGHT ) {
    Credits_Menu();
  }
  return NULL;
}
const char *handle_Key_Config_Menu ( MenuAction_t action )
{
  if ( action == ACTION_CLICK || action == ACTION_RIGHT ) {
    Key_Config_Menu();
  }
  return NULL;
}
const char *handle_QuitGameMenu ( MenuAction_t action )
{
  if ( action == ACTION_CLICK || action == ACTION_RIGHT ) {
    QuitGameMenu();
  }
  return NULL;
}

// ========== Function definitions ==========

// simple wrapper to ShowMenu() to provide the external entry point into the main menu
void showMainMenu (void)
{
  return ShowMenu ( MainMenu );
} // showMainMenu()

/*@Function============================================================
@Desc: This function prepares the screen for a menu and
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
#ifndef ANDROID
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

#endif // ANDROID
} // QuitGameMenu()


//
// Generic menu handler
//
void
ShowMenu ( const MenuEntry_t MenuEntries[] )
{
  bool finished = FALSE;
  int menu_pos = 0;
  int num_entries = 0;
  while ( MenuEntries[num_entries].name != NULL ) { num_entries ++; }

  InitiateMenu ( TRUE );

  while ( !finished )
    {
      int i;
      const char* (*handler)( MenuAction_t action ) = MenuEntries[menu_pos].handler;
      const MenuEntry_t *submenu = MenuEntries[menu_pos].submenu;

      SDL_BlitSurface (Menu_Background, NULL, ne_screen, NULL);

      // print menu
      for ( i = 0; i < num_entries; i ++ )
        {
          char fullName[256];
          const char *arg = NULL;
          if ( MenuEntries[i].handler ) {
            arg = (*MenuEntries[i].handler)( ACTION_INFO );
          }
          sprintf ( fullName, "%s%s", MenuEntries[i].name, (arg == NULL) ? "" : arg );
          PutString (ne_screen, OptionsMenu_Rect.x, Menu_Rect.y + i * fheight, fullName );
        }

      PutInfluence (Menu_Rect.x, Menu_Rect.y + (menu_pos - 0.5) * fheight);
      SDL_Flip( ne_screen );

      MenuAction_t action = ACTION_NONE;
      while ( action == ACTION_NONE )
	{

	  SDL_Delay (1);
          if ( EscapePressedR() ) 	action = ACTION_BACK;
          if ( MenuChooseR() )		action = ACTION_CLICK;
          if ( MenuUpR() )		action = ACTION_UP;
          if ( MenuDownR() )		action = ACTION_DOWN;
          if ( MenuRightR() )		action = ACTION_RIGHT;
          if ( MenuLeftR() )		action = ACTION_LEFT;

          switch ( action )
            {
            case ACTION_BACK:
	      finished = TRUE;
              break;

            case ACTION_CLICK:
              if ( !handler && !submenu ) {
                finished = TRUE;
                break;
              }
              if ( handler ) {
                (*handler)(action);
              }
              if ( submenu ) {
                ShowMenu ( submenu );
                InitiateMenu (TRUE);
              }
              break;

            case ACTION_RIGHT:
            case ACTION_LEFT:
              if ( handler ) {
                (*handler)(action);
              }
              break;

            case ACTION_UP:
	      if (menu_pos > 0) {
                menu_pos--;
              } else {
                menu_pos = num_entries - 1;
              }
	      MoveMenuPositionSound();
              break;

            case ACTION_DOWN:
	      if ( menu_pos < num_entries - 1 ) {
                menu_pos++;
              } else {
                menu_pos = 0;
              }
	      MoveMenuPositionSound();
              break;
            } // switch(action)
	} // while (action == ACTION_NONE)

    } // while !finished

  ClearGraphMem();
  SDL_ShowCursor ( SDL_ENABLE );  // reactivate mouse-cursor for game
  // Since we've faded out the whole scren, it can't hurt
  // to have the top status bar redrawn...
  BannerIsDestroyed = TRUE;
  Me.status = MOBILE;

  return;

} // ShowMenu()

/*@Function============================================================
@Desc: This function provides the credits screen.  It is a submenu of
       the big MainMenu.  Here you can see who helped developing the
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
