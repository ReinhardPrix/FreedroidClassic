/*=@Header==============================================================
 * $Source$
 *
 * @Desc: miscellaeous helpful functions for paraplus
 *	 
 * 	
 * $Revision$
 * $State$
 *
 * $Author$
 *
 * $Log$
 * Revision 1.2  1994/06/19 16:25:08  prix
 * Sat May 21 14:27:44 1994: PutMessages written
 * Sat May 21 17:29:48 1994: Arbeit an PutMessages
 * Sat May 21 17:53:41 1994: Testversion (very very alpha
 * Sat May 21 18:03:46 1994: InsertMessage hinzugef"ugt
 * Tue Jun 14 10:16:33 1994: Terminate() is now in this module
 * Wed Jun 15 13:27:09 1994: CheatMenu transported to here
 *
 * Revision 1.1  1993/07/29  17:28:56  prix
 * Initial revision
 *
 *
 *-@Header------------------------------------------------------------*/

static const char RCSid[]=\
"$Id$";

#define _misc_c

#undef DIAGONAL_KEYS_AUS
#undef QUEUEDEBUG

#define MESPOSX 0
#define MESPOSY 64
#define MESHOEHE 8
#define MESBARBREITE 320

#define MAX_MESSAGE_LEN 100
#define MESBAR_MEM	MESBARBREITE*MESHOEHE+1000

#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <alloc.h>
#include <dos.h>
#include <mem.h>
#include <string.h>

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"

// Die Definition f"ur eine Message kann ruhig lokal hier stehen, da sie
// nur innerhalb des Moduls gebraucht wird.

typedef struct {
	void* NextMessage;
	int MessageCreated;
	char* MessageText;
}message, Message;

extern int ShipEmptyCounter;
extern int WaitElevatorCounter;
extern int TimerFlag;

void CreateMessageBar(char* MText);
void CleanMessageLine(void);
void AdvanceQueue(void);

int VectsHaveBeenTurned=0;
unsigned char* MessageBar;
message* Queue=NULL;

int ThisMessageTime=0;		/* Counter fuer Message-Timing */


/* **********************************************************************
	Diese Funktion l"oscht alle Roboter auf dem momentanen Level
	**********************************************************************/
void Armageddon(void){
	int i;
	char ATaste=' ';

	gotoxy(0,0);
	printf(" Really kill all droids on ship (y/n) ?");
	while ( (ATaste!='y') && (ATaste!='n') ) ATaste=getch();
	if (ATaste == 'n') return;
	for (i=0;i<MAX_ENEMYS_ON_SHIP;i++){
		Feindesliste[i].energy=0;
	}
}

/* **********************************************************************
	Diese Funktion teleportiert an einen beliebigen Ort im Schiff.
	Es werden Levelnummer und Grobkoordinaten "ubergeben.

	**********************************************************************/
	
void Teleport(int LNum,int X,int Y)
{
int curLevel=LNum;
		int array_num=0;
		Level tmp;
		int i;
	
	/* Neuen Level und Position setzen */
	
	if( curLevel != CurLevel->levelnum )	{ /* wirklich neu ??? */

		FadeLevel();
		/* Aktuellen Level setzen */
		while( (tmp=curShip.AllLevels[array_num]) != NULL) {
			if( tmp->levelnum == curLevel ) break;
			else array_num ++;
		}
   
		CurLevel = curShip.AllLevels[array_num];

		/* Enemys gut verteilen: */
		ShuffleEnemys();

		/* Position des Influencer richtig setzen */
		Me.pos.x = X * BLOCKBREITE + BLOCKBREITE/2;
		Me.pos.y = Y * BLOCKHOEHE + BLOCKHOEHE/2;

		/* Alle Blasts und Bullets loeschen */
		for( i=0; i<MAXBLASTS; i++) AllBlasts[i].type = OUT;
		for( i=0; i<MAXBULLETS; i++) {
			AllBullets[i].type = OUT;
			AllBullets[i].mine = FALSE;
		}
	} else {
		FadeLevel();
		Me.pos.x = X*BLOCKBREITE + BLOCKBREITE / 2;
		Me.pos.y = Y*BLOCKHOEHE + BLOCKHOEHE / 2;
	}
	   
	LeaveElevatorSound();
	BeamLine=BLOCKBREITE/2;
	UnfadeLevel();
}

/* **********************************************************************
   Diese Funktion stellt ein praktisches Cheatmenu zur Verf"ugung
   Es sind geplant:
		* Robotvernichtung
		* Teleportationen nach Belieben
		* Belibige Typenauswahl
		* Ausgabe einer Gesamtrobotliste
	**********************************************************************/
void Cheatmenu(void){
	char CTaste=' ';
	int Weiter=0;
	int LNum,X,Y,i,RN,dummy;

	gotoxy(1,1);
	printf("*******************************\n");
	printf(" ----  C H E A T M E N U  ----\n");
	printf("-------------------------------\n\n");
	printf(" a. Armageddon (alle Robots sprengen)\n");
	printf(" l. Robotliste von einem Deck\n");
	printf(" g. Gesamtrobotliste\n");
	printf(" r. Robotvernichtung dieses Deck\n");
	printf(" t. Teleportation\n");
	printf(" w. Neuer Wirt\n\n");
	printf(" i. Invinciblemode ");
 	if (InvincibleMode) printf("aus\n"); else printf("ein\n");
	printf(" v. Volle Energie\n");
	printf(" b. Blinkenergie\n");
//	printf(" x. SetDebugPos = TRUE\n");
	printf(" k. Keines der Punkte\n");
	printf(" p. Plusmodus\n");
	printf(" m. Send a Message\n");
	printf(" d. DMAUse auf %d\n",!DMAUseON);
	printf(" c. Conceptview auf %d\n",!Conceptview);
	printf(" e. Einstellungen aendern\n");
	printf(" f. Automatic Fire\n");
	while (!Weiter) {
		CTaste=getch();
		switch (CTaste) {
			case 'a': Weiter=1; Armageddon(); break;
			case 'l': {
				gotoxy(1,1);
				printf("NR.\tID\tX\tY\tENERGY.\n");
				for(i=0;i<MAX_ENEMYS_ON_SHIP;i++){
					if (Feindesliste[i].levelnum==CurLevel->levelnum)
						printf("%d.\t%s\t%d\t%d\t%d.\n",i,
							Druidmap[Feindesliste[i].type].druidname,
							Feindesliste[i].pos.x,
							Feindesliste[i].pos.y,
							Feindesliste[i].energy);
				}
				while (!SpacePressed) JoystickControl();
				Weiter=1;
				break;
			}
			case '3': Weiter=1; break;
			case 'r': {
				Weiter=1;
				for(i=0;i<MAX_ENEMYS_ON_SHIP;i++){
					if (Feindesliste[i].levelnum==CurLevel->levelnum) Feindesliste[i].energy=0;
				}
				break;
			}
			case 'g': {
				Weiter=1;
				gotoxy(1,1);
				printf("Nr.\tLev.\tID\tEnergy\n");
				for(i=0;i<MAX_ENEMYS_ON_SHIP;i++){
					printf("%d\t%d\t%s\t%d\n",i,Feindesliste[i].levelnum,Druidmap[Feindesliste[i].type].druidname,Feindesliste[i].energy);
					if ((i%22)==0) {
						printf(" --- MORE --- \n");
						getch();
					}
				}
				break;
			}
			case 't': {
				Weiter=1;
				printf(" Bitte Levelnummer, Grobwert X und Grobwert Y :");
				scanf("%d %d %d",&LNum,&X,&Y);
				Teleport(LNum,X,Y);
				break;
			}
			case 'w': {
				Weiter=1;
				printf(" Nummer (nicht der Name):");
				scanf("%d",&Me.type);
				Me.energy=Druidmap[Me.type].maxenergy;
				Me.health= STARTENERGIE; // Druidmap[OpponentType].maxenergy;
				RedrawInfluenceNumber();
				break;
			}
			case 'i': Weiter=1; InvincibleMode = !InvincibleMode; break;
			case 'v': Weiter=1; Me.energy=Druidmap[Me.type].maxenergy; Me.health=Me.energy; break;
			case 'b': Weiter=1; Me.energy=1; break; 
//			case 'x': SetDebugPos = TRUE; break;
			case 'k': Weiter=1; break;
			case 'p':{
				PlusExtentionsOn = !PlusExtentionsOn;
				if (PlusExtentionsOn) PlusRahmen(RealScreen);
				else {
					TransparentLadeLBMBild(RAHMENBILD1,InternalScreen,FALSE);
					DisplayRahmen(RealScreen);
				}
				Weiter=1;
				break;
			}
			case 'm': InsertNewMessage(); Weiter=1; break;
			case 'd': {
				DMAUseON=!DMAUseON;
				printf(" DMAUseON is now %d.\n",DMAUseON);
				Weiter=1;
				break;
			}
			case 'c': Conceptview=!Conceptview; Weiter=1; break;
			case 'f': Me.autofire=!Me.autofire; Weiter=1; break;
			case 'e': {
				gotoxy(1,1);
				printf("-----------------------------\n");
				printf("--- MENU ZUR KALIBRIERUNG ---\n");
				printf("-----------------------------\n");
				printf("s. Schussgeschwindigkeit\n");
				printf("h. Hoechstfahrt\n");
				printf("k. Keine Aenderung\n");
				printf("S. Schild fuer Me\n");
				printf("F. FCU fuer Me\n");
				printf("1. RO-Shield\n");
				printf("2. RU-Shield\n");
				printf("3. LU-Shield\n");
				printf("4. LO-Shield\n");
				while (!Weiter) {
					taste=getch();
					switch (taste) {
						case 'k': Weiter=1; break;
						case 'h': {
							printf("\nRobotnummer\n");
							scanf("%d",&RN);
							printf("\nWert\n");
							scanf("%d",&dummy);
							Druidmap[RN].maxspeed=(unsigned char)dummy;
							Weiter=1;
							break;
						}
						case 's': {
							printf("\nBullettyp\n");
							scanf("%d",&RN);
							printf("\nWert\n");
							scanf("%d",&dummy);
							Bulletmap[RN].speed=(unsigned char)dummy;
							Weiter=1;
							break;
						}
						case 'S': {
							printf("\nWert\n");
							scanf("%d",&dummy);
							Me.MyShield=dummy;
							Weiter=1;
							break;
						}
						case 'F': {
							printf("\nWert\n");
							scanf("%d",&dummy);
							Me.MyFCU=dummy;
							Weiter=1;
							break;
						}
						case '1': {
							if (!Me.Shield[0]) Me.Shield[0]=50; else Me.Shield[0]=0;
							Weiter=1;
							break;
						}
						case '2': {
							if (!Me.Shield[1]) Me.Shield[1]=50; else Me.Shield[1]=0;
							Weiter=1;
							break;
						}
						case '3': {
							if (!Me.Shield[2]) Me.Shield[2]=50; else Me.Shield[2]=0;
							Weiter=1;
							break;
						}
						case '4': {
							if (!Me.Shield[3]) Me.Shield[3]=50; else Me.Shield[3]=0;
							Weiter=1;
							break;
						}
						
					}
				}
				break;
			}
		}
	}
	ClearGraphMem(RealScreen);
	DisplayRahmen(RealScreen);
	InitBars=TRUE;
}


/*@Function============================================================
@Desc: Testfunktion fuer InsertMessage()

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void InsertNewMessage(void) {
	static int counter = 0;
	char testmessage[100];
	
	counter ++;
	sprintf(testmessage, "Das ist die %d .te Message !!", counter);
	InsertMessage(testmessage);

	return;
}

/*@Function============================================================
@Desc: 	Diese Funktion beendet das Programm und setzt alle notwendigen Dinge
	auf ihre Ausgangswerte zur"uck.
	Wenn ein ExitCode != 0 angegeben wurde wartet er noch auf eine Taste.

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void Terminate(int ExitCode){
	if (ExitCode) getch();
	/* Interruptvektoren wieder restaurieren */
	RestoreIntVects();
	/* Soundblaster soll keine Toene mehr spucken */
	sbfm_silence();
#ifdef MODSCHASEIN	
	StopModPlayer();
#endif
	/* Videomodus wieder restaurieren */
	RestoreVideoMode();
	
	/* Tastaturwiederholung wieder schnell setzen */
	SetTypematicRate(TYPEMATIC_FAST);
	
	/* Zur"uck nach DOS */
	exit(ExitCode);
}


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void KillQueue(void)
{
	while (Queue) AdvanceQueue();
}

/*@Function============================================================
@Desc: AdvanceQueue(): rueckt in der Queue eins vor und loescht Vorgaenger.

@Ret:
@Int:
* $Function----------------------------------------------------------*/
void AdvanceQueue(void) {
	message *tmp;
	
	if( Queue == NULL) return;
	
	if(Queue->MessageText) free(Queue->MessageText);
	tmp = Queue;

	Queue = Queue->NextMessage;

	free(tmp);
}
	

/*@Function============================================================
@Desc: Handles all Screenmessages

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void PutMessages(void)
{
	static int MesPos=0;				/* X-Position der Message-Bar */
	static int Working=FALSE;		/* wird gerade eine Message bearbeitet ?? */
	message* LQueue;					/* Bewegl. Queue-Pointer */
	int i;

	if (!PlusExtentionsOn) return;
	
	if (!Queue) return;						/* nichts liegt an */
	if (!Working) ThisMessageTime = 0;  /* inaktiv, aber Queue->reset time */
	
	
#ifdef QUEUEDEBUG
	gotoxy(1, 10);
	printf("Time: %d",ThisMessageTime);
#endif

// Ausgabe der momentanen Liste:
#ifdef QUEUEDEBUG
	LQueue=Queue;
	i=0;
	gotoxy(1,5);
	while	(LQueue != NULL) {
		if ((LQueue->MessageText) == NULL) {
			printf(" Textpointer is NULL !!!!!!\n");
			getch();
		}
		printf("%d. '%s' %d\n",i,LQueue->MessageText,LQueue->MessageCreated);
		i++;
		LQueue=LQueue->NextMessage;
	}
	printf(" NULL reached !\n");
#endif

	
	// Wenn die Nachricht schon sehr alt ist, wird sie gel"oscht. 
	if ( Working && (ThisMessageTime>MaxMessageTime) ) {
		AdvanceQueue();
		CleanMessageLine();
		Working = FALSE;			// inaktiv
		ThisMessageTime = 0;		// Counter init.
		return;
	}


	/* Alte Mes. hat MinTime gelebt, neue wartet */
	if((ThisMessageTime > MinMessageTime) && (Queue->NextMessage) ) {
		AdvanceQueue();		/* Queue weiterbewegen */
		Working = FALSE;		/* inaktiv setzen */
		ThisMessageTime = 0;	/* counter neu init. */
		return;
	}

	// Modul inaktiv und neue Message da --> aktivieren
	if( (!Working) && Queue ) {
		
		// Wenn die neue Nachricht noch nicht erzeugt ist, dann erzeuge sie
		if (!Queue->MessageCreated) {
			CreateMessageBar(Queue->MessageText);
			Queue->MessageCreated=TRUE;
		}

		ThisMessageTime = 0;			/* counter init. */
		CleanMessageLine();			/* Zeile loeschen */
		Working = TRUE;				/* aktiviert */
	}

	
	// Modul ist gerade aktiv --> bewegen und anzeigen
	if( Working  && Queue ) {
		
		MesPos=10*ThisMessageTime;		/* zeitl. synchronisierte Bewegung */

		/* nicht ueber linken Rand fahren !! */
		if (MesPos>(MESBARBREITE-2)) MesPos=MESBARBREITE-2;

		for(i=0;i<MESHOEHE;i++) {
			memcpy(RealScreen+MESPOSX+MESBARBREITE-MesPos+(MESPOSY+i)*SCREENBREITE,
				MessageBar+i*MESBARBREITE,MesPos);
		} /* for */
		
	} /* if aktiv + Message da */
		
} /* Put Messages */

/*@Function============================================================
@Desc: CleanMessageLine: l"oscht die Nachrichtenzeile am Bildschrim

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void CleanMessageLine(void)
{
	memset(RealScreen+MESPOSX+MESPOSY*SCREENBREITE,0,SCREENBREITE*MESHOEHE);
}

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void CreateMessageBar(char* MText)
{
char Worktext[42];
int i,j;


// "Uberl"angen checken
	if (strlen(MText)>40) {
		printf(" Message hat mehr als 40 Zeichen !.\n");
		getch();
		Terminate(ERR);
	}
	
// Speicher anfordern, wenn noch keiner da
	if (MessageBar == NULL)
		if((MessageBar=MyMalloc(MESBAR_MEM)) == NULL) {
			printf(" Bekomme keinen Speicher fuer MessageBar !!\n");
			getch();
			Terminate(ERR);
		}

// Message auf 40 Zeichen erg"anzen
	strcpy(Worktext,MText);
	while (strlen(Worktext)<40) strcat(Worktext," ");
	
// Im internen Screen die Nachricht anzeigen und dann ausschneiden
	for(i=0;i<40;i++){
		for(j=0;j<8;j++){
			memcpy( (MessageBar+i*8+j*SCREENBREITE),(Data70Pointer+Worktext[i]*8*8+j*8),8);
		}
	}

// Am unteren Rand die Nachricht anzeigen und dann ausschneiden
//	gotoxy(1,4);
//	printf("%s",MText);
//	memcpy(MessageBar,RealScreen+3*8*SCREENBREITE,8*SCREENBREITE);
}

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void InsertMessage(char* MText)
{
	message* LQueue=Queue;
	

	if( LQueue ) {
		// Bis vor die n"achste freie Position vorr"ucken
		while (LQueue->NextMessage != NULL) LQueue=LQueue->NextMessage;
		LQueue->NextMessage=MyMalloc(sizeof(message)+1);
		LQueue=LQueue->NextMessage;
	} else {
		Queue = MyMalloc(sizeof(message)+1);
		LQueue = Queue;
	}
	
	LQueue->MessageText=MyMalloc(MAX_MESSAGE_LEN+1);
	strcpy(LQueue->MessageText,MText);
	LQueue->NextMessage=NULL;
	LQueue->MessageCreated = FALSE;
#ifdef QUEUEDEBUG
	gotoxy(1,11);
	printf(" A message has been added:\n%s",MText);
#endif
}


/* **********************************************************************
	Diese Funktion kopiert ist ein "Aquivalent zu memset, kopiert jedoch
	"uber DMA-Zugriff.
	**********************************************************************/
void* MyMemcpy(void* Ziel,void* Quelle,unsigned int Laenge){
	unsigned int ZOfs,ZSeg,QOfs,QSeg;
	unsigned char PCFlag,DirFlag;
	unsigned char Error=0;

	if (!DMAUseON) return(memcpy(Ziel,Quelle,Laenge));
	else {
		ZOfs=FP_OFF(Ziel);
		ZSeg=FP_SEG(Ziel);
		QOfs=FP_OFF(Quelle);
		QSeg=FP_SEG(Quelle);
	
	asm{
		mov ax,ZSeg
		mov es,ax
		mov ax,QSeg
		mov ds,ax
		mov si,QOfs
		mov di,ZOfs
		mov cx,Laenge
		cld
		
;
; *************************************************************************
; * Hier beginnt die Funktion aus der DOS Internationa 3/91 Seite 281-282 *
; *************************************************************************
;

		push cx
		pushf
		pop ax
		; Flags nach AX
		and ah,4
		; Richtungsflag ausmaskieren
		mov cl,3
		shl ah,cl
		; Bit f"ur Modusregister verschieben
		mov DirFlag,ah
;		push ds
;		mov ax,0F000h
;		mov ds,ax
;		mov BYTE ptr PCFlag,-1
;		cmp BYTE ptr ds:[0FFFEh],0FCh
		; Computertyp: PC oder AT ?
;		jnz KeinAT
		mov BYTE ptr PCFlag,0
   }
KeinAT:;
   asm{
;		pop ds
		mov ax,ds
		inc cl
		shr ah,cl
		mov dh,ah
		; Segmentnr. des Quellblocks nach DH
		mov ax,ds
		shl ax,cl
		; restliche Segmentadresse
		add si,ax
		; auf Quell-Offsetadresse addieren
		adc dh,0
		; wenn "Ubertrag: Segmentnr. erh"ohen
		mov ax,es
		shr ah,cl
		mov dl,ah
		; Segmentnr. des Zielblocks nach DL
		mov ax,es
		shl ax,cl
		; restliche Segmentadresse
		add di,ax
		; auf Ziel-Offsetadresse addieren
		adc dl,0
		pop cx
		push cx
		; DMA-"Uberlauf ?
		add cx,si
		jnc DMAOk
		jmp DMAUeb
	}
DMAOk:;
	asm{
		pop cx
		push cx
		add cx,di
		jnc DMAOk1
		jmp DMAUeb
	}
DMAOk1:;
	asm{
		mov al,00000101b
		out 0Ah,al
		; Kanal 1 maskieren
		cli
		out 0Ch,al
		; High-Low-Flip-Flop l"oschen
		mov cx,di
		mov al,cl
		out 2,al
		mov al,ch
		out 2,al
		; Quelladresse "ubertragen
		mov al,dl
		out 83h,al
		; Segmentadresse setzen
		pop cx
		dec cx
		push cx
		mov al,cl
		out 3,al
		mov al,ch
		out 3,al
		; Anzal Bytes-1 setzen
		sti
		mov al,10000101b
		or al,DirFlag
		out 0Bh,al
		; Modus f"ur Kanal1 setzen
		mov al,00000001b
		out 0Ah,al
		; Maskierung f"ur Kanal 1 aufheben
		mov al,00000100b
		out 0Ah,al
		; Kanal 0 maskieren
		cli
		out 0Ch,al
		; High-Low-Flip-Flop l"oschen
		mov cx,si
		mov al,cl
		out 0,al
		mov al,ch
		out 0,al
		; Zieladresse "ubertragen
		mov al,dh
		mov dx,0083h
		cmp BYTE ptr PCFlag,-1
		jz PC
		mov dx,0087h
	}
PC:;
	asm{
		out dx,al
		; Page-Register Kanal 0 setzen
		pop cx
		mov al,cl
		out 1,al
		mov al,ch
		out 1,al
		; Anzahl Bytes-1 setzen
		sti
		mov al,10001000b
		or al,DirFlag
		out 0Bh,al
		; Modus f"ur Kanal 0 setzen
		mov al,1
		out 8,al
		; Kommando: Speicher zu Speicher
		mov al,00000100b
		out 9,al
		; DMA-Aktion (Kanal 0) starten
		; Daten"ubertragung l"auft !
   }
DMAWrk:;
   asm{
;		in al,8
		; Statusregister lesen
;		and al,00000011b
;		jz DMAWrk
		; bis "ubertragung zu Ende
		cmp BYTE ptr PCFlag,0
		; Computertyp=AT ?
		jz KeinRf
		; dann braucht Speicherauffrischung
		; nicht reaktiviert zu werden
		mov al,00000100b
		out 0Ah,al
		; Kanal 0 maskieren
		cli
		out 0Ch,al
		; High-Low-Flip-Flop l"oschen
		mov al,0FFh
		out 1,al
		out 1,al
		; Anzahl Bytes=0FFFFhex (64 KByte)
		sti
		mov al,01011000b
		out 0Bh,al
		; Modus f"ur Kanal 0 setzen
		xor al,al
		out 8,al
		; Kommando setzen
		out 0Ah,al
		; Maskierung f"ur Kanal 0 aufheben
   }
KeinRf:;
   asm{
		clc
		; Aktion erfolgreich beendet
		jmp DMAEnde
   }
DMAUeb:;
   asm{
		stc
		mov BYTE ptr Error,1
		; DMA-"Uberlauf !
	}
DMAEnde:;
		if (Error) return (NULL);
		else return (Ziel);

	} /* else */
}

/* **********************************************************************
	Diese Funktion erledigt ein normales Malloc, trifft zuerst aber ein
	Par Sicherheitsvorkehrungen.
	**********************************************************************/
void* MyMalloc(long Mamount){
	void* Mptr=NULL;
	
	if ((Mptr=malloc((size_t)Mamount)) == NULL) {
		printf(" MyMalloc(%d) did not succeed!\n",Mamount);
		getch();
	}
	return Mptr;
}
	
/* ************************************************************** *
 * * Diese Funktion setzt die Zeichenwiederholrate der Tastatur * *
 * * Es wird zuerst das Befehlswort 0x0F3 gesendet und dann die * *
 * * neue Rate (0,1 delay 2,3,4,5,6 typematic) nach 0x60 gesandt. *
 * ************************************************************** */

int SetTypematicRate(unsigned char Rate){
	outportb(0x60,0x0f3);
	delay(1); 
	outportb(0x60,Rate);
	delay(1);
	return 0;
}

/*@Function============================================================
@Desc: DirToVect
			int direction: a number representing a direction
			Vect vector: 	a pointer to a vector for the result
								(must be already allocated !)

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void DirToVect(int dir, Vect vector) {
	
	switch(dir) {
		case OBEN:
			vector->x = 0;
			vector->y = -1;
			break;
			
		case RECHTSOBEN:
			vector->x = 1;
			vector->y = -1;
			break;
			
		case RECHTS:
			vector->x = 1;
			vector->y = 0;
			break;
			
		case RECHTSUNTEN:
			vector->x = 1;
			vector->y = 1;
			break;
			
		case UNTEN:
			vector->x = 0;
			vector->y = 1;
			break;
			
		case LINKSUNTEN:
			vector->x = -1;
			vector->y = 1;
			break;
			
		case LINKS:
			vector->x = -1;
			vector->y = 0;
			break;
			
		case LINKSOBEN:
			vector->x = -1;
			vector->y = -1;
			break;
			
				
		default:
			printf("illegal direction in VectToDir() !");
			vector->x = vector->y = 0;
			return;
	} /* switch */

} /* DirToVect */


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void JoystickControl(void){
	int JoyB, JoyX, JoyY;

	if (!GameAdapterPresent)
		return;
		
	asm{
		mov ah,84h
		mov dx,0
		int 15h
		jc NoJoy
		and al,00110000b
		mov cl,4
		shr al,cl
		xor al,00000011b
		mov ah,0
		mov JoyB,ax
	
		mov ah,84h
		mov dx,1
		int 15h
		jc NoJoy
		mov JoyX,ax
		mov JoyY,bx
		jmp AllOK
	}
NoJoy:
	gotoxy(4,4);
	printf(" No Joystick - kein Spieleadapter angeschlossen \n");
	getch();
	GameAdapterPresent=FALSE;

AllOK:
#ifdef SAYJOYPOS
	gotoxy(3,3);
	printf(" JoyX: %d JoyY: %d JoyB: %d !\n",JoyX,JoyY,JoyB);
#endif


	if ((SpacePressed) && (JoyB == FALSE)) SpaceReleased=TRUE;
	SpacePressed=JoyB;
	if (JoyX < 50) LeftPressed=TRUE; else LeftPressed=FALSE;
	if (JoyX >200) RightPressed=TRUE; else RightPressed=FALSE;
	if (JoyY < 50) UpPressed=TRUE; else UpPressed=FALSE;
	if (JoyY >200) DownPressed=TRUE; else DownPressed=FALSE;

}



/*@Function============================================================
@Desc: Diese Funktion wird bei jedem Tastendruck durch den Interrupt-
	handler 09h aufgerufen.	Die Installstion erfolgt durch die Funktion
	SetIntVect(void) am Beginn des Programms.	Das Ausklinken aus dem
	Interrupt erfolgt am Ende des Programms durch die Funktion
	RestoreIntVects(void). Das Schluesselwort interrupt bei der Deklaration
	sowohl im file "proto.h" als auch hier ist notwendig, um den Compiler
	anzuweisen, den Inhalt aller Register zu sichern, da diese durch einen
	Interrupt auf keinen Fall veraendert werden duerfen.

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void interrupt Interrupt09(void){

//	if( GameAdapterPresent ) return;
	
	KeyCode=inportb(0x60);
	switch (KeyCode){
		case 0x48 : UpPressed=1; break;
		case 0x50 : DownPressed=1; break;
		case 0x4B : LeftPressed=1; break;
		case 0x4D : RightPressed=1; break;

#ifndef DIAGONAL_KEYS_AUS
		case 0x47 : LeftPressed=1; UpPressed=1; break;
		case 0x49 : RightPressed=1; UpPressed=1; break;
		case 0x51 : RightPressed=1; DownPressed=1; break;
		case 0x4F : LeftPressed=1; DownPressed=1; break;

		case 0xC7 : LeftPressed=0; UpPressed=0; break;
		case 0xC9 : RightPressed=0; UpPressed=0; break;
		case 0xD1 : RightPressed=0; DownPressed=0; break;
		case 0xCF : LeftPressed=0; DownPressed=0; break;
#endif
		
		case 0xC8 : UpPressed=0; break;
		case 0xD0 : DownPressed=0; break;
		case 0xCB : LeftPressed=0; break;
		case 0xCD : RightPressed=0; break;
		case 0x39 : SpacePressed=1; break;
		case 0xB9 : { if (SpacePressed == TRUE) SpaceReleased=TRUE;
						  SpacePressed=0; break; }
		case 0x10 : QPressed=1; break;
		case 0x90 : QPressed=0; break;
	} /* switch */


	OldInt09h();
}

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void interrupt Interrupt1C(void){
	int i;
//	static unsigned char Palwert;

	TimerFlag = TRUE;

	ThisMessageTime ++;			/* Message Timer */
	
	if (Me.firewait > 0) Me.firewait--;
	
	if ( ShipEmptyCounter > 1 ) ShipEmptyCounter --;
	if ( WaitElevatorCounter > 0) WaitElevatorCounter --;
	
	if ( CurLevel->empty > 2) CurLevel->empty--;

	if(RealScore > ShowScore) ShowScore++;
	if(RealScore < ShowScore) ShowScore--;


	if (InterruptInfolineUpdate) {
		UpdateInfoline();
		SetInfoline();
	}

	
	/*
	 * Das Ausf"uhren des alten Interrupts ist nicht essentiell,
	 * h"alt aber schon zuvor installierte Interruptfunktionen
	 * am laufenden
	 */
	OldInt1Ch();
}

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void interrupt Interrupt23(void){
	GameOver=TRUE;
	QuitProgram=TRUE;
//	OldInt23h();
}

/*@Function============================================================
@Desc:  This function drains the keyboard buffer of characters to
			prevent nasty beeps when it is overloaded

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void KillTastaturPuffer(void){
	while (kbhit()) getch();
}


/*@Function============================================================
@Desc: Diese Funktion haengt die Adressen der Funktionen Interrupt09(void)
	und Interrupt1C an die jeweiligen Interrupthandler im Biosvariablenbereich
	ab der Adresse 0000:0000 ein. Zuvor ist es notwendig, die alten Vektoren
	der Interrupts zu sichern, nicht nur, um sie bei Beendigung des Programms
	wieder restaurieren zu koennen sondern auch, um sie durch die neuen
	Interruptfuktionen auszufuehren, da es immerhin moeglich ist, das bereits
	andere residente Programme sich dort "hineigehaengt" haben.

	Nun ist auch eine Sicherheitsabfrage gegen doppeltes Verbiegen dazugekommen.
@Ret: keiner
@Int:
* $Function----------------------------------------------------------*/
void TurnIntVects(void){
	if (!VectsHaveBeenTurned) {
		OldInt09h=getvect(0x09);
		OldInt1Ch=getvect(0x1C);
		OldInt23h=getvect(0x23);
		setvect(0x09,Interrupt09);
		setvect(0x1C,Interrupt1C);
		setvect(0x23,Interrupt23);
		VectsHaveBeenTurned=1;
	} else {
		printf(" Die Interruptvektoren waren ja schon verbogen !\n");
		getch();
	}
}

/*@Function============================================================
@Desc: Diese Funktion restauriert die Interruptvektoren 1Ch und 09h. Sie
	wird bei Beendigung des Programms aufgerufen um einen Absturz zu
	verhindern.
@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void RestoreIntVects(void){
	if (VectsHaveBeenTurned) {
		setvect(0x09,OldInt09h);
		setvect(0x1C,OldInt1Ch);
		setvect(0x23,OldInt23h);
 	} else {
		printf(" Die Interruptvektoren waren noch gar nicht verbogen!\n");
		getch();
	}
}


/*@Function============================================================
@Desc: my_sqrt:		Quadrat-Wurzel

@Ret: int
@Int:
* $Function----------------------------------------------------------*/
long my_sqrt(long wert)
{
	long base = (long)wert -1;
	long x;
	long res;
	long tmp = wert;
	int prec = 0;		/* Groessenordnung der Zahl */
	long verschiebung=1;		/* Verschiebung zur Erhoehung der Genauigkeit */
	int counter;
	
	if( wert < 1 ) return (long)1;

	while( tmp /= 10 ) prec ++;
	prec = 4-prec;		/* verschiebe auf mind. 4 stellen */
	if( prec < 0 ) prec = 0;

	counter = prec;
	while( counter -- ) verschiebung *= 10;

	x = base;
	x *= verschiebung;
	
	res = verschiebung + x/2;
	x *= base;
	res -= x/8;
	x *= base;
	res += x/16;

	while( counter --) {
		if( (counter == 0) && ((res % 10) >= 5 )) res += 10;
		res /= 10;
	}

	
	return res;
}

/*@Function============================================================
@Desc: my_abs

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
int my_abs(int wert)
{
	return (wert < 0) ? -wert : wert;

}

/*@Function============================================================
@Desc: ShowDebugInfos()

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void ShowDebugInfos(void)
{
	gotoxy(0,0);
	printf("\nMe.energy: %d", Me.energy);
	printf("\nMe.pos: %d %d", Me.pos.x, Me.pos.y);
	return;
}
#undef _misc_c

