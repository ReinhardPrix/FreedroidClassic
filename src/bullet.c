/*----------------------------------------------------------------------
 *
 * Desc: all Bullet AND Blast - related functions.
 *	 
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
#define DRUIDHITDIST2		(0.3+MORE)*(Druid_Radius_Y+MORE)
// #define DRUIDHITDIST2		0


/* ----------------------------------------------------------------------
 * This function moves all the bullets according to their speeds and the
 * current frame rate of course.
 * ---------------------------------------------------------------------- */
void
MoveBullets (void)
{
  int i;
  int map_x;
  int map_y;
  Bullet CurBullet;

  // movement of hte bullets
  for (CurBullet = AllBullets, i = 0; i < MAXBULLETS; CurBullet++, i++)
    {
      if (CurBullet->type == OUT)
	continue;

      CurBullet->pos.x += CurBullet->speed.x * Frame_Time ();
      CurBullet->pos.y += CurBullet->speed.y * Frame_Time ();

      CurBullet->time_in_frames++;
      CurBullet->time_in_seconds += Frame_Time();

      map_x= (int) rintf( CurBullet->pos.x );
      map_y= (int) rintf( CurBullet->pos.y );

      switch ( CurLevel->map[ map_y ] [ map_x ] )
	{
	case CONVEY_L:
	  CurBullet->pos.x += Conveyor_Belt_Speed * Frame_Time();
	  break;
	case CONVEY_R:
	  CurBullet->pos.x -= Conveyor_Belt_Speed * Frame_Time();
	  break;
	case CONVEY_U:
	  CurBullet->pos.y -= Conveyor_Belt_Speed * Frame_Time();
	  break;
	case CONVEY_D:
	  CurBullet->pos.y += Conveyor_Belt_Speed * Frame_Time();
	  break;
	default:
	  break;
	}
      /*
         UM ZU VERHINDERN, DASS DIE BULLETS, DIE ETWAS TREFFEN, NICHT MEHR
         DARGESTELLT WERDEN, PASSIERT DIE BULLETKOLLISIONSABFRAGE ERST NACH
         DER ZUSAMMENSTELLUNG DES INTERNFENSTERS. jp, 23.5.94 */

      /* Kollisionen mit Mauern und Druids checken UND behandeln */
      //          CheckBulletCollisions(i);

    }				/* for */
}				// void MoveBullets(void)


/*@Function============================================================
@Desc: Diese Funktion loescht das Bullet mit der uebergebenen Nummer

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void 
DeleteBullet ( int Bulletnumber , int ShallWeStartABlast )
{
  Bullet CurBullet = &AllBullets[ Bulletnumber ];
  int i;

  //--------------------
  // At first we generate the blast at the collision spot of the bullet,
  // cause later, after the bullet is deleted, it will be hard to know
  // the correct location ;)
  if ( ShallWeStartABlast ) StartBlast ( CurBullet->pos.x, CurBullet->pos.y, BULLETBLAST );

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
  CurBullet->pos.x = 0;
  CurBullet->pos.y = 0;
  CurBullet->angle = 0;

}; // void DeleteBullet( int Bulletnumber , int StartBlast )

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
  int map_x, map_y;

  // DebugPrintf( 1 , "\nvoid StartBlast( ... ) called with x=%f y=%f" , x , y );
  //--------------------
  // first we see if there are any destructible map tiles, that need to
  // be destructed this way...
  //
  map_x=(int)rintf(x);
  map_y=(int)rintf(y);

  // DebugPrintf( 1 , "\nmap value at blast destination : %d . " , CurLevel->map[ map_y ][ map_x ] );
  if ( CurLevel->map[ map_y ][ map_x ] == BOX_4 ) 
    {
      CurLevel->map[ map_y ][ map_x ] = FLOOR;
    }
  if ( ( CurLevel->map[ map_y ][ map_x ] == BOX_1 ) ||
       ( CurLevel->map[ map_y ][ map_x ] == BOX_2 ) ||
       ( CurLevel->map[ map_y ][ map_x ] == BOX_3 ) )
    {
      CurLevel->map[ map_y ][ map_x ]++;
    }

  // find out the position of the next free blast
  for (i = 0; i < MAXBLASTS; i++)
    if (AllBlasts[i].type == OUT)
      break;

  // didn't fine any --> then take the first one
  if (i >= MAXBLASTS)
    i = 0;

  // get pointer to it: more comfortable 
  NewBlast = &(AllBlasts[i]);

  // create a blast at the specified x/y coordinates
  NewBlast->PX = x;
  NewBlast->PY = y;

  NewBlast->type = type;
  NewBlast->phase = 0;

  NewBlast->MessageWasDone = 0;

  if (type == DRUIDBLAST)
    {
      DruidBlastSound ();
    }

}; // void StartBlast( ... )

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
DeleteBlast (int BlastNum)
{
  AllBlasts[ BlastNum ].phase = OUT;
  AllBlasts[ BlastNum ].type = OUT;
}; // void DeleteBlast( int BlastNum )

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
  int i;
  int level = CurLevel->levelnum;
  double xdist, ydist;
  Bullet CurBullet = &AllBullets[num];
  static int FBTZaehler = 0;

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
      if ( CurBullet->time_in_seconds > FLASH_DURATION_IN_SECONDS )
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
      if ( CurBullet->time_in_frames != 1 ) return; // we only do the damage once and thats at frame nr. 1 of the flash
      
      // for (i = 0; i < MAX_ENEMYS_ON_SHIP; i++)
      for (i = 0; i < Number_Of_Droids_On_Ship ; i++)
	{
	  if ( IsVisible (&AllEnemys[i].pos) &
	       (!Druidmap[AllEnemys[i].type].flashimmune) )
	    {
	      // ITEMS AllEnemys[i].energy -= Bulletmap[FLASH].damage;
	      AllEnemys[i].energy -= CurBullet->damage;
	      // Since the enemy just got hit, it might as well say so :)
	      EnemyHitByBulletText( i );
	    }
	}
      
      if (!InvincibleMode && !Druidmap[Me.type].flashimmune)
	{
	  // ITEMS Me.energy -= Bulletmap[FLASH].damage ;
	  Me.energy -= CurBullet->damage ;
	}

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
      
      // Check for collision with background
      if (IsPassable (CurBullet->pos.x, CurBullet->pos.y, CENTER) != CENTER)
	{
	  DeleteBullet ( num , TRUE ); // we want a bullet-explosion
	  return;			
	}
      
      // check for collision with influencer
      xdist = Me.pos.x - CurBullet->pos.x;
      ydist = Me.pos.y - CurBullet->pos.y;
      if ((xdist * xdist + ydist * ydist) < DRUIDHITDIST2)
	{
	  GotHitSound ();
	  
	  if (!InvincibleMode)
	    {
	      // ITEMS Me.energy -= Bulletmap[CurBullet->type].damage;	// loose some energy
	      Me.energy -= CurBullet->damage;	// loose some energy
	    }
	  
	  // The bullet has hit.  The damage has been calculated.  Now it can be disabled.
	  // ATTENTION!  These instructions belong here and must not be moved up.
	  // CurBullet->type = OUT;
	  // CurBullet->mine = FALSE;
	  DeleteBullet( num , TRUE ); // we want a bullet-explosion
	  return;			/* Bullet ist hin */
	}
      
      // check for collision with enemys
      // for (i = 0; i < NumEnemys; i++)
      for (i = 0; i < Number_Of_Droids_On_Ship; i++)
	{
	  if (AllEnemys[i].Status == OUT || AllEnemys[i].levelnum != level)
	    continue;
	  
	  xdist = CurBullet->pos.x - AllEnemys[i].pos.x;
	  ydist = CurBullet->pos.y - AllEnemys[i].pos.y;
	  
	  if ((xdist * xdist + ydist * ydist) < DRUIDHITDIST2)
	    {
	      // The enemy who was hit, loses some energy, depending on the bullet
	      // ITEMS AllEnemys[i].energy -= Bulletmap[CurBullet->type].damage;
	      AllEnemys[i].energy -= CurBullet->damage;
	      GotHitSound ();

	      // Maybe he will also stop doing his fixed routine and return to normal
	      // operation as well
	      AllEnemys[i].AdvancedCommand = 0;

	      // We might also start a little bullet-blast even after the
	      // collision of the bullet with an enemy (not in Paradroid)
	      DeleteBullet( num , TRUE ); // we want a bullet-explosion

	      Enemy_Post_Bullethit_Behaviour( i );

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

	  if ( fabsf(AllBullets[i].pos.x-CurBullet->pos.x) > BULLET_BULLET_COLLISION_DIST ) continue;
	  if ( fabsf(AllBullets[i].pos.y-CurBullet->pos.y) > BULLET_BULLET_COLLISION_DIST ) continue;
	  // it seems like we have a collision of two bullets!
	  // both will be deleted and replaced by blasts..
	  DebugPrintf (1, "\nBullet-Bullet-Collision detected...");
	  
	  //CurBullet->type=OUT;
	  //AllBullets[num].type=OUT;
	  StartBlast(CurBullet->pos.x, CurBullet->pos.y, DRUIDBLAST);
	  StartBlast(AllBullets[num].pos.x, AllBullets[num].pos.y, DRUIDBLAST);
	  DeleteBullet( num , TRUE ); // we want a bullet-explosion
	}
      break;
    } // switch ( Bullet-Type )
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
void
CheckBlastCollisions (int num)
{
  int i;
  int level = CurLevel->levelnum;
  Blast CurBlast = &(AllBlasts[num]);
  // static int RHBZaehler = 0;

  /* check Blast-Bullet Collisions and kill hit Bullets */
  for (i = 0; i < MAXBULLETS; i++)
    {
      if (AllBullets[i].type == OUT)
	continue;
      if (CurBlast->phase > 4)
	break;

      if (abs (AllBullets[i].pos.x - CurBlast->PX) < Blast_Radius)
	if (abs (AllBullets[i].pos.y - CurBlast->PY) < Blast_Radius)
	  {
	    /* KILL Bullet silently */
	    //AllBullets[i].type = OUT;
	    //AllBullets[i].mine = FALSE;
	    DeleteBullet( i , TRUE ); // we want a bullet-explosion
	  }

    }				/* for */

  /* Check Blast-Enemy Collisions and smash energy of hit enemy */
  for (i = 0; i < NumEnemys; i++)
    {
      if ((AllEnemys[i].Status == OUT)
	  || (AllEnemys[i].levelnum != level))
	continue;

      //      if ( ( abs (AllEnemys[i].pos.x - CurBlast->PX) < Blast_Radius + Druid_Radius_X ) &&
      //         ( abs (AllEnemys[i].pos.y - CurBlast->PY) < Blast_Radius + Druid_Radius_Y ) )
      if ( ( fabsf (AllEnemys[i].pos.x - CurBlast->PX) < Blast_Radius ) &&
	   ( fabsf (AllEnemys[i].pos.y - CurBlast->PY) < Blast_Radius ) )
	  {
	    /* drag energy of enemy */
	    AllEnemys[i].energy -= Blast_Damage_Per_Second * Frame_Time ();
	  }

      if (AllEnemys[i].energy < 0)
	AllEnemys[i].energy = 0;

    }				/* for */

  /* Check influence-Blast collisions */
  if ( (Me.status != OUT) && 
       ( fabsf (Me.pos.x - CurBlast->PX) < Blast_Radius ) &&
       ( fabsf (Me.pos.y - CurBlast->PY) < Blast_Radius ) )
    {
      if (!InvincibleMode)
	{
	  Me.energy -= Blast_Damage_Per_Second * Frame_Time ();
	  if ((PlusExtentionsOn) && (LastBlastHit > 5))
	    InsertMessage ("Blast hit me! OUCH!");
	  LastBlastHit = 0;
	  
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
