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

#define JOY_MAX_VAL 32767     // maximal amplitude of joystick axis values

#define UserCenter_x (User_Rect.x + User_Rect.w/2)
#define UserCenter_y (User_Rect.y + User_Rect.h/2)

#define Set_Rect(rect, xx, yy, ww, hh) do {\
(rect).x = (xx); (rect).y = (yy); (rect).w = (ww); (rect).h = (hh); } while(0)

#define Copy_Rect(src, dst) do {\
(dst).x = (src).x; (dst).y = (src).y; (dst).w = (src).w; (dst).h = (src).h; } while(0)

#define FreeIfUsed(pt) do { if ((pt)) SDL_FreeSurface((pt)); } while(0)

#define COLLISION_STEPSIZE   0.1

// **********************************************************************
// Constants influencing code generation and
// constants defining flags for some functions:

/* ************************************************************
 * Highscore related defines 
 *************************************************************/
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


// **********************************************************************
// Constants for Paths and names of Data-files
// the root "DATADIR" should be defined in the Makefile as $(pkgdatadir)
// if not, we set it here:
#ifndef DATADIR
#define DATADIR ".."   // our local fallback
#endif

#define GRAPHICS_DIR		"graphics/"
#define SOUND_DIR		"sound/"
#define MAP_DIR			"map/"

#define MAP_BLOCK_FILE          "map_blocks.png"
#define DROID_BLOCK_FILE	"droids.png"
#define BULLET_BLOCK_FILE 	"bullet.png"
#define BLAST_BLOCK_FILE 	"blast.png"
#define DIGIT_BLOCK_FILE        "digits.png"


#define BANNER_BLOCK_FILE       "banner.png"
#define TITLE_PIC_FILE          "title.jpg"
#define CONSOLE_PIC_FILE        "console_fg.png" 
#define CONSOLE_BG_PIC1_FILE    "console_bg1.jpg"
#define CONSOLE_BG_PIC2_FILE    "console_bg2.jpg"
#define CREDITS_PIC_FILE        "credits.jpg"

#define SHIP_ON_PIC_FILE        "ship_on.png"
#define SHIP_OFF_PIC_FILE       "ship_off.png"

#define STANDARD_MISSION        "Paradroid.mission"
#define NEW_MISSION             "CleanPrivateGoodsStorageCellar.mission"

#define PARA_FONT_FILE 		"parafont.png"

#define FPS_FONT_FILE 		"font05.png"
#define ICON_FILE		"paraicon.bmp"


// **********************************************************************
// Constants about the size (and mem?) of pixmaps

#define INITIAL_BLOCK_WIDTH	64
#define INITIAL_BLOCK_HEIGHT	64

#define INITIAL_DIGIT_WIDTH 16
#define INITIAL_DIGIT_HEIGHT 18

#define DIGITNUMBER 10

#define SCALE_FACTOR  2 /* scale-factor between actually displayed screen and 320x220 */
#define SCREENLEN		(320*SCALE_FACTOR)
#define SCREENHEIGHT		(240*SCALE_FACTOR)

#define USERFENSTERHOEHE 	4*INITIAL_BLOCK_HEIGHT
#define USERFENSTERBREITE 	9*INITIAL_BLOCK_WIDTH
#define USERFENSTERPOSX 	( (SCREENLEN-USERFENSTERBREITE) / 2)
#define USERFENSTERPOSY 	( 2*(SCREENHEIGHT-USERFENSTERHOEHE)/3 )
#define USER_FENSTER_CENTER_X (USERFENSTERPOSX + (USERFENSTERBREITE/2))
#define USER_FENSTER_CENTER_Y (USERFENSTERPOSY + (USERFENSTERHOEHE/2))

/* Startpos + dimesnions of Banner-Texts */
#define LEFT_INFO_X 	(13*2)
#define LEFT_INFO_Y	(10*2)
#define RIGHT_INFO_X	(242*2)
#define RIGHT_INFO_Y	(8*2)
#define LEFT_TEXT_LEN 10
#define RIGHT_TEXT_LEN 6
#define TEXT_STRETCH 1.2
#define BANNER_WIDTH		SCREENLEN
#define BANNER_HEIGHT		64


#define BULLET_BULLET_COLLISION_DIST (10.0/64.0)
#define BULLET_COLL_DIST2     0.0244140625
// **********************************************************************
//
//

// The following is the definition of the sound file names used in freedroid
// DO NOT EVER CHANGE THE ORDER OF APPEARENCE IN THIS LIST PLEASE!!!!!
// The order of appearance here should match the order of appearance 
// in the SoundSampleFilenames definition located in sound.c!
enum _sounds
{
  ERRORSOUND = 0,
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
  COUNTDOWN_SOUND,
  ENDCOUNTDOWN_SOUND,
  ALL_SOUNDS   // marks the last entry always!
};


// choose background music by level-color:
// if filename_raw==BYCOLOR then chose bg_music[color]
#define BYCOLOR "BYCOLOR"   
 
// The sounds when the influencers energy is low or when he is in transfer mode
// occur periodically.  These constants specify which intervals are to be used
// for these periodic happenings...
#define CRY_SOUND_INTERVAL 2
#define TRANSFER_SOUND_INTERVAL 1.1


// **********************************************************************

#define TRUE (1==1)
#define FALSE (1==0)

#define ERR 	-1
#define OK		0

/* Ship-Elevator Picture */

#define DIRECTIONS 		8

#define ALLSHIPS 		4
#define ENEMYPHASES 		8
#define DROID_PHASES            ENEMYPHASES

#define WAIT_LEVELEMPTY		1.0	/* warte bevor Graufaerben (in seconds)*/
#define WAIT_AFTER_KILLED	1500  // time (in ms) to wait and still display pictures after the destruction of 
#define SHOW_WAIT           3500   // std amount of time to show something
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
#define FLASH_DURATION           0.1   // in seconds

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


/* Maximal number of ... */

#define NUM_MAP_BLOCKS		51  // total number of map-blocks 
#define NUM_COLORS              7   // how many different level colorings?/different tilesets?

#define ALLALERTCOLORS 		3

// #define ALLBULLETTYPES		4	/* number of bullet-types */
#define ALLBLASTTYPES		2	/* number of different exposions */

#define MAXBULLETS		100	/* maximum possible Bullets in the air */
#define MAXBLASTS		100	/* max. possible Blasts visible */
#define AGGRESSIONMAX  		100
#define ROBOT_MAX_WAIT_BETWEEN_SHOTS 5  // how long shoud each droid wait at most until
                                        // is considers fireing again?

/* Map-related defines:
	WARNING leave them here, they are required in struct.h
*/
#define MAX_WP_CONNECTIONS 	12
#define MAXMAPLINES 		50
#define MAX_ENEMYS_ON_SHIP	300
#define MAX_CHAT_KEYWORDS_PER_DROID 30
#define MAX_INFLU_POSITION_HISTORY 100

#define MAX_LIFTS	 	50  	/* actually the entries to the lifts */
#define MAX_LEVELS		29   	/* don't change this easily */
				     	/* corresponds to a reserved palette range ! */
#define MAX_LIFT_ROWS		15   	/* the different lift "rows" */
					/* don't change this easily */
					/* corresponds to a reserved palette range !*/
#define MAX_LEVEL_RECTS         20   // how many rects compose a level
#define MAX_EVENT_TRIGGERS      20   // how many event triggers at most to allow
#define MAX_TRIGGERED_ACTIONS   20   // how many triggerable actions to allow at most


#define MAXWAYPOINTS 		100
#define MAX_DOORS_ON_LEVEL 	60
#define MAX_REFRESHES_ON_LEVEL	39
#define MAX_TELEPORTERS_ON_LEVEL	10

#define MAX_PHASES_IN_A_BULLET 12
#define MAX_STEPS_IN_GIVEN_COURSE 1000

#define BREMSDREHUNG 3		/* warte 3*, bevor Influencer weitergedreht wird */

/* Wegstossgeschw. von Tueren u.ae. */
// NORMALISATION #define PUSHSPEED 2
#define PUSHSPEED 2

/* Schusstypen */
enum _bullets
{
  PULSE=0,
  SINGLE_PULSE,
  MILITARY,
  FLASH,
  EXTERMINATOR,
  LASER_RIFLE
};


/* Explosionstypen */
enum _explosions
{
  BULLETBLAST=0,
  DRUIDBLAST,
  REJECTBLAST
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
  MENU,
  VICTORY
};

#define DECKCOMPLETEBONUS 500

#define OUT TERMINATED		/* Type fuer inaktive Dinge */

/* Konstanten die die Kartenwerte anschaulich machen */
enum
{
  FLOOR = 0,
  ECK_LU,        T_U,          ECK_RU,       T_L,          KREUZ,       T_R,    ECK_LO,  T_O,    ECK_RO,
  H_WALL=10,     V_WALL,       ALERT,        BLOCK1,       BLOCK2,      BLOCK3, BLOCK4,  BLOCK5,
  H_ZUTUERE=18,  H_HALBTUERE1, H_HALBTUERE2, H_HALBTUERE3, H_GANZTUERE,
  KONSOLE_L=23,  KONSOLE_R,    KONSOLE_O,    KONSOLE_U,
  V_ZUTUERE=27,  V_HALBTUERE1, V_HALBTUERE2, V_HALBTUERE3, V_GANZTUERE,
  LIFT=32,       VOID,         REFRESH1,     REFRESH2,     REFRESH3,    REFRESH4,
  TELE_1=38,     TELE_2,       TELE_3,       TELE_4,       INVISIBLE_BRICK,     FINE_GRID
};


#endif
