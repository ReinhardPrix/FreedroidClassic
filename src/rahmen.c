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
 * This file contains all functions to update and draw the top status 
 * displays with status etc...
 * ---------------------------------------------------------------------- */
/*
 * This file has been checked for remains of german comments in the code
 * I you still find some, please just kill it mercilessly.
 */

#define _rahmen_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "proto.h"
#include "global.h"
#include "text.h"
#include "items.h"

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
  char font_switchto_red [ 2 ] = { 1 , 0 };
  char font_switchto_blue [ 2 ] = { 2 , 0 };
  char font_switchto_neon [ 2 ] = { 3 , 0 };

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
      fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
An item description was requested for an item, that does not seem to \n\
exist really (i.e. has a type = (-1) ).\n\
\n\
This indicated a severe bug in freedroid.\n\
Please inform the developers about the problem.\n\
\n\
But for now Freedroid will terminate to draw attention \n\
to the item type problem it could not resolve.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" );
      Terminate( ERR );
      return;
    }

  // --------------------
  // First clear the string and the we print out the item name.  That's simple.
  // we also add the extension of the name, the 'suffix' to it.
  //
  if ( ( CurItem->suffix_code != (-1) ) || ( CurItem->prefix_code != (-1) ) )
    {
      strcat ( ItemDescText , font_switchto_blue );
    }
  else
    {
      strcat ( ItemDescText , font_switchto_neon );
    }

  if ( CurItem->type == ITEM_MONEY ) sprintf( ItemDescText , "%d " , CurItem->gold_amount );

  if ( ( CurItem->prefix_code != (-1) ) && ( CurItem->is_identified ) )
    strcat( ItemDescText , PrefixList[ CurItem->prefix_code ].bonus_name );

  strcat( ItemDescText , ItemMap[ CurItem->type ].ItemName );

  if ( ( CurItem->suffix_code != (-1) ) && ( CurItem->is_identified ) )
    strcat( ItemDescText , SuffixList[ CurItem->suffix_code ].bonus_name );

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

  // --------------------
  // If it's a drive, then we give out the maxspeed and accel values as well
  //
  if ( ItemMap[ CurItem->type ].item_can_be_installed_in_drive_slot )
    {
      if ( ForShop )
	sprintf( linebuf , "Speed: %2.1f Accel: %2.1f" , 
		 ItemMap[ CurItem->type ].item_drive_maxspeed ,
		 ItemMap[ CurItem->type ].item_drive_accel );
      else
	sprintf( linebuf , "Maximum Speed: %2.1f \n Maximum Acceleration: %2.1f \n " , 
		 ItemMap[ CurItem->type ].item_drive_maxspeed ,
		 ItemMap[ CurItem->type ].item_drive_accel );

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
      sprintf( linebuf , "Right click to use" );
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
	  
	  if ( CurItem->bonus_to_str )
	    {
	      if ( ForShop ) strcat( ItemDescText , "             " );
	      sprintf( linebuf , "+%d to Power\n" , CurItem->bonus_to_str );
	      strcat( ItemDescText , linebuf );
	    }
	  
	  if ( CurItem->bonus_to_dex )
	    {
	      if ( ForShop ) strcat( ItemDescText , "             " );
	      sprintf( linebuf , "+%d to Power Distribution\n" , CurItem->bonus_to_dex );
	      strcat( ItemDescText , linebuf );
	    }
	  
	  if ( CurItem->bonus_to_mag )
	    {
	      if ( ForShop ) strcat( ItemDescText , "             " );
	      sprintf( linebuf , "+%d to Mind\n" , CurItem->bonus_to_mag );
	      strcat( ItemDescText , linebuf );
	    }
	  
	  if ( CurItem->bonus_to_vit )
	    {
	      if ( ForShop ) strcat( ItemDescText , "             " );
	      sprintf( linebuf , "+%d to Vitality\n" , CurItem->bonus_to_vit );
	      strcat( ItemDescText , linebuf );
	    }
	  
	  if ( CurItem->bonus_to_life )
	    {
	      if ( ForShop ) strcat( ItemDescText , "             " );
	      sprintf( linebuf , "+%d Energy\n" , CurItem->bonus_to_life );
	      strcat( ItemDescText , linebuf );
	    }
	  
	  if ( CurItem->bonus_to_force )
	    {
	      if ( ForShop ) strcat( ItemDescText , "             " );
	      sprintf( linebuf , "+%d Force\n" , CurItem->bonus_to_force );
	      strcat( ItemDescText , linebuf );
	    }
	  
	  if ( CurItem->bonus_to_tohit )
	    {
	      if ( ForShop ) strcat( ItemDescText , "             " );
	      sprintf( linebuf , "+%d%% to hit\n" , CurItem->bonus_to_tohit );
	      strcat( ItemDescText , linebuf );
	    }

	  if ( CurItem->bonus_to_all_attributes )
	    {
	      if ( ForShop ) strcat( ItemDescText , "             " );
	      sprintf( linebuf , "+%d to all attributes \n" , CurItem->bonus_to_all_attributes );
	      strcat( ItemDescText , linebuf );
	    }

	  // Now we display the percentage bonus to ac or damage
	  if ( CurItem->bonus_to_ac_or_damage )
	    {
	      if ( ItemMap [ CurItem->type ].base_ac_bonus )
		{
		  if ( ForShop ) strcat( ItemDescText , "             " );
		  sprintf( linebuf , "+%d%% to armour \n" , CurItem->bonus_to_ac_or_damage );
		  strcat( ItemDescText , linebuf );
		}
	      if ( ItemMap [ CurItem->type ].base_item_gun_damage )
		{
		  if ( ForShop ) strcat( ItemDescText , "             " );
		  sprintf( linebuf , "+%d%% to damage \n" , CurItem->bonus_to_ac_or_damage );
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
GiveDroidDescription ( char* DroidDescText , enemy* CurEnemy )
{
  char linebuf[1000];
  
  // --------------------
  // First we print out the droid name.  That's simple.
  //
  strcpy( DroidDescText , Druidmap[ CurEnemy->type ].druidname );
  strcat( DroidDescText , " -- " );
  strcat( DroidDescText , Classname [ Druidmap[ CurEnemy->type ].class ] );
  strcat( DroidDescText , "\n" );

  if ( CurEnemy->Friendly == FALSE )
    {
      sprintf( linebuf , " Total Kills : %d " , Me[0].KillRecord[ CurEnemy->type ] );
      strcat ( DroidDescText , linebuf );
    }

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

  Target_Rect.x = CURRENT_SKILL_RECT_X ;
  Target_Rect.y = CURRENT_SKILL_RECT_Y ;
  Target_Rect.w = CURRENT_SKILL_RECT_W ;
  Target_Rect.h = CURRENT_SKILL_RECT_H ;

  SDL_BlitSurface ( SkillIconSurfacePointer[ Me[0].readied_skill ] , NULL , Screen , &Target_Rect );


  //--------------------
  // Here we also check for possible mouse clicks on the skill icon.  In this
  // case we activate or deactivate the skill screen.
  //
  if ( ( GetMousePos_x()+16 >= CURRENT_SKILL_RECT_X ) &&
       ( GetMousePos_x()+16 <= CURRENT_SKILL_RECT_X + CURRENT_SKILL_RECT_W ) &&
       ( GetMousePos_y()+16 >= CURRENT_SKILL_RECT_Y ) &&
       ( GetMousePos_y()+16 <= CURRENT_SKILL_RECT_Y + CURRENT_SKILL_RECT_H ) &&
       axis_is_active &&
       !Mouse_Button_Pressed_Previous_Frame )
    GameConfig.SkillScreen_Visible = ! GameConfig.SkillScreen_Visible;

  Mouse_Button_Pressed_Previous_Frame = axis_is_active;

}; // void ShowCurrentSkill ( void )

/* ----------------------------------------------------------------------
 * This function displays the status bars for mana and energy in some 
 * corner of the screen.  The dimensions and location of the bar are
 * specified in items.h
 * ---------------------------------------------------------------------- */
void 
ShowCurrentHealthAndForceLevel( void )
{
  SDL_Rect Health_Rect;
  SDL_Rect Unhealth_Rect;
  SDL_Rect Force_Rect;
  SDL_Rect Unforce_Rect;

  char *fpath;

  static SDL_Surface *SpeedMeterImage = NULL;
  static SDL_Surface *SpeedMeterEnergyArrowImage = NULL;
  static SDL_Surface *SpeedMeterManaArrowImage = NULL;
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
  if ( SpeedMeterImage == NULL )
    {
      fpath = find_file ( "speed_o_meter.png" , GRAPHICS_DIR, FALSE);
      SpeedMeterImage = IMG_Load( fpath );
      fpath = find_file ( "speed_o_meter_arrow_energy.png" , GRAPHICS_DIR, FALSE);
      SpeedMeterEnergyArrowImage = IMG_Load( fpath );
      fpath = find_file ( "speed_o_meter_arrow_mana.png" , GRAPHICS_DIR, FALSE);
      SpeedMeterManaArrowImage = IMG_Load( fpath );
      SpeedOMeterWorkingCopy = SDL_DisplayFormat( SpeedMeterImage ); // to initialize the thing

      //--------------------
      // We define the right side of the user screen as the rectangle
      // for our speed-o-meter
      //
      SpeedMeterRect.x = SCREEN_WIDTH - SpeedMeterImage->w;
      SpeedMeterRect.y = 0; 
      SpeedMeterRect.w = SpeedMeterImage->w;
      SpeedMeterRect.h = SpeedMeterImage->h;
    }

  //--------------------
  // Of course we do not blit anything during the briefing phase, so as
  // not to disturb the scrolling text at the beginning.
  //
  if ( Me[0].status == BRIEFING ) return;
  
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
      SDL_FreeSurface ( SpeedOMeterWorkingCopy );
      SpeedOMeterWorkingCopy = SDL_DisplayFormat ( SpeedMeterImage );
      SDL_SetColorKey ( SpeedOMeterWorkingCopy , SDL_SRCCOLORKEY, 
			SDL_MapRGB ( SpeedOMeterWorkingCopy -> format , 255, 0, 255 ) ); 
      

      // We blit in the red arrow, showing current energy
      RotatedArrow = rotozoomSurface( SpeedMeterEnergyArrowImage , 
				      - 360 * 3 / 4 * Me[0].energy / Me[0].maxenergy , 1.0 , FALSE );

      ArrowRect.x = PivotPosition.x - ( RotatedArrow->w / 2 ) ;
      ArrowRect.y = PivotPosition.y - ( RotatedArrow->h / 2 ) ;
      SDL_BlitSurface( RotatedArrow , NULL , SpeedOMeterWorkingCopy , & ArrowRect );
      SDL_FreeSurface( RotatedArrow );

      // We blit in the blue arrow, showing current mana
      RotatedArrow = rotozoomSurface( SpeedMeterManaArrowImage , 
				      -360 * 3 / 4 * Me[0].mana / Me[0].maxmana , 1.0 , FALSE );

      ArrowRect.x = PivotPosition.x - ( RotatedArrow->w / 2 ) ;
      ArrowRect.y = PivotPosition.y - ( RotatedArrow->h / 2 ) ;
      SDL_BlitSurface( RotatedArrow , NULL , SpeedOMeterWorkingCopy , & ArrowRect );
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
      DebugPrintf ( 2 , "\nValue change detected. --> Speed-o-meter completely reassembled...." ) ;
    }


  //--------------------
  // Finally, we blit the fully assembled speed-o-meter
  //
  SDL_BlitSurface( SpeedOMeterWorkingCopy , NULL , Screen, &SpeedMeterRect );

  return; // The rest can remain disabled for now...

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

  SDL_SetClipRect( Screen , NULL );
  SDL_FillRect( Screen , & ( Health_Rect ) , HEALTH_RECT_COLOR );
  SDL_FillRect( Screen , & ( Unhealth_Rect ) , 0x0FF00000 );
  SDL_FillRect( Screen , & ( Force_Rect ) , FORCE_RECT_COLOR );
  SDL_FillRect( Screen , & ( Unforce_Rect ) , 0x0FF0000 );
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
  grob_point inv_square;
  int InvIndex;
  int i;
  int NumberOfLinesInText = 1;
  finepoint MapPositionOfMouse;
  char* LongTextPointer;
  int InterLineDistance;
  int StringLength;

#define REQUIREMENTS_NOT_MET_TEXT "REQUIREMENTS NOT MET"

  //--------------------
  // During the title display phase, we need not have this window visible...
  //
  if ( Me[0].status == BRIEFING ) return;

  //--------------------
  // For testing purposes is bluntly insert the new banner element here:
  //
  // if ( GetMousePos_y( ) >= ( SCREEN_HEIGHT / 2 ) )
  if ( GetMousePos_y( ) + 16 >= ( UPPER_BANNER_TEXT_RECT_H + UPPER_BANNER_TEXT_RECT_Y ) )
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

  CurPos.x = GetMousePos_x() + 16 ;
  CurPos.y = GetMousePos_y() + 16 ;

  //--------------------
  // In case some item is held in hand by the player, the situation is simple:
  // we merely need to draw this items description into the description field and
  // that's it OR WE MUST SAY THAT THE requirements for this item are not met
  //
  // if ( GetHeldItemCode() != (-1) )
  if ( GetHeldItemPointer( ) != NULL )
    {
      if ( ItemUsageRequirementsMet( GetHeldItemPointer( ) , FALSE ) )
	strcpy( ItemDescText , ItemMap[ GetHeldItemCode() ].ItemName );
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
      else if ( CursorIsInWeaponRect ( CurPos.x , CurPos.y ) )
	{
	  if ( Me[0].weapon_item.type > 0 )
	    GiveItemDescription ( ItemDescText , & ( Me[0].weapon_item ) , FALSE );
	}
      else if ( CursorIsInDriveRect ( CurPos.x , CurPos.y ) )
	{
	  if ( Me[0].drive_item.type > 0 )
	   GiveItemDescription ( ItemDescText , & ( Me[0].drive_item) , FALSE );
	}
      else if ( CursorIsInShieldRect ( CurPos.x , CurPos.y ) )
	{
	   if ( Me[0].shield_item.type > 0 )
	   GiveItemDescription ( ItemDescText , & ( Me[0].shield_item) , FALSE );
	}
      else if ( CursorIsInArmourRect ( CurPos.x , CurPos.y ) )
	{
	   if ( Me[0].armour_item.type > 0 )
	   GiveItemDescription ( ItemDescText , & ( Me[0].armour_item) , FALSE );
	}
      else if ( CursorIsInAux1Rect ( CurPos.x , CurPos.y ) )
	{
	   if ( Me[0].aux1_item.type > 0 )
	   GiveItemDescription ( ItemDescText , & ( Me[0].aux1_item) , FALSE );
	}
      else if ( CursorIsInAux2Rect ( CurPos.x , CurPos.y ) )
	{
	   if ( Me[0].aux2_item.type > 0 )
	   GiveItemDescription ( ItemDescText , & ( Me[0].aux2_item) , FALSE );
	}
      else if ( CursorIsInSpecialRect ( CurPos.x , CurPos.y ) )
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
      MapPositionOfMouse.x = Me[0].pos.x + (CurPos.x - UserCenter_x) / (float) Block_Width;
      MapPositionOfMouse.y = Me[0].pos.y + (CurPos.y - UserCenter_y) / (float) Block_Height;
      // DebugPrintf( 2  , "\nMouse in map at: %f %f." , MapPositionOfMouse.x , MapPositionOfMouse.y );
      for ( i = 0 ; i < MAX_ITEMS_PER_LEVEL ; i++ )
	{
	  if ( CurLevel->ItemList[ i ].type == (-1) ) continue;

	  if ( ( fabsf( MapPositionOfMouse.x - CurLevel->ItemList[ i ].pos.x ) < 0.5 ) &&
	       ( fabsf( MapPositionOfMouse.y - CurLevel->ItemList[ i ].pos.y ) < 0.5 ) )
	    {
	      GiveItemDescription ( ItemDescText , &(CurLevel->ItemList[ i ]) , FALSE );
	      // strcpy( ItemDescText , ItemMap[ CurLevel->ItemList[ i ].type ].ItemName );
	    }
	}
      for ( i = 0 ; i < Number_Of_Droids_On_Ship ; i++ )
	{
	  // We don't describe enemys that are not on this level...
	  if ( AllEnemys[ i ].pos.z != CurLevel->levelnum ) continue;
	  if ( AllEnemys[ i ].Status == OUT ) continue;

	  if ( ( fabsf( MapPositionOfMouse.x - AllEnemys[ i ].pos.x ) < 0.5 ) &&
	       ( fabsf( MapPositionOfMouse.y - AllEnemys[ i ].pos.y ) < 0.5 ) )
	    {
	      GiveDroidDescription ( ItemDescText , &( AllEnemys[ i ]) );
	      // strcpy( ItemDescText , ItemMap[ CurLevel->ItemList[ i ].type ].ItemName );
	    }
	}
    }

  SDL_SetClipRect( Screen , NULL );  // this unsets the clipping rectangle
  if ( strlen( ItemDescText ) > 1 )
    {
      // SDL_FillRect( Screen , &Banner_Text_Rect , BANNER_TEXT_REC_BACKGROUNDCOLOR );
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


/* -----------------------------------------------------------------
 * This function updates the top status bar. 
 * To save framerate on slow machines however it will only work
 * if it thinks that work needs to be done. 
 * You can however force update if you say so with a flag.
 * 
 * BANNER_FORCE_UPDATE=1: Forces the redrawing of the title bar
 * 
 * BANNER_DONT_TOUCH_TEXT=2: Prevents DisplayBanner from touching 
 * the text.
 * 
 * BANNER_NO_SDL_UPDATE=4: Prevents any SDL_Update calls.
 * 
 ----------------------------------------------------------------- */
void
DisplayBanner (const char* left, const char* right,  int flags )
{
  char dummy[80];
  char left_box [LEFT_TEXT_LEN + 10];
  char right_box[RIGHT_TEXT_LEN + 10];
  static char previous_left_box [LEFT_TEXT_LEN + 10]="NOUGHT";
  static char previous_right_box[RIGHT_TEXT_LEN + 10]="NOUGHT";
  int left_len, right_len;   // the actualy string lengths

  SDL_SetClipRect( Screen , NULL );  // this unsets the clipping rectangle
  // SDL_BlitSurface( banner_pic, NULL, Screen , NULL);

  ShowCurrentHealthAndForceLevel ( );

  ShowCurrentTextWindow ( );

  ShowCurrentSkill ( );

  return;

  // --------------------
  // At first the text is prepared.  This can't hurt.
  // we will decide whether to dispaly it or not later...
  //

  if (left == NULL)       // Left-DEFAULT: Mode 
    left = InfluenceModeNames[Me[0].status];

  if ( right == NULL )  // Right-DEFAULT: Score 
    {
      sprintf ( dummy , "%ld" , ShowScore );
      right = dummy;
    }

  // Now fill in the text
  left_len = strlen (left);
  if( left_len > LEFT_TEXT_LEN )
    {
      printf ("\nWarning: String %s too long for Left Infoline!!",left);
      left_len = LEFT_TEXT_LEN;  // too long, so we cut it! 
      Terminate(ERR);
    }
  right_len = strlen (right);
  if( right_len > RIGHT_TEXT_LEN )
    {
      printf ("\nWarning: String %s too long for Right Infoline!!", right);
      right_len = RIGHT_TEXT_LEN;  // too long, so we cut it! 
      Terminate(ERR);
    }
  
  // Now prepare the left/right text-boxes 
  memset (left_box,  ' ', LEFT_TEXT_LEN);  // pad with spaces 
  memset (right_box, ' ', RIGHT_TEXT_LEN);  
  
  strncpy (left_box,  left, left_len);  // this drops terminating \0 ! 
  strncpy (right_box, right, left_len);  // this drops terminating \0 ! 
  
  left_box [LEFT_TEXT_LEN]  = '\0';     // that's right, we want padding!
  right_box[RIGHT_TEXT_LEN] = '\0';
  
  // --------------------
  // No we see if the screen need an update...

  if ( BannerIsDestroyed || 
       (flags & BANNER_FORCE_UPDATE ) || 
       (strcmp( left_box , previous_left_box )) || 
       (strcmp( right_box , previous_right_box )) )
    {
      // Redraw the whole background of the top status bar
      SDL_SetClipRect( Screen , NULL );  // this unsets the clipping rectangle
      SDL_BlitSurface( banner_pic, NULL, Screen , NULL);

      // Now the text should be ready and its
      // time to display it...
      if ( (strcmp( left_box , previous_left_box )) || 
	   (strcmp( right_box , previous_right_box )) ||
	   ( flags & BANNER_FORCE_UPDATE ) )
	{
	  PrintStringFont ( Screen, Para_BFont,
			    LEFT_INFO_X , LEFT_INFO_Y , left_box );
	  strcpy( previous_left_box , left_box );
	  PrintStringFont ( Screen, Para_BFont,
			    RIGHT_INFO_X , RIGHT_INFO_Y , right_box );
	  strcpy( previous_right_box , right_box );
	}

      // finally update the whole top status box
      if ( !(flags & BANNER_NO_SDL_UPDATE ) )
	SDL_UpdateRect( Screen, 0, 0, BANNER_WIDTH , BANNER_HEIGHT );
      BannerIsDestroyed=FALSE;
      return;
    } // if 

}; // void DisplayBanner( .. ) 


#undef _rahmen_c
