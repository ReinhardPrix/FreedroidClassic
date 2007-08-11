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

#define EXPERIENCE_Y 150
#define NEXT_LEVEL_Y 220

#define GOLD_Y 132

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

#define SPELL_LEVEL_BUTTONS_X 60
#define SPELL_LEVEL_BUTTONS_Y 423
#define SPELL_LEVEL_BUTTON_WIDTH 30

int Override_Power_Limit=0;

SDL_Rect SkillScreenRect;

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


/* ------------------
 * This function calculates the heat cost of running a given program (source or blob), based on current program level and casting ability
 * -----------------*/
int calculate_program_heat_cost ( int program_id )
{
    float cost_ratio [ NUMBER_OF_SKILL_LEVELS ] = { 1.5, 1.3, 1.2, 1.0, 0.9, 0.8, 0.7, 0.6, 0.5, 0.4 };
    return cost_ratio [ Me [ 0 ] . spellcasting_skill ]* ( SpellSkillMap[ program_id ] . heat_cost  + SpellSkillMap[ program_id ] . heat_cost_per_level * (Me [ 0 ] . SkillLevel [ program_id ] - 1 ));
};

/* ------------------
 * This function calculates the damage dealt by a hit of a given program
 * -----------------*/
int calculate_program_hit_damage ( int program_id )
{
    return ( SpellSkillMap[ program_id ] . damage_base  + SpellSkillMap[ program_id ] . damage_per_level * ( Me [ 0 ] . SkillLevel [ program_id ] - 1 ) + MyRandom(SpellSkillMap[ program_id ] . damage_mod));
}

/* ------------------
 * This function looks for a given program name in the program spec array
 * -------------------*/
int get_program_index_with_name ( const char * pname )
{
    int i = 0;
    while ( i < number_of_skills ) 
	{
	if ( ! strcmp(SpellSkillMap[i] . name, pname ) )
		return i;
	i++;
	}	

	fprintf(stderr, "%s\n", pname);

	 GiveStandardErrorMessage ( __FUNCTION__  , "\
FreedroidRPG could not find the program name above in the program spec array!",
                                 PLEASE_INFORM, IS_FATAL );
     return -1;
}

/* ----------------------------------------------------------------------
 * This function creates a teleporter portal to the home location.
 * ---------------------------------------------------------------------- */
void
TeleportHome ( void )
{
location HomeSpot;

     if( (! Me [ 0 ] . teleport_anchor . x) && (! Me [ 0 ] . teleport_anchor . y)) //if there is no anchor, teleport home
        {
                Me [ 0 ] . teleport_anchor . x = Me [ 0 ] . pos . x;
                Me [ 0 ] . teleport_anchor . y = Me [ 0 ] . pos . y;
                Me [ 0 ] . teleport_anchor . z = Me [ 0 ] . pos . z;
                teleport_arrival_sound ( );
                ResolveMapLabelOnShip ( "TeleportHomeTarget" , &(HomeSpot) );
                Teleport ( HomeSpot.level , HomeSpot.x , HomeSpot.y , 0 , TRUE ) ;
        }
        else //we must teleport back to the anchor
        {
                teleport_arrival_sound  ( );
                Teleport ( Me [ 0 ] . teleport_anchor . z , Me [ 0 ] . teleport_anchor . x , Me [ 0 ] . teleport_anchor . y , 0 , TRUE ) ;
        }

}; // void TeleportHome ( void )

/* ----------------------------------------------------------------------
 * This function handles the skills the player might have acitivated
 * or deactivated or whatever.
 * ---------------------------------------------------------------------- */
void
HandleCurrentlyActivatedSkill()
{
    if ( ! MouseRightClicked() ) 
	return;
    
    int Grabbed_InvPos = GetInventoryItemAt ( GetInventorySquare_x ( GetMousePos_x() ) , GetInventorySquare_y ( GetMousePos_y() ) );
    if ( Grabbed_InvPos != -1 )
	if ( ItemMap [ Me[0].Inventory[ Grabbed_InvPos ] . type ] . item_can_be_applied_in_combat ) 
	return; //if the cursor is over an item that can be applied, then the player wants to apply it not trigger a spell
    
    if ( Me [ 0 ] . SkillLevel [ Me [ 0 ] . readied_skill ] <= 0 ) return;

    /* We calculate the spellcost and check the power limit override - the temperature is raised further down, when the actual effect
	gets triggered */
    int SpellCost = calculate_program_heat_cost ( Me [ 0 ] . readied_skill );

    if ( Me [ 0 ] . temperature > Me [ 0 ] . max_temperature - SpellCost && !Override_Power_Limit )
	{
	      //Not_Enough_Mana_Sound(  );
	      Override_Power_Limit = 1;
	      return;
	}
    Override_Power_Limit = 0;

    switch ( SpellSkillMap [ Me [ 0 ] . readied_skill ] . form ) 
	{
	case PROGRAM_FORM_IMMEDIATE:
	case PROGRAM_FORM_BULLET:
	case PROGRAM_FORM_RADIAL:
		if (! MouseCursorIsInUserRect ( GetMousePos_x() , GetMousePos_y() ) ) return;; break;
	}

    DoSkill(Me [ 0 ] . readied_skill, SpellCost);

return;    
}; // void HandleCurrentlyActivatedSkill( void )

int 
DoSkill(int skill_index, int SpellCost)
{
    int index_of_droid_below_mouse_cursor = ( -1 ) ;
    int player_num = 0;

    /*we handle the form of the program now*/
    switch ( SpellSkillMap [ skill_index ] . form ) 
	{
	case PROGRAM_FORM_IMMEDIATE:
		index_of_droid_below_mouse_cursor = GetLivingDroidBelowMouseCursor ( 0 ) ;
		if ( index_of_droid_below_mouse_cursor == ( -1 ) ) 
			goto done_handling_instant_hits;
		if ( ! DirectLineWalkable ( Me [ player_num ] . pos . x , Me [ player_num ] . pos . y ,  translate_pixel_to_map_location ( 0 ,
                                                       (float) ServerThinksInputAxisX ( 0 ) ,
                                                       (float) ServerThinksInputAxisY ( 0 ) , TRUE ), 
							translate_pixel_to_map_location ( 0 ,
                                                       (float) ServerThinksInputAxisX ( 0 ) ,
                                                       (float) ServerThinksInputAxisY ( 0 ) , FALSE ), Me [ 0 ] . pos . z))
						goto done_handling_instant_hits;

		AllEnemys [ index_of_droid_below_mouse_cursor ] . energy -= calculate_program_hit_damage ( skill_index ) ;
	        Me [ 0 ] . temperature += SpellCost;
		break;
	
	case PROGRAM_FORM_SELF:
		Me [ 0 ] . energy -= calculate_program_hit_damage ( skill_index ) ;
                Me [ 0 ] . temperature += SpellCost;
		Me [ 0 ] . slowdown_duration += strcmp( SpellSkillMap [ skill_index ] . effect, "slowdown" ) ? 0 : 10;
                Me [ 0 ] . paralyze_duration += strcmp ( SpellSkillMap [ skill_index ] . effect, "paralyze" ) ? 0 : 15;
		break;

	case PROGRAM_FORM_BULLET:
                Me [ 0 ] . temperature += SpellCost;
	
		moderately_finepoint target_location;
	        target_location . x = translate_pixel_to_map_location ( 0 , ServerThinksInputAxisX ( 0 ) , ServerThinksInputAxisY ( 0 ) , TRUE ) ;
		target_location . y = translate_pixel_to_map_location ( 0 , ServerThinksInputAxisX ( 0 ) , ServerThinksInputAxisY ( 0 ) , FALSE ) ;
		
		bullet bul_parms;
		if (  SpellSkillMap [ skill_index ] . graphics_code != -1 ) 
			FillInDefaultBulletStruct( 0, &bul_parms,  SpellSkillMap [ skill_index ] . graphics_code, ITEM_LASER_PISTOL );
		else	FillInDefaultBulletStruct( 0, &bul_parms, MAGENTA_BULLET, ITEM_LASER_PISTOL );
	
		bul_parms.freezing_level = strcmp( SpellSkillMap [ skill_index ] . effect, "slowdown" ) ? 0 : 10;
		bul_parms.poison_duration = strcmp ( SpellSkillMap [ skill_index ] . effect, "poison" ) ? 0 : 10;
		bul_parms.poison_damage_per_sec = strcmp ( SpellSkillMap [ skill_index ] . effect, "poison" ) ? 0 : 1;
		bul_parms.paralysation_duration = strcmp ( SpellSkillMap [ skill_index ] . effect, "paralyze" ) ? 0 : 10;
		bul_parms.damage = calculate_program_hit_damage ( skill_index ) ;
		bul_parms.to_hit = SpellHitPercentageTable [ Me [ 0 ] . spellcasting_skill ];

		FireTuxRangedWeaponRaw ( 0 , ITEM_LASER_PISTOL , -1 , &bul_parms, target_location); 
		
		return 1; //no extra effects

	case PROGRAM_FORM_RADIAL:
		Me [ 0 ] . temperature += SpellCost;
		
		int i,j;
		for ( i = 0 ; i < MAX_ACTIVE_SPELLS ; i ++ )
	            {
	            if ( AllActiveSpells [ i ] . img_type == (-1) ) break;
                    }
	        if ( i >= MAX_ACTIVE_SPELLS ) i = 0 ;

		AllActiveSpells [ i ] . img_type = (SpellSkillMap [ skill_index ] . graphics_code == -1 ? 2 : SpellSkillMap [ skill_index ] . graphics_code);
		AllActiveSpells [ i ] . spell_center . x = Me [ 0 ] . pos . x;
	        AllActiveSpells [ i ] . spell_center . y = Me [ 0 ] . pos . y;
	        AllActiveSpells [ i ] . spell_radius = 0.3 ;
	        AllActiveSpells [ i ] . spell_age = 0 ;
	        AllActiveSpells [ i ] . mine = 1;
 
	        for ( j = 0 ; j < RADIAL_SPELL_DIRECTIONS ; j ++ )
	            { 
  	            AllActiveSpells [ i ] . active_directions [ j ] = TRUE ;
                    }
		
		AllActiveSpells [ i ] . freeze_duration = strcmp( SpellSkillMap [ skill_index ] . effect, "slowdown" ) ? 0 : 10;
		AllActiveSpells [ i ] .poison_duration = strcmp ( SpellSkillMap [ skill_index ] . effect, "poison" ) ? 0 : 10;
		AllActiveSpells [ i ] .poison_dmg = strcmp ( SpellSkillMap [ skill_index ] . effect, "poison" ) ? 0 : 1;
		AllActiveSpells [ i ] .paralyze_duration = strcmp ( SpellSkillMap [ skill_index ] . effect, "paralyze" ) ? 0 : 10;
		AllActiveSpells [ i ] .damage = calculate_program_hit_damage ( skill_index ) ;

		return 1;

	}


    done_handling_instant_hits:

    /*handle the special extra effects of the skill*/
    if ( ! strcmp ( SpellSkillMap [ skill_index ] . effect, "none" ) )
   	{
	goto out;
	}


    if ( ! strcmp ( SpellSkillMap [ skill_index ] . effect, "talk_or_takeover" ) )
   	{
	if (! MouseCursorIsInUserRect ( GetMousePos_x() , GetMousePos_y() ) ) goto out;
	index_of_droid_below_mouse_cursor = GetLivingDroidBelowMouseCursor ( player_num ) ;
	if ( index_of_droid_below_mouse_cursor == ( -1 ) ) goto out;
	if ( ! DirectLineWalkable ( Me [ player_num ] . pos . x , Me [ player_num ] . pos . y ,  translate_pixel_to_map_location ( player_num ,
                                                       (float) ServerThinksInputAxisX ( player_num ) ,
                                                       (float) ServerThinksInputAxisY ( player_num ) , TRUE ), 
							translate_pixel_to_map_location ( player_num ,
                                                       (float) ServerThinksInputAxisX ( player_num ) ,
                                                       (float) ServerThinksInputAxisY ( player_num ) , FALSE ), Me [ player_num ] . pos . z))
		goto out;


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
	goto out;
	}

    if ( ! strcmp ( SpellSkillMap [ skill_index ] . effect, "weapon" ) )
   	{
	if (! MouseCursorIsInUserRect ( GetMousePos_x() , GetMousePos_y() ) ) goto out;
        if ( MouseCursorIsInUserRect ( GetMousePos_x()  , 
				      GetMousePos_y()  ) )
		tux_wants_to_attack_now ( 0 , TRUE );
	goto out;
	}

    if ( ! strcmp ( SpellSkillMap [ skill_index ] . effect, "repair" ) )
   	{
	if ( ! MouseCursorIsInInvRect( GetMousePos_x()  , 
					 GetMousePos_y()  ) 
		     || ( !GameConfig.Inventory_Visible ) )
		{
		    //--------------------
		    // Do nothing here.  The right mouse click while in inventory screen
		    // will be handled in the inventory screen management function.
		    //
		    PlayOnceNeededSoundSample ( "effects/tux_ingame_comments/CantRepairThat.ogg" , 
						FALSE , FALSE );
		}
	goto out;
	}

    if ( ! strcmp ( SpellSkillMap [ skill_index ] . effect, "identify" ) )
   	{
	//--------------------
	// Maybe the identify mode has already been triggered and
	// is activated right now.  Then of course this (second) mouse
	// click must be ignored completely.
	//
	if ( global_ingame_mode == GLOBAL_INGAME_MODE_IDENTIFY )
		Me [ 0 ] . temperature -= SpellCost;

        silently_unhold_all_items ( );
        global_ingame_mode = GLOBAL_INGAME_MODE_IDENTIFY ;

	goto out;
	}

    if ( ! strcmp ( SpellSkillMap [ skill_index ] . effect, "invisibility" ) )
   	{
	goto out;
	}

    if ( ! strcmp ( SpellSkillMap [ skill_index ] . effect, "burnup" ) )
   	{
	goto out;
	}

    if ( ! strcmp ( SpellSkillMap [ skill_index ] . effect, "teleport_home" ) )
   	{
               TeleportHome (  ) ;
	goto out;
	}



/*    if ( ! strcmp ( SpellSkillMap [ skill_index ] . effect, "" ) )
   	{
	}*/

out:

return 1;    
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
    
     if ( y < SkillScreenRect.y + 16 + 5 * 64 + 5 * 16 ) return ( -1 );
     if ( y < SkillScreenRect.y + 16 + 6 * 64 + 5 * 16 ) return (  5 );
    
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
	blit_open_gl_texture_to_screen_position ( &SpellSkillMap [ Me [ 0 ] . readied_skill ] . icon_surface , 
						  TargetSkillRect . x , TargetSkillRect . y , TRUE );
    }
    else
    {
	our_SDL_blit_surface_wrapper ( SpellSkillMap [ Me [ 0 ] . readied_skill ] . icon_surface . surface , 
				       NULL , Screen , &TargetSkillRect );
    }
    
    
    TargetSkillRect.w = 320 - 15 - 15;
    TargetSkillRect.h = 480 - 15 ;
    DisplayText( SpellSkillMap [ Me [ 0 ] . readied_skill ] . description , 16 , 16+64+16, 
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
  for ( i = 0 ; i < number_of_skills ; i ++ )
    {
      SkillSubsetMap [ i ] = (-1) ;
    }
  for ( i = 0 ; i < number_of_skills ; i ++ )
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
  int SkillSubsetMap [ number_of_skills ] ;
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
      for ( i = 0 ; i < number_of_skills ; i ++ )
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
    point CurPos;
    int i;
    SDL_Rect SpellLevelRect;
    int SkillSubsetMap [ number_of_skills ] ;
    int SkillOfThisSlot;
    point SkillRectLocations [ NUMBER_OF_SKILLS_PER_SKILL_PAGE ] ;
  
    DebugPrintf ( 2 , "\n%s(): Function call confirmed." , __FUNCTION__ );
    
    SkillScreenRect . x = CHARACTERRECT_X;
    SkillScreenRect . y = 0; 
    SkillScreenRect . w = CHARACTERRECT_W;
    SkillScreenRect . h = CHARACTERRECT_H;
    
    for ( i = 0 ; i < NUMBER_OF_SKILLS_PER_SKILL_PAGE ; i ++ )
    {
	SkillRectLocations [ i ] . x = SkillScreenRect . x + 20 ;
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
    establish_skill_subset_map ( SkillSubsetMap );
    
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
	
	if (  i + NUMBER_OF_SKILLS_PER_SKILL_PAGE * GameConfig.spell_level_visible >= number_of_skills ) break;
	SkillOfThisSlot = SkillSubsetMap [ i + NUMBER_OF_SKILLS_PER_SKILL_PAGE * GameConfig.spell_level_visible ] ;
	if ( SkillOfThisSlot < 0 ) continue;
	
	LoadOneSkillSurfaceIfNotYetLoaded ( SkillOfThisSlot );
	
	if ( use_open_gl )
	{
	    blit_open_gl_texture_to_screen_position ( 
		&SpellSkillMap [ SkillOfThisSlot ] . icon_surface , 
		ButtonRect . x , ButtonRect . y , TRUE );
	}
	else
	{
	    our_SDL_blit_surface_wrapper ( 
		SpellSkillMap [ SkillOfThisSlot ] . icon_surface . surface , 
		NULL , Screen , &ButtonRect );
	}
	
	//--------------------
	// First we write the name of the skill to the screen
	//
    //SetCurrentFont ( Menu_BFont );
    SetCurrentFont ( FPS_Display_BFont );
    
	DisplayText( SpellSkillMap [ SkillOfThisSlot ] . name , 
		     16 + 64 + 16 + SkillScreenRect.x , 
		     FIRST_SKILLRECT_Y + i * (64 + INTER_SKILLRECT_DIST) + SkillScreenRect.y , 
		     &SkillScreenRect , TEXT_STRETCH );
      
    SetCurrentFont ( Message_BFont );
	//--------------------
	// Now we write the competence of the players character in that skill to the screen
	//
	sprintf( CharText , _("Program revision: %d "), Me[0].SkillLevel[ SkillOfThisSlot ] );
	DisplayText( CharText , 16 + 64 + 16 + SkillScreenRect.x , 
		     FIRST_SKILLRECT_Y + i * ( 64 + INTER_SKILLRECT_DIST ) + SkillScreenRect.y + 2 * FontHeight( GetCurrentFont() ) , &SkillScreenRect , TEXT_STRETCH );
	sprintf( CharText , _("Heat produced: %d "),   calculate_program_heat_cost ( SkillOfThisSlot )  );
	DisplayText( CharText , 16 + 64 + 16 + SkillScreenRect.x , 
		     FIRST_SKILLRECT_Y + i * (64 + INTER_SKILLRECT_DIST) + SkillScreenRect.y + 3 * FontHeight( GetCurrentFont() ) , &SkillScreenRect , TEXT_STRETCH );
	
    }

    //--------------------
    // Now we see if perhaps the player has just clicked on one of the skills
    // available to this class.  In this case of course we must set a different
    // skill/spell as the currently activated skill/spell.
    //
    if ( ( CursorIsOnWhichSkillButton ( CurPos.x , CurPos.y ) != ( -1 ) ) &&
	 MouseLeftClicked() )
    {
	if ( CursorIsOnWhichSkillButton ( CurPos.x , CurPos.y ) +
                              NUMBER_OF_SKILLS_PER_SKILL_PAGE * GameConfig.spell_level_visible  < number_of_skills )
		if ( SkillSubsetMap [ CursorIsOnWhichSkillButton ( CurPos.x , CurPos.y ) + 
			      NUMBER_OF_SKILLS_PER_SKILL_PAGE * GameConfig.spell_level_visible ] >= 0 ) 
		    Me[0].readied_skill = SkillSubsetMap [ CursorIsOnWhichSkillButton ( CurPos.x , CurPos.y ) + 
						   NUMBER_OF_SKILLS_PER_SKILL_PAGE * GameConfig.spell_level_visible ] ;
    }
    
    if ( MouseCursorIsOnButton ( OPEN_CLOSE_SKILL_EXPLANATION_BUTTON , CurPos.x , CurPos.y ) &&
	 MouseLeftClicked() )
    {
	toggle_game_config_screen_visibility(GAME_CONFIG_SCREEN_VISIBLE_SKILL_EXPLANATION);
    }
    
    //--------------------
    // Now we see if perhaps the player has just clicked on another skill level
    // button.  In this case of course we must set a different skill/spell level
    // as the currently visible spell level.
    //
    if ( ( CursorIsOnWhichSpellPageButton ( CurPos.x , CurPos.y ) != ( -1 ) ) &&
	 MouseLeftClicked() )
    {
	GameConfig.spell_level_visible = CursorIsOnWhichSpellPageButton ( CurPos.x , CurPos.y );
    }
    
    //--------------------
    // We want to know, if the button was pressed the previous frame when we
    // are in the next frame and back in this function.  Therefore we store
    // the current button situation, so that we can conclude on button just
    // pressed later.
    //

}; // ShowSkillsScreen ( void )

#undef _skills_c

