/* 
 *
 *   Copyright (c) 2002 Johannes Prix
 *   Copyright (c) 2002 Reinhard Prix
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
 * Desc:  The Takeover-game of Paradroid
 *
 *----------------------------------------------------------------------*/

#ifndef _takeover_h
#define _takeover_h

/* Background-color of takeover-game */
#define TO_BG_COLOR			63

/* Timing parameters */
#define COLOR_COUNTDOWN		100		/* Zeit zum Farbe auswaehlen */
#define GAME_COUNTDOWN		100		/* Zeit fuer das Spiel */
#define CAPSULE_COUNTDOWN	 40		/* 1/10 sec. Lebensdauer einer Kapsel */

#define WAIT_MOVEMENT			0			/* 1/18 sekunden Bewegungsgeschw. */
#define WAIT_COLOR_ROTATION	2			/* 1/18 sekunden aktive-Kabel */
#define WAIT_AFTER_GAME			2*18		/* Wait after a deadlock */

/* Position of Leader-Led */
#define LEADERLEDX		151
#define LEADERLEDY		83

/* Start-pos of Led-column */
#define LEDCOLUMNX		151		/* the startpos of the LED-column */
#define LEDCOLUMNY		99

/* Dimensions of a capsule */
#define CAPSULE_LEN		7
#define CAPSULE_HEIGHT	7
#define CAPSULE_MEM		CAPSULE_HEIGHT*CAPSULE_LEN

/* Positions of Druid-pictures */
#define GELB_DRUID_X			45 + USERFENSTERPOSX
#define GELB_DRUID_Y			0 + USERFENSTERPOSY
#define VIOLETT_DRUID_X		225+ USERFENSTERPOSX
#define VIOLETT_DRUID_Y		GELB_DRUID_Y

/* Left-Capsules positions */
#define GELB_LEFT_CAPSULES_X		0 + USERFENSTERPOSX
#define GELB_LEFT_CAPSULES_Y		27 + USERFENSTERPOSY
#define VIOLETT_LEFT_CAPSULES_X	288 + USERFENSTERPOSX
#define VIOLETT_LEFT_CAPSULES_Y	27	+ USERFENSTERPOSY

/* start-pos of Current Capsule */
#define GELB_CUR_CAPSULE_X			31 + USERFENSTERPOSX
#define GELB_CUR_CAPSULE_Y			19 + USERFENSTERPOSY
#define VIOLETT_CUR_CAPSULE_X		260+ USERFENSTERPOSX
#define VIOLETT_CUR_CAPSULE_Y		19 + USERFENSTERPOSY

#define MAX_CAPSULES			13		/* a 999 has 13 !!! */

/* Start-Pos of playground */
#define GELB_PLAYGROUND_X			38 + USERFENSTERPOSX
#define GELB_PLAYGROUND_Y			26 + USERFENSTERPOSY
#define VIOLETT_PLAYGROUND_X		260 + USERFENSTERPOSX
#define VIOLETT_PLAYGROUND_Y		26 + USERFENSTERPOSY

/* Dimensions of the fill-blocks (in led-column */
#define FILLBLOCKLEN		16		
#define FILLBLOCKHEIGHT  7
#define FILLBLOCKMEM		FILLBLOCKLEN*FILLBLOCKHEIGHT

/* Dimensions of ground-, column- und leader blocks */
#define GROUNDBLOCKLEN		23
#define GROUNDBLOCKHEIGHT	8
#define GROUNDBLOCKMEM		GROUNDBLOCKHEIGHT*GROUNDBLOCKLEN

#define COLUMNBLOCKLEN		30
#define COLUMNBLOCKHEIGHT	8

#define LEADERBLOCKLEN		30
#define LEADERBLOCKHEIGHT	19

#define LEFT_OFFS_X		15		/* Offset der linken "Saeule" */
#define LEFT_OFFS_Y		15

#define MID_OFFS_X		134	/* -- mittleren -- */
#define MID_OFFS_Y		8

#define RIGHT_OFFS_X		260	/* -- rechten --- */
#define RIGHT_OFFS_Y		15

enum _groundblocks {
	GELB_OBEN,
	GELB_MITTE,
	GELB_UNTEN,
	VIOLETT_OBEN,
	VIOLETT_MITTE,
	VIOLETT_UNTEN
};

/* Dimensions of the game-blocks */
#define TO_BLOCKS			11			/* anzahl versch. Game- blocks */
#define TO_ELEMENTS		 6
#define TO_BLOCKLEN		32			/* dimension of a Game- block */
#define TO_BLOCKHEIGHT	 8
#define TO_BLOCKMEM 		TO_BLOCKLEN*TO_BLOCKHEIGHT

/* the flag of an active Elements (has to be added to Element) */
#define ACTIVE_OFFSET		2*TO_BLOCKS

/* Konditions in Connection-layer */
enum condition {
	INAKTIV = 0,
	AKTIV
};

/* Names for you and "him" */
enum to_opponents {
	YOU,
	ENEMY
};

/* Color-names */
#define TO_COLORS		2
enum to_colors {
	GELB = 0,
	VIOLETT,
	REMIS
};

/* Element - Names */
enum to_elements {
	EL_KABEL,
	EL_KABELENDE,
	EL_VERSTAERKER,
	EL_FARBTAUSCHER,
	EL_VERZWEIGUNG,
	EL_GATTER
};

/* Block-Names */
enum to_blocks {
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
#define CONNECTOR			0
#define NON_CONNECTOR  	1


#define NUM_LAYERS		4			/* dimension of the playground */
#define NUM_LINES			12

/* the playground type */
typedef int playground_t[TO_COLORS][NUM_LAYERS][NUM_LINES]; 


#endif
