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

#define MAP_BEGIN_STRING	"beginning_of_map"
#define WP_BEGIN_STRING		"wp"
#define LEVEL_END_STRING	"end_of_level"

#define AREA_NAME_STRING "Area name=\""
#define LEVEL_NAME_STRING "Name of this level="
#define LEVEL_ENTER_COMMENT_STRING "Comment of the Influencer on entering this level=\""
#define BACKGROUND_SONG_NAME_STRING "Name of background song for this level="
#define MAP_END_STRING "End of pure map information for this level"

#define ITEMS_SECTION_BEGIN_STRING "Start of pure items information for this level"
#define ITEMS_SECTION_END_STRING "End of pure items information for this level"
#define ITEM_CODE_STRING "New item: type="
#define ITEM_POS_X_STRING " X="
#define ITEM_POS_Y_STRING " Y="
#define ITEM_AC_BONUS_STRING " AC="
#define ITEM_DAMAGE_STRING " DoDamage="
#define ITEM_DAMAGE_MODIFIER_STRING " ModifyDamage="
#define ITEM_MAX_DURATION_STRING " MaxDur="
#define ITEM_CUR_DURATION_STRING " CurDur="
#define ITEM_GOLD_AMOUNT_STRING " Gold="
#define ITEM_MULTIPLICITY_STRING " Multiplicity="
#define ITEM_PREFIX_CODE_STRING " PrefixCode="
#define ITEM_SUFFIX_CODE_STRING " SuffixCode="
#define ITEM_BONUS_TO_STR_STRING " StrBon="
#define ITEM_BONUS_TO_DEX_STRING " StrDex="
#define ITEM_BONUS_TO_VIT_STRING " StrVit="
#define ITEM_BONUS_TO_MAG_STRING " StrMag="
#define ITEM_BONUS_TO_ALLATT_STRING " StrAllAtt="
#define ITEM_BONUS_TO_LIFE_STRING " StrLife="
#define ITEM_BONUS_TO_FORCE_STRING " StrForce="
#define ITEM_BONUS_TO_TOHIT_STRING " StrToHit="
#define ITEM_BONUS_TO_ACDAM_STRING " StrACDam="
#define ITEM_BONUS_TO_RESELE_STRING " ResEle="
#define ITEM_BONUS_TO_RESFIR_STRING " ResFir="
#define ITEM_BONUS_TO_RESFOR_STRING " ResFor="

#define CHEST_ITEMS_SECTION_BEGIN_STRING "Start of pure chest item information for this level"
#define CHEST_ITEMS_SECTION_END_STRING "End of pure chest item information for this level"

#define STATEMENT_BEGIN_STRING "Start of pure statement information for this level"
#define STATEMENT_END_STRING "End of pure statement information for this level"
#define X_POSITION_OF_STATEMENT_STRING "PosX="
#define Y_POSITION_OF_STATEMENT_STRING "PosY="
#define STATEMENT_ITSELF_ANNOUNCE_STRING "Statement=\""

#define MAP_LABEL_BEGIN_STRING "Start of pure map label information for this level"
#define MAP_LABEL_END_STRING "End of pure map label information for this level"
#define X_POSITION_OF_LABEL_STRING "label.pos.x="
#define Y_POSITION_OF_LABEL_STRING "label.pos.y="
#define LABEL_ITSELF_ANNOUNCE_STRING "label.label_name=\""

#define CODEPANEL_SECTION_BEGIN_STRING "Start of pure codepanel information for this level"
#define CODEPANEL_SECTION_END_STRING "End of pure codepanel information for this level"
#define CODEPANEL_CODE_ANNOUNCE_STRING "Secret Code=\""
#define POSITION_X_OF_CODEPANEL_STRING "PanelposX="
#define POSITION_Y_OF_CODEPANEL_STRING "PanelposY="

#define BIG_MAP_INSERT_SECTION_BEGIN_STRING "Start of big graphics insert information for this level"
#define BIG_MAP_INSERT_SECTION_END_STRING "End of big graphics insert information for this level"
#define POSITION_X_OF_BIG_MAP_INSERT_STRING "BigGraphicsInsertPosX="
#define POSITION_Y_OF_BIG_MAP_INSERT_STRING "BigGraphicsInsertPosY="
#define BIG_MAP_INSERT_TYPE_STRING "BigGraphicsInsertType="

#define SPECIAL_FORCE_INDICATION_STRING "SpecialForce: Type="

void
TranslateToHumanReadable ( Uint16* HumanReadable , map_tile* MapInfo, int LineLength , Level Lev , int CurrentLine);
// void TranslateToHumanReadable ( Uint16* HumanReadable , Uint16* MapInfo, int LineLength , Level Lev , int CurrentLine);
void GetThisLevelsDroids( char* SectionPointer );
Level DecodeLoadedLeveldata ( char *data );
int IsWallBlock ( int block );

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
	  PositionPointer->x = ResolveLevel->labels [ i ] . pos . x ;
	  PositionPointer->y = ResolveLevel->labels [ i ] . pos . y ;
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
 color of this level: %u \n" , 
	  &(loadlevel->levelnum), &(loadlevel->xlen),
	  &(loadlevel->ylen), &(loadlevel->color));

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
 * Next we extract the codepanels of this level WITHOUT destroying
 * or damaging the data in the process!
 * ---------------------------------------------------------------------- */
void 
DecodeCodepanelsOfThisLevel ( Level loadlevel , char* DataPointer )
{
  int i;
  char PreservedLetter;
  char* CodepanelPointer;
  char* CodepanelSectionBegin;
  char* CodepanelSectionEnd;
  int NumberOfCodepanelsInThisLevel;

  //--------------------
  // First we initialize the codepanel arrays with 'empty' information
  //
  for ( i = 0 ; i < MAX_CODEPANELS_PER_LEVEL ; i ++ )
    {
      loadlevel->CodepanelList[ i ].x = ( -1 ) ;
      loadlevel->CodepanelList[ i ].y = ( -1 ) ;
      loadlevel->CodepanelList[ i ].Secret_Code = "nonono" ;
    }

  //--------------------
  // We look for the beginning and end of the codepanel section
  //
  CodepanelSectionBegin = LocateStringInData( DataPointer , CODEPANEL_SECTION_BEGIN_STRING );
  CodepanelSectionEnd = LocateStringInData( DataPointer , CODEPANEL_SECTION_END_STRING );

  //--------------------
  // We add a terminator at the end, but ONLY TEMPORARY.  The damage will be restored later!
  //
  PreservedLetter=CodepanelSectionEnd[0];
  CodepanelSectionEnd[0]=0;
  NumberOfCodepanelsInThisLevel = CountStringOccurences ( CodepanelSectionBegin , CODEPANEL_CODE_ANNOUNCE_STRING ) ;
  DebugPrintf( 1 , "\nNumber of codepanels found in this level : %d." , NumberOfCodepanelsInThisLevel );


  //--------------------
  // Now we decode all the codepanel information
  //
  CodepanelPointer=CodepanelSectionBegin;
  for ( i = 0 ; i < NumberOfCodepanelsInThisLevel ; i ++ )
    {
      CodepanelPointer = strstr ( CodepanelPointer + 1 , POSITION_X_OF_CODEPANEL_STRING );
      ReadValueFromString( CodepanelPointer , POSITION_X_OF_CODEPANEL_STRING , "%d" , 
			   &(loadlevel->CodepanelList[ i ].x) , CodepanelSectionEnd );
      ReadValueFromString( CodepanelPointer , POSITION_Y_OF_CODEPANEL_STRING , "%d" , 
			   &(loadlevel->CodepanelList[ i ].y) , CodepanelSectionEnd );
      loadlevel->CodepanelList[ i ].Secret_Code = 
	ReadAndMallocStringFromData ( CodepanelPointer , CODEPANEL_CODE_ANNOUNCE_STRING , "\"" ) ;

      DebugPrintf( 1 , "\nPosX=%d PosY=%d Codepanel=\"%s\"" , loadlevel->CodepanelList[ i ].x , 
		   loadlevel->CodepanelList[ i ].y , loadlevel->CodepanelList[ i ].Secret_Code );
    }

  //--------------------
  // Now we repair the damage done to the loaded level data
  //
  CodepanelSectionEnd[0]=PreservedLetter;

}; // void DecodeCodepanelsOfThisLevel ( Level loadlevel , char* DataPointer );


/* ----------------------------------------------------------------------
 * Next we extract the codepanels of this level WITHOUT destroying
 * or damaging the data in the process!
 * ---------------------------------------------------------------------- */
void 
DecodeBigMapInsertsOfThisLevel ( Level loadlevel , char* DataPointer )
{
  int i;
  char PreservedLetter;
  char* MapInsertPointer;
  char* MapInsertSectionBegin;
  char* MapInsertSectionEnd;
  int NumberOfMapInsertsInThisLevel;

  //----------------------------------------------------------------------
  // From here on we take apart the big map graphics inserts of the 
  // loaded level...
  //----------------------------------------------------------------------

  //--------------------
  // First we initialize the graphics insert array with 'empty' information
  //
  for ( i = 0 ; i < MAX_MAP_INSERTS_PER_LEVEL ; i ++ )
    {
      loadlevel->MapInsertList [ i ] . type = ( -1 ) ;
      loadlevel->MapInsertList [ i ] . pos . x = ( -1 ) ;
      loadlevel->MapInsertList [ i ] . pos . y = ( -1 ) ;
    }

  //--------------------
  // We look for the beginning and end of the codepanel section
  //
  MapInsertSectionBegin = LocateStringInData( DataPointer , BIG_MAP_INSERT_SECTION_BEGIN_STRING );
  MapInsertSectionEnd = LocateStringInData( DataPointer , BIG_MAP_INSERT_SECTION_END_STRING );

  //--------------------
  // We add a terminator at the end, but ONLY TEMPORARY.  The damage will be restored later!
  //
  PreservedLetter=MapInsertSectionEnd[0];
  MapInsertSectionEnd[0]=0;
  NumberOfMapInsertsInThisLevel = CountStringOccurences ( MapInsertSectionBegin , POSITION_X_OF_BIG_MAP_INSERT_STRING ) ;
  DebugPrintf( 1 , "\nNumber of big map inserts found in this level : %d." , NumberOfMapInsertsInThisLevel );


  //--------------------
  // Now we decode all the codepanel information
  //
  MapInsertPointer=MapInsertSectionBegin;
  for ( i = 0 ; i < NumberOfMapInsertsInThisLevel ; i ++ )
    {
      MapInsertPointer = strstr ( MapInsertPointer + 1 , POSITION_X_OF_BIG_MAP_INSERT_STRING );
      ReadValueFromString( MapInsertPointer , POSITION_X_OF_BIG_MAP_INSERT_STRING , "%d" , 
			   &(loadlevel->MapInsertList[ i ].pos.x) , MapInsertSectionEnd );
      ReadValueFromString( MapInsertPointer , POSITION_Y_OF_BIG_MAP_INSERT_STRING , "%d" , 
			   &(loadlevel->MapInsertList[ i ].pos.y) , MapInsertSectionEnd );

      ReadValueFromString( MapInsertPointer , BIG_MAP_INSERT_TYPE_STRING , "%d" , 
			   &(loadlevel->MapInsertList[ i ].type ) , MapInsertSectionEnd );

      DebugPrintf( 1 , "\nPosX=%d PosY=%d MapInsertType=%d" , loadlevel->MapInsertList[ i ].pos.x , 
		   loadlevel->MapInsertList[ i ].pos.y , loadlevel->MapInsertList[ i ].type );
    }

  //--------------------
  // For debugging purposes now the complete list of map inserts:
  //
  /*
  DebugPrintf( 0 , "\n\nNOW THE COMPLETE LIST: \n\n" );
  for ( i = 0 ; i < MAX_MAP_INSERTS_PER_LEVEL ; i ++ )
    {
      DebugPrintf( 0 , "\nPosX=%d PosY=%d MapInsertType=%d" , loadlevel->MapInsertList[ i ].pos.x , 
		   loadlevel->MapInsertList[ i ].pos.y , loadlevel->MapInsertList[ i ].type );
    }
  */

  //--------------------
  // Now we repair the damage done to the loaded level data
  //
  MapInsertSectionEnd[0]=PreservedLetter;


  /*
  //--------------------
  // First we initialize the codepanel arrays with 'empty' information
  //
  for ( i = 0 ; i < MAX_CODEPANELS_PER_LEVEL ; i ++ )
    {
      loadlevel->CodepanelList[ i ].x = ( -1 ) ;
      loadlevel->CodepanelList[ i ].y = ( -1 ) ;
      loadlevel->CodepanelList[ i ].Secret_Code = "nonono" ;
    }

  //--------------------
  // We look for the beginning and end of the codepanel section
  //
  CodepanelSectionBegin = LocateStringInData( DataPointer , CODEPANEL_SECTION_BEGIN_STRING );
  CodepanelSectionEnd = LocateStringInData( DataPointer , CODEPANEL_SECTION_END_STRING );

  //--------------------
  // We add a terminator at the end, but ONLY TEMPORARY.  The damage will be restored later!
  //
  PreservedLetter=CodepanelSectionEnd[0];
  CodepanelSectionEnd[0]=0;
  NumberOfCodepanelsInThisLevel = CountStringOccurences ( CodepanelSectionBegin , CODEPANEL_CODE_ANNOUNCE_STRING ) ;
  DebugPrintf( 1 , "\nNumber of codepanels found in this level : %d." , NumberOfCodepanelsInThisLevel );


  //--------------------
  // Now we decode all the codepanel information
  //
  CodepanelPointer=CodepanelSectionBegin;
  for ( i = 0 ; i < NumberOfCodepanelsInThisLevel ; i ++ )
    {
      CodepanelPointer = strstr ( CodepanelPointer + 1 , POSITION_X_OF_CODEPANEL_STRING );
      ReadValueFromString( CodepanelPointer , POSITION_X_OF_CODEPANEL_STRING , "%d" , 
			   &(loadlevel->CodepanelList[ i ].x) , CodepanelSectionEnd );
      ReadValueFromString( CodepanelPointer , POSITION_Y_OF_CODEPANEL_STRING , "%d" , 
			   &(loadlevel->CodepanelList[ i ].y) , CodepanelSectionEnd );
      loadlevel->CodepanelList[ i ].Secret_Code = 
	ReadAndMallocStringFromData ( CodepanelPointer , CODEPANEL_CODE_ANNOUNCE_STRING , "\"" ) ;

      DebugPrintf( 1 , "\nPosX=%d PosY=%d Codepanel=\"%s\"" , loadlevel->CodepanelList[ i ].x , 
		   loadlevel->CodepanelList[ i ].y , loadlevel->CodepanelList[ i ].Secret_Code );
    }

  //--------------------
  // Now we repair the damage done to the loaded level data
  //
  CodepanelSectionEnd[0]=PreservedLetter;
  */

}; // void DecodeBigMapInsertsOfThisLevel ( Level loadlevel , char* DataPointer );


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
generate_wallobstacles_from_level_map ( int level_num )
{
  int x, y;
  level* loadlevel = curShip . AllLevels [ level_num ] ;
  int obstacle_counter = 0 ;
  int i;

  //--------------------
  // First we must erase all obstacle information, that might still be
  // lurking around in the uninitialized arrays...
  //
  for ( i = 0 ; i < MAX_OBSTACLES_ON_MAP ; i ++ )
    {
      loadlevel -> obstacle_list [ i ] . type = ( -1 ) ;
    }

  //--------------------
  // Now we try to make obstacles out of the former 'wall' information,
  // that was coded into the floor tiles.
  //
  for ( y = 0 ; y < loadlevel -> ylen ; y++ )
    {
      for ( x = 0 ; x < loadlevel -> xlen ; x++ )
	{
	  switch ( loadlevel -> map [ y ] [ x ]  . floor_value )
	    {
	    case H_WALL:
	      loadlevel -> obstacle_list [ obstacle_counter ] . type = ISO_H_WALL ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . x = x + 0.5 ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . y = y + 1.0 ;
	      obstacle_counter ++ ;
	      break;
	    case H_SHUT_DOOR:
	    case H_HALF_DOOR1:
	    case H_HALF_DOOR2:
	    case H_HALF_DOOR3:
	    case H_OPEN_DOOR:
	      loadlevel -> obstacle_list [ obstacle_counter ] . type = ISO_H_DOOR_000_OPEN ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . x = x + 0.5 ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . y = y + 1.0 ;
	      obstacle_counter ++ ;
	      break;
	    case V_WALL:
	      loadlevel -> obstacle_list [ obstacle_counter ] . type = ISO_V_WALL ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . x = x + 1.0 ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . y = y + 0.5 ;
	      obstacle_counter ++ ;
	      break;
	    case CORNER_LD:
	      loadlevel -> obstacle_list [ obstacle_counter ] . type = ISO_V_WALL ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . x = x + 1.0 ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . y = y + 0.5 ;
	      obstacle_counter ++ ;
	      break;
	    case CORNER_RD:
	      loadlevel -> obstacle_list [ obstacle_counter ] . type = ISO_V_WALL ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . x = x + 1.0 ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . y = y + 0.5 ;
	      obstacle_counter ++ ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . type = ISO_H_WALL ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . x = x + 0.5 ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . y = y + 1.0 ;
	      obstacle_counter ++ ;
	      break;
	    case CORNER_RU:
	      loadlevel -> obstacle_list [ obstacle_counter ] . type = ISO_H_WALL ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . x = x + 0.5 ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . y = y + 1.0 ;
	      obstacle_counter ++ ;
	      break;
	    case CORNER_LU:
	      break;
	    case T_D:
	      loadlevel -> obstacle_list [ obstacle_counter ] . type = ISO_V_WALL ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . x = x + 1.0 ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . y = y + 0.5 ;
	      obstacle_counter ++ ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . type = ISO_H_WALL ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . x = x + 0.5 ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . y = y + 1.0 ;
	      obstacle_counter ++ ;
	      break;
	    case T_U:
	      loadlevel -> obstacle_list [ obstacle_counter ] . type = ISO_H_WALL ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . x = x + 0.5 ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . y = y + 1.0 ;
	      obstacle_counter ++ ;
	      break;
	    case T_R:
	      loadlevel -> obstacle_list [ obstacle_counter ] . type = ISO_V_WALL ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . x = x + 1.0 ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . y = y + 0.5 ;
	      obstacle_counter ++ ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . type = ISO_H_WALL ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . x = x + 0.5 ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . y = y + 1.0 ;
	      obstacle_counter ++ ;
	      break;
	    case T_L:
	      loadlevel -> obstacle_list [ obstacle_counter ] . type = ISO_V_WALL ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . x = x + 1.0 ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . y = y + 0.5 ;
	      obstacle_counter ++ ;
	      break;
	    case V_SHUT_DOOR:
	    case V_HALF_DOOR1:
	    case V_HALF_DOOR2:
	    case V_HALF_DOOR3:
	    case V_OPEN_DOOR:
	      loadlevel -> obstacle_list [ obstacle_counter ] . type = ISO_V_DOOR_000_OPEN ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . x = x + 1.0 ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . y = y + 0.5 ;
	      obstacle_counter ++ ;
	      break;

	    case REFRESH1:
	    case REFRESH2:
	    case REFRESH3:
	    case REFRESH4:
	      loadlevel -> obstacle_list [ obstacle_counter ] . type = ISO_REFRESH_1 ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . x = x + 0.5 ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . y = y + 0.5 ;
	      obstacle_counter ++ ;
	      break;

	    case TELE_1:
	    case TELE_2:
	    case TELE_3:
	    case TELE_4:
	      loadlevel -> obstacle_list [ obstacle_counter ] . type = ISO_TELEPORTER_1 ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . x = x + 0.5 ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . y = y + 0.5 ;
	      obstacle_counter ++ ;
	      break;

	    case LOCKED_H_SHUT_DOOR:
	      loadlevel -> obstacle_list [ obstacle_counter ] . type = ISO_H_DOOR_LOCKED ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . x = x + 0.5 ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . y = y + 1.0 ;
	      obstacle_counter ++ ;
	      break;
	    case LOCKED_V_SHUT_DOOR:
	      loadlevel -> obstacle_list [ obstacle_counter ] . type = ISO_V_DOOR_LOCKED ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . x = x + 1.0 ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . y = y + 0.5 ;
	      obstacle_counter ++ ;
	      break;

	    case CHEST_U:
	      loadlevel -> obstacle_list [ obstacle_counter ] . type = ISO_H_CHEST_CLOSED ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . x = x + 0.5 ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . y = y + 0.25 ;
	      obstacle_counter ++ ;
	      break;

	    case CHEST_D:
	      loadlevel -> obstacle_list [ obstacle_counter ] . type = ISO_H_CHEST_CLOSED ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . x = x + 0.5 ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . y = y + 0.75 ;
	      obstacle_counter ++ ;
	      break;

	    case CHEST_R:
	      loadlevel -> obstacle_list [ obstacle_counter ] . type = ISO_V_CHEST_CLOSED ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . x = x + 0.75 ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . y = y + 0.5 ;
	      obstacle_counter ++ ;
	      break;

	    case CHEST_L:
	      loadlevel -> obstacle_list [ obstacle_counter ] . type = ISO_V_CHEST_CLOSED ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . x = x + 0.25 ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . y = y + 0.5 ;
	      obstacle_counter ++ ;
	      break;

	    case AUTOGUN_R:
	      loadlevel -> obstacle_list [ obstacle_counter ] . type = ISO_AUTOGUN_E ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . x = x + 0.5;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . y = y + 0.5;
	      obstacle_counter ++ ;
	      break;

	    case AUTOGUN_L:
	      loadlevel -> obstacle_list [ obstacle_counter ] . type = ISO_AUTOGUN_W ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . x = x + 0.5;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . y = y + 0.5;
	      obstacle_counter ++ ;
	      break;

	    case AUTOGUN_U:
	      loadlevel -> obstacle_list [ obstacle_counter ] . type = ISO_AUTOGUN_N ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . x = x + 0.5;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . y = y + 0.5;
	      obstacle_counter ++ ;
	      break;

	    case AUTOGUN_D:
	      loadlevel -> obstacle_list [ obstacle_counter ] . type = ISO_AUTOGUN_S ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . x = x + 0.5;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . y = y + 0.5;
	      obstacle_counter ++ ;
	      break;

	    case CAVE_V_WALL:
	      loadlevel -> obstacle_list [ obstacle_counter ] . type = ISO_CAVE_WALL_V ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . x = x + 0.5 ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . y = y + 0.5 ;
	      obstacle_counter ++ ;
	      break;
	    case CAVE_H_WALL:
	      loadlevel -> obstacle_list [ obstacle_counter ] . type = ISO_CAVE_WALL_H ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . x = x + 0.5 ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . y = y + 0.5 ;
	      obstacle_counter ++ ;
	      break;

	    case CAVE_CORNER_RD:
	      loadlevel -> obstacle_list [ obstacle_counter ] . type = ISO_CAVE_CORNER_SE ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . x = x + 0.5 ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . y = y + 0.5 ;
	      obstacle_counter ++ ;
	      break;
	    case CAVE_CORNER_RU:
	      loadlevel -> obstacle_list [ obstacle_counter ] . type = ISO_CAVE_CORNER_NE ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . x = x + 0.5 ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . y = y + 0.5 ;
	      obstacle_counter ++ ;
	      break;
	    case CAVE_CORNER_LU:
	      loadlevel -> obstacle_list [ obstacle_counter ] . type = ISO_CAVE_CORNER_NW ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . x = x + 0.5 ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . y = y + 0.5 ;
	      obstacle_counter ++ ;
	      break;
	    case CAVE_CORNER_LD:
	      loadlevel -> obstacle_list [ obstacle_counter ] . type = ISO_CAVE_CORNER_SW ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . x = x + 0.5 ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . y = y + 0.5 ;
	      obstacle_counter ++ ;
	      break;

	    case CONSOLE_R:
	      loadlevel -> obstacle_list [ obstacle_counter ] . type = ISO_CONSOLE_E ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . x = x + 0.75 ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . y = y + 0.5 ;
	      obstacle_counter ++ ;
	      break;
	    case CONSOLE_L:
	      loadlevel -> obstacle_list [ obstacle_counter ] . type = ISO_CONSOLE_W ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . x = x + 0.25 ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . y = y + 0.5 ;
	      obstacle_counter ++ ;
	      break;
	    case CONSOLE_U:
	      loadlevel -> obstacle_list [ obstacle_counter ] . type = ISO_CONSOLE_N ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . x = x + 0.5 ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . y = y + 0.25 ;
	      obstacle_counter ++ ;
	      break;
	    case CONSOLE_D:
	      loadlevel -> obstacle_list [ obstacle_counter ] . type = ISO_CONSOLE_S ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . x = x + 0.5 ;
	      loadlevel -> obstacle_list [ obstacle_counter ] . pos . y = y + 0.75 ;
	      obstacle_counter ++ ;
	      break;

	    default:
	      if ( IsWallBlock ( loadlevel -> map [ y ] [ x ]  . floor_value ) ) 
		{
		  loadlevel -> obstacle_list [ obstacle_counter ] . type = 4 ;
		  loadlevel -> obstacle_list [ obstacle_counter ] . pos . x = x + 0.5;
		  loadlevel -> obstacle_list [ obstacle_counter ] . pos . y = y + 0.5;
		  obstacle_counter ++ ;
		}
	      break;
	    }
	}
    }

  loadlevel -> obstacle_list [ obstacle_counter ] . type = ISO_COOKING_POT ;
  loadlevel -> obstacle_list [ obstacle_counter ] . pos . x = 20 + 0.5;
  loadlevel -> obstacle_list [ obstacle_counter ] . pos . y = 80 + 0.5;
  obstacle_counter ++ ;

  
}; // void generate_wallobstacles_from_level_map ( int level_num )

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
      x_tile = rintf ( loadlevel -> obstacle_list [ obstacle_counter ] . pos . x );
      y_tile = rintf ( loadlevel -> obstacle_list [ obstacle_counter ] . pos . y );

      if ( x_tile < 0 ) x_tile = 0;       if ( y_tile < 0 ) y_tile = 0 ;
      if ( x_tile >= loadlevel -> xlen ) x_tile = loadlevel -> xlen - 1;
      if ( y_tile >= loadlevel -> ylen ) y_tile = loadlevel -> ylen - 1;

      for ( glue_index = 0 ; glue_index < MAX_OBSTACLES_GLUED_TO_ONE_MAP_TILE ; glue_index ++ )
	{
	  if ( loadlevel -> map [ y_tile ] [ x_tile ] . obstacles_glued_to_here [ glue_index ] != (-1) )
	    {
	      // DebugPrintf ( 0 , "\nHey, someone's already sitting here... moving to next index...: %d." ,
	      // glue_index + 1 );
	    }
	  else
	    {
	      if ( glue_index == MAX_OBSTACLES_GLUED_TO_ONE_MAP_TILE )
		{
		  GiveStandardErrorMessage ( "glue_obstacles_to_floor_tiles_for_level (...)" , "\
FreedroidRPG was unable to glue a certain obstacle to the nearest map tile.\n\
This bug can be resolved by simply raising a contant by one, but it needs to be done :)",
					     PLEASE_INFORM, IS_FATAL );
		}
	      next_free_index = glue_index ;
	      break;
	    }
	}

      //--------------------
      // Now it can be glued...
      //
      loadlevel -> map [ y_tile ] [ x_tile ] . obstacles_glued_to_here [ glue_index ] =
	obstacle_counter ; 

    }

}; // glue_obstacles_to_floor_tiles_for_level ( int level_num )

/* ----------------------------------------------------------------------
 * Next we extract the map labels of this level WITHOUT destroying
 * or damaging the data in the process!
 * ---------------------------------------------------------------------- */
void 
DecodeObstaclesOfThisLevel ( Level loadlevel , char* DataPointer )
{
  int i;
  /*
  char PreservedLetter;
  char* MapLabelPointer;
  char* MapLabelSectionBegin;
  char* MapLabelSectionEnd;
  int NumberOfMapLabelsInThisLevel;
  */

  //--------------------
  // First we initialize the obstacle list with 'empty' information
  //
  for ( i = 0 ; i < MAX_MAP_LABELS_PER_LEVEL ; i ++ )
    {
      loadlevel -> obstacle_list [ i ] . type = ( -1 ) ;
      loadlevel -> obstacle_list [ i ] . pos . x = ( -1 ) ;
      loadlevel -> obstacle_list [ i ] . pos . y = ( -1 ) ;
    }

  loadlevel -> obstacle_list [ 0 ] . type = 0 ;
  loadlevel -> obstacle_list [ 0 ] . pos . x = 20 ;
  loadlevel -> obstacle_list [ 0 ] . pos . y = 80 ;

  loadlevel -> obstacle_list [ 1 ] . type = 1 ;
  loadlevel -> obstacle_list [ 1 ] . pos . x = 22 ;
  loadlevel -> obstacle_list [ 1 ] . pos . y = 80 ;

  loadlevel -> obstacle_list [ 1 ] . type = 2 ;
  loadlevel -> obstacle_list [ 1 ] . pos . x = 20 ;
  loadlevel -> obstacle_list [ 1 ] . pos . y = 78 ;

  /*
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
  */

}; // void DecodeObstaclesOfThisLevel ( Level loadlevel , char* DataPointer );


/* ----------------------------------------------------------------------
 * This function returns TRUE for blocks classified as "Walls" and false
 * otherwise.
 * ---------------------------------------------------------------------- */
int
IsWallBlock (int block)
{
  switch (block)
    {
    case KREUZ:
    case H_WALL:
    case V_WALL:
    case H_SHUT_DOOR:
    case V_SHUT_DOOR:
    case LOCKED_H_SHUT_DOOR:
    case LOCKED_V_SHUT_DOOR:
    case CORNER_LD:
    case T_D:
    case CORNER_RD:
    case T_L:
    case T_R:
    case CORNER_LU:
    case T_U:
    case CORNER_RU:
    case ENHANCER_LD:
    case ENHANCER_RD:
    case ENHANCER_LU:
    case ENHANCER_RU:
    case BLOCK1:
    case BLOCK2:
    case BLOCK3:
    case AUTOGUN_L:
    case AUTOGUN_R:
    case AUTOGUN_U:
    case AUTOGUN_D:
    case ALERT:
    case CODEPANEL_L:
    case CODEPANEL_R:
    case CODEPANEL_U:
    case CODEPANEL_D:
    case CONSUMER_1:
    case CONSUMER_2:
    case CONSUMER_3:
    case CONSUMER_4:
    case CAVE_V_WALL:
    case CAVE_H_WALL:
    case CAVE_CORNER_LU:
    case CAVE_CORNER_RU:
    case CAVE_CORNER_LD:
    case CAVE_CORNER_RD:


    case ( BLUE_BLOCKS_OFFSET + KREUZ ) :
    case ( GREEN_BLOCKS_OFFSET + KREUZ ) :
    case ( BLUE_BLOCKS_OFFSET + H_WALL ) :
    case ( GREEN_BLOCKS_OFFSET + H_WALL ) :
    case ( BLUE_BLOCKS_OFFSET + V_WALL ) :
    case ( GREEN_BLOCKS_OFFSET + V_WALL ) :
    case ( BLUE_BLOCKS_OFFSET + H_SHUT_DOOR ) :
    case ( GREEN_BLOCKS_OFFSET + H_SHUT_DOOR ) :
    case ( BLUE_BLOCKS_OFFSET + V_SHUT_DOOR ) :
    case ( GREEN_BLOCKS_OFFSET + V_SHUT_DOOR ) :
    case ( BLUE_BLOCKS_OFFSET + LOCKED_H_SHUT_DOOR ) :
    case ( GREEN_BLOCKS_OFFSET + LOCKED_H_SHUT_DOOR ) :
    case ( BLUE_BLOCKS_OFFSET + LOCKED_V_SHUT_DOOR ) :
    case ( GREEN_BLOCKS_OFFSET + LOCKED_V_SHUT_DOOR ) :
    case ( BLUE_BLOCKS_OFFSET + CORNER_LD ) :
    case ( GREEN_BLOCKS_OFFSET + CORNER_LD ) :
    case ( BLUE_BLOCKS_OFFSET + T_D ) :
    case ( GREEN_BLOCKS_OFFSET + T_D ) :
    case ( BLUE_BLOCKS_OFFSET + CORNER_RD ) :
    case ( GREEN_BLOCKS_OFFSET + CORNER_RD ) :
    case ( BLUE_BLOCKS_OFFSET + T_L ) :
    case ( GREEN_BLOCKS_OFFSET + T_L ) :
    case ( BLUE_BLOCKS_OFFSET + T_R ) :
    case ( GREEN_BLOCKS_OFFSET + T_R ) :
    case ( BLUE_BLOCKS_OFFSET + CORNER_LU ) :
    case ( GREEN_BLOCKS_OFFSET + CORNER_LU ) :
    case ( BLUE_BLOCKS_OFFSET + T_U ) :
    case ( GREEN_BLOCKS_OFFSET + T_U ) :
    case ( BLUE_BLOCKS_OFFSET + CORNER_RU ) :
    case ( GREEN_BLOCKS_OFFSET + CORNER_RU ) :
    case ( BLUE_BLOCKS_OFFSET + ENHANCER_LD ) :
    case ( GREEN_BLOCKS_OFFSET + ENHANCER_LD ) :
    case ( BLUE_BLOCKS_OFFSET + ENHANCER_RD ) :
    case ( GREEN_BLOCKS_OFFSET + ENHANCER_RD ) :
    case ( BLUE_BLOCKS_OFFSET + ENHANCER_LU ) :
    case ( GREEN_BLOCKS_OFFSET + ENHANCER_LU ) :
    case ( BLUE_BLOCKS_OFFSET + ENHANCER_RU ) :
    case ( GREEN_BLOCKS_OFFSET + ENHANCER_RU ) :
    case ( BLUE_BLOCKS_OFFSET + BLOCK1 ) :
    case ( GREEN_BLOCKS_OFFSET + BLOCK1 ) :
    case ( BLUE_BLOCKS_OFFSET + BLOCK2 ) :
    case ( GREEN_BLOCKS_OFFSET + BLOCK2 ) :
    case ( BLUE_BLOCKS_OFFSET + BLOCK3 ) :
    case ( GREEN_BLOCKS_OFFSET + BLOCK3 ) :
    case ( BLUE_BLOCKS_OFFSET + AUTOGUN_L ) :
    case ( GREEN_BLOCKS_OFFSET + AUTOGUN_L ) :
    case ( BLUE_BLOCKS_OFFSET + AUTOGUN_R ) :
    case ( GREEN_BLOCKS_OFFSET + AUTOGUN_R ) :
    case ( BLUE_BLOCKS_OFFSET + AUTOGUN_U ) :
    case ( GREEN_BLOCKS_OFFSET + AUTOGUN_U ) :
    case ( BLUE_BLOCKS_OFFSET + AUTOGUN_D ) :
    case ( GREEN_BLOCKS_OFFSET + AUTOGUN_D ) :
    case ( BLUE_BLOCKS_OFFSET + ALERT ) :
    case ( GREEN_BLOCKS_OFFSET + ALERT ) :
    case ( BLUE_BLOCKS_OFFSET + CODEPANEL_L ) :
    case ( GREEN_BLOCKS_OFFSET + CODEPANEL_L ) :
    case ( BLUE_BLOCKS_OFFSET + CODEPANEL_R ) :
    case ( GREEN_BLOCKS_OFFSET + CODEPANEL_R ) :
    case ( BLUE_BLOCKS_OFFSET + CODEPANEL_U ) :
    case ( GREEN_BLOCKS_OFFSET + CODEPANEL_U ) :
    case ( BLUE_BLOCKS_OFFSET + CODEPANEL_D ) :
    case ( GREEN_BLOCKS_OFFSET + CODEPANEL_D ) :
    case ( BLUE_BLOCKS_OFFSET + CONSUMER_1 ) :
    case ( GREEN_BLOCKS_OFFSET + CONSUMER_1 ) :
    case ( BLUE_BLOCKS_OFFSET + CONSUMER_2 ) :
    case ( GREEN_BLOCKS_OFFSET + CONSUMER_2 ) :
    case ( BLUE_BLOCKS_OFFSET + CONSUMER_3 ) :
    case ( GREEN_BLOCKS_OFFSET + CONSUMER_3 ) :
    case ( BLUE_BLOCKS_OFFSET + CONSUMER_4 ) :
    case ( GREEN_BLOCKS_OFFSET + CONSUMER_4 ) :
    case ( BLUE_BLOCKS_OFFSET + CAVE_V_WALL ) :
    case ( GREEN_BLOCKS_OFFSET + CAVE_V_WALL ) :
    case ( BLUE_BLOCKS_OFFSET + CAVE_H_WALL ) :
    case ( GREEN_BLOCKS_OFFSET + CAVE_H_WALL ) :
    case ( BLUE_BLOCKS_OFFSET + CAVE_CORNER_LU ) :
    case ( GREEN_BLOCKS_OFFSET + CAVE_CORNER_LU ) :
    case ( BLUE_BLOCKS_OFFSET + CAVE_CORNER_RU ) :
    case ( GREEN_BLOCKS_OFFSET + CAVE_CORNER_RU ) :
    case ( BLUE_BLOCKS_OFFSET + CAVE_CORNER_LD ) :
    case ( GREEN_BLOCKS_OFFSET + CAVE_CORNER_LD ) :
    case ( BLUE_BLOCKS_OFFSET + CAVE_CORNER_RD ) :
    case ( GREEN_BLOCKS_OFFSET + CAVE_CORNER_RD ) :



      return (TRUE);
    default:
      return (FALSE);
    }				// switch
}; // int IsWallBlock( .. )

/* ----------------------------------------------------------------------
 * This function collects the automap data and stores it in the Me data
 * structure.
 * ---------------------------------------------------------------------- */
void
CollectAutomapData ( void )
{
  int x , y ;
  int start_x, start_y, end_x, end_y;
  // finepoint ObjPos;
  gps ObjPos;
  static int TimePassed;
  int level = Me [ 0 ] . pos . z ;
  Level AutomapLevel = curShip . AllLevels [ Me [ 0 ] . pos . z ] ;

  ObjPos . z = Me [ 0 ] . pos . z ;

  //--------------------
  // Checking the whole map for passablility will surely take a lot
  // of computation.  Therefore we only do this once every second of
  // real time.
  //
  if ( TimePassed == (int) Me[0].MissionTimeElapsed ) return;
  TimePassed = (int) Me[0].MissionTimeElapsed;

  //--------------------
  // Also if there is no map-maker present in inventory, then we need not
  // do a thing here...
  //
  if ( ! CountItemtypeInInventory( ITEM_MAP_MAKER_SIMPLE , 0 ) ) return;

  //--------------------
  // Earlier we had
  //
  // start_x = 0 ; start_y = 0 ; end_x = AutomapLevel->xlen ; end_y = AutomapLevel->ylen ;
  //
  // when maximal automap was generated.  Now we only add to the automap what really is on screen...
  //
  start_x = Me [ 0 ] . pos . x - 7 ; 
  end_x = Me [ 0 ] . pos . x + 7 ; 
  start_y = Me [ 0 ] . pos . y - 7 ; 
  end_y = Me [ 0 ] . pos . y + 7 ; 

  if ( start_x < 0 ) start_x = 0 ; 
  if ( end_x > AutomapLevel->xlen ) end_x = AutomapLevel->xlen ;
  if ( start_y < 0 ) start_y = 0 ; 
  if ( end_y > AutomapLevel->ylen ) end_y = AutomapLevel->ylen ;

  //--------------------
  // Now we do the actual checking for visible wall components.
  //
  for ( y = start_y ; y < end_y ; y ++ )
    {
      for ( x = start_x ; x < end_x ; x ++ )
	{
	  if ( IsWallBlock( AutomapLevel->map[y][x]  . floor_value ) ) 
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
	}
    }

}; // void CollectAutomapData ( void )

/* ----------------------------------------------------------------------
 * When one of the box tiles gets hit, e.g. by a blast exploding on the
 * tile or a influencer melee hit on the tile, then the box explodes,
 * possibly leaving some goods behind.
 * ---------------------------------------------------------------------- */
void 
SmashBox ( float x , float y )
{
  int map_x, map_y;
  Level BoxLevel = curShip . AllLevels [ Me [ 0 ] . pos . z ] ;

  map_x=(int)rintf(x);
  map_y=(int)rintf(y);

  //--------------------
  // first we see if there are any destructible map tiles, that need to
  // be destructed this way...
  //
  switch ( BoxLevel->map[ map_y ][ map_x ]  . floor_value )
    { 
    case BOX_4:
    case BOX_3:
    case BOX_2:
    case BOX_1:
      BoxLevel->map[ map_y ][ map_x ]  . floor_value = FLOOR;
      StartBlast( map_x , map_y , BoxLevel->levelnum , DRUIDBLAST );
      DropRandomItem( map_x , map_y , 1 , FALSE , FALSE );
      break;
    default:
      break;
    }

}; // void SmashBox ( float x , float y );

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
      return VOID;
      Terminate (-1);
    }
  if ( RoundX >= deck->xlen)
    {
      // printf ("\n----------------------------------------------------------------------\nunsigned char GetMapBrick(Level deck, float x, float y): Error:\n BlockPosition from outside requested: x>xlen\n----------------------------------------------------------------------\n");
      return VOID;
      Terminate (-1);
    }
  if ( RoundY < 0)
    {
      // printf ("\n----------------------------------------------------------------------\nunsigned char GetMapBrick(Level deck, float x, float y): Error:\n BlockPosition from outside requested: y<0\n----------------------------------------------------------------------\n");
      return VOID;
      Terminate (-1);
    }
  if ( RoundX < 0)
    {
      // printf ("\n----------------------------------------------------------------------\nunsigned char GetMapBrick(Level deck, float x, float y): Error:\n BlockPosition from outside requested: x<0\n----------------------------------------------------------------------\n");
      return VOID;
      Terminate (-1);
    }

  BrickWanted = deck -> map[ RoundY ][ RoundX ] . floor_value ;
  if ( BrickWanted >= NUM_MAP_BLOCKS )
    {
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
      /*
    case LIFT:
      if ( ! ( ( Me [ 0 ] . status == TRANSFERMODE ) &&
	       ( abs(Me[0].speed.x) <= 1) && ( abs(Me[0].speed.y) <= 1)))
	break;

      cx = rintf(x) - x ;
      cy = rintf(y) - y ;

      // only enter the lift, when approximately in the center (of the map tile)
      if ((cx * cx + cy * cy) < Druid_Radius_X * Druid_Radius_X)
	EnterLift ();
      break;
      */

    case CONSOLE_R:
    case CONSOLE_L:
    case CONSOLE_U:
    case CONSOLE_D:
      if ( ( Me [ 0 ] . status == TRANSFERMODE ) &&
	   ( PlayerNum == 0 ) &&
	   ( ! ServerMode ) )
	{
	  EnterConsole ( ) ;
	  DebugPrintf ( 2 , "\nvoid ActSpecialField(int x, int y):  Back from EnterConsole().\n");
	}
      break;

    case CHEST_R:
    case CHEST_L:
    case CHEST_U:
    case CHEST_D:
      /*
      if ( ( Me [ 0 ] . readied_skill == SPELL_LOOT_CHEST_OR_DEAD_BODY ) &&
	   ( PlayerNum == 0 ) &&
	   ( MouseRightPressed() ) && 
	   ( ! ServerMode ) )
	{
	  EnterChest ( ) ;
	  DebugPrintf ( 2 , "\nvoid ActSpecialField(int x, int y):  Back from EnterChest().\n");
	}
      */
      break;

      /*
    case CODEPANEL_R:
    case CODEPANEL_L:
    case CODEPANEL_D:
    case CODEPANEL_U:
      if ( Me[0].status == TRANSFERMODE )
	{
	  EnterCodepanel ( );
	  // DebugPrintf (2, "\nvoid ActSpecialField(int x, int y):  Back from EnterConsole().\n");
	}
      break;
      */

      /*
    case IDENTIFY_R:
    case IDENTIFY_L:
    case IDENTIFY_D:
    case IDENTIFY_U:
      if (Me[0].status == TRANSFERMODE)
	{
	  EnterItemIdentificationBooth ( );
	  // DebugPrintf (2, "\nvoid ActSpecialField(int x, int y):  Back from EnterConsole().\n");
	}
      break;
      */

      /*
    case REFRESH1:
    case REFRESH2:
    case REFRESH3:
    case REFRESH4:
      RefreshInfluencer ();
      break;
      */

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
    }				/* switch */

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

  InnerWaitCounter += Frame_Time () * 10;

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
 * This function moves all the consumer fields to their next phase (if
 * it's time already).
 * ---------------------------------------------------------------------- */
void
AnimateConsumer (void)
{
  static float InnerWaitCounter = 0;
  int i;
  int x, y;
  Level ConsumerLevel = curShip . AllLevels [ Me [ 0 ] . pos . z ] ;

  DebugPrintf (2, "\nvoid AnimateConsumer(void):  real function call confirmed.");

  InnerWaitCounter += Frame_Time () * 10;

  for (i = 0; i < MAX_CONSUMERS_ON_LEVEL; i++)
    {
      x = ConsumerLevel->consumers[i].x;
      y = ConsumerLevel->consumers[i].y;
      if (x == 0 || y == 0)
	break;

      ConsumerLevel->map[y][x]  . floor_value = (((int) rintf (InnerWaitCounter)) % 4) + CONSUMER_1;

    }				/* for */

  DebugPrintf (2, "\nvoid AnimateConsumer(void):  end of function reached.");

}; // void AnimateConsumer ( void )

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
      // We generate obstacles out of the current map info...
      //
      generate_wallobstacles_from_level_map ( i );

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
  strcat(LevelMem, "\n");
  
}; // void EncodeStatementsOfThisLevel ( char* LevelMem , Level Lev )

/* ----------------------------------------------------------------------
 * This function adds the statement data of this level to the chunk of 
 * data that will be written out to a file later.
 * ---------------------------------------------------------------------- */
void
EncodeCodepanelsOfThisLevel ( char* LevelMem , Level Lev )
{
  int i;
  char linebuf[5000];	  

  //--------------------
  // Now we write out a marker to announce the beginning of the codepanel data
  //
  strcat(LevelMem, CODEPANEL_SECTION_BEGIN_STRING);
  strcat(LevelMem, "\n");

  //--------------------
  // Now we write out the bulk of codepanel infos
  //
  for ( i = 0 ; i < MAX_CODEPANELS_PER_LEVEL ; i ++ )
    {
      if ( Lev->CodepanelList[ i ].x == (-1) ) continue;

      strcat( LevelMem , POSITION_X_OF_CODEPANEL_STRING );
      sprintf( linebuf , "%d " , Lev->CodepanelList[ i ].x );
      strcat( LevelMem , linebuf );

      strcat( LevelMem , POSITION_Y_OF_CODEPANEL_STRING );
      sprintf( linebuf , "%d " , Lev->CodepanelList[ i ].y );
      strcat( LevelMem , linebuf );

      strcat( LevelMem , CODEPANEL_CODE_ANNOUNCE_STRING );
      strcat( LevelMem , Lev->CodepanelList[ i ].Secret_Code );
      strcat( LevelMem , "\"\n" );
    }

  strcat(LevelMem, CODEPANEL_SECTION_END_STRING);
  strcat(LevelMem, "\n");
  
}; // void EncodeCodepanelsOfThisLevel ( char* LevelMem , Level Lev )

/* ----------------------------------------------------------------------
 * This function adds the statement data of this level to the chunk of 
 * data that will be written out to a file later.
 * ---------------------------------------------------------------------- */
void
EncodeBigMapInsertsOfThisLevel ( char* LevelMem , Level Lev )
{
  int i;
  char linebuf[5000];	  

  //--------------------
  // Now we write out a marker to announce the beginning of the 
  // big graphics inserts for this map
  //
  strcat( LevelMem, BIG_MAP_INSERT_SECTION_BEGIN_STRING );
  strcat( LevelMem, "\n" );

  //--------------------
  // Now we write out the bulk of big map insert infos
  //
  for ( i = 0 ; i < MAX_MAP_INSERTS_PER_LEVEL ; i ++ )
    {

      if ( Lev -> MapInsertList [ i ] . type <= ( -1 ) ) continue;

      strcat( LevelMem , POSITION_X_OF_BIG_MAP_INSERT_STRING );
      sprintf( linebuf , "%d " , Lev -> MapInsertList [ i ] . pos . x );
      strcat( LevelMem , linebuf );

      strcat( LevelMem , POSITION_Y_OF_BIG_MAP_INSERT_STRING );
      sprintf( linebuf , "%d " , Lev -> MapInsertList [ i ] . pos . y );
      strcat( LevelMem , linebuf );

      strcat( LevelMem , BIG_MAP_INSERT_TYPE_STRING );
      sprintf( linebuf , "%d " , Lev -> MapInsertList [ i ] . type );
      strcat( LevelMem , linebuf );
      strcat( LevelMem, "\n" );

    }

  //--------------------
  // Now we write out a marker to announce the end of the codepanel data
  //
  strcat(LevelMem, BIG_MAP_INSERT_SECTION_END_STRING);
  strcat(LevelMem, "\n");

}; // void EncodeBigMapInsertsOfThisLevel ( char* LevelMem , Level Lev )

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
  int MemAmount=0;		/* the size of the level-data */
  int xlen = Lev->xlen, ylen = Lev->ylen;
  int anz_wp;		/* number of Waypoints */
  char linebuf[5000];		/* Buffer */
  Uint16 HumanReadableMapLine[10000];
  
  /* Get the number of waypoints */
  anz_wp = 0;
  while( Lev->AllWaypoints[anz_wp++].x != 0 );
  anz_wp --;		/* we counted one too much */
		
  /* estimate the amount of memory needed */
  MemAmount = (xlen+1) * (ylen+1); 	/* Map-memory */
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
color of this level: %d\n\
jump threshold north: %d\n\
jump threshold south: %d\n\
jump threshold east: %d\n\
jump threshold west: %d\n\
jump target north: %d\n\
jump target south: %d\n\
jump target east: %d\n\
jump target west: %d\n",
	  Lev->levelnum, Lev->xlen, Lev->ylen, Lev->color , 
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

  EncodeMapLabelsOfThisLevel ( LevelMem , Lev );

  EncodeStatementsOfThisLevel ( LevelMem , Lev );

  EncodeCodepanelsOfThisLevel ( LevelMem , Lev );

  EncodeBigMapInsertsOfThisLevel ( LevelMem , Lev );

  EncodeItemSectionOfThisLevel ( LevelMem , Lev ) ;

  EncodeChestItemSectionOfThisLevel ( LevelMem , Lev ) ;

  // --------------------  
  // The next thing we must do is write the waypoints of this level also
  // to disk.

  // There might be LEADING -1 entries in front of other connection entries.
  // This is unwanted and shall be corrected here.
  CheckWaypointIntegrity( Lev );

  strcat(LevelMem, WP_BEGIN_STRING);
  strcat(LevelMem, "\n");
  
  for(i=0; i< MAXWAYPOINTS ; i++)
  // for(i=0; i< 100 ; i++) THIS LINE IS FOR FORMAT CHANGES IN LEVEL FILE.  VERY HANDY.
    {
      // if ( Lev->AllWaypoints[i].x == 0 ) continue;

      if (i>=MAXWAYPOINTS) sprintf(linebuf, "Nr.=%2d \t x=%4d \t y=%4d", i, 0 , 0 );
      else sprintf(linebuf, "Nr.=%3d x=%4d y=%4d", i, Lev->AllWaypoints[i].x , Lev->AllWaypoints[i].y );
      strcat( LevelMem, linebuf );
      strcat( LevelMem, "\t connections: ");

      for( j=0; j<MAX_WP_CONNECTIONS; j++) 
      // for( j=0; j< 12 ; j++)  THIS LINE IS FOR FORMAT CHANGES IN LEVEL FILE.  VERY HANDY.
	{
	  if ( (i>=MAXWAYPOINTS) || (j >= MAX_WP_CONNECTIONS ) ) sprintf(linebuf, " %3d", -1 );
	  else 
	    {
	      if (Lev->AllWaypoints[i].x == 0 )
		sprintf(linebuf, " %3d", (-1) );
	      else 
		sprintf(linebuf, " %3d", Lev->AllWaypoints[i].connections[j]);
	    }
	  strcat(LevelMem, linebuf);
	} /* for connections */
      strcat(LevelMem, "\n");
    } /* for waypoints */
  
  strcat(LevelMem, LEVEL_END_STRING);
  strcat(LevelMem, 
"\n\
\n\
----------------------------------------------------------------------\n\
\n");
  
  //--------------------
  // So we're done now.  Did the estimate for the amount of memory hit
  // the target or was it at least sufficient? 
  // If not, we're in trouble...
  //
  if( strlen(LevelMem) >= MemAmount) 
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
 *   Copyright (c) 1994, 2002 Johannes Prix\n\
 *   Copyright (c) 1994, 2002 Reinhard Prix\n\
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
  char ThisLine[1000];
  char* ThisLinePointer;
  char* DataPointer;

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

  DecodeMapLabelsOfThisLevel ( loadlevel , DataPointer );

  DecodeObstaclesOfThisLevel ( loadlevel , DataPointer );

  //--------------------
  // Next we extract the statments of the influencer on this level WITHOUT destroying
  // or damaging the data in the process!
  //
  DecodeStatementsOfThisLevel ( loadlevel , DataPointer );

  //--------------------
  // Next we extract the codepanels of this level WITHOUT destroying
  // or damaging the data in the process!
  //
  DecodeCodepanelsOfThisLevel ( loadlevel , DataPointer );

  DecodeBigMapInsertsOfThisLevel ( loadlevel , DataPointer );

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
  if ((wp_begin = strstr (data, WP_BEGIN_STRING)) == NULL)
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

  for ( i = 0 ; i < MAXWAYPOINTS ; i++ )
    {
      WaypointPointer = strstr ( WaypointPointer , "\n" ) +1;

      strncpy (ThisLine , WaypointPointer , strstr( WaypointPointer , "\n") - WaypointPointer + 2);
      ThisLine[strstr( WaypointPointer , "\n") - WaypointPointer + 1 ]=0;
      sscanf( ThisLine , "Nr.=%d \t x=%d \t y=%d" , &nr , &x , &y );
      // printf("\n Values: nr=%d, x=%d, y=%d" , nr , x , y );

      loadlevel->AllWaypoints[i].x=x;
      loadlevel->AllWaypoints[i].y=y;

      if ( ( loadlevel->AllWaypoints[i].x == 255 ) && ( loadlevel->AllWaypoints[i].y == 255 ) )
	{
	  loadlevel->AllWaypoints[i].x=0;
	  loadlevel->AllWaypoints[i].y=0;
	}

      ThisLinePointer = strstr ( ThisLine , "connections: " ) +strlen("connections: ");

      for ( k=0 ; k<MAX_WP_CONNECTIONS ; k++ )
	{
	  sscanf( ThisLinePointer , "%4d" , &connection );
	  // printf(", con=%d" , connection );
	  loadlevel->AllWaypoints[i].connections[k]=connection;
	  ThisLinePointer+=4;
	}

      // getchar();
    }

  /* Scan the waypoint- connections */
  pos = strtok (wp_begin, "\n");	/* Get Pointer to data-begin */

  return loadlevel;

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
  int i, line, col;
  int xlen, ylen;
  int curdoor = 0;
  int curautogun = 0;
  int curref = 0;
  int curcons = 0;
  int curtele = 0;
  char brick;
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

  // DebugPrintf ( 0 , "\nPretest for level %d." , Lev -> levelnum );
  // for (i = 0; i < MAX_DOORS_ON_LEVEL; i++)
  // {
  // DebugPrintf ( 0 , " %d " , Lev -> door_obstacle_indices [ i ] ) ;
  // }

  for (i = 0; i < MAX_CONSUMERS_ON_LEVEL; i++)
    Lev -> consumers[i].x = Lev->consumers[i].y = 0;

  //--------------------
  // New method:  proceed through the obstacles of this level
  // to find out where the doors are...
  //
  for ( obstacle_index = 0 ; obstacle_index < MAX_OBSTACLES_ON_MAP ; obstacle_index ++ )
    {
      //--------------------
      // Maybe we're done with the obstacles now...
      //
      if ( Lev -> obstacle_list [ obstacle_index ] . type == ( -1 ) ) 
	{
	  Lev -> door_obstacle_indices [ curdoor ] = ( -1 ) ;
	  break;
	}

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

  // DebugPrintf ( 0 , "\nFound %d doors on level %d." , curdoor , Lev -> levelnum );
  // for (i = 0; i < MAX_DOORS_ON_LEVEL; i++)
  // {
  // DebugPrintf ( 0 , " %d " , Lev -> door_obstacle_indices [ i ] ) ;
  // }
  

  //--------------------
  // now find the autoguns and that...
  for (line = 0; line < ylen; line++)
    {
      for (col = 0; col < xlen; col++)
	{


	  //--------------------
	  // Old method:  autoguns and that are detected from
	  // map tiles...
	  brick = Lev->map[line][col] . floor_value ;
	  switch ( brick )
	    {

	      /*
	    case H_SHUT_DOOR:
	    case H_HALF_DOOR1:
	    case H_HALF_DOOR2:
	    case H_HALF_DOOR3:
	    case H_OPEN_DOOR:
	    case V_SHUT_DOOR:
	    case V_HALF_DOOR1:
	    case V_HALF_DOOR2:
	    case V_HALF_DOOR3:
	    case V_OPEN_DOOR:
	      Lev->doors[curdoor].x = col;
	      Lev->doors[curdoor++].y = line;
	      if ( curdoor > MAX_DOORS_ON_LEVEL)
		{
		  fprintf( stderr , "\n\nLev->levelnum : %d MAX_DOORS_ON_LEVEL: %d \n" , 
			   Lev->levelnum , MAX_DOORS_ON_LEVEL );
		  GiveStandardErrorMessage ( "GetAllAnimatedMapTiles(...)" , "\
The number of doors found in a level seems to be greater than the number\n\
of doors currently allowed in a Freedroid map.",
					     PLEASE_INFORM, IS_FATAL );
		}
	    break;
	      */

	      /*
	    case AUTOGUN_L:
	    case AUTOGUN_R:
	    case AUTOGUN_U:
	    case AUTOGUN_D:
	      Lev->autoguns[curautogun].x = col;
	      Lev->autoguns[curautogun++].y = line;

	      if (curautogun > MAX_AUTOGUNS_ON_LEVEL)
		{
		  fprintf( stderr , "\n\nLev->levelnum : %d MAX_AUTOGUNS_ON_LEVEL: %d \n" , 
			   Lev->levelnum , MAX_AUTOGUNS_ON_LEVEL );
		  GiveStandardErrorMessage ( "GetAllAnimatedMapTiles(...)" , "\
The number of autoguns found in a level seems to be greater than the number\n\
of doors currently allowed in a Freedroid map.",
					     PLEASE_INFORM, IS_FATAL );
		}
	      break;
	      */

	      /*
	    case REFRESH1:
	    case REFRESH2:
	    case REFRESH3:
	    case REFRESH4:
	      Lev->refreshes[curref].x = col;
	      Lev->refreshes[curref++].y = line;

	      if (curref >= MAX_REFRESHES_ON_LEVEL)
		{
		  fprintf( stderr , "\n\nLev->levelnum : %d MAX_REFRESHES_ON_LEVEL: %d \n" , 
			   Lev->levelnum , MAX_REFRESHES_ON_LEVEL );
		  GiveStandardErrorMessage ( "GetAllAnimatedMapTiles(...)" , "\
The number of refreshes found in a level seems to be greater than the number\n\
of doors currently allowed in a Freedroid map.",
					     PLEASE_INFORM, IS_FATAL );
		}
	      break;
	      */

	    case CONSUMER_1:
	    case CONSUMER_2:
	    case CONSUMER_3:
	    case CONSUMER_4:
	      Lev->consumers[curcons].x = col;
	      Lev->consumers[curcons++].y = line;

	      if (curcons >= MAX_CONSUMERS_ON_LEVEL)
		{
		  fprintf( stderr , "\n\nLev->levelnum : %d MAX_CONSUMERS_ON_LEVEL: %d \n" , 
			   Lev->levelnum , MAX_CONSUMERS_ON_LEVEL );
		  GiveStandardErrorMessage ( "GetAllAnimatedMapTiles(...)" , "\
The number of consumers found in a level seems to be greater than the number\n\
of doors currently allowed in a Freedroid map.",
					     PLEASE_INFORM, IS_FATAL );
		}
	    break;

	    /*
	    case TELE_1:
	    case TELE_2:
	    case TELE_3:
	    case TELE_4:
	      Lev->teleporters[curtele].x = col;
	      Lev->teleporters[curtele++].y = line;

	      if (curtele > MAX_TELEPORTERS_ON_LEVEL)
		{
		  fprintf( stderr , "\n\nLev->levelnum : %d MAX_TELEPORTERS_ON_LEVEL: %d \n" , 
			   Lev->levelnum , MAX_TELEPORTERS_ON_LEVEL );
		  GiveStandardErrorMessage ( "GetAllAnimatedMapTiles(...)" , "\
The number of teleporters found in a level seems to be greater than the number\n\
of doors currently allowed in a Freedroid map.",
					     PLEASE_INFORM, IS_FATAL );
		}
	      break;
	    */

	    default:
	      // if no animated tile, even better...
	      break;
	    }
	}			/* for */
    }				/* for */
  
}; // void GetAllAnimatedMapTiles ( Level Lev )

/* ----------------------------------------------------------------------
 * This function translates map data into human readable map code, that
 * can later be written to the map file on disk.
 * ---------------------------------------------------------------------- */
void
TranslateToHumanReadable ( Uint16* HumanReadable , map_tile* MapInfo, int LineLength , Level Lev , int CurrentLine)
{
  int col;
  int i;
  char Buffer[10];

  DebugPrintf (1,"\n\nTranslating mapline into human readable format...");
  
  // Now in the game and in the level editor, it might have happend that some open
  // doors occur.  The make life easier for the saving routine, these doors should
  // be closed first.

  for (col=0; col < LineLength; col++)
    {
      for(i=0; i< Lev->ylen; i++)
	{
	  switch ( Lev->map[i][col]  . floor_value )
	    {
	    case V_SHUT_DOOR:
	    case V_HALF_DOOR1:
	    case V_HALF_DOOR2:
	    case V_HALF_DOOR3:
	    case V_OPEN_DOOR:
	      Lev->map[i][col] . floor_value = V_SHUT_DOOR;
	      break;
	    case H_SHUT_DOOR:
	    case H_HALF_DOOR1:
	    case H_HALF_DOOR2:
	    case H_HALF_DOOR3:
	    case H_OPEN_DOOR:
	      Lev->map[i][col] . floor_value = H_SHUT_DOOR;
	      break;
	      /*
	    case REFRESH1:
	    case REFRESH2:
	    case REFRESH3:
	    case REFRESH4:
	      Lev->map[i][col] . floor_value =REFRESH1;
	      break;
	      */
	    case TELE_1:
	    case TELE_2:
	    case TELE_3:
	    case TELE_4:
	      Lev->map[i][col] . floor_value = TELE_1;
	      break;
	    default:
	      break;
	    }

	}
    }

  // transpose the game-engine mapdata line to human readable format 

  HumanReadable[0]=0;  // Add a terminator at the beginning

  for (col = 0; col < LineLength; col++)
    {
      sprintf( Buffer , "%3d " , MapInfo [col] . floor_value );
      strcat ( (char*)HumanReadable , Buffer );
    }

}; // void TranslateToHumanReadable( ... )

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
      for (col = 0; col < xdim  ; col++)
	{
	  sscanf( ( ( (char*)(Lev->map[row]) ) + 4 * col) , "%d " , &tmp);
	  Buffer [ col ] . floor_value = tmp;
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
  int FreeAllEnemysPosition;

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
  int doorx, doory;
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
 * This function checks if the given position is passable for a droid,
 * i.e. if there is enough space to the left right up and down, so that
 * a droids center might be at this point without colliding with a wall.
 *
 * In case of a door, the direction into which the influencer has to
 * be moved in order to 'glide' through the door will be returned.
 *
 * In case of an inpassable location, a value of (-1) will be returned.
 *
 * In case of a completely passable location, the CENTER constant will
 * be returned.
 * ---------------------------------------------------------------------- */
int
DruidPassable ( float x , float y , int z )
{
  finepoint testpos[DIRECTIONS + 1];
  int ret = -1;
  int i;

  /* get 8 Check-Points on the druidsurface */
  testpos[OBEN].x = x;
  testpos[OBEN].y = y - Druid_Radius_Y;
  testpos[RECHTSOBEN].x = x + DRUIDRADIUSXY;
  testpos[RECHTSOBEN].y = y - DRUIDRADIUSXY;
  testpos[RECHTS].x = x + Druid_Radius_X;
  testpos[RECHTS].y = y;
  testpos[RECHTSUNTEN].x = x + DRUIDRADIUSXY;
  testpos[RECHTSUNTEN].y = y + DRUIDRADIUSXY;
  testpos[UNTEN].x = x;
  testpos[UNTEN].y = y + Druid_Radius_Y;
  testpos[LINKSUNTEN].x = x - DRUIDRADIUSXY;
  testpos[LINKSUNTEN].y = y + DRUIDRADIUSXY;
  testpos[LINKS].x = x - Druid_Radius_X;
  testpos[LINKS].y = y;
  testpos[LINKSOBEN].x = x - DRUIDRADIUSXY;
  testpos[LINKSOBEN].y = y - DRUIDRADIUSXY;

  for (i = 0; i < DIRECTIONS; i++)
    {

      ret = IsPassable ( testpos[i].x , testpos[i].y , z , i );

      if (ret != CENTER)
	break;

    }				/* for */

  return ret;

}; // int DruidPassable( ... )

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
  // If the obstacle doesn't even have a collision rectangle, then
  // of course it's easy, cause then there can't be any collsision
  //
  if ( obstacle_map [ obs_type ] . block_area_type == COLLISION_TYPE_NONE )
    return ( FALSE );

  if ( obstacle_map [ obs_type ] . block_area_type != COLLISION_TYPE_RECTANGLE )
    {
      fprintf ( stderr, "\n\nCollision_area_type: %d.\n" , obstacle_map [ obs_type ] . block_area_type );
      GiveStandardErrorMessage ( "position_collides_with_this_obstacle(...)" , "\
Error:  Unsupported type of collision area given.",
				 PLEASE_INFORM, IS_FATAL );
    }

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
  if ( ( x > upper_border ) && ( x < lower_border ) && ( y > left_border ) && ( y < right_border ) )
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
 * The location given is like the center of a droid and the 'Checkpos'
 * refers to if the left or right or up or down borders of the droid
 * should be compared for wall collisions.
 *
 * In case of CENTER given as checkpos, no such shift is done but only
 * the collision situation at the given coordinates is returned.
 *
 * A return value of CENTER means that the location is passable in that sense.
 * while any directions and (-1) indicate that it is not so and in case
 * of a direction returned, it's the direction into which the droid
 * should be pushed to resolve the collision (with a door).
 * ---------------------------------------------------------------------- */
int
IsPassable ( float x , float y , int z , int Checkpos)
{
  float fx, fy;
  unsigned char MapBrick;
  int ret = -1;
  Level PassLevel = curShip . AllLevels [ z ] ;
  int obstacle_index;
  int glue_index;
  int x_tile_start, y_tile_start;
  int x_tile_end, y_tile_end;
  int x_tile, y_tile;
  
  //--------------------
  // We take a look whether the position given in the parameter is 
  // blocked by an obstacle ON ANY SQUARE WITHIN A 3x3 TILE RECTANGLE.
  //
  x_tile_start = rintf ( x ) -1 ; y_tile_start = rintf ( y ) -1 ;
  x_tile_end = x_tile_start + 3 ; y_tile_end = y_tile_start + 3 ;
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

  return ( CENTER );

  //----------------------------------------------------------------------
  //----------------------------------------------------------------------
  //----------------------------------------------------------------------


  MapBrick = GetMapBrick ( PassLevel , x , y ) ;

  // ATTENTION!  
  // With the new coodinates, the position of the Influencer is an integer,
  // if and only if it is at the CENTER of a square brick.
  // the fx and fy is designed to be the offset from THE TOP LEFT CORNER
  // of the square.  This is from the old code.
  // Therefore we have to do a short correction here:
  fx = (x-0.5) - floor(x-0.5);
  fy = (y-0.5) - floor(y-0.5);

  // From here on, the old code can be left unchanged.

  switch (MapBrick)
    {
    case FLOOR:
    case CAVE_FLOOR:
    case FLOOR_CARPET_L:
    case FLOOR_CARPET_R:
    case FLOOR_CARPET_U:
    case FLOOR_CARPET_D:
    case CAVE_WITH_GRASS_1:
    case CAVE_WITH_GRASS_2:
    case CAVE_WITH_GRASS_3:
    case CAVE_WITH_GRASS_4:
    case FLOOR_CAVE_L:
    case FLOOR_CAVE_R:
    case FLOOR_CAVE_U:
    case FLOOR_CAVE_D:
    case FLOOR_CARPET:
    case FINE_GRID:
    case LIFT:
    case VOID:
    case BLOCK4:
    case BLOCK5:
    case REFRESH1:
    case REFRESH2:
    case REFRESH3:
    case REFRESH4:
    case CONSUMER_1:
    case CONSUMER_2:
    case CONSUMER_3:
    case CONSUMER_4:
    case CONVEY_L:
    case CONVEY_D:
    case CONVEY_R:
    case CONVEY_U:
    case TELE_1:
    case TELE_2:
    case TELE_3:
    case TELE_4:
    case CAVE_WITH_WAY_TILE:


    case ( BLUE_BLOCKS_OFFSET + FLOOR ) :
    case ( GREEN_BLOCKS_OFFSET + FLOOR ) :
    case ( BLUE_BLOCKS_OFFSET + CAVE_FLOOR ) :
    case ( GREEN_BLOCKS_OFFSET + CAVE_FLOOR ) :
    case ( BLUE_BLOCKS_OFFSET + FLOOR_CARPET_L ) :
    case ( GREEN_BLOCKS_OFFSET + FLOOR_CARPET_L ) :
    case ( BLUE_BLOCKS_OFFSET + FLOOR_CARPET_R ) :
    case ( GREEN_BLOCKS_OFFSET + FLOOR_CARPET_R ) :
    case ( BLUE_BLOCKS_OFFSET + FLOOR_CARPET_U ) :
    case ( GREEN_BLOCKS_OFFSET + FLOOR_CARPET_U ) :
    case ( BLUE_BLOCKS_OFFSET + FLOOR_CARPET_D ) :
    case ( GREEN_BLOCKS_OFFSET + FLOOR_CARPET_D ) :
    case ( BLUE_BLOCKS_OFFSET + CAVE_WITH_GRASS_1 ) :
    case ( GREEN_BLOCKS_OFFSET + CAVE_WITH_GRASS_1 ) :
    case ( BLUE_BLOCKS_OFFSET + CAVE_WITH_GRASS_2 ) :
    case ( GREEN_BLOCKS_OFFSET + CAVE_WITH_GRASS_2 ) :
    case ( BLUE_BLOCKS_OFFSET + CAVE_WITH_GRASS_3 ) :
    case ( GREEN_BLOCKS_OFFSET + CAVE_WITH_GRASS_3 ) :
    case ( BLUE_BLOCKS_OFFSET + CAVE_WITH_GRASS_4 ) :
    case ( GREEN_BLOCKS_OFFSET + CAVE_WITH_GRASS_4 ) :
    case ( BLUE_BLOCKS_OFFSET + FLOOR_CAVE_L ) :
    case ( GREEN_BLOCKS_OFFSET + FLOOR_CAVE_L ) :
    case ( BLUE_BLOCKS_OFFSET + FLOOR_CAVE_R ) :
    case ( GREEN_BLOCKS_OFFSET + FLOOR_CAVE_R ) :
    case ( BLUE_BLOCKS_OFFSET + FLOOR_CAVE_U ) :
    case ( GREEN_BLOCKS_OFFSET + FLOOR_CAVE_U ) :
    case ( BLUE_BLOCKS_OFFSET + FLOOR_CAVE_D ) :
    case ( GREEN_BLOCKS_OFFSET + FLOOR_CAVE_D ) :
    case ( BLUE_BLOCKS_OFFSET + FLOOR_CARPET ) :
    case ( GREEN_BLOCKS_OFFSET + FLOOR_CARPET ) :
    case ( BLUE_BLOCKS_OFFSET + FINE_GRID ) :
    case ( GREEN_BLOCKS_OFFSET + FINE_GRID ) :
    case ( BLUE_BLOCKS_OFFSET + LIFT ) :
    case ( GREEN_BLOCKS_OFFSET + LIFT ) :
    case ( BLUE_BLOCKS_OFFSET + VOID ) :
    case ( GREEN_BLOCKS_OFFSET + VOID ) :
    case ( BLUE_BLOCKS_OFFSET + BLOCK4 ) :
    case ( GREEN_BLOCKS_OFFSET + BLOCK4 ) :
    case ( BLUE_BLOCKS_OFFSET + BLOCK5 ) :
    case ( GREEN_BLOCKS_OFFSET + BLOCK5 ) :
    case ( BLUE_BLOCKS_OFFSET + REFRESH1 ) :
    case ( GREEN_BLOCKS_OFFSET + REFRESH1 ) :
    case ( BLUE_BLOCKS_OFFSET + REFRESH2 ) :
    case ( GREEN_BLOCKS_OFFSET + REFRESH2 ) :
    case ( BLUE_BLOCKS_OFFSET + REFRESH3 ) :
    case ( GREEN_BLOCKS_OFFSET + REFRESH3 ) :
    case ( BLUE_BLOCKS_OFFSET + REFRESH4 ) :
    case ( GREEN_BLOCKS_OFFSET + REFRESH4 ) :
    case ( BLUE_BLOCKS_OFFSET + CONSUMER_1 ) :
    case ( GREEN_BLOCKS_OFFSET + CONSUMER_1 ) :
    case ( BLUE_BLOCKS_OFFSET + CONSUMER_2 ) :
    case ( GREEN_BLOCKS_OFFSET + CONSUMER_2 ) :
    case ( BLUE_BLOCKS_OFFSET + CONSUMER_3 ) :
    case ( GREEN_BLOCKS_OFFSET + CONSUMER_3 ) :
    case ( BLUE_BLOCKS_OFFSET + CONSUMER_4 ) :
    case ( GREEN_BLOCKS_OFFSET + CONSUMER_4 ) :
    case ( BLUE_BLOCKS_OFFSET + CONVEY_L ) :
    case ( GREEN_BLOCKS_OFFSET + CONVEY_L ) :
    case ( BLUE_BLOCKS_OFFSET + CONVEY_D ) :
    case ( GREEN_BLOCKS_OFFSET + CONVEY_D ) :
    case ( BLUE_BLOCKS_OFFSET + CONVEY_R ) :
    case ( GREEN_BLOCKS_OFFSET + CONVEY_R ) :
    case ( BLUE_BLOCKS_OFFSET + CONVEY_U ) :
    case ( GREEN_BLOCKS_OFFSET + CONVEY_U ) :
    case ( BLUE_BLOCKS_OFFSET + TELE_1 ) :
    case ( GREEN_BLOCKS_OFFSET + TELE_1 ) :
    case ( BLUE_BLOCKS_OFFSET + TELE_2 ) :
    case ( GREEN_BLOCKS_OFFSET + TELE_2 ) :
    case ( BLUE_BLOCKS_OFFSET + TELE_3 ) :
    case ( GREEN_BLOCKS_OFFSET + TELE_3 ) :
    case ( BLUE_BLOCKS_OFFSET + TELE_4 ) :
    case ( GREEN_BLOCKS_OFFSET + TELE_4 ) :
    case ( BLUE_BLOCKS_OFFSET + CAVE_WITH_WAY_TILE ) :
    case ( GREEN_BLOCKS_OFFSET + CAVE_WITH_WAY_TILE ) :


      ret = CENTER;		/* these are passable */
      break;

    case ALERT:
    case ( BLUE_BLOCKS_OFFSET + ALERT ) :
    case ( GREEN_BLOCKS_OFFSET + ALERT ) :

      if (Checkpos == LIGHT)
	ret = CENTER;
      else
	{
	  if ((fy < WALLPASS) || (fy > 1 - WALLPASS) || (fx < WALLPASS) || (fx > 1 - WALLPASS))
	    ret = CENTER;
	  else ret = -1;
	}
      break;

    case CHEST_L:
    case CONSOLE_L:
    case CODEPANEL_L:
    case IDENTIFY_L:

    case ( BLUE_BLOCKS_OFFSET + CHEST_L ) :
    case ( GREEN_BLOCKS_OFFSET + CHEST_L ) :
    case ( BLUE_BLOCKS_OFFSET + CONSOLE_L ) :
    case ( GREEN_BLOCKS_OFFSET + CONSOLE_L ) :
    case ( BLUE_BLOCKS_OFFSET + CODEPANEL_L ) :
    case ( GREEN_BLOCKS_OFFSET + CODEPANEL_L ) :
    case ( BLUE_BLOCKS_OFFSET + IDENTIFY_L ) :
    case ( GREEN_BLOCKS_OFFSET + IDENTIFY_L ) :

      if (Checkpos == LIGHT)
	{
	  ret = CENTER;
	  break;
	}
      if ( (fx < WALLPASS) || ( fx > ( 1 - CONSOLEPASS_X ) ) )
	ret = CENTER;
      else
	ret = -1;
      break;

    case CHEST_R:
    case CONSOLE_R:
    case CODEPANEL_R:
    case IDENTIFY_R:

    case ( BLUE_BLOCKS_OFFSET + CHEST_R ) :
    case ( GREEN_BLOCKS_OFFSET + CHEST_R ) :
    case ( BLUE_BLOCKS_OFFSET + CONSOLE_R ) :
    case ( GREEN_BLOCKS_OFFSET + CONSOLE_R ) :
    case ( BLUE_BLOCKS_OFFSET + CODEPANEL_R ) :
    case ( GREEN_BLOCKS_OFFSET + CODEPANEL_R ) :
    case ( BLUE_BLOCKS_OFFSET + IDENTIFY_R ) :
    case ( GREEN_BLOCKS_OFFSET + IDENTIFY_R ) :

      if (Checkpos == LIGHT)
	{
	  ret = CENTER;
	  break;
	}
      if ( (fx < CONSOLEPASS_X) || (fx > 1 - WALLPASS) ) 
	ret = CENTER;
      else
	ret = -1;
      break;

    case CHEST_U:
    case CONSOLE_U:
    case CODEPANEL_U:
    case IDENTIFY_U:

    case ( BLUE_BLOCKS_OFFSET + CHEST_U ) :
    case ( GREEN_BLOCKS_OFFSET + CHEST_U ) :
    case ( BLUE_BLOCKS_OFFSET + CONSOLE_U ) :
    case ( GREEN_BLOCKS_OFFSET + CONSOLE_U ) :
    case ( BLUE_BLOCKS_OFFSET + CODEPANEL_U ) :
    case ( GREEN_BLOCKS_OFFSET + CODEPANEL_U ) :
    case ( BLUE_BLOCKS_OFFSET + IDENTIFY_U ) :
    case ( GREEN_BLOCKS_OFFSET + IDENTIFY_U ) :

      if (Checkpos == LIGHT)
	{
	  ret = CENTER;
	  break;
	}
      if ( (fy < WALLPASS) || (fy > ( 1 - CONSOLEPASS_Y )) )
	ret = CENTER;
      else
	ret = -1;
      break;

    case CHEST_D:
    case CONSOLE_D:
    case CODEPANEL_D:
    case IDENTIFY_D:

    case ( BLUE_BLOCKS_OFFSET + CHEST_D ) :
    case ( GREEN_BLOCKS_OFFSET + CHEST_D ) :
    case ( BLUE_BLOCKS_OFFSET + CONSOLE_D ) :
    case ( GREEN_BLOCKS_OFFSET + CONSOLE_D ) :
    case ( BLUE_BLOCKS_OFFSET + CODEPANEL_D ) :
    case ( GREEN_BLOCKS_OFFSET + CODEPANEL_D ) :
    case ( BLUE_BLOCKS_OFFSET + IDENTIFY_D ) :
    case ( GREEN_BLOCKS_OFFSET + IDENTIFY_D ) :

      if (Checkpos == LIGHT)
	{
	  ret = CENTER;
	  break;
	}
      if ( (fy < CONSOLEPASS_Y) || (fy > 1 - WALLPASS) )
	ret = CENTER;
      else
	ret = -1;
      break;


    case BLOCK1:
    case BLOCK2:
    case BLOCK3:
    case ENHANCER_LD:
    case ENHANCER_RD:
    case ENHANCER_LU:
    case ENHANCER_RU:
    case AUTOGUN_R:
    case AUTOGUN_D:
    case AUTOGUN_L:
    case AUTOGUN_U:

    case ( BLUE_BLOCKS_OFFSET + BLOCK1 ) :
    case ( GREEN_BLOCKS_OFFSET + BLOCK1 ) :
    case ( BLUE_BLOCKS_OFFSET + BLOCK2 ) :
    case ( GREEN_BLOCKS_OFFSET + BLOCK2 ) :
    case ( BLUE_BLOCKS_OFFSET + BLOCK3 ) :
    case ( GREEN_BLOCKS_OFFSET + BLOCK3 ) :
    case ( BLUE_BLOCKS_OFFSET + ENHANCER_LD ) :
    case ( GREEN_BLOCKS_OFFSET + ENHANCER_LD ) :
    case ( BLUE_BLOCKS_OFFSET + ENHANCER_RD ) :
    case ( GREEN_BLOCKS_OFFSET + ENHANCER_RD ) :
    case ( BLUE_BLOCKS_OFFSET + ENHANCER_LU ) :
    case ( GREEN_BLOCKS_OFFSET + ENHANCER_LU ) :
    case ( BLUE_BLOCKS_OFFSET + ENHANCER_RU ) :
    case ( GREEN_BLOCKS_OFFSET + ENHANCER_RU ) :
    case ( BLUE_BLOCKS_OFFSET + AUTOGUN_R ) :
    case ( GREEN_BLOCKS_OFFSET + AUTOGUN_R ) :
    case ( BLUE_BLOCKS_OFFSET + AUTOGUN_D ) :
    case ( GREEN_BLOCKS_OFFSET + AUTOGUN_D ) :
    case ( BLUE_BLOCKS_OFFSET + AUTOGUN_L ) :
    case ( GREEN_BLOCKS_OFFSET + AUTOGUN_L ) :
    case ( BLUE_BLOCKS_OFFSET + AUTOGUN_U ) :
    case ( GREEN_BLOCKS_OFFSET + AUTOGUN_U ) :

      if ((fy < WALLPASS) || (fy > 1 - WALLPASS) || (fx < WALLPASS) || (fx > 1 - WALLPASS))
	ret = CENTER;
      else
	ret = -1;
      break;


    case H_WALL:
    case CAVE_H_WALL:

    case ( BLUE_BLOCKS_OFFSET + H_WALL ) :
    case ( GREEN_BLOCKS_OFFSET + H_WALL ) :
    case ( BLUE_BLOCKS_OFFSET + CAVE_H_WALL ) :
    case ( GREEN_BLOCKS_OFFSET + CAVE_H_WALL ) :

      if ((fy < WALLPASS) || (fy > 1 - WALLPASS))
	ret = CENTER;
      else
	ret = -1;
      break;

    case V_WALL:
    case CAVE_V_WALL:

    case ( BLUE_BLOCKS_OFFSET + V_WALL ) :
    case ( GREEN_BLOCKS_OFFSET + V_WALL ) :
    case ( BLUE_BLOCKS_OFFSET + CAVE_V_WALL ) :
    case ( GREEN_BLOCKS_OFFSET + CAVE_V_WALL ) :

      if ((fx < WALLPASS) || (fx > 1 - WALLPASS))
	ret = CENTER;
      else
	ret = -1;
      break;

    case CORNER_RU:
    case CAVE_CORNER_RU:

    case ( BLUE_BLOCKS_OFFSET + CORNER_RU ) :
    case ( GREEN_BLOCKS_OFFSET + CORNER_RU ) :
    case ( BLUE_BLOCKS_OFFSET + CAVE_CORNER_RU ) :
    case ( GREEN_BLOCKS_OFFSET + CAVE_CORNER_RU ) :

      if ((fx > 1 - WALLPASS) || (fy < WALLPASS) ||
	  ((fx < WALLPASS) && (fy > 1 - WALLPASS)))
	ret = CENTER;
      else
	ret = -1;
      break;

    case CORNER_RD:
    case CAVE_CORNER_RD:

    case ( BLUE_BLOCKS_OFFSET + CORNER_RD ) :
    case ( GREEN_BLOCKS_OFFSET + CORNER_RD ) :
    case ( BLUE_BLOCKS_OFFSET + CAVE_CORNER_RD ) :
    case ( GREEN_BLOCKS_OFFSET + CAVE_CORNER_RD ) :

      if ((fx > 1 - WALLPASS) || (fy > 1 - WALLPASS) ||
	  ((fx < WALLPASS) && (fy < WALLPASS)))
	ret = CENTER;
      else
	ret = -1;
      break;

    case CORNER_LD:
    case CAVE_CORNER_LD:

    case ( BLUE_BLOCKS_OFFSET + CORNER_LD ) :
    case ( GREEN_BLOCKS_OFFSET + CORNER_LD ) :
    case ( BLUE_BLOCKS_OFFSET + CAVE_CORNER_LD ) :
    case ( GREEN_BLOCKS_OFFSET + CAVE_CORNER_LD ) :

      if ((fx < WALLPASS) || (fy > 1 - WALLPASS) ||
	  ((fx > 1 - WALLPASS) && (fy < WALLPASS)))
	ret = CENTER;
      else
	ret = -1;
      break;

    case CORNER_LU:
    case CAVE_CORNER_LU:

    case ( BLUE_BLOCKS_OFFSET + CORNER_LU ) :
    case ( GREEN_BLOCKS_OFFSET + CORNER_LU ) :
    case ( BLUE_BLOCKS_OFFSET + CAVE_CORNER_LU ) :
    case ( GREEN_BLOCKS_OFFSET + CAVE_CORNER_LU ) :

      if ((fx < WALLPASS) || (fy < WALLPASS) ||
	  ((fx > 1 - WALLPASS) && (fy > 1 - WALLPASS)))
	ret = CENTER;
      else
	ret = -1;
      break;

    case T_U:

    case ( BLUE_BLOCKS_OFFSET + T_U ) :
    case ( GREEN_BLOCKS_OFFSET + T_U ) :

      if ((fy < WALLPASS) ||
	  ((fy > 1 - WALLPASS) &&
	   ((fx < WALLPASS) || (fx > 1 - WALLPASS))))
	ret = CENTER;
      else
	ret = -1;
      break;

    case T_R:

    case ( BLUE_BLOCKS_OFFSET + T_R ) :
    case ( GREEN_BLOCKS_OFFSET + T_R ) :

      if ((fx > 1 - WALLPASS) ||
	  ((fx < WALLPASS) &&
	   ((fy < WALLPASS) || (fy > 1 - WALLPASS))))
	ret = CENTER;
      else
	ret = -1;
      break;

    case T_D:

    case ( BLUE_BLOCKS_OFFSET + T_D ) :
    case ( GREEN_BLOCKS_OFFSET + T_D ) :

      if ((fy > 1 - WALLPASS) ||
	  ((fy < WALLPASS) &&
	   ((fx < WALLPASS) || (fx > 1 - WALLPASS))))
	ret = CENTER;
      else
	ret = -1;
      break;

    case T_L:

    case ( BLUE_BLOCKS_OFFSET + T_L ) :
    case ( GREEN_BLOCKS_OFFSET + T_L ) :

      if ((fx < WALLPASS) ||
	  ((fx > 1 - WALLPASS) &&
	   ((fy < WALLPASS) || (fy > 1 - WALLPASS))))
	ret = CENTER;
      else
	ret = -1;
      break;

    case H_OPEN_DOOR:
    case H_HALF_DOOR3:
    case H_HALF_DOOR2:

    case ( BLUE_BLOCKS_OFFSET + H_OPEN_DOOR ) :
    case ( GREEN_BLOCKS_OFFSET + H_OPEN_DOOR ) :
    case ( BLUE_BLOCKS_OFFSET + H_HALF_DOOR3 ) :
    case ( GREEN_BLOCKS_OFFSET + H_HALF_DOOR3 ) :
    case ( BLUE_BLOCKS_OFFSET + H_HALF_DOOR2 ) :
    case ( GREEN_BLOCKS_OFFSET + H_HALF_DOOR2 ) :

      if (Checkpos == LIGHT)
	{
	  ret = CENTER;
	  break;
	}

    case H_HALF_DOOR1:
    case H_SHUT_DOOR:
    case LOCKED_H_SHUT_DOOR:

    case ( BLUE_BLOCKS_OFFSET + H_HALF_DOOR1 ) :
    case ( GREEN_BLOCKS_OFFSET + H_HALF_DOOR1 ) :
    case ( BLUE_BLOCKS_OFFSET + H_SHUT_DOOR ) :
    case ( GREEN_BLOCKS_OFFSET + H_SHUT_DOOR ) :
    case ( BLUE_BLOCKS_OFFSET + LOCKED_H_SHUT_DOOR ) :
    case ( GREEN_BLOCKS_OFFSET + LOCKED_H_SHUT_DOOR ) :

      /*
      if (Checkpos == LIGHT)
	{
	  ret = -1;
	  break;
	}
      */

      // check if the unpassable part of the door has been collided with
      if (((fx < H_RANDBREITE) || (fx > (1 - H_RANDBREITE)))
	  && ((fy >= H_RANDSPACE) && (fy <= (1 - H_RANDSPACE))))
	{
	  // in case of check for droids passable only, push direction will be returned
	  if ((Checkpos != CENTER) && (Checkpos != LIGHT)
	      && (Me[0].speed.y != 0))
	    {
	      switch (Checkpos)
		{
		case RECHTSOBEN:
		case RECHTSUNTEN:
		case RECHTS:
		  if (fx > 1 - H_RANDBREITE)
		    ret = LINKS;
		  else
		    ret = -1;
		  break;
		case LINKSOBEN:
		case LINKSUNTEN:
		case LINKS:
		  if (fx < H_RANDBREITE)
		    ret = RECHTS;
		  else
		    ret = -1;
		  break;
		default:
		  ret = -1;
		  break;
		}		/* switch Checkpos */
	    }			/* if DRUID && Me[0].speed.y != 0 */
	  else
	    ret = -1;
	} // if side of the door has been collided with...
      else
	{			// directily in the door
	  if ((MapBrick == H_OPEN_DOOR) || (MapBrick == H_HALF_DOOR3))
	    ret = CENTER;	// door open
	  else if ((fy < TUERBREITE) || (fy > 1 - TUERBREITE))
	    ret = CENTER;	// door closed, but not entirely in
	  else
	    ret = -1;		// closed door
	}			// directly in the door

      break;

    case V_OPEN_DOOR:
    case V_HALF_DOOR3:
    case V_HALF_DOOR2:

    case ( BLUE_BLOCKS_OFFSET + V_OPEN_DOOR ) :
    case ( GREEN_BLOCKS_OFFSET + V_OPEN_DOOR ) :
    case ( BLUE_BLOCKS_OFFSET + V_HALF_DOOR3 ) :
    case ( GREEN_BLOCKS_OFFSET + V_HALF_DOOR3 ) :
    case ( BLUE_BLOCKS_OFFSET + V_HALF_DOOR2 ) :
    case ( GREEN_BLOCKS_OFFSET + V_HALF_DOOR2 ) :

      if (Checkpos == LIGHT)
	{
	  ret = CENTER;
	  break;
	}

    case V_HALF_DOOR1:
    case V_SHUT_DOOR:
    case LOCKED_V_SHUT_DOOR:

    case ( BLUE_BLOCKS_OFFSET + V_HALF_DOOR1 ) :
    case ( GREEN_BLOCKS_OFFSET + V_HALF_DOOR1 ) :
    case ( BLUE_BLOCKS_OFFSET + V_SHUT_DOOR ) :
    case ( GREEN_BLOCKS_OFFSET + V_SHUT_DOOR ) :
    case ( BLUE_BLOCKS_OFFSET + LOCKED_V_SHUT_DOOR ) :
    case ( GREEN_BLOCKS_OFFSET + LOCKED_V_SHUT_DOOR ) :

      /*
      if (Checkpos == LIGHT)
	{
	  ret = -1;
	  break;
	}
      */

      // check if the side of the door has been collided with...
      if ((fy < V_RANDBREITE || fy > (1 - V_RANDBREITE)) &&
	  (fx >= V_RANDSPACE && fx <= ( 1 - V_RANDSPACE)))
	{

	  // only for droids: check if droid needs to be pushed back and return push direction
	  if ((Checkpos != CENTER) && (Checkpos != LIGHT)
	      && (Me[0].speed.x != 0))
	    {
	      switch (Checkpos)
		{
		case RECHTSOBEN:
		case LINKSOBEN:
		case OBEN:
		  if (fy < V_RANDBREITE)
		    ret = UNTEN;
		  else
		    ret = -1;
		  break;
		case RECHTSUNTEN:
		case LINKSUNTEN:
		case UNTEN:
		  if (fy > 1 - V_RANDBREITE)
		    ret = OBEN;
		  else
		    ret = -1;
		  break;
		default:
		  ret = -1;
		  break;
		}		/* switch Checkpos */
	    }			/* if DRUID && Me[0].speed.x != 0 */
	  else
	    ret = -1;
	}			// if side of door has been collided with...
      else
	{			// directly in the door center
	  if ((MapBrick == V_OPEN_DOOR) || (MapBrick == V_HALF_DOOR3))
	    ret = CENTER;	// open door
	  else if ((fx < TUERBREITE) || (fx > 1 - TUERBREITE))
	    ret = CENTER;	// closed door, but not entirely there
	  else
	    ret = -1;		// closed door
	}			// else directly in the center of the door

      break;

    default:
      ret = -1;
      break;
    }				/* switch MapBrick */

  return ret;

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
  float a_x;		/* Vector Influencer->objectpos */
  float a_y;
  finepoint step;			/* effective step */
  int step_num;			/* number of neccessary steps */
  float a_len;			/* Lenght of a */
  int i;
  finepoint testpos;
  double influ_x = Me [ PlayerNum ] . pos . x ;
  double influ_y = Me [ PlayerNum ] . pos . y ;

  DebugPrintf (2, "\nint IsVisible ( ... ) : real function call confirmed.");

  //--------------------
  // If the object and this player are on different levels, we
  // don't need to check anything.
  //
  if ( objpos -> z != Me [ PlayerNum ] . pos . z ) return FALSE;

  //--------------------
  // Otherwise we have to check visibility...
  //
  a_x = influ_x - objpos->x;
  a_y = influ_y - objpos->y;

  a_len = sqrt (  a_x * a_x + a_y * a_y );
  step_num = a_len * 4 + 1 ;

  // if (step_num == 0) step_num = 1;

  step.x = a_x / step_num;
  step.y = a_y / step_num;

  testpos.x = objpos->x;
  testpos.y = objpos->y;

  for (i = 0; i < step_num + 1 ; i++)
    {

      if ( IsPassable ( testpos.x , testpos.y , objpos->z , LIGHT ) != CENTER)
	{
	  // DebugPrintf (2, "\nint IsVisible(Point objpos): Funktionsende erreicht.");
	  return FALSE;
	}

      testpos.x += step.x;
      testpos.y += step.y;

    }
  // DebugPrintf (2, "\nint IsVisible(Point objpos): Funktionsende erreicht.");

  return TRUE;
}; // int IsVisible( Point objpos )

/* ----------------------------------------------------------------------
 * This function moves all periodically changing map tiles...
 * ---------------------------------------------------------------------- */
void
AnimateCyclingMapTiles (void)
{
  AnimateRefresh();
  AnimateConsumer();
  AnimateTeleports();
}; // void AnimateCyclingMapTiles (void)


#undef _map_c
