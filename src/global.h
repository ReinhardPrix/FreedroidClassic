/* 
 *
 *   Copyright (c) 1994, 2002 Johannes Prix
 *   Copyright (c) 1994, 2002 Reinhard Prix
 *
 *
 *  This file is part of FreeDroid
 *
 *  FreeDroid is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  FreeDroid is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with FreeDroid; see the file COPYING. If not, write to the 
 *  Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
 *  MA  02111-1307  USA
 *
 */


#ifndef _global_h
#define _global_h

#include "colodefs.h"

#undef EXTERN
#ifdef _paraplus_c
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
EXTERN char* Classes[];
EXTERN char* Height[];
EXTERN char* Weight[];
EXTERN char* Entry[];
EXTERN char* Weaponnames[];
EXTERN char* Sensornames[];
EXTERN int Sensor1[];
EXTERN int Sensor2[];
EXTERN int Sensor3[];
EXTERN int Armament[];
EXTERN int Drive[];
EXTERN int Brain[];
EXTERN char* Brainnames[];
EXTERN char* Drivenames[];
EXTERN int ThisMessageTime;

EXTERN shieldspec AllShields[];
EXTERN FCU AllFCUs[];
EXTERN influence_t Me;								/* the influence data */
EXTERN druidspec Druidmap[ALLDRUIDTYPES];	/* map of druid specifications */
EXTERN bulletspec Bulletmap[ALLBULLETTYPES];        /* map of gun specs */
EXTERN blastspec Blastmap[ALLBLASTTYPES];
#endif

EXTERN int InitBars;
EXTERN int BeamLine;
EXTERN int PreTakeEnergy;
EXTERN int Conceptview;
EXTERN int QuitProgram;
EXTERN int GameOver;
EXTERN int LastBlastHit;
EXTERN int InterruptInfolineUpdate;
EXTERN int InvincibleMode;
EXTERN int HideInvisibleMap;
EXTERN int PlusExtentionsOn;
EXTERN int Alert;
EXTERN int ThisShip;
EXTERN long RealScore;
EXTERN long ShowScore;
EXTERN long HighestScoreOfDay;
EXTERN long LowestScoreOfDay;
EXTERN long GreatScore;
EXTERN char* GreatScoreName;
EXTERN char* LowestName;
EXTERN char* HighestName;
EXTERN HallElement* Hallptr;
EXTERN char* IntroMSG1;
EXTERN enemy Feindesliste[MAX_ENEMYS_ON_SHIP];

EXTERN int NumEnemys;
EXTERN int GameAdapterPresent;
EXTERN int ModPlayerOn;

EXTERN Level CurLevel;				/* the current level data */
EXTERN ship curShip;				/* the current ship-data */

EXTERN color Transfercolor;
EXTERN color Mobilecolor;

EXTERN unsigned char *RealScreen;
EXTERN unsigned char *InternalScreen;

EXTERN bullet AllBullets[MAXBULLETS+10];
EXTERN blast AllBlasts[MAXBLASTS+10];
EXTERN int KeyCode;

EXTERN unsigned int MapBlockIndex;

EXTERN unsigned char *InternWindow;

EXTERN char LeftInfo[50];
EXTERN char RightInfo[50];

EXTERN int taste;

EXTERN int sound_on; 	/* Toggle TRUE/FALSE for turning sounds on/off */

// PORT EXTERN int UpPressed;
// PORT EXTERN int DownPressed;
// PORT EXTERN int LeftPressed;
// PORT EXTERN int RightPressed;
// PORT EXTERN int SpacePressed;
// PORT EXTERN int SpaceReleased;
// PORT EXTERN int QPressed;

#undef EXTERN
#ifdef _misc_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN int MaxMessageTime;
EXTERN int MinMessageTime;

#undef EXTERN
#ifdef _view_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN int View[INTERNHOEHE+1][INTERNBREITE];


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
EXTERN unsigned char* LevelColorArray;
EXTERN int* CRTC;
EXTERN unsigned char* Data70Pointer;

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
EXTERN unsigned char *MenuItemPointer;
EXTERN unsigned char *Robotptr;
EXTERN unsigned char *ShieldBlocks;

#undef EXTERN
#ifdef _paratext_c
#define EXTERN
#else
#define EXTERN extern
#endif

#endif
