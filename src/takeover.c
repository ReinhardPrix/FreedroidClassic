/*=@Header==============================================================
 * $Source$
 *
 * @Desc:  the takeover-game of Paradroid
 *	 
 * 	
 * $Revision$
 * $State$
 *
 * $Author$
 *
 * $Log$
 * Revision 1.5  2002/04/08 19:19:09  rp
 * Johannes latest (and last) non-cvs version to be checked in. Added graphics,sound,map-subdirs. Sound support using ALSA started.
 *
 * Revision 1.5  1997/05/31 13:30:32  rprix
 * Further update by johannes. (sent to me in tar.gz)
 *
 * Revision 1.2  1994/06/19  16:42:11  prix
 * Thu Jun 02 19:42:47 1994: ??
 *
 * Revision 1.1  1993/10/22  20:13:35  prix
 * Initial revision
 *
 *
 *-@Header------------------------------------------------------------*/

// static const char RCSid[]=\
// "$Id$";

#define _takeover_c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"
#include "takeover.h"

extern int TimerFlag;

unsigned char *ToGroundBlocks;
unsigned char *ToColumnBlock;
unsigned char *ToLeaderBlock;
unsigned char *FillBlocks;
unsigned char *CapsuleBlocks;
unsigned char *ToGameBlocks;

/* Class seperation of the blocks */
int BlockClass[TO_BLOCKS] = {
	CONNECTOR,		/* KABEL */
	NON_CONNECTOR,	/* KABELENDE */
	CONNECTOR,		/* VERSTAERKER */
	CONNECTOR,		/* FARBTAUSCHER */
	CONNECTOR,		/* VERZWEIGUNG_O */
	NON_CONNECTOR,	/* VERZWEIGUNG_M */
	CONNECTOR,		/* VERZWEIGUNG_U */
	NON_CONNECTOR,	/* GATTER_O */
	CONNECTOR,		/* GATTER_M */
	NON_CONNECTOR,	/* GATTER_U */
	NON_CONNECTOR	/* LEER */
};

/* Probability of the various elements */
#define MAX_PROB		100
int ElementProb[TO_ELEMENTS] = {
	100,		/* EL_KABEL */
	2,			/* EL_KABELENDE */
	5,		/* EL_VERSTAERKER */
	5,		/* EL_FARBTAUSCHER: only on last layer */
	5,		/* EL_VERZWEIGUNG */
	5 		/* EL_GATTER */
};
	
	
int NumCapsules[TO_COLORS] = {
	0, 0
};

point LeftCapsulesStart[TO_COLORS] = {
	{ GELB_LEFT_CAPSULES_X, GELB_LEFT_CAPSULES_Y },
	{ VIOLETT_LEFT_CAPSULES_X, VIOLETT_LEFT_CAPSULES_Y }
};

point CurCapsuleStart[TO_COLORS] = {
	{ GELB_CUR_CAPSULE_X, GELB_CUR_CAPSULE_Y },
	{ VIOLETT_CUR_CAPSULE_X, VIOLETT_CUR_CAPSULE_Y }
};


point PlaygroundStart[TO_COLORS] = {
	{ GELB_PLAYGROUND_X, GELB_PLAYGROUND_Y },
	{ VIOLETT_PLAYGROUND_X, VIOLETT_PLAYGROUND_Y }
};

point DruidStart[TO_COLORS] = {
	{ GELB_DRUID_X, GELB_DRUID_Y },
	{ VIOLETT_DRUID_X, VIOLETT_DRUID_Y}
};

int CapsuleCurRow[TO_COLORS] = {0, 0};
int CapsuleCountdown[TO_COLORS][NUM_LINES];

int LeaderColor = GELB;			/* momentary leading color */
int YourColor = GELB;
int OpponentColor = VIOLETT;
int OpponentType;			/* The druid-type of your opponent */

/* the display  column */
int DisplayColumn[NUM_LINES] = {
	GELB, VIOLETT, GELB, VIOLETT, GELB, VIOLETT, GELB, VIOLETT, GELB, VIOLETT, GELB, VIOLETT
};
	

playground_t ToPlayground;

void RollToColors(void);

#include "map.h"


/*@Function============================================================
@Desc: InitTakeover():

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void InitTakeover(void)
{
	if( RealScreen == NULL )
	  // PORT RealScreen = MK_FP(SCREENADDRESS,0000);	
	  RealScreen=malloc(64010);

	if( InternalScreen == NULL ) 
		InternalScreen = (unsigned char*)MyMalloc((size_t)SCREENLEN*SCREENHEIGHT);

	if( InternalScreen == NULL ) {		
		printf("\nvoid InitTakeover(void): Fatal Error: No memory !");
		getchar();
		
		Terminate(-1);
	}

	if( GetTakeoverGraphics() != OK ) {
		printf("Error !");
		Terminate(-1);
	}
		
	return;
}

/*@Function============================================================
@Desc: int Takeover(int enemynum): plays the takeover-game against
							a druid 

@Ret: TRUE: user has won
		FALSE: user has lost
@Int:
* $Function----------------------------------------------------------*/
int Takeover(int enemynum)
{
  int taste;
  int row;
  int FinishTakeover = FALSE;
  int waiter = 0;
  static int RejectEnergy=0;		/* your energy if you're rejected */
  char *message;

  printf("\nvoid Takeover(int enemynum): Funktion echt aufgerufen.");

  /* Damit es zu keinen St"orungen durch die Rahmenupdatefunktion im Interrupt
     kommt, wird diese Funktion tempor"ar desaktiviert. */
  InterruptInfolineUpdate = 0;

  Me.status = MOBILE;
  SetPalCol(INFLUENCEFARBWERT,Mobilecolor.rot,Mobilecolor.gruen,Mobilecolor.blau);

  /* Tastaturwiederholrate richtig setzen */
  SetTypematicRate(TYPEMATIC_FAST);
	
  /* Userfenster faerben */
  SetUserfenster(TO_BG_COLOR, RealScreen);
	
  /* Warte, bis User Space auslaesst */
  while( SpacePressed() ) {
    JoystickControl();
    keyboard_update();
  }

  LadeLBMBild(TAKEOVERBACKGROUNDBILD,RealScreen,FALSE);

  while( !FinishTakeover ) {
	
    /* Init Color-column and Capsule-Number for each opponenet and your color */
    for( row=0; row<NUM_LINES; row++) {
      DisplayColumn[row] = (row % 2);
      CapsuleCountdown[GELB][row] = -1;
      CapsuleCountdown[VIOLETT][row] = -1;
    } /* for row */
			
    YourColor = GELB;
    OpponentColor = VIOLETT;
		
    CapsuleCurRow[GELB] = 0;
    CapsuleCurRow[VIOLETT] = 0;
		
    OpponentType = Feindesliste[enemynum].type;
    NumCapsules[YOU] = 3+ClassOfDruid(Me.type);
    NumCapsules[ENEMY] = 4+ClassOfDruid(OpponentType);
		
    InventPlayground();

    ShowPlayground();

    printf("\nvoid Takeover(int enemynum): Erstmalige Darstellung ist erfolgt.");
	
    ChooseColor();

    PlayGame();

    printf("\nvoid Takeover(int enemynum): PlayGames ist wieder zurueckgekehrt.");
	
    /* Ausgang beurteilen und returnen */
    if( LeaderColor == YourColor ) {
      if( Me.type == DRUID001 ) {
	RejectEnergy = Me.energy;
	PreTakeEnergy = Me.energy;
      }
      Me.energy=(int)(
		      ((long)STARTENERGIE * Feindesliste[enemynum].energy)
		      / Druidmap[OpponentType].maxenergy);
      Me.health= STARTENERGIE; // Druidmap[OpponentType].maxenergy;
      
      Me.type = Feindesliste[enemynum].type;

      RealScore += Druidmap[OpponentType].score;

      message = "Complete";
      FinishTakeover = TRUE;
    }
		
    if( LeaderColor == OpponentColor ) {
      if( Me.type != DRUID001 ) {
	message = "Rejected";
	Me.type = DRUID001;
	Me.energy = RejectEnergy;
      } else {
	message = "Burnt Out";
	Me.energy = 0;
      }
      FinishTakeover = TRUE;
    }
		
    if( LeaderColor == REMIS ) { 
      message = "Deadlock";
    }

    RedrawInfluenceNumber();
		
    /* Feind in jedem Fall ausschalten */		
    Feindesliste[enemynum].Status = OUT;
    OpponentType = -1;		/* dont display enemy any more */
		
    /* Wait a turn */
    waiter = WAIT_AFTER_GAME;
    while( waiter != 0 ) {
      // PORT if( TimerFlag ) {
      // PORT TimerFlag = FALSE;
      usleep(30000);  /* Dies soll eine Wartezeit von 3/100stel Sekunden bringen... */

      
      waiter--;
      RollToColors();
      SetInfoline();
      strcpy(LeftInfo, message);		/* eigene Message anzeigen */
      
      ShowPlayground();
      printf("\nvoid Takeover(int EnemyNum): Mitten in der Endschleife.");
				
      // PORT } /* if TimerFlag */
    } /* while waiter */			
  } /* while !FinishTakeover */

  /* Tastaturwiederholung wieder auf langsam setzen */
  SetTypematicRate(TYPEMATIC_SLOW);
  
  /* Die Rahmenupdatefunktion kann wieder aktiviert werden. */
  InterruptInfolineUpdate = 1;

  printf("\nvoid Takeover(int enemynum): Funktionsende ordnungsgemaess erreicht....");
	
  if( LeaderColor == YourColor ) return TRUE;
  else return FALSE;
	
} // void Takeover(int enemynum)


/*@Function============================================================
@Desc: ChooseColor():	Countdown zum Waehlen der Farbe 

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void ChooseColor(void)
{
  // int countdown = COLOR_COUNTDOWN * 2;
  int countdown = 200;
  int ColorChosen = FALSE;
  char dummy[80];

  printf("\nvoid ChooseColor(void): Funktion echt aufgerufen.");
  
  while (SpacePressed()) {
    JoystickControl();
    keyboard_update();
  }

	
  //PORT  TimerFlag = FALSE;
  while( !ColorChosen ) {
    //if(kbhit()) getchar();
    
    //while( countdown );		/* Wait for next timer-interrupt */
    //TimerFlag = FALSE;

    usleep(30000);  /* Dies soll eine Wartezeit von 3/100stel Sekunden bringen... */

    countdown--;		/* Count down */
    
    JoystickControl();

    keyboard_update();

    if( RightPressed() ) {
      YourColor = VIOLETT;
      OpponentColor = GELB;
    }
    if( LeftPressed() ) {
      YourColor = GELB;
      OpponentColor = VIOLETT;
    }
    
    if( SpacePressed() ) {
      ColorChosen = TRUE;
      while (SpacePressed()) {
	JoystickControl();
	keyboard_update();
      }
    }
      
    ShowPlayground();
    printf("\nvoid ChooseColor(void): ShowPlayground erfolgreich durchgefuehrt.....");

    strcpy(LeftInfo, "Color? ");
    strcat(LeftInfo, itoa(countdown/2, dummy, 10));

    if( countdown == 0 ) ColorChosen = TRUE;
    
  } /* while */
  printf("\nvoid ChooseColor(void): Funktionsende ordnungsgemaess erreicht.");
  return;
} // void ChooseColor(void)


/*@Function============================================================
@Desc: PlayGame(): the heart of the game

@Ret:  void
@Int:
* $Function----------------------------------------------------------*/
void PlayGame(void)
{
  int countdown = GAME_COUNTDOWN*2;
  char dummy[80];
  int taste;
  int FinishTakeover = FALSE;
  int waiter = 0;
  int row;
	
  printf("\nvoid PlayGame(void): Funktion echt aufgerufen.");

  TimerFlag = FALSE;
	
  while( !FinishTakeover) {
    //PORT if( kbhit() ) taste = getchar();
    //KillTastaturPuffer();
    
    //PORT    while( !TimerFlag );
    //PORTTimerFlag = FALSE;

    keyboard_update();
    
    if( WPressed() ) {
      LeaderColor = YourColor;
      printf("\nvoid PlayGame(void): Cheat invoked!  Funktion will return now....");
      return;
    } 

    usleep(30000);  /* Dies soll eine Wartezeit von 3/100stel Sekunden bringen... */

    countdown --;
    
    strcpy(LeftInfo, "Finish-");
    strcat(LeftInfo, itoa(countdown/2,dummy, 10));

    if( countdown == 0 ) FinishTakeover = TRUE;
		
    if( waiter > 0 ) waiter --;


    RollToColors();
		
    if( QPressed() ) {
      FinishTakeover = TRUE;
    }

    printf("\nvoid PlayGame(void): EnemyMovements wird aufgerufen.....");

    EnemyMovements();

    printf("\nvoid PlayGame(void): EnemyMovements erfolgreich zuruckgekehrt.....");

    JoystickControl();

    keyboard_update();
		
    if( UpPressed() && !waiter) {
      waiter = WAIT_MOVEMENT;
      CapsuleCurRow[YourColor] --;
      if( CapsuleCurRow[YourColor] < 1 ) CapsuleCurRow[YourColor] = NUM_LINES;
      // PORT UpPressed() = FALSE;
    }

    if( DownPressed() && !waiter) {
      waiter = WAIT_MOVEMENT;
      CapsuleCurRow[YourColor] ++;
      if( CapsuleCurRow[YourColor] > NUM_LINES ) CapsuleCurRow[YourColor] = 1;
      // PORT DownPressed = FALSE;
    }

    if( SpacePressed() && (NumCapsules[YOU] != 0) ) {
      row = CapsuleCurRow[YourColor] -1;
      
      if( (row >= 0) &&
	  (ToPlayground[YourColor][0][row] != KABELENDE) &&
	  (ToPlayground[YourColor][0][row] < ACTIVE_OFFSET) ) {
	
	NumCapsules[YOU] --;
	CapsuleCurRow[YourColor] = 0;
	ToPlayground[YourColor][0][row] = VERSTAERKER;
	ToPlayground[YourColor][0][row] += ACTIVE_OFFSET;
	CapsuleCountdown[YourColor][row] = CAPSULE_COUNTDOWN*2;
      } /* if */
    }

    ProcessCapsules();		/* count down the lifetime of the capsules */
		
    ProcessPlayground();
    ProcessPlayground();
    ProcessPlayground();
    ProcessPlayground(); /* this has to be done some times to be sure */
    
    ProcessDisplayColumn();
		
    ShowPlayground();
		
  } /* while !FinishTakeover */

  /* Schluss- Countdown */
  countdown = CAPSULE_COUNTDOWN+10; 
	
  while( countdown -- ) {
    // while( !TimerFlag );
    // TimerFlag = FALSE;
    
    usleep(30000);  /* Dies soll eine Wartezeit von 3/100stel Sekunden bringen... */

    RollToColors();
    
    ProcessCapsules();		/* count down the lifetime of the capsules */
    ProcessCapsules();		/* do it twice this time to be faster */
    
    ProcessPlayground();
    ProcessPlayground();
    ProcessPlayground();
    ProcessPlayground(); /* this has to be done some times to be sure */

    ProcessDisplayColumn();
		
    ShowPlayground();
  } /* while .. */
	
  printf("\nvoid PlayGame(void): Funktionsende ordnungsgemaess erreicht....");	
} // void PlayGame(void)

/*@Function============================================================
@Desc: void RollToColors():

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void RollToColors(void)
{
  static int rotate_waiter = 0;
	
  printf("\nvoid RollToColors(void): Funktion echt aufgerufen....");	

  if( rotate_waiter-- == 0 ) {
    RotateColors(67, 70);
    RotateColors(74, 77);
    rotate_waiter = WAIT_COLOR_ROTATION;
  }
  printf("\nvoid RollToColors(void): Funktionsende ordnungsgemaess erreicht....");	
} // void RollToColors(void) 

/*@Function============================================================
@Desc: void EnemyMovements():		animiert Gegner beim Uebernehm-Spiel

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void  EnemyMovements(void)
{
	static int Actions = 3;
	static int MoveProbability = 100;
	static int TurnProbability = 10;
	static int SetProbability = 80;

	int action;
	static int direction=1;		/* start with this direction */
	int row = CapsuleCurRow[OpponentColor] -1;

	if( NumCapsules[ENEMY] == 0 ) return;
	
	
	action = MyRandom(Actions);
	switch(action) {
		
		case 0:	/* Move along */
			if( MyRandom(100) <= MoveProbability ) {
				row += direction;
				if( row > NUM_LINES-1 ) row = 0;
				if( row < 0 ) row = NUM_LINES-1;
			}
			break;

		case 1: /* Turn around */
			if( MyRandom(100) <= TurnProbability ) {
				direction *= -1;
			}
			break;

		case 2:	/* Try to set  capsule */
			if( MyRandom(100) <= SetProbability ) {
				if( (row >= 0) &&
					(ToPlayground[OpponentColor][0][row] != KABELENDE) &&
					(ToPlayground[OpponentColor][0][row] < ACTIVE_OFFSET) ) {

					NumCapsules[ENEMY] --;
					ToPlayground[OpponentColor][0][row] = VERSTAERKER;
					ToPlayground[OpponentColor][0][row] += ACTIVE_OFFSET;
					CapsuleCountdown[OpponentColor][row] = CAPSULE_COUNTDOWN;
					row = -1;	/* For the next capsule: startpos */
				} /* if */
			} /* if MyRandom */

			break;

		default:
			break;
			
	} /* switch action */

	CapsuleCurRow[OpponentColor] = row+1;

	return;
	
} /* EnemyMovements */

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
int GetTakeoverGraphics(void)
{
	unsigned char *tmp;
	int i, j;
	int curx, cury;
	

	/* Get the elements */	
	LadeLBMBild(ELEMENTS_FILE,InternalScreen,FALSE);

	curx = 0;
	cury = 0;		/* readpos in pic */
	
	/* Get the fill-blocks */
	FillBlocks = (unsigned char*)MyMalloc( 3*FILLBLOCKMEM +10);
	IsolateBlock(InternalScreen, FillBlocks, curx, cury, FILLBLOCKLEN, FILLBLOCKHEIGHT);
	curx += FILLBLOCKLEN+1;
	
	IsolateBlock(InternalScreen, FillBlocks+FILLBLOCKMEM, curx, cury,
				FILLBLOCKLEN, FILLBLOCKHEIGHT);
	curx += FILLBLOCKLEN+1;
	
	IsolateBlock(InternalScreen, FillBlocks+2*FILLBLOCKMEM, curx, cury,
				FILLBLOCKLEN, FILLBLOCKHEIGHT);
	curx += FILLBLOCKLEN+1;


	/* Get the capsule Blocks */
	CapsuleBlocks = (unsigned char*)MyMalloc( 3*CAPSULE_MEM +10);
	for( i=0; i<3; i++) {
		IsolateBlock(InternalScreen, CapsuleBlocks+i*CAPSULE_MEM,
			curx, cury, CAPSULE_LEN, CAPSULE_HEIGHT);
		curx += CAPSULE_LEN+1;
	}

	curx = 0;
	cury += FILLBLOCKHEIGHT+1;	
		
	/* get the game-blocks */
	
	ToGameBlocks = (unsigned char*)MyMalloc( 4 * TO_BLOCKS * TO_BLOCKMEM);

	tmp = ToGameBlocks;
	for ( j=0; j<4; j++) {

		for( i=0; i<7; i++ ) {
			IsolateBlock(InternalScreen, tmp, curx, cury,	TO_BLOCKLEN, TO_BLOCKHEIGHT);
			tmp += TO_BLOCKMEM;
			curx += TO_BLOCKLEN+1;
		}

		curx = 0;		
		cury += TO_BLOCKHEIGHT+1;
		
		for( i=0; i<4; i++ ) {
			IsolateBlock(InternalScreen, tmp,	curx,cury, TO_BLOCKLEN, TO_BLOCKHEIGHT);
			tmp += TO_BLOCKMEM;
			curx += TO_BLOCKLEN+1;
		}

		curx = 0;
		cury += TO_BLOCKHEIGHT+1;
	}
		
	
	/* Get the ground, column and leader blocks */
	ToGroundBlocks = (unsigned char*)MyMalloc(6*GROUNDBLOCKLEN*GROUNDBLOCKHEIGHT+10);
	tmp = ToGroundBlocks;
	for( i=0; i<6; i++, tmp += GROUNDBLOCKLEN*GROUNDBLOCKHEIGHT ) {
		IsolateBlock(InternalScreen, tmp, curx, cury, GROUNDBLOCKLEN, GROUNDBLOCKHEIGHT);
		curx += GROUNDBLOCKLEN+1;
	}
	cury += GROUNDBLOCKHEIGHT+1;
	curx = 0;

	ToColumnBlock = (unsigned char*)MyMalloc(COLUMNBLOCKLEN*COLUMNBLOCKHEIGHT+10);
	IsolateBlock(InternalScreen, ToColumnBlock, curx, cury, COLUMNBLOCKLEN, COLUMNBLOCKHEIGHT);
	curx += COLUMNBLOCKLEN+1;

	ToLeaderBlock = (unsigned char*)MyMalloc(LEADERBLOCKLEN*LEADERBLOCKHEIGHT+10);
	IsolateBlock(InternalScreen, ToLeaderBlock, curx, cury, LEADERBLOCKLEN, LEADERBLOCKHEIGHT);
	
	return OK;
	
}

/*@Function============================================================
@Desc: void ShowPlayground(void): displays complete initial Playground

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void ShowPlayground(void)
{
  int i, j;
  int color, opponent;
  unsigned char *LeftDruid, *RightDruid;
  unsigned char *Enemypic;
  register int curx, cury;
  unsigned char *tmp;
  int caps_row, caps_x, caps_y;		/* Play-capsule state */
	
  static unsigned char *WorkBlock = NULL;

  printf("\nvoid ShowPlayground(void): Funktion echt aufgerufen.");
	

  if( WorkBlock == NULL ) {
    WorkBlock = MyMalloc(BLOCKMEM+10);
  }
	
  UpdateInfoline();

  /* Linke Saeule */
  curx = USERFENSTERPOSX + LEFT_OFFS_X;
  cury = USERFENSTERPOSY + LEFT_OFFS_Y;

				
	if( YourColor == GELB ) opponent = YOU;
	else opponent = ENEMY;
	
	if( NumCapsules[opponent] > 0 ) caps_row = CapsuleCurRow[GELB];
	else caps_row = -1;

	caps_x = CurCapsuleStart[GELB].x;
	caps_y =	CurCapsuleStart[GELB].y + caps_row*(CAPSULE_HEIGHT+1);
	
	DisplayBlock( curx, cury, ToGroundBlocks + GELB_OBEN*GROUNDBLOCKMEM,	GROUNDBLOCKLEN, GROUNDBLOCKHEIGHT, RealScreen);
 	
	cury += GROUNDBLOCKHEIGHT;
	tmp = ToGroundBlocks + GELB_MITTE*GROUNDBLOCKMEM;
	for( i=0; i<12 ; i++, cury += GROUNDBLOCKHEIGHT) {
		DisplayBlock(curx, cury,tmp, GROUNDBLOCKLEN, GROUNDBLOCKHEIGHT, RealScreen);
		if( (caps_row == i) || (i==11 && caps_row == 12) )
			DisplayBlock(caps_x, caps_y,
						CapsuleBlocks,
						CAPSULE_LEN, CAPSULE_HEIGHT,
						RealScreen);
	} /* for */

	DisplayBlock(curx, cury, ToGroundBlocks + GELB_UNTEN*GROUNDBLOCKMEM, GROUNDBLOCKLEN, GROUNDBLOCKHEIGHT, RealScreen);

	/* Mittlere Saeule */
	curx = USERFENSTERPOSX + MID_OFFS_X;
	cury = USERFENSTERPOSY + MID_OFFS_Y;

	DisplayMergeBlock(curx, cury, ToLeaderBlock, LEADERBLOCKLEN, LEADERBLOCKHEIGHT, RealScreen);

	cury += LEADERBLOCKHEIGHT;
	for( i=0; i<12; i++, cury += COLUMNBLOCKHEIGHT)
		DisplayMergeBlock(curx, cury,ToColumnBlock,COLUMNBLOCKLEN, COLUMNBLOCKHEIGHT,RealScreen);

	/* rechte Saeule */
	curx = USERFENSTERPOSX + RIGHT_OFFS_X;
	cury = USERFENSTERPOSY + RIGHT_OFFS_Y;

	if( opponent == YOU ) opponent = ENEMY;
	else opponent = YOU;

	if( NumCapsules[opponent] > 0 ) caps_row = CapsuleCurRow[VIOLETT];
	else caps_row = -1;

	caps_x = CurCapsuleStart[VIOLETT].x;
	caps_y =	CurCapsuleStart[VIOLETT].y + caps_row*(CAPSULE_HEIGHT+1);
	
	DisplayBlock(curx, cury, ToGroundBlocks + VIOLETT_OBEN*GROUNDBLOCKMEM, GROUNDBLOCKLEN, GROUNDBLOCKHEIGHT, RealScreen);
							
	cury += GROUNDBLOCKHEIGHT;
	
	tmp = ToGroundBlocks+VIOLETT_MITTE*GROUNDBLOCKMEM;
	for( i=0; i<12; i++, cury += GROUNDBLOCKHEIGHT ) {
		DisplayBlock(curx, cury,tmp, GROUNDBLOCKLEN, GROUNDBLOCKHEIGHT, RealScreen);
		if( (caps_row == i) || (i==11 && caps_row == 12) ) 	
			DisplayBlock(caps_x, caps_y,
							CapsuleBlocks + CAPSULE_MEM,
							CAPSULE_LEN, CAPSULE_HEIGHT,
							RealScreen);
	} /* for */

	DisplayBlock(curx, cury, ToGroundBlocks + VIOLETT_UNTEN*GROUNDBLOCKMEM, GROUNDBLOCKLEN, GROUNDBLOCKHEIGHT, RealScreen);

	/* Fill the Leader-LED with its color */
	DisplayBlock(LEADERLEDX, LEADERLEDY,FillBlocks+LeaderColor*FILLBLOCKMEM,FILLBLOCKLEN, FILLBLOCKHEIGHT,RealScreen);
		
	DisplayBlock(LEADERLEDX, LEADERLEDY+FILLBLOCKHEIGHT,FillBlocks+LeaderColor*FILLBLOCKMEM,FILLBLOCKLEN, FILLBLOCKHEIGHT,RealScreen);

	
	/* Fill the Display Column with its colors */
	for( i=0; i<NUM_LINES; i++) 			
		 DisplayBlock(LEDCOLUMNX, LEDCOLUMNY+i*(FILLBLOCKHEIGHT+1),	FillBlocks+DisplayColumn[i]*FILLBLOCKMEM,FILLBLOCKLEN, FILLBLOCKHEIGHT,RealScreen);

	/* Show the yellow playground */
	for( i=0; i<NUM_LAYERS-1; i++ )
		for( j=0; j<NUM_LINES; j++ ) {
			DisplayBlock(PlaygroundStart[GELB].x + i*TO_BLOCKLEN,PlaygroundStart[GELB].y + j*TO_BLOCKHEIGHT,ToGameBlocks + ToPlayground[GELB][i][j] * TO_BLOCKMEM,TO_BLOCKLEN, TO_BLOCKHEIGHT,RealScreen);
		}

	/* Show the violett playground */
	curx = PlaygroundStart[VIOLETT].x- TO_BLOCKLEN;
	
	for( i=0; i<NUM_LAYERS-1; i++, curx -= TO_BLOCKLEN ) {
		cury = PlaygroundStart[VIOLETT].y;		
		for( j=0; j<NUM_LINES; j++, cury += TO_BLOCKHEIGHT ) {
			DisplayBlock(curx, cury, 
				ToGameBlocks + ToPlayground[VIOLETT][i][j] * TO_BLOCKMEM + TO_BLOCKS*TO_BLOCKMEM,
				TO_BLOCKLEN, TO_BLOCKHEIGHT,
				RealScreen);
		} /* for lines */
	} /* for layers */


	/* Show the capsules left for each player */

	for( opponent = 0;  opponent < 2; opponent++) {
		if( opponent == YOU ) color = YourColor;
		else color = OpponentColor;

		for( i=0; i<MAX_CAPSULES; i++ ) {
			if( i < NumCapsules[opponent]-1 )
				DisplayBlock(
					LeftCapsulesStart[color].x,
					LeftCapsulesStart[color].y + i*(CAPSULE_HEIGHT),
					CapsuleBlocks+ color*CAPSULE_MEM,
					CAPSULE_LEN, CAPSULE_HEIGHT,
					RealScreen);
			else
				DisplayBlock(
					LeftCapsulesStart[color].x,
					LeftCapsulesStart[color].y + i*(CAPSULE_HEIGHT),
					CapsuleBlocks + TO_COLORS*CAPSULE_MEM,
					CAPSULE_LEN, CAPSULE_HEIGHT,
					RealScreen);


		} /* for capsules */

	} /* for opponent */

	
	/* Display the two opponents */
	if( OpponentType == -1 ) Enemypic = NULL;
	else
		Enemypic = FeindZusammenstellen(Druidmap[OpponentType].druidname,0);
		
	if( YourColor == GELB )  {
		LeftDruid = Influencepointer;
		RightDruid = Enemypic;
	} else {
		LeftDruid = Enemypic;
		RightDruid = Influencepointer;
	}

	/* Show Druid - pictures, but not transparently !! */

	memset(WorkBlock, TO_BG_COLOR, BLOCKMEM);

	if( LeftDruid != NULL )
		CopyMergeBlock(WorkBlock, LeftDruid, BLOCKMEM);
		
 	DisplayBlock(
		DruidStart[GELB].x, DruidStart[GELB].y ,
		WorkBlock,
		BLOCKBREITE, BLOCKHOEHE-5,
		RealScreen);

	memset(WorkBlock, TO_BG_COLOR, BLOCKMEM);
	
	if( RightDruid != NULL )
		CopyMergeBlock(WorkBlock, RightDruid, BLOCKMEM);
		
	DisplayBlock(
		DruidStart[VIOLETT].x, DruidStart[VIOLETT].y,
		WorkBlock,
		BLOCKBREITE, BLOCKHOEHE-5,
		RealScreen);

	printf("\nvoid ShowPlayground(void): Funktionsende ordnungsgemaess erreicht....");
	

} // void ShowPlayground(void)


/*@Function============================================================
@Desc: ClearPlayground(): Clears Playground to default start-values

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void ClearPlayground(void)
{
	int color, layer, row;

	for( color = GELB; color < TO_COLORS; color ++) {
		for( layer = 0; layer < NUM_LAYERS; layer ++) {
			for( row = 0; row < NUM_LINES; row ++) {
				if( layer < TO_COLORS - 1)
					ToPlayground[color][layer][row] = KABEL;
				else
					ToPlayground[color][layer][row] = INAKTIV;
			} /* for row */
		} /* for layer */
	} /* for color */

	for( row = 0; row < NUM_LINES; row ++)
			DisplayColumn[row] = row % 2;
			
} /* ClearPlayground */


/*@Function============================================================
@Desc: InventPlayground():  generates a random Playground
	
@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void InventPlayground(void)
{
	int anElement;
	int newElement;
	int row, layer;
	int color = GELB;


	/* first clear the playground: we depend on this !! */
	ClearPlayground();
	
	for( color = GELB; color < TO_COLORS; color ++) {
		for( layer = 1; layer < NUM_LAYERS-1; layer ++ ) {
			for( row = 0; row < NUM_LINES; row ++) {
				if( ToPlayground[color][layer][row] != KABEL ) continue;
				
				newElement = MyRandom(TO_ELEMENTS);
				if( MyRandom(MAX_PROB) > ElementProb[newElement]) {
					row --;
					continue;
				}
				
				switch(newElement) {
					case EL_KABEL:		/* has not to be set any more */
						anElement = ToPlayground[color][layer-1][row];
						if( BlockClass[anElement] == NON_CONNECTOR )
							ToPlayground[color][layer][row] = LEER;

						break;
	
					case EL_KABELENDE:
						anElement = ToPlayground[color][layer-1][row];
						if( BlockClass[anElement] == NON_CONNECTOR ) 
							ToPlayground[color][layer][row] = LEER;
						else
							ToPlayground[color][layer][row] = KABELENDE;
							
						break;

					case EL_VERSTAERKER:
						anElement = ToPlayground[color][layer-1][row];
						if( BlockClass[anElement] == NON_CONNECTOR) 
							ToPlayground[color][layer][row] = LEER;
						else
							ToPlayground[color][layer][row] = VERSTAERKER;
							
						break;

					case EL_FARBTAUSCHER:
						if( layer != 2 ) {		/* only existing on layer 2 */
							row --;
							continue;
						}
					
						anElement = ToPlayground[color][layer-1][row];
						if( BlockClass[anElement] == NON_CONNECTOR )
							ToPlayground[color][layer][row] = LEER;
						else
							ToPlayground[color][layer][row] = FARBTAUSCHER;

						break;

					case EL_VERZWEIGUNG:
						if( row > NUM_LINES-3 ) {
							/* try again */
							row --;
							break;
						}

						anElement = ToPlayground[color][layer-1][row+1];
						if( BlockClass[anElement] == NON_CONNECTOR ) {
							/* try again */
							row --;
							break;
						}
						
						/* dont destroy verzweigungen in prev. layer */
						anElement = ToPlayground[color][layer-1][row];
						if( anElement == VERZWEIGUNG_O || anElement == VERZWEIGUNG_U ) {
							row --;
							break;
						}
						anElement = ToPlayground[color][layer-1][row+2];
						if( anElement == VERZWEIGUNG_O || anElement == VERZWEIGUNG_U ) {
							row --;
							break;
						}							

						/* cut off kabels in last layer, if any */
						anElement = ToPlayground[color][layer-1][row];
						if( BlockClass[anElement] == CONNECTOR ) 
							ToPlayground[color][layer-1][row] = KABELENDE;
							
						anElement = ToPlayground[color][layer-1][row+2];
						if( BlockClass[anElement] == CONNECTOR )
							ToPlayground[color][layer-1][row+2] = KABELENDE;

						/* set the verzweigung itself */
						ToPlayground[color][layer][row]   = VERZWEIGUNG_O;
						ToPlayground[color][layer][row+1] = VERZWEIGUNG_M;
						ToPlayground[color][layer][row+2] = VERZWEIGUNG_U;

						row += 2;
						break;

					case EL_GATTER:
						if( row > NUM_LINES-3 ) {
							/* try again */
							row --;
							break;
						}

						anElement = ToPlayground[color][layer-1][row];
						if( BlockClass[anElement] == NON_CONNECTOR) {
							/* try again */
							row --;
							break;
						}
						anElement = ToPlayground[color][layer-1][row+2];
						if( BlockClass[anElement] == NON_CONNECTOR) {
							/* try again */
							row --;
							break;
						}
	

						/* cut off kabels in last layer, if any */
						anElement = ToPlayground[color][layer-1][row+1];
						if( BlockClass[anElement] == CONNECTOR )
							ToPlayground[color][layer-1][row+1] = KABELENDE;

						/* set the GATTER itself */
						ToPlayground[color][layer][row]   = GATTER_O;
						ToPlayground[color][layer][row+1] = GATTER_M;
						ToPlayground[color][layer][row+2] = GATTER_U;

						row += 2;
						break;

					default:
						row --;
						break;
						
				} /* switch NewElement */

			} /* for row */
			
		} /* for layer */
		
	} /* for color */
	
} /* InventPlayground */

/*@Function============================================================
@Desc: ProcessPlayground(): process the playground following its
								intrinsic logic

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void ProcessPlayground(void)
{
	int color, layer, row;
	int TurnActive = FALSE;
	int TestElement;
	
	for( color = GELB; color < TO_COLORS; color ++) {
		for( layer = 1; layer < NUM_LAYERS; layer ++ ) {
			for( row = 0; row < NUM_LINES; row ++) {
				if( layer == NUM_LAYERS-1 ) {
					if( IsActive(color,row) )
						ToPlayground[color][layer][row] = AKTIV;
					else
						ToPlayground[color][layer][row] = INAKTIV;

					continue;
				} /* if last layer */

				TurnActive = FALSE;

				TestElement = ToPlayground[color][layer][row];
				if( TestElement >= ACTIVE_OFFSET )
					TestElement -= ACTIVE_OFFSET;
					
				switch(TestElement) {
					case FARBTAUSCHER:
					case VERZWEIGUNG_M:
					case GATTER_O:
					case GATTER_U:
					case KABEL:
						if( ToPlayground[color][layer-1][row] >= ACTIVE_OFFSET )
							TurnActive = TRUE;

						break;

					case VERSTAERKER:
						if( ToPlayground[color][layer-1][row] >= ACTIVE_OFFSET )
							TurnActive = TRUE;
							
						/* Verstaerker halten sich aber auch selbst aktiv !! */
						if( ToPlayground[color][layer][row] >= ACTIVE_OFFSET )
							TurnActive = TRUE;

						break;
						
					case KABELENDE:
						break;

					case VERZWEIGUNG_O:
						if( ToPlayground[color][layer][row+1] >= ACTIVE_OFFSET )
							TurnActive = TRUE;

						break;

					case VERZWEIGUNG_U:
						if( ToPlayground[color][layer][row-1] >= ACTIVE_OFFSET )
							TurnActive = TRUE;

						break;

					case GATTER_M:
						if( (ToPlayground[color][layer][row-1] >= ACTIVE_OFFSET) &&
							 (ToPlayground[color][layer][row+1] >= ACTIVE_OFFSET) )
							 	TurnActive = TRUE;

						break;

					default:
						break;

				} /* switch */

				if( TurnActive ) {
					if( ToPlayground[color][layer][row] < ACTIVE_OFFSET)
						ToPlayground[color][layer][row] += ACTIVE_OFFSET;
						
					TurnActive = FALSE;
				} else {
					if( ToPlayground[color][layer][row] >= ACTIVE_OFFSET )
						ToPlayground[color][layer][row] -= ACTIVE_OFFSET;
				} /* else */

			} /* for row */
			
		} /* for layer */

	} /* for color */

	return;
} /* ProcessPlayground */



/*@Function============================================================
@Desc:  ProcessDisplayColumn(): setzt die Korrekten Werte in der Display-
										Saeule. Blinkende LEDs werden ebenfalls hier
										realisiert

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void ProcessDisplayColumn(void)
{
	static int CLayer = 3;			/* the connection-layer to the Column */	
	static int flicker_color=0;
	int row, color;
	int GelbCounter, ViolettCounter;
	int Tauscher = FARBTAUSCHER + ACTIVE_OFFSET;

	flicker_color = !flicker_color;
 	
	for( row=0; row < NUM_LINES; row++ ) {
		
		/* eindeutig gelb */
		if( (ToPlayground[GELB][CLayer][row] == AKTIV) &&
			(ToPlayground[VIOLETT][CLayer][row] == INAKTIV) ) {
			/* Farbtauscher ??? */
			if( ToPlayground[GELB][CLayer-1][row] == Tauscher )
				DisplayColumn[row] = VIOLETT;
			else
				DisplayColumn[row] = GELB;
				
			continue;
		}

	
		/* eindeutig violett */
		if( (ToPlayground[GELB][CLayer][row] == INAKTIV) &&
			(ToPlayground[VIOLETT][CLayer][row] == AKTIV) ) {
			/* Farbtauscher ??? */
			if( ToPlayground[VIOLETT][CLayer-1][row] == Tauscher )
				DisplayColumn[row] = GELB;
			else
				DisplayColumn[row] = VIOLETT;
				
			continue;
		}

		/* unentschieden: Flimmern */
		if( (ToPlayground[GELB][CLayer][row] == AKTIV) &&
			(ToPlayground[VIOLETT][CLayer][row] == AKTIV) ) {
			/* Farbtauscher - Faelle */
			if( (ToPlayground[GELB][CLayer-1][row] == Tauscher) &&
				(ToPlayground[VIOLETT][CLayer-1][row] != Tauscher) )
				DisplayColumn[row] = VIOLETT;
			else if( (ToPlayground[GELB][CLayer-1][row] != Tauscher) &&
					(ToPlayground[VIOLETT][CLayer-1][row] == Tauscher) )
						DisplayColumn[row] = GELB;
			else {
				if( flicker_color == 0 ) DisplayColumn[row] = GELB;
				else DisplayColumn[row] = VIOLETT;
			} /* if - else if - else */
			
		} /* if unentschieden */
		

	} /* for */

	/* Win Color beurteilen */
	GelbCounter = 0;
	ViolettCounter = 0;
	for( row = 0; row < NUM_LINES; row ++) 
		if( DisplayColumn[row] == GELB ) GelbCounter ++;
		else ViolettCounter ++;

	if( ViolettCounter < GelbCounter ) LeaderColor = GELB;
	else if( ViolettCounter > GelbCounter ) LeaderColor = VIOLETT;
	else LeaderColor = REMIS;
	
	
} /* ProcessDisplayColumn */

/*@Function============================================================
@Desc: ProcessCapsules():	does the countdown of the capsules and
									kills them if too old

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void ProcessCapsules(void)
{
	int row;
	int color;

	for( color=GELB; color <= VIOLETT; color++ )
		for( row = 0; row < NUM_LINES; row ++ ) {
			if( CapsuleCountdown[color][row] > 0 )
				CapsuleCountdown[color][row] --;
				
			if( CapsuleCountdown[color][row] == 0 ) {
				CapsuleCountdown[color][row] = -1;
				ToPlayground[color][0][row] = KABEL;
			}

		} /* for row */
		
} /* ProcessCapsules() */		
		

/*@Function============================================================
@Desc: IsInactive(color, row): tells, wether a Column-connection
									is active or not

@Ret: TRUE/FALSE
@Int:
* $Function----------------------------------------------------------*/
int IsActive(int color, int row)
{
	int CLayer = 3;		/* the connective Layer */
	int TestElement = ToPlayground[color][CLayer-1][row];
	
	if( (TestElement >= ACTIVE_OFFSET) &&
		(BlockClass[TestElement-ACTIVE_OFFSET] == CONNECTOR))
		
		return TRUE;
		
	else return FALSE;
} /* IsActive */

#undef _takeover_c

