/*----------------------------------------------------------------------
 * Desc: contains block operating functions, that is when you want to
 * 	put something on the visible screen
 *      of the paradroid, DO NOT DO IT YOURSELF!  Use one of the functions
 *	in here, e.g. DisplayBlock(..),
 *      DisplayMergeBlock(..), CopyMergeBlock(...),.. .
 *      These functions already take into acount the position of the paradoid,
 * 	so you do not have to worry about where to put anything, and only have
 *	to supply map coordinates.  Very handy.
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
#define _blocks_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"

/*-----------------------------------------------------------------
 * @Desc: display the mini-block number "BlockN" at position (LX, LY)
 * 	  on "*Screen" of width "SBreite"
 *
 *-----------------------------------------------------------------*/
void
SmallBlock (int LX, int LY, int BlockN, unsigned char *Parameter_Screen, int SBreite)
{
  int i, j;
  unsigned char *source = MapBlocks + BLOCKBREITE * BLOCKHOEHE * BlockN;
  unsigned char *target = Parameter_Screen + LY * SBreite + LX;

  if (Parameter_Screen == RealScreen)
    target=Outline320x200 + LY * SBreite + LX;

  DebugPrintf("\nvoid SmallBlock(...): real function call confirmed.");

  if (LX > USERFENSTERPOSX + USERFENSTERBREITE)
    return;

  for (i = 0; i < 8; i++)
    {
      for (j = 0; j < 8; j++)
	{
	  *target = *source;
	  target++;
	  source += 4;
	}
      target += SBreite - 8;
      source += 4 * BLOCKBREITE - 4 * 8;
    }

} /* SmallBlock() */

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
SmallBlast (int LX, int LY, int BlastT, int phase, unsigned char *Parameter_Screen,
	    int SBreite)
{
  int i, j;

  if (LX > USERFENSTERPOSX + USERFENSTERBREITE)
    return;
  for (i = 0; i < 8; i++)
    {
      for (j = 0; j < 8; j++)
	if (*
	    (Blastmap[BlastT].picpointer + j * 4 + i * BLOCKBREITE * 4 +
	     phase * BLOCKMEM) != TRANSPARENTCOLOR)
	  Parameter_Screen[LX - DIGITLENGTH / 2 + j +
		 (LY + i - DIGITHEIGHT / 2) * SBreite] =
	    *(Blastmap[BlastT].picpointer + j * 4 + i * BLOCKBREITE * 4 +
	      phase * BLOCKMEM);
    }
}

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
SmallBullet (int LX, int LY, int BulletT, int phase, unsigned char *Parameter_Screen,
	     int SBreite)
{
  Blast CurBlast = &(AllBlasts[0]);
  int i, j;

  if (LX > USERFENSTERPOSX + USERFENSTERBREITE)
    return;
  for (i = 0; i < 8; i++)
    {
      for (j = 0; j < 8; j++)
	{
	  if (*
	      (Bulletmap[BulletT].picpointer + j * 4 + i * BLOCKBREITE * 4 +
	       phase * BLOCKMEM) != TRANSPARENTCOLOR)
	    {
	      if ((unsigned char)
		  Parameter_Screen[LX - DIGITLENGTH / 2 + j +
			 (LY + i - DIGITHEIGHT / 2) * SBreite] == BULLETCOLOR)
		StartBlast (LX * 4 + 2, LY * 4 + 2, DRUIDBLAST);
	      Parameter_Screen[LX - DIGITLENGTH / 2 + j +
		     (LY + i - DIGITHEIGHT / 2) * SBreite] =
		*(Bulletmap[BulletT].picpointer + j * 4 +
		  i * BLOCKBREITE * 4 + phase * BLOCKMEM);
	    }
	}
    }
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
}

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
SmallEnemy (int LX, int LY, int enemyclass, unsigned char *Parameter_Screen,
	    int SBreite)
{
  int i, j;
  enemyclass += 10;

  if (LX > USERFENSTERPOSX + USERFENSTERBREITE)
    return;
  for (i = 0; i < DIGITHEIGHT; i++)
    {
      for (j = 0; j < DIGITLENGTH - 1; j++)
	if (Digitpointer
	    [enemyclass * DIGITLENGTH * DIGITHEIGHT + i * DIGITLENGTH + j] !=
	    TRANSPARENTCOLOR)
	  Parameter_Screen[LX - DIGITLENGTH / 2 + j +
		 (LY + i - DIGITHEIGHT / 2) * SBreite] =
	    Digitpointer[enemyclass * DIGITLENGTH * DIGITHEIGHT +
			 i * DIGITLENGTH + j];
//                                      Enemypointer[j*4+i*BLOCKBREITE*4];
    }
}

/* *********************************************************************** */

void
GetDigits (void)
{
#ifdef NEW_ENGINE
  return;
#else

  int i;

  Digitpointer = MyMalloc (DIGITMEM);
  Load_PCX_Image (DIGITBILD_PCX, InternalScreen, FALSE);

  for (i = 0; i < 20; i++)
    {
      IsolateBlock (InternalScreen,
		    Digitpointer + DIGITHEIGHT * DIGITLENGTH * i,
		    i * DIGITLENGTH, 0, DIGITLENGTH, DIGITHEIGHT);
    }

#endif // !NEW_ENGINE
} // void GetDigits(void)

/* *********************************************************************** */

/*-----------------------------------------------------------------
 *@Desc: Diese Prozedur isoliert einzelne Teile aus *screen
 *	 angezeigten Bild und legt diese in *target
 *
 * Parameter: screen: Screen, auf dem Grafik steht
 *	      target: Ziel, in das der Block kopiert wird
 *		==> muss schon reserviert sein !!
 *					
 *	Links/Oben : Koordinaten des linken oberen Eck des Blocks
 *
 * @Ret: void
 *
 *-----------------------------------------------------------------*/
void
IsolateBlock (unsigned char *Parameter_screen,
	      unsigned char *target,
	      int BlockEckLinks,
	      int BlockEckOben, int Blockbreite, int Blockhoehe)
{
  int row;
  unsigned char *source;
  unsigned char *tmp;

  source = Parameter_screen + BlockEckOben * SCREENLEN + BlockEckLinks;
  tmp = target;

  for (row = 0; row < Blockhoehe; row++)
    {
      memcpy (tmp, source, Blockbreite);
      tmp += Blockbreite;
      source += SCREENLEN;
    }

}				/* IsolateBlock */


/*-----------------------------------------------------------------
 * Diese Prozedur isoliert ueber Aufrufe der Funktion GetBlock aus dem
 * Anfangsbild alle Bloecke, die benoetigt werden und legt diese im Speicher
 * ab
 * 
 *   Parameter: keine
 *-----------------------------------------------------------------*/
void
GetMapBlocks (void)
{
#ifdef NEW_ENGINE
  return;
#else

  int i;
  unsigned char *tmp;

  MapBlocks = (unsigned char *) MyMalloc (BLOCKANZAHL * BLOCKMEM + 100);
  Load_PCX_Image (BLOCKBILD1_PCX, InternalScreen, FALSE);

  tmp = MapBlocks;

  for (i = 0; i < 9; tmp += BLOCKMEM, i++)
    IsolateBlock (InternalScreen, tmp, i * (BLOCKBREITE + 1), 0, BLOCKBREITE,
		  BLOCKHOEHE);

  for (i = 0; i < 9; tmp += BLOCKMEM, i++)
    IsolateBlock (InternalScreen, tmp, i * (BLOCKBREITE + 1), BLOCKHOEHE + 1,
		  BLOCKBREITE, BLOCKHOEHE);

  for (i = 0; i < 9; tmp += BLOCKMEM, i++)
    IsolateBlock (InternalScreen, tmp, i * (BLOCKBREITE + 1),
		  BLOCKHOEHE * 2 + 2, BLOCKBREITE, BLOCKHOEHE);

  for (i = 0; i < 9; tmp += BLOCKMEM, i++)
    IsolateBlock (InternalScreen, tmp, i * (BLOCKBREITE + 1),
		  BLOCKHOEHE * 3 + 3, BLOCKBREITE, BLOCKHOEHE);

  for (i = 0; i < 7; tmp += BLOCKMEM, i++)
    IsolateBlock (InternalScreen, tmp, i * (BLOCKBREITE + 1),
		  BLOCKHOEHE * 4 + 4, BLOCKBREITE, BLOCKHOEHE);

#endif // !NEW_ENGINE
}

/*-----------------------------------------------------------------
 * @Desc: loads picture file, transfers blocks into main block-surface
 *        (ne_blocks), and fills SDL_Rect array with the appropriate 
 *        coordinates. 
 * 	  The target_line given refers to the line in ne_blocks.
 *
 * @Arguments:	char *picfile	: the picture-file to load
 *              int num_blocks  : number of blocks to read
 *              int blocks_per_line: blocks per line in picture file
 *                                  if 0: ==> = num_blocks
 * 		int source_line: block-line to read from in pic-file
 * 		int target_line: block-line in ne_blocks where blocks are put
 * 
 * @Ret: SDL_Rect *  : array holding the block-coordinates 
 *
 *-----------------------------------------------------------------*/
#ifdef NEW_ENGINE
SDL_Rect *
ne_get_blocks (char *picfile, int num_blocks, int blocks_per_line,
	       int source_line, int target_line)
{
  int i;
  SDL_Surface *tmp;
  SDL_Rect rect, *ret;

  /* Load the map-block BMP file into the appropriate surface */
  if( !(tmp = SDL_LoadBMP(picfile) ))
    {
      fprintf(stderr, "Couldn't load %s: %s\n", picfile, SDL_GetError());
      Terminate (ERR);
    }

  if (!blocks_per_line) /* only one line here */
    blocks_per_line = num_blocks;

  ret = (SDL_Rect*)MyMalloc(num_blocks*sizeof(SDL_Rect));


  /* now copy the individual map-blocks into ne_blocks */
  for (i=0; i < num_blocks; i++)
    {
      rect.x = (i%blocks_per_line)*(BLOCK_WIDTH+1);
      rect.y = (source_line+i/blocks_per_line)*(BLOCK_HEIGHT+1);
      rect.w = BLOCK_WIDTH;
      rect.h = BLOCK_HEIGHT;
      
      ret[i].x = i*BLOCK_WIDTH;
      ret[i].y = target_line*BLOCK_HEIGHT;
      ret[i].w = BLOCK_WIDTH;
      ret[i].h = BLOCK_HEIGHT;
      SDL_BlitSurface (tmp, &rect, ne_blocks, &ret[i]);
    }
  SDL_FreeSurface (tmp);

  return (ret);

} /* ne_get_blocks() */

SDL_Rect *
ne_get_digit_blocks (char *picfile, int num_blocks, int blocks_per_line,
	       int source_line, int target_line)
{
  int i;
  SDL_Surface *tmp;
  SDL_Rect rect, *ret;

  /* Load the map-block BMP file into the appropriate surface */
  if( !(tmp = SDL_LoadBMP(picfile) ))
    {
      fprintf(stderr, "Couldn't load %s: %s\n", picfile, SDL_GetError());
      Terminate (ERR);
    }

  if (!blocks_per_line) /* only one line here */
    blocks_per_line = num_blocks;

  ret = (SDL_Rect*)MyMalloc(num_blocks*sizeof(SDL_Rect));


  /* now copy the individual map-blocks into ne_blocks */
  for (i=0; i < num_blocks; i++)
    {
      rect.x = (i%blocks_per_line)*(DIGITLENGTH);
      rect.y = (source_line+i/blocks_per_line)*(BLOCK_HEIGHT+1);
      rect.w = DIGITLENGTH-1;
      rect.h = DIGITHEIGHT;
      
      ret[i].x = i*DIGITLENGTH;
      ret[i].y = target_line*BLOCK_HEIGHT;
      ret[i].w = DIGITLENGTH;
      ret[i].h = DIGITHEIGHT;
      SDL_BlitSurface (tmp, &rect, ne_blocks, &ret[i]);
    }
  SDL_FreeSurface (tmp);

  return (ret);

} /* ne_get_digit_blocks() */

SDL_Rect *
ne_get_rahmen_block (char *picfile, int num_blocks, int blocks_per_line,
	       int source_line, int target_line)
{
  int i;
  SDL_Surface *tmp;
  SDL_Rect rect, *ret;

  /* Load the map-block BMP file into the appropriate surface */
  if( !(tmp = SDL_LoadBMP(picfile) ))
    {
      fprintf(stderr, "Couldn't load %s: %s\n", picfile, SDL_GetError());
      Terminate (ERR);
    }

  if (!blocks_per_line) /* only one line here */
    blocks_per_line = num_blocks;

  ret = (SDL_Rect*)MyMalloc(num_blocks*sizeof(SDL_Rect));


  /* now copy the individual map-blocks into ne_blocks */
  for (i=0; i < num_blocks; i++)
    {
      rect.x = (i%blocks_per_line)*(DIGITLENGTH);
      rect.y = (source_line+i/blocks_per_line)*(BLOCK_HEIGHT+1);
      rect.w = RAHMENBREITE;
      rect.h = RAHMENHOEHE;
      
      ret[i].x = i*DIGITLENGTH;
      ret[i].y = target_line*BLOCK_HEIGHT;
      ret[i].w = DIGITLENGTH;
      ret[i].h = DIGITHEIGHT;
      SDL_BlitSurface (tmp, &rect, ne_blocks, &ret[i]);
    }
  SDL_FreeSurface (tmp);

  return (ret);

} /* ne_get_rahmen_block() */

#endif // NEW_ENGINE

/*@Function============================================================
@Desc: GetBlocks
gets the requested picture file and returns a pointer to
the requested blocks (sequtially stored)
				
@Arguments:	char *picfile	: the picture-file to load, or
NULL to use the old one
									
int line: block-line to get blocks from
int num:	number of blocks to get from line

@Ret: char * : pointer to 
@Int:
* $Function----------------------------------------------------------*/
unsigned char *
GetBlocks (char *picfile, int line, int num)
{
  int i;
  unsigned char *tmp;
  unsigned char *blocktarget;

  if (picfile)
    {
      Load_PCX_Image (picfile, InternalScreen, FALSE);
    }

  if (!num)
    return NULL;		/* this was only an 'init'-call */

  blocktarget = MyMalloc (BLOCKMEM * num + 1600);
  tmp = blocktarget;

  for (i = 0; i < num; tmp += BLOCKMEM, i++)
    IsolateBlock (InternalScreen, tmp, i * (BLOCKBREITE + 1),
		  line * (BLOCKHOEHE + 1), BLOCKBREITE, BLOCKHOEHE);

  return blocktarget;
}				// unsigned char *GetBlocks(...)


/*-----------------------------------------------------------------
 * @Desc: gibt Block *block (len*height) an angegebener
 * Bildschirmposition x/y aus auf screen
 *						
 * @Ret: void
 *
 *-----------------------------------------------------------------*/
void
DisplayBlock (int x, int y,
	      unsigned char *block,
	      int len, int height, unsigned char *Parameter_screen)
{
  int row, i;
  unsigned char *screenpos;
  unsigned char *source = block;

  if (Parameter_screen == RealScreen) 
    {

      screenpos = Outline320x200 + y * SCREENLEN + x;
      
      for (i = 0; i < height; i++)
	{
	  memcpy (screenpos, source, len);
	  screenpos += SCREENLEN;
	  source += len;
	}
    }
  else
    {
      screenpos = Parameter_screen + y * SCREENLEN + x;

      for (row = 0; row < height; row++)
	{
	  memcpy (screenpos, source, len);
	  screenpos += SCREENLEN;
	  source += len;
	}			/* for row */
    }				/* else */

  return;

}				/* DisplayBlock */

/*-----------------------------------------------------------------
 * @Desc: setzt Block *block (len*height) an angegebener
 *	Bildschirmposition x/y in den angeg. Bildschirm
 *	-beachtet dabei TRANSPARENTCOLOR 
 *						
 * @Ret: void
 *
 *-----------------------------------------------------------------*/
void
DisplayMergeBlock (int x, int y, unsigned char *block,
		   int len, int height, unsigned char *Parameter_screen)
{
  int row, col;
  unsigned char *Screenpos;
  unsigned char *source = block;

  if (Parameter_screen == NULL)
    {
      printf("\n\nvoid DisplayMergeBlock(...): NULL Pointer Received...\n\nTerminating...\n\n");
      Terminate(ERR);
    }

  Screenpos = Parameter_screen + x + y * SCREENBREITE;

  if (Parameter_screen == RealScreen)
    {
      Screenpos = Outline320x200 + x + y * SCREENBREITE;
    }

  for (row = 0; row < height; row++)
    {
      for (col = 0; col < len; col++)
	{
	  if (*source != TRANSPARENTCOLOR)
	    *Screenpos++ = *source++;
	  else
	    {
	      Screenpos++;
	      source++;
	    }
	}			/* for (col) */
      Screenpos += SCREENBREITE - len;
    }				/* for (row) */
  
  return;

}				/* DisplayMergeBlock */

/*@Function============================================================
@Desc: CopyMergeBlock(): copies a block in memory, but doesn't copy
		Transparent-color 

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void
CopyMergeBlock (unsigned char *target, unsigned char *source, int mem)
{
  int i;

  for (i = 0; i < mem; i++, source++, target++)
    if (*source != TRANSPARENTCOLOR)
      *target = *source;

}				/* Copy merge Block */


/*@Function============================================================
@Desc: MergeBlockToWindow(source, target, linelen)
This copies a block from memory (sequentially stored)
to a "window": All lines under each other, "invisible" points
are left out (TRANSPARENTCOLOR).
It must	know the length of a window-line.

int check: TRUE/FALSE: Bullet-Collisionen checken
			
@Ret: TRUE/FALSE: BulletCollision
@Int:
* $Function----------------------------------------------------------*/
int
MergeBlockToWindow (register unsigned char *source, register unsigned char *target, int WinLineLen,	/* in pixel ! */
		    int check)
{
  register int i, j;
  int ret = FALSE;
  register int lineskip = WinLineLen - BLOCKBREITE;

  if (check)
    {
      for (i = 0; i < BLOCKHOEHE; i++, target += lineskip)
	for (j = 0; j < BLOCKBREITE; j++)
	  {
	    if (*source != TRANSPARENTCOLOR)
	      {
		if (*target == BULLETCOLOR)
		  ret = TRUE;
		*target++ = *source++;
	      }
	    else
	      {
		target++;
		source++;
	      }
	  }
    }
  else
    {
      for (i = 0; i < BLOCKHOEHE; i++, target += lineskip)
	for (j = 0; j < BLOCKBREITE; j++)
	  {

	    if (*source != TRANSPARENTCOLOR)
	      {
		*target++ = *source++;
	      }
	    else
	      {
		target++;
		source++;
	      }
	  }
    }				/* if else */

  return (ret);

}				// int MergeBlockToWindow(...)




#undef _blocks_c
