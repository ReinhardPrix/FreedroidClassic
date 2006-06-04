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
	our_iso_image -> zoomed_out_surface = zoomSurface ( our_iso_image -> surface , ( 1.0 / LEVEL_EDITOR_ZOOM_OUT_FACT ) ,
							    ( 1.0 / LEVEL_EDITOR_ZOOM_OUT_FACT ) , FALSE );
    }
}; // void make_sure_zoomed_surface_is_there ( iso_image* our_iso_image )

/* ----------------------------------------------------------------------
 * The automap (in OpenGL mode) uses some smaller version of the graphics
 * used to assemble the in-game obstacle images.  These smaller versions 
 * of the graphics have to be generated.  We do that on the fly at 
 * runtime once and for all obstacles at the first game startup.
 * ---------------------------------------------------------------------- */
void
make_sure_automap_surface_is_there ( obstacle_spec* our_obstacle_spec )
{
    our_obstacle_spec -> automap_version = 
	zoomSurface ( our_obstacle_spec -> image . surface , ( 1.0 * AUTOMAP_SANITY_FACTOR / AUTOMAP_ZOOM_OUT_FACT ) ,
		      ( 1.0 * AUTOMAP_SANITY_FACTOR / AUTOMAP_ZOOM_OUT_FACT ) , FALSE );
    if ( ! our_obstacle_spec -> automap_version )
    {
	GiveStandardErrorMessage ( __FUNCTION__  , "\
Creation of automap surface failed!  That is fatal!",
				   PLEASE_INFORM, IS_FATAL );
    }
}; // void make_sure_automap_surface_is_there ( iso_image* our_iso_image )

/* ----------------------------------------------------------------------
 * This function loads the Blast image and decodes it into the multiple
 * small Blast surfaces.
 * ---------------------------------------------------------------------- */
void 
Load_Blast_Surfaces( void )
{
  int j;
  char *fpath;
  char constructed_filename[5000];

  //--------------------
  // Now that we're here, we can as well load the blast surfaces, that we might be using
  // later...
  //
  for ( j = 0 ; j < PHASES_OF_EACH_BLAST ; j ++ )
    {
      sprintf ( constructed_filename , "blasts/iso_blast_bullet_%04d.png" , j + 1 );
      fpath = find_file ( constructed_filename , GRAPHICS_DIR , FALSE );
      get_iso_image_from_file_and_path ( fpath , & ( Blastmap [ 0 ] . image [ j ] ) , TRUE ) ;
    }

  for ( j = 0 ; j < PHASES_OF_EACH_BLAST ; j ++ )
    {
      sprintf ( constructed_filename , "blasts/iso_blast_droid_%04d.png" , j + 1 );
      fpath = find_file ( constructed_filename , GRAPHICS_DIR , FALSE );
      get_iso_image_from_file_and_path ( fpath , & ( Blastmap [ 1 ] . image [ j ] ) , TRUE ) ;
    }

}; // void Load_Blast_Surfaces( void )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
load_item_surfaces_for_item_type ( int item_type )
{
  SDL_Surface* original_img;
  SDL_Surface* tmp_surf2 = NULL;
  char *fpath;
  char our_filename [ 2000 ] ;
  
  //--------------------
  // First we load the inventory image.
  //
  sprintf ( our_filename , "items/%s" , ItemMap [ item_type ] . item_inv_file_name );

  fpath = find_file ( our_filename , GRAPHICS_DIR , FALSE );
  original_img = NULL ;
  original_img = IMG_Load( fpath ); 
  if ( original_img == NULL )
    {
      DebugPrintf ( -1000 , "\nitem_type=%d. fpath: %s." , item_type , fpath );
      GiveStandardErrorMessage ( __FUNCTION__  , "\
Inventory image specified above was not found!  That is fatal!",
				 PLEASE_INFORM, IS_FATAL );
    }

  if ( use_open_gl ) flip_image_horizontally ( original_img );
  
  //--------------------
  // Now we try to guess the inventory image tile sizes (in the 
  // inventory screen) from the pixel size of the inventory image
  // loaded.
  //
  // (AH)This approach is not perfect due to resolution considerations. Scaling may or may not be done.  We keep it by default, except
  // if the size is already set.

if( ! ItemMap [ item_type ] . inv_image . inv_size . x )
  {
  if ( original_img -> w % 32 )
    {
      DebugPrintf ( -1000 , "\nitem_type=%d. fpath: %s." , item_type , fpath );
      GiveStandardErrorMessage ( __FUNCTION__  , "\
Inventory image given doesn not have a width, that is a multiple of 32!\n\
But FreedroidRPG needs a width of this type, so it can associate the right\n\
number of inventory screen tiles with the item!  Fatal!",
				 PLEASE_INFORM, IS_FATAL );
    }
  else
    {
      ItemMap [ item_type ] . inv_image . inv_size . x = original_img -> w / 32 ;
    }
  }

if( ! ItemMap [ item_type ] . inv_image . inv_size . y )
  {
  if ( original_img -> h % 32 )
    {
      DebugPrintf ( -1000 , "\nitem_type=%d. fpath: %s." , item_type , fpath );
      GiveStandardErrorMessage ( __FUNCTION__  , "\
Inventory image given doesn not have a height, that is a multiple of 32!\n\
But FreedroidRPG needs a height of this type, so it can associate the right\n\
number of inventory screen tiles with the item!  Fatal!",
				 PLEASE_INFORM, IS_FATAL );
    }
  else
    {
      ItemMap [ item_type ] . inv_image . inv_size . y = original_img -> h / 32 ;
    }
  }

  // Does the image need scaling ? (currently only one items needs it, but as I'd like to raise the standard sizes from 32x32 to at least 64x64,
  // this code makes sense)
  int target_x = ItemMap [ item_type ] . inv_image . inv_size . x * 32; 
  int target_y = ItemMap [ item_type ] . inv_image . inv_size . y * 32;
  float factor_x = (float)target_x / (float)original_img -> w;
  float factor_y = (float)target_y / (float)original_img -> h;
  tmp_surf2 = zoomSurface ( original_img , factor_x , factor_y , FALSE );
  ItemMap [ item_type ] . inv_image . Surface = our_SDL_display_format_wrapperAlpha ( tmp_surf2 ); 
  SDL_FreeSurface ( tmp_surf2 );

  //--------------------
  // For the shop, we need versions of each image, where the image is scaled so
  // that it takes up a whole 64x64 shop display square.  So we prepare scaled
  // versions here and now...

  if(original_img -> w >= original_img -> h)
    {
    target_x = 64; 
    target_y = original_img -> h * 64.0 / (float)original_img -> w; //keep the scaling ratio !
    }
  if(original_img -> h > original_img -> w) 
    {
    target_y = 64;
    target_x = original_img -> w * 64.0 / (float)original_img -> h;
    }
  factor_x = ((float)GameConfig . screen_width / 640.0) * ((float)target_x / (float)original_img -> w);
  factor_y = ((float)GameConfig . screen_height / 480.0) * ((float)target_y / (float)original_img -> h);
  tmp_surf2 = zoomSurface ( original_img , factor_x , factor_y , FALSE );
  ItemMap [ item_type ] . inv_image . scaled_surface_for_shop = our_SDL_display_format_wrapperAlpha ( tmp_surf2 ) ;
  SDL_FreeSurface ( original_img );
  SDL_FreeSurface ( tmp_surf2 );

}; // void load_item_surfaces_for_item_type ( int item_type )

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
  if ( ( ItemMap [ item_type ] . inv_image . ingame_iso_image . surface != NULL ) || ( ItemMap [ item_type ] . inv_image . ingame_iso_image . texture_has_been_created ) )
    {
      GiveStandardErrorMessage ( __FUNCTION__  , "\
Surface has been loaded already!",
				 PLEASE_INFORM, IS_FATAL );
      // DebugPrintf ( 0 , "\ntry_to_load_ingame_item_surface (...): ERROR.  Surface appears to be loaded already..." );
      // return;
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
  Whole_Image = our_IMG_load_wrapper( fpath ); // This is a surface with alpha channel, since the picture is one of this type
	  
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
      DebugPrintf ( -1000 , "\nitem_type=%d. path tried=%s" , item_type , fpath );
      GiveStandardErrorMessage ( __FUNCTION__  , "\
Unable to load an item ingame surface on demand.\n\
Since there seems to be no ingame item surface yet, the inventory\n\
item surface will be used as a substitute for now.",
				 NO_NEED_TO_INFORM, IS_WARNING_ONLY );

      if ( use_open_gl )
	{
	  ItemMap [ item_type ] . inv_image . ingame_iso_image . surface  = 
	    SDL_DisplayFormatAlpha ( ItemMap [ item_type ] . inv_image . Surface ) ;
	}
      else
	{
	  ItemMap [ item_type ] . inv_image . ingame_iso_image . surface  = 
	    ItemMap [ item_type ] . inv_image . Surface ;
	}

      ItemMap [ item_type ] . inv_image . ingame_iso_image . offset_x = 
	- ItemMap [ item_type ] . inv_image . Surface -> w / 2 ;
      ItemMap [ item_type ] . inv_image . ingame_iso_image . offset_y = 
	- ItemMap [ item_type ] . inv_image . Surface -> h / 2 ;

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
	our_SDL_display_format_wrapperAlpha( Whole_Image ); // now we have an alpha-surf of right size
      SDL_SetColorKey( ItemImageList [ ItemMap [ item_type ] . picture_number ] . ingame_surface , 0 , 0 ); // this should clear any color key in the dest surface
      */
      get_iso_image_from_file_and_path ( fpath , & ( ItemMap [ item_type ] . inv_image . ingame_iso_image ) , TRUE );
      SDL_FreeSurface( Whole_Image );
    }


  //--------------------
  // Now that it has been made sure, that a dispensable image is
  // loaded for the ingame surface, we can destroy it and make a
  // textured quad from it...
  //
  if ( use_open_gl )
    {
      make_texture_out_of_surface ( & ( ItemMap [ item_type ] . inv_image . ingame_iso_image ) ) ;
    }

}; // void try_to_load_ingame_item_surface ( int item_number )

/* ----------------------------------------------------------------------
 * This function loads the items image and decodes it into the multiple
 * small item surfaces.
 * ---------------------------------------------------------------------- */
void 
Load_Mouse_Move_Cursor_Surfaces( void )
{
  int j;
  char *fpath;
  char our_filename[2000] = "";

  for ( j = 0 ; j < NUMBER_OF_MOUSE_CURSOR_PICTURES ; j++ )
    {
      sprintf ( our_filename , "mouse_move_cursor_%d.png" , j );
      fpath = find_file ( our_filename , GRAPHICS_DIR , FALSE );
      
      get_iso_image_from_file_and_path ( fpath , & ( MouseCursorImageList [ j ] ) , TRUE ) ;

      if ( use_open_gl )
	make_texture_out_of_surface ( & ( MouseCursorImageList [ j ] ) );
    }

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

  Whole_Image = our_IMG_load_wrapper( fpath ); // This is a surface with alpha channel, since the picture is one of this type
  SDL_SetAlpha( Whole_Image , 0 , SDL_ALPHA_OPAQUE );

  tmp_surf = SDL_CreateRGBSurface( 0 , SKILL_LEVEL_BUTTON_WIDTH , SKILL_LEVEL_BUTTON_HEIGHT , 
				   vid_bpp , 0 , 0 , 0 , 0 );
  SDL_SetColorKey( tmp_surf , 0 , 0 ); // this should clear any color key in the source surface

  for ( j = 0 ; j < NUMBER_OF_SKILL_PAGES ; j++ )
    {
      Source.x = j * ( SKILL_LEVEL_BUTTON_WIDTH );
      Source.y = i * ( SKILL_LEVEL_BUTTON_HEIGHT );
      Source.w = SKILL_LEVEL_BUTTON_WIDTH ;
      Source.h = SKILL_LEVEL_BUTTON_HEIGHT ;
      Target.x = 0;
      Target.y = 0;
      Target.w = Source.w;
      Target.h = Source.h;

      SpellLevelButtonImageList[ j ] . surface = our_SDL_display_format_wrapperAlpha( tmp_surf ); // now we have an alpha-surf of right size
      SDL_SetColorKey( SpellLevelButtonImageList[ j ] . surface , 0 , 0 ); // this should clear any color key in the dest surface
      // Now we can copy the image Information
      our_SDL_blit_surface_wrapper ( Whole_Image , &Source , SpellLevelButtonImageList[ j ] . surface , &Target );
      SDL_SetAlpha( SpellLevelButtonImageList[ j ] . surface , SDL_SRCALPHA , SDL_ALPHA_OPAQUE );
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

  DebugPrintf ( 1, "Number_Of_Bullet_Types: %d." , Number_Of_Bullet_Types );

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

	      get_iso_image_from_file_and_path ( fpath , & ( Bulletmap [ i ] . image [ k ] [ j ] ) , TRUE ) ;

	    }
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
  if ( SpellSkillMap [ SkillSpellNr ] . spell_skill_icon_surface . surface || SpellSkillMap [ SkillSpellNr ] . spell_skill_icon_surface . texture_has_been_created ) return;

  //--------------------
  // Now it's time to assemble the file name to get the image from
  //
  strcpy ( AssembledFileName , "skill_icons/" );
  strcat ( AssembledFileName , SpellSkillMap [ SkillSpellNr ] . spell_skill_icon_name );
  fpath = find_file ( AssembledFileName , GRAPHICS_DIR, FALSE );

  //--------------------
  // Now we can load and prepare the image and that's it
  //
  Whole_Image = our_IMG_load_wrapper( fpath ); // This is a surface with alpha channel, since the picture is one of this type
  if ( !Whole_Image )
    {
      fprintf ( stderr , "\nfpath=%s." , fpath );
      GiveStandardErrorMessage ( __FUNCTION__  , "\
Freedroid was unable to load a certain skill surface into memory.\n\
This error indicates some installation problem with freedroid.",
				 PLEASE_INFORM, IS_FATAL );
    }

  //Now scale the skill icon (ahuillet's attempt)
  SpellSkillMap [ SkillSpellNr ] . spell_skill_icon_surface . surface = our_SDL_display_format_wrapperAlpha ( Whole_Image );
  SDL_FreeSurface ( Whole_Image );

  SDL_SetColorKey( SpellSkillMap [ SkillSpellNr ] . spell_skill_icon_surface . surface , 0 , 0 ); 
  SDL_SetAlpha( SpellSkillMap [ SkillSpellNr ] . spell_skill_icon_surface . surface , SDL_SRCALPHA , SDL_ALPHA_OPAQUE );

  if ( use_open_gl )
    make_texture_out_of_surface ( & ( SpellSkillMap [ SkillSpellNr ] . spell_skill_icon_surface ) ) ;
  
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

  our_SDL_blit_surface_wrapper( our_iso_image . surface , NULL , Screen, &target_rectangle );

}; // void blit_iso_image_to_map_position ( iso_image our_iso_image , float pos_x , float pos_y )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
blit_outline_of_iso_image_to_map_position ( iso_image our_iso_image , float pos_x , float pos_y )
{
  SDL_Rect target_rectangle;
  Uint32 previous_test_color = 0 ;
  Uint32 new_test_color = 0 ;
  int x , y ;
  Uint32 light_frame_color = SDL_MapRGB( Screen->format, 255 , 255 , 0 );
  Uint32 color_key_value = SDL_MapRGB( our_iso_image . surface -> format, 255 , 0 , 255 );
  Uint8 previous_test_alpha = 0 ;
  Uint8 new_test_alpha = 0 ;

  DebugPrintf ( 1 , "\nblit_outline_of_iso_image_to_map_position: function invoked." );

  target_rectangle . x = 
    translate_map_point_to_screen_pixel ( pos_x , pos_y , TRUE ) + 
    our_iso_image . offset_x ;
  target_rectangle . y = 
    translate_map_point_to_screen_pixel ( pos_x , pos_y , FALSE ) +
    our_iso_image . offset_y ;

  if ( our_iso_image . surface -> flags & SDL_SRCCOLORKEY )
    {

  for ( x = 0 ; x < our_iso_image . surface -> w ; x ++ )
    {
      for ( y = 0 ; y < our_iso_image . surface -> h ; y ++ )
	{

	  new_test_color = FdGetPixel ( our_iso_image . surface , x , y ) ;
	  // DebugPrintf ( -5 , "\nAlpha received: %d." , (int) new_alpha_component );

	  if ( ( ( new_test_color == color_key_value ) && ( previous_test_color != color_key_value ) ) ||
	       ( ( new_test_color != color_key_value ) && ( previous_test_color == color_key_value ) ) )
	    {
	      if ( y != 0 )
		{
		  PutPixel ( Screen , target_rectangle . x + x , target_rectangle . y + y , light_frame_color );
		  PutPixel ( Screen , target_rectangle . x + x , target_rectangle . y + y - 1 , light_frame_color );
		  // DebugPrintf ( -5 , "\nPIXEL FILLED!" );
		}
	    }

	  if ( ( y == 0 ) && ( new_test_color != color_key_value ) )
	    {
	      PutPixel ( Screen , target_rectangle . x + x , target_rectangle . y + y - 1 , light_frame_color );
	      PutPixel ( Screen , target_rectangle . x + x , target_rectangle . y + y - 2 , light_frame_color );
	    }

	  if ( ( y == our_iso_image . surface -> h - 1 ) && ( new_test_color != color_key_value ) )
	    {
	      PutPixel ( Screen , target_rectangle . x + x , target_rectangle . y + y + 1 , light_frame_color );
	      PutPixel ( Screen , target_rectangle . x + x , target_rectangle . y + y + 2 , light_frame_color );
	    }

	  previous_test_color = new_test_color ;

	}
    }


  for ( y = 0 ; y < our_iso_image . surface -> h ; y ++ )
    {
      for ( x = 0 ; x < our_iso_image . surface -> w ; x ++ )
	{

	  new_test_color = FdGetPixel ( our_iso_image . surface , x , y ) ;
	  // DebugPrintf ( -5 , "\nAlpha received: %d." , (int) new_alpha_component );

	  if ( ( ( new_test_color == color_key_value ) && ( previous_test_color != color_key_value ) ) ||
	       ( ( new_test_color != color_key_value ) && ( previous_test_color == color_key_value ) ) )
	    {
	      if ( x != 0 )
		{
		  PutPixel ( Screen , target_rectangle . x + x     , target_rectangle . y + y , light_frame_color );
		  PutPixel ( Screen , target_rectangle . x + x - 1 , target_rectangle . y + y , light_frame_color );
		  // DebugPrintf ( -5 , "\nPIXEL FILLED!" );
		}
	    }
	  
	  if ( ( x == 0 ) && ( new_test_color != color_key_value ) )
	    {
	      PutPixel ( Screen , target_rectangle . x + x - 1 , target_rectangle . y + y , light_frame_color );
	      PutPixel ( Screen , target_rectangle . x + x - 2 , target_rectangle . y + y , light_frame_color );
	    }

	  if ( ( x == our_iso_image . surface -> w - 1 ) && ( new_test_color != color_key_value ) )
	    {
	      PutPixel ( Screen , target_rectangle . x + x + 1 , target_rectangle . y + y , light_frame_color );
	      PutPixel ( Screen , target_rectangle . x + x + 2 , target_rectangle . y + y , light_frame_color );
	    }

	  previous_test_color = new_test_color ;

	}
    }

    }
  else
    {

  for ( x = 0 ; x < our_iso_image . surface -> w ; x ++ )
    {
      for ( y = 0 ; y < our_iso_image . surface -> h ; y ++ )
	{

	  new_test_alpha = GetAlphaComponent ( our_iso_image . surface , x , y ) ;
	  // DebugPrintf ( -5 , "\nAlpha received: %d." , (int) new_alpha_component );

	  if ( ( ( new_test_alpha == SDL_ALPHA_TRANSPARENT ) && ( previous_test_alpha != SDL_ALPHA_TRANSPARENT ) ) ||
	       ( ( new_test_alpha != SDL_ALPHA_TRANSPARENT ) && ( previous_test_alpha == SDL_ALPHA_TRANSPARENT ) ) )
	    {
	      if ( y != 0 )
		{
		  PutPixel ( Screen , target_rectangle . x + x , target_rectangle . y + y , light_frame_color );
		  PutPixel ( Screen , target_rectangle . x + x , target_rectangle . y + y - 1 , light_frame_color );
		  // DebugPrintf ( -5 , "\nPIXEL FILLED!" );
		}
	    }

	  if ( ( y == 0 ) && ( new_test_alpha != SDL_ALPHA_TRANSPARENT ) )
	    {
	      PutPixel ( Screen , target_rectangle . x + x , target_rectangle . y + y - 1 , light_frame_color );
	      PutPixel ( Screen , target_rectangle . x + x , target_rectangle . y + y - 2 , light_frame_color );
	    }

	  if ( ( y == our_iso_image . surface -> h - 1 ) && ( new_test_alpha != SDL_ALPHA_TRANSPARENT ) )
	    {
	      PutPixel ( Screen , target_rectangle . x + x , target_rectangle . y + y + 1 , light_frame_color );
	      PutPixel ( Screen , target_rectangle . x + x , target_rectangle . y + y + 2 , light_frame_color );
	    }

	  previous_test_alpha = new_test_alpha ;

	}
    }


  for ( y = 0 ; y < our_iso_image . surface -> h ; y ++ )
    {
      for ( x = 0 ; x < our_iso_image . surface -> w ; x ++ )
	{

	  new_test_alpha = FdGetPixel ( our_iso_image . surface , x , y ) ;
	  // DebugPrintf ( -5 , "\nAlpha received: %d." , (int) new_alpha_component );

	  if ( ( ( new_test_alpha == SDL_ALPHA_TRANSPARENT ) && ( previous_test_alpha != SDL_ALPHA_TRANSPARENT ) ) ||
	       ( ( new_test_alpha != SDL_ALPHA_TRANSPARENT ) && ( previous_test_alpha == SDL_ALPHA_TRANSPARENT ) ) )
	    {
	      if ( x != 0 )
		{
		  PutPixel ( Screen , target_rectangle . x + x     , target_rectangle . y + y , light_frame_color );
		  PutPixel ( Screen , target_rectangle . x + x - 1 , target_rectangle . y + y , light_frame_color );
		  // DebugPrintf ( -5 , "\nPIXEL FILLED!" );
		}
	    }
	  
	  if ( ( x == 0 ) && ( new_test_alpha != SDL_ALPHA_TRANSPARENT ) )
	    {
	      PutPixel ( Screen , target_rectangle . x + x - 1 , target_rectangle . y + y , light_frame_color );
	      PutPixel ( Screen , target_rectangle . x + x - 2 , target_rectangle . y + y , light_frame_color );
	    }

	  if ( ( x == our_iso_image . surface -> w - 1 ) && ( new_test_alpha != SDL_ALPHA_TRANSPARENT ) )
	    {
	      PutPixel ( Screen , target_rectangle . x + x + 1 , target_rectangle . y + y , light_frame_color );
	      PutPixel ( Screen , target_rectangle . x + x + 2 , target_rectangle . y + y , light_frame_color );
	    }

	  previous_test_alpha = new_test_alpha ;

	}
    }


    }
}; // void blit_outline_of_iso_image_to_map_position ( iso_image our_iso_image , float pos_x , float pos_y )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
blit_iso_image_to_screen_position ( iso_image our_iso_image , float pos_x , float pos_y )
{
    SDL_Rect target_rectangle;
    
    // target_rectangle . x = pos_x + our_iso_image . offset_x ;
    // target_rectangle . y = pos_y + our_iso_image . offset_y ;
    target_rectangle . x = pos_x ;
    target_rectangle . y = pos_y ;
    
    our_SDL_blit_surface_wrapper( our_iso_image . surface , NULL , Screen, &target_rectangle );

}; // void blit_iso_image_to_screen_position ( iso_image our_iso_image , float pos_x , float pos_y )

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
	our_iso_image -> offset_x / LEVEL_EDITOR_ZOOM_OUT_FACT ;
    target_rectangle . y = 
	translate_map_point_to_zoomed_screen_pixel ( pos_x , pos_y , FALSE ) +
	our_iso_image -> offset_y / LEVEL_EDITOR_ZOOM_OUT_FACT ;
    
    if ( use_open_gl )
    {
	raise ( SIGSEGV );
	blit_zoomed_open_gl_texture_to_screen_position ( our_iso_image , target_rectangle . x , 
							 target_rectangle . y , TRUE , 0.25 ) ;
    }
    else
    {
	make_sure_zoomed_surface_is_there ( our_iso_image );
	our_SDL_blit_surface_wrapper( our_iso_image -> zoomed_out_surface , NULL , Screen, &target_rectangle );
    }

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

  our_SDL_blit_surface_wrapper( our_iso_image . surface , NULL , current_buffer, &target_rectangle );

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
       ( target_rectangle . x + our_iso_image . surface -> w < GameConfig . screen_width - shift_pixels_x ) &&
       ( target_rectangle . y + our_iso_image . surface -> h < GameConfig . screen_height - shift_pixels_y ) )
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
  if ( ( OffsetFile = fopen ( offset_file_name , "rb") ) == NULL )
    {
      fprintf (stderr, "\nObtaining offset failed with file name '%s'." , offset_file_name );
      GiveStandardErrorMessage ( __FUNCTION__  , "\
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
	  GiveStandardErrorMessage ( __FUNCTION__  , "\
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
		       & ( our_iso_image -> offset_x ) , offset_data + 1000 );

  ReadValueFromString( offset_data ,  OFFSET_FILE_OFFSETY_STRING , "%d" , 
		       & ( our_iso_image -> offset_y ) , offset_data + 1000 );
  free ( offset_data );

}; // void get_offset_for_iso_image_from_file_and_path ( fpath , our_iso_image )

/* ----------------------------------------------------------------------
 * The concept of an iso image involves an SDL_Surface or an OpenGL
 * texture and also suitable offset values, such that the image can be
 * correctly placed in an isometric image.
 *
 * This function is supposed to load the SDL_Surface (from which the 
 * OpenGL texture can be made later) AND also to load the corresponding
 * offset file for the image (or supply substitute values) such that the
 * offset values are suitably initialized.
 * ---------------------------------------------------------------------- */
void
get_iso_image_from_file_and_path ( char* fpath , iso_image* our_iso_image , int use_offset_file ) 
{
    SDL_Surface* Whole_Image;
  
    //--------------------
    // First we (try to) load the image given in the parameter
    // from hard disk into memory and convert it to the right
    // format for fast blitting later.
    //
    Whole_Image = our_IMG_load_wrapper( fpath ); // This is a surface with alpha channel, since the picture is one of this type
    if ( Whole_Image == NULL )
    {
	fprintf( stderr, "\n\nfpath: '%s'\n" , fpath );
	GiveStandardErrorMessage ( __FUNCTION__  ,  
				   va ( "Could not load image: File name: see next line\n%s \n" , fpath ) , PLEASE_INFORM, IS_FATAL );
    }

    //--------------------
    // Depending on whether this is supposed to work with faster but less
    // quality color key or slower but more quality alpha channel, we set
    // appropriate parameters in the SDL surfaces and also a reminder flag
    // in the iso_image structure.
    //
    our_iso_image -> force_color_key = FALSE ;
    
    SDL_SetAlpha( Whole_Image , 0 , SDL_ALPHA_OPAQUE );
    our_iso_image -> surface = our_SDL_display_format_wrapperAlpha( Whole_Image ); // now we have an alpha-surf of right size
    our_iso_image -> zoomed_out_surface = NULL ;
    our_iso_image -> texture_has_been_created = FALSE ;
    
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
    if ( use_offset_file ) 
	get_offset_for_iso_image_from_file_and_path ( fpath , our_iso_image );
    else
    {
	//--------------------
	// We _silently_ assume there is no offset file...
	//
	our_iso_image -> offset_x = - INITIAL_BLOCK_WIDTH/2 ;
	our_iso_image -> offset_y = - INITIAL_BLOCK_HEIGHT/2 ;
    }
 
    //--------------------
    // In the case of no open_gl (and therefore no conversion to a texture)
    // we make sure, that the open_gl optiomized methods will also find
    // suitable correspondents in the SDL-loaded images, like the original
    // image size and such...
    //
    our_iso_image -> original_image_width  = our_iso_image -> surface -> w ;
    our_iso_image -> original_image_height = our_iso_image -> surface -> h ;

}; // void get_iso_image_from_file_and_path ( char* fpath , iso_image* our_iso_image ) 

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
get_iso_image_with_colorkey_from_file_and_path ( char* fpath , iso_image* our_iso_image ) 
{
    SDL_Surface* Whole_Image;
    int x , y;
    Uint32 color_key_value;
    
    //--------------------
    // First we (try to) load the image given in the parameter
    // from hard disk into memory and convert it to the right
    // format for fast blitting later.
    //
    Whole_Image = our_IMG_load_wrapper( fpath ); // This is a surface with alpha channel, since the picture is one of this type
    if ( Whole_Image == NULL )
    {
	fprintf( stderr, "\n\nfpath: '%s'\n" , fpath );
	GiveStandardErrorMessage ( __FUNCTION__  , "\
Freedroid was unable to load a certain image file from hard disk into memory.\n\
This error indicates some installation problem with freedroid.",
				   PLEASE_INFORM, IS_FATAL );
    }
    
    //--------------------
    // Depending on whether this is supposed to work with faster but less
    // quality color key or slower but more quality alpha channel, we set
    // appropriate parameters in the SDL surfaces and also a reminder flag
    // in the iso_image structure.
    //
    our_iso_image -> force_color_key = TRUE ;
    
    SDL_SetAlpha( Whole_Image , 0 , SDL_ALPHA_OPAQUE );
    our_iso_image -> surface = our_SDL_display_format_wrapper( Whole_Image ); // now we have an alpha-surf of right size
    
    color_key_value = SDL_MapRGB( our_iso_image -> surface -> format, 255 , 0 , 255 );
    
    for ( x = 0 ; x < Whole_Image -> w ; x ++ )
    {
	for ( y = 0 ; y < Whole_Image -> h ; y ++ )
	{
	    //--------------------
	    // Any pixel that is halfway transparent will now be made 
	    // into the color key...
	    //
	    if ( GetAlphaComponent ( Whole_Image , x , y ) < 50 )
	    {
		PutPixel ( our_iso_image -> surface , x , y , color_key_value );
	    }
	}
    }
    
    our_iso_image -> zoomed_out_surface = NULL ;
    SDL_SetColorKey( our_iso_image -> surface , SDL_SRCCOLORKEY , color_key_value ); // this should clear any color key in the dest surface
    
    //--------------------
    // Some test here...
    //
    // our_iso_image -> surface -> format -> Bmask = 0 ; 
    // our_iso_image -> surface -> format -> Rmask = 0 ; 
    //
    SDL_FreeSurface( Whole_Image );
    
    //--------------------
    // Now that we have loaded the image, it's time to get the proper
    // offset information for it.
    //
    get_offset_for_iso_image_from_file_and_path ( fpath , our_iso_image );

    //--------------------
    // Now finally we need to set the 'original_image_width/height', because
    // this is the default value used with both, OpenGL and SDL.
    //
    our_iso_image -> original_image_width = our_iso_image -> surface -> w ;
    our_iso_image -> original_image_height = our_iso_image -> surface -> h ;

}; // void get_iso_image_from_file_and_path ( char* fpath , iso_image* our_iso_image ) 

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void 
LoadAndPrepareEnemyRotationModelNr ( int ModelNr )
{
    char ConstructedFileName[5000];
    int i, j;
    char *fpath;
    static int FirstCallEver = TRUE ;
    static int EnemyFullyPrepared [ ENEMY_ROTATION_MODELS_AVAILABLE ] ;
    int source_direction_code;
    
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
	GiveStandardErrorMessage ( __FUNCTION__  , "\
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
    // Maybe we got an image collection file for this enemy?  Then
    // of course we'll use it and not bother with anything else...
    //
    if ( Druidmap [ ModelNr ] . use_image_archive_file )
    {
	grab_enemy_images_from_archive ( ModelNr );
	return ;
    }
    
    //--------------------
    // Now that we have the classic ball-shaped design completely done,
    // we can start doing something new:  Let's try to use some pre-rotated
    // enemy surfaces for a change.  That might work out to be cool.
    //
    for ( i = 0 ; i < ROTATION_ANGLES_PER_ROTATION_MODEL ; i ++ )
    {
	//--------------------
	// If we don't have full animation cycles for this enemy yet, then of course we
	// just load the old image, using the old file name.
	//
	if ( ! Druidmap [ ModelNr ] . use_image_archive_file )
	{
	    sprintf ( ConstructedFileName , "droids/%s/ingame_%04d.png" , PrefixToFilename [ ModelNr ] ,
		      ( ModelMultiplier [ ModelNr ] * i ) + 1 );
	    DebugPrintf ( 1 , "\nConstructedFileName = %s " , ConstructedFileName );
	    fpath = find_file ( ConstructedFileName , GRAPHICS_DIR, FALSE );
	    get_iso_image_from_file_and_path ( fpath , & ( enemy_iso_images [ ModelNr ] [ i ] [ 0 ] ) , TRUE ) ;
	    
	    //--------------------
	    // Newly, we also make textures out of all enemy surfaces...
	    // This will prove to be very handy for purposes of color filtered
	    // output and such things...
	    //
	    make_texture_out_of_surface ( & ( enemy_iso_images [ ModelNr ] [ i ] [ 0 ] ) ) ;
	}
	//--------------------
	// But if we have an animation, maybe not complete animation but at least walkcycle 
	// and the like, then we can start loading these images.
	//
	else
	{
	    for ( j = 0 ; j < last_stand_animation_image [ ModelNr ] ; j ++ )
	    {
		//--------------------
		// Depending on which file numbering convention is being used
		// for this model as far as directioning is concerned, we set
		// the proper direction number for the files on disk.  302 uses
		// the classical Tux direction number codes while the other ones
		// use the old one-image-animation direction codes for enemies.
		//
		if ( ( ModelNr == 6 ) || ( ModelNr == 14 ) || ( ModelNr == 24 ) || ( ModelNr == 25 ) || ( ModelNr == 26 ) || ( ModelNr == 27 ) || ( ModelNr == 31 ) )
		    source_direction_code = ModelMultiplier [ ModelNr ] * i * 2 ;
		else
		    source_direction_code = ( ModelMultiplier [ ModelNr ] * i ) + 0 ;
		
		//--------------------
		// Now we can proceed to load the images for each animation phase
		//
		if ( j + 1 <= last_walk_animation_image [ ModelNr ] )
		{
		    sprintf ( ConstructedFileName , "droids/%s/walk_%02d_%04d.png" , 
			      PrefixToFilename [ ModelNr ] ,
			      source_direction_code , j + 1 );
		}
		else if ( j + 1 <= last_attack_animation_image [ ModelNr ] )
		{
		    //--------------------
		    // If there is no attack animation cycle yet, then we need to use the first
		    // walk animation image for the attack cycle and that's it.
		    // But if there is some animation done and available, then we can use these
		    // images of course.
		    //
		    if ( use_default_attack_image [ ModelNr ] )
		    {
			sprintf ( ConstructedFileName , "droids/%s/walk_%02d_0001.png" , 
				  PrefixToFilename [ ModelNr ] ,
				  source_direction_code );
		    }
		    else
		    {
			sprintf ( ConstructedFileName , "droids/%s/attack_%02d_%04d.png" , 
				  PrefixToFilename [ ModelNr ] ,
				  source_direction_code , j + 1 + 1 - first_attack_animation_image [ ModelNr ] );
		    }
		}
		else if ( j + 1 <= last_gethit_animation_image [ ModelNr ] )
		{
		    //--------------------
		    // If there is no gethit animation cycle yet, then we need to use the first
		    // walk animation image for the attack cycle and that's it.
		    // But if there is some animation done and available, then we can use these
		    // images of course.
		    //
		    if ( use_default_gethit_image [ ModelNr ] )
		    {
			sprintf ( ConstructedFileName , "droids/%s/walk_%02d_0001.png" , 
				  PrefixToFilename [ ModelNr ] ,
				  source_direction_code );
		    }
		    else
		    {
			sprintf ( ConstructedFileName , "droids/%s/gethit_%02d_%04d.png" , 
				  PrefixToFilename [ ModelNr ] ,
				  source_direction_code , j + 1 + 1 - first_gethit_animation_image [ ModelNr ] );
		    }
		}
		else if ( j + 1 <= last_death_animation_image [ ModelNr ] )
		{
		    //--------------------
		    // If there is no gethit animation cycle yet, then we need to use the first
		    // walk animation image for the attack cycle and that's it.
		    // But if there is some animation done and available, then we can use these
		    // images of course.
		    //
		    if ( use_default_death_image [ ModelNr ] )
		    {
			sprintf ( ConstructedFileName , "droids/default_dead_body_%02d_0001.png" , 
				  ModelMultiplier [ ModelNr ] * i * 2 );
		    }
		    else
		    {
			sprintf ( ConstructedFileName , "droids/%s/death_%02d_%04d.png" , 
				  PrefixToFilename [ ModelNr ] ,
				  source_direction_code , j + 1 + 1 - first_death_animation_image [ ModelNr ] );
			DebugPrintf ( 1 , "\nj=%d, i=%d, death_file_name=%s." , j , i , ConstructedFileName );
		    }
		}
		else if ( j + 1 <= last_stand_animation_image [ ModelNr ] )
		{
		    //--------------------
		    // If there is no stand animation cycle yet, then we need to use the first
		    // walk animation image for the stand cycle and that's it.
		    // But if there is some animation done and available, then we can use these
		    // images of course.
		    //
		    if ( use_default_stand_image [ ModelNr ] )
		    {
			sprintf ( ConstructedFileName , "droids/%s/walk_%02d_0001.png" , 
				  PrefixToFilename [ ModelNr ] ,
				  source_direction_code );
		    }
		    else
		    {
			sprintf ( ConstructedFileName , "droids/%s/stand_%02d_%04d.png" , 
				  PrefixToFilename [ ModelNr ] ,
				  source_direction_code , j + 1 + 1 - first_stand_animation_image [ ModelNr ] );
		    }
		}
		else
		{
		    fprintf ( stderr , "\n\nModelNr=%d.\nj=%d.\n" , ModelNr , j );
		    GiveStandardErrorMessage ( __FUNCTION__  , "\
Freedroid received a rotation model number that does not exist!",
					       PLEASE_INFORM, IS_FATAL );
		}
		
		
		DebugPrintf ( 1 , "\nConstructedFileName = %s " , ConstructedFileName );
		fpath = find_file ( ConstructedFileName , GRAPHICS_DIR, FALSE );
		get_iso_image_from_file_and_path ( fpath , & ( enemy_iso_images [ ModelNr ] [ i ] [ j ] ) , TRUE ) ;
		
		//--------------------
		// Newly, we also make textures out of all enemy surfaces...
		// This will prove to be very handy for purposes of color filtered
		// output and such things...
		//
		make_texture_out_of_surface ( & ( enemy_iso_images [ ModelNr ] [ i ] [ j ] ) ) ;
		
	    }
	}
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
      GiveStandardErrorMessage ( __FUNCTION__  , "\
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
  // But of course, this only needs to be done, if there is no OpenGL present
  // on this machine, cause OpenGL can do that color filtering on the fly
  // anyway, so no need to waste memory for this...
  //
  if ( ! use_open_gl )
    {
      for ( i=0 ; i < ROTATION_ANGLES_PER_ROTATION_MODEL ; i++ )
	{
	  GreenEnemyRotationSurfacePointer [ ModelNr ] [ i ] [ 0 ] . surface = 
	    CreateColorFilteredSurface ( enemy_iso_images [ ModelNr ] [ i ] [ 0 ] . surface , FILTER_GREEN );
	  GreenEnemyRotationSurfacePointer [ ModelNr ] [ i ] [ 0 ] . offset_x = 
	    enemy_iso_images [ ModelNr ] [ i ] [ 0 ] . offset_x ;
	  GreenEnemyRotationSurfacePointer [ ModelNr ] [ i ] [ 0 ] . offset_y = 
	    enemy_iso_images [ ModelNr ] [ i ] [ 0 ] . offset_y ;
	}
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
      GiveStandardErrorMessage ( __FUNCTION__  , "\
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
  for ( i = 0 ; i < ROTATION_ANGLES_PER_ROTATION_MODEL ; i ++ )
    {
      BlueEnemyRotationSurfacePointer [ ModelNr ] [ i ] [ 0 ] . surface = 
	CreateColorFilteredSurface ( enemy_iso_images [ ModelNr ] [ i ] [ 0 ] . surface , FILTER_BLUE );
      BlueEnemyRotationSurfacePointer [ ModelNr ] [ i ] [ 0 ] . offset_x = 
	enemy_iso_images [ ModelNr ] [ i ] [ 0 ] . offset_x ;
      BlueEnemyRotationSurfacePointer [ ModelNr ] [ i ] [ 0 ] . offset_y = 
	enemy_iso_images [ ModelNr ] [ i ] [ 0 ] . offset_y ;
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
      GiveStandardErrorMessage ( __FUNCTION__  , "\
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
      RedEnemyRotationSurfacePointer [ ModelNr ] [ i ] [ 0 ] . surface = 
	CreateColorFilteredSurface ( enemy_iso_images [ ModelNr ] [ i ] [ 0 ] . surface , FILTER_RED );
      RedEnemyRotationSurfacePointer [ ModelNr ] [ i ] [ 0 ] . offset_x = 
	enemy_iso_images [ ModelNr ] [ i ] [ 0 ] . offset_x ;
      RedEnemyRotationSurfacePointer [ ModelNr ] [ i ] [ 0 ] . offset_y = 
	enemy_iso_images [ ModelNr ] [ i ] [ 0 ] . offset_y ;
    }
}; // void LoadAndPrepareRedEnemyRotationModelNr ( int ModelNr )
  
/* ----------------------------------------------------------------------
 * This function creates all the surfaces, that are nescessary to blit the
 * 'head' and 'shoes' of an enemy.  The numbers are not dealt with here.
 * ---------------------------------------------------------------------- */
void 
Load_Enemy_Surfaces( void )
{
    int i;
    int j;
    
    //--------------------
    // We clean out the rotated enemy surface pointers, so that later we
    // can judge securely which of them have been initialized (non-Null)
    // and which of them have not.
    //
    for ( j = 0 ; j < ENEMY_ROTATION_MODELS_AVAILABLE ; j ++ )
    {
	for ( i=0 ; i < ROTATION_ANGLES_PER_ROTATION_MODEL ; i++ )
	{
	    enemy_iso_images [ j ] [ i ] [ 0 ] . surface = NULL ;
	}
    }

    //--------------------
    // When using the new tux image collection files, the animation cycle
    // lengthes for droids will be taken from the image collection file itself.
    // That is good, because it's so dynamic.  However, it also means, that
    // the real animation phase lengthes and that will in general not be known
    // until the graphics for that bot has been loaded.  But on the other hand
    // it might happen that some phase computation is done before the first
    // blit already.  In that case, uninitialized data structs might cause 
    // severe harm.  Therefore we initialize some sane default values, that should
    // protect against certain cases of wrong phase counts.
    //
    for ( i = 0 ; i < ENEMY_ROTATION_MODELS_AVAILABLE ; i ++ )
    {
	first_walk_animation_image [ i ] = 1 ;
	last_walk_animation_image [ i ] = 1 ;
	first_attack_animation_image [ i ] = 1 ;
	last_attack_animation_image [ i ] = 1 ;
	first_gethit_animation_image [ i ] = 1 ;
	last_gethit_animation_image [ i ] = 1 ;
	first_death_animation_image [ i ] = 1 ;
	last_death_animation_image [ i ] = 1 ;
	first_stand_animation_image [ i ] = 1 ;
	last_stand_animation_image [ i ] = 1 ;
	use_default_attack_image [ i ] = TRUE ;
	use_default_gethit_image [ i ] = TRUE ;
	use_default_death_image [ i ] = TRUE ;
	use_default_stand_image [ i ] = TRUE ;
    }

    //--------------------
    // This needs to be initialized once, and this just seems a good place
    // to do this, so we can use the i++ syntax.
    //
    i = 0 ;
    PrefixToFilename [ i ] = "001" ; // 0
    first_walk_animation_image [ i ] = 1 ;
    last_walk_animation_image [ i ] = 1 ;
    first_attack_animation_image [ i ] = 1 ;
    last_attack_animation_image [ i ] = 1 ;
    first_gethit_animation_image [ i ] = 1 ;
    last_gethit_animation_image [ i ] = 1 ;
    first_death_animation_image [ i ] = 1 ;
    last_death_animation_image [ i ] = 1 ;
    first_stand_animation_image [ i ] = 1 ;
    last_stand_animation_image [ i ] = 1 ;
    use_default_attack_image [ i ] = TRUE ;
    use_default_gethit_image [ i ] = TRUE ;
    use_default_death_image [ i ] = TRUE ;
    use_default_stand_image [ i ] = TRUE ;
    Druidmap [ i ] . suppress_bullet_generation_when_attacking = FALSE ;
    droid_walk_animation_speed_factor [ i ] = 5 ;
    droid_attack_animation_speed_factor [ i ] = 5 ;
    droid_gethit_animation_speed_factor [ i ] = 5 ;
    droid_death_animation_speed_factor [ i ] = 5 ;
    droid_stand_animation_speed_factor [ i ] = 5 ;
    ModelMultiplier  [ i ] = 1 ; i++;

    PrefixToFilename [ i ] = "123" ; // 1
    //--------------------
    // As the 123 now uses an image collection file, the information
    // about the first and last animation images will be taken from
    // the image collection archive anyway, so no need to hard-code
    // anything here and changes in the image collection file will
    // take effect immediately without code adaption...
    //
    // first_walk_animation_image [ i ] = 1 ;
    // last_walk_animation_image [ i ] = 5 ;
    // first_attack_animation_image [ i ] = 6 ;
    // last_attack_animation_image [ i ] = 10 ;
    // first_gethit_animation_image [ i ] = 11 ;
    // last_gethit_animation_image [ i ] = 15 ;
    // first_death_animation_image [ i ] = 16 ;
    // last_death_animation_image [ i ] = 20 ;
    // first_stand_animation_image [ i ] = 21 ;
    // last_stand_animation_image [ i ] = 21 ;
    // use_default_attack_image [ i ] = FALSE ;
    // use_default_gethit_image [ i ] = FALSE ;
    // use_default_death_image [ i ] = FALSE ;
    // use_default_stand_image [ i ] = TRUE ;
    Druidmap [ i ] . suppress_bullet_generation_when_attacking = TRUE ;
    Druidmap [ i ] . use_image_archive_file = TRUE ;
    droid_walk_animation_speed_factor [ i ] = 5 ;
    droid_attack_animation_speed_factor [ i ] = 8 ;
    droid_gethit_animation_speed_factor [ i ] = 8 ;
    droid_death_animation_speed_factor [ i ] = 8;
    droid_stand_animation_speed_factor [ i ] = 5 ;
    ModelMultiplier  [ i ] = 1 ; i++;

    PrefixToFilename [ i ] = "139" ; // 2
    //--------------------
    // As the 139 now uses an image collection file, the information
    // about the first and last animation images will be taken from
    // the image collection archive anyway, so no need to hard-code
    // anything here and changes in the image collection file will
    // take effect immediately without code adaption...
    //
    // first_walk_animation_image [ i ] = 1 ;
    // last_walk_animation_image [ i ] = 1 ;
    // first_attack_animation_image [ i ] = 1 ;
    // last_attack_animation_image [ i ] = 1 ;
    // first_gethit_animation_image [ i ] = 1 ;
    // last_gethit_animation_image [ i ] = 1 ;
    // first_death_animation_image [ i ] = 1 ;
    // last_death_animation_image [ i ] = 1 ;
    // first_stand_animation_image [ i ] = 1 ;
    // last_stand_animation_image [ i ] = 1 ;
    // use_default_attack_image [ i ] = TRUE ;
    // use_default_gethit_image [ i ] = TRUE ;
    // use_default_death_image [ i ] = TRUE ;
    // use_default_stand_image [ i ] = TRUE ;
    Druidmap [ i ] . suppress_bullet_generation_when_attacking = FALSE ;
    Druidmap [ i ] . use_image_archive_file = TRUE ;
    droid_walk_animation_speed_factor [ i ] = 5 ;
    droid_attack_animation_speed_factor [ i ] = 8 ;
    droid_gethit_animation_speed_factor [ i ] = 8 ;
    droid_death_animation_speed_factor [ i ] = 8 ;
    droid_stand_animation_speed_factor [ i ] = 5 ;
    ModelMultiplier  [ i ] = 1 ; i++;

    PrefixToFilename [ i ] = "247" ; // 3 
    //--------------------
    // As the 247 now uses an image collection file, the information
    // about the first and last animation images will be taken from
    // the image collection archive anyway, so no need to hard-code
    // anything here and changes in the image collection file will
    // take effect immediately without code adaption...
    //
    // first_walk_animation_image [ i ] = 1 ;
    // last_walk_animation_image [ i ] = 5 ;
    // first_attack_animation_image [ i ] = 6 ;
    // last_attack_animation_image [ i ] = 10 ;
    // first_gethit_animation_image [ i ] = 11 ;
    // last_gethit_animation_image [ i ] = 15 ;
    // first_death_animation_image [ i ] = 16 ;
    // last_death_animation_image [ i ] = 16 ;
    // first_stand_animation_image [ i ] = 17 ;
    // last_stand_animation_image [ i ] = 17 ;
    // use_default_attack_image [ i ] = FALSE ;
    // use_default_gethit_image [ i ] = FALSE ;
    // use_default_death_image [ i ] = TRUE ;
    // use_default_stand_image [ i ] = TRUE ;
    Druidmap [ i ] . suppress_bullet_generation_when_attacking = TRUE ;
    Druidmap [ i ] . use_image_archive_file = TRUE ;
    droid_walk_animation_speed_factor [ i ] = 5 ;
    droid_attack_animation_speed_factor [ i ] = 16 ;
    droid_gethit_animation_speed_factor [ i ] = 8 ;
    droid_death_animation_speed_factor [ i ] = 8 ;
    droid_stand_animation_speed_factor [ i ] = 5 ;
    ModelMultiplier  [ i ] = 1 ;i++;
    PrefixToFilename [ i ] = "249" ; // 4
    //--------------------
    // As the 249 now uses an image collection file, the information
    // about the first and last animation images will be taken from
    // the image collection archive anyway, so no need to hard-code
    // anything here and changes in the image collection file will
    // take effect immediately without code adaption...
    //
    // first_walk_animation_image [ i ] = 1 ;
    // last_walk_animation_image [ i ] = 5 ;
    // first_attack_animation_image [ i ] = 6 ;
    // last_attack_animation_image [ i ] = 6 ;
    // first_gethit_animation_image [ i ] = 7 ;
    // last_gethit_animation_image [ i ] = 7 ;
    // first_death_animation_image [ i ] = 8 ;
    // last_death_animation_image [ i ] = 8 ;
    // first_stand_animation_image [ i ] = 9 ;
    // last_stand_animation_image [ i ] = 9 ;
    // use_default_attack_image [ i ] = TRUE ;
    // use_default_gethit_image [ i ] = TRUE ;
    // use_default_death_image [ i ] = TRUE ;
    // use_default_stand_image [ i ] = TRUE ;
    Druidmap [ i ] . suppress_bullet_generation_when_attacking = FALSE ;
    Druidmap [ i ] . use_image_archive_file = TRUE ;
    droid_walk_animation_speed_factor [ i ] = 5 ;
    droid_attack_animation_speed_factor [ i ] = 8 ;
    droid_gethit_animation_speed_factor [ i ] = 8 ;
    droid_death_animation_speed_factor [ i ] = 8 ;
    droid_stand_animation_speed_factor [ i ] = 5 ;
    ModelMultiplier  [ i ] = 1 ;i++;

    PrefixToFilename [ i ] = "296" ; // 5
    first_walk_animation_image [ i ] = 1 ;
    last_walk_animation_image [ i ] = 5 ;
    first_attack_animation_image [ i ] = 6 ;
    last_attack_animation_image [ i ] = 6 ;
    first_gethit_animation_image [ i ] = 7 ;
    last_gethit_animation_image [ i ] = 7 ;
    first_death_animation_image [ i ] = 8 ;
    last_death_animation_image [ i ] = 8 ;
    first_stand_animation_image [ i ] = 9 ;
    last_stand_animation_image [ i ] = 9 ;
    use_default_attack_image [ i ] = TRUE ;
    use_default_gethit_image [ i ] = TRUE ;
    use_default_death_image [ i ] = TRUE ;
    use_default_stand_image [ i ] = TRUE ;
    Druidmap [ i ] . suppress_bullet_generation_when_attacking = FALSE ;
    droid_walk_animation_speed_factor [ i ] = 5 ;
    droid_attack_animation_speed_factor [ i ] = 5 ;
    droid_gethit_animation_speed_factor [ i ] = 5 ;
    droid_death_animation_speed_factor [ i ] = 5 ;
    droid_stand_animation_speed_factor [ i ] = 5 ;
    ModelMultiplier  [ i ] = 1 ;i++; 

    PrefixToFilename [ i ] = "302" ; // 6
    //--------------------
    // As the 302 now uses an image collection file, the information
    // about the first and last animation images will be taken from
    // the image collection archive anyway, so no need to hard-code
    // anything here and changes in the image collection file will
    // take effect immediately without code adaption...
    //
    // first_walk_animation_image[ i ] = 1 ;
    // last_walk_animation_image[ i ] = 1 ;
    // first_attack_animation_image[ i ] = 2 ;
    // last_attack_animation_image[ i ] = 16 ;
    // first_gethit_animation_image[ i ] = 17 ;
    // last_gethit_animation_image[ i ] = 24 ;
    // first_death_animation_image[ i ] = 25 ;
    // last_death_animation_image[ i ] = 31 ;
    // first_stand_animation_image [ i ] = 32 ;
    // last_stand_animation_image [ i ] = 32 ;
    // use_default_attack_image [ i ] = FALSE ;
    // use_default_gethit_image [ i ] = FALSE ;
    // use_default_death_image [ i ] = FALSE ;
    // use_default_stand_image [ i ] = TRUE ;
    Druidmap [ i ] . suppress_bullet_generation_when_attacking = TRUE ;
    Druidmap [ i ] . use_image_archive_file = TRUE ;
    droid_walk_animation_speed_factor [ i ] = 15 ;
    droid_attack_animation_speed_factor [ i ] = 15 ;
    droid_gethit_animation_speed_factor [ i ] = 15 ;
    droid_death_animation_speed_factor [ i ] = 15 ;
    droid_stand_animation_speed_factor [ i ] = 15 ;
    ModelMultiplier  [ i ] = 1 ;i++;

    PrefixToFilename [ i ] = "329" ; // 7
    //--------------------
    // As the 329 now uses an image collection file, the information
    // about the first and last animation images will be taken from
    // the image collection archive anyway, so no need to hard-code
    // anything here and changes in the image collection file will
    // take effect immediately without code adaption...
    //
    // first_walk_animation_image [ i ] = 1 ;
    // last_walk_animation_image [ i ] = 1 ;
    // first_attack_animation_image [ i ] = 1 ;
    // last_attack_animation_image [ i ] = 1 ;
    // first_gethit_animation_image [ i ] = 1 ;
    // last_gethit_animation_image [ i ] = 1 ;
    // first_death_animation_image [ i ] = 1 ;
    // last_death_animation_image [ i ] = 1 ;
    // first_stand_animation_image [ i ] = 1 ;
    // last_stand_animation_image [ i ] = 1 ;
    // use_default_attack_image [ i ] = TRUE ;
    // use_default_gethit_image [ i ] = TRUE ;
    // use_default_death_image [ i ] = TRUE ;
    // use_default_stand_image [ i ] = TRUE ;
    Druidmap [ i ] . suppress_bullet_generation_when_attacking = TRUE ;
    Druidmap [ i ] . use_image_archive_file = TRUE ;
    droid_walk_animation_speed_factor [ i ] = 5 ;
    droid_attack_animation_speed_factor [ i ] = 8 ;
    droid_gethit_animation_speed_factor [ i ] = 8 ;
    droid_death_animation_speed_factor [ i ] = 8 ;
    droid_stand_animation_speed_factor [ i ] = 5 ;
    ModelMultiplier  [ i ] = 1 ;i++;

    PrefixToFilename [ i ] = "420" ; // 8 
    //--------------------
    // As the 420 now uses an image collection file, the information
    // about the first and last animation images will be taken from
    // the image collection archive anyway, so no need to hard-code
    // anything here and changes in the image collection file will
    // take effect immediately without code adaption...
    //
    // first_walk_animation_image [ i ] = 1 ;
    // last_walk_animation_image [ i ] = 1 ;
    // first_attack_animation_image [ i ] = 1 ;
    // last_attack_animation_image [ i ] = 1 ;
    // first_gethit_animation_image [ i ] = 1 ;
    // last_gethit_animation_image [ i ] = 1 ;
    // first_death_animation_image [ i ] = 1 ;
    // last_death_animation_image [ i ] = 1 ;
    // first_stand_animation_image [ i ] = 1 ;
    // last_stand_animation_image [ i ] = 1 ;
    // use_default_attack_image [ i ] = TRUE ;
    // use_default_gethit_image [ i ] = TRUE ;
    // use_default_death_image [ i ] = TRUE ;
    // use_default_stand_image [ i ] = TRUE ;
    Druidmap [ i ] . suppress_bullet_generation_when_attacking = TRUE ;
    Druidmap [ i ] . use_image_archive_file = TRUE ;
    droid_walk_animation_speed_factor [ i ] = 5 ;
    droid_attack_animation_speed_factor [ i ] = 5 ;
    droid_gethit_animation_speed_factor [ i ] = 5 ;
    droid_death_animation_speed_factor [ i ] = 5 ;
    droid_stand_animation_speed_factor [ i ] = 5 ;
    ModelMultiplier  [ i ] = 1 ;i++;
    PrefixToFilename [ i ] = "476" ; // 9 
    first_walk_animation_image [ i ] = 1 ;
    last_walk_animation_image [ i ] = 1 ;
    first_attack_animation_image [ i ] = 1 ;
    last_attack_animation_image [ i ] = 1 ;
    first_gethit_animation_image [ i ] = 1 ;
    last_gethit_animation_image [ i ] = 1 ;
    first_death_animation_image [ i ] = 1 ;
    last_death_animation_image [ i ] = 1 ;
    first_stand_animation_image [ i ] = 1 ;
    last_stand_animation_image [ i ] = 1 ;
    use_default_attack_image [ i ] = TRUE ;
    use_default_gethit_image [ i ] = TRUE ;
    use_default_death_image [ i ] = TRUE ;
    use_default_stand_image [ i ] = TRUE ;
    Druidmap [ i ] . suppress_bullet_generation_when_attacking = FALSE ;
    droid_walk_animation_speed_factor [ i ] = 5 ;
    droid_attack_animation_speed_factor [ i ] = 5 ;
    droid_gethit_animation_speed_factor [ i ] = 5 ;
    droid_death_animation_speed_factor [ i ] = 5 ;
    droid_stand_animation_speed_factor [ i ] = 5 ;
    ModelMultiplier  [ i ] = 1 ;i++;
    PrefixToFilename [ i ] = "493" ; // 10
    first_walk_animation_image [ i ] = 1 ;
    last_walk_animation_image [ i ] = 1 ;
    first_attack_animation_image [ i ] = 1 ;
    last_attack_animation_image [ i ] = 1 ;
    first_gethit_animation_image [ i ] = 1 ;
    last_gethit_animation_image [ i ] = 1 ;
    first_death_animation_image [ i ] = 1 ;
    last_death_animation_image [ i ] = 1 ;
    first_stand_animation_image [ i ] = 1 ;
    last_stand_animation_image [ i ] = 1 ;
    use_default_attack_image [ i ] = TRUE ;
    use_default_gethit_image [ i ] = TRUE ;
    use_default_death_image [ i ] = TRUE ;
    use_default_stand_image [ i ] = TRUE ;
    Druidmap [ i ] . suppress_bullet_generation_when_attacking = FALSE ;
    droid_walk_animation_speed_factor [ i ] = 5 ;
    droid_attack_animation_speed_factor [ i ] = 5 ;
    droid_gethit_animation_speed_factor [ i ] = 5 ;
    droid_death_animation_speed_factor [ i ] = 5 ;
    droid_stand_animation_speed_factor [ i ] = 5 ;
    ModelMultiplier  [ i ] = 1 ;i++; 
    PrefixToFilename [ i ] = "516" ; // 11
    first_walk_animation_image [ i ] = 1 ;
    last_walk_animation_image [ i ] = 1 ;
    first_attack_animation_image [ i ] = 1 ;
    last_attack_animation_image [ i ] = 1 ;
    first_gethit_animation_image [ i ] = 1 ;
    last_gethit_animation_image [ i ] = 1 ;
    first_death_animation_image [ i ] = 1 ;
    last_death_animation_image [ i ] = 1 ;
    first_stand_animation_image [ i ] = 1 ;
    last_stand_animation_image [ i ] = 1 ;
    use_default_attack_image [ i ] = TRUE ;
    use_default_gethit_image [ i ] = TRUE ;
    use_default_death_image [ i ] = TRUE ;
    use_default_stand_image [ i ] = TRUE ;
    Druidmap [ i ] . suppress_bullet_generation_when_attacking = FALSE ;
    Druidmap [ i ] . use_image_archive_file = TRUE ;
    droid_walk_animation_speed_factor [ i ] = 5 ;
    droid_attack_animation_speed_factor [ i ] = 5 ;
    droid_gethit_animation_speed_factor [ i ] = 5 ;
    droid_death_animation_speed_factor [ i ] = 5 ;
    droid_stand_animation_speed_factor [ i ] = 5 ;
    ModelMultiplier  [ i ] = 1 ;i++;
    PrefixToFilename [ i ] = "571" ; // 12
    //first_walk_animation_image [ i ] = 1 ;
    //last_walk_animation_image [ i ] = 1 ;
    //first_attack_animation_image [ i ] = 1 ;
    //last_attack_animation_image [ i ] = 1 ;
    //first_gethit_animation_image [ i ] = 1 ;
    //last_gethit_animation_image [ i ] = 1 ;
    //first_death_animation_image [ i ] = 1 ;
    //last_death_animation_image [ i ] = 1 ;
    //first_stand_animation_image [ i ] = 1 ;
    //last_stand_animation_image [ i ] = 1 ;
    //use_default_attack_image [ i ] = TRUE ;
    //use_default_gethit_image [ i ] = TRUE ;
    //use_default_death_image [ i ] = TRUE ;
    //use_default_stand_image [ i ] = TRUE ;
    Druidmap [ i ] . suppress_bullet_generation_when_attacking = TRUE ;
    Druidmap [ i ] . use_image_archive_file = TRUE ;
    droid_walk_animation_speed_factor [ i ] = 5 ;
    droid_attack_animation_speed_factor [ i ] = 5 ;
    droid_gethit_animation_speed_factor [ i ] = 5 ;
    droid_death_animation_speed_factor [ i ] = 5 ;
    droid_stand_animation_speed_factor [ i ] = 5 ;
    ModelMultiplier  [ i ] = 1 ;i++;

    PrefixToFilename [ i ] = "598" ; // 13
    //--------------------
    // As the 598 now uses an image collection file, the information
    // about the first and last animation images will be taken from
    // the image collection archive anyway, so no need to hard-code
    // anything here and changes in the image collection file will
    // take effect immediately without code adaption...
    //
    // first_walk_animation_image [ i ] = 1 ;
    // last_walk_animation_image [ i ] = 1 ;
    // first_attack_animation_image [ i ] = 1 ;
    // last_attack_animation_image [ i ] = 1 ;
    // first_gethit_animation_image [ i ] = 1 ;
    // last_gethit_animation_image [ i ] = 1 ;
    // first_death_animation_image [ i ] = 1 ;
    // last_death_animation_image [ i ] = 1 ;
    // first_stand_animation_image [ i ] = 1 ;
    // last_stand_animation_image [ i ] = 1 ;
    // use_default_attack_image [ i ] = TRUE ;
    // use_default_gethit_image [ i ] = TRUE ;
    // use_default_death_image [ i ] = TRUE ;
    // use_default_stand_image [ i ] = TRUE ;
    Druidmap [ i ] . suppress_bullet_generation_when_attacking = TRUE ;
    Druidmap [ i ] . use_image_archive_file = TRUE ;
    droid_walk_animation_speed_factor [ i ] = 5 ;
    droid_attack_animation_speed_factor [ i ] = 8 ;
    droid_gethit_animation_speed_factor [ i ] = 8 ;
    droid_death_animation_speed_factor [ i ] = 8 ;
    droid_stand_animation_speed_factor [ i ] = 5 ;
    ModelMultiplier  [ i ] = 1 ;i++;
    
    PrefixToFilename [ i ] = "614" ; // 14
    //--------------------
    // As the 614 now uses an image collection file, the information
    // about the first and last animation images will be taken from
    // the image collection archive anyway, so no need to hard-code
    // anything here and changes in the image collection file will
    // take effect immediately without code adaption...
    //
    // first_walk_animation_image [ i ] = 1 ;
    // last_walk_animation_image [ i ] = 5 ;
    // first_attack_animation_image [ i ] = 6 ;
    // last_attack_animation_image [ i ] = 6 ;
    // first_gethit_animation_image [ i ] = 7 ;
    // last_gethit_animation_image [ i ] = 7 ;
    // first_death_animation_image [ i ] = 8 ;
    // last_death_animation_image [ i ] = 8 ;
    // first_stand_animation_image [ i ] = 9 ;
    // last_stand_animation_image [ i ] = 18 ;
    // use_default_attack_image [ i ] = TRUE ;
    // use_default_gethit_image [ i ] = TRUE ;
    // use_default_death_image [ i ] = TRUE ;
    // use_default_stand_image [ i ] = FALSE ;
    Druidmap [ i ] . suppress_bullet_generation_when_attacking = FALSE ;
    Druidmap [ i ] . use_image_archive_file = TRUE ;
    droid_walk_animation_speed_factor [ i ] = 18 ;
    droid_attack_animation_speed_factor [ i ] = 5 ;
    droid_gethit_animation_speed_factor [ i ] = 5 ;
    droid_death_animation_speed_factor [ i ] = 5 ;
    droid_stand_animation_speed_factor [ i ] = 3 ;
    ModelMultiplier  [ i ] = 1 ;i++;
    
    PrefixToFilename [ i ] = "615" ; // 15
    first_walk_animation_image [ i ] = 1 ;
    last_walk_animation_image [ i ] = 1 ;
    first_attack_animation_image [ i ] = 1 ;
    last_attack_animation_image [ i ] = 1 ;
    first_gethit_animation_image [ i ] = 1 ;
    last_gethit_animation_image [ i ] = 1 ;
    first_death_animation_image [ i ] = 1 ;
    last_death_animation_image [ i ] = 1 ;
    first_stand_animation_image [ i ] = 1 ;
    last_stand_animation_image [ i ] = 1 ;
    use_default_attack_image [ i ] = TRUE ;
    use_default_gethit_image [ i ] = TRUE ;
    use_default_death_image [ i ] = TRUE ;
    use_default_stand_image [ i ] = TRUE ;
    Druidmap [ i ] . suppress_bullet_generation_when_attacking = FALSE ;
    droid_walk_animation_speed_factor [ i ] = 5 ;
    droid_attack_animation_speed_factor [ i ] = 5 ;
    droid_gethit_animation_speed_factor [ i ] = 5 ;
    droid_death_animation_speed_factor [ i ] = 5 ;
    droid_stand_animation_speed_factor [ i ] = 5 ;
    ModelMultiplier  [ i ] = 1 ;i++;
    PrefixToFilename [ i ] = "629" ; // 16
    first_walk_animation_image [ i ] = 1 ;
    last_walk_animation_image [ i ] = 1 ;
    first_attack_animation_image [ i ] = 1 ;
    last_attack_animation_image [ i ] = 1 ;
    first_gethit_animation_image [ i ] = 1 ;
    last_gethit_animation_image [ i ] = 1 ;
    first_death_animation_image [ i ] = 1 ;
    last_death_animation_image [ i ] = 1 ;
    first_stand_animation_image [ i ] = 1 ;
    last_stand_animation_image [ i ] = 1 ;
    use_default_attack_image [ i ] = TRUE ;
    use_default_gethit_image [ i ] = TRUE ;
    use_default_death_image [ i ] = TRUE ;
    use_default_stand_image [ i ] = TRUE ;
    Druidmap [ i ] . suppress_bullet_generation_when_attacking = FALSE ;
    droid_walk_animation_speed_factor [ i ] = 5 ;
    droid_attack_animation_speed_factor [ i ] = 5 ;
    droid_gethit_animation_speed_factor [ i ] = 5 ;
    droid_death_animation_speed_factor [ i ] = 5 ;
    droid_stand_animation_speed_factor [ i ] = 5 ;
    ModelMultiplier  [ i ] = 1 ;i++;
    PrefixToFilename [ i ] = "711" ; // 17
    first_walk_animation_image [ i ] = 1 ;
    last_walk_animation_image [ i ] = 1 ;
    first_attack_animation_image [ i ] = 1 ;
    last_attack_animation_image [ i ] = 1 ;
    first_gethit_animation_image [ i ] = 1 ;
    last_gethit_animation_image [ i ] = 1 ;
    first_death_animation_image [ i ] = 1 ;
    last_death_animation_image [ i ] = 1 ;
    first_stand_animation_image [ i ] = 1 ;
    last_stand_animation_image [ i ] = 1 ;
    use_default_attack_image [ i ] = TRUE ;
    use_default_gethit_image [ i ] = TRUE ;
    use_default_death_image [ i ] = TRUE ;
    use_default_stand_image [ i ] = TRUE ;
    Druidmap [ i ] . suppress_bullet_generation_when_attacking = FALSE ;
    droid_walk_animation_speed_factor [ i ] = 5 ;
    droid_attack_animation_speed_factor [ i ] = 5 ;
    droid_gethit_animation_speed_factor [ i ] = 5 ;
    droid_death_animation_speed_factor [ i ] = 5 ;
    droid_stand_animation_speed_factor [ i ] = 5 ;
    ModelMultiplier  [ i ] = 1 ;i++;
    PrefixToFilename [ i ] = "742" ; // 18
    first_walk_animation_image [ i ] = 1 ;
    last_walk_animation_image [ i ] = 1 ;
    first_attack_animation_image [ i ] = 1 ;
    last_attack_animation_image [ i ] = 1 ;
    first_gethit_animation_image [ i ] = 1 ;
    last_gethit_animation_image [ i ] = 1 ;
    first_death_animation_image [ i ] = 1 ;
    last_death_animation_image [ i ] = 1 ;
    first_stand_animation_image [ i ] = 1 ;
    last_stand_animation_image [ i ] = 1 ;
    use_default_attack_image [ i ] = TRUE ;
    use_default_gethit_image [ i ] = TRUE ;
    use_default_death_image [ i ] = TRUE ;
    use_default_stand_image [ i ] = TRUE ;
    Druidmap [ i ] . suppress_bullet_generation_when_attacking = FALSE ;
    droid_walk_animation_speed_factor [ i ] = 5 ;
    droid_attack_animation_speed_factor [ i ] = 5 ;
    droid_gethit_animation_speed_factor [ i ] = 5 ;
    droid_death_animation_speed_factor [ i ] = 5 ;
    droid_stand_animation_speed_factor [ i ] = 5 ;
    ModelMultiplier  [ i ] = 1 ;i++;
    PrefixToFilename [ i ] = "751" ; // 19
    first_walk_animation_image [ i ] = 1 ;
    last_walk_animation_image [ i ] = 1 ;
    first_attack_animation_image [ i ] = 1 ;
    last_attack_animation_image [ i ] = 1 ;
    first_gethit_animation_image [ i ] = 1 ;
    last_gethit_animation_image [ i ] = 1 ;
    first_death_animation_image [ i ] = 1 ;
    last_death_animation_image [ i ] = 1 ;
    first_stand_animation_image [ i ] = 1 ;
    last_stand_animation_image [ i ] = 1 ;
    use_default_attack_image [ i ] = TRUE ;
    use_default_gethit_image [ i ] = TRUE ;
    use_default_death_image [ i ] = TRUE ;
    use_default_stand_image [ i ] = TRUE ;
    Druidmap [ i ] . suppress_bullet_generation_when_attacking = FALSE ;
    droid_walk_animation_speed_factor [ i ] = 5 ;
    droid_attack_animation_speed_factor [ i ] = 5 ;
    droid_gethit_animation_speed_factor [ i ] = 5 ;
    droid_death_animation_speed_factor [ i ] = 5 ;
    droid_stand_animation_speed_factor [ i ] = 5 ;
    ModelMultiplier  [ i ] = 1 ;i++;
    PrefixToFilename [ i ] = "821" ; // 20
    first_walk_animation_image [ i ] = 1 ;
    last_walk_animation_image [ i ] = 1 ;
    first_attack_animation_image [ i ] = 1 ;
    last_attack_animation_image [ i ] = 1 ;
    first_gethit_animation_image [ i ] = 1 ;
    last_gethit_animation_image [ i ] = 1 ;
    first_death_animation_image [ i ] = 1 ;
    last_death_animation_image [ i ] = 1 ;
    first_stand_animation_image [ i ] = 1 ;
    last_stand_animation_image [ i ] = 1 ;
    use_default_attack_image [ i ] = TRUE ;
    use_default_gethit_image [ i ] = TRUE ;
    use_default_death_image [ i ] = TRUE ;
    use_default_stand_image [ i ] = TRUE ;
    Druidmap [ i ] . suppress_bullet_generation_when_attacking = FALSE ;
    droid_walk_animation_speed_factor [ i ] = 5 ;
    droid_attack_animation_speed_factor [ i ] = 5 ;
    droid_gethit_animation_speed_factor [ i ] = 5 ;
    droid_death_animation_speed_factor [ i ] = 5 ;
    droid_stand_animation_speed_factor [ i ] = 5 ;
    ModelMultiplier  [ i ] = 1 ;i++;
    PrefixToFilename [ i ] = "834" ; // 21
    first_walk_animation_image [ i ] = 1 ;
    last_walk_animation_image [ i ] = 1 ;
    first_attack_animation_image [ i ] = 1 ;
    last_attack_animation_image [ i ] = 1 ;
    first_gethit_animation_image [ i ] = 1 ;
    last_gethit_animation_image [ i ] = 1 ;
    first_death_animation_image [ i ] = 1 ;
    last_death_animation_image [ i ] = 1 ;
    first_stand_animation_image [ i ] = 1 ;
    last_stand_animation_image [ i ] = 1 ;
    use_default_attack_image [ i ] = TRUE ;
    use_default_gethit_image [ i ] = TRUE ;
    use_default_death_image [ i ] = TRUE ;
    use_default_stand_image [ i ] = TRUE ;
    Druidmap [ i ] . suppress_bullet_generation_when_attacking = FALSE ;
    droid_walk_animation_speed_factor [ i ] = 5 ;
    droid_attack_animation_speed_factor [ i ] = 5 ;
    droid_gethit_animation_speed_factor [ i ] = 5 ;
    droid_death_animation_speed_factor [ i ] = 5 ;
    droid_stand_animation_speed_factor [ i ] = 5 ;
    ModelMultiplier  [ i ] = 1 ;i++;
    
    PrefixToFilename [ i ] = "883" ; // 22, the 'dallek' model...
    //--------------------
    // As the 999 now uses an image collection file, the information
    // about the first and last animation images will be taken from
    // the image collection archive anyway, so no need to hard-code
    // anything here and changes in the image collection file will
    // take effect immediately without code adaption...
    //
    // first_walk_animation_image [ i ] = 1 ;
    // last_walk_animation_image [ i ] = 1 ;
    // first_attack_animation_image [ i ] = 1 ;
    // last_attack_animation_image [ i ] = 1 ;
    // first_gethit_animation_image [ i ] = 1 ;
    // last_gethit_animation_image [ i ] = 1 ;
    // first_death_animation_image [ i ] = 1 ;
    // last_death_animation_image [ i ] = 1 ;
    // first_stand_animation_image [ i ] = 1 ;
    // last_stand_animation_image [ i ] = 1 ;
    // use_default_attack_image [ i ] = TRUE ;
    // use_default_gethit_image [ i ] = TRUE ;
    // use_default_death_image [ i ] = TRUE ;
    // use_default_stand_image [ i ] = TRUE ;
    Druidmap [ i ] . suppress_bullet_generation_when_attacking = FALSE ;
    Druidmap [ i ] . use_image_archive_file = TRUE ;
    droid_walk_animation_speed_factor [ i ] = 5 ;
    droid_attack_animation_speed_factor [ i ] = 10 ;
    droid_gethit_animation_speed_factor [ i ] = 5 ;
    droid_death_animation_speed_factor [ i ] = 5 ;
    droid_stand_animation_speed_factor [ i ] = 5 ;
    ModelMultiplier  [ i ] = 1 ;i++;
    
    PrefixToFilename [ i ] = "999" ; // 23
    //--------------------
    // As the 999 now uses an image collection file, the information
    // about the first and last animation images will be taken from
    // the image collection archive anyway, so no need to hard-code
    // anything here and changes in the image collection file will
    // take effect immediately without code adaption...
    //
    // first_walk_animation_image [ i ] = 1 ;
    // last_walk_animation_image [ i ] = 1 ;
    // first_attack_animation_image [ i ] = 1 ;
    // last_attack_animation_image [ i ] = 1 ;
    // first_gethit_animation_image [ i ] = 1 ;
    // last_gethit_animation_image [ i ] = 1 ;
    // first_death_animation_image [ i ] = 1 ;
    // last_death_animation_image [ i ] = 1 ;
    // first_stand_animation_image [ i ] = 1 ;
    // last_stand_animation_image [ i ] = 1 ;
    // use_default_attack_image [ i ] = TRUE ;
    // use_default_gethit_image [ i ] = TRUE ;
    // use_default_death_image [ i ] = TRUE ;
    // use_default_stand_image [ i ] = TRUE ;
    Druidmap [ i ] . suppress_bullet_generation_when_attacking = FALSE ;
    Druidmap [ i ] . use_image_archive_file = TRUE ;
    droid_walk_animation_speed_factor [ i ] = 5 ;
    droid_attack_animation_speed_factor [ i ] = 5 ;
    droid_gethit_animation_speed_factor [ i ] = 5 ;
    droid_death_animation_speed_factor [ i ] = 5 ;
    droid_stand_animation_speed_factor [ i ] = 5 ;
    ModelMultiplier  [ i ] = 1 ;i++;
    
    PrefixToFilename [ i ] = "professor" ; // 24
    //--------------------
    // As the professor now uses an image collection file, the information
    // about the first and last animation images will be taken from
    // the image collection archive anyway, so no need to hard-code
    // anything here and changes in the image collection file will
    // take effect immediately without code adaption...
    //
    // first_walk_animation_image [ i ] = 1 ;
    // last_walk_animation_image [ i ] = 5 ;
    // first_attack_animation_image [ i ] = 6 ;
    // last_attack_animation_image [ i ] = 6 ;
    // first_gethit_animation_image [ i ] = 7 ;
    // last_gethit_animation_image [ i ] = 7 ;
    // first_death_animation_image [ i ] = 8 ;
    // last_death_animation_image [ i ] = 8 ;
    // first_stand_animation_image [ i ] = 9 ;
    // last_stand_animation_image [ i ] = 13 ;
    // use_default_attack_image [ i ] = TRUE ;
    // use_default_gethit_image [ i ] = TRUE ;
    // use_default_death_image [ i ] = TRUE ;
    // use_default_stand_image [ i ] = FALSE ;
    Druidmap [ i ] . suppress_bullet_generation_when_attacking = FALSE ;
    Druidmap [ i ] . use_image_archive_file = TRUE ;
    droid_walk_animation_speed_factor [ i ] = 8 ;
    droid_attack_animation_speed_factor [ i ] = 5 ;
    droid_gethit_animation_speed_factor [ i ] = 5 ;
    droid_death_animation_speed_factor [ i ] = 5 ;
    droid_stand_animation_speed_factor [ i ] = 2 ;
    ModelMultiplier  [ i ] = 1 ;i++;
    
    PrefixToFilename [ i ] = "red_guard" ; // 25
    //--------------------
    // As the red_guard now uses an image collection file, the information
    // about the first and last animation images will be taken from
    // the image collection archive anyway, so no need to hard-code
    // anything here and changes in the image collection file will
    // take effect immediately without code adaption...
    //
    // first_walk_animation_image [ i ] = 1 ;
    // last_walk_animation_image [ i ] = 5 ;
    // first_attack_animation_image [ i ] = 6 ;
    // last_attack_animation_image [ i ] = 10 ;
    // first_gethit_animation_image [ i ] = 11 ;
    // last_gethit_animation_image [ i ] = 15 ;
    // first_death_animation_image [ i ] = 16 ;
    // last_death_animation_image [ i ] = 20 ;
    // first_stand_animation_image [ i ] = 21 ;
    // last_stand_animation_image [ i ] = 25 ;
    // use_default_attack_image [ i ] = FALSE ;
    // use_default_gethit_image [ i ] = FALSE ;
    // use_default_death_image [ i ] = FALSE ;
    // use_default_stand_image [ i ] = FALSE ;
    Druidmap [ i ] . suppress_bullet_generation_when_attacking = FALSE ;
    Druidmap [ i ] . use_image_archive_file = TRUE ;
    droid_walk_animation_speed_factor [ i ] = 8 ;
    droid_attack_animation_speed_factor [ i ] = 8 ;
    droid_gethit_animation_speed_factor [ i ] = 8 ;
    droid_death_animation_speed_factor [ i ] = 7 ;
    droid_stand_animation_speed_factor [ i ] = 2 ;
    ModelMultiplier  [ i ] = 1 ; i++;
    
    PrefixToFilename [ i ] = "hot_mama" ; // 26
    //--------------------
    // As the hot_mama now uses an image collection file, the information
    // about the first and last animation images will be taken from
    // the image collection archive anyway, so no need to hard-code
    // anything here and changes in the image collection file will
    // take effect immediately without code adaption...
    //
    // first_walk_animation_image [ i ] = 1 ;
    // last_walk_animation_image [ i ] = 5 ;
    // first_attack_animation_image [ i ] = 6 ;
    // last_attack_animation_image [ i ] = 6 ;
    // first_gethit_animation_image [ i ] = 7 ;
    // last_gethit_animation_image [ i ] = 7 ;
    // first_death_animation_image [ i ] = 8 ;
    // last_death_animation_image [ i ] = 8 ;
    // first_stand_animation_image [ i ] = 9 ;
    // last_stand_animation_image [ i ] = 13 ;
    // use_default_attack_image [ i ] = TRUE ;
    // use_default_gethit_image [ i ] = TRUE ;
    // use_default_death_image [ i ] = TRUE ;
    // use_default_stand_image [ i ] = FALSE ;
    Druidmap [ i ] . suppress_bullet_generation_when_attacking = FALSE ;
    Druidmap [ i ] . use_image_archive_file = TRUE ;
    droid_walk_animation_speed_factor [ i ] = 5 ;
    droid_attack_animation_speed_factor [ i ] = 5 ;
    droid_gethit_animation_speed_factor [ i ] = 5 ;
    droid_death_animation_speed_factor [ i ] = 5 ;
    droid_stand_animation_speed_factor [ i ] = 1 ;
    ModelMultiplier  [ i ] = 1 ;i++;
    
    PrefixToFilename [ i ] = "female_scientist" ; // 27
    //--------------------
    // As the female_scientist now uses an image collection file, the information
    // about the first and last animation images will be taken from
    // the image collection archive anyway, so no need to hard-code
    // anything here and changes in the image collection file will
    // take effect immediately without code adaption...
    //
    // first_walk_animation_image [ i ] = 1 ;
    // last_walk_animation_image [ i ] = 5 ;
    // first_attack_animation_image [ i ] = 6 ;
    // last_attack_animation_image [ i ] = 6 ;
    // first_gethit_animation_image [ i ] = 7 ;
    // last_gethit_animation_image [ i ] = 7 ;
    // first_death_animation_image [ i ] = 8 ;
    // last_death_animation_image [ i ] = 8 ;
    // first_stand_animation_image [ i ] = 9 ;
    // last_stand_animation_image [ i ] = 13 ;
    // use_default_attack_image [ i ] = TRUE ;
    // use_default_gethit_image [ i ] = TRUE ;
    // use_default_death_image [ i ] = TRUE ;
    // use_default_stand_image [ i ] = FALSE ;
    Druidmap [ i ] . suppress_bullet_generation_when_attacking = FALSE ;
    Druidmap [ i ] . use_image_archive_file = TRUE ;
    droid_walk_animation_speed_factor [ i ] = 5 ;
    droid_attack_animation_speed_factor [ i ] = 5 ;
    droid_gethit_animation_speed_factor [ i ] = 5 ;
    droid_death_animation_speed_factor [ i ] = 5 ;
    droid_stand_animation_speed_factor [ i ] = 1 ;
    ModelMultiplier  [ i ] = 1 ;i++;
    
    /*
    PrefixToFilename [ i ] = "brown_worker" ; // 28
    first_walk_animation_image [ i ] = 1 ;
    last_walk_animation_image [ i ] = 1 ;
    first_attack_animation_image [ i ] = 1 ;
    last_attack_animation_image [ i ] = 1 ;
    first_gethit_animation_image [ i ] = 1 ;
    last_gethit_animation_image [ i ] = 1 ;
    first_death_animation_image [ i ] = 1 ;
    last_death_animation_image [ i ] = 1 ;
    first_stand_animation_image [ i ] = 1 ;
    last_stand_animation_image [ i ] = 1 ;
    use_default_attack_image [ i ] = TRUE ;
    use_default_gethit_image [ i ] = TRUE ;
    use_default_death_image [ i ] = TRUE ;
    use_default_stand_image [ i ] = TRUE ;
    Druidmap [ i ] . suppress_bullet_generation_when_attacking = FALSE ;
    droid_walk_animation_speed_factor [ i ] = 5 ;
    droid_attack_animation_speed_factor [ i ] = 5 ;
    droid_gethit_animation_speed_factor [ i ] = 5 ;
    droid_death_animation_speed_factor [ i ] = 5 ;
    droid_stand_animation_speed_factor [ i ] = 5 ;
    ModelMultiplier  [ i ] = 1 ;i++;
    */

    PrefixToFilename [ i ] = "default_male" ; // 28
    //--------------------
    // As the female_scientist now uses an image collection file, the information
    // about the first and last animation images will be taken from
    // the image collection archive anyway, so no need to hard-code
    // anything here and changes in the image collection file will
    // take effect immediately without code adaption...
    //
    // first_walk_animation_image [ i ] = 1 ;
    // last_walk_animation_image [ i ] = 1 ;
    // first_attack_animation_image [ i ] = 1 ;
    // last_attack_animation_image [ i ] = 1 ;
    // first_gethit_animation_image [ i ] = 1 ;
    // last_gethit_animation_image [ i ] = 1 ;
    // first_death_animation_image [ i ] = 1 ;
    // last_death_animation_image [ i ] = 1 ;
    // first_stand_animation_image [ i ] = 1 ;
    // last_stand_animation_image [ i ] = 1 ;
    // use_default_attack_image [ i ] = TRUE ;
    // use_default_gethit_image [ i ] = TRUE ;
    // use_default_death_image [ i ] = TRUE ;
    // use_default_stand_image [ i ] = TRUE ;
    Druidmap [ i ] . suppress_bullet_generation_when_attacking = FALSE ;
    Druidmap [ i ] . use_image_archive_file = TRUE ;
    droid_walk_animation_speed_factor [ i ] = 5 ;
    droid_attack_animation_speed_factor [ i ] = 5 ;
    droid_gethit_animation_speed_factor [ i ] = 5 ;
    droid_death_animation_speed_factor [ i ] = 5 ;
    droid_stand_animation_speed_factor [ i ] = 5 ;
    ModelMultiplier  [ i ] = 1 ;i++;
    

    PrefixToFilename [ i ] = "blue_guard" ; // 29
    //--------------------
    // As the female_scientist now uses an image collection file, the information
    // about the first and last animation images will be taken from
    // the image collection archive anyway, so no need to hard-code
    // anything here and changes in the image collection file will
    // take effect immediately without code adaption...
    //
    // first_walk_animation_image [ i ] = 1 ;
    // last_walk_animation_image [ i ] = 1 ;
    // first_attack_animation_image [ i ] = 1 ;
    // last_attack_animation_image [ i ] = 1 ;
    // first_gethit_animation_image [ i ] = 1 ;
    // last_gethit_animation_image [ i ] = 1 ;
    // first_death_animation_image [ i ] = 1 ;
    // last_death_animation_image [ i ] = 1 ;
    // first_stand_animation_image [ i ] = 1 ;
    // last_stand_animation_image [ i ] = 1 ;
    // use_default_attack_image [ i ] = TRUE ;
    // use_default_gethit_image [ i ] = TRUE ;
    // use_default_death_image [ i ] = TRUE ;
    // use_default_stand_image [ i ] = TRUE ;
    Druidmap [ i ] . suppress_bullet_generation_when_attacking = FALSE ;
    Druidmap [ i ] . use_image_archive_file = TRUE ;
    droid_walk_animation_speed_factor [ i ] = 5 ;
    droid_attack_animation_speed_factor [ i ] = 5 ;
    droid_gethit_animation_speed_factor [ i ] = 5 ;
    droid_death_animation_speed_factor [ i ] = 5 ;
    droid_stand_animation_speed_factor [ i ] = 5 ;
    ModelMultiplier  [ i ] = 1 ;i++;
    
    PrefixToFilename [ i ] = "green_guard" ; // 30
    first_walk_animation_image [ i ] = 1 ;
    last_walk_animation_image [ i ] = 1 ;
    first_attack_animation_image [ i ] = 1 ;
    last_attack_animation_image [ i ] = 1 ;
    first_gethit_animation_image [ i ] = 1 ;
    last_gethit_animation_image [ i ] = 1 ;
    first_death_animation_image [ i ] = 1 ;
    last_death_animation_image [ i ] = 1 ;
    first_stand_animation_image [ i ] = 1 ;
    last_stand_animation_image [ i ] = 1 ;
    use_default_attack_image [ i ] = TRUE ;
    use_default_gethit_image [ i ] = TRUE ;
    use_default_death_image [ i ] = TRUE ;
    use_default_stand_image [ i ] = TRUE ;
    droid_walk_animation_speed_factor [ i ] = 5 ;
    droid_attack_animation_speed_factor [ i ] = 5 ;
    droid_gethit_animation_speed_factor [ i ] = 5 ;
    droid_death_animation_speed_factor [ i ] = 5 ;
    droid_stand_animation_speed_factor [ i ] = 5 ;
    ModelMultiplier  [ i ] = 1 ; i++;
    
    PrefixToFilename [ i ] = "cook" ; // 31
    //--------------------
    // As the cook now uses an image collection file, the information
    // about the first and last animation images will be taken from
    // the image collection archive anyway, so no need to hard-code
    // anything here and changes in the image collection file will
    // take effect immediately without code adaption...
    //
    // first_walk_animation_image [ i ] = 1 ;
    // last_walk_animation_image [ i ] = 5 ;
    // first_attack_animation_image [ i ] = 6 ;
    // last_attack_animation_image [ i ] = 6 ;
    // first_gethit_animation_image [ i ] = 7 ;
    // last_gethit_animation_image [ i ] = 7 ;
    // first_death_animation_image [ i ] = 8 ;
    // last_death_animation_image [ i ] = 8 ;
    // first_stand_animation_image [ i ] = 9 ;
    // last_stand_animation_image [ i ] = 13 ;
    // use_default_attack_image [ i ] = TRUE ;
    // use_default_gethit_image [ i ] = TRUE ;
    // use_default_death_image [ i ] = TRUE ;
    // use_default_stand_image [ i ] = FALSE ;
    Druidmap [ i ] . suppress_bullet_generation_when_attacking = FALSE ;
    Druidmap [ i ] . use_image_archive_file = TRUE ;
    droid_walk_animation_speed_factor [ i ] = 5 ;
    droid_attack_animation_speed_factor [ i ] = 5 ;
    droid_gethit_animation_speed_factor [ i ] = 5 ;
    droid_death_animation_speed_factor [ i ] = 5 ;
    droid_stand_animation_speed_factor [ i ] = 1 ;
    ModelMultiplier  [ i ] = 1 ;i++;

    PrefixToFilename [ i ] = "kevin" ; // 32
    //--------------------
    // As the kevin model now uses an image collection file, the information
    // about the first and last animation images will be taken from
    // the image collection archive anyway, so no need to hard-code
    // anything here and changes in the image collection file will
    // take effect immediately without code adaption...
    //
    // first_walk_animation_image [ i ] = 1 ;
    // last_walk_animation_image [ i ] = 5 ;
    // first_attack_animation_image [ i ] = 6 ;
    // last_attack_animation_image [ i ] = 6 ;
    // first_gethit_animation_image [ i ] = 7 ;
    // last_gethit_animation_image [ i ] = 7 ;
    // first_death_animation_image [ i ] = 8 ;
    // last_death_animation_image [ i ] = 8 ;
    // first_stand_animation_image [ i ] = 9 ;
    // last_stand_animation_image [ i ] = 13 ;
    // use_default_attack_image [ i ] = TRUE ;
    // use_default_gethit_image [ i ] = TRUE ;
    // use_default_death_image [ i ] = TRUE ;
    // use_default_stand_image [ i ] = FALSE ;
    Druidmap [ i ] . suppress_bullet_generation_when_attacking = TRUE ;
    Druidmap [ i ] . use_image_archive_file = TRUE ;
    droid_walk_animation_speed_factor [ i ] = 8 ;
    droid_attack_animation_speed_factor [ i ] = 10 ;
    droid_gethit_animation_speed_factor [ i ] = 10 ;
    droid_death_animation_speed_factor [ i ] = 5 ;
    droid_stand_animation_speed_factor [ i ] = 6 ;
    ModelMultiplier  [ i ] = 1 ;i++;
        
    PrefixToFilename [ i ] = "jasmine" ; // 33
    //--------------------
    // As the kevin model now uses an image collection file, the information
    // about the first and last animation images will be taken from
    // the image collection archive anyway, so no need to hard-code
    // anything here and changes in the image collection file will
    // take effect immediately without code adaption...
    //
    // first_walk_animation_image [ i ] = 1 ;
    // last_walk_animation_image [ i ] = 5 ;
    // first_attack_animation_image [ i ] = 6 ;
    // last_attack_animation_image [ i ] = 6 ;
    // first_gethit_animation_image [ i ] = 7 ;
    // last_gethit_animation_image [ i ] = 7 ;
    // first_death_animation_image [ i ] = 8 ;
    // last_death_animation_image [ i ] = 8 ;
    // first_stand_animation_image [ i ] = 9 ;
    // last_stand_animation_image [ i ] = 13 ;
    // use_default_attack_image [ i ] = TRUE ;
    // use_default_gethit_image [ i ] = TRUE ;
    // use_default_death_image [ i ] = TRUE ;
    // use_default_stand_image [ i ] = FALSE ;
    Druidmap [ i ] . suppress_bullet_generation_when_attacking = TRUE ;
    Druidmap [ i ] . use_image_archive_file = TRUE ;
    droid_walk_animation_speed_factor [ i ] = 8 ;
    droid_attack_animation_speed_factor [ i ] = 10 ;
    droid_gethit_animation_speed_factor [ i ] = 10 ;
    droid_death_animation_speed_factor [ i ] = 5 ;
    droid_stand_animation_speed_factor [ i ] = 6 ;
    ModelMultiplier  [ i ] = 1 ;i++;

    //--------------------
    // Finally we do some test to make sure we don't write
    // over the bounds of our array or so
    //
    if ( i > ENEMY_ROTATION_MODELS_AVAILABLE )
    {
	GiveStandardErrorMessage ( __FUNCTION__  , "\
There seem to be a bit more models mentioned in the game than\n\
currently allowed in defs.h (ENEMY_ROTATION_MODELS_AVAILABLE).\n\
This should be investigated as soon as possible.",
				   PLEASE_INFORM, IS_FATAL );
    }
}; // void LoadEnemySurfaces( void )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
init_obstacle_data( void )
{
    int i;
    float standard_wall_thickness = 0.5 ;
    iso_image empty_iso_image = UNLOADED_ISO_IMAGE ;
    
    //--------------------
    // First we enter some default values.  The exceptions from the default values
    // can be added after that.
    //
    for ( i = 0 ; i < NUMBER_OF_OBSTACLE_TYPES ; i ++ )
    {
	//--------------------
	// In adition to the pure image information, we'll also need some
	// collision information for obstacles...
	//
	memcpy ( & ( obstacle_map [ i ] . image ) , & ( empty_iso_image ) , sizeof ( iso_image ) );
	
	obstacle_map [ i ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
	obstacle_map [ i ] . block_vision_too = TRUE ;
	obstacle_map [ i ] . block_area_parm_1 = 1.2 ;
	obstacle_map [ i ] . block_area_parm_2 = 1.2 ; // standard_wall_thickness ;
	obstacle_map [ i ] . is_smashable = FALSE ;
	obstacle_map [ i ] . can_be_looted = FALSE ;  // is this where treasure could be hidden?
	obstacle_map [ i ] . result_type_after_smashing_once = (-1) ;
	obstacle_map [ i ] . drop_random_treasure = FALSE ;
	obstacle_map [ i ] . needs_pre_put = FALSE ;
	obstacle_map [ i ] . transparent = FALSE ;
	obstacle_map [ i ] . emitted_light_strength = 0 ; // how much light emitted from here...

	obstacle_map [ i ] . filename = MyMalloc ( 100 ); // that should be sufficient for file names...
	sprintf ( obstacle_map [ i ] . filename , "iso_obstacle_%04d.png" , i );

	obstacle_map [ i ] . obstacle_short_name = MyMalloc ( 100 ); // that should be sufficient for most short names...
	sprintf ( obstacle_map [ i ] . obstacle_short_name , "unnamed_obstacle" );

	obstacle_map [ i ] . obstacle_long_description = MyMalloc ( 1000 ); // that should be sufficient for most long descriptions...
	sprintf ( obstacle_map [ i ] . obstacle_long_description , "This undescribed obstacle is still waiting for some contribution to its description.....");


    }
    //--------------------
    // Now we define all exceptions from the default values
    //
    obstacle_map [ ISO_V_WALL ] . block_area_parm_1 = standard_wall_thickness ;
    obstacle_map [ ISO_V_WALL ] . block_area_parm_2 = 1.2 ;
    obstacle_map [ ISO_V_WALL ] . filename = "iso_walls_0001.png" ;
    obstacle_map [ ISO_V_WALL ] . transparent = TRANSPARENCY_FOR_WALLS ;
    obstacle_map [ ISO_V_WALL ] . obstacle_short_name = "Wall";
    obstacle_map [ ISO_V_WALL ] . obstacle_long_description = "Hey ! This looks like...a WALL !";
    obstacle_map [ ISO_H_WALL ] . block_area_parm_1 = 1.2 ;
    obstacle_map [ ISO_H_WALL ] . block_area_parm_2 = standard_wall_thickness ;
    obstacle_map [ ISO_H_WALL ] . filename = "iso_walls_0002.png" ;
    obstacle_map [ ISO_H_WALL ] . transparent = TRANSPARENCY_FOR_WALLS;
    obstacle_map [ ISO_H_WALL ] . obstacle_short_name = "Wall";
    obstacle_map [ ISO_H_WALL ] . obstacle_long_description = "Hey ! This looks like...a WALL !";
        
    obstacle_map [ ISO_V_WALL_WITH_DOT ] . block_area_parm_1 = standard_wall_thickness ;
    obstacle_map [ ISO_V_WALL_WITH_DOT ] . block_area_parm_2 = 1.2 ;
    obstacle_map [ ISO_V_WALL_WITH_DOT ] . filename = "iso_walls_0003.png"; 
    obstacle_map [ ISO_V_WALL_WITH_DOT ] . transparent = TRANSPARENCY_FOR_WALLS ;
    obstacle_map [ ISO_V_WALL_WITH_DOT ] . obstacle_short_name = "Wall";
    obstacle_map [ ISO_V_WALL_WITH_DOT ] . obstacle_long_description = "Maybe you could use a door to go out. This wall seems pretty solid.";
    obstacle_map [ ISO_H_WALL_WITH_DOT ] . block_area_parm_1 = 1.2 ;
    obstacle_map [ ISO_H_WALL_WITH_DOT ] . block_area_parm_2 = standard_wall_thickness ;
    obstacle_map [ ISO_H_WALL_WITH_DOT ] . filename = "iso_walls_0004.png" ;
    obstacle_map [ ISO_H_WALL_WITH_DOT ] . transparent = TRANSPARENCY_FOR_WALLS ;
    obstacle_map [ ISO_H_WALL_WITH_DOT ] . obstacle_short_name = obstacle_map [ ISO_V_WALL_WITH_DOT ] . obstacle_short_name;
    obstacle_map [ ISO_H_WALL_WITH_DOT ] . obstacle_long_description = obstacle_map [ ISO_V_WALL_WITH_DOT ] . obstacle_long_description;
        
    obstacle_map [ ISO_GLASS_WALL_1 ] . block_area_parm_1 = standard_wall_thickness ;
    obstacle_map [ ISO_GLASS_WALL_1 ] . block_area_parm_2 = 1.2 ;
    obstacle_map [ ISO_GLASS_WALL_1 ] . block_vision_too = FALSE ;
    obstacle_map [ ISO_GLASS_WALL_1 ] . filename = "iso_walls_0020.png" ;
    obstacle_map [ ISO_GLASS_WALL_1 ] . transparent = TRANSPARENCY_FOR_SEE_THROUGH_OBJECTS ;
    obstacle_map [ ISO_GLASS_WALL_1 ] . obstacle_short_name = "Glass wall";
    obstacle_map [ ISO_GLASS_WALL_1 ] . obstacle_long_description = "A wall made out of glass. It lets the light go through.";
    
    obstacle_map [ ISO_GLASS_WALL_2 ] . block_area_parm_1 = 1.2 ;
    obstacle_map [ ISO_GLASS_WALL_2 ] . block_area_parm_2 = standard_wall_thickness ;
    obstacle_map [ ISO_GLASS_WALL_2 ] . block_vision_too = FALSE ;
    obstacle_map [ ISO_GLASS_WALL_2 ] . filename = "iso_walls_0021.png" ;
    obstacle_map [ ISO_GLASS_WALL_2 ] . transparent = TRANSPARENCY_FOR_SEE_THROUGH_OBJECTS ;
    obstacle_map [ ISO_GLASS_WALL_2 ] . obstacle_short_name = "Glass wall";
    obstacle_map [ ISO_GLASS_WALL_2 ] . obstacle_long_description = "A wall made out of glass. It lets the light go through."; 
  
    obstacle_map [ ISO_CYAN_WALL_WINDOW_1 ] . block_area_parm_1 = standard_wall_thickness ;
    obstacle_map [ ISO_CYAN_WALL_WINDOW_1 ] . block_area_parm_2 = 1.2 ;
    obstacle_map [ ISO_CYAN_WALL_WINDOW_1 ] . block_vision_too = FALSE ;
    obstacle_map [ ISO_CYAN_WALL_WINDOW_1 ] . filename = "iso_walls_0022.png" ;
    obstacle_map [ ISO_CYAN_WALL_WINDOW_1 ] . transparent = TRANSPARENCY_FOR_SEE_THROUGH_OBJECTS ;
    obstacle_map [ ISO_CYAN_WALL_WINDOW_2 ] . block_area_parm_1 = 1.2 ;
    obstacle_map [ ISO_CYAN_WALL_WINDOW_2 ] . block_area_parm_2 = standard_wall_thickness ;
    obstacle_map [ ISO_CYAN_WALL_WINDOW_2 ] . block_vision_too = FALSE ;
    obstacle_map [ ISO_CYAN_WALL_WINDOW_2 ] . filename = "iso_walls_0023.png" ;
    obstacle_map [ ISO_CYAN_WALL_WINDOW_2 ] . transparent = TRANSPARENCY_FOR_SEE_THROUGH_OBJECTS ;
    
    obstacle_map [ ISO_RED_WALL_WINDOW_1 ] . block_area_parm_1 = standard_wall_thickness ;
    obstacle_map [ ISO_RED_WALL_WINDOW_1 ] . block_area_parm_2 = 1.2 ;
    obstacle_map [ ISO_RED_WALL_WINDOW_1 ] . block_vision_too = FALSE ;
    obstacle_map [ ISO_RED_WALL_WINDOW_1 ] . filename = "iso_walls_0024.png" ;
    obstacle_map [ ISO_RED_WALL_WINDOW_1 ] . transparent = TRANSPARENCY_FOR_SEE_THROUGH_OBJECTS ;
    obstacle_map [ ISO_RED_WALL_WINDOW_2 ] . block_area_parm_1 = 1.2 ;
    obstacle_map [ ISO_RED_WALL_WINDOW_2 ] . block_area_parm_2 = standard_wall_thickness ;
    obstacle_map [ ISO_RED_WALL_WINDOW_2 ] . block_vision_too = FALSE ;
    obstacle_map [ ISO_RED_WALL_WINDOW_2 ] . filename = "iso_walls_0025.png" ;
    obstacle_map [ ISO_RED_WALL_WINDOW_2 ] . transparent = TRANSPARENCY_FOR_SEE_THROUGH_OBJECTS ;
 
    
    obstacle_map [ ISO_FLOWER_WALL_WINDOW_1 ] . block_area_parm_1 = standard_wall_thickness ;
    obstacle_map [ ISO_FLOWER_WALL_WINDOW_1 ] . block_area_parm_2 = 1.2 ;
    obstacle_map [ ISO_FLOWER_WALL_WINDOW_1 ] . block_vision_too = FALSE ;
    obstacle_map [ ISO_FLOWER_WALL_WINDOW_1 ] . filename = "iso_walls_0026.png" ;
    obstacle_map [ ISO_FLOWER_WALL_WINDOW_1 ] . transparent = TRANSPARENCY_FOR_SEE_THROUGH_OBJECTS ;
    obstacle_map [ ISO_FLOWER_WALL_WINDOW_2 ] . block_area_parm_1 = 1.2 ;
    obstacle_map [ ISO_FLOWER_WALL_WINDOW_2 ] . block_area_parm_2 = standard_wall_thickness ;
    obstacle_map [ ISO_FLOWER_WALL_WINDOW_2 ] . block_vision_too = FALSE ;
    obstacle_map [ ISO_FLOWER_WALL_WINDOW_2 ] . filename = "iso_walls_0027.png" ;
    obstacle_map [ ISO_FLOWER_WALL_WINDOW_2 ] . transparent = TRANSPARENCY_FOR_SEE_THROUGH_OBJECTS ;
    
    obstacle_map [ ISO_FUNKY_WALL_WINDOW_1 ] . block_area_parm_1 = standard_wall_thickness ;
    obstacle_map [ ISO_FUNKY_WALL_WINDOW_1 ] . block_area_parm_2 = 1.2 ;
    obstacle_map [ ISO_FUNKY_WALL_WINDOW_1 ] . block_vision_too = FALSE ;
    obstacle_map [ ISO_FUNKY_WALL_WINDOW_1 ] . filename = "iso_walls_0028.png" ;
    obstacle_map [ ISO_FUNKY_WALL_WINDOW_1 ] . transparent = TRANSPARENCY_FOR_SEE_THROUGH_OBJECTS ;
    obstacle_map [ ISO_FUNKY_WALL_WINDOW_2 ] . block_area_parm_1 = 1.2 ;
    obstacle_map [ ISO_FUNKY_WALL_WINDOW_2 ] . block_area_parm_2 = standard_wall_thickness ;
    obstacle_map [ ISO_FUNKY_WALL_WINDOW_2 ] . block_vision_too = FALSE ;
    obstacle_map [ ISO_FUNKY_WALL_WINDOW_2 ] . filename = "iso_walls_0029.png" ;
    obstacle_map [ ISO_FUNKY_WALL_WINDOW_2 ] . transparent = TRANSPARENCY_FOR_SEE_THROUGH_OBJECTS ;
    
    //the eight windowed walls share the same description because i'm lazy and have no imagination
    obstacle_map [ ISO_CYAN_WALL_WINDOW_1 ] . obstacle_short_name = "Wall with a window";
    obstacle_map [ ISO_CYAN_WALL_WINDOW_1 ] . obstacle_long_description = "Windows, are, besides a famous joke OS trademark, very useful things."; 
    obstacle_map [ ISO_CYAN_WALL_WINDOW_2 ] . obstacle_short_name = "Wall with a window";
    obstacle_map [ ISO_CYAN_WALL_WINDOW_2 ] . obstacle_long_description = "Windows, are, besides a famous joke OS trademark, very useful things.";
    obstacle_map [ ISO_RED_WALL_WINDOW_1 ] . obstacle_short_name = "Wall with a window";
    obstacle_map [ ISO_RED_WALL_WINDOW_1 ] . obstacle_long_description = "Windows, are, besides a famous joke OS trademark, very useful things.";
    obstacle_map [ ISO_RED_WALL_WINDOW_2 ] . obstacle_short_name = "Wall with a window";
    obstacle_map [ ISO_RED_WALL_WINDOW_2 ] . obstacle_long_description = "Windows, are, besides a famous joke OS trademark, very useful things.";
    obstacle_map [ ISO_FLOWER_WALL_WINDOW_1 ] . obstacle_short_name = "Wall with a window";
    obstacle_map [ ISO_FLOWER_WALL_WINDOW_1 ] . obstacle_long_description = "Windows, are, besides a famous joke OS trademark, very useful things.";
    obstacle_map [ ISO_FLOWER_WALL_WINDOW_2 ] . obstacle_short_name = "Wall with a window";
    obstacle_map [ ISO_FLOWER_WALL_WINDOW_2 ] . obstacle_long_description = "Windows, are, besides a famous joke OS trademark, very useful things.";
    obstacle_map [ ISO_FUNKY_WALL_WINDOW_1 ] . obstacle_short_name = "Wall with a window";
    obstacle_map [ ISO_FUNKY_WALL_WINDOW_1 ] . obstacle_long_description = "Windows, are, besides a famous joke OS trademark, very useful things.";
    obstacle_map [ ISO_FUNKY_WALL_WINDOW_2 ] . obstacle_short_name = "Wall with a window";
    obstacle_map [ ISO_FUNKY_WALL_WINDOW_2 ] . obstacle_long_description = "Windows, are, besides a famous joke OS trademark, very useful things.";
    
    
    obstacle_map [ ISO_V_DOOR_000_OPEN ] . block_area_parm_1 = standard_wall_thickness ;
    obstacle_map [ ISO_V_DOOR_000_OPEN ] . block_area_parm_2 = 1.2 ;
    obstacle_map [ ISO_V_DOOR_000_OPEN ] . block_vision_too = FALSE ;
    obstacle_map [ ISO_V_DOOR_000_OPEN ] . filename = "iso_doors_0006.png" ;
    obstacle_map [ ISO_V_DOOR_000_OPEN ] . transparent = TRANSPARENCY_FOR_SEE_THROUGH_OBJECTS ;
    obstacle_map [ ISO_V_DOOR_025_OPEN ] . is_smashable = FALSE ;
    obstacle_map [ ISO_V_DOOR_025_OPEN ] . filename = "iso_doors_0007.png" ;
    obstacle_map [ ISO_V_DOOR_025_OPEN ] . transparent = TRANSPARENCY_FOR_SEE_THROUGH_OBJECTS ;
    obstacle_map [ ISO_V_DOOR_050_OPEN ] . is_smashable = FALSE ;
    obstacle_map [ ISO_V_DOOR_050_OPEN ] . filename = "iso_doors_0008.png" ;
    obstacle_map [ ISO_V_DOOR_050_OPEN ] . transparent = TRANSPARENCY_FOR_SEE_THROUGH_OBJECTS ;
    obstacle_map [ ISO_V_DOOR_075_OPEN ] . is_smashable = FALSE ;
    obstacle_map [ ISO_V_DOOR_075_OPEN ] . filename = "iso_doors_0009.png" ;
    obstacle_map [ ISO_V_DOOR_075_OPEN ] . transparent = TRANSPARENCY_FOR_SEE_THROUGH_OBJECTS ;
    obstacle_map [ ISO_V_DOOR_100_OPEN ] . is_smashable = FALSE ;
    obstacle_map [ ISO_V_DOOR_100_OPEN ] . filename = "iso_doors_0010.png" ;
    obstacle_map [ ISO_V_DOOR_100_OPEN ] . block_area_type = COLLISION_TYPE_NONE ;
    obstacle_map [ ISO_V_DOOR_100_OPEN ] . transparent = TRANSPARENCY_FOR_SEE_THROUGH_OBJECTS ;
    //would you find it abnormal if I told you the 8 non-locked doors have the same desc ?
    obstacle_map [ ISO_V_DOOR_000_OPEN ] . obstacle_short_name = "Door";
    obstacle_map [ ISO_V_DOOR_000_OPEN ] . obstacle_long_description = "This is a door. Seems to open automatically when someone approaches.";
    obstacle_map [ ISO_V_DOOR_025_OPEN ] . obstacle_short_name = obstacle_map [ ISO_V_DOOR_000_OPEN ] . obstacle_short_name;
    obstacle_map [ ISO_V_DOOR_025_OPEN ] . obstacle_long_description =     obstacle_map [ ISO_V_DOOR_000_OPEN ] . obstacle_long_description;
    obstacle_map [ ISO_V_DOOR_050_OPEN ] . obstacle_short_name = obstacle_map [ ISO_V_DOOR_000_OPEN ] . obstacle_short_name;
    obstacle_map [ ISO_V_DOOR_050_OPEN ] . obstacle_long_description =     obstacle_map [ ISO_V_DOOR_000_OPEN ] . obstacle_long_description;
    obstacle_map [ ISO_V_DOOR_075_OPEN ] . obstacle_short_name = obstacle_map [ ISO_V_DOOR_000_OPEN ] . obstacle_short_name;
    obstacle_map [ ISO_V_DOOR_075_OPEN ] . obstacle_long_description =     obstacle_map [ ISO_V_DOOR_000_OPEN ] . obstacle_long_description;
    obstacle_map [ ISO_V_DOOR_100_OPEN ] . obstacle_short_name = obstacle_map [ ISO_V_DOOR_000_OPEN ] . obstacle_short_name;
    obstacle_map [ ISO_V_DOOR_100_OPEN ] . obstacle_long_description =     obstacle_map [ ISO_V_DOOR_000_OPEN ] . obstacle_long_description;
    obstacle_map [ ISO_H_DOOR_000_OPEN ] . obstacle_short_name = obstacle_map [ ISO_V_DOOR_000_OPEN ] . obstacle_short_name;
    obstacle_map [ ISO_H_DOOR_000_OPEN ] . obstacle_long_description =     obstacle_map [ ISO_V_DOOR_000_OPEN ] . obstacle_long_description;
    obstacle_map [ ISO_H_DOOR_025_OPEN ] . obstacle_short_name = obstacle_map [ ISO_V_DOOR_000_OPEN ] . obstacle_short_name;
    obstacle_map [ ISO_H_DOOR_025_OPEN ] . obstacle_long_description =     obstacle_map [ ISO_V_DOOR_000_OPEN ] . obstacle_long_description;
    obstacle_map [ ISO_H_DOOR_050_OPEN ] . obstacle_short_name = obstacle_map [ ISO_V_DOOR_000_OPEN ] . obstacle_short_name;
    obstacle_map [ ISO_H_DOOR_050_OPEN ] . obstacle_long_description =     obstacle_map [ ISO_V_DOOR_000_OPEN ] . obstacle_long_description;
    obstacle_map [ ISO_H_DOOR_075_OPEN ] . obstacle_short_name = obstacle_map [ ISO_V_DOOR_000_OPEN ] . obstacle_short_name;
    obstacle_map [ ISO_H_DOOR_075_OPEN ] . obstacle_long_description =     obstacle_map [ ISO_V_DOOR_000_OPEN ] . obstacle_long_description;
    obstacle_map [ ISO_H_DOOR_100_OPEN ] . obstacle_short_name = obstacle_map [ ISO_V_DOOR_000_OPEN ] . obstacle_short_name;
    obstacle_map [ ISO_H_DOOR_100_OPEN ] . obstacle_long_description =     obstacle_map [ ISO_V_DOOR_000_OPEN ] . obstacle_long_description;
    
    obstacle_map [ ISO_V_DOOR_LOCKED ] . block_area_parm_1 = standard_wall_thickness ;
    obstacle_map [ ISO_V_DOOR_LOCKED ] . block_area_parm_2 = 1.2 ;
    obstacle_map [ ISO_V_DOOR_LOCKED ] . filename = "iso_doors_0012.png" ;
    obstacle_map [ ISO_V_DOOR_LOCKED ] . block_vision_too = FALSE ;
    obstacle_map [ ISO_V_DOOR_LOCKED ] . transparent = TRANSPARENCY_FOR_SEE_THROUGH_OBJECTS ;
    
    obstacle_map [ ISO_H_DOOR_LOCKED ] . filename = "iso_doors_0011.png" ;
    obstacle_map [ ISO_H_DOOR_LOCKED ] . block_area_parm_1 = 1.2 ;
    obstacle_map [ ISO_H_DOOR_LOCKED ] . block_area_parm_2 = standard_wall_thickness ;
    obstacle_map [ ISO_H_DOOR_LOCKED ] . block_vision_too = FALSE ;
    obstacle_map [ ISO_H_DOOR_LOCKED ] . transparent = TRANSPARENCY_FOR_SEE_THROUGH_OBJECTS ;
    //locked doors have the same description
    obstacle_map [ ISO_H_DOOR_LOCKED ] . obstacle_short_name = "Locked door";
    obstacle_map [ ISO_H_DOOR_LOCKED ] . obstacle_long_description = "The red color probably indicates that the door is locked.";
    obstacle_map [ ISO_V_DOOR_LOCKED ] . obstacle_short_name = obstacle_map [ ISO_H_DOOR_LOCKED ] . obstacle_short_name;
	obstacle_map [ ISO_V_DOOR_LOCKED ] . obstacle_long_description = obstacle_map [ ISO_H_DOOR_LOCKED ] . obstacle_long_description ;
    obstacle_map [ ISO_H_DOOR_000_OPEN ] . is_smashable = FALSE ;
    obstacle_map [ ISO_H_DOOR_000_OPEN ] . filename = "iso_doors_0001.png" ;
    obstacle_map [ ISO_H_DOOR_000_OPEN ] . transparent = TRANSPARENCY_FOR_SEE_THROUGH_OBJECTS ;
    obstacle_map [ ISO_H_DOOR_025_OPEN ] . is_smashable = FALSE ;
    obstacle_map [ ISO_H_DOOR_025_OPEN ] . filename = "iso_doors_0002.png" ;
    obstacle_map [ ISO_H_DOOR_025_OPEN ] . transparent = TRANSPARENCY_FOR_SEE_THROUGH_OBJECTS ;
    obstacle_map [ ISO_H_DOOR_050_OPEN ] . is_smashable = FALSE ;
    obstacle_map [ ISO_H_DOOR_050_OPEN ] . filename = "iso_doors_0003.png" ;
    obstacle_map [ ISO_H_DOOR_050_OPEN ] . transparent = TRANSPARENCY_FOR_SEE_THROUGH_OBJECTS ;
    obstacle_map [ ISO_H_DOOR_075_OPEN ] . is_smashable = FALSE ;
    obstacle_map [ ISO_H_DOOR_075_OPEN ] . filename = "iso_doors_0004.png" ;
    obstacle_map [ ISO_H_DOOR_075_OPEN ] . transparent = TRANSPARENCY_FOR_SEE_THROUGH_OBJECTS ;
    obstacle_map [ ISO_H_DOOR_100_OPEN ] . is_smashable = FALSE ;
    obstacle_map [ ISO_H_DOOR_100_OPEN ] . filename = "iso_doors_0005.png" ;
    obstacle_map [ ISO_H_DOOR_100_OPEN ] . block_area_type = COLLISION_TYPE_NONE ;
    obstacle_map [ ISO_H_DOOR_100_OPEN ] . transparent = TRANSPARENCY_FOR_SEE_THROUGH_OBJECTS ;
    
    //--------------------
    // These are the normal pillars, that appear here and there in the game.
    //
    obstacle_map [ ISO_BLOCK_1 ] . block_area_parm_1 = 1.05 ;
    obstacle_map [ ISO_BLOCK_1 ] . block_area_parm_2 = 1.05 ;
    obstacle_map [ ISO_BLOCK_1 ] . block_vision_too = FALSE ;
    obstacle_map [ ISO_BLOCK_1 ] . obstacle_short_name = "Pillar";
    obstacle_map [ ISO_BLOCK_1 ] . obstacle_long_description = "A pillar made out of gray concrete.";
    obstacle_map [ ISO_BLOCK_2 ] . block_area_parm_1 = 1.05 ;
    obstacle_map [ ISO_BLOCK_2 ] . block_area_parm_2 = 1.05 ;
    obstacle_map [ ISO_BLOCK_2 ] . block_vision_too = FALSE ;
    obstacle_map [ ISO_BLOCK_2 ] . obstacle_short_name = "Pillar";
    obstacle_map [ ISO_BLOCK_2 ] . obstacle_long_description = "A pillar made out of gray concrete.";    

  //--------------------
  // These files originate from iso_machinery.blend
  //
  obstacle_map [ ISO_TV_PILLAR_W ] . block_area_parm_1 = 0.9 ;
  obstacle_map [ ISO_TV_PILLAR_W ] . block_area_parm_2 = 0.9 ;
  obstacle_map [ ISO_TV_PILLAR_W ] . filename = "iso_machinery_0001.png" ;
  obstacle_map [ ISO_TV_PILLAR_W ] . obstacle_short_name = "Supercluster terminal";
  obstacle_map [ ISO_TV_PILLAR_W ] . obstacle_long_description = "This is a small part of a gigantic computer cluster, maybe many square miles large...";
  obstacle_map [ ISO_TV_PILLAR_N ] . block_area_parm_1 = 0.9 ;
  obstacle_map [ ISO_TV_PILLAR_N ] . block_area_parm_2 = 0.9 ;
  obstacle_map [ ISO_TV_PILLAR_N ] . filename = "iso_machinery_0002.png" ;
  obstacle_map [ ISO_TV_PILLAR_N ] . obstacle_short_name = "Supercluster terminal";
  obstacle_map [ ISO_TV_PILLAR_N ] . obstacle_long_description = "This is a small part of a gigantic computer cluster, maybe many square miles large...";
  obstacle_map [ ISO_TV_PILLAR_E ] . block_area_parm_1 = 0.9 ;
  obstacle_map [ ISO_TV_PILLAR_E ] . block_area_parm_2 = 0.9 ;
  obstacle_map [ ISO_TV_PILLAR_E ] . filename = "iso_machinery_0003.png" ;
  obstacle_map [ ISO_TV_PILLAR_E ] . obstacle_short_name = "Supercluster terminal";
  obstacle_map [ ISO_TV_PILLAR_E ] . obstacle_long_description = "This is a small part of a gigantic computer cluster, maybe many square miles large...";
  obstacle_map [ ISO_TV_PILLAR_S ] . block_area_parm_1 = 0.9 ;
  obstacle_map [ ISO_TV_PILLAR_S ] . block_area_parm_2 = 0.9 ;
  obstacle_map [ ISO_TV_PILLAR_S ] . filename = "iso_machinery_0004.png" ;
  obstacle_map [ ISO_TV_PILLAR_S ] . obstacle_short_name = "Supercluster terminal";
  obstacle_map [ ISO_TV_PILLAR_S ] . obstacle_long_description = "This is a small part of a gigantic computer cluster, maybe many square miles large...";
// Magotari: Not sure if you are going to agree with the above. I just wanted an ominous description, something unknown and big, to cause awe and maybe a bit of fear. Those are my reasons. Feel free to revert or change it if you still disagree.

  
  obstacle_map [ ISO_ENHANCER_LD ] . block_area_parm_1 = 1.2 ;
  obstacle_map [ ISO_ENHANCER_LD ] . block_area_parm_2 = 1.2 ;
  obstacle_map [ ISO_ENHANCER_LD ] . filename = "iso_machinery_0005.png" ;
  obstacle_map [ ISO_ENHANCER_LD ] . transparent = TRANSPARENCY_FOR_SEE_THROUGH_OBJECTS ;
  obstacle_map [ ISO_ENHANCER_LD ] . obstacle_short_name = "Ray emitter";
  obstacle_map [ ISO_ENHANCER_LD ] . obstacle_long_description = "It appears to be in \"standby\" mode.";
  obstacle_map [ ISO_ENHANCER_LU ] . block_area_parm_1 = 1.2 ;
  obstacle_map [ ISO_ENHANCER_LU ] . block_area_parm_2 = 1.2 ;
  obstacle_map [ ISO_ENHANCER_LU ] . filename = "iso_machinery_0006.png" ;
  obstacle_map [ ISO_ENHANCER_LU ] . transparent = TRANSPARENCY_FOR_SEE_THROUGH_OBJECTS ;
  obstacle_map [ ISO_ENHANCER_LU ] . obstacle_short_name = "Ray emitter";
  obstacle_map [ ISO_ENHANCER_LU ] . obstacle_long_description = "It appears to be in \"standby\" mode.";
  obstacle_map [ ISO_ENHANCER_RU ] . block_area_parm_1 = 1.2 ;
  obstacle_map [ ISO_ENHANCER_RU ] . block_area_parm_2 = 1.2 ;
  obstacle_map [ ISO_ENHANCER_RU ] . filename = "iso_machinery_0007.png" ;
  obstacle_map [ ISO_ENHANCER_RU ] . transparent = TRANSPARENCY_FOR_SEE_THROUGH_OBJECTS; 
  obstacle_map [ ISO_ENHANCER_RU ] . obstacle_short_name = "Ray emitter";
  obstacle_map [ ISO_ENHANCER_RU ] . obstacle_long_description = "It appears to be in \"standby\" mode.";
  obstacle_map [ ISO_ENHANCER_RD ] . block_area_parm_1 = 1.2 ;
  obstacle_map [ ISO_ENHANCER_RD ] . block_area_parm_2 = 1.2 ;
  obstacle_map [ ISO_ENHANCER_RD ] . filename = "iso_machinery_0008.png" ;
  obstacle_map [ ISO_ENHANCER_RD ] . transparent = TRANSPARENCY_FOR_SEE_THROUGH_OBJECTS ;
  obstacle_map [ ISO_ENHANCER_RD ] . obstacle_short_name = "Ray emitter";
  obstacle_map [ ISO_ENHANCER_RD ] . obstacle_long_description = "It appears to be in \"standby\" mode.";
  
  obstacle_map [ ISO_REFRESH_1 ] . block_area_type = COLLISION_TYPE_NONE ;
  obstacle_map [ ISO_REFRESH_1 ] . is_smashable = FALSE ;
  obstacle_map [ ISO_REFRESH_1 ] . needs_pre_put = TRUE ;
  obstacle_map [ ISO_REFRESH_1 ] . filename = "iso_machinery_0009.png" ;
  obstacle_map [ ISO_REFRESH_1 ] . transparent = TRANSPARENCY_FOR_SEE_THROUGH_OBJECTS ;
  obstacle_map [ ISO_REFRESH_2 ] . block_area_type = COLLISION_TYPE_NONE ;
  obstacle_map [ ISO_REFRESH_2 ] . is_smashable = FALSE ;
  obstacle_map [ ISO_REFRESH_2 ] . needs_pre_put = TRUE ;
  obstacle_map [ ISO_REFRESH_2 ] . filename = "iso_machinery_0010.png" ;
  obstacle_map [ ISO_REFRESH_2 ] . transparent = TRANSPARENCY_FOR_SEE_THROUGH_OBJECTS ;
  obstacle_map [ ISO_REFRESH_3 ] . block_area_type = COLLISION_TYPE_NONE ;
  obstacle_map [ ISO_REFRESH_3 ] . is_smashable = FALSE ;
  obstacle_map [ ISO_REFRESH_3 ] . needs_pre_put = TRUE ;
  obstacle_map [ ISO_REFRESH_3 ] . filename = "iso_machinery_0011.png" ;
  obstacle_map [ ISO_REFRESH_3 ] . transparent = TRANSPARENCY_FOR_SEE_THROUGH_OBJECTS ;
  obstacle_map [ ISO_REFRESH_4 ] . block_area_type = COLLISION_TYPE_NONE ;
  obstacle_map [ ISO_REFRESH_4 ] . is_smashable = FALSE ;
  obstacle_map [ ISO_REFRESH_4 ] . needs_pre_put = TRUE ;
  obstacle_map [ ISO_REFRESH_4 ] . filename = "iso_machinery_0012.png" ;
  obstacle_map [ ISO_REFRESH_4 ] . transparent = TRANSPARENCY_FOR_SEE_THROUGH_OBJECTS ;
  obstacle_map [ ISO_REFRESH_5 ] . block_area_type = COLLISION_TYPE_NONE ;
  obstacle_map [ ISO_REFRESH_5 ] . is_smashable = FALSE ;
  obstacle_map [ ISO_REFRESH_5 ] . needs_pre_put = TRUE ;
  obstacle_map [ ISO_REFRESH_5 ] . filename = "iso_machinery_0013.png" ;
  obstacle_map [ ISO_REFRESH_5 ] . transparent = TRANSPARENCY_FOR_SEE_THROUGH_OBJECTS ;
  obstacle_map [ ISO_REFRESH_5 ] . obstacle_short_name = "Droid nest";
  obstacle_map [ ISO_REFRESH_4 ] . obstacle_short_name = "Droid nest";
  obstacle_map [ ISO_REFRESH_3 ] . obstacle_short_name = "Droid nest";
  obstacle_map [ ISO_REFRESH_2 ] . obstacle_short_name = "Droid nest";
  obstacle_map [ ISO_REFRESH_1 ] . obstacle_short_name = "Droid nest";
  obstacle_map [ ISO_REFRESH_5 ] . obstacle_long_description = "This is a service droid case. You suppose it's of no use for your quest.";
  obstacle_map [ ISO_REFRESH_4 ] . obstacle_long_description = "This is a service droid case. You suppose it's of no use for your quest.";
  obstacle_map [ ISO_REFRESH_3 ] . obstacle_long_description = "This is a service droid case. You suppose it's of no use for your quest.";
  obstacle_map [ ISO_REFRESH_2 ] . obstacle_long_description = "This is a service droid case. You suppose it's of no use for your quest.";
  obstacle_map [ ISO_REFRESH_1 ] . obstacle_long_description = "This is a service droid case. You suppose it's of no use for your quest.";
  
  obstacle_map [ ISO_TELEPORTER_1 ] . block_area_type = COLLISION_TYPE_NONE ;
  obstacle_map [ ISO_TELEPORTER_1 ] . is_smashable = FALSE ;
  obstacle_map [ ISO_TELEPORTER_1 ] . emitted_light_strength = 10 ;
  obstacle_map [ ISO_TELEPORTER_1 ] . transparent = TRANSPARENCY_FOR_SEE_THROUGH_OBJECTS ;
  obstacle_map [ ISO_TELEPORTER_2 ] . block_area_type = COLLISION_TYPE_NONE ;
  obstacle_map [ ISO_TELEPORTER_2 ] . is_smashable = FALSE ;
  obstacle_map [ ISO_TELEPORTER_2 ] . emitted_light_strength = 9 ;
  obstacle_map [ ISO_TELEPORTER_2 ] . transparent = TRANSPARENCY_FOR_SEE_THROUGH_OBJECTS ;
  obstacle_map [ ISO_TELEPORTER_3 ] . block_area_type = COLLISION_TYPE_NONE ;
  obstacle_map [ ISO_TELEPORTER_3 ] . is_smashable = FALSE ;
  obstacle_map [ ISO_TELEPORTER_3 ] . emitted_light_strength = 8 ;
  obstacle_map [ ISO_TELEPORTER_3 ] . transparent = TRANSPARENCY_FOR_SEE_THROUGH_OBJECTS ;
  obstacle_map [ ISO_TELEPORTER_4 ] . block_area_type = COLLISION_TYPE_NONE ;
  obstacle_map [ ISO_TELEPORTER_4 ] . is_smashable = FALSE ;
  obstacle_map [ ISO_TELEPORTER_4 ] . emitted_light_strength = 9 ;
  obstacle_map [ ISO_TELEPORTER_4 ] . transparent = TRANSPARENCY_FOR_SEE_THROUGH_OBJECTS ;
  obstacle_map [ ISO_TELEPORTER_5 ] . block_area_type = COLLISION_TYPE_NONE ;
  obstacle_map [ ISO_TELEPORTER_5 ] . is_smashable = FALSE ;
  obstacle_map [ ISO_TELEPORTER_5 ] . emitted_light_strength = 10 ;
  obstacle_map [ ISO_TELEPORTER_5 ] . transparent = TRANSPARENCY_FOR_SEE_THROUGH_OBJECTS ;

  obstacle_map [ ISO_V_CHEST_OPEN ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_V_CHEST_OPEN ] . block_area_parm_1 = standard_wall_thickness ;
  obstacle_map [ ISO_V_CHEST_OPEN ] . block_area_parm_2 = 0.8 ;
  obstacle_map [ ISO_V_CHEST_OPEN ] . filename = "iso_container_0004.png" ;
  obstacle_map [ ISO_V_CHEST_OPEN ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_V_CHEST_OPEN ] . can_be_looted = TRUE ;  // is this where treasure could be hidden?
  obstacle_map [ ISO_V_CHEST_OPEN ] . obstacle_short_name = "Chest";
  obstacle_map [ ISO_V_CHEST_OPEN ] . obstacle_long_description = "This chest has already been opened";
  

  obstacle_map [ ISO_V_CHEST_CLOSED ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_V_CHEST_CLOSED ] . block_area_parm_1 = standard_wall_thickness ;
  obstacle_map [ ISO_V_CHEST_CLOSED ] . block_area_parm_2 = 0.8 ;
  obstacle_map [ ISO_V_CHEST_CLOSED ] . filename = "iso_container_0002.png" ;
  obstacle_map [ ISO_V_CHEST_CLOSED ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_V_CHEST_CLOSED ] . can_be_looted = TRUE ;  // is this where treasure could be hidden?
  obstacle_map [ ISO_V_CHEST_CLOSED ] . obstacle_short_name = "Chest";
  obstacle_map [ ISO_V_CHEST_CLOSED ] . obstacle_long_description = "This chest is closed.";

  obstacle_map [ ISO_H_CHEST_OPEN ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_H_CHEST_OPEN ] . block_area_parm_1 = 0.8 ;
  obstacle_map [ ISO_H_CHEST_OPEN ] . block_area_parm_2 = standard_wall_thickness ;
  obstacle_map [ ISO_H_CHEST_OPEN ] . filename = "iso_container_0003.png" ;
  obstacle_map [ ISO_H_CHEST_OPEN ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_H_CHEST_OPEN ] . can_be_looted = TRUE ;  // is this where treasure could be hidden?
  obstacle_map [ ISO_H_CHEST_OPEN ] . obstacle_short_name = "Chest";
  obstacle_map [ ISO_H_CHEST_OPEN ] . obstacle_long_description = "This chest has already been opened";

  obstacle_map [ ISO_H_CHEST_CLOSED ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_H_CHEST_CLOSED ] . block_area_parm_1 = 0.8 ;
  obstacle_map [ ISO_H_CHEST_CLOSED ] . block_area_parm_2 = standard_wall_thickness ;
  obstacle_map [ ISO_H_CHEST_CLOSED ] . filename = "iso_container_0001.png" ;
  obstacle_map [ ISO_H_CHEST_CLOSED ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_H_CHEST_CLOSED ] . can_be_looted = TRUE ;  // is this where treasure could be hidden?
  obstacle_map [ ISO_H_CHEST_CLOSED ] . obstacle_short_name = "Chest";
  obstacle_map [ ISO_H_CHEST_CLOSED ] . obstacle_long_description = "This chest is closed.";

  obstacle_map [ ISO_AUTOGUN_N ] . block_area_parm_1 = 0.9 ;
  obstacle_map [ ISO_AUTOGUN_N ] . block_area_parm_2 = 0.9 ;
  obstacle_map [ ISO_AUTOGUN_N ] . filename = "iso_autogun_0002.png" ;
  obstacle_map [ ISO_AUTOGUN_N ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_AUTOGUN_N ] . obstacle_short_name = "Autogun";
  obstacle_map [ ISO_AUTOGUN_N ] . obstacle_long_description = "A not-that-nice autogun. Beware : it could be harmful.";
  obstacle_map [ ISO_AUTOGUN_S ] . block_area_parm_1 = 0.9 ;
  obstacle_map [ ISO_AUTOGUN_S ] . block_area_parm_2 = 0.9 ;
  obstacle_map [ ISO_AUTOGUN_S ] . filename = "iso_autogun_0004.png" ;
  obstacle_map [ ISO_AUTOGUN_S ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_AUTOGUN_S ] . obstacle_short_name = "Autogun";
  obstacle_map [ ISO_AUTOGUN_S ] . obstacle_long_description = "A not-that-nice autogun. Beware : it could be harmful.";
  obstacle_map [ ISO_AUTOGUN_E ] . block_area_parm_1 = 0.9 ;
  obstacle_map [ ISO_AUTOGUN_E ] . block_area_parm_2 = 0.9 ;
  obstacle_map [ ISO_AUTOGUN_E ] . filename = "iso_autogun_0003.png" ;
  obstacle_map [ ISO_AUTOGUN_E ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_AUTOGUN_E ] . obstacle_short_name = "Autogun";
  obstacle_map [ ISO_AUTOGUN_E ] . obstacle_long_description = "A not-that-nice autogun. Beware : it could be harmful.";
  obstacle_map [ ISO_AUTOGUN_W ] . block_area_parm_1 = 0.9 ;
  obstacle_map [ ISO_AUTOGUN_W ] . block_area_parm_2 = 0.9 ;
  obstacle_map [ ISO_AUTOGUN_W ] . filename = "iso_autogun_0001.png" ;
  obstacle_map [ ISO_AUTOGUN_W ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_AUTOGUN_W ] . obstacle_short_name = "Autogun";
  obstacle_map [ ISO_AUTOGUN_W ] . obstacle_long_description = "A not-that-nice autogun. Beware : it could be harmful.";

  obstacle_map [ ISO_CAVE_WALL_H ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_CAVE_WALL_H ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_CAVE_WALL_H ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_CAVE_WALL_H ] . filename = "iso_cave_wall_0001.png" ;
  obstacle_map [ ISO_CAVE_WALL_H ] . obstacle_short_name = "Rock barrier";
  obstacle_map [ ISO_CAVE_WALL_H ] . obstacle_long_description = "No way to pass through. Find another path.";
  obstacle_map [ ISO_CAVE_WALL_V ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_CAVE_WALL_V ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_CAVE_WALL_V ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_CAVE_WALL_V ] . filename = "iso_cave_wall_0002.png" ;
  obstacle_map [ ISO_CAVE_WALL_V ] . obstacle_short_name = "Rock barrier";
  obstacle_map [ ISO_CAVE_WALL_V ] . obstacle_long_description = "No way to pass through. Find another path.";
  obstacle_map [ ISO_CAVE_CORNER_NE ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_CAVE_CORNER_NE ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_CAVE_CORNER_NE ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_CAVE_CORNER_NE ] . filename = "iso_cave_wall_0003.png" ;
  obstacle_map [ ISO_CAVE_CORNER_NE ] . obstacle_short_name = "Rock barrier";
  obstacle_map [ ISO_CAVE_CORNER_NE ] . obstacle_long_description = "No way to pass through. Find another path.";
  obstacle_map [ ISO_CAVE_CORNER_SE ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_CAVE_CORNER_SE ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_CAVE_CORNER_SE ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_CAVE_CORNER_SE ] . filename = "iso_cave_wall_0004.png" ;
  obstacle_map [ ISO_CAVE_CORNER_SE ] . obstacle_short_name = "Rock barrier";
  obstacle_map [ ISO_CAVE_CORNER_SE ] . obstacle_long_description = "No way to pass through. Find another path.";
  obstacle_map [ ISO_CAVE_CORNER_NW ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_CAVE_CORNER_NW ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_CAVE_CORNER_NW ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_CAVE_CORNER_NW ] . filename = "iso_cave_wall_0005.png" ;
  obstacle_map [ ISO_CAVE_CORNER_NW ] . obstacle_short_name = "Rock barrier";
  obstacle_map [ ISO_CAVE_CORNER_NW ] . obstacle_long_description = "No way to pass through. Find another path.";
  obstacle_map [ ISO_CAVE_CORNER_SW ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_CAVE_CORNER_SW ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_CAVE_CORNER_SW ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_CAVE_CORNER_SW ] . filename = "iso_cave_wall_0006.png" ;
  obstacle_map [ ISO_CAVE_CORNER_SW ] . obstacle_short_name = "Rock barrier";
  obstacle_map [ ISO_CAVE_CORNER_SW ] . obstacle_long_description = "No way to pass through. Find another path.";

  obstacle_map [ ISO_COOKING_POT ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_COOKING_POT ] . block_area_parm_1 = 0.5 ;
  obstacle_map [ ISO_COOKING_POT ] . block_area_parm_2 = 0.5 ;
  obstacle_map [ ISO_COOKING_POT ] . is_smashable = TRUE ;
  obstacle_map [ ISO_COOKING_POT ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_COOKING_POT ] . transparent = TRANSPARENCY_FOR_SEE_THROUGH_OBJECTS ;
  obstacle_map [ ISO_COOKING_POT ] . obstacle_short_name = "Cooking pot";
  obstacle_map [ ISO_COOKING_POT ] . obstacle_long_description = "This cauldron can be used for many good things. Cooking beef or pork for dinner are some of them. It can be also used for many evil things. Cooking Linarians for dinner is one of them.";

  obstacle_map [ ISO_CONSOLE_N ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_CONSOLE_N ] . block_area_parm_1 = 0.8 ;
  obstacle_map [ ISO_CONSOLE_N ] . block_area_parm_2 = 0.8 ;
  obstacle_map [ ISO_CONSOLE_N ] . is_smashable = TRUE ;
  obstacle_map [ ISO_CONSOLE_N ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_CONSOLE_N ] . obstacle_short_name = "Terminal";
  obstacle_map [ ISO_CONSOLE_N ] . obstacle_long_description = "This is a Terminus 2 terminal. Made by Parafunken.";

  obstacle_map [ ISO_CONSOLE_S ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_CONSOLE_S ] . block_area_parm_1 = 0.8 ;
  obstacle_map [ ISO_CONSOLE_S ] . block_area_parm_2 = 0.8 ;
  obstacle_map [ ISO_CONSOLE_S ] . is_smashable = TRUE ;
  obstacle_map [ ISO_CONSOLE_S ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_CONSOLE_S ] . obstacle_short_name = "Terminal";
  obstacle_map [ ISO_CONSOLE_S ] . obstacle_long_description = "This is a Terminus 2 terminal. Made by Parafunken.";

  obstacle_map [ ISO_CONSOLE_E ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_CONSOLE_E ] . block_area_parm_1 = 0.8 ;
  obstacle_map [ ISO_CONSOLE_E ] . block_area_parm_2 = 0.8 ;
  obstacle_map [ ISO_CONSOLE_E ] . is_smashable = TRUE ;
  obstacle_map [ ISO_CONSOLE_E ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_CONSOLE_E ] . obstacle_short_name = "Terminal";
  obstacle_map [ ISO_CONSOLE_E ] . obstacle_long_description = "This is a Terminus 2 terminal. Made by Parafunken.";

  obstacle_map [ ISO_CONSOLE_W ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_CONSOLE_W ] . block_area_parm_1 = 0.8 ;
  obstacle_map [ ISO_CONSOLE_W ] . block_area_parm_2 = 0.8 ;
  obstacle_map [ ISO_CONSOLE_W ] . is_smashable = TRUE ;
  obstacle_map [ ISO_CONSOLE_W ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_CONSOLE_W ] . obstacle_short_name = "Terminal";
  obstacle_map [ ISO_CONSOLE_W ] . obstacle_long_description = "This is a Terminus 2 terminal. Made by Parafunken.";

  obstacle_map [ ISO_BARREL_1 ] . drop_random_treasure = TRUE ;
  obstacle_map [ ISO_BARREL_1 ] . is_smashable = TRUE ;
  obstacle_map [ ISO_BARREL_1 ] . obstacle_short_name = "Barrel";
  obstacle_map [ ISO_BARREL_1 ] . obstacle_long_description = "A painted barrel, protected from rust.";
  obstacle_map [ ISO_BARREL_2 ] . drop_random_treasure = TRUE ;
  obstacle_map [ ISO_BARREL_2 ] . is_smashable = TRUE ;
  obstacle_map [ ISO_BARREL_2 ] . obstacle_short_name = "Barrel";
  obstacle_map [ ISO_BARREL_2 ] . obstacle_long_description = "A rusty barrel.";

  //wood crates
  obstacle_map [ ISO_BARREL_3 ] . drop_random_treasure = TRUE ;
  obstacle_map [ ISO_BARREL_3 ] . is_smashable = TRUE ;
  obstacle_map [ ISO_BARREL_3 ] . obstacle_short_name = "Wooden crate";
  obstacle_map [ ISO_BARREL_3 ] . obstacle_long_description = "This is a wooden crate that could contain interesting things. Or not.";
  obstacle_map [ ISO_BARREL_4 ] . drop_random_treasure = TRUE ;
  obstacle_map [ ISO_BARREL_4 ] . is_smashable = TRUE ;
  obstacle_map [ ISO_BARREL_4 ] . obstacle_short_name = "Wooden crate";
  obstacle_map [ ISO_BARREL_4 ] . obstacle_long_description = "This is a wooden crate that could contain interesting things. Or not.";

  //--------------------
  // This is the wonderful new room lamp from basse
  // (Vision can pass through.  Later, light might radiate from it.)
  //
  obstacle_map [ ISO_LAMP_N ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_LAMP_N ] . block_area_parm_1 = 0.5 ;
  obstacle_map [ ISO_LAMP_N ] . block_area_parm_2 = 0.5 ;
  obstacle_map [ ISO_LAMP_N ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_LAMP_N ] . emitted_light_strength = 5 ; // how much light emitted from here...
  obstacle_map [ ISO_LAMP_N ] . filename = "iso_obstacle_0055.png" ;
  obstacle_map [ ISO_LAMP_N ] . obstacle_short_name = "Room lamp" ;
  obstacle_map [ ISO_LAMP_N ] . obstacle_long_description = "This is a charming room lamp.";
  
  obstacle_map [ ISO_LAMP_E ] . block_area_parm_1 = 0.5 ;
  obstacle_map [ ISO_LAMP_E ] . block_area_parm_2 = 0.5 ;
  obstacle_map [ ISO_LAMP_E ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_LAMP_E ] . emitted_light_strength = 5 ;
  obstacle_map [ ISO_LAMP_E ] . filename = "iso_obstacle_0056.png" ;
  obstacle_map [ ISO_LAMP_E ] . obstacle_short_name = "Room lamp" ;
  obstacle_map [ ISO_LAMP_E ] . obstacle_long_description = "This is a wonderful room lamp.";
  
  obstacle_map [ ISO_LAMP_S ] . block_area_parm_1 = 0.5 ;
  obstacle_map [ ISO_LAMP_S ] . block_area_parm_2 = 0.5 ;
  obstacle_map [ ISO_LAMP_S ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_LAMP_S ] . emitted_light_strength = 5 ;
  obstacle_map [ ISO_LAMP_S ] . filename = "iso_obstacle_0054.png" ;
  obstacle_map [ ISO_LAMP_S ] . obstacle_short_name = "Room lamp" ;
  obstacle_map [ ISO_LAMP_S ] . obstacle_long_description = "This is a great room lamp.";
  
  obstacle_map [ ISO_LAMP_W ] . block_area_parm_1 = 0.5 ;
  obstacle_map [ ISO_LAMP_W ] . block_area_parm_2 = 0.5 ;
  obstacle_map [ ISO_LAMP_W ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_LAMP_W ] . emitted_light_strength = 5 ;
  obstacle_map [ ISO_LAMP_W ] . filename = "iso_obstacle_0057.png" ;
  obstacle_map [ ISO_LAMP_W ] . obstacle_short_name = "Room lamp" ;
  obstacle_map [ ISO_LAMP_W ] . obstacle_long_description = "This is a perfect room lamp.";
  
  //--------------------
  // We have several types of fences.  These are typically rather
  // long and slender obstacles, which is a case that our method
  // of planting obstacles so that the visibility properties are
  // more or less correct can not so easily handle.  A feasible
  // solution is to increase the thickness of the long and slender
  // obstacles, setting e.g. the thickness to 1.1 should provide
  // some protection against small errors in the visibility on screen.
  // Maybe we will need even a bit more.  It's a fine-tuning thing.
  //
  obstacle_map [ ISO_V_WOOD_FENCE ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_V_WOOD_FENCE ] . block_area_parm_1 = 1.1 ;
  obstacle_map [ ISO_V_WOOD_FENCE ] . block_area_parm_2 = 2.2 ;
  obstacle_map [ ISO_V_WOOD_FENCE ] . obstacle_short_name = "Fence";
  obstacle_map [ ISO_V_WOOD_FENCE ] . obstacle_long_description = "This is a wooden fence. Find another path.";
  obstacle_map [ ISO_V_WOOD_FENCE ] . block_area_parm_2 = 2.2 ;
  obstacle_map [ ISO_V_DENSE_FENCE ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_V_DENSE_FENCE ] . block_area_parm_1 = 1.1 ;
  obstacle_map [ ISO_V_DENSE_FENCE ] . block_area_parm_2 = 2.2 ;
//  obstacle_map [ ISO_V_DENSE_FENCE ] . obstacle_short_name = "Fence";
//  obstacle_map [ ISO_V_DENSE_FENCE ] . obstacle_long_description = "To be filled once object appears ingame.";
  obstacle_map [ ISO_V_MESH_FENCE ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_V_MESH_FENCE ] . block_area_parm_1 = 1.1 ;
  obstacle_map [ ISO_V_MESH_FENCE ] . block_area_parm_2 = 2.2 ;
  obstacle_map [ ISO_V_MESH_FENCE ] . block_vision_too = FALSE ;
//  obstacle_map [ ISO_V_DENSE_FENCE ] . obstacle_short_name = "Fence";
//  obstacle_map [ ISO_V_DENSE_FENCE ] . obstacle_long_description = "To be filled once object appears ingame.";
  obstacle_map [ ISO_V_WIRE_FENCE ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_V_WIRE_FENCE ] . block_area_parm_1 = 1.1 ;
  obstacle_map [ ISO_V_WIRE_FENCE ] . block_area_parm_2 = 2.2 ;
  obstacle_map [ ISO_V_WIRE_FENCE ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_V_WIRE_FENCE ] . obstacle_short_name = "Fence";
  obstacle_map [ ISO_V_WIRE_FENCE ] . obstacle_long_description = "This type of a barrier constructed out of wire is called the chain link fence.";
  obstacle_map [ ISO_H_WOOD_FENCE ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_H_WOOD_FENCE ] . block_area_parm_1 = 2.2 ;
  obstacle_map [ ISO_H_WOOD_FENCE ] . block_area_parm_2 = 1.1 ;
  obstacle_map [ ISO_H_WOOD_FENCE ] . obstacle_short_name = "Fence";
  obstacle_map [ ISO_H_WOOD_FENCE ] . obstacle_long_description = "You can't break it. Find another way.";
  obstacle_map [ ISO_H_DENSE_FENCE ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_H_DENSE_FENCE ] . block_area_parm_1 = 2.2 ;
  obstacle_map [ ISO_H_DENSE_FENCE ] . block_area_parm_2 = 1.10 ;
//  obstacle_map [ ISO_H_DENSE_FENCE ] . obstacle_short_name = "Fence";
//  obstacle_map [ ISO_H_DENSE_FENCE ] . obstacle_long_description = "To be filled once object appears ingame.";
  obstacle_map [ ISO_H_MESH_FENCE ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_H_MESH_FENCE ] . block_area_parm_1 = 2.2 ;
  obstacle_map [ ISO_H_MESH_FENCE ] . block_area_parm_2 = 1.10 ;
  obstacle_map [ ISO_H_MESH_FENCE ] . block_vision_too = FALSE ;
//  obstacle_map [ ISO_H_MESH_FENCE ] . obstacle_short_name = "Fence";
//  obstacle_map [ ISO_H_MESH_FENCE ] . obstacle_long_description = "To be filled once object appears ingame.";
  obstacle_map [ ISO_H_WIRE_FENCE ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_H_WIRE_FENCE ] . block_area_parm_1 = 2.2 ;
  obstacle_map [ ISO_H_WIRE_FENCE ] . block_area_parm_2 = 1.10 ;
  obstacle_map [ ISO_H_WIRE_FENCE ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_H_WIRE_FENCE ] . obstacle_short_name = "Fence";
  obstacle_map [ ISO_H_WIRE_FENCE ] . obstacle_long_description = "This type of a barrier constructed out of wire is called the chain link fence.";

  obstacle_map [ ISO_N_TOILET_SMALL ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_N_TOILET_SMALL ] . block_area_parm_1 = 0.4 ;
  obstacle_map [ ISO_N_TOILET_SMALL ] . block_area_parm_2 = 0.4 ;
  obstacle_map [ ISO_N_TOILET_SMALL ] . is_smashable = TRUE ;
  obstacle_map [ ISO_N_TOILET_SMALL ] . filename = "iso_bathroom_furniture_0008.png";
  obstacle_map [ ISO_N_TOILET_SMALL ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_N_TOILET_SMALL ] . obstacle_short_name = "Small toilet";
  obstacle_map [ ISO_N_TOILET_SMALL ] . obstacle_long_description = "Smells good.";
  
  obstacle_map [ ISO_E_TOILET_SMALL ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_E_TOILET_SMALL ] . block_area_parm_1 = 0.4 ;
  obstacle_map [ ISO_E_TOILET_SMALL ] . block_area_parm_2 = 0.4 ;
  obstacle_map [ ISO_E_TOILET_SMALL ] . is_smashable = TRUE ;
  obstacle_map [ ISO_E_TOILET_SMALL ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_E_TOILET_SMALL ] . filename = "iso_bathroom_furniture_0009.png";
  obstacle_map [ ISO_E_TOILET_SMALL ] . obstacle_short_name = "Small toilet";
  obstacle_map [ ISO_E_TOILET_SMALL ] . obstacle_long_description = "Smells good.";
  obstacle_map [ ISO_S_TOILET_SMALL ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_S_TOILET_SMALL ] . block_area_parm_1 = 0.4 ;
  obstacle_map [ ISO_S_TOILET_SMALL ] . block_area_parm_2 = 0.4 ;
  obstacle_map [ ISO_S_TOILET_SMALL ] . is_smashable = TRUE ;
  obstacle_map [ ISO_S_TOILET_SMALL ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_S_TOILET_SMALL ] . obstacle_short_name = "Small toilet";
  obstacle_map [ ISO_S_TOILET_SMALL ] . obstacle_long_description = "Smells good.";
  obstacle_map [ ISO_W_TOILET_SMALL ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_W_TOILET_SMALL ] . block_area_parm_1 = 0.4 ;
  obstacle_map [ ISO_W_TOILET_SMALL ] . block_area_parm_2 = 0.4 ;
  obstacle_map [ ISO_W_TOILET_SMALL ] . is_smashable = TRUE ;
  obstacle_map [ ISO_W_TOILET_SMALL ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_W_TOILET_SMALL ] . obstacle_short_name = "Small toilet";
  obstacle_map [ ISO_W_TOILET_SMALL ] . obstacle_long_description = "Smells good.";
  
  obstacle_map [ ISO_N_TOILET_BIG ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_N_TOILET_BIG ] . block_area_parm_1 = 0.4 ;
  obstacle_map [ ISO_N_TOILET_BIG ] . block_area_parm_2 = 0.4 ;
  obstacle_map [ ISO_N_TOILET_BIG ] . is_smashable = TRUE ;
  obstacle_map [ ISO_N_TOILET_BIG ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_N_TOILET_BIG ] . filename = "iso_bathroom_furniture_0004.png";
  obstacle_map [ ISO_E_TOILET_BIG ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_E_TOILET_BIG ] . block_area_parm_1 = 0.4 ;
  obstacle_map [ ISO_E_TOILET_BIG ] . block_area_parm_2 = 0.4 ;
  obstacle_map [ ISO_E_TOILET_BIG ] . is_smashable = TRUE ;
  obstacle_map [ ISO_E_TOILET_BIG ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_E_TOILET_BIG ] . filename = "iso_bathroom_furniture_0005.png";
  obstacle_map [ ISO_S_TOILET_BIG ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_S_TOILET_BIG ] . block_area_parm_1 = 0.4 ;
  obstacle_map [ ISO_S_TOILET_BIG ] . block_area_parm_2 = 0.4 ;
  obstacle_map [ ISO_S_TOILET_BIG ] . is_smashable = TRUE ;
  obstacle_map [ ISO_S_TOILET_BIG ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_S_TOILET_BIG ] . filename = "iso_bathroom_furniture_0006.png";
  obstacle_map [ ISO_W_TOILET_BIG ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_W_TOILET_BIG ] . block_area_parm_1 = 0.4 ;
  obstacle_map [ ISO_W_TOILET_BIG ] . block_area_parm_2 = 0.4 ;
  obstacle_map [ ISO_W_TOILET_BIG ] . is_smashable = TRUE ;
  obstacle_map [ ISO_W_TOILET_BIG ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_W_TOILET_BIG ] . filename = "iso_bathroom_furniture_0007.png";
  obstacle_map [ ISO_W_TOILET_BIG] . obstacle_short_name = "Toilet";
  obstacle_map [ ISO_W_TOILET_BIG] . obstacle_long_description = "Feel like defecating ?";
  obstacle_map [ ISO_S_TOILET_BIG] . obstacle_short_name = "Toilet";
  obstacle_map [ ISO_S_TOILET_BIG] . obstacle_long_description = "Feel like defecating ?";
  obstacle_map [ ISO_E_TOILET_BIG] . obstacle_short_name = "Toilet";
  obstacle_map [ ISO_E_TOILET_BIG] . obstacle_long_description = "Yes, they are clean.";
  obstacle_map [ ISO_N_TOILET_BIG] . obstacle_short_name = "Toilet";
  obstacle_map [ ISO_N_TOILET_BIG] . obstacle_long_description = "Would you have something to empty ?";

  obstacle_map [ ISO_N_CHAIR ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_N_CHAIR ] . block_area_parm_1 = 0.4 ;
  obstacle_map [ ISO_N_CHAIR ] . block_area_parm_2 = 0.4 ;
  obstacle_map [ ISO_N_CHAIR ] . is_smashable = TRUE ;
  obstacle_map [ ISO_N_CHAIR ] . filename = "iso_chairs_0009.png";
  obstacle_map [ ISO_N_CHAIR ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_N_CHAIR ] . obstacle_short_name = "Chair" ;
  obstacle_map [ ISO_N_CHAIR ] . obstacle_long_description = "This is an orange chair which does not look too comfortable" ;
  obstacle_map [ ISO_E_CHAIR ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_E_CHAIR ] . block_area_parm_1 = 0.4 ;
  obstacle_map [ ISO_E_CHAIR ] . block_area_parm_2 = 0.4 ;
  obstacle_map [ ISO_E_CHAIR ] . is_smashable = TRUE ;
  obstacle_map [ ISO_E_CHAIR ] . filename = "iso_chairs_0010.png";
  obstacle_map [ ISO_E_CHAIR ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_E_CHAIR ] . obstacle_short_name = "Chair" ;
  obstacle_map [ ISO_E_CHAIR ] . obstacle_long_description = "This is an orange chair which does not look too comfortable" ;
  obstacle_map [ ISO_S_CHAIR ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_S_CHAIR ] . block_area_parm_1 = 0.4 ;
  obstacle_map [ ISO_S_CHAIR ] . block_area_parm_2 = 0.4 ;
  obstacle_map [ ISO_S_CHAIR ] . is_smashable = TRUE ;
  obstacle_map [ ISO_S_CHAIR ] . filename = "iso_chairs_0011.png";
  obstacle_map [ ISO_S_CHAIR ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_S_CHAIR ] . obstacle_short_name = "Chair" ;
  obstacle_map [ ISO_S_CHAIR ] . obstacle_long_description = "This is an orange chair which does not look too comfortable" ;
  obstacle_map [ ISO_W_CHAIR ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_W_CHAIR ] . block_area_parm_1 = 0.4 ;
  obstacle_map [ ISO_W_CHAIR ] . block_area_parm_2 = 0.4 ;
  obstacle_map [ ISO_W_CHAIR ] . is_smashable = TRUE ;
  obstacle_map [ ISO_W_CHAIR ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_W_CHAIR ] . filename = "iso_chairs_0012.png";
  obstacle_map [ ISO_W_CHAIR ] . obstacle_short_name = "Chair" ;
  obstacle_map [ ISO_W_CHAIR ] . obstacle_long_description = "This is an orange chair which does not look too comfortable" ;
  
  obstacle_map [ ISO_SOFFA_1 ] . block_area_type = COLLISION_TYPE_RECTANGLE;
  obstacle_map [ ISO_SOFFA_1 ] . block_area_parm_1 = 1.0;
  obstacle_map [ ISO_SOFFA_1 ] . block_area_parm_2 = 1.0;
  obstacle_map [ ISO_SOFFA_1 ] . block_vision_too = FALSE;
  obstacle_map [ ISO_SOFFA_1 ] . filename = "iso_chairs_0013.png";
  obstacle_map [ ISO_SOFFA_1 ] . obstacle_short_name = "Sofa" ;
  obstacle_map [ ISO_SOFFA_1 ] . obstacle_long_description = "A soft red sofa." ;
  
  obstacle_map [ ISO_SOFFA_2 ] . block_area_type = COLLISION_TYPE_RECTANGLE;
  obstacle_map [ ISO_SOFFA_2 ] . block_area_parm_1 = 1.0;
  obstacle_map [ ISO_SOFFA_2 ] . block_area_parm_2 = 1.0;
  obstacle_map [ ISO_SOFFA_2 ] . block_vision_too = FALSE;
  obstacle_map [ ISO_SOFFA_2 ] . filename = "iso_chairs_0014.png";
  obstacle_map [ ISO_SOFFA_2 ] . obstacle_short_name = "Sofa" ;
  obstacle_map [ ISO_SOFFA_2 ] . obstacle_long_description = "A soft red sofa." ;
  
  obstacle_map [ ISO_SOFFA_3 ] . block_area_type = COLLISION_TYPE_RECTANGLE;
  obstacle_map [ ISO_SOFFA_3 ] . block_area_parm_1 = 1.0;
  obstacle_map [ ISO_SOFFA_3 ] . block_area_parm_2 = 1.0;
  obstacle_map [ ISO_SOFFA_3 ] . block_vision_too = FALSE;
  obstacle_map [ ISO_SOFFA_3 ] . filename = "iso_chairs_0015.png";
  obstacle_map [ ISO_SOFFA_3 ] . obstacle_short_name = "Sofa" ;
  obstacle_map [ ISO_SOFFA_3 ] . obstacle_long_description = "A soft red sofa." ;
  
  obstacle_map [ ISO_SOFFA_4 ] . block_area_type = COLLISION_TYPE_RECTANGLE;
  obstacle_map [ ISO_SOFFA_4 ] . block_area_parm_1 = 1.0;
  obstacle_map [ ISO_SOFFA_4 ] . block_area_parm_2 = 1.0;
  obstacle_map [ ISO_SOFFA_4 ] . block_vision_too = FALSE;
  obstacle_map [ ISO_SOFFA_4 ] . filename = "iso_chairs_0016.png";
  obstacle_map [ ISO_SOFFA_4 ] . obstacle_short_name = "Sofa" ;
  obstacle_map [ ISO_SOFFA_4 ] . obstacle_long_description = "A soft red sofa." ;
  
  obstacle_map [ ISO_SOFFA_CORNER_1 ] . block_area_type = COLLISION_TYPE_RECTANGLE;
  obstacle_map [ ISO_SOFFA_CORNER_1 ] . block_area_parm_1 = 1.0;
  obstacle_map [ ISO_SOFFA_CORNER_1 ] . block_area_parm_2 = 1.0;
  obstacle_map [ ISO_SOFFA_CORNER_1 ] . block_vision_too = FALSE;
  obstacle_map [ ISO_SOFFA_CORNER_1 ] . filename = "iso_chairs_0017.png";
  
  obstacle_map [ ISO_SOFFA_CORNER_2 ] . block_area_type = COLLISION_TYPE_RECTANGLE;
  obstacle_map [ ISO_SOFFA_CORNER_2 ] . block_area_parm_1 = 1.0;
  obstacle_map [ ISO_SOFFA_CORNER_2 ] . block_area_parm_2 = 1.0;
  obstacle_map [ ISO_SOFFA_CORNER_2 ] . block_vision_too = FALSE;
  obstacle_map [ ISO_SOFFA_CORNER_2 ] . filename = "iso_chairs_0018.png";

  obstacle_map [ ISO_SOFFA_CORNER_3 ] . block_area_type = COLLISION_TYPE_RECTANGLE;
  obstacle_map [ ISO_SOFFA_CORNER_3 ] . block_area_parm_1 = 1.0;
  obstacle_map [ ISO_SOFFA_CORNER_3 ] . block_area_parm_2 = 1.0;
  obstacle_map [ ISO_SOFFA_CORNER_3 ] . block_vision_too = FALSE;
  obstacle_map [ ISO_SOFFA_CORNER_3 ] . filename = "iso_chairs_0019.png";
  
  obstacle_map [ ISO_SOFFA_CORNER_4 ] . block_area_type = COLLISION_TYPE_RECTANGLE;
  obstacle_map [ ISO_SOFFA_CORNER_4 ] . block_area_parm_1 = 1.0;
  obstacle_map [ ISO_SOFFA_CORNER_4 ] . block_area_parm_2 = 1.0;
  obstacle_map [ ISO_SOFFA_CORNER_4 ] . block_vision_too = FALSE;
  obstacle_map [ ISO_SOFFA_CORNER_4 ] . filename = "iso_chairs_0020.png";

  
  obstacle_map [ ISO_SOFFA_CORNER_PLANT_1 ] . block_area_type = COLLISION_TYPE_RECTANGLE;
  obstacle_map [ ISO_SOFFA_CORNER_PLANT_1 ] . block_area_parm_1 = 1.0;
  obstacle_map [ ISO_SOFFA_CORNER_PLANT_1 ] . block_area_parm_2 = 1.0;
  obstacle_map [ ISO_SOFFA_CORNER_PLANT_1 ] . block_vision_too = FALSE;
  obstacle_map [ ISO_SOFFA_CORNER_PLANT_1 ] . filename = "iso_chairs_0021.png";
  
  obstacle_map [ ISO_SOFFA_CORNER_PLANT_2 ] . block_area_type = COLLISION_TYPE_RECTANGLE;
  obstacle_map [ ISO_SOFFA_CORNER_PLANT_2 ] . block_area_parm_1 = 1.0;
  obstacle_map [ ISO_SOFFA_CORNER_PLANT_2 ] . block_area_parm_2 = 1.0;
  obstacle_map [ ISO_SOFFA_CORNER_PLANT_2 ] . block_vision_too = FALSE;
  obstacle_map [ ISO_SOFFA_CORNER_PLANT_2 ] . filename = "iso_chairs_0022.png";

  obstacle_map [ ISO_SOFFA_CORNER_PLANT_3 ] . block_area_type = COLLISION_TYPE_RECTANGLE;
  obstacle_map [ ISO_SOFFA_CORNER_PLANT_3 ] . block_area_parm_1 = 1.0;
  obstacle_map [ ISO_SOFFA_CORNER_PLANT_3 ] . block_area_parm_2 = 1.0;
  obstacle_map [ ISO_SOFFA_CORNER_PLANT_3 ] . block_vision_too = FALSE;
  obstacle_map [ ISO_SOFFA_CORNER_PLANT_3 ] . filename = "iso_chairs_0023.png";
  
  obstacle_map [ ISO_SOFFA_CORNER_PLANT_4 ] . block_area_type = COLLISION_TYPE_RECTANGLE;
  obstacle_map [ ISO_SOFFA_CORNER_PLANT_4 ] . block_area_parm_1 = 1.0;
  obstacle_map [ ISO_SOFFA_CORNER_PLANT_4 ] . block_area_parm_2 = 1.0;
  obstacle_map [ ISO_SOFFA_CORNER_PLANT_4 ] . block_vision_too = FALSE;
  obstacle_map [ ISO_SOFFA_CORNER_PLANT_4 ] . filename = "iso_chairs_0024.png";

  obstacle_map [ ISO_N_DESK ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_N_DESK ] . block_area_parm_1 = 0.4 ;
  obstacle_map [ ISO_N_DESK ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_N_DESK ] . is_smashable = TRUE ;
  obstacle_map [ ISO_N_DESK ] . filename = "iso_tables_0001.png";
  obstacle_map [ ISO_N_DESK ] . can_be_looted = TRUE ;  // is this where treasure could be hidden?
  obstacle_map [ ISO_N_DESK ] . obstacle_short_name = "Desk";
  obstacle_map [ ISO_N_DESK ] . obstacle_long_description = "This desk is designed to handle extreme loads of office work.";
  obstacle_map [ ISO_E_DESK ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_E_DESK ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_E_DESK ] . block_area_parm_2 = 0.4 ;
  obstacle_map [ ISO_E_DESK ] . is_smashable = TRUE ;
  obstacle_map [ ISO_E_DESK ] . filename = "iso_tables_0002.png" ;
  obstacle_map [ ISO_E_DESK ] . can_be_looted = TRUE ;  // is this where treasure could be hidden?
  obstacle_map [ ISO_E_DESK ] . obstacle_short_name = "Desk";
  obstacle_map [ ISO_E_DESK ] . obstacle_long_description = "This desk is designed to handle extreme loads of office work.";
  obstacle_map [ ISO_S_DESK ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_S_DESK ] . block_area_parm_1 = 0.4 ;
  obstacle_map [ ISO_S_DESK ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_S_DESK ] . is_smashable = TRUE ;
  obstacle_map [ ISO_S_DESK ] . filename = "iso_tables_0003.png";
  obstacle_map [ ISO_S_DESK ] . can_be_looted = TRUE ;  // is this where treasure could be hidden?
  obstacle_map [ ISO_S_DESK ] . obstacle_short_name = "Desk";
  obstacle_map [ ISO_S_DESK ] . obstacle_long_description = "This desk is designed to handle extreme loads of office work.";
  obstacle_map [ ISO_W_DESK ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_W_DESK ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_W_DESK ] . block_area_parm_2 = 0.4 ;
  obstacle_map [ ISO_W_DESK ] . is_smashable = TRUE ;
  obstacle_map [ ISO_W_DESK ] . filename = "iso_tables_0004.png";
  obstacle_map [ ISO_W_DESK ] . can_be_looted = TRUE ;  // is this where treasure could be hidden?
  obstacle_map [ ISO_W_DESK ] . obstacle_short_name = "Desk";
  obstacle_map [ ISO_W_DESK ] . obstacle_long_description = "This desk is designed to handle extreme loads of office work.";

  obstacle_map [ ISO_N_SCHOOL_CHAIR ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_N_SCHOOL_CHAIR ] . block_area_parm_1 = 0.4 ;
  obstacle_map [ ISO_N_SCHOOL_CHAIR ] . block_area_parm_2 = 0.4 ;
  obstacle_map [ ISO_N_SCHOOL_CHAIR ] . is_smashable = TRUE ;
  obstacle_map [ ISO_N_SCHOOL_CHAIR ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_E_SCHOOL_CHAIR ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_E_SCHOOL_CHAIR ] . block_area_parm_1 = 0.4 ;
  obstacle_map [ ISO_E_SCHOOL_CHAIR ] . block_area_parm_2 = 0.4 ;
  obstacle_map [ ISO_E_SCHOOL_CHAIR ] . is_smashable = TRUE ;
  obstacle_map [ ISO_E_SCHOOL_CHAIR ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_S_SCHOOL_CHAIR ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_S_SCHOOL_CHAIR ] . block_area_parm_1 = 0.4 ;
  obstacle_map [ ISO_S_SCHOOL_CHAIR ] . block_area_parm_2 = 0.4 ;
  obstacle_map [ ISO_S_SCHOOL_CHAIR ] . is_smashable = TRUE ;
  obstacle_map [ ISO_S_SCHOOL_CHAIR ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_W_SCHOOL_CHAIR ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_W_SCHOOL_CHAIR ] . block_area_parm_1 = 0.4 ;
  obstacle_map [ ISO_W_SCHOOL_CHAIR ] . block_area_parm_2 = 0.4 ;
  obstacle_map [ ISO_W_SCHOOL_CHAIR ] . is_smashable = TRUE ;
  obstacle_map [ ISO_W_SCHOOL_CHAIR ] . block_vision_too = FALSE ;

  obstacle_map [ ISO_N_BED ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_N_BED ] . block_area_parm_1 = 1.1 ;
  obstacle_map [ ISO_N_BED ] . block_area_parm_2 = 0.7 ;
  obstacle_map [ ISO_N_BED ] . is_smashable = TRUE ;
  obstacle_map [ ISO_N_BED ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_E_BED ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_E_BED ] . block_area_parm_1 = 0.7 ;
  obstacle_map [ ISO_E_BED ] . block_area_parm_2 = 1.1 ;
  obstacle_map [ ISO_E_BED ] . is_smashable = TRUE ;
  obstacle_map [ ISO_E_BED ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_S_BED ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_S_BED ] . block_area_parm_1 = 1.1 ;
  obstacle_map [ ISO_S_BED ] . block_area_parm_2 = 0.7 ;
  obstacle_map [ ISO_S_BED ] . is_smashable = TRUE ;
  obstacle_map [ ISO_S_BED ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_W_BED ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_W_BED ] . block_area_parm_1 = 0.7 ;
  obstacle_map [ ISO_W_BED ] . block_area_parm_2 = 1.1 ;
  obstacle_map [ ISO_W_BED ] . is_smashable = TRUE ;
  obstacle_map [ ISO_W_BED ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_N_FULL_PARK_BENCH ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_N_FULL_PARK_BENCH ] . block_area_parm_1 = 0.5 ;
  obstacle_map [ ISO_N_FULL_PARK_BENCH ] . block_area_parm_2 = 1.3 ;
  obstacle_map [ ISO_N_FULL_PARK_BENCH ] . is_smashable = TRUE ;
  obstacle_map [ ISO_N_FULL_PARK_BENCH ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_N_FULL_PARK_BENCH ] . obstacle_short_name = "Bench" ;
  obstacle_map [ ISO_N_FULL_PARK_BENCH ] . obstacle_long_description = "A simple white bench." ;
  obstacle_map [ ISO_E_FULL_PARK_BENCH ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_E_FULL_PARK_BENCH ] . block_area_parm_1 = 1.3 ;
  obstacle_map [ ISO_E_FULL_PARK_BENCH ] . block_area_parm_2 = 0.5 ;
  obstacle_map [ ISO_E_FULL_PARK_BENCH ] . is_smashable = TRUE ;
  obstacle_map [ ISO_E_FULL_PARK_BENCH ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_E_FULL_PARK_BENCH ] . obstacle_short_name = "Bench" ;
  obstacle_map [ ISO_E_FULL_PARK_BENCH ] . obstacle_long_description = "A simple white bench." ;
  obstacle_map [ ISO_S_FULL_PARK_BENCH ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_S_FULL_PARK_BENCH ] . block_area_parm_1 = 0.5 ;
  obstacle_map [ ISO_S_FULL_PARK_BENCH ] . block_area_parm_2 = 1.3 ;
  obstacle_map [ ISO_S_FULL_PARK_BENCH ] . is_smashable = TRUE ;
  obstacle_map [ ISO_S_FULL_PARK_BENCH ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_S_FULL_PARK_BENCH ] . obstacle_short_name = "Bench" ;
  obstacle_map [ ISO_S_FULL_PARK_BENCH ] . obstacle_long_description = "A simple white bench." ;
  obstacle_map [ ISO_W_FULL_PARK_BENCH ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_W_FULL_PARK_BENCH ] . block_area_parm_1 = 1.3 ;
  obstacle_map [ ISO_W_FULL_PARK_BENCH ] . block_area_parm_2 = 0.5 ;
  obstacle_map [ ISO_W_FULL_PARK_BENCH ] . is_smashable = TRUE ;
  obstacle_map [ ISO_W_FULL_PARK_BENCH ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_W_FULL_PARK_BENCH ] . obstacle_short_name = "Bench" ;
  obstacle_map [ ISO_W_FULL_PARK_BENCH ] . obstacle_long_description = "A simple white bench." ;

  obstacle_map [ ISO_H_BATHTUB ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_H_BATHTUB ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_H_BATHTUB ] . block_area_parm_2 = 0.5 ;
  obstacle_map [ ISO_H_BATHTUB ] . is_smashable = TRUE ;
  obstacle_map [ ISO_H_BATHTUB ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_H_BATHTUB ] . filename = "iso_bathroom_furniture_0000.png" ;
  obstacle_map [ ISO_H_BATHTUB ] . obstacle_short_name = "Bath tub";
  obstacle_map [ ISO_H_BATHTUB ] . obstacle_long_description = "An idea of what comfort is";
  obstacle_map [ ISO_V_BATHTUB ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_V_BATHTUB ] . block_area_parm_1 = 0.5 ;
  obstacle_map [ ISO_V_BATHTUB ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_V_BATHTUB ] . is_smashable = TRUE ;
  obstacle_map [ ISO_V_BATHTUB ] . filename = "iso_bathroom_furniture_0001.png" ;
  obstacle_map [ ISO_V_BATHTUB ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_V_BATHTUB ] . obstacle_short_name = "Bath tub";
  obstacle_map [ ISO_V_BATHTUB ] . obstacle_long_description = "Taking a bath is not your current quest - what a pity !";
  
  obstacle_map [ ISO_3_BATHTUB ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_3_BATHTUB ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_3_BATHTUB ] . block_area_parm_2 = 0.5 ;
  obstacle_map [ ISO_3_BATHTUB ] . is_smashable = TRUE ;
  obstacle_map [ ISO_3_BATHTUB ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_3_BATHTUB ] . filename = "iso_bathroom_furniture_0002.png" ;
  obstacle_map [ ISO_3_BATHTUB ] . obstacle_short_name = "Bath tub";
  obstacle_map [ ISO_3_BATHTUB ] . obstacle_long_description = "Taking a bath is not your current quest - what a pity !";
  
  obstacle_map [ ISO_4_BATHTUB ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_4_BATHTUB ] . block_area_parm_1 = 0.5 ;
  obstacle_map [ ISO_4_BATHTUB ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_4_BATHTUB ] . is_smashable = TRUE ;
  obstacle_map [ ISO_4_BATHTUB ] . filename = "iso_bathroom_furniture_0003.png" ;
  obstacle_map [ ISO_4_BATHTUB ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_4_BATHTUB ] . obstacle_short_name = "Bath tub";
  obstacle_map [ ISO_4_BATHTUB ] . obstacle_long_description = "Taking a bath is not your current quest - what a pity !";  

  obstacle_map [ ISO_H_WASHTUB ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_H_WASHTUB ] . block_area_parm_1 = 0.5 ;
  obstacle_map [ ISO_H_WASHTUB ] . block_area_parm_2 = 0.4 ;
  obstacle_map [ ISO_H_WASHTUB ] . is_smashable = TRUE ;
  obstacle_map [ ISO_H_WASHTUB ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_H_WASHTUB ] . obstacle_short_name = "Washing tub";
  obstacle_map [ ISO_H_WASHTUB ] . obstacle_long_description = "A metal tub. The washing machine of the post-assault 21st century.";

  obstacle_map [ ISO_V_WASHTUB ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_V_WASHTUB ] . block_area_parm_1 = 0.4 ;
  obstacle_map [ ISO_V_WASHTUB ] . block_area_parm_2 = 0.5 ;
  obstacle_map [ ISO_V_WASHTUB ] . is_smashable = TRUE ;
  obstacle_map [ ISO_V_WASHTUB ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_V_WASHTUB ] . obstacle_short_name = "Washing tub";
  obstacle_map [ ISO_V_WASHTUB ] . obstacle_long_description = "A metal tub. The washing machine of the post-assault 21st century.";
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
  obstacle_map [ ISO_N_SOFA ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_S_SOFA ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_S_SOFA ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_S_SOFA ] . block_area_parm_2 = 0.5 ;
  obstacle_map [ ISO_S_SOFA ] . is_smashable = TRUE ;
  obstacle_map [ ISO_S_SOFA ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_E_SOFA ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_E_SOFA ] . block_area_parm_1 = 0.5 ;
  obstacle_map [ ISO_E_SOFA ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_E_SOFA ] . is_smashable = TRUE ;
  obstacle_map [ ISO_E_SOFA ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_W_SOFA ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_W_SOFA ] . block_area_parm_1 = 0.5 ;
  obstacle_map [ ISO_W_SOFA ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_W_SOFA ] . is_smashable = TRUE ;
  obstacle_map [ ISO_W_SOFA ] . block_vision_too = FALSE ;

  obstacle_map [ ISO_TREE_1 ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_TREE_1 ] . block_area_parm_1 = 0.4 ;
  obstacle_map [ ISO_TREE_1 ] . block_area_parm_2 = 0.4 ;
  obstacle_map [ ISO_TREE_1 ] . is_smashable = FALSE ;
  obstacle_map [ ISO_TREE_1 ] . obstacle_short_name = "Tree";
  obstacle_map [ ISO_TREE_1 ] . obstacle_long_description = "This tree seems quite normal";
  obstacle_map [ ISO_TREE_2 ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_TREE_2 ] . block_area_parm_1 = 0.4 ;
  obstacle_map [ ISO_TREE_2 ] . block_area_parm_2 = 0.4 ;
  obstacle_map [ ISO_TREE_2 ] . is_smashable = FALSE ;
  obstacle_map [ ISO_TREE_2 ] . obstacle_short_name = "Tree";
  obstacle_map [ ISO_TREE_2 ] . obstacle_long_description = "This is a nice tree";  
  obstacle_map [ ISO_TREE_3 ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_TREE_3 ] . block_area_parm_1 = 0.4 ;
  obstacle_map [ ISO_TREE_3 ] . block_area_parm_2 = 0.4 ;
  obstacle_map [ ISO_TREE_3 ] . is_smashable = FALSE ;
  obstacle_map [ ISO_TREE_3 ] . obstacle_short_name = "Tree";
  obstacle_map [ ISO_TREE_3 ] . obstacle_long_description = "No, it definitely doesn't look like a droid";

  obstacle_map [ ISO_TREE_4 ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_TREE_4 ] . block_area_parm_1 = 1.2 ;
  obstacle_map [ ISO_TREE_4 ] . block_area_parm_2 = 1.2 ;
  obstacle_map [ ISO_TREE_4 ] . filename = "iso_tree_0000.png" ;
  obstacle_map [ ISO_TREE_4 ] . obstacle_short_name = "Tree";
  obstacle_map [ ISO_TREE_4 ] . obstacle_long_description = "This tree does not look too healthy.";
  obstacle_map [ ISO_TREE_5 ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_TREE_5 ] . block_area_parm_1 = 1.2 ;
  obstacle_map [ ISO_TREE_5 ] . block_area_parm_2 = 1.2 ;
  obstacle_map [ ISO_TREE_5 ] . filename = "iso_tree_0001.png" ;
  obstacle_map [ ISO_TREE_5 ] . obstacle_short_name = "Tree";
  obstacle_map [ ISO_TREE_5 ] . obstacle_long_description = "For some strange reason this tree has blue leaves.";

  for( i = ISO_THICK_WALL_H ; i <= ISO_THICK_WALL_T_W ; i++) {
    obstacle_map [ i ] . transparent = TRANSPARENCY_FOR_WALLS ;
  }

  obstacle_map [ ISO_THICK_WALL_H ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_THICK_WALL_H ] . block_area_parm_1 = 1.1 ;
  obstacle_map [ ISO_THICK_WALL_H ] . block_area_parm_2 = 0.5 ;
  obstacle_map [ ISO_THICK_WALL_H ] . filename = "iso_thick_wall_0001.png" ;
  obstacle_map [ ISO_THICK_WALL_H ] . obstacle_short_name = "Colony wall" ;
  obstacle_map [ ISO_THICK_WALL_H ] . obstacle_long_description = "This is just a pink wall." ;
  obstacle_map [ ISO_THICK_WALL_V ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_THICK_WALL_V ] . block_area_parm_1 = 0.5 ;
  obstacle_map [ ISO_THICK_WALL_V ] . block_area_parm_2 = 1.1 ;
  obstacle_map [ ISO_THICK_WALL_V ] . filename = "iso_thick_wall_0002.png" ;
  obstacle_map [ ISO_THICK_WALL_V ] . obstacle_short_name = "Colony wall" ;
  obstacle_map [ ISO_THICK_WALL_V ] . obstacle_long_description = "This is just a pink wall." ;
  obstacle_map [ ISO_THICK_WALL_CORNER_NE ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_THICK_WALL_CORNER_NE ] . block_area_parm_1 = 1.1 ;
  obstacle_map [ ISO_THICK_WALL_CORNER_NE ] . block_area_parm_2 = 1.1 ;
  obstacle_map [ ISO_THICK_WALL_CORNER_NE ] . filename = "iso_thick_wall_0003.png" ;
  obstacle_map [ ISO_THICK_WALL_CORNER_NE ] . obstacle_short_name = "Colony wall" ;
  obstacle_map [ ISO_THICK_WALL_CORNER_NE ] . obstacle_long_description = "This is just a pink wall." ;
  obstacle_map [ ISO_THICK_WALL_CORNER_SE ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_THICK_WALL_CORNER_SE ] . block_area_parm_1 = 1.1 ;
  obstacle_map [ ISO_THICK_WALL_CORNER_SE ] . block_area_parm_2 = 1.1 ;
  obstacle_map [ ISO_THICK_WALL_CORNER_SE ] . filename = "iso_thick_wall_0004.png" ;
  obstacle_map [ ISO_THICK_WALL_CORNER_SE ] . obstacle_short_name = "Colony wall" ;
  obstacle_map [ ISO_THICK_WALL_CORNER_SE ] . obstacle_long_description = "This is just a pink wall." ;
  obstacle_map [ ISO_THICK_WALL_CORNER_NW ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_THICK_WALL_CORNER_NW ] . block_area_parm_1 = 1.1 ;
  obstacle_map [ ISO_THICK_WALL_CORNER_NW ] . block_area_parm_2 = 1.1 ;
  obstacle_map [ ISO_THICK_WALL_CORNER_NW ] . filename = "iso_thick_wall_0005.png" ;
  obstacle_map [ ISO_THICK_WALL_CORNER_NW ] . obstacle_short_name = "Colony wall" ;
  obstacle_map [ ISO_THICK_WALL_CORNER_NW ] . obstacle_long_description = "This is just a pink wall." ;
  obstacle_map [ ISO_THICK_WALL_CORNER_SW ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_THICK_WALL_CORNER_SW ] . block_area_parm_1 = 1.1 ;
  obstacle_map [ ISO_THICK_WALL_CORNER_SW ] . block_area_parm_2 = 1.1 ;
  obstacle_map [ ISO_THICK_WALL_CORNER_SW ] . filename = "iso_thick_wall_0006.png" ;
  obstacle_map [ ISO_THICK_WALL_CORNER_SW ] . obstacle_short_name = "Colony wall" ;
  obstacle_map [ ISO_THICK_WALL_CORNER_SW ] . obstacle_long_description = "This is just a pink wall." ;
  obstacle_map [ ISO_THICK_WALL_T_N ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_THICK_WALL_T_N ] . block_area_parm_1 = 1.1 ;
  obstacle_map [ ISO_THICK_WALL_T_N ] . block_area_parm_2 = 1.1 ;
  obstacle_map [ ISO_THICK_WALL_T_N ] . filename = "iso_thick_wall_0007.png" ;
  obstacle_map [ ISO_THICK_WALL_T_N ] . obstacle_short_name = "Colony wall" ;
  obstacle_map [ ISO_THICK_WALL_T_N ] . obstacle_long_description = "This is just a pink wall." ;
  obstacle_map [ ISO_THICK_WALL_T_E ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_THICK_WALL_T_E ] . block_area_parm_1 = 1.1 ;
  obstacle_map [ ISO_THICK_WALL_T_E ] . block_area_parm_2 = 1.1 ;
  obstacle_map [ ISO_THICK_WALL_T_E ] . filename = "iso_thick_wall_0008.png" ;
  obstacle_map [ ISO_THICK_WALL_T_E ] . obstacle_short_name = "Colony wall" ;
  obstacle_map [ ISO_THICK_WALL_T_E ] . obstacle_long_description = "This is just a pink wall." ;
  obstacle_map [ ISO_THICK_WALL_T_S ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_THICK_WALL_T_S ] . block_area_parm_1 = 1.1 ;
  obstacle_map [ ISO_THICK_WALL_T_S ] . block_area_parm_2 = 1.1 ;
  obstacle_map [ ISO_THICK_WALL_T_S ] . filename = "iso_thick_wall_0009.png" ;
  obstacle_map [ ISO_THICK_WALL_T_S ] . obstacle_short_name = "Colony wall" ;
  obstacle_map [ ISO_THICK_WALL_T_S ] . obstacle_long_description = "This is just a pink wall." ;
  obstacle_map [ ISO_THICK_WALL_T_W ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_THICK_WALL_T_W ] . block_area_parm_1 = 1.1 ;
  obstacle_map [ ISO_THICK_WALL_T_W ] . block_area_parm_2 = 1.1 ;
  obstacle_map [ ISO_THICK_WALL_T_W ] . filename = "iso_thick_wall_0010.png" ;  
  obstacle_map [ ISO_THICK_WALL_T_W ] . obstacle_short_name = "Colony wall" ;
  obstacle_map [ ISO_THICK_WALL_T_W ] . obstacle_long_description = "This is just a pink wall." ;


  // restaurant stuff
  
  obstacle_map [ ISO_RESTAURANT_SHELVES_1 ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_RESTAURANT_SHELVES_1 ] . block_area_parm_1 = 0.5 ;
  obstacle_map [ ISO_RESTAURANT_SHELVES_1 ] . block_area_parm_2 = 1.1 ;
  obstacle_map [ ISO_RESTAURANT_SHELVES_1 ] . filename = "iso_restaurant_furniture_0001.png" ;
  obstacle_map [ ISO_RESTAURANT_SHELVES_2 ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_RESTAURANT_SHELVES_2 ] . block_area_parm_1 = 0.5 ;
  obstacle_map [ ISO_RESTAURANT_SHELVES_2 ] . block_area_parm_2 = 1.1 ;
  obstacle_map [ ISO_RESTAURANT_SHELVES_2 ] . filename = "iso_restaurant_furniture_0002.png" ;
  obstacle_map [ ISO_RESTAURANT_SHELVES_3 ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_RESTAURANT_SHELVES_3 ] . block_area_parm_1 = 0.5 ;
  obstacle_map [ ISO_RESTAURANT_SHELVES_3 ] . block_area_parm_2 = 1.1 ;
  obstacle_map [ ISO_RESTAURANT_SHELVES_3 ] . filename = "iso_restaurant_furniture_0003.png" ;
  obstacle_map [ ISO_RESTAURANT_SHELVES_4 ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_RESTAURANT_SHELVES_4 ] . block_area_parm_1 = 0.5 ;
  obstacle_map [ ISO_RESTAURANT_SHELVES_4 ] . block_area_parm_2 = 1.1 ;
  obstacle_map [ ISO_RESTAURANT_SHELVES_4 ] . filename = "iso_restaurant_furniture_0004.png" ;
  obstacle_map [ ISO_RESTAURANT_SHELVES_5 ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_RESTAURANT_SHELVES_5 ] . block_area_parm_1 = 0.5 ;
  obstacle_map [ ISO_RESTAURANT_SHELVES_5 ] . block_area_parm_2 = 1.1 ;
  obstacle_map [ ISO_RESTAURANT_SHELVES_5 ] . filename = "iso_restaurant_furniture_0005.png" ;
  obstacle_map [ ISO_RESTAURANT_SHELVES_6 ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_RESTAURANT_SHELVES_6 ] . block_area_parm_1 = 0.5 ;
  obstacle_map [ ISO_RESTAURANT_SHELVES_6 ] . block_area_parm_2 = 1.1 ;
  obstacle_map [ ISO_RESTAURANT_SHELVES_6 ] . filename = "iso_restaurant_furniture_0006.png" ;
  obstacle_map [ ISO_RESTAURANT_SHELVES_7 ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_RESTAURANT_SHELVES_7 ] . block_area_parm_1 = 0.5 ;
  obstacle_map [ ISO_RESTAURANT_SHELVES_7 ] . block_area_parm_2 = 1.1 ;
  obstacle_map [ ISO_RESTAURANT_SHELVES_7 ] . filename = "iso_restaurant_furniture_0007.png" ;
  obstacle_map [ ISO_RESTAURANT_SHELVES_8 ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_RESTAURANT_SHELVES_8 ] . block_area_parm_1 = 0.5 ;
  obstacle_map [ ISO_RESTAURANT_SHELVES_8 ] . block_area_parm_2 = 1.1 ;
  obstacle_map [ ISO_RESTAURANT_SHELVES_8 ] . filename = "iso_restaurant_furniture_0008.png" ;
  obstacle_map [ ISO_RESTAURANT_SHELVES_9 ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_RESTAURANT_SHELVES_9 ] . block_area_parm_1 = 0.5 ;
  obstacle_map [ ISO_RESTAURANT_SHELVES_9 ] . block_area_parm_2 = 1.1 ;
  obstacle_map [ ISO_RESTAURANT_SHELVES_9 ] . filename = "iso_restaurant_furniture_0009.png" ;
  obstacle_map [ ISO_RESTAURANT_SHELVES_10 ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_RESTAURANT_SHELVES_10 ] . block_area_parm_1 = 0.5 ;
  obstacle_map [ ISO_RESTAURANT_SHELVES_10 ] . block_area_parm_2 = 1.1 ;
  obstacle_map [ ISO_RESTAURANT_SHELVES_10 ] . filename = "iso_restaurant_furniture_0010.png" ;
  
  int ahrot = 0;
  for ( ahrot = ISO_RESTAURANT_SHELVES_1; ahrot <= ISO_RESTAURANT_SHELVES_10; ahrot ++ )
	{
  	obstacle_map [ ahrot ] . obstacle_short_name = "Restaurant shelves" ;
	obstacle_map [ ahrot ] . obstacle_long_description = "This is but standard restaurant furniture." ;
	}

    
  
  obstacle_map [ ISO_CAVE_WALL_END_W ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_CAVE_WALL_END_W ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_CAVE_WALL_END_W ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_CAVE_WALL_END_W ] . filename = "iso_cave_wall_0007.png" ;
  obstacle_map [ ISO_CAVE_WALL_END_W ] . obstacle_short_name = "Rock wall" ;
  obstacle_map [ ISO_CAVE_WALL_END_W ] . obstacle_long_description = "This rock is here now and forever, and it is not going anywhere. Go somewhere else." ;
  obstacle_map [ ISO_CAVE_WALL_END_N ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_CAVE_WALL_END_N ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_CAVE_WALL_END_N ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_CAVE_WALL_END_N ] . filename = "iso_cave_wall_0008.png" ;
  obstacle_map [ ISO_CAVE_WALL_END_N ] . obstacle_short_name = "Rock wall" ;
  obstacle_map [ ISO_CAVE_WALL_END_N ] . obstacle_long_description = "This rock is here now and forever, and it is not going anywhere. Go somewhere else." ;
  obstacle_map [ ISO_CAVE_WALL_END_E ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_CAVE_WALL_END_E ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_CAVE_WALL_END_E ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_CAVE_WALL_END_E ] . filename = "iso_cave_wall_0009.png" ;
  obstacle_map [ ISO_CAVE_WALL_END_E ] . obstacle_short_name = "Rock wall" ;
  obstacle_map [ ISO_CAVE_WALL_END_E ] . obstacle_long_description = "This rock is here now and forever, and it is not going anywhere. Go somewhere else." ;
  obstacle_map [ ISO_CAVE_WALL_END_S ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_CAVE_WALL_END_S ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_CAVE_WALL_END_S ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_CAVE_WALL_END_S ] . filename = "iso_cave_wall_0010.png" ;
  obstacle_map [ ISO_CAVE_WALL_END_S ] . obstacle_short_name = "Rock wall" ;
  obstacle_map [ ISO_CAVE_WALL_END_S ] . obstacle_long_description = "This rock is here now and forever, and it is not going anywhere. Go somewhere else." ;

  for( i = ISO_GREY_WALL_END_W ; i <= ISO_GREY_WALL_END_S ; i++) {
    obstacle_map [ i ] . transparent = TRANSPARENCY_FOR_WALLS ;
  }
  obstacle_map [ ISO_GREY_WALL_END_W ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_GREY_WALL_END_W ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_GREY_WALL_END_W ] . block_area_parm_2 = 1.1 ;
  obstacle_map [ ISO_GREY_WALL_END_W ] . filename = "iso_walls_0005.png" ;
  obstacle_map [ ISO_GREY_WALL_END_N ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_GREY_WALL_END_N ] . block_area_parm_1 = 1.1 ;
  obstacle_map [ ISO_GREY_WALL_END_N ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_GREY_WALL_END_N ] . filename = "iso_walls_0006.png" ;
  /*description for all ISO_GREY_WALL_END_*/
  obstacle_map [ ISO_GREY_WALL_END_N ] . obstacle_short_name = "Grey wall";
  obstacle_map [ ISO_GREY_WALL_END_W ] . obstacle_short_name = "Grey wall";
  obstacle_map [ ISO_GREY_WALL_END_E ] . obstacle_short_name = "Grey wall";
  obstacle_map [ ISO_GREY_WALL_END_S ] . obstacle_short_name = "Grey wall";
  obstacle_map [ ISO_GREY_WALL_END_N ] . obstacle_long_description = "This is grey and seems quite hard to destroy. People usually call it a wall. You have no chance to go through : there must be another path.";
  obstacle_map [ ISO_GREY_WALL_END_W ] . obstacle_long_description = "This is grey and seems quite hard to destroy. People usually call it a wall. You have no chance to go through : there must be another path.";
  obstacle_map [ ISO_GREY_WALL_END_E ] . obstacle_long_description = "This is grey and seems quite hard to destroy. People usually call it a wall. You have no chance to go through : there must be another path.";
  obstacle_map [ ISO_GREY_WALL_END_S ] . obstacle_long_description = "This is grey and seems quite hard to destroy. People usually call it a wall. You have no chance to go through : there must be another path.";
  /**************************************/
  obstacle_map [ ISO_GREY_WALL_END_E ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_GREY_WALL_END_E ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_GREY_WALL_END_E ] . block_area_parm_2 = 1.1 ;
  obstacle_map [ ISO_GREY_WALL_END_E ] . filename = "iso_walls_0007.png" ;
  obstacle_map [ ISO_GREY_WALL_END_S ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_GREY_WALL_END_S ] . block_area_parm_1 = 1.1 ;
  obstacle_map [ ISO_GREY_WALL_END_S ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_GREY_WALL_END_S ] . filename = "iso_walls_0008.png" ;

  for( i = ISO_LIGHT_GREEN_WALL_1 ; i <= ISO_FUNKY_WALL_4 ; i++) {
    obstacle_map [ i ] . transparent = TRANSPARENCY_FOR_WALLS ;
  }


  obstacle_map [ ISO_LIGHT_GREEN_WALL_1 ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_LIGHT_GREEN_WALL_1 ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_LIGHT_GREEN_WALL_1 ] . block_area_parm_2 = 1.1 ;
  obstacle_map [ ISO_LIGHT_GREEN_WALL_1 ] . filename = "iso_walls_0010.png"; 
  obstacle_map [ ISO_LIGHT_GREEN_WALL_1 ] . obstacle_short_name = "Green wall" ;
  obstacle_map [ ISO_LIGHT_GREEN_WALL_1 ] . obstacle_long_description = "This wall is covered by an asparagus-green wallpaper.";

  obstacle_map [ ISO_LIGHT_GREEN_WALL_2 ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_LIGHT_GREEN_WALL_2 ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_LIGHT_GREEN_WALL_2 ] . block_area_parm_2 = 1.2 ;
  obstacle_map [ ISO_LIGHT_GREEN_WALL_2 ] . filename = "iso_walls_0011.png" ;
  obstacle_map [ ISO_LIGHT_GREEN_WALL_2 ] . obstacle_short_name = "Green wall" ;
  obstacle_map [ ISO_LIGHT_GREEN_WALL_2 ] . obstacle_long_description = "This wall is covered by an asparagus-green wallpaper.";

  obstacle_map [ ISO_FUNKY_WALL_1 ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_FUNKY_WALL_1 ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_FUNKY_WALL_1 ] . block_area_parm_2 = 1.2 ;
  obstacle_map [ ISO_FUNKY_WALL_1 ] . filename = "iso_walls_0012.png" ;
  obstacle_map [ ISO_FUNKY_WALL_1 ] . obstacle_short_name = "Groovy wall" ;
  obstacle_map [ ISO_FUNKY_WALL_1 ] . obstacle_long_description = "Man, dig that groovy wallpaper. It's a real blast!";

  obstacle_map [ ISO_FUNKY_WALL_2 ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_FUNKY_WALL_2 ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_FUNKY_WALL_2 ] . block_area_parm_2 = 1.2 ;
  obstacle_map [ ISO_FUNKY_WALL_2 ] . filename = "iso_walls_0013.png" ;
  obstacle_map [ ISO_FUNKY_WALL_2 ] . obstacle_short_name = "Groovy wall" ;
  obstacle_map [ ISO_FUNKY_WALL_2 ] . obstacle_long_description = "Man, dig that groovy wallpaper. It's a real blast!";

  obstacle_map [ ISO_FUNKY_WALL_3 ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_FUNKY_WALL_3 ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_FUNKY_WALL_3 ] . block_area_parm_2 = 1.2 ;
  obstacle_map [ ISO_FUNKY_WALL_3 ] . filename = "iso_walls_0014.png" ;
  obstacle_map [ ISO_FUNKY_WALL_3 ] . obstacle_short_name = "Groovy wall" ;
  obstacle_map [ ISO_FUNKY_WALL_3 ] . obstacle_long_description = "Man, dig that groovy wallpaper. It's a real blast!";

  obstacle_map [ ISO_FUNKY_WALL_4 ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_FUNKY_WALL_4 ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_FUNKY_WALL_4 ] . block_area_parm_2 = 1.2 ;
  obstacle_map [ ISO_FUNKY_WALL_4 ] . filename = "iso_walls_0015.png" ;
  obstacle_map [ ISO_FUNKY_WALL_4 ] . obstacle_short_name = "Groovy wall" ;
  obstacle_map [ ISO_FUNKY_WALL_4 ] . obstacle_long_description = "Man, dig that groovy wallpaper. It's a real blast!";

  for( i = ISO_BRICK_WALL_H ; i <= ISO_BRICK_WALL_CORNER_4 ; i++) {
    obstacle_map [ i ] . transparent = TRANSPARENCY_FOR_WALLS ;
  }

  //--------------------
  // Brick walls are smashable.  When you smash them, there should be
  // first a cracked brick wall, then when smashing again, there will
  // be only some rubble left, and that should be *passable*.
  //
  // UPDATE: But maybe this wall shouldn't be smashable, since it isn't cracked...
  //         If thas should be different, it can be changes easily below by setting
  //         the smashable property to the desired value.
  //
  obstacle_map [ ISO_BRICK_WALL_H ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_BRICK_WALL_H ] . block_area_parm_1 = 1.2 ;
  obstacle_map [ ISO_BRICK_WALL_H ] . block_area_parm_2 = 1.2 ;
  obstacle_map [ ISO_BRICK_WALL_H ] . filename = "iso_brick_wall_0001.png" ;
  obstacle_map [ ISO_BRICK_WALL_H ] . is_smashable = FALSE ;
  obstacle_map [ ISO_BRICK_WALL_H ] . result_type_after_smashing_once = ISO_BRICK_WALL_CRACKED_1 ;
  obstacle_map [ ISO_BRICK_WALL_V ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_BRICK_WALL_V ] . block_area_parm_1 = 1.2 ;
  obstacle_map [ ISO_BRICK_WALL_V ] . block_area_parm_2 = 1.2 ;
  obstacle_map [ ISO_BRICK_WALL_V ] . filename = "iso_brick_wall_0002.png" ;
  obstacle_map [ ISO_BRICK_WALL_V ] . is_smashable = FALSE ;
  obstacle_map [ ISO_BRICK_WALL_V ] . result_type_after_smashing_once = ISO_BRICK_WALL_CRACKED_2 ;

  obstacle_map [ ISO_BRICK_WALL_END ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_BRICK_WALL_END ] . block_area_parm_1 = 1.2 ;
  obstacle_map [ ISO_BRICK_WALL_END ] . block_area_parm_2 = 1.2 ;
  obstacle_map [ ISO_BRICK_WALL_END ] . filename = "iso_brick_wall_0003.png" ;

  obstacle_map [ ISO_BRICK_WALL_CORNER_1 ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_BRICK_WALL_CORNER_1 ] . block_area_parm_1 = 1.2 ;
  obstacle_map [ ISO_BRICK_WALL_CORNER_1 ] . block_area_parm_2 = 1.2 ;
  obstacle_map [ ISO_BRICK_WALL_CORNER_1 ] . filename = "iso_brick_wall_0004.png" ;
  obstacle_map [ ISO_BRICK_WALL_CORNER_2 ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_BRICK_WALL_CORNER_2 ] . block_area_parm_1 = 1.2 ;
  obstacle_map [ ISO_BRICK_WALL_CORNER_2 ] . block_area_parm_2 = 1.2 ;
  obstacle_map [ ISO_BRICK_WALL_CORNER_2 ] . filename = "iso_brick_wall_0005.png" ;
  obstacle_map [ ISO_BRICK_WALL_CORNER_3 ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_BRICK_WALL_CORNER_3 ] . block_area_parm_1 = 1.2 ;
  obstacle_map [ ISO_BRICK_WALL_CORNER_3 ] . block_area_parm_2 = 1.2 ;
  obstacle_map [ ISO_BRICK_WALL_CORNER_3 ] . filename = "iso_brick_wall_0006.png" ;
  obstacle_map [ ISO_BRICK_WALL_CORNER_4 ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_BRICK_WALL_CORNER_4 ] . block_area_parm_1 = 1.2 ;
  obstacle_map [ ISO_BRICK_WALL_CORNER_4 ] . block_area_parm_2 = 1.2 ;
  obstacle_map [ ISO_BRICK_WALL_CORNER_4 ] . filename = "iso_brick_wall_0007.png" ;
  
  for( i = ISO_BRICK_WALL_JUNCTION_1 ; i <= ISO_BRICK_WALL_RUBBLE_2  ; i++) {
    obstacle_map [ i ] . transparent = TRANSPARENCY_FOR_WALLS ;
  }

  obstacle_map [ ISO_BRICK_WALL_JUNCTION_1 ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_BRICK_WALL_JUNCTION_1 ] . block_area_parm_1 = 1.2 ;
  obstacle_map [ ISO_BRICK_WALL_JUNCTION_1 ] . block_area_parm_2 = 1.2 ;
  obstacle_map [ ISO_BRICK_WALL_JUNCTION_1 ] . filename = "iso_brick_wall_0008.png" ;
  obstacle_map [ ISO_BRICK_WALL_JUNCTION_2 ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_BRICK_WALL_JUNCTION_2 ] . block_area_parm_1 = 1.2 ;
  obstacle_map [ ISO_BRICK_WALL_JUNCTION_2 ] . block_area_parm_2 = 1.2 ;
  obstacle_map [ ISO_BRICK_WALL_JUNCTION_2 ] . filename = "iso_brick_wall_0009.png" ;
  obstacle_map [ ISO_BRICK_WALL_JUNCTION_3 ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_BRICK_WALL_JUNCTION_3 ] . block_area_parm_1 = 1.2 ;
  obstacle_map [ ISO_BRICK_WALL_JUNCTION_3 ] . block_area_parm_2 = 1.2 ;
  obstacle_map [ ISO_BRICK_WALL_JUNCTION_3 ] . filename = "iso_brick_wall_0010.png" ;
  obstacle_map [ ISO_BRICK_WALL_JUNCTION_4 ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_BRICK_WALL_JUNCTION_4 ] . block_area_parm_1 = 1.2 ;
  obstacle_map [ ISO_BRICK_WALL_JUNCTION_4 ] . block_area_parm_2 = 1.2 ;
  obstacle_map [ ISO_BRICK_WALL_JUNCTION_4 ] . filename = "iso_brick_wall_0011.png" ;

  //all brick walls above share the same description
  obstacle_map [ ISO_BRICK_WALL_JUNCTION_4 ] . obstacle_short_name = "Brick wall" ;
  obstacle_map [ ISO_BRICK_WALL_JUNCTION_4 ] . obstacle_long_description = "A badly constructed brick wall. Maybe you could alter it...";
  obstacle_map [ ISO_BRICK_WALL_JUNCTION_3 ] . obstacle_short_name = obstacle_map [ ISO_BRICK_WALL_JUNCTION_4 ] . obstacle_short_name;
  obstacle_map [ ISO_BRICK_WALL_JUNCTION_3 ] . obstacle_long_description = obstacle_map [ ISO_BRICK_WALL_JUNCTION_4 ] . obstacle_long_description;
  obstacle_map [ ISO_BRICK_WALL_JUNCTION_2 ] . obstacle_short_name = obstacle_map [ ISO_BRICK_WALL_JUNCTION_4 ] . obstacle_short_name;
  obstacle_map [ ISO_BRICK_WALL_JUNCTION_2 ] . obstacle_long_description = obstacle_map [ ISO_BRICK_WALL_JUNCTION_4 ] . obstacle_long_description;
  obstacle_map [ ISO_BRICK_WALL_JUNCTION_1 ] . obstacle_short_name = obstacle_map [ ISO_BRICK_WALL_JUNCTION_4 ] . obstacle_short_name;
  obstacle_map [ ISO_BRICK_WALL_JUNCTION_1 ] . obstacle_long_description = obstacle_map [ ISO_BRICK_WALL_JUNCTION_4 ] . obstacle_long_description;
  obstacle_map [ ISO_BRICK_WALL_CORNER_1 ] . obstacle_short_name = obstacle_map [ ISO_BRICK_WALL_JUNCTION_4 ] . obstacle_short_name;
  obstacle_map [ ISO_BRICK_WALL_CORNER_1 ] . obstacle_long_description = obstacle_map [ ISO_BRICK_WALL_JUNCTION_4 ] . obstacle_long_description;
  obstacle_map [ ISO_BRICK_WALL_CORNER_2 ] . obstacle_short_name = obstacle_map [ ISO_BRICK_WALL_JUNCTION_4 ] . obstacle_short_name;
  obstacle_map [ ISO_BRICK_WALL_CORNER_2 ] . obstacle_long_description = obstacle_map [ ISO_BRICK_WALL_JUNCTION_4 ] . obstacle_long_description;
  obstacle_map [ ISO_BRICK_WALL_CORNER_3 ] . obstacle_short_name = obstacle_map [ ISO_BRICK_WALL_JUNCTION_4 ] . obstacle_short_name;
  obstacle_map [ ISO_BRICK_WALL_CORNER_3 ] . obstacle_long_description = obstacle_map [ ISO_BRICK_WALL_JUNCTION_4 ] . obstacle_long_description;
  obstacle_map [ ISO_BRICK_WALL_CORNER_4 ] . obstacle_short_name = obstacle_map [ ISO_BRICK_WALL_JUNCTION_4 ] . obstacle_short_name;
  obstacle_map [ ISO_BRICK_WALL_CORNER_4 ] . obstacle_long_description = obstacle_map [ ISO_BRICK_WALL_JUNCTION_4 ] . obstacle_long_description;
  obstacle_map [ ISO_BRICK_WALL_END ] . obstacle_short_name = obstacle_map [ ISO_BRICK_WALL_JUNCTION_4 ] . obstacle_short_name;
  obstacle_map [ ISO_BRICK_WALL_END ] . obstacle_long_description = obstacle_map [ ISO_BRICK_WALL_JUNCTION_4 ] . obstacle_long_description;
  obstacle_map [ ISO_BRICK_WALL_H ] . obstacle_short_name = obstacle_map [ ISO_BRICK_WALL_JUNCTION_4 ] . obstacle_short_name;
  obstacle_map [ ISO_BRICK_WALL_H ] . obstacle_long_description = obstacle_map [ ISO_BRICK_WALL_JUNCTION_4 ] . obstacle_long_description;
  obstacle_map [ ISO_BRICK_WALL_V ] . obstacle_short_name = obstacle_map [ ISO_BRICK_WALL_JUNCTION_4 ] . obstacle_short_name;
  obstacle_map [ ISO_BRICK_WALL_V ] . obstacle_long_description = obstacle_map [ ISO_BRICK_WALL_JUNCTION_4 ] . obstacle_long_description;


  //--------------------
  // Brick walls are smashable.  When you smash them, there should be
  // first a cracked brick wall, then when smashing again, there will
  // be only some rubble left, and that should be *passable*.
  //
  obstacle_map [ ISO_BRICK_WALL_CRACKED_1 ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_BRICK_WALL_CRACKED_1 ] . block_area_parm_1 = 1.2 ;
  obstacle_map [ ISO_BRICK_WALL_CRACKED_1 ] . block_area_parm_2 = 1.2 ;
  obstacle_map [ ISO_BRICK_WALL_CRACKED_1 ] . filename = "iso_brick_wall_0012.png" ;
  obstacle_map [ ISO_BRICK_WALL_CRACKED_1 ] . is_smashable = TRUE ;
  obstacle_map [ ISO_BRICK_WALL_CRACKED_1 ] . result_type_after_smashing_once = ISO_BRICK_WALL_RUBBLE_1 ;
  obstacle_map [ ISO_BRICK_WALL_CRACKED_2 ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_BRICK_WALL_CRACKED_2 ] . block_area_parm_1 = 1.2 ;
  obstacle_map [ ISO_BRICK_WALL_CRACKED_2 ] . block_area_parm_2 = 1.2 ;
  obstacle_map [ ISO_BRICK_WALL_CRACKED_2 ] . filename = "iso_brick_wall_0013.png" ;
  obstacle_map [ ISO_BRICK_WALL_CRACKED_2 ] . result_type_after_smashing_once = ISO_BRICK_WALL_RUBBLE_2 ;
  obstacle_map [ ISO_BRICK_WALL_CRACKED_2 ] . is_smashable = TRUE ;
  obstacle_map [ ISO_BRICK_WALL_CRACKED_2 ] . obstacle_short_name = "Wall";
  obstacle_map [ ISO_BRICK_WALL_CRACKED_2 ] . obstacle_long_description = "This wall is suffering. Finish it!";
  obstacle_map [ ISO_BRICK_WALL_CRACKED_1 ] . obstacle_short_name = "Wall";
  obstacle_map [ ISO_BRICK_WALL_CRACKED_1 ] . obstacle_long_description = "This wall is suffering. Finish it!"; // Magotari: I changed this to be a reference to Mortal Kombat. Feel free to revert if the original author wants his version back.
  
  obstacle_map [ ISO_BRICK_WALL_RUBBLE_1 ] . block_area_type = COLLISION_TYPE_NONE  ;
  obstacle_map [ ISO_BRICK_WALL_RUBBLE_1 ] . block_area_parm_1 = 1.2 ;
  obstacle_map [ ISO_BRICK_WALL_RUBBLE_1 ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_BRICK_WALL_RUBBLE_1 ] . filename = "iso_brick_wall_0014.png" ;
  obstacle_map [ ISO_BRICK_WALL_RUBBLE_2 ] . block_area_type = COLLISION_TYPE_NONE ;
  obstacle_map [ ISO_BRICK_WALL_RUBBLE_2 ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_BRICK_WALL_RUBBLE_2 ] . block_area_parm_2 = 1.2 ;
  obstacle_map [ ISO_BRICK_WALL_RUBBLE_2 ] . filename = "iso_brick_wall_0015.png" ;
  obstacle_map [ ISO_BRICK_WALL_RUBBLE_2 ] . obstacle_short_name = "Wall";
  obstacle_map [ ISO_BRICK_WALL_RUBBLE_2 ] . obstacle_long_description = "Well ... it was not very resistant ";
  obstacle_map [ ISO_BRICK_WALL_RUBBLE_1 ] . obstacle_short_name = "Wall";
  obstacle_map [ ISO_BRICK_WALL_RUBBLE_1 ] . obstacle_long_description = "Well ... it was not very resistant ";
  

  obstacle_map [ ISO_BLOOD_1 ] . block_area_type = COLLISION_TYPE_NONE ;
  obstacle_map [ ISO_BLOOD_1 ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_BLOOD_1 ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_BLOOD_1 ] . filename = "iso_blood_0001.png" ;
  obstacle_map [ ISO_BLOOD_1 ] . needs_pre_put = TRUE ;
  obstacle_map [ ISO_BLOOD_2 ] . block_area_type = COLLISION_TYPE_NONE ;
  obstacle_map [ ISO_BLOOD_2 ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_BLOOD_2 ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_BLOOD_2 ] . filename = "iso_blood_0002.png" ;
  obstacle_map [ ISO_BLOOD_2 ] . needs_pre_put = TRUE ;
  obstacle_map [ ISO_BLOOD_3 ] . block_area_type = COLLISION_TYPE_NONE ;
  obstacle_map [ ISO_BLOOD_3 ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_BLOOD_3 ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_BLOOD_3 ] . filename = "iso_blood_0003.png" ;
  obstacle_map [ ISO_BLOOD_3 ] . needs_pre_put = TRUE ;
  obstacle_map [ ISO_BLOOD_4 ] . block_area_type = COLLISION_TYPE_NONE ;
  obstacle_map [ ISO_BLOOD_4 ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_BLOOD_4 ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_BLOOD_4 ] . filename = "iso_blood_0004.png" ;
  obstacle_map [ ISO_BLOOD_4 ] . needs_pre_put = TRUE ;
  obstacle_map [ ISO_BLOOD_5 ] . block_area_type = COLLISION_TYPE_NONE ;
  obstacle_map [ ISO_BLOOD_5 ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_BLOOD_5 ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_BLOOD_5 ] . filename = "iso_blood_0005.png" ;
  obstacle_map [ ISO_BLOOD_5 ] . needs_pre_put = TRUE ;
  obstacle_map [ ISO_BLOOD_6 ] . block_area_type = COLLISION_TYPE_NONE ;
  obstacle_map [ ISO_BLOOD_6 ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_BLOOD_6 ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_BLOOD_6 ] . filename = "iso_blood_0006.png" ;
  obstacle_map [ ISO_BLOOD_6 ] . needs_pre_put = TRUE ;
  obstacle_map [ ISO_BLOOD_7 ] . block_area_type = COLLISION_TYPE_NONE ;
  obstacle_map [ ISO_BLOOD_7 ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_BLOOD_7 ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_BLOOD_7 ] . filename = "iso_blood_0007.png" ;
  obstacle_map [ ISO_BLOOD_7 ] . needs_pre_put = TRUE ;
  obstacle_map [ ISO_BLOOD_8 ] . block_area_type = COLLISION_TYPE_NONE ;
  obstacle_map [ ISO_BLOOD_8 ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_BLOOD_8 ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_BLOOD_8 ] . filename = "iso_blood_0008.png" ;
  obstacle_map [ ISO_BLOOD_8 ] . needs_pre_put = TRUE ;
  // the eight blood tiles have the same description - normal, right ?
  obstacle_map [ ISO_BLOOD_1 ] . obstacle_short_name = "Blood";
  obstacle_map [ ISO_BLOOD_1 ] . obstacle_long_description = "Do droids bleed ? These do.";
  obstacle_map [ ISO_BLOOD_2 ] . obstacle_short_name = obstacle_map [ ISO_BLOOD_1 ] . obstacle_short_name;
  obstacle_map [ ISO_BLOOD_2 ] . obstacle_long_description = obstacle_map [ ISO_BLOOD_1 ] . obstacle_long_description;
  obstacle_map [ ISO_BLOOD_3 ] . obstacle_short_name = obstacle_map [ ISO_BLOOD_1 ] . obstacle_short_name;
  obstacle_map [ ISO_BLOOD_3 ] . obstacle_long_description = obstacle_map [ ISO_BLOOD_1 ] . obstacle_long_description;
  obstacle_map [ ISO_BLOOD_4 ] . obstacle_short_name = obstacle_map [ ISO_BLOOD_1 ] . obstacle_short_name;
  obstacle_map [ ISO_BLOOD_4 ] . obstacle_long_description = obstacle_map [ ISO_BLOOD_1 ] . obstacle_long_description;
  obstacle_map [ ISO_BLOOD_5 ] . obstacle_short_name = obstacle_map [ ISO_BLOOD_1 ] . obstacle_short_name;
  obstacle_map [ ISO_BLOOD_5 ] . obstacle_long_description = obstacle_map [ ISO_BLOOD_1 ] . obstacle_long_description;
  obstacle_map [ ISO_BLOOD_6 ] . obstacle_short_name = obstacle_map [ ISO_BLOOD_1 ] . obstacle_short_name;
  obstacle_map [ ISO_BLOOD_6 ] . obstacle_long_description = obstacle_map [ ISO_BLOOD_1 ] . obstacle_long_description;
  obstacle_map [ ISO_BLOOD_7 ] . obstacle_short_name = obstacle_map [ ISO_BLOOD_1 ] . obstacle_short_name;
  obstacle_map [ ISO_BLOOD_7 ] . obstacle_long_description = obstacle_map [ ISO_BLOOD_1 ] . obstacle_long_description;
  obstacle_map [ ISO_BLOOD_8 ] . obstacle_short_name = obstacle_map [ ISO_BLOOD_1 ] . obstacle_short_name;
  obstacle_map [ ISO_BLOOD_8 ] . obstacle_long_description = obstacle_map [ ISO_BLOOD_1 ] . obstacle_long_description;
  
 

  obstacle_map [ ISO_EXIT_1 ] . block_area_type = COLLISION_TYPE_NONE ;
  obstacle_map [ ISO_EXIT_1 ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_EXIT_1 ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_EXIT_1 ] . filename = "iso_exits_0001.png" ;
  obstacle_map [ ISO_EXIT_1 ] . needs_pre_put = FALSE ;
  obstacle_map [ ISO_EXIT_1 ] . obstacle_short_name = "Entrance";
  obstacle_map [ ISO_EXIT_1 ] . obstacle_long_description = "It leads into the dark tunnels below...";

  obstacle_map [ ISO_EXIT_2 ] . block_area_type = COLLISION_TYPE_NONE ;
  obstacle_map [ ISO_EXIT_2 ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_EXIT_2 ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_EXIT_2 ] . filename = "iso_exits_0002.png" ;
  obstacle_map [ ISO_EXIT_2 ] . needs_pre_put = FALSE ;
  obstacle_map [ ISO_EXIT_2 ] . obstacle_short_name = "Entrance";
  obstacle_map [ ISO_EXIT_2 ] . obstacle_long_description = "It leads into the dark tunnels below...";

  obstacle_map [ ISO_EXIT_3 ] . block_area_type = COLLISION_TYPE_NONE ;
  obstacle_map [ ISO_EXIT_3 ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_EXIT_3 ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_EXIT_3 ] . filename = "iso_exits_0003.png" ;
  obstacle_map [ ISO_EXIT_3 ] . needs_pre_put = FALSE ;
  obstacle_map [ ISO_EXIT_3 ] . emitted_light_strength = 10 ;
  obstacle_map [ ISO_EXIT_3 ] . obstacle_short_name = "Ladder";
  obstacle_map [ ISO_EXIT_3 ] . obstacle_long_description = "It leads to the surface world.";

  obstacle_map [ ISO_EXIT_4 ] . block_area_type = COLLISION_TYPE_NONE ;
  obstacle_map [ ISO_EXIT_4 ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_EXIT_4 ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_EXIT_4 ] . filename = "iso_exits_0004.png" ;
  obstacle_map [ ISO_EXIT_4 ] . needs_pre_put = FALSE ;
  obstacle_map [ ISO_EXIT_4 ] . emitted_light_strength = 10 ;
  obstacle_map [ ISO_EXIT_4 ] . obstacle_short_name = "Ladder";
  obstacle_map [ ISO_EXIT_4 ] . obstacle_long_description = "It leads to the surface world.";

  //--------------------
  // This is the wonderful littel exotic plant provided by Basse.
  // It will block the Tux movement but vision should pass through
  // as it's not a particularly high object, so you can see over it.
  //
  obstacle_map [ ISO_ROCKS_N_PLANTS_1 ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_ROCKS_N_PLANTS_1 ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_ROCKS_N_PLANTS_1 ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_ROCKS_N_PLANTS_1 ] . filename = "iso_rocks_n_plants_0000.png" ;
  obstacle_map [ ISO_ROCKS_N_PLANTS_1 ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_ROCKS_N_PLANTS_1 ] . emitted_light_strength = -9 ;
  obstacle_map [ ISO_ROCKS_N_PLANTS_1 ] . obstacle_short_name = "Exotic plant";
  obstacle_map [ ISO_ROCKS_N_PLANTS_1 ] . obstacle_long_description = "A wonderful exotic plant. You can read \"Made by Basse\" on it";

  obstacle_map [ ISO_ROCKS_N_PLANTS_2 ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_ROCKS_N_PLANTS_2 ] . block_area_parm_1 = 1.5 ;
  obstacle_map [ ISO_ROCKS_N_PLANTS_2 ] . block_area_parm_2 = 1.5 ;
  obstacle_map [ ISO_ROCKS_N_PLANTS_2 ] . filename = "iso_rocks_n_plants_0001.png" ;

  obstacle_map [ ISO_ROCKS_N_PLANTS_3 ] . block_area_type = COLLISION_TYPE_NONE ;
  obstacle_map [ ISO_ROCKS_N_PLANTS_3 ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_ROCKS_N_PLANTS_3 ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_ROCKS_N_PLANTS_3 ] . filename = "iso_rocks_n_plants_0002.png" ;
  obstacle_map [ ISO_ROCKS_N_PLANTS_3 ] . needs_pre_put = TRUE ;

  obstacle_map [ ISO_ROCKS_N_PLANTS_4 ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_ROCKS_N_PLANTS_4 ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_ROCKS_N_PLANTS_4 ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_ROCKS_N_PLANTS_4 ] . filename = "iso_rocks_n_plants_0003.png" ;
  
  //the three rocks above have the same description for the moment
  //inspired by fallout1
  obstacle_map [ ISO_ROCKS_N_PLANTS_2 ] . obstacle_short_name = "Rocks";
  obstacle_map [ ISO_ROCKS_N_PLANTS_2 ] . obstacle_long_description = "Do you think a rock can attack you ? No, MS droids can't hide themselves that way : they crash before.";
  obstacle_map [ ISO_ROCKS_N_PLANTS_3 ] . obstacle_short_name = obstacle_map [ ISO_ROCKS_N_PLANTS_2 ] . obstacle_short_name;
  obstacle_map [ ISO_ROCKS_N_PLANTS_3 ] . obstacle_long_description = obstacle_map [ ISO_ROCKS_N_PLANTS_2 ] . obstacle_long_description;
  obstacle_map [ ISO_ROCKS_N_PLANTS_4 ] . obstacle_short_name = obstacle_map [ ISO_ROCKS_N_PLANTS_2 ] . obstacle_short_name;
  obstacle_map [ ISO_ROCKS_N_PLANTS_4 ] . obstacle_long_description = obstacle_map [ ISO_ROCKS_N_PLANTS_2 ] . obstacle_long_description;


  obstacle_map [ ISO_ROCKS_N_PLANTS_5 ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_ROCKS_N_PLANTS_5 ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_ROCKS_N_PLANTS_5 ] . filename = "iso_rocks_n_plants_0004.png" ;
  obstacle_map [ ISO_ROCKS_N_PLANTS_5 ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_ROCKS_N_PLANTS_5 ] . emitted_light_strength = -12 ;

  obstacle_map [ ISO_ROCKS_N_PLANTS_6 ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_ROCKS_N_PLANTS_6 ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_ROCKS_N_PLANTS_6 ] . filename = "iso_rocks_n_plants_0005.png" ;
  obstacle_map [ ISO_ROCKS_N_PLANTS_6 ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_ROCKS_N_PLANTS_6 ] . emitted_light_strength = -10 ;
  
  obstacle_map [ ISO_ROCKS_N_PLANTS_7 ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_ROCKS_N_PLANTS_7 ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_ROCKS_N_PLANTS_7 ] . filename = "iso_rocks_n_plants_0006.png" ;
  obstacle_map [ ISO_ROCKS_N_PLANTS_7 ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_ROCKS_N_PLANTS_7 ] . emitted_light_strength = -11 ;

  obstacle_map [ ISO_ROCKS_N_PLANTS_8 ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_ROCKS_N_PLANTS_8 ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_ROCKS_N_PLANTS_8 ] . filename = "iso_rocks_n_plants_0007.png" ;
  obstacle_map [ ISO_ROCKS_N_PLANTS_8 ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_ROCKS_N_PLANTS_8 ] . emitted_light_strength = -8 ;
  
  obstacle_map [ ISO_ROCKS_N_PLANTS_5 ] . obstacle_short_name = "Exotic plant";
  obstacle_map [ ISO_ROCKS_N_PLANTS_5 ] . obstacle_long_description = "Eating too much of that could lead to a kernel panic.";
  obstacle_map [ ISO_ROCKS_N_PLANTS_6 ] . obstacle_short_name = obstacle_map [ ISO_ROCKS_N_PLANTS_5 ] . obstacle_short_name;
  obstacle_map [ ISO_ROCKS_N_PLANTS_6 ] . obstacle_long_description = obstacle_map [ ISO_ROCKS_N_PLANTS_5 ] . obstacle_long_description;
  obstacle_map [ ISO_ROCKS_N_PLANTS_7 ] . obstacle_short_name = obstacle_map [ ISO_ROCKS_N_PLANTS_5 ] . obstacle_short_name;
  obstacle_map [ ISO_ROCKS_N_PLANTS_7 ] . obstacle_long_description = obstacle_map [ ISO_ROCKS_N_PLANTS_5 ] . obstacle_long_description;
  obstacle_map [ ISO_ROCKS_N_PLANTS_8 ] . obstacle_short_name = obstacle_map [ ISO_ROCKS_N_PLANTS_1 ] . obstacle_short_name;
  obstacle_map [ ISO_ROCKS_N_PLANTS_8 ] . obstacle_long_description = obstacle_map [ ISO_ROCKS_N_PLANTS_1 ] . obstacle_long_description;  
    
  for ( i = ISO_ROOM_WALL_V_RED ; i <= ISO_ROOM_WALL_H_GREEN ; i++) 
  {
    obstacle_map [ i ] . transparent = TRANSPARENCY_FOR_WALLS ;
  }

  obstacle_map [ ISO_ROOM_WALL_V_RED ]  . block_area_parm_1 = standard_wall_thickness ;
  obstacle_map [ ISO_ROOM_WALL_V_RED ]  . block_area_parm_2 = 1.2 ;
  obstacle_map [ ISO_ROOM_WALL_V_RED ]  . filename = "iso_walls_0016.png" ;
  obstacle_map [ ISO_ROOM_WALL_H_RED ]  . block_area_parm_1 = 1.2 ;
  obstacle_map [ ISO_ROOM_WALL_H_RED ]  . block_area_parm_2 = standard_wall_thickness ;
  obstacle_map [ ISO_ROOM_WALL_H_RED ]  . filename = "iso_walls_0017.png" ;
  obstacle_map [ ISO_ROOM_WALL_V_GREEN ] . block_area_parm_1 = standard_wall_thickness ;
  obstacle_map [ ISO_ROOM_WALL_V_GREEN ] . block_area_parm_2 = 1.2 ;
  obstacle_map [ ISO_ROOM_WALL_V_GREEN ] . filename = "iso_walls_0018.png" ;
  obstacle_map [ ISO_ROOM_WALL_H_GREEN ] . block_area_parm_1 = 1.2 ;
  obstacle_map [ ISO_ROOM_WALL_H_GREEN ] . block_area_parm_2 = standard_wall_thickness ;
  obstacle_map [ ISO_ROOM_WALL_H_GREEN ] . filename = "iso_walls_0019.png" ;
  obstacle_map [ ISO_ROOM_WALL_V_RED ] . obstacle_short_name = "Red wall";
  obstacle_map [ ISO_ROOM_WALL_H_RED ] . obstacle_short_name = "Red wall";
  obstacle_map [ ISO_ROOM_WALL_V_RED ] . obstacle_long_description = "You think it would be better with a penguin on it. You are right.";
  obstacle_map [ ISO_ROOM_WALL_H_RED ] . obstacle_long_description = "You think it would be better with a penguin on it. You are right.";
  obstacle_map [ ISO_ROOM_WALL_V_GREEN ] . obstacle_short_name = "Green wall";
  obstacle_map [ ISO_ROOM_WALL_H_GREEN ] . obstacle_short_name = "Green wall";
  obstacle_map [ ISO_ROOM_WALL_V_GREEN ] . obstacle_long_description = "You think it would be better with a penguin on it. You are right.";
  obstacle_map [ ISO_ROOM_WALL_H_GREEN ] . obstacle_long_description = "You think it would be better with a penguin on it. You are right.";
  //--------------------
  // These two are for the big long shop counter.  It has a suitable
  // collision rectangle, but light may pass through, so you can see
  // the person behind the counter
  //
  obstacle_map [ ISO_SHOP_FURNITURE_1 ] . block_area_parm_1 = 3.5 ;
  obstacle_map [ ISO_SHOP_FURNITURE_1 ] . block_area_parm_2 = 1.5 ;
  obstacle_map [ ISO_SHOP_FURNITURE_1 ] . filename = "iso_shop_furniture_0001.png" ;
  obstacle_map [ ISO_SHOP_FURNITURE_1 ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_SHOP_FURNITURE_1 ] . obstacle_short_name = "Shop counter" ;
  obstacle_map [ ISO_SHOP_FURNITURE_1 ] . obstacle_long_description = "This model of the counter features a built-in cash register and bright neon lights." ;
  obstacle_map [ ISO_SHOP_FURNITURE_2 ] . block_area_parm_1 = 1.5 ;
  obstacle_map [ ISO_SHOP_FURNITURE_2 ] . block_area_parm_2 = 3.5 ;
  obstacle_map [ ISO_SHOP_FURNITURE_2 ] . filename = "iso_shop_furniture_0002.png" ;
  obstacle_map [ ISO_SHOP_FURNITURE_2 ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_SHOP_FURNITURE_2 ] . obstacle_short_name = "Shop counter" ;
  obstacle_map [ ISO_SHOP_FURNITURE_2 ] . obstacle_long_description = "This model of the counter features a built-in cash register and bright neon lights." ;

  obstacle_map [ ISO_SHOP_FURNITURE_3 ] . block_area_parm_1 = 2.3 ;
  obstacle_map [ ISO_SHOP_FURNITURE_3 ] . block_area_parm_2 = 1.1 ;
  obstacle_map [ ISO_SHOP_FURNITURE_3 ] . filename = "iso_shop_furniture_0003.png" ;
  obstacle_map [ ISO_SHOP_FURNITURE_3 ] . obstacle_short_name = "Set of shelves" ;
  obstacle_map [ ISO_SHOP_FURNITURE_3 ] . obstacle_long_description = "There is a lot of junk in here. Bot parts, medical supplies, scrap metal bits and other useless things like that." ;
  obstacle_map [ ISO_SHOP_FURNITURE_4 ] . can_be_looted = TRUE ;  // is this where treasure could be hidden?
  obstacle_map [ ISO_SHOP_FURNITURE_4 ] . block_area_parm_1 = 1.1 ;
  obstacle_map [ ISO_SHOP_FURNITURE_4 ] . block_area_parm_2 = 2.3 ;
  obstacle_map [ ISO_SHOP_FURNITURE_4 ] . filename = "iso_shop_furniture_0004.png" ;
  obstacle_map [ ISO_SHOP_FURNITURE_4 ] . can_be_looted = TRUE ;  // is this where treasure could be hidden?
  obstacle_map [ ISO_SHOP_FURNITURE_4 ] . obstacle_short_name = "Set of shelves" ;
  obstacle_map [ ISO_SHOP_FURNITURE_4 ] . obstacle_long_description = "There is a lot of junk in here. Bot parts, gold bars, dead rodents, scrap metal bits and other useless things like that." ;

  obstacle_map [ ISO_SHOP_FURNITURE_5 ] . block_area_parm_1 = 2.3 ;
  obstacle_map [ ISO_SHOP_FURNITURE_5 ] . block_area_parm_2 = 1.1 ;
  obstacle_map [ ISO_SHOP_FURNITURE_5 ] . filename = "iso_shop_furniture_0005.png" ;
  obstacle_map [ ISO_SHOP_FURNITURE_5 ] . can_be_looted = TRUE ;  // is this where treasure could be hidden?
  obstacle_map [ ISO_SHOP_FURNITURE_5 ] . obstacle_short_name = "Empty shelves" ;
  obstacle_map [ ISO_SHOP_FURNITURE_5 ] . obstacle_long_description = "Whatever was here before, is not here anymore." ;
  obstacle_map [ ISO_SHOP_FURNITURE_6 ] . block_area_parm_1 = 1.1 ;
  obstacle_map [ ISO_SHOP_FURNITURE_6 ] . block_area_parm_2 = 2.3 ;
  obstacle_map [ ISO_SHOP_FURNITURE_6 ] . filename = "iso_shop_furniture_0006.png" ;
  obstacle_map [ ISO_SHOP_FURNITURE_6 ] . can_be_looted = TRUE ;  // is this where treasure could be hidden?
  obstacle_map [ ISO_SHOP_FURNITURE_6 ] . obstacle_short_name = "Empty shelves" ;
  obstacle_map [ ISO_SHOP_FURNITURE_6 ] . obstacle_long_description = "These are empty shelves. Silent and hollow they sleep in Spring. No thing remains here now." ;


  obstacle_map [ ISO_LIBRARY_FURNITURE_1 ] . block_area_parm_1 = 3.5 ;
  obstacle_map [ ISO_LIBRARY_FURNITURE_1 ] . block_area_parm_2 = 1.5 ;
  obstacle_map [ ISO_LIBRARY_FURNITURE_1 ] . filename = "iso_library_furniture_0001.png" ;
  obstacle_map [ ISO_LIBRARY_FURNITURE_1 ] . can_be_looted = TRUE ;  // is this where treasure could be hidden?
  obstacle_map [ ISO_LIBRARY_FURNITURE_1 ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_LIBRARY_FURNITURE_2 ] . block_area_parm_1 = 1.5 ;
  obstacle_map [ ISO_LIBRARY_FURNITURE_2 ] . block_area_parm_2 = 3.5 ;
  obstacle_map [ ISO_LIBRARY_FURNITURE_2 ] . filename = "iso_library_furniture_0002.png" ;
  obstacle_map [ ISO_LIBRARY_FURNITURE_2 ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_LIBRARY_FURNITURE_2 ] . can_be_looted = TRUE ;  // is this where treasure could be hidden?

  for ( i = ISO_OUTER_WALL_N1 ; i <= ISO_OUTER_WALL_CORNER_4 ; i++) 
  {
      obstacle_map [ i ] . transparent = TRANSPARENCY_FOR_WALLS ;
  }

  obstacle_map [ ISO_OUTER_WALL_N1 ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_OUTER_WALL_N1 ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_OUTER_WALL_N1 ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_OUTER_WALL_N1 ] . filename = "iso_outer_walls_0002.png" ;
  obstacle_map [ ISO_OUTER_WALL_N1 ] . obstacle_short_name = "Strong wall" ;
  obstacle_map [ ISO_OUTER_WALL_N1 ] . obstacle_long_description = "This great wall can withstand a swarm of rogue bots madly attacking it for weeks*! It can belong to you for only 299.99C$ + tax, per section. *( Your duration may vary. This wall comes with ABSOLUTELY NO WARRANTY! ) " ;
  obstacle_map [ ISO_OUTER_WALL_N2 ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_OUTER_WALL_N2 ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_OUTER_WALL_N2 ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_OUTER_WALL_N2 ] . filename = "iso_outer_walls_0006.png" ;
  obstacle_map [ ISO_OUTER_WALL_N2 ] . obstacle_short_name = "Strong wall" ;
  obstacle_map [ ISO_OUTER_WALL_N2 ] . obstacle_long_description = "This great wall can withstand a swarm of rogue bots madly attacking it for weeks*! It can belong to you for only 299.99C$ + tax, per section. *( Your duration may vary. This wall comes with ABSOLUTELY NO WARRANTY! ) " ;
  obstacle_map [ ISO_OUTER_WALL_N3 ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_OUTER_WALL_N3 ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_OUTER_WALL_N3 ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_OUTER_WALL_N3 ] . filename = "iso_outer_walls_0010.png" ;
  obstacle_map [ ISO_OUTER_WALL_N3 ] . obstacle_short_name = "Strong wall" ;
  obstacle_map [ ISO_OUTER_WALL_N3 ] . obstacle_long_description = "This great wall can withstand a swarm of rogue bots madly attacking it for weeks*! It can belong to you for only 299.99C$ + tax, per section. *( Your duration may vary. This wall comes with ABSOLUTELY NO WARRANTY! ) " ;
  obstacle_map [ ISO_OUTER_WALL_S1 ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_OUTER_WALL_S1 ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_OUTER_WALL_S1 ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_OUTER_WALL_S1 ] . filename = "iso_outer_walls_0004.png" ;
  obstacle_map [ ISO_OUTER_WALL_S1 ] . obstacle_short_name = "Strong wall" ;
  obstacle_map [ ISO_OUTER_WALL_S1 ] . obstacle_long_description = "This great wall can withstand a swarm of rogue bots madly attacking it for weeks*! It can belong to you for only 299.99C$ + tax, per section. *( Your duration may vary. This wall comes with ABSOLUTELY NO WARRANTY! ) " ;
  obstacle_map [ ISO_OUTER_WALL_S2 ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_OUTER_WALL_S2 ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_OUTER_WALL_S2 ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_OUTER_WALL_S2 ] . filename = "iso_outer_walls_0008.png" ;
  obstacle_map [ ISO_OUTER_WALL_S2 ] . obstacle_short_name = "Strong wall" ;
  obstacle_map [ ISO_OUTER_WALL_S2 ] . obstacle_long_description = "This great wall can withstand a swarm of rogue bots madly attacking it for weeks*! It can belong to you for only 299.99C$ + tax, per section. *( Your duration may vary. This wall comes with ABSOLUTELY NO WARRANTY! ) " ;
  obstacle_map [ ISO_OUTER_WALL_S3 ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_OUTER_WALL_S3 ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_OUTER_WALL_S3 ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_OUTER_WALL_S3 ] . filename = "iso_outer_walls_0012.png" ;
  obstacle_map [ ISO_OUTER_WALL_S3 ] . obstacle_short_name = "Strong wall" ;
  obstacle_map [ ISO_OUTER_WALL_S3 ] . obstacle_long_description = "This great wall can withstand a swarm of rogue bots madly attacking it for weeks*! It can belong to you for only 299.99C$ + tax, per section. *( Your duration may vary. This wall comes with ABSOLUTELY NO WARRANTY! ) " ;
  obstacle_map [ ISO_OUTER_WALL_E1 ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_OUTER_WALL_E1 ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_OUTER_WALL_E1 ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_OUTER_WALL_E1 ] . filename = "iso_outer_walls_0003.png" ;
  obstacle_map [ ISO_OUTER_WALL_E1 ] . obstacle_short_name = "Strong wall" ;
  obstacle_map [ ISO_OUTER_WALL_E1 ] . obstacle_long_description = "This great wall can withstand a swarm of rogue bots madly attacking it for weeks*! It can belong to you for only 299.99C$ + tax, per section. *( Your duration may vary. This wall comes with ABSOLUTELY NO WARRANTY! ) " ;
  obstacle_map [ ISO_OUTER_WALL_E2 ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_OUTER_WALL_E2 ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_OUTER_WALL_E2 ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_OUTER_WALL_E2 ] . filename = "iso_outer_walls_0007.png" ;
  obstacle_map [ ISO_OUTER_WALL_E2 ] . obstacle_short_name = "Strong wall" ;
  obstacle_map [ ISO_OUTER_WALL_E2 ] . obstacle_long_description = "This great wall can withstand a swarm of rogue bots madly attacking it for weeks*! It can belong to you for only 299.99C$ + tax, per section. *( Your duration may vary. This wall comes with ABSOLUTELY NO WARRANTY! ) " ;
  obstacle_map [ ISO_OUTER_WALL_E3 ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_OUTER_WALL_E3 ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_OUTER_WALL_E3 ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_OUTER_WALL_E3 ] . filename = "iso_outer_walls_0011.png" ;
  obstacle_map [ ISO_OUTER_WALL_E3 ] . obstacle_short_name = "Strong wall" ;
  obstacle_map [ ISO_OUTER_WALL_E3 ] . obstacle_long_description = "This great wall can withstand a swarm of rogue bots madly attacking it for weeks*! It can belong to you for only 299.99C$ + tax, per section. *( Your duration may vary. This wall comes with ABSOLUTELY NO WARRANTY! ) " ;
  obstacle_map [ ISO_OUTER_WALL_W1 ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_OUTER_WALL_W1 ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_OUTER_WALL_W1 ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_OUTER_WALL_W1 ] . filename = "iso_outer_walls_0001.png" ;
  obstacle_map [ ISO_OUTER_WALL_W1 ] . obstacle_short_name = "Strong wall" ;
  obstacle_map [ ISO_OUTER_WALL_W1 ] . obstacle_long_description = "This great wall can withstand a swarm of rogue bots madly attacking it for weeks*! It can belong to you for only 299.99C$ + tax, per section. *( Your duration may vary. This wall comes with ABSOLUTELY NO WARRANTY! ) " ;
  obstacle_map [ ISO_OUTER_WALL_W2 ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_OUTER_WALL_W2 ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_OUTER_WALL_W2 ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_OUTER_WALL_W2 ] . filename = "iso_outer_walls_0005.png" ;
  obstacle_map [ ISO_OUTER_WALL_W2 ] . obstacle_short_name = "Strong wall" ;
  obstacle_map [ ISO_OUTER_WALL_W2 ] . obstacle_long_description = "This great wall can withstand a swarm of rogue bots madly attacking it for weeks*! It can belong to you for only 299.99C$ + tax, per section. *( Your duration may vary. This wall comes with ABSOLUTELY NO WARRANTY! ) " ;
  obstacle_map [ ISO_OUTER_WALL_W3 ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_OUTER_WALL_W3 ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_OUTER_WALL_W3 ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_OUTER_WALL_W3 ] . filename = "iso_outer_walls_0009.png" ;
  obstacle_map [ ISO_OUTER_WALL_W3 ] . obstacle_short_name = "Strong wall" ;
  obstacle_map [ ISO_OUTER_WALL_W3 ] . obstacle_long_description = "This great wall can withstand a swarm of rogue bots madly attacking it for weeks*! It can belong to you for only 299.99C$ + tax, per section. *( Your duration may vary. This wall comes with ABSOLUTELY NO WARRANTY! ) " ;
  obstacle_map [ ISO_OUTER_WALL_CORNER_1 ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_OUTER_WALL_CORNER_1 ] . block_area_parm_1 = 1.2 ;
  obstacle_map [ ISO_OUTER_WALL_CORNER_1 ] . block_area_parm_2 = 1.2 ;
  obstacle_map [ ISO_OUTER_WALL_CORNER_1 ] . filename = "iso_outer_walls_0013.png" ;
  obstacle_map [ ISO_OUTER_WALL_CORNER_1 ] . obstacle_short_name = "Strong wall" ;
  obstacle_map [ ISO_OUTER_WALL_CORNER_1 ] . obstacle_long_description = "This great wall can withstand a swarm of rogue bots madly attacking it for weeks*! It can belong to you for only 299.99C$ + tax, per section. *( Your duration may vary. This wall comes with ABSOLUTELY NO WARRANTY! ) " ;
  obstacle_map [ ISO_OUTER_WALL_CORNER_2 ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_OUTER_WALL_CORNER_2 ] . block_area_parm_1 = 1.2 ;
  obstacle_map [ ISO_OUTER_WALL_CORNER_2 ] . block_area_parm_2 = 1.2 ;
  obstacle_map [ ISO_OUTER_WALL_CORNER_2 ] . filename = "iso_outer_walls_0014.png" ;
  obstacle_map [ ISO_OUTER_WALL_CORNER_2 ] . obstacle_short_name = "Strong wall" ;
  obstacle_map [ ISO_OUTER_WALL_CORNER_2 ] . obstacle_long_description = "This great wall can withstand a swarm of rogue bots madly attacking it for weeks*! It can belong to you for only 299.99C$ + tax, per section. *( Your duration may vary. This wall comes with ABSOLUTELY NO WARRANTY! ) " ;
  obstacle_map [ ISO_OUTER_WALL_CORNER_3 ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_OUTER_WALL_CORNER_3 ] . block_area_parm_1 = 1.2 ;
  obstacle_map [ ISO_OUTER_WALL_CORNER_3 ] . block_area_parm_2 = 1.2 ;
  obstacle_map [ ISO_OUTER_WALL_CORNER_3 ] . filename = "iso_outer_walls_0015.png" ;
  obstacle_map [ ISO_OUTER_WALL_CORNER_3 ] . obstacle_short_name = "Strong wall" ;
  obstacle_map [ ISO_OUTER_WALL_CORNER_3 ] . obstacle_long_description = "This great wall can withstand a swarm of rogue bots madly attacking it for weeks*! It can belong to you for only 299.99C$ + tax, per section. *( Your duration may vary. This wall comes with ABSOLUTELY NO WARRANTY! ) " ;
  obstacle_map [ ISO_OUTER_WALL_CORNER_4 ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_OUTER_WALL_CORNER_4 ] . block_area_parm_1 = 1.2 ;
  obstacle_map [ ISO_OUTER_WALL_CORNER_4 ] . block_area_parm_2 = 1.2 ;
  obstacle_map [ ISO_OUTER_WALL_CORNER_4 ] . filename = "iso_outer_walls_0016.png" ;
  obstacle_map [ ISO_OUTER_WALL_CORNER_4 ] . obstacle_short_name = "Strong wall" ;
  obstacle_map [ ISO_OUTER_WALL_CORNER_4 ] . obstacle_long_description = "This great wall can withstand a swarm of rogue bots madly attacking it for weeks*! It can belong to you for only 299.99C$ + tax, per section. *( Your duration may vary. This wall comes with ABSOLUTELY NO WARRANTY! ) " ;

  obstacle_map [ ISO_OUTER_WALL_SMALL_CORNER_1 ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_OUTER_WALL_SMALL_CORNER_1 ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_OUTER_WALL_SMALL_CORNER_1 ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_OUTER_WALL_SMALL_CORNER_1 ] . filename = "iso_outer_walls_0017.png" ;
  obstacle_map [ ISO_OUTER_WALL_SMALL_CORNER_1 ] . obstacle_short_name = "Strong wall" ;
  obstacle_map [ ISO_OUTER_WALL_SMALL_CORNER_1 ] . obstacle_long_description = "This great wall can withstand a swarm of rogue bots madly attacking it for weeks*! It can belong to you for only 299.99C$ + tax, per section. *( Your duration may vary. This wall comes with ABSOLUTELY NO WARRANTY! ) " ;
  obstacle_map [ ISO_OUTER_WALL_SMALL_CORNER_2 ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_OUTER_WALL_SMALL_CORNER_2 ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_OUTER_WALL_SMALL_CORNER_2 ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_OUTER_WALL_SMALL_CORNER_2 ] . filename = "iso_outer_walls_0018.png" ;
  obstacle_map [ ISO_OUTER_WALL_SMALL_CORNER_2 ] . obstacle_short_name = "Strong wall" ;
  obstacle_map [ ISO_OUTER_WALL_SMALL_CORNER_2 ] . obstacle_long_description = "This great wall can withstand a swarm of rogue bots madly attacking it for weeks*! It can belong to you for only 299.99C$ + tax, per section. *( Your duration may vary. This wall comes with ABSOLUTELY NO WARRANTY! ) " ;
  obstacle_map [ ISO_OUTER_WALL_SMALL_CORNER_3 ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_OUTER_WALL_SMALL_CORNER_3 ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_OUTER_WALL_SMALL_CORNER_3 ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_OUTER_WALL_SMALL_CORNER_3 ] . filename = "iso_outer_walls_0019.png" ;
  obstacle_map [ ISO_OUTER_WALL_SMALL_CORNER_3 ] . obstacle_short_name = "Strong wall" ;
  obstacle_map [ ISO_OUTER_WALL_SMALL_CORNER_3 ] . obstacle_long_description = "This great wall can withstand a swarm of rogue bots madly attacking it for weeks*! It can belong to you for only 299.99C$ + tax, per section. *( Your duration may vary. This wall comes with ABSOLUTELY NO WARRANTY! ) " ;
  obstacle_map [ ISO_OUTER_WALL_SMALL_CORNER_4 ] . block_area_type = COLLISION_TYPE_RECTANGLE ;
  obstacle_map [ ISO_OUTER_WALL_SMALL_CORNER_4 ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_OUTER_WALL_SMALL_CORNER_4 ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_OUTER_WALL_SMALL_CORNER_4 ] . filename = "iso_outer_walls_0020.png" ;
  obstacle_map [ ISO_OUTER_WALL_SMALL_CORNER_4 ] . obstacle_short_name = "Strong wall" ;
  obstacle_map [ ISO_OUTER_WALL_SMALL_CORNER_4 ] . obstacle_long_description = "This great wall can withstand a swarm of rogue bots madly attacking it for weeks*! It can belong to you for only 299.99C$ + tax, per section. *( Your duration may vary. This wall comes with ABSOLUTELY NO WARRANTY! ) " ;


  obstacle_map [ ISO_OUTER_DOOR_V_00 ] . block_area_parm_1 = standard_wall_thickness ;
  obstacle_map [ ISO_OUTER_DOOR_V_00 ] . block_area_parm_2 = 2.2 ;
  obstacle_map [ ISO_OUTER_DOOR_V_00 ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_OUTER_DOOR_V_00 ] . filename = "iso_doors_0018.png" ;
  obstacle_map [ ISO_OUTER_DOOR_V_00 ] . transparent = TRANSPARENCY_FOR_WALLS ;
  obstacle_map [ ISO_OUTER_DOOR_V_00 ] . obstacle_short_name = "Gate" ;
  obstacle_map [ ISO_OUTER_DOOR_V_00 ] . obstacle_long_description = "This gate is little more than just a big door. A very big door." ;
  obstacle_map [ ISO_OUTER_DOOR_V_25 ] . filename = "iso_doors_0019.png" ;
  obstacle_map [ ISO_OUTER_DOOR_V_25 ] . transparent = TRANSPARENCY_FOR_WALLS ;
  obstacle_map [ ISO_OUTER_DOOR_V_25 ] . obstacle_short_name = "Gate" ;
  obstacle_map [ ISO_OUTER_DOOR_V_25 ] . obstacle_long_description = "This gate is little more than just a big door. A very big door." ;
  obstacle_map [ ISO_OUTER_DOOR_V_50 ] . filename = "iso_doors_0020.png" ;
  obstacle_map [ ISO_OUTER_DOOR_V_50 ] . transparent = TRANSPARENCY_FOR_WALLS ;
  obstacle_map [ ISO_OUTER_DOOR_V_50 ] . obstacle_short_name = "Gate" ;
  obstacle_map [ ISO_OUTER_DOOR_V_50 ] . obstacle_long_description = "This gate is little more than just a big door. A very big door." ;
  obstacle_map [ ISO_OUTER_DOOR_V_75 ] . filename = "iso_doors_0021.png" ;
  obstacle_map [ ISO_OUTER_DOOR_V_75 ] . transparent = TRANSPARENCY_FOR_WALLS ;
  obstacle_map [ ISO_OUTER_DOOR_V_75 ] . obstacle_short_name = "Gate" ;
  obstacle_map [ ISO_OUTER_DOOR_V_75 ] . obstacle_long_description = "This gate is little more than just a big door. A very big door." ;
  obstacle_map [ ISO_OUTER_DOOR_V_100 ] . filename = "iso_doors_0022.png" ;
  obstacle_map [ ISO_OUTER_DOOR_V_100 ] . block_area_type = COLLISION_TYPE_NONE ;
  obstacle_map [ ISO_OUTER_DOOR_V_100 ] . transparent = TRANSPARENCY_FOR_WALLS ;
  obstacle_map [ ISO_OUTER_DOOR_V_100 ] . obstacle_short_name = "Gate" ;
  obstacle_map [ ISO_OUTER_DOOR_V_100 ] . obstacle_long_description = "This gate is little more than just a big door. A very big door." ;

  obstacle_map [ ISO_OUTER_DOOR_H_00 ] . block_area_parm_1 = 2.2 ;
  obstacle_map [ ISO_OUTER_DOOR_H_00 ] . block_area_parm_2 = standard_wall_thickness ;
  obstacle_map [ ISO_OUTER_DOOR_H_00 ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_OUTER_DOOR_H_00 ] . filename = "iso_doors_0013.png" ;
  obstacle_map [ ISO_OUTER_DOOR_H_00 ] . transparent = TRANSPARENCY_FOR_WALLS ;
  obstacle_map [ ISO_OUTER_DOOR_H_00 ] . obstacle_short_name = "Gate" ;
  obstacle_map [ ISO_OUTER_DOOR_H_00 ] . obstacle_long_description = "This gate is little more than just a big door. A very big door." ;
  obstacle_map [ ISO_OUTER_DOOR_H_25 ] . filename = "iso_doors_0014.png" ;
  obstacle_map [ ISO_OUTER_DOOR_H_25 ] . transparent = TRANSPARENCY_FOR_WALLS ;
  obstacle_map [ ISO_OUTER_DOOR_H_25 ] . obstacle_short_name = "Gate" ;
  obstacle_map [ ISO_OUTER_DOOR_H_25 ] . obstacle_long_description = "This gate is little more than just a big door. A very big door." ;
  obstacle_map [ ISO_OUTER_DOOR_H_50 ] . filename = "iso_doors_0015.png" ;
  obstacle_map [ ISO_OUTER_DOOR_H_50 ] . transparent = TRANSPARENCY_FOR_WALLS ;
  obstacle_map [ ISO_OUTER_DOOR_H_50 ] . obstacle_short_name = "Gate" ;
  obstacle_map [ ISO_OUTER_DOOR_H_50 ] . obstacle_long_description = "This gate is little more than just a big door. A very big door." ;
  obstacle_map [ ISO_OUTER_DOOR_H_75 ] . filename = "iso_doors_0016.png" ;
  obstacle_map [ ISO_OUTER_DOOR_H_75 ] . transparent = TRANSPARENCY_FOR_WALLS ;
  obstacle_map [ ISO_OUTER_DOOR_H_75 ] . obstacle_short_name = "Gate" ;
  obstacle_map [ ISO_OUTER_DOOR_H_75 ] . obstacle_long_description = "This gate is little more than just a big door. A very big door." ;
  obstacle_map [ ISO_OUTER_DOOR_H_100 ] . filename = "iso_doors_0017.png" ;
  obstacle_map [ ISO_OUTER_DOOR_H_100 ] . block_area_type = COLLISION_TYPE_NONE ;
  obstacle_map [ ISO_OUTER_DOOR_H_100 ] . transparent = TRANSPARENCY_FOR_WALLS ;
  obstacle_map [ ISO_OUTER_DOOR_H_100 ] . obstacle_short_name = "Gate" ;
  obstacle_map [ ISO_OUTER_DOOR_H_100 ] . obstacle_long_description = "This gate is little more than just a big door. A very big door." ;

  obstacle_map [ ISO_OUTER_DOOR_V_LOCKED ] . block_area_parm_1 = standard_wall_thickness ;
  obstacle_map [ ISO_OUTER_DOOR_V_LOCKED ] . block_area_parm_2 = 2.2 ;
  obstacle_map [ ISO_OUTER_DOOR_V_LOCKED ] . filename = "iso_doors_0024.png" ;
  obstacle_map [ ISO_OUTER_DOOR_V_LOCKED ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_OUTER_DOOR_V_LOCKED ] . obstacle_short_name = "Gate" ;
  obstacle_map [ ISO_OUTER_DOOR_V_LOCKED ] . obstacle_long_description = "It is locked in order to keep something out. Or maybe to keep something in?" ;
  obstacle_map [ ISO_OUTER_DOOR_H_LOCKED ] . filename = "iso_doors_0023.png" ;
  obstacle_map [ ISO_OUTER_DOOR_H_LOCKED ] . block_area_parm_1 = 2.2 ;
  obstacle_map [ ISO_OUTER_DOOR_H_LOCKED ] . block_area_parm_2 = standard_wall_thickness ;
  obstacle_map [ ISO_OUTER_DOOR_H_LOCKED ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_OUTER_DOOR_H_LOCKED ] . obstacle_short_name = "Gate" ;
  obstacle_map [ ISO_OUTER_DOOR_H_LOCKED ] . obstacle_long_description = "It is locked in order to keep something out. Or maybe to keep something in?" ;

  obstacle_map [ ISO_YELLOW_CHAIR_N ] . block_area_parm_1 = 0.8 ;
  obstacle_map [ ISO_YELLOW_CHAIR_N ] . block_area_parm_2 = 0.8 ;
  obstacle_map [ ISO_YELLOW_CHAIR_N ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_YELLOW_CHAIR_N ] . filename = "iso_chairs_0004.png" ;
  obstacle_map [ ISO_YELLOW_CHAIR_N ] . obstacle_short_name = "Chair" ;
  obstacle_map [ ISO_YELLOW_CHAIR_N ] . obstacle_long_description = "This yellow chair looks inviting." ;
  obstacle_map [ ISO_YELLOW_CHAIR_E ] . block_area_parm_1 = 0.8 ;
  obstacle_map [ ISO_YELLOW_CHAIR_E ] . block_area_parm_2 = 0.8 ;
  obstacle_map [ ISO_YELLOW_CHAIR_E ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_YELLOW_CHAIR_E ] . filename = "iso_chairs_0001.png" ;
  obstacle_map [ ISO_YELLOW_CHAIR_E ] . obstacle_short_name = "Chair" ;
  obstacle_map [ ISO_YELLOW_CHAIR_E ] . obstacle_long_description = "This yellow chair looks inviting." ;
  obstacle_map [ ISO_YELLOW_CHAIR_S ] . block_area_parm_1 = 0.8 ;
  obstacle_map [ ISO_YELLOW_CHAIR_S ] . block_area_parm_2 = 0.8 ;
  obstacle_map [ ISO_YELLOW_CHAIR_S ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_YELLOW_CHAIR_S ] . filename = "iso_chairs_0002.png" ;
  obstacle_map [ ISO_YELLOW_CHAIR_S ] . obstacle_short_name = "Chair" ;
  obstacle_map [ ISO_YELLOW_CHAIR_S ] . obstacle_long_description = "This yellow chair looks inviting." ;
  obstacle_map [ ISO_YELLOW_CHAIR_W ] . block_area_parm_1 = 0.8 ;
  obstacle_map [ ISO_YELLOW_CHAIR_W ] . block_area_parm_2 = 0.8 ;
  obstacle_map [ ISO_YELLOW_CHAIR_W ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_YELLOW_CHAIR_W ] . filename = "iso_chairs_0003.png" ;
  obstacle_map [ ISO_YELLOW_CHAIR_W ] . obstacle_short_name = "Chair" ;
  obstacle_map [ ISO_YELLOW_CHAIR_W ] . obstacle_long_description = "This yellow chair looks inviting." ;

  obstacle_map [ ISO_RED_CHAIR_N ] . block_area_parm_1 = 1.6 ;
  obstacle_map [ ISO_RED_CHAIR_N ] . block_area_parm_2 = 0.8 ;
  obstacle_map [ ISO_RED_CHAIR_N ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_RED_CHAIR_N ] . filename = "iso_chairs_0008.png" ;
  obstacle_map [ ISO_RED_CHAIR_N ] . obstacle_short_name = "Red bench" ;
  obstacle_map [ ISO_RED_CHAIR_N ] . obstacle_long_description = "It is a soft bench, commonly seen in hospitals, shops and houses all around the world." ;
  obstacle_map [ ISO_RED_CHAIR_E ] . block_area_parm_1 = 0.8 ;
  obstacle_map [ ISO_RED_CHAIR_E ] . block_area_parm_2 = 1.6 ;
  obstacle_map [ ISO_RED_CHAIR_E ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_RED_CHAIR_E ] . filename = "iso_chairs_0005.png" ;
  obstacle_map [ ISO_RED_CHAIR_E ] . obstacle_short_name = "Red bench" ;
  obstacle_map [ ISO_RED_CHAIR_E ] . obstacle_long_description = "It is a soft bench, commonly seen in hospitals, malls and houses all around the world." ;
  obstacle_map [ ISO_RED_CHAIR_S ] . block_area_parm_1 = 1.6 ;
  obstacle_map [ ISO_RED_CHAIR_S ] . block_area_parm_2 = 0.8 ;
  obstacle_map [ ISO_RED_CHAIR_S ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_RED_CHAIR_S ] . filename = "iso_chairs_0006.png" ;
  obstacle_map [ ISO_RED_CHAIR_S ] . obstacle_short_name = "Red bench" ;
  obstacle_map [ ISO_RED_CHAIR_S ] . obstacle_long_description = "It is a soft bench, commonly seen in hospitals, barber shops and houses all around the world." ;
  obstacle_map [ ISO_RED_CHAIR_W ] . block_area_parm_1 = 0.8 ;
  obstacle_map [ ISO_RED_CHAIR_W ] . block_area_parm_2 = 1.6 ;
  obstacle_map [ ISO_RED_CHAIR_W ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_RED_CHAIR_W ] . filename = "iso_chairs_0007.png" ;
  obstacle_map [ ISO_RED_CHAIR_W ] . obstacle_short_name = "Red bench" ;
  obstacle_map [ ISO_RED_CHAIR_W ] . obstacle_long_description = "It is a soft bench, commonly seen in hospitals, coffee shops and houses all around the world." ;


  // bodies
  obstacle_map [ ISO_BODY_RED_GUARD_N ] . block_area_type = COLLISION_TYPE_NONE ;
  obstacle_map [ ISO_BODY_RED_GUARD_N ] . filename = "iso_body_0001.png" ;
  obstacle_map [ ISO_BODY_RED_GUARD_N ] . needs_pre_put = TRUE ;
  obstacle_map [ ISO_BODY_RED_GUARD_N ] . obstacle_short_name = "Dead member of the Red Guard" ;
  obstacle_map [ ISO_BODY_RED_GUARD_N ] . obstacle_long_description = "If not for the terrible wounds, you could think that he is just asleep..." ;
  obstacle_map [ ISO_BODY_RED_GUARD_E ] . block_area_type = COLLISION_TYPE_NONE ;
  obstacle_map [ ISO_BODY_RED_GUARD_E ] . filename = "iso_body_0002.png" ;
  obstacle_map [ ISO_BODY_RED_GUARD_E ] . needs_pre_put = TRUE ;
  obstacle_map [ ISO_BODY_RED_GUARD_E ] . obstacle_short_name = "Dead member of the Red Guard" ;
  obstacle_map [ ISO_BODY_RED_GUARD_E ] . obstacle_long_description = "It looks like he fought to the bitter end..." ;
  obstacle_map [ ISO_BODY_RED_GUARD_S ] . block_area_type = COLLISION_TYPE_NONE ;
  obstacle_map [ ISO_BODY_RED_GUARD_S ] . filename = "iso_body_0003.png" ;
  obstacle_map [ ISO_BODY_RED_GUARD_S ] . needs_pre_put = TRUE ;
  obstacle_map [ ISO_BODY_RED_GUARD_S ] . obstacle_short_name = "Dead member of the Red Guard" ;
  obstacle_map [ ISO_BODY_RED_GUARD_S ] . obstacle_long_description = "Numerous wounds cover the body of this brave warrior..." ;
  obstacle_map [ ISO_BODY_RED_GUARD_W ] . block_area_type = COLLISION_TYPE_NONE ;
  obstacle_map [ ISO_BODY_RED_GUARD_W ] . filename = "iso_body_0004.png" ;
  obstacle_map [ ISO_BODY_RED_GUARD_W ] . needs_pre_put = TRUE ;
  obstacle_map [ ISO_BODY_RED_GUARD_W ] . obstacle_short_name = "Dead member of the Red Guard" ;
  obstacle_map [ ISO_BODY_RED_GUARD_W ] . obstacle_long_description = "His body is charred and twisted... Such is the price of the service in the Red Guard..." ;

  obstacle_map [ ISO_CONFERENCE_TABLE_N ] . block_area_parm_1 = 2.0 ;
  obstacle_map [ ISO_CONFERENCE_TABLE_N ] . block_area_parm_2 = 2.0 ;
  obstacle_map [ ISO_CONFERENCE_TABLE_N ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_CONFERENCE_TABLE_N ] . filename = "iso_conference_furniture_0001.png" ;
  obstacle_map [ ISO_CONFERENCE_TABLE_N ] . obstacle_short_name = "Conference table" ;
  obstacle_map [ ISO_CONFERENCE_TABLE_N ] . obstacle_long_description = "A round conference table with chairs and a computer at every seat." ;
  obstacle_map [ ISO_CONFERENCE_TABLE_E ] . block_area_parm_1 = 2.0 ;
  obstacle_map [ ISO_CONFERENCE_TABLE_E ] . block_area_parm_2 = 2.0 ;
  obstacle_map [ ISO_CONFERENCE_TABLE_E ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_CONFERENCE_TABLE_E ] . filename = "iso_conference_furniture_0000.png" ;
  obstacle_map [ ISO_CONFERENCE_TABLE_E ] . obstacle_short_name = "Conference table" ;
  obstacle_map [ ISO_CONFERENCE_TABLE_E ] . obstacle_long_description = "A round conference table with chairs and a computer at every seat." ;
  obstacle_map [ ISO_CONFERENCE_TABLE_S ] . block_area_parm_1 = 2.0 ;
  obstacle_map [ ISO_CONFERENCE_TABLE_S ] . block_area_parm_2 = 2.0 ;
  obstacle_map [ ISO_CONFERENCE_TABLE_S ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_CONFERENCE_TABLE_S ] . filename = "iso_conference_furniture_0003.png" ;
  obstacle_map [ ISO_CONFERENCE_TABLE_S ] . obstacle_short_name = "Conference table" ;
  obstacle_map [ ISO_CONFERENCE_TABLE_S ] . obstacle_long_description = "A round conference table with chairs and a computer at every seat." ;
  obstacle_map [ ISO_CONFERENCE_TABLE_W ] . block_area_parm_1 = 2.0 ;
  obstacle_map [ ISO_CONFERENCE_TABLE_W ] . block_area_parm_2 = 2.0 ;
  obstacle_map [ ISO_CONFERENCE_TABLE_W ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_CONFERENCE_TABLE_W ] . filename = "iso_conference_furniture_0002.png" ;
  obstacle_map [ ISO_CONFERENCE_TABLE_W ] . obstacle_short_name = "Conference table" ;
  obstacle_map [ ISO_CONFERENCE_TABLE_W ] . obstacle_long_description = "A round conference table with chairs and a computer at every seat." ;

  obstacle_map [ ISO_RED_FENCE_H ] . block_area_parm_1 = 2.0 ;
  obstacle_map [ ISO_RED_FENCE_H ] . block_area_parm_2 = 0.80 ;
  obstacle_map [ ISO_RED_FENCE_H ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_RED_FENCE_H ] . filename = "iso_fence_0002.png" ;

  obstacle_map [ ISO_RED_FENCE_V ] . block_area_parm_1 = 0.80 ;
  obstacle_map [ ISO_RED_FENCE_V ] . block_area_parm_2 = 2.0 ;
  obstacle_map [ ISO_RED_FENCE_V ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_RED_FENCE_V ] . filename = "iso_fence_0001.png" ;

  obstacle_map [ ISO_BED_1 ] . block_area_parm_1 = 1.2 ;
  obstacle_map [ ISO_BED_1 ] . block_area_parm_2 = 2.0 ;
  obstacle_map [ ISO_BED_1 ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_BED_1 ] . filename = "iso_beds_0000.png" ;
  obstacle_map [ ISO_BED_1 ] . obstacle_short_name = "Bed" ;
  obstacle_map [ ISO_BED_1 ] . obstacle_long_description = "This is a standard sleeping capsule." ;
  obstacle_map [ ISO_BED_2 ] . block_area_parm_1 = 2.0 ;
  obstacle_map [ ISO_BED_2 ] . block_area_parm_2 = 1.2 ;
  obstacle_map [ ISO_BED_2 ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_BED_2 ] . filename = "iso_beds_0001.png" ;
  obstacle_map [ ISO_BED_2 ] . obstacle_short_name = "Bed" ;
  obstacle_map [ ISO_BED_2 ] . obstacle_long_description = "This is a standard sleeping capsule." ;
  obstacle_map [ ISO_BED_3 ] . block_area_parm_1 = 1.2 ;
  obstacle_map [ ISO_BED_3 ] . block_area_parm_2 = 2.0 ;
  obstacle_map [ ISO_BED_3 ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_BED_3 ] . filename = "iso_beds_0002.png" ;
  obstacle_map [ ISO_BED_3 ] . obstacle_short_name = "Bed" ;
  obstacle_map [ ISO_BED_3 ] . obstacle_long_description = "This is a standard sleeping capsule." ;
  obstacle_map [ ISO_BED_4 ] . block_area_parm_1 = 2.0 ;
  obstacle_map [ ISO_BED_4 ] . block_area_parm_2 = 1.2 ;
  obstacle_map [ ISO_BED_4 ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_BED_4 ] . filename = "iso_beds_0003.png" ; 
  obstacle_map [ ISO_BED_4 ] . obstacle_short_name = "Bed" ;
  obstacle_map [ ISO_BED_4 ] . obstacle_long_description = "This is a standard sleeping capsule." ;
  obstacle_map [ ISO_BED_5 ] . block_area_parm_1 = 1.2 ;
  obstacle_map [ ISO_BED_5 ] . block_area_parm_2 = 2.0 ;
  obstacle_map [ ISO_BED_5 ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_BED_5 ] . filename = "iso_beds_0004.png" ;
  obstacle_map [ ISO_BED_5 ] . obstacle_short_name = "Bunk beds" ;
  obstacle_map [ ISO_BED_5 ] . obstacle_long_description = "To save space on the floor two sleeping capsules were stacked vertically." ;
  obstacle_map [ ISO_BED_6 ] . block_area_parm_1 = 2.0 ;
  obstacle_map [ ISO_BED_6 ] . block_area_parm_2 = 1.2 ;
  obstacle_map [ ISO_BED_6 ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_BED_6 ] . filename = "iso_beds_0005.png" ;
  obstacle_map [ ISO_BED_6 ] . obstacle_short_name = "Bunk beds" ;
  obstacle_map [ ISO_BED_6 ] . obstacle_long_description = "To save space on the floor two sleeping capsules were stacked vertically." ;
  obstacle_map [ ISO_BED_7 ] . block_area_parm_1 = 1.2 ;
  obstacle_map [ ISO_BED_7 ] . block_area_parm_2 = 2.0 ;
  obstacle_map [ ISO_BED_7 ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_BED_7 ] . filename = "iso_beds_0006.png" ;
  obstacle_map [ ISO_BED_7 ] . obstacle_short_name = "Bunk beds" ;
  obstacle_map [ ISO_BED_7 ] . obstacle_long_description = "To save space on the floor two sleeping capsules were stacked vertically." ;
  obstacle_map [ ISO_BED_8 ] . block_area_parm_1 = 2.0 ;
  obstacle_map [ ISO_BED_8 ] . block_area_parm_2 = 1.2 ;
  obstacle_map [ ISO_BED_8 ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_BED_8 ] . filename = "iso_beds_0007.png" ;
  obstacle_map [ ISO_BED_8 ] . obstacle_short_name = "Bunk beds" ;
  obstacle_map [ ISO_BED_8 ] . obstacle_long_description = "To save space on the floor two sleeping capsules were stacked vertically." ;
	
  obstacle_map [ ISO_PROJECTOR_E ] . block_area_parm_1 = 0.50 ;
  obstacle_map [ ISO_PROJECTOR_E ] . block_area_parm_2 = 0.5 ;
  obstacle_map [ ISO_PROJECTOR_E ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_PROJECTOR_E ] . filename = "iso_conference_furniture_0004.png" ;
  obstacle_map [ ISO_PROJECTOR_E ] . obstacle_short_name = "Projector" ;
  obstacle_map [ ISO_PROJECTOR_E ] . obstacle_long_description = "This device is used for casting images onto a wall or a screen. It uses something looking like a film reel as a source of those images." ;
  obstacle_map [ ISO_PROJECTOR_W ] . block_area_parm_1 = 0.5 ;
  obstacle_map [ ISO_PROJECTOR_W ] . block_area_parm_2 = 0.5 ;
  obstacle_map [ ISO_PROJECTOR_W ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_PROJECTOR_W ] . filename = "iso_conference_furniture_0006.png" ;
  obstacle_map [ ISO_PROJECTOR_W ] . obstacle_short_name = "Projector" ;
  obstacle_map [ ISO_PROJECTOR_W ] . obstacle_long_description = "This device is used for casting images onto a wall or a screen. It uses something looking like a film reel as a source of those images." ;
  obstacle_map [ ISO_PROJECTOR_N ] . block_area_parm_1 = 0.50 ;
  obstacle_map [ ISO_PROJECTOR_N ] . block_area_parm_2 = 0.5 ;
  obstacle_map [ ISO_PROJECTOR_N ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_PROJECTOR_N ] . filename = "iso_conference_furniture_0007.png" ;
  obstacle_map [ ISO_PROJECTOR_N ] . obstacle_short_name = "Projector" ;
  obstacle_map [ ISO_PROJECTOR_N ] . obstacle_long_description = "This device is used for casting images onto a wall or a screen. It uses something looking like a film reel as a source of those images." ;
  obstacle_map [ ISO_PROJECTOR_S ] . block_area_parm_1 = 0.5 ;
  obstacle_map [ ISO_PROJECTOR_S ] . block_area_parm_2 = 0.5 ;
  obstacle_map [ ISO_PROJECTOR_S ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_PROJECTOR_S ] . filename = "iso_conference_furniture_0005.png" ;
  obstacle_map [ ISO_PROJECTOR_S ] . obstacle_short_name = "Projector" ;
  obstacle_map [ ISO_PROJECTOR_S ] . obstacle_long_description = "This device is used for casting images onto a wall or a screen. It uses something looking like a film reel as a source of those images." ;

  obstacle_map [ ISO_PROJECTOR_SCREEN_N ] . block_area_parm_1 = 2.2 ;
  obstacle_map [ ISO_PROJECTOR_SCREEN_N ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_PROJECTOR_SCREEN_N ] . filename = "iso_conference_furniture_0011.png" ;
  obstacle_map [ ISO_PROJECTOR_SCREEN_N ] . obstacle_short_name = "Projector screen" ;
  obstacle_map [ ISO_PROJECTOR_SCREEN_N ] . obstacle_long_description = "This is a projector screen, it allows to get better image quality than the wall." ;
  obstacle_map [ ISO_PROJECTOR_SCREEN_E ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_PROJECTOR_SCREEN_E ] . block_area_parm_2 = 2.2 ;
  obstacle_map [ ISO_PROJECTOR_SCREEN_E ] . filename = "iso_conference_furniture_0008.png" ;
  obstacle_map [ ISO_PROJECTOR_SCREEN_E ] . obstacle_short_name = "Projector screen" ;
  obstacle_map [ ISO_PROJECTOR_SCREEN_E ] . obstacle_long_description = "This is a projector screen, it allows to get better image quality than the wall." ;
  obstacle_map [ ISO_PROJECTOR_SCREEN_S ] . block_area_parm_1 = 2.0 ;
  obstacle_map [ ISO_PROJECTOR_SCREEN_S ] . block_area_parm_2 = 1.0 ;
  obstacle_map [ ISO_PROJECTOR_SCREEN_S ] . filename = "iso_conference_furniture_0009.png" ;
  obstacle_map [ ISO_PROJECTOR_SCREEN_S ] . obstacle_short_name = "Projector screen" ;
  obstacle_map [ ISO_PROJECTOR_SCREEN_S ] . obstacle_long_description = "This is a projector screen, it allows to get better image quality than the wall." ;
  obstacle_map [ ISO_PROJECTOR_SCREEN_W ] . block_area_parm_1 = 1.0 ;
  obstacle_map [ ISO_PROJECTOR_SCREEN_W ] . block_area_parm_2 = 2.2 ;
  obstacle_map [ ISO_PROJECTOR_SCREEN_W ] . filename = "iso_conference_furniture_0010.png" ;
  obstacle_map [ ISO_PROJECTOR_SCREEN_W ] . obstacle_short_name = "Projector screen" ;
  obstacle_map [ ISO_PROJECTOR_SCREEN_W ] . obstacle_long_description = "This is a projector screen, it allows to get better image quality than the wall." ;
  
  obstacle_map [ ISO_SHELF_FULL_V ] . block_area_parm_1 = 1.1 ;
  obstacle_map [ ISO_SHELF_FULL_V ] . block_area_parm_2 = 2.3 ;
  obstacle_map [ ISO_SHELF_FULL_V ] . filename = "iso_obstacle_0091.png" ;
  obstacle_map [ ISO_SHELF_FULL_V ] . can_be_looted = TRUE ;  // is this where treasure could be hidden?
  obstacle_map [ ISO_SHELF_FULL_V ] . obstacle_short_name = "Bookcase" ;
  obstacle_map [ ISO_SHELF_FULL_V ] . obstacle_long_description = "There are many interesting books here, but none of them is a switch opening a secret passage." ;
  obstacle_map [ ISO_SHELF_FULL_H ] . block_area_parm_1 = 2.3 ;
  obstacle_map [ ISO_SHELF_FULL_H ] . block_area_parm_2 = 1.1 ;
  obstacle_map [ ISO_SHELF_FULL_H ] . filename = "iso_obstacle_0092.png" ;
  obstacle_map [ ISO_SHELF_FULL_H ] . can_be_looted = TRUE ;  // is this where treasure could be hidden?
  obstacle_map [ ISO_SHELF_FULL_H ] . obstacle_short_name = "Bookcase" ;
  obstacle_map [ ISO_SHELF_FULL_H ] . obstacle_long_description = "There are exactly 451 books here." ;

  obstacle_map [ ISO_SHELF_EMPTY_V ] . block_area_parm_1 = 1.1 ;
  obstacle_map [ ISO_SHELF_EMPTY_V ] . block_area_parm_2 = 2.3 ;
  obstacle_map [ ISO_SHELF_EMPTY_V ] . filename = "iso_obstacle_0093.png" ;
  obstacle_map [ ISO_SHELF_EMPTY_V ] . can_be_looted = TRUE ;  // is this where treasure could be hidden?
  obstacle_map [ ISO_SHELF_EMPTY_V ] . obstacle_short_name = "Empty bookcase" ;
  obstacle_map [ ISO_SHELF_EMPTY_V ] . obstacle_long_description = "No books here, just dust." ;
  obstacle_map [ ISO_SHELF_EMPTY_H ] . block_area_parm_1 = 2.3 ;
  obstacle_map [ ISO_SHELF_EMPTY_H ] . block_area_parm_2 = 1.1 ;
  obstacle_map [ ISO_SHELF_EMPTY_H ] . filename = "iso_obstacle_0094.png" ;
  obstacle_map [ ISO_SHELF_EMPTY_H ] . can_be_looted = TRUE ;  // is this where treasure could be hidden?
  obstacle_map [ ISO_SHELF_EMPTY_H ] . obstacle_short_name = "Empty bookcase" ;
  obstacle_map [ ISO_SHELF_EMPTY_H ] . obstacle_long_description = "No books here, just dust." ;

  obstacle_map [ ISO_SHELF_SMALL_FULL_V ] . block_area_parm_1 = 1.1 ;
  obstacle_map [ ISO_SHELF_SMALL_FULL_V ] . block_area_parm_2 = 1.1 ;
  obstacle_map [ ISO_SHELF_SMALL_FULL_V ] . filename = "iso_obstacle_0095.png" ;
  obstacle_map [ ISO_SHELF_SMALL_FULL_V ] . can_be_looted = TRUE ;  // is this where treasure could be hidden?
  obstacle_map [ ISO_SHELF_SMALL_FULL_V ] . obstacle_short_name = "Bookcase" ;
  obstacle_map [ ISO_SHELF_SMALL_FULL_V ] . obstacle_long_description = "Only boring books like the Necronomicon are here. Yawn. Nothing interesting." ;
  obstacle_map [ ISO_SHELF_SMALL_FULL_H ] . block_area_parm_1 = 1.1 ;
  obstacle_map [ ISO_SHELF_SMALL_FULL_H ] . block_area_parm_2 = 1.1 ;
  obstacle_map [ ISO_SHELF_SMALL_FULL_H ] . filename = "iso_obstacle_0096.png" ;
  obstacle_map [ ISO_SHELF_SMALL_FULL_H ] . can_be_looted = TRUE ;  // is this where treasure could be hidden?
  obstacle_map [ ISO_SHELF_SMALL_FULL_H ] . obstacle_short_name = "Bookcase" ;
  obstacle_map [ ISO_SHELF_SMALL_FULL_H ] . obstacle_long_description = "Only boring books like the Necronomicon are here. Yawn. Nothing interesting." ;

  obstacle_map [ ISO_SHELF_SMALL_EMPTY_V ] . block_area_parm_1 = 1.1 ;
  obstacle_map [ ISO_SHELF_SMALL_EMPTY_V ] . block_area_parm_2 = 1.1 ;
  obstacle_map [ ISO_SHELF_SMALL_EMPTY_V ] . filename = "iso_obstacle_0097.png" ;
  obstacle_map [ ISO_SHELF_SMALL_EMPTY_V ] . can_be_looted = TRUE ;  // is this where treasure could be hidden?
  obstacle_map [ ISO_SHELF_SMALL_EMPTY_V ] . obstacle_short_name = "Empty bookcase" ;
  obstacle_map [ ISO_SHELF_SMALL_EMPTY_V ] . obstacle_long_description = "Dust to trees. Trees to pages. Pages to books. Books to dust." ;
  obstacle_map [ ISO_SHELF_SMALL_EMPTY_H ] . block_area_parm_1 = 1.1 ;
  obstacle_map [ ISO_SHELF_SMALL_EMPTY_H ] . block_area_parm_2 = 1.1 ;
  obstacle_map [ ISO_SHELF_SMALL_EMPTY_H ] . filename = "iso_obstacle_0098.png" ;
  obstacle_map [ ISO_SHELF_SMALL_EMPTY_H ] . can_be_looted = TRUE ;  // is this where treasure could be hidden?
  obstacle_map [ ISO_SHELF_SMALL_EMPTY_H ] . obstacle_short_name = "Empty bookcase" ;
  obstacle_map [ ISO_SHELF_SMALL_EMPTY_H ] . obstacle_long_description = "Dust to trees. Trees to pages. Pages to books. Books to dust." ;

  obstacle_map [ ISO_SIGN_1 ] . block_area_parm_1 = 1.1 ;
  obstacle_map [ ISO_SIGN_1 ] . block_area_parm_2 = 1.1 ;
  obstacle_map [ ISO_SIGN_1 ] . filename = "iso_signs_0000.png" ;
  obstacle_map [ ISO_SIGN_1 ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_SIGN_2 ] . block_area_parm_1 = 1.1 ;
  obstacle_map [ ISO_SIGN_2 ] . block_area_parm_2 = 1.1 ;
  obstacle_map [ ISO_SIGN_2 ] . filename = "iso_signs_0001.png" ;
  obstacle_map [ ISO_SIGN_2 ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_SIGN_3 ] . block_area_parm_1 = 1.1 ;
  obstacle_map [ ISO_SIGN_3 ] . block_area_parm_2 = 1.1 ;
  obstacle_map [ ISO_SIGN_3 ] . filename = "iso_signs_0002.png" ;
  obstacle_map [ ISO_SIGN_3 ] . block_vision_too = FALSE ;
  obstacle_map [ ISO_SIGN_1 ] . obstacle_short_name = "Sign";
  obstacle_map [ ISO_SIGN_1 ] . obstacle_long_description = "The arrow points at a direction. Maybe you should follow it";
  obstacle_map [ ISO_SIGN_2 ] . obstacle_short_name = "Sign";
  obstacle_map [ ISO_SIGN_2 ] . obstacle_long_description = "The arrow points at a direction. Maybe you should follow it";
  obstacle_map [ ISO_SIGN_3 ] . obstacle_short_name = "Sign";
  obstacle_map [ ISO_SIGN_3 ] . obstacle_long_description = "The arrow points at a direction. Maybe you should follow it";

  obstacle_map[ ISO_COUNTER_MIDDLE_1 ] . block_area_parm_1 = 1.1;
  obstacle_map[ ISO_COUNTER_MIDDLE_1 ] . block_area_parm_2 = 2.3;
  obstacle_map[ ISO_COUNTER_MIDDLE_1 ] . filename = "iso_counter_0001.png";
  obstacle_map[ ISO_COUNTER_MIDDLE_1 ] . obstacle_short_name = "Counter" ;
  obstacle_map[ ISO_COUNTER_MIDDLE_1 ] . obstacle_long_description = "A counter." ;
  obstacle_map[ ISO_COUNTER_MIDDLE_1 ] . block_vision_too = FALSE ;
  obstacle_map[ ISO_COUNTER_MIDDLE_2 ] . block_area_parm_1 = 1.1;
  obstacle_map[ ISO_COUNTER_MIDDLE_2 ] . block_area_parm_2 = 2.3;
  obstacle_map[ ISO_COUNTER_MIDDLE_2 ] . filename = "iso_counter_0002.png";
  obstacle_map[ ISO_COUNTER_MIDDLE_2 ] . block_vision_too = FALSE ;
  obstacle_map[ ISO_COUNTER_MIDDLE_2 ] . obstacle_short_name = "Counter" ;
  obstacle_map[ ISO_COUNTER_MIDDLE_2 ] . obstacle_long_description = "A counter." ;
  obstacle_map[ ISO_COUNTER_MIDDLE_3 ] . block_area_parm_1 = 1.1;
  obstacle_map[ ISO_COUNTER_MIDDLE_3 ] . block_area_parm_2 = 2.3;
  obstacle_map[ ISO_COUNTER_MIDDLE_3 ] . filename = "iso_counter_0003.png";
  obstacle_map[ ISO_COUNTER_MIDDLE_3 ] . block_vision_too = FALSE ;
  obstacle_map[ ISO_COUNTER_MIDDLE_3 ] . obstacle_short_name = "Counter" ;
  obstacle_map[ ISO_COUNTER_MIDDLE_3 ] . obstacle_long_description = "A counter." ;
  obstacle_map[ ISO_COUNTER_MIDDLE_4 ] . block_area_parm_1 = 1.1;
  obstacle_map[ ISO_COUNTER_MIDDLE_4 ] . block_area_parm_2 = 2.3;
  obstacle_map[ ISO_COUNTER_MIDDLE_4 ] . filename = "iso_counter_0004.png";
  obstacle_map[ ISO_COUNTER_MIDDLE_4 ] . block_vision_too = FALSE ;
  obstacle_map[ ISO_COUNTER_MIDDLE_4 ] . obstacle_short_name = "Counter" ;
  obstacle_map[ ISO_COUNTER_MIDDLE_4 ] . obstacle_long_description = "A counter." ;

  obstacle_map[ ISO_COUNTER_CORNER_ROUND_1 ] . block_area_parm_1 = 1.1;
  obstacle_map[ ISO_COUNTER_CORNER_ROUND_1 ] . block_area_parm_2 = 2.3;
  obstacle_map[ ISO_COUNTER_CORNER_ROUND_1 ] . filename = "iso_counter_0005.png";
  obstacle_map[ ISO_COUNTER_CORNER_ROUND_1 ] . block_vision_too = FALSE ;
  obstacle_map[ ISO_COUNTER_CORNER_ROUND_1 ] . obstacle_short_name = "Counter" ;
  obstacle_map[ ISO_COUNTER_CORNER_ROUND_1 ] . obstacle_long_description = "A counter." ;
  obstacle_map[ ISO_COUNTER_CORNER_ROUND_2 ] . block_area_parm_1 = 1.1;
  obstacle_map[ ISO_COUNTER_CORNER_ROUND_2 ] . block_area_parm_2 = 2.3;
  obstacle_map[ ISO_COUNTER_CORNER_ROUND_2 ] . filename = "iso_counter_0006.png";
  obstacle_map[ ISO_COUNTER_CORNER_ROUND_2 ] . block_vision_too = FALSE ;
  obstacle_map[ ISO_COUNTER_CORNER_ROUND_2 ] . obstacle_short_name = "Counter" ;
  obstacle_map[ ISO_COUNTER_CORNER_ROUND_2 ] . obstacle_long_description = "A counter." ;
  obstacle_map[ ISO_COUNTER_CORNER_ROUND_3 ] . block_area_parm_1 = 1.1;
  obstacle_map[ ISO_COUNTER_CORNER_ROUND_3 ] . block_area_parm_2 = 2.3;
  obstacle_map[ ISO_COUNTER_CORNER_ROUND_3 ] . filename = "iso_counter_0007.png";
  obstacle_map[ ISO_COUNTER_CORNER_ROUND_3 ] . block_vision_too = FALSE ;
  obstacle_map[ ISO_COUNTER_CORNER_ROUND_3 ] . obstacle_short_name = "Counter" ;
  obstacle_map[ ISO_COUNTER_CORNER_ROUND_3 ] . obstacle_long_description = "A counter." ;
  obstacle_map[ ISO_COUNTER_CORNER_ROUND_4 ] . block_area_parm_1 = 1.1;
  obstacle_map[ ISO_COUNTER_CORNER_ROUND_4 ] . block_area_parm_2 = 2.3;
  obstacle_map[ ISO_COUNTER_CORNER_ROUND_4 ] . filename = "iso_counter_0008.png";
  obstacle_map[ ISO_COUNTER_CORNER_ROUND_4 ] . block_vision_too = FALSE ;
  obstacle_map[ ISO_COUNTER_CORNER_ROUND_4 ] . obstacle_short_name = "Counter" ;
  obstacle_map[ ISO_COUNTER_CORNER_ROUND_4 ] . obstacle_long_description = "A counter." ;
   
  obstacle_map[ ISO_COUNTER_CORNER_SHARP_1 ] . block_area_parm_1 = 1.1;
  obstacle_map[ ISO_COUNTER_CORNER_SHARP_1 ] . block_area_parm_2 = 2.3;
  obstacle_map[ ISO_COUNTER_CORNER_SHARP_1 ] . filename = "iso_counter_0009.png";
  obstacle_map[ ISO_COUNTER_CORNER_SHARP_1 ] . block_vision_too = FALSE ;
  obstacle_map[ ISO_COUNTER_CORNER_SHARP_1 ] . obstacle_short_name = "Counter" ;
  obstacle_map[ ISO_COUNTER_CORNER_SHARP_1 ] . obstacle_long_description = "A counter." ;
  obstacle_map[ ISO_COUNTER_CORNER_SHARP_2 ] . block_area_parm_1 = 1.1;
  obstacle_map[ ISO_COUNTER_CORNER_SHARP_2 ] . block_area_parm_2 = 2.3;
  obstacle_map[ ISO_COUNTER_CORNER_SHARP_2 ] . filename = "iso_counter_0010.png";
  obstacle_map[ ISO_COUNTER_CORNER_SHARP_2 ] . block_vision_too = FALSE ;
  obstacle_map[ ISO_COUNTER_CORNER_SHARP_2 ] . obstacle_short_name = "Counter" ;
  obstacle_map[ ISO_COUNTER_CORNER_SHARP_2 ] . obstacle_long_description = "A counter." ;
  obstacle_map[ ISO_COUNTER_CORNER_SHARP_3 ] . block_area_parm_1 = 1.1;
  obstacle_map[ ISO_COUNTER_CORNER_SHARP_3 ] . block_area_parm_2 = 2.3;
  obstacle_map[ ISO_COUNTER_CORNER_SHARP_3 ] . filename = "iso_counter_0011.png";
  obstacle_map[ ISO_COUNTER_CORNER_SHARP_3 ] . block_vision_too = FALSE ;
  obstacle_map[ ISO_COUNTER_CORNER_SHARP_3 ] . obstacle_short_name = "Counter" ;
  obstacle_map[ ISO_COUNTER_CORNER_SHARP_3 ] . obstacle_long_description = "A counter." ;
  obstacle_map[ ISO_COUNTER_CORNER_SHARP_4 ] . block_area_parm_1 = 1.1;
  obstacle_map[ ISO_COUNTER_CORNER_SHARP_4 ] . block_area_parm_2 = 2.3;
  obstacle_map[ ISO_COUNTER_CORNER_SHARP_4 ] . filename = "iso_counter_0012.png";
  obstacle_map[ ISO_COUNTER_CORNER_SHARP_4 ] . block_vision_too = FALSE ;
  obstacle_map[ ISO_COUNTER_CORNER_SHARP_4 ] . obstacle_short_name = "Counter" ;
  obstacle_map[ ISO_COUNTER_CORNER_SHARP_4 ] . obstacle_long_description = "A counter." ;

  
  obstacle_map[ ISO_BAR_TABLE ] . block_area_parm_1 = 1.1;
  obstacle_map[ ISO_BAR_TABLE ] . block_area_parm_2 = 1.3;
  obstacle_map[ ISO_BAR_TABLE ] . filename = "iso_tables_0005.png";
  obstacle_map[ ISO_BAR_TABLE ] . block_vision_too = FALSE ;
  obstacle_map[ ISO_BAR_TABLE ] . obstacle_short_name = "Bar table";
  obstacle_map[ ISO_BAR_TABLE ] . obstacle_long_description = "You can see some food stains on this yellow round table.";

  obstacle_map[ ISO_TABLE_OVAL_1 ] . block_area_parm_1 = 1.1;
  obstacle_map[ ISO_TABLE_OVAL_1 ] . block_area_parm_2 = 1.3;
  obstacle_map[ ISO_TABLE_OVAL_1 ] . filename = "iso_tables_0006.png";
  obstacle_map[ ISO_TABLE_OVAL_1 ] . block_vision_too = FALSE ;
  obstacle_map[ ISO_TABLE_OVAL_1 ] . obstacle_short_name = "Table";
  obstacle_map[ ISO_TABLE_OVAL_1 ] . obstacle_long_description = "Tables such as this one have been used for centuries to store various items on top of them.";

  obstacle_map[ ISO_TABLE_OVAL_2 ] . block_area_parm_1 = 1.1;
  obstacle_map[ ISO_TABLE_OVAL_2 ] . block_area_parm_2 = 1.3;
  obstacle_map[ ISO_TABLE_OVAL_2 ] . filename = "iso_tables_0007.png";
  obstacle_map[ ISO_TABLE_OVAL_2 ] . block_vision_too = FALSE ;
  obstacle_map[ ISO_TABLE_OVAL_2 ] . obstacle_short_name = "Table";
  obstacle_map[ ISO_TABLE_OVAL_2 ] . obstacle_long_description = "Tables such as this one have been used for centuries to store various items on top of them.";

  
  obstacle_map[ ISO_TABLE_GLASS_1 ] . block_area_parm_1 = 1.1;
  obstacle_map[ ISO_TABLE_GLASS_1 ] . block_area_parm_2 = 1.3;
  obstacle_map[ ISO_TABLE_GLASS_1 ] . filename = "iso_tables_0008.png";
  obstacle_map[ ISO_TABLE_GLASS_1 ] . transparent = TRANSPARENCY_FOR_SEE_THROUGH_OBJECTS; 
  obstacle_map[ ISO_TABLE_GLASS_1 ] . block_vision_too = FALSE ;
  obstacle_map[ ISO_TABLE_GLASS_1 ] . obstacle_short_name = "Glass table";
  obstacle_map[ ISO_TABLE_GLASS_1 ] . obstacle_long_description = "This table is made out of see-through glass.";
  obstacle_map[ ISO_TABLE_GLASS_2 ] . block_area_parm_1 = 1.1;
  obstacle_map[ ISO_TABLE_GLASS_2 ] . block_area_parm_2 = 1.3;
  obstacle_map[ ISO_TABLE_GLASS_2 ] . filename = "iso_tables_0009.png";
  obstacle_map[ ISO_TABLE_GLASS_2 ] . transparent = TRANSPARENCY_FOR_SEE_THROUGH_OBJECTS; 
  obstacle_map[ ISO_TABLE_GLASS_2 ] . block_vision_too = FALSE ;
  obstacle_map[ ISO_TABLE_GLASS_2 ] . obstacle_short_name = "Glass table";
  obstacle_map[ ISO_TABLE_GLASS_2 ] . obstacle_long_description = "This table is made out of see-through glass.";
    
  //--------------------
  // Now that we have defined the block area paramters, it's time to do some
  // simple but often required computations in advance:  Calculating the offsets
  // each obstacle's block area has from the obstacle center.
  //
  for ( i = 0 ; i < NUMBER_OF_OBSTACLE_TYPES ; i ++ )
    {
      obstacle_map [ i ] . upper_border = - obstacle_map [ i ] . block_area_parm_1 / 2.0 ;
      obstacle_map [ i ] . lower_border = + obstacle_map [ i ] . block_area_parm_1 / 2.0 ;
      obstacle_map [ i ] . left_border  = - obstacle_map [ i ] . block_area_parm_2 / 2.0 ;
      obstacle_map [ i ] . right_border = + obstacle_map [ i ] . block_area_parm_2 / 2.0 ;
    }

}; // void init_obstacle_data( void )

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
    char shadow_file_name[2000];

    init_obstacle_data();
    
    for ( i = 0 ; i < NUMBER_OF_OBSTACLE_TYPES ; i ++ )
    {
	//--------------------
	// At first we construct the file name of the single tile file we are about to load...
	//
	strcpy ( ConstructedFileName , "obstacles/" );
	strcat ( ConstructedFileName , obstacle_map [ i ] . filename ) ;
	fpath = find_file ( ConstructedFileName , GRAPHICS_DIR , FALSE );
	
	if ( use_open_gl )
	{
	    get_iso_image_from_file_and_path ( fpath , & ( obstacle_map [ i ] . image ) , TRUE ); 
	    make_sure_zoomed_surface_is_there ( & ( obstacle_map [ i ] . image ) ); 
	    make_sure_automap_surface_is_there ( & ( obstacle_map [ i ] ) ); 
	    
	    make_texture_out_of_surface ( & ( obstacle_map [ i ] . image ) ) ;
	}
	else
	    get_iso_image_with_colorkey_from_file_and_path ( fpath , & ( obstacle_map [ i ] . image ) ) ;

	//--------------------
	// Maybe the obstacle in question also has a shadow image?  In that
	// case we should load the shadow image now.  Otherwise we might just
	// mark the shadow image as not in use, so we won't face problems with
	// missing shadow images inside the code
	//
	// We need a new file name of course:  (this assumes, that the filename
	// has been constructed above already...
	if ( strlen ( ConstructedFileName ) >= 8 )
	{
	    strcpy ( shadow_file_name , ConstructedFileName ) ;
	    shadow_file_name [ strlen ( shadow_file_name ) - 8 ] = 0 ;
	    strcat ( shadow_file_name , "shadow_" ) ;
	    strcat ( shadow_file_name , & ( ConstructedFileName [ strlen ( ConstructedFileName ) - 8 ] ) ) ;
	    DebugPrintf ( 2 , "\n%s(): shadow file name: %s " , __FUNCTION__ , shadow_file_name ); 
	    fpath = find_file_silent ( shadow_file_name , GRAPHICS_DIR , FALSE );
	    if ( fpath == NULL ) 
	    {
		obstacle_map [ i ] . shadow_image . surface = NULL ;
		obstacle_map [ i ] . shadow_image . texture_has_been_created = FALSE ;
		DebugPrintf ( 2 , "\n%s(): no success with that last shadow image file name." , __FUNCTION__ ) ;
		continue;
	    }
	}
	
	if ( use_open_gl )
	{
	    get_iso_image_from_file_and_path ( fpath , & ( obstacle_map [ i ] . shadow_image ) , TRUE ); 
	    // make_sure_zoomed_surface_is_there ( & ( obstacle_map [ i ] . shadow_image ) ); 
	    make_texture_out_of_surface ( & ( obstacle_map [ i ] . shadow_image ) ) ;
	}
	else
	    get_iso_image_with_colorkey_from_file_and_path ( fpath , & ( obstacle_map [ i ] . shadow_image ) ) ;

	DebugPrintf ( 1 , "\n%s(): shadow image %s loaded successfully." , __FUNCTION__ , shadow_file_name );
    }

}; // void load_all_obstacles ( void )

/* ----------------------------------------------------------------------
 * This function loads the graphics for one floor tile type (given as a
 * parameter) into memory and also makes textures and the like in case
 * of OpenGL as selected graphics output method.
 * ---------------------------------------------------------------------- */
void
load_one_isometric_floor_tile ( int tile_type ) 
{
    char *fpath;
    char ConstructedFileName[2000];
    
    //--------------------
    // At first we construct the file name of the single tile file we are about to load...
    //
    strcpy ( ConstructedFileName , "floor_tiles/" );
    strcat ( ConstructedFileName , floor_tile_filenames [ tile_type ] );
    fpath = find_file ( ConstructedFileName , GRAPHICS_DIR , FALSE );
    
    if ( use_open_gl )
    {
	get_iso_image_from_file_and_path ( fpath , & ( floor_iso_images [ tile_type ] ) , TRUE ) ;
	
	make_texture_out_of_surface ( & ( floor_iso_images [ tile_type ] ) ) ;
    }
    else
    {
	get_iso_image_with_colorkey_from_file_and_path ( fpath , & ( floor_iso_images [ tile_type ] ) ) ;
    }
    
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
