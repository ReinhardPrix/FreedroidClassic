/* 
 *
 *   Copyright (c) 1994, 2002, 2003 Johannes Prix
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
/* ----------------------------------------------------------------------
 * This file contains (all) functions for keyboard and joystick handling
 * ---------------------------------------------------------------------- */

#define _input_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"

#ifndef SDL_BUTTON_WHEELUP 
#define SDL_BUTTON_WHEELUP 4
#endif
#ifndef SDL_BUTTON_WHEELDOWN
#define SDL_BUTTON_WHEELDOWN 5
#endif

point CurrentMouseAbsPos;

SDL_Event event;

int CurrentlyMouseRightPressed=0;
int CurrentlyMouseLeftPressed=0;
int CurrentlyMouseWheelUpPressed=0;
int CurrentlyMouseWheelDownPressed=0;
int MouseWheelUpMovesRecorded=0;
int MouseWheelDownMovesRecorded=0;
int ShiftWasPressedInAddition=FALSE;
int LeftShiftWasPressedInAddition=FALSE;
int RightShiftWasPressedInAddition=FALSE;
int CtrlWasPressedInAddition=FALSE;
int LeftCtrlWasPressedInAddition=FALSE;
int RightCtrlWasPressedInAddition=FALSE;
int AltWasPressedInAddition=FALSE;
int LeftAltWasPressedInAddition=FALSE;
int RightAltWasPressedInAddition=FALSE;
int CurrentlyEnterPressed=0;
int CurrentlySpacePressed=0;
int CurrentlyLeftPressed=0;
int CurrentlyRightPressed=0;
int CurrentlyUpPressed=0;
int CurrentlyDownPressed=0;
int Currently0Pressed=0;
int Currently1Pressed=0;
int Currently2Pressed=0;
int Currently3Pressed=0;
int Currently4Pressed=0;
int Currently5Pressed=0;
int Currently6Pressed=0;
int Currently7Pressed=0;
int Currently8Pressed=0;
int Currently9Pressed=0;
int CurrentlyAPressed=0;
int CurrentlyBPressed=0;
int CurrentlyCPressed=0;
int CurrentlyDPressed=0;
int CurrentlyEPressed=0;
int CurrentlyFPressed=0;
int CurrentlyGPressed=0;
int CurrentlyHPressed=0;
int CurrentlyIPressed=0;
int CurrentlyJPressed=0;
int CurrentlyKPressed=0;
int CurrentlyLPressed=0;
int CurrentlyMPressed=0;
int CurrentlyNPressed=0;
int CurrentlyOPressed=0;
int CurrentlyPPressed=0;
int CurrentlyQPressed=0;
int CurrentlyRPressed=0;
int CurrentlySPressed=0;
int CurrentlyTPressed=0;
int CurrentlyUPressed=0;
int CurrentlyVPressed=0;
int CurrentlyWPressed=0;
int CurrentlyXPressed=0;
int CurrentlyYPressed=0;
int CurrentlyZPressed=0;
int CurrentlyKP_PLUS_Pressed=0;
int CurrentlyKP_MINUS_Pressed=0;
int CurrentlyKP_MULTIPLY_Pressed=0;
int CurrentlyKP_DIVIDE_Pressed=0;
int CurrentlyKP_ENTER_Pressed=0;
int CurrentlyKP0Pressed=0;
int CurrentlyKP1Pressed=0;
int CurrentlyKP2Pressed=0;
int CurrentlyKP3Pressed=0;
int CurrentlyKP4Pressed=0;
int CurrentlyKP5Pressed=0;
int CurrentlyKP6Pressed=0;
int CurrentlyKP7Pressed=0;
int CurrentlyKP8Pressed=0;
int CurrentlyKP9Pressed=0;
int CurrentlyF1Pressed=0;
int CurrentlyF2Pressed=0;
int CurrentlyF3Pressed=0;
int CurrentlyF4Pressed=0;
int CurrentlyF5Pressed=0;
int CurrentlyF6Pressed=0;
int CurrentlyF7Pressed=0;
int CurrentlyF8Pressed=0;
int CurrentlyF9Pressed=0;
int CurrentlyF10Pressed=0;
int CurrentlyF11Pressed=0;
int CurrentlyF12Pressed=0;
int CurrentlyEscapePressed=0;
int CurrentlyTabPressed=0;
int CurrentlyShiftPressed=0;
int CurrentlyBackspacePressed=0;
int CurrentlyLeftCtrlPressed=0;

void 
UnsetAllKeys( void )
{
    CurrentlyEnterPressed=0;
    CurrentlySpacePressed=0;
    CurrentlyLeftPressed=0;
    CurrentlyRightPressed=0;
    CurrentlyUpPressed=0;
    CurrentlyDownPressed=0;
    Currently0Pressed=0;
    Currently1Pressed=0;
    Currently2Pressed=0;
    Currently3Pressed=0;
    Currently4Pressed=0;
    Currently5Pressed=0;
    Currently6Pressed=0;
    Currently7Pressed=0;
    Currently8Pressed=0;
    Currently9Pressed=0;
    CurrentlyAPressed=0;
    CurrentlyBPressed=0;
    CurrentlyCPressed=0;
    CurrentlyDPressed=0;
    CurrentlyEPressed=0;
    CurrentlyFPressed=0;
    CurrentlyGPressed=0;
    CurrentlyHPressed=0;
    CurrentlyIPressed=0;
    CurrentlyJPressed=0;
    CurrentlyKPressed=0;
    CurrentlyLPressed=0;
    CurrentlyMPressed=0;
    CurrentlyNPressed=0;
    CurrentlyOPressed=0;
    CurrentlyPPressed=0;
    CurrentlyQPressed=0;
    CurrentlyRPressed=0;
    CurrentlySPressed=0;
    CurrentlyTPressed=0;
    CurrentlyUPressed=0;
    CurrentlyVPressed=0;
    CurrentlyWPressed=0;
    CurrentlyXPressed=0;
    CurrentlyYPressed=0;
    CurrentlyZPressed=0;
    CurrentlyKP_PLUS_Pressed=0;
    CurrentlyKP_MINUS_Pressed=0;
    CurrentlyKP_MULTIPLY_Pressed=0;
    CurrentlyKP_DIVIDE_Pressed=0;
    CurrentlyKP_ENTER_Pressed=0;
    CurrentlyKP0Pressed=0;
    CurrentlyKP1Pressed=0;
    CurrentlyKP2Pressed=0;
    CurrentlyKP3Pressed=0;
    CurrentlyKP4Pressed=0;
    CurrentlyKP5Pressed=0;
    CurrentlyKP6Pressed=0;
    CurrentlyKP7Pressed=0;
    CurrentlyKP8Pressed=0;
    CurrentlyKP9Pressed=0;
    CurrentlyF1Pressed=0;
    CurrentlyF2Pressed=0;
    CurrentlyF3Pressed=0;
    CurrentlyF4Pressed=0;
    CurrentlyF5Pressed=0;
    CurrentlyF6Pressed=0;
    CurrentlyF7Pressed=0;
    CurrentlyF8Pressed=0;
    CurrentlyF9Pressed=0;
    CurrentlyF10Pressed=0;
    CurrentlyF11Pressed=0;
    CurrentlyF12Pressed=0;
    CurrentlyEscapePressed=0;
    CurrentlyBackspacePressed=0;
    CurrentlyLeftCtrlPressed=0;
    CurrentlyShiftPressed=0;
}; // void UnsetAllKeys( void )

// grob_point ItemSizeTable[ ALL_ITEMS ];

/* ----------------------------------------------------------------------
 * This function returns the current mouse position in x direction.
 * Depending on the current shape of the mouse cursor, the real 'pointer'
 * position of the mouse cursor is returned.
 * ---------------------------------------------------------------------- */
int 
GetMousePos_x(void)
{
    if ( current_mouse_cursor_shape == MOUSE_CURSOR_CROSSHAIR_SHAPE )
	return( CurrentMouseAbsPos . x + 16 );
    else
	return( CurrentMouseAbsPos . x + 0 );
}; // int GetMousePos_x(void)
int 
GetMousePos_y(void)
{
    if ( current_mouse_cursor_shape == MOUSE_CURSOR_CROSSHAIR_SHAPE )
	return( CurrentMouseAbsPos . y + 16 );
    else
	return( CurrentMouseAbsPos . y + 0 );
}; // int GetMousePos_y(void)

/*----------------------------------------------------------------------
 * A simple signum function. Strange that this seems not to be 
 * implemented in some library already?!
 * ---------------------------------------------------------------------- */
int sgn (int x)
{
  return (x ? ((x)/abs(x)) : 0);
}

void Init_Joy (void)
{
  int num_joy;

  if (SDL_InitSubSystem (SDL_INIT_JOYSTICK) == -1)
    {
      DebugPrintf (0, "Warning: Couldn't initialize SDL-Joystick: %s\n", SDL_GetError());
      return;
    } 
  else
      DebugPrintf(1, "\nSDL Joystick initialisation successful.\n");

  DebugPrintf (1, " %d Joysticks found!\n", num_joy = SDL_NumJoysticks ());

  if (num_joy > 0)
    joy = SDL_JoystickOpen (0);

  if (joy)
    {
      DebugPrintf (1, "Identifier: %s\n", SDL_JoystickName (0));
      DebugPrintf (1, "Number of Axes: %d\n", joy_num_axes = SDL_JoystickNumAxes(joy));
      DebugPrintf (1, "Number of Buttons: %d\n", SDL_JoystickNumButtons(joy));

      /* aktivate Joystick event handling */
      SDL_JoystickEventState (SDL_ENABLE); 

    }
  else 
    joy = NULL;  /* signals that no yoystick is present */


  return;
}

/* ----------------------------------------------------------------------
 * If the cheat keys are enabled (a config option in the cheat menu), 
 * then we have to check if some are pressed and properly respond to
 * that.  This is what this function is supposed to do.
 * ---------------------------------------------------------------------- */
void
check_for_cheat_keys( void )
{
    int i, j ;
    // tux_t Zwisch_Me;

    //--------------------
    // This is a convenient way of dropping some random treasure...
    //
    if ( RPressed() && CtrlWasPressed() ) 
    {
	if ( Shift_Was_Pressed() )
	    DropRandomItem( Me [ 0 ] . pos . z , Me [ 0 ] . pos . x , Me [ 0 ] . pos . y , 3 , TRUE  , FALSE , FALSE ) ;
	else
	    DropRandomItem( Me [ 0 ] . pos . z , Me [ 0 ] . pos . x , Me [ 0 ] . pos . y , 3 , FALSE , FALSE , FALSE ) ;
	
	//--------------------
	// here our cheat key for immediately forcing respawning of the
	// enemies on this level...
	//
	if ( Alt_Was_Pressed() )
	{
	    respawn_level ( Me [ 0 ] . pos . z );
	}
	
	while ( RPressed() );
    }
    //--------------------
    // For identifying items, there is a quick way too...
    //
    if ( IPressed() && Shift_Was_Pressed() )
    {
	for ( i = 0 ; i < MAX_ITEMS_IN_INVENTORY ; i ++ ) Me[0].Inventory[i].is_identified = TRUE;
	Me[0].weapon_item.is_identified = TRUE;
	Me[0].shield_item.is_identified = TRUE;
	Me[0].armour_item.is_identified = TRUE;
	Me[0].special_item.is_identified = TRUE;
	Me[0].aux1_item.is_identified = TRUE;
	Me[0].aux2_item.is_identified = TRUE;
	Me[0].drive_item.is_identified = TRUE;
	while ( IPressed() );
    }
    //--------------------
    // Now some cheat keys to quickly rise in level...
    //
    if ( KP0Pressed() )
    {
	while (KP0Pressed());
	Me[0].Experience-=1000;
    }
    if ( KP1Pressed() )
    {
	while (KP1Pressed());
	Me[0].Experience+=1000;
    }
    if ( KP2Pressed() )
    {
	while (KP2Pressed());
	Me[0].Experience *= 2;
    }
    //--------------------
    // To debug the rising in levels and the addition of points
    // I added a feature to quickly gain experience points and
    // levels via the numbers 0 , 1 and 2 on the numerical keyboard.
    // 
    if ( KP9Pressed() )
    {
	while ( KP9Pressed() );
	if ( Shift_Was_Pressed() )
	    Me [ 0 ] . base_strength += 5 ;
	else
	{
	    if ( Me [ 0 ] . melee_weapon_skill < 8 )  Me [ 0 ] . melee_weapon_skill ++ ;
	}
    }
    if ( KP8Pressed() )
    {
	while (KP8Pressed());
	if ( Shift_Was_Pressed() )
	    Me [ 0 ] . base_magic += 5 ;
	else
	{
	    if ( Me [ 0 ] . ranged_weapon_skill < 8 ) Me [ 0 ] . ranged_weapon_skill ++ ;
	}
    }
    if ( KP7Pressed() )
    {
	while ( KP7Pressed() );
	if ( Shift_Was_Pressed() )
	    Me [ 0 ] . base_dexterity += 5 ;
	else
	{
	    if ( Me [ 0 ] . spellcasting_skill < 8 ) Me [ 0 ] . spellcasting_skill ++ ;
	}
    }
    if ( KP6Pressed() )
    {
	while ( KP6Pressed() );
	if ( Me [ 0 ] . hacking_skill < 8 ) Me [ 0 ] . hacking_skill ++ ;
    }
    
    //--------------------
    // Here we insert some cheat codes for assigning and completing missions
    //
    if ( Number0Pressed() )
    {
	if ( CPressed() )
	{
	    Me [ 0 ] . AllMissions [ 0 ] . MissionWasAssigned = TRUE ;
	    Me [ 0 ] . AllMissions [ 0 ] . MissionIsComplete = TRUE ;
	}
	if ( APressed() )
	{
	    Me [ 0 ] . AllMissions [ 0 ] . MissionWasAssigned = TRUE ;
	    Me [ 0 ] . AllMissions [ 0 ] . MissionIsComplete = FALSE ;
	}
    }
    if ( Number1Pressed() )
    {
	if ( CPressed() )
	{
	    Me [ 0 ] . AllMissions [ 1 ] . MissionWasAssigned = TRUE ;
	    Me [ 0 ] . AllMissions [ 1 ] . MissionIsComplete = TRUE ;
	}
	if ( APressed() )
	{
	    Me [ 0 ] . AllMissions [ 1 ] . MissionWasAssigned = TRUE ;
	    Me [ 0 ] . AllMissions [ 1 ] . MissionIsComplete = FALSE ;
	}
    }
    if ( Number2Pressed() )
    {
	if ( CPressed() )
	{
	    Me [ 0 ] . AllMissions [ 2 ] . MissionWasAssigned = TRUE ;
	    Me [ 0 ] . AllMissions [ 2 ] . MissionIsComplete = TRUE ;
	}
	if ( APressed() )
	{
	    Me [ 0 ] . AllMissions [ 2 ] . MissionWasAssigned = TRUE ;
	    Me [ 0 ] . AllMissions [ 2 ] . MissionIsComplete = FALSE ;
	}
    }
    if ( Number3Pressed() )
    {
	if ( CPressed() )
	{
	    Me [ 0 ] . AllMissions [ 3 ] . MissionWasAssigned = TRUE ;
	    Me [ 0 ] . AllMissions [ 3 ] . MissionIsComplete = TRUE ;
	}
	if ( APressed() )
	{
	    Me [ 0 ] . AllMissions [ 3 ] . MissionWasAssigned = TRUE ;
	    Me [ 0 ] . AllMissions [ 3 ] . MissionIsComplete = FALSE ;
	}
    }
    if ( Number4Pressed() )
    {
	if ( CPressed() )
	{
	    Me [ 0 ] . AllMissions [ 4 ] . MissionWasAssigned = TRUE ;
	    Me [ 0 ] . AllMissions [ 4 ] . MissionIsComplete = TRUE ;
	}
	if ( APressed() )
	{
	    Me [ 0 ] . AllMissions [ 4 ] . MissionWasAssigned = TRUE ;
	    Me [ 0 ] . AllMissions [ 4 ] . MissionIsComplete = FALSE ;
	}
    }
    if ( Number5Pressed() )
    {
	if ( CPressed() )
	{
	    Me [ 0 ] . AllMissions [ 5 ] . MissionWasAssigned = TRUE ;
	    Me [ 0 ] . AllMissions [ 5 ] . MissionIsComplete = TRUE ;
	}
	if ( APressed() )
	{
	    Me [ 0 ] . AllMissions [ 5 ] . MissionWasAssigned = TRUE ;
	    Me [ 0 ] . AllMissions [ 5 ] . MissionIsComplete = FALSE ;
	}
    }
    if ( Number6Pressed() )
    {
	if ( CPressed() )
	{
	    Me [ 0 ] . AllMissions [ 6 ] . MissionWasAssigned = TRUE ;
	    Me [ 0 ] . AllMissions [ 6 ] . MissionIsComplete = TRUE ;
	}
	if ( APressed() )
	{
	    Me [ 0 ] . AllMissions [ 6 ] . MissionWasAssigned = TRUE ;
	    Me [ 0 ] . AllMissions [ 6 ] . MissionIsComplete = FALSE ;
	}
    }

    //--------------------
    // For debugging purposes, we introduce a key, that causes several 
    // floor values around the Tux to be printed out.
    //
    if ( NPressed() )
    {
	DebugPrintf( 0 , "\n--------------------\nStarting at x=%d/y=%d.\n" , 
		     (int) Me [ 0 ] . pos . x - 5 , (int) Me [ 0 ] . pos . y - 5 );
	for ( i = Me [ 0 ] . pos . y - 5 ; i < Me [ 0 ] . pos . y + 5 ; i ++ )
	{
	    for ( j = Me [ 0 ] . pos . x - 5 ; j < Me [ 0 ] . pos . x + 5 ; j ++ )
	    {
		DebugPrintf( 0 , "%d " , CurLevel -> map [ i ] [ j ] . floor_value );
	    }
	    DebugPrintf( 0 , "\n" );
	}
    }

}; // void check_for_cheat_keys( void )
 

/* ----------------------------------------------------------------------
 * This function takes care of any keypresses to toggle the visibility
 * of the automap.  By default, the tab key will be used to toggle the
 * automap on/off.
 * ---------------------------------------------------------------------- */
void
automap_keyboard_handling ( void )
{
    static int TabPressed_LastFrame;

    //--------------------
    // We assign the Tab key to turn on/off the auto map
    //
    if ( TabPressed() )
    {
	if ( !TabPressed_LastFrame ) 
	{
	    GameConfig.Automap_Visible = !GameConfig.Automap_Visible;
	    
	    //--------------------
	    // We reset the map position whenever the map is switched off
	    // and back on again.  That way it should be made certain, that
	    // even if the map has slided off so some edge of the screen.
	    //
	    if ( GameConfig.Automap_Visible )
	    {
		GameConfig . automap_manual_shift_x = 0 ;
		GameConfig . automap_manual_shift_y = 0 ; 
	    }
	}
	
	TabPressed_LastFrame = TRUE;
    }
    else
    {
	TabPressed_LastFrame = FALSE;
    }
    

    //--------------------
    // Now we handle keyboard movement for the automap...
    //
    if ( LeftPressed() )
	GameConfig . automap_manual_shift_x -= 10 ;
    if ( RightPressed() )
	GameConfig . automap_manual_shift_x += 10 ;
    if ( UpPressed() )
	GameConfig . automap_manual_shift_y -= 10 ;
    if ( DownPressed() )
	GameConfig . automap_manual_shift_y += 10 ;

}; // void automap_toggle_on_off ( void )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
handle_quick_appy_inventory_keys( void )
{
    static int Number1PressedLastFrame;
    static int Number2PressedLastFrame;
    static int Number3PressedLastFrame;
    static int Number4PressedLastFrame;
    static int Number5PressedLastFrame;
    static int Number6PressedLastFrame;
    static int Number7PressedLastFrame;
    static int Number8PressedLastFrame;
    static int Number9PressedLastFrame;
    
    //--------------------
    // To quick-apply the items in the very lowest row of the
    // inventory screen, the number keys 1-9 can be used.
    //
    if ( Number1Pressed() )
    {
	if ( !Number1PressedLastFrame ) Quick_ApplyItem ( 1 );
	Number1PressedLastFrame = TRUE;
    }
    else 
    {
	Number1PressedLastFrame = FALSE;
    }
    if ( Number2Pressed() )
    {
	if ( !Number2PressedLastFrame ) Quick_ApplyItem ( 2 );
	Number2PressedLastFrame = TRUE;
    }
    else
    {
	Number2PressedLastFrame = FALSE;
    }
    if ( Number3Pressed() )
    {
	if ( !Number3PressedLastFrame ) Quick_ApplyItem ( 3 );
	Number3PressedLastFrame = TRUE;
    }
    else
    {
	Number3PressedLastFrame = FALSE;
    }
    if ( Number4Pressed() )
    {
	if ( !Number4PressedLastFrame ) Quick_ApplyItem ( 4 );
	Number4PressedLastFrame = TRUE;
    }
    else
    {
	Number4PressedLastFrame = FALSE;
    }
    if ( Number5Pressed() )
    {
	if ( !Number5PressedLastFrame ) Quick_ApplyItem ( 5 );
	Number5PressedLastFrame = TRUE;
    }
    else
    {
	Number5PressedLastFrame = FALSE;
    }
    if ( Number6Pressed() )
    {
	if ( !Number6PressedLastFrame ) Quick_ApplyItem ( 6 );
	Number6PressedLastFrame = TRUE;
    }
    else
    {
	Number6PressedLastFrame = FALSE;
    }
    if ( Number7Pressed() )
    {
	if ( !Number7PressedLastFrame ) Quick_ApplyItem ( 7 );
	Number7PressedLastFrame = TRUE;
    }
    else
    {
	Number7PressedLastFrame = FALSE;
    }
    if ( Number8Pressed() )
    {
	if ( !Number8PressedLastFrame ) Quick_ApplyItem ( 8 );
	Number8PressedLastFrame = TRUE ;
    }
    else
    {
	Number8PressedLastFrame = FALSE;
    }
    if ( Number9Pressed() )
    {
	if ( !Number9PressedLastFrame ) Quick_ApplyItem ( 9 );
	Number9PressedLastFrame = TRUE;
    }
    else
    {
	Number9PressedLastFrame = FALSE;
    }
    
}; // void handle_quick_appy_inventory_keys( void )

/* ----------------------------------------------------------------------
 * This function should toggle the visibility of the inventory/character
 * and skill screen.  Of course, when one of them is turned on, the other
 * ones should be turned off again.  At least that was a popular request
 * from various sources in the past, so we heed it now.
 * ---------------------------------------------------------------------- */
void
toggle_game_config_screen_visibility ( int screen_visible )
{

    switch ( screen_visible )
    {
	case GAME_CONFIG_SCREEN_VISIBLE_INVENTORY :
	    GameConfig . Inventory_Visible = ! GameConfig . Inventory_Visible;
	    GameConfig . CharacterScreen_Visible = FALSE ;
	    GameConfig . SkillScreen_Visible = FALSE ;
	    break;
	case GAME_CONFIG_SCREEN_VISIBLE_SKILLS :
	    GameConfig . SkillScreen_Visible = !GameConfig . SkillScreen_Visible;
	    GameConfig . CharacterScreen_Visible = FALSE ;
	    GameConfig . Inventory_Visible = FALSE ;
	    break;
	case GAME_CONFIG_SCREEN_VISIBLE_CHARACTER :
	    GameConfig . CharacterScreen_Visible = !GameConfig . CharacterScreen_Visible;
	    GameConfig . Inventory_Visible = FALSE ;
	    GameConfig . SkillScreen_Visible = FALSE ;
	    break;
	default:
	    GiveStandardErrorMessage ( __FUNCTION__  , "\
unhandled skill screen code received.  something is going VERY wrong!",
				       PLEASE_INFORM, IS_FATAL );
	    break;
    }

}; // void toggle_game_config_screen_visibility ( int screen_visible )

/* ----------------------------------------------------------------------
 * The I S and C keys can be used to open/close the inventory, skills and
 * the character screens.  This function takes care of any keyboard 
 * toggling the user might want to do.
 * ---------------------------------------------------------------------- */
void
handle_cha_ski_inv_screen_on_off_keys ( void )
{
    static int IPressed_LastFrame;
    static int CPressed_LastFrame;
    static int SPressed_LastFrame;

    //--------------------
    // We assign the S key to turning on/off the skill screen
    //
    if ( SPressed() )
    {
	if ( !SPressed_LastFrame ) 
	{
	    toggle_game_config_screen_visibility ( GAME_CONFIG_SCREEN_VISIBLE_SKILLS );
	}
	
	SPressed_LastFrame = TRUE;
    }
    else
    {
	SPressed_LastFrame = FALSE;
    }
    
    //--------------------
    // We assign the I key to turn on/off the inventory screen.
    //
    if ( IPressed() )
    {
	if ( !IPressed_LastFrame ) 
	{
	    toggle_game_config_screen_visibility ( GAME_CONFIG_SCREEN_VISIBLE_INVENTORY );
	}
	IPressed_LastFrame = TRUE;
    }
    else
    {
	IPressed_LastFrame = FALSE;
    }
    
    //--------------------
    // We assign the C key to turn on/off the character screen
    //
    if ( CPressed() )
    {
	if ( !CPressed_LastFrame ) 
	{
	    toggle_game_config_screen_visibility ( GAME_CONFIG_SCREEN_VISIBLE_CHARACTER );
	}
	CPressed_LastFrame = TRUE;
    }
    else
    {
	CPressed_LastFrame = FALSE;
    }
    
}; // void handle_cha_ski_inv_screen_on_off_keys ( void )

/* ----------------------------------------------------------------------
 * This function does the reactions to keypresses of the player other
 * than pressing cursor keys.
 * ---------------------------------------------------------------------- */
void 
ReactToSpecialKeys(void)
{
    static int TPressed_LastFrame;
    static int LPressed_LastFrame;
    
    //--------------------
    // Some QUIT key and some self-destruct keys can stay in
    // there (they are most convenient for debug purposes, yet
    // they can't be used for 'cheating' in the normal sense).
    //
    if ( QPressed() )
    {
	if ( CtrlWasPressed() )
	    Terminate (OK);
	else
	{
	    quest_browser_interface ( );
	    while ( QPressed() ) ;
	}
    }
    
    if ( DPressed() && CtrlWasPressed() ) 
	Me[0].energy = 0;
    
    //--------------------
    // The 'function' keys F1-F10 can be used to quick-ready
    // a skill.  Currently the skill to associate with each
    // function key can NOT be customized.  This might follow
    // later, when the game reaches a more complete state.
    //
    if ( F1Pressed()  ) activate_nth_aquired_skill ( 0 );
    if ( F2Pressed()  ) activate_nth_aquired_skill ( 1 );
    if ( F3Pressed()  ) activate_nth_aquired_skill ( 2 );
    if ( F4Pressed()  ) activate_nth_aquired_skill ( 3 );
    if ( F5Pressed()  ) activate_nth_aquired_skill ( 4 );
    if ( F6Pressed()  ) activate_nth_aquired_skill ( 5 );
    if ( F7Pressed()  ) activate_nth_aquired_skill ( 6 );
    if ( F8Pressed()  ) activate_nth_aquired_skill ( 7 );
    if ( F9Pressed()  ) activate_nth_aquired_skill ( 8 );
    if ( F10Pressed() ) activate_nth_aquired_skill ( 9 );
    
    handle_quick_appy_inventory_keys();
    
    //--------------------
    // For debugging purposes as well, the F key will print out information
    // about all players currently known.  This can be used as well from the
    // server as from each of the client.
    //
    if ( FPressed ( ) )
    {
	Activate_Conservative_Frame_Computation ( ) ;
	PrintServerStatusInformation ( ) ;
	
	if ( CtrlWasPressed ( ) && Alt_Was_Pressed ( ) && Shift_Was_Pressed ( ) )
	{
	    DebugPrintf ( -10 , "\nForcefully flushing image cache now --> all backgrounds will be reloaded." );
	    flush_background_image_cache ( ) ;
	}
	
	while ( FPressed ( ) );
    }
    
    //--------------------
    // We assign the L key to turn on/off the quest log i.e. mission log
    //
    if ( LPressed() && ( !LPressed_LastFrame ) )
    {
	GameConfig.Mission_Log_Visible_Time = 0;
	GameConfig.Mission_Log_Visible = !GameConfig.Mission_Log_Visible;
	DebugPrintf( 0 , "\nMISSION LOG TURNED ON!\n" );
    }
    LPressed_LastFrame = LPressed();
    
    
    //--------------------
    // We assign the Space key to turn off all windows and quest log
    // and also when the Tux is dead, there shouldn't be any windows any more.
    //
    if ( ( SpacePressed( ) && !axis_is_active ) ||
	 ( Me[0].energy <= 0 ) )
    {
	GameConfig.Mission_Log_Visible = FALSE ; 
	GameConfig.SkillScreen_Visible = FALSE ;
	GameConfig.CharacterScreen_Visible = FALSE ;
	GameConfig.Inventory_Visible = FALSE ;
    }
    
    
    handle_cha_ski_inv_screen_on_off_keys ( );
    
    
    automap_keyboard_handling ( );

    //--------------------
    // We assign the G key to send greetings either to the server
    // or from the server to all players.
    //
    if ( GPressed () )
    {
	/*
	  Me[0].TextToBeDisplayed="Hello!  Greetings to the Server.";
	  Me[0].TextVisibleTime=0;
	  
	  if ( ! ServerMode )
	  {
	  sprintf ( MessageBuffer , "\nThis is a message from Player '%s'. " , Me[0].character_name );
	  // Send1024MessageToServer ( MessageBuffer );
	  SendTextMessageToServer ( MessageBuffer );
	  }
	  else
	  {
	  ServerSendMessageToAllClients ( "\nThis is a message from the server to all clients. " );
	  }
	*/
    }
    

    if ( GameConfig . enable_cheatkeys ) check_for_cheat_keys();

    //--------------------
    // To quicksave and quickload in a convenient way, I added
    // a keybinding of save and load game functions to the 3 and 4
    // keys on the numerical keyboard.
    //
    if ( KP3Pressed() )
    {
	while (KP3Pressed());
	SaveGame();
    }
    if ( KP4Pressed() )
    {
	while ( KP4Pressed() );
	LoadGame();
    }
    
    //--------------------
    // To test various things, there is of course a cheat menu
    // added to the game.  This cheat menu can be reached by pressing
    // a combination of keys, intended to be so complicated, that the
    // users are unlikely to find out about it just by trying out.
    // Of course they will learn about it if they look at the source,
    // but hey, then you can outright modify the source, so why bother
    // to make it any more complicated.
    //
    if ( CPressed() && Alt_Was_Pressed()
	 && CtrlWasPressed() && Shift_Was_Pressed() ) 
	Cheatmenu ();
    
    //--------------------
    // The 'Esc' key is assigned to the big main menu, the so called
    // Escape Menu.  But this will only pop up, if there were no screens
    // open at that time.  Otherwise, the 'Esc' key will just be interpreted
    // as a 'close-all-screens' button.
    //
    if ( EscapePressed() )
    {
	if ( GameConfig . Inventory_Visible ||
	     GameConfig . CharacterScreen_Visible ||
	     GameConfig . SkillScreen_Visible )
	{
	    GameConfig . Inventory_Visible = FALSE ;
	    GameConfig . CharacterScreen_Visible = FALSE ;
	    GameConfig . SkillScreen_Visible = FALSE ;
	    while ( EscapePressed() );
	}
	else
	{
	    EscapeMenu ();
	}
    }
    
    //--------------------
    // The 'P' key is assigned to pause mode.
    //
    if ( PPressed () )
	Pause ();
    
    //--------------------
    // t key turns on/off transparency mode
    //
    if ( TPressed() )
    {
	if ( !TPressed_LastFrame ) 
	{
	    GameConfig.transparency = ! GameConfig.transparency;
	}
	
	TPressed_LastFrame = TRUE;
    }
    else
    {
	TPressed_LastFrame = FALSE;
    }
    
}; // void ReactToSpecialKeys(void)

/* ----------------------------------------------------------------------
 * Two functions are important concerning the SHIFT keys:  One says if
 * the shift key has been pressed during the previous key-down event
 * and the other one just reports if the shift key is pressed currently.
 * These two functions should not be mixed up!!!!!!!!!!!!!!!!!!!!!!!!!!
 * ---------------------------------------------------------------------- */
int
Shift_Was_Pressed(void)
{
  return (ShiftWasPressedInAddition);  
};

int
RightShiftWasPressed(void)
{
  return (RightShiftWasPressedInAddition);  
};

int
LeftShiftWasPressed(void)
{
  return (LeftShiftWasPressedInAddition);  
};

int
Shift_Is_Pressed(void)
{
    keyboard_update ();
    return ( CurrentlyShiftPressed );  
};

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
int
CtrlWasPressed(void)
{
  return (CtrlWasPressedInAddition);  
}

int
RightCtrlWasPressed(void)
{
  return (RightCtrlWasPressedInAddition);  
}

int
LeftCtrlWasPressed(void)
{
  return (LeftCtrlWasPressedInAddition);  
}

int
Alt_Was_Pressed(void)
{
  return (AltWasPressedInAddition);  
}

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
int
LeftAltWasPressed(void)
{
    keyboard_update ();
    return (LeftAltWasPressedInAddition);  
}; // int LeftAltWasPressed(void)

int
RightAltWasPressed(void)
{
  return (RightAltWasPressedInAddition);  
}

int 
keyboard_update(void)
{
  Uint8 axis; 

  while( SDL_PollEvent( &event ) )
    {
      switch( event.type )
	{
	case SDL_QUIT:
	  printf("\n\nUser requestet Termination...\n\nTerminating...");
	  Terminate(0);
	  break;
	  /* Look for a keypress */
	case SDL_KEYDOWN:

	  if ( ( ClientMode ) && ( ! ServerMode ) ) SendPlayerKeyboardEventToServer ( event );

	  // printf("\nSLD_KEYDOWN event detected...");
	  // fflush(stdout);

	  // Check for some additional modifiers and set flags accordingly
	  if ( event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT) )
	    ShiftWasPressedInAddition=TRUE;
	  else ShiftWasPressedInAddition=FALSE;

	  if ( event.key.keysym.mod & (KMOD_LSHIFT) )
	    LeftShiftWasPressedInAddition=TRUE;
	  else LeftShiftWasPressedInAddition=FALSE;

	  if ( event.key.keysym.mod & (KMOD_RSHIFT) )
	    RightShiftWasPressedInAddition=TRUE;
	  else RightShiftWasPressedInAddition=FALSE;

	  if ( event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL) )
	    {
	      CtrlWasPressedInAddition=TRUE;
	    }
	  else CtrlWasPressedInAddition=FALSE;

	  if ( event.key.keysym.mod & (KMOD_LCTRL) ) LeftCtrlWasPressedInAddition=TRUE;
	  else LeftCtrlWasPressedInAddition=FALSE;
	  if ( event.key.keysym.mod & (KMOD_RCTRL) ) RightCtrlWasPressedInAddition=TRUE;
	  else RightCtrlWasPressedInAddition=FALSE;

	  if ( event.key.keysym.mod & (KMOD_LALT | KMOD_RALT) )
	    AltWasPressedInAddition=TRUE;
	  else AltWasPressedInAddition=FALSE;

	  if ( event.key.keysym.mod & ( KMOD_LALT ) )
	    LeftAltWasPressedInAddition=TRUE;
	  else LeftAltWasPressedInAddition=FALSE;

	  if ( event.key.keysym.mod & ( KMOD_RALT ) )
	    RightAltWasPressedInAddition=TRUE;
	  else RightAltWasPressedInAddition=FALSE;

	  /* Check the SDLKey values */
	  switch( event.key.keysym.sym )
	    {
	    case SDLK_KP_PLUS:
	      CurrentlyKP_PLUS_Pressed=TRUE;
	      break;
	    case SDLK_KP_MINUS:
	      CurrentlyKP_MINUS_Pressed=TRUE;
	      break;
	    case SDLK_KP_MULTIPLY:
	      CurrentlyKP_MULTIPLY_Pressed=TRUE;
	      break;
	    case SDLK_KP_DIVIDE:
	      CurrentlyKP_DIVIDE_Pressed=TRUE;
	      break;
	    case SDLK_KP_ENTER:
	      CurrentlyKP_ENTER_Pressed=TRUE;
	      break;
	    case SDLK_0:
	      Currently0Pressed=TRUE;
	      break;
	    case SDLK_1:
	      Currently1Pressed=TRUE;
	      break;
	    case SDLK_2:
	      Currently2Pressed=TRUE;
	      break;
	    case SDLK_3:
	      Currently3Pressed=TRUE;
	      break;
	    case SDLK_4:
	      Currently4Pressed=TRUE;
	      break;
	    case SDLK_5:
	      Currently5Pressed=TRUE;
	      break;
	    case SDLK_6:
	      Currently6Pressed=TRUE;
	      break;
	    case SDLK_7:
	      Currently7Pressed=TRUE;
	      break;
	    case SDLK_8:
	      Currently8Pressed=TRUE;
	      break;
	    case SDLK_9:
	      Currently9Pressed=TRUE;
	      break;
	    case SDLK_KP0:
	      CurrentlyKP0Pressed=TRUE;
	      break;
	    case SDLK_KP1:
	      CurrentlyKP1Pressed=TRUE;
	      break;
	    case SDLK_KP2:
	      CurrentlyKP2Pressed=TRUE;
	      break;
	    case SDLK_KP3:
	      CurrentlyKP3Pressed=TRUE;
	      break;
	    case SDLK_KP4:
	      CurrentlyKP4Pressed=TRUE;
	      break;
	    case SDLK_KP5:
	      CurrentlyKP5Pressed=TRUE;
	      break;
	    case SDLK_KP6:
	      CurrentlyKP6Pressed=TRUE;
	      break;
	    case SDLK_KP7:
	      CurrentlyKP7Pressed=TRUE;
	      break;
	    case SDLK_KP8:
	      CurrentlyKP8Pressed=TRUE;
	      break;
	    case SDLK_KP9:
	      CurrentlyKP9Pressed=TRUE;
	      break;
	    case SDLK_F1:
	      CurrentlyF1Pressed=TRUE;
	      break;
	    case SDLK_F2:
	      CurrentlyF2Pressed=TRUE;
	      break;
	    case SDLK_F3:
	      CurrentlyF3Pressed=TRUE;
	      break;
	    case SDLK_F4:
	      CurrentlyF4Pressed=TRUE;
	      break;
	    case SDLK_F5:
	      CurrentlyF5Pressed=TRUE;
	      break;
	    case SDLK_F6:
	      CurrentlyF6Pressed=TRUE;
	      break;
	    case SDLK_F7:
	      CurrentlyF7Pressed=TRUE;
	      break;
	    case SDLK_F8:
	      CurrentlyF8Pressed=TRUE;
	      break;
	    case SDLK_F9:
	      CurrentlyF9Pressed=TRUE;
	      break;
	    case SDLK_F10:
	      CurrentlyF10Pressed=TRUE;
	      break;
	    case SDLK_F11:
	      CurrentlyF11Pressed=TRUE;
	      break;
	    case SDLK_F12:
	      CurrentlyF12Pressed=TRUE;
	      TakeScreenshot();
	      break;
	    case SDLK_BACKSPACE:
	      CurrentlyBackspacePressed=TRUE;
	      break;
	    case SDLK_LCTRL:
	      CurrentlyLeftCtrlPressed=TRUE;
	      break;
	    case SDLK_LEFT:
	      CurrentlyLeftPressed=TRUE;
	      break;
	    case SDLK_RIGHT:
	      CurrentlyRightPressed=TRUE;
	      break;
	    case SDLK_UP:
	      CurrentlyUpPressed=TRUE;
	      break;
	    case SDLK_DOWN:
	      CurrentlyDownPressed=TRUE;
	      break;
	    case SDLK_SPACE:
	      CurrentlySpacePressed=TRUE;
	      break;
	    case SDLK_RETURN:
	      CurrentlyEnterPressed=TRUE;
	      break;
	    case SDLK_a:
	      CurrentlyAPressed=TRUE;
	      break;
	    case SDLK_b:
	      CurrentlyBPressed=TRUE;
	      break;
	    case SDLK_c:
	      CurrentlyCPressed=TRUE;
	      break;
	    case SDLK_d:
	      CurrentlyDPressed=TRUE;
	      break;
	    case SDLK_e:
	      CurrentlyEPressed=TRUE;
	      break;
	    case SDLK_f:
	      CurrentlyFPressed=TRUE;
	      break;
	    case SDLK_g:
	      CurrentlyGPressed=TRUE;
	      break;
	    case SDLK_h:
	      CurrentlyHPressed=TRUE;
	      break;
	    case SDLK_i:
	      CurrentlyIPressed=TRUE;
	      break;
	    case SDLK_j:
	      CurrentlyJPressed=TRUE;
	      break;
	    case SDLK_k:
	      CurrentlyKPressed=TRUE;
	      break;
	    case SDLK_l:
	      CurrentlyLPressed=TRUE;
	      break;
	    case SDLK_m:
	      CurrentlyMPressed=TRUE;
	      break;
	    case SDLK_n:
	      CurrentlyNPressed=TRUE;
	      break;
	    case SDLK_o:
	      CurrentlyOPressed=TRUE;
	      break;
	    case SDLK_p:
	      CurrentlyPPressed=TRUE;
	      break;
	    case SDLK_q:
	      CurrentlyQPressed=TRUE;
	      break;
	    case SDLK_r:
	      CurrentlyRPressed=TRUE;
	      break;
	    case SDLK_s:
	      CurrentlySPressed=TRUE;
	      break;
	    case SDLK_t:
	      CurrentlyTPressed=TRUE;
	      break;
	    case SDLK_u:
	      CurrentlyUPressed=TRUE;
	      break;
	    case SDLK_v:
	      CurrentlyVPressed=TRUE;
	      break;
	    case SDLK_w:
	      CurrentlyWPressed=TRUE;
	      break;
	    case SDLK_x:
	      CurrentlyXPressed=TRUE;
	      break;
	    case SDLK_y:
	      CurrentlyYPressed=TRUE;
	      break;
	    case SDLK_z:
	      CurrentlyZPressed=TRUE;
	      break;
	    case SDLK_ESCAPE:
	      CurrentlyEscapePressed=TRUE;
	      break;
	    case SDLK_TAB:
	      CurrentlyTabPressed=TRUE;
	      break;
	    case SDLK_RSHIFT:
	      CurrentlyShiftPressed=TRUE;
	      break;
	    case SDLK_LSHIFT:
	      CurrentlyShiftPressed=TRUE;
	      break;
	    default:
	      /*
		printf("\n\nUnhandled keystroke!! Terminating...\n\n");
		Terminate(ERR);
	      */
	      break;
	    }
	  break;
	  /* We must also use the SDL_KEYUP events to zero the x */
	  /* and y velocity variables. But we must also be       */
	  /* careful not to zero the velocities when we shouldn't*/
	case SDL_KEYUP:

	  if ( ( ClientMode ) && ( ! ServerMode ) ) SendPlayerKeyboardEventToServer ( event );

	  // printf("\nSLD_KEYUP event detected...");
	  // fflush(stdout);

	  // Check for some additional modifiers and set flags accordingly

	  /*
	  if ( event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT) )
	    ShiftWasPressedInAddition=TRUE;
	  else ShiftWasPressedInAddition=FALSE;

	  if ( event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL) )
	    CtrlWasPressedInAddition=TRUE;
	  else CtrlWasPressedInAddition=FALSE;

	  if ( event.key.keysym.mod & (KMOD_LCTRL) )
	    LeftCtrlWasPressedInAddition=TRUE;
	  else LeftCtrlWasPressedInAddition=FALSE;

	  if ( event.key.keysym.mod & (KMOD_RCTRL) )
	    RightCtrlWasPressedInAddition=TRUE;
	  else RightCtrlWasPressedInAddition=FALSE;

	  if ( event.key.keysym.mod & (KMOD_LALT | KMOD_RALT) )
	    AltWasPressedInAddition=TRUE;
	  else AltWasPressedInAddition=FALSE;
	  */
	  switch( event.key.keysym.sym )
	    {
	    case SDLK_KP_PLUS:
	      CurrentlyKP_PLUS_Pressed=FALSE;
	      break;
	    case SDLK_KP_MINUS:
	      CurrentlyKP_MINUS_Pressed=FALSE;
	      break;
	    case SDLK_KP_MULTIPLY:
	      CurrentlyKP_MULTIPLY_Pressed=FALSE;
	      break;
	    case SDLK_KP_DIVIDE:
	      CurrentlyKP_DIVIDE_Pressed=FALSE;
	      break;
	    case SDLK_KP_ENTER:
	      CurrentlyKP_ENTER_Pressed=FALSE;
	      break;
	    case SDLK_0:
	      Currently0Pressed=FALSE;
	      break;
	    case SDLK_1:
	      Currently1Pressed=FALSE;
	      break;
	    case SDLK_2:
	      Currently2Pressed=FALSE;
	      break;
	    case SDLK_3:
	      Currently3Pressed=FALSE;
	      break;
	    case SDLK_4:
	      Currently4Pressed=FALSE;
	      break;
	    case SDLK_5:
	      Currently5Pressed=FALSE;
	      break;
	    case SDLK_6:
	      Currently6Pressed=FALSE;
	      break;
	    case SDLK_7:
	      Currently7Pressed=FALSE;
	      break;
	    case SDLK_8:
	      Currently8Pressed=FALSE;
	      break;
	    case SDLK_9:
	      Currently9Pressed=FALSE;
	      break;
	    case SDLK_KP0:
	      CurrentlyKP0Pressed=FALSE;
	      break;
	    case SDLK_KP1:
	      CurrentlyKP1Pressed=FALSE;
	      break;
	    case SDLK_KP2:
	      CurrentlyKP2Pressed=FALSE;
	      break;
	    case SDLK_KP3:
	      CurrentlyKP3Pressed=FALSE;
	      break;
	    case SDLK_KP4:
	      CurrentlyKP4Pressed=FALSE;
	      break;
	    case SDLK_KP5:
	      CurrentlyKP5Pressed=FALSE;
	      break;
	    case SDLK_KP6:
	      CurrentlyKP6Pressed=FALSE;
	      break;
	    case SDLK_KP7:
	      CurrentlyKP7Pressed=FALSE;
	      break;
	    case SDLK_KP8:
	      CurrentlyKP8Pressed=FALSE;
	      break;
	    case SDLK_KP9:
	      CurrentlyKP9Pressed=FALSE;
	      break;
	    case SDLK_F1:
	      CurrentlyF1Pressed=FALSE;
	      break;
	    case SDLK_F2:
	      CurrentlyF2Pressed=FALSE;
	      break;
	    case SDLK_F3:
	      CurrentlyF3Pressed=FALSE;
	      break;
	    case SDLK_F4:
	      CurrentlyF4Pressed=FALSE;
	      break;
	    case SDLK_F5:
	      CurrentlyF5Pressed=FALSE;
	      break;
	    case SDLK_F6:
	      CurrentlyF6Pressed=FALSE;
	      break;
	    case SDLK_F7:
	      CurrentlyF7Pressed=FALSE;
	      break;
	    case SDLK_F8:
	      CurrentlyF8Pressed=FALSE;
	      break;
	    case SDLK_F9:
	      CurrentlyF9Pressed=FALSE;
	      break;
	    case SDLK_F10:
	      CurrentlyF10Pressed=FALSE;
	      break;
	    case SDLK_F11:
	      CurrentlyF11Pressed=FALSE;
	      break;
	    case SDLK_F12:
	      CurrentlyF12Pressed=FALSE;
	      break;
	    case SDLK_BACKSPACE:
	      CurrentlyBackspacePressed=FALSE;
	      break;
	    case SDLK_LCTRL:
	      CurrentlyLeftCtrlPressed=FALSE;
	      break;
	    case SDLK_LEFT:
	      CurrentlyLeftPressed=FALSE;
	      break;
	    case SDLK_RIGHT:
	      CurrentlyRightPressed=FALSE;
	      break;
	    case SDLK_UP:
	      CurrentlyUpPressed=FALSE;
	      break;
	    case SDLK_DOWN:
	      CurrentlyDownPressed=FALSE;
	      break;
	    case SDLK_SPACE:
	      CurrentlySpacePressed=FALSE;
	      break;
	    case SDLK_RETURN:
	      CurrentlyEnterPressed=FALSE;
	      break;
	    case SDLK_a:
	      CurrentlyAPressed=FALSE;
	      break;
	    case SDLK_b:
	      CurrentlyBPressed=FALSE;
	      break;
	    case SDLK_c:
	      CurrentlyCPressed=FALSE;
	      break;
	    case SDLK_d:
	      CurrentlyDPressed=FALSE;
	      break;
	    case SDLK_e:
	      CurrentlyEPressed=FALSE;
	      break;
	    case SDLK_f:
	      CurrentlyFPressed=FALSE;
	      break;
	    case SDLK_g:
	      CurrentlyGPressed=FALSE;
	      break;
	    case SDLK_h:
	      CurrentlyHPressed=FALSE;
	      break;
	    case SDLK_i:
	      CurrentlyIPressed=FALSE;
	      break;
	    case SDLK_j:
	      CurrentlyJPressed=FALSE;
	      break;
	    case SDLK_k:
	      CurrentlyKPressed=FALSE;
	      break;
	    case SDLK_l:
	      CurrentlyLPressed=FALSE;
	      break;
	    case SDLK_m:
	      CurrentlyMPressed=FALSE;
	      break;
	    case SDLK_n:
	      CurrentlyNPressed=FALSE;
	      break;
	    case SDLK_o:
	      CurrentlyOPressed=FALSE;
	      break;
	    case SDLK_p:
	      CurrentlyPPressed=FALSE;
	      break;
	    case SDLK_q:
	      CurrentlyQPressed=FALSE;
	      break;
	    case SDLK_r:
	      CurrentlyRPressed=FALSE;
	      break;
	    case SDLK_s:
	      CurrentlySPressed=FALSE;
	      break;
	    case SDLK_t:
	      CurrentlyTPressed=FALSE;
	      break;
	    case SDLK_u:
	      CurrentlyUPressed=FALSE;
	      break;
	    case SDLK_v:
	      CurrentlyVPressed=FALSE;
	      break;
	    case SDLK_w:
	      CurrentlyWPressed=FALSE;
	      break;
	    case SDLK_x:
	      CurrentlyXPressed=FALSE;
	      break;
	    case SDLK_y:
	      CurrentlyYPressed=FALSE;
	      break;
	    case SDLK_z:
	      CurrentlyZPressed=FALSE;
	      break;
	    case SDLK_ESCAPE:
	      CurrentlyEscapePressed=FALSE;
	      break;
	    case SDLK_TAB:
	      CurrentlyTabPressed=FALSE;
	      break;
	    case SDLK_RSHIFT:
	      CurrentlyShiftPressed=FALSE;
	      break;
	    case SDLK_LSHIFT:
	      CurrentlyShiftPressed=FALSE;
	      break;
	    default:
	      break;
	    }
	  break;

	case SDL_JOYAXISMOTION:
	  axis = event.jaxis.axis;
	  if (axis == 0 || ((joy_num_axes >= 5) && (axis == 3)) ) /* x-axis */
	    {
	      input_axis.x = event.jaxis.value;

	      if (event.jaxis.value > joy_sensitivity*1000)   /* about half tilted */
		{
		  CurrentlyRightPressed = TRUE;
		  CurrentlyLeftPressed = FALSE;
		}
	      else if (event.jaxis.value <  -joy_sensitivity*1000)
		{
		  CurrentlyLeftPressed = TRUE;
		  CurrentlyRightPressed = FALSE;
		}
	      else
		{
		  CurrentlyLeftPressed = FALSE;
		  CurrentlyRightPressed= FALSE;
		}
	    }
	  else if ((axis == 1) || ((joy_num_axes >=5) && (axis == 4))) /* y-axis */
	    {
	      input_axis.y = event.jaxis.value;

	      if (event.jaxis.value > joy_sensitivity*1000)  
		{
		  CurrentlyDownPressed = TRUE;
		  CurrentlyUpPressed = FALSE;
		}
	      else if (event.jaxis.value < -joy_sensitivity*1000)
		{
		  CurrentlyUpPressed = TRUE;
		  CurrentlyDownPressed = FALSE;
		}
	      else
		{
		  CurrentlyUpPressed = FALSE;
		  CurrentlyDownPressed= FALSE;
		}
	    }
		
	  break;
	  
	case SDL_JOYBUTTONDOWN:
	  CurrentlySpacePressed = TRUE;
	  axis_is_active = TRUE;
	  break;

	case SDL_JOYBUTTONUP:
	  CurrentlySpacePressed = FALSE;
	  axis_is_active = FALSE;
	  break;

	case SDL_MOUSEMOTION:
	    input_axis . x = event . button . x - UserCenter_x  ;
	    input_axis . y = event . button . y - UserCenter_y  ;
	    CurrentMouseAbsPos.x = event.button.x;
	    CurrentMouseAbsPos.y = event.button.y;

	    //--------------------
	    // Now maybe the crosshair mouse cursor shape is currently
	    // selected.  In this case of course we must shift the click
	    // position a bit, since the crosshair pointer pixel is in the
	    // middle of the mouse cursor, not in the top left as with the
	    // other mouse cursor shapes...
	    //
	    if ( current_mouse_cursor_shape == MOUSE_CURSOR_CROSSHAIR_SHAPE )
	    {
		input_axis . x += 16 ; 
		input_axis . y += 16 ; 
		CurrentMouseAbsPos . x += 16 ;
		CurrentMouseAbsPos . y += 16 ;
	    }

	  break;
	  
	case SDL_MOUSEBUTTONDOWN:
	    input_axis.x = event.button.x - UserCenter_x ; 
	    input_axis.y = event.button.y - UserCenter_y ; 	  
	    CurrentMouseAbsPos.x = event.button.x;
	    CurrentMouseAbsPos.y = event.button.y;

	    //--------------------
	    // Now maybe the crosshair mouse cursor shape is currently
	    // selected.  In this case of course we must shift the click
	    // position a bit, since the crosshair pointer pixel is in the
	    // middle of the mouse cursor, not in the top left as with the
	    // other mouse cursor shapes...
	    //
	    if ( current_mouse_cursor_shape == MOUSE_CURSOR_CROSSHAIR_SHAPE )
	    {
		input_axis . x += 16 ; 
		input_axis . y += 16 ; 
		CurrentMouseAbsPos . x += 16 ;
		CurrentMouseAbsPos . y += 16 ;
	    }

	    if ( ( ClientMode ) && ( ! ServerMode ) ) SendPlayerMouseButtonEventToServer ( event );

	    if ( event.button.button == SDL_BUTTON_LEFT )
	    {
		CurrentlySpacePressed = TRUE;
		axis_is_active = TRUE;
		CurrentlyMouseLeftPressed = TRUE;
		// DebugPrintf ( 0 , "\nLeft button press registered..." );
		
		//--------------------
		// It is possible to completely freeze freedroidRPG by holding down the left
		// mouse button continuously while striking again (the second time).  Therefore
		// we must terminate out of this right here.
		//
		return ( 0 ) ; 
	    }

	    if ( event.button.button == SDL_BUTTON_RIGHT )	   
		CurrentlyMouseRightPressed = TRUE;

	    //--------------------
	    // We need to add come conditional compilation here, so that 
	    // on some systems, where the SDL version is < 1.2.5 the code
	    // still compiles without much trouble. (At least so we hope :)
	    //
#ifdef SDL_BUTTON_WHEELUP 	
	    if ( event.button.button == SDL_BUTTON_WHEELUP )
	    {
		CurrentlyMouseWheelUpPressed = TRUE;
		MouseWheelUpMovesRecorded ++ ;
		DebugPrintf( 1 , "\n\nMOUSE WHEEL ACTION UP DETECTED!!!");
	    }
	    if ( event.button.button == SDL_BUTTON_WHEELDOWN )
	    {
		CurrentlyMouseWheelDownPressed = TRUE;
		MouseWheelDownMovesRecorded ++ ;
		DebugPrintf( 1 , "\n\nMOUSE WHEEL ACTION DOWN DETECTED!!!");
	    }
#endif
	    break;

        case SDL_MOUSEBUTTONUP:

	  input_axis.x = event.button.x - UserCenter_x + 16; 
	  input_axis.y = event.button.y - UserCenter_y + 16; 	  
	  CurrentMouseAbsPos.x = event.button.x;
	  CurrentMouseAbsPos.y = event.button.y;

	  if ( ( ClientMode ) && ( ! ServerMode ) ) SendPlayerMouseButtonEventToServer ( event );

	  if (event.button.button == SDL_BUTTON_LEFT)
	    {
	      CurrentlySpacePressed = FALSE;
	      axis_is_active = FALSE;
	    }

	  if (event.button.button == SDL_BUTTON_RIGHT)
	    CurrentlyMouseRightPressed = FALSE;

	  //--------------------
	  // We need to add come conditional compilation here, so that 
	  // on some systems, where the SDL version is < 1.2.5 the code
	  // still compiles without much trouble. (At least so we hope :)
	  //
#ifdef SDL_BUTTON_WHEELUP 
	  if ( event.button.button == SDL_BUTTON_WHEELUP )
	    {
	      CurrentlyMouseWheelUpPressed = FALSE ;
	      DebugPrintf( 1 , "\n\nMOUSE WHEEL ACTION UP STOPPED!!!");
	    }
	  if ( event.button.button == SDL_BUTTON_WHEELDOWN )
	    {
	      CurrentlyMouseWheelDownPressed = FALSE;
	      DebugPrintf( 1 , "\n\nMOUSE WHEEL ACTION DOWN STOPPED!!!");
	    }
#endif

	  break;


 	default:
 	  break;
 	}

    }

  return 0;
}

/*-----------------------------------------------------------------
 * Desc: should do roughly what getchar() does, but in raw 
 * 	 (SLD) keyboard mode. 
 * 
 * Return: the (SDLKey) of the next key-pressed event cast to (int)
 *
 *-----------------------------------------------------------------*/
int
getchar_raw (void)
{
  SDL_Event event;
  int Returnkey;

  //  keyboard_update ();   /* treat all pending keyboard-events */

  while (1)
    {
      SDL_WaitEvent (&event);    /* wait for next event */
      
      if (event.type == SDL_KEYDOWN)
	{
	  /* 
	   * here we use the fact that, I cite from SDL_keyboard.h:
	   * "The keyboard syms have been cleverly chosen to map to ASCII"
	   * ... I hope that this design feature is portable, and durable ;)  
	   */
	  Returnkey = (int) event.key.keysym.sym;
	  if ( event.key.keysym.mod & KMOD_SHIFT ) Returnkey = toupper( (int)event.key.keysym.sym );
	  DebugPrintf (3, "getchar_raw() returns key-code: %d\n", Returnkey);
	  return ( Returnkey );
	}
      else if (event.type == SDL_KEYUP)
	{

	  UnsetAllKeys ();  // we don't want to get any 'stuck' keys, and after entering
	                    // text, I'm sure no key still has to be pressed...

	  // do nothing here, but don't push this event either
	}
      else
	{
	  SDL_PushEvent (&event);  /* put this event back into the queue */
	  keyboard_update ();  /* and treat it the usual way */
	  continue;
	}

    } /* while(1) */

} /* getchar_raw() */


int 
KP_PLUS_Pressed (void)
{
  keyboard_update();
  return CurrentlyKP_PLUS_Pressed;
}

int 
KP_MULTIPLY_Pressed (void)
{
  keyboard_update();
  return CurrentlyKP_MULTIPLY_Pressed;
}

int 
KP_MINUS_Pressed (void)
{
  keyboard_update();
  return CurrentlyKP_MINUS_Pressed;
}

int 
KP_DIVIDE_Pressed (void)
{
  keyboard_update();
  return CurrentlyKP_DIVIDE_Pressed;
}

int 
KP_ENTER_Pressed (void)
{
  keyboard_update();
  return CurrentlyKP_ENTER_Pressed;
}

int
Number0Pressed (void)
{
  keyboard_update ();
  return Currently0Pressed;
}				// int KP0Pressed(void)

int
Number1Pressed (void)
{
  keyboard_update ();
  return Currently1Pressed;
}				// int KP1Pressed(void)

int
Number2Pressed (void)
{
  keyboard_update ();
  return Currently2Pressed;
}				// int KP2Pressed(void)

int
Number3Pressed (void)
{
  keyboard_update ();
  return Currently3Pressed;
}				// int KP3Pressed(void)

int
Number4Pressed (void)
{
  keyboard_update ();
  return Currently4Pressed;
}				// int KP4Pressed(void)

int
Number5Pressed (void)
{
  keyboard_update ();
  return Currently5Pressed;
}				// int KP5Pressed(void)

int
Number6Pressed (void)
{
  keyboard_update ();
  return Currently6Pressed;
}				// int KP6Pressed(void)

int
Number7Pressed (void)
{
  keyboard_update ();
  return Currently7Pressed;
}				// int KP7Pressed(void)

int
Number8Pressed (void)
{
  keyboard_update ();
  return Currently8Pressed;
}				// int KP8Pressed(void)

int
Number9Pressed (void)
{
  keyboard_update ();
  return Currently9Pressed;
}				// int KP9Pressed(void)

int
KP0Pressed (void)
{
  keyboard_update ();
  return CurrentlyKP0Pressed;
}				// int KP0Pressed(void)

int
KP1Pressed (void)
{
  keyboard_update ();
  return CurrentlyKP1Pressed;
}				// int KP1Pressed(void)

int
KP2Pressed (void)
{
  keyboard_update ();
  return CurrentlyKP2Pressed;
}				// int KP2Pressed(void)

int
KP3Pressed (void)
{
  keyboard_update ();
  return CurrentlyKP3Pressed;
}				// int KP3Pressed(void)

int
KP4Pressed (void)
{
  keyboard_update ();
  return CurrentlyKP4Pressed;
}				// int KP4Pressed(void)

int
KP5Pressed (void)
{
  keyboard_update ();
  return CurrentlyKP5Pressed;
}				// int KP5Pressed(void)

int
KP6Pressed (void)
{
  keyboard_update ();
  return CurrentlyKP6Pressed;
}				// int KP6Pressed(void)

int
KP7Pressed (void)
{
  keyboard_update ();
  return CurrentlyKP7Pressed;
}				// int KP7Pressed(void)

int
KP8Pressed (void)
{
  keyboard_update ();
  return CurrentlyKP8Pressed;
}				// int KP8Pressed(void)

int
KP9Pressed (void)
{
  keyboard_update ();
  return CurrentlyKP9Pressed;
}				// int KP9Pressed(void)

int
F1Pressed (void)
{
  keyboard_update ();
  return CurrentlyF1Pressed;
}				// int F1Pressed(void)

int
F2Pressed (void)
{
  keyboard_update ();
  return CurrentlyF2Pressed;
}				// int F2Pressed(void)

int
F3Pressed (void)
{
  keyboard_update ();
  return CurrentlyF3Pressed;
}				// int F3Pressed(void)

int
F4Pressed (void)
{
  keyboard_update ();
  return CurrentlyF4Pressed;
}				// int F4Pressed(void)

int
F5Pressed (void)
{
  keyboard_update ();
  return CurrentlyF5Pressed;
}				// int F5Pressed(void)

int
F6Pressed (void)
{
  keyboard_update ();
  return CurrentlyF6Pressed;
}				// int F6Pressed(void)

int
F7Pressed (void)
{
  keyboard_update ();
  return CurrentlyF7Pressed;
}				// int F7Pressed(void)

int
F8Pressed (void)
{
  keyboard_update ();
  return CurrentlyF8Pressed;
}				// int F8Pressed(void)

int
F9Pressed (void)
{
  keyboard_update ();
  return CurrentlyF9Pressed;
}				// int F9Pressed(void)

int
F10Pressed (void)
{
  keyboard_update ();
  return CurrentlyF10Pressed;
}				// int F10Pressed(void)

int
F11Pressed (void)
{
  keyboard_update ();
  return CurrentlyF11Pressed;
}				// int F11Pressed(void)

int
F12Pressed (void)
{
  keyboard_update ();
  return CurrentlyF12Pressed;
}				// int F12Pressed(void)

int
LeftPressed (void)
{
  keyboard_update ();
  return CurrentlyLeftPressed;
}				// int LeftPressed(void)

int
RightPressed (void)
{
  keyboard_update ();
  return CurrentlyRightPressed;
}; // int RightPressed(void)

int
UpPressed (void)
{
  keyboard_update ();
  return CurrentlyUpPressed;
}				// int UpPressed(void)

int
DownPressed (void)
{
  keyboard_update ();
  return CurrentlyDownPressed;
}				// int DownPressed(void)

int
SpacePressed (void)
{
  keyboard_update ();
  return CurrentlySpacePressed;
}				// int SpacePressed(void)

int
EnterPressed (void)
{
  keyboard_update ();
  return CurrentlyEnterPressed;
}				// int SpacePressed(void)

int
BackspacePressed (void)
{
  keyboard_update ();
  return CurrentlyBackspacePressed;
}				// int SpacePressed(void)

int
LeftCtrlPressed (void)
{
  keyboard_update ();
  return CurrentlyLeftCtrlPressed;
}				// int SpacePressed(void)

int
APressed (void)
{
  keyboard_update ();
  return CurrentlyAPressed;
}				// int PPressed(void)

int
BPressed (void)
{
  keyboard_update ();
  return CurrentlyBPressed;
}				// int PPressed(void)

int
EPressed (void)
{
  keyboard_update ();
  return CurrentlyEPressed;
}				// int PPressed(void)

int
FPressed (void)
{
  keyboard_update ();
  return CurrentlyFPressed;
}				// int PPressed(void)

int
GPressed (void)
{
  keyboard_update ();
  return CurrentlyGPressed;
}				// int PPressed(void)

int
HPressed (void)
{
  keyboard_update ();
  return CurrentlyHPressed;
}				// int PPressed(void)

int
JPressed (void)
{
  keyboard_update ();
  return CurrentlyJPressed;
}				// int PPressed(void)

int
KPressed (void)
{
  keyboard_update ();
  return CurrentlyKPressed;
}				// int PPressed(void)

int
MPressed (void)
{
  keyboard_update ();
  return CurrentlyMPressed;
}				// int PPressed(void)

int
NPressed (void)
{
  keyboard_update ();
  return CurrentlyNPressed;
}				// int PPressed(void)

int
OPressed (void)
{
  keyboard_update ();
  return CurrentlyOPressed;
}				// int PPressed(void)

int
PPressed (void)
{
  keyboard_update ();
  return CurrentlyPPressed;
}				// int PPressed(void)

int
RPressed (void)
{
  keyboard_update ();
  return CurrentlyRPressed;
}				// int PPressed(void)

int
SPressed (void)
{
  keyboard_update ();
  return CurrentlySPressed;
}				// int PPressed(void)

int
TPressed (void)
{
  keyboard_update ();
  return CurrentlyTPressed;
}				// int PPressed(void)

int
QPressed (void)
{
  keyboard_update ();
  return CurrentlyQPressed;
}				// int QPressed(void)

int
WPressed (void)
{
  keyboard_update ();
  return CurrentlyWPressed;
}				// int WPressed(void)

int
DPressed (void)
{
  keyboard_update ();
  return CurrentlyDPressed;
}				// int WPressed(void)

int
LPressed (void)
{
  keyboard_update ();
  return CurrentlyLPressed;
}				// int WPressed(void)

int
IPressed (void)
{
  keyboard_update ();
  return CurrentlyIPressed;
}				// int WPressed(void)

int
VPressed (void)
{
  keyboard_update ();
  return CurrentlyVPressed;
}				// int WPressed(void)

int
CPressed (void)
{
  keyboard_update ();
  return CurrentlyCPressed;
}				// int WPressed(void)

int
UPressed (void)
{
  keyboard_update ();
  return CurrentlyUPressed;
}				// int WPressed(void)

int
XPressed (void)
{
  keyboard_update ();
  return CurrentlyXPressed;
}				// int WPressed(void)

int
YPressed (void)
{
  keyboard_update ();
  return CurrentlyYPressed;
}				// int WPressed(void)

int
ZPressed (void)
{
  keyboard_update ();
  return CurrentlyZPressed;
}				// int WPressed(void)

int
EscapePressed (void)
{
  keyboard_update ();
  return CurrentlyEscapePressed;
}				// int WPressed(void)

int
TabPressed (void)
{
  keyboard_update ();
  return CurrentlyTabPressed;
}				// int WPressed(void)

/* ----------------------------------------------------------------------
 * This function checks whether some direction key was pressed or not.
 * In case of a direction key pressed, FALSE will be returned, otherwise
 * TRUE of course.
 * ---------------------------------------------------------------------- */
int
NoDirectionPressed (void)
{
  if ( (axis_is_active && (input_axis.x || input_axis.y)) ||
      DownPressed () || UpPressed() || LeftPressed() || RightPressed() )
    return ( FALSE );
  else
    return ( TRUE );
}; // int NoDirectionPressed( void )


int
MouseRightPressed(void)
{
  // keyboard_update();  // DON'T UPDATE HERE, OR SOMETHING GOES WRONG WITH KEEPING TRACK
  // OF MOUSE STATUS IN THE PREVIOUS FRAMES!!!!
  return CurrentlyMouseRightPressed;
}

int
MouseLeftPressed(void)
{
  // keyboard_update();  // DON'T UPDATE HERE, OR SOMETHING GOES WRONG WITH KEEPING TRACK
  // OF MOUSE STATUS IN THE PREVIOUS FRAMES!!!!
  return CurrentlyMouseLeftPressed;
}

int
MouseWheelUpPressed(void)
{
  // keyboard_update();  // DON'T UPDATE HERE, OR SOMETHING GOES WRONG WITH KEEPING TRACK
  // OF MOUSE STATUS IN THE PREVIOUS FRAMES!!!!
  if ( MouseWheelUpMovesRecorded )
    {
      MouseWheelUpMovesRecorded--;
      return ( TRUE );
    }
  else
    return ( FALSE );
  // return CurrentlyMouseWheelUpPressed;
}

int
MouseWheelDownPressed(void)
{
  // keyboard_update();  // DON'T UPDATE HERE, OR SOMETHING GOES WRONG WITH KEEPING TRACK
  // OF MOUSE STATUS IN THE PREVIOUS FRAMES!!!!
  if ( MouseWheelDownMovesRecorded )
    {
      MouseWheelDownMovesRecorded--;
      return ( TRUE );
    }
  else
    return ( FALSE );
  // return CurrentlyMouseWheelDownPressed;
}


#undef _intput_c
