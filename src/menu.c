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
 * This file contains all menu functions and their subfunctions
 * ---------------------------------------------------------------------- */

#define _menu_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"

#include "scandir.h"

int New_Game_Requested = FALSE ;

int Single_Player_Menu (void);
int Multi_Player_Menu (void);
void Credits_Menu (void);
void Contribute_Menu (void);
void Options_Menu (void);
void Show_Mission_Log_Menu (void);

EXTERN void LevelEditor(void);
extern int MyCursorX;
extern int MyCursorY;
extern int load_game_command_came_from_inside_running_game;

#define SELL_PRICE_FACTOR (0.25)
#define REPAIR_PRICE_FACTOR (0.5)

#define SINGLE_PLAYER_STRING "Single Player"
#define LOAD_EXISTING_HERO_STRING "The first 10 characters: "
#define DELETE_EXISTING_HERO_STRING "Select character to delete: "

#define MENU_SELECTION_DEBUG 1

/* ----------------------------------------------------------------------
 * This function tells over which menu item the mouse cursor would be,
 * if there were infinitely many menu items.
 * ---------------------------------------------------------------------- */
int
MouseCursorIsOverMenuItem( int first_menu_item_pos_y , int h )
{
    int PureFraction;

    PureFraction = ( GetMousePos_y ()  - first_menu_item_pos_y ) / h ;

    //--------------------
    // Now it can be that the pure difference is negative or that it is positive.
    // However we should not always round thowards zero here, but rather always to
    // the next LOWER integer!  This will be done here:
    //
    if ( ( GetMousePos_y ()  - first_menu_item_pos_y ) < 0 )
	PureFraction--;
    else
	PureFraction++;
    
    return ( PureFraction );
    
}; // void MouseCursorIsOverMenuItem( first_menu_item_pos_y )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
print_menu_text ( char* InitialText , char* MenuTexts[] , int first_menu_item_pos_y , int background_code , void* MenuFont ) 
{
    char open_gl_string [ 2000 ];
    int h = FontHeight ( GetCurrentFont ( ) );
    int i;
    
    //--------------------
    // We need to prepare the background for the menu, so that
    // it can be accessed with proper speed later...
    //
    InitiateMenu ( background_code );
    
    //--------------------
    // Maybe if this is the very first startup menu, we should also print
    // out some status variables like whether using OpenGL or not DIRECTLY
    // ON THE MENU SCREEN...
    //
    if ( ! strcmp ( MenuTexts [ 0 ] , SINGLE_PLAYER_STRING ) )
    {
	SetCurrentFont ( FPS_Display_BFont );
	RightPutString( Screen , GameConfig . screen_height - 1 * FontHeight ( GetCurrentFont() ) , VERSION );
	// printf ("\n%s %s  \n", PACKAGE, VERSION);
	sprintf ( open_gl_string , "OpenGL support compiled: " );
#ifdef HAVE_LIBGL
	strcat ( open_gl_string , " YES " ) ;
#else
	strcat ( open_gl_string , " NO " ) ;
#endif
	LeftPutString( Screen , GameConfig . screen_height - 2 * FontHeight ( GetCurrentFont() ) , open_gl_string );
	sprintf ( open_gl_string , "OpenGL output active: " );
	if ( use_open_gl )
	    strcat ( open_gl_string , " YES " ) ;
	else
	    strcat ( open_gl_string , " NO " ) ;
	LeftPutString( Screen , GameConfig . screen_height - FontHeight ( GetCurrentFont() ) , open_gl_string );
    }
    
    //--------------------
    // Now that the possible font-changing small info printing is
    // done, we can finally set the right font for the menu itself.
    //
    if ( MenuFont == NULL ) SetCurrentFont ( Menu_BFont );
    else SetCurrentFont ( (BFont_Info*) MenuFont );
    h = FontHeight ( GetCurrentFont() );
    
    if ( ( GameConfig . menu_mode == MENU_MODE_DOUBLE ) ||
	 ( GameConfig . menu_mode == MENU_MODE_FAST ) )
    {
	for ( i = 0 ; TRUE ; i ++ )
	{
	    if ( strlen( MenuTexts[ i ] ) == 0 ) break;
	    CutDownStringToMaximalSize ( MenuTexts [ i ] , 550 );
	    CenteredPutString ( Screen ,  first_menu_item_pos_y + i * h , MenuTexts[ i ] );
	}
	if ( strlen( InitialText ) > 0 ) 
	    DisplayText ( InitialText , 50 , 50 , NULL , TEXT_STRETCH );
    }
    
}; // void print_menu_text ( ... )

/* ----------------------------------------------------------------------
 * This function performs a menu for the player to select from, using the
 * keyboard only, currently, sorry.
 *
 * This function EXPECTS, that MenuTexts[] is an array of strings, THE 
 * LAST OF WHICH IS AN EMPTY STRING TO DENOTE THE END OF THE ARRAY!  If 
 * this is not respected, segfault errors are likely.
 * 
 * ---------------------------------------------------------------------- */
int
DoMenuSelection( char* InitialText , char* MenuTexts[] , int FirstItem , int background_code , void* MenuFont )
{
    int h;
    int i;
    static int MenuPosition = 1;
    int NumberOfOptionsGiven;
    int LongestOption;
    SDL_Rect HighlightRect;
    SDL_Rect BackgroundRect;
    int first_menu_item_pos_y;
    
    //--------------------
    // At first we hide the system mouse cursor, because we want to use
    // our own creation in the menus too...
    //
    make_sure_system_mouse_cursor_is_turned_off();
    
    //--------------------
    // We set the given font, if appropriate, and set the font height variable...
    //
    if ( MenuFont != NULL )
	SetCurrentFont ( MenuFont );
    h = FontHeight ( GetCurrentFont ( ) );
    
    //--------------------
    // Some menus are intended to start with the default setting of the
    // first menu option selected.  However this is not always desired.
    // It might happen, that a submenu returns to the upper menu and then
    // the upper menu should not be reset to the first position selected.
    // For this case we have some special '-1' entry reserved as the marked
    // menu entry.  This means, taht the menu position from last time will
    // simply be re-used.
    //
    if ( FirstItem != (-1) ) MenuPosition = FirstItem;
    
    //--------------------
    // First thing we do is find out how may options we have
    // been given for the menu
    //
    LongestOption = 0 ;
    for ( i = 0 ; TRUE ; i ++ )
    {
    if ( strlen( MenuTexts[ i ] ) == 0 )
        break ;
    else if ( TextWidth( MenuTexts[ i ] ) > LongestOption )
        LongestOption = TextWidth( MenuTexts[ i ] );
    }
    NumberOfOptionsGiven = i;
    
    //--------------------
    // In those cases where we don't reset the menu position upon 
    // initalization of the menu, we must check for menu positions
    // outside the bounds of the current menu.
    //
    if ( MenuPosition > NumberOfOptionsGiven ) MenuPosition = 1 ; 
    
    first_menu_item_pos_y = ( GameConfig . screen_height - NumberOfOptionsGiven * h ) / 2 ;
    
    print_menu_text ( InitialText , MenuTexts , first_menu_item_pos_y , background_code , MenuFont ) ;
    
    if ( ! use_open_gl ) StoreMenuBackground ( 0 );
    

    
    while ( 1 )
    {
	//--------------------
	// We write out the normal text of the menu, either by doing it once more
	// in the open_gl case or by restoring what we have saved earlier, in the 
	// SDL output case.
	//
	if ( ! use_open_gl ) 
	    RestoreMenuBackground ( 0 );
	else
	    print_menu_text ( InitialText , MenuTexts , FirstItem , background_code , MenuFont ) ;
	
	//--------------------
	// Maybe we should display some thumbnails with the saved games entries?
	// But this will only apply for the load_hero and the delete_hero menus...
	//
	if ( ( ! strcmp ( InitialText , LOAD_EXISTING_HERO_STRING ) ) ||
	     ( ! strcmp ( InitialText , DELETE_EXISTING_HERO_STRING ) ) )
	{
	    //--------------------
	    // We load the thumbnail, or at least we try to do it...
	    //
	    LoadAndShowThumbnail ( MenuTexts [ MenuPosition - 1 ] );
	    LoadAndShowStats ( MenuTexts [ MenuPosition - 1 ] );
	}
	
	//--------------------
	// Depending on what highlight method has been used, we so some highlighting
	// of the currently selected menu options location on the screen...
	//

    BackgroundRect.x = ( GameConfig . screen_width - LongestOption ) / 2 - 50 ;
    BackgroundRect.y = first_menu_item_pos_y - 50 ;
    BackgroundRect.w = LongestOption + 100 ;
    BackgroundRect.h = ( h * NumberOfOptionsGiven ) + 100 ;
    ShadowingRectangle ( Screen, BackgroundRect );

    HighlightRect.x = ( GameConfig . screen_width - TextWidth ( MenuTexts [ MenuPosition - 1 ] ) ) / 2 - h ;
	HighlightRect.y = first_menu_item_pos_y + ( MenuPosition - 1 ) * h ;
	HighlightRect.w = TextWidth ( MenuTexts [ MenuPosition - 1 ] ) + 2 * h ;
	HighlightRect.h = h;		    
	HighlightRectangle ( Screen , HighlightRect );


    
    
	if ( ( GameConfig . menu_mode == MENU_MODE_DOUBLE ) ||
	     ( GameConfig . menu_mode == MENU_MODE_DEFAULT ) )
	{
	    for ( i = 0 ; TRUE ; i ++ )
	    {
		if ( strlen( MenuTexts[ i ] ) == 0 ) break;
		CutDownStringToMaximalSize ( MenuTexts [ i ] , 550 );
		CenteredPutString ( Screen ,  first_menu_item_pos_y + i * h , MenuTexts[ i ] );
	    }
	    if ( strlen( InitialText ) > 0 ) 
		DisplayText ( InitialText , 50 , 50 , NULL , TEXT_STRETCH );
	}
	
	//--------------------
	// Now the mouse cursor must be brought to the screen
	//
	make_sure_system_mouse_cursor_is_turned_off();
	blit_our_own_mouse_cursor();

	//--------------------
	// Image should be ready now, so we can show it...
	//
	our_SDL_flip_wrapper( Screen );
	
	//--------------------
	// Now it's time to handle the possible keyboard and mouse 
	// input from the user...
	//
	if ( EscapePressed() )
	{
	    while ( EscapePressed() );
	    MenuItemDeselectedSound();
	    return ( -1 );
	}
	if ( EnterPressed() || ( SpacePressed() && !axis_is_active ) || RightPressed() || LeftPressed() ) 
	{
	    //--------------------
	    // The space key or enter key or left mouse button all indicate, that
	    // the user has made a selection.
	    //
	    // In the case of the mouse button, we must of couse first check, if 
	    // the mouse button really was over a valid menu item and otherwise
	    // ignore the button.
	    //
	    // In case of a key, we always have a valid selection.
	    //
	    while ( EnterPressed() || SpacePressed() ); 
	    MenuItemSelectedSound();
	    return ( MenuPosition );
	}
	if ( axis_is_active )
	{
	    while ( EnterPressed() || SpacePressed() ); // || RightPressed() || LeftPressed() );
	    //--------------------
	    // Only when the mouse click really occured on the menu do we
	    // interpret it as a menu choice.  Otherwise we'll just ignore
	    // it.
	    //
	    if ( MouseCursorIsOverMenuItem( first_menu_item_pos_y , h ) == MenuPosition )
	    {
		MenuItemSelectedSound();
		return ( MenuPosition );
	    }
	}
	if ( UpPressed() || MouseWheelUpPressed() ) 
	{
	    if (MenuPosition > 1) MenuPosition--;
	    MoveMenuPositionSound();
	    HighlightRect.x = 320 ; // ( TextWidth ( MenuTexts [ MenuPosition - 1 ] ) ) / 2 ;
	    HighlightRect.y = first_menu_item_pos_y + ( MenuPosition - 1 ) * h ;
	    SDL_WarpMouse ( HighlightRect.x , HighlightRect.y );
	    while (UpPressed());
	}
	if ( DownPressed() || MouseWheelDownPressed() ) 
	{
	    if ( MenuPosition < NumberOfOptionsGiven ) MenuPosition++;
	    MoveMenuPositionSound();
	    HighlightRect.x = 320 ; // ( TextWidth ( MenuTexts [ MenuPosition - 1 ] ) ) / 2 ;
	    HighlightRect.y = first_menu_item_pos_y + ( MenuPosition - 1 ) * h ;
	    SDL_WarpMouse ( HighlightRect.x , HighlightRect.y );
	    while (DownPressed());
	}
	
	MenuPosition = MouseCursorIsOverMenuItem( first_menu_item_pos_y , h );
	if ( MenuPosition < 1 ) MenuPosition = 1 ;
	if ( MenuPosition > NumberOfOptionsGiven ) MenuPosition = NumberOfOptionsGiven ;
	
	//--------------------
	// At this the while (1) overloop ends.  But for the menu, we really do not
	// need to hog the CPU.  Therefore some waiting should be introduced here.
	//
	SDL_Delay (1);
	//usleep ( 1 ) ;
    }
    
    return ( -1 );
}; // int DoMenuSelection( ... )

/* ----------------------------------------------------------------------
 * This function performs a menu for the player to select from, using the
 * keyboard or mouse wheel.
 * ---------------------------------------------------------------------- */
int
ChatDoMenuSelectionFlagged( char* InitialText , char* MenuTexts[ MAX_ANSWERS_PER_PERSON] , 
			    unsigned char Chat_Flags[ MAX_ANSWERS_PER_PERSON ] , int FirstItem , 
			    int background_code , void* MenuFont , enemy* ChatDroid )
{
  int MenuSelection = (-1) ;
  char* FilteredChatMenuTexts[ MAX_ANSWERS_PER_PERSON ] ;
  int i;
  int use_counter = 0;

  DebugPrintf ( 2 , "\nBEFORE:  First Item now: %d." , FirstItem );

  //--------------------
  // We filter out those answering options that are allowed by the flag mask
  //
  DebugPrintf ( MENU_SELECTION_DEBUG , "\n%s(): %d \n" , __FUNCTION__ , FirstItem ) ; 
  for ( i = 0 ; i < MAX_ANSWERS_PER_PERSON ; i ++ )
    {
      FilteredChatMenuTexts [ i ] = "" ; 
      if ( Chat_Flags[ i ] ) 
	{

	  DebugPrintf ( MENU_SELECTION_DEBUG , "%2d. " , i ) ; 
	  DebugPrintf ( MENU_SELECTION_DEBUG , "%s\n" , MenuTexts [ i ] ) ; 

	  fflush ( stdout );
	  fflush ( stderr );

	  FilteredChatMenuTexts[ use_counter ] = MenuTexts[ i ] ;
	  use_counter++;
	}
    }

  DebugPrintf ( 2 , "\nMIDDLE:  First Item now: %d." , FirstItem );

  //--------------------
  // Now we do the usual menu selection, using only the activated chat alternatives...
  //
  MenuSelection = ChatDoMenuSelection( InitialText , FilteredChatMenuTexts , 
				       FirstItem , background_code , MenuFont , ChatDroid );

  //--------------------
  // Now that we have an answer, we must transpose it back to the original array
  // of all theoretically possible answering possibilities.
  //
  if ( MenuSelection != (-1) )
    {
      use_counter = 0 ;
      for ( i = 0 ; i < MAX_ANSWERS_PER_PERSON ; i ++ )
	{

	  if ( Chat_Flags[ i ] ) 
	    {
	      FilteredChatMenuTexts[ use_counter ] = MenuTexts[ i ] ;
	      use_counter++;
	      if ( MenuSelection == use_counter ) 
		{
		  DebugPrintf( 1 , "\nOriginal MenuSelect: %d. \nTransposed MenuSelect: %d." , 
			       MenuSelection , i+1 );
		  return ( i+1 );
		}
	    }
	}
    }

  return ( MenuSelection );
}; // int ChatDoMenuSelectionFlagged( char* InitialText , char* MenuTexts[ MAX_ANSWERS_PER_PERSON] , ... 

/* ----------------------------------------------------------------------
 * Without destroying or changing anything, this function should determine
 * how many lines of text it takes to write a string with the current 
 * font into the given rectangle, provided one would start at the left
 * side as one usually does...
 * ---------------------------------------------------------------------- */
int
GetNumberOfTextLinesNeeded ( char* GivenText, SDL_Rect GivenRectangle , float text_stretch )
{
    int BackupOfMyCursorX, BackupOfMyCursorY;
    int TextLinesNeeded;
    int i;
    int TestPosition;
    int stored_height ;
    
    //--------------------
    // If we receive an empty string, we print out a warning message and then
    // return one line as the required amount of lines.
    //
    if ( strlen ( GivenText ) <= 1 )
    {
	/*
	GiveStandardErrorMessage ( __FUNCTION__  , "\
Warning.  Received empty or nearly empty string!",
				   NO_NEED_TO_INFORM, IS_WARNING_ONLY );
	*/
	return ( 1 ) ;
    }
    
    //--------------------
    // First we make a backup of everything, so that we don't destory anything.
    //
    display_char_disabled = TRUE ;
    BackupOfMyCursorX = MyCursorX;
    BackupOfMyCursorY = MyCursorY;
    
    //--------------------
    // Now in our simulated environment, we can blit the Text and see how many lines it takes...
    //
    MyCursorX = GivenRectangle . x ;
    MyCursorY = GivenRectangle . y ;
    TestPosition = MyCursorY ;
    
    stored_height = GivenRectangle.h ;
    GivenRectangle.h = 32000 ;
    TextLinesNeeded = DisplayText ( GivenText , GivenRectangle.x , GivenRectangle.y , &GivenRectangle , text_stretch );
    GivenRectangle.h = stored_height ;

    //--------------------
    // Now that we have found our solution, we can restore everything back to normal
    //
    // RestoreMenuBackground ( 1 ) ;
    display_char_disabled = FALSE ;
    
    MyCursorX = BackupOfMyCursorX;
    MyCursorY = BackupOfMyCursorY;
    
    return ( TextLinesNeeded );
    
}; // int GetNumberOfTextLinesNeeded ( MenuTexts [ i ] , Choice_Window )

/* ----------------------------------------------------------------------
 *
 * This function performs a menu for the player to select from, using the
 * keyboard or mouse wheel.
 *
 * Some complaints have been made about earlier versions of this menu
 * reacting very slow and sluggish AND also backgrounds not appearing when
 * using OpenGL output method.  These concerns have been addressed in the
 * following manner:
 *
 * 1. There is STILL a loop with the graphics being completely redrawn,
 *    because buffering the graphics seems to cause said above OpenGL pixel
 *    operation problems.
 * 2. The loop mentioned in 1. has been fitted with an inner wait loop, 
 *    that will wait until the mouse cursor (or some other event) has made
 *    redrawing the scene nescessary, while still polling the mouse and
 *    keyboard input.
 *
 * The rest of the menu is made particularly unclear by the fact that there
 * can be some multi-line options too and also there is scrolling up and
 * down possible, when there are more menu options than fit onto one
 * dialog options secection window for the player to click from.
 *
 * ---------------------------------------------------------------------- */
int
ChatDoMenuSelection( char* InitialText , char* MenuTexts[ MAX_ANSWERS_PER_PERSON ] , 
		     int FirstItem , int background_to_use , void* MenuFont , enemy* ChatDroid )
{
    int h = FontHeight (GetCurrentFont());
    int i , j ;
    static int menu_position_to_remember = 1;
    int NumberOfOptionsGiven;
#define ITEM_DIST 50
    int MenuPosX [ MAX_ANSWERS_PER_PERSON ] ;
    int MenuPosY [ MAX_ANSWERS_PER_PERSON ] ;
    int MenuOptionLineRequirement [ MAX_ANSWERS_PER_PERSON ] ;
    SDL_Rect Choice_Window;
    SDL_Rect HighlightRect;
    int MaxLinesInMenuRectangle;
    int OptionOffset = 0 ;
    int SpaceUsedSoFar;
    int BreakOffCauseAllDisplayed ;
    int BreakOffCauseNoRoom = FALSE ;
    int LastOptionVisible = 0 ;
    int MenuLineOfMouseCursor;
    int ThisOptionEnd;
    int mouse_wheel_has_turned = FALSE ;
    int mouse_now_over_different_item = FALSE ;
    int cursors_menu_position = - 1000 ;
    int mouse_has_moved = FALSE ;
    int old_mouse_x = (-1) ;
    int old_mouse_y = (-1) ;
#if __WIN32__
    int win32_iterations = 0 ;
#endif
    
    //--------------------
    // First we initialize the menu positions
    //
    for ( i = 0 ; i < MAX_ANSWERS_PER_PERSON ; i ++ )
    {
	MenuPosX [ i ] = 260 ; 
	MenuPosY [ i ] =  90 + i * ITEM_DIST ; 
	MenuOptionLineRequirement [ i ] = 0;
    }
    
    //--------------------
    // Now we set some viable choice window and we compute the maximum number of lines
    // that will still fit well into the choice window.
    //
    Choice_Window . x = 35*GameConfig . screen_width/640; Choice_Window . y = 340*GameConfig . screen_height/480; Choice_Window . w = ( 640 - 70 ) * GameConfig . screen_width / 640 ; Choice_Window . h = 110 * GameConfig . screen_height / 480 ;
    MaxLinesInMenuRectangle = Choice_Window . h / ( FontHeight ( GetCurrentFont() ) * TEXT_STRETCH ) ;
    MaxLinesInMenuRectangle = 5;
    DebugPrintf ( 1 , "\nComputed number of lines in choice window at most: %d." , MaxLinesInMenuRectangle );
    
    //--------------------
    // We don't need the system mouse cursor, as we do have our own for
    // the same purpose.
    //
    make_sure_system_mouse_cursor_is_turned_off();
    
    if ( FirstItem != (-1) ) menu_position_to_remember = FirstItem;
    
    
    //--------------------
    // First thing we do is find out how may options we have
    // been given for the menu
    //
    DebugPrintf ( MENU_SELECTION_DEBUG , "\n%s(): %d \n" , __FUNCTION__ , FirstItem ) ; 
    for ( i = 0 ; i < MAX_ANSWERS_PER_PERSON ; i ++ )
    {
	DebugPrintf ( MENU_SELECTION_DEBUG , "%2d. " , i ) ; 
	DebugPrintf ( MENU_SELECTION_DEBUG , MenuTexts [ i ] ) ; 
	DebugPrintf ( MENU_SELECTION_DEBUG , "\n" ) ; 
	fflush ( stdout );
	fflush ( stderr );
	
	if ( strlen( MenuTexts[ i ] ) == 0 ) break;
    }
    NumberOfOptionsGiven = i ;
    
    
    //--------------------
    // We need to prepare the background for the menu, so that
    // it can be accessed with proper speed later...
    //
    SDL_SetClipRect( Screen, NULL );
    StoreMenuBackground ( 0 );
    
    //--------------------
    // Now that the possible font-changing background assembling is
    // done, we can finally set the right font for the menu itself.
    //
    if ( MenuFont == NULL ) SetCurrentFont ( Menu_BFont );
    else SetCurrentFont ( (BFont_Info*) MenuFont );
    h = FontHeight ( GetCurrentFont() );
    
    OptionOffset = 0 ;
    while ( 1 )
    {
    SDL_Delay(10);
#if __WIN32__
      for ( win32_iterations = 0 ; win32_iterations < 2 ; win32_iterations ++ ) 
      {
#endif

	  if ( ! use_open_gl ) 
	      RestoreMenuBackground ( 0 );
	  else 
	  {
	      //--------------------
	      // This will re-blit the background and then put the current chat protocol
	      // right over it.  However a screen-update/flip is not performed unless we
	      // would request it, which we don't do YET.
	      //
	      display_current_chat_protocol ( CHAT_DIALOG_BACKGROUND_PICTURE_CODE , ChatDroid , FALSE );
	  }

	  //--------------------
	  // Now that the possible font-changing chat protocol display is
	  // done, we can finally set the right font for the menu itself.
	  //
	  if ( MenuFont == NULL ) SetCurrentFont ( Menu_BFont );
	  else SetCurrentFont ( (BFont_Info*) MenuFont );
	  h = FontHeight ( GetCurrentFont() );
	  
  	  //--------------------
	  // We blit to the screen all the options that are not empty and that still fit
	  // onto the screen
	  //
	  SpaceUsedSoFar = 0 ;
	  for ( i = OptionOffset ; i < MAX_ANSWERS_PER_PERSON ; i ++ )
	  {
	      //--------------------
	      // If all has been displayed already, we quit blitting...
	      //
	      if ( strlen( MenuTexts[ i ] ) == 0 ) 
	      {
		  BreakOffCauseAllDisplayed = TRUE ;
		  BreakOffCauseNoRoom = FALSE ;
		  LastOptionVisible = i ;
		  break;
	      }
	      //--------------------
	      // If there is not enough room any more, we quit blitting...
	      //
	      if ( SpaceUsedSoFar  > MaxLinesInMenuRectangle * FontHeight ( GetCurrentFont () ) ) 
	      {
		  BreakOffCauseAllDisplayed = FALSE ;
		  BreakOffCauseNoRoom = TRUE ;
		  LastOptionVisible = i ;
		  break;
	      }
	      
	      //--------------------
	      // Now that we know, that there is enough room, we can blit the next menu option.
	      //
	      MenuPosX [ i ] = Choice_Window.x;
	      MenuPosY [ i ] = Choice_Window.y + SpaceUsedSoFar;
	      MenuOptionLineRequirement [ i ]  = DisplayText ( MenuTexts [ i ] , Choice_Window.x , Choice_Window.y + SpaceUsedSoFar , &Choice_Window , TEXT_STRETCH );
	      SpaceUsedSoFar += MenuOptionLineRequirement [ i ] * ( FontHeight ( GetCurrentFont() ) * TEXT_STRETCH );
	  }


	  //--------------------
	  // We highlight the currently selected option with a highlighting rectangle
	  //
	  // (and we add some security against 'empty' chat selection menus causing
	  // some segfaults rather easily...)
	  //
	  DebugPrintf ( 1 , "\n%s(): menu_position_to_remember: %d." , __FUNCTION__ , menu_position_to_remember );
	  DebugPrintf ( 1 , "\n%s(): FirstItem: %d." , __FUNCTION__ , FirstItem );
	  if ( menu_position_to_remember <= 0 ) 
	      menu_position_to_remember = 1 ;
	  HighlightRect.x = MenuPosX[ menu_position_to_remember -1 ] - 0 * h ;
	  HighlightRect.y = MenuPosY[ menu_position_to_remember -1 ] ;
	  HighlightRect.w = TextWidth ( MenuTexts [ menu_position_to_remember - 1 ] ) + 0 * h ;
	  if ( HighlightRect . w > 550 * GameConfig . screen_width / 640 ) HighlightRect . w = 550 * GameConfig . screen_width / 640 ;
	  HighlightRect.h = MenuOptionLineRequirement [ menu_position_to_remember - 1 ] * 
	      ( FontHeight ( GetCurrentFont() ) * TEXT_STRETCH ) ;	    
	  HighlightRectangle ( Screen , HighlightRect );
	  // Display again the highlighted line
	  DisplayText ( MenuTexts [ menu_position_to_remember -1 ] , MenuPosX [ menu_position_to_remember - 1 ], MenuPosY [ menu_position_to_remember - 1 ] , &Choice_Window , TEXT_STRETCH);	  
	  
	  if ( BreakOffCauseNoRoom ) ShowGenericButtonFromList ( SCROLL_DIALOG_MENU_DOWN_BUTTON );
	  if ( OptionOffset ) ShowGenericButtonFromList ( SCROLL_DIALOG_MENU_UP_BUTTON );
	  
	  //--------------------
	  // Now the mouse cursor must be brought to the screen
	  //
	  make_sure_system_mouse_cursor_is_turned_off();
	  blit_our_own_mouse_cursor();

	  //--------------------
	  // Now everything should become visible!
	  //
	  our_SDL_flip_wrapper( Screen );
	  
#if __WIN32__
      }  // doing display twice for win32 platform...
#endif
      
      //--------------------
      // In order to reduce processor load during chat menus and also in order to
      // make menus lag less, we introduce a new loop here, so that the drawing thing
      // doesn't have to be executed so often...
      //
      mouse_wheel_has_turned = FALSE ;
      mouse_now_over_different_item = FALSE ;
      mouse_has_moved = FALSE ;
      
      while ( !mouse_has_moved && !mouse_now_over_different_item && !mouse_wheel_has_turned && !EscapePressed() && !EnterPressed() && !axis_is_active && !RightPressed() && !LeftPressed() && !UpPressed() && !DownPressed() )
      {

	  if ( GetMousePos_x() != old_mouse_x )
	  {
	      old_mouse_x = GetMousePos_x();
	      old_mouse_y = GetMousePos_y();
	      mouse_has_moved = TRUE ;
	  }
	  else if ( GetMousePos_y() != old_mouse_y )
	  {
	      old_mouse_x = GetMousePos_x();
	      old_mouse_y = GetMousePos_y();
	      mouse_has_moved = TRUE ;
	  }
	  else
	      mouse_has_moved = FALSE ;

	  //--------------------
	  // The MOUSE WHEEL cannot be queried like anything else, since querying it
	  // DOES CHANGE THE STATUS ITSELF, so we can only ask for this once and we
	  // handle it here...  (while the rest can be queried again and handled
	  // correctly then later...)
	  //
	  if ( MouseWheelUpPressed() ) 
	  {
	      if ( menu_position_to_remember > OptionOffset + 1 ) 
	      {
		  SDL_WarpMouse ( GetMousePos_x () , MenuPosY [ menu_position_to_remember - 2 ] ) ;
		  MoveMenuPositionSound();	    
	      }
	      else if ( OptionOffset > 0 ) 
	      {
		  OptionOffset -- ; 
		  MoveMenuPositionSound();	    
	      }
	      mouse_wheel_has_turned = TRUE ;
	      while ( UpPressed ( ) );
	  }
	  if ( MouseWheelDownPressed() ) 
	  {
	      if ( menu_position_to_remember < LastOptionVisible ) 
	      { 
		  SDL_WarpMouse ( GetMousePos_x () , MenuPosY [ menu_position_to_remember ] );
	      }
	      else
	      {
		  if ( BreakOffCauseNoRoom ) 
		  {
		      OptionOffset++;
		      MoveMenuPositionSound();
		  }
		  SDL_WarpMouse ( GetMousePos_x () , MenuPosY [ menu_position_to_remember - 1 ] );
	      }
	      mouse_wheel_has_turned = TRUE ;
	      while ( DownPressed ( ) );
	  }
	  
	  //--------------------
	  // Maybe the mouse is now hovering over a different menu item, that it
	  // was over (and than was therefore selected) before.  Then of course
	  // me must let the main cycle have another go...
	  //
	  MenuLineOfMouseCursor = 
	      MouseCursorIsOverMenuItem ( MenuPosY [ OptionOffset ] , FontHeight ( GetCurrentFont() ) * TEXT_STRETCH ) ;
	  if ( MenuLineOfMouseCursor < 1 ) MenuLineOfMouseCursor = 1 ;
	  
	  cursors_menu_position = 1 ;
	  
	  ThisOptionEnd = MenuPosY [ 0 ] ;
	  for ( i = OptionOffset ; i <= LastOptionVisible ; i ++ )
	  {
	      
	      ThisOptionEnd += MenuOptionLineRequirement [ i ] * ( FontHeight ( GetCurrentFont() ) * TEXT_STRETCH ) ;
	      
	      if ( GetMousePos_y ()  < ThisOptionEnd )
	      {
		  cursors_menu_position = i + 1 ; // MouseCursorIsOverMenuItem( MenuPosY [ 0 ] , MenuPosY [ 1 ] - MenuPosY [ 0 ] );
		  break;
	      }
	  }
	  
	  
	  if ( cursors_menu_position > LastOptionVisible ) 
	      cursors_menu_position = LastOptionVisible ;
	  
	  if ( MenuLineOfMouseCursor <= MaxLinesInMenuRectangle )
	  {
	      //--------------------
	      // Maybe there are some double-lines?!
	      //
	      if ( cursors_menu_position != menu_position_to_remember )
	      {
		  mouse_now_over_different_item = TRUE ;
		  DebugPrintf ( 1 , "\nChatDoMenuSelection:  mouse now over different item, therefore new main cycle..." );
	      }
	  }
	  //	  usleep ( 1 ) ;
	  SDL_Delay(1);
      }
      
      //--------------------
      // 
      //
      if ( EscapePressed() )
      {
	  while ( EscapePressed() );
	  
	  RestoreMenuBackground ( 0 );
	  our_SDL_flip_wrapper( Screen );
	  return ( -1 );
      }
      if ( EnterPressed() || ( SpacePressed() && !axis_is_active ) || RightPressed() || LeftPressed() ) 
      {
	  //--------------------
	  // The space key or enter key or left mouse button all indicate, that
	  // the user has made a selection.
	  //
	  // In the case of the mouse button, we must of couse first check, if 
	  // the mouse button really was over a valid menu item and otherwise
	  // ignore the button.
	  //
	  // In case of a key, we always have a valid selection.
	  //
	  while ( EnterPressed() || SpacePressed() ); // || RightPressed() || LeftPressed() );
	  // MenuItemSelectedSound();
	  RestoreMenuBackground ( 0 );
	  our_SDL_flip_wrapper( Screen );
	  return ( menu_position_to_remember );
      }
      
      if ( axis_is_active )
      {
	  while ( EnterPressed() || SpacePressed() ); // || RightPressed() || LeftPressed() );
	  
	  //--------------------
	  // First we see if there was perhaps a click on one of the active scroll buttons
	  //
	  if ( ( MouseCursorIsOnButton ( SCROLL_DIALOG_MENU_DOWN_BUTTON , GetMousePos_x ()  , GetMousePos_y ()  ) ) &&
	       ( BreakOffCauseNoRoom ) )
	  {
	      OptionOffset ++ ;
	  }
	  else if ( ( MouseCursorIsOnButton ( SCROLL_DIALOG_MENU_UP_BUTTON , GetMousePos_x ()  , GetMousePos_y ()  ) ) &&
		    ( OptionOffset ) )
	  {
	      OptionOffset -- ;
	  }
	  else if ( MouseCursorIsOnButton ( CHAT_PROTOCOL_SCROLL_UP_BUTTON , 
					    GetMousePos_x ()  , 
					    GetMousePos_y ()  ) )
	  {
	      chat_protocol_scroll_override_from_user -- ;
	  }
	  else if ( MouseCursorIsOnButton ( CHAT_PROTOCOL_SCROLL_DOWN_BUTTON , 
					    GetMousePos_x ()  , 
					    GetMousePos_y ()  ) )
	  {
	      chat_protocol_scroll_override_from_user ++ ;
	  }
	  //--------------------
	  // If not, then maybe it was a click into the options window.  That alone
	  // would be enough to call it a valid user decision.
	  //
	  else
	  {
	      //--------------------
	      // Now if the click has occured within the lines of the menu, we will count
	      // this as a valid choice of the user.
	      //
	      MenuLineOfMouseCursor = 
		  MouseCursorIsOverMenuItem ( MenuPosY [ OptionOffset ] , FontHeight ( GetCurrentFont() ) * TEXT_STRETCH ) ;
	      if ( ( MenuLineOfMouseCursor >= 1 ) && ( MenuLineOfMouseCursor <= MaxLinesInMenuRectangle ) )
	      {
		  RestoreMenuBackground ( 0 );
		  our_SDL_flip_wrapper( Screen );
		  return ( menu_position_to_remember );
	      }
	  }
	  
      }
      if ( UpPressed() || MouseWheelUpPressed() ) 
      {
	  if ( menu_position_to_remember > OptionOffset + 1 ) 
	  {
	      SDL_WarpMouse ( GetMousePos_x () , MenuPosY [ menu_position_to_remember - 2 ] ) ;
	      MoveMenuPositionSound();	    
	  }
	  else if ( OptionOffset > 0 ) 
	  {
	      OptionOffset -- ; 
	      MoveMenuPositionSound();	    
	  }
	  
	  while (UpPressed());
      }
      if ( DownPressed() || MouseWheelDownPressed() ) 
      {
	  if ( menu_position_to_remember < LastOptionVisible ) 
	  { 
	      SDL_WarpMouse ( GetMousePos_x () , MenuPosY [ menu_position_to_remember ] );
	  }
	  else
	  {
	      if ( BreakOffCauseNoRoom ) OptionOffset++;
	      SDL_WarpMouse ( GetMousePos_x () , MenuPosY [ menu_position_to_remember - 1 ] );
	  }
	  MoveMenuPositionSound();
	  while (DownPressed());
      }
      
      //--------------------
      // Only if the mouse position really lies within the menu, we will interpret
      // it as menu choice.  Otherwise it will be just ignored.
      //
      MenuLineOfMouseCursor = 
	  MouseCursorIsOverMenuItem ( MenuPosY [ 0 ] , FontHeight ( GetCurrentFont() ) * TEXT_STRETCH ) ;
      if ( MenuLineOfMouseCursor < 1 ) MenuLineOfMouseCursor = 1 ;
      
      //--------------------
      // If the mouse cursor was on one of the possible lines, than we can try to translate
      // it into a real menu position
      //
      if ( ( MenuLineOfMouseCursor >= 0 ) && ( MenuLineOfMouseCursor <= MaxLinesInMenuRectangle ) )
      {
	  ThisOptionEnd = MenuPosY [ 0 ] ;
	  for ( i = OptionOffset ; i <= LastOptionVisible ; i ++ )
	  {
	      
	      ThisOptionEnd += MenuOptionLineRequirement [ i ] * ( FontHeight ( GetCurrentFont() ) * TEXT_STRETCH ) ;
	      
	      if ( GetMousePos_y ()  < ThisOptionEnd )
	      {
		  menu_position_to_remember = i + 1 ; 
		  break;
	      }
	  }
      }
      
      if ( menu_position_to_remember < OptionOffset + 1 ) menu_position_to_remember = OptionOffset + 1 ;
      if ( menu_position_to_remember > LastOptionVisible ) menu_position_to_remember = LastOptionVisible ;
      
    }
    RestoreMenuBackground ( 0 );
    our_SDL_flip_wrapper( Screen );
    return ( -1 );
    
}; // int ChatDoMenuSelection( char* InitialText , char* MenuTexts[] , asdfasd .... )

/* ----------------------------------------------------------------------
 * This function prepares the screen for the big Escape menu and 
 * its submenus.  This means usual content of the screen, i.e. the 
 * combat screen and top status bar, is "faded out", the rest of 
 * the screen is cleared.  This function resolves some redundance 
 * that occured since there are so many submenus needing this.
 * ---------------------------------------------------------------------- */
void 
InitiateMenu( int background_code )
{
    //--------------------
    // Here comes the standard initializer for all the menus and submenus
    // of the big escape menu.  This prepares the screen, so that we can
    // write on it further down.
    //
    SDL_SetClipRect( Screen, NULL );
    
    if ( background_code == ( -1 ) )
    {
	DisplayBanner ( ) ;
	AssembleCombatPicture ( USE_OWN_MOUSE_CURSOR );
	MakeGridOnScreen( NULL );
    }
    else
    {
	// DisplayImage ( find_file ( BackgroundToUse , GRAPHICS_DIR, FALSE ) );
	blit_special_background ( background_code ) ;
    }
    
    SDL_SetClipRect( Screen, NULL );
}; // void InitiateMenu(void)

// extern int CurrentlyCPressed; 	/* the key that brought as in here */
				/* we need to make sure it is set as released */
				/* before we leave ...*/
/* ----------------------------------------------------------------------
 * This function provides a convenient cheat menu, so that any 
 * tester does not have to play all through the game again and again
 * to see if a bug in a certain position has been removed or not.
 * ---------------------------------------------------------------------- */
void
Cheatmenu (void)
{
    char *input;		// string input from user 
    int can_continue;
    int LNum, X, Y, num;
    int i, l;
    int x0, y0, line;
    Waypoint WpList;      // pointer on current waypoint-list  

    //--------------------
    // Prevent distortion of framerate by the delay coming from 
    // the time spend in the menu.
    //
    Activate_Conservative_Frame_Computation();

    //--------------------
    // Some small font is needed, such that we can get a lot of lines on
    // one single cheat menu page...
    //
    SetCurrentFont ( FPS_Display_BFont ); 
				
    x0 = 50;
    y0 = 20;
    line = 0;

    can_continue = FALSE;
    while (!can_continue)
    {
	ClearGraphMem ();
	printf_SDL (Screen, x0, y0, "Current position: Level=%d, X=%d, Y=%d\n",
		    CurLevel->levelnum, (int)Me[0].pos.x, (int)Me[0].pos.y);
	printf_SDL (Screen, -1, -1, " a. Armageddon (alle Robots sprengen)\n");
	printf_SDL (Screen, -1, -1, " l. robot list of current level\n");
	printf_SDL (Screen, -1, -1, " g. complete robot list\n");
	printf_SDL (Screen, -1, -1, " d. destroy robots on current level\n");
	printf_SDL (Screen, -1, -1, " t. Teleportation\n");
	printf_SDL (Screen, -1, -1, " r. change to new robot type\n");
	printf_SDL (Screen, -1, -1, " e. set energy\n");
	printf_SDL (Screen, -1, -1, " h. Auto-aquire all skills\n" );
	printf_SDL (Screen, -1, -1, " n. No hidden droids: %s",
		    show_all_droids ? "ON\n" : "OFF\n" );
	printf_SDL (Screen, -1, -1, " m. Map of Deck xy\n");
	printf_SDL (Screen, -1, -1, " x. Cheatkeys : %s",
		    GameConfig . enable_cheatkeys ? "ON\n" : "OFF\n");
	printf_SDL (Screen, -1, -1, " w. Print current waypoints\n");
	printf_SDL (Screen, -1, -1, " f. xray_vision_for_tux: %s",
		    xray_vision_for_tux ? "ON\n" : "OFF\n");
	printf_SDL (Screen, -1, -1, " q. RESUME game\n");

	//--------------------
	// Now we show it...
	//
	our_SDL_flip_wrapper ( Screen );

	switch ( getchar_raw ( ) )
	{
	    case 'f':
		xray_vision_for_tux = !xray_vision_for_tux;
		break;

	    case 'a': // armageddon: kills all robots on ship...
		can_continue = 1;
		Armageddon ();
		break;
		
	    case 'l': // robot list of this deck 
		l = 0;  // l is counter for lines of display of enemy output
		for ( i = 0 ; i < MAX_ENEMYS_ON_SHIP ; i ++ )
		{
		    if ( AllEnemys [ i ] . pos . z == CurLevel -> levelnum ) 
		    {
			if (l && !(l%20)) 
			{
			    printf_SDL (Screen, -1, -1, " --- MORE --- \n");
			    our_SDL_flip_wrapper ( Screen );
			    if( getchar_raw () == 'q')
				break;
			}
			if (!(l % 20) )  
			{
			    ClearGraphMem ();
			    printf_SDL (Screen, x0, y0,
					"NR.   ID  X    Y   ENERGY   speedX Status Friendly An-type An-Phase \n");
			    printf_SDL (Screen, -1, -1,
					"---------------------------------------------\n");
			}
			
			l ++;
			if ( ( AllEnemys [ i ] . type >= 0 ) &&
			     ( AllEnemys [ i ] . type <= Number_Of_Droid_Types ) )
			{
			    printf_SDL (Screen, 15, -1,
					"%4d. %s   %3.1f   %3.1f   %4d    %g ", 
					i,
					Druidmap[AllEnemys[i].type].druidname,
					AllEnemys[i].pos.x,
					AllEnemys[i].pos.y,
					(int)AllEnemys[i].energy,
					AllEnemys[i].speed.x );
			}
			else
			{
			    printf_SDL ( Screen , 15 , -1 , "SEVERE ERROR: Type=%d. " , 
					 AllEnemys [ i ] . type ) ;
			}
			if ( AllEnemys[i].Status == MOBILE ) printf_SDL (Screen, -1, -1, "MOB" );
			else if ( AllEnemys[i].Status == INFOUT ) printf_SDL (Screen, -1, -1, "OUT" );
			else printf_SDL (Screen, -1, -1, "ERROR-UNKNOWN" );
			if ( AllEnemys[i].is_friendly ) printf_SDL (Screen, -1, -1, " YES" );
			else printf_SDL (Screen, -1, -1, "  NO" );
			switch ( AllEnemys [ i ] . animation_type ) 
			{
			    case WALK_ANIMATION:
				printf_SDL (Screen, -1, -1, " Walk" );
				break;
			    case ATTACK_ANIMATION:
				printf_SDL (Screen, -1, -1, " Atta" );
				break;
			    case GETHIT_ANIMATION:
				printf_SDL (Screen, -1, -1, " GHit" );
				break;
			    case DEATH_ANIMATION:
				printf_SDL (Screen, -1, -1, " Deth" );
				break;
			    case STAND_ANIMATION:
				printf_SDL (Screen, -1, -1, " Stnd" );
				break;
			    default:
				printf_SDL (Screen, -1, -1, " ERROR!" );
				break;
			}
			printf_SDL (Screen, -1, -1, " %4.1f" , AllEnemys [ i ] . animation_phase );
			printf_SDL (Screen, -1, -1, "\n" );
			
		    } // if (enemy on current level)  
		} // for ( i < MAX_ENEMYS_ON_SHIP ) 
		
		printf_SDL (Screen, 15, -1," --- END --- \n");
		CountNumberOfDroidsOnShip ( );
		printf_SDL (Screen, 15, -1," BTW:  Number_Of_Droids_On_Ship: %d \n" , Number_Of_Droids_On_Ship );
		our_SDL_flip_wrapper ( Screen );
		while ( ( !SpacePressed()) && (!EscapePressed()) );
		while ( SpacePressed() || EscapePressed() );
		break;
		
	    case 'g': // complete robot list of this ship 
		for (i = 0; i < MAX_ENEMYS_ON_SHIP ; i++)
		{
		    if ( AllEnemys[i].type == (-1) ) continue;
		    
		    if (i && !(i%13)) 
		    {
			printf_SDL (Screen, -1, -1, " --- MORE --- ('q' to quit)\n");
			our_SDL_flip_wrapper ( Screen );
			if (getchar_raw () == 'q')
			    break;
		    }
		    if ( !(i % 13) )
		    {
			ClearGraphMem ();
			printf_SDL (Screen, x0, y0, "Nr.  Lev. ID  Energy  Speed.x\n");
			printf_SDL (Screen, -1, -1, "------------------------------\n");
		    }
		    
		    printf_SDL (Screen, -1, -1, "%d  %d  %s  %d  %g\n",
				i, AllEnemys[i].pos.z,
				Druidmap[AllEnemys[i].type].druidname,
				(int)AllEnemys[i].energy,
				AllEnemys[i].speed.x);
		} // for ( i < MAX_ENEMYS_ON_SHIP ) 
		
		printf_SDL (Screen, -1, -1, " --- END ---\n");
		our_SDL_flip_wrapper ( Screen );
		getchar_raw ();
		break;
		
		
	    case 'd': // destroy all robots on this level, very useful
		// for ( i = 0 ; i < Number_Of_Droids_On_Ship ; i ++ )
		for ( i = 0 ; i < MAX_ENEMYS_ON_SHIP ; i ++ )
		{
		    if (AllEnemys[i].pos.z == CurLevel->levelnum)
			AllEnemys[i].energy = -100;
		}
		printf_SDL (Screen, -1, -1, "All robots on this deck killed!\n");
		our_SDL_flip_wrapper ( Screen );
		getchar_raw ();
		break;
		
		
	    case 't': // Teleportation 
		ClearGraphMem ();
		input = GetString ( 40 , 2 , NE_TITLE_PIC_BACKGROUND_CODE , "\nEnter Level, X, Y\n(and please don't forget the commas...)\n> " );
		if ( input == NULL ) break ; // We take into account the possibility of escape being pressed...
		sscanf (input, "%d, %d, %d\n", &LNum, &X, &Y);
		free (input);
		Teleport ( LNum , X , Y , 0 , TRUE , TRUE ) ;
		break;
		
	    case 'r': // change to new robot type 
		ClearGraphMem ();
		input = GetString ( 40 , 2 , NE_TITLE_PIC_BACKGROUND_CODE , "Type number of new robot: ");
		if ( input == NULL ) break ; // We take into account the possibility of escape being pressed...
		for (i = 0; i < Number_Of_Droid_Types ; i++)
		    if (!strcmp (Druidmap[i].druidname, input))
			break;
		
		if ( i == Number_Of_Droid_Types )
		{
		    printf_SDL (Screen, x0, y0+20,
				"Unrecognized robot-type: %s", input);
		    getchar_raw ();
		    ClearGraphMem();
		}
		else
		{
		    Me[0].type = i;
		    Me[0].energy = Me[0].maxenergy;
		    Me[0].health = Me[0].energy;
		    printf_SDL (Screen, x0, y0+20, "You are now a %s. Have fun!\n", input);
		    getchar_raw ();
		}
		free (input);
		break;
		
	    case 'e': // set current energy to arbitrary value
		ClearGraphMem();
		input = GetString ( 40 , 2 , NE_TITLE_PIC_BACKGROUND_CODE , "Enter your new energy: " );
		if ( input == NULL ) break ; // We take into account the possibility of escape being pressed...
		sscanf (input, "%d", &num);
		free (input);
		Me [ 0 ] . energy = (double) num;
		if ( Me [ 0 ] . energy > Me [ 0 ] . health ) Me [ 0 ] . health = Me [ 0 ] . energy;
		break;
		
	    case 'h': // auto-aquire all skills
		for ( i = 0 ; i < NUMBER_OF_SKILLS ; i ++ ) Me [ 0 ] . base_skill_level [ i ] = 1 ;
		break;
		
	    case 'n': // toggle display of all droids 
		show_all_droids = !show_all_droids;
		break;
		
	    case 'm': /* Show deck map in Concept view */
		input = GetString ( 40 , 2 , NE_TITLE_PIC_BACKGROUND_CODE , "Levelnum: " );
		if ( input == NULL ) break ; // We take into account the possibility of escape being pressed...
		sscanf (input, "%d", &LNum);
		free (input);
		ShowDeckMap ( curShip . AllLevels [ LNum ] );
		getchar_raw ();
		break;
		
	    case 'x': 
		GameConfig . enable_cheatkeys = ! GameConfig . enable_cheatkeys;
		break;
		
	    case 'w':  /* print waypoint info of current level */
		WpList = CurLevel->AllWaypoints;
		for (i=0; i<MAXWAYPOINTS && WpList[i].x; i++)
		{
		    if (i && !(i%20))
		    {
			printf_SDL (Screen, -1, -1, " ---- MORE -----\n");
			if (getchar_raw () == 'q')
			    break;
		    }
		    if ( !(i%20) )
		    {
			ClearGraphMem ();
			printf_SDL (Screen, x0, y0, "Nr.   X   Y      C1  C2  C3  C4\n");
			printf_SDL (Screen, -1, -1, "------------------------------------\n");
		    }
		    printf_SDL (Screen, -1, -1, "%2d   %2d  %2d      %2d  %2d  %2d  %2d\n",
				i, WpList[i].x, WpList[i].y,
				WpList[i].connections[0],
				WpList[i].connections[1],
				WpList[i].connections[2],
				WpList[i].connections[3]);
		    
		} /* for (all waypoints) */
		printf_SDL (Screen, -1, -1, " --- END ---\n");
		getchar_raw ();
		break;
		
	    case ' ':
	    case 'q':
		can_continue = 1;
		break;
	} /* switch (getchar_raw()) */
    } /* while (!can_continue) */
    
    ClearGraphMem ();
    our_SDL_flip_wrapper (Screen);
    
    keyboard_update (); /* treat all pending keyboard events */
    /* 
     * when changing windows etc, sometimes a key-release event gets 
     * lost, so we have to make sure that CPressed is no longer set
     * or we stay here for ever...
     */
    // CurrentlyCPressed = FALSE;
    while ( CPressed() );
    
    return;
}; // void Cheatmenu() 

/* ----------------------------------------------------------------------
 * This function lets you select whether you want to play the single player
 * mode or the multi player mode or the credits or the intro again or exit.
 * ---------------------------------------------------------------------- */
void
StartupMenu (void)
{
#define FIRST_MIS_SELECT_ITEM_POS_X (0.0)
#define FIRST_MIS_SELECT_ITEM_POS_Y (BANNER_HEIGHT + FontHeight(Menu_BFont))
    enum
	{ 
	    SINGLE_PLAYER_POSITION=1, 
	    MULTI_PLAYER_POSITION,
	    CREDITS_POSITION,
	    CONTRIBUTE_POSITION,
	    EXIT_FREEDROID_POSITION
	};
    int can_continue = 0 ;
    int MenuPosition = 1 ;
    char* MenuTexts [ 10 ] ;
    
    Me [ 0 ] . status = MENU ;
    
    DebugPrintf ( 1 , "\nvoid StartupMenu ( void ): real function call confirmed. "); 
    
    SDL_SetClipRect( Screen , NULL );
    
    // Prevent distortion of framerate by the delay coming from 
    // the time spent in the menu.
    Activate_Conservative_Frame_Computation ( ) ;
    while (!can_continue)
    {
	SetCurrentFont ( Menu_BFont );
        SwitchBackgroundMusicTo( MENU_BACKGROUND_MUSIC_SOUND );

	MenuTexts[0]= SINGLE_PLAYER_STRING ;
	MenuTexts[1]="Multi Player";
	MenuTexts[2]="Credits";
	MenuTexts[3]="Contribute";
	MenuTexts[4]="Exit Freedroid";
	MenuTexts[5]="";
	
	if ( ! skip_initial_menus )
	    MenuPosition = DoMenuSelection( "" , MenuTexts , -1 , NE_TITLE_PIC_BACKGROUND_CODE , NULL );
	else
	    MenuPosition = SINGLE_PLAYER_POSITION ;
	
	switch (MenuPosition) 
	{
	    case SINGLE_PLAYER_POSITION:
		can_continue = Single_Player_Menu ( );
		break;
	    case MULTI_PLAYER_POSITION:
		can_continue = Multi_Player_Menu();
		break;
	    case CREDITS_POSITION:
		Credits_Menu();
		break;
	    case CONTRIBUTE_POSITION:
		Contribute_Menu();
		break;
	    case (-1):
	    case EXIT_FREEDROID_POSITION:
		Terminate( OK );
		break;
	    default: 
		break;
	} 
	
    }
    
    ClearGraphMem();
    // Since we've faded out the whole scren, it can't hurt
    // to have the top status bar redrawn...
    Me[0].status=MOBILE;
    
    return;
    
}; // void StartupMenu( void );

/* ----------------------------------------------------------------------
 * This function provides a the big escape menu from where you can get 
 * into different submenus.
 * ---------------------------------------------------------------------- */
void
EscapeMenu (void)
{
enum
  { 
    SAVE_GAME_POSITION=1,
    RESUME_GAME_POSITION,
    OPTIONS_POSITION, 
    LEVEL_EDITOR_POSITION, 
    LOAD_GAME_POSITION,
    NEW_GAME_POSITION,
    QUIT_POSITION
  };

  int can_continue = 0;
  int MenuPosition=1;
  char* MenuTexts[10];

  Me [ 0 ] . status = MENU;

  DebugPrintf (2, "\nvoid EscapeMenu(void): real function call confirmed."); 

  //--------------------
  // Prevent distortion of framerate by the delay coming from 
  // the time spend in the menu.
  Activate_Conservative_Frame_Computation();

  //--------------------
  // Escape must be expected to be pressed right now for this menu to
  // to be entered, so we wait until the escape key is released...
  //
  while ( EscapePressed() );

  while (!can_continue)
    {
      MenuTexts[0]="Save Game";
      MenuTexts[1]="Resume Game";
      MenuTexts[2]="Options";
      MenuTexts[3]="Level Editor";
      MenuTexts[4]="Load Game";
      MenuTexts[5]="New Game";
      MenuTexts[6]="Quit";
      MenuTexts[7]="";

      //MenuPosition = DoMenuSelection( "" , MenuTexts , 1 ,  NE_TITLE_PIC_BACKGROUND_CODE , Menu_BFont );
      MenuPosition = DoMenuSelection( "" , MenuTexts , 1 ,  -1 , Menu_BFont );

      switch (MenuPosition) 
	{
	case ( -1 ) :
	case ( RESUME_GAME_POSITION ) :
	  can_continue=!can_continue;
	  break;
	case OPTIONS_POSITION:
	  while (EnterPressed() || SpacePressed() );
	  Options_Menu();
	  // can_continue = TRUE;   /* jp forgot this... ;) */
	  break;
	case LEVEL_EDITOR_POSITION:
	  while (EnterPressed() || SpacePressed() );
	  LevelEditor();
	  can_continue = TRUE;  
	  break;
	case LOAD_GAME_POSITION:
	  LoadGame ( ) ;
	  can_continue = TRUE ;
	  break;
	case NEW_GAME_POSITION:
	  Me [ 0 ] . energy = 100 ;
	  GameOver = TRUE ;
	  can_continue = TRUE;
	  break;
	case SAVE_GAME_POSITION:
	  SaveGame(  );
	  break;
	case QUIT_POSITION:
	  DebugPrintf (2, "\nvoid EscapeMenu( void ): Quit Requested by user.  Terminating...");
	  Terminate(0);
	  break;
	default: 
	  break;
	} 

    }

  ClearGraphMem();
  // Since we've faded out the whole scren, it can't hurt
  // to have the top status bar redrawn...
  Me[0].status=MOBILE;

  return;

}; // void EscapeMenu( void )

/* ----------------------------------------------------------------------
 * This menu allows the user to change the screen resolution of the game.
 * However this will only be available 
 *
 * ---------------------------------------------------------------------- */
void
Change_Screen_Resolution_Menu ( void )
{
    int can_continue = 0;
    int MenuPosition=1;
    char* MenuTexts[10];
    enum
	{ 
	    SET_640_480 = 1 , 
	    SET_800_600, 
	    SET_1024_768,
	    LEAVE_OPTIONS_MENU 
	};
    
    //--------------------
    // Users of SDL output must stick to 640x480 screen resolution.
    //
    if ( ! use_open_gl ) return;

    while ( EscapePressed() );
    
    while (!can_continue)
    {
	MenuTexts[0]="640x480";
	MenuTexts[1]="800x600";
	MenuTexts[2]="1024x768";
	MenuTexts[3]="Back";
	MenuTexts[4]="";
	
	MenuPosition = DoMenuSelection( "Changes will take effect\n       when you next start the game" , 
					MenuTexts , -1 , -1 , NULL );
	
	switch (MenuPosition) 
	{
	    case (-1):
		can_continue=!can_continue;
		break;
		
	    case SET_640_480:
		while ( EnterPressed( ) || SpacePressed( ) );
		GameConfig . next_time_width_of_screen = 640 ;
		GameConfig . next_time_height_of_screen = 480 ;
		GiveMouseAlertWindow ( "\n\
You selected 640x480 pixel.\n\n\
Change of screen resolution will\n\
take effect automatically when you next\n\
start FreedroidRPG.\n\
\n\
Thank you.\n");
		SetCurrentFont ( Menu_BFont );
		break;
		
	    case SET_800_600:
		while ( EnterPressed( ) || SpacePressed( ) );
		GameConfig . next_time_width_of_screen = 800 ;
		GameConfig . next_time_height_of_screen = 600 ;
		GiveMouseAlertWindow ( "\n\
You selected 800x600 pixel.\n\n\
Change of screen resolution will\n\
take effect automatically when you next\n\
start FreedroidRPG.\n\
\n\
Thank you.\n");
		SetCurrentFont ( Menu_BFont );
		break;
		
	    case SET_1024_768:
		while ( EnterPressed( ) || SpacePressed( ) );
		GameConfig . next_time_width_of_screen = 1024 ;
		GameConfig . next_time_height_of_screen = 768 ;
		GiveMouseAlertWindow ( "\n\
You selected 1024x768 pixel.\n\n\
Change of screen resolution will take\n\
effect automatically when you next\n\
start FreedroidRPG.\n\
\n\
Thank you.\n");
		SetCurrentFont ( Menu_BFont );
		break;
		
	    case LEAVE_OPTIONS_MENU:
		while (EnterPressed() || SpacePressed() );
		can_continue=TRUE;
		break;
		
	    default: 
		break;
		
	} 
    }
    
    ClearGraphMem ();
    DisplayBanner ( ) ;
    
}; // void Change_Screen_Resolution_Menu ( void )

/* ----------------------------------------------------------------------
 * This function provides a the options menu.  This menu is a 
 * submenu of the big EscapeMenu.  Here you can change sound vol.,
 * gamma correction, fullscreen mode, display of FPS and such
 * things.
 * ---------------------------------------------------------------------- */
void
Graphics_Options_Menu (void)
{
    int can_continue = 0;
    int MenuPosition=1;
    char Options0[1000];
    char Options1[1000];
    char Options2[1000];
    char Options3[1000];
    char Options4[1000];
    char* MenuTexts[10];
    enum
	{ 
	    SET_GAMMA_CORRECTION = 1 , 
	    SET_FULLSCREEN_FLAG, 
	    CHANGE_SCREEN_RESOLUTION,
	    SET_SHOW_BLOOD_FLAG,
	    SET_AUTOMAP_SCALE,
	    LEAVE_OPTIONS_MENU 
	};
    
    // This is not some Debug Menu but an optically impressive 
    // menu for the player.  Therefore I suggest we just fade out
    // the game screen a little bit.
    
    while ( EscapePressed() );
    
    while (!can_continue)
    {
	sprintf( Options0 , "Gamma Correction: %1.2f", GameConfig.current_gamma_correction );
	sprintf( Options1 , "Fullscreen Mode: %s", fullscreen_on ? "ON" : "OFF");
	sprintf( Options2 , "Change Screen Resolution" );
	sprintf( Options3 , "Show Blood: %s", 
		 GameConfig . show_blood ? "YES" : "NO" );
	sprintf( Options4 , "Automap Scale: %2.1f", 
		 GameConfig . automap_display_scale );
	MenuTexts[0]=Options0;
	MenuTexts[1]=Options1;
	MenuTexts[2]=Options2;
	MenuTexts[3]=Options3;
	MenuTexts[4]=Options4;
	MenuTexts[5]="Back";
	MenuTexts[6]="";
	
	MenuPosition = DoMenuSelection( "" , MenuTexts , -1 , -1 , NULL );
	
	switch (MenuPosition) 
	{
	    
	    case (-1):
		can_continue=!can_continue;
		break;
		
	    case SET_GAMMA_CORRECTION:
		
		if ( RightPressed() ) 
		{
		    while ( RightPressed());
		    GameConfig.current_gamma_correction+=0.05;
		    SDL_SetGamma( GameConfig.current_gamma_correction , GameConfig.current_gamma_correction , GameConfig.current_gamma_correction );
		}
		
		if ( LeftPressed() ) 
		{
		    while (LeftPressed());
		    GameConfig.current_gamma_correction-=0.05;
		    SDL_SetGamma( GameConfig.current_gamma_correction , GameConfig.current_gamma_correction , GameConfig.current_gamma_correction );
		}
		
		break;
		
	    case SET_FULLSCREEN_FLAG:
		while ( EnterPressed( ) || SpacePressed( ) );
		#ifndef __WIN32__
		SDL_WM_ToggleFullScreen ( Screen );
		fullscreen_on = !fullscreen_on;
		#else
		GiveMouseAlertWindow("\nUnfortunately, fullscreen cannot be\ntoggled at runtime under Windows.\nWe apologise for this.\n\n\
There are good Linux distributions out there,\n please check them out.\n\nOr you can launch the game with the -w option.\n\n   Thank you.\n");
		#endif
		break;
		
	    case CHANGE_SCREEN_RESOLUTION:
		while ( EnterPressed() || SpacePressed() );
		if ( ! use_open_gl )
		{
		    GiveMouseAlertWindow ( "\n\
You are using SDL instead of OpenGL\n\
for graphics ouput right now.\n\
\n\
Other screen resolutions than 640x480\n\
are currently not supported with SDL.\n\
\n\
You might want to restart the game using\n\
OpenGL instead.  Then you can change the\n\
screen resolution using this menu option.\n\
\n\
Thank you.\n");
		    SetCurrentFont ( Menu_BFont );
		}
		else
		    Change_Screen_Resolution_Menu();
		break;
		
	    case SET_SHOW_BLOOD_FLAG:
		while (EnterPressed() || SpacePressed() );
		GameConfig . show_blood = !GameConfig . show_blood;
		break;
		
	    case SET_AUTOMAP_SCALE:
		if ( RightPressed() ) 
		{
		    while ( RightPressed() );
		    if ( GameConfig . automap_display_scale < 9.1 )
			GameConfig . automap_display_scale += 1.0 ;
		}
		
		if ( LeftPressed() ) 
		{
		    while (LeftPressed());
		    if ( GameConfig . automap_display_scale >= 1.9 )
		    GameConfig . automap_display_scale -= 1.0 ;
		}
		
		break;
		
	    case LEAVE_OPTIONS_MENU:
		while (EnterPressed() || SpacePressed() );
		can_continue=TRUE;
		break;
		
	    default: 
		break;
		
	} 
    }
    
    ClearGraphMem ();
    DisplayBanner ( ) ;
    
}; // void Graphics_Options_Menu (void)

/* ----------------------------------------------------------------------
 * This function provides a the options menu.  This menu is a 
 * submenu of the big EscapeMenu.  Here you can change sound vol.,
 * gamma correction, fullscreen mode, display of FPS and such
 * things.
 * ---------------------------------------------------------------------- */
void
Sound_Options_Menu (void)
{
    int can_continue = 0;
    int MenuPosition=1;
    char Options0[1000];
    char Options1[1000];
    char Options2[1000];
    char Options3[1000];
    char* MenuTexts[10];
    enum
	{ 
	    SET_BG_MUSIC_VOLUME=1, 
	    SET_SOUND_FX_VOLUME, 
	    SET_TERMINATE_ON_MISSING_FLAG,
	    SET_SHOW_SUBTITLE_FLAG,
	    LEAVE_OPTIONS_MENU 
	};
    
    // This is not some Debug Menu but an optically impressive 
    // menu for the player.  Therefore I suggest we just fade out
    // the game screen a little bit.
    
    while ( EscapePressed() );
    
    while (!can_continue)
    {
	
	sprintf ( Options0 , "Background Music Volume: %1.2f" , GameConfig.Current_BG_Music_Volume );
	sprintf ( Options1 , "Sound Effects Volume: %1.2f", GameConfig.Current_Sound_FX_Volume );
	sprintf( Options2 , "Terminate On Missing Sample: %s", 
		 GameConfig.terminate_on_missing_speech_sample ? "YES" : "NO" );
	sprintf( Options3 , "Show Subtitles in Dialogs: %s", 
		 GameConfig.show_subtitles_in_dialogs ? "YES" : "NO" );
	MenuTexts [ 0 ] = Options0;
	MenuTexts [ 1 ] = Options1;
	MenuTexts [ 2 ] = Options2;
	MenuTexts [ 3 ] = Options3;
	MenuTexts [ 4 ] = "Back";
	MenuTexts [ 5 ] = "";
	
	MenuPosition = DoMenuSelection( "" , MenuTexts , -1 , -1 , NULL );
	
	switch (MenuPosition) 
	{
	    
	    case (-1):
		can_continue=!can_continue;
		break;
		
	    case SET_BG_MUSIC_VOLUME:
		
		if ( RightPressed() ) 
		{
		    while ( RightPressed());
		    if ( GameConfig.Current_BG_Music_Volume < 1 ) GameConfig.Current_BG_Music_Volume += 0.05;
		    SetBGMusicVolume( GameConfig.Current_BG_Music_Volume );
		}
		
		
		if ( LeftPressed() ) 
		{
		    while (LeftPressed());
		    if ( GameConfig.Current_BG_Music_Volume > 0 ) GameConfig.Current_BG_Music_Volume -= 0.05;
		    SetBGMusicVolume( GameConfig.Current_BG_Music_Volume );
		}
		
		break;
		
	    case SET_SOUND_FX_VOLUME:
		
		if ( RightPressed() ) 
		{
		    while ( RightPressed());
		    if ( GameConfig.Current_Sound_FX_Volume < 1 ) GameConfig.Current_Sound_FX_Volume += 0.05;
		    SetSoundFXVolume( GameConfig.Current_Sound_FX_Volume );
		}
		
		if ( LeftPressed() ) 
		{
		    while (LeftPressed());
		    if ( GameConfig.Current_Sound_FX_Volume > 0 ) GameConfig.Current_Sound_FX_Volume -= 0.05;
		    SetSoundFXVolume( GameConfig.Current_Sound_FX_Volume );
		}
		
		break;
		
	    case SET_TERMINATE_ON_MISSING_FLAG:
		while (EnterPressed() || SpacePressed() );
		GameConfig.terminate_on_missing_speech_sample = !GameConfig.terminate_on_missing_speech_sample;
		break;
		
	    case SET_SHOW_SUBTITLE_FLAG:
		while (EnterPressed() || SpacePressed() );
		GameConfig.show_subtitles_in_dialogs = !GameConfig.show_subtitles_in_dialogs;
		break;
		
	    case LEAVE_OPTIONS_MENU:
		while (EnterPressed() || SpacePressed() );
		can_continue=TRUE;
		break;
		
	    default: 
		break;
		
	} 
    }
    
    ClearGraphMem ();
    DisplayBanner ( );
    
}; // void Sound_Options_Menu (void)

/* ----------------------------------------------------------------------
 * This function provides a the options menu.  This menu is a 
 * submenu of the big EscapeMenu.  Here you can change sound vol.,
 * gamma correction, fullscreen mode, display of FPS and such
 * things.
 * ---------------------------------------------------------------------- */
void
PerformanceTweaksOptionsMenu (void)
{
    int can_continue = 0;
    int MenuPosition=1;
    char Options0[1000];
    char Options1[1000];
    char Options2[1000];
    char Options3[1000];
    char Options4[1000];
    char Options5[1000];
    char Options6[1000];
    char* MenuTexts[10];
    enum
	{ 
	    SET_HOG_CPU_FLAG = 1,
	    SET_HIGHLIGHTING_MODE,
	    SET_MENU_HANDLING_MODE,
	    SHOW_QUICK_INVENTORY_MODE,
	    SKIP_LIGHT_RADIUS_MODE,
	    USE_BARS_INSTEAD_OF_ENERGY_O_METER_MODE,
	    SKIP_SHADOWS,
	    LEAVE_PERFORMANCE_TWEAKS_MENU 
	};
    
    // This is not some Debug Menu but an optically impressive 
    // menu for the player.  Therefore I suggest we just fade out
    // the game screen a little bit.
    
    while ( EscapePressed() );
    
    while (!can_continue)
    {
	
	sprintf ( Options0 , "Hog CPU for max. performance: %s", 
		  GameConfig.hog_CPU ? "YES" : "NO" );
	sprintf ( Options1 , "Highlighting mode: %s", GameConfig.highlighting_mode_full ? "FULL" : "REDUCED" );
	sprintf ( Options3 , "Show quick inventory: %s", GameConfig . show_quick_inventory ? "YES" : "NO" );
	sprintf ( Options4 , "Skip light radius: %s", GameConfig . skip_light_radius ? "YES" : "NO" );
	sprintf ( Options5 , "Use bars for energy display: %s", 
		  GameConfig . use_bars_instead_of_energy_o_meter ? "YES" : "NO" );
	sprintf ( Options6 , "Skip shadow blitting: %s", 
		  GameConfig . skip_shadow_blitting ? "YES" : "NO" );
	
	strcpy ( Options2 , "Menu handling: " );
	switch ( GameConfig . menu_mode )
	{
	    case MENU_MODE_FAST:
		strcat ( Options2, "FAST" );
		break;
	    case MENU_MODE_DEFAULT:
		strcat ( Options2, "DEFAULT" );
		break;
	    case MENU_MODE_DOUBLE:
		strcat ( Options2, "DOUBLE" );
		break;
	    default:
		break;
	}
	
	MenuTexts[0]=Options0;
	MenuTexts[1]=Options1;
	MenuTexts[2]=Options2;
	MenuTexts[3]=Options3;
	MenuTexts[4]=Options4;
	MenuTexts[5]=Options5;
	MenuTexts[6]=Options6;
	MenuTexts[7]="Back";
	MenuTexts[8]="";
	
	MenuPosition = DoMenuSelection( "" , MenuTexts , -1 , -1 , NULL );
	
	switch (MenuPosition) 
	{
	    case (-1):
		can_continue=!can_continue;
		break;
		
	    case SET_HOG_CPU_FLAG:
		while (EnterPressed() || SpacePressed() );
		GameConfig . hog_CPU = ! GameConfig . hog_CPU ;
		break;
		
	    case SET_HIGHLIGHTING_MODE:
		while (EnterPressed() || SpacePressed() );
		GameConfig . highlighting_mode_full = ! GameConfig . highlighting_mode_full ;
		break;
		
	    case SET_MENU_HANDLING_MODE:
		while (EnterPressed() || SpacePressed() );
		if ( GameConfig . menu_mode == MENU_MODE_FAST )
		    GameConfig . menu_mode = MENU_MODE_DEFAULT;
		else if ( GameConfig . menu_mode == MENU_MODE_DEFAULT )
		    GameConfig . menu_mode = MENU_MODE_DOUBLE;
		else if ( GameConfig . menu_mode == MENU_MODE_DOUBLE )
		    GameConfig . menu_mode = MENU_MODE_FAST;
		//--------------------
		// In OpenGL, there is no need for the reduced menu display (which is
		// based on restoring a saved "screenshot", which isn't working so well
		// in OpenGL anyway) so we don't even allow for reduced menus in this
		// mode.
		//
		if ( use_open_gl ) GameConfig . menu_mode = MENU_MODE_DEFAULT;
		break;
		
	    case SHOW_QUICK_INVENTORY_MODE:
		while (EnterPressed() || SpacePressed() );
		GameConfig . show_quick_inventory = ! GameConfig . show_quick_inventory ;
		break;
		
	    case SKIP_LIGHT_RADIUS_MODE:
		while (EnterPressed() || SpacePressed() );
		GameConfig . skip_light_radius = ! GameConfig . skip_light_radius ;
		break;
		
	    case USE_BARS_INSTEAD_OF_ENERGY_O_METER_MODE:
		while (EnterPressed() || SpacePressed() );
		GameConfig . use_bars_instead_of_energy_o_meter = ! GameConfig . use_bars_instead_of_energy_o_meter ;
		break;
		
	    case SKIP_SHADOWS:
		while (EnterPressed() || SpacePressed() );
		GameConfig . skip_shadow_blitting = ! GameConfig . skip_shadow_blitting ;
		break;

	    case LEAVE_PERFORMANCE_TWEAKS_MENU:
		while (EnterPressed() || SpacePressed() );
		can_continue=TRUE;
		break;
		
	    default: 
		break;
		
	} 
    }
    
    ClearGraphMem ();
    DisplayBanner ( );
    
}; // void PerformanceTweaksOptionsMenu (void)

/* ----------------------------------------------------------------------
 * This function provides a the options menu.  This menu is a 
 * submenu of the big EscapeMenu.  Here you can change sound vol.,
 * gamma correction, fullscreen mode, display of FPS and such
 * things.
 * ---------------------------------------------------------------------- */
void
On_Screen_Display_Options_Menu (void)
{
  int can_continue = 0;
  int MenuPosition=1;
  char Options0[1000];
  char Options1[1000];
  char Options2[1000];
  char Options3[1000];
  char Options4[1000];
  char Options5[1000];
  char* MenuTexts[10];
  enum
    { 
      SHOW_POSITION=1, 
      SHOW_FRAMERATE, 
      SHOW_TUX_ENERGY,
      SHOW_ENEMY_ENERGY_BARS,
      PARALLEL_BIG_SCREEN_MESSAGES_AT_MOST_POSITION,
      BIG_SCREEN_MESSAGES_DURATION_POSITION,
      LEAVE_OPTIONS_MENU 
    };

  // This is not some Debug Menu but an optically impressive 
  // menu for the player.  Therefore I suggest we just fade out
  // the game screen a little bit.

  while ( EscapePressed() );

  while (!can_continue)
    {

      sprintf( Options0 , "Show Position: %s", GameConfig.Draw_Position ? "ON" : "OFF" );
      sprintf( Options1 , "Show Framerate: %s", GameConfig.Draw_Framerate? "ON" : "OFF" );
      sprintf( Options2 , "Show Tux Energy: %s", GameConfig.Draw_Energy? "ON" : "OFF" );
      sprintf( Options3 , "Show Enemy Energy Bars: %s", GameConfig.enemy_energy_bars_visible? "ON" : "OFF" );
      sprintf( Options4 , "Screen Messages at most: %d", GameConfig.number_of_big_screen_messages );
      sprintf( Options5 , "Screen Message time: %3.1f", GameConfig.delay_for_big_screen_messages );
      MenuTexts[0]=Options0;
      MenuTexts[1]=Options1;
      MenuTexts[2]=Options2;
      MenuTexts[3]=Options3;
      MenuTexts[4]=Options4;
      MenuTexts[5]=Options5;
      MenuTexts[6]="Back";
      MenuTexts[7]="";

      MenuPosition = DoMenuSelection( "" , MenuTexts , -1 , -1 , NULL );

      switch (MenuPosition) 
	{
	case (-1):
	  can_continue=!can_continue;
	  break;
	case SHOW_POSITION:
	  while (EnterPressed() || SpacePressed() );
	  GameConfig.Draw_Position=!GameConfig.Draw_Position;
	  break;
	case SHOW_FRAMERATE:
	  while (EnterPressed() || SpacePressed() );
	  GameConfig.Draw_Framerate=!GameConfig.Draw_Framerate;
	  break;
	case SHOW_TUX_ENERGY:
	  while (EnterPressed() || SpacePressed() );
	  GameConfig.Draw_Energy=!GameConfig.Draw_Energy;
	  break;
	case SHOW_ENEMY_ENERGY_BARS:
	  while (EnterPressed() || SpacePressed() );
	  GameConfig . enemy_energy_bars_visible = ! GameConfig . enemy_energy_bars_visible ;
	  break;

	case PARALLEL_BIG_SCREEN_MESSAGES_AT_MOST_POSITION:
	  if ( LeftPressed() )
	    {
	      while ( LeftPressed() );
	      if ( GameConfig . number_of_big_screen_messages > 0 )
		{
		  GameConfig . number_of_big_screen_messages -- ;
		}
	    }
	  else if ( RightPressed() )
	    {
	      while ( RightPressed() );
	      if ( GameConfig . number_of_big_screen_messages < MAX_BIG_SCREEN_MESSAGES )
		{
		  GameConfig . number_of_big_screen_messages ++ ;
		}
	    }
	  break;

	case BIG_SCREEN_MESSAGES_DURATION_POSITION:
	  if ( LeftPressed() )
	    {
	      while ( LeftPressed() );
	      if ( GameConfig . delay_for_big_screen_messages >= 0.5 )
		{
		  GameConfig . delay_for_big_screen_messages -= 0.5 ;
		}
	    }
	  else if ( RightPressed() )
	    {
	      while ( RightPressed() );
	      GameConfig . delay_for_big_screen_messages += 0.5 ;
	    }
	  break;

	case LEAVE_OPTIONS_MENU:
	  while (EnterPressed() || SpacePressed() );
	  can_continue=TRUE;
	  break;
	default: 
	  break;
	} 
    }

  ClearGraphMem ();
  DisplayBanner ( );

}; // On_Screen_Display_Options_Menu

/* ----------------------------------------------------------------------
 * This function provides a the options menu.  This menu is a 
 * submenu of the big EscapeMenu.  Here you can change sound vol.,
 * gamma correction, fullscreen mode, display of FPS and such
 * things.
 * ---------------------------------------------------------------------- */
void
Droid_Talk_Options_Menu (void)
{

  int can_continue = 0;
  int MenuPosition=1;
  char Options0[1000];
  char Options1[1000];
  char Options2[1000];
  char Options3[1000];
  char Options4[1000];
  char Options5[1000];
  char* MenuTexts[10]={ "" , "" , "" , "" , "" ,
			"" , "" , "" , "" , "" };
  enum
    { 
      INFLU_REFRESH_TEXT=1,
      INFLU_BLAST_TEXT,
      ENEMY_HIT_TEXT,
      ENEMY_BUMP_TEXT,
      ENEMY_AIM_TEXT,
      ALL_TEXTS,
      LEAVE_DROID_TALK_OPTIONS_MENU 
    };

  while (!can_continue)
    {
      sprintf( Options0 , "Influencer Refresh Texts: %s" , GameConfig.Influencer_Refresh_Text ? "ON" : "OFF" );
      sprintf( Options1 , "Influencer Blast Texts: %s", GameConfig.Influencer_Blast_Text ? "ON" : "OFF" );
      sprintf( Options2 , "Enemy Hit Texts: %s", GameConfig.Enemy_Hit_Text ? "ON" : "OFF" );
      sprintf( Options3 , "Enemy Bumped Texts: %s", GameConfig.Enemy_Bump_Text ? "ON" : "OFF" );
      sprintf( Options4 , "Enemy Aim Texts: %s", GameConfig.Enemy_Aim_Text ? "ON" : "OFF" );
      sprintf( Options5 , "All in-game Speech: %s", GameConfig.All_Texts_Switch ? "ON" : "OFF" );
      MenuTexts[0]=Options0;
      MenuTexts[1]=Options1;
      MenuTexts[2]=Options2;
      MenuTexts[3]=Options3;
      MenuTexts[4]=Options4;
      MenuTexts[5]=Options5;
      MenuTexts[6]="Back";

      MenuPosition = DoMenuSelection( "" , MenuTexts , -1 , -1 , NULL );

      switch (MenuPosition) 
	{
	case (-1):
	  can_continue=!can_continue;
	  break;
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
	  can_continue=TRUE;
	  break;
	default: 
	  break;
	} 
    }

  ClearGraphMem ();
  DisplayBanner ( );

}; // Droid_Talk_Options_Menu

/* ----------------------------------------------------------------------
 * This function provides a the options menu.  This menu is a 
 * submenu of the big EscapeMenu.  Here you can change sound vol.,
 * gamma correction, fullscreen mode, display of FPS and such
 * things.
 * ---------------------------------------------------------------------- */
void
Options_Menu (void)
{
    int can_continue = 0;
    int MenuPosition=1;
    char* MenuTexts[10];
    enum
	{ 
	    GRAPHICS_OPTIONS=1, 
	    SOUND_OPTIONS,
	    DROID_TALK_OPTIONS,
	    ON_SCREEN_DISPLAYS,
	    PERFORMANCE_TWEAKS_OPTIONS,
	    SAVE_OPTIONS, 
	    LEAVE_OPTIONS_MENU 
	};
    
    MenuTexts[0]="Graphics Options";
    MenuTexts[1]="Sound Options";
    MenuTexts[2]="Droid Talk";
    MenuTexts[3]="On-Screen Displays";
    MenuTexts[4]="Performance Tweaks";
    MenuTexts[5]="Save Options";
    MenuTexts[6]="Back";
    MenuTexts[7]="";
    
    while ( !can_continue )
    {
	
	MenuPosition = DoMenuSelection( "" , MenuTexts , 1 , -1 , Menu_BFont );
	
	switch (MenuPosition) 
	{
	    case (-1):
		can_continue=!can_continue;
		break;
	    case GRAPHICS_OPTIONS:
		while (EnterPressed() || SpacePressed() );
		Graphics_Options_Menu();
		break;
	    case SOUND_OPTIONS:
		while (EnterPressed() || SpacePressed() );
		Sound_Options_Menu();
		break;
	    case DROID_TALK_OPTIONS:
		while (EnterPressed() || SpacePressed() );
		Droid_Talk_Options_Menu();
		break;
	    case ON_SCREEN_DISPLAYS:
		while (EnterPressed() || SpacePressed() );
		On_Screen_Display_Options_Menu();
		break;
	    case PERFORMANCE_TWEAKS_OPTIONS:
		while (EnterPressed() || SpacePressed() );
		PerformanceTweaksOptionsMenu();
		break;
	    case SAVE_OPTIONS:
		while (EnterPressed() || SpacePressed() );
		break;
	    case LEAVE_OPTIONS_MENU:
		while (EnterPressed() || SpacePressed() );
		can_continue=TRUE;
		break;
	    default: 
		break;
	} 
    }
    
    ClearGraphMem ();
    DisplayBanner ( );
    
} // Options_Menu

/* ----------------------------------------------------------------------
 * This reads in the new name for the character...
 * ---------------------------------------------------------------------- */
void
Get_Server_Name ( void )
{
  char* Temp;
  InitiateMenu(  NE_TITLE_PIC_BACKGROUND_CODE );

  Temp = GetString( 140 , FALSE  , NE_TITLE_PIC_BACKGROUND_CODE , "\n\
 Please enter name of server to connect to:\n\
 You can give an empty string for the local host.\n\
 > " );
  
  if ( Temp == NULL )
    {
      strcpy ( ServerName , "NoSeverNameGiven" );
    }
  else
    {
      strcpy ( ServerName , Temp );
      free( Temp );
    }

}; // void Get_Server_Name ( void )

/* ----------------------------------------------------------------------
 * This reads in the new name for the character...
 * ---------------------------------------------------------------------- */
void
Get_New_Character_Name ( void )
{
    char* Temp;
    InitiateMenu( NE_TITLE_PIC_BACKGROUND_CODE );

    if ( ! skip_initial_menus )
	Temp = GetString ( 12 , FALSE  , NE_TITLE_PIC_BACKGROUND_CODE , "\n\
     Please enter a name\n\
     for the new hero: \n\
     (ESCAPE to cancel.)\n\n\
     > " );
    else
	Temp = "MapEd" ;

    //--------------------
    // In case 'Escape has been pressed inside GetString, then a NULL pointer
    // will be returned, which means no name has been given to the character
    // yet, so we set an empty string for the character name.
    //
    if ( Temp == NULL )
    {
	//--------------------
	// WARNING:  We should not use "" here, since that is also used inside
	//           the menu code as the termination string for the list of menu
	//           options.  That would cause problems in the load and display
	//           directory content later.
	//           Therefore we supply some default name if empty
	//           string was received...  (some more decent workaround for the
	//           problem might be written some time later...)
	//
	strcpy ( Me [ 0 ] . character_name , "HaveNoName" );
	return ;
    }
    
    //--------------------
    // If a real name has been given to the character, we can copy that name into
    // the corresponding structure and return here (not without freeing the string
    // received.  Could be some valuable 20 bytes after all :)
    //
    // Parse Temp for illegal chars
    unsigned int i;
    for ( i = 0 ; i < strlen( Temp ) ; i ++)
	if( ! isalnum ( Temp[i] ) && Temp [ i ] != '-')
		Temp [ i ] = '-';
    strcpy ( Me [ 0 ] . character_name , Temp );
    free( Temp );
    
}; // void Get_New_Character_Name ( void )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
clear_player_inventory_and_stats ( void )
{
    int i;

    //--------------------
    // At first we clear the inventory of the new character
    // of any items (or numeric waste) that might be in there
    //
    for ( i = 0 ; i < MAX_ITEMS_IN_INVENTORY ; i ++ )
    {
	Me [ 0 ] . Inventory [ i ] . type = (-1);
	Me [ 0 ] . Inventory [ i ] . prefix_code = (-1);
	Me [ 0 ] . Inventory [ i ] . suffix_code = (-1);
	Me [ 0 ] . Inventory [ i ] . currently_held_in_hand = FALSE;
    }
    DebugPrintf ( 1 , "\n%s(): Inventory has been emptied..." , __FUNCTION__ );

    //--------------------
    // Now we add some safety, against 'none present' items
    //
    Me [ 0 ] . weapon_item.type = ( -1 ) ;
    Me [ 0 ] . drive_item.type = ( -1 ) ;
    Me [ 0 ] . armour_item.type = ( -1 ) ;
    Me [ 0 ] . shield_item.type = ( -1 ) ;
    Me [ 0 ] . aux1_item.type = ( -1 ) ;
    Me [ 0 ] . aux2_item.type = ( -1 ) ;
    Me [ 0 ] . special_item.type = ( -1 ) ;
    
    Me [ 0 ] . weapon_item.prefix_code = ( -1 ) ;
    Me [ 0 ] . drive_item.prefix_code = ( -1 ) ;
    Me [ 0 ] . armour_item.prefix_code = ( -1 ) ;
    Me [ 0 ] . shield_item.prefix_code = ( -1 ) ;
    Me [ 0 ] . aux1_item.prefix_code = ( -1 ) ;
    Me [ 0 ] . aux2_item.prefix_code = ( -1 ) ;
    Me [ 0 ] . special_item.prefix_code = ( -1 ) ;
    
    Me [ 0 ] . weapon_item.suffix_code = ( -1 ) ;
    Me [ 0 ] . drive_item.suffix_code = ( -1 ) ;
    Me [ 0 ] . armour_item.suffix_code = ( -1 ) ;
    Me [ 0 ] . shield_item.suffix_code = ( -1 ) ;
    Me [ 0 ] . aux1_item.suffix_code = ( -1 ) ;
    Me [ 0 ] . aux2_item.suffix_code = ( -1 ) ;
    Me [ 0 ] . special_item.suffix_code = ( -1 ) ;
    
    Me [ 0 ] . weapon_item.currently_held_in_hand = ( -1 ) ;
    Me [ 0 ] . drive_item.currently_held_in_hand = ( -1 ) ;
    Me [ 0 ] . armour_item.currently_held_in_hand = ( -1 ) ;
    Me [ 0 ] . shield_item.currently_held_in_hand = ( -1 ) ;
    Me [ 0 ] . aux1_item.currently_held_in_hand = ( -1 ) ;
    Me [ 0 ] . aux2_item.currently_held_in_hand = ( -1 ) ;
    Me [ 0 ] . special_item.currently_held_in_hand = ( -1 ) ;
    
    Me [ 0 ] . character_class = WAR_BOT ;
    Me [ 0 ] . base_vitality = 25 ;
    Me [ 0 ] . base_strength = 10 ;
    Me [ 0 ] . base_dexterity = 15 ;
    Me [ 0 ] . base_magic = 25 ;
    Me [ 0 ] . exp_level = 1 ;

}; // void clear_player_inventory_and_stats ( void )

/* ----------------------------------------------------------------------
 * This function prepares a new hero for adventure...
 * ---------------------------------------------------------------------- */
int
PrepareNewHero (void)
{

    clear_player_inventory_and_stats ( ) ;

    UpdateAllCharacterStats ( 0 ) ;

    Get_New_Character_Name( );
    
    Me [ 0 ] . is_town_guard_member = FALSE ;

    //--------------------
    // If the special string "HaveNoName" is being supplied, then
    // we treat this as no name given and will return false.
    //
    if ( ! strcmp ( Me [ 0 ] . character_name , "HaveNoName" ) ) return ( FALSE ) ;

    //--------------------
    // If a real name has been given, then we can proceed and start the
    // game.  If no real name has been given or 'Escape' has been pressed,
    // then the calling function will return to the menu and do nothing
    // else.
    //
    if ( strlen ( Me [ 0 ] . character_name ) > 0 ) return ( TRUE ); else return ( FALSE );
    
}; // int PrepareNewHero (void)

/* ----------------------------------------------------------------------
 * This function does the selection of the hero class...
 * ---------------------------------------------------------------------- */
int
Connect_To_Existing_Server_Menu (void)
{
    //--------------------
    // Now we add some safety, against 'none present' items
    //
    Me [ 0 ] . weapon_item.type = ( -1 ) ;
    Me [ 0 ] . drive_item.type = ( -1 ) ;
    Me [ 0 ] . armour_item.type = ( -1 ) ;
    Me [ 0 ] . shield_item.type = ( -1 ) ;
    Me [ 0 ] . aux1_item.type = ( -1 ) ;
    Me [ 0 ] . aux2_item.type = ( -1 ) ;
    Me [ 0 ] . special_item.type = ( -1 ) ;

    Me[0].weapon_item.prefix_code = ( -1 ) ;
    Me[0].drive_item.prefix_code = ( -1 ) ;
    Me[0].armour_item.prefix_code = ( -1 ) ;
    Me[0].shield_item.prefix_code = ( -1 ) ;
    Me[0].aux1_item.prefix_code = ( -1 ) ;
    Me[0].aux2_item.prefix_code = ( -1 ) ;
    Me[0].special_item.prefix_code = ( -1 ) ;
    
    Me[0].weapon_item.suffix_code = ( -1 ) ;
    Me[0].drive_item.suffix_code = ( -1 ) ;
    Me[0].armour_item.suffix_code = ( -1 ) ;
    Me[0].shield_item.suffix_code = ( -1 ) ;
    Me[0].aux1_item.suffix_code = ( -1 ) ;
    Me[0].aux2_item.suffix_code = ( -1 ) ;
    Me[0].special_item.suffix_code = ( -1 ) ;
    
    Me[0].character_class = WAR_BOT;
    Me[0].base_vitality = 25;
    Me[0].base_strength = 30;
    Me[0].base_dexterity = 25;
    Me[0].base_magic = 10;
    
    // Me[0].weapon_item.type = ITEM_SHORT_SWORD;
    Me[0].drive_item.type = ( -1 ) ;
    
    Me[0].Inventory[ 0 ].type = ITEM_SHORT_SWORD;
    Me[0].Inventory[ 0 ].inventory_position.x = 0;
    Me[0].Inventory[ 0 ].inventory_position.y = 0;
    Me[0].Inventory[ 1 ].type = ITEM_BUCKLER;
    Me[0].Inventory[ 1 ].inventory_position.x = 2;
    Me[0].Inventory[ 1 ].inventory_position.y = 0;
    Me[0].Inventory[ 2 ].type = ITEM_SMALL_HEALTH_POTION;
    Me[0].Inventory[ 2 ].inventory_position.x = 0;
    Me[0].Inventory[ 2 ].inventory_position.y = INVENTORY_GRID_HEIGHT-1;
    Me[0].Inventory[ 3 ].type = ITEM_SMALL_HEALTH_POTION;
    Me[0].Inventory[ 3 ].inventory_position.x = 1;
    Me[0].Inventory[ 3 ].inventory_position.y = INVENTORY_GRID_HEIGHT-1;
    FillInItemProperties ( & Me[0].Inventory[ 0 ] , TRUE , 0 );
    FillInItemProperties ( & Me[0].Inventory[ 1 ] , TRUE , 0 );
    FillInItemProperties ( & Me[0].Inventory[ 2 ] , TRUE , 0 );
    FillInItemProperties ( & Me[0].Inventory[ 3 ] , TRUE , 0 );
    
    Get_Server_Name ( );
    Get_New_Character_Name( );
    
    ConnectToFreedroidServer (  );
    
    if ( strlen ( ServerName ) > 0 )
	return ( TRUE );
    else
	return ( FALSE );
    
}; // int Connect_To_Existing_Server_Menu ( void );

/* ----------------------------------------------------------------------
 * The GNU C Library is SOOOO COOOL!!! It contains functions for directory
 * manipulations that are SOOOO powerful, it's really awesome.  One of
 * these very powerful functions can be used to filter directory entries,
 * so that only a certain kind of files will be displayed any more.  How
 * convenient.  So as a parameter to this powerful function (scandir), you
 * have to specify a sorting function of a certain kind.  And this is just
 * the sorting function that seems appropriate for our little program.
 * ---------------------------------------------------------------------- */
int
filename_filter_func ( const struct dirent *unused )
{
    if ( strstr ( unused->d_name , "savegame" ) != NULL )
    {
	return ( 1 ) ;
    }
    else
    {
	return ( 0 ) ;
    }
    
    // to make compilers happy...
    return ( 0 );
    
}; // static int filename_filter_func (const struct dirent *unused)

/* ----------------------------------------------------------------------
 * This is the function available from the freedroid startup menu, that
 * should display the available characters in the users home directory
 * and eventually let the player select one of his old characters there.
 * ---------------------------------------------------------------------- */
int 
Load_Existing_Hero_Menu ( void )
{
    char Saved_Games_Dir[1000];
    char* MenuTexts[ MAX_SAVED_CHARACTERS_ON_DISK + 1 ] ;
    struct dirent **eps;
    int n;  
    int cnt;
    int MenuPosition;
    
    DebugPrintf ( 1 , "\nint Load_Existing_Hero_Menu ( void ): real function call confirmed.");
    InitiateMenu( NE_TITLE_PIC_BACKGROUND_CODE );
    
    //--------------------
    // We use empty strings to denote the end of any menu selection, 
    // therefore also for the end of the list of saved characters.  So
    // We prepare such a list now by initializing the list with empty
    // strings.  Memory leak at this point can be neglected.  Really.
    //
    for ( n = 0 ; n < MAX_SAVED_CHARACTERS_ON_DISK + 1 ; n ++ )
    {
	MenuTexts [ n ] = malloc ( strlen ( "" ) + 1 ) ;
	strcpy ( MenuTexts [ n ] , "" );
    }

    //--------------------
    // First we must find the home directory of the user.  From there on
    // we can then construct the full directory path of the saved games directory.
    //
    
#if __WIN32__
    our_homedir = ".";
#else
    // first we need the user's homedir for loading/saving stuff
    if ( (our_homedir = getenv("HOME")) == NULL )
    {
	DebugPrintf ( 0 , "ERROR: Environment does not contain HOME variable... \n\
I need to know that for saving. Abort.\n");
	return ( ERR );
    }
#endif
    
    //--------------------
    // Now we generate the right directory for loading from the home
    // directory.
    //
    sprintf ( Saved_Games_Dir , "%s/.freedroid_rpg" , our_homedir );
    // DisplayText ( "This is the record of all your characters:\n\n" , 50 , 50 , NULL );
    
    //--------------------
    // This is a slightly modified copy of the code sniplet from the
    // GNU C Library description on directory operations...
    //
    // the scandir here will give a list of ONLY THOSE FILES WITH .savegame
    // EXTENTION, see GNU C Library docu for the formal details...
    //
    n = scandir ( Saved_Games_Dir , &eps, filename_filter_func , alphasort);
    if (n > 0)
    {
	for (cnt = 0; cnt < n; ++cnt) 
	{
	    // puts ( eps[cnt]->d_name );
	    DisplayText ( eps[cnt]->d_name , 50 , 150 + cnt * 40 , NULL , TEXT_STRETCH );
	    if ( cnt < MAX_SAVED_CHARACTERS_ON_DISK ) 
	    {
		MenuTexts[ cnt ] = ReadAndMallocStringFromData ( eps[cnt]->d_name , "" , ".savegame" ) ;
		//DebugPrintf ( 1 , "\nNOTE:  int Load_Existing_Hero_Menu(void):  Another load game name found: %s.\n" , MenuTexts [ cnt ] );
	    }
	}
	
	MenuPosition = DoMenuSelection( LOAD_EXISTING_HERO_STRING , MenuTexts , 1 , NE_TITLE_PIC_BACKGROUND_CODE , NULL );
	
	if ( MenuPosition == (-1) ) return ( FALSE );
	else
	{
	    // LoadShip ( find_file ( "Asteroid.maps" , MAP_DIR, FALSE) ) ;
	    // PrepareStartOfNewCharacter ( NEW_MISSION );
	    strcpy ( Me [ 0 ] . character_name , MenuTexts [ MenuPosition -1 ] );
	    if ( LoadGame ( ) == OK )
	    {
		GetEventsAndEventTriggers ( "EventsAndEventTriggers" );
		GetQuestList ( "QuestList" );
   		Item_Held_In_Hand = ( -1 );
		return ( TRUE );
	    }
	}
    }
    else
    {
	fprintf ( stderr, "\n\nSaved_Games_Dir: '%s'.\n" , Saved_Games_Dir );
	GiveStandardErrorMessage ( __FUNCTION__  , "\
Either it couldn't open the directory for the saved games\n\
which should be %s,\n\
or there were no saved games present in this directory.\n\
Freedroid will continue execution now, since this problem\n\
 will be dealt with in-game.",
				   NO_NEED_TO_INFORM, IS_WARNING_ONLY );
	
	MenuTexts[0]="BACK";
	MenuTexts[1]="";
	
	while ( SpacePressed() || EnterPressed() );
	DoMenuSelection ( "\n\nNo saved games found!!  Loading Cancelled. " , MenuTexts , 1 , NE_TITLE_PIC_BACKGROUND_CODE , NULL );
	
	//--------------------
	// Now we got to return the problem to the calling function...
	//
	return ( FALSE );
    }
    
    our_SDL_flip_wrapper( Screen );

    return ( OK );
}; // int Load_Existing_Hero_Menu ( void )


/* ----------------------------------------------------------------------
 * This is the function available from the freedroid startup menu, that
 * should display the available characters in the users home directory
 * and eventually let the player select one of his old characters there.
 * ---------------------------------------------------------------------- */
int 
Delete_Existing_Hero_Menu ( void )
{
    char Saved_Games_Dir[1000];
    char* MenuTexts[ MAX_SAVED_CHARACTERS_ON_DISK + 1 ] ;
    struct dirent **eps;
    int n;  
    int cnt;
    int MenuPosition;
    int FinalDecision;
    char SafetyText[2000];
    
    DebugPrintf ( 0 , "\nint Delete_Existing_Hero_Menu ( void ): real function call confirmed.");
    InitiateMenu( NE_TITLE_PIC_BACKGROUND_CODE );
    
    //--------------------
    // We use empty strings to denote the end of any menu selection, 
    // therefore also for the end of the list of saved characters.  So
    // We prepare such a list now by initializing the list with empty
    // strings.  Memory leak at this point can be neglected.  Really.
    //
    for ( n = 0 ; n < MAX_SAVED_CHARACTERS_ON_DISK + 1 ; n ++ )
    {
	MenuTexts [ n ] = malloc ( strlen ( "" ) + 1 ) ;
	strcpy ( MenuTexts [ n ] , "" );
    }

#if __WIN32__
    our_homedir = ".";
#else
    // first we need the user's homedir for loading/saving stuff
    if ( (our_homedir = getenv("HOME")) == NULL )
    {
	DebugPrintf ( 0 , "ERROR: Environment does not contain HOME variable... \n\
I need to know that for saving. Abort.\n");
	return ( ERR );
    }
#endif
    
    //--------------------
    // Now we generate the right directory for saving from the home
    // directory.
    //
    sprintf ( Saved_Games_Dir , "%s/.freedroid_rpg" , our_homedir );
    
    
    // DisplayText ( "This is the record of all your characters:\n\n" , 50 , 50 , NULL );
    
    //--------------------
    // This is a slightly modified copy of the code sniplet from the
    // GNU C Library description on directory operations...
    //
    n = scandir ( Saved_Games_Dir , &eps, filename_filter_func , alphasort);
    if (n > 0)
    {
	for (cnt = 0; cnt < n; ++cnt) 
	{
	    puts ( eps[cnt]->d_name );
	    DisplayText ( eps[cnt]->d_name , 50 , 150 + cnt * 40 , NULL , TEXT_STRETCH );
	    if ( cnt < MAX_SAVED_CHARACTERS_ON_DISK ) 
	    {
		MenuTexts[ cnt ] = ReadAndMallocStringFromData ( eps[cnt]->d_name , "" , ".savegame" ) ;
		DebugPrintf ( -1 , "\nAnother delete game name found: %s.\n" , MenuTexts [ cnt ] );
	    }
	}
	
	MenuPosition = DoMenuSelection( DELETE_EXISTING_HERO_STRING , MenuTexts , 1 , NE_TITLE_PIC_BACKGROUND_CODE , NULL );
	
	if ( MenuPosition == (-1) ) return ( FALSE );
	else
	{
	    //--------------------
	    // We store the character name, just for the case of eventual deletion later!
	    //
	    strcpy( Me[0].character_name , MenuTexts[ MenuPosition -1 ] );
	    
	    //--------------------
	    // We do a final safety check to ask for confirmation.
	    //
	    MenuTexts [ 0 ] = "Sure!" ;
	    MenuTexts [ 1 ] = "BACK" ;
	    MenuTexts [ 2 ] = "";
	    sprintf( SafetyText , "Really delete hero '%s'?" , Me[0].character_name ) ;
	    FinalDecision = DoMenuSelection( SafetyText , MenuTexts , 1 , NE_TITLE_PIC_BACKGROUND_CODE , NULL );
	    
	    if ( FinalDecision == 1 )
	    {
		DeleteGame( );
	    }
	    return ( TRUE );
	}
    }
    else
    {
	fprintf ( stderr, "\n\nSaved_Games_Dir: '%s'.\n" , Saved_Games_Dir );
	GiveStandardErrorMessage ( __FUNCTION__  , "\
Either it couldn't open the directory for the saved games\n\
which should be %s,\n\
or there were no saved games present in this directory.\n\
Freedroid will continue execution now, since this problem\n\
 will be dealt with in-game.",
				   NO_NEED_TO_INFORM, IS_WARNING_ONLY );
	
	MenuTexts[0]="BACK";
	MenuTexts[1]="";
	
	while ( SpacePressed() || EnterPressed() );
	DoMenuSelection ( "\n\nNo saved games found!!  Deletion Cancelled. " , MenuTexts , 1 , NE_TITLE_PIC_BACKGROUND_CODE , NULL );
	
	//--------------------
	// Now we got to return the problem to the calling function...
	//
	return ( FALSE );
    }
    
    our_SDL_flip_wrapper( Screen );
    
    return ( OK );
}; // int Delete_Existing_Hero_Menu ( void )


/* ----------------------------------------------------------------------
 * This function provides the single player menu.  It offers to start a
 * new hero, to load an old one and to go back.
 *
 * The return value indicates, whether the calling function (StartupMenu)
 * can really enter the new game after this function (cause a new player
 * has been set up properly) or not, cause no player is specified yet and
 * nothing is known.
 *
 * ---------------------------------------------------------------------- */
int
Single_Player_Menu (void)
{
    int can_continue = 0;
    int MenuPosition=1;
    char* MenuTexts[10];
    
    enum
	{ 
	    NEW_HERO_POSITION=1, 
	    LOAD_EXISTING_HERO_POSITION, 
	    DELETE_EXISTING_HERO_POSITION,
	    BACK_POSITION
	};
    
    MenuTexts[0]="New Hero";
    MenuTexts[1]="Load existing Hero";
    MenuTexts[2]="Delete existing Hero";
    MenuTexts[3]="Back";
    MenuTexts[4]="";
    
    while (!can_continue)
    {
	
	if ( ! skip_initial_menus )
	    MenuPosition = DoMenuSelection( "" , MenuTexts , 1 , NE_TITLE_PIC_BACKGROUND_CODE , Menu_BFont );
	else
	    MenuPosition = NEW_HERO_POSITION ;
	
	switch (MenuPosition) 
	{
	    case NEW_HERO_POSITION:
		while ( EnterPressed() || SpacePressed() ) ;
		
		if ( PrepareNewHero ( ) == TRUE )
		{
		    LoadShip ( find_file ( "Asteroid.maps" , MAP_DIR, FALSE) ) ;
		    PrepareStartOfNewCharacter ( ) ;
		    can_continue=TRUE;
		    load_game_command_came_from_inside_running_game = TRUE ;
		    return ( TRUE );
		}
		else
		{
		    can_continue=FALSE;
		}
		break;
		
	    case LOAD_EXISTING_HERO_POSITION: 
		while ( EnterPressed() || SpacePressed() ) ;
		
		if ( Load_Existing_Hero_Menu ( ) == TRUE )
		{
		    can_continue = TRUE;
		    return ( TRUE );
		}
		else
		{
		    can_continue = FALSE;
		    // return ( FALSE );
		}
		break;
		
	    case DELETE_EXISTING_HERO_POSITION: 
		while (EnterPressed() || SpacePressed() ) ;
		Delete_Existing_Hero_Menu ( ) ;
		can_continue= FALSE ;
		// return ( FALSE );
		break;
		
	    case (-1):
	    case BACK_POSITION:
		while (EnterPressed() || SpacePressed() || EscapePressed() ) ;
		can_continue=!can_continue;
		return ( FALSE );
		break;
	    default: 
		break;
	}
    }
    return ( TRUE );
}; // void Single_Player_Menu ( void );


/* ----------------------------------------------------------------------
 * This function provides the multi player menu.  It is a submenu
 * of the big EscapeMenu.  Instead of connecting to a server or 
 * something it simply displayes the nonchalant message, that 
 * nothing is implemented yet, but sooner or later it will be.
 * ---------------------------------------------------------------------- */
int
Multi_Player_Menu (void)
{
    int can_continue = 0;
    int MenuPosition=1;
    char* MenuTexts[10];
    
    enum
	{ 
	    START_AS_SERVER_POSITION=1, 
	    JOIN_EXISTING_MULTIPLAYER_POSITION, 
	    LIST_KNOWN_SERVERS,
	    BACK_POSITION
	};
    
    MenuTexts[0]="Start as a Server";
    MenuTexts[1]="Join existing Multiplayer game";
    MenuTexts[2]="List known Servers";
    MenuTexts[3]="Back";
    MenuTexts[4]="";
    
    GiveMouseAlertWindow ( "\nW A R N I N G !\n\nMultiplayer and network play is still very much experimental code.\nCurrently we are not continuing the networking code, simply because our team does not have enough manpower for this additional task and because it slows down development of new features of the single player game a lot." ) ;
    SetCurrentFont ( Menu_BFont );
    
    while (!can_continue)
    {
	MenuPosition = DoMenuSelection( "" , MenuTexts , 1 , NE_TITLE_PIC_BACKGROUND_CODE , NULL );
	
	switch (MenuPosition) 
	{
	    case START_AS_SERVER_POSITION:
		while (EnterPressed() || SpacePressed() ) ;
		LoadShip ( find_file ( "Asteroid.maps" , MAP_DIR , FALSE ) ) ;
		PrepareStartOfNewCharacter (  );	
		ServerMode = TRUE ;
		OpenTheServerSocket (  );
		can_continue=TRUE;
		return ( TRUE );
		break;
		
	case JOIN_EXISTING_MULTIPLAYER_POSITION: 
	    while (EnterPressed() || SpacePressed() ) ;
	    
	    if ( Connect_To_Existing_Server_Menu ( ) == TRUE )
	    {
		can_continue = TRUE;
		LoadShip ( find_file ( "Asteroid.maps" , MAP_DIR, FALSE) ) ;
		PrepareStartOfNewCharacter (  );
		ClientMode = TRUE;
		return ( TRUE );
	    }
	    else
	    {
		can_continue = FALSE;
		// return ( FALSE );
	    }
	    
	    break;
	    case LIST_KNOWN_SERVERS: 
		while (EnterPressed() || SpacePressed() ) ;
		
		if ( Load_Existing_Hero_Menu ( ) == TRUE )
		{
		    can_continue = TRUE;
		    return ( TRUE );
		}
		else
		{
		    can_continue = FALSE;
		    // return ( FALSE );
		}
		
		break;
	    case BACK_POSITION:
	    case (-1):
	    default: 
		while (EnterPressed() || SpacePressed() ) ;
		can_continue=!can_continue;
		return ( FALSE );
		break;
	}
    }
    return ( TRUE );
    
}; // int Multi_Player_Menu ( void ) 

/* ----------------------------------------------------------------------
 * This function provides the credits screen.  It is a submenu of
 * the big EscapeMenu.  Here you can see who helped developing the
 * game.
 * ---------------------------------------------------------------------- */
void
Credits_Menu (void)
{
    char* CreditsText = "\n\n\n\
                                            CREDITS\n\n\n\
   PROGRAMMING:\n\n\
                                      Johannes Prix\n\n\
                                      Reinhard Prix\n\n\
                                      Arthur Huillet\n\n\n\
   ARTWORK:\n\n\
                                      Bastian Salmela\n\n\n\
   DIALOGUES AND MINOR TWEAKS:\n\n\
                                      Karol Swietlicki\n\n\n\
   PROFILING AND TESTING:\n\n\
                                      Clint Herron\n\n\n\
   SOUND EFFECTS:\n\n\
                                      Johannes Prix\n\n\
                                      Chris Hoeppner\n\n\n\
   VOICES:\n\n\
                                      Tiina Heinonen\n\n\
                                      Doris Stubenrauch\n\n\
                                      Andrew A. Gill\n\n\
                                      Johannes Prix\n\n\
                                      Mbrola\n\
                                       text-to-speech-system\n\n\n\
   OGG COMPOSERS:\n\n\
                                      \"The Beginning\"\n\
                                        by 4t thieves\n\
                                        of kahvi collective\n\
                                      (www.kahvi.org)\n\n\
                                      \"Daybreak\"\n\
                                        by realsmokers\n\
                                        of kahvi collective\n\
                                      (www.realsmokers.de)\n\
                                      (www.kahvi.org)\n\n\
                                      \"Bleostrada\"\n\
                                        by stud\n\
                                        of kahvi collective\n\
                                      (www.atl3.com/stud)\n\
                                      (www.kahvi.org)\n\n\n\
   VARIOUS OLDER CONTRIBUTIONS (PRE-0.9.13 people):\n\n\
                                      Ryan 'simcop2387' Voots\n\n\
                                      Andrew A. Gill\n\n\
                                      Zombie Ryushu\n\n\
                                      Ted Cipicchio\n\n\
                                      The Doctor\n\n\
                                      Simon Newton\n\n\n\n\n";
    
    User_Rect . x = Full_Screen_Rect . x ;
    User_Rect . y = Full_Screen_Rect . y ;
    User_Rect . w = Full_Screen_Rect . w ;
    User_Rect . h = Full_Screen_Rect . h ;
    
    while( SpacePressed() || EscapePressed() ) ; /* wait for key release */
    
    // InitiateMenu();
    
    SwitchBackgroundMusicTo ( CREDITS_BACKGROUND_MUSIC_SOUND );
    
    ScrollText ( CreditsText , SCROLLSTARTX, SCROLLSTARTY, User_Rect.y , NE_CREDITS_PIC_BACKGROUND_CODE );
    
    while( SpacePressed() || EscapePressed() ) ; /* wait for key release */
    
}; // void Credits_Menu(void)

/* ----------------------------------------------------------------------
 * This function provides the contribution screen.  It is a submenu of
 * the big EscapeMenu.  Here you can see who can help developing the
 * game into an even better game :)
 * ---------------------------------------------------------------------- */
void
Contribute_Menu (void)
{
    char* ContributeText = "\n\n\
                       HOW TO CONTRIUBTE\n\n\n\
FreedroidRPG is entirely free software AND free artwork.\n\
It is developed exclusively by volunteers in their leisure time.\n\
\n\
FreedroidRPG depends on these contributions to become a better game.  \
If you would like to contribute something as well, \
but are not completely sure how, here's a list of things:\n\
\n\
\n\
   TESTING AND BUG REPORTING:\n\
\n\
We need people to try out the game and report anything that does not work correctly or seems to be unstable.  This includes minor things, such as spelling mistakes and the like.  The task of testing is growing with the size of FreedroidRPG.  So we need your help.\n\
\n\
\n\
   MAP IMPROVEMENTS:\n\
\n\
The world of FreedroidRPG can be enlarged and modified by point-and-click with the level editor integrated into FreedroidRPG.  There is lots of room for improvements and enrichments in the current FreedroidRPG landscape.\n\
\n\
\n\
   MORE STORY AND CHARACTERS:\n\
\n\
FreedroidRPG comes with a separate program for editing and inspecting dialogs.  Your help in setting up some new characters and mission would be greatly appreciated.\n\
\n\
\n\
   VOICE SAMPLES:\n\
\n\
Currently we are not accepting new speech. All of the dialogues are being rewritten, we will add voice to them later. If you are one of our old voice actors, then please get in touch with the team.\n\
\n\
\n\
   CONTACT:\n\n\
If you would like to contribute something or give some feedback concerning the game, you can easily get in contact with the people currently involved.  Just send e-mail to the following address:\n\
\n\
freedroid-discussion@lists.sourceforge.net\n\
\n\
Or, if you prefer that, you might pop into our IRC channel.  The channel is #freedroid on irc.freenode.net and everyone's invited.  Bring your friends and coke, beer for Basse and Arthur.\n\
\n\
Thank you,\n\
\n\
                The FreedroidRPG dev team.\n\
                                      \n\n\n\n\n";
    
    while ( SpacePressed() || EscapePressed() ) ; 
    
    SwitchBackgroundMusicTo ( CREDITS_BACKGROUND_MUSIC_SOUND );
    
    ScrollText ( ContributeText , SCROLLSTARTX, SCROLLSTARTY, User_Rect.y , NE_CREDITS_PIC_BACKGROUND_CODE );

    while ( SpacePressed() || EscapePressed() ) ; 
  
}; // void Contribute_Menu(void)

/*@Function============================================================
@Desc: This function provides the details of a mission that has been
       assigned to the player or has been solved perhaps too

@Ret:  none
* $Function----------------------------------------------------------*/
void
Show_Mission_Details ( int MissionNumber )
{
    int can_continue = 0;

    while( SpacePressed() || EnterPressed() ) keyboard_update(); 
    
    while (!can_continue)
    {
	
	DisplayImage (find_file (HS_BACKGROUND_FILE, GRAPHICS_DIR, FALSE));
	MakeGridOnScreen ( (SDL_Rect*) & Full_Screen_Rect );
	DisplayBanner( );
	//InitiateMenu();
	
	CenteredPutString ( Screen ,  1*FontHeight(Menu_BFont),    "MISSION DETAILS");
	
	printf_SDL ( Screen , User_Rect.x , 3 *FontHeight(Menu_BFont) , "Kill all droids : "  );
	if ( Me[0].AllMissions[ MissionNumber ].KillAll != (-1) ) printf_SDL( Screen , -1 , -1 , "YES" ); 
	else printf_SDL( Screen , -1 , -1 , "NO" );
	
	printf_SDL ( Screen , User_Rect.x , 4 *FontHeight(Menu_BFont) , "Kill special : "  );
	if ( Me[0].AllMissions[ MissionNumber ].KillOne != (-1) ) printf_SDL( Screen , -1 , -1 , "YES" ); 
	else printf_SDL( Screen , -1 , -1 , "NO" );
	printf_SDL ( Screen , -1 , -1 , "   ReachLevel : "  );
	if ( Me[0].AllMissions[ MissionNumber ].MustReachLevel != (-1) ) printf_SDL( Screen , -1 , -1 , "%d\n" , Me[0].AllMissions[ MissionNumber ].MustReachLevel ); 
	else printf_SDL( Screen , -1 , -1 , "NONE\n" );
	
	printf_SDL ( Screen , User_Rect.x , 5 *FontHeight(Menu_BFont) , "Reach X= : "  );
	if ( Me[0].AllMissions[ MissionNumber ].MustReachPoint.x != (-1) ) printf_SDL( Screen , -1 , -1 , "%d" , Me[0].AllMissions[ MissionNumber ].MustReachPoint.x ); 
	else printf_SDL( Screen , -1 , -1 , "NONE" );
	printf_SDL ( Screen , -1 , -1 , "   Reach Y= : "  );
	if ( Me[0].AllMissions[ MissionNumber ].MustReachPoint.y != (-1) ) printf_SDL( Screen , -1 , -1 , "%d\n" , Me[0].AllMissions[ MissionNumber ].MustReachPoint.y );
	else printf_SDL( Screen , -1 , -1 , "NONE\n" );
	
	printf_SDL ( Screen , User_Rect.x , 6 *FontHeight(Menu_BFont) , "Live Time : "  );
	if ( Me[0].AllMissions[ MissionNumber ].MustLiveTime != (-1) ) printf_SDL( Screen , -1 , -1 , "%4.0f" , Me[0].AllMissions[ MissionNumber ].MustLiveTime ); 
	else printf_SDL( Screen , -1 , -1 , "NONE" );
	printf_SDL ( Screen , User_Rect.x , 7 *FontHeight(Menu_BFont) , "Must be class : "  );
	if ( Me[0].AllMissions[ MissionNumber ].MustBeClass != (-1) ) printf_SDL( Screen , -1 , -1 , "%d\n" , Me[0].AllMissions[ MissionNumber ].MustBeClass );
	else printf_SDL( Screen , -1 , -1 , "NONE\n" );
	
	printf_SDL ( Screen , User_Rect.x , 8 *FontHeight(Menu_BFont) , "Must be type : "  );
	if ( Me[0].AllMissions[ MissionNumber ].MustBeType != (-1) ) printf_SDL( Screen , -1 , -1 , "%d" , Me[0].AllMissions[ MissionNumber ].MustBeType ); 
	else printf_SDL( Screen , -1 , -1 , "NONE" );
	printf_SDL ( Screen , User_Rect.x , 9*FontHeight(Menu_BFont) , "Must be special : "  );
	if ( Me[0].AllMissions[ MissionNumber ].MustBeOne != (-1) ) printf_SDL( Screen , -1 , -1 , "YES" );
	else printf_SDL( Screen , -1 , -1 , "NO\n" );
	
	printf_SDL ( Screen , User_Rect.x , 10 * FontHeight(Menu_BFont) , "Kill Class : "  );
	if ( Me[0].AllMissions[ MissionNumber ].KillClass != (-1) ) printf_SDL( Screen , -1 , -1 , "%s" , Classname[Me[0].AllMissions[ MissionNumber ].KillClass] ); 
	else printf_SDL( Screen , -1 , -1 , "NONE\n" );
	
	our_SDL_flip_wrapper( Screen );
	
	while ( (!EscapePressed()) && (!EnterPressed()) && (!SpacePressed()) );
	// Wait until the user does SOMETHING
	
	if ( EscapePressed() || EnterPressed() || SpacePressed() )
	{
	    can_continue=!can_continue;
	}
    }
    while ( EscapePressed() || EnterPressed() || SpacePressed() );

}; // void Show_Mission_Details (void)

/* ----------------------------------------------------------------------
 * This function provides an overview over the missions currently
 * assigned to the player
 * ---------------------------------------------------------------------- */
void
Show_Mission_Log_Menu (void)
{
  int can_continue = 0;
  int i;
  int NoOfActiveMissions;
  int MenuPosition=1;
  int InterLineSpace=60;
  SDL_Rect* Mission_Window_Pointer=&User_Rect;

#define MISSION_NAME_POS_X 230
#define FIRST_MISSION_POS_Y 50


  while( SpacePressed() || EnterPressed() ) keyboard_update(); 

  while (!can_continue)
    {

      DisplayImage (find_file (HS_BACKGROUND_FILE, GRAPHICS_DIR, FALSE));
      MakeGridOnScreen ( (SDL_Rect*) & Full_Screen_Rect );
      DisplayBanner( );

      SetCurrentFont( Para_BFont );

      DisplayText ( "This is the record of all missions you have been assigned:\n\n" , 
		    0 , FIRST_MISSION_POS_Y - 2 * InterLineSpace , Mission_Window_Pointer , TEXT_STRETCH );

      NoOfActiveMissions=0;
      for ( i = 0 ; i < MAX_MISSIONS_IN_GAME ; i ++ )
	{

	  if ( Me[0].AllMissions[i].MissionExistsAtAll != TRUE ) continue;

	  NoOfActiveMissions++;

	  // DisplayText ( "\nMission status: " , -1 , -1 , Mission_Window_Pointer );

	  if ( Me[0].AllMissions[i].MissionIsComplete == TRUE )
	    {
	      DisplayText ( "SOLVED: " , 0 , FIRST_MISSION_POS_Y + NoOfActiveMissions * InterLineSpace , Mission_Window_Pointer , TEXT_STRETCH );
	    }
	  else if ( Me[0].AllMissions[i].MissionWasFailed == TRUE )
	    {
	      DisplayText ( "FAILED: " , 0 , FIRST_MISSION_POS_Y + NoOfActiveMissions * InterLineSpace , Mission_Window_Pointer , TEXT_STRETCH );
	    }
	  else if ( Me[0].AllMissions[i].MissionWasAssigned == TRUE ) 
	    {
	      DisplayText ( "ASSIGNED: " , 0 , FIRST_MISSION_POS_Y + NoOfActiveMissions * InterLineSpace , Mission_Window_Pointer , TEXT_STRETCH );
	    }
	  else
	    {
	      DisplayText ( "UNASSIGNED: " , 0 , FIRST_MISSION_POS_Y +  NoOfActiveMissions * InterLineSpace , Mission_Window_Pointer , TEXT_STRETCH );
	    }

	  DisplayText ( Me[0].AllMissions[i].MissionName , MISSION_NAME_POS_X , 
			FIRST_MISSION_POS_Y + NoOfActiveMissions * InterLineSpace ,  Mission_Window_Pointer , TEXT_STRETCH );

	}

      DisplayText ( "\n\n--- Currently no missions beyond that ---" , 
		    -1 , -1 , Mission_Window_Pointer , TEXT_STRETCH );

      // Highlight currently selected option with an influencer before it
      blit_tux( MISSION_NAME_POS_X , FIRST_MISSION_POS_Y + (MenuPosition) * InterLineSpace - 16 , 0 );

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

      our_SDL_flip_wrapper( Screen );

      if ( EscapePressed() || EnterPressed() || SpacePressed() )
	{
	  can_continue=!can_continue;
	}
    } // end of while loop

  // Wait until the user does SOMETHING
  //while ( (!EscapePressed()) && (!EnterPressed()) && (!SpacePressed()) );

  while ( EscapePressed() || EnterPressed() || SpacePressed() );

}; // void Show_Mission_Log_Menu ( void )

#undef _menu_c
