/* 
 *
 *   Copyright (c) 2002, 2003 Johannes Prix
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

#define ITEM_TAKE_DIST (1.2)

enum
  {
    WEAPON_SLOT = 0 ,
    SHIELD_SLOT , 
    SPECIAL_SLOT , 
    ARMOUR_SLOT , 
    AUX1_SLOT , 
    AUX2_SLOT , 
    DRIVE_SLOT ,
    FIRST_INV_SLOT 
  };

/*
 *
 * FIRST_INV_SLOT - FIRST_INV_SLOT + MAX_ITEMS_IN_INVENTORY: inventory of player
 * ?? perhaps afterwards: items on the floor of the current level?  maybe not.
 *
 */

/* ----------------------------------------------------------------------
 * When the player closes the inventory screen, items currently held in 
 * hand should not be held in hand any more.  This is what this function
 * should do:  It should make all items unheld by the player.
 * ---------------------------------------------------------------------- */
void
silently_unhold_all_items ( void )
{
  int i;
  Level ItemLevel = curShip . AllLevels [ Me [ 0 ] . pos . z ] ;

  //--------------------
  // At first we disable the picture at the mouse cursor location.  This
  // really is the picture only, nothing else so far.
  //
  Item_Held_In_Hand = ( -1 ); // ItemMap[ PlayerLevel->ItemList[ i ].type ].picture_number ;

  //--------------------
  // Now we remove all 'currently held' markers from all
  // items in inventory and in slots and also from all items on the floor.
  //
  for ( i = 0 ; i < MAX_ITEMS_PER_LEVEL ; i ++ )
    {
      ItemLevel -> ItemList [ i ] . currently_held_in_hand = FALSE ;
    }

  for ( i = 0 ; i < MAX_ITEMS_IN_INVENTORY ; i ++ )
    {
      Me [ 0 ] . Inventory [ i ] . currently_held_in_hand = FALSE ;
    }

  Me [ 0 ] . weapon_item . currently_held_in_hand = FALSE ;
  Me [ 0 ] . drive_item . currently_held_in_hand = FALSE ;
  Me [ 0 ] . armour_item . currently_held_in_hand = FALSE ;
  Me [ 0 ] . shield_item . currently_held_in_hand = FALSE ;
  Me [ 0 ] . special_item . currently_held_in_hand = FALSE ;
  Me [ 0 ] . aux1_item . currently_held_in_hand = FALSE ;
  Me [ 0 ] . aux2_item . currently_held_in_hand = FALSE ;

}; // void silently_unhold_all_items ( void )

/* ----------------------------------------------------------------------
 * For communication with the server, we can not use pointers to items.
 * That's a pity, cause it would be most convenient.  Instead we must 
 * think of something new and devise some unique item position codes.
 *
 * This item position code is a number that uniquely describes a position
 * in all the existing item lists.  It works as follows:
 *
 * 0: WEAPON_SLOT
 * 1: SHIELD_SLOT
 * 2: SPECIAL_SLOT
 * 3: AUX1_SLOT
 * 4: AUX2_SLOT
 * 5: DRIVE_SLOT
 * 6-5+MAX_ITEMS_IN_INVENTORY: inventory of player
 * ?? perhaps afterwards: items on the floor of the current level?  maybe not.
 *
 * ---------------------------------------------------------------------- */
int
GetPositionCode ( Item ItemPointer ) 
{
  int InvIndex;
  int levelnum;

  if ( & ( Me [ 0 ] . weapon_item ) == ItemPointer )
   {
      return WEAPON_SLOT ;
   }
 else if ( & ( Me [ 0 ] . shield_item ) == ItemPointer )
   {
      return SHIELD_SLOT;
   }
 else if ( & ( Me [ 0 ] . armour_item ) == ItemPointer )
   {
      return ARMOUR_SLOT;
   }
 else if ( & ( Me [ 0 ] . special_item ) == ItemPointer )
   {
      return SPECIAL_SLOT;
   }
 else if ( & ( Me [ 0 ] . aux1_item ) == ItemPointer )
   {
      return AUX1_SLOT;
   }
 else if ( & ( Me [ 0 ] . aux2_item ) == ItemPointer )
   {
      return AUX2_SLOT;
   }
 else if ( & ( Me [ 0 ] . drive_item ) == ItemPointer )
   {
      return DRIVE_SLOT;
   }
 else
   {
     for ( InvIndex = 0 ; InvIndex < MAX_ITEMS_IN_INVENTORY ; InvIndex ++ )
       {
	 if ( & ( Me [ 0 ] . Inventory [ InvIndex ] ) == ItemPointer )
	   {
	     DebugPrintf ( 0 , "\nPosition code taken from Inv Item Nr. %d. " , InvIndex );
	     return ( InvIndex + FIRST_INV_SLOT ) ;
	   }
       }
     for ( levelnum = 0 ; levelnum < curShip.num_levels ; levelnum ++ )
       {
	 for ( InvIndex = 0 ; InvIndex < MAX_ITEMS_PER_LEVEL ; InvIndex ++ )
	   {
	     if ( & ( curShip . AllLevels [ levelnum ] -> ItemList [ InvIndex ] ) ==  ItemPointer )
	       {
		 DebugPrintf ( 0 , "\nPosition code taken from Level Item Nr. %d. " , InvIndex );
		 return ( InvIndex + FIRST_INV_SLOT + MAX_ITEMS_IN_INVENTORY ) ;
	       }
	   }
       }
   }

  DebugPrintf ( 0 , "\nERROR! Unidentifiable pointer given to int GetPositionCode ( Item ). Terminating... " ) ;
  Terminate ( ERR ) ;

 return ( 0 ) ;

}; // int GetPositionCode ( Item ItemPointer ) 

/* ----------------------------------------------------------------------
 * This function does the home made item repair, i.e. the item repair via
 * the repair skill in contrast to the item repair via the shop, which of
 * course works much better.
 * ---------------------------------------------------------------------- */
void
HomeMadeItemRepair ( Item RepairItem ) 
{
  //--------------------
  // At this point we know, that we have just selected an item
  // for home-made repair.
  //
  if ( RepairItem->max_duration == (-1) )
    {
      PlayOnceNeededSoundSample ( "../effects/tux_ingame_comments/Tux_Item_Cant_Be_0.wav" , FALSE , FALSE );
    }
  else
    {
      if ( RepairItem -> max_duration <= 3 ) 
	{
	  PlayOnceNeededSoundSample ( "../effects/tux_ingame_comments/Tux_Item_Too_Fragile_0.wav" , FALSE , FALSE );
	}
      else
	{
	  RepairItem->max_duration *= 0.5 ;
	  RepairItem->current_duration = RepairItem->max_duration;
	  // Play_Shop_ItemRepairedSound( );
	  PlayOnceNeededSoundSample ( "../effects/tux_ingame_comments/Tux_This_Quick_Fix_0.wav" , FALSE , FALSE );
	}
    }
}; // void HomeMadeItemRepair ( Item RepairItem ) 

/* ----------------------------------------------------------------------
 * Same as GetPositionCode makes a position code out of a pointer, we now
 * want to it the other way around and find a pointer to a given position
 * code and player number.
 * ---------------------------------------------------------------------- */
Item
FindPointerToPositionCode ( int SlotCode , int PlayerNum ) 
{
  DebugPrintf ( 0 , "\nSlotCode received : %d . Player Number : %d. " , SlotCode , PlayerNum ) ;
  switch ( SlotCode )
    {
    case WEAPON_SLOT:
      DebugPrintf ( 0 , "\nItem Found! It's of type %d. It's from weapon slot. " , 
		    Me [ PlayerNum ] . weapon_item . type ) ;
      return ( & ( Me [ PlayerNum ] . weapon_item ) ) ;
      break;
    case SPECIAL_SLOT:
      DebugPrintf ( 0 , "\nItem Found! It's of type %d. It's from special slot. " , 
		    Me [ PlayerNum ] . special_item . type ) ;
      return ( & ( Me [ PlayerNum ] . special_item ) ) ;
      break;
    case SHIELD_SLOT:
      DebugPrintf ( 0 , "\nItem Found! It's of type %d. It's from shield slot. " , 
		    Me [ PlayerNum ] . shield_item . type ) ;
      return ( & ( Me [ PlayerNum ] . shield_item ) ) ;
      break;
    case ARMOUR_SLOT:
      DebugPrintf ( 0 , "\nItem Found! It's of type %d. It's from armour slot. " , 
		    Me [ PlayerNum ] . armour_item . type ) ;
      return ( & ( Me [ PlayerNum ] . armour_item ) ) ;
      break;
    case AUX1_SLOT:
      return ( & ( Me [ PlayerNum ] . aux1_item ) ) ;
      break;
    case AUX2_SLOT:
      return ( & ( Me [ PlayerNum ] . aux2_item ) ) ;
      break;
    case DRIVE_SLOT:
      return ( & ( Me [ PlayerNum ] . drive_item ) ) ;
      break;
    default:
      if ( SlotCode < FIRST_INV_SLOT + MAX_ITEMS_IN_INVENTORY )
	{
	  DebugPrintf ( 0 , "\nItem Found! It's of type %d. It's directly from inventory. " , 
			Me [ PlayerNum ] . Inventory [ SlotCode - FIRST_INV_SLOT ] . type ) ;
	  return ( & ( Me [ PlayerNum ] . Inventory [ SlotCode - FIRST_INV_SLOT ] ) ) ;
	}
      else if ( SlotCode < FIRST_INV_SLOT + MAX_ITEMS_IN_INVENTORY + MAX_ITEMS_PER_LEVEL )
	{
	  DebugPrintf ( 0 , "\nPosition code to find indicated level item Nr. %d. " , SlotCode - FIRST_INV_SLOT - MAX_ITEMS_IN_INVENTORY );
	  return ( & ( curShip . AllLevels [ Me [ PlayerNum ] . pos . z ] -> ItemList [ SlotCode - FIRST_INV_SLOT - MAX_ITEMS_IN_INVENTORY ] ) ) ;
	}
      else 
	{
	  DebugPrintf ( 0 , "\nERROR! Unidentifiable item slot code given to Item FindPointerToPositionCode ( int , int ). Terminating... " ) ;
	  Terminate ( ERR ) ;
	}
   }

  return NULL ;

}; // Item FindPointerToPositionCode ( PositionCode , PlayerNum ) 

/* ----------------------------------------------------------------------
 * This function calculates the price of a given item, taking into account
 * (*) the items base list price 
 * (*) the items given prefix modifier
 * (*) the items given suffix modifier
 * (*) AND THE CURRENT DURATION of the item in relation to its max duration.
 * ---------------------------------------------------------------------- */
long
calculate_item_buy_price ( item* BuyItem )
{
  float PrefixMultiplier = 1;
  float SuffixMultiplier = 1;
  float Multiplicity = BuyItem->multiplicity ;

  //--------------------
  // Maybe the item is magical in one way or the other.  Then we have to
  // multiply a factor to the price, no matter whether repairing or buying
  // or selling the item.
  //
  if ( BuyItem -> prefix_code != (-1) )
    {
      PrefixMultiplier = PrefixList[ BuyItem->prefix_code ].price_factor;
    }
  if ( BuyItem -> suffix_code != (-1) )
    SuffixMultiplier = SuffixList[ BuyItem->suffix_code ].price_factor;


  //--------------------
  // If the item is destructible, we take the current duration into
  // account, and reduce the item value accordingly...
  //
  if ( BuyItem->max_duration != (-1 ) )
    {
      return ( Multiplicity * ItemMap [ BuyItem->type ].base_list_price * SuffixMultiplier * PrefixMultiplier *
	       ( BuyItem->current_duration ) / BuyItem->max_duration ); 
    }
  else
    {
      return ( Multiplicity * ItemMap [ BuyItem->type ].base_list_price * SuffixMultiplier * PrefixMultiplier );
    }

  return 0; // just to make compilers happy (no warnings...)

}; // long calculate_item_buy_price ( item* BuyItem )

/* ----------------------------------------------------------------------
 * This function calculates the price of a given item, taking into account
 * (*) the items base list price 
 * (*) the items given prefix modifier
 * (*) the items given suffix modifier
 * (*) AND THE CURRENT DURATION of the item in relation to its max duration.
 * ---------------------------------------------------------------------- */
long
calculate_item_sell_price ( item* BuyItem )
{
  float PrefixMultiplier = 1;
  float SuffixMultiplier = 1;
  float Multiplicity = BuyItem -> multiplicity ;

  //--------------------
  // Maybe the item is magical in one way or the other.  Then we have to
  // multiply a factor to the price, no matter whether repairing or buying
  // or selling the item.
  //
  if ( BuyItem -> prefix_code != (-1) )
    {
      PrefixMultiplier = PrefixList [ BuyItem -> prefix_code ] . price_factor;
    }
  if ( BuyItem -> suffix_code != (-1) )
    SuffixMultiplier = SuffixList [ BuyItem -> suffix_code ] . price_factor;

  //--------------------
  // When selling an item, you don't get the full value of the item, but
  // instead, only half of the original list price, cause it's a used good.
  //
#define SELL_PRICE_FACTOR (0.5)
  PrefixMultiplier *= SELL_PRICE_FACTOR ;


  //--------------------
  // If the item is destructible, we take the current duration into
  // account, and reduce the item value accordingly...
  //
  if ( BuyItem->max_duration != (-1 ) )
    {
      return ( Multiplicity * ItemMap [ BuyItem->type ] . base_list_price * SuffixMultiplier * PrefixMultiplier *
	       ( BuyItem -> current_duration ) / BuyItem -> max_duration ); 
    }
  else
    {
      return ( Multiplicity * ItemMap [ BuyItem->type ] . base_list_price * SuffixMultiplier * PrefixMultiplier );
    }

  return 0; // just to make compilers happy (no warnings...)

}; // long calculate_item_sell_price ( item* BuyItem )

/* ----------------------------------------------------------------------
 * This function calculates the price of a given item, taking into account
 * (*) the items base list price 
 * (*) the items given prefix modifier
 * (*) the items given suffix modifier
 * (*) AND THE CURRENT DURATION of the item in relation to its max duration.
 * ---------------------------------------------------------------------- */
long
calculate_item_repair_price ( item* repair_item )
{
  float PrefixMultiplier = 1;
  float SuffixMultiplier = 1;
  float Multiplicity = repair_item->multiplicity ;

  //--------------------
  // Maybe the item is magical in one way or the other.  Then we have to
  // multiply a factor to the price, no matter whether repairing or buying
  // or selling the item.
  //
  if ( repair_item -> prefix_code != (-1) )
    {
      PrefixMultiplier = PrefixList [ repair_item -> prefix_code ] . price_factor;
    }
  if ( repair_item -> suffix_code != (-1) )
    SuffixMultiplier = SuffixList [ repair_item -> suffix_code ] . price_factor;

  //--------------------
  // For repair, it's not the full 'buy' cost...
  //
#define REPAIR_PRICE_FACTOR (0.5)
  PrefixMultiplier *= REPAIR_PRICE_FACTOR ;

  //--------------------
  // This is the price of the DAMAGE in the item, haha
  // This can only be requested for repair items
  //
  if ( repair_item->max_duration != (-1 ) )
    {
      return ( Multiplicity * ItemMap [ repair_item->type ].base_list_price * SuffixMultiplier * PrefixMultiplier *
	       ( repair_item -> max_duration - repair_item -> current_duration ) / repair_item -> max_duration ); 
    }
  else
    {
      DebugPrintf( 0 , "\n\nERROR!! CALCULATING REPAIR PRICE FOR INDESTRUCTIBLE ITEM!! \n\n Terminating..." );
      Terminate( ERR );
    }
  return 0;
}; // long calculate_item_repair_price ( item* repair_item )


/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
FillInItemProperties( item* ThisItem , int FullDuration , int TreasureChestRange )
{

  ThisItem -> damage = ItemMap[ ThisItem->type ].base_item_gun_damage;
  ThisItem -> damage_modifier = ItemMap[ ThisItem->type ].item_gun_damage_modifier;
  ThisItem -> ac_bonus = ItemMap[ ThisItem->type ].base_ac_bonus +
    MyRandom( ItemMap[ ThisItem->type ].ac_bonus_modifier );

  //--------------------
  // In case of cyberbucks, we have to specify the amount of cyberbucks
  //
  if ( ThisItem->type == ITEM_MONEY )
    {
      ThisItem->gold_amount = 20 * TreasureChestRange + MyRandom( 20 ) + 1;
    }
  ThisItem->multiplicity = 1;

  //--------------------
  // We now have to set a duration, as well a maximum duration
  // as well as a current duration, the later of which will be
  // a fraction of the maximum duration.
  //
  if ( ItemMap [ ThisItem->type ] . base_item_duration != (-1) )
    {
      ThisItem -> max_duration = ItemMap [ ThisItem -> type ] . base_item_duration +
	MyRandom ( ItemMap[ ThisItem->type ] . item_duration_modifier );
      if ( FullDuration ) ThisItem->current_duration = ThisItem->max_duration ;
      else ThisItem->current_duration = ThisItem->max_duration / 4 + MyRandom ( ThisItem -> max_duration / 2 ) ;
    }
  else
    {
      ThisItem->max_duration = ( -1 );
      ThisItem->current_duration = 1 ;
    }

  //--------------------
  // Now it's time to fill in the magic properties of the item.  We have
  // the basic ranges for the modifiers given in the prefix and suffix lists
  // and just need to create random values in the given ranges for the item.
  //
  ThisItem -> bonus_to_str = 0;
  ThisItem -> bonus_to_dex = 0;
  ThisItem -> bonus_to_mag = 0;
  ThisItem -> bonus_to_vit = 0;
  ThisItem -> bonus_to_all_attributes = 0;

  ThisItem -> bonus_to_life = 0;
  ThisItem -> bonus_to_force = 0;
  ThisItem -> bonus_to_tohit = 0;
  ThisItem -> bonus_to_ac_or_damage = 0;

  ThisItem -> bonus_to_resist_fire = 0;
  ThisItem -> bonus_to_resist_electricity = 0;
  ThisItem -> bonus_to_resist_disruptor = 0;

  ThisItem->throw_time = 0 ; 

  ThisItem->is_identified = TRUE;

  if ( ThisItem -> suffix_code != (-1) )
    {
      ThisItem->bonus_to_str += SuffixList [ ThisItem -> suffix_code ].base_bonus_to_str +
	MyRandom ( SuffixList [ ThisItem -> suffix_code ].modifier_to_bonus_to_str ) ;
      ThisItem->bonus_to_dex += SuffixList [ ThisItem -> suffix_code ].base_bonus_to_dex +
	MyRandom ( SuffixList [ ThisItem -> suffix_code ].modifier_to_bonus_to_dex ) ;
      ThisItem->bonus_to_mag += SuffixList [ ThisItem -> suffix_code ].base_bonus_to_mag +
	MyRandom ( SuffixList [ ThisItem -> suffix_code ].modifier_to_bonus_to_mag ) ;
      ThisItem->bonus_to_vit += SuffixList [ ThisItem -> suffix_code ].base_bonus_to_vit +
	MyRandom ( SuffixList [ ThisItem -> suffix_code ].modifier_to_bonus_to_vit ) ;
      ThisItem->bonus_to_all_attributes += SuffixList [ ThisItem -> suffix_code ].base_bonus_to_all_attributes +
	MyRandom ( SuffixList [ ThisItem -> suffix_code ].modifier_to_bonus_to_all_attributes ) ;

      ThisItem->bonus_to_life += SuffixList [ ThisItem -> suffix_code ].base_bonus_to_life +
	MyRandom ( SuffixList [ ThisItem -> suffix_code ].modifier_to_bonus_to_life ) ;
      ThisItem->bonus_to_force += SuffixList [ ThisItem -> suffix_code ].base_bonus_to_force +
	MyRandom ( SuffixList [ ThisItem -> suffix_code ].modifier_to_bonus_to_force ) ;

      ThisItem->bonus_to_tohit += SuffixList [ ThisItem -> suffix_code ].base_bonus_to_tohit +
	MyRandom ( SuffixList [ ThisItem -> suffix_code ].modifier_to_bonus_to_tohit ) ;
      ThisItem->bonus_to_ac_or_damage += SuffixList [ ThisItem -> suffix_code ].base_bonus_to_ac_or_damage +
	MyRandom ( SuffixList [ ThisItem -> suffix_code ].modifier_to_bonus_to_ac_or_damage ) ;

      ThisItem->bonus_to_resist_fire += SuffixList [ ThisItem -> suffix_code ].base_bonus_to_resist_fire +
	MyRandom ( SuffixList [ ThisItem -> suffix_code ].modifier_to_bonus_to_resist_fire ) ;
      ThisItem->bonus_to_resist_electricity += SuffixList [ ThisItem -> suffix_code ].base_bonus_to_resist_electricity +
	MyRandom ( SuffixList [ ThisItem -> suffix_code ].modifier_to_bonus_to_resist_electricity ) ;
      ThisItem->bonus_to_resist_disruptor += SuffixList [ ThisItem -> suffix_code ].base_bonus_to_resist_disruptor +
	MyRandom ( SuffixList [ ThisItem -> suffix_code ].modifier_to_bonus_to_resist_disruptor ) ;

      ThisItem->is_identified = FALSE;
    }
  if ( ThisItem -> prefix_code != (-1) )
    {
      ThisItem->bonus_to_str += PrefixList [ ThisItem -> prefix_code ].base_bonus_to_str +
	MyRandom ( PrefixList [ ThisItem -> prefix_code ].modifier_to_bonus_to_str ) ;
      ThisItem->bonus_to_dex += PrefixList [ ThisItem -> prefix_code ].base_bonus_to_dex +
	MyRandom ( PrefixList [ ThisItem -> prefix_code ].modifier_to_bonus_to_dex ) ;
      ThisItem->bonus_to_mag += PrefixList [ ThisItem -> prefix_code ].base_bonus_to_mag +
	MyRandom ( PrefixList [ ThisItem -> prefix_code ].modifier_to_bonus_to_mag ) ;
      ThisItem->bonus_to_vit += PrefixList [ ThisItem -> prefix_code ].base_bonus_to_vit +
	MyRandom ( PrefixList [ ThisItem -> prefix_code ].modifier_to_bonus_to_vit ) ;
      ThisItem->bonus_to_all_attributes += PrefixList [ ThisItem -> prefix_code ].base_bonus_to_all_attributes +
	MyRandom ( SuffixList [ ThisItem -> prefix_code ].modifier_to_bonus_to_all_attributes ) ;

      ThisItem->bonus_to_life += PrefixList [ ThisItem -> prefix_code ].base_bonus_to_life +
	MyRandom ( PrefixList [ ThisItem -> prefix_code ].modifier_to_bonus_to_life ) ;
      ThisItem->bonus_to_force += PrefixList [ ThisItem -> prefix_code ].base_bonus_to_force +
	MyRandom ( PrefixList [ ThisItem -> prefix_code ].modifier_to_bonus_to_force ) ;

      ThisItem->bonus_to_tohit += PrefixList [ ThisItem -> prefix_code ].base_bonus_to_tohit +
	MyRandom ( PrefixList [ ThisItem -> prefix_code ].modifier_to_bonus_to_tohit ) ;
      ThisItem->bonus_to_ac_or_damage += PrefixList [ ThisItem -> prefix_code ].base_bonus_to_ac_or_damage +
	MyRandom ( PrefixList [ ThisItem -> prefix_code ].modifier_to_bonus_to_ac_or_damage ) ;

      ThisItem->bonus_to_resist_fire += PrefixList [ ThisItem -> prefix_code ].base_bonus_to_resist_fire +
	MyRandom ( PrefixList [ ThisItem -> prefix_code ].modifier_to_bonus_to_resist_fire ) ;
      ThisItem->bonus_to_resist_electricity += PrefixList [ ThisItem -> prefix_code ].base_bonus_to_resist_electricity +
	MyRandom ( PrefixList [ ThisItem -> prefix_code ].modifier_to_bonus_to_resist_electricity ) ;
      ThisItem->bonus_to_resist_disruptor += PrefixList [ ThisItem -> prefix_code ].base_bonus_to_resist_disruptor +
	MyRandom ( PrefixList [ ThisItem -> prefix_code ].modifier_to_bonus_to_resist_disruptor ) ;

      ThisItem->is_identified = FALSE;
    }

}; // void FillInItemProperties( item* ThisItem , int FullDuration , int TreasureChestRange )

/* ----------------------------------------------------------------------
 * This function drops an item at a given place, assigning it the given
 * suffix and prefix code.
 * ---------------------------------------------------------------------- */
void
DropItemAt( int ItemType , float x , float y , int prefix , int suffix , int TreasureChestRange , int multiplicity )
{
  int i;

  //--------------------
  // If given a non-existent item type, we don't do anything
  // of course (and also don't produce a SEGFAULT or something...)
  //
  if ( ItemType == ( -1 ) ) return;
  if ( ItemType >= Number_Of_Item_Types ) 
    {
      fprintf ( stderr, "\n\nItemType: '%d'.\n" , ItemType );
      GiveStandardErrorMessage ( __FUNCTION__  , "\
There was an item code for an item to drop given to the function \n\
DropItemAt( ... ), which is pointing beyond the scope of the known\n\
item types.  This indicates a severe bug in Freedroid.",
				 PLEASE_INFORM, IS_FATAL );
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
  CurLevel -> ItemList [ i ] . type = ItemType;
  CurLevel -> ItemList [ i ] . pos . x = x;
  CurLevel -> ItemList [ i ] . pos . y = y;
  CurLevel -> ItemList [ i ] . prefix_code = prefix;
  CurLevel -> ItemList [ i ] . suffix_code = suffix;

  FillInItemProperties ( & ( CurLevel->ItemList[ i ] ) , FALSE, TreasureChestRange );

  CurLevel -> ItemList [ i ] . multiplicity = multiplicity ;
  CurLevel -> ItemList [ i ] . throw_time = 0.01 ; // something > 0 
  if ( ( prefix == (-1) ) && ( suffix == (-1) ) ) CurLevel -> ItemList [ i ] . is_identified = TRUE ;
  else CurLevel -> ItemList [ i ] . is_identified = FALSE ;

  // PlayItemSound( ItemMap[ ItemType ].sound_number );
  play_item_sound( ItemType );

}; // void DropItemAt( int ItemType , int x , int y , int prefix , int suffix , int TreasureChestRange )

/* ----------------------------------------------------------------------
 * This function drops an item at a given place, assigning it the given
 * suffix and prefix code.
 * ---------------------------------------------------------------------- */
void
DropChestItemAt( int ItemType , float x , float y , int prefix , int suffix , int TreasureChestRange )
{
    int i;

    //--------------------
    // If given a non-existent item type, we don't do anything
    // of course (and also don't produce a SEGFAULT or something...)
    //
    if ( ItemType == ( -1 ) ) return;
    if ( ItemType >= Number_Of_Item_Types ) 
    {
	fprintf ( stderr, "\n\nItemType: '%d'.\n" , ItemType );
	GiveStandardErrorMessage ( __FUNCTION__  , "\
There was an item code for an item to drop given to the function \n\
DropItemAt( ... ), which is pointing beyond the scope of the known\n\
item types.  This indicates a severe bug in Freedroid.",
				   PLEASE_INFORM, IS_FATAL );
    }
    
    //--------------------
    // At first we must find a free item index on this level,
    // so that we can enter the new item there.
    //
    for ( i = 0 ; i < MAX_ITEMS_PER_LEVEL ; i ++ )
    {
	if ( CurLevel->ChestItemList[ i ].type == (-1) ) 
	{
	    break;
	}
    }
    if ( i >= MAX_ITEMS_PER_LEVEL )
    {
	GiveStandardErrorMessage ( __FUNCTION__  , "\
No more items dropable into this level!!  Fatal (for now..).",
				   PLEASE_INFORM, IS_FATAL );
    }
    
    //--------------------
    // Now we can construct the new item
    //
    CurLevel->ChestItemList[ i ].type = ItemType;
    CurLevel->ChestItemList[ i ].pos.x = x;
    CurLevel->ChestItemList[ i ].pos.y = y;
    CurLevel->ChestItemList[ i ].prefix_code = prefix;
    CurLevel->ChestItemList[ i ].suffix_code = suffix;
    
    FillInItemProperties ( & ( CurLevel->ChestItemList[ i ] ) , FALSE , TreasureChestRange );
    
    // PlayItemSound( ItemMap[ ItemType ].sound_number );
    play_item_sound( ItemType );
    
}; // void DropChestItemAt( int ItemType , int x , int y , int prefix , int suffix , int TreasureChestRange )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
int
item_type_cannot_be_equipped ( int drop_item_type )
{

    if ( ItemMap [ drop_item_type ] . item_can_be_installed_in_weapon_slot ) return ( FALSE ) ;
    if ( ItemMap [ drop_item_type ] . item_can_be_installed_in_drive_slot ) return ( FALSE ) ;
    if ( ItemMap [ drop_item_type ] . item_can_be_installed_in_armour_slot ) return ( FALSE ) ;
    if ( ItemMap [ drop_item_type ] . item_can_be_installed_in_shield_slot ) return ( FALSE ) ;
    if ( ItemMap [ drop_item_type ] . item_can_be_installed_in_special_slot ) return ( FALSE ) ;
    if ( ItemMap [ drop_item_type ] . item_can_be_installed_in_aux_slot ) return ( FALSE ) ;
    
    return ( TRUE ) ;
}; // int item_type_cannot_be_equipped ( int drop_item_type )

/* ----------------------------------------------------------------------
 * When random treasure is dropped, we need to attach a suffix and a
 * prefix to the item in question.  However, for this it would be 
 * suitable to know the number of suffixes that are currently defined.
 * So this function takes a look at the suffix list, will search for the
 * termination entry in that list and then return the number of suffixes
 * available.
 * ---------------------------------------------------------------------- */
int
count_suffixes_available ( void )
{
    int i;

    for ( i = 0 ; TRUE ; i++ )
    {
	if ( ! strcmp ( SuffixList [ i ] . bonus_name , "*** END OF SUFFIX LIST ***" ) )
	{
	    DebugPrintf ( 1 , "\n%s(): End of Suffix list found at pos: %d.\n" , __FUNCTION__ , i );
	    return ( i ) ;
	}
    }

    GiveStandardErrorMessage ( __FUNCTION__  , "\
End of suffix list not found!  This isn't supposed to happen.",
			       PLEASE_INFORM, IS_FATAL );
    return ( 0 ) ;
}; // int count_suffixes_available ( void )

/* ----------------------------------------------------------------------
 * When random treasure is dropped, we need to attach a prefix and a
 * prefix to the item in question.  However, for this it would be 
 * suitable to know the number of prefixes that are currently defined.
 * So this function takes a look at the prefix list, will search for the
 * termination entry in that list and then return the number of prefixes
 * available.
 * ---------------------------------------------------------------------- */
int
count_prefixes_available ( void )
{
    int i;

    for ( i = 0 ; TRUE ; i++ )
    {
	if ( ! strcmp ( PrefixList [ i ] . bonus_name , "*** END OF PREFIX LIST ***" ) )
	{
	    DebugPrintf ( 1 , "\n%s(): End of Prefix list found at pos: %d.\n" , __FUNCTION__ , i );
	    return ( i ) ;
	}
    }

    GiveStandardErrorMessage ( __FUNCTION__  , "\
End of prefix list not found!  This isn't supposed to happen.",
			       PLEASE_INFORM, IS_FATAL );
    return ( 0 ) ;
}; // int count_prefixes_available ( void )

/* ----------------------------------------------------------------------
 * When random treasure is dropped, we need to attach a suffix and a
 * prefix to the item in question.  However, for this it would be 
 * suitable to know the number of suffixes and prefixes that are 
 * currently defined, but also the number of treasure chests available,
 * such that we can properly divide the range of suffixes and prefixes
 * to be equally distributed on the different treasure chests.  So this
 * function will take a look at the enemy archetypes and then find out
 * how many treasure chests are actually in use.
 * ---------------------------------------------------------------------- */
int
count_treasure_chests_in_use ( void )
{
    int i;
    int max_treasure_chest_in_use = 0 ;

    //--------------------
    // NOTE:  This function requires, that the Number_Of_Droid_Types is
    //        already known, i.e. the droid archetypes have been read in
    //        from the freedroid.droid_archetypes file already...
    //

    for ( i = 0 ; i < Number_Of_Droid_Types ; i ++ )
    {
	if ( Druidmap [ i ] . monster_level > max_treasure_chest_in_use )
	    max_treasure_chest_in_use = Druidmap [ i ] . monster_level ;
    }

    //--------------------
    // We return the number of the highest trease chest + 1 because the
    // treasure chests are numbered starting at 0, not at 1 
    //
    DebugPrintf ( 1 , "\n%s(): treasure chests total: %d.\n" , __FUNCTION__ , max_treasure_chest_in_use + 1 ) ;
    return ( max_treasure_chest_in_use + 1 );

}; // int count_treasure_chests_in_use ( void )

/* ----------------------------------------------------------------------
 * When a random item is being dropped, there is a chance of it being
 * magical.  In that case, there might be a modifying suffix attached to 
 * the item.  This function is supposed to find a suitable suffix for
 * the item in question.
 * ----------------------------------------------------------------------- */
int 
find_suitable_suffix_for_item ( int drop_item_type , int TreasureChestRange )
{
    int number_of_suffixes_available;
    int number_of_treasure_chests_in_use;
    int suffix_code_generated;

    //--------------------
    // First we catch the case of non-equipable items being magical.
    // This does not make sense and therefore is caught.
    //
    if ( item_type_cannot_be_equipped ( drop_item_type ) )
	return ( -1 );

    //--------------------
    // We check for the current database of treasure chests and
    // also suffixs...
    //
    number_of_suffixes_available = count_suffixes_available () ;
    number_of_treasure_chests_in_use = count_treasure_chests_in_use () ;
    
    //--------------------
    // Now we find a suitable modifier
    //
    suffix_code_generated = MyRandom ( TreasureChestRange * (  number_of_suffixes_available / number_of_treasure_chests_in_use ) ) ;
    if ( suffix_code_generated >= number_of_suffixes_available )
    {
	GiveStandardErrorMessage ( __FUNCTION__  , "\
Suffix code generation error!  Illegal suffix!  This isn't supposed to happen.",
				   PLEASE_INFORM, IS_FATAL );
    }
    return ( suffix_code_generated );

}; // int find_suitable_suffix_for_item ( int drop_item_type , int TreasureChestRange )

/* ----------------------------------------------------------------------
 * When a random item is being dropped, there is a chance of it being
 * magical.  In that case, there might be a modifying prefix attached to 
 * the item.  This function is supposed to find a suitable prefix for
 * the item in question.
 * ----------------------------------------------------------------------- */
int 
find_suitable_prefix_for_item ( int drop_item_type , int TreasureChestRange )
{
    int number_of_prefixes_available;
    int number_of_treasure_chests_in_use;
    int prefix_code_generated;

    //--------------------
    // First we catch the case of non-equipable items being magical.
    // This does not make sense and therefore is caught.
    //
    if ( item_type_cannot_be_equipped ( drop_item_type ) )
	return ( -1 );

    //--------------------
    // We check for the current database of treasure chests and
    // also prefixs...
    //
    number_of_prefixes_available = count_prefixes_available () ;
    number_of_treasure_chests_in_use = count_treasure_chests_in_use () ;
    
    //--------------------
    // Now we find a suitable modifier
    //
    prefix_code_generated = MyRandom ( TreasureChestRange * (  number_of_prefixes_available / number_of_treasure_chests_in_use ) ) ;
    if ( prefix_code_generated >= number_of_prefixes_available )
    {
	GiveStandardErrorMessage ( __FUNCTION__  , "\
Prefix code generation error!  Illegal prefix!  This isn't supposed to happen.",
				   PLEASE_INFORM, IS_FATAL );
    }
    return ( prefix_code_generated );

}; // int find_suitable_prefix_for_item ( int drop_item_type , int TreasureChestRange )

/* ----------------------------------------------------------------------
 * This function drops a random item to the floor of the current level
 * at position ( x , y ).
 *
 * The strategy in droping the item is that one can easily set up and
 * modify the tabe of items to be dropped.
 *
 * ---------------------------------------------------------------------- */
void
DropRandomItem( float x , float y , int TreasureChestRange , int ForceMagical , int ForceDrop , int ChestItem )
{
    int Suf; int Pre;
    int DropDecision;
    int drop_item_type = 1 ;
    int drop_item_multiplicity = 1 ;
    
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
    // Since we don't drop real treasure any more, we just 
    // drop some minimal stuff
    //
    if ( ChestItem ) 
    {
	switch ( MyRandom ( 4 ) )
	{
	    case 0:
		DropChestItemAt( ITEM_DROID_PART_1 , x , y , -1 , -1 , TreasureChestRange );
		break;
	    case 1:
		DropChestItemAt( ITEM_DROID_PART_2 , x , y , -1 , -1 , TreasureChestRange );
		break;
	    case 2:
		DropChestItemAt( ITEM_DROID_PART_3 , x , y , -1 , -1 , TreasureChestRange );
		break;
	    case 3:
		DropChestItemAt( ITEM_DROID_PART_4 , x , y , -1 , -1 , TreasureChestRange );
		break;
	    case 4:
		DropChestItemAt( ITEM_DROID_PART_5 , x , y , -1 , -1 , TreasureChestRange );
		break;
	}
	return;
    }
    
    //--------------------
    // Perhaps it's some gold that will be dropped.  That's rather
    // simple, so we do this first.
    //
    if ( ( !ForceDrop ) && ( DropDecision < 100 - ITEM_DROP_PERCENTAGE ) )
    {
	DropItemAt( ITEM_MONEY , x , y , -1 , -1 , TreasureChestRange , 1 );
	return;
    }
    
    switch ( MyRandom ( TreasureChestRange ) )
    {
	case 0:
	    switch ( MyRandom ( 14 ) )
	    {
		case 0:
		    drop_item_type = ITEM_TRACKS ;
		    drop_item_multiplicity =  1 ;
		    break;
		case 1:
		    drop_item_type = ITEM_SHORT_BOW ;
		    drop_item_multiplicity =  1 ;
		    break;
		case 2:
		    drop_item_type = ITEM_SMALL_HEALTH_POTION ;
		    drop_item_multiplicity =  1 ;
		    break;
		case 3:
		    drop_item_type = ITEM_SMALL_MANA_POTION ;
		    drop_item_multiplicity =  1 ;
		    break;
		case 4:
		    drop_item_type = ITEM_BUCKLER ;
		    drop_item_multiplicity =  1 ;
		    break;
		case 5:
		    drop_item_type = ITEM_SHORT_SWORD ;
		    drop_item_multiplicity =  1 ;
		    break;
		case 6:
		    drop_item_type = ITEM_DAGGER ;
		    drop_item_multiplicity =  1 ;
		    break;
		case 7:
		    drop_item_type = ITEM_CAP ;
		    drop_item_multiplicity =  1 ;
		    break;
		case 8:
		    drop_item_type = ITEM_STAFF ;
		    drop_item_multiplicity =  1 ;
		    break;
		case 9:
		    drop_item_type = ITEM_CLOAK ;
		    drop_item_multiplicity =  1 ;
		    break;
		case 10:
		    drop_item_type = ITEM_PLASMA_AMMUNITION ;
		    drop_item_multiplicity =  2 + MyRandom ( 9 ) ;
		    break;
		case 11:
		    drop_item_type = ITEM_LASER_AMMUNITION ;
		    drop_item_multiplicity =  2 + MyRandom ( 9 ) ;
		    break;
		case 12:
		    drop_item_type = ITEM_CLUB ;
		    drop_item_multiplicity =  1 ;
		    break;
		case 13:
		    drop_item_type = ITEM_SHOES ;
		    drop_item_multiplicity =  1 ;
		    break;
		case 14:
		    drop_item_type = ITEM_RUNNING_POWER_POTION ;
		    drop_item_multiplicity =  1 ;
		    break;
	    } // inner switch
	    break;
	case 1:
	    switch ( MyRandom ( 14 ) )
	    {
		case 0:
		    drop_item_type = ITEM_WHEELS ;
		    drop_item_multiplicity = 1 ;
		    break;
		case 1:
		    drop_item_type = ITEM_HUNTERS_BOW ;
		    drop_item_multiplicity =  1 ;
		    break;
		case 2:
		    drop_item_type = ITEM_MEDIUM_HEALTH_POTION ;
		    drop_item_multiplicity =  1 ;
		    break;
		case 3:
		    drop_item_type = ITEM_MEDIUM_MANA_POTION ;
		    drop_item_multiplicity =  1 ;
		    break;
		case 4:
		    drop_item_type = ITEM_SMALL_SHIELD ;
		    drop_item_multiplicity =  1 ;
		    break;
		case 5:
		    drop_item_type = ITEM_SCIMITAR ;
		    drop_item_multiplicity =  1 ;
		    break;
		case 6:
		    drop_item_type = ITEM_SMALL_HELM ;
		    drop_item_multiplicity =  1 ;
		    break;
		case 7:
		    drop_item_type = ITEM_ROBE ;
		    drop_item_multiplicity =  1 ;
		    break;
		case 8:
		    drop_item_type = ITEM_PLASMA_AMMUNITION ;
		    drop_item_multiplicity =  10 + MyRandom ( 9 ) ;
		    break;
		case 9:
		    drop_item_type = ITEM_LASER_AMMUNITION ;
		    drop_item_multiplicity =  10 + MyRandom ( 9 ) ;
		    break;
		case 10:
		    drop_item_type = ITEM_EXTERMINATOR_AMMUNITION ;
		    drop_item_multiplicity =  10 + MyRandom ( 9 ) ;
		    break;
		case 11:
		    drop_item_type = ITEM_MACE ;
		    drop_item_multiplicity = 1 ;
		    break;
		case 12:
		    drop_item_type = ITEM_BOOTS ;
		    drop_item_multiplicity =  1 ;
		    break;
		case 13:
		    drop_item_type = ITEM_SMALL_HEALTH_POTION ;
		    drop_item_multiplicity =  1 ;
		    break;
		case 14:
		    drop_item_type = ITEM_SMALL_MANA_POTION ;
		    drop_item_multiplicity =  1 ;
		    break;
	    } // inner switch
	    break;
	case 2:
	    switch ( MyRandom ( 16 ) )
	    {
		case 0:
		    drop_item_type = ITEM_TRIPEDAL ;
		    drop_item_multiplicity =  1 ;
		    break;
		case 1:
		    drop_item_type = ITEM_LONG_BOW ;
		    drop_item_multiplicity =  1 ;
		    break;
		case 2:
		    drop_item_type = ITEM_FULL_HEALTH_POTION ;
		    drop_item_multiplicity =  1 ;
		    break;
		case 3:
		    drop_item_type = ITEM_FULL_MANA_POTION ;
		    drop_item_multiplicity =  1 ;
		    break;
		case 4:
		    drop_item_type = ITEM_SMALL_SHIELD ;
		    drop_item_multiplicity =  1 ;
		    break;
		case 5:
		    drop_item_type = ITEM_SABER ;
		    drop_item_multiplicity =  1 ;
		    break;
		case 6:
		    drop_item_type = ITEM_IRON_HAT ;
		    drop_item_multiplicity =  1 ;
		    break;
		case 7:
		    drop_item_type = ITEM_QUILTED_ARMOUR ;
		    drop_item_multiplicity =  1 ;
		    break;
		case 8:
		    drop_item_type = ITEM_STANDARD_SHIELD ;
		    drop_item_multiplicity =  1 ;
		    break;
		case 9:
		    drop_item_type = ITEM_PLASMA_AMMUNITION ;
		    drop_item_multiplicity =  20 + MyRandom ( 9 );
		    break;
		case 10:
		    drop_item_type = ITEM_LASER_AMMUNITION ;
		    drop_item_multiplicity =  20 + MyRandom ( 9 ) ;
		    break;
		case 11:
		    drop_item_type = ITEM_EXTERMINATOR_AMMUNITION ;
		    drop_item_multiplicity =  20 + MyRandom ( 9 ) ;
		    break;
		case 12:
		    drop_item_type = ITEM_SMALL_HEALTH_POTION ;
		    drop_item_multiplicity =  1 ;
		    break;
		case 13:
		    drop_item_type = ITEM_SMALL_MANA_POTION ;
		    drop_item_multiplicity =  1 ;
		    break;
		case 14:
		    drop_item_type = ITEM_MEDIUM_HEALTH_POTION ;
		    drop_item_multiplicity =  1 ;
		    break;
		case 15:
		    drop_item_type = ITEM_MEDIUM_MANA_POTION ;
		    drop_item_multiplicity =  1 ;
		    break;
		case 16:
		    drop_item_type = ITEM_RUNNING_POWER_POTION ;
		    drop_item_multiplicity =  1 ;
		    break;
	    } // inner switch
	    break;
	case 3:
	    switch ( MyRandom ( 15 ) )
	    {
		case 0:
		    drop_item_type = ITEM_COMPOSITE_BOW ;
		    drop_item_multiplicity =  1 ;
		    break;
		case 1:
		    drop_item_type = ITEM_ANTIGRAV_POD ;
		    drop_item_multiplicity =  1 ;
		    break;
		case 2:
		    drop_item_type = ITEM_FULL_HEALTH_POTION ;
		    drop_item_multiplicity =  1 ;
		    break;
		case 3:
		    drop_item_type = ITEM_FULL_MANA_POTION ;
		    drop_item_multiplicity =  1 ;
		    break;
		case 4:
		    drop_item_type = ITEM_SMALL_SHIELD ;
		    drop_item_multiplicity =  1 ;
		    break;
		case 5:
		    drop_item_type = ITEM_FALCHION ;
		    drop_item_multiplicity =  1 ;
		    break;
		case 6:
		    drop_item_type = ITEM_IRON_HELM ;
		    drop_item_multiplicity =  1 ;
		    break;
		case 7:
		    drop_item_type = ITEM_LEATHER_ARMOUR ;
		    drop_item_multiplicity =  1 ;
		    break;
		case 8:
		    drop_item_type = ITEM_LARGE_SHIELD ;
		    drop_item_multiplicity =  1 ;
		    break;
		case 9:
		    drop_item_type = ITEM_PLASMA_AMMUNITION ;
		    drop_item_multiplicity = 30 + MyRandom ( 9 ) ;
		    break;
		case 10:
		    drop_item_type = ITEM_LASER_AMMUNITION ;
		    drop_item_multiplicity = 30 + MyRandom ( 9 ) ;
		    break;
		case 11:
		    drop_item_type = ITEM_EXTERMINATOR_AMMUNITION ;
		    drop_item_multiplicity = 30 + MyRandom ( 9 ) ;
		    break;
		case 12:
		    drop_item_type = ITEM_SMALL_HEALTH_POTION ;
		    drop_item_multiplicity =  1 ;
		    break;
		case 13:
		    drop_item_type = ITEM_SMALL_MANA_POTION ;
		    drop_item_multiplicity =  1 ;
		    break;
		case 14:
		    drop_item_type = ITEM_MEDIUM_HEALTH_POTION ;
		    drop_item_multiplicity =  1 ;
		    break;
		case 15:
		    drop_item_type = ITEM_MEDIUM_MANA_POTION ;
		    drop_item_multiplicity =  1 ;
		    break;
	    } // inner switch
	    break;
	default:
	    GiveStandardErrorMessage ( __FUNCTION__  , "\
Unhandled treasure chest encountered!  This isn't supposed to happen.",
				       PLEASE_INFORM, IS_FATAL );
	    break;
    } // switch
    
    //--------------------
    // So at this point we know, that an item will be dropped...
    //
    // Since there are no prefixes set up yet, we just need to consider
    // the suffixes.  In case 
    //
    Suf = ( -1 );
    Pre = ( -1 ) ;
    if ( ForceMagical || ( MyRandom ( 14 ) <= 2 ) )
    {
	Suf = find_suitable_suffix_for_item ( drop_item_type , TreasureChestRange );
    }
    if ( ForceMagical || ( MyRandom ( 14 ) <= 2 ) )
    {
	Pre = find_suitable_prefix_for_item ( drop_item_type , TreasureChestRange );
    }
    
    DropItemAt( drop_item_type , x , y , Pre , Suf , TreasureChestRange , drop_item_multiplicity );
    
}; // void DropRandomItem( int x , int y )

/* ----------------------------------------------------------------------
 * When the influencer gets hit, all of his equipment suffers some damage.
 * This is exactly what this function does:  apply the damage.
 * ---------------------------------------------------------------------- */
void 
DamageItem( item* CurItem )
{
  
  return;
  
  //--------------------
  // If the item mentioned as parameter exists and if it is of 
  // a destructable sort, then we apply the usual damage to it
  // (which is currently a bit high)
  //
  if ( ( CurItem->type != (-1) ) &&
       ( CurItem->max_duration != (-1) ) )
    {
      if ( ! CurItem->damage ) 
	CurItem->current_duration -= 0.001 * MyRandom( 100 ) ;
      else
	{
	  CurItem->current_duration -= 0.0003 * MyRandom( 100 ) ;
	  DebugPrintf ( 1 , "\nDamaged item seems to be a weapon... lower damage done." );
	}

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
DamageAllEquipment( int PlayerNum )
{

  // DamageItem ( & ( Me [ PlayerNum ] . weapon_item  ) );
  DamageItem ( & ( Me [ PlayerNum ] . armour_item  ) );
  DamageItem ( & ( Me [ PlayerNum ] . shield_item  ) );
  DamageItem ( & ( Me [ PlayerNum ] . drive_item   ) );
  DamageItem ( & ( Me [ PlayerNum ] . special_item ) );
  DamageItem ( & ( Me [ PlayerNum ] . aux1_item    ) );
  DamageItem ( & ( Me [ PlayerNum ] . aux2_item    ) );

}; // void DamageAllEquipment( void )

/* ----------------------------------------------------------------------
 * This function is used when an equipment EXCHANGE is performed, i.e.
 * one weapon equiped is replaced by a new item using the mouse.  This 
 * leads to an exchange in the items.  Yet, if the new item came from 
 * inventory, the old item can't be just put in the same place where the
 * new item was, cause it might be bigger.  So, attemting to solve the
 * problem, the old item from the slot can just be made into an item on
 * the floor, but not visible yet of course, cause it still gets the 
 * held in hand attribute.
 * ---------------------------------------------------------------------- */
void
MakeHeldFloorItemOutOf( item* SourceItem )
{
  int i;
  int SourceCode, DestCode;

  for ( i = 0 ; i < MAX_ITEMS_PER_LEVEL ; i ++ )
    {
      if ( CurLevel->ItemList [ i ] . type == ( -1 ) ) break;
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

  CurLevel->ItemList[ i ].pos.x = Me[0].pos.x;
  CurLevel->ItemList[ i ].pos.y = Me[0].pos.y;
  CurLevel->ItemList[ i ].currently_held_in_hand = TRUE;
  
  Item_Held_In_Hand = CurLevel -> ItemList [ i ] . type ;

  DeleteItem ( SourceItem ) ;

  //--------------------
  // And of course we shouldn't forget to tell the server about this 
  // movement as well....
  //
  if ( ClientMode && ! ServerMode ) 
    {
      SourceCode = GetPositionCode ( SourceItem ) ;
      DestCode = GetPositionCode ( & ( CurLevel -> ItemList [ i ] ) ) ;
      SendPlayerItemMoveToServer ( SourceCode , DestCode , -1 , -1 ) ;
    }
  
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
      if ( Me[0].Inventory[ InvPos ].currently_held_in_hand && ( Me[0].Inventory[ InvPos ].type > 0 ) ) 
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
item* 
GetHeldItemPointer( void )
{
  int InvIndex;
  int i;

  //--------------------
  // We must not access the levels item array, if the level was not yet
  // initialized!!! Or a SEGFAULT will occur!!!  Therefore we check for
  // ininitialized level first.!
  //
  if ( CurLevel == NULL ) 
    {
      DebugPrintf ( 0 , "\nERROR IN GetHeldItemPointer : CurLevel not yet initialized... " ) ;
      return ( NULL );
    }

  InvIndex = GetHeldItemInventoryIndex(  );

  if ( InvIndex != (-1) )
    {
      // DebugPrintf( 2 , "\nitem* GetHeldItemPointer( void ) : An item in inventory was held in hand.  Good.");
      return ( & ( Me[0].Inventory[ InvIndex ] ) );
    } 
  else if ( Me[0].weapon_item.currently_held_in_hand > 0 )
    {
      // DebugPrintf( 2 , "\nitem* GetHeldItemPointer( void ) : An item in weapon slot was held in hand.  Good.");
      return ( & ( Me[0].weapon_item ) );
    }
  else if ( Me[0].drive_item.currently_held_in_hand > 0 )
    {
      // DebugPrintf( 2 , "\nitem* GetHeldItemPointer( void ) : An item in weapon slot was held in hand.  Good.");
      return ( & ( Me[0].drive_item ) );
    }
  else if ( Me[0].shield_item.currently_held_in_hand > 0 )
    {
      // DebugPrintf( 2 , "\nitem* GetHeldItemPointer( void ) : An item in weapon slot was held in hand.  Good.");
      return ( & ( Me[0].shield_item ) );
    }
  else if ( Me[0].armour_item.currently_held_in_hand > 0 )
    {
      // DebugPrintf( 2 , "\nitem* GetHeldItemPointer( void ) : An item in weapon slot was held in hand.  Good.");
      return ( & ( Me[0].armour_item ) );
    }
  else if ( Me[0].special_item.currently_held_in_hand > 0 )
    {
      // DebugPrintf( 2 , "\nitem* GetHeldItemPointer( void ) : An item in weapon slot was held in hand.  Good.");
      return ( & ( Me[0].special_item ) );
    }
  else if ( Me[0].aux1_item.currently_held_in_hand > 0 )
    {
      // DebugPrintf( 2 , "\nitem* GetHeldItemPointer( void ) : An item in weapon slot was held in hand.  Good.");
      return ( & ( Me[0].aux1_item ) );
    }
  else if ( Me[0].aux2_item.currently_held_in_hand > 0 )
    {
      // DebugPrintf( 2 , "\nitem* GetHeldItemPointer( void ) : An item in weapon slot was held in hand.  Good.");
      return ( & ( Me[0].aux2_item ) );
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
DeleteItem ( item* Item )
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

  if ( MakeSound ) 
    {
      // PlayItemSound( ItemMap[ SourceItem->type ].sound_number );
      play_item_sound( SourceItem -> type );
    }

}; // void MoveItem( item* SourceItem , item* DestItem )

/* ----------------------------------------------------------------------
 * This function COPIES an item from the source location to the destination
 * location.  The source location is then marked as unused inventory 
 * entry.
 * ---------------------------------------------------------------------- */
void
CopyItemWithoutHeldProperty ( item* SourceItem , item* DestItem , int MakeSound ) 
{

  int temp = DestItem -> currently_held_in_hand ;
  CopyItem ( SourceItem , DestItem , MakeSound );
  DestItem -> currently_held_in_hand = temp;

}; // void CopyItemWithoutHeldProperty ( item* SourceItem , item* DestItem , int MakeSound ) 

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

  ApplyItem ( & ( Me[0].Inventory[ FoundItemNr ] ) );

}; // void Quick_ApplyItem( item* CurItem )

/* ----------------------------------------------------------------------
 * This function applies a given item (to the influencer) and maybe 
 * eliminates the item after that, if it's an item that gets used up.
 * ---------------------------------------------------------------------- */
void
ApplyItem( item* CurItem )
{
    DebugPrintf( 1 , "\n%s(): function call confirmed." , __FUNCTION__ );

    // If the inventory slot is not at all filled, we need not do anything more...
    if ( CurItem->type == (-1) ) return;
    
    if ( ItemMap[ CurItem->type ] . item_can_be_applied_in_combat == FALSE ) 
    {
	Me [ 0 ] . TextVisibleTime = 0;
	Me [ 0 ] . TextToBeDisplayed = "I can't use this item here.";
	return;
    }

    //--------------------
    // At this point we know that the item is applicable in combat situation
    // and therefore all we need to do from here on is execute the item effect
    // upon the influencer or his environment.
    //
    if ( CurItem->type == ITEM_BLUE_ENERGY_DRINK )
    {
	Me [ 0 ] . health += 15;
	Me [ 0 ] . energy += 15;
	Me [ 0 ] . mana += 15;
	Me [ 0 ] . running_power += 15;
    }
    if ( CurItem->type == ITEM_SMALL_HEALTH_POTION )
    {
	Me [ 0 ] . health += 25;
	Me [ 0 ] . energy += 25;
    }
    else if ( CurItem->type == ITEM_MEDIUM_HEALTH_POTION )
    {
	Me [ 0 ] . health += 50;
	Me [ 0 ] . energy += 50;
    }
    else if ( CurItem->type == ITEM_FULL_HEALTH_POTION )
    {
	Me[0].health += Me[0].maxenergy;
	Me[0].energy += Me[0].maxenergy;
    }
    else if ( CurItem->type == ITEM_SMALL_MANA_POTION )
    {
	Me[0].mana += 25;
    }
    else if ( CurItem->type == ITEM_MEDIUM_MANA_POTION )
    {
	Me[0].mana += 50;
    }
    else if ( CurItem->type == ITEM_FULL_MANA_POTION )
    {
	Me[0].mana += Me[0].maxmana;
    }
    else if ( CurItem->type == ITEM_RUNNING_POWER_POTION )
    {
	Me [ 0 ] . running_power = Me [ 0 ] . max_running_power;
	Me [ 0 ] . running_must_rest = FALSE ;
    }
    else if ( CurItem->type == ITEM_MAP_MAKER_SIMPLE )
    {
	Me [ 0 ] . map_maker_is_present = TRUE ;
	GameConfig . Automap_Visible = TRUE ;
	Play_Spell_ForceToEnergy_Sound( );
    }
    else if ( CurItem->type == ITEM_VMX_GAS_GRENADE )
    {
	RadialVMXWave ( Me [ 0 ] . pos , FALSE );
    }
    else if ( CurItem->type == ITEM_EMP_SHOCK_GRENADE )
    {
	RadialEMPWave ( Me [ 0 ] . pos , FALSE );
    }
    else if ( CurItem->type == ITEM_PLASMA_GRENADE )
    {
	RadialFireWave ( Me [ 0 ] . pos , FALSE );
    }
    else if ( CurItem->type == ITEM_STRENGTH_PILL )
    {
	Me [ 0 ] . base_strength ++ ;
    }
    else if ( CurItem->type == ITEM_DEXTERITY_PILL )
    {
	Me [ 0 ] . base_dexterity ++ ;
    }
    else if ( CurItem->type == ITEM_MAGIC_PILL )
    {
	Me [ 0 ] . base_magic ++ ;
    }
    else if ( CurItem->type == ITEM_SPELLBOOK_OF_HEALING )
    {
	Me [ 0 ] . SkillLevel [ SPELL_FORCE_TO_ENERGY ] ++ ;
	Me [ 0 ] . base_skill_level [ SPELL_FORCE_TO_ENERGY ] ++ ;
	Play_Spell_ForceToEnergy_Sound( );
    }
    else if ( CurItem->type == ITEM_SPELLBOOK_OF_EXPLOSION_CIRCLE )
    {
	Me [ 0 ] . SkillLevel [ SPELL_FORCE_EXPLOSION_CIRCLE ] ++ ;
	Me [ 0 ] . base_skill_level [ SPELL_FORCE_EXPLOSION_CIRCLE ] ++ ;
	Play_Spell_ForceToEnergy_Sound( );
    }
    else if ( CurItem->type == ITEM_SPELLBOOK_OF_EXPLOSION_RAY )
    {
	Me [ 0 ] . SkillLevel [ SPELL_FORCE_EXPLOSION_RAY ] ++ ;
	Me [ 0 ] . base_skill_level [ SPELL_FORCE_EXPLOSION_RAY ] ++ ;
	Play_Spell_ForceToEnergy_Sound( );
    }
    else if ( CurItem->type == ITEM_SPELLBOOK_OF_TELEPORT_HOME )
    {
	Me [ 0 ] . SkillLevel [ SPELL_TELEPORT_HOME ] ++ ;
	Me [ 0 ] . base_skill_level [ SPELL_TELEPORT_HOME ] ++ ;
	Play_Spell_ForceToEnergy_Sound( );
    }
    else if ( CurItem->type == ITEM_SPELLBOOK_OF_PLASMA_BOLT )
    {
	Me [ 0 ] . SkillLevel [ SPELL_FIREY_BOLT ] ++ ;
	Me [ 0 ] . base_skill_level [ SPELL_FIREY_BOLT ] ++ ;
	Play_Spell_ForceToEnergy_Sound( );
    }
    else if ( CurItem->type == ITEM_SPELLBOOK_OF_ICE_BOLT )
    {
	Me [ 0 ] . SkillLevel [ SPELL_COLD_BOLT ] ++ ;
	Me [ 0 ] . base_skill_level [ SPELL_COLD_BOLT ] ++ ;
	Play_Spell_ForceToEnergy_Sound( );
    }
    else if ( CurItem->type == ITEM_SPELLBOOK_OF_POISON_BOLT )
    {
	Me [ 0 ] . SkillLevel [ SPELL_POISON_BOLT ] ++ ;
	Me [ 0 ] . base_skill_level [ SPELL_POISON_BOLT ] ++ ;
	Play_Spell_ForceToEnergy_Sound( );
    }
    else if ( CurItem->type == ITEM_SPELLBOOK_OF_PETRIFICATION )
    {
	Me [ 0 ] . SkillLevel [ SPELL_PARALYZE_BOLT ] ++ ;
	Me [ 0 ] . base_skill_level [ SPELL_PARALYZE_BOLT ] ++ ;
	Play_Spell_ForceToEnergy_Sound( );
    }
    else if ( CurItem->type == ITEM_SPELLBOOK_OF_RADIAL_EMP_WAVE )
    {
	Me [ 0 ] . SkillLevel [ SPELL_RADIAL_EMP_WAVE ] ++ ;
	Me [ 0 ] . base_skill_level [ SPELL_RADIAL_EMP_WAVE ] ++ ;
	Play_Spell_ForceToEnergy_Sound( );
    }
    else if ( CurItem->type == ITEM_SPELLBOOK_OF_RADIAL_VMX_WAVE )
    {
	Me [ 0 ] . SkillLevel [ SPELL_RADIAL_VMX_WAVE ] ++ ;
	Me [ 0 ] . base_skill_level [ SPELL_RADIAL_VMX_WAVE ] ++ ;
	Play_Spell_ForceToEnergy_Sound( );
    }
    else if ( CurItem->type == ITEM_SPELLBOOK_OF_RADIAL_PLASMA_WAVE )
    {
	Me [ 0 ] . SkillLevel [ SPELL_RADIAL_FIRE_WAVE ] ++ ;
	Me [ 0 ] . base_skill_level [ SPELL_RADIAL_FIRE_WAVE ] ++ ;
	Play_Spell_ForceToEnergy_Sound( );
    }
    else if ( CurItem->type == ITEM_SPELLBOOK_OF_DETECT_ITEMS )
    {
	Me [ 0 ] . SkillLevel [ SPELL_DETECT_ITEM ] ++ ;
	Me [ 0 ] . base_skill_level [ SPELL_DETECT_ITEM ] ++ ;
	Play_Spell_ForceToEnergy_Sound( );
    }
    else if ( CurItem->type == ITEM_SPELLBOOK_OF_IDENTIFY )
    {
	Me [ 0 ] . SkillLevel [ SPELL_IDENTIFY_SKILL ] ++ ;
	Me [ 0 ] . base_skill_level [ SPELL_IDENTIFY_SKILL ] ++ ;
	Play_Spell_ForceToEnergy_Sound( );
    }
    
    if ( Me[0].energy > Me[0].maxenergy ) Me[0].energy = Me[0].maxenergy ;
    if ( Me[0].mana > Me[0].maxmana ) Me[0].mana = Me[0].maxmana ;
    
    // PlayItemSound( ItemMap[ CurItem->type ].sound_number );
    play_item_sound( CurItem -> type );
    
    //--------------------
    // In some cases the item concerned is a one-shot-device like a health potion, which should
    // evaporize after the first application.  Therefore we delete the item from the inventory list.
    //
    
    if ( CurItem->multiplicity > 1 )
	CurItem->multiplicity--;
    else DeleteItem ( CurItem );
    
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
  

  for ( i = 0 ; i < MAX_ITEMS_IN_INVENTORY -1 ; i++ )
    {
      if ( Me[0].Inventory[ i ].type == ( -1 ) ) continue;
      if ( Me[0].Inventory[ i ].currently_held_in_hand ) continue;

      // for ( item_height = 0 ; item_height < ItemSizeTable[ Me[0].Inventory[ i ].type ].y ; item_height ++ )
      for ( item_height = 0 ; item_height < ItemMap [ Me [ 0 ] . Inventory [ i ] . type ] . inv_image . inv_size . y ; item_height ++ )
	{
	  for ( item_width = 0 ; item_width < ItemMap [ Me [ 0 ] . Inventory [ i ] . type ] . inv_image . inv_size . x ; item_width ++ )
	    {
	      if ( ( ( Me[0].Inventory[ i ].inventory_position.x + item_width ) == x ) &&
		   ( ( Me[0].Inventory[ i ].inventory_position.y + item_height ) == y ) )
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
  
  for ( i = 0 ; i < MAX_ITEMS_IN_INVENTORY -1 ; i++ )
    {
      if ( Me[0].Inventory[ i ].type == ( -1 ) ) continue;

      for ( item_height = 0 ; item_height < ItemMap [ Me [ 0 ] . Inventory [ i ] . type ] . inv_image . inv_size . y ; item_height ++ )
	{
	  for ( item_width = 0 ; item_width < ItemMap [ Me [ 0 ] . Inventory [ i ] . type ] . inv_image . inv_size . x ; item_width ++ )
	    {
	      if ( ( ( Me[0].Inventory[ i ].inventory_position.x + item_width ) == x ) &&
		   ( ( Me[0].Inventory[ i ].inventory_position.y + item_height ) == y ) )
		{
		  return ( i );
		}
	    }
	}
    }
  return ( -1 ); // Nothing found at this grabbing location!!

}; // int GetInventoryItemAt ( int x , int y )

/* ----------------------------------------------------------------------
 *
 * Often, especially in dialogs and in order to determine if some answer
 * should be allowed for the Tux or not, it is important to know if the
 * Tux has some special item of a given type in inventory or not and also
 * how many of those items the Tux really has.  
 *
 * This function is now intended to count the number of items of a given
 * type in the inventory of the Player.
 *
 * ---------------------------------------------------------------------- */
int
CountItemtypeInInventory( int Itemtype , int PlayerNum )
{
  int i;
  int NumberOfItemsFound = 0 ;

  for ( i = 0 ; i < MAX_ITEMS_IN_INVENTORY ; i++ )
    {
      if ( Me [ PlayerNum ] . Inventory [ i ] . type == Itemtype ) NumberOfItemsFound++;
    }
  return NumberOfItemsFound;
}; // int CountItemtypeInInventory( int Itemtype , int PlayerNum )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
int
FindFirstInventoryIndexWithItemType ( int Itemtype , int PlayerNum )
{
  int i;

  for ( i = 0 ; i < MAX_ITEMS_IN_INVENTORY ; i++ )
    {
      if ( Me [ PlayerNum ] . Inventory [ i ] . type == Itemtype ) return ( i );
    }
  
  //--------------------
  // Severe error:  Item type NOT found in inventory!!!
  //
  fprintf ( stderr, "\n\nItemType: '%d'.\n" , Itemtype );
  GiveStandardErrorMessage ( __FUNCTION__  , "\
There was an item code for an item to locate in inventory, but inventory\n\
did not contain this item type at all!  This indicates a severe bug in Freedroid.",
			     PLEASE_INFORM, IS_FATAL );

  return ( -1 );
  
}; // int FindFirstInventoryIndexWithItemType ( ItemPointer->type , PLAYER_NR_0 )

/* ----------------------------------------------------------------------
 * At some point the Tux will hand over all his items of a given type
 * to a dialog partner.  This function is intended to do exactly this:
 * To remove all items of a given type from the inventory of a given 
 * player.
 * ---------------------------------------------------------------------- */
void
DeleteAllInventoryItemsOfType( int Itemtype , int PlayerNum )
{
  int i;
  for ( i = 0 ; i < MAX_ITEMS_IN_INVENTORY ; i++ )
    {
      if ( Me [ PlayerNum ] . Inventory [ i ] . type == Itemtype ) 
	DeleteItem ( & ( Me [ PlayerNum ] . Inventory [ i ] ) );
    }
}; // void DeleteAllInventoryItemsOfType( int Itemtype , int PlayerNum )

/* ----------------------------------------------------------------------
 * This deletes ONE item of the given type, like one bullet that has 
 * just been expended.
 * ---------------------------------------------------------------------- */
void
DeleteOneInventoryItemsOfType( int Itemtype , int PlayerNum )
{
  int i;
  for ( i = 0 ; i < MAX_ITEMS_IN_INVENTORY ; i++ )
    {
      if ( Me [ PlayerNum ] . Inventory [ i ] . type == Itemtype ) 
	{
	  if ( Me [ PlayerNum ] . Inventory [ i ] . multiplicity > 1 )  
	    Me [ PlayerNum ] . Inventory [ i ] . multiplicity--;
	  else DeleteItem ( & ( Me [ PlayerNum ] . Inventory [ i ] ) );
	  return;
	}
    }

  //--------------------
  // This point must never be reached or a severe error has occured...
  //
  fprintf ( stderr, "\n\nItemType: '%d'.\n" , Itemtype );
  GiveStandardErrorMessage ( __FUNCTION__  , "\
One single item of all the items of a given type in the Tux inventory\n\
should be removed, but there was not even one such item ever found in\n\
Tux invenrtory.  Something must have gone awry...",
			     PLEASE_INFORM, IS_FATAL );

}; // void DeleteOneInventoryItemsOfType( int Itemtype , int PlayerNum )

/* ----------------------------------------------------------------------
 * This function checks if a given screen position lies within the user
 * i.e. combat rectangle or not.
 * ---------------------------------------------------------------------- */
int 
MouseCursorIsInUserRect( int x , int y )
{
  if ( x > User_Rect.x + User_Rect.w ) return ( FALSE );
  if ( x < User_Rect.x ) return ( FALSE );
  if ( y > User_Rect.y + User_Rect.h ) return ( FALSE );
  if ( y < User_Rect.y ) return ( FALSE );
  return ( TRUE );
}; // int MouseCursorIsInUserRect( int x , int y )

/* ----------------------------------------------------------------------
 * This function checks if a given screen position lies within the user
 * i.e. combat rectangle or not.
 * ---------------------------------------------------------------------- */
int 
MouseCursorIsInInvRect( int x , int y )
{
  if ( x > InventoryRect.x + InventoryRect.w ) return ( FALSE );
  if ( x < InventoryRect.x ) return ( FALSE );
  if ( y > InventoryRect.y + InventoryRect.h ) return ( FALSE );
  if ( y < InventoryRect.y ) return ( FALSE );
  return ( TRUE );
}; // int MouseCursorIsInInvRect( int x , int y )

/* ----------------------------------------------------------------------
 * This function checks if a given screen position lies within the grid
 * where the inventory of the player is usually located or not.
 * ---------------------------------------------------------------------- */
int 
MouseCursorIsInInventoryGrid( int x , int y )
{
  point CurPos;

  CurPos.x = x ;
  CurPos.y = y ;

  if ( ( CurPos.x >= INVENTORY_RECT_X ) && ( CurPos.x <= INVENTORY_RECT_X + INVENTORY_GRID_WIDTH * INVENTORY_SUBSQUARE_WIDTH ) )
    {
      DebugPrintf( INVENTORY_RECTANGLE_DEBUG_LEVEL , "\nMight be grabbing in inventory, as far as x is concerned.");
      if ( ( CurPos.y >= User_Rect.y + INVENTORY_RECT_Y ) && 
	   ( CurPos.y <= User_Rect.y + INVENTORY_RECT_Y + INVENTORY_SUBSQUARE_WIDTH * INVENTORY_GRID_HEIGHT ) )
	{
	  DebugPrintf( INVENTORY_RECTANGLE_DEBUG_LEVEL , "\nMight be grabbing in inventory, as far as y is concerned.");
	  return( TRUE );
	}
    }
  return( FALSE );
}; // int MouseCursorIsInInventoryGrid( int x , int y )

/* ----------------------------------------------------------------------
 * This function gives the x coordinate of the inventory square that 
 * corresponds to the mouse cursor location given to the function.
 * ---------------------------------------------------------------------- */
int
GetInventorySquare_x( int x )
{
  return ( ( x - INVENTORY_RECT_X ) / INVENTORY_SUBSQUARE_WIDTH );
}; // int GetInventorySquare_x( x )

/* ----------------------------------------------------------------------
 * This function gives the y coordinate of the inventory square that 
 * corresponds to the mouse cursor location given to the function.
 * ---------------------------------------------------------------------- */
int
GetInventorySquare_y( int y )
{
  return ( ( y - (User_Rect.y + INVENTORY_RECT_Y ) ) / INVENTORY_SUBSQUARE_WIDTH );
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
  if ( ItemMap [ ItemType ] . inv_image . inv_size . x - 1 + InvPos_x >= 
       INVENTORY_GRID_WIDTH  ) return ( FALSE );
  if ( ItemMap [ ItemType ] . inv_image . inv_size . y - 1 + InvPos_y >= 
       INVENTORY_GRID_HEIGHT ) return ( FALSE );

  // --------------------
  // Now that we know, that the desired position is at least inside the inventory
  // grid, we can start to test for the details of the available inventory space
  //
  for ( item_height = 0 ; item_height < ItemMap[ ItemType ] . inv_image . inv_size . y ; item_height ++ )
    {
      for ( item_width = 0 ; item_width < ItemMap [ ItemType ] . inv_image . inv_size . x ; item_width ++ )
	{
	  if ( ! Inv_Pos_Is_Free( InvPos_x + item_width , InvPos_y + item_height ) ) return ( FALSE );
	}
    }
  return ( TRUE );

}; // int ItemCanBeDroppedInInv ( int ItemType , int InvPos_x , int InvPos_y )

/* ---------------------------------------------------------------------- 
 * This function should drop a given item to the floor. 
 *
 * Of couse this function should also initiate a signal to the server,
 * telling the server, that the item drop has occured and that the 
 * server should perform it too.
 *
 * ---------------------------------------------------------------------- */
void 
DropItemToTheFloor ( Item DropItemPointer , float x , float y , int levelnum )
{
  int i;
  Level DropLevel = curShip . AllLevels [ levelnum ] ;

  // --------------------
  // Now we want to drop the item to the floor.
  // We therefore find a free position in the item list of this level
  // where we can add the item later.
  //
  i = find_free_floor_items_index ( levelnum ) ;

  //--------------------
  // Now we enter the item into the item list of this level
  //
  CopyItem( DropItemPointer , & ( DropLevel -> ItemList [ i ] ) , TRUE ) ;
  // DropLevel->ItemList[ i ].pos.x = Me[0].pos.x;
  // DropLevel->ItemList[ i ].pos.y = Me[0].pos.y;
  DropLevel -> ItemList [ i ] . pos . x = x ; 
  DropLevel -> ItemList [ i ] . pos . y = y ; 
  DropLevel -> ItemList [ i ] . currently_held_in_hand = FALSE;
  DropLevel -> ItemList [ i ] . throw_time = 0.01; // something > 0 
  // DropLevel->ItemList[ i ].type = Me[0].Inventory[ InvPos ].type;
  
  // Me[0].Inventory[ InvPos ].type = ( -1 );
  DeleteItem ( DropItemPointer ) ;

  
  if ( ClientMode && ! ServerMode ) 
    {
      SendPlayerItemDropToServer ( GetPositionCode ( DropItemPointer )  , 
				   DropLevel -> ItemList [ i ] . pos . x ,
				   DropLevel -> ItemList [ i ] . pos . y ) ;
    }

}; // void DropItemToTheFloor ( void )

/* ---------------------------------------------------------------------- 
 * This function should drop a held item to the floor. 
 *
 * Of couse this function should also initiate a signal to the server,
 * telling the server, that the item drop has occured and that the 
 * server should perform it too.
 *
 * ---------------------------------------------------------------------- */
void 
DropHeldItemToTheFloor ( void )
{
  item* DropItemPointer;
  float x , y ;

  // --------------------
  // First we find out which item we want to drop onto the floor
  //
  DropItemPointer = GetHeldItemPointer(  );
  if ( DropItemPointer == NULL )
    {
      DebugPrintf( 0 , "\nvoid DropHeldItemToTheFloor ( void ) : No item in inventory seems to be currently held in hand...");
      return;
    } 

  x = translate_pixel_to_map_location ( 0 , 
					ServerThinksInputAxisX ( 0 ) , 
					ServerThinksInputAxisY ( 0 ) , TRUE ) ;
  y = translate_pixel_to_map_location ( 0 , 
					ServerThinksInputAxisX ( 0 ) , 
					ServerThinksInputAxisY ( 0 ) , FALSE ) ;

  DropItemToTheFloor ( DropItemPointer , x , y , Me [ 0 ] . pos . z ) ;

  timeout_from_item_drop = 0.4 ;

}; // void DropHeldItemToTheFloor ( void )

/* ----------------------------------------------------------------------
 * This function checks if the usage requirements for a given item are
 * met by the influencer or not.
 * ---------------------------------------------------------------------- */
int 
ItemUsageRequirementsMet( item* UseItem , int MakeSound )
{
  if ( Me[0].Strength < ItemMap[ UseItem->type ].item_require_strength )
    {
      if ( MakeSound ) Not_Enough_Power_Sound( );
      return ( FALSE );
    }
  if ( Me[0].Dexterity < ItemMap[ UseItem->type ].item_require_dexterity )
    {
      if ( MakeSound ) Not_Enough_Dist_Sound( );
      return ( FALSE );
    }
  if ( Me[0].Magic < ItemMap[ UseItem->type ].item_require_magic )
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
  int SourcePositionCode , DestPositionCode;

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
  //
  CopyItem( DropItemPointer , SlotItem , TRUE );
  SlotItem->currently_held_in_hand = FALSE;

  // Now the item is removed from the source location and no longer held in hand as well, 
  // but of course only if it is not the same as the original item
  if ( DropItemPointer != SlotItem )
    {

      DeleteItem( DropItemPointer );

      //--------------------
      // Now we inform the server of our performed move....
      //
      if ( ClientMode && ! ServerMode ) 
	{
	  SourcePositionCode = GetPositionCode ( DropItemPointer );
	  DestPositionCode = GetPositionCode ( SlotItem  ) ;
	  SendPlayerItemMoveToServer ( SourcePositionCode , DestPositionCode , -1 , -1  ) ;
	}

    }

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
  for ( InvPos = 0 ; InvPos < MAX_ITEMS_IN_INVENTORY -1 ; InvPos ++ )
    {
      if ( Me[0].Inventory[ InvPos ].type == (-1) ) 
	{
	  return ( InvPos );
	}
    }

  // --------------------
  // If this point is reached, the severe error mentioned above has
  // occured, an error message must be printed out and the program
  // must be terminated.
  //
  GiveStandardErrorMessage ( __FUNCTION__  , "\
A FREE INVENTORY INDEX POSITION COULD NOT BE FOUND.\n\
This is an internal error, that must never happen unless there are\n\
severe bugs in the inventory system.",
				 PLEASE_INFORM, IS_FATAL );
  return ( -1 ) ; // just to make compilers happy.
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
  int SourcePositionCode, DestPositionCode ;
  // item Temp;

  FreeInvIndex = GetFreeInventoryIndex( );

  // --------------------
  // First we find out which item we want to drop into the inventory
  //
  DropItemPointer = GetHeldItemPointer (  );
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
  CurPos.x = GetMousePos_x()  - 
    ( 16 * ( ItemMap [ DropItemPointer -> type ] . inv_image . inv_size . x - 1 ) ) ;
  CurPos.y = GetMousePos_y()  - 
    ( 16 * ( ItemMap [ DropItemPointer -> type ] . inv_image . inv_size . y - 1 ) ) ;

  if ( ItemCanBeDroppedInInv ( DropItemPointer->type , GetInventorySquare_x ( CurPos.x ) , 
			       GetInventorySquare_y ( CurPos.y ) ) )
    {
      CopyItem( DropItemPointer , & ( Me [ 0 ] . Inventory [ FreeInvIndex ] ) , TRUE );
      DeleteItem( DropItemPointer );
      Me [ 0 ] . Inventory [ FreeInvIndex ] . inventory_position.x = GetInventorySquare_x ( CurPos.x ) ;
      Me [ 0 ] . Inventory [ FreeInvIndex ] . inventory_position.y = GetInventorySquare_y ( CurPos.y ) ;
      Me [ 0 ] . Inventory [ FreeInvIndex ] . currently_held_in_hand = FALSE;

      // --------------------
      // Now that we know that the item could be dropped directly to inventory 
      // without swapping any paces, we can as well make the item
      // 'not held in hand' immediately and return
      //
      DropItemPointer->currently_held_in_hand = FALSE ;
      Item_Held_In_Hand = ( -1 );

      //--------------------
      // Now we inform the server of our performed move....
      //
      if ( ClientMode && ! ServerMode ) 
	{
	  SourcePositionCode = GetPositionCode ( DropItemPointer );
	  DestPositionCode = GetPositionCode ( & ( Me [ 0 ] . Inventory [ FreeInvIndex ] ) ) ;
	  SendPlayerItemMoveToServer ( SourcePositionCode , DestPositionCode , 
				       Me [ 0 ] . Inventory [ FreeInvIndex ] . inventory_position . x ,
				       Me [ 0 ] . Inventory [ FreeInvIndex ] . inventory_position . y ) ;
	}
      return;
    }
  else
    {
      //--------------------
      // So the item could not be placed into inventory directly, but maybe
      // it can be placed there if we swap our dropitem with some other item.
      // Let's test this opportunity here.
      //
      for ( i = 0 ; i < MAX_ITEMS_IN_INVENTORY -1 ; i ++ )
	{
	  //--------------------
	  // So we make a copy of each of the items we remove in order to 
	  // try to create new space for the drop item.  After that, we can
	  // remove it.
	  //
	  // CopyItem ( & ( Me[0].Inventory[ i ] ) , & ( Temp ) , FALSE );
	  //
	  CopyItem ( & ( Me[0].Inventory[ i ] ) , & ( Me [ 0 ] . Inventory [ MAX_ITEMS_IN_INVENTORY - 1 ]  ) , FALSE );
	  
	  //--------------------
	  // FIRST: Security check against segfaults:  It might happen that we 
	  // delete the Dropitem itself while trying several items as candidates
	  // for removal.  This would cause testing dropability with a -1 item
	  // type and a SEGFAULT would result...
	  //
	  if ( & ( Me[0].Inventory[ i ] ) == DropItemPointer ) continue;

	  Me [ 0 ] . Inventory [ i ] . type = ( -1 ) ;

	  if ( ItemCanBeDroppedInInv ( DropItemPointer->type , GetInventorySquare_x ( CurPos.x ) , 
				       GetInventorySquare_y ( CurPos.y ) ) )
	    {
	      //--------------------
	      // So if with the removed item Nr. i putting of the DropItem is 
	      // suddenly possible, then we make a held item on the floor out
	      // of it.  The other removed item can stay removed, since it will
	      // be overwritten anyway and a copy is now on the floor.
	      //
	      Item_Held_In_Hand = Me [ 0 ] . Inventory [ MAX_ITEMS_IN_INVENTORY -1 ] . type ;
	      
	      // THIS FAR EVERYTHING WORKS FINE!!!

	      //--------------------
	      // Now we inform the server of our performed move....
	      //
	      // WARNING!!  In this case here, the temporarily disabled item does not get
	      // restored!!  Therefore we have to inform the server, that now there is an 
	      // additional item move, namely the disabling of the one item.  This must
	      // be done FIRST!
	      //
	      if ( ClientMode && ! ServerMode ) 
		{

		  SourcePositionCode = GetPositionCode ( & ( Me [ 0 ] . Inventory [ i ] ) ) ;
		  DestPositionCode = GetPositionCode ( & ( Me [ 0 ] . Inventory [ MAX_ITEMS_IN_INVENTORY -1 ] ) ) ;
		  SendPlayerItemMoveToServer ( SourcePositionCode , DestPositionCode , 
					       Me [ 0 ] . Inventory [ FreeInvIndex ] . inventory_position . x ,
					       Me [ 0 ] . Inventory [ FreeInvIndex ] . inventory_position . y ) ;
		}

	      MakeHeldFloorItemOutOf ( & ( Me [ 0 ] . Inventory [ MAX_ITEMS_IN_INVENTORY - 1 ] ) ) ;
	      Me [ 0 ] . Inventory [ MAX_ITEMS_IN_INVENTORY - 1 ] . currently_held_in_hand = TRUE ;

	      //--------------------
	      // Otherwise we just need to add the new item for the inventory
	      // grid as usual
	      //
	      CopyItem ( DropItemPointer , & ( Me [ 0 ] . Inventory [ FreeInvIndex ] ) , TRUE );
	      DeleteItem ( DropItemPointer );
	      Me [ 0 ] . Inventory[ FreeInvIndex ].inventory_position.x = GetInventorySquare_x ( CurPos.x ) ;
	      Me [ 0 ] . Inventory[ FreeInvIndex ].inventory_position.y = GetInventorySquare_y ( CurPos.y ) ;
	      Me [ 0 ] . Inventory[ FreeInvIndex ].currently_held_in_hand = FALSE;

	      // And of course the item is no longer held in hand as well
	      DropItemPointer->currently_held_in_hand = FALSE ;

	      //--------------------
	      // Now we inform the server of the second part of our performed move....
	      //
	      if ( ClientMode && ! ServerMode ) 
		{

		  SourcePositionCode = GetPositionCode ( DropItemPointer );
		  DestPositionCode = GetPositionCode ( & ( Me [ 0 ] . Inventory [ FreeInvIndex ] ) ) ;
		  SendPlayerItemMoveToServer ( SourcePositionCode , DestPositionCode , 
					       Me [ 0 ] . Inventory [ FreeInvIndex ] . inventory_position . x ,
					       Me [ 0 ] . Inventory [ FreeInvIndex ] . inventory_position . y ) ;
		}

	      return;
	    }


	  //--------------------
	  // But if even the removal of one item was not enough, so that the new
	  // item would fit into the inventory, then of course we should re-add the
	  // removed item to the inventory, so that no other items get lost.
	  //
	  CopyItem ( & ( Me [ 0 ] . Inventory [ MAX_ITEMS_IN_INVENTORY -1 ] ) , & ( Me[0].Inventory[ i ] ) , FALSE );
	  // CopyItemWithoutHeldProperty ( & ( Me [ 0 ] . Inventory [ MAX_ITEMS_IN_INVENTORY -1 ] ) , & ( Me[0].Inventory[ i ] ) , FALSE );

	} // for: try all items if removal is the solution
    } // if not immediately place findable

  // --------------------
  // So at this point we know, that even the removal of other items was not the 
  // solution.  So the item cannot be put into inventory, even at best attampts
  // to do so.  What a pitty.
  //
  Item_Held_In_Hand = GetHeldItemCode ( ) ;

}; // void DropHeldItemToInventory( void )

/* ----------------------------------------------------------------------
 * This function shows the quick inventory items on the right side of
 * the screen.
 * ---------------------------------------------------------------------- */
void
ShowQuickInventory ( void )
{
  int i;
  SDL_Rect TargetRect;
  int Index;

  if ( ! GameConfig . show_quick_inventory ) return;

  //--------------------
  // We must not blit something right over the active character screen or the
  // active skill screen of course.  That would be irritating.
  //
  if ( ( GameConfig.SkillScreen_Visible ) || ( GameConfig.CharacterScreen_Visible ) ) return;

  //--------------------
  // Now we can blit all the objects in the quick inventory, but of course only
  // those small objects, that have a 1x1 inventory grid size, so that they really
  // can be drawn from the 'belt' that is actually the quick inventory.
  //
  for ( i = 0 ; i < 9 ; i ++ )
    {
      PutCharFont ( Screen , FPS_Display_BFont , SCREEN_WIDTH - INVENTORY_SUBSQUARE_WIDTH , 100 + i * 32 , '1' + i );
      if ( ( ( Index = GetInventoryItemAt ( i , INVENTORY_GRID_HEIGHT -1 ) ) != (-1) ) &&
	   ( Me[0].Inventory[ Index ].inventory_position.x == i ) &&
	   ( Me[0].Inventory[ Index ].inventory_position.y == INVENTORY_GRID_HEIGHT -1 ) )
	{
	  TargetRect.x = SCREEN_WIDTH - 32 ;
	  TargetRect.y = 100 + i * 32 ;
      
	  our_SDL_blit_surface_wrapper ( ItemMap [ Me [ 0 ] . Inventory [ Index ] . type ] . inv_image . Surface , 
					 NULL , Screen , &TargetRect );
	  
	}
    }
}; // void ShowQuickInventory ( void )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
int
get_floor_item_index_under_mouse_cursor ( int player_num )
{
    Level PlayerLevel = curShip . AllLevels [ Me [ player_num ] . pos . z ] ;
    finepoint MapPositionOfMouse;
    int i;
    
    //--------------------
    // In the case that shift was pressed, we don't use the item positions but rather
    // we use the item slot rectangles from the item texts.
    //
    if ( Shift_Is_Pressed() )
    {
	for ( i = 0 ; i < MAX_ITEMS_PER_LEVEL ; i++ )
	{
	    if ( PlayerLevel -> ItemList [ i ] . type == (-1) ) continue;

	    if ( MouseCursorIsInRect ( & ( PlayerLevel -> ItemList [ i ] . text_slot_rectangle ) , 
				       GetMousePos_x ( )  , 
				       GetMousePos_y ( )  ) )
	    {
		return ( i ) ;
	    }
	}
    }
    //--------------------
    // If no shift was pressed, we only use the floor position the mouse
    // has pointed to and see if we can find an item that has geographically
    // that very same (or a similar enough) position.
    //
    else
    {
	MapPositionOfMouse . x = 
	    translate_pixel_to_map_location ( 0 , 
					      ServerThinksInputAxisX ( 0 ) , 
					      ServerThinksInputAxisY ( 0 ) , TRUE ) ;
	MapPositionOfMouse . y = 
	    translate_pixel_to_map_location ( 0 , 
					      ServerThinksInputAxisX ( 0 ) , 
					      ServerThinksInputAxisY ( 0 ) , FALSE ) ;
	
	for ( i = 0 ; i < MAX_ITEMS_PER_LEVEL ; i++ )
	{
	    if ( PlayerLevel -> ItemList [ i ] . type == (-1) ) continue;
	    
	    if ( ( fabsf( MapPositionOfMouse.x - PlayerLevel -> ItemList [ i ] . pos . x ) < 0.5 ) &&
		 ( fabsf( MapPositionOfMouse.y - PlayerLevel -> ItemList [ i ] . pos . y ) < 0.5 ) )
	    {
		return ( i ) ;
	    }
	}
    }

    return ( -1 );
    
}; // int get_floor_item_index_under_mouse_cursor ( int player_num )

/* ----------------------------------------------------------------------
 * This function display the inventory screen and also checks for mouse
 * actions in the invenotry screen.
 * ---------------------------------------------------------------------- */
void 
ManageInventoryScreen ( void )
{
  static int MouseButtonPressedPreviousFrame = FALSE;
  static int RightPressedPreviousFrame = FALSE;
  point CurPos;
  point Inv_GrabLoc;
  int Grabbed_InvPos;
  finepoint MapPositionOfMouse;
  Level PlayerLevel = curShip . AllLevels [ Me [ 0 ] . pos . z ] ;
  int SpellCost = SpellSkillMap [ SPELL_IDENTIFY_SKILL ] . mana_cost_table [ Me[ 0 ] . spellcasting_skill ] ;
  int index_of_item_under_mouse_cursor = (-1) ;

  DebugPrintf (2, "\nvoid ShowInventoryMessages( ... ): Function call confirmed.");

  DebugPrintf ( INVENTORY_MOUSE_BUTTON_DEBUG , "\nRight: %d Left: %d Held: %d ", 
		RightPressedPreviousFrame , MouseButtonPressedPreviousFrame , Item_Held_In_Hand );

  //--------------------
  // In case the Tux is dead already, we do not need to display any inventory screen
  // or even to pick up any stuff for the Tux...
  //
  if ( Me [ 0 ] . energy <= 0 ) 
    {
      return;
    }

  // --------------------
  // We will need the current mouse position on several spots...
  //
  CurPos . x = GetMousePos_x() ;
  CurPos . y = GetMousePos_y()  ;

  //--------------------
  // If the log is not set to visible right now, we do not need to 
  // do anything more, but to see if there is an object directly under
  // the mouse cursor, and in case this is so, we just try to add it
  // to inventory instead of giving it into the players 'hand' for
  // drag and drop and individual placing.
  //
  if ( GameConfig.Inventory_Visible == FALSE ) 
    {
      
      silently_unhold_all_items ( );

      // DebugPrintf( 2 , "\nINVENTORY NOT VISIBLE!!" );
      if ( ( axis_is_active ) && ( !MouseButtonPressedPreviousFrame ) && ( Item_Held_In_Hand == (-1) ) )
	{
	  // DebugPrintf( 1 , "\nCollecting items for direct addition to the inventory without grabbing." );
	  MapPositionOfMouse . x = translate_pixel_to_map_location ( 0 , ServerThinksInputAxisX ( 0 ) , 
								     ServerThinksInputAxisY ( 0 ) , TRUE ) ;
	  MapPositionOfMouse . y = translate_pixel_to_map_location ( 0 , ServerThinksInputAxisX ( 0 ) , 
								     ServerThinksInputAxisY ( 0 ) , FALSE ) ;

	  //--------------------
	  // We only take items directly, when they are close enough ...
	  //
	  if ( ( fabsf( MapPositionOfMouse . x - Me [ 0 ] . pos . x ) < ITEM_TAKE_DIST ) &&
	       ( fabsf( MapPositionOfMouse . y - Me [ 0 ] . pos . y ) < ITEM_TAKE_DIST ) )
	    {

	      index_of_item_under_mouse_cursor = get_floor_item_index_under_mouse_cursor ( 0 );

	      if ( index_of_item_under_mouse_cursor != (-1) )
		{
		  Item_Held_In_Hand = ( -1 ); 
		  AddFloorItemDirectlyToInventory( & ( PlayerLevel -> ItemList [ index_of_item_under_mouse_cursor ] ) );
		  MouseButtonPressedPreviousFrame = axis_is_active;
		  RightPressedPreviousFrame = MouseRightPressed ( ) ;
		  return;
		}

	    }
	  //--------------------
	  // If the item isn't close enough, we start a combined move_and_do
	  // procedure to go to the item and then pick it up once the Tux is
	  // there...
	  //
	  else
	    {
	      index_of_item_under_mouse_cursor = get_floor_item_index_under_mouse_cursor ( 0 );

	      if ( index_of_item_under_mouse_cursor != (-1) )
		{
		  //--------------------
		  // We set course to the item in question, directly to it's location,
		  // not somewhere remote, just for simplicity (for now)...
		  //
		  Me [ 0 ] . mouse_move_target . x = 
		    PlayerLevel -> ItemList [ index_of_item_under_mouse_cursor ] . pos . x ;
		  Me [ 0 ] . mouse_move_target . y = 
		    PlayerLevel -> ItemList [ index_of_item_under_mouse_cursor ] . pos . y ;
		  set_up_intermediate_course_for_tux ( 0 ) ;
		  
		  //--------------------
		  // We set up the combo_action, so that the barrel can be smashed later...
		  //
		  Me [ 0 ] . mouse_move_target_is_enemy = ( -1 ) ;
		  Me [ 0 ] . mouse_move_target_combo_action_type = COMBO_ACTION_PICK_UP_ITEM ;
		  Me [ 0 ] . mouse_move_target_combo_action_parameter = index_of_item_under_mouse_cursor ;
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
      DebugPrintf( 1 , "\nTrying to 'grab' the item below the mouse cursor.");
      
      if ( MouseCursorIsInInventoryGrid( CurPos.x , CurPos.y ) )
	{
	  Inv_GrabLoc.x = GetInventorySquare_x ( CurPos.x );
	  Inv_GrabLoc.y = GetInventorySquare_y ( CurPos.y );

	  DebugPrintf( 1 , "\nGrabbing at inv-pos: %d %d." , Inv_GrabLoc.x , Inv_GrabLoc.y );
	      
	  Grabbed_InvPos = GetInventoryItemAt ( Inv_GrabLoc.x , Inv_GrabLoc.y );
	  DebugPrintf( 1 , "\nGrabbing inventory entry no.: %d." , Grabbed_InvPos );

	  if ( Grabbed_InvPos == (-1) )
	    {
	      // Nothing grabbed, so we need not do anything more here..
	      Item_Held_In_Hand = ( -1 );
	      DebugPrintf( 1 , "\nGrabbing in INVENTORY grid FAILED!" );
	    }
	  else
	    {
	      //--------------------
	      // At this point we know, that we have just grabbed something from the inventory
	      // So we set, that something should be displayed in the 'hand', and it should of
	      // course be the image of the item grabbed from inventory.
	      //
	      Item_Held_In_Hand = Me [ 0 ] . Inventory [ Grabbed_InvPos ] . type ;
	      // PlayItemSound( ItemMap[ Me[0].Inventory[ Grabbed_InvPos ].type ].sound_number );
	      play_item_sound( Me[0].Inventory[ Grabbed_InvPos ].type );
	      Me[0].Inventory[ Grabbed_InvPos ].currently_held_in_hand = TRUE;
	    }
	}
      else if ( MouseCursorIsOnButton ( WEAPON_RECT_BUTTON , CurPos.x , CurPos.y ) )
	{
	  DebugPrintf( 1 , "\nGrabbing in weapons rect!" );
	  if ( Me[0].weapon_item.type > 0 )
	    {
	      //--------------------
	      // At this point we know, that we have just grabbed something from the weapon rect
	      // So we set, that something should be displayed in the 'hand', and it should of
	      // course be the image of the item grabbed from inventory.
	      //
	      Item_Held_In_Hand = Me [ 0 ] . weapon_item . type ;
	      Me[0].weapon_item.currently_held_in_hand = TRUE;
	    }
	}
      else if ( MouseCursorIsOnButton ( DRIVE_RECT_BUTTON , CurPos.x , CurPos.y ) )
	{
	  DebugPrintf( 1 , "\nGrabbing in drive rect!" );
	  if ( Me[0].drive_item.type > 0 )
	    {
	      //--------------------
	      // At this point we know, that we have just grabbed something from the weapon rect
	      // So we set, that something should be displayed in the 'hand', and it should of
	      // course be the image of the item grabbed from inventory.
	      //
	      Item_Held_In_Hand = Me [ 0 ] . drive_item . type ;
	      Me[0].drive_item.currently_held_in_hand = TRUE;
	    }
	}
      else if ( MouseCursorIsOnButton( SHIELD_RECT_BUTTON , CurPos.x , CurPos.y ) )
	{
	  DebugPrintf( 1 , "\nGrabbing in shield rect!" );
	  if ( Me[0].shield_item.type > 0 )
	    {
	      //--------------------
	      // At this point we know, that we have just grabbed something from the weapon rect
	      // So we set, that something should be displayed in the 'hand', and it should of
	      // course be the image of the item grabbed from inventory.
	      //
	      Item_Held_In_Hand = Me [ 0 ] . shield_item . type ;
	      Me[0].shield_item.currently_held_in_hand = TRUE;
	    }
	  else if ( Me [ 0 ] . weapon_item . type > 0 )
	    {
		if ( ItemMap [ Me [ 0 ] . weapon_item . type ] . item_gun_requires_both_hands )
		{
		    //--------------------
		    // At this point we know, that we have just grabbed something from the shield rect
		    // but it's not the shield but rather the 2-handed weapon, that is in the weapon
		    // slot.  
		    // So we set, that something should be displayed in the 'hand', and it should of
		    // course be the image of the item grabbed from inventory.
		    //
		    Item_Held_In_Hand = Me [ 0 ] . weapon_item . type ;
		    Me [ 0 ] . weapon_item . currently_held_in_hand = TRUE;
		}
	    }


	}
      else if ( MouseCursorIsOnButton( ARMOUR_RECT_BUTTON , CurPos.x , CurPos.y ) )
	{
	  DebugPrintf( 1 , "\nGrabbing in armour rect!" );
	  if ( Me[0].armour_item.type > 0 )
	    {
	      //--------------------
	      // At this point we know, that we have just grabbed something from the weapon rect
	      // So we set, that something should be displayed in the 'hand', and it should of
	      // course be the image of the item grabbed from inventory.
	      //
	      Item_Held_In_Hand = Me [ 0 ] . armour_item . type ;
	      Me[0].armour_item.currently_held_in_hand = TRUE;
	    }
	}
      else if ( MouseCursorIsOnButton( HELMET_RECT_BUTTON , CurPos.x , CurPos.y ) )
	{
	  DebugPrintf( 1 , "\nGrabbing in special rect!" );
	  if ( Me[0].special_item.type > 0 )
	    {
	      //--------------------
	      // At this point we know, that we have just grabbed something from the weapon rect
	      // So we set, that something should be displayed in the 'hand', and it should of
	      // course be the image of the item grabbed from inventory.
	      //
	      Item_Held_In_Hand = Me [ 0 ] . special_item . type ;
	      Me[0].special_item.currently_held_in_hand = TRUE;
	    }
	}
      else if ( MouseCursorIsOnButton( AUX1_RECT_BUTTON , CurPos.x , CurPos.y ) )
	{
	  DebugPrintf( 1 , "\nGrabbing in aux1 rect!" );
	  if ( Me[0].aux1_item.type > 0 )
	    {
	      //--------------------
	      // At this point we know, that we have just grabbed something from the weapon rect
	      // So we set, that something should be displayed in the 'hand', and it should of
	      // course be the image of the item grabbed from inventory.
	      //
	      Item_Held_In_Hand = Me [ 0 ] . aux1_item . type ;
	      Me[0].aux1_item.currently_held_in_hand = TRUE;
	    }
	}
      else if ( MouseCursorIsOnButton( AUX2_RECT_BUTTON , CurPos.x , CurPos.y ) )
	{
	  DebugPrintf( 1 , "\nGrabbing in aux2 rect!" );
	  if ( Me[0].aux2_item.type > 0 )
	    {
	      //--------------------
	      // At this point we know, that we have just grabbed something from the weapon rect
	      // So we set, that something should be displayed in the 'hand', and it should of
	      // course be the image of the item grabbed from inventory.
	      //
	      Item_Held_In_Hand = Me [ 0 ] . aux2_item . type ;
	      Me [ 0 ] . aux2_item . currently_held_in_hand = TRUE;
	    }
	}
      else if ( MouseCursorIsInUserRect( CurPos.x , CurPos.y ) )
	{
	  DebugPrintf( 1 , "\nGrabbing in user rect!" );
	  DebugPrintf( 0  , "\nMouse in map at: %f %f." , MapPositionOfMouse.x , MapPositionOfMouse.y );

	  index_of_item_under_mouse_cursor = get_floor_item_index_under_mouse_cursor ( 0 );

	  if ( index_of_item_under_mouse_cursor != (-1) )
	    {
	      if ( PlayerLevel -> ItemList [ index_of_item_under_mouse_cursor ] . type == ITEM_MONEY ) 		      
		{
		  AddFloorItemDirectlyToInventory( & ( PlayerLevel -> ItemList [ index_of_item_under_mouse_cursor ] ) );
		  return;
		}
	      else
		{
		  Item_Held_In_Hand = PlayerLevel -> ItemList [ index_of_item_under_mouse_cursor ] . type ;
		  PlayerLevel -> ItemList [ index_of_item_under_mouse_cursor ] . currently_held_in_hand = TRUE;
		  // break;
		}
	    }
	}
      else
	{
	  // Nothing grabbed, so we need not do anything more here..
	  Item_Held_In_Hand = ( -1 );
	  DebugPrintf( 1 , "\nGrabbing UTTERLY FAILED!" );
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
      if ( MouseCursorIsInInventoryGrid( CurPos.x , CurPos.y ) )
	{
	  DebugPrintf( 1 , "\nItem dropped in inventory window!" );
	  Item_Held_In_Hand = ( -1 );
	  DropHeldItemToInventory( );
	}

      //--------------------
      // If the cursor is in the user_rect, i.e. the combat window, then
      // the item should be dropped onto the players current location
      //
      if ( MouseCursorIsInUserRect ( CurPos.x , CurPos.y ) )
	{
	  DebugPrintf( 1 , "\nItem dropped onto the floor of the combat window!" );
	  Item_Held_In_Hand = ( -1 );
	  DropHeldItemToTheFloor( );
	}

      //--------------------
      // If the cursor is in the weapons rect, i.e. the small box top left, then
      // the item should be dropped onto the players current weapon slot
      //
      if ( MouseCursorIsOnButton ( WEAPON_RECT_BUTTON , CurPos.x , CurPos.y ) )
	{
	  DebugPrintf( 1 , "\nItem dropped onto the weapons rectangle!" );
	  DebugPrintf( 1 , "\nGetHeldItemCode: %d." , GetHeldItemCode() );
	  if ( ( GetHeldItemCode() != (-1) ) &&
	       ( ItemMap[ GetHeldItemCode() ].item_can_be_installed_in_weapon_slot ) )
	    {
	      if ( HeldItemUsageRequirementsMet(  ) )
		{
		  //--------------------
		  // Now a weapon is about to be dropped to the weapons rectangle and obviously
		  // the stat requirements for usage are met.  But maybe this is a 2-handed weapon.
		  // In this case we need to do some extra check.  If it isn't a 2-handed weapon,
		  // then we can just go ahead and equip the item
		  if ( ItemMap [ GetHeldItemCode ( ) ] . item_gun_requires_both_hands )
		    {
		      //--------------------
		      // Now if the shield slot is just empty, that makes matters a lot simpler,
		      // cause then we can just drop this 2-handed weapon to the weapon slot and
		      // all is fine, cause no conflicts will result...
		      //
		      if ( Me [ 0 ] . shield_item . type == (-1) )
			{
			  Item_Held_In_Hand = ( -1 );
			  DropHeldItemToSlot ( & ( Me [ 0 ] . weapon_item ) );
			}
		      else
			{
			  //--------------------
			  // But if there is something in the shield slot too, then we need to be
			  // a bit more sophisticated and either swap the 2-handed item in for just
			  // the shield alone, which then will be held OR we need to refuse completely
			  // because there might be a weapon AND a shield equipped already.
			  //
			  if ( Me [ 0 ] . weapon_item . type == (-1) )
			    {
			      Item_Held_In_Hand = ( -1 );
			      DropHeldItemToSlot ( & ( Me [ 0 ] . weapon_item ) );
			      MakeHeldFloorItemOutOf( & ( Me [ 0 ] . shield_item ) ) ;
			    }
			  else
			    {
			      //--------------------
			      // Since a sword and a shield are both equipped, we must refuse to
			      // equip this 2-handed weapon here and now...
			      //
			      PlayOnceNeededSoundSample ( "../effects/tux_ingame_comments/ThisItemRequiresBothHands.wav" , FALSE , FALSE );
			    }
			}
		    }
		  else
		    {
		      Item_Held_In_Hand = ( -1 );
		      DropHeldItemToSlot ( & ( Me [ 0 ] . weapon_item ) );
		    }
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
      if ( MouseCursorIsOnButton ( DRIVE_RECT_BUTTON, CurPos.x , CurPos.y ) )
	{
	  DebugPrintf( 1 , "\nItem dropped onto the drive rectangle!" );
	  DebugPrintf( 1 , "\nGetHeldItemCode: %d." , GetHeldItemCode() );
	  if ( ( GetHeldItemCode() != (-1) ) &&
	       ( ItemMap[ GetHeldItemCode() ].item_can_be_installed_in_drive_slot ) )
	    {
	      if ( HeldItemUsageRequirementsMet(  ) )
		{
		  Item_Held_In_Hand = ( -1 );
		  // DropHeldItemToDriveSlot ( );
		  DropHeldItemToSlot ( & ( Me[0].drive_item ) );
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
      if ( MouseCursorIsOnButton ( ARMOUR_RECT_BUTTON , CurPos.x , CurPos.y ) )
	{
	  DebugPrintf( 1 , "\nItem dropped onto the armour rectangle!" );
	  DebugPrintf( 1 , "\nGetHeldItemCode: %d." , GetHeldItemCode() );
	  if ( ( GetHeldItemCode() != (-1) ) &&
	       ( ItemMap[ GetHeldItemCode() ].item_can_be_installed_in_armour_slot ) )
	    {
	      if ( HeldItemUsageRequirementsMet(  ) )
		{
		  Item_Held_In_Hand = ( -1 );
		  // DropHeldItemToArmourSlot ( );
		  DropHeldItemToSlot ( & ( Me[0].armour_item ) );
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
      if ( MouseCursorIsOnButton ( SHIELD_RECT_BUTTON , CurPos.x , CurPos.y ) )
	{
	  DebugPrintf( 1 , "\nItem dropped onto the shield rectangle!" );
	  DebugPrintf( 1 , "\nGetHeldItemCode: %d." , GetHeldItemCode() );
	  if ( ( GetHeldItemCode() != (-1) ) &&
	       ( ItemMap [ GetHeldItemCode() ] . item_can_be_installed_in_shield_slot ) )
	    {
	      if ( HeldItemUsageRequirementsMet(  ) )
		{
		  //--------------------
		  // Now if there isn't any weapon equipped right now, the matter
		  // is rather simple and we just need to do the normal drop-to-slot-thing.
		  //
		  if ( Me [ 0 ] . weapon_item . type == (-1) )
		    {
		      Item_Held_In_Hand = ( -1 );
		      DropHeldItemToSlot ( & ( Me[0].shield_item ) );
		    }
		  else
		    {
		      Item_Held_In_Hand = ( -1 );
		      DropHeldItemToSlot ( & ( Me [ 0 ] . shield_item ) );

		      //--------------------
		      // A shield, when equipped, will push out any 2-handed weapon currently
		      // equipped from it's weapon slot...
		      //
		      if ( ItemMap [ Me [ 0 ] . weapon_item . type ] . item_gun_requires_both_hands )
			{
			  MakeHeldFloorItemOutOf ( & ( Me [ 0 ] . weapon_item ) ) ;
			}
		    }
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
      if ( MouseCursorIsOnButton ( HELMET_RECT_BUTTON , CurPos.x , CurPos.y ) )
	{
	  DebugPrintf( 1 , "\nItem dropped onto the special rectangle!" );
	  DebugPrintf( 1 , "\nGetHeldItemCode: %d." , GetHeldItemCode() );
	  if ( ( GetHeldItemCode() != (-1) ) &&
	       ( ItemMap[ GetHeldItemCode() ].item_can_be_installed_in_special_slot ) )
	    {
	      if ( HeldItemUsageRequirementsMet(  ) )
		{
		  Item_Held_In_Hand = ( -1 );
		  // DropHeldItemToSpecialSlot ( );
		  DropHeldItemToSlot ( & ( Me[0].special_item ) );
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
      if ( MouseCursorIsOnButton ( AUX1_RECT_BUTTON , CurPos.x , CurPos.y ) )
	{
	  DebugPrintf( 1 , "\nItem dropped onto the aux1 rectangle!" );
	  DebugPrintf( 1 , "\nGetHeldItemCode: %d." , GetHeldItemCode() );
	  if ( ( GetHeldItemCode() != (-1) ) &&
	       ( ItemMap[ GetHeldItemCode() ].item_can_be_installed_in_aux_slot ) )
	    {
	      if ( HeldItemUsageRequirementsMet(  ) )
		{
		  Item_Held_In_Hand = ( -1 );
		  // DropHeldItemToAux1Slot ( );
		  DropHeldItemToSlot ( & ( Me[0].aux1_item ) );
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
      if ( MouseCursorIsOnButton ( AUX2_RECT_BUTTON , CurPos.x , CurPos.y ) )
	{
	  DebugPrintf( 1 , "\nItem dropped onto the aux2 rectangle!" );
	  DebugPrintf( 1 , "\nGetHeldItemCode: %d." , GetHeldItemCode() );
	  if ( ( GetHeldItemCode() != (-1) ) &&
	       ( ItemMap[ GetHeldItemCode() ].item_can_be_installed_in_aux_slot ) )
	    {
	      if ( HeldItemUsageRequirementsMet(  ) )
		{
		  Item_Held_In_Hand = ( -1 );
		  // DropHeldItemToAux2Slot ( );
		  DropHeldItemToSlot ( & ( Me[0].aux2_item ) );
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

      switch ( Me [ 0 ] . readied_skill )
	{
	case SPELL_REPAIR_SKILL:
	  //--------------------
	  // Here we know, that the repair skill is selected, therefore we try to 
	  // repair the item currently under the mouse cursor.
	  //
	  if ( MouseCursorIsInInventoryGrid( CurPos.x , CurPos.y ) )
	    {
	      Inv_GrabLoc.x = GetInventorySquare_x ( CurPos.x );
	      Inv_GrabLoc.y = GetInventorySquare_y ( CurPos.y );
	      
	      DebugPrintf( 0 , "\nTrying to repair item at inv-pos: %d %d." , Inv_GrabLoc.x , Inv_GrabLoc.y );
	      
	      Grabbed_InvPos = GetInventoryItemAt ( Inv_GrabLoc.x , Inv_GrabLoc.y );
	      DebugPrintf( 0 , "\nTrying to repair inventory entry no.: %d." , Grabbed_InvPos );
	      
	      if ( Grabbed_InvPos == (-1) )
		{
		  // Nothing grabbed, so we need not do anything more here..
		  DebugPrintf( 0 , "\nRepairing in INVENTORY grid FAILED:  NO ITEM AT THIS POSITION FOUND!" );
		}
	      else
		{
		  HomeMadeItemRepair ( & ( Me[0].Inventory[ Grabbed_InvPos ] ) ) ;
		}
	    }
	  else if ( MouseCursorIsOnButton ( WEAPON_RECT_BUTTON , CurPos.x , CurPos.y ) )
	    {
	      DebugPrintf( 0 , "\nItem repair requested for the weapons rectangle!" );
	      if ( Me [ 0 ] . weapon_item . type != (-1) )
		HomeMadeItemRepair ( & ( Me [ 0 ] . weapon_item ) );
	    }
	  else if ( MouseCursorIsOnButton ( DRIVE_RECT_BUTTON , CurPos.x , CurPos.y ) )
	    {
	      DebugPrintf( 0 , "\nItem repair requested for the drive rectangle!" );
	      if ( Me [ 0 ] . drive_item . type != (-1) )
		HomeMadeItemRepair ( & ( Me [ 0 ] . drive_item ) );
	    }
	  else if ( MouseCursorIsOnButton ( SHIELD_RECT_BUTTON , CurPos.x , CurPos.y ) )
	    {
	      DebugPrintf( 0 , "\nItem repair requested for the shield rectangle!" );
	      if ( Me [ 0 ] . shield_item . type != (-1) )
		HomeMadeItemRepair ( & ( Me [ 0 ] . shield_item ) );
	    }
	  else if ( MouseCursorIsOnButton ( ARMOUR_RECT_BUTTON , CurPos.x , CurPos.y ) )
	    {
	      DebugPrintf( 0 , "\nItem repair requested for the armour rectangle!" );
	      if ( Me [ 0 ] . armour_item . type != (-1) )
		HomeMadeItemRepair ( & ( Me [ 0 ] . armour_item ) );
	    }
	  else if ( MouseCursorIsOnButton ( HELMET_RECT_BUTTON , CurPos.x , CurPos.y ) )
	    {
	      DebugPrintf( 0 , "\nItem repair requested for the special rectangle!" );
	      if ( Me [ 0 ] . special_item . type != (-1) )
		HomeMadeItemRepair ( & ( Me [ 0 ] . special_item ) );
	    }
	  else if ( MouseCursorIsOnButton ( AUX1_RECT_BUTTON , CurPos.x , CurPos.y ) )
	    {
	      DebugPrintf( 0 , "\nItem repair requested for the Aux1 rectangle!" );
	      if ( Me [ 0 ] . aux1_item . type != (-1) )
		HomeMadeItemRepair ( & ( Me [ 0 ] . aux1_item ) );
	    }
	  else if ( MouseCursorIsOnButton ( AUX2_RECT_BUTTON , CurPos.x , CurPos.y ) )
	    {
	      DebugPrintf( 0 , "\nItem repair requested for the Aux2 rectangle!" );
	      if ( Me [ 0 ] . aux2_item . type != (-1) )
		HomeMadeItemRepair ( & ( Me [ 0 ] . aux2_item ) );
	    }
	  break;

	case SPELL_IDENTIFY_SKILL:
	  //--------------------
	  // Here we know, that the identify skill is selected, therefore we try to 
	  // repair the item currently under the mouse cursor.
	  //
	  if ( MouseCursorIsInInventoryGrid( CurPos.x , CurPos.y ) )
	    {
	      Inv_GrabLoc.x = GetInventorySquare_x ( CurPos.x );
	      Inv_GrabLoc.y = GetInventorySquare_y ( CurPos.y );
	      
	      DebugPrintf( 0 , "\nTrying to repair item at inv-pos: %d %d." , Inv_GrabLoc.x , Inv_GrabLoc.y );
	      
	      Grabbed_InvPos = GetInventoryItemAt ( Inv_GrabLoc.x , Inv_GrabLoc.y );
	      DebugPrintf( 0 , "\nTrying to repair inventory entry no.: %d." , Grabbed_InvPos );
	      
	      if ( Grabbed_InvPos == (-1) )
		{
		  // Nothing grabbed, so we need not do anything more here..
		  DebugPrintf( 0 , "\nIdentifying in INVENTORY grid FAILED:  NO ITEM AT THIS POSITION FOUND!" );
		}
	      else
		{
		  if ( Me [ 0 ] . Inventory [ Grabbed_InvPos ] . is_identified == TRUE )
		    {
		      PlayOnceNeededSoundSample ( "../effects/is_already_indentif.wav" , FALSE , FALSE );
		    }
		  else
		    {
		      if ( Me [ 0 ] . mana >= SpellCost )
			{
			  Me [ 0 ] . mana -= SpellCost;
			  Me [ 0 ] . Inventory [ Grabbed_InvPos ] . is_identified = TRUE ;
			  Play_Spell_ForceToEnergy_Sound( );
			  Me [ 0 ] . readied_skill = SPELL_TRANSFERMODE ;
			}
		      else
			{
			  Me [ 0 ] . TextVisibleTime = 0;
			  Me [ 0 ] . TextToBeDisplayed = "Not enough force left within me.";
			  Not_Enough_Mana_Sound(  );
			}
		    }
		}
	    }
	  break;

	default:
	  //--------------------
	  // The default behaviour for right mouse clicks (i.e. when no repair or
	  // identify skills are selected) is to try to 'apply' the item in combat, 
	  // i.e. expend the item, if it can be used up just so like a potion.  So
	  // we do this here.
	  //
	  if ( MouseCursorIsInInventoryGrid( CurPos.x , CurPos.y ) )
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
		  ApplyItem( & ( Me[0].Inventory[ Grabbed_InvPos ] ) );
		}
	    }
	  break;

	}

    }

  //--------------------
  // Finally, we want the part of the screen we have been editing to become
  // visible and therefore we must updated it here, since it is currently not
  // contained within the user rectangle that also gets updated every frame.
  //
  // our_SDL_update_rect_wrapper( Screen , InventoryRect.x , InventoryRect.y , InventoryRect.w , InventoryRect.h );
  //
  MouseButtonPressedPreviousFrame = axis_is_active;
  RightPressedPreviousFrame = MouseRightPressed ( ) ;
}; // void ManageInventoryScreen ( void );

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
raw_move_picked_up_item_to_entry ( item* ItemPointer , item* TargetPointer , point Inv_Loc )
{
  char TempText[1000];
  int SourceCode , DestCode ;

  // We announce that we have taken the item
  Me[0].TextVisibleTime = 0;
  sprintf( TempText , "Item taken: %s." , ItemMap[ ItemPointer->type ].item_name );
  Me[0].TextToBeDisplayed=MyMalloc( strlen( TempText ) + 1 );
  strcpy ( Me[0].TextToBeDisplayed , TempText );
  
  // We add the new item to the inventory
  CopyItem( ItemPointer , TargetPointer , FALSE );
  TargetPointer -> inventory_position . x = Inv_Loc . x ;
  TargetPointer -> inventory_position.y = Inv_Loc . y ;
  
  // We make the sound of an item being taken
  // PlayItemSound( ItemMap[ ItemPointer->type ].sound_number );
  play_item_sound( ItemPointer -> type );
  
  DeleteItem( ItemPointer );
  
  //--------------------
  // And of course we shouldn't forget to tell the server about this 
  // movement as well....
  //
  if ( ClientMode && ! ServerMode ) 
    {
      SourceCode = GetPositionCode ( ItemPointer ) ;
      DestCode = GetPositionCode ( TargetPointer ) ;
      SendPlayerItemMoveToServer ( SourceCode , DestCode , Inv_Loc.x , Inv_Loc.y ) ;
    }
}; // void move_picked_up_item_to_entry ( ItemPointer , TargetPointer )

/* ----------------------------------------------------------------------
 *
 * ---------------------------------------------------------------------- */
int
place_item_on_this_position_if_you_can ( item* ItemPointer , point Inv_Loc , int InvPos )
{
  int item_height;
  int item_width;

  for ( item_height = 0 ; item_height < ItemMap [ ItemPointer -> type ] . inv_image . inv_size . y ; item_height ++ )
    {
      for ( item_width = 0 ; item_width < ItemMap [ ItemPointer -> type ] . inv_image . inv_size . x ; item_width ++ )
	{
	  DebugPrintf( 1 , "\nAddFloorItemDirectlyToInventory:  Checking pos: %d %d " , Inv_Loc.x + item_width , Inv_Loc.y + item_height );
	  if ( !Inv_Pos_Is_Free( Inv_Loc . x + item_width , 
				 Inv_Loc . y + item_height ) )
	    {
	      Me [ 0 ] . Inventory [ InvPos ] . inventory_position . x = -1;
	      Me [ 0 ] . Inventory [ InvPos ] . inventory_position . y = -1;
	      // goto This_Is_No_Possible_Location;
	      return ( FALSE ) ;
	    }
	}
    }
  // if ( !Inv_Pos_Is_Free( Inv_Loc.x , Inv_Loc.y ) ) continue;
  
  // At this point we know we have reached a position where we can plant this item.
  Me [ 0 ] . Inventory [ InvPos ] . inventory_position . x = Inv_Loc . x ;
  Me [ 0 ] . Inventory [ InvPos ] . inventory_position . y = Inv_Loc . y ;
  DebugPrintf( 1 , "\nAddFloorItemDirectlyToInventory:  FINE INVENTORY POSITION FOUND!!");
  
  //--------------------
  if ( ( InvPos >= MAX_ITEMS_IN_INVENTORY -1 ) || ( Me[0].Inventory[ InvPos ].inventory_position.x == (-1) ) )
    {
      Me [ 0 ] . TextVisibleTime = 0;
      Me [ 0 ] . TextToBeDisplayed = "I can't carry any more.";
      CantCarrySound();
      // can't take any more items,
    }
  else
    {
      raw_move_picked_up_item_to_entry ( ItemPointer , & ( Me [ 0 ] . Inventory [ InvPos ] ) , Inv_Loc );
    }
  return ( TRUE );
}; // int place_item_on_this_position_if_you_can ( ... )

/* ----------------------------------------------------------------------
 * This function deals with the case, that WHILE THERE IS NO INVENTORY
 * SCREEN OPEN, the Tux still clicks some items on the floor to pick them
 * up.  So no big visible operation is required, but instead the items
 * picked up should be either auto-equipped, if possible, or they should
 * be put into the inventory items pool OR IN CASE THERE IS NO ROOM ANY
 * MORE the function should also say that and not do much else...
 * ---------------------------------------------------------------------- */
void 
AddFloorItemDirectlyToInventory( item* ItemPointer )
{
  int InvPos;
  point Inv_Loc = { -1 , -1 } ;
  int TargetItemIndex;

  //--------------------
  // In case we found an item on the floor, we remove it from the floor
  // and add it to influs inventory
  //
  if ( ItemPointer == NULL ) return;

  //--------------------
  // In the special case of money, we add the amount of money to our
  // money counter and eliminate the item on the floor.
  //
  if ( ItemPointer->type == ITEM_MONEY )
    {
      // PlayItemSound( ItemMap[ ItemPointer->type ].sound_number );
      play_item_sound( ItemPointer -> type );
      Me [ 0 ] . Gold += ItemPointer->gold_amount;
      DeleteItem( ItemPointer );
      return;
    }
  
  //--------------------
  // In the special case, that this is an item, that groups together with others
  // of the same type AND we also have as item of this type already in inventory,
  // then we just need to manipulate multiplicity a bit and we're done.  Very easy.
  //
  if ( ItemMap [ ItemPointer->type ] . item_group_together_in_inventory )
    {
      if ( CountItemtypeInInventory ( ItemPointer->type , PLAYER_NR_0 ) )
	{
	  TargetItemIndex = FindFirstInventoryIndexWithItemType ( ItemPointer->type , PLAYER_NR_0 );
	  Me [ PLAYER_NR_0 ] . Inventory [ TargetItemIndex ] . multiplicity += ItemPointer->multiplicity;
	  // PlayItemSound( ItemMap[ ItemPointer->type ].sound_number );
	  play_item_sound ( ItemPointer -> type );
	  DeleteItem( ItemPointer );
	  return;
	}
    }
  
  //--------------------
  // Maybe the item is of a kind that can be equipped right now.  Then
  // we decide to directly drop it to the corresponding slot.
  //
  if ( ( Me [ 0 ] . weapon_item . type == (-1) ) && ( ItemMap [ ItemPointer -> type ] . item_can_be_installed_in_weapon_slot ) )
    {
      if ( ItemUsageRequirementsMet( ItemPointer , TRUE ) )
	{
	  //--------------------
	  // Now we're picking up a weapon while no weapon is equipped.  But still
	  // it might be a 2-handed weapon while there is some shield equipped.  Well,
	  // when that is the case, we refuse to put it directly to the proper slot, 
	  // otherwise we do it.
	  //
	  if ( Me [ 0 ] . shield_item . type == (-1) )
	    {
	      raw_move_picked_up_item_to_entry ( ItemPointer , & ( Me [ 0 ] . weapon_item ) , Inv_Loc );
	      return;
	    }
	  //--------------------
	  // So now we know that some shield item is equipped.  Let's be careful:  2-handed
	  // weapons will be rejected from direct addition to the slot.
	  //
	  if ( ! ItemMap [ ItemPointer -> type ] . item_gun_requires_both_hands )
	    {
	      raw_move_picked_up_item_to_entry ( ItemPointer , & ( Me [ 0 ] . weapon_item ) , Inv_Loc );
	      return;
	    }
	}
    }
  if ( ( Me [ 0 ] . shield_item . type == (-1) ) && ( ItemMap [ ItemPointer -> type ] . item_can_be_installed_in_shield_slot ) )
    {
      if ( ItemUsageRequirementsMet( ItemPointer , TRUE ) )
	{
	  //--------------------
	  // Auto-equipping shields can be done.  But only if there isn't a 2-handed
	  // weapon equipped already.  Well, in case of no weapon present it's easy:
	  //
	  if ( Me [ 0 ] . weapon_item . type == (-1) )
	    {
	      raw_move_picked_up_item_to_entry ( ItemPointer , & ( Me [ 0 ] . shield_item ) , Inv_Loc );
	      return;
	    }
	  //--------------------
	  // But now we know, that there is some weapon present.  We need to be careful:
	  // it might be a 2-handed weapon.
	  // 
	  if ( ! ItemMap [ Me [ 0 ] . weapon_item . type ] . item_gun_requires_both_hands )
	    {
	      raw_move_picked_up_item_to_entry ( ItemPointer , & ( Me [ 0 ] . shield_item ) , Inv_Loc );
	      return;
	    }
	}
    }
  if ( ( Me [ 0 ] . armour_item . type == (-1) ) && ( ItemMap [ ItemPointer -> type ] . item_can_be_installed_in_armour_slot ) )
    {
      if ( ItemUsageRequirementsMet( ItemPointer , TRUE ) )
	{
	  raw_move_picked_up_item_to_entry ( ItemPointer , & ( Me [ 0 ] . armour_item ) , Inv_Loc );
	  return;
	}
    }
  if ( ( Me [ 0 ] . drive_item . type == (-1) ) && ( ItemMap [ ItemPointer -> type ] . item_can_be_installed_in_drive_slot ) )
    {
      if ( ItemUsageRequirementsMet( ItemPointer , TRUE ) )
	{
	  raw_move_picked_up_item_to_entry ( ItemPointer , & ( Me [ 0 ] . drive_item ) , Inv_Loc );
	  return;
	}
    }
  if ( ( Me [ 0 ] . special_item . type == (-1) ) && ( ItemMap [ ItemPointer -> type ] . item_can_be_installed_in_special_slot ) )
    {
      if ( ItemUsageRequirementsMet( ItemPointer , TRUE ) )
	{
	  raw_move_picked_up_item_to_entry ( ItemPointer , & ( Me [ 0 ] . special_item ) , Inv_Loc );
	  return;
	}
    }
  
  
  // find a free position in the inventory list
  for ( InvPos = 0 ; InvPos < MAX_ITEMS_IN_INVENTORY -1 ; InvPos++ )
    {
      if ( Me[0].Inventory [ InvPos ].type == (-1) ) break;
    }
  if ( InvPos >= MAX_ITEMS_IN_INVENTORY -1 )
    {
      GiveStandardErrorMessage ( __FUNCTION__  , 
				 "Ran out of inventory positions.  This doesn't mean inventory is simpy full.\nIt means that FreedroidRPG is wasting inventory positions due to internal bugs.",
				 PLEASE_INFORM, IS_FATAL );
    }
  
  // find enough free squares in the inventory to fit
  for ( Inv_Loc.y = 0; Inv_Loc.y < InventorySize.y - ItemMap [ ItemPointer -> type ] . inv_image . inv_size . y + 1 ; Inv_Loc.y ++ )
    {
      for ( Inv_Loc.x = 0; Inv_Loc.x < InventorySize.x - ItemMap[ ItemPointer->type ] . inv_image . inv_size . x + 1 ; Inv_Loc.x ++ )
	{
	  if ( place_item_on_this_position_if_you_can ( ItemPointer , Inv_Loc , InvPos ) ) return ;
	}
    }


  if ( Me[0].Inventory[ InvPos ].inventory_position.x == (-1) )
    {
      Me[0].TextVisibleTime = 0;
      Me[0].TextToBeDisplayed = "I can't carry any more.";
      CantCarrySound();
    }
  else
    {
      raw_move_picked_up_item_to_entry ( ItemPointer , & ( Me [ 0 ] . Inventory [ InvPos ] ) , Inv_Loc );
    }
      
  
}; // void AddFloorItemDirectlyToInventory( item* ItemPointer )

#undef _items_c
