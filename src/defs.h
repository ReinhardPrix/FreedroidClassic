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
#define FREEDROID_INTERNAL_ERROR_SIGNAL -7890

#ifndef max
#define max(x,y) ((x) < (y) ? (y) : (x) ) 
#endif
#ifndef min
#define min(x,y) ((x) > (y) ? (y) : (x) ) 
#endif

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

enum
  {
    PART_GROUP_HEAD = 0 ,
    PART_GROUP_SHIELD = 1 , 
    PART_GROUP_TORSO = 2 ,
    PART_GROUP_FEET = 3 ,
    PART_GROUP_WEAPON = 4 ,
    PART_GROUP_WEAPONARM = 5 ,
    ALL_PART_GROUPS = 6
  };
enum
  {
    TUX_IMAGE_UPDATE_EVERYTHING_AT_ONCE = -133 ,
    TUX_IMAGE_UPDATE_CONTINUOUSLY = -134 
  };
#define NOT_LOADED_MARKER "nothing_loaded"

#define ENEMY_ROTATION_MODELS_AVAILABLE 34
#define ROTATION_ANGLES_PER_ROTATION_MODEL 8

#define MAX_ENEMY_MOVEMENT_PHASES 50
#define WALK_ANIMATION 113
#define ATTACK_ANIMATION 114
#define GETHIT_ANIMATION 115
#define DEATH_ANIMATION 116
#define STAND_ANIMATION 117


#define LIGHT_RADIUS_CHUNK_SIZE (1.0/5.0)
#define LIGHT_RADIUS_CRUDENESS_FACTOR 1
#define NUMBER_OF_SHADOW_IMAGES 20


#define MAX_OBSTACLES_ON_MAP 3000
#define MAX_OBSTACLE_NAMES_PER_LEVEL 100
#define MAX_OBSTACLE_DESCRIPTIONS_PER_LEVEL 100

#define FLOOR_TILES_VISIBLE_AROUND_TUX 11
// #define MAX_OBSTACLES_GLUED_TO_ONE_MAP_TILE 4
#define MAX_OBSTACLES_GLUED_TO_ONE_MAP_TILE 5
#define MAX_ITEMS_PER_LEVEL 300
#define MAX_CHEST_ITEMS_PER_LEVEL (MAX_ITEMS_PER_LEVEL)
#define MAX_MAP_LABELS_PER_LEVEL 100
#define MAX_ITEMS_IN_INVENTORY 100
#define INVENTORY_GRID_WIDTH 10
#define INVENTORY_GRID_HEIGHT 6
#define INV_SUBSQUARE_WIDTH (31)
#define INV_SUBSQUARE_HEIGHT 32
#define NUMBER_OF_ITEM_PICTURES 70
#define NUMBER_OF_MOUSE_CURSOR_PICTURES 2

#define ITEM_DROP_PERCENTAGE 25
#define GOLD_DROP_PERCENTAGE 15

#define MAX_DIALOGUE_OPTIONS_IN_ROSTER 100
#define MAX_REPLIES_PER_OPTION 100
#define MAX_SUBTITLES_N_SAMPLES_PER_DIALOGUE_OPTION 20
#define MAX_EXTRAS_PER_OPTION 10
#define CHAT_DEBUG_LEVEL 1

#define MAX_BIG_SCREEN_MESSAGES 10


#ifndef _itemdefs_h
#include "itemdefs.h"
#endif

#define WAR_BOT 1
#define SNIPER_BOT 2
#define MIND_BOT 3

#define NUMBER_OF_SKILL_LEVELS 10
#define NUMBER_OF_SKILL_PAGES 8
#define NUMBER_OF_SKILLS_PER_SKILL_PAGE 5
#define MAX_ACTIVE_SPELLS 100

enum skill_forms
{
    PROGRAM_FORM_IMMEDIATE = 60000, //instant hit whatever behind cursor
    PROGRAM_FORM_RADIAL, //nova
    PROGRAM_FORM_BULLET, //fire a bullet
    PROGRAM_FORM_SELF, //instant hit player
};

enum _global_ingame_modes
{
    GLOBAL_INGAME_MODE_IDENTIFY = 7123 ,
    GLOBAL_INGAME_MODE_NORMAL = 7124 ,
    GLOBAL_INGAME_MODE_EXAMINE = 7125 ,
    GLOBAL_INGAME_MODE_LOOT = 7126 ,
    GLOBAL_INGAME_MODE_SCROLL_UP = 7127, 
    GLOBAL_INGAME_MODE_SCROLL_DOWN = 7128 ,
    GLOBAL_INGAME_MODE_REPAIR = 7129 ,
    GLOBAL_INGAME_MODE_UNLOCK = 7130 ,
    GLOBAL_INGAME_MODE_TALK = 7131 ,
    GLOBAL_INGAME_MODE_FIRST_AID = 7132 ,
    GLOBAL_INGAME_MODE_ATTACK = 7133 ,
    GLOBAL_INGAME_MODE_PICKPOCKET = 7134 ,
};

enum _game_config_screen_visible_parameters
{
    GAME_CONFIG_SCREEN_VISIBLE_INVENTORY = 8001 ,
    GAME_CONFIG_SCREEN_VISIBLE_SKILLS = 8002 ,
    GAME_CONFIG_SCREEN_VISIBLE_CHARACTER = 8003 ,
    GAME_CONFIG_SCREEN_VISIBLE_SKILL_EXPLANATION = 8004 ,
};

//--------------------
// This is old and should go out, but that must be done from at home, 
// cause here I can't check if the code is still running without it
//
#define FILENAME_LEN	5000

#define END_OF_OFFSET_FILE_STRING "** End of iso_image offset file **"
#define START_OF_OFFSET_FILE_STRING "** Start of iso_image offset file **"
#define OFFSET_FILE_OFFSETX_STRING "OffsetX="
#define OFFSET_FILE_OFFSETY_STRING "OffsetY="

#define JOY_MAX_VAL 32767     // maximal amplitude of joystick axis values

#define Set_Rect(rect, xx, yy, ww, hh) {\
(rect).x = (xx); (rect).y = (yy); (rect).w = (ww); (rect).h = (hh); }

#define Copy_Rect(src, dst) {\
(dst).x = (src).x; (dst).y = (src).y; (dst).w = (src).w; (dst).h = (src).h; }

#define UserCenter_x (User_Rect.x+User_Rect.w/2)
#define UserCenter_y (User_Rect.y+User_Rect.h/2)

#define MAX_CHARACTER_NAME_LENGTH (25)
#define MAX_LENGTH_FOR_DIALOG_SECTION_NAME 100
#define MAX_LENGTH_OF_SHORT_DESCRIPTION_STRING 200

// The flags for AssembleCombatWindow are:
enum
{ ONLY_SHOW_MAP = 1 , DO_SCREEN_UPDATE = 2 , ONLY_SHOW_MAP_AND_TEXT = 4 , SHOW_GRID = 8 , SHOW_TUX_AND_ENEMIES = 16 , SHOW_TEXT = 32 , SHOW_ITEMS = 64 , OMIT_OBSTACLES = 128 , OMIT_TUX = 256 , OMIT_ENEMIES = 512 , SHOW_OBSTACLE_NAMES = 1024 , ZOOM_OUT = 2048 , OMIT_BLASTS = 4096 , USE_OWN_MOUSE_CURSOR = 8192 , SKIP_LIGHT_RADIUS = (2*8192) };

//--------------------
// This controls the zoom factor for the automap.  Since this uses
// a different update policy than the level editor, even strong zoom
// will not be a problem here...
//
#define LEVEL_EDITOR_ZOOM_OUT_FACT 3.0

//--------------------
// This controls the zoom factor for the automap.  Since this uses
// a different update policy than the level editor, even strong zoom
// will not be a problem here...
//
// However some other considerations must be taken into account:  OPENGL
// DOES IN GENERAL NOT WORK WELL ANY MORE WITH TEXTURES LARGER THAN 1024
// AND SOMETIMES EVEN NOT WELL WITH ANYTHING > 256 (old vodoo cards!!!)
// EVEN GEFORCE II HAS PROBLEMS WITH 2048 TEXTURE SIZE.
//
// Therefore some sanity-factor is introduced here:  it will scale down
// the internal automap texture but scale it up again when the acutal
// part-transparent blit is being done.  Using 0.25 for the sanity factor
// should be the safest bet.  However quality is poor:  you get lots of
// rectangular blocks where a wall/door/tree would be. 
// So we're using larger textures, assuming that most people will have 
// decen graphics adapters. Vodoo users can recompile the game for their needs...
// 
#define AUTOMAP_ZOOM_OUT_FACT 8.0
#define AUTOMAP_SANITY_FACTOR 1
#define AUTOMAP_TEXTURE_WIDTH (2048*AUTOMAP_SANITY_FACTOR)
#define AUTOMAP_TEXTURE_HEIGHT (1024*AUTOMAP_SANITY_FACTOR)

#define LIGHT_RADIUS_STRETCH_TEXTURE_WIDTH 64
#define LIGHT_RADIUS_STRETCH_TEXTURE_HEIGHT 64

// The color filters are:
enum
{ FILTER_BLUE = 1 , FILTER_GREEN = 2 , FILTER_RED = 3 };


//--------------------
// Constants for Paths and names of Data-files
// the root "FD_DATADIR" should be defined in the Makefile as $(pkgdatadir)
// if not, we set it here:
//
#ifndef FD_DATADIR
#define FD_DATADIR ".."   // our local fallback
#endif

#define GRAPHICS_DIR		"graphics/"
#define SOUND_DIR		"sound/"
#define MAP_DIR			"map/"
#define TITLES_DIR		"map/titles/"
#define DIALOG_DIR		"dialogs/"

#define MOUSE_CURSOR_BLOCK_FILE "mouse_move_cursor.png"
#define SKILL_ICON_FILE         "skills.png"
#define SKILL_LEVEL_BUTTON_FILE "skill_buttons.png"

#define HS_BACKGROUND_FILE     "backgrounds/transfer.jpg"  

#define INVENTORY_SCREEN_BACKGROUND_CODE 0
#define CHARACTER_SCREEN_BACKGROUND_CODE 1
#define SKILL_SCREEN_BACKGROUND_CODE 2
#define SKILL_SCREEN_EXPLANATION_BACKGROUND_CODE 3
#define NE_TITLE_PIC_BACKGROUND_CODE 4
#define NE_CREDITS_PIC_BACKGROUND_CODE 5
#define SHOP_BACKGROUND_IMAGE_CODE 6
#define ITEM_BROWSER_BG_PIC_BACKGROUND_CODE 7
#define ITEM_BROWSER_SHOP_BACKGROUND_CODE 8
#define NE_CONSOLE_FG_1_PIC_CODE 9
#define NE_CONSOLE_FG_2_PIC_CODE 10
#define NE_CONSOLE_FG_3_PIC_CODE 11
#define NE_CONSOLE_FG_4_PIC_CODE 12
#define NE_CONSOLE_BACKGROUND_CODE 13
#define LEVEL_EDITOR_BANNER_CODE 14 
// #define LEVEL_EDITOR_BANNER_CODE2 15
// #define LEVEL_EDITOR_BANNER_CODE3 16 
// #define LEVEL_EDITOR_BANNER_CODE4 17 
// #define LEVEL_EDITOR_BANNER_CODE5 18 
// #define LEVEL_EDITOR_BANNER_CODE6 19 
// #define LEVEL_EDITOR_BANNER_CODE7 20 
#define FREEDROID_LOADING_PICTURE_CODE 21
#define MOUSE_BUTTON_CHA_BACKGROUND_PICTURE_CODE 22
#define MOUSE_BUTTON_INV_BACKGROUND_PICTURE_CODE 23
#define MOUSE_BUTTON_SKI_BACKGROUND_PICTURE_CODE 24
#define MOUSE_BUTTON_PLUS_BACKGROUND_PICTURE_CODE 25
#define CHAT_DIALOG_BACKGROUND_PICTURE_CODE 26
#define CHAT_DIALOG_BACKGROUND_EXCERPT_CODE 27
#define TAKEOVER_BACKGROUND_CODE 28
#define QUEST_BROWSER_BACKGROUND_CODE 29
#define NUMBER_SELECTOR_BACKGROUND_CODE 30
#define GAME_MESSAGE_WINDOW_BACKGROUND_CODE 31
#define HUD_BACKGROUND_CODE 32

#define NEW_MISSION             "ReturnOfTux.mission"
#define ICON_FILE		"paraicon.bmp"

//--------------------
// Constants about the size (and mem?) of pixmaps
//
#define SKILL_LEVEL_BUTTON_HEIGHT 32
#define SKILL_LEVEL_BUTTON_WIDTH 30

#define FIRST_MENU_ITEM_POS_X (64)
#define FIRST_MENU_ITEM_POS_XX ( GameConfig . screen_width - FIRST_MENU_ITEM_POS_X )
#define FIRST_MENU_ITEM_POS_Y (BANNER_HEIGHT + FontHeight(Menu_BFont) * 3 )

#define NUMBER_OF_ITEMS_ON_ONE_SCREEN 4
#define ITEM_MENU_DISTANCE 80
#define ITEM_FIRST_POS_Y 130

#define INITIAL_BLOCK_WIDTH	64
#define INITIAL_BLOCK_HEIGHT	64
#define BLOCKMEM  		INITIAL_BLOCK_WIDTH*INITIAL_BLOCK_HEIGHT
#define MAX_AMOUNT_OF_TEXTURES_WE_WILL_USE 10000

#define SCROLLSTARTX		User_Rect.x
#define SCROLLSTARTY		GameConfig . screen_height

#define INITIAL_DIGIT_LENGTH (8*2)
#define INITIAL_DIGIT_HEIGHT (9*2)
#define INITIAL_DIGIT_POS_X 5*2
#define INITIAL_DIGIT_POS_Y 12*2
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
#define TEXT_STRETCH (6.0/5.0)

#define BACKGROUND_TEXT_RECT_ALPHA 110

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

#define TAKEOVER_BACKGROUND_MUSIC_SOUND "TheBeginning.ogg"
#define CONSOLE_BACKGROUND_MUSIC_SOUND "TheBeginning.ogg"
#define CREDITS_BACKGROUND_MUSIC_SOUND "TheBeginning.ogg"
#define MENU_BACKGROUND_MUSIC_SOUND "menu.ogg"
#define BIGFIGHT_BACKGROUND_MUSIC_SOUND "hellforce.ogg"
#define SILENCE (NULL)
enum _sounds
{
  ERRORSOUND = 0,
};

//--------------------
// The sounds when the influencers energy is low or when he is in transfer mode
// occur periodically.  These constants specify which intervals are to be used
// for these periodic happenings...
#define CRY_SOUND_INTERVAL 2
#define TRANSFER_SOUND_INTERVAL 1.1

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
    PERSON_STANDARD_BOT_AFTER_TAKEOVER,
    PERSON_BRUCE,
    PERSON_SUBDIALOG_DUMMY,
    PERSON_TYBALT,
    PERSON_EWALD,
    PERSON_KEVINS_GUARD,
    PERSON_KEVIN,
    PERSON_LUKAS,
    PERSON_JASMINE,
    PERSON_SADD,
    LAST_PERSON
  };
#define MAX_PERSONS             (LAST_PERSON)

#define MAX_ANSWERS_PER_PERSON 100
#define END_ANSWER (MAX_ANSWERS_PER_PERSON-1)

enum
  { 
    LOG_SCREEN_TOGGLE_BUTTON, 
    CHA_SCREEN_TOGGLE_BUTTON, 
    INV_SCREEN_TOGGLE_BUTTON,
    SKI_SCREEN_TOGGLE_BUTTON, 
    CHA_SCREEN_TOGGLE_BUTTON_RED,
    LOG_SCREEN_TOGGLE_BUTTON_YELLOW, 
    CHA_SCREEN_TOGGLE_BUTTON_YELLOW, 
    INV_SCREEN_TOGGLE_BUTTON_YELLOW,
    SKI_SCREEN_TOGGLE_BUTTON_YELLOW, 
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
    SELL_BUTTON,
    TAKE_BUTTON,
    PUT_BUTTON,
    REPAIR_BUTTON,
    IDENTIFY_BUTTON,

    OPEN_CLOSE_SKILL_EXPLANATION_BUTTON,

    GO_LEVEL_NORTH_BUTTON,
    GO_LEVEL_SOUTH_BUTTON,
    GO_LEVEL_EAST_BUTTON,
    GO_LEVEL_WEST_BUTTON,
    EXPORT_THIS_LEVEL_BUTTON,
    LEVEL_EDITOR_SAVE_SHIP_BUTTON,
    LEVEL_EDITOR_TOGGLE_WAYPOINT_BUTTON,
    LEVEL_EDITOR_TOGGLE_CONNECTION_BLUE_BUTTON,
    LEVEL_EDITOR_TOGGLE_CONNECTION_RED_BUTTON,

    LEVEL_EDITOR_DELETE_OBSTACLE_BUTTON,
    LEVEL_EDITOR_NEXT_OBSTACLE_BUTTON,

    LEVEL_EDITOR_BEAUTIFY_GRASS_BUTTON,
    LEVEL_EDITOR_ZOOM_IN_BUTTON,
    LEVEL_EDITOR_ZOOM_OUT_BUTTON,
    LEVEL_EDITOR_RECURSIVE_FILL_BUTTON,
    LEVEL_EDITOR_NEW_OBSTACLE_LABEL_BUTTON,
    LEVEL_EDITOR_NEW_OBSTACLE_DESCRIPTION_BUTTON,
    LEVEL_EDITOR_NEW_MAP_LABEL_BUTTON,
    LEVEL_EDITOR_NEW_ITEM_BUTTON,
    LEVEL_EDITOR_ESC_BUTTON,
    LEVEL_EDITOR_LEVEL_RESIZE_BUTTON,
    LEVEL_EDITOR_KEYMAP_BUTTON,
    LEVEL_EDITOR_QUIT_BUTTON,

    LEVEL_EDITOR_UNDERGROUND_LIGHT_ON_BUTTON,
    LEVEL_EDITOR_UNDERGROUND_LIGHT_OFF_BUTTON,

    LEVEL_EDITOR_TOGGLE_TUX_BUTTON,
    LEVEL_EDITOR_TOGGLE_TUX_BUTTON_OFF,
    LEVEL_EDITOR_TOGGLE_ENEMIES_BUTTON,
    LEVEL_EDITOR_TOGGLE_ENEMIES_BUTTON_OFF,
    LEVEL_EDITOR_TOGGLE_OBSTACLES_BUTTON,
    LEVEL_EDITOR_TOGGLE_OBSTACLES_BUTTON_OFF,
    LEVEL_EDITOR_TOGGLE_TOOLTIPS_BUTTON,
    LEVEL_EDITOR_TOGGLE_TOOLTIPS_BUTTON_OFF,
    LEVEL_EDITOR_TOGGLE_COLLISION_RECTS_BUTTON ,
    LEVEL_EDITOR_TOGGLE_COLLISION_RECTS_BUTTON_OFF,
    LEVEL_EDITOR_TOGGLE_GRID_BUTTON,
    LEVEL_EDITOR_TOGGLE_GRID_BUTTON_FULL,
    LEVEL_EDITOR_TOGGLE_GRID_BUTTON_OFF,

    LEVEL_EDITOR_NEXT_ITEM_GROUP_BUTTON,
    LEVEL_EDITOR_PREV_ITEM_GROUP_BUTTON,
    LEVEL_EDITOR_NEXT_PREFIX_BUTTON,
    LEVEL_EDITOR_PREV_PREFIX_BUTTON,
    LEVEL_EDITOR_NEXT_SUFFIX_BUTTON,
    LEVEL_EDITOR_PREV_SUFFIX_BUTTON,
    LEVEL_EDITOR_CANCEL_ITEM_DROP_BUTTON,

    SAVE_GAME_BANNER,
    LOAD_GAME_BANNER,

    WEAPON_RECT_BUTTON,
    DRIVE_RECT_BUTTON,
    SHIELD_RECT_BUTTON,
    AUX1_RECT_BUTTON,
    AUX2_RECT_BUTTON,
    HELMET_RECT_BUTTON,
    ARMOUR_RECT_BUTTON,

    SCROLL_DIALOG_MENU_UP_BUTTON,
    SCROLL_DIALOG_MENU_DOWN_BUTTON,
    
    MORE_STR_BUTTON,
    MORE_MAG_BUTTON,
    MORE_DEX_BUTTON,
    MORE_VIT_BUTTON,

    SCROLL_TEXT_UP_BUTTON,
    SCROLL_TEXT_DOWN_BUTTON,

    DESCRIPTION_WINDOW_UP_BUTTON , 
    DESCRIPTION_WINDOW_DOWN_BUTTON , 

    DRUID_SHOW_EXIT_BUTTON , 

    CHAT_PROTOCOL_SCROLL_UP_BUTTON,
    CHAT_PROTOCOL_SCROLL_DOWN_BUTTON,
    CHAT_PROTOCOL_SCROLL_OFF_BUTTON,
    CHAT_PROTOCOL_SCROLL_OFF2_BUTTON,

    LEVEL_EDITOR_FLOOR_TAB,
    LEVEL_EDITOR_WALLS_TAB,
    LEVEL_EDITOR_MACHINERY_TAB,
    LEVEL_EDITOR_FURNITURE_TAB,
    LEVEL_EDITOR_CONTAINERS_TAB,
    LEVEL_EDITOR_PLANTS_TAB,
    LEVEL_EDITOR_ALL_TAB,
    LEVEL_EDITOR_QUICK_TAB,

    QUEST_BROWSER_EXIT_BUTTON,
    QUEST_BROWSER_OPEN_QUESTS_BUTTON,
    QUEST_BROWSER_OPEN_QUESTS_OFF_BUTTON,
    QUEST_BROWSER_DONE_QUESTS_BUTTON,
    QUEST_BROWSER_DONE_QUESTS_OFF_BUTTON,
    QUEST_BROWSER_NOTES_BUTTON,
    QUEST_BROWSER_NOTES_OFF_BUTTON,
    QUEST_BROWSER_SCROLL_UP_BUTTON,
    QUEST_BROWSER_SCROLL_DOWN_BUTTON,
    QUEST_BROWSER_ITEM_SHORT_BUTTON,
    QUEST_BROWSER_ITEM_LONG_BUTTON,

    TAKEOVER_HELP_BUTTON,
	
    WEAPON_MODE_BUTTON, 
    SKI_ICON_BUTTON,

    //--------------------
    // Please leave this here as the last entry, since it conveniently and
    // automatically counts the number of buttons defined.  Other buttons
    // can be inserted above.
    //
    MAX_MOUSE_PRESS_BUTTONS
  };

#define CHAT_SUBDIALOG_WINDOW_X ((260)*GameConfig . screen_width/640)
#define CHAT_SUBDIALOG_WINDOW_Y ((42)*GameConfig . screen_height/480)
#define CHAT_SUBDIALOG_WINDOW_W ((337)*GameConfig . screen_width/640)
#define CHAT_SUBDIALOG_WINDOW_H ((216)*GameConfig . screen_height/480)

#define STR_BASE_X 100
#define STR_NOW_X 148
#define STR_Y 143
#define DEX_Y 171
#define MAG_Y 197
#define VIT_Y 227
#define POINTS_Y 252

#define BUTTON_MOD_X (-6)
#define BUTTON_MOD_Y (-4)
#define BUTTON_WIDTH 35
#define BUTTON_HEIGHT 19

#define CHARACTERRECT_X (GameConfig . screen_width-320)
#define CHARACTERRECT_W (320)
#define CHARACTERRECT_H (480)

#define EL_BLOCK_LEN		8
#define EL_BLOCK_HEIGHT		8
#define EL_BLOCK_MEM 		EL_BLOCK_LEN * EL_BLOCK_HEIGHT

#define BULLETSPEEDINFLUENCE 	2

#define DROID_PHASES            8
#define DEAD_DROID_PHASES       1

#define TUX_SWING_PHASES 	14
#define TUX_BREATHE_PHASES 	1
#define TUX_GOT_HIT_PHASES      0
#define TUX_WALK_CYCLE_PHASES   10
#define TUX_RUN_CYCLE_PHASES   10
#define TUX_TOTAL_PHASES (TUX_SWING_PHASES+TUX_BREATHE_PHASES+TUX_GOT_HIT_PHASES+TUX_WALK_CYCLE_PHASES+TUX_RUN_CYCLE_PHASES)

#define TUX_RUNNING_SPEED (7.1)
#define TUX_WALKING_SPEED (2.5)

#define TUX_MODELS              13
#define MAX_TUX_DIRECTIONS      16
#define RADIAL_SPELL_DIRECTIONS 16
#define BULLET_DIRECTIONS       16

#define WAIT_AFTER_KILLED	3.0  // time to wait and still display pictures after the destruction of 
                                     // the players droid.  This is now measured in seconds and can be a float
#define WAIT_AFTER_GAME_WON	5.0  // how long to show tux after the game is won
#define WAIT_COLLISION		2.5 // after a little collision with Tux or another enemy, hold position for a while
                                    // this variable describes the amount of time in SECONDS 
#define FLASH_DURATION_IN_SECONDS 0.1

#define ALLBLASTTYPES		3	/* number of different exposions */

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
#define MAX_CLEARANCES 100
#define MAX_PASSWORDS 100
#define MAX_PASSWORD_LENGTH 20
#define MAX_COOKIES 100
#define MAX_COOKIE_LENGTH 200
#define MAX_INTERMEDIATE_WAYPOINTS_FOR_TUX 100

#define RIGHT_WALL_BIT 1
#define LEFT_WALL_BIT 2
#define UP_WALL_BIT 4
#define DOWN_WALL_BIT 8
#define SQUARE_SEEN_AT_ALL_BIT 16

#define MAX_SAVED_CHARACTERS_ON_DISK 7

#define MAX_ENEMYS_ON_SHIP	1000
#define MAX_INFLU_POSITION_HISTORY 10000
#define MAX_MISSIONS_IN_GAME 10         // how many missions can there be at most in Freedroid
#define MAX_MISSION_TRIGGERED_ACTIONS 10
#define MAX_MISSION_DESCRIPTION_TEXTS 10

#define MAX_LEVELS		39   	// how many map levels are allowed in one ship

#define MAX_EVENT_TRIGGERS      100   // how many event triggers at most to allow
#define MAX_TRIGGERED_ACTIONS_IN_GAME   100   // how many triggerable actions to allow at most

#define MAXWAYPOINTS 		200
#define MAX_DOORS_ON_LEVEL 	120
#define MAX_AUTOGUNS_ON_LEVEL 	60
#define MAX_REFRESHES_ON_LEVEL	39
#define MAX_TELEPORTERS_ON_LEVEL 20

#define MAX_PHASES_IN_A_BULLET 12
#define PHASES_OF_EACH_BLAST 20
#define MAX_STEPS_IN_GIVEN_COURSE 100

#undef USE_MISS_HIT_ARRAYS
#define UNCHECKED 0
#define HIT 1
#define MISS 2

#define ITEM_INVENTORY_IMAGE_FILE_NAME "File or directory name for inventory image=\""
#define ITEM_DROP_SOUND_FILE_NAME "Item uses drop sound with filename=\""

#define UNIVERSAL_COORD_W(W) (int)((float)W * ((float)(GameConfig . screen_width) / 640.0))
#define UNIVERSAL_COORD_H(H) (int)((float)H * ((float)(GameConfig . screen_height) / 480.0))


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
    INFOUT=-30,
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

enum _busytype
{
    NONE=0,
    DRINKING_POTION=1,
    WEAPON_FIREWAIT,
    WEAPON_RELOAD,
    THROWING_GRENADE,
    RUNNING_PROGRAM,
};
//--------------------
// The possible edito modes for the level editor
//
enum _level_editor_edit_modes
{
    LEVEL_EDITOR_SELECTION_FLOOR = 0,
    LEVEL_EDITOR_SELECTION_WALLS = 1,
    LEVEL_EDITOR_SELECTION_MACHINERY = 2,
    LEVEL_EDITOR_SELECTION_FURNITURE = 3,
    LEVEL_EDITOR_SELECTION_CONTAINERS = 4,
    LEVEL_EDITOR_SELECTION_PLANTS = 5,
    LEVEL_EDITOR_SELECTION_ALL = 6,
    NUMBER_OF_LEVEL_EDITOR_GROUPS = 7
};

enum _enemy_combat_state_machine_states
{
    MOVE_ALONG_RANDOM_WAYPOINTS = -301 , 
    STOP_AND_EYE_TUX = -302 ,
    MAKE_ATTACK_RUN = -303 , 
    HUNTING_TUX = -304 , 
    SEEK_BETTER_POSITION = -305 ,
    FIGHT_ON_TUX_SIDE = -306 ,
    RELENTLESS_FIRE_TO_GIVEN_POSITION = -307 ,
    TURN_THOWARDS_NEXT_WAYPOINT = -308 ,
    RUSH_TUX_ON_SIGHT_AND_OPEN_TALK = -309 ,
    WAIT_AND_TURN_AROUND_AIMLESSLY = -310 ,
    WAYPOINTLESS_WANDERING = -311 ,
    TURN_THOWARDS_WAYPOINTLESS_SPOT = -312 
    // BACK_OFF_AFTER_GETTING_HIT, 
};
enum _enemy_combat_target_codes
{
    ATTACK_TARGET_IS_ENEMY = -201,
    ATTACK_TARGET_IS_PLAYER = -202,
    ATTACK_TARGET_IS_NOTHING = -203
};
enum
{
    MOUSE_CURSOR_ARROW_SHAPE = -402
}; 
enum
{
    TRANSPARENCY_FOR_WALLS = -501 ,
    TRANSPARENCY_FOR_SEE_THROUGH_OBJECTS = -502
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

//--------------------
// It is possible, that the Tux must fist move somewhere AND
// then open a chest or move somewhere AND then pick something up
// or the like.  These are called 'combo_actions' and some definition
// has to be made about the type of current combo_action.
//
enum _combo_action_types
{
    NO_COMBO_ACTION_SET = -10 ,
    COMBO_ACTION_OPEN_CHEST = 15 ,
    COMBO_ACTION_PICK_UP_ITEM = 16 ,
    COMBO_ACTION_SMASH_BARREL = 17 ,
    COMBO_ACTION_LOOT_OBSTACLE = 18 ,
};

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
    TAKE_100_ITEMS = 11 ,
    REPAIR_ITEM = 12 , 
    IDENTIFY_ITEM = 13 
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
    LIFT=32,       FDVOID,         
    REFRESH1,     REFRESH2,     REFRESH3,    REFRESH4,
    TELE_1=38,     TELE_2,       TELE_3,       TELE_4,       INVISIBLE_BRICK,
    LOCKED_H_SHUT_DOOR=43,         LOCKED_V_SHUT_DOOR,
    OLD_CODEPANEL_L=45,   OLD_CODEPANEL_R, OLD_CODEPANEL_U, OLD_CODEPANEL_D,
    BOX_1=49,      BOX_2,        BOX_3,        BOX_4,        UNUSED_BRICK,
    OLD_CONVEY_L=54,   OLD_CONVEY_D,     OLD_CONVEY_R,     OLD_CONVEY_U,     FINE_GRID,
    AUTOGUN_R=59,   AUTOGUN_D,     AUTOGUN_L,     AUTOGUN_U
};

enum
{

  ISO_TREE_4 = 0 ,

  //--------------------
  // These obstacles originate from wall_tiles.blend.
  //
  ISO_V_WALL = 1 ,
  ISO_H_WALL = 2 ,
  ISO_V_WALL_WITH_DOT = 3 ,
  ISO_H_WALL_WITH_DOT = 4 ,

  ISO_TREE_5 = 5 ,

  //--------------------
  // These obstacles originate from door_tiles.blend.
  //
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

  //--------------------
  // These obstacles are generated from 'iso_refresh_and_teleport.blend' file
  // in the cvs rep.
  //
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

  //--------------------
  // These obstacles currently don't have a blender file in the cvs tree,
  // but their meaning should be fairly obvious:
  //
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
//see at the end for disabled autoguns
  //--------------------
  // These obstacles are generated from 'iso_cave_walls.blend' file
  // in the cvs rep.
  //
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

  //--------------------
  // These obstacles are generated from 'iso_pillars_and_barrels.blend' file
  // in the cvs rep.
  //
  ISO_BLOCK_1 = 47 ,
  ISO_BLOCK_2 = 48 ,
  ISO_TV_PILLAR_W = 49 ,

  ISO_BARREL_1 = 50 ,
  ISO_BARREL_2 = 51 ,
  ISO_BARREL_3 = 52 ,
  ISO_BARREL_4 = 53 ,

  ISO_LAMP_S ,

  ISO_ENHANCER_RU = 57 ,
  ISO_ENHANCER_LU = 56 ,
  ISO_ENHANCER_RD = 58 ,
  ISO_ENHANCER_LD = 55 ,

  //--------------------
  // These obstacles are generated from 'commercial_models_for_fences.blend' file
  // which is currently not in the cvs rep. for licensing issues, but I'll send a
  // version to any Freedroid co-worker involved with the graphics
  //
  ISO_V_WOOD_FENCE = 59 ,
  ISO_H_WOOD_FENCE = 60 , 
  ISO_V_DENSE_FENCE = 61 ,
  ISO_H_DENSE_FENCE = 62 ,
  ISO_V_MESH_FENCE = 63 ,
  ISO_H_MESH_FENCE = 64 , 
  ISO_V_WIRE_FENCE = 65 ,
  ISO_H_WIRE_FENCE = 66 ,

  //--------------------
  // These obstacles are generated from 'commercial_models_for_furniture.blend' file
  // which is currently not in the cvs rep. for licensing issues, but I'll send a
  // version to any Freedroid co-worker involved with the graphics
  //
  ISO_N_TOILET_SMALL = 67 ,
  ISO_E_TOILET_SMALL ,
  ISO_S_TOILET_SMALL ,
  ISO_W_TOILET_SMALL ,
  ISO_N_TOILET_BIG ,
  ISO_E_TOILET_BIG ,
  ISO_S_TOILET_BIG ,
  ISO_W_TOILET_BIG ,
  ISO_N_CHAIR ,
  ISO_E_CHAIR ,
  ISO_S_CHAIR ,
  ISO_W_CHAIR ,
  ISO_N_DESK ,
  ISO_E_DESK ,
  ISO_S_DESK ,
  ISO_W_DESK ,
  ISO_N_SCHOOL_CHAIR ,
  ISO_E_SCHOOL_CHAIR ,
  ISO_S_SCHOOL_CHAIR ,
  ISO_W_SCHOOL_CHAIR ,

  //--------------------
  // These obstacles are generated from 'commercial_models_for_furniture2.blend' file
  // which is currently not in the cvs rep. for licensing issues, but I'll send a
  // version to any Freedroid co-worker involved with the graphics
  //
  ISO_N_BED = 87 ,
  ISO_E_BED ,
  ISO_S_BED ,
  ISO_W_BED ,
  
  // Except these ones which were done by Basse :)
  ISO_SHELF_FULL_V,
  ISO_SHELF_FULL_H,
  ISO_SHELF_EMPTY_V,
  ISO_SHELF_EMPTY_H,
  ISO_SHELF_SMALL_FULL_V,
  ISO_SHELF_SMALL_FULL_H,
  ISO_SHELF_SMALL_EMPTY_V,
  ISO_SHELF_SMALL_EMPTY_H,

  ISO_N_FULL_PARK_BENCH ,
  ISO_E_FULL_PARK_BENCH ,
  ISO_S_FULL_PARK_BENCH ,
  ISO_W_FULL_PARK_BENCH ,

  //--------------------
  // These obstacles are generated from 'commercial_models_for_furniture3.blend' file
  // which is currently not in the cvs rep. for licensing issues, but I'll send a
  // version to any Freedroid co-worker involved with the graphics
  //
  ISO_H_BATHTUB = 103 , 
  ISO_V_BATHTUB ,
  ISO_H_WASHTUB , 
  ISO_V_WASHTUB ,
  ISO_V_CURTAIN ,
  ISO_H_CURTAIN ,
  ISO_E_SOFA , 
  ISO_S_SOFA , 
  ISO_W_SOFA , 
  ISO_N_SOFA ,

  //--------------------
  // Here we insert a few trees generated with the LSystem python script.  wow!
  //
  ISO_TREE_1 , // = 113  ,
  ISO_TREE_2 ,
  ISO_TREE_3 ,
  
  ISO_THICK_WALL_H , // = 116 ,
  ISO_THICK_WALL_V ,
  ISO_THICK_WALL_CORNER_NE ,
  ISO_THICK_WALL_CORNER_SE ,
  ISO_THICK_WALL_CORNER_NW ,
  ISO_THICK_WALL_CORNER_SW ,
  ISO_THICK_WALL_T_N ,
  ISO_THICK_WALL_T_E ,
  ISO_THICK_WALL_T_S ,
  ISO_THICK_WALL_T_W ,

  ISO_CAVE_WALL_END_W , // = 126 ,
  ISO_CAVE_WALL_END_N ,
  ISO_CAVE_WALL_END_E ,
  ISO_CAVE_WALL_END_S ,

  ISO_GREY_WALL_END_W , // = 130 ,
  ISO_GREY_WALL_END_N ,
  ISO_GREY_WALL_END_E ,
  ISO_GREY_WALL_END_S ,

  ISO_BRICK_WALL_H , // = 134 ,
  ISO_BRICK_WALL_V ,
  ISO_BRICK_WALL_END ,

  ISO_BRICK_WALL_CORNER_1 , // = 137 , 
  ISO_BRICK_WALL_CORNER_2 , 
  ISO_BRICK_WALL_CORNER_3 , 
  ISO_BRICK_WALL_CORNER_4 ,

  ISO_BLOOD_1 , // = 141 ,
  ISO_BLOOD_2 ,
  ISO_BLOOD_3 ,
  ISO_BLOOD_4 ,
  ISO_BLOOD_5 ,
  ISO_BLOOD_6 ,
  ISO_BLOOD_7 ,
  ISO_BLOOD_8 ,

  ISO_EXIT_1 , // = 149 ,
  ISO_EXIT_2 ,

  ISO_ROCKS_N_PLANTS_1 , // = 151 ,
  ISO_ROCKS_N_PLANTS_2 ,
  ISO_ROCKS_N_PLANTS_3 ,
  ISO_ROCKS_N_PLANTS_4 ,

  ISO_ROOM_WALL_V_RED , // = 155 ,
  ISO_ROOM_WALL_H_RED ,
  ISO_ROOM_WALL_V_GREEN ,
  ISO_ROOM_WALL_H_GREEN ,

  ISO_SHOP_FURNITURE_1 , // = 159 ,
  ISO_SHOP_FURNITURE_2 ,
  ISO_SHOP_FURNITURE_3 ,
  ISO_SHOP_FURNITURE_4 ,
  ISO_SHOP_FURNITURE_5 ,
  ISO_SHOP_FURNITURE_6 ,

  ISO_OUTER_WALL_N1 , // = 165 ,
  ISO_OUTER_WALL_N2 ,
  ISO_OUTER_WALL_N3 ,
  ISO_OUTER_WALL_S1 ,
  ISO_OUTER_WALL_S2 ,
  ISO_OUTER_WALL_S3 ,
  ISO_OUTER_WALL_E1 ,
  ISO_OUTER_WALL_E2 ,
  ISO_OUTER_WALL_E3 ,
  ISO_OUTER_WALL_W1 ,
  ISO_OUTER_WALL_W2 ,
  ISO_OUTER_WALL_W3 ,
  
  ISO_OUTER_WALL_CORNER_1 , // = 177 ,
  ISO_OUTER_WALL_CORNER_2 ,
  ISO_OUTER_WALL_CORNER_3 ,
  ISO_OUTER_WALL_CORNER_4 ,
 
  ISO_OUTER_DOOR_V_00  , // = 181 ,
  ISO_OUTER_DOOR_V_25  ,
  ISO_OUTER_DOOR_V_50  ,
  ISO_OUTER_DOOR_V_75  ,
  ISO_OUTER_DOOR_V_100 ,
  
  ISO_OUTER_DOOR_H_00  ,
  ISO_OUTER_DOOR_H_25  ,
  ISO_OUTER_DOOR_H_50  ,
  ISO_OUTER_DOOR_H_75  ,
  ISO_OUTER_DOOR_H_100 ,

  ISO_OUTER_DOOR_V_LOCKED , // =191
  ISO_OUTER_DOOR_H_LOCKED ,
  
  ISO_TV_PILLAR_N , // =193,
  ISO_TV_PILLAR_E ,
  ISO_TV_PILLAR_S ,

  ISO_YELLOW_CHAIR_N , // = 196
  ISO_YELLOW_CHAIR_E ,
  ISO_YELLOW_CHAIR_S ,
  ISO_YELLOW_CHAIR_W ,
  ISO_RED_CHAIR_N , // = 200
  ISO_RED_CHAIR_E ,
  ISO_RED_CHAIR_S ,
  ISO_RED_CHAIR_W ,
 
  ISO_BODY_RED_GUARD_N, // 204
  ISO_BODY_RED_GUARD_E,
  ISO_BODY_RED_GUARD_S,
  ISO_BODY_RED_GUARD_W,
 
  ISO_CONFERENCE_TABLE_N, //208
  ISO_CONFERENCE_TABLE_E,
  ISO_CONFERENCE_TABLE_S,
  ISO_CONFERENCE_TABLE_W,

  ISO_RED_FENCE_V, //212
  ISO_RED_FENCE_H,
  ISO_BED_1,
  ISO_BED_2,
  ISO_BED_3,
  ISO_BED_4,
  ISO_BED_5,
  ISO_BED_6,
  ISO_BED_7,
  ISO_BED_8,
  ISO_PROJECTOR_E, // 222
  ISO_PROJECTOR_W,
  
  ISO_LAMP_E ,  
  ISO_LAMP_N ,  
  ISO_LAMP_W , 

  ISO_ROCKS_N_PLANTS_5 , //227
  ISO_ROCKS_N_PLANTS_6 ,
  ISO_ROCKS_N_PLANTS_7 ,
  ISO_ROCKS_N_PLANTS_8 ,

  ISO_BRICK_WALL_JUNCTION_1, //231
  ISO_BRICK_WALL_JUNCTION_2,
  ISO_BRICK_WALL_JUNCTION_3,
  ISO_BRICK_WALL_JUNCTION_4,
  ISO_BRICK_WALL_CRACKED_1,
  ISO_BRICK_WALL_CRACKED_2,
  ISO_BRICK_WALL_RUBBLE_1,
  ISO_BRICK_WALL_RUBBLE_2,  

  ISO_PROJECTOR_SCREEN_N, // 239
  ISO_PROJECTOR_SCREEN_E,
  ISO_PROJECTOR_SCREEN_S,
  ISO_PROJECTOR_SCREEN_W,

  ISO_PROJECTOR_N, // 243
  ISO_PROJECTOR_S,

  ISO_SIGN_1, //245
  ISO_SIGN_2,
  ISO_SIGN_3,

  ISO_LIGHT_GREEN_WALL_1, // 248
  ISO_LIGHT_GREEN_WALL_2,
  ISO_FUNKY_WALL_1,
  ISO_FUNKY_WALL_2,
  ISO_FUNKY_WALL_3,
  ISO_FUNKY_WALL_4,

  ISO_COUNTER_MIDDLE_1,
  ISO_COUNTER_MIDDLE_2,
  ISO_COUNTER_MIDDLE_3,
  ISO_COUNTER_MIDDLE_4,

  ISO_COUNTER_CORNER_ROUND_1,
  ISO_COUNTER_CORNER_ROUND_2,
  ISO_COUNTER_CORNER_ROUND_3,
  ISO_COUNTER_CORNER_ROUND_4,

  ISO_COUNTER_CORNER_SHARP_1,
  ISO_COUNTER_CORNER_SHARP_2,
  ISO_COUNTER_CORNER_SHARP_3,
  ISO_COUNTER_CORNER_SHARP_4,

  ISO_LIBRARY_FURNITURE_1, 
  ISO_LIBRARY_FURNITURE_2,

  ISO_3_BATHTUB , 
  ISO_4_BATHTUB ,
  ISO_BAR_TABLE,

  ISO_EXIT_3 ,
  ISO_EXIT_4 ,

  ISO_OUTER_WALL_SMALL_CORNER_1,
  ISO_OUTER_WALL_SMALL_CORNER_2,
  ISO_OUTER_WALL_SMALL_CORNER_3,
  ISO_OUTER_WALL_SMALL_CORNER_4,
  
  ISO_TABLE_OVAL_1,
  ISO_TABLE_OVAL_2,
  ISO_TABLE_GLASS_1,
  ISO_TABLE_GLASS_2,
  
  ISO_GLASS_WALL_1,
  ISO_GLASS_WALL_2,  
  
  ISO_CYAN_WALL_WINDOW_1,
  ISO_CYAN_WALL_WINDOW_2,
  ISO_RED_WALL_WINDOW_1,
  ISO_RED_WALL_WINDOW_2,
  ISO_FLOWER_WALL_WINDOW_1,
  ISO_FLOWER_WALL_WINDOW_2,
  ISO_FUNKY_WALL_WINDOW_1,
  ISO_FUNKY_WALL_WINDOW_2,

  ISO_RESTAURANT_SHELVES_1,
  ISO_RESTAURANT_SHELVES_2,
  ISO_RESTAURANT_SHELVES_3,
  ISO_RESTAURANT_SHELVES_4,
  ISO_RESTAURANT_SHELVES_5,
  ISO_RESTAURANT_SHELVES_6,
  ISO_RESTAURANT_SHELVES_7,
  ISO_RESTAURANT_SHELVES_8,
  ISO_RESTAURANT_SHELVES_9,
  ISO_RESTAURANT_SHELVES_10,
  
  ISO_SOFFA_1,
  ISO_SOFFA_2,
  ISO_SOFFA_3,
  ISO_SOFFA_4,
  ISO_SOFFA_CORNER_1,
  ISO_SOFFA_CORNER_2,
  ISO_SOFFA_CORNER_3,
  ISO_SOFFA_CORNER_4,
  ISO_SOFFA_CORNER_PLANT_1,
  ISO_SOFFA_CORNER_PLANT_2,
  ISO_SOFFA_CORNER_PLANT_3,
  ISO_SOFFA_CORNER_PLANT_4,
  
  ISO_OIL_STAINS_1,
  ISO_OIL_STAINS_2,
  ISO_OIL_STAINS_3,
  ISO_OIL_STAINS_4,
  ISO_OIL_STAINS_5,
  ISO_OIL_STAINS_6,
  ISO_OIL_STAINS_7,
  ISO_OIL_STAINS_8,
  
  ISO_TRANSP_FOR_WATER,
  
  ISO_BRICK_WALL_EH , // = 322 ,
  ISO_BRICK_WALL_EV ,

  ISO_DIS_AUTOGUN_W,
  ISO_DIS_AUTOGUN_N,
  ISO_DIS_AUTOGUN_E,
  ISO_DIS_AUTOGUN_S,

  ISO_BRICK_WALL_CABLES_H ,
  ISO_BRICK_WALL_CABLES_V ,

  ISO_BRICK_WALL_CABLES_CORNER_1 ,
  ISO_BRICK_WALL_CABLES_CORNER_2 , 
  ISO_BRICK_WALL_CABLES_CORNER_3 , 
  ISO_BRICK_WALL_CABLES_CORNER_4 ,

  NUMBER_OF_OBSTACLE_TYPES
};

enum
{
  ISO_FLOOR_ERROR_TILE = 0 ,
  ISO_FLOOR_STONE_FLOOR = 1 ,
  ISO_FLOOR_STONE_FLOOR_WITH_DOT = 2 , 
  ISO_FLOOR_STONE_FLOOR_WITH_GRATE = 3 ,
  ISO_FLOOR_SAND = 4 ,
  ISO_FLOOR_HOUSE_FLOOR = 5 ,

  ISO_FLOOR_SAND_WITH_GRASS_1 ,
  ISO_FLOOR_SAND_WITH_GRASS_2 ,
  ISO_FLOOR_SAND_WITH_GRASS_3 ,
  ISO_FLOOR_SAND_WITH_GRASS_4 ,
  ISO_FLOOR_SAND_WITH_GRASS_5 ,
  ISO_FLOOR_SAND_WITH_GRASS_6 ,
  ISO_FLOOR_SAND_WITH_GRASS_7 ,
  ISO_FLOOR_SAND_WITH_GRASS_8 ,
  ISO_FLOOR_SAND_WITH_GRASS_9 ,
  ISO_FLOOR_SAND_WITH_GRASS_10 ,

  ISO_FLOOR_SAND_WITH_GRASS_11 ,
  ISO_FLOOR_SAND_WITH_GRASS_12 ,
  ISO_FLOOR_SAND_WITH_GRASS_13 ,

  ISO_FLOOR_SAND_WITH_GRASS_14 ,
  ISO_FLOOR_SAND_WITH_GRASS_15 ,
  ISO_FLOOR_SAND_WITH_GRASS_16 ,
  ISO_FLOOR_SAND_WITH_GRASS_17 ,

  ISO_FLOOR_SAND_WITH_GRASS_18 ,
  ISO_FLOOR_SAND_WITH_GRASS_19 ,
  ISO_FLOOR_SAND_WITH_GRASS_20 ,
  ISO_FLOOR_SAND_WITH_GRASS_21 ,

  ISO_FLOOR_SAND_WITH_GRASS_22 ,
  ISO_FLOOR_SAND_WITH_GRASS_23 ,
  ISO_FLOOR_SAND_WITH_GRASS_24 ,

  ISO_WATER ,
  ISO_COMPLETELY_DARK,
  ISO_RED_WAREHOUSE_FLOOR,
  ISO_FLOOR_SAND_WITH_GRASS_25 ,
  ISO_FLOOR_SAND_WITH_GRASS_26 ,
  ISO_FLOOR_SAND_WITH_GRASS_27 ,
  ISO_FLOOR_SAND_WITH_GRASS_28 ,
  ISO_FLOOR_SAND_WITH_GRASS_29 ,
  
  ISO_MISCELLANEOUS_FLOOR_9 ,
  ISO_MISCELLANEOUS_FLOOR_10 ,
  ISO_MISCELLANEOUS_FLOOR_11 ,
  ISO_MISCELLANEOUS_FLOOR_12 ,
  ISO_MISCELLANEOUS_FLOOR_13 ,
  ISO_MISCELLANEOUS_FLOOR_14 ,
  ISO_MISCELLANEOUS_FLOOR_15 ,
  ISO_MISCELLANEOUS_FLOOR_16 ,
  ISO_MISCELLANEOUS_FLOOR_17 ,
  ISO_MISCELLANEOUS_FLOOR_18 ,
  ISO_MISCELLANEOUS_FLOOR_19 ,
  ISO_MISCELLANEOUS_FLOOR_20 ,
  ISO_MISCELLANEOUS_FLOOR_21 ,
  ISO_MISCELLANEOUS_FLOOR_22 ,

  ISO_SIDEWALK_1 ,
  ISO_SIDEWALK_2 ,
  ISO_SIDEWALK_3 ,
  ISO_SIDEWALK_4 ,
  ISO_SIDEWALK_5 ,
  ISO_SIDEWALK_6 ,
  ISO_SIDEWALK_7 ,
  ISO_SIDEWALK_8 ,
  ISO_SIDEWALK_9 ,
  ISO_SIDEWALK_10 ,
  ISO_SIDEWALK_11 ,
  ISO_SIDEWALK_12 ,
  ISO_SIDEWALK_13 ,
  ISO_SIDEWALK_14 ,
  ISO_SIDEWALK_15 ,
  ISO_SIDEWALK_16 ,
  ISO_SIDEWALK_17 ,
  ISO_SIDEWALK_18 ,
  ISO_SIDEWALK_19 ,
  ISO_SIDEWALK_20 ,

  ISO_MISCELLANEOUS_FLOOR_23 , 

  ISO_SAND_FLOOR_1,
  ISO_SAND_FLOOR_2,
  ISO_SAND_FLOOR_3,
  ISO_SAND_FLOOR_4,
  ISO_SAND_FLOOR_5,
  ISO_SAND_FLOOR_6,

  ALL_ISOMETRIC_FLOOR_TILES 
};


#endif
