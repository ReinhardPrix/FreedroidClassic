/*=@Header==============================================================
 * $Source$
 *
 * @Desc: the konsole- and elevator functions
 *	 
 * 	
 * $Revision$
 * $State$
 *
 * $Author$
 *
 * $Log$
 * Revision 1.3  2002/04/08 09:48:23  rp
 * Remaining modifs of the original version (which had not yet been checked in). Date: ~09/07/1994
 *
 * Revision 1.2  1994/06/19  16:40:20  prix
 * Sat May 21 12:41:50 1994: FadeLevel bei Elevatoreintritt
 * Sat May 21 16:11:52 1994: neue Aufrufparameter an DisplayText()
 * Tue Jun 14 10:34:17 1994: ClearUserFenster made global
 *
 * Revision 1.1  1993/08/07  19:14:45  prix
 * Initial revision
 *
 *
 *-@Header------------------------------------------------------------*/

static const char RCSid[]=\
"$Id$";

#define _ship_c
#include <process.h>
#include <stdio.h>
#include <conio.h>
#include <alloc.h>
#include <dos.h>
#include <math.h>
#include <mem.h>
#include <string.h>

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"
#include "paratext.h"
#include "ship.h"


int NoKeyPressed(void);

void ShowDeckMap(Level deck);
void GreatDruidShow(void); 

void ShowElevators(void);
void SetElColor(unsigned char *block, byte color);

void AlleElevatorsGleichFaerben(void);

void PaintConsoleMenu(void);

int WaitElevatorCounter = 0;


byte ElevatorRaster[ELEVATOR_HEIGHT][ELEVATOR_LEN] = {
	{ 0, 0, 0, 0, 0, 1, 5, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 2, 5, 5, 5, 5, 5, 5, 5, 0, 0, 1, 5, 5, 5, 5, 2, 5, 5, 5, 5, 6, 0, 0, 1, 7, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 4, 5, 2, 5, 5, 5, 5, 5, 5, 5, 5, 5, 2, 5, 5, 5, 5, 2, 5, 5, 5, 5, 5, 1, 5, 3, 5, 5, 5, 5, 6, 0, 0, 0, 0 },
	{12,12,12,12,12, 2,12,12,12, 1, 8, 8, 8, 8, 8, 8, 8, 9, 4, 5, 2, 5, 5, 5, 5, 5, 2, 5, 5, 5, 5, 5, 5, 5, 5, 6, 0, 0 },
	{12,12,12,12,12, 2,12,12,12, 2,11,11,11,11,11,11,11,11,11,11, 3, 1, 5, 5, 5, 5, 2, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6 },
	{12,12,12,12,12, 2,12,12,12, 2,10,10,10,10,10,10,10,10,10,10,10, 2, 4, 5, 5, 5, 2, 5, 5, 5, 5, 5, 5, 5, 5, 6, 0, 0 },
	{12,12,12,12,12, 2,12,12,12, 2, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 2, 4, 5, 5, 5, 2, 5, 5, 5, 5, 5, 5, 6, 0, 0, 0, 0 },
	{12,12,12,12,12, 2,12,12,12, 2,11,11,11,11,11,11,11,11,11,11,11, 2, 4, 1, 5, 5, 3, 5, 5, 6, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 4, 5, 5, 5, 5, 2, 5, 5, 5, 2,10,10,10,10,10,10,10,10,10,10,10, 2, 4, 2, 5, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 4, 5, 5, 5, 5, 3, 5, 5, 5, 2, 8, 8, 8, 8, 8, 8, 8, 9,15, 8, 8, 2, 4, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,11,11,11,11,11,11,11,13,16,10,10, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,10,10,10,10,10,10,10,14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};


byte ElevatorColors[ELEVATOR_HEIGHT][ELEVATOR_LEN] = {
	{ 0, 0, 0, 0, 0,18,10,10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,10,21,10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0,18,11,11,11,11,11,11,11, 0, 0,20, 9, 9, 9, 9,21, 9, 9, 9, 9, 9, 0, 0,25, 9, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0,12,12,18,12,12,12,12,12,12,12,12,12,20, 8, 8, 8, 8,21, 8, 8, 8, 8, 8,24, 8,25, 8, 8, 8, 8, 8, 0, 0, 0, 0 },
	{17,17,17,17,17,18,17,17,17,19,13,13,13,13,13,13,13,13, 7, 7,21, 7, 7, 7, 7, 7,24, 7, 7, 7, 7, 7, 7, 7, 7, 7, 0, 0 },
	{17,17,17,17,17,18,17,17,17,19,13,13,13,13,13,13,13,13,13,13,21,22, 6, 6, 6, 6,24, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6 },
	{17,17,17,17,17,18,17,17,17,19,13,13,13,13,13,13,13,13,13,13,13,22, 5, 5, 5, 5,24, 5, 5, 5, 5, 5, 5, 5, 5, 5, 0, 0 },
	{17,17,17,17,17,18,17,17,17,19,14,14,14,14,14,14,14,14,14,14,14,22, 4, 4, 4, 4,24, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0 },
	{17,17,17,17,17,18,17,17,17,19,14,14,14,14,14,14,14,14,14,14,14,22, 3,23, 3, 3,24, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 2, 2, 2, 2, 2,18, 2, 2, 2,19,14,14,14,14,14,14,14,14,14,14,14,22, 2,23, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 1, 1, 1, 1, 1,18, 1, 1, 1,19,15,15,15,15,15,15,15,15,16,16,16,22, 1,23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0,19,15,15,15,15,15,15,15,15,16,16,16,22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0,19,15,15,15,15,15,15,15,15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};



/*@Function============================================================
@Desc: EnterElevator(): does all the work if the Influencer enters an
							elevator

@Ret: voidn
@Int:
* $Function----------------------------------------------------------*/
void EnterElevator(void)
{
	int i;
	int curLevel;
	int curElev, upElev, downElev, row;

	curLevel = CurLevel->levelnum;

	if( (curElev = GetCurrentElevator()) == -1) {
		gotoxy(2,2);
		printf("Elevator out of order, I'm so sorry !");
		return;
	}

	EnterElevatorSound();
	FadeLevel();
   
	upElev = curShip.AllElevators[curElev].up;
	downElev = curShip.AllElevators[curElev].down;
   
	row = curShip.AllElevators[curElev].elevator_row;

	ShowElevators();
	HilightElevator(row);
	
	/* Warten, bis User Feuer auslaesst */
	while( SpacePressed) JoystickControl();
	SpaceReleased = FALSE;

	while( !SpaceReleased ) {
		JoystickControl();		/* Falls vorhanden einlesen */
		if( kbhit() ) getch();		/* Tastatur-Buffer leeren */
	   
		if( UpPressed && !DownPressed ) 
			if( upElev != -1 ) { 	/* gibt es noch einen Lift hoeher ? */
				if( curShip.AllElevators[upElev].x == 99 ) {
					gotoxy(1,1);
					printf("Out of order, so sorry ..");
				} else {
				   
					downElev = curElev;
					curElev = upElev;
					curLevel = curShip.AllElevators[curElev].level;
					upElev = curShip.AllElevators[curElev].up;
				   
 					AlleLevelsGleichFaerben();
	 				HilightLevel(curLevel); 	/* highlight new level */
					   
					/* Warten, bis user Taste auslaesst */
					WaitElevatorCounter = WAIT_ELEVATOR;
					MoveElevatorSound();
					while( UpPressed && WaitElevatorCounter ) JoystickControl();
				}
			   
			} /* if uplevel */


		if( DownPressed && !UpPressed )
			if( downElev != -1 ) {	/* gibt es noch einen Lift tiefer ? */
				if( curShip.AllElevators[downElev].x == 99 ) {
						gotoxy(1,1);
						printf("Out of order, so sorry ..");
				} else {
					upElev = curElev;
					curElev = downElev;
					curLevel = curShip.AllElevators[curElev].level;
					downElev = curShip.AllElevators[curElev].down;
   
					AlleLevelsGleichFaerben();
					HilightLevel(curLevel);
					
					/* Warten, bis User Taste auslaesst */
					WaitElevatorCounter = WAIT_ELEVATOR;
					MoveElevatorSound();
					while( DownPressed && WaitElevatorCounter ) JoystickControl();
				}
			   
			} /* if downlevel */

	} /* while !SpaceReleased */

	/* Neuen Level und Position setzen */
	if( curLevel != CurLevel->levelnum )	{ /* wirklich neu ??? */
		int array_num=0;
		Level tmp;
	   
		/* Aktuellen Level setzen */
		while( (tmp=curShip.AllLevels[array_num]) != NULL) {
			if( tmp->levelnum == curLevel ) break;
			else array_num ++;
		}
   
		CurLevel = curShip.AllLevels[array_num];

		/* Enemys gut verteilen: */
		ShuffleEnemys();

		/* Position des Influencer richtig setzen */
		Me.pos.x = curShip.AllElevators[curElev].x * BLOCKBREITE + BLOCKBREITE/2;
		Me.pos.y = curShip.AllElevators[curElev].y * BLOCKHOEHE + BLOCKHOEHE/2;

		/* Alle Blasts und Bullets loeschen */
		for( i=0; i<MAXBLASTS; i++) AllBlasts[i].type = OUT;
		for( i=0; i<MAXBULLETS; i++) {
			AllBullets[i].type = OUT;
			AllBullets[i].mine = FALSE;
		}
	   
	} /* if neuer Level */
	   
	LeaveElevatorSound();
	ClearGraphMem(RealScreen);
	DisplayRahmen(RealScreen);

	UnfadeLevel();

	/* Wenn Level leer: grau faerben */
	if( CurLevel->empty ) LevelGrauFaerben();

	InitBars=TRUE;
	   
} /* EnterElevator */

/*@Function============================================================
@Desc: ShowElevators(): 	gibt Seitenansicht des Schiffs aus

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void ShowElevators(void)
{
	int row, col;
	byte block;
	byte color;
	unsigned char *BlockPt;
	int StartX, StartY;
	int x, y;
	int curLevel = CurLevel->levelnum;

	if (!PlusExtentionsOn) {
	StartX = USERFENSTERPOSX;
	StartY = USERFENSTERPOSY + EL_BLOCK_HEIGHT + 5;

	/* Zuerst Screen loeschen (InternalScreen) */
	ClearGraphMem(InternalScreen);

	/* Userfenster faerben */
	SetUserfenster(EL_BG_COLOR, InternalScreen);
   
	y = StartY;
	for( row = 0; row < ELEVATOR_HEIGHT; row ++ ) {
		x = StartX;
		y += EL_BLOCK_HEIGHT;
		for( col = 0; col < ELEVATOR_LEN; col++ ) {	   
			block = ElevatorRaster[row][col];
			if( block != 0 ) {
				BlockPt = ElevatorBlocks+block*EL_BLOCK_MEM;
				color = EL_STARTCOLOR + ElevatorColors[row][col];
				SetElColor(BlockPt, color);
			   
				DisplayBlock(
					x, y,
					BlockPt,
					EL_BLOCK_LEN, EL_BLOCK_HEIGHT,
					InternalScreen);
			} /* if block */

			x += EL_BLOCK_LEN;
		   
		} /* for row */
	   
	} /* for col */

	DisplayRahmen(InternalScreen);		/* Rahmen dazu */
	SayLeftInfo(LeftInfo,InternalScreen);
	SayRightInfo(RightInfo,InternalScreen);
	AlleLevelsGleichFaerben();
	AlleElevatorsGleichFaerben();
	
	HilightLevel(curLevel);
	SwapScreen();								/* und anzeigen */
	return;
	}

	LadeLBMBild(SEITENANSICHTBILD,RealScreen,FALSE);
	AlleLevelsGleichFaerben();
	AlleElevatorsGleichFaerben();
	
	HilightLevel(curLevel);

	return;
} /* ShowElevators() */
   

/*@Function============================================================
@Desc: EnterKonsole(): does all konsole- duties

This function runs the consoles. This means the following duties:

	2	* Show a small-scale plan of the current deck

	3	* Show a side-elevation on the ship

	1	* Give all available data on lower druid types

	0	* Reenter the game without squashing the colortable

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void EnterKonsole(void)
{
	int MenuPoint=0;
	int ReenterGame=0;
	int i;
	int TasteOK;
	char LocalTaste;

	Me.status = CONSOLE;
	 
	/* Initialisierung der Konsole */

	InterruptInfolineUpdate = FALSE;
	ClearGraphMem(RealScreen);
	KillTastaturPuffer();

	/* Gleich zu Beginn die Farben richtig setzten */
	if (MenuPoint == 0) SetPalCol(M1C,HR,HG,HB);
	else SetPalCol(M1C,GR,GG,GB);
	if (MenuPoint == 1) SetPalCol(M2C,HR,HG,HB);
	else SetPalCol(M2C,GR,GG,GB);
	if (MenuPoint == 2) SetPalCol(M3C,HR,HG,HB);
	else SetPalCol(M3C,GR,GG,GB);
	if (MenuPoint == 3) SetPalCol(M4C,HR,HG,HB);
	else SetPalCol(M4C,GR,GG,GB);
	

	/* Gesamtkonsolenschleife */
	
	while (!ReenterGame) {

		PaintConsoleMenu();
		
		/* Nichts tun bis eine vern"unftige Taste gedr"uckt wurde */
		TasteOK=0;
		while (!TasteOK) {
			JoystickControl();
			if (UpPressed) {
				MenuPoint--;
				TasteOK=1;
			}
			if (DownPressed) {
				MenuPoint++;
				TasteOK=1;
			}
			if (SpacePressed) TasteOK=1;
		}
		
		/* Verhindern, da"s der Menucursor das Menu verl"a"st */		
		if (MenuPoint < 0) MenuPoint=0;
		if (MenuPoint > 3) MenuPoint=3;

		/* Anzeigen des aktuellen Menupunktes durch Palettenwertsetzen */
		if (MenuPoint == 0) SetPalCol(M1C,HR,HG,HB);
			else SetPalCol(M1C,GR,GG,GB);
		if (MenuPoint == 1) SetPalCol(M2C,HR,HG,HB);
			else SetPalCol(M2C,GR,GG,GB);
		if (MenuPoint == 2) SetPalCol(M3C,HR,HG,HB);
			else SetPalCol(M3C,GR,GG,GB);
		if (MenuPoint == 3) SetPalCol(M4C,HR,HG,HB);
			else SetPalCol(M4C,GR,GG,GB);

		/* gew"ahlte Menupunkte betreten */
		if ( (MenuPoint == 0) & (SpacePressed) ) ReenterGame=TRUE;
		if ( (MenuPoint == 1) & (SpacePressed) ) GreatDruidShow();
		if ( (MenuPoint == 2) & (SpacePressed) ) ShowDeckMap(CurLevel);
		if ( (MenuPoint == 3) & (SpacePressed) ) {
			while (SpacePressed) JoystickControl();
			ShowElevators();
			while (!SpacePressed) JoystickControl();
			while (SpacePressed) JoystickControl();
			KillTastaturPuffer();
		}

		while (DownPressed) JoystickControl();
		while (UpPressed) JoystickControl();

	} /* (while !ReenterGane) */
	
	KillTastaturPuffer();
	SpacePressed=FALSE;
	Me.status=MOBILE;
	/* Die Textfarben wieder setzen wie sie vorher waren */
	SetTextColor(FONT_WHITE,FONT_RED);  /* BG: Rahmenwei"s FG: FONT_RED */
	UpdateInfoline();
	InterruptInfolineUpdate = TRUE;
	InitBars=TRUE;
}



/*@Function============================================================
@Desc: diese Funktion zeigt die m"oglichen Auswahlpunkte des Menus
       Sie soll die Schriftfarben nicht ver"andern

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void PaintConsoleMenu(void){
	char MenuText[200];
	unsigned int fg,bg;

	ClearGraphMem(InternalScreen);
	GetTextColor(&bg,&fg);           /* Diese Funktion soll die Schrift-
													farben nicht ver"andern */
	
	/* Userfenster faerben */
   SetUserfenster(KON_BG_COLOR, InternalScreen);

	/* Darstellung des oberen Rahmen mit Inhalt */
//   SetInfoline();
//	strcpy(LeftInfo, "Console");
	DisplayRahmen(InternalScreen);
	SetTextColor(RAHMEN_BG_COLOR,FONT_RED);  /* BG: Rahmenwei"s FG: FONT_RED */
	SayLeftInfo(LeftInfo,InternalScreen);
	SayRightInfo(RightInfo,InternalScreen);
	
	/* Konsolen-Menue Farbe setzen */
	SetTextColor(KON_BG_COLOR, KON_TEXT_COLOR);
 
   strcpy(MenuText,"Unit type ");
   strcat(MenuText,Druidmap[Me.type].druidname);
   strcat(MenuText," - ");
   strcat(MenuText,Classname[Druidmap[Me.type].class]);
   DisplayText(MenuText,USERFENSTERPOSX,USERFENSTERPOSY,InternalScreen,FALSE);

   SetTextBorder(MENUTEXT_X, USERFENSTERPOSY,USERFENSTERPOSX+USERFENSTERBREITE,USERFENSTERPOSY+USERFENSTERHOEHE,30);
   
   strcpy(MenuText,"\nAccess granted.\nShip : ");
   strcat(MenuText,Shipnames[ThisShip]);
	strcat(MenuText,"\nDeck : ");
	strcat(MenuText,Decknames[CurLevel->levelnum]);
   strcat(MenuText,"\n\nAlert: ");
   strcat(MenuText,Alertcolor[Alert]);
   
   DisplayText(MenuText,MENUTEXT_X,USERFENSTERPOSY+15,InternalScreen,FALSE);


   SetTextBorder(0, 0, SCREENBREITE, SCREENHOEHE, 40);

   /*
    * Hier werden die Icons des Menus ausgegeben
    *
    */
    
   DisplayBlock(
      MENUITEMPOSX, MENUITEMPOSY + BLOCKHOEHE,
      MenuItemPointer,
      MENUITEMLENGTH, MENUITEMHEIGHT,
      InternalScreen);
      
   DisplayMergeBlock(
		MENUITEMPOSX+15, MENUITEMPOSY,
		Influencepointer + BLOCKMEM * Me.phase,
		BLOCKBREITE, BLOCKHOEHE,
		InternalScreen);

	SwapScreen();
	UpdateInfoline();
	SetTextColor(bg,fg);
}



/*@Function============================================================
@Desc: Zeigt alle erlaubten Roboter.

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void GreatDruidShow(void){
	char InfoText[200];
	int Infodroid;
	char PassOn=0;
   
	KillTastaturPuffer();	// Vorbeugung gegen vorzeigiges Verlassen

   SetUserfenster(KON_BG_COLOR, InternalScreen);
   SetTextColor(KON_BG_COLOR, FONT_BLUE);	// RED // YELLOW

	/*
    * Beginn der gro"sen "Ubersicht "uber alle Roboter
	 *
	 */

	while (SpacePressed) JoystickControl();
    
	for (Infodroid=Me.type;Infodroid > -1;) {

		ClearUserFenster();
		
RebeginSequence:;
		KillTastaturPuffer();
		
// Erste Seitet allein der anzeige der Nummer


		/*
		 * Ausgabe der ersten Zeile, die den Druidtyp beschreibt
		 *
		 */


		KillTastaturPuffer();
		PassOn=0;
		while (!PassOn) {

		// Am Bildschirm anzeigen
			strcpy(InfoText,"Unit type ");
			strcat(InfoText,Druidmap[Infodroid].druidname);
			strcat(InfoText," - ");
			strcat(InfoText,Classname[Druidmap[Infodroid].class]);
			
			SetTextBorder(USERFENSTERPOSX,USERFENSTERPOSY,USERFENSTERPOSX+
							  USERFENSTERBREITE,USERFENSTERPOSY+USERFENSTERHOEHE,36);
			DisplayText(InfoText,USERFENSTERPOSX,USERFENSTERPOSY,RealScreen,FALSE);
 		   ShowRobotPicture(USERFENSTERPOSX,USERFENSTERPOSY,Infodroid,RealScreen);

 		   // Einem zu schnellen Weiterbl"attern vorbeugen
 		   while (UpPressed || DownPressed) JoystickControl();
 		   if (!GameAdapterPresent) while (!kbhit());
 		   
 		// Eingabe behandeln
			JoystickControl();
			if (UpPressed) Infodroid--;
			if (DownPressed) Infodroid++;
			if ((RightPressed) || (LeftPressed)) PassOn=1;
			
			if (Infodroid > Me.type) Infodroid=DRUID001;
			if (Infodroid < DRUID001) Infodroid=Me.type;
			if (SpacePressed) {
				KillTastaturPuffer();
				while (SpacePressed) JoystickControl();
				SetTextBorder(0, 0, SCREENBREITE, SCREENHOEHE, 40);			
				return;
			}
			KillTastaturPuffer();
		}
		while (LeftPressed || UpPressed || DownPressed || RightPressed)
			JoystickControl();

		/*
		 * Ausgabe der Liste von Werten dieses Druids
		 *
		 */
		
		ClearUserFenster();
				
		strcpy(InfoText,"Entry : ");
		strcat(InfoText,Entry[Infodroid]);
		strcat(InfoText,"\nClass : ");
		strcat(InfoText,Classes[Druidmap[Infodroid].class]);
		strcat(InfoText,"\nHeight: ");
		strcat(InfoText,Height[Infodroid]);
		strcat(InfoText,"\nWeight: ");
		strcat(InfoText,Weight[Infodroid]);
		strcat(InfoText,"\nDrive : ");
		strcat(InfoText,Drivenames[Drive[Infodroid]]);
		strcat(InfoText,"\nBrain : ");
		strcat(InfoText,Brainnames[Brain[Infodroid]]);
		
		SetTextBorder(MENUTEXT_X,USERFENSTERPOSX,USERFENSTERPOSX+
						  USERFENSTERBREITE,USERFENSTERHOEHE+USERFENSTERPOSY,30);
		DisplayText(InfoText,MENUTEXT_X,USERFENSTERPOSY+17,RealScreen,FALSE);


  		KillTastaturPuffer();
		PassOn=0;
		while (!PassOn) {
			JoystickControl();
			if ((RightPressed) || (LeftPressed) || (UpPressed) || (DownPressed)) {
				PassOn=1;
				while ((RightPressed) || (LeftPressed) || (UpPressed) || (DownPressed))
					JoystickControl();
			}
			if (SpacePressed) {
				KillTastaturPuffer();
				while (SpacePressed) JoystickControl();
				SetTextBorder(0, 0, SCREENBREITE, SCREENHOEHE, 40);			
				return;
			}
		}


		/*
		 * Ausgabe der Liste von Ausr"ustung dieses Druids
		 *
		 */

		ClearUserFenster();
				
		strcpy(InfoText,"Armamant : ");
		strcat(InfoText,Weaponnames[Armament[Infodroid]]);
		strcat(InfoText,"\nSensors  1: ");
		strcat(InfoText,Sensornames[Sensor1[Infodroid]]);
		strcat(InfoText,"\n          2: ");
		strcat(InfoText,Sensornames[Sensor2[Infodroid]]);
		strcat(InfoText,"\n          3: ");
		strcat(InfoText,Sensornames[Sensor3[Infodroid]]);
		
		SetTextBorder(MENUTEXT_X,USERFENSTERPOSX,USERFENSTERPOSX+
						  USERFENSTERBREITE,USERFENSTERHOEHE+USERFENSTERPOSY,30);
		DisplayText(InfoText,MENUTEXT_X,USERFENSTERPOSY+17,RealScreen,FALSE);


  		KillTastaturPuffer();
		PassOn=0;
		while (!PassOn) {
			JoystickControl();
			if ((RightPressed) || (LeftPressed) || (UpPressed) || (DownPressed)) {
				PassOn=1;
				while ((RightPressed) || (LeftPressed) || (UpPressed) || (DownPressed))
					JoystickControl();
			}
			if (SpacePressed) {
				KillTastaturPuffer();
				while (SpacePressed) JoystickControl();
				SetTextBorder(0, 0, SCREENBREITE, SCREENHOEHE, 40);			
				return;
			}
		}

		
		/*
		 * Ausgabe der Informationen bezuglich des Druidhintergrundes
		 *
		 */
	
		ClearUserFenster();
 
		strcpy(InfoText,"Notes: ");
		strcat(InfoText,Druidmap[Infodroid].notes);
	
		SetTextBorder(MENUTEXT_X,USERFENSTERPOSX,USERFENSTERPOSX+USERFENSTERBREITE,USERFENSTERHOEHE+USERFENSTERPOSY,30);
		DisplayText(InfoText,MENUTEXT_X,USERFENSTERPOSY+17,RealScreen,FALSE);

  		KillTastaturPuffer();
		PassOn=0;
		while (!PassOn) {
			JoystickControl();
			if ((RightPressed) || (LeftPressed) || (UpPressed) || (DownPressed)) {
				PassOn=1;
				while ((RightPressed) || (LeftPressed) || (UpPressed) || (DownPressed))
					JoystickControl();
			}
			if (SpacePressed) {
				KillTastaturPuffer();
				while (SpacePressed) JoystickControl();
				SetTextBorder(0, 0, SCREENBREITE, SCREENHOEHE, 40);			
				return;
			}
		}
	}   /* for */
	
} /* GreatDruidShow() */


/*@Function============================================================
@Desc: Zeigt eine Verkleinerung des momentanen Decks auf dem RealScreen

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void ShowDeckMap(Level deck){
int i,j;
int LX;
int LY;


	// Darstellung der Miniaturkarte
	LX=USERFENSTERPOSX;
	LY=USERFENSTERPOSY;
	
	ClearUserFenster();
	for(i=0;i<deck->ylen;i++) {
		for(j=0;j<deck->xlen;j++) {
			SmallBlock(LX,LY,GetMapBrick(deck, j*BLOCKBREITE,i*BLOCKHOEHE),
				RealScreen,SCREENBREITE);
			LX+=8;
		}
		LX=USERFENSTERPOSX;
		LY+=8;
	}

	
	// Vorbeugung gegen vorzeitiges Verlassen
	KillTastaturPuffer();
	while (SpacePressed) JoystickControl();
	JoystickControl();


	while (!SpacePressed) JoystickControl();
	while (SpacePressed) JoystickControl();
}


/*@Function============================================================
@Desc: Diese Funktion dient dazu, wenn die Seitenansicht des Schiffes
	gezeigt wird, alle Levels gleich zu faerben

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void AlleLevelsGleichFaerben(void){
	static unsigned char* FarbFeldPointer=NULL;
	int i;
	unsigned char rot=0;
	unsigned char gruen=0;
	unsigned char blau=50;
	static unsigned int FOfs;
	static unsigned int FSeg;

	if (!FarbFeldPointer) {
		FarbFeldPointer=MyMalloc(32*3);
		if (!FarbFeldPointer) {
			printf(" Kein Speicher fuer AlleLev.");
			Terminate(-1);
		}
	}
   
	for (i=0;i<16;i++){
		FarbFeldPointer[i*3]=rot;
		FarbFeldPointer[i*3+1]=gruen;
		FarbFeldPointer[i*3+2]=blau;
	}
	FSeg=FP_SEG(FarbFeldPointer);
	FOfs=FP_OFF(FarbFeldPointer);

   
	asm{
		push es
		mov ax,1012h
		mov bx,EL_FIRSTCOLOR
		mov cx,16
		mov dx,FSeg
		mov es,dx
		mov dx,FOfs
		int 10h
		pop es
	}
}


/*@Function============================================================
@Desc: 	Alle Farben der Elevators in der Seitenansicht gleich setzen 


@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void AlleElevatorsGleichFaerben(void){
	static unsigned char* FarbFeldPointer=NULL;
	int i;
	unsigned char rot=0;
	unsigned char gruen=0;
	unsigned char blau=30;
	static unsigned int FOfs;
	static unsigned int FSeg;

	if (!FarbFeldPointer) {
		FarbFeldPointer=MyMalloc(32*3);
		if (!FarbFeldPointer) {
			printf(" Kein Speicher fuer AlleLev.");
			Terminate(-1);
		}
	}
   
	for (i=0;i<7;i++){
		FarbFeldPointer[i*3]=rot;
		FarbFeldPointer[i*3+1]=gruen;
		FarbFeldPointer[i*3+2]=blau;
	}
	FSeg=FP_SEG(FarbFeldPointer);
	FOfs=FP_OFF(FarbFeldPointer);

   
	asm{
		push es
		mov ax,1012h
		mov bx,EL_FIRST_ELEVATOR_COLOR
		mov cx,7
		mov dx,FSeg
		mov es,dx
		mov dx,FOfs
		int 10h
		pop es
	}
}

   


/*@Function============================================================
@Desc: Diese Funktion dient dazu, wenn die Seitenansicht des aktuellen
	Schiffes gezeigt ist, den momentanen Level als solchen zu kennzeichnen

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void HilightLevel(int Levelnummer){
	int rot=47;
	int gruen=63;
	int blau=33;

	SetPalCol(Levelnummer+EL_FIRSTCOLOR,rot,gruen,blau);
}

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void HilightElevator(int ElevatorRow){
	int rot=63;
	int gruen=63;
	int blau=63;

	SetPalCol(ElevatorRow+EL_FIRST_ELEVATOR_COLOR,rot,gruen,blau);
}

/*@Function============================================================
@Desc: SetElColor(unsigned char *block, int color):
				Setzt die EL_FIRSTCOLOR in block auf color

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void SetElColor(unsigned char *block, byte color)
{
	int counter;
	unsigned char *tmp = block;
   
	for( counter=0; counter < EL_BLOCK_HEIGHT*EL_BLOCK_LEN; counter ++, tmp++)
		if( (*tmp >= EL_FIRSTCOLOR) && (*tmp <= EL_LASTCOLOR) )
			*tmp = color;

	return;
}		   


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
int LevelEmpty(void){
	int i;
	int levelnum = CurLevel->levelnum;

	if( CurLevel->empty ) return TRUE;
	
	for (i=0;i<NumEnemys;i++){
		if ((Feindesliste[i].Status!=OUT) && (Feindesliste[i].levelnum==levelnum))
			return FALSE;
	}

	CurLevel->empty = TRUE;
	RealScore+=DECKCOMPLETEBONUS;
	ShowScore+=DECKCOMPLETEBONUS;
	
	if (ShipEmpty() ) ThouArtVictorious();
	
	return TRUE;
}

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/

int ShipEmpty(void){
	int i;

	for(i=0;i<ALLLEVELS;i++) {
		if( curShip.AllLevels[i] == NULL ) continue;
		
		if (! ((curShip.AllLevels[i])->empty) ) return (FALSE);
	}
	return (TRUE);
}


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
int NoKeyPressed(void){
	if (SpacePressed) return (FALSE);
	if (LeftPressed) return (FALSE);
	if (RightPressed) return (FALSE);
	if (UpPressed) return (FALSE);
	if (DownPressed) return (FALSE);
	return (TRUE);
}


/*@Function============================================================
@Desc: l"oscht das Userfenster

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void ClearUserFenster(void){
	int i;

	for (i=USERFENSTERPOSY;i<(USERFENSTERPOSY+USERFENSTERHOEHE);i++){
		setmem(RealScreen+i*SCREENBREITE+USERFENSTERPOSX,USERFENSTERBREITE,KON_BG_COLOR);
	}
}

#undef _ship_c
