// static const char RCSid[]=\
// "$Id$";

/*
 * Dieses Modul enth"alt Funktionen, die dem Aufbau des Bildes dienen.
 *
 */

#define _view_c

/* locale Schalter zu DEBUG-Zwecken */
#define INFLUENCEOFF  	0
#define BULLETOFF			0
#define BLASTOFF			0
#define ENEMYOFF			0

//#define SHOWSTATS

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "map.h"
#include "proto.h"
#include "colodefs.h"

void FlashWindow(int Flashcolor);
void RecFlashFill(int LX,int LY,int Color,unsigned char* screen, int SBreite);
int Cent(int);

char* Affected;

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
int Cent(int Val){
	Val=Val-(Val%BLOCKBREITE)+BLOCKBREITE/2;
	return Val;
}

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void RecFlashFill(
	int LX, int LY, int Color,
	unsigned char* Screen,
	int SBreite)
{
	int i;
	static int num;

	LY=LY;
	LX=LX;
	num++;

//	gotoxy(1,1);
//	printf(" RFF: X=%d Y=%d.\n",LX,LY);
//	getchar();

	// Dieses Feld als Wirkungsbereich kenntzeichnen
	Affected[LY/BLOCKHOEHE*CurLevel->xlen+LX/BLOCKBREITE]=TRUE;
	
	// Dieses Feld anf"ullen
	for(i=LY/4-((LY/4)%8);i<(LY/4-((LY/4)%8)+8);i++) {
		memset(Screen+i*SBreite+LX/4-((LX/4)%8),Color,8);
	}
	i-=4;

	// Feld rechts davon anf"ullen
	if ( (*(Screen+i*SBreite+LX/4+8) != Color) &&
		(IsPassable(Cent(LX+BLOCKBREITE),Cent(LY),CENTER) == CENTER) )
			RecFlashFill(LX+BLOCKBREITE,LY,Color,Screen,SBreite);
			
	// Feld links davon anf"ullen
	if (LX>BLOCKBREITE) {
		if ( (*(Screen+i*SBreite+LX/4-8) != Color) &&
			(IsPassable(Cent(LX-BLOCKBREITE),Cent(LY),CENTER) == CENTER) )
				RecFlashFill(LX-BLOCKBREITE,LY,Color,Screen,SBreite);
	}
			
	// Feld oben davon anf"ullen
	if ((i>8) && (LY>BLOCKHOEHE)) {
		if ( (*(Screen+(i-8)*SBreite+LX/4) != Color) &&
			(IsPassable(Cent(LX),Cent(LY-BLOCKHOEHE),CENTER) == CENTER) )
				RecFlashFill(LX,LY-BLOCKHOEHE,Color,Screen,SBreite);
	}
			
	// Feld unten davon anf"ullen
	if ( (*(Screen+(i+8)*SBreite+LX/4) != Color) &&
		(IsPassable(Cent(LX),Cent(LY+BLOCKHOEHE),CENTER) == CENTER) )
			RecFlashFill(LX,LY+BLOCKHOEHE,Color,Screen,SBreite);
}

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
unsigned char *FeindZusammenstellen(const char* FZahl,int FPhase)
{
unsigned char* LSrce;
unsigned char* LDest;
int i;

	for (i=0;i<3;i++) {
		LSrce=Digitpointer+10*9*9+(*(FZahl+i)-'0')*9*9;
		LDest=Enemypointer+NUMBEROFS+i*8+BLOCKMEM*FPhase;
		DrawDigit(LSrce,LDest);		
	}
	return (Enemypointer + FPhase*BLOCKMEM);
}


/*@Function============================================================
@Desc: 	Diese Prozedur stellt den momentan intern sichtbaren Kartenausschnitt
			zusammen

			Parameter: keine

@Ret: 	void
* $Function----------------------------------------------------------*/
void GetView(void)
{
  int col, line;
  point testpos;
  int me_gx, me_gy;		/* influ- Grobkoord. */	
  signed int mapX0, mapY0;
  signed int mapX, mapY;	/* The map-coordinates, which are to be copied */

  printf("\nvoid GetView(void): Funktion echt aufgerufen.");
  printf("\nvoid GetView(void): CurLevel->xlen: %d. CurLevel->ylen: %d.",CurLevel->xlen,CurLevel->ylen);

  me_gx = Me.pos.x / BLOCKBREITE;
  me_gy = Me.pos.y / BLOCKHOEHE;

  mapX0 = me_gx - (INTERNBREITE/2);
  mapY0 = me_gy - (INTERNHOEHE/2);
	
  mapX = mapX0;
  mapY = mapY0;
	
  for (line=0;line<INTERNHOEHE;line++, mapY++) {
    mapX = mapX0;
    for(col=0;col<INTERNBREITE;col++, mapX++) {
      
      /* Achtung, falls map-brick nicht mehr sichtbar ! */
      if( HideInvisibleMap ) {
	testpos.x = mapX*BLOCKBREITE+BLOCKBREITE/2;
	testpos.y = mapY*BLOCKHOEHE+BLOCKHOEHE/2;

				/* relative Lage des Blocks zum Influ. ber"ucksichtigen */
	if( me_gx < mapX ) testpos.x -= BLOCKBREITE/2;
	else if(me_gx > mapX) testpos.x += BLOCKBREITE/2;
	if( me_gy < mapY) testpos.y -= BLOCKHOEHE/2;
	else if( me_gy > mapY) testpos.y += BLOCKHOEHE/2;
				
	if( !IsVisible(&testpos)){
	  View[line][col]= INVISIBLE_BRICK;
	  continue;
	}
      }
			
      /* Achtung wenn Koordinate ausserhalb der Map */
      if( mapX < 0 ||
	  mapY < 0 ||
	  mapX >= (CurLevel->xlen) ||
	  mapY >= (CurLevel->ylen))		
	View[line][col] = VOID;
      else
	View[line][col]=CurLevel->map[mapY][mapX];
    } /* for col */
  } /* for line */

  printf("\nvoid GetView(void): Funktionsende ordnungsgemaess erreicht.");
  return;
} /* GetView */


/*@Function============================================================
@Desc:  	Diese Prozedur gibt den momentan intern sichtbaren Kartenausschnitt in
			Textform	am Bildschirm aus

		   Parameter: keine

@Ret: 	void
* $Function----------------------------------------------------------*/
void DisplayView(void)
{
	int i;
	int ii;

	printf(" Das Interne Fenster sieht wie folgt aus:\n");
   for(i=0;i<INTERNHOEHE;i++) {
   	for(ii=0;ii<INTERNBREITE;ii++) {
      	printf("%u-",View[i][ii]);
      }
      printf("\n");
   }
}


/*@Function============================================================
@Desc: Diese Prozedur setzt das Bild im Speicher zusammen, damit es dann
von PutInternFenster in den Bildschirmspeicher kopiert werden kann.

@Ret: none
* $Function----------------------------------------------------------*/
void GetInternFenster(void)
{
  Blast CurBlast = &(AllBlasts[0]);
  int MapBrick;
  int line;
  int col;
  int i;
  int LX,LY,j;
  
  unsigned char *source;		/* the current block to copy */
  unsigned char *target;
  
  target=InternWindow;
  printf("\nvoid GetInternFenster(void) wurde ECHT aufgerufen.\n");
  
  memset(target, 1 , INTERNHOEHE*INTERNBREITE*BLOCKMEM);

  if (Conceptview) {
    GetConceptInternFenster();
    return;
  }
		
  for (line=0;line<(INTERNHOEHE);line++) {
    for (col=0;col<(INTERNBREITE);col++) {
      if( (MapBrick=View[line][col]) != INVISIBLE_BRICK) {
	source = MapBlocks + MapBrick*BLOCKMEM;
	for(i=0; i<BLOCKHOEHE; i++) {
	  MyMemcpy(target, source, BLOCKBREITE);
	  target += INTERNBREITE*BLOCKBREITE;
	  source += BLOCKBREITE;
	}
      } else //
	target += INTERNBREITE*BLOCKMEM;
      if(col<INTERNBREITE-1) target-=INTERNBREITE*BLOCKMEM-BLOCKBREITE;
    } // for(col=0...
    target-=(INTERNBREITE-1)*BLOCKBREITE;
  } // for (line=0...
  printf("\nvoid GetInternFenster(void): Doppelfor-Schleife scheint abgearbeitet zu sein...\n");


#if ENEMYOFF == 0
  for (i=0;i<NumEnemys;i++) PutEnemy(i);
#endif
  
  if (Me.energy > 0) PutInfluence();
	
  for (i=0;i<(MAXBULLETS);i++)
    if (AllBullets[i].type != OUT)
      PutBullet(i);
  
		
  for (i=0;i<(MAXBLASTS);i++)
    if (AllBlasts[i].type != OUT )
      PutBlast(i);
   
   // Sofortiger Check auf Bullet-Blast-Kollisionen
   for(j=0;j<MAXBLASTS;j++){
     /* check Blast-Bullet Collisions and kill hit Bullets */
     for( i=0; i<MAXBULLETS; i++ ) {
       if( AllBullets[i].type == OUT ) continue;
       if( CurBlast->phase > 4) break;
       
       if( abs(AllBullets[i].PX - CurBlast->PX) < BLASTRADIUS ) 
	 if( abs(AllBullets[i].PY - CurBlast->PY) < BLASTRADIUS)
	   {
	     /* KILL Bullet silently */
	     AllBullets[i].type = OUT;
	     AllBullets[i].mine = FALSE;
	   }
     } /* for */
     CurBlast++;
   } /* for */
  printf("\nvoid GetInternFenster(void): Ende der Funktion fehlerfrei erreicht...\n");
} // void GetInternFenster(void) 

/*@Function============================================================
@Desc: GetConceptInternFenster(): analoge Funktion zu GetInternFenster()
			fuer Concept-View.

		Parameter: keine
@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void GetConceptInternFenster(void)
{
	int LX=0;
	int LY=0;
	int i,j;

	// Darstellen der blo"sen Deckkarte 
	for(i=0;i<CurLevel->ylen;i++) {
		for(j=0;j<CurLevel->xlen;j++) {
			SmallBlock(LX,LY,GetMapBrick(CurLevel, j*BLOCKBREITE,i*BLOCKHOEHE),
				InternWindow,INTERNBREITE*BLOCKBREITE);
			LX+=8;
		}
		LX=0;
		LY+=8;
	}

	// Darstellen der Feinde am Deck
	for(i=0;i<MAX_ENEMYS_ON_SHIP;i++) {
		if (Feindesliste[i].levelnum != CurLevel->levelnum) continue;
		if (Feindesliste[i].Status == OUT) continue;
		SmallEnemy(Feindesliste[i].pos.x/4,Feindesliste[i].pos.y/4,
			Druidmap[Feindesliste[i].type].class,InternWindow,INTERNBREITE*BLOCKBREITE);
	}

	// Darstellen des Influencers, wenn er nicht schon vernichtet wurde
	if (Me.energy>0) SmallEnemy(Me.pos.x/4,Me.pos.y/4,-10+Druidmap[Me.type].class,InternWindow,INTERNBREITE*BLOCKBREITE);
	
	// Darstellen der Blasts
	for(i=0;i<MAXBLASTS;i++) {
		if (AllBlasts[i].type == OUT) continue;
		SmallBlast(AllBlasts[i].PX/4,AllBlasts[i].PY/4,AllBlasts[i].type,
			AllBlasts[i].phase,InternWindow,INTERNBREITE*BLOCKBREITE);
	}
	
	// Darstellen der Bullets
	for(i=0;i<MAXBULLETS;i++) {
		if (AllBullets[i].type == OUT) continue;
		
		if (AllBullets[i].type == FLASH) {
		// Wenn der FLASH vorbei ist, l"oschen und fertig
			if (AllBullets[i].time > 2) {
				AllBullets[i].time=0;
				AllBullets[i].type=OUT;
				AllBullets[i].mine=FALSE;
				return;
			}

			// Das ganze Fenster entweder schwarz oder weiss f"arben
			Affected=MyMalloc(CurLevel->xlen*CurLevel->ylen+100);
			memset(Affected,CurLevel->xlen*CurLevel->ylen,FALSE);
 
			if (AllBullets[i].time == 1) {
//					FlashWindow(0);
				RecFlashFill(AllBullets[i].PX,AllBullets[i].PY,FLASHCOLOR1,InternWindow,INTERNBREITE*BLOCKBREITE);
			}
			if (AllBullets[i].time == 2) {
//					FlashWindow(15);
				RecFlashFill(AllBullets[i].PX,AllBullets[i].PY,FLASHCOLOR2,InternWindow,INTERNBREITE*BLOCKBREITE);
			}

			/*
			 * Alle betroffenen Enemys, die nicht immun sind besch"adigen
			 * Auch den Influencer wenn er nicht immun ist besch"adigen
			 */
	 
			for (j=0;j<MAX_ENEMYS_ON_SHIP;j++) {
				if (Feindesliste[j].levelnum != CurLevel->levelnum) continue;
				if ( Affected[Feindesliste[j].pos.x/BLOCKBREITE+Feindesliste[j].pos.y/BLOCKHOEHE*CurLevel->xlen] &&
					  (!Druidmap[Feindesliste[j].type].flashimmune) ) {
							Feindesliste[j].energy-=Bulletmap[FLASH].damage/2;
				}
			}

			if (!InvincibleMode && !Druidmap[Me.type].flashimmune &&
				Affected[Me.pos.x/BLOCKBREITE+Me.pos.y/BLOCKHOEHE*CurLevel->xlen])
				Me.energy-=Bulletmap[FLASH].damage/2;

			free( Affected );
		} else {
			SmallBullet(AllBullets[i].PX/4,AllBullets[i].PY/4,AllBullets[i].type,
				AllBullets[i].phase,InternWindow,INTERNBREITE*BLOCKBREITE);
		}
	}
	return;
}

/*@Function============================================================
@Desc: 	Diese Funktion malt den Influencer an die Position die das Zentrum
    des angezeigten Bildausschnittes sein wird.

    Parameter : die momentane Phase der Drehung des 001 : Darstellphase

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void PutInfluence(void)
{
  // MODIFIED FOR THE PORT!!!!!!!!!!!!!!!!!

  //
  // ACHTUNG!! FUER DEN PORT HAB ICH DIE HALBE FUNKTION ABGESCHALTET!!!
  // SONST GIBTS NACH CA. 10 BILDERN DEN ERSTEN SEGMENTATION FAULT!!!!
  //

  int InternFensterOffset;
  unsigned char *target;
  unsigned char *source;		/* the druid-block to copy */
  int i,j;
  static BeamDelay=1;

  printf("\nvoid PutInfluence(void): REAL function called.");


  source = Influencepointer+(Me.phase)*BLOCKMEM;


  // PORT
  // PutObject(Me.pos.x, Me.pos.y, source, FALSE);
  // return;
  // PORT

  BeamLine=0;

  if (BeamLine > BLOCKBREITE/2) { 
    BeamLine--; 
    return; 
  }
  
  if (!BeamLine) {
    PutObject(Me.pos.x, Me.pos.y, source, FALSE);
  } else {
    for (i=0;i<BLOCKHOEHE;i++) {
      // MyMemcpy(target+BLOCKBREITE*INTERNBREITE*i+BeamLine,source+BLOCKHOEHE*i+BeamLine,(BLOCKBREITE/2+1-BeamLine)*2);
      for (j=0;j<(BLOCKBREITE/2+1-BeamLine)*2;j++) {
	if (*(source+BLOCKHOEHE*i+BeamLine+j) != TRANSPARENTCOLOR)
	  *(target+BLOCKBREITE*INTERNBREITE*i+BeamLine+j)=*(source+BLOCKHOEHE*i+BeamLine+j);
      }
    }
    
    for(i=0;i<BLOCKHOEHE;i++) {
      *(target+BLOCKBREITE*INTERNBREITE*i+BeamLine-1)=BULLETCOLOR;
      *(target+BLOCKBREITE*INTERNBREITE*i+BLOCKBREITE/2+(BLOCKBREITE/2-BeamLine+1))=BULLETCOLOR;
    }
    if (BeamDelay) BeamDelay--; else {	BeamLine--; BeamDelay=1; }
  }
  printf("\nvoid PutInfluence(void): REAL function ended.");
}  // void PutInfluence(void)

/*@Function============================================================
@Desc: PutEnemy: setzt Enemy der Nummer Enum ins InternWindow
					 dazu wird dir Function PutObject verwendet
@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void PutEnemy(int Enum){
	int i;
	int j;

	unsigned char *Enemypic;	
	int enemyX, enemyY;
	const char *druidname;		/* the number-name of the Enemy */
	int phase;
	
	/* if enemy is on other level, return */
	if (Feindesliste[Enum].levelnum != CurLevel->levelnum) return;

	/* wenn dieser Feind abgeschossen ist kann sofort zurueckgekehrt werden */
	if (Feindesliste[Enum].Status == OUT ) return;

	/* Wenn Feind nicht sichtbar: weiter */
	if( ! IsVisible(&Feindesliste[Enum].pos) ) {
		Feindesliste[Enum].onscreen=FALSE;
		return;
	} else Feindesliste[Enum].onscreen=TRUE;

	/* Bild des Feindes mit richtiger Nummer in der richtigen Phase darstellen */
	druidname = Druidmap[Feindesliste[Enum].type].druidname;
	phase = Feindesliste[Enum].feindphase;

	Enemypic = FeindZusammenstellen(druidname,phase);

	enemyX = Feindesliste[Enum].pos.x;
	enemyY = Feindesliste[Enum].pos.y;
	
	PutObject(enemyX, enemyY, Enemypic, FALSE);
	
}


/*@Function============================================================
@Desc: PutBullet: setzt das Bullet BulletNummer ins InternWindow
						dazu wird PutObject verwendet
@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void PutBullet(int BulletNummer)
{
	int i;
	Bullet CurBullet = &AllBullets[BulletNummer];
	unsigned char *bulletpic;

#if BULLETOFF == 1
	return;
#endif
	
	bulletpic = Bulletmap[CurBullet->type].picpointer +
						CurBullet->phase*BLOCKMEM;

	/*
	 * Wenn ein FLASH gestartet ist, wird einfach der ganze Screen
	 * zuerst schwarz, dann weiss geschaltet
	 */
	if (CurBullet->type == FLASH) {

		/*
		 * Wenn der FLASH vorbei ist, l"oschen und fertig
		 */
		 
		if (CurBullet->time > 2) {
			CurBullet->time=0;
			CurBullet->type=OUT;
			CurBullet->mine=FALSE;
			return;
		}

		/*
		 * Alle sichtbaren enemys, die nicht immun sind besch"adigen
		 * Auch den Influencer wenn er nicht immun ist besch"adigen
		 */
		 
		for (i=0;i<MAX_ENEMYS_ON_SHIP;i++) {
			if ( (Feindesliste[i].onscreen) &
				  (!Druidmap[Feindesliste[i].type].flashimmune) ) {
						Feindesliste[i].energy-=Bulletmap[FLASH].damage/2;
			}
		}

		if (!InvincibleMode && !Druidmap[Me.type].flashimmune)
			Me.energy-=Bulletmap[FLASH].damage/2;

		
		/*
		 * Das ganze Fenster entweder schwarz oder weiss f"arben
		 */
		 
		if (CurBullet->time == 1) {
			FlashWindow(0);
			return;
		}
		if (CurBullet->time == 2) {
			FlashWindow(15);
			return;
		}
		
				  
	}

						
	if( PutObject(CurBullet->PX, CurBullet->PY, bulletpic, TRUE) == TRUE) {
		/* Bullet-Bullet Collision: Bullet loeschen */
		CurBullet->type = OUT;
		CurBullet->mine = FALSE;

		/* Druid-Blast dort erzeugen: killt zweites Bullet */
		StartBlast(CurBullet->PX, CurBullet->PY, DRUIDBLAST);
	} /* if */
	
} /* PutBullet */

/*@Function============================================================
@Desc:  PutBlast: setzt das Blast BlastNummer ins InternWindow
							dazu wird PutObject verwendet

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void PutBlast(int BlastNummer)
{
	Blast CurBlast = &AllBlasts[BlastNummer];
	unsigned char *blastpic;

#if BLASTOFF == 1
	return;
#endif

	/* Wenn Blast OUT ist sofort naechsten bearbeiten */
	if (CurBlast->type == OUT) return;
	
   blastpic = Blastmap[CurBlast->type].picpointer + (CurBlast->phase)*BLOCKMEM;

	PutObject(CurBlast->PX, CurBlast->PY, blastpic, FALSE);
}

/*@Function============================================================
@Desc: PutObject: Puts object with center-coordinates x/y and the
			imagepointer to the InternWindow

		check: ON: Bullet-Collsion wird gecheckt
		
		can be used for Influencer, Enemys, Bullets and Blasts 

@Ret: int: TRUE/FALSE: BulletCollision 
@Int:
* $Function----------------------------------------------------------*/
int PutObject(int x, int y, unsigned char *pic, int check)
{
	int DifX, DifY; 		/* Verschiebung zum Influencer */
	unsigned int InternWindowOffset;
	unsigned char *source, *target;
	int ret = FALSE;
	
	/* Verschiebung zum Influencer (which is the center of screen */
	DifX= x - Me.pos.x; 
	DifY= y - Me.pos.y;

	/* Nur Objekte innerhalb des USERFENSTERS anzeigen */
	if ( (DifX>=USERFENSTERBREITE/2) || (DifY>=USERFENSTERHOEHE/2) ||
			(DifX<=-USERFENSTERBREITE/2) || (DifY<=-USERFENSTERHOEHE/2) )
		return ret;

	/* Offset des Exakten Bildmittelpunktes abzueglich der Eckenverschiebung */
	InternWindowOffset=(INTERNHOEHE)*BLOCKHOEHE/2*BLOCKBREITE*INTERNBREITE+
		INTERNBREITE*BLOCKBREITE/2- BLOCKBREITE/2 -
		BLOCKBREITE*INTERNBREITE*BLOCKHOEHE/2;
				
	/* Verschiebung zum Influencer  (linkes oberes Eck !!) */
 	InternWindowOffset += ((Me.pos.y % BLOCKHOEHE) - BLOCKHOEHE/2 )*
			BLOCKBREITE*INTERNBREITE +
			(Me.pos.x % BLOCKBREITE) - BLOCKBREITE/2;

	/* relative Verschiebung des Objekts zum Influencer */
	InternWindowOffset += DifY * BLOCKBREITE*INTERNBREITE + DifX;

	/* center of screen */
	target = InternWindow + InternWindowOffset;

	/* check memory violations */
	if( (target < InternWindow) ||
		(target > InternWindow + INTERNBREITE*INTERNHOEHE*BLOCKMEM-BLOCKMEM) ) {
		gotoxy(1,1);
		printf("Memory violation by PutObject !!!");
		getchar();
		return FALSE;
	}
	
	source = pic;

	return (MergeBlockToWindow(source, target, INTERNBREITE*BLOCKBREITE, check));
}


/*@Function============================================================
@Desc: 	Diese Prozedur schreibt das im Speicher zusammengebaute Bild
			in den Bildschirmspeicher.

		   Parameter: keine
@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void PutInternFenster(void)
{
  // MODIFIED FOR THE PORT!!!!!!!!!!!
  int StartX, StartY;
  int i,j;

  unsigned char *target;
  unsigned char *source;

  printf("void PutInternFenster(void) wurde ECHT aufgerufen..."); 

  if (Conceptview) {
    for(i=0;i<USERFENSTERHOEHE;i++) {
      memcpy(RealScreen+(USERFENSTERPOSY+i)*SCREENBREITE+USERFENSTERPOSX,
	     InternWindow+i*INTERNBREITE*BLOCKBREITE,USERFENSTERBREITE);
    }
    return;
  }
			
  StartX=(Me.pos.x % BLOCKBREITE)-BLOCKBREITE/2;
  StartY=((Me.pos.y % BLOCKHOEHE)-BLOCKHOEHE/2) * BLOCKBREITE * INTERNBREITE;

   //   WaitVRetrace();		/* dont waste time with this */

   for(i=0; i<USERFENSTERHOEHE; i++) {
     source = InternWindow +
       BLOCKBREITE*(INTERNBREITE-VIEWBREITE)/2 + 
       INTERNBREITE*BLOCKBREITE* (BLOCKHOEHE*(INTERNHOEHE-VIEWHOEHE))/2+
       // USERFENSTEROBEN*INTERNBREITE*BLOCKBREITE + 
       //       USERFENSTERLINKS +
       StartY + StartX + i * INTERNBREITE * BLOCKBREITE ;
#ifdef SLOW_VIDEO_CALLS     
     for(j=0; j<USERFENSTERBREITE; j++) {
       vga_setcolor(*source);
       source++;
       vga_drawpixel(USERFENSTERPOSX+j,USERFENSTERPOSY+i);
     } // for(j=0; ...
#else
     vga_drawscansegment(source, USERFENSTERPOSX,USERFENSTERPOSY+i,USERFENSTERBREITE);
     // source+=USERFENSTERBREITE;
#endif
   } // for(i=0; ...
};  // void PutInternFenster(void)

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void RedrawInfluenceNumber(){
unsigned char* LSource;
unsigned char* LDest;
int i;
int j;
int k;
int LPhase=0;
unsigned char LDigit=0;

	for (LPhase=0;LPhase<ENEMYPHASES;LPhase++) {
		for (j=0;j<3;j++) {
			LDigit=*(Druidmap[Me.type].druidname+j)-'0';
			LSource=Digitpointer+LDigit*9*9;
			LDest=Influencepointer+LPhase*BLOCKBREITE*BLOCKHOEHE+j*8+NUMBEROFS;
			DrawDigit(LSource,LDest);
		}
	}
}


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void DrawDigit(unsigned char* Src, unsigned char* Dst) {
	int i;
	int j;

	
	for (i=0;i<9;i++) {
		for (j=0;j<8;j++) {
			*(Dst+i*BLOCKBREITE+j)=*(Src+i*9+j);
		}
	}
}


/*@Function============================================================
@Desc: Diese Funktion setzt die Schu"sfarbe um einen Wert weiter

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void RotateBulletColor(void){
	static int BulColNum;
	int rot,gruen,blau;
	static color BulletColors[MAXBULCOL]=
		{BULLETCOLOR1, BULLETCOLOR2, BULLETCOLOR3, BULLETCOLOR4, BULLETCOLOR5 };
	BulColNum++;
	BulColNum=MyRandom(MAXBULCOL);
	
	if (BulColNum > (MAXBULCOL-1)) BulColNum=0;

	SetPalCol(BULLETCOLOR,BulletColors[BulColNum].rot,
		BulletColors[BulColNum].gruen, BulletColors[BulColNum].blau);
}


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void FlashWindow(int Flashcolor){
	int i;

	/*
	 * das ganze Fenster kann durch einen einzigen Aufruf vom memset
	 * auf den gew"unschten Wert gesetzt werden
	 */

	memset(InternWindow,Flashcolor,
		INTERNBREITE*INTERNHOEHE*BLOCKBREITE*BLOCKHOEHE);
	
}

/*@Function============================================================
@Desc: void SetUserfenster(int color): Setzt die Hintergrundfarbe fuer das
				Userfenster
@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void SetUserfenster(int color, unsigned char *screen)
{
	int row;
	unsigned char *screenstart =
		screen + USERFENSTERPOSY*SCREENLEN + USERFENSTERPOSX;
		
	for(row =0; row<USERFENSTERHOEHE; row++) {
		memset(screenstart, color, USERFENSTERBREITE);
		screenstart += SCREENLEN;
	}

	return;
}

/* **********************************************************************
	Diese Funktion zeigt einen Robotter an
	Dazu mu"s zuerst InitRobotPictures einmal aufgerufen worden sein.
	Wenn InitRobotPictures auf effizientere Methoden umsteigt, so mu"s
	diese Funktion nat"urlich auch entsprechend angepa"st werden.
	**********************************************************************/

#define ROBOTBILDHOEHE SCREENHOEHE/3
#define ROBOTBILDBREITE SCREENBREITE/8

void ShowRobotPicture(int PosX,int PosY, int Number, unsigned char* Screen){
  int j;

  printf("\nvoid ShowRobotPicture(...): Function call confirmed.");

  for(j=0;j<ROBOTBILDHOEHE;j++){
    // PORT MyMemcpy(Screen+PosX+(PosY+j)*SCREENBREITE,Robotptr+((Number/8)+j)*
    // PORT	     SCREENBREITE+(Number % 8)*ROBOTBILDBREITE,ROBOTBILDBREITE);
  }

  printf("\nvoid ShowRobotPicture(...): Usual end of function reached.");
} // void ShowRobotPicture(...)

#undef _view_c

/*=@Header==============================================================
 * $Source$
 *
 * @Desc: all the functions managing the things one gets to see
 *	 
 * 	
 * $Revision$
 * $State$
 *
 * $Author$
 *
 * $Log$
 * Revision 1.11  1997/06/07 09:47:23  jprix
 * improved svgalibcalls: this pushed my framerate from 90FPS to a new average of 140FPS :->
 *
 * Revision 1.10  2002/04/08 19:19:09  rp
 * Johannes latest (and last) non-cvs version to be checked in. Added graphics,sound,map-subdirs. Sound support using ALSA started.
 *
 * Revision 1.10  1997/05/31 13:30:32  rprix
 * Further update by johannes. (sent to me in tar.gz)
 *
 * Revision 1.7  1994/06/19  16:44:10  prix
 * Wed Jun 08 13:46:12 1994: Influence is beamed into the ship when the game starts
 * Wed Jun 15 20:18:01 1994: Schild wird beim Teleportieren nicht angezeigt.
 *
 * Revision 1.6  1993/09/22  14:27:14  prix
 * Sat Jul 31 21:20:14 1993: Koordinatenverschiebung bei ungerader INTERNHOEHE
 * Sun Aug 01 10:37:50 1993: Verschiebungen der Offsets, um 001 ins Zentrum zu bekommen
 * Sun Aug 01 11:51:18 1993: written general PutObject function
 * Sun Aug 01 12:03:26 1993: FeindZusammenstellen liefert Pointer auf Feind-Picture !
 * Sun Aug 01 12:23:41 1993: Written all Put* Functions new: using PutObject
 * Mon Aug 02 09:18:48 1993: RotateBulletColor hinzugefuegt
 * Mon Aug 02 10:00:44 1993: farben genauer an c-64 angenaehert
 * Tue Aug 10 14:28:55 1993: BulletCollision check in MergeBlockToWindow()
 * Tue Aug 24 11:31:04 1993: call to IsVisible()
 *
 * Revision 1.5  1993/08/01  00:48:26  prix
 * Mon May 31 16:51:40 1993: rewritten GetInternFenster in C !
 * Mon May 31 17:11:14 1993: rewritten PutInfluence() in C
 * Mon May 31 17:20:05 1993: PutBullet rewritten in C !
 * Mon May 31 17:24:37 1993: rewritten PutBlast in C
 * Mon May 31 17:55:00 1993: PutInternFenster rewritten in C !!!
 * Mon May 31 19:57:37 1993: TRANSPARENTCOLOR in PutInfluence beruecksichtigen !
 * Mon May 31 20:00:06 1993: char pointers unsigned
 * Tue Jun 01 08:34:08 1993: Bullets ausserhalb des USERFENSTERS nicht darstellen
 * Tue Jun 01 08:37:40 1993: anzeige der Energie
 * Tue Jun 01 08:43:27 1993: Blast ausserhalb des USERFENSTERs nicht mehr anzeigen
 * Tue Jun 01 09:21:58 1993: added func MergeBlockToWindow: adds visible Block to a 'window'
 * Tue Jun 01 09:33:49 1993: ShowPosition wird mit SHOWSTATS define eingeschaltet
 * Tue Jun 01 13:46:40 1993: PutInfluence: erweitert fuer animierte roboter
 * Fri Jul 23 10:43:03 1993: Optionale Positionsausgabe in Koordinatzen eingefuehrt
 * Sat Jul 24 08:08:50 1993: RedrawInfluenceNumber als very alpha eingefuehrt
 * Sat Jul 24 11:30:34 1993: mehrere Digits werden geschrieben
 * Sat Jul 24 11:42:22 1993: die Schrift an die Richtige Position stellen
 * Sat Jul 24 13:54:07 1993: PutEnemy eingefuehrt
 * Sat Jul 24 14:27:44 1993: EnemyAusgabe in GetInternFenster includiert
 * Sat Jul 24 14:36:03 1993: Feindzusammenstellen hinzugefuegt
 * Sat Jul 24 15:02:55 1993: jetzt sollte Putenemy einen sichtbaren Effekt haben
 * Sat Jul 24 15:07:08 1993: Sicherheit gengen Offsetueberlauf eingefuehrt
 * Sat Jul 24 15:19:37 1993: Feinzusammenstellen bis auf DrawDigit reaktiviert
 * Sat Jul 24 15:34:30 1993: Putenemy ohne einschraenkende Kommentare aktiv
 * Sat Jul 24 15:57:23 1993: MergeBlockToWindow wird nicht mehr verwendet
 * Sat Jul 24 16:07:48 1993: Korrekturen an der Zusammenstellung eines Feindes
 * Sat Jul 24 16:24:08 1993: Phasen bei Feinden werden jetzt beruecksichtigt
 * Sat Jul 24 16:31:08 1993: Phase vollstaendig beruecksichtigen
 * Sat Jul 24 16:42:37 1993: Enemys nicht anzeigen wenn der Abstand zu groá ist
 * Sun Jul 25 08:10:13 1993: mehr points statt zwei Zahlen werden fuer enemys verwendet
 * Sun Jul 25 13:18:09 1993: Bereichsueberschreitungscheck fuer PutEnemy eingefuehrt
 * Sun Jul 25 15:34:46 1993: abgeschossene werden nicht angezeigt
 * Mon Jul 26 08:46:13 1993: debugswitch eingefuehrt
 * Sun Mar 26 00:49:49 2000: Blasts die OUT sind nicht anzeigen
 * Fri Jul 30 07:59:05 1993: Influence wird nicht angezeigt wenn er explodiert
 * Sat Jul 31 07:38:11 1993: Ferben aendern sich abhaengig vom Me.status
 * Sat Jul 31 07:55:07 1993: keine Structs mehr im assembler-teil
 * Sat Jul 31 18:04:28 1993: ES GIBT KEINE phases-VARIABLE ! mehr
 * Sat Jul 31 18:56:13 1993: NO MORE ENEMYMAXONLEVEL
 *
 * Revision 1.4  1993/05/31  20:30:36  prix
 * Sun May 23 18:30:16 1993: View zeigt nun den bereich etwas weiter links des Influencers
 * Sun May 23 18:36:59 1993: Neue Berechung des Offset des 001 im internen Fenster
 * Sun May 23 19:11:30 1993: belibige Angaben fuer INTERNBREITE UND INTERNHOEHE moeglich
 * Mon May 24 15:04:27 1993: Endversion des Hintergrunganzeigealgorithmus (Alphavers.)
 * Mon May 24 19:21:58 1993: Korrektur der Anzeige des Influencers fertig
 * Tue May 25 14:54:14 1993: Bulletanzeige auf Alsolutkoordinaten umstellen 
 * Tue May 25 15:15:49 1993: bullet wird nicht mehr Angezeigt wennnicht mehr in Sicht
 * Tue May 25 15:19:49 1993: bullets passieren ungehindert durch offene Tueren
 * Fri May 28 17:47:09 1993: replaced BulletBlockPointer by struct-reference Gunmap
 * Fri May 28 18:29:54 1993: uses picpointer of struct to get pictures
 * Fri May 28 19:06:25 1993: eliminated Darstellphase from PutInfluence (for the moment)
 * Sun May 30 10:28:35 1993: struct.h must be included before proto.h
 * Sun May 30 10:50:15 1993: Anpassung, dass CurLevel nun pointer
 * Sun May 30 13:46:34 1993: Kartenausschnitte ausserhalb des Levels werden als 0 angezeigty
 * Sun May 30 13:56:31 1993: Blasts und Bullets nur anzeigen, wenn existent
 * Sun May 30 14:08:10 1993: BLAST/BULLETANZAHL to ALLBLASTS/BULLETS
 * Mon May 31 14:39:16 1993: killed BlastBlockPointer
 * Mon May 31 16:18:21 1993: GetInternFenster auch mit Offsets ungleich 0 !
 * Mon May 31 16:25:22 1993: killed the offset=0 Pointers
 *
 * Revision 1.3  1993/05/23  21:04:51  prix
 * Sat May 22 19:54:01 1993: Energiebalken anzeigen
 * Sun May 23 08:21:48 1993: Internal
 * Sun May 23 12:17:02 1993: Level replaced by CurLevel.map struct
 *
 * Revision 1.2  1993/05/22  21:49:22  rp
 * Sat May 22 17:48:39 1993: written junk
 *
 * Revision 1.1  1993/05/22  20:54:32  rp
 * Initial revision
 *
 *
 *-@Header------------------------------------------------------------*/

