/* 
 *
 *   Copyright (c) 1994, 2002 Johannes Prix
 *   Copyright (c) 1994, 2002 Reinhard Prix
 *
 *
 *  This file is part of FreeDroid
 *
 *  FreeDroid is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  FreeDroid is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with FreeDroid; see the file COPYING. If not, write to the 
 *  Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
 *  MA  02111-1307  USA
 *
 */

/*----------------------------------------------------------------------
 *
 * Desc: all features, movement, fireing, collision and extras of the
 *	influencer are done in here.
 *
 *----------------------------------------------------------------------*/
#include <config.h>

#define _influ_c

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"

#define NOSTRAIGHTDIR 112
#define TIMETILLNEXTBULLET 14

#define REFRESH_ENERGY		3
#define COLLISION_PUSHSPEED	4

#define BOUNCE_LOSE_ENERGY 3	/* amount of lose-energy at enemy-collisions */
#define BOUNCE_LOSE_FACT 1
void BounceLoseEnergy (int enemynum);	/* influ can lose energy on coll. */
void PermanentLoseEnergy (void);	/* influ permanently loses energy */
int NoInfluBulletOnWay (void);
long MyAbs (long);

int
isignf (float Wert)
{
  if (Wert == 0)
    return 0;
  return (Wert / fabsf (Wert));
}				// int sign

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
signed long
MyAbs (signed long Wert)
{
  if (Wert < 0)
    return (-Wert);
  return Wert;
}

/*@Function============================================================
@Desc: Fires Bullets automatically

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
AutoFireBullet (void)
{
  int j, i;
  int TargetNum = -1;
  signed long BestDist = 200000;
  int guntype;
  int xdist, ydist;
  signed long LDist, LXDist, LYDist;

  if (CurLevel->empty)
    return;

  if (Me.firewait)
    return;
  Me.firewait = Bulletmap[Druidmap[Me.type].gun].WaitNextTime;

  // Nummer der Schu"szieles herausfinden
  for (i = 0; i < MAX_ENEMYS_ON_SHIP; i++)
    {
      if (Feindesliste[i].Status == OUT)
	continue;
      if (Feindesliste[i].levelnum != CurLevel->levelnum)
	continue;
      if (!IsVisible (&Feindesliste[i].pos))
	continue;
      LXDist = (Feindesliste[i].pos.x - Me.pos.x);
      LYDist = (Feindesliste[i].pos.y - Me.pos.y);
      LDist = LXDist * LXDist + LYDist * LYDist;
      if (LDist <= 0)
	{
	  DebugPrintf (" ERROR determination of LDist !!.");
	  getchar ();
	  Terminate (-1);
	}
      if (LDist < BestDist)
	{
	  TargetNum = i;
	  BestDist = LDist;
	}
    }
  if (TargetNum == -1)
    {
      //                  gotoxy(1,1);
      //                  printf(" Sorry, nobody in reach.");
      //                  getchar();
      //                  Terminate(-1);
      return;
    }

  guntype = Druidmap[Me.type].gun;

  Fire_Bullet_Sound ( guntype );

  xdist = Feindesliste[TargetNum].pos.x - Me.pos.x;
  ydist = Feindesliste[TargetNum].pos.y - Me.pos.y;

  // Sicherheit gegen Divisionen durch Null !!!!
  if (xdist == 0)
    xdist = 2;
  if (ydist == 0)
    ydist = 2;
  if (xdist == 1)
    xdist = 2;
  if (ydist == 1)
    ydist = 2;
  if (xdist == -1)
    xdist = 2;
  if (ydist == -1)
    ydist = 2;

  /* Einen bulleteintragg suchen, der noch nicht belegt ist */
  for (j = 0; j < MAXBULLETS - 1; j++)
    {
      if (AllBullets[j].type == OUT)
	break;
    }

  /* Schussrichtung festlegen */
  if (abs (xdist) > abs (ydist))
    {
      AllBullets[j].speed.x = Bulletmap[guntype].speed;
      AllBullets[j].speed.y = ydist * AllBullets[j].speed.x / xdist;
      if (xdist < 0)
	{
	  AllBullets[j].speed.x = -AllBullets[j].speed.x;
	  AllBullets[j].speed.y = -AllBullets[j].speed.y;
	}
    }

  if (abs (xdist) < abs (ydist))
    {
      AllBullets[j].speed.x = Bulletmap[guntype].speed;
      AllBullets[j].speed.y = xdist * AllBullets[j].speed.y / ydist;
      if (ydist < 0)
	{
	  AllBullets[j].speed.x = -AllBullets[j].speed.x;
	  AllBullets[j].speed.y = -AllBullets[j].speed.y;
	}
    }

  /* Schussphase festlegen ( ->phase=Schussbild ) */
  AllBullets[j].phase = NOSTRAIGHTDIR;
  if ((abs (xdist) * 2 / 3) / abs (ydist))
    AllBullets[j].phase = RECHTS;
  if ((abs (ydist) * 2 / 3) / abs (xdist))
    AllBullets[j].phase = OBEN;
  if (AllBullets[j].phase == NOSTRAIGHTDIR)
    {
      if (((xdist < 0) && (ydist < 0)) || ((xdist > 0) && (ydist > 0)))
	AllBullets[j].phase = RECHTSUNTEN;
      else
	AllBullets[j].phase = RECHTSOBEN;
    }

  if (AllBullets[j].speed.x == 0)
    AllBullets[j].phase = OBEN;
  if (AllBullets[j].speed.y == 0)
    AllBullets[j].phase = RECHTS;

  /* Bullets im Zentrum des Schuetzen starten */
  AllBullets[j].pos.x = Me.pos.x;
  AllBullets[j].pos.y = Me.pos.y;

  /* Bullets so abfeuern, dass sie nicht den Schuetzen treffen */
  AllBullets[j].pos.x += AllBullets[j].speed.x;
  AllBullets[j].pos.y += AllBullets[j].speed.y;
  AllBullets[j].pos.x += Me.speed.x;
  AllBullets[j].pos.y += Me.speed.y;

  /* Bullettype gemaes dem ueblichen guntype fuer den robottyp setzen */
  AllBullets[j].type = guntype;
}				// void AutoFireBullet(void)


/*@Function============================================================
@Desc: Diese Funktion bewegt den Influencer gemaess seiner momentanen
	Geschwindigkeit. Ausserdem wird er weitergedreht gemaess der momentanen
   Energie.
@Ret: keiner
@Int: keiner
* $Function----------------------------------------------------------*/
void
MoveInfluence (void)
{
  signed int accel = Druidmap[Me.type].accel;
  static float TransferCounter = 0;
  /* zum Bremsen der Drehung, wenn man auf der Taste bleibt: */
  static int counter = -1;

  DebugPrintf ("\nvoid MoveInfluence(void):  Real function call confirmed.");
  // printf("\nvoid MoveInfluence(void):  accel ist jetzt: %d.",accel);

  if (BeamLine)
    return;

  counter++;
  counter %= BREMSDREHUNG;	/* Wird mal vom Druid abhaengen */

  PermanentLoseEnergy ();	/* influ permanently loses energy */

  /* Checken, ob Influencer noch OK */
  if (Me.energy <= 0)
    {
      if (Me.type != DRUID001)
	{
	  Me.type = DRUID001;
	  RedrawInfluenceNumber ();
	  Me.speed.x = 0;
	  Me.speed.y = 0;
	  Me.energy = PreTakeEnergy;
	  Me.health = BLINKENERGY;
	  StartBlast (Me.pos.x, Me.pos.y, DRUIDBLAST);
	}
      else
	{
	  Me.status = OUT;
	  ThouArtDefeated ();
	  DebugPrintf
	    ("\nvoid MoveInfluence(void):  Alternate end of function reached.");
	  return;
	}
    }

  /* Time passed before entering Transfermode ?? */
  if ( TransferCounter >= WAIT_TRANSFERMODE )
    {
      Me.status = TRANSFERMODE;
      TransferCounter=0;
    }

  if (UpPressed ())
    Me.speed.y -= accel;
  if (DownPressed ())
    Me.speed.y += accel;
  if (LeftPressed ())
    Me.speed.x -= accel;
  if (RightPressed ())
    Me.speed.x += accel;

  if (!SpacePressed ())
    Me.status = MOBILE;

  if (TransferCounter == 1)
    {
      Me.status = TRANSFERMODE;
      TransferCounter = 0;
    }

  if ((SpacePressed ()) && (NoDirectionPressed ()) &&
      (Me.status != WEAPON) && (Me.status != TRANSFERMODE) )
    TransferCounter += Frame_Time();

  if ((SpacePressed ()) && (!NoDirectionPressed ()) &&
      (Me.status != TRANSFERMODE))
    Me.status = WEAPON;

  if (Me.autofire)
    AutoFireBullet ();
  else
    if ((SpacePressed ()) && (!NoDirectionPressed ()) && (Me.status == WEAPON)
	&& (Me.firewait == 0) && (NoInfluBulletOnWay ()))
    FireBullet ();

  /* Checken, ob auf Sonder-Feld (Lift, Konsole) und im Transfermode */
  ActSpecialField (Me.pos.x, Me.pos.y);

  DebugPrintf ("\nvoid MoveInfluence(void):  Usual end of function reached.");
}				/* MoveInfluence */


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
int
NoInfluBulletOnWay (void)
{
  int i;

  if (PlusExtentionsOn)
    return TRUE;

  if (!Bulletmap[Druidmap[Me.type].gun].oneshotonly)
    return TRUE;

  for (i = 0; i < MAXBULLETS; i++)
    {
      if ((AllBullets[i].type != OUT) && (AllBullets[i].mine))
	return FALSE;
    }

  return TRUE;
}

/*@Function============================================================
@Desc: AnimateInfluence: zaehlt die Phasen weiter, falls der Roboter
							mehrphasig ist
@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
AnimateInfluence (void)
{
  static unsigned char Palwert = 0;
  static int blinkwaiter = 0;
  static int Crywait = 1;
  static int Overtaketunewait = 2;

  /*
   * Phase des Influencers in fein gestuften Schritten weiterz"ahlen
   */


  Me.phase +=
    (Me.energy / (Druidmap[DRUID001].maxenergy)) * Frame_Time () *
    ENEMYPHASES * 3;
  if (((int) rintf (Me.phase)) >= ENEMYPHASES)
    {
      Me.phase = 0;
    }


  /*
   * Farbe des Influencers (15) richtig setzen
   */

  if ((Me.status == TRANSFERMODE) && (Me.energy > BLINKENERGY))
    SetPalCol (INFLUENCEFARBWERT, Transfercolor.rot, Transfercolor.gruen,
	       Transfercolor.blau);

  if ((Me.status == MOBILE) && (Me.energy > BLINKENERGY))
    SetPalCol (INFLUENCEFARBWERT, Mobilecolor.rot, Mobilecolor.gruen,
	       Mobilecolor.blau);

  /*
   * Wenn die Energie den kritischen Stand erreicht hat,
   * beginnt der Robot zu blinken
   */

#define CRYWAITTIME 14

  if ((Me.energy <= BLINKENERGY) && (blinkwaiter == 0))
    {
      //          Palwert+=15;
      //          if(Palwert>63) Palwert=0;
      Palwert = MyRandom (64);
      if (Me.status == TRANSFERMODE)
	SetPalCol (INFLUENCEFARBWERT, 63, Palwert, Palwert);
      else
	SetPalCol (INFLUENCEFARBWERT, Palwert, Palwert, Palwert);
    }
  if (Me.energy <= BLINKENERGY)
    {
      if (Crywait > 0)
	Crywait--;
      else
	{
	  Crywait = CRYWAITTIME + Me.energy;
	  CrySound ();
	}
    }
  if (Me.status == TRANSFERMODE)
    {
      if (Overtaketunewait > 0)
	Overtaketunewait--;
      else
	{
	  Overtaketunewait = 12;
	  StartSound (6);
	}
    }
  blinkwaiter++;
  blinkwaiter %= 3;
}				// void AnimateInfluence(void)

/*@Function============================================================
@Desc: BounceInfluencer: prueft Kollisionen mit Tueren und Mauer
       mit DruidPassable() und wirft Influencer
       entsprechend zurueck

NEW: This functions HAS to take into account the current framerate!
     

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void
BounceInfluencer (void)
{
  int sign;
  float SX = Me.speed.x * Frame_Time ();
  float SY = Me.speed.y * Frame_Time ();
  finepoint lastpos;
  int res;			/* Ergebnis aus DruidPassable() */
  int safty_sx = 0, safty_sy = 0;	/* wegstoss - Geschwindigkeiten (falls noetig) */

  int crashx = FALSE, crashy = FALSE;	/* Merker wo kollidiert wurde */

  lastpos.x = Me.pos.x - SX;
  lastpos.y = Me.pos.y - SY;

  res = DruidPassable (Me.pos.x, Me.pos.y);

  switch (res)
    {
    case -1:
      /* Influence ist blockiert: zurueckwerfen */

      /* Festellen, in welcher Richtung die Mauer lag,
         und den Influencer entsprechend stoppen */
      if (SX && (DruidPassable (lastpos.x + SX, lastpos.y) != CENTER))
	{
	  crashx = TRUE;	/* In X wurde gecrasht */
	  sign = (SX < 0) ? -1 : 1;
	  SX = abs (SX);
	  while (--SX
		 && (DruidPassable (lastpos.x + sign * SX, lastpos.y) !=
		     CENTER));
	  Me.pos.x = lastpos.x + SX * sign;
	  Me.speed.x = 0;

	  /* falls Influencer weggestossen werden muss ! */
	  safty_sx = (-1) * sign * PUSHSPEED;
	}

      if (SY && (DruidPassable (lastpos.x, lastpos.y + SY) != CENTER))
	{
	  crashy = TRUE;	/* in Y wurde gecrasht */
	  sign = (SY < 0) ? -1 : 1;
	  SY = abs (SY);
	  while (--SY
		 && (DruidPassable (lastpos.x, lastpos.y + sign * SY) !=
		     CENTER));
	  Me.pos.y = lastpos.y + SY * sign;
	  Me.speed.y = 0;

	  /* Falls Influencer weggestossen werden muss */
	  safty_sy = (-1) * sign * PUSHSPEED;
	}

      /* Hat das nichts geholfen, noch etwas wegschubsen */
      if (DruidPassable (Me.pos.x, Me.pos.y) != CENTER)
	{
	  if (crashx)
	    {
	      Me.speed.x = safty_sx;
	      Me.pos.x += Me.speed.x;
	    }

	  if (crashy)
	    {
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
      DebugPrintf ("Illegal return value from DruidPassable() ");
      Terminate (-1);
      break;

    }				/* switch */
}				/* BounceInfluencer */

/*@Function============================================================
@Desc: Dies Prozedur passt die momentane Geschwindigkeit an die Hoechst-
	geschwindigkeit an.
@Ret: keiner
@Int: keiner
* $Function----------------------------------------------------------*/
void
AdjustSpeed (void)
{
  int maxspeed = Druidmap[Me.type].maxspeed;
  if (Me.speed.x > maxspeed)
    Me.speed.x = maxspeed;
  if (Me.speed.x < (-maxspeed))
    Me.speed.x = (-maxspeed);

  if (Me.speed.y > maxspeed)
    Me.speed.y = maxspeed;
  if (Me.speed.y < (-maxspeed))
    Me.speed.y = (-maxspeed);
}				// void AdjustSpeed(void)


/*@Function============================================================
@Desc: Diese Funktion reduziert die Fahrt des Influencers sobald keine
	Taste Richtungstaste mehr gedrueckt ist

@Ret: keiner

* $Function----------------------------------------------------------*/
void
Reibung (void)
{
  int i;

  for (i = 0; i < 5; i++)
    {
      if (!UpPressed () && !DownPressed ())
	{
	  if (Me.speed.y < 0)
	    Me.speed.y++;
	  if (Me.speed.y > 0)
	    Me.speed.y--;
	}
      if (!RightPressed () && !LeftPressed ())
	{
	  if (Me.speed.x < 0)
	    Me.speed.x++;
	  if (Me.speed.x > 0)
	    Me.speed.x--;
	}
    }
}				// void Reibung(void)

/*@Function============================================================
@Desc: ExplodeInfluencer(): generiert eine grosse Explosion an
				der Position des Influencers

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void
ExplodeInfluencer (void)
{
  int i;
  int counter;

  Me.status = OUT;

  DebugPrintf
    ("\nvoid ExplodeInfluencer(void): Real function call confirmed.");

  /* ein paar versetze Explosionen */
  for (i = 0; i < 4; i++)
    {
      /* freien Blast finden */
      counter = 0;
      while (AllBlasts[counter++].type != OUT);
      counter -= 1;
      AllBlasts[counter].type = DRUIDBLAST;
      AllBlasts[counter].PX =
	Me.pos.x - DRUIDRADIUSX / 2 + MyRandom (DRUIDRADIUSX);
      AllBlasts[counter].PY =
	Me.pos.y - DRUIDRADIUSY / 2 + MyRandom (DRUIDRADIUSY);
      AllBlasts[counter].phase = i;
    }

  DebugPrintf
    ("\nvoid ExplodeInfluencer(void): Usual end of function reached.");
}				/* ExplodeInfluencer */

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
InfluenceEnemyCollision (void)
{
  int i;
  int xdist;
  int ydist;
  long dist2;
  int swap;
  int first_collision = TRUE;	/* marker */

  for (i = 0; i < NumEnemys; i++)
    {
      /* ignore debug-enemys */
      if (Feindesliste[i].type == DEBUG_ENEMY)
	continue;

      /* ignore enemy that are not on this level or dead */
      if (Feindesliste[i].levelnum != CurLevel->levelnum)
	continue;
      if (Feindesliste[i].Status == OUT)
	continue;

      xdist = Me.pos.x - Feindesliste[i].pos.x;
      ydist = Me.pos.y - Feindesliste[i].pos.y;

      if (abs (xdist) > BLOCKBREITE)
	continue;
      if (abs (ydist) > BLOCKHOEHE)
	continue;

      dist2 = (long) xdist *xdist + ydist * ydist;
      if (dist2 > (long) 4 * DRUIDRADIUSX * DRUIDRADIUSY)
	continue;


      if (Me.status != TRANSFERMODE)
	{

	  if (first_collision)
	    {			/* nur beim ersten mal !!! */
	      /* den Geschwindigkeitsvektor des Influencers invertieren */
	      Me.speed.x = -Me.speed.x;
	      Me.speed.y = -Me.speed.y;
	      if (Me.speed.x != 0)
		Me.speed.x +=
		  COLLISION_PUSHSPEED * (Me.speed.x / abs (Me.speed.x));
	      else if (xdist)
		Me.speed.x = COLLISION_PUSHSPEED * (xdist / abs (xdist));
	      if (Me.speed.y != 0)
		Me.speed.y +=
		  COLLISION_PUSHSPEED * (Me.speed.y / abs (Me.speed.y));
	      else if (ydist)
		Me.speed.y = COLLISION_PUSHSPEED * (ydist / abs (ydist));

	      /* den Influencer etwas aus dem Feind hinausschieben */
	      Me.pos.x += Me.speed.x * Frame_Time ();
	      Me.pos.y += Me.speed.y * Frame_Time ();

	      /* etwaige Wand - collisionen beruecksichtigen */
	      BounceInfluencer ();

	      BounceSound ();

	      InsertMessage ("void InfEnemColl: Collision detected");

	    }			/* if first_collision */

	  /* Den Feind kurz stoppen und dann umdrehen */
	  if (!Feindesliste[i].warten)
	    {
	      Feindesliste[i].warten = WAIT_COLLISION;
	      swap = Feindesliste[i].nextwaypoint;
	      Feindesliste[i].nextwaypoint = Feindesliste[i].lastwaypoint;
	      Feindesliste[i].lastwaypoint = swap;
	    }
	  BounceLoseEnergy (i);	/* someone loses energy ! */

	}
      else
	{
	  Takeover (i);

	  if (LevelEmpty ())
	    CurLevel->empty = WAIT_LEVELEMPTY;

	}			/* if !Transfer else .. */

    }				/* for */

}				/* InfluenceEnemyCollision */

/*@Function============================================================
@Desc: Fire-Routine for the Influencer only !! (should be changed)

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
FireBullet (void)
{
  int i = 0;
  Bullet CurBullet = NULL;	/* das Bullet, um das es jetzt geht */
  int guntype = Druidmap[Me.type].gun;	/* which gun do we have ? */
  int BulletSpeedX = Bulletmap[guntype].speed;
  int BulletSpeedY = Bulletmap[guntype].speed;
  int firedir = LINKS;

/* Wenn noch kein Schuss loesbar ist sofort zurueck */
  if (Me.firewait > 0)
    return;
  Me.firewait = Bulletmap[guntype].WaitNextTime;

/* Geraeusch eines geloesten Schusses fabrizieren */
  Fire_Bullet_Sound ( guntype );

/* Naechste Freie Bulletposition suchen */
  for (i = 0; i < (MAXBULLETS); i++)
    {
      if (AllBullets[i].type == OUT)
	{
	  CurBullet = &AllBullets[i];
	  break;
	}
    }

  /* Kein freies Bullet gefunden: Nimm das erste */
  if (CurBullet == NULL)
    CurBullet = &AllBullets[0];

  CurBullet->pos.x = Me.pos.x;
  CurBullet->pos.y = Me.pos.y;
  CurBullet->type = guntype;
  CurBullet->mine = TRUE;
  CurBullet->owner = -1;

  if (DownPressed ())
    firedir = UNTEN;
  if (UpPressed ())
    firedir = OBEN;
  if (LeftPressed ())
    firedir = LINKS;
  if (RightPressed ())
    firedir = RECHTS;
  if (RightPressed () && DownPressed ())
    firedir = RECHTSUNTEN;
  if (LeftPressed () && DownPressed ())
    firedir = LINKSUNTEN;
  if (LeftPressed () && UpPressed ())
    firedir = LINKSOBEN;
  if (RightPressed () && UpPressed ())
    firedir = RECHTSOBEN;

  switch (firedir)
    {
    case OBEN:
      CurBullet->speed.x = 0;
      CurBullet->speed.y = -BulletSpeedY;
      CurBullet->phase = OBEN;
      break;

    case RECHTSOBEN:
      CurBullet->speed.x = BulletSpeedX;
      CurBullet->speed.y = -BulletSpeedY;
      CurBullet->phase = RECHTSOBEN;
      break;

    case RECHTS:
      CurBullet->speed.x = BulletSpeedX;
      CurBullet->speed.y = 0;
      CurBullet->phase = RECHTS;
      break;

    case RECHTSUNTEN:
      CurBullet->speed.x = BulletSpeedX;
      CurBullet->speed.y = BulletSpeedY;
      CurBullet->phase = RECHTSUNTEN;
      break;

    case UNTEN:
      CurBullet->speed.x = 0;
      CurBullet->speed.y = BulletSpeedY;
      CurBullet->phase = OBEN;
      break;

    case LINKSUNTEN:
      CurBullet->speed.x = -BulletSpeedX;
      CurBullet->speed.y = BulletSpeedY;
      CurBullet->phase = RECHTSOBEN;
      break;

    case LINKS:
      CurBullet->speed.x = -BulletSpeedX;
      CurBullet->speed.y = 0;
      CurBullet->phase = RECHTS;
      break;

    case LINKSOBEN:
      CurBullet->speed.x = -BulletSpeedX;
      CurBullet->speed.y = -BulletSpeedY;
      CurBullet->phase = RECHTSUNTEN;
      break;
    }

  /* Um Selbstabschuss zu verhindern Bullet weiterbewegen */

  CurBullet->pos.x += isignf (CurBullet->speed.x) * BLOCKBREITE / 2;
  CurBullet->pos.y += isignf (CurBullet->speed.y) * BLOCKHOEHE / 2;

  //  CurBullet->pos.x += Me.speed.x * Frame_Time();
  //  CurBullet->pos.y += Me.speed.y * Frame_Time();

  if ((fabsf (BulletSpeedX) < 13) && (fabsf (BulletSpeedY) < 13))
    {
      CurBullet->pos.x += isignf (CurBullet->speed.x) * BLOCKBREITE / 3;
      CurBullet->pos.y += isignf (CurBullet->speed.y) * BLOCKHOEHE / 3;
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

  if (PlusExtentionsOn)
    {
      if (!Me.vneut)
	{
	  CurBullet->speed.x += Me.speed.x / Druidmap[Me.type].vneutral;
	  CurBullet->speed.y += Me.speed.y / Druidmap[Me.type].vneutral;
	}
    }

  return;

}				/* FireBullet */

/*@Function============================================================
@Desc: RefreshInfluencer(): Refresh fields can be used to regain energy
lost due to bullets or collisions, but not energy lost due to permanent
loss of health in PermanentLoseEnergy.

NEW: this function now takes into account the framerates.

@Ret: void
@In
* $Function----------------------------------------------------------*/
void
RefreshInfluencer (void)
{
  static int timecounter = 3;	/* to slow down healing process */

  if (--timecounter)
    return;
  if (timecounter == 0)
    timecounter = 3;

  if (Me.energy < Me.health)
    {
      Me.energy += REFRESH_ENERGY * Frame_Time () * 2;
      RealScore -= REFRESH_ENERGY * Frame_Time () * 10;
      if (Me.energy > Me.health)
	Me.energy = Me.health;

      if (LastRefreshSound > 0.6)
	{
	  RefreshSound ();
	  LastRefreshSound = 0;
	}
    }

  return;
}				/* RefreshInfluence */

/*@Function============================================================
@Desc: BounceLoseEnergy(): influ-enemy collisions are sucking someones
       energy, depending on colliding types

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void
BounceLoseEnergy (int enemynum)
{
  int enemytype = Feindesliste[enemynum].type;

  if (Me.type <= enemytype)
    {
      if (InvincibleMode)
	return;
      Me.energy -=
	(Druidmap[enemytype].class -
	 Druidmap[Me.type].class) * BOUNCE_LOSE_FACT;
    }
  else
    Feindesliste[enemynum].energy -=
      (Druidmap[Me.type].class -
       Druidmap[enemytype].class) * BOUNCE_LOSE_FACT;

  //    else Feindesliste[enemynum].energy -= BOUNCE_LOSE_ENERGY;

  return;
}				// void BounceLoseEnergy(int enemynum)

/*@Function============================================================
@Desc: PermanentLoseEnergy(): In the classic paradroid game, the influencer
continuously lost energy.  This loss was, in contrast to damage from fighting
and collisions, NOT regainable by using refresh fields.

NEW: this function now takes into account the framerate.

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void
PermanentLoseEnergy (void)
{
  static int time_counter = 2 * 18;	/* to obtain approx. 2 sec. */

  if (InvincibleMode)
    return;

  if (--time_counter)
    return;			/* wait */

  if (time_counter == 0)
    time_counter = 2 * 18;	/* ca. 2 sec. */

  /* health decreases with time */
  Me.health -= Druidmap[Me.type].lose_health * Frame_Time ();

  /* you cant have more energy than health */
  if (Me.energy > Me.health)
    Me.energy = Me.health;

  return;
}				// void PermanentLoseEnergy(void)

#undef _influ_c
