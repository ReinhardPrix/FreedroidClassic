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
 * Revision 1.6  1994/06/19 16:20:13  prix
 * Fri May 28 16:56:52 1993: killed further vars
 * Fri May 28 17:12:45 1993: moved Macros to defs.h
 * Fri May 28 17:38:17 1993: added Gunmap[]
 * Fri May 28 19:47:14 1993: made BlockPointers far
 * Sun May 30 10:49:04 1993: CurLevel is now a pointer (Level statt level!)
 * Sun May 30 14:09:37 1993: new def-names for declarations
 * Mon May 31 14:32:30 1993: Describe Changes:
 * Mon May 31 14:32:49 1993: MapBlockPointer not void* but char* !!
 * Mon May 31 14:42:26 1993: Blast/Bullet types now blast/bullet !
 * Mon May 31 17:56:41 1993: added ScreenPointer
 * Mon May 31 20:00:59 1993: char pointers unsigned
 * Sat Jul 24 07:37:41 1993: Digitpointer eingefuehrt
 * Sat Jul 24 11:09:19 1993: Influencepointer eingefuehrt
 * Sat Jul 24 14:07:04 1993: Enemypointer eingewfuehrt
 * Sat Jul 24 14:40:20 1993: Feindesliste hinzugefuegt
 * Sun Jul 25 15:52:51 1993: sicherheit
 * Wed Jul 28 16:41:46 1993: JoyXYB
 * Wed Jul 28 16:43:05 1993: GameAdapterPresent = 1
 * Wed Jul 28 17:36:56 1993: Joy* Variable nicht global
 * Fri Jul 30 07:26:38 1993: Added Agression
 * Sat Jul 31 07:43:42 1993: Transfercolor und Mobilecolor sing global
 * Sat Jul 31 17:58:33 1993: Me ist kein gewoehnlicher "druid" sondern ein"influence_t"
 * Sat Jul 31 18:01:38 1993: Enemy ist kei "druid" sondern ein "enemy"
 * Sat Jul 31 19:08:01 1993: added GameAdapterPresent (again)
 * Fri Aug 06 15:30:57 1993: added CurShip
 * Tue Aug 10 10:23:33 1993: LevelColorArray hinzugefuegt
 * Tue Aug 10 12:40:54 1993: LevelColorArray ist jetzt ein char*
 * Wed Aug 11 08:41:50 1993: Space Released hinzugefuegt
 * Wed Sep 15 14:04:21 1993: parafont hat einen eigenen Abschnitt
 * Sat Sep 18 11:28:12 1993: ADDED Pointer: RahmenPicture for the Rahmen-Bild
 * Tue Sep 21 06:08:36 1993: MyCursorX,Y
 * Tue Sep 21 14:37:01 1993: some fontvariables added
 * Sat Sep 25 10:18:13 1993: added LeftInfo and RightInfo
 * Sat Sep 25 15:53:44 1993: added ElevatorPicture
 * Tue Oct 05 13:54:16 1993: added ElevatorRaster[][]: for rastering the el.-picture
 * Tue Oct 05 15:22:18 1993: renamed MapBlockPointer to MapBlocks[]
 * Thu Oct 07 20:17:26 1993: added ElevatorColors[][]
 * Sun Oct 17 08:44:27 1993: PulsExterntionOn eingefuehrt
 * Sat May 21 17:27:25 1994: MessageBar hinzugef"ugt
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
