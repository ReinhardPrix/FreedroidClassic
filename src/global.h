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

EXTERN const SDL_Rect User_Rect;
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
EXTERN char *Decknames[];
EXTERN char *Classname[ALLCLASSNAMES];
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

EXTERN FCU AllFCUs[];
EXTERN influence_t Me;		/* the influence data */
EXTERN druidspec Druidmap[ALLDRUIDTYPES];	/* map of druid specifications */
EXTERN bulletspec Bulletmap[ALLBULLETTYPES];	/* map of gun specs */
EXTERN blastspec Blastmap[ALLBLASTTYPES];
#endif

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
EXTERN float RealScore;
EXTERN long ShowScore;

EXTERN Hall_entry highscores;

EXTERN enemy Feindesliste[MAX_ENEMYS_ON_SHIP];

EXTERN int NumEnemys;
EXTERN int GameAdapterPresent;

EXTERN Level CurLevel;		/* the current level data */
EXTERN ship curShip;		/* the current ship-data */

EXTERN color Transfercolor;
EXTERN color Mobilecolor;

EXTERN unsigned char *RealScreen;
EXTERN unsigned char *InternalScreen;
EXTERN unsigned char *Outline320x200;

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

#undef EXTERN
#ifdef _misc_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN float CurrentCombatScaleFactor;
EXTERN int MaxMessageTime;
EXTERN int MinMessageTime;
EXTERN BFont_Info *Menu_BFont;
EXTERN BFont_Info *Para_BFont;
EXTERN BFont_Info *FPS_Display_BFont;
EXTERN float Overall_Average;
EXTERN int SkipAFewFrames;

#undef EXTERN
#ifdef _view_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN int View[INTERNHOEHE + 1][INTERNBREITE];


#undef EXTERN
#ifdef _sound_c
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN float Current_BG_Music_Volume;
EXTERN float Current_Sound_FX_Volume;

#undef EXTERN
#ifdef _graphics_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN int RahmenIsDestroyed;
EXTERN unsigned char *LevelColorArray;
EXTERN int *CRTC;
EXTERN unsigned char *Data70Pointer;

#ifdef NEW_ENGINE
EXTERN int Block_Width;
EXTERN int Block_Height;
EXTERN int Digit_Length;
EXTERN int Digit_Height;
EXTERN int Digit_Pos_X;
EXTERN int Digit_Pos_Y;
EXTERN const SDL_VideoInfo *ne_vid_info;    /* info about current video mode */
EXTERN color ne_transp_rgb;                /* RGB of transparent color */
EXTERN int ne_bpp; 			   /* bits per pixel */
EXTERN Uint32 ne_transp_key;               /* key of transparent color */
EXTERN SDL_Surface *ne_blocks;             /* here we collect all combat game blocks */
EXTERN SDL_Surface *ne_static;             /* here we collect all non-resizable game blocks */
EXTERN SDL_Rect *ne_map_block; 			/* arrays of block positions */
EXTERN SDL_Rect *ne_droid_block;
EXTERN SDL_Rect *ne_influ_block;
EXTERN SDL_Rect *ne_digit_block;
EXTERN SDL_Rect *ne_rahmen_block;
EXTERN SDL_Surface *ne_console_surface;
#endif


#undef EXTERN
#ifdef _blocks_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN unsigned char *RahmenPicture;
EXTERN unsigned char *Enemypointer;
EXTERN unsigned char *Influencepointer;
EXTERN unsigned char *MapBlocks;
EXTERN unsigned char *Digitpointer;
EXTERN unsigned char *ElevatorBlocks;
EXTERN unsigned char *ElevatorPicture;
EXTERN unsigned char *MenuItemPointer;
EXTERN unsigned char *Robotptr;


#undef EXTERN
#ifdef _text_c
#define EXTERN
#else
#define EXTERN extern
#endif

#undef EXTERN
#ifdef _svgaemu_c
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN float Current_Gamma_Correction;
EXTERN int Draw_Framerate;
EXTERN int Draw_Energy;

#ifdef NEW_ENGINE  /* this is for the new graphics engine */
EXTERN SDL_Surface *ne_screen;   /* the graphics display */
EXTERN SDL_Surface *ne_scaled_screen;   /* the graphics display */
#else
EXTERN SDL_Surface *screen;
EXTERN SDL_Surface *ScaledSurface;
#endif


#undef EXTERN
#ifdef _joy_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN SDL_Joystick *joy;
EXTERN int num_joy_axes; /* number of joystick axes */ 
EXTERN int joy_sensitivity;

#endif  // _global_h
