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
#ifndef _takeover_h
#define _takeover_h

#ifndef _defs_h
#include "defs.h"    // we need this here for path defines
#endif

void
to_show_banner (const char* left, const char* right);

/* Background-color of takeover-game */
#define TO_BG_COLOR			63

/* File containing the Takeover-blocks */
#define TO_BLOCK_FILE		"to_elem.png"
#define TO_BG_FILE		"to_background.jpg"
/* --------------- individual block dimensions --------------- */
#define NUM_PHASES		5       /* number of color-phases for current "flow" */
					/* inclusive "inactive" phase */

/* Dimensions of the game-blocks */
#define TO_BLOCKS		11	/* anzahl versch. Game- blocks */

#define NUM_TO_BLOCKS		2*NUM_PHASES*TO_BLOCKS // total number of takover blocks 
#define TO_ELEMENTS		6
#define TO_BLOCKLEN		2*32	/* dimension of a Game- block */
#define TO_BLOCKHEIGHT	 	2*8

/* Dimensions of the fill-blocks (in led-column */
#define NUM_FILL_BLOCKS		3   // yellow, violett and black
#define FILL_BLOCK_LEN		2*16
#define FILL_BLOCK_HEIGHT  	2*7

/* Dimensions of a capsule */
#define NUM_CAPS_BLOCKS		3  // yellow, violett and red (?what for)
#define CAPSULE_LEN		2*7
#define CAPSULE_HEIGHT		2*7

/* Dimensions of ground-, column- und leader blocks */
#define NUM_GROUND_BLOCKS	6
#define GROUNDBLOCKLEN		2*23
#define GROUNDBLOCKHEIGHT	2*8

#define COLUMNBLOCKLEN		2*30
#define COLUMNBLOCKHEIGHT	2*8

#define LEADERBLOCKLEN		2*30
#define LEADERBLOCKHEIGHT	2*19

/* --------------- Timing parameters --------------- */
#define COLOR_COUNTDOWN		100	/* Zeit zum Farbe auswaehlen */
#define GAME_COUNTDOWN		100	/* Zeit fuer das Spiel */
#define CAPSULE_COUNTDOWN	 40	/* 1/10 sec. Lebensdauer einer Kapsel */

#define WAIT_MOVEMENT		0	/* 1/18 sekunden Bewegungsgeschw. */
#define WAIT_COLOR_ROTATION	2	/* 1/18 sekunden aktive-Kabel */
#define WAIT_AFTER_GAME		2*18	/* Wait after a deadlock */

#define TO_TICK_LENGTH		40     /* Time in ms between ticks */


/* --------------- Playground layout --------------- */

/* Position of Leader-Led */
#define LEADERLED_X		2*136
#define LEADERLED_Y		2*11

/* Start-pos of Led-column */
#define LEDCOLUMN_X		2*136
#define LEDCOLUMN_Y		2*27

/* Positions of Druid-pictures */
#define GELB_DRUID_X		2*40
#define GELB_DRUID_Y		0
#define VIOLETT_DRUID_X		2*220
#define VIOLETT_DRUID_Y		GELB_DRUID_Y

/* Left-Capsules positions */
#define GELB_LEFT_CAPSULES_X	4
#define GELB_LEFT_CAPSULES_Y	2*27 
#define VIOLETT_LEFT_CAPSULES_X	RIGHT_OFFS_X + COLUMNBLOCKLEN -10
#define VIOLETT_LEFT_CAPSULES_Y	2*27

/* start-pos of Current Capsule */
#define GELB_CUR_CAPSULE_X	2*26
#define GELB_CUR_CAPSULE_Y	2*19
#define VIOLETT_CUR_CAPSULE_X	2*255
#define VIOLETT_CUR_CAPSULE_Y	2*19

#define MAX_CAPSULES		13	/* a 999 has 13 !!! */

/* Start-Pos of playground */
#define GELB_PLAYGROUND_X	2*33
#define GELB_PLAYGROUND_Y	2*26
#define VIOLETT_PLAYGROUND_X	2*159
#define VIOLETT_PLAYGROUND_Y	2*26 

#define LEFT_OFFS_X		2*10	/* Offset der linken "Saeule" */
#define LEFT_OFFS_Y		2*15

#define MID_OFFS_X		2*129	/* -- mittleren -- */
#define MID_OFFS_Y		2*8

#define RIGHT_OFFS_X		2*255	/* -- rechten --- */
#define RIGHT_OFFS_Y		2*15



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
enum to_block_tyes
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


/* there are two classes of blocks: connectors and non-connectors */
#define CONNECTOR		0
#define NON_CONNECTOR  		1


#define NUM_LAYERS		4	/* dimension of the playground */
#define NUM_LINES		12

/* the playground type */
typedef int playground_t[TO_COLORS][NUM_LAYERS][NUM_LINES];


#endif
