/* 
 *
 *   Copyright (c) 1994, 2002, 2003 Johannes Prix
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

void get_iso_image_from_file_and_path ( char* fpath , iso_image* our_iso_image );

char *PrefixToFilename[ ENEMY_ROTATION_MODELS_AVAILABLE ];
int ModelMultiplier[ ENEMY_ROTATION_MODELS_AVAILABLE ];

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
make_sure_zoomed_surface_is_there ( iso_image* our_iso_image )
{
  if ( our_iso_image -> zoomed_out_surface == NULL )
    {
      our_iso_image -> zoomed_out_surface = zoomSurface ( our_iso_image -> surface , ( 1.0 / FIXED_ZOOM_OUT_FACT ) ,
							  ( 1.0 / FIXED_ZOOM_OUT_FACT ) , FALSE );
    }
}; // void make_sure_zoomed_surface_is_there ( iso_image* our_iso_image )

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

  for ( j = 0 ; j < NUMBER_OF_ITEM_PICTURES ; j ++ )
    {
      Source.x = j * ( Block_Height + 2 );
      Source.y = i * ( Block_Width  + 2 );
      Source.w = (Block_Width/2) * ItemImageList [ j ] . inv_size . x ;
      Source.h = (Block_Height/2) * ItemImageList [ j ] . inv_size . y ;
      Target.x = 0 ;
      Target.y = 0 ;
      Target.w = Source . w ;
      Target.h = Source . h ;

      tmp_surf = SDL_CreateRGBSurface( 0 , Source . w , Source . h , vid_bpp , 0 , 0 , 0 , 0 );
      SDL_SetColorKey( tmp_surf , 0 , 0 ); // this should clear any color key in the source surface

      ItemImageList [ j ] . Surface = SDL_DisplayFormatAlpha ( tmp_surf ); // now we have an alpha-surf of right size
      SDL_SetColorKey( ItemImageList[ j ].Surface , 0 , 0 ); // this should clear any color key in the dest surface
      // Now we can copy the image Information
      SDL_BlitSurface ( Whole_Image , &Source , ItemImageList[ j ].Surface , &Target );
      SDL_SetAlpha( ItemImageList[ j ].Surface , SDL_SRCALPHA , SDL_ALPHA_OPAQUE );
      SDL_FreeSurface( tmp_surf );

      //--------------------
      // We must mark the in_game item surface as not yet loaded, so it
      // will be loaded later as soon as there is some demand...
      //
      ItemImageList [ j ] . ingame_iso_image . surface = NULL ;

    }

  SDL_FreeSurface( Whole_Image );

}; // void Load_Item_Surfaces( void )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
try_to_load_ingame_item_surface ( int item_type )
{
  char ConstructedFileName[5000];
  char* fpath;
  SDL_Surface *Whole_Image;

  //--------------------
  // First we handle a case, that shouldn't really be happening due to
  // calling function checking already.  But it can't hurt to always double-check
  //
  if ( ItemImageList [ ItemMap [ item_type ] . picture_number ] . ingame_iso_image . surface != NULL )
    {
      DebugPrintf ( 0 , "\ntry_to_load_ingame_item_surface (...): ERROR.  Surface appears to be loaded already..." );
      return;
    }

  //--------------------
  // Now we should try to load the real in-game item surface...
  // That will be added later...
  //
  //--------------------
  // At first we will try to find some item rotation models in the
  // new directory structure.
  //
  sprintf ( ConstructedFileName , "items/%s/ingame.png" , ItemMap[ item_type ] . item_rotation_series_prefix );
  fpath = find_file ( ConstructedFileName , GRAPHICS_DIR, FALSE );
  Whole_Image = IMG_Load( fpath ); // This is a surface with alpha channel, since the picture is one of this type
	  
  //--------------------
  // If that didn't work, then it's time to try out the 'classic' rotation models directory.
  // Maybe there's still some rotation image there.
  //
  if ( Whole_Image == NULL )
    {
      DebugPrintf ( 1 , "\nNo luck trying to load .png item ingame image..." );
      //--------------------
      // No ingame item surface found? -- give error message and then use
      // the inventory item_surface for the job.
      //
      fprintf ( stderr , "\nitem_type=%d." , item_type );
      GiveStandardErrorMessage ( "try_to_load_ingame_item_surface (...)" , "\
Unable to load an item ingame surface on demand.\n\
Since there seems to be no ingame item surface yet, the inventory\n\
item surface will be used as a substitute for now.",
				 NO_NEED_TO_INFORM, IS_WARNING_ONLY );
      ItemImageList [ ItemMap [ item_type ] . picture_number ] . ingame_iso_image . surface = ItemImageList [ ItemMap [ item_type ] . picture_number ] . Surface ;
      ItemImageList [ ItemMap [ item_type ] . picture_number ] . ingame_iso_image . offset_x = - ItemImageList [ ItemMap [ item_type ] . picture_number ] . Surface -> w / 2 ;
      ItemImageList [ ItemMap [ item_type ] . picture_number ] . ingame_iso_image . offset_y = - ItemImageList [ ItemMap [ item_type ] . picture_number ] . Surface -> h / 2 ;
    }
  else
    {
      //--------------------
      // So if an image of the required type can be found there, we 
      // can start to load it.  But for this we will use standard iso
      // object loading function, so that offset gets respected too...
      //
      /*
      SDL_SetAlpha( Whole_Image , 0 , SDL_ALPHA_OPAQUE );
      ItemImageList [ ItemMap [ item_type ] . picture_number ] . ingame_surface = 
	SDL_DisplayFormatAlpha( Whole_Image ); // now we have an alpha-surf of right size
      SDL_SetColorKey( ItemImageList [ ItemMap [ item_type ] . picture_number ] . ingame_surface , 0 , 0 ); // this should clear any color key in the dest surface
      */
      get_iso_image_from_file_and_path ( fpath , & ( ItemImageList [ ItemMap [ item_type ] . picture_number ] . ingame_iso_image ) );

      SDL_FreeSurface( Whole_Image );


    }


}; // void try_to_load_ingame_item_surface ( int item_number )

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
 *
 *
 * ---------------------------------------------------------------------- */
void
iso_load_bullet_surfaces ( void )
{
  int i , j , k ;
  char *fpath;
  char constructed_filename[ 5000 ];
  char* bullet_identifiers[] =
    {
      "pulse"        , // 0 "pluse" or "classic 001"
      "single"       , // 1 "single" or "classic 476"
      "military"     , // 2 "military" or "classic 821"
      "flash_dummy"  , // 3 dummy:  "classic flash", will be left out
      "exterminator" , // 4 "exterminator" , same as in classic
      "laser_rifle"  , // 5 "laser rifle" , same as in classic
      "half_pulse"   , // 6 "just one half of the two classic 001"
      "plasma_white" , // 7 small round white plasma ball
      "laser_sword"  , // 8
      "laser_axe"    , // 9
      "single"       , // 10 repetition of the single
      "half_green"   , // 11 that's the poison
      "half_blue"    , // 12 that's the cold
      "half_magenta" , // 13 that's the ??
      "half_white"   , // 14 that's the stone
      "ERROR:  UNHANDLED BULLET IMAGE TYPE" , // 15 error-code
      "ERROR:  UNHANDLED BULLET IMAGE TYPE" , // 16 error-code
      "ERROR:  UNHANDLED BULLET IMAGE TYPE" , // 17 error-code
      "ERROR:  UNHANDLED BULLET IMAGE TYPE" , // 18 error-code
      "ERROR:  UNHANDLED BULLET IMAGE TYPE" , // 19 error-code
    };

  DebugPrintf ( 0 , "Number_Of_Bullet_Types: %d." , Number_Of_Bullet_Types );

  for ( i=0 ; i < Number_Of_Bullet_Types ; i++ )
    {
      //--------------------
      // Flash is not something we would have to load.
      //
      if ( strlen ( bullet_identifiers [ i ] ) && !strcmp( bullet_identifiers [ i ] , "flash_dummy" ) ) 
	continue;
      if ( strlen ( bullet_identifiers [ i ] ) && !strcmp( bullet_identifiers [ i ] , "ERROR:  UNHANDLED BULLET IMAGE TYPE" ) ) 
	continue;

      for ( j=0 ; j < Bulletmap [ i ] . phases ; j++ )
	{
	  for ( k = 0 ; k < BULLET_DIRECTIONS ; k ++ )
	    {
	      //--------------------
	      // We construct the file name
	      //
	      sprintf ( constructed_filename , "bullets/iso_bullet_%s_%02d_%04d.png" , bullet_identifiers [ i ] , k , j + 1 );
	      fpath = find_file ( constructed_filename , GRAPHICS_DIR , FALSE );

	      get_iso_image_from_file_and_path ( fpath , & ( Bulletmap [ i ] . image [ k ] [ j ] ) ) ;

	    }

	  //--------------------
	  // Now we add proper offset in here, so that we can later conveniently use
	  // the standard iso_object blitting functions refering to map locations.
	  //
	  Bulletmap [ i ] . image [ 0 ] [ j ] . offset_x = - Bulletmap [ i ] . image [ 0 ] [ j ] . surface -> w / 2 ;
	  Bulletmap [ i ] . image [ 0 ] [ j ] . offset_y = - Bulletmap [ i ] . image [ 0 ] [ j ] . surface -> h / 2 ;
	}
    }

}; // void iso_load_bullet_surfaces ( void )

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
blit_iso_image_to_map_position ( iso_image our_iso_image , float pos_x , float pos_y )
{
  SDL_Rect target_rectangle;

  target_rectangle . x = 
    translate_map_point_to_screen_pixel ( pos_x , pos_y , TRUE ) + 
    our_iso_image . offset_x ;
  target_rectangle . y = 
    translate_map_point_to_screen_pixel ( pos_x , pos_y , FALSE ) +
    our_iso_image . offset_y ;

  SDL_BlitSurface( our_iso_image . surface , NULL , Screen, &target_rectangle );

}; // void blit_iso_image_to_map_position ( iso_image our_iso_image , float pos_x , float pos_y )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
blit_zoomed_iso_image_to_map_position ( iso_image* our_iso_image , float pos_x , float pos_y )
{
  SDL_Rect target_rectangle;

  target_rectangle . x = 
    translate_map_point_to_zoomed_screen_pixel ( pos_x , pos_y , TRUE ) + 
    our_iso_image -> offset_x / FIXED_ZOOM_OUT_FACT ;
  target_rectangle . y = 
    translate_map_point_to_zoomed_screen_pixel ( pos_x , pos_y , FALSE ) +
    our_iso_image -> offset_y / FIXED_ZOOM_OUT_FACT ;

  make_sure_zoomed_surface_is_there ( our_iso_image );
  SDL_BlitSurface( our_iso_image -> zoomed_out_surface , NULL , Screen, &target_rectangle );

}; // void blit_zoomed_iso_image_to_map_position ( iso_image our_iso_image , float pos_x , float pos_y )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
blit_iso_image_to_map_position_in_buffer ( SDL_Surface *current_buffer , 
					   iso_image our_iso_image , float pos_x , float pos_y )
{
  SDL_Rect target_rectangle;

  target_rectangle . x = 
    translate_map_point_to_screen_pixel ( pos_x , pos_y , TRUE ) + 
    our_iso_image . offset_x ;
  target_rectangle . y = 
    translate_map_point_to_screen_pixel ( pos_x , pos_y , FALSE ) +
    our_iso_image . offset_y ;

  SDL_BlitSurface( our_iso_image . surface , NULL , current_buffer, &target_rectangle );

}; // void blit_iso_image_to_map_position_in_buffer ( ... )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
int
iso_image_positioned_inside_copy_rectangle ( iso_image our_iso_image , float pos_x , float pos_y , 
					     float shift_pixels_x , float shift_pixels_y )
{
  SDL_Rect target_rectangle;

  target_rectangle . x = 
    translate_map_point_to_screen_pixel ( pos_x , pos_y , TRUE ) + 
    our_iso_image . offset_x ;
  target_rectangle . y = 
    translate_map_point_to_screen_pixel ( pos_x , pos_y , FALSE ) +
    our_iso_image . offset_y ;

  if ( ( target_rectangle . x > shift_pixels_x ) && ( target_rectangle . y > shift_pixels_y ) &&
       ( target_rectangle . x + our_iso_image . surface -> w < SCREEN_WIDTH - shift_pixels_x ) &&
       ( target_rectangle . y + our_iso_image . surface -> h < SCREEN_HEIGHT - shift_pixels_y ) )
    return ( TRUE );

  return ( FALSE );
};

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
get_offset_for_iso_image_from_file_and_path ( char* fpath , iso_image* our_iso_image )
{
  char offset_file_name[10000];
  FILE *OffsetFile; 
  char* offset_data;
  //--------------------
  // Now we try to load the associated offset file, that we'll be needing
  // in order to properly fine-position the image later when blitting is to
  // a map location.
  //
  strcpy ( offset_file_name , fpath );
  offset_file_name [ strlen ( offset_file_name ) - 4 ] = 0 ;
  strcat ( offset_file_name , ".offset" );

  //--------------------
  // Let's see if we can find an offset file...
  //
  if ( ( OffsetFile = fopen ( offset_file_name , "r") ) == NULL )
    {
      DebugPrintf ( 0 , "\nSeeking to gain offset from file names '%s'." , offset_file_name );
      GiveStandardErrorMessage ( "get_offset_for_iso_image_from_file_and_path(...)" , "\
Freedroid was unable to open a given offset file for an isometric image.\n\
Since the offset could not be obtained from the offset file, some default\n\
values will be used instead.  This can lead to minor positioning pertubations\n\
in graphics displayed, but FreedroidRPG will continue to work.",
				 NO_NEED_TO_INFORM, IS_WARNING_ONLY );
      our_iso_image -> offset_x = - INITIAL_BLOCK_WIDTH/2 ;
      our_iso_image -> offset_y = - INITIAL_BLOCK_HEIGHT/2 ;
      return ;
    }
  else
    {
      DebugPrintf ( 1 , "\nThe offset file seems to be there at least.....");

      if ( fclose ( OffsetFile ) == EOF)
	{
	  fprintf( stderr, "\n\noffset_file_name: '%s'\n" , offset_file_name );
	  GiveStandardErrorMessage ( "get_offset_for_iso_image_from_file_and_path(...)" , "\
Freedroid was unable to close an offset file.\nThis is a very strange occasion!",
				     PLEASE_INFORM, IS_FATAL );
	}
      else
	{
	  DebugPrintf( 1 , "\nThe offset file, that is there, was closed again successfully...");
	}

    }

  //--------------------
  // So at this point we can be certain, that the offset file is there.
  // That means, that we can now use the (otherwise terminating) read-and-malloc-...
  // functions.
  //
  offset_data = ReadAndMallocAndTerminateFile( offset_file_name , END_OF_OFFSET_FILE_STRING ) ;

  ReadValueFromString( offset_data ,  OFFSET_FILE_OFFSETX_STRING , "%d" , 
		       & ( our_iso_image -> offset_x ) , offset_data + 30000 );

  ReadValueFromString( offset_data ,  OFFSET_FILE_OFFSETY_STRING , "%d" , 
		       & ( our_iso_image -> offset_y ) , offset_data + 30000 );
  free ( offset_data );
  
}; // void get_offset_for_iso_image_from_file_and_path ( fpath , our_iso_image )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
get_iso_image_from_file_and_path ( char* fpath , iso_image* our_iso_image ) 
{
  SDL_Surface* Whole_Image;

  //--------------------
  // First we (try to) load the image given in the parameter
  // from hard disk into memory and convert it to the right
  // format for fast blitting later.
  //
  Whole_Image = IMG_Load( fpath ); // This is a surface with alpha channel, since the picture is one of this type
  if ( Whole_Image == NULL )
    {
      fprintf( stderr, "\n\nfpath: '%s'\n" , fpath );
      GiveStandardErrorMessage ( "get_iso_image_from_file_and_path (...)" , "\
Freedroid was unable to load a certain image file from hard disk into memory.\n\
This error indicates some installation problem with freedroid.",
				 PLEASE_INFORM, IS_FATAL );
    }
  SDL_SetAlpha( Whole_Image , 0 , SDL_ALPHA_OPAQUE );
  our_iso_image -> surface = SDL_DisplayFormatAlpha( Whole_Image ); // now we have an alpha-surf of right size
  our_iso_image -> zoomed_out_surface = NULL ;
  SDL_SetColorKey( our_iso_image -> surface , 0 , 0 ); // this should clear any color key in the dest surface
  //--------------------
  // Some test here...
  //
  // our_iso_image -> surface -> format -> Bmask = 0 ; 
  // our_iso_image -> surface -> format -> Rmask = 0 ; 

  SDL_FreeSurface( Whole_Image );

  //--------------------
  // Now that we have loaded the image, it's time to get the proper
  // offset information for it.
  //
  get_offset_for_iso_image_from_file_and_path ( fpath , our_iso_image );

}; // void get_iso_image_from_file_and_path ( char* fpath , iso_image* our_iso_image ) 

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void 
LoadAndPrepareEnemyRotationModelNr ( int ModelNr )
{
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

      sprintf ( ConstructedFileName , "droids/%s/ingame_%04d.png" , PrefixToFilename [ ModelNr ] ,
		( ModelMultiplier [ ModelNr ] * i ) + 1 );
      DebugPrintf ( 1 , "\nConstructedFileName = %s " , ConstructedFileName );
      fpath = find_file ( ConstructedFileName , GRAPHICS_DIR, FALSE );

      get_iso_image_from_file_and_path ( fpath , & ( enemy_iso_images [ ModelNr ] [ i ] ) ) ;
      
    }    

}; // void LoadAndPrepareEnemyRotationModelNr ( int j )

/* ----------------------------------------------------------------------
 * If needed, we will assemble differently colored versions of the enemy
 * rotation models...
 * ---------------------------------------------------------------------- */
void 
LoadAndPrepareGreenEnemyRotationModelNr ( int ModelNr )
{
  int i;
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
  // Now that we have our enemy surfaces ready, we can create some modified
  // copies of those surfaces but this a color filter applied to them...
  //
  for ( i=0 ; i < ROTATION_ANGLES_PER_ROTATION_MODEL ; i++ )
    {
      GreenEnemyRotationSurfacePointer [ ModelNr ] [ i ] = 
	CreateColorFilteredSurface ( enemy_iso_images [ ModelNr ] [ i ] . surface , FILTER_GREEN );
    }
}; // void LoadAndPrepareGreenEnemyRotationModelNr ( int ModelNr )
  
/* ----------------------------------------------------------------------
 * If needed, we will assemble differently colored versions of the enemy
 * rotation models...
 * ---------------------------------------------------------------------- */
void 
LoadAndPrepareBlueEnemyRotationModelNr ( int ModelNr )
{
  int i;
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
  // Now that we have our enemy surfaces ready, we can create some modified
  // copies of those surfaces but this a color filter applied to them...
  //
  for ( i=0 ; i < ROTATION_ANGLES_PER_ROTATION_MODEL ; i++ )
    {
      BlueEnemyRotationSurfacePointer [ ModelNr ] [ i ] = 
	CreateColorFilteredSurface ( enemy_iso_images [ ModelNr ] [ i ] . surface , FILTER_BLUE );
    }
}; // void LoadAndPrepareBlueEnemyRotationModelNr ( int ModelNr )
  
/* ----------------------------------------------------------------------
 * If needed, we will assemble differently colored versions of the enemy
 * rotation models...
 * ---------------------------------------------------------------------- */
void 
LoadAndPrepareRedEnemyRotationModelNr ( int ModelNr )
{
  int i;
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
  // Now that we have our enemy surfaces ready, we can create some modified
  // copies of those surfaces but this a color filter applied to them...
  //
  for ( i=0 ; i < ROTATION_ANGLES_PER_ROTATION_MODEL ; i++ )
    {
      RedEnemyRotationSurfacePointer [ ModelNr ] [ i ] = 
	CreateColorFilteredSurface ( enemy_iso_images [ ModelNr ] [ i ] . surface , FILTER_RED );
    }
}; // void LoadAndPrepareRedEnemyRotationModelNr ( int ModelNr )
  
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
	  enemy_iso_images [ j ] [ i ] . surface = NULL ;
	}
    }

  //--------------------
  // This needs to be initialized once, and this just seems a good place
  // to do this, so we can use the i++ syntax.
  //
  i=0;
  PrefixToFilename [ i ] = "001" ; // 0
  ModelMultiplier  [ i ] = 1 ; i++;
  PrefixToFilename [ i ] = "123" ; // 1
  ModelMultiplier  [ i ] = 1 ; i++;
  PrefixToFilename [ i ] = "139" ; // 2
  ModelMultiplier  [ i ] = 1 ;i++;
  PrefixToFilename [ i ] = "247" ; // 3 
  ModelMultiplier  [ i ] = 1 ;i++;
  PrefixToFilename [ i ] = "249" ; // 4
  ModelMultiplier  [ i ] = 1 ;i++;
  PrefixToFilename [ i ] = "296" ; // 5
  ModelMultiplier  [ i ] = 1 ;i++; 
  PrefixToFilename [ i ] = "302" ; // 6
  ModelMultiplier  [ i ] = 1 ;i++;
  PrefixToFilename [ i ] = "329" ; // 7
  ModelMultiplier  [ i ] = 1 ;i++;
  PrefixToFilename [ i ] = "420" ; // 8 
  ModelMultiplier  [ i ] = 1 ;i++;
  PrefixToFilename [ i ] = "476" ; // 9 
  ModelMultiplier  [ i ] = 1 ;i++;
  PrefixToFilename [ i ] = "493" ; // 10
  ModelMultiplier  [ i ] = 1 ;i++; 
  PrefixToFilename [ i ] = "516" ; // 11
  ModelMultiplier  [ i ] = 1 ;i++;
  PrefixToFilename [ i ] = "571" ; // 12
  ModelMultiplier  [ i ] = 1 ;i++;
  PrefixToFilename [ i ] = "598" ; // 13
  ModelMultiplier  [ i ] = 1 ;i++;
  PrefixToFilename [ i ] = "614" ; // 14
  ModelMultiplier  [ i ] = 1 ;i++;
  PrefixToFilename [ i ] = "615" ; // 15
  ModelMultiplier  [ i ] = 1 ;i++;
  PrefixToFilename [ i ] = "629" ; // 16
  ModelMultiplier  [ i ] = 1 ;i++;
  PrefixToFilename [ i ] = "711" ; // 17
  ModelMultiplier  [ i ] = 1 ;i++;
  PrefixToFilename [ i ] = "742" ; // 18
  ModelMultiplier  [ i ] = 1 ;i++;
  PrefixToFilename [ i ] = "751" ; // 19
  ModelMultiplier  [ i ] = 1 ;i++;
  PrefixToFilename [ i ] = "821" ; // 20
  ModelMultiplier  [ i ] = 1 ;i++;
  PrefixToFilename [ i ] = "834" ; // 21
  ModelMultiplier  [ i ] = 1 ;i++;
  PrefixToFilename [ i ] = "883" ; // 22
  ModelMultiplier  [ i ] = 1 ;i++;
  PrefixToFilename [ i ] = "999" ; // 23
  ModelMultiplier  [ i ] = 1 ;i++;
  PrefixToFilename [ i ] = "professor" ; // 24
  ModelMultiplier  [ i ] = 1 ;i++;
  PrefixToFilename [ i ] = "red_guard" ; // 25
  ModelMultiplier  [ i ] = 1 ;i++;
  PrefixToFilename [ i ] = "brown_worker" ; // 26
  ModelMultiplier  [ i ] = 1 ;i++;
  PrefixToFilename [ i ] = "blue_guard" ; // 27
  ModelMultiplier  [ i ] = 1 ;i++;
  PrefixToFilename [ i ] = "green_guard" ; // 28
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
 * 
 *
 * ---------------------------------------------------------------------- */
void
LoadOneTuxSurfaceIfNotYetLoaded ( int TuxModel , int TuxPhase )
{
  SDL_Surface* Whole_Image;
  char *fpath;
  char ConstructedFileName[2000];

  //--------------------
  // This function only has something to do at all, if the motion surface in question
  // hasn't been loaded already anyway.  So maybe we can just do nothing an return.
  // That's queried here.
  //
  if ( TuxMotionArchetypes [ TuxModel ] [ TuxPhase ] != NULL ) return;
    
  //--------------------
  // Now that we know, that we really have some work to do, we must
  // see to it, that not too much of a glitch is created from this...
  //
  Activate_Conservative_Frame_Computation ( ) ;

  //--------------------
  // Now we can proceed to really load the surfaces...
  // 
  sprintf ( ConstructedFileName , "tux_motion_parts/tux_motion_%02d_%02d.png" , TuxModel , TuxPhase );
  fpath = find_file ( ConstructedFileName , GRAPHICS_DIR, FALSE );
  Whole_Image = IMG_Load( fpath ); // This is a surface with alpha channel, since the picture is one of this type
  if ( Whole_Image == NULL )
    {
      fprintf( stderr, "\n\nfpath: '%s'\n" , fpath );
      GiveStandardErrorMessage ( "Load_Tux_Surfaces(...)" , "\
Freedroid was unable to load a certain Tux surface from the hard disk\n\
into memory.\n\
This error indicates some installation problem with freedroid.",
				 PLEASE_INFORM, IS_FATAL );
    }
  
  SDL_SetAlpha( Whole_Image , 0 , SDL_ALPHA_OPAQUE ); // this should 
  SDL_SetColorKey( Whole_Image , 0 , 0 ); // this should clear any color key in the source surface
  
  TuxMotionArchetypes [ TuxModel ] [ TuxPhase ] = SDL_DisplayFormatAlpha ( Whole_Image ); // now we have an alpha-surf of right size
  SDL_SetColorKey ( TuxMotionArchetypes [ TuxModel ] [ TuxPhase ] , SDL_SRCCOLORKEY, 
		    SDL_MapRGB ( TuxMotionArchetypes [ TuxModel ] [ TuxPhase ] -> format , 255 , 0 , 255 ) ); 
  SDL_SetAlpha( TuxMotionArchetypes [ TuxModel ] [ TuxPhase ] , SDL_SRCALPHA , SDL_ALPHA_OPAQUE );
  SDL_FreeSurface( Whole_Image );

}; // void LoadOneTuxSurfaceIfNotYetLoaded ( int TuxModel , int TuxPhase )

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

  return;

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
      LoadOneTuxSurfaceIfNotYetLoaded ( 7 , i ) ;
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
	  LoadOneTuxSurfaceIfNotYetLoaded ( 8 , i ) ;
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
	  LoadOneTuxSurfaceIfNotYetLoaded ( 4 , i ) ;
	  tmp = CreateAlphaCombinedSurface ( TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] , TuxMotionArchetypes[4][i] );
	  SDL_FreeSurface ( TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] );
	  TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] = tmp;
	}
    }
  else if ( Me[0].weapon_item.type == ITEM_STAFF )
    {
      for ( i = 0 ; i < TUX_GOT_HIT_PHASES + TUX_SWING_PHASES + TUX_BREATHE_PHASES ; i ++ )
	{
	  LoadOneTuxSurfaceIfNotYetLoaded ( 1 , i ) ;
	  tmp = CreateAlphaCombinedSurface ( TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] , TuxMotionArchetypes[1][i] );
	  SDL_FreeSurface ( TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] );
	  TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] = tmp;
	}
    }
  else if ( ( Me[0].weapon_item.type == ITEM_DAGGER ) ||
	    ( Me[0].weapon_item.type == ITEM_SCIMITAR ) ||
	    ( Me[0].weapon_item.type == ITEM_FALCHION ) )
    {
      for ( i = 0 ; i < TUX_GOT_HIT_PHASES + TUX_SWING_PHASES + TUX_BREATHE_PHASES ; i ++ )
	{
	  LoadOneTuxSurfaceIfNotYetLoaded ( 11 , i ) ;
	  tmp = CreateAlphaCombinedSurface ( TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] , TuxMotionArchetypes[11][i] );
	  SDL_FreeSurface ( TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] );
	  TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] = tmp;
	}
    }
  else if ( ( Me[0].weapon_item.type == ITEM_LONG_SWORD ) ||
	    ( Me[0].weapon_item.type == ITEM_CLAYMORE ) ||
	    ( Me[0].weapon_item.type == ITEM_SABER ) )
    {
      for ( i = 0 ; i < TUX_GOT_HIT_PHASES + TUX_SWING_PHASES + TUX_BREATHE_PHASES ; i ++ )
	{
	  LoadOneTuxSurfaceIfNotYetLoaded ( 12 , i ) ;
	  tmp = CreateAlphaCombinedSurface ( TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] , TuxMotionArchetypes[12][i] );
	  SDL_FreeSurface ( TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] );
	  TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] = tmp;
	}
    }
  else if ( ItemMap [ Me[0].weapon_item.type ].item_gun_angle_change == 0 )
    {
      for ( i = 0 ; i < TUX_GOT_HIT_PHASES + TUX_SWING_PHASES + TUX_BREATHE_PHASES ; i ++ )
	{
	  LoadOneTuxSurfaceIfNotYetLoaded ( 2 , i ) ;
	  tmp = CreateAlphaCombinedSurface ( TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] , TuxMotionArchetypes[2][i] );
	  SDL_FreeSurface ( TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] );
	  TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] = tmp;
	}
    }
  else
    {
      for ( i = 0 ; i < TUX_GOT_HIT_PHASES + TUX_SWING_PHASES + TUX_BREATHE_PHASES ; i ++ )
	{
	  LoadOneTuxSurfaceIfNotYetLoaded ( 0 , i ) ;
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
	  LoadOneTuxSurfaceIfNotYetLoaded ( 3 , i ) ;
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
      LoadOneTuxSurfaceIfNotYetLoaded ( 6 , i ) ;
      tmp = CreateAlphaCombinedSurface ( TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] , TuxMotionArchetypes[6][i] );
      SDL_FreeSurface ( TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] );
      TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] = tmp;
    }

  if ( ( ( Me[0].special_item.type == ( ITEM_IRON_HAT ) ) || ( Me[0].special_item.type == ( ITEM_SMALL_HELM ) ) ) && ( ! Me[0].special_item.currently_held_in_hand ) )
    {
      for ( i = 0 ; i < TUX_GOT_HIT_PHASES + TUX_SWING_PHASES + TUX_BREATHE_PHASES ; i ++ )
	{
	  LoadOneTuxSurfaceIfNotYetLoaded ( 9 , i ) ;
	  tmp = CreateAlphaCombinedSurface ( TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] , TuxMotionArchetypes[9][i] );
	  SDL_FreeSurface ( TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] );
	  TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] = tmp;
	}
    }
  else if ( ( Me[0].special_item.type == ITEM_IRON_HELM ) && ( ! Me[0].special_item.currently_held_in_hand ) )
    {
      for ( i = 0 ; i < TUX_GOT_HIT_PHASES + TUX_SWING_PHASES + TUX_BREATHE_PHASES ; i ++ )
	{
	  LoadOneTuxSurfaceIfNotYetLoaded ( 10 , i ) ;
	  tmp = CreateAlphaCombinedSurface ( TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] , TuxMotionArchetypes[10][i] );
	  SDL_FreeSurface ( TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] );
	  TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] = tmp;
	}
    }
  else if ( ( Me[0].special_item.type != (-1) ) && ( ! Me[0].special_item.currently_held_in_hand ) )
    {
      for ( i = 0 ; i < TUX_GOT_HIT_PHASES + TUX_SWING_PHASES + TUX_BREATHE_PHASES ; i ++ )
	{
	  LoadOneTuxSurfaceIfNotYetLoaded ( 5 , i ) ;
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
 * The Tux working copy will be freed and reallocated serveral times.  So
 * be must initialize it once when the program is started up.
 * ---------------------------------------------------------------------- */
void
InitTuxWorkingCopy( void )
{
  int PlayerNum;
  SDL_Surface* DummySurface;
  int k, i;

#define TUX_WIDTH 130
#define TUX_HEIGHT 130

  DummySurface = SDL_CreateRGBSurface ( 0 , TUX_WIDTH , TUX_HEIGHT , vid_bpp , 0 , 0 , 0 , 0 ) ;

  //--------------------
  // And at this point, we also initialize the Tux working copys, so we
  // won't run into trouble with uninitialized working copys later.
  //
  for ( i = 0 ; i < TUX_GOT_HIT_PHASES + TUX_SWING_PHASES + TUX_BREATHE_PHASES ; i++ ) 
    {
      for ( PlayerNum = 0 ; PlayerNum < MAX_PLAYERS ; PlayerNum ++ )
	{
	  TuxWorkingCopy [ PlayerNum ] [i] [ 0 ] = SDL_DisplayFormatAlpha( DummySurface );
	}
      for ( k = 1 ; k < MAX_TUX_DIRECTIONS ; k ++ )
	{
	  for ( PlayerNum = 0 ; PlayerNum < MAX_PLAYERS ; PlayerNum ++ )
	    {
	      TuxWorkingCopy [ PlayerNum ] [i] [ k ] = NULL ;
	    }
	}
    }

}; // void InitTuxWorkingCopy( void )

/* ----------------------------------------------------------------------
 * This function loads the all tux surfaces, that are needed to display 
 * the alternative tux character.
 * ---------------------------------------------------------------------- */
void 
Load_Tux_Surfaces( void )
{
  int i , j ;

  for ( j = 0 ; j < TUX_MODELS ; j ++ )
    {

      ShowStartupPercentage ( 30 + j * 4 ) ; 

      for ( i=0 ; i < TUX_GOT_HIT_PHASES + TUX_SWING_PHASES + TUX_BREATHE_PHASES ; i++ )
	{
	  
	  // LoadOneTuxSurfaceIfNotYetLoaded ( j , i );

	  TuxMotionArchetypes [ j ] [ i ] = NULL;

	}
    }

}; // void Load_Tux_Surfaces( void )

/* ---------------------------------------------------------------------- 
 * This function should initialize all obstacle types that are known in
 * FreedroidRPG, such as walls and doors and pillars and teleporters and
 * the like...
 *
 * for now it will not load 'offset' files, but rather just use hard-coded
 * info...
 *
 * ---------------------------------------------------------------------- */
void
load_all_obstacles ( void )
{
  int i;
  char *fpath;
  char ConstructedFileName[2000];
  char NumberBuffer[1000];

  for ( i = 0 ; i < NUMBER_OF_OBSTACLE_TYPES ; i ++ )
    {
      //--------------------
      // At first we construct the file name of the single tile file we are about to load...
      //
      strcpy ( ConstructedFileName , "iso_obstacle_" );
      sprintf ( NumberBuffer , "%04d" , i );
      strcat ( ConstructedFileName , NumberBuffer );
      strcat ( ConstructedFileName , ".png" );
      fpath = find_file ( ConstructedFileName , GRAPHICS_DIR , FALSE );
      
      get_iso_image_from_file_and_path ( fpath , & ( obstacle_map [ i ] . image ) ) ;

      //--------------------
      // In adition to the pure image information, we'll also need some
      // collision information for obstacles...
      //
      obstacle_map [ i ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
      obstacle_map [ i ] . block_area_parm_1 = 1.2 ;
      obstacle_map [ i ] . block_area_parm_2 = 0.25 ;
      obstacle_map [ i ] . is_smashable = FALSE ;
      obstacle_map [ i ] . drop_random_treasure = FALSE ;
    }

  obstacle_map [ ISO_V_WALL ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_V_WALL ] . block_area_parm_1 = 0.25 ;
  obstacle_map [ ISO_V_WALL ] . block_area_parm_2 = 1.2 ;

  obstacle_map [ ISO_V_DOOR_000_OPEN ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_V_DOOR_000_OPEN ] . block_area_parm_1 = 0.25 ;
  obstacle_map [ ISO_V_DOOR_000_OPEN ] . block_area_parm_2 = 1.2 ;
  obstacle_map [ ISO_V_DOOR_000_OPEN ] . is_smashable = FALSE ;
  obstacle_map [ ISO_V_DOOR_025_OPEN ] . is_smashable = FALSE ;
  obstacle_map [ ISO_V_DOOR_050_OPEN ] . is_smashable = FALSE ;
  obstacle_map [ ISO_V_DOOR_075_OPEN ] . is_smashable = FALSE ;
  obstacle_map [ ISO_V_DOOR_100_OPEN ] . is_smashable = FALSE ;
  obstacle_map [ ISO_V_DOOR_LOCKED ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_V_DOOR_LOCKED ] . block_area_parm_1 = 0.25 ;
  obstacle_map [ ISO_V_DOOR_LOCKED ] . block_area_parm_2 = 1.2 ;
  obstacle_map [ ISO_V_DOOR_LOCKED ] . is_smashable = FALSE ;

  obstacle_map [ ISO_H_DOOR_LOCKED ] . is_smashable = FALSE ;
  obstacle_map [ ISO_H_DOOR_000_OPEN ] . is_smashable = FALSE ;
  obstacle_map [ ISO_H_DOOR_025_OPEN ] . is_smashable = FALSE ;
  obstacle_map [ ISO_H_DOOR_050_OPEN ] . is_smashable = FALSE ;
  obstacle_map [ ISO_H_DOOR_075_OPEN ] . is_smashable = FALSE ;
  obstacle_map [ ISO_H_DOOR_100_OPEN ] . is_smashable = FALSE ;

  obstacle_map [ ISO_V_DOOR_100_OPEN ] . block_area_type = COLLISION_TYPE_NONE ;

  obstacle_map [ ISO_H_DOOR_100_OPEN ] . block_area_type = COLLISION_TYPE_NONE ;

  obstacle_map [ ISO_REFRESH_1 ] . block_area_type = COLLISION_TYPE_NONE ;
  obstacle_map [ ISO_REFRESH_1 ] . is_smashable = FALSE ;
  obstacle_map [ ISO_REFRESH_2 ] . block_area_type = COLLISION_TYPE_NONE ;
  obstacle_map [ ISO_REFRESH_2 ] . is_smashable = FALSE ;
  obstacle_map [ ISO_REFRESH_3 ] . block_area_type = COLLISION_TYPE_NONE ;
  obstacle_map [ ISO_REFRESH_3 ] . is_smashable = FALSE ;
  obstacle_map [ ISO_REFRESH_4 ] . block_area_type = COLLISION_TYPE_NONE ;
  obstacle_map [ ISO_REFRESH_4 ] . is_smashable = FALSE ;
  obstacle_map [ ISO_REFRESH_5 ] . block_area_type = COLLISION_TYPE_NONE ;
  obstacle_map [ ISO_REFRESH_5 ] . is_smashable = FALSE ;


  obstacle_map [ ISO_TELEPORTER_1 ] . block_area_type = COLLISION_TYPE_NONE ;
  obstacle_map [ ISO_TELEPORTER_1 ] . is_smashable = FALSE ;
  obstacle_map [ ISO_TELEPORTER_2 ] . block_area_type = COLLISION_TYPE_NONE ;
  obstacle_map [ ISO_TELEPORTER_2 ] . is_smashable = FALSE ;
  obstacle_map [ ISO_TELEPORTER_3 ] . block_area_type = COLLISION_TYPE_NONE ;
  obstacle_map [ ISO_TELEPORTER_3 ] . is_smashable = FALSE ;
  obstacle_map [ ISO_TELEPORTER_4 ] . block_area_type = COLLISION_TYPE_NONE ;
  obstacle_map [ ISO_TELEPORTER_4 ] . is_smashable = FALSE ;
  obstacle_map [ ISO_TELEPORTER_5 ] . block_area_type = COLLISION_TYPE_NONE ;
  obstacle_map [ ISO_TELEPORTER_5 ] . is_smashable = FALSE ;


  obstacle_map [ ISO_V_CHEST_OPEN ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_V_CHEST_OPEN ] . block_area_parm_1 = 0.25 ;
  obstacle_map [ ISO_V_CHEST_OPEN ] . block_area_parm_2 = 0.8 ;
  obstacle_map [ ISO_V_CHEST_CLOSED ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_V_CHEST_CLOSED ] . block_area_parm_1 = 0.25 ;
  obstacle_map [ ISO_V_CHEST_CLOSED ] . block_area_parm_2 = 0.8 ;

  obstacle_map [ ISO_H_CHEST_OPEN ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_H_CHEST_OPEN ] . block_area_parm_1 = 0.8 ;
  obstacle_map [ ISO_H_CHEST_OPEN ] . block_area_parm_2 = 0.25 ;
  obstacle_map [ ISO_H_CHEST_CLOSED ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_H_CHEST_CLOSED ] . block_area_parm_1 = 0.8 ;
  obstacle_map [ ISO_H_CHEST_CLOSED ] . block_area_parm_2 = 0.25 ;
  
  obstacle_map [ ISO_CRUDE_BLOCK ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_CRUDE_BLOCK ] . block_area_parm_1 = 0.8 ;
  obstacle_map [ ISO_CRUDE_BLOCK ] . block_area_parm_2 = 0.8 ;

  obstacle_map [ ISO_AUTOGUN_N ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_AUTOGUN_N ] . block_area_parm_1 = 0.9 ;
  obstacle_map [ ISO_AUTOGUN_N ] . block_area_parm_2 = 0.9 ;
  obstacle_map [ ISO_AUTOGUN_N ] . is_smashable = FALSE ;
  obstacle_map [ ISO_AUTOGUN_S ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_AUTOGUN_S ] . block_area_parm_1 = 0.9 ;
  obstacle_map [ ISO_AUTOGUN_S ] . block_area_parm_2 = 0.9 ;
  obstacle_map [ ISO_AUTOGUN_S ] . is_smashable = FALSE ;
  obstacle_map [ ISO_AUTOGUN_E ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_AUTOGUN_E ] . block_area_parm_1 = 0.9 ;
  obstacle_map [ ISO_AUTOGUN_E ] . block_area_parm_2 = 0.9 ;
  obstacle_map [ ISO_AUTOGUN_E ] . is_smashable = FALSE ;
  obstacle_map [ ISO_AUTOGUN_W ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_AUTOGUN_W ] . block_area_parm_1 = 0.9 ;
  obstacle_map [ ISO_AUTOGUN_W ] . block_area_parm_2 = 0.9 ;
  obstacle_map [ ISO_AUTOGUN_W ] . is_smashable = FALSE ;

  obstacle_map [ ISO_CAVE_CORNER_NE ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_CAVE_CORNER_NE ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_CAVE_CORNER_NE ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_CAVE_CORNER_NW ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_CAVE_CORNER_NW ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_CAVE_CORNER_NW ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_CAVE_CORNER_SE ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_CAVE_CORNER_SE ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_CAVE_CORNER_SE ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_CAVE_CORNER_SW ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_CAVE_CORNER_SW ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_CAVE_CORNER_SW ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_CAVE_WALL_H ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_CAVE_WALL_H ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_CAVE_WALL_H ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_CAVE_WALL_V ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_CAVE_WALL_V ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_CAVE_WALL_V ] . block_area_parm_2 = 1.0 ;

  obstacle_map [ ISO_COOKING_POT ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_COOKING_POT ] . block_area_parm_1 = 0.5 ;
  obstacle_map [ ISO_COOKING_POT ] . block_area_parm_2 = 0.5 ;
  obstacle_map [ ISO_COOKING_POT ] . is_smashable = TRUE ;

  obstacle_map [ ISO_CONSOLE_N ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_CONSOLE_N ] . block_area_parm_1 = 0.8 ;
  obstacle_map [ ISO_CONSOLE_N ] . block_area_parm_2 = 0.25 ;
  obstacle_map [ ISO_CONSOLE_N ] . is_smashable = TRUE ;

  obstacle_map [ ISO_CONSOLE_S ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_CONSOLE_S ] . block_area_parm_1 = 0.8 ;
  obstacle_map [ ISO_CONSOLE_S ] . block_area_parm_2 = 0.25 ;
  obstacle_map [ ISO_CONSOLE_S ] . is_smashable = TRUE ;

  obstacle_map [ ISO_CONSOLE_E ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_CONSOLE_E ] . block_area_parm_1 = 0.25 ;
  obstacle_map [ ISO_CONSOLE_E ] . block_area_parm_2 = 0.8 ;
  obstacle_map [ ISO_CONSOLE_E ] . is_smashable = TRUE ;

  obstacle_map [ ISO_CONSOLE_W ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_CONSOLE_W ] . block_area_parm_1 = 0.25 ;
  obstacle_map [ ISO_CONSOLE_W ] . block_area_parm_2 = 0.8 ;
  obstacle_map [ ISO_CONSOLE_W ] . is_smashable = TRUE ;

  obstacle_map [ ISO_BARREL_1 ] . drop_random_treasure = TRUE ;
  obstacle_map [ ISO_BARREL_1 ] . is_smashable = TRUE ;
  obstacle_map [ ISO_BARREL_2 ] . drop_random_treasure = TRUE ;
  obstacle_map [ ISO_BARREL_2 ] . is_smashable = TRUE ;
  obstacle_map [ ISO_BARREL_3 ] . drop_random_treasure = TRUE ;
  obstacle_map [ ISO_BARREL_3 ] . is_smashable = TRUE ;
  obstacle_map [ ISO_BARREL_4 ] . drop_random_treasure = TRUE ;
  obstacle_map [ ISO_BARREL_4 ] . is_smashable = TRUE ;

  obstacle_map [ ISO_V_WOOD_FENCE ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_V_WOOD_FENCE ] . block_area_parm_1 = 0.80 ;
  obstacle_map [ ISO_V_WOOD_FENCE ] . block_area_parm_2 = 2.0 ;
  obstacle_map [ ISO_V_WOOD_FENCE ] . is_smashable = FALSE ;
  obstacle_map [ ISO_V_DENSE_FENCE ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_V_DENSE_FENCE ] . block_area_parm_1 = 0.80 ;
  obstacle_map [ ISO_V_DENSE_FENCE ] . block_area_parm_2 = 2.0 ;
  obstacle_map [ ISO_V_DENSE_FENCE ] . is_smashable = FALSE ;
  obstacle_map [ ISO_V_MESH_FENCE ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_V_MESH_FENCE ] . block_area_parm_1 = 0.80 ;
  obstacle_map [ ISO_V_MESH_FENCE ] . block_area_parm_2 = 2.0 ;
  obstacle_map [ ISO_V_MESH_FENCE ] . is_smashable = FALSE ;
  obstacle_map [ ISO_V_WIRE_FENCE ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_V_WIRE_FENCE ] . block_area_parm_1 = 0.80 ;
  obstacle_map [ ISO_V_WIRE_FENCE ] . block_area_parm_2 = 2.0 ;
  obstacle_map [ ISO_V_WIRE_FENCE ] . is_smashable = FALSE ;
  obstacle_map [ ISO_H_WOOD_FENCE ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_H_WOOD_FENCE ] . block_area_parm_1 = 2.0 ;
  obstacle_map [ ISO_H_WOOD_FENCE ] . block_area_parm_2 = 0.80 ;
  obstacle_map [ ISO_H_WOOD_FENCE ] . is_smashable = FALSE ;
  obstacle_map [ ISO_H_DENSE_FENCE ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_H_DENSE_FENCE ] . block_area_parm_1 = 2.0 ;
  obstacle_map [ ISO_H_DENSE_FENCE ] . block_area_parm_2 = 0.80 ;
  obstacle_map [ ISO_H_DENSE_FENCE ] . is_smashable = FALSE ;
  obstacle_map [ ISO_H_MESH_FENCE ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_H_MESH_FENCE ] . block_area_parm_1 = 2.0 ;
  obstacle_map [ ISO_H_MESH_FENCE ] . block_area_parm_2 = 0.80 ;
  obstacle_map [ ISO_H_MESH_FENCE ] . is_smashable = FALSE ;
  obstacle_map [ ISO_H_WIRE_FENCE ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_H_WIRE_FENCE ] . block_area_parm_1 = 2.0 ;
  obstacle_map [ ISO_H_WIRE_FENCE ] . block_area_parm_2 = 0.80 ;
  obstacle_map [ ISO_H_WIRE_FENCE ] . is_smashable = FALSE ;

  obstacle_map [ ISO_N_TOILET_SMALL ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_N_TOILET_SMALL ] . block_area_parm_1 = 0.4 ;
  obstacle_map [ ISO_N_TOILET_SMALL ] . block_area_parm_2 = 0.4 ;
  obstacle_map [ ISO_N_TOILET_SMALL ] . is_smashable = TRUE ;
  obstacle_map [ ISO_E_TOILET_SMALL ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_E_TOILET_SMALL ] . block_area_parm_1 = 0.4 ;
  obstacle_map [ ISO_E_TOILET_SMALL ] . block_area_parm_2 = 0.4 ;
  obstacle_map [ ISO_E_TOILET_SMALL ] . is_smashable = TRUE ;
  obstacle_map [ ISO_S_TOILET_SMALL ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_S_TOILET_SMALL ] . block_area_parm_1 = 0.4 ;
  obstacle_map [ ISO_S_TOILET_SMALL ] . block_area_parm_2 = 0.4 ;
  obstacle_map [ ISO_S_TOILET_SMALL ] . is_smashable = TRUE ;
  obstacle_map [ ISO_W_TOILET_SMALL ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_W_TOILET_SMALL ] . block_area_parm_1 = 0.4 ;
  obstacle_map [ ISO_W_TOILET_SMALL ] . block_area_parm_2 = 0.4 ;
  obstacle_map [ ISO_W_TOILET_SMALL ] . is_smashable = TRUE ;
  obstacle_map [ ISO_N_TOILET_BIG ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_N_TOILET_BIG ] . block_area_parm_1 = 0.4 ;
  obstacle_map [ ISO_N_TOILET_BIG ] . block_area_parm_2 = 0.4 ;
  obstacle_map [ ISO_N_TOILET_BIG ] . is_smashable = TRUE ;
  obstacle_map [ ISO_E_TOILET_BIG ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_E_TOILET_BIG ] . block_area_parm_1 = 0.4 ;
  obstacle_map [ ISO_E_TOILET_BIG ] . block_area_parm_2 = 0.4 ;
  obstacle_map [ ISO_E_TOILET_BIG ] . is_smashable = TRUE ;
  obstacle_map [ ISO_S_TOILET_BIG ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_S_TOILET_BIG ] . block_area_parm_1 = 0.4 ;
  obstacle_map [ ISO_S_TOILET_BIG ] . block_area_parm_2 = 0.4 ;
  obstacle_map [ ISO_S_TOILET_BIG ] . is_smashable = TRUE ;
  obstacle_map [ ISO_W_TOILET_BIG ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_W_TOILET_BIG ] . block_area_parm_1 = 0.4 ;
  obstacle_map [ ISO_W_TOILET_BIG ] . block_area_parm_2 = 0.4 ;
  obstacle_map [ ISO_W_TOILET_BIG ] . is_smashable = TRUE ;

  obstacle_map [ ISO_N_CHAIR ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_N_CHAIR ] . block_area_parm_1 = 0.4 ;
  obstacle_map [ ISO_N_CHAIR ] . block_area_parm_2 = 0.4 ;
  obstacle_map [ ISO_N_CHAIR ] . is_smashable = TRUE ;
  obstacle_map [ ISO_E_CHAIR ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_E_CHAIR ] . block_area_parm_1 = 0.4 ;
  obstacle_map [ ISO_E_CHAIR ] . block_area_parm_2 = 0.4 ;
  obstacle_map [ ISO_E_CHAIR ] . is_smashable = TRUE ;
  obstacle_map [ ISO_S_CHAIR ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_S_CHAIR ] . block_area_parm_1 = 0.4 ;
  obstacle_map [ ISO_S_CHAIR ] . block_area_parm_2 = 0.4 ;
  obstacle_map [ ISO_S_CHAIR ] . is_smashable = TRUE ;
  obstacle_map [ ISO_W_CHAIR ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_W_CHAIR ] . block_area_parm_1 = 0.4 ;
  obstacle_map [ ISO_W_CHAIR ] . block_area_parm_2 = 0.4 ;
  obstacle_map [ ISO_W_CHAIR ] . is_smashable = TRUE ;
  obstacle_map [ ISO_N_DESK ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_N_DESK ] . block_area_parm_1 = 0.4 ;
  obstacle_map [ ISO_N_DESK ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_N_DESK ] . is_smashable = TRUE ;
  obstacle_map [ ISO_E_DESK ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_E_DESK ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_E_DESK ] . block_area_parm_2 = 0.4 ;
  obstacle_map [ ISO_E_DESK ] . is_smashable = TRUE ;
  obstacle_map [ ISO_S_DESK ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_S_DESK ] . block_area_parm_1 = 0.4 ;
  obstacle_map [ ISO_S_DESK ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_S_DESK ] . is_smashable = TRUE ;
  obstacle_map [ ISO_W_DESK ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_W_DESK ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_W_DESK ] . block_area_parm_2 = 0.4 ;
  obstacle_map [ ISO_W_DESK ] . is_smashable = TRUE ;
  obstacle_map [ ISO_N_SCHOOL_CHAIR ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_N_SCHOOL_CHAIR ] . block_area_parm_1 = 0.4 ;
  obstacle_map [ ISO_N_SCHOOL_CHAIR ] . block_area_parm_2 = 0.4 ;
  obstacle_map [ ISO_N_SCHOOL_CHAIR ] . is_smashable = TRUE ;
  obstacle_map [ ISO_E_SCHOOL_CHAIR ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_E_SCHOOL_CHAIR ] . block_area_parm_1 = 0.4 ;
  obstacle_map [ ISO_E_SCHOOL_CHAIR ] . block_area_parm_2 = 0.4 ;
  obstacle_map [ ISO_E_SCHOOL_CHAIR ] . is_smashable = TRUE ;
  obstacle_map [ ISO_S_SCHOOL_CHAIR ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_S_SCHOOL_CHAIR ] . block_area_parm_1 = 0.4 ;
  obstacle_map [ ISO_S_SCHOOL_CHAIR ] . block_area_parm_2 = 0.4 ;
  obstacle_map [ ISO_S_SCHOOL_CHAIR ] . is_smashable = TRUE ;
  obstacle_map [ ISO_W_SCHOOL_CHAIR ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_W_SCHOOL_CHAIR ] . block_area_parm_1 = 0.4 ;
  obstacle_map [ ISO_W_SCHOOL_CHAIR ] . block_area_parm_2 = 0.4 ;
  obstacle_map [ ISO_W_SCHOOL_CHAIR ] . is_smashable = TRUE ;

  obstacle_map [ ISO_N_BED ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_N_BED ] . block_area_parm_1 = 1.1 ;
  obstacle_map [ ISO_N_BED ] . block_area_parm_2 = 0.7 ;
  obstacle_map [ ISO_N_BED ] . is_smashable = TRUE ;
  obstacle_map [ ISO_E_BED ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_E_BED ] . block_area_parm_1 = 0.7 ;
  obstacle_map [ ISO_E_BED ] . block_area_parm_2 = 1.1 ;
  obstacle_map [ ISO_E_BED ] . is_smashable = TRUE ;
  obstacle_map [ ISO_S_BED ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_S_BED ] . block_area_parm_1 = 1.1 ;
  obstacle_map [ ISO_S_BED ] . block_area_parm_2 = 0.7 ;
  obstacle_map [ ISO_S_BED ] . is_smashable = TRUE ;
  obstacle_map [ ISO_W_BED ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_W_BED ] . block_area_parm_1 = 0.7 ;
  obstacle_map [ ISO_W_BED ] . block_area_parm_2 = 1.1 ;
  obstacle_map [ ISO_W_BED ] . is_smashable = TRUE ;
  obstacle_map [ ISO_N_EMPTY_BOOKSHELF ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_N_EMPTY_BOOKSHELF ] . block_area_parm_1 = 0.5 ;
  obstacle_map [ ISO_N_EMPTY_BOOKSHELF ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_N_EMPTY_BOOKSHELF ] . is_smashable = TRUE ;
  obstacle_map [ ISO_E_EMPTY_BOOKSHELF ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_E_EMPTY_BOOKSHELF ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_E_EMPTY_BOOKSHELF ] . block_area_parm_2 = 0.5 ;
  obstacle_map [ ISO_E_EMPTY_BOOKSHELF ] . is_smashable = TRUE ;
  obstacle_map [ ISO_S_EMPTY_BOOKSHELF ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_S_EMPTY_BOOKSHELF ] . block_area_parm_1 = 0.5 ;
  obstacle_map [ ISO_S_EMPTY_BOOKSHELF ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_S_EMPTY_BOOKSHELF ] . is_smashable = TRUE ;
  obstacle_map [ ISO_W_EMPTY_BOOKSHELF ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_W_EMPTY_BOOKSHELF ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_W_EMPTY_BOOKSHELF ] . block_area_parm_2 = 0.5 ;
  obstacle_map [ ISO_W_EMPTY_BOOKSHELF ] . is_smashable = TRUE ;
  obstacle_map [ ISO_N_FULL_BOOKSHELF ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_N_FULL_BOOKSHELF ] . block_area_parm_1 = 0.5 ;
  obstacle_map [ ISO_N_FULL_BOOKSHELF ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_N_FULL_BOOKSHELF ] . is_smashable = TRUE ;
  obstacle_map [ ISO_E_FULL_BOOKSHELF ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_E_FULL_BOOKSHELF ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_E_FULL_BOOKSHELF ] . block_area_parm_2 = 0.5 ;
  obstacle_map [ ISO_E_FULL_BOOKSHELF ] . is_smashable = TRUE ;
  obstacle_map [ ISO_S_FULL_BOOKSHELF ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_S_FULL_BOOKSHELF ] . block_area_parm_1 = 0.5 ;
  obstacle_map [ ISO_S_FULL_BOOKSHELF ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_S_FULL_BOOKSHELF ] . is_smashable = TRUE ;
  obstacle_map [ ISO_W_FULL_BOOKSHELF ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_W_FULL_BOOKSHELF ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_W_FULL_BOOKSHELF ] . block_area_parm_2 = 0.5 ;
  obstacle_map [ ISO_W_FULL_BOOKSHELF ] . is_smashable = TRUE ;
  obstacle_map [ ISO_N_FULL_PARK_BENCH ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_N_FULL_PARK_BENCH ] . block_area_parm_1 = 0.5 ;
  obstacle_map [ ISO_N_FULL_PARK_BENCH ] . block_area_parm_2 = 1.3 ;
  obstacle_map [ ISO_N_FULL_PARK_BENCH ] . is_smashable = TRUE ;
  obstacle_map [ ISO_E_FULL_PARK_BENCH ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_E_FULL_PARK_BENCH ] . block_area_parm_1 = 1.3 ;
  obstacle_map [ ISO_E_FULL_PARK_BENCH ] . block_area_parm_2 = 0.5 ;
  obstacle_map [ ISO_E_FULL_PARK_BENCH ] . is_smashable = TRUE ;
  obstacle_map [ ISO_S_FULL_PARK_BENCH ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_S_FULL_PARK_BENCH ] . block_area_parm_1 = 0.5 ;
  obstacle_map [ ISO_S_FULL_PARK_BENCH ] . block_area_parm_2 = 1.3 ;
  obstacle_map [ ISO_S_FULL_PARK_BENCH ] . is_smashable = TRUE ;
  obstacle_map [ ISO_W_FULL_PARK_BENCH ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_W_FULL_PARK_BENCH ] . block_area_parm_1 = 1.3 ;
  obstacle_map [ ISO_W_FULL_PARK_BENCH ] . block_area_parm_2 = 0.5 ;
  obstacle_map [ ISO_W_FULL_PARK_BENCH ] . is_smashable = TRUE ;

  obstacle_map [ ISO_H_BATHTUB ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_H_BATHTUB ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_H_BATHTUB ] . block_area_parm_2 = 0.5 ;
  obstacle_map [ ISO_H_BATHTUB ] . is_smashable = TRUE ;
  obstacle_map [ ISO_V_BATHTUB ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_V_BATHTUB ] . block_area_parm_1 = 0.5 ;
  obstacle_map [ ISO_V_BATHTUB ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_V_BATHTUB ] . is_smashable = TRUE ;
  obstacle_map [ ISO_H_WASHTUB ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_H_WASHTUB ] . block_area_parm_1 = 0.5 ;
  obstacle_map [ ISO_H_WASHTUB ] . block_area_parm_2 = 0.3 ;
  obstacle_map [ ISO_H_WASHTUB ] . is_smashable = TRUE ;
  obstacle_map [ ISO_V_WASHTUB ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_V_WASHTUB ] . block_area_parm_1 = 0.3 ;
  obstacle_map [ ISO_V_WASHTUB ] . block_area_parm_2 = 0.5 ;
  obstacle_map [ ISO_V_WASHTUB ] . is_smashable = TRUE ;
  obstacle_map [ ISO_V_CURTAIN ] . block_area_type = COLLISION_TYPE_NONE ;
  obstacle_map [ ISO_V_CURTAIN ] . block_area_parm_1 = -1 ;
  obstacle_map [ ISO_V_CURTAIN ] . block_area_parm_2 = -1 ;
  obstacle_map [ ISO_V_CURTAIN ] . is_smashable = FALSE ;
  obstacle_map [ ISO_H_CURTAIN ] . block_area_type = COLLISION_TYPE_NONE ;
  obstacle_map [ ISO_H_CURTAIN ] . block_area_parm_1 = -1 ;
  obstacle_map [ ISO_H_CURTAIN ] . block_area_parm_2 = -1 ;
  obstacle_map [ ISO_H_CURTAIN ] . is_smashable = FALSE ;
  obstacle_map [ ISO_N_SOFA ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_N_SOFA ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_N_SOFA ] . block_area_parm_2 = 0.5 ;
  obstacle_map [ ISO_N_SOFA ] . is_smashable = TRUE ;
  obstacle_map [ ISO_S_SOFA ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_S_SOFA ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_S_SOFA ] . block_area_parm_2 = 0.5 ;
  obstacle_map [ ISO_S_SOFA ] . is_smashable = TRUE ;
  obstacle_map [ ISO_E_SOFA ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_E_SOFA ] . block_area_parm_1 = 0.5 ;
  obstacle_map [ ISO_E_SOFA ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_E_SOFA ] . is_smashable = TRUE ;
  obstacle_map [ ISO_W_SOFA ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_W_SOFA ] . block_area_parm_1 = 0.5 ;
  obstacle_map [ ISO_W_SOFA ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_W_SOFA ] . is_smashable = TRUE ;

}; // void load_all_obstacles ( void )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
load_one_isometric_floor_tile ( int tile_type ) 
{
  char *fpath;
  char ConstructedFileName[2000];
  char NumberBuffer[1000];

  //--------------------
  // At first we construct the file name of the single tile file we are about to load...
  //
  strcpy ( ConstructedFileName , "iso_floor_" );
  sprintf ( NumberBuffer , "%04d" , tile_type );
  strcat ( ConstructedFileName , NumberBuffer );
  strcat ( ConstructedFileName , ".png" );
  fpath = find_file ( ConstructedFileName , GRAPHICS_DIR , FALSE );

  get_iso_image_from_file_and_path ( fpath , & ( floor_iso_images [ tile_type ] ) ) ;

}; // void load_one_isometric_floor_tile ( int tile_type ) 

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
load_all_isometric_floor_tiles ( void )
{
  int i;

  for ( i = 0 ; i < ALL_ISOMETRIC_FLOOR_TILES ; i ++ )
    {
      load_one_isometric_floor_tile ( i ) ;
    }
}; // void load_all_isometric_floor_tiles ( void )

#undef _blocks_c
