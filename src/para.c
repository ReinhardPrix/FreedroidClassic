static const char RCSid[]=\
"$Id$";

#define _paraplus_c

extern void interrupt Interrupt1C(void);

/* *********************************************************************** */
/* ************  P R é P R O Z E S S O R D I R E K T I V E N  ************ */
/* *********************************************************************** */

/* Schalter fuer die bedingte Kompilierung */
#undef DREHSCHUESSE
#undef SCHUSSDEBUG
#undef INFLUNCECLPOSDEBUG
#undef INFLUENCEWALLDEBUG
#undef DEBUGREPORT
#undef BULLETSPEEDUPONRICHTUNG
#undef NOKEYBEEPS
#undef SAYINFLUENCEPOS
#undef KEYDEBUG
#undef SAYJOYPOS
#undef SPEICHERSPAREN
//#define TITLEAUS
#undef MODSCHASEIN
#undef NOJUNKWHILEINIT


#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <alloc.h>
#include <dos.h>
#include <time.h>

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"
#include "fm_hrd.h"
#include "paratext.h"
#include "paravars.h"
#include "ship.h"

#define MOVEENEMYSOFF 				FALSE
#define ANIMATEENEMYSOFF 			FALSE
#define INFLUENCEGETROFFENOFF 	FALSE

#define FENSTEROFF 					FALSE
#define ENERGIEBALKENOFF 			TRUE
#define REDRAW001OFF 				FALSE
#define USEINTSOFF 					FALSE

/* Scroll- Fenster */
#define SCROLLSTARTX		USERFENSTERPOSX 
#define SCROLLSTARTY		SCREENHOEHE
#define CHARSPERLINE		(int)(USERFENSTERBREITE/FONTBREITE)

int SetDebugPos=FALSE;

void Debriefing(void);
void ShowHighscoreList(void);

/*@Function============================================================
@Desc: main(): the heart of the Game 

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void main(void)
{
	int i;

	GameOver = FALSE;
	QuitProgram = FALSE;
	Conceptview = FALSE;

	InterruptInfolineUpdate=TRUE;
	DMAUseON = 0;

	/* Initialisieren der globalen Variablen und Arrays */
	SaveVideoMode();
	InitParaplus();

#if REDRAW001OFF == 0
	RedrawInfluenceNumber();
#endif

	while( !QuitProgram ) {
		
		/* InitNewGame */
		InitNewGame();
	
		GameOver = FALSE;
		
		while (!GameOver && !QuitProgram) {

			PlusRahmen(RealScreen);
		
			/* Umschaltung Joystick/Keyboard */
			while( kbhit() ) {
				if( (taste = getch()) == 'k' ) {
					GameAdapterPresent = !GameAdapterPresent;
					RightPressed = LeftPressed = UpPressed = DownPressed = SpacePressed = FALSE;
				}
				if(taste == 'q') QuitProgram = TRUE;
				if(taste == 'm') ModPlayerOn = !ModPlayerOn;
				if(taste == 'd') Me.energy=0; 
				if(taste == 'l') ShowHighscoreList();
				if(taste == 'i') ShowDebugInfos();
				if(taste == 'v') HideInvisibleMap = !HideInvisibleMap; 
				if(taste == 'c') Cheatmenu();
				if(taste == 'p') {
					while (!SpacePressed) {
						JoystickControl();
						Me.status=PAUSE;
						while (!TimerFlag) JoystickControl();
						TimerFlag=FALSE;
						AnimateInfluence();
						AnimateRefresh();
						AnimateEnemys();
						GetInternFenster();
						PutInternFenster();
						if (kbhit()) taste=getch();
						if (taste == 'c') {
							JoystickControl();
							Me.status=CHEESE;
							while (!SpacePressed) {
								JoystickControl();
								if (kbhit()) taste=getch();
								if (taste == ' ') SpacePressed = TRUE;
							}
							while (SpacePressed) {
								JoystickControl();
								if (kbhit()) taste=getch();
								if (taste == ' ') SpacePressed = FALSE;
							}
							taste = 1;
						}
						if (taste == ' ') SpacePressed = TRUE;
					}
					while (SpacePressed) JoystickControl();
					KillTastaturPuffer();
				}
			} /* if kbhit() */
	
			if( !TimerFlag ) continue;		/* the clock - timing */
			else TimerFlag = FALSE;
	
			if( ShipEmptyCounter == 1) GameOver = TRUE;

			LastBlastHit++;

			/*
			 * Hier wird die Statuszeile ausgegeben
			 *
			 */
#define STATUS_OFF
#ifndef STATUS_OFF
			gotoxy(1,1);
			printf("Paradroid ");
			if (PlusExtentionsOn) printf("+"); else printf("C-64");
			printf(" Control:");
			if (GameAdapterPresent) printf("Joy"); else printf("Keys");
			printf(" Energy: %d\n",Me.energy);
#endif
#ifdef KEYDEBUG
			printf("Right:%d,Left:%d,Up:%d,Down:%d,Space:%d.\n",RightPressed,LeftPressed,UpPressed,DownPressed,SpacePressed);
			printf(" KeyCode: %x  \n",KeyCode);
#endif
			 
			/* Die Tueren im Level auf und zu bewegen */
	    	MoveLevelDoors();
	
	    	/* Refreshes Animieren */
	    	AnimateRefresh();
	
			for (i=0;i<NumEnemys;i++) {
				if (Feindesliste[i].warten > 0) Feindesliste[i].warten--;
				if (Feindesliste[i].firewait > 0) Feindesliste[i].firewait--;
			}
	
	    		
			/* Bullets entsprechend ihrer Geschwindigkeit weiterbewegen */
			MoveBullets();
	
			/* Blasts in der Explosionsphase weiterbewegen */
			ExplodeBlasts();
	
	
			/* Einen Ausschnitt aus der Gesamtlevelkarte machen */
			GetView();
	
			/* Eventuell die momentane Position und Keycode anzeigen */
			ShowPosition();
			
	
			GetInternFenster();
			PutInternFenster();
			for (i=0;i<MAXBULLETS;i++) CheckBulletCollisions(i);
			PutMessages();
	
			/* Wenn vorhanden: Joystick einlesen */
			JoystickControl();

#ifdef NOKEYBEEPS
			KillTastaturPuffer();		// BEEPs vorbeugen
#endif
	
			/* Gemaess den gedrueckten Tasten die Geschwindigkeit veraendern */
			MoveInfluence();
	

			MoveEnemys();			/* Auch die Feinde bewegen */

	
			/* Bei animierten Influencer die Phasen weiterzaehlen */
			AnimateInfluence();
	
			/* Bei den Feinden auch Phasen weiterzaehlen */

			AnimateEnemys();

			/* Raeder bremsen die Fahrt des Influencers erheblich */
	      Reibung();
	
			/* Influencedruid nach der momentanen Geschwindigkeit weiterbewegen */
			Me.pos.x+=SpeedX;
			Me.pos.y+=SpeedY;
			AdjustSpeed();
	
			/* Testen ob der Weg nicht durch Mauern verstellt ist */
			BounceInfluencer();
			InfluenceEnemyCollision();
	
			RotateBulletColor();

			if( CurLevel->empty == 2) {
				LevelGrauFaerben();
				CurLevel->empty = TRUE;
			} /* if */
	
			
			/* Die Ausgaben im Rahmen aktualisieren */
//			SetInfoline();
//			UpdateInfoline();
		
	   } /* while !GameOver */
	} /* while !QuitProgram */

	Terminate(0);
}

/*@Function============================================================
@Desc: InitNewGame(): 	Startwerte fuer neues Spiel einstellen 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void InitNewGame(void)
{
	int i;	

	InterruptInfolineUpdate = TRUE;
	LastBlastHit = 0;
	BeamLine = BLOCKBREITE;
	/*
	 * Die Punkte wieder auf 0 setzen
	 */
	RealScore=0;
	ShowScore=0;

	/* L"oschen der Messagequeue */
	KillQueue();
	InsertMessage(" Hello. Good Game And Good Luck To The.");
	
	/* Alle Bullets und Blasts loeschen */
	for (i=0; i<MAXBULLETS; i++) {
		AllBullets[i].type = OUT;
		AllBullets[i].mine = FALSE;
	}

	for (i=0; i<MAXBLASTS; i++) {
		AllBlasts[i].phase = OUT;
		AllBlasts[i].type = OUT;
	}

	/* Alle Levels wieder aktivieren */
	for(i=0; i<curShip.LevelsOnShip; i++ )
		curShip.AllLevels[i]->empty =FALSE;


	i=random(4);
	switch(i) {
		case 0: {
			Me.pos.x = 120;
			Me.pos.y =  48;
			CurLevel = curShip.AllLevels[4];
			break;
		}
		case 1: {
			Me.pos.x = 120;
			Me.pos.y =  48;
			CurLevel = curShip.AllLevels[5];
			break;
		}
		case 2: {
			Me.pos.x = 120;
			Me.pos.y =  48;
			CurLevel = curShip.AllLevels[6];
			break;
		}
		case 3: {
			Me.pos.x = 120;
			Me.pos.y =  48;
			CurLevel = curShip.AllLevels[7];
			break;
		}
	}

	/* Alertcolor auf Gruen zurueckschalten */
	Alert = 0;
	
	/* Enemys initialisieren */
	if( GetCrew(SHIPNAME) == ERR ) Terminate(-1);

	/* Influ initialisieren */
	Me.type = DRUID001;
	Me.speed.x = 0;
	Me.speed.y = 0;
	Me.energy = STARTENERGIE;
	Me.health = Me.energy;		/* start with max. health */
	Me.autofire = FALSE;
	Me.status = MOBILE;
	RedrawInfluenceNumber();
	

	/* Introduction und Title */
#ifndef TITLEAUS
	Title();
#endif
	
	/* Farben des aktuellen Levels einstellen */
 	SetLevelColor(CurLevel->color);
	
	LeftInfo[0] = '\0';
	RightInfo[0] = '\0';
	
	/* Den Rahmen fuer das Spiel anzeigen */
	ClearGraphMem(RealScreen);
	DisplayRahmen(RealScreen);

   SetTextBorder(0,0, SCREENBREITE, SCREENHOEHE, 40 );

	SetTextColor(FONT_WHITE,FONT_RED);
//	InitPalette();
#ifdef NOJUNKWHILEINIT
	Monitorsignalunterbrechung(0);
#endif
	InitBars=TRUE;

} /* InitNewGame */

/*@Function============================================================
@Desc: InitParaplus(): initialisiert das Spiel beim Programmstart

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void InitParaplus(void)
{
	int i;
	init_sb();

	/* Zuerst den Videomodus setzen */
	SetVideoMode(0x13);

	/* Unterbrechung des Monitorsignal solange Initialisierung l"auft. */
#ifdef NOJUNKWHILEINIT
	Monitorsignalunterbrechung(1);
#endif

	/* Initialisierung der Highscorewerte */
	LowestName=MyMalloc(200);
	HighestName=MyMalloc(200);
	GreatScoreName=MyMalloc(200);
	strcpy(LowestName," I'm the lowest.");
	strcpy(HighestName,"I'm the highest.");
	strcpy(GreatScoreName,"I'm great.");
	Hallptr=MyMalloc(sizeof(HallElement)+1);
	Hallptr->PlayerScore=0;
	Hallptr->PlayerName=MyMalloc(10);
	strcpy(Hallptr->PlayerName," dummy ");
	Hallptr->NextPlayer=NULL;

	LowestScoreOfDay=100;
	HighestScoreOfDay=1000;
	GreatScore=101;

	HideInvisibleMap=TRUE;         /* Hide invisible map-parts */
	
	/* Initialisieren der CRTC Portadresse */
	CRTC=(int*)MK_FP(0,0x0463);


	if (InitLevelColorTable() == FALSE) {
		printf(" Kann Farben nicht initialisieren !");
		getch();
		Terminate(0);
	}
 
	if (InitParaplusFont() == ERR) {
		printf(" Kann Schrift nicht initialisieren !");
		getch();
		Terminate(ERR);
	}

	MinMessageTime=55;
	MaxMessageTime=850;
/* Farbwerte fuer die Funktion SetColors */
	Transfercolor.gruen=13;
	Transfercolor.blau=13;
	Transfercolor.rot=63;
	Mobilecolor.gruen=63;
	Mobilecolor.blau=63;
	Mobilecolor.rot=63;

	
	Druidmap[DRUID001].notes="robot activity\ninfluence device. This\nhelmet is self-powered\nand will control any\nrobot for a short time.\nLasers are turret‹mounted.\n";
 	Druidmap[DRUID123].notes="simpe rubbish\ndiposal robot. Common\ndevice in most space\ncraft to maintain a clean\nship.\n";
 	Druidmap[DRUID139].notes="created by Dr.\nMasternak to clean up\nlarge heaps of rubbish.\nIts large scoop is used to\ncollect rubbish. It is\nthen crushed internally.\n";
 	Druidmap[DRUID247].notes="light duty servant\nrobot. One of the first\nto use the anti-grav\nsystem.\n";
 	Druidmap[DRUID249].notes="cheaper version of\nthe anti-grav servant\nrobot.\n";
 	Druidmap[DRUID296].notes="this robot is used\nmainly for serving drinks.\nA tray is mounted on the\nhead. Built by Orchard\nand Marsden Enterprises.\n";
 	Druidmap[DRUID302].notes="common device\nfor moving small\npackages. Clamp is\nmounted on the lower\nbody.\n";
 	Druidmap[DRUID329].notes="early type\nmessenger robot. Large\nwheels impede motion on\nsmall craft.an";
 	Druidmap[DRUID420].notes="slow maintenance\nrobot. Confined to drive\nmaintenance during flight.\n";
 	Druidmap[DRUID476].notes="ship maintenance\nrobot. Fitted with\nmultiple arms to carry\nout repairs to the ship\nefficiently. All craft\nbuilt after the Jupiter‹incident are supplied\nwith a team of these.\n";
 	Druidmap[DRUID493].notes="slave maintenance\ndroid. Standard version\nwill carry its own\ntoolbox.\n";
 	Druidmap[DRUID516].notes="early crew droid.\nAble to carry out simple\nflight checks only. No\nlonger supplied.\n";
 	Druidmap[DRUID571].notes="standard crew\ndroid. Supplied with the\nship.\n";
 	Druidmap[DRUID598].notes="a highly\nsophisticated device.\nAble to control the\nRobo-Freighter on its\nown.\n";
 	Druidmap[DRUID614].notes="low security\nsentinel droid. Used to\nprotect areas of the ship\nfrom intruders. A slow\nbut sure device.\n";
 	Druidmap[DRUID615].notes="sophisticated\nsentinel droid. Only 2000\nbuilt by the Nicholson\nCompany. these are now\nvery rare.";
 	Druidmap[DRUID629].notes="low sentinel\ndroid. Lasers are built\ninto the turret. These\nare mounted on a small\ntank body. May be fitted\nwith an auto-cannon on‹the Gillen version.\n";
 	Druidmap[DRUID711].notes="heavy duty battle\ndroid. Disruptor is built\ninto the head. One of the\nfirst in service with the\nMilitary.\n";
 	Druidmap[DRUID742].notes="this version is\nthe one mainly used by\nthe Military.\n";
 	Druidmap[DRUID751].notes="very heavy duty\nbattle droid. Only a few\nhave so far entered\nservice. These are the\nmost powerful battle\nunits ever built.\n";
 	Druidmap[DRUID821].notes="a very reliable\nanti-grav unit is fitted\ninto this droid. It will\npatrol the ship and\neliminate intruders as\nsoon as detected by‹powerful sensors.\n";
 	Druidmap[DRUID834].notes="early type\nanti-grav security droid.\nFitted with an\nover-driven anti-grav unit.\nThis droid is very fast\nbut is not reliable.\n";
 	Druidmap[DRUID883].notes="this droid was\ndesigned from archive\ndata. For some unknown\nreason it instils great\nfear in Human\nadversaries.\n";
 	Druidmap[DRUID999].notes="experimental\ncommand cyborg. Fitted\nwith a new tipe of\nbrain. Mounted on a\nsecurity droid anti-grav\nunit for convenience.‹warning: the influence\ndevice may not control a\nprimode brain for long.\n"; 
 	

	IntroMSG1="Dies symbolisiert den Text, der zu Beginn des Spiels ausgegeben werden soll. Nach einer gewissen Zeit soll der Text nach unten weiterscrolloen und zwar in einer moeglichst fliessenden Form.";
	
	GameAdapterPresent=FALSE;		/* start with this */
	taste=255;

	/* Sounds on/off */
	ModPlayerOn = FALSE;
	
	
	/* ScreenPointer setzen */
	RealScreen = MK_FP(SCREENADDRESS, 0);
	InternalScreen = (unsigned char*)MyMalloc(SCREENHOEHE*SCREENBREITE);

	/* Zufallsgenerator initialisieren */
	randomize();
		
	if( LoadShip(SHIPNAME) == ERR) {
		printf("Error in LoadShip");
		Terminate(-1);
	}
	
	/* Now fill the pictures correctly to the structs */
	if (!InitPictures()) {	/* Fehler aufgetreten */
		return;
	}

	/* Init the Takeover- Game */
	InitTakeover();


	/* Die Zahlen, mit denen die Robotkennungen erzeugt werden einlesen */
	GetDigits(); 


	/* InternWindow uses InternalScreen : spares memory */
	InternWindow = InternalScreen;		/* use this, its big eneough */

	/* eigenen Zeichensatz installieren */
	LadeZeichensatz(DATA70ZEICHENSATZ);

	// Initialisieren der Schildbilder
	GetShieldBlocks();

	/* verbiegen der Interrupts */
#if USEINTSOFF == 0
	TurnIntVects();
#endif

	/* richtige Paletten-Werte einstellen */
	InitPalette();

	/* Tastaturwiederholrate auf den geringsten Wert setzen */
	SetTypematicRate(TYPEMATIC_SLOW);

	/* Initialisierung beendet. Monitor wird aktiviert. */
}

/*@Function============================================================
@Desc: 	int InitPictures(void):
				get the pics for: druids, bullets, blasts
				
				reads all blocks and puts the right pointers into
				the various structs

@Ret: FALSE: ERROR  	TRUE: OK
@Int:
* $Function----------------------------------------------------------*/
int InitPictures(void) {

	int i;
	char *druidpics;
	char *tmp;
	int druidmem;
	int picts;

	/* First read the map blocks */
	GetMapBlocks();

	/* Get the enemy-blocks */
	GetBlocks(ENEMYBILD, 0, 0);
	Enemypointer = GetBlocks(NULL, 0, ENEMYPHASES);

	/* Get the influence-blocks */
	GetBlocks(INFLUENCEBILD, 0, 0);
	Influencepointer = GetBlocks(NULL, 0, ENEMYPHASES);
	
	/* the same game for the bullets */
	GetBlocks(BULLETBILD, 0, 0);
	for (i=0; i<ALLBULLETTYPES; i++) {
		Bulletmap[i].picpointer = GetBlocks(NULL, i, Bulletmap[i].phases);
	}
		

	/* ...and the blasts */
	GetBlocks(BLASTBILD, 0, 0);
	for (i=0; i<ALLBLASTTYPES; i++) {
		Blastmap[i].picpointer = GetBlocks(NULL, i, Blastmap[i].phases);
	}

	/* Get the Frame */
	LadeLBMBild(RAHMENBILD1,InternalScreen,FALSE); 
	RahmenPicture = (unsigned char *)MyMalloc(RAHMENBREITE*RAHMENHOEHE+10);
	IsolateBlock(InternalScreen, RahmenPicture, 0, 0, RAHMENBREITE, RAHMENHOEHE);

	/* get the Elevator-Blocks */
	ElevatorBlocks = (unsigned char*)MyMalloc(NUM_EL_BLOCKS*EL_BLOCK_MEM+100);
	LadeLBMBild(EL_BLOCKS_FILE,InternalScreen,FALSE);
	for( i=0; i<NUM_EL_BLOCKS; i++)
		IsolateBlock(
			InternalScreen, 
			ElevatorBlocks+i*EL_BLOCK_MEM,
			1+i*(EL_BLOCK_LEN+1), 1,
			EL_BLOCK_LEN, EL_BLOCK_HEIGHT);
		

	/* get Menublocks */
	LadeLBMBild(CONSOLENBILD,InternalScreen,FALSE);
	MenuItemPointer=MyMalloc(MENUITEMMEM);
	IsolateBlock(InternalScreen, MenuItemPointer, 0, 0, MENUITEMLENGTH, MENUITEMHEIGHT);	

	return TRUE;
}


/*@Function============================================================
@Desc: Diese Prozedur ist fuer die Introduction in das Spiel verantwortlich. Im
   Moment beschrÑnkt sich ihre Funktion auf das Laden und anzeigen eines
   Titelbildes, das dann ausgeblendet wird.

@Ret: keiner
@Int: keiner
* $Function----------------------------------------------------------*/
void Title(void)
{
	char LTaste;
	int i,j;
	int LX,LY;
	int ScrollEndLine = USERFENSTERPOSY;		/* Endpunkt des Scrollens */
	int OldUpdateStatus = InterruptInfolineUpdate;

	InterruptInfolineUpdate=FALSE;
#ifdef NOJUNKWHILEINIT
	Monitorsignalunterbrechung(0);
#endif

	LadeLBMBild(TITELBILD1,RealScreen,FALSE);	/* Titelbild laden */
	while (!SpacePressed) JoystickControl();
	
   FadeColors1();					/* Titelbild langsam ausblenden */
   for(i=0;i<9;i++) {
   	TimerFlag=FALSE;
	   while (!TimerFlag);
	}

	ClearGraphMem(RealScreen);
  	InitPalette();			/* This function writes into InternalScreen ! */
	ClearGraphMem(InternalScreen);
	DisplayRahmen(InternalScreen);
	SetTypematicRate(TYPEMATIC_FAST);
	
	SetTextBorder(USERFENSTERPOSX, USERFENSTERPOSY,
					USERFENSTERPOSX+USERFENSTERBREITE,
					USERFENSTERPOSY+USERFENSTERHOEHE,
					CHARSPERLINE );

 	SetTextColor(FONT_BLACK, FONT_RED);
	
// *		Auskommentiert zu Testzwecken
// *
   ScrollText(TitleText1, SCROLLSTARTX, SCROLLSTARTY, ScrollEndLine);
   ScrollText(TitleText2, SCROLLSTARTX, SCROLLSTARTY, ScrollEndLine);
   ScrollText(TitleText3, SCROLLSTARTX, SCROLLSTARTY, ScrollEndLine);

   SetTextBorder(0,0, SCREENBREITE, SCREENHOEHE, 40 );

	SetTypematicRate(TYPEMATIC_SLOW);
   InterruptInfolineUpdate=OldUpdateStatus;
}

/*@Function============================================================
@Desc: Diese Funktion ermittelt, ob irgend eine Richtungstaste gedrueckt ist

@Ret: wenn eine Richtungstaste gedrueckt ist FALSE
												ansonsten TRUE 
* $Function----------------------------------------------------------*/

int NoDirectionPressed(){
	if (DownPressed) return (0);
   if (UpPressed) return (0);
   if (LeftPressed) return (0);
   if (RightPressed) return (0);
	return (1);
}

/*@Function============================================================
@Desc: Diese Funktion Sprengt den Influencer und beendet das Programm

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void ThouArtDefeated(void){
	int i,j;

	Me.status=TERMINATED;
	ExplodeInfluencer();
		
	for (i=0;i<WAIT_AFTER_KILLED; i++) {
//		UpdateInfoline();
//		SetInfoline();
		DisplayRahmen(RealScreen);
		GetInternFenster();
		PutInternFenster();
		ExplodeBlasts();
		MoveBullets();
		MoveEnemys();
		for(j=0;j<MAXBULLETS;j++) CheckBulletCollisions(j);
		RotateBulletColor();
		delay(100);
	}

	/* Ein Flimmer zieht "uber den Schirm */
	Flimmern();
	Debriefing();

	/* Soundblaster soll keine Toene mehr spucken */
	sbfm_silence();

	GameOver = TRUE;
}

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void ThouArtVictorious(void){
	ShipEmptyCounter = WAIT_SHIPEMPTY;
	GameOver = TRUE;				  /*  */
	
	KillTastaturPuffer();
	ClearUserFenster();
	printf(" BRAVO ! Sie haben es geschafft ! ");
	getch();
}

/* **********************************************************************
	Diese Funktion updated die Highscoreliste
	**********************************************************************/
void Debriefing(void){
	char* Scoretext;
	HallElement* Oldptr;
	HallElement* Newptr;
	HallElement* SaveHallptr=Hallptr;
	char* PName;
	int DebriefColor;

	DebriefColor=FONT_WHITE;

	Me.status=DEBRIEFING;
	if (!PlusExtentionsOn) {
		Scoretext=MyMalloc(1000);
		if (RealScore>GreatScore) {
			SetUserfenster(DebriefColor,RealScreen); // KON_BG_COLOR
			SetTextColor(DebriefColor, KON_TEXT_COLOR); // KON_BG_COLOR
			strcpy(Scoretext,"\n    Great Score !\n Enter your name:");
			DisplayText(Scoretext,USERFENSTERPOSX,USERFENSTERPOSY,RealScreen,FALSE);
			free(GreatScoreName);
			GreatScoreName=GetString(18);
			GreatScore=RealScore;
		} else
		if (RealScore<LowestScoreOfDay) {
			SetUserfenster(DebriefColor,RealScreen); // KON_BG_COLOR
			SetTextColor(DebriefColor, KON_TEXT_COLOR); // KON_BG_COLOR
			strcpy(Scoretext,"\n   Lowest Score of Day! \n Enter your name:");
			DisplayText(Scoretext,USERFENSTERPOSX,USERFENSTERPOSY,RealScreen,FALSE);
			free(LowestName);
			LowestName=GetString(18);
			LowestScoreOfDay=RealScore;
		} else 
		if (RealScore>HighestScoreOfDay) {
			SetUserfenster(DebriefColor,RealScreen); // KON_BG_COLOR
			SetTextColor(DebriefColor, KON_TEXT_COLOR); // KON_BG_COLOR
			strcpy(Scoretext,"\n   Highest Score of Day! \n Enter your name:");
			DisplayText(Scoretext,USERFENSTERPOSX,USERFENSTERPOSY,RealScreen,FALSE);
			free(HighestName);
			HighestName=GetString(18);
			HighestScoreOfDay=RealScore;
		}
		free(Scoretext);
		
	} else {
		SaveHallptr=Hallptr;
		
		/* Wir brauchen keine Versager ! */
		if (RealScore == 0) return;
		/* Setzten der Umgebung */
		SetUserfenster(KON_BG_COLOR, RealScreen);
		SetTextColor(KON_BG_COLOR, KON_TEXT_COLOR);
		DisplayText(" You have gained entry to the hall\n of fame!\n\
Enter your name:\n  ",USERFENSTERPOSX,USERFENSTERPOSY,RealScreen,FALSE);

		/* Den neuen Eintrag in die Liste integrieren */
		if (Hallptr->PlayerScore < RealScore) {
			Oldptr=Hallptr;
			Hallptr=MyMalloc(sizeof(HallElement)+1);
			Hallptr->PlayerScore=RealScore;
			Hallptr->PlayerName=GetString(18);
			Hallptr->NextPlayer=Oldptr;
			SaveHallptr=Hallptr;
		} else {
			Oldptr=Hallptr;
			while (Hallptr->PlayerScore > RealScore) {
				Hallptr=Hallptr->NextPlayer;
				if (Hallptr->PlayerScore > RealScore) Oldptr=Oldptr->NextPlayer;
			}
			Newptr=MyMalloc(sizeof(HallElement)+1);
			Newptr->PlayerScore=RealScore;
			Newptr->PlayerName=GetString(18);
			Newptr->NextPlayer=Hallptr;
			Oldptr->NextPlayer=Newptr;
		}

		/* Message an exit */
		DisplayText("You are now added to the hall\n of fame!\n",
						USERFENSTERPOSX,USERFENSTERPOSY,RealScreen,FALSE);
		Hallptr=SaveHallptr;
		getch();
	}
}

/* **********************************************************************
	Diese Funktion gibt die momentane Highscoreliste aus
	Derweil ist sie noch im Textmodus.
	Wenn sie fertig ist, soll sie die Liste in Paraplusart nach oben
	scrollen.
	**********************************************************************/
void ShowHighscoreList(void){
	int j,k;
	int Rankcounter=0;
	HallElement* SaveHallptr=Hallptr;

	if (!PlusExtentionsOn) {
		printf(" Dies ist die aktuelle Highsoreliste:\n\n");
		printf(" Highest Score: %d :",HighestScoreOfDay);
		printf("%s\n",HighestName);
		printf(" Great Score: %d :",GreatScore);
		printf("%s\n",GreatScoreName);
		printf(" Lowest Score: %d :",LowestScoreOfDay);
		printf("%s\n",LowestName);
		getch();
	} else {
		printf(" This is today's Hall of Fame:\n\n");
		printf("\tRank\tName\tScore\n");
		while (Hallptr->NextPlayer != NULL) {
			printf("\t%d\t",Rankcounter);
			printf("%s",Hallptr->PlayerName);
			printf("\t%d\n",Hallptr->PlayerScore);
			Hallptr=Hallptr->NextPlayer;
			Rankcounter++;
		}
		getch();
	}
	Hallptr=SaveHallptr;
}


#undef _paraplus_c
/*=Header==============================================================
 * $Source$
 *
 * @Desc: the paraplus main program
 *	 
 * 	
 * $Revision$
 * $State$
 *
 * $Author$
 *
 * $Log$
 * Revision 1.6  1993/05/31 20:06:20  prix
 * Fri May 28 18:29:23 1993: heavy changes to new struct support
 * Fri May 28 18:47:01 1993: No Shield On/Off toggle possible at the moment !
 * Fri May 28 18:57:29 1993: ACHTUNG: DrawShieldToInfluence deaktiviert: geht so nicht !
 * Fri May 28 19:01:43 1993: Umstellungen auf neue structs
 * Fri May 28 19:39:47 1993: Shieldcolors-init auskommentiert
 * Fri May 28 21:22:29 1993: added init of phases -var in druid-structs
 * Fri May 28 21:44:46 1993: beginning to kill grob/fein-pos from Blasts
 * Sun May 30 10:20:58 1993: LoadMap routine aktiviert
 * Sun May 30 10:23:15 1993: CurLevel is now a pointer !: some changes necessary
 * Sun May 30 10:46:40 1993: anpassung an map als char *
 * Sun May 30 13:33:02 1993: added moving and support for vertical doors
 * Sun May 30 13:34:56 1993: NotPassable an vertical doors angepasst
 * Sun May 30 14:05:15 1993: inaktive Bullets und Blasts nun OUT, nicht 0. Nicht in PX !
 * Sun May 30 14:07:20 1993: BULLET/BLAST-ANZAHL to ALL-BULLETS/BLASTS
 * Sun May 30 15:52:00 1993: No Energy level
 * Sun May 30 16:14:15 1993: Mauern "reflektieren" jetzt den 001
 * Sun May 30 16:21:17 1993: NotPassable(): gets coordinates as argument now
 * Sun May 30 17:20:14 1993: Reibung verbessert: x und y gesondert
 * Sun May 30 20:33:54 1993: written InitPictures()
 * Mon May 31 14:01:31 1993: ShowBlock Aufrufe zum debuggen
 * Mon May 31 14:34:37 1993: InfluenceBlockPointer killed
 *
 * 
 * Revision 1.13  1994/05/21  16:27:15  prix
 * Fri May 20 14:11:58 1994: Plusmodusunterscheidung: PlusRahmen funktioniert
 * Sat May 21 09:18:57 1994: Cheatteleporter funktioniert
 * Sat May 21 09:32:55 1994: Cheatmenu erweitert
 *
 * Revision 1.12  1993/10/02  16:26:11  prix
 * Tue Aug 24 16:43:23 1993: call InitEnemys
 * Sun Aug 29 11:07:46 1993: added clock timing
 * Wed Sep 15 10:35:10 1993: Timerinterrupt entlastet
 * Wed Sep 15 14:45:10 1993: IntroMSG1 initialisiert
 * Fri Sep 17 15:49:44 1993: call to InitTakeover();
 * Sat Sep 18 10:41:53 1993: Load Enemy-Blocks in InitPictures, not in GetEnemyBox()
 * Sat Sep 18 10:46:53 1993: Init of enemy/influence pictures consistently in InitPictures
 * Sat Sep 18 11:32:25 1993: Rahmenpicture now in Mem: RahmenPicture
 * Sun Sep 19 07:31:07 1993: Bessere Werte f"ur die Schu"stypen
 * Sun Sep 19 07:34:26 1993: Bessere Werte f"ur die energy
 * Sun Sep 19 07:50:47 1993: Font wird getestet.
 * Sun Sep 19 07:52:43 1993: Schrift wird auch initialisiert
 * Sun Sep 19 08:43:44 1993: fade colors desaktiviert
 * Tue Sep 21 05:59:02 1993: laengere Intromessage
 * Tue Sep 21 06:06:45 1993: Starttext wird gescrollt
 * Tue Sep 21 14:17:11 1993: ClearScreen eingefuehrt
 * Thu Sep 23 11:01:57 1993: parameters of ScrollText are a little different now
 * Thu Sep 23 11:05:46 1993: including paratext.h now
 * Thu Sep 23 14:13:57 1993: RealScreen and InternalScreen now used here
 * Sat Sep 25 10:17:15 1993: written UpdateInfoline
 * Sat Sep 25 10:24:14 1993: added InfluenceModeNames
 * Sat Sep 25 10:25:27 1993: written SetInfoline()
 * Sat Sep 25 15:56:32 1993: ElevatorPicture is loaded only one time !!
 * Wed Sep 29 16:16:30 1993: MenuItems werden grafisch angezeigt
 * Thu Sep 30 13:56:36 1993: No pic-Pointer in Druimap any more
 * Fri Oct 01 19:42:24 1993: InitNewGameWritten
 * Sat Oct 02 12:24:28 1993: call to GetCrew() now in InitNewGame()
 *
 * Revision 1.11  1993/08/24  20:28:38  prix
 * Wed Aug 04 12:27:11 1993: exported all influencer-functions to influence.c
 * Wed Aug 04 12:39:45 1993: ausgemistet
 * Wed Aug 04 12:50:33 1993: removed call to RotateShieldColors
 * Wed Aug 04 12:57:27 1993: removed WallReflectShots
 * Sun Aug 08 12:57:06 1993: AlleLevelsgleichFaerben hinyugefuegt
 * Sun Aug 08 13:15:01 1993: Levelfaerbeprozeduren disloziert
 * Sun Aug 08 21:05:48 1993: changed Feindesliste to new struct enem
 * Mon Aug 09 17:19:37 1993: call to ShuffleEnemys in  InitGlobalVars()
 * Mon Aug 09 19:01:45 1993: more enemys
 * Tue Aug 10 08:19:14 1993: DRUIDBAST
 * Tue Aug 10 10:15:15 1993: InitLevelColorTable aktiviert
 * Tue Aug 10 20:08:56 1993: kein EnemysGetroffen() meht
 * Tue Aug 10 20:15:39 1993: No InfluenceGetroffen mehr
 * Fri Aug 13 14:20:03 1993: Toene aus beim Programmende
 * Sat Aug 21 14:58:13 1993: Call to new LoadShip
 * Tue Aug 24 09:52:22 1993:  call Animate Refresh
 * Tue Aug 24 10:25:48 1993: randomize()
 *
 * Revision 1.10  1993/08/04  15:13:22  prix
 * Fri Jul 30 18:26:16 1993: working on new MoveLevelDoors()
 * Fri Jul 30 18:57:29 1993: still working
 * Fri Jul 30 19:16:35 1993: First Release of New MoveLevelDoors
 * Sat Jul 31 07:25:14 1993: SetColors hinzugefuegt
 * Sat Jul 31 07:40:46 1993: farbwerte hinzugefuegt
 * Sat Jul 31 11:04:54 1993: Waypointliste - Array nun ausserhalb der Feindliste: Pointer
 * Sat Jul 31 11:22:27 1993: made HALBTUERE3 passable
 * Sat Jul 31 11:48:28 1993: changes in druidsped: init
 * Sat Jul 31 12:18:31 1993: adapts to new druid-struct: firewait in FireBullet
 * Sat Jul 31 18:30:11 1993: Speicher reservieren in Druidmap[0].type
 * Sat Jul 31 19:44:47 1993: added levelnum init in Feindliste
 * Sat Jul 31 19:48:38 1993: EnemyCollision only for living druid on current level
 * Sat Jul 31 20:25:37 1993: BounceBack stoppt nun Influencer statt zu reflektieren
 * Sat Jul 31 21:02:07 1993: removed brain-damaged shifts of Influence coordinates
 * Sat Jul 31 22:04:39 1993: New MoveLevelDoors: open for all druids
 * Sun Aug 01 08:23:53 1993: MoveLevelDoors: 0/0 beendet Funktion
 * Sun Aug 01 13:30:50 1993: dist - Werte in MoveLevelDoors long !!
 * Sun Aug 01 13:52:40 1993: rewriting NotPassable; exact checks
 * Sun Aug 01 14:59:31 1993: New NotPassable
 * Sun Aug 01 15:01:00 1993: Aufrufe von NotPassable an neue Paramter angepasst
 * Sun Aug 01 15:08:52 1993: mod. notPassable
 * Sun Aug 01 16:42:59 1993: New BounceBack
 * Mon Aug 02 09:26:30 1993: rotateBulletColor() aktiviert
 * Mon Aug 02 10:03:01 1993: schussgeschwindigkeit erhoeht
 * Tue Aug 03 12:34:41 1993: some unnescesary extras transfered into the file "plusjunk"
 * Tue Aug 03 13:52:28 1993: documentation and influence mortal
 * Tue Aug 03 14:33:11 1993: removed drive init of Druidmap (struct changed !)
 * Tue Aug 03 14:41:01 1993: NotPassable verbessert: Tuerraender
 * Tue Aug 03 14:49:22 1993: use of Me.firedir removed in MoveInfluence() und FireBullet
 * Tue Aug 03 14:51:13 1993: removed init of Me.firedir
 * Tue Aug 03 17:34:59 1993: BounceBack verbessert: schnelles Abgleiten an Mauer
 * Tue Aug 03 18:20:23 1993: written DoorCatch()
 * Tue Aug 03 18:22:59 1993: call to DoorCatch in CheckWallHit
 * Tue Aug 03 18:30:43 1993: killed CheckWallHit. New: BounceInfluencer
 * Tue Aug 03 19:41:52 1993: fighting with Pos-correction-functions (SHIITTTT)
 * Tue Aug 03 20:55:37 1993: written BounceInfluencer(), NotPassable() and DruidPassable()
 * Tue Aug 03 22:24:21 1993: changed DruidPassable and BounceInfluencer
 * Wed Aug 04 10:20:23 1993: added some lines to DruidPassable
 *
 * Revision 1.9  1993/07/30  22:19:13  prix
 * Wed Jul 28 20:58:33 1993: keine Phasen mehr bei den Bullets
 * Wed Jul 28 21:03:11 1993: phases ausradiert, Richtungen fuer Bullets
 * Wed Jul 28 21:07:08 1993: Phasen nicht erhoehen und neu beginnen bei Bullets
 * Thu Jul 29 07:26:01 1993: phases auf 3 gesetzt
 * Thu Jul 29 07:33:25 1993: Richtung fuer Bullets verwirklicht
 * Thu Jul 29 07:42:26 1993: warten wird dekrementiert
 * Thu Jul 29 10:11:31 1993: MemoryAmount jetzt voellig ohne Sicherheit
 * Thu Jul 29 12:21:15 1993: Bullet etwas versetzt abschiessen
 * Thu Jul 29 12:31:58 1993: Inlfuencer treffbar
 * Fri Jul 30 07:30:39 1993: Agression wird initialisiert
 * Fri Jul 30 07:47:26 1993: Defeated soll anschauricher sein
 * Fri Jul 30 11:58:57 1993: MoveLevelDoors:  now 6 phases !!
 * Fri Jul 30 18:15:16 1993: INFLUENCEGETROFFENOFF Schalter added
 *
 * Revision 1.8  1993/07/29  00:57:39  prix
 * Mon May 31 17:40:13 1993: InternWindow wieder ausgerichtet (voruebergehend !)
 * Mon May 31 17:42:22 1993: PutInternFenster ausgeschaltet (debug)
 * Mon May 31 17:45:14 1993: Bild wird direkt am Screen aufgebaut (zum testen !!)
 * Mon May 31 17:55:48 1993: InternWindow stimmt wieder
 * Mon May 31 17:57:44 1993: added init of ScreenPointer
 * Mon May 31 17:58:55 1993: PutInternFenster wieder aktiviert !
 * Mon May 31 20:08:52 1993: No DrawBulletDir..ToInfluence any more
 * Tue Jun 01 08:53:20 1993: Energie geht nicht beliebig in die Hoehe (bis maxenergy)
 * Tue Jun 01 09:37:29 1993: ExplodeBlasts an neue structs angepasst
 * Tue Jun 01 10:06:30 1993: New Blast has 7 phases
 * Tue Jun 01 11:56:07 1993: every bullet can have its own blast !
 * Tue Jun 01 11:59:51 1993: addit init of blast in Bulletmap (new !)
 * Tue Jun 01 12:15:57 1993: No ShowBlocks any more
 * Tue Jun 01 13:51:39 1993: added AnimateInfluence
 * Tue Jun 01 15:29:38 1993: andere Steuerung fuer Me.turnable == TRUE !
 * Tue Jun 01 15:40:55 1993: Drehung etwas bremsen, wenn man auf Taste bleibt
 * Tue Jun 01 15:55:10 1993: neuer Drive-Typ wird unterstuetzt: ROCKETS
 * Tue Jun 01 15:57:06 1993: 001 now has ROCKETS
 * Fri Jul 23 09:59:19 1993: Bewegung wieder urspruenglich
 * Fri Jul 23 10:03:32 1993: Versuch, die Schussrichtung zu restaurieren
 * Fri Jul 23 10:09:22 1993: Schussrichtung von Richtungsangebe abhaengig
 * Fri Jul 23 10:17:29 1993: Energiebalken wieder anzeigen
 * Fri Jul 23 10:22:48 1993: Energiehaushalt ueberpruefen und verstehen
 * Fri Jul 23 10:39:26 1993: Debug eingeschaltet
 * Fri Jul 23 10:43:59 1993: Debugswitch eingefuehrt
 * Fri Jul 23 11:09:02 1993: Anpassung von CheckWallHit an die verschobenen Y-Koordinaten
 * Fri Jul 23 11:22:53 1993: MoveLevelDoors an die veraenderte Me.pos.y-Variable angepasst
 * Fri Jul 23 11:56:57 1993: Drehung entsprechend der momentanenergie zu maxenergie
 * Fri Jul 23 12:20:05 1993: math library includiert
 * Sat Jul 24 06:43:58 1993: Weiterdrehung jetzt ohne Floatzahlen
 * Sat Jul 24 06:53:03 1993: Reactor ausgeschaltet, Standardenergielieferungen auch
 * Sat Jul 24 07:03:26 1993: Energieverlust in Abhaengigkeit des Robottypes realisiert
 * Sat Jul 24 07:15:25 1993: mathh wird nicht mehr benoetigt
 * Sat Jul 24 07:23:37 1993: GetDigits() eingefuehrt (als Namen)
 * Sat Jul 24 07:57:44 1993: Position von GetDigits veraendert
 * Sat Jul 24 11:11:17 1993: Inlfuencepointer eingefuehrt
 * Sat Jul 24 11:22:27 1993: RedrawInfluenceNumber wieCrd aufgerufen
 * Sat Jul 24 14:08:28 1993: Enemypointer hinzugefuegt
 * Sat Jul 24 14:23:21 1993: Feindesliste eingefuehrt
 * Sat Jul 24 14:29:41 1993: Enemymaxonlevel wird verwendet
 * Sat Jul 24 15:30:27 1993: GetEnemybox aktiviert !!!!! ich dodl
 * Sat Jul 24 16:57:50 1993: Feinde drehen sich gemaess ihrem energielevel
 * Sun Jul 25 08:08:00 1993: Feindesliste an die neue enemystruktur angepasst
 * Sun Jul 25 08:29:58 1993: MoveEnemys eingefuehrt
 * Sun Jul 25 12:50:26 1993: Endannaeherung und waypointweiterzaehlung realisiert
 * Sun Jul 25 14:10:38 1993: Robotter-Bullet-Kollisionen werden registriert
 * Sun Jul 25 15:15:58 1993: Feinde nicht bewegen wenn diese abgeschossen sind
 * Mon Jul 26 08:21:56 1993: giveBackMemory aktiviert
 * Mon Jul 26 09:49:03 1993: Bullets die treffen sind OUT
 * Mon Jul 26 12:56:50 1993: more notes added
 * Mon Jul 26 14:12:57 1993: Joystickabfrage eingefuehrt
 * Mon Jul 26 14:52:22 1993: Joystick kann abgeschaltet werden
 * Wed Jul 28 09:17:35 1993: New Testmap: authentic to paradroid on c64
 * Wed Jul 28 12:42:38 1993: JoystickControl eingefuehrt
 * Wed Jul 28 19:58:39 1993: CheckEnemyCollision aktiviert
 * Wed Jul 28 20:12:40 1993: Transfermode wird angezeigt
 * Wed Jul 28 20:27:38 1993: waitafter wird beruecksichtigt
 * Wed Jul 28 20:33:13 1993: Int1Ch wird auch verbogen
 * Wed Jul 28 20:44:36 1993: WEAPON als neuen Modus hinzuaddiert
 *
 * Revision 1.7  1993/05/31  20:28:27  prix
 * Mon May 31 16:07:42 1993: InternFenster wird nicht mehr auf off=0 ausgerichtet
 *
 * Revision 1.6  1993/05/31  20:06:20  prix
 * Fri May 28 18:29:23 1993: heavy changes to new struct support
 * Fri May 28 18:47:01 1993: No Shield On/Off toggle possible at the moment !
 * Fri May 28 18:57:29 1993: ACHTUNG: DrawShieldToInfluence deaktiviert: geht so nicht !
 * Fri May 28 19:01:43 1993: Umstellungen auf neue structs
 * Fri May 28 19:39:47 1993: Shieldcolors-init auskommentiert
 * Fri May 28 21:22:29 1993: added init of phases -var in druid-structs
 * Fri May 28 21:44:46 1993: beginning to kill grob/fein-pos from Blasts
 * Sun May 30 10:20:58 1993: LoadMap routine aktiviert
 * Sun May 30 10:23:15 1993: CurLevel is now a pointer !: some changes necessary
 * Sun May 30 10:46:40 1993: anpassung an map als char *
 * Sun May 30 13:33:02 1993: added moving and support for vertical doors
 * Sun May 30 13:34:56 1993: NotPassable an vertical doors angepasst
 * Sun May 30 14:05:15 1993: inaktive Bullets und Blasts nun OUT, nicht 0. Nicht in PX !
 * Sun May 30 14:07:20 1993: BULLET/BLAST-ANZAHL to ALL-BULLETS/BLASTS
 * Sun May 30 15:52:00 1993: No Energy level
 * Sun May 30 16:14:15 1993: Mauern "reflektieren" jetzt den 001
 * Sun May 30 16:21:17 1993: NotPassable(): gets coordinates as argument now
 * Sun May 30 17:20:14 1993: Reibung verbessert: x und y gesondert
 * Sun May 30 20:33:54 1993: written InitPictures()
 * Mon May 31 14:01:31 1993: ShowBlock Aufrufe zum debuggen
 * Mon May 31 14:34:37 1993: InfluenceBlockPointer killed
 *
 * Revision 1.5  1993/05/28  20:58:07  prix
 * Mon May 24 14:55:57 1993: General Note hinzugefuegt
 * Mon May 24 19:26:54 1993: Videomodus zuruecksetzen nach Beendung des Programms
 * Mon May 24 20:00:50 1993: AdjustSpeed rewritten to new data-structs
 * Mon May 24 20:06:35 1993: init of Druidmap
 * Tue May 25 07:22:49 1993: added init of Me
 * Tue May 25 14:15:28 1993: Wallchecks wiederbeleben (Anfang)
 * Tue May 25 14:24:13 1993: CheckWall respektiert nun offene Tueren als passierbar
 * Tue May 25 14:27:58 1993: Offsne Tueren werden jetzt als durchgaenglich behandelt
 * Tue May 25 14:30:30 1993: CalculateCL hinausgeworfen
 * Tue May 25 14:43:31 1993: Bullet auf Absolutkoordinaten umstellen (Alpha)
 * Thu May 27 09:36:45 1993: maxspeed verringert ->test
 * Fri May 28 16:49:47 1993: further adopts to new structs
 *
 * Revision 1.4  1993/05/23  21:04:04  prix
 * Sun May 23 11:02:50 1993: added CurLevel init to InitGlobalVars
 * Sun May 23 11:21:30 1993: modified MoveLevelDoors: removed multiple ifs and made a switch()
 * Sun May 23 11:47:59 1993: Level[][] replaced by CurLevel.map[][]
 * Sun May 23 12:18:41 1993: old Level removed completely
 * Sun May 23 13:14:37 1993: Umstellung auf neue Level-struct fertig
 *
 * Revision 1.3  1993/05/23  14:47:02  prix
 * Sat May 22 18:33:18 1993: SB-Initialisierung hizugefuegt
 * Sat May 22 19:28:26 1993: Bullet soll hoerbar sein
 * Sat May 22 19:58:48 1993: Energieblaken anzeigen
 * Sun May 23 07:40:54 1993: LaufRichtung begonnen
 * Sun May 23 08:07:22 1993: Laufanzeige sollte kunktionieren !
 * Sun May 23 08:27:11 1993: Schussrichtung wird korrekt angezeigt !
 * Sun May 23 08:55:40 1993: Standardenergieverbrauch eingefuehrt
 * Sun May 23 10:39:45 1993: Reactor eingefuehrt
 *
 * Revision 1.2  1993/05/23  12:28:33  prix
 * Die Richtung in die der Lauf zeigt wird angezeigt !
 *
 * Revision 1.1  1993/05/22  20:53:36  rp
 * Initial revision
 *
 *
 *-@Header------------------------------------------------------------*/
