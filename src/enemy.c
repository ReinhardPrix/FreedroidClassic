/*----------------------------------------------------------------------
 *
 * Desc: All enemy - realted functions.  This includes their whole behaviour,
 *	healing, initialization, shuffling them around after evevator-transitions
 *	of the paradroid, deleting them, collisions of enemys among themselves,
 *	their fireing, animation and such.
 *
 *----------------------------------------------------------------------*/

/* 
 *
 *   Copyright (c) 1994, 2002 Johannes Prix
 *   Copyright (c) 1994, 2002 Reinhard Prix
 *
 *
 *  This file is part of Freedroid
 *
 *  Freedroid is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Freedroid is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Freedroid; see the file COPYING. If not, write to the 
 *  Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
 *  MA  02111-1307  USA
 *
 */
#define _enemy_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"



#define NOSTRAIGHTDIR 255
#undef ENEMYPHASEDEBUG

#define COL_SPEED		3	/* wegstossen bei enemy-enemy collision */

#define FIREDIST2	8 // according to the intro, the laser can be "focused on any target
                          // within a range of eight metres"

// void PermanentHealRobots (void);

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
PermanentHealRobots (void)
{
  int i;

  for (i = 0; i < MAX_ENEMYS_ON_SHIP; i++)
    {
      if (Feindesliste[i].Status == OUT)
	continue;
      if (Feindesliste[i].energy < Druidmap[Feindesliste[i].type].maxenergy)
	Feindesliste[i].energy += Druidmap[Feindesliste[i].type].lose_health * Frame_Time();
    }
} // void PermanentHealRobots(void)

/*-----------------------------------------------------------------
 * @Desc: initialisiert Feindesliste vollstaendig
 * 
 *
 *-----------------------------------------------------------------*/
void
InitEnemys (void)
{
  int i;
  int type;

  for (i = 0; i < NumEnemys; i++)
    {
      type = Feindesliste[i].type;
      Feindesliste[i].energy = Druidmap[type].maxenergy;
    }

  /* und gut umruehren */
  ShuffleEnemys ();

  return;

} /* InitEnemys */

/*-----------------------------------------------------------------
 * @Desc: setzt Feindesliste - Array auf 0
 *
 *
 *-----------------------------------------------------------------*/
void
ClearEnemys (void)
{
  int i;

  for (i = 0; i < MAX_ENEMYS_ON_SHIP; i++)
    {
      Feindesliste[i].type = -1;
      Feindesliste[i].levelnum = Feindesliste[i].energy = 0;
      Feindesliste[i].feindphase = 0;
      Feindesliste[i].nextwaypoint = Feindesliste[i].lastwaypoint = 0;
      Feindesliste[i].Status = OUT;
      Feindesliste[i].warten = Feindesliste[i].firewait = 0;
    }

  return;

} /*  ClearEnemys() */


/*-----------------------------------------------------------------
 * @Desc: Vermischt enemys in CurLevel auf die Waypoints
 *
 *
 *-----------------------------------------------------------------*/
void
ShuffleEnemys (void)
{
  int curlevel = CurLevel->levelnum;
  int i;
  int nth_enemy;
  int wp_num;
  int wp = 0;
  finepoint influ_coord;

  /* Anzahl der Waypoints auf CurLevel abzaehlen */
  wp_num = 0;
  while (CurLevel->AllWaypoints[wp_num].x != 0)
    wp_num ++;

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
	  DebugPrintf ("\nWeniger waypoints als Gegner auf Level ?? !");
	  Terminate (-1);
	}

      // NORMALISATION Feindesliste[i].pos.x = Grob2Fein (CurLevel->AllWaypoints[wp].x);
      // NORMALISATION Feindesliste[i].pos.y = Grob2Fein (CurLevel->AllWaypoints[wp].y);
      Feindesliste[i].pos.x = CurLevel->AllWaypoints[wp].x;
      Feindesliste[i].pos.y = CurLevel->AllWaypoints[wp].y;

      Feindesliste[i].lastwaypoint = wp;
      Feindesliste[i].nextwaypoint = wp;

    }/* for(NumEnemys) */

  /* enemys ein bisschen sich selbst ueberlassen */

  /* Influencer zuerst entfernen */
  influ_coord.x = Me.pos.x;
  influ_coord.y = Me.pos.y;
  Me.pos.x = Me.pos.y = 0;

  for (i = 0; i < 30; i++)     MoveEnemys ();

  /* influencer wieder her */
  Me.pos.x = influ_coord.x;
  Me.pos.y = influ_coord.y;

}	/* ShuffleEnemys() */

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
MoveEnemys (void)
{
  int i,j;
  finepoint Restweg;
  Waypoint WpList;		/* Pointer to waypoint-liste */
  int nextwp;
  finepoint nextwp_pos;
  int trywp;

  PermanentHealRobots ();

  AnimateEnemys ();	// move the "phase" of the rotation of enemys

  for (i = 0; i < NumEnemys; i++)
     {

       /* 
	* what the heck is this ?? (rp) 
	*/
       if (Feindesliste[i].nextwaypoint == 100)
	 continue;

       /* ignore robots on other levels */
       if (Feindesliste[i].levelnum != CurLevel->levelnum)
	 continue;

       // ignore dead robots as well...
       if (Feindesliste[i].Status == OUT)
	 continue;

       // if the robot just got killed, initiate the
       // explosion and all that...
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

       // If its a combat droid, then if might attack...
       if (Druidmap[Feindesliste[i].type].aggression)
	 AttackInfluence (i);

       // robots that still have to wait also do not need to
       // be processed...
       if (Feindesliste[i].warten > 0)
	 continue;

       // Now check for collisions of this enemy with his colleagues
       CheckEnemyEnemyCollision (i);

       /* Ermittlung des Restweges zum naechsten Ziel */
       WpList = CurLevel->AllWaypoints;
       nextwp = Feindesliste[i].nextwaypoint;
       // NORMALISATION nextwp_pos.x = Grob2Fein (WpList[nextwp].x);
       // NORMALISATION nextwp_pos.y = Grob2Fein (WpList[nextwp].y);
       nextwp_pos.x = WpList[nextwp].x;
       nextwp_pos.y = WpList[nextwp].y;

       Restweg.x = nextwp_pos.x - Feindesliste[i].pos.x;
       Restweg.y = nextwp_pos.y - Feindesliste[i].pos.y;

       // printf("\n Restweg.x: %g Restweg.y: %g ", Restweg.x, Restweg.y );
       // printf("\n NextWP.x: %g NextWP.y: %g ", nextwp_pos.x , nextwp_pos.y );


       // --------------------
       // As long a the distance from the current position of the enemy
       // to its next wp is large, movement is rather sinple:

       if (( fabsf (Restweg.x * Block_Width) >= 1) )
	 {
	   Feindesliste[i].speed.x =
	     (Restweg.x / fabsf (Restweg.x)) *
	     Druidmap[Feindesliste[i].type].maxspeed;
	   Feindesliste[i].pos.x += Feindesliste[i].speed.x * Frame_Time ();
	 }

       if (( fabsf (Restweg.y * Block_Height ) >= 1) )
	 {
	   Feindesliste[i].speed.y =
	     (Restweg.y / fabsf (Restweg.y)) *
	     Druidmap[Feindesliste[i].type].maxspeed;
	   Feindesliste[i].pos.y += Feindesliste[i].speed.y * Frame_Time ();
	 }

       // --------------------
       // Once this enemy is close to his final destination waypoint, we have
       // to do some fine tuning, and then of course set the next waypoint.

       if ( fabsf(Restweg.x * Block_Width ) < 1 )
	 {
	   Feindesliste[i].pos.x = nextwp_pos.x;
	   Feindesliste[i].speed.x = 0;
	   // printf("\n Final Destination in x reached.");
	 }

       if ( fabsf(Restweg.y * Block_Height ) < 1 )
	 {
	   Feindesliste[i].pos.y = nextwp_pos.y;
	   Feindesliste[i].speed.y = 0;
	   // printf("\n Final Destination in y reached.");
	 }


       if ((Restweg.x == 0) && (Restweg.y == 0))
	 {
	   Feindesliste[i].lastwaypoint = Feindesliste[i].nextwaypoint;
	   Feindesliste[i].warten = MyRandom (ENEMYMAXWAIT);

	   /* suche moegliche Verbindung von hier */
	   DebugPrintf ("/* suche moegliche Verbindung von hier */\n");
	   /* but only if there are connections possible */
	   for ( j=0; j<MAX_WP_CONNECTIONS; j++ )
	     if ( WpList[nextwp].connections[j] != -1 )
	       break;
	   if ( j < MAX_WP_CONNECTIONS )
	     while ( (trywp = WpList[nextwp].
		      connections[MyRandom (MAX_WP_CONNECTIONS - 1)]) == -1);
	   else
	     {
	       printf ("\nWeird waypoint %d has no connections!\n", nextwp);
	       Terminate(ERR);
	     }

	  /* setze neuen Waypoint */
	  Feindesliste[i].nextwaypoint = trywp;
	}			/* if */
    }	/* for (NumEnemeys) */

} /* MoveEnemys() */

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
  float xdist, ydist;
  float dist2;

  /* Ermittlung des Abstandsvektors zum Influencer */
  xdist = Me.pos.x - Feindesliste[enemynum].pos.x;
  ydist = Me.pos.y - Feindesliste[enemynum].pos.y;

  if (xdist == 0) xdist = 0.01;
  if (ydist == 0) ydist = 0.01;

  /* Sicherheit gegen Division durch 0 */
  //  if (fabsf (xdist) < 2)
  //    xdist = 2;
  //  if (abs (ydist) < 2)
  //    ydist = 2;

  /* wenn die Vorzeichen gut sind einen Schuss auf den 001 abgeben */
  guntype = Druidmap[Feindesliste[enemynum].type].gun;

  dist2 = sqrt(xdist * xdist + ydist * ydist);

  /* Only fire, if the influencer is in range.... */
  if ((dist2 < FIREDIST2) &&
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
      if (fabsf (xdist) > fabsf (ydist))
	{
	  AllBullets[j].speed.x = Bulletmap[guntype].speed;
	  AllBullets[j].speed.y = ydist * AllBullets[j].speed.x / xdist;
	  if (xdist < 0)
	    {
	      AllBullets[j].speed.x = -AllBullets[j].speed.x;
	      AllBullets[j].speed.y = -AllBullets[j].speed.y;
	    }
	}

      if (fabsf (xdist) < fabsf (ydist))
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
      if ( fabsf (xdist) / fabsf (ydist) > (3/2.0) )
	AllBullets[j].phase = RECHTS;
      if ( fabsf (ydist) / fabsf (xdist) > (3/2.0) )
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
      AllBullets[j].pos.x +=
	(AllBullets[j].speed.x) / fabsf (Bulletmap[guntype].speed) * 0.5;
      AllBullets[j].pos.y +=
	(AllBullets[j].speed.y) / fabsf (Bulletmap[guntype].speed) * 0.5;

      // The following lines could be improved: Use not the sign, but only */
      // the fraction of the maxspeed times constant!
      // SINCE WE CAN ASSUME HIGH FRAMERATE DISABLE THIS CRAP! Within one */
      // frame, the robot cant move into its own bullet.
      // AllBullets[j].pos.x+=isignf(Feindesliste[enemynum].speed.x)*Block_Width/2;      
      // AllBullets[j].pos.y+=isignf(Feindesliste[enemynum].speed.y)*Block_Height/2;

      /* Dem Bullettype entsprechend lange warten vor naechstem Schuss */

      Feindesliste[enemynum].firewait =
	MyRandom ( Druidmap[Feindesliste[enemynum].type].firewait ) ;

      /* Bullettype gemaes dem ueblichen guntype fuer den robottyp setzen */
      AllBullets[j].type = guntype;

    }	/* if */

}   /* AttackInfluence */

/*@Function============================================================
@Desc: CheckEnemyEnemyCollision()

@Ret: 	TRUE: if enemy Nr. Enemynum collided with another enemy
@Int:
* $Function----------------------------------------------------------*/
int
CheckEnemyEnemyCollision (int enemynum)
{
  int i;
  int curlev = CurLevel->levelnum;
  float check_x, check_y;
  int swap;
  float xdist, ydist;
  float dist2;
  float speed_x, speed_y;

  check_x = Feindesliste[enemynum].pos.x;
  check_y = Feindesliste[enemynum].pos.y;

  for (i = 0; i < NumEnemys; i++)
    {
      // check only collisions of LIVING enemys on this level
      if (Feindesliste[i].Status == OUT || Feindesliste[i].levelnum != curlev)
	continue;
      // dont check yourself...
      if (i == enemynum)
	continue;

      /* get distance between enemy i and enemynum */
      xdist = check_x - Feindesliste[i].pos.x;
      ydist = check_y - Feindesliste[i].pos.y;

      dist2 = sqrt(xdist * xdist + ydist * ydist);

      // Is there a Collision?
      if ( dist2 <= 2*DRUIDRADIUSXY )
	{

	  // am I waiting already?  If so, keep waiting... 
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
	    Feindesliste[i].pos.x -= xdist / fabsf (xdist) * Frame_Time();
	  if (ydist)
	    Feindesliste[i].pos.y -= ydist / fabsf (ydist) * Frame_Time();

	  swap = Feindesliste[enemynum].nextwaypoint;
	  Feindesliste[enemynum].nextwaypoint =
	    Feindesliste[enemynum].lastwaypoint;
	  Feindesliste[enemynum].lastwaypoint = swap;

	  /* Etwas aus Gegner herausbewegen !! */
	  speed_x = Feindesliste[enemynum].speed.x;
	  speed_y = Feindesliste[enemynum].speed.y;

	  if (speed_x)
	    Feindesliste[enemynum].pos.x -=
	      Frame_Time() * COL_SPEED * (speed_x) / fabsf (speed_x);
	  if (speed_y)
	    Feindesliste[enemynum].pos.y -=
	      Frame_Time() * COL_SPEED * (speed_y) / fabsf (speed_y);

	  return TRUE;

	}			/* if dist zu klein */

    }				/* for */

  return FALSE;
} // int CheckEnemyEnemyCollision


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
