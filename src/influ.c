/*----------------------------------------------------------------------
 *
 * Desc: all features, movement, fireing, collision and extras of the
 *	influencer are done in here.
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
#define _influ_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"

#define NOSTRAIGHTDIR 112
#define TIMETILLNEXTBULLET 14

#define REFRESH_ENERGY		3
// NORMALISATION #define COLLISION_PUSHSPEED	70
#define COLLISION_PUSHSPEED	7

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
@Desc: This function moves the influencer, adjusts his speed according to
keys pressed and also adjusts his status and current "phase" of his rotation.

@Ret: none
* $Function----------------------------------------------------------*/
void
MoveInfluence (void)
{
  float accel = Druidmap[Me.type].accel;
  static float TransferCounter = 0;
  /* zum Bremsen der Drehung, wenn man auf der Taste bleibt: */
  static int counter = -1;
  int i;

  DebugPrintf ("\nvoid MoveInfluence(void):  Real function call confirmed.");

  for (i=0; i<10; i++)
    {
      Me.Position_History[10-i].x=Me.Position_History[9-i].x;
      Me.Position_History[10-i].y=Me.Position_History[9-i].y;
    }
  Me.Position_History[0].x=Me.pos.x;
  Me.Position_History[0].y=Me.pos.y;

  counter++;
  counter %= BREMSDREHUNG;	/* Wird mal vom Druid abhaengen */

  PermanentLoseEnergy ();	/* influ permanently loses energy */

  /* Checken, ob Influencer noch OK */
  if (Me.energy <= 0)
    {
      if (Me.type != DRUID001)
	{
	  Me.type = DRUID001;
	  // NONSENSE FROM THE OLD ENGINE RedrawInfluenceNumber ();
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

  if ( (SpacePressed ()) && (NoDirectionPressed ()) &&
       (Me.status != WEAPON) && (Me.status != TRANSFERMODE) )
    TransferCounter += Frame_Time();

  if ( (SpacePressed ()) && (!NoDirectionPressed () ) &&
       (Me.status != TRANSFERMODE) )
    Me.status = WEAPON;

  if (Me.autofire)
    AutoFireBullet ();
  else
    if ((SpacePressed ()) && (!NoDirectionPressed ()) && (Me.status == WEAPON)
	&& (Me.firewait == 0) && (NoInfluBulletOnWay ()))
    FireBullet ();

  InfluenceFrictionWithAir (); // The influ should lose some of his speed when no key is pressed

  AdjustSpeed ();  // If the influ is faster than allowed for his type, slow him

  // Move influence according to current speed
  Me.pos.x += Me.speed.x * Frame_Time ();
  Me.pos.y += Me.speed.y * Frame_Time ();

  // Check it the influ is on a special field like a lift, a console or a refresh
  ActSpecialField ( Me.pos.x , Me.pos.y );

  AnimateInfluence ();	// move the "phase" of influencers rotation

  DebugPrintf ("\nvoid MoveInfluence(void):  Usual end of function reached.");

} /* MoveInfluence */


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
} // NoInfluBulletOnWay

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
    (Me.energy / (Druidmap[Me.type].maxenergy)) * Frame_Time () *
    ENEMYPHASES * 3;
  if (((int) rintf (Me.phase)) >= ENEMYPHASES)
    {
      Me.phase = 0;
    }


  /*
   * Farbe des Influencers (15) richtig setzen
   */

#define SET_INFLU_COLOR_EVERY_FRAME
#ifdef SET_INFLU_COLOR_EVERY_FRAME
  if ((Me.status == TRANSFERMODE) && ( (Me.energy*100 / Druidmap[Me.type].maxenergy) > BLINKENERGY))
    SetPalCol (INFLUENCEFARBWERT, Transfercolor.rot, Transfercolor.gruen,
	       Transfercolor.blau);

  if (((Me.status == MOBILE) || (Me.status == WEAPON) ) && ( (Me.energy*100 / Druidmap[Me.type].maxenergy) > BLINKENERGY))
    SetPalCol (INFLUENCEFARBWERT, Mobilecolor.rot, Mobilecolor.gruen,
	       Mobilecolor.blau);

  if (((Me.status == WEAPON) || (Me.status == WEAPON) ) && ( (Me.energy*100 / Druidmap[Me.type].maxenergy) > BLINKENERGY))
    SetPalCol (INFLUENCEFARBWERT, Mobilecolor.rot, Mobilecolor.gruen,
	       Mobilecolor.blau);
#endif

  /*
   * Wenn die Energie den kritischen Stand erreicht hat,
   * beginnt der Robot zu blinken
   */

#define CRYWAITTIME 14

  if ( ( (Me.energy*100/Druidmap[Me.type].maxenergy) <= BLINKENERGY) && (blinkwaiter == 0))
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
@Desc: This function checks for collisions of the influencer with walls,
doors, consoles, boxes and all other map elements.
In case of a collision, the position and speed of the influencer are
adapted accordingly.
NOTE: Of course this functions HAS to take into account the current framerate!
     
@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void
CheckInfluenceWallCollisions (void)
{
  int sign;
  double SX = Me.speed.x * Frame_Time ();
  double SY = Me.speed.y * Frame_Time ();
  finepoint lastpos;
  int res; 
  int NumberOfShifts=0;
  int safty_sx = 0, safty_sy = 0;	/* wegstoss - Geschwindigkeiten (falls noetig) */
  int NorthSouthAxisBlocked=FALSE;
  int EastWestAxisBlocked=FALSE;
  int H_Door_Sliding_Active = FALSE;
  

  int crashx = FALSE, crashy = FALSE;	/* Merker wo kollidiert wurde */

  lastpos.x = Me.pos.x - SX;
  lastpos.y = Me.pos.y - SY;

  // lastpos.x = Me.Position_History[0].x;
  // lastpos.y = Me.Position_History[0].y;

  res = DruidPassable (Me.pos.x, Me.pos.y);

#define NEW_BOUNCE_CHECK
#ifdef NEW_BOUNCE_CHECK

  // Influence-Wall-Collision only has to be checked in case of
  // a collision of course, which is indicated by res not CENTER.
  if (res != CENTER )
    {

      //--------------------
      // At first we just check in which directions (from the last position)
      // the ways are blocked and in which directions the ways are open.
      //
      if ( ! ( ( DruidPassable(lastpos.x , lastpos.y + Druidmap[Me.type].maxspeed * Frame_Time() ) != CENTER ) ||
	       ( DruidPassable(lastpos.x , lastpos.y - Druidmap[Me.type].maxspeed * Frame_Time() ) != CENTER ) ) )
	{
	  // printf("\nNorth-south-Axis seems to be free.");
	  NorthSouthAxisBlocked = FALSE;
	}
      else
	{
	  NorthSouthAxisBlocked = TRUE;
	}

      if ( ( DruidPassable(lastpos.x + Druidmap[Me.type].maxspeed * Frame_Time() , lastpos.y ) == CENTER ) &&
	   ( DruidPassable(lastpos.x - Druidmap[Me.type].maxspeed * Frame_Time() , lastpos.y ) == CENTER ) )
	{
	  // printf("\nEast-west-Axis seems to be free.");
	  EastWestAxisBlocked = FALSE;
	}
      else 
	{
	  EastWestAxisBlocked = TRUE;
	}

      //--------------------
      // Now we try to handle the sitution:
      //

      if ( NorthSouthAxisBlocked )
	{
	  // printf("\nNS-Axis seems NOT to be free.");
	  // printf("\nCorrection movement and position in this direction...");

	  // NorthSouthCorrectionDone=TRUE;
	  Me.pos.y = lastpos.y;
	  Me.speed.y = 0;
	  
	  // if its an open door, we also correct the east-west position, in the
	  // sense that we move thowards the middle
	  if ( (GetMapBrick(CurLevel, Me.pos.x , Me.pos.y - 0.5 ) == H_GANZTUERE ) || 
	       (GetMapBrick(CurLevel, Me.pos.x , Me.pos.y + 0.5 ) == H_GANZTUERE ) )
	    {
	      Me.pos.x += copysignf ( PUSHSPEED * Frame_Time() , ( rintf(Me.pos.x) - Me.pos.x ));
	      H_Door_Sliding_Active = TRUE;
	      // printf("\nDOOR SLIDING ACTIVATED!!!");
	    }
	}

      if ( EastWestAxisBlocked )
	{
	  // printf("\nEW-Axis seems NOT to be free.");
	  // printf("\nCorrection movement and position in this direction...");

	  // EastWestCorrectionDone=TRUE;
	  if ( !H_Door_Sliding_Active ) Me.pos.x = lastpos.x;
	  Me.speed.x = 0;

	  // if its an open door, we also correct the north-south position, in the
	  // sense that we move thowards the middle
	  if ( (GetMapBrick(CurLevel, Me.pos.x +0.5 , Me.pos.y) == V_GANZTUERE ) || 
	       (GetMapBrick(CurLevel, Me.pos.x -0.5 , Me.pos.y) == V_GANZTUERE ) )
	    Me.pos.y += copysignf (PUSHSPEED * Frame_Time() , ( rintf(Me.pos.y) - Me.pos.y ));
	}

      if ( EastWestAxisBlocked && NorthSouthAxisBlocked )
	{
	  // printf("\nBOTH AXES BLOCKED... Corner handling activated...");
	  // in case both axes were blocked, we must be at a corner.  
	  // both axis-blocked-routines have been executed, so the speed has
	  // been set to absolutely zero and we are at the previous position.
	  //
	  // But perhaps everything would be fine,
	  // if we just restricted ourselves to moving in only ONE direction.
	  // try if this would make sense...
	  // (Of course we may only move into the one direction that is free)
	  //
	  if ( DruidPassable( Me.pos.x + SX , Me.pos.y ) == CENTER ) Me.pos.x += SX;
	  if ( DruidPassable( Me.pos.x , Me.pos.y +SY ) == CENTER ) Me.pos.y += SY;
	}

      // Here I introduce some extra security as a fallback:  Obviously
      // if the influencer is blocked FOR THE SECOND TIME, then the throw-back-algorithm
      // above HAS FAILED.  The absolutely fool-proof and secure handling is now done by
      // simply reverting to the last influ coordinated, where influ was NOT BLOCKED.
      // For this reason, a history of influ-coordinates has been introduced.  This will all
      // be done here and now:
      
      if ( (DruidPassable (Me.pos.x, Me.pos.y) != CENTER) && 
	   (DruidPassable (Me.Position_History[0].x, Me.Position_History[0].y) != CENTER) &&
	   (DruidPassable (Me.Position_History[1].x, Me.Position_History[1].y) != CENTER) )
	{
	  Me.pos.x=Me.Position_History[2].x;
	  Me.pos.y=Me.Position_History[2].y;
	  printf("\nATTENTION! CheckInfluenceWallCollsision FALLBACK ACTIVATED!!");
	}

    }

  return;

#endif 

  switch (res)
    {
      // In this case, the influencer is (completely?) blocked.
    case -1:
      // --------------------
      // We handle here the case, that the influencer is completely blocked.
      // WHAT DO WE DO?  ---  The new algorithm proceeds as follows:
      // 1. Check if the north-south axis would be free FROM THE PREVIOUS POSITION
      // 2. Check if the east-west axis would be free FROM THE PREVIOUS POSITION
      //    and both of the above under the assumption of full speed.
      // 3. If the north south axis is free, it must have been the east-west movement
      //    otherwise it must have been the north-west movement, which caused the
      //    collision.
      // 4. Therefore restore the last position and move from there, but only in
      //    the free direction and not in the other.
      //

      /*
      if ( ( DruidPassable(lastpos.x , lastpos.y + Druidmap[Me.type].maxspeed * Frame_Time() ) == CENTER ) &&
	   ( DruidPassable(lastpos.x , lastpos.y - Druidmap[Me.type].maxspeed * Frame_Time() ) == CENTER ) )
	{
	  printf("\nNorth-south-Axis seems to be free.");
	}
      else
	{
	  printf("\nNorth-south-Axis seems NOT to be free.");
	  printf("\nCorrection movement and position in this direction...");
	  Me.pos.y = lastpos.y;
	  Me.speed.y = 0;
	  // return;
	}

      if ( ( DruidPassable(lastpos.x + Druidmap[Me.type].maxspeed * Frame_Time() , lastpos.y ) == CENTER ) &&
	   ( DruidPassable(lastpos.x - Druidmap[Me.type].maxspeed * Frame_Time() , lastpos.y ) == CENTER ) )
	{
	  printf("\nEast-west-Axis seems to be free.");
	}
      else 
	{
	  printf("\nEast-west-Axis seems NOT to be free.");
	  printf("\nCorrection movement and position in this direction...");
	  Me.pos.x = lastpos.x;
	  Me.speed.x = 0;
	  //return;
	}

      return;

      */

      /* Festellen, in welcher Richtung die Mauer lag,
         und den Influencer entsprechend stoppen */
      if ( rintf(SX) && (DruidPassable (lastpos.x + SX, lastpos.y) != CENTER))
	{
	  crashx = TRUE;	/* In X wurde gecrasht */
	  sign = (SX < 0) ? -1 : 1;
	  SX = abs (SX);
	  NumberOfShifts=0;
	  while (--SX
		 && (DruidPassable (lastpos.x + sign * SX, lastpos.y) !=
		     CENTER) && (NumberOfShifts++ < 4));
	  Me.pos.x = lastpos.x + SX * sign;
	  Me.speed.x = 0;

	  /* falls Influencer weggestossen werden muss ! */
	  safty_sx = (-1) * sign * PUSHSPEED;
	}

      if (rintf(SY) && (DruidPassable (lastpos.x, lastpos.y + SY) != CENTER))
	{
	  crashy = TRUE;	/* in Y wurde gecrasht */
	  sign = (SY < 0) ? -1 : 1;
	  SY = abs (SY);
	  NumberOfShifts=0;
	  while (--SY
		 && (DruidPassable (lastpos.x, lastpos.y + sign * SY) !=
		     CENTER) && (NumberOfShifts++ < 4));
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
	      Me.pos.x += Me.speed.x * Frame_Time() ;
	    }

	  if (crashy)
	    {
	      Me.speed.y = safty_sy;
	      Me.pos.y += Me.speed.y * Frame_Time() ;
	    }
	}

      break;

      /* Von Tuerrand wegschubsen */
    case OBEN:
      Me.speed.y = -PUSHSPEED;
      Me.pos.y += Me.speed.y; // * Frame_Time();
      break;

    case UNTEN:
      Me.speed.y = PUSHSPEED;
      Me.pos.y += Me.speed.y; // * Frame_Time() ;
      break;

    case RECHTS:
      Me.speed.x = PUSHSPEED;
      Me.pos.x += Me.speed.x; // * Frame_Time() ;
      break;

    case LINKS:
      Me.speed.x = -PUSHSPEED;
      Me.pos.x += Me.speed.x; // * Frame_Time() ;
      break;

      /* Not blocked at all ! */
    case CENTER:
      break;

    default:
      DebugPrintf ("Illegal return value from DruidPassable() ");
      Terminate (-1);
      break;

    } /* switch */

  // This old bouncing code is no longer working in all cases due to bigger numbers
  // and frame_rate dependence.  I therefore introduce some extra security:  Obviously
  // if the influencer is blocked FOR THE SECOND TIME, then the throw-back-algorithm
  // above HAS FAILED.  The absolutely fool-proof and secure handling is now done by
  // simply reverting to the last influ coordinated, where influ was NOT BLOCKED.
  // For this reason, a history of influ-coordinates has been introduced.  This will all
  // be done here and now:

  if ( (DruidPassable (Me.pos.x, Me.pos.y) != CENTER) && 
       (DruidPassable (Me.Position_History[0].x, Me.Position_History[0].y) != CENTER) &&
       (DruidPassable (Me.Position_History[1].x, Me.Position_History[1].y) != CENTER) )
    {
      Me.pos.x=Me.Position_History[2].x;
      Me.pos.y=Me.Position_History[2].y;
    }

} /* CheckInfluenceWallCollisions */

/*@Function============================================================
@Desc: Dies Prozedur passt die momentane Geschwindigkeit an die Hoechst-
	geschwindigkeit an.
@Ret: keiner
@Int: keiner
* $Function----------------------------------------------------------*/
void
AdjustSpeed (void)
{
  double maxspeed = Druidmap[Me.type].maxspeed;
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
InfluenceFrictionWithAir (void)
{

  if (!UpPressed () && !DownPressed ())
    {
      /*
	if (Me.speed.y < 0)
	Me.speed.y++;
	if (Me.speed.y > 0)
	Me.speed.y--;
      */
      Me.speed.y *= exp(log(0.02) * Frame_Time());
    }
  if (!RightPressed () && !LeftPressed ())
    {
      /*
	if (Me.speed.x < 0)
	Me.speed.x++;
	if (Me.speed.x > 0)
	Me.speed.x--;
      */
      Me.speed.x *= exp(log(0.02) * Frame_Time());
    }

} // InfluenceFrictionWithAir (void)

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
  for (i = 0; i < 10; i++)
    {
      /* freien Blast finden */
      counter = 0;
      while (AllBlasts[counter++].type != OUT);
      counter -= 1;
      if (counter >= MAXBLASTS)
	{
	  printf("\n\nWent out of blasts in ExplodeInfluencer...\n\n");
	  Terminate(ERR);
	}
      AllBlasts[counter].type = DRUIDBLAST;
      AllBlasts[counter].PX =
	Me.pos.x - DRUIDRADIUSX / 2 + MyRandom (10)*0.05;
      AllBlasts[counter].PY =
	Me.pos.y - DRUIDRADIUSY / 2 + MyRandom (10)*0.05;
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
CheckInfluenceEnemyCollision (void)
{
  int i;
  float xdist;
  float ydist;
  float dist2;
  int swap;
  int first_collision = TRUE;	/* marker */

  // return;

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

      if (abs (xdist) > 1)
	continue;
      if (abs (ydist) > 1)
	continue;

      dist2 = sqrt( xdist *xdist + ydist * ydist );
      if (dist2 > 2 * DRUIDRADIUSX)
	continue;


      if (Me.status != TRANSFERMODE)
	{

	  if (first_collision)
	    {			
	      /* nur beim ersten mal !!! */
	      /* den Geschwindigkeitsvektor des Influencers invertieren */
	      Me.speed.x = -Me.speed.x;
	      Me.speed.y = -Me.speed.y;

	      if (Me.speed.x != 0)
		Me.speed.x +=
		  COLLISION_PUSHSPEED * (Me.speed.x / fabsf (Me.speed.x));
	      else if (xdist)
		Me.speed.x = COLLISION_PUSHSPEED * (xdist / fabsf (xdist));
	      if (Me.speed.y != 0)
		Me.speed.y +=
		  COLLISION_PUSHSPEED * (Me.speed.y / fabsf (Me.speed.y));
	      else if (ydist)
		Me.speed.y = COLLISION_PUSHSPEED * (ydist / fabsf (ydist));

	      /* den Influencer etwas aus dem Feind hinausschieben */
	      Me.pos.x += Me.speed.x * Frame_Time ();
	      Me.pos.y += Me.speed.y * Frame_Time ();

	      /* etwaige Wand - collisionen beruecksichtigen */
	      CheckInfluenceWallCollisions ();

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

} // CheckInfluenceEnemyCollision

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
  float BulletSpeedX = Bulletmap[guntype].speed;
  float BulletSpeedY = Bulletmap[guntype].speed;
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

  // To prevent influ from hitting himself with his own bullets,
  // move them a bit..

  //NORMALISATION CurBullet->pos.x += isignf (CurBullet->speed.x) * Block_Width / 2;
  CurBullet->pos.x += isignf (CurBullet->speed.x) * 0.5;
  //NORMALISATION CurBullet->pos.y += isignf (CurBullet->speed.y) * Block_Height / 2;
  CurBullet->pos.y += isignf (CurBullet->speed.y) * 0.5;

  //  CurBullet->pos.x += Me.speed.x * Frame_Time();
  //  CurBullet->pos.y += Me.speed.y * Frame_Time();

  // NORMALISATION if ((fabsf (BulletSpeedX) < (13)) && (fabsf (BulletSpeedY) < (13)) )
  if ((fabsf (BulletSpeedX) < (26/64.0)) && (fabsf (BulletSpeedY) < (26/64.0)) )
    {
      //NORMALISATION CurBullet->pos.x += isignf (CurBullet->speed.x) * Block_Width / 3;
      CurBullet->pos.x += isignf (CurBullet->speed.x)  / 3.0;
      //NORMALISATION CurBullet->pos.y += isignf (CurBullet->speed.y) * Block_Height / 3;
      CurBullet->pos.y += isignf (CurBullet->speed.y)  / 3.0;
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
      Me.energy += REFRESH_ENERGY * Frame_Time () * 5;
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
  // Of course if in invincible mode, no energy will ever be lost...
  if (InvincibleMode) return;

  /* health decreases with time */
  Me.health -= Druidmap[Me.type].lose_health * Frame_Time ();

  /* you cant have more energy than health */
  if (Me.energy > Me.health) Me.energy = Me.health;

} // void PermanentLoseEnergy(void)

#undef _influ_c
