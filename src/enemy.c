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
 * Desc: All enemy - realted functions.  This includes their whole behaviour,
 *	healing, initialization, shuffling them around after evevator-transitions
 *	of the paradroid, deleting them, collisions of enemys among themselves,
 *	their fireing, animation and such.
 *
 *----------------------------------------------------------------------*/
#include <config.h>

#define _enemy_c

#define NOSTRAIGHTDIR 255
#undef ENEMYPHASEDEBUG

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"


#define COL_SPEED		3	/* wegstossen bei enemy-enemy collision */

/* Distance, where to attack Influencer */
// #define FIREDIST2                            INTERNBREITE*INTERNHOEHE*BLOCKBREITE*BLOCKHOEHE/4
#define FIREDIST2	(INTERNBREITE*BLOCKBREITE/2)*(INTERNBREITE*BLOCKBREITE/2)+(INTERNHOEHE*BLOCKHOEHE/2)*(INTERNHOEHE*BLOCKHOEHE/2)

void PermanentHealRobots (void);

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
PermanentHealRobots (void)
{
  static int TimerCounter = 2 * 18;
  int i;

  if (--TimerCounter)
    return;
  if (TimerCounter == 0)
    TimerCounter = 2 * 18;

  for (i = 0; i < MAX_ENEMYS_ON_SHIP; i++)
    {
      if (Feindesliste[i].Status == OUT)
	continue;
      if (Feindesliste[i].energy < Druidmap[Feindesliste[i].type].maxenergy)
	Feindesliste[i].energy += Druidmap[Feindesliste[i].type].lose_health;
    }
}

/*@Function============================================================
@Desc: InitEnenmys(): initialisiert Feindesliste vollstaendig

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void
InitEnemys (void)
{
  int i;
  int type;

  DebugPrintf ("\nvoid InitEnemys(void): real function call confirmed....:");

  for (i = 0; i < NumEnemys; i++)
    {
      type = Feindesliste[i].type;
      Feindesliste[i].energy = Druidmap[type].maxenergy;
    }

  /* und gut umruehren */
  ShuffleEnemys ();

  DebugPrintf ("\nvoid InitEnemys(void): end of function reached.");
}				/* InitEnemys */

/*@Function============================================================
@Desc: ClearEnemys(): setzt Feindesliste - Array auf 000...

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void
ClearEnemys (void)
{
  int i;

  DebugPrintf ("\nvoid ClearEnemys(void): real function call confirmed...:");

  for (i = 0; i < MAX_ENEMYS_ON_SHIP; i++)
    {
      Feindesliste[i].type = -1;
      Feindesliste[i].levelnum = Feindesliste[i].energy = 0;
      Feindesliste[i].feindphase = 0;
      Feindesliste[i].nextwaypoint = Feindesliste[i].lastwaypoint = 0;
      Feindesliste[i].Status = OUT;
      Feindesliste[i].warten = Feindesliste[i].firewait = 0;
    }

  DebugPrintf ("\nvoid ClearEnemys(void): end of function reached...:");

}				// void ClearEnemys(void)


/*@Function============================================================
@Desc: ShuffleEnemys(): Vermischt enemys in CurLevel auf die Waypoints
		
@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void
ShuffleEnemys (void)
{
  int curlevel = CurLevel->levelnum;
  int i;
  int nth_enemy;
  int wp_num;
  int wp;
  finepoint influ_coord;

  /* Anzahl der Waypoints auf CurLevel abzaehlen */
  wp_num = 0;
  while (CurLevel->AllWaypoints[wp_num++].x != 0);
  wp_num--;			/* einer zuviel */

  nth_enemy = 0;
  for (i = 0; i < NumEnemys; i++)
    {
      if (Feindesliste[i].Status == OUT
	  || Feindesliste[i].levelnum != curlevel)
	continue;		/* dont handle dead enemys or on other level */

      nth_enemy++;
      if (nth_enemy < wp_num)
	wp = nth_enemy;
      else
	{
	  gotoxy (1, 1);
	  DebugPrintf ("\nWeniger waypoints als \n Gegner hier !!");
	  getchar ();
	  return;
	}

      Feindesliste[i].pos.x =
	(CurLevel->AllWaypoints[wp].x) * BLOCKBREITE + BLOCKBREITE / 2;

      Feindesliste[i].pos.y =
	(CurLevel->AllWaypoints[wp].y) * BLOCKHOEHE + BLOCKHOEHE / 2;

      Feindesliste[i].lastwaypoint = wp;
      Feindesliste[i].nextwaypoint = wp;

    }				/* for */

  /* enemys ein bisschen sich selbst ueberlassen */

  /* Influencer zuerst entfernen */
  influ_coord.x = Me.pos.x;
  influ_coord.y = Me.pos.y;
  Me.pos.x = Me.pos.y = 0;

  for (i = 0; i < 30; i++)
    MoveEnemys ();

  /* influencer wieder her */
  Me.pos.x = influ_coord.x;
  Me.pos.y = influ_coord.y;

}				/* ShuffleEnemys() */

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
MoveEnemys (void)
{
  int i;
  point Restweg;
  Waypoint WpList;		/* Pointer to waypoint-liste */
  int nextwp;
  finepoint nextwp_pos;
  int trywp;
  int PossibleConnections;

  if (BeamLine)
    return;

  PermanentHealRobots ();

  for (i = 0; i < NumEnemys; i++)
    {

      if (Feindesliste[i].nextwaypoint == 100)
	continue;

      /* ignore robots on other levels */
      if (Feindesliste[i].levelnum != CurLevel->levelnum)
	continue;

      /* Wenn der robot tot ist dann weiter */
      if (Feindesliste[i].Status == OUT)
	continue;

      /* ist der Robot gerade toedlich getroffen worden ?? */
      if (Feindesliste[i].energy <= 0)
	{
	  Feindesliste[i].Status = OUT;
	  RealScore += Druidmap[Feindesliste[i].type].score;
	  StartBlast (Feindesliste[i].pos.x, Feindesliste[i].pos.y,
		      DRUIDBLAST);
	  if (LevelEmpty ())
	    CurLevel->empty = WAIT_LEVELEMPTY;
	  continue;		/* naechster Enemy, der ist hin */
	}

      if (Druidmap[Feindesliste[i].type].aggression)
	AttackInfluence (i);

      /* Wenn der Robot noch zu warten hat dann gleich weiter */
      if (Feindesliste[i].warten > 0)
	continue;

      /* collision mit anderem Druid */
      EnemyEnemyCollision (i);

      /* Ermittlung des Restweges zum naechsten Ziel */
      WpList = CurLevel->AllWaypoints;
      nextwp = Feindesliste[i].nextwaypoint;
      nextwp_pos.x = WpList[nextwp].x * BLOCKBREITE + BLOCKBREITE / 2;
      nextwp_pos.y = WpList[nextwp].y * BLOCKHOEHE + BLOCKHOEHE / 2;

      Restweg.x = nextwp_pos.x - Feindesliste[i].pos.x;
      Restweg.y = nextwp_pos.y - Feindesliste[i].pos.y;


      /* Bewegung wenn der Abstand noch groesser als maxspeed ist */
      if ((fabsf (Restweg.x) >=
	   Druidmap[Feindesliste[i].type].maxspeed * Frame_Time ())
	  && (Restweg.x != 0))
	{
	  Feindesliste[i].speed.x =
	    (Restweg.x / abs (Restweg.x)) *
	    Druidmap[Feindesliste[i].type].maxspeed;
	  Feindesliste[i].pos.x += Feindesliste[i].speed.x * Frame_Time ();
	}

      if ((fabsf (Restweg.y) >=
	   Druidmap[Feindesliste[i].type].maxspeed * Frame_Time ())
	  && (Restweg.y != 0))
	{
	  Feindesliste[i].speed.y =
	    (Restweg.y / fabsf (Restweg.y)) *
	    Druidmap[Feindesliste[i].type].maxspeed;
	  Feindesliste[i].pos.y += Feindesliste[i].speed.y * Frame_Time ();
	}

      /* Endannaeherung aktuellen waypoint und anvisieren des naechsten */
      DebugPrintf
	("/* Endannaeherung aktuellen waypoint und anvisieren des naechsten */");
      if (abs (Restweg.x) <
	  Druidmap[Feindesliste[i].type].maxspeed * Frame_Time ())
	{
	  Feindesliste[i].pos.x = nextwp_pos.x;
	  Feindesliste[i].speed.x = 0;
	}

      if (abs (Restweg.y) <
	  Druidmap[Feindesliste[i].type].maxspeed * Frame_Time ())
	{
	  Feindesliste[i].pos.y = nextwp_pos.y;
	  Feindesliste[i].speed.y = 0;
	}

      if ((Restweg.x == 0) && (Restweg.y == 0))
	{
	  Feindesliste[i].lastwaypoint = Feindesliste[i].nextwaypoint;
	  Feindesliste[i].warten = MyRandom (ENEMYMAXWAIT);

	  /* suche moegliche Verbindung von hier */
	  DebugPrintf ("/* suche moegliche Verbindung von hier */");
	  PossibleConnections = -1;
	  while ((PossibleConnections < MAX_WP_CONNECTIONS) &&
		 (WpList[nextwp].connections[++PossibleConnections] != -1));

	  if (PossibleConnections > 0)
	    {
	      do
		{
		  trywp =
		    (WpList[nextwp]).
		    connections[MyRandom (PossibleConnections)];
		}
	      while (trywp == -1);

	      /* setze neuen Waypoint */
	      Feindesliste[i].nextwaypoint = trywp;
	    }			/* if */
	}			/* if */

    }				/* for */

}				/* MoveEnemys */

/*@Function============================================================
@Desc: AttackInfluence(): enemynum schiesst unter gegebenen Umstaenden auf
								Influencer

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void
AttackInfluence (int enemynum)
{
  int j;
  int guntype;
  int xdist, ydist;
  long dist2;

  /* Ermittlung des Abstandsvektors zum Influencer */
  xdist = Me.pos.x - Feindesliste[enemynum].pos.x;
  ydist = Me.pos.y - Feindesliste[enemynum].pos.y;

  /* Sicherheit gegen Division durch 0 */
  if (abs (xdist) < 2)
    xdist = 2;
  if (abs (ydist) < 2)
    ydist = 2;

  /* wenn die Vorzeichen gut sind einen Schuss auf den 001 abgeben */
  guntype = Druidmap[Feindesliste[enemynum].type].gun;

  dist2 = (xdist * xdist + ydist * ydist);

  /* Only fire, if the influencer is in range.... */
  if ((dist2 < (long) FIREDIST2) &&
      (!Feindesliste[enemynum].firewait) &&
      IsVisible (&Feindesliste[enemynum].pos))
    {
      if ( MyRandom (AGGRESSIONMAX)  >=
 	  Druidmap[Feindesliste[enemynum].type].aggression )
	{
	  /* Diesmal nicht schiessen */
	  Feindesliste[enemynum].firewait =
	    MyRandom (Druidmap[Feindesliste[enemynum].type].firewait);
	  return;
	}

      Fire_Bullet_Sound ( guntype );

      /* Einen bulleteintragg suchen, der noch nicht belegt ist */
      for (j = 0; j < MAXBULLETS; j++)
	{
	  if (AllBullets[j].type == OUT)
	    break;
	}
      if (j == MAXBULLETS)
	{
	  DebugPrintf
	    ("\nvoid AttackInfluencer(void):  Ran out of Bullets.... Terminating....");
	  Terminate (ERR);
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
	  AllBullets[j].speed.y = Bulletmap[guntype].speed;
	  AllBullets[j].speed.x = xdist * AllBullets[j].speed.y / ydist;
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

      /* Bullets im Zentrum des Schuetzen starten */
      AllBullets[j].pos.x = Feindesliste[enemynum].pos.x;
      AllBullets[j].pos.y = Feindesliste[enemynum].pos.y;

      /* Bullets so abfeuern, dass sie nicht den Schuetzen treffen */
      // AllBullets[j].pos.x+=isignf(AllBullets[j].speed.x)*BLOCKBREITE/2;
      // AllBullets[j].pos.y+=isignf(AllBullets[j].speed.y)*BLOCKHOEHE/2;
      AllBullets[j].pos.x +=
	(AllBullets[j].speed.x) / abs (Bulletmap[guntype].speed) *
	BLOCKBREITE / 2;
      AllBullets[j].pos.y +=
	(AllBullets[j].speed.y) / abs (Bulletmap[guntype].speed) *
	BLOCKHOEHE / 2;

      // The following lines could be improved: Use not the sign, but only the fraction of the maxspeed times constant!
      // SINCE WE CAN ASSUME HIGH FRAMERATE DISABLE THIS CRAP! Within one frame, the robot cant move into its own bullet.
      // AllBullets[j].pos.x+=isignf(Feindesliste[enemynum].speed.x)*BLOCKBREITE/2;      
      // AllBullets[j].pos.y+=isignf(Feindesliste[enemynum].speed.y)*BLOCKHOEHE/2;

      /* Dem Bullettype entsprechend lange warten vor naechstem Schuss */

      Feindesliste[enemynum].firewait =
	MyRandom ( Druidmap[Feindesliste[enemynum].type].firewait ) ;

      /* Bullettype gemaes dem ueblichen guntype fuer den robottyp setzen */
      AllBullets[j].type = guntype;

    }	/* if */

}   /* AttackInfluence */

/*@Function============================================================
@Desc: EnemyEnemyCollision()

@Ret: 	TRUE: Enemynum kollidierte mit anderem Enemy
@Int:
* $Function----------------------------------------------------------*/
int
EnemyEnemyCollision (int enemynum)
{
  int i;
  int curlev = CurLevel->levelnum;
  int check_x, check_y;
  int swap;
  long xdist, ydist;
  long dist2;
  long crit_dist2 = 4 * DRUIDRADIUSX * DRUIDRADIUSX;
  int speed_x, speed_y;

  check_x = Feindesliste[enemynum].pos.x;
  check_y = Feindesliste[enemynum].pos.y;

  for (i = 0; i < NumEnemys; i++)
    {
      /* only living enemys on this level */
      if (Feindesliste[i].Status == OUT || Feindesliste[i].levelnum != curlev)
	continue;
      /* dont check yourself */
      if (i == enemynum)
	continue;

      /* get distance between enemy i and enemynum */
      xdist = check_x - Feindesliste[i].pos.x;
      ydist = check_y - Feindesliste[i].pos.y;

      dist2 = xdist * xdist + ydist * ydist;

      /* Kollision ?? */
      if (dist2 <= crit_dist2)
	{

	  /* Warte ich ??: */
	  if (Feindesliste[enemynum].warten)
	    {
	      /* weiter warten */
	      Feindesliste[enemynum].warten = WAIT_COLLISION;
	      continue;
	    }

	  /* Sonst: Feind stoppen und selbst umdrehen */
	  Feindesliste[i].warten = WAIT_COLLISION;

	  /* gestoppten gegner ein wenig zurueckstossen */
	  if (xdist)
	    Feindesliste[i].pos.x -= xdist / abs ((int) xdist);
	  if (ydist)
	    Feindesliste[i].pos.y -= ydist / abs ((int) ydist);

	  swap = Feindesliste[enemynum].nextwaypoint;
	  Feindesliste[enemynum].nextwaypoint =
	    Feindesliste[enemynum].lastwaypoint;
	  Feindesliste[enemynum].lastwaypoint = swap;

	  /* Etwas aus Gegner herausbewegen !! */
	  speed_x = Feindesliste[enemynum].speed.x;
	  speed_y = Feindesliste[enemynum].speed.y;

	  if (speed_x)
	    Feindesliste[enemynum].pos.x -=
	      COL_SPEED * (speed_x) / abs (speed_x);
	  if (speed_y)
	    Feindesliste[enemynum].pos.y -=
	      COL_SPEED * (speed_y) / abs (speed_y);

	  return TRUE;

	}			/* if dist zu klein */

    }				/* for */

  return FALSE;
}


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
AnimateEnemys (void)
{
  int i;

  for (i = 0; i < NumEnemys; i++)
    {
      if (Feindesliste[i].type == DRUID598)
	{
	  //   printf(" \n Feindrehcode : %d \n maxenergy: %d \n nowenergy: %d \n phase: %d ! ",
	  //   Feindesliste[i].feindrehcode,
	  //   Druidmap[Feindesliste[i].type].maxenergy,
	  //   Feindesliste[i].energy,
	  //  Feindesliste[i].feindphase);
	}

      /* ignore enemys that are dead or on other levels or dummys */
      if (Feindesliste[i].type == DEBUG_ENEMY)
	continue;
      if (Feindesliste[i].levelnum != CurLevel->levelnum)
	continue;
      if (Feindesliste[i].Status == OUT)
	continue;

      // Feindesliste[i].feindrehcode+=Feindesliste[i].energy;
      Feindesliste[i].feindphase +=
	(Feindesliste[i].energy / Druidmap[Feindesliste[i].type].maxenergy) *
	Frame_Time () * ENEMYPHASES * 2.5;

      if (Feindesliste[i].feindphase >= ENEMYPHASES)
	{
#ifdef ENEMYPHASEDEBUG
	  if (Feindesliste[i].type == DRUID598)
	    {
	      printf (" Broke at: %d ", Feindesliste[i].feindphase);
	      getchar ();
	    }
#endif
	  Feindesliste[i].feindphase = 0;
	}
    }
}				// void AnimateEnemys(void)

/*@Function============================================================
@Desc: ClassOfDruid(druidtype): liefert die Classe des Druidtypes type

@Ret: int class: 0-9
@Int:
* $Function----------------------------------------------------------*/
int
ClassOfDruid (int druidtype)
{
  char classnumber[2];

  classnumber[0] = Druidmap[druidtype].druidname[0];	/* first digit is class */
  classnumber[1] = '\0';

  return (atoi (classnumber));
}				/* ClassOfDruid */



#undef _enemy_c
