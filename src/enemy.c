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
      if (AllEnemys[i].Status == OUT)
	continue;
      if (AllEnemys[i].energy < Druidmap[AllEnemys[i].type].maxenergy)
	AllEnemys[i].energy += Druidmap[AllEnemys[i].type].lose_health * Frame_Time();
    }
} // void PermanentHealRobots(void)

/*-----------------------------------------------------------------
 * @Desc: initialisiert the energy of all enemys
 * 
 *
 *-----------------------------------------------------------------*/
void
InitEnemys (void)
{
  int i;
  int type;

  // printf("\nNumEnemys ist jetzt: %d " ,NumEnemys );
  // fflush(stdout);
  for (i = 0; i < MAX_ENEMYS_ON_SHIP; i++)
    {
      type = AllEnemys[i].type;
      if (type == OUT) continue;
      AllEnemys[i].energy = Druidmap[type].maxenergy;
      AllEnemys[i].Status = !OUT;
    }

} /* InitEnemys */

/*-----------------------------------------------------------------
 * @Desc: setzt AllEnemys - Array auf 0
 *
 *
 *-----------------------------------------------------------------*/
void
ClearEnemys (void)
{
  int i;

  for (i = 0; i < MAX_ENEMYS_ON_SHIP; i++)
    {
      AllEnemys[i].type = -1;
      AllEnemys[i].levelnum = AllEnemys[i].energy = 0;
      AllEnemys[i].feindphase = 0;
      AllEnemys[i].nextwaypoint = AllEnemys[i].lastwaypoint = 0;
      AllEnemys[i].Status = OUT;
      AllEnemys[i].warten = 0;
      AllEnemys[i].firewait = 0;
      AllEnemys[i].energy = 0;
      AllEnemys[i].SpecialForce = 0;
      AllEnemys[i].CompletelyFixed = 0;
      AllEnemys[i].Parameter1 = 0;
      AllEnemys[i].Parameter2 = 0;
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
  // WARNING!! IT SHOULD BE NOTED THAT THIS FUNCTION REQUIRES THE
  // CURLEVEL STRUCTURE TO BE SET ALREADY, OR IT WILL SEGFAULT,
  // EVEN WHEN A RETURN IS PLACED AT THE START OF THE FUNCTION!!!!
  //
  int curlevel = CurLevel->levelnum;
  int i;
  int nth_enemy;
  int wp_num;
  int wp = 0;
  finepoint influ_coord;

  /* Anzahl der Waypoints auf CurLevel abzaehlen */
  wp_num = 0;

  for ( i=0 ; i<MAXWAYPOINTS ; i++ )
    {
      if ( CurLevel->AllWaypoints[wp_num].x != 0 ) wp_num ++;
    }

  nth_enemy = 0;
  for (i = 0; i < MAX_ENEMYS_ON_SHIP ; i++)
    {
      if (AllEnemys[i].Status == OUT
	  || AllEnemys[i].levelnum != curlevel)
	continue;		/* dont handle dead enemys or on other level */

      if (AllEnemys[i].SpecialForce) continue;


      nth_enemy++;
      if (nth_enemy < wp_num)
	wp = nth_enemy;
      else
	{
	  
	  printf ("\nLess waypoints than enemys on level %d? !", CurLevel->levelnum );
	  Terminate (ERR);
	}

      AllEnemys[i].pos.x = CurLevel->AllWaypoints[wp].x;
      AllEnemys[i].pos.y = CurLevel->AllWaypoints[wp].y;

      AllEnemys[i].lastwaypoint = wp;
      AllEnemys[i].nextwaypoint = wp;

    }/* for (MAX_ENEMYS_ON_SHIP) */

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
  float maxspeed;

  PermanentHealRobots ();

  AnimateEnemys ();	// move the "phase" of the rotation of enemys

  for (i = 0; i < MAX_ENEMYS_ON_SHIP ; i++)
     {
       maxspeed = Druidmap[Me.type].maxspeed;

       /* 
	* what the heck is this ?? (rp) 
	*/
       if (AllEnemys[i].nextwaypoint == 100)
	 continue;

       /* ignore robots on other levels */
       if (AllEnemys[i].levelnum != CurLevel->levelnum)
	 continue;

       // ignore dead robots as well...
       if (AllEnemys[i].Status == OUT)
	 continue;

       // if the robot just got killed, initiate the
       // explosion and all that...
       if (AllEnemys[i].energy <= 0)
	 {
	   AllEnemys[i].Status = OUT;
	   RealScore += Druidmap[AllEnemys[i].type].score;
	   StartBlast (AllEnemys[i].pos.x, AllEnemys[i].pos.y,
		       DRUIDBLAST);
	   if (LevelEmpty ())
	     CurLevel->empty = WAIT_LEVELEMPTY;
	   continue;		/* naechster Enemy, der ist hin */
	 }

       // If its a combat droid, then if might attack...
       if (Druidmap[AllEnemys[i].type].aggression)
	 AttackInfluence (i);

       // ignore all enemys with CompletelyFixed flag set...
       if ( AllEnemys[i].CompletelyFixed ) continue;

       // robots that still have to wait also do not need to
       // be processed...
       if (AllEnemys[i].warten > 0)
	 continue;

       // Now check for collisions of this enemy with his colleagues
       CheckEnemyEnemyCollision (i);

       /* Ermittlung des Restweges zum naechsten Ziel */
       WpList = CurLevel->AllWaypoints;
       nextwp = AllEnemys[i].nextwaypoint;
       // NORMALISATION nextwp_pos.x = Grob2Fein (WpList[nextwp].x);
       // NORMALISATION nextwp_pos.y = Grob2Fein (WpList[nextwp].y);
       nextwp_pos.x = WpList[nextwp].x;
       nextwp_pos.y = WpList[nextwp].y;

       Restweg.x = nextwp_pos.x - AllEnemys[i].pos.x;
       Restweg.y = nextwp_pos.y - AllEnemys[i].pos.y;

       // printf("\n Restweg.x: %g Restweg.y: %g ", Restweg.x, Restweg.y );
       // printf("\n NextWP.x: %g NextWP.y: %g ", nextwp_pos.x , nextwp_pos.y );


       // --------------------
       // As long a the distance from the current position of the enemy
       // to its next wp is large, movement is rather sinple:

       if ( fabsf (Restweg.x)  > Frame_Time() * maxspeed )
	 {
	   AllEnemys[i].speed.x =
	     (Restweg.x / fabsf (Restweg.x)) * maxspeed;
	   AllEnemys[i].pos.x += AllEnemys[i].speed.x * Frame_Time ();
	 } 	 
       else
	 {
	   // --------------------
	   // Once this enemy is close to his final destination waypoint, we have
	   // to do some fine tuning, and then of course set the next waypoint.
	   AllEnemys[i].pos.x = nextwp_pos.x;
	   AllEnemys[i].speed.x = 0;
	   // printf("\n Final Destination in x reached.");
	 }


       if ( fabsf (Restweg.y)  > Frame_Time() * maxspeed )
	 {
	   AllEnemys[i].speed.y =
	     (Restweg.y / fabsf (Restweg.y)) * maxspeed;
	   AllEnemys[i].pos.y += AllEnemys[i].speed.y * Frame_Time ();
	 }
       else
	 {
	   // AllEnemys[i].pos.y += (nextwp_pos.y-AllEnemys[i].pos.y)*Frame_Time();
	   AllEnemys[i].pos.y = nextwp_pos.y;
	   AllEnemys[i].speed.y = 0;
	   // printf("\n Final Destination in y reached.");
	 }


       //--------------------
       // Now we can see if we are perhaps already there?
       // then it might be time to set a new waypoint.
       //
       if ((Restweg.x == 0) && (Restweg.y == 0))
	 {
	   AllEnemys[i].lastwaypoint = AllEnemys[i].nextwaypoint;
	   AllEnemys[i].warten = MyRandom (ENEMYMAXWAIT);

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
	       // Terminate(ERR);
	     }

	  /* setze neuen Waypoint */
	  AllEnemys[i].nextwaypoint = trywp;
	}			/* if */
    }	/* for (MAX_ENEMYS_ON_SHIP) */

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
  xdist = Me.pos.x - AllEnemys[enemynum].pos.x;
  ydist = Me.pos.y - AllEnemys[enemynum].pos.y;

  if (xdist == 0) xdist = 0.01;
  if (ydist == 0) ydist = 0.01;

  /* Sicherheit gegen Division durch 0 */
  //  if (fabsf (xdist) < 2)
  //    xdist = 2;
  //  if (abs (ydist) < 2)
  //    ydist = 2;

  /* wenn die Vorzeichen gut sind einen Schuss auf den 001 abgeben */
  guntype = Druidmap[AllEnemys[enemynum].type].gun;

  dist2 = sqrt(xdist * xdist + ydist * ydist);

  //--------------------
  // If some special command was given, like 
  // ATTACK_FIXED_MAP_POSITION, then we do the following:
  //
  if ( AllEnemys[enemynum].AdvancedCommand == 1 )
    {
      if (AllEnemys[enemynum].firewait) return;  // can't fire:  gun not yet reloaded...


      xdist = AllEnemys[enemynum].Parameter1 - AllEnemys[enemynum].pos.x;
      ydist = AllEnemys[enemynum].Parameter2 - AllEnemys[enemynum].pos.y;

      Fire_Bullet_Sound ( guntype );

      // find a bullet entry, that isn't currently used... 
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
      AllBullets[j].pos.x = AllEnemys[enemynum].pos.x;
      AllBullets[j].pos.y = AllEnemys[enemynum].pos.y;

      /* Bullets so abfeuern, dass sie nicht den Schuetzen treffen */
      AllBullets[j].pos.x +=
	(AllBullets[j].speed.x) / fabsf (Bulletmap[guntype].speed) * 0.5;
      AllBullets[j].pos.y +=
	(AllBullets[j].speed.y) / fabsf (Bulletmap[guntype].speed) * 0.5;

      // The following lines could be improved: Use not the sign, but only */
      // the fraction of the maxspeed times constant!
      // SINCE WE CAN ASSUME HIGH FRAMERATE DISABLE THIS CRAP! Within one */
      // frame, the robot cant move into its own bullet.
      // AllBullets[j].pos.x+=isignf(AllEnemys[enemynum].speed.x)*Block_Width/2;      
      // AllBullets[j].pos.y+=isignf(AllEnemys[enemynum].speed.y)*Block_Height/2;

      /* Dem Bullettype entsprechend lange warten vor naechstem Schuss */

      AllEnemys[enemynum].firewait = Bulletmap[Druidmap[AllEnemys[enemynum].type].gun].recharging_time ;

      /* Bullettype gemaes dem ueblichen guntype fuer den robottyp setzen */
      AllBullets[j].type = guntype;

      return;
    }



  //--------------------
  // From here on, it's classical Paradroid robot behaviour concerning fireing....
  //

  /* Only fire, if the influencer is in range.... */
  if ((dist2 < FIREDIST2) &&
      (!AllEnemys[enemynum].firewait) &&
      IsVisible (&AllEnemys[enemynum].pos))
    {

      if ( MyRandom (AGGRESSIONMAX)  >=
 	  Druidmap[AllEnemys[enemynum].type].aggression )
	{
	  AllEnemys[enemynum].firewait += drand48()*10; //MyRandom (Druidmap[AllEnemys[enemynum].type].firewait);
	  return;
	}


      Fire_Bullet_Sound ( guntype );

      // find a bullet entry, that isn't currently used... 
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
      AllBullets[j].pos.x = AllEnemys[enemynum].pos.x;
      AllBullets[j].pos.y = AllEnemys[enemynum].pos.y;

      /* Bullets so abfeuern, dass sie nicht den Schuetzen treffen */
      AllBullets[j].pos.x +=
	(AllBullets[j].speed.x) / fabsf (Bulletmap[guntype].speed) * 0.5;
      AllBullets[j].pos.y +=
	(AllBullets[j].speed.y) / fabsf (Bulletmap[guntype].speed) * 0.5;

      // The following lines could be improved: Use not the sign, but only */
      // the fraction of the maxspeed times constant!
      // SINCE WE CAN ASSUME HIGH FRAMERATE DISABLE THIS CRAP! Within one */
      // frame, the robot cant move into its own bullet.
      // AllBullets[j].pos.x+=isignf(AllEnemys[enemynum].speed.x)*Block_Width/2;      
      // AllBullets[j].pos.y+=isignf(AllEnemys[enemynum].speed.y)*Block_Height/2;

      /* Dem Bullettype entsprechend lange warten vor naechstem Schuss */

      AllEnemys[enemynum].firewait = Bulletmap[Druidmap[AllEnemys[enemynum].type].gun].recharging_time ;

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

  check_x = AllEnemys[enemynum].pos.x;
  check_y = AllEnemys[enemynum].pos.y;

  for (i = 0; i < MAX_ENEMYS_ON_SHIP ; i++)
    {
      // check only collisions of LIVING enemys on this level
      if (AllEnemys[i].Status == OUT || AllEnemys[i].levelnum != curlev)
	continue;
      // dont check yourself...
      if (i == enemynum)
	continue;

      /* get distance between enemy i and enemynum */
      xdist = check_x - AllEnemys[i].pos.x;
      ydist = check_y - AllEnemys[i].pos.y;

      dist2 = sqrt(xdist * xdist + ydist * ydist);

      // Is there a Collision?
      if ( dist2 <= 2*DRUIDRADIUSXY )
	{

	  // am I waiting already?  If so, keep waiting... 
	  if (AllEnemys[enemynum].warten)
	    {
	      /* weiter warten */
	      AllEnemys[enemynum].warten = WAIT_COLLISION;
	      continue;
	    }

	  /* Sonst: Feind stoppen und selbst umdrehen */
	  AllEnemys[i].warten = WAIT_COLLISION;

	  /* gestoppten gegner ein wenig zurueckstossen */
	  if (xdist)
	    AllEnemys[i].pos.x -= xdist / fabsf (xdist) * Frame_Time();
	  if (ydist)
	    AllEnemys[i].pos.y -= ydist / fabsf (ydist) * Frame_Time();

	  swap = AllEnemys[enemynum].nextwaypoint;
	  AllEnemys[enemynum].nextwaypoint =
	    AllEnemys[enemynum].lastwaypoint;
	  AllEnemys[enemynum].lastwaypoint = swap;

	  /* Etwas aus Gegner herausbewegen !! */
	  speed_x = AllEnemys[enemynum].speed.x;
	  speed_y = AllEnemys[enemynum].speed.y;

	  if (speed_x)
	    AllEnemys[enemynum].pos.x -=
	      Frame_Time() * COL_SPEED * (speed_x) / fabsf (speed_x);
	  if (speed_y)
	    AllEnemys[enemynum].pos.y -=
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

  for (i = 0; i < MAX_ENEMYS_ON_SHIP ; i++)
    {
      if (AllEnemys[i].type == DRUID598)
	{
	  //   printf(" \n Feindrehcode : %d \n maxenergy: %d \n nowenergy: %d \n phase: %d ! ",
	  //   AllEnemys[i].feindrehcode,
	  //   Druidmap[AllEnemys[i].type].maxenergy,
	  //   AllEnemys[i].energy,
	  //  AllEnemys[i].feindphase);
	}

      /* ignore enemys that are dead or on other levels or dummys */
      if (AllEnemys[i].type == DEBUG_ENEMY)
	continue;
      if (AllEnemys[i].levelnum != CurLevel->levelnum)
	continue;
      if (AllEnemys[i].Status == OUT)
	continue;

      // AllEnemys[i].feindrehcode+=AllEnemys[i].energy;
      AllEnemys[i].feindphase +=
	(AllEnemys[i].energy / Druidmap[AllEnemys[i].type].maxenergy) *
	Frame_Time () * ENEMYPHASES * 2.5;

      if (AllEnemys[i].feindphase >= ENEMYPHASES)
	{
#ifdef ENEMYPHASEDEBUG
	  if (AllEnemys[i].type == DRUID598)
	    {
	      printf (" Broke at: %d ", AllEnemys[i].feindphase);
	      getchar ();
	    }
#endif
	  AllEnemys[i].feindphase = 0;
	}
    }
} // void AnimateEnemys(void)

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
