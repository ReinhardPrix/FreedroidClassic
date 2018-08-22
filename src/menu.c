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
typedef struct MenuEntry_s
{
  const char *name;			/**< menu entry string */
  const char *(*handler)();		/**< handler & info function for this menu entry (none if NULL) */
  const struct MenuEntry_s *submenu; 	/**< enter this submenu (if non-NULL) */
} MenuEntry_t;


// ----- local prototypes ----------
void Key_Config_Menu (void);
void Display_Key_Config (int selx, int sely);
void ShowCredits (void);

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

const char *handle_OpenLevelEditor ( MenuAction_t action );
const char *handle_Highscores ( MenuAction_t action );
const char *handle_Credits ( MenuAction_t action );
const char *handle_ConfigureKeys ( MenuAction_t action );
const char *handle_QuitGame ( MenuAction_t action );

const char *handle_LE_Exit ( MenuAction_t action );
const char *handle_LE_LevelNumber ( MenuAction_t action );
const char *handle_LE_Color ( MenuAction_t action );
const char *handle_LE_SizeX ( MenuAction_t action );
const char *handle_LE_SizeY ( MenuAction_t action );
const char *handle_LE_Name ( MenuAction_t action );
const char *handle_LE_Music ( MenuAction_t action );
const char *handle_LE_Comment ( MenuAction_t action );
const char *handle_LE_SaveShip ( MenuAction_t action );

void setTheme ( int theme_index );
const char *isToggleOn ( int toggle );
void flipToggle ( int *toggle );
void menuChangeFloat ( MenuAction_t action, float *val, float step, float min_val, float max_val );
void menuChangeInt ( MenuAction_t action, int *val, int step, int min_val, int max_val );

// ----- define menus ----------
MenuEntry_t LegacyMenu[] =
  {
    { "Back",			NULL,			NULL },
    { "Set Strictly Classic",	handle_StrictlyClassic,	NULL },
    { "Combat Window: ", 	handle_WindowType, 	NULL },
    { "Graphics Theme: ", 	handle_Theme,		NULL },
    { "Droid Talk: ", 		handle_DroidTalk,	NULL },
    { "Show Decals: ", 		handle_ShowDecals,	NULL },
    { "All Map Visible: ", 	handle_AllMapVisible,	NULL },
#ifndef ANDROID
    { "Transfer = Activate: ", 	handle_TransferIsActivate, NULL },
    { "Hold Fire to Transfer: ",handle_FireIsTransfer,	NULL },
#endif
    { NULL,			NULL, 			NULL }
  };

MenuEntry_t GraphicsSoundMenu[] =
  {
    { "Back", 			NULL,			NULL },
    { "Music Volume: ", 	handle_MusicVolume,	NULL },
    { "Sound Volume: ", 	handle_SoundVolume,	NULL },
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

MenuEntry_t LevelEditorMenu[] =
{
  { "Exit Level Editor", 	handle_LE_Exit,         NULL },
  { "Current Level: ", 		handle_LE_LevelNumber,	NULL },
  { "Level Color: ",		handle_LE_Color,	NULL },
  { "Levelsize X: ",		handle_LE_SizeX,	NULL },
  { "Levelsize Y: ",		handle_LE_SizeY,	NULL },
  { "Level Name: ", 		handle_LE_Name,		NULL },
  //{ "Background Music: ",	handle_LE_Music,	NULL },
  //{ "Level Comment: ",	handle_LE_Comment,	NULL },
  { "Save ship: ",              handle_LE_SaveShip,	NULL },
  { NULL,			NULL, 			NULL }
};

MenuEntry_t MainMenu[] =
  {
    { "Back to Game", 		NULL, 				NULL },
    { "Graphics & Sound", 	NULL, 				GraphicsSoundMenu },
    { "Legacy Options", 	NULL,				LegacyMenu },
    { "HUD Settings",		NULL, 				HUDMenu },
#ifndef ANDROID
    { "Level Editor",		handle_OpenLevelEditor,		NULL },
#endif
    { "Highscores", 		handle_Highscores, 		NULL },
    { "Credits", 		handle_Credits, 		NULL },
#ifndef ANDROID
    { "Configure Keys", 	handle_ConfigureKeys, 		NULL },
#endif
    { "Quit Game",		handle_QuitGame, 		NULL },
    { NULL,			NULL, 				NULL }
  };


const char *
isToggleOn ( int toggle )
{
  return toggle ? "YES" : "NO";
}
void
flipToggle ( int *toggle )
{
  if (toggle) {
    //MoveLiftSound();
    MenuItemSelectedSound();
    (*toggle) = !(*toggle);
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
      MoveLiftSound();
      (*val) += step;
      if ( (*val) > max_val ) (*val) = max_val;
    }
  if ( (action == ACTION_LEFT) && ( (*val) > min_val ) )
    {
      MoveLiftSound();
      (*val) -= step;
      if ( (*val) < min_val ) (*val) = min_val;
    }
  return;
} // menuChangeFloat()
void
menuChangeInt ( MenuAction_t action, int *val, int step, int min_val, int max_val )
{
  float float_val = (float)(*val);
  menuChangeFloat ( action, &float_val, step, min_val, max_val );
  (*val) = (int)round(float_val);
  return;
} // menuChangeInt()


// ========== menu entry handler functions ====================
const char *
handle_StrictlyClassic ( MenuAction_t action )
{
  if ( action == ACTION_CLICK )
    {
      MenuItemSelectedSound();
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
      MoveLiftSound();
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

const char *handle_OpenLevelEditor ( MenuAction_t action )
{
  if ( action == ACTION_CLICK ) {
    MenuItemSelectedSound();
    LevelEditor();
  }
  return NULL;
}

const char *handle_LE_Exit ( MenuAction_t action )
{
  if ( action == ACTION_CLICK )
    {
      MenuItemSelectedSound();
      quit_LevelEditor = TRUE;
      quit_Menu = TRUE;
    }
  return NULL;
}

const char *handle_LE_LevelNumber ( MenuAction_t action )
{
  static char buf[256];
  if ( action == ACTION_INFO ) {
    sprintf ( buf, "%d" , CurLevel->levelnum );
    return buf;
  }

  int curlevel = CurLevel->levelnum;
  menuChangeInt ( action, &curlevel, 1, 0, curShip.num_levels -1 );
  Teleport ( curlevel, 3 , 3 );
  Switch_Background_Music_To ( BYCOLOR );
  InitiateMenu(FALSE);

  return NULL;
}

const char *handle_LE_Color ( MenuAction_t action )
{
  if ( action == ACTION_INFO ) {
    return ColorNames[CurLevel->color];
  }
  menuChangeInt ( action, &(CurLevel->color), 1, 0, numLevelColors-1 );
  Switch_Background_Music_To ( BYCOLOR );
  InitiateMenu(FALSE);

  return NULL;
}

const char *handle_LE_SizeX ( MenuAction_t action )
{
  static char buf[256];
  if ( action == ACTION_INFO ) {
    sprintf ( buf, "%d" , CurLevel->xlen );
    return buf;
  }

  int oldxlen = CurLevel->xlen;
  menuChangeInt ( action, &(CurLevel->xlen), 1, 0, MAX_MAP_COLS-1 );
  size_t newmem = CurLevel->xlen * sizeof( CurLevel->map[0][0] );
  // adjust memory sizes for new value
  int row;
  for ( row = 0 ; row < CurLevel->ylen ; row++ )
    {
      CurLevel->map[row] = realloc( CurLevel->map[row], newmem );
      if ( CurLevel->map[row] == NULL ) {
        DebugPrintf ( 0, "Failed to re-allocate to %z bytes in map row %d\n", newmem, row );
        Terminate(ERR);
      }
      if ( CurLevel->xlen > oldxlen ) { // fill new map area with VOID
        CurLevel->map[ row ] [ CurLevel->xlen - 1 ] = VOID;
      }
    }
  InitiateMenu(FALSE);
  return NULL;
}

const char *handle_LE_SizeY ( MenuAction_t action )
{
  static char buf[256];
  if ( action == ACTION_INFO ) {
    sprintf ( buf, "%d" , CurLevel->ylen );
    return buf;
  }

  int oldylen = CurLevel->ylen;
  menuChangeInt ( action, &(CurLevel->ylen), 1, 0, MAX_MAP_ROWS-1 );
  if ( oldylen > CurLevel->ylen )
    {
      free ( CurLevel->map[oldylen - 1] );
      CurLevel->map[oldylen - 1] = NULL;
    }
  else if ( oldylen < CurLevel->ylen )
    {
      CurLevel->map[ CurLevel->ylen-1 ] = MyMalloc ( CurLevel->xlen * sizeof( CurLevel->map[0][0] ) );
      memset ( CurLevel->map[ CurLevel->ylen-1 ], VOID, CurLevel->xlen * sizeof( CurLevel->map[0][0] ) );
    }

  InitiateMenu (FALSE);
  return NULL;
}

const char *handle_LE_Name ( MenuAction_t action )
{
  if ( action == ACTION_INFO ) {
    return CurLevel->Levelname;
  }

  if ( action == ACTION_CLICK )
    {
      DisplayText ("New level name: ", Menu_Rect.x-2*fheight, Menu_Rect.y - 3*fheight, &Full_User_Rect );
      SDL_Flip( ne_screen );
      free ( CurLevel->Levelname );
      CurLevel->Levelname = GetString(15, 2);
      InitiateMenu (FALSE);
    }

  return NULL;
}

const char *handle_LE_Music ( MenuAction_t action )
{
  if ( action == ACTION_INFO ) {
    return CurLevel->Background_Song_Name;
  }

  if ( action == ACTION_CLICK )
    {
      DisplayText ("Music filename: ", Menu_Rect.x - 2*fheight, Menu_Rect.y - 3*fheight, &Full_User_Rect);
      SDL_Flip( ne_screen );
      free ( CurLevel->Background_Song_Name );
      CurLevel->Background_Song_Name = GetString(20, 2);
      Switch_Background_Music_To ( CurLevel->Background_Song_Name );
    }

  return NULL;
}

const char *handle_LE_Comment ( MenuAction_t action )
{
  if ( action == ACTION_INFO ) {
    return CurLevel->Level_Enter_Comment;
  }
  return NULL;
}

const char *handle_LE_SaveShip ( MenuAction_t action )
{
  const char *shipname = "Testship";
  static char fname[255];
  snprintf ( fname, sizeof(fname)-1, "%s%s", shipname, SHIP_EXT );
  if ( action == ACTION_INFO ) {
    return fname;
  }
  if ( action == ACTION_CLICK )
    {
      SaveShip( shipname );
      char output[255];
      snprintf ( output, sizeof(output)-1, "Ship saved as '%s'", fname );
      CenteredPutString (ne_screen, 3*FontHeight(Menu_BFont), output );
      SDL_Flip ( ne_screen );
      wait_for_key_pressed();
      InitiateMenu (FALSE);
    }

  return NULL;
}

const char *handle_Highscores ( MenuAction_t action )
{
  if ( action == ACTION_CLICK ) {
    MenuItemSelectedSound();
    ShowHighscores();
  }
  return NULL;
}
const char *handle_Credits ( MenuAction_t action )
{
  if ( action == ACTION_CLICK ) {
    MenuItemSelectedSound();
    ShowCredits();
  }

  return NULL;

} // handle_Credits()

const char *handle_ConfigureKeys ( MenuAction_t action )
{
  if ( action == ACTION_CLICK ) {
    MenuItemSelectedSound();
    Key_Config_Menu();
  }
  return NULL;
}
const char *handle_QuitGame ( MenuAction_t action )
{
  if ( action != ACTION_CLICK ) {
    return NULL;
  }

  MenuItemSelectedSound();
  InitiateMenu (TRUE);

#ifdef GCW0
  PutString (ne_screen, User_Rect.x + User_Rect.w/3,
             User_Rect.y + User_Rect.h/2, "Press A to quit");
#else
  PutString (ne_screen, User_Rect.x + User_Rect.w/5,
             User_Rect.y + User_Rect.h/2, "Hit 'y' or press Fire to quit");
#endif
  SDL_Flip (ne_screen);
#ifdef GCW0
  while ( (!Gcw0AnyButtonPressed()) ) SDL_Delay(1);
  if ( (Gcw0APressed()) ) {
    while ( (!Gcw0AnyButtonPressedR()) ) SDL_Delay(1); // In case FirePressed && !Gcw0APressed() -> would cause a loop otherwise in the menu...
    Terminate (OK);
  }
#else
  wait_for_all_keys_released();
  int key = wait_for_key_pressed();
  if ( (key == 'y') || (key == key_cmds[CMD_FIRE][0]) || (key == key_cmds[CMD_FIRE][1]) || (key == key_cmds[CMD_FIRE][2]) ) {
    Terminate (OK);
  }
#endif

  return NULL;
} // handle_QuitGame()

// ========== Function definitions ==========

// simple wrapper to ShowMenu() to provide the external entry point into the main menu
void showMainMenu (void)
{
  ShowMenu ( MainMenu );
} // showMainMenu()

// simple wrapper to ShowMenu() to provide the external entry point into the Level Editor menu
void
showLevelEditorMenu (void)
{
  quit_LevelEditor = FALSE;
  ShowMenu ( LevelEditorMenu );
}

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
// get menu input actions
// NOTE: built-in time delay to ensure spurious key-repetitions
// such as from touchpad 'wheel' or android joystic emulation
// don't create unexpected menu movements:
// ==> ignore all movement commands withing delay_ms milliseconds of each other
MenuAction_t
getMenuAction ( Uint32 wait_repeat_ticks )
{
  // 'normal' menu action keys get released
  if ( KeyIsPressedR ( SDLK_BACKSPACE ) ) {
    return ACTION_DELETE;
  }
  if ( FirePressedR() || ReturnPressedR() || SpacePressedR() ) {
    return ACTION_CLICK;
  }
  if ( cmd_is_activeR(CMD_BACK) || KeyIsPressedR(SDLK_ESCAPE) ) {
    return ACTION_BACK;
  }

  // ----- up/down motion: allow for key-repeat, but carefully control repeat rate (modelled on takeover game)
  static Uint32 last_movekey_time = 0;

  static int up    = FALSE;
  static int down  = FALSE;
  static int left  = FALSE;
  static int right = FALSE;

  // we register if there have been key-press events in the "waiting period" between move-ticks
  if ( !up && (UpPressed () || KeyIsPressed(SDLK_UP)) )
    {
      up = TRUE;
      last_movekey_time = SDL_GetTicks();
      return ACTION_UP;
    }
  if (!down && (DownPressed() || KeyIsPressed(SDLK_DOWN)) )
    {
      down = TRUE;
      last_movekey_time = SDL_GetTicks();
      return ACTION_DOWN;
    }
  if ( !left && (LeftPressed() || KeyIsPressed(SDLK_LEFT)) )
    {
      left = TRUE;
      last_movekey_time = SDL_GetTicks();
      return ACTION_LEFT;
    }
  if ( !right && (RightPressed() || KeyIsPressed(SDLK_RIGHT)) )
    {
      right = TRUE;
      last_movekey_time = SDL_GetTicks();
      return ACTION_RIGHT;
    }

  if (! (UpPressed()   || KeyIsPressed(SDLK_UP)))    { up   = FALSE; }
  if (! (DownPressed() || KeyIsPressed(SDLK_DOWN)))  { down = FALSE; }
  if (! (LeftPressed() || KeyIsPressed(SDLK_LEFT)))  { left = FALSE; }
  if (! (RightPressed()|| KeyIsPressed(SDLK_RIGHT))) { right= FALSE; }

  // check if enough time since we registered last new move-action
  if ( SDL_GetTicks() - last_movekey_time > wait_repeat_ticks )
    {
      if ( up ) {
        return ACTION_UP;
      }
      if ( down ) {
        return ACTION_DOWN;
      }
      if ( left ) {
        return ACTION_LEFT;
      }
      if ( right ) {
        return ACTION_RIGHT;
      }
    }

  return ACTION_NONE;

} // getMenuAction()


//
// Generic menu handler
//
void
ShowMenu ( const MenuEntry_t MenuEntries[] )
{
  int menu_pos = 0;
  int num_entries = 0;
  while ( MenuEntries[num_entries].name != NULL ) { num_entries ++; }

  InitiateMenu ( FALSE );
  while ( any_key_is_pressedR() ) // wait for all key/controller-release
    SDL_Delay(1);

  MenuAction_t action = ACTION_NONE;
  const Uint32 wait_move_ticks = 100;
  static Uint32 last_move_tick = 0;
  bool finished = FALSE;
  quit_Menu = FALSE;
  bool need_update = TRUE;
  while ( !finished )
    {
      const char* (*handler)( MenuAction_t action ) = MenuEntries[menu_pos].handler;
      const MenuEntry_t *submenu = MenuEntries[menu_pos].submenu;

      if ( need_update )
        {
          SDL_BlitSurface (Menu_Background, NULL, ne_screen, NULL);
          // print menu
          int i;
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
          need_update = FALSE;
        }

      action = getMenuAction( 250 );
      bool allow_move = ( SDL_GetTicks() - last_move_tick > wait_move_ticks );
      switch ( action )
        {
        case ACTION_BACK:
          finished = TRUE;
          break;

        case ACTION_CLICK:
          if ( !handler && !submenu ) {
            MenuItemSelectedSound();
            finished = TRUE;
            break;
          }
          if ( handler ) {
            wait_for_all_keys_released();
            (*handler)(action);
          }
          if ( submenu ) {
            MenuItemSelectedSound();
            wait_for_all_keys_released();
            ShowMenu ( submenu );
            InitiateMenu (FALSE);
          }
          need_update = TRUE;
          break;

        case ACTION_RIGHT:
        case ACTION_LEFT:
          if ( !allow_move ) {
            continue;
          }
          if ( handler ) {
            (*handler)(action);
          }
          last_move_tick = SDL_GetTicks();
          need_update = TRUE;
          break;

        case ACTION_UP:
          if ( !allow_move ) {
            continue;
          }
          MoveMenuPositionSound();
          if (menu_pos > 0) {
            menu_pos--;
          } else {
            menu_pos = num_entries - 1;
          }
          last_move_tick = SDL_GetTicks();
          need_update = TRUE;
          break;

        case ACTION_DOWN:
          if ( !allow_move ) {
            continue;
          }
          MoveMenuPositionSound();
          if ( menu_pos < num_entries - 1 ) {
            menu_pos++;
          } else {
            menu_pos = 0;
          }
          last_move_tick = SDL_GetTicks();
          need_update = TRUE;
          break;

        default:
          break;
        } // switch(action)

      if ( quit_Menu ) {
        finished = TRUE;
      }

      SDL_Delay(1);	// don't hog CPU
    } // while !finished

  ClearGraphMem();
  SDL_ShowCursor ( SDL_ENABLE );  // reactivate mouse-cursor for game
  // Since we've faded out the whole scren, it can't hurt
  // to have the top status bar redrawn...
  BannerIsDestroyed = TRUE;
  Me.status = MOBILE;

  while ( any_key_is_pressedR() ) // wait for all key/controller-release
    SDL_Delay(1);

  return;

} // ShowMenu()

/*@Function============================================================
@Desc: This function provides the credits screen.  It is a submenu of
       the big MainMenu.  Here you can see who helped developing the
       game, currently jp, rp and bastian.

@Ret:  none
* $Function----------------------------------------------------------*/
void
ShowCredits (void)
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

  wait_for_key_pressed();

  SetCurrentFont (oldfont);

  return;

} // Show_Credits()

// ======================================================================

// ------------------------------------------------------------
// show/edit keyboard-config
// ------------------------------------------------------------
void
Key_Config_Menu (void)
{
  int LastMenuPos = CMD_LAST;
  int selx = 1, sely = 1;   // currently selected menu-position
  int oldkey, newkey = -1;
  MenuAction_t action = ACTION_NONE;
  const Uint32 wait_move_ticks = 100;
  static Uint32 last_move_tick = 0;

  bool finished = FALSE;
  while (!finished)
    {
      Display_Key_Config (selx, sely);

      action = getMenuAction( 250 );
      if ( SDL_GetTicks() - last_move_tick > wait_move_ticks )
        {
          switch ( action )
            {
            case ACTION_BACK:
              finished = TRUE;
              break;

            case ACTION_CLICK:
              MenuItemSelectedSound();

              oldkey = key_cmds[sely-1][selx-1];
              key_cmds[sely-1][selx-1] = '_';
              Display_Key_Config (selx, sely);
              newkey = getchar_raw(); // includes joystick input!
              key_cmds[sely-1][selx-1] = newkey;
              while ( any_key_is_pressedR() ) // wait for key/controller-release
                SDL_Delay(1);
              break;

            case ACTION_UP:
              if ( sely > 1 ) sely--;
              else sely = LastMenuPos;
              MoveMenuPositionSound();
              last_move_tick = SDL_GetTicks();
              break;

            case ACTION_DOWN:
              if ( sely < LastMenuPos ) sely++;
              else sely = 1;
              MoveMenuPositionSound();
              last_move_tick = SDL_GetTicks();
              break;

            case ACTION_RIGHT:
              if ( selx < 3 ) selx++;
              else selx = 1;
              MoveMenuPositionSound();
              last_move_tick = SDL_GetTicks();
              break;

            case ACTION_LEFT:
              if ( selx > 1 ) selx--;
              else selx = 3;
              MoveMenuPositionSound();
              last_move_tick = SDL_GetTicks();
              break;

            case ACTION_DELETE:
              key_cmds[sely-1][selx-1] = 0;
              MenuItemSelectedSound();
              break;
            default:
              break;
            } // switch(action)
        } // if now - last_move_tick > wait_move_ticks
      SDL_Delay(1);
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
  int lheight = FontHeight (Font0_BFont) + 2;

  SDL_BlitSurface (Menu_Background, NULL, ne_screen, NULL);

  //      PutInfluence (startx - 1.1*Block_Rect.w, starty + (MenuPosition-1.5)*lheight);
  //PrintStringFont (ne_screen, (sely==1)? Font2_BFont:Font1_BFont, startx, starty+(posy++)*lheight, "Back");
#ifdef GCW0
  PrintStringFont (ne_screen, Font0_BFont, col1, starty, "(RShldr to clear an entry)");
#else
  PrintStringFont (ne_screen, Font0_BFont, col1, starty, "(Backspace to clear an entry)");
#endif
  posy++;
  PrintStringFont (ne_screen, Font0_BFont, startx, starty + (posy)*lheight, "Command");
  PrintStringFont (ne_screen, Font0_BFont, col1,   starty + (posy)*lheight, "Key1");
  PrintStringFont (ne_screen, Font0_BFont, col2,   starty + (posy)*lheight, "Key2");
  PrintStringFont (ne_screen, Font0_BFont, col3,   starty + (posy)*lheight, "Key3");
  posy ++;

  int i;
  for (i=0; i < CMD_LAST; i++)
    {
      PrintStringFont (ne_screen, Font0_BFont,  startx, starty+(posy)*lheight, cmd_strings[i]);
      PrintStringFont (ne_screen, PosFont(1,1+i), col1, starty+(posy)*lheight, keystr[key_cmds[i][0]]);
      PrintStringFont (ne_screen, PosFont(2,1+i), col2, starty+(posy)*lheight, keystr[key_cmds[i][1]]);
      PrintStringFont (ne_screen, PosFont(3,1+i), col3, starty+(posy)*lheight, keystr[key_cmds[i][2]]);
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
  int x0, y0;
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
