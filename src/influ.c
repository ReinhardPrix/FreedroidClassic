/*=@Header==============================================================
 * $Source$
 *
 * @Desc: influence - related functions 
 *	 
 * 	
 * $Revision$
 * $State$
 *
 * $Author$
 *
 * $Log$
 * Revision 1.2  1994/06/19 16:22:55  prix
 * Wed Jun 08 13:52:40 1994: Influ moves only when beam finished
 *
 * Revision 1.1  1993/08/04  16:27:43  prix
 * Initial revision
 *
 *
 *-@Header------------------------------------------------------------*/
static const char RCSid[]=\
"$Id$";

#define _influ_c

#include <stdio.h>
#include <stdlib.h>
#include <alloc.h>
#include <dos.h>
#include <math.h>
#include <conio.h>

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"

#define NOSTRAIGHTDIR 112
#define TIMETILLNEXTBULLET 14

#define REFRESH_ENERGY		3
#define COLLISION_PUSHSPEED	4

#define BOUNCE_LOSE_ENERGY 3		/* amount of lose-energy at enemy-collisions */
#define BOUNCE_LOSE_FACT 1
void BounceLoseEnergy(int enemynum); /* influ can lose energy on coll. */
void PermanentLoseEnergy(void);		/* influ permanently loses energy */
int NoInfluBulletOnWay(void);
long MyAbs(long);

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
signed long MyAbs(signed long Wert){
	if (Wert < 0) return (-Wert);
	return Wert;
}

/*@Function============================================================
@Desc: Fires Bullets automatically

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void AutoFireBullet(void){
	int j,i;
	int TargetNum= -1;
	signed long BestDist=200000;
	int enemynum, guntype;
	int xdist, ydist;
	signed long LDist,LXDist,LYDist;
	
	if (CurLevel->empty) return;

	if (Me.firewait) return;
		Me.firewait=Bulletmap[Druidmap[Me.type].gun].WaitNextTime;
	
		// Nummer der Schu"szieles herausfinden
		for (i=0;i<MAX_ENEMYS_ON_SHIP;i++) {
			if (Feindesliste[i].Status == OUT) continue;
			if (Feindesliste[i].levelnum != CurLevel->levelnum) continue;
			if (!IsVisible( &Feindesliste[i].pos)) continue;
			LXDist=(Feindesliste[i].pos.x-Me.pos.x);
			LYDist=(Feindesliste[i].pos.y-Me.pos.y);
			LDist=LXDist*LXDist + LYDist*LYDist;
			if (LDist <= 0) { printf(" ERROR determination of LDist !!."); getch(); Terminate(-1); }
			if ( LDist < BestDist) {
				TargetNum=i;
				enemynum=i;
				BestDist=LDist;
			}
		}
		if (TargetNum == -1) {
//			gotoxy(1,1);
//			printf(" Sorry, nobody in reach.");
//			getch();
//			Terminate(-1);
			return;
		}

		FireBulletSound();

		xdist=Feindesliste[enemynum].pos.x-Me.pos.x;
		ydist=Feindesliste[enemynum].pos.y-Me.pos.y;

		// Sicherheit gegen Divisionen durch Null !!!!
		if (xdist == 0) xdist=2;
		if (ydist == 0) ydist=2;
		if (xdist == 1) xdist=2;
		if (ydist == 1) ydist=2;
		if (xdist == -1) xdist=2;
		if (ydist == -1) ydist=2;
		
		guntype=Druidmap[Me.type].gun;
		
		/* Einen bulleteintragg suchen, der noch nicht belegt ist */
		for (j=0;j<MAXBULLETS-1;j++) {
			if (AllBullets[j].type == OUT) break; 
		}

		/* Schussrichtung festlegen */
		if (abs(xdist) > abs(ydist) ) {
			AllBullets[j].SX=Bulletmap[guntype].speed;
			AllBullets[j].SY=ydist*AllBullets[j].SX/xdist;
			if (xdist < 0) {
				AllBullets[j].SX=-AllBullets[j].SX;
				AllBullets[j].SY=-AllBullets[j].SY;
			}
		} 
  		
		if (abs(xdist) < abs(ydist) ) {
			AllBullets[j].SY=Bulletmap[guntype].speed;
			AllBullets[j].SX=xdist*AllBullets[j].SY/ydist;
			if (ydist < 0) {
				AllBullets[j].SX=-AllBullets[j].SX;
				AllBullets[j].SY=-AllBullets[j].SY;
			}
		}

		/* Schussphase festlegen ( ->phase=Schussbild ) */
		AllBullets[j].phase=NOSTRAIGHTDIR;
		if ((abs(xdist)*2/3)/abs(ydist)) AllBullets[j].phase=RECHTS;
		if ((abs(ydist)*2/3)/abs(xdist)) AllBullets[j].phase=OBEN;
		if (AllBullets[j].phase == NOSTRAIGHTDIR) {
			if (((xdist < 0) && (ydist < 0)) || ((xdist > 0) && (ydist > 0)))
				AllBullets[j].phase=RECHTSUNTEN;
				else AllBullets[j].phase=RECHTSOBEN;
		}

		if (AllBullets[j].SX==0) AllBullets[j].phase=OBEN;
		if (AllBullets[j].SY==0) AllBullets[j].phase=RECHTS;
		
		/* Bullets im Zentrum des Schuetzen starten */
		AllBullets[j].PX=Me.pos.x;
		AllBullets[j].PY=Me.pos.y;
		
		/* Bullets so abfeuern, dass sie nicht den Schuetzen treffen */
		AllBullets[j].PX+=AllBullets[j].SX;
		AllBullets[j].PY+=AllBullets[j].SY;
		AllBullets[j].PX+=Me.speed.x;
		AllBullets[j].PY+=Me.speed.y;
			
		/* Bullettype gemaes dem ueblichen guntype fuer den robottyp setzen */
		AllBullets[j].type=guntype;
}


/*@Function============================================================
@Desc: Diese Funktion bewegt den Influencer gemaess seiner momentanen
	Geschwindigkeit. Ausserdem wird er weitergedreht gemaess der momentanen
   Energie.
@Ret: keiner
@Int: keiner
* $Function----------------------------------------------------------*/
void MoveInfluence(void)
{
	unsigned char MapBrick;
	signed int accel = Druidmap[Me.type].accel;
	static TransferCounter = 0;
	/* zum Bremsen der Drehung, wenn man auf der Taste bleibt: */
	static int counter=-1;

  	if (BeamLine) return;
	
	counter ++;
	counter %= BREMSDREHUNG;		/* Wird mal vom Druid abhaengen */

	PermanentLoseEnergy();			/* influ permanently loses energy */
	
	/* Checken, ob Influencer noch OK */
	if( Me.energy <= 0) {
		if (Me.type != DRUID001) {
			Me.type = DRUID001;
			RedrawInfluenceNumber();
			Me.speed.x = 0;
			Me.speed.y = 0;
			Me.energy = PreTakeEnergy;
			Me.health = BLINKENERGY;
			StartBlast(Me.pos.x,Me.pos.y,DRUIDBLAST);
		} else {
			Me.status = OUT;
			ThouArtDefeated();
		}
	}

	/* Time passed before entering Transfermode ?? */
	if( TransferCounter && (TransferCounter-- == 1) ) Me.status = TRANSFERMODE;
	
	if (UpPressed) SpeedY-= accel;
	if (DownPressed) SpeedY+= accel;
	if (LeftPressed) SpeedX-= accel;
	if (RightPressed) SpeedX+= accel;

	if (!SpacePressed) Me.status=MOBILE;

	if( TransferCounter == 1 ) {
		Me.status = TRANSFERMODE;
		TransferCounter = 0;
	}
	
	if ((SpacePressed) && (NoDirectionPressed()) &&
			(Me.status!=WEAPON) && (Me.status != TRANSFERMODE) && (!TransferCounter) )
		TransferCounter = WAIT_TRANSFERMODE;
		
	if ((SpacePressed) && (!NoDirectionPressed()) &&
			(Me.status != TRANSFERMODE) )
		Me.status=WEAPON;

	if (Me.autofire) AutoFireBullet(); else
		if ((SpacePressed) && (!NoDirectionPressed()) && (Me.status == WEAPON) &&
			(Me.firewait == 0) && (NoInfluBulletOnWay())) FireBullet();

	/* Checken, ob auf Sonder-Feld (Lift, Konsole) und im Transfermode */
	ActSpecialField(Me.pos.x, Me.pos.y);

	
} /* MoveInfluence */


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
int NoInfluBulletOnWay(void)
{
	int i;

	if (PlusExtentionsOn) return TRUE;
	if (!Bulletmap[Druidmap[Me.type].gun].oneshotonly) return TRUE;
	
	for (i=0;i<MAXBULLETS;i++) {
		if ( (AllBullets[i].type != OUT) && (AllBullets[i].mine) ) return FALSE;
	}
	
	return TRUE;
}

/*@Function============================================================
@Desc: AnimateInfluence: zaehlt die Phasen weiter, falls der Roboter
							mehrphasig ist
@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void AnimateInfluence(void) {
	int gruen,blau,rot;
	static Teilphase;
	static unsigned char Palwert=0;
	static int blinkwaiter=0;
	static int maxblinkvalue=3;
	static int Crywait=1;
	static int Overtaketunewait=2;
	
	/*
	 * Phase des Influencers in fein gestuften Schritten weiterz"ahlen
	 */
	 
	Teilphase+=Me.energy;
	Me.phase=Teilphase/(Druidmap[DRUID001].maxenergy);
	if (Me.phase >= ENEMYPHASES) {
		Me.phase=0;
		Teilphase=0;
	}


	/*
	 * Farbe des Influencers (15) richtig setzen
	 */
	 
	if ((Me.status == TRANSFERMODE) && (Me.energy > BLINKENERGY)) SetPalCol(INFLUENCEFARBWERT,Transfercolor.rot,Transfercolor.gruen,Transfercolor.blau);

	if ((Me.status == MOBILE) && (Me.energy > BLINKENERGY))
		SetPalCol(INFLUENCEFARBWERT, Mobilecolor.rot, Mobilecolor.gruen,
			Mobilecolor.blau);
	
	/*
	 * Wenn die Energie den kritischen Stand erreicht hat,
	 * beginnt der Robot zu blinken
	 */
		 
#define CRYWAITTIME 14

	if ((Me.energy <= BLINKENERGY) && (blinkwaiter == 0)) {
//		Palwert+=15;
//		if(Palwert>63) Palwert=0;
		Palwert=random(64);
		if (Me.status == TRANSFERMODE) SetPalCol(INFLUENCEFARBWERT,63,Palwert,Palwert);
		else SetPalCol(INFLUENCEFARBWERT,Palwert,Palwert,Palwert);
	}
	if (Me.energy <= BLINKENERGY){
		if (Crywait > 0) Crywait--;
		else {
			Crywait=CRYWAITTIME+Me.energy;
			CrySound();
		}
	}
	if (Me.status == TRANSFERMODE) {
		if (Overtaketunewait > 0) Overtaketunewait--;
		else {
			Overtaketunewait=12;
			StartSound(6);
		}
	}
	blinkwaiter++;
	blinkwaiter %= 3;
}

/*@Function============================================================
@Desc: BounceInfluencer: prueft Kollisionen mit Tueren und Mauer
								mit DruidPassable() und wirft Influencer
								entsprechend zurueck

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void BounceInfluencer(void)
{	int sign;
	int SX=SpeedX, SY=SpeedY;
	point lastpos;
	int res;		/* Ergebnis aus DruidPassable() */
	int fx, fy;		/* Feinkoordinaten */
	int gx, gy;		/* Grobkoordinaten */
	int safty_sx, safty_sy;	/* wegstoss - Geschwindigkeiten (falls noetig)*/

	int crashx = FALSE, crashy = FALSE;		/* Merker wo kollidiert wurde */
	
	lastpos.x = Me.pos.x - SX;
	lastpos.y = Me.pos.y - SY;

	res=DruidPassable(Me.pos.x, Me.pos.y);
	switch (res) {
		case -1:
			/* Influence ist blockiert: zurueckwerfen */
   
		   /* Festellen, in welcher Richtung die Mauer lag,
			und den Influencer entsprechend stoppen */
			if (SX && (DruidPassable(lastpos.x+SX, lastpos.y) != CENTER) ) {
				crashx = TRUE;		/* In X wurde gecrasht */
				sign = (SX < 0) ? -1 : 1;
				SX = abs(SX);
				while( --SX && (DruidPassable(lastpos.x+sign*SX,lastpos.y)!=CENTER));
				Me.pos.x = lastpos.x + SX*sign;
				SpeedX = 0;
				
				/* falls Influencer weggestossen werden muss ! */
				safty_sx = (-1)*sign*PUSHSPEED;
			}
			
			if (SY && (DruidPassable(lastpos.x, lastpos.y+SY)!= CENTER) ) {
				crashy = TRUE;		/* in Y wurde gecrasht */
				sign = (SY < 0) ? -1 : 1;
				SY = abs(SY);
				while(--SY && (DruidPassable(lastpos.x,lastpos.y+sign*SY)!=CENTER));
				Me.pos.y = lastpos.y + SY*sign;
				SpeedY = 0;

				/* Falls Influencer weggestossen werden muss */
				safty_sy = (-1)*sign*PUSHSPEED;
			}

			/* Hat das nichts geholfen, noch etwas wegschubsen */
			if( DruidPassable(Me.pos.x, Me.pos.y) != CENTER) {
				if (crashx) {
					Me.speed.x = safty_sx;
					Me.pos.x += Me.speed.x;
				}
				
				if (crashy) {
					Me.speed.y = safty_sy;
					Me.pos.y += Me.speed.y;
				}
			}
			
			break;
			
		/* Von Tuerrand wegschubsen */
		case OBEN:
			Me.speed.y = -PUSHSPEED;
			Me.pos.y += Me.speed.y;
			break;
			
		case UNTEN:
			Me.speed.y = PUSHSPEED;
			Me.pos.y += Me.speed.y;
			break;
			
		case RECHTS:
			Me.speed.x = PUSHSPEED;
			Me.pos.x += Me.speed.x;
			break;
			
		case LINKS:
			Me.speed.x = -PUSHSPEED;
			Me.pos.x += Me.speed.x;
			break;

		/* Not blocked at all ! */
		case CENTER:
			break;
		
		default:
			printf("Illegal return value from DruidPassable() ");
			Terminate(-1);
			break;
				

	} /* switch */
} /* BounceInfluencer */

/*@Function============================================================
@Desc: Dies Prozedur passt die momentane Geschwindigkeit an die Hoechst-
	geschwindigkeit an.
@Ret: keiner
@Int: keiner
* $Function----------------------------------------------------------*/
void AdjustSpeed(void)
{
int maxspeed=Druidmap[Me.type].maxspeed;
	if (Me.speed.x > maxspeed) Me.speed.x = maxspeed;
   if (Me.speed.x < (-maxspeed)) Me.speed.x = (-maxspeed);
   
	if (Me.speed.y > maxspeed) Me.speed.y = maxspeed;
   if (Me.speed.y < (-maxspeed) ) Me.speed.y = (-maxspeed);
}


/*@Function============================================================
@Desc: Diese Funktion reduziert die Fahrt des Influencers sobald keine
	Taste Richtungstaste mehr gedrueckt ist

@Ret: keiner

* $Function----------------------------------------------------------*/
void Reibung(void){
	if ( !UpPressed && !DownPressed) {
		if (SpeedY < 0) SpeedY ++;
		if (SpeedY > 0) SpeedY --;
	}
	if ( !RightPressed && !LeftPressed) {
		if (SpeedX < 0) SpeedX ++;
		if (SpeedX > 0) SpeedX --;
	}
}

/*@Function============================================================
@Desc: ExplodeInfluencer(): generiert eine grosse Explosion an
				der Position des Influencers

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void ExplodeInfluencer(void)
{
	int i;
	int counter;
	
	Me.status = OUT;

	
	/* ein paar versetze Explosionen */
	for(i = 0; i<4; i++) {
		/* freien Blast finden */
		counter = 0;
		while( AllBlasts[counter++].type != OUT);
		counter -= 1;
		AllBlasts[counter].type = DRUIDBLAST;
		AllBlasts[counter].PX = Me.pos.x -DRUIDRADIUSX/2 + random(DRUIDRADIUSX);
		AllBlasts[counter].PY = Me.pos.y - DRUIDRADIUSY/2 + random(DRUIDRADIUSY);
		AllBlasts[counter].phase = i;
	}
	
	
} /* ExplodeInfluencer */

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void InfluenceEnemyCollision(void) {
	int i;
	int xdist;
	int ydist;
	long dist2;
	int swap;
	int first_collision = TRUE;		/* marker */
	
	for (i=0;i<NumEnemys;i++) {
		/* ignore debug-enemys */
		if( Feindesliste[i].type == DEBUG_ENEMY ) continue;
		
		/* ignore enemy that are not on this level or dead */
		if ( Feindesliste[i].levelnum != CurLevel->levelnum ) continue;
		if ( Feindesliste[i].Status == OUT ) continue;

		xdist = Me.pos.x - Feindesliste[i].pos.x;
		ydist = Me.pos.y - Feindesliste[i].pos.y;
		
		if( abs(xdist) > BLOCKBREITE ) continue;
		if( abs(ydist) > BLOCKHOEHE  ) continue;
		
		dist2 = (long)xdist*xdist + ydist*ydist;
		if( dist2 > (long)4*DRUIDRADIUSX*DRUIDRADIUSY ) continue;

				
		if (Me.status != TRANSFERMODE) {

			if( first_collision ) { /* nur beim ersten mal !!! */
				/* den Geschwindigkeitsvektor des Influencers invertieren */
			  	Me.speed.x = - Me.speed.x ;
			  	Me.speed.y = - Me.speed.y ;
				if( SpeedX != 0 )
					Me.speed.x += COLLISION_PUSHSPEED * (SpeedX/abs(SpeedX));
				else if( xdist)
					Me.speed.x = COLLISION_PUSHSPEED * (xdist/abs(xdist));
				if( SpeedY != 0 )
					Me.speed.y += COLLISION_PUSHSPEED * (SpeedY/abs(SpeedY));
				else if( ydist)
					Me.speed.y = COLLISION_PUSHSPEED * (ydist/abs(ydist));
				
				/* den Influencer etwas aus dem Feind hinausschieben */	 
				Me.pos.x += SpeedX;
				Me.pos.y += SpeedY;

				/* etwaige Wand - collisionen beruecksichtigen */
				BounceInfluencer();

				BounceSound();
				
			} /* if first_collision */
			
			/* Den Feind kurz stoppen und dann umdrehen */
			if( !Feindesliste[i].warten ) {
				Feindesliste[i].warten = WAIT_COLLISION;
				swap = Feindesliste[i].nextwaypoint;
				Feindesliste[i].nextwaypoint = Feindesliste[i].lastwaypoint;
				Feindesliste[i].lastwaypoint = swap;
			} 
			BounceLoseEnergy(i); /* someone loses energy ! */

		} else {
			Takeover(i);
				
			if (LevelEmpty()) CurLevel->empty=WAIT_LEVELEMPTY;
			
		} /* if !Transfer else .. */
		
	} /* for */
	
} /* InfluenceEnemyCollision */

/*@Function============================================================
@Desc: Fire-Routine for the Influencer only !! (should be changed)

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void FireBullet(void){
	int i=0;
	Bullet CurBullet=NULL;		/* das Bullet, um das es jetzt geht */
	int guntype = Druidmap[Me.type].gun;	/* which gun do we have ? */
	int BulletSpeedX = Bulletmap[guntype].speed;
	int BulletSpeedY = Bulletmap[guntype].speed;
	int firedir;

/* Wenn noch kein Schuss loesbar ist sofort zurueck */
	if (Me.firewait > 0) return;
	Me.firewait = Bulletmap[guntype].WaitNextTime;

/* Geraeusch eines geloesten Schusses fabrizieren */
	FireBulletSound();

/* Naechste Freie Bulletposition suchen */
	for(i=0;i<(MAXBULLETS);i++){
    	if (AllBullets[i].type == OUT) {
			CurBullet=&AllBullets[i];
			break;
   	}
   }

   /* Kein freies Bullet gefunden: Nimm das erste */
	if (CurBullet == NULL) CurBullet=&AllBullets[0];

	CurBullet->PX=Me.pos.x;
	CurBullet->PY=Me.pos.y;
   CurBullet->type=guntype;
   CurBullet->mine = TRUE;
   CurBullet->owner = -1;

	if (DownPressed) firedir=UNTEN;
	if (UpPressed) firedir=OBEN;
	if (LeftPressed) firedir=LINKS;
	if (RightPressed) firedir=RECHTS;
	if (RightPressed && DownPressed) firedir=RECHTSUNTEN;
	if (LeftPressed && DownPressed) firedir=LINKSUNTEN;
	if (LeftPressed && UpPressed) firedir=LINKSOBEN; 
	if (RightPressed && UpPressed) firedir=RECHTSOBEN;

	switch (firedir) {
		case OBEN:
			CurBullet->SX = 0;
			CurBullet->SY = -BulletSpeedY;
			CurBullet->phase = OBEN;
			break;

		case RECHTSOBEN:
        	CurBullet->SX = BulletSpeedX; 
			CurBullet->SY = -BulletSpeedY;
			CurBullet->phase = RECHTSOBEN;
			break;
			
		case RECHTS:
			CurBullet->SX = BulletSpeedX;
			CurBullet->SY = 0;
			CurBullet->phase = RECHTS;
			break;
			
		case RECHTSUNTEN:
			CurBullet->SX = BulletSpeedX;
        	CurBullet->SY = BulletSpeedY;
        	CurBullet->phase = RECHTSUNTEN;
			break;
			
		case UNTEN:
			CurBullet->SX = 0;
			CurBullet->SY = BulletSpeedY;
			CurBullet->phase = OBEN;
			break;
			
		case LINKSUNTEN:
			CurBullet->SX = -BulletSpeedX;		
			CurBullet->SY = BulletSpeedY;
			CurBullet->phase = RECHTSOBEN;
			break;
			
		case LINKS:
			CurBullet->SX = -BulletSpeedX;
			CurBullet->SY = 0;
			CurBullet->phase = RECHTS;
			break;
			
		case LINKSOBEN:
			CurBullet->SX = -BulletSpeedX;
			CurBullet->SY = -BulletSpeedY;
        	CurBullet->phase = RECHTSUNTEN;
			break;
    }

/* Um Selbstabschuss zu verhindern Bullet weiterbewegen */
	
		CurBullet->PX+=CurBullet->SX;
		CurBullet->PY+=CurBullet->SY;
		if ((abs(BulletSpeedX) < 13) && (abs(BulletSpeedY) < 13)) {
			CurBullet->PX+=CurBullet->SX;
			CurBullet->PY+=CurBullet->SY;
		}
		
    /*
     * F"ur Geschosse gilt die alsolute Gescho"sgeschwindigkeit am c-64
	  * und die Newtonsche Physik mit Vektoraddition bei Paraplus.
	  *
	  * Allerdings wird nicht die ganze Fahrt auf den Schu"s "ubertragen
	  * Die Robotter sollen einen Wert haben, der angibt ob durch einen
	  * "Velocityneutralisator" die Fahrt ganz, halb ect. oder gar keine
	  * auswirkungen hat.
	  *
	  * Eventuell sollte dieser Menupunkt auch ausschaltbar sein.
	  *
	  */

	if (PlusExtentionsOn) {
		if (!Me.vneut) {
			CurBullet->SX+=SpeedX / Druidmap[Me.type].vneutral;
			CurBullet->SY+=SpeedY / Druidmap[Me.type].vneutral;
		}
	}

	return;
	
}	/* FireBullet */

/*@Function============================================================
@Desc: RefreshInfluencer(): Schuss- und Kollisions Verluste wieder
							auffrischen 

@Ret: void
@In
* $Function----------------------------------------------------------*/
void RefreshInfluencer(void)
{
	static int timecounter=3;		/* to slow down healing process */

	if( --timecounter ) return;
	if( timecounter == 0 ) timecounter = 3;
	
	if( Me.energy < Me.health ) {
		Me.energy += REFRESH_ENERGY;
		RealScore -= REFRESH_ENERGY*5;
		if ( Me.energy > Me.health ) Me.energy = Me.health;
		RefreshSound();
	}

	return;
} /* RefreshInfluence */
			
/*@Function============================================================
@Desc: BounceLoseEnergy(): influ-enemy collisions are sucking someones
								energy, depending on colliding types

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void BounceLoseEnergy(int enemynum)
{
	int enemytype = Feindesliste[enemynum].type;

	
	if( Me.type <= enemytype ) {
		if( InvincibleMode ) return;
		Me.energy -= (Druidmap[enemytype].class - Druidmap[Me.type].class)*BOUNCE_LOSE_FACT;
	} else Feindesliste[enemynum].energy -=
		(Druidmap[Me.type].class - Druidmap[enemynum].class)*BOUNCE_LOSE_FACT;
		
//	else Feindesliste[enemynum].energy -= BOUNCE_LOSE_ENERGY;
		
	return;
}	

/*@Function============================================================
@Desc: PermanentLoseEnergy(): staendiger Energieverlust des Influ

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void PermanentLoseEnergy(void)
{
	static time_counter=2*18; /* to obtain approx. 2 sec. */

	if( InvincibleMode ) return;
	
	if( --time_counter ) return; /* wait */
	
	if( time_counter == 0 ) time_counter = 2*18;	/* ca. 2 sec. */

	/* health decreases with time */
	Me.health -= Druidmap[Me.type].lose_health;
	
	/* you cant have more energy than health */
	if( Me.energy > Me.health ) Me.energy = Me.health;

	return;
}

#undef _influ_c
