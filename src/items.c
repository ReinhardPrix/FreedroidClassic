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
 * This file contains all the functions managing the items in the game.
 * ---------------------------------------------------------------------- */

#define _items_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"



void
ApplyItemFromInventory( int ItemNum )
{
  DebugPrintf( 0 , "\nvoid ApplyItemFromInventory( int ItemNum ): function call confirmed.");

  // If the inventory slot is not at all filled, we need not do anything more...
  if ( Me.Inventory[ ItemNum ].type == (-1) ) return;

  if ( ItemMap[ Me.Inventory[ ItemNum ].type ].item_can_be_applied_in_combat == FALSE ) 
    {
      Me.TextVisibleTime = 0;
      Me.TextToBeDisplayed = "I can't use this item here.";
      return;
    }

  //--------------------
  // At this point we know that the item is applicable in combat situation
  // and therefore all we need to do from here on is execute the item effect
  // upon the influencer or his environment.
  //
  Me.health += ItemMap[ Me.Inventory[ ItemNum ].type ].energy_gain_uppon_application_in_combat;
  Me.energy += ItemMap[ Me.Inventory[ ItemNum ].type ].energy_gain_uppon_application_in_combat;

  //--------------------
  // In some cases the item concerned is a one-shot-device like a health potion, which should
  // evaporize after the first application.  Therefore we delete the item from the inventory list.
  //
  Me.Inventory[ ItemNum ].type = (-1);

}; // void ApplyItemFromInventory( int ItemNum )

int
Inv_Pos_Is_Free( int x , int y )
{
  int i;
  int item_width;
  int item_height;
  

  for ( i = 0 ; i < MAX_ITEMS_IN_INVENTORY; i++ )
    {
      if ( Me.Inventory[ i ].type == ( -1 ) ) continue;

      // for ( item_height = 0 ; item_height < ItemSizeTable[ Me.Inventory[ i ].type ].y ; item_height ++ )
      for ( item_height = 0 ; item_height < ItemImageList[ ItemMap[ Me.Inventory[ i ].type ].picture_number ].inv_size.y ; item_height ++ )
	{
	  for ( item_width = 0 ; item_width < ItemImageList[ ItemMap[ Me.Inventory[ i ].type ].picture_number ].inv_size.x ; item_width ++ )
	    {
	      if ( ( ( Me.Inventory[ i ].inventory_position.x + item_width ) == x ) &&
		   ( ( Me.Inventory[ i ].inventory_position.y + item_height ) == y ) )
		return ( FALSE );
	    }
	}
    }
  return ( TRUE );
}; // int Inv_Pos_Is_Free( Inv_Loc.x , Inv_Loc.y )



int 
GetInventoryItemAt ( int x , int y )
{
  int i;
  int item_width;
  int item_height;
  

  for ( i = 0 ; i < MAX_ITEMS_IN_INVENTORY; i++ )
    {
      if ( Me.Inventory[ i ].type == ( -1 ) ) continue;

      for ( item_height = 0 ; item_height < ItemImageList[ ItemMap[ Me.Inventory[ i ].type ].picture_number ].inv_size.y ; item_height ++ )
	{
	  for ( item_width = 0 ; item_width < ItemImageList[ ItemMap[ Me.Inventory[ i ].type ].picture_number ].inv_size.x ; item_width ++ )
	    {
	      if ( ( ( Me.Inventory[ i ].inventory_position.x + item_width ) == x ) &&
		   ( ( Me.Inventory[ i ].inventory_position.y + item_height ) == y ) )
		{
		  
		  return ( i );
		}
	    }
	}
    }
  return ( -1 ); // Nothing found at this grabbing location!!

};


#undef _items_c
