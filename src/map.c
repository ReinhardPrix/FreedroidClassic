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
 * This file contains (all?) map-related functions, which also includes 
 * loading of decks and whole ships, starting the lifts and consoles if 
 * close to the paradroid, refreshes as well as determining the map brick 
 * that contains specified coordinates are done in this file.
 * ---------------------------------------------------------------------- */

#define _map_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "proto.h"
#include "global.h"

#include "map.h"

void
TranslateToHumanReadable ( Uint16* HumanReadable , map_tile* MapInfo, int LineLength , Level Lev , int CurrentLine);
// void TranslateToHumanReadable ( Uint16* HumanReadable , Uint16* MapInfo, int LineLength , Level Lev , int CurrentLine);
void GetThisLevelsDroids( char* SectionPointer );
Level DecodeLoadedLeveldata ( char *data );
// int IsWallBlock ( int block );


/* -----------------------------------------------------------------
 *
 *
 *-----------------------------------------------------------------*/
int
decode_floor_tiles_of_this_level (Level Lev)
{
  int xdim = Lev->xlen;
  int ydim = Lev->ylen;
  int row, col;
  map_tile *Buffer;
  int tmp;
  int glue_index;

  DebugPrintf ( 1 , "\nStarting to translate the map from human readable disk format into game-engine format.");

  for (row = 0; row < ydim  ; row++)
    {

      //--------------------
      // Now a strange thing is going on here:  The floor tile information, stored
      // as text is already in the 'map' poitner, that is NORMALLY SOMETHING 
      // COMPLETELY DIFFERENT than a text pointer.  But the information is there
      // we need to convert it into real map information with proper struct...
      // Finally the proper struct can then replace the old map pointer.
      //
      Buffer = MyMalloc( sizeof ( map_tile ) * ( xdim + 10 ) );
      for ( col = 0 ; col < xdim  ; col ++ )
	{
	  sscanf( ( ( (char*)(Lev->map[row]) ) + 4 * col) , "%04d " , &tmp);
	  Buffer [ col ] . floor_value = (Uint16) tmp;
	  for ( glue_index = 0 ; glue_index < MAX_OBSTACLES_GLUED_TO_ONE_MAP_TILE ; glue_index ++ )
	    {
	      Buffer [ col ] . obstacles_glued_to_here [ glue_index ] = ( -1 ) ;
	    }
	}

      //--------------------
      // Now the old text pointer can be replaced with a pointer to the 
      // correctly assembled struct...
      //
      Lev -> map [ row ] = Buffer;

    }				/* for (row=0..) */

  DebugPrintf (2, "\nint decode_floor_tiles_of_this_level (Level Lev): end of function reached.");

  return OK;
}; // int decode_floor_tiles_of_this_level ( Level lev )

/* ----------------------------------------------------------------------
 * Now that we plan not to use hard-coded and explicitly human written 
 * coordinates any more, we need to use some labels instead.  But there
 * should be a function to conveniently resolve a given label within a
 * given map.  That's what this function is supposed to do.
 * ---------------------------------------------------------------------- */
void
ResolveMapLabelOnLevel ( char* MapLabel , location* PositionPointer , int LevelNum )
{
  Level ResolveLevel = curShip . AllLevels [ LevelNum ] ;
  int i;
  
  for ( i = 0 ; i < MAX_MAP_LABELS_PER_LEVEL ; i ++ )
    {
      if ( ResolveLevel->labels [ i ] . pos . x == (-1) ) continue;
      
      if ( !strcmp ( ResolveLevel->labels [ i ] . label_name , MapLabel ) )
	{
	  PositionPointer->x = ResolveLevel->labels [ i ] . pos . x + 0.5 ;
	  PositionPointer->y = ResolveLevel->labels [ i ] . pos . y + 0.5 ;
	  PositionPointer->level = LevelNum ;
	  DebugPrintf ( 1 , "\nResolving map label '%s' succeeded: pos.x=%d, pos.y=%d, pos.z=%d." ,
			MapLabel , PositionPointer->x , PositionPointer->y , PositionPointer->level );
	  return;
	}
    }

  PositionPointer->x = -1;
  PositionPointer->y = -1;
  PositionPointer->level = -1 ;
  DebugPrintf ( 1 , "\nResolving map label '%s' failed on level %d." ,
		MapLabel , LevelNum );
}; // void ResolveMapLabel ( char* MapLabel , grob_point* PositionPointer )

/* ----------------------------------------------------------------------
 * This is the ultimate function to resolve a given label within a
 * given SHIP.
 * ---------------------------------------------------------------------- */
void
ResolveMapLabelOnShip ( char* MapLabel , location* PositionPointer )
{
  int i ;

  //--------------------
  // We empty the given target pointer, so that we can tell
  // a successful resolve later...
  //
  PositionPointer->x = -1;
  PositionPointer->y = -1;
  
  //--------------------
  // Now we check each level of the ship, if it maybe contains this
  // label...
  //
  for ( i = 0 ; i < curShip.num_levels ; i ++ )
    {
      ResolveMapLabelOnLevel ( MapLabel , PositionPointer , i );
      
      if ( PositionPointer->x != ( -1 ) ) return;
    }

  fprintf ( stderr, "\n\nMapLabel: '%s'.\n" , MapLabel );
  GiveStandardErrorMessage ( "ResolveMapLabelOnShip(...)" , "\
Resolving a certain map label failed on the complete ship!\n\
This is a severe error in the game data of Freedroid.",
			     PLEASE_INFORM, IS_FATAL );

}; // void ResolveMapLabelOnShip ( char* MapLabel , grob_point* PositionPointer , int LevelNum )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void 
DecodeInterfaceDataForThisLevel ( Level loadlevel , char* DataPointer )
{
  char* TempSectionPointer;
  char PreservedLetter;

  //--------------------
  // Now we read in the jump points associated with this map
  //

  // We look for the beginning and end of the map statement section
  TempSectionPointer = LocateStringInData( DataPointer , MAP_BEGIN_STRING );

  // We add a terminator at the end, but ONLY TEMPORARY.  The damage will be restored later!
  PreservedLetter=TempSectionPointer[0];
  TempSectionPointer[0]=0;

#define DEBUG_LEVEL_INTERFACES 1

  ReadValueFromString( DataPointer , "jump threshold north: " , "%d" , 
		       &(loadlevel->jump_threshold_north) , TempSectionPointer );
  DebugPrintf( DEBUG_LEVEL_INTERFACES , "\nSuccessfully read jump theshold north : %d ", loadlevel->jump_threshold_north );
  ReadValueFromString( DataPointer , "jump threshold south: " , "%d" , 
		       &(loadlevel->jump_threshold_south) , TempSectionPointer );
  DebugPrintf( DEBUG_LEVEL_INTERFACES , "\nSuccessfully read jump theshold south : %d ", loadlevel->jump_threshold_south );
  ReadValueFromString( DataPointer , "jump threshold east: " , "%d" , 
		       &(loadlevel->jump_threshold_east) , TempSectionPointer );
  DebugPrintf( DEBUG_LEVEL_INTERFACES , "\nSuccessfully read jump theshold east : %d ", loadlevel->jump_threshold_east );
  ReadValueFromString( DataPointer , "jump threshold west: " , "%d" , 
		       &(loadlevel->jump_threshold_west) , TempSectionPointer );
  DebugPrintf( DEBUG_LEVEL_INTERFACES , "\nSuccessfully read jump theshold west : %d ", loadlevel->jump_threshold_west );

  ReadValueFromString( DataPointer , "jump target north: " , "%d" , 
		       &(loadlevel->jump_target_north) , TempSectionPointer );
  DebugPrintf( DEBUG_LEVEL_INTERFACES , "\nSuccessfully read jump target north : %d ", loadlevel->jump_target_north );
  ReadValueFromString( DataPointer , "jump target south: " , "%d" , 
		       &(loadlevel->jump_target_south) , TempSectionPointer );
  DebugPrintf( DEBUG_LEVEL_INTERFACES , "\nSuccessfully read jump target south : %d ", loadlevel->jump_target_south );
  ReadValueFromString( DataPointer , "jump target east: " , "%d" , 
		       &(loadlevel->jump_target_east) , TempSectionPointer );
  DebugPrintf( DEBUG_LEVEL_INTERFACES , "\nSuccessfully read jump target east : %d ", loadlevel->jump_target_east );
  ReadValueFromString( DataPointer , "jump target west: " , "%d" , 
		       &(loadlevel->jump_target_west) , TempSectionPointer );
  DebugPrintf( DEBUG_LEVEL_INTERFACES , "\nSuccessfully read jump target west : %d ", loadlevel->jump_target_west );


  TempSectionPointer [ 0 ] = PreservedLetter ;

}; // void DecodeInterfaceDataForThisLevel ( Level loadlevel , char* data )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void DecodeDimensionsOfThisLevel ( Level loadlevel , char* DataPointer )
{
  sscanf ( DataPointer , "Levelnumber: %u \n\
 xlen of this level: %u \n\
 ylen of this level: %u \n\
 light radius bonus of this level: %u \n" , 
	  &(loadlevel->levelnum), &(loadlevel->xlen),
	  &(loadlevel->ylen), &( loadlevel -> light_radius_bonus ));

  DebugPrintf( 2 , "\nLevelnumber : %d ", loadlevel->levelnum );
  DebugPrintf( 2 , "\nxlen of this level: %d ", loadlevel->xlen );
  DebugPrintf( 2 , "\nylen of this level: %d ", loadlevel->ylen );
  DebugPrintf( 2 , "\ncolor of this level: %d ", loadlevel->ylen );

  if ( loadlevel->ylen >= MAX_MAP_LINES ) 
    {
      GiveStandardErrorMessage ( "DecodeDimensionsOfThisLevel(...)" , "\
A maplevel Freedroid was supposed to load has more map lines than allowed\n\
for a map level as by the constant MAX_MAP_LINES in defs.h.\n\
Sorry, but unless this constant is raised, Freedroid will refuse to load this map.",
				 PLEASE_INFORM, IS_FATAL );
    }
}; // void DecodeDimensionsOfThisLevel ( Level loadlevel , char* DataPointer );

/* ----------------------------------------------------------------------
 * Next we extract the statments of the influencer on this level WITHOUT 
 * destroying or damaging the data in the process!
 * ---------------------------------------------------------------------- */
void 
DecodeStatementsOfThisLevel ( Level loadlevel , char* DataPointer )
{
  char PreservedLetter;
  int i , NumberOfStatementsInThisLevel;
  char* StatementSectionBegin;
  char* StatementSectionEnd;
  char* StatementPointer;
  
  //--------------------
  // First we initialize the statement array with 'empty' values
  //
  for ( i = 0 ; i < MAX_STATEMENTS_PER_LEVEL ; i ++ )
    {
      loadlevel->StatementList[ i ].x = ( -1 ) ;
      loadlevel->StatementList[ i ].y = ( -1 ) ;
      loadlevel->StatementList[ i ].Statement_Text = "No Statement loaded." ;
    }

  // We look for the beginning and end of the map statement section
  StatementSectionBegin = LocateStringInData( DataPointer , STATEMENT_BEGIN_STRING );
  StatementSectionEnd = LocateStringInData( DataPointer , STATEMENT_END_STRING );

  // We add a terminator at the end, but ONLY TEMPORARY.  The damage will be restored later!
  PreservedLetter=StatementSectionEnd[0];
  StatementSectionEnd[0]=0;
  NumberOfStatementsInThisLevel = CountStringOccurences ( StatementSectionBegin , STATEMENT_ITSELF_ANNOUNCE_STRING ) ;
  DebugPrintf( 1 , "\nNumber of statements found in this level : %d." , NumberOfStatementsInThisLevel );

  StatementPointer=StatementSectionBegin;
  for ( i = 0 ; i < NumberOfStatementsInThisLevel ; i ++ )
    {
      StatementPointer = strstr ( StatementPointer + 1 , X_POSITION_OF_STATEMENT_STRING );
      ReadValueFromString( StatementPointer , X_POSITION_OF_STATEMENT_STRING , "%d" , 
			   &(loadlevel->StatementList[ i ].x) , StatementSectionEnd );
      ReadValueFromString( StatementPointer , Y_POSITION_OF_STATEMENT_STRING , "%d" , 
			   &(loadlevel->StatementList[ i ].y) , StatementSectionEnd );
      loadlevel->StatementList[ i ].Statement_Text = 
	ReadAndMallocStringFromData ( StatementPointer , STATEMENT_ITSELF_ANNOUNCE_STRING , "\"" ) ;

      DebugPrintf( 1 , "\nPosX=%d PosY=%d Statement=\"%s\"" , loadlevel->StatementList[ i ].x , 
		   loadlevel->StatementList[ i ].y , loadlevel->StatementList[ i ].Statement_Text );
    }

  // Now we repair the damage done to the loaded level data
  StatementSectionEnd[0]=PreservedLetter;

}; // void DecodeStatementsOfThisLevel ( Level loadlevel , char* DataPointer );

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
decode_obstacles_of_this_level ( Level loadlevel , char* DataPointer )
{
  int i;
  char PreservedLetter;
  char* obstacle_Pointer;
  char* obstacle_SectionBegin;
  char* obstacle_SectionEnd;
  int NumberOfobstacle_sInThisLevel;

  //--------------------
  // First we initialize the obstacles with 'empty' information
  //
  for ( i = 0 ; i < MAX_OBSTACLES_ON_MAP ; i ++ )
    {
      loadlevel -> obstacle_list [ i ] . type = ( -1 ) ;
      loadlevel -> obstacle_list [ i ] . pos . x = ( -1 ) ;
      loadlevel -> obstacle_list [ i ] . pos . y = ( -1 ) ;
      loadlevel -> obstacle_list [ i ] . name_index = ( -1 ) ;
    }

  //--------------------
  // Now we look for the beginning and end of the obstacle section
  //
  obstacle_SectionBegin = LocateStringInData( DataPointer , OBSTACLE_DATA_BEGIN_STRING );
  obstacle_SectionEnd = LocateStringInData( DataPointer , OBSTACLE_DATA_END_STRING );

  //--------------------
  // We add a terminator at the end, but ONLY TEMPORARY.  The damage will be restored later!
  //
  PreservedLetter=obstacle_SectionEnd[0];
  obstacle_SectionEnd[0]=0;
  NumberOfobstacle_sInThisLevel = CountStringOccurences ( obstacle_SectionBegin , OBSTACLE_TYPE_STRING ) ;
  DebugPrintf( 1 , "\nNumber of obstacles found in this level : %d." , NumberOfobstacle_sInThisLevel );

  //--------------------
  // Now we decode all the obstacle information
  //
  obstacle_Pointer=obstacle_SectionBegin;
  for ( i = 0 ; i < NumberOfobstacle_sInThisLevel ; i ++ )
    {
      obstacle_Pointer = strstr ( obstacle_Pointer + 1 , OBSTACLE_TYPE_STRING );
      ReadValueFromString( obstacle_Pointer , OBSTACLE_TYPE_STRING , "%d" , 
			   & ( loadlevel -> obstacle_list [ i ] . type ) , obstacle_SectionEnd );
      ReadValueFromString( obstacle_Pointer , OBSTACLE_X_POSITION_STRING , "%f" , 
			   & ( loadlevel -> obstacle_list [ i ] . pos . x ) , obstacle_SectionEnd );
      ReadValueFromString( obstacle_Pointer , OBSTACLE_Y_POSITION_STRING , "%f" , 
			   & ( loadlevel -> obstacle_list [ i ] . pos . y ) , obstacle_SectionEnd );
      ReadValueFromString( obstacle_Pointer , OBSTACLE_LABEL_INDEX_STRING , "%d" , 
			   & ( loadlevel -> obstacle_list [ i ] . name_index ) , obstacle_SectionEnd );

      // DebugPrintf( 0 , "\nobtacle_type=%d pos.x=%3.2f pos.y=%3.2f" , loadlevel -> obstacle_list [ i ] . type , 
      // loadlevel -> obstacle_list [ i ] . pos . x , loadlevel-> obstacle_list [ i ] . pos . y );
    }

  //--------------------
  // Now we repair the damage done to the loaded level data
  //
  obstacle_SectionEnd [ 0 ] = PreservedLetter;
  
}; // void decode_obstacles_of_this_level ( loadlevel , DataPointer )

/* ----------------------------------------------------------------------
 * Next we extract the map labels of this level WITHOUT destroying
 * or damaging the data in the process!
 * ---------------------------------------------------------------------- */
void 
DecodeMapLabelsOfThisLevel ( Level loadlevel , char* DataPointer )
{
  int i;
  char PreservedLetter;
  char* MapLabelPointer;
  char* MapLabelSectionBegin;
  char* MapLabelSectionEnd;
  int NumberOfMapLabelsInThisLevel;

  //--------------------
  // First we initialize the map labels array with 'empty' information
  //
  for ( i = 0 ; i < MAX_MAP_LABELS_PER_LEVEL ; i ++ )
    {
      loadlevel -> labels [ i ] . pos . x = ( -1 ) ;
      loadlevel -> labels [ i ] . pos . y = ( -1 ) ;
      loadlevel -> labels [ i ] . label_name = "no_label_defined" ;
    }

  //--------------------
  // Now we look for the beginning and end of the map labels section
  //
  MapLabelSectionBegin = LocateStringInData( DataPointer , MAP_LABEL_BEGIN_STRING );
  MapLabelSectionEnd = LocateStringInData( DataPointer , MAP_LABEL_END_STRING );

  //--------------------
  // We add a terminator at the end, but ONLY TEMPORARY.  The damage will be restored later!
  //
  PreservedLetter=MapLabelSectionEnd[0];
  MapLabelSectionEnd[0]=0;
  NumberOfMapLabelsInThisLevel = CountStringOccurences ( MapLabelSectionBegin , LABEL_ITSELF_ANNOUNCE_STRING ) ;
  DebugPrintf( 1 , "\nNumber of map labels found in this level : %d." , NumberOfMapLabelsInThisLevel );

  //--------------------
  // Now we decode all the map label information
  //
  MapLabelPointer=MapLabelSectionBegin;
  for ( i = 0 ; i < NumberOfMapLabelsInThisLevel ; i ++ )
    {
      MapLabelPointer = strstr ( MapLabelPointer + 1 , X_POSITION_OF_LABEL_STRING );
      ReadValueFromString( MapLabelPointer , X_POSITION_OF_LABEL_STRING , "%d" , 
			   &(loadlevel->labels[ i ].pos.x) , MapLabelSectionEnd );
      ReadValueFromString( MapLabelPointer , Y_POSITION_OF_LABEL_STRING , "%d" , 
			   &(loadlevel->labels[ i ].pos.y) , MapLabelSectionEnd );
      loadlevel->labels[ i ].label_name = 
	ReadAndMallocStringFromData ( MapLabelPointer , LABEL_ITSELF_ANNOUNCE_STRING , "\"" ) ;

      DebugPrintf( 1 , "\npos.x=%d pos.y=%d label_name=\"%s\"" , loadlevel->labels[ i ].pos.x , 
		   loadlevel->labels[ i ].pos.y , loadlevel->labels[ i ].label_name );
    }

  //--------------------
  // Now we repair the damage done to the loaded level data
  //
  MapLabelSectionEnd[0]=PreservedLetter;

}; // void DecodeMapLabelsOfThisLevel ( Level loadlevel , char* DataPointer );

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
decode_obstacle_names_of_this_level ( Level loadlevel , char* DataPointer )
{
  int i;
  char PreservedLetter;
  char* obstacle_namePointer;
  char* obstacle_nameSectionBegin;
  char* obstacle_nameSectionEnd;
  int NumberOfobstacle_namesInThisLevel;
  int target_index;

  //--------------------
  // At first we set all the obstacle name pointers to NULL in order to
  // mark them as unused.
  //
  for ( i = 0 ; i < MAX_OBSTACLE_NAMES_PER_LEVEL ; i ++ )
    {
      loadlevel -> obstacle_name_list [ i ] = NULL ;
    }

  //--------------------
  // Now we look for the beginning and end of the map labels section
  //
  obstacle_nameSectionBegin = LocateStringInData( DataPointer , OBSTACLE_LABEL_BEGIN_STRING );
  obstacle_nameSectionEnd = LocateStringInData( DataPointer , OBSTACLE_LABEL_END_STRING );

  //--------------------
  // We add a terminator at the end, but ONLY TEMPORARY.  The damage will be restored later!
  //
  PreservedLetter=obstacle_nameSectionEnd[0];
  obstacle_nameSectionEnd[0]=0;
  NumberOfobstacle_namesInThisLevel = CountStringOccurences ( obstacle_nameSectionBegin , OBSTACLE_LABEL_ANNOUNCE_STRING ) ;
  DebugPrintf( 1 , "\nNumber of obstacle labels found in this level : %d." , NumberOfobstacle_namesInThisLevel );

  //--------------------
  // Now we decode all the map label information
  //
  obstacle_namePointer=obstacle_nameSectionBegin;
  for ( i = 0 ; i < NumberOfobstacle_namesInThisLevel ; i ++ )
    {
      obstacle_namePointer = strstr ( obstacle_namePointer + 1 , INDEX_OF_OBSTACLE_NAME );
      ReadValueFromString( obstacle_namePointer , INDEX_OF_OBSTACLE_NAME , "%d" , 
			   &(target_index) , obstacle_nameSectionEnd );

      loadlevel -> obstacle_name_list [ target_index ] = 
	ReadAndMallocStringFromData ( obstacle_namePointer , OBSTACLE_LABEL_ANNOUNCE_STRING , "\"" ) ;

      DebugPrintf( 1 , "\nobstacle_name_index=%d obstacle_label_name=\"%s\"" , target_index ,
		   loadlevel -> obstacle_name_list [ target_index ] );
    }

  //--------------------
  // Now we repair the damage done to the loaded level data
  //
  obstacle_nameSectionEnd [ 0 ] = PreservedLetter;
  

}; // void decode_obstacle_names_of_this_level ( loadlevel , DataPointer )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
glue_obstacles_to_floor_tiles_for_level ( int level_num )
{
  level* loadlevel = curShip . AllLevels [ level_num ] ;
  int obstacle_counter = 2 ;
  int x_tile;
  int y_tile;
  int glue_index;
  int next_free_index;

  //--------------------
  // We clean out any obstacle glue information that might be still
  // in this level.
  //
  for ( x_tile = 0 ; x_tile < loadlevel -> xlen ; x_tile ++ )
    {
      for ( y_tile = 0 ; y_tile < loadlevel -> ylen ; y_tile ++ )
	{
	  for ( glue_index = 0 ; glue_index < MAX_OBSTACLES_GLUED_TO_ONE_MAP_TILE ; glue_index ++ )
	    {
	      loadlevel -> map [ y_tile ] [ x_tile ] . obstacles_glued_to_here [ glue_index ] = (-1) ;
	    }
	}
    }  

  //--------------------
  // Each obstacles must to be anchored to exactly one (the closest!)
  // map tile, so that we can find out obstacles 'close' to somewhere
  // more easily...
  //
  for ( obstacle_counter = 0 ; obstacle_counter < MAX_OBSTACLES_ON_MAP ; obstacle_counter  ++ )
    {
      //--------------------
      // Maybe we're done here already...?
      //
      if ( loadlevel -> obstacle_list [ obstacle_counter ] . type <= (-1) ) break;

      //--------------------
      // We need to glue this one and we glue it to the closest map tile center we have...
      // For this we need first to prepare some things...
      //
      x_tile = rintf ( loadlevel -> obstacle_list [ obstacle_counter ] . pos . x - 0.5 );
      y_tile = rintf ( loadlevel -> obstacle_list [ obstacle_counter ] . pos . y - 0.5 );

      if ( x_tile < 0 ) x_tile = 0;       if ( y_tile < 0 ) y_tile = 0 ;
      if ( x_tile >= loadlevel -> xlen ) x_tile = loadlevel -> xlen - 1;
      if ( y_tile >= loadlevel -> ylen ) y_tile = loadlevel -> ylen - 1;

      next_free_index = MAX_OBSTACLES_GLUED_TO_ONE_MAP_TILE ;
      for ( glue_index = 0 ; glue_index < MAX_OBSTACLES_GLUED_TO_ONE_MAP_TILE ; glue_index ++ )
	{
	  if ( loadlevel -> map [ y_tile ] [ x_tile ] . obstacles_glued_to_here [ glue_index ] != (-1) )
	    {
	      // DebugPrintf ( 0 , "\nHey, someone's already sitting here... moving to next index...: %d." ,
	      // glue_index + 1 );
	    }
	  else
	    {
	      next_free_index = glue_index ;
	      break;
	    }
	}

      //--------------------
      // some safety check against writing beyond the bonds of the
      // array.
      //
      if ( next_free_index >= MAX_OBSTACLES_GLUED_TO_ONE_MAP_TILE )
	{
	  /*
	    We disable this VERY FREQUENT warning now...
	    
	  DebugPrintf ( 0 , "The position where the problem occured is: x_tile=%d, y_tile=%d." , x_tile , y_tile );
	  GiveStandardErrorMessage ( "glue_obstacles_to_floor_tiles_for_level (...)" , "\
FreedroidRPG was unable to glue a certain obstacle to the nearest map tile.\n\
This bug can be resolved by simply raising a contant by one, but it needs to be done :)",
				     PLEASE_INFORM, IS_WARNING_ONLY );
	  */
	  continue ;
	}

      //--------------------
      // Now it can be glued...
      //
      loadlevel -> map [ y_tile ] [ x_tile ] . obstacles_glued_to_here [ next_free_index ] =
	obstacle_counter ; 

    }

}; // glue_obstacles_to_floor_tiles_for_level ( int level_num )

/* ----------------------------------------------------------------------
 * This function collects the automap data and stores it in the Me data
 * structure.
 * ---------------------------------------------------------------------- */
void
CollectAutomapData ( void )
{
  int x , y ;
  int start_x, start_y, end_x, end_y;
  gps ObjPos;
  static int TimePassed;
  Level automap_level = curShip . AllLevels [ Me [ 0 ] . pos . z ] ;
  int i;
  Obstacle our_obstacle;
  int level = Me [ 0 ] . pos . z ;

  ObjPos . z = Me [ 0 ] . pos . z ;

  //--------------------
  // Checking the whole map for passablility will surely take a lot
  // of computation.  Therefore we only do this once every second of
  // real time.
  //
  if ( TimePassed == (int) Me[0].MissionTimeElapsed ) return;
  TimePassed = (int) Me[0].MissionTimeElapsed;

  // DebugPrintf ( -3 , "\nCollecting Automap data... " );

  //--------------------
  // Also if there is no map-maker present in inventory, then we need not
  // do a thing here...
  //
  if ( ! CountItemtypeInInventory( ITEM_MAP_MAKER_SIMPLE , 0 ) ) return;

  //--------------------
  // Earlier we had
  //
  // start_x = 0 ; start_y = 0 ; end_x = automap_level->xlen ; end_y = automap_level->ylen ;
  //
  // when maximal automap was generated.  Now we only add to the automap what really is on screen...
  //
  start_x = Me [ 0 ] . pos . x - 7 ; 
  end_x = Me [ 0 ] . pos . x + 7 ; 
  start_y = Me [ 0 ] . pos . y - 7 ; 
  end_y = Me [ 0 ] . pos . y + 7 ; 

  if ( start_x < 0 ) start_x = 0 ; 
  if ( end_x >= automap_level->xlen ) end_x = automap_level->xlen-1 ;
  if ( start_y < 0 ) start_y = 0 ; 
  if ( end_y >= automap_level->ylen ) end_y = automap_level->ylen-1 ;

  //--------------------
  // Now we do the actual checking for visible wall components.
  //
  for ( y = start_y ; y < end_y ; y ++ )
    {
      for ( x = start_x ; x < end_x ; x ++ )
	{

	  for ( i = 0 ; i < MAX_OBSTACLES_GLUED_TO_ONE_MAP_TILE ; i ++ )
	    {
	      if ( automap_level -> map [ y ] [ x ] . obstacles_glued_to_here [ i ] == (-1) ) continue;

	      our_obstacle = & ( automap_level -> obstacle_list [ automap_level -> map [ y ] [ x ] . obstacles_glued_to_here [ i ] ] ) ;
	      if ( obstacle_map [ our_obstacle -> type ] . block_area_type == COLLISION_TYPE_RECTANGLE )
		{
		  Me [ 0 ] . Automap [ level ] [ y ] [ x ] = Me [ 0 ] . Automap [ level ] [ y ] [ x ] | RIGHT_WALL_BIT ;
		  Me [ 0 ] . Automap [ level ] [ y ] [ x ] = Me [ 0 ] . Automap [ level ] [ y ] [ x ] | LEFT_WALL_BIT ;
		}
	    }
	  /*
	  if ( IsWallBlock( automap_level->map[y][x]  . floor_value ) ) 
	    {
	      //--------------------
	      // First we check, if there are some right sides of walls visible
	      //
	      ObjPos.x = x + 0.75;
	      ObjPos.y = y + 0;
	      if ( IsVisible ( &ObjPos , 0 ) ) 
		{
		  Me [ 0 ] . Automap[level][y][x] = Me [ 0 ] . Automap[level][y][x] | RIGHT_WALL_BIT ;
		}
	      //--------------------
	      // Now we check, if there are some left sides of walls visible
	      //
	      ObjPos.x = x - 0.75;
	      ObjPos.y = y + 0;
	      if ( IsVisible ( &ObjPos , 0 ) )
		{
		  Me [ 0 ] . Automap[level][y][x] = Me [ 0 ] . Automap[level][y][x] | LEFT_WALL_BIT ;
		}
	      //--------------------
	      // Now we check, if there are some southern sides of walls visible
	      //
	      ObjPos.x = x + 0;
	      ObjPos.y = y + 0.75;
	      if ( IsVisible ( &ObjPos , 0 ) ) 
		{
		  Me [ 0 ] . Automap[level][y][x] = Me [ 0 ] . Automap[level][y][x] | DOWN_WALL_BIT ;
		}
	      //--------------------
	      // Now we check, if there are some northern sides of walls visible
	      //
	      ObjPos.x = x + 0.0 ;
	      ObjPos.y = y - 0.75 ;
	      if ( IsVisible ( &ObjPos , 0 ) ) 
		{
		  Me [ 0 ] . Automap[level][y][x] = Me [ 0 ] . Automap[level][y][x] | UP_WALL_BIT ;
		}
	    }
	  */
	}
    }

}; // void CollectAutomapData ( void )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
int
smash_obstacles_only_on_tile ( float x , float y , int map_x , int map_y )
{
  Level BoxLevel = curShip . AllLevels [ Me [ 0 ] . pos . z ] ;
  int i ;
  Obstacle target_obstacle;
  int smashed_something = FALSE ;
  moderately_finepoint blast_start_pos;

  //--------------------
  // First some security checks against touching the outsides of the map...
  //
  if ( ( map_x < 0 ) || ( map_y < 0 ) || ( map_x >= BoxLevel -> xlen ) || ( map_y >= BoxLevel -> ylen ) )
    return ( FALSE ) ;

  //--------------------
  // We check all the obstacles on this square if they are maybe destructable
  // and if they are, we destruct them, haha
  //
  for ( i = 0 ; i < MAX_OBSTACLES_GLUED_TO_ONE_MAP_TILE ; i ++ )
    {
      //--------------------
      // First we see if there is something glued to this map tile at all.
      //
      if ( BoxLevel -> map [ map_y ] [ map_x ] . obstacles_glued_to_here [ i ] == (-1) ) continue;

      target_obstacle = & ( BoxLevel -> obstacle_list [ BoxLevel -> map [ map_y ] [ map_x ] . 
							obstacles_glued_to_here [ i ] ] );

      if ( ! obstacle_map [ target_obstacle -> type ] . is_smashable ) continue;

      //--------------------
      // Now we check if the item really was close enough to the strike target.
      // A range of 0.5 should do.
      //
      if ( fabsf ( x - target_obstacle -> pos . x ) > 0.4 ) continue ;
      if ( fabsf ( y - target_obstacle -> pos . y ) > 0.4 ) continue ;

      DebugPrintf ( 1 , "\nObject smashed at: (%f/%f) by hit/explosion at (%f/%f)." ,
		    target_obstacle -> pos . x , target_obstacle -> pos . y ,
		    x , y );

      smashed_something = TRUE ;

      //--------------------
      // Before we destroy the obstacle (and lose the obstacle type) we see if we
      // should maybe drop some item.
      //
      if ( obstacle_map [ target_obstacle -> type ] . drop_random_treasure )
	DropRandomItem( target_obstacle -> pos . x , target_obstacle -> pos . y , 1 , FALSE , FALSE , FALSE );

      //--------------------
      // Since the obstacle is destroyed, we start a blast at it's position.
      // But here a WARNING WARNING WARNING! is due!  We must not start the
      // blast before the obstacle is removed, because the blast will again
      // cause this very obstacle removal function, so we need to be careful
      // so as not to incide endless recursion.  We memorize the position for
      // later, then delete the obstacle, then we start the blast.
      //
      blast_start_pos . x = target_obstacle -> pos . x ;
      blast_start_pos . y = target_obstacle -> pos . y ;

      //--------------------
      // Now we delete the obstacle in question.  For this we got a standard function to
      // safely do it and not make some errors into the glue structure or obstacles lists...
      //
      delete_obstacle ( BoxLevel , target_obstacle );

      //--------------------
      // Now that the obstacle is removed AND ONLY NOW that the obstacle is
      // removed, we may start a blast at this position.  Otherwise we would
      // run into trouble, see the warning further above.
      //
      StartBlast( blast_start_pos . x , blast_start_pos . y , BoxLevel->levelnum , DRUIDBLAST );

      /*
      target_obstacle -> type = ( -1 ) ;
      BoxLevel -> map [ map_y ] [ map_x ] . obstacles_glued_to_here [ i ] = (-1) ;

      //--------------------
      // Maybe there are other obstacles glued to here.  Then we need to fill the
      // gap in the glue array that we have just created.
      //
      for ( j = i ; j < MAX_OBSTACLES_GLUED_TO_ONE_MAP_TILE - 1 ; j ++ )
	{
	  if ( BoxLevel -> map [ map_y ] [ map_x ] . obstacles_glued_to_here [ j + 1 ] != (-1) )
	    {
	      BoxLevel -> map [ map_y ] [ map_x ] . obstacles_glued_to_here [ j ] = 
		BoxLevel -> map [ map_y ] [ map_x ] . obstacles_glued_to_here [ j + 1 ] ;
	      BoxLevel -> map [ map_y ] [ map_x ] . obstacles_glued_to_here [ j + 1 ] = (-1) ;
	    }
	}
      */

    }

  return ( smashed_something );

}; // int smash_obstacles_only_on_tile ( float x , float y , int map_x , int map_y )

/* ----------------------------------------------------------------------
 * When a destructable type of obstacle gets hit, 
 * e.g. by a blast exploding on the
 * tile or a influencer melee hit on the tile, then the barrel explodes,
 * possibly leaving some goods behind.
 * ---------------------------------------------------------------------- */
int 
smash_obstacle ( float x , float y )
{
  int map_x, map_y;
  int smash_x, smash_y ;
  int smashed_something = FALSE;

  map_x=(int)rintf(x);
  map_y=(int)rintf(y);

  for ( smash_x = map_x - 1 ; smash_x < map_x + 2 ; smash_x ++ )
    {
      for ( smash_y = map_y - 1 ; smash_y < map_y + 2 ; smash_y ++ )
	{
	  if ( smash_obstacles_only_on_tile ( x , y , smash_x , smash_y ) ) 
	    smashed_something = TRUE ;
	}
    }

  return ( smashed_something );

}; // int smash_obstacle ( float x , float y );

/* ----------------------------------------------------------------------
 * This function returns the map brick code of the tile that occupies the
 * given position.
 * ---------------------------------------------------------------------- */
Uint16
GetMapBrick (Level deck, float x, float y)
{
  Uint16 BrickWanted;
  int RoundX, RoundY;
  
  /* 
   * ATTENTION! BE CAREFUL HERE!  What we want is an integer division with rest, 
   * not an exact foating point division!  Beware of "improvements" here!!!
   */
  RoundX = (int) rintf (x) ;
  RoundY = (int) rintf (y) ;

  if ( RoundY >= deck->ylen)
    {
      // printf ("\n----------------------------------------------------------------------\nunsigned char GetMapBrick(Level deck, float x, float y): Error:\n BlockPosition from outside requested: y>ylen\n----------------------------------------------------------------------\n");
      return ISO_COMPLETELY_DARK;
      Terminate (-1);
    }
  if ( RoundX >= deck->xlen)
    {
      // printf ("\n----------------------------------------------------------------------\nunsigned char GetMapBrick(Level deck, float x, float y): Error:\n BlockPosition from outside requested: x>xlen\n----------------------------------------------------------------------\n");
      return ISO_COMPLETELY_DARK;
      Terminate (-1);
    }
  if ( RoundY < 0)
    {
      // printf ("\n----------------------------------------------------------------------\nunsigned char GetMapBrick(Level deck, float x, float y): Error:\n BlockPosition from outside requested: y<0\n----------------------------------------------------------------------\n");
      return ISO_COMPLETELY_DARK;
      Terminate (-1);
    }
  if ( RoundX < 0)
    {
      // printf ("\n----------------------------------------------------------------------\nunsigned char GetMapBrick(Level deck, float x, float y): Error:\n BlockPosition from outside requested: x<0\n----------------------------------------------------------------------\n");
      return ISO_COMPLETELY_DARK;
      Terminate (-1);
    }

  BrickWanted = deck -> map[ RoundY ][ RoundX ] . floor_value ;
  if ( BrickWanted >= ALL_ISOMETRIC_FLOOR_TILES )
    {
      fprintf( stderr , "\nBrickWanted: %d at pos X=%d Y=%d Z=%d." , BrickWanted , RoundX , RoundY , deck->levelnum );
      GiveStandardErrorMessage ( "GetMapBrick(...)" , "\
A maplevel in Freedroid contained a brick type, that does not have a\n\
real graphical representation.  This is a severe error, that really \n\
shouldn't be occuring in normal game, except perhaps if the level editor\n\
was just used to add/remove some new doors or refreshes or other animated\n\
map tiles.",
				 PLEASE_INFORM, IS_FATAL );
    }

  return BrickWanted;

}; // int GetMapBrick( ... ) 
 
/* ---------------------------------------------------------------------- 
 * This function checks if something special has to be done, cause the
 * Influencer or tux has stepped on some special fields like a lifts or
 * a console or a refresh or something like that.
 * ---------------------------------------------------------------------- */
void
ActSpecialField ( int PlayerNum )
{
  unsigned char MapBrick;
  // float cx, cy;			
  float x = Me [ PlayerNum ] . pos . x ;
  float y = Me [ PlayerNum ] . pos . y ;
  Level SpecialFieldLevel;

  // DebugPrintf (2, "\nvoid ActSpecialField ( int PlayerNum  ) :  Real function call confirmed." ) ;

  //--------------------
  // We don't do anything for this player, if it's an
  // inactive player.
  //
  if ( Me [ PlayerNum ] . status == OUT ) return;

  //--------------------
  // If the player is on the one special teleporter square, that gets
  // the player teleported back to where he came from, we will teleport
  // him back there...
  //
  if ( ( ((int) rintf(Me[0].pos.x) ) == 3 ) &&
       ( ((int) rintf(Me[0].pos.y) ) == 7 ) &&
       ( ((int) rintf(Me[0].pos.z) ) == 0 ) )
    {
      Teleport ( (int) Me[0].teleport_anchor.z , (int) Me[0].teleport_anchor.x , (int) Me[0].teleport_anchor.y , 0 , FALSE , TRUE );
    }

  SpecialFieldLevel = curShip . AllLevels [ Me [ PlayerNum ] . pos . z ] ;

  MapBrick = GetMapBrick ( SpecialFieldLevel , x , y ) ;

  switch (MapBrick)
    {
    case CONSUMER_1:
    case CONSUMER_2:
    case CONSUMER_3:
    case CONSUMER_4:
      GameConfig.Inventory_Visible=FALSE;
      GameConfig.CharacterScreen_Visible=FALSE;
      GameConfig.Mission_Log_Visible=FALSE;
      GameOver=TRUE;
      PlayATitleFile ( "EndOfGame.title" );
      Credits_Menu();
      break;

    default:
      break;
    }	// switch 

  DebugPrintf (2, "\nvoid ActSpecialField(int x, int y):  end of function reached.");

}; // void ActSpecialField ( ... )

/* ----------------------------------------------------------------------
 * This function moves all the refresh fields to their next phase (if
 * it's time already).
 * ---------------------------------------------------------------------- */
void
AnimateRefresh (void)
{
  static float InnerWaitCounter = 0;
  int i;
  Level RefreshLevel = curShip.AllLevels [ Me [ 0 ] . pos . z ] ;

  DebugPrintf (2, "\nvoid AnimateRefresh(void):  real function call confirmed.");

  InnerWaitCounter += Frame_Time () * 3;

  for (i = 0; i < MAX_REFRESHES_ON_LEVEL; i++)
    {
      if ( RefreshLevel -> refresh_obstacle_indices [ i ] <= ( -1 ) ) break;

      switch ( RefreshLevel -> obstacle_list [ RefreshLevel -> refresh_obstacle_indices [ i ] ] . type )
	{
	case ISO_REFRESH_1:
	case ISO_REFRESH_2:
	case ISO_REFRESH_3:
	case ISO_REFRESH_4:
	case ISO_REFRESH_5:
	  //--------------------
	  // All is well :)
	  //
	  break;
	default:
	  // fprintf ( stderr, "\n*Pos: '%d'.\ni: %d\nPlayerNum: %d\nlevelnum: %d\nObstacle index: %d" , *Pos , i , PlayerNum , DoorLevel -> levelnum , door_obstacle_index );
	  GiveStandardErrorMessage ( "AnimateRefresh (...)" , "\
Error:  A refresh index pointing not to a refresh obstacles found.",
				     PLEASE_INFORM, IS_FATAL );
	  break;
	}

      RefreshLevel -> obstacle_list [ RefreshLevel -> refresh_obstacle_indices [ i ] ] . type = (((int) rintf (InnerWaitCounter)) % 5) + ISO_REFRESH_1;

    }	// for

  DebugPrintf (2, "\nvoid AnimateRefresh(void):  end of function reached.");

}; // void AnimateRefresh ( void )

/* ----------------------------------------------------------------------
 * This function moves all the teleporter fields to their next phase (if
 * it's time already).
 * ---------------------------------------------------------------------- */
void
AnimateTeleports (void)
{
  static float InnerWaitCounter = 0;
  int i;
  Level TeleportLevel = curShip.AllLevels [ Me [ 0 ] . pos . z ] ;

  DebugPrintf (2, "\nvoid AnimateTeleports(void):  real function call confirmed.");

  InnerWaitCounter += Frame_Time () * 10;

  for (i = 0; i < MAX_TELEPORTERS_ON_LEVEL; i++)
    {
      if ( TeleportLevel -> teleporter_obstacle_indices [ i ] <= ( -1 ) ) break;

      switch ( TeleportLevel -> obstacle_list [ TeleportLevel -> teleporter_obstacle_indices [ i ] ] . type )
	{
	case ISO_TELEPORTER_1:
	case ISO_TELEPORTER_2:
	case ISO_TELEPORTER_3:
	case ISO_TELEPORTER_4:
	case ISO_TELEPORTER_5:
	  //--------------------
	  // All is well :)
	  //
	  break;
	default:
	  // fprintf ( stderr, "\n*Pos: '%d'.\ni: %d\nPlayerNum: %d\nlevelnum: %d\nObstacle index: %d" , *Pos , i , PlayerNum , DoorLevel -> levelnum , door_obstacle_index );
	  GiveStandardErrorMessage ( "AnimateTeleport (...)" , "\
Error:  A teleporter index pointing not to a teleporter obstacle found.",
				     PLEASE_INFORM, IS_FATAL );
	  break;
	}

      TeleportLevel -> obstacle_list [ TeleportLevel -> teleporter_obstacle_indices [ i ] ] . type = (((int) rintf (InnerWaitCounter)) % 5) + ISO_TELEPORTER_1;

    }	// for

  DebugPrintf (2, "\nvoid AnimateTeleports(void):  end of function reached.");

}; // void AnimateTeleports ( void )

/* ----------------------------------------------------------------------
 * This function loads the data for a whole ship
 * Possible return values are : OK and ERR
 * ---------------------------------------------------------------------- */
int
LoadShip (char *filename)
{
  char *ShipData;
  char *endpt;				/* Pointer to end-strings */
  char *LevelStart[MAX_LEVELS];		/* Pointer to a level-start */
  int level_anz;
  int i;

#define END_OF_SHIP_DATA_STRING "*** End of Ship Data ***"

  //--------------------
  // Read the whole ship-data to memory 
  //
  ShipData = ReadAndMallocAndTerminateFile( filename , END_OF_SHIP_DATA_STRING ) ;

  //--------------------
  // Now we read the shipname information from the loaded data
  //
  curShip.AreaName = ReadAndMallocStringFromData ( ShipData , AREA_NAME_STRING , "\"" ) ;

  //--------------------
  // Now we count the number of levels and remember their start-addresses.
  // This is done by searching for the LEVEL_END_STRING again and again
  // until it is no longer found in the ship file.  good.
  //
  level_anz = 0;
  endpt = ShipData;
  LevelStart[level_anz] = ShipData;
  while ((endpt = strstr (endpt, LEVEL_END_STRING)) != NULL)
    {
      endpt += strlen (LEVEL_END_STRING);
      level_anz++;
      LevelStart[level_anz] = endpt + 1;
    }
  curShip . num_levels = level_anz;

  //--------------------
  // Now we can start to take apart the information about each level...
  //
  for (i = 0; i < curShip.num_levels; i++)
    {
      curShip . AllLevels [ i ] = DecodeLoadedLeveldata ( LevelStart [ i ] );

      decode_floor_tiles_of_this_level ( curShip . AllLevels [ i ] ) ;

      //--------------------
      // The level structure contains an array with the locations of all
      // doors that might have to be opened or closed during the game.  This
      // list is prepared in advance, so that we don't have to search for doors
      // on all of the map during program runtime.
      //
      // It requires, that the obstacles have been read in already.
      //
      GetAllAnimatedMapTiles ( curShip . AllLevels [ i ] );

      //--------------------
      // We attach each obstacle to a floor tile, just so that we can sort
      // out the obstacles 'close' more easily within an array of literally
      // thousands of obstacles...
      //
      glue_obstacles_to_floor_tiles_for_level ( i );

      ShowSaveLoadGameProgressMeter( (100*(i+1)) / level_anz , FALSE )  ;

    }

  //--------------------
  // Now that all the information has been copied, we can free the loaded data
  // again.
  //
  free ( ShipData );

  return OK;

}; // int LoadShip ( ... ) 

/* ----------------------------------------------------------------------
 * This function is intended to eliminate leading -1 entries before
 * real entries in the waypoint connection structure.
 *
 * Such leading -1 entries might cause problems later, because some
 * Enemy-Movement routines expect that the "real" entries are the
 * first entries in the connection list.
 * ---------------------------------------------------------------------- */
void CheckWaypointIntegrity(Level Lev)
{
  int i, j , k , l ;

  for ( i = 0 ; i < MAXWAYPOINTS ; i++ )
    {
      // Search for the first -1 entry:  j contains this number
      for ( j = 0 ; j < MAX_WP_CONNECTIONS ; j++ )
	{
	  if (Lev->AllWaypoints[i].connections[j] == -1 ) break;
	}

      // have only non-(-1)-entries?  then we needn't do anything.
      if ( j == MAX_WP_CONNECTIONS ) continue;
      // have only one (-1) entry in the last position?  then we needn't do anything.
      if ( j == MAX_WP_CONNECTIONS - 1 ) continue;
      
      // search for the next non-(-1)-entry AFTER the -1 entry fount first
      for ( k = j + 1 ; k < MAX_WP_CONNECTIONS ; k ++ )
	{
	  if (Lev->AllWaypoints[i].connections[k] != -1 ) break;
	}
      
      // none found? -- that would be good.  no corrections nescessary.  we can go.
      if ( k == MAX_WP_CONNECTIONS ) continue;

      // At this point we have found a non-(-1)-entry after a -1 entry.  that means work!!

      DebugPrintf( 0 , "\n WARNING!! INCONSISTENSY FOUND ON LEVEL %d!! " , Lev->levelnum );
      DebugPrintf( 0 , "\n NUMBER OF LEADING -1 ENTRIES: %d!! " , k-j );
      DebugPrintf( 0 , "\n COMPENSATION ACTIVATED..." );

      // So we move the later waypoints just right over the existing leading -1 entries

      for ( l = j ; l < MAX_WP_CONNECTIONS-(k-j) ; l++ )
	{
	  Lev->AllWaypoints[i].connections[l]=Lev->AllWaypoints[i].connections[l+(k-j)];
	}

      // So the block of leading -1 entries has been eliminated
      // BUT:  This may have introduced double entries of waypoints, e.g. if there was a -1
      // at the start and all other entries filled!  WE DO NOT HANDLE THIS CASE.  SORRY.
      // Also there might be a second sequence of -1 entries followed by another non-(-1)-entry
      // sequence.  SORRY, THAT CASE WILL ALSO NOT BE HANDLES SEPARATELY.  Maybe later.
      // For now this function will do perfectly well as it is now.

    }

}; // void CheckWaypointIntegrity(Level Lev)

/* ----------------------------------------------------------------------
 * This should write the obstacle information in human-readable form into
 * a buffer.
 * ---------------------------------------------------------------------- */
void
encode_obstacles_of_this_level ( char* LevelMem , Level Lev )
{
  int i;
  char linebuf[5000];	  

  //--------------------
  // Now we write out a marker.  This marker is not really
  // vital for reading in the file again, but it adds clearness to the files structure.
  //
  strcat(LevelMem, OBSTACLE_DATA_BEGIN_STRING);
  strcat(LevelMem, "\n");

  for ( i = 0 ; i < MAX_OBSTACLES_ON_MAP ; i ++ )
    {
      if ( Lev -> obstacle_list [ i ] . type == (-1) ) continue;

      strcat( LevelMem , OBSTACLE_TYPE_STRING );
      sprintf( linebuf , "%d " , Lev -> obstacle_list [ i ] . type );
      strcat( LevelMem , linebuf );

      strcat( LevelMem , OBSTACLE_X_POSITION_STRING );
      sprintf( linebuf , "%3.2f " , Lev -> obstacle_list [ i ] . pos . x );
      strcat( LevelMem , linebuf );

      strcat( LevelMem , OBSTACLE_Y_POSITION_STRING );
      sprintf( linebuf , "%3.2f " , Lev -> obstacle_list [ i ] . pos . y );
      strcat( LevelMem , linebuf );

      strcat( LevelMem , OBSTACLE_LABEL_INDEX_STRING );
      sprintf( linebuf , "%d " , Lev -> obstacle_list [ i ] . name_index );
      strcat( LevelMem , linebuf );

      strcat( LevelMem , "\n" );
    }
  
  //--------------------
  // Now we write out a marker at the end of the map data.  This marker is not really
  // vital for reading in the file again, but it adds clearness to the files structure.
  //
  strcat(LevelMem, OBSTACLE_DATA_END_STRING );
  strcat(LevelMem, "\n");
  
}; // void encode_obstacles_of_this_level ( LevelMem , Lev )

/* ----------------------------------------------------------------------
 * This function adds the statement data of this level to the chunk of 
 * data that will be written out to a file later.
 * ---------------------------------------------------------------------- */
void
EncodeMapLabelsOfThisLevel ( char* LevelMem , Level Lev )
{
  int i;
  char linebuf[5000];	  

  //--------------------
  // Now we write out a marker at the end of the map data.  This marker is not really
  // vital for reading in the file again, but it adds clearness to the files structure.
  //
  strcat(LevelMem, MAP_LABEL_BEGIN_STRING);
  strcat(LevelMem, "\n");

  for ( i = 0 ; i < MAX_MAP_LABELS_PER_LEVEL ; i ++ )
    {
      if ( Lev -> labels [ i ] . pos . x == (-1) ) continue;

      strcat( LevelMem , X_POSITION_OF_LABEL_STRING );
      sprintf( linebuf , "%d " , Lev -> labels [ i ] . pos . x );
      strcat( LevelMem , linebuf );

      strcat( LevelMem , Y_POSITION_OF_LABEL_STRING );
      sprintf( linebuf , "%d " , Lev -> labels [ i ] . pos . y );
      strcat( LevelMem , linebuf );

      strcat( LevelMem , LABEL_ITSELF_ANNOUNCE_STRING );
      strcat( LevelMem , Lev -> labels [ i ] . label_name );
      strcat( LevelMem , "\"\n" );
    }
  
  //--------------------
  // Now we write out a marker at the end of the map data.  This marker is not really
  // vital for reading in the file again, but it adds clearness to the files structure.
  //
  strcat(LevelMem, MAP_LABEL_END_STRING);
  strcat(LevelMem, "\n");
  
}; // void EncodeMapLabelsOfThisLevel ( char* LevelMem , Level Lev )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
encode_obstacle_names_of_this_level ( char* LevelMem , Level Lev )
{
  int i;
  char linebuf[5000];	  

  //--------------------
  // Now we write out a marker at the end of the map data.  This marker is not really
  // vital for reading in the file again, but it adds clearness to the files structure.
  //
  strcat(LevelMem, OBSTACLE_LABEL_BEGIN_STRING);
  strcat(LevelMem, "\n");

  for ( i = 0 ; i < MAX_OBSTACLE_NAMES_PER_LEVEL ; i ++ )
    {
      if ( Lev -> obstacle_name_list [ i ] == NULL ) continue;

      strcat( LevelMem , INDEX_OF_OBSTACLE_NAME );
      sprintf( linebuf , "%d " , i );
      strcat( LevelMem , linebuf );

      strcat( LevelMem , OBSTACLE_LABEL_ANNOUNCE_STRING );
      strcat( LevelMem , Lev -> obstacle_name_list [ i ] );
      strcat( LevelMem , "\"\n" );
    }
  
  //--------------------
  // Now we write out a marker at the end of the map data.  This marker is not really
  // vital for reading in the file again, but it adds clearness to the files structure.
  //
  strcat(LevelMem, OBSTACLE_LABEL_END_STRING);
  strcat(LevelMem, "\n\n");
  
}; // void encode_obstacle_names_of_this_level ( char* LevelMem , Level Lev )

/* ----------------------------------------------------------------------
 * This function adds the statement data of this level to the chunk of 
 * data that will be written out to a file later.
 * ---------------------------------------------------------------------- */
void
EncodeStatementsOfThisLevel ( char* LevelMem , Level Lev )
{
  int i;
  char linebuf[5000];	  

  //--------------------
  // Now we write out a marker at the end of the map data.  This marker is not really
  // vital for reading in the file again, but it adds clearness to the files structure.
  //
  strcat(LevelMem, STATEMENT_BEGIN_STRING);
  strcat(LevelMem, "\n");

  for ( i = 0 ; i < MAX_STATEMENTS_PER_LEVEL ; i ++ )
    {
      if ( Lev->StatementList[ i ].x == (-1) ) continue;

      strcat( LevelMem , X_POSITION_OF_STATEMENT_STRING );
      sprintf( linebuf , "%d " , Lev->StatementList[ i ].x );
      strcat( LevelMem , linebuf );

      strcat( LevelMem , Y_POSITION_OF_STATEMENT_STRING );
      sprintf( linebuf , "%d " , Lev->StatementList[ i ].y );
      strcat( LevelMem , linebuf );

      strcat( LevelMem , STATEMENT_ITSELF_ANNOUNCE_STRING );
      strcat( LevelMem , Lev->StatementList[ i ].Statement_Text );
      strcat( LevelMem , "\"\n" );
    }
  
  //--------------------
  // Now we write out a marker at the end of the map data.  This marker is not really
  // vital for reading in the file again, but it adds clearness to the files structure.
  //
  strcat(LevelMem, STATEMENT_END_STRING);
  strcat(LevelMem, "\n\n");
  
}; // void EncodeStatementsOfThisLevel ( char* LevelMem , Level Lev )

/* ----------------------------------------------------------------------
 *
 * 
 * ---------------------------------------------------------------------- */
void
WriteOutOneItem ( char* LevelMem , Item ItemToWriteOut ) 
{
  char linebuf[5000];	  

  strcat( LevelMem , ITEM_CODE_STRING );
  sprintf( linebuf , "%d " , ItemToWriteOut->type );
  strcat( LevelMem , linebuf );
  
  strcat( LevelMem , ITEM_POS_X_STRING );
  sprintf( linebuf , "%f " , ItemToWriteOut->pos.x );
  strcat( LevelMem , linebuf );
  
  strcat( LevelMem , ITEM_POS_Y_STRING );
  sprintf( linebuf , "%f " , ItemToWriteOut->pos.y );
  strcat( LevelMem , linebuf );
  
  strcat( LevelMem , ITEM_AC_BONUS_STRING );
  sprintf( linebuf , "%d " , ItemToWriteOut->ac_bonus );
  strcat( LevelMem , linebuf );
  
  strcat( LevelMem , ITEM_DAMAGE_STRING );
  sprintf( linebuf , "%d " , ItemToWriteOut->damage );
  strcat( LevelMem , linebuf );
  
  strcat( LevelMem , ITEM_DAMAGE_MODIFIER_STRING );
  sprintf( linebuf , "%d " , ItemToWriteOut->damage_modifier );
  strcat( LevelMem , linebuf );
  
  strcat( LevelMem , ITEM_MAX_DURATION_STRING );
  sprintf( linebuf , "%d " , ItemToWriteOut->max_duration );
  strcat( LevelMem , linebuf );
  
  strcat( LevelMem , ITEM_CUR_DURATION_STRING );
  sprintf( linebuf , "%f " , ItemToWriteOut->current_duration );
  strcat( LevelMem , linebuf );
  
  strcat( LevelMem , ITEM_GOLD_AMOUNT_STRING );
  sprintf( linebuf , "%d " , ItemToWriteOut->gold_amount );
  strcat( LevelMem , linebuf );
  
  strcat( LevelMem , ITEM_MULTIPLICITY_STRING );
  sprintf( linebuf , "%d " , ItemToWriteOut->multiplicity );
  strcat( LevelMem , linebuf );
  
  strcat( LevelMem , ITEM_PREFIX_CODE_STRING );
  sprintf( linebuf , "%d " , ItemToWriteOut->prefix_code );
  strcat( LevelMem , linebuf );
  
  strcat( LevelMem , ITEM_SUFFIX_CODE_STRING );
  sprintf( linebuf , "%d " , ItemToWriteOut->suffix_code );
  strcat( LevelMem , linebuf );
  
  // Now we save the primary stat (attribute) boni
  
  strcat( LevelMem , ITEM_BONUS_TO_STR_STRING );
  sprintf( linebuf , "%d " , ItemToWriteOut->bonus_to_str );
  strcat( LevelMem , linebuf );
  
  strcat( LevelMem , ITEM_BONUS_TO_DEX_STRING );
  sprintf( linebuf , "%d " , ItemToWriteOut->bonus_to_dex );
  strcat( LevelMem , linebuf );
  
  strcat( LevelMem , ITEM_BONUS_TO_VIT_STRING );
  sprintf( linebuf , "%d " , ItemToWriteOut->bonus_to_vit );
  strcat( LevelMem , linebuf );
  
  strcat( LevelMem , ITEM_BONUS_TO_MAG_STRING );
  sprintf( linebuf , "%d " , ItemToWriteOut->bonus_to_mag );
  strcat( LevelMem , linebuf );
  
  strcat( LevelMem , ITEM_BONUS_TO_ALLATT_STRING );
  sprintf( linebuf , "%d " , ItemToWriteOut->bonus_to_all_attributes );
  strcat( LevelMem , linebuf );
  
  // Now we save the secondary stat boni
  
  strcat( LevelMem , ITEM_BONUS_TO_LIFE_STRING );
  sprintf( linebuf , "%d " , ItemToWriteOut->bonus_to_life );
  strcat( LevelMem , linebuf );
  
  strcat( LevelMem , ITEM_BONUS_TO_FORCE_STRING );
  sprintf( linebuf , "%d " , ItemToWriteOut->bonus_to_force );
  strcat( LevelMem , linebuf );
  
  strcat( LevelMem , ITEM_BONUS_TO_TOHIT_STRING );
  sprintf( linebuf , "%d " , ItemToWriteOut->bonus_to_tohit );
  strcat( LevelMem , linebuf );
  
  strcat( LevelMem , ITEM_BONUS_TO_ACDAM_STRING );
  sprintf( linebuf , "%d " , ItemToWriteOut->bonus_to_ac_or_damage );
  strcat( LevelMem , linebuf );
  
  // Now we save the resistanc boni
  
  strcat( LevelMem , ITEM_BONUS_TO_RESELE_STRING );
  sprintf( linebuf , "%d " , ItemToWriteOut->bonus_to_resist_electricity );
  strcat( LevelMem , linebuf );
  
  strcat( LevelMem , ITEM_BONUS_TO_RESFOR_STRING );
  sprintf( linebuf , "%d " , ItemToWriteOut->bonus_to_resist_force );
  strcat( LevelMem , linebuf );
  
  strcat( LevelMem , ITEM_BONUS_TO_RESFIR_STRING );
  sprintf( linebuf , "%d " , ItemToWriteOut->bonus_to_resist_fire );
  strcat( LevelMem , linebuf );
  
  strcat( LevelMem , "\n" );
  
}; // void WriteOutOneItem ( LevelMem , ItemToWriteOut ) 

/* ----------------------------------------------------------------------
 *
 * ---------------------------------------------------------------------- */
void
EncodeItemSectionOfThisLevel ( char* LevelMem , Level Lev ) 
{
  // char linebuf[5000];	// Buffer 
  int i;

  //--------------------
  // Now we write out a marker to announce the beginning of the items data
  //
  strcat(LevelMem, ITEMS_SECTION_BEGIN_STRING);
  strcat(LevelMem, "\n");

  //--------------------
  // Now we write out the bulk of items infos
  //
  for ( i = 0 ; i < MAX_ITEMS_PER_LEVEL ; i ++ )
    {
      if ( Lev->ItemList[ i ].type == (-1) ) continue;

      WriteOutOneItem ( LevelMem , & ( Lev->ItemList[ i ] ) ); 

    }
  //--------------------
  // Now we write out a marker to announce the end of the items data
  //
  strcat(LevelMem, ITEMS_SECTION_END_STRING);
  strcat(LevelMem, "\n");
  
}; // void EncodeItemSectionOfThisLevel ( LevelMem , Lev ) 

		
/* ----------------------------------------------------------------------
 *
 * ---------------------------------------------------------------------- */
void
EncodeChestItemSectionOfThisLevel ( char* LevelMem , Level Lev ) 
{
  // char linebuf[5000];	// Buffer 
  int i;

  //--------------------
  // Now we write out a marker to announce the beginning of the items data
  //
  strcat(LevelMem, CHEST_ITEMS_SECTION_BEGIN_STRING);
  strcat(LevelMem, "\n");

  //--------------------
  // Now we write out the bulk of items infos
  //
  for ( i = 0 ; i < MAX_ITEMS_PER_LEVEL ; i ++ )
    {
      if ( Lev->ChestItemList[ i ].type == (-1) ) continue;

      WriteOutOneItem ( LevelMem , & ( Lev->ChestItemList[ i ] ) ); 

    }
  //--------------------
  // Now we write out a marker to announce the end of the items data
  //
  strcat(LevelMem, CHEST_ITEMS_SECTION_END_STRING);
  strcat(LevelMem, "\n");
  
}; // void EncodeChestItemSectionOfThisLevel ( LevelMem , Lev ) 

		
/* ----------------------------------------------------------------------
 * This function generates savable text out of the current lavel data
 * ---------------------------------------------------------------------- */
char *
EncodeLevelForSaving(Level Lev)
{
  char *LevelMem;
  int i, j;
  unsigned int MemAmount = 0 ;		// the size of the level-data 
  int xlen = Lev->xlen, ylen = Lev->ylen;
  int anz_wp;		// number of Waypoints 
  char linebuf[5000];		// Buffer 
  waypoint *this_wp;
  Uint16 HumanReadableMapLine[10000];
  
  // Get the number of waypoints 
  anz_wp = 0;
  while( Lev->AllWaypoints[anz_wp++].x != 0 );
  anz_wp -- ;		// we counted one too much 
		
  // estimate the amount of memory needed 
  MemAmount = (xlen+1) * (ylen+1); 	// Map-memory 
  MemAmount += MAXWAYPOINTS * MAX_WP_CONNECTIONS * 4;
  MemAmount += 500000;		// add some safety buffer for dimension-strings and marker strings...
  
  /* allocate some memory */
  if( (LevelMem = (char*)malloc(MemAmount)) == NULL) {
    DebugPrintf( 0 , "\n\nError in StructToMem:  Could not allocate memory...\n\nTerminating...\n\n");
    Terminate(ERR);
    return NULL;
  }

  // Write the data to memory:
  // Here the levelnumber and general information about the level is written
  sprintf(linebuf, "Levelnumber: %d\n\
xlen of this level: %d\n\
ylen of this level: %d\n\
light radius bonus of this level: %d\n\
jump threshold north: %d\n\
jump threshold south: %d\n\
jump threshold east: %d\n\
jump threshold west: %d\n\
jump target north: %d\n\
jump target south: %d\n\
jump target east: %d\n\
jump target west: %d\n",
	  Lev->levelnum, Lev->xlen, Lev->ylen, Lev -> light_radius_bonus , 
	  Lev->jump_threshold_north, 
	  Lev->jump_threshold_south, 
	  Lev->jump_threshold_east, 
	  Lev->jump_threshold_west, 
	  Lev->jump_target_north, 
	  Lev->jump_target_south, 
	  Lev->jump_target_east, 
	  Lev->jump_target_west
	  );
  strcpy(LevelMem, linebuf);
  strcat(LevelMem, LEVEL_NAME_STRING );
  strcat(LevelMem, Lev->Levelname );
  strcat(LevelMem, "\n" );
  strcat(LevelMem, LEVEL_ENTER_COMMENT_STRING );
  strcat(LevelMem, Lev->Level_Enter_Comment );
  strcat(LevelMem, "\n" );
  strcat(LevelMem, BACKGROUND_SONG_NAME_STRING );
  strcat(LevelMem, Lev->Background_Song_Name );
  // strcat(LevelMem, Decknames[Lev->levelnum] ); 
  strcat(LevelMem, "\n" );
  
  // Now the beginning of the actual map data is marked:
  strcat(LevelMem, MAP_BEGIN_STRING);
  strcat(LevelMem, "\n");

  // Now in the loop each line of map data should be saved as a whole
  for( i = 0 ; i < ylen ; i++ ) {

    // But before we can write this line of the map to the disk, we need to
    // convert is back to human readable format.
    TranslateToHumanReadable ( HumanReadableMapLine , Lev->map[i] , xlen , Lev , i );
    strncat( LevelMem, (char*) HumanReadableMapLine , xlen * 4 * 2 ); // We need FOUR , no EIGHT chars per map tile
    strcat(LevelMem, "\n");
  }

  // Now we write out a marker at the end of the map data.  This marker is not really
  // vital for reading in the file again, but it adds clearness to the files structure.
  strcat(LevelMem, MAP_END_STRING);
  strcat(LevelMem, "\n");

  encode_obstacles_of_this_level ( LevelMem , Lev );

  EncodeMapLabelsOfThisLevel ( LevelMem , Lev );

  encode_obstacle_names_of_this_level ( LevelMem , Lev );

  EncodeStatementsOfThisLevel ( LevelMem , Lev );

  EncodeItemSectionOfThisLevel ( LevelMem , Lev ) ;

  EncodeChestItemSectionOfThisLevel ( LevelMem , Lev ) ;

  // --------------------  
  // The next thing we must do is write the waypoints of this level also
  // to disk.

  // There might be LEADING -1 entries in front of other connection entries.
  // This is unwanted and shall be corrected here.
  CheckWaypointIntegrity( Lev );

  strcat(LevelMem, WP_SECTION_BEGIN_STRING);
  strcat(LevelMem, "\n");
  
  for(i=0; i< Lev->num_waypoints ; i++)
    {
      sprintf(linebuf, "Nr.=%3d x=%4d y=%4d", i, Lev->AllWaypoints[i].x , Lev->AllWaypoints[i].y );
      strcat( LevelMem, linebuf );
      strcat( LevelMem, "\t ");
      strcat (LevelMem, CONNECTION_STRING);

      this_wp = &Lev->AllWaypoints[i];
      for( j=0; j < this_wp->num_connections; j++) 
	{
	  sprintf(linebuf, " %3d", Lev->AllWaypoints[i].connections[j]);
	  strcat(LevelMem, linebuf);
	} /* for connections */
      strcat(LevelMem, "\n");
    } /* for waypoints */
  
  strcat(LevelMem, LEVEL_END_STRING);
  strcat(LevelMem, "\n----------------------------------------------------------------------\n");
  
  //--------------------
  // So we're done now.  Did the estimate for the amount of memory hit
  // the target or was it at least sufficient? 
  // If not, we're in trouble...
  //
  if ( strlen ( LevelMem ) >= MemAmount ) 
    {
      printf("\n\nError in StructToMem:  Estimate of memory was wrong...\n\nTerminating...\n\n");
      Terminate(ERR);
      return NULL;
    } 
  
  /* all ok : */
  return LevelMem;
  
}; // char *EncodeLevelForSaving ( Level Lev )


/* ----------------------------------------------------------------------
 * This function should save a whole ship to disk to the given filename.
 * It is not only used by the level editor, but also by the function that
 * saves whole games.
 * ---------------------------------------------------------------------- */
int 
SaveShip(char *filename)
{
  char *LevelMem;		/* linear memory for one Level */
  char *MapHeaderString;
  FILE *ShipFile;  // to this file we will save all the ship data...
  int level_anz;
  int array_i, array_num;
  int i;

  DebugPrintf (2, "\nint SaveShip(char *shipname): real function call confirmed.");
  
  ShowSaveLoadGameProgressMeter( 0 , TRUE ) ;

  /* count the levels */
  level_anz = 0;
  while(curShip.AllLevels[level_anz++]);
  level_anz --;
  
  DebugPrintf (2, "\nint SaveShip(char *shipname): now opening the ship file...");

  /* open file */
  if( (ShipFile = fopen(filename, "w")) == NULL) {
    printf("\n\nError opening ship file...\n\nTerminating...\n\n");
    Terminate(ERR);
    return ERR;
  }
  
  //--------------------
  // Now that the file is opend for writing, we can start writing.  And the first thing
  // we will write to the file will be a fine header, indicating what this file is about
  // and things like that...
  //
  MapHeaderString="\n\
----------------------------------------------------------------------\n\
 *\n\
 *   Copyright (c) 2002, 2003 Johannes Prix\n\
 *\n\
 *\n\
 *  This file is part of Freedroid\n\
 *\n\
 *  Freedroid is free software; you can redistribute it and/or modify\n\
 *  it under the terms of the GNU General Public License as published by\n\
 *  the Free Software Foundation; either version 2 of the License, or\n\
 *  (at your option) any later version.\n\
 *\n\
 *  Freedroid is distributed in the hope that it will be useful,\n\
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of\n\
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n\
 *  GNU General Public License for more details.\n\
 *\n\
 *  You should have received a copy of the GNU General Public License\n\
 *  along with Freedroid; see the file COPYING. If not, write to the \n\
 *  Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, \n\
 *  MA  02111-1307  USA\n\
 *\n\
----------------------------------------------------------------------\n\
\n\
This file was generated using the Freedroid level editor.\n\
Please feel free to make any modifications you like, but in order for you\n\
to have an easier time, it is recommended that you use the Freedroid level\n\
editor for this purpose.  If you have created some good new maps, please \n\
send a short notice (not too large files attached) to the freedroid project.\n\
\n\
freedroid-discussion@lists.sourceforge.net\n\
\n";
  fwrite ( MapHeaderString , strlen( MapHeaderString), sizeof(char), ShipFile);  

  // Now we write the area name back into the file
  fwrite ( AREA_NAME_STRING , strlen( AREA_NAME_STRING ), sizeof(char), ShipFile);  
  fwrite ( curShip.AreaName , strlen( curShip.AreaName ), sizeof(char), ShipFile);  
  fwrite( "\"\n\n  ", strlen( "\"\n\n  " ) , sizeof(char) , ShipFile );

  /* Save all Levels */
  
  DebugPrintf (2, "\nint SaveShip(char *shipname): now saving levels...");

  for( i = 0 ; i < level_anz ; i++ ) 
    {

      //--------------------
      // What the heck does this do?
      // Do we really need this?  Why?
      //
      array_i =-1;
      array_num = -1;
      while( curShip.AllLevels[++array_i] != NULL) 
	{
	  if( curShip.AllLevels[array_i]->levelnum == i)
	    {
	      if( array_num != -1 ) 
		{
		  printf("\n\nIdentical Levelnumber Error in SaveShip...\n\nTerminating\n\n");
		  Terminate(ERR);
		  return ERR;
		} 
	      else array_num = array_i;
	    }
	} // while 
      if ( array_num == -1 ) {

	printf("\n\nMissing Levelnumber error in SaveShip...\n\nTerminating\n\n");
	Terminate(ERR);
      
	level_anz ++;
	continue;
      }
    
      //--------------------
      // Now comes the real saving part FOR ONE LEVEL.  First THE LEVEL is packed into a string and
      // then this string is wirtten to the file.  easy. simple.
      //
      LevelMem = EncodeLevelForSaving (curShip.AllLevels[array_num]);
      fwrite(LevelMem, strlen(LevelMem), sizeof(char), ShipFile);
    
      free(LevelMem);

      ShowSaveLoadGameProgressMeter( (int) ( (100 * (i+1)) / level_anz ) , TRUE ); 
    }

  //--------------------
  // Now we are almost done writing.  Everything that is missing is
  // the termination string for the ship file.  This termination string
  // is needed later for the ship loading functions to find the end of
  // the data and to be able to terminate the long file-string with a
  // null character at the right position.
  //
  fwrite( END_OF_SHIP_DATA_STRING , strlen( END_OF_SHIP_DATA_STRING ) , sizeof(char) , ShipFile );
  fwrite( "\n\n  ", strlen( "\n\n  " ) , sizeof(char) , ShipFile );

  
  DebugPrintf (2, "\nint SaveShip(char *shipname): now closing ship file...");

  if( fclose(ShipFile) == EOF) 
    {
      printf("\n\nClosing of ship file failed in SaveShip...\n\nTerminating\n\n");
      Terminate(ERR);
      return ERR;
    }
  
  DebugPrintf (2, "\nint SaveShip(char *shipname): end of function reached.");
  
  return OK;

}; // int SaveShip ( char* filename )

void
ReadInOneItem ( char* ItemPointer , char* ItemsSectionEnd , Item TargetItem )
{

  ReadValueFromString( ItemPointer , ITEM_CODE_STRING , "%d" , 
		       &(TargetItem -> type) , ItemsSectionEnd );
  ReadValueFromString( ItemPointer , ITEM_POS_X_STRING , "%lf" , 
		       &(TargetItem -> pos.x) , ItemsSectionEnd );
  ReadValueFromString( ItemPointer , ITEM_POS_Y_STRING , "%lf" , 
		       &(TargetItem -> pos.y) , ItemsSectionEnd );
  ReadValueFromString( ItemPointer , ITEM_AC_BONUS_STRING , "%d" , 
		       &( TargetItem -> ac_bonus ) , ItemsSectionEnd );
  ReadValueFromString( ItemPointer , ITEM_DAMAGE_STRING , "%d" , 
		       &( TargetItem -> damage ) , ItemsSectionEnd );
  ReadValueFromString( ItemPointer , ITEM_DAMAGE_MODIFIER_STRING , "%d" , 
		       &( TargetItem -> damage_modifier ) , ItemsSectionEnd );
  ReadValueFromString( ItemPointer , ITEM_MAX_DURATION_STRING , "%d" , 
		       &( TargetItem -> max_duration ) , ItemsSectionEnd );
  ReadValueFromString( ItemPointer , ITEM_CUR_DURATION_STRING , "%f" , 
		       &( TargetItem -> current_duration ) , ItemsSectionEnd );
  ReadValueFromString( ItemPointer , ITEM_GOLD_AMOUNT_STRING , "%d" , 
		       &( TargetItem -> gold_amount ) , ItemsSectionEnd );
  ReadValueFromString( ItemPointer , ITEM_MULTIPLICITY_STRING , "%d" , 
		       &( TargetItem -> multiplicity ) , ItemsSectionEnd );
  ReadValueFromString( ItemPointer , ITEM_PREFIX_CODE_STRING , "%d" , 
		       &( TargetItem -> prefix_code ) , ItemsSectionEnd );
  ReadValueFromString( ItemPointer , ITEM_SUFFIX_CODE_STRING , "%d" , 
		       &( TargetItem -> suffix_code ) , ItemsSectionEnd );
  // Now we read in the boni to the primary stats (attributes)
  ReadValueFromString( ItemPointer , ITEM_BONUS_TO_STR_STRING , "%d" , 
		       &( TargetItem -> bonus_to_str ) , ItemsSectionEnd );
  ReadValueFromString( ItemPointer , ITEM_BONUS_TO_DEX_STRING , "%d" , 
		       &( TargetItem -> bonus_to_dex ) , ItemsSectionEnd );
  ReadValueFromString( ItemPointer , ITEM_BONUS_TO_MAG_STRING , "%d" , 
		       &( TargetItem -> bonus_to_mag ) , ItemsSectionEnd );
  ReadValueFromString( ItemPointer , ITEM_BONUS_TO_VIT_STRING , "%d" , 
		       &( TargetItem -> bonus_to_vit ) , ItemsSectionEnd );
  ReadValueFromString( ItemPointer , ITEM_BONUS_TO_ALLATT_STRING , "%d" , 
		       &( TargetItem -> bonus_to_all_attributes ) , ItemsSectionEnd );
  // Now we read in the boni for the secondary stats
  ReadValueFromString( ItemPointer , ITEM_BONUS_TO_LIFE_STRING , "%d" , 
		       &( TargetItem -> bonus_to_life ) , ItemsSectionEnd );
  ReadValueFromString( ItemPointer , ITEM_BONUS_TO_FORCE_STRING , "%d" , 
		       &( TargetItem -> bonus_to_force ) , ItemsSectionEnd );
  ReadValueFromString( ItemPointer , ITEM_BONUS_TO_TOHIT_STRING , "%d" , 
		       &( TargetItem -> bonus_to_tohit ) , ItemsSectionEnd );
  ReadValueFromString( ItemPointer , ITEM_BONUS_TO_ACDAM_STRING , "%d" , 
		       &( TargetItem -> bonus_to_ac_or_damage ) , ItemsSectionEnd );
  // Now we read in the boni for resistances
  ReadValueFromString( ItemPointer , ITEM_BONUS_TO_RESELE_STRING , "%d" , 
		       &( TargetItem -> bonus_to_resist_electricity ) , ItemsSectionEnd );
  ReadValueFromString( ItemPointer , ITEM_BONUS_TO_RESFIR_STRING , "%d" , 
		       &( TargetItem -> bonus_to_resist_fire ) , ItemsSectionEnd );
  ReadValueFromString( ItemPointer , ITEM_BONUS_TO_RESFOR_STRING , "%d" , 
		       &( TargetItem -> bonus_to_resist_force ) , ItemsSectionEnd );
  DebugPrintf( 1 , "\nPosX=%f PosY=%f Item=%d" , TargetItem -> pos.x , 
	       TargetItem -> pos.y , TargetItem -> type );
  
}; // void ReadInOneItem ( ItemPointer , &(loadlevel->ItemList[ i ]) )

//----------------------------------------------------------------------
// From here on we take apart the items section of the loaded level...
//----------------------------------------------------------------------
void
DecodeItemSectionOfThisLevel ( Level loadlevel , char* data )
{
  int i;
  char Preserved_Letter;
  int NumberOfItemsInThisLevel;
  char* ItemPointer;
  char* ItemsSectionBegin;
  char* ItemsSectionEnd;

  //--------------------
  // First we initialize the items arrays with 'empty' information
  //
  for ( i = 0 ; i < MAX_ITEMS_PER_LEVEL ; i ++ )
    {
      loadlevel->ItemList[ i ].pos.x = ( -1 ) ;
      loadlevel->ItemList[ i ].pos.y = ( -1 ) ;
      loadlevel->ItemList[ i ].type = ( -1 ) ;
      loadlevel->ItemList[ i ].currently_held_in_hand = FALSE;
    }

  // We look for the beginning and end of the items section
  ItemsSectionBegin = LocateStringInData( data , ITEMS_SECTION_BEGIN_STRING );
  ItemsSectionEnd = LocateStringInData( data , ITEMS_SECTION_END_STRING );

  // We add a terminator at the end of the items section, but ONLY TEMPORARY.  
  // The damage will be restored later!
  Preserved_Letter=ItemsSectionEnd[0];
  ItemsSectionEnd[0]=0;
  NumberOfItemsInThisLevel = CountStringOccurences ( ItemsSectionBegin , ITEM_CODE_STRING ) ;
  DebugPrintf( 1 , "\nNumber of items found in this level : %d." , NumberOfItemsInThisLevel );

  // Now we decode all the item information
  ItemPointer=ItemsSectionBegin;
  for ( i = 0 ; i < NumberOfItemsInThisLevel ; i ++ )
    {
      ItemPointer = strstr ( ItemPointer + 1 , ITEM_CODE_STRING );
      ReadInOneItem ( ItemPointer , ItemsSectionEnd , &(loadlevel->ItemList[ i ]) );
    }
  
  // Now we repair the damage done to the loaded level data
  ItemsSectionEnd[0]=Preserved_Letter;
}; // void DecodeItemSectionOfThisLevel ( Level loadlevel , char* data )

//----------------------------------------------------------------------
// From here on we take apart the chest items section of the loaded level...
//----------------------------------------------------------------------
void
DecodeChestItemSectionOfThisLevel ( Level loadlevel , char* data )
{
  int i;
  char Preserved_Letter;
  int NumberOfItemsInThisLevel;
  char* ItemPointer;
  char* ItemsSectionBegin;
  char* ItemsSectionEnd;

  //--------------------
  // First we initialize the items arrays with 'empty' information
  //
  for ( i = 0 ; i < MAX_ITEMS_PER_LEVEL ; i ++ )
    {
      loadlevel->ChestItemList[ i ].pos.x = ( -1 ) ;
      loadlevel->ChestItemList[ i ].pos.y = ( -1 ) ;
      loadlevel->ChestItemList[ i ].type = ( -1 ) ;
      loadlevel->ChestItemList[ i ].currently_held_in_hand = FALSE;
    }

  // We look for the beginning and end of the items section
  ItemsSectionBegin = LocateStringInData( data , CHEST_ITEMS_SECTION_BEGIN_STRING );
  ItemsSectionEnd = LocateStringInData( data , CHEST_ITEMS_SECTION_END_STRING );

  // We add a terminator at the end of the items section, but ONLY TEMPORARY.  
  // The damage will be restored later!
  Preserved_Letter=ItemsSectionEnd[0];
  ItemsSectionEnd[0]=0;
  NumberOfItemsInThisLevel = CountStringOccurences ( ItemsSectionBegin , ITEM_CODE_STRING ) ;
  DebugPrintf( 1 , "\nNumber of chest items found in this level : %d." , NumberOfItemsInThisLevel );

  // Now we decode all the item information
  ItemPointer=ItemsSectionBegin;
  for ( i = 0 ; i < NumberOfItemsInThisLevel ; i ++ )
    {
      ItemPointer = strstr ( ItemPointer + 1 , ITEM_CODE_STRING );
      ReadInOneItem ( ItemPointer , ItemsSectionEnd , &(loadlevel->ChestItemList[ i ]) );
    }
  
  // Now we repair the damage done to the loaded level data
  ItemsSectionEnd[0]=Preserved_Letter;

}; // void DecodeChestItemSectionOfThisLevel ( Level loadlevel , char* data )


/* ----------------------------------------------------------------------
 * This function is for LOADING map data!
 * This function extracts the data from *data and writes them 
 * into a Level-struct:
 *
 * NOTE:  Here, the map-data are NOT yet translated to their 
 *        their internal values, like "VOID", "H_OPEN_DOOR" and
 *         all the other values from the defs.h file.
 *
 * Doors and Waypoints Arrays are initialized too
 *
 * @Ret:  Level or NULL
 * ---------------------------------------------------------------------- */
Level
DecodeLoadedLeveldata ( char *data )
{
  Level loadlevel;
  char *pos;
  char *map_begin, *wp_begin;
  char *WaypointPointer;
  int i;
  int nr, x, y;
  int k;
  int connection;
  char *this_line;
  char* DataPointer;
  char* level_end;
  int res;

  //--------------------
  // Get the memory for one level 
  //
  loadlevel = (Level) MyMalloc (sizeof (level));

  DebugPrintf (2, "\n-----------------------------------------------------------------");
  DebugPrintf (2, "\nStarting to process information for another level:\n");

  //--------------------
  // Read Header Data: levelnum and x/ylen 
  //
  DataPointer = strstr( data , "Levelnumber:" );
  if ( DataPointer == NULL )
    {
      DebugPrintf( 0 , "No Levelnumber entry found! Terminating! ");
      Terminate(ERR);
    }

  DecodeInterfaceDataForThisLevel ( loadlevel , DataPointer );

  DecodeDimensionsOfThisLevel ( loadlevel , DataPointer );

  loadlevel->Levelname = ReadAndMallocStringFromData ( data , LEVEL_NAME_STRING , "\n" );
  loadlevel->Background_Song_Name = ReadAndMallocStringFromData ( data , BACKGROUND_SONG_NAME_STRING , "\n" );
  loadlevel->Level_Enter_Comment = ReadAndMallocStringFromData ( data , LEVEL_ENTER_COMMENT_STRING , "\n" );

  decode_obstacles_of_this_level ( loadlevel , DataPointer );

  DecodeMapLabelsOfThisLevel ( loadlevel , DataPointer );

  decode_obstacle_names_of_this_level ( loadlevel , DataPointer );

  //--------------------
  // Next we extract the statments of the influencer on this level WITHOUT destroying
  // or damaging the data in the process!
  //
  DecodeStatementsOfThisLevel ( loadlevel , DataPointer );

  DecodeItemSectionOfThisLevel ( loadlevel , data );

  DecodeChestItemSectionOfThisLevel ( loadlevel , data );

  //--------------------
  // find the map data
  // NOTE, that we here only set up a pointer to the map data
  // as they are stored in the file.  This is NOT the same format
  // as the map data stored internally for the game, but rather
  // an easily human readable format with acceptable ascii 
  // characters.  The transformation into game-usable data is
  // done in a later step outside of this function!
  //
  if ((map_begin = strstr (data, MAP_BEGIN_STRING)) == NULL)
    return NULL;

  /* set position to Waypoint-Data */
  if ((wp_begin = strstr (data, WP_SECTION_BEGIN_STRING)) == NULL)
    return NULL;

  /* now scan the map */
  strtok (map_begin, "\n");	/* init strtok to map-begin */

  /* read MapData */
  for (i = 0; i < loadlevel->ylen; i++)
    if ( ( (char*)loadlevel -> map[i] = strtok ( NULL, "\n") ) == NULL)
      return NULL;

  /* Get Waypoints */
  WaypointPointer = wp_begin;

  DebugPrintf( 2 , "\nReached Waypoint-read-routine.");

  level_end = LocateStringInData ( wp_begin , LEVEL_END_STRING ) ;
  
  //--------------------
  // We decode the waypoint data from the data file into the waypoint
  // structs...
  strtok (wp_begin, "\n");

  for ( i = 0 ; i < MAXWAYPOINTS ; i++ )
    {
      if ( (this_line = strtok (NULL, "\n")) == NULL)
	return (NULL);
      if (this_line == level_end)
	{
	  loadlevel->num_waypoints = i;
	  break;
	}
      sscanf( this_line , "Nr.=%d \t x=%d \t y=%d" , &nr , &x , &y );

      // completely ignore x=0/y=0 entries, which are considered non-waypoints!!
      if ( x == 0 && y == 0 )
	continue;
      
      loadlevel->AllWaypoints[i].x=x;
      loadlevel->AllWaypoints[i].y=y;

      pos = strstr (this_line, CONNECTION_STRING);
      pos += strlen (CONNECTION_STRING);	// skip connection-string
      pos += strspn (pos, WHITE_SPACE); 		// skip initial whitespace


      for ( k = 0 ; k < MAX_WP_CONNECTIONS ; k++ )
	{
	  if (*pos == '\0')
	    break;
	  res = sscanf( pos , "%d" , &connection );
	  if ( (connection == -1) || (res == 0) || (res == EOF) )
	    break;
	  loadlevel->AllWaypoints[i].connections[k]=connection;

	  pos += strcspn (pos, WHITE_SPACE); // skip last token
	  pos += strspn (pos, WHITE_SPACE);  // skip initial whitespace for next one

	} // for k < MAX_WP_CONNECTIONS

      loadlevel->AllWaypoints[i].num_connections = k;

    } // for i < MAXWAYPOINTS

  //not quite finished yet: the old waypoint treatment has probably lead to 
  // "holes" (0/0) in the waypoint-list, and we had to keep all of the in
  // but now let's get rid of them!
  PurifyWaypointList (loadlevel);

  return (loadlevel);

}; // Level DecodeLoadedLeveldata (char *data)

/* ----------------------------------------------------------------------
 * Some structures within Freedroid rpg maps are animated in the sense
 * that the map tiles used on the secected square rotates through a number
 * of different map tile types.
 * Now of course it would be possible to search all the map all the time
 * for tiles of this type and then update only those again and again.
 * But this would loose a lot of time checking 100x100 map tiles for a
 * lot of cases again and again.
 * Therefore it seems favorable to generate a list of these tiles in 
 * advance and then update only according to the current list of such
 * map tiles.  
 * Not this function is here to assemble such a list of animated map
 * tiles for one particular map level.
 * ---------------------------------------------------------------------- */
void
GetAllAnimatedMapTiles ( Level Lev )
{
  int i;
  int xlen, ylen;
  int curdoor = 0;
  int curautogun = 0;
  int curref = 0;
  int curtele = 0;
  int obstacle_index;

  xlen = Lev -> xlen;
  ylen = Lev -> ylen;

  //--------------------
  // At first we must clear out all the junk that might
  // still be in these arrays...
  //
  for (i = 0; i < MAX_DOORS_ON_LEVEL; i++)
    Lev -> door_obstacle_indices [ i ] = ( -1 ) ;
  for (i = 0; i < MAX_TELEPORTERS_ON_LEVEL; i++)
    Lev -> teleporter_obstacle_indices [ i ] = ( -1 ) ;
  for (i = 0; i < MAX_REFRESHES_ON_LEVEL; i++)
    Lev -> refresh_obstacle_indices [ i ] = ( -1 ) ;
  for (i = 0; i < MAX_AUTOGUNS_ON_LEVEL; i++)
    Lev -> autogun_obstacle_indices [ i ] = ( -1 ) ;
  for ( i = 0 ; i < MAX_CONSUMERS_ON_LEVEL ; i ++ )
    Lev -> consumers [ i ] . x = Lev -> consumers [ i ] . y = 0;

  // DebugPrintf ( 0 , "\nPretest for level %d." , Lev -> levelnum );
  // for (i = 0; i < MAX_DOORS_ON_LEVEL; i++)
  // {
  // DebugPrintf ( 0 , " %d " , Lev -> door_obstacle_indices [ i ] ) ;
  // }

  //--------------------
  // New method:  proceed through the obstacles of this level
  // to find out where the doors are...
  //
  for ( obstacle_index = 0 ; obstacle_index < MAX_OBSTACLES_ON_MAP ; obstacle_index ++ )
    {
      /*
      //--------------------
      // Maybe we're done with the obstacles now...
      //
      if ( Lev -> obstacle_list [ obstacle_index ] . type == ( -1 ) ) 
	{
	  Lev -> door_obstacle_indices [ curdoor ] = ( -1 ) ;
	  break;
	}
      */

      switch ( Lev -> obstacle_list [ obstacle_index ] . type )
	{
	case ISO_H_DOOR_000_OPEN:
	case ISO_H_DOOR_025_OPEN:
	case ISO_H_DOOR_050_OPEN:
	case ISO_H_DOOR_075_OPEN:
	case ISO_H_DOOR_100_OPEN:

	case ISO_V_DOOR_000_OPEN:
	case ISO_V_DOOR_025_OPEN:
	case ISO_V_DOOR_050_OPEN:
	case ISO_V_DOOR_075_OPEN:
	case ISO_V_DOOR_100_OPEN:
	  
	  //--------------------
	  // We've found another door obstacle, so we add it's index
	  // into the door obstacle index list of this level...
	  //
	  Lev -> door_obstacle_indices [ curdoor ] = obstacle_index ;
	  curdoor++;
	  if ( curdoor > MAX_DOORS_ON_LEVEL)
	    {
	      fprintf( stderr , "\n\nLev->levelnum : %d MAX_DOORS_ON_LEVEL: %d \n" , 
		       Lev -> levelnum , MAX_DOORS_ON_LEVEL );
	      GiveStandardErrorMessage ( "GetAllAnimatedMapTiles(...)" , "\
The number of doors found in a level seems to be greater than the number\n\
of doors currently allowed in a Freedroid map.",
					 PLEASE_INFORM, IS_FATAL );
	    }
	  break;


	case ISO_REFRESH_1:
	case ISO_REFRESH_2:
	case ISO_REFRESH_3:
	case ISO_REFRESH_4:
	case ISO_REFRESH_5:

	  //--------------------
	  // We've found another refresh obstacle, so we add it's index
	  // into the refresh obstacle index list of this level...
	  //
	  Lev -> refresh_obstacle_indices [ curref ] = obstacle_index ;
	  curref++;
	  if ( curref > MAX_REFRESHES_ON_LEVEL)
	    {
	      fprintf( stderr , "\n\nLev->levelnum : %d MAX_REFRESHES_ON_LEVEL: %d \n" , 
		       Lev -> levelnum , MAX_REFRESHES_ON_LEVEL );
	      GiveStandardErrorMessage ( "GetAllAnimatedMapTiles(...)" , "\
The number of refreshes found in a level seems to be greater than the number\n\
of refreshes currently allowed in a Freedroid map.",
					 PLEASE_INFORM, IS_FATAL );
	    }
	  break;

	case ISO_TELEPORTER_1:
	case ISO_TELEPORTER_2:
	case ISO_TELEPORTER_3:
	case ISO_TELEPORTER_4:
	case ISO_TELEPORTER_5:
	  //--------------------
	  // We've found another teleporter obstacle, so we add it's index
	  // into the teleporter obstacle index list of this level...
	  //
	  Lev -> teleporter_obstacle_indices [ curtele ] = obstacle_index ;
	  curtele++;
	  if ( curtele > MAX_TELEPORTERS_ON_LEVEL)
	    {
	      fprintf( stderr , "\n\nLev->levelnum : %d MAX_TELEPORTERS_ON_LEVEL: %d \n" , 
		       Lev -> levelnum , MAX_TELEPORTERS_ON_LEVEL );
	      GiveStandardErrorMessage ( "GetAllAnimatedMapTiles(...)" , "\
The number of teleporters found in a level seems to be greater than the number\n\
of teleporters currently allowed in a Freedroid map.",
					 PLEASE_INFORM, IS_FATAL );
	    }
	  break;

	case ISO_AUTOGUN_N:
	case ISO_AUTOGUN_S:
	case ISO_AUTOGUN_E:
	case ISO_AUTOGUN_W:
	  //--------------------
	  // We've found another autogun obstacle, so we add it's index
	  // into the autogun obstacle index list of this level...
	  //
	  Lev -> autogun_obstacle_indices [ curautogun ] = obstacle_index ;
	  curautogun++;
	  if ( curautogun > MAX_TELEPORTERS_ON_LEVEL)
	    {
	      fprintf( stderr , "\n\nLev->levelnum : %d MAX_AUTOGUNS_ON_LEVEL: %d \n" , 
		       Lev -> levelnum , MAX_AUTOGUNS_ON_LEVEL );
	      GiveStandardErrorMessage ( "GetAllAnimatedMapTiles(...)" , "\
The number of autoguns found in a level seems to be greater than the number\n\
of autoguns currently allowed in a Freedroid map.",
					 PLEASE_INFORM, IS_FATAL );
	    }
	  break;

	default:
	  //--------------------
	  // This isn't a door, so we do nothing here...
	  //
	  break;
	}
    }

}; // void GetAllAnimatedMapTiles ( Level Lev )

/* ----------------------------------------------------------------------
 * This function translates map data into human readable map code, that
 * can later be written to the map file on disk.
 * ---------------------------------------------------------------------- */
void
TranslateToHumanReadable ( Uint16* HumanReadable , map_tile* MapInfo, int LineLength , Level Lev , int CurrentLine)
{
  int col;
  char Buffer[10];

  DebugPrintf (1,"\n\nTranslating mapline into human readable format...");
  
  // Now in the game and in the level editor, it might have happend that some open
  // doors occur.  The make life easier for the saving routine, these doors should
  // be closed first.


  HumanReadable[0]=0;  // Add a terminator at the beginning

  for (col = 0; col < LineLength; col++)
    {
      sprintf( Buffer , "%3d " , MapInfo [col] . floor_value );
      strcat ( (char*)HumanReadable , Buffer );
    }

}; // void TranslateToHumanReadable( ... )

/* ----------------------------------------------------------------------
 * This function is used to calculate the number of the droids on the 
 * ship, which is a global variable.
 * ---------------------------------------------------------------------- */
void
CountNumberOfDroidsOnShip ( void )
{
  int i;
  int type;

  Number_Of_Droids_On_Ship=0;
  for (i = 0; i < MAX_ENEMYS_ON_SHIP; i++)
    {
      type = AllEnemys[i].type;
      if ( type == (-1) ) continue;  // Do nothing to unused entries
      Number_Of_Droids_On_Ship++;
    }
}; // void CountNumberOfDroidsOnShip ( void )

/* ----------------------------------------------------------------------
 * This function is used to calculate the number of the droids on the 
 * ship, which is a global variable.
 * ---------------------------------------------------------------------- */
void
ReviveAllDroidsOnShip ( void )
{
  int i;
  int type;

  for (i = 0; i < MAX_ENEMYS_ON_SHIP; i++)
    {
      type = AllEnemys[i].type;
      if ( type == (-1) ) continue;  // Do nothing to unused entries
      AllEnemys[i].energy = Druidmap[type].maxenergy;
      AllEnemys[i].Status = MOBILE; // !OUT;
      AllEnemys[i].has_greeted_influencer = FALSE ;
      AllEnemys[i].combat_state = MOVE_ALONG_RANDOM_WAYPOINTS ;
      AllEnemys[i].state_timeout = 0 ;
      AllEnemys[i].animation_phase = 0 ;
    }
}; // void ReviveAllDroidsOnShip ( void )

/* -----------------------------------------------------------------
 * This function initializes all enemys, which means that enemys are
 * filled in into the enemy list according to the enemys types that 
 * are to be found on each deck.
 * ----------------------------------------------------------------- */
int
GetCrew (char *filename)
{
  char *fpath;
  char *MainDroidsFilePointer;
  char *DroidSectionPointer;
  char *EndOfThisDroidSectionPointer;

#define START_OF_DROID_DATA_STRING "*** Beginning of Droid Data ***"
#define END_OF_DROID_DATA_STRING "*** End of Droid Data ***"
#define DROIDS_LEVEL_DESCRIPTION_START_STRING "** Beginning of new Level **"
#define DROIDS_LEVEL_DESCRIPTION_END_STRING "** End of this levels droid data **"


  //--------------------
  // There can be two cases:  Either the droids records must be read and initialized
  // from scratch or they need not be modified in any way
  //
  // Which is the case?  ---  This can be controlled from the mission file by 
  // specifying either 

  if ( strcmp ( filename , "none" ) == 0 ) 
    {
      DebugPrintf( 0 , "\nint GetCrew( char* filename ): none received as parameter --> not reseting crew file." );
      return (OK);
    }

  ClearEnemys ();

  //--------------------
  //Now its time to start decoding the droids file.
  //For that, we must get it into memory first.
  //The procedure is the same as with LoadShip
  //
  fpath = find_file (filename, MAP_DIR, FALSE);

  MainDroidsFilePointer = ReadAndMallocAndTerminateFile( fpath , END_OF_DROID_DATA_STRING ) ;

  //--------------------
  // The Droid crew file for this map is now completely read into memory
  // It's now time to decode the file and to fill the array of enemys with
  // new droids of the given types.
  //
  DroidSectionPointer=MainDroidsFilePointer;
  while ( ( DroidSectionPointer = strstr ( DroidSectionPointer, DROIDS_LEVEL_DESCRIPTION_START_STRING )) != NULL )
    {
      DroidSectionPointer+=strlen( DROIDS_LEVEL_DESCRIPTION_START_STRING );
      DebugPrintf ( 1 , "\nFound another levels droids description starting point entry!");
      EndOfThisDroidSectionPointer = strstr ( DroidSectionPointer , DROIDS_LEVEL_DESCRIPTION_END_STRING ) ;
      if ( EndOfThisDroidSectionPointer == NULL )
	{
	  DebugPrintf( 0 , "\n\
----------------------------------------------------------------------\n\
GetCrew:  Unterminated droid section encountered!!\n\nTerminating....\n\
----------------------------------------------------------------------\n");
	  Terminate(ERR);
	}
      // EndOfThisDroidSectionPointer[0]=0;
      GetThisLevelsDroids( DroidSectionPointer );
      DroidSectionPointer = EndOfThisDroidSectionPointer+2; // Move past the inserted String terminator
    }


  //--------------------
  // Now that the correct crew types have been filled into the 
  // right structure, it's time to set the energy of the corresponding
  // droids to "full" which means to the maximum of each type.
  //
  CountNumberOfDroidsOnShip ();
  ReviveAllDroidsOnShip ();

  return (OK);
}; // int GetCrew ( ... ) 

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
GetThisLevelsSpecialForces ( char* SearchPointer , int OurLevelNumber , int FreeAllEnemysPosition , char* EndOfThisLevelData )
{
  char TypeIndicationString[1000];
  int ListIndex;
  char* StartMapLabel;
  char* YesNoString;
  location StartupLocation;
  char* DialogSection;
  char* ShortDescription;

  while ( ( SearchPointer = strstr ( SearchPointer , SPECIAL_FORCE_INDICATION_STRING)) != NULL)
    {
      SearchPointer += strlen ( SPECIAL_FORCE_INDICATION_STRING );
      strncpy( TypeIndicationString , SearchPointer , 3 ); // Every type is 3 characters long
      TypeIndicationString[3]=0;
      DebugPrintf( 1 , "\nSpecial Force Type indication found!  It reads: %s." , TypeIndicationString );

      // Now that we have got a type indication string, we only need to translate it
      // into a number corresponding to that droid in the droid list
      for ( ListIndex = 0 ; ListIndex < Number_Of_Droid_Types ; ListIndex++ )
	{
	  if ( !strcmp( Druidmap[ListIndex].druidname , TypeIndicationString ) ) break ;
	}
      if ( ListIndex == Number_Of_Droid_Types )
	{
	  fprintf ( stderr, "\n\nTypeIndicationString: '%s' OurLevelNumber: %d .\n" , 
		    TypeIndicationString , OurLevelNumber );
	  GiveStandardErrorMessage ( "GetThisLevelsSpecialForces(...)" , "\
The function reading and interpreting the crew file stunbled into something:\n\
It was unable to assign the SPECIAL FORCE droid type identification string '%s' found \n\
in the entry of the droid types allowed for level %d to an entry in\n\
the List of droids obtained from the gama data specification\n\
file you use.",
				     PLEASE_INFORM, IS_FATAL );
	}
      else
	{
	  DebugPrintf( 1 , "\nSpecial force's Type indication string %s translated to type Nr.%d." , 
		       TypeIndicationString , ListIndex );
	}

      for ( FreeAllEnemysPosition=0 ; FreeAllEnemysPosition < MAX_ENEMYS_ON_SHIP ; FreeAllEnemysPosition++ )
	{
	  if ( AllEnemys[ FreeAllEnemysPosition ].Status == OUT ) break;
	}
      if ( FreeAllEnemysPosition == MAX_ENEMYS_ON_SHIP )
	{
	  printf("\n\n No more free position to fill random droids into in GetCrew...Terminating....");
	  Terminate(ERR);
	}

      ReadValueFromString ( SearchPointer ,"X=","%lf", &AllEnemys[ FreeAllEnemysPosition ].pos.x , EndOfThisLevelData );
      ReadValueFromString ( SearchPointer ,"Y=","%lf", &AllEnemys[ FreeAllEnemysPosition ].pos.y , EndOfThisLevelData );
      ReadValueFromString ( SearchPointer ,"Fixed=","%d", &AllEnemys[ FreeAllEnemysPosition ].CompletelyFixed , 
			    EndOfThisLevelData );
      ReadValueFromString ( SearchPointer ,"Marker=","%d", &AllEnemys[ FreeAllEnemysPosition ] . marker , 
			    EndOfThisLevelData );
      ReadValueFromString ( SearchPointer ,"AdvancedCommand=","%d", &AllEnemys[ FreeAllEnemysPosition ].AdvancedCommand , 
			    EndOfThisLevelData );
      ReadValueFromString ( SearchPointer ,"Parameter1=","%lf", &AllEnemys[ FreeAllEnemysPosition ].Parameter1 , 
			    EndOfThisLevelData );
      ReadValueFromString ( SearchPointer ,"Parameter2=","%lf", &AllEnemys[ FreeAllEnemysPosition ].Parameter2 , 
			    EndOfThisLevelData );
      ReadValueFromString ( SearchPointer ,"Friendly=","%d", &AllEnemys[ FreeAllEnemysPosition ].is_friendly , 
			    EndOfThisLevelData );
      StartMapLabel = 
	ReadAndMallocStringFromData ( SearchPointer , "StartUpAtLabel=\"" , "\"" ) ;
      ResolveMapLabelOnShip ( StartMapLabel , &StartupLocation );
      AllEnemys[ FreeAllEnemysPosition ].pos.x = StartupLocation.x;
      AllEnemys[ FreeAllEnemysPosition ].pos.y = StartupLocation.y;

      YesNoString = ReadAndMallocStringFromData ( SearchPointer , "RushTux=\"" , "\"" ) ;
      if ( strcmp( YesNoString , "yes" ) == 0 )
	{
	  AllEnemys[ FreeAllEnemysPosition ] . will_rush_tux = TRUE;
	}
      else if ( strcmp( YesNoString , "no" ) == 0 )
	{
	  AllEnemys[ FreeAllEnemysPosition ] . will_rush_tux = FALSE;
	}
      else
	{
	  GiveStandardErrorMessage ( "GetThisLevelsSpecialForces(...)" , "\
The item specification of an item in ReturnOfTux.droids should contain an \n\
answer that is either 'yes' or 'no', but which was neither 'yes' nor 'no'.\n\
This indicated a corrupted freedroid.ruleset file with an error at least in\n\
the item specification section.",
				     PLEASE_INFORM, IS_FATAL );
	}

      DialogSection = 
	ReadAndMallocStringFromData ( SearchPointer , "DialogSectionToUse=\"" , "\"" ) ;
      if ( strlen ( DialogSection ) >= MAX_LENGTH_FOR_DIALOG_SECTION_NAME-1 )
	{
	  GiveStandardErrorMessage ( "GetThisLevelsSpecialForces(...)" , "\
The dialog section specification string for a bot was too large.\n\
This indicated a corrupted ReturnOfTux.droids file with an error when specifying\n\
the dialog section name for one special force droid/character.",
				     PLEASE_INFORM, IS_FATAL );
	}
      strcpy ( AllEnemys[ FreeAllEnemysPosition ].dialog_section_name , DialogSection );
      free ( DialogSection );

      ShortDescription = 
	ReadAndMallocStringFromData ( SearchPointer , "ShortLabel=\"" , "\"" ) ;
      if ( strlen ( DialogSection ) >= MAX_LENGTH_OF_SHORT_DESCRIPTION_STRING )
	{
	  GiveStandardErrorMessage ( "GetThisLevelsSpecialForces(...)" , "\
The short description specification string for a bot was too large.\n\
This indicated a corrupted ReturnOfTux.droids file with an error when specifying\n\
the dialog section name for one special force droid/character.",
				     PLEASE_INFORM, IS_FATAL );
	}
      strcpy ( AllEnemys[ FreeAllEnemysPosition ].short_description_text , ShortDescription );
      free ( ShortDescription );

      

      AllEnemys[ FreeAllEnemysPosition ].type = ListIndex;
      AllEnemys[ FreeAllEnemysPosition ].pos.z = OurLevelNumber;
      AllEnemys[ FreeAllEnemysPosition ].Status = MOBILE ; // !OUT;
      AllEnemys[ FreeAllEnemysPosition ].SpecialForce = 1;

    } // while Special force droid found...

  CountNumberOfDroidsOnShip (  );

}; // void GetThisLevelsSpecialForces ( char* SearchPointer )

/* ----------------------------------------------------------------------
 * This function receives a pointer to the already read in crew section
 * in a already read in droids file and decodes all the contents of that
 * droid section to fill the AllEnemys array with droid types accoriding
 * to the specifications made in the file.
 * ---------------------------------------------------------------------- */
void
GetThisLevelsDroids( char* SectionPointer )
{
  int OurLevelNumber;
  char* SearchPointer;
  char* EndOfThisLevelData;
  int MaxRand;
  int MinRand;
  int RealNumberOfRandomDroids;
  int DifferentRandomTypes;
  int ListIndex;
  char TypeIndicationString[1000];
  int ListOfTypesAllowed[1000];
  int FreeAllEnemysPosition = 0 ;

#define DROIDS_LEVEL_INDICATION_STRING "Level="
#define DROIDS_LEVEL_END_INDICATION_STRING "** End of this levels droid data **"
#define DROIDS_MAXRAND_INDICATION_STRING "Maximum number of Random Droids="
#define DROIDS_MINRAND_INDICATION_STRING "Minimum number of Random Droids="
#define ALLOWED_TYPE_INDICATION_STRING "Allowed Type of Random Droid for this level: "

  // printf("\nReceived another levels droid section for decoding. It reads: %s " , SectionPointer );

  EndOfThisLevelData = LocateStringInData ( SectionPointer , DROIDS_LEVEL_END_INDICATION_STRING );
  EndOfThisLevelData[0]=0;

  // Now we read in the level number for this level
  ReadValueFromString( SectionPointer , DROIDS_LEVEL_INDICATION_STRING , "%d" , &OurLevelNumber , 
		       EndOfThisLevelData );

  // Now we read in the maximal number of random droids for this level
  ReadValueFromString( SectionPointer , DROIDS_MAXRAND_INDICATION_STRING , "%d" , &MaxRand , 
		       EndOfThisLevelData );

  // Now we read in the minimal number of random droids for this level
  ReadValueFromString( SectionPointer , DROIDS_MINRAND_INDICATION_STRING , "%d" , &MinRand , 
		       EndOfThisLevelData );

  DifferentRandomTypes=0;
  SearchPointer = SectionPointer;
  while ( ( SearchPointer = strstr ( SearchPointer , ALLOWED_TYPE_INDICATION_STRING)) != NULL)
    {
      SearchPointer += strlen ( ALLOWED_TYPE_INDICATION_STRING );
      strncpy( TypeIndicationString , SearchPointer , 3 ); // Every type is 3 characters long
      TypeIndicationString[3]=0;
      // printf("\nType indication found!  It reads: %s." , TypeIndicationString );

      // Now that we have got a type indication string, we only need to translate it
      // into a number corresponding to that droid in the droid list
      for ( ListIndex = 0 ; ListIndex < Number_Of_Droid_Types ; ListIndex++ )
	{
	  if ( !strcmp( Druidmap[ListIndex].druidname , TypeIndicationString ) ) break ;
	}
      if ( ListIndex >= Number_Of_Droid_Types )
	{
	  fprintf ( stderr, "\n\nTypeIndicationString: '%s' OurLevelNumber: %d .\n" , 
		    TypeIndicationString , OurLevelNumber );
	  GiveStandardErrorMessage ( "GetThisLevelsDroids(...)" , "\
The function reading and interpreting the crew file stunbled into something:\n\
It was unable to assign the droid type identification string '%s' found \n\
in the entry of the droid types allowed for level %d to an entry in\n\
the List of droids obtained from the gama data specification\n\
file you use.  \n\
Please check that this type really is spelled correctly, that it consists of\n\
only three characters and that it really has a corresponding entry in the\n\
game data file with all droid type specifications.",
				     PLEASE_INFORM, IS_FATAL );
	}
      else
	{
	  DebugPrintf( 1 , "\nType indication string %s translated to type Nr.%d." , TypeIndicationString , ListIndex );
	}
      ListOfTypesAllowed[DifferentRandomTypes]=ListIndex;
      DifferentRandomTypes++;
    }
  DebugPrintf( 1 , "\nFound %d different allowed random types for this level. " , DifferentRandomTypes );
  
  //--------------------
  // At this point, the List "ListOfTypesAllowed" has been filled with the NUMBERS of
  // the allowed types.  The number of different allowed types found is also available.
  // That means that now we can add the apropriate droid types into the list of existing
  // droids in that mission.

  RealNumberOfRandomDroids = MyRandom ( MaxRand - MinRand) + MinRand;

  while ( RealNumberOfRandomDroids-- )
    {
      for ( FreeAllEnemysPosition=0 ; FreeAllEnemysPosition < MAX_ENEMYS_ON_SHIP ; FreeAllEnemysPosition++ )
	{
	  if ( AllEnemys[ FreeAllEnemysPosition ].Status == OUT ) break;
	}
      if ( FreeAllEnemysPosition == MAX_ENEMYS_ON_SHIP )
	{
	  printf("\n\n No more free position to fill random droids into in GetCrew...Terminating....");
	  Terminate(ERR);
	}

      AllEnemys[ FreeAllEnemysPosition ].type = ListOfTypesAllowed[MyRandom (DifferentRandomTypes-1)];
      AllEnemys[ FreeAllEnemysPosition ].pos.z = OurLevelNumber;
      AllEnemys[ FreeAllEnemysPosition ].Status = MOBILE ; // !OUT;

      strcpy ( AllEnemys[ FreeAllEnemysPosition ] . short_description_text , "No Description For This One" );

    }  // while (enemy-limit of this level not reached) 

  SearchPointer=SectionPointer;

  GetThisLevelsSpecialForces ( SearchPointer , OurLevelNumber , FreeAllEnemysPosition , EndOfThisLevelData );

}; // void GetThisLevelsDroids( char* SectionPointer )

/* ---------------------------------------------------------------------- 
 * This funtion moves the level doors in the sense that they are opened
 * or closed depending on whether there is a robot close to the door or
 * not.  Initially this function did not take into account the framerate
 * and just worked every frame.  But this WASTES COMPUTATION time and it
 * DOES THE ANIMATION TOO QUICKLY.  So, the most reasonable way out seems
 * to be to operate this function only from time to time, e.g. after a
 * specified delay has passed.
 * ---------------------------------------------------------------------- */
void
MoveLevelDoors ( int PlayerNum )
{
  int i, j;
  float xdist, ydist;
  float dist2;
  int *Pos;
  Level DoorLevel;
  int one_player_close_enough = FALSE;
  int PlayerIndex ;
  int door_obstacle_index;

  DoorLevel = curShip . AllLevels [ Me [ PlayerNum ] . pos . z ] ;

  //--------------------
  // This prevents animation going too quick.
  // The constant should be replaced by a variable, that can be
  // set from within the theme, but that may be done later...
  //
  if ( LevelDoorsNotMovedTime < Time_For_Each_Phase_Of_Door_Movement ) return;

  //--------------------
  // But only the last of these function calls for each player may 
  // reset the time counter, or the players after the first will
  // NEVER EVER BE CHECKED!!!!
  //
  if ( PlayerNum == MAX_PLAYERS -1 ) LevelDoorsNotMovedTime=0;

  if ( Me [ PlayerNum ] . status == OUT ) return;

  // DebugPrintf ( 0 , "\nMoving Doors for Player %d on level %d . != %d " , PlayerNum , DoorLevel -> levelnum , Me [ PlayerNum ] . pos . z );

  //--------------------
  // Now we go through the whole prepared list of doors for this
  // level.  This list has been prepared in advance, when the level
  // was read in.
  //
  for ( i = 0 ; i < MAX_DOORS_ON_LEVEL ; i ++ )
    {
      door_obstacle_index = DoorLevel -> door_obstacle_indices [ i ] ;

      // no more doors?
      if ( door_obstacle_index == (-1) )
	{
	  // DebugPrintf ( 0 , "\nNumber of last door moved on this level : %d." , i );
	  // DebugPrintf ( 0 , "\nNumber_Of_Droids_On_Ship : %d." , Number_Of_Droids_On_Ship );
	  break;
	}

      //--------------------
      // We make a convenient pointer to the type of the obstacle, that
      // is supposed to be a door and might need to be changed as far as
      // it's opening status is concerned...
      //
      Pos = & ( DoorLevel -> obstacle_list [ door_obstacle_index ] . type ) ;

      //--------------------
      // Some security check against changing anything that isn't a door here...
      //
      switch ( *Pos )
	{
	case ISO_H_DOOR_000_OPEN:
	case ISO_H_DOOR_025_OPEN:
	case ISO_H_DOOR_050_OPEN:
	case ISO_H_DOOR_075_OPEN:
	case ISO_H_DOOR_100_OPEN:

	case ISO_V_DOOR_000_OPEN:
	case ISO_V_DOOR_025_OPEN:
	case ISO_V_DOOR_050_OPEN:
	case ISO_V_DOOR_075_OPEN:
	case ISO_V_DOOR_100_OPEN:
	  break;
	  
	default:
	  fprintf ( stderr, "\n*Pos: '%d'.\ni: %d\nPlayerNum: %d\nlevelnum: %d\nObstacle index: %d" , *Pos , i , PlayerNum , DoorLevel -> levelnum , door_obstacle_index );
	  GiveStandardErrorMessage ( "MoveLevelDoors (...)" , "\
Error:  Doors pointing not to door obstacles found.",
				     PLEASE_INFORM, IS_FATAL );
	  break;
	}

      //--------------------
      // First we see if one of the players is close enough to the
      // door, so that it would get opened.
      //
      one_player_close_enough = FALSE;
      for ( PlayerIndex = 0 ; PlayerIndex < MAX_PLAYERS ; PlayerIndex ++ )
	{
	  //--------------------
	  // Maybe this player is on a different level, than we are 
	  // interested now.
	  //
	  if ( Me [ PlayerIndex ] . pos . z != DoorLevel -> levelnum ) continue;

	  //--------------------
	  // But this player is on the right level, we need to check it's distance 
	  // to this door.
	  //
	  xdist = Me [ PlayerIndex ] . pos . x - DoorLevel -> obstacle_list [ door_obstacle_index ] . pos . x ;
	  ydist = Me [ PlayerIndex ] . pos . y - DoorLevel -> obstacle_list [ door_obstacle_index ] . pos . y ;
	  dist2 = xdist * xdist + ydist * ydist ;
	  if ( dist2 < DOOROPENDIST2 )
	    {
	      one_player_close_enough = TRUE ;
	    }
	}

      // --------------------
      // If one of the players is close enough, the door gets opened
      // and we are done.
      //
      if ( one_player_close_enough )
	{
	  if ( ( *Pos != ISO_H_DOOR_100_OPEN ) && ( *Pos != ISO_V_DOOR_100_OPEN ) )
	    *Pos += 1;
	}
      else 
	{
	  //--------------------
	  // But if the Tux is not close enough, then we must
	  // see if perhaps one of the enemys is close enough, so that
	  // the door would still get opened instead of closed.
	  //
	  for ( j = 0 ; j < Number_Of_Droids_On_Ship ; j ++ )
	    {
	      /* ignore druids that are dead or on other levels */
	      if ( ( AllEnemys[j].Status == OUT ) ||
		   ( AllEnemys[j].pos.z  != DoorLevel->levelnum ) )
		continue;

	      // xdist = abs ( AllEnemys[j].pos.x - doorx ) ;
	      xdist = abs ( AllEnemys [ j ] . pos . x - DoorLevel -> obstacle_list [ door_obstacle_index ] . pos . x ) ;
	      if (xdist < Block_Width)
		{
		  // ydist = abs (AllEnemys[j].pos.y - doory);
		  ydist = abs ( AllEnemys [ j ] . pos . y - DoorLevel -> obstacle_list [ door_obstacle_index ] . pos . y ) ;
		  if (ydist < Block_Height)
		    {
		      dist2 = xdist * xdist + ydist * ydist;
		      if (dist2 < DOOROPENDIST2)
			{
			  if ( ( *Pos != ISO_H_DOOR_100_OPEN ) && ( *Pos != ISO_V_DOOR_100_OPEN ) )
			    *Pos += 1;

			  break;	/* one druid is enough to open a door */
			}	/* if */
		    }		/* if */
		}		/* if */
	    }			/* for */

	  /* No druid near: close door if it isnt closed */
	  if ( j == Number_Of_Droids_On_Ship )
	    if ( ( *Pos != ISO_V_DOOR_000_OPEN ) && ( *Pos != ISO_H_DOOR_000_OPEN ) )
	      *Pos -= 1;

	}			/* else */
    }				/* for */
}; // void MoveLevelDoors ( void )


/* ----------------------------------------------------------------------
 * This function does all the firing for the autocannons installed in
 * the map of this level.
 * ---------------------------------------------------------------------- */
void
WorkLevelGuns ( int PlayerNum )
{
  int i;
  float autogunx, autoguny;
  int *AutogunType;
  Level GunLevel;

  //--------------------
  // The variables for the gun.
  //
  int j = 0;
  int weapon_item_type = ITEM_SHORT_BOW ;
  Bullet CurBullet = NULL;  // the bullet we're currentl dealing with
  int bullet_image_type = ItemMap[ weapon_item_type ].item_gun_bullet_image_type;   // which gun do we have ? 
  double BulletSpeed = ItemMap[ weapon_item_type ].item_gun_speed;
  double speed_norm;
  moderately_finepoint speed;

  GunLevel = curShip . AllLevels [ Me [ PlayerNum ] . pos . z ] ;

  //--------------------
  // This prevents animation going too quick.
  // The constant should be replaced by a variable, that can be
  // set from within the theme, but that may be done later...
  //
  if ( LevelGunsNotFiredTime < 0.25 ) return;

  //--------------------
  // But only the last of these function calls for each player may 
  // reset the time counter, or the players after the first will
  // NEVER EVER BE CHECKED!!!!
  //
  if ( PlayerNum == MAX_PLAYERS -1 ) LevelGunsNotFiredTime = 0 ;

  if ( Me [ PlayerNum ] . status == OUT ) return;

  // DebugPrintf ( 0 , "\nMoving Doors for Player %d on level %d . != %d " , PlayerNum , GunLevel -> levelnum , Me [ PlayerNum ] . pos . z );

  //--------------------
  // Now we go through the whole prepared list of autoguns for this
  // level.  This list has been prepared in advance, when the level
  // was read in.
  //
  for ( i = 0 ; i < MAX_AUTOGUNS_ON_LEVEL ; i ++ )
    {

      autogunx = ( GunLevel -> obstacle_list [ GunLevel -> autogun_obstacle_indices [ i ] ] . pos . x );
      autoguny = ( GunLevel -> obstacle_list [ GunLevel -> autogun_obstacle_indices [ i ] ] . pos . y );
      // autoguny = ( GunLevel -> autoguns [ i ] . y );

      // no more autoguns?
      // if ( ( autogunx == 0 ) && ( autoguny == 0 ) )
      // break;
      if ( GunLevel -> autogun_obstacle_indices [ i ] == ( -1 ) ) break;

      // Pos = & ( GunLevel -> map [autoguny] [autogunx]  . floor_value ) ;
      AutogunType = & ( GunLevel -> obstacle_list [ GunLevel -> autogun_obstacle_indices [ i ] ] . type );

      //--------------------
      // From here on goes the bullet code, that originally came from
      // the FireTuxRangesWeaponRaw function.
      //

      //--------------------
      // search for the next free bullet list entry
      //
      for (j = 0; j < (MAXBULLETS); j++)
	{
	  if (AllBullets[j].type == OUT)
	    {
	      CurBullet = &AllBullets[j];
	      break;
	    }
	}

      // didn't find any free bullet entry? --> take the first
      if (CurBullet == NULL)
	CurBullet = &AllBullets[0];

      CurBullet->type = bullet_image_type;

      //--------------------
      // Previously, we had the damage done only dependant upon the weapon used.  Now
      // the damage value is taken directly from the character stats, and the UpdateAll...stats
      // has to do the right computation and updating of this value.  hehe. very conventient.
      CurBullet->damage = 20 ;
      CurBullet->mine = FALSE;
      CurBullet->owner = -1;
      CurBullet->bullet_lifetime        = ItemMap[ weapon_item_type ].item_gun_bullet_lifetime;
      CurBullet->angle_change_rate      = ItemMap[ weapon_item_type ].item_gun_angle_change;
      CurBullet->fixed_offset           = ItemMap[ weapon_item_type ].item_gun_fixed_offset;
      CurBullet->ignore_wall_collisions = ItemMap[ weapon_item_type ].item_gun_bullet_ignore_wall_collisions;
      // CurBullet->owner_pos = & ( Me [ PlayerNum ] .pos );
      CurBullet->owner_pos = NULL ;
      CurBullet->time_in_frames = 0;
      CurBullet->time_in_seconds = 0;
      CurBullet->was_reflected = FALSE;
      CurBullet->reflect_other_bullets   = ItemMap[ weapon_item_type ].item_gun_bullet_reflect_other_bullets;
      CurBullet->pass_through_explosions = ItemMap[ weapon_item_type ].item_gun_bullet_pass_through_explosions;
      CurBullet->pass_through_hit_bodies = ItemMap[ weapon_item_type ].item_gun_bullet_pass_through_hit_bodies;
      CurBullet->miss_hit_influencer = UNCHECKED ;
      memset( CurBullet->total_miss_hit , UNCHECKED , MAX_ENEMYS_ON_SHIP );
      
      //--------------------
      // Depending on whether this is a real bullet (-1 given as parameter)
      // or not, we assign this bullet the appropriate to-hit propability
      //
      CurBullet->to_hit = 150 ;
      
      //--------------------
      // Maybe the bullet has some magic properties.  This is handled here.
      //
      CurBullet->freezing_level = 0; 
      CurBullet->poison_duration = 0;
      CurBullet->poison_damage_per_sec = 0;
      CurBullet->paralysation_duration = 0;
      
      speed.x = 0.0;
      speed.y = 0.0;
      
      switch ( *AutogunType )
	{
	case ISO_AUTOGUN_W:
	  speed.x = -1.0;
	  break;
	case ISO_AUTOGUN_E:
	  speed.x =  1.0;
	  break;
	case ISO_AUTOGUN_N:
	  speed.y = -1.0;
	  break;
	case ISO_AUTOGUN_S:
	  speed.y = +1.0;
	  break;
	default:
	  fprintf ( stderr, "\n*AutogunType: '%d'.\n" , *AutogunType );
	  GiveStandardErrorMessage ( "WorkLevelGuns(...)" , "\
There seems to be an autogun in the autogun list of this level, but it\n\
is not really an autogun.  Instead it's something else.",
				     PLEASE_INFORM, IS_FATAL );
	  break;
	}

      CurBullet -> pos.x = autogunx + speed.x * 0.75 ;
      CurBullet -> pos.y = autoguny + speed.y * 0.75 ;
      CurBullet -> pos.z = Me [ PlayerNum ] . pos . z ;
      
      
      //--------------------
      // It might happen, that this is not a normal shot, but rather the
      // swing of a melee weapon.  Then of course, we should make a swing
      // and not start in this direction, but rather somewhat 'before' it,
      // so that the rotation will hit the target later.
      //
      // RotateVectorByAngle ( & speed , ItemMap[ weapon_item_type ].item_gun_start_angle_modifier );
      
      speed_norm = sqrt (speed.x * speed.x + speed.y * speed.y);
      CurBullet->speed.x = (speed.x/speed_norm);
      CurBullet->speed.y = (speed.y/speed_norm);
      
      //--------------------
      // Now we determine the angle of rotation to be used for
      // the picture of the bullet itself
      //
      
      CurBullet->angle= - ( atan2 (speed.y,  speed.x) * 180 / M_PI + 90 + 45 );
      
      DebugPrintf( 1 , "\nWorkLevelGuns(...) : Phase of bullet=%d." , CurBullet->phase );
      DebugPrintf( 1 , "\nWorkLevelGuns(...) : angle of bullet=%f." , CurBullet->angle );
      
      CurBullet->speed.x *= BulletSpeed;
      CurBullet->speed.y *= BulletSpeed;
      

    } // for

}; // void WorkLevelGuns ( void )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
int
position_collides_with_this_obstacle ( float x , float y , obstacle* our_obstacle )
{
  float upper_border;
  float lower_border;
  float left_border;
  float right_border;
  int obs_type = our_obstacle -> type ;

  //--------------------
  // First we check for non-existent obstacle.
  //
  if ( obs_type <= -1 ) return ( FALSE );

  //--------------------
  // Now we check if maybe it's a door.  Doors should get ignored, 
  // if the global ignore_doors_for_collisions flag is set.  This
  // flag is introduced the reduce function overhead, especially
  // in the recursions used here and there.
  //
  if ( global_ignore_doors_for_collisions_flag )
    {
      if ( ( obs_type >= ISO_H_DOOR_000_OPEN ) && ( obs_type <= ISO_V_DOOR_100_OPEN ) )
	return ( FALSE );
      // ISO_H_DOOR_000_OPEN = 6,
      // ISO_H_DOOR_025_OPEN = 7,
      // ISO_H_DOOR_050_OPEN = 8,
      // ISO_H_DOOR_075_OPEN = 9,
      // ISO_H_DOOR_100_OPEN = 10,
      // ISO_V_DOOR_000_OPEN = 11,
      // ISO_V_DOOR_025_OPEN = 12,
      // ISO_V_DOOR_050_OPEN = 13,
      // ISO_V_DOOR_075_OPEN = 14,
      // ISO_V_DOOR_100_OPEN = 15,
    }

  //--------------------
  // If the obstacle doesn't even have a collision rectangle, then
  // of course it's easy, cause then there can't be any collsision
  //
  if ( obstacle_map [ obs_type ] . block_area_type == COLLISION_TYPE_NONE )
    return ( FALSE );
  /*
  if ( obstacle_map [ obs_type ] . block_area_type != COLLISION_TYPE_RECTANGLE )
    {
      fprintf ( stderr, "\n\nCollision_area_type: %d.\n" , obstacle_map [ obs_type ] . block_area_type );
      GiveStandardErrorMessage ( "position_collides_with_this_obstacle(...)" , "\
Error:  Unsupported type of collision area given.",
				 PLEASE_INFORM, IS_FATAL );
    }
  */

  //--------------------
  // first we find out where the borders of our collision rectangle
  // lie...
  //
  upper_border = our_obstacle -> pos . x - obstacle_map [ obs_type ] . block_area_parm_1 / 2.0 ;
  lower_border = our_obstacle -> pos . x + obstacle_map [ obs_type ] . block_area_parm_1 / 2.0 ;
  left_border = our_obstacle -> pos . y - obstacle_map [ obs_type ] . block_area_parm_2 / 2.0 ;
  right_border = our_obstacle -> pos . y + obstacle_map [ obs_type ] . block_area_parm_2 / 2.0 ;

  //--------------------
  // Now if the position lies inside the collision rectangle, then there's
  // a collion.  Otherwise not.
  //
  if ( ( x > our_obstacle -> pos . x + obstacle_map [ obs_type ] . upper_border ) && 
       ( x < our_obstacle -> pos . x + obstacle_map [ obs_type ] . lower_border ) && 
       ( y > our_obstacle -> pos . y + obstacle_map [ obs_type ] . left_border ) && 
       ( y < our_obstacle -> pos . y + obstacle_map [ obs_type ] . right_border ) )
    return ( TRUE );

  return ( FALSE );
}; // int position_collides_with_this_obstacle ( float x , float y , obstacle* our_obstacle )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
int 
position_collides_with_obstacles_on_square ( float x, float y , int x_tile , int y_tile , Level PassLevel )
{
  int glue_index;
  int obstacle_index;

  //--------------------
  // We take a look whether the position given in the parameter is 
  // blocked by an obstacle glued close to this square...
  //
  for ( glue_index = 0 ; glue_index < MAX_OBSTACLES_GLUED_TO_ONE_MAP_TILE ; glue_index ++ )
    {
      if ( PassLevel -> map [ y_tile ] [ x_tile ] . obstacles_glued_to_here [ glue_index ] == (-1) ) break;
      obstacle_index = PassLevel -> map [ y_tile ] [ x_tile ] . obstacles_glued_to_here [ glue_index ] ;

      if ( position_collides_with_this_obstacle ( x , y , & ( PassLevel -> obstacle_list [ obstacle_index ] ) ) ) 
	return ( TRUE );

    }

  return ( FALSE ) ;
  
}; // int position_collides_with_obstacles_on_square ( int x , int y )

/* ---------------------------------------------------------------------- 
 * This function checks if a given location lies within a wall or not.
 *
 * A return value of CENTER means that the location is passable in that sense.
 * while any directions and (-1) indicate that it is not so and in case
 * of a direction returned, it's the direction into which the droid
 * should be pushed to resolve the collision (with a door).
 * ---------------------------------------------------------------------- */
int
IsPassable ( float x , float y , int z )
{
  Level PassLevel = curShip . AllLevels [ z ] ;
  // int x_tile_start, y_tile_start;
  // int x_tile_end, y_tile_end;
  int x_tile, y_tile;
  
  //--------------------
  // We take a look whether the position given in the parameter is 
  // blocked by an obstacle ON ANY SQUARE WITHIN A 3x3 TILE RECTANGLE.
  //
  /*
  x_tile_start = rintf ( x ) -1         ; y_tile_start = rintf ( y ) -1 ;
  x_tile_end   = x_tile_start + 2       ; y_tile_end   = y_tile_start + 2 ;
  if ( x_tile_start < 0 ) x_tile_start = 0 ; 
  if ( y_tile_start < 0 ) y_tile_start = 0 ; 
  if ( x_tile_end >= PassLevel -> xlen ) x_tile_end = PassLevel->xlen -1 ;
  if ( y_tile_end >= PassLevel -> ylen ) y_tile_end = PassLevel->ylen -1 ; 

  for ( x_tile = x_tile_start ; x_tile < x_tile_end ; x_tile ++ )
    {

      // DebugPrintf ( 0 , " %d " , x_tile );
      for ( y_tile = y_tile_start ; y_tile < y_tile_end ; y_tile ++ )
	{
	  if ( position_collides_with_obstacles_on_square ( x , y , x_tile , y_tile , PassLevel ) ) return ( -1 );
	}
    }
  */

  //--------------------
  // It should be safe to skip any out-of-map checks here, because the Tux
  // or any other object can't get this close to the map borders anyway.
  //
  x_tile = rintf ( x ) ;
  y_tile = rintf ( y ) ;

  //--------------------
  // Now if the position in question is directly outside of the borders of the
  // current level, then this position can't be passable in the normal sense, 
  // or at least it wouldn't make sense for anything to pass there, so we can
  // as well declare is not passable outright.
  //
  if ( ( x < 0 ) || ( y < 0 ) || ( x_tile >= PassLevel -> xlen ) || ( y_tile >= PassLevel -> ylen ) )
    return ( FALSE );

  //--------------------
  // Now if the x/y position is at least inside the map, we need to look closer
  //
  if ( x_tile > 0 )
    {
      if ( position_collides_with_obstacles_on_square ( x , y , x_tile - 1 , y_tile , PassLevel ) ) 
	return ( FALSE );

      if ( y_tile > 0 )
	if ( position_collides_with_obstacles_on_square ( x , y , x_tile - 1 , y_tile - 1 , PassLevel ) ) 
	  return ( FALSE );

      if ( y_tile + 1 < PassLevel -> ylen )
	if ( position_collides_with_obstacles_on_square ( x , y , x_tile - 1 , y_tile + 1 , PassLevel ) ) 
	  return ( FALSE );
    }

  if ( x_tile + 1 < PassLevel -> xlen )
    {
      if ( position_collides_with_obstacles_on_square ( x , y , x_tile + 1 , y_tile     , PassLevel ) ) 
	return ( FALSE );

      if ( y_tile + 1 < PassLevel -> ylen )
	if ( position_collides_with_obstacles_on_square ( x , y , x_tile + 1 , y_tile + 1 , PassLevel ) ) 
	  return ( FALSE );

      if ( y_tile > 0 )
	if ( position_collides_with_obstacles_on_square ( x , y , x_tile + 1 , y_tile - 1 , PassLevel ) ) 
	  return ( FALSE );
    }

  if ( position_collides_with_obstacles_on_square ( x , y , x_tile , y_tile , PassLevel ) ) 
    return ( FALSE );

  if ( y_tile + 1 < PassLevel -> ylen )
    if ( position_collides_with_obstacles_on_square ( x , y , x_tile     , y_tile + 1 , PassLevel ) ) 
      return ( FALSE );

  if ( y_tile > 0 )
    if ( position_collides_with_obstacles_on_square ( x , y , x_tile     , y_tile - 1 , PassLevel ) ) 
      return ( FALSE );

  return ( TRUE );

}; // int IsPassable ( ... )

/* ----------------------------------------------------------------------
 * This function determines wether a given object on x/y is visible to
 * the 001 or not (due to some walls or something in between
 * 
 * Return values are TRUE or FALSE accodinly
 *
 * ---------------------------------------------------------------------- */
int
IsVisible ( GPS objpos , int PlayerNum )
{

  //--------------------
  // For the purpose of visibility checking, we might as well exclude objects
  // that are too far away to ever be visible and thereby save some checks of
  // longer lines on the map, that wouldn't be nescessary or helpful anyway.
  //
  if ( ( fabsf ( Me [ PlayerNum ] . pos . x - objpos -> x ) > 6.5 ) ||
       ( fabsf ( Me [ PlayerNum ] . pos . y - objpos -> y ) > 6.5 ) )
    return ( FALSE );

  //--------------------
  // So if the object in question is close enough to be visible, we'll do the
  // actual check and see if the line of sight is free or blocked, a rather
  // time-consuming and often re-iterated process.  (Maybe some do-it-every-
  // -10th-frame-only code could be added here later... and in the meantime
  // old values could be used from a stored flag?!
  //
  return ( DirectLineWalkable( objpos -> x , objpos -> y , 
			       Me [ PlayerNum ] . pos . x , Me [ PlayerNum ] . pos . y , 
			       objpos -> z ) )  ;

}; // int IsVisible( Point objpos )

/* ----------------------------------------------------------------------
 * This function moves all periodically changing map tiles...
 * ---------------------------------------------------------------------- */
void
AnimateCyclingMapTiles (void)
{
  AnimateRefresh();
  // AnimateConsumer();
  AnimateTeleports();
}; // void AnimateCyclingMapTiles (void)


/*----------------------------------------------------------------------
 *  get rid of all "holes" (0/0) in waypoint-list, which are due to
 * old waypoint-handling
 *----------------------------------------------------------------------*/
void
PurifyWaypointList (level* Lev)
{
  int i;
  waypoint *WpList, *ThisWp;

  WpList = Lev->AllWaypoints;
  for (i=0; i < Lev->num_waypoints; i++)
    {
      ThisWp = &WpList[i];
      if (ThisWp->x == 0 && ThisWp->y == 0)
	{
	  DeleteWaypoint (Lev, i);
	  i--; // this one was deleted, so dont' step on
	}
    }

  return;

} // PurifyWaypointList()

/*----------------------------------------------------------------------
 * delete given waypoint num (and all its connections) on level Lev
 *----------------------------------------------------------------------*/
void
DeleteWaypoint (level *Lev, int num)
{
  int i, j;
  waypoint *WpList, *ThisWp;
  int wpmax;

  WpList = Lev->AllWaypoints;
  wpmax = Lev->num_waypoints - 1;
  
  // is this the last one? then just delete
  if (num == wpmax)
    WpList[num].num_connections = 0;
  else // otherwise shift down all higher waypoints
    memcpy (&WpList[num], &WpList[num+1], (wpmax - num) * sizeof(waypoint) );

  // now there's one less:
  Lev->num_waypoints --;
  wpmax --;

  // now adjust the remaining wp-list to the changes:
  ThisWp = WpList;
  for (i=0; i < Lev->num_waypoints; i++, ThisWp++)
    for (j=0; j < ThisWp->num_connections; j++)
      {
	// eliminate all references to this waypoint
	if (ThisWp->connections[j] == num)
	  {
	    // move all connections after this one down
	    memcpy (&(ThisWp->connections[j]), &(ThisWp->connections[j+1]), 
		    (ThisWp->num_connections-1 - j)*sizeof(int));
	    ThisWp->num_connections --;
	    j --;  // just to be sure... check the next connection as well...(they have been shifted!)
	    continue;
	  }
	// adjust all connections to the shifted waypoint-numbers
	else if (ThisWp->connections[j] > num)
	  ThisWp->connections[j] --;
	
      } // for j < num_connections

  return;

} // DeleteWaypoint()

/*----------------------------------------------------------------------
 * create a new empty waypoint on position x/y
 *----------------------------------------------------------------------*/
void
CreateWaypoint (level *Lev, int x, int y)
{
  int num;

  if (Lev->num_waypoints == MAXWAYPOINTS)
    {
      DebugPrintf (0, "WARNING: maximal number of waypoints (%d) reached on this level!!\n",
		   MAXWAYPOINTS);
      DebugPrintf (0, "... cannot insert any more, sorry!\n");
      return;
    }

  num = Lev->num_waypoints;
  Lev->num_waypoints ++;

  Lev->AllWaypoints[num].x = x;
  Lev->AllWaypoints[num].y = y;
  Lev->AllWaypoints[num].num_connections = 0;

  return;
} // CreateWaypoint()





#undef _map_c
