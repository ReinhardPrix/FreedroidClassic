/* 
 *
 *   Copyright (c) 2002, 2003 Johannes Prix
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

#include "SDL_rotozoom.h"

void ShowWaypoints( int PrintConnectionList , int maks );
void LevelEditor(void);
Level CreateNewMapLevel( void );
void SetLevelInterfaces ( void );
void delete_obstacle ( level* EditLevel , obstacle* our_obstacle );
void duplicate_all_obstacles_in_area ( Level source_level ,
				       float source_start_x , float source_start_y , 
				       float source_area_width , float source_area_height ,
				       Level target_level ,
				       float target_start_x , float target_start_y );
void add_obstacle ( Level EditLevel , float x , float y , int new_obstacle_type );

// EXTERN SDL_Surface *BackupMapBlockSurfacePointer[ NUM_COLORS ][ NUM_MAP_BLOCKS ];

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
 *
 *
 * ---------------------------------------------------------------------- */
void
create_new_obstacle_on_level ( Level EditLevel , int our_obstacle_type , float pos_x , float pos_y )
{
  int i;
  int free_index = ( -1 ) ;

  //--------------------
  // First we find a free obstacle index to insert our new obstacle
  //
  for ( i = 0 ; i < MAX_OBSTACLES_ON_MAP ; i ++ )
    {
      if ( EditLevel -> obstacle_list [ i ] . type == (-1) )
	{
	  free_index = i ;
	  break;
	}
    }
  if ( free_index < 0 )
    {
      GiveStandardErrorMessage ( "create_new_obstacle_on_level (...)" , "\
Ran out of obstacles!   Too bad!  Raise max obstacles constant!" , 
				 PLEASE_INFORM , IS_FATAL );
    }

  //--------------------
  // Now that we have an obstacle at our disposal, we can start to fill in
  // the right obstacle properties.
  //
  EditLevel -> obstacle_list [ free_index ] . type = our_obstacle_type; 
  EditLevel -> obstacle_list [ free_index ] . pos . x = pos_x ;
  EditLevel -> obstacle_list [ free_index ] . pos . y = pos_y ;
  EditLevel -> obstacle_list [ free_index ] . name_index = (-1) ;

  //--------------------
  // Now that the new obstacle has been created, it must still be glued to the
  // floor on some tile, so that the engine will recognize the need to blit it.
  //
  glue_obstacles_to_floor_tiles_for_level ( EditLevel -> levelnum );

}; // void create_new_obstacle_on_level ( Level EditLevel , int our_obstacle_type , float pos_x , float pos_y )

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
 * This function drops an item onto the floor.  It works with a selection
 * of item images and clicking with the mouse on an item image or on one
 * of the buttons presented to the person editing the level.
 * ---------------------------------------------------------------------- */
void
ItemDropFromLevelEditor( void )
{
  int SelectionDone = FALSE;
  int NewItemCode = ( -1 );
  int i;
  int j;
  item temp_item;
  int row_len = 5 ;
  int line_len = 8 ; 
  int item_group = 0 ; 

  while ( GPressed() );

  while ( !SelectionDone )
    {

      while ( SpacePressed() );

      SDL_FillRect ( Screen , NULL , 0 );

      for ( j = 0 ; j < row_len ; j ++ )
	{
	  for ( i = 0 ; i < line_len ; i ++ ) 
	    {
	      temp_item . type = i + j * line_len + item_group * line_len * row_len ;
	      if ( temp_item.type >= Number_Of_Item_Types )  temp_item.type = 1 ;
	      ShowRescaledItem ( i , 32 + (64+2) * j, & ( temp_item ) );
	    }
	}

      ShowGenericButtonFromList ( LEVEL_EDITOR_NEXT_ITEM_GROUP_BUTTON );
      ShowGenericButtonFromList ( LEVEL_EDITOR_PREV_ITEM_GROUP_BUTTON );
      ShowGenericButtonFromList ( LEVEL_EDITOR_CANCEL_ITEM_DROP_BUTTON );

      SDL_Flip( Screen );

      while ( ! SpacePressed() ) usleep ( 2 );

      if ( CursorIsOnButton ( LEVEL_EDITOR_NEXT_ITEM_GROUP_BUTTON ,
			      GetMousePos_x()+16 , GetMousePos_y()+16 ) )
	{
	  item_group ++ ;
	}
      else if ( CursorIsOnButton ( LEVEL_EDITOR_PREV_ITEM_GROUP_BUTTON ,
			      GetMousePos_x()+16 , GetMousePos_y()+16 ) )
	{
	  if ( item_group > 0 ) item_group -- ;
	}
      else if ( CursorIsOnButton ( LEVEL_EDITOR_CANCEL_ITEM_DROP_BUTTON ,
			      GetMousePos_x()+16 , GetMousePos_y()+16 ) )
	{
	  return ;
	}
      else if ( (GetMousePos_x()+16 > 55 ) && ( GetMousePos_x()+16 < 55 + 64 * line_len ) &&
		(GetMousePos_y()+16 > 32 ) && ( GetMousePos_y()+16 < 32 + 66 * row_len ) )
	{
	  SelectionDone = TRUE ;
	  NewItemCode = ( ( GetMousePos_x()+16 - 55 ) / 64 + ( GetMousePos_y()+16 - 32 ) / 66 * line_len + item_group * line_len * row_len ) ;
	}
    }

  if ( NewItemCode >= Number_Of_Item_Types ) 
    {
      NewItemCode=0;
    }
  
  DropItemAt( NewItemCode , rintf( Me[0].pos.x ) , rintf( Me[0].pos.y ) , -1 , -1 , 0 );

  while ( SpacePressed() );

}; // void ItemDropFromLevelEditor( void )

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
      if ( FirstBlock < NUMBER_OF_OBSTACLE_TYPES -8 ) FirstBlock+= 8;
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
	}
    }

  //--------------------
  // Now some extra security against selecting indices that would point to
  // undefined objects (floor tiles or obstacles) later
  //
  if ( GameConfig . level_editor_edit_mode == LEVEL_EDITOR_EDIT_FLOOR )
    {
      if ( Highlight >= ALL_ISOMETRIC_FLOOR_TILES )
	Highlight = ALL_ISOMETRIC_FLOOR_TILES - 1;
    }
  else if ( GameConfig . level_editor_edit_mode == LEVEL_EDITOR_EDIT_OBSTACLES )
    {
      if ( Highlight >= NUMBER_OF_OBSTACLE_TYPES )
	Highlight = NUMBER_OF_OBSTACLE_TYPES - 1;
    }

}; // void HandleBannerMouseClick( void )

/* ----------------------------------------------------------------------
 * On the very top of the screen during level editor work, there is a 
 * scrollbar with the various tiles that may be placed on the floor of the
 * map.  This scrollbar is drawn here.
 * ---------------------------------------------------------------------- */
void
ShowLevelEditorTopMenu( int Highlight )
{
  int i;
  SDL_Rect TargetRectangle;
  int selected_index = FirstBlock;
  static SDL_Surface *LevelEditorTopBanner = NULL;
  SDL_Surface *tmp = NULL;
  char* fpath;
  float zoom_factor;

  //--------------------
  // At first we check if we might need to load the top status and
  // selection banner.  This will have to be done only once.
  //
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
  
  //--------------------
  // Now we blit the top status and selection banner background.  Fine.
  //
  SDL_BlitSurface ( LevelEditorTopBanner , NULL , Screen , &EditorBannerRect );

  //--------------------
  // Time to fill something into the top selection banner, so that the
  // user can really has something to select from there.  But this must be
  // done differently, depending on whether we show the menu for the floor
  // edit mode or for the obstacle edit mode.
  //
  if ( GameConfig . level_editor_edit_mode == LEVEL_EDITOR_EDIT_FLOOR )
    {
      for ( i = 0 ; i < 9 ; i ++ )
	{
	  TargetRectangle.x = INITIAL_BLOCK_WIDTH/2 + INITIAL_BLOCK_WIDTH * i ;
	  TargetRectangle.y = INITIAL_BLOCK_HEIGHT/3 ;
	  
	  //--------------------
	  // We create a scaled version of the floor tile in question
	  //
	  tmp = zoomSurface ( floor_iso_images [ selected_index ] . surface , 0.5 , 0.5, FALSE );
	  
	  //--------------------
	  // Now we can show and free the scaled verion of the floor tile again.
	  //
	  SDL_BlitSurface( tmp , NULL , Screen, &TargetRectangle);
	  SDL_FreeSurface ( tmp );
	  
	  if ( selected_index == Highlight ) 
	    HighlightRectangle ( Screen , TargetRectangle );
	  
	  if ( selected_index < ALL_ISOMETRIC_FLOOR_TILES -1 ) selected_index ++ ;
	}
    }
  else if ( GameConfig . level_editor_edit_mode == LEVEL_EDITOR_EDIT_OBSTACLES )
    {
      for ( i = 0 ; i < 9 ; i ++ )
	{
	  if ( selected_index >= NUMBER_OF_OBSTACLE_TYPES ) continue ;


	  TargetRectangle.x = INITIAL_BLOCK_WIDTH/2 + INITIAL_BLOCK_WIDTH * i ;
	  TargetRectangle.y = INITIAL_BLOCK_HEIGHT/3 ;
	  
	  //--------------------
	  // We create a scaled version of the obstacle in question
	  //
	  zoom_factor = min ( 
			     ( (float)INITIAL_BLOCK_WIDTH / (float)obstacle_map [ selected_index ] . image . surface->w ) ,
			     ( (float)INITIAL_BLOCK_HEIGHT / (float)obstacle_map [ selected_index ] . image . surface->h ) );
	  tmp = zoomSurface ( obstacle_map [ selected_index ] . image . surface , zoom_factor , zoom_factor , FALSE );
	  
	  //--------------------
	  // Now we can show and free the scaled verion of the floor tile again.
	  //
	  SDL_BlitSurface( tmp , NULL , Screen, &TargetRectangle);
	  SDL_FreeSurface ( tmp );
	  
	  if ( selected_index == Highlight ) 
	    HighlightRectangle ( Screen , TargetRectangle );
	  
	  if ( selected_index < NUMBER_OF_OBSTACLE_TYPES -1 ) selected_index ++ ;
	}
    }
  else
    {
      GiveStandardErrorMessage ( "ShowLevelEditorTopMenu (...)" , "\
Unhandles level editor edit mode received.",
				 PLEASE_INFORM , IS_FATAL );
    }

  ShowGenericButtonFromList ( LEFT_LEVEL_EDITOR_BUTTON ) ;
  ShowGenericButtonFromList ( RIGHT_LEVEL_EDITOR_BUTTON ) ;

}; // void ShowLevelEditorTopMenu( void )

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
  map_tile* OldMapPointer;

  EditLevel->xlen++;
  // In case of enlargement, we need to do more:
  for ( i = 0 ; i < EditLevel->ylen ; i++ )
    {
      OldMapPointer=EditLevel->map[i];
      EditLevel->map[i] = MyMalloc ( sizeof ( map_tile ) * ( EditLevel->xlen +1) ) ;
      memcpy( EditLevel->map[i] , OldMapPointer , EditLevel->xlen-1 );
      // We don't want to fill the new area with junk, do we? So we make it floor tiles
      EditLevel->map[ i ] [ EditLevel->xlen-1 ] . floor_value = FLOOR;  
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
      EditLevel->map [ i ] [ EditLevel->xlen - EditLevel->jump_threshold_east - 1 ] . floor_value = FLOOR ;
    }

  MoveWaypointsEastOf ( EditLevel->xlen - EditLevel->jump_threshold_east - 1 , +1 , EditLevel ) ;
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
  map_tile* temp;
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
  char* MenuTexts[ 100 ];
  char Options [ 20 ] [1000];
  int Weiter = FALSE ;
  int MenuPosition=1;
  int Done=FALSE;
  int i;

enum
  { 
    SAVE_LEVEL_POSITION=1, 
    CHANGE_LEVEL_POSITION, 
    CHANGE_TILE_SET_POSITION, 
    CHANGE_SIZE_X, 
    SET_LEVEL_NAME , 
    SET_BACKGROUND_SONG_NAME , 
    SET_LEVEL_COMMENT, 
    ADD_NEW_LEVEL , 
    SET_LEVEL_INTERFACE_POSITION , 
    EDIT_LEVEL_DIMENSIONS,
    QUIT_LEVEL_EDITOR_POSITION 
};

  while (!Weiter)
    {
      
      EditLevel = curShip.AllLevels [ Me [ 0 ] . pos . z ] ;

      InitiateMenu( NULL );
      
      i=0;
      MenuTexts[ i ] = "Save whole ship to 'Testship.shp'" ; i++;
      sprintf( Options [ 0 ] , "Current: %d.  Level Up/Down" , EditLevel->levelnum );
      MenuTexts[ i ] = Options [ 0 ]; i++;
      MenuTexts[ i ] = "Change tile set" ; i++;
      sprintf( Options [ 1 ] , "Current levelsize: %d x %d map tiles." , EditLevel->xlen , EditLevel->ylen );
      MenuTexts[ i ] = Options [ 1 ]; i++;
      sprintf( Options [ 3 ] , "Level name: %s" , EditLevel->Levelname );
      MenuTexts[ i ] = Options [ 3 ] ; i++;
      sprintf( Options [ 4 ] , "Background music file name: %s" , EditLevel->Background_Song_Name );
      MenuTexts[ i ] = Options [ 4 ] ; i++;
      sprintf( Options [ 5 ] , "Set Level Comment: %s" , EditLevel->Level_Enter_Comment );
      MenuTexts[ i ] = Options [ 5 ] ; i++;
      MenuTexts[ i ] = "Add completely new level" ; i++;
      MenuTexts[ i ] = "Set Level Interfaces" ; i++;
      MenuTexts[ i ] = "Edit Level Dimensions" ; i++;
      MenuTexts[ i ] = "Quit Level Editor" ; i++;
      MenuTexts[ i ] = "" ; i++;
	  
      MenuPosition = DoMenuSelection( "" , MenuTexts , -1 , NULL , FPS_Display_BFont );

      
      while (EnterPressed() || SpacePressed() );
      
      switch (MenuPosition) 
	{
	  
	case (-1):
	  while ( EscapePressed() );
	  Weiter=!Weiter;
	  // if ( CurrentCombatScaleFactor != 1 ) SetCombatScaleTo( 1 );
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
 * If we want to synchronize two levels, we need to remove the old obstacles
 * before we can add new ones.  Else the place might get too crowded with
 * obstacles. :)
 * ---------------------------------------------------------------------- */
void
delete_all_obstacles_in_area ( Level TargetLevel , float start_x , float start_y , float area_width , float area_height )
{
  int i;
  
  for ( i = 0 ; i < MAX_OBSTACLES_ON_MAP ; i ++ )
    {
      if ( TargetLevel -> obstacle_list [ i ] . type <= (-1) ) continue;
      if ( TargetLevel -> obstacle_list [ i ] . pos . x < start_x ) continue;
      if ( TargetLevel -> obstacle_list [ i ] . pos . y < start_y ) continue;
      if ( TargetLevel -> obstacle_list [ i ] . pos . x > start_x + area_width ) continue;
      if ( TargetLevel -> obstacle_list [ i ] . pos . y > start_y + area_height ) continue;
      delete_obstacle ( TargetLevel , & ( TargetLevel -> obstacle_list [ i ] ) );
      i--; // this is so that this obstacle will be processed AGAIN, since deleting might
           // have moved a different obstacle to this list position.
    }
}; // void delete_all_obstacles_in_area ( curShip . AllLevels [ TargetLevel ] , 0 , TargetLevel->ylen-AreaHeight , AreaWidth , AreaHeight )

/* ----------------------------------------------------------------------
 * This function should allow for conveninet duplication of obstacles from
 * one map to the other.  It assumes, that the target area has been cleaned
 * out of obstacles already.
 * ---------------------------------------------------------------------- */
void
duplicate_all_obstacles_in_area ( Level source_level ,
				  float source_start_x , float source_start_y , 
				  float source_area_width , float source_area_height ,
				  Level target_level ,
				  float target_start_x , float target_start_y )
{
  int i;

  for ( i = 0 ; i < MAX_OBSTACLES_ON_MAP ; i ++ )
    {
      if ( source_level -> obstacle_list [ i ] . type <= (-1) ) continue;
      if ( source_level -> obstacle_list [ i ] . pos . x < source_start_x ) continue;
      if ( source_level -> obstacle_list [ i ] . pos . y < source_start_y ) continue;
      if ( source_level -> obstacle_list [ i ] . pos . x > source_start_x + source_area_width ) continue;
      if ( source_level -> obstacle_list [ i ] . pos . y > source_start_y + source_area_height ) continue;

      add_obstacle ( target_level , 
		     target_start_x  + source_level -> obstacle_list [ i ] . pos . x - source_start_x ,
		     target_start_y  + source_level -> obstacle_list [ i ] . pos . y - source_start_y ,
		     source_level -> obstacle_list [ i ] . type );

		    //delete_obstacle ( source_level , & ( source_level -> obstacle_list [ i ] ) );
		    // i--; // this is so that this obstacle will be processed AGAIN, since deleting might
		    // // have moved a different obstacle to this list position.
    }
  
}; // void duplicate_all_obstacles_in_area ( ... )
      
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
      
      delete_all_obstacles_in_area ( curShip . AllLevels [ TargetLevel ] , 0 , curShip . AllLevels [ TargetLevel ] ->ylen-AreaHeight , AreaWidth , AreaHeight );
      

      duplicate_all_obstacles_in_area ( curShip . AllLevels [ LevelNum ] , 0 , 0 , 
					AreaWidth , AreaHeight ,
					curShip . AllLevels [ TargetLevel ] , 0 , curShip . AllLevels [ TargetLevel ] -> ylen-AreaHeight );

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

      delete_all_obstacles_in_area ( curShip . AllLevels [ TargetLevel ] , 0 , 0 , AreaWidth , AreaHeight );

      duplicate_all_obstacles_in_area ( curShip . AllLevels [ LevelNum ] , 0 , curShip . AllLevels [ LevelNum ] -> ylen - AreaHeight ,
					AreaWidth , AreaHeight ,
					curShip . AllLevels [ TargetLevel ] , 0 , 0 );

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

      delete_all_obstacles_in_area ( curShip . AllLevels [ TargetLevel ] , 0 , 0 , AreaWidth , AreaHeight );

      duplicate_all_obstacles_in_area ( curShip . AllLevels [ LevelNum ] , curShip . AllLevels [ LevelNum ] -> xlen - AreaWidth , 0 , 
					AreaWidth , AreaHeight ,
					curShip . AllLevels [ TargetLevel ] , 0 , 0 );

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

      delete_all_obstacles_in_area ( curShip . AllLevels [ TargetLevel ] , curShip . AllLevels [ TargetLevel ] -> xlen - AreaWidth , 0 , AreaWidth , AreaHeight );

      duplicate_all_obstacles_in_area ( curShip . AllLevels [ LevelNum ] , 0 , 0 , AreaWidth , AreaHeight ,
					curShip . AllLevels [ TargetLevel ] , curShip . AllLevels [ TargetLevel ] -> xlen - AreaWidth , 0 );

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
	  NewLevel->map[i][0] . floor_value = CORNER_LU;
	  NewLevel->map[i][ NewLevel->xlen -1 ]  . floor_value = CORNER_RU;
	}
      else if ( i == NewLevel->ylen -1 )
	{
	  NewLevel->map[i][0] . floor_value = CORNER_LD;
	  NewLevel->map[i][ NewLevel->xlen -1 ]  . floor_value = CORNER_RD;
	}
      else
	{
	  NewLevel->map[i][0] . floor_value = V_WALL;
	  NewLevel->map[i][ NewLevel->xlen -1 ]  . floor_value = V_WALL;
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
 * Now we print out the map label information about this map location.
 * ---------------------------------------------------------------------- */
void
PrintMapLabelInformationOfThisSquare ( Level EditLevel )
{
  int MapLabelIndex;
  char PanelText[5000]="";

  for ( MapLabelIndex = 0 ; MapLabelIndex < MAX_MAP_LABELS_PER_LEVEL ; MapLabelIndex ++ )
    {
      if ( ( fabsf ( Me [ 0 ] . pos . x - ( EditLevel -> labels [ MapLabelIndex ] . pos . x + 0.5 ) ) <= 0.5 ) && 
	   ( fabsf ( Me [ 0 ] . pos . y - ( EditLevel -> labels [ MapLabelIndex ] . pos . y + 0.5 ) ) <= 0.5 ) )
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
  Level EditLevel;
  static iso_image level_editor_cursor = { NULL , 0 , 0 } ;
  char* fpath;

  EditLevel = curShip.AllLevels [ Me [ 0 ] . pos . z ] ;
#define HIGHLIGHTCOLOR 255

  //--------------------
  // Maybe, if the level editor floor cursor has not yet been loaded,
  // we need to load it.
  //
  if ( level_editor_cursor . surface == NULL )
    {
      fpath = find_file ( "level_editor_floor_cursor.png" , GRAPHICS_DIR, FALSE );
      get_iso_image_from_file_and_path ( fpath , & ( level_editor_cursor ) ) ;
      if ( level_editor_cursor . surface == NULL )
	{
	  GiveStandardErrorMessage ( "Highlight_Current_Block (...)" , "\
Unable to load the level editor floor cursor.",
				     PLEASE_INFORM, IS_FATAL );
	}
    }

  blit_iso_image_to_map_position ( level_editor_cursor , Me [ 0 ] . pos . x , Me [ 0 ] . pos . y );

  PrintMapLabelInformationOfThisSquare ( EditLevel );

} // void Highlight_Current_Block(void)

/* ----------------------------------------------------------------------
 * This function is used to draw a line between given map tiles.  It is
 * mainly used for the map editor to highlight connections and the 
 * current map tile target.
 * ---------------------------------------------------------------------- */
void 
draw_connection_between_tiles ( float x1 , float y1 , float x2 , float y2 )
{
  float steps;
  float dist;
  int i;
  static iso_image level_editor_dot_cursor = { NULL , 0 , 0 };
  char* fpath;

  //--------------------
  // Maybe, if the level editor dot cursor has not yet been loaded,
  // we need to load it.
  //
  if ( level_editor_dot_cursor . surface == NULL )
    {
      fpath = find_file ( "level_editor_waypoint_dot.png" , GRAPHICS_DIR, FALSE );
      get_iso_image_from_file_and_path ( fpath , & ( level_editor_dot_cursor ) ) ;
      if ( level_editor_dot_cursor . surface == NULL )
	{
	  GiveStandardErrorMessage ( "draw_connection_between_tiles (...)" , "\
Unable to load the level editor waypoint dot cursor.",
				     PLEASE_INFORM, IS_FATAL );
	}
    }

  //--------------------
  // So now that the dot cursor has been loaded, we can start to
  // actually draw the dots.
  //

  //--------------------
  // We measure the distance that we have to go and then we draw some
  // dots at some convex combinations of our two vectors.  Very fine.
  //
  dist = sqrt ( ( x1 - x2 ) * ( x1 - x2 ) + ( y1 - y2 ) * ( y1 - y2 ) );

  steps = dist * 4;  // let's say 4 dots per square to mark the line, ok?

  for ( i = 0 ; i < steps+1 ; i ++ )
    {
      blit_iso_image_to_map_position ( level_editor_dot_cursor ,
				       ( ((float)i) / steps ) * x1 + x2 * ( steps - i )/steps , 
				       ( ((float)i) / steps ) * y1 + y2 * ( steps - i )/steps );
    }

}; // void draw_connection_between_tiles ( .... )

/* ----------------------------------------------------------------------
 * This function is used by the Level Editor integrated into 
 * freedroid.  It marks all waypoints with a cross.
 * ---------------------------------------------------------------------- */
void 
ShowWaypoints( int PrintConnectionList , int mask )
{
  int wp;
  int i;
  int BlockX, BlockY;
  char ConnectionText[5000];
  char TextAddition[1000];
  Level EditLevel;
  static iso_image level_editor_waypoint_cursor = { NULL , 0 , 0 } ;
  char* fpath;

  EditLevel = curShip.AllLevels [ Me [ 0 ] . pos . z ] ;

#define ACTIVE_WP_COLOR 0x0FFFFFFFF

  //--------------------
  // Maybe, if the level editor floor cursor has not yet been loaded,
  // we need to load it.
  //
  if ( level_editor_waypoint_cursor . surface == NULL )
    {
      fpath = find_file ( "level_editor_waypoint_cursor.png" , GRAPHICS_DIR, FALSE );
      get_iso_image_from_file_and_path ( fpath , & ( level_editor_waypoint_cursor ) ) ;
      if ( level_editor_waypoint_cursor . surface == NULL )
	{
	  GiveStandardErrorMessage ( "ShowWaypoints( int PrintConnectionList )" , "\
Unable to load the level editor waypoint cursor.",
				     PLEASE_INFORM, IS_FATAL );
	}
    }

  BlockX = rintf ( Me [ 0 ] . pos . x - 0.5 );
  BlockY = rintf ( Me [ 0 ] . pos . y - 0.5 );
	  
  for (wp=0; wp<MAXWAYPOINTS; wp++)
    {

      if ( EditLevel->AllWaypoints[wp].x == 0) continue;

      if ( mask && ZOOM_OUT )
	blit_zoomed_iso_image_to_map_position ( & ( level_editor_waypoint_cursor ) , 
						EditLevel -> AllWaypoints [ wp ] . x + 0.5 , 
						EditLevel -> AllWaypoints [ wp ] . y + 0.5 ) ;
      else
	blit_iso_image_to_map_position ( level_editor_waypoint_cursor , 
					 EditLevel -> AllWaypoints [ wp ] . x + 0.5 , 
					 EditLevel -> AllWaypoints [ wp ] . y + 0.5 ) ;
      
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

		  draw_connection_between_tiles ( EditLevel->AllWaypoints[wp].x + 0.5 , EditLevel->AllWaypoints[wp].y + 0.5 , 
						  EditLevel->AllWaypoints[EditLevel->AllWaypoints[wp].connections[i]].x + 0.5 , 
						  EditLevel->AllWaypoints[EditLevel->AllWaypoints[wp].connections[i]].y + 0.5 );

		}
	    }
	}
    }

}; // void ShowWaypoints( int PrintConnectionList );

/* ----------------------------------------------------------------------
 * This function is used by the Level Editor integrated into 
 * freedroid.  It marks all places that have a label attached to them.
 * ---------------------------------------------------------------------- */
void 
ShowMapLabels( int mask )
{
  int LabelNr;
  Level EditLevel;
  static iso_image map_label_indicator;
  static int first_function_call = TRUE ;
  char* fpath;
  EditLevel = curShip.AllLevels [ Me [ 0 ] . pos . z ] ;

  //--------------------
  // On the first function call to this function, we must load the map label indicator
  // iso image from the disk to memory and keep it there as static.  That should be
  // it for here.
  //
  if ( first_function_call )
    {
      first_function_call = FALSE;
      fpath = find_file ( "level_editor_map_label_indicator.png" , GRAPHICS_DIR, FALSE);
      get_iso_image_from_file_and_path ( fpath , & ( map_label_indicator ) );
    }
  
  //--------------------
  // Now we can draw a fine indicator at all the position nescessary...
  //
  for (LabelNr=0; LabelNr<MAXWAYPOINTS; LabelNr++)
    {
      if ( EditLevel->labels[LabelNr].pos.x == (-1) ) continue;

      if ( ! ( mask && ZOOM_OUT ) )
	{
	  blit_iso_image_to_map_position ( map_label_indicator , EditLevel -> labels [ LabelNr ] . pos . x + 0.5 , 
					   EditLevel -> labels [ LabelNr ] . pos . y + 0.5 );
	}
      else
	{
	  blit_zoomed_iso_image_to_map_position ( & ( map_label_indicator ) , EditLevel -> labels [ LabelNr ] . pos . x + 0.5 , 
						  EditLevel -> labels [ LabelNr ] . pos . y + 0.5 );
	}
    }

}; // void ShowMapLabels( void );

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
      EditLevel -> map [ BlockY ] [ BlockX ]  . floor_value = TELE_1 ;
    }
  
  //--------------------
  // Pressing the '1' to '5' keys will insert either classic 'block'
  // fixed map tiles or decructible 'box' map blocks.
  //
  if (Number1Pressed()) 
    {
      if ( Shift_Was_Pressed() )
	EditLevel->map[BlockY][BlockX] . floor_value =BOX_1;
      else
	EditLevel->map[BlockY][BlockX] . floor_value =BLOCK1;
    }
  if (Number2Pressed()) 
    {
      if ( Shift_Was_Pressed() )
	EditLevel->map[BlockY][BlockX] . floor_value =BOX_2;
      else
	EditLevel->map[BlockY][BlockX] . floor_value =BLOCK2;
    }
  if (Number3Pressed()) 
    {
      if ( Shift_Was_Pressed() )
	EditLevel->map[BlockY][BlockX] . floor_value =BOX_3;
      else
	EditLevel->map[BlockY][BlockX] . floor_value =BLOCK3;
    }
  if (Number4Pressed()) 
    {
      if ( Shift_Was_Pressed() )
	EditLevel->map[BlockY][BlockX] . floor_value =BOX_4;
      else
	EditLevel->map[BlockY][BlockX] . floor_value =BLOCK4;
    }
  if (Number5Pressed()) 
    {
      EditLevel->map[BlockY][BlockX] . floor_value =BLOCK5;
    }
  if (LPressed()) 
    {
      EditLevel->map[BlockY][BlockX] . floor_value =LIFT;
    }
  if (KP_PLUS_Pressed()) 
    {
      EditLevel->map[BlockY][BlockX] . floor_value =V_WALL;
      if ( Alt_Was_Pressed() ) EditLevel->map[BlockY][BlockX] . floor_value =CAVE_V_WALL;
    }
  if (KP0Pressed()) 
    {
      EditLevel->map[BlockY][BlockX] . floor_value =H_WALL;
      if ( Alt_Was_Pressed() ) EditLevel->map[BlockY][BlockX] . floor_value =CAVE_H_WALL;
    }
  if (KP1Pressed()) 
    {
      if ( Shift_Was_Pressed() ) EditLevel->map[BlockY][BlockX] . floor_value =AUTOGUN_L;
      else if ( CtrlWasPressed() ) EditLevel->map[BlockY][BlockX] . floor_value =ENHANCER_LD;
      else if ( Alt_Was_Pressed() ) EditLevel->map[BlockY][BlockX] . floor_value =CAVE_CORNER_LD;
      else EditLevel->map[BlockY][BlockX] . floor_value =CORNER_LD;
    }
  if (KP2Pressed()) 
    {
      if ( Shift_Was_Pressed() )
	EditLevel->map[BlockY][BlockX] . floor_value =CONSOLE_D;
      else if ( RightCtrlWasPressed() ) 
	EditLevel->map[BlockY][BlockX] . floor_value =FLOOR_CARPET_D;
      else if ( RightAltWasPressed() ) 
	EditLevel->map[BlockY][BlockX] . floor_value =FLOOR_CAVE_D;
      else EditLevel->map[BlockY][BlockX] . floor_value =T_D;
    }
  if (KP3Pressed()) 
    {
      if ( Shift_Was_Pressed() ) EditLevel->map[BlockY][BlockX] . floor_value =AUTOGUN_U;
      else if ( CtrlWasPressed() ) EditLevel->map[BlockY][BlockX] . floor_value =ENHANCER_RD;
      else if ( Alt_Was_Pressed() ) EditLevel->map[BlockY][BlockX] . floor_value =CAVE_CORNER_RD;
      else EditLevel->map[BlockY][BlockX] . floor_value =CORNER_RD;
    }
  if (KP4Pressed()) 
    {
      if ( Shift_Was_Pressed() )
	EditLevel->map[BlockY][BlockX] . floor_value =CONSOLE_L;
      else if ( LeftCtrlWasPressed() ) 
	EditLevel->map[BlockY][BlockX] . floor_value =FLOOR_CARPET_L;
      else if ( LeftAltWasPressed() ) 
	EditLevel->map[BlockY][BlockX] . floor_value =FLOOR_CAVE_R;
      else EditLevel->map[BlockY][BlockX] . floor_value =T_L;
    }
  if (KP5Pressed()) 
    {
      if ( Shift_Was_Pressed() )
	EditLevel->map[BlockY][BlockX] . floor_value =VOID;
      else if ( RightCtrlWasPressed() ) 
	EditLevel->map[BlockY][BlockX] . floor_value =FLOOR_CARPET;
      else if ( RightAltWasPressed() ) 
	EditLevel->map[BlockY][BlockX] . floor_value =CAVE_FLOOR;
      else EditLevel->map[BlockY][BlockX] . floor_value =KREUZ;
    }
  if (KP6Pressed()) 
    {
      if ( LeftShiftWasPressed() )
	EditLevel->map[BlockY][BlockX] . floor_value =CONSOLE_R;
      else if ( LeftCtrlWasPressed() ) 
	EditLevel->map[BlockY][BlockX] . floor_value =FLOOR_CARPET_R;
      else if ( RightAltWasPressed() ) 
	EditLevel->map[BlockY][BlockX] . floor_value =FLOOR_CAVE_L;
      else EditLevel->map[BlockY][BlockX] . floor_value =T_R;
    }
  if (KP7Pressed()) 
    {
      if ( Shift_Was_Pressed() ) EditLevel->map[BlockY][BlockX] . floor_value =AUTOGUN_D;
      else if ( CtrlWasPressed() ) EditLevel->map[BlockY][BlockX] . floor_value =ENHANCER_LU;
      else if ( Alt_Was_Pressed() ) EditLevel->map[BlockY][BlockX] . floor_value =CAVE_CORNER_LU;
      else EditLevel->map[BlockY][BlockX] . floor_value =CORNER_LU;
    }
  if ( KP8Pressed() ) 
    {
      if ( Shift_Was_Pressed() )
	EditLevel->map[BlockY][BlockX] . floor_value =CONSOLE_U;
      else if ( LeftCtrlWasPressed() ) 
	EditLevel->map[BlockY][BlockX] . floor_value =FLOOR_CARPET_U;
      else if ( RightAltWasPressed() ) 
	EditLevel->map[BlockY][BlockX] . floor_value =FLOOR_CAVE_U;
      else EditLevel->map[BlockY][BlockX] . floor_value =T_U;
    }
  if (KP9Pressed()) 
    {
      if ( Shift_Was_Pressed() ) EditLevel->map[BlockY][BlockX] . floor_value =AUTOGUN_R;
      else if ( CtrlWasPressed() ) EditLevel->map[BlockY][BlockX] . floor_value =ENHANCER_RU;
      else if ( Alt_Was_Pressed() ) EditLevel->map[BlockY][BlockX] . floor_value =CAVE_CORNER_RU;
      else EditLevel->map[BlockY][BlockX] . floor_value =CORNER_RU;
    }
  if (APressed())
    {
      EditLevel->map[BlockY][BlockX] . floor_value =ALERT;	      
    }
  if (RPressed())
    {
      if ( Shift_Was_Pressed() ) EditLevel->map[BlockY][BlockX]  . floor_value = CONSUMER_1;
      else EditLevel->map[BlockY][BlockX]  . floor_value = REFRESH1;	            
    }
  if (DPressed())
    {
      if ( !CtrlWasPressed())
	{
	  if (Shift_Was_Pressed())
	    create_new_obstacle_on_level ( EditLevel , ISO_V_DOOR_000_OPEN , ((int)Me[0].pos.x) , ((int)Me[0].pos.y) + 0.5 );
	  else 	    
	    create_new_obstacle_on_level ( EditLevel , ISO_H_DOOR_000_OPEN , ((int)Me[0].pos.x) + 0.5 , ((int)Me[0].pos.y) );
	}
      else
	{
	  if (Shift_Was_Pressed())
	    create_new_obstacle_on_level ( EditLevel , ISO_V_DOOR_LOCKED , ((int)Me[0].pos.x) , ((int)Me[0].pos.y) + 0.5 );
	  else 
	    create_new_obstacle_on_level ( EditLevel , ISO_H_DOOR_LOCKED , ((int)Me[0].pos.x) + 0.5 , ((int)Me[0].pos.y) );
	}
      while ( DPressed() );
    }
  if (UPressed())
    {
      if ( !CtrlWasPressed())
	{
	  if (Shift_Was_Pressed())
	    EditLevel->map[BlockY][BlockX] . floor_value =CHEST_U;	            	      
	  else EditLevel->map[BlockY][BlockX] . floor_value =CHEST_D;	            	      
	}
      else
	{
	  if (Shift_Was_Pressed())
	    EditLevel->map[BlockY][BlockX] . floor_value =CHEST_L;	            	      
	  else EditLevel->map[BlockY][BlockX] . floor_value =CHEST_R;	            	      
	}
    }
  if (SpacePressed() && !axis_is_active )
    {
      if ( Shift_Was_Pressed() )
	EditLevel->map[BlockY][BlockX] . floor_value =FINE_GRID;	            	      	    
      else
	EditLevel->map[BlockY][BlockX] . floor_value =FLOOR;	            	      	    
    }
  
}; // void HandleMapTileEditingKeys ( Level EditLevel , int BlockX , int BlockY )

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
      if ( ( fabsf ( EditLevel -> labels [ i ] . pos . x + 0.5 - Me[0].pos.x ) < 0.5 ) &&
	   ( fabsf ( EditLevel -> labels [ i ] . pos . y + 0.5 - Me[0].pos.y ) ) ) 
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
      if ( i >= MAX_MAP_LABELS_PER_LEVEL )
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
      EditLevel -> labels [ i ] . pos . x = rintf( Me[0].pos.x - 0.5 );
      EditLevel -> labels [ i ] . pos . y = rintf( Me[0].pos.y - 0.5 );
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
  int SourceAreaTileType = EditLevel->map[BlockY][BlockX] . floor_value ;

  EditLevel->map[BlockY][BlockX]  . floor_value = SpecialMapValue ;

  if ( BlockX > 0 )
    {
      if ( EditLevel->map[BlockY][BlockX-1]  . floor_value == SourceAreaTileType )
	RecFillMap ( EditLevel , BlockY , BlockX -1 , SpecialMapValue );
    }
  if ( BlockX < EditLevel->xlen -1 )
    {
      if ( EditLevel->map[BlockY][BlockX+1]  . floor_value == SourceAreaTileType )
	RecFillMap ( EditLevel , BlockY , BlockX +1 , SpecialMapValue );
    }
  if ( BlockY > 0 )
    {
      if ( EditLevel->map[BlockY-1][BlockX]  . floor_value == SourceAreaTileType )
	RecFillMap ( EditLevel , BlockY-1 , BlockX , SpecialMapValue );
    }
  if ( BlockY < EditLevel->ylen -1 )
    {
      if ( EditLevel->map[BlockY+1][BlockX]  . floor_value == SourceAreaTileType )
	RecFillMap ( EditLevel , BlockY+1 , BlockX , SpecialMapValue );
    }
};

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
show_button_tooltip ( char* tooltip_text )
{
  SDL_Rect TargetRect;

  Activate_Conservative_Frame_Computation();

  TargetRect . w = 400 ; 
  TargetRect . h = 220 ; 
  TargetRect . x = ( 640 - TargetRect . w ) / 2 ; 
  TargetRect . y = 2 * ( 480 - TargetRect . h ) / 3 ; 
  SDL_FillRect ( Screen , &TargetRect , 
		 SDL_MapRGB ( Screen->format, 0 , 0 , 0 ) ) ;
  
#define IN_WINDOW_TEXT_OFFSET 15
  TargetRect . w -= IN_WINDOW_TEXT_OFFSET;
  TargetRect . h -= IN_WINDOW_TEXT_OFFSET;
  TargetRect . x += IN_WINDOW_TEXT_OFFSET;
  TargetRect . y += IN_WINDOW_TEXT_OFFSET;

  SetCurrentFont ( FPS_Display_BFont );

  DisplayText ( tooltip_text, TargetRect . x, TargetRect . y , &TargetRect )  ;

}; // void show_button_tooltip ( char* tooltip_text )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
show_level_editor_tooltips ( void )
{
  static float time_spent_on_some_button = 0 ;

  static float previous_function_call_time = 0 ;

  time_spent_on_some_button += SDL_GetTicks() - previous_function_call_time ; 

  previous_function_call_time = SDL_GetTicks();

#define TICKS_UNTIL_TOOLTIP 1200

  if ( CursorIsOnButton ( GO_LEVEL_NORTH_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) )
    {
      if ( time_spent_on_some_button > TICKS_UNTIL_TOOLTIP )
	show_button_tooltip ( "Use this button to move one level north, i.e. to the level that is glued to the northern side of this level." );
    }
  else if ( CursorIsOnButton ( GO_LEVEL_SOUTH_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) )
    {
      if ( time_spent_on_some_button > TICKS_UNTIL_TOOLTIP )
	show_button_tooltip ( "Use this button to move one level south, i.e. to the level that is glued to the southern side of this level." );
    }
  else if ( CursorIsOnButton ( GO_LEVEL_EAST_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) )
    {
      if ( time_spent_on_some_button > TICKS_UNTIL_TOOLTIP )
	show_button_tooltip ( "Use this button to move one level east, i.e. to the level that is glued to the eastern side of this level." );
    }
  else if ( CursorIsOnButton ( GO_LEVEL_WEST_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) )
    {
      if ( time_spent_on_some_button > TICKS_UNTIL_TOOLTIP )
	show_button_tooltip ( "Use this button to move one level west, i.e. to the level that is glued to the western side of this level." );
    }
  else if ( CursorIsOnButton ( EXPORT_THIS_LEVEL_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) )
    {
      if ( time_spent_on_some_button > TICKS_UNTIL_TOOLTIP )
	show_button_tooltip ( "In FreedroidRPG maps can be glued together to form one big map.  But that requires that the maps are identical where they overlap.  This button will copy the borders of this level to the borders of the neighbouring levels, so that the maps are in sync again." );
    }
  else if ( CursorIsOnButton ( LEVEL_EDITOR_SAVE_SHIP_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) )
    {
      if ( time_spent_on_some_button > TICKS_UNTIL_TOOLTIP )
	show_button_tooltip ( "This button will save your current ship to the file 'Testship.shp' in your current working directory.  If you are sure that you want this, you can copy it over the regular file 'Asteroid.maps' in maps subdirectory to make your map the default FreedroidRPG map." );
    }
  else if ( CursorIsOnButton ( LEVEL_EDITOR_ZOOM_IN_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) )
    {
      if ( time_spent_on_some_button > TICKS_UNTIL_TOOLTIP )
	show_button_tooltip ( "\nUse this button to zoom INTO the level.\n\nYou can also use the hotkey 'I' for this." );
    }
  else if ( CursorIsOnButton ( LEVEL_EDITOR_ZOOM_OUT_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) )
    {
      if ( time_spent_on_some_button > TICKS_UNTIL_TOOLTIP )
	show_button_tooltip ( "\nUse this button to zoom OUT of the level.\n\nYou can also use the hotkey 'O' for this." );
    }
  else if ( CursorIsOnButton ( LEVEL_EDITOR_RECURSIVE_FILL_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) )
    {
      if ( time_spent_on_some_button > TICKS_UNTIL_TOOLTIP )
	show_button_tooltip ( "Use this button to fill a certain area of the map with the currently selected map tile.  Filling will proceed from the cursor in all direction until a change of map tile is encountered." );
    }
  else if ( CursorIsOnButton ( LEVEL_EDITOR_NEW_MAP_LABEL_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) )
    {
      if ( time_spent_on_some_button > TICKS_UNTIL_TOOLTIP )
	show_button_tooltip ( "Use this button to attach a new map label to the current cursor position.  These map labels can be used to define starting points for bots and characters or also to define locations for events and triggers." );
    }
  else if ( CursorIsOnButton ( LEVEL_EDITOR_NEW_ITEM_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) )
    {
      if ( time_spent_on_some_button > TICKS_UNTIL_TOOLTIP )
	show_button_tooltip ( "Use this button to drop a new item to the floor.  You can also use the hotkey 'G' for this." );
    }
  else if ( CursorIsOnButton ( LEVEL_EDITOR_LEVEL_RESIZE_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) )
    {
      if ( time_spent_on_some_button > TICKS_UNTIL_TOOLTIP )
	show_button_tooltip ( "Use this button to enter the level resize menu.  Levels can be resized in various ways so as not to destroy your current map too much and so as to insert the new space where you would best like it to be." );
    }
  else if ( CursorIsOnButton ( LEVEL_EDITOR_KEYMAP_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) )
    {
      if ( time_spent_on_some_button > TICKS_UNTIL_TOOLTIP )
	show_button_tooltip ( "Use this button to enter the level editor keymap display." );
    }
  else if ( CursorIsOnButton ( LEVEL_EDITOR_QUIT_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) )
    {
      if ( time_spent_on_some_button > TICKS_UNTIL_TOOLTIP )
	show_button_tooltip ( "Use this button to quit out of the level editor and back to continue the normal game in normal mode.  Useful for e.g. putting objects into boxes.  You can always re-enter the level editor." );
    }
  else if ( CursorIsOnButton ( LEVEL_EDITOR_TOGGLE_ENEMIES_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) )
    {
      if ( time_spent_on_some_button > TICKS_UNTIL_TOOLTIP )
	show_button_tooltip ( "Use this button to toggle between enemies dispalyed in level editor or enemies hidden in level editor." );
    }
  else if ( CursorIsOnButton ( LEVEL_EDITOR_TOGGLE_OBSTACLES_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) )
    {
      if ( time_spent_on_some_button > TICKS_UNTIL_TOOLTIP )
	show_button_tooltip ( "Use this button to toggle between obstacles dispalyed in level editor or obstacles hidden in level editor." );
    }
  else if ( CursorIsOnButton ( LEVEL_EDITOR_TOGGLE_TUX_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) )
    {
      if ( time_spent_on_some_button > TICKS_UNTIL_TOOLTIP )
	show_button_tooltip ( "Use this button to toggle between Tux dispalyed in level editor or Tux hidden in level editor." );
    }
  else
    {
      time_spent_on_some_button = 0 ;
    }

}; // void show_level_editor_tooltips ( void )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
add_obstacle ( Level EditLevel , float x , float y , int new_obstacle_type )
{
  int i;

  for ( i = 0 ; i < MAX_OBSTACLES_ON_MAP ; i ++ )
    {
      if ( EditLevel -> obstacle_list [ i ] . type == (-1) )
	{
	  EditLevel -> obstacle_list [ i ] . type = new_obstacle_type ;
	  EditLevel -> obstacle_list [ i ] . pos . x = x ;
	  EditLevel -> obstacle_list [ i ] . pos . y = y ;
	  glue_obstacles_to_floor_tiles_for_level ( EditLevel -> levelnum );
	  DebugPrintf ( 0 , "\nNew obstacle has been added!!!" );
	  fflush(stdout);
	  return;
	}
    }

}; // void add_obstacle ( Level EditLevel , float x , float y , int Highlight )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
delete_obstacle ( level* EditLevel , obstacle* our_obstacle )
{
  int i;
  int obstacle_index = (-1) ;

  //--------------------
  // The likely case that no obstacle was currently marked.
  //
  if ( our_obstacle == NULL ) return;

  //--------------------
  // We need to find out the index of the obstacle in question,
  // so that we can find out and eliminate any glue for this 
  // obstacle.
  //
  for ( i = 0 ; i < MAX_OBSTACLES_ON_MAP ; i ++ )
    {
      if ( our_obstacle == & ( EditLevel -> obstacle_list [ i ] ) )
	{
	  obstacle_index = i ;
	  break;
	}
    }

  //--------------------
  // Maybe there is a severe bug somewhere in FreedroidRPG.  We catch
  // this case as well...
  //
  if ( obstacle_index == (-1) )
    {
      GiveStandardErrorMessage ( "delete_obstacle (...)" , "\
Unable to find the obstacle in question within the obstacle list!",
				 PLEASE_INFORM , IS_FATAL );
    }

  /*
  //--------------------
  // Now we can eliminate all the glue...
  //
  for ( xtile = 0 ; xtile < EditLevel -> xlen ; xtile ++ )
    {
      for ( ytile = 0 ; ytile < EditLevel -> ylen ; ytile ++ )
	{
	  for ( i = 0 ; i < MAX_OBSTACLES_GLUED_TO_ONE_MAP_TILE ; i ++ )
	    if ( EditLevel -> map [ ytile ] [ xtile ] . obstacles_glued_to_here [ i ] == 
		 obstacle_index )
	      {
		EditLevel -> map [ ytile ] [ xtile ] . obstacles_glued_to_here [ i ] = (-1) ;
		//--------------------
		// Maybe there are other obstacles glued to here.  Then we need to fill the
		// gap in the glue array that we have just created.
		//
		for ( j = i ; j < MAX_OBSTACLES_GLUED_TO_ONE_MAP_TILE - 1 ; j ++ )
		  {
		    if ( EditLevel -> map [ ytile ] [ xtile ] . obstacles_glued_to_here [ j + 1 ] != (-1) )
		      {
			EditLevel -> map [ ytile ] [ xtile ] . obstacles_glued_to_here [ j ] = 
			  EditLevel -> map [ ytile ] [ xtile ] . obstacles_glued_to_here [ j + 1 ] ;
			EditLevel -> map [ ytile ] [ xtile ] . obstacles_glued_to_here [ j + 1 ] = (-1) ;
		      }
		  }
		
	      }
	}
    }
  */

  //--------------------
  // And of course we must not forget to delete the obstalce itself
  // as well, not only the glue...
  //
  our_obstacle -> type = ( -1 ) ;

  //--------------------
  // Maybe filling out the gap isn't so desireable after all.  Might that cause
  // problems with keeping track of the indices when obstacles are named?  Should
  // we do away with this?  But then we also need to go over -1 entried in the
  // loops coursing throught he whole list in other places...  So it will stay for
  // now I guess...
  //
  memmove ( & ( EditLevel -> obstacle_list [ obstacle_index ] ) , 
	    & ( EditLevel -> obstacle_list [ obstacle_index + 1 ] ) ,
	    ( MAX_OBSTACLES_ON_MAP - obstacle_index - 2 ) * sizeof ( obstacle ) );

  //--------------------
  // Now doing that must have shifted the glue!  That is a problem.  We need to
  // reglue everything to the map...
  //
  glue_obstacles_to_floor_tiles_for_level ( EditLevel -> levelnum );

}; // void delete_obstacle ( obstacle* our_obstacle )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
int
marked_obstacle_is_glued_to_here ( Level EditLevel , float x , float y )
{
  int j;
  int current_mark_index = (-1);

  if ( level_editor_marked_obstacle == NULL ) return ( FALSE );
  
  for ( j = 0 ; j < MAX_OBSTACLES_GLUED_TO_ONE_MAP_TILE ; j ++ )
    {
      if ( level_editor_marked_obstacle == & ( EditLevel -> obstacle_list [ EditLevel -> map [ (int)y ] [ (int)x ] . obstacles_glued_to_here [ j ] ] ) )
	current_mark_index = j ;
    }
  
  if ( current_mark_index != (-1) ) return ( TRUE );
  return ( FALSE );

}; // int marked_obstacle_is_glued_to_here ( Me [ 0 ] . pos . x , Me [ 0 ] . pos . y )

/* ----------------------------------------------------------------------
 * This function should assign a new name to a given obstacle on a given
 * level.  New indices must be found and the user must be queried for his
 * input about the desired new obstacle name.
 * ---------------------------------------------------------------------- */
void
give_new_name_to_obstacle ( Level EditLevel , obstacle* our_obstacle )
{
  int i;
  int free_index=(-1);

  //--------------------
  // If the obstacle already has a name, we can use that index for the 
  // new name now.
  //
  if ( our_obstacle -> name_index >= 0 )
    free_index = our_obstacle -> name_index ;
  else
    {
      //--------------------
      // Else we must find a free index in the list of obstacle names for this level
      //
      for ( i = 0 ; i < MAX_OBSTACLE_NAMES_PER_LEVEL ; i ++ )
	{
	  if ( EditLevel -> obstacle_name_list [ i ] == NULL )
	    {
	      free_index = i ;
	      break;
	    }
	}
      if ( free_index < 0 ) return;
    }

  //--------------------
  // We must query the user for the desired new name
  //
  if ( EditLevel -> obstacle_name_list [ free_index ] == NULL )
    EditLevel -> obstacle_name_list [ free_index ] = "" ;
  EditLevel -> obstacle_name_list [ free_index ] = 
    GetEditableStringInPopupWindow ( 1000 , "\nPlease enter name for this obstacle: \n\n" ,
				     EditLevel -> obstacle_name_list [ free_index ] );

  //--------------------
  // We must select the right index as the name of this obstacle.
  //
  our_obstacle -> name_index = free_index ;

  //--------------------
  // But if the given name was empty, then we remove everything again.
  //
  if ( strlen ( EditLevel -> obstacle_name_list [ free_index ] ) == 0 )
    {
      EditLevel -> obstacle_name_list [ free_index ] = NULL ;
      our_obstacle -> name_index = (-1);
    }

}; // void give_new_name_to_obstacle ( EditLevel , level_editor_marked_obstacle )
     
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
  int BlockX = rintf( Me [ 0 ] . pos . x + 0.5 );
  int BlockY = rintf( Me [ 0 ] . pos . y + 0.5 );
  int Done = FALSE;
  int Weiter = FALSE;
  int i ;
  char linebuf[10000];
  long OldTicks;
  Level EditLevel;
  char* NewCommentOnThisSquare;
  int LeftMousePressedPreviousFrame = FALSE;
  int RightMousePressedPreviousFrame = FALSE;
  moderately_finepoint TargetSquare;
  int new_x, new_y;
  int current_mark_index ;
  int j;

  //--------------------
  // We set the Tux position to something 'round'.
  //
  Me [ 0 ] . pos . x = rintf ( Me [ 0 ] . pos . x ) + 0.5 ;
  Me [ 0 ] . pos . y = rintf ( Me [ 0 ] . pos . y ) + 0.5 ;

  //--------------------
  // We disable all the 'screens' so that we have full view on the
  // map for the purpose of level editing.
  //
  GameConfig.Inventory_Visible = FALSE;
  GameConfig.CharacterScreen_Visible = FALSE;
  GameConfig.SkillScreen_Visible = FALSE;
  RespectVisibilityOnMap = FALSE ;

  //--------------------
  // We init the 'vanishing message' structs, so that there is always
  // something to display, and we set the time to 'out of date' already.
  //
  EditLevel = curShip.AllLevels [ Me [ 0 ] . pos . z ] ;
  strcpy ( VanishingMessage , "Hello" );
  VanishingMessageDisplayTime = 0 ;

  //--------------------
  // For drawing new waypoints, we init this.
  //
  OriginWaypoint = (-1);

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

	  BlockX = rintf ( Me [ 0 ] . pos . x - 0.5 );
	  BlockY = rintf ( Me [ 0 ] . pos . y - 0.5 );
	  
	  EditLevel = curShip.AllLevels [ Me [ 0 ] . pos . z ] ;	  
	  GetAllAnimatedMapTiles ( EditLevel );

	  //--------------------
	  // If the cursor is close to the currently marked obstacle, we leave everything as it
	  // is.  (There might be some human choice made here already.)
	  // Otherwise we just select the next best obstacle as the new marked obstacle.
	  //
	  if ( level_editor_marked_obstacle != NULL ) 
	    {
	      // if ( ( fabsf ( level_editor_marked_obstacle -> pos . x - Me [ 0 ] . pos . x ) >= 0.98 ) ||
	      // ( fabsf ( level_editor_marked_obstacle -> pos . y - Me [ 0 ] . pos . y ) >= 0.98 ) )
	      //level_editor_marked_obstacle = NULL ;
	      if ( ! marked_obstacle_is_glued_to_here ( EditLevel , Me [ 0 ] . pos . x , Me [ 0 ] . pos . y ) )
		level_editor_marked_obstacle = NULL ;
	    }
	  else
	    {
	      if ( EditLevel -> map [ BlockY ] [ BlockX ] . obstacles_glued_to_here [ 0 ] != (-1) )
		{
		  level_editor_marked_obstacle = & ( EditLevel -> obstacle_list [ EditLevel -> map [ BlockY ] [ BlockX ] . obstacles_glued_to_here [ 0 ] ] ) ;
		  DebugPrintf ( 0 , "\nObstacle marked now!" );
		}
	      else
		{
		  level_editor_marked_obstacle = NULL ;
		  DebugPrintf ( 0 , "\nNo obstacle marked now!" );
		}
	    }

	  VanishingMessageDisplayTime += ( SDL_GetTicks ( ) - OldTicks ) / 1000.0 ;
	  OldTicks = SDL_GetTicks ( ) ;

	  ClearUserFenster();
	  AssembleCombatPicture ( ONLY_SHOW_MAP_AND_TEXT | SHOW_GRID | SHOW_ITEMS | GameConfig.omit_tux_in_level_editor * OMIT_TUX | GameConfig.omit_obstacles_in_level_editor * OMIT_OBSTACLES | GameConfig.omit_enemies_in_level_editor * OMIT_ENEMIES | SHOW_OBSTACLE_NAMES | ZOOM_OUT * GameConfig . zoom_is_on | OMIT_BLASTS );

	  Highlight_Current_Block();

	  ShowWaypoints( FALSE , ZOOM_OUT * GameConfig . zoom_is_on );
	  ShowMapLabels( ZOOM_OUT * GameConfig . zoom_is_on );
	  
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
	  ShowGenericButtonFromList ( LEVEL_EDITOR_NEW_ITEM_BUTTON );
	  ShowGenericButtonFromList ( LEVEL_EDITOR_LEVEL_RESIZE_BUTTON );
	  ShowGenericButtonFromList ( LEVEL_EDITOR_KEYMAP_BUTTON );
	  ShowGenericButtonFromList ( LEVEL_EDITOR_QUIT_BUTTON );

	  ShowGenericButtonFromList ( LEVEL_EDITOR_TOGGLE_TUX_BUTTON );
	  ShowGenericButtonFromList ( LEVEL_EDITOR_TOGGLE_ENEMIES_BUTTON );
	  ShowGenericButtonFromList ( LEVEL_EDITOR_TOGGLE_OBSTACLES_BUTTON );

	  show_level_editor_tooltips (  );

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
	  /*
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
		  EditLevel->map[BlockY][BlockX] . floor_value =SpecialMapValue;
		}
	    }
	  */

	  //--------------------
	  // From the level editor, it should also be possible to drop new goods
	  // at some location via the 'G' key. (G like in Goods.)
	  //
	  if ( GPressed () )
	    {
	      ItemDropFromLevelEditor(  );
	    }

	  //--------------------
	  // The FKEY can be used to toggle between 'floor' and 'obstacle' edit modes
	  //
	  if ( FPressed () )
	    {
	      if ( GameConfig . level_editor_edit_mode == LEVEL_EDITOR_EDIT_FLOOR )
		GameConfig . level_editor_edit_mode = LEVEL_EDITOR_EDIT_OBSTACLES ;
	      else if ( GameConfig . level_editor_edit_mode == LEVEL_EDITOR_EDIT_OBSTACLES )
		GameConfig . level_editor_edit_mode = LEVEL_EDITOR_EDIT_FLOOR ;
	      while ( FPressed() );
	      Highlight = 0 ;
	    }

	  //--------------------
	  // If the person using the level editor decides he/she wants a different
	  // scale for the editing process, he/she may say so by using the O/I keys.
	  //
	  if ( OPressed () ) 
	    {
	      GameConfig . zoom_is_on = !GameConfig . zoom_is_on ;
	      while ( OPressed() );
	      // ZoomOut();
	    }
	  if ( IPressed () ) 
	    {
	      // ZoomIn();
	    }

	  if ( XPressed () )
	    {
	      delete_obstacle ( EditLevel , level_editor_marked_obstacle );
	      level_editor_marked_obstacle = NULL ;
	      while ( XPressed() );
	    }

	  //--------------------
	  // The HKEY can be used to give a name to the currently marked obstacle
	  //
	  if ( HPressed() )
	    {
	      if ( level_editor_marked_obstacle != NULL )
		{
		  give_new_name_to_obstacle ( EditLevel , level_editor_marked_obstacle );
		  while ( HPressed() );
		}
	    }

	  if ( NPressed() )
	    {
	      if ( level_editor_marked_obstacle != NULL )
		{
		  //--------------------
		  // See if this floor tile has some other obstacles glued to it as well
		  //
		  if ( EditLevel -> map [ BlockY ] [ BlockX ] . obstacles_glued_to_here [ 1 ] != (-1) )
		    {
		      //--------------------
		      // Find out which one of these is currently marked
		      //
		      current_mark_index = (-1);
		      for ( j = 0 ; j < MAX_OBSTACLES_GLUED_TO_ONE_MAP_TILE ; j ++ )
			{
			  if ( level_editor_marked_obstacle == & ( EditLevel -> obstacle_list [ EditLevel -> map [ BlockY ] [ BlockX ] . obstacles_glued_to_here [ j ] ] ) )
			    current_mark_index = j ;
			}
		      		      
		      if ( current_mark_index != (-1) ) 
			{
			  if ( EditLevel -> map [ BlockY ] [ BlockX ] . obstacles_glued_to_here [ current_mark_index + 1 ] != (-1) )
			    level_editor_marked_obstacle = & ( EditLevel -> obstacle_list [ EditLevel -> map [ BlockY ] [ BlockX ] . obstacles_glued_to_here [ current_mark_index + 1 ] ] ) ;
			  else
			    level_editor_marked_obstacle = & ( EditLevel -> obstacle_list [ EditLevel -> map [ BlockY ] [ BlockX ] . obstacles_glued_to_here [ 0 ] ] ) ;
			}

		    }
		}
	      while ( NPressed() );
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

	  //----------------------------------------------------------------------
	  // If the person using the level editor pressed some editing keys, insert the
	  // corresponding map tile.  This is done in the following:
	  //
	  HandleMapTileEditingKeys ( EditLevel , BlockX , BlockY );

	  //--------------------
	  // First we find out which map square the player MIGHT wish us to operate on
	  // via a POTENTIAL mouse click
	  //
	  if ( GameConfig . zoom_is_on )
	    {
	      TargetSquare . x = translate_pixel_to_zoomed_map_location ( 0 , (float) GetMousePos_x ( ) + 16.0 - ( SCREEN_WIDTH / 2 ) , 
								   (float) GetMousePos_y ( ) + 16.0 - ( SCREEN_HEIGHT / 2 ), TRUE );
	      TargetSquare . y = translate_pixel_to_zoomed_map_location ( 0 , (float) GetMousePos_x ( ) + 16.0 - ( SCREEN_WIDTH / 2 ), 
								   (float) GetMousePos_y ( ) + 16.0 - ( SCREEN_HEIGHT / 2 ), FALSE );
	    }
	  else
	    {
	      TargetSquare . x = translate_pixel_to_map_location ( 0 , (float) GetMousePos_x ( ) + 16.0 - ( SCREEN_WIDTH / 2 ) , 
								   (float) GetMousePos_y ( ) + 16.0 - ( SCREEN_HEIGHT / 2 ), TRUE );
	      TargetSquare . y = translate_pixel_to_map_location ( 0 , (float) GetMousePos_x ( ) + 16.0 - ( SCREEN_WIDTH / 2 ), 
								   (float) GetMousePos_y ( ) + 16.0 - ( SCREEN_HEIGHT / 2 ), FALSE );
	    }

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
		  GameConfig . zoom_is_on = FALSE ;
		  // ZoomIn();
		}
	      else if ( CursorIsOnButton ( LEVEL_EDITOR_ZOOM_OUT_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) )
		{
		  GameConfig . zoom_is_on = TRUE ;
		  // ZoomOut();
		}
	      else if ( CursorIsOnButton ( LEVEL_EDITOR_RECURSIVE_FILL_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) )
		{
		  RecFillMap ( EditLevel , BlockY , BlockX , Highlight );
		}
	      else if ( CursorIsOnButton ( LEVEL_EDITOR_NEW_MAP_LABEL_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) )
		{
		  EditMapLabelData ( EditLevel );
		}
	      else if ( CursorIsOnButton ( LEVEL_EDITOR_NEW_ITEM_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) )
		{
		  ItemDropFromLevelEditor(  );
		}
	      else if ( CursorIsOnButton ( LEVEL_EDITOR_LEVEL_RESIZE_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) )
		{
		  EditLevelDimensions (  );
		}
	      else if ( CursorIsOnButton ( LEVEL_EDITOR_KEYMAP_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) )
		{
		  ShowLevelEditorKeymap (  );
		}
	      else if ( CursorIsOnButton ( LEVEL_EDITOR_TOGGLE_TUX_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) )
		{
		  GameConfig . omit_tux_in_level_editor = ! GameConfig . omit_tux_in_level_editor ;
		}
	      else if ( CursorIsOnButton ( LEVEL_EDITOR_TOGGLE_ENEMIES_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) )
		{
		  GameConfig . omit_enemies_in_level_editor = ! GameConfig . omit_enemies_in_level_editor ;
		}
	      else if ( CursorIsOnButton ( LEVEL_EDITOR_TOGGLE_OBSTACLES_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) )
		{
		  GameConfig . omit_obstacles_in_level_editor = ! GameConfig . omit_obstacles_in_level_editor ;
		}
	      else if ( CursorIsOnButton ( LEVEL_EDITOR_QUIT_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) )
		{
		  Weiter=!Weiter;
		  Done=TRUE;
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
		  Me [ 0 ] . pos . x = 
		    translate_pixel_to_map_location ( 0 , (float) GetMousePos_x ( ) + 16.0 - ( SCREEN_WIDTH / 2 ) , 
						      (float) GetMousePos_y ( ) + 16.0 - ( SCREEN_HEIGHT / 2 ), TRUE ); 
		  if ( Me [ 0 ] . pos . x >= curShip.AllLevels[Me[0].pos.z]->xlen-1 )
		    Me [ 0 ] . pos . x = curShip.AllLevels[Me[0].pos.z]->xlen-1 ;
		  if ( Me [ 0 ] . pos . x <= 0 ) Me [ 0 ] . pos . x = 0;
		  Me [ 0 ] . pos . y = 
		    translate_pixel_to_map_location ( 0 , (float) GetMousePos_x ( ) + 16.0 - ( SCREEN_WIDTH / 2 ), 
						      (float) GetMousePos_y ( ) + 16.0 - ( SCREEN_HEIGHT / 2 ), FALSE );
		  if ( Me [ 0 ] . pos . y >= curShip.AllLevels[Me[0].pos.z]->ylen-1 )
		    Me [ 0 ] . pos . y = curShip.AllLevels[Me[0].pos.z]->ylen-1 ;
		  if ( Me [ 0 ] . pos . y <= 0 ) Me [ 0 ] . pos . y = 0;
		}
	    }

	  //--------------------
	  // With the right mouse button, it should be possible to actually 'draw'
	  // something into the level.  This seems to work so far.  Caution is nescessary
	  // to prevent segfault due to writing outside the level, but that's easily
	  // accomplished.
	  //
	  if ( MouseRightPressed() && !RightMousePressedPreviousFrame )
	    {
	      if ( ( (int)TargetSquare . x >= 0 ) &&
		   ( (int)TargetSquare . x <= EditLevel->xlen-1 ) &&
		   ( (int)TargetSquare . y >= 0 ) &&
		   ( (int)TargetSquare . y <= EditLevel->ylen-1 ) )
		{
		  if ( GameConfig . level_editor_edit_mode == LEVEL_EDITOR_EDIT_FLOOR )
		    EditLevel -> map [ (int)TargetSquare . y ] [ (int)TargetSquare . x ] . floor_value = Highlight ;
		  else if ( GameConfig . level_editor_edit_mode == LEVEL_EDITOR_EDIT_OBSTACLES )
		    {
		      add_obstacle ( EditLevel , TargetSquare . x , TargetSquare . y , Highlight );
		    }
		}
	    }

	  if (QPressed())
	    {
	      Terminate(0);
	    }

	  LeftMousePressedPreviousFrame = axis_is_active; 
	  RightMousePressedPreviousFrame = MouseRightPressed() ;

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
