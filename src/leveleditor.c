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

void Show_Waypoints( int PrintConnectionList );
void LevelEditor(void);
Level CreateNewMapLevel( void );
void SetLevelInterfaces ( void );

EXTERN char Previous_Mission_Name[1000];

char VanishingMessage[10000]="Hello";
float VanishingMessageDisplayTime = 0;
int OriginWaypoint = (-1);

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
 * Self-explanatory.
 * ---------------------------------------------------------------------- */
void
InsertLineVerySouth ( Level CurLevel )
{

  //--------------------
  // The enlargement of levels in y direction is limited by a constant
  // defined in defs.h.  This is carefully checked or no operation at
  // all will be performed.
  //
  if ( (CurLevel->ylen)+1 < MAX_MAP_LINES )
    {
      CurLevel->ylen++;
      // In case of enlargement, we need to do more:
      CurLevel->map[ CurLevel->ylen-1 ] = MyMalloc( CurLevel->xlen +1) ;
      // We don't want to fill the new area with junk, do we? So we make it floor tiles
      memset( CurLevel->map[ CurLevel->ylen-1 ] , FLOOR , CurLevel->xlen );
    }

}; // void InsertLineVerySouth ( Level CurLevel )

/* ----------------------------------------------------------------------
 * Self-explanatory.
 * ---------------------------------------------------------------------- */
void
InsertColumnVeryEast ( Level CurLevel )
{
  int i;
  char* OldMapPointer;

  CurLevel->xlen++;
  // In case of enlargement, we need to do more:
  for ( i = 0 ; i < CurLevel->ylen ; i++ )
    {
      OldMapPointer=CurLevel->map[i];
      CurLevel->map[i] = MyMalloc( CurLevel->xlen +1) ;
      memcpy( CurLevel->map[i] , OldMapPointer , CurLevel->xlen-1 );
      // We don't want to fill the new area with junk, do we? So we make it floor tiles
      CurLevel->map[ i ] [ CurLevel->xlen-1 ] = FLOOR;  
    }

}; // void InsertColumnVeryEast ( Level CurLevel )
      
/* ----------------------------------------------------------------------
 * Self-explanatory.
 * ---------------------------------------------------------------------- */
void
InsertColumnEasternInterface( Level CurLevel )
{
  int i;

  //--------------------
  // First a sanity check:  If there's no eastern threshold, this
  // must be a mistake and will just be ignored...
  //
  if ( CurLevel -> jump_threshold_east <= 0 ) return;

  //--------------------
  // We use available methods to add a column, even if in the wrong
  // place for now.
  //
  InsertColumnVeryEast ( CurLevel );

  //--------------------
  // Now the new memory and everything is done.  All we
  // need to do is move the information to the east
  //
  for ( i = 0 ; i < CurLevel->ylen ; i ++ )
    {
      //--------------------
      // REMEMBER:  WE MUST NO USE MEMCPY HERE, CAUSE THE AREAS IN QUESTION
      // MIGHT (EVEN WILL) OVERLAP!!  THAT MUST NOT BE THE CASE WITH MEMCPY!!
      //
      memmove ( & ( CurLevel->map [ i ] [ CurLevel->xlen - CurLevel->jump_threshold_east - 1 ] ) ,
		& ( CurLevel->map [ i ] [ CurLevel->xlen - CurLevel->jump_threshold_east - 2 ] ) ,
		CurLevel->jump_threshold_east );
      CurLevel->map [ i ] [ CurLevel->xlen - CurLevel->jump_threshold_east - 1 ] = FLOOR ;
    }

}; // void InsertColumnEasternInterface( CurLevel );

/* ----------------------------------------------------------------------
 * Self-explanatory.
 * ---------------------------------------------------------------------- */
void
RemoveColumnEasternInterface( Level CurLevel )
{
  int i;

  //--------------------
  // First a sanity check:  If there's no eastern threshold, this
  // must be a mistake and will just be ignored...
  //
  if ( CurLevel -> jump_threshold_east <= 0 ) return;

  //--------------------
  // Now the new memory and everything is done.  All we
  // need to do is move the information to the east
  //
  for ( i = 0 ; i < CurLevel->ylen ; i ++ )
    {
      //--------------------
      // REMEMBER:  WE MUST NO USE MEMCPY HERE, CAUSE THE AREAS IN QUESTION
      // MIGHT (EVEN WILL) OVERLAP!!  THAT MUST NOT BE THE CASE WITH MEMCPY!!
      //
      memmove ( & ( CurLevel->map [ i ] [ CurLevel->xlen - CurLevel->jump_threshold_east - 1 ] ) ,
		& ( CurLevel->map [ i ] [ CurLevel->xlen - CurLevel->jump_threshold_east - 0 ] ) ,
		CurLevel->jump_threshold_east - 0 );
      // CurLevel->map [ i ] [ CurLevel->xlen - CurLevel->jump_threshold_east - 1 ] = FLOOR ;
    }

  CurLevel -> xlen --;

}; // void InsertColumnEasternInterface( CurLevel );

/* ----------------------------------------------------------------------
 * Self-explanatory.
 * ---------------------------------------------------------------------- */
void
InsertColumnWesternInterface( Level CurLevel )
{
  int BackupOfEasternInterface;

  //--------------------
  // First a sanity check:  If there's no western threshold, this
  // must be a mistake and will just be ignored...
  //
  if ( CurLevel -> jump_threshold_west <= 0 ) return;

  //--------------------
  // Again we exploit existing code, namely the insertion procedure
  // for the eastern interface.  We shortly change the interface, use
  // that code from the eastern interface and restore the eastern interface.
  //
  BackupOfEasternInterface = CurLevel->jump_threshold_east;
  CurLevel->jump_threshold_east = CurLevel->xlen - CurLevel->jump_threshold_west ;
  InsertColumnEasternInterface ( CurLevel );
  CurLevel->jump_threshold_east = BackupOfEasternInterface ;

}; // void InsertColumnWesternInterface( Level CurLevel )

/* ----------------------------------------------------------------------
 * Self-explanatory.
 * ---------------------------------------------------------------------- */
void
RemoveColumnWesternInterface( Level CurLevel )
{
  int BackupOfEasternInterface;

  //--------------------
  // First a sanity check:  If there's no western threshold, this
  // must be a mistake and will just be ignored...
  //
  if ( CurLevel -> jump_threshold_west <= 0 ) return;

  //--------------------
  // Again we exploit existing code, namely the insertion procedure
  // for the eastern interface.  We shortly change the interface, use
  // that code from the eastern interface and restore the eastern interface.
  //
  BackupOfEasternInterface = CurLevel->jump_threshold_east;
  CurLevel->jump_threshold_east = CurLevel->xlen - CurLevel->jump_threshold_west - 1;
  RemoveColumnEasternInterface ( CurLevel );
  CurLevel->jump_threshold_east = BackupOfEasternInterface ;

}; // void RemoveColumnWesternInterface( Level CurLevel )

/* ----------------------------------------------------------------------
 * Self-Explanatory.
 * ---------------------------------------------------------------------- */
void
InsertLineSouthernInterface ( Level CurLevel )
{
  char* temp;
  int i;

  //--------------------
  // First a sanity check for existing interface
  //
  if ( CurLevel -> jump_threshold_south <= 0 ) return;

  //--------------------
  // We build upon the existing code again.
  //
  InsertLineVerySouth( CurLevel );
  
  //--------------------
  // Now we do some swapping of lines
  //
  temp = CurLevel -> map [ CurLevel -> ylen - 1 ] ;

  for ( i = 0 ; i < CurLevel -> jump_threshold_south ; i ++ )
    {
      CurLevel -> map [ CurLevel -> ylen - i - 1 ] = 
	CurLevel -> map [ CurLevel -> ylen - i - 2 ] ;
    }
  CurLevel -> map [ CurLevel -> ylen - 1 - CurLevel -> jump_threshold_south ] = temp ;

}; // void InsertLineSouthernInterface ( CurLevel )

/* ----------------------------------------------------------------------
 * Self-Explanatory.
 * ---------------------------------------------------------------------- */
void
RemoveLineSouthernInterface ( Level CurLevel )
{
  int i;

  //--------------------
  // First a sanity check for existing interface
  //
  if ( CurLevel -> jump_threshold_south <= 0 ) return;

  //--------------------
  // Now we do some swapping of lines
  //
  for ( i = CurLevel -> ylen - 1 - CurLevel -> jump_threshold_south ; 
	i < CurLevel -> ylen - 1 ; i ++ )
    {
      CurLevel -> map [ i ] = CurLevel -> map [ i + 1 ] ;
    }
  CurLevel -> ylen -- ;

}; // void RemoveLineSouthernInterface ( CurLevel )

/* ----------------------------------------------------------------------
 * Self-Explanatory.
 * ---------------------------------------------------------------------- */
void
InsertLineNorthernInterface ( Level CurLevel )
{
  int OldSouthernInterface;

  //--------------------
  // First a sanity check for existing interface
  //
  if ( CurLevel -> jump_threshold_north <= 0 ) return;

  //--------------------
  // We shortly change the southern interface to reuse the code for there
  //
  OldSouthernInterface = CurLevel -> jump_threshold_south;

  CurLevel -> jump_threshold_south = CurLevel -> ylen - CurLevel -> jump_threshold_north - 0 ;
  InsertLineSouthernInterface ( CurLevel );

  CurLevel -> jump_threshold_south = OldSouthernInterface ;

}; // void InsertLineNorthernInterface ( CurLevel )

/* ----------------------------------------------------------------------
 * Self-Explanatory.
 * ---------------------------------------------------------------------- */
void
RemoveLineNorthernInterface ( Level CurLevel )
{
  int OldSouthernInterface;

  //--------------------
  // First a sanity check for existing interface
  //
  if ( CurLevel -> jump_threshold_north <= 0 ) return;

  //--------------------
  // We shortly change the southern interface to reuse the code for there
  //
  OldSouthernInterface = CurLevel -> jump_threshold_south;

  CurLevel -> jump_threshold_south = CurLevel -> ylen - CurLevel -> jump_threshold_north - 1 ;
  RemoveLineSouthernInterface ( CurLevel );

  CurLevel -> jump_threshold_south = OldSouthernInterface ;

}; // void RemoveLineNorthernInterface ( Level CurLevel )

/* ----------------------------------------------------------------------
 * Self-Explanatory.
 * ---------------------------------------------------------------------- */
void
InsertLineVeryNorth ( Level CurLevel )
{
  int OldSouthernInterface;

  //--------------------
  // We shortly change the southern interface to reuse the code for there
  //
  OldSouthernInterface = CurLevel -> jump_threshold_south;

  CurLevel -> jump_threshold_south = CurLevel -> ylen - 0 ;
  InsertLineSouthernInterface ( CurLevel );

  CurLevel -> jump_threshold_south = OldSouthernInterface ;

}; // void InsertLineVeryNorth ( CurLevel )

/* ----------------------------------------------------------------------
 * Self-Explanatory.
 * ---------------------------------------------------------------------- */
void
RemoveLineVeryNorth ( Level CurLevel )
{
  int OldSouthernInterface;

  //--------------------
  // We shortly change the southern interface to reuse the code for there
  //
  OldSouthernInterface = CurLevel -> jump_threshold_south;

  CurLevel -> jump_threshold_south = CurLevel -> ylen - 1 ;
  RemoveLineSouthernInterface ( CurLevel );

  CurLevel -> jump_threshold_south = OldSouthernInterface ;

}; // void RemoveLineVeryNorth ( Level CurLevel )

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
      
      sprintf( Options [ 0 ] , "Current level size in X: %d." , CurLevel->xlen );
      MenuTexts[ 8 ] = Options [ 0 ];
      sprintf( Options [ 1 ] , "Current level size in Y: %d." , CurLevel->ylen  );
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
	      InsertColumnVeryEast( CurLevel );
	      while (RightPressed());
	    }
	  if ( LeftPressed() )
	    {
	      CurLevel->xlen--; // making it smaller is always easy:  just modify the value for size
	      // allocation of new memory or things like that are not nescessary
	      while (LeftPressed());
	    }
	  break;

	case INSERTREMOVE_COLUMN_EASTERN_INTERFACE:
	  if ( RightPressed() )
	    {
	      InsertColumnEasternInterface( CurLevel );
	      while (RightPressed());
	    }
	  if ( LeftPressed() )
	    {
	      RemoveColumnEasternInterface( CurLevel );
	      while (LeftPressed());
	    }
	  break;

	case INSERTREMOVE_COLUMN_WESTERN_INTERFACE:
	  if ( RightPressed() )
	    {
	      InsertColumnWesternInterface( CurLevel );
	      while (RightPressed());
	    }
	  if ( LeftPressed() )
	    {
	      RemoveColumnWesternInterface( CurLevel );
	      while (LeftPressed());
	    }
	  break;
	  
	case INSERTREMOVE_LINE_VERY_SOUTH:
	  if ( RightPressed() )
	    {
	      InsertLineVerySouth ( CurLevel );
	      while (RightPressed());
	    }
	  
	  if ( LeftPressed() )
	    {
	      CurLevel->ylen--; // making it smaller is always easy:  just modify the value for size
	      // allocation of new memory or things like that are not nescessary.
	      while (LeftPressed());
	    }
	  break;

	case INSERTREMOVE_LINE_SOUTHERN_INTERFACE:
	  if ( RightPressed() )
	    {
	      InsertLineSouthernInterface ( CurLevel );
	      while (RightPressed());
	    }
	  if ( LeftPressed() )
	    {
	      RemoveLineSouthernInterface ( CurLevel );
	      while (LeftPressed());
	    }
	  break;

	case INSERTREMOVE_LINE_NORTHERN_INTERFACE:
	  if ( RightPressed() )
	    {
	      InsertLineNorthernInterface ( CurLevel );
	      while (RightPressed());
	    }
	  if ( LeftPressed() )
	    {
	      RemoveLineNorthernInterface ( CurLevel );
	      while (LeftPressed());
	    }
	  break;

	case INSERTREMOVE_LINE_VERY_NORTH:
	  if ( RightPressed() )
	    {
	      InsertLineVeryNorth ( CurLevel );
	      while (RightPressed());
	    }
	  if ( LeftPressed() )
	    {
	      RemoveLineVeryNorth ( CurLevel );
	      while (LeftPressed());
	    }
	  break;

	case (-1):
	case BACK_TO_LE_MAIN_MENU:
	  while (EnterPressed() || SpacePressed() || EscapePressed() ) ;
	  GetDoors ( CurLevel );
	  GetRefreshes ( CurLevel );
	  GetTeleports ( CurLevel );
	  GetAutoguns ( CurLevel );
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
DoLevelEditorMainMenu ( Level CurLevel )
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
      
      InitiateMenu( NULL );
      
      MenuTexts[ 0 ] = "Save whole ship to 'Testship.shp'" ;
      sprintf( Options [ 0 ] , "Current: %d.  Level Up/Down" , CurLevel->levelnum );
      MenuTexts[ 1 ] = Options [ 0 ];
      MenuTexts[ 2 ] = "Change tile set" ;
      sprintf( Options [ 1 ] , "Current levelsize: %d x %d map tiles." , CurLevel->xlen , CurLevel->ylen );
      MenuTexts[ 3 ] = Options [ 1 ];
      sprintf( Options [ 2 ] , "Levelsize in Y: %d.  Shrink/Enlarge" , CurLevel->ylen  );
      MenuTexts[ 4 ] = Options [ 2 ] ;
      sprintf( Options [ 3 ] , "Level name: %s" , CurLevel->Levelname );
      MenuTexts[ 5 ] = Options [ 3 ] ;
      sprintf( Options [ 4 ] , "Background music file name: %s" , CurLevel->Background_Song_Name );
      MenuTexts[ 6 ] = Options [ 4 ] ;
      sprintf( Options [ 5 ] , "Set Level Comment: %s" , CurLevel->Level_Enter_Comment );
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
	  // if ( CurLevel->levelnum ) Teleport ( CurLevel->levelnum-1 , Me[0].pos.x , Me[0].pos.y ); 
	  while (EnterPressed() || SpacePressed() ) ;
	  break;
	case CHANGE_TILE_SET_POSITION: 
	  while (EnterPressed() || SpacePressed() ) ;
	  break;
	case SET_LEVEL_NAME:
	  while (EnterPressed() || SpacePressed() ) ;
	  CenteredPutString ( Screen ,  12*FontHeight(Menu_BFont), "Please enter new level name:");
	  SDL_Flip( Screen );
	  CurLevel->Levelname=GetString( 100 , FALSE );
	  Weiter=!Weiter;
	  break;
	case SET_BACKGROUND_SONG_NAME:
	  while (EnterPressed() || SpacePressed() ) ;
	  CenteredPutString ( Screen ,  12*FontHeight(Menu_BFont), "Please enter new music file name:");
	  SDL_Flip( Screen );
	  CurLevel->Background_Song_Name=GetString( 100 , FALSE );
	  Weiter=!Weiter;
	  break;
	case SET_LEVEL_COMMENT:
	  while (EnterPressed() || SpacePressed() ) ;
	  CenteredPutString ( Screen ,  12*FontHeight(Menu_BFont), "Please enter new level comment:\n");
	  SDL_Flip( Screen );
	  SetTextCursor( 15 , 440 );
	  CurLevel->Level_Enter_Comment=GetString( 100 , FALSE );
	  Weiter=!Weiter;
	  break;
	case ADD_NEW_LEVEL:
	  while (EnterPressed() || SpacePressed() ) ;
	  curShip.AllLevels[ curShip.num_levels ] = CreateNewMapLevel();
	  curShip.num_levels ++ ;
	  CenteredPutString ( Screen ,  12*FontHeight(Menu_BFont), "New level has been added!");
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
		  if ( CurLevel->levelnum > 0 )
		    Teleport ( CurLevel->levelnum -1 , 3 , 3 , 0 , TRUE );
		  while (LeftPressed());
		}
	      if ( RightPressed() )
		{
		  if ( CurLevel->levelnum < curShip.num_levels -1 )
		    Teleport ( CurLevel->levelnum +1 , 3 , 3 , 0 , TRUE );
		  while (RightPressed());
		}
	      if ( CurrentCombatScaleFactor != 1 ) SetCombatScaleTo ( CurrentCombatScaleFactor );
	      break;
	      
	    case CHANGE_TILE_SET_POSITION:
	      if ( RightPressed() && (CurLevel->color  < 6 ) )
		{
		  CurLevel->color++;
		  while (RightPressed());
		}
	      if ( LeftPressed() && (CurLevel->color > 0) )
		{
		  CurLevel->color--;
		  while (LeftPressed());
		}
	      Teleport ( CurLevel->levelnum , Me[0].pos.x , Me[0].pos.y , 0 , TRUE ); 
	      break;

	    case CHANGE_SIZE_X:
	      /*
	      if ( RightPressed() )
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
		  while (RightPressed());
		}
	      if ( LeftPressed() )
		{
		  CurLevel->xlen--; // making it smaller is always easy:  just modify the value for size
		  // allocation of new memory or things like that are not nescessary.
		  while (LeftPressed());
		}
	      */
	      break;
	      
	    case CHANGE_SIZE_Y:
	      
	      /*
	      if ( RightPressed() )
		{
		  
		  //--------------------
		  // The enlargement of levels in y direction is limited by a constant
		  // defined in defs.h.  This is carefully checked or no operation at
		  // all will be performed.
		  //
		  if ( (CurLevel->ylen)+1 < MAX_MAP_LINES )
		    {
		      CurLevel->ylen++;
		      // In case of enlargement, we need to do more:
		      CurLevel->map[ CurLevel->ylen-1 ] = MyMalloc( CurLevel->xlen +1) ;
		      // We don't want to fill the new area with junk, do we? So we set it VOID
		      memset( CurLevel->map[ CurLevel->ylen-1 ] , VOID , CurLevel->xlen );
		    }
		  while (RightPressed());
		}
	      
	      if ( LeftPressed() )
		{
		  CurLevel->ylen--; // making it smaller is always easy:  just modify the value for size
		  // allocation of new memory or things like that are not nescessary.
		  while (LeftPressed());
		}
	      */

	      break;
	      
	    }
	} // if LeftPressed || RightPressed
      
    }

  return ( Done );

}; // void DoLevelEditorMainMenu ( Level CurLevel );

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void 
ShowLevelEditorKeymap ( void )
{
  int k=1;

  // SDL_BlitSurface ( console_bg_pic2 , NULL, ne_screen, NULL);
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
      GetDoors ( curShip . AllLevels [ TargetLevel ] );
      GetRefreshes ( curShip . AllLevels [ TargetLevel ] );
      GetTeleports ( curShip . AllLevels [ TargetLevel ] );
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
      GetDoors ( curShip . AllLevels [ TargetLevel ] );
      GetRefreshes ( curShip . AllLevels [ TargetLevel ] );
      GetTeleports ( curShip . AllLevels [ TargetLevel ] );
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
      GetDoors ( curShip . AllLevels [ TargetLevel ] );
      GetRefreshes ( curShip . AllLevels [ TargetLevel ] );
      GetTeleports ( curShip . AllLevels [ TargetLevel ] );
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
      GetDoors ( curShip . AllLevels [ TargetLevel ] );
      GetRefreshes ( curShip . AllLevels [ TargetLevel ] );
      GetTeleports ( curShip . AllLevels [ TargetLevel ] );
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

  while (!Weiter)
    {
      
      InitiateMenu( NULL );
      
      sprintf( Options [ 0 ] , "Jump threshold north: %d.  Up/Down" , CurLevel->jump_threshold_north );
      MenuTexts [ 0 ] = Options [ 0 ] ;
      sprintf( Options [ 1 ] , "Jump threshold south: %d.  Up/Down" , CurLevel->jump_threshold_south );
      MenuTexts [ 1 ] = Options [ 1 ] ;
      sprintf( Options [ 2 ] , "Jump threshold east: %d.  Up/Down" , CurLevel->jump_threshold_east );
      MenuTexts [ 2 ] = Options [ 2 ] ;
      sprintf( Options [ 3 ] , "Jump threshold west: %d.  Up/Down" , CurLevel->jump_threshold_west );
      MenuTexts [ 3 ] = Options [ 3 ] ;
      sprintf( Options [ 4 ] , "Jump target north: %d.  Up/Down" , CurLevel->jump_target_north );
      MenuTexts [ 4 ] = Options [ 4 ] ;
      sprintf( Options [ 5 ] , "Jump target south: %d.  Up/Down" , CurLevel->jump_target_south );
      MenuTexts [ 5 ] = Options [ 5 ] ;
      sprintf( Options [ 6 ] , "Jump target east: %d.  Up/Down" , CurLevel->jump_target_east );
      MenuTexts [ 6 ] = Options [ 6 ] ;
      sprintf( Options [ 7 ] , "Jump target west: %d.  Up/Down" , CurLevel->jump_target_west );
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
	  ReportInconsistenciesForLevel ( 0 );
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
		  if ( CurLevel->jump_threshold_north >= 0 ) CurLevel->jump_threshold_north -- ;
		  while (LeftPressed());
		}
	      if ( RightPressed() )
		{
		  CurLevel->jump_threshold_north ++ ;
		  while (RightPressed());
		}
	      break;
	      
	    case JUMP_THRESHOLD_SOUTH:
	      if ( LeftPressed() )
		{
		  if ( CurLevel->jump_threshold_south >= 0 ) CurLevel->jump_threshold_south -- ;
		  while (LeftPressed());
		}
	      if ( RightPressed() )
		{
		  CurLevel->jump_threshold_south ++ ;
		  while (RightPressed());
		}
	      break;

	    case JUMP_THRESHOLD_EAST:
	      if ( LeftPressed() )
		{
		  if ( CurLevel->jump_threshold_east >= 0 ) CurLevel->jump_threshold_east -- ;
		  while (LeftPressed());
		}
	      if ( RightPressed() )
		{
		  CurLevel->jump_threshold_east ++ ;
		  while (RightPressed());
		}
	      break;

	    case JUMP_THRESHOLD_WEST:
	      if ( LeftPressed() )
		{
		  if ( CurLevel->jump_threshold_west >= 0 ) CurLevel->jump_threshold_west -- ;
		  while (LeftPressed());
		}
	      if ( RightPressed() )
		{
		  CurLevel->jump_threshold_west ++ ;
		  while (RightPressed());
		}
	      break;
	      
	    case JUMP_TARGET_NORTH:
	      if ( LeftPressed() )
		{
		  if ( CurLevel->jump_target_north >= 0 ) CurLevel->jump_target_north -- ;
		  while (LeftPressed());
		}
	      if ( RightPressed() )
		{
		  CurLevel->jump_target_north ++ ;
		  while (RightPressed());
		}
	      break;
	      
	    case JUMP_TARGET_SOUTH:
	      if ( LeftPressed() )
		{
		  if ( CurLevel->jump_target_south >= 0 ) CurLevel->jump_target_south -- ;
		  while (LeftPressed());
		}
	      if ( RightPressed() )
		{
		  CurLevel->jump_target_south ++ ;
		  while (RightPressed());
		}
	      break;

	    case JUMP_TARGET_EAST:
	      if ( LeftPressed() )
		{
		  if ( CurLevel->jump_target_east >= 0 ) CurLevel->jump_target_east -- ;
		  while (LeftPressed());
		}
	      if ( RightPressed() )
		{
		  CurLevel->jump_target_east ++ ;
		  while (RightPressed());
		}
	      break;

	    case JUMP_TARGET_WEST:
	      if ( LeftPressed() )
		{
		  if ( CurLevel->jump_target_west >= 0 ) CurLevel->jump_target_west -- ;
		  while (LeftPressed());
		}
	      if ( RightPressed() )
		{
		  CurLevel->jump_target_west ++ ;
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

  NewLevel->empty = FALSE;

  DebugPrintf (0, "\n-----------------------------------------------------------------");
  DebugPrintf (0, "\nStarting to create and add a completely new level to the ship.");

  NewLevel->levelnum = curShip.num_levels;
  NewLevel->xlen = 9;
  NewLevel->ylen = 9;
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
      NewLevel -> AllWaypoints [ i ] . x = -1 ;
      NewLevel -> AllWaypoints [ i ] . y = -1 ;

      for ( k=0 ; k<MAX_WP_CONNECTIONS ; k++ )
	{
	  NewLevel -> AllWaypoints [ i ] . connections [ k ] = -1 ;
	}
    }

  //--------------------
  return NewLevel;
}; // Level CreateNewMapLevel( void )

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
  char PanelText[5000]="";
  int Codepanel_Index;

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
  switch ( CurLevel->map [ (int)rintf( Me[0].pos.y) ] [ (int)rintf( Me[0].pos.x ) ] )
    {
    case CODEPANEL_L:
    case CODEPANEL_R:
    case CODEPANEL_U:
    case CODEPANEL_D:

      for ( Codepanel_Index = 0 ; Codepanel_Index < MAX_CODEPANELS_PER_LEVEL ; Codepanel_Index ++ )
	{
	  if ( ( ( (int) rintf( Me[0].pos.x ) ) == CurLevel->CodepanelList[ Codepanel_Index ].x ) && 
	       ( ( (int) rintf( Me[0].pos.y ) ) == CurLevel->CodepanelList[ Codepanel_Index ].y ) )
	    break;
	}

      if ( Codepanel_Index >= MAX_CODEPANELS_PER_LEVEL )
	{
	  sprintf( PanelText , "\nWARNING!  Either no codepanel code present or last entry used.\n" );
	}
      else
	{
	  sprintf( PanelText , "\nCode Panel Information: \n Codeword=\"%s\"." , 
		   CurLevel->CodepanelList[ Codepanel_Index ].Secret_Code );
	}

      DisplayText ( PanelText , User_Rect.x , User_Rect.y , &User_Rect );
      break;
    default:
      break;
    }
} // void Highlight_Current_Block(void)

/* ----------------------------------------------------------------------
 * This function is used by the Level Editor integrated into 
 * freedroid.  It marks all waypoints with a cross.
 * ---------------------------------------------------------------------- */
void 
Show_Waypoints( int PrintConnectionList )
{
  int wp;
  int i;
  int x;
  int y;
  int BlockX, BlockY;
  int color;
  char ConnectionText[5000];
  char TextAddition[1000];

#define ACTIVE_WP_COLOR 0x0FFFFFFFF

  BlockX=rintf(Me[0].pos.x);
  BlockY=rintf(Me[0].pos.y);
	  
  SDL_LockSurface( Screen );

  for (wp=0; wp<MAXWAYPOINTS; wp++)
    {

      if ( CurLevel->AllWaypoints[wp].x == 0) continue;

      //--------------------
      // Draw the cross in the middle of the middle of the tile
      //
      for (i= Block_Width/4; i<3 * Block_Width / 4; i++)
	{
	  // This draws a (double) line at the upper border of the current block
	  x = i + User_Rect.x+(User_Rect.w/2)- (( Me[0].pos.x)-CurLevel->AllWaypoints[wp].x + 0.5) * Block_Width;
	  y = i + User_Rect.y+User_Rect.h/2 - (( Me[0].pos.y)-CurLevel->AllWaypoints[wp].y + 0.5) * Block_Height;
	  if ( ( x < User_Rect.x ) || ( x > User_Rect.x + User_Rect.w ) || ( y < User_Rect. y) || ( y > User_Rect.y + User_Rect.h ) ) continue;
	  putpixel( Screen , x , y , HIGHLIGHTCOLOR );

		    
	  x = i + User_Rect.x + (User_Rect.w/2) - (( Me[0].pos.x )-CurLevel->AllWaypoints[wp].x + 0.5) * Block_Width;
	  y = i + User_Rect.y+User_Rect.h/2- (( Me[0].pos.y)-CurLevel->AllWaypoints[wp].y + 0.5) * Block_Height + 1;
	  if ( ( x < User_Rect.x ) || ( x > User_Rect.x + User_Rect.w ) || ( y < User_Rect. y) || ( y > User_Rect.y + User_Rect.h ) ) continue;
	  putpixel( Screen , x , y , HIGHLIGHTCOLOR );
	  
	  // This draws a line at the lower border of the current block
	  x = i + User_Rect.x + (User_Rect.w/2) - (( Me[0].pos.x)-CurLevel->AllWaypoints[wp].x + 0.5) * Block_Width;
	  y = -i + User_Rect.y + User_Rect.h/2 - (( Me[0].pos.y )-CurLevel->AllWaypoints[wp].y - 0.5 ) * Block_Height -1;
	  if ( ( x < User_Rect.x ) || ( x > User_Rect.x + User_Rect.w ) || ( y < User_Rect. y) || ( y > User_Rect.y + User_Rect.h ) ) continue;
	  putpixel( Screen , x , y , HIGHLIGHTCOLOR );

	  x = i + User_Rect.x + (User_Rect.w/2) - (( Me[0].pos.x)-CurLevel->AllWaypoints[wp].x + 0.5) * Block_Width;
	  y = -i + User_Rect.y + User_Rect.h/2 - ((Me[0].pos.y)-CurLevel->AllWaypoints[wp].y - 0.5 ) * Block_Height -2;
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
	  if ( CurLevel->AllWaypoints[wp].connections[i] != (-1) )
	    {
	      if ( ( BlockX == CurLevel->AllWaypoints[wp].x ) && ( BlockY == CurLevel->AllWaypoints[wp].y ) )
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
				CurLevel->AllWaypoints[CurLevel->AllWaypoints[wp].connections[i]].x , 
				CurLevel->AllWaypoints[CurLevel->AllWaypoints[wp].connections[i]].y 
				);
		      strcat ( ConnectionText , TextAddition );
		      DisplayText ( ConnectionText , User_Rect.x , User_Rect.y , &User_Rect );
		      SDL_LockSurface( Screen );
		    }
		      
		  DrawLineBetweenTiles( CurLevel->AllWaypoints[wp].x , CurLevel->AllWaypoints[wp].y , 
					CurLevel->AllWaypoints[CurLevel->AllWaypoints[wp].connections[i]].x , 
					CurLevel->AllWaypoints[CurLevel->AllWaypoints[wp].connections[i]].y ,
					color );
		}
	    }
	}
	      
    }
  SDL_UnlockSurface( Screen );

} // void Show_Waypoints(void);

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
HandleMapTileEditingKeys ( Level CurLevel , int BlockX , int BlockY )
{
  
  //--------------------
  // Pressing the 'T' key will insert a teleporter field...
  //
  if ( TPressed()) 
    {
      CurLevel -> map [ BlockY ] [ BlockX ] = TELE_1 ;
    }
  
  //--------------------
  // Pressing the '1' to '5' keys will insert either classic 'block'
  // fixed map tiles or decructible 'box' map blocks.
  //
  if (Number1Pressed()) 
    {
      if ( Shift_Was_Pressed() )
	CurLevel->map[BlockY][BlockX]=BOX_1;
      else
	CurLevel->map[BlockY][BlockX]=BLOCK1;
    }
  if (Number2Pressed()) 
    {
      if ( Shift_Was_Pressed() )
	CurLevel->map[BlockY][BlockX]=BOX_2;
      else
	CurLevel->map[BlockY][BlockX]=BLOCK2;
    }
  if (Number3Pressed()) 
    {
      if ( Shift_Was_Pressed() )
	CurLevel->map[BlockY][BlockX]=BOX_3;
      else
	CurLevel->map[BlockY][BlockX]=BLOCK3;
    }
  if (Number4Pressed()) 
    {
      if ( Shift_Was_Pressed() )
	CurLevel->map[BlockY][BlockX]=BOX_4;
      else
	CurLevel->map[BlockY][BlockX]=BLOCK4;
    }
  if (Number5Pressed()) 
    {
      CurLevel->map[BlockY][BlockX]=BLOCK5;
    }
  if (LPressed()) 
    {
      CurLevel->map[BlockY][BlockX]=LIFT;
    }
  if (KP_PLUS_Pressed()) 
    {
      CurLevel->map[BlockY][BlockX]=V_WALL;
    }
  if (KP0Pressed()) 
    {
      CurLevel->map[BlockY][BlockX]=H_WALL;
    }
  if (KP1Pressed()) 
    {
      if ( Shift_Was_Pressed() ) CurLevel->map[BlockY][BlockX]=AUTOGUN_L;
      else if ( Ctrl_Was_Pressed() ) CurLevel->map[BlockY][BlockX]=ENHANCER_LD;
      else CurLevel->map[BlockY][BlockX]=CORNER_LD;
    }
  if (KP2Pressed()) 
    {
      if ( Shift_Was_Pressed() )
	CurLevel->map[BlockY][BlockX]=CONSOLE_D;
      else if ( Ctrl_Was_Pressed() ) 
	CurLevel->map[BlockY][BlockX]=CODEPANEL_D;
      else if ( Alt_Was_Pressed() ) 
	CurLevel->map[BlockY][BlockX]=CONVEY_D;
      else CurLevel->map[BlockY][BlockX]=T_D;
    }
  if (KP3Pressed()) 
    {
      if ( Shift_Was_Pressed() ) CurLevel->map[BlockY][BlockX]=AUTOGUN_U;
      else if ( Ctrl_Was_Pressed() ) CurLevel->map[BlockY][BlockX]=ENHANCER_RD;
      else CurLevel->map[BlockY][BlockX]=CORNER_RD;
    }
  if (KP4Pressed()) 
    {
      if ( Shift_Was_Pressed() )
	CurLevel->map[BlockY][BlockX]=CONSOLE_L;
      else if ( Ctrl_Was_Pressed() ) 
	CurLevel->map[BlockY][BlockX]=CODEPANEL_L;
      else if ( Alt_Was_Pressed() ) 
	CurLevel->map[BlockY][BlockX]=CONVEY_R;
      else CurLevel->map[BlockY][BlockX]=T_L;
    }
  if (KP5Pressed()) 
    {
      if (!Shift_Was_Pressed())
	CurLevel->map[BlockY][BlockX]=KREUZ;
      else CurLevel->map[BlockY][BlockX]=VOID;
    }
  if (KP6Pressed()) 
    {
      if ( Shift_Was_Pressed() )
	CurLevel->map[BlockY][BlockX]=CONSOLE_R;
      else if ( Ctrl_Was_Pressed() ) 
	CurLevel->map[BlockY][BlockX]=CODEPANEL_R;
      else if ( Alt_Was_Pressed() ) 
	CurLevel->map[BlockY][BlockX]=CONVEY_L;
      else CurLevel->map[BlockY][BlockX]=T_R;
    }
  if (KP7Pressed()) 
    {
      if ( Shift_Was_Pressed() ) CurLevel->map[BlockY][BlockX]=AUTOGUN_D;
      else if ( Ctrl_Was_Pressed() ) CurLevel->map[BlockY][BlockX]=ENHANCER_LU;
      else CurLevel->map[BlockY][BlockX]=CORNER_LU;
    }
  if ( KP8Pressed() ) 
    {
      if ( Shift_Was_Pressed() )
	CurLevel->map[BlockY][BlockX]=CONSOLE_U;
      else if ( Ctrl_Was_Pressed() ) 
	CurLevel->map[BlockY][BlockX]=CODEPANEL_U;
      else if ( Alt_Was_Pressed() ) 
	CurLevel->map[BlockY][BlockX]=CONVEY_U;
      else CurLevel->map[BlockY][BlockX]=T_U;
    }
  if (KP9Pressed()) 
    {
      if ( Shift_Was_Pressed() ) CurLevel->map[BlockY][BlockX]=AUTOGUN_R;
      else if ( Ctrl_Was_Pressed() ) CurLevel->map[BlockY][BlockX]=ENHANCER_RU;
      else CurLevel->map[BlockY][BlockX]=CORNER_RU;
    }
  if (APressed())
    {
      CurLevel->map[BlockY][BlockX]=ALERT;	      
    }
  if (RPressed())
    {
      if ( Shift_Was_Pressed() ) CurLevel->map[BlockY][BlockX] = CONSUMER_1;
      else CurLevel->map[BlockY][BlockX] = REFRESH1;	            
    }
  if (DPressed())
    {
      if ( !Ctrl_Was_Pressed())
	{
	  if (Shift_Was_Pressed())
	    CurLevel->map[BlockY][BlockX]=V_SHUT_DOOR;	            	      
	  else CurLevel->map[BlockY][BlockX]=H_SHUT_DOOR;	            	      
	}
      else
	{
	  if (Shift_Was_Pressed())
	    CurLevel->map[BlockY][BlockX]=LOCKED_V_SHUT_DOOR;	            	      
	  else CurLevel->map[BlockY][BlockX]=LOCKED_H_SHUT_DOOR;	            	      
	}
    }
  if (SpacePressed())
    {
      if ( Shift_Was_Pressed() )
	CurLevel->map[BlockY][BlockX]=FINE_GRID;	            	      	    
      else
	CurLevel->map[BlockY][BlockX]=FLOOR;	            	      	    
    }
  
}; // void 

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void 
ToggleBigGraphicsInserts ( Level CurLevel , int BlockX, int BlockY )
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
      if ( CurLevel->MapInsertList [ MapInsertNr ] . type == ( -1 ) ) continue; 
      if ( CurLevel->MapInsertList [ MapInsertNr ] . pos.x != BlockX ) continue; 
      if ( CurLevel->MapInsertList [ MapInsertNr ] . pos.y != BlockY ) continue; 
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
	  if ( CurLevel->MapInsertList [ MapInsertNr ] . type == ( -1 ) ) break;
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
      CurLevel->MapInsertList [ MapInsertNr ] . pos.x = BlockX ;
      CurLevel->MapInsertList [ MapInsertNr ] . pos.y = BlockY ;
      
    }
      
  //--------------------
  // At this point we know, that we have a good map insert index at our
  // hands.  Therefore we increase the number of the type and possible
  // reset it to -1 if the last index was exceeded...
  //
  CurLevel->MapInsertList [ MapInsertNr ] . type ++ ;
  
  if ( CurLevel->MapInsertList [ MapInsertNr ] . type >= MAX_MAP_INSERTS )
    CurLevel->MapInsertList [ MapInsertNr ] . type = -1;
  
}; // void ToggleBigGraphicsInserts ( Level CurLevel , int BlockX, int BlockY );

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void 
HandleLevelEditorCursorKeys ( void )
{
  if (LeftPressed()) 
    {
      if ( rintf(Me[0].pos.x) > 0 ) Me[0].pos.x-=1;
      while (LeftPressed());
    }
  if (RightPressed()) 
    {
      if ( rintf(Me[0].pos.x) < CurLevel->xlen-1 ) Me[0].pos.x+=1;
      while (RightPressed());
    }
  if (UpPressed()) 
    {
      if ( rintf(Me[0].pos.y) > 0 ) Me[0].pos.y-=1;
      while (UpPressed());
    }
  if (DownPressed()) 
    {
      if ( rintf(Me[0].pos.y) < CurLevel->ylen-1 ) Me[0].pos.y+=1;
      while (DownPressed());
    }
}; // void HandleLevelEditorCursorKeys ( void )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
ToggleWaypoint ( Level CurLevel , int BlockX , int BlockY )
{
  int i , k , j ;

  // find out if there is a waypoint on the current square
  for (i=0 ; i < MAXWAYPOINTS ; i++)
    {
      if ( ( CurLevel->AllWaypoints[i].x == BlockX ) &&
	   ( CurLevel->AllWaypoints[i].y == BlockY ) ) break;
    }
  
  // if its waypoint already, this waypoint must be deleted.
  if ( i != MAXWAYPOINTS )
    {
      // Eliminate the waypoint itself
      CurLevel->AllWaypoints[i].x = 0;
      CurLevel->AllWaypoints[i].y = 0;
      for ( k = 0; k < MAX_WP_CONNECTIONS ; k++) 
	CurLevel->AllWaypoints[i].connections[k] = (-1) ;
      
		  
      // Eliminate all connections pointing to this waypoint
      for ( j = 0; j < MAXWAYPOINTS ; j++ )
	{
	  for ( k = 0; k < MAX_WP_CONNECTIONS ; k++) 
	    if ( CurLevel->AllWaypoints[j].connections[k] == i )
	      CurLevel->AllWaypoints[j].connections[k] = (-1) ;
	}
    }
  else // if its not a waypoint already, it must be made into one
    {
      // seek a free position
      for ( i = 0 ; i < MAXWAYPOINTS ; i++ )
	{
	  if ( CurLevel->AllWaypoints[i].x == 0 ) break;
	}
      if ( i == MAXWAYPOINTS )
	{
	  printf("\n\nSorry, no free waypoint available.  Using the first one.");
	  i = 0;
	}
      
      // Now make the new entry into the waypoint list
      CurLevel->AllWaypoints[i].x = BlockX;
      CurLevel->AllWaypoints[i].y = BlockY;
      
      // delete all old connection information from the new waypoint
      for ( k = 0; k < MAX_WP_CONNECTIONS ; k++ ) 
	CurLevel->AllWaypoints[i].connections[k] = (-1) ;
      
    }
  
  printf("\n\n  i is now: %d ", i ); fflush(stdout);
  
}; // void ToggleWaypoint ( Level CurLevel , int BlockX , int BlockY )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
ToggleWaypointConnection ( Level CurLevel , int BlockX , int BlockY )
{
  int i , k ;

  // Determine which waypoint is currently targeted
  for (i=0 ; i < MAXWAYPOINTS ; i++)
    {
      if ( ( CurLevel->AllWaypoints[i].x == BlockX ) &&
	   ( CurLevel->AllWaypoints[i].y == BlockY ) ) break;
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
		  if (CurLevel->AllWaypoints[ OriginWaypoint ].connections[k] == (-1) ) break;
		}
	      if ( k == MAX_WP_CONNECTIONS ) 
		{
		  strcat ( VanishingMessage , "\nSORRY. NO MORE CONNECTIONS AVAILABLE FROM THERE." );
		}
	      else
		{
		  CurLevel->AllWaypoints[ OriginWaypoint ].connections[k] = i;
		  strcat ( VanishingMessage , "\nOPERATION DONE!! CONNECTION SHOULD BE THERE." );
		}
	      OriginWaypoint = (-1);
	    }
	}
    }

}; // void ToggleWaypointConnection ( Level CurLevel , int BlockX , int BlockY )

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
  char* NewCommentOnThisSquare;
  char linebuf[10000];
  long OldTicks;
  SDL_Rect Editor_Window;
  
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
      while (!EscapePressed())
	{
	  //--------------------
	  // Also in the Level-Editor, there is no need to go at full framerate...
	  // We can do with less, cause there are no objects supposed to be 
	  // moving fluently anyway.  Therefore we introduce some rest for the CPU.
	  //
	  usleep ( 2 );

	  BlockX=rintf(Me[0].pos.x);
	  BlockY=rintf(Me[0].pos.y);
	  
	  VanishingMessageDisplayTime += ( SDL_GetTicks ( ) - OldTicks ) / 1000.0 ;
	  OldTicks = SDL_GetTicks ( ) ;

	  ClearUserFenster();
	  AssembleCombatPicture ( ONLY_SHOW_MAP_AND_TEXT | SHOW_GRID );
	  Highlight_Current_Block();
	  Show_Waypoints( FALSE );
	  
	  SetCurrentFont ( FPS_Display_BFont ) ;

	  //--------------------
	  // Now we print out the current status directly onto the window:
	  //
	  CenteredPutString ( Screen ,  0*FontHeight( GetCurrentFont () ),    "LEVEL EDITOR");
	  LeftPutString   ( Screen ,  (1)*FontHeight(Menu_BFont), "F1...Level Editor Keymap"); 

	  if ( OriginWaypoint == ( -1 ) )
	    {
	      sprintf ( linebuf , " Source Waypoint selected : NONE" );
	    }
	  else
	    {
	      sprintf ( linebuf , " Source Waypoint selected : X=%d Y=%d. " , 
			CurLevel -> AllWaypoints [ OriginWaypoint ] . x , 
			CurLevel -> AllWaypoints [ OriginWaypoint ] . y );
	    }
	  LeftPutString ( Screen , 4 * FontHeight( GetCurrentFont() ), linebuf );

	  //--------------------
	  // Now we print out the latest connection operation success or failure...
	  //
	  if ( VanishingMessageDisplayTime < 7 )
	    {
	      DisplayText ( VanishingMessage ,  1 , 5 * FontHeight ( GetCurrentFont () ) , NULL );
	    }

	  //--------------------
	  // Now that everything is blitted and printed, we may update the screen again...
	  //
	  SDL_Flip( Screen );

	  //--------------------
	  // If the user of the Level editor pressed some cursor keys, move the
	  // highlited filed (that is Me[0].pos) accordingly. This is done here:
	  //
	  HandleLevelEditorCursorKeys();

	  if ( F1Pressed() ) ShowLevelEditorKeymap ();	   

	  //--------------------
	  // With the 'S' key, you can attach a statement for the influencer to 
	  // say to a given location, i.e. the location the map editor cursor
	  // currently is on.
	  //
	  if ( SPressed () )
	    {
	      while (SPressed());
	      SetCurrentFont( FPS_Display_BFont );
	      // CenteredPutString   ( Screen ,  6*FontHeight(Menu_BFont), "Please enter new value (blindly):");
	      DisplayText ( "\n Please enter comment below: \n" , -1 , -1 , &User_Rect );
	      SDL_Flip( Screen );
	      NewCommentOnThisSquare = GetString( 1000, FALSE );  // TRUE currently not implemented
	      for ( i = 0 ; i < MAX_STATEMENTS_PER_LEVEL ; i ++ )
		{
		  if ( CurLevel->StatementList[ i ].x == (-1) ) break;
		}
	      if ( i == MAX_STATEMENTS_PER_LEVEL ) 
		{
		  DisplayText ( "\nNo more free comment position.  Using first. " , -1 , -1 , &User_Rect );
		  i=0;
		  SDL_Flip ( Screen );
		  getchar_raw();
		  // Terminate( ERR );
		}

	      CurLevel->StatementList[ i ].Statement_Text = NewCommentOnThisSquare;
	      CurLevel->StatementList[ i ].x = rintf( Me[0].pos.x );
	      CurLevel->StatementList[ i ].y = rintf( Me[0].pos.y );
	    }

	  //--------------------
	  // With the 'P' key, you can edit the codepanel codeword attached to any 
	  // codepanel.  Of course the cursor must be positioned at this codepanel
	  // so the feature can work.
	  //
	  if ( PPressed () )
	    {
	      while (PPressed());
	      SetCurrentFont( FPS_Display_BFont );

	      // First we check if we really are directly on a codepanel:
	      switch ( CurLevel->map [ (int)rintf( Me[0].pos.y) ] [ (int)rintf( Me[0].pos.x ) ] )
		{
		case CODEPANEL_L:
		case CODEPANEL_R:
		case CODEPANEL_U:
		case CODEPANEL_D:

		  // If yes, we ask for the new codepanel keyword
		  DisplayText ( "\n Please enter new codepanel codeword: \n" , -1 , -1 , &User_Rect );
		  SDL_Flip( Screen );
		  NewCommentOnThisSquare = GetString( 1000, FALSE );  // TRUE currently not implemented

		  // Now we see if a codepanel entry is existing already for this square
		  for ( i = 0 ; i < MAX_CODEPANELS_PER_LEVEL ; i ++ )
		    {
		      if ( ( CurLevel->CodepanelList[ i ].x == (int)rintf( Me[0].pos.x) ) &&
			   ( CurLevel->CodepanelList[ i ].y == (int)rintf( Me[0].pos.y) ) ) break;
		    }
		  if ( i >= MAX_CODEPANELS_PER_LEVEL ) 
		    {
		      DisplayText ( "\nNo existing codepanel entry found...\n" , -1 , -1 , &User_Rect );
		      i=0;
		      for ( i = 0 ; i < MAX_CODEPANELS_PER_LEVEL ; i ++ )
			{
			  if ( CurLevel->CodepanelList[ i ].x == (-1) )
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
		  CurLevel->CodepanelList[ i ].Secret_Code = NewCommentOnThisSquare;
		  CurLevel->CodepanelList[ i ].x = rintf( Me[0].pos.x );
		  CurLevel->CodepanelList[ i ].y = rintf( Me[0].pos.y );


		  SDL_Flip ( Screen );
		  getchar_raw();
		  
		  break;
		default:
		  DisplayText ( "\nBut you are not on a codepanel!!\n" , -1 , -1 , &User_Rect );
		  SDL_Flip( Screen );
		  getchar_raw();
		  break;
		}
	      
	    }

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
	      CenteredPutString   ( Screen ,  6*FontHeight(Menu_BFont), "Please enter new value (blindly):");
	      SDL_Flip( Screen );
	      NumericInputString=GetString( 10, FALSE );  // TRUE currently not implemented
	      sscanf( NumericInputString , "%d" , &SpecialMapValue );
	      if ( SpecialMapValue >= NUM_MAP_BLOCKS ) SpecialMapValue=0;
	      CurLevel->map[BlockY][BlockX]=SpecialMapValue;
	    }

	  //--------------------
	  // From the level editor, it should also be possible to drop new goods
	  // at some location via the 'G' key. (G like in Goods.)
	  //
	  if ( GPressed () )
	    {
	      while ( GPressed() );
	      CenteredPutString   ( Screen ,  6*FontHeight(Menu_BFont), "Please enter code of new item:");
	      SDL_Flip( Screen );
	      NumericInputString=GetString( 10, FALSE );  // TRUE currently not implemented
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
	  if ( OPressed () )
	    {
	      if (CurrentCombatScaleFactor > 0.25 )
		CurrentCombatScaleFactor -= 0.25;
	      SetCombatScaleTo (CurrentCombatScaleFactor);
	      while (OPressed());
	    }
	  if ( IPressed () )
	    {
	      CurrentCombatScaleFactor += 0.25;
	      SetCombatScaleTo (CurrentCombatScaleFactor);
	      while (IPressed());
	    }
  
	  //--------------------
	  // If the person using the level editor pressed w, the waypoint is
	  // toggled on the current square.  That means either removed or added.
	  // And in case of removal, also the connections must be removed.
	  //
	  if (WPressed())
	    {
	      ToggleWaypoint ( CurLevel , BlockX, BlockY );
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
	      ToggleWaypointConnection ( CurLevel, BlockX, BlockY );
	      while (CPressed());
	      fflush(stdout);
	    }

	  //--------------------
	  // Pressing the 'B' key will toggle the big graphics insert by one...
	  //
	  if ( BPressed()) 
	    {
	      while ( BPressed() );
	      ToggleBigGraphicsInserts ( CurLevel , BlockX, BlockY );
	    }
	  
	  //----------------------------------------------------------------------
	  // If the person using the level editor pressed some editing keys, insert the
	  // corresponding map tile.  This is done in the following:
	  //
	  HandleMapTileEditingKeys ( CurLevel , BlockX , BlockY );

	  if (QPressed())
	    {
	      Terminate(0);
	    }

	} // while (!EscapePressed())
      while( EscapePressed() );

      //--------------------
      // After Level editing is done and escape has been pressed, 
      // display the Menu with level save options and all that.
      //
      Done = DoLevelEditorMainMenu ( CurLevel );
      
    } // while (!Done)

}; // void LevelEditor ( void )








#undef _leveleditor_c
