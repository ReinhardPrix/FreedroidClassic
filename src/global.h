/*=@Header==============================================================
 * $Source$
 *
 * @Desc: global variables (wuerg, urgs, stoehn,...)
 * 	
 * $Revision$
 * $State$
 *
 * $Author$
 *
 * $Log$
 * Revision 1.4  1993/05/23 21:08:14  prix
 * Sun May 23 16:31:46 1993: added new-struct variables to replace old ones
 *
 * Revision 1.5  1993/05/28  20:53:58  prix
 * Sun May 23 17:10:35 1993: Grob-Fein-Koordinaten ausradiert !!!
 * Tue May 25 07:19:29 1993: killed KRIT(geht nicht). some extern decl. in #extern clause
 * Thu May 27 09:41:35 1993: SpeedX, SpeedY now macros -> Me.speed.x...
 *
 * Revision 1.4  1993/05/23  21:08:14  prix
 * Sun May 23 16:31:46 1993: added new-struct variables to replace old ones
 *
 * Revision 1.3  1993/05/23  20:19:19  prix
 * Sat May 22 18:19:55 1993: Soundblastervariablen hinzugefuegt
 * Sun May 23 12:19:11 1993: removed old Level array
 *
 * Revision 1.2  1993/05/22  21:57:45  rp
 * Sat May 22 17:15:42 1993: changed int Level[][] to level CurLevel
 * Sat May 22 17:35:34 1993: int Level[][] wieder her, damit irgendwas laeuft inzwischen
 * Sat May 22 17:40:54 1993: added dimensions to Level
 *
 * Revision 1.1  1993/05/22  20:55:20  rp
 * Initial revision
 *
 *
 *-@Header------------------------------------------------------------*/
#ifndef _global_h
#define _global_h

/* *********************************************************************** */
/* **************************  V A R I A B L E N  ************************ */
/* *********************************************************************** */

#include "colodefs.h"

#undef EXTERN
#ifdef _paraplus_c
#define EXTERN
#else
#define EXTERN extern

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
EXTERN int DMAUseON;
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

EXTERN int UpPressed;
EXTERN int DownPressed;
EXTERN int LeftPressed;
EXTERN int RightPressed;
EXTERN int SpacePressed;
EXTERN int SpaceReleased;
EXTERN int QPressed;

#undef EXTERN
#ifdef _misc_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void interrupt (*OldInt09h)(void);
EXTERN void interrupt (*OldInt1Ch)(void);
EXTERN void interrupt (*OldInt23h)(void);
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
/* Ist fuer den Soundblaster */
EXTERN sb_register[256];


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
