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
 * Desc: all the functions managing the things one gets to see.
 *	That includes assembling of enemys, assembling the currently
 *	relevant porting of the map (the bricks I mean), drawing all visible
 *	elements like bullets, blasts, enemys or influencer in a nonvisible
 *	place in memory at first, and finally drawing them to the visible
 *	screen for the user.
 *
 *----------------------------------------------------------------------*/
#include <config.h>

/*
 * Dieses Modul enth"alt Funktionen, die dem Aufbau des Bildes dienen.
 *
 */

#define _view_c

/* locale Schalter zu DEBUG-Zwecken */
#define INFLUENCEOFF  	0
#define BULLETOFF			0
#define BLASTOFF			0
#define ENEMYOFF			0

//#define SHOWSTATS

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <vga.h>
#include <vgagl.h>

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "map.h"
#include "proto.h"
#include "colodefs.h"

void GetConceptInternFenster (void);
void FlashWindow (int Flashcolor);
void RecFlashFill (int LX, int LY, int Color, unsigned char *screen,
		   int SBreite);
int Cent (int);

char *Affected;

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
int
Cent (int Val)
{
  Val = Val - (Val % BLOCKBREITE) + BLOCKBREITE / 2;
  return Val;
}

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
RecFlashFill (int LX, int LY, int Color, unsigned char *Screen, int SBreite)
{
  int i;
  static int num;

  LY = LY;
  LX = LX;
  num++;

//      gotoxy(1,1);
//      printf(" RFF: X=%d Y=%d.\n",LX,LY);
//      getchar();

  // Dieses Feld als Wirkungsbereich kenntzeichnen
  Affected[LY / BLOCKHOEHE * CurLevel->xlen + LX / BLOCKBREITE] = TRUE;

  // Dieses Feld anf"ullen
  for (i = LY / 4 - ((LY / 4) % 8); i < (LY / 4 - ((LY / 4) % 8) + 8); i++)
    {
      memset (Screen + i * SBreite + LX / 4 - ((LX / 4) % 8), Color, 8);
    }
  i -= 4;

  // Feld rechts davon anf"ullen
  if ((*(Screen + i * SBreite + LX / 4 + 8) != Color) &&
      (IsPassable (Cent (LX + BLOCKBREITE), Cent (LY), CENTER) == CENTER))
    RecFlashFill (LX + BLOCKBREITE, LY, Color, Screen, SBreite);

  // Feld links davon anf"ullen
  if (LX > BLOCKBREITE)
    {
      if ((*(Screen + i * SBreite + LX / 4 - 8) != Color) &&
	  (IsPassable (Cent (LX - BLOCKBREITE), Cent (LY), CENTER) == CENTER))
	RecFlashFill (LX - BLOCKBREITE, LY, Color, Screen, SBreite);
    }

  // Feld oben davon anf"ullen
  if ((i > 8) && (LY > BLOCKHOEHE))
    {
      if ((*(Screen + (i - 8) * SBreite + LX / 4) != Color) &&
	  (IsPassable (Cent (LX), Cent (LY - BLOCKHOEHE), CENTER) == CENTER))
	RecFlashFill (LX, LY - BLOCKHOEHE, Color, Screen, SBreite);
    }

  // Feld unten davon anf"ullen
  if ((*(Screen + (i + 8) * SBreite + LX / 4) != Color) &&
      (IsPassable (Cent (LX), Cent (LY + BLOCKHOEHE), CENTER) == CENTER))
    RecFlashFill (LX, LY + BLOCKHOEHE, Color, Screen, SBreite);
}

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
unsigned char *
FeindZusammenstellen (const char *FZahl, int FPhase)
{
  unsigned char *LSrce;
  unsigned char *LDest;
  int i;
  int Verschiebung;
  int ZielVerschiebung;

  DebugPrintf
    ("\nunsigned char * FeindZusammenstellen(...): real function call confirmed.\n");

  //  printf( "\n Zuletzt berichtete Phase in FeindZusammenstellen(...) is: %d." , FPhase );
  //  printf( "\n Zuletzt berichtete Zahl is: %s.\n\n" , FZahl );

  for (i = 0; i < 3; i++)
    {
      printf("\n Variable i ist jetzt: %d.",i);

      Verschiebung = (*(FZahl + i) - '1' + 11 ) * 9 * 9;

      if (Verschiebung < 0) 
	{
	  printf(" SUSPICIOUS Verschiebung in FeindZusammenstellen: NEGATIVE. Terminating.");
	  Terminate(ERR);
	}
      
      if (Verschiebung >= DIGITMEM) 
	{
	  printf(" SUSPICIOUS Verschiebung in FeindZusammenstellen: > DIGITMEM!. Terminating.");
	  Terminate(ERR);
	}

      /*
      if ( ( (*(FZahl + i)) - '1' -1 ) < 0 )
	{
	  printf(" Digit das kein Digit ist in FeindZusammenstellen?: Terminiere.");
	  Terminate(ERR);
	}
      */

      ZielVerschiebung =  NUMBEROFS + i * 8 + BLOCKMEM * FPhase; 

      if ( ZielVerschiebung < 0) 
	{
	  printf(" SUSPICIOUS ZielVerschiebung in FeindZusammenstellen: NEGATIVE. Terminating.");
	  Terminate(ERR);
	}
      
      if ( ZielVerschiebung >= ENEMYPHASES * BLOCKMEM ) 
	{
	  printf(" SUSPICIOUS ZielVerschiebung in FeindZusammenstellen: > ENEMYPHASES * BLOCKMEM!. Terminating.");
	  Terminate(ERR);
	}

      LSrce = Digitpointer + Verschiebung;
      LDest = Enemypointer + ZielVerschiebung;
      DrawDigit ( LSrce , LDest );
    }

  DebugPrintf
    ("\nunsigned char * FeindZusammenstellen(...): usual end of function reached.\n");

  return (Enemypointer + FPhase * BLOCKMEM);
} // unsigned char * FeindZusammenstellen (...)


/*@Function============================================================
@Desc: 	Diese Prozedur stellt den momentan intern sichtbaren Kartenausschnitt
			zusammen

			Parameter: keine

@Ret: 	void
* $Function----------------------------------------------------------*/
void
GetView (void)
{
  int col, line;
  finepoint testpos;
  int me_gx, me_gy;		/* influ- Grobkoord. */
  signed int mapX0, mapY0;
  signed int mapX, mapY;	/* The map-coordinates, which are to be copied */

  DebugPrintf ("\nvoid GetView(void): Funktion echt aufgerufen.");
  printf ("\nvoid GetView(void): CurLevel->xlen: %d. CurLevel->ylen: %d.",
	  CurLevel->xlen, CurLevel->ylen);

  me_gx = Me.pos.x / BLOCKBREITE;
  me_gy = Me.pos.y / BLOCKHOEHE;

  mapX0 = me_gx - (INTERNBREITE / 2);
  mapY0 = me_gy - (INTERNHOEHE / 2);

  mapX = mapX0;
  mapY = mapY0;

  for (line = 0; line < INTERNHOEHE; line++, mapY++)
    {
      mapX = mapX0;
      for (col = 0; col < INTERNBREITE; col++, mapX++)
	{

	  /* Achtung, falls map-brick nicht mehr sichtbar ! */
	  if (HideInvisibleMap)
	    {
	      testpos.x = mapX * BLOCKBREITE + BLOCKBREITE / 2;
	      testpos.y = mapY * BLOCKHOEHE + BLOCKHOEHE / 2;

	      /* relative Lage des Blocks zum Influ. ber"ucksichtigen */
	      if (me_gx < mapX)
		testpos.x -= BLOCKBREITE / 2;
	      else if (me_gx > mapX)
		testpos.x += BLOCKBREITE / 2;
	      if (me_gy < mapY)
		testpos.y -= BLOCKHOEHE / 2;
	      else if (me_gy > mapY)
		testpos.y += BLOCKHOEHE / 2;

	      if (!IsVisible (&testpos))
		{
		  View[line][col] = INVISIBLE_BRICK;
		  continue;
		}
	    }

	  /* Achtung wenn Koordinate ausserhalb der Map */
	  if (mapX < 0 ||
	      mapY < 0 ||
	      mapX >= (CurLevel->xlen) || mapY >= (CurLevel->ylen))
	    View[line][col] = VOID;
	  else
	    View[line][col] = CurLevel->map[mapY][mapX];
	}			/* for col */
    }				/* for line */

  DebugPrintf
    ("\nvoid GetView(void): Funktionsende ordnungsgemaess erreicht.");
  return;
}				/* GetView */


/*@Function============================================================
@Desc:  	Diese Prozedur gibt den momentan intern sichtbaren Kartenausschnitt in
			Textform	am Bildschirm aus

		   Parameter: keine

@Ret: 	void
* $Function----------------------------------------------------------*/
void
DisplayView (void)
{
  int i;
  int ii;

  DebugPrintf (" Das Interne Fenster sieht wie folgt aus:\n");
  for (i = 0; i < INTERNHOEHE; i++)
    {
      for (ii = 0; ii < INTERNBREITE; ii++)
	{
	  printf ("%u-", View[i][ii]);
	}
      DebugPrintf ("\n");
    }
}


/*-----------------------------------------------------------------
 * @Desc: Diese Prozedur setzt das Bild im Speicher zusammen, damit
 * 	es dann von PutInternFenster in den Bildschirmspeicher kopiert
 * 	werden kann.
 * Param: mask= SHOW_ALL:   display all entities
 *              SHOW_MAP:    display only map
 *
 * @Ret: none
 *-----------------------------------------------------------------*/
void
GetInternFenster (int mask)
{
  Blast CurBlast = &(AllBlasts[0]);
  int MapBrick;
  int line, col;
  int i, j;

  unsigned char *source;	/* the current block to copy */
  unsigned char *target;

  target = InternWindow;
  DebugPrintf ("\nvoid GetInternFenster(void) wurde ECHT aufgerufen.\n");

  memset (target, 1, INTERNHOEHE * INTERNBREITE * BLOCKMEM);

  if (Conceptview)
    {
      GetConceptInternFenster ();
      return;
    }

  for (line = 0; line < (INTERNHOEHE); line++)
    {
      for (col = 0; col < (INTERNBREITE); col++)
	{
	  if ((MapBrick = View[line][col]) != INVISIBLE_BRICK)
	    {
	      source = MapBlocks + MapBrick * BLOCKMEM;
	      for (i = 0; i < BLOCKHOEHE; i++)
		{
		  memcpy (target, source, BLOCKBREITE);
		  target += INTERNBREITE * BLOCKBREITE;
		  source += BLOCKBREITE;
		}
	    }			/* if !INVISIBLE_BRICK */
	  else
	    {
	      target += INTERNBREITE * BLOCKMEM;
	    }
	  if (col < INTERNBREITE - 1)
	    target -= INTERNBREITE * BLOCKMEM - BLOCKBREITE;
	}			/* for(col) */
      target -= (INTERNBREITE - 1) * BLOCKBREITE;
    }				/* for(line) */

  if (mask == SHOW_MAP)		/* we want only the map, nothing else */
    return;


#if ENEMYOFF == 0
  for (i = 0; i < NumEnemys; i++)
    PutEnemy (i);
#endif

  if (Me.energy > 0)
    PutInfluence ();

  for (i = 0; i < (MAXBULLETS); i++)
    if (AllBullets[i].type != OUT)
      PutBullet (i);


  for (i = 0; i < (MAXBLASTS); i++)
    if (AllBlasts[i].type != OUT)
      PutBlast (i);

  // Sofortiger Check auf Bullet-Blast-Kollisionen
  for (j = 0; j < MAXBLASTS; j++)
    {
      /* check Blast-Bullet Collisions and kill hit Bullets */
      for (i = 0; i < MAXBULLETS; i++)
	{
	  if (AllBullets[i].type == OUT)
	    continue;
	  if (CurBlast->phase > 4)
	    break;

	  if (abs (AllBullets[i].pos.x - CurBlast->PX) < BLASTRADIUS)
	    if (abs (AllBullets[i].pos.y - CurBlast->PY) < BLASTRADIUS)
	      {
		/* KILL Bullet silently */
		AllBullets[i].type = OUT;
		AllBullets[i].mine = FALSE;
	      }
	}			/* for */
      CurBlast++;
    }				/* for */
  DebugPrintf
    ("\nvoid GetInternFenster(void): Ende der Funktion fehlerfrei erreicht...\n");
}				// void GetInternFenster(void) 

/*@Function============================================================
@Desc: GetConceptInternFenster(): analoge Funktion zu GetInternFenster()
			fuer Concept-View.

		Parameter: keine
@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void
GetConceptInternFenster (void)
{
  int LX = 0;
  int LY = 0;
  int i, j;

  // Darstellen der blo"sen Deckkarte 
  for (i = 0; i < CurLevel->ylen; i++)
    {
      for (j = 0; j < CurLevel->xlen; j++)
	{
	  SmallBlock (LX, LY,
		      GetMapBrick (CurLevel, j * BLOCKBREITE, i * BLOCKHOEHE),
		      InternWindow, INTERNBREITE * BLOCKBREITE);
	  LX += 8;
	}
      LX = 0;
      LY += 8;
    }

  // Darstellen der Feinde am Deck
  for (i = 0; i < MAX_ENEMYS_ON_SHIP; i++)
    {
      if (Feindesliste[i].levelnum != CurLevel->levelnum)
	continue;
      if (Feindesliste[i].Status == OUT)
	continue;
      SmallEnemy (Feindesliste[i].pos.x / 4, Feindesliste[i].pos.y / 4,
		  Druidmap[Feindesliste[i].type].class, InternWindow,
		  INTERNBREITE * BLOCKBREITE);
    }

  // Darstellen des Influencers, wenn er nicht schon vernichtet wurde
  if (Me.energy > 0)
    SmallEnemy (((int) Me.pos.x) / 4, ((int) Me.pos.y) / 4,
		-10 + Druidmap[Me.type].class, InternWindow,
		INTERNBREITE * BLOCKBREITE);

  // Darstellen der Blasts
  for (i = 0; i < MAXBLASTS; i++)
    {
      if (AllBlasts[i].type == OUT)
	continue;
      SmallBlast (AllBlasts[i].PX / 4, AllBlasts[i].PY / 4, AllBlasts[i].type,
		  AllBlasts[i].phase, InternWindow,
		  INTERNBREITE * BLOCKBREITE);
    }

  // Darstellen der Bullets
  for (i = 0; i < MAXBULLETS; i++)
    {
      if (AllBullets[i].type == OUT)
	continue;

      if (AllBullets[i].type == FLASH)
	{
	  // Wenn der FLASH vorbei ist, l"oschen und fertig
	  if ( AllBullets[i].time_in_frames > FLASH_DURATION_IN_FRAMES )
	    {
	      AllBullets[i].time_in_frames = 0;
	      AllBullets[i].time_in_seconds = 0;
	      AllBullets[i].type = OUT;
	      AllBullets[i].mine = FALSE;
	      return;
	    }

	  // Das ganze Fenster entweder schwarz oder weiss f"arben
	  Affected = MyMalloc (CurLevel->xlen * CurLevel->ylen + 100);
	  memset (Affected, CurLevel->xlen * CurLevel->ylen, FALSE);

	  if ( (AllBullets[i].time_in_frames % 2) == 1)
	    {
//                                      FlashWindow(0);
	      RecFlashFill (AllBullets[i].pos.x, AllBullets[i].pos.y,
			    FLASHCOLOR1, InternWindow,
			    INTERNBREITE * BLOCKBREITE);
	    }
	  if ( (AllBullets[i].time_in_frames % 2) == 0)
	    {
//                                      FlashWindow(15);
	      RecFlashFill (AllBullets[i].pos.x, AllBullets[i].pos.y,
			    FLASHCOLOR2, InternWindow,
			    INTERNBREITE * BLOCKBREITE);
	    }

	  /*
	   * Alle betroffenen Enemys, die nicht immun sind besch"adigen
	   * Auch den Influencer wenn er nicht immun ist besch"adigen
	   */

	  for (j = 0; j < MAX_ENEMYS_ON_SHIP; j++)
	    {
	      if (Feindesliste[j].levelnum != CurLevel->levelnum)
		continue;
	      if (Affected
		  [((int) rintf (Feindesliste[j].pos.x)) / BLOCKBREITE +
		   ((int) rintf (Feindesliste[j].pos.y)) / BLOCKHOEHE *
		   CurLevel->xlen]
		  && (!Druidmap[((int) rintf (Feindesliste[j].type))].
		      flashimmune))
		{
		  Feindesliste[j].energy -= Bulletmap[FLASH].damage / 2;
		}
	    }

	  if (!InvincibleMode && !Druidmap[Me.type].flashimmune &&
	      Affected[((int) Me.pos.x) / BLOCKBREITE +
		       ((int) Me.pos.y) / BLOCKHOEHE * CurLevel->xlen])
	    Me.energy -= Bulletmap[FLASH].damage / 2;

	  free (Affected);
	}
      else
	{
	  SmallBullet (AllBullets[i].pos.x / 4, AllBullets[i].pos.y / 4,
		       AllBullets[i].type, AllBullets[i].phase, InternWindow,
		       INTERNBREITE * BLOCKBREITE);
	}
    }
  return;
}

/*@Function============================================================
@Desc: 	Diese Funktion malt den Influencer an die Position die das Zentrum
    des angezeigten Bildausschnittes sein wird.

    Parameter : die momentane Phase der Drehung des 001 : Darstellphase

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
PutInfluence (void)
{
  // MODIFIED FOR THE PORT!!!!!!!!!!!!!!!!!

  //
  // ACHTUNG!! FUER DEN PORT HAB ICH DIE HALBE FUNKTION ABGESCHALTET!!!
  // SONST GIBTS NACH CA. 10 BILDERN DEN ERSTEN SEGMENTATION FAULT!!!!
  //

  unsigned char *target;
  unsigned char *source;	/* the druid-block to copy */
  int i, j;
  static int BeamDelay = 1;

  DebugPrintf ("\nvoid PutInfluence(void): REAL function called.");


  source = Influencepointer + ((int) rintf (Me.phase)) * BLOCKMEM;


  // PORT
  // PutObject(Me.pos.x, Me.pos.y, source, FALSE);
  // return;
  // PORT

  BeamLine = 0;

  if (BeamLine > BLOCKBREITE / 2)
    {
      BeamLine--;
      return;
    }

  if (!BeamLine)
    {
      PutObject (Me.pos.x, Me.pos.y, source, FALSE);
    }
  else
    {
      for (i = 0; i < BLOCKHOEHE; i++)
	{
	  // memcpy(target+BLOCKBREITE*INTERNBREITE*i+BeamLine,source+BLOCKHOEHE*i+BeamLine,(BLOCKBREITE/2+1-BeamLine)*2);
	  for (j = 0; j < (BLOCKBREITE / 2 + 1 - BeamLine) * 2; j++)
	    {
	      if (*(source + BLOCKHOEHE * i + BeamLine + j) !=
		  TRANSPARENTCOLOR)
		*(target + BLOCKBREITE * INTERNBREITE * i + BeamLine + j) =
		  *(source + BLOCKHOEHE * i + BeamLine + j);
	    }
	}

      for (i = 0; i < BLOCKHOEHE; i++)
	{
	  *(target + BLOCKBREITE * INTERNBREITE * i + BeamLine - 1) =
	    BULLETCOLOR;
	  *(target + BLOCKBREITE * INTERNBREITE * i + BLOCKBREITE / 2 +
	    (BLOCKBREITE / 2 - BeamLine + 1)) = BULLETCOLOR;
	}
      if (BeamDelay)
	BeamDelay--;
      else
	{
	  BeamLine--;
	  BeamDelay = 1;
	}
    }
  DebugPrintf ("\nvoid PutInfluence(void): REAL function ended.");
}				// void PutInfluence(void)

/*@Function============================================================
@Desc: PutEnemy: setzt Enemy der Nummer Enum ins InternWindow
                 dazu wird dir Function PutObject verwendet
@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void
PutEnemy (int Enum)
{
  unsigned char *Enemypic;
  int enemyX, enemyY;
  const char *druidname;	/* the number-name of the Enemy */
  int phase;

  DebugPrintf
    ("\nvoid PutEnemy(int Enum): real function call confirmed...\n");

  /* if enemy is on other level, return */
  if (Feindesliste[Enum].levelnum != CurLevel->levelnum)
    {
      DebugPrintf
	("\nvoid PutEnemy(int Enum): DIFFERENT LEVEL-->usual end of function reached.\n");
      return;
    }

  /* wenn dieser Feind abgeschossen ist kann sofort zurueckgekehrt werden */
  if (Feindesliste[Enum].Status == OUT)
    {
      DebugPrintf
	("\nvoid PutEnemy(int Enum): STATUS==OUT --> usual end of function reached.\n");
      return;
    }

  /* Wenn Feind nicht sichtbar: weiter */
  if (!IsVisible (&Feindesliste[Enum].pos))
    {
      Feindesliste[Enum].onscreen = FALSE;
      DebugPrintf
	("\nvoid PutEnemy(int Enum): ONSCREEN=FALSE --> usual end of function reached.\n");
      return;

    }
  else
    Feindesliste[Enum].onscreen = TRUE;

  DebugPrintf
    ("\nvoid PutEnemy(int Enum): it seems that we must draw this one on the screen....\n");

  // Den Druidtyp nochmals mit einer Sicherheitsabfrage ueberpruefen:
  if ( Feindesliste[Enum].type >= ALLDRUIDTYPES )
    {
      DebugPrintf("\nvoid PutEnemy(int Enum): ERROR!  IMPOSSIBLE DRUIDTYPE ENCOUNTERED!  EMERGENCY WORKAROUND DONE!!");
      DebugPrintf("\nvoid PutEnemy(int Enum): NOTE THAT THIS PROBLEM REMAINS!  PLEASE FOLLOW THIS BUG AND CORRECT IT!!");
      Feindesliste[Enum].type = 0;
    }

  /* Bild des Feindes mit richtiger Nummer in der richtigen Phase darstellen */
  druidname = Druidmap[Feindesliste[Enum].type].druidname;
  phase = Feindesliste[Enum].feindphase;

  Enemypic = FeindZusammenstellen (druidname, phase);

  enemyX = Feindesliste[Enum].pos.x;
  enemyY = Feindesliste[Enum].pos.y;

  PutObject (enemyX, enemyY, Enemypic, FALSE);

  DebugPrintf ("\nvoid PutEnemy(int Enum): ENEMY HAS BEEN PUT --> usual end of function reached.\n");

}				// void PutEnemy(int Enum) 


/*@Function============================================================
@Desc: PutBullet: setzt das Bullet BulletNummer ins InternWindow
						dazu wird PutObject verwendet
@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void
PutBullet (int BulletNummer)
{
  int i;
  Bullet CurBullet = &AllBullets[BulletNummer];
  unsigned char *bulletpic;

  DebugPrintf
    ("\nvoid PutBullet(int BulletNummer): real function call confirmed.\n");

#if BULLETOFF == 1
  return;
#endif

  bulletpic = Bulletmap[CurBullet->type].picpointer +
    CurBullet->phase * BLOCKMEM;

  /*
   * Wenn ein FLASH gestartet ist, wird einfach der ganze Screen
   * zuerst schwarz, dann weiss geschaltet
   */
  if (CurBullet->type == FLASH)
    {

      /*
       * Wenn der FLASH vorbei ist, l"oschen und fertig
       */

      if ( CurBullet->time_in_frames > FLASH_DURATION_IN_FRAMES )
	{
	  CurBullet->time_in_frames = 0;
	  CurBullet->time_in_seconds = 0;
	  CurBullet->type = OUT;
	  CurBullet->mine = FALSE;
	  return;
	}

      /*
       * Alle sichtbaren enemys, die nicht immun sind besch"adigen
       * Auch den Influencer wenn er nicht immun ist besch"adigen
       */

      for (i = 0; i < MAX_ENEMYS_ON_SHIP; i++)
	{
	  if ((Feindesliste[i].onscreen) &
	      (!Druidmap[Feindesliste[i].type].flashimmune))
	    {
	      Feindesliste[i].energy -= Bulletmap[FLASH].damage / 2;
	    }
	}

      if (!InvincibleMode && !Druidmap[Me.type].flashimmune)
	Me.energy -= Bulletmap[FLASH].damage / 2;


      /*
       * Das ganze Fenster entweder schwarz oder weiss f"arben
       */

      if ( (CurBullet->time_in_frames % 2) == 1)
	{
	  FlashWindow (0);
	  return;
	}
      if ( (CurBullet->time_in_frames % 2) == 0)
	{
	  FlashWindow (15);
	  return;
	}


    }


  if (PutObject (CurBullet->pos.x, CurBullet->pos.y, bulletpic, TRUE) == TRUE)
    {
      /* Bullet-Bullet Collision: Bullet loeschen */
      CurBullet->type = OUT;
      CurBullet->mine = FALSE;

      /* Druid-Blast dort erzeugen: killt zweites Bullet */
      StartBlast (CurBullet->pos.x, CurBullet->pos.y, DRUIDBLAST);
    }				/* if */

  DebugPrintf
    ("\nvoid PutBullet(int BulletNummer): end of function reched.\n");

}				/* PutBullet */

/*@Function============================================================
@Desc:  PutBlast: setzt das Blast BlastNummer ins InternWindow
							dazu wird PutObject verwendet

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void
PutBlast (int BlastNummer)
{
  Blast CurBlast = &AllBlasts[BlastNummer];
  unsigned char *blastpic;

#if BLASTOFF == 1
  return;
#endif

  /* Wenn Blast OUT ist sofort naechsten bearbeiten */
  if (CurBlast->type == OUT)
    return;

  blastpic =
    Blastmap[CurBlast->type].picpointer +
    ((int) rintf (CurBlast->phase)) * BLOCKMEM;

  PutObject (CurBlast->PX, CurBlast->PY, blastpic, FALSE);
}				// void PutBlast(int BlastNummer)

/*@Function============================================================
@Desc: PutObject: Puts object with center-coordinates x/y and the
			imagepointer to the InternWindow

		check: ON: Bullet-Collsion wird gecheckt
		
		can be used for Influencer, Enemys, Bullets and Blasts 

@Ret: int: TRUE/FALSE: BulletCollision 
@Int:
* $Function----------------------------------------------------------*/
int
PutObject (int x, int y, unsigned char *pic, int check)
{
  int DifX, DifY;		/* Verschiebung zum Influencer */
  unsigned int InternWindowOffset;
  unsigned char *source, *target;
  int ret = FALSE;
  int Return_Value;

  DebugPrintf
    ("\nint PutObject(...): real function call confirmed.\n");

  /* Verschiebung zum Influencer (which is the center of screen */
  DifX = x - Me.pos.x;
  DifY = y - Me.pos.y;

  /* Nur Objekte innerhalb des USERFENSTERS anzeigen */
  if ((DifX >= USERFENSTERBREITE / 2) || (DifY >= USERFENSTERHOEHE / 2) ||
      (DifX <= -USERFENSTERBREITE / 2) || (DifY <= -USERFENSTERHOEHE / 2))
    return ret;

  /* Offset des Exakten Bildmittelpunktes abzueglich der Eckenverschiebung */
  InternWindowOffset =
    (INTERNHOEHE) * BLOCKHOEHE / 2 * BLOCKBREITE * INTERNBREITE +
    INTERNBREITE * BLOCKBREITE / 2 - BLOCKBREITE / 2 -
    BLOCKBREITE * INTERNBREITE * BLOCKHOEHE / 2;

  /* Verschiebung zum Influencer  (linkes oberes Eck !!) */
  InternWindowOffset += ((((int) Me.pos.y) % BLOCKHOEHE) - BLOCKHOEHE / 2) *
    BLOCKBREITE * INTERNBREITE +
    (((int) Me.pos.x) % BLOCKBREITE) - BLOCKBREITE / 2;

  /* relative Verschiebung des Objekts zum Influencer */
  InternWindowOffset += DifY * BLOCKBREITE * INTERNBREITE + DifX;

  /* center of screen */
  target = InternWindow + InternWindowOffset;

  /* check memory violations */
  if ((target < InternWindow) ||
      (target >
       InternWindow + INTERNBREITE * INTERNHOEHE * BLOCKMEM - BLOCKMEM))
    {
      gotoxy (1, 1);
      DebugPrintf ("Memory violation by PutObject !!!");
      getchar ();
      return FALSE;
    }

  source = pic;

  DebugPrintf
    ("\nint PutObject(...): usual end of function reached.\n");

  Return_Value=(MergeBlockToWindow
	  (source, target, INTERNBREITE * BLOCKBREITE, check));

  DebugPrintf
    ("\nint PutObject(...): usual end of function reached.\n");

  return (Return_Value);
} // int PutObject(...)


/*@Function============================================================
@Desc: 	Diese Prozedur schreibt das im Speicher zusammengebaute Bild
			in den Bildschirmspeicher.

		   Parameter: keine
@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
PutInternFenster (void)
{
  int StartX, StartY;
  int i;
#ifdef SLOW_VIDEO_CALLS
  int j;
#endif

  unsigned char *source;

  DebugPrintf ("void PutInternFenster(void) wurde ECHT aufgerufen...");

  if (Conceptview)
    {
      for (i = 0; i < USERFENSTERHOEHE; i++)
	{
	  memcpy (RealScreen + (USERFENSTERPOSY + i) * SCREENBREITE +
		  USERFENSTERPOSX,
		  InternWindow + i * INTERNBREITE * BLOCKBREITE,
		  USERFENSTERBREITE);
	}
      return;
    }

  StartX = (((int) Me.pos.x) % BLOCKBREITE) - BLOCKBREITE / 2;
  StartY =
    ((((int) Me.pos.y) % BLOCKHOEHE) -
     BLOCKHOEHE / 2) * BLOCKBREITE * INTERNBREITE;

  WaitVRetrace ();		//

  DisplayRahmen ( RealScreen );

  for (i = 0; i < USERFENSTERHOEHE; i++)
    {
      source = InternWindow +
	BLOCKBREITE * (INTERNBREITE - VIEWBREITE) / 2 +
	INTERNBREITE * BLOCKBREITE * (BLOCKHOEHE *
				      (INTERNHOEHE - VIEWHOEHE)) / 2 +
	// USERFENSTEROBEN*INTERNBREITE*BLOCKBREITE + 
	//       USERFENSTERLINKS +
	StartY + StartX + i * INTERNBREITE * BLOCKBREITE;
#ifdef SLOW_VIDEO_CALLS
      for (j = 0; j < USERFENSTERBREITE; j++)
	{
	  vga_setcolor (*source);
	  source++;
	  vga_drawpixel (USERFENSTERPOSX + j, USERFENSTERPOSY + i);
	}			// for(j=0; ...
#else
      vga_drawscansegment (source, USERFENSTERPOSX, USERFENSTERPOSY + i,
			   USERFENSTERBREITE);
      // source+=USERFENSTERBREITE;
#endif
    }				// for(i=0; ...
};				// void PutInternFenster(void)

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
RedrawInfluenceNumber (void)
{
  unsigned char *LSource;
  unsigned char *LDest;
  int LPhase = 0;
  unsigned char LDigit = 0;
  int j;

  for (LPhase = 0; LPhase < ENEMYPHASES; LPhase++)
    {
      for (j = 0; j < 3; j++)
	{
	  LDigit = *(Druidmap[Me.type].druidname + j) - '0';
	  LSource = Digitpointer + LDigit * 9 * 9;
	  LDest =
	    Influencepointer + LPhase * BLOCKBREITE * BLOCKHOEHE + j * 8 +
	    NUMBEROFS;
	  DrawDigit (LSource, LDest);
	}			// for
    }				// for
}				// void RedrawInfluenceNumber(void)


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
DrawDigit (unsigned char *Src, unsigned char *Dst)
{
  int i;
  int j;

  DebugPrintf ("\nvoid DrawDigit(...): real function call confirmed.");

  for (i = 0; i < DIGITHEIGHT; i++)
    {
      for (j = 0; j < DIGITLENGTH-1; j++)
	{
	  *(Dst + i * BLOCKBREITE + j) = *(Src + i * 9 + j);
	}
    }

  DebugPrintf ("\nvoid DrawDigit(...): end of usual function reached.");

} // void DrawDigit(,..)


/*@Function============================================================
@Desc: Diese Funktion setzt die Schu"sfarbe um einen Wert weiter

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
RotateBulletColor (void)
{
  static int BulColNum;

  static color BulletColors[MAXBULCOL] =
    { BULLETCOLOR1, BULLETCOLOR2, BULLETCOLOR3, BULLETCOLOR4, BULLETCOLOR5 };
  BulColNum++;
  BulColNum = MyRandom (MAXBULCOL);

  if (BulColNum > (MAXBULCOL - 1))
    BulColNum = 0;

  SetPalCol (BULLETCOLOR, BulletColors[BulColNum].rot,
	     BulletColors[BulColNum].gruen, BulletColors[BulColNum].blau);
}				// void RotateBulletColor(void)


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
FlashWindow (int Flashcolor)
{
  /*
   * das ganze Fenster kann durch einen einzigen Aufruf vom memset
   * auf den gew"unschten Wert gesetzt werden
   */
  memset (InternWindow, Flashcolor,
	  INTERNBREITE * INTERNHOEHE * BLOCKBREITE * BLOCKHOEHE);
}				// void FlashWindow(int Flashcolor)

/*-----------------------------------------------------------------
 * @Desc: Setzt die Hintergrundfarbe fuer das Userfenster using glvga
 * @Ret: void
 *
 *-----------------------------------------------------------------*/
void
SetUserfenster (int color, unsigned char *screen)
{
  int row;
  int x0 = USERFENSTERPOSX;
  int x1 = x0 + USERFENSTERBREITE;
  int y0 = USERFENSTERPOSY;
  int y1 = y0 + USERFENSTERHOEHE;

  if (screen == RealScreen)
    {
      for (row = y0; row < y1; row++)
	gl_hline (x0, row, x1, color);
    }
  else
    {
      for (row = 0; row < USERFENSTERHOEHE; row++)
	memset (screen + USERFENSTERPOSX +
		(USERFENSTERPOSY + row) * SCREENBREITE,
		color, USERFENSTERBREITE);
    }
  return;
}				/* SetUserFenster */

/* **********************************************************************
   Diese Funktion zeigt einen Robotter an
   Dazu mu"s zuerst InitRobotPictures einmal aufgerufen worden sein.
   Wenn InitRobotPictures auf effizientere Methoden umsteigt, so mu"s
   diese Funktion nat"urlich auch entsprechend angepa"st werden.
**********************************************************************/

#define ROBOTBILDHOEHE SCREENHOEHE/3
#define ROBOTBILDBREITE SCREENBREITE/8

void
ShowRobotPicture (int PosX, int PosY, int Number, unsigned char *Screen)
{

  DebugPrintf ("\nvoid ShowRobotPicture(...): Function call confirmed.");

  // Is the following line a reason for segfaults?  I turn is of for test purposes
  // gl_putbox(PosX, PosY, DRUIDIMAGE_LENGTH, DRUIDIMAGE_HEIGHT, Druidmap[Number].image );
  DisplayBlock (PosX, PosY, Druidmap[Number].image, DRUIDIMAGE_LENGTH,
		DRUIDIMAGE_HEIGHT, Screen);

  DebugPrintf
    ("\nvoid ShowRobotPicture(...): Usual end of function reached.");

}				// void ShowRobotPicture(...)

#undef _view_c
