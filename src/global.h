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

EXTERN shieldspec AllShields[];
EXTERN FCU AllFCUs[];
EXTERN influence_t Me;		/* the influence data */
EXTERN druidspec Druidmap[ALLDRUIDTYPES];	/* map of druid specifications */
EXTERN bulletspec Bulletmap[ALLBULLETTYPES];	/* map of gun specs */
EXTERN blastspec Blastmap[ALLBLASTTYPES];
#endif

EXTERN int InitBars;
EXTERN int PreTakeEnergy;
EXTERN int Conceptview;
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
EXTERN float HighestScoreOfDay;
EXTERN float LowestScoreOfDay;
EXTERN float GreatScore;
EXTERN char *GreatScoreName;
EXTERN char *LowestName;
EXTERN char *HighestName;
EXTERN HallElement *Hallptr;
EXTERN char *IntroMSG1;
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
EXTERN int MaxMessageTime;
EXTERN int MinMessageTime;
// EXTERN BFont_Info *Font1=NULL;
EXTERN BFont_Info *Font1;


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
EXTERN unsigned char *LevelColorArray;
EXTERN int *CRTC;
EXTERN unsigned char *Data70Pointer;

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
EXTERN unsigned char *ShieldBlocks;

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
EXTERN SDL_Surface *screen;
EXTERN SDL_Surface *ScaledSurface;

#endif
