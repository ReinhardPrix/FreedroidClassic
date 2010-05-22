/*
 *
 *   Copyright (c) 1994, 2002, 2003  Johannes Prix
 *   Copyright (c) 1994, 2002, 2003  Reinhard Prix
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

/*----------------------------------------------------------------------
 *
 * Desc: All map-related functions, which also includes loading of decks
 * and whole ships, starting the lifts and consoles if close to the
 * paradroid, refreshes as well as determining the map brick that contains
 * specified coordinates are done in this file.
 *
 *----------------------------------------------------------------------*/

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

void ResetLevelMap (Level Lev);
void GetThisLevelsDroids( char* SectionPointer );

/*@Function============================================================
  @Desc: unsigned char GetMapBrick(Level deck, float x, float y): liefert
  intern-code des Elements, das sich auf (deck x/y) befindet

@Ret:
@Int:
* $Function----------------------------------------------------------*/
unsigned char
GetMapBrick (Level deck, float x, float y)
{
  int xx, yy;

  xx = (int) rintf(x);
  yy = (int) rintf(y);

  if ( (yy >= deck->ylen)|| (yy < 0) || (xx >= deck->xlen) || (xx<0) )
    return VOID;
  else
    return (deck->map[yy][xx]);
} /* GetMapBrick() */

/*@Function============================================================
@Desc: int GetCurrentLift: finds Lift-number to your position

@Ret: -1: 	Not found !!
		num: 	Number of cur. Lift in AllLifts[]

@Int:
* $Function----------------------------------------------------------*/
int
GetCurrentLift (void)
{
  int i;
  int curlev = CurLevel->levelnum;
  int gx, gy;

  gx = rintf(Me.pos.x);
  gy = rintf(Me.pos.y);

  DebugPrintf( 1 , "\nint GetCurrentLift( void ): curlev=%d gx=%d gy=%d" , curlev, gx, gy );
  DebugPrintf( 1 , "\nint GetCurrentLift( void ): List of elevators:\n");
  for (i = 0; i < curShip.num_lifts+1; i++)
    {
      DebugPrintf( 1 , "\nIndex=%d level=%d gx=%d gy=%d" , i ,
		   curShip.AllLifts[i].level , curShip.AllLifts[i].x , curShip.AllLifts[i].y );
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
}; // GetCurrentLift


/*@Function============================================================
@Desc: ActSpecialField: checks Influencer on SpecialFields like
Lifts and Konsoles and acts on it

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void
ActSpecialField (float x, float y)
{
  unsigned char MapBrick;
  float cx, cy;			/* tmp: NullPunkt im Blockzentrum */
  float myspeed2;

  DebugPrintf (2, "\nvoid ActSpecialField(int x, int y):  Real function call confirmed.");

  MapBrick = GetMapBrick (CurLevel, x, y);

  myspeed2 = Me.speed.x*Me.speed.x + Me.speed.y*Me.speed.y;

  switch (MapBrick)
    {
    case LIFT:
      if ( myspeed2 > 1.0 )
	break;

      if ( (Me.status == ACTIVATE) || (GameConfig.TakeoverActivates && (Me.status==TRANSFERMODE)) )
	{
	  cx = rintf(x) - x ;
	  cy = rintf(y) - y ;

	  /* Lift nur betreten, wenn ca. im Zentrum */
	  if ((cx * cx + cy * cy) < Droid_Radius * Droid_Radius)
	    EnterLift ();
	}

      break;

    case KONSOLE_R:
    case KONSOLE_L:
    case KONSOLE_O:
    case KONSOLE_U:
      if (myspeed2 > 1.0)
	break;
      if ( (Me.status == ACTIVATE) || (GameConfig.TakeoverActivates && (Me.status==TRANSFERMODE)))
	{
	  EnterKonsole ();
	  DebugPrintf (2, "\nvoid ActSpecialField(int x, int y):  Back from EnterKonsole().\n");
	}
      break;


    case REFRESH1:
    case REFRESH2:
    case REFRESH3:
    case REFRESH4:
      RefreshInfluencer ();
      break;

    default:
      break;
    }				/* switch */

  DebugPrintf (2, "\nvoid ActSpecialField(int x, int y):  end of function reached.");

} /* ActSpecialField */

/*@Function============================================================
@Desc: 	AnimateRefresh():

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void
AnimateRefresh (void)
{
  static float InnerWaitCounter = 0;
  static int InnerPhase = 0;	/* Zaehler fuer innere Phase */
  int i, j;
  int x, y;

  DebugPrintf (2, "\nvoid AnimateRefresh(void):  real function call confirmed.");

  InnerWaitCounter += Frame_Time () * 10;

  // if( (((int)rintf(InnerWaitCounter)) % INNER_REFRESH_COUNTER) == 0) {
  // InnerPhase ++;
  // InnerPhase %= INNER_PHASES;

  InnerPhase = (((int) rintf (InnerWaitCounter)) % INNER_PHASES);


  for (i = 0; i < MAX_REFRESHES_ON_LEVEL; i++)
    {
      x = CurLevel->refreshes[i].x;
      y = CurLevel->refreshes[i].y;
      if (x == -1 || y == -1)
	break;

      CurLevel->map[y][x] = (((int) rintf (InnerWaitCounter)) % 4) + REFRESH1;

      /* Inneres Refresh animieren */
      for (j = 0; j < 4; j++)
	{
	  ;  /* nix hier noch... */ // FIXME
	}			/* for */

    }				/* for */

  DebugPrintf (2, "\nvoid AnimateRefresh(void):  end of function reached.");

  return;

}				/* AnimateRefresh */

/*@Function============================================================
@Desc: 	LoadShip(): loads the data for a whole ship

@Ret: OK | ERR
@Int:
* $Function----------------------------------------------------------*/
int
LoadShip (char *filename)
{
  char *fpath;
  char *ShipData;
  char *endpt;				/* Pointer to end-strings */
  char *LevelStart[MAX_LEVELS];		/* Pointer to a level-start */
  int level_anz;
  char *Buffer;
  int i;

#define END_OF_SHIP_DATA_STRING "*** End of Ship Data ***"

  /* Read the whole ship-data to memory */
  fpath = find_file (filename, MAP_DIR, NO_THEME, CRITICAL);
  ShipData = ReadAndMallocAndTerminateFile( fpath , END_OF_SHIP_DATA_STRING ) ;

  //--------------------
  // Now we read the Area-name from the loaded data
  //
  Buffer = ReadAndMallocStringFromData (ShipData, AREA_NAME_STRING, "\"");
  strncpy (curShip.AreaName, Buffer, 99);
  curShip.AreaName[99]='\0';
  free (Buffer);

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
      curShip.AllLevels[i] = LevelToStruct (LevelStart[i]);

      if (curShip.AllLevels[i] == NULL)
	{
	  DebugPrintf (0, "ERROR: reading of level %d failed\n", i);
	  return (ERR);
	}
      InterpretMap (curShip.AllLevels[i]); // initialize doors, refreshes and lifts
    }

  free (ShipData);

  return OK;

} /* LoadShip () */

/*@Function============================================================
@Desc: char *StructToMem(Level Lev):

@Ret: 	char *: pointer to Map in a memory field
@Int:
* $Function----------------------------------------------------------*/
char *StructToMem(Level Lev)
{
  char *LevelMem;
  int i, j;
  int MemAmount=0;		/* the size of the level-data */
  int xlen = Lev->xlen, ylen = Lev->ylen;
  int anz_wp;		/* number of Waypoints */
  char linebuf[500];		/* Buffer */
  waypoint *this_wp;

  anz_wp = Lev->num_waypoints;

  /* estimate the amount of memory needed */
  MemAmount = (xlen+1) * ylen; 	/* Map-memory */
  MemAmount += anz_wp * MAX_WP_CONNECTIONS * 4;
  MemAmount += 50000;		/* Puffer fuer Dimensionen, mark-strings .. */

  /* allocate some memory */
  if( (LevelMem = (char*)MyMalloc(MemAmount)) == NULL) {
    DebugPrintf(1, "\n\nError in StructToMem:  Could not allocate memory...\n\nTerminating...\n\n");
    Terminate(ERR);
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

    ResetLevelMap (Lev); // make sure all doors are closed
    for (j=0; j<xlen; j++)
      {
	sprintf (linebuf, "%2d ", Lev->map[i][j]);
	strcat (LevelMem, linebuf);
      }
    strcat(LevelMem, "\n");
  }

  // --------------------
  // The next thing we must do is write the waypoints of this level

  strcat(LevelMem, WP_BEGIN_STRING);
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
	  sprintf(linebuf, "%2d ", this_wp->connections[j]);
	  strcat(LevelMem, linebuf);
	} /* for connections */
      strcat(LevelMem, "\n");
    } /* for waypoints */

  strcat(LevelMem, LEVEL_END_STRING);
  strcat(LevelMem, "\n----------------------------------------------------------------------\n");

  /* FERTIG:   hat die Memory - Schaetzung gestimmt ?? */
  /* wenn nicht: :-(  */
  if( strlen(LevelMem) >= MemAmount)
    {
      DebugPrintf(0, "\n\nError in StructToMem:  Estimate of memory was wrong...\nTerminating...\n");
      Terminate(ERR);
    }

  /* all ok : */
  return (LevelMem);

} /* Struct to Mem */

/*@Function============================================================
@Desc: int SaveShip(void): saves ship-data to disk

@Ret: OK | ERR
@Int:
* $Function----------------------------------------------------------*/
int SaveShip(char *shipname)
{
  char *LevelMem;		/* linear memory for one Level */
  char *MapHeaderString;
  FILE *ShipFile;  // to this file we will save all the ship data...
  char filename[FILENAME_LEN+1];
  int level_anz;
  int array_i, array_num;
  int i;

  DebugPrintf (2, "\nint SaveShip(char *shipname): real function call confirmed.");

  /* Get the complete filename */
  strcpy(filename, shipname);
  strcat(filename, SHIP_EXT);

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
This file was generated using the Freedroid level editor.\n\
Please feel free to make any modifications you like, but in order for you\n\
to have an easier time, it is recommended that you use the Freedroid level\n\
editor for this purpose.  If you have created some good new maps, please \n\
send a short notice (not too large files attached) to the freedroid project.\n\
\n\
freedroid-discussion@lists.sourceforge.net\n\
----------------------------------------------------------------------\n\
\n";
  fwrite ( MapHeaderString , strlen( MapHeaderString), sizeof(char), ShipFile);

  // Now we write the area name back into the file
  fwrite ( AREA_NAME_STRING , strlen( AREA_NAME_STRING ), sizeof(char), ShipFile);
  fwrite ( curShip.AreaName , strlen( curShip.AreaName ), sizeof(char), ShipFile);
  fwrite( "\"\n\n  ", strlen( "\"\n\n  " ) , sizeof(char) , ShipFile );

  /* Save all Levels */

  DebugPrintf (2, "\nint SaveShip(char *shipname): now saving levels...");

  for( i=0; i<level_anz; i++)
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
      LevelMem = StructToMem(curShip.AllLevels[array_num]);
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
} /* SaveShip */


/*@Function============================================================
 * @Desc: Level LevelToStruct(char *data):
 *      This function is for LOADING map data!
 * 	This function extracts the data from *data and writes them
 *      into a Level-struct:
 *
 *	Doors and Waypoints Arrays are initialized too
 *
 *	@Ret:  Level or NULL
* $Function----------------------------------------------------------*/
Level
LevelToStruct (char *data)
{
  Level loadlevel;
  char *map_begin, *wp_begin, *level_end;
  char *this_line, *next_line;
  char *pos;
  size_t len;
  int i;
  int nr, x, y;
  int k;
  int connection;
  char* DataPointer;
  int res;
  int tmp;

  /* Get the memory for one level */
  loadlevel = (Level) MyMalloc (sizeof (level));

  loadlevel->empty = FALSE;

  DebugPrintf (2, "\n-----------------------------------------------------------------");
  DebugPrintf (2, "\nStarting to process information for another level:\n");

  /* Read Header Data: levelnum and x/ylen */
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

  loadlevel->Levelname = ReadAndMallocStringFromData ( data , LEVEL_NAME_STRING , "\n" );
  loadlevel->Background_Song_Name = ReadAndMallocStringFromData ( data , BACKGROUND_SONG_NAME_STRING , "\n" );
  loadlevel->Level_Enter_Comment = ReadAndMallocStringFromData ( data , LEVEL_ENTER_COMMENT_STRING , "\n" );

  // find the map data
  if ((map_begin = strstr (data, MAP_BEGIN_STRING)) == NULL)
    return(NULL);

  /* set position to Waypoint-Data */
  if ((wp_begin = strstr (data, WP_BEGIN_STRING)) == NULL)
    return(NULL);

  // find end of level-data
  if ((level_end = strstr (data, LEVEL_END_STRING)) == NULL)
    return(NULL);

  /* now scan the map */
  next_line = map_begin;
  this_line = strtok (next_line, "\n");

  /* read MapData */
  for (i = 0; i < loadlevel->ylen; i++)
    {
      if ((this_line = strtok (NULL, "\n")) == NULL)
	return(NULL);
      loadlevel->map[i] = MyMalloc( loadlevel->xlen + 10 );
      pos = this_line;
      pos += strspn (pos, WHITE_SPACE);  // skip initial whitespace

      for (k=0; k < loadlevel->xlen; k++)
	{
	  if (*pos == '\0')
	    return (NULL);
	  res = sscanf (pos, "%d", &tmp);
          *(loadlevel->map[i]+k) = (char)tmp;
	  if ( (res == 0) || (res == EOF) )
	    return (NULL);
	  pos += strcspn (pos, WHITE_SPACE);  // skip last token
	  pos += strspn (pos, WHITE_SPACE);   // skip initial whitespace of next one

	}
    }

  /* Get Waypoints */
  next_line = wp_begin;
  this_line = strtok (next_line, "\n");

  for (i=0; i<MAXWAYPOINTS ; i++)
    {
      if ( (this_line = strtok (NULL, "\n")) == NULL)
	return (NULL);
      if (this_line == level_end)
	{
	  loadlevel->num_waypoints = i;
	  break;
	}

      sscanf( this_line , "Nr.=%d \t x=%d \t y=%d" , &nr , &x , &y );

      loadlevel->AllWaypoints[i].x=x;
      loadlevel->AllWaypoints[i].y=y;

      pos = strstr (this_line, CONNECTION_STRING);
      pos += strlen (CONNECTION_STRING);	// skip connection-string
      pos += strspn (pos, WHITE_SPACE); 		// skip initial whitespace

      for ( k=0 ; k<MAX_WP_CONNECTIONS ; k++ )
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

  return (loadlevel);

} /* LevelToStruct */



/*@Function============================================================
@Desc: GetDoors: initializes the Doors array of the given level structure
Of course the level data must be in the structure already!!

@Ret: Number of doors found or ERR
* $Function----------------------------------------------------------*/
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
    Lev->doors[i].x = Lev->doors[i].y = -1;

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
}				/* GetDoors */

/*@Function============================================================
@Desc: This function initialized the array of Refreshes for animation
       within the level

@Ret: Number of refreshes found or ERR
* $Function----------------------------------------------------------*/
int
GetRefreshes (Level Lev)
{
  int i, row, col;
  int xlen, ylen;
  int curref = 0;

  xlen = Lev->xlen;
  ylen = Lev->ylen;

  /* init refreshes array to -1 */
  for (i = 0; i < MAX_REFRESHES_ON_LEVEL; i++)
    Lev->refreshes[i].x = Lev->refreshes[i].y = -1;

  /* now find all the refreshes */
  for (row = 0; row < ylen; row++)
    for (col = 0; col < xlen; col++)
      {
	if (Lev->map[row][col] == REFRESH1 )
	  {
	    Lev->refreshes[curref].x = col;
	    Lev->refreshes[curref++].y = row;

	    if (curref > MAX_REFRESHES_ON_LEVEL)
	      {
		fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
The number of refreshes found in level %d seems to be greater than the number\n\
of refreshes currently allowed in a freedroid map.\n\
\n\
The constant for the maximum number of refreshes currently is set to %d in the\n\
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
}				// int GetRefreshed(Level lev)


//----------------------------------------------------------------------
// Find all alerts on this level and initialize their position-array
//----------------------------------------------------------------------
void
GetAlerts (Level Lev)
{
  int i, row, col;
  int xlen, ylen;
  int curref = 0;

  xlen = Lev->xlen;
  ylen = Lev->ylen;

  // init alert array to -1
  for (i = 0; i < MAX_ALERTS_ON_LEVEL; i++)
    Lev->alerts[i].x = Lev->alerts[i].y = -1;

  // now find all the alerts
  for (row = 0; row < ylen; row++)
    for (col = 0; col < xlen; col++)
      {
	if (Lev->map[row][col] == ALERT_GREEN)
	  {
	    Lev->alerts[curref].x = col;
	    Lev->alerts[curref++].y = row;

	    if (curref > MAX_ALERTS_ON_LEVEL)
	      {
		DebugPrintf(0, "WARNING: more alert-tiles found on level %d than allowed (%d)!!",
			    Lev->levelnum, MAX_ALERTS_ON_LEVEL);
		DebugPrintf(0, "Remaining Alerts will be inactive... \n");
		break;
	      }
	  }    // if alert found

      }	// for cols

  return;

} // int GetAlerts()






/*======================================================================
  IsWallBlock():  Returns TRUE (1) for blocks classified as "Walls",
  		  0 otherwise
 ======================================================================*/
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
    case ECK_LU:
    case T_U:
    case ECK_RU:
    case T_L:
    case T_R:
    case ECK_LO:
    case T_O:
    case ECK_RO:
      return (TRUE);
    default:
      return (FALSE);
    }				// switch
}				// IsWallBlock()

/*----------------------------------------------------------------------
 * close all doors and set refreshes to first phase for "canonical map"
 *
 ----------------------------------------------------------------------*/
void
ResetLevelMap (Level Lev)
{
  int col;
  int i;

  // Now in the game and in the level editor, it might have happend that some open
  // doors occur.  The make life easier for the saving routine, these doors should
  // be closed first.

  for (col=0; col < Lev->xlen; col++)
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

	    case ALERT_GREEN:
	    case ALERT_YELLOW:
	    case ALERT_AMBER:
	    case ALERT_RED:
	      Lev->map[i][col] = ALERT_GREEN;
	      break;
	    default:
	      break;
	    }
	}
    }

  return;

} // ResetLevelMap



/*-----------------------------------------------------------------
 * @Desc: initialize doors, refreshes and lifts for the given level-data
 *
 * @Ret: OK | ERR
 *
 *-----------------------------------------------------------------*/
int
InterpretMap (Level Lev)
{
  /* Get Doors Array */
  GetDoors ( Lev );

  // Get Refreshes
  GetRefreshes ( Lev );

  // Get Alerts
  GetAlerts (Lev);

  return(OK);
}


/*@Function============================================================
@Desc: GetLiftConnections(char *ship): loads lift-connctions
					to cur-ship struct

@Ret: 	OK | ERR
@Int:
* $Function----------------------------------------------------------*/
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

  int x,y,w,h;

#define END_OF_LIFT_DATA_STRING "*** End of elevator specification file ***"
#define START_OF_LIFT_DATA_STRING "*** Beginning of Lift Data ***"
#define START_OF_LIFT_RECTANGLE_DATA_STRING "*** Beginning of elevator rectangles ***"
#define END_OF_LIFT_CONNECTION_DATA_STRING "*** End of Lift Connection Data ***"



  /* Now get the lift-connection data from "FILE.elv" file */

  fpath = find_file (filename, MAP_DIR, NO_THEME, CRITICAL);

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

  curShip.num_lift_rows = 0;
  while ( ( EntryPointer = strstr( EntryPointer , "Elevator Number=" ) ) != NULL )
    {
      ReadValueFromString (EntryPointer, "Elevator Number=", "%d", &ElevatorIndex);
      EntryPointer ++;

      ReadValueFromString (EntryPointer, "ElRowX=", "%d", &x);
      ReadValueFromString (EntryPointer, "ElRowY=", "%d", &y);
      ReadValueFromString (EntryPointer, "ElRowW=", "%d", &w);
      ReadValueFromString (EntryPointer, "ElRowH=", "%d", &h);

      curShip.LiftRow_Rect[ElevatorIndex].x = x;
      curShip.LiftRow_Rect[ElevatorIndex].y = y;
      curShip.LiftRow_Rect[ElevatorIndex].w = w;
      curShip.LiftRow_Rect[ElevatorIndex].h = h;

      curShip.num_lift_rows ++;
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
      ReadValueFromString (EntryPointer, "DeckNr=", "%d", &DeckIndex);
      ReadValueFromString (EntryPointer, "RectNumber=", "%d", &RectIndex);
      EntryPointer ++;  // to prevent doubly taking this entry

      curShip.num_level_rects[ DeckIndex ] ++; // count the number of rects for this deck one up

      ReadValueFromString (EntryPointer, "DeckX=", "%d", &x);
      ReadValueFromString (EntryPointer, "DeckY=", "%d", &y);
      ReadValueFromString (EntryPointer, "DeckW=", "%d", &w);
      ReadValueFromString (EntryPointer, "DeckH=", "%d", &h);

      curShip.Level_Rects[DeckIndex][RectIndex].x = x;
      curShip.Level_Rects[DeckIndex][RectIndex].y = y;
      curShip.Level_Rects[DeckIndex][RectIndex].w = w;
      curShip.Level_Rects[DeckIndex][RectIndex].h = h;
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
      ReadValueFromString (EntryPointer, "Label=", "%d", &Label);
      CurLift = &(curShip.AllLifts[Label]);
      EntryPointer++; // to avoid doubly taking this entry

      ReadValueFromString (EntryPointer, "Deck=", "%d", &(CurLift->level));
      ReadValueFromString (EntryPointer, "PosX=", "%d", &(CurLift->x));
      ReadValueFromString (EntryPointer, "PosY=", "%d", &(CurLift->y));
      ReadValueFromString (EntryPointer, "LevelUp=", "%d", &(CurLift->up));
      ReadValueFromString (EntryPointer, "LevelDown=", "%d", &(CurLift->down));
      ReadValueFromString (EntryPointer, "LiftRow=", "%d", &(CurLift->lift_row));
    }

  curShip.num_lifts = Label;

  return OK;
}; // int GetLiftConnections(char *shipname)

/*-----------------------------------------------------------------
 * @Desc: This function initializes all enemys
 *
 *
 * @Ret: OK or ERR
 *
 *-----------------------------------------------------------------*/
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

  /* Clear Enemy - Array */
  ClearEnemys ();

  //--------------------
  //Now its time to start decoding the droids file.
  //For that, we must get it into memory first.
  //The procedure is the same as with LoadShip
  //
  fpath = find_file (filename, MAP_DIR, NO_THEME, CRITICAL);

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
  NumEnemys = 0;
  for (i = 0; i < MAX_ENEMYS_ON_SHIP; i++)
    {
      type = AllEnemys[i].type;
      if ( type == (-1) ) continue;  // Do nothing to unused entries
      AllEnemys[i].energy = Druidmap[type].maxenergy;
      AllEnemys[i].status = MOBILE;
      NumEnemys++;
    }

  return (OK);
} /* GetCrew () */

/*
----------------------------------------------------------------------
This function receives a pointer to the already read in crew section
in a already read in droids file and decodes all the contents of that
droid section to fill the AllEnemys array with droid types accoriding
to the specifications made in the file.
----------------------------------------------------------------------
*/
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
  ReadValueFromString (SectionPointer, DROIDS_LEVEL_INDICATION_STRING, "%d", &OurLevelNumber);

  // Now we read in the maximal number of random droids for this level
  ReadValueFromString (SectionPointer, DROIDS_MAXRAND_INDICATION_STRING, "%d", &MaxRand);

  // Now we read in the minimal number of random droids for this level
  ReadValueFromString (SectionPointer, DROIDS_MINRAND_INDICATION_STRING, "%d", &MinRand);

  DifferentRandomTypes=0;
  SearchPointer = SectionPointer;
  while ( ( SearchPointer = strstr ( SearchPointer , ALLOWED_TYPE_INDICATION_STRING)) != NULL)
    {
      SearchPointer += strlen ( ALLOWED_TYPE_INDICATION_STRING );
      strncpy( TypeIndicationString , SearchPointer , 3 ); // Every type is 3 characters long
      TypeIndicationString[3]=0;
      // Now that we have got a type indication string, we only need to translate it
      // into a number corresponding to that droid in the droid list
      for ( ListIndex = 0 ; ListIndex < Number_Of_Droid_Types ; ListIndex++ )
	{
	  if ( !strcmp( Druidmap[ListIndex].druidname , TypeIndicationString ) ) break ;
	}
      if ( ListIndex >= Number_Of_Droid_Types )
	{
	  DebugPrintf (0, "ERROR: unknown droid type: %s found in data file for level %d\n",
		       TypeIndicationString , OurLevelNumber);
	  Terminate(ERR);
	}
      else
	DebugPrintf( 1 , "\nType indication string %s translated to type Nr.%d." ,
		     TypeIndicationString , ListIndex );
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
	  if ( AllEnemys[ FreeAllEnemysPosition ].status == OUT ) break;
	}
      if ( FreeAllEnemysPosition == MAX_ENEMYS_ON_SHIP )
	{
	  printf("\n\n No more free position to fill random droids into in GetCrew...Terminating....");
	  Terminate(ERR);
	}

      AllEnemys[ FreeAllEnemysPosition ].type = ListOfTypesAllowed[MyRandom (DifferentRandomTypes-1)];
      AllEnemys[ FreeAllEnemysPosition ].levelnum = OurLevelNumber;
      AllEnemys[ FreeAllEnemysPosition ].status = MOBILE;

    }  // while (enemy-limit of this level not reached)


  return;
}


/*@Function============================================================
@Desc: This funtion moves the level doors in the sense that they are opened
       or closed depending on whether there is a robot close to the door or
       not.  Initially this function did not take into account the framerate
       and just worked every frame.  But this WASTES COMPUTATION time and it
       DOES THE ANIMATION TOO QUICKLY.  So, the most reasonable way out seems
       to be to operate this function only from time to time, e.g. after a
       specified delay has passed.

@Ret:
* $Function----------------------------------------------------------*/
void
MoveLevelDoors (void)
{
  int i, j;
  int doorx, doory;
  float xdist, ydist;
  float dist2;
  char *Pos;

  // This prevents animation going too quick.
  // The constant should be replaced by a variable, that can be
  // set from within the theme, but that may be done later...
  if ( LevelDoorsNotMovedTime < Time_For_Each_Phase_Of_Door_Movement ) return;
  LevelDoorsNotMovedTime=0;


  for (i = 0; i < MAX_DOORS_ON_LEVEL; i++)
    {
      doorx = (CurLevel->doors[i].x);
      doory = (CurLevel->doors[i].y);

      /* Keine weiteren Tueren */
      if (doorx == -1 && doory == -1)
	break;

      Pos = &(CurLevel->map[doory][doorx]);

      // NORMALISATION doorx = doorx * Block_Rect.w + Block_Rect.w / 2;
      // NORMALISATION doory = doory * Block_Rect.h + Block_Rect.h / 2;

      /* first check Influencer gegen Tuer */
      xdist = Me.pos.x - doorx;
      ydist = Me.pos.y - doory;
      dist2 = xdist * xdist + ydist * ydist;

      if (dist2 < DOOROPENDIST2)
	{
	  if ((*Pos != H_GANZTUERE) && (*Pos != V_GANZTUERE))
	    *Pos += 1;
	}
      else
	{
	  /* alle Enemys checken */
	  for (j = 0; j < NumEnemys; j++)
	    {
	      /* ignore druids that are dead or on other levels */
	      if (AllEnemys[j].status == OUT || AllEnemys[j].status == TERMINATED ||
		  AllEnemys[j].levelnum != CurLevel->levelnum)
		continue;

	      xdist = abs (AllEnemys[j].pos.x - doorx);
	      if (xdist < Block_Rect.w)
		{
		  ydist = abs (AllEnemys[j].pos.y - doory);
		  if (ydist < Block_Rect.h)
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
}				/* MoveLevelDoors */


/*@Function============================================================
@Desc: 	int DruidPassable(int x, int y) - prueft, ob Pos x/y fuer
Druid passierbar ist, liefert Richtungswerte, falls
der Druid von einer Tuer "weggestossen" wird

@Ret: 	-1:		Not passable
Direction:  Druid in Richtung Direction wegschubsen
CENTER:		Position passable
* $Function----------------------------------------------------------*/
int
DruidPassable (float x, float y)
{
  finepoint testpos[DIRECTIONS + 1];
  int ret = -1;
  int i;

  /* get 8 Check-Points on the druidsurface */
  testpos[OBEN].x = x;
  testpos[OBEN].y = y - Droid_Radius;
  testpos[RECHTSOBEN].x = x + Droid_Radius;
  testpos[RECHTSOBEN].y = y - Droid_Radius;
  testpos[RECHTS].x = x + Droid_Radius;
  testpos[RECHTS].y = y;
  testpos[RECHTSUNTEN].x = x + Droid_Radius;
  testpos[RECHTSUNTEN].y = y + Droid_Radius;
  testpos[UNTEN].x = x;
  testpos[UNTEN].y = y + Droid_Radius;
  testpos[LINKSUNTEN].x = x - Droid_Radius;
  testpos[LINKSUNTEN].y = y + Droid_Radius;
  testpos[LINKS].x = x - Droid_Radius;
  testpos[LINKS].y = y;
  testpos[LINKSOBEN].x = x - Droid_Radius;
  testpos[LINKSOBEN].y = y - Droid_Radius;

  for (i = 0; i < DIRECTIONS; i++)
    {

      ret = IsPassable (testpos[i].x, testpos[i].y, i);

      if (ret != CENTER)
	break;

    }				/* for */

  return ret;
}				// int DruidPassable(int x, int y)


/*@Function============================================================
@Desc: IsPassable(int x, int y, int Checkpos):
prueft, ob der Punkt x/y passierbar ist
Checkpos: Falls Druid gecheckt wird: aktuelle Check-position
Checkpos = CENTER means: No Druid check

@Ret: CENTER: 	TRUE
Directions + (-1) : FALSE

Directions mean Push Druid if it is one, else is's not passable
@Int:
* $Function----------------------------------------------------------*/
int
IsPassable (float x, float y, int Checkpos)
{
  float fx, fy;
  unsigned char MapBrick;
  int ret = -1;

  MapBrick = GetMapBrick (CurLevel, x, y);

  fx = (x-0.5) - floor(x-0.5);
  fy = (y-0.5) - floor(y-0.5);

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
    case FINE_GRID:
      ret = CENTER;		/* these are passable */
      break;

    case ALERT_GREEN:
    case ALERT_YELLOW:
    case ALERT_AMBER:
    case ALERT_RED:
      if (Checkpos == LIGHT)
	ret = CENTER;
      else
	ret = -1;
      break;

    case KONSOLE_L:
      if (Checkpos == LIGHT)
	{
	  ret = CENTER;
	  break;
	}
      //NORMALISATION      if (fx > (Block_Rect.w - KONSOLEPASS_X))
      if (fx > (1.0 - KONSOLEPASS_X))
	ret = CENTER;
      else
	ret = -1;
      break;

    case KONSOLE_R:
      if (Checkpos == LIGHT)
	{
	  ret = CENTER;
	  break;
	}
      if (fx < KONSOLEPASS_X)
	ret = CENTER;
      else
	ret = -1;
      break;

    case KONSOLE_O:
      if (Checkpos == LIGHT)
	{
	  ret = CENTER;
	  break;
	}
      //NORMALISATION if (fy > (Block_Rect.h - KONSOLEPASS_Y))
      if (fy > (1 - KONSOLEPASS_Y))
	ret = CENTER;
      else
	ret = -1;
      break;

    case KONSOLE_U:
      if (Checkpos == LIGHT)
	{
	  ret = CENTER;
	  break;
	}
      if (fy < KONSOLEPASS_Y)
	ret = CENTER;
      else
	ret = -1;
      break;

    case H_WALL:
      //NORMALISATION if ((fy < WALLPASS) || (fy > Block_Rect.h - WALLPASS))
      if ((fy < WALLPASS) || (fy > 1 - WALLPASS))
	ret = CENTER;
      else
	ret = -1;
      break;

    case V_WALL:
      //NORMALISATION if ((fx < WALLPASS) || (fx > Block_Rect.w - WALLPASS))
      if ((fx < WALLPASS) || (fx > 1 - WALLPASS))
	ret = CENTER;
      else
	ret = -1;
      break;

    case ECK_RO:
      //NORMALISATION if ((fx > Block_Rect.w - WALLPASS) || (fy < WALLPASS) ||
      if ((fx > 1 - WALLPASS) || (fy < WALLPASS) ||
	  //NORMALISATION ((fx < WALLPASS) && (fy > Block_Rect.h - WALLPASS)))
	  ((fx < WALLPASS) && (fy > 1 - WALLPASS)))
	ret = CENTER;
      else
	ret = -1;
      break;

    case ECK_RU:
      //NORMALISATION if ((fx > Block_Rect.w - WALLPASS) || (fy > Block_Rect.h - WALLPASS) ||
      if ((fx > 1 - WALLPASS) || (fy > 1 - WALLPASS) ||
	  ((fx < WALLPASS) && (fy < WALLPASS)))
	ret = CENTER;
      else
	ret = -1;
      break;

    case ECK_LU:
      //NORMALISATION if ((fx < WALLPASS) || (fy > Block_Rect.h - WALLPASS) ||
      if ((fx < WALLPASS) || (fy > 1 - WALLPASS) ||
	  //NORMALISATION ((fx > Block_Rect.w - WALLPASS) && (fy < WALLPASS)))
	  ((fx > 1 - WALLPASS) && (fy < WALLPASS)))
	ret = CENTER;
      else
	ret = -1;
      break;

    case ECK_LO:
      if ((fx < WALLPASS) || (fy < WALLPASS) ||
	  //NORMALISATION ((fx > Block_Rect.w - WALLPASS) && (fy > Block_Rect.h - WALLPASS)))
	  ((fx > 1 - WALLPASS) && (fy > 1 - WALLPASS)))
	ret = CENTER;
      else
	ret = -1;
      break;

    case T_O:
      if ((fy < WALLPASS) ||
	  //NORMALISATION ((fy > Block_Rect.h - WALLPASS) &&
	  ((fy > 1 - WALLPASS) &&
	   //NORMALISATION ((fx < WALLPASS) || (fx > Block_Rect.w - WALLPASS))))
	   ((fx < WALLPASS) || (fx > 1 - WALLPASS))))
	ret = CENTER;
      else
	ret = -1;
      break;

    case T_R:
      //NORMALISATION if ((fx > Block_Rect.w - WALLPASS) ||
      if ((fx > 1 - WALLPASS) ||
	  ((fx < WALLPASS) &&
	   //NORMALISATION ((fy < WALLPASS) || (fy > Block_Rect.h - WALLPASS))))
	   ((fy < WALLPASS) || (fy > 1 - WALLPASS))))
	ret = CENTER;
      else
	ret = -1;
      break;

    case T_U:
      //NORMALISATION if ((fy > Block_Rect.h - WALLPASS) ||
      if ((fy > 1 - WALLPASS) ||
	  ((fy < WALLPASS) &&
	   //NORMALISATION ((fx < WALLPASS) || (fx > Block_Rect.w - WALLPASS))))
	   ((fx < WALLPASS) || (fx > 1 - WALLPASS))))
	ret = CENTER;
      else
	ret = -1;
      break;

    case T_L:
      if ((fx < WALLPASS) ||
	  //NORMALISATION ((fx > Block_Rect.w - WALLPASS) &&
	  ((fx > 1 - WALLPASS) &&
	   //NORMALISATION ((fy < WALLPASS) || (fy > Block_Rect.h - WALLPASS))))
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
      if (Checkpos == LIGHT)
	{
	  ret = -1;
	  break;
	}

      /* pruefen, ob Rand der Tuer angefahren */
      //NORMALISATION if (((fx < H_RANDBREITE) || (fx > (Block_Rect.w - H_RANDBREITE)))
      if (((fx < H_RANDBREITE) || (fx > (1 - H_RANDBREITE)))
	  //NORMALISATION && ((fy >= H_RANDSPACE) && (fy <= (Block_Rect.h - H_RANDSPACE))))
	  && ((fy >= H_RANDSPACE) && (fy <= (1 - H_RANDSPACE))))
	{
	  /* DRUIDS: Nur bei Fahrt durch Tuer wegstossen */
	  if ((Checkpos != CENTER) && (Checkpos != LIGHT)
	      && (Me.speed.y != 0))
	    {
	      switch (Checkpos)
		{
		case RECHTSOBEN:
		case RECHTSUNTEN:
		case RECHTS:
		  //NORMALISATION if (fx > Block_Rect.w - H_RANDBREITE)
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
	    }			/* if DRUID && Me.speed.y != 0 */
	  else
	    ret = -1;
	}			/* if Rand angefahren */
      else
	{			/* mitten in der Tuer */
	  if ((MapBrick == H_GANZTUERE) || (MapBrick == H_HALBTUERE3))
	    ret = CENTER;	/* Tuer offen */
	  //NORMALISATION else if ((fy < TUERBREITE) || (fy > Block_Rect.h - TUERBREITE))
	  else if ((fy < TUERBREITE) || (fy > 1 - TUERBREITE))
	    ret = CENTER;	/* Tuer zu, aber noch nicht ganz drin */
	  else
	    ret = -1;		/* an geschlossener tuer */
	}			/* else Mitten in der Tuer */

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
      if (Checkpos == LIGHT)
	{
	  ret = -1;
	  break;
	}

      /* pruefen , ob Rand der Tuer angefahren */
      //NORMALISATION if ((fy < V_RANDBREITE || fy > (Block_Rect.h - V_RANDBREITE)) &&
      if ((fy < V_RANDBREITE || fy > (1 - V_RANDBREITE)) &&
	  //NORMALISATION (fx >= V_RANDSPACE && fx <= (Block_Rect.w - V_RANDSPACE)))
	  (fx >= V_RANDSPACE && fx <= ( 1 - V_RANDSPACE)))
	{

	  /* DRUIDS: bei Fahrt durch Tuer wegstossen */
	  if ((Checkpos != CENTER) && (Checkpos != LIGHT)
	      && (Me.speed.x != 0))
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
		  //NORMALISATION if (fy > Block_Rect.h - V_RANDBREITE)
		  if (fy > 1 - V_RANDBREITE)
		    ret = OBEN;
		  else
		    ret = -1;
		  break;
		default:
		  ret = -1;
		  break;
		}		/* switch Checkpos */
	    }			/* if DRUID && Me.speed.x != 0 */
	  else
	    ret = -1;
	}			/* if Rand angefahren */
      else
	{			/* mitten in die tuer */
	  if ((MapBrick == V_GANZTUERE) || (MapBrick == V_HALBTUERE3))
	    ret = CENTER;	/* Tuer offen */
	  //NORMALISATION else if ((fx < TUERBREITE) || (fx > Block_Rect.w - TUERBREITE))
	  else if ((fx < TUERBREITE) || (fx > 1 - TUERBREITE))
	    ret = CENTER;	/* tuer zu, aber noch nicht ganz dort */
	  else
	    ret = -1;		/* an geschlossener Tuer */
	}			/* else Mitten in der Tuer */

      break;

    default:
      ret = -1;
      break;
    }				/* switch MapBrick */

  return ret;

}				/* IsPassable */


/*@Function============================================================
@Desc: 	IsVisible(): determines wether object on x/y is visible to
	the 001 or not
@Ret: TRUE/FALSE
@Int:
* $Function----------------------------------------------------------*/
int
IsVisible (Finepoint objpos)
{
  float a_x;		/* Vector Influencer->objectpos */
  float a_y;
  finepoint step;			/* effective step */
  int step_num;			/* number of neccessary steps */
  float a_len;			/* Lenght of a */
  int i;
  finepoint testpos;
  float influ_x = Me.pos.x;
  float influ_y = Me.pos.y;

  DebugPrintf (2, "\nint IsVisible(Point objpos): Funktion echt aufgerufen.");

  a_x = influ_x - objpos->x;
  a_y = influ_y - objpos->y;

  a_len = sqrt (  a_x * a_x + a_y * a_y );
  step_num = a_len * 4.0;

  if (step_num == 0)
    step_num = 1;

  step.x = a_x / step_num;
  step.y = a_y / step_num;

  testpos.x = objpos->x;
  testpos.y = objpos->y;

  for (i = 1; i < step_num; i++)
    {

      testpos.x += step.x;
      testpos.y += step.y;

      if (IsPassable (testpos.x, testpos.y, LIGHT) != CENTER)
	{
	  DebugPrintf (2, "\nint IsVisible(Point objpos): Funktionsende erreicht.");
	  return FALSE;
	}
    }
  DebugPrintf (2, "\nint IsVisible(Point objpos): Funktionsende erreicht.");

  return TRUE;
}				// int IsVisible(Point objpos)


#undef _map_c
