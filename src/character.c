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

#define STR_BASE_X 100
#define STR_NOW_X 148
#define STR_Y 143
#define MAG_Y 171
#define DEX_Y 200
#define VIT_Y 227
#define POINTS_Y 252

#define BUTTON_MOD_X (-6)
#define BUTTON_MOD_Y (-4)
#define BUTTON_WIDTH 35
#define BUTTON_HEIGHT 19

#define DAMAGE_X 260
#define DAMAGE_Y 228

#define RECHARGE_X 260
#define RECHARGE_Y 200

#define AC_X 260
#define AC_Y 171

#define CHARACTERRECT_X (SCREENLEN/2)
#define CHARACTERRECT_Y (User_Rect.y)
#define CHARACTERRECT_W (SCREENLEN/2)
#define CHARACTERRECT_H (User_Rect.h)

#define ENERGY_GAIN_PER_VIT_POINT 4
#define DAMAGE_GAIN_PER_STR_POINT 2
#define AC_GAIN_PER_DEX_POINT 1
#define RECHARGE_SPEED_PERCENT_PER_DEX_POINT 3

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
  static SDL_Rect CHA_Button_Rect;
  static SDL_Rect INV_Button_Rect;
  static int WasPressed;
  char* fpath;
  
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

    }

      CHA_Button_Rect.x = CHA_BUTTON_X;
      CHA_Button_Rect.y = CHA_BUTTON_Y;
      // CHA_Button_Rect.w = CHARACTERRECT_W;
      // CHA_Button_Rect.h = CHARACTERRECT_H;

      INV_Button_Rect.x = INV_BUTTON_X;
      INV_Button_Rect.y = INV_BUTTON_Y;
      // INV_Button_Rect.w = CHARACTERRECT_W;
      // INV_Button_Rect.h = CHARACTERRECT_H;
  
  SDL_BlitSurface( CHA_ButtonImage , NULL , Screen , &CHA_Button_Rect );
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
  if ( y > CHARACTERRECT_Y + BUTTON_MOD_X + STR_Y + BUTTON_HEIGHT ) return ( FALSE );
  if ( y < CHARACTERRECT_Y + BUTTON_MOD_X + STR_Y ) return ( FALSE );
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
  if ( y > CHARACTERRECT_Y + BUTTON_MOD_X + DEX_Y + BUTTON_HEIGHT ) return ( FALSE );
  if ( y < CHARACTERRECT_Y + BUTTON_MOD_X + DEX_Y ) return ( FALSE );
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
  if ( y > CHARACTERRECT_Y + BUTTON_MOD_X + MAG_Y + BUTTON_HEIGHT ) return ( FALSE );
  if ( y < CHARACTERRECT_Y + BUTTON_MOD_X + MAG_Y ) return ( FALSE );
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
  if ( y > CHARACTERRECT_Y + BUTTON_MOD_X + VIT_Y + BUTTON_HEIGHT ) return ( FALSE );
  if ( y < CHARACTERRECT_Y + BUTTON_MOD_X + VIT_Y ) return ( FALSE );
  return ( TRUE );
}; // int CursorIsOnVitButton( int x , int y )



/* ----------------------------------------------------------------------
 * This function should re-compute all character stats according to the
 * currently equipped items and currenly distributed stats points.
 * ---------------------------------------------------------------------- */
void 
UpdateAllCharacterStats ( void )
{
  int BaseExpRequired = 1500;

  //--------------------
  // Maybe the influencer has reached a new experience level?
  // Let's check this.
  // 
  // if ( Me.exp_level == 1 ) Me.ExpRequired = BaseExpRequired;
  Me.ExpRequired = BaseExpRequired * ( exp ( ( Me.exp_level - 1 ) * log ( 2 ) ) ) ;

  if ( RealScore > Me.ExpRequired ) 
    {
      Me.exp_level ++ ;
      Me.PointsToDistribute += 5;
      
    }


  //--------------------
  // First we compute the maximum energy of the influencer
  //
  Druidmap[ DRUID001 ].maxenergy = 100 + (Me.Vitality - 15) * ENERGY_GAIN_PER_VIT_POINT;
  
  //--------------------
  // Now we compute the damage the influecer can do
  //
  if ( Druidmap[ DRUID001 ].weapon_item.type != (-1) )
    {
      Me.Damage = ItemMap[ Druidmap[ DRUID001 ].weapon_item.type ].item_gun_damage + 
	(Me.Strength - 15) * DAMAGE_GAIN_PER_STR_POINT;
    }
  else
    {
      Me.Damage = 0;
    }

  //--------------------
  // Now we compute the armour class of the influecer
  //
  Me.AC = ( Me.Dexterity - 15 ) * AC_GAIN_PER_DEX_POINT;
  if ( Druidmap[ DRUID001 ].armour_item.type != (-1) )
    {
      // DebugPrintf( 2 , "\nAn armour is beeing used!!!, type = %d" , Druidmap[ Me.type ].armour_item.type);
      // DebugPrintf( 2 , "\nAC bonus = %f" , ItemMap[ Druidmap[ Me.type ].armour_item.type ].item_armour_ac_bonus );
      Me.AC += ItemMap[ Druidmap[ Me.type ].armour_item.type ].item_armour_ac_bonus;
    }

  //--------------------
  // Now we compute the current recharging time for the weapon for each shot
  // And also the current recharge time modifier to be applied.
  // 
  Me.RechargeTimeModifier = 100 - ( Me.Dexterity - 15 ) * RECHARGE_SPEED_PERCENT_PER_DEX_POINT;
  Me.RechargeTime = ItemMap[ Druidmap[ DRUID001 ].weapon_item.type ].item_gun_recharging_time * 
    0.01 * Me.RechargeTimeModifier;

}; // void UpdateAllCharacterStats ( void )


/* ----------------------------------------------------------------------
 * This function displays the character screen.
 * ---------------------------------------------------------------------- */
void 
ShowCharacterScreen ( void )
{
  static SDL_Rect CharacterRect;
  static SDL_Rect ButtonRect;
  static SDL_Surface *CharacterScreenImage = NULL;
  static SDL_Surface *PlusButtonImage = NULL;
  char *fpath;
  char CharText[1000];
  SDL_Rect TargetRect;
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
      /*
      CharacterRect.x = SCREENLEN/2;
      CharacterRect.y = User_Rect.y;
      CharacterRect.w = SCREENLEN/2;
      CharacterRect.h = User_Rect.h;
      */
      CharacterRect.x = CHARACTERRECT_X;
      CharacterRect.y = CHARACTERRECT_Y; 
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
  // Name, Level, Exp, Strength, Dex, ...
  //
  DisplayText( Me.character_name , 20 + CharacterRect.x , 18 + CharacterRect.y , &CharacterRect );

  sprintf( CharText , "%d", Me.exp_level );
  DisplayText( CharText , 62 + CharacterRect.x , 56 + CharacterRect.y , &CharacterRect );

  Me.Experience = RealScore;
  sprintf( CharText , "%6ld", Me.Experience ); // this should be the real score, sooner or later
  DisplayText( CharText , 250 + CharacterRect.x ,  58 + CharacterRect.y , &CharacterRect );

  sprintf( CharText , "%6ld", Me.ExpRequired ); 
  DisplayText( CharText , 250 + CharacterRect.x ,  85 + CharacterRect.y , &CharacterRect );

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

  sprintf( CharText , "%d", (int) Me.Damage );
  DisplayText( CharText , DAMAGE_X + CharacterRect.x , DAMAGE_Y + CharacterRect.y , &CharacterRect );

  sprintf( CharText , "%d", (int) Me.RechargeTimeModifier );
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
	}
      if ( CursorIsOnVitButton( CurPos.x , CurPos.y ) && ( axis_is_active ) && ( ! MouseButtonPressedPreviousFrame ) )
	{
	  Me.Vitality++;
	  Me.PointsToDistribute--;
	  Me.health += ENERGY_GAIN_PER_VIT_POINT;
	  Me.energy += ENERGY_GAIN_PER_VIT_POINT;
	}
    }

  //--------------------
  // Finally, we want the part of the screen we have been editing to become
  // visible and therefore we must updated it here, since it is currently not
  // contained within the user rectangle that also gets updated every frame.
  //
  SDL_UpdateRect( Screen , CharacterRect.x , CharacterRect.y , CharacterRect.w , CharacterRect.h );


  //--------------------
  // We want to know, if the button was pressed the previous frame when we
  // are in the next frame and back in this function.  Therefore we store
  // the current button situation, so that we can conclude on button just
  // pressed later.
  //
  MouseButtonPressedPreviousFrame = axis_is_active;

}; // ShowCharacterScreen ( void )

#undef _character_c
