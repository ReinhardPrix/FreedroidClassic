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
 *-@Header------------------------------------------------------------*/
// static const char RCSid[]=\
// "$Id$";

#define _paraplus_c

/* *********************************************************************** */
/* ************  P R AE P R O Z E S S O R D I R E K T I V E N  ************ */
/* *********************************************************************** */

// Die folgenden Schalter schalten Debugmeldungen in gewissen speziellen Funktionen aus oder ein
#undef DEBUG_MYRANDOM

/* Schalter fuer die bedingte Kompilierung */
#undef DREHSCHUESSE
#undef SCHUSSDEBUG
#undef INFLUNCECLPOSDEBUG
#undef INFLUENCEWALLDEBUG
#undef DEBUGREPORT
#undef BULLETSPEEDUPONRICHTUNG

#undef SAYJOYPOS
#undef SPEICHERSPAREN

#define TITLE_EIN

#undef MODSCHASEIN
#undef NOJUNKWHILEINIT


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <vga.h>
#include <vgagl.h>
#include <vgakeyboard.h>
#include <sys/stat.h>
#include <sys/asoundlib.h>

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"
// That goes out for the PORT !!!! #include "fm_hrd.h"
#include "paratext.h"
#include "paravars.h"
#include "ship.h"

#define MOVEENEMYSOFF 		FALSE
#define ANIMATEENEMYSOFF 	FALSE
#define INFLUENCEGETROFFENOFF 	FALSE

#define FENSTEROFF 		FALSE
#define ENERGIEBALKENOFF 	TRUE
#define REDRAW001OFF 		FALSE
#define USEINTSOFF 		FALSE

/* Scroll- Fenster */
#define SCROLLSTARTX		USERFENSTERPOSX 
#define SCROLLSTARTY		SCREENHOEHE
#define CHARSPERLINE		(int)(USERFENSTERBREITE/FONTBREITE)

int AutoTerminationTime=60;
int SetDebugPos=FALSE;
int ThisMessageTime;

int card = 0;
int device = 0;
void* handle;

struct timeval now, oneframetimestamp, tenframetimestamp, onehundredframetimestamp, differenz;

long oneframedelay=0;
long tenframedelay=0;
long onehundredframedelay=0;
// float oneframedelay, tenframedelay, onehundredframedelay;
float FPSover1=10;
float FPSover10=10;
float FPSover100=10;
int framenr=0;

int TestSound(void);
void CalibratedDelay(long);
void Debriefing(void);
void ShowHighscoreList(void);
void Pause(void);

float Frame_Time(void){
  if (FPSover1 > 10) {
    return (1/FPSover1);
  } else {
    return (1/10);
  }
} // float Frame_Time(void)

void CalibratedDelay(long delay){
  usleep(delay);
} // void CalibatedDelay(long delay)


// Before the port to Linux, a (non-periodic) interrupt updated several global variables,
// that should reflect the keyboard status.  This was a rather crude solution, since some
// keystrokes tended to get missed if too many keys were pressed simulaneously.
//
// But now we've got the svgalib functionality anyway, which does a far better job:  The
// svgalib has certain functions to immediately report the status of a give key.  The old
// global variables could now be replaced by functions of the same name, that do not contain
// the status of the previous interrupt call, but that gain the information about the current
// status directly from the keyboard!  Long live the linux kernel and the svgalib!
//                                                                          jp, 10.04.2002
void ClearKbState(void) {
  keyboard_clearstate();  // This resets the state of all keys when keyboard in raw mode
} // void ClearKbState(void)

//
// This function is for stability while working with the SVGALIB, which otherwise would
// be inconvenient if not dangerous in the following respect:  When SVGALIB has switched to
// graphic mode and has grabbed the keyboard in raw mode and the program gets stuck, the 
// console will NOT be returned to normal, the keyboard will remain useless and login from
// outside and shutting down or reseting the console will be the only way to avoid a hard
// reset!
// Therefore this function is introduced.  When Paradroid starts up, the operating system is
// instructed to generate a signal ALARM after a specified time has passed.  This signal will
// be handled by this function, which in turn restores to console to normal and resets the
// yiff sound server access if applicable. (All this is done via calling Terminate of course.)
//                                                                           jp, 10.04.2002
//

static void timeout(int sig)
{
  printf("\n\nstatic void timeout(int sig): Automatic termination after %d seconds.\n\n", AutoTerminationTime);
  Terminate(0);
} // static void timeout(int sig)


// This Function is for the PORT!!!!
// Replacing all MyRandom-calls with MyMyRandom-calls

void reverse(char s[])
{
  int c,i,j;
  for(i=0,j=strlen(s)-1;i<j;i++,j--){
    c=s[i];
    s[i]=s[j];
    s[j]=c;
  }
} // void reverse(char s[]) siehe Kernighan&Ritchie!

// This Function is for the PORT!!!!
// Replacing all MyRandom-calls with MyMyRandom-calls

char* itoa(int n,char s[], int Dummy) {
  int i, sign;

  if ((sign=n)<0)
    n=-n;
  i=0;
  do {
    s[i++]=n%10+'0';
  } while ((n /= 10) > 0);
  if (sign < 0)
    s[i++]='-';
  s[i]='\0';
  reverse(s);
  return s;
} // void itoa(int n, char s[]) siehe Kernighan&Ritchie!

// This Function is for the PORT!!!!
// Replacing all MyRandom-calls with MyMyRandom-calls

char *ltoa(long n,char s[],int Dummy) {
  int i, sign;

  if ((sign=n)<0)
    n=-n;
  i=0;
  do {
    s[i++]=n%10+'0';
  } while ((n /= 10) > 0);
  if (sign < 0)
    s[i++]='-';
  s[i]='\0';
  reverse(s);
  return s;
} // void ltoa(long n, char s[]) angelehnt an itoa!

// This Function is for the PORT!!!!
// Replacing all MyRandom-calls with MyMyRandom-calls

void delay(int Dauer){

};

// This Function is for the PORT!!!!
// Replacing all MyRandom-calls with MyMyRandom-calls

void gotoxy(int x, int y){

};

// This Function is for the PORT!!!!
// Replacing all MyRandom-calls with MyMyRandom-calls

int kbhit(void){

};

// This Function is for the PORT!!!!
// Replacing all MyRandom-calls with MyMyRandom-calls

int MyRandom(int Obergrenze)
{
  float Zwisch;
  double ReinerZufall;
  int Endwert;


  //  printf("\nint MyRandom(int Obergrenze): Obergrenze ist jetzt: %d.",Obergrenze);
  ReinerZufall=(double)rand();
  //  printf("\nint MyRandom(int Obergrenze): Reiner Zufall ist jetzt: %f.",ReinerZufall);
  Zwisch=((float)ReinerZufall) / ((float)RAND_MAX);
  //  printf("\nint MyRandom(int Obergrenze): Zwisch ist jetzt: %f.",Zwisch);
  Zwisch=Zwisch*((float)Obergrenze);
  Endwert=(unsigned int)Zwisch;
#ifdef DEBUG_MYRANDOM
  printf("\nint MyRandom(int Obergrenze): Endwert ist jetzt: %d.",Endwert);
#endif
  return Endwert;
};





/*@Function============================================================
@Desc: main(): the heart of the Game 

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
int main(void)
{
  int i;

  GameOver = FALSE;
  QuitProgram = FALSE;
  Conceptview = FALSE;
  
  InterruptInfolineUpdate=TRUE;

  /* Initialisieren der globalen Variablen und Arrays */
  SaveVideoMode();
  InitParaplus();

#if REDRAW001OFF == 0
  RedrawInfluenceNumber();
#endif

  printf("void main(void): Vor Beginn der !QuitProgram - Schreife....\n");

  while( !QuitProgram ) {
		
    /* InitNewGame */
    InitNewGame();
	
    GameOver = FALSE;
		
    printf("void main(void): Vor Beginn der !GameOver && !QuitProgram - Schleife....\n");

    while (!GameOver && !QuitProgram) {

      framenr++; // This ensures, that 0 is never an encountered framenr, therefore count to 100 here
      // take the time now for calculating the frame rate (DO NOT MOVE THIS COMMAND PLEASE!)
      gettimeofday(&oneframetimestamp,NULL);
      if (  framenr %  10 == 1) gettimeofday(&tenframetimestamp,NULL); 
      if (  framenr % 100 == 1) gettimeofday(&onehundredframetimestamp,NULL);

      // Here are some things, that were previously done by some periodic interrupt function
      ThisMessageTime++;
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
      // This is the end of the things, that were previously done by periodic interrupt

	
      printf("void main(void): Innerhalb der !GameOver && !QuitProgram - Schleife....\n");

      keyboard_update();

      DisplayRahmen(RealScreen);

      // CalibratedDelay(20000);  //  This should cause a little delay to make the game more playable

      if(keyboard_keypressed(SCANCODE_Q)) {
	printf("\n*****************************************************");
	printf("\nvoid main(void): Termination cause of Q-Pressing!!!!!");
	printf("\n*****************************************************\n\n");
	Terminate(0);
      }
      if(keyboard_keypressed(SCANCODE_D)) Me.energy=0; 
      if(keyboard_keypressed(SCANCODE_L)) ShowHighscoreList();
      if(keyboard_keypressed(SCANCODE_I)) ShowDebugInfos();
      if(keyboard_keypressed(SCANCODE_V)) HideInvisibleMap = !HideInvisibleMap; 
      if(keyboard_keypressed(SCANCODE_C)) Cheatmenu();
      if(PPressed()) Pause();
      // } /* if while () */

      //PORT	if( !TimerFlag ) continue;		/* the clock - timing */
      //PORT	else TimerFlag = FALSE;

      if( ShipEmptyCounter == 1) GameOver = TRUE;

      LastBlastHit++;

      /*
       * Hier wird die Statuszeile ausgegeben
       *
       */

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

      GetInternFenster();
      PutInternFenster();

#ifdef PARADROID_SOUND_SUPPORT_ON
      YIFF_Server_Check_Events();
#endif

      UpdateInfoline();
      for (i=0;i<MAXBULLETS;i++) CheckBulletCollisions(i);
      PutMessages();

      printf("\nvoid main(void): PutMessages() ist zumindest ohne Probleme wieder zurueckgekehrt.");

      /* Wenn vorhanden: Joystick einlesen */
      JoystickControl();

      /* Gemaess den gedrueckten Tasten die Geschwindigkeit veraendern */
      MoveInfluence();


      MoveEnemys();			/* Auch die Feinde bewegen */


      /* Bei animierten Influencer die Phasen weiterzaehlen */
      AnimateInfluence();

      /* Bei den Feinden auch Phasen weiterzaehlen */

      AnimateEnemys();

      /* Raeder bremsen die Fahrt des Influencers erheblich */
      printf("\nvoid main(void): SpeedX ist jetzt: %d.",SpeedX);
      printf("\nvoid main(void): SpeedY ist jetzt: %d.",SpeedY);
      Reibung();

      /* Influencedruid nach der momentanen Geschwindigkeit weiterbewegen */
      printf("\nvoid main(void): SpeedX ist jetzt: %d.",SpeedX);
      printf("\nvoid main(void): SpeedY ist jetzt: %d.",SpeedY);
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

      // calculate the framerate:
      gettimeofday(&now,NULL);

      oneframedelay=(now.tv_usec-oneframetimestamp.tv_usec)+(now.tv_sec-oneframetimestamp.tv_sec)*1000000; 
      if (framenr % 10 == 0) 
	tenframedelay=((now.tv_usec-tenframetimestamp.tv_usec))+(now.tv_sec-tenframetimestamp.tv_sec)*1000000;
      if ((framenr % 100) == 0) {
	onehundredframedelay=(now.tv_sec-onehundredframetimestamp.tv_sec)*1000000+
	  (now.tv_usec-onehundredframetimestamp.tv_usec);
	framenr=0;
      }

      FPSover1  =1000000*  1 / (float)oneframedelay;
      FPSover10 =1000000* 10 / (float)tenframedelay;
      FPSover100=1000000*100 / (float)onehundredframedelay;
      // gl_printf(1,30,"   1fr: %d ms FPS1: %f \n",oneframedelay,FPSover1);
      // gl_printf(-1,-1," 10fr: %d ms FPS10: %f \n",tenframedelay,FPSover10);
      gl_printf(1,35,"100fr: %d ms FPS100: %f \n",onehundredframedelay,FPSover100);
      gl_printf(-1,-1,"Frame_Time(): %f \n",Frame_Time());
      // gl_printf(-1,-1,"sec : %d usec : %d \n",now.tv_sec,now.tv_usec);
      // gl_printf(-1,-1,"sec : %d usec : %d \n",onehundredframetimestamp.tv_sec,onehundredframetimestamp.tv_usec);
      // gl_printf(-1,-1,"sec : %d usec : %d \n",now.tv_sec-onehundredframetimestamp.tv_sec,now.tv_usec-onehundredframetimestamp.tv_usec);
      //      gl_printf(-1,-1,"sec : %d \n",onehundredframedelay);
      // gl_printf(-1,-1,"sec : %d \n",framenr % 100);

      // gl_printf(-1,-1,"%f\n",oneframetimestamp);
      //      gl_printf(-1,-1,ltoa((long)onehundredframedelay,Dummystring,10));
    } /* while !GameOver */
  } /* while !QuitProgram */
  Terminate(0);
} // void main(void)

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
  PlusExtentionsOn = FALSE;
  ThisMessageTime=0;
  
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


  i=MyRandom(4);
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
#ifdef TITLE_EIN
  Title();
#endif
	
  /* Farben des aktuellen Levels einstellen */
  SetLevelColor(CurLevel->color);
	
  LeftInfo[0] = '\0';
  RightInfo[0] = '\0';
	
  /* Den Rahmen fuer das Spiel anzeigen */
  ClearVGAScreen();
  DisplayRahmen(RealScreen);
  // DisplayRahmen(NULL);

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

  printf("void InitParaplus(void) wurde echt aufgerufen....\n");

  Set_SVGALIB_Video_ON(); 

  signal(SIGALRM,timeout);


  printf("\nvoid InitParaplus(void): An alarm signal will be issued in %d seconds, terminating the program for safety.", 
	 AutoTerminationTime);
  alarm(AutoTerminationTime);	/* Terminate after some seconds for safety. */

  // ******** ACHTUNG!  Hier folgt nun die Original-Initialisierungsroutine ***********

#ifdef PARADROID_SOUND_SUPPORT_ON
  Init_YIFF_Sound_Server();
#endif

  /* Unterbrechung des Monitorsignal solange Initialisierung l"auft. */
  // #ifdef NOJUNKWHILEINIT
  //  Monitorsignalunterbrechung(1);
  // #endif

  /* Initialisierung der Highscorewerte */
  LowestName=MyMalloc(200);
  HighestName=MyMalloc(200);
  GreatScoreName=MyMalloc(200);
  strcpy(LowestName," Bill Gates");
  strcpy(HighestName,"Linus");
  strcpy(GreatScoreName,"Mister X");
  Hallptr=MyMalloc(sizeof(HallElement)+1);
  Hallptr->PlayerScore=0;
  Hallptr->PlayerName=MyMalloc(10);
  strcpy(Hallptr->PlayerName," dummy ");
  Hallptr->NextPlayer=NULL;
  
  LowestScoreOfDay=1;
  HighestScoreOfDay=1000;
  GreatScore=101;
  
  HideInvisibleMap=TRUE;         /* Hide invisible map-parts */
	
  printf("\nvoid InitParaplus(void): Highscorevariablen wurden erfolgreich initialisiert...");

  /* Initialisieren der CRTC Portadresse */
  //PORT CRTC=(int*)MK_FP(0,0x0463);


  if (InitLevelColorTable() == FALSE) {
    printf(" Kann Farben nicht initialisieren !");
    getchar();
    Terminate(0);
  }
 
  if (InitParaplusFont() == ERR) {
    printf(" Kann Schrift nicht initialisieren !");
    getchar();
    Terminate(ERR);
  }

  printf("\nvoid InitParaplus(void): Farben- und Fontinitialisierung zumindest fehlerfrei ueberwunden....");

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

  printf("\nvoid InitParaplus(void): Textmeldungsvariablen wurden erfolgreich initialisiert....");

  /* ScreenPointer setzen */
  // PORT RealScreen = MK_FP(SCREENADDRESS, 0);
  RealScreen = malloc(64010);
  InternalScreen = (unsigned char*)MyMalloc(SCREENHOEHE*SCREENBREITE);

  printf("\nvoid InitParaplus(void): Realscreen und Internalscreen haben erfolgreich Speicher erhalten....");

  /* Zufallsgenerator initialisieren */
  //PORT MyRandomize();
		
  if( LoadShip(SHIPNAME) == ERR) {
    printf("Error in LoadShip");
    Terminate(-1);
  }

  printf("\nvoid InitParaplus(void): LoadShip(...) ist erfolgreich zurueckgekehrt....");
	
  /* Now fill the pictures correctly to the structs */
  if (!InitPictures()) {	/* Fehler aufgetreten */
    return;
  }

  printf("\nvoid InitParaplus(void): InitPictures(void) ist erfolgreich zurueckgekehrt....");
	
  /* Init the Takeover- Game */
  InitTakeover();

  printf("\nvoid InitParaplus(void): InitTakeover(void) ist erfolgreich zurueckgekehrt....");
	
  /* Die Zahlen, mit denen die Robotkennungen erzeugt werden einlesen */
  GetDigits(); 

  printf("\nvoid InitParaplus(void): GetDigits(void) ist erfolgreich zurueckgekehrt....");
	
  /* InternWindow */
  /* wenn moeglich: Speicher sparen und mit InternalScreen ueberlappen: */
  if( INTERNHOEHE*INTERNBREITE*BLOCKMEM <= SCREENHOEHE*SCREENBREITE ) {
    InternWindow = InternalScreen;
  } else {
    if( (InternWindow =
	 (unsigned char*)
	 MyMalloc(INTERNBREITE*INTERNHOEHE*BLOCKMEM+100)) == NULL) {
      printf("\nFatal: Out of Memory for InternWindow.");
      getchar();
      Terminate(-1);
    }
  }

  printf("\nvoid InitParaplus(void): InternWindow wurde erfolgreich initialisiert....");

  /* eigenen Zeichensatz installieren */
  LadeZeichensatz(DATA70ZEICHENSATZ);

  printf("\nvoid InitParaplus(void): Zeichensatz wurde erfolgreich geladen....");

  // Initialisieren der Schildbilder
  GetShieldBlocks();

  printf("\nvoid InitParaplus(void): GetShieldBlocks(void) ist fehlerfrei zurueckgekehrt....");

  /* verbiegen der Interrupts */
#if USEINTSOFF == 0
  TurnIntVects();
#endif

  /* richtige Paletten-Werte einstellen */
  InitPalette();

  /* Tastaturwiederholrate auf den geringsten Wert setzen */
  SetTypematicRate(TYPEMATIC_SLOW);

  /* Initialisierung beendet. Monitor wird aktiviert. */
  printf("\nvoid InitParaplus(void): Funktionsende fehlerfrei erreicht....");

} // void InitParaplus(void)

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
  while (!SpacePressed()) JoystickControl();
	
  FadeColors1();					/* Titelbild langsam ausblenden */

  // CRAP WE CAN NO LONGER USE FROM BEFORE THE PORT!!!!
  // for(i=0;i<9;i++) {
  //    TimerFlag=FALSE;
  //    while (!TimerFlag);
  //  }

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
@Desc: Diese Funktion Sprengt den Influencer und beendet das Programm

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void ThouArtDefeated(void){
  int i,j;

  printf("\nvoid ThouArtDefeated(void): Real function call confirmed.");
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
    CalibratedDelay(30000);
  }

  /* Ein Flimmer zieht "uber den Schirm */
  Flimmern();
  Debriefing();

  /* Soundblaster soll keine Toene mehr spucken */
  //PORT sbfm_silence();

  GameOver = TRUE;
  
  printf("\nvoid ThouArtDefeated(void): Usual end of function reached.");
} // void ThouArtDefeated(void)

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
  getchar();
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

  printf("\nvoid Debriefing(void): Real function call confirmed.");

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
    DisplayText(" You have gained entry to the hall\n of fame!\nEnter your name:\n  ",
		USERFENSTERPOSX,USERFENSTERPOSY,RealScreen,FALSE);
    
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
    getchar();
  }

  printf("\nvoid Debriefing(void): Usual end of function reached.");
}  // void Debriefing(void)

// This function does the Pause-Mode, which means, that the game process is halted,
// while the graphics and animations are not.  The "Mode" can be set to CHEESE, which is
// a feature from the original program that should allow for better screenshots and
// of course allow the player to take a rest.
//
// Status: functioning perfectly
//

void Pause(void){
  Me.status=PAUSE;
  SetInfoline();
  UpdateInfoline();
  ClearKbState();
  while (!PPressed() ) {
    usleep(30000);
    JoystickControl();
    //PORT: worfuer war das??  
    //while (!TimerFlag) JoystickControl();
    AnimateInfluence();
    AnimateRefresh();
    RotateBulletColor();
    AnimateEnemys();
    GetView();
    GetInternFenster();
    PutInternFenster();
    //PORT	    if (kbhit()) taste=getch();
    if (CPressed()) {
      JoystickControl();
      Me.status=CHEESE;
      SetInfoline();
      UpdateInfoline();
      while (!SpacePressed()) {
	JoystickControl();
	keyboard_update();
      }
      Me.status=PAUSE;
      SetInfoline();
      UpdateInfoline();
      while (SpacePressed()) {
	JoystickControl();
	keyboard_update();
      }
      taste = 1;
    }
  }
  ClearKbState();
} // void Pause(void)

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

  vga_clear();
  keyboard_close();  // for the moment we don't bother, just use normal kb mode
  if (!PlusExtentionsOn) {
    gl_printf(20,5," Dies ist die aktuelle Highsoreliste:\n\n");
    gl_printf(-1,-1," Highest Score: %10s : %4d\n",HighestName, HighestScoreOfDay);
    gl_printf(-1,-1," Ok Score:      %10s : %4d\n",GreatScoreName, GreatScore);
    gl_printf(-1,-1," Lowest Score:  %10s : %4d\n",LowestName, LowestScoreOfDay);
    getchar();
  } else {
    gl_printf(-1,-1," This is today's Hall of Fame:\n\n");
    gl_printf(-1,-1,"\tRank\tName\tScore\n");
    while (Hallptr->NextPlayer != NULL) {
      gl_printf(-1,-1,"\t%d\t",Rankcounter);
      gl_printf(-1,-1,"%s",Hallptr->PlayerName);
      gl_printf(-1,-1,"\t%d\n",Hallptr->PlayerScore);
      Hallptr=Hallptr->NextPlayer;
      Rankcounter++;
    }
    getchar();
  }
  Hallptr=SaveHallptr;

  vga_clear();
  // return to raw kb mode
  keyboard_init();
  
}

#undef _paraplus_c
