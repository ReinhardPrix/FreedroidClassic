/*
 *
 *   Copyright (c) 1994, 2002, 2003  Johannes Prix
 *   Copyright (c) 1994, 2002, 2003  Reinhard Prix
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

/*----------------------------------------------------------------------
 *
 * Desc: all Bullet AND Blast - related functions.
 *
 *
 *----------------------------------------------------------------------*/

#define _bullet_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"

/* Distances for hitting a druid */
//NORMALISATION #define MORE		4
#define MORE		(4/64.0)
//NORMALISATION #define DRUIDHITDIST2		(Druid_Radius_X+MORE)*(DRUIDRADIUSY+MORE)
// #define DRUIDHITDIST2		0
// #define DRUIDHITDIST2		(Druid_Radius_X+MORE)*(DRUIDRADIUSY+MORE)
#define DRUIDHITDIST2		(0.3+MORE)*(Droid_Radius+MORE)
// #define DRUIDHITDIST2		0


/*@Function============================================================
@Desc: this function moves all the bullets according to their speeds.

NEW: this function also takes into accoung the current framerate.

@Ret: keiner

@Int: keiner
* $Function----------------------------------------------------------*/

void
MoveBullets (void)
{

  /* lokale Variablen der Funktion: */
  int i;
  Bullet CurBullet;

  /* Bewegung der Bullets */
  for (i = 0; i < MAXBULLETS; i++)
    {
      CurBullet = &AllBullets[i];

      if ( (CurBullet->type == OUT) )
	continue;

      CurBullet->prev_pos.x = CurBullet->pos.x;
      CurBullet->prev_pos.y = CurBullet->pos.y;

      CurBullet->pos.x += CurBullet->speed.x * Frame_Time ();
      CurBullet->pos.y += CurBullet->speed.y * Frame_Time ();

      CurBullet->time_in_frames++;
      CurBullet->time_in_seconds += Frame_Time();
    }				/* for */

  return;
}				// void MoveBullets(void)


/*@Function============================================================
@Desc: delete bullet of given number, set it type=OUT, put it at x/y=-1/-1
       and create a Bullet-blast if with_blast==TRUE
@Ret:
@Int:
* $Function----------------------------------------------------------*/
void
DeleteBullet (int Bulletnumber)
{
  Bullet CurBullet = &AllBullets[Bulletnumber];
  int i;

  if (CurBullet->type == OUT) // ignore dead bullets
    return;

  //--------------------
  // At first we generate the blast at the collision spot of the bullet,
  // cause later, after the bullet is deleted, it will be hard to know
  // the correct location ;)

  // RP (18/11/02): nay, we do that manually before DeleteBullet() now,
  // --> not all bullets should create Blasts (i.e. not if droid was hit)
  //  StartBlast (CurBullet->pos.x, CurBullet->pos.y, BULLETBLAST);

  //--------------------
  // maybe, the bullet had several SDL_Surfaces attached to it.  Then we need to
  // free the SDL_Surfaces again as well...
  //
  // if ( ( CurBullet->type != FLASH) && ( CurBullet->type != OUT ) )
  if ( CurBullet->Surfaces_were_generated )
    {
      DebugPrintf( 1 , "\nvoid DeleteBullet(...): freeing this bullets attached surfaces...");
      for ( i=0 ; i < Bulletmap[ CurBullet->type ].phases ; i++ )
	{
	  SDL_FreeSurface( CurBullet->SurfacePointer[i] );
	  CurBullet->SurfacePointer[i] = NULL;
	}
      CurBullet->Surfaces_were_generated = FALSE;
    }

  //--------------------
  // Now that the memory has been freed again, we can finally delete this bullet entry.
  // Hope, that this does not give us a SEGFAULT, but it should not do so.
  //
  CurBullet->type = OUT;
  CurBullet->time_in_seconds = 0;
  CurBullet->time_in_frames = 0;
  CurBullet->mine = FALSE;
  CurBullet->phase = 0;
  CurBullet->pos.x = -1;
  CurBullet->pos.y = -1;
  CurBullet->angle = 0;

  return;

}; // void DeleteBullet(int Bulletnumber)

/*@Function============================================================
@Desc: StartBlast(): erzeugt einen Blast type an x/y

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void
StartBlast (float x, float y, int type)
{
  int i;
  Blast NewBlast;

  /* Position des naechsten freien Blasts herausfinden */
  for (i = 0; i < MAXBLASTS; i++)
    if (AllBlasts[i].type == OUT)
      break;

  /* keinen gefunden: nimm den ersten */
  if (i >= MAXBLASTS)
    i = 0;

  /* Get Pointer to it: more comfortable */
  NewBlast = &(AllBlasts[i]);


  if (type == REJECTBLAST)
    {
      NewBlast->mine = TRUE;
      type = DRUIDBLAST;    // not really a different type, just avoid damaging influencer
    }
  else
    NewBlast->mine = FALSE;

  /* Einen Blast an x/y erzeugen */
  NewBlast->PX = x;
  NewBlast->PY = y;

  NewBlast->type = type;
  NewBlast->phase = 0;

  NewBlast->MessageWasDone = 0;


  if (type == DRUIDBLAST)
    {
     DruidBlastSound ();
    }

}				/* StartBlast */

/*@Function============================================================
@Desc: Diese Funktion zaehlt die Phasen aller Explosionen weiter
@Ret: keiner
@Int: keiner
* $Function----------------------------------------------------------*/
void
ExplodeBlasts (void)
{
  int i;
  Blast CurBlast = AllBlasts;

  for (i = 0; i < MAXBLASTS; i++, CurBlast++)
    if (CurBlast->type != OUT)
      {

	/* Druidblasts sind gefaehrlich !! */
	if (CurBlast->type == DRUIDBLAST)
	  CheckBlastCollisions (i);

	// CurBlast->phase++;
	CurBlast->phase += Frame_Time () * Blastmap[ CurBlast->type ].phases / Blastmap[ CurBlast->type ].total_animation_time;
	if (((int) floorf (CurBlast->phase)) >=
	    Blastmap[CurBlast->type].phases)
	  DeleteBlast (i);
      }				/* if */
}				/* ExplodeBlasts */

/*@Function============================================================
@Desc: Einen eizelnen Blast ausloeschen

@Ret: keiner
@Int:
* $Function----------------------------------------------------------*/
void
DeleteBlast (int Blastnummer)
{
  AllBlasts[Blastnummer].type = OUT;
}

/*@Function============================================================
@Desc:

@Ret:
@Int:
* $Function----------------------------------------------------------*/
int
GetDirection (point robo, point bul)
{

  if ((robo.x < bul.x) && (robo.y > bul.y))
    return 0;
  if ((robo.x < bul.x) && (robo.y < bul.y))
    return 1;
  if ((robo.x > bul.x) && (robo.y < bul.y))
    return 2;
  if ((robo.x > bul.x) && (robo.y > bul.y))
    return 3;
  if ((robo.x == bul.x) && (robo.y == bul.y))
    {
      DebugPrintf (2, " Center hit directy!");
      getchar ();
    }
  return 0;
}

/*@Function============================================================
@Desc: CheckBulletCollisions(int num)
	checkt Collisions des Bullets Num mit Hintergrund && Druids

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void
CheckBulletCollisions (int num)
{
  int level = CurLevel->levelnum;
  float xdist, ydist;
  Bullet CurBullet = &AllBullets[num];
  static int FBTZaehler = 0;
  finepoint step;
  int num_check_steps, stepnum;
  int i;


  switch (CurBullet->type)
    {
      // --------------------
      // Never do any collision checking if the bullet is OUT already...
    case OUT:
      return;
      break;

      // --------------------
      // Next we handle the case that the bullet is of type FLASH
    case FLASH:
      // if the flash is over, just delete it and return
      if (CurBullet->time_in_seconds >= FLASH_DURATION)
	{
	  CurBullet->time_in_frames = 0;
	  CurBullet->time_in_seconds = 0;
	  CurBullet->type = OUT;
	  CurBullet->mine = FALSE;
	  return;
	}

      // if the flash is not yet over, do some checking for who gets
      // hurt by it.
      // Two different methode for doing this are available:
      // The first but less elegant Method is just to check for
      // flash immunity, for distance and visiblity.
      // The second and more elegant method is to recursively fill
      // out the room where the flash-maker is in and to hurt all
      // robots in there except of course for those immune.
      if ( CurBullet->time_in_frames != 1 )
	break; // we only do the damage once and thats at frame nr. 1 of the flash

      for (i = 0; i < NumEnemys; i++)
	{
	  // !! dont't forget: Only droids on our level are harmed!! (bugfix)
	  if (AllEnemys[i].levelnum != level)
	    continue;

	  if ( IsVisible (&AllEnemys[i].pos) & (!Druidmap[AllEnemys[i].type].flashimmune) )
	    {
	      AllEnemys[i].energy -= Bulletmap[FLASH].damage;
	      // Since the enemy just got hit, it might as well say so :)
	      EnemyHitByBulletText( i );
	    }
	}

      // droids with flash are always flash-immune!
      // -> we don't get hurt by our own flashes!
      if (!InvincibleMode && !Druidmap[Me.type].flashimmune)
	Me.energy -= Bulletmap[FLASH].damage ;

      return;
      break;

      // --------------------
      // If its a "normal" Bullet, several checks have to be
      // done, one for collisions with background,
      // one for collision with influencer
      // some for collisions with enemys
      // and some for collisions with other bullets
      // and some for collisions with blast
      //
    default:

      // first check for collision with background
      step.x = CurBullet->pos.x - CurBullet->prev_pos.x;
      step.y = CurBullet->pos.y - CurBullet->prev_pos.y;
      num_check_steps = (int)( sqrt(step.x*step.x + step.y*step.y)/ COLLISION_STEPSIZE);
      if (num_check_steps == 0) num_check_steps = 1;
      step.x /= 1.0* num_check_steps;
      step.y /= 1.0* num_check_steps;

      CurBullet->pos.x = CurBullet->prev_pos.x;
      CurBullet->pos.y = CurBullet->prev_pos.y;

      for (stepnum=0; stepnum < num_check_steps; stepnum++)
	{
	  CurBullet->pos.x += step.x;
	  CurBullet->pos.y += step.y;

	  if (IsPassable (CurBullet->pos.x, CurBullet->pos.y, CENTER) != CENTER)
	    {
	      StartBlast (CurBullet->pos.x, CurBullet->pos.y, BULLETBLAST);
	      DeleteBullet (num);
	      return;
	    }

	  // check for collision with influencer
	  if (!CurBullet->mine)
	    {
	      xdist = Me.pos.x - CurBullet->pos.x;
	      ydist = Me.pos.y - CurBullet->pos.y;
	      if ((xdist * xdist + ydist * ydist) < DRUIDHITDIST2)  // FIXME: don't use DRUIDHITDIST2!!
		{
		  GotHitSound ();

		  if (!InvincibleMode)
		    Me.energy -= Bulletmap[CurBullet->type].damage;	/* Energie verlieren */

		  DeleteBullet( num );
		  return;			/* Bullet ist hin */
		}
	    } // if Bullet!=mine

	      // check for collision with enemys
	  for (i = 0; i < NumEnemys; i++)
	    {
	      if (AllEnemys[i].status == OUT || AllEnemys[i].status == TERMINATED ||
		  AllEnemys[i].levelnum != level)
		continue;

	      xdist = CurBullet->pos.x - AllEnemys[i].pos.x;
	      ydist = CurBullet->pos.y - AllEnemys[i].pos.y;

	      if ((xdist * xdist + ydist * ydist) < DRUIDHITDIST2) // FIXME
		{
		  // The enemy who was hit, loses some energy, depending on the bullet
		  AllEnemys[i].energy -= Bulletmap[CurBullet->type].damage;

		  DeleteBullet( num );
		  GotHitSound ();

		  if (!CurBullet->mine)
		    {
		      FBTZaehler++;
		    }
		  CurBullet->type = OUT;
		  CurBullet->mine = FALSE;
		  // break;		/* Schleife beenden */
		  return;
		}			/* if getroffen */
	    }  /* for AllEnemys */


	  // check for collisions with other bullets
	  for (i = 0; i < MAXBULLETS; i++)
	    {
	      if (i == num) continue;  // never check for collision with youself.. ;)
	      if (AllBullets[i].type == OUT) continue; // never check for collisions with dead bullets..
	      if (AllBullets[i].type == FLASH) continue; // never check for collisions with flashes bullets..

	      xdist = AllBullets[i].pos.x-CurBullet->pos.x;
	      ydist = AllBullets[i].pos.y-CurBullet->pos.y;
	      if ( xdist*xdist + ydist*ydist > BULLET_COLL_DIST2 ) continue;

	      // it seems like we have a collision of two bullets!
	      // both will be deleted and replaced by blasts..
	      DebugPrintf (1, "\nBullet-Bullet-Collision detected...");

	      StartBlast(CurBullet->pos.x, CurBullet->pos.y, DRUIDBLAST);
	      //	??why start two blasts?
	      // StartBlast(AllBullets[num].pos.x, AllBullets[num].pos.y, DRUIDBLAST);
	      DeleteBullet (num);
	      DeleteBullet (i);
	    }

	} // for numsteps < num_check_steps

      break;
    } // switch ( Bullet-Type )
} /* CheckBulletCollisions */

/*@Function============================================================
  @Desc: CheckBlastCollsions(int num)
  checkt Collisionen des Blasts num mit Bullets und Druids
  UND reagiert darauf

  @Ret: void
  @Int:
  * $Function----------------------------------------------------------*/
void
CheckBlastCollisions (int num)
{
  int i;
  int level = CurLevel->levelnum;
  Blast CurBlast = &(AllBlasts[num]);
  Bullet CurBullet;
  float dist;
  vect vdist;

  /* check Blast-Bullet Collisions and kill hit Bullets */
  for (i = 0; i < MAXBULLETS; i++)
    {
      CurBullet = &AllBullets[i];
      if (CurBullet->type == OUT)
	continue;

      vdist.x = CurBullet->pos.x - CurBlast->PX;
      vdist.y = CurBullet->pos.y - CurBlast->PY;
      dist = sqrt(vdist.x*vdist.x + vdist.y*vdist.y);
      if (dist < Blast_Radius)
	{
	  StartBlast (CurBullet->pos.x, CurBullet->pos.y, BULLETBLAST);
	  DeleteBullet( i );
	}

    }	/* for */

  /* Check Blast-Enemy Collisions and smash energy of hit enemy */
  for (i = 0; i < NumEnemys; i++)
    {
      if ((AllEnemys[i].status == OUT)
	  || (AllEnemys[i].levelnum != level))
	continue;

      vdist.x = AllEnemys[i].pos.x - CurBlast->PX;
      vdist.y = AllEnemys[i].pos.y - CurBlast->PY;
      dist = sqrt(vdist.x*vdist.x + vdist.y*vdist.y);

      if (dist < Blast_Radius+Droid_Radius)
	{
	  /* drag energy of enemy */
	  AllEnemys[i].energy -= Blast_Damage_Per_Second * Frame_Time ();
	}

      if (AllEnemys[i].energy < 0)
	AllEnemys[i].energy = 0;

    }				/* for */

  /* Check influence-Blast collisions */
  vdist.x = Me.pos.x - CurBlast->PX;
  vdist.y = Me.pos.y - CurBlast->PY;
  dist = sqrt(vdist.x*vdist.x + vdist.y*vdist.y);

  if ( (Me.status != OUT) && (!CurBlast->mine) && (dist < Blast_Radius+Droid_Radius) )
    {
      if (!InvincibleMode)
	{
	  Me.energy -= Blast_Damage_Per_Second * Frame_Time ();

	  // So the influencer got some damage from the hot blast
	  // Now most likely, he then will also say so :)
	  if ( !CurBlast->MessageWasDone )
	    {
	      AddInfluBurntText();
	      CurBlast->MessageWasDone=TRUE;
	    }

	}
      // In order to avoid a new sound EVERY frame we check for how long the previous blast
      // lies back in time.  LastBlastHit is a float, that counts SECONDS real-time !!
      if (LastGotIntoBlastSound > 1.2)
	{
	  GotIntoBlastSound ();
	  LastGotIntoBlastSound = 0;
	}
    }

}				/* CheckBlastCollisions */

#undef _bullet_c
