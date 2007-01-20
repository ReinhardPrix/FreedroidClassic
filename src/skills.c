/* 
 *
 *   Copyright (c) 2002, 2003 Johannes Prix
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
 * This file contains all the functions managing the character skills,
 * which means all the special functions and also the spells of the
 * players character.
 * ---------------------------------------------------------------------- */

#define _skills_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"

#define CLASS_X 175

#define EXPERIENCE_Y 55
#define NEXT_LEVEL_Y 82

#define GOLD_Y 132

#define STR_BASE_X 100
#define STR_NOW_X 148
#define STR_Y 143
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

#define INV_BUTTON_X 600
#define INV_BUTTON_Y 400
#define CHA_BUTTON_X 600
#define CHA_BUTTON_Y 430
#define INV_BUTTON_WIDTH 38
#define INV_BUTTON_HEIGHT 22

#define SPELL_LEVEL_BUTTONS_X 10
#define SPELL_LEVEL_BUTTONS_Y 413
#define SPELL_LEVEL_BUTTON_WIDTH 30

SDL_Rect SkillScreenRect;

/* ----------------------------------------------------------------------
 * This function clears the list of detected items for one player.  
 * Useful after level changes and when detecting items again.
 * ---------------------------------------------------------------------- */
void
ClearDetectedItemList( int PlayerNum )
{
    int i;
    
    for ( i = 0 ; i < MAX_ITEMS_PER_LEVEL ; i ++ )
    {
	Me [ PlayerNum ] . DetectedItemList [ i ] . x = 0;
	Me [ PlayerNum ] . DetectedItemList [ i ] . y = 0;
    }
}; // void ClearDetectedItemList( int PlayerNum )

/* ----------------------------------------------------------------------
 * This function improves a generic skill (hack melee ranged magic) by one
 * 
 * ---------------------------------------------------------------------- */
void
ImproveSkill( int * skill )
{
    if ( *skill >= NUMBER_OF_SKILL_LEVELS - 1) return;  
    (*skill)++;
}; // void ImproveSkill ( int * skill )

/* ----------------------------------------------------------------------
 * This function detects all items on this level.
 * ---------------------------------------------------------------------- */
void
DetectItemsSpell ( void )
{
  int SpellCost = SpellSkillMap [ SPELL_DETECT_ITEM ] . mana_cost_table [ Me[ 0 ]. spellcasting_skill ] ;
  Level AutomapLevel = curShip . AllLevels [ Me [ 0 ] . pos . z ] ;
  int i;

  if ( Me [ 0 ] . mana >= SpellCost )
    {
      Me[0].mana -= SpellCost;

      ClearDetectedItemList( 0 ); // 0 is the playernum

      // FireTuxRangedWeaponRaw ( 0 , ITEM_LASER_PISTOL , WHITE_BULLET, TRUE , 0 , 0 , 0 , 7 ) ;
      for ( i = 0 ; i < MAX_ITEMS_PER_LEVEL ; i ++ )
	{
	  if ( AutomapLevel->ItemList[i].type != (-1) )
	    {
	      Me[0].DetectedItemList[i].x = AutomapLevel->ItemList[i].pos.x ;
	      Me[0].DetectedItemList[i].y = AutomapLevel->ItemList[i].pos.y ;
	    }
	}

      // Play_Spell_ForceToEnergy_Sound( );
      Play_Spell_DetectItems_Sound( );

    }
  else
    {
      Me[0].TextVisibleTime = 0;
      Me[0].TextToBeDisplayed = "Not enough force left within me.";
      Not_Enough_Mana_Sound(  );
    }
}; // void DetectItemsSpell ( void )

/* ----------------------------------------------------------------------
 * This function creates a paralyzing bolt (spell, but really a bullet).
 * ---------------------------------------------------------------------- */
void
ParalyzeBoltSpell ( gps BoltSource )
{
  int SpellCost = SpellSkillMap [ SPELL_PARALYZE_BOLT ] . mana_cost_table [ Me[ 0 ]. spellcasting_skill ] ;
  moderately_finepoint target_location;

  target_location . x = translate_pixel_to_map_location ( 0 , ServerThinksInputAxisX ( 0 ) , ServerThinksInputAxisY ( 0 ) , TRUE ) ;
  target_location . y = translate_pixel_to_map_location ( 0 , ServerThinksInputAxisX ( 0 ) , ServerThinksInputAxisY ( 0 ) , FALSE ) ;
  if ( Me [ 0 ] . mana >= SpellCost )
  {
      Me[0].mana -= SpellCost;
      
      FireTuxRangedWeaponRaw ( 0 , ITEM_LASER_PISTOL , WHITE_BULLET, TRUE , 0 , 0 , 0 , 7 , SpellHitPercentageTable [ Me [ 0 ] . spellcasting_skill ] , target_location ) ;
      
      Play_Spell_ForceToEnergy_Sound( );
      
  }
  else
  {
      Me[0].TextVisibleTime = 0;
      Me[0].TextToBeDisplayed = "Not enough force left within me.";
      Not_Enough_Mana_Sound(  );
  }
}; // void ParalyzeBoltSpell ( gps PortalTarget )

/* ----------------------------------------------------------------------
 * This function creates a firey bolt (spell, but really a bullet).
 * ---------------------------------------------------------------------- */
void
FireyBoltSpell ( gps BoltSource )
{
  int SpellCost = SpellSkillMap [ SPELL_FIREY_BOLT ] . mana_cost_table [ Me[ 0 ]. spellcasting_skill ] ;
  moderately_finepoint target_location;

  target_location . x = translate_pixel_to_map_location ( 0 , ServerThinksInputAxisX ( 0 ) , ServerThinksInputAxisY ( 0 ) , TRUE ) ;
  target_location . y = translate_pixel_to_map_location ( 0 , ServerThinksInputAxisX ( 0 ) , ServerThinksInputAxisY ( 0 ) , FALSE ) ;

  if ( Me [ 0 ] . mana >= SpellCost )
    {
      Me[0].mana -= SpellCost;

      FireTuxRangedWeaponRaw ( 0 , ITEM_LASER_PISTOL , MAGENTA_BULLET, TRUE , 0 , 0 , 0 , 0 , SpellHitPercentageTable [ Me [ 0 ] . spellcasting_skill ] , target_location ) ;

      Play_Spell_ForceToEnergy_Sound( );

    }
  else
    {
      Me[0].TextVisibleTime = 0;
      Me[0].TextToBeDisplayed = "Not enough force left within me.";
      Not_Enough_Mana_Sound(  );
    }
}; // void FireyBoltSpell ( gps PortalTarget )

/* ----------------------------------------------------------------------
 * This function creates a cold bolt. (spell, but really a bullet).
 * ---------------------------------------------------------------------- */
void
ColdBoltSpell ( gps BoltSource )
{
  int SpellCost = SpellSkillMap [ SPELL_COLD_BOLT ] . mana_cost_table [ Me[ 0 ]. spellcasting_skill ] ;
  moderately_finepoint target_location;

  target_location . x = translate_pixel_to_map_location ( 0 , ServerThinksInputAxisX ( 0 ) , ServerThinksInputAxisY ( 0 ) , TRUE ) ;
  target_location . y = translate_pixel_to_map_location ( 0 , ServerThinksInputAxisX ( 0 ) , ServerThinksInputAxisY ( 0 ) , FALSE ) ;

  if ( Me [ 0 ] . mana >= SpellCost )
    {
      Me[0].mana -= SpellCost;

      FireTuxRangedWeaponRaw ( 0 , ITEM_LASER_PISTOL , BLUE_BULLET , TRUE , 3 , 0 , 0 , 0 , SpellHitPercentageTable [ Me [ 0 ] . spellcasting_skill ] , target_location ) ;

      Play_Spell_ForceToEnergy_Sound( );

    }
  else
    {
      Me[0].TextVisibleTime = 0;
      Me[0].TextToBeDisplayed = "Not enough force left within me.";
      Not_Enough_Mana_Sound(  );
    }
}; // void ColdBoltSpell ( ... )

/* ----------------------------------------------------------------------
 * This function creates a poison bolt.  (spell, but really a bullet)
 * ---------------------------------------------------------------------- */
void
PoisonBoltSpell ( gps BoltSource )
{
    int SpellCost = SpellSkillMap [ SPELL_POISON_BOLT ] . mana_cost_table [ Me[ 0 ] . spellcasting_skill ] ;
    moderately_finepoint target_location;
    
    target_location . x = translate_pixel_to_map_location ( 0 , ServerThinksInputAxisX ( 0 ) , ServerThinksInputAxisY ( 0 ) , TRUE ) ;
    target_location . y = translate_pixel_to_map_location ( 0 , ServerThinksInputAxisX ( 0 ) , ServerThinksInputAxisY ( 0 ) , FALSE ) ;
    
    if ( Me [ 0 ] . mana >= SpellCost )
    {
	Me[0].mana -= SpellCost;
	
	//FireTuxRangedWeaponRaw ( 0 , ITEM_LASER_RIFLE ) ;
	FireTuxRangedWeaponRaw ( 0 , ITEM_LASER_PISTOL , GREEN_BULLET , TRUE , 0 , 3 , 1 , 0 , SpellHitPercentageTable [ Me [ 0 ] . spellcasting_skill ] , target_location ) ;
	
	Play_Spell_ForceToEnergy_Sound( );
	
    }
    else
    {
	Me[0].TextVisibleTime = 0;
	Me[0].TextToBeDisplayed = "Not enough force left within me.";
	Not_Enough_Mana_Sound(  );
    }
}; // void PoisonBoltSpell ( ... )

/* ----------------------------------------------------------------------
 * This function creates a teleporter portal to the home location.
 * ---------------------------------------------------------------------- */
void
CreateTeleportal ( gps PortalTarget )
{
    int SpellCost = SpellSkillMap [ SPELL_TELEPORT_HOME ] . mana_cost_table [ Me[ 0 ]. spellcasting_skill ] ;
    
    if ( Me [ 0 ] . mana >= SpellCost )
    {
	Me[0].mana -= SpellCost;
	
	if ( curShip.AllLevels[ PortalTarget.z ]->map [ (int) PortalTarget.y ] [ (int) PortalTarget.x ] . floor_value == FLOOR )
	{
	    curShip.AllLevels[ PortalTarget.z ]->map [ (int) PortalTarget.y ] [ (int) PortalTarget.x ] . floor_value = TELE_1 ;
	}
	
	Play_Spell_ForceToEnergy_Sound( );
	
    }
    else
    {
	Me[0].TextVisibleTime = 0;
	Me[0].TextToBeDisplayed = "Not enough force left within me.";
	Not_Enough_Mana_Sound(  );
    }
}; // void CreateTeleportal ( gps PortalTarget )

/* ----------------------------------------------------------------------
 * This function creates a teleporter portal to the home location.
 * ---------------------------------------------------------------------- */
void
TeleportHome ( void )
{
    int SpellCost = SpellSkillMap [ SPELL_TELEPORT_HOME ] . mana_cost_table [ Me[ 0 ]. spellcasting_skill ] ;
    location HomeSpot;
    
    if ( Me [ 0 ] . mana >= SpellCost )
    {
	Me[0].mana -= SpellCost;
	
	

	if( (! Me [ 0 ] . teleport_anchor . x) && (! Me [ 0 ] . teleport_anchor . y)) //if there is no anchor, teleport home
        {
                Me [ 0 ] . teleport_anchor . x = Me [ 0 ] . pos . x;
                Me [ 0 ] . teleport_anchor . y = Me [ 0 ] . pos . y;
                Me [ 0 ] . teleport_anchor . z = Me [ 0 ] . pos . z;
                teleport_arrival_sound ( );
                ResolveMapLabelOnShip ( "TeleportHomeTarget" , &(HomeSpot) );
                Teleport ( HomeSpot.level , HomeSpot.x , HomeSpot.y , 0 , FALSE , TRUE ) ;
        }
        else //we must teleport back to the anchor
        {
                teleport_arrival_sound  ( );
                Teleport ( Me [ 0 ] . teleport_anchor . z , Me [ 0 ] . teleport_anchor . x , Me [ 0 ] . teleport_anchor . y , 0 , FALSE , TRUE ) ;
        }
    }
    else
    {
	Me[0].TextVisibleTime = 0;
	Me[0].TextToBeDisplayed = "Not enough force left within me.";
	Not_Enough_Mana_Sound(  );
    }
}; // void TeleportHome ( void )

/* ----------------------------------------------------------------------
 * This function handles the ForceExplosionCircle skill.
 * ---------------------------------------------------------------------- */
void
ForceExplosionCircle ( gps ExpCenter )
{
  int SpellCost = SpellSkillMap [ SPELL_FORCE_EXPLOSION_CIRCLE ] . mana_cost_table [ Me[ 0 ]. spellcasting_skill ] ;

  if ( Me[0].mana >= SpellCost )
    {
      Me[0].mana -= SpellCost;
      StartBlast ( ExpCenter.x + 1   , ExpCenter.y       , ExpCenter.z , DRUIDBLAST, Blast_Damage_Per_Second );
      StartBlast ( ExpCenter.x - 1   , ExpCenter.y       , ExpCenter.z , DRUIDBLAST, Blast_Damage_Per_Second );
      StartBlast ( ExpCenter.x       , ExpCenter.y - 1   , ExpCenter.z , DRUIDBLAST, Blast_Damage_Per_Second );
      StartBlast ( ExpCenter.x       , ExpCenter.y + 1   , ExpCenter.z , DRUIDBLAST, Blast_Damage_Per_Second );

      StartBlast ( ExpCenter.x + 0.5 , ExpCenter.y + 0.5 , ExpCenter.z , DRUIDBLAST, Blast_Damage_Per_Second );
      StartBlast ( ExpCenter.x - 0.5 , ExpCenter.y + 0.5 , ExpCenter.z , DRUIDBLAST, Blast_Damage_Per_Second );
      StartBlast ( ExpCenter.x + 0.5 , ExpCenter.y - 0.5 , ExpCenter.z , DRUIDBLAST, Blast_Damage_Per_Second );
      StartBlast ( ExpCenter.x - 0.5 , ExpCenter.y - 0.5 , ExpCenter.z , DRUIDBLAST, Blast_Damage_Per_Second );
    }
  else
    {
      Me[0].TextVisibleTime = 0;
      Me[0].TextToBeDisplayed = "Not enough force left within me.";
      Not_Enough_Mana_Sound(  );
    }
}; // void ForceExplosionCircle ( finepoint ExpCenter )

/* ----------------------------------------------------------------------
 * This function handles the RadialEMPWave skill.
 * ---------------------------------------------------------------------- */
void
RadialEMPWave ( gps ExpCenter , int SpellCostsMana )
{
    int SpellCost = SpellSkillMap [ SPELL_RADIAL_EMP_WAVE ] . mana_cost_table [ Me[ 0 ]. spellcasting_skill ] ;
    int i;
    int j;

    if ( ( Me[0].mana >= SpellCost ) || !SpellCostsMana )
    {
	//--------------------
	// For now, this spell is for free!! gratis!! yeah!! oh groovy!
	//
	if ( SpellCostsMana ) Me[0].mana -= SpellCost;
	//
	Play_Spell_ForceToEnergy_Sound( );
	
	//--------------------
	// First we find a new entry in the active spells list
	//
	for ( i = 0 ; i < MAX_ACTIVE_SPELLS ; i ++ )
	{
	    if ( AllActiveSpells [ i ] . type == (-1) ) break;
	}
	if ( i >= MAX_ACTIVE_SPELLS ) i = 0 ;
	
	//--------------------
	// Now we start our new emp wave
	//
	AllActiveSpells [ i ] . type = SPELL_RADIAL_EMP_WAVE ; 
	AllActiveSpells [ i ] . spell_center . x = Me [ 0 ] . pos . x;
	AllActiveSpells [ i ] . spell_center . y = Me [ 0 ] . pos . y;
	AllActiveSpells [ i ] . spell_radius = 0.3 ;
	AllActiveSpells [ i ] . spell_age = 0 ; 

	for ( j = 0 ; j < RADIAL_SPELL_DIRECTIONS ; j ++ )
	{
	    AllActiveSpells [ i ] . active_directions [ j ] = TRUE ; 	    
	}

    }
    else
    {
	Me[0].TextVisibleTime = 0;
	Me[0].TextToBeDisplayed = "Not enough force left within me.";
	Not_Enough_Mana_Sound(  );
    }

}; // void RadialEMPWave ( finepoint ExpCenter )

/* ----------------------------------------------------------------------
 * This function handles the RadialVMXWave skill.
 * ---------------------------------------------------------------------- */
void
RadialVMXWave ( gps ExpCenter , int SpellCostsMana )
{
    int SpellCost = SpellSkillMap [ SPELL_RADIAL_VMX_WAVE ] . mana_cost_table [ Me[ 0 ]. spellcasting_skill ] ;
    int i;
    int j;

    if ( ( Me[0].mana >= SpellCost ) || !SpellCostsMana ) 
    {
	//--------------------
	// For now, this spell is for free!! gratis!! yeah!! oh groovy!
	//
	if ( SpellCostsMana ) Me[0].mana -= SpellCost;
	//
	Play_Spell_ForceToEnergy_Sound( );
	
	//--------------------
	// First we find a new entry in the active spells list
	//
	for ( i = 0 ; i < MAX_ACTIVE_SPELLS ; i ++ )
	{
	    if ( AllActiveSpells [ i ] . type == (-1) ) break;
	}
	if ( i >= MAX_ACTIVE_SPELLS ) i = 0 ;
	
	//--------------------
	// Now we start our new emp wave
	//
	AllActiveSpells [ i ] . type = SPELL_RADIAL_VMX_WAVE ; 
	AllActiveSpells [ i ] . spell_center . x = Me [ 0 ] . pos . x;
	AllActiveSpells [ i ] . spell_center . y = Me [ 0 ] . pos . y;
	AllActiveSpells [ i ] . spell_radius = 0.3 ;
	AllActiveSpells [ i ] . spell_age = 0 ; 

	for ( j = 0 ; j < RADIAL_SPELL_DIRECTIONS ; j ++ )
	{
	    AllActiveSpells [ i ] . active_directions [ j ] = TRUE ; 	    
	}
	
    }
    else
    {
	Me[0].TextVisibleTime = 0;
	Me[0].TextToBeDisplayed = "Not enough force left within me.";
	Not_Enough_Mana_Sound(  );
    }

}; // void RadialVMXWave ( finepoint ExpCenter )

/* ----------------------------------------------------------------------
 * This function handles the RadialFireWave skill.
 * ---------------------------------------------------------------------- */
void
RadialFireWave ( gps ExpCenter , int SpellCostsMana )
{
    int SpellCost = SpellSkillMap [ SPELL_RADIAL_FIRE_WAVE ] . mana_cost_table [ Me[ 0 ]. spellcasting_skill ] ;
    int i;
    int j;
    
    if ( ( Me[0].mana >= SpellCost ) || !SpellCostsMana ) 
    {
	//--------------------
	// For now, this spell is for free!! gratis!! yeah!! oh groovy!
	//
	if ( SpellCostsMana ) Me[0].mana -= SpellCost;
	//
	Play_Spell_ForceToEnergy_Sound( );
	
	//--------------------
	// First we find a new entry in the active spells list
	//
	for ( i = 0 ; i < MAX_ACTIVE_SPELLS ; i ++ )
	{
	    if ( AllActiveSpells [ i ] . type == (-1) ) break;
	}
	if ( i >= MAX_ACTIVE_SPELLS ) i = 0 ;
	
	//--------------------
	// Now we start our new emp wave
	//
	AllActiveSpells [ i ] . type = SPELL_RADIAL_FIRE_WAVE ; 
	AllActiveSpells [ i ] . spell_center . x = Me [ 0 ] . pos . x;
	AllActiveSpells [ i ] . spell_center . y = Me [ 0 ] . pos . y;
	AllActiveSpells [ i ] . spell_radius = 0.3 ;
	AllActiveSpells [ i ] . spell_age = 0 ; 
	
	for ( j = 0 ; j < RADIAL_SPELL_DIRECTIONS ; j ++ )
	{
	    AllActiveSpells [ i ] . active_directions [ j ] = TRUE ; 	    
	}

    }
    else
    {
	Me [ 0 ] . TextVisibleTime = 0;
	Me [ 0 ] . TextToBeDisplayed = "Not enough force left within me.";
	Not_Enough_Mana_Sound(  );
    }
}; // void RadialFireWave ( finepoint ExpCenter )

/* ----------------------------------------------------------------------
 * This function handles the ForceExplosionRay skill.
 * ---------------------------------------------------------------------- */
void
ForceExplosionRay ( gps ExpCenter , float target_vector_x , float target_vector_y )
{
  int i ;
  moderately_finepoint step;
  int SpellCost = SpellSkillMap [ SPELL_FORCE_EXPLOSION_RAY ] . mana_cost_table [ Me [ 0 ] . spellcasting_skill ] ;

  if ( Me [ 0 ] . mana >= SpellCost )
    {
      Me [ 0 ] . mana -= SpellCost;
      step . x = ( target_vector_x * 0.25 ) ;
      step . y = ( target_vector_y * 0.25 ) ;

      for ( i = 1 ; i < 5 ; i ++ )
	{
	  StartBlast ( ExpCenter . x + i * step . x , ExpCenter . y + i * step . y , ExpCenter . z , DRUIDBLAST, Blast_Damage_Per_Second );
	}
    }
  else
    {
      Me [ 0 ] . TextVisibleTime = 0;
      Me [ 0 ] . TextToBeDisplayed = "Not enough force left within me.";
      Not_Enough_Mana_Sound(  );
    }

}; // void ForceExplosionRay ( gps ExpCenter , point TargetVector )

/* ----------------------------------------------------------------------
 * This function handles the ForceToEnergyConversion skill.
 * ---------------------------------------------------------------------- */
void
ForceToEnergyConversion ( void )
{
    int SpellCost = SpellSkillMap [ SPELL_FORCE_TO_ENERGY ] . mana_cost_table [ Me[ 0 ]. spellcasting_skill ] ;

    if ( Me [ 0 ] . mana >= SpellCost )
    {
	Me [ 0 ] . mana -= SpellCost;
	Me [ 0 ] . energy += 10 ; // * SpellLevel
	if ( Me [ 0 ] . energy > Me [ 0 ] . maxenergy ) Me [ 0 ] . energy = Me [ 0 ] . maxenergy ;

	healing_spell_sound ( );
    }
    else
    {
	Me [ 0 ] . TextVisibleTime = 0;
	Me [ 0 ] . TextToBeDisplayed = "Not enough force left within me.";
	Not_Enough_Mana_Sound(  );
    }

}; // void ForceToEnergyConversion ( void )

/* ----------------------------------------------------------------------
 * This function handles the skills the player might have acitivated
 * or deactivated or whatever.
 * ---------------------------------------------------------------------- */
void
HandleCurrentlyActivatedSkill( int player_num )
{
    static int RightPressedPreviousFrame = 0;
    // int i;
    // float xdist, ydist, dist2;
    // Level ChestLevel = curShip . AllLevels [ Me [ 0 ] . pos . z ] ;
    // moderately_finepoint loc_pos ;
    int index_of_droid_below_mouse_cursor = ( -1 ) ;
    int SpellCost;

    switch ( Me [ 0 ] . readied_skill )
    {
	case SPELL_WEAPON:
	    if ( MouseRightPressed ( ) != 1 ) break;
	    
	    if ( MouseCursorIsInUserRect ( GetMousePos_x()  , 
				      GetMousePos_y()  ) )
		tux_wants_to_attack_now ( 0 , TRUE );
	    
	    break;
	case  SPELL_TRANSFERMODE:
	    if ( MouseRightPressed ( ) != 1 ) break;
	    
	    if ( ! MouseCursorIsInUserRect ( GetMousePos_x() , GetMousePos_y() ) ) break;

	    index_of_droid_below_mouse_cursor = GetLivingDroidBelowMouseCursor ( player_num ) ;
	    if ( index_of_droid_below_mouse_cursor == ( -1 ) ) break;
	    if ( ! DirectLineWalkable ( Me [ player_num ] . pos . x , Me [ player_num ] . pos . y ,  translate_pixel_to_map_location ( player_num ,
                                                       (float) ServerThinksInputAxisX ( player_num ) ,
                                                       (float) ServerThinksInputAxisY ( player_num ) , TRUE ), 
							translate_pixel_to_map_location ( player_num ,
                                                       (float) ServerThinksInputAxisX ( player_num ) ,
                                                       (float) ServerThinksInputAxisY ( player_num ) , FALSE ), Me [ player_num ] . pos . z))
		break;


	    if ( AllEnemys [ index_of_droid_below_mouse_cursor ] . is_friendly )
		ChatWithFriendlyDroid( & ( AllEnemys [ index_of_droid_below_mouse_cursor ] ) );
	    else
	    {
		//--------------------
		// Only droids can be hacked.  Humans can't be 
		// hacked.
		//
		if ( ! ( Druidmap [ AllEnemys [ index_of_droid_below_mouse_cursor ] . type ] . is_human ) )
		    Takeover ( index_of_droid_below_mouse_cursor ) ;
	    }
	    break;
	case SPELL_LOOT_CHEST_OR_DEAD_BODY:

	    /*
	    //--------------------
	    // If the right mouse button wasn't pressed at all, then there
	    // is nothing to do here...
	    //
	    if ( ! MouseRightPressed() ) break;
	    
	    //--------------------
	    // Maybe we're standing right on a chest field.  That is the
	    // easiest case.  Then we just need to open the chest.
	    //
	    for ( i = 0 ; i < MAX_OBSTACLES_ON_MAP ; i ++ )
	    {
		switch ( ChestLevel -> obstacle_list [ i ] . type )
		{
		    case ISO_V_CHEST_OPEN:
		    case ISO_H_CHEST_OPEN:
			if ( ( ( Me [ 0 ] . pos . x - ChestLevel -> obstacle_list [ i ] . pos . x ) *
			       ( Me [ 0 ] . pos . x - ChestLevel -> obstacle_list [ i ] . pos . x ) +
			       ( Me [ 0 ] . pos . y - ChestLevel -> obstacle_list [ i ] . pos . y ) *
			       ( Me [ 0 ] . pos . y - ChestLevel -> obstacle_list [ i ] . pos . y ) ) < 0.5 )
			{
			    EnterChest( ChestLevel -> obstacle_list [ i ] . pos );
			    return;
			}
			break;
		    case ISO_V_CHEST_CLOSED:
		    case ISO_H_CHEST_CLOSED:
			if ( ( ( Me [ 0 ] . pos . x - ChestLevel -> obstacle_list [ i ] . pos . x ) *
			       ( Me [ 0 ] . pos . x - ChestLevel -> obstacle_list [ i ] . pos . x ) +
			       ( Me [ 0 ] . pos . y - ChestLevel -> obstacle_list [ i ] . pos . y ) *
			       ( Me [ 0 ] . pos . y - ChestLevel -> obstacle_list [ i ] . pos . y ) ) < 0.5 )
			{
			    //--------------------
			    // Later we might introduce some takeover game here for the Tux, so that
			    // the chest also has to be hacked or it will not open for the Tux without
			    // the proper key.
			    //
			    if ( ChestLevel -> obstacle_list [ i ] . type == ISO_V_CHEST_CLOSED )
				ChestLevel -> obstacle_list [ i ] . type = ISO_V_CHEST_OPEN;
			    if ( ChestLevel -> obstacle_list [ i ] . type == ISO_H_CHEST_CLOSED )
				ChestLevel -> obstacle_list [ i ] . type = ISO_H_CHEST_OPEN;
			    EnterChest( ChestLevel -> obstacle_list [ i ] . pos );
			    return;
			}
			break;
		    default:
			break;
		}
	    }
	    
	    //--------------------
	    // Now we check if maybe a dead body is close and if then
	    // the player meant to loot this dead body...
	    //
	    for ( i = 0 ; i < MAX_ENEMYS_ON_SHIP ; i++ )
	    {
		//--------------------
		// ignore enemy that are not on this level or dead 
		//
		if ( AllEnemys [ i ] . pos . z != CurLevel -> levelnum )
		    continue;
		if ( AllEnemys [ i ] . type == ( -1 ) )
		    continue;
		//--------------------
		// We determine the distance and back out immediately if there
		// is still one whole square distance or even more...
		//
		xdist = Me [ 0 ] . pos . x - AllEnemys [ i ] . pos . x;
		ydist = Me [ 0 ] . pos . y - AllEnemys [ i ] . pos . y;
		dist2 = sqrt( (xdist * xdist) + (ydist * ydist) );
		if ( dist2 < 2 * Druid_Radius_X )
		{
		    loc_pos . x = AllEnemys [ i ] . pos . x ;
		    loc_pos . y = AllEnemys [ i ] . pos . y ;
		    EnterChest( loc_pos );
		    return;
		}
	    }
	    PlayOnceNeededSoundSample ( "../effects/I_See_No_Chest_Sound_0.wav" , FALSE , FALSE );
	    break;
	    */

/*
	case SPELL_TRANSFERMODE:
	    if (MouseRightPressed() == 1)
	    {
		//--------------------
		// We switch status to transfermode
		Me[0].status = TRANSFERMODE;
		
		//--------------------
		// Now we check if maybe a console was near.  If that is so, then we
		// see how close it really is and maybe we start the console menu.
		//
		for ( i = 0 ; i < MAX_OBSTACLES_ON_MAP ; i ++ )
		{
		    switch ( ChestLevel -> obstacle_list [ i ] . type )
		    {
			case ISO_CONSOLE_S:
			case ISO_CONSOLE_N:
			case ISO_CONSOLE_E:
			case ISO_CONSOLE_W:
			    if ( ( ( Me [ 0 ] . pos . x - ChestLevel -> obstacle_list [ i ] . pos . x ) *
				   ( Me [ 0 ] . pos . x - ChestLevel -> obstacle_list [ i ] . pos . x ) +
				   ( Me [ 0 ] . pos . y - ChestLevel -> obstacle_list [ i ] . pos . y ) *
				   ( Me [ 0 ] . pos . y - ChestLevel -> obstacle_list [ i ] . pos . y ) ) < 0.4 )
			    {
				EnterConsole();
				return;
			    }
			    break;
			default:
			    break;
		    }
		}
	    }
	    break;
*/
    
	case SPELL_FORCE_EXPLOSION_CIRCLE:
	    if ( MouseRightPressed() && ( ! RightPressedPreviousFrame ) )
	    {
		if ( MouseCursorIsInUserRect ( GetMousePos_x()  , 
					  GetMousePos_y()  ) )
		    ForceExplosionCircle( Me[0].pos );
	    }
	    break;
	case SPELL_FORCE_EXPLOSION_RAY:
	    if ( MouseRightPressed() && ( ! RightPressedPreviousFrame ) )
	    {
		if ( MouseCursorIsInUserRect ( GetMousePos_x()  , 
					  GetMousePos_y()  ) )
		{
		    ForceExplosionRay ( 
			Me [ 0 ] . pos , 
			translate_pixel_to_map_location ( player_num , 
							  (float) ServerThinksInputAxisX ( player_num ) , 
							  (float) ServerThinksInputAxisY ( player_num ) , TRUE ) 
			- Me [ player_num ] . pos . x ,
			translate_pixel_to_map_location ( player_num , 
							  (float) ServerThinksInputAxisX ( player_num ) , 
							  (float) ServerThinksInputAxisY ( player_num ) , FALSE ) 
			- Me [ player_num ] . pos . y );
		}
	    }
	    break;
	case SPELL_FORCE_TO_ENERGY:
	    if ( MouseRightPressed() && ( ! RightPressedPreviousFrame ) )
	    {
		if ( MouseCursorIsInUserRect ( GetMousePos_x()  , 
					  GetMousePos_y()  ) )
		{
		    ForceToEnergyConversion ( );
		}
	    }
	    break;
	case SPELL_TELEPORT_HOME:
	    if ( MouseRightPressed() && ( ! RightPressedPreviousFrame ) )
	    {
		if ( MouseCursorIsInUserRect ( GetMousePos_x()  , 
					  GetMousePos_y()  ) )
		{
		    // TeleportHome ( TargetLocation ) ;
		    TeleportHome (  ) ;
		}
	    }
	    break;
	case SPELL_FIREY_BOLT:
	    if ( MouseRightPressed() && ( ! RightPressedPreviousFrame ) )
	    {
		if ( MouseCursorIsInUserRect ( GetMousePos_x()  , 
					  GetMousePos_y()  ) )
		{
		    FireyBoltSpell ( Me [ 0 ] . pos );
		}
	    }
	    break;
	case SPELL_COLD_BOLT:
	    if ( MouseRightPressed() && ( ! RightPressedPreviousFrame ) )
	    {
		if ( MouseCursorIsInUserRect ( GetMousePos_x()  , 
					  GetMousePos_y()  ) )
		{
		    ColdBoltSpell ( Me [ 0 ] . pos );
		}
	    }
	    break;
	case SPELL_REPAIR_SKILL:
	    if ( MouseRightPressed() && ( ! RightPressedPreviousFrame ) )
	    {
		if ( ! MouseCursorIsInInvRect( GetMousePos_x()  , 
					 GetMousePos_y()  ) 
		     || ( !GameConfig.Inventory_Visible ) )
		{
		    //--------------------
		    // Do nothing here.  The right mouse click while in inventory screen
		    // will be handled in the inventory screen management function.
		    //
		    PlayOnceNeededSoundSample ( "../effects/tux_ingame_comments/CantRepairThat.wav" , 
						FALSE , FALSE );
		}
	    }
	    break;
	case SPELL_IDENTIFY_SKILL:
	    if ( MouseRightPressed() && ( ! RightPressedPreviousFrame ) )
	    {
		//--------------------
		// Maybe the identify spell has already been triggered and
		// is activated right now.  Then of course this (second) mouse
		// click must be ignored completely.
		//
		if ( global_ingame_mode == GLOBAL_INGAME_MODE_IDENTIFY ) break;

		//--------------------
		// We need to see if sufficient mana for the identify skill
		// is present.  Only then can we deduct mana and enter
		// identification global mode.
		//
		SpellCost = SpellSkillMap [ SPELL_IDENTIFY_SKILL ] . mana_cost_table [ Me[ 0 ] . spellcasting_skill ] ;
		if ( Me [ 0 ] . mana >= SpellCost )
		{
		    Me [ 0 ] . mana -= SpellCost;
		    Play_Spell_ForceToEnergy_Sound( );

		    silently_unhold_all_items ( );
		    global_ingame_mode = GLOBAL_INGAME_MODE_IDENTIFY ;
		}
		else
		{
		    Me [ 0 ] . TextVisibleTime = 0;
		    Me [ 0 ] . TextToBeDisplayed = "Not enough force left within me.";
		    Not_Enough_Mana_Sound(  );
		}
	    }
	    break;
	case SPELL_POISON_BOLT:
	    if ( MouseRightPressed() && ( ! RightPressedPreviousFrame ) )
	    {
		if ( MouseCursorIsInUserRect ( GetMousePos_x()  , 
					  GetMousePos_y()  ) )
		{
		    PoisonBoltSpell ( Me [ 0 ] . pos );
		}
	    }
	    break;
	case SPELL_PARALYZE_BOLT:
	    if ( MouseRightPressed() && ( ! RightPressedPreviousFrame ) )
	    {
		if ( MouseCursorIsInUserRect ( GetMousePos_x()  , 
					  GetMousePos_y()  ) )
		{
		    ParalyzeBoltSpell ( Me [ 0 ] . pos );
		}
	    }
	    break;
	case SPELL_DETECT_ITEM:
	    if ( MouseRightPressed() && ( ! RightPressedPreviousFrame ) )
	    {
		if ( MouseCursorIsInUserRect ( GetMousePos_x()  , 
					  GetMousePos_y()  ) )
		{
		    DetectItemsSpell (  ) ;
		}
	    }
	    break;
	case SPELL_RADIAL_EMP_WAVE:
	    if ( MouseRightPressed() && ( ! RightPressedPreviousFrame ) )
	    {
		if ( MouseCursorIsInUserRect ( GetMousePos_x()  , 
					  GetMousePos_y()  ) )
		{
		    RadialEMPWave ( Me [ 0 ] . pos , TRUE );
		}
	    }
	    break;
	case SPELL_RADIAL_VMX_WAVE:
	    if ( MouseRightPressed() && ( ! RightPressedPreviousFrame ) )
	    {
		if ( MouseCursorIsInUserRect ( GetMousePos_x()  , 
					  GetMousePos_y()  ) )
		{
		    RadialVMXWave ( Me [ 0 ] . pos , TRUE );
		}
	    }
	    break;
	case SPELL_RADIAL_FIRE_WAVE:
	    if ( MouseRightPressed() && ( ! RightPressedPreviousFrame ) )
	    {
		if ( MouseCursorIsInUserRect ( GetMousePos_x()  , 
					  GetMousePos_y()  ) )
		{
		    RadialFireWave ( Me [ 0 ] . pos , TRUE );
		}
	    }
	    break;
	case SPELL_EXTRACT_PLASMA_TRANSISTORS : /*handle them by doing nothing so as to avoid a warning message*/
	case  SPELL_EXTRACT_SUPERCONDUCTORS : 
	case  SPELL_EXTRACT_ANTIMATTER_CONVERTERS : 
	case SPELL_EXTRACT_ENTROPY_INVERTERS : 
	case  SPELL_EXTRACT_TACHYON_CONDENSATORS : break;

	default:
	    fprintf( stderr, "\n\nMe [ 0 ] . readied_skill: %d \n" , Me [ 0 ] . readied_skill );
	    GiveStandardErrorMessage ( __FUNCTION__  , "\
There was a request for a spell/skill that is currently not handled.\n\
Nothing will be done then.  This is a warning message only.\n\
Usually this error is not severe.",
				       NO_NEED_TO_INFORM, IS_WARNING_ONLY );
	    break;
    }
    
    RightPressedPreviousFrame = MouseRightPressed() ;
    
}; // void HandleCurrentlyActivatedSkill( void )


/* ----------------------------------------------------------------------
 * This function checks if a given screen position lies within the 
 * one of the skill icons and returns the number of that skill icon.
 * ---------------------------------------------------------------------- */
int
CursorIsOnWhichSkillButton( int x , int y )
{
    //--------------------
    // First we check if the cursor is in at least horizontally
    // in the row of the skill items
    //
    if ( x > SkillScreenRect.x + 16 + 64 ) return ( -1 );
    if ( x < SkillScreenRect.x + 16      ) return ( -1 );
    
    //--------------------
    // Now we can check on which skill rectangle exactly the cursor
    // is hovering, since we know that it is hitting, horizontally
    // at least, the row of skill icons.
    //
    if ( y < SkillScreenRect.y + 16 + 0 * 64      ) return ( -1 );
    if ( y < SkillScreenRect.y + 16 + 1 * 64      ) return (  0 );
    
    if ( y < SkillScreenRect.y + 16 + 1 * 64 + 16 ) return ( -1 );
    if ( y < SkillScreenRect.y + 16 + 2 * 64 + 16 ) return (  1 );
    
    if ( y < SkillScreenRect.y + 16 + 2 * 64 + 2 * 16 ) return ( -1 );
    if ( y < SkillScreenRect.y + 16 + 3 * 64 + 2 * 16 ) return (  2 );
    
    if ( y < SkillScreenRect.y + 16 + 3 * 64 + 3 * 16 ) return ( -1 );
    if ( y < SkillScreenRect.y + 16 + 4 * 64 + 3 * 16 ) return (  3 );
    
    if ( y < SkillScreenRect.y + 16 + 4 * 64 + 4 * 16 ) return ( -1 );
    if ( y < SkillScreenRect.y + 16 + 5 * 64 + 4 * 16 ) return (  4 );
    
    // if ( y < SkillScreenRect.y + 16 + 5 * 64 + 5 * 16 ) return ( -1 );
    // if ( y < SkillScreenRect.y + 16 + 6 * 64 + 5 * 16 ) return (  5 );
    
    return ( -1 );
}; // int CursorIsOnWhichSkillButton( int x , int y )


/* ----------------------------------------------------------------------
 * This function checks if a given screen position lies within 
 * one of the spell level buttons and returns the number of that 
 * spell level button.
 * ---------------------------------------------------------------------- */
int
CursorIsOnWhichSpellPageButton( int x , int y )
{
    int i;
    
    //--------------------
    // First we check if the cursor is in at least horizontally
    // and vertically in the line with the spell level buttons.
    //
    if ( x > SkillScreenRect.x + SPELL_LEVEL_BUTTONS_X + 300 ) return ( -1 );
    if ( x < SkillScreenRect.x + SPELL_LEVEL_BUTTONS_X       ) return ( -1 );
    if ( y > SkillScreenRect.y + SPELL_LEVEL_BUTTONS_Y + 16  ) return ( -1 );
    if ( y < SkillScreenRect.y + SPELL_LEVEL_BUTTONS_Y       ) return ( -1 );
    
    //--------------------
    // Now we can check on which skill rectangle exactly the cursor
    // is hovering, since we know that it is hitting, horizontally
    // at least, the row of skill icons.
    //
    for ( i = 0 ; i < NUMBER_OF_SKILL_PAGES ; i ++ )
    {
	if ( x < SkillScreenRect.x + SPELL_LEVEL_BUTTONS_X + ( i + 1 ) * SPELL_LEVEL_BUTTON_WIDTH ) 
	    return i;
    }
    
    return ( -1 );
}; // int CursorIsOnWhichSpellLevelButton( int x , int y )

/* ---------------------------------------------------------------------- 
 *
 *
 * ---------------------------------------------------------------------- */
void
ShowSkillsExplanationScreen( void )
{
    SDL_Rect TargetSkillRect;
    
    //--------------------
    // This should draw the background...
    //
    blit_special_background ( SKILL_SCREEN_EXPLANATION_BACKGROUND_CODE ) ;

    //--------------------
    // We will use the FPS display font, cause the small one isn't 
    // very well readable on the silver background
    //
    SetCurrentFont ( FPS_Display_BFont );
    
    TargetSkillRect.x = 15;
    TargetSkillRect.y = 15;
    
    LoadOneSkillSurfaceIfNotYetLoaded ( Me [ 0 ] . readied_skill );
    
    if ( use_open_gl )
    {
	blit_open_gl_texture_to_screen_position ( SpellSkillMap [ Me [ 0 ] . readied_skill ] . spell_skill_icon_surface , 
						  TargetSkillRect . x , TargetSkillRect . y , TRUE );
    }
    else
    {
	our_SDL_blit_surface_wrapper ( SpellSkillMap [ Me [ 0 ] . readied_skill ] . spell_skill_icon_surface . surface , 
				       NULL , Screen , &TargetSkillRect );
    }
    
    
    TargetSkillRect.w = 320 - 15 - 15;
    TargetSkillRect.h = 480 - 15 ;
    DisplayText( SpellSkillMap [ Me [ 0 ] . readied_skill ] . spell_skill_description , 16 , 16+64+16, 
		 &TargetSkillRect , TEXT_STRETCH );


}; // void ShowSkillsExplanationScreen( void )

/* ---------------------------------------------------------------------- 
 * We will draw only those skills to the skills inventory, that are
 * already present in the Tux.  That way the game remains open for new
 * skills to the player and he doesn't now in advance which skills there
 * are, which is more interesting than complete control and overview.
 *
 * Any skills not in use will be marked as -1.
 *
 * The first few entries will be filled with internal skill index numbers
 * for reference.
 *
 * ---------------------------------------------------------------------- */
void
establish_skill_subset_map ( int *SkillSubsetMap )
{
  int i;
  int NextPosition=0;

  for ( i = 0 ; i < NUMBER_OF_SKILLS ; i ++ )
    {
      SkillSubsetMap [ i ] = (-1) ;
    }
  for ( i = 0 ; i < NUMBER_OF_SKILLS ; i ++ )
    {
      if ( Me [ 0 ] . SkillLevel [ i ] > 0 )
	{
	  SkillSubsetMap [ NextPosition ] = i ;
	  NextPosition++;
	}
    }
}; // void establish_skill_subset_map ( int *SkillSubsetMap );

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
activate_nth_aquired_skill ( int skill_num )
{
  int SkillSubsetMap [ NUMBER_OF_SKILLS ] ;
  int i;

  //--------------------
  // We will choose only from those skills, that have been aquired already,
  // so we prepare a list of exactly these skills...
  //
  establish_skill_subset_map ( & ( SkillSubsetMap [ 0 ] ) );

  //--------------------
  // If the n-th skill does exist, we activate the n-th skill,
  // otherwise we activate the last aquired skill.
  //
  if ( SkillSubsetMap [ skill_num ] != (-1) )
    {
      Me [ 0 ] . readied_skill = SkillSubsetMap [ skill_num ] ;
    }
  else
    {
      for ( i = 0 ; i < NUMBER_OF_SKILLS ; i ++ )
	{
	  if ( SkillSubsetMap [ i ] != (-1) )
	    Me [ 0 ] . readied_skill = SkillSubsetMap [ i ] ;
	}
    }
  
}; // void activate_nth_skill ( int skill_num )

/* ----------------------------------------------------------------------
 * This function displays the SKILLS SCREEN.  This is NOT the same as the
 * CHARACTER SCREEN.  In the skills screen you can see what skills/spells
 * you currenlty have availabe and you can select a new readied skill by
 * clicking on it with the mouse.
 * ---------------------------------------------------------------------- */
void 
ShowSkillsScreen ( void )
{
#define INTER_SKILLRECT_DIST 17
#define FIRST_SKILLRECT_Y 16

    static SDL_Rect ButtonRect;
    char CharText[1000];
    static int MouseButtonPressedPreviousFrame = FALSE;
    point CurPos;
    int i;
    SDL_Rect SpellLevelRect;
    int SkillSubsetMap [ NUMBER_OF_SKILLS ] ;
    int SkillOfThisSlot;
    point SkillRectLocations [ NUMBER_OF_SKILLS_PER_SKILL_PAGE ] ;
  
    DebugPrintf ( 2 , "\n%s(): Function call confirmed." , __FUNCTION__ );
    
    SkillScreenRect . x = CHARACTERRECT_X;
    SkillScreenRect . y = 0; 
    SkillScreenRect . w = CHARACTERRECT_W;
    SkillScreenRect . h = CHARACTERRECT_H;
    
    for ( i = 0 ; i < NUMBER_OF_SKILLS_PER_SKILL_PAGE ; i ++ )
    {
	SkillRectLocations [ i ] . x = SkillScreenRect . x + 17 ;
	SkillRectLocations [ i ] . y = SkillScreenRect.y + FIRST_SKILLRECT_Y + i * ( 64 + INTER_SKILLRECT_DIST ) + 3 ;
    }

    //--------------------
    // If the log is not set to visible right now, we do not need to 
    // do anything more, but to restore the usual user rectangle size
    // back to normal and to return...
    //
    if ( GameConfig . SkillScreen_Visible == FALSE ) return;

    //--------------------
    // We will use the FPS display font, cause the small one isn't 
    // very well readable on the silver background
    //
    SetCurrentFont ( FPS_Display_BFont );
    
    //--------------------
    // Maybe the skill circle images for clicking between different spell circles
    // have not been loaded yet.  Then it is time to do so.  If this was already
    // done before, then the function will know it and don't do anything anyway.
    //
    Load_Skill_Level_Button_Surfaces( );
    
    // --------------------
    // We will need the current mouse position on several spots...
    //
    CurPos.x = GetMousePos_x()  ;
    CurPos.y = GetMousePos_y()  ;
    
    //--------------------
    // We will draw only those skills to the skills inventory, that are
    // already present in the Tux.  That way the game remains open for new
    // skills to the player and he doesn't now in advance which skills there
    // are, which is more interesting than complete control and overview.
    //
    establish_skill_subset_map ( & ( SkillSubsetMap [ 0 ] ) );
    
    //--------------------
    // At this point we know, that the skill screen is desired and must be
    // displayed in-game:
    //
    // SDL_SetClipRect( Screen, NULL );
    // our_SDL_blit_surface_wrapper ( SkillScreenImage , NULL , Screen , &SkillScreenRect );
    //
    blit_special_background ( SKILL_SCREEN_BACKGROUND_CODE );
    
    if ( GameConfig . skill_explanation_screen_visible )
	ShowSkillsExplanationScreen( );
    
    //--------------------
    // According to the page in the spell book currently opened,
    // we draw a 'button' or activation mark over the appropriate spot
    //
    SpellLevelRect.x = SkillScreenRect.x + SPELL_LEVEL_BUTTONS_X + 
	SPELL_LEVEL_BUTTON_WIDTH * GameConfig.spell_level_visible ;
    SpellLevelRect.y = SkillScreenRect.y + SPELL_LEVEL_BUTTONS_Y ;
    our_SDL_blit_surface_wrapper ( 
	SpellLevelButtonImageList [ GameConfig.spell_level_visible ] . surface , 
	NULL , Screen , &SpellLevelRect );
    
    //--------------------
    // Now we fill in the skills available to this bot.  ( For now, these skills 
    // are not class-specific, like in diablo or something, but this is our first
    // approach to the topic after all.... :)
    //
    for ( i = 0 ; i < NUMBER_OF_SKILLS_PER_SKILL_PAGE ; i ++ )
    {
	ButtonRect.x = SkillRectLocations [ i ] . x ;
	ButtonRect.y = SkillRectLocations [ i ] . y ; 
	ButtonRect.w = 64;
	ButtonRect.h = 64;
	
	SkillOfThisSlot = SkillSubsetMap [ i + NUMBER_OF_SKILLS_PER_SKILL_PAGE * GameConfig.spell_level_visible ] ;
	if ( SkillOfThisSlot < 0 ) continue;
	
	LoadOneSkillSurfaceIfNotYetLoaded ( SkillOfThisSlot );
	
	if ( use_open_gl )
	{
	    blit_open_gl_texture_to_screen_position ( 
		SpellSkillMap [ SkillOfThisSlot ] . spell_skill_icon_surface , 
		ButtonRect . x , ButtonRect . y , TRUE );
	}
	else
	{
	    our_SDL_blit_surface_wrapper ( 
		SpellSkillMap [ SkillOfThisSlot ] . spell_skill_icon_surface . surface , 
		NULL , Screen , &ButtonRect );
	}
	
	//--------------------
	// First we write the name of the skill to the screen
	//
	DisplayText( SpellSkillMap [ SkillOfThisSlot ] . spell_skill_name , 
		     16 + 64 + 16 + SkillScreenRect.x , 
		     FIRST_SKILLRECT_Y + i * (64 + INTER_SKILLRECT_DIST) + SkillScreenRect.y , 
		     &SkillScreenRect , TEXT_STRETCH );
      
	//--------------------
	// Now we write the competence of the players character in that skill to the screen
	//
	sprintf( CharText , "Skill Level: %d " , Me[0].SkillLevel[ SkillOfThisSlot ] );
	DisplayText( CharText , 16 + 64 + 16 + SkillScreenRect.x , 
		     FIRST_SKILLRECT_Y + i * ( 64 + INTER_SKILLRECT_DIST ) + SkillScreenRect.y + FontHeight( GetCurrentFont() ) , &SkillScreenRect , TEXT_STRETCH );
	sprintf( CharText , "Mana cost: %d " , SpellSkillMap [ SkillOfThisSlot ] . mana_cost_table [Me[0]. spellcasting_skill ] );
	DisplayText( CharText , 16 + 64 + 16 + SkillScreenRect.x , 
		     FIRST_SKILLRECT_Y + i * (64 + INTER_SKILLRECT_DIST) + SkillScreenRect.y + 2 * FontHeight( GetCurrentFont() ) , &SkillScreenRect , TEXT_STRETCH );
	
    }

    //--------------------
    // Now we see if perhaps the player has just clicked on one of the skills
    // available to this class.  In this case of course we must set a different
    // skill/spell as the currently activated skill/spell.
    //
    if ( ( CursorIsOnWhichSkillButton ( CurPos.x , CurPos.y ) != ( -1 ) ) &&
	 axis_is_active && ( ! MouseButtonPressedPreviousFrame ) )
    {
	if ( SkillSubsetMap [ CursorIsOnWhichSkillButton ( CurPos.x , CurPos.y ) + 
			      NUMBER_OF_SKILLS_PER_SKILL_PAGE * GameConfig.spell_level_visible ] >= 0 ) 
	    Me[0].readied_skill = SkillSubsetMap [ CursorIsOnWhichSkillButton ( CurPos.x , CurPos.y ) + 
						   NUMBER_OF_SKILLS_PER_SKILL_PAGE * GameConfig.spell_level_visible ] ;
    }
    
    if ( MouseCursorIsOnButton ( OPEN_CLOSE_SKILL_EXPLANATION_BUTTON , CurPos.x , CurPos.y ) &&
	 axis_is_active && ( ! MouseButtonPressedPreviousFrame ) )
    {
	GameConfig.skill_explanation_screen_visible = !GameConfig.skill_explanation_screen_visible;
    }
    
    //--------------------
    // Now we see if perhaps the player has just clicked on another skill level
    // button.  In this case of course we must set a different skill/spell level
    // as the currently visible spell level.
    //
    if ( ( CursorIsOnWhichSpellPageButton ( CurPos.x , CurPos.y ) != ( -1 ) ) &&
	 axis_is_active && ( ! MouseButtonPressedPreviousFrame ) )
    {
	GameConfig.spell_level_visible = CursorIsOnWhichSpellPageButton ( CurPos.x , CurPos.y );
    }
    
    //--------------------
    // We want to know, if the button was pressed the previous frame when we
    // are in the next frame and back in this function.  Therefore we store
    // the current button situation, so that we can conclude on button just
    // pressed later.
    //
    MouseButtonPressedPreviousFrame = axis_is_active;

}; // ShowSkillsScreen ( void )

#undef _skills_c

