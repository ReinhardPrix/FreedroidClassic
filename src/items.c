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

#include "items.h"

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
      // DebugPrintf( 0 , "\nNo item in inventory seems to be currently held in hand...");
      InvPos = ( -1 ) ;
    }
  else
    {
      // DebugPrintf( 0 , "\nInventory item index %d was held in hand." , InvPos );
    }
  return ( InvPos );
}; // int GetHeldItemInventoryIndex( void )

/* ----------------------------------------------------------------------
 * This function generates a pointer to the (hopefully one and only) item
 * that is currently marked as held in hand.  If no such item can be 
 * found, the returned pointer will be NULL.
 * ---------------------------------------------------------------------- */
item* GetHeldItemPointer( void )
{
  int InvIndex;
  int i;

  // We must not access the levels item array, if the level was not yet
  // initialized!!! Or a SEGFAULT will occur!!!  Therefore we check for
  // ininitialized level first.!
  if ( CurLevel == NULL ) return ( NULL );

  InvIndex = GetHeldItemInventoryIndex(  );

  if ( InvIndex != (-1) )
    {
      DebugPrintf( 0 , "\nitem* GetHeldItemPointer( void ) : An item in inventory was held in hand.  Good.");
      return ( & ( Me.Inventory[ InvIndex ] ) );
    } 
  else if ( Druidmap[ Me.type ].weapon_item.currently_held_in_hand )
    {
      DebugPrintf( 0 , "\nitem* GetHeldItemPointer( void ) : An item in weapon slot was held in hand.  Good.");
      return ( & ( Druidmap[ Me.type ].weapon_item ) );
    }
  else if ( Druidmap[ Me.type ].drive_item.currently_held_in_hand )
    {
      DebugPrintf( 0 , "\nitem* GetHeldItemPointer( void ) : An item in weapon slot was held in hand.  Good.");
      return ( & ( Druidmap[ Me.type ].drive_item ) );
    }
  else if ( Druidmap[ Me.type ].shield_item.currently_held_in_hand )
    {
      DebugPrintf( 0 , "\nitem* GetHeldItemPointer( void ) : An item in weapon slot was held in hand.  Good.");
      return ( & ( Druidmap[ Me.type ].shield_item ) );
    }
  else if ( Druidmap[ Me.type ].armour_item.currently_held_in_hand )
    {
      DebugPrintf( 0 , "\nitem* GetHeldItemPointer( void ) : An item in weapon slot was held in hand.  Good.");
      return ( & ( Druidmap[ Me.type ].armour_item ) );
    }
  else if ( Druidmap[ Me.type ].special_item.currently_held_in_hand )
    {
      DebugPrintf( 0 , "\nitem* GetHeldItemPointer( void ) : An item in weapon slot was held in hand.  Good.");
      return ( & ( Druidmap[ Me.type ].special_item ) );
    }
  else if ( Druidmap[ Me.type ].aux1_item.currently_held_in_hand )
    {
      DebugPrintf( 0 , "\nitem* GetHeldItemPointer( void ) : An item in weapon slot was held in hand.  Good.");
      return ( & ( Druidmap[ Me.type ].aux1_item ) );
    }
  else if ( Druidmap[ Me.type ].aux2_item.currently_held_in_hand )
    {
      DebugPrintf( 0 , "\nitem* GetHeldItemPointer( void ) : An item in weapon slot was held in hand.  Good.");
      return ( & ( Druidmap[ Me.type ].aux2_item ) );
    }
  else
    {
      // --------------------
      // Not that we find that no item is held in hand in the entire inventory 
      // and all the slots, we go and look if one of the items on this levels
      // map is perhaps held in hand, but if that also fails, then no item at
      // all was held in hand.
      //
      for ( i = 0 ; i < MAX_ITEMS_PER_LEVEL ; i++ )
	{
	  if ( CurLevel->ItemList[ i ].type == (-1) ) continue;
	  if ( ! CurLevel->ItemList[ i ].currently_held_in_hand ) continue;
	  return ( & (CurLevel->ItemList[ i ] ) );
	}


      DebugPrintf( 0 , "\nitem* GetHeldItemPointer( void ) : NO ITEM AT ALL SEEMS TO HAVE BEEN HELD IN HAND!!");
      return ( NULL );
    }

  
}; // item* GetHeldItemPointer( void )

/* ----------------------------------------------------------------------
 * This function DELETES an item from the source location.
 * ---------------------------------------------------------------------- */
void
DeleteItem( item* Item )
{

  Item->type = -1 ;
  Item->currently_held_in_hand = FALSE;

}; // void DeleteItem( item* Item )

/* ----------------------------------------------------------------------
 * This function COPIES an item from the source location to the destination
 * location.  The source location is then marked as unused inventory 
 * entry.
 * ---------------------------------------------------------------------- */
void
CopyItem( item* SourceItem , item* DestItem )
{

  memcpy ( DestItem, SourceItem, sizeof ( item ));

}; // void MoveItem( item* SourceItem , item* DestItem )

/* ----------------------------------------------------------------------
 * This function MOVES an item from the source location to the destination
 * location.  The source location is then marked as unused inventory 
 * entry.
 * ---------------------------------------------------------------------- */
void
MoveItem( item* SourceItem , item* DestItem )
{

  memcpy ( DestItem, SourceItem, sizeof ( item ));
  
  SourceItem->type = (-1) ;

}; // void MoveItem( item* SourceItem , item* DestItem )

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
 * 
 * NOTE: The mentioned coordinates refer to the squares of the inventory grid!!
 *
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
 * This function checks if a given screen position lies within the small
 * rectangle defining the weapon slot in the inventory screen.
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
 * This function checks if a given screen position lies within the small
 * rectangle defining the drive slot in the inventory screen.
 * ---------------------------------------------------------------------- */
int 
CursorIsInDriveRect( int x , int y )
{
  point CurPos;
  CurPos.x = x ;
  CurPos.y = y ;
#define DRIVE_RECT_WIDTH 64
#define DRIVE_RECT_HEIGHT 64

  if ( ( CurPos.x >= 240 ) && ( CurPos.x <= 240 + DRIVE_RECT_WIDTH ) )
    {
      DebugPrintf( 0 , "\nMight be grabbing in drive rectangle, as far as x is concerned.");
      if ( ( CurPos.y >= User_Rect.y + 93 ) && 
	   ( CurPos.y <= User_Rect.y + 93 + DRIVE_RECT_HEIGHT ) )
	{
	  DebugPrintf( 0 , "\nMight be grabbing in drive rectangle, as far as y is concerned.");
	  return( TRUE );
	}
    }
  return( FALSE );
}; // int CursorIsInDriveRect( int x , int y )

/* ----------------------------------------------------------------------
 * This function checks if a given screen position lies within the small
 * rectangle defining the armour slot in the inventory screen.
 * ---------------------------------------------------------------------- */
int 
CursorIsInArmourRect( int x , int y )
{
  point CurPos;
  CurPos.x = x ;
  CurPos.y = y ;

  if ( ( CurPos.x >= ARMOUR_POS_X ) && ( CurPos.x <= ARMOUR_POS_X + ARMOUR_RECT_WIDTH ) )
    {
      DebugPrintf( 0 , "\nMight be grabbing in armour rectangle, as far as x is concerned.");
      if ( ( CurPos.y >= User_Rect.y + ARMOUR_POS_Y ) && 
	   ( CurPos.y <= User_Rect.y + ARMOUR_POS_Y + ARMOUR_RECT_HEIGHT ) )
	{
	  DebugPrintf( 0 , "\nMight be grabbing in armour rectangle, as far as y is concerned.");
	  return( TRUE );
	}
    }
  return( FALSE );
}; // int CursorIsInArmourRect( int x , int y )

/* ----------------------------------------------------------------------
 * This function checks if a given screen position lies within the small
 * rectangle defining the shield slot in the inventory screen.
 * ---------------------------------------------------------------------- */
int 
CursorIsInShieldRect( int x , int y )
{
  point CurPos;
  CurPos.x = x ;
  CurPos.y = y ;

  if ( ( CurPos.x >= SHIELD_POS_X ) && ( CurPos.x <= SHIELD_POS_X + SHIELD_RECT_WIDTH ) )
    {
      DebugPrintf( 0 , "\nMight be grabbing in armour rectangle, as far as x is concerned.");
      if ( ( CurPos.y >= User_Rect.y + SHIELD_POS_Y ) && 
	   ( CurPos.y <= User_Rect.y + SHIELD_POS_Y + SHIELD_RECT_HEIGHT ) )
	{
	  DebugPrintf( 0 , "\nMight be grabbing in armour rectangle, as far as y is concerned.");
	  return( TRUE );
	}
    }
  return( FALSE );
}; // int CursorIsInShieldRect( int x , int y )

/* ----------------------------------------------------------------------
 * This function checks if a given screen position lies within the small
 * rectangle defining the armour slot in the inventory screen.
 * ---------------------------------------------------------------------- */
int 
CursorIsInSpecialRect( int x , int y )
{
  point CurPos;
  CurPos.x = x ;
  CurPos.y = y ;

  if ( ( CurPos.x >= SPECIAL_POS_X ) && ( CurPos.x <= SPECIAL_POS_X + SPECIAL_RECT_WIDTH ) )
    {
      DebugPrintf( 0 , "\nMight be grabbing in armour rectangle, as far as x is concerned.");
      if ( ( CurPos.y >= User_Rect.y + SPECIAL_POS_Y ) && 
	   ( CurPos.y <= User_Rect.y + SPECIAL_POS_Y + SPECIAL_RECT_HEIGHT ) )
	{
	  DebugPrintf( 0 , "\nMight be grabbing in armour rectangle, as far as y is concerned.");
	  return( TRUE );
	}
    }
  return( FALSE );
}; // int CursorIsInSpecialRect( int x , int y )

/* ----------------------------------------------------------------------
 * This function checks if a given screen position lies within the small
 * rectangle defining the aux1 slot in the inventory screen.
 * ---------------------------------------------------------------------- */
int 
CursorIsInAux1Rect( int x , int y )
{
  point CurPos;
  CurPos.x = x ;
  CurPos.y = y ;

  if ( ( CurPos.x >= AUX1_POS_X ) && ( CurPos.x <= AUX1_POS_X + AUX1_RECT_WIDTH ) )
    {
      DebugPrintf( 0 , "\nMight be grabbing in aux1 rectangle, as far as x is concerned.");
      if ( ( CurPos.y >= User_Rect.y + AUX1_POS_Y ) && 
	   ( CurPos.y <= User_Rect.y + AUX1_POS_Y + AUX1_RECT_HEIGHT ) )
	{
	  DebugPrintf( 0 , "\nMight be grabbing in aux1 rectangle, as far as y is concerned.");
	  return( TRUE );
	}
    }
  return( FALSE );
}; // int CursorIsInAux1Rect( int x , int y )

/* ----------------------------------------------------------------------
 * This function checks if a given screen position lies within the small
 * rectangle defining the aux2 slot in the inventory screen.
 * ---------------------------------------------------------------------- */
int 
CursorIsInAux2Rect( int x , int y )
{
  point CurPos;
  CurPos.x = x ;
  CurPos.y = y ;

  if ( ( CurPos.x >= AUX2_POS_X ) && ( CurPos.x <= AUX2_POS_X + AUX2_RECT_WIDTH ) )
    {
      DebugPrintf( 0 , "\nMight be grabbing in aux1 rectangle, as far as x is concerned.");
      if ( ( CurPos.y >= User_Rect.y + AUX2_POS_Y ) && 
	   ( CurPos.y <= User_Rect.y + AUX2_POS_Y + AUX2_RECT_HEIGHT ) )
	{
	  DebugPrintf( 0 , "\nMight be grabbing in aux1 rectangle, as far as y is concerned.");
	  return( TRUE );
	}
    }
  return( FALSE );
}; // int CursorIsInAux2Rect( int x , int y )

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
      // DebugPrintf( 0 , "\nMight be grabbing in inventory, as far as x is concerned.");
      if ( ( CurPos.y >= User_Rect.y + 480 -16 - 64 - 32 * INVENTORY_GRID_HEIGHT ) && 
	   ( CurPos.y <= User_Rect.y + 480 - 64 -16 ) )
	{
	  // DebugPrintf( 0 , "\nMight be grabbing in inventory, as far as y is concerned.");
	  return( TRUE );
	}
    }
  return( FALSE );
}; // int CursorIsInInventoryGrid( int x , int y )

/* ----------------------------------------------------------------------
 * This function gives the x coordinate of the inventory square that 
 * corresponds to the mouse cursor location given to the function.
 * ---------------------------------------------------------------------- */
int
GetInventorySquare_x( int x )
{
  return ( ( x - 16 ) / 32 );
}; // int GetInventorySquare_x( x )

/* ----------------------------------------------------------------------
 * This function gives the y coordinate of the inventory square that 
 * corresponds to the mouse cursor location given to the function.
 * ---------------------------------------------------------------------- */
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
  item* ItemPointer;

  ItemPointer = GetHeldItemPointer( );

  if ( ItemPointer != NULL )
    {
      return ( ItemPointer->type );
    }

  //--------------------
  // If we ever reach this point, that means that the held items code
  // could not be correctly computed, which should mean a reason to
  // terminate immediately with severe error
  //

  DebugPrintf( 0 , "\nint GetHeldItemCode ( void ):  COULDN't FIND HELD ITEM!! " );
  // Terminate( ERR );
  return ( -1 );
  

  /*
  int InvPos;

  // --------------------
  // First we find out the inventory index of the item we want to
  // drop
  //
  for ( InvPos = 0 ; InvPos < MAX_ITEMS_IN_INVENTORY ; InvPos ++ )
    {
      if ( Me.Inventory[ InvPos ].currently_held_in_hand ) 
	{
	  return ( Me.Inventory[ InvPos ].type );
	}
    }

  if ( Druidmap[ Me.type ].weapon_item.currently_held_in_hand ) 
    {
      return ( Druidmap[ Me.type ].weapon_item.type );
    }

  if ( Druidmap[ Me.type ].drive_item.currently_held_in_hand ) 
    {
      return ( Druidmap[ Me.type ].drive_item.type );
    }

  if ( Druidmap[ Me.type ].armour_item.currently_held_in_hand ) 
    {
      return ( Druidmap[ Me.type ].armour_item.type );
    }

  if ( Druidmap[ Me.type ].shield_item.currently_held_in_hand ) 
    {
      return ( Druidmap[ Me.type ].shield_item.type );
    }

  if ( Druidmap[ Me.type ].special_item.currently_held_in_hand ) 
    {
      return ( Druidmap[ Me.type ].special_item.type );
    }

  if ( Druidmap[ Me.type ].aux1_item.currently_held_in_hand ) 
    {
      return ( Druidmap[ Me.type ].aux1_item.type );
    }

  if ( Druidmap[ Me.type ].aux2_item.currently_held_in_hand ) 
    {
      return ( Druidmap[ Me.type ].aux2_item.type );
    }
  */


}; // int GetHeldItemCode ( void )

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
  item* DropItemPointer;
  int i;

  // --------------------
  // First we find out which item we want to drop onto the floor
  //
  DropItemPointer = GetHeldItemPointer(  );
  if ( DropItemPointer == NULL )
    {
      DebugPrintf( 0 , "\nvoid DropHeldItemToTheFloor ( void ) : No item in inventory seems to be currently held in hand...");
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
  CopyItem( DropItemPointer , &(CurLevel->ItemList[ i ]) );
  CurLevel->ItemList[ i ].pos.x = Me.pos.x;
  CurLevel->ItemList[ i ].pos.y = Me.pos.y;
  CurLevel->ItemList[ i ].currently_held_in_hand = FALSE;
  // CurLevel->ItemList[ i ].pos.x = Me.pos.x;
  // CurLevel->ItemList[ i ].pos.y = Me.pos.y;
  // CurLevel->ItemList[ i ].type = Me.Inventory[ InvPos ].type;
  
  // Me.Inventory[ InvPos ].type = ( -1 );
  DeleteItem( DropItemPointer );

}; // void DropHeldItemToTheFloor ( void )

void
DropHeldItemToWeaponSlot ( void )
{
  item* DropItemPointer;

  // --------------------
  // First we find out which item we want to drop into the weapon slot
  //
  DropItemPointer = GetHeldItemPointer(  );
  if ( DropItemPointer == NULL )
    {
      DebugPrintf( 0 , "\nvoid DropHeldItemToWeaponSlot ( void ) : No item in inventory seems to be currently held in hand...");
      return;
    } 
  
  // Now the item is installed into the weapon slot of the influencer
  
  // Druidmap[ DRUID001 ].weapon_item = Me.Inventory[ InvPos ].type;
  CopyItem( DropItemPointer , &(Druidmap[ DRUID001 ].weapon_item) );
  Druidmap[ DRUID001 ].weapon_item.currently_held_in_hand = FALSE;

  // Now the item is removed from the source location and no longer held in hand as well, 
  // but of course only if it is not the same as the original item
  if ( DropItemPointer != &(Druidmap[ DRUID001 ].weapon_item) )
    DeleteItem( DropItemPointer );

}; // void DropHeldItemToWeaponSlot ( void )

void
DropHeldItemToDriveSlot ( void )
{
  item* DropItemPointer;

  // --------------------
  // First we find out which item we want to drop into the weapon slot
  //
  DropItemPointer = GetHeldItemPointer(  );
  if ( DropItemPointer == NULL )
    {
      DebugPrintf( 0 , "\nvoid DropHeldItemToWeaponSlot ( void ) : No item in inventory seems to be currently held in hand...");
      return;
    } 

  // Now the item is installed into the drive slot of the influencer
  CopyItem( DropItemPointer , &(Druidmap[ DRUID001 ].drive_item) );
  Druidmap[ DRUID001 ].drive_item.currently_held_in_hand = FALSE;

  // Now the item is removed from the source location and no longer held in hand as well, 
  // but of course only if it is not the same as the original item
  if ( DropItemPointer != &(Druidmap[ DRUID001 ].drive_item) )
    DeleteItem( DropItemPointer );

}; // void DropHeldItemToDriveSlot ( void )

void
DropHeldItemToArmourSlot ( void )
{
  item* DropItemPointer;

  // --------------------
  // First we find out which item we want to drop into the weapon slot
  //
  DropItemPointer = GetHeldItemPointer(  );
  if ( DropItemPointer == NULL )
    {
      DebugPrintf( 0 , "\nvoid DropHeldItemToWeaponSlot ( void ) : No item in inventory seems to be currently held in hand...");
      return;
    } 

  // Now the item is installed into the weapon slot of the influencer
  // Druidmap[ DRUID001 ].armour_item = Me.Inventory[ InvPos ].type;
  CopyItem ( DropItemPointer , &(Druidmap[ DRUID001 ].armour_item) );
  Druidmap[ DRUID001 ].armour_item.currently_held_in_hand = FALSE;

  // Now the item is removed from the source location and no longer held in hand as well, 
  // but of course only if it is not the same as the original item
  if ( DropItemPointer != &(Druidmap[ DRUID001 ].armour_item) )
    DeleteItem( DropItemPointer );

}; // void DropHeldItemToArmourSlot ( void )

void
DropHeldItemToShieldSlot ( void )
{
  item* DropItemPointer;

  // --------------------
  // First we find out which item we want to drop into the weapon slot
  //
  DropItemPointer = GetHeldItemPointer(  );
  if ( DropItemPointer == NULL )
    {
      DebugPrintf( 0 , "\nvoid DropHeldItemToWeaponSlot ( void ) : No item in inventory seems to be currently held in hand...");
      return;
    } 

  // Now the item is installed into the weapon slot of the influencer
  CopyItem ( DropItemPointer , &(Druidmap[ DRUID001 ].shield_item) );
  Druidmap[ DRUID001 ].shield_item.currently_held_in_hand = FALSE;

  // Now the item is removed from the source location and no longer held in hand as well, 
  // but of course only if it is not the same as the original item
  if ( DropItemPointer != &(Druidmap[ DRUID001 ].shield_item) )
    DeleteItem( DropItemPointer );

}; // void DropHeldItemToShieldSlot ( void )

void
DropHeldItemToSpecialSlot ( void )
{
  item* DropItemPointer;

  // --------------------
  // First we find out which item we want to drop into the weapon slot
  //
  DropItemPointer = GetHeldItemPointer(  );
  if ( DropItemPointer == NULL )
    {
      DebugPrintf( 0 , "\nvoid DropHeldItemToWeaponSlot ( void ) : No item in inventory seems to be currently held in hand...");
      return;
    } 

  // Now the item is installed into the weapon slot of the influencer
  CopyItem( DropItemPointer , &( Druidmap[ DRUID001 ].special_item) );
  Druidmap[ DRUID001 ].special_item.currently_held_in_hand = FALSE;

  // Now the item is removed from the source location and no longer held in hand as well, 
  // but of course only if it is not the same as the original item
  if ( DropItemPointer != &(Druidmap[ DRUID001 ].special_item) )
    DeleteItem( DropItemPointer );

}; // void DropHeldItemToShieldSlot ( void )

void
DropHeldItemToAux1Slot ( void )
{
  item* DropItemPointer;

  // --------------------
  // First we find out which item we want to drop into the weapon slot
  //
  DropItemPointer = GetHeldItemPointer(  );
  if ( DropItemPointer == NULL )
    {
      DebugPrintf( 0 , "\nvoid DropHeldItemToWeaponSlot ( void ) : No item in inventory seems to be currently held in hand...");
      return;
    } 

  // Now the item is installed into the weapon slot of the influencer
  CopyItem( DropItemPointer , &( Druidmap[ DRUID001 ].aux1_item) );
  Druidmap[ DRUID001 ].aux1_item.currently_held_in_hand = FALSE;

  // Now the item is removed from the source location and no longer held in hand as well, 
  // but of course only if it is not the same as the original item
  if ( DropItemPointer != &(Druidmap[ DRUID001 ].aux1_item) )
    DeleteItem( DropItemPointer );

}; // void DropHeldItemToAux1Slot ( void )

void
DropHeldItemToAux2Slot ( void )
{
  item* DropItemPointer;

  // --------------------
  // First we find out which item we want to drop into the weapon slot
  //
  DropItemPointer = GetHeldItemPointer(  );
  if ( DropItemPointer == NULL )
    {
      DebugPrintf( 0 , "\nvoid DropHeldItemToWeaponSlot ( void ) : No item in inventory seems to be currently held in hand...");
      return;
    } 

  // Now the item is installed into the weapon slot of the influencer
  CopyItem( DropItemPointer , &( Druidmap[ DRUID001 ].aux2_item) );
  Druidmap[ DRUID001 ].aux2_item.currently_held_in_hand = FALSE;

  // Now the item is removed from the source location and no longer held in hand as well, 
  // but of course only if it is not the same as the original item
  if ( DropItemPointer != &(Druidmap[ DRUID001 ].aux2_item) )
    DeleteItem( DropItemPointer );

}; // void DropHeldItemToAux2Slot ( void )

/* ----------------------------------------------------------------------
 * This function looks for a free inventory index.  Since there are more
 * inventory indices than squares in the inventory grid, the function 
 * should always be able to find a free invenotry index.  If not, this is
 * considered a severe program error, which will cause immediate 
 * termination of FreeDroid.
 * ---------------------------------------------------------------------- */
int 
GetFreeInventoryIndex( void )
{
  int InvPos;

  // --------------------
  // We find out the first free inventory index:
  //
  for ( InvPos = 0 ; InvPos < MAX_ITEMS_IN_INVENTORY ; InvPos ++ )
    {
      if ( Me.Inventory[ InvPos ].type == (-1) ) 
	{
	  return ( InvPos );
	}
    }

  // --------------------
  // If this point is reached, the severe error mentioned above has
  // occured, an error message must be printed out and the program
  // must be terminated.
  //
  fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a severe problem:\n\
A FREE INVENTORY INDEX POSITION COULD NOT BE FOUND.\n\
\n\
This is an internal error, that must never happen unless there are\n\
severe bugs in the inventory system.\n\
\n\
If you receive this error, PLEASE INFORM THE FREEDROID DEVELOPERS ABOUT IT!!\n\
Thanks a lot!!!\n\
\n\
For now Freedroid will terminate to draw attention \n\
to the internal problem it could not resolve.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" );
  Terminate(ERR);

  // to make compilers happy...
  return ( 0 ); 
}; // int GetFreeInventoryIndex( void )

/* ----------------------------------------------------------------------
 * If an item is held and then clicked again in the inventory field, this
 * item should be dropped into the inventory field, provided there is room
 * enough in it at that location.  If that is the case, then the item is
 * dropped onto this inventory location, else nothing is done.
 * ---------------------------------------------------------------------- */
void 
DropHeldItemToInventory( void )
{
  point CurPos;
  item* DropItemPointer;
  int FreeInvIndex;

  FreeInvIndex = GetFreeInventoryIndex( );

  // --------------------
  // First we find out which item we want to drop into the inventory
  //
  DropItemPointer = GetHeldItemPointer(  );
  if ( DropItemPointer == NULL )
    {
      DebugPrintf( 0 , "\nvoid DropHeldItemToInventory ( void ) : No item in inventory seems to be currently held in hand...");
      return;
    } 
  

  // --------------------
  // Now we want to drop the item to the right location again.
  // Therefore we need to find out the right position, which of course
  // depends as well on current mouse cursor location as well as the
  // size of the dropped item.
  //
  CurPos.x = GetMousePos_x() + 16 - ( 16 * ItemImageList[ ItemMap[ DropItemPointer->type ].picture_number ].inv_size.x - 16 ) ;
  CurPos.y = GetMousePos_y() + 16 - ( 16 * ItemImageList[ ItemMap[ DropItemPointer->type ].picture_number ].inv_size.y - 16 ) ;

  if ( ItemCanBeDroppedInInv ( DropItemPointer->type , GetInventorySquare_x ( CurPos.x ) , 
			       GetInventorySquare_y ( CurPos.y ) ) )
    {
      CopyItem( DropItemPointer , &( Me.Inventory[ FreeInvIndex ] ) );
      DeleteItem( DropItemPointer );
      Me.Inventory[ FreeInvIndex ].inventory_position.x = GetInventorySquare_x ( CurPos.x ) ;
      Me.Inventory[ FreeInvIndex ].inventory_position.y = GetInventorySquare_y ( CurPos.y ) ;
      Me.Inventory[ FreeInvIndex ].currently_held_in_hand = FALSE;
    }

  // --------------------
  // Now that we know the inventory index, we can as well make the item
  // 'not held in hand' immediately.
  //
  DropItemPointer->currently_held_in_hand = FALSE ;

}; // void DropHeldItemToInventory( void )


/* ----------------------------------------------------------------------
 * This function display the inventory screen and also checks for mouse
 * actions in the invenotry screen.
 * ---------------------------------------------------------------------- */
void 
ManageInventoryScreen ( void )
{
  SDL_Rect TargetRect;
  static int Item_Held_In_Hand = -1 ;
  static int MouseButtonPressedPreviousFrame = FALSE;
  point CurPos;
  point Inv_GrabLoc;
  int Grabbed_InvPos;
  int i;
  finepoint MapPositionOfMouse;
  // static int Item_Grabbed = FALSE;

  DebugPrintf (2, "\nvoid ShowInventoryMessages( ... ): Function call confirmed.");

  // --------------------
  // We will need the current mouse position on several spots...
  //
  CurPos.x = GetMousePos_x() + 16;
  CurPos.y = GetMousePos_y() + 16;

  //--------------------
  // If the log is not set to visible right now, we do not need to 
  // do anything more, but to restore the usual user rectangle size
  // back to normal and to return...
  //
  if ( GameConfig.Inventory_Visible == FALSE ) return;

  //--------------------
  // Next we display all the inventory screen and we also fill in all
  // the pictures for the items the influencer is currently fitted with.
  //
  ShowInventoryScreen();

  //--------------------
  // If the user now presses the mouse button and it was not pressed before,
  // the the user has 'grabbed' the item directly under the mouse button
  //
  if ( ( axis_is_active ) && ( !MouseButtonPressedPreviousFrame ) && ( Item_Held_In_Hand == (-1) ) )
    {
      DebugPrintf( 0 , "\nTrying to 'grab' the item below the mouse cursor.");
      
      if ( CursorIsInInventoryGrid( CurPos.x , CurPos.y ) )
	{
	  Inv_GrabLoc.x = GetInventorySquare_x ( CurPos.x );
	  Inv_GrabLoc.y = GetInventorySquare_y ( CurPos.y );

	  DebugPrintf( 0 , "\nGrabbing at inv-pos: %d %d." , Inv_GrabLoc.x , Inv_GrabLoc.y );
	      
	  Grabbed_InvPos = GetInventoryItemAt ( Inv_GrabLoc.x , Inv_GrabLoc.y );
	  DebugPrintf( 0 , "\nGrabbing inventory entry no.: %d." , Grabbed_InvPos );

	  if ( Grabbed_InvPos == (-1) )
	    {
	      // Nothing grabbed, so we need not do anything more here..
	      Item_Held_In_Hand = ( -1 );
	      DebugPrintf( 0 , "\nGrabbing in INVENTORY grid FAILED!" );
	    }
	  else
	    {
	      //--------------------
	      // At this point we know, that we have just grabbed something from the inventory
	      // So we set, that something should be displayed in the 'hand', and it should of
	      // course be the image of the item grabbed from inventory.
	      //
	      Item_Held_In_Hand = ItemMap[ Me.Inventory[ Grabbed_InvPos ].type ].picture_number ;
	      PlayItemSound( ItemMap[ Me.Inventory[ Grabbed_InvPos ].type ].sound_number );
	      Me.Inventory[ Grabbed_InvPos ].currently_held_in_hand = TRUE;
	    }
	}
      else if ( CursorIsInWeaponRect( CurPos.x , CurPos.y ) )
	{
	  DebugPrintf( 0 , "\nGrabbing in weapons rect!" );
	  if ( Druidmap[ Me.type ].weapon_item.type > 0 )
	    {
	      //--------------------
	      // At this point we know, that we have just grabbed something from the weapon rect
	      // So we set, that something should be displayed in the 'hand', and it should of
	      // course be the image of the item grabbed from inventory.
	      //
	      Item_Held_In_Hand = ItemMap[ Druidmap [ Me.type ].weapon_item.type ].picture_number ;
	      Druidmap[ Me.type ].weapon_item.currently_held_in_hand = TRUE;
	    }
	}
      else if ( CursorIsInDriveRect( CurPos.x , CurPos.y ) )
	{
	  DebugPrintf( 0 , "\nGrabbing in drive rect!" );
	  if ( Druidmap[ Me.type ].drive_item.type > 0 )
	    {
	      //--------------------
	      // At this point we know, that we have just grabbed something from the weapon rect
	      // So we set, that something should be displayed in the 'hand', and it should of
	      // course be the image of the item grabbed from inventory.
	      //
	      Item_Held_In_Hand = ItemMap[ Druidmap [ Me.type ].drive_item.type ].picture_number ;
	      Druidmap[ Me.type ].drive_item.currently_held_in_hand = TRUE;
	    }
	}
      else if ( CursorIsInShieldRect( CurPos.x , CurPos.y ) )
	{
	  DebugPrintf( 0 , "\nGrabbing in shield rect!" );
	  if ( Druidmap[ Me.type ].shield_item.type > 0 )
	    {
	      //--------------------
	      // At this point we know, that we have just grabbed something from the weapon rect
	      // So we set, that something should be displayed in the 'hand', and it should of
	      // course be the image of the item grabbed from inventory.
	      //
	      Item_Held_In_Hand = ItemMap[ Druidmap [ Me.type ].shield_item.type ].picture_number ;
	      Druidmap[ Me.type ].shield_item.currently_held_in_hand = TRUE;
	    }
	}
      else if ( CursorIsInArmourRect( CurPos.x , CurPos.y ) )
	{
	  DebugPrintf( 0 , "\nGrabbing in armour rect!" );
	  if ( Druidmap[ Me.type ].armour_item.type > 0 )
	    {
	      //--------------------
	      // At this point we know, that we have just grabbed something from the weapon rect
	      // So we set, that something should be displayed in the 'hand', and it should of
	      // course be the image of the item grabbed from inventory.
	      //
	      Item_Held_In_Hand = ItemMap[ Druidmap [ Me.type ].armour_item.type ].picture_number ;
	      Druidmap[ Me.type ].armour_item.currently_held_in_hand = TRUE;
	    }
	}
      else if ( CursorIsInSpecialRect( CurPos.x , CurPos.y ) )
	{
	  DebugPrintf( 0 , "\nGrabbing in special rect!" );
	  if ( Druidmap[ Me.type ].special_item.type > 0 )
	    {
	      //--------------------
	      // At this point we know, that we have just grabbed something from the weapon rect
	      // So we set, that something should be displayed in the 'hand', and it should of
	      // course be the image of the item grabbed from inventory.
	      //
	      Item_Held_In_Hand = ItemMap[ Druidmap [ Me.type ].special_item.type ].picture_number ;
	      Druidmap[ Me.type ].special_item.currently_held_in_hand = TRUE;
	    }
	}
      else if ( CursorIsInAux1Rect( CurPos.x , CurPos.y ) )
	{
	  DebugPrintf( 0 , "\nGrabbing in aux1 rect!" );
	  if ( Druidmap[ Me.type ].aux1_item.type > 0 )
	    {
	      //--------------------
	      // At this point we know, that we have just grabbed something from the weapon rect
	      // So we set, that something should be displayed in the 'hand', and it should of
	      // course be the image of the item grabbed from inventory.
	      //
	      Item_Held_In_Hand = ItemMap[ Druidmap [ Me.type ].aux1_item.type ].picture_number ;
	      Druidmap[ Me.type ].aux1_item.currently_held_in_hand = TRUE;
	    }
	}
      else if ( CursorIsInAux2Rect( CurPos.x , CurPos.y ) )
	{
	  DebugPrintf( 0 , "\nGrabbing in aux1 rect!" );
	  if ( Druidmap[ Me.type ].aux2_item.type > 0 )
	    {
	      //--------------------
	      // At this point we know, that we have just grabbed something from the weapon rect
	      // So we set, that something should be displayed in the 'hand', and it should of
	      // course be the image of the item grabbed from inventory.
	      //
	      Item_Held_In_Hand = ItemMap[ Druidmap [ Me.type ].aux2_item.type ].picture_number ;
	      Druidmap[ Me.type ].aux2_item.currently_held_in_hand = TRUE;
	    }
	}
      else if ( CursorIsInUserRect( CurPos.x , CurPos.y ) )
	{
	  DebugPrintf( 0 , "\nGrabbing in user rect!" );
	  MapPositionOfMouse.x = Me.pos.x + (CurPos.x - UserCenter_x) / (float) Block_Width;
	  MapPositionOfMouse.y = Me.pos.y + (CurPos.y - UserCenter_y) / (float) Block_Height;
	  DebugPrintf( 0  , "\nMouse in map at: %f %f." , MapPositionOfMouse.x , MapPositionOfMouse.y );
	  for ( i = 0 ; i < MAX_ITEMS_PER_LEVEL ; i++ )
	    {
	      if ( CurLevel->ItemList[ i ].type == (-1) ) continue;
	      
	      if ( ( fabsf( MapPositionOfMouse.x - CurLevel->ItemList[ i ].pos.x ) < 0.5 ) &&
		   ( fabsf( MapPositionOfMouse.y - CurLevel->ItemList[ i ].pos.y ) < 0.5 ) )
		{
		  // We've found some item to grab!!! How wonderful!!!
		  Item_Held_In_Hand = ItemMap[ CurLevel->ItemList[ i ].type ].picture_number ;
		  CurLevel->ItemList[ i ].currently_held_in_hand = TRUE;
		  break;
		}
	    }
	}
      else
	{
	  // Nothing grabbed, so we need not do anything more here..
	  Item_Held_In_Hand = ( -1 );
	  DebugPrintf( 0 , "\nGrabbing UTTERLY FAILED!" );
	}
      goto NoMoreGrabbing;
    }
  
  //--------------------
  // Now the OTHER CASE:  If the user now no longer presses the mouse button and it WAS pressed before,
  // the the user has 'released' the item directly under the mouse button
  //
  if ( ( axis_is_active ) && ( !MouseButtonPressedPreviousFrame ) && ( Item_Held_In_Hand != (-1) ) )
    //if ( ( !axis_is_active ) && ( MouseButtonPressedPreviousFrame ) )
    {
      //--------------------
      // In case the user didn't hold anything in his hand, then nothing
      // needs to be released as well...
      //
      if ( Item_Held_In_Hand == ( -1 ) ) goto NoMoreReleasing;

      //--------------------
      // If the cursor is in the inventory window again, then we must see if 
      // the item was dropped onto a correct inventory location and should from
      // then on not only no longer be in the players hand but also remain at
      // the newly assigned position.
      //
      if ( CursorIsInInventoryGrid( CurPos.x , CurPos.y ) )
	{
	  DebugPrintf( 0 , "\nItem dropped in inventory window!" );
	  DropHeldItemToInventory( );
	  Item_Held_In_Hand = ( -1 );
	}

      //--------------------
      // If the cursor is in the user_rect, i.e. the combat window, then
      // the item should be dropped onto the players current location
      //
      if ( CursorIsInUserRect ( CurPos.x , CurPos.y ) )
	{
	  DebugPrintf( 0 , "\nItem dropped onto the floor of the combat window!" );
	  DropHeldItemToTheFloor( );
	  Item_Held_In_Hand = ( -1 );
	}

      //--------------------
      // If the cursor is in the weapons rect, i.e. the small box top left, then
      // the item should be dropped onto the players current weapon slot
      //
      if ( CursorIsInWeaponRect ( CurPos.x , CurPos.y ) )
	{
	  DebugPrintf( 0 , "\nItem dropped onto the weapons rectangle!" );
	  DebugPrintf( 0 , "\nGetHeldItemCode: %d." , GetHeldItemCode() );
	  if ( ( GetHeldItemCode() != (-1) ) &&
	       ( ItemMap[ GetHeldItemCode() ].item_can_be_installed_in_weapon_slot ) )
	    {
	      DropHeldItemToWeaponSlot ( );
	      Item_Held_In_Hand = ( -1 );
	    }
	  else
	    {
	      // If the item can't be used as a weapon, we don't do anything
	    }
	}

      //--------------------
      // If the cursor is in the drive rect, i.e. the small box to the right, then
      // the item should be dropped onto the players current weapon slot
      //
      if ( CursorIsInDriveRect ( CurPos.x , CurPos.y ) )
	{
	  DebugPrintf( 0 , "\nItem dropped onto the drive rectangle!" );
	  DebugPrintf( 0 , "\nGetHeldItemCode: %d." , GetHeldItemCode() );
	  if ( ( GetHeldItemCode() != (-1) ) &&
	       ( ItemMap[ GetHeldItemCode() ].item_can_be_installed_in_drive_slot ) )
	    {
	      DropHeldItemToDriveSlot ( );
	      Item_Held_In_Hand = ( -1 );
	    }
	  else
	    {
	      // If the item can't be used as a weapon, we don't do anything
	    }
	}

      //--------------------
      // If the cursor is in the armour rect, then
      // the item should be dropped onto the players current weapon slot
      //
      if ( CursorIsInArmourRect ( CurPos.x , CurPos.y ) )
	{
	  DebugPrintf( 0 , "\nItem dropped onto the armour rectangle!" );
	  DebugPrintf( 0 , "\nGetHeldItemCode: %d." , GetHeldItemCode() );
	  if ( ( GetHeldItemCode() != (-1) ) &&
	       ( ItemMap[ GetHeldItemCode() ].item_can_be_installed_in_armour_slot ) )
	    {
	      DropHeldItemToArmourSlot ( );
	      Item_Held_In_Hand = ( -1 );
	    }
	  else
	    {
	      // If the item can't be used as a weapon, we don't do anything
	    }
	}

      //--------------------
      // If the cursor is in the shield rect, i.e. the small box to the top right, then
      // the item should be dropped onto the players current weapon slot
      //
      if ( CursorIsInShieldRect ( CurPos.x , CurPos.y ) )
	{
	  DebugPrintf( 0 , "\nItem dropped onto the shield rectangle!" );
	  DebugPrintf( 0 , "\nGetHeldItemCode: %d." , GetHeldItemCode() );
	  if ( ( GetHeldItemCode() != (-1) ) &&
	       ( ItemMap[ GetHeldItemCode() ].item_can_be_installed_in_shield_slot ) )
	    {
	      DropHeldItemToShieldSlot ( );
	      Item_Held_In_Hand = ( -1 );
	    }
	  else
	    {
	      // If the item can't be used as a weapon, we don't do anything
	    }
	}

      //--------------------
      // If the cursor is in the special rect, i.e. the small box to the top left, then
      // the item should be dropped onto the players current weapon slot
      //
      if ( CursorIsInSpecialRect ( CurPos.x , CurPos.y ) )
	{
	  DebugPrintf( 0 , "\nItem dropped onto the shield rectangle!" );
	  DebugPrintf( 0 , "\nGetHeldItemCode: %d." , GetHeldItemCode() );
	  if ( ( GetHeldItemCode() != (-1) ) &&
	       ( ItemMap[ GetHeldItemCode() ].item_can_be_installed_in_special_slot ) )
	    {
	      DropHeldItemToSpecialSlot ( );
	      Item_Held_In_Hand = ( -1 );
	    }
	  else
	    {
	      // If the item can't be used as a weapon, we don't do anything
	    }
	}

      //--------------------
      // If the cursor is in the aux1 rect, i.e. the small box to the left middle, then
      // the item should be dropped onto the players current aux1 slot
      //
      if ( CursorIsInAux1Rect ( CurPos.x , CurPos.y ) )
	{
	  DebugPrintf( 0 , "\nItem dropped onto the aux1 rectangle!" );
	  DebugPrintf( 0 , "\nGetHeldItemCode: %d." , GetHeldItemCode() );
	  if ( ( GetHeldItemCode() != (-1) ) &&
	       ( ItemMap[ GetHeldItemCode() ].item_can_be_installed_in_aux_slot ) )
	    {
	      DropHeldItemToAux1Slot ( );
	      Item_Held_In_Hand = ( -1 );
	    }
	  else
	    {
	      // If the item can't be used as a weapon, we don't do anything
	    }
	}

      //--------------------
      // If the cursor is in the aux2 rect, i.e. the small box to the left middle, then
      // the item should be dropped onto the players current aux1 slot
      //
      if ( CursorIsInAux2Rect ( CurPos.x , CurPos.y ) )
	{
	  DebugPrintf( 0 , "\nItem dropped onto the aux2 rectangle!" );
	  DebugPrintf( 0 , "\nGetHeldItemCode: %d." , GetHeldItemCode() );
	  if ( ( GetHeldItemCode() != (-1) ) &&
	       ( ItemMap[ GetHeldItemCode() ].item_can_be_installed_in_aux_slot ) )
	    {
	      DropHeldItemToAux2Slot ( );
	      Item_Held_In_Hand = ( -1 );
	    }
	  else
	    {
	      // If the item can't be used as a weapon, we don't do anything
	    }
	}

    } // if release things...

 NoMoreReleasing:
 NoMoreGrabbing:


  //--------------------
  // Now that we have filled all the positions in the inventory, we can start to draw the
  // items the player currently has 'in his hand' via the mouse drag-and-drop
  // grip feature.
  //
  // if ( axis_is_active ) 
  if ( Item_Held_In_Hand != (-1) )
    {
      DisplayItemImageAtMouseCursor( Item_Held_In_Hand );
    }
  else
    {
      // In case the player does not have anything in his hand, then of course we need to
      // unset everything as 'not in his hand'.
      //
      // printf("\n Mouse button should cause no image now.");
    }


  //--------------------
  // Finally, we want the part of the screen we have been editing to become
  // visible and therefore we must updated it here, since it is currently not
  // contained within the user rectangle that also gets updated every frame.
  //
  SDL_UpdateRect( Screen , InventoryRect.x , InventoryRect.y , InventoryRect.w , InventoryRect.h );


  MouseButtonPressedPreviousFrame = axis_is_active;
}; // void ManageInventoryScreen ( void );

/* ----------------------------------------------------------------------
 * This function display the character screen.
 * ---------------------------------------------------------------------- */
void 
ShowCharacterScreen ( void )
{
  static SDL_Rect CharacterRect;
  static SDL_Surface *CharacterScreenImage = NULL;
  char *fpath;
  char fname[]="character.png";
  char CharText[1000];
  SDL_Rect TargetRect;

  DebugPrintf (2, "\nvoid ShowInventoryMessages( ... ): Function call confirmed.");

  //--------------------
  // If the log is not set to visible right now, we do not need to 
  // do anything more, but to restore the usual user rectangle size
  // back to normal and to return...
  //
  if ( GameConfig.CharacterScreen_Visible == FALSE ) return;

  // --------------------
  // Some things like the loading of the character screen
  // need to be done only once at the first call of this
  // function. 
  //
  if ( CharacterScreenImage == NULL )
    {
      // SDL_FillRect( Screen, & InventoryRect , 0x0FFFFFF );
      fpath = find_file ( fname , GRAPHICS_DIR, FALSE);
      CharacterScreenImage = IMG_Load( fpath );
      //--------------------
      // We define the right side of the user screen as the rectangle
      // for our inventory screen.
      //
      CharacterRect.x = SCREENLEN/2;
      CharacterRect.y = User_Rect.y;
      CharacterRect.w = SCREENLEN/2;
      CharacterRect.h = User_Rect.h;
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
  // Strength, Dex, ...
  //
  sprintf( CharText , "%d", Me.Strength );
  DisplayText( CharText , 100 + CharacterRect.x , 143 + CharacterRect.y , &CharacterRect );
  sprintf( CharText , "%d", Me.Strength );
  DisplayText( CharText , 148 + CharacterRect.x , 143 + CharacterRect.y , &CharacterRect );

  sprintf( CharText , "%d", Me.Magic );
  DisplayText( CharText , 100 + CharacterRect.x , 171 + CharacterRect.y , &CharacterRect );
  sprintf( CharText , "%d", Me.Magic );
  DisplayText( CharText , 148 + CharacterRect.x , 171 + CharacterRect.y , &CharacterRect );

  sprintf( CharText , "%d", Me.Dexterity );
  DisplayText( CharText , 100 + CharacterRect.x , 200 + CharacterRect.y , &CharacterRect );
  sprintf( CharText , "%d", Me.Dexterity );
  DisplayText( CharText , 148 + CharacterRect.x , 200 + CharacterRect.y , &CharacterRect );

  sprintf( CharText , "%d", Me.Vitality );
  DisplayText( CharText , 100 + CharacterRect.x , 227 + CharacterRect.y , &CharacterRect );
  sprintf( CharText , "%d", Me.Vitality );
  DisplayText( CharText , 148 + CharacterRect.x , 227 + CharacterRect.y , &CharacterRect );

  Me.Experience = RealScore;
  sprintf( CharText , "%6ld", Me.Experience ); // this should be the real score, sooner or later
  DisplayText( CharText , 250 + CharacterRect.x ,  58 + CharacterRect.y , &CharacterRect );
  // sprintf( CharText , "%d", Me.Vitality );
  // DisplayText( CharText , 148 + CharacterRect.x , 227 + CharacterRect.y , &CharacterRect );

  sprintf( CharText , "%d", (int) Druidmap[ DRUID001 ].maxenergy );
  DisplayText( CharText , 95 + CharacterRect.x , 293 + CharacterRect.y , &CharacterRect );

  sprintf( CharText , "%d", (int) Me.energy );
  DisplayText( CharText , 143 + CharacterRect.x , 293 + CharacterRect.y , &CharacterRect );


  //--------------------
  // Finally, we want the part of the screen we have been editing to become
  // visible and therefore we must updated it here, since it is currently not
  // contained within the user rectangle that also gets updated every frame.
  //
  SDL_UpdateRect( Screen , CharacterRect.x , CharacterRect.y , CharacterRect.w , CharacterRect.h );



}; // ShowCharacterScreen ( void )



#undef _items_c
