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

EXTERN char ConfigDir[255];
EXTERN SDL_Rect User_Rect;
EXTERN const SDL_Rect Classic_User_Rect;
EXTERN const SDL_Rect Full_User_Rect;

EXTERN const SDL_Rect Full_Screen_Rect;
EXTERN const SDL_Rect Menu_Rect;


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
EXTERN char *Classname[];
EXTERN char *Classes[];
EXTERN char *Height[];
EXTERN char *Weight[];
EXTERN char *Entry[];
EXTERN char *Weaponnames[];
EXTERN char *Sensornames[];
EXTERN int Sensor1[];
EXTERN int Sensor2[];
EXTERN int Sensor3[];
EXTERN int Armament[];
EXTERN int Drive[];
EXTERN int Brain[];
EXTERN char *Brainnames[];
EXTERN char *Drivenames[];
EXTERN int ThisMessageTime;

EXTERN influence_t Me;		/* the influence data */
// EXTERN druidspec Druidmap[ALLDRUIDTYPES];	/* map of druid specifications */
EXTERN Druidspec Druidmap;     
// EXTERN bulletspec Bulletmap[ALLBULLETTYPES];	/* map of gun specs */
EXTERN Bulletspec Bulletmap;
EXTERN blastspec Blastmap[ALLBLASTTYPES];
#endif

EXTERN int Number_Of_Droid_Types;
EXTERN int InitBars;
EXTERN int PreTakeEnergy;
EXTERN int QuitProgram;
EXTERN int GameOver;
EXTERN int InvincibleMode;
EXTERN int HideInvisibleMap;
EXTERN int PlusExtentionsOn;
EXTERN int Alert;
EXTERN int ThisShip;
EXTERN float RealScore;
EXTERN long ShowScore;

EXTERN enemy AllEnemys[MAX_ENEMYS_ON_SHIP];

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
EXTERN int show_all_droids;     /* display enemys regardless of IsVisible() */
EXTERN int stop_influencer;     /* for bullet debugging: stop where u are */
EXTERN int mouse_control;       /* allow for mouse control */

#undef EXTERN
#ifdef _misc_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN double Time_For_Each_Phase_Of_Door_Movement;
EXTERN double Blast_Damage_Per_Second;
EXTERN double Blast_Radius;
EXTERN double Droid_Radius;
EXTERN float LevelDoorsNotMovedTime;
EXTERN double collision_lose_energy_calibrator;
EXTERN configuration_for_freedroid GameConfig;
EXTERN float CurrentCombatScaleFactor;
EXTERN BFont_Info *Menu_BFont;
EXTERN BFont_Info *Para_BFont;
EXTERN BFont_Info *FPS_Display_BFont;
EXTERN BFont_Info *Highscore_BFont;
EXTERN int SkipAFewFrames;

#undef EXTERN
#ifdef _view_c
#define EXTERN
#else
#define EXTERN extern
#endif

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
EXTERN int Number_Of_Bullet_Types;
EXTERN SDL_Surface *ne_screen;   /* the graphics display */
EXTERN SDL_Surface *ne_scaled_screen;   /* the graphics display */
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
EXTERN int First_Digit_Pos_X;
EXTERN int First_Digit_Pos_Y;
EXTERN int Second_Digit_Pos_X;
EXTERN int Second_Digit_Pos_Y;
EXTERN int Third_Digit_Pos_X;
EXTERN int Third_Digit_Pos_Y;
EXTERN const SDL_VideoInfo *ne_vid_info;/* info about current video mode */
EXTERN color ne_transp_rgb;             /* RGB of transparent color */
EXTERN int ne_bpp; 			/* bits per pixel */
EXTERN Uint32 ne_transp_key;            /* key of transparent color */
EXTERN SDL_Surface *ne_static;          /* here we collect all non-resizable blocks */
EXTERN SDL_Surface *ne_console_surface;
EXTERN SDL_Surface *ne_console_bg_pic1;
EXTERN SDL_Surface *ne_console_bg_pic2;

EXTERN SDL_Surface *ship_off_pic;    	/* Side-view of ship: lights off */
EXTERN SDL_Surface *ship_on_pic;	/* Side-view of ship: lights on */

EXTERN SDL_Rect level_rect[MAX_LEVELS]; /* rect's of levels in side-view */
EXTERN SDL_Rect liftrow_rect[MAX_LIFT_ROWS]; /* the lift-row rect's in side-view*/
EXTERN SDL_Rect *ne_rahmen_block;
EXTERN SDL_Surface *ne_console_surface;


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


#endif  // _global_h
