/* 
 *
 *   Copyright (c) 2002, 2003 Johannes Prix
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
 * This file contains all the functions managing the character attributes
 * and the character stats.
 * ---------------------------------------------------------------------- */

#define _character_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"

//--------------------
// Now we give some definitions for the computation of the
// character stats
//                                      For Warrior  For Sniper  For Hacker
float Energy_Gain_Per_Vit_Point[]={ -1 ,     2 ,        1.5 ,        1 };
float Mana_Gain_Per_Magic_Point[]={ -1 ,     2 ,        1.5 ,        2 };
float AC_Gain_Per_Dex_Point[]={     -1 ,     0.5 ,         0.5  ,       0.5 };

#define RECHARGE_SPEED_PERCENT_PER_DEX_POINT 0
#define TOHIT_PERCENT_PER_DEX_POINT (0.5)

//--------------------
// At first we state some geometry constants for where to insert
// which character stats in the character screen...
//

#define CLASS_X 175

#define EXPERIENCE_Y 55
#define NEXT_LEVEL_Y 82

#define GOLD_Y 132

#define DAMAGE_X 260
#define DAMAGE_Y 225

#define RECHARGE_X 260
#define RECHARGE_Y 200

#define AC_X 260
#define AC_Y 171

#define LV_1_BOT_HITS_CHANCE_X 254
#define LV_1_BOT_HITS_CHANCE_Y 249

#define MELEE_SKILL_X 130
#define MELEE_SKILL_Y 346
#define RANGED_SKILL_X 130
#define RANGED_SKILL_Y 373
#define SPELLCASTING_SKILL_X 130
#define SPELLCASTING_SKILL_Y 398
#define HACKING_SKILL_X 130
#define HACKING_SKILL_Y 421

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void 
InitiateNewCharacter ( int PlayerNum , int CharacterClass )
{
    int i;
    int MissionTargetIndex;
    
    //--------------------
    // We set the coordinates to the generic starting point for all players....
    //
    Me [ PlayerNum ] . pos. x = 2 ;
    Me [ PlayerNum ] . pos. y = 3 ;
    Me [ PlayerNum ] . pos. z = 0 ;
    
    //--------------------
    // At first, we clear out any inventory items, that may still be equipped
    // for this one character.
    //
    for ( i = 0 ; i < MAX_ITEMS_IN_INVENTORY ; i ++ )
    {
	Me [ PlayerNum ] . Inventory [ i ] . type = -1 ;
	Me [ PlayerNum ] . Inventory [ i ] . inventory_position.x = -1 ;
	Me [ PlayerNum ] . Inventory [ i ] . inventory_position.y = -1 ; 
	Me [ PlayerNum ] . Inventory [ i ] . currently_held_in_hand = FALSE ; 
    }
    
    Me [ PlayerNum ] .weapon_item.type = ( -1 ) ;
    Me [ PlayerNum ] .drive_item.type = ( -1 ) ;
    Me [ PlayerNum ] .armour_item.type = ( -1 ) ;
    Me [ PlayerNum ] .shield_item.type = ( -1 ) ;
    Me [ PlayerNum ] .aux1_item.type = ( -1 ) ;
    Me [ PlayerNum ] .aux2_item.type = ( -1 ) ;
    Me [ PlayerNum ] .special_item.type = ( -1 ) ;
    
    Me [ PlayerNum ] .weapon_item.prefix_code = ( -1 ) ;
    Me [ PlayerNum ] .drive_item.prefix_code = ( -1 ) ;
    Me [ PlayerNum ] .armour_item.prefix_code = ( -1 ) ;
    Me [ PlayerNum ] .shield_item.prefix_code = ( -1 ) ;
    Me [ PlayerNum ] .aux1_item.prefix_code = ( -1 ) ;
    Me [ PlayerNum ] .aux2_item.prefix_code = ( -1 ) ;
    Me [ PlayerNum ] .special_item.prefix_code = ( -1 ) ;
    
    Me [ PlayerNum ] .weapon_item.suffix_code = ( -1 ) ;
    Me [ PlayerNum ] .drive_item.suffix_code = ( -1 ) ;
    Me [ PlayerNum ] .armour_item.suffix_code = ( -1 ) ;
    Me [ PlayerNum ] .shield_item.suffix_code = ( -1 ) ;
    Me [ PlayerNum ] .aux1_item.suffix_code = ( -1 ) ;
    Me [ PlayerNum ] .aux2_item.suffix_code = ( -1 ) ;
    Me [ PlayerNum ] .special_item.suffix_code = ( -1 ) ;
    
    Me [ PlayerNum ] .type = DRUID001;
    Me [ PlayerNum ] .speed.x = 0;
    Me [ PlayerNum ] .speed.y = 0;
    Me [ PlayerNum ] .status = MOBILE;
    Me [ PlayerNum ] .phase = 0;
    Me [ PlayerNum ] .MissionTimeElapsed=0;
    Me [ PlayerNum ] .Current_Victim_Resistance_Factor=1;
    Me [ PlayerNum ] .FramesOnThisLevel=0;
    Me [ PlayerNum ] .weapon_swing_time = (-1);  // currently not swinging this means...
    Me [ PlayerNum ] .got_hit_time = (-1);  // currently not stunned and needing time to recover...
    
    Me [ PlayerNum ] .points_to_distribute = 0;
    Me [ PlayerNum ] .ExpRequired = 1500;
    for ( i = 0 ; i < 1000 ; i ++ ) Me [ PlayerNum ] .KillRecord[ i ] = 0;
    for ( i = 0 ; i < MAX_LEVELS ; i ++ ) Me [ PlayerNum ] .HaveBeenToLevel [ i ] = FALSE ;
    Me [ PlayerNum ] .exp_level = 1;
    Me [ PlayerNum ] .Gold = 100;
    
    switch ( CharacterClass ) 
    {
	case WAR_BOT:
	    Me [ PlayerNum ] .character_class = WAR_BOT;
	    Me [ PlayerNum ] .base_vitality = 25;
	    Me [ PlayerNum ] .base_strength = 30;
	    Me [ PlayerNum ] .base_dexterity = 25;
	    Me [ PlayerNum ] .base_magic = 10;
	    
	    Me [ PlayerNum ] .drive_item.type = ITEM_ANTIGRAV_POD ;
	    Me [ PlayerNum ] .Inventory[ 0 ].type = ITEM_SHORT_SWORD;
	    Me [ PlayerNum ] .Inventory[ 0 ].inventory_position.x = 0;
	    Me [ PlayerNum ] .Inventory[ 0 ].inventory_position.y = 0;
	    Me [ PlayerNum ] .Inventory[ 1 ].type = ITEM_BUCKLER;
	    Me [ PlayerNum ] .Inventory[ 1 ].inventory_position.x = 2;
	    Me [ PlayerNum ] .Inventory[ 1 ].inventory_position.y = 0;
	    Me [ PlayerNum ] .Inventory[ 2 ].type = ITEM_SMALL_HEALTH_POTION;
	    Me [ PlayerNum ] .Inventory[ 2 ].inventory_position.x = 0;
	    Me [ PlayerNum ] .Inventory[ 2 ].inventory_position.y = INVENTORY_GRID_HEIGHT-1;
	    Me [ PlayerNum ] .Inventory[ 3 ].type = ITEM_SMALL_HEALTH_POTION;
	    Me [ PlayerNum ] .Inventory[ 3 ].inventory_position.x = 1;
	    Me [ PlayerNum ] .Inventory[ 3 ].inventory_position.y = INVENTORY_GRID_HEIGHT-1;
	    FillInItemProperties ( & Me [ PlayerNum ] .Inventory[ 0 ] , TRUE , 0 );
	    FillInItemProperties ( & Me [ PlayerNum ] .Inventory[ 1 ] , TRUE , 0 );
	    FillInItemProperties ( & Me [ PlayerNum ] .Inventory[ 2 ] , TRUE , 0 );
	    FillInItemProperties ( & Me [ PlayerNum ] .Inventory[ 3 ] , TRUE , 0 );
	    break;
	    
	case SNIPER_BOT: 
	    Me [ PlayerNum ] .character_class = SNIPER_BOT;
	    Me [ PlayerNum ] .base_vitality = 20;
	    Me [ PlayerNum ] .base_strength = 25;
	    Me [ PlayerNum ] .base_dexterity = 35;
	    Me [ PlayerNum ] .base_magic = 20;
	    
	    Me [ PlayerNum ] .drive_item.type = ITEM_ANTIGRAV_POD;
	    Me [ PlayerNum ] .Inventory[ 0 ].type = ITEM_LASER_PISTOL;
	    Me [ PlayerNum ] .Inventory[ 0 ].inventory_position.x = 0;
	    Me [ PlayerNum ] .Inventory[ 0 ].inventory_position.y = 0;
	    Me [ PlayerNum ] .Inventory[ 1 ].type = ITEM_SMALL_HEALTH_POTION;
	    Me [ PlayerNum ] .Inventory[ 1 ].inventory_position.x = 0;
	    Me [ PlayerNum ] .Inventory[ 1 ].inventory_position.y = INVENTORY_GRID_HEIGHT-1;
	    Me [ PlayerNum ] .Inventory[ 2 ].type = ITEM_SMALL_HEALTH_POTION;
	    Me [ PlayerNum ] .Inventory[ 2 ].inventory_position.x = 1;
	    Me [ PlayerNum ] .Inventory[ 2 ].inventory_position.y = INVENTORY_GRID_HEIGHT-1;
	    FillInItemProperties ( & Me [ PlayerNum ] .Inventory[ 0 ] , TRUE , 0 );
	    FillInItemProperties ( & Me [ PlayerNum ] .Inventory[ 1 ] , TRUE , 0 );
	    FillInItemProperties ( & Me [ PlayerNum ] .Inventory[ 2 ] , TRUE , 0 );
	    break;
	    
	case MIND_BOT: 
	    Me [ PlayerNum ] .character_class = MIND_BOT;
	    Me [ PlayerNum ] .base_vitality = 15;
	    Me [ PlayerNum ] .base_strength = 15;
	    Me [ PlayerNum ] .base_dexterity = 20;
	    Me [ PlayerNum ] .base_magic = 35;
	    Me [ PlayerNum ] .drive_item.type = ITEM_ANTIGRAV_POD;
	    
	    Me [ PlayerNum ] .Inventory[ 0 ].type = ITEM_STAFF;
	    Me [ PlayerNum ] .Inventory[ 0 ].inventory_position.x = 0;
	    Me [ PlayerNum ] .Inventory[ 0 ].inventory_position.y = 0;
	    Me [ PlayerNum ] .Inventory[ 1 ].type = ITEM_SMALL_MANA_POTION;
	    Me [ PlayerNum ] .Inventory[ 1 ].inventory_position.x = 0;
	    Me [ PlayerNum ] .Inventory[ 1 ].inventory_position.y = INVENTORY_GRID_HEIGHT-1;
	    Me [ PlayerNum ] .Inventory[ 2 ].type = ITEM_SMALL_MANA_POTION;
	    Me [ PlayerNum ] .Inventory[ 2 ].inventory_position.x = 1;
	    Me [ PlayerNum ] .Inventory[ 2 ].inventory_position.y = INVENTORY_GRID_HEIGHT-1;
	    FillInItemProperties ( & Me [ PlayerNum ] .Inventory[ 0 ] , TRUE , 0 );
	    FillInItemProperties ( & Me [ PlayerNum ] .Inventory[ 1 ] , TRUE , 0 );
	    FillInItemProperties ( & Me [ PlayerNum ] .Inventory[ 2 ] , TRUE , 0 );
	    break;
	default: 
	    DebugPrintf ( 0 , "\nERROR!! UNKNOWN CHARACTER CLASS REQUESTED!!! Terminating... " );
	    Terminate ( ERR );
	    break;
    }
    
    UpdateAllCharacterStats( PlayerNum );
    
    Me [ PlayerNum ] .energy = Me [ PlayerNum ] .maxenergy;
    Me [ PlayerNum ] .temperature = 0;
    DebugPrintf( 1 , "\n Me [ PlayerNum ] .energy : %f . " , Me [ PlayerNum ] .energy );
    Me [ PlayerNum ] .health = Me [ PlayerNum ] .energy;	/* start with max. health */
    
    Me [ PlayerNum ] .weapon_item.currently_held_in_hand = FALSE;
    Me [ PlayerNum ] .armour_item.currently_held_in_hand = FALSE;
    Me [ PlayerNum ] .shield_item.currently_held_in_hand = FALSE;
    Me [ PlayerNum ] .special_item.currently_held_in_hand = FALSE;
    Me [ PlayerNum ] .drive_item.currently_held_in_hand = FALSE;
    Me [ PlayerNum ] .aux1_item.currently_held_in_hand = FALSE;
    Me [ PlayerNum ] .aux2_item.currently_held_in_hand = FALSE;
    
    FillInItemProperties ( & ( Me [ PlayerNum ] .weapon_item ) , TRUE , 0 );
    FillInItemProperties ( & ( Me [ PlayerNum ] .drive_item ) , TRUE , 0 );
    
    
    Item_Held_In_Hand = ( -1 );
    
    ShuffleEnemys( Me [ PlayerNum ] .pos.z ); // NOTE: THIS REQUIRES CurLevel TO BE INITIALIZED !! --> NOT ANY MORE!!!
    
    //--------------------
    // Now we start those missions, that are to be assigned automatically to the
    // player at game start
    //
    for ( MissionTargetIndex = 0 ; MissionTargetIndex < MAX_MISSIONS_IN_GAME ; MissionTargetIndex ++ )
    {
	if ( Me [ PlayerNum ] .AllMissions[ MissionTargetIndex ].AutomaticallyAssignThisMissionAtGameStart ) 
	{
	    AssignMission( MissionTargetIndex );
	}
    }
    
}; // void InitiateNewCharacter ( int PlayerNum , int CharacterClass )

/* ----------------------------------------------------------------------
 * This function displays all the buttons that open up the character
 * screen and the invenotry screen
 * ---------------------------------------------------------------------- */
void
DisplayButtons( void )
{

    //--------------------
    // When the Tux has some extra skill points, that can be distributed
    // to some character stats or saved for later training with some trainer
    // character in the city, we mark the character screen toggle button as
    // red to indicate the available points.
    //
    if ( Me[0].points_to_distribute > 0 )
    {
	// blit_special_background ( MOUSE_BUTTON_PLUS_BACKGROUND_PICTURE_CODE );
	ShowGenericButtonFromList ( CHA_SCREEN_TOGGLE_BUTTON_RED );
    }
    
    if ( MouseCursorIsOnButton( INV_SCREEN_TOGGLE_BUTTON , GetMousePos_x()  , GetMousePos_y()  ) )
    {
	ShowGenericButtonFromList ( INV_SCREEN_TOGGLE_BUTTON_YELLOW );
	if ( MouseLeftClicked())
	{
	    toggle_game_config_screen_visibility ( GAME_CONFIG_SCREEN_VISIBLE_INVENTORY );
	    DebugPrintf ( 2 , "\nClick inside inventory button registered..." );
	}
    }
    else if ( MouseCursorIsOnButton( CHA_SCREEN_TOGGLE_BUTTON , GetMousePos_x()  , GetMousePos_y()  ) )
    {
	ShowGenericButtonFromList ( CHA_SCREEN_TOGGLE_BUTTON_YELLOW );
	if ( MouseLeftClicked() )
	{
	    toggle_game_config_screen_visibility ( GAME_CONFIG_SCREEN_VISIBLE_CHARACTER );
	    DebugPrintf ( 2 , "\nClick inside character button registered..." );
	}
    }
    else if ( MouseCursorIsOnButton( SKI_SCREEN_TOGGLE_BUTTON , GetMousePos_x()  , GetMousePos_y()  ) )
    {
	ShowGenericButtonFromList ( SKI_SCREEN_TOGGLE_BUTTON_YELLOW );
	if ( MouseLeftClicked() )
	{
	    toggle_game_config_screen_visibility ( GAME_CONFIG_SCREEN_VISIBLE_SKILLS );
	    DebugPrintf ( 2 , "\nClick inside skills button registered..." );
	}
    }
    else if ( MouseCursorIsOnButton( LOG_SCREEN_TOGGLE_BUTTON , GetMousePos_x()  , GetMousePos_y()  ) )
    {
	ShowGenericButtonFromList ( LOG_SCREEN_TOGGLE_BUTTON_YELLOW );
	if ( MouseLeftClicked() )
	{
	    DebugPrintf ( 2 , "\nClick inside questlog button registered..." );
	    quest_browser_interface ( );
	}
    }
    else if ( MouseCursorIsOnButton( WEAPON_MODE_BUTTON , GetMousePos_x()  , GetMousePos_y()  ) )
    {
	if ( MouseLeftClicked() )
	{
	TuxReloadWeapon ( );
	}
    }
    else if ( MouseCursorIsOnButton( SKI_ICON_BUTTON , GetMousePos_x()  , GetMousePos_y()  ) )
    {
	if ( MouseLeftClicked() )
	{
        toggle_game_config_screen_visibility ( GAME_CONFIG_SCREEN_VISIBLE_SKILLS );
	}
    }
    
}; // void DisplayButtons( void )

/* ----------------------------------------------------------------------
 * This function checks if a given screen position lies within the 
 * strength plus button or not
 * ---------------------------------------------------------------------- */
int
CursorIsOnStrButton( int x , int y )
{
    if ( x > CharacterRect.x + BUTTON_MOD_X + STR_NOW_X + BUTTON_WIDTH ) return ( FALSE );
    if ( x < CharacterRect.x + BUTTON_MOD_X + STR_NOW_X ) return ( FALSE );
    if ( y > CharacterRect.y + BUTTON_MOD_X + STR_Y + BUTTON_HEIGHT ) return ( FALSE );
    if ( y < CharacterRect.y + BUTTON_MOD_X + STR_Y ) return ( FALSE );
    return ( TRUE );
}; // int CursorIsOnStrButton( int x , int y )

/* ----------------------------------------------------------------------
 * This function adds any bonuses that might be on the influencers things
 * concerning ONLY PRIMARY STATS, NOT SECONDARY STATS!
 * ---------------------------------------------------------------------- */
void
AddInfluencerItemAttributeBonus( item* BonusItem )
{
  int i;


  //--------------------
  // In case of no item, the thing to do is pretty easy...
  //
  if ( BonusItem->type == ( -1 ) ) return;

  //--------------------
  // In case of a unique special plugin, we do the nescessary modification
  // here...
  //
  if ( BonusItem->type == ITEM_START_PLUGIN_WARRIOR )
    {
      Me [ 0 ] . freezing_melee_targets = 7 ;
    }
  if ( BonusItem->type == ITEM_START_PLUGIN_SNIPER )
    {
      Me [ 0 ] . double_ranged_damage = TRUE ;
    }
  if ( BonusItem->type == ITEM_START_PLUGIN_HACKER )
    {
      for ( i = 0 ; i < number_of_skills ; i ++ ) 
	{
	  Me [ 0 ] . SkillLevel [ i ] += 2 ;
	}
    }

  //--------------------
  // In case of a suffix modifier, we need to apply the suffix...
  //
  if ( ( ( BonusItem->suffix_code != ( -1 ) ) || ( BonusItem->prefix_code != ( -1 ) ) ) &&
       BonusItem -> is_identified )
    {
      Me[0].Strength  += BonusItem->bonus_to_str + BonusItem->bonus_to_all_attributes ;
      Me[0].Dexterity += BonusItem->bonus_to_dex + BonusItem->bonus_to_all_attributes ;
      Me[0].Magic     += BonusItem->bonus_to_mag + BonusItem->bonus_to_all_attributes ;
      Me[0].Vitality  += BonusItem->bonus_to_vit + BonusItem->bonus_to_all_attributes ;
    }

}; // void AddInfluencerItemAttributeBonus( item* BonusItem )

/* ----------------------------------------------------------------------
 * This function adds any bonuses that might be on the influencers things
 * concerning ONLY SECONDARY STATS, NOT PRIMARY STATS!
 * ---------------------------------------------------------------------- */
void
AddInfluencerItemSecondaryBonus( item* BonusItem )
{
    //--------------------
    // In case of no item, the thing to do is pretty easy...
    //
    if ( BonusItem->type == ( -1 ) ) return;
    
    //--------------------
    // In case of present suffix or prefix modifiers, we need to apply the suffix...
    //
    if ( ( ( BonusItem->suffix_code != ( -1 ) ) || ( BonusItem->prefix_code != ( -1 ) ) ) &&
	 BonusItem->is_identified )
    {
	
	//--------------------
	// Some modifiers might not be random at all but fixed to the 
	// item prefix or item suffix.  In that case, we must get the
	// modifier strength from the suffix/prefix spec itself...
	//
	if ( BonusItem -> suffix_code != ( -1 ) )
	    Me [ 0 ] . light_bonus_from_tux += SuffixList [ BonusItem -> suffix_code ] . light_bonus_value ;
	if ( BonusItem -> prefix_code != ( -1 ) )
	    Me [ 0 ] . light_bonus_from_tux += PrefixList [ BonusItem -> prefix_code ] . light_bonus_value ;

	//--------------------
	// Now we can apply the modifiers, that have been generated from
	// the suffix spec and then (with some randomness) written into the
	// item itself.  In that case we won't need the suffix- or 
	// prefix-lists here...
	//
	Me [ 0 ] . to_hit    += BonusItem -> bonus_to_tohit ;
	Me [ 0 ] . max_temperature   += BonusItem -> bonus_to_force ;
	Me [ 0 ] . maxenergy += BonusItem -> bonus_to_life ; 
	Me [ 0 ] . health_recovery_rate += BonusItem -> bonus_to_health_recovery ; 
	Me [ 0 ] . cooling_rate += BonusItem -> bonus_to_cooling_rate ; 
	
	Me [ 0 ] . resist_disruptor   += BonusItem -> bonus_to_resist_disruptor ;
	Me [ 0 ] . resist_fire        += BonusItem -> bonus_to_resist_fire ;
	Me [ 0 ] . resist_electricity += BonusItem -> bonus_to_resist_electricity ;

	
	// if ( ItemMap [ BonusItem->type ] . can_be_installed_in_weapon_slot )
	// Me[0].freezing_enemys_property += BonusItem->freezing_time_in_seconds;
    }

}; // void AddInfluencerItemSecondaryBonus( item* BonusItem )

/* ----------------------------------------------------------------------
 * Maybe the influencer has reached a new experience level?
 * Let's check this...
 * ---------------------------------------------------------------------- */
void
check_for_new_experience_level_reached ( int PlayerNum )
{
    int BaseExpRequired = 400;

    if ( Me [ PlayerNum ] . exp_level >= 24 )
	{
	SetNewBigScreenMessage( "Max level reached!");
	return;
	}

    Me [ PlayerNum ] . ExpRequired = 
	BaseExpRequired * ( exp ( ( Me [ PlayerNum ] . exp_level - 1 ) * log ( 2 ) ) ) ;
    
    //--------------------
    // For display reasons in the experience graph, we also state the experience 
    // needed for the previous level inside the tux struct.  Therefore all exp/level
    // calculations are found in this function.
    //
    if ( Me [ PlayerNum ] . exp_level > 1 )
    {
	Me [ PlayerNum ] . ExpRequired_previously = 
	    BaseExpRequired * ( exp ( ( Me [ PlayerNum ] .exp_level - 2 ) * log ( 2 ) ) ) ;
    }
    else
	Me [ PlayerNum ] . ExpRequired_previously = 0 ;

    if ( Me [ PlayerNum ] . Experience > Me [ PlayerNum ] . ExpRequired ) 
    {
	Me [ PlayerNum ] . exp_level ++ ;
	Me [ PlayerNum ] . points_to_distribute += 5;

	//--------------------
	// Like in the Gothic 1 game, maximum life force will now automatically
	// be increased upon reaching a new character level.
	//
	Me [ PlayerNum ] . base_vitality += 3;

	//--------------------
	// When a droid reaches a new experience level, all health and 
	// force are restored to full this one time no longer.  Gothic
	// rulez more than Diablo rulez.
	//
	// Me [ PlayerNum ] .energy = Me [ PlayerNum ] .maxenergy ;
	// Me [ PlayerNum ] .mana   = Me [ PlayerNum ] .maxmana   ;

	//--------------------
	// Also when a new level is reached, we will display a big message
	// right over the combat window.
	//
	SetNewBigScreenMessage( "Level Gained!" );
	Takeover_Game_Won_Sound();
    }
}; // void check_for_new_experience_level_reached ( int PlayerNum )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
update_all_primary_stats ( int PlayerNum )
{
    int i;

    //--------------------
    // Now we base PRIMARY stats
    //
    Me [ PlayerNum ] . Strength = Me [ PlayerNum ] . base_strength;
    Me [ PlayerNum ] . Dexterity = Me [ PlayerNum ] . base_dexterity;
    Me [ PlayerNum ] . Magic = Me [ PlayerNum ] . base_magic;
    Me [ PlayerNum ] . Vitality = Me [ PlayerNum ] . base_vitality;
    
    Me [ PlayerNum ] . freezing_melee_targets = 0;
    Me [ PlayerNum ] . double_ranged_damage = FALSE;

    //--------------------
    // Now we re-initialize the SKILL LEVELS
    //
    for ( i = 0; i < number_of_skills; i ++)
	 Me [ PlayerNum ] . SkillLevel[i] =  Me [ PlayerNum ] . base_skill_level[i];

    //--------------------
    // Now we add all bonuses to the influencers PRIMARY stats
    //
    AddInfluencerItemAttributeBonus( & Me [ PlayerNum ] . armour_item );
    AddInfluencerItemAttributeBonus( & Me [ PlayerNum ] . weapon_item );
    AddInfluencerItemAttributeBonus( & Me [ PlayerNum ] . drive_item );
    AddInfluencerItemAttributeBonus( & Me [ PlayerNum ] . shield_item );
    AddInfluencerItemAttributeBonus( & Me [ PlayerNum ] . special_item );
    AddInfluencerItemAttributeBonus( & Me [ PlayerNum ] . aux1_item );
    AddInfluencerItemAttributeBonus( & Me [ PlayerNum ] . aux2_item );

    item * itrot [7] = {  & Me [ PlayerNum ] . armour_item,  & Me [ PlayerNum ] . weapon_item ,  & Me [ PlayerNum ] . drive_item ,  
    		& Me [ PlayerNum ] . shield_item , & Me [ PlayerNum ] . special_item ,  & Me [ PlayerNum ] . aux1_item ,
		& Me [ PlayerNum ] . aux2_item };
    i = 0;
    while ( i < 7 ) 
	{
	if ( itrot [ i ] -> type == -1 ) 
		{
		i ++;
		continue;
		}
	if ( ! ItemUsageRequirementsMet ( itrot [ i ] , FALSE ) )
		{ //we have to move away the item
		if ( ! AddFloorItemDirectlyToInventory ( itrot [ i ] ) ) itrot [ i ] -> type = -1;
		else 
			{ //inventory is full... ouch
			DropItemToTheFloor(itrot[i], Me[0].pos.x, Me[0].pos.y, Me[0].pos.z);
			itrot [ i ] -> type = -1;
			}
		}
	i ++;
	}



    //--------------------
    // Maybe there is some boost from a potion or magic spell going on right
    // now...
    //
    if ( Me [ 0 ] . dexterity_bonus_end_date > Me [ 0 ] . current_game_date )
	Me [ 0 ] . Dexterity += Me [ 0 ] . current_dexterity_bonus ;
    if ( Me [ 0 ] . power_bonus_end_date > Me [ 0 ] . current_game_date )
	Me [ 0 ] . Strength += Me [ 0 ] . current_power_bonus ;

}; // void update_all_primary_stats ( int PlayerNum )

/* ----------------------------------------------------------------------
 * This function computes secondary stats (i.e. chances for success or
 * getting hit and the like) using ONLY THE PRIMARY STATS.  Bonuses from
 * current 'magic' modifiers from equipped items will be applied somewhere
 * else.
 * ---------------------------------------------------------------------- */
void
update_secondary_stats_from_primary_stats ( int PlayerNum )
{
    //--------------------
    // The chance that this player character will score a hit on an enemy
    //
    Me [ PlayerNum ] . to_hit = 
	60 + ( Me [ PlayerNum ] . Dexterity - 15 ) * TOHIT_PERCENT_PER_DEX_POINT;

    //--------------------
    // How many life points can this character aquire currently
    //
    Me [ PlayerNum ] . maxenergy = 
	( Me [ PlayerNum ] . Vitality ) * Energy_Gain_Per_Vit_Point [ Me [ PlayerNum ] . character_class ];
    
    //--------------------
    // The maximum mana value computed from the primary stats
    //
    Me [ PlayerNum ] . max_temperature = 
	( Me [ PlayerNum ] . Magic )    * Mana_Gain_Per_Magic_Point [ Me [ PlayerNum ] . character_class ];

    //--------------------
    // How long can this character run until he must take a break and
    // walk a bit
    //
    Me [ PlayerNum ] . max_running_power = 
	( Me [ PlayerNum ] . Strength ) + 
	( Me [ PlayerNum ] . Dexterity ) + 
	( Me [ PlayerNum ] . Vitality ) +
	Me [ PlayerNum ] . running_power_bonus ;


    //--------------------
    // base regeneration speed set to 0.2 points per second
    Me [ PlayerNum ] . health_recovery_rate = 0.2;
    Me [ PlayerNum ] . cooling_rate = 0.2;
}; // void update_secondary_stats_from_primary_stats ( int PlayerNum )

/* ----------------------------------------------------------------------
 * Now we compute the possible damage the player character can do.
 * The damage value of course depends on the weapon type that the
 * character is using.  And depending on the weapon type (melee or
 * ranged weapon) some additional attributes will also play a role.
 * ---------------------------------------------------------------------- */
void
update_damage_tux_can_do ( int PlayerNum )
{
    if ( Me [ PlayerNum ] . weapon_item . type != (-1) )
    {
	if ( ItemMap[ Me [ PlayerNum ] . weapon_item . type ] . item_gun_angle_change != 0 )
	{
	    //--------------------
	    // Damage modifier in case of MELEE WEAPON is computed:  
	    // weapon's modifier * (100+Strength)%
	    //
	    Me [ PlayerNum ] . base_damage = Me [ PlayerNum ] . weapon_item.damage * 
		( Me [ PlayerNum ] . Strength + 100.0) / 100.0 ;
	    
	    Me [ PlayerNum ] . damage_modifier = Me [ PlayerNum ] . weapon_item . damage_modifier * 
		( Me [ PlayerNum ] . Strength + 100.0) / 100.0 ;
	    
	    //--------------------
	    // Damage AND damage modifier a modified by additional melee weapon
	    // skill:  A multiplier is applied!
	    //
	    Me [ PlayerNum ] . damage_modifier *= MeleeDamageMultiplierTable [ Me [ 0 ] . melee_weapon_skill ] ;
	    Me [ PlayerNum ] . base_damage     *= MeleeDamageMultiplierTable [ Me [ 0 ] . melee_weapon_skill ] ;
	}
	else
	{
	    //--------------------
	    // Damage modifier in case of RANGED WEAPON is computed:  
	    // weapon's modifier * (100+Dexterity)%
	    //
	    Me [ PlayerNum ] . base_damage = Me [ PlayerNum ] . weapon_item . damage * 
		( Me [ PlayerNum ] . Dexterity + 100.0 ) / 100.0 ;
	    Me [ PlayerNum ] . damage_modifier = Me [ PlayerNum ] . weapon_item . damage_modifier * 
		( Me [ PlayerNum ] . Dexterity + 100.0 ) / 100.0 ;
	    
	    //--------------------
	    // Damage AND damage modifier a modified by additional ranged weapon
	    // skill:  A multiplier is applied!
	    //
	    Me [ PlayerNum ] . damage_modifier *= RangedDamageMultiplierTable [ Me [ 0 ] . ranged_weapon_skill ] ;
	    Me [ PlayerNum ] . base_damage     *= RangedDamageMultiplierTable [ Me [ 0 ] . ranged_weapon_skill ] ;
	    
	    //--------------------
	    // Maybe there is a plugin for double damage with ranged
	    // weapons present?
	    //
	    if ( Me [ PlayerNum ] . double_ranged_damage != 0 )
	    {
		Me [ PlayerNum ] . base_damage *= 2;
		Me [ PlayerNum ] . damage_modifier *= 2;
	    }
	}
    }
    else
    {
	//--------------------
	// In case of no weapon equipped at all, we initialize
	// the damage values with some simple numbers.  Currently
	// strength and dexterity play NO ROLE in weaponless combat.
	// Maybe that should be changed for something more suitable
	// at some point...
	//
	Me [ PlayerNum ] . base_damage = 1;
	Me [ PlayerNum ] . damage_modifier = 1;
    }
}; // void update_damage_tux_can_do ( int PlayerNum )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
update_tux_armour_class ( int PlayerNum )
{
    //--------------------
    // We initialize the armour class value from the primary stat, 
    // using the dexterity value (and the 'character class')
    //
    Me [ PlayerNum ] . AC = 
	( Me [ PlayerNum ] . Dexterity - 15 ) * 
	AC_Gain_Per_Dex_Point [ Me [ PlayerNum ] . character_class ];

    //--------------------
    // Now we apply the armour bonuses from the currently equipped
    // items to the total defence value
    //
    if ( Me [ PlayerNum ] . armour_item . type != (-1) )
    {
	Me [ PlayerNum ] . AC += Me [ PlayerNum ] . armour_item . ac_bonus;
    }
    if ( Me [ PlayerNum ] . shield_item.type != (-1) )
    {
	Me [ PlayerNum ] . AC += Me [ PlayerNum ] . shield_item . ac_bonus;
    }
    if ( Me [ PlayerNum ] . special_item.type != (-1) )
    {
	Me [ PlayerNum ] . AC += Me [ PlayerNum ] . special_item . ac_bonus;
    }
    if ( Me [ PlayerNum ] . aux1_item.type != (-1) )
    {
        Me [ PlayerNum ] . AC += Me [ PlayerNum ] . aux1_item . ac_bonus;
    }
    if ( Me [ PlayerNum ] . aux2_item.type != (-1) )
    {
        Me [ PlayerNum ] . AC += Me [ PlayerNum ] . aux2_item . ac_bonus;
    }
    if ( Me [ PlayerNum ] . drive_item.type != (-1) )
    {
        Me [ PlayerNum ] . AC += Me [ PlayerNum ] . drive_item . ac_bonus;
    }

}; // void update_tux_armour_class ( int PlayerNum )


/* ----------------------------------------------------------------------
 * This function should re-compute all character stats according to the
 * currently equipped items and currenly distributed stats points.
 * ---------------------------------------------------------------------- */
void 
UpdateAllCharacterStats ( int PlayerNum )
{
    //--------------------
    // Maybe the influencer has reached a new experience level?
    // Let's check this...
    // 
    check_for_new_experience_level_reached ( PlayerNum );

    //--------------------
    // The primary status must be computed/updated first, because
    // the secondary status (chances and the like) will depend on
    // them...
    //
    update_all_primary_stats ( PlayerNum );

    //--------------------
    // At this point we know, that the primary stats of the influencer
    // have been fully computed.  So that means, that finally we can compute
    // all base SECONDARY stats, that are dependent upon the influencer primary
    // stats.  Once we are done with that, the modifiers to the secondary
    // stats can be applied as well.
    //
    update_secondary_stats_from_primary_stats ( PlayerNum );

    //--------------------
    // Now we compute the possible damage the player character can do.
    // The damage value of course depends on the weapon type that the
    // character is using.  And depending on the weapon type (melee or
    // ranged weapon) some additional attributes will also play a role.
    //
    update_damage_tux_can_do ( PlayerNum );

    //--------------------
    // Update tux armour class
    //
    update_tux_armour_class ( PlayerNum );

    //--------------------
    // So at this point we can finally apply all the modifiers to the influencers
    // SECONDARY stats due to 'magical' items and spells and the like
    //
    Me [ PlayerNum ] . light_bonus_from_tux = 0 ;
    Me [ PlayerNum ] . resist_disruptor = 0 ;
    AddInfluencerItemSecondaryBonus( & Me [ PlayerNum ] . armour_item );
    AddInfluencerItemSecondaryBonus( & Me [ PlayerNum ] . weapon_item );
    AddInfluencerItemSecondaryBonus( & Me [ PlayerNum ] . drive_item );
    AddInfluencerItemSecondaryBonus( & Me [ PlayerNum ] . shield_item );
    AddInfluencerItemSecondaryBonus( & Me [ PlayerNum ] . special_item );
    AddInfluencerItemSecondaryBonus( & Me [ PlayerNum ] . aux1_item );
    AddInfluencerItemSecondaryBonus( & Me [ PlayerNum ] . aux2_item );

    //--------------------
    // There also should be an upper limit to disruptor protection,
    // so that negative values can be avoided and also such that
    // disruptor bots don't become completely useless...
    //
    if ( Me [ PlayerNum ] . resist_disruptor > 85 ) Me [ PlayerNum ] . resist_disruptor = 85 ;
	
	
    //--------------------
    // Check player's health and temperature
    if ( Me [ PlayerNum ] . energy > Me [ PlayerNum ] . maxenergy ) Me [ PlayerNum ] . energy = Me [ PlayerNum ] . maxenergy;
    if ( Me [ PlayerNum ] . temperature < 0 ) Me [ PlayerNum ] . temperature = 0;


    //--------------------
    // Now that the defence stat is computed, we can compute the chance, that
    // a randomly chosen lv. 1 bot will hit the Tux in any given strike...
    //
    Me [ PlayerNum ] . lv_1_bot_will_hit_percentage =
	( int ) ( exp ( - 0.018 * ( (float) Me [ PlayerNum ] . AC ) ) * 100.0 );

}; // void UpdateAllCharacterStats ( void )

/* ----------------------------------------------------------------------
 * Now we print out the current skill levels in hacking skill, 
 * spellcasting, melee combat, ranged weapon combat and repairing things
 * ---------------------------------------------------------------------- */
void
show_character_screen_skills ( int player_num )
{
    
    //--------------------
    // We add some security against skill values out of allowed
    // bounds.
    //
    if ( ( Me [ player_num ] . melee_weapon_skill  < 0 ) ||
	 ( Me [ player_num ] . melee_weapon_skill >= NUMBER_OF_SKILL_LEVELS ) )
    {
	fprintf ( stderr , "\nmelee_weapon_skill: %d." , Me [ player_num ] . melee_weapon_skill );
	GiveStandardErrorMessage ( __FUNCTION__ , "\
Error: melee weapon skill seems out of bounds.",
				   PLEASE_INFORM, IS_FATAL );
    }
    DisplayText( AllSkillTexts [ Me [ player_num ] . melee_weapon_skill ] , 
		 MELEE_SKILL_X + CharacterRect.x , MELEE_SKILL_Y + CharacterRect.y , &CharacterRect , TEXT_STRETCH );

    //--------------------
    // We add some security against skill values out of allowed
    // bounds.
    //
    if ( ( Me [ player_num ] . ranged_weapon_skill < 0 ) ||
	 ( Me [ player_num ] . ranged_weapon_skill >= NUMBER_OF_SKILL_LEVELS ) )
    {
	fprintf ( stderr , "\nranged_weapon_skill: %d." , Me [ player_num ] . ranged_weapon_skill );
	GiveStandardErrorMessage ( __FUNCTION__ , "\
Error: ranged weapon skill seems out of bounds.",
				   PLEASE_INFORM, IS_FATAL );
    }
    DisplayText( AllSkillTexts [ Me [ player_num ] . ranged_weapon_skill ] , 
		 RANGED_SKILL_X + CharacterRect.x , RANGED_SKILL_Y + CharacterRect.y , &CharacterRect , TEXT_STRETCH );

    //--------------------
    // We add some security against skill values out of allowed
    // bounds.
    //
    if ( ( Me [ player_num ] . spellcasting_skill < 0 ) ||
	 ( Me [ player_num ] . spellcasting_skill >= NUMBER_OF_SKILL_LEVELS ) )
    {
	fprintf ( stderr , "\nProgramming_Skill: %d." , Me [ player_num ] . spellcasting_skill );
	GiveStandardErrorMessage ( __FUNCTION__ , "\
Error: Programming_Skill skill seems out of bounds.",
				   PLEASE_INFORM, IS_FATAL );
    }
    DisplayText( AllSkillTexts [ Me [ player_num ] . spellcasting_skill ] , 
		 SPELLCASTING_SKILL_X + CharacterRect.x , SPELLCASTING_SKILL_Y + CharacterRect.y , &CharacterRect , TEXT_STRETCH );

    //--------------------
    // We add some security against skill values out of allowed
    // bounds.
    //
    if ( ( Me [ player_num ] . hacking_skill < 0 ) ||
	 ( Me [ player_num ] . hacking_skill >= NUMBER_OF_SKILL_LEVELS ) )
    {
	fprintf ( stderr , "\nhacking_skill: %d." , Me [ player_num ] . hacking_skill );
	GiveStandardErrorMessage ( __FUNCTION__ , "\
Error: hacking skill seems out of bounds.",
				   PLEASE_INFORM, IS_FATAL );
    }
    DisplayText( AllSkillTexts [ Me [ player_num ] . hacking_skill ] , 
		 HACKING_SKILL_X + CharacterRect.x , HACKING_SKILL_Y + CharacterRect.y , &CharacterRect , TEXT_STRETCH );

    /*
      if ( Me [ player_num ] . repair_skill ) 
      DisplayText( "Yes" , CharacterRect.x + 80 , CharacterRect.y + 444 , &CharacterRect );
      else
      DisplayText( "No" , CharacterRect.x + 80 , CharacterRect.y + 444 , &CharacterRect );
    */
}; // void show_character_screen_skills ( int player_num )

/* ----------------------------------------------------------------------
 * This function displays the character screen.
 * ---------------------------------------------------------------------- */
void 
ShowCharacterScreen ( int player_num )
{
    char CharText[1000];
    point CurPos;
    
    DebugPrintf ( 2 , "\n%s(): Function call confirmed." , __FUNCTION__ );
    
    //--------------------
    // If the log is not set to visible right now, we do not need to 
    // do anything more, but to restore the usual user rectangle size
    // back to normal and to return...
    //
    if ( GameConfig.CharacterScreen_Visible == FALSE ) return;

    SetCurrentFont ( Message_BFont );
    
    // --------------------
    // We will need the current mouse position on several spots...
    //
    CurPos.x = GetMousePos_x() ;
    CurPos.y = GetMousePos_y() ;
    
    //--------------------
    // We define the right side of the user screen as the rectangle
    // for our inventory screen.
    //
    CharacterRect . x = GameConfig . screen_width - CHARACTERRECT_W;
    CharacterRect . y = 0; 
    CharacterRect . w = CHARACTERRECT_W;
    CharacterRect . h = CHARACTERRECT_H;
    
    blit_special_background ( CHARACTER_SCREEN_BACKGROUND_CODE );
    
    //--------------------
    // Now we can start to fill in the character values:
    // Name, Class, Level, Exp, Strength, Dex, ...
    //
    DisplayText( Me[0].character_name , 20 + CharacterRect.x , 18 + CharacterRect.y , &CharacterRect , TEXT_STRETCH );
    
    if ( Me [ 0 ] . is_town_guard_member )
	DisplayText( "Red Guard" , CLASS_X + CharacterRect . x , 18 + CharacterRect . y , &CharacterRect , TEXT_STRETCH );
    else
	DisplayText( "Novice" , CLASS_X + CharacterRect . x , 18 + CharacterRect . y , &CharacterRect , TEXT_STRETCH );
    
    /*
      switch ( Me[0].character_class )
      {
      case WAR_BOT:
      DisplayText( "War Bot" , CLASS_X + CharacterRect.x , 18 + CharacterRect.y , &CharacterRect );
      break;
      case SNIPER_BOT:
      DisplayText( "Sniper Bot" , CLASS_X + CharacterRect.x , 18 + CharacterRect.y , &CharacterRect );
      break;
      case MIND_BOT:
      DisplayText( "Mind Bot" , CLASS_X + CharacterRect.x , 18 + CharacterRect.y , &CharacterRect );
      break;
      default:
      DebugPrintf( 0 , "\n\nILLEGAL CHARACTER CLASS FOUND!!! ERROR!!! TERMINATING....." );
      Terminate( ERR );
      break;
      }
    */
    
    sprintf( CharText , "%4d", Me[0].exp_level );
    DisplayText( CharText , 62 + CharacterRect.x , 56 + CharacterRect.y , &CharacterRect , TEXT_STRETCH );
    
    // Me[0].Experience = RealScore;
    sprintf( CharText , "%6ld", Me[0].Experience ); // this should be the real score, sooner or later
    DisplayText( CharText , 240 + CharacterRect.x ,  EXPERIENCE_Y + CharacterRect.y , &CharacterRect , TEXT_STRETCH );
    
    sprintf( CharText , "%6ld", Me[0].ExpRequired ); 
    DisplayText( CharText , 240 + CharacterRect.x ,  NEXT_LEVEL_Y + CharacterRect.y , &CharacterRect , TEXT_STRETCH ) ;
    
    sprintf( CharText , "%6ld", Me[0].Gold ); 
    DisplayText( CharText , 240 + CharacterRect.x ,  GOLD_Y + CharacterRect.y , &CharacterRect , TEXT_STRETCH );
    
    SetCurrentFont( Message_BFont) ;
    sprintf( CharText , "%d", Me[0].base_strength );
    DisplayText( CharText , STR_BASE_X + CharacterRect.x , STR_Y + CharacterRect.y , &CharacterRect , TEXT_STRETCH );
    sprintf( CharText , "%d", Me[0].Strength );
    if ( Me[0].Strength != Me[0].base_strength ) SetCurrentFont( Red_BFont) ;
    DisplayText( CharText , STR_NOW_X + CharacterRect.x , STR_Y + CharacterRect.y , &CharacterRect , TEXT_STRETCH );
    
    SetCurrentFont( Message_BFont) ;
    sprintf( CharText , "%d", Me[0].base_magic );
    DisplayText( CharText , 100 + CharacterRect.x , MAG_Y + CharacterRect.y , &CharacterRect , TEXT_STRETCH );
    sprintf( CharText , "%d", Me[0].Magic );
    if ( Me[0].Magic != Me[0].base_magic ) SetCurrentFont( Red_BFont) ;
    DisplayText( CharText , 148 + CharacterRect.x , MAG_Y + CharacterRect.y , &CharacterRect , TEXT_STRETCH );
    
    SetCurrentFont( Message_BFont) ;
    sprintf( CharText , "%d", Me[0].base_dexterity );
    DisplayText( CharText , 100 + CharacterRect.x , DEX_Y + CharacterRect.y , &CharacterRect , TEXT_STRETCH );
    sprintf( CharText , "%d", Me[0].Dexterity );
    if ( Me[0].Dexterity != Me[0].base_dexterity ) SetCurrentFont( Red_BFont) ;
    DisplayText( CharText , 148 + CharacterRect.x , DEX_Y + CharacterRect.y , &CharacterRect , TEXT_STRETCH );
    
    SetCurrentFont( Message_BFont) ;
    sprintf( CharText , "%d", Me[0].base_vitality );
    DisplayText( CharText , 100 + CharacterRect.x , VIT_Y + CharacterRect.y , &CharacterRect , TEXT_STRETCH );
    sprintf( CharText , "%d", Me[0].Vitality );
    if ( Me[0].Vitality != Me[0].base_vitality ) SetCurrentFont( Red_BFont) ;
    DisplayText( CharText , 148 + CharacterRect.x , VIT_Y + CharacterRect.y , &CharacterRect , TEXT_STRETCH );
    
    SetCurrentFont( Message_BFont) ;
    sprintf( CharText , "%d", Me[0].points_to_distribute );
    DisplayText( CharText , 100 + CharacterRect.x , POINTS_Y + CharacterRect.y , &CharacterRect , TEXT_STRETCH );
    
    sprintf( CharText , "%d", (int) Me[0].maxenergy );
    DisplayText( CharText , 95 + CharacterRect.x , 293 + CharacterRect.y , &CharacterRect , TEXT_STRETCH );
    
    sprintf( CharText , "%d", (int) Me[0].energy );
    DisplayText( CharText , 143 + CharacterRect.x , 293 + CharacterRect.y , &CharacterRect , TEXT_STRETCH );
    
    sprintf( CharText , "%d", (int) Me[0].max_temperature );
    DisplayText( CharText , 95 + CharacterRect.x , 318 + CharacterRect.y , &CharacterRect , TEXT_STRETCH );
    
    sprintf( CharText , "%d", (int) Me[0].temperature );
    DisplayText( CharText , 143 + CharacterRect.x , 318 + CharacterRect.y , &CharacterRect , TEXT_STRETCH );
    
    sprintf( CharText , "%d-%d", (int) Me[0].base_damage , (int) Me[0].base_damage + (int) Me[0].damage_modifier );
    DisplayText( CharText , DAMAGE_X + CharacterRect.x , DAMAGE_Y + CharacterRect.y , &CharacterRect , TEXT_STRETCH );
    
    // sprintf( CharText , "%d", (int) Me[0].RechargeTimeModifier );
    sprintf( CharText , "%d", (int) Me[0].to_hit );
    strcat( CharText , "%" );
    DisplayText( CharText , RECHARGE_X + CharacterRect.x , RECHARGE_Y + CharacterRect.y , &CharacterRect , TEXT_STRETCH );
    
    sprintf( CharText , "%d", (int) Me[0].AC );
    DisplayText( CharText , AC_X + CharacterRect.x , AC_Y + CharacterRect.y , &CharacterRect , TEXT_STRETCH );
    
    sprintf( CharText , " %d%%", (int) Me [ 0 ] . lv_1_bot_will_hit_percentage );
    DisplayText( CharText , LV_1_BOT_HITS_CHANCE_X + CharacterRect.x , LV_1_BOT_HITS_CHANCE_Y + CharacterRect.y , &CharacterRect , TEXT_STRETCH );

    //--------------------
    // We fill in the numbers for the current running power.
    //
    sprintf( CharText , "%3d", (int) Me [ 0 ] . max_running_power );
    DisplayText( CharText , 223 + CharacterRect.x , 275 + CharacterRect.y , &CharacterRect , TEXT_STRETCH );
    sprintf( CharText , "%3d", (int) Me [ 0 ] . running_power );
    DisplayText( CharText , 269 + CharacterRect.x , 275 + CharacterRect.y , &CharacterRect , TEXT_STRETCH );


    sprintf( CharText , "%3d", (int) Me [ 0 ] . resist_disruptor );
    DisplayText( CharText , 269 + CharacterRect.x , 305 + CharacterRect.y , &CharacterRect , TEXT_STRETCH );
    
    //--------------------
    // Now we print out the current skill levels in hacking skill, 
    // spellcasting, melee combat, ranged weapon combat and repairing things
    //
    show_character_screen_skills ( player_num );
    if ( Me[0].points_to_distribute > 0 )
    {
	ShowGenericButtonFromList ( MORE_STR_BUTTON );
	ShowGenericButtonFromList ( MORE_DEX_BUTTON );
	ShowGenericButtonFromList ( MORE_VIT_BUTTON );
	ShowGenericButtonFromList ( MORE_MAG_BUTTON );
    }
}; //ShowCharacterScreen ( int player_num ) 

/* ----------------------------------------------------------------------
 * This function handles input for the character screen.
 * ---------------------------------------------------------------------- */
void
HandleCharacterScreen ( int player_num )
{

    if ( ! GameConfig . CharacterScreen_Visible ) return;
    //--------------------
    // It might be the case, that the character has some points to distribute upon the character
    // stats.  Then of course, we must display the plus button instead of all character 'now' values
    //
    // Me[0].points_to_distribute = 5;
    if ( Me[0].points_to_distribute > 0 )
    {
	if ( MouseCursorIsOnButton( MORE_STR_BUTTON , GetMousePos_x()  , GetMousePos_y()  ) && MouseLeftClicked() )
	{
	    Me[0].base_strength++;
	    Me[0].points_to_distribute--;
	    if ( Me[0].points_to_distribute <= 0 )
	    {
		while ( MouseLeftPressed() );
	    }
	}
	if ( MouseCursorIsOnButton( MORE_DEX_BUTTON , GetMousePos_x()  , GetMousePos_y()  ) && MouseLeftClicked() )
	{
	    Me[0].base_dexterity++;
	    Me[0].points_to_distribute--;
	    if ( Me[0].points_to_distribute <= 0 )
	    {
		while ( MouseLeftPressed() );
	    }
	}
	if ( MouseCursorIsOnButton( MORE_MAG_BUTTON , GetMousePos_x()  , GetMousePos_y()  ) && MouseLeftClicked() )
	{
	    Me[0].base_magic++;
	    Me[0].points_to_distribute--;
	    Me[0].max_temperature += Mana_Gain_Per_Magic_Point [ Me [ 0 ] . character_class ];
	    if ( Me[0].points_to_distribute <= 0 )
	    {
		while ( MouseLeftPressed() );
	    }
	}
	if ( MouseCursorIsOnButton( MORE_VIT_BUTTON , GetMousePos_x()  , GetMousePos_y()  ) && MouseLeftClicked() )
	{
	    Me[0].base_vitality++;
	    Me[0].points_to_distribute--;
	    // Me[0].health += Energy_Gain_Per_Vit_Point [ Me [ PlayerNum ] . character_class ];	  
	    // Me[0].energy += Energy_Gain_Per_Vit_Point [ Me [ PlayerNum ] . character_class ];	  
	    Me[0].health += Energy_Gain_Per_Vit_Point [ Me [ 0 ] . character_class ];	  
	    Me[0].energy += Energy_Gain_Per_Vit_Point [ Me [ 0 ] . character_class ];	  
	    if ( Me[0].points_to_distribute <= 0 )
	    {
		while ( MouseLeftPressed() );
	    }
	}
	
	//--------------------
	// It might happen that the last str point was just spent.  Then we can
	// automatically close the character window for convenience of the player.
	// Update by A.H.: inverted this behavior for convenience of the player
	// if ( Me[0].points_to_distribute == 0 ) GameConfig.CharacterScreen_Visible = FALSE;
    }
    
    
}; // HandleCharacterScreen ( void )

#undef _character_c
