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
  Val = Val - (Val % Block_Width) + Block_Width / 2;
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
  Affected[LY / Block_Height * CurLevel->xlen + LX / Block_Width] = TRUE;

  // Dieses Feld anf"ullen
  for (i = LY / 4 - ((LY / 4) % 8); i < (LY / 4 - ((LY / 4) % 8) + 8); i++)
    {
      memset (Parameter_Screen + i * SBreite + LX / 4 - ((LX / 4) % 8), Color, 8);
    }
  i -= 4;

  // Feld rechts davon anf"ullen
  if ((*(Parameter_Screen + i * SBreite + LX / 4 + 8) != Color) &&
      (IsPassable (Cent (LX + Block_Width), Cent (LY), CENTER) == CENTER))
    RecFlashFill (LX + Block_Width, LY, Color, Parameter_Screen, SBreite);

  // Feld links davon anf"ullen
  if (LX > Block_Width)
    {
      if ((*(Parameter_Screen + i * SBreite + LX / 4 - 8) != Color) &&
	  (IsPassable (Cent (LX - Block_Width), Cent (LY), CENTER) == CENTER))
	RecFlashFill (LX - Block_Width, LY, Color, Parameter_Screen, SBreite);
    }

  // Feld oben davon anf"ullen
  if ((i > 8) && (LY > Block_Height))
    {
      if ((*(Parameter_Screen + (i - 8) * SBreite + LX / 4) != Color) &&
	  (IsPassable (Cent (LX), Cent (LY - Block_Height), CENTER) == CENTER))
	RecFlashFill (LX, LY - Block_Height, Color, Parameter_Screen, SBreite);
    }

  // Feld unten davon anf"ullen
  if ((*(Parameter_Screen + (i + 8) * SBreite + LX / 4) != Color) &&
      (IsPassable (Cent (LX), Cent (LY + Block_Height), CENTER) == CENTER))
    RecFlashFill (LX, LY + Block_Height, Color, Parameter_Screen, SBreite);
}

/*
-----------------------------------------------------------------
@Desc: This function assembles the contents of the combat window 
       in ne_screen.

       Several FLAGS can be used to control its behaviour:

       (*) ONLY_SHOW_MAP = 1:  This flag indicates not do draw any
           game elements but the map blocks

       (*) DO_SCREEN_UPDATE = 2: This flag indicates for the function
           to also cause an SDL_Update of the portion of the screen
           that has been modified

 @Ret: none
-----------------------------------------------------------------
*/

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

  // Why not blit the WHOLE map?  Lets try it!
  // THAT IS A VERY POWERFUL AND VERY ABSTRACT PROCEDURE:
  // * INTERNBREITE IS NO LONGER NEEDED IN HERE
  // * INTERNHOEHE IS NO LONGER NEEDED IN HERE
  // * THE COMBATSCREENSIZE COULD *EASYLY* BE CHANGED WITHOUT HAVING TO CHANGE THE CODE!!!
  // 

  SDL_SetClipRect( ne_screen , &CombatRectangle );

  for (line = 0; line < CurLevel->ylen ; line++)
    {
      for (col = 0; col < CurLevel->xlen ; col++)
	{
	  if ((MapBrick = CurLevel->map[line][col]) != INVISIBLE_BRICK)
	    {
	      TargetRectangle.x=USER_FENSTER_CENTER_X + ( -Me.pos.x+col-0.5 )   *Block_Width;
	      TargetRectangle.y=USER_FENSTER_CENTER_Y + ( -Me.pos.y+line-0.5 ) *Block_Height;
	      SDL_BlitSurface(ne_blocks, ne_map_block+MapBrick, ne_screen, &TargetRectangle);
	    }			// if !INVISIBLE_BRICK 
	}			// for(col) 
    }				// for(line) 


  if (mask & ONLY_SHOW_MAP) 
    {
      // in case we only draw the map, we are done here.  But
      // of course we must check if we should update the screen too.
      if ( mask & DO_SCREEN_UPDATE ) 
	SDL_UpdateRect( ne_screen , CombatRectangle.x , CombatRectangle.y , CombatRectangle.w , CombatRectangle.h );
      return;
    }

  // At this point we know that now only the map is to be drawn.
  // so we start drawing the rest of the INTERIOR of the combat window:

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

  // At this point we are done with the drawing procedure
  // and all that remains to be done is updating the screen.
  // Depending on where we did our modifications, we update
  // an according portion of the screen.

  if ( mask & DO_SCREEN_UPDATE )
    {
      SDL_UpdateRect( ne_screen , CombatRectangle.x , CombatRectangle.y , CombatRectangle.w , CombatRectangle.h );
    }

  return;  // for now


  // DIRTY CODE:  CHECKING COLLISIONS WITHIN THE
  // DRAWING ROUTINE:  THAT REMAINS HERE JUST FOR
  // RECYCLING PURPOSES

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
		      GetMapBrick (CurLevel, j * Block_Width, i * Block_Height),
		      OutputPointer, INTERNBREITE * Block_Width);
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
		  INTERNBREITE * Block_Width);
    }

  // Darstellen des Influencers, wenn er nicht schon vernichtet wurde
  if (Me.energy > 0)
    SmallEnemy (((int) Me.pos.x) / 4, ((int) Me.pos.y) / 4,
		-10 + Druidmap[Me.type].class, OutputPointer,
		INTERNBREITE * Block_Width);

  // Darstellen der Blasts
  for (i = 0; i < MAXBLASTS; i++)
    {
      if (AllBlasts[i].type == OUT)
	continue;
      SmallBlast (AllBlasts[i].PX / 4, AllBlasts[i].PY / 4, AllBlasts[i].type,
		  AllBlasts[i].phase, OutputPointer,
		  INTERNBREITE * Block_Width);
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
			    INTERNBREITE * Block_Width);
	    }
	  if ( (AllBullets[i].time_in_frames % 2) == 0)
	    {
	      RecFlashFill (AllBullets[i].pos.x, AllBullets[i].pos.y,
			    FLASHCOLOR2, OutputPointer,
			    INTERNBREITE * Block_Width);
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
		  [(int)(((int) (rintf (Feindesliste[j].pos.x)) / Block_Width) +
		   ((int) (rintf (Feindesliste[j].pos.y)) / Block_Height) *
		   CurLevel->xlen )]
		  && (!Druidmap[ Feindesliste[j].type ].flashimmune))
		{
		  Feindesliste[j].energy -= Bulletmap[FLASH].damage / 2;
		}
	    }

	  if (!InvincibleMode && !Druidmap[Me.type].flashimmune &&
	      Affected[((int) Me.pos.x) / Block_Width +
		       ((int) Me.pos.y) / Block_Height * CurLevel->xlen])
	    Me.energy -= Bulletmap[FLASH].damage / 2;

	  free (Affected);
	}
      else
	{
	  SmallBullet (AllBullets[i].pos.x / 4, AllBullets[i].pos.y / 4,
		       AllBullets[i].type, AllBullets[i].phase, OutputPointer,
		       INTERNBREITE * Block_Width);
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
      TargetRectangle.x=USER_FENSTER_CENTER_X - Block_Width/2;
      TargetRectangle.y=USER_FENSTER_CENTER_Y - Block_Height/2;
    }
  else
    {
      TargetRectangle.x=x;
      TargetRectangle.y=y;
    }

  // Now we draw the hat and shoes of the influencer
  SDL_BlitSurface( ne_blocks , ne_influ_block+((int) rintf (Me.phase)), ne_screen, &TargetRectangle );

  // Now we draw the first digit of the influencers current number.
  // SDL SOMETIMES MODIFIES THE TARGET ENTRY, THEREFORE IT HAS TO BE 
  // COMPUTED ANEW!!!!
  if ( x == -1 ) 
    {
      TargetRectangle.x=USER_FENSTER_CENTER_X - Block_Width/2 + DIGIT_POS_X;
      TargetRectangle.y=USER_FENSTER_CENTER_Y - Block_Height/2 + DIGIT_POS_Y;
    }
  else
    {
      TargetRectangle.x=x + DIGIT_POS_X;
      TargetRectangle.y=y + DIGIT_POS_Y;
    }
  SDL_BlitSurface( ne_blocks , ne_digit_block + (Druidmap[Me.type].druidname[0]-'1'+1) , ne_screen, &TargetRectangle );

  // Now we draw the second digit of the influencers current number.
  // SDL SOMETIMES MODIFIES THE TARGET ENTRY, THEREFORE IT HAS TO BE 
  // COMPUTED ANEW!!!!
  if ( x == -1 ) 
    {
      TargetRectangle.x=USER_FENSTER_CENTER_X - Block_Width/2 + DIGIT_POS_X + DIGITLENGTH;
      TargetRectangle.y=USER_FENSTER_CENTER_Y - Block_Height/2 + DIGIT_POS_Y;
    }
  else
    {
      TargetRectangle.x=x + DIGIT_POS_X + DIGITLENGTH;
      TargetRectangle.y=y + DIGIT_POS_Y;
    }
  SDL_BlitSurface( ne_blocks , ne_digit_block + (Druidmap[Me.type].druidname[1]-'1'+1) , ne_screen, &TargetRectangle );

  // Now we draw the third digit of the influencers current number.
  // SDL SOMETIMES MODIFIES THE TARGET ENTRY, THEREFORE IT HAS TO BE 
  // COMPUTED ANEW!!!!
  if ( x == -1 ) 
    {
      TargetRectangle.x=USER_FENSTER_CENTER_X - Block_Width/2 + DIGIT_POS_X + 2*DIGITLENGTH;
      TargetRectangle.y=USER_FENSTER_CENTER_Y - Block_Height/2 + DIGIT_POS_Y;
    }
  else
    {
      TargetRectangle.x=x + DIGIT_POS_X + 2*DIGITLENGTH;
      TargetRectangle.y=y + DIGIT_POS_Y;
    }
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

  TargetRectangle.x=USER_FENSTER_CENTER_X+ 
    ( (-Me.pos.x+Feindesliste[Enum].pos.x ) ) * Block_Width  -Block_Width/2;
  TargetRectangle.y=USER_FENSTER_CENTER_Y+ 
    ( (-Me.pos.y+Feindesliste[Enum].pos.y ) ) * Block_Height -Block_Height/2;
  // TargetRectangle.w=Block_Width;
  // TargetRectangle.h=Block_Height;

  SDL_BlitSurface(ne_blocks , ne_droid_block+phase, ne_screen, &TargetRectangle);

  // Now the numbers should be blittet.

  TargetRectangle.x=USER_FENSTER_CENTER_X - 
    (Me.pos.x-Feindesliste[Enum].pos.x) * Block_Width + DIGIT_POS_X  - Block_Width/2; 
  TargetRectangle.y=USER_FENSTER_CENTER_Y - 
    (Me.pos.y-Feindesliste[Enum].pos.y) * Block_Height + DIGIT_POS_Y - Block_Height/2;
  SDL_BlitSurface( ne_blocks , ne_digit_block + (Druidmap[Feindesliste[Enum].type].druidname[0]-'1'+11) , 
		   ne_screen, &TargetRectangle );

  TargetRectangle.x=USER_FENSTER_CENTER_X - 
    (Me.pos.x-Feindesliste[Enum].pos.x)*Block_Height + DIGIT_POS_X + DIGITLENGTH-1 - Block_Width/2;
  TargetRectangle.y=USER_FENSTER_CENTER_Y - 
    (Me.pos.y-Feindesliste[Enum].pos.y)*Block_Height + DIGIT_POS_Y - Block_Height/2 ;
  SDL_BlitSurface( ne_blocks , ne_digit_block + (Druidmap[Feindesliste[Enum].type].druidname[1]-'1'+11) , 
		   ne_screen, &TargetRectangle );

  TargetRectangle.x=USER_FENSTER_CENTER_X - (Me.pos.x-Feindesliste[Enum].pos.x)*Block_Width - Block_Width/2 + DIGIT_POS_X + 2*(DIGITLENGTH-1);
  TargetRectangle.y=USER_FENSTER_CENTER_Y - (Me.pos.y-Feindesliste[Enum].pos.y)*Block_Width - Block_Height/2 + DIGIT_POS_Y;
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

  TargetRectangle.x=USER_FENSTER_CENTER_X-(Me.pos.x-CurBullet->pos.x)*Block_Width-Block_Width/2;
  TargetRectangle.y=USER_FENSTER_CENTER_Y-(Me.pos.y-CurBullet->pos.y)*Block_Width-Block_Height/2;

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

  TargetRectangle.x=USER_FENSTER_CENTER_X - (Me.pos.x - CurBlast->PX)*Block_Width  -Block_Width/2;
  TargetRectangle.y=USER_FENSTER_CENTER_Y - (Me.pos.y - CurBlast->PY)*Block_Height -Block_Height/2;
  SDL_BlitSurface( ne_blocks, 
		   Blastmap[CurBlast->type].block + ((int) rintf(CurBlast->phase)), ne_screen , &TargetRectangle);

  return;
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
    (INTERNHOEHE) * Block_Height / 2 * Block_Width * INTERNBREITE +
    INTERNBREITE * Block_Width / 2 - Block_Width / 2 -
    Block_Width * INTERNBREITE * Block_Height / 2;

  /* Verschiebung zum Influencer  (linkes oberes Eck !!) */
  InternWindowOffset += ((((int) Me.pos.y) % Block_Height) - Block_Height / 2) *
    Block_Width * INTERNBREITE +
    (((int) Me.pos.x) % Block_Width) - Block_Width / 2;

  /* relative Verschiebung des Objekts zum Influencer */
  InternWindowOffset += DifY * Block_Width * INTERNBREITE + DifX;

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
	  (source, target, INTERNBREITE * Block_Width, check));

  DebugPrintf
    ("\nint PutObject(...): usual end of function reached.\n");

  return (Return_Value);
} /* PutObject() */


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
	  INTERNBREITE * INTERNHOEHE * Block_Width * Block_Height);
}				// void FlashWindow(int Flashcolor)

/*-----------------------------------------------------------------
 * @Desc: Setzt die Hintergrundfarbe fuer das Userfenster using SDL
 * @Ret: void
 *
 *-----------------------------------------------------------------*/
void
SetUserfenster (int color)
{
  SDL_Rect LocalRectangle;

  LocalRectangle.x=USERFENSTERPOSX;
  LocalRectangle.y=USERFENSTERPOSY;
  LocalRectangle.w=USERFENSTERBREITE;
  LocalRectangle.h=USERFENSTERHOEHE;

  SDL_FillRect( ne_screen , &LocalRectangle, color );
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
  SDL_Surface *tmp;
  SDL_Rect SourceRectangle;
  SDL_Rect TargetRectangle;
  char ImageFilename[100]="../graphics/";

  DebugPrintf ("\nvoid ShowRobotPicture(...): Function call confirmed.");

  strcat( ImageFilename , Druidmap[Number].druidname );
  strcat( ImageFilename , ".jpg" );

  if ( !(tmp=IMG_Load( ImageFilename ) ) )
    {
      fprintf (stderr,
	     "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
The image file named %s could not be read by SDL.\n\
\n\
Please check that the file is present and not corrupted\n\
in your distribution of Freedroid.\n\
\n\
Freedroid will terminate now to point at the error.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" , ImageFilename );
      Terminate (ERR);
    }
  
  SourceRectangle.x=0;
  SourceRectangle.y=0;
  SourceRectangle.w=USERFENSTERBREITE;
  SourceRectangle.h=USERFENSTERHOEHE;
  TargetRectangle.x=USERFENSTERPOSX;
  TargetRectangle.y=USERFENSTERPOSY + TEXT_STRETCH * FontHeight(Menu_BFont) ;
  SDL_BlitSurface( tmp , &SourceRectangle, ne_screen , &TargetRectangle );
  
  SDL_FreeSurface(tmp);

  DebugPrintf
    ("\nvoid ShowRobotPicture(...): Usual end of function reached.");
}				// void ShowRobotPicture(...)

#undef _view_c
