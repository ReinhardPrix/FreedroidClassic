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
 * This file contains block operating functions, that is when you want to
 * put something on the visible screen, DO NOT DO IT YOURSELF!  Use one of 
 * the functions in here.  These functions already take into acount the 
 * position of the player character so you do not have to worry about 
 * where to put anything, and only have to supply map coordinates.  
 * Very handy.
 * ---------------------------------------------------------------------- */

#define _blocks_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"

/* ----------------------------------------------------------------------
 * This function loads the Blast image and decodes it into the multiple
 * small Blast surfaces.
 * ---------------------------------------------------------------------- */
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
	  SDL_FreeSurface( tmp_surf );
	}
    }
  SDL_FreeSurface( Whole_Image );

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
      SDL_FreeSurface( tmp_surf );
    }
  SDL_FreeSurface( Whole_Image );
}; // void Load_Item_Surfaces( void )

/* ----------------------------------------------------------------------
 * This function loads the items image and decodes it into the multiple
 * small item surfaces.
 * ---------------------------------------------------------------------- */
void 
Load_Mouse_Move_Cursor_Surfaces( void )
{
  SDL_Surface* Whole_Image;
  SDL_Surface* tmp_surf;
  SDL_Rect Source;
  SDL_Rect Target;
  int i=0;
  int j;
  char *fpath;

  fpath = find_file ( MOUSE_CURSOR_BLOCK_FILE , GRAPHICS_DIR, TRUE);

  Whole_Image = IMG_Load( fpath ); // This is a surface with alpha channel, since the picture is one of this type
  SDL_SetAlpha( Whole_Image , 0 , SDL_ALPHA_OPAQUE );

  for ( j=0 ; j < NUMBER_OF_MOUSE_CURSOR_PICTURES ; j++ )
    {
      Source.x = j * ( Block_Height + 2 );
      Source.y = i * ( Block_Width  + 2 );
      Source.w = Block_Width ;
      Source.h = Block_Height ;
      Target.x = 0;
      Target.y = 0;
      Target.w = Source.w;
      Target.h = Source.h;
      tmp_surf = SDL_CreateRGBSurface( 0 , Source.w , Source.h , vid_bpp , 0 , 0 , 0 , 0 );
      SDL_SetColorKey( tmp_surf , 0 , 0 ); // this should clear any color key in the source surface
      MouseCursorImageList[ j ] = SDL_DisplayFormatAlpha( tmp_surf ); // now we have an alpha-surf of right size
      SDL_SetColorKey( MouseCursorImageList[ j ] , 0 , 0 ); // this should clear any color key in the dest surface
      // Now we can copy the image Information
      SDL_BlitSurface ( Whole_Image , &Source , MouseCursorImageList[ j ] , &Target );
      SDL_SetAlpha( MouseCursorImageList[ j ] , SDL_SRCALPHA , SDL_ALPHA_OPAQUE );
      SDL_FreeSurface( tmp_surf );
    }
  SDL_FreeSurface( Whole_Image );
}; // void Load_Mouse_Move_Cursor_Surfaces( void )

/* ----------------------------------------------------------------------
 * This function loads the image containing the different buttons for the
 * different skills in the skill book of the Tux.
 * ---------------------------------------------------------------------- */
void 
Load_Skill_Level_Button_Surfaces( void )
{
  SDL_Surface* Whole_Image;
  SDL_Surface* tmp_surf;
  SDL_Rect Source;
  SDL_Rect Target;
  int i=0;
  int j;
  char *fpath;

  fpath = find_file ( SKILL_LEVEL_BUTTON_FILE , GRAPHICS_DIR, TRUE);

  Whole_Image = IMG_Load( fpath ); // This is a surface with alpha channel, since the picture is one of this type
  SDL_SetAlpha( Whole_Image , 0 , SDL_ALPHA_OPAQUE );

  for ( j=0 ; j < NUMBER_OF_SKILL_LEVELS ; j++ )
    {
      Source.x = j * ( SKILL_LEVEL_BUTTON_WIDTH );
      Source.y = i * ( SKILL_LEVEL_BUTTON_HEIGHT );
      Source.w = SKILL_LEVEL_BUTTON_WIDTH ;
      Source.h = SKILL_LEVEL_BUTTON_HEIGHT ;
      Target.x = 0;
      Target.y = 0;
      Target.w = Source.w;
      Target.h = Source.h;
      tmp_surf = SDL_CreateRGBSurface( 0 , Source.w , Source.h , vid_bpp , 0 , 0 , 0 , 0 );
      SDL_SetColorKey( tmp_surf , 0 , 0 ); // this should clear any color key in the source surface
      SpellLevelButtonImageList[ j ] = SDL_DisplayFormatAlpha( tmp_surf ); // now we have an alpha-surf of right size
      SDL_SetColorKey( SpellLevelButtonImageList[ j ] , 0 , 0 ); // this should clear any color key in the dest surface
      // Now we can copy the image Information
      SDL_BlitSurface ( Whole_Image , &Source , SpellLevelButtonImageList[ j ] , &Target );
      SDL_SetAlpha( SpellLevelButtonImageList[ j ] , SDL_SRCALPHA , SDL_ALPHA_OPAQUE );
      SDL_FreeSurface( tmp_surf );
    }
  SDL_FreeSurface( Whole_Image );
}; // void Load_Skill_Level_Button_Surfaces( void )

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
  int RowTop=0;
  char *fpath;
  int BulletImageHeightTable[30];
  
  BulletImageHeightTable[  0 ] = Block_Height;
  BulletImageHeightTable[  1 ] = Block_Height;
  BulletImageHeightTable[  2 ] = Block_Height;
  BulletImageHeightTable[  3 ] = Block_Height;
  BulletImageHeightTable[  4 ] = Block_Height;
  BulletImageHeightTable[  5 ] = Block_Height;
  BulletImageHeightTable[  6 ] = Block_Height;
  BulletImageHeightTable[  7 ] = Block_Height;
  BulletImageHeightTable[  8 ] = Block_Height * 2 ;
  BulletImageHeightTable[  9 ] = Block_Height;
  BulletImageHeightTable[ 10 ] = Block_Height;
  BulletImageHeightTable[ 11 ] = Block_Height;
  BulletImageHeightTable[ 12 ] = Block_Height;
  BulletImageHeightTable[ 13 ] = Block_Height;
  BulletImageHeightTable[ 14 ] = Block_Height;
  BulletImageHeightTable[ 15 ] = Block_Height;
  BulletImageHeightTable[ 16 ] = Block_Height;
  BulletImageHeightTable[ 17 ] = Block_Height;
  BulletImageHeightTable[ 18 ] = Block_Height;
  BulletImageHeightTable[ 19 ] = Block_Height;
  BulletImageHeightTable[ 20 ] = Block_Height;

  fpath = find_file (NE_BULLET_BLOCK_FILE, GRAPHICS_DIR, TRUE);

  Whole_Image = IMG_Load( fpath ); // This is a surface with alpha channel, since the picture is one of this type
  SDL_SetAlpha( Whole_Image , 0 , SDL_ALPHA_OPAQUE );

  for ( i=0 ; i < Number_Of_Bullet_Types ; i++ )
    {
      for ( j=0 ; j < Bulletmap[i].phases ; j++ )
	{
	  tmp_surf = SDL_CreateRGBSurface( 0 , Block_Width, BulletImageHeightTable[ i ], vid_bpp, 0, 0, 0, 0);
	  SDL_SetColorKey( tmp_surf , 0 , 0 ); // this should clear any color key in the source surface
	  Bulletmap[i].SurfacePointer[j] = SDL_DisplayFormatAlpha( tmp_surf ); // now we have an alpha-surf of right size
	  SDL_SetColorKey( Bulletmap[i].SurfacePointer[j] , 0 , 0 ); // this should clear any color key in the dest surface
	  // Now we can copy the image Information
	  Source.x = j*(Block_Width+2);
	  // Source.y = i*(Block_Height+2);
	  Source.y = RowTop ;
	  Source.w = Block_Width;
	  Source.h = BulletImageHeightTable[ i ]; // Block_Height;
	  Target.x = 0;
	  Target.y = 0;
	  Target.w = 0; // Block_Width;
	  Target.h = 0; // Block_Height;
	  SDL_BlitSurface ( Whole_Image , &Source , Bulletmap[i].SurfacePointer[j] , &Target );
	  SDL_SetAlpha( Bulletmap[i].SurfacePointer[j] , SDL_SRCALPHA , SDL_ALPHA_OPAQUE );
	  SDL_FreeSurface( tmp_surf );
	}
      RowTop += BulletImageHeightTable[ i ] + 2;
    }
  SDL_FreeSurface( Whole_Image );
}; // void Load_Bullet_Surfaces( void )


/* ----------------------------------------------------------------------
 * This function creates all the surfaces, that are nescessary to blit the
 * skill icons somewhere on the screen, so that you know what skill is
 * currently set to active.
 * ---------------------------------------------------------------------- */
void 
Load_SkillIcon_Surfaces( void )
{
  SDL_Surface* Whole_Image;
  SDL_Surface* tmp_surf;
  SDL_Rect Source;
  SDL_Rect Target;
  int i;
  char *fpath;

  fpath = find_file ( SKILL_ICON_FILE , GRAPHICS_DIR, TRUE);

  Whole_Image = IMG_Load( fpath ); // This is a surface with alpha channel, since the picture is one of this type
  SDL_SetAlpha( Whole_Image , 0 , SDL_ALPHA_OPAQUE );

  for ( i=0 ; i < NUMBER_OF_SKILLS ; i++ )
    {
      tmp_surf = SDL_CreateRGBSurface( 0 , Block_Width, Block_Height, vid_bpp, 0, 0, 0, 0);
      SDL_SetColorKey( tmp_surf , 0 , 0 ); // this should clear any color key in the source surface
      SkillIconSurfacePointer[i] = SDL_DisplayFormatAlpha( tmp_surf ); // now we have an alpha-surf of right size
      SDL_SetColorKey( SkillIconSurfacePointer[i] , 0 , 0 ); // this should clear any color key in the dest surface
      // Now we can copy the image Information
      Source.x=i*(Block_Height+2);
      Source.y=0*(Block_Width+2);
      Source.w=Block_Width;
      Source.h=Block_Height;
      Target.x=0;
      Target.y=0;
      Target.w=Block_Width;
      Target.h=Block_Height;
      SDL_BlitSurface ( Whole_Image , &Source , SkillIconSurfacePointer[i] , &Target );
      SDL_SetAlpha( SkillIconSurfacePointer[i] , SDL_SRCALPHA , SDL_ALPHA_OPAQUE );
      SDL_FreeSurface( tmp_surf );
    }
  SDL_FreeSurface( Whole_Image );
}; // void Load_SkillIcon_Surfaces( void )


/* ----------------------------------------------------------------------
 * This function creates all the surfaces, that are nescessary to blit the
 * 'head' and 'shoes' of an enemy.  The numbers are not dealt with here.
 * ---------------------------------------------------------------------- */
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

  for ( i=0 ; i < DROID_PHASES + DEAD_DROID_PHASES ; i++ )
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
      SDL_FreeSurface( tmp_surf );
    }
  SDL_FreeSurface( Whole_Image );

  //--------------------
  // Now that we have our enemy surfaces ready, we can create some modified
  // copies of those surfaces but this a color filter applied to them...
  //
  for ( i=0 ; i < DROID_PHASES + DEAD_DROID_PHASES ; i++ )
    {
      BlueEnemySurfacePointer [ i ] = CreateColorFilteredSurface ( EnemySurfacePointer [ i ] , FILTER_BLUE );
      GreenEnemySurfacePointer [ i ] = CreateColorFilteredSurface ( EnemySurfacePointer [ i ] , FILTER_GREEN );
    }

}; // void LoadEnemySurfaces( void )


/* ----------------------------------------------------------------------
 * This function loads all the surfaces needed to display the classicla
 * 001 robot and all friendly robots too.
 * ---------------------------------------------------------------------- */
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

  for ( i=0 ; i < DROID_PHASES + DEAD_DROID_PHASES ; i++ )
    {
      tmp_surf = SDL_CreateRGBSurface( 0 , Block_Width, Block_Height, vid_bpp, 0, 0, 0, 0);
      SDL_SetColorKey( tmp_surf , 0 , 0 ); // this should clear any color key in the source surface
      InfluencerSurfacePointer[i] = SDL_DisplayFormatAlpha( tmp_surf ); // now we have an alpha-surf of right size
      DebugPrintf( 1 , "\nInfluencerSurfacePonter[%d] is now initialized..." , i );
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
      SDL_FreeSurface( tmp_surf );
    }


  SDL_FreeSurface( Whole_Image );
}; // void Load_Influencer_Surfaces( void )

/* ----------------------------------------------------------------------
 * This function loads the all tux surfaces, that are needed to display 
 * the alternative tux character.
 * ---------------------------------------------------------------------- */
void 
Homemade_Update_Tux_Working_Copy ( int PlayerNum )
{
  int i;
  static int Previous_weapon_item  [ MAX_PLAYERS ] = { -2 , -2 , -2 , -2 , -2 } ;
  static int Previous_shield_item  [ MAX_PLAYERS ] = { -2 , -2 , -2 , -2 , -2 } ;
  static int Previous_special_item [ MAX_PLAYERS ] = { -2 , -2 , -2 , -2 , -2 } ;
  static int Previous_armour_item  [ MAX_PLAYERS ] = { -2 , -2 , -2 , -2 , -2 } ; 
  SDL_Surface* tmp;

  if ( ( Previous_weapon_item  [ PlayerNum ] == Me [ PlayerNum ] . weapon_item. type ) &&
       ( Previous_shield_item  [ PlayerNum ] == Me [ PlayerNum ] . shield_item. type ) &&
       ( Previous_armour_item  [ PlayerNum ] == Me [ PlayerNum ] . armour_item.type  ) &&
       ( Previous_special_item [ PlayerNum ] == Me [ PlayerNum ] . special_item.type ) )
    {
      return;
    }
  else
    {
      Previous_weapon_item  [ PlayerNum ] = Me [ PlayerNum ] . weapon_item. type ;
      Previous_shield_item  [ PlayerNum ] = Me [ PlayerNum ] . shield_item. type ;
      Previous_armour_item  [ PlayerNum ] = Me [ PlayerNum ] . armour_item. type ;
      Previous_special_item [ PlayerNum ] = Me [ PlayerNum ] . special_item.type ; 
    }

  //--------------------
  // Since the assembling of the tux with the home made alpha merging code
  // takes considerable time, be must activate the conservative frame computation
  // here, or the player might jump through walls or something...
  //
  Activate_Conservative_Frame_Computation ( ) ;
    
  //--------------------
  // First we blit the bare chest and feet of the Tux.
  //
  for ( i = 0 ; i < TUX_GOT_HIT_PHASES + TUX_SWING_PHASES + TUX_BREATHE_PHASES ; i ++ )
    {
      SDL_FreeSurface ( TuxWorkingCopy [ PlayerNum ] [i] );
      TuxWorkingCopy [ PlayerNum ]  [ i ] = SDL_DisplayFormatAlpha( TuxMotionArchetypes[7][i] );
    }
  
  //--------------------
  // Now we blit the armour item directly over the bare chest and feet, 
  // if some armour item is equipped of course.
  //
  if ( Me[0].armour_item.type != (-1) ) 
    {
      for ( i = 0 ; i < TUX_GOT_HIT_PHASES + TUX_SWING_PHASES + TUX_BREATHE_PHASES ; i ++ )
	{
	  tmp = CreateAlphaCombinedSurface ( TuxWorkingCopy [ PlayerNum ] [i] , TuxMotionArchetypes[8][i] );
	  SDL_FreeSurface ( TuxWorkingCopy [ PlayerNum ] [i] );
	  TuxWorkingCopy [ PlayerNum ] [i] = tmp;
	}
    }

  //--------------------
  // Next we blit the weapon (and arms) of the tux
  //
  if ( ( Me[0].weapon_item.type == (-1) ) || ( Me[0].weapon_item.currently_held_in_hand ) )
    {
      for ( i = 0 ; i < TUX_GOT_HIT_PHASES + TUX_SWING_PHASES + TUX_BREATHE_PHASES ; i ++ )
	{
	  tmp = CreateAlphaCombinedSurface ( TuxWorkingCopy [ PlayerNum ] [i] , TuxMotionArchetypes[4][i] );
	  SDL_FreeSurface ( TuxWorkingCopy [ PlayerNum ] [i] );
	  TuxWorkingCopy [ PlayerNum ] [i] = tmp;
	}
    }
  else if ( Me[0].weapon_item.type == ITEM_STAFF )
    {
      for ( i = 0 ; i < TUX_GOT_HIT_PHASES + TUX_SWING_PHASES + TUX_BREATHE_PHASES ; i ++ )
	{
	  tmp = CreateAlphaCombinedSurface ( TuxWorkingCopy [ PlayerNum ] [i] , TuxMotionArchetypes[1][i] );
	  SDL_FreeSurface ( TuxWorkingCopy [ PlayerNum ] [i] );
	  TuxWorkingCopy [ PlayerNum ] [i] = tmp;
	}
    }
  else if ( ItemMap [ Me[0].weapon_item.type ].item_gun_angle_change == 0 )
    {
      for ( i = 0 ; i < TUX_GOT_HIT_PHASES + TUX_SWING_PHASES + TUX_BREATHE_PHASES ; i ++ )
	{
	  tmp = CreateAlphaCombinedSurface ( TuxWorkingCopy [ PlayerNum ] [i] , TuxMotionArchetypes[2][i] );
	  SDL_FreeSurface ( TuxWorkingCopy [ PlayerNum ] [i] );
	  TuxWorkingCopy [ PlayerNum ] [i] = tmp;
	}
    }
  else
    {
      for ( i = 0 ; i < TUX_GOT_HIT_PHASES + TUX_SWING_PHASES + TUX_BREATHE_PHASES ; i ++ )
	{
	  tmp = CreateAlphaCombinedSurface ( TuxWorkingCopy [ PlayerNum ] [i] , TuxMotionArchetypes[0][i] );
	  SDL_FreeSurface ( TuxWorkingCopy [ PlayerNum ] [i] );
	  TuxWorkingCopy [ PlayerNum ] [i] = tmp;
	}
    }

  //--------------------
  // Now we blit the shields OVER it.
  //
  if ( ( Me[0].shield_item.type != (-1) ) && ( ! Me[0].shield_item.currently_held_in_hand ) )
    {
      for ( i = 0 ; i < TUX_GOT_HIT_PHASES + TUX_SWING_PHASES + TUX_BREATHE_PHASES ; i ++ )
	{
	  tmp = CreateAlphaCombinedSurface ( TuxWorkingCopy [ PlayerNum ] [i] , TuxMotionArchetypes[3][i] );
	  SDL_FreeSurface ( TuxWorkingCopy [ PlayerNum ] [i] );
	  TuxWorkingCopy [ PlayerNum ] [i] = tmp;
	}
    }

  //--------------------
  // Now as the last part, we blit the head OVER the rest and than the hat OVER it all.
  //
  for ( i = 0 ; i < TUX_GOT_HIT_PHASES + TUX_SWING_PHASES + TUX_BREATHE_PHASES ; i ++ )
    {
      tmp = CreateAlphaCombinedSurface ( TuxWorkingCopy [ PlayerNum ] [i] , TuxMotionArchetypes[6][i] );
      SDL_FreeSurface ( TuxWorkingCopy [ PlayerNum ] [i] );
      TuxWorkingCopy [ PlayerNum ] [i] = tmp;
    }

  if ( ( ( Me[0].special_item.type == ( ITEM_IRON_HAT ) ) || ( Me[0].special_item.type == ( ITEM_SMALL_HELM ) ) ) && ( ! Me[0].special_item.currently_held_in_hand ) )
    {
      for ( i = 0 ; i < TUX_GOT_HIT_PHASES + TUX_SWING_PHASES + TUX_BREATHE_PHASES ; i ++ )
	{
	  tmp = CreateAlphaCombinedSurface ( TuxWorkingCopy [ PlayerNum ] [i] , TuxMotionArchetypes[9][i] );
	  SDL_FreeSurface ( TuxWorkingCopy [ PlayerNum ] [i] );
	  TuxWorkingCopy [ PlayerNum ] [i] = tmp;
	}
    }
  else if ( ( Me[0].special_item.type == ITEM_IRON_HELM ) && ( ! Me[0].special_item.currently_held_in_hand ) )
    {
      for ( i = 0 ; i < TUX_GOT_HIT_PHASES + TUX_SWING_PHASES + TUX_BREATHE_PHASES ; i ++ )
	{
	  tmp = CreateAlphaCombinedSurface ( TuxWorkingCopy [ PlayerNum ] [i] , TuxMotionArchetypes[10][i] );
	  SDL_FreeSurface ( TuxWorkingCopy [ PlayerNum ] [i] );
	  TuxWorkingCopy [ PlayerNum ] [i] = tmp;
	}
    }
  else if ( ( Me[0].special_item.type != (-1) ) && ( ! Me[0].special_item.currently_held_in_hand ) )
    {
      for ( i = 0 ; i < TUX_GOT_HIT_PHASES + TUX_SWING_PHASES + TUX_BREATHE_PHASES ; i ++ )
	{
	  tmp = CreateAlphaCombinedSurface ( TuxWorkingCopy [ PlayerNum ] [i] , TuxMotionArchetypes[5][i] );
	  SDL_FreeSurface ( TuxWorkingCopy [ PlayerNum ] [i] );
	  TuxWorkingCopy [ PlayerNum ] [i] = tmp;
	}
    }
  
}; // void Homemade_Update_Tux_Working_Copy ( void )

/* ----------------------------------------------------------------------
 * This function loads the all tux surfaces, that are needed to display 
 * the alternative tux character.
 * ---------------------------------------------------------------------- */
void 
Load_Tux_Surfaces( void )
{
  SDL_Surface* Whole_Image;
  SDL_Surface* tmp_surf;
  SDL_Rect Source;
  SDL_Rect Target;
  int i;
  char *fpath;
  int j;
  int PlayerNum;

#define TUX_WIDTH 130
#define TUX_HEIGHT 130


  fpath = find_file ( NE_DROID_BLOCK_FILE , GRAPHICS_DIR, TRUE);

  Whole_Image = IMG_Load( fpath ); // This is a surface with alpha channel, since the picture is one of this type
  SDL_SetAlpha( Whole_Image , 0 , SDL_ALPHA_OPAQUE ); // this should 
  SDL_SetColorKey( Whole_Image , 0 , 0 ); // this should clear any color key in the source surface

  for ( j = 0 ; j < TUX_MODELS ; j ++ )
    {

      ShowStartupPercentage ( 30 + j * 4 ) ; 

      for ( i=0 ; i < TUX_GOT_HIT_PHASES + TUX_SWING_PHASES + TUX_BREATHE_PHASES ; i++ )
	{
	  tmp_surf = SDL_CreateRGBSurface( 0 , TUX_WIDTH , TUX_HEIGHT , vid_bpp , 0 , 0 , 0 , 0 );
	  SDL_SetColorKey( tmp_surf , 0 , 0 ); // this should clear any color key in the source surface
	  TuxMotionArchetypes[j][i] = SDL_DisplayFormatAlpha( tmp_surf ); // now we have an alpha-surf of right size

	  SDL_SetColorKey ( TuxMotionArchetypes[j][i] , SDL_SRCCOLORKEY, 
			    SDL_MapRGB( TuxMotionArchetypes[j][i]->format, 255, 0, 255) ); 
	  // Now we can copy the image Information
	  Source.x=i*( TUX_WIDTH  + 2 );
	  Source.y=( 1 + j ) * ( TUX_HEIGHT + 2 ) ;
	  Source.w=TUX_WIDTH;
	  Source.h=TUX_HEIGHT;
	  Target.x=0;
	  Target.y=0;
	  Target.w=Block_Width;
	  Target.h=Block_Height;
	  SDL_BlitSurface ( Whole_Image , &Source , TuxMotionArchetypes[j][i] , &Target );
	  SDL_SetAlpha( TuxMotionArchetypes[j][i] , SDL_SRCALPHA , SDL_ALPHA_OPAQUE );
	  SDL_FreeSurface( tmp_surf );

	  //--------------------
	  // And at this point, we also initialize the Tux working copys, so we
	  // won't run into trouble with uninitialized working copys later.
	  //
	  if ( j == 7 ) 
	    {
	      for ( PlayerNum = 0 ; PlayerNum < MAX_PLAYERS ; PlayerNum ++ )
		TuxWorkingCopy [ PlayerNum ] [i] = SDL_DisplayFormatAlpha( TuxMotionArchetypes[j][i] );
	    }

	}
    }


  SDL_FreeSurface( Whole_Image );
}; // void Load_Tux_Surfaces( void )

/* ----------------------------------------------------------------------
 * This function creates all the surfaces, that are nescessary to store
 * the big map inserts, that might add some spice to the otherwise rather
 * monotonous rooms and halls of the freedroid maps.
 * ---------------------------------------------------------------------- */
void 
Load_Big_Map_Insert_Surfaces( void )
{
  int i ;
  char *fpath;
  SDL_Surface* TempSurface;

  for ( i = 0 ; i < MAX_MAP_INSERTS ; i ++ )
    {

      //--------------------
      // Now we try to load the surface and then we also convert it
      // immediately to display format, so it can be blittet later and
      // at maximum speed!!!
      //
      fpath = find_file ( AllMapInserts [ i ] . map_insert_file_name , GRAPHICS_DIR , FALSE );
      TempSurface = IMG_Load( fpath ) ;
      if ( TempSurface == 0 )
	{
	  fprintf( stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
Freedroid was unable to load a big graphics insert from the hard disk\n\
into memory.\n\
\n\
The full path name of the file, that could not be loaded was : \n\
%s\n\
\n\
This error indicates some installation problem with freedroid.\n\
Please contact the developers, as always freedroid-discussion@lists.sourceforge.net.\n\
Thanks a lot.\n\
\n\
But for now Freedroid will terminate to draw attention \n\
to the graphics loading problem it could not resolve.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" , fpath );
	  Terminate(ERR);


	}
      AllMapInserts [ i ] . insert_surface = SDL_DisplayFormat ( TempSurface ) ;
      SDL_FreeSurface ( TempSurface ) ;

      //--------------------
      // Now we check if the file has been loaded successfully, or otherwise
      // we print out an error message...
      //
      if ( ! AllMapInserts [ i ] . insert_surface )
	{
	  fprintf( stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
Freedroid was unable to load a big graphics insert from the hard disk\n\
into memory.\n\
\n\
The full path name of the file, that could not be loaded was : \n\
%s\n\
\n\
This error indicates some installation problem with freedroid.\n\
Please contact the developers, as always freedroid-discussion@lists.sourceforge.net.\n\
Thanks a lot.\n\
\n\
But for now Freedroid will terminate to draw attention \n\
to the graphics loading problem it could not resolve.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" , fpath );
	  Terminate(ERR);

	}

    }
	  
}; // void Load_Big_Map_Insert_Surfaces( void )

/* ----------------------------------------------------------------------
 * This function creates all the surfaces, that are nescessary to blit a
 * digit.
 * ---------------------------------------------------------------------- */
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
      SDL_FreeSurface( tmp_surf );
    }

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
      SDL_FreeSurface( tmp_surf );
    }
  SDL_FreeSurface( Whole_Image );
}; // void Load_Digit_Surfaces( void )

/* ----------------------------------------------------------------------
 * This function creates all the surfaces, that are nescessary to blit
 * some map tiles of any color.
 * ---------------------------------------------------------------------- */
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

  char *fname = "map_blocks.png";

  Block_Width=INITIAL_BLOCK_WIDTH;
  Block_Height=INITIAL_BLOCK_HEIGHT;
  
  fpath = find_file ( fname, GRAPHICS_DIR, TRUE);
  for ( color = 0 ; color < NUM_COLORS ; color ++ )
    {
      Whole_Image = IMG_Load( fpath ); // This is a surface with alpha channel, since the picture is one of this type
      SDL_SetAlpha( Whole_Image , 0 , SDL_ALPHA_OPAQUE );
      
      for ( i=0 ; i < NUM_MAP_BLOCKS ; i++ )
	{
	  tmp_surf = SDL_CreateRGBSurface( 0 , Block_Width, Block_Height, vid_bpp, 0, 0, 0, 0);
	  SDL_SetColorKey( tmp_surf , 0 , 0 ); // this should clear any color key in the source surface
	  MapBlockSurfacePointer[ color ][i] = SDL_DisplayFormat( tmp_surf ); // now we have an alpha-surf of right size
	  SDL_SetColorKey( MapBlockSurfacePointer[ color ][i] , 0 , 0 ); // this should clear any color key in the dest surface
	  // Now we can copy the image Information
	  Source.x=i*(Block_Height+2);
	  Source.y=color*(Block_Width+2);
	  Source.w=Block_Width;
	  Source.h=Block_Height;
	  Target.x=0;
	  Target.y=0;
	  Target.w=Block_Width;
	  Target.h=Block_Height;
	  SDL_BlitSurface ( Whole_Image , &Source , MapBlockSurfacePointer[ color ][i] , &Target );
	  SDL_SetAlpha( MapBlockSurfacePointer[ color ][i] , 0 , 0 );
	  SDL_FreeSurface( tmp_surf );
	}
      SDL_FreeSurface( Whole_Image );
    }
}; // void Load_MapBlock_Surfaces( void )

#undef _blocks_c
