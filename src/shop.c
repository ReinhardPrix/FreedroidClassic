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

#define FIRST_MENU_ITEM_POS_X (1*Block_Width)
#define FIRST_MENU_ITEM_POS_XX ( SCREEN_WIDTH - FIRST_MENU_ITEM_POS_X )
#define FIRST_MENU_ITEM_POS_Y (BANNER_HEIGHT + FontHeight(Menu_BFont) * 3 )

#define NUMBER_OF_ITEMS_ON_ONE_SCREEN 4
#define ITEM_MENU_DISTANCE 80
#define ITEM_FIRST_POS_Y 130

#define SELL_PRICE_FACTOR (0.25)
#define REPAIR_PRICE_FACTOR (0.5)

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
  fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a severe problem:\n\
The MENU CODE was unable to properly resolve a mouse button press\n\
in the function int ClickedMenuItemPosition( void ) in menu.c.\n\
\n\
This indicates a bug in Freedroid.  Please contact the developers.\n\
\n\
For now Freedroid will terminate to draw attention \n\
to the position resolution problem it could not resolve.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" );
  Terminate(ERR);

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
	  DisplayText( DescriptionText , 580 , 50 + (i+1) * ITEM_MENU_DISTANCE , NULL );
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

      //--------------------
      // Maybe the mouse wheel was pressed up or down.  Then of course we
      // must not move the cursor, which is only used for keyboard input
      // but instead we have the menu scrolling up or down, depending on
      // what's currently requested.
      //
      /*
      if ( MouseWheelUpPressed() )
	{
	  if ( MenuInListPosition > 0 )
	    MenuInListPosition --;

	  while ( MouseWheelUpPressed() );
	}
      if ( MouseWheelDownPressed() )
	{
	  if ( MenuInListPosition < Pointer_Index - NUMBER_OF_ITEMS_ON_ONE_SCREEN )
	    MenuInListPosition ++;

	  while ( MouseWheelDownPressed() );
	}      
      */

    } // while not space pressed...

  if ( SpacePressed() || axis_is_active ) 
    {
      SDL_ShowCursor( SDL_ENABLE );
      return ( InMenuPosition + MenuInListPosition ) ;
    }
  /*
  if ( SpacePressed() && !axis_is_active ) 
    {
      SDL_ShowCursor( SDL_ENABLE );
      return ( InMenuPosition + MenuInListPosition ) ;
    }
  else
    {
      if ( ( ClickedMenuItemPosition() != (-1) ) && ( ClickedMenuItemPosition() < Pointer_Index ) )
	{
	  SDL_ShowCursor( SDL_ENABLE );
	  return ( ClickedMenuItemPosition() + MenuInListPosition ) ;
	}
    }
  */

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
	  Play_Shop_ItemRepairedSound( );
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
	  Play_Shop_ItemIdentifiedSound( );

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
TryToSellItem( item* SellItem )
{
  int MenuPosition;
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

  while ( SpacePressed() || EnterPressed() );

  while ( 1 )
    {
      GiveItemDescription( linebuf , SellItem , TRUE );
      strcat ( linebuf , "\n\n    Are you sure you wish to sell this item?" );
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
	  Play_Shop_ItemSoldSound( );
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
      GiveItemDescription( linebuf , BuyItem , TRUE );
      strcat ( linebuf , "\n\n    You can't afford to purchase this item!" );
      DoMenuSelection( linebuf , MenuTexts , 1 , NULL , NULL );
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
		  MenuPosition = DoMenuSelection( linebuf , MenuTexts , 1 , NULL , NULL );
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
		      Play_Shop_ItemBoughtSound( );
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
  item SalesList[ MAX_ITEMS_IN_INVENTORY ];
  item* Buy_Pointer_List[ MAX_ITEMS_IN_INVENTORY ];
  int i;
  // int InMenuPosition = 0;
  // int MenuInListPosition = 0;
  char DescriptionText[5000];
  int basic_items_number = 20; // this must be <= MAX_ITEMS_IN_INVENTORY -1 !!!
  int ItemSelected;

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

      Buy_Pointer_List [ i ] = & ( SalesList[ i ] ) ;
    }
  //--------------------
  // Now me make sure that this list really gets terminated
  // as required by the item selection function.
  //
  Buy_Pointer_List [ i ] = NULL ; 
  
  //--------------------
  // Now we display the list of items for sale and react
  // to the given user selection.
  //
  sprintf( DescriptionText , " I HAVE THESE ITEMS FOR SALE         YOUR GOLD:  %4ld" , Me[0].Gold );
  ItemSelected = DoEquippmentListSelection( DescriptionText , Buy_Pointer_List , PRICING_FOR_BUY );

  if ( ItemSelected != (-1) ) TryToBuyItem( Buy_Pointer_List[ ItemSelected ] ) ;

}; // void Buy_Basic_Items( void )

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
      DoMenuSelection ( " YOU DONT HAVE ANYTHING THAT WOULD NEED REPAIR " , MenuTexts , 1 , NULL , NULL );
      return;
    }

  //--------------------
  // Now we display the list of items for repair and react
  // to the given user selection.
  //
  sprintf( DescriptionText , " I COULD REPAIR THESE ITEMS           YOUR GOLD:  %4ld" , Me[0].Gold );
  ItemSelected = DoEquippmentListSelection( DescriptionText , Repair_Pointer_List , PRICING_FOR_REPAIR );

  if ( ItemSelected != (-1) ) TryToRepairItem( Repair_Pointer_List[ ItemSelected ] ) ;

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
      DoMenuSelection ( " YOU DONT HAVE ANYTHING THAT WOULD NEED TO BE IDENTIFIED!" , MenuTexts , 1 , NULL , NULL );
      return;
    }

  //--------------------
  // Now we display the list of items for identification and react
  // to the given user selection.
  //
  sprintf( DescriptionText , " I COULD IDENTIFY THESE ITEMS             YOUR GOLD:  %4ld" , Me[0].Gold );
  ItemSelected = DoEquippmentListSelection( DescriptionText , Identify_Pointer_List , PRICING_FOR_IDENTIFY );
  if ( ItemSelected != (-1) ) TryToIdentifyItem( Identify_Pointer_List[ ItemSelected ] ) ;

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
      DoMenuSelection ( " YOU DONT HAVE ANYTHING IN INVENTORY (I.E. NOT WORN), THAT COULD BE SOLD. " , 
			MenuTexts, 1 , NULL , NULL );
      return;
    }

  //--------------------
  // Now we display the list of items for sale and react
  // to the given user selection.
  //
  sprintf( DescriptionText , " I WOULD BUY FROM YOU THESE ITEMS        YOUR GOLD:  %4ld" , Me[0].Gold );
  ItemSelected = DoEquippmentListSelection( DescriptionText , Sell_Pointer_List , PRICING_FOR_SELL );

  if ( ItemSelected != (-1) ) TryToSellItem( Sell_Pointer_List[ ItemSelected ] ) ;

}; // void Sell_Items( void )

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

      // MenuPosition = DoMenuSelection( "" , MenuTexts , -1 , SHOP_BACKGROUND_IMAGE , NULL );
      MenuPosition = DoMenuSelection( "" , MenuTexts , -1 , SHOP_BACKGROUND_IMAGE , Menu_Filled_BFont );

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

      MenuPosition = DoMenuSelection( "" , MenuTexts , 1 , NULL , NULL );

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


#undef _shop_c

