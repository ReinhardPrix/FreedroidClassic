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

/*
 * This file has been checked for remaining german comments. However, if you still
 * find some, please either translate them to english or tell me where you found them.
 * Thanks a lot, Johannes Prix.
 */

// This stuff is nescessary, so that global.h and proto.h know that they have been
// included this time from the file map.h
#ifndef _map_h
#define _map_h

/* some defines */

#define OUTER_REFRESH_COUNTER 2
#define INNER_REFRESH_COUNTER 4
#define INNER_PHASES		4

#define MAX_TYPES_ON_LEVEL		30
#define CREW_LINE_LEN			MAX_TYPES_ON_LEVEL * 3 + 20

//--------------------
// Distance, where door opens 
//
#define DOOROPENDIST2 1

//--------------------
// How big is the margin the walls have as far as collisions are concerned.
//
#define WALLPASS (4.0/64.0)

//--------------------
// How big is the margin in console tiles
//
#define CONSOLEPASS_X 	((INITIAL_BLOCK_WIDTH/2 + 4)/64.0)
#define CONSOLEPASS_Y 	((INITIAL_BLOCK_HEIGHT/2 + 4)/64.0)

//--------------------
// How big is the margin of doors and the passable area in front of doors
//
#define TUERBREITE	(6/64.0)
#define V_RANDSPACE		WALLPASS
#define V_RANDBREITE		(5/64.0)
#define H_RANDSPACE		WALLPASS
#define H_RANDBREITE		(5/64.0)

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


#endif
