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
 * This file contains all menu functions and their subfunctions
 * ---------------------------------------------------------------------- */

#define _shop_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"
#include "SDL_rotozoom.h"

#define FIRST_MENU_ITEM_POS_X (1*Block_Width)
#define FIRST_MENU_ITEM_POS_XX ( SCREEN_WIDTH - FIRST_MENU_ITEM_POS_X )
#define FIRST_MENU_ITEM_POS_Y (BANNER_HEIGHT + FontHeight(Menu_BFont) * 3 )

#define NUMBER_OF_ITEMS_ON_ONE_SCREEN 4
#define ITEM_MENU_DISTANCE 80
#define ITEM_FIRST_POS_Y 130

// #define SELL_PRICE_FACTOR (0.25)
#define SELL_PRICE_FACTOR (0.5)
#define REPAIR_PRICE_FACTOR (0.5)

#define SHOP_ROW_LENGTH 8

typedef struct
{
  int shop_command;
  int item_selected;
}
shop_decision, *Shop_decision;

SDL_Rect ShopItemRowRect;
SDL_Rect TuxItemRowRect;

/* ----------------------------------------------------------------------
 * This function tries to buy the item given as parameter.  Currently
 * is just drops the item to the floor under the influencer and will
 * reduce influencers money.
 * ---------------------------------------------------------------------- */
int 
TryToPutItem( item* SellItem , int AmountToSellAtMost )
{
  int i;
  int FreeIndex;

  while ( SpacePressed() || EnterPressed() );

  if ( AmountToSellAtMost > SellItem -> multiplicity )
    AmountToSellAtMost = SellItem -> multiplicity ;

  //--------------------
  // At first we try to see if we can just add the multiplicity of the item in question
  // to the existing multiplicity of an item of the same type
  //
  if ( ItemMap [ SellItem->type ] . item_group_together_in_inventory )
    {
      for ( i = 0 ; i < MAX_ITEMS_PER_LEVEL ; i ++ )
	{
	  if ( curShip . AllLevels [ Me [ 0 ] . pos . z ] -> ChestItemList [ i ] . type == SellItem->type )
	    {
	      while ( 1 )
		{
		  while (EnterPressed() || SpacePressed() );

		  PlayItemSound( ItemMap[ SellItem->type ].sound_number );

		  //--------------------
		  // We add the multiplicity to the one of the similar item found in the 
		  // box.
		  //
		  curShip . AllLevels [ Me [ 0 ] . pos . z ] -> ChestItemList [ i ] . multiplicity +=
		    AmountToSellAtMost;

		  //--------------------
		  // Depending on whether all has been put or just a small part of it,
		  // we either just reduce multiplicity or completely delete the item 
		  // on the part of the giving side.
		  //
		  if ( AmountToSellAtMost < SellItem->multiplicity )
		    SellItem->multiplicity -= AmountToSellAtMost;
		  else DeleteItem( SellItem );

		  return ( TRUE );
		}
	    }
	}
    }

  //--------------------
  // Not that we know that there is no item of the desired form present in
  // the container already, we will try to find a new item entry that we can
  // use for our purposes.
  //
  FreeIndex = (-1) ;
  for ( i = 0 ; i < MAX_ITEMS_PER_LEVEL ; i ++ )
    {
      if ( curShip . AllLevels [ Me [ 0 ] . pos . z ] -> ChestItemList [ i ] . type == (-1) )
	{
	  FreeIndex = i ;
	  break;
	}
    }

  //--------------------
  // If no free index was found, then of course we must cancel the whole 
  // operation.
  //
  if ( FreeIndex == (-1) )
    {
      GiveStandardErrorMessage ( "TryToPutItem( ... )" , "\
The function used to put items into chests and containers encountered the\n\
case that there was no more room in the list of items in containers of this\n\
level.  We didn't think that this case would ever be reached, so we also didn't\n\
suitably handle it for now, i.e. we'll just quit, though it might be easy to\n\
write some suitable code here...",
				 PLEASE_INFORM, IS_FATAL );
      return ( FALSE );
    }

  //--------------------
  // Now we know that we have found a useable chest item index here
  // and we can take full advantage of it now.
  //
  CopyItem ( SellItem , & ( curShip . AllLevels [ Me [ 0 ] . pos . z ] -> ChestItemList [ i ] ) , TRUE );
  curShip . AllLevels [ Me [ 0 ] . pos . z ] -> ChestItemList [ i ] . multiplicity = AmountToSellAtMost;
  curShip . AllLevels [ Me [ 0 ] . pos . z ] -> ChestItemList [ i ] . pos . x = Me [ 0 ] . pos . x ;
  curShip . AllLevels [ Me [ 0 ] . pos . z ] -> ChestItemList [ i ] . pos . y = Me [ 0 ] . pos . y ;
  if ( AmountToSellAtMost < SellItem->multiplicity )
    SellItem->multiplicity -= AmountToSellAtMost;
  else DeleteItem( SellItem );
  
  return ( TRUE );

}; // void TryToPutItem( ... )

/* ----------------------------------------------------------------------
 * This function prepares a new set of items, that will be displayed for
 * the Tux to buy.
 *
 * This function is different from the other 'AssemblePointerList..'
 * functions in the sense, that here we really must first CREATE the items.
 *
 * Since the shop will always COPY and DELETE items and not only point
 * some pointer to different directions, recurrent calls of this function
 * should not cause any damage, as long as there is only ONE player in 
 * the game.
 * ---------------------------------------------------------------------- */
void
AssembleItemListForTradeCharacter ( item* ListToBeFilled , int ShopCharacterCode )
{
  item* ListPointer = ListToBeFilled;
  int i;

  //--------------------
  // At first we clean out the given list.
  //
  ListPointer = ListToBeFilled;
  for ( i = 0 ; i < MAX_ITEMS_IN_INVENTORY ; i ++ )
    {
      ListPointer->type = (-1) ;
      ListPointer->prefix_code = (-1) ;
      ListPointer->suffix_code = (-1) ;
      ListPointer++;
    }

  //--------------------
  // Depending on the character code given, we'll now refill the list
  // of items to be made available
  //
  ListPointer = ListToBeFilled;
  if ( ShopCharacterCode == PERSON_STONE )
    {
      ListPointer->type = ITEM_SHORT_BOW; ListPointer++;
      ListPointer->type = ITEM_HUNTERS_BOW; ListPointer++;
      ListPointer->type = ITEM_LASER_AMMUNITION; ListPointer++;
      ListPointer->type = ITEM_BUCKLER; ListPointer++;
      ListPointer->type = ITEM_SMALL_SHIELD; ListPointer++;
      ListPointer->type = ITEM_CLOAK; ListPointer++;
      ListPointer->type = ITEM_ROBE; ListPointer++;
      ListPointer->type = ITEM_DAGGER; ListPointer++;
      ListPointer->type = ITEM_SHORT_SWORD; ListPointer++;
      ListPointer->type = ITEM_SCIMITAR; ListPointer++;
      ListPointer->type = ITEM_STAFF; ListPointer++;
      ListPointer->type = ITEM_CAP; ListPointer++;
      ListPointer->type = ITEM_SMALL_HELM; ListPointer++;
    }
  else if ( ShopCharacterCode == PERSON_DOC_MOORE )
    {
      ListPointer->type = ITEM_SMALL_HEALTH_POTION; ListPointer++;
      ListPointer->type = ITEM_SMALL_MANA_POTION; ListPointer++;
      ListPointer->type = ITEM_MEDIUM_HEALTH_POTION; ListPointer++;
      ListPointer->type = ITEM_MEDIUM_MANA_POTION; ListPointer++;
    }
  else if ( ShopCharacterCode == PERSON_SKIPPY )
    {
      ListPointer->type = ITEM_MAP_MAKER_SIMPLE; ListPointer++;
    }
  else if ( ShopCharacterCode == PERSON_DUNCAN )
    {
      ListPointer->type = ITEM_VMX_GAS_GRENADE; ListPointer++;
      ListPointer->type = ITEM_EMP_SHOCK_GRENADE; ListPointer++;
      ListPointer->type = ITEM_PLASMA_GRENADE; ListPointer++;
    }
  else if ( ShopCharacterCode == PERSON_SORENSON )
    {
      ListPointer->type = ITEM_SPELLBOOK_OF_HEALING; ListPointer++;
      ListPointer->type = ITEM_SPELLBOOK_OF_EXPLOSION_CIRCLE; ListPointer++;
      ListPointer->type = ITEM_SPELLBOOK_OF_EXPLOSION_RAY; ListPointer++;
      ListPointer->type = ITEM_SPELLBOOK_OF_TELEPORT_HOME; ListPointer++;

      ListPointer->type = ITEM_SPELLBOOK_OF_PLASMA_BOLT; ListPointer++;
      ListPointer->type = ITEM_SPELLBOOK_OF_ICE_BOLT; ListPointer++;
      ListPointer->type = ITEM_SPELLBOOK_OF_POISON_BOLT; ListPointer++;
      ListPointer->type = ITEM_SPELLBOOK_OF_PETRIFICATION; ListPointer++;

      ListPointer->type = ITEM_SPELLBOOK_OF_RADIAL_EMP_WAVE; ListPointer++;
      ListPointer->type = ITEM_SPELLBOOK_OF_RADIAL_VMX_WAVE; ListPointer++;
      ListPointer->type = ITEM_SPELLBOOK_OF_RADIAL_PLASMA_WAVE; ListPointer++;
      ListPointer->type = ITEM_SPELLBOOK_OF_DETECT_ITEMS; ListPointer++;
    }
  else
    {
      GiveStandardErrorMessage ( "AssembleItemListForTradeCharacter( ... )" , "\
The function has received an unexpected character code.  This is not handled\n\
currently and therefore initiates immediate termination now...",
				 PLEASE_INFORM, IS_FATAL );
    }

  //--------------------
  // Now it's time to fill in the correct item properties and set
  // the right flags, so that we get a 'normal' item.
  //
  ListPointer = ListToBeFilled;
  for ( i = 0 ; i < MAX_ITEMS_IN_INVENTORY ; i ++ )
    {
      if ( ListPointer->type == (-1) ) break;
      FillInItemProperties( ListPointer , TRUE , 0 );
      ListPointer -> is_identified = TRUE;
      ListPointer++;
    }

}; // void AssembleItemListForTradeCharacter ( .. )
  
/* ----------------------------------------------------------------------
 * At some points in the game, like when at the shop interface or at the
 * items browser at the console, we wish to show a list of the items 
 * currently in inventory.  This function assembles this list.  It lets
 * the caller decide on whether to include worn items in the list or not
 * and it will return the number of items finally filled into that list.
 * ---------------------------------------------------------------------- */
int
AssemblePointerListForItemShow ( item** ItemPointerListPointer , int IncludeWornItems, int PlayerNum )
{
  int i;
  item** CurrentItemPointer;
  int NumberOfItems = 0 ;

  //--------------------
  // First we clean out the new Show_Pointer_List
  //
  CurrentItemPointer = ItemPointerListPointer ;
  for ( i = 0 ; i < MAX_ITEMS_IN_INVENTORY ; i ++ )
    {
      *CurrentItemPointer = NULL;
      CurrentItemPointer++;
    }

  //--------------------
  // Now we start to fill the Show_Pointer_List with the items
  // currently equipped, if that is what is desired by parameters...
  //
  CurrentItemPointer = ItemPointerListPointer;
  if ( IncludeWornItems )
    {
      if ( Me [ PlayerNum ] .weapon_item.type != ( -1 ) )
	{
	  *CurrentItemPointer = & ( Me [ PlayerNum ] .weapon_item );
	  CurrentItemPointer ++;
	  NumberOfItems ++;
	}
      if ( Me [ PlayerNum ] .drive_item.type != ( -1 ) )
	{
	  *CurrentItemPointer = & ( Me [ PlayerNum ] .drive_item );
	  CurrentItemPointer ++;
	  NumberOfItems ++;
	}
      if ( Me [ PlayerNum ] .armour_item.type != ( -1 ) )
	{
	  *CurrentItemPointer = & ( Me [ PlayerNum ] .armour_item );
	  CurrentItemPointer ++;
	  NumberOfItems ++;
	}
      if ( Me [ PlayerNum ] .shield_item.type != ( -1 ) )
	{
	  *CurrentItemPointer = & ( Me [ PlayerNum ] .shield_item );
	  CurrentItemPointer ++;
	  NumberOfItems ++;
	}
      if ( Me [ PlayerNum ] .special_item.type != ( -1 ) )
	{
	  *CurrentItemPointer = & ( Me [ PlayerNum ] .special_item );
	  CurrentItemPointer ++;
	  NumberOfItems ++;
	}
      if ( Me [ PlayerNum ] .aux1_item.type != ( -1 ) )
	{
	  *CurrentItemPointer = & ( Me [ PlayerNum ] .aux1_item );
	  CurrentItemPointer ++;
	  NumberOfItems ++;
	}
      if ( Me [ PlayerNum ] .aux2_item.type != ( -1 ) )
	{
	  *CurrentItemPointer = & ( Me [ PlayerNum ] .aux2_item );
	  CurrentItemPointer ++;
	  NumberOfItems ++;
	}
    }
  
  //--------------------
  // Now we start to fill the Show_Pointer_List with the items in the
  // pure unequipped inventory
  //
  for ( i = 0 ; i < MAX_ITEMS_IN_INVENTORY ; i ++ )
    {
      if ( Me [ PlayerNum ] .Inventory [ i ].type == (-1) ) continue;
      else
	{
	  *CurrentItemPointer = & ( Me [ PlayerNum ] .Inventory[ i ] );
	  CurrentItemPointer ++;
	  NumberOfItems ++;
	}
    }
  
  return ( NumberOfItems );
  
}; // void AssemblePointerListForItemShow ( .. )
  
/* ----------------------------------------------------------------------
 * Before we enter the chest put/take interface menu, we must assemble
 * the list of pointers to the items currently in this chest.  An item
 * is in the chest at x/y if it's in the chest-item-list of the current
 * players level and if it's coordinates are those of the current players
 * chest.
 * ---------------------------------------------------------------------- */
int
AssemblePointerListForChestShow ( item** ItemPointerListPointer , int PlayerNum )
{
  int i;
  item** CurrentItemPointer;
  int NumberOfItems = 0 ;

  //--------------------
  // First we clean out the new Show_Pointer_List
  //
  CurrentItemPointer = ItemPointerListPointer ;
  for ( i = 0 ; i < MAX_ITEMS_IN_INVENTORY ; i ++ )
    {
      *CurrentItemPointer = NULL;
      CurrentItemPointer++;
    }

  CurrentItemPointer = ItemPointerListPointer;

  for ( i = 0 ; i < MAX_ITEMS_PER_LEVEL ; i ++ )
    {
      if ( curShip . AllLevels [ Me [ PlayerNum ] . pos . z ] -> ChestItemList [ i ] . type == (-1) ) continue;

      //--------------------
      // All the items in chests within a range of 1 square around the Tux 
      // will be collected together to be shown in the chest inventory.
      //
      if ( sqrt ( ( Me [ PlayerNum ] . pos . x - curShip . AllLevels [ Me [ PlayerNum ] . pos . z ] -> ChestItemList [ i ] . pos . x ) *
		  ( Me [ PlayerNum ] . pos . x - curShip . AllLevels [ Me [ PlayerNum ] . pos . z ] -> ChestItemList [ i ] . pos . x ) +
		  ( Me [ PlayerNum ] . pos . y - curShip . AllLevels [ Me [ PlayerNum ] . pos . z ] -> ChestItemList [ i ] . pos . y )  *
		  ( Me [ PlayerNum ] . pos . y - curShip . AllLevels [ Me [ PlayerNum ] . pos . z ] -> ChestItemList [ i ] . pos . y )  ) < 1 )
	{
	  *CurrentItemPointer = & ( curShip . AllLevels [ Me [ PlayerNum ] . pos . z ] -> ChestItemList [ i ] );
	  CurrentItemPointer ++;
	  NumberOfItems ++;
	}
    }
  
  return ( NumberOfItems );
  
}; // void AssemblePointerListForChestShow ( .. )
  
/* ----------------------------------------------------------------------
 * Maybe the user has clicked right onto the item overview row.  Then of
 * course we must find out and return the index of the item clicked on.
 * If no item was clicked on, then a -1 will be returned as index.
 * ---------------------------------------------------------------------- */
int
ClickWasOntoItemRowPosition ( int x , int y , int TuxItemRow )
{

  if ( TuxItemRow )
    {
      if ( y < TuxItemRowRect . y ) return (-1) ;
      if ( y > TuxItemRowRect . y + TuxItemRowRect.h ) return (-1) ;
      if ( x < TuxItemRowRect . x ) return (-1) ;
      if ( x > TuxItemRowRect . x + TuxItemRowRect.w ) return (-1) ;
      
      //--------------------
      // Now at this point we know, that the click really was in the item
      // overview row.  Therefore we just need to find out the index and
      // can return;
      //
      return ( ( x - TuxItemRowRect . x ) / INITIAL_BLOCK_WIDTH );
    }
  else
    {
      if ( y < ShopItemRowRect . y ) return (-1) ;
      if ( y > ShopItemRowRect . y + ShopItemRowRect.h ) return (-1) ;
      if ( x < ShopItemRowRect . x ) return (-1) ;
      if ( x > ShopItemRowRect . x + ShopItemRowRect.w ) return (-1) ;
      
      //--------------------
      // Now at this point we know, that the click really was in the item
      // overview row.  Therefore we just need to find out the index and
      // can return;
      //
      return ( ( x - ShopItemRowRect . x ) / INITIAL_BLOCK_WIDTH );
    }
}; // int ClickWasOntoItemRowPosition ( int x , int y , int TuxItemRow )

/* ----------------------------------------------------------------------
 * The item row in the shop interface (or whereever we're going to use it)
 * should display not only the rotating item display but also a row or a
 * column of the current equipment, so that some better overview is given
 * as well and the item can be better associated with it's in-game inventory
 * representation.  This function displays one such representation with 
 * the correct size to fit perfectly into the overview item row.
 * ---------------------------------------------------------------------- */
void
ShowRescaledItem ( int position , int TuxItemRow , item* ShowItem )
{
  int PictureIndex;
  float RescaleFactor;
  SDL_Rect TargetRectangle;
  static int IsFirstFunctionCall = TRUE;
  static SDL_Surface* ScaledItemImageBackups[ NUMBER_OF_ITEM_PICTURES ];
  SDL_Surface* tmp_surface;
  int i;

  if ( IsFirstFunctionCall )
    {
      for ( i = 0 ; i < NUMBER_OF_ITEM_PICTURES ; i ++ )
	ScaledItemImageBackups [ i ] = NULL ;
      IsFirstFunctionCall = FALSE ;
    }

  TuxItemRowRect . x = 55 ;
  TuxItemRowRect . y = 410;
  TuxItemRowRect . h = INITIAL_BLOCK_HEIGHT;
  TuxItemRowRect . w = INITIAL_BLOCK_WIDTH * SHOP_ROW_LENGTH ;

  ShopItemRowRect . x = 55 ;
  ShopItemRowRect . y = 10;
  ShopItemRowRect . h = INITIAL_BLOCK_HEIGHT;
  ShopItemRowRect . w = INITIAL_BLOCK_WIDTH * SHOP_ROW_LENGTH ;

  if ( TuxItemRow )
    {
      TargetRectangle . x = TuxItemRowRect . x + position * INITIAL_BLOCK_WIDTH ;
      TargetRectangle . y = TuxItemRowRect . y ;
    }
  else
    {
      TargetRectangle . x = ShopItemRowRect . x + position * INITIAL_BLOCK_WIDTH ;
      TargetRectangle . y = ShopItemRowRect . y ;
    }

  PictureIndex = ItemMap [ ShowItem->type ] . picture_number ;

  if ( ( ItemImageList[ PictureIndex ] . inv_size . x == 1 ) &&
       ( ItemImageList[ PictureIndex ] . inv_size . y == 1 ) )
    RescaleFactor = 2.0 ;
  else if ( ItemImageList[ PictureIndex ] . inv_size . y == 3 ) 
    RescaleFactor = 2.0 / 3.0 ;
  else RescaleFactor = 1.0;

  if ( ScaledItemImageBackups [ PictureIndex ] == NULL )
    {
      DebugPrintf ( 1 , "\nShowRescaledItem:  first call, so scaling has to be done...\n" );
      tmp_surface = zoomSurface ( ItemImageList[ PictureIndex ] . Surface , RescaleFactor , RescaleFactor , FALSE );
      ScaledItemImageBackups [ PictureIndex ] = SDL_DisplayFormatAlpha ( tmp_surface ) ;
      SDL_FreeSurface ( tmp_surface );
    }
  
  SDL_BlitSurface( ScaledItemImageBackups [ PictureIndex ] , NULL , Screen , &TargetRectangle );

}; // void ShowRescaledItem ( int position , item* ShowItem )

/* ----------------------------------------------------------------------
 * This function does the item show when the user has selected item
 * show from the console menu.
 * ---------------------------------------------------------------------- */
int
GreatShopInterface ( int NumberOfItems , item* ShowPointerList[ MAX_ITEMS_IN_INVENTORY ] , 
		     int NumberOfItemsInTuxRow , item* TuxItemsList[ MAX_ITEMS_IN_INVENTORY] , 
		     shop_decision* ShopOrder , int ShowChestButtons )
{
  int Displacement=0;
  bool finished = FALSE;
  static int WasPressed = FALSE ;
  int i;
  int ClickTarget;
  static int RowStart=0;
  static int TuxRowStart=0;
  static int ItemIndex=0;
  static int TuxItemIndex=-1;
  int RowLength=SHOP_ROW_LENGTH;
  int TuxRowLength=SHOP_ROW_LENGTH;
  char GoldString[1000];
  SDL_Rect HighlightRect;
  int BuyButtonActive = FALSE ;
  int SellButtonActive = FALSE ;
  int Buy10ButtonActive = FALSE ;
  int Sell10ButtonActive = FALSE ;
  int Buy100ButtonActive = FALSE ;
  int Sell100ButtonActive = FALSE ;

  //--------------------
  // We add some secutiry against indexing beyond the
  // range of items given in the list.
  //
  if ( RowLength > NumberOfItems ) RowLength = NumberOfItems;
  while ( ItemIndex >= NumberOfItems ) ItemIndex -- ;
  while ( RowStart + RowLength > NumberOfItems ) RowStart -- ;
  if ( RowStart < 0 ) RowStart = 0 ;

  if ( TuxRowLength > NumberOfItemsInTuxRow ) TuxRowLength = NumberOfItemsInTuxRow;
  while ( TuxItemIndex >= NumberOfItemsInTuxRow ) TuxItemIndex -- ;
  while ( TuxRowStart + TuxRowLength > NumberOfItemsInTuxRow ) TuxRowStart -- ;
  if ( TuxRowStart < 0 ) TuxRowStart = 0 ;

  if ( NumberOfItemsInTuxRow <= 0 ) 
    TuxItemIndex = (-1) ;
  if ( NumberOfItems <= 0 ) 
    ItemIndex = (-1) ;


  //--------------------
  // We initialize the text rectangle
  //
  Cons_Text_Rect . x = 258 ;
  Cons_Text_Rect . y = 108 ;
  Cons_Text_Rect . w = 346 ;
  Cons_Text_Rect . h = 255 ;

  /*
  if ( ( ShowPointerList[0] == NULL ) && ( TuxItemsList[0] == NULL ) )
    {
      MenuTexts[0]=" BACK ";
      MenuTexts[1]="";
      DoMenuSelection ( " YOU DONT HAVE ANYTHING IN INVENTORY, THAT COULD BE VIEWED. " , 
			MenuTexts, 1 , NULL , NULL );
      ShopOrder -> item_selected = -1 ;
      ShopOrder -> shop_command = DO_NOTHING ;
      return (-1) ;
    }
  */

  // ItemType = ShowPointerList [ ItemIndex ] -> type ;

  Displacement = 0;

  while (!finished)
    {

      usleep ( 35 );
      ShopOrder -> shop_command = DO_NOTHING ;

      //--------------------
      // We show all the info and the buttons that should be in this
      // interface...
      //
      if ( ItemIndex >= 0 )
	ShowItemInfo ( ShowPointerList [ ItemIndex ] , Displacement , TRUE , "backgrounds/item_browser_shop.jpg" );
      else if ( TuxItemIndex >= 0 )
	ShowItemInfo ( TuxItemsList [ TuxItemIndex ] , Displacement , TRUE , "backgrounds/item_browser_shop.jpg" );
      else DisplayImage ( find_file( "backgrounds/item_browser_shop.jpg" , GRAPHICS_DIR, FALSE) );

      for ( i = 0 ; i < RowLength ; i++ )
	{
	  ShowRescaledItem ( i , FALSE , ShowPointerList [ i + RowStart ] );
	}

      for ( i = 0 ; i < TuxRowLength ; i++ )
	{
	  ShowRescaledItem ( i , TRUE , TuxItemsList [ i + TuxRowStart ] );
	}

      if ( ItemIndex >= 0 ) 
	{
	  HighlightRect . x = ShopItemRowRect . x + ( ItemIndex - RowStart ) * INITIAL_BLOCK_WIDTH ;
	  HighlightRect . y = ShopItemRowRect . y ;
	  HighlightRect . w = INITIAL_BLOCK_WIDTH ;
	  HighlightRect . h = INITIAL_BLOCK_HEIGHT ;
	  HighlightRectangle ( Screen , HighlightRect );
	}
      if ( TuxItemIndex >= 0 )
	{
	  HighlightRect . x = TuxItemRowRect . x + ( TuxItemIndex - TuxRowStart ) * INITIAL_BLOCK_WIDTH ;
	  HighlightRect . y = TuxItemRowRect . y ;
	  HighlightRect . w = INITIAL_BLOCK_WIDTH ;
	  HighlightRect . h = INITIAL_BLOCK_HEIGHT ;
	  HighlightRectangle ( Screen , HighlightRect );
	}

      ShowGenericButtonFromList ( LEFT_SHOP_BUTTON );
      ShowGenericButtonFromList ( RIGHT_SHOP_BUTTON );

      ShowGenericButtonFromList ( LEFT_TUX_SHOP_BUTTON );
      ShowGenericButtonFromList ( RIGHT_TUX_SHOP_BUTTON );

      if ( ItemIndex >= 0 )
	{
	  if ( ShowChestButtons ) ShowGenericButtonFromList ( TAKE_BUTTON );
	  else ShowGenericButtonFromList ( BUY_BUTTON );
	  BuyButtonActive = TRUE; 
	  SellButtonActive = FALSE ;
	  Sell10ButtonActive = FALSE ;
	  Sell100ButtonActive = FALSE ;
	  if ( ItemMap [ ShowPointerList [ ItemIndex ] -> type ] . item_group_together_in_inventory )
	    {
	      if ( ShowChestButtons )
		{
		  ShowGenericButtonFromList ( TAKE_10_BUTTON );
		  ShowGenericButtonFromList ( TAKE_100_BUTTON );
		}
	      else
		{
		  ShowGenericButtonFromList ( BUY_10_BUTTON );
		  ShowGenericButtonFromList ( BUY_100_BUTTON );
		}
	      Buy10ButtonActive = TRUE ;
	      Buy100ButtonActive = TRUE ;
	    }
	}
      else if ( TuxItemIndex >= 0 )
	{
	  if ( ShowChestButtons ) ShowGenericButtonFromList ( PUT_BUTTON );
	  else ShowGenericButtonFromList ( SELL_BUTTON );
	  SellButtonActive = TRUE; 
	  BuyButtonActive = FALSE ;
	  Buy10ButtonActive = FALSE ;
	  Buy100ButtonActive = FALSE ;
	  if ( ItemMap [ TuxItemsList [ TuxItemIndex ] -> type ] . item_group_together_in_inventory )
	    {
	      if ( ShowChestButtons ) 
		{
		  ShowGenericButtonFromList ( PUT_10_BUTTON );
		  ShowGenericButtonFromList ( PUT_100_BUTTON );
		}
	      else
		{
		  ShowGenericButtonFromList ( SELL_10_BUTTON );
		  ShowGenericButtonFromList ( SELL_100_BUTTON );
		}
	      Sell10ButtonActive = TRUE; 
	      Sell100ButtonActive = TRUE; 
	    }
	}
      else
	{
	  BuyButtonActive = FALSE ;
	  Buy10ButtonActive = FALSE ;
	  Buy100ButtonActive = FALSE ;
	  SellButtonActive = FALSE ;
	  Sell10ButtonActive = FALSE ;
	  Sell100ButtonActive = FALSE ;
	}

      sprintf ( GoldString , "%d." , (int) Me [ 0 ] . Gold );
      PutStringFont ( Screen , Menu_BFont, 90, 141, GoldString );

      SDL_Flip( Screen );

      if (SpacePressed() || EscapePressed() || axis_is_active )
	{
	  /*
	  if ( CursorIsOnButton( ITEM_BROWSER_RIGHT_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) && axis_is_active && !WasPressed )
	    {
	      if ( ItemIndex < NumberOfItems -1 ) 
		{
		  ItemIndex ++;	    
		  MoveMenuPositionSound();
		  Displacement = 0;
		}
	    }
	  else if ( CursorIsOnButton( ITEM_BROWSER_LEFT_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) && axis_is_active && !WasPressed )
	    {
	      if ( ItemIndex > 0) 
		{
		  ItemIndex --;	      
		  MoveMenuPositionSound();
		  Displacement = 0;
		}
	    }
	  else 
	  */
	  if ( CursorIsOnButton( UP_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) && axis_is_active && !WasPressed )
	    {
	      MoveMenuPositionSound();
	      Displacement += FontHeight ( GetCurrentFont () );
	    }
	  else if ( CursorIsOnButton( DOWN_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) && axis_is_active && !WasPressed )
	    {
	      MoveMenuPositionSound();
	      Displacement -= FontHeight ( GetCurrentFont () );
	    }
	  else if ( CursorIsOnButton( ITEM_BROWSER_EXIT_BUTTON , GetMousePos_x ( ) + 16 , GetMousePos_y ( ) + 16 ) && axis_is_active && !WasPressed )
	    {
	      finished = TRUE;
	      while (SpacePressed() ||EscapePressed());
	    }
	  else if ( CursorIsOnButton( LEFT_TUX_SHOP_BUTTON , GetMousePos_x ( ) + 16 , GetMousePos_y ( ) + 16 ) && axis_is_active && !WasPressed )
	    {
	      if ( 0 < RowStart ) 
		{
		  RowStart --;
		  if ( ( ItemIndex != (-1) ) && ( ItemIndex >= RowStart + RowLength ) ) ItemIndex --;
		}
	      MoveMenuPositionSound();
	      while (SpacePressed() ||EscapePressed());
	    }
	  else if ( CursorIsOnButton( RIGHT_TUX_SHOP_BUTTON , GetMousePos_x ( ) + 16 , GetMousePos_y ( ) + 16 ) && axis_is_active && !WasPressed )
	    {
	      if ( RowStart + RowLength < NumberOfItems ) 
		{
		  RowStart ++;
		  if ( ( ItemIndex != (-1) ) && ( ItemIndex < RowStart ) ) ItemIndex++;
		}
	      MoveMenuPositionSound();
	      while (SpacePressed() ||EscapePressed());
	    }
	  else if ( CursorIsOnButton( LEFT_SHOP_BUTTON , GetMousePos_x ( ) + 16 , GetMousePos_y ( ) + 16 ) && axis_is_active && !WasPressed )
	    {
	      if ( 0 < TuxRowStart ) 
		{
		  TuxRowStart --;
		  if ( ( TuxItemIndex != (-1) ) && ( TuxItemIndex >= TuxRowStart + TuxRowLength ) ) TuxItemIndex --;
		}
	      MoveMenuPositionSound();
	      while (SpacePressed() ||EscapePressed());
	    }
	  else if ( CursorIsOnButton( RIGHT_SHOP_BUTTON , GetMousePos_x ( ) + 16 , GetMousePos_y ( ) + 16 ) && axis_is_active && !WasPressed )
	    {
	      if ( TuxRowStart + TuxRowLength < NumberOfItemsInTuxRow ) 
		{
		  TuxRowStart ++;
		  if ( ( TuxItemIndex != (-1) ) && ( TuxItemIndex < TuxRowStart ) ) TuxItemIndex ++ ;
		}
	      MoveMenuPositionSound();
	      while (SpacePressed() ||EscapePressed());
	    }
	  else if ( ( ( ClickTarget = ClickWasOntoItemRowPosition ( GetMousePos_x ( ) + 16 , GetMousePos_y ( ) + 16 , FALSE ) ) >= 0 ) && axis_is_active && !WasPressed )
	    {
	      if ( ClickTarget < NumberOfItems )
		{
		  ItemIndex = RowStart + ClickTarget ;
		  TuxItemIndex = (-1) ;
		}
	    }
	  else if ( ( ( ClickTarget = ClickWasOntoItemRowPosition ( GetMousePos_x ( ) + 16 , GetMousePos_y ( ) + 16 , TRUE ) ) >= 0 ) && axis_is_active && !WasPressed )
	    {
	      if ( ClickTarget < NumberOfItemsInTuxRow )
		{
		  TuxItemIndex = TuxRowStart + ClickTarget ;
		  ItemIndex = (-1) ;
		}
	    }
	  else if ( CursorIsOnButton( BUY_BUTTON , GetMousePos_x ( ) + 16 , GetMousePos_y ( ) + 16 ) && axis_is_active && !WasPressed )
	    {
	      if ( BuyButtonActive )
		{
		  ShopOrder -> item_selected = ItemIndex ;
		  ShopOrder -> shop_command = BUY_1_ITEM ;
		  return ( 0 );
		}
	      else if ( SellButtonActive )
		{
		  ShopOrder -> item_selected = TuxItemIndex ;
		  ShopOrder -> shop_command = SELL_1_ITEM ;
		  return ( 0 );
		}
	    }
	  else if ( CursorIsOnButton( BUY_10_BUTTON , GetMousePos_x ( ) + 16 , GetMousePos_y ( ) + 16 ) && axis_is_active && !WasPressed )
	    {
	      if ( Buy10ButtonActive )
		{
		  ShopOrder -> item_selected = ItemIndex ;
		  ShopOrder -> shop_command = BUY_10_ITEMS ;
		  return ( 0 );
		}
	      else if ( Sell10ButtonActive )
		{
		  ShopOrder -> item_selected = TuxItemIndex ;
		  ShopOrder -> shop_command = SELL_10_ITEMS ;
		  return ( 0 );
		}
	    }
	  else if ( CursorIsOnButton( BUY_100_BUTTON , GetMousePos_x ( ) + 16 , GetMousePos_y ( ) + 16 ) && axis_is_active && !WasPressed )
	    {
	      if ( Buy100ButtonActive )
		{
		  ShopOrder -> item_selected = ItemIndex ;
		  ShopOrder -> shop_command = BUY_100_ITEMS ;
		  return ( 0 );
		}
	      else if ( Sell100ButtonActive )
		{
		  ShopOrder -> item_selected = TuxItemIndex ;
		  ShopOrder -> shop_command = SELL_100_ITEMS ;
		  return ( 0 );
		}
	    }
	}

      WasPressed = axis_is_active;

      if (UpPressed() || MouseWheelUpPressed())
	{
	  MoveMenuPositionSound();
	  while (UpPressed());
	  Displacement += FontHeight ( GetCurrentFont () );
	}
      if (DownPressed() || MouseWheelDownPressed())
	{
	  MoveMenuPositionSound();
	  while (DownPressed());
	  Displacement -= FontHeight ( GetCurrentFont () );
	}
      if (RightPressed() )
	{
	  MoveMenuPositionSound();
	  while (RightPressed());
	  // if ( ItemType < Me[0].type) ItemType ++;
	}
      if (LeftPressed() )
	{
	  MoveMenuPositionSound();
	  while (LeftPressed());
	  // if (ItemType > 0) ItemType --;
	}
      
      if ( EscapePressed() )
	{
	  while ( EscapePressed() );
	  return (-1);
	}

    } // while !finished 

  return ( -1 ) ;  // Currently equippment selection is not yet possible...

}; // int GreatShopInterface ( int NumberOfItems , item* ShowPointerList[ MAX_ITEMS_IN_INVENTORY ] )


/* ----------------------------------------------------------------------
 * This function tells us which item in the menu has been clicked upon.
 * It does not check for lower than 4 items in the menu available.
 * ---------------------------------------------------------------------- */
int
ClickedMenuItemPosition( void )
{
  int CursorX, CursorY;
  int i;

  CursorX = GetMousePos_x() + 16 ; // this is already the position corrected for 16 pixels!!
  CursorY = GetMousePos_y() + 16 ; // this is already the position corrected for 16 pixels!!

#define ITEM_MENU_DISTANCE 80
#define ITEM_FIRST_POS_Y 130
#define NUMBER_OF_ITEMS_ON_ONE_SCREEN 4

  //--------------------
  // When a character is blitted to the screen at x y, then the x and y
  // refer to the top left corner of the coming blit.  Using this information
  // we will define the areas where a click 'on the blitted text' has occured
  // or not.
  //
  if ( CursorY < ITEM_FIRST_POS_Y )
    return (-1);
  if ( CursorY > ITEM_FIRST_POS_Y + NUMBER_OF_ITEMS_ON_ONE_SCREEN * ITEM_MENU_DISTANCE )
    return (-1);

  for ( i = 0 ; i < NUMBER_OF_ITEMS_ON_ONE_SCREEN ; i++ )
    {
      if ( CursorY < ITEM_FIRST_POS_Y + ( i+1 ) * ITEM_MENU_DISTANCE ) return i;
    }
  
  //--------------------
  // At this point we've already determined and returned to right click-area.
  // if this point is ever reached, a severe error has occured, and Freedroid
  // should therefore also say so.
  //
  GiveStandardErrorMessage ( "ClickedMenuItemPosition(...)" , "\
The MENU CODE was unable to properly resolve a mouse button press.",
				 PLEASE_INFORM, IS_FATAL );

  return ( 3 ); // to make compilers happy :)

}; // int ClickedMenuItemPosition( void )

/* ----------------------------------------------------------------------
 * There are numerous functions in shops where you have to select one out
 * of may pieces of equippment.  Therefore a function is provided, that 
 * should be able to perform this selection process with the user and also
 * check for unwanted events, like non-present items selected and that.
 *
 * Several pricing methods exist and can be given as parameter, useful
 * for using this function either for buy, sell, repair or identification
 * purposes.
 *
 * ---------------------------------------------------------------------- */
#define PRICING_FOR_BUY 1
#define PRICING_FOR_SELL 2
#define PRICING_FOR_IDENTIFY 3
#define PRICING_FOR_REPAIR 4
int 
DoEquippmentListSelection( char* Startstring , item* Item_Pointer_List[ MAX_ITEMS_IN_INVENTORY ] , int PricingMethod )
{
  int Pointer_Index=0;
  int i;
  int InMenuPosition = 0;
  int MenuInListPosition = 0;
  char DescriptionText[5000];
  float PriceFound;

  //--------------------
  // At first we hide the mouse cursor, so that there can not be any
  // ambiguity whether to thing of the tux cursor or the mouse cursor
  // to be the pointer we use.
  //
  SDL_ShowCursor( SDL_DISABLE );

  //--------------------
  // First we make sure, that neither space nor Escape are
  // pressed in the beginning, so that a real menu selection
  // will be done.
  //
  while ( SpacePressed() || EscapePressed() );

  //--------------------
  // At first we count how many items are really in this list.
  // The last one is indicated by a NULL pointer following directly
  // after it.
  //
  for ( i = 0 ; i < MAX_ITEMS_IN_INVENTORY ; i++ )
    {
      if ( Item_Pointer_List[ i ] == NULL ) 
	{
	  Pointer_Index = i ;
	  break;
	}
    }

  //--------------------
  // Maybe there is NO ITEM IN THE LIST AT ALL!
  // Then of course we are done, hehe, and return the usual 'back'.
  // Thats perfectly normal and ok.
  //
  if ( Pointer_Index == 0 )
    {
      DebugPrintf ( 0 , "\nDoEquippmentListSelection(..):  No item in given list.  Returning... " );
      return ( -1 );
    }

  //--------------------
  // Now we clean the list from any (-1) items, that might come from
  // an items just having been sold and therefore deleted from the list.
  //
  for ( i = 0 ; i < Pointer_Index ; i++ )
    {
      if ( Item_Pointer_List[ i ]->type == (-1 ) )
	{
	  DebugPrintf ( 0 , "\nNOTE:  DoEquippmentListSelection(...): Cleaning a '-1' type item from the Item_Pointer_List... " );

	  CopyItem ( Item_Pointer_List[ Pointer_Index - 1 ] , Item_Pointer_List[ i ] , FALSE );
	  DeleteItem ( Item_Pointer_List[ Pointer_Index - 1 ] ); // this is for -1 cause of SOLD items
	  Item_Pointer_List[ Pointer_Index -1 ] = NULL;
	  Pointer_Index --;

	  return ( DoEquippmentListSelection ( Startstring , Item_Pointer_List , PricingMethod ) );
	}
    }

  //--------------------
  // Now IN CASE OF REPAIR_PRICING, we clean the list from any items, that 
  // don't need repair.
  //
  // TAKE CARE, THAT ITEMS MUST NOT BE OVERWRITTEN, ONLY THE POINTERS MAY BE MANIPULATED!!
  // Or we would destroy the Tux' inventory list!
  //
  if ( PricingMethod == PRICING_FOR_REPAIR )
    {
      for ( i = 0 ; i < Pointer_Index ; i++ )
	{
	  if ( Item_Pointer_List[ i ]->current_duration == Item_Pointer_List[ i ]->max_duration ) 
	    {
	      DebugPrintf ( 0 , "\nNOTE:  DoEquippmentListSelection(...): Cleaning an item that does not need repair from the pointer list... " );
	      
	      // CopyItem ( Item_Pointer_List[ Pointer_Index - 1 ] , Item_Pointer_List[ i ] , FALSE );
	      Item_Pointer_List[ i ] = Item_Pointer_List[ Pointer_Index -1 ] ;
	      Item_Pointer_List[ Pointer_Index -1 ] = NULL;
	      Pointer_Index --;
	      
	      return ( DoEquippmentListSelection ( Startstring , Item_Pointer_List , PricingMethod ) );
	    }
	}
    }

  //--------------------
  // Now IN CASE OF IDENTIFY_PRICING, we clean the list from any items, that 
  // don't need to be identified.
  //
  // TAKE CARE, THAT ITEMS MUST NOT BE OVERWRITTEN, ONLY THE POINTERS MAY BE MANIPULATED!!
  // Or we would destroy the Tux' inventory list!
  //
  if ( PricingMethod == PRICING_FOR_IDENTIFY )
    {
      for ( i = 0 ; i < Pointer_Index ; i++ )
	{
	  if ( Item_Pointer_List[ i ]->is_identified ) 
	    {
	      DebugPrintf ( 0 , "\nNOTE:  DoEquippmentListSelection(...): Cleaning an item that does not need to be identified from the pointer list... " );
	      
	      Item_Pointer_List[ i ] = Item_Pointer_List[ Pointer_Index -1 ] ;
	      Item_Pointer_List[ Pointer_Index -1 ] = NULL;
	      Pointer_Index --;
	      
	      return ( DoEquippmentListSelection ( Startstring , Item_Pointer_List , PricingMethod ) );
	    }
	}
    }



  //--------------------
  // Now we can perform the actual menu selection.
  // We will loop until a decision of one kind or the other
  // has been made.
  //
  while ( !SpacePressed() && !EscapePressed() )
    {
      // InitiateMenu( NULL );
      InitiateMenu( SHOP_BACKGROUND_IMAGE );

      //--------------------
      // Now we draw our selection of items to the screen, at least the part
      // of it, that's currently visible
      //
      DisplayText( Startstring , 50 , 50 + (0) * ITEM_MENU_DISTANCE , NULL );

      // DisplayText( DescriptionText , 580 , 50 + ( 0 ) * 80 , NULL );
      for ( i = 0 ; ( (i < NUMBER_OF_ITEMS_ON_ONE_SCREEN) && ( Item_Pointer_List[ i + MenuInListPosition ] != NULL ) ) ; i++ )
	{
	  // DisplayText( ItemMap [ Repair_Pointer_List[ i + ]->type ].item_name , 50 , 50 + i * 50 , NULL );
	  // DisplayText( "\n" , -1 , -1, NULL );
	  GiveItemDescription( DescriptionText , Item_Pointer_List [ i + MenuInListPosition ] , TRUE );
	  DisplayText( DescriptionText , 50 , 50 + (i+1) * ITEM_MENU_DISTANCE , NULL );

	  //--------------------
	  // Now we print out the price for this item, depending of course
	  // on the context in which we display this item.
	  //
	  switch ( PricingMethod )
	    {
	    case PRICING_FOR_SELL:
	      PriceFound = SELL_PRICE_FACTOR * 
		( (float) CalculateItemPrice ( Item_Pointer_List[ i + MenuInListPosition] , FALSE ) ) ;
	      break;
	    case PRICING_FOR_BUY:
	      PriceFound = 
		CalculateItemPrice ( Item_Pointer_List [ i + MenuInListPosition ] , FALSE ) ;
	      break;
	    case PRICING_FOR_IDENTIFY:
	      PriceFound = 100.0 ;
	      break;
	    case PRICING_FOR_REPAIR:
	      PriceFound = REPAIR_PRICE_FACTOR * 
		( (float) CalculateItemPrice ( Item_Pointer_List[ i + MenuInListPosition] , TRUE ) );
	      break;
	    default:
	      DebugPrintf( 0 , "ERROR:  PRICING METHOD UNSPECIFIED IN SHOP.C!!!\n\nTerminating...\n\n" );
	      Terminate ( ERR );
	      break;

	    }
	  sprintf( DescriptionText , "%6.0f" , PriceFound );
	  DisplayText( DescriptionText , 560 , 50 + (i+1) * ITEM_MENU_DISTANCE , NULL );
	}

      //--------------------
      // Now we add a 'BACK' button outside the normal item display area.
      // Where exactly we put this button is pretty much unimportant, cause any
      // click outside the items displayed will return -1 to the calling function
      // and that's the same as clicking directly on the back button.
      //
      // DisplayText( "BACK" , 580 , 50 + (i+1) * ITEM_MENU_DISTANCE , NULL );
      // CenteredPutStringFont( Screen , Menu_Filled_BFont , 50 + (i+1) * ITEM_MENU_DISTANCE , " BACK " );
      //
      CenteredPutString( Screen , 50 + (i+1) * ITEM_MENU_DISTANCE , " BACK " );
      
      //--------------------
      // Now we draw the influencer as a cursor
      //
      PutInfluence ( 10 , 50 + ( InMenuPosition + 1 ) * ITEM_MENU_DISTANCE , 0 );

      //--------------------
      //
      //
      SDL_Flip ( Screen );

      //--------------------
      // Maybe the cursor key up or cursor key down was pressed.  Then of
      // course the cursor must either move down or the whole menu must
      // scroll one step down, if that is still possible.
      // 
      // Mouse wheel action will be checked for further down.
      //
      if ( UpPressed() || MouseWheelUpPressed() )
	{
	  if ( InMenuPosition > 0 ) InMenuPosition --;
	  else 
	    {
	      if ( MenuInListPosition > 0 )
		MenuInListPosition --;
	    }
	  while ( UpPressed() );
	}
      if ( DownPressed() || MouseWheelDownPressed() )
	{
	  if ( ( InMenuPosition < NUMBER_OF_ITEMS_ON_ONE_SCREEN - 1 ) &&
	       ( InMenuPosition < Pointer_Index -1 ) )
	    {
	      InMenuPosition ++;
	    }
	  else 
	    {
	      if ( MenuInListPosition < Pointer_Index - NUMBER_OF_ITEMS_ON_ONE_SCREEN )
		MenuInListPosition ++;
	    }
	  while ( DownPressed() );
	}      

    } // while not space pressed...

  if ( SpacePressed() || axis_is_active ) 
    {
      SDL_ShowCursor( SDL_ENABLE );
      return ( InMenuPosition + MenuInListPosition ) ;
    }

  while ( SpacePressed() || EscapePressed() );

  SDL_ShowCursor( SDL_ENABLE );
  return (-1); // just to make compilers happy :)

}; // int DoEquippmentListSelection( char* Startstring , item* Item_Pointer_List[ MAX_ITEMS_IN_INVENTORY ] )

/* ----------------------------------------------------------------------
 * This function tries to buy the item given as parameter.  Currently
 * is just drops the item to the floor under the influencer and will
 * reduce influencers money.
 * ---------------------------------------------------------------------- */
void 
TryToRepairItem( item* RepairItem )
{
  int MenuPosition;
  char linebuf[1000];

#define ANSWER_YES 1
#define ANSWER_NO 2

  char* MenuTexts[ 10 ];
  MenuTexts[0]="Yes";
  MenuTexts[1]="No";
  MenuTexts[2]="";

  while ( SpacePressed() || EnterPressed() );

  if ( REPAIR_PRICE_FACTOR * CalculateItemPrice ( RepairItem , TRUE ) > Me[0].Gold )
    {
      PlayOnceNeededSoundSample ( "STO_You_Cant_Repaired_0.wav" , FALSE );
      MenuTexts[0]=" BACK ";
      MenuTexts[1]="";
      DoMenuSelection ( "\n\nYou can't afford to have this item repaired! " , MenuTexts , 1 , NULL , NULL );
      return;
    }

  while ( 1 )
    {
      GiveItemDescription( linebuf , RepairItem , TRUE );
      strcat ( linebuf , "\n\n    Are you sure you want this item repaired?" );
      MenuPosition = DoMenuSelection( linebuf , MenuTexts , 1 , NULL , NULL );
      switch (MenuPosition) 
	{
	case (-1):
	  return;
	  break;
	case ANSWER_YES:
	  while (EnterPressed() || SpacePressed() );
	  Me[0].Gold -= REPAIR_PRICE_FACTOR * CalculateItemPrice ( RepairItem , TRUE ) ;
	  RepairItem->current_duration = RepairItem->max_duration;
	  PlayOnceNeededSoundSample ( "../effects/Shop_ItemRepairedSound_0.wav" , FALSE );
	  return;
	  break;
	case ANSWER_NO:
	  while (EnterPressed() || SpacePressed() );
	  return;
	  break;
	}
    }
}; // void TryToRepairItem( item* RepairItem )

/* ----------------------------------------------------------------------
 * This function tries to identify the item given as parameter.  
 * ---------------------------------------------------------------------- */
void 
TryToIdentifyItem( item* IdentifyItem )
{
  int MenuPosition;
  char linebuf[1000];

#define ANSWER_YES 1
#define ANSWER_NO 2

  char* MenuTexts[ 10 ];
  MenuTexts[0]="Yes";
  MenuTexts[1]="No";
  MenuTexts[2]="";

  while ( SpacePressed() || EnterPressed() );

  if ( 100 > Me[0].Gold )
    {
      PlayOnceNeededSoundSample ( "STO_You_Cant_Identified_0.wav" , FALSE );
      MenuTexts[0]=" BACK ";
      MenuTexts[1]="";
      DoMenuSelection ( "You can't afford to have this item identified! " , MenuTexts , 1 , NULL , NULL );
      return;
    }

  while ( 1 )
    {
      GiveItemDescription( linebuf , IdentifyItem , TRUE );
      strcat ( linebuf , "\n\n    Are you sure you want this item identified?" );
      MenuPosition = DoMenuSelection( linebuf , MenuTexts , 1 , NULL , NULL );
      // MenuPosition = DoMenuSelection( " Are you sure you want this item identified? " , MenuTexts , 1 , NULL , NULL );
      switch (MenuPosition) 
	{
	case (-1):
	  return;
	  break;
	case ANSWER_YES:
	  while (EnterPressed() || SpacePressed() );
	  Me[0].Gold -= 100 ;
	  IdentifyItem -> is_identified = TRUE ;
	  PlayOnceNeededSoundSample ( "../effects/Shop_ItemIdentifiedSound_0.wav" , FALSE );

	  MenuTexts[0]=" BACK ";
	  MenuTexts[1]="";
	  GiveItemDescription( linebuf , IdentifyItem , TRUE );
	  strcat ( linebuf , "\n\n " );
	  MenuPosition = DoMenuSelection( linebuf , MenuTexts , 1 , NULL , NULL );

	  return;
	  break;
	case ANSWER_NO:
	  while (EnterPressed() || SpacePressed() );
	  return;
	  break;
	}
    }
}; // void TryToIdentifyItem( item* IdentifyItem )

/* ----------------------------------------------------------------------
 * This function tries to buy the item given as parameter.  Currently
 * is just drops the item to the floor under the influencer and will
 * reduce influencers money.
 * ---------------------------------------------------------------------- */
void 
TryToSellItem( item* SellItem , int WithBacktalk , int AmountToSellAtMost )
{
  int MenuPosition;
  char linebuf[1000];

#define ANSWER_YES 1
#define ANSWER_NO 2

  char* MenuTexts[ 10 ];
  MenuTexts[0]="Yes";
  MenuTexts[1]="No";
  MenuTexts[2]="";

  if ( AmountToSellAtMost > SellItem -> multiplicity )
    AmountToSellAtMost = SellItem -> multiplicity ;

  while ( SpacePressed() || EnterPressed() );

  if ( WithBacktalk )
    {
      while ( 1 )
	{
	  GiveItemDescription( linebuf , SellItem , TRUE );
	  strcat ( linebuf , "\n\n    Are you sure you wish to sell this/(some of these) item(s)?" );
	  MenuPosition = DoMenuSelection( linebuf , MenuTexts , 1 , NULL , NULL );
	  switch (MenuPosition) 
	    {
	    case (-1):
	      return;
	      break;
	    case ANSWER_YES:
	      while (EnterPressed() || SpacePressed() );

	      Me[0].Gold += SELL_PRICE_FACTOR * CalculateItemPrice ( SellItem , FALSE );
	      DeleteItem( SellItem );
	      PlayOnceNeededSoundSample ( "../effects/Shop_ItemSoldSound_0.wav" , FALSE );

	      return;
	      break;
	    case ANSWER_NO:
	      while (EnterPressed() || SpacePressed() );
	      return;
	      break;
	    }
	}
    }
  else
    {
      Me[0].Gold += SELL_PRICE_FACTOR * CalculateItemPrice ( SellItem , FALSE ) * (float)AmountToSellAtMost / (float)SellItem->multiplicity ;
      if ( AmountToSellAtMost < SellItem->multiplicity )
	SellItem->multiplicity -= AmountToSellAtMost;
      else DeleteItem( SellItem );
      PlayOnceNeededSoundSample ( "../effects/Shop_ItemSoldSound_0.wav" , FALSE );
    }
}; // void TryToSellItem( item* SellItem )

/* ----------------------------------------------------------------------
 * This function tries to put an item into the inventory, either by adding
 * this items multiplicity to the multiplicity of an already present item
 * of the very same type or by allocating a new inventory item for this
 * new item and putting it there.
 *
 * In the case that both methods couldn't succeed, a FALSE value is 
 * returned to let the caller know, that this procedure has failed.
 * Otherwise TRUE will indicate that everything is ok and went well.
 * ---------------------------------------------------------------------- */
int
TryToIntegrateItemIntoInventory ( item* BuyItem , int AmountToBuyAtMost )
{
  int x, y;
  int FreeIndex;
  char linebuf[1000];
  int i;
  char* MenuTexts[ 10 ];

  //--------------------
  // At first we try to see if we can just add the multiplicity of the item in question
  // to the existing multiplicity of an item of the same type
  //
  if ( ItemMap [ BuyItem->type ] . item_group_together_in_inventory )
    {
      for ( i = 0 ; i < MAX_ITEMS_IN_INVENTORY ; i ++ )
	{
	  if ( Me [ 0 ] . Inventory [ i ] . type == BuyItem->type )
	    {
	      while ( 1 )
		{
		  while (EnterPressed() || SpacePressed() );
		  Me [ 0 ] . Inventory [ i ] . multiplicity += AmountToBuyAtMost ;

		  //--------------------
		  // This is new.  I hope it's not dangerous.
		  //
		  if ( AmountToBuyAtMost >= BuyItem->multiplicity )
		    DeleteItem ( BuyItem );
		  else
		    BuyItem->multiplicity -= AmountToBuyAtMost ;
		  return ( TRUE );
		}
	    }
	}
    }

  //--------------------
  // Now we must find out if there is an inventory position where we can put the
  // item in question.
  //
  FreeIndex = GetFreeInventoryIndex(  );

  for ( x = 0 ; x < INVENTORY_GRID_WIDTH ; x ++ )
    {
      for ( y = 0 ; y < INVENTORY_GRID_HEIGHT ; y ++ )
	{
	  if ( ItemCanBeDroppedInInv ( BuyItem->type , x , y ) )
	    {
	      while ( 1 )
		{
		  while (EnterPressed() || SpacePressed() );

		  CopyItem( BuyItem , & ( Me[0].Inventory[ FreeIndex ] ) , FALSE );
		  Me[0].Inventory[ FreeIndex ] . multiplicity = AmountToBuyAtMost ;

		  Me[0].Inventory[ FreeIndex ].currently_held_in_hand = FALSE;
		  Me[0].Inventory[ FreeIndex ].inventory_position.x = x;
		  Me[0].Inventory[ FreeIndex ].inventory_position.y = y;

		  //--------------------
		  // This is new.  I hope it's not dangerous.
		  //
		  if ( BuyItem -> multiplicity <= AmountToBuyAtMost ) DeleteItem ( BuyItem );
		  else BuyItem -> multiplicity -= AmountToBuyAtMost ;

		  return ( TRUE );
		}
	    }
	}
    }

  //--------------------
  // If this point is ever reached, we know that an item has been selected 
  // for buying and could be bought, if only ONE HAD ENOUGH ROOM IN INVENTORY!!
  // Therefore a message must be displayed, saying what the problem is.
  //
  PlayOnceNeededSoundSample ( "Tux_Hold_On_I_0.wav" , FALSE );
  MenuTexts[0]=" BACK ";
  MenuTexts[1]="";
  GiveItemDescription( linebuf , BuyItem , TRUE );
  strcat ( linebuf , "\n\n   No room for this item in inventory!" );
  DoMenuSelection( linebuf , MenuTexts , 1 , NULL , NULL );
  return ( FALSE );

}; // void TryToIntegrateItemIntoInventory ( item* BuyItem , int AmountToBuyAtMost )

/* ----------------------------------------------------------------------
 * This function tries to buy the item given as parameter.  Currently
 * is just drops the item to the floor under the influencer and will
 * reduce influencers money.
 * ---------------------------------------------------------------------- */
void 
TryToTakeItem( item* BuyItem , int AmountToBuyAtMost )
{
  int StoredItemType;

  StoredItemType = BuyItem -> type ;

  //--------------------
  // We prevent some take-put-cheating here.  For buying items this must
  // NOT be done.
  //
  if ( AmountToBuyAtMost >= BuyItem -> multiplicity ) AmountToBuyAtMost = BuyItem -> multiplicity ;

  if ( TryToIntegrateItemIntoInventory ( BuyItem , AmountToBuyAtMost ) )
    {
      PlayItemSound( ItemMap[ StoredItemType ].sound_number );
    }
}; // void TryToTakeItem( item* BuyItem , int AmountToBuyAtMost )

/* ----------------------------------------------------------------------
 * This function tries to buy the item given as parameter.  Currently
 * is just drops the item to the floor under the influencer and will
 * reduce influencers money.
 * ---------------------------------------------------------------------- */
void 
TryToBuyItem( item* BuyItem , int WithBacktalk , int AmountToBuyAtMost )
{
  int FreeIndex;
  char linebuf[1000];
  float PotentialPrice;

#define ANSWER_YES 1
#define ANSWER_NO 2

  char* MenuTexts[ 10 ];
  MenuTexts[0]="Yes";
  MenuTexts[1]="No";
  MenuTexts[2]="";

  DebugPrintf ( 0 , "\nTryToBuyItem (...):  function called." );

  BuyItem -> multiplicity = AmountToBuyAtMost ;

  FreeIndex = GetFreeInventoryIndex(  );

  while ( SpacePressed() || EnterPressed() || axis_is_active );

  if ( CalculateItemPrice ( BuyItem , FALSE ) > Me[0].Gold )
    {
      PlayOnceNeededSoundSample ( "STO_You_Cant_Buy_0.wav" , FALSE );
      if ( WithBacktalk )
	{
	  MenuTexts[0]=" BACK ";
	  MenuTexts[1]="";
	  GiveItemDescription( linebuf , BuyItem , TRUE );
	  strcat ( linebuf , "\n\n    You can't afford to purchase this item!" );
	  DoMenuSelection( linebuf , MenuTexts , 1 , NULL , NULL );
	}
      return;
    }

  //--------------------
  // In the case that the item could be afforded in theory, we need to
  // calculate the price, then have the item integrated into the inventory
  // if that's possible, and if so, subtract the items price from the
  // current gold.
  //
  PotentialPrice = CalculateItemPrice ( BuyItem , FALSE ) ;

  if ( TryToIntegrateItemIntoInventory ( BuyItem , AmountToBuyAtMost ) )
    {
      Me[0].Gold -= PotentialPrice ;
      PlayOnceNeededSoundSample ( "../effects/Shop_ItemBoughtSound_0.wav" , FALSE );
    }
  else
    {
      // bad luck.  couldn't store item in inventory, so no price paid...
    }

}; // void TryToBuyItem( item* BuyItem )

/* ----------------------------------------------------------------------
 * This is the menu, where you can buy basic items.
 * ---------------------------------------------------------------------- */
void
InitTradeWithCharacter( int CharacterCode )
{

#define FIXED_SHOP_INVENTORY TRUE
#define NUMBER_OF_ITEMS_IN_SHOP 17
  // #define NUMBER_OF_ITEMS_IN_SHOP 4

  item SalesList[ MAX_ITEMS_IN_INVENTORY ];
  item* BuyPointerList[ MAX_ITEMS_IN_INVENTORY ];
  item* TuxItemsList[ MAX_ITEMS_IN_INVENTORY ];
  int i;
  int ItemSelected=0;
  shop_decision ShopOrder;
  int NumberOfItemsInTuxRow=0;
  int NumberOfItemsInShop=0;

  AssembleItemListForTradeCharacter ( & (SalesList[0]) , CharacterCode );
  for ( i = 0 ; i < MAX_ITEMS_IN_INVENTORY ; i ++ )
    {
      if ( SalesList [ i ] . type == ( - 1 ) ) BuyPointerList [ i ] = NULL ;
      else BuyPointerList [ i ] = & ( SalesList[ i ] ) ;
    }
  
  //--------------------
  // Now here comes the new thing:  This will be a loop from now
  // on.  The buy and buy and buy until at one point we say 'BACK'
  //
  while ( ItemSelected != (-1) )
    {

      NumberOfItemsInTuxRow = AssemblePointerListForItemShow ( &( TuxItemsList[0]), FALSE, 0 );
      
      for ( i = 0 ; i < MAX_ITEMS_IN_INVENTORY ; i ++ )
	{
	  if ( BuyPointerList [ i ] == NULL )
	    {
	      NumberOfItemsInShop = i ;
	      break;
	    }
	}

      ItemSelected = GreatShopInterface ( NumberOfItemsInShop , BuyPointerList , 
					  NumberOfItemsInTuxRow , TuxItemsList , &(ShopOrder) , FALSE );

      switch ( ShopOrder . shop_command )
	{
	case BUY_1_ITEM:
	  TryToBuyItem( BuyPointerList[ ShopOrder . item_selected ] , FALSE , 1 ) ;
	  break;
	case BUY_10_ITEMS:
	  TryToBuyItem( BuyPointerList[ ShopOrder . item_selected ] , FALSE , 10 ) ;
	  break;
	case BUY_100_ITEMS:
	  TryToBuyItem( BuyPointerList[ ShopOrder . item_selected ] , FALSE , 100 ) ;
	  break;
	case SELL_1_ITEM:
	  TryToSellItem( TuxItemsList[ ShopOrder . item_selected ] , FALSE , 1 ) ;
	  break;
	case SELL_10_ITEMS:
	  TryToSellItem( TuxItemsList[ ShopOrder . item_selected ] , FALSE , 10 ) ;
	  break;
	case SELL_100_ITEMS:
	  TryToSellItem( TuxItemsList[ ShopOrder . item_selected ] , FALSE , 100 ) ;
	  break;
	default:
	  
	  break;
	};

      //--------------------
      // And since it can be assumed that the shop never runs
      // out of supply for a certain good, we can as well restore
      // the shop inventory list at this position.
      //
      /*
      if ( FIXED_SHOP_INVENTORY )
	{
	  for ( i = 0 ; i < NUMBER_OF_ITEMS_IN_SHOP ; i++ )
	    {
	      SalesList[ i ].type = StandardShopInventory [ i ];
	      SalesList[ i ].prefix_code = ( -1 );
	      SalesList[ i ].suffix_code = ( -1 );
	      FillInItemProperties( & ( SalesList[ i ] ) , TRUE , 0 );
	      Buy_Pointer_List [ i ] = & ( SalesList[ i ] ) ;
	    }
	  Buy_Pointer_List [ i ] = NULL ; 
	}
      */
      AssembleItemListForTradeCharacter ( & (SalesList[0]) , CharacterCode );
      for ( i = 0 ; i < MAX_ITEMS_IN_INVENTORY ; i ++ )
	{
	  if ( SalesList [ i ] . type == ( - 1 ) ) BuyPointerList [ i ] = NULL ;
	  else BuyPointerList [ i ] = & ( SalesList[ i ] ) ;
	}

    }

}; // void InitTradeWithCharacter( void )

/* ----------------------------------------------------------------------
 * This is the menu, where you can select items for repair.
 * ---------------------------------------------------------------------- */
void
Repair_Items( void )
{
#define BASIC_ITEMS_NUMBER 10
  item* Repair_Pointer_List[ MAX_ITEMS_IN_INVENTORY + 10 ];  // the inventory plus 7 slots or so
  int Pointer_Index=0;
  int i;
  // int InMenuPosition = 0;
  // int MenuInListPosition = 0;
  int ItemSelected;
  char DescriptionText[5000];
  char* MenuTexts[ 10 ];
  MenuTexts[0]="Yes";
  MenuTexts[1]="No";
  MenuTexts[2]="";

  //--------------------
  // First we clean out the new Repair_Pointer_List
  //
  for ( i = 0 ; i < MAX_ITEMS_IN_INVENTORY ; i ++ )
    {
      Repair_Pointer_List[ i ] = NULL;
    }

  //--------------------
  // Now we start to fill the Repair_Pointer_List
  //
  if ( ( Me[0].weapon_item.current_duration < Me[0].weapon_item.max_duration ) && 
       ( Me[0].weapon_item.type != ( -1 ) ) )
    {
      Repair_Pointer_List [ Pointer_Index ] = & ( Me[0].weapon_item );
      Pointer_Index ++;
    }
  if ( ( Me[0].drive_item.current_duration < Me[0].drive_item.max_duration ) &&
       ( Me[0].drive_item.type != ( -1 ) ) )
    {
      Repair_Pointer_List [ Pointer_Index ] = & ( Me[0].drive_item );
      Pointer_Index ++;
    }
  if ( ( Me[0].armour_item.current_duration < Me[0].armour_item.max_duration ) &&
       ( Me[0].armour_item.type != ( -1 ) ) )
    {
      Repair_Pointer_List [ Pointer_Index ] = & ( Me[0].armour_item );
      Pointer_Index ++;
    }
  if ( ( Me[0].shield_item.current_duration < Me[0].shield_item.max_duration ) &&
       ( Me[0].shield_item.type != ( -1 ) ) )
    {
      Repair_Pointer_List [ Pointer_Index ] = & ( Me[0].shield_item );
      Pointer_Index ++;
    }

  for ( i = 0 ; i < MAX_ITEMS_IN_INVENTORY ; i ++ )
    {
      if ( Me[0].Inventory [ i ].type == (-1) ) continue;
      if ( Me[0].Inventory [ i ].max_duration == (-1) ) continue;
      if ( Me[0].Inventory [ i ].current_duration < Me[0].Inventory [ i ] .max_duration ) 
	{
	  Repair_Pointer_List [ Pointer_Index ] = & ( Me[0].Inventory[ i ] );
	  Pointer_Index ++;
	}
    }

  if ( Pointer_Index == 0 )
    {
      PlayOnceNeededSoundSample ( "STO_Sorry_But_Repair_0.wav" , FALSE );
      MenuTexts[0]=" BACK ";
      MenuTexts[1]="";
      DoMenuSelection ( " YOU DONT HAVE ANYTHING THAT WOULD NEED REPAIR " , MenuTexts , 1 , NULL , NULL );
      return;
    }

  //--------------------
  // Now here comes the new thing:  This will be a loop from now
  // on.  The buy and buy and buy until at one point we say 'BACK'
  //
  ItemSelected = 0;

  while ( ItemSelected != (-1) )
    {
      sprintf( DescriptionText , " I COULD REPAIR THESE ITEMS           YOUR GOLD:  %4ld" , Me[0].Gold );
      ItemSelected = DoEquippmentListSelection( DescriptionText , Repair_Pointer_List , PRICING_FOR_REPAIR );
      if ( ItemSelected != (-1) ) TryToRepairItem( Repair_Pointer_List[ ItemSelected ] ) ;
    }

}; // void Repair_Items( void )

/* ----------------------------------------------------------------------
 * This is the menu, where you can buy basic items.
 * ---------------------------------------------------------------------- */
void
Identify_Items ( void )
{
#define BASIC_ITEMS_NUMBER 10
  item* Identify_Pointer_List[ MAX_ITEMS_IN_INVENTORY + 10 ];  // the inventory plus 7 slots or so
  int Pointer_Index=0;
  int i;
  // int InMenuPosition = 0;
  // int MenuInListPosition = 0;
  char DescriptionText[5000];
  int ItemSelected;
  char* MenuTexts[ 10 ];
  MenuTexts[0]="Yes";
  MenuTexts[1]="No";
  MenuTexts[2]="";

  //--------------------
  // First we clean out the new Identify_Pointer_List
  //
  for ( i = 0 ; i < MAX_ITEMS_IN_INVENTORY ; i ++ )
    {
      Identify_Pointer_List[ i ] = NULL;
    }

  //--------------------
  // Now we start to fill the Identify_Pointer_List
  //
  if ( ( !Me[0].weapon_item.is_identified ) && 
       ( Me[0].weapon_item.type != ( -1 ) ) )
    {
      Identify_Pointer_List [ Pointer_Index ] = & ( Me[0].weapon_item );
      Pointer_Index ++;
    }
  if ( ( !Me[0].drive_item.is_identified ) &&
       ( Me[0].drive_item.type != ( -1 ) ) )
    {
      Identify_Pointer_List [ Pointer_Index ] = & ( Me[0].drive_item );
      Pointer_Index ++;
    }
  if ( ( !Me[0].armour_item.is_identified ) &&
       ( Me[0].armour_item.type != ( -1 ) ) )
    {
      Identify_Pointer_List [ Pointer_Index ] = & ( Me[0].armour_item );
      Pointer_Index ++;
    }
  if ( ( !Me[0].shield_item.is_identified ) &&
       ( Me[0].shield_item.type != ( -1 ) ) )
    {
      Identify_Pointer_List [ Pointer_Index ] = & ( Me[0].shield_item );
      Pointer_Index ++;
    }

  for ( i = 0 ; i < MAX_ITEMS_IN_INVENTORY ; i ++ )
    {
      if ( Me[0].Inventory [ i ].type == (-1) ) continue;
      if ( Me[0].Inventory [ i ].is_identified ) continue;

      Identify_Pointer_List [ Pointer_Index ] = & ( Me[0].Inventory[ i ] );
      Pointer_Index ++;
    }

  if ( Pointer_Index == 0 )
    {
      PlayOnceNeededSoundSample ( "STO_You_Dont_Have_0.wav" , FALSE );
      MenuTexts[0]=" BACK ";
      MenuTexts[1]="";
      DoMenuSelection ( " YOU DONT HAVE ANYTHING THAT WOULD NEED TO BE IDENTIFIED!" , MenuTexts , 1 , NULL , NULL );
      return;
    }

  //--------------------
  // Now here comes the new thing:  This will be a loop from now
  // on.  The buy and buy and buy until at one point we say 'BACK'
  //
  ItemSelected = 0;

  while ( ItemSelected != (-1) )
    {
      sprintf( DescriptionText , " I COULD IDENTIFY THESE ITEMS             YOUR GOLD:  %4ld" , Me[0].Gold );
      ItemSelected = DoEquippmentListSelection( DescriptionText , Identify_Pointer_List , PRICING_FOR_IDENTIFY );
      if ( ItemSelected != (-1) ) TryToIdentifyItem( Identify_Pointer_List[ ItemSelected ] ) ;
    }

}; // void Identify_Items( void )


/* ----------------------------------------------------------------------
 * This is the menu, where you can sell inventory items.
 * ---------------------------------------------------------------------- */
void
Sell_Items( int ForHealer )
{
#define BASIC_ITEMS_NUMBER 10
  item* Sell_Pointer_List[ MAX_ITEMS_IN_INVENTORY ];
  int Pointer_Index=0;
  int i;
  int ItemSelected;
  //  int InMenuPosition = 0;
  //  int MenuInListPosition = 0;
  char DescriptionText[5000];
  char* MenuTexts[ 10 ];
  MenuTexts[0]="Yes";
  MenuTexts[1]="No";
  MenuTexts[2]="";

  //--------------------
  // First we clean out the new Sell_Pointer_List
  //
  for ( i = 0 ; i < MAX_ITEMS_IN_INVENTORY ; i ++ )
    {
      Sell_Pointer_List[ i ] = NULL;
    }

  //--------------------
  // Now we start to fill the Sell_Pointer_List
  //
  for ( i = 0 ; i < MAX_ITEMS_IN_INVENTORY ; i ++ )
    {
      if ( Me[0].Inventory [ i ].type == (-1) ) continue;
      else
	{
	  //--------------------
	  // Now depending on whether we sell to the healer or to
	  // the weaponsmith, we can either sell one thing or the
	  // other
	  //
	  if ( ( ForHealer ) &&  ! ItemMap [ Me[0].Inventory[ i ].type ].item_can_be_applied_in_combat ) continue;
	  if ( ! ( ForHealer ) &&  ItemMap [ Me[0].Inventory[ i ].type ].item_can_be_applied_in_combat ) continue;
	  Sell_Pointer_List [ Pointer_Index ] = & ( Me[0].Inventory[ i ] );
	  Pointer_Index ++;
	}
    }

  if ( Pointer_Index == 0 )
    {
      PlayOnceNeededSoundSample ( "STO_Sorry_But_You_0.wav" , FALSE );
      MenuTexts[0]=" BACK ";
      MenuTexts[1]="";
      DoMenuSelection ( " YOU DONT HAVE ANYTHING IN INVENTORY (I.E. NOT WORN), THAT COULD BE SOLD. " , 
			MenuTexts, 1 , NULL , NULL );
      return;
    }

  //--------------------
  // Now here comes the new thing:  This will be a loop from now
  // on.  The buy and buy and buy until at one point we say 'BACK'
  //
  ItemSelected = 0;

  while ( ItemSelected != (-1) )
    {
      sprintf( DescriptionText , " I WOULD BUY FROM YOU THESE ITEMS        YOUR GOLD:  %4ld" , Me[0].Gold );
      ItemSelected = DoEquippmentListSelection( DescriptionText , Sell_Pointer_List , PRICING_FOR_SELL );
      if ( ItemSelected != (-1) ) TryToSellItem( Sell_Pointer_List[ ItemSelected ] , TRUE , 1 ) ;
    }

}; // void Sell_Items( int ForHealer )

/* ----------------------------------------------------------------------
 * This function does all the buying/selling interaction with the 
 * weaponsmith Mr. Stone.
 * ---------------------------------------------------------------------- */
void
BuySellMenu ( void )
{
enum
  { 
    BUY_BASIC_ITEMS=1, 
    BUY_PREMIUM_ITEMS, 
    SELL_ITEMS, 
    REPAIR_ITEMS,
    IDENTIFY_ITEMS,
    LEAVE_BUYSELLMENU
  };

  int Weiter = 0;
  int MenuPosition=1;
  char* MenuTexts[10];

  Me[0].status=MENU;

  DebugPrintf (2, "\nvoid BuySellMenu(void): real function call confirmed."); 

  // Prevent distortion of framerate by the delay coming from 
  // the time spend in the menu.
  Activate_Conservative_Frame_Computation();
  while ( EscapePressed() );

  while (!Weiter)
    {
      MenuTexts[0]="Buy Basic Items";
      MenuTexts[1]="Buy Premium Items";
      MenuTexts[2]="Sell Items";
      MenuTexts[3]="Repair Items";
      MenuTexts[5]="Leave the Sales Representative";
      MenuTexts[4]="Identify Items";
      MenuTexts[8]="";

      // MenuPosition = DoMenuSelection( "" , MenuTexts , -1 , SHOP_BACKGROUND_IMAGE , NULL );
      MenuPosition = DoMenuSelection( "" , MenuTexts , -1 , SHOP_BACKGROUND_IMAGE , Menu_Filled_BFont );

      switch (MenuPosition) 
	{
	case (-1):
	  Weiter=!Weiter;
	  break;
	case BUY_BASIC_ITEMS:
	  while (EnterPressed() || SpacePressed() );
	  InitTradeWithCharacter( FALSE );
	  break;
	case BUY_PREMIUM_ITEMS:
	  while (EnterPressed() || SpacePressed() );
	  InitTradeWithCharacter( FALSE );
	  break;
	case SELL_ITEMS:
	  while (EnterPressed() || SpacePressed() );
	  Sell_Items( FALSE );
	  break;
	case REPAIR_ITEMS:
	  while (EnterPressed() || SpacePressed() );
	  Repair_Items();
	  break;
	case IDENTIFY_ITEMS:
	  while (EnterPressed() || SpacePressed() );
	  Identify_Items();
	  break;
	case LEAVE_BUYSELLMENU:
	  Weiter = !Weiter;
	  break;
	default: 
	  break;
	} 
    }

  ClearGraphMem();
  // Since we've faded out the whole scren, it can't hurt
  // to have the top status bar redrawn...
  BannerIsDestroyed=TRUE;
  Me[0].status=MOBILE;

  return;
}; // void BuySellMenu ( void )

/* ----------------------------------------------------------------------
 * When the Tux opens a chest map tile, then there should be an interface
 * where the Tux can put in stuff and take out stuff from the chest, 
 * which is exactly what this function is supposed to do.
 * ---------------------------------------------------------------------- */
void
EnterChest (void)
{
  int ItemSelected = 0 ;
  int NumberOfItemsInTuxRow = 0 ;
  int NumberOfItemsInChest = 0 ;
  item* Buy_Pointer_List[ MAX_ITEMS_IN_INVENTORY ];
  item* TuxItemsList[ MAX_ITEMS_IN_INVENTORY ];
  shop_decision ShopOrder;

  Activate_Conservative_Frame_Computation();

  while ( ItemSelected != (-1) )
    {
      
      NumberOfItemsInTuxRow = AssemblePointerListForItemShow ( &( TuxItemsList[0]), FALSE , 0 );
      NumberOfItemsInChest = AssemblePointerListForChestShow ( &( Buy_Pointer_List[0]), 0 );

      ItemSelected = GreatShopInterface ( NumberOfItemsInChest , Buy_Pointer_List , 
					  NumberOfItemsInTuxRow , TuxItemsList , &(ShopOrder) , TRUE );

      if ( ItemSelected == (-1) ) ShopOrder . shop_command = DO_NOTHING ;

      switch ( ShopOrder . shop_command )
	{
	case BUY_1_ITEM:
	  TryToTakeItem( Buy_Pointer_List[ ShopOrder . item_selected ] , 1 ) ;
	  break;
	case BUY_10_ITEMS:
	  TryToTakeItem( Buy_Pointer_List[ ShopOrder . item_selected ] , 10 ) ;
	  break;
	case BUY_100_ITEMS:
	  TryToTakeItem( Buy_Pointer_List[ ShopOrder . item_selected ] , 100 ) ;
	  break;
	case SELL_1_ITEM:
	  TryToPutItem( TuxItemsList[ ShopOrder . item_selected ] , 1 ) ;
	  break;
	case SELL_10_ITEMS:
	  TryToPutItem( TuxItemsList[ ShopOrder . item_selected ] , 10 ) ;
	  break;
	case SELL_100_ITEMS:
	  TryToPutItem( TuxItemsList[ ShopOrder . item_selected ] , 100 ) ;
	  break;
	default:
	  
	  break;
	};
    }

}; // void EnterChest (void)


#undef _shop_c

