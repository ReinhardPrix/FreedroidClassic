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

#define ITEM_TAKE_DIST (1.2)

/* ----------------------------------------------------------------------
 * This function calculates the price of a given item, taking into account
 * (*) the items base list price 
 * (*) the items given prefix modifier
 * (*) the items given suffix modifier
 * (*) AND THE CURRENT DURATION of the item in relation to its max duration.
 * ---------------------------------------------------------------------- */
long
CalculateItemPrice ( item* BuyItem , int ForRepair )
{
  float PrefixMultiplier = 1;
  float SuffixMultiplier = 1;

  if ( BuyItem -> suffix_code != (-1) )
    SuffixMultiplier = SuffixList[ BuyItem->suffix_code ].price_factor;

  if ( !ForRepair )
    {
      if ( BuyItem->max_duration != (-1 ) )
	{
	  return ( ItemMap [ BuyItem->type ].base_list_price * SuffixMultiplier * PrefixMultiplier *
		   ( BuyItem->current_duration ) / BuyItem->max_duration ); 
	}
      else
	{
	  return ( ItemMap [ BuyItem->type ].base_list_price * SuffixMultiplier * PrefixMultiplier );
	}
    }

  // --------------------
  // This is the price of the DAMAGE in the item, haha
  // This can only be requested for repair items
  //
  if ( BuyItem->max_duration != (-1 ) )
    {
      return ( ItemMap [ BuyItem->type ].base_list_price * SuffixMultiplier * PrefixMultiplier *
	       ( BuyItem->max_duration - BuyItem->current_duration ) / BuyItem->max_duration ); 
    }
  else
    {
      DebugPrintf( 0 , "\n\nERROR!! CALCULATING REPAIR PRICE FOR INDESTRUCTIBLE ITEM!! \n\n Terminating..." );
      Terminate( ERR );
    }
  return 0;
}; // long CalculateItemPrice ( item* BuyItem )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
FillInItemProperties( item* ThisItem , int FullDuration , int TreasureChestRange )
{

  ThisItem->damage = ItemMap[ ThisItem->type ].base_item_gun_damage;
  ThisItem->damage_modifier = ItemMap[ ThisItem->type ].item_gun_damage_modifier;
  ThisItem->ac_bonus = ItemMap[ ThisItem->type ].base_ac_bonus +
    MyRandom( ItemMap[ ThisItem->type ].ac_bonus_modifier );

  //--------------------
  // In case of cyberbucks, we have to specify the amount of cyberbucks
  //
  if ( ThisItem->type == ITEM_MONEY )
    {
      ThisItem->gold_amount = 20 * TreasureChestRange + MyRandom( 20 ) + 1;
    }

  //--------------------
  // We now have to set a duration, as well a maximum duration
  // as well as a current duration, the later of which will be
  // a fraction of the maximum duration.
  //
  if ( ItemMap[ ThisItem->type ].base_item_duration != (-1) )
    {
      ThisItem->max_duration = ItemMap[ ThisItem->type ].base_item_duration +
	MyRandom( ItemMap[ ThisItem->type ].item_duration_modifier );
      if ( FullDuration ) ThisItem->current_duration = ThisItem->max_duration ;
      else ThisItem->current_duration = MyRandom( ThisItem->max_duration ) ;
    }
  else
    {
      ThisItem->max_duration = ( -1 );
      ThisItem->current_duration = 1 ;
    }
}; // void FillInItemProperties( item* ThisItem , int FullDuration , int TreasureChestRange )

/* ----------------------------------------------------------------------
 * This function drops an item at a given place, assigning it the given
 * suffix and prefix code.
 * ---------------------------------------------------------------------- */
void
DropItemAt( int ItemType , float x , float y , int prefix , int suffix , int TreasureChestRange )
{
  int i;

  //--------------------
  // If given a non-existent item type, we don't do anything
  // of course (and also don't produce a SEGFAULT or something...)
  //
  if ( ItemType == ( -1 ) ) return;
  if ( ItemType >= Number_Of_Item_Types ) 
    {
      fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
There was an item code for an item to drop given to the function \n\
DropItemAt( ... ), which is pointing beyond the scope of the known\n\
item types.  This indicates a severe bug in Freedroid.\n\
\n\
The given item type was : %d.\n\
\n\
Please inform the Freedroid developers about the problem.\n\
For now Freedroid will terminate to draw attention \n\
to the item problem it could not resolve.\n\
Sorry if that stops a major game of yours....\n\
----------------------------------------------------------------------\n\
\n" , ItemType );
      Terminate(ERR);
      return;
    }

  //--------------------
  // At first we must find a free item index on this level,
  // so that we can enter the new item there.
  //
  for ( i = 0 ; i < MAX_ITEMS_PER_LEVEL ; i ++ )
    {
      if ( CurLevel->ItemList[ i ].type == (-1) ) 
	{
	  break;
	}
    }
  if ( i >= MAX_ITEMS_PER_LEVEL )
    {
      DebugPrintf( 0 , "\n\nNO MORE ITEMS DROPABLE INTO THIS LEVEL!!\n\nTerminating!" );
      Terminate( ERR );
    }

  //--------------------
  // Now we can construct the new item
  //
  CurLevel->ItemList[ i ].type = ItemType;
  CurLevel->ItemList[ i ].pos.x = x;
  CurLevel->ItemList[ i ].pos.y = y;
  CurLevel->ItemList[ i ].prefix_code = prefix;
  CurLevel->ItemList[ i ].suffix_code = suffix;

  FillInItemProperties ( & ( CurLevel->ItemList[ i ] ) , FALSE , TreasureChestRange );


  PlayItemSound( ItemMap[ ItemType ].sound_number );

}; // void DropItemAt( int ItemType , int x , int y , int prefix , int suffix , int TreasureChestRange )

/* ----------------------------------------------------------------------
 * This function drops a random item to the floor of the current level
 * at position ( x , y ).
 *
 * The strategy in droping the item is that one can easily set up and
 * modify the tabe of items to be dropped.
 *
 * ---------------------------------------------------------------------- */
void
DropRandomItem( float x , float y , int TreasureChestRange , int ForceMagical , int ForceDrop )
{
  int Suf; int Pre;
  int DropDecision;

  //--------------------
  // First we determine if there is something dropped at all or not,
  // cause in the later case, we can return immediately.  If a drop is
  // forced, we need not check for not do drop.
  //
  DropDecision = MyRandom( 100 );

  //--------------------
  // We decide whether we drop something at all or not
  //
  if ( ( !ForceDrop ) && ( DropDecision < 100 - GOLD_DROP_PERCENTAGE - ITEM_DROP_PERCENTAGE ) ) return;

  //--------------------
  // Perhaps it's some gold that will be dropped.  That's rather
  // simple, so we do this first.
  //
  if ( ( !ForceDrop ) && ( DropDecision < 100 - ITEM_DROP_PERCENTAGE ) )
    {
      DropItemAt( ITEM_MONEY , x , y , -1 , -1 , TreasureChestRange );
      return;
    }

  //--------------------
  // So at this point we know, that an item will be dropped...
  //
  // Since there are no prefixes set up yet, we just need to consider
  // the suffixes.  In case 
  //
  Pre = ( -1 ) ;
  if ( ForceMagical )
    {
      Suf = MyRandom( 4 * TreasureChestRange );
    }
  else
    {
      Suf = ( -1 );
    }


  switch ( MyRandom ( TreasureChestRange ) )
    {
    case 0:
      switch ( MyRandom ( 7 ) )
	{
	case 0:
	  DropItemAt( ITEM_ANTIGRAV_ALPHA , x , y , Pre , Suf , TreasureChestRange );
	  break;
	case 1:
	  DropItemAt( ITEM_SHORT_BOW , x , y , Pre , Suf , TreasureChestRange );
	  break;
	case 2:
	  DropItemAt( ITEM_SMALL_HEALTH_POTION , x , y , -1 , -1 , TreasureChestRange );
	  break;
	case 3:
	  DropItemAt( ITEM_SMALL_MANA_POTION , x , y , -1 , -1 , TreasureChestRange );
	  break;
	case 4:
	  DropItemAt( ITEM_BUCKLER , x , y , Pre , Suf , TreasureChestRange );
	  break;
	case 5:
	  DropItemAt( ITEM_SHORT_SWORD , x , y , Pre , Suf , TreasureChestRange );
	  break;
	case 6:
	  DropItemAt( ITEM_DAGGER , x , y , Pre , Suf , TreasureChestRange );
	  break;
	case 7:
	  DropItemAt( ITEM_CAP , x , y , Pre , Suf , TreasureChestRange );
	  break;
	}
      break;
    case 1:
      switch ( MyRandom ( 5 ) )
	{
	case 0:
	  DropItemAt( ITEM_WHEELS , x , y , Pre , Suf , TreasureChestRange );
	  break;
	case 1:
	  DropItemAt( ITEM_HUNTERS_BOW , x , y , Pre , Suf , TreasureChestRange );
	  break;
	case 2:
	  DropItemAt( ITEM_FULL_HEALTH_POTION , x , y , -1 , -1 , TreasureChestRange );
	  break;
	case 3:
	  DropItemAt( ITEM_FULL_MANA_POTION , x , y , -1 , -1 , TreasureChestRange );
	  break;
	case 4:
	  DropItemAt( ITEM_SMALL_SHIELD , x , y , Pre , Suf , TreasureChestRange );
	  break;
	case 5:
	  DropItemAt( ITEM_SCIMITAR , x , y , Pre , Suf , TreasureChestRange );
	  break;
	}
      break;
    case 2:
      switch ( MyRandom ( 5 ) )
	{
	case 0:
	  DropItemAt( ITEM_ANTIGRAV_BETA , x , y , Pre , Suf , TreasureChestRange );
	  break;
	case 1:
	  DropItemAt( ITEM_HUNTERS_BOW , x , y , Pre , Suf , TreasureChestRange );
	  break;
	case 2:
	  DropItemAt( ITEM_FULL_HEALTH_POTION , x , y , -1 , -1 , TreasureChestRange );
	  break;
	case 3:
	  DropItemAt( ITEM_FULL_MANA_POTION , x , y , -1 , -1 , TreasureChestRange );
	  break;
	case 4:
	  DropItemAt( ITEM_SMALL_SHIELD , x , y , Pre , Suf , TreasureChestRange );
	  break;
	case 5:
	  DropItemAt( ITEM_SCIMITAR , x , y , Pre , Suf , TreasureChestRange );
	  break;
	}
      break;
    case 3:
      switch ( MyRandom ( 7 ) )
	{
	case 0:
	  DropItemAt( ITEM_HUNTERS_BOW , x , y , Pre , Suf , TreasureChestRange );
	  break;
	case 1:
	  DropItemAt( ITEM_ANTIGRAV_BETA , x , y , Pre , Suf , TreasureChestRange );
	  break;
	case 2:
	  DropItemAt( ITEM_FULL_HEALTH_POTION , x , y , -1 , -1 , TreasureChestRange );
	  break;
	case 3:
	  DropItemAt( ITEM_FULL_MANA_POTION , x , y , -1 , -1 , TreasureChestRange );
	  break;
	case 4:
	  DropItemAt( ITEM_SMALL_SHIELD , x , y , Pre , Suf , TreasureChestRange );
	  break;
	case 5:
	  DropItemAt( ITEM_SCIMITAR , x , y , Pre , Suf , TreasureChestRange );
	  break;
	}
      break;
    default:
      DebugPrintf( 0 , "\n\nERRROR!!! UNHANDLED TREASURE CHEST ENCOUNTERED!!! TERMINATING...\n\n" );
      Terminate ( ERR );
      break;
    }

}; // void DropRandomItem( int x , int y )

/* ----------------------------------------------------------------------
 * When the influencer gets hit, all of his equipment suffers some damage.
 * This is exactly what this function does:  apply the damage.
 * ---------------------------------------------------------------------- */
void 
DamageItem( item* CurItem )
{

  //--------------------
  // If the item mentioned as parameter exists and if it is of 
  // a destructable sort, then we apply the usual damage to it
  // (which is currently a bit high)
  //
  if ( ( CurItem->type != (-1) ) &&
       ( CurItem->max_duration != (-1) ) )
    {
      CurItem->current_duration -= 0.003 * MyRandom( 100 ) ;

      //--------------------
      // If the item has gone over it's threshhold of duration, it finally
      // breaks and vaporizes
      //
      if ( rintf( CurItem->current_duration ) <= 0 )
	{
	  DeleteItem( CurItem );
	}
    }

}; // void DamageItem( item* CurItem )

/* ----------------------------------------------------------------------
 * When the influencer gets hit, all of his equipment suffers some damage.
 * This is exactly what this function does:  apply the damage.
 * ---------------------------------------------------------------------- */
void 
DamageAllEquipment( void )
{
  DamageItem( & ( Me.weapon_item ) );
  DamageItem( & ( Me.armour_item ) );
  DamageItem( & ( Me.shield_item ) );
  DamageItem( & ( Me.drive_item ) );
  DamageItem( & ( Me.special_item ) );
  DamageItem( & ( Me.aux1_item ) );
  DamageItem( & ( Me.aux2_item ) );
}; // void DamageAllEquipment( void )

void
MakeHeldFloorItemOutOf( item* SourceItem )
{
  int i;

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
  CopyItem( SourceItem , &(CurLevel->ItemList[ i ]) , FALSE );

  CurLevel->ItemList[ i ].pos.x = Me.pos.x;
  CurLevel->ItemList[ i ].pos.y = Me.pos.y;
  CurLevel->ItemList[ i ].currently_held_in_hand = TRUE;
  
  Item_Held_In_Hand = ItemMap [ CurLevel->ItemList[ i ].type ].picture_number ;
  
  // The original item will be overwritten anyway when the new item
  // replaces the old one afterwards.

}; // void MakeHeldFloorItemOutOf( item* SourceItem )

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
      if ( Me.Inventory[ InvPos ].currently_held_in_hand && ( Me.Inventory[ InvPos ].type > 0 ) ) 
	{
	  return ( InvPos );
	}
    }

  return ( -1 );
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
      // DebugPrintf( 2 , "\nitem* GetHeldItemPointer( void ) : An item in inventory was held in hand.  Good.");
      return ( & ( Me.Inventory[ InvIndex ] ) );
    } 
  else if ( Me.weapon_item.currently_held_in_hand > 0 )
    {
      // DebugPrintf( 2 , "\nitem* GetHeldItemPointer( void ) : An item in weapon slot was held in hand.  Good.");
      return ( & ( Me.weapon_item ) );
    }
  else if ( Me.drive_item.currently_held_in_hand > 0 )
    {
      // DebugPrintf( 2 , "\nitem* GetHeldItemPointer( void ) : An item in weapon slot was held in hand.  Good.");
      return ( & ( Me.drive_item ) );
    }
  else if ( Me.shield_item.currently_held_in_hand > 0 )
    {
      // DebugPrintf( 2 , "\nitem* GetHeldItemPointer( void ) : An item in weapon slot was held in hand.  Good.");
      return ( & ( Me.shield_item ) );
    }
  else if ( Me.armour_item.currently_held_in_hand > 0 )
    {
      // DebugPrintf( 2 , "\nitem* GetHeldItemPointer( void ) : An item in weapon slot was held in hand.  Good.");
      return ( & ( Me.armour_item ) );
    }
  else if ( Me.special_item.currently_held_in_hand > 0 )
    {
      // DebugPrintf( 2 , "\nitem* GetHeldItemPointer( void ) : An item in weapon slot was held in hand.  Good.");
      return ( & ( Me.special_item ) );
    }
  else if ( Me.aux1_item.currently_held_in_hand > 0 )
    {
      // DebugPrintf( 2 , "\nitem* GetHeldItemPointer( void ) : An item in weapon slot was held in hand.  Good.");
      return ( & ( Me.aux1_item ) );
    }
  else if ( Me.aux2_item.currently_held_in_hand > 0 )
    {
      // DebugPrintf( 2 , "\nitem* GetHeldItemPointer( void ) : An item in weapon slot was held in hand.  Good.");
      return ( & ( Me.aux2_item ) );
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


      // DebugPrintf( 2 , "\nitem* GetHeldItemPointer( void ) : NO ITEM AT ALL SEEMS TO HAVE BEEN HELD IN HAND!!");
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
CopyItem( item* SourceItem , item* DestItem , int MakeSound )
{

  memcpy ( DestItem, SourceItem, sizeof ( item ));

  if ( MakeSound ) PlayItemSound( ItemMap[ SourceItem->type ].sound_number );

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

/* ----------------------------------------------------------------------
 * This function applies a given item (to the influencer) and maybe 
 * eliminates the item after that, if it's an item that gets used up.
 * ---------------------------------------------------------------------- */
void
Quick_ApplyItem( int ItemKey )
{
  int FoundItemNr;

  DebugPrintf( 0 , "\nvoid Quick_ApplyItem( ... ): function call confirmed.");
  
  //--------------------
  // At first we find out which item occupies the given position, and 
  // we return immediately of course, if there is nothing at all at that
  // given location.
  //
  FoundItemNr = GetInventoryItemAt ( ItemKey -1 , INVENTORY_GRID_HEIGHT -1 );
  if ( FoundItemNr == (-1) ) return;

  //--------------------
  // Now that we have found the discusses item, we can just apply
  // it...
  //

  ApplyItem ( & ( Me.Inventory[ FoundItemNr ] ) );

}; // void Quick_ApplyItem( item* CurItem )

/* ----------------------------------------------------------------------
 * This function applies a given item (to the influencer) and maybe 
 * eliminates the item after that, if it's an item that gets used up.
 * ---------------------------------------------------------------------- */
void
ApplyItem( item* CurItem )
{
  DebugPrintf( 0 , "\nvoid ApplyItem( ... ): function call confirmed.");

  // If the inventory slot is not at all filled, we need not do anything more...
  if ( CurItem->type == (-1) ) return;

  if ( ItemMap[ CurItem->type ].item_can_be_applied_in_combat == FALSE ) 
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
  if ( CurItem->type == ITEM_SMALL_HEALTH_POTION )
    {
      Me.health += 25;
      Me.energy += 25;
    }
  else if ( CurItem->type == ITEM_FULL_HEALTH_POTION )
    {
      Me.health += Me.maxenergy;
      Me.energy += Me.maxenergy;
    }
  else if ( CurItem->type == ITEM_SMALL_MANA_POTION )
    {
      Me.mana += 25;
    }
  else if ( CurItem->type == ITEM_FULL_MANA_POTION )
    {
      Me.mana += Me.maxmana;
    }

  if ( Me.energy > Me.maxenergy ) Me.energy = Me.maxenergy ;
  if ( Me.mana > Me.maxmana ) Me.mana = Me.maxmana ;

  PlayItemSound( ItemMap[ CurItem->type ].sound_number );

  //--------------------
  // In some cases the item concerned is a one-shot-device like a health potion, which should
  // evaporize after the first application.  Therefore we delete the item from the inventory list.
  //
  DeleteItem ( CurItem );

}; // void ApplyItemFromInventory( int ItemNum )

/* ----------------------------------------------------------------------
 * This function checks if a given coordinate within the influencers
 * inventory grid can be considered as free or as occupied by some item.
 * ---------------------------------------------------------------------- */
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

  if ( ( CurPos.x >= WEAPON_RECT_X ) && ( CurPos.x <= WEAPON_RECT_X + WEAPON_RECT_WIDTH ) )
    {
      DebugPrintf( INVENTORY_RECTANGLE_DEBUG_LEVEL , "\nMight be grabbing in weapon rectangle, as far as x is concerned.");
      if ( ( CurPos.y >= User_Rect.y + WEAPON_RECT_Y ) && 
	   ( CurPos.y <= User_Rect.y + WEAPON_RECT_Y + WEAPON_RECT_HEIGHT ) )
	{
	  DebugPrintf( INVENTORY_RECTANGLE_DEBUG_LEVEL , "\nMight be grabbing in weapon rectangle, as far as y is concerned.");
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

  if ( ( CurPos.x >= DRIVE_RECT_X ) && ( CurPos.x <= DRIVE_RECT_X + DRIVE_RECT_WIDTH ) )
    {
      DebugPrintf( INVENTORY_RECTANGLE_DEBUG_LEVEL , "\nMight be grabbing in drive rectangle, as far as x is concerned.");
      if ( ( CurPos.y >= User_Rect.y + DRIVE_RECT_Y ) && 
	   ( CurPos.y <= User_Rect.y + DRIVE_RECT_Y + DRIVE_RECT_HEIGHT ) )
	{
	  DebugPrintf( INVENTORY_RECTANGLE_DEBUG_LEVEL , "\nMight be grabbing in drive rectangle, as far as y is concerned.");
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
      DebugPrintf( INVENTORY_RECTANGLE_DEBUG_LEVEL , 
		   "\nMight be grabbing in armour rectangle, as far as x is concerned.");
      if ( ( CurPos.y >= User_Rect.y + ARMOUR_POS_Y ) && 
	   ( CurPos.y <= User_Rect.y + ARMOUR_POS_Y + ARMOUR_RECT_HEIGHT ) )
	{
	  DebugPrintf( INVENTORY_RECTANGLE_DEBUG_LEVEL , 
		       "\nMight be grabbing in armour rectangle, as far as y is concerned.");
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
      DebugPrintf( INVENTORY_RECTANGLE_DEBUG_LEVEL , 
		   "\nMight be grabbing in armour rectangle, as far as x is concerned.");
      if ( ( CurPos.y >= User_Rect.y + SHIELD_POS_Y ) && 
	   ( CurPos.y <= User_Rect.y + SHIELD_POS_Y + SHIELD_RECT_HEIGHT ) )
	{
	  DebugPrintf( INVENTORY_RECTANGLE_DEBUG_LEVEL , 
		       "\nMight be grabbing in armour rectangle, as far as y is concerned.");
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
      DebugPrintf( INVENTORY_RECTANGLE_DEBUG_LEVEL , 
		   "\nMight be grabbing in armour rectangle, as far as x is concerned.");
      if ( ( CurPos.y >= User_Rect.y + SPECIAL_POS_Y ) && 
	   ( CurPos.y <= User_Rect.y + SPECIAL_POS_Y + SPECIAL_RECT_HEIGHT ) )
	{
	  DebugPrintf( INVENTORY_RECTANGLE_DEBUG_LEVEL , 
		       "\nMight be grabbing in armour rectangle, as far as y is concerned.");
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
      DebugPrintf( INVENTORY_RECTANGLE_DEBUG_LEVEL , 
		   "\nMight be grabbing in aux1 rectangle, as far as x is concerned.");
      if ( ( CurPos.y >= User_Rect.y + AUX1_POS_Y ) && 
	   ( CurPos.y <= User_Rect.y + AUX1_POS_Y + AUX1_RECT_HEIGHT ) )
	{
	  DebugPrintf( INVENTORY_RECTANGLE_DEBUG_LEVEL , 
		       "\nMight be grabbing in aux1 rectangle, as far as y is concerned.");
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
      DebugPrintf( INVENTORY_RECTANGLE_DEBUG_LEVEL , 
		   "\nMight be grabbing in aux1 rectangle, as far as x is concerned.");
      if ( ( CurPos.y >= User_Rect.y + AUX2_POS_Y ) && 
	   ( CurPos.y <= User_Rect.y + AUX2_POS_Y + AUX2_RECT_HEIGHT ) )
	{
	  DebugPrintf( INVENTORY_RECTANGLE_DEBUG_LEVEL , 
		       "\nMight be grabbing in aux1 rectangle, as far as y is concerned.");
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

  if ( ( CurPos.x >= INVENTORY_RECT_X ) && ( CurPos.x <= INVENTORY_RECT_X + INVENTORY_GRID_WIDTH * 32 ) )
    {
      DebugPrintf( INVENTORY_RECTANGLE_DEBUG_LEVEL , "\nMight be grabbing in inventory, as far as x is concerned.");
      if ( ( CurPos.y >= User_Rect.y + INVENTORY_RECT_Y ) && 
	   ( CurPos.y <= User_Rect.y + INVENTORY_RECT_Y + 32 * INVENTORY_GRID_HEIGHT ) )
	{
	  DebugPrintf( INVENTORY_RECTANGLE_DEBUG_LEVEL , "\nMight be grabbing in inventory, as far as y is concerned.");
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
  return ( ( x - INVENTORY_RECT_X ) / 32 );
}; // int GetInventorySquare_x( x )

/* ----------------------------------------------------------------------
 * This function gives the y coordinate of the inventory square that 
 * corresponds to the mouse cursor location given to the function.
 * ---------------------------------------------------------------------- */
int
GetInventorySquare_y( int y )
{
  return ( ( y - (User_Rect.y + INVENTORY_RECT_Y ) ) / 32 );
}; // int GetInventorySquare_y( y )

/* ----------------------------------------------------------------------
 * This function gives the item type of the currently held item.  
 *
 * THIS IS NOT NOT NOT THE PICTURE NUMBER, BUT THE ITEM TYPE!!!!
 *
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

  // DebugPrintf( 2 , "\nint GetHeldItemCode ( void ):  COULDN't FIND HELD ITEM!! " );
  // Terminate( ERR );
  return ( -1 );
  
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
  CopyItem( DropItemPointer , &(CurLevel->ItemList[ i ]) , TRUE );
  // CurLevel->ItemList[ i ].pos.x = Me.pos.x;
  // CurLevel->ItemList[ i ].pos.y = Me.pos.y;
  CurLevel->ItemList[ i ].pos.x = Me.pos.x + (GetMousePos_x() + 16 - UserCenter_x) / (float) Block_Width;
  CurLevel->ItemList[ i ].pos.y = Me.pos.y + (GetMousePos_y() + 16 - UserCenter_y) / (float) Block_Height; 
  CurLevel->ItemList[ i ].currently_held_in_hand = FALSE;
  // CurLevel->ItemList[ i ].type = Me.Inventory[ InvPos ].type;
  
  // Me.Inventory[ InvPos ].type = ( -1 );
  DeleteItem( DropItemPointer );

}; // void DropHeldItemToTheFloor ( void )

/* ----------------------------------------------------------------------
 * This function checks if the usage requirements for a given item are
 * met by the influencer or not.
 * ---------------------------------------------------------------------- */
int 
ItemUsageRequirementsMet( item* UseItem , int MakeSound )
{
  if ( Me.Strength < ItemMap[ UseItem->type ].item_require_strength )
    {
      if ( MakeSound ) Not_Enough_Power_Sound( );
      return ( FALSE );
    }
  if ( Me.Dexterity < ItemMap[ UseItem->type ].item_require_dexterity )
    {
      if ( MakeSound ) Not_Enough_Dist_Sound( );
      return ( FALSE );
    }
  if ( Me.Magic < ItemMap[ UseItem->type ].item_require_magic )
    {
      return ( FALSE );
    }
  return ( TRUE );
}; // int ItemUsageRequirementsMet( item* UseItem )

/* ----------------------------------------------------------------------
 * This function checks, if the influencer mets the requirements of the
 * item currently held in hand by the player/influencer.  Which item this
 * is will be found out by the function.
 * ---------------------------------------------------------------------- */
int 
HeldItemUsageRequirementsMet( void )
{
  item* DropItemPointer;

  // --------------------
  // First we find out which item we want to check
  //
  DropItemPointer = GetHeldItemPointer(  );
  if ( DropItemPointer == NULL )
    {
      DebugPrintf( 0 , "\nvoid HeldItemUsageRequirementsMet ( void ) : No item in inventory seems to be currently held in hand...");
      return ( FALSE ) ;
    } 
  
  return ( ItemUsageRequirementsMet ( DropItemPointer , TRUE ) );
};

/* ----------------------------------------------------------------------
 * This function installs an item into a slot.  The given parameter is 
 * only the slot where this item should be installed.  The source item
 * will be found out from inside this function.  Very convenient.
 * ---------------------------------------------------------------------- */
void
DropHeldItemToSlot ( item* SlotItem )
{
  item* DropItemPointer;

  // --------------------
  // First we find out which item we want to drop into the weapon slot
  //
  DropItemPointer = GetHeldItemPointer(  );
  if ( DropItemPointer == NULL )
    {
      DebugPrintf( 0 , "\nvoid DropHeldItemToSlot ( void ) : No item in inventory seems to be currently held in hand...");
      return;
    } 

  //--------------------
  // If there is an old item in the slot, we make a held item on the
  // floor out of it and also set the HeldItemType accordingly, so that
  // after the new item was placed successfully, the old item will
  // be out of all inventory slots, but still in the hand of the 
  // player and ready to be put somewhere else
  //
  // But this may only be done of course, if the 'old item' is not
  // the item we want to put there itself!!!!  HAHAHAHA!!!!
  //
  if ( ( SlotItem->type != (-1) ) &&
       ( SlotItem->currently_held_in_hand == FALSE ) )
    MakeHeldFloorItemOutOf( SlotItem );

  //--------------------
  // Now the item is installed into the weapon slot of the influencer
  
  CopyItem( DropItemPointer , SlotItem , TRUE );
  SlotItem->currently_held_in_hand = FALSE;

  // Now the item is removed from the source location and no longer held in hand as well, 
  // but of course only if it is not the same as the original item
  if ( DropItemPointer != SlotItem )
    DeleteItem( DropItemPointer );

}; // void DropHeldItemToSlot ( item* SlotItem )

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
  int i;
  item Temp;

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
      CopyItem( DropItemPointer , &( Me.Inventory[ FreeInvIndex ] ) , TRUE );
      DeleteItem( DropItemPointer );
      Me.Inventory[ FreeInvIndex ].inventory_position.x = GetInventorySquare_x ( CurPos.x ) ;
      Me.Inventory[ FreeInvIndex ].inventory_position.y = GetInventorySquare_y ( CurPos.y ) ;
      Me.Inventory[ FreeInvIndex ].currently_held_in_hand = FALSE;

      // --------------------
      // Now that we know that the item could be dropped directly to inventory 
      // without swapping any paces, we can as well make the item
      // 'not held in hand' immediately and return
      //
      DropItemPointer->currently_held_in_hand = FALSE ;
      Item_Held_In_Hand = ( -1 );
      return;
    }
  else
    {
      //--------------------
      // So the item could not be placed into inventory directly, but maybe
      // it can be placed there if we swap our dropitem with some other item.
      // Let's test this opportunity here.
      //
      for ( i = 0 ; i < MAX_ITEMS_PER_LEVEL ; i ++ )
	{
	  CopyItem ( & ( Me.Inventory[ i ] ) , & ( Temp ) , FALSE );
	  
	  //--------------------
	  // FIRST: Security check against segfaults:  It might happen that we 
	  // delete the Dropitem itself while trying several items as candidates
	  // for removal.  This would cause testing dropability with a -1 item
	  // type and a SEGFAULT would result...
	  //
	  if ( & ( Me.Inventory[ i ] ) == DropItemPointer ) continue;



	  Me.Inventory[ i ].type = ( -1 ) ;

	  if ( ItemCanBeDroppedInInv ( DropItemPointer->type , GetInventorySquare_x ( CurPos.x ) , 
				       GetInventorySquare_y ( CurPos.y ) ) )
	    {
	      //--------------------
	      // So if with the removed item Nr. i putting of the DropItem is 
	      // suddenly possible, then we make a held item on the floor out
	      // of it.  The other removed item can stay removed, since it will
	      // be overwritten anyway and a copy is now on the floor.
	      Item_Held_In_Hand = ItemMap[ Temp.type ].picture_number;
	      MakeHeldFloorItemOutOf( &( Temp ) );

	      //--------------------
	      // Otherwise we just need to add the new item for the inventory
	      // grid as usual
	      CopyItem( DropItemPointer , &( Me.Inventory[ FreeInvIndex ] ) , TRUE );
	      DeleteItem( DropItemPointer );
	      Me.Inventory[ FreeInvIndex ].inventory_position.x = GetInventorySquare_x ( CurPos.x ) ;
	      Me.Inventory[ FreeInvIndex ].inventory_position.y = GetInventorySquare_y ( CurPos.y ) ;
	      Me.Inventory[ FreeInvIndex ].currently_held_in_hand = FALSE;

	      // And of course the item is no longer held in hand as well
	      DropItemPointer->currently_held_in_hand = FALSE ;
	      return;
	    }


	  //--------------------
	  // But if even the removal of one item was not enough, so that the new
	  // item would fit into the inventory, then of course we should re-add the
	  // removed item to the inventory, so that no other items get lost.
	  //
	  CopyItem ( & ( Temp ) , & ( Me.Inventory[ i ] ) , FALSE );

	} // for: try all items if removal is the solution
    } // if not immediately place findable

  // --------------------
  // So at this point we know, that even the removal of other items was not the 
  // solution.  So the item cannot be put into inventory, even at best attampts
  // to do so.  What a pitty.
  
  Item_Held_In_Hand = ItemMap[ GetHeldItemCode( ) ].picture_number;

}; // void DropHeldItemToInventory( void )


/* ----------------------------------------------------------------------
 * This function display the inventory screen and also checks for mouse
 * actions in the invenotry screen.
 * ---------------------------------------------------------------------- */
void 
ManageInventoryScreen ( void )
{
  // SDL_Rect TargetRect;
  static int MouseButtonPressedPreviousFrame = FALSE;
  static int RightPressedPreviousFrame = FALSE;
  point CurPos;
  point Inv_GrabLoc;
  int Grabbed_InvPos;
  int i;
  finepoint MapPositionOfMouse;
  // static int Item_Grabbed = FALSE;

  DebugPrintf (2, "\nvoid ShowInventoryMessages( ... ): Function call confirmed.");

  DebugPrintf ( INVENTORY_MOUSE_BUTTON_DEBUG , "\nRight: %d Left: %d Held: %d ", 
		RightPressedPreviousFrame , MouseButtonPressedPreviousFrame , Item_Held_In_Hand );


  // --------------------
  // We will need the current mouse position on several spots...
  //
  CurPos.x = GetMousePos_x() + 16;
  CurPos.y = GetMousePos_y() + 16;

  //--------------------
  // If the log is not set to visible right now, we do not need to 
  // do anything more, but to see if there is an object directly under
  // the mouse cursor, and in case this is so, we just try to add it
  // to inventory instead of giving it into the players 'hand' for
  // drag and drop and individual placing.
  //
  if ( GameConfig.Inventory_Visible == FALSE ) 
    {
      // DebugPrintf( 2 , "\nINVENTORY NOT VISIBLE!!" );
      if ( ( axis_is_active ) && ( !MouseButtonPressedPreviousFrame ) && ( Item_Held_In_Hand == (-1) ) )
	{
	  // DebugPrintf( 1 , "\nCollecting items for direct addition to the invenotry without grabbing." );
	  MapPositionOfMouse.x = Me.pos.x + (CurPos.x - UserCenter_x) / (float) Block_Width;
	  MapPositionOfMouse.y = Me.pos.y + (CurPos.y - UserCenter_y) / (float) Block_Height;

	  // We only take items, when they are close enough 
	  if ( ( fabsf( MapPositionOfMouse.x - Me.pos.x ) < ITEM_TAKE_DIST ) &&
	       ( fabsf( MapPositionOfMouse.y - Me.pos.y ) < ITEM_TAKE_DIST ) )
	    {
	      // DebugPrintf( 1  , "\nMouse in map at: %f %f." , MapPositionOfMouse.x , MapPositionOfMouse.y );
	      for ( i = 0 ; i < MAX_ITEMS_PER_LEVEL ; i++ )
		{
		  if ( CurLevel->ItemList[ i ].type == (-1) ) continue;
		  
		  if ( ( fabsf( MapPositionOfMouse.x - CurLevel->ItemList[ i ].pos.x ) < 0.5 ) &&
		       ( fabsf( MapPositionOfMouse.y - CurLevel->ItemList[ i ].pos.y ) < 0.5 ) )
		    {
		      //--------------------
		      // We've found some item to grab!!! How wonderful!!!
		      // We simply add it to the inventory as good as we can.
		      //
		      Item_Held_In_Hand = ( -1 ); // ItemMap[ CurLevel->ItemList[ i ].type ].picture_number ;
		      // CurLevel->ItemList[ i ].currently_held_in_hand = TRUE;
		      AddFloorItemDirectlyToInventory( &( CurLevel->ItemList[ i ] ) );
		      
		      MouseButtonPressedPreviousFrame = axis_is_active;
		      RightPressedPreviousFrame = MouseRightPressed ( ) ;
		      return;
		    }
		}
	    }
	}

      // In case of no inventory visible, we need not do anything more...
      MouseButtonPressedPreviousFrame = axis_is_active;
      RightPressedPreviousFrame = MouseRightPressed ( ) ;
      return;
    }


  //--------------------
  // Next we display all the inventory screen and we also fill in all
  // the pictures for the items the influencer is currently fitted with.
  //
  ShowInventoryScreen();

  //--------------------
  // If the user now presses the left mouse button and it was not pressed before,
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
	  if ( Me.weapon_item.type > 0 )
	    {
	      //--------------------
	      // At this point we know, that we have just grabbed something from the weapon rect
	      // So we set, that something should be displayed in the 'hand', and it should of
	      // course be the image of the item grabbed from inventory.
	      //
	      Item_Held_In_Hand = ItemMap[ Me.weapon_item.type ].picture_number ;
	      Me.weapon_item.currently_held_in_hand = TRUE;
	    }
	}
      else if ( CursorIsInDriveRect( CurPos.x , CurPos.y ) )
	{
	  DebugPrintf( 0 , "\nGrabbing in drive rect!" );
	  if ( Me.drive_item.type > 0 )
	    {
	      //--------------------
	      // At this point we know, that we have just grabbed something from the weapon rect
	      // So we set, that something should be displayed in the 'hand', and it should of
	      // course be the image of the item grabbed from inventory.
	      //
	      Item_Held_In_Hand = ItemMap[ Me.drive_item.type ].picture_number ;
	      Me.drive_item.currently_held_in_hand = TRUE;
	    }
	}
      else if ( CursorIsInShieldRect( CurPos.x , CurPos.y ) )
	{
	  DebugPrintf( 0 , "\nGrabbing in shield rect!" );
	  if ( Me.shield_item.type > 0 )
	    {
	      //--------------------
	      // At this point we know, that we have just grabbed something from the weapon rect
	      // So we set, that something should be displayed in the 'hand', and it should of
	      // course be the image of the item grabbed from inventory.
	      //
	      Item_Held_In_Hand = ItemMap[ Me.shield_item.type ].picture_number ;
	      Me.shield_item.currently_held_in_hand = TRUE;
	    }
	}
      else if ( CursorIsInArmourRect( CurPos.x , CurPos.y ) )
	{
	  DebugPrintf( 0 , "\nGrabbing in armour rect!" );
	  if ( Me.armour_item.type > 0 )
	    {
	      //--------------------
	      // At this point we know, that we have just grabbed something from the weapon rect
	      // So we set, that something should be displayed in the 'hand', and it should of
	      // course be the image of the item grabbed from inventory.
	      //
	      Item_Held_In_Hand = ItemMap[ Me.armour_item.type ].picture_number ;
	      Me.armour_item.currently_held_in_hand = TRUE;
	    }
	}
      else if ( CursorIsInSpecialRect( CurPos.x , CurPos.y ) )
	{
	  DebugPrintf( 0 , "\nGrabbing in special rect!" );
	  if ( Me.special_item.type > 0 )
	    {
	      //--------------------
	      // At this point we know, that we have just grabbed something from the weapon rect
	      // So we set, that something should be displayed in the 'hand', and it should of
	      // course be the image of the item grabbed from inventory.
	      //
	      Item_Held_In_Hand = ItemMap[ Me.special_item.type ].picture_number ;
	      Me.special_item.currently_held_in_hand = TRUE;
	    }
	}
      else if ( CursorIsInAux1Rect( CurPos.x , CurPos.y ) )
	{
	  DebugPrintf( 0 , "\nGrabbing in aux1 rect!" );
	  if ( Me.aux1_item.type > 0 )
	    {
	      //--------------------
	      // At this point we know, that we have just grabbed something from the weapon rect
	      // So we set, that something should be displayed in the 'hand', and it should of
	      // course be the image of the item grabbed from inventory.
	      //
	      Item_Held_In_Hand = ItemMap[ Me.aux1_item.type ].picture_number ;
	      Me.aux1_item.currently_held_in_hand = TRUE;
	    }
	}
      else if ( CursorIsInAux2Rect( CurPos.x , CurPos.y ) )
	{
	  DebugPrintf( 0 , "\nGrabbing in aux1 rect!" );
	  if ( Me.aux2_item.type > 0 )
	    {
	      //--------------------
	      // At this point we know, that we have just grabbed something from the weapon rect
	      // So we set, that something should be displayed in the 'hand', and it should of
	      // course be the image of the item grabbed from inventory.
	      //
	      Item_Held_In_Hand = ItemMap[ Me.aux2_item.type ].picture_number ;
	      Me.aux2_item.currently_held_in_hand = TRUE;
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
	  Item_Held_In_Hand = ( -1 );
	  DropHeldItemToInventory( );
	}

      //--------------------
      // If the cursor is in the user_rect, i.e. the combat window, then
      // the item should be dropped onto the players current location
      //
      if ( CursorIsInUserRect ( CurPos.x , CurPos.y ) )
	{
	  DebugPrintf( 0 , "\nItem dropped onto the floor of the combat window!" );
	  Item_Held_In_Hand = ( -1 );
	  DropHeldItemToTheFloor( );
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
	      if ( HeldItemUsageRequirementsMet(  ) )
		{
		  Item_Held_In_Hand = ( -1 );
		  // DropHeldItemToWeaponSlot ( );
		  DropHeldItemToSlot ( & ( Me.weapon_item ) );
		}
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
	      if ( HeldItemUsageRequirementsMet(  ) )
		{
		  Item_Held_In_Hand = ( -1 );
		  // DropHeldItemToDriveSlot ( );
		  DropHeldItemToSlot ( & ( Me.drive_item ) );
		}
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
	      if ( HeldItemUsageRequirementsMet(  ) )
		{
		  Item_Held_In_Hand = ( -1 );
		  // DropHeldItemToArmourSlot ( );
		  DropHeldItemToSlot ( & ( Me.armour_item ) );
		}
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
	      if ( HeldItemUsageRequirementsMet(  ) )
		{
		  Item_Held_In_Hand = ( -1 );
		  // DropHeldItemToShieldSlot ( );
		  DropHeldItemToSlot ( & ( Me.shield_item ) );
		}
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
	      if ( HeldItemUsageRequirementsMet(  ) )
		{
		  Item_Held_In_Hand = ( -1 );
		  // DropHeldItemToSpecialSlot ( );
		  DropHeldItemToSlot ( & ( Me.special_item ) );
		}
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
	      if ( HeldItemUsageRequirementsMet(  ) )
		{
		  Item_Held_In_Hand = ( -1 );
		  // DropHeldItemToAux1Slot ( );
		  DropHeldItemToSlot ( & ( Me.aux1_item ) );
		}
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
	      if ( HeldItemUsageRequirementsMet(  ) )
		{
		  Item_Held_In_Hand = ( -1 );
		  // DropHeldItemToAux2Slot ( );
		  DropHeldItemToSlot ( & ( Me.aux2_item ) );
		}
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
  // Maybe the user is just pressing the RIGHT mouse button inside the inventory recatangle
  // which would mean for us that he is applying the item under the mouse button
  //
  
  if ( MouseRightPressed ( ) && ( !RightPressedPreviousFrame ) )
    {
      if ( CursorIsInInventoryGrid( CurPos.x , CurPos.y ) )
	{
	  Inv_GrabLoc.x = GetInventorySquare_x ( CurPos.x );
	  Inv_GrabLoc.y = GetInventorySquare_y ( CurPos.y );

	  DebugPrintf( 0 , "\nApplying item at inv-pos: %d %d." , Inv_GrabLoc.x , Inv_GrabLoc.y );
	      
	  Grabbed_InvPos = GetInventoryItemAt ( Inv_GrabLoc.x , Inv_GrabLoc.y );
	  DebugPrintf( 0 , "\nApplying inventory entry no.: %d." , Grabbed_InvPos );

	  if ( Grabbed_InvPos == (-1) )
	    {
	      // Nothing grabbed, so we need not do anything more here..
	      DebugPrintf( 0 , "\nApplying in INVENTORY grid FAILED:  NO ITEM AT THIS POSITION FOUND!" );
	    }
	  else
	    {
	      //--------------------
	      // At this point we know, that we have just applied something from the inventory
	      //
	      ApplyItem( & ( Me.Inventory[ Grabbed_InvPos ] ) );
	    }
	}
    }

  //--------------------
  // Finally, we want the part of the screen we have been editing to become
  // visible and therefore we must updated it here, since it is currently not
  // contained within the user rectangle that also gets updated every frame.
  //
  // SDL_UpdateRect( Screen , InventoryRect.x , InventoryRect.y , InventoryRect.w , InventoryRect.h );


  MouseButtonPressedPreviousFrame = axis_is_active;
  RightPressedPreviousFrame = MouseRightPressed ( ) ;
}; // void ManageInventoryScreen ( void );

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void 
AddFloorItemDirectlyToInventory( item* ItemPointer )
{
  int InvPos;
  int item_height;
  int item_width;
  grob_point Inv_Loc;
  char TempText[1000];

  //--------------------
  // In case we found an item on the floor, we remove it from the floor
  // and add it to influs inventory
  //
  if ( ItemPointer != NULL )
    {
      //--------------------
      // In the special case of money, we add the amount of money to our
      // money counter and eliminate the item on the floor.
      //
      if ( ItemPointer->type == ITEM_MONEY )
	{
	  PlayItemSound( ItemMap[ ItemPointer->type ].sound_number );
	  Me.Gold += ItemPointer->gold_amount;
	  DeleteItem( ItemPointer );
	  return;
	}



      // find a free position in the inventory list
      for ( InvPos = 0 ; InvPos < MAX_ITEMS_IN_INVENTORY ; InvPos++ )
	{
	  if ( Me.Inventory [ InvPos ].type == (-1) ) break;
	}
      

      // find enough free squares in the inventory to fit
      for ( Inv_Loc.y = 0; Inv_Loc.y < InventorySize.y - ItemImageList[ ItemMap[ ItemPointer->type ].picture_number ].inv_size.y + 1 ; Inv_Loc.y ++ )
	{
	  for ( Inv_Loc.x = 0; Inv_Loc.x < InventorySize.x - ItemImageList[ ItemMap[ ItemPointer->type ].picture_number ].inv_size.x + 1 ; Inv_Loc.x ++ )
	    {
	      
	      for ( item_height = 0 ; item_height < ItemImageList[ ItemMap[ ItemPointer->type ].picture_number ].inv_size.y ; item_height ++ )
		{
		  for ( item_width = 0 ; item_width < ItemImageList[ ItemMap[ ItemPointer->type ].picture_number ].inv_size.x ; item_width ++ )
		    {
		      printf( "\nChecking pos: %d %d " , Inv_Loc.x + item_width , Inv_Loc.y + item_height );
		      if ( !Inv_Pos_Is_Free( Inv_Loc.x + item_width , 
					     Inv_Loc.y + item_height ) )
			{
			  Me.Inventory[ InvPos ].inventory_position.x = -1;
			  Me.Inventory[ InvPos ].inventory_position.y = -1;
			  goto This_Is_No_Possible_Location;
			}
		    }
		}
	      // if ( !Inv_Pos_Is_Free( Inv_Loc.x , Inv_Loc.y ) ) continue;
	      
	      // At this point we know we have reached a position where we can plant this item.
	      Me.Inventory[ InvPos ].inventory_position.x = Inv_Loc.x;
	      Me.Inventory[ InvPos ].inventory_position.y = Inv_Loc.y;
	      DebugPrintf( 0 , "FINE INVENTORY POSITION FOUND!!");
	      goto Inv_Loc_Found;
	      
	    This_Is_No_Possible_Location:
	      
	    }
	  
	}
      
    Inv_Loc_Found:
      
      if ( ( InvPos == MAX_ITEMS_IN_INVENTORY ) || ( Me.Inventory[ InvPos ].inventory_position.x == (-1) ) )
	{
	  Me.TextVisibleTime = 0;
	  Me.TextToBeDisplayed = "I can't carry any more.";
	  CantCarrySound();
	  // can't take any more items,
	}
      else
	{
	  // We announce that we have taken the item
	  Me.TextVisibleTime = 0;
	  sprintf( TempText , "Item taken: %s." , ItemMap[ ItemPointer->type ].ItemName );
	  Me.TextToBeDisplayed=MyMalloc( strlen( TempText ) + 1 );
	  strcpy ( Me.TextToBeDisplayed , TempText );
	  
	  // We add the new item to the inventory
	  CopyItem( ItemPointer , & ( Me.Inventory[ InvPos ] ) , FALSE );
	  Me.Inventory[ InvPos ].inventory_position.x = Inv_Loc.x;
	  Me.Inventory[ InvPos ].inventory_position.y = Inv_Loc.y;

	  // We make the sound of an item being taken
	  PlayItemSound( ItemMap[ ItemPointer->type ].sound_number );
	  //ItemTakenSound();

	  // ItemPointer->type = (-1);
	  DeleteItem( ItemPointer );
	}
    }
  
  
}; // void AddFloorItemDirectlyToInventory( item* ItemPointer )

#undef _items_c
