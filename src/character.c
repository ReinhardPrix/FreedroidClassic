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

#define CHARACTERRECT_X (SCREENLEN/2)
// #define CHARACTERRECT_Y (User_Rect.y)
#define CHARACTERRECT_W (SCREENLEN/2)
#define CHARACTERRECT_H (User_Rect.h)

#define ENERGY_GAIN_PER_VIT_POINT 2
#define MANA_GAIN_PER_MAGIC_POINT 2
// #define DAMAGE_GAIN_PER_STR_POINT 2
#define AC_GAIN_PER_DEX_POINT 1
#define RECHARGE_SPEED_PERCENT_PER_DEX_POINT 0
#define TOHIT_PERCENT_PER_DEX_POINT (0.5)

// #define INV_BUTTON_X 20
#define INV_BUTTON_X 600
#define INV_BUTTON_Y 400
// #define INV_BUTTON_Y 10
// #define CHA_BUTTON_X 20
#define CHA_BUTTON_X 600
#define CHA_BUTTON_Y 430
// #define CHA_BUTTON_Y 30
#define INV_BUTTON_WIDTH 38
#define INV_BUTTON_HEIGHT 22

SDL_Rect CharacterRect;


/* ----------------------------------------------------------------------
 * This function checks if a given screen position lies within the 
 * strength plus button or not
 * ---------------------------------------------------------------------- */
int
CursorIsOnINVButton( int x , int y )
{
  if ( x > INV_BUTTON_X + INV_BUTTON_WIDTH  ) return ( FALSE );
  if ( x < INV_BUTTON_X                     ) return ( FALSE );
  if ( y > INV_BUTTON_Y + INV_BUTTON_HEIGHT ) return ( FALSE );
  if ( y < INV_BUTTON_Y                     ) return ( FALSE );
  return ( TRUE );
}; // int CursorIsOnStrButton( int x , int y )

/* ----------------------------------------------------------------------
 * This function checks if a given screen position lies within the 
 * strength plus button or not
 * ---------------------------------------------------------------------- */
int
CursorIsOnCHAButton( int x , int y )
{
  if ( x > CHA_BUTTON_X + INV_BUTTON_WIDTH  ) return ( FALSE );
  if ( x < CHA_BUTTON_X                     ) return ( FALSE );
  if ( y > CHA_BUTTON_Y + INV_BUTTON_HEIGHT ) return ( FALSE );
  if ( y < CHA_BUTTON_Y                     ) return ( FALSE );
  return ( TRUE );
}; // int CursorIsOnStrButton( int x , int y )


/* ----------------------------------------------------------------------
 * This function displays all the buttons that open up the character
 * screen and the invenotry screen
 * ---------------------------------------------------------------------- */
void
DisplayButtons( void )
{
  static SDL_Surface *CHA_ButtonImage = NULL;
  static SDL_Surface *INV_ButtonImage = NULL;
  static SDL_Surface *PlusButtonImage = NULL;
  static SDL_Rect CHA_Button_Rect;
  static SDL_Rect INV_Button_Rect;
  static int WasPressed;
  char* fpath;

  if ( GameConfig.CharacterScreen_Visible == TRUE ) return;
  
  // --------------------
  // Some things like the loading of the character screen
  // need to be done only once at the first call of this
  // function. 
  //
  if ( INV_ButtonImage == NULL )
    {
      // SDL_FillRect( Screen, & InventoryRect , 0x0FFFFFF );
      fpath = find_file ( "CHAButton.png" , GRAPHICS_DIR, FALSE);
      CHA_ButtonImage = IMG_Load( fpath );
      fpath = find_file ( "INVButton.png" , GRAPHICS_DIR, FALSE);
      INV_ButtonImage = IMG_Load( fpath );
      fpath = find_file ( "PlusButton.png" , GRAPHICS_DIR, FALSE);
      PlusButtonImage = IMG_Load( fpath );
    }

  CHA_Button_Rect.x = CHA_BUTTON_X;
  CHA_Button_Rect.y = CHA_BUTTON_Y;
  // CHA_Button_Rect.w = CHARACTERRECT_W;
  // CHA_Button_Rect.h = CHARACTERRECT_H;
  
  INV_Button_Rect.x = INV_BUTTON_X;
  INV_Button_Rect.y = INV_BUTTON_Y;
  // INV_Button_Rect.w = CHARACTERRECT_W;
  // INV_Button_Rect.h = CHARACTERRECT_H;

  //--------------------
  // Now we can draw either the plus button or the 'cha' button, depending
  // on whether there are points to distribute or not
  //
  if ( Me.PointsToDistribute > 0 )
    {
      SDL_BlitSurface( PlusButtonImage , NULL , Screen , &CHA_Button_Rect );
    }
  else
    {
      SDL_BlitSurface( CHA_ButtonImage , NULL , Screen , &CHA_Button_Rect );
    }

  SDL_BlitSurface( INV_ButtonImage , NULL , Screen , &INV_Button_Rect );
  
  if ( CursorIsOnINVButton( GetMousePos_x() + 16 , GetMousePos_y() + 16 ) && axis_is_active && !WasPressed )
    {
      GameConfig.Inventory_Visible = ! GameConfig.Inventory_Visible;
    }

  if ( CursorIsOnCHAButton( GetMousePos_x() + 16 , GetMousePos_y() + 16 ) && axis_is_active && !WasPressed )
    {
      GameConfig.CharacterScreen_Visible = ! GameConfig.CharacterScreen_Visible;
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
 * This function checks if a given screen position lies within the 
 * dexterity plus button or not
 * ---------------------------------------------------------------------- */
int
CursorIsOnDexButton( int x , int y )
{
  if ( x > CHARACTERRECT_X + BUTTON_MOD_X + STR_NOW_X + BUTTON_WIDTH ) return ( FALSE );
  if ( x < CHARACTERRECT_X + BUTTON_MOD_X + STR_NOW_X ) return ( FALSE );
  if ( y > CharacterRect.y + BUTTON_MOD_X + DEX_Y + BUTTON_HEIGHT ) return ( FALSE );
  if ( y < CharacterRect.y + BUTTON_MOD_X + DEX_Y ) return ( FALSE );
  return ( TRUE );
}; // int CursorIsOnDexButton( int x , int y )

/* ----------------------------------------------------------------------
 * This function checks if a given screen position lies within the 
 * magic plus button or not
 * ---------------------------------------------------------------------- */
int
CursorIsOnMagButton( int x , int y )
{
  if ( x > CHARACTERRECT_X + BUTTON_MOD_X + STR_NOW_X + BUTTON_WIDTH ) return ( FALSE );
  if ( x < CHARACTERRECT_X + BUTTON_MOD_X + STR_NOW_X ) return ( FALSE );
  if ( y > CharacterRect.y + BUTTON_MOD_X + MAG_Y + BUTTON_HEIGHT ) return ( FALSE );
  if ( y < CharacterRect.y + BUTTON_MOD_X + MAG_Y ) return ( FALSE );
  return ( TRUE );
}; // int CursorIsOnMagButton( int x , int y )

/* ----------------------------------------------------------------------
 * This function checks if a given screen position lies within the 
 * vitality plus button or not
 * ---------------------------------------------------------------------- */
int
CursorIsOnVitButton( int x , int y )
{
  if ( x > CHARACTERRECT_X + BUTTON_MOD_X + STR_NOW_X + BUTTON_WIDTH ) return ( FALSE );
  if ( x < CHARACTERRECT_X + BUTTON_MOD_X + STR_NOW_X ) return ( FALSE );
  if ( y > CharacterRect.y + BUTTON_MOD_X + VIT_Y + BUTTON_HEIGHT ) return ( FALSE );
  if ( y < CharacterRect.y + BUTTON_MOD_X + VIT_Y ) return ( FALSE );
  return ( TRUE );
}; // int CursorIsOnVitButton( int x , int y )



/* ----------------------------------------------------------------------
 * This function should re-compute all character stats according to the
 * currently equipped items and currenly distributed stats points.
 * ---------------------------------------------------------------------- */
void 
UpdateAllCharacterStats ( void )
{
  int BaseExpRequired = 2000;

  //--------------------
  // Maybe the influencer has reached a new experience level?
  // Let's check this.
  // 
  Me.ExpRequired = BaseExpRequired * ( exp ( ( Me.exp_level - 1 ) * log ( 2 ) ) ) ;

  if ( Me.Experience > Me.ExpRequired ) 
    {
      Me.exp_level ++ ;
      Me.PointsToDistribute += 5;
      Me.energy = Druidmap [ Me.type ].maxenergy ;
    }


  //--------------------
  // First we compute the maximum energy of the influencer
  //
  // Druidmap[ DRUID001 ].maxenergy = 100 + (Me.Vitality - 15) * ENERGY_GAIN_PER_VIT_POINT;
  // Druidmap[ DRUID001 ].maxmana = 100 + (Me.Magic - 15) * MANA_GAIN_PER_MAGIC_POINT;
  Druidmap[ DRUID001 ].maxenergy = (Me.Vitality) * ENERGY_GAIN_PER_VIT_POINT;
  Druidmap[ DRUID001 ].maxmana = (Me.Magic) * MANA_GAIN_PER_MAGIC_POINT;

  //--------------------
  // Now we compute the damage the influecer can do
  //
  if ( Me.weapon_item.type != (-1) )
    {
      /*
      Me.Base_Damage = Me.weapon_item.damage + 
	(Me.Strength - 15) * DAMAGE_GAIN_PER_STR_POINT;
      Me.Damage_Modifier = Me.weapon_item.damage_modifier + 
	(Me.Strength - 15) * DAMAGE_GAIN_PER_STR_POINT;
      */
      if ( ItemMap[ Me.weapon_item.type ].item_gun_angle_change != 0 )
	{
	  Me.Base_Damage = Me.weapon_item.damage * 
	    ( Me.Strength + 100.0) / 100.0 ;
	  Me.Damage_Modifier = Me.weapon_item.damage_modifier * 
	    ( Me.Strength + 100.0) / 100.0 ;
	}
      else
	{
	  Me.Base_Damage = Me.weapon_item.damage * 
	    ( Me.Dexterity + 100.0) / 100.0 ;
	  Me.Damage_Modifier = Me.weapon_item.damage_modifier * 
	    ( Me.Dexterity + 100.0) / 100.0 ;
	}
    }
  else
    {
      Me.Base_Damage = 0;
    }

  //--------------------
  // Now we compute the armour class of the influecer
  //
  Me.AC = ( Me.Dexterity - 15 ) * AC_GAIN_PER_DEX_POINT;
  if ( Me.armour_item.type != (-1) )
    {
      // DebugPrintf( 2 , "\nAn armour is beeing used!!!, type = %d" , Me.armour_item.type);
      // DebugPrintf( 2 , "\nAC bonus = %f" , ItemMap[ Me.armour_item.type ].item_armour_ac_bonus );
      // Me.AC += ItemMap[ Me.armour_item.type ].item_armour_ac_bonus;
      Me.AC += Me.armour_item.ac_bonus;
    }
  if ( Me.shield_item.type != (-1) )
    {
      // DebugPrintf( 0 , "\nA shield is beeing used!!!, type = %d" , Me.shield_item.type);
      // DebugPrintf( 0 , "\nAC bonus = %f" , ItemMap[ Me.shield_item.type ].item_shield_ac_bonus );
      // Me.AC += ItemMap[ Me.shield_item.type ].item_shield_ac_bonus;
      Me.AC += Me.shield_item.ac_bonus;
    }

  //--------------------
  // Now we compute the current to-hit chance of the influencer
  // 
  Me.to_hit = 60 + ( Me.Dexterity - 15 ) * TOHIT_PERCENT_PER_DEX_POINT;

}; // void UpdateAllCharacterStats ( void )


/* ----------------------------------------------------------------------
 * This function displays the character screen.
 * ---------------------------------------------------------------------- */
void 
ShowCharacterScreen ( void )
{
  static SDL_Rect ButtonRect;
  static SDL_Surface *CharacterScreenImage = NULL;
  static SDL_Surface *PlusButtonImage = NULL;
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
      fpath = find_file ( "character.png" , GRAPHICS_DIR, FALSE);
      CharacterScreenImage = IMG_Load( fpath );
      fpath = find_file ( "PlusButton.png" , GRAPHICS_DIR, FALSE);
      PlusButtonImage = IMG_Load( fpath );

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
  DisplayText( Me.character_name , 20 + CharacterRect.x , 18 + CharacterRect.y , &CharacterRect );
  switch ( Me.character_class )
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

  sprintf( CharText , "%4d", Me.exp_level );
  DisplayText( CharText , 62 + CharacterRect.x , 56 + CharacterRect.y , &CharacterRect );

  // Me.Experience = RealScore;
  sprintf( CharText , "%6ld", Me.Experience ); // this should be the real score, sooner or later
  DisplayText( CharText , 240 + CharacterRect.x ,  EXPERIENCE_Y + CharacterRect.y , &CharacterRect );

  sprintf( CharText , "%6ld", Me.ExpRequired ); 
  DisplayText( CharText , 240 + CharacterRect.x ,  NEXT_LEVEL_Y + CharacterRect.y , &CharacterRect );

  sprintf( CharText , "%6ld", Me.Gold ); 
  DisplayText( CharText , 240 + CharacterRect.x ,  GOLD_Y + CharacterRect.y , &CharacterRect );

  sprintf( CharText , "%d", Me.Strength );
  DisplayText( CharText , STR_BASE_X + CharacterRect.x , STR_Y + CharacterRect.y , &CharacterRect );
  sprintf( CharText , "%d", Me.Strength );
  DisplayText( CharText , STR_NOW_X + CharacterRect.x , STR_Y + CharacterRect.y , &CharacterRect );

  sprintf( CharText , "%d", Me.Magic );
  DisplayText( CharText , 100 + CharacterRect.x , MAG_Y + CharacterRect.y , &CharacterRect );
  sprintf( CharText , "%d", Me.Magic );
  DisplayText( CharText , 148 + CharacterRect.x , MAG_Y + CharacterRect.y , &CharacterRect );

  sprintf( CharText , "%d", Me.Dexterity );
  DisplayText( CharText , 100 + CharacterRect.x , DEX_Y + CharacterRect.y , &CharacterRect );
  sprintf( CharText , "%d", Me.Dexterity );
  DisplayText( CharText , 148 + CharacterRect.x , DEX_Y + CharacterRect.y , &CharacterRect );

  sprintf( CharText , "%d", Me.Vitality );
  DisplayText( CharText , 100 + CharacterRect.x , VIT_Y + CharacterRect.y , &CharacterRect );
  sprintf( CharText , "%d", Me.Vitality );
  DisplayText( CharText , 148 + CharacterRect.x , VIT_Y + CharacterRect.y , &CharacterRect );

  sprintf( CharText , "%d", Me.PointsToDistribute );
  DisplayText( CharText , 100 + CharacterRect.x , POINTS_Y + CharacterRect.y , &CharacterRect );

  sprintf( CharText , "%d", (int) Druidmap[ DRUID001 ].maxenergy );
  DisplayText( CharText , 95 + CharacterRect.x , 293 + CharacterRect.y , &CharacterRect );

  sprintf( CharText , "%d", (int) Me.energy );
  DisplayText( CharText , 143 + CharacterRect.x , 293 + CharacterRect.y , &CharacterRect );

  sprintf( CharText , "%d", (int) Druidmap[ DRUID001 ].maxmana );
  DisplayText( CharText , 95 + CharacterRect.x , 318 + CharacterRect.y , &CharacterRect );

  sprintf( CharText , "%d", (int) Me.mana );
  DisplayText( CharText , 143 + CharacterRect.x , 318 + CharacterRect.y , &CharacterRect );

  sprintf( CharText , "%d-%d", (int) Me.Base_Damage , (int) Me.Base_Damage + (int) Me.Damage_Modifier );
  DisplayText( CharText , DAMAGE_X + CharacterRect.x , DAMAGE_Y + CharacterRect.y , &CharacterRect );

  // sprintf( CharText , "%d", (int) Me.RechargeTimeModifier );
  sprintf( CharText , "%d", (int) Me.to_hit );
  strcat( CharText , "%" );
  DisplayText( CharText , RECHARGE_X + CharacterRect.x , RECHARGE_Y + CharacterRect.y , &CharacterRect );

  sprintf( CharText , "%d", (int) Me.AC );
  DisplayText( CharText , AC_X + CharacterRect.x , AC_Y + CharacterRect.y , &CharacterRect );

  //--------------------
  // It might be the case, that the character has some points to distribute upon the character
  // stats.  Then of course, we must display the plus button instead of all character 'now' values
  //
  // Me.PointsToDistribute = 5;
  if ( Me.PointsToDistribute > 0 )
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
	  Me.Strength++;
	  Me.PointsToDistribute--;
	}
      if ( CursorIsOnDexButton( CurPos.x , CurPos.y ) && ( axis_is_active ) && ( ! MouseButtonPressedPreviousFrame ) )
	{
	  Me.Dexterity++;
	  Me.PointsToDistribute--;
	}
      if ( CursorIsOnMagButton( CurPos.x , CurPos.y ) && ( axis_is_active ) && ( ! MouseButtonPressedPreviousFrame ) )
	{
	  Me.Magic++;
	  Me.PointsToDistribute--;
	  Me.mana += MANA_GAIN_PER_MAGIC_POINT;
	}
      if ( CursorIsOnVitButton( CurPos.x , CurPos.y ) && ( axis_is_active ) && ( ! MouseButtonPressedPreviousFrame ) )
	{
	  Me.Vitality++;
	  Me.PointsToDistribute--;
	  Me.health += ENERGY_GAIN_PER_VIT_POINT;
	  Me.energy += ENERGY_GAIN_PER_VIT_POINT;
	}

      //--------------------
      // It might happen that the last str point was just spent.  Then we can
      // automatically close the character window for convenience of the player.
      //
      if ( Me.PointsToDistribute == 0 ) GameConfig.CharacterScreen_Visible = FALSE;
    }

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
