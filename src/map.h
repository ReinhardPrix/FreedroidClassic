/* 
 *
 *   Copyright (c) 1994, 2002 Johannes Prix
 *   Copyright (c) 1994, 2002 Reinhard Prix
 *
 *
 *  This file is part of FreeParadroid+
 *
 *  FreeParadroid+ is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  FreeParadroid+ is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with FreeParadroid+; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/*----------------------------------------------------------------------
 *
 * Desc:  Definitions for the map.c file
 *
 *----------------------------------------------------------------------*/

#ifndef _map_h
#define _map_h

/* some defines */

#define OUTER_REFRESH_COUNTER 2
#define INNER_REFRESH_COUNTER 4
#define INNER_PHASES		4

#define MAX_TYPES_ON_LEVEL		30
#define CREW_LINE_LEN			MAX_TYPES_ON_LEVEL * 3 + 20

/* Distance, where door opens */
#define DOOROPENDIST2 	(BLOCKHOEHE + BLOCKBREITE)*(BLOCKBREITE + BLOCKHOEHE)/4

/* Randbreite der Wand */
#define WALLPASS		4

/* Randbreite der Konsolen */
#define KONSOLEPASS_X 	(BLOCKBREITE/2 + 4)
#define KONSOLEPASS_Y 	(BLOCKHOEHE/2 	+4)

/* Breite der Tueren freien Raumes vor Tuer*/
#define TUERBREITE	6

/* Rand der offenen Tueren */
#define V_RANDSPACE		WALLPASS
#define V_RANDBREITE		5

#define H_RANDSPACE		WALLPASS
#define H_RANDBREITE		5

/* Konstanten die die Kartenwerte anschaulich machen */
enum {
FLOOR=0,
ECK_LU, T_U, ECK_RU, T_L, KREUZ, T_R, ECK_LO, T_O, ECK_RO,
H_WALL, V_WALL, ALERT, BLOCK1, BLOCK2, BLOCK3, BLOCK4, BLOCK5, 
H_ZUTUERE, H_HALBTUERE1, H_HALBTUERE2, H_HALBTUERE3, H_GANZTUERE,
KONSOLE_L, KONSOLE_R, KONSOLE_O, KONSOLE_U,
V_ZUTUERE, V_HALBTUERE1, V_HALBTUERE2, V_HALBTUERE3, V_GANZTUERE,
LIFT, VOID, REFRESH1, REFRESH2, REFRESH3, REFRESH4,
I_REFRESH1, I_REFRESH2, I_REFRESH3, I_REFRESH4, INVISIBLE_BRICK
};


#define WAYPOINT_CHAR		'x'
#define NO_WAYPOINT  		-1

/* Extensions for Map and Elevator - data */
#define FILENAME_LEN	128
#define SHIP_EXT	".shp"
#define ELEVEXT		".elv"
#define CREWEXT		".crw"


/* string - signs in ship-data files */
#define MAP_BEGIN_STRING	"map"
#define WP_BEGIN_STRING		"wp"
#define LEVEL_END_STRING	"end"


typedef struct {
	char ascii;			/* the map-symbols in ascii notation */
	int intern;			/* the map-symbols in internal notation */
} symtrans;


enum _colornames {
	PD_RED,
	PD_YELLOW,
	PD_GREEN,
	PD_GRAY,
	PD_BLUE,
	PD_GREENBLUE,
	PD_DARK
};

#if  (defined _gen_c) || (defined _map_c)

/* Color - names */
char *ColorNames[] = {
	"Red",
	"Yellow",
	"Green",
	"Gray",
	"Blue",
	"GreenBlue",
	"Dark",
	NULL
};
#else
	extern char* ColorNames[];
#endif



#endif
