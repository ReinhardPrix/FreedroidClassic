#define _map_c

#include <stdio.h>
#include <stdlib.h>
#include <alloc.h>
#include <conio.h>
#include <math.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "defs.h"
#include "struct.h"
#include "proto.h"
#include "global.h"

#include "map.h"


extern SetDebugPos;

/*@Function============================================================
@Desc: unsigned char GetMapBrick(int x, int y): liefert intern-code des
					Elements, das sich auf x/y befindet

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
unsigned char GetMapBrick(int x, int y)
{
	return CurLevel->map[y/BLOCKHOEHE][x/BLOCKBREITE];
}

/*@Function============================================================
@Desc: int GetCurrentElevator: finds Elevator-number to your position 

@Ret: -1: 	Not found !!
		num: 	Number of cur. Elevator in AllElevators[]
		
@Int:
* $Function----------------------------------------------------------*/
int GetCurrentElevator(void)
{
	int i;
	int curlev = CurLevel->levelnum;
	int gx = GrobX, gy = GrobY;

	for( i=0; i<ALLELEVATORS; i++) {
		if( curShip.AllElevators[i].level != curlev ) continue;
		if( (curShip.AllElevators[i].x == gx) &&
				(curShip.AllElevators[i].y == gy))
			break;
	}

	if( i == ALLELEVATORS )	/* keinen gefunden */
		return -1;
	else
		return i;
} /* GetCurrentElevator */


/*@Function============================================================
@Desc: ActSpecialField: checks Influencer on SpecialFields like
							Elevators and Konsoles and acts on it 

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void ActSpecialField(int x, int y)
{
	unsigned char MapBrick;
	int cx, cy;		/* tmp: NullPunkt im Blockzentrum */
	
	MapBrick = GetMapBrick(x, y);

	switch(MapBrick) {
		case LIFT:
			if (!((Me.status == TRANSFERMODE) &&
				(Me.speed.x == 0) && (Me.speed.y == 0))) break;
			
			cx = x % BLOCKBREITE - BLOCKBREITE/2;
			cy = y % BLOCKHOEHE - BLOCKHOEHE/2;
			
			/* Lift nur betreten, wenn ca. im Zentrum */
			if( (cx*cx + cy*cy) < DRUIDRADIUSX*DRUIDRADIUSX)
				EnterElevator();
			break;
			
		case KONSOLE_R:
		case KONSOLE_L:
		case KONSOLE_O:
		case KONSOLE_U:
			if( Me.status == TRANSFERMODE)
				EnterKonsole();
			break;


		case REFRESH1:
		case REFRESH2:
		case REFRESH3:
		case REFRESH4:
			RefreshInfluencer();
			break;
			
		default:
			break;
	} /* switch */


	
} /* ActSpecialField */

/*@Function============================================================
@Desc: 	AnimateRefresh():

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void AnimateRefresh(void)
{
	static OuterWaitCounter = 0;
	static InnerWaitCounter = 0;
	static InnerPhase = 0;		/* Zaehler fuer innere Phase */
	int i,j;
	int x, y;

	OuterWaitCounter ++;
	OuterWaitCounter %= OUTER_REFRESH_COUNTER;
	InnerWaitCounter ++;
	InnerWaitCounter %= INNER_REFRESH_COUNTER;
	
	if( InnerWaitCounter == 0) {
		InnerPhase ++;
		InnerPhase %= INNER_PHASES;
	}
	
	for(i=0; i<MAX_REFRESHES_ON_LEVEL; i++) {
		x = CurLevel->refreshes[i].x;
		y = CurLevel->refreshes[i].y;
		if( x == 0 || y == 0 ) break;

		if( OuterWaitCounter == 0)
			CurLevel->map[y][x] ++;
		if( CurLevel->map[y][x] > REFRESH4 ) CurLevel->map[y][x] = REFRESH1;

		/* Inneres Refresh animieren */
		for( j=0; j<4; j++) {
			MergeBlockToWindow(
				MapBlocks+(unsigned)(I_REFRESH1+InnerPhase)*BLOCKMEM,
				MapBlocks+(unsigned)(REFRESH1+j)*BLOCKMEM,
				BLOCKBREITE,
				FALSE);
		} /* for */
				
	} /* for */
	

} /* AnimateRefresh */

/*@Function============================================================
@Desc: 	LoadShip(): loads the data for a whole ship

@Ret: OK | ERR
@Int:
* $Function----------------------------------------------------------*/
int LoadShip(char *shipname)
{
	struct stat stbuf;
	char *filename;
	FILE *ShipFile;
	char *ShipData;
	char *endpt;			/* Pointer to end-strings */
	char *LevelStart[MAX_LEVELS_ON_SHIP];		/* Pointer to a level-start */
	int level_anz;
	int i;


	/* build complete filename from ship-name */
	filename = (char*)MyMalloc(strlen(shipname)+strlen(SHIP_EXT)+10);
	
	strcpy(filename, shipname);
	strcat(filename, SHIP_EXT);

	/* Read the whole ship-data to memory */
	if( (ShipFile = fopen(filename, "r")) == NULL) {
		printf("File Error ");
		getch();
		return ERR;
	}
	free(filename);
	

	if( fstat(fileno(ShipFile), &stbuf) == EOF) {
		return ERR;
	}

	if( (ShipData = (char far*)farmalloc(stbuf.st_size + 10)) == NULL) {
		printf("\nOut of Memory in LoadShip()");
		getch();
		return ERR;
	}

	fread(ShipData, (size_t)64, (size_t) (stbuf.st_size/64 + 1), ShipFile);

	/*  count the number of levels and remember their start-addresses */
	level_anz = 0;
	endpt = ShipData;
	LevelStart[level_anz] = ShipData;
	
	while( (endpt=strstr(endpt, LEVEL_END_STRING)) != NULL) {
		endpt += strlen(LEVEL_END_STRING);
		level_anz ++;
		LevelStart[level_anz] = endpt+1;
	}

	/* init the level-structs */
	curShip.LevelsOnShip = level_anz;
	
	for( i=0; i<MAX_LEVELS_ON_SHIP; i++) {
		if( i<level_anz) {
			if( (curShip.AllLevels[i] = LevelToStruct(LevelStart[i])) == NULL) {
				return ERR;
			} else {
				TranslateMap(curShip.AllLevels[i]);
			}
			
		} else curShip.AllLevels[i] = NULL;
	}

	/* Get the elevator connections */
	if( GetElevatorConnections(shipname) == ERR) {
		printf("\nErr in GetElevatorConnections ");
		getch();
		return ERR;
	}


	return OK;
}


/*@Function============================================================
@Desc: Level LevelToStruct(char *data):
				Extrahiert die Daten aus *data und schreibt sie in eine
				Level-struct:
					Map- Daten noch NICHT in interne Werte uebersetzt
					Doors and Waypoints Arrays initialisiert

@Ret:  Level or NULL
@Int:
* $Function----------------------------------------------------------*/
Level LevelToStruct(char *data)
{
	Level loadlevel;
	char *pos, *tmp;
	char *map_begin, *wp_begin;
	int i, j;
	int NumWaypoints;
//	int NumDoors, NumRefreshes;
	int zahl;
	

	/* Get the memory for one level */
	loadlevel = (Level)MyMalloc(sizeof(level));

	loadlevel->empty = FALSE;
	
	/* Read Header Data: levelnum and x/ylen */
	sscanf(data, "%u %u %u %u",
		&(loadlevel->levelnum), &(loadlevel->xlen),
		&(loadlevel->ylen), &(loadlevel->color) );

	/* find Map-data */
	if( (map_begin = strstr(data, MAP_BEGIN_STRING)) == NULL) {
		return NULL;
	}

	/* Position on Waypoint-Data */
	if( (wp_begin = strstr(data, WP_BEGIN_STRING)) == NULL) {
		return NULL;
	}

	/* now scan the map */
	strtok(map_begin, "\n");	/* init strtok to map-begin */
	
	/* read MapData */
	for(i=0; i<loadlevel->ylen; i++)
		if( (loadlevel->map[i] = strtok(NULL, "\n")) == NULL) {
			return NULL;
		}

	/* Get Doors Array */
	// NumDoors =
	GetDoors(loadlevel);

	/* Get Waypoints */
	NumWaypoints = GetWaypoints(loadlevel);

	/* Get Refreshes */
	// NumRefreshes =
	GetRefreshes(loadlevel);

	/* Scan the waypoint- connections */
	pos = strtok(wp_begin, "\n");	/* Get Pointer to data-begin */
	
	/* Read Waypoint-data */
	for(i=0; i<NumWaypoints; i++) {
		for(j=0; j<MAX_WP_CONNECTIONS; j++) {
			if( (pos = strtok(NULL, " \n\t")) == NULL) {
				return NULL;
			}
			
			if( sscanf(pos, "%d", &zahl) == EOF) {
				return NULL;
			}
			
			loadlevel->AllWaypoints[i].connections[j] = zahl;
		}
	}
	
	return loadlevel;
} /* LevelToStruct */



/*@Function============================================================
@Desc: GetDoors: initialisiert Doors Array der uebergebenen Level-struct
				ACHTUNG: 	Map-Daten mussen schon in struct sein !!

@Ret: Anz. der Tueren || ERR
@Int:
* $Function----------------------------------------------------------*/
int GetDoors(Level Lev)
{
	int i,line, col;
	int xlen, ylen;
	int curdoor = 0;
	char brick;
	
	xlen = Lev->xlen;
	ylen = Lev->ylen;

	/* init Doors- Array to 0 */
	for(i=0; i<MAX_DOORS_ON_LEVEL; i++)
		Lev->doors[i].x = Lev->doors[i].y = 0;

	/* now find the doors */
	for(line=0; line<ylen; line++) {
		for(col=0; col<xlen; col++) {
			brick = Lev->map[line][col];
			if( brick == '=' || brick == '"' ) {
				Lev->doors[curdoor].x = col;
				Lev->doors[curdoor++].y = line;
				
				if( curdoor > MAX_DOORS_ON_LEVEL) {
					return ERR;
				}
				
			} /* if */
		} /* for */
	} /* for */

	return curdoor;
} /* GetDoors */	
		
/*@Function============================================================
@Desc: GetWaypoints: initialisiert Waypoint-Koordinaten des
						Waypoint-arrays der uebergebenen Level-struct
					ACHTUNG: Map-daten muessen schon in struct stehen 

@Ret: Anz. der Waypoints || ERR
@Int:
* $Function----------------------------------------------------------*/
int GetWaypoints(Level Lev)
{
	int i, line, col;
	int xlen, ylen;
	int curwp = 0;

	xlen = Lev->xlen;
	ylen = Lev->ylen;

	/* Init Wp-array to 0 */
	for(i=0; i<MAXWAYPOINTS; i++) {
		Lev->AllWaypoints[i].x = 0;
		Lev->AllWaypoints[i].y = 0;
	}

	/* Now find the waypoints */
	for(line=0; line<ylen; line++)
		for(col=0; col<xlen; col++) {
			if( Lev->map[line][col] == WAYPOINT_CHAR ) {
				Lev->AllWaypoints[curwp].x = col;
				Lev->AllWaypoints[curwp++].y = line;
				
				if( curwp > MAXWAYPOINTS ) {
					return ERR;
				}
				
			} /* if */
		} /* for */
		
				
	
	return curwp;
	
}	/* GetWaypoints */

/*@Function============================================================
@Desc: int GetRefreshes(Level Lev): legt array der refr. positionen an

@Ret: Number of found refreshes or ERR
@Int:
* $Function----------------------------------------------------------*/
int GetRefreshes(Level Lev)
{
	int i, row, col;
	int xlen, ylen;
	int curref = 0;
	char brick;

	xlen = Lev->xlen;
	ylen = Lev->ylen;

	/* init refreshes array to 0 */
	for( i=0; i<MAX_REFRESHES_ON_LEVEL; i++)
		Lev->refreshes[i].x = Lev->refreshes[i].y = 0;

	/* now find all the refreshes */
	for(row=0; row<ylen; row++)
		for(col=0; col<xlen; col++) {
			if( Lev->map[row][col] == '@' ) {
				Lev->refreshes[curref].x = col;
				Lev->refreshes[curref++].y = row;

				if( curref > MAX_REFRESHES_ON_LEVEL)
					return ERR;

			} /* if */
		} /* for */

	return curref;
}

/*@Function============================================================
@Desc: int TranslateMap(Level Lev): uebersetzt die geladene Karte
			in die internen Werte

@Ret: OK | ERR
@Int:
* $Function----------------------------------------------------------*/
int TranslateMap(Level Lev)
{
	int xdim = Lev->xlen;
	int ydim = Lev->ylen;
	int row, col;
	int i;

	/* transpose these ascii -mapdata to internal numbers for map */
	for( row=0; row<ydim; row++)
		for(col=0; col<xdim; col++) {
			for(i=0; (i<BLOCKANZAHL) &&
				(Translator[i].ascii != Lev->map[row][col]); i++);
			Lev->map[row][col] = Translator[i].intern;
	}

	return OK;
} /* Translate Map */


/*@Function============================================================
@Desc: GetElevatorConnections(char *ship): loads elevator-connctions
					to cur-ship struct

@Ret: 	OK | ERR
@Int:
* $Function----------------------------------------------------------*/
int GetElevatorConnections(char *shipname)
{
	char filename[FILENAME_LEN+1];
	int i;
	FILE *Elevfile;
	int cur_lev, cur_x, cur_y, up, down, elev_row;
	Elevator CurElev;
	
	/* Now get the elevator-connection data from "FILE.elv" file */
	strcpy(filename, shipname);		/* get elevator filename */
	strcat(filename, ELEVEXT);
	
	if( (Elevfile=fopen(filename, "r")) == NULL) return FALSE;
	
	for(i=0; i<ALLELEVATORS; i++) {
		if( fscanf(Elevfile, "%d %d %d %d %d %d",
				&cur_lev, &cur_x, &cur_y, &up, &down, &elev_row) == EOF)
		{
				printf("Illegal Elevator file: %s", filename);
				return FALSE;
		}
		CurElev = &(curShip.AllElevators[i]);
		CurElev->level = cur_lev;
		CurElev->x = cur_x;
		CurElev->y = cur_y;
		CurElev->up = up;
		CurElev->down = down;
		CurElev->elevator_row = elev_row;
	}

	if( fclose(Elevfile) == EOF ) return ERR;

	return OK;
}

/*@Function============================================================
@Desc: int GetCrew(char *shipname): intialisiert Feindesliste

@Ret: OK or ERR
@Int:
* $Function----------------------------------------------------------*/
int GetCrew(char *shipname)
{
	char filename[FILENAME_LEN+1];
	int i;
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
	strcpy(filename, shipname);
	strcat(filename, CREWEXT);


	/* Clear Enmey - Array */
	ClearEnemys();
	
	if( (CrewFile = fopen(filename, "r")) == NULL) return FALSE;

	enemy_nr = 0;
	
	while( fgets(line, linelen, CrewFile) ) {
		if( sscanf(line, "%d %d %d ",
			&level_num, &upper_limit, &lower_limit) == EOF) return ERR;

		if ( strtok(line, ",") == NULL ) return ERR;

		type_anz = 0;
		while( (pos=strtok(NULL, " \t")) != NULL)
			sscanf(pos, "%d", &(types[type_anz++]));
			

		this_limit = random(upper_limit-lower_limit) + lower_limit;
		while( this_limit --) {
			Feindesliste[enemy_nr].type = types[random(type_anz)];
			Feindesliste[enemy_nr].levelnum = level_num;
			Feindesliste[enemy_nr].Status = 0;
			enemy_nr ++;
		}/* while noch_enemy */

		if( enemy_nr >= MAX_ENEMYS_ON_SHIP ) return ERR;
			
	} /* while fgets() */
	
	NumEnemys = enemy_nr;
		
	fclose(CrewFile);

	InitEnemys();		/* Energiewerte richtig setzen */
		
	return OK;
}	
		

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void MoveLevelDoors(void){
	int i,j;
	int ii;
	int doorx, doory;
	long xdist, ydist;
	long dist2;
	int gx, gy;
	char *Pos;

	for(i=0; i<MAX_DOORS_ON_LEVEL; i++) {
		doorx = (CurLevel->doors[i].x);
		doory = (CurLevel->doors[i].y);

		/* Keine weiteren Tueren */
		if ( doorx == 0 && doory == 0 ) break;
		
		Pos = &(CurLevel->map[doory][doorx]);

		doorx = doorx * BLOCKBREITE + BLOCKBREITE/2;
		doory = doory * BLOCKHOEHE + BLOCKHOEHE/2;
		
		/* first check Influencer gegen Tuer */
		xdist = Me.pos.x - doorx;
		ydist = Me.pos.y - doory;
		dist2 = xdist*xdist + ydist*ydist;

		if ( dist2 < DOOROPENDIST2 ) {
			if ((*Pos != H_GANZTUERE )&&( *Pos != V_GANZTUERE ) )
				*Pos += 1;
		} else {
			/* alle Enemys checken */
			for(j=0; j < NumEnemys; j++ ) {
				/* ignore druids that are dead or on other levels */
				if( Feindesliste[j].Status == OUT ||
						Feindesliste[j].levelnum != CurLevel->levelnum )
							continue;
							
				xdist = abs(Feindesliste[j].pos.x - doorx);
				if (xdist < BLOCKBREITE) {
					ydist = abs(Feindesliste[j].pos.y - doory);
					if (ydist < BLOCKHOEHE) {
						dist2 = xdist*xdist + ydist*ydist;
						if ( dist2 < DOOROPENDIST2 ) {
							if ((*Pos != H_GANZTUERE) && (*Pos != V_GANZTUERE) )
								*Pos += 1;

							break; 	/* one druid is enough to open a door */
						} /* if */
					} /* if */
				} /* if */
			} /* for */
			
			/* No druid near: close door if it isnt closed */
			if ( j == NumEnemys)
				if ( (*Pos != V_ZUTUERE) && (*Pos != H_ZUTUERE) )
							*Pos -= 1;
							
		} /* else */
	} /* for */
	

} /* MoveLevelDoors */


/*@Function============================================================
@Desc: 	int DruidPassable(int x, int y) - prueft, ob Pos x/y fuer
						Druid passierbar ist, liefert Richtungswerte, falls
						der Druid von einer Tuer "weggestossen" wird
			

@Ret: 	-1:		Not passable
		Direction:  Druid in Richtung Direction wegschubsen
		CENTER:		Position passable
* $Function----------------------------------------------------------*/
int DruidPassable(int x, int y)
{
	point testpos[DIRECTIONS+1];
	int ret=-1;
	int i;

	/* get 8 Check-Points on the druidsurface */
	testpos[OBEN].x=x;testpos[OBEN].y=y-DRUIDRADIUSY;	
	testpos[RECHTSOBEN].x=x+DRUIDRADIUSXY;testpos[RECHTSOBEN].y=y-DRUIDRADIUSXY;
	testpos[RECHTS].x=x+DRUIDRADIUSX;testpos[RECHTS].y=y;
	testpos[RECHTSUNTEN].x=x+DRUIDRADIUSXY;testpos[RECHTSUNTEN].y=y+DRUIDRADIUSXY;
	testpos[UNTEN].x=x;testpos[UNTEN].y=y+DRUIDRADIUSY;
	testpos[LINKSUNTEN].x=x-DRUIDRADIUSXY;testpos[LINKSUNTEN].y=y+DRUIDRADIUSXY;
	testpos[LINKS].x=x-DRUIDRADIUSX;testpos[LINKS].y=y;
	testpos[LINKSOBEN].x=x-DRUIDRADIUSXY;testpos[LINKSOBEN].y=y-DRUIDRADIUSXY;
		
	for (i=0; i<DIRECTIONS; i++) {
			
		ret = IsPassable(testpos[i].x, testpos[i].y, i);			
					
		if ( ret != CENTER ) break;
			
	} /* for */

	return ret;
}


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
int IsPassable(int x, int y, int Checkpos) {
	int fx, fy;  	/* Feinkoordinaten von x/y */
	unsigned char MapBrick;
	int ret = -1;
	
	MapBrick = GetMapBrick(x, y);

	fx = x % BLOCKBREITE;
	fy = y % BLOCKHOEHE;
	
	switch (MapBrick) {
		case FLOOR:
		case LIFT:
		case VOID:
		case BLOCK4:
		case BLOCK5:
		case REFRESH1:
		case REFRESH2:
		case REFRESH3:
		case REFRESH4:
			ret = CENTER;	/* these are passable */
			break;

		case ALERT:
			if( Checkpos == LIGHT)
				ret = CENTER;
			else
				ret = -1;
			break;
			
		case KONSOLE_L:
 			if( Checkpos == LIGHT) {
				ret = CENTER;
				break;
			}
			if( fx > (BLOCKBREITE-KONSOLEPASS_X) ) ret = CENTER;
			else ret = -1;
			break;
				
		case KONSOLE_R:
			if( Checkpos == LIGHT) {
				ret = CENTER;
				break;
			}
			if( fx < KONSOLEPASS_X ) ret = CENTER;
			else ret = -1;
			break;

		case KONSOLE_O:
			if( Checkpos == LIGHT) {
				ret = CENTER;
				break;
			}		
			if( fy > (BLOCKHOEHE-KONSOLEPASS_Y) )	ret = CENTER;
			else ret = -1;
			break;

		case KONSOLE_U:
			if( Checkpos == LIGHT) {
				ret = CENTER;
				break;
			}		
			if( fy < KONSOLEPASS_Y ) ret = CENTER;
			else ret = -1;
			break;

		case H_WALL:
			if( (fy < WALLPASS) || (fy > BLOCKHOEHE-WALLPASS) ) ret = CENTER;
			else ret =-1;
			break;

		case V_WALL:
			if( (fx < WALLPASS) || (fx > BLOCKBREITE-WALLPASS) ) ret = CENTER;
			else ret =-1;
			break;

		case ECK_RO:
			if( (fx > BLOCKBREITE-WALLPASS) || (fy < WALLPASS) ||
					( (fx < WALLPASS) && (fy > BLOCKHOEHE-WALLPASS)) )
				ret = CENTER;
			else
				ret = -1;
			break;

		case ECK_RU:
			if( (fx > BLOCKBREITE-WALLPASS) || (fy > BLOCKHOEHE-WALLPASS) ||
					( (fx < WALLPASS) && (fy < WALLPASS) ) )
				ret = CENTER;
			else
				ret = -1;
			break;

		case ECK_LU:
			if( (fx < WALLPASS) || (fy > BLOCKHOEHE-WALLPASS) ||
					( (fx > BLOCKBREITE-WALLPASS) && (fy < WALLPASS) ) )
				ret = CENTER;
			else
				ret = -1;
			break;

		case ECK_LO:
			if( (fx < WALLPASS) || (fy < WALLPASS) ||
					( (fx > BLOCKBREITE-WALLPASS) && (fy > BLOCKHOEHE-WALLPASS)))
				ret = CENTER;
			else
				ret = -1;
			break;

		case T_O:
			if( (fy < WALLPASS) ||
					( (fy > BLOCKHOEHE-WALLPASS) &&
					( (fx <WALLPASS) || (fx > BLOCKBREITE-WALLPASS))))
				ret = CENTER;
			else
				ret = -1;
			break;

		case T_R:
			if( (fx>BLOCKBREITE-WALLPASS) ||
					( (fx < WALLPASS) &&
					( (fy < WALLPASS) || (fy > BLOCKHOEHE-WALLPASS))))
				ret = CENTER;
			else
				ret = -1;
			break;

		case T_U:
			if( (fy > BLOCKHOEHE-WALLPASS) ||
					( (fy < WALLPASS) &&
						( (fx < WALLPASS) || (fx > BLOCKBREITE-WALLPASS))))
				ret = CENTER;
			else
				ret = -1;
			break;

		case T_L:
			if( (fx < WALLPASS) ||
					( (fx > BLOCKBREITE-WALLPASS) &&
						( (fy < WALLPASS) || (fy > BLOCKHOEHE-WALLPASS))))
				ret = CENTER;
			else
				ret = -1;
			break;


		case H_GANZTUERE:
		case H_HALBTUERE3:		
		case H_HALBTUERE2:
			if( Checkpos == LIGHT ) {
				 ret = CENTER;
				 break;
			}		
		case H_HALBTUERE1:
		case H_ZUTUERE:
 			if( Checkpos == LIGHT ) {
 				ret = -1;
 				break;
 			}
 			
			/* pruefen, ob Rand der Tuer angefahren */
			if ( ( (fx < H_RANDBREITE) || (fx > (BLOCKBREITE-H_RANDBREITE)) )
					&& ((fy >= H_RANDSPACE) && (fy <= (BLOCKHOEHE-H_RANDSPACE)) ) ){
				/* DRUIDS: Nur bei Fahrt durch Tuer wegstossen */
				if ( (Checkpos != CENTER) && (Checkpos != LIGHT)
					&& (Me.speed.y != 0) ) {
					switch(Checkpos) {
						case RECHTSOBEN:
						case RECHTSUNTEN:
						case RECHTS:
							if(fx > BLOCKBREITE-H_RANDBREITE) ret = LINKS;
							else ret = -1;
							break;
						case LINKSOBEN:
						case LINKSUNTEN:
						case LINKS:
							if(fx < H_RANDBREITE) ret = RECHTS;
							else ret = -1;
							break;
						default:
							ret = -1;
							break;
					} /* switch Checkpos */
				} /* if DRUID && Me.speed.y != 0 */
				else ret = -1;
			} /* if Rand angefahren */
			else {	/* mitten in der Tuer */
				if( (MapBrick == H_GANZTUERE) || (MapBrick == H_HALBTUERE3))
					ret = CENTER; 	/* Tuer offen */
				else if( (fy < TUERBREITE) || (fy > BLOCKHOEHE-TUERBREITE) ) 
					ret = CENTER; /* Tuer zu, aber noch nicht ganz drin */
				else ret = -1;		/* an geschlossener tuer */
			} /* else Mitten in der Tuer */
					
			break;
		case V_GANZTUERE:				
		case V_HALBTUERE3:
		case V_HALBTUERE2:
			if( Checkpos == LIGHT ) {
				ret = CENTER;
				break;
			}
		case V_HALBTUERE1:
		case V_ZUTUERE:
			if( Checkpos == LIGHT ) {
				ret = -1;
				break;
			}

			/* pruefen , ob Rand der Tuer angefahren */
			if ( (fy < V_RANDBREITE || fy > (BLOCKHOEHE-V_RANDBREITE)) &&
				(fx >= V_RANDSPACE && fx <= (BLOCKBREITE-V_RANDSPACE)) ) {
							
				/* DRUIDS: bei Fahrt durch Tuer wegstossen */
				if( (Checkpos != CENTER) && (Checkpos != LIGHT)
					&& (Me.speed.x != 0) ) {
					switch(Checkpos) {
						case RECHTSOBEN:
						case LINKSOBEN:
						case OBEN:
							if( fy < V_RANDBREITE )
								ret = UNTEN;
							else
								ret = -1;
							break;
						case RECHTSUNTEN:
						case LINKSUNTEN:
						case UNTEN:
							if( fy > BLOCKHOEHE-V_RANDBREITE )
								ret = OBEN;
							else
								ret = -1;
							break;
						default:
							ret = -1;
							break;
					} /* switch Checkpos */
				} /* if DRUID && Me.speed.x != 0 */
				else ret = -1;
			} /* if Rand angefahren */
			else {	/* mitten in die tuer */
				if( (MapBrick == V_GANZTUERE) || (MapBrick == V_HALBTUERE3) )
					ret = CENTER;	/* Tuer offen */
				else if( (fx < TUERBREITE) || (fx > BLOCKBREITE-TUERBREITE) )
					ret = CENTER;	/* tuer zu, aber noch nicht ganz dort */
				else
					ret = -1;		/* an geschlossener Tuer */
			} /* else Mitten in der Tuer */

			break;
				
		default:
			ret = -1;
			break;
	} /* switch MapBrick */

	return ret;		

}	/* IsPassable */


/*@Function============================================================
@Desc: 	IsVisible(): 	determines wether object on x/y is visible to
						the 001 or not
@Ret: TRUE/FALSE
@Int:
* $Function----------------------------------------------------------*/
int IsVisible(Point objpos){
	signed int a_x;		/* Vector Influencer->objectpos */
	signed int a_y;
	vect step;			/* effective step */
	int step_len=7;		/* the approx. length of a step-vect. */
	int step_num;		/* number of neccessary steps */
	int a_len;		/* Lenght of a */
	int i;
	point testpos;
	int influ_x = Me.pos.x;
	int influ_y = Me.pos.y;
	static point debugpos = {-1, -1};
	int a, b;

	if( SetDebugPos ) {
		SetDebugPos = FALSE;
		gotoxy(3,3);
		printf("Debug-pos eingeben: ");
		scanf("%d %d", &a, &b);
		debugpos.x = a;
		debugpos.y = b;
	}
		
	a_x = influ_x -objpos->x;
	a_y = influ_y -objpos->y;

	a_len = (int)sqrt((float)((unsigned long)a_x*a_x+a_y*a_y));

	step_num = a_len / step_len;
	if( step_num == 0 ) step_num = 1;
	
	step.x = a_x/step_num;
	step.y = a_y/step_num;
	
	testpos.x = objpos->x;
	testpos.y = objpos->y;

	for( i=0; i<step_num; i++) {
		if( (debugpos.x == objpos->x) && (debugpos.y == objpos->y) ) {
			Feindesliste[i].energy = 100;
			Feindesliste[i].Status = MOBILE;
			Feindesliste[i].levelnum = CurLevel->levelnum;
			Feindesliste[i].pos.x = testpos.x;
			Feindesliste[i].pos.y = testpos.y;
			Feindesliste[i].nextwaypoint = 100;
			Feindesliste[i].type = DEBUG_ENEMY;
		}
	
		testpos.x += step.x;
		testpos.y += step.y;

		if( IsPassable(testpos.x, testpos.y, LIGHT) != CENTER ) return FALSE;
	}
	
	return TRUE;
	

} /* IsVisible */


#undef _map_c

/*=@Header==============================================================
 * $Source$
 *
 * @Desc:	Functions for MAP Managing in Paraplus
 *				mainly file-loading routines
 *	 
 * 	
 * $Revision$
 * $State$
 *
 * $Author$
 *
 * $Log$
 * Revision 1.5  1993/10/02 16:23:07  prix
 * Sun Aug 08 17:18:44 1993: Merged DruidPassable and IsPassable( old: NotPassable)
 * Sun Aug 08 18:12:31 1993: written GetCurrentElevator()
 * Sun Aug 08 22:31:17 1993: added code to read in Waypoint-data in LoadMap()
 * Mon Aug 09 17:52:15 1993: init of wp-list
 * Mon Aug 09 20:01:11 1993: level->empty init
 * Wed Aug 11 20:08:42 1993: Lift to enter only in center of Block
 * Sat Aug 21 14:44:12 1993: new Ship-loading functions from maped imported
 * Sat Aug 21 14:54:42 1993: written GetElevatorConnections
 * Sat Aug 21 15:24:45 1993: written TranslateMap()
 * Tue Aug 24 09:49:25 1993: written AnimateRefresh()
 * Tue Aug 24 10:00:50 1993: call GetRefreshes
 * Tue Aug 24 17:19:10 1993: moved IsVisible to here
 * Wed Aug 25 18:36:21 1993: writing GetCrew()
 * Sat Sep 18 12:44:43 1993: Refreshes langsameer
 * Sat Sep 18 17:44:56 1993: InnerRefresh wird nun animiert
 * Sat Oct 02 12:22:20 1993: dont call GetCrew() in LoadShip, but in InitNewGame()
 *
 * Revision 1.5  1993/10/02  16:23:07  prix
 * Sun Aug 08 17:18:44 1993: Merged DruidPassable and IsPassable( old: NotPassable)
 * Sun Aug 08 18:12:31 1993: written GetCurrentElevator()
 * Sun Aug 08 22:31:17 1993: added code to read in Waypoint-data in LoadMap()
 * Mon Aug 09 17:52:15 1993: init of wp-list
 * Mon Aug 09 20:01:11 1993: level->empty init
 * Wed Aug 11 20:08:42 1993: Lift to enter only in center of Block
 * Sat Aug 21 14:44:12 1993: new Ship-loading functions from maped imported
 * Sat Aug 21 14:54:42 1993: written GetElevatorConnections
 * Sat Aug 21 15:24:45 1993: written TranslateMap()
 * Tue Aug 24 09:49:25 1993: written AnimateRefresh()
 * Tue Aug 24 10:00:50 1993: call GetRefreshes
 * Tue Aug 24 17:19:10 1993: moved IsVisible to here
 * Wed Aug 25 18:36:21 1993: writing GetCrew()
 * Sat Sep 18 12:44:43 1993: Refreshes langsameer
 * Sat Sep 18 17:44:56 1993: InnerRefresh wird nun animiert
 * Sat Oct 02 12:22:20 1993: dont call GetCrew() in LoadShip, but in InitNewGame()
 *
 * Revision 1.4  1993/08/08  21:00:19  prix
 * Wed Aug 04 12:44:47 1993: added include of global.h
 * Wed Aug 04 15:06:42 1993: KONSOLEN richtig passable
 * Thu Aug 05 11:03:16 1993: Walls und Konsolen in DruidPassable bedacht
 * Thu Aug 05 11:20:21 1993: added ECK_RO
 * Thu Aug 05 11:34:44 1993: added ECKEN to DruidPassable
 * Fri Aug 06 14:14:23 1993: added T Kollision checkin in DruidPassable
 * Fri Aug 06 15:38:27 1993: modified LoadMap: dont load from disk but from memory
 * Fri Aug 06 16:12:04 1993: writing LoadShip
 * Sat Aug 07 14:51:01 1993: added GetMapBrick()
 * Sat Aug 07 14:54:21 1993: added calls to GetMapBrick()
 * Sat Aug 07 15:20:35 1993: added ActSpecialField()
 * Sat Aug 07 16:05:32 1993: added Elevator-load to LoadShip
 * Sun Aug 08 16:59:37 1993: DruidPassable verbessert
 *
 * Revision 1.3  1993/08/04  16:31:59  prix
 * Sat Jul 31 12:07:52 1993: load levelnum with map
 * Sat Jul 31 21:42:35 1993: loadmap now generates a list of the doors
 * Sun Aug 01 09:54:04 1993: only one line-terminator !!!!!
 * Sun Aug 01 09:59:54 1993: init doors-map to 0
 * Wed Aug 04 10:41:35 1993: modified LoadMap to new symtrans struct
 * Wed Aug 04 12:31:39 1993: added some functions from paraplus.c
 *
 * Revision 1.2  1993/07/31  16:05:36  prix
 * Thu Jul 29 16:23:49 1993: probably found the crashing error: wrong cast ?? in malloc
 * Thu Jul 29 16:46:18 1993: probably found reason for crash: string not alloc'ed !
 *
 * Revision 1.1  1993/07/29  17:29:28  prix
 * Initial revision
 *
 *
 *-@Header------------------------------------------------------------*/
static const char RCSid[]=\
"$Id$";

