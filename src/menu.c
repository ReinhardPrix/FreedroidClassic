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

#define _menu_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"

#define SELL_PRICE_FACTOR (0.25)
#define REPAIR_PRICE_FACTOR (0.5)

int New_Game_Requested=FALSE;

int Single_Player_Menu (void);
int Multi_Player_Menu (void);
void Credits_Menu (void);
void Options_Menu (void);
void Show_Mission_Log_Menu (void);
EXTERN void Level_Editor(void);

EXTERN char Previous_Mission_Name[1000];

#define FIRST_MENU_ITEM_POS_X (1*Block_Width)
#define FIRST_MENU_ITEM_POS_XX ( SCREENLEN - FIRST_MENU_ITEM_POS_X )
#define FIRST_MENU_ITEM_POS_Y (BANNER_HEIGHT + FontHeight(Menu_BFont) * 3 )

/* ----------------------------------------------------------------------
 * This function tries to buy the item given as parameter.  Currently
 * is just drops the item to the floor under the influencer and will
 * reduce influencers money.
 * ---------------------------------------------------------------------- */
void 
TryToRepairItem( item* RepairItem )
{
  int MenuPosition;

#define ANSWER_YES 1
#define ANSWER_NO 2

  char* MenuTexts[ 10 ];
  MenuTexts[0]="Yes";
  MenuTexts[1]="No";
  MenuTexts[2]="";
  MenuTexts[3]="";
  MenuTexts[4]="";
  MenuTexts[5]="";
  MenuTexts[8]="";
  MenuTexts[6]="";
  MenuTexts[7]="";
  MenuTexts[9]="";

  while ( SpacePressed() || EnterPressed() );

  if ( REPAIR_PRICE_FACTOR * CalculateItemPrice ( RepairItem , TRUE ) > Me[0].Gold )
    {
      MenuTexts[0]=" BACK ";
      MenuTexts[1]="";
      DoMenuSelection ( "YOU CAN't AFFORD TO HAVE THIS ITEM REPAIRED! " , MenuTexts , 1 , NULL );
      return;
    }

  while ( 1 )
    {
      MenuPosition = DoMenuSelection( " Are you sure you want this item repaired? " , MenuTexts , 1 , NULL );
      switch (MenuPosition) 
	{
	case (-1):
	  return;
	  break;
	case ANSWER_YES:
	  while (EnterPressed() || SpacePressed() );
	  Me[0].Gold -= REPAIR_PRICE_FACTOR * CalculateItemPrice ( RepairItem , TRUE ) ;
	  RepairItem->current_duration = RepairItem->max_duration;
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

#define ANSWER_YES 1
#define ANSWER_NO 2

  char* MenuTexts[ 10 ];
  MenuTexts[0]="Yes";
  MenuTexts[1]="No";
  MenuTexts[2]="";
  MenuTexts[3]="";
  MenuTexts[4]="";
  MenuTexts[5]="";
  MenuTexts[8]="";
  MenuTexts[6]="";
  MenuTexts[7]="";
  MenuTexts[9]="";

  while ( SpacePressed() || EnterPressed() );

  if ( 100 > Me[0].Gold )
    {
      MenuTexts[0]=" BACK ";
      MenuTexts[1]="";
      DoMenuSelection ( "YOU CAN't AFFORD TO HAVE THIS ITEM IDENTIFIED! " , MenuTexts , 1 , NULL );
      return;
    }

  while ( 1 )
    {
      MenuPosition = DoMenuSelection( " Are you sure you want this item identified? " , MenuTexts , 1 , NULL );
      switch (MenuPosition) 
	{
	case (-1):
	  return;
	  break;
	case ANSWER_YES:
	  while (EnterPressed() || SpacePressed() );
	  Me[0].Gold -= 100 ;
	  IdentifyItem -> is_identified = TRUE ;
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
TryToSellItem( item* SellItem )
{
  int MenuPosition;

#define ANSWER_YES 1
#define ANSWER_NO 2

  char* MenuTexts[ 10 ];
  MenuTexts[0]="Yes";
  MenuTexts[1]="No";
  MenuTexts[2]="";
  MenuTexts[3]="";
  MenuTexts[4]="";
  MenuTexts[5]="";
  MenuTexts[8]="";
  MenuTexts[6]="";
  MenuTexts[7]="";
  MenuTexts[9]="";

  while ( SpacePressed() || EnterPressed() );

  while ( 1 )
    {
      MenuPosition = DoMenuSelection( " Are you sure you want to sell this itemd? " , MenuTexts , 1 , NULL );
      switch (MenuPosition) 
	{
	case (-1):
	  return;
	  break;
	case ANSWER_YES:
	  while (EnterPressed() || SpacePressed() );
	  Me[0].Gold += SELL_PRICE_FACTOR * CalculateItemPrice ( SellItem , FALSE );
	  DeleteItem( SellItem );
	  return;
	  break;
	case ANSWER_NO:
	  while (EnterPressed() || SpacePressed() );
	  return;
	  break;
	}
    }
}; // void TryToSellItem( item* SellItem )

/* ----------------------------------------------------------------------
 * This function tries to buy the item given as parameter.  Currently
 * is just drops the item to the floor under the influencer and will
 * reduce influencers money.
 * ---------------------------------------------------------------------- */
void 
TryToBuyItem( item* BuyItem )
{
  int x, y;
  int MenuPosition;
  int FreeIndex;
  char linebuf[1000];

#define ANSWER_YES 1
#define ANSWER_NO 2

  char* MenuTexts[ 10 ];
  MenuTexts[0]="Yes";
  MenuTexts[1]="No";
  MenuTexts[2]="";
  MenuTexts[3]="";
  MenuTexts[4]="";
  MenuTexts[5]="";
  MenuTexts[8]="";
  MenuTexts[6]="";
  MenuTexts[7]="";
  MenuTexts[9]="";

  FreeIndex = GetFreeInventoryIndex(  );

  while ( SpacePressed() || EnterPressed() );

  if ( CalculateItemPrice ( BuyItem , FALSE ) > Me[0].Gold )
    {
      MenuTexts[0]=" BACK ";
      MenuTexts[1]="";
      DoMenuSelection ( "YOU CAN'T AFFORD TO PURCHASE THIS ITEM! " , MenuTexts , 1 , NULL );
      return;
    }

  for ( x = 0 ; x < INVENTORY_GRID_WIDTH ; x ++ )
    {
      for ( y = 0 ; y < INVENTORY_GRID_HEIGHT ; y ++ )
	{
	  if ( ItemCanBeDroppedInInv ( BuyItem->type , x , y ) )
	    {
	      while ( 1 )
		{
		  GiveItemDescription( linebuf , BuyItem , TRUE );
		  strcat ( linebuf , "\n\n    Are you sure you wish to purchase this item?" );
		  MenuPosition = DoMenuSelection( linebuf , MenuTexts , 1 , NULL );
		  switch (MenuPosition) 
		    {
		    case (-1):
		      return;
		      break;
		    case ANSWER_YES:
		      while (EnterPressed() || SpacePressed() );
		      CopyItem( BuyItem , & ( Me[0].Inventory[ FreeIndex ] ) , TRUE );
		      Me[0].Inventory[ FreeIndex ].currently_held_in_hand = FALSE;
		      Me[0].Inventory[ FreeIndex ].inventory_position.x = x;
		      Me[0].Inventory[ FreeIndex ].inventory_position.y = y;
		      Me[0].Gold -= CalculateItemPrice ( BuyItem , FALSE );
		      return;
		      break;
		    case ANSWER_NO:
		      while (EnterPressed() || SpacePressed() );
		      return;
		      break;
		    }
		}
	    }
	}
    }
  

}; // void TryToBuyItem( item* BuyItem )


/* ----------------------------------------------------------------------
 * This is the menu, where you can buy basic items.
 * ---------------------------------------------------------------------- */
void
Buy_Basic_Items( int ForHealer , int ForceMagic )
{
#define NUMBER_OF_ITEMS_ON_ONE_SCREEN 4
#define ITEM_MENU_DISTANCE 80
  item SalesList[ 1000 ];
  int i;
  int InMenuPosition = 0;
  int MenuInListPosition = 0;
  char DescriptionText[5000];
  int basic_items_number = 10;

  //--------------------
  // First we make a selection of items, that can be considered 'basic'.
  // This selection depends of course on wheter the menu is generated
  // for the smith or for the healer.
  //
  for ( i = 0 ; i < basic_items_number ; i++ )
    {
      if ( ForHealer ) 
	{
	  SalesList[ i ].type = 0 ; // something that can NOT be applied in combat
	  while ( ! ItemMap [ SalesList[ i ].type ].item_can_be_applied_in_combat ) 
	    SalesList[ i ].type = MyRandom( Number_Of_Item_Types - 2 ) + 1;
	}
      else
	{
	  SalesList[ i ].type = 1 ; // something that can be applied in combat
	  while ( ItemMap [ SalesList[ i ].type ].item_can_be_applied_in_combat || 
		  SalesList [ i ].type == ITEM_MONEY ) 
	    SalesList[ i ].type = MyRandom( Number_Of_Item_Types - 2 ) + 1;
	}

      SalesList[ i ].prefix_code = ( -1 );
      if ( ForceMagic ) SalesList[ i ].suffix_code = ( MyRandom(10) );
      else SalesList[ i ].suffix_code = ( -1 );
      FillInItemProperties( & ( SalesList[ i ] ) , TRUE , 0 );
      SalesList[ i ].is_identified = TRUE;
    }

  while ( !SpacePressed() && !EscapePressed() )
    {
      // InitiateMenu ( NULL );
      InitiateMenu ( SHOP_BACKGROUND_IMAGE );

      //--------------------
      // Now we draw our selection of items to the screen, at least the part
      // of it, that's currently visible
      //
      DisplayText( " I HAVE THESE ITEMS FOR SALE         YOUR GOLD:" , 50 , 50 + (0) * ITEM_MENU_DISTANCE , NULL );
      sprintf( DescriptionText , "%4ld" , Me[0].Gold );
      DisplayText( DescriptionText , 580 , 50 + ( 0 ) * 80 , NULL );
      for ( i = 0 ; i < NUMBER_OF_ITEMS_ON_ONE_SCREEN ; i++ )
	{
	  // DisplayText( ItemMap [ SalesList[ i ].type ].ItemName , 50 , 50 + i * 50 , NULL );
	  // DisplayText( "\n" , -1 , -1, NULL );
	  GiveItemDescription( DescriptionText , & ( SalesList[ i + MenuInListPosition ] ) , TRUE );
	  DisplayText( DescriptionText , 50 , 50 + (i+1) * ITEM_MENU_DISTANCE , NULL );
	  sprintf( DescriptionText , "%4ld" , 
		   CalculateItemPrice ( & ( SalesList[ i + MenuInListPosition ] ) , FALSE ) );
	  DisplayText( DescriptionText , 580 , 50 + (i+1) * ITEM_MENU_DISTANCE , NULL );
	}
      
      //--------------------
      // Now we draw the influencer as a cursor
      //
      PutInfluence ( 10 , 50 + ( InMenuPosition + 1 ) * ITEM_MENU_DISTANCE , 0 );

      //--------------------
      //
      //
      SDL_Flip ( Screen );

      if ( UpPressed() )
	{
	  if ( InMenuPosition > 0 ) InMenuPosition --;
	  else 
	    {
	      if ( MenuInListPosition > 0 )
		MenuInListPosition --;
	    }
	  while ( UpPressed() );
	}
      if ( DownPressed() )
	{
	  if ( InMenuPosition < NUMBER_OF_ITEMS_ON_ONE_SCREEN - 1 ) InMenuPosition ++;
	  else 
	    {
	      if ( MenuInListPosition < basic_items_number - NUMBER_OF_ITEMS_ON_ONE_SCREEN )
		MenuInListPosition ++;
	    }
	  while ( DownPressed() );
	}      
    } // while not space pressed...

  if ( SpacePressed() ) TryToBuyItem( & ( SalesList[ InMenuPosition + MenuInListPosition ] ) ) ;

  while ( SpacePressed() || EscapePressed() );

}; // void Buy_Basic_Items( void )

/* ----------------------------------------------------------------------
 * This is the menu, where you can buy basic items.
 * ---------------------------------------------------------------------- */
void
Repair_Items( void )
{
#define BASIC_ITEMS_NUMBER 10
#define NUMBER_OF_ITEMS_ON_ONE_SCREEN 4
#define ITEM_MENU_DISTANCE 80
  item* Repair_Pointer_List[ MAX_ITEMS_IN_INVENTORY + 10 ];  // the inventory plus 7 slots or so
  int Pointer_Index=0;
  int i;
  int InMenuPosition = 0;
  int MenuInListPosition = 0;
  char DescriptionText[5000];
  char* MenuTexts[ 10 ];
  MenuTexts[0]="Yes";
  MenuTexts[1]="No";
  MenuTexts[2]="";
  MenuTexts[3]="";
  MenuTexts[4]="";
  MenuTexts[5]="";
  MenuTexts[8]="";
  MenuTexts[6]="";
  MenuTexts[7]="";
  MenuTexts[9]="";


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
      MenuTexts[0]=" BACK ";
      MenuTexts[1]="";
      DoMenuSelection ( " YOU DONT HAVE ANYTHING THAT WOULD NEED REPAIR " , MenuTexts , 1 , NULL );
      return;
    }


  while ( !SpacePressed() && !EscapePressed() )
    {
      InitiateMenu ( NULL );

      //--------------------
      // Now we draw our selection of items to the screen, at least the part
      // of it, that's currently visible
      //
      DisplayText( " I COULD REPAIR THESE ITEMS                YOUR GOLD:" , 50 , 50 + (0) * ITEM_MENU_DISTANCE , NULL );
      sprintf( DescriptionText , "%4ld" , Me[0].Gold );
      DisplayText( DescriptionText , 580 , 50 + ( 0 ) * 80 , NULL );
      for ( i = 0 ; ( (i < NUMBER_OF_ITEMS_ON_ONE_SCREEN) && (Repair_Pointer_List[ i + MenuInListPosition ] != NULL ) ) ; i++ )
	{
	  // DisplayText( ItemMap [ Repair_Pointer_List[ i + ]->type ].ItemName , 50 , 50 + i * 50 , NULL );
	  // DisplayText( "\n" , -1 , -1, NULL );
	  GiveItemDescription( DescriptionText , Repair_Pointer_List [ i + MenuInListPosition ] , TRUE );
	  DisplayText( DescriptionText , 50 , 50 + (i+1) * ITEM_MENU_DISTANCE , NULL );
	  sprintf( DescriptionText , "%6.0f" , 
		   REPAIR_PRICE_FACTOR * 
		   CalculateItemPrice ( Repair_Pointer_List[ i + MenuInListPosition] , TRUE ) );
	  DisplayText( DescriptionText , 580 , 50 + (i+1) * ITEM_MENU_DISTANCE , NULL );
	}
      
      //--------------------
      // Now we draw the influencer as a cursor
      //
      PutInfluence ( 10 , 50 + ( InMenuPosition + 1 ) * ITEM_MENU_DISTANCE , 0 );

      //--------------------
      //
      //
      SDL_Flip ( Screen );

      if ( UpPressed() )
	{
	  if ( InMenuPosition > 0 ) InMenuPosition --;
	  else 
	    {
	      if ( MenuInListPosition > 0 )
		MenuInListPosition --;
	    }
	  while ( UpPressed() );
	}
      if ( DownPressed() )
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

  if ( SpacePressed() ) TryToRepairItem( Repair_Pointer_List[ InMenuPosition + MenuInListPosition ] ) ;

  while ( SpacePressed() || EscapePressed() );

}; // void Repair_Items( void )

/* ----------------------------------------------------------------------
 * This is the menu, where you can buy basic items.
 * ---------------------------------------------------------------------- */
void
Identify_Items ( void )
{
#define BASIC_ITEMS_NUMBER 10
#define NUMBER_OF_ITEMS_ON_ONE_SCREEN 4
#define ITEM_MENU_DISTANCE 80
  item* Identify_Pointer_List[ MAX_ITEMS_IN_INVENTORY + 10 ];  // the inventory plus 7 slots or so
  int Pointer_Index=0;
  int i;
  int InMenuPosition = 0;
  int MenuInListPosition = 0;
  char DescriptionText[5000];
  char* MenuTexts[ 10 ];
  MenuTexts[0]="Yes";
  MenuTexts[1]="No";
  MenuTexts[2]="";
  MenuTexts[3]="";
  MenuTexts[4]="";
  MenuTexts[5]="";
  MenuTexts[8]="";
  MenuTexts[6]="";
  MenuTexts[7]="";
  MenuTexts[9]="";


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
      MenuTexts[0]=" BACK ";
      MenuTexts[1]="";
      DoMenuSelection ( " YOU DONT HAVE ANYTHING THAT WOULD NEED TO BE IDENTIFIED!" , MenuTexts , 1 , NULL );
      return;
    }


  while ( !SpacePressed() && !EscapePressed() )
    {
      InitiateMenu ( NULL );

      //--------------------
      // Now we draw our selection of items to the screen, at least the part
      // of it, that's currently visible
      //
      DisplayText( " I COULD IDENTIFY THESE ITEMS              YOUR GOLD:" , 50 , 50 + (0) * ITEM_MENU_DISTANCE , NULL );
      sprintf( DescriptionText , "%4ld" , Me[0].Gold );
      DisplayText( DescriptionText , 580 , 50 + ( 0 ) * 80 , NULL );
      for ( i = 0 ; ( (i < NUMBER_OF_ITEMS_ON_ONE_SCREEN) && (Identify_Pointer_List[ i + MenuInListPosition ] != NULL ) ) ; i++ )
	{
	  // DisplayText( ItemMap [ Identify_Pointer_List[ i + ]->type ].ItemName , 50 , 50 + i * 50 , NULL );
	  // DisplayText( "\n" , -1 , -1, NULL );
	  GiveItemDescription( DescriptionText , Identify_Pointer_List [ i + MenuInListPosition ] , TRUE );
	  DisplayText( DescriptionText , 50 , 50 + (i+1) * ITEM_MENU_DISTANCE , NULL );
	  sprintf( DescriptionText , "%6.0f" , 100.0 );
	  DisplayText( DescriptionText , 580 , 50 + (i+1) * ITEM_MENU_DISTANCE , NULL );
	}
      
      //--------------------
      // Now we draw the influencer as a cursor
      //
      PutInfluence ( 10 , 50 + ( InMenuPosition + 1 ) * ITEM_MENU_DISTANCE , 0 );

      //--------------------
      //
      //
      SDL_Flip ( Screen );

      if ( UpPressed() )
	{
	  if ( InMenuPosition > 0 ) InMenuPosition --;
	  else 
	    {
	      if ( MenuInListPosition > 0 )
		MenuInListPosition --;
	    }
	  while ( UpPressed() );
	}
      if ( DownPressed() )
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

  if ( SpacePressed() ) TryToIdentifyItem( Identify_Pointer_List[ InMenuPosition + MenuInListPosition ] ) ;

  while ( SpacePressed() || EscapePressed() );

}; // void Identify_Items( void )

/* ----------------------------------------------------------------------
 * This is the menu, where you can sell inventory items.
 * ---------------------------------------------------------------------- */
void
Sell_Items( int ForHealer )
{
#define BASIC_ITEMS_NUMBER 10
#define NUMBER_OF_ITEMS_ON_ONE_SCREEN 4
#define ITEM_MENU_DISTANCE 80
  item* Sell_Pointer_List[ MAX_ITEMS_IN_INVENTORY ];
  int Pointer_Index=0;
  int i;
  int InMenuPosition = 0;
  int MenuInListPosition = 0;
  char DescriptionText[5000];
  char* MenuTexts[ 10 ];
  MenuTexts[0]="Yes";
  MenuTexts[1]="No";
  MenuTexts[2]="";
  MenuTexts[3]="";
  MenuTexts[4]="";
  MenuTexts[5]="";
  MenuTexts[8]="";
  MenuTexts[6]="";
  MenuTexts[7]="";
  MenuTexts[9]="";


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
      MenuTexts[0]=" BACK ";
      MenuTexts[1]="";
      DoMenuSelection ( " YOU DONT HAVE ANYTHING IN INVENTORY (i.e. not equipped!), THAT COULD BE SOLD. " , 
			MenuTexts, 1 , NULL );
      return;
    }


  while ( !SpacePressed() && !EscapePressed() )
    {
      // InitiateMenu( NULL );
      InitiateMenu( SHOP_BACKGROUND_IMAGE );

      //--------------------
      // Now we draw our selection of items to the screen, at least the part
      // of it, that's currently visible
      //
      DisplayText( " I WOULD BUY FROM YOU THESE ITEMS        YOUR GOLD:" , 50 , 50 + (0) * ITEM_MENU_DISTANCE , NULL );
      sprintf( DescriptionText , "%4ld" , Me[0].Gold );
      DisplayText( DescriptionText , 580 , 50 + ( 0 ) * 80 , NULL );
      for ( i = 0 ; ( (i < NUMBER_OF_ITEMS_ON_ONE_SCREEN) && (Sell_Pointer_List[ i + MenuInListPosition ] != NULL ) ) ; i++ )
	{
	  // DisplayText( ItemMap [ Repair_Pointer_List[ i + ]->type ].ItemName , 50 , 50 + i * 50 , NULL );
	  // DisplayText( "\n" , -1 , -1, NULL );
	  GiveItemDescription( DescriptionText , Sell_Pointer_List [ i + MenuInListPosition ] , TRUE );
	  DisplayText( DescriptionText , 50 , 50 + (i+1) * ITEM_MENU_DISTANCE , NULL );
	  sprintf( DescriptionText , "%6.0f" , 
		   SELL_PRICE_FACTOR * CalculateItemPrice ( Sell_Pointer_List[ i + MenuInListPosition] , FALSE ) );
	  DisplayText( DescriptionText , 580 , 50 + (i+1) * ITEM_MENU_DISTANCE , NULL );
	}
      
      //--------------------
      // Now we draw the influencer as a cursor
      //
      PutInfluence ( 10 , 50 + ( InMenuPosition + 1 ) * ITEM_MENU_DISTANCE , 0 );

      //--------------------
      //
      //
      SDL_Flip ( Screen );

      if ( UpPressed() )
	{
	  if ( InMenuPosition > 0 ) InMenuPosition --;
	  else 
	    {
	      if ( MenuInListPosition > 0 )
		MenuInListPosition --;
	    }
	  while ( UpPressed() );
	}
      if ( DownPressed() )
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

  if ( SpacePressed() ) TryToSellItem( Sell_Pointer_List[ InMenuPosition + MenuInListPosition ] ) ;

  while ( SpacePressed() || EscapePressed() );

}; // void Sell_Items( void )

/* ----------------------------------------------------------------------
 * This function tells over which menu item the mouse cursor would be,
 * if there were infinitely many menu items.
 * ---------------------------------------------------------------------- */
int
MouseCursorIsOverMenuItem( first_menu_item_pos_y )
{
  int h = FontHeight ( GetCurrentFont() );
  
  return ( ( ( GetMousePos_y () + 16 - first_menu_item_pos_y ) / h ) + 1 );

}; // void MouseCursorIsOverMenuItem( first_menu_item_pos_y )


/* ----------------------------------------------------------------------
 * This function performs a menu for the player to select from, using the
 * keyboard only, currently, sorry.
 * ---------------------------------------------------------------------- */
int
DoMenuSelection( char* InitialText , char* MenuTexts[10] , int FirstItem , char* BackgroundToUse )
{
  int h = FontHeight (GetCurrentFont());
  int i;
  static int MenuPosition = 1;
  int NumberOfOptionsGiven;
  int first_menu_item_pos_y;

  if ( FirstItem != (-1) ) MenuPosition = FirstItem;

  //--------------------
  // First thing we do is find out how may options we have
  // been given for the menu
  //
  for ( i = 0 ; i < 10 ; i ++ )
    {
      if ( strlen( MenuTexts[ i ] ) == 0 ) break;
    }
  NumberOfOptionsGiven = i;


  first_menu_item_pos_y = ( SCREENHEIGHT - NumberOfOptionsGiven * h ) / 2 ;

  SetCurrentFont ( Menu_BFont );
  // SetCurrentFont ( FPS_Display_BFont );
  h = FontHeight ( GetCurrentFont() );

  // DisplayText ( InitialText , 50 , 50 , &Full_Screen_Rect );

  while ( 1 )
    {

      InitiateMenu( BackgroundToUse );

      //--------------------
      // We highlight the currently selected option with an 
      // influencer to the left and right of it.
      //
      PutInfluence( ( SCREENLEN - TextWidth ( MenuTexts [ MenuPosition - 1 ] ) ) / 2 - Block_Width/3 , 
		    first_menu_item_pos_y +
		    ( MenuPosition - 0.5 ) * h , 0 );
      PutInfluence( ( SCREENLEN + TextWidth ( MenuTexts [ MenuPosition - 1 ] ) ) / 2 + Block_Width/3 , 
		    first_menu_item_pos_y +
		    ( MenuPosition - 0.5 ) * h , 0 );

      for ( i = 0 ; i < 10 ; i ++ )
	{
	  if ( strlen( MenuTexts[ i ] ) == 0 ) continue;
	  CenteredPutString ( Screen ,  first_menu_item_pos_y + i * h , MenuTexts[ i ] );
	}
      if ( strlen( InitialText ) > 0 ) 
	DisplayText ( InitialText , 50 , 50 , NULL );

      SDL_Flip( Screen );
  
      if ( EscapePressed() )
	{
	  while ( EscapePressed() );
	  MenuItemDeselectedSound();
	  return ( -1 );
	}
      if ( EnterPressed() || SpacePressed() ) 
	{
	  //--------------------
	  // The space key or enter key or left mouse button all indicate, that
	  // the user has made a selection.
	  //
	  // In the case of the mouse button, we must of couse first check, if 
	  // the mouse button really was over a valid menu item and otherwise
	  // ignore the button.
	  //
	  // In case of a key, we always have a valid selection.
	  //
	  if ( axis_is_active )
	    {
	      if ( ( MouseCursorIsOverMenuItem( first_menu_item_pos_y ) >= 1 ) &&
		   ( MouseCursorIsOverMenuItem( first_menu_item_pos_y ) <= NumberOfOptionsGiven ) )
		{
		  MenuPosition = MouseCursorIsOverMenuItem( first_menu_item_pos_y );
		  while ( EnterPressed() || SpacePressed() );
		  MenuItemSelectedSound();
		  return ( MenuPosition );
		}
	    }
	  else
	    {
	      while ( EnterPressed() || SpacePressed() );
	      MenuItemSelectedSound();
	      return ( MenuPosition );
	    }
	}
      if (UpPressed()) 
	{
	  if (MenuPosition > 1) MenuPosition--;
	  MoveMenuPositionSound();
	  while (UpPressed());
	}
      if (DownPressed()) 
	{
	  if ( MenuPosition < NumberOfOptionsGiven ) MenuPosition++;
	  MoveMenuPositionSound();
	  while (DownPressed());
	}

      /*
      if ( ( MouseCursorIsOverMenuItem( first_menu_item_pos_y ) >= 1 ) &&
	   ( MouseCursorIsOverMenuItem( first_menu_item_pos_y ) <= NumberOfOptionsGiven ) )
	{
	  MenuPosition = MouseCursorIsOverMenuItem( first_menu_item_pos_y );
	}
      */

    }

  return ( -1 );
}; // int DoMenuSelection( char* InitialText , char* MenuTexts[10] , asdfasd .... )

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
      MenuTexts[6]="";
      MenuTexts[7]="";
      MenuTexts[9]="";

      MenuPosition = DoMenuSelection( "" , MenuTexts , -1 , SHOP_BACKGROUND_IMAGE );

      switch (MenuPosition) 
	{
	case (-1):
	  Weiter=!Weiter;
	  break;
	case BUY_BASIC_ITEMS:
	  while (EnterPressed() || SpacePressed() );
	  Buy_Basic_Items( FALSE , FALSE );
	  break;
	case BUY_PREMIUM_ITEMS:
	  while (EnterPressed() || SpacePressed() );
	  Buy_Basic_Items( FALSE , TRUE );
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
 * This function does all the buying/selling interaction with the 
 * healer of the small starting town.
 * ---------------------------------------------------------------------- */
void
HealerMenu ( void )
{
enum
  { 
    BUY_POTIONS=1, 
    SELL_POTIONS, 
    HEALER_GOSSIP, 
    LEAVE_HEALER
  };

  int Weiter = 0;
  int MenuPosition=1;
  char* MenuTexts[10];

  Me[0].status=MENU;

  DebugPrintf (2, "\nvoid HealerMenu(void): real function call confirmed."); 

  // Prevent distortion of framerate by the delay coming from 
  // the time spend in the menu.
  Activate_Conservative_Frame_Computation();
  while ( EscapePressed() );

  while (!Weiter)
    {
      MenuTexts[0]="Buy Potions and Stuff";
      MenuTexts[1]="Sell Stuff";
      MenuTexts[2]="Gossip";
      MenuTexts[3]="Leave the Healer alone";
      MenuTexts[4]="";
      MenuTexts[5]="";
      MenuTexts[8]="";
      MenuTexts[6]="";
      MenuTexts[7]="";
      MenuTexts[9]="";

      MenuPosition = DoMenuSelection( "" , MenuTexts , 1 , NULL );

      switch (MenuPosition) 
	{
	case (-1):
	  Weiter=!Weiter;
	  break;
	case BUY_POTIONS:
	  while (EnterPressed() || SpacePressed() );
	  Buy_Basic_Items ( TRUE , FALSE );
	  break;
	case SELL_POTIONS:
	  while (EnterPressed() || SpacePressed() );
	  Sell_Items ( TRUE );
	  break;
	case HEALER_GOSSIP:
	  while (EnterPressed() || SpacePressed() );
	  // Sell_Items( TRUE );
	  break;
	case LEAVE_HEALER:
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
}; // void HealerMenu ( void )

/*@Function============================================================
@Desc: This function prepares the screen for the big Escape menu and 
       its submenus.  This means usual content of the screen, i.e. the 
       combat screen and top status bar, is "faded out", the rest of 
       the screen is cleared.  This function resolves some redundance 
       that occured since there are so many submenus needing this.

@Ret: none
* $Function----------------------------------------------------------*/
void 
InitiateMenu( char* BackgroundToUse )
{
  //--------------------
  // Here comes the standard initializer for all the menus and submenus
  // of the big escape menu.  This prepares the screen, so that we can
  // write on it further down.
  //
  SDL_SetClipRect( Screen, NULL );
  ClearGraphMem();

  if ( BackgroundToUse == NULL )
    {
      DisplayBanner (NULL, NULL,  BANNER_NO_SDL_UPDATE | BANNER_FORCE_UPDATE );
      Assemble_Combat_Picture ( 0 );
      MakeGridOnScreen( NULL );
    }
  else
    {
      DisplayImage ( find_file ( BackgroundToUse , GRAPHICS_DIR, FALSE ) );
    }

  SDL_SetClipRect( Screen, NULL );
} // void InitiateMenu(void)

/*@Function============================================================
@Desc: This function provides a convenient cheat menu, so that any 
       tester does not have to play all through the game again and again
       to see if a bug in a certain position has been removed or not.

@Ret:  none
* $Function----------------------------------------------------------*/
extern int CurrentlyCPressed; 	/* the key that brought as in here */
				/* we need to make sure it is set as released */
				/* before we leave ...*/
void
Cheatmenu (void)
{
  char *input;		/* string input from user */
  int Weiter;
  int LNum, X, Y, num;
  int i, l;
  int x0, y0, line;
  Waypoint WpList;      /* pointer on current waypoint-list  */
  BFont_Info *font;

  // Prevent distortion of framerate by the delay coming from 
  // the time spend in the menu.
  Activate_Conservative_Frame_Computation();

  font =  FPS_Display_BFont;


  SetCurrentFont (font);  /* not the ideal one, but there's currently */
				/* no other it seems.. */
  x0 = 50;
  y0 = 20;
  line = 0;

  Weiter = FALSE;
  while (!Weiter)
    {
      ClearGraphMem ();
      printf_SDL (Screen, x0, y0, "Current position: Level=%d, X=%d, Y=%d\n",
		   CurLevel->levelnum, (int)Me[0].pos.x, (int)Me[0].pos.y);
      printf_SDL (Screen, -1, -1, " a. Armageddon (alle Robots sprengen)\n");
      printf_SDL (Screen, -1, -1, " l. robot list of current level\n");
      printf_SDL (Screen, -1, -1, " g. complete robot list\n");
      printf_SDL (Screen, -1, -1, " d. destroy robots on current level\n");
      printf_SDL (Screen, -1, -1, " t. Teleportation\n");
      printf_SDL (Screen, -1, -1, " r. change to new robot type\n");
      printf_SDL (Screen, -1, -1, " i. Invinciblemode: %s",
		  InvincibleMode ? "ON\n" : "OFF\n");
      printf_SDL (Screen, -1, -1, " e. set energy\n");
      printf_SDL (Screen, -1, -1, " h. Hide invisible map parts: %s",
		  HideInvisibleMap ? "ON\n" : "OFF\n" );
      printf_SDL (Screen, -1, -1, " n. No hidden droids: %s",
		  show_all_droids ? "ON\n" : "OFF\n" );
      printf_SDL (Screen, -1, -1, " m. Map of Deck xy\n");
      printf_SDL (Screen, -1, -1, " s. Sound: %s",
		  sound_on ? "ON\n" : "OFF\n");
      printf_SDL (Screen, -1, -1, " x. Fullscreen : %s",
		  fullscreen_on ? "ON\n" : "OFF\n");
      printf_SDL (Screen, -1, -1, " w. Print current waypoints\n");
      printf_SDL (Screen, -1, -1, " z. change Zoom factor\n");
      printf_SDL (Screen, -1, -1, " f. Freeze on this positon: %s",
		  stop_influencer ? "ON\n" : "OFF\n");
      printf_SDL (Screen, -1, -1, " q. RESUME game\n");

      switch (getchar_raw ())
	{
	case 'f':
	  stop_influencer = !stop_influencer;
	  break;

	case 'z':
	  ClearGraphMem();
	  printf_SDL (Screen, x0, y0, "Current Zoom factor: %f\n",
		      CurrentCombatScaleFactor); 
	  printf_SDL (Screen, -1, -1, "New zoom factor: ");
	  input = GetString (40, 2);
	  sscanf (input, "%f", &CurrentCombatScaleFactor);
	  free (input);
	  SetCombatScaleTo (CurrentCombatScaleFactor);
	  break;

	case 'a': /* armageddon */
	  Weiter = 1;
	  Armageddon ();
	  break;

	case 'l': /* robot list of this deck */
	  l = 0; /* line counter for enemy output */
	  for (i = 0; i < NumEnemys; i++)
	    {
	      if (AllEnemys[i].pos.z == CurLevel->levelnum) 
		{
		  if (l && !(l%20)) 
		    {
		      printf_SDL (Screen, -1, -1, " --- MORE --- \n");
		      if( getchar_raw () == 'q')
			break;
		    }
		  if (!(l % 20) )  
		    {
		      ClearGraphMem ();
		      printf_SDL (Screen, x0, y0,
				   " NR.   ID  X    Y   ENERGY   speedX\n");
		      printf_SDL (Screen, -1, -1,
				  "---------------------------------------------\n");
		    }
		  
		  l ++;
		  printf_SDL (Screen, -1, -1,
			      "%d.   %s   %d   %d   %d    %g.\n", i,
			       Druidmap[AllEnemys[i].type].druidname,
			       (int)AllEnemys[i].pos.x,
			       (int)AllEnemys[i].pos.y,
			       (int)AllEnemys[i].energy,
			       AllEnemys[i].speed.x);
		} /* if (enemy on current level)  */
	    } /* for (i<NumEnemys) */

	  printf_SDL (Screen, -1, -1," --- END --- \n");
	  getchar_raw ();
	  break;

	case 'g': /* complete robot list of this ship */
	  for (i = 0; i < MAX_ENEMYS_ON_SHIP ; i++)
	    {
	      if ( AllEnemys[i].type == (-1) ) continue;

	      if (i && !(i%13)) 
		{
		  printf_SDL (Screen, -1, -1, " --- MORE --- ('q' to quit)\n");
		  if (getchar_raw () == 'q')
		    break;
		}
	      if ( !(i % 13) )
		{
		  ClearGraphMem ();
		  printf_SDL (Screen, x0, y0, "Nr.  Lev. ID  Energy  Speed.x\n");
		  printf_SDL (Screen, -1, -1, "------------------------------\n");
		}
	      
	      printf_SDL (Screen, -1, -1, "%d  %d  %s  %d  %g\n",
			  i, AllEnemys[i].pos.z,
			  Druidmap[AllEnemys[i].type].druidname,
			  (int)AllEnemys[i].energy,
			  AllEnemys[i].speed.x);
	    } /* for (i<NumEnemys) */

	  printf_SDL (Screen, -1, -1, " --- END ---\n");
	  getchar_raw ();
	  break;


	case 'd': /* destroy all robots on this level, haha */
	  for (i = 0; i < NumEnemys; i++)
	    {
	      if (AllEnemys[i].pos.z == CurLevel->levelnum)
		AllEnemys[i].energy = -100;
	    }
	  printf_SDL (Screen, -1, -1, "All robots on this deck killed!\n");
	  getchar_raw ();
	  break;


	case 't': /* Teleportation */
	  ClearGraphMem ();
	  printf_SDL (Screen, x0, y0, "Enter Level, X, Y: ");
	  input = GetString (40, 2);
	  sscanf (input, "%d, %d, %d\n", &LNum, &X, &Y);
	  free (input);
	  Teleport ( LNum , X , Y , 0 ) ;
	  break;

	case 'r': /* change to new robot type */
	  ClearGraphMem ();
	  printf_SDL (Screen, x0, y0, "Type number of new robot: ");
	  input = GetString (40, 2);
	  for (i = 0; i < Number_Of_Droid_Types ; i++)
	    if (!strcmp (Druidmap[i].druidname, input))
	      break;

	  if ( i == Number_Of_Droid_Types )
	    {
	      printf_SDL (Screen, x0, y0+20,
			  "Unrecognized robot-type: %s", input);
	      getchar_raw ();
	      ClearGraphMem();
	    }
	  else
	    {
	      Me[0].type = i;
	      Me[0].energy = Me[0].maxenergy;
	      Me[0].health = Me[0].energy;
	      printf_SDL (Screen, x0, y0+20, "You are now a %s. Have fun!\n", input);
	      getchar_raw ();
	    }
	  free (input);
	  break;

	case 'i': /* togge Invincible mode */
	  InvincibleMode = !InvincibleMode;
	  break;

	case 'e': /* complete heal */
	  ClearGraphMem();
	  printf_SDL (Screen, x0, y0, "Current energy: %f\n", Me[0].energy);
	  printf_SDL (Screen, -1, -1, "Enter your new energy: ");
	  input = GetString (40, 2);
	  sscanf (input, "%d", &num);
	  free (input);
	  Me[0].energy = (double) num;
	  if (Me[0].energy > Me[0].health) Me[0].health = Me[0].energy;
	  break;

	case 'h': /* toggle hide invisible map */
	  HideInvisibleMap = !HideInvisibleMap;
	  break;

	case 'n': /* toggle display of all droids */
	  show_all_droids = !show_all_droids;
	  break;

	case 's': /* toggle sound on/off */
	  sound_on = !sound_on;
	  break;

	case 'm': /* Show deck map in Concept view */
	  printf_SDL (Screen, -1, -1, "\nLevelnum: ");
	  input = GetString (40, 2);
	  sscanf (input, "%d", &LNum);
	  free (input);
	  ShowDeckMap (curShip.AllLevels[LNum]);
	  getchar_raw ();
	  break;

	case 'x': /* toggle fullscreen - mode */
	  fullscreen_on = !fullscreen_on;
	  break;
	  
	case 'w':  /* print waypoint info of current level */
	  WpList = CurLevel->AllWaypoints;
	  for (i=0; i<MAXWAYPOINTS && WpList[i].x; i++)
	    {
	      if (i && !(i%20))
		{
		  printf_SDL (Screen, -1, -1, " ---- MORE -----\n");
		  if (getchar_raw () == 'q')
		    break;
		}
	      if ( !(i%20) )
		{
		  ClearGraphMem ();
		  printf_SDL (Screen, x0, y0, "Nr.   X   Y      C1  C2  C3  C4\n");
		  printf_SDL (Screen, -1, -1, "------------------------------------\n");
		}
	      printf_SDL (Screen, -1, -1, "%2d   %2d  %2d      %2d  %2d  %2d  %2d\n",
			  i, WpList[i].x, WpList[i].y,
			  WpList[i].connections[0],
			  WpList[i].connections[1],
			  WpList[i].connections[2],
			  WpList[i].connections[3]);

	    } /* for (all waypoints) */
	  printf_SDL (Screen, -1, -1, " --- END ---\n");
	  getchar_raw ();
	  break;

	case ' ':
	case 'q':
	  Weiter = 1;
	  break;
	} /* switch (getchar_raw()) */
    } /* while (!Weiter) */

  InitBars = TRUE;

  ClearGraphMem ();
  SDL_Flip (Screen);

  keyboard_update (); /* treat all pending keyboard events */
  /* 
   * when changing windows etc, sometimes a key-release event gets 
   * lost, so we have to make sure that CPressed is no longer set
   * or we stay here for ever...
   */
  CurrentlyCPressed = FALSE;

  return;
} /* Cheatmenu() */

/* ----------------------------------------------------------------------
 * This function lets you select whether you with to play the classical
 * Paradroid episode or the new missions.  (Might be obsolete soon, since
 * classical paradroid is finished.)
 * ---------------------------------------------------------------------- */
void
MissionSelectMenu (void)
{
#define FIRST_MIS_SELECT_ITEM_POS_X (0.0*Block_Width)
#define FIRST_MIS_SELECT_ITEM_POS_Y (BANNER_HEIGHT + FontHeight(Menu_BFont))
enum
  { 
    CLASSIC_PARADROID_MISSION_POSITION=1, 
    NEW_MISSION_POSITION,
    RESTART_PREVIOUS_MISSION
  };
  int Weiter = 0;
  int MenuPosition=1;
  int key;
  static int NoMissionLoadedEver=TRUE;

  Me[0].status=MENU;

  DebugPrintf (2, "\nvoid MissionSelectMenu(void): real function call confirmed."); 

  SDL_SetClipRect( Screen , NULL );

  // Prevent distortion of framerate by the delay coming from 
  // the time spend in the menu.
  Activate_Conservative_Frame_Computation();

  // This is not some Debug Menu but an optically impressive 
  // menu for the player.  Therefore I suggest we just fade out
  // the game screen a little bit.

  SetCurrentFont( Para_BFont );
  
  while ( EscapePressed() );

  while (!Weiter)
    {

      // InitiateMenu();
      DisplayImage (find_file (NE_TITLE_PIC_FILE, GRAPHICS_DIR, FALSE));

      // 
      // we highlight the currently selected option with an 
      // influencer to the left before it
      // PutInfluence( FIRST_MENU_ITEM_POS_X , 
      // FIRST_MENU_ITEM_POS_Y + (MenuPosition-1) * (FontHeight(Menu_BFont)) - Block_Width/4 );
      SetCurrentFont ( Menu_BFont );
      PutInfluence( FIRST_MIS_SELECT_ITEM_POS_X , FIRST_MIS_SELECT_ITEM_POS_Y + ( MenuPosition - 1.5 ) * (FontHeight( Menu_BFont )) , 0 );

      CenteredPutString (Screen ,  FIRST_MIS_SELECT_ITEM_POS_Y -2*FontHeight(GetCurrentFont()), "Mission Selection Menu");
      CenteredPutString (Screen ,  FIRST_MIS_SELECT_ITEM_POS_Y ,    "Classic Paradroid");
      CenteredPutString (Screen ,  FIRST_MIS_SELECT_ITEM_POS_Y +1*FontHeight(GetCurrentFont()), "The Return of the Influence Device");
      CenteredPutString (Screen ,  FIRST_MIS_SELECT_ITEM_POS_Y +2*FontHeight(GetCurrentFont()), "Restart Previous Mission");

      SDL_Flip( Screen );

      // Wait until the user does SOMETHING

      key = getchar_raw ();
      if ( (key == SDLK_RETURN) || (key == SDLK_SPACE))
	{
	  MenuItemSelectedSound();
	  switch (MenuPosition) 
	    {

	    case CLASSIC_PARADROID_MISSION_POSITION:
	      InitNewMissionList ( STANDARD_MISSION );
	      NoMissionLoadedEver = FALSE;
	      Weiter = TRUE;   
	      break;
	    case NEW_MISSION_POSITION:
	      InitNewMissionList ( NEW_MISSION );
	      NoMissionLoadedEver = FALSE;
	      Weiter = TRUE;   // jp forgot this... ;)
	      break;
	    case RESTART_PREVIOUS_MISSION:
	      if ( NoMissionLoadedEver )
		{
		  CenteredPutString (Screen ,  FIRST_MIS_SELECT_ITEM_POS_Y +5*FontHeight(GetCurrentFont()), "No previous mission known.");
		  SDL_Flip( Screen );
		  while ( EnterPressed() );
		  while ( (!EnterPressed()) && (!SpacePressed()) );
		}
	      else
		{
		  InitNewMissionList ( Previous_Mission_Name );
		  Weiter = TRUE;   /* jp forgot this... ;) */
		}
	      break;
	    default: 
	      break;
	    } 
	  // Weiter=!Weiter;
	}
      if ( key == SDLK_UP )
	{
	  if (MenuPosition > 1) MenuPosition--;
	  MoveMenuPositionSound();
	}
      if ( key == SDLK_DOWN )
	{
	  if ( MenuPosition < RESTART_PREVIOUS_MISSION ) MenuPosition++;
	  MoveMenuPositionSound();
	}
      if ( key == SDLK_ESCAPE )
	{
	  Terminate( OK );
	}
    }

  ClearGraphMem();
  // Since we've faded out the whole scren, it can't hurt
  // to have the top status bar redrawn...
  BannerIsDestroyed=TRUE;
  Me[0].status=MOBILE;

  return;

} // MissionSelectMenu

/* ----------------------------------------------------------------------
 * This function lets you select whether you want to play the single player
 * mode or the multi player mode or the credits or the intro again or exit.
 * ---------------------------------------------------------------------- */
void
StartupMenu (void)
{
#define FIRST_MIS_SELECT_ITEM_POS_X (0.0*Block_Width)
#define FIRST_MIS_SELECT_ITEM_POS_Y (BANNER_HEIGHT + FontHeight(Menu_BFont))
enum
  { 
    SINGLE_PLAYER_POSITION=1, 
    MULTI_PLAYER_POSITION,
    CREDITS_POSITION,
    EXIT_FREEDROID_POSITION
  };
  int Weiter = 0;
  int MenuPosition=1;
  char* MenuTexts[10];

  Me[0].status=MENU;

  DebugPrintf ( 1 , "\nvoid StartupMenu ( void ): real function call confirmed. "); 

  SDL_SetClipRect( Screen , NULL );

  // Prevent distortion of framerate by the delay coming from 
  // the time spent in the menu.
  Activate_Conservative_Frame_Computation();

  while (!Weiter)
    {
      SetCurrentFont ( Menu_BFont );

      MenuTexts[0]="Single Player";
      MenuTexts[1]="Multi Player";
      MenuTexts[2]="Credits";
      MenuTexts[3]="Exit Freedroid";
      MenuTexts[4]="";
      MenuTexts[5]="";
      MenuTexts[6]="";
      MenuTexts[7]="";
      MenuTexts[8]="";
      MenuTexts[9]="";

      MenuPosition = DoMenuSelection( "" , MenuTexts , -1 , NE_TITLE_PIC_FILE );

      switch (MenuPosition) 
	{
	case SINGLE_PLAYER_POSITION:
	  // InitNewMissionList ( NEW_MISSION );
	  Weiter = Single_Player_Menu ( );
	  break;
	case MULTI_PLAYER_POSITION:
	  DisplayImage (find_file (NE_TITLE_PIC_FILE, GRAPHICS_DIR, FALSE));
	  SetCurrentFont ( Menu_BFont );
	  Weiter = Multi_Player_Menu();
	  break;
	case CREDITS_POSITION:
	  Credits_Menu();
	  break;
	case (-1):
	case EXIT_FREEDROID_POSITION:
	  Terminate( OK );
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

}; // void StartupMenu( void );

/* ----------------------------------------------------------------------
 * This function provides a the big escape menu from where you can get 
 * into different submenus.
 * ---------------------------------------------------------------------- */
void
EscapeMenu (void)
{
enum
  { 
    SAVE_GAME_POSITION=1,
    // SINGLE_PLAYER_POSITION, 
    OPTIONS_POSITION, 
    SET_THEME,
    LEVEL_EDITOR_POSITION, 
    LOAD_GAME_POSITION,
    QUIT_POSITION
  };

  int Weiter = 0;
  int MenuPosition=1;
  char theme_string[40];
  int i;
  char* MenuTexts[10];

  Me[0].status=MENU;

  DebugPrintf (2, "\nvoid EscapeMenu(void): real function call confirmed."); 

  // Prevent distortion of framerate by the delay coming from 
  // the time spend in the menu.
  Activate_Conservative_Frame_Computation();
  while ( EscapePressed() );

  while (!Weiter)
    {
      strcpy (theme_string, "Theme: ");
      if (strstr (GameConfig.Theme_SubPath, "classic"))
	strcat (theme_string, "Classic");
      else if (strstr (GameConfig.Theme_SubPath, "lanzz"))
	strcat (theme_string, "Lanzz");
      else
	strcat (theme_string, "unknown");

      MenuTexts[1]="Single Player";

      MenuTexts[0]="Save Game";
      MenuTexts[1]="Options";
      MenuTexts[2]=theme_string;
      MenuTexts[3]="Level Editor";
      MenuTexts[4]="Load Game";
      MenuTexts[5]="Quit Game";
      MenuTexts[6]="";
      MenuTexts[7]="";
      MenuTexts[8]="";
      MenuTexts[9]="";

      MenuPosition = DoMenuSelection( "" , MenuTexts , -1 , NE_TITLE_PIC_FILE );

      switch (MenuPosition) 
	{
	case (-1):
	  Weiter=!Weiter;
	  break;
	  /*
	case SINGLE_PLAYER_POSITION:
	  while (EnterPressed() || SpacePressed() );
	  New_Game_Requested=FALSE;
	  Single_Player_Menu();
	  if (New_Game_Requested) Weiter = TRUE;   // jp forgot this... ;) 
	  break;
	  */
	case OPTIONS_POSITION:
	  while (EnterPressed() || SpacePressed() );
	  Options_Menu();
	  // Weiter = TRUE;   /* jp forgot this... ;) */
	  break;
	case SET_THEME:
	  while (EnterPressed() || SpacePressed() );
	  if ( !strcmp ( GameConfig.Theme_SubPath , "classic_theme/" ) )
	    {
	      GameConfig.Theme_SubPath="lanzz_theme/";
	    }
	  else
	    {
	      GameConfig.Theme_SubPath="classic_theme/";
	    }
	  ReInitPictures();
	  
	  //--------------------
	  // Now we have loaded a new theme with new images!!  It might however be the
	  // case, that also the number of phases per bullet, which is specific to each
	  // theme, has been changed!!! THIS MUST NOT BE IGNORED, OR WE'LL SEGFAULT!!!!
	  // Because the old number of phases is still attached to living bullets, it
	  // might try to blit a new (higher) number of phases although there are only
	  // less Surfaces generated for the bullet in the old theme.  The solution seems
	  // to be simply to request new graphics to be attached to each bullet, which
	  // should be simply setting a flag for each of the bullets:
	  for ( i = 0 ; i < MAXBULLETS ; i++ )
	    {
	      AllBullets[i].Surfaces_were_generated = FALSE ;
	    }
	  break;
	case LEVEL_EDITOR_POSITION:
	  while (EnterPressed() || SpacePressed() );
	  Level_Editor();
	  // Weiter = TRUE;   /* jp forgot this... ;) */
	  break;
	case LOAD_GAME_POSITION:
	  LoadGame(  );
	  Weiter = TRUE;
	  break;
	case SAVE_GAME_POSITION:
	  SaveGame(  );
	  break;
	case QUIT_POSITION:
	  DebugPrintf (2, "\nvoid EscapeMenu( void ): Quit Requested by user.  Terminating...");
	  Terminate(0);
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

}; // void EscapeMenu( void )

/*@Function============================================================
@Desc: This function provides a the options menu.  This menu is a 
       submenu of the big EscapeMenu.  Here you can change sound vol.,
       gamma correction, fullscreen mode, display of FPS and such
       things.

@Ret:  none
* $Function----------------------------------------------------------*/
void
GraphicsSound_Options_Menu (void)
{
  int Weiter = 0;
  int MenuPosition=1;
#define OPTIONS_MENU_ITEM_POS_X (Block_Width/2)
enum
  { SET_BG_MUSIC_VOLUME=1, 
    SET_SOUND_FX_VOLUME, 
    SET_GAMMA_CORRECTION, 
    SET_FULLSCREEN_FLAG, 
    CW_SIZE,
    LEAVE_OPTIONS_MENU };

  // This is not some Debug Menu but an optically impressive 
  // menu for the player.  Therefore I suggest we just fade out
  // the game screen a little bit.

  while ( EscapePressed() );

  while (!Weiter)
    {

      SDL_SetClipRect( Screen, NULL );
      ClearGraphMem();
      DisplayBanner (NULL, NULL,  BANNER_NO_SDL_UPDATE | BANNER_FORCE_UPDATE );
      Assemble_Combat_Picture ( 0 );
      SDL_SetClipRect( Screen, NULL );
      MakeGridOnScreen( NULL );

      // 
      // we highlight the currently selected option with an 
      // influencer to the left before it
      // PutInfluence( FIRST_MENU_ITEM_POS_X , 
      // FIRST_MENU_ITEM_POS_Y + (MenuPosition-1) * (FontHeight(Menu_BFont)) - Block_Width/4 , 0 );
      PutInfluence( OPTIONS_MENU_ITEM_POS_X - Block_Width/2, 
		    FIRST_MENU_ITEM_POS_Y + ( MenuPosition - 1.5 ) * (FontHeight( Menu_BFont )) , 0 );


      PrintStringFont (Screen , Menu_BFont, OPTIONS_MENU_ITEM_POS_X , FIRST_MENU_ITEM_POS_Y+0*FontHeight(Menu_BFont),
		       "Background Music Volume: %1.2f" , GameConfig.Current_BG_Music_Volume );
      PrintStringFont (Screen , Menu_BFont, OPTIONS_MENU_ITEM_POS_X , FIRST_MENU_ITEM_POS_Y+1*FontHeight(Menu_BFont), 
		       "Sound Effects Volume: %1.2f", GameConfig.Current_Sound_FX_Volume );
      PrintStringFont (Screen , Menu_BFont, OPTIONS_MENU_ITEM_POS_X , FIRST_MENU_ITEM_POS_Y+2*FontHeight(Menu_BFont), 
		       "Gamma Correction: %1.2f", GameConfig.Current_Gamma_Correction );
      PrintStringFont (Screen , Menu_BFont, OPTIONS_MENU_ITEM_POS_X , FIRST_MENU_ITEM_POS_Y+3*FontHeight(Menu_BFont), 
		       "Fullscreen Mode: %s", fullscreen_on ? "ON" : "OFF");
      PrintStringFont (Screen , Menu_BFont, OPTIONS_MENU_ITEM_POS_X , FIRST_MENU_ITEM_POS_Y+4*FontHeight(Menu_BFont), 
		       "Combat Window Size: %s", classic_user_rect ? "CLASSIC" : "FULL" );

      //PrintStringFont (Screen , Menu_BFont, OPTIONS_MENU_ITEM_POS_X , FIRST_MENU_ITEM_POS_Y+4*FontHeight(Menu_BFont),
      //"Show Framerate: %s", GameConfig.Draw_Framerate? "ON" : "OFF");
      //PrintStringFont (Screen , Menu_BFont, OPTIONS_MENU_ITEM_POS_X , FIRST_MENU_ITEM_POS_Y+5*FontHeight(Menu_BFont),
      //"Show Energy: %s", GameConfig.Draw_Energy? "ON" : "OFF");
      PrintStringFont (Screen , Menu_BFont, OPTIONS_MENU_ITEM_POS_X , FIRST_MENU_ITEM_POS_Y+6*FontHeight(Menu_BFont), 
		       "Back");

      SDL_Flip( Screen );

      // Wait until the user does SOMETHING

      while( !SpacePressed() && !EnterPressed() && !UpPressed()
	     && !DownPressed() && !LeftPressed() && !RightPressed() && !EscapePressed() ) ;

      if ( EscapePressed() )
	{
	  while ( EscapePressed() );
	  Weiter=!Weiter;
	}

      // Some menu options can be controlled by pressing right or left
      // These options are gamma corrections, sound volume and the like
      // Therefore left and right key must be resprected.  This is done here:
      if (RightPressed() || LeftPressed() ) 
	{
	  if (MenuPosition == SET_BG_MUSIC_VOLUME ) 
	    {
	      if (RightPressed()) 
		{
		  while (RightPressed());
		  if ( GameConfig.Current_BG_Music_Volume < 1 ) GameConfig.Current_BG_Music_Volume += 0.05;
		  Set_BG_Music_Volume( GameConfig.Current_BG_Music_Volume );
		}
	      if (LeftPressed()) 
		{
		  while (LeftPressed());
		  if ( GameConfig.Current_BG_Music_Volume > 0 ) GameConfig.Current_BG_Music_Volume -= 0.05;
		  Set_BG_Music_Volume( GameConfig.Current_BG_Music_Volume );
		}
	    }
	  if (MenuPosition == SET_SOUND_FX_VOLUME ) 
	    {
	      if (RightPressed()) 
		{
		  while (RightPressed());
		  if ( GameConfig.Current_Sound_FX_Volume < 1 ) GameConfig.Current_Sound_FX_Volume += 0.05;
		  Set_Sound_FX_Volume( GameConfig.Current_Sound_FX_Volume );
		}
	      if (LeftPressed()) 
		{
		  while (LeftPressed());
		  if ( GameConfig.Current_Sound_FX_Volume > 0 ) GameConfig.Current_Sound_FX_Volume -= 0.05;
		  Set_Sound_FX_Volume( GameConfig.Current_Sound_FX_Volume );
		}
	    }
	  if (MenuPosition == SET_GAMMA_CORRECTION ) 
	    {
	      if (RightPressed()) 
		{
		  while (RightPressed());
		  GameConfig.Current_Gamma_Correction+=0.05;
		  SDL_SetGamma( GameConfig.Current_Gamma_Correction , GameConfig.Current_Gamma_Correction , GameConfig.Current_Gamma_Correction );
		}
	      if (LeftPressed()) 
		{
		  while (LeftPressed());
		  GameConfig.Current_Gamma_Correction-=0.05;
		  SDL_SetGamma( GameConfig.Current_Gamma_Correction , GameConfig.Current_Gamma_Correction , GameConfig.Current_Gamma_Correction );
		}
	    }
	}


      if (EnterPressed() || SpacePressed() ) 
	{
	  MenuItemSelectedSound();
	  switch (MenuPosition) 
	    {
	    case SET_FULLSCREEN_FLAG:
	      while (EnterPressed() || SpacePressed() );
	      SDL_WM_ToggleFullScreen (Screen);
	      fullscreen_on = !fullscreen_on;
	      break;

	    case CW_SIZE:
	      while (EnterPressed() || SpacePressed() );
	      
	      if (classic_user_rect)
		{
		  classic_user_rect = FALSE;
		  Copy_Rect (Full_User_Rect, User_Rect);
		}
	      else
		{
		  classic_user_rect = TRUE;
		  Copy_Rect (Classic_User_Rect, User_Rect);
		}

	      ClearGraphMem();
	      DisplayBanner( NULL , NULL , BANNER_FORCE_UPDATE );
	      SDL_Flip( Screen );
	      
	      break;

	    case LEAVE_OPTIONS_MENU:
	      while (EnterPressed() || SpacePressed() );
	      Weiter=TRUE;
	      break;
	    default: 
	      break;
	    } 
	  // Weiter=!Weiter;
	}
      if (UpPressed()) 
	{
	  if ( MenuPosition > 1 ) MenuPosition--;
	  MoveMenuPositionSound();
	  while (UpPressed());
	}
      if (DownPressed()) 
	{
	  if ( MenuPosition < LEAVE_OPTIONS_MENU ) MenuPosition++;
	  MoveMenuPositionSound();
	  while (DownPressed());
	}
    }

  ClearGraphMem ();
  DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE );
  InitBars = TRUE;

  return;

}; // GraphicsSound_Options_Menu

/*@Function============================================================
@Desc: This function provides a the options menu.  This menu is a 
       submenu of the big EscapeMenu.  Here you can change sound vol.,
       gamma correction, fullscreen mode, display of FPS and such
       things.

@Ret:  none
* $Function----------------------------------------------------------*/
void
On_Screen_Display_Options_Menu (void)
{
  int Weiter = 0;
  int MenuPosition=1;
  char Options0[1000];
  char Options1[1000];
  char Options2[1000];
  char* MenuTexts[10]={ "" , "" , "" , "" , "" ,
			"" , "" , "" , "" , "" };
  enum
    { 
      SHOW_POSITION=1, 
      SHOW_FRAMERATE, 
      SHOW_ENERGY,
      LEAVE_OPTIONS_MENU 
    };

  // This is not some Debug Menu but an optically impressive 
  // menu for the player.  Therefore I suggest we just fade out
  // the game screen a little bit.

  while ( EscapePressed() );

  while (!Weiter)
    {

      sprintf( Options0 , "Show Position: %s", GameConfig.Draw_Position ? "ON" : "OFF" );
      sprintf( Options1 , "Show Framerate: %s", GameConfig.Draw_Framerate? "ON" : "OFF" );
      sprintf( Options2 , "Show Energy: %s", GameConfig.Draw_Energy? "ON" : "OFF" );
      MenuTexts[0]=Options0;
      MenuTexts[1]=Options1;
      MenuTexts[2]=Options2;
      MenuTexts[3]="Back";

      MenuPosition = DoMenuSelection( "" , MenuTexts , -1 , NULL );

      switch (MenuPosition) 
	{
	case (-1):
	  Weiter=!Weiter;
	  break;
	case SHOW_POSITION:
	  while (EnterPressed() || SpacePressed() );
	  GameConfig.Draw_Position=!GameConfig.Draw_Position;
	  break;
	case SHOW_FRAMERATE:
	  while (EnterPressed() || SpacePressed() );
	  GameConfig.Draw_Framerate=!GameConfig.Draw_Framerate;
	  break;
	case SHOW_ENERGY:
	  while (EnterPressed() || SpacePressed() );
	  GameConfig.Draw_Energy=!GameConfig.Draw_Energy;
	  break;
	case LEAVE_OPTIONS_MENU:
	  while (EnterPressed() || SpacePressed() );
	  Weiter=TRUE;
	  break;
	default: 
	  break;
	} 
    }

  ClearGraphMem ();
  DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE );
  InitBars = TRUE;

  return;

}; // On_Screen_Display_Options_Menu

/*@Function============================================================
@Desc: This function provides a the options menu.  This menu is a 
       submenu of the big EscapeMenu.  Here you can change sound vol.,
       gamma correction, fullscreen mode, display of FPS and such
       things.

@Ret:  none
* $Function----------------------------------------------------------*/
void
Droid_Talk_Options_Menu (void)
{

  int Weiter = 0;
  int MenuPosition=1;
  char Options0[1000];
  char Options1[1000];
  char Options2[1000];
  char Options3[1000];
  char Options4[1000];
  char Options5[1000];
  char* MenuTexts[10]={ "" , "" , "" , "" , "" ,
			"" , "" , "" , "" , "" };
  enum
    { 
      INFLU_REFRESH_TEXT=1,
      INFLU_BLAST_TEXT,
      ENEMY_HIT_TEXT,
      ENEMY_BUMP_TEXT,
      ENEMY_AIM_TEXT,
      ALL_TEXTS,
      LEAVE_DROID_TALK_OPTIONS_MENU 
    };

  while (!Weiter)
    {
      sprintf( Options0 , "Influencer Refresh Texts: %s" , GameConfig.Influencer_Refresh_Text ? "ON" : "OFF" );
      sprintf( Options1 , "Influencer Blast Texts: %s", GameConfig.Influencer_Blast_Text ? "ON" : "OFF" );
      sprintf( Options2 , "Enemy Hit Texts: %s", GameConfig.Enemy_Hit_Text ? "ON" : "OFF" );
      sprintf( Options3 , "Enemy Bumped Texts: %s", GameConfig.Enemy_Bump_Text ? "ON" : "OFF" );
      sprintf( Options4 , "Enemy Aim Texts: %s", GameConfig.Enemy_Aim_Text ? "ON" : "OFF" );
      sprintf( Options5 , "All in-game Speech: %s", GameConfig.All_Texts_Switch ? "ON" : "OFF" );
      MenuTexts[0]=Options0;
      MenuTexts[1]=Options1;
      MenuTexts[2]=Options2;
      MenuTexts[3]=Options3;
      MenuTexts[4]=Options4;
      MenuTexts[5]=Options5;
      MenuTexts[6]="Back";

      MenuPosition = DoMenuSelection( "" , MenuTexts , -1 , NULL );

      switch (MenuPosition) 
	{
	case (-1):
	  Weiter=!Weiter;
	  break;
	case INFLU_REFRESH_TEXT:
	  while (EnterPressed() || SpacePressed() );
	  GameConfig.Influencer_Refresh_Text=!GameConfig.Influencer_Refresh_Text;
	  break;
	case INFLU_BLAST_TEXT:
	  while (EnterPressed() || SpacePressed() );
	  GameConfig.Influencer_Blast_Text=!GameConfig.Influencer_Blast_Text;
	  break;
	case ENEMY_HIT_TEXT:
	  while (EnterPressed() || SpacePressed() );
	  GameConfig.Enemy_Hit_Text=!GameConfig.Enemy_Hit_Text;
	  break;
	case ENEMY_BUMP_TEXT:
	  while (EnterPressed() || SpacePressed() );
	  GameConfig.Enemy_Bump_Text=!GameConfig.Enemy_Bump_Text;
	  break;
	case ENEMY_AIM_TEXT:
	  while (EnterPressed() || SpacePressed() );
	  GameConfig.Enemy_Aim_Text=!GameConfig.Enemy_Aim_Text;
	  break;
	case ALL_TEXTS:
	  while (EnterPressed() || SpacePressed() );
	  GameConfig.All_Texts_Switch=!GameConfig.All_Texts_Switch;
	  break;
	case LEAVE_DROID_TALK_OPTIONS_MENU:
	  while (EnterPressed() || SpacePressed() );
	  Weiter=TRUE;
	  break;
	default: 
	  break;
	} 
    }

  ClearGraphMem ();
  DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE );
  InitBars = TRUE;

  return;

}; // Droid_Talk_Options_Menu

/*@Function============================================================
@Desc: This function provides a the options menu.  This menu is a 
       submenu of the big EscapeMenu.  Here you can change sound vol.,
       gamma correction, fullscreen mode, display of FPS and such
       things.

@Ret:  none
* $Function----------------------------------------------------------*/
void
Options_Menu (void)
{
  int Weiter = 0;
  int MenuPosition=1;
  char* MenuTexts[10];
enum
  { GRAPHICS_SOUND_OPTIONS=1, 
    DROID_TALK_OPTIONS,
    ON_SCREEN_DISPLAYS,
    SAVE_OPTIONS, 
    LEAVE_OPTIONS_MENU };

  // This is not some Debug Menu but an optically impressive 
  // menu for the player.  Therefore I suggest we just fade out
  // the game screen a little bit.

  MenuTexts[0]="Graphics & Sound";
  MenuTexts[1]="Droid Talk";
  MenuTexts[2]="On-Screen Displays";
  MenuTexts[3]="Save Options";
  MenuTexts[4]="Back";
  MenuTexts[5]="";
  MenuTexts[6]="";
  MenuTexts[7]="";
  MenuTexts[8]="";
  MenuTexts[9]="";

  while ( !Weiter )
    {
      MenuPosition = DoMenuSelection( "" , MenuTexts , -1 , NULL );

      switch (MenuPosition) 
	{
	case (-1):
	  Weiter=!Weiter;
	  break;
	case GRAPHICS_SOUND_OPTIONS:
	  while (EnterPressed() || SpacePressed() );
	  GraphicsSound_Options_Menu();
	  break;
	case DROID_TALK_OPTIONS:
	  while (EnterPressed() || SpacePressed() );
	  Droid_Talk_Options_Menu();
	  break;
	case ON_SCREEN_DISPLAYS:
	  while (EnterPressed() || SpacePressed() );
	  On_Screen_Display_Options_Menu();
	  break;
	case SAVE_OPTIONS:
	  while (EnterPressed() || SpacePressed() );
	  break;
	case LEAVE_OPTIONS_MENU:
	  while (EnterPressed() || SpacePressed() );
	  Weiter=TRUE;
	  break;
	default: 
	  break;
	} 
    }

  ClearGraphMem ();
  DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE );
  InitBars = TRUE;

  return;

} // Options_Menu

/* ----------------------------------------------------------------------
 * This reads in the new name for the character...
 * ---------------------------------------------------------------------- */
void
Get_Server_Name ( void )
{
  char* Temp;
  InitiateMenu( NE_TITLE_PIC_FILE );

  DisplayText ( "\n\
 Please enter name of server to connect to:\n\
 You can give an empty string for the local host.\n\
 \n      " , 50 , 50 , NULL );

  Temp = GetString( 140 , FALSE );
  strcpy ( ServerName , Temp );
  free( Temp );
}; // void Get_New_Character_Name ( void )

/* ----------------------------------------------------------------------
 * This reads in the new name for the character...
 * ---------------------------------------------------------------------- */
void
Get_New_Character_Name ( void )
{
  char* Temp;
  InitiateMenu( NE_TITLE_PIC_FILE );

  DisplayText ( "\n     Enter the name for the new hero:\n\n\n      " , 
		50 , 50 , NULL );

  Temp = GetString( 20 , FALSE );
  strcpy ( Me[0].character_name , Temp );
  free( Temp );
}; // void Get_New_Character_Name ( void )

/* ----------------------------------------------------------------------
 * This function does the selection of the hero class...
 * ---------------------------------------------------------------------- */
int
Select_Hero_Class_Menu (void)
{
  int Weiter = 0;
  int MenuPosition=1;
  char* MenuTexts[10];
  int i;

enum
  { 
    WAR_BOT_POSITION=1, 
    SNIPER_BOT_POSITION, 
    MIND_BOT_POSITION,
    BACK_POSITION
  };

  MenuTexts[0]="War Tux";
  MenuTexts[1]="Sniper Tux";
  MenuTexts[2]="Hacker Tux";
  MenuTexts[3]="Back";
  MenuTexts[4]="";
  MenuTexts[5]="";
  MenuTexts[6]="";
  MenuTexts[7]="";
  MenuTexts[8]="";
  MenuTexts[9]="";

  //--------------------
  // At first we clear the inventory of the new character
  // of any items (or numeric waste) that might be in there
  //
  for ( i = 0 ; i < MAX_ITEMS_IN_INVENTORY ; i ++ )
    {
      Me[0].Inventory[ i ].type = (-1);
      Me[0].Inventory[ i ].prefix_code = (-1);
      Me[0].Inventory[ i ].suffix_code = (-1);
      Me[0].Inventory[ i ].currently_held_in_hand = FALSE;
    }
  DebugPrintf ( 1 , "\nSelect_Hero...( ... ): Inventory has been emptied...");


  //--------------------
  // Now we add some safety, against 'none present' items
  //
  Me[0].weapon_item.type = ( -1 ) ;
  Me[0].drive_item.type = ( -1 ) ;
  Me[0].armour_item.type = ( -1 ) ;
  Me[0].shield_item.type = ( -1 ) ;
  Me[0].aux1_item.type = ( -1 ) ;
  Me[0].aux2_item.type = ( -1 ) ;
  Me[0].special_item.type = ( -1 ) ;

  Me[0].weapon_item.prefix_code = ( -1 ) ;
  Me[0].drive_item.prefix_code = ( -1 ) ;
  Me[0].armour_item.prefix_code = ( -1 ) ;
  Me[0].shield_item.prefix_code = ( -1 ) ;
  Me[0].aux1_item.prefix_code = ( -1 ) ;
  Me[0].aux2_item.prefix_code = ( -1 ) ;
  Me[0].special_item.prefix_code = ( -1 ) ;

  Me[0].weapon_item.suffix_code = ( -1 ) ;
  Me[0].drive_item.suffix_code = ( -1 ) ;
  Me[0].armour_item.suffix_code = ( -1 ) ;
  Me[0].shield_item.suffix_code = ( -1 ) ;
  Me[0].aux1_item.suffix_code = ( -1 ) ;
  Me[0].aux2_item.suffix_code = ( -1 ) ;
  Me[0].special_item.suffix_code = ( -1 ) ;

  while (!Weiter)
    {
      MenuPosition = DoMenuSelection( "" , MenuTexts , -1 , NE_TITLE_PIC_FILE );

      //--------------------
      // Now let's see what the user has pressed...
      //
      switch (MenuPosition) 
	{
	case (-1):
	  Weiter=!Weiter;
	  break;
	case WAR_BOT_POSITION:
	  while (EnterPressed() || SpacePressed() ) ;

	  Me[0].character_class = WAR_BOT;
	  Me[0].base_vitality = 25;
	  Me[0].base_strength = 30;
	  Me[0].base_dexterity = 25;
	  Me[0].base_magic = 10;

	  // Me[0].weapon_item.type = ITEM_SHORT_SWORD;
	  Me[0].drive_item.type = ITEM_ANTIGRAV_BETA;

	  Me[0].Inventory[ 0 ].type = ITEM_SHORT_SWORD;
	  Me[0].Inventory[ 0 ].inventory_position.x = 0;
	  Me[0].Inventory[ 0 ].inventory_position.y = 0;
	  Me[0].Inventory[ 1 ].type = ITEM_BUCKLER;
	  Me[0].Inventory[ 1 ].inventory_position.x = 2;
	  Me[0].Inventory[ 1 ].inventory_position.y = 0;
	  Me[0].Inventory[ 2 ].type = ITEM_SMALL_HEALTH_POTION;
	  Me[0].Inventory[ 2 ].inventory_position.x = 0;
	  Me[0].Inventory[ 2 ].inventory_position.y = INVENTORY_GRID_HEIGHT-1;
	  Me[0].Inventory[ 3 ].type = ITEM_SMALL_HEALTH_POTION;
	  Me[0].Inventory[ 3 ].inventory_position.x = 1;
	  Me[0].Inventory[ 3 ].inventory_position.y = INVENTORY_GRID_HEIGHT-1;
	  FillInItemProperties ( & Me[0].Inventory[ 0 ] , TRUE , 0 );
	  FillInItemProperties ( & Me[0].Inventory[ 1 ] , TRUE , 0 );
	  FillInItemProperties ( & Me[0].Inventory[ 2 ] , TRUE , 0 );
	  FillInItemProperties ( & Me[0].Inventory[ 3 ] , TRUE , 0 );

	  Get_New_Character_Name( );
	  return ( TRUE );
	  break;
	case SNIPER_BOT_POSITION: 
	  while (EnterPressed() || SpacePressed() ) ;

	  Me[0].character_class = SNIPER_BOT;
	  Me[0].base_vitality = 20;
	  Me[0].base_strength = 25;
	  Me[0].base_dexterity = 35;
	  Me[0].base_magic = 20;
	  
	  Me[0].drive_item.type = ITEM_ANTIGRAV_BETA;

	  Me[0].Inventory[ 0 ].type = ITEM_SHORT_BOW;
	  Me[0].Inventory[ 0 ].inventory_position.x = 0;
	  Me[0].Inventory[ 0 ].inventory_position.y = 0;
	  Me[0].Inventory[ 1 ].type = ITEM_SMALL_HEALTH_POTION;
	  Me[0].Inventory[ 1 ].inventory_position.x = 0;
	  Me[0].Inventory[ 1 ].inventory_position.y = INVENTORY_GRID_HEIGHT-1;
	  Me[0].Inventory[ 2 ].type = ITEM_SMALL_HEALTH_POTION;
	  Me[0].Inventory[ 2 ].inventory_position.x = 1;
	  Me[0].Inventory[ 2 ].inventory_position.y = INVENTORY_GRID_HEIGHT-1;
	  FillInItemProperties ( & Me[0].Inventory[ 0 ] , TRUE , 0 );
	  FillInItemProperties ( & Me[0].Inventory[ 1 ] , TRUE , 0 );
	  FillInItemProperties ( & Me[0].Inventory[ 2 ] , TRUE , 0 );

	  Get_New_Character_Name( );
	  return ( TRUE );
	  break;
	case MIND_BOT_POSITION: 
	  while (EnterPressed() || SpacePressed() ) ;

	  Me[0].character_class = MIND_BOT;
	  Me[0].base_vitality = 15;
	  Me[0].base_strength = 15;
	  Me[0].base_dexterity = 20;
	  Me[0].base_magic = 35;
	  Me[0].drive_item.type = ITEM_ANTIGRAV_ALPHA;

	  Me[0].Inventory[ 0 ].type = ITEM_STAFF;
	  Me[0].Inventory[ 0 ].inventory_position.x = 0;
	  Me[0].Inventory[ 0 ].inventory_position.y = 0;
	  Me[0].Inventory[ 1 ].type = ITEM_SMALL_MANA_POTION;
	  Me[0].Inventory[ 1 ].inventory_position.x = 0;
	  Me[0].Inventory[ 1 ].inventory_position.y = INVENTORY_GRID_HEIGHT-1;
	  Me[0].Inventory[ 2 ].type = ITEM_SMALL_MANA_POTION;
	  Me[0].Inventory[ 2 ].inventory_position.x = 1;
	  Me[0].Inventory[ 2 ].inventory_position.y = INVENTORY_GRID_HEIGHT-1;
	  FillInItemProperties ( & Me[0].Inventory[ 0 ] , TRUE , 0 );
	  FillInItemProperties ( & Me[0].Inventory[ 1 ] , TRUE , 0 );
	  FillInItemProperties ( & Me[0].Inventory[ 2 ] , TRUE , 0 );

	  Get_New_Character_Name( );
	  return ( TRUE );
	  break;
	case BACK_POSITION:
	  while (EnterPressed() || SpacePressed() ) ;
	  Weiter=!Weiter;
	  return ( FALSE );
	  break;
	default: 
	  break;
	}
    }
  return ( FALSE );
}; // int Select_Hero_Class_Menu ( void );

/* ----------------------------------------------------------------------
 * This function does the selection of the hero class...
 * ---------------------------------------------------------------------- */
int
Connect_To_Existing_Server_Menu (void)
{

  //--------------------
  // Now we add some safety, against 'none present' items
  //
  Me[0].weapon_item.type = ( -1 ) ;
  Me[0].drive_item.type = ( -1 ) ;
  Me[0].armour_item.type = ( -1 ) ;
  Me[0].shield_item.type = ( -1 ) ;
  Me[0].aux1_item.type = ( -1 ) ;
  Me[0].aux2_item.type = ( -1 ) ;
  Me[0].special_item.type = ( -1 ) ;

  Me[0].weapon_item.prefix_code = ( -1 ) ;
  Me[0].drive_item.prefix_code = ( -1 ) ;
  Me[0].armour_item.prefix_code = ( -1 ) ;
  Me[0].shield_item.prefix_code = ( -1 ) ;
  Me[0].aux1_item.prefix_code = ( -1 ) ;
  Me[0].aux2_item.prefix_code = ( -1 ) ;
  Me[0].special_item.prefix_code = ( -1 ) ;

  Me[0].weapon_item.suffix_code = ( -1 ) ;
  Me[0].drive_item.suffix_code = ( -1 ) ;
  Me[0].armour_item.suffix_code = ( -1 ) ;
  Me[0].shield_item.suffix_code = ( -1 ) ;
  Me[0].aux1_item.suffix_code = ( -1 ) ;
  Me[0].aux2_item.suffix_code = ( -1 ) ;
  Me[0].special_item.suffix_code = ( -1 ) ;

  Me[0].character_class = WAR_BOT;
  Me[0].base_vitality = 25;
  Me[0].base_strength = 30;
  Me[0].base_dexterity = 25;
  Me[0].base_magic = 10;

  // Me[0].weapon_item.type = ITEM_SHORT_SWORD;
  Me[0].drive_item.type = ITEM_ANTIGRAV_BETA;

  Me[0].Inventory[ 0 ].type = ITEM_SHORT_SWORD;
  Me[0].Inventory[ 0 ].inventory_position.x = 0;
  Me[0].Inventory[ 0 ].inventory_position.y = 0;
  Me[0].Inventory[ 1 ].type = ITEM_BUCKLER;
  Me[0].Inventory[ 1 ].inventory_position.x = 2;
  Me[0].Inventory[ 1 ].inventory_position.y = 0;
  Me[0].Inventory[ 2 ].type = ITEM_SMALL_HEALTH_POTION;
  Me[0].Inventory[ 2 ].inventory_position.x = 0;
  Me[0].Inventory[ 2 ].inventory_position.y = INVENTORY_GRID_HEIGHT-1;
  Me[0].Inventory[ 3 ].type = ITEM_SMALL_HEALTH_POTION;
  Me[0].Inventory[ 3 ].inventory_position.x = 1;
  Me[0].Inventory[ 3 ].inventory_position.y = INVENTORY_GRID_HEIGHT-1;
  FillInItemProperties ( & Me[0].Inventory[ 0 ] , TRUE , 0 );
  FillInItemProperties ( & Me[0].Inventory[ 1 ] , TRUE , 0 );
  FillInItemProperties ( & Me[0].Inventory[ 2 ] , TRUE , 0 );
  FillInItemProperties ( & Me[0].Inventory[ 3 ] , TRUE , 0 );

  Get_Server_Name ( );
  Get_New_Character_Name( );

  ConnectToFreedroidServer (  );

  if ( strlen ( ServerName ) > 0 )
    return ( TRUE );
  else
    return ( FALSE );

}; // int Connect_To_Existing_Server_Menu ( void );

/* ----------------------------------------------------------------------
 * The GNU C Library is SOOOO COOOL!!! It contains functions for directory
 * manipulations that are SOOOO powerful, it's really awesome.  One of
 * these very powerful functions can be used to filter directory entries,
 * so that only a certain kind of files will be displayed any more.  How
 * convenient.  So as a parameter to this powerful function (scandir), you
 * have to specify a sorting function of a certain kind.  And this is just
 * the sorting function that seems appropriate for our little program.
 * ---------------------------------------------------------------------- */
static int
one (const struct dirent *unused)
{
  if ( strstr ( unused->d_name , "savegame" ) != NULL )
    {
      return ( 1 ) ;
    }
  else
    {
      return ( 0 ) ;
    }

  // to make compilers happy...
  return ( 0 );

}; // static int one (struct dirent *unused)

/* ----------------------------------------------------------------------
 * This is the function available from the freedroid startup menu, that
 * should display the available characters in the users home directory
 * and eventually let the player select one of his old characters there.
 * ---------------------------------------------------------------------- */
int 
Load_Existing_Hero_Menu ( void )
{
  char *homedir;
  // DIR *dp;
  // struct dirent *ep;
  struct dirent **eps;
  int n;  
  int cnt;
  char* MenuTexts[10];
  int MenuPosition;

  DebugPrintf ( 0 , "\nint Load_Existing_Hero_Menu ( void ): real function call confirmed.");
  InitiateMenu( NE_TITLE_PIC_FILE );
  MenuTexts[0]="";
  MenuTexts[1]="";
  MenuTexts[2]="";
  MenuTexts[3]="";
  MenuTexts[4]="";
  MenuTexts[5]="";
  MenuTexts[6]="";
  MenuTexts[7]="";
  MenuTexts[8]="";
  MenuTexts[9]="";

  // get home-directory to save in
  if ( (homedir = getenv("HOME")) == NULL ) 
    {
      DebugPrintf ( 0 , "ERROR: Environment does not contain HOME variable... \n\
I need to know that for saving. Abort.\n");
      Terminate( ERR );
      return (ERR);
    }

  // DisplayText ( "This is the record of all your characters:\n\n" , 50 , 50 , NULL );

  //--------------------
  // This is a slightly modified copy of the code sniplet from the
  // GNU C Library description on directory operations...
  //
  n = scandir ( homedir , &eps, one , alphasort);
  if (n >= 0)
    {
      for (cnt = 0; cnt < n; ++cnt) 
	{
	  puts ( eps[cnt]->d_name );
	  DisplayText ( eps[cnt]->d_name , 50 , 150 + cnt * 40 , NULL );
	  if ( cnt < 10 ) 
	    {
	      MenuTexts[ cnt ] = ReadAndMallocStringFromData ( eps[cnt]->d_name , "" , ".savegame" ) ;
	    }
	}

      MenuPosition = DoMenuSelection( "The first 10 characters: " , MenuTexts , 1 , NE_TITLE_PIC_FILE );

      if ( MenuPosition == (-1) ) return ( FALSE );
      else
	{
	  InitNewMissionList ( NEW_MISSION );
	  strcpy( Me[0].character_name , MenuTexts[ MenuPosition -1 ] );
	  LoadGame( );
	  return ( TRUE );
	}
    }
  else
    {
      DebugPrintf( 0 , "\n\nERROR!! Couldn't open the directory in int Load_Existing_Hero_Menu ( void ).\nTerminatin...");
      Terminate( ERR );
    }


  SDL_Flip( Screen );

  return ( OK );
}; // int Load_Existing_Hero_Menu ( void )


/* ----------------------------------------------------------------------
 * This function provides the single player menu.  It offers to start a
 * new hero, to load an old one and to go back.
 * ---------------------------------------------------------------------- */
int
Single_Player_Menu (void)
{
  int Weiter = 0;
  int MenuPosition=1;
  char* MenuTexts[10];

enum
  { 
    NEW_HERO_POSITION=1, 
    LOAD_EXISTING_HERO_POSITION, 
    BACK_POSITION
  };

  MenuTexts[0]="New Hero";
  MenuTexts[1]="Load existing Hero";
  MenuTexts[2]="Back";
  MenuTexts[3]="";
  MenuTexts[4]="";
  MenuTexts[5]="";
  MenuTexts[6]="";
  MenuTexts[7]="";
  MenuTexts[8]="";
  MenuTexts[9]="";

  while (!Weiter)
    {
      MenuPosition = DoMenuSelection( "" , MenuTexts , 1 , NE_TITLE_PIC_FILE );

      switch (MenuPosition) 
	{
	case (-1):
	  Weiter=!Weiter;
	  break;
	case NEW_HERO_POSITION:
	  while (EnterPressed() || SpacePressed() ) ;

	  if ( Select_Hero_Class_Menu ( ) )
	    {
	      InitNewMissionList ( NEW_MISSION );
	      Weiter=TRUE;
	      return ( TRUE );
	    }

	  break;
	case LOAD_EXISTING_HERO_POSITION: 
	  while (EnterPressed() || SpacePressed() ) ;

	  if ( Load_Existing_Hero_Menu ( ) == TRUE )
	    {
	      Weiter = TRUE;
	      return ( TRUE );
	    }
	  else
	    {
	      Weiter = FALSE;
	      // return ( FALSE );
	    }

	  break;
	case BACK_POSITION:
	  while (EnterPressed() || SpacePressed() ) ;
	  Weiter=!Weiter;
	  return ( FALSE );
	  break;
	default: 
	  break;
	}
    }
  return ( TRUE );
}; // void Single_Player_Menu ( void );


/* ----------------------------------------------------------------------
 * This function provides the multi player menu.  It is a submenu
 * of the big EscapeMenu.  Instead of connecting to a server or 
 * something it simply displayes the nonchalant message, that 
 * nothing is implemented yet, but sooner or later it will be.
 * ---------------------------------------------------------------------- */
int
Multi_Player_Menu (void)
{
  int Weiter = 0;
  int MenuPosition=1;
  char* MenuTexts[10];

enum
  { 
    START_AS_SERVER_POSITION=1, 
    JOIN_EXISTING_MULTIPLAYER_POSITION, 
    LIST_KNOWN_SERVERS,
    BACK_POSITION
  };

  MenuTexts[0]="Start as a Server";
  MenuTexts[1]="Join existing Multiplayer game";
  MenuTexts[2]="List known Servers";
  MenuTexts[3]="Back";
  MenuTexts[4]="";
  MenuTexts[5]="";
  MenuTexts[6]="";
  MenuTexts[7]="";
  MenuTexts[8]="";
  MenuTexts[9]="";

  while (!Weiter)
    {
      MenuPosition = DoMenuSelection( "" , MenuTexts , 1 , NE_TITLE_PIC_FILE );

      switch (MenuPosition) 
	{
	case (-1):
	  Weiter=!Weiter;
	  break;
	case START_AS_SERVER_POSITION:
	  while (EnterPressed() || SpacePressed() ) ;

	  InitNewMissionList ( NEW_MISSION );	

	  ServerMode = TRUE ;

	  OpenTheServerSocket (  );

	  Weiter=TRUE;
	  return ( TRUE );

	  break;

	case JOIN_EXISTING_MULTIPLAYER_POSITION: 
	  while (EnterPressed() || SpacePressed() ) ;

	  if ( Connect_To_Existing_Server_Menu ( ) == TRUE )
	    {
	      Weiter = TRUE;
	      InitNewMissionList ( NEW_MISSION );
	      ClientMode = TRUE;
	      return ( TRUE );
	    }
	  else
	    {
	      Weiter = FALSE;
	      // return ( FALSE );
	    }

	  break;
	case LIST_KNOWN_SERVERS: 
	  while (EnterPressed() || SpacePressed() ) ;

	  if ( Load_Existing_Hero_Menu ( ) == TRUE )
	    {
	      Weiter = TRUE;
	      return ( TRUE );
	    }
	  else
	    {
	      Weiter = FALSE;
	      // return ( FALSE );
	    }

	  break;
	case BACK_POSITION:
	  while (EnterPressed() || SpacePressed() ) ;
	  Weiter=!Weiter;
	  return ( FALSE );
	  break;
	default: 
	  break;
	}
    }
  return ( TRUE );

} // Multi_Player_Menu

/*@Function============================================================
@Desc: This function provides the credits screen.  It is a submenu of
       the big EscapeMenu.  Here you can see who helped developing the
       game, currently jp, rp and bastian.

@Ret:  none
* $Function----------------------------------------------------------*/
void
Credits_Menu (void)
{
  while( SpacePressed() || EnterPressed() ) ; /* wait for key release */

  // InitiateMenu();
      
  DisplayImage ( find_file(NE_CREDITS_PIC_FILE,GRAPHICS_DIR,FALSE) );

  CenteredPutString ( Screen , 1*FontHeight(Menu_BFont), "CREDITS" );
  LeftPutString ( Screen , 3*FontHeight(Menu_BFont), "   PROGRAMMING:");
  RightPutString ( Screen , 4*FontHeight(Menu_BFont), "Johannes Prix   ");
  RightPutString ( Screen , 5*FontHeight(Menu_BFont), "Reinhard Prix   ");
  LeftPutString ( Screen , 7*FontHeight(Menu_BFont), "   ARTWORK:");
  RightPutString ( Screen , 8*FontHeight(Menu_BFont), "Bastian Salmela   ");
  RightPutString ( Screen , 9*FontHeight(Menu_BFont), "Lanzz   ");

  SDL_Flip( Screen );

  // Wait until the user does SOMETHING
  getchar_raw();

} // Credits_Menu

/*@Function============================================================
@Desc: This function provides the details of a mission that has been
       assigned to the player or has been solved perhaps too

@Ret:  none
* $Function----------------------------------------------------------*/
void
Show_Mission_Details ( int MissionNumber )
{
  int Weiter = 0;

  while( SpacePressed() || EnterPressed() ) keyboard_update(); 

  while (!Weiter)
    {

      DisplayImage (find_file (HS_BACKGROUND_FILE, GRAPHICS_DIR, FALSE));
      MakeGridOnScreen ( (SDL_Rect*) & Full_Screen_Rect );
      DisplayBanner( NULL , NULL , BANNER_FORCE_UPDATE );
      //InitiateMenu();

      CenteredPutString ( Screen ,  1*FontHeight(Menu_BFont),    "MISSION DETAILS");

      printf_SDL ( Screen , User_Rect.x , 3 *FontHeight(Menu_BFont) , "Kill all droids : "  );
      if ( Me[0].AllMissions[ MissionNumber ].KillAll != (-1) ) printf_SDL( Screen , -1 , -1 , "YES" ); 
      else printf_SDL( Screen , -1 , -1 , "NO" );

      printf_SDL ( Screen , User_Rect.x , 4 *FontHeight(Menu_BFont) , "Kill special : "  );
      if ( Me[0].AllMissions[ MissionNumber ].KillOne != (-1) ) printf_SDL( Screen , -1 , -1 , "YES" ); 
      else printf_SDL( Screen , -1 , -1 , "NO" );
      printf_SDL ( Screen , -1 , -1 , "   ReachLevel : "  );
      if ( Me[0].AllMissions[ MissionNumber ].MustReachLevel != (-1) ) printf_SDL( Screen , -1 , -1 , "%d\n" , Me[0].AllMissions[ MissionNumber ].MustReachLevel ); 
      else printf_SDL( Screen , -1 , -1 , "NONE\n" );

      printf_SDL ( Screen , User_Rect.x , 5 *FontHeight(Menu_BFont) , "Reach X= : "  );
      if ( Me[0].AllMissions[ MissionNumber ].MustReachPoint.x != (-1) ) printf_SDL( Screen , -1 , -1 , "%d" , Me[0].AllMissions[ MissionNumber ].MustReachPoint.x ); 
      else printf_SDL( Screen , -1 , -1 , "NONE" );
      printf_SDL ( Screen , -1 , -1 , "   Reach Y= : "  );
      if ( Me[0].AllMissions[ MissionNumber ].MustReachPoint.y != (-1) ) printf_SDL( Screen , -1 , -1 , "%d\n" , Me[0].AllMissions[ MissionNumber ].MustReachPoint.y );
      else printf_SDL( Screen , -1 , -1 , "NONE\n" );

      printf_SDL ( Screen , User_Rect.x , 6 *FontHeight(Menu_BFont) , "Live Time : "  );
      if ( Me[0].AllMissions[ MissionNumber ].MustLiveTime != (-1) ) printf_SDL( Screen , -1 , -1 , "%4.0f" , Me[0].AllMissions[ MissionNumber ].MustLiveTime ); 
      else printf_SDL( Screen , -1 , -1 , "NONE" );
      printf_SDL ( Screen , User_Rect.x , 7 *FontHeight(Menu_BFont) , "Must be class : "  );
      if ( Me[0].AllMissions[ MissionNumber ].MustBeClass != (-1) ) printf_SDL( Screen , -1 , -1 , "%d\n" , Me[0].AllMissions[ MissionNumber ].MustBeClass );
      else printf_SDL( Screen , -1 , -1 , "NONE\n" );

      printf_SDL ( Screen , User_Rect.x , 8 *FontHeight(Menu_BFont) , "Must be type : "  );
      if ( Me[0].AllMissions[ MissionNumber ].MustBeType != (-1) ) printf_SDL( Screen , -1 , -1 , "%d" , Me[0].AllMissions[ MissionNumber ].MustBeType ); 
      else printf_SDL( Screen , -1 , -1 , "NONE" );
      printf_SDL ( Screen , User_Rect.x , 9*FontHeight(Menu_BFont) , "Must be special : "  );
      if ( Me[0].AllMissions[ MissionNumber ].MustBeOne != (-1) ) printf_SDL( Screen , -1 , -1 , "YES" );
      else printf_SDL( Screen , -1 , -1 , "NO\n" );

      printf_SDL ( Screen , User_Rect.x , 10 * FontHeight(Menu_BFont) , "Kill Class : "  );
      if ( Me[0].AllMissions[ MissionNumber ].KillClass != (-1) ) printf_SDL( Screen , -1 , -1 , "%s" , Classname[Me[0].AllMissions[ MissionNumber ].KillClass] ); 
      else printf_SDL( Screen , -1 , -1 , "NONE\n" );

      SDL_Flip( Screen );

      while ( (!EscapePressed()) && (!EnterPressed()) && (!SpacePressed()) );
      // Wait until the user does SOMETHING

      if ( EscapePressed() || EnterPressed() || SpacePressed() )
	{
	  Weiter=!Weiter;
	}
    }
  while ( EscapePressed() || EnterPressed() || SpacePressed() );

  

}; // void Show_Mission_Details (void)

/* ----------------------------------------------------------------------
 * This function provides an overview over the missions currently
 * assigned to the player
 * ---------------------------------------------------------------------- */
void
Show_Mission_Log_Menu (void)
{
  int Weiter = 0;
  int i;
  int NoOfActiveMissions;
  int MenuPosition=1;
  int InterLineSpace=60;
  SDL_Rect* Mission_Window_Pointer=&User_Rect;

#define MISSION_NAME_POS_X 230
#define FIRST_MISSION_POS_Y 50


  while( SpacePressed() || EnterPressed() ) keyboard_update(); 

  while (!Weiter)
    {

      DisplayImage (find_file (HS_BACKGROUND_FILE, GRAPHICS_DIR, FALSE));
      MakeGridOnScreen ( (SDL_Rect*) & Full_Screen_Rect );
      DisplayBanner( NULL , NULL , BANNER_FORCE_UPDATE );

      SetCurrentFont( Para_BFont );

      DisplayText ( "This is the record of all missions you have been assigned:\n\n" , 
		    0 , FIRST_MISSION_POS_Y - 2 * InterLineSpace , Mission_Window_Pointer );

      NoOfActiveMissions=0;
      for ( i = 0 ; i < MAX_MISSIONS_IN_GAME ; i ++ )
	{

	  if ( Me[0].AllMissions[i].MissionExistsAtAll != TRUE ) continue;

	  NoOfActiveMissions++;

	  // DisplayText ( "\nMission status: " , -1 , -1 , Mission_Window_Pointer );

	  if ( Me[0].AllMissions[i].MissionIsComplete == TRUE )
	    {
	      DisplayText ( "SOLVED: " , 0 , FIRST_MISSION_POS_Y + NoOfActiveMissions * InterLineSpace , Mission_Window_Pointer );
	    }
	  else if ( Me[0].AllMissions[i].MissionWasFailed == TRUE )
	    {
	      DisplayText ( "FAILED: " , 0 , FIRST_MISSION_POS_Y + NoOfActiveMissions * InterLineSpace , Mission_Window_Pointer );
	    }
	  else if ( Me[0].AllMissions[i].MissionWasAssigned == TRUE ) 
	    {
	      DisplayText ( "ASSIGNED: " , 0 , FIRST_MISSION_POS_Y + NoOfActiveMissions * InterLineSpace , Mission_Window_Pointer );
	    }
	  else
	    {
	      DisplayText ( "UNASSIGNED: " , 0 , FIRST_MISSION_POS_Y +  NoOfActiveMissions * InterLineSpace , Mission_Window_Pointer );
	    }

	  DisplayText ( Me[0].AllMissions[i].MissionName , MISSION_NAME_POS_X , 
			FIRST_MISSION_POS_Y + NoOfActiveMissions * InterLineSpace ,  Mission_Window_Pointer );

	}

      DisplayText ( "\n\n--- Currently no missions beyond that ---" , 
		    -1 , -1 , Mission_Window_Pointer );

      // Highlight currently selected option with an influencer before it
      PutInfluence( MISSION_NAME_POS_X , FIRST_MISSION_POS_Y + (MenuPosition) * InterLineSpace - Block_Width/4 , 0 );

      // If the user pressed up or down, the cursor within
      // the level editor menu has to be moved, which is done here:
      if (UpPressed()) 
	{
	  if (MenuPosition > 1) MenuPosition--;
	  MoveMenuPositionSound();
	  while (UpPressed());
	}
      if (DownPressed()) 
	{
	  if ( MenuPosition < NoOfActiveMissions ) MenuPosition++;
	  MoveMenuPositionSound();
	  while (DownPressed());
	}

      if ( EnterPressed() || SpacePressed() )
	{
	  Show_Mission_Details ( MenuPosition-1 );
	  while ( EnterPressed() || SpacePressed() );
	}

      SDL_Flip( Screen );

      if ( EscapePressed() || EnterPressed() || SpacePressed() )
	{
	  Weiter=!Weiter;
	}
    } // end of while loop

  // Wait until the user does SOMETHING
  //while ( (!EscapePressed()) && (!EnterPressed()) && (!SpacePressed()) );

  while ( EscapePressed() || EnterPressed() || SpacePressed() );

}; // void Show_Mission_Log_Menu ( void )

#undef _menu_c
