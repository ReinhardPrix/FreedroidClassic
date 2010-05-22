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
#ifndef _takeover_h
#define _takeover_h

/* Background-color of takeover-game */
#define TO_BG_COLOR			63

/* File containing the Takeover-blocks */
#define TO_BLOCK_FILE		"to_elem.png"

/* --------------- individual block dimensions --------------- */
#define NUM_PHASES		5       /* number of color-phases for current "flow" */
					/* inclusive "inactive" phase */

/* Dimensions of the game-blocks */
#define TO_BLOCKS		11	/* anzahl versch. Game- blocks */

#define NUM_TO_BLOCKS		2*NUM_PHASES*TO_BLOCKS // total number of takover blocks
#define TO_ELEMENTS		6

/* Dimensions of the fill-blocks (in led-column */
#define NUM_FILL_BLOCKS		3   // yellow, violett and black

/* Dimensions of a capsule */
#define NUM_CAPS_BLOCKS		3  // yellow, violett and red (?what for)

/* Dimensions of ground-, column- und leader blocks */
#define NUM_GROUND_BLOCKS	6

/* --------------- Timing parameters --------------- */
#define COLOR_COUNTDOWN		100	/* Zeit zum Farbe auswaehlen */
#define GAME_COUNTDOWN		100	/* Zeit fuer das Spiel */
#define CAPSULE_COUNTDOWN	 40	/* 1/10 sec. Lebensdauer einer Kapsel */

#define WAIT_MOVEMENT		0	/* 1/18 sekunden Bewegungsgeschw. */
#define WAIT_COLOR_ROTATION	2	/* 1/18 sekunden aktive-Kabel */
#define WAIT_AFTER_GAME		2*18	/* Wait after a deadlock */

#define TO_TICK_LENGTH		40     /* Time in ms between ticks */

/* --------------- Playground layout --------------- */

#define MAX_CAPSULES		13	/* a 999 has 13 !!! */


/* there are two classes of blocks: connectors and non-connectors */
#define CONNECTOR		0
#define NON_CONNECTOR  		1


#define NUM_LAYERS		4	/* dimension of the playground */
#define NUM_LINES		12


enum _groundblocks
{
  GELB_OBEN,
  GELB_MITTE,
  GELB_UNTEN,
  VIOLETT_OBEN,
  VIOLETT_MITTE,
  VIOLETT_UNTEN
};


/* Konditions in Connection-layer */
enum condition
{
  INACTIVE = 0,
  ACTIVE1,
  ACTIVE2,
  ACTIVE3,
  ACTIVE4
};

/* Names for you and "him" */
enum to_opponents
{
  YOU,
  ENEMY
};

/* Color-names */
#define TO_COLORS		2
enum to_colors
{
  GELB = 0,
  VIOLETT,
  REMIS
};

/* Element - Names */
enum to_elements
{
  EL_KABEL,
  EL_KABELENDE,
  EL_VERSTAERKER,
  EL_FARBTAUSCHER,
  EL_VERZWEIGUNG,
  EL_GATTER
};

/* Block-Names */
enum to_blocks
{
  KABEL,
  KABELENDE,
  VERSTAERKER,
  FARBTAUSCHER,
  VERZWEIGUNG_O,
  VERZWEIGUNG_M,
  VERZWEIGUNG_U,
  GATTER_O,
  GATTER_M,
  GATTER_U,
  LEER
};


/* the playground type */
typedef int playground_t[TO_COLORS][NUM_LAYERS][NUM_LINES];


#endif
