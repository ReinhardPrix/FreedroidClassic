/*=@Header==============================================================
 * $Source$
 *
 * @Desc: Bullet AND Blast - related functions 
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
 * Revision 1.2  1994/06/19  16:15:51  prix
 * *** empty log message ***
 *
 * Revision 1.1  1993/08/08  21:20:50  prix
 * Initial revision
 *
 *
 *-@Header------------------------------------------------------------*/
static const char RCSid[]=\
"$Id$";

#define _bullet_c

#include <stdio.h>
#include <math.h>
#include <conio.h>

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"

/* Distances for hitting a druid */
#define MORE		4
#define DRUIDHITDIST2		(DRUIDRADIUSX+MORE)*(DRUIDRADIUSY+MORE)


/*@Function============================================================
@Desc: Diese Funktion bewegt alle Bullets gemaess ihrer Geschwindigkeit.
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
		
		CurBullet->PX += CurBullet->SX;
		CurBullet->PY += CurBullet->SY;

		CurBullet->time++;

/*
	UM ZU VERHINDERN, DASS DIE BULLETS, DIE ETWAS TREFFEN, NICHT MEHR
	DARGESTELLT WERDEN, PASSIERT DIE BULLETKOLLISIONSABFRAGE ERST NACH
	DER ZUSAMMENSTELLUNG DES INTERNFENSTERS. jp, 23.5.94 */
		
		/* Kollisionen mit Mauern und Druids checken UND behandeln */
//		CheckBulletCollisions(i);

	} /* for */
} /* MoveBullets */


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
	StartBlast(CurBullet->PX, CurBullet->PY, BULLETBLAST);
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
    		
			CurBlast->phase++;
			if (CurBlast->phase >= Blastmap[CurBlast->type].phases) 
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
		getch();
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
	if (IsPassable(CurBullet->PX, CurBullet->PY, CENTER) != CENTER) {
		DeleteBullet(num);
		return;		/* Bullet ist hin */
	}

	/* Influence getroffen ?? */
	xdist = Me.pos.x - CurBullet->PX;
	ydist = Me.pos.y - CurBullet->PY;
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

		xdist = CurBullet->PX - Feindesliste[i].pos.x;
		ydist = CurBullet->PY - Feindesliste[i].pos.y;

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
		
		if( abs(AllBullets[i].PX - CurBlast->PX) < BLASTRADIUS ) 
			 if( abs(AllBullets[i].PY - CurBlast->PY) < BLASTRADIUS)
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
