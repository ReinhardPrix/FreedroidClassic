/*----------------------------------------------------------------------
 * Desc: contains block operating functions, that is when you want to
 * 	put something on the visible screen
 *      of the paradroid, DO NOT DO IT YOURSELF!  Use one of the functions
 *	in here.
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
 *                    !! allocated here !!
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

  SDL_SetAlpha( tmp , 0 ,  0 ); // this command is used to TAKE THE ALPHA */
				// CHANNEL WITH US IN THE BLIT AND 
                                // NOT APPLY IT HERE

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

  // Digit_Pos_X=INITIAL_DIGIT_POS_X;
  // Digit_Pos_Y = INITIAL_DIGIT_POS_Y;


  return (ret);

} /* ne_get_digit_blocks() */

SDL_Rect *
ne_get_rahmen_block (char *picfile)
{
  SDL_Surface *tmp;
  SDL_Rect rect, *ret;

  /* Load the map-block BMP file into the appropriate surface */
  if( !(tmp = IMG_Load(picfile) ))
    {
      fprintf(stderr, "Couldn't load %s: %s\n", picfile, SDL_GetError());
      Terminate (ERR);
    }

  ret = (SDL_Rect*)MyMalloc(sizeof(SDL_Rect));


  /* now copy the block to ne_static  */
  rect.x = 0;
  rect.y = 0;
  rect.w = BANNER_WIDTH;
  rect.h = BANNER_HEIGHT;
  
  ret[0].x = 0;
  ret[0].y = 0;
  ret[0].w = BANNER_WIDTH;
  ret[0].h = BANNER_HEIGHT;
  SDL_BlitSurface (tmp, &rect, ne_static, &ret[0]);

  SDL_FreeSurface (tmp);

  return (ret);

} /* ne_get_rahmen_block() */





#undef _blocks_c
