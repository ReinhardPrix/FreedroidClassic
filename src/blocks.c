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

/* ----------------------------------------------------------------------
 * Desc: contains block operating functions, that is when you want to
 * 	put something on the visible screen
 *      of the paradroid, DO NOT DO IT YOURSELF!  Use one of the functions
 *	in here.
 *      These functions already take into acount the position of the paradoid,
 * 	so you do not have to worry about where to put anything, and only have
 *	to supply map coordinates.  Very handy.
 * ---------------------------------------------------------------------- */

#define _blocks_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"

/* *********************************************************************** */

/*
----------------------------------------------------------------------
This function loads the Blast image and decodes it into the multiple
small Blast surfaces.
----------------------------------------------------------------------
*/
void 
Load_Blast_Surfaces( void )
{
  SDL_Surface* Whole_Image;
  SDL_Surface* tmp_surf;
  SDL_Rect Source;
  SDL_Rect Target;
  int i;
  int j;
  char *fpath;

  fpath = find_file (NE_BLAST_BLOCK_FILE, GRAPHICS_DIR, TRUE);

  Whole_Image = IMG_Load( fpath ); // This is a surface with alpha channel, since the picture is one of this type
  SDL_SetAlpha( Whole_Image , 0 , SDL_ALPHA_OPAQUE );

  for ( i=0 ; i < ALLBLASTTYPES ; i++ )
    {
      for ( j=0 ; j < Blastmap[i].phases ; j++ )
	{
	  tmp_surf = SDL_CreateRGBSurface( 0 , Block_Width, Block_Height, vid_bpp, 0, 0, 0, 0);
	  SDL_SetColorKey( tmp_surf , 0 , 0 ); // this should clear any color key in the source surface
	  Blastmap[i].SurfacePointer[j] = SDL_DisplayFormatAlpha( tmp_surf ); // now we have an alpha-surf of right size
	  SDL_SetColorKey( Blastmap[i].SurfacePointer[j] , 0 , 0 ); // this should clear any color key in the dest surface
	  // Now we can copy the image Information
	  Source.x=j*(Block_Height+2);
	  Source.y=i*(Block_Width+2);
	  Source.w=Block_Width;
	  Source.h=Block_Height;
	  Target.x=0;
	  Target.y=0;
	  Target.w=Block_Width;
	  Target.h=Block_Height;
	  SDL_BlitSurface ( Whole_Image , &Source , Blastmap[i].SurfacePointer[j] , &Target );
	  SDL_SetAlpha( Blastmap[i].SurfacePointer[j] , SDL_SRCALPHA , SDL_ALPHA_OPAQUE );
	}
    }

  SDL_FreeSurface( tmp_surf );

}; // void Load_Blast_Surfaces( void )


/* ----------------------------------------------------------------------
 * This function loads the items image and decodes it into the multiple
 * small item surfaces.
 * ---------------------------------------------------------------------- */
void 
Load_Item_Surfaces( void )
{
  SDL_Surface* Whole_Image;
  SDL_Surface* tmp_surf;
  SDL_Rect Source;
  SDL_Rect Target;
  int i=0;
  int j;
  char *fpath;

  ItemImageList[ 0 ].inv_size.x = 1;
  ItemImageList[ 0 ].inv_size.y = 1;
  ItemImageList[ 1 ].inv_size.x = 2;
  ItemImageList[ 1 ].inv_size.y = 2;
  ItemImageList[ 2 ].inv_size.x = 2;
  ItemImageList[ 2 ].inv_size.y = 2;
  ItemImageList[ 3 ].inv_size.x = 2;
  ItemImageList[ 3 ].inv_size.y = 2;
  ItemImageList[ 4 ].inv_size.x = 2;
  ItemImageList[ 4 ].inv_size.y = 2;
  ItemImageList[ 5 ].inv_size.x = 2;
  ItemImageList[ 5 ].inv_size.y = 2;
  ItemImageList[ 6 ].inv_size.x = 2;
  ItemImageList[ 6 ].inv_size.y = 2;
  ItemImageList[ 7 ].inv_size.x = 2;
  ItemImageList[ 7 ].inv_size.y = 2;
  ItemImageList[ 8 ].inv_size.x = 2;
  ItemImageList[ 8 ].inv_size.y = 2;
  ItemImageList[ 9 ].inv_size.x = 2;
  ItemImageList[ 9 ].inv_size.y = 2;
  ItemImageList[ 10 ].inv_size.x = 2;
  ItemImageList[ 10 ].inv_size.y = 2;
  ItemImageList[ 11 ].inv_size.x = 2;
  ItemImageList[ 11 ].inv_size.y = 2;
  ItemImageList[ 12 ].inv_size.x = 2;
  ItemImageList[ 12 ].inv_size.y = 2;
  ItemImageList[ 13 ].inv_size.x = 2;
  ItemImageList[ 13 ].inv_size.y = 2;
  ItemImageList[ 14 ].inv_size.x = 2;
  ItemImageList[ 14 ].inv_size.y = 2;
  ItemImageList[ 15 ].inv_size.x = 2;
  ItemImageList[ 15 ].inv_size.y = 3;
  ItemImageList[ 16 ].inv_size.x = 2;
  ItemImageList[ 16 ].inv_size.y = 2;
  ItemImageList[ 17 ].inv_size.x = 2;
  ItemImageList[ 17 ].inv_size.y = 2;
  ItemImageList[ 18 ].inv_size.x = 2;
  ItemImageList[ 18 ].inv_size.y = 2;
  ItemImageList[ 19 ].inv_size.x = 2;
  ItemImageList[ 19 ].inv_size.y = 2;
  ItemImageList[ 20 ].inv_size.x = 2;
  ItemImageList[ 20 ].inv_size.y = 2;

  fpath = find_file ( NE_ITEMS_BLOCK_FILE , GRAPHICS_DIR, TRUE);

  Whole_Image = IMG_Load( fpath ); // This is a surface with alpha channel, since the picture is one of this type
  SDL_SetAlpha( Whole_Image , 0 , SDL_ALPHA_OPAQUE );

  for ( j=0 ; j < NUMBER_OF_ITEM_PICTURES ; j++ )
    {
      Source.x = j * ( Block_Height + 2 );
      Source.y = i * ( Block_Width  + 2 );
      Source.w = (Block_Width/2) * ItemImageList[ j ].inv_size.x ;
      Source.h = (Block_Height/2) * ItemImageList[ j ].inv_size.y ;
      Target.x = 0;
      Target.y = 0;
      Target.w = Source.w;
      Target.h = Source.h;
      tmp_surf = SDL_CreateRGBSurface( 0 , Source.w , Source.h , vid_bpp , 0 , 0 , 0 , 0 );
      SDL_SetColorKey( tmp_surf , 0 , 0 ); // this should clear any color key in the source surface
      ItemImageList[ j ].Surface = SDL_DisplayFormatAlpha( tmp_surf ); // now we have an alpha-surf of right size
      SDL_SetColorKey( ItemImageList[ j ].Surface , 0 , 0 ); // this should clear any color key in the dest surface
      // Now we can copy the image Information
      SDL_BlitSurface ( Whole_Image , &Source , ItemImageList[ j ].Surface , &Target );
      SDL_SetAlpha( ItemImageList[ j ].Surface , SDL_SRCALPHA , SDL_ALPHA_OPAQUE );
    }

  SDL_FreeSurface( tmp_surf );


}; // void Load_Item_Surfaces( void )

/* ----------------------------------------------------------------------
 * This function loads the Bullet image and decodes it into the multiple
 * small Blast surfaces.
 * ---------------------------------------------------------------------- */
void 
Load_Bullet_Surfaces( void )
{
  SDL_Surface* Whole_Image;
  SDL_Surface* tmp_surf;
  SDL_Rect Source;
  SDL_Rect Target;
  int i;
  int j;
  char *fpath;

  fpath = find_file (NE_BULLET_BLOCK_FILE, GRAPHICS_DIR, TRUE);

  Whole_Image = IMG_Load( fpath ); // This is a surface with alpha channel, since the picture is one of this type
  SDL_SetAlpha( Whole_Image , 0 , SDL_ALPHA_OPAQUE );

  for ( i=0 ; i < Number_Of_Bullet_Types ; i++ )
    {
      for ( j=0 ; j < Bulletmap[i].phases ; j++ )
	{
	  tmp_surf = SDL_CreateRGBSurface( 0 , Block_Width, Block_Height, vid_bpp, 0, 0, 0, 0);
	  SDL_SetColorKey( tmp_surf , 0 , 0 ); // this should clear any color key in the source surface
	  Bulletmap[i].SurfacePointer[j] = SDL_DisplayFormatAlpha( tmp_surf ); // now we have an alpha-surf of right size
	  SDL_SetColorKey( Bulletmap[i].SurfacePointer[j] , 0 , 0 ); // this should clear any color key in the dest surface
	  // Now we can copy the image Information
	  Source.x=j*(Block_Height+2);
	  Source.y=i*(Block_Width+2);
	  Source.w=Block_Width;
	  Source.h=Block_Height;
	  Target.x=0;
	  Target.y=0;
	  Target.w=Block_Width;
	  Target.h=Block_Height;
	  SDL_BlitSurface ( Whole_Image , &Source , Bulletmap[i].SurfacePointer[j] , &Target );
	  SDL_SetAlpha( Bulletmap[i].SurfacePointer[j] , SDL_SRCALPHA , SDL_ALPHA_OPAQUE );
	}
    }

  SDL_FreeSurface( tmp_surf );

}; // void Load_Bullet_Surfaces( void )


/* 
----------------------------------------------------------------------
----------------------------------------------------------------------
*/
void 
Load_Enemy_Surfaces( void )
{
  SDL_Surface* Whole_Image;
  SDL_Surface* tmp_surf;
  SDL_Rect Source;
  SDL_Rect Target;
  int i;
  char *fpath;

  fpath = find_file ( NE_DROID_BLOCK_FILE , GRAPHICS_DIR, TRUE);

  Whole_Image = IMG_Load( fpath ); // This is a surface with alpha channel, since the picture is one of this type
  SDL_SetAlpha( Whole_Image , 0 , SDL_ALPHA_OPAQUE );

  for ( i=0 ; i < DROID_PHASES ; i++ )
    {
      tmp_surf = SDL_CreateRGBSurface( 0 , Block_Width, Block_Height, vid_bpp, 0, 0, 0, 0);
      SDL_SetColorKey( tmp_surf , 0 , 0 ); // this should clear any color key in the source surface
      EnemySurfacePointer[i] = SDL_DisplayFormatAlpha( tmp_surf ); // now we have an alpha-surf of right size
      SDL_SetColorKey( EnemySurfacePointer[i] , 0 , 0 ); // this should clear any color key in the dest surface
      // Now we can copy the image Information
      Source.x=i*(Block_Height+2);
      Source.y=1*(Block_Width+2);
      Source.w=Block_Width;
      Source.h=Block_Height;
      Target.x=0;
      Target.y=0;
      Target.w=Block_Width;
      Target.h=Block_Height;
      SDL_BlitSurface ( Whole_Image , &Source , EnemySurfacePointer[i] , &Target );
      SDL_SetAlpha( EnemySurfacePointer[i] , SDL_SRCALPHA , SDL_ALPHA_OPAQUE );
    }

  SDL_FreeSurface( tmp_surf );

}; // void LoadEnemySurfaces( void )


/* 
----------------------------------------------------------------------
----------------------------------------------------------------------
*/
void 
Load_Influencer_Surfaces( void )
{
  SDL_Surface* Whole_Image;
  SDL_Surface* tmp_surf;
  SDL_Rect Source;
  SDL_Rect Target;
  int i;
  char *fpath;

  fpath = find_file ( NE_DROID_BLOCK_FILE , GRAPHICS_DIR, TRUE);

  Whole_Image = IMG_Load( fpath ); // This is a surface with alpha channel, since the picture is one of this type
  SDL_SetAlpha( Whole_Image , 0 , SDL_ALPHA_OPAQUE );

  for ( i=0 ; i < DROID_PHASES ; i++ )
    {
      tmp_surf = SDL_CreateRGBSurface( 0 , Block_Width, Block_Height, vid_bpp, 0, 0, 0, 0);
      SDL_SetColorKey( tmp_surf , 0 , 0 ); // this should clear any color key in the source surface
      InfluencerSurfacePointer[i] = SDL_DisplayFormatAlpha( tmp_surf ); // now we have an alpha-surf of right size
      SDL_SetColorKey( InfluencerSurfacePointer[i] , 0 , 0 ); // this should clear any color key in the dest surface
      // Now we can copy the image Information
      Source.x=i*(Block_Height+2);
      Source.y=0*(Block_Width+2);
      Source.w=Block_Width;
      Source.h=Block_Height;
      Target.x=0;
      Target.y=0;
      Target.w=Block_Width;
      Target.h=Block_Height;
      SDL_BlitSurface ( Whole_Image , &Source , InfluencerSurfacePointer[i] , &Target );
      SDL_SetAlpha( InfluencerSurfacePointer[i] , SDL_SRCALPHA , SDL_ALPHA_OPAQUE );
    }

  SDL_FreeSurface( tmp_surf );

}; // void Load_Influencer_Surfaces( void )

/* 
----------------------------------------------------------------------
----------------------------------------------------------------------
*/
void 
Load_Digit_Surfaces( void )
{
  SDL_Surface* Whole_Image;
  SDL_Surface* tmp_surf;
  SDL_Rect Source;
  SDL_Rect Target;
  int i;
  char *fpath;

  fpath = find_file ( NE_DIGIT_BLOCK_FILE , GRAPHICS_DIR, TRUE);

  Whole_Image = IMG_Load( fpath ); // This is a surface with alpha channel, since the picture is one of this type
  SDL_SetAlpha( Whole_Image , 0 , SDL_ALPHA_OPAQUE );

  for ( i=0 ; i < DIGITNUMBER ; i++ )
    {
      tmp_surf = SDL_CreateRGBSurface( 0 , INITIAL_DIGIT_LENGTH , INITIAL_DIGIT_HEIGHT, vid_bpp, 0, 0, 0, 0);
      SDL_SetColorKey( tmp_surf , 0 , 0 ); // this should clear any color key in the source surface
      InfluDigitSurfacePointer[i] = SDL_DisplayFormat( tmp_surf ); // now we have an alpha-surf of right size
      // SDL_SetColorKey( InfluDigitSurfacePointer[i] , 0 , 0 ); // this should clear any color key in the dest surface
      // Now we can copy the image Information
      Source.x=i*( INITIAL_DIGIT_LENGTH + 2 );
      Source.y=0*( INITIAL_DIGIT_HEIGHT + 2);
      Source.w=INITIAL_DIGIT_LENGTH;
      Source.h=INITIAL_DIGIT_HEIGHT;
      Target.x=0;
      Target.y=0;
      Target.w=INITIAL_DIGIT_LENGTH;
      Target.h=INITIAL_DIGIT_HEIGHT;
      SDL_BlitSurface ( Whole_Image , &Source , InfluDigitSurfacePointer[i] , &Target );
      SDL_SetAlpha( InfluDigitSurfacePointer[i] , 0 , SDL_ALPHA_OPAQUE );
      if ( SDL_SetColorKey( InfluDigitSurfacePointer[i] , SDL_SRCCOLORKEY, transp_key ) == -1 )
	{
	  fprintf (stderr, "Transp setting by SDL_SetColorKey() failed: %s \n",
		   SDL_GetError());
	  Terminate( ERR );
	}
    }
  SDL_FreeSurface( tmp_surf );

  for ( i=0 ; i < DIGITNUMBER ; i++ )
    {
      tmp_surf = SDL_CreateRGBSurface( 0 , INITIAL_DIGIT_LENGTH , INITIAL_DIGIT_HEIGHT, vid_bpp, 0, 0, 0, 0);
      SDL_SetColorKey( tmp_surf , 0 , 0 ); // this should clear any color key in the source surface
      EnemyDigitSurfacePointer[i] = SDL_DisplayFormat( tmp_surf ); // now we have an alpha-surf of right size
      // SDL_SetColorKey( EnemyDigitSurfacePointer[i] , 0 , 0 ); // this should clear any color key in the dest surface
      // Now we can copy the image Information
      Source.x=(i+10)*( INITIAL_DIGIT_LENGTH + 2 );
      Source.y=0*( INITIAL_DIGIT_HEIGHT + 2);
      Source.w=INITIAL_DIGIT_LENGTH;
      Source.h=INITIAL_DIGIT_HEIGHT;
      Target.x=0;
      Target.y=0;
      Target.w=INITIAL_DIGIT_LENGTH;
      Target.h=INITIAL_DIGIT_HEIGHT;
      SDL_BlitSurface ( Whole_Image , &Source , EnemyDigitSurfacePointer[i] , &Target );
      SDL_SetAlpha( EnemyDigitSurfacePointer[i] , 0 , SDL_ALPHA_OPAQUE );
      if ( SDL_SetColorKey( EnemyDigitSurfacePointer[i] , SDL_SRCCOLORKEY, transp_key ) == -1 )
	{
	  fprintf (stderr, "Transp setting by SDL_SetColorKey() failed: %s \n",
		   SDL_GetError());
	  Terminate( ERR );
	}
    }
  SDL_FreeSurface( tmp_surf );

}; // void Load_Digit_Surfaces( void )

/* 
----------------------------------------------------------------------
----------------------------------------------------------------------
*/
void 
Load_MapBlock_Surfaces( void )
{
  SDL_Surface* Whole_Image;
  SDL_Surface* tmp_surf;
  SDL_Rect Source;
  SDL_Rect Target;
  int i;
  int color;
  char *fpath;

  char *ColoredBlockFiles[] = {
    "block_red.png",
    "block_yellow.png",
    "block_green.png",
    "block_gray.png",
    "block_blue.png",
    "block_turquoise.png",
    "block_dark.png",
    NULL
  }; 

  Block_Width=INITIAL_BLOCK_WIDTH;
  Block_Height=INITIAL_BLOCK_HEIGHT;
  
  for ( color = 0 ; color < NUM_COLORS ; color ++ )
    {

      fpath = find_file ( ColoredBlockFiles[ color ] , GRAPHICS_DIR, TRUE);

      Whole_Image = IMG_Load( fpath ); // This is a surface with alpha channel, since the picture is one of this type
      SDL_SetAlpha( Whole_Image , 0 , SDL_ALPHA_OPAQUE );
      
      for ( i=0 ; i < NUM_MAP_BLOCKS ; i++ )
	{
	  tmp_surf = SDL_CreateRGBSurface( 0 , Block_Width, Block_Height, vid_bpp, 0, 0, 0, 0);
	  SDL_SetColorKey( tmp_surf , 0 , 0 ); // this should clear any color key in the source surface
	  MapBlockSurfacePointer[ color ][i] = SDL_DisplayFormat( tmp_surf ); // now we have an alpha-surf of right size
	  SDL_SetColorKey( MapBlockSurfacePointer[ color ][i] , 0 , 0 ); // this should clear any color key in the dest surface
	  // Now we can copy the image Information
	  Source.x=(i%9)*(Block_Height+2);
	  Source.y=(i/9)*(Block_Width+2);
	  Source.w=Block_Width;
	  Source.h=Block_Height;
	  Target.x=0;
	  Target.y=0;
	  Target.w=Block_Width;
	  Target.h=Block_Height;
	  SDL_BlitSurface ( Whole_Image , &Source , MapBlockSurfacePointer[ color ][i] , &Target );
	  SDL_SetAlpha( MapBlockSurfacePointer[ color ][i] , 0 , 0 );
	}
      SDL_FreeSurface( tmp_surf );
    }
}; // void Load_MapBlock_Surfaces( void )

#undef _blocks_c
