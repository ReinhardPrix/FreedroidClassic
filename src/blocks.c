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
#include "SDL_rotozoom.h"

char *PrefixToFilename[ ENEMY_ROTATION_MODELS_AVAILABLE ];
int ModelMultiplier[ ENEMY_ROTATION_MODELS_AVAILABLE ];

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

  tmp_surf = SDL_CreateRGBSurface( 0 , Block_Width, Block_Height, vid_bpp, 0, 0, 0, 0);
  SDL_SetColorKey( tmp_surf , 0 , 0 ); // this should clear any color key in the source surface

  for ( i=0 ; i < ALLBLASTTYPES ; i++ )
    {
      for ( j=0 ; j < Blastmap[i].phases ; j++ )
	{
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

  tmp_surf = SDL_CreateRGBSurface( 0 , Block_Width , Block_Height , vid_bpp , 0 , 0 , 0 , 0 );
  SDL_SetColorKey( tmp_surf , 0 , 0 ); // this should clear any color key in the source surface

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

      MouseCursorImageList[ j ] = SDL_DisplayFormatAlpha( tmp_surf ); // now we have an alpha-surf of right size
      SDL_SetColorKey( MouseCursorImageList[ j ] , 0 , 0 ); // this should clear any color key in the dest surface
      // Now we can copy the image Information
      SDL_BlitSurface ( Whole_Image , &Source , MouseCursorImageList[ j ] , &Target );
      SDL_SetAlpha( MouseCursorImageList[ j ] , SDL_SRCALPHA , SDL_ALPHA_OPAQUE );
    }

  SDL_FreeSurface( tmp_surf );
  SDL_FreeSurface( Whole_Image );

}; // void Load_Mouse_Move_Cursor_Surfaces( void )

/* ----------------------------------------------------------------------
 * This function loads the image containing the different buttons for the
 * different skills in the skill book of the Tux.
 * ---------------------------------------------------------------------- */
void 
Load_Skill_Level_Button_Surfaces( void )
{
  static int SkillLevelButtonsAreAlreadyLoaded = FALSE;
  SDL_Surface* Whole_Image;
  SDL_Surface* tmp_surf;
  SDL_Rect Source;
  SDL_Rect Target;
  int i=0;
  int j;
  char *fpath;

  //--------------------
  // Maybe this function has been called before.  Then we do not
  // need to do anything (again) here and can just return.
  //
  if ( SkillLevelButtonsAreAlreadyLoaded ) return;

  //--------------------
  // Now we proceed to load all the skill circle buttons.
  //
  fpath = find_file ( SKILL_LEVEL_BUTTON_FILE , GRAPHICS_DIR, TRUE);

  Whole_Image = IMG_Load( fpath ); // This is a surface with alpha channel, since the picture is one of this type
  SDL_SetAlpha( Whole_Image , 0 , SDL_ALPHA_OPAQUE );

  tmp_surf = SDL_CreateRGBSurface( 0 , SKILL_LEVEL_BUTTON_WIDTH , SKILL_LEVEL_BUTTON_HEIGHT , 
				   vid_bpp , 0 , 0 , 0 , 0 );
  SDL_SetColorKey( tmp_surf , 0 , 0 ); // this should clear any color key in the source surface

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

      SpellLevelButtonImageList[ j ] = SDL_DisplayFormatAlpha( tmp_surf ); // now we have an alpha-surf of right size
      SDL_SetColorKey( SpellLevelButtonImageList[ j ] , 0 , 0 ); // this should clear any color key in the dest surface
      // Now we can copy the image Information
      SDL_BlitSurface ( Whole_Image , &Source , SpellLevelButtonImageList[ j ] , &Target );
      SDL_SetAlpha( SpellLevelButtonImageList[ j ] , SDL_SRCALPHA , SDL_ALPHA_OPAQUE );
    }

  SDL_FreeSurface( tmp_surf );
  SDL_FreeSurface( Whole_Image );

  SkillLevelButtonsAreAlreadyLoaded = TRUE; 

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
      tmp_surf = SDL_CreateRGBSurface( 0 , Block_Width, BulletImageHeightTable[ i ], vid_bpp, 0, 0, 0, 0);
      SDL_SetColorKey( tmp_surf , 0 , 0 ); // this should clear any color key in the source surface

      for ( j=0 ; j < Bulletmap[i].phases ; j++ )
	{
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
	  // SDL_BlitSurface ( Whole_Image , &Source , Bulletmap[i].SurfacePointer[j] , &Target );
	  SDL_BlitSurface ( Whole_Image , &Source , Bulletmap[i].SurfacePointer[j] , NULL );
	  SDL_SetAlpha( Bulletmap[i].SurfacePointer[j] , SDL_SRCALPHA , SDL_ALPHA_OPAQUE );
	}
      RowTop += BulletImageHeightTable[ i ] + 2;
      SDL_FreeSurface( tmp_surf );
    }
  SDL_FreeSurface( Whole_Image );

}; // void Load_Bullet_Surfaces( void )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
LoadOneSkillSurfaceIfNotYetLoaded ( int SkillSpellNr )
{
  SDL_Surface* Whole_Image;
  char *fpath;
  char AssembledFileName [ 2000 ] ;

  //--------------------
  // Maybe this spell/skill icon surface has already been loaded, i.e. it's not
  // NULL any more.  Then we needn't do anything here.
  //
  if ( SpellSkillMap [ SkillSpellNr ] . spell_skill_icon_surface ) return;

  //--------------------
  // Now it's time to assemble the file name to get the image from
  //
  strcpy ( AssembledFileName , "skill_icons/" );
  strcat ( AssembledFileName , SpellSkillMap [ SkillSpellNr ] . spell_skill_icon_name );
  fpath = find_file ( AssembledFileName , GRAPHICS_DIR, FALSE );

  //--------------------
  // Now we can load and prepare the image and that's it
  //
  Whole_Image = IMG_Load( fpath ); // This is a surface with alpha channel, since the picture is one of this type
  if ( !Whole_Image )
    {
      fprintf ( stderr , "\nfpath=%s." , fpath );
      GiveStandardErrorMessage ( "LoadOneSkillSurfaceIfNotYetLoaded(...)" , "\
Freedroid was unable to load a certain skill surface into memory.\n\
This error indicates some installation problem with freedroid.",
				 PLEASE_INFORM, IS_FATAL );
    }

  SpellSkillMap [ SkillSpellNr ] . spell_skill_icon_surface = SDL_DisplayFormatAlpha( Whole_Image ); 

  SDL_SetColorKey( SpellSkillMap [ SkillSpellNr ] . spell_skill_icon_surface , 0 , 0 ); 
  SDL_SetAlpha( SpellSkillMap [ SkillSpellNr ] . spell_skill_icon_surface , SDL_SRCALPHA , SDL_ALPHA_OPAQUE );

  SDL_FreeSurface( Whole_Image );
  
}; // void LoadOneSkillSurfaceIfNotYetLoaded ( int SkillSpellNr )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void 
LoadAndPrepareEnemyRotationModelNr ( int ModelNr )
{
  SDL_Surface* Whole_Image;
  char ConstructedFileName[5000];
  int i;
  char *fpath;
  static int FirstCallEver = TRUE ;
  static int EnemyFullyPrepared [ ENEMY_ROTATION_MODELS_AVAILABLE ] ;

  //--------------------
  // Maybe this function has just been called for the first time ever.
  // Then of course we need to initialize the array, that is used for
  // keeping track of the currently loaded enemy rotation surfaces.
  // This we do here.
  //
  if ( FirstCallEver )
    {
      for ( i = 0 ; i < ENEMY_ROTATION_MODELS_AVAILABLE ; i ++ )
	{
	  EnemyFullyPrepared [ i ] = FALSE ;
	}
      FirstCallEver = FALSE ;
    }

  //--------------------
  // Now a sanity check against using rotation types, that don't exist
  // in Freedroid RPG at all!
  //
  if ( ( ModelNr < 0 ) || ( ModelNr >= ENEMY_ROTATION_MODELS_AVAILABLE ) )
    {
      fprintf ( stderr , "\n\nModelNr=%d.\n\n" , ModelNr );
      GiveStandardErrorMessage ( "LoadAndPrepareEnemyRotationModelNr(...)" , "\
Freedroid received a rotation model number that does not exist!",
				 PLEASE_INFORM, IS_FATAL );
    }

  //--------------------
  // Now we can check if the given rotation model type was perhaps already
  // allocated and loaded and fully prepared.  Then of course we need not 
  // do anything here...  Otherwise we can have trust and mark it as loaded
  // already...
  //
  if ( EnemyFullyPrepared [ ModelNr ] ) return;
  EnemyFullyPrepared [ ModelNr ] = TRUE;
  Activate_Conservative_Frame_Computation();

  //--------------------
  // Now that we have the classic ball-shaped design completely done,
  // we can start doing something new:  Let's try to use some pre-rotated
  // enemy surfaces for a change.  That might work out to be cool.
  //
  for ( i=0 ; i < ROTATION_ANGLES_PER_ROTATION_MODEL ; i++ )
    {
      sprintf ( ConstructedFileName , "rotation_models/%s_%04d.png" , PrefixToFilename [ ModelNr ] , 
		( ModelMultiplier [ ModelNr ] * i ) + 1 );
      DebugPrintf ( 1 , "\nConstructedFileName = %s " , ConstructedFileName );
      // fpath = find_file ( "rotation_models/anim0001.png" , GRAPHICS_DIR, FALSE );
      fpath = find_file ( ConstructedFileName , GRAPHICS_DIR, FALSE );
      
      Whole_Image = IMG_Load( fpath ); // This is a surface with alpha channel, since the picture is one of this type
      if ( Whole_Image == NULL )
	{
	  fprintf( stderr, "\n\nfpath: '%s'\n" , fpath );
	  GiveStandardErrorMessage ( "LoadAndPrepareEnemyRotationModelNr(...)" , "\
Freedroid was unable to load a rotated image of a droid into memory.\n\
This error indicates some installation problem with freedroid.",
				     PLEASE_INFORM, IS_FATAL );
	}
      
      SDL_SetAlpha( Whole_Image , 0 , SDL_ALPHA_OPAQUE );
      
      EnemyRotationSurfacePointer [ ModelNr ] [ i ] = SDL_DisplayFormatAlpha( Whole_Image ); // now we have an alpha-surf of right size
      SDL_SetColorKey( EnemyRotationSurfacePointer [ ModelNr ] [ i ] , 0 , 0 ); // this should clear any color key in the dest surface
      
      SDL_FreeSurface( Whole_Image );
  
    }    

  //--------------------
  // Now that we have our enemy surfaces ready, we can create some modified
  // copies of those surfaces but this a color filter applied to them...
  //
  for ( i=0 ; i < ROTATION_ANGLES_PER_ROTATION_MODEL ; i++ )
    {
      BlueEnemyRotationSurfacePointer [ ModelNr ] [ i ] = 
	CreateColorFilteredSurface ( EnemyRotationSurfacePointer [ ModelNr ] [ i ] , FILTER_BLUE );
      GreenEnemyRotationSurfacePointer [ ModelNr ] [ i ] = 
	CreateColorFilteredSurface ( EnemyRotationSurfacePointer [ ModelNr ] [ i ] , FILTER_GREEN );
      RedEnemyRotationSurfacePointer [ ModelNr ] [ i ] = 
	CreateColorFilteredSurface ( EnemyRotationSurfacePointer [ ModelNr ] [ i ] , FILTER_RED );
    }

}; // void LoadAndPrepareEnemyRotationModelNr ( int j )
  
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
  int j;
  char *fpath;

  //--------------------
  // We clean out the rotated enemy surface pointers, so that later we
  // can judge securely which of them have been initialized (non-Null)
  // and which of them have not.
  //
  for ( j = 0 ; j < ENEMY_ROTATION_MODELS_AVAILABLE ; j ++ )
    {
      for ( i=0 ; i < ROTATION_ANGLES_PER_ROTATION_MODEL ; i++ )
	{
	  EnemyRotationSurfacePointer[j][i] = NULL ;
	}
    }

  //--------------------
  // This needs to be initialized once, and this just seems a good place
  // to do this, so we can use the i++ syntax.
  //
  i=0;
  PrefixToFilename [ i ] = "001" ; // 0
  ModelMultiplier  [ i ] = 1 ; i++;
  PrefixToFilename [ i ] = "123" ; 
  ModelMultiplier  [ i ] = 1 ; i++;
  PrefixToFilename [ i ] = "139" ;
  ModelMultiplier  [ i ] = 1 ;i++;
  PrefixToFilename [ i ] = "247" ;
  ModelMultiplier  [ i ] = 1 ;i++;
  PrefixToFilename [ i ] = "249" ;
  ModelMultiplier  [ i ] = 1 ;i++;
  PrefixToFilename [ i ] = "296" ;
  ModelMultiplier  [ i ] = 1 ;i++;
  PrefixToFilename [ i ] = "302" ;
  ModelMultiplier  [ i ] = 1 ;i++;
  PrefixToFilename [ i ] = "329" ;
  ModelMultiplier  [ i ] = 1 ;i++;
  PrefixToFilename [ i ] = "420" ;
  ModelMultiplier  [ i ] = 1 ;i++;
  PrefixToFilename [ i ] = "476" ;
  ModelMultiplier  [ i ] = 1 ;i++;
  PrefixToFilename [ i ] = "493" ; // 10
  ModelMultiplier  [ i ] = 1 ;i++; 
  PrefixToFilename [ i ] = "516" ; 
  ModelMultiplier  [ i ] = 1 ;i++;
  PrefixToFilename [ i ] = "571" ;
  ModelMultiplier  [ i ] = 1 ;i++;
  PrefixToFilename [ i ] = "598" ;
  ModelMultiplier  [ i ] = 1 ;i++;
  PrefixToFilename [ i ] = "614" ;
  ModelMultiplier  [ i ] = 1 ;i++;
  PrefixToFilename [ i ] = "615" ;
  ModelMultiplier  [ i ] = 1 ;i++;
  PrefixToFilename [ i ] = "629" ;
  ModelMultiplier  [ i ] = 1 ;i++;
  PrefixToFilename [ i ] = "711" ;
  ModelMultiplier  [ i ] = 1 ;i++;
  PrefixToFilename [ i ] = "742" ;
  ModelMultiplier  [ i ] = 1 ;i++;
  PrefixToFilename [ i ] = "751" ;
  ModelMultiplier  [ i ] = 1 ;i++;
  PrefixToFilename [ i ] = "821" ; // 20
  ModelMultiplier  [ i ] = 1 ;i++;
  PrefixToFilename [ i ] = "834" ; 
  ModelMultiplier  [ i ] = 1 ;i++;
  PrefixToFilename [ i ] = "883" ;
  ModelMultiplier  [ i ] = 1 ;i++;
  PrefixToFilename [ i ] = "999" ;
  ModelMultiplier  [ i ] = 1 ;i++;
  PrefixToFilename [ i ] = "proffa" ;
  ModelMultiplier  [ i ] = 1 ;i++;
  PrefixToFilename [ i ] = "red_guard" ;
  ModelMultiplier  [ i ] = 1 ;i++;
  PrefixToFilename [ i ] = "brown_worker" ;
  ModelMultiplier  [ i ] = 1 ;i++;

  fpath = find_file ( BALL_SHAPED_DROIDS_FILE , GRAPHICS_DIR, TRUE);

  Whole_Image = IMG_Load( fpath ); // This is a surface with alpha channel, since the picture is one of this type
  SDL_SetAlpha( Whole_Image , 0 , SDL_ALPHA_OPAQUE );

  tmp_surf = SDL_CreateRGBSurface( 0 , Block_Width, Block_Height, vid_bpp, 0, 0, 0, 0);
  SDL_SetColorKey( tmp_surf , 0 , 0 ); // this should clear any color key in the source surface

  for ( i=0 ; i < DROID_PHASES + DEAD_DROID_PHASES ; i++ )
    {
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

  SDL_FreeSurface( Whole_Image );
  SDL_FreeSurface( tmp_surf );

  //--------------------
  // Now that we have our enemy surfaces ready, we can create some modified
  // copies of those surfaces but this a color filter applied to them...
  //
  for ( i=0 ; i < DROID_PHASES + DEAD_DROID_PHASES ; i++ )
    {
      BlueEnemySurfacePointer [ i ] = CreateColorFilteredSurface ( EnemySurfacePointer [ i ] , FILTER_BLUE );
      GreenEnemySurfacePointer [ i ] = CreateColorFilteredSurface ( EnemySurfacePointer [ i ] , FILTER_GREEN );
      RedEnemySurfacePointer [ i ] = CreateColorFilteredSurface ( EnemySurfacePointer [ i ] , FILTER_RED );
    }

  for ( j = 0 ; j < ENEMY_ROTATION_MODELS_AVAILABLE ; j ++ )
    {
      // LoadAndPrepareEnemyRotationModelNr ( j );
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

  fpath = find_file ( BALL_SHAPED_DROIDS_FILE , GRAPHICS_DIR, TRUE);

  Whole_Image = IMG_Load( fpath ); // This is a surface with alpha channel, since the picture is one of this type
  SDL_SetAlpha( Whole_Image , 0 , SDL_ALPHA_OPAQUE );

  tmp_surf = SDL_CreateRGBSurface( 0 , Block_Width, Block_Height, vid_bpp, 0, 0, 0, 0);
  SDL_SetColorKey( tmp_surf , 0 , 0 ); // this should clear any color key in the source surface

  for ( i=0 ; i < DROID_PHASES + DEAD_DROID_PHASES ; i++ )
    {
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
    }

  SDL_FreeSurface( Whole_Image );
  SDL_FreeSurface( tmp_surf );

}; // void Load_Influencer_Surfaces( void )

/* ----------------------------------------------------------------------
 * This function loads the all tux surfaces, that are needed to display 
 * the alternative tux character.
 * ---------------------------------------------------------------------- */
void 
HomemadeUpdateTuxWorkingCopy ( int PlayerNum )
{
  int i , j;
  static int Previous_weapon_item  [ MAX_PLAYERS_AT_MOST ] = { -2 , -2 , -2 , -2 , -2 } ;
  static int Previous_shield_item  [ MAX_PLAYERS_AT_MOST ] = { -2 , -2 , -2 , -2 , -2 } ;
  static int Previous_special_item [ MAX_PLAYERS_AT_MOST ] = { -2 , -2 , -2 , -2 , -2 } ;
  static int Previous_armour_item  [ MAX_PLAYERS_AT_MOST ] = { -2 , -2 , -2 , -2 , -2 } ; 
  SDL_Surface* tmp;
  float angle;

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
      SDL_FreeSurface ( TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] );
      TuxWorkingCopy [ PlayerNum ]  [ i ] [ 0 ] = SDL_DisplayFormatAlpha( TuxMotionArchetypes[7][i] );
    }
  
  //--------------------
  // Now we blit the armour item directly over the bare chest and feet, 
  // if some armour item is equipped of course.
  //
  if ( Me[0].armour_item.type != (-1) ) 
    {
      for ( i = 0 ; i < TUX_GOT_HIT_PHASES + TUX_SWING_PHASES + TUX_BREATHE_PHASES ; i ++ )
	{
	  tmp = CreateAlphaCombinedSurface ( TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] , TuxMotionArchetypes[8][i] );
	  SDL_FreeSurface ( TuxWorkingCopy [ PlayerNum ] [i] [ 0 ]);
	  TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] = tmp;
	}
    }

  //--------------------
  // Next we blit the weapon (and arms) of the tux
  //
  if ( ( Me[0].weapon_item.type == (-1) ) || ( Me[0].weapon_item.currently_held_in_hand ) )
    {
      for ( i = 0 ; i < TUX_GOT_HIT_PHASES + TUX_SWING_PHASES + TUX_BREATHE_PHASES ; i ++ )
	{
	  tmp = CreateAlphaCombinedSurface ( TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] , TuxMotionArchetypes[4][i] );
	  SDL_FreeSurface ( TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] );
	  TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] = tmp;
	}
    }
  else if ( Me[0].weapon_item.type == ITEM_STAFF )
    {
      for ( i = 0 ; i < TUX_GOT_HIT_PHASES + TUX_SWING_PHASES + TUX_BREATHE_PHASES ; i ++ )
	{
	  tmp = CreateAlphaCombinedSurface ( TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] , TuxMotionArchetypes[1][i] );
	  SDL_FreeSurface ( TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] );
	  TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] = tmp;
	}
    }
  else if ( ItemMap [ Me[0].weapon_item.type ].item_gun_angle_change == 0 )
    {
      for ( i = 0 ; i < TUX_GOT_HIT_PHASES + TUX_SWING_PHASES + TUX_BREATHE_PHASES ; i ++ )
	{
	  tmp = CreateAlphaCombinedSurface ( TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] , TuxMotionArchetypes[2][i] );
	  SDL_FreeSurface ( TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] );
	  TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] = tmp;
	}
    }
  else
    {
      for ( i = 0 ; i < TUX_GOT_HIT_PHASES + TUX_SWING_PHASES + TUX_BREATHE_PHASES ; i ++ )
	{
	  tmp = CreateAlphaCombinedSurface ( TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] , TuxMotionArchetypes[0][i] );
	  SDL_FreeSurface ( TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] );
	  TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] = tmp;
	}
    }

  //--------------------
  // Now we blit the shields OVER it.
  //
  if ( ( Me[0].shield_item.type != (-1) ) && ( ! Me[0].shield_item.currently_held_in_hand ) )
    {
      for ( i = 0 ; i < TUX_GOT_HIT_PHASES + TUX_SWING_PHASES + TUX_BREATHE_PHASES ; i ++ )
	{
	  tmp = CreateAlphaCombinedSurface ( TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] , TuxMotionArchetypes[3][i] );
	  SDL_FreeSurface ( TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] );
	  TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] = tmp;
	}
    }

  //--------------------
  // Now as the last part, we blit the head OVER the rest and than the hat OVER it all.
  //
  for ( i = 0 ; i < TUX_GOT_HIT_PHASES + TUX_SWING_PHASES + TUX_BREATHE_PHASES ; i ++ )
    {
      tmp = CreateAlphaCombinedSurface ( TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] , TuxMotionArchetypes[6][i] );
      SDL_FreeSurface ( TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] );
      TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] = tmp;
    }

  if ( ( ( Me[0].special_item.type == ( ITEM_IRON_HAT ) ) || ( Me[0].special_item.type == ( ITEM_SMALL_HELM ) ) ) && ( ! Me[0].special_item.currently_held_in_hand ) )
    {
      for ( i = 0 ; i < TUX_GOT_HIT_PHASES + TUX_SWING_PHASES + TUX_BREATHE_PHASES ; i ++ )
	{
	  tmp = CreateAlphaCombinedSurface ( TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] , TuxMotionArchetypes[9][i] );
	  SDL_FreeSurface ( TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] );
	  TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] = tmp;
	}
    }
  else if ( ( Me[0].special_item.type == ITEM_IRON_HELM ) && ( ! Me[0].special_item.currently_held_in_hand ) )
    {
      for ( i = 0 ; i < TUX_GOT_HIT_PHASES + TUX_SWING_PHASES + TUX_BREATHE_PHASES ; i ++ )
	{
	  tmp = CreateAlphaCombinedSurface ( TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] , TuxMotionArchetypes[10][i] );
	  SDL_FreeSurface ( TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] );
	  TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] = tmp;
	}
    }
  else if ( ( Me[0].special_item.type != (-1) ) && ( ! Me[0].special_item.currently_held_in_hand ) )
    {
      for ( i = 0 ; i < TUX_GOT_HIT_PHASES + TUX_SWING_PHASES + TUX_BREATHE_PHASES ; i ++ )
	{
	  tmp = CreateAlphaCombinedSurface ( TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] , TuxMotionArchetypes[5][i] );
	  SDL_FreeSurface ( TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] );
	  TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] = tmp;
	}
    }
  
  //--------------------
  // After the Tux has been assembled, we can do the rotation here as 
  // well.  Later this rotation step will no longer be nescessary, once
  // we use isometric viewpoint.  Then, we'll have much more assembling
  // work above instead...might be much more time-consuming...
  //
  for ( j = 1 ; j < MAX_TUX_DIRECTIONS ; j ++ )
    {
      for ( i = 0 ; i < TUX_GOT_HIT_PHASES + TUX_SWING_PHASES + TUX_BREATHE_PHASES ; i ++ )
	{
	  angle = ( j * 360.0 ) / MAX_TUX_DIRECTIONS;
	  if ( TuxWorkingCopy [ PlayerNum ] [ i ] [ j ] != NULL )
	    SDL_FreeSurface ( TuxWorkingCopy [ PlayerNum ] [ i ] [ j ] );
	  TuxWorkingCopy [ PlayerNum ] [ i ] [ j ] = 
	    rotozoomSurface( TuxWorkingCopy [ PlayerNum ] [ i ] [ 0 ] , angle , 1.0 , FALSE );
	}
    }

}; // void HomemadeUpdateTuxWorkingCopy ( void )

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
  int i , j , k;
  char *fpath;
  int PlayerNum;

#define TUX_WIDTH 130
#define TUX_HEIGHT 130


  fpath = find_file ( TUX_MOTIONS_FILE , GRAPHICS_DIR, TRUE);

  Whole_Image = IMG_Load( fpath ); // This is a surface with alpha channel, since the picture is one of this type
  SDL_SetAlpha( Whole_Image , 0 , SDL_ALPHA_OPAQUE ); // this should 
  SDL_SetColorKey( Whole_Image , 0 , 0 ); // this should clear any color key in the source surface

  tmp_surf = SDL_CreateRGBSurface( 0 , TUX_WIDTH , TUX_HEIGHT , vid_bpp , 0 , 0 , 0 , 0 );
  SDL_SetColorKey( tmp_surf , 0 , 0 ); // this should clear any color key in the source surface

  for ( j = 0 ; j < TUX_MODELS ; j ++ )
    {

      ShowStartupPercentage ( 30 + j * 4 ) ; 

      for ( i=0 ; i < TUX_GOT_HIT_PHASES + TUX_SWING_PHASES + TUX_BREATHE_PHASES ; i++ )
	{
	  TuxMotionArchetypes[j][i] = SDL_DisplayFormatAlpha( tmp_surf ); // now we have an alpha-surf of right size

	  SDL_SetColorKey ( TuxMotionArchetypes[j][i] , SDL_SRCCOLORKEY, 
			    SDL_MapRGB( TuxMotionArchetypes[j][i]->format, 255, 0, 255) ); 
	  // Now we can copy the image Information
	  Source.x=i*( TUX_WIDTH  + 2 );
	  Source.y=( 0 + j ) * ( TUX_HEIGHT + 2 ) ;
	  Source.w=TUX_WIDTH;
	  Source.h=TUX_HEIGHT;
	  Target.x=0;
	  Target.y=0;
	  Target.w=Block_Width;
	  Target.h=Block_Height;
	  SDL_BlitSurface ( Whole_Image , &Source , TuxMotionArchetypes[j][i] , &Target );
	  SDL_SetAlpha( TuxMotionArchetypes[j][i] , SDL_SRCALPHA , SDL_ALPHA_OPAQUE );

	  //--------------------
	  // And at this point, we also initialize the Tux working copys, so we
	  // won't run into trouble with uninitialized working copys later.
	  //
	  if ( j == 7 ) 
	    {
	      for ( PlayerNum = 0 ; PlayerNum < MAX_PLAYERS ; PlayerNum ++ )
		{
		  TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] = SDL_DisplayFormatAlpha( TuxMotionArchetypes[j][i] );
		}
	    }

	  for ( k = 1 ; k < MAX_TUX_DIRECTIONS ; k ++ )
	    {
	      for ( PlayerNum = 0 ; PlayerNum < MAX_PLAYERS ; PlayerNum ++ )
		{
		  TuxWorkingCopy [ PlayerNum ] [i] [ k ] = NULL ;
		}
	    }
	}
    }

  SDL_FreeSurface( Whole_Image );
  SDL_FreeSurface( tmp_surf );

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
	  fprintf( stderr, "\n\nfpath: '%s'\n" , fpath );
	  GiveStandardErrorMessage ( "Load_Big_Map_Insert_Surfaces(...)" , "\
Freedroid was unable to load a big graphics insert from the hard disk\n\
into memory.\n\
This error indicates some installation problem with freedroid.",
				     PLEASE_INFORM, IS_FATAL );
	}
      AllMapInserts [ i ] . insert_surface = SDL_DisplayFormat ( TempSurface ) ;
      SDL_FreeSurface ( TempSurface ) ;

      //--------------------
      // Now we check if the file has been loaded successfully, or otherwise
      // we print out an error message...
      //
      if ( ! AllMapInserts [ i ] . insert_surface )
	{
	  fprintf( stderr, "\n\nfpath: '%s'\n" , fpath );
	  GiveStandardErrorMessage ( "Load_Big_Map_Insert_Surfaces(...)" , "\
Freedroid was unable to load a big graphics insert from the hard disk\n\
into memory.\n\
This error indicates some installation problem with freedroid.",
				     PLEASE_INFORM, IS_FATAL );
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

  tmp_surf = SDL_CreateRGBSurface( 0 , INITIAL_DIGIT_LENGTH , INITIAL_DIGIT_HEIGHT, vid_bpp, 0, 0, 0, 0);
  SDL_SetColorKey( tmp_surf , 0 , 0 ); // this should clear any color key in the source surface

  for ( i=0 ; i < DIGITNUMBER ; i++ )
    {
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

  tmp_surf = SDL_CreateRGBSurface( 0 , INITIAL_DIGIT_LENGTH , INITIAL_DIGIT_HEIGHT, vid_bpp, 0, 0, 0, 0);
  SDL_SetColorKey( tmp_surf , 0 , 0 ); // this should clear any color key in the source surface
  for ( i=0 ; i < DIGITNUMBER ; i++ )
    {
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

  SDL_FreeSurface( Whole_Image );
  SDL_FreeSurface( tmp_surf );

}; // void Load_Digit_Surfaces( void )

/* ----------------------------------------------------------------------
 * In a more sophisticated approach to map block loading, not everything
 * will be loaded right at game startup but rather if and only if the
 * map block is required somewhere and no sooner than that.  (Except 
 * perhaps for level editor purposes maybe...)
 * So this function should do exactly, that:  Load the desired map block
 * from the disk and into memory for immediate usage, but only that.  No
 * separate copy for the level editor will be created here.  This has to
 * be done somewhere else at the appropriate time...
 * ---------------------------------------------------------------------- */
void
LoadOneMapTileIfNotYetLoaded( int BlockNr , int Color )
{
  SDL_Surface* Whole_Image;
  char *fpath;
  char ConstructedFileName[2000];
  char NumberBuffer[1000];

  //--------------------
  // At first we construct the file name of the single tile file we are about to load...
  //
  strcpy ( ConstructedFileName , "single_map_tiles/map_tile_" );
  strcat ( ConstructedFileName , "red_" );
  sprintf ( NumberBuffer , "%d" , BlockNr );
  strcat ( ConstructedFileName , NumberBuffer );
  strcat ( ConstructedFileName , ".png" );
  fpath = find_file ( ConstructedFileName , GRAPHICS_DIR , TRUE );

  //--------------------
  // Now we load the single tile image file and check for errors while loading...
  //
  Whole_Image = IMG_Load( fpath );

  //--------------------
  // Now we convert this to display format and set alpha and colorkey
  // properties right...
  //
  SDL_SetAlpha( Whole_Image , 0 , SDL_ALPHA_OPAQUE );
  MapBlockSurfacePointer [ Color ] [ BlockNr ] = SDL_DisplayFormat( Whole_Image );
  SDL_SetColorKey( MapBlockSurfacePointer [ Color ] [ BlockNr ] , 0 , 0 );
  SDL_SetAlpha( MapBlockSurfacePointer [ Color ] [ BlockNr ] , 0 , 0 );

  //--------------------
  // Now that this is all done, we can mark the map tile as loaded (later)
  // and free the small image we have loaded from the disk.
  //
  SDL_FreeSurface( Whole_Image );
  
}; // void LoadOneMapTileIfNotYetLoaded( int BlockNr , int Color )

/* ---------------------------------------------------------------------- 
 *
 *
 * ---------------------------------------------------------------------- */
void
LoadAllMapTilesThatAreNotYetLoaded( void )
{
  int i;
  int color;

  for ( color = 0 ; color < NUM_COLORS ; color ++ )
    {
      for ( i=0 ; i < NUM_MAP_BLOCKS ; i++ )
	{
	  LoadOneMapTileIfNotYetLoaded( i , color );
	}
    }

}; // void LoadAllMapTilesThatAreNotYetLoaded( void )

/* ----------------------------------------------------------------------
 * This function creates all the surfaces, that are nescessary to blit
 * some map tiles of any color.
 * ---------------------------------------------------------------------- */
void 
Load_MapBlock_Surfaces( void )
{

  Block_Width=INITIAL_BLOCK_WIDTH;
  Block_Height=INITIAL_BLOCK_HEIGHT;

  LoadAllMapTilesThatAreNotYetLoaded( );

  /*
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
  
  tmp_surf = SDL_CreateRGBSurface( 0 , Block_Width, Block_Height, vid_bpp, 0, 0, 0, 0);
  SDL_SetColorKey( tmp_surf , 0 , 0 ); // this should clear any color key in the source surface

  fpath = find_file ( fname, GRAPHICS_DIR, TRUE);
  for ( color = 0 ; color < NUM_COLORS ; color ++ )
    {
      Whole_Image = IMG_Load( fpath ); // This is a surface with alpha channel, since the picture is one of this type
      SDL_SetAlpha( Whole_Image , 0 , SDL_ALPHA_OPAQUE );
      
      for ( i=0 ; i < NUM_MAP_BLOCKS ; i++ )
	{
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
	}
      SDL_FreeSurface( Whole_Image );
    }

  SDL_FreeSurface( tmp_surf );
  */

}; // void Load_MapBlock_Surfaces( void )

#undef _blocks_c
