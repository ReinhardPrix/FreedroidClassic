/* 
 *
 *   Copyright (c) 1994, 2002, 2003 Johannes Prix
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
// Some technical definitions...
//
#ifndef TRUE
#define TRUE (1==1)
#endif
#ifndef FALSE
#define FALSE (1==0)
#endif

#define ERR 	-1
#define OK		0

#define max(x,y) ((x) < (y) ? (y) : (x) ) 
#define min(x,y) ((x) > (y) ? (y) : (x) ) 

#define USE_SDL_FRAMERATE

//--------------------
// Here you can control the debug level for the various modules, or at least
// that's the way it is planned to be.  Turn on and off debugging as you like.
//
#define SAVE_LOAD_GAME_DEBUG 1
#define INVENTORY_RECTANGLE_DEBUG_LEVEL 2 
#define INVENTORY_MOUSE_BUTTON_DEBUG 2
enum
  {
    NO_NEED_TO_INFORM = 0,
    PLEASE_INFORM = 1
  };
enum
  {
    IS_WARNING_ONLY = 0,
    IS_FATAL = 1
  };
enum
  {
    MENU_MODE_FAST = 2,
    MENU_MODE_DEFAULT,
    MENU_MODE_DOUBLE,
  };

enum
  {
    COLLISION_TYPE_NONE = -1 , 
    COLLISION_TYPE_RECTANGLE = 2 ,
    COLLISION_TYPE_CIRCLE = 3 
  };

#define ENEMY_ROTATION_MODELS_AVAILABLE 29
#define ROTATION_ANGLES_PER_ROTATION_MODEL 8
#define MAX_OBSTACLES_ON_MAP 10000
#define FLOOR_TILES_VISIBLE_AROUND_TUX 9
#define MAX_OBSTACLES_GLUED_TO_ONE_MAP_TILE 4
#define MAX_ITEMS_PER_LEVEL 300
#define MAX_MAP_LABELS_PER_LEVEL 100
#define MAX_ITEMS_IN_INVENTORY 100
#define INVENTORY_GRID_WIDTH 9
#define INVENTORY_GRID_HEIGHT 6
#define NUMBER_OF_ITEM_PICTURES 68
#define NUMBER_OF_OBSTACLE_TYPES 59
#define NUMBER_OF_MOUSE_CURSOR_PICTURES 2

#define ITEM_DROP_PERCENTAGE 20
#define GOLD_DROP_PERCENTAGE 20

#define MAX_DIALOGUE_OPTIONS_IN_ROSTER 100
#define MAX_REPLIES_PER_OPTION 100
#define MAX_SUBTITLES_N_SAMPLES_PER_DIALOGUE_OPTION 20
#define MAX_EXTRAS_PER_OPTION 10
#define CHAT_DEBUG_LEVEL 1



#ifndef _itemdefs_h
#include "itemdefs.h"
#endif

#define WAR_BOT 1
#define SNIPER_BOT 2
#define MIND_BOT 3

#define NUMBER_OF_SKILL_LEVELS 10
#define NUMBER_OF_SKILLS_PER_SKILL_LEVEL 5
#define NUMBER_OF_SKILLS ( NUMBER_OF_SKILLS_PER_SKILL_LEVEL * NUMBER_OF_SKILL_LEVELS )

#define MAX_ACTIVE_SPELLS 100

enum _spell_names
{
  SPELL_TRANSFERMODE = 0, // 0
  SPELL_LOOT_CHEST_OR_DEAD_BODY, 
  SPELL_REPAIR_SKILL, // 
  SPELL_WEAPON, // 
  SPELL_FORCE_TO_ENERGY, // 
  SPELL_TELEPORT_HOME, // 
  SPELL_FIREY_BOLT, // 
  SPELL_COLD_BOLT, // 
  SPELL_FORCE_EXPLOSION_CIRCLE, // 
  SPELL_POISON_BOLT, // 
  SPELL_PARALYZE_BOLT, // 
  SPELL_DETECT_ITEM, // 
  SPELL_RADIAL_EMP_WAVE, //
  SPELL_RADIAL_VMX_WAVE, // 
  SPELL_RADIAL_FIRE_WAVE, // 
  SPELL_EXTRACT_PLASMA_TRANSISTORS,
  SPELL_EXTRACT_SUPERCONDUCTORS,
  SPELL_EXTRACT_ANTIMATTER_CONVERTERS,
  SPELL_EXTRACT_ENTROPY_INVERTERS,
  SPELL_EXTRACT_TACHYON_CONDENSATORS,
  SPELL_FORCE_EXPLOSION_RAY, // 
};

//--------------------
// This is old and should go out, but that must be done from at home, 
// cause here I can't check if the code is still running without it
//
#define FILENAME_LEN	5000
#define SHIP_EXT	".shp"

#define END_OF_OFFSET_FILE_STRING "** End of iso_image offset file **"
#define START_OF_OFFSET_FILE_STRING "** Start of iso_image offset file **"
#define OFFSET_FILE_OFFSETX_STRING "OffsetX="
#define OFFSET_FILE_OFFSETY_STRING "OffsetY="

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

#define MAX_CHARACTER_NAME_LENGTH (25)
#define MAX_LENGTH_FOR_DIALOG_SECTION_NAME 100
#define MAX_LENGTH_OF_SHORT_DESCRIPTION_STRING 200

//--------------------
// Highscore related defines 
// 
#define HS_EMPTY_ENTRY  "--- empty ---"
#define MAX_NAME_LEN 	15     /* max len of highscore name entry */
#define MAX_HIGHSCORES  10     /* only keep Top10 */
#define DATE_LEN	10     /* reserved for the date-string */

// The flags for DisplayBanner are:
enum
  { BANNER_FORCE_UPDATE=1 , BANNER_DONT_TOUCH_TEXT=2 , BANNER_NO_SDL_UPDATE=4 };

// The flags for AssembleCombatWindow are:
enum
{ ONLY_SHOW_MAP = 1 , DO_SCREEN_UPDATE = 2 , ONLY_SHOW_MAP_AND_TEXT = 4 , SHOW_GRID = 8 , SHOW_TUX_AND_ENEMIES = 16 , SHOW_TEXT = 32 , SHOW_ITEMS = 64 , OMIT_OBSTACLES = 128 , OMIT_TUX = 256 , OMIT_ENEMIES = 512 };

// The color filters are:
enum
{ FILTER_BLUE = 1 , FILTER_GREEN = 2 , FILTER_RED = 3 };


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
#define DIALOG_DIR		"dialogs/"

#define BALL_SHAPED_DROIDS_FILE	"droids.png"
#define TUX_MOTIONS_FILE        "tux_motions.png"
#define NE_BULLET_BLOCK_FILE 	"bullet.png"
#define NE_BLAST_BLOCK_FILE 	"blast.png"
#define NE_DIGIT_BLOCK_FILE     "digits.gif"
#define NE_ITEMS_BLOCK_FILE     "items.png"
#define MOUSE_CURSOR_BLOCK_FILE "mouse_move_cursor.png"
#define SKILL_ICON_FILE         "skills.png"
#define SKILL_LEVEL_BUTTON_FILE "skill_buttons.png"
#define SHOP_BACKGROUND_IMAGE   "backgrounds/shoppe.jpg"

#define NE_CONSOLE_PIC_FILE     "backgrounds/console_fg.png" 
#define NE_TITLE_PIC_FILE       "backgrounds/title.jpg"
#define NE_CONSOLE_BG_PIC1_FILE "backgrounds/console_bg1.jpg"
#define NE_CONSOLE_BG_PIC2_FILE "backgrounds/console_bg2.jpg"
#define ITEM_BROWESER_BG_PIC_FILE "backgrounds/item_browser.jpg"
#define NE_CREDITS_PIC_FILE     "backgrounds/credits.jpg"
#define FREEDROID_LOADING_PICTURE_NAME "backgrounds/startup1.jpg"
#define HS_BACKGROUND_FILE     "backgrounds/transfer.jpg"  

#define CHARACTER_SCREEN_BACKGROUND_FILE "backgrounds/character.png" 
#define SKILL_SCREEN_BACKGROUND_FILE "backgrounds/SkillScreen.png" 
#define SKILL_EXPLANATION_SCREEN_BACKGROUND_FILE "backgrounds/SkillExplanationScreen.png" 
#define INVENTORY_SCREEN_BACKGROUND_FILE "backgrounds/inventory.png"
#define INVENTORY_SQUARE_OCCUPIED_FILE "backgrounds/TransparentPlate.png"
#define LEVEL_EDITOR_BANNER_FILE "backgrounds/LevelEditorBanner.png"

#define NEW_MISSION             "ReturnOfTux.mission"

#define PARA_FONT_FILE 		"font/parafont.png"
#define MENU_FONT_FILE 		"font/cpuFont.png"
#define MENU_FILLED_FONT_FILE 	"font/ArialGold.png"
#define RED_FONT_FILE 		"font/font05_red.png"
#define BLUE_FONT_FILE 		"font/font05_white.png"
#define FPS_FONT_FILE 		"font/font05.png"
#define ICON_FILE		"paraicon.bmp"


//--------------------
// Constants about the size (and mem?) of pixmaps
//
#define SKILL_LEVEL_BUTTON_HEIGHT 32
#define SKILL_LEVEL_BUTTON_WIDTH 30

#define INITIAL_BLOCK_WIDTH	64
#define INITIAL_BLOCK_HEIGHT	64
// #define ISO_WIDTH               128.0
// #define ISO_HEIGHT              94.0
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

//--------------------
// Startpos + dimesnions of Banner-Texts 
//
#define LEFT_INFO_X 	(13*2)
#define LEFT_INFO_Y	(10*2)
#define RIGHT_INFO_X	(242*2)
#define RIGHT_INFO_Y	(8*2)
#define LEFT_TEXT_LEN 10
#define RIGHT_TEXT_LEN 6
#define TEXT_STRETCH 1.2
#define BANNER_WIDTH		SCREEN_WIDTH
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
#define TAKEOVER_BACKGROUND_MUSIC_SOUND "Intro-Music.mod"
#define CONSOLE_BACKGROUND_MUSIC_SOUND "ARDA.MOD"
#define CREDITS_BACKGROUND_MUSIC_SOUND "NOWHERE.MOD"
#define SILENCE (NULL)
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
  CANT_CARRY_SOUND_0,
  CANT_CARRY_SOUND_1,
  CANT_CARRY_SOUND_2,
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
  FIRST_CONTACT_SOUND_2,
  FIRST_CONTACT_SOUND_3,
  NOT_ENOUGH_POWER_SOUND,
  NOT_ENOUGH_DIST_SOUND,
  NOT_ENOUGH_FORCE_SOUND,
  INFLUENCER_SCREAM_SOUND_0,
  INFLUENCER_SCREAM_SOUND_1,
  INFLUENCER_SCREAM_SOUND_2,
  INFLUENCER_SCREAM_SOUND_3,
  INFLUENCER_SCREAM_SOUND_4,
  SPELL_FORCETOENERGY_SOUND_0,
  SPELL_DETECTITEMS_SOUND_0,
  NO_AMMO_SOUND_0
};

//--------------------
// The sounds when the influencers energy is low or when he is in transfer mode
// occur periodically.  These constants specify which intervals are to be used
// for these periodic happenings...
#define CRY_SOUND_INTERVAL 2
#define TRANSFER_SOUND_INTERVAL 1.1


//--------------------
// Ship-Elevator pictures
//

#define NUM_EL_BLOCKS		17

// #define MAX_PLAYERS             5
#define MAX_PLAYERS             1
#define MAX_PLAYERS_AT_MOST     5
#define PLAYER_NR_0             0

enum _non_player_characters
  {
    PERSON_CHA = 0,
    PERSON_SORENSON,
    PERSON_RMS,
    PERSON_STONE,
    PERSON_DIXON,
    PERSON_DUNCAN,
    PERSON_PENDRAGON,
    PERSON_614,
    PERSON_MER,
    PERSON_FRANCIS,
    PERSON_ERNIE,
    PERSON_BENJAMIN,
    PERSON_BENDER,
    PERSON_SPENCER,
    PERSON_BUTCH,
    PERSON_DARWIN,
    PERSON_DOC_MOORE,
    PERSON_MELFIS,
    PERSON_MICHELANGELO,
    PERSON_SKIPPY,
    PERSON_STANDARD_OLD_TOWN_GATE_GUARD,
    PERSON_STANDARD_NEW_TOWN_GATE_GUARD,
    PERSON_OLD_TOWN_GATE_GUARD_LEADER,
    PERSON_STANDARD_MS_FACILITY_GATE_GUARD,
    PERSON_MS_FACILITY_GATE_GUARD_LEADER,
    PERSON_HEA,
    PERSON_BRUCE,
    PERSON_SUBDIALOG_DUMMY,
    LAST_PERSON
  };
#define MAX_PERSONS             (LAST_PERSON)

#define MAX_ANSWERS_PER_PERSON 100
#define END_ANSWER (MAX_ANSWERS_PER_PERSON-1)

#define MAX_MAP_INSERTS         8
#define MAX_MAP_INSERTS_PER_LEVEL 20

#define MAX_MOUSE_PRESS_BUTTONS 87
enum
  { 
    CHA_BUTTON, 
    INV_BUTTON,
    SKI_BUTTON, 
    PLUS_BUTTON,
    UP_BUTTON,
    DOWN_BUTTON,
    LEFT_BUTTON,
    RIGHT_BUTTON,
    MAP_EXIT_BUTTON,
    MAP_UNLOCK_BUTTON_GREEN,
    MAP_UNLOCK_BUTTON_YELLOW,
    MAP_UNLOCK_BUTTON_RED,
    MAP_GUNOFF_BUTTON_GREEN,
    MAP_GUNOFF_BUTTON_YELLOW,
    MAP_GUNOFF_BUTTON_RED,
    MAP_SECURITYMIDDLE_BUTTON,
    MAP_SECURITYLEFT_BUTTON,
    MAP_SECURITYRIGHT_BUTTON,
    MAP_PASSWORDMIDDLE_BUTTON,
    MAP_PASSWORDLEFT_BUTTON,
    MAP_PASSWORDRIGHT_BUTTON,
    MAP_REQUEST_ENERGY_RATION_GREEN_BUTTON,
    MAP_REQUEST_ENERGY_RATION_RED_BUTTON,
    MAP_READ_EMAIL_GREEN_BUTTON,
    MAP_READ_EMAIL_RED_BUTTON,
    MAP_GUN_TYPE_1_BUTTON_RED,
    MAP_GUN_TYPE_2_BUTTON_RED,
    MAP_GUN_TYPE_3_BUTTON_RED,
    MAP_GUN_TYPE_4_BUTTON_RED,
    MAP_GUNON_BUTTON_GREEN,
    MAP_GUNON_BUTTON_YELLOW,
    MAP_GUNON_BUTTON_RED,
    MAP_GUN_TYPE_1_BUTTON_YELLOW,
    MAP_GUN_TYPE_2_BUTTON_YELLOW,
    MAP_GUN_TYPE_3_BUTTON_YELLOW,
    MAP_GUN_TYPE_4_BUTTON_YELLOW,
    CONSOLE_IDENTIFY_BUTTON_GREEN,
    CONSOLE_IDENTIFY_BUTTON_RED,
    CONSOLE_IDENTIFY_BUTTON_YELLOW,
    ITEM_BROWSER_LEFT_BUTTON,
    ITEM_BROWSER_RIGHT_BUTTON,
    ITEM_BROWSER_EXIT_BUTTON,

    LEFT_SHOP_BUTTON,
    RIGHT_SHOP_BUTTON,
    LEFT_TUX_SHOP_BUTTON,
    RIGHT_TUX_SHOP_BUTTON,
    LEFT_LEVEL_EDITOR_BUTTON,
    RIGHT_LEVEL_EDITOR_BUTTON,

    NUMBER_SELECTOR_OK_BUTTON,
    NUMBER_SELECTOR_LEFT_BUTTON,
    NUMBER_SELECTOR_RIGHT_BUTTON,

    BUY_BUTTON,
    // BUY_10_BUTTON,
    // BUY_100_BUTTON,
    SELL_BUTTON,
    // SELL_10_BUTTON,
    // SELL_100_BUTTON,
    TAKE_BUTTON,
    // TAKE_10_BUTTON,
    // TAKE_100_BUTTON,
    PUT_BUTTON,
    // PUT_10_BUTTON,
    // PUT_100_BUTTON,

    OPEN_CLOSE_SKILL_EXPLANATION_BUTTON,

    GO_LEVEL_NORTH_BUTTON,
    GO_LEVEL_SOUTH_BUTTON,
    GO_LEVEL_EAST_BUTTON,
    GO_LEVEL_WEST_BUTTON,
    EXPORT_THIS_LEVEL_BUTTON,
    LEVEL_EDITOR_SAVE_SHIP_BUTTON,
    LEVEL_EDITOR_ZOOM_IN_BUTTON,
    LEVEL_EDITOR_ZOOM_OUT_BUTTON,
    LEVEL_EDITOR_RECURSIVE_FILL_BUTTON,
    LEVEL_EDITOR_NEW_MAP_LABEL_BUTTON,
    LEVEL_EDITOR_NEW_ITEM_BUTTON,
    LEVEL_EDITOR_LEVEL_RESIZE_BUTTON,
    LEVEL_EDITOR_KEYMAP_BUTTON,
    LEVEL_EDITOR_QUIT_BUTTON,

    LEVEL_EDITOR_TOGGLE_TUX_BUTTON,
    LEVEL_EDITOR_TOGGLE_ENEMIES_BUTTON,
    LEVEL_EDITOR_TOGGLE_OBSTACLES_BUTTON,

    LEVEL_EDITOR_NEXT_ITEM_GROUP_BUTTON,
    LEVEL_EDITOR_PREV_ITEM_GROUP_BUTTON,
    LEVEL_EDITOR_CANCEL_ITEM_DROP_BUTTON,

    SAVE_GAME_BANNER,
    LOAD_GAME_BANNER,

    WEAPON_RECT_BUTTON,
    DRIVE_RECT_BUTTON,
    SHIELD_RECT_BUTTON,
    AUX1_RECT_BUTTON,
    AUX2_RECT_BUTTON,
    SPECIAL_RECT_BUTTON,
    ARMOUR_RECT_BUTTON,

    SCROLL_DIALOG_MENU_UP_BUTTON,
    SCROLL_DIALOG_MENU_DOWN_BUTTON,
  };


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

// #define TUX_SWING_PHASES 	7
// #define TUX_BREATHE_PHASES 	5
// #define TUX_GOT_HIT_PHASES      3

#define TUX_SWING_PHASES 	14
#define TUX_BREATHE_PHASES 	1
#define TUX_GOT_HIT_PHASES      0


// #define TUX_MODELS              9
#define TUX_MODELS              13
#define MAX_TUX_DIRECTIONS      16
#define BULLET_DIRECTIONS       16

#define WAIT_LEVELEMPTY		18	// time in frames to wait before removing the light in an empty level

#define WAIT_AFTER_KILLED	1.5  // time to wait and still display pictures after the destruction of 
                                 // the players droid.  This is now measured in seconds and can be a float
#define WAIT_SHIPEMPTY		20
#define WAIT_TRANSFERMODE	0.3	/* this is a "float" indicating the number of seconds the influence 
					   stand still with space pressed, before switching into transfermode 
					   This variable describes the amount in SECONDS */
#define WAIT_COLLISION		2.5 // after a little collision with Tux or another enemy, hold position for a while
                                    // this variable describes the amount of time in SECONDS 
// #define ENEMYMAXWAIT 2.0		  // after each robot has reached its current destination waypoint is waits a
                                  // while.  This variable describes the amount of time in SECONDS.  However,
                                  // the final wait time is a random number within [0,ENEMYMAXWAIT].
#define FLASH_DURATION_IN_SECONDS 0.1

#define TUX_MAXSPEED_WITHOUT_DRIVE_ITEM 2
#define TUX_ACCEL_WITHOUT_DRIVE_ITEM 2

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
#define MAX_CLEARANCES 100
#define MAX_PASSWORDS 100
#define MAX_PASSWORD_LENGTH 20
#define MAX_COOKIES 100
#define MAX_COOKIE_LENGTH 200

#define RIGHT_WALL_BIT 1
#define LEFT_WALL_BIT 2
#define UP_WALL_BIT 4
#define DOWN_WALL_BIT 8

#define MAX_ENEMYS_ON_SHIP	1000
#define MAX_INFLU_POSITION_HISTORY 10000
#define MAX_MISSIONS_IN_GAME 10         // how many missions can there be at most in Freedroid
#define MAX_MISSION_TRIGGERED_ACTIONS 10

#define MAX_LIFTS	 	50  	/* actually the entries to the lifts */
#define MAX_LEVELS		39   	// how many map levels are allowed in one ship
				     	/* corresponds to a reserved palette range ! */
#define MAX_LIFT_ROWS		15   	/* the different lift "rows" */
					/* don't change this easily */
					/* corresponds to a reserved palette range !*/
#define MAX_LEVEL_RECTS         20   // how many rects compose a level
#define MAX_EVENT_TRIGGERS      100   // how many event triggers at most to allow
#define MAX_TRIGGERED_ACTIONS_IN_GAME   100   // how many triggerable actions to allow at most


#define MAXWAYPOINTS 		100
#define MAX_DOORS_ON_LEVEL 	80
#define MAX_AUTOGUNS_ON_LEVEL 	60
#define MAX_REFRESHES_ON_LEVEL	39
#define MAX_CONSUMERS_ON_LEVEL	39
#define MAX_TELEPORTERS_ON_LEVEL 20

#define MAX_PHASES_IN_A_BULLET 12
// #define MAX_STEPS_IN_GIVEN_COURSE 1000
#define MAX_STEPS_IN_GIVEN_COURSE 100

#undef USE_MISS_HIT_ARRAYS
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
  PLASMA_PISTOL,
  LASER_SWORD_1,
  LASER_AXE,
  LASER_SWORD_2,
  GREEN_BULLET,
  BLUE_BULLET,
  MAGENTA_BULLET,
  WHITE_BULLET
  //--------------------
  // If you add some more bullet types here, don't forget to
  //   1.  Also add a specification in the freedroid ruleset
  //   2.  Also add an entry for the number of phases in the config.theme file!!
  //   3.  Specify the size of the bullet image in the blocks.c loading function
  //
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
#define DRUID001 0

//--------------------
// possible states of the influencer as displayed in the top status banner
//
enum _status
{
  OUT=-30,
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
// The possible edito modes for the level editor
//
enum _level_editor_edit_modes
  {
    LEVEL_EDITOR_EDIT_FLOOR = 3,
    LEVEL_EDITOR_EDIT_OBSTACLES = 4
  };

enum _enemy_combat_state_machine_states
{
  MOVE_ALONG_RANDOM_WAYPOINTS = 2, 
  STOP_AND_EYE_TUX,
  MAKE_ATTACK_RUN, 
  HUNTING_TUX, 
  // BACK_OFF_AFTER_GETTING_HIT, 
  SEEK_BETTER_POSITION,
  FIGHT_ON_TUX_SIDE,
  RELENTLESS_FIRE_TO_GIVEN_POSITION,
  TURN_THOWARDS_NEXT_WAYPOINT,
  RUSH_TUX_ON_SIGHT_AND_OPEN_TALK,
  WAIT_AND_TURN_AROUND_AIMLESSLY
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
// For shop interfaces and chest interfaces we need some constants to
// transmit the intended shop/chest action.
//
enum
  {
    DO_NOTHING = -1 , 
    BUY_1_ITEM = 1 , 
    BUY_10_ITEMS = 2 , 
    BUY_100_ITEMS = 3 , 
    SELL_1_ITEM = 4 , 
    SELL_10_ITEMS = 5 , 
    SELL_100_ITEMS = 6 , 
    PUT_1_ITEM = 6 , 
    PUT_10_ITEMS = 7 , 
    PUT_100_ITEMS = 8 , 
    TAKE_1_ITEM = 9 , 
    TAKE_10_ITEMS = 10 , 
    TAKE_100_ITEMS = 11
  };


//--------------------
// some constants intended to make the map tiles numbers somewhat
// more intuitive for case-separation and such things...
//
enum
{
  FLOOR = 0,
  CORNER_LD,        T_D,          CORNER_RD,       T_L,          KREUZ,       T_R,    CORNER_LU,  T_U,    CORNER_RU,
  H_WALL=10,     V_WALL,         ALERT,        
  BLOCK1,       BLOCK2,      BLOCK3, BLOCK4,  BLOCK5,
  H_SHUT_DOOR=18,  H_HALF_DOOR1, H_HALF_DOOR2, H_HALF_DOOR3, H_OPEN_DOOR,
  CONSOLE_L=23,  CONSOLE_R,    CONSOLE_U,    CONSOLE_D,
  V_SHUT_DOOR=27,  V_HALF_DOOR1, V_HALF_DOOR2, V_HALF_DOOR3, V_OPEN_DOOR,
  LIFT=32,       VOID,         
  REFRESH1,     REFRESH2,     REFRESH3,    REFRESH4,
  TELE_1=38,     TELE_2,       TELE_3,       TELE_4,       INVISIBLE_BRICK,
  LOCKED_H_SHUT_DOOR=43,         LOCKED_V_SHUT_DOOR,
  CODEPANEL_L=45,   CODEPANEL_R, CODEPANEL_U, CODEPANEL_D,
  BOX_1=49,      BOX_2,        BOX_3,        BOX_4,        UNUSED_BRICK,
  CONVEY_L=54,   CONVEY_D,     CONVEY_R,     CONVEY_U,     FINE_GRID,
  AUTOGUN_R=59,   AUTOGUN_D,     AUTOGUN_L,     AUTOGUN_U,
  ENHANCER_RU=63,   ENHANCER_LU,     ENHANCER_RD,     ENHANCER_LD,
  CONSUMER_1=67,     CONSUMER_2,     CONSUMER_3,    CONSUMER_4,
  CHEST_U=71,   CHEST_L,     CHEST_D,     CHEST_R,
  FLOOR_CARPET_L=75,  FLOOR_CARPET_R, FLOOR_CARPET_U, FLOOR_CARPET_D, FLOOR_CARPET,
  CAVE_FLOOR=80, CAVE_V_WALL,     CAVE_H_WALL,       
  CAVE_CORNER_LU=83,        CAVE_CORNER_RU,       CAVE_CORNER_LD,      CAVE_CORNER_RD,
  FLOOR_CAVE_L=87,  FLOOR_CAVE_R, FLOOR_CAVE_U, FLOOR_CAVE_D,
  CAVE_WITH_WAY_TILE=91,
  CAVE_WITH_GRASS_1=92,   CAVE_WITH_GRASS_2,   CAVE_WITH_GRASS_3,   CAVE_WITH_GRASS_4,
  IDENTIFY_L,   IDENTIFY_R,     IDENTIFY_U,     IDENTIFY_D, 
  LAST_BLOCK_IN_LIST_DUMMY_ENTRY

};

enum
{
  ISO_ERROR_BRICK = 0 ,
  ISO_V_WALL = 1 ,
  ISO_H_WALL = 2 ,

  ISO_CRUDE_BLOCK = 4 ,

  ISO_H_DOOR_000_OPEN = 6,
  ISO_H_DOOR_025_OPEN = 7,
  ISO_H_DOOR_050_OPEN = 8,
  ISO_H_DOOR_075_OPEN = 9,
  ISO_H_DOOR_100_OPEN = 10,

  ISO_V_DOOR_000_OPEN = 11,
  ISO_V_DOOR_025_OPEN = 12,
  ISO_V_DOOR_050_OPEN = 13,
  ISO_V_DOOR_075_OPEN = 14,
  ISO_V_DOOR_100_OPEN = 15,

  ISO_TELEPORTER_1 = 16,
  ISO_TELEPORTER_2 = 17,
  ISO_TELEPORTER_3 = 18,
  ISO_TELEPORTER_4 = 19,
  ISO_TELEPORTER_5 = 20,

  ISO_REFRESH_1 = 21,
  ISO_REFRESH_2 = 22,
  ISO_REFRESH_3 = 23,
  ISO_REFRESH_4 = 24,
  ISO_REFRESH_5 = 25,

  ISO_H_DOOR_LOCKED = 26,
  ISO_V_DOOR_LOCKED = 27,

  ISO_H_CHEST_CLOSED = 28,
  ISO_V_CHEST_CLOSED = 29,
  ISO_H_CHEST_OPEN = 30,
  ISO_V_CHEST_OPEN = 31,

  ISO_AUTOGUN_W = 32,
  ISO_AUTOGUN_N = 33,
  ISO_AUTOGUN_E = 34,
  ISO_AUTOGUN_S = 35,

  ISO_CAVE_WALL_H = 36,
  ISO_CAVE_WALL_V = 37,

  ISO_CAVE_CORNER_NE = 38,
  ISO_CAVE_CORNER_SE = 39,
  ISO_CAVE_CORNER_NW = 40,
  ISO_CAVE_CORNER_SW = 41,

  ISO_COOKING_POT = 42,

  ISO_CONSOLE_S = 43,
  ISO_CONSOLE_E = 44,
  ISO_CONSOLE_N = 45,
  ISO_CONSOLE_W = 46,

  ISO_BLOCK_1 = 47 ,
  ISO_BLOCK_2 = 48 ,
  ISO_BLOCK_3 = 49 ,

  ISO_BARREL_1 = 50 ,
  ISO_BARREL_2 = 51 ,
  ISO_BARREL_3 = 52 ,
  ISO_BARREL_4 = 53 ,

  ISO_ALERT = 54 ,

  ISO_ENHANCER_RU = 57 ,
  ISO_ENHANCER_LU = 56 ,
  ISO_ENHANCER_RD = 58 ,
  ISO_ENHANCER_LD = 55 

};

enum
{
  ISO_FLOOR_ERROR_TILE = 0 ,
  ISO_FLOOR_STONE_FLOOR = 1 ,
  ISO_FLOOR_STONE_FLOOR_WITH_DOT = 2 , 
  ISO_FLOOR_STONE_FLOOR_WITH_GRATE = 3 ,
  ISO_FLOOR_SAND = 4 ,
  ISO_FLOOR_WATER = 5 ,
  ISO_FLOOR_SAND_WITH_GRASS_1 = 6 ,
  ISO_FLOOR_SAND_WITH_GRASS_2 = 7 ,
  ISO_FLOOR_SAND_WITH_GRASS_3 = 8 ,
  ISO_FLOOR_SAND_WITH_GRASS_4 = 9 ,
  ISO_FLOOR_SAND_WITH_DOT = 10 ,
  ISO_FLOOR_PARQUET = 11 ,
  ALL_ISOMETRIC_FLOOR_TILES = 12 // this is always the last entry.  It's used to count the number of floor tiles.
};




#define BLUE_BLOCKS_OFFSET 100
#define GREEN_BLOCKS_OFFSET 200

// #define NUM_MAP_BLOCKS		80  // total number of map-blocks 
// #define NUM_MAP_BLOCKS (3*LAST_BLOCK_IN_LIST_DUMMY_ENTRY) // total number of map-blocks 
#define NUM_MAP_BLOCKS (1*LAST_BLOCK_IN_LIST_DUMMY_ENTRY) // total number of map-blocks 


#endif
