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

#define SELL_PRICE_FACTOR (0.25)
#define REPAIR_PRICE_FACTOR (0.5)

#define SHOP_ROW_LENGTH 8

SDL_Rect ItemRowRect;

/* ----------------------------------------------------------------------
 * Maybe the user has clicked right onto the item overview row.  Then of
 * course we must find out and return the index of the item clicked on.
 * If no item was clicked on, then a -1 will be returned as index.
 * ---------------------------------------------------------------------- */
int
ClickWasOntoItemRowPosition ( int x , int y )
{
  if ( y < ItemRowRect . y ) return (-1) ;
  if ( y > ItemRowRect . y + ItemRowRect.h ) return (-1) ;
  if ( x < ItemRowRect . x ) return (-1) ;
  if ( x > ItemRowRect . x + ItemRowRect.w ) return (-1) ;

  //--------------------
  // Now at this point we know, that the click really was in the item
  // overview row.  Therefore we just need to find out the index and
  // can return;
  //
  return ( ( x - ItemRowRect . x ) / INITIAL_BLOCK_WIDTH );

};

/* ----------------------------------------------------------------------
 * The item row in the shop interface (or whereever we're going to use it)
 * should display not only the rotating item display but also a row or a
 * column of the current equipment, so that some better overview is given
 * as well and the item can be better associated with it's in-game inventory
 * representation.  This function displays one such representation with 
 * the correct size to fit perfectly into the overview item row.
 * ---------------------------------------------------------------------- */
void
ShowRescaledItem ( int position , item* ShowItem )
{
  int PictureIndex;
  SDL_Surface* RescaledSurface;
  float RescaleFactor;
  SDL_Rect TargetRectangle;

  ItemRowRect . x = 50 ;
  ItemRowRect . y = 410;
  ItemRowRect . h = INITIAL_BLOCK_HEIGHT;
  ItemRowRect . w = INITIAL_BLOCK_WIDTH * SHOP_ROW_LENGTH ;

  TargetRectangle . x = ItemRowRect . x + position * INITIAL_BLOCK_WIDTH ;
  TargetRectangle . y = ItemRowRect . y ;

  PictureIndex = ItemMap [ ShowItem->type ] . picture_number ;

  if ( ( ItemImageList[ PictureIndex ] . inv_size . x == 1 ) &&
       ( ItemImageList[ PictureIndex ] . inv_size . y == 1 ) )
    RescaleFactor = 2.0 ;
  else if ( ItemImageList[ PictureIndex ] . inv_size . y == 3 ) 
    RescaleFactor = 2.0 / 3.0 ;
  else RescaleFactor = 1.0;

  RescaledSurface = zoomSurface ( ItemImageList[ PictureIndex ] . Surface , RescaleFactor , RescaleFactor , FALSE );
  
  SDL_BlitSurface( RescaledSurface , NULL , Screen , &TargetRectangle );

  SDL_FreeSurface ( RescaledSurface );
    
}; // void ShowRescaledItem ( int position , item* ShowItem )

/* ----------------------------------------------------------------------
 * This function does the item show when the user has selected item
 * show from the console menu.
 * ---------------------------------------------------------------------- */
int
GreatShopInterface ( int NumberOfItems , item* ShowPointerList[ MAX_ITEMS_IN_INVENTORY ] )
{
  int ItemType;
  int Displacement=0;
  bool finished = FALSE;
  static int WasPressed = FALSE ;
  // item* ShowPointerList[ MAX_ITEMS_IN_INVENTORY ];
  // int NumberOfItems;
  static int ItemIndex=0;
  int PasswordIndex = (-1) ;
  // int ClearanceIndex = (-1) ;
  int IdentifyAllowed = FALSE ;
  char* MenuTexts[ 10 ];
  int i;
  
  static int RowStart=0;
  int RowLength=SHOP_ROW_LENGTH;

  //--------------------
  // We add some secutiry against indexing beyond the
  // range of items given in the list.
  //
  while ( ItemIndex >= NumberOfItems ) ItemIndex -- ;
  if ( RowStart + RowLength  > NumberOfItems ) RowStart = 0 ;


  //--------------------
  // We initialize the text rectangle
  //
  Cons_Text_Rect . x = 258 ;
  Cons_Text_Rect . y = 89 ;
  Cons_Text_Rect . w = 346 ;
  Cons_Text_Rect . h = 282 ;

  // NumberOfItems = AssemblePointerListForItemShow ( &(ShowPointerList[0]), 0 );

  if ( ShowPointerList[0] == NULL )
    {
      MenuTexts[0]=" BACK ";
      MenuTexts[1]="";
      DoMenuSelection ( " YOU DONT HAVE ANYTHING IN INVENTORY, THAT COULD BE VIEWED. " , 
			MenuTexts, 1 , NULL , NULL );
      return (-1) ;
    }

  ItemType = ShowPointerList [ ItemIndex ] -> type ;

  Displacement = 0;

  while (!finished)
    {
      usleep ( 35 );

      //--------------------
      // We show all the info and the buttons that should be in this
      // interface...
      //
      ShowItemInfo ( ShowPointerList [ ItemIndex ] , Displacement , TRUE , "backgrounds/item_browser_shop.jpg" );

      for ( i = 0 ; i < RowLength ; i++ )
	{
	  ShowRescaledItem ( i , ShowPointerList [ i + RowStart ] );
	}

      ShowGenericButtonFromList ( LEFT_SHOP_BUTTON );
      ShowGenericButtonFromList ( RIGHT_SHOP_BUTTON );

      ShowGenericButtonFromList ( BUY_BUTTON );
      if ( ItemMap [ ShowPointerList [ ItemIndex ] -> type ] . item_group_together_in_inventory )
	{
	  ShowGenericButtonFromList ( BUY_10_BUTTON );
	  ShowGenericButtonFromList ( BUY_100_BUTTON );
	}

      SDL_Flip( Screen );

      //--------------------
      // Now we see if identification of the current item is allowed
      // or not.

      //
      IdentifyAllowed = FALSE ;
      if ( PasswordIndex >= 0 )
	{
	  if ( ! strcmp ( Me [ 0 ] . password_list [ PasswordIndex ] , "Tux Idenfity" ) )
	    {
	      IdentifyAllowed = TRUE ;
	    }
	}

      ItemType = ShowPointerList [ ItemIndex ] -> type ;

      if (SpacePressed() || EscapePressed() || axis_is_active )
	{
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
	  else if ( CursorIsOnButton( UP_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) && axis_is_active && !WasPressed )
	    {
	      MoveMenuPositionSound();
	      Displacement += FontHeight ( GetCurrentFont () );
	    }
	  else if ( CursorIsOnButton( DOWN_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) && axis_is_active && !WasPressed )
	    {
	      MoveMenuPositionSound();
	      // if (page > 0) page --;
	      Displacement -= FontHeight ( GetCurrentFont () );
	    }
	  else if ( CursorIsOnButton( ITEM_BROWSER_EXIT_BUTTON , GetMousePos_x ( ) + 16 , GetMousePos_y ( ) + 16 ) && axis_is_active && !WasPressed )
	    {
	      finished = TRUE;
	      while (SpacePressed() ||EscapePressed());
	    }
	  else if ( CursorIsOnButton( LEFT_SHOP_BUTTON , GetMousePos_x ( ) + 16 , GetMousePos_y ( ) + 16 ) && axis_is_active && !WasPressed )
	    {
	      if ( 0 < RowStart ) RowStart --;
	      MoveMenuPositionSound();
	      while (SpacePressed() ||EscapePressed());
	    }
	  else if ( CursorIsOnButton( RIGHT_SHOP_BUTTON , GetMousePos_x ( ) + 16 , GetMousePos_y ( ) + 16 ) && axis_is_active && !WasPressed )
	    {
	      if ( RowStart + RowLength < NumberOfItems ) RowStart ++;
	      MoveMenuPositionSound();
	      while (SpacePressed() ||EscapePressed());
	    }
	  else if ( ( ClickWasOntoItemRowPosition ( GetMousePos_x ( ) + 16 , GetMousePos_y ( ) + 16 ) >= 0 )&& axis_is_active && !WasPressed )
	    {
	      ItemIndex = RowStart + ClickWasOntoItemRowPosition ( GetMousePos_x ( ) + 16 , GetMousePos_y ( ) + 16 ) ;
	    }
	  else if ( CursorIsOnButton( BUY_BUTTON , GetMousePos_x ( ) + 16 , GetMousePos_y ( ) + 16 ) && axis_is_active && !WasPressed )
	    {
	      return ( ItemIndex );
	    }
	  else if ( CursorIsOnButton( BUY_10_BUTTON , GetMousePos_x ( ) + 16 , GetMousePos_y ( ) + 16 ) && axis_is_active && !WasPressed )
	    {
	      ShowPointerList [ ItemIndex ] -> multiplicity = 10 ;
	      return ( ItemIndex );
	    }
	  else if ( CursorIsOnButton( BUY_100_BUTTON , GetMousePos_x ( ) + 16 , GetMousePos_y ( ) + 16 ) && axis_is_active && !WasPressed )
	    {
	      ShowPointerList [ ItemIndex ] -> multiplicity = 100 ;
	      return ( ItemIndex );
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
	  if ( ItemType < Me[0].type) ItemType ++;
	}
      if (LeftPressed() )
	{
	  MoveMenuPositionSound();
	  while (LeftPressed());
	  if (ItemType > 0) ItemType --;
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
TryToBuyItem( item* BuyItem , int WithBacktalk )
{
  int x, y;
  int MenuPosition;
  int FreeIndex;
  char linebuf[1000];
  int i;

#define ANSWER_YES 1
#define ANSWER_NO 2

  char* MenuTexts[ 10 ];
  MenuTexts[0]="Yes";
  MenuTexts[1]="No";
  MenuTexts[2]="";

  DebugPrintf ( 0 , "\nTryToBuyItem (...):  function called." );

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
		  if ( WithBacktalk )
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
			  Me [ 0 ] . Inventory [ i ] . multiplicity += BuyItem -> multiplicity ;
			  Me[0].Gold -= CalculateItemPrice ( BuyItem , FALSE );
			  Play_Shop_ItemBoughtSound( );
			  //--------------------
			  // This is new.  I hope it's not dangerous.
			  DeleteItem ( BuyItem );
			  return;
			  break;
			case ANSWER_NO:
			  while (EnterPressed() || SpacePressed() );
			  return;
			  break;
			}
		    }
		  else
		    {
		      while (EnterPressed() || SpacePressed() );
		      Me [ 0 ] . Inventory [ i ] . multiplicity += BuyItem -> multiplicity ;
		      Me[0].Gold -= CalculateItemPrice ( BuyItem , FALSE );
		      Play_Shop_ItemBoughtSound( );
		      //--------------------
		      // This is new.  I hope it's not dangerous.
		      DeleteItem ( BuyItem );
		      return;
		    }
		}
	    }
	}
    }

  //--------------------
  // Now we must find out if there is an inventory position where we can put the
  // item in question.
  //

  for ( x = 0 ; x < INVENTORY_GRID_WIDTH ; x ++ )
    {
      for ( y = 0 ; y < INVENTORY_GRID_HEIGHT ; y ++ )
	{
	  if ( ItemCanBeDroppedInInv ( BuyItem->type , x , y ) )
	    {
	      while ( 1 )
		{
		  if ( WithBacktalk )
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
			  //--------------------
			  // This is new.  I hope it's not dangerous.
			  DeleteItem ( BuyItem );
			  return;
			  break;
			case ANSWER_NO:
			  while (EnterPressed() || SpacePressed() );
			  return;
			  break;
			}
		    }
		  else
		    {
		      while (EnterPressed() || SpacePressed() );
		      CopyItem( BuyItem , & ( Me[0].Inventory[ FreeIndex ] ) , TRUE );
		      Me[0].Inventory[ FreeIndex ].currently_held_in_hand = FALSE;
		      Me[0].Inventory[ FreeIndex ].inventory_position.x = x;
		      Me[0].Inventory[ FreeIndex ].inventory_position.y = y;
		      Me[0].Gold -= CalculateItemPrice ( BuyItem , FALSE );
		      Play_Shop_ItemBoughtSound( );
		      //--------------------
		      // This is new.  I hope it's not dangerous.
		      DeleteItem ( BuyItem );
		      return;
		    }
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

}; // void TryToBuyItem( item* BuyItem )

/* ----------------------------------------------------------------------
 * This is the menu, where you can buy basic items.
 * ---------------------------------------------------------------------- */
void
Buy_Basic_Items( int ForHealer , int ForceMagic )
{

#define FIXED_SHOP_INVENTORY TRUE
#define NUMBER_OF_ITEMS_IN_SHOP 17

  item SalesList[ MAX_ITEMS_IN_INVENTORY ];
  item* Buy_Pointer_List[ MAX_ITEMS_IN_INVENTORY ];
  int i;
  char DescriptionText[5000];
  int ItemSelected=0;

  int StandardShopInventory[ NUMBER_OF_ITEMS_IN_SHOP ] = 
    { 
      ITEM_SMALL_HEALTH_POTION,
      ITEM_SMALL_MANA_POTION,
      ITEM_MEDIUM_HEALTH_POTION,
      ITEM_MEDIUM_MANA_POTION,

      ITEM_SHORT_BOW,
      ITEM_HUNTERS_BOW,
      ITEM_LASER_AMMUNITION,

      ITEM_BUCKLER,
      ITEM_SMALL_SHIELD,

      ITEM_CLOAK,
      ITEM_ROBE,

      ITEM_DAGGER,
      ITEM_SHORT_SWORD,
      ITEM_SCIMITAR,
      ITEM_STAFF,

      ITEM_CAP,
      ITEM_SMALL_HELM,

      
    };

  //--------------------
  // First we make a selection of items, that can be considered 'basic'.
  // This selection depends of course on wheter the menu is generated
  // for the smith or for the healer.
  //
  for ( i = 0 ; i < NUMBER_OF_ITEMS_IN_SHOP ; i++ )
    {
      if ( ForHealer ) 
	{
	  //--------------------
	  // Here comes the random item selection for the healer
	  //
	  SalesList[ i ].type = 0 ; // something that can NOT be applied in combat
	  while ( ( ! ItemMap [ SalesList[ i ].type ].item_can_be_applied_in_combat ) ||
		  ( ! ItemMap [ SalesList[ i ].type ].item_can_be_bought_in_shop ) )
	    SalesList[ i ].type = MyRandom( Number_Of_Item_Types - 2 ) + 1;
	}
      else
	{
	  //--------------------
	  // Here comes the random item selection for the general shop
	  //
	  SalesList[ i ].type = 1 ; // something that can be applied in combat
	  while ( ItemMap [ SalesList[ i ].type ].item_can_be_applied_in_combat || 
		  ( ! ItemMap [ SalesList[ i ].type ].item_can_be_bought_in_shop ) ||
		  ( SalesList [ i ].type == ITEM_MONEY ) )
	    SalesList[ i ].type = MyRandom( Number_Of_Item_Types - 2 ) + 1;

	  if ( FIXED_SHOP_INVENTORY )
	    {
	      SalesList[ i ].type = StandardShopInventory [ i ];
	    }
	}

      SalesList[ i ].prefix_code = ( -1 );
      if ( ForceMagic ) SalesList[ i ].suffix_code = ( MyRandom(10) );
      else SalesList[ i ].suffix_code = ( -1 );

      FillInItemProperties( & ( SalesList[ i ] ) , TRUE , 0 );
      // if ( SalesList[ i ] . type == ITEM_LASER_AMMUNITION ) SalesList [ i ] . multiplicity = 100 ;
      SalesList[ i ].is_identified = TRUE;

      Buy_Pointer_List [ i ] = & ( SalesList[ i ] ) ;
    }
  //--------------------
  // Now me make sure that this list really gets terminated
  // as required by the item selection function.
  //
  Buy_Pointer_List [ i ] = NULL ; 
  


  //--------------------
  // Now here comes the new thing:  This will be a loop from now
  // on.  The buy and buy and buy until at one point we say 'BACK'
  //
  while ( ItemSelected != (-1) )
    {
      sprintf( DescriptionText , " I HAVE THESE ITEMS FOR SALE         YOUR GOLD:  %4ld" , Me[0].Gold );
      // ItemSelected = DoEquippmentListSelection( DescriptionText , Buy_Pointer_List , PRICING_FOR_BUY );
      // ItemSelected = DoEquippmentShowSelection ( DescriptionText , Buy_Pointer_List , PRICING_FOR_BUY );
      // ItemSelected = GreatItemShow ( NUMBER_OF_ITEMS_IN_SHOP , Buy_Pointer_List );
      ItemSelected = GreatShopInterface ( NUMBER_OF_ITEMS_IN_SHOP , Buy_Pointer_List );
      if ( ItemSelected != (-1) ) TryToBuyItem( Buy_Pointer_List[ ItemSelected ] , FALSE ) ;

      //--------------------
      // And since it can be assumed that the shop never runs
      // out of supply for a certain good, we can as well restore
      // the shop inventory list at this position.
      //
      if ( FIXED_SHOP_INVENTORY )
	{
	  for ( i = 0 ; i < NUMBER_OF_ITEMS_IN_SHOP ; i++ )
	    {
	      SalesList[ i ].type = StandardShopInventory [ i ];
	      SalesList[ i ].prefix_code = ( -1 );
	      SalesList[ i ].suffix_code = ( -1 );
	      FillInItemProperties( & ( SalesList[ i ] ) , TRUE , 0 );
	      // if ( SalesList[ i ] . type == ITEM_LASER_AMMUNITION ) SalesList [ i ] . multiplicity = 100 ;
	      Buy_Pointer_List [ i ] = & ( SalesList[ i ] ) ;
	    }
	  Buy_Pointer_List [ i ] = NULL ; 
	}
    }

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
      if ( ItemSelected != (-1) ) TryToSellItem( Sell_Pointer_List[ ItemSelected ] ) ;
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

