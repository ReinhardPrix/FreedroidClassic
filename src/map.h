/*=@Header==============================================================
 * $Source$
 *
 * @Desc:  Definitions for the map.c file
 * 	
 * $Revision$
 * $State$
 *
 * $Author$
 *
 * $Log$
 * Revision 1.3  1994/06/19 16:43:06  prix
 * clean up
 *
 * Revision 1.2  1993/10/25  17:54:27  prix
 * Fri Jul 30 11:50:42 1993: new door phases: map werte verschoben
 * Wed Aug 04 10:37:50 1993: made symtrans struct more readable
 * Wed Aug 04 13:18:53 1993: added some defines from defs.h that belong to here
 * Wed Aug 04 13:28:56 1993: Konstanten fuer Kartenwerte
 * Wed Aug 04 13:32:05 1993: moved PUSHSPEED to defs.h again
 * Wed Aug 04 13:39:20 1993: moved BLOCKANZAHL in here
 * Wed Aug 04 13:42:34 1993: removed BLOCKANZAHL again
 * Wed Aug 04 15:12:04 1993: added KONSOLEPASS_*
 * Sun Aug 08 22:39:41 1993: added Waypoint-symbol (x) to Translator
 * Mon Aug 09 18:15:02 1993: moved DRUIDRADIUS* to defs.h
 * Sat Aug 21 15:00:42 1993: NEW DEFINES
 * Sat Aug 21 15:01:42 1993: added some defines of maped.h
 * Sat Aug 21 19:30:31 1993: NEUE Reihenfolge der Bloecke
 * Sat Aug 21 19:43:30 1993: 4 refresh phases now
 * Fri Aug 27 20:59:17 1993: inserted a void-block
 * Sat Sep 18 12:45:20 1993: added REFRESH_WAIT_COUNTER
 * Sat Sep 18 17:17:17 1993: added inner-refresh values
 * Sat Sep 18 17:29:20 1993: OUTER and INNER REFRESH_COUNTER
 * Sat Sep 18 17:31:41 1993: added INNER_PHASES
 * Mon Oct 25 13:53:46 1993: added translation for block4 and 5
 *
 * Revision 1.1  1993/07/29  17:30:12  prix
 * Initial revision
 *
 *
 *-@Header------------------------------------------------------------*/
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
#define NO_WAYPOINT  	-1

/* Extensions for Map and Elevator - data */
#define FILENAME_LEN		128
#define SHIP_EXT	".shp"
#define ELEVEXT	".elv"
#define CREWEXT	".crw"


/* string - signs in ship-data files */
#define MAP_BEGIN_STRING	"map"
#define WP_BEGIN_STRING		"wp"
#define LEVEL_END_STRING	"end"


typedef struct {
	char ascii;			/* the map-symbols in ascii notation */
	int intern;			/* the map-symbols in internal notation */
} symtrans;

#ifdef _map_c

symtrans Translator[BLOCKANZAHL] = {
	{'.',FLOOR},
	{'\'',VOID},
	{'x',FLOOR},	/* A waypoint is invisible */
	{'À',ECK_LU},
	{'Á',T_U},
	{'Ù',ECK_RU},
	{'Ã',T_L},
	{'Å',KREUZ},
	{'´',T_R},
	{'Ú',ECK_LO},
	{'Â',T_O},
	{'¿',ECK_RO},
	{'Ä',H_WALL},
	{'³',V_WALL},
	{'"',H_ZUTUERE},
	{'=',V_ZUTUERE},
	{'[',KONSOLE_L},
	{']',KONSOLE_R},
	{'(',KONSOLE_O},
	{')',KONSOLE_U},
	{'o',LIFT},
	{'@',REFRESH1},
	{'a',ALERT},
	{'1',BLOCK1},
	{'2',BLOCK2},
	{'3',BLOCK3},
	{'4',BLOCK4},
	{'5',BLOCK5}
};
#endif /* defined _map_c */

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
