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
 * This file contains (all?) map-related functions, which also includes 
 * loading of decks and whole ships, starting the lifts and consoles if 
 * close to the paradroid, refreshes as well as determining the map brick 
 * that contains specified coordinates are done in this file.
 * ---------------------------------------------------------------------- */

/*
 * This file has been checked for remains of german in the documentation.
 * They should be all out by now, and if you still find any, please do not
 * hesitate to remove them.
 */

#define _map_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "proto.h"
#include "global.h"

#include "map.h"

#include "maped.h"

#define AREA_NAME_STRING "Area name=\""
#define LEVEL_NAME_STRING "Name of this level="
#define LEVEL_ENTER_COMMENT_STRING "Comment of the Influencer on entering this level=\""
#define BACKGROUND_SONG_NAME_STRING "Name of background song for this level="
#define MAP_END_STRING "End of pure map information for this level"
#define STATEMENT_BEGIN_STRING "Start of pure statement information for this level"
#define STATEMENT_END_STRING "End of pure statement information for this level"
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
#define X_POSITION_OF_STATEMENT_STRING "PosX="
#define Y_POSITION_OF_STATEMENT_STRING "PosY="
#define STATEMENT_ITSELF_ANNOUNCE_STRING "Statement=\""

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


void TranslateToHumanReadable ( char* HumanReadable , unsigned char* MapInfo, int LineLength , Level Lev , int CurrentLine);
void GetThisLevelsDroids( char* SectionPointer );
Level Decode_Loaded_Leveldata ( char *data );

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
    case H_ZUTUERE:
    case V_ZUTUERE:
    case LOCKED_H_ZUTUERE:
    case LOCKED_V_ZUTUERE:
    case ECK_LU:
    case T_U:
    case ECK_RU:
    case T_L:
    case T_R:
    case ECK_LO:
    case T_O:
    case ECK_RO:
    case BLOCK1:
    case BLOCK2:
    case BLOCK3:
    case ALERT:
    case CODEPANEL_L:
    case CODEPANEL_R:
    case CODEPANEL_U:
    case CODEPANEL_D:
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
  // finepoint ObjPos;
  gps ObjPos;
  static int TimePassed;
  int level = Me [ 0 ] . pos . z ;

  ObjPos . z = Me [ 0 ] . pos . z ;

  //--------------------
  // Checking the whole map for passablility will surely take a lot
  // of computation.  Therefore we only do this once every second of
  // real time.
  //
  if ( TimePassed == (int) Me[0].MissionTimeElapsed ) return;
  TimePassed = (int) Me[0].MissionTimeElapsed;

  //--------------------
  // Now we do the actual checking for visible wall components.
  //
  for ( y = 0 ; y < CurLevel->ylen ; y ++ )
    {
      for ( x = 0 ; x < CurLevel->xlen ; x ++ )
	{
	  if ( IsWallBlock( CurLevel->map[y][x] ) ) 
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
Smash_Box ( float x , float y )
{
  int map_x, map_y;

  map_x=(int)rintf(x);
  map_y=(int)rintf(y);

  //--------------------
  // first we see if there are any destructible map tiles, that need to
  // be destructed this way...
  //
  switch ( CurLevel->map[ map_y ][ map_x ] )
    { 
    case BOX_4:
    case BOX_3:
    case BOX_2:
    case BOX_1:
      CurLevel->map[ map_y ][ map_x ] = FLOOR;
      StartBlast( map_x , map_y , CurLevel->levelnum , DRUIDBLAST );
      DropRandomItem( map_x , map_y , 1 , FALSE , FALSE );
      break;
    default:
      break;
    }

}; // void Smash_Box ( float x , float y );

/* ----------------------------------------------------------------------
 * This function returns the map brick code of the tile that occupies the
 * given position.
 * ---------------------------------------------------------------------- */
unsigned char
GetMapBrick (Level deck, float x, float y)
{

  /* 
   * ATTENTION! BE CAREFUL HERE!  What we want is an integer division with rest, 
   * not an exact foating point division!  Beware of "improvements" here!!!
   */

  if (((int) rintf (y))  >= deck->ylen)
    {
      // printf ("\n----------------------------------------------------------------------\nunsigned char GetMapBrick(Level deck, float x, float y): Error:\n BlockPosition from outside requested: y>ylen\n----------------------------------------------------------------------\n");
      return VOID;
      Terminate (-1);
    }
  if (((int) rintf (x))  >= deck->xlen)
    {
      // printf ("\n----------------------------------------------------------------------\nunsigned char GetMapBrick(Level deck, float x, float y): Error:\n BlockPosition from outside requested: x>xlen\n----------------------------------------------------------------------\n");
      return VOID;
      Terminate (-1);
    }
  if (((int) rintf (y))  < 0)
    {
      // printf ("\n----------------------------------------------------------------------\nunsigned char GetMapBrick(Level deck, float x, float y): Error:\n BlockPosition from outside requested: y<0\n----------------------------------------------------------------------\n");
      return VOID;
      Terminate (-1);
    }
  if (((int) rintf (x))  < 0)
    {
      // printf ("\n----------------------------------------------------------------------\nunsigned char GetMapBrick(Level deck, float x, float y): Error:\n BlockPosition from outside requested: x<0\n----------------------------------------------------------------------\n");
      return VOID;
      Terminate (-1);
    }
  return deck->map[((int) rintf (y)) ][((int) rintf (x)) ];
}; // int GetMapBrick( ... ) 
 
/* ---------------------------------------------------------------------- 
 * This function finds the lift number what corresponds to the current
 * position of the influencer.
 *
 * A return value of  (-1) means: No lift connected to here found !!
 * Other return values contain the number of the lift in AllLifts[],
 * that has a connection to this square.
 *
 * ---------------------------------------------------------------------- */
int
GetCurrentLift (void)
{
  int i;
  int curlev = CurLevel->levelnum;
  int gx, gy;

  gx = rintf(Me[0].pos.x);
  gy = rintf(Me[0].pos.y);

  DebugPrintf( 1 , "\nint GetCurrentLift( void ): curlev=%d gx=%d gy=%d" , curlev, gx, gy );
  DebugPrintf( 1 , "\nint GetCurrentLift( void ): List of elevators:\n");
  for (i = 0; i < curShip.num_lifts+1; i++)
    {
      DebugPrintf( 1 , "\nIndex=%d level=%d gx=%d gy=%d" , i , curShip.AllLifts[i].level , curShip.AllLifts[i].x , curShip.AllLifts[i].y );
    }

  for (i = 0; i < curShip.num_lifts+1; i++) // we check for one more than present, so the last reached
                                            // will really mean: NONE FOUND.
    {
      if (curShip.AllLifts[i].level != curlev)
	continue;
      if ((curShip.AllLifts[i].x == gx) &&
	  (curShip.AllLifts[i].y == gy))
	break;
    }

  if (i == curShip.num_lifts+1)	// none found
    return -1;
  else
    return i;
}; // int GetCurrentLift ( void )

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
      Teleport ( (int) Me[0].teleport_anchor.z , (int) Me[0].teleport_anchor.x , (int) Me[0].teleport_anchor.y , 0 );
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

    case KONSOLE_R:
    case KONSOLE_L:
    case KONSOLE_O:
    case KONSOLE_U:
      if ( ( Me [ 0 ] . status == TRANSFERMODE ) &&
	   ( PlayerNum == 0 ) &&
	   ( ! ServerMode ) )
	{
	  EnterKonsole ( ) ;
	  DebugPrintf ( 2 , "\nvoid ActSpecialField(int x, int y):  Back from EnterKonsole().\n");
	}
      break;

      /*
    case CODEPANEL_R:
    case CODEPANEL_L:
    case CODEPANEL_D:
    case CODEPANEL_U:
      if ( Me[0].status == TRANSFERMODE )
	{
	  EnterCodepanel ( );
	  // DebugPrintf (2, "\nvoid ActSpecialField(int x, int y):  Back from EnterKonsole().\n");
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
	  // DebugPrintf (2, "\nvoid ActSpecialField(int x, int y):  Back from EnterKonsole().\n");
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
  int x, y;

  DebugPrintf (2, "\nvoid AnimateRefresh(void):  real function call confirmed.");

  InnerWaitCounter += Frame_Time () * 10;

  for (i = 0; i < MAX_REFRESHES_ON_LEVEL; i++)
    {
      x = CurLevel->refreshes[i].x;
      y = CurLevel->refreshes[i].y;
      if (x == 0 || y == 0)
	break;

      CurLevel->map[y][x] = (((int) rintf (InnerWaitCounter)) % 4) + REFRESH1;

    }				/* for */

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
  int i, x, y;
  Level TeleporterLevel = curShip.AllLevels [ Me [ 0 ] . pos . z ] ;


  DebugPrintf (2, "\nvoid AnimateRefresh(void):  real function call confirmed.");

  InnerWaitCounter += Frame_Time () * 30;

  for (i = 0; i < MAX_TELEPORTERS_ON_LEVEL; i++)
    {
      x = TeleporterLevel->teleporters[i].x;
      y = TeleporterLevel->teleporters[i].y;
      if (x == 0 || y == 0)
	break;

      TeleporterLevel->map[y][x] = (((int) rintf (InnerWaitCounter)) % 4) + TELE_1;

    }				/* for */

  DebugPrintf (2, "\nvoid AnimateRefresh(void):  end of function reached.");

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

  /* Read the whole ship-data to memory */
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

  /* init the level-structs */
  curShip.num_levels = level_anz;

  for (i = 0; i < curShip.num_levels; i++)
    {
      
      curShip . AllLevels [ i ] = Decode_Loaded_Leveldata ( LevelStart [ i ] );

      TranslateMap ( curShip . AllLevels [ i ] ) ;

    }

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
 * This function generates savable text out of the current lavel data
 * ---------------------------------------------------------------------- */
char *
Encode_Level_For_Saving(Level Lev)
{
  char *LevelMem;
  int i, j;
  int MemAmount=0;		/* the size of the level-data */
  int xlen = Lev->xlen, ylen = Lev->ylen;
  int anz_wp;		/* number of Waypoints */
  char linebuf[81];		/* Buffer */
  char HumanReadableMapLine[10000]="Hello, this is gonna be a made into a readable map-string.";
  
  /* Get the number of waypoints */
  anz_wp = 0;
  while( Lev->AllWaypoints[anz_wp++].x != 0 );
  anz_wp --;		/* we counted one too much */
		
  /* estimate the amount of memory needed */
  MemAmount = (xlen+1) * ylen; 	/* Map-memory */
  MemAmount += MAXWAYPOINTS * MAX_WP_CONNECTIONS * 4;
  MemAmount += 500000;		/* Puffer fuer Dimensionen, mark-strings .. */
  
  /* allocate some memory */
  if( (LevelMem = (char*)malloc(MemAmount)) == NULL) {
    DebugPrintf(1, "\n\nError in StructToMem:  Could not allocate memory...\n\nTerminating...\n\n");
    Terminate(ERR);
    return NULL;
  }

  // Write the data to memory:
  // Here the levelnumber and general information about the level is written
  sprintf(linebuf, "Levelnumber: %d\nxlen of this level: %d\nylen of this level: %d\ncolor of this level: %d\n",
	  Lev->levelnum, Lev->xlen, Lev->ylen, Lev->color);
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
    strncat(LevelMem, HumanReadableMapLine , xlen * 4 ); // We need FOUR chars per map tile
    strcat(LevelMem, "\n");
  }

  // Now we write out a marker at the end of the map data.  This marker is not really
  // vital for reading in the file again, but it adds clearness to the files structure.
  strcat(LevelMem, MAP_END_STRING);
  strcat(LevelMem, "\n");
  
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

  //--------------------
  // Now we write out a marker to announce the end of the codepanel data
  //
  strcat(LevelMem, CODEPANEL_SECTION_END_STRING);
  strcat(LevelMem, "\n");
  

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

      if ( Lev -> MapInsertList [ i ] . type == ( -1 ) ) continue;

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

      strcat( LevelMem , ITEM_CODE_STRING );
      sprintf( linebuf , "%d " , Lev->ItemList[ i ].type );
      strcat( LevelMem , linebuf );

      strcat( LevelMem , ITEM_POS_X_STRING );
      sprintf( linebuf , "%f " , Lev->ItemList[ i ].pos.x );
      strcat( LevelMem , linebuf );

      strcat( LevelMem , ITEM_POS_Y_STRING );
      sprintf( linebuf , "%f " , Lev->ItemList[ i ].pos.y );
      strcat( LevelMem , linebuf );

      strcat( LevelMem , ITEM_AC_BONUS_STRING );
      sprintf( linebuf , "%d " , Lev->ItemList[ i ].ac_bonus );
      strcat( LevelMem , linebuf );

      strcat( LevelMem , ITEM_DAMAGE_STRING );
      sprintf( linebuf , "%d " , Lev->ItemList[ i ].damage );
      strcat( LevelMem , linebuf );

      strcat( LevelMem , ITEM_DAMAGE_MODIFIER_STRING );
      sprintf( linebuf , "%d " , Lev->ItemList[ i ].damage_modifier );
      strcat( LevelMem , linebuf );

      strcat( LevelMem , ITEM_MAX_DURATION_STRING );
      sprintf( linebuf , "%d " , Lev->ItemList[ i ].max_duration );
      strcat( LevelMem , linebuf );

      strcat( LevelMem , ITEM_CUR_DURATION_STRING );
      sprintf( linebuf , "%f " , Lev->ItemList[ i ].current_duration );
      strcat( LevelMem , linebuf );

      strcat( LevelMem , ITEM_GOLD_AMOUNT_STRING );
      sprintf( linebuf , "%d " , Lev->ItemList[ i ].gold_amount );
      strcat( LevelMem , linebuf );

      strcat( LevelMem , ITEM_PREFIX_CODE_STRING );
      sprintf( linebuf , "%d " , Lev->ItemList[ i ].prefix_code );
      strcat( LevelMem , linebuf );

      strcat( LevelMem , ITEM_SUFFIX_CODE_STRING );
      sprintf( linebuf , "%d " , Lev->ItemList[ i ].suffix_code );
      strcat( LevelMem , linebuf );

      // Now we save the primary stat (attribute) boni

      strcat( LevelMem , ITEM_BONUS_TO_STR_STRING );
      sprintf( linebuf , "%d " , Lev->ItemList[ i ].bonus_to_str );
      strcat( LevelMem , linebuf );

      strcat( LevelMem , ITEM_BONUS_TO_DEX_STRING );
      sprintf( linebuf , "%d " , Lev->ItemList[ i ].bonus_to_dex );
      strcat( LevelMem , linebuf );

      strcat( LevelMem , ITEM_BONUS_TO_VIT_STRING );
      sprintf( linebuf , "%d " , Lev->ItemList[ i ].bonus_to_vit );
      strcat( LevelMem , linebuf );

      strcat( LevelMem , ITEM_BONUS_TO_MAG_STRING );
      sprintf( linebuf , "%d " , Lev->ItemList[ i ].bonus_to_mag );
      strcat( LevelMem , linebuf );

      strcat( LevelMem , ITEM_BONUS_TO_ALLATT_STRING );
      sprintf( linebuf , "%d " , Lev->ItemList[ i ].bonus_to_all_attributes );
      strcat( LevelMem , linebuf );

      // Now we save the secondary stat boni

      strcat( LevelMem , ITEM_BONUS_TO_LIFE_STRING );
      sprintf( linebuf , "%d " , Lev->ItemList[ i ].bonus_to_life );
      strcat( LevelMem , linebuf );

      strcat( LevelMem , ITEM_BONUS_TO_FORCE_STRING );
      sprintf( linebuf , "%d " , Lev->ItemList[ i ].bonus_to_force );
      strcat( LevelMem , linebuf );

      strcat( LevelMem , ITEM_BONUS_TO_TOHIT_STRING );
      sprintf( linebuf , "%d " , Lev->ItemList[ i ].bonus_to_tohit );
      strcat( LevelMem , linebuf );

      strcat( LevelMem , ITEM_BONUS_TO_ACDAM_STRING );
      sprintf( linebuf , "%d " , Lev->ItemList[ i ].bonus_to_ac_or_damage );
      strcat( LevelMem , linebuf );

      // Now we save the resistanc boni

      strcat( LevelMem , ITEM_BONUS_TO_RESELE_STRING );
      sprintf( linebuf , "%d " , Lev->ItemList[ i ].bonus_to_resist_electricity );
      strcat( LevelMem , linebuf );

      strcat( LevelMem , ITEM_BONUS_TO_RESFOR_STRING );
      sprintf( linebuf , "%d " , Lev->ItemList[ i ].bonus_to_resist_force );
      strcat( LevelMem , linebuf );

      strcat( LevelMem , ITEM_BONUS_TO_RESFIR_STRING );
      sprintf( linebuf , "%d " , Lev->ItemList[ i ].bonus_to_resist_fire );
      strcat( LevelMem , linebuf );

      strcat( LevelMem , "\n" );
    }
  //--------------------
  // Now we write out a marker to announce the end of the items data
  //
  strcat(LevelMem, ITEMS_SECTION_END_STRING);
  strcat(LevelMem, "\n");
  

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
  
}; // char *Encode_Level_For_Saving ( Level Lev )


/* ----------------------------------------------------------------------
 * This function should save a whole ship to disk to the given filename.
 * It is not only used by the level editor, but also by the function that
 * saves whole games.
 * ---------------------------------------------------------------------- */
int SaveShip(char *filename)
{
  char *LevelMem;		/* linear memory for one Level */
  char *MapHeaderString;
  FILE *ShipFile;  // to this file we will save all the ship data...
  int level_anz;
  int array_i, array_num;
  int i;

  DebugPrintf (2, "\nint SaveShip(char *shipname): real function call confirmed.");
  
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
      LevelMem = Encode_Level_For_Saving (curShip.AllLevels[array_num]);
      fwrite(LevelMem, strlen(LevelMem), sizeof(char), ShipFile);
    
      free(LevelMem);
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

/* ----------------------------------------------------------------------
 * This function is for LOADING map data!
 * This function extracts the data from *data and writes them 
 * into a Level-struct:
 *
 * NOTE:  Here, the map-data are NOT yet translated to their 
 *        their internal values, like "VOID", "H_GANZTUERE" and
 *         all the other values from the defs.h file.
 *
 * Doors and Waypoints Arrays are initialized too
 *
 * @Ret:  Level or NULL
 * ---------------------------------------------------------------------- */
Level
Decode_Loaded_Leveldata ( char *data )
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
  char* StatementSectionBegin;
  char* StatementSectionEnd;
  char* StatementPointer;
  char* CodepanelPointer;
  char* CodepanelSectionBegin;
  char* CodepanelSectionEnd;
  char* MapInsertPointer;
  char* MapInsertSectionBegin;
  char* MapInsertSectionEnd;
  char* ItemPointer;
  char* ItemsSectionBegin;
  char* ItemsSectionEnd;
  int NumberOfStatementsInThisLevel;
  int NumberOfCodepanelsInThisLevel;
  int NumberOfMapInsertsInThisLevel;
  int NumberOfItemsInThisLevel;
  char Preserved_Letter;

  //--------------------
  // Get the memory for one level 
  //
  loadlevel = (Level) MyMalloc (sizeof (level));

  loadlevel->empty = FALSE;

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
  sscanf ( DataPointer , "Levelnumber: %u \n xlen of this level: %u \n ylen of this level: %u \n color of this level: %u",
	  &(loadlevel->levelnum), &(loadlevel->xlen),
	  &(loadlevel->ylen), &(loadlevel->color));

  DebugPrintf( 2 , "\nLevelnumber : %d ", loadlevel->levelnum );
  DebugPrintf( 2 , "\nxlen of this level: %d ", loadlevel->xlen );
  DebugPrintf( 2 , "\nylen of this level: %d ", loadlevel->ylen );
  DebugPrintf( 2 , "\ncolor of this level: %d ", loadlevel->ylen );

  if ( loadlevel->ylen >= MAX_MAP_LINES ) 
    {
	  fprintf( stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
A maplevel Freedroid was supposed to load has more map lines than allowed\n\
for a map level as by the constant MAX_MAP_LINES in defs.h.\n\
\n\
Sorry, but unless this constant is raised, I'll refuse to load this map.\n\
\n\
If you receive this error message, either raise the constant yourself and recompile or\n\
contact the developers, as always freedroid-discussion@lists.sourceforge.net.\n\
Thanks a lot.\n\
\n\
But for now Freedroid will terminate to draw attention \n\
to the map loading problem it could not resolve.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" );
	  Terminate(ERR);
    }

  loadlevel->Levelname = ReadAndMallocStringFromData ( data , LEVEL_NAME_STRING , "\n" );
  loadlevel->Background_Song_Name = ReadAndMallocStringFromData ( data , BACKGROUND_SONG_NAME_STRING , "\n" );
  loadlevel->Level_Enter_Comment = ReadAndMallocStringFromData ( data , LEVEL_ENTER_COMMENT_STRING , "\n" );

  //--------------------
  // Next we extract the statments of the influencer on this level WITHOUT destroying
  // or damaging the data in the process!
  //
  
  // First we initialize the statement array with 'empty' values
  //
  for ( i = 0 ; i < MAX_STATEMENTS_PER_LEVEL ; i ++ )
    {
      loadlevel->StatementList[ i ].x = ( -1 ) ;
      loadlevel->StatementList[ i ].y = ( -1 ) ;
      loadlevel->StatementList[ i ].Statement_Text = "No Statement loaded." ;
    }

  // We look for the beginning and end of the map statement section
  StatementSectionBegin = LocateStringInData( data , STATEMENT_BEGIN_STRING );
  StatementSectionEnd = LocateStringInData( data , STATEMENT_END_STRING );

  // We add a terminator at the end, but ONLY TEMPORARY.  The damage will be restored later!
  Preserved_Letter=StatementSectionEnd[0];
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
  StatementSectionEnd[0]=Preserved_Letter;


  //----------------------------------------------------------------------
  // From here on we take apart the codepanel section of the loaded level...
  //----------------------------------------------------------------------

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
  CodepanelSectionBegin = LocateStringInData( data , CODEPANEL_SECTION_BEGIN_STRING );
  CodepanelSectionEnd = LocateStringInData( data , CODEPANEL_SECTION_END_STRING );

  //--------------------
  // We add a terminator at the end, but ONLY TEMPORARY.  The damage will be restored later!
  //
  Preserved_Letter=CodepanelSectionEnd[0];
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
  CodepanelSectionEnd[0]=Preserved_Letter;






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
  MapInsertSectionBegin = LocateStringInData( data , BIG_MAP_INSERT_SECTION_BEGIN_STRING );
  MapInsertSectionEnd = LocateStringInData( data , BIG_MAP_INSERT_SECTION_END_STRING );

  //--------------------
  // We add a terminator at the end, but ONLY TEMPORARY.  The damage will be restored later!
  //
  Preserved_Letter=MapInsertSectionEnd[0];
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
  // Now we repair the damage done to the loaded level data
  //
  MapInsertSectionEnd[0]=Preserved_Letter;




  



  //----------------------------------------------------------------------
  // From here on we take apart the items section of the loaded level...
  //----------------------------------------------------------------------
  
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
      ReadValueFromString( ItemPointer , ITEM_CODE_STRING , "%d" , 
			   &(loadlevel->ItemList[ i ].type) , ItemsSectionEnd );
      ReadValueFromString( ItemPointer , ITEM_POS_X_STRING , "%lf" , 
			   &(loadlevel->ItemList[ i ].pos.x) , ItemsSectionEnd );
      ReadValueFromString( ItemPointer , ITEM_POS_Y_STRING , "%lf" , 
			   &(loadlevel->ItemList[ i ].pos.y) , ItemsSectionEnd );
      ReadValueFromString( ItemPointer , ITEM_AC_BONUS_STRING , "%d" , 
			   &( loadlevel->ItemList[ i ].ac_bonus ) , ItemsSectionEnd );
      ReadValueFromString( ItemPointer , ITEM_DAMAGE_STRING , "%d" , 
			   &( loadlevel->ItemList[ i ].damage ) , ItemsSectionEnd );
      ReadValueFromString( ItemPointer , ITEM_DAMAGE_MODIFIER_STRING , "%d" , 
			   &( loadlevel->ItemList[ i ].damage_modifier ) , ItemsSectionEnd );
      ReadValueFromString( ItemPointer , ITEM_MAX_DURATION_STRING , "%d" , 
			   &( loadlevel->ItemList[ i ].max_duration ) , ItemsSectionEnd );
      ReadValueFromString( ItemPointer , ITEM_CUR_DURATION_STRING , "%f" , 
			   &( loadlevel->ItemList[ i ].current_duration ) , ItemsSectionEnd );
      ReadValueFromString( ItemPointer , ITEM_GOLD_AMOUNT_STRING , "%d" , 
			   &( loadlevel->ItemList[ i ].gold_amount ) , ItemsSectionEnd );
      ReadValueFromString( ItemPointer , ITEM_PREFIX_CODE_STRING , "%d" , 
			   &( loadlevel->ItemList[ i ].prefix_code ) , ItemsSectionEnd );
      ReadValueFromString( ItemPointer , ITEM_SUFFIX_CODE_STRING , "%d" , 
			   &( loadlevel->ItemList[ i ].suffix_code ) , ItemsSectionEnd );
      // Now we read in the boni to the primary stats (attributes)
      ReadValueFromString( ItemPointer , ITEM_BONUS_TO_STR_STRING , "%d" , 
			   &( loadlevel->ItemList[ i ].bonus_to_str ) , ItemsSectionEnd );
      ReadValueFromString( ItemPointer , ITEM_BONUS_TO_DEX_STRING , "%d" , 
			   &( loadlevel->ItemList[ i ].bonus_to_dex ) , ItemsSectionEnd );
      ReadValueFromString( ItemPointer , ITEM_BONUS_TO_MAG_STRING , "%d" , 
			   &( loadlevel->ItemList[ i ].bonus_to_mag ) , ItemsSectionEnd );
      ReadValueFromString( ItemPointer , ITEM_BONUS_TO_VIT_STRING , "%d" , 
			   &( loadlevel->ItemList[ i ].bonus_to_vit ) , ItemsSectionEnd );
      ReadValueFromString( ItemPointer , ITEM_BONUS_TO_ALLATT_STRING , "%d" , 
			   &( loadlevel->ItemList[ i ].bonus_to_all_attributes ) , ItemsSectionEnd );
      // Now we read in the boni for the secondary stats
      ReadValueFromString( ItemPointer , ITEM_BONUS_TO_LIFE_STRING , "%d" , 
			   &( loadlevel->ItemList[ i ].bonus_to_life ) , ItemsSectionEnd );
      ReadValueFromString( ItemPointer , ITEM_BONUS_TO_FORCE_STRING , "%d" , 
			   &( loadlevel->ItemList[ i ].bonus_to_force ) , ItemsSectionEnd );
      ReadValueFromString( ItemPointer , ITEM_BONUS_TO_TOHIT_STRING , "%d" , 
			   &( loadlevel->ItemList[ i ].bonus_to_tohit ) , ItemsSectionEnd );
      ReadValueFromString( ItemPointer , ITEM_BONUS_TO_ACDAM_STRING , "%d" , 
			   &( loadlevel->ItemList[ i ].bonus_to_ac_or_damage ) , ItemsSectionEnd );
      // Now we read in the boni for resistances
      ReadValueFromString( ItemPointer , ITEM_BONUS_TO_RESELE_STRING , "%d" , 
			   &( loadlevel->ItemList[ i ].bonus_to_resist_electricity ) , ItemsSectionEnd );
      ReadValueFromString( ItemPointer , ITEM_BONUS_TO_RESFIR_STRING , "%d" , 
			   &( loadlevel->ItemList[ i ].bonus_to_resist_fire ) , ItemsSectionEnd );
      ReadValueFromString( ItemPointer , ITEM_BONUS_TO_RESFOR_STRING , "%d" , 
			   &( loadlevel->ItemList[ i ].bonus_to_resist_force ) , ItemsSectionEnd );

      DebugPrintf( 1 , "\nPosX=%f PosY=%f Item=%d" , loadlevel->ItemList[ i ].pos.x , 
		   loadlevel->ItemList[ i ].pos.y , loadlevel->ItemList[ i ].type );
    }
  
  // Now we repair the damage done to the loaded level data
  ItemsSectionEnd[0]=Preserved_Letter;

  fflush(stdout);

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
    if ((loadlevel->map[i] = strtok (NULL, "\n")) == NULL)
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

}; // Level Decode_Loaded_Leveldata (char *data)

/* ----------------------------------------------------------------------
 * This function initializes the Doors array of the given level structure
 * Of course the level data must be in the structure already!!
 *
 * Return value: the number of doors found or ERR
 * ---------------------------------------------------------------------- */
int
GetDoors (Level Lev)
{
  int i, line, col;
  int xlen, ylen;
  int curdoor = 0;
  char brick;

  xlen = Lev->xlen;
  ylen = Lev->ylen;

  /* init Doors- Array to 0 */
  for (i = 0; i < MAX_DOORS_ON_LEVEL; i++)
    Lev->doors[i].x = Lev->doors[i].y = 0;

  /* now find the doors */
  for (line = 0; line < ylen; line++)
    {
      for (col = 0; col < xlen; col++)
	{
	  brick = Lev->map[line][col];
	  // if (brick == '=' || brick == '"')
	  if ( brick == V_ZUTUERE || brick == H_ZUTUERE )
	    {
	      Lev->doors[curdoor].x = col;
	      Lev->doors[curdoor++].y = line;

	      if (curdoor > MAX_DOORS_ON_LEVEL)
		{
      fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
The number of doors found in level %d seems to be greater than the number\n\
of doors currently allowed in a freedroid map.\n\
\n\
The constant for the maximum number of doors currently is set to %d in the\n\
freedroid defs.h file.  You can enlarge the constant there, then start make\n\
and make install again, and the map will be loaded without complaint.\n\
\n\
The constant in defs.h is names 'MAX_DOORS_ON_LEVEL'.  If you received this \n\
message, please also tell the developers of the freedroid project, that they\n\
should enlarge the constant in all future versions as well.\n\
\n\
Thanks a lot.\n\
\n\
But for now Freedroid will terminate to draw attention to this small map problem.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" , Lev->levelnum , MAX_DOORS_ON_LEVEL );
		  Terminate(ERR);
		}

	    }			/* if */
	}			/* for */
    }				/* for */

  return curdoor;
}; // int GetDoors (...)

/* ----------------------------------------------------------------------
 * This function initialized the array of Refreshes for animation
 * within the level
 * 
 * Return value: the number of refreshes found or ERR
 * ---------------------------------------------------------------------- */
int
GetRefreshes (Level Lev)
{
  int i, row, col;
  int xlen, ylen;
  int curref = 0;

  xlen = Lev->xlen;
  ylen = Lev->ylen;

  /* init refreshes array to 0 */
  for (i = 0; i < MAX_REFRESHES_ON_LEVEL; i++)
    Lev->refreshes[i].x = Lev->refreshes[i].y = 0;

  /* now find all the refreshes */
  for (row = 0; row < ylen; row++)
    for (col = 0; col < xlen; col++)
      {
	if (Lev->map[row][col] == REFRESH1 )
	  {
	    Lev->refreshes[curref].x = col;
	    Lev->refreshes[curref++].y = row;

	    if (curref >= MAX_REFRESHES_ON_LEVEL)
	      {
		fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
The number of refreshes found in level %d seems to be greater than the number\n\
of refreshes currently allowed in a freedroid map.\n\
\n\
The constant for the maximum number of doors currently is set to %d in the\n\
freedroid defs.h file.  You can enlarge the constant there, then start make\n\
and make install again, and the map will be loaded without complaint.\n\
\n\
The constant in defs.h is names 'MAX_REFRESHES_ON_LEVEL'.  If you received this \n\
message, please also tell the developers of the freedroid project, that they\n\
should enlarge the constant in all future versions as well.\n\
\n\
Thanks a lot.\n\
\n\
But for now Freedroid will terminate to draw attention to this small map problem.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" , Lev->levelnum , MAX_REFRESHES_ON_LEVEL );
		Terminate(ERR);
		return ERR;
	      }
	  }			/* if */
      }				/* for */
  return curref;
}; // int GetRefreshes (Level lev)

/* ----------------------------------------------------------------------
 * This function initialized the array of Teleports for animation
 * within the level
 *
 * Return value:  number of refreshes found or ERR
 * ---------------------------------------------------------------------- */
int
GetTeleports (Level Lev)
{
  int i, row, col;
  int xlen, ylen;
  int curref = 0;

  xlen = Lev->xlen;
  ylen = Lev->ylen;

  // init teleporters array to 0 
  for (i = 0; i < MAX_TELEPORTERS_ON_LEVEL; i++)
    Lev->teleporters[i].x = Lev->teleporters[i].y = 0;

  // now find all the teleporters 
  for (row = 0; row < ylen; row++)
    for (col = 0; col < xlen; col++)
      {
	if (Lev->map[row][col] == TELE_1 )
	  {
	    Lev->teleporters[curref].x = col;
	    Lev->teleporters[curref++].y = row;

	    if (curref > MAX_TELEPORTERS_ON_LEVEL)
	      {
		fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
The number of teleporters found in level %d seems to be greater than the number\n\
of teleporters currently allowed in a freedroid map.\n\
\n\
The constant for the maximum number of doors currently is set to %d in the\n\
freedroid defs.h file.  You can enlarge the constant there, then start make\n\
and make install again, and the map will be loaded without complaint.\n\
\n\
The constant in defs.h is names 'MAX_TELEPORTERS_ON_LEVEL'.  If you received this \n\
message, please also tell the developers of the freedroid project, that they\n\
should enlarge the constant in all future versions as well.\n\
\n\
Thanks a lot.\n\
\n\
But for now Freedroid will terminate to draw attention to this small map problem.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" , Lev->levelnum , MAX_TELEPORTERS_ON_LEVEL );
		Terminate(ERR);
	      }
	  }			/* if */
      }				/* for */
  return curref;
}; // int GetTeleports(Level lev)

/* ----------------------------------------------------------------------
 * This function translates map data into human readable map code, that
 * can later be written to the map file on disk.
 * ---------------------------------------------------------------------- */
void
TranslateToHumanReadable ( char* HumanReadable , unsigned char* MapInfo, int LineLength , Level Lev , int CurrentLine)
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
	  switch ( Lev->map[i][col] )
	    {
	    case V_ZUTUERE:
	    case V_HALBTUERE1:
	    case V_HALBTUERE2:
	    case V_HALBTUERE3:
	    case V_GANZTUERE:
	      Lev->map[i][col]=V_ZUTUERE;
	      break;
	    case H_ZUTUERE:
	    case H_HALBTUERE1:
	    case H_HALBTUERE2:
	    case H_HALBTUERE3:
	    case H_GANZTUERE:
	      Lev->map[i][col]=H_ZUTUERE;
	      break;
	    case REFRESH1:
	    case REFRESH2:
	    case REFRESH3:
	    case REFRESH4:
	      Lev->map[i][col]=REFRESH1;
	      break;
	    case TELE_1:
	    case TELE_2:
	    case TELE_3:
	    case TELE_4:
	      Lev->map[i][col]=TELE_1;
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
      sprintf( Buffer , "%3d " , MapInfo[col] );
      strcat ( HumanReadable , Buffer );
    }

}; // void TranslateToHumanReadable( ... )

/* -----------------------------------------------------------------
 * @Desc: When the ship is loaded from disk, the data of the map 
 *        are initally in a human readable form with sensible
 *        ascii characters.  This however is NOT the format and
 *        the map encoding actually used by the game engine.
 *        Therefore a translation of human readable format to
 *        game-engine format has to occur and that is what this
 *        function achieves.
 *
 * @Ret: OK | ERR
 *
 *-----------------------------------------------------------------*/
int
TranslateMap (Level Lev)
{
  int xdim = Lev->xlen;
  int ydim = Lev->ylen;
  int row, col;
  char *Buffer;
  int tmp;

  DebugPrintf (2, "\n\nStarting to translate the map from human readable disk format into game-engine format.");

  // first round: transpose all ascii-mapdata to internal numbers for map 
  for (row = 0; row < ydim  ; row++)
    {

      Buffer = MyMalloc( xdim + 10 );

      for (col = 0; col < xdim  ; col++)
	{
	  sscanf( Lev->map[row]+4*col , "%d " , &tmp);
	  Buffer[col] = (char)tmp;
	}

      Lev->map[row]=Buffer;
    }				/* for (row=0..) */

  /* Get Doors Array */
  GetDoors ( Lev );

  // NumWaypoints = GetWaypoints (loadlevel);

  // Get Refreshes 
  GetRefreshes ( Lev );

  // Get Teleports
  GetTeleports ( Lev );

  DebugPrintf (2, "\nint TranslateMap(Level Lev): end of function reached.");
  return OK;
}; // int Translate Map( Level lev )


/* ----------------------------------------------------------------------
 * This function loads lift-connctions into cur-ship struct
 * Return values are OK or ERR
 * ---------------------------------------------------------------------- */
int
GetLiftConnections (char *filename)
{
  char *fpath;
  char *Data;
  char *EntryPointer;
  char *EndOfDeckRectangleSection;
  int i;
  int Label;
  int DeckIndex;
  int RectIndex;
  int ElevatorIndex;
  char* EndOfLiftRectangleSection;
  char* EndOfLiftConnectionData;
  Lift CurLift;

#define END_OF_LIFT_DATA_STRING "*** End of elevator specification file ***"
#define START_OF_LIFT_DATA_STRING "*** Beginning of Lift Data ***"
#define START_OF_LIFT_RECTANGLE_DATA_STRING "*** Beginning of elevator rectangles ***"
#define END_OF_LIFT_CONNECTION_DATA_STRING "*** End of Lift Connection Data ***"



  /* Now get the lift-connection data from "FILE.elv" file */

  fpath = find_file (filename, MAP_DIR, FALSE);

  Data = ReadAndMallocAndTerminateFile( fpath , END_OF_LIFT_DATA_STRING ) ;

  /*
  if ( (EntryPointer = strstr( Data , START_OF_LIFT_RECTANGLE_DATA_STRING ) ) == NULL )
    {
      DebugPrintf ( 0 , "\nERROR!  START OF LIFT RECTANGLE DATA STRING NOT FOUND!  Terminating...");
      Terminate(ERR);
    }
  */

  EntryPointer = LocateStringInData ( Data , START_OF_LIFT_RECTANGLE_DATA_STRING );
  EndOfLiftRectangleSection = LocateStringInData ( Data , "*** End of elevator rectangles ***" );


  //--------------------
  // At first we read in the rectangles that define where the colums of the
  // lift are, so that we can highlight them later.
  //
  while ( ( EntryPointer = strstr( EntryPointer , "Elevator Number=" ) ) != NULL )
    {
      ReadValueFromString( EntryPointer , "Elevator Number=" , "%d" , &ElevatorIndex , EndOfLiftRectangleSection );
      EntryPointer ++;

      ReadValueFromString( EntryPointer , "ElRowX=" , "%d" , &curShip.LiftRow_Rect[ ElevatorIndex ].x , EndOfLiftRectangleSection );
      ReadValueFromString( EntryPointer , "ElRowY=" , "%d" , &curShip.LiftRow_Rect[ ElevatorIndex ].y , EndOfLiftRectangleSection );
      ReadValueFromString( EntryPointer , "ElRowW=" , "%d" , &curShip.LiftRow_Rect[ ElevatorIndex ].w , EndOfLiftRectangleSection );
      ReadValueFromString( EntryPointer , "ElRowH=" , "%d" , &curShip.LiftRow_Rect[ ElevatorIndex ].h , EndOfLiftRectangleSection );
    }

  //--------------------
  // Now we read in the rectangles that define where the decks of the
  // current area system are, so that we can highlight them later in the
  // elevator and console functions.
  //
  for ( i = 0 ; i < MAX_LEVELS ; i++ )   curShip.num_level_rects[i] = 0; // this initializes zeros for the number

  EndOfDeckRectangleSection = LocateStringInData ( Data , "*** End of deck rectangle section ***" );
  EntryPointer = Data ;
  
  while ( ( EntryPointer = strstr( EntryPointer , "DeckNr=" ) ) != NULL )
    {
      ReadValueFromString( EntryPointer , "DeckNr=" , "%d" , &DeckIndex , EndOfDeckRectangleSection );
      ReadValueFromString( EntryPointer , "RectNumber=" , "%d" , &RectIndex , EndOfDeckRectangleSection );
      EntryPointer ++;  // to prevent doubly taking this entry
      
      curShip.num_level_rects[ DeckIndex ] ++; // count the number of rects for this deck one up

      ReadValueFromString( EntryPointer , "DeckX=" , "%d" , &curShip.Level_Rects[ DeckIndex ][ RectIndex ].x , EndOfDeckRectangleSection );
      ReadValueFromString( EntryPointer , "DeckY=" , "%d" , &curShip.Level_Rects[ DeckIndex ][ RectIndex ].y , EndOfDeckRectangleSection );
      ReadValueFromString( EntryPointer , "DeckW=" , "%d" , &curShip.Level_Rects[ DeckIndex ][ RectIndex ].w , EndOfDeckRectangleSection );
      ReadValueFromString( EntryPointer , "DeckH=" , "%d" , &curShip.Level_Rects[ DeckIndex ][ RectIndex ].h , EndOfDeckRectangleSection );

    }

  

  //--------------------
  //
  //
  if ( (EntryPointer = strstr( Data , START_OF_LIFT_DATA_STRING ) ) == NULL )
    {
      DebugPrintf ( 0 , "\nERROR!  START OF LIFT DATA STRING NOT FOUND!  Terminating...");
      Terminate(ERR);
    }

  EndOfLiftConnectionData = LocateStringInData ( Data , END_OF_LIFT_CONNECTION_DATA_STRING );
  EntryPointer = Data;

  while ( ( EntryPointer = strstr( EntryPointer , "Label=" ) ) != NULL )
    {
      ReadValueFromString( EntryPointer , "Label=" , "%d" , &Label , EndOfLiftConnectionData );
      CurLift = &(curShip.AllLifts[Label]);
      EntryPointer++; // to avoid doubly taking this entry

      ReadValueFromString( EntryPointer , "Deck=" , "%d" , &(CurLift->level) , EndOfLiftConnectionData );
      ReadValueFromString( EntryPointer , "PosX=" , "%d" , &(CurLift->x) , EndOfLiftConnectionData );
      ReadValueFromString( EntryPointer , "PosY=" , "%d" , &(CurLift->y) , EndOfLiftConnectionData );
      ReadValueFromString( EntryPointer , "LevelUp=" , "%d" , &(CurLift->up) , EndOfLiftConnectionData );
      ReadValueFromString( EntryPointer , "LevelDown=" , "%d" , &(CurLift->down) , EndOfLiftConnectionData );
      ReadValueFromString( EntryPointer , "LiftRow=" , "%d" , &(CurLift->lift_row) , EndOfLiftConnectionData );
      
    }

  curShip.num_lifts = Label;

  return OK;
}; // int GetLiftConnections(char *shipname)

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
  int i, type;

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

  /* Clear Enemy - Array */
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
      DebugPrintf (2, "\nFound another levels droids description starting point entry!");
      EndOfThisDroidSectionPointer = strstr ( DroidSectionPointer , DROIDS_LEVEL_DESCRIPTION_END_STRING ) ;
      if ( EndOfThisDroidSectionPointer == NULL )
	{
	  printf("\nGetCrew:  Unterminated droid section encountered!!\n\nTerminating....");
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
  Number_Of_Droids_On_Ship=0;
  for (i = 0; i < MAX_ENEMYS_ON_SHIP; i++)
    {
      type = AllEnemys[i].type;
      if ( type == (-1) ) continue;  // Do nothing to unused entries
      AllEnemys[i].energy = Druidmap[type].maxenergy;
      AllEnemys[i].Status = !OUT;
      AllEnemys[i].has_greeted_influencer = FALSE ;
      Number_Of_Droids_On_Ship++;
    }

  return (OK);
}; // int GetCrew ( ... ) 

/* -----------------------------------------------------------------
 * Friendly droids have a list of keywords they understand and to
 * which they answer something, that is a question for the influencer,
 * which the influencer shall answer one way or the other (2 options
 * at most for now) and this function initializes these structures
 * in the Enemylist which contain the triggers of such question,
 * the questions themselves, the possible answers and the possible
 * ansers of the robot to that again plus perhaps some events 
 * that are triggered this way or some missions that are assigned
 * this way.
 * ----------------------------------------------------------------- */
void 
GetThisRobotsDecisionRequestList( char* SearchPointer , int RobotNum )
{

#define FRIENDLY_DROID_DECISION_REQUEST_LIST_START_STRING "Start of Decision-Request List for this friendly droid"
#define FRIENDLY_DROID_DECISION_REQUEST_LIST_END_STRING "End of Decision-Request List for this friendly droid"

#define REQUEST_TRIGGER_START_STRING "RequestTrigger=\""
#define REQUEST_REQUIRES_MISSION_DONE "RequestRequiresMissionDone="
#define REQUEST_REQUIRES_MISSION_UNASSIGNED "RequestRequiredMissionUnassigned="
#define REQUEST_TEXT_START_STRING "RequestText=\""
#define ANSWER_YES_START_STRING "AnswerYes=\""
#define ANSWER_NO_START_STRING "AnswerNo=\""
#define RESPONSE_YES_START_STRING "ResponseYes=\""
#define RESPONSE_NO_START_STRING "ResponseNo=\""
#define ACTION_TRIGGER_START_STRING "LabelOfActionInConfirmationCase=\""

  char* RequestListStart;
  char* RequestListEnd;
  char* NextDecisionEntry;
  char* EndOfSearchSectionPointer;

  char* TempSearchArea;
  int DecisionNr = 0;
  int j;

  //--------------------
  // Before we do anything else, we must make sure, that all the pointers
  // are initialized or we might compare the entered text with some forbidden
  // memory the uninitialized pointer points to.  So we must prevent this
  // and do some dummy initialisation.
  //
  for ( j = 0 ; j < MAX_REQUESTS_PER_DROID ; j++ )
    {
      AllEnemys[ RobotNum ].RequestList[ j ].RequestRequiresMissionDone = -1;
      AllEnemys[ RobotNum ].RequestList[ j ].RequestRequiresMissionUnassigned = -1;
      AllEnemys[ RobotNum ].RequestList[ j ].RequestTrigger = "";
      AllEnemys[ RobotNum ].RequestList[ j ].RequestText = "";
      AllEnemys[ RobotNum ].RequestList[ j ].AnswerYes = "";
      AllEnemys[ RobotNum ].RequestList[ j ].AnswerNo = "";
      AllEnemys[ RobotNum ].RequestList[ j ].ResponseYes = "";
      AllEnemys[ RobotNum ].RequestList[ j ].ResponseNo = "";
    }

  //--------------------
  // No we can start to read in the details of this robots requests and all that...
  //

  RequestListStart = LocateStringInData( SearchPointer , FRIENDLY_DROID_DECISION_REQUEST_LIST_START_STRING );
  RequestListEnd = LocateStringInData( SearchPointer , FRIENDLY_DROID_DECISION_REQUEST_LIST_END_STRING );

  TempSearchArea = MyMalloc( RequestListEnd - RequestListStart + 1 ); // First we allocate enough memory
  strncpy ( TempSearchArea , RequestListStart , RequestListEnd-RequestListStart ); // this copys the relevant part
  TempSearchArea[ RequestListEnd - RequestListStart ] = 0; // This shall terminate the string
  EndOfSearchSectionPointer=&(TempSearchArea[RequestListEnd - RequestListStart]);

  NextDecisionEntry = TempSearchArea;
  
  while ( ( NextDecisionEntry = strstr( NextDecisionEntry , REQUEST_TRIGGER_START_STRING  ) ) != NULL )
    {

      //--------------------
      // At first we check against writing beyond the scope of the array containing
      // the questions and answers for the communication with this friendly droid
      //
      if ( DecisionNr >= MAX_CHAT_KEYWORDS_PER_DROID )
	{
	  fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
FREEDROID HAS ENCOUNTERED A PROBLEM:\n\
The function reading and interpreting the friendly droids question-answer list\n\
stumbled into something:\n\
\n\
The number of Decisions and Answers specified for one of the droids is bigger\n\
than the array containing all these Decision and answers defined in the source.\n\
\n\
If you receive this error, just increase the constant MAX_CHAT_KEYWORDS_PER_DROIS\n\
in the source code, then recompile the game and all will be fine.\n\
\n\
But for now Freedroid will terminate to draw attention to the droid file reading\n\
problem it could not resolve.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" );
	  Terminate(ERR);
	}


      AllEnemys[ RobotNum ].RequestList[ DecisionNr ].RequestTrigger = 
	ReadAndMallocStringFromData ( NextDecisionEntry , REQUEST_TRIGGER_START_STRING , "\"" );
      ReadValueFromString( NextDecisionEntry , REQUEST_REQUIRES_MISSION_DONE , "%d" , 
			   &AllEnemys[ RobotNum ].RequestList[ DecisionNr ].RequestRequiresMissionDone , 
			   EndOfSearchSectionPointer );
      ReadValueFromString( NextDecisionEntry , REQUEST_REQUIRES_MISSION_UNASSIGNED , "%d" , 
			   &AllEnemys[ RobotNum ].RequestList[ DecisionNr ].RequestRequiresMissionUnassigned , 
			   EndOfSearchSectionPointer );
      AllEnemys[ RobotNum ].RequestList[ DecisionNr ].RequestText = 
	ReadAndMallocStringFromData ( NextDecisionEntry , REQUEST_TEXT_START_STRING , "\"" );
      AllEnemys[ RobotNum ].RequestList[ DecisionNr ].AnswerYes = 
	ReadAndMallocStringFromData ( NextDecisionEntry , ANSWER_YES_START_STRING , "\"" );
      AllEnemys[ RobotNum ].RequestList[ DecisionNr ].AnswerNo = 
	ReadAndMallocStringFromData ( NextDecisionEntry , ANSWER_NO_START_STRING , "\"" );
      AllEnemys[ RobotNum ].RequestList[ DecisionNr ].ResponseYes = 
	ReadAndMallocStringFromData ( NextDecisionEntry , RESPONSE_YES_START_STRING , "\"" );
      AllEnemys[ RobotNum ].RequestList[ DecisionNr ].ResponseNo = 
	ReadAndMallocStringFromData ( NextDecisionEntry , RESPONSE_NO_START_STRING , "\"" );
      // ReadValueFromString( NextDecisionEntry , "Action is mission assignment=" , "%d" , 
      // &AllEnemys[ RobotNum ].RequestList[ DecisionNr ].ActionTrigger , EndOfLiftRectangleSection );
      AllEnemys[ RobotNum ].RequestList[ DecisionNr ].ActionTrigger = 
	ReadAndMallocStringFromData ( NextDecisionEntry , ACTION_TRIGGER_START_STRING , "\"" );

      DebugPrintf( 0 , "\n\nRequest trigger entry found : %s \n" , 
		   AllEnemys[ RobotNum ].RequestList[ DecisionNr ].RequestTrigger );
      DebugPrintf( 0 , "RequestText entry for this request : %s \n" , 
		   AllEnemys[ RobotNum ].RequestList[ DecisionNr ].RequestText );
      DebugPrintf( 0 , "Answer 'yes' entry found : %s \n" , 
		   AllEnemys[ RobotNum ].RequestList[ DecisionNr ].AnswerYes );
      DebugPrintf( 0 , "Answer 'no' entry found : %s \n" , 
		   AllEnemys[ RobotNum ].RequestList[ DecisionNr ].AnswerNo );
      DebugPrintf( 0 , "Response to 'yes' entry found : %s \n" , 
		   AllEnemys[ RobotNum ].RequestList[ DecisionNr ].ResponseYes );
      DebugPrintf( 0 , "Response to 'no' entry found : %s \n" , 
		   AllEnemys[ RobotNum ].RequestList[ DecisionNr ].ResponseNo );
      DebugPrintf( 0 , "Action trigger entry found : %s \n" , 
		   AllEnemys[ RobotNum ].RequestList[ DecisionNr ].ActionTrigger );

      // Not that this entry has been read, we move all indexes up by one
      DecisionNr++;
      NextDecisionEntry++;
    }

  free ( TempSearchArea );

}; // void GetThisRobotsDecisionRequestList( char* SearchPointer, int RobotNum );

/* -----------------------------------------------------------------
 * Friendly droids have a list of keywords they understand and they
 * can answer in a predefined way.  This list must be decoded from
 * the format style of the droids file to the AllEnemys arrays
 * fields and this is exactly what this function does.
 * ----------------------------------------------------------------- */
void 
GetThisRobotsQuestionResponseList( char* SearchPointer , int RobotNum )
{
#define FRIENDLY_DROID_QUESTION_RESPONSE_LIST_START_STRING "Start of Question-Response List for this friendly droid"
#define FRIENDLY_DROID_QUESTION_RESPONSE_LIST_END_STRING "End of Question-Response List for this friendly droid"
#define QUESTION_START_STRING "Question=\""
#define ANSWER_START_STRING "Answer=\""
  char* ResponseListStart;
  char* ResponseListEnd;
  char* NextQuestionEntry;

  char* TempSearchArea;
  int QuestionNr = 0;

  ResponseListStart = LocateStringInData( SearchPointer , FRIENDLY_DROID_QUESTION_RESPONSE_LIST_START_STRING );
  ResponseListEnd = LocateStringInData( SearchPointer , FRIENDLY_DROID_QUESTION_RESPONSE_LIST_END_STRING );

  TempSearchArea = MyMalloc( ResponseListEnd - ResponseListStart + 1 ); // First we allocate enough memory
  strncpy ( TempSearchArea , ResponseListStart , ResponseListEnd-ResponseListStart ); // this copys the relevant part
  TempSearchArea[ ResponseListEnd - ResponseListStart ] = 0; // This shall terminate the string

  NextQuestionEntry = TempSearchArea;
  
  while ( ( NextQuestionEntry = strstr( NextQuestionEntry , QUESTION_START_STRING  ) ) != NULL )
    {

      //--------------------
      // At first we check against writing beyond the scope of the array containing
      // the questions and answers for the communication with this friendly droid
      //
      if ( QuestionNr >= MAX_CHAT_KEYWORDS_PER_DROID )
	{
	  fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
FREEDROID HAS ENCOUNTERED A PROBLEM:\n\
The function reading and interpreting the friendly droids question-answer list\n\
stumbled into something:\n\
\n\
The number of Questions and Answers specified for one of the droids is bigger\n\
than the array containing all these Question and answers defined in the source.\n\
\n\
If you receive this error, just increase the constant MAX_CHAT_KEYWORDS_PER_DROIS\n\
in the source code, then recompile the game and all will be fine.\n\
\n\
But for now Freedroid will terminate to draw attention to the droid file reading\n\
problem it could not resolve.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" );
	  Terminate(ERR);
	}


      AllEnemys[ RobotNum ].QuestionResponseList[ QuestionNr*2 ] = 
	ReadAndMallocStringFromData ( NextQuestionEntry , QUESTION_START_STRING , "\"" );
      AllEnemys[ RobotNum ].QuestionResponseList[ QuestionNr*2 +1 ] = 
	ReadAndMallocStringFromData ( NextQuestionEntry , ANSWER_START_STRING , "\"" );

      DebugPrintf( 1 , "Question Entry found : %s \n\n" , 
		   AllEnemys[ RobotNum ].QuestionResponseList[ QuestionNr*2 ] );
      DebugPrintf( 1 , "Anser Entry to this question : %s \n\n" , 
		   AllEnemys[ RobotNum ].QuestionResponseList[ QuestionNr*2 + 1 ] );

      // Not that this entry has been read, we move all indexes up by one
      QuestionNr++;
      NextQuestionEntry++;
    }

  free ( TempSearchArea );
}; // void GetThisRobotsQuestionResponseList( char* SearchPointer , int RobotNum )

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
#define SPECIAL_FORCE_INDICATION_STRING "SpecialForce: Type="

#define DROID_DECISION_REQUEST_LIST_START_STRING "Start of Decision-Request List for this friendly droid"
#define DROID_DECISION_REQUEST_LIST_END_STRING "End of Decision-Request List for this friendly droid"


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
      fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
FREEDROID HAS ENCOUNTERED A PROBLEM:\n\
The function reading and interpreting the crew file stunbled into something:\n\
\n\
It was unable to assign the droid type identification string '%s' found \n\
in the entry of the droid types allowed for level %d to an entry in\n\
the List of droids obtained from the gama data specification\n\
file you use.  \n\
\n\
Please check that this type really is spelled correctly, that it consists of\n\
only three characters and that it really has a corresponding entry in the\n\
game data file with all droid type specifications.\n\
\n\
But for now Freedroid will terminate to draw attention to the sound problem\n\
it could not resolve.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" , TypeIndicationString , OurLevelNumber );
	  Terminate(ERR);
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
      AllEnemys[ FreeAllEnemysPosition ].Status = !OUT;

    }  // while (enemy-limit of this level not reached) 


  SearchPointer=SectionPointer;
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
      fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
FREEDROID HAS ENCOUNTERED A PROBLEM:\n\
The function reading and interpreting the crew file stunbled into something:\n\
\n\
It was unable to assign the SPECIAL FORCE droid type identification string '%s' found \n\
in the entry of the droid types allowed for level %d to an entry in\n\
the List of droids obtained from the gama data specification\n\
file you use.  \n\
\n\
Please check that this type really is spelled correctly, that it consists of\n\
only three characters and that it really has a corresponding entry in the\n\
game data file with all droid type specifications.\n\
\n\
But for now Freedroid will terminate to draw attention to the sound problem\n\
it could not resolve.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" , TypeIndicationString , OurLevelNumber );
	  Terminate(ERR);
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
      ReadValueFromString ( SearchPointer ,"Marker=","%d", &AllEnemys[ FreeAllEnemysPosition ].Marker , 
			    EndOfThisLevelData );
      ReadValueFromString ( SearchPointer ,"AdvancedCommand=","%d", &AllEnemys[ FreeAllEnemysPosition ].AdvancedCommand , 
			    EndOfThisLevelData );
      ReadValueFromString ( SearchPointer ,"Parameter1=","%lf", &AllEnemys[ FreeAllEnemysPosition ].Parameter1 , 
			    EndOfThisLevelData );
      ReadValueFromString ( SearchPointer ,"Parameter2=","%lf", &AllEnemys[ FreeAllEnemysPosition ].Parameter2 , 
			    EndOfThisLevelData );
      ReadValueFromString ( SearchPointer ,"Friendly=","%d", &AllEnemys[ FreeAllEnemysPosition ].Friendly , 
			    EndOfThisLevelData );

      AllEnemys[ FreeAllEnemysPosition ].type = ListIndex;
      AllEnemys[ FreeAllEnemysPosition ].pos.z = OurLevelNumber;
      AllEnemys[ FreeAllEnemysPosition ].Status = !OUT;
      AllEnemys[ FreeAllEnemysPosition ].SpecialForce = 1;

      //--------------------
      // AT THIS POINT WE KNOW WHETHER THE DROID IS FRIENDLY OR NOT
      // In case of a friendly droid, we need to check out the question-response list for
      // this droid and read it into the appropriate data structures in AllEnemys too
      //
      if ( AllEnemys[ FreeAllEnemysPosition ].Friendly )
	{
	  GetThisRobotsQuestionResponseList( SearchPointer , FreeAllEnemysPosition );
	  GetThisRobotsDecisionRequestList( SearchPointer , FreeAllEnemysPosition );
	}

    } // while Special force droid found...

  NumEnemys=FreeAllEnemysPosition+1; // we silently assume monotonely increasing FreePosition index. seems ok.
  // getchar();
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
  char *Pos;
  Level DoorLevel;
  int one_player_close_enough = FALSE;
  int PlayerIndex ;

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
      doorx = ( DoorLevel -> doors [ i ] . x );
      doory = ( DoorLevel -> doors [ i ] . y );

      // no more doors?
      if ( ( doorx == 0 ) && ( doory == 0 ) )
	break;

      Pos = & ( DoorLevel -> map [doory] [doorx] ) ;

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
	  xdist = Me [ PlayerIndex ] . pos . x - doorx ;
	  ydist = Me [ PlayerIndex ] . pos . y - doory ;
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
	  if ( (*Pos != H_GANZTUERE) && (*Pos != V_GANZTUERE) )
	    *Pos += 1;
	}
      else 
	{
	  //--------------------
	  // But if the influencer is not close enough, then we must
	  // see if perhaps one of the enemys is close enough, so that
	  // the door would still get opened instead of closed.
	  //
	  // for (j = 0; j < NumEnemys; j++)
	  for (j = 0; j < NumEnemys; j++)
	    {
	      /* ignore druids that are dead or on other levels */
	      if ( ( AllEnemys[j].Status == OUT ) ||
		   ( AllEnemys[j].pos.z  != DoorLevel->levelnum ) )
		continue;

	      xdist = abs ( AllEnemys[j].pos.x - doorx ) ;
	      if (xdist < Block_Width)
		{
		  ydist = abs (AllEnemys[j].pos.y - doory);
		  if (ydist < Block_Height)
		    {
		      dist2 = xdist * xdist + ydist * ydist;
		      if (dist2 < DOOROPENDIST2)
			{
			  if ((*Pos != H_GANZTUERE) && (*Pos != V_GANZTUERE))
			    *Pos += 1;

			  break;	/* one druid is enough to open a door */
			}	/* if */
		    }		/* if */
		}		/* if */
	    }			/* for */

	  /* No druid near: close door if it isnt closed */
	  if (j == NumEnemys)
	    if ((*Pos != V_ZUTUERE) && (*Pos != H_ZUTUERE))
	      *Pos -= 1;

	}			/* else */
    }				/* for */
}; // void MoveLevelDoors ( void )

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
    case LIFT:
    case VOID:
    case BLOCK4:
    case BLOCK5:
    case REFRESH1:
    case REFRESH2:
    case REFRESH3:
    case REFRESH4:
    case CONVEY_L:
    case CONVEY_D:
    case CONVEY_R:
    case CONVEY_U:
    case TELE_1:
    case TELE_2:
    case TELE_3:
    case TELE_4:
      ret = CENTER;		/* these are passable */
      break;

    case ALERT:
      if (Checkpos == LIGHT)
	ret = CENTER;
      else
	ret = -1;
      break;

    case KONSOLE_L:
    case CODEPANEL_L:
    case IDENTIFY_L:
      if (Checkpos == LIGHT)
	{
	  ret = CENTER;
	  break;
	}
      if ( (fx < WALLPASS) || ( fx > ( 1 - KONSOLEPASS_X ) ) )
	ret = CENTER;
      else
	ret = -1;
      break;

    case KONSOLE_R:
    case CODEPANEL_R:
    case IDENTIFY_R:
      if (Checkpos == LIGHT)
	{
	  ret = CENTER;
	  break;
	}
      if ( (fx < KONSOLEPASS_X) || (fx > 1 - WALLPASS) ) 
	ret = CENTER;
      else
	ret = -1;
      break;

    case KONSOLE_O:
    case CODEPANEL_U:
    case IDENTIFY_U:
      if (Checkpos == LIGHT)
	{
	  ret = CENTER;
	  break;
	}
      if ( (fy < WALLPASS) || (fy > ( 1 - KONSOLEPASS_Y )) )
	ret = CENTER;
      else
	ret = -1;
      break;


    case KONSOLE_U:
    case CODEPANEL_D:
    case IDENTIFY_D:
      if (Checkpos == LIGHT)
	{
	  ret = CENTER;
	  break;
	}
      if ( (fy < KONSOLEPASS_Y) || (fy > 1 - WALLPASS) )
	ret = CENTER;
      else
	ret = -1;
      break;



    case H_WALL:
      if ((fy < WALLPASS) || (fy > 1 - WALLPASS))
	ret = CENTER;
      else
	ret = -1;
      break;

    case V_WALL:
      if ((fx < WALLPASS) || (fx > 1 - WALLPASS))
	ret = CENTER;
      else
	ret = -1;
      break;

    case ECK_RO:
      if ((fx > 1 - WALLPASS) || (fy < WALLPASS) ||
	  ((fx < WALLPASS) && (fy > 1 - WALLPASS)))
	ret = CENTER;
      else
	ret = -1;
      break;

    case ECK_RU:
      if ((fx > 1 - WALLPASS) || (fy > 1 - WALLPASS) ||
	  ((fx < WALLPASS) && (fy < WALLPASS)))
	ret = CENTER;
      else
	ret = -1;
      break;

    case ECK_LU:
      if ((fx < WALLPASS) || (fy > 1 - WALLPASS) ||
	  ((fx > 1 - WALLPASS) && (fy < WALLPASS)))
	ret = CENTER;
      else
	ret = -1;
      break;

    case ECK_LO:
      if ((fx < WALLPASS) || (fy < WALLPASS) ||
	  ((fx > 1 - WALLPASS) && (fy > 1 - WALLPASS)))
	ret = CENTER;
      else
	ret = -1;
      break;

    case T_O:
      if ((fy < WALLPASS) ||
	  ((fy > 1 - WALLPASS) &&
	   ((fx < WALLPASS) || (fx > 1 - WALLPASS))))
	ret = CENTER;
      else
	ret = -1;
      break;

    case T_R:
      if ((fx > 1 - WALLPASS) ||
	  ((fx < WALLPASS) &&
	   ((fy < WALLPASS) || (fy > 1 - WALLPASS))))
	ret = CENTER;
      else
	ret = -1;
      break;

    case T_U:
      if ((fy > 1 - WALLPASS) ||
	  ((fy < WALLPASS) &&
	   ((fx < WALLPASS) || (fx > 1 - WALLPASS))))
	ret = CENTER;
      else
	ret = -1;
      break;

    case T_L:
      if ((fx < WALLPASS) ||
	  ((fx > 1 - WALLPASS) &&
	   ((fy < WALLPASS) || (fy > 1 - WALLPASS))))
	ret = CENTER;
      else
	ret = -1;
      break;

    case H_GANZTUERE:
    case H_HALBTUERE3:
    case H_HALBTUERE2:
      if (Checkpos == LIGHT)
	{
	  ret = CENTER;
	  break;
	}
    case H_HALBTUERE1:
    case H_ZUTUERE:
    case LOCKED_H_ZUTUERE:
      if (Checkpos == LIGHT)
	{
	  ret = -1;
	  break;
	}

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
	  if ((MapBrick == H_GANZTUERE) || (MapBrick == H_HALBTUERE3))
	    ret = CENTER;	// door open
	  else if ((fy < TUERBREITE) || (fy > 1 - TUERBREITE))
	    ret = CENTER;	// door closed, but not entirely in
	  else
	    ret = -1;		// closed door
	}			// directly in the door

      break;
    case V_GANZTUERE:
    case V_HALBTUERE3:
    case V_HALBTUERE2:
      if (Checkpos == LIGHT)
	{
	  ret = CENTER;
	  break;
	}
    case V_HALBTUERE1:
    case V_ZUTUERE:
    case LOCKED_V_ZUTUERE:
      if (Checkpos == LIGHT)
	{
	  ret = -1;
	  break;
	}

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
	  if ((MapBrick == V_GANZTUERE) || (MapBrick == V_HALBTUERE3))
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
  step_num = a_len * 3;

  if (step_num == 0)
    step_num = 1;

  step.x = a_x / step_num;
  step.y = a_y / step_num;

  testpos.x = objpos->x;
  testpos.y = objpos->y;

  for (i = 0; i < step_num + 1 ; i++)
    {

      if ( IsPassable ( testpos.x , testpos.y , objpos->z , LIGHT ) != CENTER)
	{
	  DebugPrintf (2, "\nint IsVisible(Point objpos): Funktionsende erreicht.");
	  return FALSE;
	}

      testpos.x += step.x;
      testpos.y += step.y;

    }
  DebugPrintf (2, "\nint IsVisible(Point objpos): Funktionsende erreicht.");

  return TRUE;
}; // int IsVisible( Point objpos )


#undef _map_c
