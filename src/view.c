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
#define _view_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "map.h"
#include "proto.h"
#include "colodefs.h"

#include "SDL_rotozoom.h"


/* locale Schalter zu DEBUG-Zwecken */
#define INFLUENCEOFF  	0
#define BULLETOFF			0
#define BLASTOFF			0

//#define SHOWSTATS



void GetConceptInternFenster (void);
void FlashWindow (int Flashcolor);
void RecFlashFill (int LX, int LY, int Color, unsigned char *Parameter_Screen,
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
RecFlashFill (int LX, int LY, int Color, unsigned char *Parameter_Screen, int SBreite)
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
      memset (Parameter_Screen + i * SBreite + LX / 4 - ((LX / 4) % 8), Color, 8);
    }
  i -= 4;

  // Feld rechts davon anf"ullen
  if ((*(Parameter_Screen + i * SBreite + LX / 4 + 8) != Color) &&
      (IsPassable (Cent (LX + BLOCKBREITE), Cent (LY), CENTER) == CENTER))
    RecFlashFill (LX + BLOCKBREITE, LY, Color, Parameter_Screen, SBreite);

  // Feld links davon anf"ullen
  if (LX > BLOCKBREITE)
    {
      if ((*(Parameter_Screen + i * SBreite + LX / 4 - 8) != Color) &&
	  (IsPassable (Cent (LX - BLOCKBREITE), Cent (LY), CENTER) == CENTER))
	RecFlashFill (LX - BLOCKBREITE, LY, Color, Parameter_Screen, SBreite);
    }

  // Feld oben davon anf"ullen
  if ((i > 8) && (LY > BLOCKHOEHE))
    {
      if ((*(Parameter_Screen + (i - 8) * SBreite + LX / 4) != Color) &&
	  (IsPassable (Cent (LX), Cent (LY - BLOCKHOEHE), CENTER) == CENTER))
	RecFlashFill (LX, LY - BLOCKHOEHE, Color, Parameter_Screen, SBreite);
    }

  // Feld unten davon anf"ullen
  if ((*(Parameter_Screen + (i + 8) * SBreite + LX / 4) != Color) &&
      (IsPassable (Cent (LX), Cent (LY + BLOCKHOEHE), CENTER) == CENTER))
    RecFlashFill (LX, LY + BLOCKHOEHE, Color, Parameter_Screen, SBreite);
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
Assemble_Combat_Picture (int mask)
{
  Blast CurBlast = &(AllBlasts[0]);
  int MapBrick;
  int line, col;
  int i, j;
  SDL_Rect TargetRectangle;
  SDL_Rect CombatRectangle;

  DebugPrintf ("\nvoid Assemble_Combat_Picture(...): Real function call confirmed.");

  CombatRectangle.x=USERFENSTERPOSX;
  CombatRectangle.y=USERFENSTERPOSY;
  CombatRectangle.w=USERFENSTERBREITE;
  CombatRectangle.h=USERFENSTERHOEHE;
  

  if (Conceptview)
    {
      GetConceptInternFenster ();
      return;
    }

  SDL_SetClipRect( ne_screen , &CombatRectangle );

  // Why not clip the WHOLE map?  Lets try it!
  // --> It seems to work!!  THAT IS A VERY POWERFUL AND VERY ABSTRACT PROCEDURE:
  // * INTERNBREITE IS NO LONGER NEEDED IN HERE
  // * INTERNHOEHE IS NO LONGER NEEDED IN HERE
  // * THE COMBATSCREENSIZE COULD *EASYLY* BE CHANGED WITHOUT HAVING TO CHANGE THE CODE!!!
  // 

  for (line = 0; line < CurLevel->ylen ; line++)
    {
      for (col = 0; col < CurLevel->xlen ; col++)
	{
	  if ((MapBrick = CurLevel->map[line][col]) != INVISIBLE_BRICK)
	    {
	      TargetRectangle.x=USER_FENSTER_CENTER_X-Me.pos.x+col*BLOCKBREITE;
	      TargetRectangle.y=USER_FENSTER_CENTER_Y-Me.pos.y+line*BLOCKHOEHE;
	      SDL_BlitSurface(ne_blocks, ne_map_block+MapBrick, ne_screen, &TargetRectangle);
	    }			// if !INVISIBLE_BRICK 
	}			// for(col) 
    }				// for(line) 

  if (mask == SHOW_MAP)		// we want only the map, nothing else 
    return;


  for (i = 0; i < NumEnemys; i++)
    PutEnemy (i);

  if (Me.energy > 0)
    PutInfluence ( -1 , -1 );

  for (i = 0; i < (MAXBULLETS); i++)
    if (AllBullets[i].type != OUT)
      PutBullet (i);

  for (i = 0; i < (MAXBLASTS); i++)
    if (AllBlasts[i].type != OUT)
      PutBlast (i);

  //#define INFRAME_SCALING
   #undef INFRAME_SCALING 
#ifdef INFRAME_SCALING

  ne_scaled_screen = zoomSurface ( ne_screen , 2 , 2 , 0);

  SDL_SetClipRect( ne_screen, NULL);

  SDL_BlitSurface( ne_scaled_screen , NULL , ne_screen , NULL );

  SDL_Flip ( ne_screen );

  SDL_FreeSurface( ne_scaled_screen );

#else

  SDL_Flip( ne_screen );

#endif

  return;  // for now


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

  DebugPrintf ("\nvoid Assemble_Combat_Picture(...): end of function reached.");

  return;

} // void Assemble_Combat_Picture(...)

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

  char* OutputPointer=InternWindow;

  // Darstellen der blo"sen Deckkarte 
  for (i = 0; i < CurLevel->ylen; i++)
    {
      for (j = 0; j < CurLevel->xlen; j++)
	{
	  SmallBlock (LX, LY,
		      GetMapBrick (CurLevel, j * BLOCKBREITE, i * BLOCKHOEHE),
		      OutputPointer, INTERNBREITE * BLOCKBREITE);
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
      if ( !IsVisible (&Feindesliste[i].pos) )
	continue;
      SmallEnemy (Feindesliste[i].pos.x / 4, Feindesliste[i].pos.y / 4,
		  Druidmap[Feindesliste[i].type].class, OutputPointer,
		  INTERNBREITE * BLOCKBREITE);
    }

  // Darstellen des Influencers, wenn er nicht schon vernichtet wurde
  if (Me.energy > 0)
    SmallEnemy (((int) Me.pos.x) / 4, ((int) Me.pos.y) / 4,
		-10 + Druidmap[Me.type].class, OutputPointer,
		INTERNBREITE * BLOCKBREITE);

  // Darstellen der Blasts
  for (i = 0; i < MAXBLASTS; i++)
    {
      if (AllBlasts[i].type == OUT)
	continue;
      SmallBlast (AllBlasts[i].PX / 4, AllBlasts[i].PY / 4, AllBlasts[i].type,
		  AllBlasts[i].phase, OutputPointer,
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
	  Affected = MyMalloc ((CurLevel->xlen+100) * (CurLevel->ylen + 100));
	  memset (Affected, CurLevel->xlen * CurLevel->ylen, FALSE);

	  if ( (AllBullets[i].time_in_frames % 2) == 1)
	    {
	      RecFlashFill (AllBullets[i].pos.x, AllBullets[i].pos.y,
			    FLASHCOLOR1, OutputPointer,
			    INTERNBREITE * BLOCKBREITE);
	    }
	  if ( (AllBullets[i].time_in_frames % 2) == 0)
	    {
	      RecFlashFill (AllBullets[i].pos.x, AllBullets[i].pos.y,
			    FLASHCOLOR2, OutputPointer,
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
		  [(int)(((int) (rintf (Feindesliste[j].pos.x)) / BLOCKBREITE) +
		   ((int) (rintf (Feindesliste[j].pos.y)) / BLOCKHOEHE) *
		   CurLevel->xlen )]
		  && (!Druidmap[ Feindesliste[j].type ].flashimmune))
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
		       AllBullets[i].type, AllBullets[i].phase, OutputPointer,
		       INTERNBREITE * BLOCKBREITE);
	}
    }
  return;

} // GetConceptInternWindow()

/*-----------------------------------------------------------------
 * Desc: Diese Funktion malt den Influencer an die Position die
 *    das Zentrum des angezeigten Bildausschnittes sein wird.
 * 
 * Param: die momentane Phase der Drehung des 001 : Darstellphase
 *
 *-----------------------------------------------------------------*/

void
PutInfluence ( int x, int y)
{
  SDL_Rect TargetRectangle;

  DebugPrintf ("\nvoid PutInfluence(void): REAL function called.");

  if ( x == -1 ) 
    {
      TargetRectangle.x=USER_FENSTER_CENTER_X - BLOCKBREITE/2;
      TargetRectangle.y=USER_FENSTER_CENTER_Y - BLOCKHOEHE/2;
    }
  else
    {
      TargetRectangle.x=x;
      TargetRectangle.y=y;
    }

  SDL_BlitSurface( ne_blocks , ne_influ_block+((int) rintf (Me.phase)), ne_screen, &TargetRectangle );

  // SDL_BlitSurface( ne_blocks , ne_digit_block+(Druidmap[Me.type].druidname[0]-'1') , ne_screen, &TargetRectangle );
  TargetRectangle.x += DIGIT_POS_X;
  TargetRectangle.y += DIGIT_POS_Y;
  SDL_BlitSurface( ne_blocks , ne_digit_block + (Druidmap[Me.type].druidname[0]-'1'+1) , ne_screen, &TargetRectangle );
  TargetRectangle.x += DIGITLENGTH;
  SDL_BlitSurface( ne_blocks , ne_digit_block + (Druidmap[Me.type].druidname[1]-'1'+1) , ne_screen, &TargetRectangle );
  TargetRectangle.x += DIGITLENGTH;
  SDL_BlitSurface( ne_blocks , ne_digit_block + (Druidmap[Me.type].druidname[2]-'1'+1) , ne_screen, &TargetRectangle );

  DebugPrintf ("\nvoid PutInfluence(void): REAL function ended.");

} /* PutInfluence() */


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
  SDL_Rect TargetRectangle;

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
      DebugPrintf
	("\nvoid PutEnemy(int Enum): ONSCREEN=FALSE --> usual end of function reached.\n");
      return;
    }

  DebugPrintf
    ("\nvoid PutEnemy(int Enum): it seems that we must draw this one on the screen....\n");

  // Den Druidtyp nochmals mit einer Sicherheitsabfrage ueberpruefen:
  if ( Feindesliste[Enum].type >= ALLDRUIDTYPES )
    {
      DebugPrintf("\nvoid PutEnemy(int Enum): ERROR!  IMPOSSIBLE DRUIDTYPE ENCOUNTERED!  EMERGENCY WORKAROUND DONE!!");
      DebugPrintf("\nvoid PutEnemy(int Enum): NOTE THAT THIS PROBLEM REMAINS!  PLEASE FOLLOW THIS BUG AND CORRECT IT!!");
      Feindesliste[Enum].type = 0;
      Terminate(ERR);
    }

  // First blit just the enemy hat and shoes.
  // The number will be blittet later

  druidname = Druidmap[Feindesliste[Enum].type].druidname;
  phase = Feindesliste[Enum].feindphase;

  TargetRectangle.x=USER_FENSTER_CENTER_X-Me.pos.x+Feindesliste[Enum].pos.x-BLOCKBREITE/2;
  TargetRectangle.y=USER_FENSTER_CENTER_Y-Me.pos.y+Feindesliste[Enum].pos.y-BLOCKHOEHE/2;
  // TargetRectangle.w=BLOCKBREITE;
  // TargetRectangle.h=BLOCKHOEHE;

  SDL_BlitSurface(ne_blocks , ne_droid_block+phase, ne_screen, &TargetRectangle);

  // Now the numbers should be blittet.

  TargetRectangle.x=USER_FENSTER_CENTER_X-Me.pos.x+Feindesliste[Enum].pos.x-BLOCKBREITE/2 + DIGIT_POS_X;
  TargetRectangle.y=USER_FENSTER_CENTER_Y-Me.pos.y+Feindesliste[Enum].pos.y-BLOCKHOEHE/2 + DIGIT_POS_Y;
  SDL_BlitSurface( ne_blocks , ne_digit_block + (Druidmap[Feindesliste[Enum].type].druidname[0]-'1'+11) , 
		   ne_screen, &TargetRectangle );
  TargetRectangle.x=USER_FENSTER_CENTER_X-Me.pos.x+Feindesliste[Enum].pos.x-BLOCKBREITE/2 + DIGIT_POS_X + DIGITLENGTH-1;
  TargetRectangle.y=USER_FENSTER_CENTER_Y-Me.pos.y+Feindesliste[Enum].pos.y-BLOCKHOEHE/2 + DIGIT_POS_Y;
  SDL_BlitSurface( ne_blocks , ne_digit_block + (Druidmap[Feindesliste[Enum].type].druidname[1]-'1'+11) , 
		   ne_screen, &TargetRectangle );
  TargetRectangle.x=USER_FENSTER_CENTER_X-Me.pos.x+Feindesliste[Enum].pos.x-BLOCKBREITE/2 + DIGIT_POS_X + 2*(DIGITLENGTH-1);
  TargetRectangle.y=USER_FENSTER_CENTER_Y-Me.pos.y+Feindesliste[Enum].pos.y-BLOCKHOEHE/2 + DIGIT_POS_Y;
  SDL_BlitSurface( ne_blocks , ne_digit_block + (Druidmap[Feindesliste[Enum].type].druidname[2]-'1'+11) , 
		   ne_screen, &TargetRectangle );

  DebugPrintf ("\nvoid PutEnemy(int Enum): ENEMY HAS BEEN PUT --> usual end of function reached.\n");

}	// void PutEnemy(int Enum) 

/*@Function============================================================
@Desc: PutBullet: setzt das Bullet BulletNummer ins InternWindow
						dazu wird PutObject verwendet
@Ret: void
@Int:
* $Function----------------------------------------------------------*/

void
PutBullet (int BulletNummer)
{
  Bullet CurBullet = &AllBullets[BulletNummer];
  unsigned char *bulletpic;
  SDL_Rect TargetRectangle;

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
      // Now the whole window will be filled with either white
      // or black each frame until the flash is over.  (Flash 
      // deletion after some time is done in CheckBulletCollisions.)
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
    } // if type == FLASH


  /*
  if (PutObject (CurBullet->pos.x, CurBullet->pos.y, bulletpic, TRUE) == TRUE)
    {
      // Bullet-Bullet Collision: Bullet loeschen 
      CurBullet->type = OUT;
      CurBullet->mine = FALSE;

      // Druid-Blast dort erzeugen: killt zweites Bullet 
      StartBlast (CurBullet->pos.x, CurBullet->pos.y, DRUIDBLAST);
    }	// if 
  */

  TargetRectangle.x=USER_FENSTER_CENTER_X-Me.pos.x+CurBullet->pos.x-BLOCKBREITE/2;
  TargetRectangle.y=USER_FENSTER_CENTER_Y-Me.pos.y+CurBullet->pos.y-BLOCKHOEHE/2;

  SDL_BlitSurface( ne_blocks , Bulletmap[CurBullet->type].block + CurBullet->phase, ne_screen , &TargetRectangle );

  DebugPrintf
    ("\nvoid PutBullet(int BulletNummer): end of function reched.\n");

}	/* PutBullet */

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
  SDL_Rect TargetRectangle;

#if BLASTOFF == 1
  return;
#endif

  /* Wenn Blast OUT ist sofort naechsten bearbeiten */
  if (CurBlast->type == OUT)
    return;

  TargetRectangle.x=USER_FENSTER_CENTER_X - Me.pos.x + CurBlast->PX -BLOCKBREITE/2;
  TargetRectangle.y=USER_FENSTER_CENTER_Y - Me.pos.y + CurBlast->PY -BLOCKHOEHE/2;
  SDL_BlitSurface( ne_blocks, Blastmap[CurBlast->type].block + ((int) rintf(CurBlast->phase)), ne_screen , &TargetRectangle);


  return;

  blastpic =
    Blastmap[CurBlast->type].picpointer +
    ((int) rintf (CurBlast->phase)) * BLOCKMEM;

  PutObject (CurBlast->PX, CurBlast->PY, blastpic, FALSE);
}  // void PutBlast(int BlastNummer)


/*-----------------------------------------------------------------
 * @Desc: PutObject: Puts object with center-coordinates x/y and the
 *	imagepointer to the InternWindow
 *
 * check: ON: Bullet-Collsion wird gecheckt
 *	can be used for Influencer, Enemys, Bullets and Blasts 
 *
 * @Ret: int: TRUE/FALSE: BulletCollision 
 *
 *-----------------------------------------------------------------*/
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
      DebugPrintf ("Memory violation by PutObject !!!");
      Terminate(-1);
    }

  source = pic;

  DebugPrintf
    ("\nint PutObject(...): usual end of function reached.\n");

  Return_Value=(MergeBlockToWindow
	  (source, target, INTERNBREITE * BLOCKBREITE, check));

  DebugPrintf
    ("\nint PutObject(...): usual end of function reached.\n");

  return (Return_Value);
} /* PutObject() */


/*-----------------------------------------------------------------
 * @Desc: Diese Prozedur schreibt das im Speicher zusammengebaute
 * 	Bild in den Bildschirmspeicher.
 *
 * 
 *-----------------------------------------------------------------*/
void
PutInternFenster (int also_update_scaled_surface)
{
  int StartX, StartY;
  int i;
  unsigned char *source;
  unsigned char *target;

  return;

  /*
    In case the Conceptview switch is set, only a small map is drawn, like in 
    the console when you request the map of the entire deck.
  */
  if (Conceptview)
    {
      for (i = 0; i < USERFENSTERHOEHE; i++)
	{
	  memcpy (Outline320x200 + (USERFENSTERPOSY + i) * SCREENBREITE +
		  USERFENSTERPOSX,
		  InternWindow + i * INTERNBREITE * BLOCKBREITE,
		  USERFENSTERBREITE);
	}
      
      PrepareScaledSurface(also_update_scaled_surface);

      return;
    }


  StartX = (((int) Me.pos.x) % BLOCKBREITE) - BLOCKBREITE / 2;
  StartY =
    ((((int) Me.pos.y) % BLOCKHOEHE) -
     BLOCKHOEHE / 2) * BLOCKBREITE * INTERNBREITE;

  DisplayRahmen ( Outline320x200 );
  SetInfoline (NULL, NULL);

  for (i = 0; i < USERFENSTERHOEHE; i++)
    {
      source = InternWindow +
	BLOCKBREITE * (INTERNBREITE - VIEWBREITE) / 2 +
	INTERNBREITE * BLOCKBREITE * (BLOCKHOEHE *
				      (INTERNHOEHE - VIEWHOEHE)) / 2 +
	// USERFENSTEROBEN*INTERNBREITE*BLOCKBREITE + 
	//       USERFENSTERLINKS +
	StartY + StartX + i * INTERNBREITE * BLOCKBREITE;
      target = Outline320x200 + USERFENSTERPOSX + (USERFENSTERPOSY+i) * SCREENBREITE;

      memcpy(target, source, USERFENSTERBREITE);

    }	// for(i=0; ...


  PrepareScaledSurface(also_update_scaled_surface);

  return;

}; // void PutInternFenster(void)

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

  return;

  BulColNum++;
  BulColNum = MyRandom (MAXBULCOL);

  if (BulColNum > (MAXBULCOL - 1))
    BulColNum = 0;

  SetPalCol (BULLETCOLOR, BulletColors[BulColNum].rot,
	     BulletColors[BulColNum].gruen, BulletColors[BulColNum].blau);
} /* void RotateBulletColor(void) */


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
 * @Desc: Setzt die Hintergrundfarbe fuer das Userfenster using SDLx
 * @Ret: void
 *
 *-----------------------------------------------------------------*/
void
SetUserfenster (int color, unsigned char *Parameter_screen)
{


  int row;
  SDL_Rect LocalRectangle;

  return;

}				/* SetUserFenster() */

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
