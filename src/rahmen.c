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
 * This file contains all functions to update and draw the top status 
 * displays with status etc...
 * ---------------------------------------------------------------------- */

#define _rahmen_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "proto.h"
#include "global.h"

#include "SDL_rotozoom.h" // that's for rotating the speed-o-meter arrows

extern char *InfluenceModeNames[];

/* ----------------------------------------------------------------------
 * This function writes the description of an item into the item description
 * string.
 * ---------------------------------------------------------------------- */
void 
GiveItemDescription ( char* ItemDescText , item* CurItem , int ForShop )
{
  char linebuf[5000];
  char AppendToLine = 0 ; // if we should write the next bonus with a comma as separator or with a newline

  //--------------------
  // We initialize the description text, so that at least something,
  // i.e. something terminated with a null charcter is in there.
  //
  strcpy( ItemDescText , "" );
  
  //--------------------
  // In case of a NULL given as item pointer, we can return immediately.
  //
  if ( CurItem == NULL ) return;
  if ( CurItem->type == (-1) ) 
    {
      GiveStandardErrorMessage ( "GiveItemDescription(...)" , "\
An item description was requested for an item, that does not seem to \n\
exist really (i.e. has a type = (-1) ).",
				 PLEASE_INFORM, IS_FATAL );
      return;
    }

  //--------------------
  // we get the pure item name, also with font changes enabled.
  //
  write_full_item_name_into_string ( CurItem , linebuf ) ;
  strcat( ItemDescText , linebuf );

  if ( ForShop )
    {
      strcat( ItemDescText , "\n             " );
    }
  else
    {
      strcat( ItemDescText , "\n" );
    }

  // --------------------
  // If it's a weapon, then we give out the damage value of that weapon as well
  //
  if ( CurItem->damage )
    {
      if ( ! ForShop ) 
	sprintf( linebuf , "Damage: %d to %d \n" , CurItem->damage , CurItem->damage_modifier + CurItem->damage );
      else
	sprintf( linebuf , "Dam: %d-%d " , CurItem->damage , CurItem->damage_modifier + CurItem->damage );
      strcat( ItemDescText , linebuf );
    }

  //--------------------
  // If this item has a multiplicity, we print it out
  //
  if ( ItemMap [ CurItem->type ] . item_group_together_in_inventory )
    {
      sprintf( linebuf , "Multiplicity: %d \n" , CurItem->multiplicity );
      strcat( ItemDescText , linebuf );
    }

  // --------------------
  // If this item gives some armour bonus, we say so
  //
  if ( CurItem->ac_bonus )
    {
      sprintf( linebuf , "Armour: %d" , CurItem->ac_bonus );
      strcat( ItemDescText , linebuf );
      if ( !ForShop ) strcat( ItemDescText , " \n " );
    }

  // --------------------
  // If this is a destructible item, we finally give it's current condition
  // and if it can be equipped, but not destroyed, we will also say so
  //
  if ( CurItem->max_duration != (-1) )
    {
      if ( ! ForShop ) 
	sprintf( linebuf , " Durability: %d of %d" , (int) CurItem->current_duration , (int) CurItem->max_duration );
      else
	sprintf( linebuf , " Dur: %d/%d" , (int) CurItem->current_duration , (int) CurItem->max_duration );
      strcat( ItemDescText , linebuf );
    }
  else if ( ItemMap [ CurItem->type ].item_can_be_installed_in_influ )
    {
      strcat( ItemDescText , " Indestructable" );
    };

  // --------------------
  // If this item has some strength or dex or magic requirements, we say so
  //
  if ( ( ItemMap[ CurItem->type ].item_require_strength  != ( -1 ) ) || 
       ( ItemMap[ CurItem->type ].item_require_dexterity != ( -1 ) ) )
    {
      if ( ! ForShop ) strcat ( ItemDescText , "\n" );
      if ( ForShop ) strcat ( ItemDescText , " Required:" );
      if ( ItemMap[ CurItem->type ].item_require_strength != ( -1 ) )
	{
	  if ( ForShop )
	    sprintf( linebuf , "   Pow: %d" , ItemMap[ CurItem->type ].item_require_strength );
	  else 
	    sprintf( linebuf , " Required Power: %d " , ItemMap[ CurItem->type ].item_require_strength );
	  strcat( ItemDescText , linebuf );
	}
      if ( ItemMap[ CurItem->type ].item_require_dexterity != ( -1 ) )
	{
	  if ( ForShop )
	    sprintf( linebuf , "   Dis: %d" ,  ItemMap[ CurItem->type ].item_require_dexterity );
	  else
	    sprintf( linebuf , " Power Distribution: %d " ,  ItemMap[ CurItem->type ].item_require_dexterity );
	  strcat( ItemDescText , linebuf );
	}
    }
  else if ( ForShop )
    {
      strcat ( ItemDescText , " , No required attributes " );
    }


  // --------------------
  // If it's a usable item, then we say, that it can be used via right-clicking on it
  //
  if ( ( ItemMap[ CurItem->type ].item_can_be_applied_in_combat ) && ( !ForShop ) )
    {
      switch ( CurItem->type )
	{
	case ITEM_SMALL_HEALTH_POTION:
	case ITEM_MEDIUM_HEALTH_POTION:
	case ITEM_FULL_HEALTH_POTION:
	  sprintf( linebuf , "Recover Health\n" );
	  strcat( ItemDescText , linebuf );
	  break;

	case ITEM_MEDIUM_MANA_POTION:
	case ITEM_FULL_MANA_POTION:
	case ITEM_SMALL_MANA_POTION:
	  sprintf( linebuf , "Recover Force\n" );
	  strcat( ItemDescText , linebuf );
	  break;

	case ITEM_STRENGTH_PILL:
	  sprintf( linebuf , "Permanently gain +1 strength\n" );
	  strcat( ItemDescText , linebuf );
	  break;

	case ITEM_DEXTERITY_PILL:
	  sprintf( linebuf , "Permanently gain +1 dexterity\n" );
	  strcat( ItemDescText , linebuf );
	  break;

	case ITEM_MAGIC_PILL:
	  sprintf( linebuf , "Permanently gain +1 magic\n" );
	  strcat( ItemDescText , linebuf );
	  break;

	case ITEM_SPELLBOOK_OF_HEALING:
	case ITEM_SPELLBOOK_OF_EXPLOSION_CIRCLE:
	case ITEM_SPELLBOOK_OF_EXPLOSION_RAY:
	case ITEM_SPELLBOOK_OF_TELEPORT_HOME:
	case ITEM_SPELLBOOK_OF_PLASMA_BOLT:
	case ITEM_SPELLBOOK_OF_ICE_BOLT:
	case ITEM_SPELLBOOK_OF_POISON_BOLT:
	case ITEM_SPELLBOOK_OF_PETRIFICATION:
	case ITEM_SPELLBOOK_OF_RADIAL_EMP_WAVE:
	case ITEM_SPELLBOOK_OF_RADIAL_VMX_WAVE:
	case ITEM_SPELLBOOK_OF_RADIAL_PLASMA_WAVE:
	case ITEM_SPELLBOOK_OF_DETECT_ITEMS:
	case ITEM_SPELLBOOK_OF_IDENTIFY:
	  sprintf( linebuf , "Permanently aquire this skill/spell\n" );
	  strcat( ItemDescText , linebuf );
	  break;

	default:
	  sprintf( linebuf , "USE UNDESCRIBED YET\n" );
	  strcat( ItemDescText , linebuf );
	  break;
	}
      sprintf( linebuf , "Right click to use" );
      strcat( ItemDescText , linebuf );
    }

  //--------------------
  // Maybe it's a special unique plugin, then we print out this items use
  //
  if ( ( CurItem->type == ITEM_START_PLUGIN_WARRIOR ) && ( !ForShop ) )
    {
      sprintf( linebuf , "\nFreezes targets after melee hit" );
      strcat( ItemDescText , linebuf );
    }
  if ( ( CurItem->type == ITEM_START_PLUGIN_SNIPER ) && ( !ForShop ) )
    {
      sprintf( linebuf , "\nAdds double damage to ranged attacks" );
      strcat( ItemDescText , linebuf );
    }
  if ( ( CurItem->type == ITEM_START_PLUGIN_HACKER ) && ( !ForShop ) )
    {
      sprintf( linebuf , "\nAdds two levels to all spells" );
      strcat( ItemDescText , linebuf );
    }


  //--------------------
  // If the item has some suffixes, we describe these as well, but ONLY IF
  // THE ITEM HAS BEEN IDENTIFIED YET of course.
  //
  if ( ( CurItem->suffix_code != (-1) ) || ( CurItem->prefix_code != (-1) ) ) 
    {
      if ( CurItem->is_identified == TRUE )
	{
	  strcat ( ItemDescText , "\n" );
	  strcat ( ItemDescText , font_switchto_red );
	  AppendToLine = 0 ;
	  
	  if ( ForShop ) strcat( ItemDescText , "             " );

	  if ( CurItem->bonus_to_str )
	    {
	      if ( CurItem->bonus_to_str > 0 ) strcat( ItemDescText , "+" );
	      // sprintf( linebuf , "%d to Power\n" , CurItem->bonus_to_str );
	      sprintf( linebuf , "%d to Power" , CurItem->bonus_to_str );
	      // if ( ForShop ) strcat ( linebuf , ", " ); else strcat ( linebuf , "\n" );
	      AppendToLine = TRUE;
	      strcat( ItemDescText , linebuf );
	    }
	  
	  if ( CurItem->bonus_to_dex )
	    {
	      if ( AppendToLine ) { if ( ForShop ) strcat ( ItemDescText , ", " ); else strcat ( ItemDescText , "\n" ); };
	      AppendToLine = TRUE;
	      if ( CurItem->bonus_to_dex > 0 ) strcat( ItemDescText , "+" );
	      sprintf( linebuf , "%d to Power Distribution" , CurItem->bonus_to_dex );
	      strcat( ItemDescText , linebuf );
	    }
	  
	  if ( CurItem->bonus_to_mag )
	    {
	      if ( AppendToLine ) { if ( ForShop ) strcat ( ItemDescText , ", " ); else strcat ( ItemDescText , "\n" ); };
	      AppendToLine = TRUE;
	      if ( CurItem->bonus_to_mag > 0 ) strcat( ItemDescText , "+" );
	      sprintf( linebuf , "%d to Mind" , CurItem->bonus_to_mag );
	      strcat( ItemDescText , linebuf );
	    }
	  
	  if ( CurItem->bonus_to_vit )
	    {
	      if ( CurItem->bonus_to_vit > 0 ) strcat( ItemDescText , "+" );
	      if ( AppendToLine ) { if ( ForShop ) strcat ( ItemDescText , ", " ); else strcat ( ItemDescText , "\n" ); };
	      AppendToLine = TRUE;
	      sprintf( linebuf , "%d to Vitality" , CurItem->bonus_to_vit );
	      strcat( ItemDescText , linebuf );
	    }
	  
	  if ( CurItem->bonus_to_life )
	    {
	      if ( AppendToLine ) { if ( ForShop ) strcat ( ItemDescText , ", " ); else strcat ( ItemDescText , "\n" ); };
	      AppendToLine = TRUE;
	      if ( CurItem->bonus_to_life > 0 ) strcat( ItemDescText , "+" );
	      sprintf( linebuf , "%d Energy" , CurItem->bonus_to_life );
	      strcat( ItemDescText , linebuf );
	    }
	  
	  if ( CurItem->bonus_to_force )
	    {
	      if ( AppendToLine ) { if ( ForShop ) strcat ( ItemDescText , ", " ); else strcat ( ItemDescText , "\n" ); };
	      AppendToLine = TRUE;
	      if ( CurItem->bonus_to_force > 0 ) strcat( ItemDescText , "+" );
	      sprintf( linebuf , "%d Force" , CurItem->bonus_to_force );
	      strcat( ItemDescText , linebuf );
	    }
	  
	  if ( CurItem->bonus_to_tohit )
	    {
	      if ( AppendToLine ) { if ( ForShop ) strcat ( ItemDescText , ", " ); else strcat ( ItemDescText , "\n" ); };
	      if ( CurItem->bonus_to_tohit > 0 ) strcat( ItemDescText , "+" );
	      AppendToLine = TRUE;
	      sprintf( linebuf , "%d%% to hit" , CurItem->bonus_to_tohit );
	      strcat( ItemDescText , linebuf );
	    }

	  if ( CurItem->bonus_to_all_attributes )
	    {
	      if ( AppendToLine ) { if ( ForShop ) strcat ( ItemDescText , ", " ); else strcat ( ItemDescText , "\n" ); };
	      if ( CurItem->bonus_to_all_attributes > 0 ) strcat( ItemDescText , "+" );
	      AppendToLine = TRUE;
	      sprintf( linebuf , "%d to all attributes" , CurItem->bonus_to_all_attributes );
	      strcat( ItemDescText , linebuf );
	    }

	  // Now we display the percentage bonus to ac or damage
	  if ( CurItem->bonus_to_ac_or_damage )
	    {
	      if ( ItemMap [ CurItem->type ].base_ac_bonus )
		{
		  // if ( ForShop ) strcat( ItemDescText , "             " );
		  if ( AppendToLine ) 
		    { 
		      if ( ForShop ) strcat ( ItemDescText , ", " ); 
		      else strcat ( ItemDescText , "\n" ); 
		    }
		  AppendToLine = TRUE;
		  if ( CurItem->bonus_to_ac_or_damage > 0 ) strcat( ItemDescText , "+" );
		  sprintf( linebuf , "%d%% to armour" , CurItem->bonus_to_ac_or_damage );
		  strcat( ItemDescText , linebuf );
		}
	      if ( ItemMap [ CurItem->type ].base_item_gun_damage )
		{
		  // if ( ForShop ) strcat( ItemDescText , "             " );
		  if ( AppendToLine ) 
		    { 
		      if ( ForShop ) strcat ( ItemDescText , ", " ); 
		      else strcat ( ItemDescText , "\n" ); 
		    }
		  AppendToLine = TRUE;
		  if ( CurItem->bonus_to_ac_or_damage > 0 ) strcat( ItemDescText , "+" );
		  sprintf( linebuf , "%d%% to damage" , CurItem->bonus_to_ac_or_damage );
		  strcat( ItemDescText , linebuf );
		}
	    }


	}
      else
	{
	  strcat ( ItemDescText , "\n" );
	  strcat ( ItemDescText , font_switchto_red );
	  strcat ( ItemDescText , " UNIDENTIFIED " );
	}
    }
}; // void GiveItemDescription ( char* ItemDescText , item* CurItem , int ForShop )

/* ----------------------------------------------------------------------
 * This function writes the description of a droid into the description
 * string.
 * ---------------------------------------------------------------------- */
void 
create_and_blit_droid_description ( int enemy_num )
{
  int text_length;
  SDL_Rect temp_fill_rect;
  Enemy cur_enemy = & AllEnemys [ enemy_num ] ;
  BFont_Info* BFont_to_use = Blue_BFont ;

  text_length = TextWidthFont ( BFont_to_use , AllEnemys [ enemy_num ] . short_description_text );
  // strcpy( ItemDescText , AllEnemys [ index_of_droid_below_mouse_cursor ] . short_description_text );
  temp_fill_rect . y = 50 ;
  temp_fill_rect . h = FontHeight ( BFont_to_use ) ;
  temp_fill_rect . w = ( text_length * cur_enemy -> energy ) / Druidmap [ cur_enemy -> type ] . maxenergy ;
  if ( cur_enemy -> energy <= 0 ) temp_fill_rect . w = 0 ;
  temp_fill_rect . x = UserCenter_x - text_length / 2 ;

  //--------------------
  // If the 'enemy' is hostile, then we use red underlying color.  If
  // it's a friendly entity, then we use green underlying color for the
  // enemy's name.
  //
  if ( ! cur_enemy -> is_friendly ) 
    {
      our_SDL_fill_rect_wrapper ( Screen , & ( temp_fill_rect ) , SDL_MapRGB ( Screen->format , 0x099 , 0x000 , 0x000 ) );
    }
  else
    {
      our_SDL_fill_rect_wrapper ( Screen , & ( temp_fill_rect ) , SDL_MapRGB ( Screen->format , 0x000 , 0x099 , 0x000 ) );
    }

  temp_fill_rect . x = temp_fill_rect . x + temp_fill_rect . w ;
  temp_fill_rect . w = text_length - temp_fill_rect . w ;
  our_SDL_fill_rect_wrapper ( Screen , & ( temp_fill_rect ) , SDL_MapRGB ( Screen->format , 0x000 , 0x000 , 0x000 ) );
  temp_fill_rect . x = UserCenter_x - text_length / 2 ;

  PutStringFont ( Screen , BFont_to_use , temp_fill_rect . x , temp_fill_rect . y , 
		  AllEnemys [ enemy_num ] . short_description_text );

  /*
  // --------------------
  // First we print out the droid name.  That's simple.
  //
  strcpy( DroidDescText , Druidmap[ CurEnemy->type ].druidname );
  strcat( DroidDescText , " -- " );
  strcat( DroidDescText , Classname [ Druidmap[ CurEnemy->type ].class ] );
  strcat( DroidDescText , "\n" );

  if ( CurEnemy->is_friendly == FALSE )
    {
      sprintf( linebuf , " Total Kills : %d " , Me[0].KillRecord[ CurEnemy->type ] );
      strcat ( DroidDescText , linebuf );
    }
  */

}; // void GiveDroidDescription ( char* ItemDescText , item* CurItem )


/* ----------------------------------------------------------------------
 * This function displays the status bars for mana and energy in some 
 * corner of the screen.  The dimensions and location of the bar are
 * specified in items.h
 * ---------------------------------------------------------------------- */
void 
ShowCurrentSkill( void )
{
  SDL_Rect Target_Rect;
  static int Mouse_Button_Pressed_Previous_Frame = FALSE;

  if ( Me[0].status == BRIEFING ) return;
  if ( GameConfig.Inventory_Visible ) return;
  if ( GameConfig.SkillScreen_Visible && GameConfig.skill_explanation_screen_visible ) return;

  Target_Rect.x = CURRENT_SKILL_RECT_X ;
  Target_Rect.y = CURRENT_SKILL_RECT_Y ;
  Target_Rect.w = CURRENT_SKILL_RECT_W ;
  Target_Rect.h = CURRENT_SKILL_RECT_H ;

  LoadOneSkillSurfaceIfNotYetLoaded ( Me[0].readied_skill );

  if ( use_open_gl )
    {
      blit_open_gl_texture_to_screen_position ( SpellSkillMap [ Me[0].readied_skill ] . spell_skill_icon_surface , Target_Rect . x , Target_Rect . y , TRUE ) ;
    }
  else
    our_SDL_blit_surface_wrapper ( SpellSkillMap [ Me[0].readied_skill ] . spell_skill_icon_surface . surface , NULL , Screen , &Target_Rect );

  /*
  //--------------------
  // Here we also check for possible mouse clicks on the skill icon.  In this
  // case we activate or deactivate the skill screen.
  //
  if ( ( GetMousePos_x() + MOUSE_CROSSHAIR_OFFSET_X >= CURRENT_SKILL_RECT_X ) &&
       ( GetMousePos_x() + MOUSE_CROSSHAIR_OFFSET_X <= CURRENT_SKILL_RECT_X + CURRENT_SKILL_RECT_W ) &&
       ( GetMousePos_y() + MOUSE_CROSSHAIR_OFFSET_Y >= CURRENT_SKILL_RECT_Y ) &&
       ( GetMousePos_y() + MOUSE_CROSSHAIR_OFFSET_Y <= CURRENT_SKILL_RECT_Y + CURRENT_SKILL_RECT_H ) &&
       axis_is_active &&
       !Mouse_Button_Pressed_Previous_Frame )
    GameConfig.SkillScreen_Visible = ! GameConfig.SkillScreen_Visible;
  */

  Mouse_Button_Pressed_Previous_Frame = axis_is_active;

}; // void ShowCurrentSkill ( void )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
blit_energy_o_meter( void )
{
  SDL_Surface* tmp;
  char *fpath;
  static iso_image speed_meter_iso_image = UNLOADED_ISO_IMAGE ;
  static iso_image SpeedMeterEnergyArrowImage = UNLOADED_ISO_IMAGE ;
  static iso_image SpeedMeterManaArrowImage = UNLOADED_ISO_IMAGE ;
  static SDL_Surface *SpeedOMeterWorkingCopy = NULL;
  static SDL_Rect SpeedMeterRect;
  static int Previous_Energy = - 1234; // a completely unrealistic value;
  static int Previous_Mana = - 123; // a completely unrealistic value;
  static int Previous_Maxenergy = - 123; // a completely unrealistic value;
  static int Previous_Maxmana = - 123; // a completely unrealistic value;
  point PivotPosition = { 42 , 49 };
  SDL_Surface *RotatedArrow; // this will be blitted into the speed-o-meter working copy
  SDL_Rect ArrowRect;

  //--------------------
  // At first we read in the raw images for the speed-o-meter and 
  // for the speed-o-meter arrow pointer.
  // 
  // This can be done even during briefing phases, even should be
  // done then to prevent framerate-distortion later, when the first
  // real-game-frame is drawn.
  //
  if ( ( speed_meter_iso_image . surface == NULL ) && ( ! speed_meter_iso_image . texture_has_been_created ) )
    {
      fpath = find_file ( "speed_o_meter.png" , GRAPHICS_DIR, FALSE);
      get_iso_image_from_file_and_path ( fpath , & ( speed_meter_iso_image ) , FALSE ) ;
      tmp = speed_meter_iso_image . surface ;
      speed_meter_iso_image . surface = SDL_DisplayFormatAlpha ( speed_meter_iso_image . surface );
      SDL_FreeSurface ( tmp ) ;

      fpath = find_file ( "speed_o_meter_arrow_energy.png" , GRAPHICS_DIR, FALSE);
      get_iso_image_from_file_and_path ( fpath , & ( SpeedMeterEnergyArrowImage ) , FALSE ) ;
      tmp = SpeedMeterEnergyArrowImage . surface ;
      SpeedMeterEnergyArrowImage . surface = SDL_DisplayFormatAlpha ( SpeedMeterEnergyArrowImage . surface ) ;
      SDL_FreeSurface ( tmp ) ;

      fpath = find_file ( "speed_o_meter_arrow_mana.png" , GRAPHICS_DIR, FALSE);
      get_iso_image_from_file_and_path ( fpath , & ( SpeedMeterManaArrowImage ) , FALSE ) ;
      tmp = SpeedMeterManaArrowImage . surface ;
      SpeedMeterManaArrowImage . surface = SDL_DisplayFormatAlpha ( SpeedMeterManaArrowImage . surface ) ;
      SDL_FreeSurface ( tmp ) ;

      //--------------------
      // We define the right side of the user screen as the rectangle
      // for our speed-o-meter
      //
      SpeedMeterRect.x = SCREEN_WIDTH - speed_meter_iso_image . surface ->w;
      SpeedMeterRect.y = 0; 
      SpeedMeterRect.w = speed_meter_iso_image . surface -> w;
      SpeedMeterRect.h = speed_meter_iso_image . surface -> h;

      if ( use_open_gl )
	{
	  make_texture_out_of_surface ( & ( speed_meter_iso_image ) ) ;
	  make_texture_out_of_surface ( & ( SpeedMeterEnergyArrowImage ) ) ;
	  make_texture_out_of_surface ( & ( SpeedMeterManaArrowImage ) ) ;
	}
    }

  //--------------------
  // Of course we do not blit anything during the briefing phase, so as
  // not to disturb the scrolling text at the beginning.
  //
  if ( Me[0].status == BRIEFING ) return;
  

  if ( use_open_gl ) 
    {

      prepare_open_gl_for_blending_textures ( );

      blit_open_gl_texture_to_screen_position ( speed_meter_iso_image , 
						SCREEN_WIDTH - speed_meter_iso_image . original_image_width , 0 , FALSE );

      blit_rotated_open_gl_texture_with_center ( SpeedMeterEnergyArrowImage , 
						 SCREEN_WIDTH - speed_meter_iso_image . original_image_width + PivotPosition . x , 
						 0 + PivotPosition . y  , 
						 - 360 * 3 / 4 * Me[0].energy / Me[0].maxenergy );

      blit_rotated_open_gl_texture_with_center ( SpeedMeterManaArrowImage , 
						 SCREEN_WIDTH - speed_meter_iso_image . original_image_width + PivotPosition . x , 
						 0 + PivotPosition . y  , 
						 - 360 * 3 / 4 * Me [ 0 ] . mana / Me [ 0 ] . maxmana );

      remove_open_gl_blending_mode_again ( );

    } // if ( use_open_gl )
  else
    {
      //--------------------
      // We only need to regenerate the whole image, blitting the arrows into
      // a new working copy, if something has changed in the values displayed
      // by the arrow position.  
      //
      if ( ( Previous_Energy    != (int) Me[0].energy ) ||
	   ( Previous_Mana      != (int) Me[0].mana ) ||
	   ( Previous_Maxenergy != (int) Me[0].maxenergy ) ||
	   ( Previous_Maxmana   != (int) Me[0].maxmana ) )
	{
	  //--------------------
	  // We generate a new fresh empty speed-o-meter in the working copy
	  //
	  SDL_FreeSurface ( SpeedOMeterWorkingCopy );
	  SpeedOMeterWorkingCopy = our_SDL_display_format_wrapperAlpha ( speed_meter_iso_image . surface );
	  SDL_SetColorKey ( SpeedOMeterWorkingCopy , SDL_SRCCOLORKEY, 
			    SDL_MapRGB ( SpeedOMeterWorkingCopy -> format , 255, 0, 255 ) ); 
	  
	  // We blit in the red arrow, showing current energy
	  RotatedArrow = rotozoomSurface( SpeedMeterEnergyArrowImage . surface , 
					  - 360 * 3 / 4 * Me[0].energy / Me[0].maxenergy , 1.0 , FALSE );
	  
	  ArrowRect.x = PivotPosition.x - ( RotatedArrow->w / 2 ) ;
	  ArrowRect.y = PivotPosition.y - ( RotatedArrow->h / 2 ) ;
	  our_SDL_blit_surface_wrapper( RotatedArrow , NULL , SpeedOMeterWorkingCopy , & ArrowRect );
	  SDL_FreeSurface( RotatedArrow );
	  
	  // We blit in the blue arrow, showing current mana
	  RotatedArrow = rotozoomSurface( SpeedMeterManaArrowImage . surface , 
					  -360 * 3 / 4 * Me[0].mana / Me[0].maxmana , 1.0 , FALSE );
	  
	  ArrowRect.x = PivotPosition.x - ( RotatedArrow->w / 2 ) ;
	  ArrowRect.y = PivotPosition.y - ( RotatedArrow->h / 2 ) ;
	  our_SDL_blit_surface_wrapper( RotatedArrow , NULL , SpeedOMeterWorkingCopy , & ArrowRect );
	  SDL_FreeSurface( RotatedArrow );
	  
	  //--------------------
	  // And of course we must remember the current values, so that
	  // we can detect any changes when this function is called during 
	  // the next frame.
	  //
	  Previous_Energy    = Me[0].energy ;
	  Previous_Mana      = Me[0].mana ;
	  Previous_Maxenergy = Me[0].maxenergy ;
	  Previous_Maxmana   = Me[0].maxmana ;
	  
	  //--------------------
	  // Just to make sure, that we really achieved, that the thing is not reassembled
	  // every frame, we print out a message.
	  //
	  // DebugPrintf ( 2 , "\nValue change detected. --> Speed-o-meter completely reassembled...." ) ;
	}

      //--------------------
      // Finally, we blit the fully assembled speed-o-meter
      //
      our_SDL_blit_surface_wrapper( SpeedOMeterWorkingCopy , NULL , Screen, &SpeedMeterRect );

    } // if ( ! use_open_gl )

}; // void blit_energy_o_meter ( void )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
blit_experience_countdown_bars ( void )
{
  SDL_Rect experience_countdown_rect;
  SDL_Rect un_experience_countdown_rect;
  static Uint32 experience_countdown_rect_color = 0 ;
  static Uint32 un_experience_countdown_rect_color = 0 ;
  int exp_range = Me [ 0 ] . ExpRequired - Me [ 0 ] . ExpRequired_previously ;
  int exp_achieved = Me [ 0 ] . Experience - Me [ 0 ] . ExpRequired_previously ;

  //--------------------
  // At game startup, it might be that an uninitialized Tux (with 0 in the
  // max running power entry) is still in the data structure and when the
  // title displayes, this causes division by zero... 
  //
  if ( Me [ 0 ] . ExpRequired <= 1 ) return ;
  if ( ( Me [ 0 ] . Experience > Me [ 0 ] . ExpRequired ) ||
       ( exp_range <= 1 ) ||
       ( exp_achieved < 0 ) )
    {
      GiveStandardErrorMessage ( "blit_experience_countdown_bars(...)" , "\
The current experience of the Tux is higher than the next level while trying\n\
to blit the 'experience countdown' bar.  Graphics will be suppressed for now...",
				 NO_NEED_TO_INFORM, IS_WARNING_ONLY );
      return ;
    }

  //--------------------
  // Upon the very first function call, the health and force colors are not yet
  // set.  Therefore we set these colors once and for the rest of the game.
  //
  if ( experience_countdown_rect_color == 0 )
    {
      un_experience_countdown_rect_color = SDL_MapRGBA( Screen->format , 50 , 50 , 50 , 80 );
      experience_countdown_rect_color = SDL_MapRGBA( Screen->format , 255 , 255 , 120 , 80 );
    }


  //--------------------
  // We set the 'full' bar or the experience rect...
  //
  experience_countdown_rect . x = WHOLE_EXPERIENCE_COUNTDOWN_RECT_X ;
  experience_countdown_rect . y = WHOLE_EXPERIENCE_COUNTDOWN_RECT_Y ;
  // +  ( ( WHOLE_EXPERIENCE_COUNTDOWN_RECT_H * ( Me [ 0 ] . max_experience_countdown - Me [ 0 ] . experience_countdown ) ) / Me [ 0 ] . max_experience_countdown ) ;
  experience_countdown_rect . w = ( WHOLE_EXPERIENCE_COUNTDOWN_RECT_W * exp_achieved ) / exp_range ;
  if ( exp_achieved < 0 ) experience_countdown_rect . w = 0;
  experience_countdown_rect . h = WHOLE_EXPERIENCE_COUNTDOWN_RECT_H ;

  //--------------------
  // We set the 'empty' part of the experience rect...
  //
  un_experience_countdown_rect . x = experience_countdown_rect . x + experience_countdown_rect . w ;
  un_experience_countdown_rect . y = experience_countdown_rect . y ;
  un_experience_countdown_rect . w = ( WHOLE_EXPERIENCE_COUNTDOWN_RECT_W * ( exp_range - exp_achieved ) ) / exp_range ;
  un_experience_countdown_rect . h = WHOLE_EXPERIENCE_COUNTDOWN_RECT_H ;

  //--------------------
  // Now wthat all our rects are set up, we can start to display the current
  // running power status on screen...
  //
  SDL_SetClipRect( Screen , NULL );
  our_SDL_fill_rect_wrapper( Screen , & ( experience_countdown_rect ) , experience_countdown_rect_color );
  our_SDL_fill_rect_wrapper( Screen , & ( un_experience_countdown_rect ) , un_experience_countdown_rect_color );

}; // void blit_experience_countdown_bars ( void )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
blit_running_power_bars ( void )
{
  SDL_Rect running_power_rect;
  SDL_Rect un_running_power_rect;
  static Uint32 running_power_rect_color = 0 ;
  static Uint32 un_running_power_rect_color = 0 ;
  static Uint32 rest_running_power_rect_color = 0 ;

  //--------------------
  // At game startup, it might be that an uninitialized Tux (with 0 in the
  // max running power entry) is still in the data structure and when the
  // title displayes, this causes division by zero... 
  //
  if ( Me [ 0 ] . max_running_power <= 1 ) return ;

  //--------------------
  // Upon the very first function call, the health and force colors are not yet
  // set.  Therefore we set these colors once and for the rest of the game.
  //
  if ( running_power_rect_color == 0 )
    {
      un_running_power_rect_color = SDL_MapRGBA( Screen->format , 20 , 20 , 20 , 80 );
      running_power_rect_color = SDL_MapRGBA( Screen->format , 255 , 255 , 0 , 80 );
      rest_running_power_rect_color = SDL_MapRGBA( Screen->format , 255 , 20 , 20 , 80 );
    }

  if ( GameConfig . Inventory_Visible ) 
    {
      return ;
    }

  running_power_rect.x = WHOLE_RUNNING_POWER_RECT_X;
  running_power_rect.y = WHOLE_RUNNING_POWER_RECT_Y 
    +  ( ( WHOLE_RUNNING_POWER_RECT_H * ( Me [ 0 ] . max_running_power - Me [ 0 ] . running_power ) ) / Me [ 0 ] . max_running_power ) ;
  running_power_rect.w = WHOLE_RUNNING_POWER_RECT_W;
  running_power_rect.h = ( WHOLE_RUNNING_POWER_RECT_H * Me [ 0 ] . running_power ) / Me [ 0 ] . max_running_power ;
  if ( Me [ 0 ] . running_power < 0 ) running_power_rect . h = 0;

  un_running_power_rect . x = running_power_rect . x ;
  un_running_power_rect . y = WHOLE_RUNNING_POWER_RECT_Y ;
  // +  ( ( WHOLE_RUNNING_POWER_RECT_H * Me [ 0 ] . running_power ) / Me [ 0 ] . max_running_power ) ;
  un_running_power_rect . w = WHOLE_RUNNING_POWER_RECT_W;
  un_running_power_rect . h = WHOLE_RUNNING_POWER_RECT_H - 
    ( ( WHOLE_RUNNING_POWER_RECT_H * Me [ 0 ] . running_power ) / Me [ 0 ] . max_running_power ) ;
  if ( Me [ 0 ] . running_power < 0 ) un_running_power_rect . h = WHOLE_RUNNING_POWER_RECT_H ;

  //--------------------
  // Now wthat all our rects are set up, we can start to display the current
  // running power status on screen...
  //
  SDL_SetClipRect( Screen , NULL );
  if ( Me [ 0 ] . running_must_rest )
    our_SDL_fill_rect_wrapper( Screen , & ( running_power_rect ) , rest_running_power_rect_color );
  else
    our_SDL_fill_rect_wrapper( Screen , & ( running_power_rect ) , running_power_rect_color );
  our_SDL_fill_rect_wrapper( Screen , & ( un_running_power_rect ) , un_running_power_rect_color );

}; // void blit_running_power_bars ( void )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
blit_energy_and_mana_bars ( void )
{
  SDL_Rect Health_Rect;
  SDL_Rect Unhealth_Rect;
  SDL_Rect Force_Rect;
  SDL_Rect Unforce_Rect;
  SDL_Rect running_power_rect;
  SDL_Rect un_running_power_rect;
  static Uint32 health_rect_color = 0 ;
  static Uint32 force_rect_color = 0 ;
  static Uint32 un_health_rect_color = 0 ;
  static Uint32 un_force_rect_color = 0 ;
  static Uint32 running_power_rect_color = 0 ;

  //--------------------
  // Upon the very first function call, the health and force colors are not yet
  // set.  Therefore we set these colors once and for the rest of the game.
  //
  if ( health_rect_color == 0 )
    {
      health_rect_color = SDL_MapRGBA( Screen->format , 255 , 0 , 0 , 0 );
      force_rect_color = SDL_MapRGBA( Screen->format , 0 , 0 , 255 , 0 );
      un_health_rect_color = SDL_MapRGBA( Screen->format , 20 , 0 , 0 , 0 );
      un_force_rect_color = SDL_MapRGBA( Screen->format , 0 , 0 , 20 , 0 );
      running_power_rect_color = SDL_MapRGBA( Screen->format , 255 , 255 , 0 , 0 );
    }

  if ( GameConfig.Inventory_Visible ) 
    {
      Health_Rect.x = RIGHT_WHOLE_HEALTH_RECT_X;
      Force_Rect.x = RIGHT_WHOLE_FORCE_RECT_X;
      if ( GameConfig.CharacterScreen_Visible ) return;
    }
  else
    {
      Health_Rect.x = LEFT_WHOLE_HEALTH_RECT_X;
      Force_Rect.x = LEFT_WHOLE_FORCE_RECT_X;
    }

  Health_Rect.y = WHOLE_HEALTH_RECT_Y;
  Health_Rect.w = WHOLE_HEALTH_RECT_W;
  Health_Rect.h = ( WHOLE_HEALTH_RECT_H * Me[0].energy ) / Me[0].maxenergy ;
  if ( Me[0].energy < 0 ) Health_Rect.h = 0;
  Force_Rect.y = WHOLE_FORCE_RECT_Y;
  Force_Rect.w = WHOLE_FORCE_RECT_W;
  Force_Rect.h = ( WHOLE_FORCE_RECT_H * Me[0].mana ) / Me[0].maxmana ;
  if ( Me[0].mana < 0 ) Force_Rect.h = 0;

  running_power_rect.y = WHOLE_RUNNING_POWER_RECT_Y;
  running_power_rect.w = WHOLE_RUNNING_POWER_RECT_W;
  running_power_rect.h = ( WHOLE_RUNNING_POWER_RECT_H * Me [ 0 ] . running_power ) / Me [ 0 ] . max_running_power ;
  if ( Me [ 0 ] . running_power < 0 ) running_power_rect . h = 0;

  Unhealth_Rect.x = Health_Rect.x;
  Unhealth_Rect.y = WHOLE_HEALTH_RECT_Y + ( ( WHOLE_HEALTH_RECT_H * Me[0].energy ) / Me[0].maxenergy ) ;
  Unhealth_Rect.w = WHOLE_HEALTH_RECT_W;
  Unhealth_Rect.h = WHOLE_HEALTH_RECT_H - ( ( WHOLE_HEALTH_RECT_H * Me[0].energy ) / Me[0].maxenergy ) ;
  if ( Unhealth_Rect.h > WHOLE_HEALTH_RECT_H ) Unhealth_Rect.h = 0;
  Unforce_Rect.x = Force_Rect.x;
  Unforce_Rect.y = WHOLE_FORCE_RECT_Y + ( ( WHOLE_FORCE_RECT_H * Me[0].mana ) / Me[0].maxmana ) ;
  Unforce_Rect.w = WHOLE_FORCE_RECT_W;
  Unforce_Rect.h = WHOLE_FORCE_RECT_H - ( ( WHOLE_FORCE_RECT_H * Me[0].mana ) / Me[0].maxmana ) ;
  if ( Unforce_Rect.h > WHOLE_FORCE_RECT_H ) Unforce_Rect.h = 0;

  un_running_power_rect . x = running_power_rect . x ;
  un_running_power_rect . y = WHOLE_RUNNING_POWER_RECT_Y + 
    ( ( WHOLE_RUNNING_POWER_RECT_H * Me[0].energy ) / Me[0].maxenergy ) ;
  un_running_power_rect . w = WHOLE_RUNNING_POWER_RECT_W;
  un_running_power_rect . h = WHOLE_RUNNING_POWER_RECT_H - 
    ( ( WHOLE_RUNNING_POWER_RECT_H * Me[0].energy ) / Me[0].maxenergy ) ;
  if ( un_running_power_rect . h > WHOLE_RUNNING_POWER_RECT_H ) un_running_power_rect . h = 0;


  SDL_SetClipRect( Screen , NULL );
  our_SDL_fill_rect_wrapper( Screen , & ( Health_Rect ) , health_rect_color );
  our_SDL_fill_rect_wrapper( Screen , & ( Unhealth_Rect ) , un_health_rect_color );
  our_SDL_fill_rect_wrapper( Screen , & ( Force_Rect ) , force_rect_color );
  our_SDL_fill_rect_wrapper( Screen , & ( Unforce_Rect ) , un_force_rect_color );
  
}; // void blit_energy_and_mana_bars ( void )

/* ----------------------------------------------------------------------
 * This function displays the status bars for mana and energy in some 
 * corner of the screen.  The dimensions and location of the bar are
 * specified in items.h
 * ---------------------------------------------------------------------- */
void 
ShowCurrentHealthAndForceLevel( void )
{

  if ( GameConfig . use_bars_instead_of_energy_o_meter )
    blit_energy_and_mana_bars();
  else
    blit_energy_o_meter();

  blit_running_power_bars ( );

  blit_experience_countdown_bars ( ) ;

}; // void ShowCurrentHealthAndForceLevel( void )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
ShowCurrentTextWindow ( void )
{
  SDL_Rect Banner_Text_Rect;
  point CurPos;
  char ItemDescText[5000]=" ";
  char TextLine[10][1000];
  point inv_square;
  int InvIndex;
  int i;
  int NumberOfLinesInText = 1;
  finepoint MapPositionOfMouse;
  char* LongTextPointer;
  int InterLineDistance;
  int StringLength;
  int index_of_droid_below_mouse_cursor = GetLivingDroidBelowMouseCursor ( 0 ) ;
  int index_of_floor_item_below_mouse_cursor = ( -1 );
#define REQUIREMENTS_NOT_MET_TEXT "REQUIREMENTS NOT MET"

  //--------------------
  // During the title display phase, we need not have this window visible...
  //
  if ( Me[0].status == BRIEFING ) return;

  //--------------------
  // For testing purposes is bluntly insert the new banner element here:
  //
  // if ( GetMousePos_y( ) >= ( SCREEN_HEIGHT / 2 ) )
  if ( GetMousePos_y( ) + MOUSE_CROSSHAIR_OFFSET_Y >= ( UPPER_BANNER_TEXT_RECT_H + UPPER_BANNER_TEXT_RECT_Y ) )
    {
      Banner_Text_Rect.x = UPPER_BANNER_TEXT_RECT_X;
      Banner_Text_Rect.y = UPPER_BANNER_TEXT_RECT_Y;
      Banner_Text_Rect.w = UPPER_BANNER_TEXT_RECT_W;
      Banner_Text_Rect.h = UPPER_BANNER_TEXT_RECT_H;
    }
  else
    {
      Banner_Text_Rect.x = LOWER_BANNER_TEXT_RECT_X;
      Banner_Text_Rect.y = LOWER_BANNER_TEXT_RECT_Y;
      Banner_Text_Rect.w = LOWER_BANNER_TEXT_RECT_W;
      Banner_Text_Rect.h = LOWER_BANNER_TEXT_RECT_H;
    }

  CurPos.x = GetMousePos_x() + MOUSE_CROSSHAIR_OFFSET_X ;
  CurPos.y = GetMousePos_y() + MOUSE_CROSSHAIR_OFFSET_Y ;

  //--------------------
  // In case some item is held in hand by the player, the situation is simple:
  // we merely need to draw this items description into the description field and
  // that's it OR WE MUST SAY THAT THE requirements for this item are not met
  //
  if ( GetHeldItemPointer( ) != NULL )
    {
      if ( ItemUsageRequirementsMet( GetHeldItemPointer( ) , FALSE ) )
	strcpy( ItemDescText , ItemMap[ GetHeldItemCode() ].item_name );
      else 
	{
	  strcpy( ItemDescText , REQUIREMENTS_NOT_MET_TEXT  );
	}
    }

  //--------------------
  // in the other case however, that no item is currently held in hand, we need to
  // work a little more:  we need to find out if the cursor is currently over some
  // inventory or other item and in case that's true, we need to give the 
  // description of this item.
  //
  else if ( GameConfig.Inventory_Visible )
    {
      //--------------------
      // Perhaps the cursor is over some item of the inventory?
      // let's check this case first.
      //
      if ( CursorIsInInventoryGrid( CurPos.x , CurPos.y ) )
	{
	  inv_square.x = GetInventorySquare_x( CurPos.x );
	  inv_square.y = GetInventorySquare_y( CurPos.y );
	  // DebugPrintf( 0 , "\nInv target x: %d." , inv_square.x );
	  // DebugPrintf( 0 , "\nInv target y: %d." , inv_square.y );
	  InvIndex = GetInventoryItemAt ( inv_square.x , inv_square.y );
	  // DebugPrintf( 0 , "\nInv Index targeted: %d." , InvIndex );
	  if ( InvIndex != (-1) )
	    {
	      GiveItemDescription ( ItemDescText , &(Me[0].Inventory[ InvIndex ]) , FALSE );
	    }
	} 
      else if ( CursorIsOnButton ( WEAPON_RECT_BUTTON , CurPos.x , CurPos.y ) )
	{
	  if ( Me[0].weapon_item.type > 0 )
	    GiveItemDescription ( ItemDescText , & ( Me[0].weapon_item ) , FALSE );
	}
      else if ( CursorIsOnButton ( DRIVE_RECT_BUTTON , CurPos.x , CurPos.y ) )
	{
	  if ( Me[0].drive_item.type > 0 )
	   GiveItemDescription ( ItemDescText , & ( Me[0].drive_item) , FALSE );
	}
      else if ( CursorIsOnButton ( SHIELD_RECT_BUTTON , CurPos.x , CurPos.y ) )
	{
	   if ( Me[0].shield_item.type > 0 )
	   GiveItemDescription ( ItemDescText , & ( Me[0].shield_item) , FALSE );
	}
      else if ( CursorIsOnButton ( ARMOUR_RECT_BUTTON , CurPos.x , CurPos.y ) )
	{
	   if ( Me[0].armour_item.type > 0 )
	   GiveItemDescription ( ItemDescText , & ( Me[0].armour_item) , FALSE );
	}
      else if ( CursorIsOnButton ( AUX1_RECT_BUTTON , CurPos.x , CurPos.y ) )
	{
	   if ( Me[0].aux1_item.type > 0 )
	   GiveItemDescription ( ItemDescText , & ( Me[0].aux1_item) , FALSE );
	}
      else if ( CursorIsOnButton ( AUX2_RECT_BUTTON , CurPos.x , CurPos.y ) )
	{
	   if ( Me[0].aux2_item.type > 0 )
	   GiveItemDescription ( ItemDescText , & ( Me[0].aux2_item) , FALSE );
	}
      else if ( CursorIsOnButton ( HELMET_RECT_BUTTON , CurPos.x , CurPos.y ) )
	{
	   if ( Me[0].special_item.type > 0 )
	   GiveItemDescription ( ItemDescText , & ( Me[0].special_item) , FALSE );
	}

    } // if nothing is 'held in hand' && inventory-screen visible

  //--------------------
  // If the mouse cursor is within the user rectangle, then we check if
  // either the cursor is over an inventory item or over some other droid
  // and in both cases, we give a description of the object in the small
  // black rectangle in the top status banner.
  //

  if ( CursorIsInUserRect( CurPos.x , CurPos.y ) && ( CurLevel != NULL ) )
    {
      // DebugPrintf( 2  , "\nCursor is in userfenster... --> see if hovering over an item...");
      // MapPositionOfMouse.x = Me[0].pos.x + (float)(CurPos.x - UserCenter_x) / (float) Block_Width;
      // MapPositionOfMouse.y = Me[0].pos.y + (float)(CurPos.y - UserCenter_y) / (float) Block_Height;
      // DebugPrintf( 2  , "\nMouse in map at: %f %f." , MapPositionOfMouse.x , MapPositionOfMouse.y );

      MapPositionOfMouse.x = translate_pixel_to_map_location ( 0 , 
							       (float) ServerThinksInputAxisX ( 0 ) , 
							       (float) ServerThinksInputAxisY ( 0 ) , TRUE ) ;
      MapPositionOfMouse.y = translate_pixel_to_map_location ( 0 , 
							       (float) ServerThinksInputAxisX ( 0 ) , 
							       (float) ServerThinksInputAxisY ( 0 ) , FALSE ) ;


      index_of_floor_item_below_mouse_cursor = get_floor_item_index_under_mouse_cursor ( 0 );

      if ( index_of_floor_item_below_mouse_cursor != (-1) )
	GiveItemDescription ( ItemDescText , & ( CurLevel -> ItemList [ index_of_floor_item_below_mouse_cursor ] ) , 
			      FALSE );

      //--------------------
      // Maybe the cursor in the user rect is hovering right over a closed chest.
      // In this case we say so in the top status banner.
      //
      if ( closed_chest_below_mouse_cursor ( 0 ) != (-1) )
	{
	  strcpy ( ItemDescText , "  C  H  E  S  T  ! ! ! " ); 
	}

      //--------------------
      // Maybe the cursor in the user rect is hovering right over a closed chest.
      // In this case we say so in the top status banner.
      //
      if ( smashable_barred_below_mouse_cursor ( 0 ) != (-1) )
	{
	  strcpy ( ItemDescText , "  B  A  R  R  E  L  ! ! ! " ); 
	}

      //--------------------
      // Maybe there is a teleporter event connected to the square where the mouse
      // cursor is currently hovering.  In this case we should create a message about
      // where the teleporter connection would bring the Tux...
      //
      if ( teleporter_square_below_mouse_cursor ( 0 , ItemDescText ) )
	{
	  //--------------------
	  // Do nothing here, 'cause the function above has filled in the proper
	  // text already...
	  //
	}

      //--------------------
      // Maybe there is a living droid below the current mouse cursor.  In this
      // case, we'll give the decription of the corresponding bot.  (Also this serves
      // as a good way to check whether the 'droid below mouse cursor' functions are
      // doing a good job or not.
      //
      if ( index_of_droid_below_mouse_cursor != (-1) )
	{
	  create_and_blit_droid_description ( index_of_droid_below_mouse_cursor ) ;
	  return;
	}
    }

  SDL_SetClipRect( Screen , NULL );  // this unsets the clipping rectangle
  if ( strlen( ItemDescText ) > 1 )
    {
      if ( use_open_gl ) 
	GL_HighlightRectangle ( Screen , Banner_Text_Rect , 0 , 0 , 0 , 160 );
      else
	our_SDL_fill_rect_wrapper( Screen , &Banner_Text_Rect , BANNER_TEXT_REC_BACKGROUNDCOLOR );
    }

  if ( strcmp ( ItemDescText , REQUIREMENTS_NOT_MET_TEXT ) == 0 )
    {
      SetCurrentFont( Red_BFont );
    }
  else
    {
      SetCurrentFont( FPS_Display_BFont );
    }

  //--------------------
  // Now we count how many lines are to be printed
  //
  NumberOfLinesInText = 1 + CountStringOccurences ( ItemDescText , "\n" ) ;

  //--------------------
  // Now we separate the lines and fill them into the line-array
  //
  InterLineDistance = ( UPPER_BANNER_TEXT_RECT_H - NumberOfLinesInText * FontHeight( GetCurrentFont() ) ) / 
    ( NumberOfLinesInText + 1 );

  LongTextPointer = ItemDescText;
  for ( i = 0 ; i < NumberOfLinesInText-1 ; i ++ )
    {
      StringLength = strstr( LongTextPointer , "\n" ) - LongTextPointer ;

      strncpy ( TextLine[ i ] , LongTextPointer , StringLength );
      TextLine [ i ] [ StringLength ] = 0;

      LongTextPointer += StringLength + 1;
      CenteredPutString ( Screen , Banner_Text_Rect.y + InterLineDistance + 
			  i * ( InterLineDistance + FontHeight( GetCurrentFont() ) ) , TextLine[ i ] );
    }
  CenteredPutString ( Screen , Banner_Text_Rect.y + InterLineDistance + 
		      i * ( InterLineDistance + FontHeight( GetCurrentFont() ) ) , LongTextPointer );

}; // void ShowCurrentTextWindow ( void )


/* ----------------------------------------------------------------------
 * This function updates the various displays that are usually blitted
 * right into the combat window, like energy and status meter and that...
 * ---------------------------------------------------------------------- */
void
DisplayBanner (const char* left, const char* right,  int flags )
{

  SDL_SetClipRect( Screen , NULL ); 

  if ( ( ! GameConfig.CharacterScreen_Visible ) && ( ! GameConfig.SkillScreen_Visible ) )
    ShowCurrentHealthAndForceLevel ( );

  ShowCurrentTextWindow ( );
  ShowCurrentSkill ( );

}; // void DisplayBanner( .. ) 


#undef _rahmen_c
