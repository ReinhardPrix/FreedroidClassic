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

Uint8 * key_pressed_real_frame; /* SDL internal arrey */
Uint8 * key_pressed_this_frame; /* This one is allocated by us and is the 'current state' */
Uint8 * key_pressed_last_frame; /* This one is allocated by us and is the 'last' state*/
int key_pressed_array_size;

SDLMod modifiers_this_frame;
SDLMod modifiers_last_frame;

Uint8 mouse_state_last_frame;
Uint8 mouse_state_this_frame;
;
int CurrentlyMouseWheelUpPressed;
int CurrentlyMouseWheelDownPressed;
int MouseWheelUpMovesRecorded;
int MouseWheelDownMovesRecorded;

int realSDLK_0 = SDLK_0;
int realSDLK_1 = SDLK_1;
int realSDLK_2 = SDLK_2;
int realSDLK_3 = SDLK_3;
int realSDLK_4 = SDLK_4;
int realSDLK_5 = SDLK_5;
int realSDLK_6 = SDLK_6;
int realSDLK_7 = SDLK_7;
int realSDLK_8 = SDLK_8;
int realSDLK_9 = SDLK_9;

void init_keyboard_input_array()
{
key_pressed_real_frame = SDL_GetKeyState(&key_pressed_array_size);
key_pressed_this_frame = (Uint8 *)calloc(1, key_pressed_array_size);
key_pressed_last_frame = (Uint8 *)calloc(1, key_pressed_array_size);
}

void free_keyboard_input_array()
{
free(key_pressed_last_frame);
free(key_pressed_this_frame);
}

/* ----------------------------------------------------------------------
 * This function returns the current mouse position in x direction.
 * Depending on the current shape of the mouse cursor, the real 'pointer'
 * position of the mouse cursor is returned.
 * ---------------------------------------------------------------------- */
int 
GetMousePos_x(void)
{
/*    if ( current_mouse_cursor_shape == MOUSE_CURSOR_CROSSHAIR_SHAPE )
	return( CurrentMouseAbsPos . x + 16 );
    else*/
	return( CurrentMouseAbsPos . x + 0 );
}; // int GetMousePos_x(void)
int 
GetMousePos_y(void)
{
/*    if ( current_mouse_cursor_shape == MOUSE_CURSOR_CROSSHAIR_SHAPE )
	return( CurrentMouseAbsPos . y + 16 );
    else*/
	return( CurrentMouseAbsPos . y + 0 );
}; // int GetMousePos_y(void)

/* ----------------------------------------------------------------------
 * This should initialize the joystick, if such a device is present.
 * ---------------------------------------------------------------------- */
void 
Init_Joy ( void )
{
    int num_joy;

    if ( SDL_InitSubSystem ( SDL_INIT_JOYSTICK ) == (-1) )
    {
	DebugPrintf ( 0 , "Warning: Couldn't initialize SDL-Joystick: %s\n", SDL_GetError() );
	return;
    } 
    else
	DebugPrintf( 1 , "\nSDL Joystick initialisation successful.\n" );
    
    DebugPrintf ( 1 , " %d Joysticks found!\n" , num_joy = SDL_NumJoysticks () );
    
    if ( num_joy > 0 )
	joy = SDL_JoystickOpen (0);
    
    if ( joy )
    {
	DebugPrintf (1, "Identifier: %s\n", SDL_JoystickName (0));
	DebugPrintf (1, "Number of Axes: %d\n", joy_num_axes = SDL_JoystickNumAxes(joy));
	DebugPrintf (1, "Number of Buttons: %d\n", SDL_JoystickNumButtons(joy));
	
	// aktivate Joystick event handling 
	SDL_JoystickEventState (SDL_ENABLE); 
    }
    else 
	joy = NULL;  // signals that no yoystick is present 
    
}; // void Init_Joy ( void )

/* ----------------------------------------------------------------------
 * Here we insert some cheat codes for assigning and completing missions.
 * ---------------------------------------------------------------------- */
void
mission_status_cheat_handling ( int player_num )
{

    if ( Number0Pressed() )
    {
	if ( CPressed() )
	{
	    Me [ player_num ] . AllMissions [ 0 ] . MissionWasAssigned = TRUE ;
	    Me [ player_num ] . AllMissions [ 0 ] . MissionIsComplete = TRUE ;
	}
	if ( APressed() )
	{
	    Me [ player_num ] . AllMissions [ 0 ] . MissionWasAssigned = TRUE ;
	    Me [ player_num ] . AllMissions [ 0 ] . MissionIsComplete = FALSE ;
	}
    }
    if ( Number1Pressed() )
    {
	if ( CPressed() )
	{
	    Me [ player_num ] . AllMissions [ 1 ] . MissionWasAssigned = TRUE ;
	    Me [ player_num ] . AllMissions [ 1 ] . MissionIsComplete = TRUE ;
	}
	if ( APressed() )
	{
	    Me [ player_num ] . AllMissions [ 1 ] . MissionWasAssigned = TRUE ;
	    Me [ player_num ] . AllMissions [ 1 ] . MissionIsComplete = FALSE ;
	}
    }
    if ( Number2Pressed() )
    {
	if ( CPressed() )
	{
	    Me [ player_num ] . AllMissions [ 2 ] . MissionWasAssigned = TRUE ;
	    Me [ player_num ] . AllMissions [ 2 ] . MissionIsComplete = TRUE ;
	}
	if ( APressed() )
	{
	    Me [ player_num ] . AllMissions [ 2 ] . MissionWasAssigned = TRUE ;
	    Me [ player_num ] . AllMissions [ 2 ] . MissionIsComplete = FALSE ;
	}
    }
    if ( Number3Pressed() )
    {
	if ( CPressed() )
	{
	    Me [ player_num ] . AllMissions [ 3 ] . MissionWasAssigned = TRUE ;
	    Me [ player_num ] . AllMissions [ 3 ] . MissionIsComplete = TRUE ;
	}
	if ( APressed() )
	{
	    Me [ player_num ] . AllMissions [ 3 ] . MissionWasAssigned = TRUE ;
	    Me [ player_num ] . AllMissions [ 3 ] . MissionIsComplete = FALSE ;
	}
    }
    if ( Number4Pressed() )
    {
	if ( CPressed() )
	{
	    Me [ player_num ] . AllMissions [ 4 ] . MissionWasAssigned = TRUE ;
	    Me [ player_num ] . AllMissions [ 4 ] . MissionIsComplete = TRUE ;
	}
	if ( APressed() )
	{
	    Me [ player_num ] . AllMissions [ 4 ] . MissionWasAssigned = TRUE ;
	    Me [ player_num ] . AllMissions [ 4 ] . MissionIsComplete = FALSE ;
	}
    }
    if ( Number5Pressed() )
    {
	if ( CPressed() )
	{
	    Me [ player_num ] . AllMissions [ 5 ] . MissionWasAssigned = TRUE ;
	    Me [ player_num ] . AllMissions [ 5 ] . MissionIsComplete = TRUE ;
	}
	if ( APressed() )
	{
	    Me [ player_num ] . AllMissions [ 5 ] . MissionWasAssigned = TRUE ;
	    Me [ player_num ] . AllMissions [ 5 ] . MissionIsComplete = FALSE ;
	}
    }
    if ( Number6Pressed() )
    {
	if ( CPressed() )
	{
	    Me [ player_num ] . AllMissions [ 6 ] . MissionWasAssigned = TRUE ;
	    Me [ player_num ] . AllMissions [ 6 ] . MissionIsComplete = TRUE ;
	}
	if ( APressed() )
	{
	    Me [ player_num ] . AllMissions [ 6 ] . MissionWasAssigned = TRUE ;
	    Me [ player_num ] . AllMissions [ 6 ] . MissionIsComplete = FALSE ;
	}
    }
    if ( Number7Pressed() )
    {
	if ( CPressed() )
	{
	    Me [ player_num ] . AllMissions [ 7 ] . MissionWasAssigned = TRUE ;
	    Me [ player_num ] . AllMissions [ 7 ] . MissionIsComplete = TRUE ;
	}
	if ( APressed() )
	{
	    Me [ player_num ] . AllMissions [ 7 ] . MissionWasAssigned = TRUE ;
	    Me [ player_num ] . AllMissions [ 7 ] . MissionIsComplete = FALSE ;
	}
    }
    if ( Number8Pressed() )
    {
	if ( CPressed() )
	{
	    Me [ player_num ] . AllMissions [ 8 ] . MissionWasAssigned = TRUE ;
	    Me [ player_num ] . AllMissions [ 8 ] . MissionIsComplete = TRUE ;
	}
	if ( APressed() )
	{
	    Me [ player_num ] . AllMissions [ 8 ] . MissionWasAssigned = TRUE ;
	    Me [ player_num ] . AllMissions [ 8 ] . MissionIsComplete = FALSE ;
	}
    }

}; // void mission_status_cheat_handling ( int player_num )

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
	if ( ShiftWasPressed() )
	    DropRandomItem( Me [ 0 ] . pos . z , Me [ 0 ] . pos . x , Me [ 0 ] . pos . y , 3 , TRUE  , FALSE , FALSE ) ;
	else
	    DropRandomItem( Me [ 0 ] . pos . z , Me [ 0 ] . pos . x , Me [ 0 ] . pos . y , 3 , FALSE , FALSE , FALSE ) ;
	
	//--------------------
	// here our cheat key for immediately forcing respawning of the
	// enemies on this level...
	//
	if ( AltWasPressed() )
	{
	    respawn_level ( Me [ 0 ] . pos . z );
	}
	
	while ( RPressed() );
    }
    //--------------------
    // For identifying items, there is a quick way too...
    //
    if ( IPressed() && ShiftWasPressed() )
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
	if ( ShiftWasPressed() )
	    Me [ 0 ] . base_strength += 5 ;
	else
	{
	    if ( Me [ 0 ] . melee_weapon_skill < 8 )  Me [ 0 ] . melee_weapon_skill ++ ;
	}
    }
    if ( KP8Pressed() )
    {
	while (KP8Pressed());
	if ( ShiftWasPressed() )
	    Me [ 0 ] . base_magic += 5 ;
	else
	{
	    if ( Me [ 0 ] . ranged_weapon_skill < 8 ) Me [ 0 ] . ranged_weapon_skill ++ ;
	}
    }
    if ( KP7Pressed() )
    {
	while ( KP7Pressed() );
	if ( ShiftWasPressed() )
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
    mission_status_cheat_handling ( 0 );

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
	    if ( Me [ 0 ] . map_maker_is_present )
	    {
		if ( GameConfig.Automap_Visible )
		    append_new_game_message ( "Automap ON." );
		else
		    append_new_game_message ( "Automap OFF." );
	    }
	    else
	    {
		append_new_game_message ( "Sorry, you don't have automap yet:  map maker item not present." );
	    }

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
    //--------------------
    // To quick-apply the items in the very lowest row of the
    // inventory screen, the number keys 1-9 can be used.
    //
    if ( Number1Hit() )
    {
	Quick_ApplyItem ( 1 );
    }

    if ( Number2Hit() )
    {
	Quick_ApplyItem ( 2 );
    }

    if ( Number3Hit() )
    {
	Quick_ApplyItem ( 3 );
    }

    if ( Number4Hit() )
    {
	Quick_ApplyItem ( 4 );
    }
    if ( Number5Hit() )
    {
	Quick_ApplyItem ( 5 );
    }
    if ( Number6Hit() )
    {
	Quick_ApplyItem ( 6 );
    }
    if ( Number7Hit() )
    {
	Quick_ApplyItem ( 7 );
    }
    if ( Number8Hit() )
    {
	Quick_ApplyItem ( 8 );
    }
    if ( Number9Hit() )
    {
	Quick_ApplyItem ( 9 );
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
	    GameConfig . skill_explanation_screen_visible = FALSE;
	    break;
	case GAME_CONFIG_SCREEN_VISIBLE_SKILLS :
	    GameConfig . SkillScreen_Visible = !GameConfig . SkillScreen_Visible;
	    GameConfig . CharacterScreen_Visible = FALSE ;
	    break;
	case GAME_CONFIG_SCREEN_VISIBLE_CHARACTER :
	    GameConfig . CharacterScreen_Visible = !GameConfig . CharacterScreen_Visible;
	    GameConfig . SkillScreen_Visible = FALSE ;
	    break;
	case GAME_CONFIG_SCREEN_VISIBLE_SKILL_EXPLANATION :
            GameConfig.skill_explanation_screen_visible = !GameConfig.skill_explanation_screen_visible;
	    GameConfig . Inventory_Visible = FALSE;
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
    static int RPressed_LastFrame;
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

    //--------------------
    // We assign the R key to weapon reloading
    //
    if ( RPressed() )
    {
	if ( !RPressed_LastFrame ) 
	{
	    TuxReloadWeapon();
	}
	RPressed_LastFrame = TRUE;
    }
    else
    {
	RPressed_LastFrame = FALSE;
    }
    
}; // void handle_cha_ski_inv_screen_on_off_keys ( void )

/* ----------------------------------------------------------------------
 * This function does the reactions to keypresses of the player other
 * than pressing cursor keys.
 * ---------------------------------------------------------------------- */
void 
ReactToSpecialKeys(void)
{
    
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
	
	if ( CtrlWasPressed ( ) && AltWasPressed ( ) && ShiftWasPressed ( ) )
	{
	    DebugPrintf ( -10 , "\nForcefully flushing image cache now --> all backgrounds will be reloaded." );
	    flush_background_image_cache ( ) ;
	}
	
	while ( FPressed ( ) );
    }
    
    //--------------------
    // We assign the L key to turn on/off the quest log i.e. mission log
    //
    if ( LHit() )
    {
	GameConfig.Mission_Log_Visible_Time = 0;
	GameConfig.Mission_Log_Visible = !GameConfig.Mission_Log_Visible;
	DebugPrintf( 0 , "\nMISSION LOG TURNED ON!\n" );
    }
    
    
    //--------------------
    // We assign the Space key to turn off all windows and quest log
    // and also when the Tux is dead, there shouldn't be any windows any more.
    //
    if ( ( SpacePressed( ) && !MouseLeftPressed() ) ||
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
    if ( CPressed() && AltPressed()
	 && CtrlPressed() && ShiftPressed() ) 
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
     if ( THit() ) 	 
     { 	 
         GameConfig.transparency = ! GameConfig.transparency; 	 
     }    

    if ( ZHit())
	{
	always_show_items_text = ! always_show_items_text;
	}
    
}; // void ReactToSpecialKeys(void)


void track_last_frame_input_status()
{
memcpy(key_pressed_last_frame, key_pressed_this_frame, sizeof(Uint8) * key_pressed_array_size);
memcpy(key_pressed_this_frame, key_pressed_real_frame, sizeof(Uint8) * key_pressed_array_size);
mouse_state_last_frame = mouse_state_this_frame;
mouse_state_this_frame = SDL_GetMouseState(NULL, NULL);
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
		printf("\n\nUser requested termination...\n\nTerminating...");
		Terminate(0);
		break;
		/* Look for a keypress */

	    case SDL_KEYDOWN:
	    case SDL_KEYUP:
		//We have a pointer towards the internal array already, nothing to do.
		#ifndef WIN32
		if (event.key.keysym.scancode > 19 || event.key.keysym.scancode < 10) break;
		else switch ( event.key.keysym.scancode )
			{
			case 10: realSDLK_1 = event.key.keysym.sym; break;
			case 11: realSDLK_2 = event.key.keysym.sym; break;
			case 12: realSDLK_3 = event.key.keysym.sym; break;
			case 13: realSDLK_4 = event.key.keysym.sym; break;
			case 14: realSDLK_5 = event.key.keysym.sym; break;
			case 15: realSDLK_6 = event.key.keysym.sym; break;
			case 16: realSDLK_7 = event.key.keysym.sym; break;
			case 17: realSDLK_8 = event.key.keysym.sym; break;
			case 18: realSDLK_9 = event.key.keysym.sym; break;
			case 19: realSDLK_0 = event.key.keysym.sym; break;
			}
		#endif
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
/*		if ( current_mouse_cursor_shape == MOUSE_CURSOR_CROSSHAIR_SHAPE )
		{
		    input_axis . x += 16 ; 
		    input_axis . y += 16 ; 
		    CurrentMouseAbsPos . x += 16 ;
		    CurrentMouseAbsPos . y += 16 ;
		}*/
		
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
/*		if ( current_mouse_cursor_shape == MOUSE_CURSOR_CROSSHAIR_SHAPE )
		{
		    input_axis . x += 16 ; 
		    input_axis . y += 16 ; 
		    CurrentMouseAbsPos . x += 16 ;
		    CurrentMouseAbsPos . y += 16 ;
		}*/
		
		if ( ( ClientMode ) && ( ! ServerMode ) ) SendPlayerMouseButtonEventToServer ( event );
		
		if ( event.button.button == SDL_BUTTON_LEFT )
			;		
		if ( event.button.button == SDL_BUTTON_RIGHT )	   
			;		
		//--------------------
		// We need to add come conditional compilation here, so that 
		// on some systems, where the SDL version is < 1.2.5 the code
		// still compiles without much trouble. (At least so we hope :)
		//
#ifdef SDL_BUTTON_WHEELUP 	
		if ( event.button.button == SDL_BUTTON_WHEELUP )
		{
		    CurrentlyMouseWheelUpPressed = TRUE;
	 	    MouseWheelUpMovesRecorded++;
		}
		if ( event.button.button == SDL_BUTTON_WHEELDOWN )
		{
		    CurrentlyMouseWheelDownPressed = TRUE;
		    MouseWheelDownMovesRecorded++;
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
			;
		
		if (event.button.button == SDL_BUTTON_RIGHT)
			;
		
		//--------------------
		// We need to add come conditional compilation here, so that 
		// on some systems, where the SDL version is < 1.2.5 the code
		// still compiles without much trouble. (At least so we hope :)
		//
#ifdef SDL_BUTTON_WHEELUP 
		if ( event.button.button == SDL_BUTTON_WHEELUP )
		{
		    CurrentlyMouseWheelUpPressed = FALSE ;
		}
		if ( event.button.button == SDL_BUTTON_WHEELDOWN )
		{
		    CurrentlyMouseWheelDownPressed = FALSE;
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
	if (event.type == SDL_KEYUP) ;
	else
	{
	    SDL_PushEvent (&event);  /* put this event back into the queue */
	    keyboard_update ();  /* and treat it the usual way */
	    continue;
	}
	
    } /* while(1) */
    
} /* getchar_raw() */


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


int LeftPressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_LEFT]));}
int RightPressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_RIGHT]));}

int LeftWasPressed () { keyboard_update(); return ((key_pressed_last_frame[SDLK_LEFT]));}
int RightWasPressed () { keyboard_update(); return ((key_pressed_last_frame[SDLK_RIGHT]));}

int UpPressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_UP]));}
int DownPressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_DOWN]));}
int SpacePressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_SPACE]));}
int EnterPressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_RETURN]));}
int EscapePressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_ESCAPE]));}
int TabPressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_TAB]));}
int BackspacePressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_BACKSPACE]));}
int LeftCtrlPressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_LCTRL]));}

int CtrlPressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_LCTRL]) || key_pressed_real_frame[SDLK_RCTRL]);}
int CtrlWasPressed () { keyboard_update(); return ((key_pressed_last_frame[SDLK_RCTRL] || key_pressed_last_frame[SDLK_LCTRL]));}

int ShiftPressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_RSHIFT] || key_pressed_real_frame[SDLK_LSHIFT]));}
int ShiftWasPressed () { keyboard_update(); return ((key_pressed_last_frame[SDLK_RSHIFT] || key_pressed_last_frame[SDLK_LSHIFT]));}

int AltPressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_RALT] || key_pressed_real_frame[SDLK_LALT])); }
int AltWasPressed () { keyboard_update(); return ((key_pressed_last_frame[SDLK_RALT] || key_pressed_last_frame[SDLK_LALT]));}

int KP_PLUS_Pressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_KP_PLUS]));}
int KP_MINUS_Pressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_KP_MINUS]));}
int KP_MULTIPLY_Pressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_KP_MULTIPLY]));}
int KP_DIVIDE_Pressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_KP_DIVIDE]));}
int Number0Pressed () { keyboard_update(); return ((key_pressed_real_frame[realSDLK_0]));}
int Number0Hit () { return (!(key_pressed_last_frame[realSDLK_0]) && (key_pressed_this_frame[realSDLK_0])); }
int Number1Pressed () { keyboard_update(); return ((key_pressed_real_frame[realSDLK_1])) ;}
int Number1Hit () { return (!(key_pressed_last_frame[realSDLK_1]) && (key_pressed_this_frame[realSDLK_1])); }
int Number2Pressed () { keyboard_update(); return ((key_pressed_real_frame[realSDLK_2]));}
int Number2Hit () { return (!(key_pressed_last_frame[realSDLK_2]) && (key_pressed_this_frame[realSDLK_2])); }
int Number3Pressed () { keyboard_update(); return ((key_pressed_real_frame[realSDLK_3])) ;}
int Number3Hit () { return (!(key_pressed_last_frame[realSDLK_3]) && (key_pressed_this_frame[realSDLK_3])); }
int Number4Pressed () { keyboard_update(); return ((key_pressed_real_frame[realSDLK_4])) ;}
int Number4Hit () { return (!(key_pressed_last_frame[realSDLK_4]) && (key_pressed_this_frame[realSDLK_4])); }
int Number5Pressed () { keyboard_update(); return ((key_pressed_real_frame[realSDLK_5]));}
int Number5Hit () { return (!(key_pressed_last_frame[realSDLK_5]) && (key_pressed_this_frame[realSDLK_5])); }
int Number6Pressed () { keyboard_update(); return ((key_pressed_real_frame[realSDLK_6])) ;}
int Number6Hit () { return (!(key_pressed_last_frame[realSDLK_6]) && (key_pressed_this_frame[realSDLK_6])); }
int Number7Pressed () { keyboard_update(); return ((key_pressed_real_frame[realSDLK_7])) ;}
int Number7Hit () { return (!(key_pressed_last_frame[realSDLK_7]) && (key_pressed_this_frame[realSDLK_7])); }
int Number8Pressed () { keyboard_update(); return ((key_pressed_real_frame[realSDLK_8]));}
int Number8Hit () { return (!(key_pressed_last_frame[realSDLK_8]) && (key_pressed_this_frame[realSDLK_8])); }
int Number9Pressed () { keyboard_update(); return ((key_pressed_real_frame[realSDLK_9])) ;}
int Number9Hit () { return (!(key_pressed_last_frame[realSDLK_9]) && (key_pressed_this_frame[realSDLK_9])); }
int KP0Pressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_KP0])) ;}
int KP1Pressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_KP1]));}
int KP2Pressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_KP2])) ;}
int KP3Pressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_KP3])) ;}
int KP4Pressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_KP4]));}
int KP5Pressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_KP5])) ;}
int KP6Pressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_KP6])) ;}
int KP7Pressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_KP7]));}
int KP8Pressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_KP8])) ;}
int KP9Pressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_KP9])) ;}
int F1Pressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_F1])) ;}
int F2Pressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_F2])) ;}
int F3Pressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_F3])) ;}
int F4Pressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_F4]));}
int F5Pressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_F5])) ;}
int F6Pressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_F6])) ;}
int F7Pressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_F7]));}
int F8Pressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_F8])) ;}
int F9Pressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_F9]));}
int F10Pressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_F10]));}
int F11Pressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_F11])) ;}
int F12Pressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_F12])) ;}
int APressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_a]));}
int BPressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_b])) ;}
int CPressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_c])) ;}
int DPressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_d]));}
int EPressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_e])) ;}
int FPressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_f])) ;}
int GPressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_g]));}
int HPressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_h])) ;}
int IPressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_i])) ;}
int JPressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_j]));}
int KPressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_k])) ;}
int LPressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_l])) ;}
int LHit () { return (!(key_pressed_last_frame[SDLK_l]) && (key_pressed_this_frame[SDLK_l])); }
int MPressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_m]));}
int NPressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_n])) ;}
int OPressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_o])) ;}
int PPressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_p]));}
int QPressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_q])) ;}
int RPressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_r])) ;}
int SPressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_s]));}
int TPressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_t])) ;}
int THit () { return (!(key_pressed_last_frame[SDLK_t]) && (key_pressed_this_frame[SDLK_t])); }
int UPressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_u])) ;}
int VPressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_v]));}
int WPressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_w])) ;}
int XPressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_x])) ;}
int YPressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_y]));}
int ZPressed () { keyboard_update(); return ((key_pressed_real_frame[SDLK_z]));}
int ZWasPressed () { keyboard_update(); return ((key_pressed_last_frame[SDLK_z]));}
int ZHit () { return (!(key_pressed_last_frame[SDLK_z]) && (key_pressed_this_frame[SDLK_z])); }
int MouseRightPressed() { keyboard_update(); return ((SDL_GetMouseState(NULL, NULL))&(SDL_BUTTON(3))); }
int MouseLeftPressed() { keyboard_update(); return (SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1));}
int MouseRightClicked() { return (!(mouse_state_last_frame&SDL_BUTTON(3)) && (mouse_state_this_frame&SDL_BUTTON(3))); }
int MouseLeftClicked() { return (!(mouse_state_last_frame&SDL_BUTTON(1)) && (mouse_state_this_frame&SDL_BUTTON(1))); }
#undef _input_c

