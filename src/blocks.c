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

SDL_Rect *
ne_get_blocks (char *picfile, int num_blocks, int blocks_per_line,
	       int source_line, int target_line)
{
  int i;
  SDL_Surface *tmp;
  SDL_Rect rect, *ret;

  /* Load the map-block BMP file into the appropriate surface */
  if( !(tmp = IMG_Load(picfile) ))
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
      rect.x = (i%blocks_per_line)*(Block_Width+2);
      rect.y = (source_line+i/blocks_per_line)*(Block_Height+2);
      rect.w = Block_Width;
      rect.h = Block_Height;
      
      ret[i].x = i*Block_Width;
      ret[i].y = target_line*Block_Height;
      ret[i].w = Block_Width;
      ret[i].h = Block_Height;
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
      rect.x = (i%blocks_per_line)*(INITIAL_DIGIT_LENGTH+2);
      rect.y = (source_line+i/blocks_per_line)*(Block_Height+2);
      rect.w = INITIAL_DIGIT_LENGTH-1;
      rect.h = INITIAL_DIGIT_HEIGHT;
      
      ret[i].x = i*INITIAL_DIGIT_LENGTH;
      ret[i].y = target_line*Block_Height;
      ret[i].w = INITIAL_DIGIT_LENGTH;
      ret[i].h = INITIAL_DIGIT_HEIGHT;
      SDL_BlitSurface (tmp, &rect, ne_blocks, &ret[i]);
    }

  SDL_FreeSurface (tmp);

  Digit_Length=INITIAL_DIGIT_LENGTH;
  Digit_Height=INITIAL_DIGIT_HEIGHT;
  Digit_Pos_X=INITIAL_DIGIT_POS_X;
  Digit_Pos_Y = INITIAL_DIGIT_POS_Y;


  return (ret);

} /* ne_get_digit_blocks() */

SDL_Rect *
ne_get_rahmen_block (char *picfile)
{
  SDL_Surface *tmp;
  SDL_Rect rect, *ret;

  /* Load the map-block BMP file into the appropriate surface */
  if( !(tmp = SDL_LoadBMP(picfile) ))
    {
      fprintf(stderr, "Couldn't load %s: %s\n", picfile, SDL_GetError());
      Terminate (ERR);
    }

  ret = (SDL_Rect*)MyMalloc(sizeof(SDL_Rect));


  /* now copy the block to ne_static  */
  rect.x = 0;
  rect.y = 0;
  rect.w = RAHMENBREITE;
  rect.h = RAHMENHOEHE;
  
  ret[0].x = 0;
  ret[0].y = 0;
  ret[0].w = RAHMENBREITE;
  ret[0].h = RAHMENHOEHE;
  SDL_BlitSurface (tmp, &rect, ne_static, &ret[0]);

  SDL_FreeSurface (tmp);

  return (ret);

} /* ne_get_rahmen_block() */

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
  register int lineskip = WinLineLen - Block_Width;

  if (check)
    {
      for (i = 0; i < Block_Height; i++, target += lineskip)
	for (j = 0; j < Block_Width; j++)
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
      for (i = 0; i < Block_Height; i++, target += lineskip)
	for (j = 0; j < Block_Width; j++)
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
