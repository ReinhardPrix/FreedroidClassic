/*=@Header==============================================================
 * $Source$
 *
 * @Desc: all Bullet AND Blast - related functions.
 *	 
 * 	
 * $Revision$
 * $State$
 *
 * $Author$
 *
 * $Log$
 * Revision 1.10  1997/06/09 23:08:58  jprix
 * Blast phases now adapted to the current framerate.  New constant for speed of animation independant of framerate.
 *
 * Revision 1.9  1997/06/09 13:42:17  jprix
 * Bullets work now completle adjusted to the framerate.  That looks smooth. fine.
 * Modified makefile to depend also on the important paravars.h .
 *
 * Revision 1.8  1997/06/09 13:01:29  jprix
 * Bullet position and speed now also as float.  Program still functionin. Heeyooh! Great!
 *
 * Revision 1.7  1997/06/08 14:49:40  jprix
 * Added file FILES describing the files of this project.
 * Added more doku while writing the files description.
 * Added -Wall compilerflag for maximal generation of sane warnings.
 *
 * Revision 1.6  1997/06/05 09:24:15  jprix
 * Habe YIFF Soundserver eingebaut, doch derweil bleibt er noch durch einen bedingten Compilierungsschalter deaktiviert, weil er bei euch nicht laufen wird.  He. Ich war grad in irgendeiner Form von vi gefangen! Hilfe! Bis der Soundserver aber wirklich geht, wird es noch ein Bischen dauern.  Er ist aber Klasse und das wird sicher toll.  Bis bald, Johannes.
 *
 * Revision 1.5  2002/04/08 19:19:09  rp
 * Johannes latest (and last) non-cvs version to be checked in. Added graphics,sound,map-subdirs. Sound support using ALSA started.
 *
 * Revision 1.6  1997/05/31 13:30:31  rprix
 * Further update by johannes. (sent to me in tar.gz)
 *
 * Revision 1.2  1994/06/19  16:15:51  prix
 * *** empty log message ***
 *
 * Revision 1.1  1993/08/08  21:20:50  prix
 * Initial revision
 *
 *
 *-@Header------------------------------------------------------------*/

/* static const char RCSid[]=\
   "$Id$ "; */ 

#define _bullet_c

#include <stdio.h>
#include <math.h>

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"

/* Distances for hitting a druid */
#define MORE		4
#define DRUIDHITDIST2		(DRUIDRADIUSX+MORE)*(DRUIDRADIUSY+MORE)


/*@Function============================================================
@Desc: this function moves all the bullets according to their speeds.

NEW: this function also takes into accoung the current framerate.

@Ret: keiner

@Int: keiner
* $Function----------------------------------------------------------*/

void MoveBullets(void)
{

  /* lokale Variablen der Funktion: */
  int i;
  Bullet CurBullet;

  /* Bewegung der Bullets */
  for(CurBullet=AllBullets, i=0;i<MAXBULLETS;CurBullet++, i++) {
    if (CurBullet->type == OUT) continue;
    
    CurBullet->pos.x += CurBullet->speed.x * Frame_Time();
    CurBullet->pos.y += CurBullet->speed.y * Frame_Time();
    
    CurBullet->time++;

    /*
      UM ZU VERHINDERN, DASS DIE BULLETS, DIE ETWAS TREFFEN, NICHT MEHR
      DARGESTELLT WERDEN, PASSIERT DIE BULLETKOLLISIONSABFRAGE ERST NACH
      DER ZUSAMMENSTELLUNG DES INTERNFENSTERS. jp, 23.5.94 */
		
    /* Kollisionen mit Mauern und Druids checken UND behandeln */
    //		CheckBulletCollisions(i);

  } /* for */
} // void MoveBullets(void)


/*@Function============================================================
@Desc: Diese Funktion loescht das Bullet mit der uebergebenen Nummer

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void DeleteBullet(int Bulletnummer)
{
	Bullet CurBullet=&AllBullets[Bulletnummer];

	/* Das Bullet loeschen */
	CurBullet->type = OUT;
	CurBullet->time = 0;
	CurBullet->mine = FALSE;

	/* Blast erzeugen: type BULLETBLAST */
	StartBlast(CurBullet->pos.x, CurBullet->pos.y, BULLETBLAST);
}

/*@Function============================================================
@Desc: StartBlast(): erzeugt einen Blast type an x/y

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void StartBlast(int x, int y, int type)
{
	int i;
	Blast NewBlast;
	
	/* Position des naechsten freien Blasts herausfinden */
	for(i=0; i<MAXBLASTS; i++) 
		if( AllBlasts[i].type == OUT ) break;

	/* keinen gefunden: nimm den ersten */
	if (i==MAXBLASTS) i = 0;

	/* Get Pointer to it: more comfortable */
	NewBlast = &(AllBlasts[i]);
	
	/* Einen Blast an x/y erzeugen */
	NewBlast->PX=x;
	NewBlast->PY=y;
	
	NewBlast->type=type;
   NewBlast->phase=0;


   if (type == DRUIDBLAST) Play_YIFF_Server_Sound(BLASTSOUND);

} /* StartBlast */

/*@Function============================================================
@Desc: Diese Funktion zaehlt die Phasen aller Explosionen weiter
@Ret: keiner
@Int: keiner
* $Function----------------------------------------------------------*/
void ExplodeBlasts(void){
  int i;
  Blast CurBlast = AllBlasts;
	
  for (i=0;i<MAXBLASTS;i++, CurBlast ++)
    if (CurBlast->type != OUT )  {
    		
      /* Druidblasts sind gefaehrlich !! */
      if( CurBlast->type == DRUIDBLAST) CheckBlastCollisions(i);
      
      // CurBlast->phase++;
      CurBlast->phase += Frame_Time() * BLASTPHASES_PER_SECOND;
      if ( ((int)rintf(CurBlast->phase)) >= Blastmap[CurBlast->type].phases ) 
	DeleteBlast(i);
    } /* if */
} /* ExplodeBlasts */

/*@Function============================================================
@Desc: Einen eizelnen Blast ausloeschen

@Ret: keiner
@Int:
* $Function----------------------------------------------------------*/
void DeleteBlast(int Blastnummer){
	AllBlasts[Blastnummer].type=OUT;
}

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
int GetDirection(point robo,point bul){
	
	if ((robo.x<bul.x) && (robo.y>bul.y)) return 0;
	if ((robo.x<bul.x) && (robo.y<bul.y)) return 1;
	if ((robo.x>bul.x) && (robo.y<bul.y)) return 2;
	if ((robo.x>bul.x) && (robo.y>bul.y)) return 3;
	if ((robo.x==bul.x) && (robo.y==bul.y)) {
		printf(" Center hit directy!");
		getchar();
	}
	return 0;
}

/*@Function============================================================
@Desc: CheckBulletCollisions(int num)
			checkt Collisions des Bullets Num mit Hintergrund && Druids

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void CheckBulletCollisions(int num)
{
	int i;
	int level = CurLevel->levelnum;
	long xdist, ydist;
	Bullet CurBullet = &AllBullets[num];
	static int FBTZaehler=0;

	if(CurBullet->type == FLASH) return;
	if(CurBullet->type == OUT) return;
	
	/* Kollision der Bullets mit dem Hintergrund feststellen */
	if (IsPassable(CurBullet->pos.x, CurBullet->pos.y, CENTER) != CENTER) {
		DeleteBullet(num);
		return;		/* Bullet ist hin */
	}

	/* Influence getroffen ?? */
	xdist = Me.pos.x - CurBullet->pos.x;
	ydist = Me.pos.y - CurBullet->pos.y;
	if( (xdist*xdist+ydist*ydist) < DRUIDHITDIST2 ) {
		CurBullet->type = OUT;
		CurBullet->mine = FALSE;
		GotHitSound();
		Me.energy -= Bulletmap[CurBullet->type].damage;	/* Energie verlieren */
		return;		/* Bullet ist hin */
	}
	
	/* Alle Enemys checken */
	for( i=0; i<NumEnemys; i++) {
		if( Feindesliste[i].Status == OUT || Feindesliste[i].levelnum != level)
			continue;

		xdist = CurBullet->pos.x - Feindesliste[i].pos.x;
		ydist = CurBullet->pos.y - Feindesliste[i].pos.y;

		if( (xdist*xdist+ydist*ydist) < DRUIDHITDIST2 ) {
			Feindesliste[i].energy -= Bulletmap[CurBullet->type].damage;
			if (!CurBullet->mine) {
				FBTZaehler++;
			}
			CurBullet->type = OUT;
			CurBullet->mine = FALSE;
			break; /* Schleife beenden */
		} /* if getroffen */
		
	} /* for Feindesliste */
		
		
} /* CheckBulletCollisions */

/*@Function============================================================
@Desc: CheckBlastCollsions(int num)
			checkt Collisionen des Blasts num mit Bullets und Druids
			UND reagiert darauf

			LastBlastHit: Diese Variable dient dazu, doppelte Messages zu unter-
			dr"ucken. Blasts schaden mehrere Phasen lang. Der Z"ahler LastBlastHit
			gibt den Zeitabstand zur letzten Verletzung durch Blasts an.
			Er wird in der Hauptschleife erh"oht.
@Ret: void 
@Int:
* $Function----------------------------------------------------------*/
void CheckBlastCollisions(int num)
{
	int i;
	int level = CurLevel->levelnum;
	Blast CurBlast = &(AllBlasts[num]);
	static int RHBZaehler=0;
	
	/* check Blast-Bullet Collisions and kill hit Bullets */
	for( i=0; i<MAXBULLETS; i++ ) {
		if( AllBullets[i].type == OUT ) continue;
		if( CurBlast->phase > 4) break;
		
		if( abs(AllBullets[i].pos.x - CurBlast->PX) < BLASTRADIUS ) 
			 if( abs(AllBullets[i].pos.y - CurBlast->PY) < BLASTRADIUS)
			 {
			 	/* KILL Bullet silently */
			 	AllBullets[i].type = OUT;
			 	AllBullets[i].mine = FALSE;
			 }

	} /* for */
 
	/* Check Blast-Enemy Collisions and smash energy of hit enemy */
	for( i=0; i<NumEnemys; i++) {
		if( (Feindesliste[i].Status == OUT) || (Feindesliste[i].levelnum != level))
			continue;

		if( abs(Feindesliste[i].pos.x - CurBlast->PX) < BLASTRADIUS+DRUIDRADIUSX )
			if( abs(Feindesliste[i].pos.y - CurBlast->PY) < BLASTRADIUS+DRUIDRADIUSY) {
				/* drag energy of enemy */
				Feindesliste[i].energy -= BLASTDAMAGE;
				gotoxy(1,2);
				printf(" Robot hit by Blast %d.\n",RHBZaehler++);
			}
				
		if( Feindesliste[i].energy < 0) Feindesliste[i].energy = 0;

	} /* for */

	/* Check influence-Blast collisions */
	if( (Me.status != OUT) && (abs(Me.pos.x - CurBlast->PX) < DRUIDRADIUSX) )
		if( abs(Me.pos.y - CurBlast->PY) < DRUIDRADIUSY) {
			if (!InvincibleMode) {
				Me.energy -= BLASTDAMAGE;
				if ((PlusExtentionsOn) && (LastBlastHit>5))
					InsertMessage("Blast hit me! OUCH!");
				LastBlastHit=0;
			}
			GotIntoBlastSound();
		}

} /* CheckBlastCollisions */

#undef _bullet_c
