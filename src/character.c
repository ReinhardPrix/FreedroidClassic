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
 * This file contains all the functions managing the character attributes
 * and the character stats.
 * ---------------------------------------------------------------------- */

#define _character_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"

#include "items.h"

//--------------------
// Now we give some definitions for the computation of the
// character stats
//                                      For Warrior  For Sniper  For Hacker
float Energy_Gain_Per_Vit_Point[]={ -1 ,     2 ,        1.5 ,        1 };
float Mana_Gain_Per_Magic_Point[]={ -1 ,     1 ,        1.5 ,        2 };
float AC_Gain_Per_Dex_Point[]={     -1 ,     1 ,         1  ,        1 };

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

#define STR_BASE_X 100
#define STR_NOW_X 148
#define STR_Y 143
#define MAG_Y 171
#define DEX_Y 197
#define VIT_Y 227
#define POINTS_Y 252

#define BUTTON_MOD_X (-6)
#define BUTTON_MOD_Y (-4)
#define BUTTON_WIDTH 35
#define BUTTON_HEIGHT 19

#define DAMAGE_X 260
#define DAMAGE_Y 225

#define RECHARGE_X 260
#define RECHARGE_Y 200

#define AC_X 260
#define AC_Y 171

#define CHARACTERRECT_X (SCREEN_WIDTH/2)
#define CHARACTERRECT_W (SCREEN_WIDTH/2)
#define CHARACTERRECT_H (User_Rect.h)

#define MELEE_SKILL_X 130
#define MELEE_SKILL_Y 346
#define RANGED_SKILL_X 130
#define RANGED_SKILL_Y 373
#define SPELLCASTING_SKILL_X 130
#define SPELLCASTING_SKILL_Y 398
#define HACKING_SKILL_X 130
#define HACKING_SKILL_Y 421

SDL_Rect CharacterRect;

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
  Me [ PlayerNum ] .autofire = FALSE;
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
      
      Me [ PlayerNum ] .drive_item.type = ITEM_ANTIGRAV_BETA;
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
	  
      Me [ PlayerNum ] .drive_item.type = ITEM_ANTIGRAV_BETA;
      Me [ PlayerNum ] .Inventory[ 0 ].type = ITEM_SHORT_BOW;
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
      Me [ PlayerNum ] .drive_item.type = ITEM_ANTIGRAV_ALPHA;
      
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
  Me [ PlayerNum ] .mana = Me [ PlayerNum ] .maxmana;
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
  static int WasPressed;

  //--------------------
  // Now we can draw either the plus button or the 'cha' button, depending
  // on whether there are points to distribute or not
  //
  if ( Me[0].points_to_distribute > 0 )
    {
      ShowGenericButtonFromList ( PLUS_BUTTON );
    }
  else
    {
      ShowGenericButtonFromList ( CHA_BUTTON );
    }
  ShowGenericButtonFromList ( INV_BUTTON );
  ShowGenericButtonFromList ( SKI_BUTTON );



  if ( CursorIsOnButton( INV_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) && axis_is_active && !WasPressed )
    {
      GameConfig.Inventory_Visible = ! GameConfig.Inventory_Visible;
    }

  if ( CursorIsOnButton( CHA_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) && axis_is_active && !WasPressed )
    {
      GameConfig.CharacterScreen_Visible = ! GameConfig.CharacterScreen_Visible;
    }

  if ( CursorIsOnButton( SKI_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) && axis_is_active && !WasPressed )
    {
      GameConfig.SkillScreen_Visible = ! GameConfig.SkillScreen_Visible;
    }

  WasPressed = axis_is_active;

}; // void DisplayButtons( void )

/* ----------------------------------------------------------------------
 * This function checks if a given screen position lies within the 
 * strength plus button or not
 * ---------------------------------------------------------------------- */
int
CursorIsOnStrButton( int x , int y )
{
  if ( x > CHARACTERRECT_X + BUTTON_MOD_X + STR_NOW_X + BUTTON_WIDTH ) return ( FALSE );
  if ( x < CHARACTERRECT_X + BUTTON_MOD_X + STR_NOW_X ) return ( FALSE );
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
      for ( i = 0 ; i < NUMBER_OF_SKILLS ; i ++ ) 
	{
	  Me [ 0 ] . SkillLevel [ i ] += 2 ;
	}
      // Me [ 0 ] . spell_level_bonus = 2 ;
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

      Me[0].to_hit    += BonusItem->bonus_to_tohit ;
      Me[0].maxmana   += BonusItem->bonus_to_force ;
      Me[0].maxenergy += BonusItem->bonus_to_life ; 
      Me[0].Vitality  += BonusItem->bonus_to_vit ;

      Me[0].resist_force       += BonusItem->bonus_to_resist_force ;
      Me[0].resist_fire        += BonusItem->bonus_to_resist_fire ;
      Me[0].resist_electricity += BonusItem->bonus_to_resist_electricity ;

      // if ( ItemMap [ BonusItem->type ] . can_be_installed_in_weapon_slot )
      // Me[0].freezing_enemys_property += BonusItem->freezing_time_in_seconds;
    }

}; // void AddInfluencerItemSecondaryBonus( item* BonusItem )

/* ----------------------------------------------------------------------
 * This function should re-compute all character stats according to the
 * currently equipped items and currenly distributed stats points.
 * ---------------------------------------------------------------------- */
void 
UpdateAllCharacterStats ( int PlayerNum )
{
  int BaseExpRequired = 2000;
  int i;

  //--------------------
  // Maybe the influencer has reached a new experience level?
  // Let's check this.
  // 
  Me [ PlayerNum ] .ExpRequired = BaseExpRequired * ( exp ( ( Me [ PlayerNum ] .exp_level - 1 ) * log ( 2 ) ) ) ;

  if ( Me [ PlayerNum ] .Experience > Me [ PlayerNum ] .ExpRequired ) 
    {
      Me [ PlayerNum ] .exp_level ++ ;
      Me [ PlayerNum ] .points_to_distribute += 10;

      //--------------------
      // Like in the Gothic 1 game, maximum life force will now automatically
      // be increased upon reaching a new character level.
      //
      Me [ PlayerNum ] .base_vitality += 10;

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

  //--------------------
  // Now we base PRIMARY stats
  //
  Me [ PlayerNum ] .Strength = Me [ PlayerNum ] .base_strength;
  Me [ PlayerNum ] .Dexterity = Me [ PlayerNum ] .base_dexterity;
  Me [ PlayerNum ] .Magic = Me [ PlayerNum ] .base_magic;
  Me [ PlayerNum ] .Vitality = Me [ PlayerNum ] .base_vitality;

  Me [ PlayerNum ] . freezing_melee_targets = 0;
  Me [ PlayerNum ] . double_ranged_damage = FALSE;
  // Me [ PlayerNum ] . spell_level_bonus = 0 ;

  //--------------------
  // Now we re-initialize the SKILL LEVELS
  //
  for ( i = 0 ; i < NUMBER_OF_SKILLS ; i ++ ) 
    {
      Me [ PlayerNum ] . SkillLevel [ i ] = Me [ PlayerNum ] . base_skill_level [ i ] ;
    }



  //--------------------
  // Now we add all bonuses to the influencers PRIMARY stats
  //
  AddInfluencerItemAttributeBonus( & Me [ PlayerNum ] .armour_item );
  AddInfluencerItemAttributeBonus( & Me [ PlayerNum ] .weapon_item );
  AddInfluencerItemAttributeBonus( & Me [ PlayerNum ] .drive_item );
  AddInfluencerItemAttributeBonus( & Me [ PlayerNum ] .shield_item );
  AddInfluencerItemAttributeBonus( & Me [ PlayerNum ] .special_item );
  AddInfluencerItemAttributeBonus( & Me [ PlayerNum ] .aux1_item );
  AddInfluencerItemAttributeBonus( & Me [ PlayerNum ] .aux2_item );

  //--------------------
  // At this point we know, that the primary stats of the influencer
  // have been fully computed.  So that means, that finally we can compute
  // all base SECONDARY stats, that are dependent upon the influencer primary
  // stats.  Once we are done with that, the modifiers to the secondary
  // stats can be applied as well.
  //
  Me [ PlayerNum ] .to_hit = 60 + ( Me [ PlayerNum ] .Dexterity - 15 ) * TOHIT_PERCENT_PER_DEX_POINT;

  //  Me [ PlayerNum ] .maxenergy = (Me [ PlayerNum ] .Vitality) * ENERGY_GAIN_PER_VIT_POINT;
  Me [ PlayerNum ] .maxenergy = (Me [ PlayerNum ] .Vitality) * Energy_Gain_Per_Vit_Point [ Me [ PlayerNum ] . character_class ];

  // Me [ PlayerNum ] .maxmana   = (Me [ PlayerNum ] .Magic)    * MANA_GAIN_PER_MAGIC_POINT;
  Me [ PlayerNum ] .maxmana   = (Me [ PlayerNum ] .Magic)    * Mana_Gain_Per_Magic_Point [ Me [ PlayerNum ] . character_class ];
  // This includes damage done as well...
  if ( Me [ PlayerNum ] .weapon_item.type != (-1) )
    {
      if ( ItemMap[ Me [ PlayerNum ] .weapon_item.type ].item_gun_angle_change != 0 )
	{
	  Me [ PlayerNum ] .base_damage = Me [ PlayerNum ] .weapon_item.damage * 
	    ( Me [ PlayerNum ] .Strength + 100.0) / 100.0 ;

	  //--------------------
	  // Damage modifier is computed:  
	  // WEAPON's modifier * (100+Strenth)%
	  //
	  Me [ PlayerNum ] .damage_modifier = Me [ PlayerNum ] .weapon_item.damage_modifier * 
	    ( Me [ PlayerNum ] .Strength + 100.0) / 100.0 ;

	  //--------------------
	  // Damage AND damage modifier a modified by additional melee weapon
	  // skill:  A multiplier is applied!
	  //
	  Me [ PlayerNum ] .damage_modifier *= MeleeDamageMultiplierTable [ Me [ 0 ] . melee_weapon_skill ] ;
	  Me [ PlayerNum ] .base_damage     *= MeleeDamageMultiplierTable [ Me [ 0 ] . melee_weapon_skill ] ;

	}
      else
	{

	  //--------------------
	  // Damage modifier is computed:  
	  // WEAPON's modifier * (100+Dexterity)%
	  //
	  Me [ PlayerNum ] .base_damage = Me [ PlayerNum ] .weapon_item.damage * 
	    ( Me [ PlayerNum ] .Dexterity + 100.0) / 100.0 ;
	  Me [ PlayerNum ] .damage_modifier = Me [ PlayerNum ] .weapon_item.damage_modifier * 
	    ( Me [ PlayerNum ] .Dexterity + 100.0) / 100.0 ;

	  //--------------------
	  // Damage AND damage modifier a modified by additional ranged weapon
	  // skill:  A multiplier is applied!
	  //
	  Me [ PlayerNum ] .damage_modifier *= RangedDamageMultiplierTable [ Me [ 0 ] . ranged_weapon_skill ] ;
	  Me [ PlayerNum ] .base_damage     *= RangedDamageMultiplierTable [ Me [ 0 ] . ranged_weapon_skill ] ;

	  //--------------------
	  // Maybe there is a plugin for double damage present ?
	  //
	  if ( Me [ PlayerNum ] . double_ranged_damage != 0 )
	    {
	      Me [ PlayerNum ] .base_damage *= 2;
	      Me [ PlayerNum ] .damage_modifier *= 2;
	    }

	}
    }
  else
    {
      Me [ PlayerNum ] .base_damage = 1;
      Me [ PlayerNum ] .damage_modifier = 1;
    }
  // ... and also armour class
  Me [ PlayerNum ] .AC = ( Me [ PlayerNum ] .Dexterity - 15 ) * AC_Gain_Per_Dex_Point [ Me [ PlayerNum ] . character_class ];
  if ( Me [ PlayerNum ] .armour_item.type != (-1) )
    {
      Me [ PlayerNum ] .AC += Me [ PlayerNum ] .armour_item.ac_bonus;
    }
  if ( Me [ PlayerNum ] .shield_item.type != (-1) )
    {
      Me [ PlayerNum ] .AC += Me [ PlayerNum ] .shield_item.ac_bonus;
    }
  if ( Me [ PlayerNum ] .special_item.type != (-1) )
    {
      Me [ PlayerNum ] .AC += Me [ PlayerNum ] .special_item.ac_bonus;
    }

  //--------------------
  // So at this point we can finally apply all the modifiers to the influencers
  // SECONDARY stats due to 'magical' items and spells and the like
  //
  AddInfluencerItemSecondaryBonus( & Me [ PlayerNum ] .armour_item );
  AddInfluencerItemSecondaryBonus( & Me [ PlayerNum ] .weapon_item );
  AddInfluencerItemSecondaryBonus( & Me [ PlayerNum ] .drive_item );
  AddInfluencerItemSecondaryBonus( & Me [ PlayerNum ] .shield_item );
  AddInfluencerItemSecondaryBonus( & Me [ PlayerNum ] .special_item );
  AddInfluencerItemSecondaryBonus( & Me [ PlayerNum ] .aux1_item );
  AddInfluencerItemSecondaryBonus( & Me [ PlayerNum ] .aux2_item );


}; // void UpdateAllCharacterStats ( void )


/* ----------------------------------------------------------------------
 * This function displays the character screen.
 * ---------------------------------------------------------------------- */
void 
ShowCharacterScreen ( void )
{
  // static SDL_Rect ButtonRect;
  static SDL_Surface *CharacterScreenImage = NULL;
  // static SDL_Surface *PlusButtonImage = NULL;
  SDL_Surface *tmp = NULL;
  char *fpath;
  char CharText[1000];
  // SDL_Rect TargetRect;
  static int MouseButtonPressedPreviousFrame = FALSE;
  point CurPos;

  DebugPrintf (2, "\nvoid ShowInventoryMessages( ... ): Function call confirmed.");

  //--------------------
  // If the log is not set to visible right now, we do not need to 
  // do anything more, but to restore the usual user rectangle size
  // back to normal and to return...
  //
  if ( GameConfig.CharacterScreen_Visible == FALSE ) return;

  // --------------------
  // We will need the current mouse position on several spots...
  //
  CurPos.x = GetMousePos_x() + 16;
  CurPos.y = GetMousePos_y() + 16;

  // --------------------
  // Some things like the loading of the character screen
  // need to be done only once at the first call of this
  // function. 
  //
  if ( CharacterScreenImage == NULL )
    {
      // SDL_FillRect( Screen, & InventoryRect , 0x0FFFFFF );
      fpath = find_file ( CHARACTER_SCREEN_BACKGROUND_FILE , GRAPHICS_DIR, FALSE);
      tmp = IMG_Load( fpath );
      CharacterScreenImage = SDL_DisplayFormat ( tmp );
      SDL_FreeSurface ( tmp );

      /*
      fpath = find_file ( "mouse_buttons/PlusButton.png" , GRAPHICS_DIR, FALSE);
      tmp = IMG_Load( fpath );
      PlusButtonImage = SDL_DisplayFormat ( tmp );
      SDL_FreeSurface ( tmp );
      */

      //--------------------
      // We define the right side of the user screen as the rectangle
      // for our inventory screen.
      //
      CharacterRect.x = CHARACTERRECT_X;
      CharacterRect.y = 0; 
      CharacterRect.w = CHARACTERRECT_W;
      CharacterRect.h = CHARACTERRECT_H;
    }
  
  //--------------------
  // At this point we know, that the character screen is desired and must be
  // displayed in-game:
  //
  // Into this inventory rectangle we draw the inventory mask
  //
  SDL_SetClipRect( Screen, NULL );
  SDL_BlitSurface ( CharacterScreenImage , NULL , Screen , &CharacterRect );

  //--------------------
  // Now we can start to fill in the character values:
  // Name, Class, Level, Exp, Strength, Dex, ...
  //
  DisplayText( Me[0].character_name , 20 + CharacterRect.x , 18 + CharacterRect.y , &CharacterRect );
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

  sprintf( CharText , "%4d", Me[0].exp_level );
  DisplayText( CharText , 62 + CharacterRect.x , 56 + CharacterRect.y , &CharacterRect );

  // Me[0].Experience = RealScore;
  sprintf( CharText , "%6ld", Me[0].Experience ); // this should be the real score, sooner or later
  DisplayText( CharText , 240 + CharacterRect.x ,  EXPERIENCE_Y + CharacterRect.y , &CharacterRect );

  sprintf( CharText , "%6ld", Me[0].ExpRequired ); 
  DisplayText( CharText , 240 + CharacterRect.x ,  NEXT_LEVEL_Y + CharacterRect.y , &CharacterRect );

  sprintf( CharText , "%6ld", Me[0].Gold ); 
  DisplayText( CharText , 240 + CharacterRect.x ,  GOLD_Y + CharacterRect.y , &CharacterRect );

  SetCurrentFont( FPS_Display_BFont) ;
  sprintf( CharText , "%d", Me[0].base_strength );
  DisplayText( CharText , STR_BASE_X + CharacterRect.x , STR_Y + CharacterRect.y , &CharacterRect );
  sprintf( CharText , "%d", Me[0].Strength );
  if ( Me[0].Strength != Me[0].base_strength ) SetCurrentFont( Red_BFont) ;
  DisplayText( CharText , STR_NOW_X + CharacterRect.x , STR_Y + CharacterRect.y , &CharacterRect );

  SetCurrentFont( FPS_Display_BFont) ;
  sprintf( CharText , "%d", Me[0].base_magic );
  DisplayText( CharText , 100 + CharacterRect.x , MAG_Y + CharacterRect.y , &CharacterRect );
  sprintf( CharText , "%d", Me[0].Magic );
  if ( Me[0].Magic != Me[0].base_magic ) SetCurrentFont( Red_BFont) ;
  DisplayText( CharText , 148 + CharacterRect.x , MAG_Y + CharacterRect.y , &CharacterRect );

  SetCurrentFont( FPS_Display_BFont) ;
  sprintf( CharText , "%d", Me[0].base_dexterity );
  DisplayText( CharText , 100 + CharacterRect.x , DEX_Y + CharacterRect.y , &CharacterRect );
  sprintf( CharText , "%d", Me[0].Dexterity );
  if ( Me[0].Dexterity != Me[0].base_dexterity ) SetCurrentFont( Red_BFont) ;
  DisplayText( CharText , 148 + CharacterRect.x , DEX_Y + CharacterRect.y , &CharacterRect );

  SetCurrentFont( FPS_Display_BFont) ;
  sprintf( CharText , "%d", Me[0].base_vitality );
  DisplayText( CharText , 100 + CharacterRect.x , VIT_Y + CharacterRect.y , &CharacterRect );
  sprintf( CharText , "%d", Me[0].Vitality );
  if ( Me[0].Vitality != Me[0].base_vitality ) SetCurrentFont( Red_BFont) ;
  DisplayText( CharText , 148 + CharacterRect.x , VIT_Y + CharacterRect.y , &CharacterRect );

  SetCurrentFont( FPS_Display_BFont) ;
  sprintf( CharText , "%d", Me[0].points_to_distribute );
  DisplayText( CharText , 100 + CharacterRect.x , POINTS_Y + CharacterRect.y , &CharacterRect );

  sprintf( CharText , "%d", (int) Me[0].maxenergy );
  DisplayText( CharText , 95 + CharacterRect.x , 293 + CharacterRect.y , &CharacterRect );

  sprintf( CharText , "%d", (int) Me[0].energy );
  DisplayText( CharText , 143 + CharacterRect.x , 293 + CharacterRect.y , &CharacterRect );

  sprintf( CharText , "%d", (int) Me[0].maxmana );
  DisplayText( CharText , 95 + CharacterRect.x , 318 + CharacterRect.y , &CharacterRect );

  sprintf( CharText , "%d", (int) Me[0].mana );
  DisplayText( CharText , 143 + CharacterRect.x , 318 + CharacterRect.y , &CharacterRect );

  sprintf( CharText , "%d-%d", (int) Me[0].base_damage , (int) Me[0].base_damage + (int) Me[0].damage_modifier );
  DisplayText( CharText , DAMAGE_X + CharacterRect.x , DAMAGE_Y + CharacterRect.y , &CharacterRect );

  // sprintf( CharText , "%d", (int) Me[0].RechargeTimeModifier );
  sprintf( CharText , "%d", (int) Me[0].to_hit );
  strcat( CharText , "%" );
  DisplayText( CharText , RECHARGE_X + CharacterRect.x , RECHARGE_Y + CharacterRect.y , &CharacterRect );

  sprintf( CharText , "%d", (int) Me[0].AC );
  DisplayText( CharText , AC_X + CharacterRect.x , AC_Y + CharacterRect.y , &CharacterRect );

  //--------------------
  // Now we print out the current skill levels in hacking skill, 
  // spellcasting, melee combat, ranged weapon combat and repairing things
  //
  DisplayText( AllSkillTexts [ Me [ 0 ] . melee_weapon_skill ] , 
	       MELEE_SKILL_X + CharacterRect.x , MELEE_SKILL_Y + CharacterRect.y , &CharacterRect );
  DisplayText( AllSkillTexts [ Me [ 0 ] . ranged_weapon_skill ] , 
	       RANGED_SKILL_X + CharacterRect.x , RANGED_SKILL_Y + CharacterRect.y , &CharacterRect );
  DisplayText( AllSkillTexts [ Me [ 0 ] . spellcasting_skill ] , 
	       SPELLCASTING_SKILL_X + CharacterRect.x , SPELLCASTING_SKILL_Y + CharacterRect.y , &CharacterRect );
  DisplayText( AllSkillTexts [ Me [ 0 ] . hacking_skill ] , 
	       HACKING_SKILL_X + CharacterRect.x , HACKING_SKILL_Y + CharacterRect.y , &CharacterRect );
  if ( Me [ 0 ] . repair_skill ) 
    DisplayText( "Yes" , CharacterRect.x + 80 , CharacterRect.y + 444 , &CharacterRect );
  else
    DisplayText( "No" , CharacterRect.x + 80 , CharacterRect.y + 444 , &CharacterRect );

  /*

  //--------------------
  // It might be the case, that the character has some points to distribute upon the character
  // stats.  Then of course, we must display the plus button instead of all character 'now' values
  //
  // Me[0].points_to_distribute = 5;
  if ( Me[0].points_to_distribute > 0 )
    {
      ButtonRect.x = STR_NOW_X + BUTTON_MOD_X + CharacterRect.x;
      ButtonRect.y = STR_Y + BUTTON_MOD_Y + CharacterRect.y;
      SDL_BlitSurface ( PlusButtonImage , NULL , Screen , &ButtonRect );
      ButtonRect.x = STR_NOW_X + BUTTON_MOD_X + CharacterRect.x;
      ButtonRect.y = MAG_Y + BUTTON_MOD_Y + CharacterRect.y;
      SDL_BlitSurface ( PlusButtonImage , NULL , Screen , &ButtonRect );
      ButtonRect.x = STR_NOW_X + BUTTON_MOD_X + CharacterRect.x;
      ButtonRect.y = DEX_Y + BUTTON_MOD_Y + CharacterRect.y;
      SDL_BlitSurface ( PlusButtonImage , NULL , Screen , &ButtonRect );
      ButtonRect.x = STR_NOW_X + BUTTON_MOD_X + CharacterRect.x;
      ButtonRect.y = VIT_Y + BUTTON_MOD_Y + CharacterRect.y;
      SDL_BlitSurface ( PlusButtonImage , NULL , Screen , &ButtonRect );

      if ( CursorIsOnStrButton( CurPos.x , CurPos.y ) && ( axis_is_active ) && ( ! MouseButtonPressedPreviousFrame ) )
	{
	  Me[0].base_strength++;
	  Me[0].points_to_distribute--;
	}
      if ( CursorIsOnDexButton( CurPos.x , CurPos.y ) && ( axis_is_active ) && ( ! MouseButtonPressedPreviousFrame ) )
	{
	  Me[0].base_dexterity++;
	  Me[0].points_to_distribute--;
	}
      if ( CursorIsOnMagButton( CurPos.x , CurPos.y ) && ( axis_is_active ) && ( ! MouseButtonPressedPreviousFrame ) )
	{
	  Me[0].base_magic++;
	  Me[0].points_to_distribute--;
	  Me[0].mana += Mana_Gain_Per_Magic_Point [ Me [ 0 ] . character_class ];
	}
      if ( CursorIsOnVitButton( CurPos.x , CurPos.y ) && ( axis_is_active ) && ( ! MouseButtonPressedPreviousFrame ) )
	{
	  Me[0].base_vitality++;
	  Me[0].points_to_distribute--;
	  // Me[0].health += Energy_Gain_Per_Vit_Point [ Me [ PlayerNum ] . character_class ];	  
	  // Me[0].energy += Energy_Gain_Per_Vit_Point [ Me [ PlayerNum ] . character_class ];	  
	  Me[0].health += Energy_Gain_Per_Vit_Point [ Me [ 0 ] . character_class ];	  
	  Me[0].energy += Energy_Gain_Per_Vit_Point [ Me [ 0 ] . character_class ];	  
	}

      //--------------------
      // It might happen that the last str point was just spent.  Then we can
      // automatically close the character window for convenience of the player.
      //
      if ( Me[0].points_to_distribute == 0 ) GameConfig.CharacterScreen_Visible = FALSE;
    }

  */

  //--------------------
  // Finally, we want the part of the screen we have been editing to become
  // visible and therefore we must updated it here, since it is currently not
  // contained within the user rectangle that also gets updated every frame.
  //
  // SDL_UpdateRect( Screen , CharacterRect.x , CharacterRect.y , CharacterRect.w , CharacterRect.h );


  //--------------------
  // We want to know, if the button was pressed the previous frame when we
  // are in the next frame and back in this function.  Therefore we store
  // the current button situation, so that we can conclude on button just
  // pressed later.
  //
  MouseButtonPressedPreviousFrame = axis_is_active;

}; // ShowCharacterScreen ( void )

#undef _character_c
