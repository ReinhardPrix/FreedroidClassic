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

/* ----------------------------------------------------------------------
 * This file contains all the define-constants and macros
 * ---------------------------------------------------------------------- */

/*
 * This file has been checked for remnants of german documentation.
 * If you still find some german documentation anywhere in here, please
 * let me know.  Thanks a lot.
 */
#ifndef _defs_h
#define _defs_h

#ifndef _colodefs_h
#include "colodefs.h"
#endif

#define JOY_MAX_VAL 32767     // maximal amplitude of joystick axis values

#define Set_Rect(rect, xx, yy, ww, hh) {\
(rect).x = (xx); (rect).y = (yy); (rect).w = (ww); (rect).h = (hh); }

#define Copy_Rect(src, dst) {\
(dst).x = (src).x; (dst).y = (src).y; (dst).w = (src).w; (dst).h = (src).h; }


//--------------------
// Constants influencing code generation and
// constants defining flags for some functions:
//
#define USE_SDL_FRAMERATE

//--------------------
// Highscore related defines 
// 
#define HS_BACKGROUND_FILE     "transfer.jpg"
#define HS_EMPTY_ENTRY  "--- empty ---"
#define MAX_NAME_LEN 	15     /* max len of highscore name entry */
#define MAX_HIGHSCORES  10     /* only keep Top10 */
#define DATE_LEN	10     /* reserved for the date-string */

// The flags for DisplayBanner are:
enum
  { BANNER_FORCE_UPDATE=1 , BANNER_DONT_TOUCH_TEXT=2 , BANNER_NO_SDL_UPDATE=4 };

// The flags for AssembleCombatWindow are:
enum
{ ONLY_SHOW_MAP = 1 , DO_SCREEN_UPDATE = 2 };


//--------------------
// Constants for Paths and names of Data-files
// the root "DATADIR" should be defined in the Makefile as $(pkgdatadir)
// if not, we set it here:
//
#ifndef DATADIR
#define DATADIR ".."   // our local fallback
#endif

#define GRAPHICS_DIR		"graphics/"
#define SOUND_DIR		"sound/"
#define MAP_DIR			"map/"

#define NE_DROID_BLOCK_FILE	"ne_droids.png"
#define NE_BULLET_BLOCK_FILE 	"ne_bullet.png"
#define NE_BLAST_BLOCK_FILE 	"ne_blast.png"
#define NE_DIGIT_BLOCK_FILE     "ne_digits.gif"
#define NE_ITEMS_BLOCK_FILE     "ne_items.png"

#define NE_CONSOLEN_PIC_FILE    "console_fg.png" 
#define NE_BANNER_BLOCK_FILE    "banner.png"
#define NE_TITLE_PIC_FILE       "title.jpg"
#define NE_CONSOLE_BG_PIC1_FILE  "console_bg1.jpg"
#define NE_CONSOLE_BG_PIC2_FILE  "console_bg2.jpg"
#define NE_CREDITS_PIC_FILE     "credits.jpg"

#define STANDARD_MISSION        "Paradroid.mission"
#define NEW_MISSION             "CleanPrivateGoodsStorageCellar.mission"

#define PARA_FONT_FILE 		"parafont.png"
#define MENU_FONT_FILE 		"ArialGold.png"
#define FPS_FONT_FILE 		"font05.png"
#define ICON_FILE		"paraicon.bmp"


//--------------------
// Constants about the size (and mem?) of pixmaps
//

#define INITIAL_BLOCK_WIDTH	64
#define INITIAL_BLOCK_HEIGHT	64
#define BLOCKMEM  		INITIAL_BLOCK_WIDTH*INITIAL_BLOCK_HEIGHT

#define INITIAL_DIGIT_LENGTH (8*2)
#define INITIAL_DIGIT_HEIGHT (9*2)
#define INITIAL_DIGIT_POS_X 5*2
#define INITIAL_DIGIT_POS_Y 12*2
#define DIGITNUMBER (22+26)
#define DIGITMEM DIGITHEIGHT*DIGITLENGTH*DIGITNUMBER

#define SCALE_FACTOR  2 /* scale-factor between actually displayed screen and 320x220 */
#define SCREENLEN		(320*SCALE_FACTOR)
#define SCREENHEIGHT		(240*SCALE_FACTOR)

#define USERFENSTERHOEHE 	4*INITIAL_BLOCK_HEIGHT
#define USERFENSTERBREITE 	9*INITIAL_BLOCK_WIDTH
#define USERFENSTERPOSX 	( (SCREENLEN-USERFENSTERBREITE) / 2)
#define USERFENSTERPOSY 	( 2*(SCREENHEIGHT-USERFENSTERHOEHE)/3 )
#define USER_FENSTER_CENTER_X (USERFENSTERPOSX + (USERFENSTERBREITE/2))
#define USER_FENSTER_CENTER_Y (USERFENSTERPOSY + (USERFENSTERHOEHE/2))

#define DRUIDIMAGE_LENGTH       66
#define DRUIDIMAGE_HEIGHT       90



/* Startpos + dimesnions of Banner-Texts */
#define LEFT_INFO_X 	(13*2)
#define LEFT_INFO_Y	(10*2)
#define RIGHT_INFO_X	(242*2)
#define RIGHT_INFO_Y	(8*2)
#define LEFT_TEXT_LEN 10
#define RIGHT_TEXT_LEN 6
#define TEXT_STRETCH 1.2
#define BANNER_WIDTH		SCREENLEN
#define BANNER_HEIGHT		INITIAL_BLOCK_HEIGHT



//--------------------
// Definitions for the menu inside the in-game console
//
#define MENUITEMHEIGHT 		77
#define MENUITEMLENGTH 		50*2
#define MENUITEMMEM 		(MENUITEMLENGTH * MENUITEMHEIGHT)

//--------------------
// dimensions of the droids as required for functions NotPassable() and CheckEnemyCollsion() )
//

//NORMALISATION #define DRUIDRADIUSX		((10*2)/64.0)
// #define DRUIDRADIUSX		(18/64.0)
//NORMALISATION #define DRUIDRADIUSY		((10*2)/64.0)
// #define DRUIDRADIUSY		(18/64.0)
// #define DRUIDRADIUSXY	        ((7*2)/64.0)
#define DRUIDRADIUSXY	        (7*2/64.0)
#define BULLET_BULLET_COLLISION_DIST (10/64.0)
// #define BULLET_BULLET_COLLISION_DIST (8.0/64.0)
// #define BULLET_BULLET_COLLISION_DIST (4.0/64.0)

//--------------------
// Some definitions for the menus
//

#define SINGLE_PLAYER_MENU_POINTER_POS_X (INITIAL_BLOCK_WIDTH/2)
enum
  { NEW_GAME_POSITION=1, SHOW_HISCORE_POSITION=2,
    SHOW_MISSION_POSITION=3, BACK_POSITION=4 };

//--------------------
// The following is the definition of the sound file names used in freedroid
// DO NOT EVER CHANGE THE ORDER OF APPEARENCE IN THIS LIST PLEASE!!!!!
// The order of appearance here should match the order of appearance 
// in the SoundSampleFilenames definition located in sound.c!
//

#define COMBAT_BACKGROUND_MUSIC_SOUND "The_Last_V8.mod"
// #define TAKEOVER_BACKGROUND_MUSIC_SOUND "The_Last_V8.mod"
#define TAKEOVER_BACKGROUND_MUSIC_SOUND "Intro-Music.mod"
#define CONSOLE_BACKGROUND_MUSIC_SOUND "ARDA.MOD"
#define CLASSICAL_BEEP_BEEP_BACKGROUND_MUSIC "A_City_at_Night.mod"

// #define SILENCE (-1)
#define SILENCE (NULL)

enum _sounds
{
  ERRORSOUND = 0,
  FORMER_COMBAT_BACKGROUND_MUSIC_SOUND,
  FORMER_TAKEOVER_BACKGROUND_MUSIC_SOUND,
  FORMER_CONSOLE_BACKGROUND_MUSIC_SOUND,
  FORMER_CLASSICAL_BEEP_BEEP_BACKGROUND_MUSIC,
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
  FIRE_BULLET_FLASH_SOUND,
  FIRE_BULLET_EXTERMINATOR_SOUND,
  FIRE_BULLET_LASER_RIFLE_SOUND,
  CRY_SOUND,
  TRANSFER_SOUND,
  MISSION_STATUS_CHANGE_SOUND,
  ITEM_TAKEN_SOUND
};

//--------------------
// The sounds when the influencers energy is low or when he is in transfer mode
// occur periodically.  These constants specify which intervals are to be used
// for these periodic happenings...
#define CRY_SOUND_INTERVAL 2
#define TRANSFER_SOUND_INTERVAL 1.1


//--------------------
//

#define TRUE (1==1)
#define FALSE (1==0)

#define ERR 	-1
#define OK		0

//--------------------
// Ship-Elevator pictures
//

#define NUM_EL_BLOCKS		17

#define EL_BLOCK_LEN		8
#define EL_BLOCK_HEIGHT		8
#define EL_BLOCK_MEM 		EL_BLOCK_LEN * EL_BLOCK_HEIGHT

#define ELEVATOR_LEN		38
#define ELEVATOR_HEIGHT		12

#define BULLETSPEEDINFLUENCE 	2

#define DIRECTIONS 		8

#define ALLSHIPS 		4

#define ENEMYPHASES 		8
#define DROID_PHASES            ENEMYPHASES

#define WAIT_LEVELEMPTY		18	// time in frames to wait before removing the light in an empty level

#define WAIT_AFTER_KILLED	1.5  // time to wait and still display pictures after the destruction of 
                                 // the players droid.  This is now measured in seconds and can be a float
#define WAIT_SHIPEMPTY		20
#define WAIT_TRANSFERMODE	0.3	/* this is a "float" indicating the number of seconds the influence 
					   stand still with space pressed, before switching into transfermode 
					   This variable describes the amount in SECONDS */
#define WAIT_COLLISION		1 // after a little collision with influ, enemys hold position for a while
                                  // this variable describes the amount of time in SECONDS 
#define ENEMYMAXWAIT 2.0		  // after each robot has reached its current destination waypoint is waits a
                                  // while.  This variable describes the amount of time in SECONDS.  However,
                                  // the final wait time is a random number within [0,ENEMYMAXWAIT].
#define FLASH_DURATION_IN_SECONDS 0.1

//--------------------
// direction definitions (fireing bullets and testing blockedness of positions) 
//
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


//--------------------
// Maximal number of ...
//

#define NUM_MAP_BLOCKS		70  /* total number of map-blocks */
#define NUM_COLORS              7   // how many different level colorings?/different tilesets?

// #define ALLBULLETTYPES		4	/* number of bullet-types */
#define ALLBLASTTYPES		2	/* number of different exposions */

#define MAXBULLETS		100	/* maximum possible Bullets in the air */
#define MAXBLASTS		100	/* max. possible Blasts visible */
#define AGGRESSIONMAX  		100
#define ROBOT_MAX_WAIT_BETWEEN_SHOTS 5  // how long shoud each droid wait at most until
                                        // is considers fireing again?

//--------------------
// Map- and levelrelated defines
// WARNING leave them here, they are required in struct.h
//

#define MAX_WP_CONNECTIONS 	12
#define MAXMAPLINES 		50
#define MAX_STATEMENTS_PER_LEVEL 30
#define MAX_CODEPANELS_PER_LEVEL 30

#define MAX_ENEMYS_ON_SHIP	1000
#define MAX_REQUESTS_PER_DROID 5
#define MAX_CHAT_KEYWORDS_PER_DROID 30
#define MAX_INFLU_POSITION_HISTORY 10000
#define MAX_MISSIONS_IN_GAME 10         // how many missions can there be at most in Freedroid
#define MAX_MISSION_TRIGGERED_ACTIONS 10

#define MAX_ITEMS_PER_LEVEL 30
#define MAX_ITEMS_IN_INVENTORY 10
#define NUMBER_OF_ITEM_PICTURES 20
#define ALL_ITEMS 100

#define MAX_LIFTS	 	50  	/* actually the entries to the lifts */
#define MAX_LEVELS		29   	/* don't change this easily */
				     	/* corresponds to a reserved palette range ! */
#define MAX_LIFT_ROWS		15   	/* the different lift "rows" */
					/* don't change this easily */
					/* corresponds to a reserved palette range !*/
#define MAX_LEVEL_RECTS         20   // how many rects compose a level
#define MAX_EVENT_TRIGGERS      20   // how many event triggers at most to allow
#define MAX_TRIGGERED_ACTIONS_IN_GAME   20   // how many triggerable actions to allow at most


#define MAXWAYPOINTS 		100
#define MAX_DOORS_ON_LEVEL 	60
#define MAX_REFRESHES_ON_LEVEL	39
#define MAX_TELEPORTERS_ON_LEVEL	10

#define MAX_PHASES_IN_A_BULLET 12
#define MAX_STEPS_IN_GIVEN_COURSE 1000

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

#define BREMSDREHUNG 3		// STUPID AND SHOULD BE REMOVED

//--------------------
// NORMALISATION #define PUSHSPEED 2
#define PUSHSPEED 2 // speed at which influ will be pushed away from doors and such...

//--------------------
// bullet types
//
enum _bullets
{
  PULSE=0,
  SINGLE_PULSE,
  MILITARY,
  FLASH,
  EXTERMINATOR,
  LASER_RIFLE
};


//--------------------
// explosion types
//
enum _explosions
{
  BULLETBLAST=0,
  DRUIDBLAST,
  OWNBLAST
};

#define BLINKENERGY 25

//--------------------
// droid types
//
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

//--------------------
// possible states of the influencer as displayed in the top status banner
//
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

#define DECKCOMPLETEBONUS 500

//--------------------
// inactive things are assigned the status of 'out' which is somewhat
// dangerous, since it corresponds to 12 or so and this a value that 
// might well be used at some point as a normal index for the possible
// states as well...
// 
#define OUT TERMINATED	

//--------------------
// some constants intended to make the map tiles numbers somewhat
// more intuitive for case-separation and such things...
//
enum
{
  FLOOR = 0,
  ECK_LU,        T_U,          ECK_RU,       T_L,          KREUZ,       T_R,    ECK_LO,  T_O,    ECK_RO,
  H_WALL=10,     V_WALL,       ALERT,        BLOCK1,       BLOCK2,      BLOCK3, BLOCK4,  BLOCK5,
  H_ZUTUERE=18,  H_HALBTUERE1, H_HALBTUERE2, H_HALBTUERE3, H_GANZTUERE,
  KONSOLE_L=23,  KONSOLE_R,    KONSOLE_O,    KONSOLE_U,
  V_ZUTUERE=27,  V_HALBTUERE1, V_HALBTUERE2, V_HALBTUERE3, V_GANZTUERE,
  LIFT=32,       VOID,         REFRESH1,     REFRESH2,     REFRESH3,    REFRESH4,
  TELE_1=38,     TELE_2,       TELE_3,       TELE_4,       INVISIBLE_BRICK,
  LOCKED_H_ZUTUERE=43,         LOCKED_V_ZUTUERE,
  CODEPANEL_L=45,   CODEPANEL_R, CODEPANEL_U, CODEPANEL_D,
  BOX_1=49,      BOX_2,        BOX_3,        BOX_4,        UNUSED_BRICK,
  CONVEY_L=54,   CONVEY_D,     CONVEY_R,     CONVEY_U,
  IDENTIFY_L=58,   IDENTIFY_R,     IDENTIFY_U,     IDENTIFY_D
};


#endif
