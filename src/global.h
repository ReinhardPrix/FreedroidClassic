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


#ifndef _global_h
#define _global_h

#include "BFont.h"
#include "colodefs.h"

#undef EXTERN
#ifdef _main_c
#define EXTERN
#else
#define EXTERN extern

EXTERN item_image_spec ItemImageList[ NUMBER_OF_ITEM_PICTURES ];
EXTERN item_bonus PrefixList[];
EXTERN item_bonus SuffixList[];
EXTERN char EndTitleText1[];
EXTERN char EndTitleText2[];
EXTERN char TitleText1[];
EXTERN char TitleText2[];
EXTERN char TitleText3[];
EXTERN char TitleText4[];
EXTERN char Text1[];
EXTERN float LastRefreshSound;
EXTERN float LastGotIntoBlastSound;
EXTERN float FPSover1;
EXTERN float FPSover10;
EXTERN float FPSover100;
EXTERN char *Alertcolor[ALLALERTCOLORS];
EXTERN char *Shipnames[ALLSHIPS];
EXTERN char *NetworkClientStatusNames[];
EXTERN char *Classname[];
EXTERN char *Classes[];
EXTERN char *Height[];
EXTERN char *Weight[];
EXTERN char *Entry[];
EXTERN char *Weaponnames[];
EXTERN char *Sensornames[];
EXTERN char *AllSkillTexts[];
EXTERN char *SkillName[];
EXTERN char *Brainnames[];
EXTERN int ThisMessageTime;

EXTERN int ManaCostTable [ NUMBER_OF_SKILLS ] [ NUMBER_OF_SKILL_LEVELS ] ;
EXTERN int SpellHitPercentageTable [  ] ;
EXTERN float MeleeDamageMultiplierTable [  ] ;
EXTERN float MeleeRechargeMultiplierTable [  ] ;
EXTERN float RangedDamageMultiplierTable [  ] ;
EXTERN float RangedRechargeMultiplierTable [  ] ;
EXTERN influence_t Me[ MAX_PLAYERS ];		/* the influence data */
EXTERN network_influence_t NetworkMe[ MAX_PLAYERS ];		/* the influence data */
EXTERN map_insert_spec AllMapInserts[ MAX_MAP_INSERTS ] ;
// EXTERN druidspec Druidmap[ALLDRUIDTYPES];	/* map of druid specifications */
EXTERN Druidspec Druidmap;     
// EXTERN bulletspec Bulletmap[ALLBULLETTYPES];	/* map of gun specs */
EXTERN Bulletspec Bulletmap;
EXTERN blastspec Blastmap[ALLBLASTTYPES];
#endif

EXTERN SDL_Rect User_Rect;
EXTERN SDL_Rect Full_Screen_Rect;
EXTERN SDL_Rect Classic_User_Rect;
EXTERN SDL_Rect Full_User_Rect;
EXTERN SDL_Rect Cons_Rect;
EXTERN SDL_Rect Cons_Menu_Rect;
EXTERN SDL_Rect Cons_Text_Rect;


EXTERN const SDL_Rect Menu_Rect;


EXTERN int Number_Of_Droid_Types;
EXTERN int InitBars;
EXTERN int PreTakeEnergy;
EXTERN int QuitProgram;
EXTERN int GameOver;
EXTERN int LastBlastHit;
EXTERN int InvincibleMode;
EXTERN int HideInvisibleMap;
EXTERN int PlusExtentionsOn;
EXTERN int Alert;
EXTERN int ThisShip;
EXTERN long ShowScore;
EXTERN int RespectVisibilityOnMap;

EXTERN int ServerMode;
EXTERN int ClientMode;

EXTERN enemy AllEnemys[ MAX_ENEMYS_ON_SHIP ];
EXTERN spell_active AllActiveSpells[ MAX_ACTIVE_SPELLS ];
EXTERN network_enemy NetworkAllEnemys[ MAX_ENEMYS_ON_SHIP ];
EXTERN event_trigger AllEventTriggers[ MAX_EVENT_TRIGGERS ];
EXTERN triggered_action AllTriggeredActions[ MAX_TRIGGERED_ACTIONS_IN_GAME ];
EXTERN char ServerName[ 10000 ];

EXTERN int NumEnemys;

EXTERN Level CurLevel;		/* the current level data */
EXTERN ship curShip;		/* the current ship-data */

EXTERN bullet AllBullets[MAXBULLETS + 10];
EXTERN blast AllBlasts[MAXBLASTS + 10];
EXTERN int KeyCode;

EXTERN unsigned int MapBlockIndex;

EXTERN unsigned char *InternWindow;

EXTERN int taste;

EXTERN int sound_on;		/* Toggle TRUE/FALSE for turning sounds on/off */
EXTERN int debug_level;       	/* 0=no debug 1=some debug messages 2=...etc */
				/* (currently only 0 or !=0 is implemented) */
EXTERN int fullscreen_on;	/* toggle for use of fullscreen vs. X11-window */
EXTERN int show_all_droids;     /* display enemys regardless of IsVisible() */
EXTERN int stop_influencer;     /* for bullet debugging: stop where u are */
EXTERN int mouse_control;       /* allow for mouse control */
EXTERN int classic_user_rect;   /* use the User-Rect dimensions of the original game? */
#undef EXTERN
#ifdef _misc_c
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN mouse_press_button AllMousePressButtons[ MAX_MOUSE_PRESS_BUTTONS ] ;
EXTERN int Item_Held_In_Hand;
EXTERN point InventorySize;
// EXTERN item AllItems[ MAX_ITEMS_PER_LEVEL ];
EXTERN itemspec ItemMap[ ALL_ITEMS ];

// EXTERN SDL_Surface* ItemSurfaceList[ NUMBER_OF_ITEM_PICTURES ];
// EXTERN item_image_spec ItemImageList[ NUMBER_OF_ITEM_PICTURES ];
EXTERN SDL_Surface* MouseCursorImageList[ NUMBER_OF_MOUSE_CURSOR_PICTURES ];
EXTERN SDL_Surface* SpellLevelButtonImageList[ NUMBER_OF_SKILL_LEVELS ];

EXTERN double Conveyor_Belt_Speed;
EXTERN long Total_Frames_Passed_In_Mission;
EXTERN int Number_Of_Droids_On_Ship;
EXTERN double Time_For_Each_Phase_Of_Door_Movement;
EXTERN double Blast_Damage_Per_Second;
EXTERN double Blast_Radius;
EXTERN double Druid_Radius_X;
EXTERN double Druid_Radius_Y;
EXTERN float LevelDoorsNotMovedTime;
EXTERN float LevelGunsNotFiredTime;
EXTERN double collision_lose_energy_calibrator;
EXTERN configuration_for_freedroid GameConfig;
EXTERN float CurrentCombatScaleFactor;
EXTERN int MaxMessageTime;
EXTERN int MinMessageTime;
EXTERN BFont_Info *Menu_BFont;
EXTERN BFont_Info *Menu_Filled_BFont;
EXTERN BFont_Info *Para_BFont;
EXTERN BFont_Info *FPS_Display_BFont;
EXTERN BFont_Info *Blue_BFont;
EXTERN BFont_Info *Red_BFont;
EXTERN BFont_Info *Highscore_BFont;
EXTERN float Overall_Average;
EXTERN int SkipAFewFrames;

#undef EXTERN
#ifdef _view_c
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN SDL_Rect InventoryRect;

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
EXTERN int Number_Of_Item_Types;
EXTERN int Number_Of_Bullet_Types;
EXTERN SDL_Surface *Screen;   /* the graphics display */
EXTERN SDL_Surface *SkillIconSurfacePointer[ NUMBER_OF_SKILLS ]; // A pointer all skill icon surfaces
EXTERN SDL_Surface *EnemySurfacePointer[ DROID_PHASES + DEAD_DROID_PHASES ];   // A pointer to the surfaces containing the pictures of the
                                               // enemys in different phases of rotation
EXTERN SDL_Surface *BlueEnemySurfacePointer[ DROID_PHASES + DEAD_DROID_PHASES ];   // A pointer to the surfaces containing the pictures of the
EXTERN SDL_Surface *GreenEnemySurfacePointer[ DROID_PHASES + DEAD_DROID_PHASES ];   // A pointer to the surfaces containing the pictures of the
EXTERN SDL_Surface *RedEnemySurfacePointer[ DROID_PHASES + DEAD_DROID_PHASES ];   // A pointer to the surfaces containing the pictures of the
EXTERN SDL_Surface *InfluencerSurfacePointer[ DROID_PHASES + DEAD_DROID_PHASES ];   // A pointer to the surfaces containing the pictures of the
EXTERN SDL_Surface *EnemyRotationSurfacePointer[ ENEMY_ROTATION_MODELS_AVAILABLE ] [ ROTATION_ANGLES_PER_ROTATION_MODEL ];   // A pointer to the surfaces containing the pictures of the
                                               // enemys in different anles of rotation
EXTERN SDL_Surface *BlueEnemyRotationSurfacePointer[ ENEMY_ROTATION_MODELS_AVAILABLE ] [ ROTATION_ANGLES_PER_ROTATION_MODEL ];   // A pointer to the surfaces containing the pictures of the
                                               // enemys in different anles of rotation
EXTERN SDL_Surface *RedEnemyRotationSurfacePointer[ ENEMY_ROTATION_MODELS_AVAILABLE ] [ ROTATION_ANGLES_PER_ROTATION_MODEL  ];   // A pointer to the surfaces containing the pictures of the
                                               // enemys in different anles of rotation
EXTERN SDL_Surface *GreenEnemyRotationSurfacePointer[ ENEMY_ROTATION_MODELS_AVAILABLE ] [ ROTATION_ANGLES_PER_ROTATION_MODEL ];   // A pointer to the surfaces containing the pictures of the
                                               // enemys in different anles of rotation
                                               // influencer in different phases of rotation
EXTERN SDL_Surface *TuxWorkingCopy [ MAX_PLAYERS ] [ TUX_BREATHE_PHASES + TUX_SWING_PHASES + TUX_GOT_HIT_PHASES ] [ MAX_TUX_DIRECTIONS ] ;   // A pointer to the surfaces containing the tux
EXTERN SDL_Surface *TuxMotionArchetypes[ TUX_MODELS ][ TUX_BREATHE_PHASES + TUX_SWING_PHASES + TUX_GOT_HIT_PHASES ];   // A pointer to the surfaces containing the tux
EXTERN SDL_Surface *InfluDigitSurfacePointer[ DIGITNUMBER ];   // A pointer to the surfaces containing the pictures of the
                                               // influencer in different phases of rotation
EXTERN SDL_Surface *EnemyDigitSurfacePointer[ DIGITNUMBER ];   // A pointer to the surfaces containing the pictures of the
                                               // influencer in different phases of rotation
EXTERN SDL_Surface *MapBlockSurfacePointer[ NUM_COLORS ][ NUM_MAP_BLOCKS ];   // A pointer to the surfaces containing the pictures of the
                                               // influencer in different phases of rotation
EXTERN char *ship_on_filename;
EXTERN char *ship_off_filename;
EXTERN int BannerIsDestroyed;
EXTERN unsigned char *LevelColorArray;
EXTERN int *CRTC;
EXTERN unsigned char *Data70Pointer;

EXTERN int Block_Width;
EXTERN int Block_Height;
EXTERN int Digit_Length;
EXTERN int Digit_Height;
EXTERN point Digit_Pos[3];
EXTERN const SDL_VideoInfo *vid_info;/* info about current video mode */
EXTERN color transp_rgb;             /* RGB of transparent color */
EXTERN int vid_bpp; 			/* bits per pixel */
EXTERN Uint32 transp_key;            /* key of transparent color */
EXTERN SDL_Surface *static_blocks;          /* here we collect all non-resizable blocks */
EXTERN SDL_Surface *ship_off_pic;    	/* Side-view of ship: lights off */
EXTERN SDL_Surface *ship_on_pic;	/* Side-view of ship: lights on */
EXTERN SDL_Surface *banner_pic;

EXTERN SDL_Rect level_rect[MAX_LEVELS]; /* rect's of levels in side-view */
EXTERN SDL_Rect liftrow_rect[MAX_LIFT_ROWS]; /* the lift-row rect's in side-view*/

#undef EXTERN
#ifdef _blocks_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN unsigned char *BannerPicture;
EXTERN unsigned char *Enemypointer;
EXTERN unsigned char *Influencepointer;
EXTERN unsigned char *MapBlocks;
EXTERN unsigned char *Digitpointer;
EXTERN unsigned char *MenuItemPointer;
EXTERN unsigned char *Robotptr;


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


#endif  // _global_h
