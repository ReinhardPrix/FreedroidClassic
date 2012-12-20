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


#ifndef _global_h
#define _global_h

#include "system.h"
#include "BFont.h"
#include "takeover.h"

#undef EXTERN
#ifdef _main_c
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN char ConfigDir[255];

EXTERN SDL_Rect OrigBlock_Rect;
EXTERN SDL_Rect Block_Rect;
EXTERN SDL_Rect Screen_Rect;
EXTERN SDL_Rect User_Rect;
EXTERN SDL_Rect Classic_User_Rect;
EXTERN SDL_Rect Full_User_Rect;
EXTERN SDL_Rect Banner_Rect;
EXTERN SDL_Rect Portrait_Rect;
EXTERN SDL_Rect Cons_Droid_Rect;
EXTERN SDL_Rect Menu_Rect;
EXTERN SDL_Rect OptionsMenu_Rect;
EXTERN SDL_Rect OrigDigit_Rect;
EXTERN SDL_Rect Digit_Rect;
EXTERN SDL_Rect FirstDigit_Rect;
EXTERN SDL_Rect SecondDigit_Rect;
EXTERN SDL_Rect ThirdDigit_Rect;

EXTERN SDL_Rect Cons_Header_Rect;
EXTERN SDL_Rect Cons_Menu_Rect;
EXTERN SDL_Rect Cons_Text_Rect;
EXTERN SDL_Rect Cons_Menu_Rects[4];

EXTERN SDL_Rect LeftInfo_Rect;
EXTERN SDL_Rect RightInfo_Rect;

EXTERN SDL_Rect ConsMenuItem_Rect;


EXTERN SDL_Rect ProgressMeter_Rect;
EXTERN SDL_Rect ProgressBar_Rect;
EXTERN SDL_Rect ProgressText_Rect;


EXTERN float LastRefreshSound;
EXTERN float LastGotIntoBlastSound;
EXTERN float FPSover1;
EXTERN float FPSover10;
EXTERN float FPSover100;
EXTERN char *Alertcolor[AL_LAST];
EXTERN char *Shipnames[ALLSHIPS];

EXTERN char *Classname[];
EXTERN char *Classes[];
EXTERN char *Weaponnames[];
EXTERN char *Sensornames[];
EXTERN char *Brainnames[];
EXTERN char *Drivenames[];
EXTERN char *InfluenceModeNames[];
EXTERN int ThisMessageTime;

EXTERN influence_t Me;		/* the influence data */
EXTERN Druidspec Druidmap;
EXTERN Bulletspec Bulletmap;
EXTERN blastspec Blastmap[ALLBLASTTYPES];

EXTERN int Number_Of_Droid_Types;
EXTERN int PreTakeEnergy;
EXTERN int QuitProgram;
EXTERN int GameOver;
EXTERN int InvincibleMode;
EXTERN int HideInvisibleMap;
EXTERN int AlertLevel;
EXTERN int AlertThreshold; // threshold for FIRST Alert-color (yellow), the others are 2*, 3*..
EXTERN float AlertBonusPerSec; // bonus/sec for FIRST Alert-color, the others are 2*, 3*,...
EXTERN float DeathCount;   // a cumulative/draining counter of kills->determines Alert!
EXTERN float DeathCountDrainSpeed;  // drain per second
EXTERN float RealScore;
EXTERN long ShowScore;

EXTERN enemy AllEnemys[MAX_ENEMYS_ON_SHIP];

EXTERN int NumEnemys;

EXTERN Level CurLevel;		/* the current level data */
EXTERN ship curShip;		/* the current ship-data */

EXTERN bullet AllBullets[MAXBULLETS + 10];
EXTERN blast AllBlasts[MAXBLASTS + 10];

EXTERN int sound_on;		/* Toggle TRUE/FALSE for turning sounds on/off */
EXTERN int debug_level;       	/* 0=no debug 1=some debug messages 2=...etc */
				/* (currently only 0 or !=0 is implemented) */
EXTERN int show_all_droids;     /* display enemys regardless of IsVisible() */
EXTERN int stop_influencer;     /* for bullet debugging: stop where u are */

#undef EXTERN
#ifdef _misc_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN float Time_For_Each_Phase_Of_Door_Movement;
EXTERN float Blast_Damage_Per_Second;
EXTERN float Blast_Radius;
EXTERN float Droid_Radius;
EXTERN float LevelDoorsNotMovedTime;
EXTERN float collision_lose_energy_calibrator;
EXTERN config_t GameConfig;
EXTERN float CurrentCombatScaleFactor;
EXTERN BFont_Info *Menu_BFont;
EXTERN BFont_Info *Para_BFont;
EXTERN BFont_Info *Highscore_BFont;
EXTERN BFont_Info *Font0_BFont;
EXTERN BFont_Info *Font1_BFont;
EXTERN BFont_Info *Font2_BFont;
EXTERN int SkipAFewFrames;

#undef EXTERN
#ifdef _view_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN SDL_Color Black;

#undef EXTERN
#ifdef _sound_c
#define EXTERN
#else
#define EXTERN extern
#endif

#undef EXTERN
#ifdef _graphics_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN themelist_t AllThemes;
EXTERN int classic_theme_index;
EXTERN SDL_Cursor *crosshair_cursor;
EXTERN SDL_Cursor  *arrow_cursor;
EXTERN int Number_Of_Bullet_Types;
EXTERN SDL_Surface *ne_screen;   /* the graphics display */

EXTERN SDL_Surface *EnemySurfacePointer[ ENEMYPHASES ];   // A pointer to the surfaces containing the pictures of the
                                               // enemys in different phases of rotation
EXTERN SDL_Surface *InfluencerSurfacePointer[ ENEMYPHASES ];   // A pointer to the surfaces containing the pictures of the
                                               // influencer in different phases of rotation
EXTERN SDL_Surface *InfluDigitSurfacePointer[ DIGITNUMBER ];   // A pointer to the surfaces containing the pictures of the
                                               // influencer in different phases of rotation
EXTERN SDL_Surface *EnemyDigitSurfacePointer[ DIGITNUMBER ];   // A pointer to the surfaces containing the pictures of the
                                               // influencer in different phases of rotation
EXTERN SDL_Surface *MapBlockSurfacePointer[ NUM_COLORS ][ NUM_MAP_BLOCKS ];   // A pointer to the surfaces containing the map-pics, which may be rescaled with respect to
EXTERN SDL_Surface *OrigMapBlockSurfacePointer[ NUM_COLORS ][ NUM_MAP_BLOCKS ];   // A pointer to the surfaces containing the original map-pics as read from disk
EXTERN SDL_Surface *BuildBlock;			// a block for temporary pic-construction

EXTERN int BannerIsDestroyed;

EXTERN SDL_Surface *banner_pic;      /* the banner pic */
EXTERN SDL_Surface *pic999;
EXTERN SDL_RWops *packed_portraits[NUM_DROIDS];

EXTERN SDL_Surface *Decal_pics[NUM_DECAL_PICS];

EXTERN SDL_Surface *takeover_bg_pic;
EXTERN SDL_Surface *console_pic;
EXTERN SDL_Surface *console_bg_pic1;
EXTERN SDL_Surface *console_bg_pic2;

EXTERN SDL_Surface *arrow_up;
EXTERN SDL_Surface *arrow_down;
EXTERN SDL_Surface *arrow_right;
EXTERN SDL_Surface *arrow_left;

EXTERN SDL_Surface *ship_off_pic;    	/* Side-view of ship: lights off */
EXTERN SDL_Surface *ship_on_pic;	/* Side-view of ship: lights on */

EXTERN SDL_Surface *progress_meter_pic;
EXTERN SDL_Surface *progress_filler_pic;

EXTERN SDL_Rect level_rect[MAX_LEVELS]; /* rect's of levels in side-view */
EXTERN SDL_Rect liftrow_rect[MAX_LIFT_ROWS]; /* the lift-row rect's in side-view*/


#undef EXTERN
#ifdef _text_c
#define EXTERN
#else
#define EXTERN extern
#endif

#undef EXTERN
#ifdef _input_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN SDL_Joystick *joy;
EXTERN int joy_num_axes; /* number of joystick axes */
EXTERN int joy_sensitivity;
EXTERN point input_axis;  /* joystick (and mouse) axis values */
EXTERN int axis_is_active;  /* is firing to use axis-values or not */
EXTERN Uint32 last_mouse_event; // SDL-ticks of last mouse event

#undef EXTERN
#ifdef _highscore_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN highscore_entry **Highscores;
EXTERN int num_highscores;  /* total number of entries in our list (fixed) */

#undef EXTERN
#ifdef _takeover_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN SDL_Surface *to_blocks;      /* the global surface containing all game-blocks */
/* the rectangles containing the blocks */
EXTERN SDL_Rect FillBlocks[NUM_FILL_BLOCKS];
EXTERN SDL_Rect CapsuleBlocks[NUM_CAPS_BLOCKS];
EXTERN SDL_Rect ToGameBlocks[NUM_TO_BLOCKS];
EXTERN SDL_Rect ToGroundBlocks[NUM_GROUND_BLOCKS];
EXTERN SDL_Rect ToColumnBlock;
EXTERN SDL_Rect ToLeaderBlock;


EXTERN point LeftCapsulesStart[TO_COLORS];
EXTERN point CurCapsuleStart[TO_COLORS];
EXTERN point PlaygroundStart[TO_COLORS];
EXTERN point DruidStart[TO_COLORS];
EXTERN point TO_LeftGroundStart;
EXTERN point TO_RightGroundStart;
EXTERN point TO_ColumnStart;
EXTERN point TO_LeaderBlockStart;

EXTERN SDL_Rect TO_LeaderLed;
EXTERN SDL_Rect TO_FillBlock;
EXTERN SDL_Rect TO_ElementRect;
EXTERN SDL_Rect TO_CapsuleRect;
EXTERN SDL_Rect TO_GroundRect;
EXTERN SDL_Rect TO_ColumnRect;

#endif  // _global_h
