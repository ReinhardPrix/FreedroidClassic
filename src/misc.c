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
 *
 *-@Header------------------------------------------------------------*/

// static const char RCSid[]=\
// "$Id$";

#define _misc_c

#undef DIAGONAL_KEYS_AUS
// #undef QUEUEDEBUG
#define QUEUEDEBUG

#define MESPOSX 0
#define MESPOSY 64
#define MESHOEHE 8
#define MESBARBREITE 320

#define MAX_MESSAGE_LEN 100
#define MESBAR_MEM	MESBARBREITE*MESHOEHE+1000

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vga.h>
#include <vgagl.h>
#include <vgakeyboard.h>

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

void CreateMessageBar(char* MText);
void CleanMessageLine(void);
void AdvanceQueue(void);

int VectsHaveBeenTurned=0;
unsigned char* MessageBar;
message* Queue=NULL;

// int ThisMessageTime=0;		/* Counter fuer Message-Timing */


/* **********************************************************************
	Diese Funktion l"oscht alle Roboter auf dem momentanen Level
	**********************************************************************/
void Armageddon(void){
  int i;
  char ATaste=' ';
	
  gl_printf(-1,-1,"\nKill all droids on ship (y/n) ?");
  while ( (ATaste!='y') && (ATaste!='n') ) ATaste=getchar();
  if (ATaste == 'n') return;
  else
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
  char *userInput;
  int vgamode;
  char NewRoboType[80]; // name of new influencer robot-type, i.e. "123"
  int Weiter=0;
  int LNum,X,Y,i,RN,dummy;
  int X0=20, Y0=5;   // startpos for gl_- text writing
  keyboard_close();  // return to normal keyboard operation

  //  gotoxy(1,1);
  //  vgamode = vga_getcurrentmode();
  //  vga_setmode(TEXT);

  // rp: try out the gl-textfunctions
  while (!Weiter) {
    vga_clear();
    gl_setfont(8, 8, gl_font8x8);
    gl_setwritemode(FONT_COMPRESSED + WRITEMODE_OVERWRITE);
    gl_setfontcolors(0, vga_white());

    gl_printf(X0,Y0, "*******************************\n");
    gl_printf(-1,-1, " ----  C H E A T M E N U  ----\n");
    gl_printf(-1,-1, "-------------------------------\n\n");
    gl_printf(-1,-1, " a. Armageddon (alle Robots sprengen)\n");
    gl_printf(-1,-1, " l. Robotliste von einem Deck\n");
    gl_printf(-1,-1, " g. Gesamtrobotliste\n");
    gl_printf(-1,-1, " r. Robotvernichtung dieses Deck\n");
    gl_printf(-1,-1, " t. Teleportation\n");
    gl_printf(-1,-1, " w. Wechsle Robo-typ\n\n");
    gl_printf(-1,-1, " i. Invinciblemode: %s\n",InvincibleMode?"ON":"OFF");
    gl_printf(-1,-1, " v. Volle Energie\n");
    gl_printf(-1,-1, " b. Blinkenergie\n");
    gl_printf(-1,-1, " c. Conceptview: %s\n",Conceptview?"ON":"OFF");
    gl_printf(-1,-1, " m. Map von Deck xy\n");
    gl_printf(-1,-1, "\n q. RESUME game\n");

    CTaste=getchar();
    switch (CTaste) {
    case 'a': Weiter=1; Armageddon(); break;
    case 'l': 
      vga_clear();
      gl_printf(X0,Y0,"NR.\tID\tX\tY\tENERGY.\n");
      for(i=0;i<MAX_ENEMYS_ON_SHIP;i++){
	if (Feindesliste[i].levelnum==CurLevel->levelnum)
	  gl_printf(-1,-1,"%d.\t%s\t%d\t%d\t%d.\n",i,
		    Druidmap[Feindesliste[i].type].druidname,
		    Feindesliste[i].pos.x, Feindesliste[i].pos.y,
		    Feindesliste[i].energy);
      }
      getchar();
      break;
    case 'r': 
      for(i=0;i<MAX_ENEMYS_ON_SHIP;i++){
	if (Feindesliste[i].levelnum==CurLevel->levelnum) Feindesliste[i].energy=0;
      }
      gl_printf(-1,-1,"All robots on this deck killed!");
      getchar();
      break;

    case 'g': 
      vga_clear();
      gl_printf(X0,Y0,"Nr.\tLev.\tID\tEnergy\n");
      for(i=0;i<MAX_ENEMYS_ON_SHIP;i++){
	gl_printf(-1,-1,"%d\t%d\t%s\t%d\n",
	       i,Feindesliste[i].levelnum,
	       Druidmap[Feindesliste[i].type].druidname,
	       Feindesliste[i].energy);
	if ((i%22)==0) {
	  gl_printf(-1,-1," --- MORE --- \n");
	  getchar();
	  vga_clear();
	  gl_printf(X0,Y0,"Nr.\tLev.\tID\tEnergy\n");
	}
      }
      break;

    case 't': 
      gl_printf(-1,-1,"\n Enter Levelnummer, X-Pos, Y-Pos: ");
      scanf("%d, %d, %d",&LNum,&X,&Y);
      getchar();  // remove the cr from input
      vga_clear();
      Teleport(LNum,X,Y);
      gl_printf(1,1,"This is your position on level %d.\n",LNum);
      gl_printf(-1,-1,"Press key to continue");
      getchar();
      break;

    case 'w': 
      gl_printf(-1,-1,"\nTypennummer ihres neuen robos: ");
      scanf("%s",NewRoboType);
      getchar();  // remove cr from input
      for(i=0; i<ALLDRUIDTYPES; i++) {
	if( !strcmp(Druidmap[i].druidname,NewRoboType) ) break;
      }
      if( i == ALLDRUIDTYPES ) {
	gl_printf(-1,-1,"\nUnrecognized robot-type: %s\n", NewRoboType);
	getchar();
      }  else {
	Me.type = i;
	Me.energy=Druidmap[Me.type].maxenergy;
	Me.health= STARTENERGIE; // Druidmap[OpponentType].maxenergy;
	gl_printf(-1,-1,"\nYou are now a %s. Have fun!\n", NewRoboType);
	getchar();
	RedrawInfluenceNumber();
      }
      break;
    
    case 'i': InvincibleMode = !InvincibleMode; break;
    case 'v': 
      Me.energy=Druidmap[Me.type].maxenergy; 
      Me.health=Me.energy;
      gl_printf(-1,-1,"\nSie sind wieder gesund!");
      getchar();
      break;
    case 'b': 
      Me.energy=1; 
      gl_printf(-1,-1,"\nSie sind jetzt ziemlich schwach!");
      getchar();
      break; 
    case 'c': Conceptview=!Conceptview; break;
    case 'm':
      gl_printf(-1,-1,"\nLevelnum:");
      scanf("%d", &LNum);
      ShowDeckMap(curShip.AllLevels[LNum]);
      break;
      
    case ' ': 
    case 'q':
      Weiter=1; break;
    }
  }
  ClearGraphMem(RealScreen);
  DisplayRahmen(RealScreen);
  InitBars=TRUE;

  vga_clear();
  keyboard_init();   // return to raw keyboard mode
  //  vga_setmode(vgamode);  // return to VGA mode
  
  return;
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

  //
  // THIS COMES IN FOR THE PORT AND IS NEW!
  //

  printf("\nvoid Terminate(int ExitStatus) wurde aufgerufen....\n");
  printf("GameOver : %i\n",GameOver);
  printf("Diese Meldung wurde durch PRINTF ausgegeben VOR dem Umschalten auf Textmode.\n");
  keyboard_close();
  vga_setmode(TEXT);
  printf("Diese Meldung wurde durch PRINTF ausgegeben NACH dem Umschalten auf Textmode.\n");
  exit(ExitCode);
  return;

  //
  // THIS WAS IN BEFORE THE PORT AND WILL NO LONGER BE EXECUTED,
  // EVEN IF IT IS NOT COMMENTED OUT
  //

  if (ExitCode) getchar();
  /* Interruptvektoren wieder restaurieren */
  RestoreIntVects();
  /* Soundblaster soll keine Toene mehr spucken */
  // PORT sbfm_silence();
#ifdef MODSCHASEIN	
  // PORT StopModPlayer();
#endif
  /* Videomodus wieder restaurieren */
  RestoreVideoMode();
  
  /* Tastaturwiederholung wieder schnell setzen */
  SetTypematicRate(TYPEMATIC_FAST);
  
  /* Zur"uck nach DOS */
  exit(ExitCode);
} // void Terminate(int ExitCode)


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
	
  printf("\nvoid AdvanceQueue(void): Funktion wurde echt aufgerufen.");

  if( Queue == NULL) return;
	
  if(Queue->MessageText) free(Queue->MessageText);
  tmp = Queue;
	
  Queue = Queue->NextMessage;

  free(tmp);

  printf("\nvoid AdvanceQueue(void): Funktion hat ihr natuerliches Ende erfolgreich erreicht....");
} // void AdvanceQueue(void)
	

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

  printf("\nvoid PutMessages(void): Funktion wurde echt aufgerufen.");

  //  if (!PlusExtentionsOn) return;
  
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
  printf("\nvoid PutMessages(void): This is the Queue of Messages:\n");
  while	(LQueue != NULL) {
    if ((LQueue->MessageText) == NULL) {
      printf("\nvoid PutMessages(void): ERROR: Textpointer is NULL !!!!!!\n");
      getchar();
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
    
    // 
    // SINCE 'REALSCREEN' NO LONGER RELEVANT, A NEW ROUTINE HAS TO BE FOUND FOR THE PORT
    //
    //    for(i=0;i<MESHOEHE;i++) {
    //      memcpy(
    //             RealScreen+MESPOSX+MESBARBREITE-MesPos+(MESPOSY+i)*SCREENBREITE,
    //	           MessageBar+i*MESBARBREITE,
    //             MesPos
    //            );
    //    } /* for */
    
    //
    // THIS IS THE NEW ROUTINE.
    //
    for(i=0;i<MESHOEHE;i++) {
      vga_drawscansegment(MessageBar+i*MESBARBREITE,MESPOSX-MesPos,MESPOSY+i,MesPos);
    }
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

  printf("\nvoid CreateMessageBar(char* MText): Funktion echt aufgerufen.");

  // "Uberl"angen checken
  if (strlen(MText)>40) {
    printf("\nvoid CreateMessageBar(char* MText): Message hat mehr als 40 Zeichen !.\n");
    getchar();
    Terminate(ERR);
  }
	
  // Speicher anfordern, wenn noch keiner da
  if (MessageBar == NULL)
    if((MessageBar=MyMalloc(MESBAR_MEM)) == NULL) {
      printf("\nvoid CreateMessageBar(char* MText): Bekomme keinen Speicher fuer MessageBar !!\n");
      getchar();
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

  printf("\nvoid CreateMessageBar(char* MText): Funktion hat ihr natuerliches Ende erreicht.");
}  // void CreateMessageBar(char* MText)

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

	if (!DMAUseON) return(memcpy(Ziel,Quelle,Laenge));
	else {
		printf("ERror: NO DMA usable");
		getchar();
		Terminate(-1);
		return(NULL);
	}

}

/* **********************************************************************
	Diese Funktion erledigt ein normales Malloc, trifft zuerst aber ein
	Par Sicherheitsvorkehrungen.
	**********************************************************************/
void* MyMalloc(long Mamount){
	void* Mptr=NULL;
	
	if ((Mptr=malloc((size_t)Mamount)) == NULL) {
		printf(" MyMalloc(%d) did not succeed!\n",Mamount);
		getchar();
	}
	return Mptr;
}
	
/* ************************************************************** *
 * * Diese Funktion setzt die Zeichenwiederholrate der Tastatur * *
 * * Es wird zuerst das Befehlswort 0x0F3 gesendet und dann die * *
 * * neue Rate (0,1 delay 2,3,4,5,6 typematic) nach 0x60 gesandt. *
 * ************************************************************** */

int SetTypematicRate(unsigned char Rate){
  //	outportb(0x60,0x0f3);
  //	delay(1); 
  //	outportb(0x60,Rate);
  //	delay(1);
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

	//	if (!GameAdapterPresent)
	//		return;
	//		
	//	asm{
	//		mov ah,84h
	//		mov dx,0
	//		int 15h
	//		jc NoJoy
	//		and al,00110000b
	//		mov cl,4
	//		shr al,cl
	//		xor al,00000011b
	//		mov ah,0
	//		mov JoyB,ax
	//	
	//		mov ah,84h
	//		mov dx,1
	//		int 15h
	//		jc NoJoy
	//		mov JoyX,ax
	//		mov JoyY,bx
	//		jmp AllOK
	//	}
	//NoJoy:
	//	gotoxy(4,4);
	//	printf(" No Joystick - kein Spieleadapter angeschlossen \n");
	//	getchar();
	//	GameAdapterPresent=FALSE;
	//
	//AllOK:
	//#ifdef SAYJOYPOS
	//	gotoxy(3,3);
	//	printf(" JoyX: %d JoyY: %d JoyB: %d !\n",JoyX,JoyY,JoyB);
	//#endif
	//
	//
	//	if ((SpacePressed) && (JoyB == FALSE)) SpaceReleased=TRUE;
	//	SpacePressed=JoyB;
	//	if (JoyX < 50) LeftPressed=TRUE; else LeftPressed=FALSE;
	//	if (JoyX >200) RightPressed=TRUE; else RightPressed=FALSE;
	//	if (JoyY < 50) UpPressed=TRUE; else UpPressed=FALSE;
	//	if (JoyY >200) DownPressed=TRUE; else DownPressed=FALSE;
	//
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
// void interrupt Interrupt09(void){

//	if( GameAdapterPresent ) return;
	
  //	KeyCode=inportb(0x60);
  //	switch (KeyCode){
  //		case 0x48 : UpPressed=1; break;
  //		case 0x50 : DownPressed=1; break;
  //		case 0x4B : LeftPressed=1; break;
  //		case 0x4D : RightPressed=1; break;
  //
  //#ifndef DIAGONAL_KEYS_AUS
  //		case 0x47 : LeftPressed=1; UpPressed=1; break;
  //		case 0x49 : RightPressed=1; UpPressed=1; break;
  //		case 0x51 : RightPressed=1; DownPressed=1; break;
  //		case 0x4F : LeftPressed=1; DownPressed=1; break;
  //
  //		case 0xC7 : LeftPressed=0; UpPressed=0; break;
  //		case 0xC9 : RightPressed=0; UpPressed=0; break;
  //		case 0xD1 : RightPressed=0; DownPressed=0; break;
  //		case 0xCF : LeftPressed=0; DownPressed=0; break;
  //#endif
  //		
  //		case 0xC8 : UpPressed=0; break;
  //		case 0xD0 : DownPressed=0; break;
  //		case 0xCB : LeftPressed=0; break;
  //		case 0xCD : RightPressed=0; break;
  //		case 0x39 : SpacePressed=1; break;
  //		case 0xB9 : { if (SpacePressed == TRUE) SpaceReleased=TRUE;
  //						  SpacePressed=0; break; }
  //		case 0x10 : QPressed=1; break;
  //		case 0x90 : QPressed=0; break;
  //	} /* switch */
  //
  //
  //	OldInt09h();
// }

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
//void interrupt Interrupt1C(void){
//	int i;
//	static unsigned char Palwert;

	//	TimerFlag = TRUE;
	//
	//	ThisMessageTime ++;			/* Message Timer */
	//	
	//	if (Me.firewait > 0) Me.firewait--;
	//	
	//	if ( ShipEmptyCounter > 1 ) ShipEmptyCounter --;
	//	if ( WaitElevatorCounter > 0) WaitElevatorCounter --;
	//	
	//	if ( CurLevel->empty > 2) CurLevel->empty--;
	//
	//	if(RealScore > ShowScore) ShowScore++;
	//	if(RealScore < ShowScore) ShowScore--;
	//
	//
	//	if (InterruptInfolineUpdate) {
	//		UpdateInfoline();
	//		SetInfoline();
	//	}

	
	/*
	 * Das Ausf"uhren des alten Interrupts ist nicht essentiell,
	 * h"alt aber schon zuvor installierte Interruptfunktionen
	 * am laufenden
	 */
	//	OldInt1Ch();
//}

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
//void interrupt Interrupt23(void){
//	GameOver=TRUE;
//	QuitProgram=TRUE;
////	OldInt23h();
//}

/*@Function============================================================
@Desc:  This function drains the keyboard buffer of characters to
			prevent nasty beeps when it is overloaded

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void KillTastaturPuffer(void){

  // PORT while (kbhit()) getchar();
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
//	if (!VectsHaveBeenTurned) {
//		OldInt09h=getvect(0x09);
//		OldInt1Ch=getvect(0x1C);
//		OldInt23h=getvect(0x23);
//		setvect(0x09,Interrupt09);
  //		setvect(0x1C,Interrupt1C);
  //		setvect(0x23,Interrupt23);
  //		VectsHaveBeenTurned=1;
  //	} else {
  //		printf(" Die Interruptvektoren waren ja schon verbogen !\n");
  //		getchar();
  //	}
}

/*@Function============================================================
@Desc: Diese Funktion restauriert die Interruptvektoren 1Ch und 09h. Sie
	wird bei Beendigung des Programms aufgerufen um einen Absturz zu
	verhindern.
@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void RestoreIntVects(void){
  //	if (VectsHaveBeenTurned) {
  //		setvect(0x09,OldInt09h);
  //		setvect(0x1C,OldInt1Ch);
  //		setvect(0x23,OldInt23h);
  // 	} else {
  //		printf(" Die Interruptvektoren waren noch gar nicht verbogen!\n");
  //		getchar();
  //	}
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

