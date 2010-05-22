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

SDL_Rect SkillScreenRect;

/* ----------------------------------------------------------------------
 * This function creates a teleporter portal to the home location.
 * ---------------------------------------------------------------------- */
void
CreateTeleportal ( gps PortalTarget )
{
  int SpellCost = ManaCostTable [ 4 ][ Me[ 0 ].SkillLevel [ 4 ] ] ;

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
  int SpellCost = ManaCostTable [ 4 ][ Me[ 0 ].SkillLevel [ 4 ] ] ;

  if ( Me [ 0 ] . mana >= SpellCost )
    {
      Me[0].mana -= SpellCost;

      Me[0].teleport_anchor.x = Me [ 0 ] . pos . x;
      Me[0].teleport_anchor.y = Me [ 0 ] . pos . y;
      Me[0].teleport_anchor.z = Me [ 0 ] . pos . z;

      Play_Spell_ForceToEnergy_Sound( );

      Teleport ( 0 , 3 , 3 , 0 ) ;

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
  int SpellCost = ManaCostTable [ 1 ][ Me[ 0 ].SkillLevel [ 1 ] ] ;

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
ForceExplosionRay ( gps ExpCenter , point TargetVector )
{
  int i ;
  moderately_finepoint step;
  int SpellCost = ManaCostTable [ 2 ][ Me[ 0 ].SkillLevel [ 2 ] ] ;

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
  int SpellCost = ManaCostTable [ 3 ][ Me[ 0 ].SkillLevel [ 3 ] ] ;

  if ( Me[0].mana >= SpellCost )
    {
      Me[0].mana   -= SpellCost;
      Me[0].energy += 10 ; // * SpellLevel

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
  // gps TargetLocation;

  if ( Me[0].readied_skill == 0 )
    {
      if (MouseRightPressed() == 1)
	Me[0].status = TRANSFERMODE;
    }
  else if ( Me[0].readied_skill == 1 )
    {
      if ( MouseRightPressed() && ( ! RightPressedPreviousFrame ) )
	{
	  if ( CursorIsInUserRect ( GetMousePos_x() + 16 , GetMousePos_y() + 16) )
	    ForceExplosionCircle( Me[0].pos );
	}
    }
  else if ( Me[0].readied_skill == 2 )
    {
      if ( MouseRightPressed() && ( ! RightPressedPreviousFrame ) )
	{
	  if ( CursorIsInUserRect ( GetMousePos_x() + 16 , GetMousePos_y() + 16) )
	    {
	      ForceExplosionRay ( Me[0].pos , input_axis );
	    }
	}
    }
  else if ( Me[0].readied_skill == 3 )
    {
      if ( MouseRightPressed() && ( ! RightPressedPreviousFrame ) )
	{
	  if ( CursorIsInUserRect ( GetMousePos_x() + 16 , GetMousePos_y() + 16) )
	    {
	      ForceToEnergyConversion ( );
	    }
	}
    }
  else if ( Me[0].readied_skill == 4 )
    {
      if ( MouseRightPressed() && ( ! RightPressedPreviousFrame ) )
	{
	  if ( CursorIsInUserRect ( GetMousePos_x() + 16 , GetMousePos_y() + 16) )
	    {
	      // TeleportHome ( TargetLocation ) ;
	      TeleportHome (  ) ;
	    }
	}
    }


  RightPressedPreviousFrame = MouseRightPressed() ;

}; // void HandleCurrentlyActivatedSkill( void )


/* ----------------------------------------------------------------------
 * This function checks if a given screen position lies within the
 * strength plus button or not
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
 * This function displays the skills screen.
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
  char* SkillName[ NUMBER_OF_SKILLS ];

  SkillName[ 0 ] = "Takeover/Talk Skill";
  SkillName[ 1 ] = "Force Explosion Circle";
  SkillName[ 2 ] = "Force Explosion Ray";
  SkillName[ 3 ] = "Force -> Energy ";
  SkillName[ 4 ] = "Create Teleportal Home";
  SkillName[ 5 ] = "Unknown yet";

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
      fpath = find_file ( "SkillScreen.png" , GRAPHICS_DIR, FALSE);
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
  // Now we fill in the skills available to this bot.  ( For now, these skills
  // are not class-specific, like in diablo or something, but this is our first
  // approach to the topic after all.... :)
  //
  for ( i = 0 ; i < 5 ; i ++ )
    {
      ButtonRect.x = SkillScreenRect.x + 16;
      ButtonRect.y = SkillScreenRect.y + 16 + i * ( 64 + 16 );
      ButtonRect.w = 64;
      ButtonRect.h = 64;

      SDL_BlitSurface ( SkillIconSurfacePointer[ i ] , NULL , Screen , &ButtonRect );

      //--------------------
      // First we write the name of the skill to the screen
      //
      DisplayText( SkillName[i] , 16 + 64 + 16 + SkillScreenRect.x ,
		   16 + 20 + i * (64 + 16) + SkillScreenRect.y , &SkillScreenRect );

      //--------------------
      // Now we write the competence of the players character in that skill to the screen
      //
      sprintf( CharText , "Skill Level: %d " , Me[0].SkillLevel[i] );
      DisplayText( CharText , 16 + 64 + 16 + SkillScreenRect.x ,
		   16 + 20 + i * (64 + 16) + SkillScreenRect.y + FontHeight( GetCurrentFont() ) , &SkillScreenRect );
      sprintf( CharText , "Mana cost: %d " , ManaCostTable[i][Me[0].SkillLevel[i]] );
      DisplayText( CharText , 16 + 64 + 16 + SkillScreenRect.x ,
		   16 + 20 + i * (64 + 16) + SkillScreenRect.y + 2 * FontHeight( GetCurrentFont() ) , &SkillScreenRect );

    }

  //--------------------
  // It might be the case, that the character has some points to distribute upon the character
  // stats.  Then of course, we must display the plus button instead of all character 'now' values
  //
  if ( ( CursorIsOnWhichSkillButton ( CurPos.x , CurPos.y ) != ( -1 ) ) &&
       axis_is_active &&
       ! MouseButtonPressedPreviousFrame )
    Me[0].readied_skill = CursorIsOnWhichSkillButton ( CurPos.x , CurPos.y );


  //--------------------
  // We want to know, if the button was pressed the previous frame when we
  // are in the next frame and back in this function.  Therefore we store
  // the current button situation, so that we can conclude on button just
  // pressed later.
  //
  MouseButtonPressedPreviousFrame = axis_is_active;

}; // ShowSkillsScreen ( void )

#undef _skills_c

