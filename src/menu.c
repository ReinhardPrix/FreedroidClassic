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

/* ----------------------------------------------------------------------
 * This file contains all menu functions and their subfunctions
 * ---------------------------------------------------------------------- */

#define _menu_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"

int New_Game_Requested=FALSE;
SDL_Surface* StoredMenuBackground = NULL;

int Single_Player_Menu (void);
int Multi_Player_Menu (void);
void Credits_Menu (void);
void Options_Menu (void);
void Show_Mission_Log_Menu (void);
EXTERN void LevelEditor(void);

#define FIRST_MENU_ITEM_POS_X (1*Block_Width)
#define FIRST_MENU_ITEM_POS_XX ( SCREEN_WIDTH - FIRST_MENU_ITEM_POS_X )
#define FIRST_MENU_ITEM_POS_Y (BANNER_HEIGHT + FontHeight(Menu_BFont) * 3 )

#define NUMBER_OF_ITEMS_ON_ONE_SCREEN 4
#define ITEM_MENU_DISTANCE 80
#define ITEM_FIRST_POS_Y 130

#define SELL_PRICE_FACTOR (0.25)
#define REPAIR_PRICE_FACTOR (0.5)

#define SINGLE_PLAYER_STRING "Single Player"
#define LOAD_EXISTING_HERO_STRING "The first 10 characters: "
#define DELETE_EXISTING_HERO_STRING "Select character to delete: "

/* ----------------------------------------------------------------------
 * This function tells over which menu item the mouse cursor would be,
 * if there were infinitely many menu items.
 * ---------------------------------------------------------------------- */
int
MouseCursorIsOverMenuItem( int first_menu_item_pos_y , int h )
{
  // int h = FontHeight ( GetCurrentFont() );
  
  //--------------------
  // The value of GetMousePos_y() NOT YET corrected for 16 pixels!!
  // Therefore we can write:
  //
  return ( ( ( GetMousePos_y () + 16 - first_menu_item_pos_y ) / h ) + 1 );

}; // void MouseCursorIsOverMenuItem( first_menu_item_pos_y )


/* ----------------------------------------------------------------------
 * This function restores the menu background, that must have been stored
 * before using the function of similar name.
 * ---------------------------------------------------------------------- */
void
RestoreMenuBackground ( void )
{
  SDL_BlitSurface ( StoredMenuBackground , NULL , Screen , NULL );
}; // void RestoreMenuBackground ( void )

/* ----------------------------------------------------------------------
 * This function stores the current background as the background for a
 * menu, so that it can be refreshed much faster than by reassembling it
 * every frame.
 * ---------------------------------------------------------------------- */
void
StoreMenuBackground ( void )
{
  //--------------------
  // If the memory was not yet allocated, we need to do that now...
  //
  // otherwise we free the old surface and create a new copy of the
  // current screen content...
  //
  if ( StoredMenuBackground == NULL )
    {
      StoredMenuBackground = SDL_DisplayFormat ( Screen );
    }
  else
    {
      SDL_FreeSurface ( StoredMenuBackground );
      StoredMenuBackground = SDL_DisplayFormat ( Screen );
    }

}; // void StoreMenuBackground ( void )

/* ----------------------------------------------------------------------
 * This function performs a menu for the player to select from, using the
 * keyboard only, currently, sorry.
 * ---------------------------------------------------------------------- */
int
DoMenuSelection( char* InitialText , char* MenuTexts[] , int FirstItem , char* BackgroundToUse , void* MenuFont )
{
  int h = FontHeight (GetCurrentFont());
  int i;
  static int MenuPosition = 1;
  int NumberOfOptionsGiven;
  int first_menu_item_pos_y;
  SDL_Rect HighlightRect;
  int MenuWithFileInformation = FALSE;

  //--------------------
  // At first we hide the mouse cursor, so that there can not be any
  // ambiguity whether to think of the tux cursor or the mouse cursor
  // to be the pointer we use.
  //
  // SDL_ShowCursor( SDL_DISABLE );
  //
  SDL_ShowCursor( SDL_ENABLE );

  if ( FirstItem != (-1) ) MenuPosition = FirstItem;

  //--------------------
  // First thing we do is find out how may options we have
  // been given for the menu
  //
  for ( i = 0 ; TRUE ; i ++ )
    {
      if ( strlen( MenuTexts[ i ] ) == 0 ) break;
    }
  NumberOfOptionsGiven = i;

  //--------------------
  // In those cases where we don't reset the menu position upon 
  // initalization of the menu, we must check for menu positions
  // outside the bounds of the current menu.
  //
  if ( MenuPosition > NumberOfOptionsGiven ) MenuPosition = 1 ; 

  first_menu_item_pos_y = ( SCREEN_HEIGHT - NumberOfOptionsGiven * h ) / 2 ;

  //--------------------
  // We need to prepare the background for the menu, so that
  // it can be accessed with proper speed later...
  //
  InitiateMenu( BackgroundToUse );
  if ( ! strcmp ( MenuTexts [ 0 ] , SINGLE_PLAYER_STRING ) )
    {
      RightPutString( Screen , SCREEN_HEIGHT - FontHeight ( GetCurrentFont() ) , VERSION );
      // printf ("\n%s %s  \n", PACKAGE, VERSION);
    }
  else if ( ( ! strcmp ( InitialText , LOAD_EXISTING_HERO_STRING ) ) ||
	    ( ! strcmp ( InitialText , DELETE_EXISTING_HERO_STRING ) ) )
    {
      MenuWithFileInformation = TRUE ;
    }
  StoreMenuBackground ();

  //--------------------
  // Now that the possible font-changing background assembling is
  // done, we can finally set the right font for the menu itself.
  //
  if ( MenuFont == NULL ) SetCurrentFont ( Menu_BFont );
  else SetCurrentFont ( (BFont_Info*) MenuFont );
  h = FontHeight ( GetCurrentFont() );

  while ( 1 )
    {

      RestoreMenuBackground ();

      if ( MenuWithFileInformation )
	{
	  //--------------------
	  // We load the thumbnail, or at least we try to do it...
	  //
	  LoadAndShowThumbnail ( MenuTexts [ MenuPosition - 1 ] );
	  LoadAndShowStats ( MenuTexts [ MenuPosition - 1 ] );
	}
      HighlightRect.x = ( SCREEN_WIDTH - TextWidth ( MenuTexts [ MenuPosition - 1 ] ) ) / 2 - h ;
      HighlightRect.y = first_menu_item_pos_y + ( MenuPosition - 1 ) * h ;
      HighlightRect.w = TextWidth ( MenuTexts [ MenuPosition - 1 ] ) + 2 * h ;
      HighlightRect.h = h;		    
      HighlightRectangle ( Screen , HighlightRect );


      for ( i = 0 ; TRUE ; i ++ )
	{
	  if ( strlen( MenuTexts[ i ] ) == 0 ) break;
	  CutDownStringToMaximalSize ( MenuTexts [ i ] , 550 );
	  CenteredPutString ( Screen ,  first_menu_item_pos_y + i * h , MenuTexts[ i ] );
	}
      if ( strlen( InitialText ) > 0 ) 
	DisplayText ( InitialText , 50 , 50 , NULL );

      SDL_Flip( Screen );
  
      if ( EscapePressed() )
	{
	  while ( EscapePressed() );
	  MenuItemDeselectedSound();

	  SDL_ShowCursor( SDL_ENABLE );
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
	  MenuItemSelectedSound();

	  SDL_ShowCursor( SDL_ENABLE );
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
	      SDL_ShowCursor( SDL_ENABLE );
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
      usleep ( 50 );
    }

  SDL_ShowCursor( SDL_ENABLE );
  return ( -1 );
}; // int DoMenuSelection( char* InitialText , char* MenuTexts[10] , asdfasd .... )

/* ----------------------------------------------------------------------
 * This function performs a menu for the player to select from, using the
 * keyboard or mouse wheel.
 * ---------------------------------------------------------------------- */
int
ChatDoMenuSelectionFlagged( char* InitialText , char* MenuTexts[ MAX_ANSWERS_PER_PERSON] , 
			    unsigned char Chat_Flags[ MAX_ANSWERS_PER_PERSON ] , int FirstItem , 
			    char* BackgroundToUse , void* MenuFont )
{
  int MenuSelection = (-1) ;
  char* FilteredChatMenuTexts[ 10 ] = { "" , "" , "" , "" , "" , "" , "" , "" , "" , "" } ;
  int i;
  int use_counter = 0;

  //--------------------
  // We filter out those answering options that are allowed by the flag mask
  //
  for ( i = 0 ; i < MAX_ANSWERS_PER_PERSON ; i ++ )
    {
      if ( Chat_Flags[ i ] ) 
	{
	  FilteredChatMenuTexts[ use_counter ] = MenuTexts[ i ] ;
	  use_counter++;
	  if ( use_counter >= 10 )
	    {
	      DebugPrintf( 0 , "\n\nERROR:  TOO MANY CHAT ALTERNATIVES ACTIVATED... TERMINATING...\n\n" );
	      Terminate ( ERR );
	    }
	}
    }

  //--------------------
  // Now we do the usual menu selection, using only the activated chat alternatives...
  //
  MenuSelection = ChatDoMenuSelection( InitialText , FilteredChatMenuTexts , 
				       FirstItem , BackgroundToUse , MenuFont );

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
	      if ( use_counter >= 10 )
		{
		  DebugPrintf( 0 , "\n\nERROR:  TOO MANY CHAT ALTERNATIVES ACTIVATED... TERMINATING...\n\n" );
		  Terminate ( ERR );
		}
	      if ( MenuSelection == use_counter ) 
		{
		  DebugPrintf( 0 , "\nOriginal MenuSelect: %d. \nTransposed MenuSelect: %d." , 
			       MenuSelection , i+1 );
		  return ( i+1 );
		}
	    }
	}
    }

  return ( MenuSelection );
}; // int ChatDoMenuSelectionFlagged( char* InitialText , char* MenuTexts[ MAX_ANSWERS_PER_PERSON] , ... 

/* ----------------------------------------------------------------------
 * This function performs a menu for the player to select from, using the
 * keyboard or mouse wheel.
 * ---------------------------------------------------------------------- */
int
ChatDoMenuSelection( char* InitialText , char* MenuTexts[ 10 ] , int FirstItem , char* BackgroundToUse , void* MenuFont )
{
  int h = FontHeight (GetCurrentFont());
  int i;
  static int MenuPosition = 1;
  int NumberOfOptionsGiven;
#define ITEM_DIST 50
  int MenuPosX[20] = { 260 , 260 , 260 , 260 , 260 , 260 , 260 , 260 , 260 , 260 } ;
  int MenuPosY[20] = {  90 , 90 + 1 * ITEM_DIST , 90 + 2 * ITEM_DIST , 90 + 3 * ITEM_DIST , 90 + 4 * ITEM_DIST , 
      90 + 5 * ITEM_DIST , 90 + 6 * ITEM_DIST , 90 + 7 * ITEM_DIST , 90 + 8 * ITEM_DIST , 90 + 9 * ITEM_DIST } ;
  SDL_Rect Choice_Window;
  SDL_Rect HighlightRect;

  //--------------------
  // This is the old (before Bastians redesign) choice window:
  //
  // Choice_Window.x= 260; Choice_Window.y=60; Choice_Window.w=640 - 10 - 260; Choice_Window.h= 480 - 60;
  // 
  // Now we have a new design:
  //
  Choice_Window . x = 35; Choice_Window . y = 340; Choice_Window . w = 640 - 70; Choice_Window . h = 110;

  //--------------------
  // At first we hide the mouse cursor, so that there can not be any
  // ambiguity whether to think of the tux cursor or the mouse cursor
  // to be the pointer we use.
  //
  // SDL_ShowCursor( SDL_DISABLE );
  //
  SDL_ShowCursor( SDL_ENABLE );

  if ( FirstItem != (-1) ) MenuPosition = FirstItem;

  //--------------------
  // First thing we do is find out how may options we have
  // been given for the menu
  //
  for ( i = 0 ; i < 10 ; i ++ )
    {
      if ( strlen( MenuTexts[ i ] ) == 0 ) break;
    }
  NumberOfOptionsGiven = i;

  //--------------------
  // Now that we have a new choice window, we should automatically compute the right
  // positions for the various chat alternatives.
  //
  for ( i = 0 ; i < NumberOfOptionsGiven ; i++ )
    {
      MenuPosX[ i ]  = Choice_Window . x ;
      MenuPosY[ i ]  = Choice_Window . y + i * Choice_Window .h / NumberOfOptionsGiven ;
    }

  //--------------------
  // We need to prepare the background for the menu, so that
  // it can be accessed with proper speed later...
  //
  SDL_SetClipRect( Screen, NULL );
  StoreMenuBackground ();

  //--------------------
  // Now that the possible font-changing background assembling is
  // done, we can finally set the right font for the menu itself.
  //
  if ( MenuFont == NULL ) SetCurrentFont ( Menu_BFont );
  else SetCurrentFont ( (BFont_Info*) MenuFont );
  h = FontHeight ( GetCurrentFont() );

  while ( 1 )
    {

      RestoreMenuBackground ();

      //--------------------
      // We highlight the currently selected option with an 
      // influencer to the left and right of it.
      //
      // PutInfluence( MenuPosX[ MenuPosition -1 ] , MenuPosY[ MenuPosition -1 ] + (h/2) , 0 );
      //
      HighlightRect.x = MenuPosX[ MenuPosition -1 ] - 0 * h ;
      HighlightRect.y = MenuPosY[ MenuPosition -1 ] ;
      HighlightRect.w = TextWidth ( MenuTexts [ MenuPosition - 1 ] ) + 0 * h ;
      HighlightRect.h = h;		    
      HighlightRectangle ( Screen , HighlightRect );

      for ( i = 0 ; i < 10 ; i ++ )
	{
	  if ( strlen( MenuTexts[ i ] ) == 0 ) continue;
	  // PutString ( Screen ,  MenuPosX[i] , MenuPosY[i] , MenuTexts[ i ] );
	  DisplayText ( MenuTexts[i] , MenuPosX[i] , MenuPosY[i] , &Choice_Window );
	}

      // if ( strlen( InitialText ) > 0 ) DisplayText ( InitialText , 50 , 50 , NULL );


      SDL_Flip( Screen );
  
      if ( EscapePressed() )
	{
	  while ( EscapePressed() );
	  // MenuItemDeselectedSound();

	  RestoreMenuBackground ();
	  SDL_Flip( Screen );
	  SDL_ShowCursor( SDL_ENABLE );
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
	  
	  SDL_ShowCursor( SDL_ENABLE );
	  RestoreMenuBackground ();
	  SDL_Flip( Screen );
	  return ( MenuPosition );

	}
      if ( axis_is_active )
	{
	  while ( EnterPressed() || SpacePressed() ); // || RightPressed() || LeftPressed() );
	  //--------------------
	  // Only if the mouse click really occured within the menu, we will interpret
	  // it as menu choice.  Otherwise it will be just ignored.
	  //
	  if ( MouseCursorIsOverMenuItem( MenuPosY [ 0 ] , MenuPosY [ 1 ] - MenuPosY [ 0 ] ) == MenuPosition )
	    {
	      SDL_ShowCursor( SDL_ENABLE );
	      RestoreMenuBackground ();
	      SDL_Flip( Screen );
	      return ( MenuPosition );
	    }
	  
	}
      if ( UpPressed() || MouseWheelUpPressed() ) 
	{
	  if (MenuPosition > 1) MenuPosition--;
	  MoveMenuPositionSound();
	  HighlightRect.x = MenuPosX[ MenuPosition-1 ] + 2 * h ; 
	  HighlightRect.y = MenuPosY[ MenuPosition-1 ] ;
	  SDL_WarpMouse ( HighlightRect.x , HighlightRect.y );
	  while (UpPressed());
	}
      if ( DownPressed() || MouseWheelDownPressed() ) 
	{
	  if ( MenuPosition < NumberOfOptionsGiven ) MenuPosition++;
	  MoveMenuPositionSound();
	  HighlightRect.x = MenuPosX[ MenuPosition-1 ] + 2 * h ; 
	  HighlightRect.y = MenuPosY[ MenuPosition-1 ] ;
	  SDL_WarpMouse ( HighlightRect.x , HighlightRect.y );
	  while (DownPressed());
	}

      MenuPosition = MouseCursorIsOverMenuItem( MenuPosY [ 0 ] , MenuPosY [ 1 ] - MenuPosY [ 0 ] );
      if ( MenuPosition < 1 ) MenuPosition = 1 ;
      if ( MenuPosition > NumberOfOptionsGiven ) MenuPosition = NumberOfOptionsGiven ;

    }

  SDL_ShowCursor( SDL_ENABLE );
  RestoreMenuBackground ();
  SDL_Flip( Screen );
  return ( -1 );

}; // int ChatDoMenuSelection( char* InitialText , char* MenuTexts[10] , asdfasd .... )

/* ----------------------------------------------------------------------
 * This function prepares the screen for the big Escape menu and 
 * its submenus.  This means usual content of the screen, i.e. the 
 * combat screen and top status bar, is "faded out", the rest of 
 * the screen is cleared.  This function resolves some redundance 
 * that occured since there are so many submenus needing this.
 * ---------------------------------------------------------------------- */
void 
InitiateMenu( char* BackgroundToUse )
{
  //--------------------
  // Here comes the standard initializer for all the menus and submenus
  // of the big escape menu.  This prepares the screen, so that we can
  // write on it further down.
  //
  SDL_SetClipRect( Screen, NULL );
  ClearGraphMem();

  if ( BackgroundToUse == NULL )
    {
      DisplayBanner (NULL, NULL,  BANNER_NO_SDL_UPDATE | BANNER_FORCE_UPDATE );
      AssembleCombatPicture ( 0 );
      MakeGridOnScreen( NULL );
    }
  else
    {
      DisplayImage ( find_file ( BackgroundToUse , GRAPHICS_DIR, FALSE ) );
    }

  SDL_SetClipRect( Screen, NULL );
}; // void InitiateMenu(void)

/* ----------------------------------------------------------------------
 * This function provides a convenient cheat menu, so that any 
 * tester does not have to play all through the game again and again
 * to see if a bug in a certain position has been removed or not.
 * ---------------------------------------------------------------------- */
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
      printf_SDL (Screen, x0, y0, "Current position: Level=%d, X=%d, Y=%d\n",
		   CurLevel->levelnum, (int)Me[0].pos.x, (int)Me[0].pos.y);
      printf_SDL (Screen, -1, -1, " a. Armageddon (alle Robots sprengen)\n");
      printf_SDL (Screen, -1, -1, " l. robot list of current level\n");
      printf_SDL (Screen, -1, -1, " g. complete robot list\n");
      printf_SDL (Screen, -1, -1, " d. destroy robots on current level\n");
      printf_SDL (Screen, -1, -1, " t. Teleportation\n");
      printf_SDL (Screen, -1, -1, " r. change to new robot type\n");
      printf_SDL (Screen, -1, -1, " i. Invinciblemode: %s",
		  InvincibleMode ? "ON\n" : "OFF\n");
      printf_SDL (Screen, -1, -1, " e. set energy\n");
      printf_SDL (Screen, -1, -1, " h. Auto-aquire all skills\n" );
      printf_SDL (Screen, -1, -1, " n. No hidden droids: %s",
		  show_all_droids ? "ON\n" : "OFF\n" );
      printf_SDL (Screen, -1, -1, " m. Map of Deck xy\n");
      printf_SDL (Screen, -1, -1, " s. Sound: %s",
		  sound_on ? "ON\n" : "OFF\n");
      printf_SDL (Screen, -1, -1, " x. Fullscreen : %s",
		  fullscreen_on ? "ON\n" : "OFF\n");
      printf_SDL (Screen, -1, -1, " w. Print current waypoints\n");
      printf_SDL (Screen, -1, -1, " z. change Zoom factor\n");
      printf_SDL (Screen, -1, -1, " f. Freeze on this positon: %s",
		  stop_influencer ? "ON\n" : "OFF\n");
      printf_SDL (Screen, -1, -1, " q. RESUME game\n");

      switch (getchar_raw ())
	{
	case 'f':
	  stop_influencer = !stop_influencer;
	  break;

	case 'z':
	  ClearGraphMem();
	  printf_SDL (Screen, x0, y0, "Current Zoom factor: %f\n",
		      CurrentCombatScaleFactor); 
	  printf_SDL (Screen, -1, -1, "New zoom factor: ");
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
	      if (AllEnemys[i].pos.z == CurLevel->levelnum) 
		{
		  if (l && !(l%20)) 
		    {
		      printf_SDL (Screen, -1, -1, " --- MORE --- \n");
		      if( getchar_raw () == 'q')
			break;
		    }
		  if (!(l % 20) )  
		    {
		      ClearGraphMem ();
		      printf_SDL (Screen, x0, y0,
				   " NR.   ID  X    Y   ENERGY   speedX Status Friendly\n");
		      printf_SDL (Screen, -1, -1,
				  "---------------------------------------------\n");
		    }
		  
		  l ++;
		  printf_SDL (Screen, 15, -1,
			      "%d.   %s   %d   %d   %d    %g ", i,
			       Druidmap[AllEnemys[i].type].druidname,
			       (int)AllEnemys[i].pos.x,
			       (int)AllEnemys[i].pos.y,
			       (int)AllEnemys[i].energy,
			       AllEnemys[i].speed.x );
		  if ( AllEnemys[i].Status == MOBILE ) printf_SDL (Screen, -1, -1, "MOB" );
		  else if ( AllEnemys[i].Status == OUT ) printf_SDL (Screen, -1, -1, "OUT" );
		  else printf_SDL (Screen, -1, -1, "ERROR-UNKNOWN" );
		  if ( AllEnemys[i].is_friendly ) printf_SDL (Screen, -1, -1, " YES" );
		  else printf_SDL (Screen, -1, -1, " NO" );
		  printf_SDL (Screen, -1, -1, "\n" );

		} /* if (enemy on current level)  */
	    } /* for (i<NumEnemys) */

	  printf_SDL (Screen, 15, -1," --- END --- \n");
	  printf_SDL (Screen, 15, -1," BTW:  Number_Of_Droids_On_Ship: %d \n" , Number_Of_Droids_On_Ship );
	  while ( ( !SpacePressed()) && (!EscapePressed()) );
	  while ( SpacePressed() || EscapePressed() );
	  break;

	case 'g': /* complete robot list of this ship */
	  for (i = 0; i < MAX_ENEMYS_ON_SHIP ; i++)
	    {
	      if ( AllEnemys[i].type == (-1) ) continue;

	      if (i && !(i%13)) 
		{
		  printf_SDL (Screen, -1, -1, " --- MORE --- ('q' to quit)\n");
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
	    } /* for (i<NumEnemys) */

	  printf_SDL (Screen, -1, -1, " --- END ---\n");
	  getchar_raw ();
	  break;


	case 'd': /* destroy all robots on this level, haha */
	  for (i = 0; i < NumEnemys; i++)
	    {
	      if (AllEnemys[i].pos.z == CurLevel->levelnum)
		AllEnemys[i].energy = -100;
	    }
	  printf_SDL (Screen, -1, -1, "All robots on this deck killed!\n");
	  getchar_raw ();
	  break;


	case 't': /* Teleportation */
	  ClearGraphMem ();
	  printf_SDL (Screen, x0, y0, "Enter Level, X, Y: ");
	  input = GetString (40, 2);
	  sscanf (input, "%d, %d, %d\n", &LNum, &X, &Y);
	  free (input);
	  Teleport ( LNum , X , Y , 0 , TRUE , TRUE ) ;
	  break;

	case 'r': /* change to new robot type */
	  ClearGraphMem ();
	  printf_SDL (Screen, x0, y0, "Type number of new robot: ");
	  input = GetString (40, 2);
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

	case 'i': /* togge Invincible mode */
	  InvincibleMode = !InvincibleMode;
	  break;

	case 'e': /* complete heal */
	  ClearGraphMem();
	  printf_SDL (Screen, x0, y0, "Current energy: %f\n", Me[0].energy);
	  printf_SDL (Screen, -1, -1, "Enter your new energy: ");
	  input = GetString (40, 2);
	  sscanf (input, "%d", &num);
	  free (input);
	  Me[0].energy = (double) num;
	  if (Me[0].energy > Me[0].health) Me[0].health = Me[0].energy;
	  break;

	case 'h': // auto-aquire all skills
	  for ( i = 0 ; i < NUMBER_OF_SKILLS ; i ++ ) Me [ 0 ] . base_skill_level [ i ] = 1 ;
	  break;

	case 'n': /* toggle display of all droids */
	  show_all_droids = !show_all_droids;
	  break;

	case 's': /* toggle sound on/off */
	  sound_on = !sound_on;
	  break;

	case 'm': /* Show deck map in Concept view */
	  printf_SDL (Screen, -1, -1, "\nLevelnum: ");
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
	  Weiter = 1;
	  break;
	} /* switch (getchar_raw()) */
    } /* while (!Weiter) */

  InitBars = TRUE;

  ClearGraphMem ();
  SDL_Flip (Screen);

  keyboard_update (); /* treat all pending keyboard events */
  /* 
   * when changing windows etc, sometimes a key-release event gets 
   * lost, so we have to make sure that CPressed is no longer set
   * or we stay here for ever...
   */
  CurrentlyCPressed = FALSE;

  return;
}; /* Cheatmenu() */

/* ----------------------------------------------------------------------
 * This function lets you select whether you want to play the single player
 * mode or the multi player mode or the credits or the intro again or exit.
 * ---------------------------------------------------------------------- */
void
StartupMenu (void)
{
#define FIRST_MIS_SELECT_ITEM_POS_X (0.0*Block_Width)
#define FIRST_MIS_SELECT_ITEM_POS_Y (BANNER_HEIGHT + FontHeight(Menu_BFont))
enum
  { 
    SINGLE_PLAYER_POSITION=1, 
    MULTI_PLAYER_POSITION,
    CREDITS_POSITION,
    EXIT_FREEDROID_POSITION
  };
  int Weiter = 0;
  int MenuPosition=1;
  char* MenuTexts[10];

  Me[0].status=MENU;

  DebugPrintf ( 1 , "\nvoid StartupMenu ( void ): real function call confirmed. "); 

  SDL_SetClipRect( Screen , NULL );

  // Prevent distortion of framerate by the delay coming from 
  // the time spent in the menu.
  Activate_Conservative_Frame_Computation();

  while (!Weiter)
    {
      SetCurrentFont ( Menu_BFont );

      MenuTexts[0]= SINGLE_PLAYER_STRING ;
      MenuTexts[1]="Multi Player";
      MenuTexts[2]="Credits";
      MenuTexts[3]="Exit Freedroid";
      MenuTexts[4]="";
      MenuTexts[5]="";
      MenuTexts[6]="";
      MenuTexts[7]="";
      MenuTexts[8]="";
      MenuTexts[9]="";

      MenuPosition = DoMenuSelection( "" , MenuTexts , -1 , NE_TITLE_PIC_FILE , NULL );

      switch (MenuPosition) 
	{
	case SINGLE_PLAYER_POSITION:
	  // InitNewMissionList ( NEW_MISSION );
	  Weiter = Single_Player_Menu ( );
	  break;
	case MULTI_PLAYER_POSITION:
	  DisplayImage (find_file (NE_TITLE_PIC_FILE, GRAPHICS_DIR, FALSE));
	  SetCurrentFont ( Menu_BFont );
	  Weiter = Multi_Player_Menu();
	  break;
	case CREDITS_POSITION:
	  Credits_Menu();
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
  BannerIsDestroyed=TRUE;
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
    // SINGLE_PLAYER_POSITION, 
    RESUME_GAME_POSITION,
    OPTIONS_POSITION, 
    SET_THEME,
    LEVEL_EDITOR_POSITION, 
    LOAD_GAME_POSITION,
    QUIT_POSITION
  };

  int Weiter = 0;
  int MenuPosition=1;
  char theme_string[40];
  // int i;
  char* MenuTexts[10];

  Me[0].status=MENU;

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

  while (!Weiter)
    {
      strcpy (theme_string, "Theme: ");
      if (strstr (GameConfig.Theme_SubPath, "classic"))
	strcat (theme_string, "Classic");
      else if (strstr (GameConfig.Theme_SubPath, "lanzz"))
	strcat (theme_string, "Lanzz");
      else
	strcat (theme_string, "unknown");

      MenuTexts[0]="Save Game";
      MenuTexts[1]="Resume Game";
      MenuTexts[2]="Options";
      MenuTexts[3]=theme_string;
      MenuTexts[4]="Level Editor";
      MenuTexts[5]="Load Game";
      MenuTexts[6]="Quit Game";
      MenuTexts[7]="";
      MenuTexts[8]="";
      MenuTexts[9]="";

      MenuPosition = DoMenuSelection( "" , MenuTexts , 1 , NE_TITLE_PIC_FILE , NULL );

      switch (MenuPosition) 
	{
	case ( -1 ) :
	case ( RESUME_GAME_POSITION ) :
	  Weiter=!Weiter;
	  break;
	case OPTIONS_POSITION:
	  while (EnterPressed() || SpacePressed() );
	  Options_Menu();
	  // Weiter = TRUE;   /* jp forgot this... ;) */
	  break;
	case SET_THEME:
	  while (EnterPressed() || SpacePressed() );
	  /*
	  if ( !strcmp ( GameConfig.Theme_SubPath , "classic_theme/" ) )
	    {
	      // GameConfig.Theme_SubPath="lanzz_theme/";
	      strcpy ( GameConfig.Theme_SubPath , "lanzz_theme/" );
	    }
	  else
	    {
	      // GameConfig.Theme_SubPath="classic_theme/";
	      strcpy ( GameConfig.Theme_SubPath , "classic_theme/" );
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

	  */
	  break;
	case LEVEL_EDITOR_POSITION:
	  while (EnterPressed() || SpacePressed() );
	  LevelEditor();
	  // Weiter = TRUE;   /* jp forgot this... ;) */
	  break;
	case LOAD_GAME_POSITION:
	  LoadGame(  );
	  Weiter = TRUE;
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
  BannerIsDestroyed=TRUE;
  Me[0].status=MOBILE;

  return;

}; // void EscapeMenu( void )

/* ----------------------------------------------------------------------
 * This function provides a the options menu.  This menu is a 
 * submenu of the big EscapeMenu.  Here you can change sound vol.,
 * gamma correction, fullscreen mode, display of FPS and such
 * things.
 * ---------------------------------------------------------------------- */
void
New_GraphicsSound_Options_Menu (void)
{
  int Weiter = 0;
  int MenuPosition=1;
  char Options0[1000];
  char Options1[1000];
  char Options2[1000];
  char Options3[1000];
  char Options4[1000];
  char Options5[1000];
  char Options6[1000];
  char* MenuTexts[10]={ "" , "" , "" , "" , "" ,
			"" , "" , "" , "" , "" };
  enum
    { 
      SET_BG_MUSIC_VOLUME=1, 
      SET_SOUND_FX_VOLUME, 
      SET_GAMMA_CORRECTION, 
      SET_FULLSCREEN_FLAG, 
      CW_SIZE,
      SET_TERMINATE_ON_MISSING_FLAG,
      SET_SHOW_SUBTITLE_FLAG,
      LEAVE_OPTIONS_MENU 
    };

  // This is not some Debug Menu but an optically impressive 
  // menu for the player.  Therefore I suggest we just fade out
  // the game screen a little bit.

  while ( EscapePressed() );

  while (!Weiter)
    {

      sprintf( Options0 , "Background Music Volume: %1.2f" , GameConfig.Current_BG_Music_Volume );
      sprintf( Options1 , "Sound Effects Volume: %1.2f", GameConfig.Current_Sound_FX_Volume );
      sprintf( Options2 , "Gamma Correction: %1.2f", GameConfig.Current_Gamma_Correction );
      sprintf( Options3 , "Fullscreen Mode: %s", fullscreen_on ? "ON" : "OFF");
      sprintf( Options4 , "Combat Window Size: %s", classic_user_rect ? "CLASSIC" : "FULL" );
      sprintf( Options5 , "Terminate On Missing Sample: %s", 
	       GameConfig.terminate_on_missing_speech_sample ? "YES" : "NO" );
      sprintf( Options6 , "Show Subtitles in Dialogs: %s", 
	       GameConfig.show_subtitles_in_dialogs ? "YES" : "NO" );
      MenuTexts[0]=Options0;
      MenuTexts[1]=Options1;
      MenuTexts[2]=Options2;
      MenuTexts[3]=Options3;
      MenuTexts[4]=Options4;
      MenuTexts[5]=Options5;
      MenuTexts[6]=Options6;
      MenuTexts[7]="Back";

      MenuPosition = DoMenuSelection( "" , MenuTexts , -1 , NULL , NULL );

      switch (MenuPosition) 
	{

	case (-1):
	  Weiter=!Weiter;
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

	case SET_GAMMA_CORRECTION:

	  if ( RightPressed() ) 
	    {
	      while ( RightPressed());
	      GameConfig.Current_Gamma_Correction+=0.05;
	      SDL_SetGamma( GameConfig.Current_Gamma_Correction , GameConfig.Current_Gamma_Correction , GameConfig.Current_Gamma_Correction );
	    }

	  if ( LeftPressed() ) 
	    {
	      while (LeftPressed());
	      GameConfig.Current_Gamma_Correction-=0.05;
	      SDL_SetGamma( GameConfig.Current_Gamma_Correction , GameConfig.Current_Gamma_Correction , GameConfig.Current_Gamma_Correction );
	    }

	  break;

	case SET_FULLSCREEN_FLAG:
	  while (EnterPressed() || SpacePressed() );
	  SDL_WM_ToggleFullScreen (Screen);
	  fullscreen_on = !fullscreen_on;
	  break;

	case CW_SIZE:
	  while (EnterPressed() || SpacePressed() );
	  
	  if (classic_user_rect)
	    {
	      classic_user_rect = FALSE;
	      Copy_Rect (Full_User_Rect, User_Rect);
	    }
	  else
	    {
	      classic_user_rect = TRUE;
	      Copy_Rect (Classic_User_Rect, User_Rect);
	    }
	  
	  ClearGraphMem();
	  DisplayBanner( NULL , NULL , BANNER_FORCE_UPDATE );
	  SDL_Flip( Screen );
	  
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
	  Weiter=TRUE;
	  break;

	default: 
	  break;

	} 
    }

  ClearGraphMem ();
  DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE );
  InitBars = TRUE;

  return;

}; // void New_GraphicsSound_Options_Menu (void)

/* ----------------------------------------------------------------------
 * This function provides a the options menu.  This menu is a 
 * submenu of the big EscapeMenu.  Here you can change sound vol.,
 * gamma correction, fullscreen mode, display of FPS and such
 * things.
 * ---------------------------------------------------------------------- */
void
On_Screen_Display_Options_Menu (void)
{
  int Weiter = 0;
  int MenuPosition=1;
  char Options0[1000];
  char Options1[1000];
  char Options2[1000];
  char Options3[1000];
  char* MenuTexts[10]={ "" , "" , "" , "" , "" ,
			"" , "" , "" , "" , "" };
  enum
    { 
      SHOW_POSITION=1, 
      SHOW_FRAMERATE, 
      SHOW_ENERGY,
      SHOW_DROID_DIGITS,
      LEAVE_OPTIONS_MENU 
    };

  // This is not some Debug Menu but an optically impressive 
  // menu for the player.  Therefore I suggest we just fade out
  // the game screen a little bit.

  while ( EscapePressed() );

  while (!Weiter)
    {

      sprintf( Options0 , "Show Position: %s", GameConfig.Draw_Position ? "ON" : "OFF" );
      sprintf( Options1 , "Show Framerate: %s", GameConfig.Draw_Framerate? "ON" : "OFF" );
      sprintf( Options2 , "Show Energy: %s", GameConfig.Draw_Energy? "ON" : "OFF" );
      sprintf( Options3 , "Show Droid Digits: %s", GameConfig.show_digits_of_droids? "ON" : "OFF" );
      MenuTexts[0]=Options0;
      MenuTexts[1]=Options1;
      MenuTexts[2]=Options2;
      MenuTexts[3]=Options3;
      MenuTexts[4]="Back";

      MenuPosition = DoMenuSelection( "" , MenuTexts , -1 , NULL , NULL );

      switch (MenuPosition) 
	{
	case (-1):
	  Weiter=!Weiter;
	  break;
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
	case SHOW_DROID_DIGITS:
	  while (EnterPressed() || SpacePressed() );
	  GameConfig . show_digits_of_droids = ! GameConfig . show_digits_of_droids;
	  break;
	case LEAVE_OPTIONS_MENU:
	  while (EnterPressed() || SpacePressed() );
	  Weiter=TRUE;
	  break;
	default: 
	  break;
	} 
    }

  ClearGraphMem ();
  DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE );
  InitBars = TRUE;

  return;

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

  int Weiter = 0;
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

  while (!Weiter)
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

      MenuPosition = DoMenuSelection( "" , MenuTexts , -1 , NULL , NULL );

      switch (MenuPosition) 
	{
	case (-1):
	  Weiter=!Weiter;
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
	  Weiter=TRUE;
	  break;
	default: 
	  break;
	} 
    }

  ClearGraphMem ();
  DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE );
  InitBars = TRUE;

  return;

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
  int Weiter = 0;
  int MenuPosition=1;
  char* MenuTexts[10];
enum
  { 
    GRAPHICS_SOUND_OPTIONS=1, 
    DROID_TALK_OPTIONS,
    ON_SCREEN_DISPLAYS,
    SAVE_OPTIONS, 
    LEAVE_OPTIONS_MENU 
  };

  MenuTexts[0]="Graphics & Sound";
  MenuTexts[1]="Droid Talk";
  MenuTexts[2]="On-Screen Displays";
  MenuTexts[3]="Save Options";
  MenuTexts[4]="Back";
  MenuTexts[5]="";
  MenuTexts[6]="";
  MenuTexts[7]="";
  MenuTexts[8]="";
  MenuTexts[9]="";

  while ( !Weiter )
    {

      MenuPosition = DoMenuSelection( "" , MenuTexts , 1 , NULL , NULL );

      switch (MenuPosition) 
	{
	case (-1):
	  Weiter=!Weiter;
	  break;
	case GRAPHICS_SOUND_OPTIONS:
	  while (EnterPressed() || SpacePressed() );
	  New_GraphicsSound_Options_Menu();
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
    }

  ClearGraphMem ();
  DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE );
  InitBars = TRUE;

  return;

} // Options_Menu

/* ----------------------------------------------------------------------
 * This reads in the new name for the character...
 * ---------------------------------------------------------------------- */
void
Get_Server_Name ( void )
{
  char* Temp;
  InitiateMenu( NE_TITLE_PIC_FILE );

  DisplayText ( "\n\
 Please enter name of server to connect to:\n\
 You can give an empty string for the local host.\n\
 \n      " , 50 , 50 , NULL );

  Temp = GetString( 140 , FALSE );
  strcpy ( ServerName , Temp );
  free( Temp );
}; // void Get_New_Character_Name ( void )

/* ----------------------------------------------------------------------
 * This reads in the new name for the character...
 * ---------------------------------------------------------------------- */
void
Get_New_Character_Name ( void )
{
  char* Temp;
  InitiateMenu( NE_TITLE_PIC_FILE );

  DisplayText ( "\n     Enter the name\n     for the new hero:\n   > " ,
		50 , 50 , NULL );

  Temp = GetString( 20 , FALSE );
  strcpy ( Me[0].character_name , Temp );
  free( Temp );
}; // void Get_New_Character_Name ( void )

/* ----------------------------------------------------------------------
 * This function prepares a new hero for adventure...
 * ---------------------------------------------------------------------- */
int
PrepareNewHero (void)
{
  int i;

  //--------------------
  // At first we clear the inventory of the new character
  // of any items (or numeric waste) that might be in there
  //
  for ( i = 0 ; i < MAX_ITEMS_IN_INVENTORY ; i ++ )
    {
      Me[0].Inventory[ i ].type = (-1);
      Me[0].Inventory[ i ].prefix_code = (-1);
      Me[0].Inventory[ i ].suffix_code = (-1);
      Me[0].Inventory[ i ].currently_held_in_hand = FALSE;
    }
  DebugPrintf ( 1 , "\nPrepareNewHero...( ... ): Inventory has been emptied...");

  //--------------------
  // Now we add some safety, against 'none present' items
  //
  Me[0].weapon_item.type = ( -1 ) ;
  Me[0].drive_item.type = ( -1 ) ;
  Me[0].armour_item.type = ( -1 ) ;
  Me[0].shield_item.type = ( -1 ) ;
  Me[0].aux1_item.type = ( -1 ) ;
  Me[0].aux2_item.type = ( -1 ) ;
  Me[0].special_item.type = ( -1 ) ;

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
  Me[0].base_vitality = 10;
  Me[0].base_strength = 10;
  Me[0].base_dexterity = 10;
  Me[0].base_magic = 10;

  Get_New_Character_Name( );
  return ( TRUE );

}; // int SelectHeroClassMenu ( void );

/* ----------------------------------------------------------------------
 * This function does the selection of the hero class...
 * ---------------------------------------------------------------------- */
int
Connect_To_Existing_Server_Menu (void)
{

  //--------------------
  // Now we add some safety, against 'none present' items
  //
  Me[0].weapon_item.type = ( -1 ) ;
  Me[0].drive_item.type = ( -1 ) ;
  Me[0].armour_item.type = ( -1 ) ;
  Me[0].shield_item.type = ( -1 ) ;
  Me[0].aux1_item.type = ( -1 ) ;
  Me[0].aux2_item.type = ( -1 ) ;
  Me[0].special_item.type = ( -1 ) ;

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
  Me[0].drive_item.type = ITEM_ANTIGRAV_BETA;

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
static int
one (const struct dirent *unused)
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

}; // static int one (struct dirent *unused)

/* ----------------------------------------------------------------------
 * This is the function available from the freedroid startup menu, that
 * should display the available characters in the users home directory
 * and eventually let the player select one of his old characters there.
 * ---------------------------------------------------------------------- */
int 
Load_Existing_Hero_Menu ( void )
{
  char *homedir;
  char Saved_Games_Dir[1000];
  char* MenuTexts[ 10 ];
  // DIR *dp;
  // struct dirent *ep;
  struct dirent **eps;
  int n;  
  int cnt;
  int MenuPosition;

  DebugPrintf ( 1 , "\nint Load_Existing_Hero_Menu ( void ): real function call confirmed.");
  InitiateMenu( NE_TITLE_PIC_FILE );
  MenuTexts[0]="";
  MenuTexts[1]="";
  MenuTexts[2]="";
  MenuTexts[3]="";
  MenuTexts[4]="";
  MenuTexts[5]="";
  MenuTexts[6]="";
  MenuTexts[7]="";
  MenuTexts[8]="";
  MenuTexts[9]="";

  //--------------------
  // get home-directory to save in
  if ( (homedir = getenv("HOME")) == NULL ) 
    {
      DebugPrintf ( 0 , "ERROR: Environment does not contain HOME variable... \n\
I need to know that for saving. Abort.\n");
      Terminate( ERR );
      return (ERR);
    }

  //--------------------
  // Now we generate the right directory for saving from the home
  // directory.
  //
  sprintf ( Saved_Games_Dir , "%s/.freedroid_rpg" , homedir );


  // DisplayText ( "This is the record of all your characters:\n\n" , 50 , 50 , NULL );

  //--------------------
  // This is a slightly modified copy of the code sniplet from the
  // GNU C Library description on directory operations...
  //
  n = scandir ( Saved_Games_Dir , &eps, one , alphasort);
  if (n > 0)
    {
      for (cnt = 0; cnt < n; ++cnt) 
	{
	  puts ( eps[cnt]->d_name );
	  DisplayText ( eps[cnt]->d_name , 50 , 150 + cnt * 40 , NULL );
	  if ( cnt < 10 ) 
	    {
	      MenuTexts[ cnt ] = ReadAndMallocStringFromData ( eps[cnt]->d_name , "" , ".savegame" ) ;
	    }
	}

      MenuPosition = DoMenuSelection( LOAD_EXISTING_HERO_STRING , MenuTexts , 1 , NE_TITLE_PIC_FILE , NULL );

      if ( MenuPosition == (-1) ) return ( FALSE );
      else
	{
	  InitNewMissionList ( NEW_MISSION );
	  strcpy( Me[0].character_name , MenuTexts[ MenuPosition -1 ] );
	  LoadGame( );
	  return ( TRUE );
	}
    }
  else
    {
      fprintf ( stderr, "\n\nSaved_Games_Dir: '%s'.\n" , Saved_Games_Dir );
      GiveStandardErrorMessage ( "Load_Existing_Hero_Menu(...)" , "\
Either it couldn't open the directory for the saved games\n\
which should be %s,\n\
or there were no saved games present in this directory.\n\
Freedroid will continue execution now, since this problem\n\
 will be dealt with in-game.",
				 NO_NEED_TO_INFORM, IS_WARNING_ONLY );

      MenuTexts[0]="BACK";
      MenuTexts[1]="";
      MenuTexts[2]=""; MenuTexts[3]=""; MenuTexts[4]=""; MenuTexts[5]="";
      MenuTexts[8]=""; MenuTexts[6]=""; MenuTexts[7]=""; MenuTexts[9]="";

      while ( SpacePressed() || EnterPressed() );
      DoMenuSelection ( "\n\nNo saved games found!!  Loading Cancelled. " , MenuTexts , 1 , NE_TITLE_PIC_FILE , NULL );
      
      //--------------------
      // Now we got to return the problem to the calling function...
      //
      return ( FALSE );
    }


  SDL_Flip( Screen );

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
  char *homedir;
  char Saved_Games_Dir[1000];
  char* MenuTexts[ 10 ];
  struct dirent **eps;
  int n;  
  int cnt;
  int MenuPosition;
  int FinalDecision;
  char SafetyText[2000];

  DebugPrintf ( 0 , "\nint Delete_Existing_Hero_Menu ( void ): real function call confirmed.");
  InitiateMenu( NE_TITLE_PIC_FILE );
  MenuTexts[0]="";
  MenuTexts[1]="";
  MenuTexts[2]="";
  MenuTexts[3]="";
  MenuTexts[4]="";
  MenuTexts[5]="";
  MenuTexts[6]="";
  MenuTexts[7]="";
  MenuTexts[8]="";
  MenuTexts[9]="";

  //--------------------
  // get home-directory to save in
  //
  if ( (homedir = getenv("HOME")) == NULL ) 
    {
      DebugPrintf ( 0 , "ERROR: Environment does not contain HOME variable... \n\
I need to know that for saving. Abort.\n");
      Terminate( ERR );
      return (ERR);
    }

  //--------------------
  // Now we generate the right directory for saving from the home
  // directory.
  //
  sprintf ( Saved_Games_Dir , "%s/.freedroid_rpg" , homedir );


  // DisplayText ( "This is the record of all your characters:\n\n" , 50 , 50 , NULL );

  //--------------------
  // This is a slightly modified copy of the code sniplet from the
  // GNU C Library description on directory operations...
  //
  n = scandir ( Saved_Games_Dir , &eps, one , alphasort);
  if (n > 0)
    {
      for (cnt = 0; cnt < n; ++cnt) 
	{
	  puts ( eps[cnt]->d_name );
	  DisplayText ( eps[cnt]->d_name , 50 , 150 + cnt * 40 , NULL );
	  if ( cnt < 10 ) 
	    {
	      MenuTexts[ cnt ] = ReadAndMallocStringFromData ( eps[cnt]->d_name , "" , ".savegame" ) ;
	    }
	}

      MenuPosition = DoMenuSelection( DELETE_EXISTING_HERO_STRING , MenuTexts , 1 , NE_TITLE_PIC_FILE , NULL );

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
	  MenuTexts [ 3 ] = "";
	  MenuTexts [ 4 ] = "";
	  MenuTexts [ 5 ] = "";
	  MenuTexts [ 6 ] = "";
	  MenuTexts [ 7 ] = "";
	  MenuTexts [ 8 ] = "";
	  MenuTexts [ 9 ] = "";
	  sprintf( SafetyText , "Really delete hero '%s'?" , Me[0].character_name ) ;
	  FinalDecision = DoMenuSelection( SafetyText , MenuTexts , 1 , NE_TITLE_PIC_FILE , NULL );

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
      GiveStandardErrorMessage ( "Delete_Existing_Hero_Menu(...)" , "\
Either it couldn't open the directory for the saved games\n\
which should be %s,\n\
or there were no saved games present in this directory.\n\
Freedroid will continue execution now, since this problem\n\
 will be dealt with in-game.",
				 NO_NEED_TO_INFORM, IS_WARNING_ONLY );

      MenuTexts[0]="BACK";
      MenuTexts[1]="";
      MenuTexts[2]=""; MenuTexts[3]=""; MenuTexts[4]=""; MenuTexts[5]="";
      MenuTexts[8]=""; MenuTexts[6]=""; MenuTexts[7]=""; MenuTexts[9]="";

      while ( SpacePressed() || EnterPressed() );
      DoMenuSelection ( "\n\nNo saved games found!!  Deletion Cancelled. " , MenuTexts , 1 , NE_TITLE_PIC_FILE , NULL );
      
      //--------------------
      // Now we got to return the problem to the calling function...
      //
      return ( FALSE );
    }

  SDL_Flip( Screen );

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
  int Weiter = 0;
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
  MenuTexts[5]="";
  MenuTexts[6]="";
  MenuTexts[7]="";
  MenuTexts[8]="";
  MenuTexts[9]="";

  while (!Weiter)
    {
      MenuPosition = DoMenuSelection( "" , MenuTexts , 1 , NE_TITLE_PIC_FILE , NULL );

      switch (MenuPosition) 
	{
	case NEW_HERO_POSITION:
	  while (EnterPressed() || SpacePressed() ) ;

	  // if ( Select_Hero_Class_Menu ( ) )
	  // {
	  PrepareNewHero ();
	  InitNewMissionList ( NEW_MISSION );
	  Weiter=TRUE;
	  return ( TRUE );
	  //}
	  break;

	case LOAD_EXISTING_HERO_POSITION: 
	  while (EnterPressed() || SpacePressed() ) ;

	  if ( Load_Existing_Hero_Menu ( ) == TRUE )
	    {
	      Weiter = TRUE;
	      return ( TRUE );
	    }
	  else
	    {
	      Weiter = FALSE;
	      // return ( FALSE );
	    }
	  break;

	case DELETE_EXISTING_HERO_POSITION: 
	  while (EnterPressed() || SpacePressed() ) ;
	  Delete_Existing_Hero_Menu ( ) ;
	  Weiter= FALSE ;
	  // return ( FALSE );
	  break;

	case (-1):
	case BACK_POSITION:
	  while (EnterPressed() || SpacePressed() || EscapePressed() ) ;
	  Weiter=!Weiter;
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
  int Weiter = 0;
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
  MenuTexts[5]="";
  MenuTexts[6]="";
  MenuTexts[7]="";
  MenuTexts[8]="";
  MenuTexts[9]="";

  while (!Weiter)
    {
      MenuPosition = DoMenuSelection( "" , MenuTexts , 1 , NE_TITLE_PIC_FILE , NULL );

      switch (MenuPosition) 
	{
	case START_AS_SERVER_POSITION:
	  while (EnterPressed() || SpacePressed() ) ;

	  InitNewMissionList ( NEW_MISSION );	

	  ServerMode = TRUE ;

	  OpenTheServerSocket (  );

	  Weiter=TRUE;
	  return ( TRUE );

	  break;

	case JOIN_EXISTING_MULTIPLAYER_POSITION: 
	  while (EnterPressed() || SpacePressed() ) ;

	  if ( Connect_To_Existing_Server_Menu ( ) == TRUE )
	    {
	      Weiter = TRUE;
	      InitNewMissionList ( NEW_MISSION );
	      ClientMode = TRUE;
	      return ( TRUE );
	    }
	  else
	    {
	      Weiter = FALSE;
	      // return ( FALSE );
	    }

	  break;
	case LIST_KNOWN_SERVERS: 
	  while (EnterPressed() || SpacePressed() ) ;

	  if ( Load_Existing_Hero_Menu ( ) == TRUE )
	    {
	      Weiter = TRUE;
	      return ( TRUE );
	    }
	  else
	    {
	      Weiter = FALSE;
	      // return ( FALSE );
	    }

	  break;
	case BACK_POSITION:
	case (-1):
	default: 
	  while (EnterPressed() || SpacePressed() ) ;
	  Weiter=!Weiter;
	  return ( FALSE );
	  break;
	}
    }
  return ( TRUE );

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
  // BFont_Info* Credits_BFont = FPS_Display_BFont;
  // int i;
  char* CreditsText = "\n\n\n\
                                            CREDITS\n\n\n\
   PROGRAMMING:\n\n\
                                      Johannes Prix\n\n\
                                      Reinhard Prix\n\n\n\
   ADDITIONAL PROGRAMMING:\n\n\
                                      Leslie Viljonen\n\n\n\
   ARTWORK:\n\n\
                                      Bastian Salmela\n\n\
                                      Lanzz\n\n\n\
   STORY AND CHARACTERS:\n\n\
                                      Johannes Prix\n\n\n\
   OPENING TEXT:\n\n\
                                      Pete Spicer\n\n\n\
   LEVEL DESIGN:\n\n\
                                      Johannes Prix\n\n\n\
   MOD COMPOSERS:\n\n\
                                      speedball (Ace-II)\n\n\
                                      knert&cosmos (Your World)\n\n\
                                      freeman/lard design\n\
                                       (The Great Bath)\n\n\
                                      radix/solitude (Nowhere)\n\n\
                                      jester of sanity (elysium)\n\n\
                                      Arachno/fadeout (Nostalgia)\n\n\
                                      mantronix+tip of phenomena\n\
                                       (Princess)\n\n\
                                      b.i.t./eon (Style Blend)\n\n\
                                      4-mat/anarchy (The Last V8 ,\n\
                                      Hardcore Theme,\n\
                                      Psychospugs, Starworx)\n\n\
                                      ???   (alfa , bladerunner,\n\
                                      delta2, slow)\n\n\n\
   VOICES:\n\n\
                                      Tiina Heinonen\n\n\
                                      Doris Stubenrauch\n\n\
                                      Johannes Prix\n\n\
                                      Mbrola\n\
                                       text-to-speech-system\n\n\n\
   .rpm AND .deb PACKAGING:\n\n\
                                      Ted Cipicchio\n\n\
                                      The Doctor\n\n\n\n\n";

  while( SpacePressed() || EscapePressed() ) ; /* wait for key release */

  // InitiateMenu();

  SwitchBackgroundMusicTo ( CREDITS_BACKGROUND_MUSIC_SOUND );

  ScrollText ( CreditsText , SCROLLSTARTX, SCROLLSTARTY, User_Rect.y , NE_CREDITS_PIC_FILE );

  while( SpacePressed() || EscapePressed() ) ; /* wait for key release */

}; // void Credits_Menu(void)

/*@Function============================================================
@Desc: This function provides the details of a mission that has been
       assigned to the player or has been solved perhaps too

@Ret:  none
* $Function----------------------------------------------------------*/
void
Show_Mission_Details ( int MissionNumber )
{
  int Weiter = 0;

  while( SpacePressed() || EnterPressed() ) keyboard_update(); 

  while (!Weiter)
    {

      DisplayImage (find_file (HS_BACKGROUND_FILE, GRAPHICS_DIR, FALSE));
      MakeGridOnScreen ( (SDL_Rect*) & Full_Screen_Rect );
      DisplayBanner( NULL , NULL , BANNER_FORCE_UPDATE );
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

      SDL_Flip( Screen );

      while ( (!EscapePressed()) && (!EnterPressed()) && (!SpacePressed()) );
      // Wait until the user does SOMETHING

      if ( EscapePressed() || EnterPressed() || SpacePressed() )
	{
	  Weiter=!Weiter;
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

      SetCurrentFont( Para_BFont );

      DisplayText ( "This is the record of all missions you have been assigned:\n\n" , 
		    0 , FIRST_MISSION_POS_Y - 2 * InterLineSpace , Mission_Window_Pointer );

      NoOfActiveMissions=0;
      for ( i = 0 ; i < MAX_MISSIONS_IN_GAME ; i ++ )
	{

	  if ( Me[0].AllMissions[i].MissionExistsAtAll != TRUE ) continue;

	  NoOfActiveMissions++;

	  // DisplayText ( "\nMission status: " , -1 , -1 , Mission_Window_Pointer );

	  if ( Me[0].AllMissions[i].MissionIsComplete == TRUE )
	    {
	      DisplayText ( "SOLVED: " , 0 , FIRST_MISSION_POS_Y + NoOfActiveMissions * InterLineSpace , Mission_Window_Pointer );
	    }
	  else if ( Me[0].AllMissions[i].MissionWasFailed == TRUE )
	    {
	      DisplayText ( "FAILED: " , 0 , FIRST_MISSION_POS_Y + NoOfActiveMissions * InterLineSpace , Mission_Window_Pointer );
	    }
	  else if ( Me[0].AllMissions[i].MissionWasAssigned == TRUE ) 
	    {
	      DisplayText ( "ASSIGNED: " , 0 , FIRST_MISSION_POS_Y + NoOfActiveMissions * InterLineSpace , Mission_Window_Pointer );
	    }
	  else
	    {
	      DisplayText ( "UNASSIGNED: " , 0 , FIRST_MISSION_POS_Y +  NoOfActiveMissions * InterLineSpace , Mission_Window_Pointer );
	    }

	  DisplayText ( Me[0].AllMissions[i].MissionName , MISSION_NAME_POS_X , 
			FIRST_MISSION_POS_Y + NoOfActiveMissions * InterLineSpace ,  Mission_Window_Pointer );

	}

      DisplayText ( "\n\n--- Currently no missions beyond that ---" , 
		    -1 , -1 , Mission_Window_Pointer );

      // Highlight currently selected option with an influencer before it
      PutInfluence( MISSION_NAME_POS_X , FIRST_MISSION_POS_Y + (MenuPosition) * InterLineSpace - Block_Width/4 , 0 );

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

      SDL_Flip( Screen );

      if ( EscapePressed() || EnterPressed() || SpacePressed() )
	{
	  Weiter=!Weiter;
	}
    } // end of while loop

  // Wait until the user does SOMETHING
  //while ( (!EscapePressed()) && (!EnterPressed()) && (!SpacePressed()) );

  while ( EscapePressed() || EnterPressed() || SpacePressed() );

}; // void Show_Mission_Log_Menu ( void )

#undef _menu_c
