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
 * This file contains all functions for the heart of the level editor.
 * ---------------------------------------------------------------------- */

#define _leveleditor_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"

void ShowWaypoints( int PrintConnectionList );
void LevelEditor(void);
Level CreateNewMapLevel( void );
void SetLevelInterfaces ( void );

EXTERN SDL_Surface *BackupMapBlockSurfacePointer[ NUM_COLORS ][ NUM_MAP_BLOCKS ];

char VanishingMessage[10000]="Hello";
float VanishingMessageDisplayTime = 0;
int OriginWaypoint = (-1);
SDL_Rect EditorBannerRect;
int FirstBlock = 0 ;
int Highlight = 3 ;

enum
  {
    JUMP_THRESHOLD_NORTH = 1,
    JUMP_THRESHOLD_SOUTH ,
    JUMP_THRESHOLD_EAST ,
    JUMP_THRESHOLD_WEST ,
    JUMP_TARGET_NORTH ,
    JUMP_TARGET_SOUTH ,
    JUMP_TARGET_EAST ,
    JUMP_TARGET_WEST ,
    EXPORT_THIS_LEVEL , 
    REPORT_INTERFACE_INCONSISTENCIES , 
    QUIT_THRESHOLD_EDITOR_POSITION
  };

enum
  { 
    SAVE_LEVEL_POSITION=1, 
    CHANGE_LEVEL_POSITION, 
    CHANGE_TILE_SET_POSITION, 
    CHANGE_SIZE_X, 
    CHANGE_SIZE_Y, 
    SET_LEVEL_NAME , 
    SET_BACKGROUND_SONG_NAME , 
    SET_LEVEL_COMMENT, 
    ADD_NEW_LEVEL , 
    SET_LEVEL_INTERFACE_POSITION , 
    EDIT_LEVEL_DIMENSIONS,
    QUIT_LEVEL_EDITOR_POSITION 
};

enum
  {
    INSERTREMOVE_COLUMN_VERY_WEST = 1,
    INSERTREMOVE_COLUMN_WESTERN_INTERFACE,
    INSERTREMOVE_COLUMN_EASTERN_INTERFACE,
    INSERTREMOVE_COLUMN_VERY_EAST,
    INSERTREMOVE_LINE_VERY_NORTH,
    INSERTREMOVE_LINE_NORTHERN_INTERFACE,
    INSERTREMOVE_LINE_SOUTHERN_INTERFACE,
    INSERTREMOVE_LINE_VERY_SOUTH,
    DUMMY_NO_REACTION1,
    DUMMY_NO_REACTION2,
    BACK_TO_LE_MAIN_MENU
  };

/* ----------------------------------------------------------------------
 * ---------------------------------------------------------------------- */
void
ZoomIn( void )
{
  CurrentCombatScaleFactor += 0.25;
  SetCombatScaleTo (CurrentCombatScaleFactor);
  while (IPressed());
}; // void ZoomIn( void )

/* ----------------------------------------------------------------------
 * ---------------------------------------------------------------------- */
void
ZoomOut( void )
{
  if (CurrentCombatScaleFactor > 0.25 )
    CurrentCombatScaleFactor -= 0.25;
  SetCombatScaleTo (CurrentCombatScaleFactor);
  while (OPressed());
}; // void ZoomOut( void )

/* ----------------------------------------------------------------------
 * This function shall determine, whether a given left mouse click was in 
 * given rect or not.
 * ---------------------------------------------------------------------- */
int
ClickWasInRect ( SDL_Rect TargetRect )
{
  if ( GetMousePos_x ( ) + 16 > TargetRect.x + TargetRect.w ) return FALSE;
  if ( GetMousePos_x ( ) + 16 < TargetRect.x ) return FALSE;
  if ( GetMousePos_y ( ) + 16 > TargetRect.y + TargetRect.h ) return FALSE;
  if ( GetMousePos_y ( ) + 16 < TargetRect.y ) return FALSE;

  return ( TRUE );
}; // int ClickWasInRect ( SDL_Rect TargetRect )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
int 
ClickWasInEditorBannerRect( void )
{
  return ( ClickWasInRect ( EditorBannerRect ) );
}; // int ClickWasInEditorBannerRect( void )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
HandleBannerMouseClick( void )
{
  SDL_Rect TargetRect;
  int i;

  if ( CursorIsOnButton ( LEFT_LEVEL_EDITOR_BUTTON , GetMousePos_x ( ) + 16 , GetMousePos_y ( ) + 16 ) )
    {
      if ( FirstBlock > 3 ) FirstBlock-= 8;
      DebugPrintf ( 1 , "\nBlocks should be scrolling now, if apprpirate..." );
    }
  if ( CursorIsOnButton ( RIGHT_LEVEL_EDITOR_BUTTON , GetMousePos_x ( ) + 16 , GetMousePos_y ( ) + 16 ) )
    {
      if ( FirstBlock < NUM_MAP_BLOCKS -8 ) FirstBlock+= 8;
      DebugPrintf ( 1 , "\nBlocks should be scrolling now, if apprpirate..." );
    }

  for ( i = 0 ; i < 9 ; i ++ ) 
    {
      TargetRect.x = INITIAL_BLOCK_WIDTH/2 + INITIAL_BLOCK_WIDTH * i; 
      TargetRect.y = INITIAL_BLOCK_HEIGHT/3;
      TargetRect.w = INITIAL_BLOCK_WIDTH;
      TargetRect.h = INITIAL_BLOCK_HEIGHT;
      if ( ClickWasInRect ( TargetRect ) )
	{
	  Highlight = FirstBlock + i;
	}; 
    }

}; // void HandleBannerMouseClick( void )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
ShowLevelEditorTopMenu( int Highlight )
{
  int i;
  SDL_Rect TargetRectangle;
  int MapBrick = FirstBlock;
  Level DisplayLevel = curShip . AllLevels [ Me [ 0 ] . pos . z ] ;
  static SDL_Surface *LevelEditorTopBanner = NULL;
  SDL_Surface *tmp = NULL;
  char* fpath;

  if ( LevelEditorTopBanner == NULL )
    {
      fpath = find_file ( LEVEL_EDITOR_BANNER_FILE , GRAPHICS_DIR, FALSE);
      tmp = IMG_Load( fpath );
      LevelEditorTopBanner = SDL_DisplayFormat ( tmp );
      SDL_FreeSurface ( tmp );

      EditorBannerRect.x = 0;
      EditorBannerRect.y = 0; 
      EditorBannerRect.w = LevelEditorTopBanner -> w;
      EditorBannerRect.h = LevelEditorTopBanner -> h;
    }
  
  SDL_BlitSurface ( LevelEditorTopBanner , NULL , Screen , &EditorBannerRect );

  for ( i = 0 ; i < 9 ; i ++ )
    {
      TargetRectangle.x = INITIAL_BLOCK_WIDTH/2 + INITIAL_BLOCK_WIDTH * i ;
      TargetRectangle.y = INITIAL_BLOCK_HEIGHT/3 ;

      if ( INITIAL_BLOCK_WIDTH == Block_Width )
	{
	  SDL_BlitSurface( MapBlockSurfacePointer[ DisplayLevel->color ][MapBrick] , NULL ,
			   Screen, &TargetRectangle);
	}
      else
	{
	  SDL_BlitSurface( BackupMapBlockSurfacePointer[ DisplayLevel->color ][ MapBrick ] , NULL ,
			   Screen, &TargetRectangle);
	}

      if ( MapBrick == Highlight ) 
	HighlightRectangle ( Screen , TargetRectangle );

      if ( MapBrick < NUM_MAP_BLOCKS -1 ) MapBrick ++ ;
    }

  ShowGenericButtonFromList ( LEFT_LEVEL_EDITOR_BUTTON ) ;
  ShowGenericButtonFromList ( RIGHT_LEVEL_EDITOR_BUTTON ) ;
  

}; // void ShowLevelEditorTopMenu( void )

/* ----------------------------------------------------------------------
 * When new lines are inserted into the map, the map inserts south of this
 * line must move too with the rest of the map.  This function sees to it.
 * ---------------------------------------------------------------------- */
void
MoveBigMapInsertsSouthOf ( int FromWhere , int ByWhat , Level EditLevel )
{
  int i;

  for ( i = 0 ; i < MAX_MAP_INSERTS_PER_LEVEL ; i ++ )
    {
      if ( EditLevel -> MapInsertList [ i ] . type == ( -1 ) ) continue;
      
      if ( EditLevel -> MapInsertList [ i ] . pos . y >= FromWhere )
	EditLevel -> MapInsertList [ i ] . pos . y += ByWhat;
    }
  
}; // void MoveBigMapInsertsSouthOf ( int FromWhere , int ByWhat , Level EditLevel)

/* ----------------------------------------------------------------------
 * When new lines are inserted into the map, the map inserts east of this
 * line must move too with the rest of the map.  This function sees to it.
 * ---------------------------------------------------------------------- */
void
MoveBigMapInsertsEastOf ( int FromWhere , int ByWhat , Level EditLevel )
{
  int i;

  for ( i = 0 ; i < MAX_MAP_INSERTS_PER_LEVEL ; i ++ )
    {
      if ( EditLevel -> MapInsertList [ i ] . type == ( -1 ) ) continue;
      
      if ( EditLevel -> MapInsertList [ i ] . pos . x >= FromWhere )
	EditLevel -> MapInsertList [ i ] . pos . x += ByWhat;
    }
  
}; // void MoveBigMapInsertsEastOf ( int FromWhere , int ByWhat , Level EditLevel)

/* ----------------------------------------------------------------------
 * When new lines are inserted into the map, the map labels south of this
 * line must move too with the rest of the map.  This function sees to it.
 * ---------------------------------------------------------------------- */
void
MoveMapLabelsSouthOf ( int FromWhere , int ByWhat , Level EditLevel )
{
  int i;

  for ( i = 0 ; i < MAX_MAP_LABELS_PER_LEVEL ; i ++ )
    {
      if ( EditLevel -> labels [ i ] . pos . x <= ( -1 ) ) continue;
      
      if ( EditLevel -> labels [ i ] . pos . y >= FromWhere )
	EditLevel -> labels [ i ] . pos . y += ByWhat;
    }
  
}; // void MoveMapLabelsSouthOf ( int FromWhere , int ByWhat , Level EditLevel)

/* ----------------------------------------------------------------------
 * When new lines are inserted into the map, the map labels east of this
 * line must move too with the rest of the map.  This function sees to it.
 * ---------------------------------------------------------------------- */
void
MoveMapLabelsEastOf ( int FromWhere , int ByWhat , Level EditLevel )
{
  int i;

  for ( i = 0 ; i < MAX_MAP_LABELS_PER_LEVEL ; i ++ )
    {
      if ( EditLevel -> labels [ i ] . pos . x <= ( -1 ) ) continue;
      
      if ( EditLevel -> labels [ i ] . pos . x >= FromWhere )
	EditLevel -> labels [ i ] . pos . x += ByWhat;
    }
  
}; // void MoveMapLabelsEastOf ( int FromWhere , int ByWhat , Level EditLevel)

/* ----------------------------------------------------------------------
 * When new lines are inserted into the map, the waypoints south of this
 * line must move too with the rest of the map.  This function sees to it.
 * ---------------------------------------------------------------------- */
void
MoveWaypointsSouthOf ( int FromWhere , int ByWhat , Level EditLevel )
{
  int i;

  for ( i = 0 ; i < MAXWAYPOINTS ; i ++ )
    {
      if ( EditLevel -> AllWaypoints [ i ] . x == ( 0 ) ) continue;
      
      if ( EditLevel -> AllWaypoints [ i ] . y >= FromWhere )
	EditLevel -> AllWaypoints [ i ] . y += ByWhat;
    }
  
}; // void MoveWaypointsSouthOf ( int FromWhere , int ByWhat , Level EditLevel)

/* ----------------------------------------------------------------------
 * When new lines are inserted into the map, the waypoints east of this
 * line must move too with the rest of the map.  This function sees to it.
 * ---------------------------------------------------------------------- */
void
MoveWaypointsEastOf ( int FromWhere , int ByWhat , Level EditLevel )
{
  int i;

  for ( i = 0 ; i < MAXWAYPOINTS ; i ++ )
    {
      if ( EditLevel -> AllWaypoints [ i ] . x == ( 0 ) ) continue;
      
      if ( EditLevel -> AllWaypoints [ i ] . x >= FromWhere )
	EditLevel -> AllWaypoints [ i ] . x += ByWhat;
    }
  
}; // void MoveWaypointsEastOf ( int FromWhere , int ByWhat , Level EditLevel)

/* ----------------------------------------------------------------------
 * Self-explanatory.
 * ---------------------------------------------------------------------- */
void
InsertLineVerySouth ( Level EditLevel )
{

  //--------------------
  // The enlargement of levels in y direction is limited by a constant
  // defined in defs.h.  This is carefully checked or no operation at
  // all will be performed.
  //
  if ( (EditLevel->ylen)+1 < MAX_MAP_LINES )
    {
      EditLevel->ylen++;
      // In case of enlargement, we need to do more:
      EditLevel->map[ EditLevel->ylen-1 ] = MyMalloc( EditLevel->xlen +1) ;
      // We don't want to fill the new area with junk, do we? So we make it floor tiles
      memset( EditLevel->map[ EditLevel->ylen-1 ] , FLOOR , EditLevel->xlen );
    }

}; // void InsertLineVerySouth ( Level EditLevel )

/* ----------------------------------------------------------------------
 * Self-explanatory.
 * ---------------------------------------------------------------------- */
void
InsertColumnVeryEast ( Level EditLevel )
{
  int i;
  char* OldMapPointer;

  EditLevel->xlen++;
  // In case of enlargement, we need to do more:
  for ( i = 0 ; i < EditLevel->ylen ; i++ )
    {
      OldMapPointer=EditLevel->map[i];
      EditLevel->map[i] = MyMalloc( EditLevel->xlen +1) ;
      memcpy( EditLevel->map[i] , OldMapPointer , EditLevel->xlen-1 );
      // We don't want to fill the new area with junk, do we? So we make it floor tiles
      EditLevel->map[ i ] [ EditLevel->xlen-1 ] = FLOOR;  
    }

}; // void InsertColumnVeryEast ( Level EditLevel )
      
/* ----------------------------------------------------------------------
 * Self-explanatory.
 * ---------------------------------------------------------------------- */
void
InsertColumnEasternInterface( Level EditLevel )
{
  int i;

  //--------------------
  // First a sanity check:  If there's no eastern threshold, this
  // must be a mistake and will just be ignored...
  //
  if ( EditLevel -> jump_threshold_east <= 0 ) return;

  //--------------------
  // We use available methods to add a column, even if in the wrong
  // place for now.
  //
  InsertColumnVeryEast ( EditLevel );

  //--------------------
  // Now the new memory and everything is done.  All we
  // need to do is move the information to the east
  //
  for ( i = 0 ; i < EditLevel->ylen ; i ++ )
    {
      //--------------------
      // REMEMBER:  WE MUST NO USE MEMCPY HERE, CAUSE THE AREAS IN QUESTION
      // MIGHT (EVEN WILL) OVERLAP!!  THAT MUST NOT BE THE CASE WITH MEMCPY!!
      //
      memmove ( & ( EditLevel->map [ i ] [ EditLevel->xlen - EditLevel->jump_threshold_east - 1 ] ) ,
		& ( EditLevel->map [ i ] [ EditLevel->xlen - EditLevel->jump_threshold_east - 2 ] ) ,
		EditLevel->jump_threshold_east );
      EditLevel->map [ i ] [ EditLevel->xlen - EditLevel->jump_threshold_east - 1 ] = FLOOR ;
    }

  MoveWaypointsEastOf ( EditLevel->xlen - EditLevel->jump_threshold_east - 1 , +1 , EditLevel ) ;
  MoveBigMapInsertsEastOf ( EditLevel->xlen - EditLevel->jump_threshold_east - 1 , +1 , EditLevel ) ;
  MoveMapLabelsEastOf ( EditLevel->xlen - EditLevel->jump_threshold_east - 1 , +1 , EditLevel ) ;

}; // void InsertColumnEasternInterface( EditLevel );

/* ----------------------------------------------------------------------
 * Self-explanatory.
 * ---------------------------------------------------------------------- */
void
RemoveColumnEasternInterface( Level EditLevel )
{
  int i;

  //--------------------
  // First a sanity check:  If there's no eastern threshold, this
  // must be a mistake and will just be ignored...
  //
  if ( EditLevel -> jump_threshold_east <= 0 ) return;

  //--------------------
  // Now the new memory and everything is done.  All we
  // need to do is move the information to the east
  //
  for ( i = 0 ; i < EditLevel->ylen ; i ++ )
    {
      //--------------------
      // REMEMBER:  WE MUST NO USE MEMCPY HERE, CAUSE THE AREAS IN QUESTION
      // MIGHT (EVEN WILL) OVERLAP!!  THAT MUST NOT BE THE CASE WITH MEMCPY!!
      //
      memmove ( & ( EditLevel->map [ i ] [ EditLevel->xlen - EditLevel->jump_threshold_east - 1 ] ) ,
		& ( EditLevel->map [ i ] [ EditLevel->xlen - EditLevel->jump_threshold_east - 0 ] ) ,
		EditLevel->jump_threshold_east - 0 );
      // EditLevel->map [ i ] [ EditLevel->xlen - EditLevel->jump_threshold_east - 1 ] = FLOOR ;
    }

  EditLevel -> xlen --;

  MoveWaypointsEastOf ( EditLevel->xlen - EditLevel->jump_threshold_east + 1 , -1 , EditLevel ) ;
  MoveBigMapInsertsEastOf ( EditLevel->xlen - EditLevel->jump_threshold_east + 1 , -1 , EditLevel ) ;
  MoveMapLabelsEastOf ( EditLevel->xlen - EditLevel->jump_threshold_east + 1 , -1 , EditLevel ) ;

}; // void InsertColumnEasternInterface( EditLevel );

/* ----------------------------------------------------------------------
 * Self-explanatory.
 * ---------------------------------------------------------------------- */
void
InsertColumnWesternInterface( Level EditLevel )
{
  int BackupOfEasternInterface;

  //--------------------
  // First a sanity check:  If there's no western threshold, this
  // must be a mistake and will just be ignored...
  //
  if ( EditLevel -> jump_threshold_west <= 0 ) return;

  //--------------------
  // Again we exploit existing code, namely the insertion procedure
  // for the eastern interface.  We shortly change the interface, use
  // that code from the eastern interface and restore the eastern interface.
  //
  BackupOfEasternInterface = EditLevel->jump_threshold_east;
  EditLevel->jump_threshold_east = EditLevel->xlen - EditLevel->jump_threshold_west ;
  InsertColumnEasternInterface ( EditLevel );
  EditLevel->jump_threshold_east = BackupOfEasternInterface ;

}; // void InsertColumnWesternInterface( Level EditLevel )

/* ----------------------------------------------------------------------
 * Self-explanatory.
 * ---------------------------------------------------------------------- */
void
RemoveColumnWesternInterface( Level EditLevel )
{
  int BackupOfEasternInterface;

  //--------------------
  // First a sanity check:  If there's no western threshold, this
  // must be a mistake and will just be ignored...
  //
  if ( EditLevel -> jump_threshold_west <= 0 ) return;

  //--------------------
  // Again we exploit existing code, namely the insertion procedure
  // for the eastern interface.  We shortly change the interface, use
  // that code from the eastern interface and restore the eastern interface.
  //
  BackupOfEasternInterface = EditLevel->jump_threshold_east;
  EditLevel->jump_threshold_east = EditLevel->xlen - EditLevel->jump_threshold_west - 1;
  RemoveColumnEasternInterface ( EditLevel );
  EditLevel->jump_threshold_east = BackupOfEasternInterface ;

}; // void RemoveColumnWesternInterface( Level EditLevel )

/* ----------------------------------------------------------------------
 * Self-Explanatory.
 * ---------------------------------------------------------------------- */
void
InsertColumnVeryWest ( Level EditLevel )
{
  int OldEasternInterface;

  //--------------------
  // We shortly change the eastern interface to reuse the code for there
  //
  OldEasternInterface = EditLevel -> jump_threshold_south;

  EditLevel -> jump_threshold_east = EditLevel -> xlen - 0 ;
  InsertColumnEasternInterface ( EditLevel );

  EditLevel -> jump_threshold_east = OldEasternInterface ;

}; // void InsertColumnVeryWest ( EditLevel )

/* ----------------------------------------------------------------------
 * Self-Explanatory.
 * ---------------------------------------------------------------------- */
void
RemoveColumnVeryWest ( Level EditLevel )
{
  int OldEasternInterface;

  //--------------------
  // We shortly change the eastern interface to reuse the code for there
  //
  OldEasternInterface = EditLevel -> jump_threshold_east;

  EditLevel -> jump_threshold_east = EditLevel -> xlen - 1 ;
  RemoveColumnEasternInterface ( EditLevel );

  EditLevel -> jump_threshold_east = OldEasternInterface ;

}; // void RemoveColumnVeryEast ( Level EditLevel )

/* ----------------------------------------------------------------------
 * Self-Explanatory.
 * ---------------------------------------------------------------------- */
void
InsertLineSouthernInterface ( Level EditLevel )
{
  char* temp;
  int i;

  //--------------------
  // First a sanity check for existing interface
  //
  if ( EditLevel -> jump_threshold_south <= 0 ) return;

  //--------------------
  // We build upon the existing code again.
  //
  InsertLineVerySouth( EditLevel );
  
  //--------------------
  // Now we do some swapping of lines
  //
  temp = EditLevel -> map [ EditLevel -> ylen - 1 ] ;

  for ( i = 0 ; i < EditLevel -> jump_threshold_south ; i ++ )
    {
      EditLevel -> map [ EditLevel -> ylen - i - 1 ] = 
	EditLevel -> map [ EditLevel -> ylen - i - 2 ] ;
    }
  EditLevel -> map [ EditLevel -> ylen - 1 - EditLevel -> jump_threshold_south ] = temp ;

  //--------------------
  // Now we have the waypoints moved as well
  //
  MoveWaypointsSouthOf ( EditLevel -> ylen - 1 - EditLevel -> jump_threshold_south , +1 , EditLevel ) ;
  MoveBigMapInsertsSouthOf ( EditLevel -> ylen - 1 - EditLevel -> jump_threshold_south , +1 , EditLevel ) ;
  MoveMapLabelsSouthOf ( EditLevel -> ylen - 1 - EditLevel -> jump_threshold_south , +1 , EditLevel ) ;

}; // void InsertLineSouthernInterface ( EditLevel )

/* ----------------------------------------------------------------------
 * Self-Explanatory.
 * ---------------------------------------------------------------------- */
void
RemoveLineSouthernInterface ( Level EditLevel )
{
  int i;

  //--------------------
  // First a sanity check for existing interface
  //
  if ( EditLevel -> jump_threshold_south <= 0 ) return;

  //--------------------
  // Now we do some swapping of lines
  //
  for ( i = EditLevel -> ylen - 1 - EditLevel -> jump_threshold_south ; 
	i < EditLevel -> ylen - 1 ; i ++ )
    {
      EditLevel -> map [ i ] = EditLevel -> map [ i + 1 ] ;
    }
  EditLevel -> ylen -- ;

  //--------------------
  // Now we have the waypoints moved as well
  //
  MoveWaypointsSouthOf ( EditLevel -> ylen - 0 - EditLevel -> jump_threshold_south , -1 , EditLevel ) ;
  MoveBigMapInsertsSouthOf ( EditLevel -> ylen - 0 - EditLevel -> jump_threshold_south , -1 , EditLevel ) ;
  MoveMapLabelsSouthOf ( EditLevel -> ylen - 0 - EditLevel -> jump_threshold_south , -1 , EditLevel ) ;

}; // void RemoveLineSouthernInterface ( EditLevel )

/* ----------------------------------------------------------------------
 * Self-Explanatory.
 * ---------------------------------------------------------------------- */
void
InsertLineNorthernInterface ( Level EditLevel )
{
  int OldSouthernInterface;

  //--------------------
  // First a sanity check for existing interface
  //
  if ( EditLevel -> jump_threshold_north <= 0 ) return;

  //--------------------
  // We shortly change the southern interface to reuse the code for there
  //
  OldSouthernInterface = EditLevel -> jump_threshold_south;

  EditLevel -> jump_threshold_south = EditLevel -> ylen - EditLevel -> jump_threshold_north - 0 ;
  InsertLineSouthernInterface ( EditLevel );

  EditLevel -> jump_threshold_south = OldSouthernInterface ;

}; // void InsertLineNorthernInterface ( EditLevel )

/* ----------------------------------------------------------------------
 * Self-Explanatory.
 * ---------------------------------------------------------------------- */
void
RemoveLineNorthernInterface ( Level EditLevel )
{
  int OldSouthernInterface;

  //--------------------
  // First a sanity check for existing interface
  //
  if ( EditLevel -> jump_threshold_north <= 0 ) return;

  //--------------------
  // We shortly change the southern interface to reuse the code for there
  //
  OldSouthernInterface = EditLevel -> jump_threshold_south;

  EditLevel -> jump_threshold_south = EditLevel -> ylen - EditLevel -> jump_threshold_north - 1 ;
  RemoveLineSouthernInterface ( EditLevel );

  EditLevel -> jump_threshold_south = OldSouthernInterface ;

}; // void RemoveLineNorthernInterface ( Level EditLevel )

/* ----------------------------------------------------------------------
 * Self-Explanatory.
 * ---------------------------------------------------------------------- */
void
InsertLineVeryNorth ( Level EditLevel )
{
  int OldSouthernInterface;

  //--------------------
  // We shortly change the southern interface to reuse the code for there
  //
  OldSouthernInterface = EditLevel -> jump_threshold_south;

  EditLevel -> jump_threshold_south = EditLevel -> ylen - 0 ;
  InsertLineSouthernInterface ( EditLevel );

  EditLevel -> jump_threshold_south = OldSouthernInterface ;

}; // void InsertLineVeryNorth ( EditLevel )

/* ----------------------------------------------------------------------
 * Self-Explanatory.
 * ---------------------------------------------------------------------- */
void
RemoveLineVeryNorth ( Level EditLevel )
{
  int OldSouthernInterface;

  //--------------------
  // We shortly change the southern interface to reuse the code for there
  //
  OldSouthernInterface = EditLevel -> jump_threshold_south;

  EditLevel -> jump_threshold_south = EditLevel -> ylen - 1 ;
  RemoveLineSouthernInterface ( EditLevel );

  EditLevel -> jump_threshold_south = OldSouthernInterface ;

}; // void RemoveLineVeryNorth ( Level EditLevel )

/* ----------------------------------------------------------------------
 * This a a menu interface to allow to edit the level dimensions in a
 * convenient way, i.e. so that little stupid copying work or things like
 * that have to be done and more time can be spent creating game material.
 * ---------------------------------------------------------------------- */
void
EditLevelDimensions ( void )
{
  char* MenuTexts[ 20 ];
  char Options [ 20 ] [1000];
  int MenuPosition = 1 ;
  int Weiter = FALSE ;
  Level EditLevel;

  EditLevel = curShip.AllLevels [ Me [ 0 ] . pos . z ] ;

  while ( !Weiter )
    {

      InitiateMenu( NULL );
      
      MenuTexts[ 0 ] = "Insert/Remove column to the very west" ;
      MenuTexts[ 1 ] = "Insert/Remove column just east of western Interface" ;
      MenuTexts[ 2 ] = "Insert/Remove column just west of eastern Interface" ;
      MenuTexts[ 3 ] = "Insert/Remove column to the very east" ;

      MenuTexts[ 4 ] = "Insert/Remove line to the very north" ;
      MenuTexts[ 5 ] = "Insert/Remove line just south of northern Interface" ;
      MenuTexts[ 6 ] = "Insert/Remove line just north of southern Interface" ;
      MenuTexts[ 7 ] = "Insert/Remove line to the very south" ;
      
      sprintf( Options [ 0 ] , "Current level size in X: %d." , EditLevel->xlen );
      MenuTexts[ 8 ] = Options [ 0 ];
      sprintf( Options [ 1 ] , "Current level size in Y: %d." , EditLevel->ylen  );
      MenuTexts[ 9 ] = Options [ 1 ] ;

      MenuTexts[ 10 ] = "Back To Level Editor Main Menu" ;
      MenuTexts[ 11 ] = "" ;
      
      MenuPosition = DoMenuSelection( "" , MenuTexts , -1 , NULL , FPS_Display_BFont );
      
      while (EnterPressed() || SpacePressed() );
      
      switch (MenuPosition) 
	{
	case INSERTREMOVE_COLUMN_VERY_EAST:
	  if ( RightPressed() )
	    {
	      InsertColumnVeryEast( EditLevel );
	      while (RightPressed());
	    }
	  if ( LeftPressed() )
	    {
	      EditLevel->xlen--; // making it smaller is always easy:  just modify the value for size
	      // allocation of new memory or things like that are not nescessary
	      while (LeftPressed());
	    }
	  break;

	case INSERTREMOVE_COLUMN_EASTERN_INTERFACE:
	  if ( RightPressed() )
	    {
	      InsertColumnEasternInterface( EditLevel );
	      while (RightPressed());
	    }
	  if ( LeftPressed() )
	    {
	      RemoveColumnEasternInterface( EditLevel );
	      while (LeftPressed());
	    }
	  break;

	case INSERTREMOVE_COLUMN_WESTERN_INTERFACE:
	  if ( RightPressed() )
	    {
	      InsertColumnWesternInterface( EditLevel );
	      while (RightPressed());
	    }
	  if ( LeftPressed() )
	    {
	      RemoveColumnWesternInterface( EditLevel );
	      while (LeftPressed());
	    }
	  break;
	  
	case INSERTREMOVE_COLUMN_VERY_WEST:
	  if ( RightPressed() )
	    {
	      InsertColumnVeryWest ( EditLevel );
	      while (RightPressed());
	    }
	  if ( LeftPressed() )
	    {
	      RemoveColumnVeryWest ( EditLevel );
	      while (LeftPressed());
	    }
	  break;

	case INSERTREMOVE_LINE_VERY_SOUTH:
	  if ( RightPressed() )
	    {
	      InsertLineVerySouth ( EditLevel );
	      while (RightPressed());
	    }
	  
	  if ( LeftPressed() )
	    {
	      EditLevel->ylen--; // making it smaller is always easy:  just modify the value for size
	      // allocation of new memory or things like that are not nescessary.
	      while (LeftPressed());
	    }
	  break;

	case INSERTREMOVE_LINE_SOUTHERN_INTERFACE:
	  if ( RightPressed() )
	    {
	      InsertLineSouthernInterface ( EditLevel );
	      while (RightPressed());
	    }
	  if ( LeftPressed() )
	    {
	      RemoveLineSouthernInterface ( EditLevel );
	      while (LeftPressed());
	    }
	  break;

	case INSERTREMOVE_LINE_NORTHERN_INTERFACE:
	  if ( RightPressed() )
	    {
	      InsertLineNorthernInterface ( EditLevel );
	      while (RightPressed());
	    }
	  if ( LeftPressed() )
	    {
	      RemoveLineNorthernInterface ( EditLevel );
	      while (LeftPressed());
	    }
	  break;

	case INSERTREMOVE_LINE_VERY_NORTH:
	  if ( RightPressed() )
	    {
	      InsertLineVeryNorth ( EditLevel );
	      while (RightPressed());
	    }
	  if ( LeftPressed() )
	    {
	      RemoveLineVeryNorth ( EditLevel );
	      while (LeftPressed());
	    }
	  break;

	case (-1):
	case BACK_TO_LE_MAIN_MENU:
	  while (EnterPressed() || SpacePressed() || EscapePressed() ) ;
	  GetAllAnimatedMapTiles ( EditLevel );
	  Weiter=!Weiter;
	  break;

	default: 
	  break;

	}

    } // while (!Weiter)
    
}; // void EditLevelDimensions ( void )
  
/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
int
DoLevelEditorMainMenu ( Level EditLevel )
{
  char* MenuTexts[ 20 ];
  char Options [ 20 ] [1000];
  int Weiter = FALSE ;
  int MenuPosition=1;
  int Done=FALSE;
  // int i;
  // char* OldMapPointer;

  while (!Weiter)
    {
      
      EditLevel = curShip.AllLevels [ Me [ 0 ] . pos . z ] ;

      InitiateMenu( NULL );
      
      MenuTexts[ 0 ] = "Save whole ship to 'Testship.shp'" ;
      sprintf( Options [ 0 ] , "Current: %d.  Level Up/Down" , EditLevel->levelnum );
      MenuTexts[ 1 ] = Options [ 0 ];
      MenuTexts[ 2 ] = "Change tile set" ;
      sprintf( Options [ 1 ] , "Current levelsize: %d x %d map tiles." , EditLevel->xlen , EditLevel->ylen );
      MenuTexts[ 3 ] = Options [ 1 ];
      sprintf( Options [ 2 ] , " --- UNUSED --- " );
      MenuTexts[ 4 ] = Options [ 2 ] ;
      sprintf( Options [ 3 ] , "Level name: %s" , EditLevel->Levelname );
      MenuTexts[ 5 ] = Options [ 3 ] ;
      sprintf( Options [ 4 ] , "Background music file name: %s" , EditLevel->Background_Song_Name );
      MenuTexts[ 6 ] = Options [ 4 ] ;
      sprintf( Options [ 5 ] , "Set Level Comment: %s" , EditLevel->Level_Enter_Comment );
      MenuTexts[ 7 ] = Options [ 5 ] ;
      MenuTexts[ 8 ] = "Add completely new level" ; 
      MenuTexts[ 9 ] = "Set Level Interfaces" ;
      MenuTexts[ 10 ] = "Edit Level Dimensions" ;
      MenuTexts[ 11 ] = "Quit Level Editor" ;
      MenuTexts[ 12 ] = "" ;
	  


      MenuPosition = DoMenuSelection( "" , MenuTexts , -1 , NULL , FPS_Display_BFont );
      
      while (EnterPressed() || SpacePressed() );
      
      switch (MenuPosition) 
	{
	  
	case (-1):
	  while ( EscapePressed() );
	  Weiter=!Weiter;
	  if ( CurrentCombatScaleFactor != 1 ) SetCombatScaleTo( 1 );
	  break;
	case SAVE_LEVEL_POSITION:
	  while (EnterPressed() || SpacePressed() ) ;
	  SaveShip("Testship.shp");
	  CenteredPutString ( Screen ,  11*FontHeight(Menu_BFont),    "Your ship was saved...");
	  SDL_Flip ( Screen );
	  while (!EnterPressed() && !SpacePressed() ) ;
	  while (EnterPressed() || SpacePressed() ) ;
	  // Weiter=!Weiter;
	  break;
	case CHANGE_LEVEL_POSITION: 
	  // if ( EditLevel->levelnum ) Teleport ( EditLevel->levelnum-1 , Me[0].pos.x , Me[0].pos.y ); 
	  while (EnterPressed() || SpacePressed() ) ;
	  break;
	case CHANGE_TILE_SET_POSITION: 
	  while (EnterPressed() || SpacePressed() ) ;
	  break;
	case SET_LEVEL_NAME:
	  while (EnterPressed() || SpacePressed() ) ;
	  EditLevel->Levelname = 
	    GetEditableStringInPopupWindow ( 1000 , "\n Please enter new level name: \n\n" ,
					     EditLevel->Levelname );
	  Weiter=!Weiter;
	  break;
	case SET_BACKGROUND_SONG_NAME:
	  while (EnterPressed() || SpacePressed() ) ;
	  EditLevel->Background_Song_Name = 
	    GetEditableStringInPopupWindow ( 1000 , "\n Please enter new music file name: \n\n" ,
					     EditLevel->Background_Song_Name );
	  Weiter=!Weiter;
	  break;
	case SET_LEVEL_COMMENT:
	  while (EnterPressed() || SpacePressed() ) ;
	  EditLevel->Level_Enter_Comment = 
	    GetEditableStringInPopupWindow ( 1000 , "\n Please enter new level comment: \n\n" ,
					     EditLevel->Level_Enter_Comment );
	  Weiter=!Weiter;
	  break;
	case ADD_NEW_LEVEL:
	  while (EnterPressed() || SpacePressed() ) ;
	  curShip.AllLevels[ curShip.num_levels ] = CreateNewMapLevel();
	  curShip.num_levels ++ ;
	  CenteredPutString ( Screen ,  12*FontHeight(FPS_Display_BFont), "New level has been added!");
	  SDL_Flip( Screen );
	  while (!SpacePressed() && !EnterPressed() );
	  while (EnterPressed() || SpacePressed() ) ;
	  SetTextCursor( 15 , 440 );
	  Weiter=!Weiter;
	  break;
	case SET_LEVEL_INTERFACE_POSITION:
	  while (EnterPressed() || SpacePressed() ) ;
	  // Weiter=!Weiter;
	  SetLevelInterfaces ( );
	  break;
	case EDIT_LEVEL_DIMENSIONS:
	  while (EnterPressed() || SpacePressed() ) ;
	  // Weiter=!Weiter;
	  EditLevelDimensions ( );
	  break;
	case QUIT_LEVEL_EDITOR_POSITION:
	  while (EnterPressed() || SpacePressed() ) ;
	  Weiter=!Weiter;
	  Done=TRUE;
	  SetCombatScaleTo( 1 );
	  break;
	default: 
	  break;
	  
	} // switch
      
	  // If the user of the level editor pressed left or right, that should have
	  // an effect IF he/she is a the change level menu point

      if (LeftPressed() || RightPressed() ) 
	{
	  switch (MenuPosition)
	    {
	      
	    case CHANGE_LEVEL_POSITION:
	      if ( LeftPressed() )
		{
		  if ( EditLevel->levelnum > 0 )
		    Teleport ( EditLevel->levelnum -1 , 3 , 3 , 0 , TRUE , FALSE );
		  while (LeftPressed());
		}
	      if ( RightPressed() )
		{
		  if ( EditLevel->levelnum < curShip.num_levels -1 )
		    Teleport ( EditLevel->levelnum +1 , 3 , 3 , 0 , TRUE , FALSE );
		  while (RightPressed());
		}
	      if ( CurrentCombatScaleFactor != 1 ) SetCombatScaleTo ( CurrentCombatScaleFactor );
	      break;
	      
	    case CHANGE_TILE_SET_POSITION:
	      if ( RightPressed() && (EditLevel->color  < 6 ) )
		{
		  EditLevel->color++;
		  while (RightPressed());
		}
	      if ( LeftPressed() && (EditLevel->color > 0) )
		{
		  EditLevel->color--;
		  while (LeftPressed());
		}
	      Teleport ( EditLevel->levelnum , Me[0].pos.x , Me[0].pos.y , 0 , TRUE , FALSE ); 
	      break;

	    case CHANGE_SIZE_X:
	      break;
	    case CHANGE_SIZE_Y:

	      break;
	      
	    }
	} // if LeftPressed || RightPressed
      
    }

  return ( Done );

}; // void DoLevelEditorMainMenu ( Level EditLevel );

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void 
ShowLevelEditorKeymap ( void )
{
  int k=1;

  DisplayImage (find_file ( NE_CONSOLE_BG_PIC1_FILE, GRAPHICS_DIR, FALSE));
#define KeymapOffset 15
  CenteredPutString   ( Screen ,  (k)*FontHeight(Menu_BFont), "Level Editor Keymap"); k+=1;
  PutString ( Screen , KeymapOffset , (k) * FontHeight(Menu_BFont)  , "Use cursor keys to move around." ); k++;
  PutString ( Screen , KeymapOffset , (k) * FontHeight(Menu_BFont)  , "Use number pad to plant walls." ); k++;
  PutString ( Screen , KeymapOffset , (k) * FontHeight(Menu_BFont)  , "Use SHIFT and number pad to plant extras." ); k++;
  PutString ( Screen , KeymapOffset , (k) * FontHeight(Menu_BFont)  , "Use CTRL and number pad for more extras." ); k++;
  PutString ( Screen , KeymapOffset , (k) * FontHeight(Menu_BFont)  , "R...Refresh, 1-5...Blocktype 1-5, L...Lift, F...Fine grid" ); k++;
  PutString ( Screen , KeymapOffset , (k) * FontHeight(Menu_BFont)  , "D/SHIFT+D/CTRL+D...(Locked) Doors, A...Alert" ); k++;
  PutString ( Screen , KeymapOffset , (k) * FontHeight(Menu_BFont)  , "E...Enter tile by number, Space/Enter...Floor" ); k++; 
  
  PutString ( Screen , KeymapOffset , (k) * FontHeight(Menu_BFont)  , "I/O...zoom INTO/OUT OF the map" ); k+=1;
  PutString ( Screen , KeymapOffset , (k) * FontHeight(Menu_BFont)  , "W...toggle wayPOINT on/off" ); k++;
  PutString ( Screen , KeymapOffset , (k) * FontHeight(Menu_BFont)  , "C...start/end waypoint CONNECTION" ); k+=2;
  PutString ( Screen , KeymapOffset , (k) * FontHeight(Menu_BFont)  , "For more keys see our home page" ); k++;
  
  SDL_Flip ( Screen );

  while ( SpacePressed() || EscapePressed() );
  while ( !SpacePressed() && !EscapePressed() );
  while ( SpacePressed() || EscapePressed() );
  
}; // void ShowLevelEditorKeymap ( void )

/* ----------------------------------------------------------------------
 * The levels in Freedroid may be connected into one big map by simply
 * 'gluing' then together, i.e. we define some interface areas to the
 * sides of a map and when the Tux passes these areas, he'll be silently
 * put into another map without much fuss.  This operation is performed
 * silently and the two maps must be synchronized in this interface area
 * so the map change doesn't become apparend to the player.  Part of this
 * synchronisation, namely copying the map tiles to the other map, is 
 * done automatically, but some inconsistencies like non-matching map
 * sizes or non-symmetric jump directions (i.e. not back and forth but
 * back and forth-to-somewhere else) are not resolved automatically.
 * Instead, a report on inconsistencies will be created and the person
 * editing the map can then resolve the inconsistencies manually in one
 * fashion or the other.
 * ---------------------------------------------------------------------- */
void
ReportInconsistenciesForLevel ( int LevelNum )
{
  int TargetLevel;
  SDL_Rect ReportRect;

  ReportRect.x = 20;
  ReportRect.y = 20;
  ReportRect.w = 600;
  ReportRect.h = 440;

  AssembleCombatPicture ( ONLY_SHOW_MAP_AND_TEXT | SHOW_GRID );

  DisplayText ( "\nThe list of inconsistencies of the jump interfaces for this level:\n\n" ,
		ReportRect.x, ReportRect.y + FontHeight ( GetCurrentFont () ) , &ReportRect);

  //--------------------
  // First we test for inconsistencies of back-forth ways, i.e. if the transit
  // in one direction will lead back in the right direction when returning.
  //
  if ( curShip.AllLevels [ LevelNum ] -> jump_target_north != (-1) ) 
    {
      TargetLevel = curShip.AllLevels [ LevelNum ] -> jump_target_north ;
      if ( curShip.AllLevels [ TargetLevel ] -> jump_target_south != LevelNum )
	{
	  DisplayText ( "BACK-FORTH-MISMATCH: North doesn't lead back here (yet)!\n" ,
			-1 , -1 , &ReportRect);
	}
    }
  if ( curShip.AllLevels [ LevelNum ] -> jump_target_south != (-1) ) 
    {
      TargetLevel = curShip.AllLevels [ LevelNum ] -> jump_target_south ;
      if ( curShip.AllLevels [ TargetLevel ] -> jump_target_north != LevelNum )
	{
	  DisplayText ( "BACK-FORTH-MISMATCH: South doesn't lead back here (yet)!\n" ,
			-1 , -1 , &ReportRect);
	}
    }
  if ( curShip.AllLevels [ LevelNum ] -> jump_target_east != (-1) ) 
    {
      TargetLevel = curShip.AllLevels [ LevelNum ] -> jump_target_east ;
      if ( curShip.AllLevels [ TargetLevel ] -> jump_target_west != LevelNum )
	{
	  DisplayText ( "BACK-FORTH-MISMATCH: East doesn't lead back here (yet)!\n" ,
			-1 , -1 , &ReportRect);
	}
    }
  if ( curShip.AllLevels [ LevelNum ] -> jump_target_west != (-1) ) 
    {
      TargetLevel = curShip.AllLevels [ LevelNum ] -> jump_target_west ;
      if ( curShip.AllLevels [ TargetLevel ] -> jump_target_east != LevelNum )
	{
	  DisplayText ( "BACK-FORTH-MISMATCH: West doesn't lead back here (yet)!\n" ,
			-1 , -1 , &ReportRect);
	}
    }
  DisplayText ( "\nNO OTHER BACK-FORTH-MISMATCH ERRORS other than those listed above\n\n" ,
		-1 , -1 , &ReportRect);

  //--------------------
  // Now we test for inconsistencies of interface sizes, i.e. if the interface source level
  // has an interface as large as the target interface level.
  //
  if ( curShip.AllLevels [ LevelNum ] -> jump_target_north != (-1) ) 
    {
      TargetLevel = curShip.AllLevels [ LevelNum ] -> jump_target_north ;
      if ( curShip.AllLevels [ TargetLevel ] -> jump_threshold_south != 
	   curShip.AllLevels [ LevelNum ] -> jump_threshold_north )
	{
	  DisplayText ( "INTERFACE SIZE MISMATCH: North doesn't lead so same-sized interface level!!!\n" ,
			-1 , -1 , &ReportRect);
	}
    }
  if ( curShip.AllLevels [ LevelNum ] -> jump_target_south != (-1) ) 
    {
      TargetLevel = curShip.AllLevels [ LevelNum ] -> jump_target_south ;
      if ( curShip.AllLevels [ TargetLevel ] -> jump_threshold_north != 
	   curShip.AllLevels [ LevelNum ] -> jump_threshold_south )
	{
	  DisplayText ( "INTERFACE SIZE MISMATCH: South doesn't lead so same-sized interface level!!!\n" ,
			-1 , -1 , &ReportRect);
	}
    }
  if ( curShip.AllLevels [ LevelNum ] -> jump_target_east != (-1) ) 
    {
      TargetLevel = curShip.AllLevels [ LevelNum ] -> jump_target_east ;
      if ( curShip.AllLevels [ TargetLevel ] -> jump_threshold_west != 
	   curShip.AllLevels [ LevelNum ] -> jump_threshold_east )
	{
	  DisplayText ( "INTERFACE SIZE MISMATCH: East doesn't lead so same-sized interface level!!!\n" ,
			-1 , -1 , &ReportRect);
	}
    }
  if ( curShip.AllLevels [ LevelNum ] -> jump_target_west != (-1) ) 
    {
      TargetLevel = curShip.AllLevels [ LevelNum ] -> jump_target_west ;
      if ( curShip.AllLevels [ TargetLevel ] -> jump_threshold_east != 
	   curShip.AllLevels [ LevelNum ] -> jump_threshold_west )
	{
	  DisplayText ( "INTERFACE SIZE MISMATCH: West doesn't lead so same-sized interface level!!!\n" ,
			-1 , -1 , &ReportRect);
	}
    }

  //--------------------
  // Now we test for inconsistencies of level sizes, i.e. if the interface source level
  // has the same relevant dimension like the target interface level.
  //
  if ( curShip.AllLevels [ LevelNum ] -> jump_target_north != (-1) ) 
    {
      TargetLevel = curShip.AllLevels [ LevelNum ] -> jump_target_north ;
      if ( curShip.AllLevels [ TargetLevel ] -> xlen != curShip.AllLevels [ LevelNum ] -> xlen )
	{
	  DisplayText ( "LEVEL DIMENSION MISMATCH: North doesn't lead so same-sized level (non-fatal, but no good comes from this)!\n" ,
			-1 , -1 , &ReportRect);
	}
    }
  if ( curShip.AllLevels [ LevelNum ] -> jump_target_south != (-1) ) 
    {
      TargetLevel = curShip.AllLevels [ LevelNum ] -> jump_target_south ;
      if ( curShip.AllLevels [ TargetLevel ] -> xlen != curShip.AllLevels [ LevelNum ] -> xlen )
	{
	  DisplayText ( "LEVEL DIMENSION MISMATCH: South doesn't lead so same-sized level (non-fatal, but no good comes from this)!\n" ,
			-1 , -1 , &ReportRect);
	}
    }
  if ( curShip.AllLevels [ LevelNum ] -> jump_target_east != (-1) ) 
    {
      TargetLevel = curShip.AllLevels [ LevelNum ] -> jump_target_east ;
      if ( curShip.AllLevels [ TargetLevel ] -> ylen != curShip.AllLevels [ LevelNum ] -> ylen )
	{
	  DisplayText ( "LEVEL DIMENSION MISMATCH: East doesn't lead so same-sized level (non-fatal, but no good comes from this)!\n" ,
			-1 , -1 , &ReportRect);
	}
    }
  if ( curShip.AllLevels [ LevelNum ] -> jump_target_west != (-1) ) 
    {
      TargetLevel = curShip.AllLevels [ LevelNum ] -> jump_target_west ;
      if ( curShip.AllLevels [ TargetLevel ] -> ylen != curShip.AllLevels [ LevelNum ] -> ylen )
	{
	  DisplayText ( "LEVEL DIMENSION MISMATCH: West doesn't lead so same-sized level (non-fatal, but no good comes from this)!\n" ,
			-1 , -1 , &ReportRect);
	}
    }
  
  //--------------------
  // This was it.  We can say so and return.
  //
  DisplayText ( "\n\n--- End of List --- Press Space to return to menu ---\n" ,
		-1 , -1 , &ReportRect);
  
  SDL_Flip ( Screen );

}; // void ReportInconsistenciesForLevel ( int LevelNum )

/* ----------------------------------------------------------------------
 * When we connect two maps smoothly together, we want an area in both
 * maps, that is really synchronized with the other level we connect to.
 * But this isn't a task that should be done manually.  We rather make
 * a function, that does this synchronisation work, overwriting the 
 * higher level number with the data from the lower level number.
 * ---------------------------------------------------------------------- */
void
ExportLevelInterface ( int LevelNum )
{

  int AreaWidth;
  int AreaHeight;
  int TargetLevel;
  int y;
  int TargetStartLine;


  //--------------------
  // First we see if we need to copy the northern interface region
  // into another map.
  //
  TargetLevel = curShip.AllLevels [ LevelNum ] -> jump_target_north ;
  if ( TargetLevel != (-1) ) 
    {
      //--------------------
      // First we find out the dimensions of the area we want to copy
      //
      if ( curShip . AllLevels [ LevelNum ] -> xlen < curShip . AllLevels [ TargetLevel ] -> xlen )
	AreaWidth = curShip . AllLevels [ LevelNum ] -> xlen;
      else 
	AreaWidth = curShip . AllLevels [ TargetLevel ] -> xlen ;
      
      AreaHeight = curShip . AllLevels [ LevelNum ] -> jump_threshold_north;
      
      if ( AreaHeight <= 0 ) return;
      
      TargetStartLine = ( curShip . AllLevels [ TargetLevel ] -> ylen ) - 1 ;

      //--------------------
      // Now we can start to make the copy...
      //
      for ( y = 0 ; y < AreaHeight ; y ++ )
	{
	  memcpy ( curShip . AllLevels [ TargetLevel ] -> map[ TargetStartLine - y ] ,
		   curShip . AllLevels [ LevelNum ] -> map[ AreaHeight-1 - y ] ,
		   AreaWidth ) ;
	}
      
      GetAllAnimatedMapTiles ( curShip . AllLevels [ TargetLevel ] );
    }

  //--------------------
  // Now we see if we need to copy the southern interface region
  // into another map.
  //
  TargetLevel = curShip.AllLevels [ LevelNum ] -> jump_target_south ;
  if ( TargetLevel != (-1) ) 
    {
      //--------------------
      // First we find out the dimensions of the area we want to copy
      //
      if ( curShip . AllLevels [ LevelNum ] -> xlen < curShip . AllLevels [ TargetLevel ] -> xlen )
	AreaWidth = curShip . AllLevels [ LevelNum ] -> xlen;
      else 
	AreaWidth = curShip . AllLevels [ TargetLevel ] -> xlen ;
      
      AreaHeight = curShip . AllLevels [ LevelNum ] -> jump_threshold_south;
      
      if ( AreaHeight <= 0 ) return;
      
      TargetStartLine = ( curShip . AllLevels [ LevelNum ] -> ylen ) - 1 ;

      //--------------------
      // Now we can start to make the copy...
      //
      for ( y = 0 ; y < AreaHeight ; y ++ )
	{
	  memcpy ( curShip . AllLevels [ TargetLevel ] -> map[ AreaHeight-1 - y ] ,
		   curShip . AllLevels [ LevelNum ] -> map[ TargetStartLine - y ] ,
		   AreaWidth ) ;
	}
      GetAllAnimatedMapTiles ( curShip . AllLevels [ TargetLevel ] );
    }

  //--------------------
  // Now we see if we need to copy the eastern interface region
  // into another map.
  //
  TargetLevel = curShip.AllLevels [ LevelNum ] -> jump_target_east ;
  if ( TargetLevel != (-1) ) 
    {
      //--------------------
      // First we find out the dimensions of the area we want to copy
      //
      if ( curShip . AllLevels [ LevelNum ] -> ylen < curShip . AllLevels [ TargetLevel ] -> ylen )
	AreaHeight = curShip . AllLevels [ LevelNum ] -> ylen;
      else 
	AreaHeight = curShip . AllLevels [ TargetLevel ] -> ylen ;
      
      AreaWidth = curShip . AllLevels [ LevelNum ] -> jump_threshold_east;
      
      if ( AreaWidth <= 0 ) return;
      
      TargetStartLine = ( curShip . AllLevels [ TargetLevel ] -> ylen ) - 1 ;

      //--------------------
      // Now we can start to make the copy...
      //
      for ( y = 0 ; y < AreaHeight ; y ++ )
	{
	  memcpy ( curShip . AllLevels [ TargetLevel ] -> map[ y ] ,
		   (curShip . AllLevels [ LevelNum ] -> map[ y ]) + 
		    curShip . AllLevels [ LevelNum ] -> xlen - 0 - AreaWidth ,
		   AreaWidth ) ;
	}
      GetAllAnimatedMapTiles ( curShip . AllLevels [ TargetLevel ] );
    }

  //--------------------
  // Now we see if we need to copy the western interface region
  // into another map.
  //
  TargetLevel = curShip.AllLevels [ LevelNum ] -> jump_target_west ;
  if ( TargetLevel != (-1) ) 
    {
      //--------------------
      // First we find out the dimensions of the area we want to copy
      //
      if ( curShip . AllLevels [ LevelNum ] -> ylen < curShip . AllLevels [ TargetLevel ] -> ylen )
	AreaHeight = curShip . AllLevels [ LevelNum ] -> ylen;
      else 
	AreaHeight = curShip . AllLevels [ TargetLevel ] -> ylen ;
      
      AreaWidth = curShip . AllLevels [ LevelNum ] -> jump_threshold_west;
      
      if ( AreaWidth <= 0 ) return;
      
      TargetStartLine = ( curShip . AllLevels [ TargetLevel ] -> ylen ) - 1 ;

      //--------------------
      // Now we can start to make the copy...
      //
      for ( y = 0 ; y < AreaHeight ; y ++ )
	{
	  memcpy ( ( curShip . AllLevels [ TargetLevel ] -> map[ y ] ) + 
		   curShip . AllLevels [ TargetLevel ] -> xlen - 0 - AreaWidth,
		   ( curShip . AllLevels [ LevelNum ] -> map[ y ] ) + 0 , 
		   AreaWidth ) ;
	}
      GetAllAnimatedMapTiles ( curShip . AllLevels [ TargetLevel ] );
    }

}; // void SynchronizeLevelInterfaces ( void )
      
/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
SetLevelInterfaces ( void )
{
  char *MenuTexts[ 100 ];
  int Weiter = FALSE;
  int MenuPosition = 1 ;
  char Options [ 20 ] [ 500 ] ;
  Level EditLevel;

  EditLevel = curShip.AllLevels [ Me [ 0 ] . pos . z ] ;

  while (!Weiter)
    {
      
      EditLevel = curShip.AllLevels [ Me [ 0 ] . pos . z ] ;

      InitiateMenu( NULL );
      
      sprintf( Options [ 0 ] , "Jump threshold north: %d.  Up/Down" , EditLevel->jump_threshold_north );
      MenuTexts [ 0 ] = Options [ 0 ] ;
      sprintf( Options [ 1 ] , "Jump threshold south: %d.  Up/Down" , EditLevel->jump_threshold_south );
      MenuTexts [ 1 ] = Options [ 1 ] ;
      sprintf( Options [ 2 ] , "Jump threshold east: %d.  Up/Down" , EditLevel->jump_threshold_east );
      MenuTexts [ 2 ] = Options [ 2 ] ;
      sprintf( Options [ 3 ] , "Jump threshold west: %d.  Up/Down" , EditLevel->jump_threshold_west );
      MenuTexts [ 3 ] = Options [ 3 ] ;
      sprintf( Options [ 4 ] , "Jump target north: %d.  Up/Down" , EditLevel->jump_target_north );
      MenuTexts [ 4 ] = Options [ 4 ] ;
      sprintf( Options [ 5 ] , "Jump target south: %d.  Up/Down" , EditLevel->jump_target_south );
      MenuTexts [ 5 ] = Options [ 5 ] ;
      sprintf( Options [ 6 ] , "Jump target east: %d.  Up/Down" , EditLevel->jump_target_east );
      MenuTexts [ 6 ] = Options [ 6 ] ;
      sprintf( Options [ 7 ] , "Jump target west: %d.  Up/Down" , EditLevel->jump_target_west );
      MenuTexts [ 7 ] = Options [ 7 ] ;
      MenuTexts [ 8 ] = "Export this level to other target levels" ;
      MenuTexts [ 9 ] = "Report interface inconsistencies";
      MenuTexts [ 10 ] = "Quit Threshold Editor" ;
      MenuTexts [ 11 ] = "" ;

      MenuPosition = DoMenuSelection( "" , MenuTexts , -1 , NULL , FPS_Display_BFont );
      
      while (EnterPressed() || SpacePressed() );
      
      switch (MenuPosition) 
	{
	  
	case (-1):
	  while ( EscapePressed() );
	  Weiter=!Weiter;
	  if ( CurrentCombatScaleFactor != 1 ) SetCombatScaleTo( 1 );
	  break;
	case EXPORT_THIS_LEVEL:
	  while (EnterPressed() || SpacePressed() ) ;
	  ExportLevelInterface ( Me [ 0 ] . pos . z );
	  // Weiter=!Weiter;
	  break;
	case REPORT_INTERFACE_INCONSISTENCIES:
	  while (EnterPressed() || SpacePressed() ) ;
	  ReportInconsistenciesForLevel ( Me [ 0 ] . pos . z );
	  while ( !EnterPressed() && !SpacePressed() ) ;
	  while (EnterPressed() || SpacePressed() ) ;
	  break;

	case QUIT_THRESHOLD_EDITOR_POSITION:
	  while (EnterPressed() || SpacePressed() ) ;
	  Weiter=!Weiter;
	  break;
	default: 
	  break;
	  
	} // switch
      
	  // If the user of the level editor pressed left or right, that should have
	  // an effect IF he/she is a the change level menu point
      
      if (LeftPressed() || RightPressed() ) 
	{
	  switch (MenuPosition)
	    {
	      
	    case JUMP_THRESHOLD_NORTH:
	      if ( LeftPressed() )
		{
		  if ( EditLevel->jump_threshold_north >= 0 ) EditLevel->jump_threshold_north -- ;
		  while (LeftPressed());
		}
	      if ( RightPressed() )
		{
		  EditLevel->jump_threshold_north ++ ;
		  while (RightPressed());
		}
	      break;
	      
	    case JUMP_THRESHOLD_SOUTH:
	      if ( LeftPressed() )
		{
		  if ( EditLevel->jump_threshold_south >= 0 ) EditLevel->jump_threshold_south -- ;
		  while (LeftPressed());
		}
	      if ( RightPressed() )
		{
		  EditLevel->jump_threshold_south ++ ;
		  while (RightPressed());
		}
	      break;

	    case JUMP_THRESHOLD_EAST:
	      if ( LeftPressed() )
		{
		  if ( EditLevel->jump_threshold_east >= 0 ) EditLevel->jump_threshold_east -- ;
		  while (LeftPressed());
		}
	      if ( RightPressed() )
		{
		  EditLevel->jump_threshold_east ++ ;
		  while (RightPressed());
		}
	      break;

	    case JUMP_THRESHOLD_WEST:
	      if ( LeftPressed() )
		{
		  if ( EditLevel->jump_threshold_west >= 0 ) EditLevel->jump_threshold_west -- ;
		  while (LeftPressed());
		}
	      if ( RightPressed() )
		{
		  EditLevel->jump_threshold_west ++ ;
		  while (RightPressed());
		}
	      break;
	      
	    case JUMP_TARGET_NORTH:
	      if ( LeftPressed() )
		{
		  if ( EditLevel->jump_target_north >= 0 ) EditLevel->jump_target_north -- ;
		  while (LeftPressed());
		}
	      if ( RightPressed() )
		{
		  EditLevel->jump_target_north ++ ;
		  while (RightPressed());
		}
	      break;
	      
	    case JUMP_TARGET_SOUTH:
	      if ( LeftPressed() )
		{
		  if ( EditLevel->jump_target_south >= 0 ) EditLevel->jump_target_south -- ;
		  while (LeftPressed());
		}
	      if ( RightPressed() )
		{
		  EditLevel->jump_target_south ++ ;
		  while (RightPressed());
		}
	      break;

	    case JUMP_TARGET_EAST:
	      if ( LeftPressed() )
		{
		  if ( EditLevel->jump_target_east >= 0 ) EditLevel->jump_target_east -- ;
		  while (LeftPressed());
		}
	      if ( RightPressed() )
		{
		  EditLevel->jump_target_east ++ ;
		  while (RightPressed());
		}
	      break;

	    case JUMP_TARGET_WEST:
	      if ( LeftPressed() )
		{
		  if ( EditLevel->jump_target_west >= 0 ) EditLevel->jump_target_west -- ;
		  while (LeftPressed());
		}
	      if ( RightPressed() )
		{
		  EditLevel->jump_target_west ++ ;
		  while (RightPressed());
		}
	      break;
	      

	    }
	} // if LeftPressed || RightPressed
      
    }

}; // void SetLevelInterfaces ( void )

/* ----------------------------------------------------------------------
 * This function should create a completely new level into the existing
 * ship structure that we already have.  The new level will be rather
 * small and simple.
 * ---------------------------------------------------------------------- */
Level
CreateNewMapLevel( void )
{
  Level NewLevel;
  int i, k;

  //--------------------
  // Get the memory for one level 
  //
  NewLevel = (Level) MyMalloc (sizeof (level));

  DebugPrintf (0, "\n-----------------------------------------------------------------");
  DebugPrintf (0, "\nStarting to create and add a completely new level to the ship.");

  NewLevel->levelnum = curShip.num_levels;
  NewLevel->xlen = 90;
  NewLevel->ylen = 90;
  NewLevel->color = 1;

  DebugPrintf( 2 , "\nLevelnumber : %d ", NewLevel->levelnum );
  DebugPrintf( 2 , "\nxlen of this level: %d ", NewLevel->xlen );
  DebugPrintf( 2 , "\nylen of this level: %d ", NewLevel->ylen );
  DebugPrintf( 2 , "\ncolor of this level: %d ", NewLevel->ylen );

  NewLevel->Levelname = "New Level just created..." ;
  NewLevel->Background_Song_Name = "NOWHERE.MOD" ;
  NewLevel->Level_Enter_Comment = "This is a new level..." ;

  //--------------------
  // First we initialize the statement array with 'empty' values
  //
  for ( i = 0 ; i < MAX_STATEMENTS_PER_LEVEL ; i ++ )
    {
      NewLevel->StatementList[ i ].x = ( -1 ) ;
      NewLevel->StatementList[ i ].y = ( -1 ) ;
      NewLevel->StatementList[ i ].Statement_Text = "No Statement loaded." ;
    }

  //--------------------
  // Now we initialize the level jump interface variables with 'empty' values
  //
  NewLevel->jump_target_north = (-1) ;
  NewLevel->jump_target_south = (-1) ;
  NewLevel->jump_target_east = (-1) ;
  NewLevel->jump_target_west = (-1) ;
  NewLevel->jump_threshold_north = (-1) ;
  NewLevel->jump_threshold_south = (-1) ;
  NewLevel->jump_threshold_east = (-1) ;
  NewLevel->jump_threshold_west = (-1) ;

  //--------------------
  // First we initialize the codepanel arrays with 'empty' information
  //
  for ( i = 0 ; i < MAX_CODEPANELS_PER_LEVEL ; i ++ )
    {
      NewLevel->CodepanelList[ i ].x = ( -1 ) ;
      NewLevel->CodepanelList[ i ].y = ( -1 ) ;
      NewLevel->CodepanelList[ i ].Secret_Code = "nonono" ;
    }

  //--------------------
  // First we initialize the graphics insert array with 'empty' information
  //
  for ( i = 0 ; i < MAX_MAP_INSERTS_PER_LEVEL ; i ++ )
    {
      NewLevel->MapInsertList [ i ] . type = ( -1 ) ;
      NewLevel->MapInsertList [ i ] . pos . x = ( -1 ) ;
      NewLevel->MapInsertList [ i ] . pos . y = ( -1 ) ;
    }

  //--------------------
  // First we initialize the items arrays with 'empty' information
  //
  for ( i = 0 ; i < MAX_ITEMS_PER_LEVEL ; i ++ )
    {
      NewLevel->ItemList[ i ].pos.x = ( -1 ) ;
      NewLevel->ItemList[ i ].pos.y = ( -1 ) ;
      NewLevel->ItemList[ i ].type = ( -1 ) ;
      NewLevel->ItemList[ i ].currently_held_in_hand = FALSE;
    }

  //--------------------
  // Now we initialize the chest items arrays with 'empty' information
  //
  for ( i = 0 ; i < MAX_ITEMS_PER_LEVEL ; i ++ )
    {
      NewLevel->ChestItemList[ i ].pos.x = ( -1 ) ;
      NewLevel->ChestItemList[ i ].pos.y = ( -1 ) ;
      NewLevel->ChestItemList[ i ].type = ( -1 ) ;
      NewLevel->ChestItemList[ i ].currently_held_in_hand = FALSE;
    }

  //--------------------
  // Now we initialize the map labels array with 'empty' information
  //
  for ( i = 0 ; i < MAX_MAP_LABELS_PER_LEVEL ; i ++ )
    {
      NewLevel -> labels [ i ] . pos . x = ( -1 ) ;
      NewLevel -> labels [ i ] . pos . y = ( -1 ) ;
      NewLevel -> labels [ i ] . label_name = "no_label_defined" ;
    }

  //--------------------
  // Now we add some dummy map content...
  //
  for ( i = 0 ; i < NewLevel->ylen ; i ++ )
    {
      NewLevel->map[i] = MyMalloc ( NewLevel->xlen + 1 );
      if ( ( i == 0 ) || ( i == NewLevel->ylen -1 ) ) memset ( NewLevel->map[i] , H_WALL , NewLevel->xlen );
      else memset ( NewLevel->map[i] , FLOOR , NewLevel->xlen );

      if ( i == 0 )
	{
	  NewLevel->map[i][0] = CORNER_LU;
	  NewLevel->map[i][ NewLevel->xlen -1 ] = CORNER_RU;
	}
      else if ( i == NewLevel->ylen -1 )
	{
	  NewLevel->map[i][0] = CORNER_LD;
	  NewLevel->map[i][ NewLevel->xlen -1 ] = CORNER_RD;
	}
      else
	{
	  NewLevel->map[i][0] = V_WALL;
	  NewLevel->map[i][ NewLevel->xlen -1 ] = V_WALL;
	}

    }

  //--------------------
  // Now we add empty waypoint information...
  //
  for ( i = 0 ; i < MAXWAYPOINTS ; i++ )
    {
      NewLevel -> AllWaypoints [ i ] . x = 0 ;
      NewLevel -> AllWaypoints [ i ] . y = 0 ;

      for ( k=0 ; k<MAX_WP_CONNECTIONS ; k++ )
	{
	  NewLevel -> AllWaypoints [ i ] . connections [ k ] = -1 ;
	}
    }

  //--------------------
  return NewLevel;
}; // Level CreateNewMapLevel( void )

/* ----------------------------------------------------------------------
 * Now we print out the codepanel information about this tile
 * just in case it is really a codepanel.
 * ---------------------------------------------------------------------- */
void
PrintCodepanelInformationOfThisSquare ( Level EditLevel )
{
  int CodepanelIndex;
  char PanelText[5000]="";

  switch ( EditLevel->map [ (int)rintf( Me[0].pos.y) ] [ (int)rintf( Me[0].pos.x ) ] )
    {
    case CODEPANEL_L:
    case CODEPANEL_R:
    case CODEPANEL_U:
    case CODEPANEL_D:

      for ( CodepanelIndex = 0 ; CodepanelIndex < MAX_CODEPANELS_PER_LEVEL ; CodepanelIndex ++ )
	{
	  if ( ( ( (int) rintf( Me[0].pos.x ) ) == EditLevel->CodepanelList[ CodepanelIndex ].x ) && 
	       ( ( (int) rintf( Me[0].pos.y ) ) == EditLevel->CodepanelList[ CodepanelIndex ].y ) )
	    break;
	}

      if ( CodepanelIndex >= MAX_CODEPANELS_PER_LEVEL )
	{
	  sprintf( PanelText , "\nWARNING!  Either no codepanel code present or last entry used.\n" );
	}
      else
	{
	  sprintf( PanelText , "\nCode Panel Information: \n Codeword=\"%s\"." , 
		   EditLevel->CodepanelList[ CodepanelIndex ].Secret_Code );
	}

      DisplayText ( PanelText , User_Rect.x , 91 + User_Rect.y , &User_Rect );
      break;
    default:
      break;
    }
}; // void PrintCodepanelInformationOfThisSquare ( Level EditLevel )


/* ----------------------------------------------------------------------
 * Now we print out the map label information about this map location.
 * ---------------------------------------------------------------------- */
void
PrintMapLabelInformationOfThisSquare ( Level EditLevel )
{
  int MapLabelIndex;
  char PanelText[5000]="";

  for ( MapLabelIndex = 0 ; MapLabelIndex < MAX_MAP_LABELS_PER_LEVEL ; MapLabelIndex ++ )
    {
      if ( ( ( (int) rintf( Me[0].pos.x ) ) == EditLevel -> labels [ MapLabelIndex ] . pos . x ) && 
	   ( ( (int) rintf( Me[0].pos.y ) ) == EditLevel -> labels [ MapLabelIndex ] . pos . y ) )
	break;
    }
  
  if ( MapLabelIndex >= MAX_MAP_LABELS_PER_LEVEL ) return;

  sprintf( PanelText , "\n Map Label Information: \n label_name=\"%s\"." , 
	   EditLevel -> labels [ MapLabelIndex ] . label_name );
  
  DisplayText ( PanelText , User_Rect.x , 91 + User_Rect.y , &User_Rect );

}; // void PrintMapLabelInformationOfThisSquare ( Level EditLevel )

/* ----------------------------------------------------------------------
 * This function is used by the Level Editor integrated into 
 * freedroid.  It highlights the map position that is currently 
 * edited or would be edited, if the user pressed something.  I.e. 
 * it provides a "cursor" for the Level Editor.
 * ---------------------------------------------------------------------- */
void 
Highlight_Current_Block(void)
{
  int i;
  Level EditLevel;

  EditLevel = curShip.AllLevels [ Me [ 0 ] . pos . z ] ;

#define HIGHLIGHTCOLOR 255

  //--------------------
  // At first we draw all the four lines that make up the 
  // cursor in the level editor
  //
  SDL_LockSurface( Screen );
  for (i=0; i<Block_Width; i++)
    {
      // This draws a (double) line at the upper border of the current block
      putpixel( Screen , i + User_Rect.x + (User_Rect.w/2) + (rintf(Me[0].pos.x)-Me[0].pos.x - 0.5) * Block_Width , User_Rect.y + User_Rect.h/2 + (rintf(Me[0].pos.y)-Me[0].pos.y - 0.5 ) * Block_Height , HIGHLIGHTCOLOR );
      putpixel( Screen , i + User_Rect.x + (User_Rect.w/2) + (rintf(Me[0].pos.x)-Me[0].pos.x - 0.5) * Block_Width , User_Rect.y + User_Rect.h/2 + (rintf(Me[0].pos.y)-Me[0].pos.y - 0.5 ) * Block_Height + 1, HIGHLIGHTCOLOR );

      // This draws a line at the lower border of the current block
      putpixel( Screen , i + User_Rect.x + (User_Rect.w/2) + (rintf(Me[0].pos.x)-Me[0].pos.x - 0.5) * Block_Width , User_Rect.y + User_Rect.h/2 + (rintf(Me[0].pos.y)-Me[0].pos.y + 0.5 ) * Block_Height -1, HIGHLIGHTCOLOR );
      putpixel( Screen , i + User_Rect.x + (User_Rect.w/2) + (rintf(Me[0].pos.x)-Me[0].pos.x - 0.5) * Block_Width , User_Rect.y + User_Rect.h/2 + (rintf(Me[0].pos.y)-Me[0].pos.y + 0.5 ) * Block_Height -2, HIGHLIGHTCOLOR );

      // This draws a line at the left border of the current block
      putpixel( Screen , 0 + User_Rect.x + (User_Rect.w/2) + (rintf(Me[0].pos.x)-Me[0].pos.x - 0.5) * Block_Width , User_Rect.y + User_Rect.h/2 + (rintf(Me[0].pos.y)-Me[0].pos.y - 0.5 ) * Block_Height + i , HIGHLIGHTCOLOR );
      putpixel( Screen , 1 + User_Rect.x + (User_Rect.w/2) + (rintf(Me[0].pos.x)-Me[0].pos.x - 0.5) * Block_Width , User_Rect.y + User_Rect.h/2 + (rintf(Me[0].pos.y)-Me[0].pos.y - 0.5 ) * Block_Height + i , HIGHLIGHTCOLOR );

      // This draws a line at the right border of the current block
      putpixel( Screen , -1 + User_Rect.x + (User_Rect.w/2) + (rintf(Me[0].pos.x)-Me[0].pos.x + 0.5) * Block_Width , User_Rect.y + User_Rect.h/2 + (rintf(Me[0].pos.y)-Me[0].pos.y - 0.5 ) * Block_Height + i , HIGHLIGHTCOLOR );
      putpixel( Screen , -2 + User_Rect.x + (User_Rect.w/2) + (rintf(Me[0].pos.x)-Me[0].pos.x + 0.5) * Block_Width , User_Rect.y + User_Rect.h/2 + (rintf(Me[0].pos.y)-Me[0].pos.y - 0.5 ) * Block_Height + i , HIGHLIGHTCOLOR );

    }
  SDL_UnlockSurface( Screen );

  //--------------------
  // Now we print out the codepanel information about this tile
  // just in case it is really a codepanel.
  //
  PrintCodepanelInformationOfThisSquare ( EditLevel );

  PrintMapLabelInformationOfThisSquare ( EditLevel );

} // void Highlight_Current_Block(void)

/* ----------------------------------------------------------------------
 * This function is used by the Level Editor integrated into 
 * freedroid.  It marks all waypoints with a cross.
 * ---------------------------------------------------------------------- */
void 
ShowWaypoints( int PrintConnectionList )
{
  int wp;
  int i;
  int x;
  int y;
  int BlockX, BlockY;
  int color;
  char ConnectionText[5000];
  char TextAddition[1000];
  Level EditLevel;

  EditLevel = curShip.AllLevels [ Me [ 0 ] . pos . z ] ;

#define ACTIVE_WP_COLOR 0x0FFFFFFFF

  BlockX=rintf(Me[0].pos.x);
  BlockY=rintf(Me[0].pos.y);
	  
  SDL_LockSurface( Screen );

  for (wp=0; wp<MAXWAYPOINTS; wp++)
    {

      if ( EditLevel->AllWaypoints[wp].x == 0) continue;

      //--------------------
      // Draw the cross in the middle of the middle of the tile
      //
      for (i= Block_Width/4; i<3 * Block_Width / 4; i++)
	{
	  // This draws a (double) line at the upper border of the current block
	  x = i + User_Rect.x+(User_Rect.w/2)- (( Me[0].pos.x)-EditLevel->AllWaypoints[wp].x + 0.5) * Block_Width;
	  y = i + User_Rect.y+User_Rect.h/2 - (( Me[0].pos.y)-EditLevel->AllWaypoints[wp].y + 0.5) * Block_Height;
	  if ( ( x < User_Rect.x ) || ( x > User_Rect.x + User_Rect.w ) || ( y < User_Rect. y) || ( y > User_Rect.y + User_Rect.h ) ) continue;
	  putpixel( Screen , x , y , HIGHLIGHTCOLOR );

		    
	  x = i + User_Rect.x + (User_Rect.w/2) - (( Me[0].pos.x )-EditLevel->AllWaypoints[wp].x + 0.5) * Block_Width;
	  y = i + User_Rect.y+User_Rect.h/2- (( Me[0].pos.y)-EditLevel->AllWaypoints[wp].y + 0.5) * Block_Height + 1;
	  if ( ( x < User_Rect.x ) || ( x > User_Rect.x + User_Rect.w ) || ( y < User_Rect. y) || ( y > User_Rect.y + User_Rect.h ) ) continue;
	  putpixel( Screen , x , y , HIGHLIGHTCOLOR );
	  
	  // This draws a line at the lower border of the current block
	  x = i + User_Rect.x + (User_Rect.w/2) - (( Me[0].pos.x)-EditLevel->AllWaypoints[wp].x + 0.5) * Block_Width;
	  y = -i + User_Rect.y + User_Rect.h/2 - (( Me[0].pos.y )-EditLevel->AllWaypoints[wp].y - 0.5 ) * Block_Height -1;
	  if ( ( x < User_Rect.x ) || ( x > User_Rect.x + User_Rect.w ) || ( y < User_Rect. y) || ( y > User_Rect.y + User_Rect.h ) ) continue;
	  putpixel( Screen , x , y , HIGHLIGHTCOLOR );

	  x = i + User_Rect.x + (User_Rect.w/2) - (( Me[0].pos.x)-EditLevel->AllWaypoints[wp].x + 0.5) * Block_Width;
	  y = -i + User_Rect.y + User_Rect.h/2 - ((Me[0].pos.y)-EditLevel->AllWaypoints[wp].y - 0.5 ) * Block_Height -2;
	  if ( ( x < User_Rect.x ) || ( x > User_Rect.x + User_Rect.w ) || ( y < User_Rect. y) || ( y > User_Rect.y + User_Rect.h ) ) continue;
	  putpixel( Screen , x , y , HIGHLIGHTCOLOR );
	  
	}

      
      //--------------------
      // Draw the connections to other waypoints, BUT ONLY FOR THE WAYPOINT CURRENTLY TARGETED
      //
      if ( PrintConnectionList )
	{
	  strcpy( ConnectionText , "List of connection for this wp:\n" );
	}

      for ( i=0; i<MAX_WP_CONNECTIONS; i++ )
	{
	  if ( EditLevel->AllWaypoints[wp].connections[i] != (-1) )
	    {
	      if ( ( BlockX == EditLevel->AllWaypoints[wp].x ) && ( BlockY == EditLevel->AllWaypoints[wp].y ) )
		{
		  // color = ACTIVE_WP_COLOR ;
		  // else color = HIGHLIGHTCOLOR ; 
		  // printf(" Found a connection!! ");
		  // printf_SDL ( Screen  , 100 , 100 , "Waypoint connection to: " );

		  
		  //--------------------
		  // If this is desired, we also print a list of connections from
		  // this waypoint to other waypoints in text form...
		  //
		  if ( PrintConnectionList )
		    {
		      SDL_UnlockSurface( Screen );
		      sprintf ( TextAddition , "To: X=%d Y=%d    " , 
				EditLevel->AllWaypoints[EditLevel->AllWaypoints[wp].connections[i]].x , 
				EditLevel->AllWaypoints[EditLevel->AllWaypoints[wp].connections[i]].y 
				);
		      strcat ( ConnectionText , TextAddition );
		      DisplayText ( ConnectionText , User_Rect.x , User_Rect.y , &User_Rect );
		      SDL_LockSurface( Screen );
		    }
		      
		  DrawLineBetweenTiles( EditLevel->AllWaypoints[wp].x , EditLevel->AllWaypoints[wp].y , 
					EditLevel->AllWaypoints[EditLevel->AllWaypoints[wp].connections[i]].x , 
					EditLevel->AllWaypoints[EditLevel->AllWaypoints[wp].connections[i]].y ,
					color );
		}
	    }
	}
	      
    }
  SDL_UnlockSurface( Screen );

} // void ShowWaypoints( int PrintConnectionList );

/* ----------------------------------------------------------------------
 * This function is used by the Level Editor integrated into 
 * freedroid.  It marks all places that have a label attached to them.
 * ---------------------------------------------------------------------- */
void 
ShowMapLabels( void )
{
  int LabelNr;
  int i , x , y;
  int BlockX, BlockY;
  Level EditLevel;

  EditLevel = curShip.AllLevels [ Me [ 0 ] . pos . z ] ;

  // #define LABEL_COLOR 0x0FFFFFFFF

  BlockX = rintf (Me[0].pos.x);
  BlockY = rintf (Me[0].pos.y);
	  
  SDL_LockSurface( Screen );

  for (LabelNr=0; LabelNr<MAXWAYPOINTS; LabelNr++)
    {

      if ( EditLevel->labels[LabelNr].pos.x == (-1) ) continue;

      //--------------------
      // Draw the cross in the middle of the middle of the tile
      //
      for (i= Block_Width/4; i<3 * Block_Width / 4; i++)
	{
	  // This draws the left border of our square marker
	  x = Block_Width/4 + User_Rect.x+(User_Rect.w/2)- (( Me[0].pos.x)-EditLevel->labels[LabelNr].pos.x + 0.5) * Block_Width;
	  y = i + User_Rect.y+User_Rect.h/2 - (( Me[0].pos.y)-EditLevel->labels[LabelNr].pos.y + 0.5) * Block_Height;
	  if ( ( x < User_Rect.x ) || ( x > User_Rect.x + User_Rect.w ) || ( y < User_Rect. y) || ( y > User_Rect.y + User_Rect.h ) ) continue;
	  putpixel( Screen , x , y , HIGHLIGHTCOLOR );

		    
	  x = Block_Width/4 + User_Rect.x + (User_Rect.w/2) - (( Me[0].pos.x )-EditLevel->labels[LabelNr].pos.x + 0.5) * Block_Width + 1 ;
	  y = i + User_Rect.y + User_Rect.h/2- (( Me[0].pos.y)-EditLevel->labels[LabelNr].pos.y + 0.5) * Block_Height ;
	  if ( ( x < User_Rect.x ) || ( x > User_Rect.x + User_Rect.w ) || ( y < User_Rect. y) || ( y > User_Rect.y + User_Rect.h ) ) continue;
	  putpixel( Screen , x , y , HIGHLIGHTCOLOR );
	  
	  // This draws the right border of our square marker
	  x = 3*Block_Width/4 + User_Rect.x + (User_Rect.w/2) - (( Me[0].pos.x)-EditLevel->labels[LabelNr].pos.x + 0.5) * Block_Width;
	  y = -i + User_Rect.y + User_Rect.h/2 - (( Me[0].pos.y )-EditLevel->labels[LabelNr].pos.y - 0.5 ) * Block_Height -1;
	  if ( ( x < User_Rect.x ) || ( x > User_Rect.x + User_Rect.w ) || ( y < User_Rect. y) || ( y > User_Rect.y + User_Rect.h ) ) continue;
	  putpixel( Screen , x , y , HIGHLIGHTCOLOR );

	  x = 3*Block_Width/4 + User_Rect.x + (User_Rect.w/2) - (( Me[0].pos.x)-EditLevel->labels[LabelNr].pos.x + 0.5) * Block_Width + 1 ;
	  y = -i + User_Rect.y + User_Rect.h/2 - ((Me[0].pos.y)-EditLevel->labels[LabelNr].pos.y - 0.5 ) * Block_Height ;
	  if ( ( x < User_Rect.x ) || ( x > User_Rect.x + User_Rect.w ) || ( y < User_Rect. y) || ( y > User_Rect.y + User_Rect.h ) ) continue;
	  putpixel( Screen , x , y , HIGHLIGHTCOLOR );
	  
	  // This draws the upper border of our square marker
	  x = i + User_Rect.x+(User_Rect.w/2)- (( Me[0].pos.x)-EditLevel->labels[LabelNr].pos.x + 0.5) * Block_Width;
	  y = Block_Height/4 + User_Rect.y+User_Rect.h/2 - (( Me[0].pos.y)-EditLevel->labels[LabelNr].pos.y + 0.5) * Block_Height;
	  if ( ( x < User_Rect.x ) || ( x > User_Rect.x + User_Rect.w ) || ( y < User_Rect. y) || ( y > User_Rect.y + User_Rect.h ) ) continue;
	  putpixel( Screen , x , y , HIGHLIGHTCOLOR );
		    
	  x = i + User_Rect.x + (User_Rect.w/2) - (( Me[0].pos.x )-EditLevel->labels[LabelNr].pos.x + 0.5) * Block_Width;
	  y = Block_Height/4 + User_Rect.y + User_Rect.h/2- (( Me[0].pos.y)-EditLevel->labels[LabelNr].pos.y + 0.5) * Block_Height + 1;
	  if ( ( x < User_Rect.x ) || ( x > User_Rect.x + User_Rect.w ) || ( y < User_Rect. y) || ( y > User_Rect.y + User_Rect.h ) ) continue;
	  putpixel( Screen , x , y , HIGHLIGHTCOLOR );
	  
	  // This draws the lower border of our square marker
	  x = i + User_Rect.x+(User_Rect.w/2)- (( Me[0].pos.x)-EditLevel->labels[LabelNr].pos.x + 0.5) * Block_Width;
	  y = 3*Block_Height/4 + User_Rect.y+User_Rect.h/2 - (( Me[0].pos.y)-EditLevel->labels[LabelNr].pos.y + 0.5) * Block_Height;
	  if ( ( x < User_Rect.x ) || ( x > User_Rect.x + User_Rect.w ) || ( y < User_Rect. y) || ( y > User_Rect.y + User_Rect.h ) ) continue;
	  putpixel( Screen , x , y , HIGHLIGHTCOLOR );
		    
	  x = i + User_Rect.x + (User_Rect.w/2) - (( Me[0].pos.x )-EditLevel->labels[LabelNr].pos.x + 0.5) * Block_Width;
	  y = 3*Block_Height/4 + User_Rect.y + User_Rect.h/2- (( Me[0].pos.y)-EditLevel->labels[LabelNr].pos.y + 0.5) * Block_Height + 1;
	  if ( ( x < User_Rect.x ) || ( x > User_Rect.x + User_Rect.w ) || ( y < User_Rect. y) || ( y > User_Rect.y + User_Rect.h ) ) continue;
	  putpixel( Screen , x , y , HIGHLIGHTCOLOR );
	  
	}
	      
    }

  SDL_UnlockSurface( Screen );

} // void ShowMapLabels( void );

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
HandleMapTileEditingKeys ( Level EditLevel , int BlockX , int BlockY )
{
  
  //--------------------
  // Pressing the 'T' key will insert a teleporter field...
  //
  if ( TPressed()) 
    {
      EditLevel -> map [ BlockY ] [ BlockX ] = TELE_1 ;
    }
  
  //--------------------
  // Pressing the '1' to '5' keys will insert either classic 'block'
  // fixed map tiles or decructible 'box' map blocks.
  //
  if (Number1Pressed()) 
    {
      if ( Shift_Was_Pressed() )
	EditLevel->map[BlockY][BlockX]=BOX_1;
      else
	EditLevel->map[BlockY][BlockX]=BLOCK1;
    }
  if (Number2Pressed()) 
    {
      if ( Shift_Was_Pressed() )
	EditLevel->map[BlockY][BlockX]=BOX_2;
      else
	EditLevel->map[BlockY][BlockX]=BLOCK2;
    }
  if (Number3Pressed()) 
    {
      if ( Shift_Was_Pressed() )
	EditLevel->map[BlockY][BlockX]=BOX_3;
      else
	EditLevel->map[BlockY][BlockX]=BLOCK3;
    }
  if (Number4Pressed()) 
    {
      if ( Shift_Was_Pressed() )
	EditLevel->map[BlockY][BlockX]=BOX_4;
      else
	EditLevel->map[BlockY][BlockX]=BLOCK4;
    }
  if (Number5Pressed()) 
    {
      EditLevel->map[BlockY][BlockX]=BLOCK5;
    }
  if (LPressed()) 
    {
      EditLevel->map[BlockY][BlockX]=LIFT;
    }
  if (KP_PLUS_Pressed()) 
    {
      EditLevel->map[BlockY][BlockX]=V_WALL;
      if ( Alt_Was_Pressed() ) EditLevel->map[BlockY][BlockX]=CAVE_V_WALL;
    }
  if (KP0Pressed()) 
    {
      EditLevel->map[BlockY][BlockX]=H_WALL;
      if ( Alt_Was_Pressed() ) EditLevel->map[BlockY][BlockX]=CAVE_H_WALL;
    }
  if (KP1Pressed()) 
    {
      if ( Shift_Was_Pressed() ) EditLevel->map[BlockY][BlockX]=AUTOGUN_L;
      else if ( CtrlWasPressed() ) EditLevel->map[BlockY][BlockX]=ENHANCER_LD;
      else if ( Alt_Was_Pressed() ) EditLevel->map[BlockY][BlockX]=CAVE_CORNER_LD;
      else EditLevel->map[BlockY][BlockX]=CORNER_LD;
    }
  if (KP2Pressed()) 
    {
      if ( Shift_Was_Pressed() )
	EditLevel->map[BlockY][BlockX]=CONSOLE_D;
      else if ( LeftCtrlWasPressed() ) 
	EditLevel->map[BlockY][BlockX]=CODEPANEL_D;
      else if ( RightCtrlWasPressed() ) 
	EditLevel->map[BlockY][BlockX]=FLOOR_CARPET_D;
      else if ( LeftAltWasPressed() ) 
	EditLevel->map[BlockY][BlockX]=CONVEY_D;
      else if ( RightAltWasPressed() ) 
	EditLevel->map[BlockY][BlockX]=FLOOR_CAVE_D;
      else EditLevel->map[BlockY][BlockX]=T_D;
    }
  if (KP3Pressed()) 
    {
      if ( Shift_Was_Pressed() ) EditLevel->map[BlockY][BlockX]=AUTOGUN_U;
      else if ( CtrlWasPressed() ) EditLevel->map[BlockY][BlockX]=ENHANCER_RD;
      else if ( Alt_Was_Pressed() ) EditLevel->map[BlockY][BlockX]=CAVE_CORNER_RD;
      else EditLevel->map[BlockY][BlockX]=CORNER_RD;
    }
  if (KP4Pressed()) 
    {
      if ( Shift_Was_Pressed() )
	EditLevel->map[BlockY][BlockX]=CONSOLE_L;
      else if ( LeftCtrlWasPressed() ) 
	EditLevel->map[BlockY][BlockX]=CODEPANEL_L;
      else if ( RightCtrlWasPressed() ) 
	EditLevel->map[BlockY][BlockX]=FLOOR_CARPET_L;
      else if ( LeftAltWasPressed() ) 
	EditLevel->map[BlockY][BlockX]=CONVEY_R;
      else if ( RightAltWasPressed() ) 
	EditLevel->map[BlockY][BlockX]=FLOOR_CAVE_R;
      else EditLevel->map[BlockY][BlockX]=T_L;
    }
  if (KP5Pressed()) 
    {
      if ( Shift_Was_Pressed() )
	EditLevel->map[BlockY][BlockX]=VOID;
      else if ( RightCtrlWasPressed() ) 
	EditLevel->map[BlockY][BlockX]=FLOOR_CARPET;
      else if ( RightAltWasPressed() ) 
	EditLevel->map[BlockY][BlockX]=CAVE_FLOOR;
      else EditLevel->map[BlockY][BlockX]=KREUZ;
    }
  if (KP6Pressed()) 
    {
      if ( LeftShiftWasPressed() )
	EditLevel->map[BlockY][BlockX]=CONSOLE_R;
      else if ( LeftCtrlWasPressed() ) 
	EditLevel->map[BlockY][BlockX]=CODEPANEL_R;
      else if ( RightCtrlWasPressed() ) 
	EditLevel->map[BlockY][BlockX]=FLOOR_CARPET_R;
      else if ( LeftAltWasPressed() ) 
	EditLevel->map[BlockY][BlockX]=CONVEY_L;
      else if ( RightAltWasPressed() ) 
	EditLevel->map[BlockY][BlockX]=FLOOR_CAVE_L;
      else EditLevel->map[BlockY][BlockX]=T_R;
    }
  if (KP7Pressed()) 
    {
      if ( Shift_Was_Pressed() ) EditLevel->map[BlockY][BlockX]=AUTOGUN_D;
      else if ( CtrlWasPressed() ) EditLevel->map[BlockY][BlockX]=ENHANCER_LU;
      else if ( Alt_Was_Pressed() ) EditLevel->map[BlockY][BlockX]=CAVE_CORNER_LU;
      else EditLevel->map[BlockY][BlockX]=CORNER_LU;
    }
  if ( KP8Pressed() ) 
    {
      if ( Shift_Was_Pressed() )
	EditLevel->map[BlockY][BlockX]=CONSOLE_U;
      else if ( LeftCtrlWasPressed() ) 
	EditLevel->map[BlockY][BlockX]=CODEPANEL_U;
      else if ( RightCtrlWasPressed() ) 
	EditLevel->map[BlockY][BlockX]=FLOOR_CARPET_U;
      else if ( LeftAltWasPressed() ) 
	EditLevel->map[BlockY][BlockX]=CONVEY_U;
      else if ( RightAltWasPressed() ) 
	EditLevel->map[BlockY][BlockX]=FLOOR_CAVE_U;
      else EditLevel->map[BlockY][BlockX]=T_U;
    }
  if (KP9Pressed()) 
    {
      if ( Shift_Was_Pressed() ) EditLevel->map[BlockY][BlockX]=AUTOGUN_R;
      else if ( CtrlWasPressed() ) EditLevel->map[BlockY][BlockX]=ENHANCER_RU;
      else if ( Alt_Was_Pressed() ) EditLevel->map[BlockY][BlockX]=CAVE_CORNER_RU;
      else EditLevel->map[BlockY][BlockX]=CORNER_RU;
    }
  if (APressed())
    {
      EditLevel->map[BlockY][BlockX]=ALERT;	      
    }
  if (RPressed())
    {
      if ( Shift_Was_Pressed() ) EditLevel->map[BlockY][BlockX] = CONSUMER_1;
      else EditLevel->map[BlockY][BlockX] = REFRESH1;	            
    }
  if (DPressed())
    {
      if ( !CtrlWasPressed())
	{
	  if (Shift_Was_Pressed())
	    EditLevel->map[BlockY][BlockX]=V_SHUT_DOOR;	            	      
	  else EditLevel->map[BlockY][BlockX]=H_SHUT_DOOR;	            	      
	}
      else
	{
	  if (Shift_Was_Pressed())
	    EditLevel->map[BlockY][BlockX]=LOCKED_V_SHUT_DOOR;	            	      
	  else EditLevel->map[BlockY][BlockX]=LOCKED_H_SHUT_DOOR;	            	      
	}
    }
  if (UPressed())
    {
      if ( !CtrlWasPressed())
	{
	  if (Shift_Was_Pressed())
	    EditLevel->map[BlockY][BlockX]=CHEST_U;	            	      
	  else EditLevel->map[BlockY][BlockX]=CHEST_D;	            	      
	}
      else
	{
	  if (Shift_Was_Pressed())
	    EditLevel->map[BlockY][BlockX]=CHEST_L;	            	      
	  else EditLevel->map[BlockY][BlockX]=CHEST_R;	            	      
	}
    }
  if (SpacePressed() && !axis_is_active )
    {
      if ( Shift_Was_Pressed() )
	EditLevel->map[BlockY][BlockX]=FINE_GRID;	            	      	    
      else
	EditLevel->map[BlockY][BlockX]=FLOOR;	            	      	    
    }
  
}; // void HandleMapTileEditingKeys ( Level EditLevel , int BlockX , int BlockY )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void 
ToggleBigGraphicsInserts ( Level EditLevel , int BlockX, int BlockY )
{
  int MapInsertNr;

  //--------------------
  // First we wait till the b key is released again...
  //
  while ( BPressed() );
  
  //--------------------
  // Now we go and take a look if there is some grapics insert
  // at this location present already...
  //
  for ( MapInsertNr = 0 ; MapInsertNr < MAX_MAP_INSERTS_PER_LEVEL ; MapInsertNr ++ )
    {
      if ( EditLevel->MapInsertList [ MapInsertNr ] . type == ( -1 ) ) continue; 
      if ( EditLevel->MapInsertList [ MapInsertNr ] . pos.x != BlockX ) continue; 
      if ( EditLevel->MapInsertList [ MapInsertNr ] . pos.y != BlockY ) continue; 
      break;
    }
  
  if ( MapInsertNr < MAX_MAP_INSERTS_PER_LEVEL )
    {
      sprintf( VanishingMessage , " Old map insert on this position found... " );
      VanishingMessageDisplayTime = 0 ;
    }
  else
    {
      sprintf( VanishingMessage , " No Old map insert on this position found. Opening new index...." );
      VanishingMessageDisplayTime = 0 ;
      for ( MapInsertNr = 0 ; MapInsertNr < MAX_MAP_INSERTS_PER_LEVEL ; MapInsertNr ++ )
	{
	  if ( EditLevel->MapInsertList [ MapInsertNr ] . type == ( -1 ) ) break;
	}
      if ( MapInsertNr >= MAX_MAP_INSERTS_PER_LEVEL )
	{
	  sprintf( VanishingMessage , " No new map insert index available.\n Overwriting first one..." );
	  VanishingMessageDisplayTime = 0 ;
	  MapInsertNr = 0;
	}
      
      //--------------------
      // Now we enter the right coordinates for our new map index...
      //
      EditLevel->MapInsertList [ MapInsertNr ] . pos.x = BlockX ;
      EditLevel->MapInsertList [ MapInsertNr ] . pos.y = BlockY ;
      
    }
      
  //--------------------
  // At this point we know, that we have a good map insert index at our
  // hands.  Therefore we increase the number of the type and possible
  // reset it to -1 if the last index was exceeded...
  //
  EditLevel->MapInsertList [ MapInsertNr ] . type ++ ;
  
  if ( EditLevel->MapInsertList [ MapInsertNr ] . type >= MAX_MAP_INSERTS )
    EditLevel->MapInsertList [ MapInsertNr ] . type = -1;
  
}; // void ToggleBigGraphicsInserts ( Level EditLevel , int BlockX, int BlockY );

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void 
HandleLevelEditorCursorKeys ( void )
{
  Level EditLevel;

  EditLevel = curShip.AllLevels [ Me [ 0 ] . pos . z ] ;

  if (LeftPressed()) 
    {
      if ( rintf(Me[0].pos.x) > 0 ) Me[0].pos.x-=1;
      while (LeftPressed());
    }
  if (RightPressed()) 
    {
      if ( rintf(Me[0].pos.x) < EditLevel->xlen-1 ) Me[0].pos.x+=1;
      while (RightPressed());
    }
  if (UpPressed()) 
    {
      if ( rintf(Me[0].pos.y) > 0 ) Me[0].pos.y-=1;
      while (UpPressed());
    }
  if (DownPressed()) 
    {
      if ( rintf(Me[0].pos.y) < EditLevel->ylen-1 ) Me[0].pos.y+=1;
      while (DownPressed());
    }
}; // void HandleLevelEditorCursorKeys ( void )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
ToggleWaypoint ( Level EditLevel , int BlockX , int BlockY )
{
  int i , k , j ;

  // find out if there is a waypoint on the current square
  for (i=0 ; i < MAXWAYPOINTS ; i++)
    {
      if ( ( EditLevel->AllWaypoints[i].x == BlockX ) &&
	   ( EditLevel->AllWaypoints[i].y == BlockY ) ) break;
    }
  
  // if its waypoint already, this waypoint must be deleted.
  if ( i != MAXWAYPOINTS )
    {
      // Eliminate the waypoint itself
      EditLevel->AllWaypoints[i].x = 0;
      EditLevel->AllWaypoints[i].y = 0;
      for ( k = 0; k < MAX_WP_CONNECTIONS ; k++) 
	EditLevel->AllWaypoints[i].connections[k] = (-1) ;
      
		  
      // Eliminate all connections pointing to this waypoint
      for ( j = 0; j < MAXWAYPOINTS ; j++ )
	{
	  for ( k = 0; k < MAX_WP_CONNECTIONS ; k++) 
	    if ( EditLevel->AllWaypoints[j].connections[k] == i )
	      EditLevel->AllWaypoints[j].connections[k] = (-1) ;
	}
    }
  else // if its not a waypoint already, it must be made into one
    {
      // seek a free position
      for ( i = 0 ; i < MAXWAYPOINTS ; i++ )
	{
	  if ( EditLevel->AllWaypoints[i].x == 0 ) break;
	}
      if ( i == MAXWAYPOINTS )
	{
	  printf("\n\nSorry, no free waypoint available.  Using the first one.");
	  i = 0;
	}
      
      // Now make the new entry into the waypoint list
      EditLevel->AllWaypoints[i].x = BlockX;
      EditLevel->AllWaypoints[i].y = BlockY;
      
      // delete all old connection information from the new waypoint
      for ( k = 0; k < MAX_WP_CONNECTIONS ; k++ ) 
	EditLevel->AllWaypoints[i].connections[k] = (-1) ;
      
    }
  
  printf("\n\n  i is now: %d ", i ); fflush(stdout);
  
}; // void ToggleWaypoint ( Level EditLevel , int BlockX , int BlockY )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
ToggleWaypointConnection ( Level EditLevel , int BlockX , int BlockY )
{
  int i , k ;

  // Determine which waypoint is currently targeted
  for (i=0 ; i < MAXWAYPOINTS ; i++)
    {
      if ( ( EditLevel->AllWaypoints[i].x == BlockX ) &&
	   ( EditLevel->AllWaypoints[i].y == BlockY ) ) break;
    }
  
  if ( i == MAXWAYPOINTS )
    {
      sprintf( VanishingMessage , "\n\nSorry, don't know which waypoint you mean." );
      VanishingMessageDisplayTime = 0;
    }
  else
    {
      sprintf( VanishingMessage , "\n\nYou specified waypoint nr. %d." , i );
      VanishingMessageDisplayTime = 0;
      if ( OriginWaypoint== (-1) )
	{
	  strcat ( VanishingMessage , "\nIt has been marked as the origin of the next connection." );
	  OriginWaypoint = i;
	}
      else
	{
	  if ( OriginWaypoint == i )
	    {
	      strcat ( VanishingMessage , "\n\nOrigin==Target --> Connection Operation cancelled.");
	      OriginWaypoint = (-1);
	    }
	  else
	    {
	      sprintf( VanishingMessage , "\n\nOrigin: %d Target: %d. Operation makes sense.", OriginWaypoint , i );
	      for ( k = 0; k < MAX_WP_CONNECTIONS ; k++ ) 
		{
		  if (EditLevel->AllWaypoints[ OriginWaypoint ].connections[k] == (-1) ) break;
		}
	      if ( k == MAX_WP_CONNECTIONS ) 
		{
		  strcat ( VanishingMessage , "\nSORRY. NO MORE CONNECTIONS AVAILABLE FROM THERE." );
		}
	      else
		{
		  EditLevel->AllWaypoints[ OriginWaypoint ].connections[k] = i;
		  strcat ( VanishingMessage , "\nOPERATION DONE!! CONNECTION SHOULD BE THERE." );
		}
	      OriginWaypoint = (-1);
	    }
	}
    }

}; // void ToggleWaypointConnection ( Level EditLevel , int BlockX , int BlockY )

/* ----------------------------------------------------------------------
 * With the 'P' key, you can edit the codepanel codeword attached to any 
 * codepanel.  Of course the cursor must be positioned at this codepanel
 * so the feature can work.  This function does all this.
 * ---------------------------------------------------------------------- */
void 
EditCodepanelData ( Level EditLevel )
{
  char* NewCommentOnThisSquare;
  int i;

  while (PPressed());
  SetCurrentFont( FPS_Display_BFont );

  // First we check if we really are directly on a codepanel:
  switch ( EditLevel->map [ (int)rintf( Me[0].pos.y) ] [ (int)rintf( Me[0].pos.x ) ] )
    {
    case CODEPANEL_L:
    case CODEPANEL_R:
    case CODEPANEL_U:
    case CODEPANEL_D:
      
      // If yes, we ask for the new codepanel keyword
      NewCommentOnThisSquare = 
	GetEditableStringInPopupWindow ( 1000 , "\n Please enter new codepanel codeword: \n\n" ,
					 "" );
      
      // Now we see if a codepanel entry is existing already for this square
      for ( i = 0 ; i < MAX_CODEPANELS_PER_LEVEL ; i ++ )
	{
	  if ( ( EditLevel->CodepanelList[ i ].x == (int)rintf( Me[0].pos.x) ) &&
	       ( EditLevel->CodepanelList[ i ].y == (int)rintf( Me[0].pos.y) ) ) break;
	}
      if ( i >= MAX_CODEPANELS_PER_LEVEL ) 
	{
	  DisplayText ( "\nNo existing codepanel entry found...\n" , -1 , -1 , &User_Rect );
	  i=0;
	  for ( i = 0 ; i < MAX_CODEPANELS_PER_LEVEL ; i ++ )
	    {
	      if ( EditLevel->CodepanelList[ i ].x == (-1) )
		break;
	    }
	  if ( i >= MAX_CODEPANELS_PER_LEVEL )
	    {
	      DisplayText ( "\nNo more free codepanel entry found... using first\n" , -1 , -1 , &User_Rect );
	      i = 0;
	    }
	  else
	    {
	      DisplayText ( "\nUsing new codepanel list entry...\n" , -1 , -1 , &User_Rect );
	    }
	  // Terminate( ERR );
	}
      else
	{
	  DisplayText ( "\nOverwriting existing codepanel list entry...\n" , -1 , -1 , &User_Rect );
	  
	}
      EditLevel->CodepanelList[ i ].Secret_Code = NewCommentOnThisSquare;
      EditLevel->CodepanelList[ i ].x = rintf( Me[0].pos.x );
      EditLevel->CodepanelList[ i ].y = rintf( Me[0].pos.y );
      
      
      SDL_Flip ( Screen );
      getchar_raw();
      
      break;
    default:
      DisplayText ( "\nBut you are not on a codepanel!!\n" , -1 , -1 , &User_Rect );
      SDL_Flip( Screen );
      getchar_raw();
      break;
    }
  
}; // void EditCodepanelData ( EditLevel )

/* ----------------------------------------------------------------------
 * With the 'M' key, you can edit the map labels.
 * The label will be assumed to be directly under the map cursor.
 * ---------------------------------------------------------------------- */
void 
EditMapLabelData ( Level EditLevel )
{
  char* NewCommentOnThisSquare;
  int i;

  while (PPressed());
  SetCurrentFont( FPS_Display_BFont );

  //--------------------
  // Now we see if a map label entry is existing already for this spot
  //
  for ( i = 0 ; i < MAX_MAP_LABELS_PER_LEVEL ; i ++ )
    {
      if ( ( EditLevel -> labels [ i ] . pos . x == (int)rintf( Me[0].pos.x) ) &&
	   ( EditLevel -> labels [ i ] . pos . y == (int)rintf( Me[0].pos.y) ) ) 
	{
	  break;
	}
    }
  if ( i >= MAX_MAP_LABELS_PER_LEVEL ) 
    {
      DisplayText ( "\nNo existing map label entry found...\n" , -1 , -1 , &User_Rect );
      NewCommentOnThisSquare = 
	GetEditableStringInPopupWindow ( 1000 , "\n Please enter new label for this map position: \n\n" ,
					 "" );

      i=0;
      for ( i = 0 ; i < MAX_MAP_LABELS_PER_LEVEL ; i ++ )
	{
	  if ( EditLevel -> labels [ i ] . pos . x == (-1) )
	    break;
	}
      if ( i >= MAX_CODEPANELS_PER_LEVEL )
	{
	  DisplayText ( "\nNo more free map label entry found... using first on instead ...\n" , -1 , -1 , &User_Rect );
	  i = 0;
	}
      else
	{
	  DisplayText ( "\nUsing new map label list entry...\n" , -1 , -1 , &User_Rect );
	}
      // Terminate( ERR );
    }
  else
    {
      DisplayText ( "\nOverwriting existing map label list entry...\n" , -1 , -1 , &User_Rect );
      NewCommentOnThisSquare = 
	GetEditableStringInPopupWindow ( 1000 , "\n Please enter new label for this map position: \n\n" ,
					 EditLevel -> labels [ i ] . label_name );
    }

  //--------------------
  // At this point, we've got our new index for a good map label list
  // position we can use.  But we'll only fill in something if the string
  // given wasn't empty.  Otherwise, the old label is to be deleted.
  // 
  if ( strlen ( NewCommentOnThisSquare ) )
    {
      EditLevel -> labels [ i ] . label_name = NewCommentOnThisSquare;
      EditLevel -> labels [ i ] . pos . x = rintf( Me[0].pos.x );
      EditLevel -> labels [ i ] . pos . y = rintf( Me[0].pos.y );
    }
  else
    {
      EditLevel -> labels [ i ] . label_name = "NoLabelHere" ;
      EditLevel -> labels [ i ] . pos . x = (-1) ;
      EditLevel -> labels [ i ] . pos . y = (-1) ;
    }

  SDL_Flip ( Screen );
  
}; // void EditMapLabelData ( EditLevel )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
RecFillMap ( Level EditLevel , int BlockY , int BlockX , int SpecialMapValue )
{
  int SourceAreaTileType=EditLevel->map[BlockY][BlockX];

  EditLevel->map[BlockY][BlockX] = SpecialMapValue ;

  if ( BlockX > 0 )
    {
      if ( EditLevel->map[BlockY][BlockX-1] == SourceAreaTileType )
	RecFillMap ( EditLevel , BlockY , BlockX -1 , SpecialMapValue );
    }
  if ( BlockX < EditLevel->xlen -1 )
    {
      if ( EditLevel->map[BlockY][BlockX+1] == SourceAreaTileType )
	RecFillMap ( EditLevel , BlockY , BlockX +1 , SpecialMapValue );
    }
  if ( BlockY > 0 )
    {
      if ( EditLevel->map[BlockY-1][BlockX] == SourceAreaTileType )
	RecFillMap ( EditLevel , BlockY-1 , BlockX , SpecialMapValue );
    }
  if ( BlockY < EditLevel->ylen -1 )
    {
      if ( EditLevel->map[BlockY+1][BlockX] == SourceAreaTileType )
	RecFillMap ( EditLevel , BlockY+1 , BlockX , SpecialMapValue );
    }
};

/* ----------------------------------------------------------------------
 * This function is provides the Level Editor integrated into 
 * freedroid.  Actually this function is a submenu of the big
 * Escape Menu.  In here you can edit the level and, upon pressing
 * escape, you can enter a new submenu where you can save the level,
 * change level name and quit from level editing.
 * ---------------------------------------------------------------------- */
void 
LevelEditor(void)
{
  int BlockX=rintf(Me[0].pos.x);
  int BlockY=rintf(Me[0].pos.y);
  int Done=FALSE;
  int Weiter=FALSE;
  int i ;
  int SpecialMapValue;
  int NewItemCode;
  char* NumericInputString;
  char linebuf[10000];
  long OldTicks;
  SDL_Rect Editor_Window;
  Level EditLevel;
  char* NewCommentOnThisSquare;
  int LeftMousePressedPreviousFrame = FALSE;
  point TargetSquare;
  int new_x, new_y;

  GameConfig.Inventory_Visible = FALSE;
  GameConfig.CharacterScreen_Visible = FALSE;
  GameConfig.SkillScreen_Visible = FALSE;
  RespectVisibilityOnMap = FALSE ;

  EditLevel = curShip.AllLevels [ Me [ 0 ] . pos . z ] ;

  strcpy ( VanishingMessage , "Hello" );
  VanishingMessageDisplayTime = 0 ;
  OriginWaypoint = (-1);

  Editor_Window.x=User_Rect.x;
  Editor_Window.y=User_Rect.y;  
  Editor_Window.w=User_Rect.w;
  Editor_Window.h=User_Rect.h;
  
  while ( !Done )
    {
      Weiter=FALSE;
      OldTicks = SDL_GetTicks ( ) ;
      while ( ( ! EscapePressed() ) && ( !Done ) )
	{
	  //--------------------
	  // Also in the Level-Editor, there is no need to go at full framerate...
	  // We can do with less, cause there are no objects supposed to be 
	  // moving fluently anyway.  Therefore we introduce some rest for the CPU.
	  //
	  usleep ( 2 );

	  BlockX=rintf(Me[0].pos.x);
	  BlockY=rintf(Me[0].pos.y);
	  
	  EditLevel = curShip.AllLevels [ Me [ 0 ] . pos . z ] ;	  
	  GetAllAnimatedMapTiles ( EditLevel );

	  VanishingMessageDisplayTime += ( SDL_GetTicks ( ) - OldTicks ) / 1000.0 ;
	  OldTicks = SDL_GetTicks ( ) ;

	  ClearUserFenster();
	  AssembleCombatPicture ( ONLY_SHOW_MAP_AND_TEXT | SHOW_GRID );
	  Highlight_Current_Block();
	  ShowWaypoints( FALSE );
	  ShowMapLabels( );
	  
	  SetCurrentFont ( FPS_Display_BFont ) ;

	  //--------------------
	  // Now we print out the current status directly onto the window:
	  //

	  if ( OriginWaypoint == ( -1 ) )
	    {
	      sprintf ( linebuf , " Source Waypoint selected : NONE" );
	    }
	  else
	    {
	      sprintf ( linebuf , " Source Waypoint selected : X=%d Y=%d. " , 
			EditLevel -> AllWaypoints [ OriginWaypoint ] . x , 
			EditLevel -> AllWaypoints [ OriginWaypoint ] . y );
	    }
	  LeftPutString ( Screen , 91 + 4 * FontHeight( GetCurrentFont() ), linebuf );

	  //--------------------
	  // Now we print out the latest connection operation success or failure...
	  //
	  if ( VanishingMessageDisplayTime < 7 )
	    {
	      DisplayText ( VanishingMessage ,  1 , 191 + 5 * FontHeight ( GetCurrentFont () ) , NULL );
	    }

	  ShowLevelEditorTopMenu( Highlight );
	  if ( EditLevel -> jump_target_north >= 0 )
	    ShowGenericButtonFromList ( GO_LEVEL_NORTH_BUTTON );
	  if ( EditLevel -> jump_target_south >= 0 )
	    ShowGenericButtonFromList ( GO_LEVEL_SOUTH_BUTTON );
	  if ( EditLevel -> jump_target_east >= 0 )
	    ShowGenericButtonFromList ( GO_LEVEL_EAST_BUTTON );
	  if ( EditLevel -> jump_target_west >= 0 )
	    ShowGenericButtonFromList ( GO_LEVEL_WEST_BUTTON );
	  ShowGenericButtonFromList ( EXPORT_THIS_LEVEL_BUTTON );

	  ShowGenericButtonFromList ( LEVEL_EDITOR_SAVE_SHIP_BUTTON );
	  ShowGenericButtonFromList ( LEVEL_EDITOR_ZOOM_IN_BUTTON );
	  ShowGenericButtonFromList ( LEVEL_EDITOR_ZOOM_OUT_BUTTON );
	  ShowGenericButtonFromList ( LEVEL_EDITOR_RECURSIVE_FILL_BUTTON );
	  ShowGenericButtonFromList ( LEVEL_EDITOR_NEW_MAP_LABEL_BUTTON );
	  ShowGenericButtonFromList ( LEVEL_EDITOR_KEYMAP_BUTTON );
	  ShowGenericButtonFromList ( LEVEL_EDITOR_QUIT_BUTTON );

	  //--------------------
	  // Now that everything is blitted and printed, we may update the screen again...
	  //
	  SDL_Flip( Screen );

	  //--------------------
	  // If the user of the Level editor pressed some cursor keys, move the
	  // highlited filed (that is Me[0].pos) accordingly. This is done here:
	  //
	  HandleLevelEditorCursorKeys();

	  //--------------------
	  // With the 'S' key, you can attach a statement for the influencer to 
	  // say to a given location, i.e. the location the map editor cursor
	  // currently is on.
	  //
	  if ( SPressed () )
	    {
	      while (SPressed());
	      SetCurrentFont( FPS_Display_BFont );
	      NewCommentOnThisSquare = 
		GetEditableStringInPopupWindow ( 1000 , "\n Please enter new statement for this tile: \n\n" ,
						 "" );
	      for ( i = 0 ; i < MAX_STATEMENTS_PER_LEVEL ; i ++ )
		{
		  if ( EditLevel->StatementList[ i ].x == (-1) ) break;
		}
	      if ( i == MAX_STATEMENTS_PER_LEVEL ) 
		{
		  DisplayText ( "\nNo more free comment position.  Using first. " , -1 , -1 , &User_Rect );
		  i=0;
		  SDL_Flip ( Screen );
		  getchar_raw();
		  // Terminate( ERR );
		}

	      EditLevel->StatementList[ i ].Statement_Text = NewCommentOnThisSquare;
	      EditLevel->StatementList[ i ].x = rintf( Me[0].pos.x );
	      EditLevel->StatementList[ i ].y = rintf( Me[0].pos.y );
	    }

	  //--------------------
	  // With the 'P' key, you can edit the codepanel codeword attached to any 
	  // codepanel.  Of course the cursor must be positioned at this codepanel
	  // so the feature can work.
	  //
	  if ( PPressed () ) EditCodepanelData ( EditLevel );

	  //--------------------
	  // With the 'M' key, you can edit the current map label.
	  // The label will be assumed to be directly under the cursor.
	  //
	  if ( MPressed () ) EditMapLabelData ( EditLevel );

	  //--------------------
	  // Since the level editor will not always be able to
	  // immediately feature all the the map tiles that might
	  // have been added recently, we should offer a feature, so that you can
	  // specify the value of a map piece just numerically.  This will be
	  // done upon pressing the 'e' key.
	  //
	  if ( EPressed () )
	    {
	      while (EPressed());
	      NumericInputString = 
		GetEditableStringInPopupWindow ( 10 , "\n You have chosen to enter a floor tile by it's numeric value.\n\nPlease enter the numeric value of the tile below: \n\n" ,
						 "" );
	      sscanf( NumericInputString , "%d" , &SpecialMapValue );
	      if ( SpecialMapValue >= NUM_MAP_BLOCKS ) SpecialMapValue=0;

	      if ( Shift_Was_Pressed() )
		{
		  RecFillMap ( EditLevel , BlockY , BlockX , SpecialMapValue );
		}
	      else
		{
		  EditLevel->map[BlockY][BlockX]=SpecialMapValue;
		}
	    }

	  //--------------------
	  // From the level editor, it should also be possible to drop new goods
	  // at some location via the 'G' key. (G like in Goods.)
	  //
	  if ( GPressed () )
	    {
	      while ( GPressed() );
	      NumericInputString = 
		GetEditableStringInPopupWindow ( 10 , "\n You have chosen to drop an item to the floor here.\n\nPlease enter the numeric value of the new item below: \n\n" ,
						 "" );
	      sscanf( NumericInputString , "%d" , &NewItemCode );
	      if ( NewItemCode >= Number_Of_Item_Types ) 
		{
		  NewItemCode=0;
		  
		}

	      // DropSpecificItemAtPosition( rintf( Me[0].pos.x ) , rintf( Me[0].pos.y ) , NewItemCode );
	      DropItemAt( NewItemCode , rintf( Me[0].pos.x ) , rintf( Me[0].pos.y ) , -1 , -1 , 0 );
	    }

	  //--------------------
	  // If the person using the level editor decides he/she wants a different
	  // scale for the editing process, he/she may say so by using the O/I keys.
	  //
	  if ( OPressed () ) ZoomOut();
	  if ( IPressed () )
	    {
	      ZoomIn();
	    }
  
	  //--------------------
	  // If the person using the level editor pressed w, the waypoint is
	  // toggled on the current square.  That means either removed or added.
	  // And in case of removal, also the connections must be removed.
	  //
	  if (WPressed())
	    {
	      ToggleWaypoint ( EditLevel , BlockX, BlockY );
	      while ( WPressed() );
	    }

	  //--------------------
	  // If the person using the level editor presses C that indicated he/she wants
	  // a connection between waypoints.  If this is the first selected waypoint, its
	  // an origin and the second "C"-pressed waypoint will be used a target.
	  // If origin and destination are the same, the operation is cancelled.
	  //
	  if (CPressed())
	    {
	      ToggleWaypointConnection ( EditLevel, BlockX, BlockY );
	      while (CPressed());
	      fflush(stdout);
	    }

	  //--------------------
	  // Pressing the 'B' key will toggle the big graphics insert by one...
	  //
	  if ( BPressed()) 
	    {
	      while ( BPressed() );
	      ToggleBigGraphicsInserts ( EditLevel , BlockX, BlockY );
	    }
	  
	  //----------------------------------------------------------------------
	  // If the person using the level editor pressed some editing keys, insert the
	  // corresponding map tile.  This is done in the following:
	  //
	  HandleMapTileEditingKeys ( EditLevel , BlockX , BlockY );

	  //--------------------
	  // First we find out which map square the player MIGHT wish us to operate on
	  // via a POTENTIAL mouse click
	  //
	  TargetSquare.x = rintf ( Me [ 0 ] . pos . x + (float)( GetMousePos_x ( ) + 16 - ( SCREEN_WIDTH / 2 ) ) / ( (float)INITIAL_BLOCK_WIDTH * CurrentCombatScaleFactor ) ) ;
	  TargetSquare.y = rintf ( Me [ 0 ] . pos . y + (float)( GetMousePos_y ( ) + 16 - ( SCREEN_HEIGHT / 2 ) ) / ( (float)INITIAL_BLOCK_HEIGHT * CurrentCombatScaleFactor ) ) ;
		  
	  if ( axis_is_active && !LeftMousePressedPreviousFrame )
	    {
	      if ( ClickWasInEditorBannerRect() )
		HandleBannerMouseClick();
	      else if ( CursorIsOnButton ( GO_LEVEL_NORTH_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) )
		{
		  if ( Me [ 0 ] . pos . x < curShip . AllLevels [ EditLevel -> jump_target_north ] -> xlen -1 )
		    new_x = Me [ 0 ] . pos . x ; 
		  else 
		    new_x = 3;
		  new_y = curShip . AllLevels [ EditLevel -> jump_target_north ] -> xlen - 4 ;
		  if ( EditLevel -> jump_target_north >= 0 ) 
		    Teleport ( EditLevel -> jump_target_north , new_x , new_y , 0 , TRUE , FALSE );
		}
	      else if ( CursorIsOnButton ( GO_LEVEL_SOUTH_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) )
		{
		  if ( Me [ 0 ] . pos . x < curShip . AllLevels [ EditLevel -> jump_target_south ] -> xlen -1 )
		    new_x = Me [ 0 ] . pos . x ; 
		  else 
		    new_x = 3;
		  new_y = 4;
		  if ( EditLevel -> jump_target_south >= 0 ) 
		    Teleport ( EditLevel -> jump_target_south , new_x , new_y , 0 , TRUE , FALSE );
		}
	      else if ( CursorIsOnButton ( GO_LEVEL_EAST_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) )
		{
		  new_x = 3;
		  if ( Me [ 0 ] . pos . y < curShip . AllLevels [ EditLevel -> jump_target_east ] -> ylen -1 )
		    new_y = Me [ 0 ] . pos . y ; 
		  else 
		    new_y = 4;
		  if ( EditLevel -> jump_target_east >= 0 ) 
		    Teleport ( EditLevel -> jump_target_east , new_x , new_y , 0 , TRUE , FALSE );
		}
	      else if ( CursorIsOnButton ( GO_LEVEL_WEST_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) )
		{
		  new_x = curShip . AllLevels [ EditLevel -> jump_target_west ] -> xlen -4 ;
		  if ( Me [ 0 ] . pos . y < curShip . AllLevels [ EditLevel -> jump_target_west ] -> ylen -1 )
		    new_y = Me [ 0 ] . pos . y ; 
		  else 
		    new_y = 4;
		  if ( EditLevel -> jump_target_west >= 0 ) 
		    Teleport ( EditLevel -> jump_target_west , new_x , new_y , 0 , TRUE , FALSE );
		}
	      else if ( CursorIsOnButton ( EXPORT_THIS_LEVEL_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) )
		{
		  ExportLevelInterface ( Me [ 0 ] . pos . z );
		}
	      else if ( CursorIsOnButton ( LEVEL_EDITOR_SAVE_SHIP_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) )
		{
		  SaveShip("Testship.shp");

		  // CenteredPutString ( Screen ,  11*FontHeight(Menu_BFont),    "Your ship was saved...");
		  // SDL_Flip ( Screen );

		  GiveMouseAlertWindow ( "\nM E S S A G E\n\nYour ship was saved to file 'Testship.shp'.\nIf you are sure, that you wish to use this file in the game, copy it over the 'maps/Asteroid.maps' file so that FreedroidRPG will really use it.\n\nIf you have set up something cool and you wish to contribute it to FreedroidRPG, please contact the FreedroidRPG dev team." ) ;

		}
	      else if ( CursorIsOnButton ( LEVEL_EDITOR_ZOOM_IN_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) )
		{
		  ZoomIn();
		}
	      else if ( CursorIsOnButton ( LEVEL_EDITOR_ZOOM_OUT_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) )
		{
		  ZoomOut();
		}
	      else if ( CursorIsOnButton ( LEVEL_EDITOR_RECURSIVE_FILL_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) )
		{
		  RecFillMap ( EditLevel , BlockY , BlockX , Highlight );
		}
	      else if ( CursorIsOnButton ( LEVEL_EDITOR_NEW_MAP_LABEL_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) )
		{
		  EditMapLabelData ( EditLevel );
		}
	      else if ( CursorIsOnButton ( LEVEL_EDITOR_KEYMAP_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) )
		{
		  ShowLevelEditorKeymap (  );
		}
	      else if ( CursorIsOnButton ( LEVEL_EDITOR_QUIT_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) )
		{
		  Weiter=!Weiter;
		  Done=TRUE;
		  SetCombatScaleTo( 1 );
		  Me [ 0 ] . mouse_move_target . x = Me [ 0 ] . pos . x ;
		  Me [ 0 ] . mouse_move_target . y = Me [ 0 ] . pos . y ;
		  Me [ 0 ] . mouse_move_target . z = Me [ 0 ] . pos . z ;
		  Me [ 0 ] . mouse_move_target_is_enemy = ( -1 ) ;
		}
	      else
		{
		  //--------------------
		  // Maybe a left mouse click has in the map area.  Then it might be best to interpret this
		  // simply as bigger move command, which might indeed be much handier than 
		  // using only keyboard cursor keys to move around on the map.
		  //
		  Me [ 0 ] . pos . x += ( GetMousePos_x ( ) + 16 - ( SCREEN_WIDTH / 2 ) ) / ( INITIAL_BLOCK_WIDTH * CurrentCombatScaleFactor ) ;
		  if ( Me [ 0 ] . pos . x >= curShip.AllLevels[Me[0].pos.z]->xlen-2 )
		    Me [ 0 ] . pos . x = curShip.AllLevels[Me[0].pos.z]->xlen-2 ;
		  if ( Me [ 0 ] . pos . x <= 2 ) Me [ 0 ] . pos . x = 2;
		  
		  Me [ 0 ] . pos . y += ( GetMousePos_y ( ) + 16 - ( SCREEN_HEIGHT / 2 ) ) / ( INITIAL_BLOCK_WIDTH * CurrentCombatScaleFactor ) ;
		  if ( Me [ 0 ] . pos . y >= curShip.AllLevels[Me[0].pos.z]->ylen-2 )
		    Me [ 0 ] . pos . y = curShip.AllLevels[Me[0].pos.z]->ylen-2 ;
		  if ( Me [ 0 ] . pos . y <= 2 ) Me [ 0 ] . pos . y = 2;
		}
	    }

	  //--------------------
	  // With the right mouse button, it should be possible to actually 'draw'
	  // something into the level.  This seems to work so far.  Caution is nescessary
	  // to prevent segfault due to writing outside the level, but that's easily
	  // accomplished.
	  //
	  if ( MouseRightPressed() )
	    {
	      if ( ( TargetSquare . x >= 0 ) &&
		   ( TargetSquare . x <= EditLevel->xlen-1 ) &&
		   ( TargetSquare . y >= 0 ) &&
		   ( TargetSquare . y <= EditLevel->ylen-1 ) )
		EditLevel->map[ TargetSquare . y ] [ TargetSquare . x ] = Highlight ;	      
	    }

	  if (QPressed())
	    {
	      Terminate(0);
	    }

	  LeftMousePressedPreviousFrame = axis_is_active; 

	} // while (!EscapePressed())
      while( EscapePressed() );

      //--------------------
      // After Level editing is done and escape has been pressed, 
      // display the Menu with level save options and all that.
      //
      if ( !Done ) Done = DoLevelEditorMainMenu ( EditLevel );
      
    } // while (!Done)

  RespectVisibilityOnMap = TRUE ;

}; // void LevelEditor ( void )

#undef _leveleditor_c
