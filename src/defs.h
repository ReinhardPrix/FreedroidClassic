/*----------------------------------------------------------------------
 *
 * Desc: all the define-constants and macros
 *
 *----------------------------------------------------------------------*/

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


#ifndef _defs_h
#define _defs_h

#ifndef _colodefs_h
#include "colodefs.h"
#endif


#define Set_Rect(rect, xx, yy, ww, hh) {\
(rect).x = (xx); (rect).y = (yy); (rect).w = (ww); (rect).h = (hh); }

// **********************************************************************
// Constants influencing code generation and
// constants defining flags for some functions:

#define USE_SDL_FRAMERATE
#define NEW_ENGINE

/* ************************************************************
 * Highscore related defines 
 *************************************************************/
#define MAX_NAME_LEN 	20     /* max len of highscore name entry */
#define MAX_HIGHSCORES  10     /* only keep Top10 */
#define DATE_LEN	20     /* reserved for the date-string */

// The flags for DisplayBanner are:
enum
  { BANNER_FORCE_UPDATE=1 , BANNER_DONT_TOUCH_TEXT=2 , BANNER_NO_SDL_UPDATE=4 };

// The flags for AssembleCombatWindow are:
enum
{ ONLY_SHOW_MAP = 1 , DO_SCREEN_UPDATE = 2 };


// **********************************************************************
// Constants for Paths and names of Data-files
// the root "DATADIR" should be defined in the Makefile as $(pkgdatadir)
// if not, we set it here:
#ifndef DATADIR
#define DATADIR ".."   // our local fallback
#endif

#define GRAPHICS_DIR		DATADIR "/graphics/"
#define SOUND_DIR		DATADIR "/sound/"
#define MAP_DIR			DATADIR "/map/"

#define NE_MAP_BLOCK_FILE	GRAPHICS_DIR "ne_block_red.gif"
#define NE_DROID_BLOCK_FILE	GRAPHICS_DIR "ne_droids.gif"
#define NE_BULLET_BLOCK_FILE 	GRAPHICS_DIR "ne_bullet.gif"
#define NE_BLAST_BLOCK_FILE 	GRAPHICS_DIR "ne_blast.gif"
#define NE_ELEVATOR_PIC_FILE    GRAPHICS_DIR "ne_ship.gif"
#define NE_CONSOLEN_PIC_FILE    GRAPHICS_DIR "ne_cons.gif" 
#define NE_DIGIT_BLOCK_FILE     GRAPHICS_DIR "ne_digits.gif"
#define NE_BANNER_BLOCK_FILE    GRAPHICS_DIR "ne_rahmen.gif"
#define NE_TITLE_PIC_FILE       GRAPHICS_DIR "ne_title.jpg"

#define SHIPNAME		MAP_DIR "ship1"
#define COLORFILE		MAP_DIR "levels.col"

#define PARA_FONT_FILE 		GRAPHICS_DIR "parafont.png"
// #define MENU_FONT_FILE 		GRAPHICS_DIR "font05.png"
// #define MENU_FONT_FILE		GRAPHICS_DIR "NeonFont.png"
// #define MENU_FONT_FILE 		GRAPHICS_DIR "parafont.png"
#define MENU_FONT_FILE 		GRAPHICS_DIR "ArialGold.png"
// #define FPS_FONT_FILE		GRAPHICS_DIR "font05.png"
#define FPS_FONT_FILE 		GRAPHICS_DIR "parafont.png"
#define ICON_FILE		GRAPHICS_DIR "paraicon.bmp"


// **********************************************************************
// Constants about the size (and mem?) of pixmaps

#define INITIAL_BLOCK_WIDTH	64
#define INITIAL_BLOCK_HEIGHT	64
#define BLOCKMEM  		INITIAL_BLOCK_WIDTH*INITIAL_BLOCK_HEIGHT

#define INITIAL_DIGIT_LENGTH (8*2)
#define INITIAL_DIGIT_HEIGHT (9*2)
#define INITIAL_DIGIT_POS_X 5*2
#define INITIAL_DIGIT_POS_Y 12*2
#define DIGITNUMBER 22
#define DIGITMEM DIGITHEIGHT*DIGITLENGTH*DIGITNUMBER

#define SCREENBREITE 		(320*2)
#define SCREENHOEHE 		(200*2)
#define SCREENLEN		(320*2)
#define SCREENHEIGHT		(200*2)

#define USERFENSTERHOEHE 	4*INITIAL_BLOCK_HEIGHT
#define USERFENSTERBREITE 	9*INITIAL_BLOCK_WIDTH
#define USERFENSTERPOSX 	( (SCREENBREITE-USERFENSTERBREITE) / 2)
#define USERFENSTERPOSY 	( (SCREENHOEHE-USERFENSTERHOEHE) )
#define USER_FENSTER_CENTER_X (USERFENSTERPOSX + (USERFENSTERBREITE/2))
#define USER_FENSTER_CENTER_Y (USERFENSTERPOSY + (USERFENSTERHOEHE/2))

#define DRUIDIMAGE_LENGTH       66
#define DRUIDIMAGE_HEIGHT       90

#define SCALE_FACTOR  2 /* This constant (please let it be an int!) defines how many times the
			   actually displayed screen shall be bigger than the 320x200 window. */

/* Startpos + dimesnions of Banner-Texts */
#define LEFT_INFO_X 	(12*2)
#define LEFT_INFO_Y	(8*2)
#define RIGHT_INFO_X	(242*2)
#define RIGHT_INFO_Y	(8*2)
#define LEFT_TEXT_LEN 10
#define RIGHT_TEXT_LEN 6
#define TEXT_STRETCH 1.2
#define BANNER_WIDTH		SCREENBREITE
#define BANNER_HEIGHT		INITIAL_BLOCK_HEIGHT



/* Menu in Konsole */
#define MENUITEMHEIGHT 		77
#define MENUITEMLENGTH 		50*2
#define MENUITEMMEM 		(MENUITEMLENGTH * MENUITEMHEIGHT)

/* Dimensionen der Druids (fuer NotPassable() and CheckEnemyCollsion() ) */
//NORMALISATION #define DRUIDRADIUSX		((10*2)/64.0)
#define DRUIDRADIUSX		(20/64.0)
//NORMALISATION #define DRUIDRADIUSY		((10*2)/64.0)
#define DRUIDRADIUSY		(20/64.0)
// #define DRUIDRADIUSXY	        ((7*2)/64.0)
#define DRUIDRADIUSXY	        (7*2/64.0)
#define BULLET_BULLET_COLLISION_DIST (10/64.0)

/* Dimension eines Blasts */
#define BLASTRADIUS		( ( INITIAL_BLOCK_WIDTH/64.0 ) * 0.4 )

// **********************************************************************
//
//

#define SINGLE_PLAYER_MENU_POINTER_POS_X (INITIAL_BLOCK_WIDTH/2)
enum
  { NEW_GAME_POSITION=1, SHOW_HISCORE_POSITION=2,
    SHOW_MISSION_POSITION=3, BACK_POSITION=4 };



// The following is the definition of the sound file names used in freedroid
// DO NOT EVER CHANGE THE ORDER OF APPEARENCE IN THIS LIST PLEASE!!!!!
// The order of appearance here should match the order of appearance 
// in the SoundSampleFilenames definition located in sound.c!

#define SILENCE (-1)
enum _sounds
{
  ERRORSOUND = 0,
  COMBAT_BACKGROUND_MUSIC_SOUND,
  TAKEOVER_BACKGROUND_MUSIC_SOUND,
  CONSOLE_BACKGROUND_MUSIC_SOUND,
  CLASSICAL_BEEP_BEEP_BACKGROUND_MUSIC,
  BLASTSOUND,
  COLLISIONSOUND,
  GOT_INTO_BLAST_SOUND,
  MOVE_ELEVATOR_SOUND,
  REFRESH_SOUND,
  LEAVE_ELEVATOR_SOUND,
  ENTER_ELEVATOR_SOUND,
  THOU_ART_DEFEATED_SOUND,
  GOT_HIT_SOUND,
  TAKEOVER_SET_CAPSULE_SOUND,
  MENU_ITEM_SELECTED_SOUND,
  MOVE_MENU_POSITION_SOUND,
  TAKEOVER_GAME_WON_SOUND,
  TAKEOVER_GAME_DEADLOCK_SOUND,
  TAKEOVER_GAME_LOST_SOUND,
  FIRE_BULLET_PULSE_SOUND,
  FIRE_BULLET_SINGLE_PULSE_SOUND,
  FIRE_BULLET_MILITARY_SOUND,
  FIRE_BULLET_FLASH_SOUND
};

// **********************************************************************

#define TRUE (1==1)
#define FALSE (1==0)

#define ERR 	-1
#define OK		0

/* Ship-Elevator Picture */
#define NUM_EL_BLOCKS		17

#define EL_BLOCK_LEN		8
#define EL_BLOCK_HEIGHT		8
#define EL_BLOCK_MEM 		EL_BLOCK_LEN * EL_BLOCK_HEIGHT

#define ELEVATOR_LEN		38
#define ELEVATOR_HEIGHT		12


#define BULLETSPEEDINFLUENCE 	2

#define DIRECTIONS 		8

#define ALLCLASSNAMES 		11
#define ALLSHIPS 		4

#define ENEMYPHASES 		8
#define DROID_PHASES            ENEMYPHASES

#define WAIT_LEVELEMPTY		18	/* warte bevor Graufaerben */
#define WAIT_AFTER_KILLED	1  // time to wait and still display pictures after the destruction of 
                                   // the players droid.  This is now measured in seconds and can be a float
#define WAIT_SHIPEMPTY		20
#define WAIT_TRANSFERMODE	0.3	/* this is a "float" indicating the number of seconds the influence 
					   stand still with space pressed, before switching into transfermode 
					   This variable describes the amount in SECONDS */
#define WAIT_COLLISION		1 // after a little collision with influ, enemys hold position for a while
                                  // this variable describes the amount of time in SECONDS 
#define ENEMYMAXWAIT 2		  // after each robot has reached its current destination waypoint is waits a
                                  // while.  This variable describes the amount of time in SECONDS.  However,
                                  // the final wait time is a random number within [0,ENEMYMAXWAIT].
#define FLASH_DURATION_IN_FRAMES 10

/* direction definitions (fireing bullets and testing blockedness of positions) */
enum _directions
{
  OBEN = 0,
  RECHTSOBEN,
  RECHTS,
  RECHTSUNTEN,
  UNTEN,
  LINKSUNTEN,
  LINKS,
  LINKSOBEN,
  CENTER,
  LIGHT				/* special: checking passability for light, not for a checkpos */
};

/* Koordinaten der Bloecke die isoliert werden sollen */
#define INFLUENCEBLOCKPOSITIONX 0
#define INFLUENCEBLOCKPOSITIONY 0	/* INITIAL_BLOCK_HEIGHT */
#define BULLETBLOCKPOSITIONY 0	/* (INITIAL_BLOCK_HEIGHT*4) */
#define BLASTBLOCKPOSITIONY 0	/* (INITIAL_BLOCK_HEIGHT*3) */

// #define STARTENERGIE 100
#define ENERGIEBALKENBREITE 15
#define MAXENERGIEBALKENLAENGE 160
#define ENERGIEPOSY 180

/* Maximale Anzahl von ... */

#define BLOCKANZAHL 		43
#define NUM_MAP_BLOCKS		BLOCKANZAHL  /* total number of map-blocks */

#define ALLDRUIDTYPES		24	/* number of druid-models that exist */
#define ALLBULLETTYPES		4	/* number of bullet-types */
#define ALLBLASTTYPES		2	/* number of different exposions */

#define MAXBULLETS		100	/* maximum possible Bullets in the air */
#define MAXBLASTS		100	/* max. possible Blasts visible */
#define AGGRESSIONMAX  		100

/* Map-related defines:
	WARNING leave them here, they are required in struct.h
*/
#define MAX_WP_CONNECTIONS 	4
#define MAXMAPLINES 		50
#define MAX_ENEMYS_ON_SHIP	1000

#define MAX_LIFTS	 	50  	/* actually the entries to the lifts */
#define MAX_LEVELS		29   	/* don't change this easily */
				     	/* corresponds to a reserved palette range ! */
#define MAX_LIFTROWS		15   	/* the different lift "rows" */
					/* don't change this easily */
					/* corresponds to a reserved palette range !*/

#define MAXWAYPOINTS 		30
#define MAX_DOORS_ON_LEVEL 	20
#define MAX_REFRESHES_ON_LEVEL	10

/* Macros */
#define GrobX (Me.pos.x / INITIAL_BLOCK_WIDTH)
#define GrobY (Me.pos.y / INITIAL_BLOCK_HEIGHT)
#define FeinX (Me.pos.x%INITIAL_BLOCK_WIDTH)
#define FeinY (Me.pos.y%INITIAL_BLOCK_HEIGHT)

#define CLFeinY ((Me.pos.y+INITIAL_BLOCK_HEIGHT/2) % INITIAL_BLOCK_HEIGHT)
#define CLGrobY ((Me.pos.y+INITIAL_BLOCK_HEIGHT/2) / INITIAL_BLOCK_HEIGHT)
#define CLFeinX ((Me.pos.x+INITIAL_BLOCK_WIDTH/2) % INITIAL_BLOCK_HEIGHT)
#define CLGrobX ((Me.pos.x+INITIAL_BLOCK_WIDTH/2) / INITIAL_BLOCK_HEIGHT)


/* 
 * these macros are a bit of a hack, as they will work correctly only
 * as long as INITIAL_BLOCK_WIDTH = INITIAL_BLOCK_HEIGHT
 * but the handling of the grob/fein coordinates should be changed anyway
 * in the longer term...
 */
#define Grob2Fein(grob) 	\
 ((grob)* INITIAL_BLOCK_WIDTH + INITIAL_BLOCK_WIDTH / 2)

#define Fein2Grob(fein)		\
  ((int)(fein) / INITIAL_BLOCK_WIDTH)

#define BREMSDREHUNG 3		/* warte 3*, bevor Influencer weitergedreht wird */

/* Wegstossgeschw. von Tueren u.ae. */
// NORMALISATION #define PUSHSPEED 2
#define PUSHSPEED 2

/* Schusstypen */
enum _bullets
{
  PULSE,
  SINGLE_PULSE,
  MILITARY,
  FLASH
};


/* Explosionstypen */
enum _explosions
{
  BULLETBLAST,
  DRUIDBLAST,
  OWNBLAST
};

#define BLINKENERGY 25

/* Druidtypen */
enum _druids
{
  DRUID001 = 0,			/* You will know why are the numbers there, when you */
  DRUID123 = 1,			/* enter the crew of a level !! */
  DRUID139 = 2,
  DRUID247 = 3,
  DRUID249 = 4,
  DRUID296 = 5,
  DRUID302 = 6,
  DRUID329 = 7,
  DRUID420 = 8,
  DRUID476 = 9,
  DRUID493 = 10,
  DRUID516 = 11,
  DRUID571 = 12,
  DRUID598 = 13,
  DRUID614 = 14,
  DRUID615 = 15,
  DRUID629 = 16,
  DRUID711 = 17,
  DRUID742 = 18,
  DRUID751 = 19,
  DRUID821 = 20,
  DRUID834 = 21,
  DRUID883 = 22,
  DRUID999 = 23,
  DEBUG_ENEMY = 24
};

/* Status- Werte der Druids */
enum _status
{
  MOBILE,
  TRANSFERMODE,
  WEAPON,
  CAPTURED,
  COMPLETE,
  REJECTED,
  CONSOLE,
  DEBRIEFING,
  TERMINATED,
  PAUSE,
  CHEESE,
  ELEVATOR,
  BRIEFING,
  MENU
};


#define BLASTDAMAGE		45
#define BLASTPHASES_PER_SECOND  15

#define DECKCOMPLETEBONUS 500

/* Tastaturgeschwindigkeiten */
#define TYPEMATIC_SLOW 127
#define TYPEMATIC_FAST 0
#define TYPEMATIC_TAKEOVER 92	/* For the takeover-game */

#define OUT TERMINATED		/* Type fuer inaktive Dinge */

/* Konstanten die die Kartenwerte anschaulich machen */
enum
{
  FLOOR = 0,
  ECK_LU,     T_U,          ECK_RU,       T_L,          KREUZ,       T_R,    ECK_LO,  T_O,    ECK_RO,
  H_WALL,     V_WALL,       ALERT,        BLOCK1,       BLOCK2,      BLOCK3, BLOCK4,  BLOCK5,
  H_ZUTUERE,  H_HALBTUERE1, H_HALBTUERE2, H_HALBTUERE3, H_GANZTUERE,
  KONSOLE_L,  KONSOLE_R,    KONSOLE_O,    KONSOLE_U,
  V_ZUTUERE,  V_HALBTUERE1, V_HALBTUERE2, V_HALBTUERE3, V_GANZTUERE,
  LIFT,       VOID,         REFRESH1,     REFRESH2,     REFRESH3,    REFRESH4,
  I_REFRESH1, I_REFRESH2,   I_REFRESH3,   I_REFRESH4,   INVISIBLE_BRICK
};


#endif
