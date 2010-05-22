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

#ifndef _map_h
#define _map_h

/* some defines */

#define OUTER_REFRESH_COUNTER 2
#define INNER_REFRESH_COUNTER 4
#define INNER_PHASES		4

#define MAX_TYPES_ON_LEVEL		30
#define CREW_LINE_LEN			MAX_TYPES_ON_LEVEL * 3 + 20

/* Distance, where door opens */
//NORMALISATION #define DOOROPENDIST2 	(Block_Rect.h + Block_Rect.w)*(Block_Rect.w + Block_Rect.h)/4
// #define DOOROPENDIST2 	(((Block_Rect.h + Block_Rect.w)*(Block_Rect.w + Block_Rect.h)/4)/64.0)
#define DOOROPENDIST2 1

/* Randbreite der Wand */
#define WALLPASS (4.0/64.0)

/* Randbreite der Konsolen */
#define KONSOLEPASS_X 	0.5625
#define KONSOLEPASS_Y 	0.5625

/* Breite der Tueren freien Raumes vor Tuer*/
#define TUERBREITE	(6/64.0)

/* Rand der offenen Tueren */
#define V_RANDSPACE		WALLPASS
#define V_RANDBREITE		(5/64.0)
#define H_RANDSPACE		WALLPASS
#define H_RANDBREITE		(5/64.0)

#define WAYPOINT_CHAR		'x'
#define NO_WAYPOINT  		-1

/* Extensions for Map and Elevator - data */
#define FILENAME_LEN	128
#define SHIP_EXT	".shp"
#define ELEVEXT		".elv"
#define CREWEXT		".crw"


/* string - signs in ship-data files */
#define WHITE_SPACE 		" \t"

#define MAP_BEGIN_STRING	"begin_map"
#define WP_BEGIN_STRING		"begin_waypoints"
#define LEVEL_END_STRING	"end_level"
#define CONNECTION_STRING	"connections: "


typedef struct
{
  char ascii;			/* the map-symbols in ascii notation */
  int intern;			/* the map-symbols in internal notation */
}
symtrans;


enum _colornames
{
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
  "Turquoise",
  "Dark",
  NULL
};
#else
extern char *ColorNames[];
#endif



#endif
