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

/* ----------------------------------------------------------------------
 * This function looks through the inventory list and returns the index
 * of the first inventory item that is marked as 'held in hand'.
 * If no such item is found, an index of (-1) is returned.
 * ---------------------------------------------------------------------- */
int
GetHeldItemInventoryIndex( void )
{
  int InvPos;

  // --------------------
  // First we find out the inventory index of the item we want to
  // drop
  //
  for ( InvPos = 0 ; InvPos < MAX_ITEMS_IN_INVENTORY ; InvPos ++ )
    {
      if ( Me.Inventory[ InvPos ].currently_held_in_hand ) break;
    }
  if ( InvPos >=  MAX_ITEMS_IN_INVENTORY )
    {
      DebugPrintf( 0 , "\nNo item in inventory seems to be currently held in hand...");
      InvPos = ( -1 ) ;
    }
  else
    {
      DebugPrintf( 0 , "\nInventory item index %d was held in hand." , InvPos );
    }
  return ( InvPos );
}; // int GetHeldItemInventoryIndex( void )

int
Inv_Pos_Is_Free( int x , int y )
{
  int i;
  int item_width;
  int item_height;
  

  for ( i = 0 ; i < MAX_ITEMS_IN_INVENTORY; i++ )
    {
      if ( Me.Inventory[ i ].type == ( -1 ) ) continue;
      if ( Me.Inventory[ i ].currently_held_in_hand ) continue;

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

/* ----------------------------------------------------------------------
 * This function returns the index in the invenotry list of the object
 * at the inventory position x y.  If no object is found to occupy that
 * square, an index of (-1) is returned.
 * ---------------------------------------------------------------------- */
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

}; // int GetInventoryItemAt ( int x , int y )

/* ----------------------------------------------------------------------
 * This function checks if a given screen position lies within the user
 * i.e. combat rectangle or not.
 * ---------------------------------------------------------------------- */
int 
CursorIsInUserRect( int x , int y )
{
  if ( x > User_Rect.x + User_Rect.w ) return ( FALSE );
  if ( x < User_Rect.x ) return ( FALSE );
  if ( y > User_Rect.y + User_Rect.h ) return ( FALSE );
  if ( y < User_Rect.y ) return ( FALSE );
  return ( TRUE );
}; // int CursorIsInUserRect( int x , int y )

/* ----------------------------------------------------------------------
 * This function checks if a given screen position lies within the user
 * i.e. combat rectangle or not.
 * ---------------------------------------------------------------------- */
int 
CursorIsInWeaponRect( int x , int y )
{
  point CurPos;
  CurPos.x = x ;
  CurPos.y = y ;
#define WEAPON_RECT_WIDTH 64
#define WEAPON_RECT_HEIGHT 64

  if ( ( CurPos.x >= 20 ) && ( CurPos.x <= 20 + WEAPON_RECT_WIDTH ) )
    {
      DebugPrintf( 0 , "\nMight be grabbing in weapon rectangle, as far as x is concerned.");
      if ( ( CurPos.y >= User_Rect.y + 10 ) && 
	   ( CurPos.y <= User_Rect.y + 10 + WEAPON_RECT_HEIGHT ) )
	{
	  DebugPrintf( 0 , "\nMight be grabbing in weapon rectangle, as far as y is concerned.");
	  return( TRUE );
	}
    }
  return( FALSE );
}; // int CursorIsInWeaponRect( int x , int y )

/* ----------------------------------------------------------------------
 * This function checks if a given screen position lies within the grid
 * where the inventory of the player is usually located or not.
 * ---------------------------------------------------------------------- */
int 
CursorIsInInventoryGrid( int x , int y )
{
  point CurPos;
  CurPos.x = x ;
  CurPos.y = y ;

  if ( ( CurPos.x >= 16 ) && ( CurPos.x <= 16 + INVENTORY_GRID_WIDTH * 32 ) )
    {
      DebugPrintf( 0 , "\nMight be grabbing in inventory, as far as x is concerned.");
      if ( ( CurPos.y >= User_Rect.y + 480 -16 - 64 - 32 * INVENTORY_GRID_HEIGHT ) && 
	   ( CurPos.y <= User_Rect.y + 480 - 64 -16 ) )
	{
	  DebugPrintf( 0 , "\nMight be grabbing in inventory, as far as y is concerned.");
	  return( TRUE );
	}
    }
  return( FALSE );
}; // int CursorIsInInventoryGrid( int x , int y )

int
GetInventorySquare_x( int x )
{
  return ( ( x - 16 ) / 32 );
}; // int GetInventorySquare_x( x )

int
GetInventorySquare_y( int y )
{
  return ( ( y - (User_Rect.y + 480 -16 - 64 - 32 * 6 ) ) / 32 );
}; // int GetInventorySquare_y( y )

/* ----------------------------------------------------------------------
 * This function gives the item type of the currently held item.  
 * ---------------------------------------------------------------------- */
int
GetHeldItemCode ( void )
{
  int InvPos;

  // --------------------
  // First we find out the inventory index of the item we want to
  // drop
  //
  for ( InvPos = 0 ; InvPos < MAX_ITEMS_IN_INVENTORY ; InvPos ++ )
    {
      if ( Me.Inventory[ InvPos ].currently_held_in_hand ) break;
    }
  if ( InvPos >=  MAX_ITEMS_IN_INVENTORY )
    {
      DebugPrintf( 0 , "\nNo item in inventory seems to be currently held in hand...");
      return ( -1 );
    }
  else
    {
      DebugPrintf( 0 , "\nInventory item index %d was held in hand." , InvPos );
    }
  
  return ( Me.Inventory[ InvPos ].type );
}; // int GetInventorySquare_y( y )

/* ----------------------------------------------------------------------
 * This function checks if a given item type could be dropped into the 
 * inventory grid at location x y.  Only the space is taken into account
 * and if other items block the way or not.
 * ---------------------------------------------------------------------- */
int 
ItemCanBeDroppedInInv ( int ItemType , int InvPos_x , int InvPos_y )
{
  int item_height;
  int item_width;

  //--------------------
  // Perhaps the item reaches even outside the inventory grid.  Then of course
  // it does not fit and we need/should not even test the details...
  //
  if ( ItemImageList[ ItemMap[ ItemType ].picture_number ].inv_size.x - 1 + InvPos_x >= 
       INVENTORY_GRID_WIDTH  ) return ( FALSE );
  if ( ItemImageList[ ItemMap[ ItemType ].picture_number ].inv_size.y - 1 + InvPos_y >= 
       INVENTORY_GRID_HEIGHT ) return ( FALSE );

  // --------------------
  // Now that we know, that the desired position is at least inside the inventory
  // grid, we can start to test for the details of the available inventory space
  //
  for ( item_height = 0 ; item_height < ItemImageList[ ItemMap[ ItemType ].picture_number ].inv_size.y ; item_height ++ )
    {
      for ( item_width = 0 ; item_width < ItemImageList[ ItemMap[ ItemType ].picture_number ].inv_size.x ; item_width ++ )
	{
	  if ( ! Inv_Pos_Is_Free( InvPos_x + item_width , InvPos_y + item_height ) ) return ( FALSE );
	}
    }
  return ( TRUE );

}; // int ItemCanBeDroppedInInv ( int ItemType , int InvPos_x , int InvPos_y )


  
void 
DropHeldItemToTheFloor ( void )
{
  // point CurPos;
  int InvPos;
  int i;

  // --------------------
  // First we find out the inventory index of the item we want to
  // drop
  //
  InvPos = GetHeldItemInventoryIndex(  );

  if ( InvPos ==  (-1) )
    {
      DebugPrintf( 0 , "\nvoid DropHeldItemToTheFloor: No item in inventory seems to be currently held in hand...");
      return;
    }

  // --------------------
  // Now we want to drop the item to the floor.
  // We therefore find a free position in the item list of this level
  // where we can add the item later.
  //
  for ( i = 0 ; i < MAX_ITEMS_PER_LEVEL ; i ++ )
    {
      if ( CurLevel->ItemList [ i ].type == ( -1 ) ) break;
    }
  if ( i >= MAX_ITEMS_PER_LEVEL )
    {
      DebugPrintf( 0 , "\n No free position to drop item!!! ");
      i = 0 ;
      Terminate( ERR );
    }

  // --------------------
  // Now we enter the item into the item list of this level
  //
  CurLevel->ItemList[ i ].type = Me.Inventory[ InvPos ].type;
  CurLevel->ItemList[ i ].pos.x = Me.pos.x;
  CurLevel->ItemList[ i ].pos.y = Me.pos.y;
  Me.Inventory[ InvPos ].type = ( -1 );
  Me.Inventory[ InvPos ].currently_held_in_hand = FALSE;

}; // void DropHeldItemToTheFloor ( void )

void
DropHeldItemToWeaponSlot ( void )
{
  int InvPos;

  InvPos = GetHeldItemInventoryIndex( );

  // Now the item is installed into the weapon slot of the influencer
  Druidmap[ DRUID001 ].weapon_item = Me.Inventory[ InvPos ].type;

  // Now the item is removed from inventory and no longer held in hand as well...
  Me.Inventory[ InvPos ].type = ( -1 );
  Me.Inventory[ InvPos ].currently_held_in_hand = FALSE;

}; // void DropHeldItemToWeaponSlot ( void )

void 
DropHeldItemToInventory( void )
{
  point CurPos;
  int InvPos;

  // --------------------
  // First we find out the inventory index of the item we want to
  // drop
  //
  InvPos = GetHeldItemInventoryIndex(  );

  if ( InvPos == (-1) )
    {
      DebugPrintf( 0 , "\nNo item in inventory seems to be currently held in hand...");
      return;
    }

  // --------------------
  // Now we want to drop the item to the right location again.
  // Therefore we need to find out the right position, which of course
  // depends as well on current mouse cursor location as well as the
  // size of the dropped item.
  //
  CurPos.x = GetMousePos_x() + 16 - ( 16 * ItemImageList[ ItemMap[ Me.Inventory[ InvPos ].type ].picture_number ].inv_size.x - 16 ) ;
  CurPos.y = GetMousePos_y() + 16 - ( 16 * ItemImageList[ ItemMap[ Me.Inventory[ InvPos ].type ].picture_number ].inv_size.y - 16 ) ;

  if ( ItemCanBeDroppedInInv ( Me.Inventory[ InvPos ].type , GetInventorySquare_x ( CurPos.x ) , 
			       GetInventorySquare_y ( CurPos.y ) ) )
    {
      Me.Inventory[ InvPos ].inventory_position.x = GetInventorySquare_x ( CurPos.x ) ;
      Me.Inventory[ InvPos ].inventory_position.y = GetInventorySquare_y ( CurPos.y ) ;
    }

  // --------------------
  // Now that we know the inventory index, we can as well make the item
  // 'not held in hand' immediately.
  //
  Me.Inventory[ InvPos ].currently_held_in_hand = FALSE ;

}; // void DropHeldItemToInventory( void )


#undef _items_c
