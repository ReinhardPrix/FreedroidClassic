/* 
 *
 *   Copyright (c) 1994, 2002, 2003, 2004 Johannes Prix
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

#define _hud_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "proto.h"
#include "global.h"

#include "SDL_rotozoom.h" // that's for rotating the speed-o-meter arrows

//--------------------
// Here we define the dimensions of the banner rectangle.  Note,
// that some items have a lot of modifiers and magic boni, therefore
// they need a lot of lines, so 120 really seems reasonable.  If you
// want less, take care that the uppermost line is not omitted, a 
// bug that only occurs when very little space is given for those
// rectangles...
//
#define UPPER_BANNER_TEXT_RECT_X (GameConfig . screen_width/4)
#define UPPER_BANNER_TEXT_RECT_Y 1
#define UPPER_BANNER_TEXT_RECT_W (GameConfig . screen_width/3)
#define UPPER_BANNER_TEXT_RECT_H 130
#define LOWER_BANNER_TEXT_RECT_X UPPER_BANNER_TEXT_RECT_X
#define LOWER_BANNER_TEXT_RECT_Y ( GameConfig . screen_height - UPPER_BANNER_TEXT_RECT_Y - UPPER_BANNER_TEXT_RECT_H )
#define LOWER_BANNER_TEXT_RECT_W UPPER_BANNER_TEXT_RECT_W
#define LOWER_BANNER_TEXT_RECT_H UPPER_BANNER_TEXT_RECT_H
#define BANNER_TEXT_REC_BACKGROUNDCOLOR 0x00

extern char *InfluenceModeNames[];

int best_banner_pos_x, best_banner_pos_y;
char* game_message_protocol = NULL ;

/* ----------------------------------------------------------------------
 * The hud contains several status graphs.  These graphs appear as 
 * vertical columns, that are more or less filled, like liquid in a tube.
 * Since these appear multiple times, it appears sensible to make a 
 * function to draw such bars in a convenient way, which is what this
 * function is supposed to do.
 * ---------------------------------------------------------------------- */
void
blit_vertical_status_bar ( float max_value , float current_value , Uint32 filled_color_code ,
			   Uint32 empty_color_code , int x , int y , int w , int h , int scale_to_screen_resolution )
{
    SDL_Rect running_power_rect;
    SDL_Rect un_running_power_rect;

    //--------------------
    // Now we might get the case of current value exceeding the max value by far.  This 
    // does happen when we set ridiculously high energy values for invincible Tux in the
    // course of testing and convenient debugging.  To prevent arithmetic exceptions, we
    // set precautions to allow for a maximum of 3 times the full scale to represent 
    // extremely high (energy) values.
    //
    if ( current_value > 3 * max_value ) current_value = 3 * max_value;

    running_power_rect.x = x ;
    running_power_rect.y = y 
	+  ( ( h * ( max_value - current_value ) ) / max_value ) ;
    running_power_rect.w = w ;
    running_power_rect.h = ( h * current_value ) / max_value ;
    if ( current_value < 0 ) running_power_rect . h = 0;
    
    un_running_power_rect . x = running_power_rect . x ;
    un_running_power_rect . y = y ;
    un_running_power_rect . w = w ;
    un_running_power_rect . h = h - 
	( ( h * current_value ) / max_value ) ;
    if ( current_value < 0 ) un_running_power_rect . h = h ;
    if ( current_value > max_value ) un_running_power_rect . h = 0 ;
    
    //--------------------
    // If we are to scale the status bar, we do so :)
    //
    if ( scale_to_screen_resolution )
    {
	un_running_power_rect . x =
	    un_running_power_rect . x * GameConfig.screen_width / 640.0 ;
	un_running_power_rect . y = 
	    un_running_power_rect . y * GameConfig.screen_height / 480.0 ;
	un_running_power_rect . w = 
	    un_running_power_rect . w * GameConfig.screen_width / 640.0 ;
	un_running_power_rect . h = 
	    un_running_power_rect . h * GameConfig.screen_height / 480.0 ;
	running_power_rect . x =
	    running_power_rect . x * GameConfig.screen_width / 640.0 ;
	running_power_rect . y = 
	    running_power_rect . y * GameConfig.screen_height / 480.0 ;
	running_power_rect . w =
	    running_power_rect . w * GameConfig.screen_width / 640.0 ;
	running_power_rect . h = 
	    running_power_rect . h * GameConfig.screen_height / 480.0 ;
    }

    //--------------------
    // Now wthat all our rects are set up, we can start to display the current
    // running power status on screen...
    //
    SDL_SetClipRect( Screen , NULL );
    our_SDL_fill_rect_wrapper( Screen , & ( running_power_rect ) , filled_color_code );
    our_SDL_fill_rect_wrapper( Screen , & ( un_running_power_rect ) , empty_color_code );

}; // void blit_vertical_status_bar ( ... )

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
	GiveStandardErrorMessage ( __FUNCTION__  , "\
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

    if ( ItemMap [ CurItem->type ] . item_gun_ammo_clip_size )
    {
	sprintf( linebuf , "Ammo : %d of %d\n", CurItem->ammo_clip,  ItemMap [ CurItem->type ] . item_gun_ammo_clip_size);
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
	strcat( ItemDescText , " Indestructible" );
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
		sprintf( linebuf , "   Str: %d" , ItemMap[ CurItem->type ].item_require_strength );
	    else 
		sprintf( linebuf , " Required strength: %d " , ItemMap[ CurItem->type ].item_require_strength );
	    strcat( ItemDescText , linebuf );
	}
	if ( ItemMap[ CurItem->type ].item_require_dexterity != ( -1 ) )
	{
	    if ( ForShop )
		sprintf( linebuf , "   Dex: %d" ,  ItemMap[ CurItem->type ].item_require_dexterity );
	    else
		sprintf( linebuf , " Dexterity: %d " ,  ItemMap[ CurItem->type ].item_require_dexterity );
	    strcat( ItemDescText , linebuf );
	}
    }
    else if ( ItemMap [ CurItem->type ] . item_can_be_applied_in_combat )
    {
	//--------------------
	// Maybe it's an applicable item, that still has some stat
	// requirements.  Typically spellbooks fall into that category.
	//
	switch ( CurItem -> type )
	{
	    case ITEM_SPELLBOOK_OF_HEALING:
	    case ITEM_SPELLBOOK_OF_EXPLOSION_CIRCLE:
	    case ITEM_SPELLBOOK_OF_EXPLOSION_RAY:
	    case ITEM_SPELLBOOK_OF_TELEPORT_HOME:
	    case ITEM_SPELLBOOK_OF_DETECT_ITEMS:
	    case ITEM_SPELLBOOK_OF_IDENTIFY:
	    case ITEM_SPELLBOOK_OF_PLASMA_BOLT:
	    case ITEM_SPELLBOOK_OF_ICE_BOLT:
	    case ITEM_SPELLBOOK_OF_POISON_BOLT:
	    case ITEM_SPELLBOOK_OF_PETRIFICATION:
	    case ITEM_SPELLBOOK_OF_RADIAL_EMP_WAVE:
	    case ITEM_SPELLBOOK_OF_RADIAL_VMX_WAVE:
	    case ITEM_SPELLBOOK_OF_RADIAL_PLASMA_WAVE:

		sprintf( linebuf , "Spellcasting skill: %s\n " ,  
			 AllSkillTexts [ required_spellcasting_skill_for_item ( CurItem -> type ) ] );
		strcat( ItemDescText , linebuf );
		sprintf( linebuf , "Magic: %d\n " ,  
			 required_magic_stat_for_next_level_and_item ( CurItem -> type ) );
		strcat( ItemDescText , linebuf );
		break;
	    default:
		break;
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
		
	    case ITEM_SCRIPT_OF_IDENTIFY:
	    case ITEM_SCRIPT_OF_TELEPORT_HOME:
		sprintf( linebuf , "Invokes a spell/skill once\n" );
		strcat( ItemDescText , linebuf );
		break;
		
	    case ITEM_MEDIUM_MANA_POTION:
	    case ITEM_FULL_MANA_POTION:
	    case ITEM_SMALL_MANA_POTION:
		sprintf( linebuf , "Recover Force\n" );
		strcat( ItemDescText , linebuf );
		break;

	    case ITEM_BLUE_ENERGY_DRINK:
		sprintf( linebuf , "Recover Health, Force\nand Running Power\n" );
		strcat( ItemDescText , linebuf );
		break;
		
	    case ITEM_RUNNING_POWER_POTION:
		sprintf( linebuf , "Recover Running Power\n" );
		strcat( ItemDescText , linebuf );
		break;
		
	    case ITEM_TEMP_STRENGTH_POTION:
		sprintf( linebuf , "Temporary Boost to Strength\n" );
		strcat( ItemDescText , linebuf );
		break;
		
	    case ITEM_TEMP_DEXTERITY_POTION:
		sprintf( linebuf , "Temporary Boost to Dexterity\n" );
		strcat( ItemDescText , linebuf );
		break;
		
	    case ITEM_MAP_MAKER_SIMPLE:
		sprintf( linebuf , "To implant the automap device\n" );
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
		sprintf( linebuf , "Permanently aquire/enhance this skill/spell\n" );
		strcat( ItemDescText , linebuf );
		break;

	    case ITEM_EMP_SHOCK_GRENADE:
    		sprintf( linebuf , "Electromagnetic pulse\n" );
		strcat( ItemDescText , linebuf );
		break;

	    case ITEM_VMX_GAS_GRENADE:
		sprintf( linebuf , "Gas attack\n" );
		strcat( ItemDescText , linebuf );
		break;

	    case ITEM_PLASMA_GRENADE:
		sprintf( linebuf , "Huge explosion\n" );
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
	    //strcat ( ItemDescText , font_switchto_red );
	    AppendToLine = 0 ;
	    
	    if ( ForShop ) strcat( ItemDescText , "             " );
	    
	    if ( CurItem->bonus_to_str )
	    {
		if ( CurItem->bonus_to_str > 0 ) strcat( ItemDescText , "+" );
		else strcat ( ItemDescText, "-");
		sprintf( linebuf , "%d to strength" , CurItem->bonus_to_str );
		AppendToLine = TRUE;
		strcat( ItemDescText , linebuf );
	    }
	    
	    if ( CurItem->bonus_to_dex )
	    {
		if ( AppendToLine ) { if ( ForShop ) strcat ( ItemDescText , ", " ); else strcat ( ItemDescText , "\n" ); };
		AppendToLine = TRUE;
		if ( CurItem->bonus_to_dex > 0 ) strcat( ItemDescText , "+" );
		sprintf( linebuf , "%d to dexterity" , CurItem->bonus_to_dex );
		strcat( ItemDescText , linebuf );
	    }
	    
	    if ( CurItem->bonus_to_mag )
	    {
		if ( AppendToLine ) { if ( ForShop ) strcat ( ItemDescText , ", " ); else strcat ( ItemDescText , "\n" ); };
		AppendToLine = TRUE;
		if ( CurItem->bonus_to_mag > 0 ) strcat( ItemDescText , "+" );
		sprintf( linebuf , "%d to magic" , CurItem->bonus_to_mag );
		strcat( ItemDescText , linebuf );
	    }
	    
	    if ( CurItem->bonus_to_vit )
	    {
		if ( AppendToLine ) { if ( ForShop ) strcat ( ItemDescText , ", " ); else strcat ( ItemDescText , "\n" ); };
		if ( CurItem->bonus_to_vit > 0 ) strcat( ItemDescText , "+" );
		AppendToLine = TRUE;
		sprintf( linebuf , "%d to vitality" , CurItem->bonus_to_vit );
		strcat( ItemDescText , linebuf );
	    }
	    
	    if ( CurItem->bonus_to_life )
	    {
		if ( AppendToLine ) { if ( ForShop ) strcat ( ItemDescText , ", " ); else strcat ( ItemDescText , "\n" ); };
		AppendToLine = TRUE;
		if ( CurItem->bonus_to_life > 0 ) strcat( ItemDescText , "+" );
		sprintf( linebuf , "%d health points" , CurItem->bonus_to_life );
		strcat( ItemDescText , linebuf );
	    }

	    if ( CurItem->bonus_to_health_recovery )
	    {
		if ( AppendToLine ) { if ( ForShop ) strcat ( ItemDescText , ", " ); else strcat ( ItemDescText , "\n" ); };
		AppendToLine = TRUE;
		if ( CurItem->bonus_to_health_recovery > 0 ) strcat( ItemDescText , "+" );
		sprintf( linebuf , "%0.1f health points recovered per second" , CurItem->bonus_to_health_recovery );
		strcat( ItemDescText , linebuf );
	    }

	    if ( CurItem->bonus_to_mana_recovery )
	    {
		if ( AppendToLine ) { if ( ForShop ) strcat ( ItemDescText , ", " ); else strcat ( ItemDescText , "\n" ); };
		AppendToLine = TRUE;
		if ( CurItem->bonus_to_mana_recovery > 0 ) strcat( ItemDescText , "+" );
		sprintf( linebuf , "%0.1f mana points recovered per second" , CurItem->bonus_to_mana_recovery );
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

	    if ( CurItem -> bonus_to_resist_disruptor )
	    {
		if ( AppendToLine ) { if ( ForShop ) strcat ( ItemDescText , ", " ); else strcat ( ItemDescText , "\n" ); };
		if ( CurItem->bonus_to_all_attributes > 0 ) strcat( ItemDescText , "+" );
		AppendToLine = TRUE;
		sprintf( linebuf , "+%d to resist disruptor" , CurItem->bonus_to_resist_disruptor );
		strcat( ItemDescText , linebuf );
	    }

	    if ( CurItem -> bonus_to_resist_fire )
	    {
		if ( AppendToLine ) { if ( ForShop ) strcat ( ItemDescText , ", " ); else strcat ( ItemDescText , "\n" ); };
		if ( CurItem->bonus_to_all_attributes > 0 ) strcat( ItemDescText , "+" );
		AppendToLine = TRUE;
		sprintf( linebuf , "+%d to resist fire" , CurItem->bonus_to_resist_fire );
		strcat( ItemDescText , linebuf );
	    }
	    
	    if ( CurItem -> bonus_to_resist_electricity )
	    {
		if ( AppendToLine ) { if ( ForShop ) strcat ( ItemDescText , ", " ); else strcat ( ItemDescText , "\n" ); };
		if ( CurItem->bonus_to_all_attributes > 0 ) strcat( ItemDescText , "+" );
		AppendToLine = TRUE;
		sprintf( linebuf , "+%d to resist electricity" , CurItem->bonus_to_resist_electricity );
		strcat( ItemDescText , linebuf );
	    }
	    
	    
	    //--------------------
	    // Maybe this item will give some bonus to the light radius?
	    // (That is a very special case, because light bonuses are 
	    // currently attached to the suffix/prefix, not to the item 
	    // itself, so they also have no randomness...)
	    //
	    if ( CurItem -> prefix_code != (-1) )
	    {
		if ( PrefixList [ CurItem -> prefix_code ] . light_bonus_value ) 
		{
		    if ( AppendToLine ) { if ( ForShop ) strcat ( ItemDescText , ", " ); else strcat ( ItemDescText , "\n" ); };
		    strcat( ItemDescText , "+" );
		    AppendToLine = TRUE;
		    sprintf( linebuf , "%d to light radius" , PrefixList [ CurItem -> prefix_code ] . light_bonus_value );
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
    temp_fill_rect . h = FontHeight ( BFont_to_use ) ;
    temp_fill_rect . w = ( text_length * cur_enemy -> energy ) / Druidmap [ cur_enemy -> type ] . maxenergy ;
    if ( cur_enemy -> energy <= 0 ) temp_fill_rect . w = 0 ;

    //--------------------
    // Instead of having a centered top bar, we now move to have a bar right
    // over the character in question...
    //
    // temp_fill_rect . y = 50 ;
    // temp_fill_rect . x = UserCenter_x - text_length / 2 ;
    //
    temp_fill_rect . x = translate_map_point_to_screen_pixel ( cur_enemy -> pos . x , cur_enemy -> pos . y , TRUE ) - text_length / 2 ;;
    temp_fill_rect . y = translate_map_point_to_screen_pixel ( cur_enemy -> pos . x , cur_enemy -> pos . y , FALSE ) + enemy_iso_images [ AllEnemys [ enemy_num ] . type ] [ 0 ] [ 0 ] . offset_y - 2.5 * FontHeight ( BFont_to_use ) ;
    
    //--------------------
    // If the 'enemy' is hostile, then we use red underlying color.  If
    // it's a friendly entity, then we use green underlying color for the
    // enemy's name.
    //
    if ( ! cur_enemy -> is_friendly ) 
    {
	if ( use_open_gl )
	{
	    GL_HighlightRectangle ( Screen , temp_fill_rect , 0x99 , 0x00 , 0 , BACKGROUND_TEXT_RECT_ALPHA );
	}
	else
	{
	    our_SDL_fill_rect_wrapper ( Screen , & ( temp_fill_rect ) , SDL_MapRGB ( Screen->format , 0x099 , 0x000 , 0x000 ) );
	}
    }
    else
    {
	if ( use_open_gl )
	{
	    GL_HighlightRectangle ( Screen , temp_fill_rect , 0 , 0x55 , 0 , BACKGROUND_TEXT_RECT_ALPHA );
	}
	else
	{
	    our_SDL_fill_rect_wrapper ( Screen , & ( temp_fill_rect ) , SDL_MapRGB ( Screen->format , 0x000 , 0x055 , 0x000 ) );
	}
    }

    //--------------------
    // Now depending on the energy status of this enemy, there will
    // also be some dark area under the short description text
    //
    // We take some extra precautions here to prevent some unsigned
    // int underflow (and ugly too wide rectangles...)
    //
    temp_fill_rect . x = temp_fill_rect . x + temp_fill_rect . w ;
    if ( temp_fill_rect . w < text_length )
	temp_fill_rect . w = text_length - temp_fill_rect . w ;
    else
	temp_fill_rect . w = 0 ;
    our_SDL_fill_rect_wrapper ( Screen , & ( temp_fill_rect ) , SDL_MapRGB ( Screen->format , 0x000 , 0x000 , 0x000 ) );
    
    //--------------------
    // Now we can blit the actual droid short description text
    //
    // temp_fill_rect . x = UserCenter_x - text_length / 2 ;
    temp_fill_rect . x = translate_map_point_to_screen_pixel ( cur_enemy -> pos . x , cur_enemy -> pos . y , TRUE ) - text_length / 2 ; 
    PutStringFont ( Screen , BFont_to_use , temp_fill_rect . x , temp_fill_rect . y , 
		    AllEnemys [ enemy_num ] . short_description_text );
    
}; // void GiveDroidDescription ( char* ItemDescText , item* CurItem )


/* ----------------------------------------------------------------------
 * This function displays the icon of the current readied skill 
 * The dimensions and location of the picture are
 * specified in itemdefs.h
 * ---------------------------------------------------------------------- */
void 
ShowCurrentSkill( void )
{
    SDL_Rect Target_Rect;
    static int Mouse_Button_Pressed_Previous_Frame = FALSE;
    
    if ( Me [ 0 ] . status == BRIEFING ) return;
    if ( GameConfig . Inventory_Visible && GameConfig . screen_width == 640 ) return;
    if ( GameConfig . SkillScreen_Visible && GameConfig . skill_explanation_screen_visible ) return;

    Target_Rect.x = UNIVERSAL_COORD_W(CURRENT_SKILL_RECT_X) + (CURRENT_SKILL_RECT_W * GameConfig . screen_width / 640 - CURRENT_SKILL_RECT_W) / 2;
    Target_Rect.y = CURRENT_SKILL_RECT_Y + (CURRENT_SKILL_RECT_H * GameConfig . screen_height / 480 - CURRENT_SKILL_RECT_H) / 2;
    Target_Rect.w = CURRENT_SKILL_RECT_W;
    Target_Rect.h = CURRENT_SKILL_RECT_H;
    
    LoadOneSkillSurfaceIfNotYetLoaded ( Me [ 0 ] . readied_skill );

    if ( use_open_gl )
    {
	blit_open_gl_texture_to_screen_position ( SpellSkillMap [ Me[0].readied_skill ] . spell_skill_icon_surface , Target_Rect . x , Target_Rect . y , TRUE ) ;
    }
    else
	our_SDL_blit_surface_wrapper ( SpellSkillMap [ Me[0].readied_skill ] . spell_skill_icon_surface . surface , NULL , Screen , &Target_Rect );
    
  Mouse_Button_Pressed_Previous_Frame = axis_is_active;

}; // void ShowCurrentSkill ( void )

/* ----------------------------------------------------------------------
 * This function displays the icon of the current readied weapon, 
 * and the state of the charger
 * The dimensions and location of the picture are
 * specified in itemdefs.h
 * ---------------------------------------------------------------------- */
void 
ShowCurrentWeapon( void )
{
    SDL_Rect Target_Rect;
    static int Mouse_Button_Pressed_Previous_Frame = FALSE;
    char current_ammo[10];
    if ( Me [ 0 ] . status == BRIEFING ) return;
    if ( GameConfig . Inventory_Visible && GameConfig . screen_width == 640 ) return;
    if ( Me [ 0 ] . weapon_item . type == -1 ) return;

    Target_Rect.x = CURRENT_WEAPON_RECT_X + (CURRENT_WEAPON_RECT_W * GameConfig . screen_width / 640 - CURRENT_WEAPON_RECT_W) / 2;
    Target_Rect.y = CURRENT_WEAPON_RECT_Y + (CURRENT_WEAPON_RECT_H * GameConfig . screen_height / 480 - CURRENT_WEAPON_RECT_H) / 2;
    Target_Rect.w = CURRENT_WEAPON_RECT_W;
    Target_Rect.h = CURRENT_WEAPON_RECT_H;
    
    our_SDL_blit_surface_wrapper ( ItemMap [ Me[0].weapon_item . type ] . inv_image . Surface , NULL , Screen , &Target_Rect );
    
    Mouse_Button_Pressed_Previous_Frame = axis_is_active;
	
    //now we create and blit the ammo count
    if ( ItemMap[ Me [ 0 ] . weapon_item . type ] . item_gun_angle_change != 0 )
	return; //Melee weapons don't have ammunition [yet]

    if (Me [ 0 ] . busy_type == WEAPON_RELOAD)
	sprintf(current_ammo, "reloading");
    else if (! Me [ 0 ] . weapon_item . ammo_clip )
	sprintf(current_ammo, " %sEMPTY", font_switchto_red );
	else sprintf(current_ammo, "%2d / %2d", Me [ 0 ] . weapon_item . ammo_clip, ItemMap [ Me [ 0 ] . weapon_item . type ] . item_gun_ammo_clip_size);
    PutStringFont( Screen ,  FPS_Display_BFont , Target_Rect.x , Target_Rect.y + 50 , current_ammo );

}; // void ShowCurrentWeapon ( void )

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
	SpeedMeterRect.x = GameConfig . screen_width - speed_meter_iso_image . surface ->w;
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
						  GameConfig . screen_width - speed_meter_iso_image . original_image_width , 0 , FALSE );
	
	blit_rotated_open_gl_texture_with_center ( SpeedMeterEnergyArrowImage , 
						   GameConfig . screen_width - speed_meter_iso_image . original_image_width + PivotPosition . x , 
						   0 + PivotPosition . y  , 
						   - 360 * 3 / 4 * Me [ 0 ] . energy / Me [ 0 ] . maxenergy );
	
	blit_rotated_open_gl_texture_with_center ( SpeedMeterManaArrowImage , 
						   GameConfig . screen_width - speed_meter_iso_image . original_image_width + PivotPosition . x , 
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
	    
	    ArrowRect.x = PivotPosition.x - ( RotatedArrow -> w / 2 ) ;
	    ArrowRect.y = PivotPosition.y - ( RotatedArrow -> h / 2 ) ;
	    our_SDL_blit_surface_wrapper( RotatedArrow , NULL , SpeedOMeterWorkingCopy , & ArrowRect );
	    SDL_FreeSurface( RotatedArrow );
	    
	    // We blit in the blue arrow, showing current mana
	    RotatedArrow = rotozoomSurface( SpeedMeterManaArrowImage . surface , 
					    -360 * 3 / 4 * Me[0].mana / Me[0].maxmana , 1.0 , FALSE );
	    
	    ArrowRect.x = PivotPosition.x - ( RotatedArrow -> w / 2 ) ;
	    ArrowRect.y = PivotPosition.y - ( RotatedArrow -> h / 2 ) ;
	    our_SDL_blit_surface_wrapper( RotatedArrow , NULL , SpeedOMeterWorkingCopy , & ArrowRect );
	    SDL_FreeSurface( RotatedArrow );
	    
	    //--------------------
	    // And of course we must remember the current values, so that
	    // we can detect any changes when this function is called during 
	    // the next frame.
	    //
	    Previous_Energy    = Me [ 0 ] . energy ;
	    Previous_Mana      = Me [ 0 ] . mana ;
	    Previous_Maxenergy = Me [ 0 ] . maxenergy ;
	    Previous_Maxmana   = Me [ 0 ] . maxmana ;
	    
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
 * The experience needed for the next level and the experience achieved
 * already to gain the next level can be seen from an experience countdown
 * bar on (top of the) screen.  We draw it here.
 * ---------------------------------------------------------------------- */
void
blit_experience_countdown_bars ( void )
{
    static Uint32 experience_countdown_rect_color = 0 ;
    static Uint32 un_experience_countdown_rect_color = 0 ;
    int exp_range = Me [ 0 ] . ExpRequired - Me [ 0 ] . ExpRequired_previously ;
    int exp_achieved = Me [ 0 ] . Experience - Me [ 0 ] . ExpRequired_previously ;
    
    if ( GameConfig . Inventory_Visible  && GameConfig . screen_width == 640 )
    {
        return ;
    }

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
	DebugPrintf ( 1 , "\nblit_experience_countdown_bars(...)\n\
The current experience of the Tux is higher than the next level while trying\n\
to blit the 'experience countdown' bar.  Graphics will be suppressed for now..." );
	return ;
    }

    //--------------------
    // Upon the very first function call, the health and force colors are not yet
    // set.  Therefore we set these colors once and for the rest of the game.
    //
    if ( experience_countdown_rect_color == 0 )
    {
	un_experience_countdown_rect_color = SDL_MapRGBA( Screen->format , 50 , 50 , 50 , 80 );
	experience_countdown_rect_color = SDL_MapRGBA( Screen->format , 255 , 120 , 120 , 80 );
    }
    
    blit_vertical_status_bar ( exp_range , exp_achieved , 
			       experience_countdown_rect_color , un_experience_countdown_rect_color ,
			       WHOLE_EXPERIENCE_COUNTDOWN_RECT_X , 
			       WHOLE_EXPERIENCE_COUNTDOWN_RECT_Y , 
			       WHOLE_EXPERIENCE_COUNTDOWN_RECT_W ,  
			       WHOLE_EXPERIENCE_COUNTDOWN_RECT_H , TRUE );

}; // void blit_experience_countdown_bars ( void )

/* ----------------------------------------------------------------------
 * The tux has a limited running ability.
 *
 * ---------------------------------------------------------------------- */
void
blit_running_power_bars ( void )
{
    static Uint32 running_power_rect_color = 0 ;
    static Uint32 un_running_power_rect_color = 0 ;
    static Uint32 rest_running_power_rect_color = 0 ;
    static Uint32 infinite_running_power_rect_color = 0 ;

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
	infinite_running_power_rect_color = SDL_MapRGBA( Screen->format , 255 , 255 , 255 , 80 );
    }
    
    if ( GameConfig . Inventory_Visible  && GameConfig . screen_width == 640 ) 
    {
	return ;
    }
    
    //--------------------
    // Now wthat all our rects are set up, we can start to display the current
    // running power status on screen...
    //
    SDL_SetClipRect( Screen , NULL );
    if ( curShip . AllLevels [ Me [ 0 ] . pos . z ] -> infinite_running_on_this_level )
    {
	blit_vertical_status_bar ( 2.0 , 2.0 , infinite_running_power_rect_color ,
				   un_running_power_rect_color , 
				   WHOLE_RUNNING_POWER_RECT_X ,  
				   WHOLE_RUNNING_POWER_RECT_Y , 
				   WHOLE_RUNNING_POWER_RECT_W ,  
				   WHOLE_RUNNING_POWER_RECT_H , TRUE );
    }
    else
    {
	if ( Me [ 0 ] . running_must_rest )
	    blit_vertical_status_bar ( Me [ 0 ] . max_running_power , Me [ 0 ] . running_power , 
				       rest_running_power_rect_color ,
				       un_running_power_rect_color , 
				       WHOLE_RUNNING_POWER_RECT_X ,
				       WHOLE_RUNNING_POWER_RECT_Y ,
				       WHOLE_RUNNING_POWER_RECT_W ,
				       WHOLE_RUNNING_POWER_RECT_H , TRUE );
	else
	    blit_vertical_status_bar ( Me [ 0 ] . max_running_power , Me [ 0 ] . running_power , 
				       running_power_rect_color ,
				       un_running_power_rect_color , 
				       WHOLE_RUNNING_POWER_RECT_X , 
				       WHOLE_RUNNING_POWER_RECT_Y , 
				       WHOLE_RUNNING_POWER_RECT_W , 
				       WHOLE_RUNNING_POWER_RECT_H , TRUE );
    }

}; // void blit_running_power_bars ( void )

/* ----------------------------------------------------------------------
 * Basically there are currently two methods of displaying the current
 * energy and mana of the Tux.  One method is to use the energy-o-meter,
 * an analog energy/mana display.  
 * The other method is to use classic energy bars.  This function is here
 * to provide the energy bars if desired.
 * ---------------------------------------------------------------------- */
void
blit_energy_and_mana_bars ( void )
{
    static Uint32 health_rect_color = 0 ;
    static Uint32 force_rect_color = 0 ;
    static Uint32 un_health_rect_color = 0 ;
    static Uint32 un_force_rect_color = 0 ;

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
    }

    blit_vertical_status_bar ( Me[0].maxenergy , Me[0].energy , 
			       health_rect_color , un_health_rect_color ,
			       WHOLE_HEALTH_RECT_X , 
			       WHOLE_HEALTH_RECT_Y , 
			       WHOLE_HEALTH_RECT_W ,  
			       WHOLE_HEALTH_RECT_H , TRUE );
    
    blit_vertical_status_bar ( Me[0].maxmana , Me[0].mana , 
			       force_rect_color , un_force_rect_color ,
			       WHOLE_FORCE_RECT_X , 
			       WHOLE_FORCE_RECT_Y , 
			       WHOLE_FORCE_RECT_W ,  
			       WHOLE_FORCE_RECT_H , TRUE );
    
}; // void blit_energy_and_mana_bars ( void )

/* ----------------------------------------------------------------------
 * This function displays the status bars for mana and energy in some 
 * corner of the screen.  The dimensions and location of the bar are
 * specified in items.h
 * ---------------------------------------------------------------------- */
void 
ShowCurrentHealthAndForceLevel( void )
{

    //--------------------
    // When running as a game server, there's no need to display 
    // the energy or mana levels or the like...
    //
    if ( ServerMode ) return;

    if ( GameConfig . screen_width != 640 || (( ! GameConfig.CharacterScreen_Visible ) && ( ! GameConfig.SkillScreen_Visible )) )
	{
	if ( GameConfig . use_bars_instead_of_energy_o_meter )
		blit_energy_and_mana_bars();
	else
		blit_energy_o_meter();
	}

    blit_running_power_bars ( );
    
    blit_experience_countdown_bars ( ) ;

}; // void ShowCurrentHealthAndForceLevel( void )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
int
teleporter_square_below_mouse_cursor ( int player_num , char* ItemDescText )
{
    finepoint MapPositionOfMouse;
    int i;
    int action_number;
    
    if ( MouseCursorIsInUserRect( GetMousePos_x()  , 
				  GetMousePos_y()  ) && ( CurLevel != NULL ) )
    {
	MapPositionOfMouse . x = 
	    translate_pixel_to_map_location ( player_num , 
					      (float) ServerThinksInputAxisX ( player_num ) , 
					      (float) ServerThinksInputAxisY ( player_num ) , TRUE ) ;
	MapPositionOfMouse . y = 
	    translate_pixel_to_map_location ( player_num , 
					      (float) ServerThinksInputAxisX ( player_num ) , 
					      (float) ServerThinksInputAxisY ( player_num ) , FALSE ) ;

	for ( i = 0 ; i < MAX_EVENT_TRIGGERS ; i++ )
	{
	    if ( ( ( (int) MapPositionOfMouse . x ) != AllEventTriggers [ i ] . Influ_Must_Be_At_Point . x ) )
		continue;
	    if ( ( ( (int) MapPositionOfMouse . y ) != AllEventTriggers [ i ] . Influ_Must_Be_At_Point . y ) )
		continue;
	    if ( Me [ player_num ] . pos . z != AllEventTriggers [ i ] . Influ_Must_Be_At_Level )
		continue;
	    
	    // DebugPrintf ( -1000 , "\nSome trigger seems to be here..." );
	    
	    //--------------------
	    // Now we know, that the mouse is currently exactly over an event trigger.  The
	    // question to be answered still is whether this trigger also triggers a teleporter
	    // action or not and if yes, where the connection leads to...
	    //
	    action_number = GiveNumberToThisActionLabel ( AllEventTriggers [ i ] . TargetActionLabel ) ;
	    
	    if ( action_number == -1 ) return FALSE ;
	    
	    if ( AllTriggeredActions [ action_number ] . TeleportTargetLevel != (-1) )
	    {
		sprintf ( ItemDescText , "To %s...." , curShip . AllLevels [ AllTriggeredActions [ action_number ] . TeleportTargetLevel ] -> Levelname ) ;
		return ( TRUE );
	    }
	}
    }
    return ( FALSE );
}; // void teleporter_square_below_mouse_cursor ( int player_num , char* ItemDescText )

/* ----------------------------------------------------------------------
 * This function sets up the text, that is to appear in a bigger text
 * rectangle, possibly next to the mouse cursor, e.g. when the mouse is
 * hovering over an item or barrel or crate or teleporter.
 * ---------------------------------------------------------------------- */
void
prepare_text_window_content ( char* ItemDescText ) 
{
    point CurPos;
    point inv_square;
    int InvIndex;
    int index_of_droid_below_mouse_cursor = GetLivingDroidBelowMouseCursor ( 0 ) ;
    int index_of_floor_item_below_mouse_cursor = ( -1 );
    int index_of_chest_below_mouse_cursor = ( -1 );
    int index_of_barrel_below_mouse_cursor = ( -1 );
    finepoint MapPositionOfMouse;
    int obs_type;

#define REQUIREMENTS_NOT_MET_TEXT "REQUIREMENTS NOT MET"

    CurPos . x = GetMousePos_x()  ;
    CurPos . y = GetMousePos_y()  ;

    best_banner_pos_x = CurPos . x ;
    best_banner_pos_y = CurPos . y ;

    //--------------------
    // In case some item is held in hand by the player, the situation is simple:
    // we merely need to draw this items description into the description field and
    // that's it OR WE MUST SAY THAT THE requirements for this item are not met
    //
    if ( GetHeldItemPointer( ) != NULL )
    {
	if ( ItemUsageRequirementsMet( GetHeldItemPointer( ) , FALSE ) )
	    strcpy( ItemDescText , ItemMap [ GetHeldItemCode() ] . item_name );
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
    else if ( GameConfig . Inventory_Visible )
    {
	//--------------------
	// Perhaps the cursor is over some item of the inventory?
	// let's check this case first.
	//
	if ( MouseCursorIsInInventoryGrid( CurPos.x , CurPos.y ) )
	{
	    inv_square.x = GetInventorySquare_x( CurPos.x );
	    inv_square.y = GetInventorySquare_y( CurPos.y );
	    // DebugPrintf( 0 , "\nInv target x: %d." , inv_square.x );
	    // DebugPrintf( 0 , "\nInv target y: %d." , inv_square.y );
	    InvIndex = GetInventoryItemAt ( inv_square.x , inv_square.y );
	    // DebugPrintf( 0 , "\nInv Index targeted: %d." , InvIndex );
	    if ( InvIndex != (-1) )
	    {
		GiveItemDescription ( ItemDescText , & ( Me [ 0 ] . Inventory [ InvIndex ] ) , FALSE );
		best_banner_pos_x = (  Me [ 0 ] . Inventory [ InvIndex ] . inventory_position . x + ItemMap [ Me [ 0 ] . Inventory [ InvIndex ] . type ] . inv_image . inv_size . x ) * 30 + 16 ;
		best_banner_pos_y = 300 ;
	    }
	} 
	else if ( MouseCursorIsOnButton ( WEAPON_RECT_BUTTON , CurPos.x , CurPos.y ) )
	{
	    if ( Me[0].weapon_item.type > 0 )
	    {
		GiveItemDescription ( ItemDescText , & ( Me[0].weapon_item ) , FALSE );
		best_banner_pos_x = WEAPON_RECT_X + 30 + WEAPON_RECT_WIDTH ;
		best_banner_pos_y = WEAPON_RECT_Y - 30 ;
	    }
	}
	else if ( MouseCursorIsOnButton ( DRIVE_RECT_BUTTON , CurPos.x , CurPos.y ) )
	{
	    if ( Me[0].drive_item.type > 0 )
	    {
		GiveItemDescription ( ItemDescText , & ( Me[0].drive_item) , FALSE );
		best_banner_pos_x = DRIVE_RECT_X + 30 + DRIVE_RECT_WIDTH ;
		best_banner_pos_y = DRIVE_RECT_Y - 30 ;
	    }
	}
	else if ( MouseCursorIsOnButton ( SHIELD_RECT_BUTTON , CurPos.x , CurPos.y ) )
	{
	    if ( Me [ 0 ] . shield_item . type > 0 )
	    {
		GiveItemDescription ( ItemDescText , & ( Me [ 0 ] . shield_item ) , FALSE );
		best_banner_pos_x = SHIELD_RECT_X + 30 + SHIELD_RECT_WIDTH ;
		best_banner_pos_y = SHIELD_RECT_Y - 30 ;
	    }
	    else if ( Me [ 0 ] . weapon_item . type > 0 )
	    {
		if ( ItemMap [ Me [ 0 ] . weapon_item . type ] . item_gun_requires_both_hands )
		{
		    GiveItemDescription ( ItemDescText , & ( Me [ 0 ] . weapon_item ) , FALSE );
		    best_banner_pos_x = SHIELD_RECT_X + 30 + SHIELD_RECT_WIDTH ;
		    best_banner_pos_y = SHIELD_RECT_Y - 30 ;
		}
	    }
	}
	else if ( MouseCursorIsOnButton ( ARMOUR_RECT_BUTTON , CurPos.x , CurPos.y ) )
	{
	    if ( Me[0].armour_item.type > 0 )
	    {
		GiveItemDescription ( ItemDescText , & ( Me[0].armour_item) , FALSE );
		best_banner_pos_x = ARMOUR_RECT_X + 30 + ARMOUR_RECT_WIDTH ;
		best_banner_pos_y = ARMOUR_RECT_Y - 30 ;
	    }
	}
	else if ( MouseCursorIsOnButton ( HELMET_RECT_BUTTON , CurPos.x , CurPos.y ) )
	{
	    if ( Me[0].special_item.type > 0 )
	    {
		GiveItemDescription ( ItemDescText , & ( Me[0].special_item) , FALSE );
		best_banner_pos_x = HELMET_RECT_X + 30 + HELMET_RECT_WIDTH ;
		best_banner_pos_y = HELMET_RECT_Y - 30 ;
	    }
	}
	else if ( MouseCursorIsOnButton ( AUX1_RECT_BUTTON , CurPos.x , CurPos.y ) )
	{
	    if ( Me[0].aux1_item.type > 0 )
	    {
		GiveItemDescription ( ItemDescText , & ( Me[0].aux1_item) , FALSE );
		best_banner_pos_x = AUX1_RECT_X + 30 + AUX1_RECT_WIDTH ;
		best_banner_pos_y = AUX1_RECT_Y - 30 ;
	    }
	}
	else if ( MouseCursorIsOnButton ( AUX2_RECT_BUTTON , CurPos.x , CurPos.y ) )
	{
	    if ( Me[0].aux2_item.type > 0 )
	    {
		GiveItemDescription ( ItemDescText , & ( Me[0].aux2_item) , FALSE );
		best_banner_pos_x = AUX2_RECT_X + 30 + AUX2_RECT_WIDTH ;
		best_banner_pos_y = AUX2_RECT_Y - 30 ;
	    }
	}
	
    } // if nothing is 'held in hand' && inventory-screen visible
    

    if (( CurPos . x > WHOLE_HEALTH_RECT_X * GameConfig . screen_width / 640) && 
	(CurPos .x * 640 / GameConfig . screen_width < WHOLE_HEALTH_RECT_X + WHOLE_HEALTH_RECT_W) && 
	(CurPos.y > WHOLE_HEALTH_RECT_Y * GameConfig . screen_height / 480) &&
	( CurPos.y * 480 / GameConfig . screen_height < WHOLE_HEALTH_RECT_Y + WHOLE_HEALTH_RECT_H))
	{
	best_banner_pos_x = ( WHOLE_HEALTH_RECT_X ) * GameConfig . screen_width / 640;
	best_banner_pos_y = ( WHOLE_HEALTH_RECT_Y - 25 ) * GameConfig . screen_height / 480;
	sprintf(ItemDescText, "%s%d/%d", Me[0].energy / Me[0].maxenergy <= 0.1 ? font_switchto_red:"", (int)rintf(Me[0].energy), (int)rintf(Me[0].maxenergy));
	}

    if (( CurPos . x > WHOLE_FORCE_RECT_X * GameConfig . screen_width / 640) && 
	(CurPos .x * 640 / GameConfig . screen_width < WHOLE_FORCE_RECT_X + WHOLE_FORCE_RECT_W) && 
	(CurPos.y > WHOLE_FORCE_RECT_Y * GameConfig . screen_height / 480) &&
	( CurPos.y * 480 / GameConfig . screen_height < WHOLE_FORCE_RECT_Y + WHOLE_FORCE_RECT_H))
	{
	best_banner_pos_x = ( WHOLE_FORCE_RECT_X ) * GameConfig . screen_width / 640;
	best_banner_pos_y = ( WHOLE_FORCE_RECT_Y - 25 ) * GameConfig . screen_height / 480;
	sprintf(ItemDescText, "%s%d/%d",  Me[0].mana / Me[0].maxmana <= 0.1 ? font_switchto_red:"", (int)rintf(Me[0].mana), (int)rintf(Me[0].maxmana));
	}

    if (( CurPos . x > WHOLE_RUNNING_POWER_RECT_X * GameConfig . screen_width / 640) &&
        (CurPos .x * 640 / GameConfig . screen_width < WHOLE_RUNNING_POWER_RECT_X + WHOLE_RUNNING_POWER_RECT_W) &&
        (CurPos.y > WHOLE_RUNNING_POWER_RECT_Y * GameConfig . screen_height / 480) &&
        ( CurPos.y * 480 / GameConfig . screen_height < WHOLE_RUNNING_POWER_RECT_Y + WHOLE_RUNNING_POWER_RECT_H))
        {
        best_banner_pos_x = ( WHOLE_RUNNING_POWER_RECT_X ) * GameConfig . screen_width / 640;
        best_banner_pos_y = ( WHOLE_RUNNING_POWER_RECT_Y - 25 ) * GameConfig . screen_height / 480;  
        sprintf(ItemDescText, "%s%d/%d",  Me[0].running_power / Me[0].max_running_power <= 0.1 ? font_switchto_red:"", (int)rintf(Me[0].running_power), (int)rintf(Me[0].max_running_power));
        }     

    if (( CurPos . x > WHOLE_EXPERIENCE_COUNTDOWN_RECT_X * GameConfig . screen_width / 640) &&
        (CurPos .x * 640 / GameConfig . screen_width < WHOLE_EXPERIENCE_COUNTDOWN_RECT_X + WHOLE_EXPERIENCE_COUNTDOWN_RECT_W) &&
        (CurPos.y > WHOLE_EXPERIENCE_COUNTDOWN_RECT_Y * GameConfig . screen_height / 480) &&
        ( CurPos.y * 480 / GameConfig . screen_height < WHOLE_EXPERIENCE_COUNTDOWN_RECT_Y + WHOLE_EXPERIENCE_COUNTDOWN_RECT_H))
        {
        best_banner_pos_x = ( WHOLE_EXPERIENCE_COUNTDOWN_RECT_X ) * GameConfig . screen_width / 640;
        best_banner_pos_y = ( WHOLE_EXPERIENCE_COUNTDOWN_RECT_Y - 25 ) * GameConfig . screen_height / 480;  
        sprintf(ItemDescText, "%ld/%ld", Me[0].Experience, Me[0].ExpRequired);
        }     



    //--------------------
    // If the mouse cursor is within the user rectangle, then we check if
    // either the cursor is over an inventory item or over some other droid
    // and in both cases, we give a description of the object in the small
    // black rectangle in the top status banner.
    //
    
    if ( MouseCursorIsInUserRect( CurPos.x , CurPos.y ) && ( CurLevel != NULL ) )
    {
	// DebugPrintf( 2  , "\nCursor is in userfenster... --> see if hovering over an item...");
	
	MapPositionOfMouse.x = translate_pixel_to_map_location ( 0 , 
								 (float) ServerThinksInputAxisX ( 0 ) , 
								 (float) ServerThinksInputAxisY ( 0 ) , TRUE ) ;
	MapPositionOfMouse.y = translate_pixel_to_map_location ( 0 , 
								 (float) ServerThinksInputAxisX ( 0 ) , 
								 (float) ServerThinksInputAxisY ( 0 ) , FALSE );
	
	index_of_floor_item_below_mouse_cursor = get_floor_item_index_under_mouse_cursor ( 0 );
	
	if ( index_of_floor_item_below_mouse_cursor != (-1) )
	{
	    GiveItemDescription ( 
		ItemDescText , & ( CurLevel -> ItemList [ index_of_floor_item_below_mouse_cursor ] ) , 
		FALSE );
	    best_banner_pos_x = translate_map_point_to_screen_pixel ( 
		CurLevel -> ItemList [ index_of_floor_item_below_mouse_cursor ] . pos . x , 
		CurLevel -> ItemList [ index_of_floor_item_below_mouse_cursor ] . pos . y  ,
		TRUE ) + 80 ;
	    best_banner_pos_y = translate_map_point_to_screen_pixel ( 
		CurLevel -> ItemList [ index_of_floor_item_below_mouse_cursor ] . pos . x , 
		CurLevel -> ItemList [ index_of_floor_item_below_mouse_cursor ] . pos . y  ,
		FALSE ) - 30 ;
	}
	
	//--------------------
	// Maybe the cursor in the user rect is hovering right over a closed chest.
	// In this case we say so in the top status banner.
	//
	index_of_chest_below_mouse_cursor = closed_chest_below_mouse_cursor ( 0 ) ;
	if ( index_of_chest_below_mouse_cursor != (-1) )
	{
	    strcpy ( ItemDescText , " C H E S T " ); 
	    // index_of_chest_below_mouse_cursor
	    best_banner_pos_x = translate_map_point_to_screen_pixel ( 
		CurLevel -> obstacle_list [ index_of_chest_below_mouse_cursor ] . pos . x , 
		CurLevel -> obstacle_list [ index_of_chest_below_mouse_cursor ] . pos . y  ,
		TRUE ) + 70 ;
	    best_banner_pos_y = translate_map_point_to_screen_pixel ( 
		CurLevel -> obstacle_list [ index_of_chest_below_mouse_cursor ] . pos . x , 
		CurLevel -> obstacle_list [ index_of_chest_below_mouse_cursor ] . pos . y  ,
		FALSE ) - 20 ;
	}
	
	//--------------------
	// Maybe the cursor in the user rect is hovering right over a closed chest.
	// In this case we say so in the top status banner.
	//
	index_of_barrel_below_mouse_cursor = smashable_barrel_below_mouse_cursor ( 0 ) ;
	if ( index_of_barrel_below_mouse_cursor != (-1) )
	{
	    //--------------------
	    // We do some case separation for the type of barrel/crate
	    // in question.
	    //
	    obs_type = curShip . AllLevels [ Me [ 0 ] . pos . z ] -> obstacle_list [ index_of_barrel_below_mouse_cursor ] . type ;
	    switch ( obs_type )
	    {
		case ISO_BARREL_1:
		case ISO_BARREL_2:
		    strcpy ( ItemDescText , " B A R R E L " ); 
		    break;
		case ISO_BARREL_3:
		case ISO_BARREL_4:
		    strcpy ( ItemDescText , " C R A T E " ); 
		    break;
		default:
		    fprintf ( stderr , "\nobs_type: %d." , obs_type );
		    GiveStandardErrorMessage ( __FUNCTION__  , "\
A barrel was detected, but the barrel type was not valid.",
					       PLEASE_INFORM, IS_FATAL );
			break;
	    }
	    best_banner_pos_x = translate_map_point_to_screen_pixel ( 
		CurLevel -> obstacle_list [ index_of_barrel_below_mouse_cursor ] . pos . x , 
		CurLevel -> obstacle_list [ index_of_barrel_below_mouse_cursor ] . pos . y  ,
		TRUE ) + 70 ;
	    best_banner_pos_y = translate_map_point_to_screen_pixel ( 
		CurLevel -> obstacle_list [ index_of_barrel_below_mouse_cursor ] . pos . x , 
		CurLevel -> obstacle_list [ index_of_barrel_below_mouse_cursor ] . pos . y  ,
		FALSE ) - 20 ;
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
    
}; // void prepare_text_window_content ( ItemDescText ) 

/* ----------------------------------------------------------------------
 * At various points in the game, especially when the mouse in over an
 * interesting object inside the game, a popup window will appear, e.g.
 * to describe the object in question.
 * This function is responsible for bringing up these text windows.
 * ---------------------------------------------------------------------- */
void
ShowCurrentTextWindow ( void )
{
    SDL_Rect Banner_Text_Rect;
    char ItemDescText[5000]=" ";
    char TextLine[10][1000];
    int i;
    int NumberOfLinesInText = 1;
    char* LongTextPointer;
    int InterLineDistance;
    int StringLength;
    int lines_needed;

    //--------------------
    // During the title display phase, we need not have this window visible...
    //
    if ( Me [ 0 ] . status == BRIEFING ) return;

    //--------------------
    // We prepare the string, that is to be displayed inside the text 
    // rectangle...
    //
    prepare_text_window_content ( ItemDescText ) ;
    
    Banner_Text_Rect . x = best_banner_pos_x ;
    Banner_Text_Rect . y = best_banner_pos_y ;

    Banner_Text_Rect . w = LOWER_BANNER_TEXT_RECT_W;
    Banner_Text_Rect . h = LOWER_BANNER_TEXT_RECT_H;
	
    if ( strlen ( ItemDescText) < 10 ) Banner_Text_Rect . w = 100;
    //--------------------
    // We count the text lines needed for the banner rectangle, just
    // to make sure we don't wast too much space here.
    //
    lines_needed = GetNumberOfTextLinesNeeded ( ItemDescText , Banner_Text_Rect , 1.0 ) ;
    if ( lines_needed <= 20 )
    {
	Banner_Text_Rect . h = ( lines_needed + 2 ) *  FontHeight( FPS_Display_BFont );
    }

    //--------------------
    // Now we add some extra correction, so that the banner rectangle can not
    // reach outside of the visible screen...
    //
    if ( Banner_Text_Rect . x + Banner_Text_Rect . w > GameConfig . screen_width )
    {
	Banner_Text_Rect . x = GameConfig . screen_width - Banner_Text_Rect . w ;
    }
    if ( Banner_Text_Rect . y + Banner_Text_Rect . h > GameConfig . screen_height )
    {
	Banner_Text_Rect . y = GameConfig . screen_height - Banner_Text_Rect . h ;
    }

    //--------------------
    // Now we can start to draw the rectangle
    //
    SDL_SetClipRect( Screen , NULL );  // this unsets the clipping rectangle
    if ( strlen( ItemDescText ) > 1 )
    {
	if ( use_open_gl ) 
	    GL_HighlightRectangle ( Screen , Banner_Text_Rect , 0 , 0 , 0 , BACKGROUND_TEXT_RECT_ALPHA );
	else
	    our_SDL_fill_rect_wrapper( Screen , &Banner_Text_Rect , BANNER_TEXT_REC_BACKGROUNDCOLOR );
    }
    else
    {
	return;
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
    InterLineDistance = ( Banner_Text_Rect . h - NumberOfLinesInText * FontHeight( GetCurrentFont() ) ) / 
	( NumberOfLinesInText + 1 );
    
    LongTextPointer = ItemDescText;
    for ( i = 0 ; i < NumberOfLinesInText-1 ; i ++ )
    {
	StringLength = strstr( LongTextPointer , "\n" ) - LongTextPointer ;
	
	strncpy ( TextLine[ i ] , LongTextPointer , StringLength );
	TextLine [ i ] [ StringLength ] = 0;
	
	LongTextPointer += StringLength + 1;
	PutString ( Screen , 
		    Banner_Text_Rect . x + ( Banner_Text_Rect . w - TextWidth ( TextLine [ i ] ) ) / 2 ,
		    Banner_Text_Rect . y + InterLineDistance + 
		    i * ( InterLineDistance + FontHeight( GetCurrentFont() ) ) , TextLine[ i ] );
    }
    PutString ( Screen , 
		Banner_Text_Rect . x + ( Banner_Text_Rect . w - TextWidth ( LongTextPointer ) ) / 2 ,
		Banner_Text_Rect.y + InterLineDistance + 
		i * ( InterLineDistance + FontHeight( GetCurrentFont() ) ) , LongTextPointer );

}; // void ShowCurrentTextWindow ( void )

/* ---------------------------------------------------------------------- 
 * This function derives the 'minutes' component of the time already 
 * elapsed in this game.
 * ---------------------------------------------------------------------- */
int
get_minutes_of_game_duration ( float current_game_date )
{
    return ( ((int)( 10 * Me [ 0 ] . current_game_date / ( 60 ))) % 60 ) ;
}; // void get_minutes_of_game_duration ( float current_game_date )

/* ---------------------------------------------------------------------- 
 * This function derives the 'hours' component of the time already 
 * elapsed in this game.
 * ---------------------------------------------------------------------- */
int
get_hours_of_game_duration ( float current_game_date )
{
    return ( ((int)( 10 * Me [ 0 ] . current_game_date / ( 60 * 60 ))) % 60 ) ;
}; // void get_hours_of_game_duration ( float current_game_date )

/* ---------------------------------------------------------------------- 
 * This function derives the 'days' component of the time already 
 * elapsed in this game.
 * ---------------------------------------------------------------------- */
int
get_days_of_game_duration ( float current_game_date )
{
    return ( ((int)( 10 * Me [ 0 ] . current_game_date / ( 60 * 60 * 24 ))) );
}; // void get_days_of_game_duration ( float current_game_date )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
append_new_game_message ( char* game_message_text )
{
    if ( game_message_protocol == NULL )
    {
	game_message_protocol = MyMalloc ( 500000 ) ; // enough for any protocol
	sprintf ( game_message_protocol , "--- Message Protocol ---" );
    }

    strcat ( game_message_protocol , "\n* " ) ;
    strcat ( game_message_protocol , game_message_text ) ;

    game_message_protocol_scroll_override_from_user = 0 ;

}; // void append_new_game_message ( char* game_message_text )

/* ----------------------------------------------------------------------
 * We display a window with the current text messages.
 * ---------------------------------------------------------------------- */
void
display_current_game_message_window ( void )
{
    SDL_Rect Subtitle_Window;
    int lines_needed ;
    int protocol_offset ;
    float our_stretch_factor = 1.00 ; // TEXT_STRETCH
    // float extra_stretch_calibrator = ((float)1.0/(float)1.07) ;  // 1.04
    float extra_stretch_calibrator = 1.00 ;

#define AVERAGE_LINES_IN_MESSAGE_WINDOW 3*GameConfig . screen_height/480

    SetCurrentFont ( Message_BFont );
    if ( game_message_protocol == NULL )
    {
	game_message_protocol = MyMalloc ( 500000 ) ; // enough for any protocol
	sprintf ( game_message_protocol , "This is the protocol.\nOh yes, it is indeed!\nIt has multiple lines too!\nIsn't that great?  But how long may any one line be?  Is there a limit for that?  Anyway, it looks good." );
    }

    //--------------------
    // First we define our subtitle window.  We formerly had a small
    // narrow subtitle window of a format like this:
    //
    // { 65 , 410 , 500 , 70 }       
    
    Subtitle_Window . x = ( ( 90 + 20 ) * GameConfig . screen_width ) / 640 ;
    Subtitle_Window . y = ( ( 380 + 8 ) * GameConfig . screen_height ) / 480 ;
    Subtitle_Window . w = ( ( 500 - 20 - 20 ) * GameConfig . screen_width ) / 640 ;
    Subtitle_Window . h = ( 60 * GameConfig . screen_height ) / 480 ;

    //--------------------
    // First we need to know where to begin with our little display.
    //
    lines_needed = GetNumberOfTextLinesNeeded ( game_message_protocol , Subtitle_Window , our_stretch_factor );
    DebugPrintf ( 1 , "\nLines needed: %d. " , lines_needed );
    
    if ( lines_needed <= AVERAGE_LINES_IN_MESSAGE_WINDOW ) 
    {
	//--------------------
	// When there isn't anything to scroll yet, we keep the default
	// position and also the users clicks on up/down button will be
	// reset immediately
	//
	protocol_offset = 0 ;
	game_message_protocol_scroll_override_from_user = 0 ;
    }
    else
	protocol_offset = ( FontHeight ( GetCurrentFont() ) * our_stretch_factor ) 
	    * ( lines_needed - AVERAGE_LINES_IN_MESSAGE_WINDOW + game_message_protocol_scroll_override_from_user ) * extra_stretch_calibrator ;

    //--------------------
    // Now if the protocol offset is really negative, we don't really want
    // that and force the user offset back to something sane again.
    //
    if ( protocol_offset < 0 )
    {
	game_message_protocol_scroll_override_from_user ++ ;
	protocol_offset = 0 ;
    }

    // blit_special_background ( GAME_MESSAGE_WINDOW_BACKGROUND_CODE );
    blit_special_background ( HUD_BACKGROUND_CODE );
    
    //--------------------
    // Now we can display the text and update the screen...
    //
    SDL_SetClipRect( Screen, NULL );
    // { 65 , 410 , 500 , 70 }       
    Subtitle_Window . x = ( ( 90 + 20 ) * GameConfig . screen_width ) / 640 ;
    Subtitle_Window . y = ( ( 380 + 8 ) * GameConfig . screen_height ) / 480 ;
    Subtitle_Window . w = ( ( 500 - 20 - 20 ) * GameConfig . screen_width ) / 640 ;
    Subtitle_Window . h = ( 40 * GameConfig . screen_height ) / 480 ;
    SetCurrentFont ( Message_BFont );
    DisplayText ( game_message_protocol , Subtitle_Window.x , Subtitle_Window.y - protocol_offset , &Subtitle_Window , our_stretch_factor );

    /*
    if ( protocol_offset > 0 ) 
	ShowGenericButtonFromList ( CHAT_PROTOCOL_SCROLL_UP_BUTTON );
    else
	ShowGenericButtonFromList ( CHAT_PROTOCOL_SCROLL_OFF_BUTTON );
    if ( lines_needed <= AVERAGE_LINES_IN_PROTOCOL_WINDOW ) 
	ShowGenericButtonFromList ( CHAT_PROTOCOL_SCROLL_OFF2_BUTTON );
    else
	ShowGenericButtonFromList ( CHAT_PROTOCOL_SCROLL_DOWN_BUTTON );
    */
    // if ( with_update ) our_SDL_update_rect_wrapper ( Screen , Subtitle_Window.x , Subtitle_Window.y , Subtitle_Window.w , Subtitle_Window.h );
    
}; // void display_current_game_message_window ( int background_picture_code , int with_update )

/* ----------------------------------------------------------------------
 * This function updates the various displays that are usually blitted
 * right into the combat window, like energy and status meter and that...
 * ---------------------------------------------------------------------- */
void
DisplayBanner ( void )
{
    char level_name_and_time [ 1000 ] ;

    SDL_SetClipRect( Screen , NULL ); 

    ShowCurrentHealthAndForceLevel ( );
    
    ShowCurrentTextWindow ( );
    ShowCurrentSkill ( );
    ShowCurrentWeapon ( );

    //--------------------
    // We display the name of the current level and the current time inside
    // the game.
    //
    if ( ! ( GameConfig . CharacterScreen_Visible || GameConfig . SkillScreen_Visible ))
    {
	sprintf ( level_name_and_time , "%s (%03.1f:%03.1f:%d) day %d %02d:%02d " , 
		  curShip . AllLevels [ Me [ 0 ] . pos . z ] -> Levelname , 
		  Me [ 0 ] . pos . x , 
		  Me [ 0 ] . pos . y , 
		  Me [ 0 ] . pos . z , 
		  get_days_of_game_duration ( Me [ 0 ] . current_game_date ) ,
		  get_hours_of_game_duration ( Me [ 0 ] . current_game_date ) ,
		  get_minutes_of_game_duration ( Me [ 0 ] . current_game_date ) );
	RightPutStringFont ( Screen , FPS_Display_BFont , 2 , level_name_and_time );
    }

}; // void DisplayBanner( void ) 


#undef _hud_c
