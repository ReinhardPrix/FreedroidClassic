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

#include "items.h"
// #include "BFont.h"

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
// #define CHARACTERRECT_Y (User_Rect.y)
#define CHARACTERRECT_W (SCREEN_WIDTH/2)
#define CHARACTERRECT_H (User_Rect.h)

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
 * This function detects all items on this level.
 * ---------------------------------------------------------------------- */
void
DetectItemsSpell ( void )
{
  // int SpellCost = ManaCostTable [ SPELL_DETECT_ITEM ][ Me[ 0 ].SkillLevel [ 6 ] ] ;
  int SpellCost = ManaCostTable [ SPELL_DETECT_ITEM ][ Me[ 0 ]. spellcasting_skill ] ;
  Level AutomapLevel = curShip . AllLevels [ Me [ 0 ] . pos . z ] ;
  int i;

  if ( Me [ 0 ] . mana >= SpellCost )
    {
      Me[0].mana -= SpellCost;

      ClearDetectedItemList( 0 ); // 0 is the playernum

      // FireTuxRangedWeaponRaw ( 0 , ITEM_SHORT_BOW , WHITE_BULLET, TRUE , 0 , 0 , 0 , 7 ) ;
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
 * This function creates a teleporter portal to the home location.
 * ---------------------------------------------------------------------- */
void
ParalyzeBoltSpell ( gps BoltSource , moderately_finepoint BoltTarget )
{
  // int SpellCost = ManaCostTable [ SPELL_PARALYZE_BOLT ][ Me[ 0 ].SkillLevel [ 6 ] ] ;
  int SpellCost = ManaCostTable [ SPELL_PARALYZE_BOLT ][ Me[ 0 ]. spellcasting_skill ] ;

  if ( Me [ 0 ] . mana >= SpellCost )
    {
      Me[0].mana -= SpellCost;

      //FireTuxRangedWeaponRaw ( 0 , ITEM_COMPOSITE_BOW ) ;
      FireTuxRangedWeaponRaw ( 0 , ITEM_SHORT_BOW , WHITE_BULLET, TRUE , 0 , 0 , 0 , 7 , SpellHitPercentageTable [ Me [ 0 ] . spellcasting_skill ] ) ;

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
 * This function creates a teleporter portal to the home location.
 * ---------------------------------------------------------------------- */
void
FireyBoltSpell ( gps BoltSource , moderately_finepoint BoltTarget )
{
  // int SpellCost = ManaCostTable [ SPELL_FIREY_BOLT ][ Me[ 0 ].SkillLevel [ 6 ] ] ;
  int SpellCost = ManaCostTable [ SPELL_FIREY_BOLT ][ Me[ 0 ]. spellcasting_skill ] ;

  if ( Me [ 0 ] . mana >= SpellCost )
    {
      Me[0].mana -= SpellCost;

      //FireTuxRangedWeaponRaw ( 0 , ITEM_COMPOSITE_BOW ) ;
      FireTuxRangedWeaponRaw ( 0 , ITEM_SHORT_BOW , MAGENTA_BULLET, TRUE , 0 , 0 , 0 , 0 , SpellHitPercentageTable [ Me [ 0 ] . spellcasting_skill ] ) ;

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
 * This function creates a teleporter portal to the home location.
 * ---------------------------------------------------------------------- */
void
ColdBoltSpell ( gps BoltSource , moderately_finepoint BoltTarget )
{
  // int SpellCost = ManaCostTable [ SPELL_COLD_BOLT ][ Me[ 0 ].SkillLevel [ 6 ] ] ;
  int SpellCost = ManaCostTable [ SPELL_COLD_BOLT ][ Me[ 0 ]. spellcasting_skill ] ;

  if ( Me [ 0 ] . mana >= SpellCost )
    {
      Me[0].mana -= SpellCost;

      //FireTuxRangedWeaponRaw ( 0 , ITEM_COMPOSITE_BOW ) ;
      FireTuxRangedWeaponRaw ( 0 , ITEM_SHORT_BOW , BLUE_BULLET , TRUE , 3 , 0 , 0 , 0 , SpellHitPercentageTable [ Me [ 0 ] . spellcasting_skill ] ) ;

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
 * This function creates a teleporter portal to the home location.
 * ---------------------------------------------------------------------- */
void
PoisonBoltSpell ( gps BoltSource , moderately_finepoint BoltTarget )
{
  int SpellCost = ManaCostTable [ SPELL_POISON_BOLT ][ Me[ 0 ] . spellcasting_skill ] ;

  if ( Me [ 0 ] . mana >= SpellCost )
    {
      Me[0].mana -= SpellCost;

      //FireTuxRangedWeaponRaw ( 0 , ITEM_COMPOSITE_BOW ) ;
      FireTuxRangedWeaponRaw ( 0 , ITEM_SHORT_BOW , GREEN_BULLET , TRUE , 0 , 3 , 1 , 0 , SpellHitPercentageTable [ Me [ 0 ] . spellcasting_skill ] ) ;

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
  // int SpellCost = ManaCostTable [ SPELL_TELEPORT_HOME ][ Me[ 0 ].SkillLevel [ 4 ] ] ;
  int SpellCost = ManaCostTable [ SPELL_TELEPORT_HOME ][ Me[ 0 ]. spellcasting_skill ] ;

  if ( Me [ 0 ] . mana >= SpellCost )
    {
      Me[0].mana -= SpellCost;

      if ( curShip.AllLevels[ PortalTarget.z ]->map [ (int) PortalTarget.y ] [ (int) PortalTarget.x ] == FLOOR )
	{
	  curShip.AllLevels[ PortalTarget.z ]->map [ (int) PortalTarget.y ] [ (int) PortalTarget.x ] = TELE_1 ;
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
  int SpellCost = ManaCostTable [ SPELL_TELEPORT_HOME ][ Me[ 0 ]. spellcasting_skill ] ;
  location HomeSpot;

  if ( Me [ 0 ] . mana >= SpellCost )
    {
      Me[0].mana -= SpellCost;

      Me[0].teleport_anchor.x = Me [ 0 ] . pos . x;
      Me[0].teleport_anchor.y = Me [ 0 ] . pos . y;
      Me[0].teleport_anchor.z = Me [ 0 ] . pos . z;

      Play_Spell_ForceToEnergy_Sound( );

      ResolveMapLabelOnShip ( "TeleportHomeTarget" , &(HomeSpot) );
      Teleport ( HomeSpot.level , HomeSpot.x , HomeSpot.y , 0 , FALSE ) ;

    }
  else
    {
      Me[0].TextVisibleTime = 0;
      Me[0].TextToBeDisplayed = "Not enough force left within me.";
      Not_Enough_Mana_Sound(  );
    }
}; // void CreateTeleportal ( gps PortalTarget )

/* ----------------------------------------------------------------------
 * This function handles the ForceExplosionCircle skill.
 * ---------------------------------------------------------------------- */
void
ForceExplosionCircle ( gps ExpCenter )
{
  // int SpellCost = ManaCostTable [ SPELL_FORCE_EXPLOSION_CIRCLE ][ Me[ 0 ].SkillLevel [ SPELL_FORCE_EXPLOSION_CIRCLE ] ] ;
  int SpellCost = ManaCostTable [ SPELL_FORCE_EXPLOSION_CIRCLE ][ Me[ 0 ]. spellcasting_skill ] ;

  if ( Me[0].mana >= SpellCost )
    {
      Me[0].mana -= SpellCost;
      StartBlast ( ExpCenter.x + 1   , ExpCenter.y       , ExpCenter.z , DRUIDBLAST );
      StartBlast ( ExpCenter.x - 1   , ExpCenter.y       , ExpCenter.z , DRUIDBLAST );
      StartBlast ( ExpCenter.x       , ExpCenter.y - 1   , ExpCenter.z , DRUIDBLAST );
      StartBlast ( ExpCenter.x       , ExpCenter.y + 1   , ExpCenter.z , DRUIDBLAST );

      StartBlast ( ExpCenter.x + 0.5 , ExpCenter.y + 0.5 , ExpCenter.z , DRUIDBLAST );
      StartBlast ( ExpCenter.x - 0.5 , ExpCenter.y + 0.5 , ExpCenter.z , DRUIDBLAST );
      StartBlast ( ExpCenter.x + 0.5 , ExpCenter.y - 0.5 , ExpCenter.z , DRUIDBLAST );
      StartBlast ( ExpCenter.x - 0.5 , ExpCenter.y - 0.5 , ExpCenter.z , DRUIDBLAST );
    }
  else
    {
      Me[0].TextVisibleTime = 0;
      Me[0].TextToBeDisplayed = "Not enough force left within me.";
      Not_Enough_Mana_Sound(  );
    }
}; // void ForceExplosionCircle ( finepoint ExpCenter )

/* ----------------------------------------------------------------------
 * This function handles the ForceExplosionCircle skill.
 * ---------------------------------------------------------------------- */
void
RadialEMPWave ( gps ExpCenter , int SpellCostsMana )
{
  int SpellCost = ManaCostTable [ SPELL_RADIAL_EMP_WAVE ][ Me[ 0 ]. spellcasting_skill ] ;
  int i;

  if ( ( Me[0].mana >= SpellCost ) || !SpellCostsMana )
    {
      //--------------------
      // For now, this spell is for free!! gratis!! yeah!! oh groovy!
      //
      if ( SpellCostsMana ) Me[0].mana -= SpellCost;
      //
      PlaySound ( SPELL_FORCETOENERGY_SOUND_0 ) ;

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
    }
  else
    {
      Me[0].TextVisibleTime = 0;
      Me[0].TextToBeDisplayed = "Not enough force left within me.";
      Not_Enough_Mana_Sound(  );
    }
}; // void RadialEMPWave ( finepoint ExpCenter )

/* ----------------------------------------------------------------------
 * This function handles the ForceExplosionCircle skill.
 * ---------------------------------------------------------------------- */
void
RadialVMXWave ( gps ExpCenter , int SpellCostsMana )
{
  int SpellCost = ManaCostTable [ SPELL_RADIAL_VMX_WAVE ][ Me[ 0 ]. spellcasting_skill ] ;
  int i;

  if ( ( Me[0].mana >= SpellCost ) || !SpellCostsMana ) 
    {
      //--------------------
      // For now, this spell is for free!! gratis!! yeah!! oh groovy!
      //
      if ( SpellCostsMana ) Me[0].mana -= SpellCost;
      //
      PlaySound ( SPELL_FORCETOENERGY_SOUND_0 ) ;

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
      
    }
  else
    {
      Me[0].TextVisibleTime = 0;
      Me[0].TextToBeDisplayed = "Not enough force left within me.";
      Not_Enough_Mana_Sound(  );
    }
}; // void RadialVMXWave ( finepoint ExpCenter )

/* ----------------------------------------------------------------------
 * This function handles the ForceExplosionCircle skill.
 * ---------------------------------------------------------------------- */
void
ForceExplosionRay ( gps ExpCenter , point TargetVector )
{
  int i ;
  moderately_finepoint step;
  int SpellCost = ManaCostTable [ SPELL_FORCE_EXPLOSION_RAY ][ Me[ 0 ]. spellcasting_skill ] ;

  if ( Me[0].mana >= SpellCost )
    {
      Me[0].mana -= SpellCost;
      step.x = ( TargetVector.x * 0.25 ) / Block_Width;
      step.y = ( TargetVector.y * 0.25 ) / Block_Height;

      for ( i = 1 ; i < 5 ; i ++ )
	{
	  StartBlast ( ExpCenter.x + i * step.x , ExpCenter.y + i * step.y , ExpCenter.z , DRUIDBLAST );
	}
    }
  else
    {
      Me[0].TextVisibleTime = 0;
      Me[0].TextToBeDisplayed = "Not enough force left within me.";
      Not_Enough_Mana_Sound(  );
    }

}; // void ForceExplosionCircle ( finepoint ExpCenter )

/* ----------------------------------------------------------------------
 * This function handles the ForceToEnergyConversion skill.
 * ---------------------------------------------------------------------- */
void
ForceToEnergyConversion ( void )
{
  // int SpellCost = ManaCostTable [ SPELL_FORCE_TO_ENERGY ][ Me[ 0 ].SkillLevel [ SPELL_FORCE_TO_ENERGY ] ] ;
  int SpellCost = ManaCostTable [ SPELL_FORCE_TO_ENERGY ][ Me[ 0 ]. spellcasting_skill ] ;

  if ( Me[0].mana >= SpellCost )
    {
      Me[0].mana   -= SpellCost;
      Me[0].energy += 10 ; // * SpellLevel
      if ( Me [ 0 ] . energy > Me [ 0 ] . maxenergy ) Me [ 0 ] . energy = Me [ 0 ] . maxenergy ;

      Play_Spell_ForceToEnergy_Sound( );

    }
  else
    {
      Me[0].TextVisibleTime = 0;
      Me[0].TextToBeDisplayed = "Not enough force left within me.";
      Not_Enough_Mana_Sound(  );
    }

}; // void ForceExplosionCircle ( finepoint ExpCenter )

/* ----------------------------------------------------------------------
 * This function handles the skills the player might have acitivated
 * or deactivated or whatever.
 * ---------------------------------------------------------------------- */
void
HandleCurrentlyActivatedSkill( void )
{
  static int RightPressedPreviousFrame = 0;
  moderately_finepoint TargetPoint;

  switch ( Me [ 0 ] . readied_skill )
    {
    case SPELL_TRANSFERMODE:
      if (MouseRightPressed() == 1)
	Me[0].status = TRANSFERMODE;
      break;
    case SPELL_FORCE_EXPLOSION_CIRCLE:
      if ( MouseRightPressed() && ( ! RightPressedPreviousFrame ) )
	{
	  if ( CursorIsInUserRect ( GetMousePos_x() + 16 , GetMousePos_y() + 16) )
	    ForceExplosionCircle( Me[0].pos );
	}
      break;
    case SPELL_FORCE_EXPLOSION_RAY:
      if ( MouseRightPressed() && ( ! RightPressedPreviousFrame ) )
	{
	  if ( CursorIsInUserRect ( GetMousePos_x() + 16 , GetMousePos_y() + 16) )
	    {
	      ForceExplosionRay ( Me[0].pos , input_axis );
	    }
	}
      break;
    case SPELL_FORCE_TO_ENERGY:
      if ( MouseRightPressed() && ( ! RightPressedPreviousFrame ) )
	{
	  if ( CursorIsInUserRect ( GetMousePos_x() + 16 , GetMousePos_y() + 16) )
	    {
	      ForceToEnergyConversion ( );
	    }
	}
      break;
    case SPELL_TELEPORT_HOME:
      if ( MouseRightPressed() && ( ! RightPressedPreviousFrame ) )
	{
	  if ( CursorIsInUserRect ( GetMousePos_x() + 16 , GetMousePos_y() + 16) )
	    {
	      // TeleportHome ( TargetLocation ) ;
	      TeleportHome (  ) ;
	    }
	}
      break;
    case SPELL_FIREY_BOLT:
      if ( MouseRightPressed() && ( ! RightPressedPreviousFrame ) )
	{
	  if ( CursorIsInUserRect ( GetMousePos_x() + 16 , GetMousePos_y() + 16) )
	    {
	      TargetPoint . x = Me [ 0 ] . pos . x + ( GetMousePos_x() + 16 ) / Block_Width ;
	      TargetPoint . y = Me [ 0 ] . pos . y + ( GetMousePos_y() + 16 ) / Block_Height ; 
	      FireyBoltSpell ( Me [ 0 ] . pos , TargetPoint );
	    }
	}
      break;
    case SPELL_COLD_BOLT:
      if ( MouseRightPressed() && ( ! RightPressedPreviousFrame ) )
	{
	  if ( CursorIsInUserRect ( GetMousePos_x() + 16 , GetMousePos_y() + 16) )
	    {
	      TargetPoint . x = Me [ 0 ] . pos . x + ( GetMousePos_x() + 16 ) / Block_Width ;
	      TargetPoint . y = Me [ 0 ] . pos . y + ( GetMousePos_y() + 16 ) / Block_Height ; 
	      ColdBoltSpell ( Me [ 0 ] . pos , TargetPoint );
	    }
	}
      break;
    case SPELL_REPAIR_SKILL:
      if ( MouseRightPressed() && ( ! RightPressedPreviousFrame ) )
	{
	  if ( !CursorIsInInvRect( GetMousePos_x() + 16 , GetMousePos_y() + 16 ) || !GameConfig.Inventory_Visible )
	    {
	      //--------------------
	      // Do nothing here.  The right mouse click while in inventory screen
	      // will be handled in the inventory screen management function.
	      //
	      PlayOnceNeededSoundSample ( "../effects/Tux_I_Can_Only_0.wav" , FALSE );      
	    }
	}
      break;
    case SPELL_POISON_BOLT:
      if ( MouseRightPressed() && ( ! RightPressedPreviousFrame ) )
	{
	  if ( CursorIsInUserRect ( GetMousePos_x() + 16 , GetMousePos_y() + 16) )
	    {
	      TargetPoint . x = Me [ 0 ] . pos . x + ( GetMousePos_x() + 16 ) / Block_Width ;
	      TargetPoint . y = Me [ 0 ] . pos . y + ( GetMousePos_y() + 16 ) / Block_Height ; 
	      PoisonBoltSpell ( Me [ 0 ] . pos , TargetPoint );
	    }
	}
      break;
    case SPELL_PARALYZE_BOLT:
      if ( MouseRightPressed() && ( ! RightPressedPreviousFrame ) )
	{
	  if ( CursorIsInUserRect ( GetMousePos_x() + 16 , GetMousePos_y() + 16) )
	    {
	      TargetPoint . x = Me [ 0 ] . pos . x + ( GetMousePos_x() + 16 ) / Block_Width ;
	      TargetPoint . y = Me [ 0 ] . pos . y + ( GetMousePos_y() + 16 ) / Block_Height ; 
	      ParalyzeBoltSpell ( Me [ 0 ] . pos , TargetPoint );
	    }
	}
      break;
    case SPELL_DETECT_ITEM:
      if ( MouseRightPressed() && ( ! RightPressedPreviousFrame ) )
	{
	  if ( CursorIsInUserRect ( GetMousePos_x() + 16 , GetMousePos_y() + 16) )
	    {
	      DetectItemsSpell (  ) ;
	    }
	}
      break;
    case SPELL_RADIAL_EMP_WAVE:
      if ( MouseRightPressed() && ( ! RightPressedPreviousFrame ) )
	{
	  if ( CursorIsInUserRect ( GetMousePos_x() + 16 , GetMousePos_y() + 16) )
	    {
	      RadialEMPWave ( Me [ 0 ] . pos , TRUE );
	    }
	}
      break;
    case SPELL_RADIAL_VMX_WAVE:
      if ( MouseRightPressed() && ( ! RightPressedPreviousFrame ) )
	{
	  if ( CursorIsInUserRect ( GetMousePos_x() + 16 , GetMousePos_y() + 16) )
	    {
	      RadialVMXWave ( Me [ 0 ] . pos , TRUE );
	    }
	}
      break;
    default:
      fprintf( stderr, "\n\nMe [ 0 ] . readied_skill: %d \n" , Me [ 0 ] . readied_skill );
      GiveStandardErrorMessage ( "HandleCurrentlyActivatedSkill(...)" , "\
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
CursorIsOnWhichSpellLevelButton( int x , int y )
{
  int i;

  //--------------------
  // First we check if the cursor is in at least horizontally
  // and vertically in the line with the spell level buttons.
  //
  if ( x > SkillScreenRect.x + SPELL_LEVEL_BUTTONS_X + 300 ) return ( -1 );
  if ( x < SkillScreenRect.x + SPELL_LEVEL_BUTTONS_X       ) return ( -1 );
  if ( y > SkillScreenRect.y + SPELL_LEVEL_BUTTONS_Y + 32  ) return ( -1 );
  if ( y < SkillScreenRect.y + SPELL_LEVEL_BUTTONS_Y       ) return ( -1 );

#define SPELL_LEVEL_BUTTONS_X 10
#define SPELL_LEVEL_BUTTONS_Y 413
#define SPELL_LEVEL_BUTTON_WIDTH 30

  //--------------------
  // Now we can check on which skill rectangle exactly the cursor
  // is hovering, since we know that it is hitting, horizontally
  // at least, the row of skill icons.
  //
  for ( i = 0 ; i < 10 ; i ++ )
    {
      if ( x < SkillScreenRect.x + SPELL_LEVEL_BUTTONS_X + ( i + 1 ) * SPELL_LEVEL_BUTTON_WIDTH ) 
	return i;
    }

  return ( -1 );
}; // int CursorIsOnWhichSpellLevelButton( int x , int y )


/* ----------------------------------------------------------------------
 * This function displays the SKILLS SCREEN.  This is NOT the same as the
 * CHARACTER SCREEN.  In the skills screen you can see what skills/spells
 * you currenlty have availabe and you can select a new readied skill by
 * clicking on it with the mouse.
 * ---------------------------------------------------------------------- */
void 
ShowSkillsScreen ( void )
{
  static SDL_Rect ButtonRect;
  static SDL_Surface *SkillScreenImage = NULL;
  SDL_Surface *tmp;
  char *fpath;
  char CharText[1000];
  static int MouseButtonPressedPreviousFrame = FALSE;
  point CurPos;
  int i;
  SDL_Rect SpellLevelRect;

#define INTER_SKILLRECT_DIST 17
#define FIRST_SKILLRECT_Y 16

  DebugPrintf (2, "\nvoid ShowInventoryMessages( ... ): Function call confirmed.");

  //--------------------
  // If the log is not set to visible right now, we do not need to 
  // do anything more, but to restore the usual user rectangle size
  // back to normal and to return...
  //
  if ( GameConfig.SkillScreen_Visible == FALSE ) return;

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
  if ( SkillScreenImage == NULL )
    {
      // SDL_FillRect( Screen, & InventoryRect , 0x0FFFFFF );
      fpath = find_file ( SKILL_SCREEN_BACKGROUND_FILE , GRAPHICS_DIR, FALSE);
      tmp = IMG_Load( fpath );
      SkillScreenImage = SDL_DisplayFormat ( tmp );
      SDL_FreeSurface ( tmp );

      //--------------------
      // We define the right side of the user screen as the rectangle
      // for our inventory screen.
      //
      SkillScreenRect.x = CHARACTERRECT_X;
      SkillScreenRect.y = 0; 
      SkillScreenRect.w = CHARACTERRECT_W;
      SkillScreenRect.h = CHARACTERRECT_H;
    }
  
  //--------------------
  // At this point we know, that the skill screen is desired and must be
  // displayed in-game:
  //
  SDL_SetClipRect( Screen, NULL );
  SDL_BlitSurface ( SkillScreenImage , NULL , Screen , &SkillScreenRect );

  //--------------------
  // According to the page in the spell book currently opened,
  // we draw a 'button' or activation mark over the appropriate spot
  //
  SpellLevelRect.x = SkillScreenRect.x + SPELL_LEVEL_BUTTONS_X + 
    SPELL_LEVEL_BUTTON_WIDTH * GameConfig.spell_level_visible ;
  SpellLevelRect.y = SkillScreenRect.y + SPELL_LEVEL_BUTTONS_Y ;
  SDL_BlitSurface ( SpellLevelButtonImageList[ GameConfig.spell_level_visible ] , NULL , Screen , &SpellLevelRect );

  //--------------------
  // Now we fill in the skills available to this bot.  ( For now, these skills 
  // are not class-specific, like in diablo or something, but this is our first
  // approach to the topic after all.... :)
  //
  for ( i = 0 ; i < NUMBER_OF_SKILLS_PER_SKILL_LEVEL ; i ++ )
    {
      ButtonRect.x = SkillScreenRect.x + 15 ;
      ButtonRect.y = SkillScreenRect.y + FIRST_SKILLRECT_Y + i * ( 64 + INTER_SKILLRECT_DIST );
      ButtonRect.w = 64;
      ButtonRect.h = 64;

      SDL_BlitSurface ( SkillIconSurfacePointer[ i + NUMBER_OF_SKILLS_PER_SKILL_LEVEL * GameConfig.spell_level_visible ] , NULL , Screen , &ButtonRect );

      //--------------------
      // First we write the name of the skill to the screen
      //
      DisplayText( SkillName[ i + NUMBER_OF_SKILLS_PER_SKILL_LEVEL * GameConfig.spell_level_visible ] , 
		   16 + 64 + 16 + SkillScreenRect.x , 
		   FIRST_SKILLRECT_Y + i * (64 + INTER_SKILLRECT_DIST) + SkillScreenRect.y , &SkillScreenRect );
      
      //--------------------
      // Now we write the competence of the players character in that skill to the screen
      //
      sprintf( CharText , "Skill Level: %d " , Me[0].SkillLevel[ i + NUMBER_OF_SKILLS_PER_SKILL_LEVEL * GameConfig.spell_level_visible ] );
      DisplayText( CharText , 16 + 64 + 16 + SkillScreenRect.x , 
		   FIRST_SKILLRECT_Y + i * (64 + INTER_SKILLRECT_DIST) + SkillScreenRect.y + FontHeight( GetCurrentFont() ) , &SkillScreenRect );
      // sprintf( CharText , "Mana cost: %d " , ManaCostTable[ i + NUMBER_OF_SKILLS_PER_SKILL_LEVEL * GameConfig.spell_level_visible ][Me[0]. SkillLevel[ i + NUMBER_OF_SKILLS_PER_SKILL_LEVEL * GameConfig.spell_level_visible ]] );
      sprintf( CharText , "Mana cost: %d " , ManaCostTable[ i + NUMBER_OF_SKILLS_PER_SKILL_LEVEL * GameConfig.spell_level_visible ][Me[0]. spellcasting_skill ] );
      DisplayText( CharText , 16 + 64 + 16 + SkillScreenRect.x , 
		   FIRST_SKILLRECT_Y + i * (64 + INTER_SKILLRECT_DIST) + SkillScreenRect.y + 2 * FontHeight( GetCurrentFont() ) , &SkillScreenRect );
      
    }

  //--------------------
  // Now we see if perhaps the player has just clicked on one of the skills
  // available to this class.  In this case of course we must set a different
  // skill/spell as the currently activated skill/spell.
  //
  if ( ( CursorIsOnWhichSkillButton ( CurPos.x , CurPos.y ) != ( -1 ) ) &&
       axis_is_active &&
       ! MouseButtonPressedPreviousFrame )
    Me[0].readied_skill = CursorIsOnWhichSkillButton ( CurPos.x , CurPos.y ) + 
      NUMBER_OF_SKILLS_PER_SKILL_LEVEL * GameConfig.spell_level_visible ;

  //--------------------
  // Now we see if perhaps the player has just clicked on another skill level
  // button.  In this case of course we must set a different skill/spell level
  // as the currently visible spell level.
  //
  if ( ( CursorIsOnWhichSpellLevelButton ( CurPos.x , CurPos.y ) != ( -1 ) ) &&
       axis_is_active &&
       ! MouseButtonPressedPreviousFrame )
    {
      GameConfig.spell_level_visible = CursorIsOnWhichSpellLevelButton ( CurPos.x , CurPos.y );
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

