/*----------------------------------------------------------------------
 *
 * Desc: All map-related functions, which also includes loading of decks 
 * and whole ships, starting the elevators and consoles if close to the 
 * paradroid, refreshes as well as determining the map brick that contains
 * specified coordinates are done in this file.
 *
 *----------------------------------------------------------------------*/
/* 
 *
 *   Copyright (c) 1994, 2002 Johannes Prix
 *   Copyright (c) 1994, 2002 Reinhard Prix
 *
 *
 *  This file is part of FreeDroid
 *
 *  FreeDroid is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  FreeDroid is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with FreeDroid; see the file COPYING. If not, write to the 
 *  Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
 *  MA  02111-1307  USA
 *
 */
#include <config.h>

#define _map_c

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "defs.h"
#include "struct.h"
#include "proto.h"
#include "global.h"

#include "map.h"


symtrans Translator[BLOCKANZAHL] = {
  {'.', FLOOR},
  {'\'', VOID},
  {'x', FLOOR},			/* A waypoint is invisible */
  {'+', KREUZ},
  {'-', H_WALL},
  {'|', V_WALL},
  {'"', H_ZUTUERE},
  {'=', V_ZUTUERE},
  {'[', KONSOLE_L},
  {']', KONSOLE_R},
  {'(', KONSOLE_O},
  {')', KONSOLE_U},
  {'o', LIFT},
  {'@', REFRESH1},
  {'a', ALERT},
  {'1', BLOCK1},
  {'2', BLOCK2},
  {'3', BLOCK3},
  {'4', BLOCK4},
  {'5', BLOCK5},
  {0, -1}			// marks the end
};


/*@Function============================================================
  @Desc: unsigned char GetMapBrick(Level deck, float x, float y): liefert
  intern-code des Elements, das sich auf (deck x/y) befindet

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
unsigned char
GetMapBrick (Level deck, float x, float y)
{

  /* 
   * ATTENTION! BE CAREFUL HERE!  What we want is an integer division with rest, 
   * not an exact foating point division!  Beware of "improvements" here!!!
   */

  if (((int) rintf (y)) / BLOCKHOEHE >= deck->ylen)
    {
      printf
	("\nunsigned char GetMapBrick(Level deck, float x, float y): Fehler1! Terminiere...");
      return VOID;
      Terminate (-1);
    }
  if (((int) rintf (x)) / BLOCKBREITE >= deck->xlen)
    {
      printf
	("\nunsigned char GetMapBrick(Level deck, float x, float y): Fehler2! Terminiere...");
      return VOID;
      Terminate (-1);
    }
  if (((int) rintf (y)) / BLOCKHOEHE < 0)
    {
      printf
	("\nunsigned char GetMapBrick(Level deck, float x, float y): Fehler3! Terminiere...");
      return VOID;
      Terminate (-1);
    }
  if (((int) rintf (x)) / BLOCKBREITE < 0)
    {
      printf
	("\nunsigned char GetMapBrick(Level deck, float x, float y): Fehler4! Terminiere...");
      return VOID;
      Terminate (-1);
    }
  return deck->map[((int) rintf (y)) / BLOCKHOEHE][((int) rintf (x)) /
						   BLOCKBREITE];
} /* GetMapBrick() */

/*@Function============================================================
@Desc: int GetCurrentElevator: finds Elevator-number to your position 

@Ret: -1: 	Not found !!
		num: 	Number of cur. Elevator in AllElevators[]
		
@Int:
* $Function----------------------------------------------------------*/
int
GetCurrentElevator (void)
{
  int i;
  int curlev = CurLevel->levelnum;
  int gx = GrobX, gy = GrobY;

  for (i = 0; i < curShip.num_lifts; i++)
    {
      if (curShip.AllElevators[i].level != curlev)
	continue;
      if ((curShip.AllElevators[i].x == gx) &&
	  (curShip.AllElevators[i].y == gy))
	break;
    }

  if (i == curShip.num_lifts)	/* keinen gefunden */
    return -1;
  else
    return i;
}				/* GetCurrentElevator */


/*@Function============================================================
@Desc: ActSpecialField: checks Influencer on SpecialFields like
							Elevators and Konsoles and acts on it 

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void
ActSpecialField (float x, float y)
{
  unsigned char MapBrick;
  int cx, cy;			/* tmp: NullPunkt im Blockzentrum */

  DebugPrintf
    ("\nvoid ActSpecialField(int x, int y):  Real function call confirmed.");

  MapBrick = GetMapBrick (CurLevel, x, y);

  switch (MapBrick)
    {
    case LIFT:
      if (!((Me.status == TRANSFERMODE) &&
	    (Me.speed.x == 0) && (Me.speed.y == 0)))
	break;

      cx = (((int) rintf (x)) % BLOCKBREITE) - BLOCKBREITE / 2;
      cy = (((int) rintf (y)) % BLOCKHOEHE) - BLOCKHOEHE / 2;

      /* Lift nur betreten, wenn ca. im Zentrum */
      if ((cx * cx + cy * cy) < DRUIDRADIUSX * DRUIDRADIUSX)
	EnterElevator ();
      break;

    case KONSOLE_R:
    case KONSOLE_L:
    case KONSOLE_O:
    case KONSOLE_U:
      if (Me.status == TRANSFERMODE)
	{
	  EnterKonsole ();
	  DebugPrintf
	    ("\nvoid ActSpecialField(int x, int y):  Back from EnterKonsole().\n");
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

  DebugPrintf
    ("\nvoid ActSpecialField(int x, int y):  end of function reached.");

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

  DebugPrintf ("\nvoid AnimateRefresh(void):  real function call confirmed.");

  InnerWaitCounter += Frame_Time () * 10;

  // if( (((int)rintf(InnerWaitCounter)) % INNER_REFRESH_COUNTER) == 0) {
  // InnerPhase ++;
  // InnerPhase %= INNER_PHASES;

  InnerPhase = (((int) rintf (InnerWaitCounter)) % INNER_PHASES);


  for (i = 0; i < MAX_REFRESHES_ON_LEVEL; i++)
    {
      x = CurLevel->refreshes[i].x;
      y = CurLevel->refreshes[i].y;
      if (x == 0 || y == 0)
	break;

      CurLevel->map[y][x] = (((int) rintf (InnerWaitCounter)) % 4) + REFRESH1;

      /* Inneres Refresh animieren */
      for (j = 0; j < 4; j++)
	{
	  MergeBlockToWindow (MapBlocks +
			      (unsigned) (I_REFRESH1 + InnerPhase) * BLOCKMEM,
			      MapBlocks + (unsigned) (REFRESH1 +
						      j) * BLOCKMEM,
			      BLOCKBREITE, FALSE);
	}			/* for */

    }				/* for */

  DebugPrintf ("\nvoid AnimateRefresh(void):  end of function reached.");

}				/* AnimateRefresh */

/*@Function============================================================
@Desc: 	LoadShip(): loads the data for a whole ship

@Ret: OK | ERR
@Int:
* $Function----------------------------------------------------------*/
int
LoadShip (char *shipname)
{
  struct stat stbuf;
  char *filename;
  FILE *ShipFile;
  char *ShipData;
  char *endpt;				/* Pointer to end-strings */
  char *LevelStart[MAX_LEVELS];		/* Pointer to a level-start */
  int level_anz;
  int i;


  /* build complete filename from ship-name */
  filename = (char *) MyMalloc (strlen (shipname) + strlen (SHIP_EXT) + 10);

  strcpy (filename, shipname);
  strcat (filename, SHIP_EXT);

  /* Read the whole ship-data to memory */
  if ((ShipFile = fopen (filename, "r")) == NULL)
    {
      DebugPrintf ("\nint LoadShip(char *shipname): Error opening file.... ");
      getchar ();
      return ERR;
    }
  free (filename);


  if (fstat (fileno (ShipFile), &stbuf) == EOF)
    {
      DebugPrintf
	("\nint LoadShip(char* shipname): Error fstat-ing File....");
      return ERR;
    }

  if ((ShipData = (char *) malloc (stbuf.st_size + 10)) == NULL)
    {
      DebugPrintf ("\nint LoadShip(char *shipname): Out of Memory? ");
      getchar ();
      return ERR;
    }

  fread (ShipData, (size_t) 64, (size_t) (stbuf.st_size / 64 + 1), ShipFile);

  /*  count the number of levels and remember their start-addresses */
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
      if ((curShip.AllLevels[i] = LevelToStruct (LevelStart[i])) == NULL)
	return ERR;
      else
	TranslateMap (curShip.AllLevels[i]);
    }

  /* Get the elevator connections */
  if (GetElevatorConnections (shipname) == ERR)
    {
      DebugPrintf ("\nErr in GetElevatorConnections ");
      getchar ();
      return ERR;
    }
  return OK;
} /* LoadShip () */


/*@Function============================================================
 * @Desc: Level LevelToStruct(char *data):
 * 	Extrahiert die Daten aus *data und schreibt sie in eine
 *	Level-struct:
 *	Map- Daten noch NICHT in interne Werte uebersetzt
 *	Doors and Waypoints Arrays initialisiert
 *
 *	@Ret:  Level or NULL
 *	@Int:
* $Function----------------------------------------------------------*/
Level
LevelToStruct (char *data)
{
  Level loadlevel;
  char *pos;
  char *map_begin, *wp_begin;
  int i, j;
  int NumWaypoints;
  /*    int NumDoors, NumRefreshes; */
  int zahl;

  /* Get the memory for one level */
  loadlevel = (Level) MyMalloc (sizeof (level));

  loadlevel->empty = FALSE;

  /* Read Header Data: levelnum and x/ylen */
  sscanf (data, "%u %u %u %u",
	  &(loadlevel->levelnum), &(loadlevel->xlen),
	  &(loadlevel->ylen), &(loadlevel->color));

  /* find Map-data */
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


  /* Get Doors Array */
  // NumDoors =
  GetDoors (loadlevel);

  /* Get Waypoints */
  NumWaypoints = GetWaypoints (loadlevel);

  /* Get Refreshes */
  // NumRefreshes =
  GetRefreshes (loadlevel);

  /* Scan the waypoint- connections */
  pos = strtok (wp_begin, "\n");	/* Get Pointer to data-begin */

  /* Read Waypoint-data */
  for (i = 0; i < NumWaypoints; i++)
    {
      for (j = 0; j < MAX_WP_CONNECTIONS; j++)
	{
	  if ((pos = strtok (NULL, " \n\t")) == NULL)
	    return NULL;
	  if (sscanf (pos, "%d", &zahl) == EOF)
	      return NULL;

	  loadlevel->AllWaypoints[i].connections[j] = zahl;
	}
    }
  return loadlevel;
}				/* LevelToStruct */



/*@Function============================================================
@Desc: GetDoors: initialisiert Doors Array der uebergebenen Level-struct
				ACHTUNG: 	Map-Daten mussen schon in struct sein !!

@Ret: Anz. der Tueren || ERR
@Int:
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
    Lev->doors[i].x = Lev->doors[i].y = 0;

  /* now find the doors */
  for (line = 0; line < ylen; line++)
    {
      for (col = 0; col < xlen; col++)
	{
	  brick = Lev->map[line][col];
	  if (brick == '=' || brick == '"')
	    {
	      Lev->doors[curdoor].x = col;
	      Lev->doors[curdoor++].y = line;

	      if (curdoor > MAX_DOORS_ON_LEVEL)
		{
		  return ERR;
		}

	    }			/* if */
	}			/* for */
    }				/* for */

  return curdoor;
}				/* GetDoors */

/*@Function============================================================
 * @Desc: GetWaypoints: initialisiert Waypoint-Koordinaten des
 *	Waypoint-arrays der uebergebenen Level-struct
 *	ACHTUNG: Map-daten muessen schon in struct stehen 
 *
 * @Ret: Anz. der Waypoints || ERR
 * @Int:
* $Function----------------------------------------------------------*/
int
GetWaypoints (Level Lev)
{
  int i, line, col;
  int xlen, ylen;
  int curwp = 0;

  xlen = Lev->xlen;
  ylen = Lev->ylen;

  /* Init Wp-array to 0 */
  for (i = 0; i < MAXWAYPOINTS; i++)
    {
      Lev->AllWaypoints[i].x = 0;
      Lev->AllWaypoints[i].y = 0;
    }

  /* Now find the waypoints */
  for (line = 0; line < ylen; line++)
    for (col = 0; col < xlen; col++)
      if (Lev->map[line][col] == WAYPOINT_CHAR)
	{
	  Lev->AllWaypoints[curwp].x = col;
	  Lev->AllWaypoints[curwp].y = line;
	  curwp ++;

	  if (curwp > MAXWAYPOINTS)
	    return ERR;
	} /* if (WAYPOINT_CHAR) */

  return curwp;  /* return number of waypoints found */

} /* GetWaypoints() */

/*@Function============================================================
@Desc: int GetRefreshes(Level Lev): legt array der refr. positionen an

@Ret: Number of found refreshes or ERR
@Int:
* $Function----------------------------------------------------------*/
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
	if (Lev->map[row][col] == '@')
	  {
	    Lev->refreshes[curref].x = col;
	    Lev->refreshes[curref++].y = row;

	    if (curref > MAX_REFRESHES_ON_LEVEL)
	      return ERR;

	  }			/* if */
      }				/* for */
  return curref;
}				// int GetRefreshed(Level lev)


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


/*-----------------------------------------------------------------
 * @Desc: uebersetzt die geladene Karte in die internen Werte
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
  int i;
  int WAbove, WBelow, WLeft, WRight;	// Walls around CROSS? yes=1,no=0
  int environs;			// encodes the "Wall-environment" of a "+"
  int NewBlock = KREUZ;		// Neuen "Eck-Block" in den wir KREUZ verwandeln

  DebugPrintf
    ("\nint TranslateMap(Level Lev): real function call confirmed.");

  /* Erste Runde: transpose these ascii -mapdata to internal numbers for map */
  for (row = 0; row < ydim; row++)
    {
      for (col = 0; col < xdim; col++)
	{
	  for (i = 0;
	       Translator[i].ascii
	       && (Translator[i].ascii != Lev->map[row][col]); i++);

	  if (!Translator[i].ascii)
	    {
	      gl_printf (20, 5, "In TranslateMap: Unknown map-char: %c\n",
			 Lev->map[row][col]);
	      keyboard_close ();
	      getchar ();
	      Terminate (-1);
	    }
	  else
	    Lev->map[row][col] = Translator[i].intern;
	}
    }				/* for (row=0..) */

  // Zweiter Durchlauf: Kreuze "abschleifen", 
  //     i.e. in entsprechende Ecken umwandeln wo noetig
  for (row = 0; row < ydim; row++)
    {
      for (col = 0; col < xdim; col++)
	{
	  if (Lev->map[row][col] != KREUZ)
	    continue;
	  // KREUZ: mal die Nachbarn ansehen: "Wall" or not?
	  WAbove = (row > 0) ? IsWallBlock (Lev->map[row - 1][col]) : 0;
	  WBelow =
	    (row < ydim - 1) ? IsWallBlock (Lev->map[row + 1][col]) : 0;
	  WLeft = (col > 0) ? IsWallBlock (Lev->map[row][col - 1]) : 0;
	  WRight =
	    (col < xdim - 1) ? IsWallBlock (Lev->map[row][col + 1]) : 0;

	  // encode this environment into one single number:
	  environs = 0x1000 * WAbove + 0x100 * WRight + 0x10 * WBelow + WLeft;

	  // ... und unnoetige Enden entfernen:
	  switch (environs)
	    {
	    case 0x0000:	// no walls around
	    case 0x1000:	// just one connecting wall, bit lonely?
	    case 0x0100:
	    case 0x0010:
	    case 0x0001:
	      printf ("\nUnconnected '+' found on Level %i.\n",
		      Lev->levelnum);
	      break;

	    case 0x1010:	// just part of a straight wall, a bit redundant?
	    case 0x0101:
	      printf ("\nUnconnected '+' found on Level %i.\n",
		      Lev->levelnum);
	      // don't do anything
	      break;

	      // pure corners
	    case 0x1100:
	      NewBlock = ECK_LU;
	      break;
	    case 0x0110:
	      NewBlock = ECK_LO;
	      break;
	    case 0x0011:
	      NewBlock = ECK_RO;
	      break;
	    case 0x1001:
	      NewBlock = ECK_RU;
	      break;

	      // T - connectors
	    case 0x1110:
	      NewBlock = T_L;
	      break;
	    case 0x1101:
	      NewBlock = T_U;
	      break;
	    case 0x1011:
	      NewBlock = T_R;
	      break;
	    case 0x0111:
	      NewBlock = T_O;
	      break;

	      // full cross
	    case 0x1111:
	      NewBlock = KREUZ;
	      break;
	    default:
	      DebugPrintf ("\nMap-panic. TranslateMap() is messed up!\n");
	      DebugPrintf
		("\nint TranslateMap(Level Lev): end of function reached.");
	      return (ERR);
	      break;
	    }			// switch(environs)
	  Lev->map[row][col] = NewBlock;	// ok, hope we got it right ;)
	}			/* for(col) */
    }				/* for(row) */

  DebugPrintf ("\nint TranslateMap(Level Lev): end of function reached.");
  return OK;
}				// int Translate Map(Level lev)


/*@Function============================================================
@Desc: GetElevatorConnections(char *ship): loads elevator-connctions
					to cur-ship struct

@Ret: 	OK | ERR
@Int:
* $Function----------------------------------------------------------*/
int
GetElevatorConnections (char *shipname)
{
  char filename[FILENAME_LEN + 1];
  int i;
  FILE *Elevfile;
  int cur_lev, cur_x, cur_y, up, down, elev_row;
  Elevator CurElev;

  /* Now get the elevator-connection data from "FILE.elv" file */
  strcpy (filename, shipname);	/* get elevator filename */
  strcat (filename, ELEVEXT);

  if ((Elevfile = fopen (filename, "r")) == NULL)
    return FALSE;

  for (i = 0; i < MAX_LIFTS; i++)
    {
      if (fscanf (Elevfile, "%d %d %d %d %d %d",
		  &cur_lev, &cur_x, &cur_y, &up, &down, &elev_row) == EOF)
	{
	  curShip.num_lifts = i;
	  break;
	}

      CurElev = &(curShip.AllElevators[i]);
      CurElev->level = cur_lev;
      CurElev->x = cur_x;
      CurElev->y = cur_y;
      CurElev->up = up;
      CurElev->down = down;
      CurElev->elevator_row = elev_row;
    }

  if (fclose (Elevfile) == EOF)
    return ERR;

  return OK;
}				// int GetElevatorConnections(char *shipname)

/*-----------------------------------------------------------------
 * @Desc: intialisiert Feindesliste
 *
 * @Ret: OK or ERR
 * 
 *-----------------------------------------------------------------*/
int
GetCrew (char *shipname)
{
  char filename[FILENAME_LEN + 1];
  FILE *CrewFile;
  int level_num;
  int enemy_nr;
  int type_anz;
  int types[MAX_TYPES_ON_LEVEL];
  int upper_limit, lower_limit;
  int this_limit;
  int linelen = CREW_LINE_LEN;
  char line[CREW_LINE_LEN];
  char *pos;

  /* get filename */
  strcpy (filename, shipname);
  strcat (filename, CREWEXT);

  /* Clear Enemy - Array */
  ClearEnemys ();

  if ((CrewFile = fopen (filename, "r")) == NULL)
    {
      printf ("\nCouldn't open crew-file: %s \n", filename);
      return (ERR);
    }

  enemy_nr = 0;
  while (fgets (line, linelen, CrewFile))  /* one line per level */
    {
      if (sscanf (line, "%d %d %d ",
		  &level_num, &upper_limit, &lower_limit) == EOF)
	{
	  printf ("\n Read Error in crew-file %s !\n", filename);
	  return (ERR);
	}

      if (strtok (line, ",") == NULL)
	{
	  printf ("\n Read Error in crew-file %s !\n", filename);
	  return (ERR);
	}

      type_anz = 0;
      while ((pos = strtok (NULL, " \t")) != NULL)
	sscanf (pos, "%d", &(types[type_anz++]));

      this_limit = MyRandom (upper_limit - lower_limit) + lower_limit;

      while (this_limit--)
	{
	  Feindesliste[enemy_nr].type = types[MyRandom (type_anz-1)];
	  Feindesliste[enemy_nr].levelnum = level_num;
	  Feindesliste[enemy_nr].Status = 0;
	  enemy_nr++;
	} /* while (enemy-limit of this level not reached) */

      if (enemy_nr >= MAX_ENEMYS_ON_SHIP)
	{
	  printf ("\nToo many enemys on ship: %s! \n", filename);
	  return (ERR);
	}

    }	/* while (lines in crew-file to read) */

  NumEnemys = enemy_nr;

  fclose (CrewFile);

  InitEnemys ();		/* Energiewerte richtig setzen */

  return (OK);

} /* GetCrew () */


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
MoveLevelDoors (void)
{
  int i, j;
  int doorx, doory;
  long xdist, ydist;
  long dist2;
  char *Pos;

  for (i = 0; i < MAX_DOORS_ON_LEVEL; i++)
    {
      doorx = (CurLevel->doors[i].x);
      doory = (CurLevel->doors[i].y);

      /* Keine weiteren Tueren */
      if (doorx == 0 && doory == 0)
	break;

      Pos = &(CurLevel->map[doory][doorx]);

      doorx = doorx * BLOCKBREITE + BLOCKBREITE / 2;
      doory = doory * BLOCKHOEHE + BLOCKHOEHE / 2;

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
	      if (Feindesliste[j].Status == OUT ||
		  Feindesliste[j].levelnum != CurLevel->levelnum)
		continue;

	      xdist = abs (Feindesliste[j].pos.x - doorx);
	      if (xdist < BLOCKBREITE)
		{
		  ydist = abs (Feindesliste[j].pos.y - doory);
		  if (ydist < BLOCKHOEHE)
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
DruidPassable (int x, int y)
{
  point testpos[DIRECTIONS + 1];
  int ret = -1;
  int i;

  /* get 8 Check-Points on the druidsurface */
  testpos[OBEN].x = x;
  testpos[OBEN].y = y - DRUIDRADIUSY;
  testpos[RECHTSOBEN].x = x + DRUIDRADIUSXY;
  testpos[RECHTSOBEN].y = y - DRUIDRADIUSXY;
  testpos[RECHTS].x = x + DRUIDRADIUSX;
  testpos[RECHTS].y = y;
  testpos[RECHTSUNTEN].x = x + DRUIDRADIUSXY;
  testpos[RECHTSUNTEN].y = y + DRUIDRADIUSXY;
  testpos[UNTEN].x = x;
  testpos[UNTEN].y = y + DRUIDRADIUSY;
  testpos[LINKSUNTEN].x = x - DRUIDRADIUSXY;
  testpos[LINKSUNTEN].y = y + DRUIDRADIUSXY;
  testpos[LINKS].x = x - DRUIDRADIUSX;
  testpos[LINKS].y = y;
  testpos[LINKSOBEN].x = x - DRUIDRADIUSXY;
  testpos[LINKSOBEN].y = y - DRUIDRADIUSXY;

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
IsPassable (int x, int y, int Checkpos)
{
  int fx, fy;			/* Feinkoordinaten von x/y */
  unsigned char MapBrick;
  int ret = -1;

  MapBrick = GetMapBrick (CurLevel, (float) x, (float) y);

  fx = x % BLOCKBREITE;
  fy = y % BLOCKHOEHE;

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
      ret = CENTER;		/* these are passable */
      break;

    case ALERT:
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
      if (fx > (BLOCKBREITE - KONSOLEPASS_X))
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
      if (fy > (BLOCKHOEHE - KONSOLEPASS_Y))
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
      if ((fy < WALLPASS) || (fy > BLOCKHOEHE - WALLPASS))
	ret = CENTER;
      else
	ret = -1;
      break;

    case V_WALL:
      if ((fx < WALLPASS) || (fx > BLOCKBREITE - WALLPASS))
	ret = CENTER;
      else
	ret = -1;
      break;

    case ECK_RO:
      if ((fx > BLOCKBREITE - WALLPASS) || (fy < WALLPASS) ||
	  ((fx < WALLPASS) && (fy > BLOCKHOEHE - WALLPASS)))
	ret = CENTER;
      else
	ret = -1;
      break;

    case ECK_RU:
      if ((fx > BLOCKBREITE - WALLPASS) || (fy > BLOCKHOEHE - WALLPASS) ||
	  ((fx < WALLPASS) && (fy < WALLPASS)))
	ret = CENTER;
      else
	ret = -1;
      break;

    case ECK_LU:
      if ((fx < WALLPASS) || (fy > BLOCKHOEHE - WALLPASS) ||
	  ((fx > BLOCKBREITE - WALLPASS) && (fy < WALLPASS)))
	ret = CENTER;
      else
	ret = -1;
      break;

    case ECK_LO:
      if ((fx < WALLPASS) || (fy < WALLPASS) ||
	  ((fx > BLOCKBREITE - WALLPASS) && (fy > BLOCKHOEHE - WALLPASS)))
	ret = CENTER;
      else
	ret = -1;
      break;

    case T_O:
      if ((fy < WALLPASS) ||
	  ((fy > BLOCKHOEHE - WALLPASS) &&
	   ((fx < WALLPASS) || (fx > BLOCKBREITE - WALLPASS))))
	ret = CENTER;
      else
	ret = -1;
      break;

    case T_R:
      if ((fx > BLOCKBREITE - WALLPASS) ||
	  ((fx < WALLPASS) &&
	   ((fy < WALLPASS) || (fy > BLOCKHOEHE - WALLPASS))))
	ret = CENTER;
      else
	ret = -1;
      break;

    case T_U:
      if ((fy > BLOCKHOEHE - WALLPASS) ||
	  ((fy < WALLPASS) &&
	   ((fx < WALLPASS) || (fx > BLOCKBREITE - WALLPASS))))
	ret = CENTER;
      else
	ret = -1;
      break;

    case T_L:
      if ((fx < WALLPASS) ||
	  ((fx > BLOCKBREITE - WALLPASS) &&
	   ((fy < WALLPASS) || (fy > BLOCKHOEHE - WALLPASS))))
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
      if (((fx < H_RANDBREITE) || (fx > (BLOCKBREITE - H_RANDBREITE)))
	  && ((fy >= H_RANDSPACE) && (fy <= (BLOCKHOEHE - H_RANDSPACE))))
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
		  if (fx > BLOCKBREITE - H_RANDBREITE)
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
	  else if ((fy < TUERBREITE) || (fy > BLOCKHOEHE - TUERBREITE))
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
      if ((fy < V_RANDBREITE || fy > (BLOCKHOEHE - V_RANDBREITE)) &&
	  (fx >= V_RANDSPACE && fx <= (BLOCKBREITE - V_RANDSPACE)))
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
		  if (fy > BLOCKHOEHE - V_RANDBREITE)
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
	  else if ((fx < TUERBREITE) || (fx > BLOCKBREITE - TUERBREITE))
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
@Desc: 	IsVisible(): 	determines wether object on x/y is visible to
						the 001 or not
@Ret: TRUE/FALSE
@Int:
* $Function----------------------------------------------------------*/
int
IsVisible (Finepoint objpos)
{
  signed int a_x;		/* Vector Influencer->objectpos */
  signed int a_y;
  vect step;			/* effective step */
  int step_len = 7;		/* the approx. length of a step-vect. */
  int step_num;			/* number of neccessary steps */
  int a_len;			/* Lenght of a */
  int i;
  point testpos;
  int influ_x = Me.pos.x;
  int influ_y = Me.pos.y;

  DebugPrintf ("\nint IsVisible(Point objpos): Funktion echt aufgerufen.");

  a_x = influ_x - objpos->x;
  a_y = influ_y - objpos->y;

  a_len = (int) sqrt ((float) ((unsigned long) a_x * a_x + a_y * a_y));

  step_num = a_len / step_len;
  if (step_num == 0)
    step_num = 1;

  step.x = a_x / step_num;
  step.y = a_y / step_num;

  testpos.x = objpos->x;
  testpos.y = objpos->y;

  for (i = 0; i < step_num; i++)
    {

      testpos.x += step.x;
      testpos.y += step.y;

      if (IsPassable (testpos.x, testpos.y, LIGHT) != CENTER)
	{
	  DebugPrintf
	    ("\nint IsVisible(Point objpos): Funktionsende erreicht.");
	  return FALSE;
	}
    }
  DebugPrintf ("\nint IsVisible(Point objpos): Funktionsende erreicht.");

  return TRUE;
}				// int IsVisible(Point objpos)


#undef _map_c
