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

//--------------------
// Here you can control the debug level for the various modules, or at least
// that's the way it is planned to be.  Turn on and off debugging as you like.
//
#define SAVE_LOAD_GAME_DEBUG 0
#define INVENTORY_RECTANGLE_DEBUG_LEVEL 2 
#define INVENTORY_MOUSE_BUTTON_DEBUG 2

#define MAX_ITEMS_PER_LEVEL 300
#define MAX_ITEMS_IN_INVENTORY 100
#define INVENTORY_GRID_WIDTH 9
#define INVENTORY_GRID_HEIGHT 6
#define NUMBER_OF_ITEM_PICTURES 48
#define NUMBER_OF_MOUSE_CURSOR_PICTURES 2
#define ALL_ITEMS 100

#define ITEM_DROP_PERCENTAGE 20
#define GOLD_DROP_PERCENTAGE 20

#define ITEM_MONEY 6

#define ITEM_SMALL_HEALTH_POTION 1
#define ITEM_SMALL_MANA_POTION 41
#define ITEM_MEDIUM_HEALTH_POTION 48
#define ITEM_MEDIUM_MANA_POTION 49
#define ITEM_FULL_HEALTH_POTION 2
#define ITEM_FULL_MANA_POTION 42

#define ITEM_SHORT_BOW 22
#define ITEM_HUNTERS_BOW 03
#define ITEM_LONG_BOW 16
#define ITEM_COMPOSITE_BOW 12

#define ITEM_ANTIGRAV_ALPHA 5
#define ITEM_ANTIGRAV_BETA 11
#define ITEM_TRACKS 10
#define ITEM_TRIPEDAL 8
#define ITEM_WHEELS 07

#define ITEM_BUCKLER 19
#define ITEM_SMALL_SHIELD 43
#define ITEM_STANDARD_SHIELD 33
#define ITEM_MEDIUM_SHIELD 56
#define ITEM_ROUND_SHIELD 0
#define ITEM_KITE_SHIELD 0
#define ITEM_DOOR_SHIELD 0
#define ITEM_TOWER_SHIELD 0

#define ITEM_CLOAK 25
#define ITEM_ROBE 29
#define ITEM_QUILTED_ARMOUR 30
#define ITEM_LEATHER_ARMOUR 31
#define ITEM_DROID_PLATING 18

#define ITEM_DAGGER 37       // very low sword
#define ITEM_SHORT_SWORD 27  // lowest sword
#define ITEM_SCIMITAR 34     // low sword
#define ITEM_SABER 35        // medium sword
#define ITEM_FALCHION 36     // good sword

#define ITEM_CAP 20
#define ITEM_SMALL_HELM 50
#define ITEM_IRON_HAT 53
#define ITEM_IRON_HELM 54
#define ITEM_POTHELM 55

#define ITEM_STAFF 39

#define ITEM_SPELLBOOK_OF_HEALING 44
#define ITEM_SPELLBOOK_OF_EXPLOSION_CIRCLE 46
#define ITEM_SPELLBOOK_OF_EXPLOSION_RAY 47
#define ITEM_SPELLBOOK_OF_TELEPORT_HOME 45

#define ITEM_START_PLUGIN_WARRIOR 21
#define ITEM_START_PLUGIN_SNIPER 51
#define ITEM_START_PLUGIN_HACKER 52

#define WAR_BOT 1
#define SNIPER_BOT 2
#define MIND_BOT 3

#define NUMBER_OF_SKILLS 6
#define NUMBER_OF_SKILL_LEVELS 10

//--------------------
// This is old and should go out, but that must be done from at home, 
// cause here I can't check if the code is still running without it
//
#define FILENAME_LEN	5000
#define SHIP_EXT	".shp"


#define JOY_MAX_VAL 32767     // maximal amplitude of joystick axis values

#define Set_Rect(rect, xx, yy, ww, hh) {\
(rect).x = (xx); (rect).y = (yy); (rect).w = (ww); (rect).h = (hh); }

#define Copy_Rect(src, dst) {\
(dst).x = (src).x; (dst).y = (src).y; (dst).w = (src).w; (dst).h = (src).h; }

#define UserCenter_x (User_Rect.x + User_Rect.w/2)
#define UserCenter_y (User_Rect.y + User_Rect.h/2)

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

/* Macros */
#define GrobX (Me[0].pos.x / INITIAL_BLOCK_WIDTH)
#define GrobY (Me[0].pos.y / INITIAL_BLOCK_HEIGHT)
#define FeinX (Me[0].pos.x%INITIAL_BLOCK_WIDTH)
#define FeinY (Me[0].pos.y%INITIAL_BLOCK_HEIGHT)

#define CLFeinY ((Me[0].pos.y+INITIAL_BLOCK_HEIGHT/2) % INITIAL_BLOCK_HEIGHT)
#define CLGrobY ((Me[0].pos.y+INITIAL_BLOCK_HEIGHT/2) / INITIAL_BLOCK_HEIGHT)
#define CLFeinX ((Me[0].pos.x+INITIAL_BLOCK_WIDTH/2) % INITIAL_BLOCK_HEIGHT)
#define CLGrobX ((Me[0].pos.x+INITIAL_BLOCK_WIDTH/2) / INITIAL_BLOCK_HEIGHT)

//--------------------
// Constants influencing code generation and
// constants defining flags for some functions:
//
#define USE_SDL_FRAMERATE

#define MAX_CHARACTER_NAME_LENGTH (25)

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
{ ONLY_SHOW_MAP = 1 , DO_SCREEN_UPDATE = 2 , ONLY_SHOW_MAP_AND_TEXT = 4 };


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

#define NE_DROID_BLOCK_FILE	"droids.png"
#define NE_BULLET_BLOCK_FILE 	"bullet.png"
#define NE_BLAST_BLOCK_FILE 	"blast.png"
#define NE_DIGIT_BLOCK_FILE     "digits.gif"
#define NE_ITEMS_BLOCK_FILE     "items.png"
#define MOUSE_CURSOR_BLOCK_FILE "mouse_move_cursor.png"
#define SKILL_ICON_FILE         "skills.png"
#define SKILL_LEVEL_BUTTON_FILE "skill_buttons.png"
#define SHOP_BACKGROUND_IMAGE   "shoppe.jpg"

#define NE_CONSOLE_PIC_FILE    "console_fg.png" 
#define NE_BANNER_BLOCK_FILE    "banner.png"
#define NE_TITLE_PIC_FILE       "title.jpg"
#define NE_CONSOLE_BG_PIC1_FILE "console_bg1.jpg"
#define NE_CONSOLE_BG_PIC2_FILE "console_bg2.jpg"
#define NE_CREDITS_PIC_FILE     "credits.jpg"

#define FREEDROID_LOADING_PICTURE_NAME "startup1.jpg"

#define STANDARD_MISSION        "Paradroid.mission"
#define NEW_MISSION             "CleanPrivateGoodsStorageCellar.mission"

#define PARA_FONT_FILE 		"parafont.png"
#define MENU_FONT_FILE 		"ArialGold_Depleted.png"
// #define RED_FONT_FILE 		"redfont.png"
#define RED_FONT_FILE 		"font05_red.png"
#define BLUE_FONT_FILE 		"font05_white.png"
#define FPS_FONT_FILE 		"font05.png"
#define ICON_FILE		"paraicon.bmp"


//--------------------
// Constants about the size (and mem?) of pixmaps
//
#define SKILL_LEVEL_BUTTON_HEIGHT 32
#define SKILL_LEVEL_BUTTON_WIDTH 30

#define INITIAL_BLOCK_WIDTH	64
#define INITIAL_BLOCK_HEIGHT	64
#define BLOCKMEM  		INITIAL_BLOCK_WIDTH*INITIAL_BLOCK_HEIGHT

/* Scroll- Fenster */
#define SCROLLSTARTX		User_Rect.x
#define SCROLLSTARTY		SCREEN_HEIGHT


#define INITIAL_DIGIT_LENGTH (8*2)
#define INITIAL_DIGIT_HEIGHT (9*2)
#define INITIAL_DIGIT_POS_X 5*2
#define INITIAL_DIGIT_POS_Y 12*2
#define DIGITNUMBER (22+26)
#define DIGITMEM DIGITHEIGHT*DIGITLENGTH*DIGITNUMBER

#define SCALE_FACTOR  2 /* scale-factor between actually displayed screen and 320x220 */
#define SCREEN_WIDTH		(320*SCALE_FACTOR)
#define SCREEN_HEIGHT		(240*SCALE_FACTOR)

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
#define BANNER_WIDTH		SCREEN_WIDTH
// #define BANNER_HEIGHT		( INITIAL_BLOCK_HEIGHT+15 )
// #define BANNER_HEIGHT		( 115 )
#define BANNER_HEIGHT		( 2 )



//--------------------
// Definitions for the menu inside the in-game console
//
#define CONS_MENU_HEIGHT 		256
#define CONS_MENU_LENGTH 		100

//--------------------
// dimensions of the droids as required for functions NotPassable() and CheckEnemyCollsion() )
//
#define DRUIDRADIUSXY	        (7*2/64.0)
#define BULLET_BULLET_COLLISION_DIST (10/64.0)

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
  ENEMY_GOT_HIT_SOUND_0,
  TAKEOVER_SET_CAPSULE_SOUND,
  MENU_ITEM_SELECTED_SOUND,
  MENU_ITEM_DESELECTED_SOUND,
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
  FIRE_BULLET_SINGLE_LASER_SOUND, 
  FIRE_BULLET_PLASMA_PISTOL_SOUND,
  FIRE_BULLET_SWORD_SOUND,
  BULLET_REFLECTED_SOUND,
  CRY_SOUND,
  TRANSFER_SOUND,
  MISSION_STATUS_CHANGE_SOUND,
  ITEM_TAKEN_SOUND,
  CANT_CARRY_SOUND,
  MS_MACHINES_CLOSE_NOW_SOUND,
  ITEM_DROP_SOUND_0,
  ITEM_DROP_SOUND_1,
  ITEM_DROP_SOUND_2,
  ITEM_DROP_SOUND_3,
  ITEM_DROP_SOUND_4,
  ITEM_DROP_SOUND_5,
  ITEM_ARMOUR_PUT_SOUND,
  ITEM_WHEELS_PUT_SOUND,
  ITEM_RANGE_WEAPON_PUT_SOUND,
  FIRST_CONTACT_SOUND_0,
  FIRST_CONTACT_SOUND_1,
  NOT_ENOUGH_POWER_SOUND,
  NOT_ENOUGH_DIST_SOUND,
  NOT_ENOUGH_FORCE_SOUND,
  INFLUENCER_SCREAM_SOUND_0,
  INFLUENCER_SCREAM_SOUND_1,
  INFLUENCER_SCREAM_SOUND_2,
  INFLUENCER_SCREAM_SOUND_3,
  INFLUENCER_SCREAM_SOUND_4,
  SPELL_FORCETOENERGY_SOUND_0
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

#define MAX_PLAYERS             5

#define MAX_MAP_INSERTS         3
#define MAX_MAP_INSERTS_PER_LEVEL 20

#define EL_BLOCK_LEN		8
#define EL_BLOCK_HEIGHT		8
#define EL_BLOCK_MEM 		EL_BLOCK_LEN * EL_BLOCK_HEIGHT

#define ELEVATOR_LEN		38
#define ELEVATOR_HEIGHT		12

#define BULLETSPEEDINFLUENCE 	2

#define DIRECTIONS 		8

#define ALLSHIPS 		4

#define DROID_PHASES            8
#define DEAD_DROID_PHASES       1
#define TUX_SWING_PHASES 	7
#define TUX_BREATHE_PHASES 	5
#define TUX_GOT_HIT_PHASES      3
#define TUX_MODELS              9

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
#define MAX_MAP_LINES 		100
#define MAX_STATEMENTS_PER_LEVEL 30
#define MAX_CODEPANELS_PER_LEVEL 30

#define RIGHT_WALL_BIT 1
#define LEFT_WALL_BIT 2
#define UP_WALL_BIT 4
#define DOWN_WALL_BIT 8

#define MAX_ENEMYS_ON_SHIP	1000
#define MAX_REQUESTS_PER_DROID 5
#define MAX_CHAT_KEYWORDS_PER_DROID 30
#define MAX_INFLU_POSITION_HISTORY 10000
#define MAX_MISSIONS_IN_GAME 10         // how many missions can there be at most in Freedroid
#define MAX_MISSION_TRIGGERED_ACTIONS 10

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
// #define MAX_STEPS_IN_GIVEN_COURSE 1000
#define MAX_STEPS_IN_GIVEN_COURSE 100

#define UNCHECKED 0
#define HIT 1
#define MISS 2



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
  LASER_RIFLE,
  SINGLE_LASER,
  PLASMA_PISTOL
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
  TERMINATED=-1,
  MOBILE,
  TRANSFERMODE,
  WEAPON,
  CAPTURED,
  COMPLETE,
  REJECTED,
  CONSOLE,
  DEBRIEFING,
  PAUSE,
  CHEESE,
  ELEVATOR,
  BRIEFING,
  MENU
};

//--------------------
// The possible networking statuses of clients.
//
enum _networking_status
  {
    NETWORK_ERROR = 0 ,
    UNCONNECTED , 
    CONNECTION_FRESHLY_OPENED ,
    NAME_HAS_BEEN_TRANSMITTED , 
    GAME_ON
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
