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

#define ARMOUR_RECT_WIDTH 64
#define ARMOUR_RECT_HEIGHT 64
#define ARMOUR_POS_X 22
#define ARMOUR_POS_Y 81

#define SHIELD_RECT_WIDTH 64
#define SHIELD_RECT_HEIGHT 64
#define SHIELD_POS_X 240
#define SHIELD_POS_Y 16

#define SPECIAL_RECT_WIDTH 32
#define SPECIAL_RECT_HEIGHT 32
#define SPECIAL_POS_X 93
#define SPECIAL_POS_Y 29


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
      // DebugPrintf( 0 , "\nNo item in inventory seems to be currently held in hand...");
      InvPos = ( -1 ) ;
    }
  else
    {
      // DebugPrintf( 0 , "\nInventory item index %d was held in hand." , InvPos );
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
      return ( -1 );
    }
  else
    {
      // DebugPrintf( 0 , "\nInventory item index %d was held in hand." , InvPos );
    }
  
  return ( Me.Inventory[ InvPos ].type );
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
DropHeldItemToDriveSlot ( void )
{
  int InvPos;

  InvPos = GetHeldItemInventoryIndex( );

  // Now the item is installed into the weapon slot of the influencer
  Druidmap[ DRUID001 ].drive_item = Me.Inventory[ InvPos ].type;

  // Now the item is removed from inventory and no longer held in hand as well...
  Me.Inventory[ InvPos ].type = ( -1 );
  Me.Inventory[ InvPos ].currently_held_in_hand = FALSE;

}; // void DropHeldItemToDriveSlot ( void )

void
DropHeldItemToArmourSlot ( void )
{
  int InvPos;

  InvPos = GetHeldItemInventoryIndex( );

  // Now the item is installed into the weapon slot of the influencer
  Druidmap[ DRUID001 ].armour_item = Me.Inventory[ InvPos ].type;

  // Now the item is removed from inventory and no longer held in hand as well...
  Me.Inventory[ InvPos ].type = ( -1 );
  Me.Inventory[ InvPos ].currently_held_in_hand = FALSE;

}; // void DropHeldItemToArmourSlot ( void )

void
DropHeldItemToShieldSlot ( void )
{
  int InvPos;

  InvPos = GetHeldItemInventoryIndex( );

  // Now the item is installed into the weapon slot of the influencer
  Druidmap[ DRUID001 ].shield_item = Me.Inventory[ InvPos ].type;

  // Now the item is removed from inventory and no longer held in hand as well...
  Me.Inventory[ InvPos ].type = ( -1 );
  Me.Inventory[ InvPos ].currently_held_in_hand = FALSE;

}; // void DropHeldItemToShieldSlot ( void )

void
DropHeldItemToSpecialSlot ( void )
{
  int InvPos;

  InvPos = GetHeldItemInventoryIndex( );

  // Now the item is installed into the weapon slot of the influencer
  Druidmap[ DRUID001 ].special_item = Me.Inventory[ InvPos ].type;

  // Now the item is removed from inventory and no longer held in hand as well...
  Me.Inventory[ InvPos ].type = ( -1 );
  Me.Inventory[ InvPos ].currently_held_in_hand = FALSE;

}; // void DropHeldItemToShieldSlot ( void )

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


/* ----------------------------------------------------------------------
 * This function display the inventory screen and also checks for mouse
 * actions in the invenotry screen.
 * ---------------------------------------------------------------------- */
void 
ShowInventoryScreen ( void )
{
  int SlotNum;
  static SDL_Rect InventoryRect;
  SDL_Rect TargetRect;
  static SDL_Surface *InventoryImage = NULL;
  char *fpath;
  char fname[]="inventory.png";
  static int Item_Held_In_Hand = -1 ;
  static int MouseButtonPressedPreviousFrame = FALSE;
  point CurPos;
  point Inv_GrabLoc;
  int Grabbed_InvPos;
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
  if ( GameConfig.Inventory_Visible == FALSE ) 
    {
      User_Rect.x = 0;
      User_Rect.w = SCREENLEN;
      return;
    }

  // --------------------
  // Some things like the loading of the inventory and initialisation of the
  // inventory rectangle need to be done only once at the first call of this
  // function. 
  //
  if ( InventoryImage == NULL )
    {
      // SDL_FillRect( Screen, & InventoryRect , 0x0FFFFFF );
      fpath = find_file ( fname , GRAPHICS_DIR, FALSE);
      InventoryImage = IMG_Load( fpath );
      //--------------------
      // We define the right side of the user screen as the rectangle
      // for our inventory screen.
      //
      InventoryRect.x = 0;
      InventoryRect.y = User_Rect.y;
      InventoryRect.w = SCREENLEN/2;
      InventoryRect.h = User_Rect.h;
    }

  //--------------------
  // At this point we know, that the inventory screen is desired and must be
  // displayed in-game:
  //
  // Therefore we split the screen to half, so that one half can be used for 
  // the inventory and the other for the normal user (combat) rectangle
  //
  // Into this inventory rectangle we draw the inventory mask
  //
  User_Rect.x = SCREENLEN/2;
  User_Rect.w = SCREENLEN/2;
  // SDL_SetClipRect( Screen, &InventoryRect );
  SDL_SetClipRect( Screen, NULL );
  SDL_BlitSurface ( InventoryImage , NULL , Screen , &InventoryRect );

  //--------------------
  // Now we display the item in the influencer drive slot
  //
  TargetRect.x = InventoryRect.x + 240;
  TargetRect.y = InventoryRect.y + 93;
  TargetRect.w = 50;
  TargetRect.h = 50;
  SDL_BlitSurface( ItemImageList[ ItemMap[ Druidmap[ Me.type ].drive_item ].picture_number ].Surface , NULL , Screen , &TargetRect );
  
  //--------------------
  // Now we display the item in the influencer weapon slot
  //
  TargetRect.x = InventoryRect.x + 20;
  TargetRect.y = InventoryRect.y + 10;
  TargetRect.w = 50;
  TargetRect.h = 50;
  SDL_BlitSurface( ItemImageList[ ItemMap[ Druidmap[ Me.type ].weapon_item ].picture_number ].Surface , NULL , Screen , &TargetRect );
  
  //--------------------
  // Now we display the item in the influencer armour slot
  //
  TargetRect.x = InventoryRect.x + ARMOUR_POS_X ;
  TargetRect.y = InventoryRect.y + ARMOUR_POS_Y ;
  TargetRect.w = 50;
  TargetRect.h = 50;
  SDL_BlitSurface( ItemImageList[ ItemMap[ Druidmap[ Me.type ].armour_item ].picture_number ].Surface , NULL , Screen , &TargetRect );
  
  //--------------------
  // Now we display the item in the influencer shield slot
  //
  TargetRect.x = InventoryRect.x + SHIELD_POS_X ;
  TargetRect.y = InventoryRect.y + SHIELD_POS_Y ;
  TargetRect.w = 50;
  TargetRect.h = 50;
  SDL_BlitSurface( ItemImageList[ ItemMap[ Druidmap[ Me.type ].shield_item ].picture_number ].Surface , NULL , Screen , &TargetRect );
  
  //--------------------
  // Now we display the item in the influencer special slot
  //
  TargetRect.x = InventoryRect.x + SPECIAL_POS_X ;
  TargetRect.y = InventoryRect.y + SPECIAL_POS_Y ;
  TargetRect.w = 50;
  TargetRect.h = 50;
  SDL_BlitSurface( ItemImageList[ ItemMap[ Druidmap[ Me.type ].special_item ].picture_number ].Surface , NULL , Screen , &TargetRect );
  

  //--------------------
  // Now we display all the items the influencer is carrying with him
  //
  for ( SlotNum = 0 ; SlotNum < MAX_ITEMS_IN_INVENTORY; SlotNum ++ )
    {
      // In case the item does not exist at all, we need not do anything more...
      if ( Me.Inventory[ SlotNum ].type == ( -1 ) ) 
	{
	  // DisplayText( "\n--- Slot empty ---" , -1 , -1 , &InventoryRect );
	  continue;
	}

      // In case the item is currently held in hand, we need not do anything more HERE ...
      if ( Me.Inventory[ SlotNum ].currently_held_in_hand == TRUE )
	{
	  continue;
	}

      TargetRect.x = 16 + 32 * Me.Inventory[ SlotNum ].inventory_position.x;
      TargetRect.y = User_Rect.y - 64 + 480 - 16 - 32 * 6 + 32 * Me.Inventory[ SlotNum ].inventory_position.y;
      TargetRect.w = 50;
      TargetRect.h = 50;

      SDL_BlitSurface( ItemImageList[ ItemMap[ Me.Inventory[ SlotNum ].type ].picture_number ].Surface , NULL , Screen , &TargetRect );
    }


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
	      DebugPrintf( 0 , "\nGrabbing FAILED!" );
	    }
	  else
	    {
	      //--------------------
	      // At this point we know, that we have just grabbed something from the inventory
	      // So we set, that something should be displayed in the 'hand', and it should of
	      // course be the image of the item grabbed from inventory.
	      //
	      Item_Held_In_Hand = ItemMap[ Me.Inventory[ Grabbed_InvPos ].type ].picture_number ;
	      Me.Inventory[ Grabbed_InvPos ].currently_held_in_hand = TRUE;
	    }
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
	  if ( ItemMap[ GetHeldItemCode() ].item_can_be_installed_in_weapon_slot )
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
	  if ( ItemMap[ GetHeldItemCode() ].item_can_be_installed_in_drive_slot )
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
	  if ( ItemMap[ GetHeldItemCode() ].item_can_be_installed_in_armour_slot )
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
	  if ( ItemMap[ GetHeldItemCode() ].item_can_be_installed_in_shield_slot )
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
	  if ( ItemMap[ GetHeldItemCode() ].item_can_be_installed_in_special_slot )
	    {
	      DropHeldItemToSpecialSlot ( );
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
}; // void ShowInventoryScreen ( void );

/* ----------------------------------------------------------------------
 * This function display the character screen.
 * ---------------------------------------------------------------------- */
void 
ShowCharacterScreen ( void )
{
  static SDL_Rect CharacterRect;
  SDL_Rect TargetRect;
  static SDL_Surface *CharacterScreenImage = NULL;
  char *fpath;
  char fname[]="character.png";
  char CharText[1000];

  DebugPrintf (2, "\nvoid ShowInventoryMessages( ... ): Function call confirmed.");

  //--------------------
  // If the log is not set to visible right now, we do not need to 
  // do anything more, but to restore the usual user rectangle size
  // back to normal and to return...
  //
  if ( GameConfig.CharacterScreen_Visible == FALSE ) 
    {
      User_Rect.x = 0;
      User_Rect.w = SCREENLEN;
      return;
    }

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
  // Therefore we split the screen to half, so that one half can be used for 
  // the inventory and the other for the normal user (combat) rectangle
  //
  // Into this inventory rectangle we draw the inventory mask
  //
  User_Rect.x = 0; // SCREENLEN/2;
  User_Rect.w = SCREENLEN/2;
  // SDL_SetClipRect( Screen, &InventoryRect );
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
