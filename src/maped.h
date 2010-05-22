/*
 * Map - editor related defs' struct's etc.
 * Taken from the separate old DOS maped code.
 *
 */

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
#ifndef _maped_h
#define _maped_h

// #include "..\\defs.h"
// #include "..\\struct.h"
// #include "..\\map.h"

#ifndef TRUE
#define TRUE (1==1)
#endif

#ifndef FALSE
#define FALSE (1==0)
#endif


#define OK	0
#define ERR -1



/* Datei Defines */
#define LEVELNAME_LEN			8
#define SHIPNAME_LEN				8

#define MAX_COLORLEN			20

#define EXT		0xff

/* Cursorbewegungen */
#define CUR_UP				EXT + 72
#define CUR_DOWN			EXT + 80
#define CUR_RIGHT			EXT + 77
#define CUR_LEFT			EXT + 75

#define DELETE				EXT + 83
#define BACKSPACE			8

#define PAGE_UP			EXT + 73
#define PAGE_DOWN			EXT + 81

#define HOME				EXT + 71
#define END					EXT + 79

#define CTRL_UP			EXT + 141
#define CTRL_DOWN			EXT + 145
#define CTRL_RIGHT		EXT + 116
#define CTRL_LEFT			EXT + 115

#define RETURN				13

#define ESC		27 /* Keycode */
#define SPACE	32


/* COMMANDS */
#define NIX							0

#define NEXT_LEVEL				EXT + 132		/* Ctrl - PgUp */
#define PREV_LEVEL				EXT + 118		/* Ctrl - PgDown */

#define LOAD_SHIP					EXT + 59		/* f1 */
#define SAVE_SHIP					EXT + 61		/* F3 */

#define ADD_LEVEL					EXT + 60		/* f2 */

#define CENTER_WP					RETURN
#define TOGGLE_CONNECT_WP		SPACE

#define CHANGE_NAME				EXT + 63		/* f5 */
#define CHANGE_COLOR				EXT + 64		/* f6 */
#define CHANGE_LEVELNUM			EXT + 65		/* f7 */

#define EXIT						ESC



/* Sprunggroesse bei Ctrl-cursor: */
#define JUMP		5


/* Parameter zum Aussehen */

/* Bildschirm- Groessen */
#define SCREEN_WIDTH		80
#define SCREEN_HEIGHT	43


#define MENULINE_X	1
#define MENULINE_Y	49

#define MENULINE_STRING	"F1: Load Ship F2: Add Level F3: Save Ship F5: Name F6: Color F7: Levelnum"

#define W_EDITSTART_X	1
#define W_EDITSTART_Y	4
#define W_EDIT_LEN		80
#define W_EDIT_HEIGHT	41


#define W_DIALOGSTART_X	  W_EDITSTART_X +5
#define W_DIALOGSTART_Y	  W_EDITSTART_Y + W_EDIT_HEIGHT
#define W_DIALOG_LEN 	W_EDIT_LEN -10
#define W_DIALOG_LINES		1


#define W_HEADERSTART_X		1
#define W_HEADERSTART_Y		1
#define W_HEADER_LEN			W_EDIT_LEN
#define W_HEADER_HEIGHT		3
#define W_INTERS1_X			35
#define W_INTERS2_X			57

/* Box-Characters */
#define LO_BOXCHAR		"É"
#define RO_BOXCHAR		"»"
#define LU_BOXCHAR		"È"
#define RU_BOXCHAR		"¼"
#define H_BOXCHAR			"Í"
#define V_BOXCHAR			"º"
#define TL_BOXCHAR		"Ì"
#define TR_BOXCHAR		"¹"
#define TO_BOXCHAR		"Ë"
#define TU_BOXCHAR		"Ê"

#define FLOOR_BOXCHAR	'.'
#define VOID_BOXCHAR		'\''	/* This HAS to be a char !!! for DrawBox() */

/* Colors */
#define BOXCOLOR		RED
#define TEXTCOLOR		GREEN
#define MENUCOLOR		LIGHTBLUE
#define MAPCOLOR		GREEN
#define DIALOGCOLOR	LIGHTGREEN

#define CENTER_WAYPOINT_COLOR		LIGHTGREEN
#define WP_CONN_COLOR				LIGHTBLUE

enum {
	NOERR,
	FILE_NOT_FOUND,
	FILE_ERROR,
	CLOSE_FILE_ERROR,
	NO_FILENAME,
	TOO_MUCH_DOORS,
	TOO_MUCH_WP,
	WRONG_MAP_FORMAT,
	INCONSISTENT_MAP,
	ILLEGAL_WP_DATA,
	ILLEGAL_MAP_CHAR,
	CRITICAL_ERROR,
	NO_MEMORY,
	NO_LEVEL_TO_EDIT,
	MISSING_LEVELNUMBER,
	IDENTICAL_LEVELNUMBERS

};

#ifdef _maped_c

char *ErrList[] =
{
	"Ok.",
	"File not found: %s",
	"General file error on file: %s",
	"Couldn't close file %s",
	"No filename.",
	"Too much doors on this level",
	"Too much waypoints on this level",
	"Wrong Map-format: missing '%s' - marker",
	"Inconsistent map data",
	"Illegal Waypoint Data",
	"Illegal Map char: '%s' ",
	"Sorry: critical error, I would recommend to reboot... :-) ",
	"Cant get more memory, I'm so sorry ...",
	"You haven't got any level to edit, sorry !",
	"Warning: The levelnumber %s is missing !",
	"Error: There are identical levelnumbers. Can't save.. ",
	NULL
};


struct text_info DialogTextInfo = {
	W_DIALOGSTART_X, W_DIALOGSTART_Y,
	W_DIALOGSTART_X+W_DIALOG_LEN-2, W_DIALOGSTART_Y+W_DIALOG_LINES,
	7,7,C4350,0,0,0,0
};

struct text_info EditTextInfo = {
	W_EDITSTART_X+1, W_EDITSTART_Y+1,
	W_EDITSTART_X+W_EDIT_LEN-2, W_EDITSTART_Y+W_EDIT_HEIGHT-2,
	7,7,C4350, 0, 0, 0, 0
};

struct text_info ShipnameTextInfo = {
	W_HEADERSTART_X+1, W_HEADERSTART_Y+1,
	W_INTERS1_X-1, W_HEADERSTART_Y+W_HEADER_HEIGHT-2,
	7,7,C4350, 0, 0, 0, 0
};

struct text_info ColorTextInfo = {
	W_INTERS1_X+1, W_HEADERSTART_Y+1,
	W_INTERS2_X-1, W_HEADERSTART_Y+W_HEADER_HEIGHT-2,
	7, 7, C4350, 0, 0, 0, 0
};

struct text_info LevelTextInfo = {
	W_INTERS2_X+1, W_HEADERSTART_Y+1,
	W_HEADERSTART_X+W_HEADER_LEN-2, W_HEADERSTART_Y+W_HEADER_HEIGHT-2,
	7,7, C4350, 0,0, 0, 0
};


ship curShip;			/* the current ship to edit */

Level CurLevel=NULL;		 /* Pointer to current Level */


#else
extern char *ErrList[];
extern struct text_info DialogTextInfo, EditTextInfo, ShipnameTextInfo;
extern struct text_info ColorTextInfo, LevelTextInfo;
extern ship curShip;
extern Level CurLevel;
#endif



/* Some macros */
#define Say(string) { SayToWindow(&DialogTextInfo, string); }

#define SayShipname(fname) { 	char msg[81];	strcpy(msg, "   Shipname: "); strcat(msg, fname); SayToWindow(&ShipnameTextInfo, msg); }

#define SayColor(_color) { char msg[81]; strcpy(msg, "     Color: "); strcat(msg, ColorNames[_color]); SayToWindow(&ColorTextInfo, msg); }

#define SayLevel(lev) { char msg[81]; char buf[10]; strcpy(msg, "    Level: ");	sprintf(buf, "%d", lev); strcat(msg,buf); SayToWindow(&LevelTextInfo, msg); }


#endif

