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
/*----------------------------------------------------------------------
 *
 * Desc: contains functions to update and draw the top status line with
 *	score and status etc...
 *
 *----------------------------------------------------------------------*/

#define _rahmen_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "proto.h"
#include "global.h"
#include "text.h"
#include "items.h"

extern char *InfluenceModeNames[];

// Dieses Array enth"alt die Datenstrukturen "uber alle im Spiel vorkommenden
// Anzeigebalken.
//
// { {point pos}, int len, int hgt, int oldval, int col }
//

bar AllBars[] = {
// Bar for the Energydisplay
  {{3, 52}, 300, 10, 0, FONT_RED},
// Bars for Shield1-4
  {{260, 5}, 50, 5, 0, FONT_GREEN},
  {{260, 12}, 50, 5, 0, FONT_GREEN},
  {{260, 19}, 50, 5, 0, FONT_GREEN},
  {{260, 26}, 50, 5, 0, FONT_GREEN}
};

void DrawBar (int, int, unsigned char *);

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
DrawBar (int BarCode, int Wert, unsigned char *Parameter_Screen)
{
  unsigned char *BarPoint = Parameter_Screen;
  int xlen;
  int barcol = 0;
  int i;

  DebugPrintf (2, "\nvoid DrawBar(...):  real function call confirmed.");

  if (Wert < 0)
    Wert = 0;
  BarPoint += AllBars[BarCode].pos.x + AllBars[BarCode].pos.y * SCREENLEN;

  if (InitBars)
    {
      for (i = 0; i < AllBars[BarCode].hgt; i++)
	{
	  memset (BarPoint, AllBars[BarCode].col, Wert);
	  memset (BarPoint + Wert, 0, abs (AllBars[BarCode].len - Wert));
	  BarPoint += SCREENLEN;
	}
      AllBars[BarCode].oldval = Wert;
      return;
    }

  if (Wert == AllBars[BarCode].oldval)
    {
      DebugPrintf (2, "\nvoid DrawBar(...):  end of function reached.");
      return;
    }

  xlen = abs (Wert - AllBars[BarCode].oldval);

  // Den Cursor an die Position stellen und rot oder schwarz einstellen.        
  if (Wert > AllBars[BarCode].oldval)
    {
      barcol = AllBars[BarCode].col;
      BarPoint += AllBars[BarCode].oldval;
    }
  else
    BarPoint += Wert;

  // Balken soweit zeichnen, wie die Ver"anderung ausmacht.
  for (i = 0; i < AllBars[BarCode].hgt; i++)
    {
      memset (BarPoint, barcol, xlen);
      BarPoint += SCREENLEN;
    }

  AllBars[BarCode].oldval = Wert;

  DebugPrintf (2, "\nvoid DrawBar(...):  end of function reached.");

} // void DrawBar(...)

/* ----------------------------------------------------------------------
 * This function writes the description of an item into the item description
 * string.
 * ---------------------------------------------------------------------- */
void 
GiveItemDescription ( char* ItemDescText , item* CurItem )
{
  char linebuf[1000];

  // --------------------
  // First we print out the item name.  That's simple.
  //
  strcpy( ItemDescText , ItemMap[ CurItem->type ].ItemName );
  strcat( ItemDescText , "\n" );

  // --------------------
  // If it's a weapon, then we give out the damage value of that weapon as well
  //
  if ( ItemMap[ CurItem->type ].item_can_be_installed_in_weapon_slot )
    {
      sprintf( linebuf , "Damage=%d" , ItemMap[ CurItem->type ].item_gun_damage );
      strcat( ItemDescText , linebuf );
    }

  // --------------------
  // If it's a drive, then we give out the maxspeed and accel values as well
  //
  if ( ItemMap[ CurItem->type ].item_can_be_installed_in_drive_slot )
    {
      sprintf( linebuf , "Maxspeed=%2.1f Accel=%2.1f" , 
	       ItemMap[ CurItem->type ].item_drive_maxspeed ,
	       ItemMap[ CurItem->type ].item_drive_accel );
      strcat( ItemDescText , linebuf );
    }

}; // void GiveItemDescription ( char* ItemDescText , item* CurItem )


/* -----------------------------------------------------------------
 * This function updates the top status bar. 
 * To save framerate on slow machines however it will only work
 * if it thinks that work needs to be done. 
 * You can however force update if you say so with a flag.
 * 
 * BANNER_FORCE_UPDATE=1: Forces the redrawing of the title bar
 * 
 * BANNER_DONT_TOUCH_TEXT=2: Prevents DisplayBanner from touching 
 * the text.
 * 
 * BANNER_NO_SDL_UPDATE=4: Prevents any SDL_Update calls.
 * 
 ----------------------------------------------------------------- */
void
DisplayBanner (const char* left, const char* right,  int flags )
{
  char dummy[80];
  char left_box [LEFT_TEXT_LEN + 10];
  char right_box[RIGHT_TEXT_LEN + 10];
  static char previous_left_box [LEFT_TEXT_LEN + 10]="NOUGHT";
  static char previous_right_box[RIGHT_TEXT_LEN + 10]="NOUGHT";
  int left_len, right_len;   /* the actualy string-lens */
  SDL_Rect Banner_Text_Rect;
  point CurPos;
  char ItemDescText[5000]="=== Nothing ===";
  grob_point inv_square;
  int InvIndex;
  int i;
  finepoint MapPositionOfMouse;

  Banner_Text_Rect.x = BANNER_TEXT_RECT_X;
  Banner_Text_Rect.y = BANNER_TEXT_RECT_Y;
  Banner_Text_Rect.w = BANNER_TEXT_RECT_W;
  Banner_Text_Rect.h = BANNER_TEXT_RECT_H;

  //--------------------
  // For testing purposes is bluntly insert the new banner element here:
  //
  CurPos.x = GetMousePos_x() + 16 ;
  CurPos.y = GetMousePos_y() + 16 ;
  SDL_SetClipRect( Screen , NULL );  // this unsets the clipping rectangle
  SDL_FillRect( Screen , &Banner_Text_Rect , 0 );

  //--------------------
  // In case some item is held in hand by the player, the situation is simple:
  // we merely need to draw this items description into the description field and
  // that's it.
  //
  if ( GetHeldItemCode() != (-1) )
    {
      strcpy( ItemDescText , ItemMap[ GetHeldItemCode() ].ItemName );
    }

  //--------------------
  // in the other case however, that no item is currently held in hand, we need to
  // work a little more:  we need to find out if the cursor is currently over some
  // inventory or other item and in case that's true, we need to give the 
  // description of this item.
  //
  else if ( GameConfig.Inventory_Visible )
    {
      //--------------------
      // Perhaps the cursor is over some item of the inventory?
      // let's check this case first.
      //
      if ( CursorIsInInventoryGrid( CurPos.x , CurPos.y ) )
	{
	  inv_square.x = GetInventorySquare_x( CurPos.x );
	  inv_square.y = GetInventorySquare_y( CurPos.y );
	  // DebugPrintf( 0 , "\nInv target x: %d." , inv_square.x );
	  // DebugPrintf( 0 , "\nInv target y: %d." , inv_square.y );
	  InvIndex = GetInventoryItemAt ( inv_square.x , inv_square.y );
	  // DebugPrintf( 0 , "\nInv Index targeted: %d." , InvIndex );
	  if ( InvIndex != (-1) )
	    {
	      GiveItemDescription ( ItemDescText , &(Me.Inventory[ InvIndex ]) );
	    }
	} 
      else if ( CursorIsInWeaponRect ( CurPos.x , CurPos.y ) )
	{
	  if ( Druidmap [ Me.type ].weapon_item.type )
	    GiveItemDescription ( ItemDescText , &(Druidmap[ Me.type ].weapon_item) );
	}
      else if ( CursorIsInDriveRect ( CurPos.x , CurPos.y ) )
	{
	  if ( Druidmap [ Me.type ].drive_item.type )
	   GiveItemDescription ( ItemDescText , &(Druidmap[ Me.type ].drive_item) );
	}
      else if ( CursorIsInShieldRect ( CurPos.x , CurPos.y ) )
	{
	   if ( Druidmap [ Me.type ].shield_item.type )
	   GiveItemDescription ( ItemDescText , &(Druidmap[ Me.type ].shield_item) );
	}
      else if ( CursorIsInArmourRect ( CurPos.x , CurPos.y ) )
	{
	   if ( Druidmap [ Me.type ].armour_item.type )
	   GiveItemDescription ( ItemDescText , &(Druidmap[ Me.type ].armour_item) );
	}
      else if ( CursorIsInAux1Rect ( CurPos.x , CurPos.y ) )
	{
	   if ( Druidmap [ Me.type ].aux1_item.type )
	   GiveItemDescription ( ItemDescText , &(Druidmap[ Me.type ].aux1_item) );
	}
      else if ( CursorIsInAux2Rect ( CurPos.x , CurPos.y ) )
	{
	   if ( Druidmap [ Me.type ].aux2_item.type )
	   GiveItemDescription ( ItemDescText , &(Druidmap[ Me.type ].aux2_item) );
	}
      else if ( CursorIsInSpecialRect ( CurPos.x , CurPos.y ) )
	{
	   if ( Druidmap [ Me.type ].special_item.type )
	   GiveItemDescription ( ItemDescText , &(Druidmap[ Me.type ].special_item) );
	}

    } // if nothing is 'held in hand' && inventory-screen visible

  if ( CursorIsInUserRect( CurPos.x , CurPos.y ) && ( CurLevel != NULL ) )
    {
      DebugPrintf( 0  , "\nCursor is in userfenster... --> see if hovering over an item...");
      MapPositionOfMouse.x = Me.pos.x + (CurPos.x - UserCenter_x) / (float) Block_Width;
      MapPositionOfMouse.y = Me.pos.y + (CurPos.y - UserCenter_y) / (float) Block_Height;
      DebugPrintf( 0  , "\nMouse in map at: %f %f." , MapPositionOfMouse.x , MapPositionOfMouse.y );
      for ( i = 0 ; i < MAX_ITEMS_PER_LEVEL ; i++ )
	{
	  if ( CurLevel->ItemList[ i ].type == (-1) ) continue;

	  if ( ( fabsf( MapPositionOfMouse.x - CurLevel->ItemList[ i ].pos.x ) < 0.5 ) &&
	       ( fabsf( MapPositionOfMouse.y - CurLevel->ItemList[ i ].pos.y ) < 0.5 ) )
	    {
	      GiveItemDescription ( ItemDescText , &(CurLevel->ItemList[ i ]) );
	      // strcpy( ItemDescText , ItemMap[ CurLevel->ItemList[ i ].type ].ItemName );
	    }
	}
    }

  SetCurrentFont( FPS_Display_BFont );
  DisplayText ( ItemDescText , 
		Banner_Text_Rect.x , Banner_Text_Rect.y , &Banner_Text_Rect );

  SDL_UpdateRect( Screen , Banner_Text_Rect.x , Banner_Text_Rect.y , 
		  Banner_Text_Rect.w , Banner_Text_Rect.h );
  //--------------------



  // --------------------
  // At first the text is prepared.  This can't hurt.
  // we will decide whether to dispaly it or not later...
  //

  if (left == NULL)       /* Left-DEFAULT: Mode */
    left = InfluenceModeNames[Me.status];

  if ( right == NULL )  /* Right-DEFAULT: Score */
    {
      sprintf ( dummy , "%ld" , ShowScore );
      right = dummy;
    }

  // Now fill in the text
  left_len = strlen (left);
  if( left_len > LEFT_TEXT_LEN )
    {
      printf ("\nWarning: String %s too long for Left Infoline!!",left);
      left_len = LEFT_TEXT_LEN;  /* too long, so we cut it! */
      Terminate(ERR);
    }
  right_len = strlen (right);
  if( right_len > RIGHT_TEXT_LEN )
    {
      printf ("\nWarning: String %s too long for Right Infoline!!", right);
      right_len = RIGHT_TEXT_LEN;  /* too long, so we cut it! */
      Terminate(ERR);
    }
  
  /* Now prepare the left/right text-boxes */
  memset (left_box,  ' ', LEFT_TEXT_LEN);  /* pad with spaces */
  memset (right_box, ' ', RIGHT_TEXT_LEN);  
  
  strncpy (left_box,  left, left_len);  /* this drops terminating \0 ! */
  strncpy (right_box, right, left_len);  /* this drops terminating \0 ! */
  
  left_box [LEFT_TEXT_LEN]  = '\0';     /* that's right, we want padding! */
  right_box[RIGHT_TEXT_LEN] = '\0';
  
  // --------------------
  // No we see if the screen need an update...

  if ( BannerIsDestroyed || 
       (flags & BANNER_FORCE_UPDATE ) || 
       (strcmp( left_box , previous_left_box )) || 
       (strcmp( right_box , previous_right_box )) )
    {
      // Redraw the whole background of the top status bar
      SDL_SetClipRect( Screen , NULL );  // this unsets the clipping rectangle
      SDL_BlitSurface( banner_pic, NULL, Screen , NULL);

      // Now the text should be ready and its
      // time to display it...
      if ( (strcmp( left_box , previous_left_box )) || 
	   (strcmp( right_box , previous_right_box )) ||
	   ( flags & BANNER_FORCE_UPDATE ) )
	{
	  PrintStringFont ( Screen, Para_BFont,
			    LEFT_INFO_X , LEFT_INFO_Y , left_box );
	  strcpy( previous_left_box , left_box );
	  PrintStringFont ( Screen, Para_BFont,
			    RIGHT_INFO_X , RIGHT_INFO_Y , right_box );
	  strcpy( previous_right_box , right_box );
	}

      // finally update the whole top status box
      if ( !(flags & BANNER_NO_SDL_UPDATE ) )
	SDL_UpdateRect( Screen, 0, 0, BANNER_WIDTH , BANNER_HEIGHT );
      BannerIsDestroyed=FALSE;
      return;
    } /* if */

} /* DisplayBanner() */


#undef _rahmen_c
