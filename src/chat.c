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
 * This file contains all functions dealing with the dialog interface,
 * including blitting the chat protocol to the screen and drawing the
 * right portrait images to the screen.
 * ---------------------------------------------------------------------- */

#define _chat_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "proto.h"
#include "global.h"
#include "SDL_rotozoom.h"


#define PUSH_ROSTER 2
#define POP_ROSTER 3 

dialogue_option ChatRoster[MAX_DIALOGUE_OPTIONS_IN_ROSTER];
EXTERN char *PrefixToFilename[ ENEMY_ROTATION_MODELS_AVAILABLE ];
char* chat_protocol = NULL ;

void DoChatFromChatRosterData( int PlayerNum , int ChatPartnerCode , Enemy ChatDroid , int ClearProtocol );

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
push_or_pop_chat_roster ( int push_or_pop )
{
    static dialogue_option LocalChatRoster[MAX_DIALOGUE_OPTIONS_IN_ROSTER];
    
    if ( push_or_pop == PUSH_ROSTER )
    {
	memcpy ( LocalChatRoster , ChatRoster , sizeof ( dialogue_option ) * MAX_DIALOGUE_OPTIONS_IN_ROSTER ) ;
    }
    else if ( push_or_pop == POP_ROSTER )
    {
	memcpy ( ChatRoster , LocalChatRoster , sizeof ( dialogue_option ) * MAX_DIALOGUE_OPTIONS_IN_ROSTER ) ;
    }
    else
    {
	GiveStandardErrorMessage ( __FUNCTION__  , "\
There was an unrecognized parameter handled to this function." ,
				   PLEASE_INFORM, IS_FATAL );
    }
    
}; // push_or_pop_chat_roster ( int push_or_pop )


/* ----------------------------------------------------------------------
 * This function finds the index of the array where the chat flags for
 * this person are stored.  It does this by exploiting on the (unique?)
 * dialog section to use entry of each (friendly) droid.
 * ---------------------------------------------------------------------- */
int
ResolveDialogSectionToChatFlagsIndex ( char* SectionName )
{
    if ( strcmp ( SectionName , "Chandra" ) == 0 ) return PERSON_CHA ;
    if ( strcmp ( SectionName , "Sorenson" ) == 0 ) return PERSON_SORENSON;
    if ( strcmp ( SectionName , "614" ) == 0 ) return PERSON_614;
    if ( strcmp ( SectionName , "Stone" ) == 0 ) return PERSON_STONE;
    if ( strcmp ( SectionName , "Pendragon" ) == 0 ) return PERSON_PENDRAGON;
    if ( strcmp ( SectionName , "Dixon" ) == 0 ) return PERSON_DIXON;
    if ( strcmp ( SectionName , "RMS" ) == 0 ) return PERSON_RMS;
    if ( strcmp ( SectionName , "MER" ) == 0 ) return PERSON_MER;
    if ( strcmp ( SectionName , "Francis" ) == 0 ) return PERSON_FRANCIS;
    if ( strcmp ( SectionName , "Ernie" ) == 0 ) return PERSON_ERNIE;
    if ( strcmp ( SectionName , "Bruce" ) == 0 ) return PERSON_BRUCE;
    if ( strcmp ( SectionName , "Benjamin" ) == 0 ) return PERSON_BENJAMIN;
    if ( strcmp ( SectionName , "Bender" ) == 0 ) return PERSON_BENDER;
    if ( strcmp ( SectionName , "Spencer" ) == 0 ) return PERSON_SPENCER;
    if ( strcmp ( SectionName , "Butch" ) == 0 ) return PERSON_BUTCH;
    if ( strcmp ( SectionName , "Darwin" ) == 0 ) return PERSON_DARWIN;
    if ( strcmp ( SectionName , "Duncan" ) == 0 ) return PERSON_DUNCAN;
    if ( strcmp ( SectionName , "DocMoore" ) == 0 ) return PERSON_DOC_MOORE;
    if ( strcmp ( SectionName , "Melfis" ) == 0 ) return PERSON_MELFIS;
    if ( strcmp ( SectionName , "Michelangelo" ) == 0 ) return PERSON_MICHELANGELO;
    if ( strcmp ( SectionName , "Skippy" ) == 0 ) return PERSON_SKIPPY;
    if ( strcmp ( SectionName , "StandardOldTownGateGuard" ) == 0 ) return PERSON_STANDARD_OLD_TOWN_GATE_GUARD;
    if ( strcmp ( SectionName , "StandardNewTownGateGuard" ) == 0 ) return PERSON_STANDARD_NEW_TOWN_GATE_GUARD;
    if ( strcmp ( SectionName , "OldTownGateGuardLeader" ) == 0 ) return PERSON_OLD_TOWN_GATE_GUARD_LEADER;
    if ( strcmp ( SectionName , "StandardMSFacilityGateGuard" ) == 0 ) return PERSON_STANDARD_MS_FACILITY_GATE_GUARD;
    if ( strcmp ( SectionName , "MSFacilityGateGuardLeader" ) == 0 ) return PERSON_MS_FACILITY_GATE_GUARD_LEADER;
    if ( strcmp ( SectionName , "HEA" ) == 0 ) return PERSON_HEA;
    if ( strcmp ( SectionName , "StandardBotAfterTakeover" ) == 0 ) return PERSON_STANDARD_BOT_AFTER_TAKEOVER;
    if ( strcmp ( SectionName , "Tybalt" ) == 0 ) return PERSON_TYBALT;
    if ( strcmp ( SectionName , "Ewald" ) == 0 ) return PERSON_EWALD;
    if ( strcmp ( SectionName , "KevinGuard" ) == 0 ) return PERSON_KEVINS_GUARD;
    if ( strcmp ( SectionName , "Kevin" ) == 0 ) return PERSON_KEVIN;
    if ( strcmp ( SectionName , "Jasmine" ) == 0 ) return PERSON_JASMINE;
    if ( strcmp ( SectionName , "Lukas" ) == 0 ) return PERSON_LUKAS;

    if ( strncmp ( SectionName , "subdlg_" , 6 ) == 0 ) return PERSON_SUBDIALOG_DUMMY;

    DebugPrintf ( -1000 , "\n--------------------\nSectionName: %s." , SectionName );
    GiveStandardErrorMessage ( __FUNCTION__  , "\
There was a dialogue section to be used with a droid, that does not have a \n\
corresponding chat flags array index." ,
			       PLEASE_INFORM, IS_FATAL );
    return (-1);
    
}; // int ResolveDialogSectionToChatFlagsIndex ( Enemy ChatDroid )


/* ----------------------------------------------------------------------
 * This function plants a cookie, i.e. sets a new text string with the
 * purpose of serving as a flag.  These flags can be set from the dialog
 * file and used from within there and they get stored and loaded with
 * every gave via the tux_t structure.
 * ---------------------------------------------------------------------- */
void
PlantCookie ( char* CookieString , int PlayerNum )
{
    int i;
    
    //--------------------
    // First a security check against attempts to plant too long cookies...
    //
    if ( strlen ( CookieString ) >= MAX_COOKIE_LENGTH -1 )
    {
	fprintf( stderr, "\n\nCookieString: %s\n" , CookieString );
	GiveStandardErrorMessage ( __FUNCTION__  , "\
There was a cookie given that exceeds the maximal length allowed for a\n\
cookie to be set in FreedroidRPG.",
				   PLEASE_INFORM, IS_FATAL );
    }
    
    //--------------------
    // Maybe the cookie string received still has the ':' attached at
    // the end.  In this case we first remove the ':'.
    //
    if ( strlen ( CookieString ) > 1 )
    {
	DebugPrintf ( 1 , "\nLast character of cookie text received: %c." , 
		      CookieString [ strlen ( CookieString ) - 1 ] );
	if ( CookieString [ strlen ( CookieString ) - 1 ] == ':' )
	{
	    CookieString [ strlen ( CookieString ) - 1 ] = 0 ;
	    DebugPrintf ( 1 , "\nRemoving trailing ':' character from cookie text..." , 
			  CookieString [ strlen ( CookieString ) - 1 ] );
	}
    }
    
    //--------------------
    // Check if maybe the cookie has already been set.  In this case we would
    // not have to do anything...
    //
    for ( i = 0 ; i < MAX_COOKIES ; i ++ )
    {
	if ( strlen ( Me [ PlayerNum ] . cookie_list [ i ] ) <= 0 )
	{
	    if ( !strcmp ( Me [ PlayerNum ] . cookie_list [ i ] , CookieString ) )
	    {
		DebugPrintf ( 0 , "\n\nTHAT COOKIE WAS ALREADY SET... DOING NOTHING...\n\n" );
		return;
	    }
	}
    }
    
    //--------------------
    // Now we find a good new and free position for our new cookie...
    //
    for ( i = 0 ; i < MAX_COOKIES ; i ++ )
    {
	if ( strlen ( Me [ PlayerNum ] . cookie_list [ i ] ) <= 0 )
	{
	    break;
	}
    }
    
    //--------------------
    // Maybe the position we have found is the last one.  That would mean too
    // many cookies, a case that should never occur in FreedroidRPG and that is
    // a considered a fatal error...
    //
    if ( i >= MAX_COOKIES ) 
    {
	fprintf( stderr, "\n\nCookieString: %s\n" , CookieString );
	GiveStandardErrorMessage ( __FUNCTION__  , "\
There were no more free positions available to store this cookie.\n\
This should not be possible without a severe bug in FreedroidRPG.",
				   PLEASE_INFORM, IS_FATAL );
    }
    
    //--------------------
    // Now that we know that we have found a good position for storing our
    // new cookie, we can do it.
    //
    strcpy ( Me [ PlayerNum ] . cookie_list [ i ] , CookieString );
    DebugPrintf ( 0 , "\n\nNEW COOKIE STORED:  Position=%d Text='%s'.\n\n" , 
		  i , CookieString );
    
    
}; // void PlantCookie ( char* CookieString , int PlayerNum )

/* ----------------------------------------------------------------------
 * This function restores all chat-with-friendly-droid variables to their
 * initial values.  This means, that NOT ALL FLAGS CAN BE SET HERE!!  Some
 * of them must remain at their current values!!! TAKE CARE!!
 * ---------------------------------------------------------------------- */
void
RestoreChatVariableToInitialValue( int PlayerNum )
{
    int j;
    
    //--------------------
    // You can always ask the moron 614 bots the same things and they
    // will always respond in the very same manner, so no need to
    // remember anything that has been talked in any previous conversation
    // with them.
    //
    Me [ PlayerNum ] . Chat_Flags [ PERSON_614 ] [ 0 ] = 1 ;
    Me [ PlayerNum ] . Chat_Flags [ PERSON_614 ] [ 1 ] = 1 ;
    Me [ PlayerNum ] . Chat_Flags [ PERSON_614 ] [ 2 ] = 1 ;
    Me [ PlayerNum ] . Chat_Flags [ PERSON_614 ] [ 3 ] = 1 ;
    
    //--------------------
    // The 'END' options should always be availabe for all dialogs
    // at the beginning.  THIS we know.
    //
    for ( j = 0 ; j < MAX_PERSONS ; j++ )
    {
	Me [ PlayerNum ] . Chat_Flags [ j ] [ END_ANSWER ] = 1 ;
    }
    
}; // void RestoreChatVariableToInitialValue( int PlayerNum )

/* ----------------------------------------------------------------------
 * This function displays a subtitle for the ChatWithFriendlyDroid interface,
 * so that you can also understand everything with sound disabled.
 * ---------------------------------------------------------------------- */
void
DisplaySubtitle( char* SubtitleText , int subtitle_background )
{
    SDL_Rect Subtitle_Window;
    
    //--------------------
    // If the user has disabled the subtitles in the dialogs, we need
    // not do anything and just return...
    // 
    if ( ! GameConfig . show_subtitles_in_dialogs ) return;
    
    //--------------------
    // First we define our subtitle window.  We formerly had a small
    // narrow subtitle window of a format like this:
    //
    Subtitle_Window . x = CHAT_SUBDIALOG_WINDOW_X; 
    Subtitle_Window . y = CHAT_SUBDIALOG_WINDOW_Y; 
    Subtitle_Window . w = CHAT_SUBDIALOG_WINDOW_W;
    Subtitle_Window . h = CHAT_SUBDIALOG_WINDOW_H;

    //--------------------
    // Now we need to clear this window, cause there might still be some
    // garbage from the previous subtitle in there...
    //
    // our_SDL_blit_surface_wrapper ( SubtitleBackground , &Subtitle_Window , Screen , &Subtitle_Window );
    
    // our_SDL_fill_rect_wrapper ( Screen , &Subtitle_Window , 0 );
    
    // our_SDL_fill_rect_wrapper (SDL_Surface *dst, SDL_Rect *dstrect, Uint32 color)
    
    blit_special_background ( CHAT_DIALOG_BACKGROUND_EXCERPT_CODE ) ;
    
    //--------------------
    // Now we can display the text and update the screen...
    //
    SDL_SetClipRect( Screen, NULL );
    DisplayText ( SubtitleText , Subtitle_Window.x , Subtitle_Window.y , &Subtitle_Window , TEXT_STRETCH );
    our_SDL_update_rect_wrapper ( Screen , Subtitle_Window.x , Subtitle_Window.y , Subtitle_Window.w , Subtitle_Window.h );
    
}; // void DisplaySubtitle( char* SubtitleText , void* SubtitleBackground )

/* ----------------------------------------------------------------------
 * During the Chat with a friendly droid or human, there is a window with
 * the full text transcript of the conversation so far.  This function is
 * here to display said text window and it's content, scrolled to the
 * position desired by the player himself.
 * ---------------------------------------------------------------------- */
void
display_current_chat_protocol ( int background_picture_code , enemy* ChatDroid , int with_update )
{
    SDL_Rect Subtitle_Window;
    int lines_needed ;
    int protocol_offset ;
    
#define AVERAGE_LINES_IN_PROTOCOL_WINDOW 9*GameConfig . screen_height/480

    if ( ! GameConfig . show_subtitles_in_dialogs ) return;
    
    SetCurrentFont( FPS_Display_BFont );
    
    //--------------------
    // First we define our subtitle window.  We formerly had a small
    // narrow subtitle window of a format like this:
    //
    Subtitle_Window . x = CHAT_SUBDIALOG_WINDOW_X; 
    Subtitle_Window . y = CHAT_SUBDIALOG_WINDOW_Y; 
    Subtitle_Window . w = CHAT_SUBDIALOG_WINDOW_W;
    Subtitle_Window . h = CHAT_SUBDIALOG_WINDOW_H;

    //--------------------
    // First we need to know where to begin with our little display.
    //
    lines_needed = GetNumberOfTextLinesNeeded ( chat_protocol , Subtitle_Window , TEXT_STRETCH );
    DebugPrintf ( 1 , "\nLines needed: %d. " , lines_needed );
    
    if ( lines_needed <= AVERAGE_LINES_IN_PROTOCOL_WINDOW ) 
    {
	//--------------------
	// When there isn't anything to scroll yet, we keep the default
	// position and also the users clicks on up/down button will be
	// reset immediately
	//
	protocol_offset = 0 ;
	chat_protocol_scroll_override_from_user = 0 ;
    }
    else
	protocol_offset = ( FontHeight ( GetCurrentFont() ) * TEXT_STRETCH ) 
	    * ( lines_needed - AVERAGE_LINES_IN_PROTOCOL_WINDOW + chat_protocol_scroll_override_from_user ) * 1.04 ;

    //--------------------
    // Now if the protocol offset is really negative, we don't really want
    // that and force the user offset back to something sane again.
    //
    if ( protocol_offset < 0 )
    {
	chat_protocol_scroll_override_from_user ++ ;
	protocol_offset = 0 ;
    }
    
    //--------------------
    // Now we need to clear this window, cause there might still be some
    // garbage from the previous subtitle in there...
    //
    // blit_special_background ( CHAT_DIALOG_BACKGROUND_EXCERPT_CODE ) ;
    PrepareMultipleChoiceDialog ( ChatDroid , FALSE );
    
    //--------------------
    // Now we can display the text and update the screen...
    //
    SDL_SetClipRect( Screen, NULL );
    Subtitle_Window . x = CHAT_SUBDIALOG_WINDOW_X; 
    Subtitle_Window . y = CHAT_SUBDIALOG_WINDOW_Y; 
    Subtitle_Window . w = CHAT_SUBDIALOG_WINDOW_W;
    Subtitle_Window . h = CHAT_SUBDIALOG_WINDOW_H;
    DisplayText ( chat_protocol , Subtitle_Window.x , Subtitle_Window.y - protocol_offset , &Subtitle_Window , TEXT_STRETCH );
    if ( protocol_offset > 0 ) 
	ShowGenericButtonFromList ( CHAT_PROTOCOL_SCROLL_UP_BUTTON );
    else
	ShowGenericButtonFromList ( CHAT_PROTOCOL_SCROLL_OFF_BUTTON );
    if ( lines_needed <= AVERAGE_LINES_IN_PROTOCOL_WINDOW ) 
	ShowGenericButtonFromList ( CHAT_PROTOCOL_SCROLL_OFF2_BUTTON );
    else
	ShowGenericButtonFromList ( CHAT_PROTOCOL_SCROLL_DOWN_BUTTON );
    if ( with_update ) our_SDL_update_rect_wrapper ( Screen , Subtitle_Window.x , Subtitle_Window.y , Subtitle_Window.w , Subtitle_Window.h );
    
}; // void display_current_chat_protocol ( int background_picture_code , int with_update )

/* ----------------------------------------------------------------------
 * This function should first display a subtitle and then also a sound
 * sample.  It is not very sophisticated or complicated, but nevertheless
 * important, because this combination does indeed occur so often.
 * ---------------------------------------------------------------------- */
void
GiveSubtitleNSample( char* SubtitleText , char* SampleFilename , enemy* ChatDroid , int with_update )
{

    strcat ( chat_protocol , SubtitleText );
    strcat ( chat_protocol , "\n" );
    
    if ( strcmp ( SubtitleText , "NO_SUBTITLE_AND_NO_WAITING_EITHER" ) )
    {
	// DisplaySubtitle ( SubtitleText , CHAT_DIALOG_BACKGROUND_PICTURE_CODE );
	display_current_chat_protocol ( CHAT_DIALOG_BACKGROUND_PICTURE_CODE , ChatDroid , with_update );
	PlayOnceNeededSoundSample( SampleFilename , TRUE , FALSE );
    }
    else
    {
	PlayOnceNeededSoundSample( SampleFilename , FALSE , FALSE );
    }
}; // void GiveSubtitleNSample( char* SubtitleText , char* SampleFilename )

/* ----------------------------------------------------------------------
 * Chat options may contain some extra commands, that specify things that
 * the engine is supposed to do, like open a shop interface, drop some
 * extra item to the inventory, remove an item from inventory, assign a
 * mission, mark a mission as solved and such things.
 *
 * This function is supposed to decode such extra commands and then to
 * execute the desired effect as well.
 *
 * ---------------------------------------------------------------------- */
void
ExecuteChatExtra ( char* ExtraCommandString , Enemy ChatDroid )
{
    int TempValue;
    char WorkString[5000];
    char *TempMessage;
    item NewItem;
    char tmp_filename [ 5000 ] ;
    char* fpath;
    int i;
    int mis_num , mis_diary_entry_num;
    
    if ( ! strcmp ( ExtraCommandString , "BreakOffAndBecomeHostile" ) )
    {
	ChatDroid -> is_friendly = FALSE ;
	ChatDroid -> combat_state = MAKE_ATTACK_RUN ;
    }
    else if ( ! strcmp ( ExtraCommandString , "EverybodyBecomesHostile" ) )
    {
	DebugPrintf ( -1000 , "\nEverybody SHOULD NOW BE HOSTILE!" );
	for ( i = 0 ; i < MAX_ENEMYS_ON_SHIP ; i ++ )
	{
	    AllEnemys [ i ] . is_friendly = FALSE ;
	    AllEnemys [ i ] . combat_state = MAKE_ATTACK_RUN ;
	}
    }
    else if ( ! strcmp ( ExtraCommandString , "SetCompletelyFixedProperty" ) )
    {
	DebugPrintf ( -1000 , "\nThis bot will now pull its breaks and not move any more." );
	ChatDroid -> CompletelyFixed = TRUE ;
    }
    else if ( ! strcmp ( ExtraCommandString , "UnsetCompletelyFixedProperty" ) )
    {
	DebugPrintf ( -1000 , "\nThis bot will now move again." );
	ChatDroid -> CompletelyFixed = FALSE ;
    }
    else if ( ! strcmp ( ExtraCommandString , "SetFollowTuxProperty" ) )
    {
	DebugPrintf ( -1000 , "\nThis bot will now follow Tux." );
	ChatDroid -> follow_tux = TRUE ;
	ChatDroid -> CompletelyFixed = FALSE ;
    }
    else if ( ! strcmp ( ExtraCommandString , "SetMoveFreelyProperty" ) )
    {
	DebugPrintf ( -1000 , "\nThis bot will now move freely." );
	ChatDroid -> CompletelyFixed = FALSE ;
	ChatDroid -> follow_tux = FALSE ;
    }
    else if ( ! strcmp ( ExtraCommandString , "MakeTuxTownGuardMember" ) )
    {
	DebugPrintf ( -1000 , "\nTux should now be a member of the old town's guard." );
	Me [ 0 ] . is_town_guard_member = TRUE ;
	Mission_Status_Change_Sound();
    }
    else if ( ! strcmp ( ExtraCommandString , "IncreaseMeleeWeaponSkill" ) )
    {
	Me [ 0 ] . melee_weapon_skill ++; 
	SetNewBigScreenMessage( "Melee fighting ability improved!" );
    }
    else if ( ! strcmp ( ExtraCommandString , "IncreaseRangedWeaponSkill" ) )
    {
	Me [ 0 ] . ranged_weapon_skill ++; 
	SetNewBigScreenMessage( "Ranged combat ability improved!" );
    }
    else if ( ! strcmp ( ExtraCommandString , "IncreaseSpellcastingSkill" ) )
    {
	Me [ 0 ] . spellcasting_skill ++; 
	SetNewBigScreenMessage( "Spellcasting ability improved!" );
    }
    else if ( ! strcmp ( ExtraCommandString , "IncreaseHackingSkill" ) )
    {
	Me [ 0 ] . hacking_skill ++; 
	SetNewBigScreenMessage( "Hacking ability improved!" );
    }
    else if ( ! strcmp ( ExtraCommandString , "IncreaseExtractPlasmaTransistorSkill" ) )
    {
	Me [ 0 ] . base_skill_level [ SPELL_EXTRACT_PLASMA_TRANSISTORS ] ++; 
	SetNewBigScreenMessage( "Extraction of plasma" );
	SetNewBigScreenMessage( "transistors learned!" );
    }
    else if ( ! strcmp ( ExtraCommandString , "IncreaseExtractSuperconductorSkill" ) )
    {
	Me [ 0 ] . base_skill_level [ SPELL_EXTRACT_SUPERCONDUCTORS ] ++; 
	SetNewBigScreenMessage( "Extraction of superconductors learned!" );
    }
    else if ( ! strcmp ( ExtraCommandString , "IncreaseExtractConverterSkill" ) )
    {
	Me [ 0 ] . base_skill_level [ SPELL_EXTRACT_ANTIMATTER_CONVERTERS ] ++; 
	SetNewBigScreenMessage( "Extraction of antimatter-matter" );
	SetNewBigScreenMessage( "converters learned!" );
    }
    else if ( ! strcmp ( ExtraCommandString , "IncreaseExtractInverterSkill" ) )
    {
	Me [ 0 ] . base_skill_level [ SPELL_EXTRACT_ENTROPY_INVERTERS ] ++; 
	SetNewBigScreenMessage( "Extraction of entropy inverters learned!" );
    }
    else if ( ! strcmp ( ExtraCommandString , "IncreaseExtractCondensatorSkill" ) )
    {
	Me [ 0 ] . base_skill_level [ SPELL_EXTRACT_TACHYON_CONDENSATORS ] ++; 
	SetNewBigScreenMessage( "Extraction of tachyon " );
	SetNewBigScreenMessage( "condensators learned!" );
    }
    else if ( CountStringOccurences ( ExtraCommandString , "GiveItem:" ) )
    {
	DebugPrintf( CHAT_DEBUG_LEVEL , "\nExtra invoked giving an item to the Tux. --> have to decode... " );
	ReadValueFromString( ExtraCommandString , "GiveItem:" , "%d" , 
			     &TempValue , ExtraCommandString + strlen ( ExtraCommandString ) + 0 );
	DebugPrintf( CHAT_DEBUG_LEVEL , "\n...decoding...item to give is: %d." , TempValue );
	NewItem.type = TempValue  ;
	NewItem.prefix_code = (-1);
	NewItem.suffix_code = (-1);
	FillInItemProperties ( &NewItem , TRUE , 0 );
	//--------------------
	// Either we put the new item directly into inventory or we issue a warning
	// that there is no room and then drop the item to the floor directly under 
	// the current Tux position.  That can't fail, right?
	//
	if ( !TryToIntegrateItemIntoInventory ( & NewItem , 1 ) )
	{
	    DropItemToTheFloor ( &NewItem , Me [ 0 ] . pos . x , Me [ 0 ] . pos . y , Me [ 0 ] . pos. z ) ;
	    SetNewBigScreenMessage( "1 Item received (on floor)" );
	}
	else
	{
	    SetNewBigScreenMessage( "1 Item received!" );
	}
    }
    else if ( CountStringOccurences ( ExtraCommandString , "OpenQuestDiaryEntry:" ) )
    {
	DebugPrintf( -4 , "\nExtra invoked enabling a new quest diary entry: %s." ,
		     ExtraCommandString + strlen ( "OpenQuestDiaryEntry:" ) );
	strncpy ( WorkString , ExtraCommandString + strlen ( "OpenQuestDiaryEntry:" ) , 10 );
	WorkString [ 10 ] = 0 ;
	sscanf ( WorkString , "M%dE%d:" , &mis_num , &mis_diary_entry_num );
	DebugPrintf ( -4 , "\nreceived mission number: %d and diary entry number: %d." , 
		      mis_num , mis_diary_entry_num );
	quest_browser_enable_new_diary_entry ( mis_num , mis_diary_entry_num , 0 );
    }
    else if ( CountStringOccurences ( ExtraCommandString , "ExecuteActionWithLabel:" ) )
    {
	DebugPrintf( CHAT_DEBUG_LEVEL , "\nExtra invoked execution of action with label: %s. Doing it... " ,
		     ExtraCommandString + strlen ( "ExecuteActionWithLabel:" ) );
	ExecuteActionWithLabel ( ExtraCommandString + strlen ( "ExecuteActionWithLabel:" ) , 0 ) ;
    }
    else if ( CountStringOccurences ( ExtraCommandString , "ExecuteSubdialog:" ) )
    {
	strcpy ( tmp_filename , ExtraCommandString + strlen ( "ExecuteSubdialog:" ) ) ;
	DebugPrintf( 0 , "\nExtra invoked start of SUBDIALOG! with label: %s. Doing it... " ,
		     tmp_filename );
	
	push_or_pop_chat_roster ( PUSH_ROSTER );
	
	//--------------------
	// We have to load the subdialog specified...
	//
	// InitChatRosterForNewDialogue(  );
	strcat ( tmp_filename , ".dialog" );
	fpath = find_file ( tmp_filename , DIALOG_DIR, FALSE);
	
	for ( i = 0 ; i < MAX_DIALOGUE_OPTIONS_IN_ROSTER ; i ++ )
	{
	    delete_one_dialog_option ( i , TRUE ); // this must be done so, that no freeing is done...
	}
	
	for ( i = 0 ; i < MAX_ANSWERS_PER_PERSON ; i ++ )
	{
	    Me [ 0 ] . Chat_Flags [ PERSON_SUBDIALOG_DUMMY ] [ i ] = 0 ;
	}
	
	LoadChatRosterWithChatSequence ( fpath );
	// ChatDroid -> will_rush_tux = TRUE ;
	ChatDroid -> combat_state = RUSH_TUX_ON_SIGHT_AND_OPEN_TALK ;
	DoChatFromChatRosterData( 0 , PERSON_SUBDIALOG_DUMMY , ChatDroid , FALSE );
	
	push_or_pop_chat_roster ( POP_ROSTER );
	
    }
    else if ( CountStringOccurences ( ExtraCommandString , "PlantCookie:" ) )
    {
	DebugPrintf( CHAT_DEBUG_LEVEL , "\nExtra invoked planting of a cookie: %s. Doing it... " ,
		     ExtraCommandString + strlen ( "PlantCookie:" ) );
	PlantCookie ( ExtraCommandString + strlen ( "PlantCookie:" ) , 0 ) ;
    }
    else if ( CountStringOccurences ( ExtraCommandString , "InitTradeWithCharacter:" ) )
    {
	TempValue = ResolveDialogSectionToChatFlagsIndex ( ExtraCommandString + strlen ( "InitTradeWithCharacter:" ) ) ;
	InitTradeWithCharacter( TempValue );
    }
    else if ( CountStringOccurences ( ExtraCommandString , "AssignMission:" ) )
    {
	DebugPrintf( CHAT_DEBUG_LEVEL , "\nExtra invoked assigning of mission. --> have to decode... " );
	ReadValueFromString( ExtraCommandString , "AssignMission:" , "%d" , 
			     &TempValue , ExtraCommandString + strlen ( ExtraCommandString ) + 0 );
	DebugPrintf( CHAT_DEBUG_LEVEL , "\n...decoding...Mission to assign is: %d." , TempValue );
	AssignMission ( TempValue );
    }
    else if ( CountStringOccurences ( ExtraCommandString , "MarkMissionComplete:" ) )
    {
	DebugPrintf( CHAT_DEBUG_LEVEL , "\nExtra invoked marking a mission as completed. --> have to decode... " );
	ReadValueFromString( ExtraCommandString , "MarkMissionComplete:" , "%d" , 
			     &TempValue , ExtraCommandString + strlen ( ExtraCommandString ) + 0 );
	DebugPrintf( CHAT_DEBUG_LEVEL , "\n...decoding...Mission to mark as complete is: %d." , TempValue );
	Me [ 0 ] . AllMissions[ TempValue ] . MissionIsComplete = TRUE;
    }
    else if ( CountStringOccurences ( ExtraCommandString , "AddExperienceBonus:" ) )
    {
	DebugPrintf( CHAT_DEBUG_LEVEL , "\nExtra invoked adding an exerpience bonus. --> have to decode... " );
	ReadValueFromString( ExtraCommandString , "AddExperienceBonus:" , "%d" , 
			     &TempValue , ExtraCommandString + strlen ( ExtraCommandString ) + 0 );
	DebugPrintf( CHAT_DEBUG_LEVEL , "\n...decoding...bonus to add is: %d." , TempValue );
	Me [ 0 ] . Experience += TempValue;
	sprintf( WorkString , "+%d Experience Points" , TempValue );
	SetNewBigScreenMessage ( WorkString );
    }
    else if ( CountStringOccurences ( ExtraCommandString , "AddBigScreenMessageBUT_WITH_TERMINATION_CHARACTER_PLEASE:" ) )
    {
	DebugPrintf( CHAT_DEBUG_LEVEL , "\nExtra invoked adding a big screen message. --> have to decode... " );
	TempMessage = ReadAndMallocStringFromData ( ExtraCommandString , "AddBigScreenMessageBUT_WITH_TERMINATION_CHARACTER_PLEASE:" , ":" ) ;
	DebugPrintf( CHAT_DEBUG_LEVEL , "\n...decoding...message is: %s." , TempMessage );
	SetNewBigScreenMessage ( TempMessage );
    }
    else if ( CountStringOccurences ( ExtraCommandString , "AddBaseMagic:" ) )
    {
	DebugPrintf( CHAT_DEBUG_LEVEL , "\nExtra invoked adding some base magic points. --> have to decode... " );
	ReadValueFromString( ExtraCommandString , "AddBaseMagic:" , "%d" , 
			     &TempValue , ExtraCommandString + strlen ( ExtraCommandString ) + 0 );
	DebugPrintf( CHAT_DEBUG_LEVEL , "\n...decoding... amount of magic points mentioned is: %d." , TempValue );
	Me [ 0 ] . base_magic += TempValue;
    }
    else if ( CountStringOccurences ( ExtraCommandString , "AddBaseDexterity:" ) )
    {
	DebugPrintf( CHAT_DEBUG_LEVEL , "\nExtra invoked adding some base dexteritypoints. --> have to decode... " );
	ReadValueFromString( ExtraCommandString , "AddBaseDexterity:" , "%d" , 
			     &TempValue , ExtraCommandString + strlen ( ExtraCommandString ) + 0 );
	DebugPrintf( CHAT_DEBUG_LEVEL , "\n...decoding... amount of dexterity points mentioned is: %d." , TempValue );
	Me [ 0 ] . base_dexterity += TempValue;
    }
    else if ( CountStringOccurences ( ExtraCommandString , "SubtractPointsToDistribute:" ) )
    {
	DebugPrintf( CHAT_DEBUG_LEVEL , "\nExtra invoked subtracting points to distribute. --> have to decode... " );
	ReadValueFromString( ExtraCommandString , "SubtractPointsToDistribute:" , "%d" , 
			     &TempValue , ExtraCommandString + strlen ( ExtraCommandString ) + 0 );
	DebugPrintf( CHAT_DEBUG_LEVEL , "\n...decoding... amount of points mentioned is: %d." , TempValue );
	Me [ 0 ] . points_to_distribute -= TempValue;
    }
    else if ( CountStringOccurences ( ExtraCommandString , "SubtractGold:" ) )
    {
	DebugPrintf( CHAT_DEBUG_LEVEL , "\nExtra invoked subtracting gold. --> have to decode... " );
	ReadValueFromString( ExtraCommandString , "SubtractGold:" , "%d" , 
			     &TempValue , ExtraCommandString + strlen ( ExtraCommandString ) + 0 );
	DebugPrintf( CHAT_DEBUG_LEVEL , "\n...decoding... amount of gold mentioned is: %d." , TempValue );
	Me [ 0 ] . Gold -= TempValue;
	sprintf ( WorkString , "%d bucks given" , TempValue );
	SetNewBigScreenMessage ( WorkString );
    }
    else if ( CountStringOccurences ( ExtraCommandString , "ForceBotRespawnOnLevel:" ) )
    {
	DebugPrintf( CHAT_DEBUG_LEVEL , "\nExtra invoked forcing bot respawn on level --> have to decode... " );
	ReadValueFromString( ExtraCommandString , "ForceBotRespawnOnLevel:" , "%d" , 
			     &TempValue , ExtraCommandString + strlen ( ExtraCommandString ) + 0 );
	DebugPrintf( CHAT_DEBUG_LEVEL , "\n...decoding...Level to respawn bots on is: %d." , TempValue );
	respawn_level ( TempValue );
    }
    else if ( CountStringOccurences ( ExtraCommandString , "AddGold:" ) )
    {
	DebugPrintf( CHAT_DEBUG_LEVEL , "\nExtra invoked adding gold. --> have to decode... " );
	ReadValueFromString( ExtraCommandString , "AddGold:" , "%d" , 
			     &TempValue , ExtraCommandString + strlen ( ExtraCommandString ) + 0 );
	DebugPrintf( CHAT_DEBUG_LEVEL , "\n...decoding... amount of gold mentioned is: %d." , TempValue );
	Me [ 0 ] . Gold += TempValue;
	sprintf ( WorkString , "%d bucks received" , TempValue );
	SetNewBigScreenMessage ( WorkString );
    }
    else if ( CountStringOccurences ( ExtraCommandString , "DeleteAllInventoryItemsWithCode:" ) )
    {
	DebugPrintf( CHAT_DEBUG_LEVEL , "\nExtra invoked deletion of all inventory items with a certain code. --> have to decode... " );
	ReadValueFromString( ExtraCommandString , "DeleteAllInventoryItemsWithCode:" , "%d" , 
			     &TempValue , ExtraCommandString + strlen ( ExtraCommandString ) + 0 );
	DebugPrintf( CHAT_DEBUG_LEVEL , "\n...decoding... code of item to delete found is: %d." , TempValue );
	DeleteAllInventoryItemsOfType( TempValue , 0 );      
    }
    else if ( CountStringOccurences ( ExtraCommandString , "DeleteAllInventoryItemsOfType:" ) )
    {
	DebugPrintf( CHAT_DEBUG_LEVEL , "\nExtra invoked deletion of all inventory items of type '%s'. --> have to decode... " ,
		     ExtraCommandString + strlen ( "DeleteAllInventoryItemsOfType:" ) );
	
	if ( !strcmp ( ExtraCommandString + strlen ( "DeleteAllInventoryItemsOfType:" ) , "ITEM_DIXONS_TOOLBOX" ) )
	{
	    TempValue = ITEM_DIXONS_TOOLBOX;
	}
	else if ( !strcmp ( ExtraCommandString + strlen ( "DeleteAllInventoryItemsOfType:" ) , "ITEM_RED_DILITIUM_CRYSTAL" ) )
	{
	    TempValue = ITEM_RED_DILITIUM_CRYSTAL;
	}
	else
	{
	    fprintf( stderr, "\n\nErrorneous string: %s \n" , 
		     ExtraCommandString + strlen ( "DeleteAllInventoryItemsOfType:" ) );
	    GiveStandardErrorMessage ( __FUNCTION__  , "\
ERROR:  UNKNOWN ITEM STRING GIVEN AS ITEM TO DELETE FROM INVENTORY!",
				       PLEASE_INFORM, IS_FATAL );
	}
	
	DebugPrintf( CHAT_DEBUG_LEVEL , "\n...decoding...item to remove is: %d." , TempValue );
	DeleteAllInventoryItemsOfType( TempValue , 0 );      
	
    }
    else if ( ! strcmp ( ExtraCommandString , "CompletelyHealTux" ) )
    {
	Me [ 0 ] . energy = Me [ 0 ] . maxenergy ;
    }
    else 
    {
	fprintf( stderr, "\n\nExtraCommandString: %s \n" , ExtraCommandString );
	GiveStandardErrorMessage ( __FUNCTION__  , "\
ERROR:  UNKNOWN COMMAND STRING GIVEN!",
				   PLEASE_INFORM, IS_FATAL );
    }
}; // void ExecuteChatExtra ( char* ExtraCommandString )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
make_sure_chat_portraits_loaded_for_this_droid ( Enemy this_droid )
{
    SDL_Surface* Small_Droid;
    SDL_Surface* Large_Droid;
    char *fpath;
    char fname[500];
    int i;
    int model_number;
    static int first_call = TRUE ;
    static int this_type_has_been_loaded [ ENEMY_ROTATION_MODELS_AVAILABLE ] ;
    
    //--------------------
    // We make sure we only load the portrait files once and not
    // every time...
    //
    if ( first_call )
    {
	for ( i = 0 ; i < ENEMY_ROTATION_MODELS_AVAILABLE ; i ++ )
	    this_type_has_been_loaded [ i ] = FALSE ;
    }
    first_call = FALSE ;
    
    //--------------------
    // We look up the model number for this chat partner.
    //
    model_number = this_droid -> type ;
    
    //--------------------
    // We should make sure, that we don't double-load images that we have loaded
    // already, thereby wasting more resources, including OpenGL texture positions.
    //
    if ( this_type_has_been_loaded [ model_number ] )
	return;
    this_type_has_been_loaded [ model_number ] = TRUE ;
    
    //--------------------
    // At first we try to load the image, that is named after this
    // chat section.  If that succeeds, perfect.  If not, we'll revert
    // to a default image.
    //
    strcpy( fname, "droids/" );
    strcat( fname, PrefixToFilename [ model_number ] ) ;
    strcat( fname , "/portrait.png" );
    fpath = find_file (fname, GRAPHICS_DIR, FALSE);
    DebugPrintf ( 2 , "\nFilename used for portrait: %s." , fpath );
    
    Small_Droid = our_IMG_load_wrapper (fpath) ;
    if ( Small_Droid == NULL )
    {
	strcpy( fname, "droids/" );
	strcat( fname, "DefaultPortrait.png" );
	fpath = find_file (fname, GRAPHICS_DIR, FALSE);
	Small_Droid = our_IMG_load_wrapper ( fpath ) ;
    }
    if ( Small_Droid == NULL )
    {
	fprintf( stderr, "\n\nfpath: %s \n" , fpath );
	GiveStandardErrorMessage ( __FUNCTION__  , "\
It wanted to load a small portrait file in order to display it in the \n\
chat interface of Freedroid.  But:  Loading this file has ALSO failed.",
				   PLEASE_INFORM, IS_FATAL );
    }
    
    Large_Droid = zoomSurface( Small_Droid , (float) Droid_Image_Window . w / (float) Small_Droid -> w , 
			       (float) Droid_Image_Window . w / (float) Small_Droid -> w , 0 );
    
    chat_portrait_of_droid [ model_number ] . surface = Large_Droid ;
    
    if ( use_open_gl )
	swap_red_and_blue_for_open_gl ( chat_portrait_of_droid [ model_number ] . surface );
    
    SDL_FreeSurface( Small_Droid );
    
}; // void make_sure_chat_portraits_loaded_for_this_droid ( Enemy this_droid )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
make_sure_all_chat_portraits_are_loaded ( void )
{
    SDL_Surface* Small_Droid;
    SDL_Surface* Large_Droid;
    char *fpath;
    char fname[500];
    int i;
    static int first_call = TRUE ;
    
    //--------------------
    // We make sure we only load the portrait files once and not
    // every time...
    //
    if ( ! first_call ) return ;
    first_call = FALSE ;
    
    //--------------------
    // All of the portrait files will now be loaded one after the
    // other...
    //
    for ( i = 0 ; i < ENEMY_ROTATION_MODELS_AVAILABLE ; i ++ )
    {
	//--------------------
	// At first we try to load the image, that is named after this
	// chat section.  If that succeeds, perfect.  If not, we'll revert
	// to a default image.
	//
	strcpy( fname, "droids/" );
	strcat( fname, PrefixToFilename [ i ] ) ;
	strcat( fname , "/portrait.png" );
	fpath = find_file (fname, GRAPHICS_DIR, FALSE);
	DebugPrintf ( -1000, "\nFilename used for portrait: %s." , fpath );
	
	Small_Droid = our_IMG_load_wrapper (fpath) ;
	if ( Small_Droid == NULL )
	{
	    strcpy( fname, "droids/" );
	    strcat( fname, "DefaultPortrait.png" );
	    fpath = find_file (fname, GRAPHICS_DIR, FALSE);
	    Small_Droid = our_IMG_load_wrapper (fpath) ;
	}
	if ( Small_Droid == NULL )
	{
	    fprintf( stderr, "\n\nfpath: %s \n" , fpath );
	    GiveStandardErrorMessage ( __FUNCTION__  , "\
It wanted to load a small portrait file in order to display it in the \n\
chat interface of Freedroid.  But:  Loading this file has ALSO failed.",
				       PLEASE_INFORM, IS_FATAL );
	}
	// Large_Droid = zoomSurface( Small_Droid , 1.8 , 1.8 , 0 );
	
	Large_Droid = zoomSurface( Small_Droid , (float)Droid_Image_Window.w / (float)Small_Droid->w , (float)Droid_Image_Window.w / (float)Small_Droid->w , 0 );
	
	chat_portrait_of_droid [ i ] . surface = Large_Droid ;
	
	if ( use_open_gl )
	    swap_red_and_blue_for_open_gl ( chat_portrait_of_droid [ i ] . surface );
	
	SDL_FreeSurface( Small_Droid );
	
    }

}; // void make_sure_all_chat_portraits_are_loaded ( void )

/* ----------------------------------------------------------------------
 * This function prepares the chat background window and displays the
 * image of the dialog partner and also sets the right font.
 * ---------------------------------------------------------------------- */
void
PrepareMultipleChoiceDialog ( Enemy ChatDroid , int with_flip )
{
    //--------------------
    // The dialog will always take more than a few seconds to process
    // so we need to prevent framerate distortion...
    //
    Activate_Conservative_Frame_Computation( );
    
    //--------------------
    // We make sure that all the chat portraits we might need are
    // loaded....
    //
    make_sure_chat_portraits_loaded_for_this_droid ( ChatDroid ) ;
    //make_sure_all_chat_portraits_are_loaded ( );
    
    //--------------------
    // We select small font for the menu interaction...
    //
    SetCurrentFont( FPS_Display_BFont );
    
    AssembleCombatPicture ( USE_OWN_MOUSE_CURSOR ) ;
    blit_special_background ( CHAT_DIALOG_BACKGROUND_PICTURE_CODE );
    our_SDL_blit_surface_wrapper ( chat_portrait_of_droid [ ChatDroid -> type ] . surface , NULL , 
				   Screen , &Droid_Image_Window );
    
    if ( with_flip ) 
	our_SDL_flip_wrapper( Screen );
    
#if __WIN32__
    //--------------------
    // When using win32, after flipping we may have a damaged (or rather out of date)
    // screen in the other buffer or something... dunno... well, we just paint the
    // same background a second time.  That should resolve the problem in all cases.
    //
    blit_special_background ( CHAT_DIALOG_BACKGROUND_PICTURE_CODE );
    our_SDL_blit_surface_wrapper ( chat_portrait_of_droid [ ChatDroid -> type ] . surface , NULL , 
				   Screen , &Droid_Image_Window );
    if ( with_flip ) our_SDL_flip_wrapper( Screen );
#endif
    
}; // void PrepareMultipleChoiceDialog ( int Enum )

/* ----------------------------------------------------------------------
 * It is possible to specify a conditional goto command from the chat
 * information file 'Freedroid.dialogues'.  But in order to execute this
 * conditional jump, we need to know whether a statment given as pure text
 * string is true or not.  This function is intended to find out whether
 * it is true or not.
 * ---------------------------------------------------------------------- */
int
TextConditionIsTrue ( char* ConditionString )
{
    int TempValue;
    char* CookieText;
    int i ;
    int old_town_mission_score;
    
    if ( CountStringOccurences ( ConditionString , "MissionComplete" ) )
    {
	DebugPrintf ( CHAT_DEBUG_LEVEL , "\nCondition String identified as question for mission complete." );
	ReadValueFromString( ConditionString , ":", "%d" , 
			     &TempValue , ConditionString + strlen ( ConditionString ) );
	DebugPrintf ( CHAT_DEBUG_LEVEL , "\nCondition String referred to mission number: %d." , TempValue );
	
	if ( Me [ 0 ] . AllMissions [ TempValue ] . MissionIsComplete )
	    return ( TRUE );
	else
	    return ( FALSE );
    }
    else if ( CountStringOccurences ( ConditionString , "MissionAssigned" ) )
    {
	DebugPrintf ( CHAT_DEBUG_LEVEL , "\nCondition String identified as question for mission assigned." );
	ReadValueFromString( ConditionString , ":", "%d" , 
			     &TempValue , ConditionString + strlen ( ConditionString ) );
	DebugPrintf ( CHAT_DEBUG_LEVEL , "\nCondition String referred to mission number: %d." , TempValue );
	
	if ( Me [ 0 ] . AllMissions [ TempValue ] . MissionWasAssigned )
	    return ( TRUE );
	else
	    return ( FALSE );
    }
    else if ( CountStringOccurences ( ConditionString , "HaveItemWithCode" ) )
    {
	DebugPrintf ( CHAT_DEBUG_LEVEL , "\nCondition String identified as question for have item in inventory." );
	ReadValueFromString( ConditionString , ":", "%d" , 
			     &TempValue , ConditionString + strlen ( ConditionString ) );
	DebugPrintf ( CHAT_DEBUG_LEVEL , "\nCondition String referred to item code: %d." , TempValue );
	
	if ( CountItemtypeInInventory ( TempValue , 0 ) )
	    return ( TRUE );
	else
	    return ( FALSE );
    }
    else if ( CountStringOccurences ( ConditionString , "PointsToDistributeAtLeast" ) )
    {
	DebugPrintf ( CHAT_DEBUG_LEVEL , "\nCondition String identified as question for available skill points to distribute." );
	ReadValueFromString( ConditionString , ":", "%d" , 
			     &TempValue , ConditionString + strlen ( ConditionString ) );
	DebugPrintf ( CHAT_DEBUG_LEVEL , "\nCondition String mentioned number of points: %d." , TempValue );
	
	if ( Me [ 0 ] . points_to_distribute >= TempValue )
	    return ( TRUE );
	else
	    return ( FALSE );
    }
    else if ( CountStringOccurences ( ConditionString , "GoldIsLessThan" ) )
    {
	DebugPrintf ( CHAT_DEBUG_LEVEL , "\nCondition String identified as question for amount of gold Tux has on him." );
	ReadValueFromString( ConditionString , ":", "%d" , 
			     &TempValue , ConditionString + strlen ( ConditionString ) );
	DebugPrintf ( CHAT_DEBUG_LEVEL , "\nCondition String mentioned concrete amout of gold: %d." , TempValue );
	
	if ( Me [ 0 ] . Gold < TempValue )
	    return ( TRUE );
	else
	    return ( FALSE );
    }
    else if ( CountStringOccurences ( ConditionString , "MeleeSkillLesserThan" ) )
    {
	DebugPrintf ( CHAT_DEBUG_LEVEL , "\nCondition String identified as question for melee skill lesser than value." );
	ReadValueFromString( ConditionString , ":", "%d" , 
			     &TempValue , ConditionString + strlen ( ConditionString ) );
	DebugPrintf ( CHAT_DEBUG_LEVEL , "\nCondition String mentioned level: %d." , TempValue );
	
	if ( Me [ 0 ] . melee_weapon_skill < TempValue )
	    return ( TRUE );
	else
	    return ( FALSE );
    }
    else if ( CountStringOccurences ( ConditionString , "CookieIsPlanted" ) )
    {
	DebugPrintf ( -4 , "\nCondition String identified as question for cookie planted." );
	
	CookieText = 
	    ReadAndMallocStringFromData ( ConditionString , "CookieIsPlanted:" , ":" ) ;
	DebugPrintf ( -4 , "\nCookieText mentioned: '%s'." , CookieText );
	
	for ( i = 0 ; i < MAX_COOKIES ; i ++ )
	{
	    DebugPrintf ( 1 , "\nCookie entry to compare to: %s." , Me [ 0 ] . cookie_list [ i ] );
	    if ( ! strlen ( Me [ 0 ] . cookie_list [ i ] ) ) continue;
	    if ( ! strcmp ( Me [ 0 ] . cookie_list [ i ] , CookieText ) ) 
		return ( TRUE );
	    //--------------------
	    // Now some extra safety, cause the ':' termination character might still be on 
	    // the cookie or on the comparison string
	    //
	    if ( strcmp ( Me [ 0 ] . cookie_list [ i ] , CookieText ) >= ( ( int ) strlen ( CookieText ) ) ) 
		return ( TRUE ); 
	}
	
	free ( CookieText );
	
	return ( FALSE );
	
    }
    else if ( CountStringOccurences ( ConditionString , "OldTownMissionScoreAtLeast" ) )
    {
	DebugPrintf ( CHAT_DEBUG_LEVEL , "\nCondition String identified as question for old town mission score." );
	ReadValueFromString( ConditionString , ":", "%d" , 
			     &TempValue , ConditionString + strlen ( ConditionString ) );
	DebugPrintf ( CHAT_DEBUG_LEVEL , "\nCondition String mentioned mission score of : %d old town mission points." , 
		      TempValue );
	
	old_town_mission_score = 0 ;
	if ( Me [ 0 ] . AllMissions [ 0 ] . MissionIsComplete )
	    old_town_mission_score += 10 ;
	if ( Me [ 0 ] . AllMissions [ 1 ] . MissionIsComplete )
	    old_town_mission_score += 15 ;
	if ( Me [ 0 ] . AllMissions [ 2 ] . MissionIsComplete )
	    old_town_mission_score += 10 ;
	if ( Me [ 0 ] . AllMissions [ 3 ] . MissionIsComplete )
	    old_town_mission_score += 10 ;
	if ( Me [ 0 ] . AllMissions [ 4 ] . MissionIsComplete )
	    old_town_mission_score += 20 ;
	if ( Me [ 0 ] . AllMissions [ 5 ] . MissionIsComplete )
	    old_town_mission_score += 15 ;
	
	if ( old_town_mission_score >= TempValue )
	    return ( TRUE );
	else
	    return ( FALSE );
    }
    
    fprintf( stderr, "\n\nConditionString: %s. \n" , ConditionString );
    GiveStandardErrorMessage ( __FUNCTION__  , "\
There were was a Condition string (most likely used for an on-goto-command\n\
in the Freedroid.dialogues file, that contained a seemingly bogus condition.\n\
Freedroid was unable to determine the type of said condition.",
			       PLEASE_INFORM, IS_FATAL );
    
    return ( TRUE );
}; // int TextConditionIsTrue ( char* ConditionString )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
ProcessThisChatOption ( int MenuSelection , int PlayerNum , int ChatPartnerCode , Enemy ChatDroid )
{
    int i;

    //--------------------
    // Now a menu section has been made.  We do the reaction:
    // say the samples and the replies, later we'll set the new option values
    //
    // But it might be the case that this option is more technical and not accompanied
    // by any reply.  This case must also be caught.
    //
    if ( strcmp ( ChatRoster [ MenuSelection ] . option_sample_file_name , "NO_SAMPLE_HERE_AND_DONT_WAIT_EITHER" ) )
    {
	// PlayOnceNeededSoundSample( ChatRoster [ MenuSelection ] . option_sample_file_name , TRUE );
	strcat ( chat_protocol , "\1TUX:" );
	GiveSubtitleNSample ( ChatRoster [ MenuSelection ] . option_text ,
			      ChatRoster [ MenuSelection ] . option_sample_file_name , ChatDroid , TRUE ) ;
	strcat ( chat_protocol , "\2" );
    }
    
    //--------------------
    // Now we can proceed to execute
    // the rest of the reply that has been set up for this (the now maybe modified)
    // dialog option.
    //
    for ( i = 0 ; i < MAX_REPLIES_PER_OPTION ; i ++ )
    {
	//--------------------
	// Once we encounter an empty string here, we're done with the reply...
	//
	if ( ! strlen ( ChatRoster [ MenuSelection ] . reply_subtitle_list [ i ] ) ) 
	    break;
	
	GiveSubtitleNSample ( ChatRoster [ MenuSelection ] . reply_subtitle_list [ i ] ,
			      ChatRoster [ MenuSelection ] . reply_sample_list [ i ] , ChatDroid , TRUE ) ;
    }
    
    //--------------------
    // Now that all the replies have been made, we can start on changing
    // the option flags to their new values
    //
    for ( i = 0 ; i < MAX_ANSWERS_PER_PERSON ; i ++ )
    {
	//--------------------
	// Maybe all nescessary changes were made by now.  Then it's time
	// to quit...
	//
	if ( ChatRoster [ MenuSelection ] . change_option_nr [ i ] == (-1) ) 
	    break;
	
	Me [ PlayerNum ] . Chat_Flags [ ChatPartnerCode ] [ ChatRoster [ MenuSelection ] . change_option_nr [ i ] ] =
	    ChatRoster [ MenuSelection ] . change_option_to_value [ i ]  ;
	DebugPrintf ( CHAT_DEBUG_LEVEL , "\nChanged chat flag nr. %d to new value %d." ,
		      ChatRoster [ MenuSelection ] . change_option_nr[i] ,
		      ChatRoster [ MenuSelection ] . change_option_to_value[i] );
    }
    
    //--------------------
    // Maybe this option should also invoke some extra function like opening
    // a shop interface or something.  So we do this here.
    //
    for ( i = 0 ; i < MAX_EXTRAS_PER_OPTION ; i ++ )
    {
	//--------------------
	// Maybe all nescessary extras were executed by now.  Then it's time
	// to quit...
	//
	if ( !strlen ( ChatRoster [ MenuSelection ] . extra_list [ i ] ) )
	    break;
	
	DebugPrintf ( CHAT_DEBUG_LEVEL , "\nWARNING!  Starting to invoke extra.  Text is: %s." ,
		      ChatRoster [ MenuSelection ] . extra_list[i] );
	
	ExecuteChatExtra ( ChatRoster [ MenuSelection ] . extra_list[i] , ChatDroid );
	
	//--------------------
	// Maybe the chat extra has annoyed the chat partner and he is now
	// suddenly hostile and breaks off the chat.  This is handled here.
	//
	if ( ! ChatDroid -> is_friendly ) return ;
	
	//--------------------
	// It can't hurt to have the overall background redrawn after each extra command
	// which could have destroyed the background by drawing e.g. a shop interface
	PrepareMultipleChoiceDialog ( ChatDroid , FALSE ) ;
    }
    
    //--------------------
    // Maybe there was an ON-GOTO-CONDITION specified for this option.
    // Then of course we have to jump to the new location!!!
    //
    if ( strlen ( ChatRoster [ MenuSelection ] . on_goto_condition ) )
    {
	DebugPrintf( CHAT_DEBUG_LEVEL , "\nON-GOTO-CONDITION ENCOUNTERED... CHECKING... " );
	if ( TextConditionIsTrue ( ChatRoster [ MenuSelection ] . on_goto_condition ) )
	{
	    DebugPrintf( CHAT_DEBUG_LEVEL , "...SEEMS TRUE... CONTINUING AT OPTION: %d. " , 
			 ChatRoster [ MenuSelection ] . on_goto_first_target );
	    MenuSelection = ChatRoster [ MenuSelection ] . on_goto_first_target ;
	}
	else
	{
	    DebugPrintf( CHAT_DEBUG_LEVEL , "...SEEMS FALSE... CONTINUING AT OPTION: %d. " , 
			 ChatRoster [ MenuSelection ] . on_goto_second_target );
	    MenuSelection = ChatRoster [ MenuSelection ] . on_goto_second_target ;
	}
	ProcessThisChatOption ( MenuSelection , PlayerNum , ChatPartnerCode , ChatDroid );
    }
    
}; // void ProcessThisChatOption ( int MenuSelection , int PlayerNum , int ChatPartnerCode , Enemy ChatDroid )


/* ----------------------------------------------------------------------
 * This is the most important subfunction of the whole chat with friendly
 * droids and characters.  After the pure chat data has been loaded from
 * disk, this function is invoked to handle the actual chat interaction
 * and the dialog flow.
 * ---------------------------------------------------------------------- */
void
DoChatFromChatRosterData( int PlayerNum , int ChatPartnerCode , Enemy ChatDroid , int clear_protocol )
{
    int i ;
    SDL_Rect Chat_Window;
    int MenuSelection = (-1) ;
    char* DialogMenuTexts[ MAX_ANSWERS_PER_PERSON ];
    
    //--------------------
    // We always should clear the chat protocol.  Only for SUBDIALOGS it is
    // suitable not to clear the chat protocol.
    //
    if ( clear_protocol )
    {
	if ( chat_protocol != NULL ) free ( chat_protocol );
	chat_protocol = MyMalloc ( 500000 ); // enough for any chat...
	strcpy ( chat_protocol , "\2--- Start of Dialog ---\n" );
	chat_protocol_scroll_override_from_user = 0 ;
	SetCurrentFont ( FPS_Display_BFont );
    }
    
    PrepareMultipleChoiceDialog ( ChatDroid , TRUE );
    
    Chat_Window . x = 242 ; Chat_Window . y = 100 ; Chat_Window . w = 380; Chat_Window . h = 314 ;
    
    //--------------------
    // We load the option texts into the dialog options variable..
    //
    for ( i = 0 ; i < MAX_ANSWERS_PER_PERSON ; i ++ )
    {
	if ( strlen ( ChatRoster [ i ] . option_text ) )
	{
	    DialogMenuTexts [ i ] = ChatRoster [ i ] . option_text ;
	}
    }
    // DialogMenuTexts [ MAX_ANSWERS_PER_PERSON - 1 ] = " END ";

    //--------------------
    // Now we execute all the options that were marked to be executed
    // prior to dialog startup
    //
    for ( i = 0 ; i < MAX_ANSWERS_PER_PERSON ; i ++ )
    {
	if ( ChatRoster [ i ] . always_execute_this_option_prior_to_dialog_start )
	{
	    DebugPrintf ( 0 , "\nExecuting option no. %d prior to dialog start.\n" , i );
	    ProcessThisChatOption ( i , PlayerNum , ChatPartnerCode , ChatDroid );
	}
    }
    
    while (1)
    {
	//--------------------
	// Now maybe this is one of the bots that is rushing the Tux!  Then of course
	// we won't do the first selection, but instead immediately call the very first
	// dialog option and then continue with normal dialog.
	//
	if ( ChatDroid -> combat_state == RUSH_TUX_ON_SIGHT_AND_OPEN_TALK )
	{
	    MenuSelection = 0 ;
	    ChatDroid -> combat_state = TURN_THOWARDS_NEXT_WAYPOINT ;
	    ChatDroid -> persuing_given_course = FALSE ;
	    ChatDroid -> has_greeted_influencer = TRUE ;
	}
	else
	{
	    MenuSelection = ChatDoMenuSelectionFlagged ( "What will you say?" , DialogMenuTexts , Me [ PlayerNum ] . Chat_Flags [ ChatPartnerCode ]  , 1 , -1 , FPS_Display_BFont , ChatDroid );
	    //--------------------
	    // We do some correction of the menu selection variable:
	    // The first entry of the menu will give a 1 and so on and therefore
	    // we need to correct this to more C style.
	    //
	    MenuSelection --;
	}
	if ( ( MenuSelection >= MAX_ANSWERS_PER_PERSON ) || ( MenuSelection < 0 ) )
	{
	    DebugPrintf ( 0 , "%s: Error: MenuSelection %i out of range!\n" , __FUNCTION__, MenuSelection );
	    MenuSelection = END_ANSWER ;
	}
	
	ProcessThisChatOption ( MenuSelection , PlayerNum , ChatPartnerCode , ChatDroid );
	
	if ( ! ChatDroid -> is_friendly ) return ;
	
	if ( MenuSelection == END_ANSWER )
	{
	    return;
	}
    }
    
}; // void DoChatFromChatRosterData( ... )

/* ----------------------------------------------------------------------
 * When the Tux (or rather the player :) ) clicks on a friendly droid,
 * a chat menu will be invoked to do the communication with that friendly
 * character.  However, before the chat menu even starts up, there is a
 * certain time frame still spent in the isometric viewpoint where the
 * two characters (Tux and the chat partner) should turn to each other,
 * so the scene looks a bit more personal and realistic.  This function
 * handles that time interval and the two characters turning to each
 * other.
 * ---------------------------------------------------------------------- */
void
DialogPartnersTurnToEachOther ( Enemy ChatDroid )
{
    int TurningDone = FALSE;
    float AngleInBetween;
    float WaitBeforeTurningTime = 0.1 ;
    float WaitAfterTurningTime = 0.1 ;
    int TurningStartTime;
    float OldAngle;
    float RightAngle;
    float TurningDirection;
    
#define TURN_SPEED 90.0

    //--------------------
    // We reset the mouse cursor shape and abort any other
    // mouse global mode operation.
    //
    global_ingame_mode = GLOBAL_INGAME_MODE_NORMAL ;
    
    Activate_Conservative_Frame_Computation();
    
    //--------------------
    // We make sure the one droid in question is in the standing and not
    // in the middle of the walking motion when turning to the chat partner...
    //
    // Calling AnimatEnemies() ONCE for this task seems justified...
    //
    ChatDroid -> speed . x = 0 ;
    ChatDroid -> speed . y = 0 ;
    AnimateEnemys ( ) ; 
    
    //--------------------
    // At first do some waiting before the turning around starts...
    //
    TurningStartTime = SDL_GetTicks();  TurningDone = FALSE ;
    while ( !TurningDone )
    {
	StartTakingTimeForFPSCalculation();       
	
	AssembleCombatPicture ( SHOW_ITEMS | USE_OWN_MOUSE_CURSOR ); 
	
	our_SDL_flip_wrapper ( Screen );
	
	if ( ( SDL_GetTicks() - TurningStartTime ) >= 1000.0 * WaitBeforeTurningTime )
	    TurningDone = TRUE;
	
	ComputeFPSForThisFrame();
    }
    
    //--------------------
    // Now we find out what the final target direction of facing should
    // be.
    //
    // For this we use the atan2, which gives angles from -pi to +pi.
    // 
    // Attention must be paid, since 'y' in our coordinates ascends when
    // moving down and descends when moving 'up' on the scren.  So that
    // one sign must be corrected, so that everything is right again.
    //
    RightAngle = ( atan2 ( - ( Me [ 0 ] . pos . y - ChatDroid -> pos . y ) ,  
			   + ( Me [ 0 ] . pos . x - ChatDroid -> pos . x ) ) * 180.0 / M_PI ) ;
    //
    // Another thing there is, that must also be corrected:  '0' begins
    // with facing 'down' in the current rotation models.  Therefore angle
    // 0 corresponds to that.  We need to shift again...
    //
    RightAngle += 90 ;
    
    //--------------------
    // Now it's time do determine which direction to move, i.e. if to 
    // turn to the left or to turn to the right...  For this purpose
    // we convert the current angle, which is between 270 and -90 degrees
    // to one between -180 and +180 degrees...
    //
    if ( RightAngle > 180.0 ) RightAngle -= 360.0 ; 
    
    // DebugPrintf ( 0 , "\nRightAngle: %f." , RightAngle );
    // DebugPrintf ( 0 , "\nCurrent angle: %f." , ChatDroid -> current_angle );
    
    //--------------------
    // Having done these preparations, it's now easy to determine the right
    // direction of rotation...
    //
    AngleInBetween = RightAngle - ChatDroid -> current_angle ;
    if ( AngleInBetween > 180 ) AngleInBetween -= 360;
    if ( AngleInBetween <= -180 ) AngleInBetween += 360;
    
    if ( AngleInBetween > 0 )
	TurningDirection = +1 ; 
    else 
	TurningDirection = -1 ; 
    
    //--------------------
    // Now we turn and show the image until both chat partners are
    // facing each other, mostly the chat partner is facing the Tux,
    // since the Tux may still turn around to somewhere else all the 
    // while, if the chose so
    //
    TurningStartTime = SDL_GetTicks();  TurningDone = FALSE ;
    while ( !TurningDone )
    {
	StartTakingTimeForFPSCalculation();       
	
	AssembleCombatPicture ( SHOW_ITEMS | USE_OWN_MOUSE_CURSOR ); 
	our_SDL_flip_wrapper ( Screen );
	
	OldAngle = ChatDroid -> current_angle;
	
	ChatDroid -> current_angle = OldAngle + TurningDirection * Frame_Time() * TURN_SPEED ;
	
	//--------------------
	// In case of positive turning direction, we wait, till our angle is greater
	// than the right angle.
	// Otherwise we wait till our angle is lower than the right angle.
	//
	AngleInBetween = RightAngle - ChatDroid -> current_angle ;
	if ( AngleInBetween > 180 ) AngleInBetween -= 360;
	if ( AngleInBetween <= -180 ) AngleInBetween += 360;
	
	if ( ( TurningDirection > 0 ) && ( AngleInBetween < 0 ) ) TurningDone = TRUE;
	if ( ( TurningDirection < 0 ) && ( AngleInBetween > 0 ) ) TurningDone = TRUE;
	
	ComputeFPSForThisFrame();
    }
    
    //--------------------
    // Now that turning around is basically done, we still wait a few frames
    // until we start the dialog...
    //
    TurningStartTime = SDL_GetTicks();  TurningDone = FALSE ;
    while ( !TurningDone )
    {
	StartTakingTimeForFPSCalculation();       
	
	AssembleCombatPicture ( SHOW_ITEMS | USE_OWN_MOUSE_CURSOR ); 
	our_SDL_flip_wrapper ( Screen );
	
	if ( ( SDL_GetTicks() - TurningStartTime ) >= 1000.0 * WaitAfterTurningTime )
	    TurningDone = TRUE;
	
	ComputeFPSForThisFrame();
    }
    
}; // void DialogPartnersTurnToEachOther ( Enemy ChatDroid )

/* ----------------------------------------------------------------------
 * This is more or less the 'main' function of the chat with friendly 
 * droids and characters.  It is invoked directly from the user interface
 * function as soon as the player requests communication or there is a
 * friendly bot who rushes Tux and opens talk.
 * ---------------------------------------------------------------------- */
void 
ChatWithFriendlyDroid( Enemy ChatDroid )
{
    int i ;
    SDL_Rect Chat_Window;
    char* DialogMenuTexts[ MAX_ANSWERS_PER_PERSON ];
    int ChatFlagsIndex = (-1);
    char *fpath;
    char tmp_filename[5000];
    
    //--------------------
    // Now that we know, that a chat with a friendly droid is planned, the 
    // friendly droid and the Tux should first turn to each other before the
    // real dialog is started...
    //
    DialogPartnersTurnToEachOther ( ChatDroid );
    
    Chat_Window . x = 242 ; Chat_Window . y = 100; Chat_Window . w = 380; Chat_Window . h = 314;
    
    //--------------------
    // First we empty the array of possible answers in the
    // chat interface.
    //
    for ( i = 0 ; i < MAX_ANSWERS_PER_PERSON ; i ++ )
    {
	DialogMenuTexts [ i ] = "" ;
    }
    
    //--------------------
    // This should make all the answering possibilities available
    // that are there without any prerequisite and that can be played
    // through again and again without any modification.
    //
    RestoreChatVariableToInitialValue( 0 ); // Player=0 for now.
    
    // From initiating transfer mode, space might still have been pressed. 
    // So we wait till it's released...
    while ( SpacePressed ( ) );
    
    //--------------------
    // We clean out the chat roster from any previous use
    //
    InitChatRosterForNewDialogue(  );
    
    ChatFlagsIndex = ResolveDialogSectionToChatFlagsIndex ( ChatDroid -> dialog_section_name ) ;
    
    //--------------------
    // Now that the 'LoadChatRosterWithChatSequence' function will also be
    // used from within the dialog editor, but with explicit path and file
    // name there, we can not assemble the file name inside of the function
    // but must do it here instead...
    //
    strcpy ( tmp_filename , ChatDroid -> dialog_section_name );
    strcat ( tmp_filename , ".dialog" );
    fpath = find_file ( tmp_filename , DIALOG_DIR, FALSE);
    LoadChatRosterWithChatSequence ( fpath );
    
    //--------------------
    // Now with the loaded chat data, we can do the real chat now...
    //
    DoChatFromChatRosterData( 0 , ChatFlagsIndex , ChatDroid , TRUE );
    
}; // void ChatWithFriendlyDroid( int Enum );



#undef _chat_c
