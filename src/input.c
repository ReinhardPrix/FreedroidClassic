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
 * This file contains (all) functions for keyboard and joystick handling
 * ---------------------------------------------------------------------- */

#define _input_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"

point CurrentMouseAbsPos;
int CurrentlyMouseRightPressed=0;
SDL_Event event;
int ShiftWasPressedInAddition=FALSE;
int CtrlWasPressedInAddition=FALSE;
int AltWasPressedInAddition=FALSE;
int CurrentlyEnterPressed=0;
int CurrentlySpacePressed=0;
int CurrentlyLeftPressed=0;
int CurrentlyRightPressed=0;
int CurrentlyUpPressed=0;
int CurrentlyDownPressed=0;
int Currently0Pressed=0;
int Currently1Pressed=0;
int Currently2Pressed=0;
int Currently3Pressed=0;
int Currently4Pressed=0;
int Currently5Pressed=0;
int Currently6Pressed=0;
int Currently7Pressed=0;
int Currently8Pressed=0;
int Currently9Pressed=0;
int CurrentlyAPressed=0;
int CurrentlyBPressed=0;
int CurrentlyCPressed=0;
int CurrentlyDPressed=0;
int CurrentlyEPressed=0;
int CurrentlyFPressed=0;
int CurrentlyGPressed=0;
int CurrentlyHPressed=0;
int CurrentlyIPressed=0;
int CurrentlyJPressed=0;
int CurrentlyKPressed=0;
int CurrentlyLPressed=0;
int CurrentlyMPressed=0;
int CurrentlyNPressed=0;
int CurrentlyOPressed=0;
int CurrentlyPPressed=0;
int CurrentlyQPressed=0;
int CurrentlyRPressed=0;
int CurrentlySPressed=0;
int CurrentlyTPressed=0;
int CurrentlyUPressed=0;
int CurrentlyVPressed=0;
int CurrentlyWPressed=0;
int CurrentlyXPressed=0;
int CurrentlyYPressed=0;
int CurrentlyZPressed=0;
int CurrentlyKP_PLUS_Pressed=0;
int CurrentlyKP_MINUS_Pressed=0;
int CurrentlyKP_MULTIPLY_Pressed=0;
int CurrentlyKP_DIVIDE_Pressed=0;
int CurrentlyKP_ENTER_Pressed=0;
int CurrentlyKP0Pressed=0;
int CurrentlyKP1Pressed=0;
int CurrentlyKP2Pressed=0;
int CurrentlyKP3Pressed=0;
int CurrentlyKP4Pressed=0;
int CurrentlyKP5Pressed=0;
int CurrentlyKP6Pressed=0;
int CurrentlyKP7Pressed=0;
int CurrentlyKP8Pressed=0;
int CurrentlyKP9Pressed=0;
int CurrentlyF1Pressed=0;
int CurrentlyF2Pressed=0;
int CurrentlyF3Pressed=0;
int CurrentlyF4Pressed=0;
int CurrentlyF5Pressed=0;
int CurrentlyF6Pressed=0;
int CurrentlyF7Pressed=0;
int CurrentlyF8Pressed=0;
int CurrentlyF9Pressed=0;
int CurrentlyF10Pressed=0;
int CurrentlyF11Pressed=0;
int CurrentlyF12Pressed=0;
int CurrentlyEscapePressed=0;
int CurrentlyBackspacePressed=0;


void 
UnsetAllKeys( void )
{
  CurrentlyEnterPressed=0;
  CurrentlySpacePressed=0;
  CurrentlyLeftPressed=0;
  CurrentlyRightPressed=0;
  CurrentlyUpPressed=0;
  CurrentlyDownPressed=0;
  Currently0Pressed=0;
  Currently1Pressed=0;
  Currently2Pressed=0;
  Currently3Pressed=0;
  Currently4Pressed=0;
  Currently5Pressed=0;
  Currently6Pressed=0;
  Currently7Pressed=0;
  Currently8Pressed=0;
  Currently9Pressed=0;
  CurrentlyAPressed=0;
  CurrentlyBPressed=0;
  CurrentlyCPressed=0;
  CurrentlyDPressed=0;
  CurrentlyEPressed=0;
  CurrentlyFPressed=0;
  CurrentlyGPressed=0;
  CurrentlyHPressed=0;
  CurrentlyIPressed=0;
  CurrentlyJPressed=0;
  CurrentlyKPressed=0;
  CurrentlyLPressed=0;
  CurrentlyMPressed=0;
  CurrentlyNPressed=0;
  CurrentlyOPressed=0;
  CurrentlyPPressed=0;
  CurrentlyQPressed=0;
  CurrentlyRPressed=0;
  CurrentlySPressed=0;
  CurrentlyTPressed=0;
  CurrentlyUPressed=0;
  CurrentlyVPressed=0;
  CurrentlyWPressed=0;
  CurrentlyXPressed=0;
  CurrentlyYPressed=0;
  CurrentlyZPressed=0;
  CurrentlyKP_PLUS_Pressed=0;
  CurrentlyKP_MINUS_Pressed=0;
  CurrentlyKP_MULTIPLY_Pressed=0;
  CurrentlyKP_DIVIDE_Pressed=0;
  CurrentlyKP_ENTER_Pressed=0;
  CurrentlyKP0Pressed=0;
  CurrentlyKP1Pressed=0;
  CurrentlyKP2Pressed=0;
  CurrentlyKP3Pressed=0;
  CurrentlyKP4Pressed=0;
  CurrentlyKP5Pressed=0;
  CurrentlyKP6Pressed=0;
  CurrentlyKP7Pressed=0;
  CurrentlyKP8Pressed=0;
  CurrentlyKP9Pressed=0;
  CurrentlyF1Pressed=0;
  CurrentlyF2Pressed=0;
  CurrentlyF3Pressed=0;
  CurrentlyF4Pressed=0;
  CurrentlyF5Pressed=0;
  CurrentlyF6Pressed=0;
  CurrentlyF7Pressed=0;
  CurrentlyF8Pressed=0;
  CurrentlyF9Pressed=0;
  CurrentlyF10Pressed=0;
  CurrentlyF11Pressed=0;
  CurrentlyF12Pressed=0;
  CurrentlyEscapePressed=0;
  CurrentlyBackspacePressed=0;
}; // void UnsetAllKeys( void )

// grob_point ItemSizeTable[ ALL_ITEMS ];

int 
GetMousePos_x(void)
{
  return( CurrentMouseAbsPos.x );
};

//
int 
GetMousePos_y(void)
{
  return( CurrentMouseAbsPos.y );
};



int sgn (int x)
{
  return (x ? ((x)/abs(x)) : 0);
}

void Init_Joy (void)
{
  int num_joy;

  if (SDL_InitSubSystem (SDL_INIT_JOYSTICK) == -1)
    {
      fprintf(stderr, "Couldn't initialize SDL-Joystick: %s\n",SDL_GetError());
      Terminate(ERR);
    } else
      DebugPrintf(1, "\nSDL Joystick initialisation successful.\n");


  DebugPrintf (1, " %d Joysticks found!\n", num_joy = SDL_NumJoysticks ());

  if (num_joy > 0)
    joy = SDL_JoystickOpen (0);

  if (joy)
    {
      DebugPrintf (1, "Identifier: %s\n", SDL_JoystickName (0));
      DebugPrintf (1, "Number of Axes: %d\n", joy_num_axes = SDL_JoystickNumAxes(joy));
      DebugPrintf (1, "Number of Buttons: %d\n", SDL_JoystickNumButtons(joy));

      /* aktivate Joystick event handling */
      SDL_JoystickEventState (SDL_ENABLE); 

    }
  else 
    joy = NULL;  /* signals that no yoystick is present */


  return;
}

/* ----------------------------------------------------------------------
 * This function does the reactions to keypresses of the player other
 * than pressing cursor keys.
 * ---------------------------------------------------------------------- */
void 
ReactToSpecialKeys(void)
{
  int i;
  int InvPos;
  static int IPressed_LastFrame;
  static int CPressed_LastFrame;
  char TempText[1000];
  grob_point Inv_Loc;
  int item_width;
  int item_height;

  if ( QPressed() ) /* user asked for quit */
    Terminate (OK);
  if ( DPressed() )
    Me.energy = 0;

  if ( Number0Pressed() )
    ApplyItemFromInventory( 0 );

  if ( Number1Pressed() )
    ApplyItemFromInventory( 1 );

  if ( Number2Pressed() )
    ApplyItemFromInventory( 2 );

  if ( Number3Pressed() )
    ApplyItemFromInventory( 3 );

  if ( Number4Pressed() )
    ApplyItemFromInventory( 4 );

  if ( Number5Pressed() )
    ApplyItemFromInventory( 5 );

  if ( Number6Pressed() )
    ApplyItemFromInventory( 6 );

  if ( Number7Pressed() )
    ApplyItemFromInventory( 7 );

  if ( Number8Pressed() )
    ApplyItemFromInventory( 8 );

  if ( Number9Pressed() )
    ApplyItemFromInventory( 9 );

  //--------------------
  // For debugging purposes, we introduce a key, that causes several 
  // values to be printed out.  This MUST be removed for the next release.
  //
  if ( MPressed() )
    {

      for ( i=0 ; i < MAX_STATEMENTS_PER_LEVEL ; i ++ )
	{
	  DebugPrintf( 1 , "\nPosX: %d PosY: %d Statement: %s" , CurLevel->StatementList[ i ].x ,
		       CurLevel->StatementList[ i ].x , CurLevel->StatementList[ i ].Statement_Text );
	}

      for ( i=0 ; i < ALL_ITEMS ; i ++ )
	{
	  if ( ItemMap[ i ].ItemName == NULL ) continue;
	  DebugPrintf( 0 , "\n\nItemName: %s " , ItemMap[ i ].ItemName );
	  DebugPrintf( 0 , "\nItemClass: %s " , ItemMap[ i ].ItemClass );
	  DebugPrintf( 0 , "\nitem_can_be_applied_in_combat: %d " , ItemMap[ i ].item_can_be_applied_in_combat );
	  DebugPrintf( 0 , "\nitem_can_be_installed_in_influ: %d " , ItemMap[ i ].item_can_be_installed_in_influ );
	  DebugPrintf( 0 , "\nitem_can_be_installed_in_weapon_slot: %d " , ItemMap[ i ].item_can_be_installed_in_weapon_slot );
	  DebugPrintf( 0 , "\nitem_can_be_installed_in_drive_slot: %d " , ItemMap[ i ].item_can_be_installed_in_drive_slot );
	  DebugPrintf( 0 , "\nitem_gun_damage: %d " , ItemMap[ i ].item_gun_damage );
	  DebugPrintf( 0 , "\nitem_gun_recharging_time: %f " , ItemMap[ i ].item_gun_recharging_time );

/*
  int New_Laser_Type_After_Installation;
  int New_Drive_Type_After_Installation;
  double energy_gain_uppon_application_in_combat;
  double item_weight;

  // How good is the item as drive???
  double item_drive_maxspeed;	// how fast can this item go used as the drive of the droid
  double item_drive_accel;	// as drive, how fast can you accelerate with this item

  // How good is the item as weapon???
  double item_gun_speed;			// speed of the bullet 
  int item_gun_blast;			// which blast does this bullet create 
  int item_gun_oneshotonly;	        // if this is set, there is only 1 shot 

  // Which picture to use for this item, when it's lying on the floor?
  int picture_number;
*/

	}


      for ( i=0 ; i < Number_Of_Droid_Types ; i ++ )
	{
	  if ( Druidmap[ i ].druidname == NULL ) continue;
	  DebugPrintf( 0 , "\n\ndruidname: %s " , Druidmap[ i ].druidname );
	  DebugPrintf( 0 , "\ndrive_item: %d (%s)" , Druidmap[ i ].drive_item.type , ItemMap[ Druidmap[ i ].drive_item.type ].ItemName );
	  DebugPrintf( 0 , "\nweapon_item: %d (%s) " , Druidmap[ i ].weapon_item.type , ItemMap[ Druidmap[ i ].weapon_item.type ].ItemName );
	}


      while ( MPressed() );
    }

  //--------------------
  // We assign the L key to turn on/off the quest log i.e. mission log
  //
  if ( LPressed() )
    {
      GameConfig.Mission_Log_Visible_Time = 0;
      GameConfig.Mission_Log_Visible = !GameConfig.Mission_Log_Visible;
    }

  //--------------------
  // We assign the T key to taking an item from the floor
  //
  if ( TPressed() )
    {
      InventorySize.x = 9;
      InventorySize.y = 6;
      
      for ( i = 0 ; i < MAX_ITEMS_PER_LEVEL ; i++ )
	{
	  if ( CurLevel->ItemList[ i ].type == (-1) ) continue;
	  if ( ( fabsf( Me.pos.x - CurLevel->ItemList[ i ].pos.x ) < 0.25 ) &&
	       ( fabsf( Me.pos.y - CurLevel->ItemList[ i ].pos.y ) < 0.25 ) )
	    break;
	}
      //--------------------
      // In case we found an item on the floor, we remove it from the floor
      // and add it to influs inventory
      //
      if ( i < MAX_ITEMS_PER_LEVEL )
	{
	  // find a free position in the inventory list
	  for ( InvPos = 0 ; InvPos < MAX_ITEMS_IN_INVENTORY ; InvPos++ )
	    {
	      if ( Me.Inventory [ InvPos ].type == (-1) ) break;
	    }


	  // find enough free squares in the inventory to fit
	  for ( Inv_Loc.y = 0; Inv_Loc.y < InventorySize.y - ItemImageList[ ItemMap[ CurLevel->ItemList[ i ].type ].picture_number ].inv_size.y + 1 ; Inv_Loc.y ++ )
	    {
	      for ( Inv_Loc.x = 0; Inv_Loc.x < InventorySize.x - ItemImageList[ ItemMap[ CurLevel->ItemList[ i ].type ].picture_number ].inv_size.x + 1 ; Inv_Loc.x ++ )
		{
		  
		  for ( item_height = 0 ; item_height < ItemImageList[ ItemMap[CurLevel->ItemList[ i ].type].picture_number ].inv_size.y ; item_height ++ )
		    {
		      for ( item_width = 0 ; item_width < ItemImageList[ ItemMap[CurLevel->ItemList[ i ].type ].picture_number ].inv_size.x ; item_width ++ )
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
	      sprintf( TempText , "Item taken: %s." , ItemMap[ CurLevel->ItemList[ i ].type ].ItemName );
	      Me.TextToBeDisplayed=MyMalloc( strlen( TempText ) + 1 );
	      strcpy ( Me.TextToBeDisplayed , TempText );

	      // We
	      Me.Inventory[ InvPos ].type = CurLevel->ItemList[ i ].type;

	      // We remove the item from the floor
	      CurLevel->ItemList[ i ].type = (-1);

	      // We make the sound of an item being taken
	      ItemTakenSound();
	    }
	}
    }

  //--------------------
  // We assign the I key to turn on/off the inventory log 
  //
  if ( IPressed() )
    {
      if ( !IPressed_LastFrame ) 
	{
	  GameConfig.Inventory_Visible_Time = 0;
	  GameConfig.Inventory_Visible = !GameConfig.Inventory_Visible;
	}

      IPressed_LastFrame = TRUE;
    }
  else
    {
      IPressed_LastFrame = FALSE;
    }

  //--------------------
  // We assign the C key to turn on/off the character screen
  //
  if ( CPressed() )
    {
      if ( !CPressed_LastFrame ) 
	{
	  GameConfig.CharacterScreen_Visible_Time = 0;
	  GameConfig.CharacterScreen_Visible = !GameConfig.CharacterScreen_Visible;
	}

      CPressed_LastFrame = TRUE;
    }
  else
    {
      CPressed_LastFrame = FALSE;
    }



  if ( GPressed () )
    {
      Me.TextToBeDisplayed="Hello!  Greetings to all other Freedom Fighters.";
      Me.TextVisibleTime=0;
    }
  
  // To debug the Debriefing() I added a function to add or subtract
  // a thousand points of score via numerical keyboard functions.
  // Activate this if you want to test that.  
  /*
  if ( KP0Pressed() )
    {
      while (KP0Pressed());
      RealScore-=1000;
    }
  if ( KP1Pressed() )
    {
      while (KP1Pressed());
      RealScore+=1000;
    }
  */
  
  if ( CPressed() && Alt_Was_Pressed()
       && Ctrl_Was_Pressed() && Shift_Was_Pressed() ) 
    Cheatmenu ();
  if ( EscapePressed() )
    EscapeMenu ();
  if ( PPressed () )
    Pause ();
  
  if ( UPressed () )
    {
      InitNewMissionList ( STANDARD_MISSION ) ;
      while (UPressed());
    }
  
} // void ReactToSpecialKeys(void)

int
Shift_Was_Pressed(void)
{
  return (ShiftWasPressedInAddition);  
}

int
Ctrl_Was_Pressed(void)
{
  return (CtrlWasPressedInAddition);  
}

int
Alt_Was_Pressed(void)
{
  return (AltWasPressedInAddition);  
}

int 
keyboard_update(void)
{
  Uint8 axis; 

  while( SDL_PollEvent( &event ) )
    {
      switch( event.type )
	{
	case SDL_QUIT:
	  printf("\n\nUser requestet Termination...\n\nTerminating...");
	  Terminate(0);
	  break;
	  /* Look for a keypress */
	case SDL_KEYDOWN:
	  // printf("\nSLD_KEYDOWN event detected...");
	  // fflush(stdout);

	  // Check for some additional modifiers and set flags accordingly
	  if ( event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT) )
	    ShiftWasPressedInAddition=TRUE;
	  else ShiftWasPressedInAddition=FALSE;

	  if ( event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL) )
	    CtrlWasPressedInAddition=TRUE;
	  else CtrlWasPressedInAddition=FALSE;

	  if ( event.key.keysym.mod & (KMOD_LALT | KMOD_RALT) )
	    AltWasPressedInAddition=TRUE;
	  else AltWasPressedInAddition=FALSE;

	  /* Check the SDLKey values */
	  switch( event.key.keysym.sym )
	    {
	    case SDLK_KP_PLUS:
	      CurrentlyKP_PLUS_Pressed=TRUE;
	      break;
	    case SDLK_KP_MINUS:
	      CurrentlyKP_MINUS_Pressed=TRUE;
	      break;
	    case SDLK_KP_MULTIPLY:
	      CurrentlyKP_MULTIPLY_Pressed=TRUE;
	      break;
	    case SDLK_KP_DIVIDE:
	      CurrentlyKP_DIVIDE_Pressed=TRUE;
	      break;
	    case SDLK_KP_ENTER:
	      CurrentlyKP_ENTER_Pressed=TRUE;
	      break;
	    case SDLK_0:
	      Currently0Pressed=TRUE;
	      break;
	    case SDLK_1:
	      Currently1Pressed=TRUE;
	      break;
	    case SDLK_2:
	      Currently2Pressed=TRUE;
	      break;
	    case SDLK_3:
	      Currently3Pressed=TRUE;
	      break;
	    case SDLK_4:
	      Currently4Pressed=TRUE;
	      break;
	    case SDLK_5:
	      Currently5Pressed=TRUE;
	      break;
	    case SDLK_6:
	      Currently6Pressed=TRUE;
	      break;
	    case SDLK_7:
	      Currently7Pressed=TRUE;
	      break;
	    case SDLK_8:
	      Currently8Pressed=TRUE;
	      break;
	    case SDLK_9:
	      Currently9Pressed=TRUE;
	      break;
	    case SDLK_KP0:
	      CurrentlyKP0Pressed=TRUE;
	      break;
	    case SDLK_KP1:
	      CurrentlyKP1Pressed=TRUE;
	      break;
	    case SDLK_KP2:
	      CurrentlyKP2Pressed=TRUE;
	      break;
	    case SDLK_KP3:
	      CurrentlyKP3Pressed=TRUE;
	      break;
	    case SDLK_KP4:
	      CurrentlyKP4Pressed=TRUE;
	      break;
	    case SDLK_KP5:
	      CurrentlyKP5Pressed=TRUE;
	      break;
	    case SDLK_KP6:
	      CurrentlyKP6Pressed=TRUE;
	      break;
	    case SDLK_KP7:
	      CurrentlyKP7Pressed=TRUE;
	      break;
	    case SDLK_KP8:
	      CurrentlyKP8Pressed=TRUE;
	      break;
	    case SDLK_KP9:
	      CurrentlyKP9Pressed=TRUE;
	      break;
	    case SDLK_F1:
	      CurrentlyF1Pressed=TRUE;
	      break;
	    case SDLK_F2:
	      CurrentlyF2Pressed=TRUE;
	      TakeScreenshot();
	      break;
	    case SDLK_F3:
	      CurrentlyF3Pressed=TRUE;
	      break;
	    case SDLK_F4:
	      CurrentlyF4Pressed=TRUE;
	      break;
	    case SDLK_F5:
	      CurrentlyF5Pressed=TRUE;
	      break;
	    case SDLK_F6:
	      CurrentlyF6Pressed=TRUE;
	      break;
	    case SDLK_F7:
	      CurrentlyF7Pressed=TRUE;
	      break;
	    case SDLK_F8:
	      CurrentlyF8Pressed=TRUE;
	      break;
	    case SDLK_F9:
	      CurrentlyF9Pressed=TRUE;
	      break;
	    case SDLK_F10:
	      CurrentlyF10Pressed=TRUE;
	      break;
	    case SDLK_F11:
	      CurrentlyF11Pressed=TRUE;
	      break;
	    case SDLK_F12:
	      CurrentlyF12Pressed=TRUE;
	      break;
	    case SDLK_BACKSPACE:
	      CurrentlyBackspacePressed=TRUE;
	      break;
	    case SDLK_LEFT:
	      CurrentlyLeftPressed=TRUE;
	      break;
	    case SDLK_RIGHT:
	      CurrentlyRightPressed=TRUE;
	      break;
	    case SDLK_UP:
	      CurrentlyUpPressed=TRUE;
	      break;
	    case SDLK_DOWN:
	      CurrentlyDownPressed=TRUE;
	      break;
	    case SDLK_SPACE:
	      CurrentlySpacePressed=TRUE;
	      break;
	    case SDLK_RETURN:
	      CurrentlyEnterPressed=TRUE;
	      break;
	    case SDLK_a:
	      CurrentlyAPressed=TRUE;
	      break;
	    case SDLK_b:
	      CurrentlyBPressed=TRUE;
	      break;
	    case SDLK_c:
	      CurrentlyCPressed=TRUE;
	      break;
	    case SDLK_d:
	      CurrentlyDPressed=TRUE;
	      break;
	    case SDLK_e:
	      CurrentlyEPressed=TRUE;
	      break;
	    case SDLK_f:
	      CurrentlyFPressed=TRUE;
	      break;
	    case SDLK_g:
	      CurrentlyGPressed=TRUE;
	      break;
	    case SDLK_h:
	      CurrentlyHPressed=TRUE;
	      break;
	    case SDLK_i:
	      CurrentlyIPressed=TRUE;
	      break;
	    case SDLK_j:
	      CurrentlyJPressed=TRUE;
	      break;
	    case SDLK_k:
	      CurrentlyKPressed=TRUE;
	      break;
	    case SDLK_l:
	      CurrentlyLPressed=TRUE;
	      break;
	    case SDLK_m:
	      CurrentlyMPressed=TRUE;
	      break;
	    case SDLK_n:
	      CurrentlyNPressed=TRUE;
	      break;
	    case SDLK_o:
	      CurrentlyOPressed=TRUE;
	      break;
	    case SDLK_p:
	      CurrentlyPPressed=TRUE;
	      break;
	    case SDLK_q:
	      CurrentlyQPressed=TRUE;
	      break;
	    case SDLK_r:
	      CurrentlyRPressed=TRUE;
	      break;
	    case SDLK_s:
	      CurrentlySPressed=TRUE;
	      break;
	    case SDLK_t:
	      CurrentlyTPressed=TRUE;
	      break;
	    case SDLK_u:
	      CurrentlyUPressed=TRUE;
	      break;
	    case SDLK_v:
	      CurrentlyVPressed=TRUE;
	      break;
	    case SDLK_w:
	      CurrentlyWPressed=TRUE;
	      break;
	    case SDLK_x:
	      CurrentlyXPressed=TRUE;
	      break;
	    case SDLK_y:
	      CurrentlyYPressed=TRUE;
	      break;
	    case SDLK_z:
	      CurrentlyZPressed=TRUE;
	      break;
	    case SDLK_ESCAPE:
	      CurrentlyEscapePressed=TRUE;
	      break;
	    default:
	      /*
		printf("\n\nUnhandled keystroke!! Terminating...\n\n");
		Terminate(ERR);
	      */
	      break;
	    }
	  break;
	  /* We must also use the SDL_KEYUP events to zero the x */
	  /* and y velocity variables. But we must also be       */
	  /* careful not to zero the velocities when we shouldn't*/
	case SDL_KEYUP:

	  // printf("\nSLD_KEYUP event detected...");
	  // fflush(stdout);

	  // Check for some additional modifiers and set flags accordingly
	  if ( event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT) )
	    ShiftWasPressedInAddition=TRUE;
	  else ShiftWasPressedInAddition=FALSE;

	  if ( event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL) )
	    CtrlWasPressedInAddition=TRUE;
	  else CtrlWasPressedInAddition=FALSE;

	  if ( event.key.keysym.mod & (KMOD_LALT | KMOD_RALT) )
	    AltWasPressedInAddition=TRUE;
	  else AltWasPressedInAddition=FALSE;

	  switch( event.key.keysym.sym )
	    {
	    case SDLK_KP_PLUS:
	      CurrentlyKP_PLUS_Pressed=FALSE;
	      break;
	    case SDLK_KP_MINUS:
	      CurrentlyKP_MINUS_Pressed=FALSE;
	      break;
	    case SDLK_KP_MULTIPLY:
	      CurrentlyKP_MULTIPLY_Pressed=FALSE;
	      break;
	    case SDLK_KP_DIVIDE:
	      CurrentlyKP_DIVIDE_Pressed=FALSE;
	      break;
	    case SDLK_KP_ENTER:
	      CurrentlyKP_ENTER_Pressed=FALSE;
	      break;
	    case SDLK_0:
	      Currently0Pressed=FALSE;
	      break;
	    case SDLK_1:
	      Currently1Pressed=FALSE;
	      break;
	    case SDLK_2:
	      Currently2Pressed=FALSE;
	      break;
	    case SDLK_3:
	      Currently3Pressed=FALSE;
	      break;
	    case SDLK_4:
	      Currently4Pressed=FALSE;
	      break;
	    case SDLK_5:
	      Currently5Pressed=FALSE;
	      break;
	    case SDLK_6:
	      Currently6Pressed=FALSE;
	      break;
	    case SDLK_7:
	      Currently7Pressed=FALSE;
	      break;
	    case SDLK_8:
	      Currently8Pressed=FALSE;
	      break;
	    case SDLK_9:
	      Currently9Pressed=FALSE;
	      break;
	    case SDLK_KP0:
	      CurrentlyKP0Pressed=FALSE;
	      break;
	    case SDLK_KP1:
	      CurrentlyKP1Pressed=FALSE;
	      break;
	    case SDLK_KP2:
	      CurrentlyKP2Pressed=FALSE;
	      break;
	    case SDLK_KP3:
	      CurrentlyKP3Pressed=FALSE;
	      break;
	    case SDLK_KP4:
	      CurrentlyKP4Pressed=FALSE;
	      break;
	    case SDLK_KP5:
	      CurrentlyKP5Pressed=FALSE;
	      break;
	    case SDLK_KP6:
	      CurrentlyKP6Pressed=FALSE;
	      break;
	    case SDLK_KP7:
	      CurrentlyKP7Pressed=FALSE;
	      break;
	    case SDLK_KP8:
	      CurrentlyKP8Pressed=FALSE;
	      break;
	    case SDLK_KP9:
	      CurrentlyKP9Pressed=FALSE;
	      break;
	    case SDLK_F1:
	      CurrentlyF1Pressed=FALSE;
	      break;
	    case SDLK_F2:
	      CurrentlyF2Pressed=FALSE;
	      break;
	    case SDLK_F3:
	      CurrentlyF3Pressed=FALSE;
	      break;
	    case SDLK_F4:
	      CurrentlyF4Pressed=FALSE;
	      break;
	    case SDLK_F5:
	      CurrentlyF5Pressed=FALSE;
	      break;
	    case SDLK_F6:
	      CurrentlyF6Pressed=FALSE;
	      break;
	    case SDLK_F7:
	      CurrentlyF7Pressed=FALSE;
	      break;
	    case SDLK_F8:
	      CurrentlyF8Pressed=FALSE;
	      break;
	    case SDLK_F9:
	      CurrentlyF9Pressed=FALSE;
	      break;
	    case SDLK_F10:
	      CurrentlyF10Pressed=FALSE;
	      break;
	    case SDLK_F11:
	      CurrentlyF11Pressed=FALSE;
	      break;
	    case SDLK_F12:
	      CurrentlyF12Pressed=FALSE;
	      break;
	    case SDLK_BACKSPACE:
	      CurrentlyBackspacePressed=FALSE;
	      break;
	    case SDLK_LEFT:
	      CurrentlyLeftPressed=FALSE;
	      break;
	    case SDLK_RIGHT:
	      CurrentlyRightPressed=FALSE;
	      break;
	    case SDLK_UP:
	      CurrentlyUpPressed=FALSE;
	      break;
	    case SDLK_DOWN:
	      CurrentlyDownPressed=FALSE;
	      break;
	    case SDLK_SPACE:
	      CurrentlySpacePressed=FALSE;
	      break;
	    case SDLK_RETURN:
	      CurrentlyEnterPressed=FALSE;
	      break;
	    case SDLK_a:
	      CurrentlyAPressed=FALSE;
	      break;
	    case SDLK_b:
	      CurrentlyBPressed=FALSE;
	      break;
	    case SDLK_c:
	      CurrentlyCPressed=FALSE;
	      break;
	    case SDLK_d:
	      CurrentlyDPressed=FALSE;
	      break;
	    case SDLK_e:
	      CurrentlyEPressed=FALSE;
	      break;
	    case SDLK_f:
	      CurrentlyFPressed=FALSE;
	      break;
	    case SDLK_g:
	      CurrentlyGPressed=FALSE;
	      break;
	    case SDLK_h:
	      CurrentlyHPressed=FALSE;
	      break;
	    case SDLK_i:
	      CurrentlyIPressed=FALSE;
	      break;
	    case SDLK_j:
	      CurrentlyJPressed=FALSE;
	      break;
	    case SDLK_k:
	      CurrentlyKPressed=FALSE;
	      break;
	    case SDLK_l:
	      CurrentlyLPressed=FALSE;
	      break;
	    case SDLK_m:
	      CurrentlyMPressed=FALSE;
	      break;
	    case SDLK_n:
	      CurrentlyNPressed=FALSE;
	      break;
	    case SDLK_o:
	      CurrentlyOPressed=FALSE;
	      break;
	    case SDLK_p:
	      CurrentlyPPressed=FALSE;
	      break;
	    case SDLK_q:
	      CurrentlyQPressed=FALSE;
	      break;
	    case SDLK_r:
	      CurrentlyRPressed=FALSE;
	      break;
	    case SDLK_s:
	      CurrentlySPressed=FALSE;
	      break;
	    case SDLK_t:
	      CurrentlyTPressed=FALSE;
	      break;
	    case SDLK_u:
	      CurrentlyUPressed=FALSE;
	      break;
	    case SDLK_v:
	      CurrentlyVPressed=FALSE;
	      break;
	    case SDLK_w:
	      CurrentlyWPressed=FALSE;
	      break;
	    case SDLK_x:
	      CurrentlyXPressed=FALSE;
	      break;
	    case SDLK_y:
	      CurrentlyYPressed=FALSE;
	      break;
	    case SDLK_z:
	      CurrentlyZPressed=FALSE;
	      break;
	    case SDLK_ESCAPE:
	      CurrentlyEscapePressed=FALSE;
	      break;
	    default:
	      break;
	    }
	  break;

	case SDL_JOYAXISMOTION:
	  axis = event.jaxis.axis;
	  if (axis == 0 || ((joy_num_axes >= 5) && (axis == 3)) ) /* x-axis */
	    {
	      input_axis.x = event.jaxis.value;

	      if (event.jaxis.value > joy_sensitivity*1000)   /* about half tilted */
		{
		  CurrentlyRightPressed = TRUE;
		  CurrentlyLeftPressed = FALSE;
		}
	      else if (event.jaxis.value <  -joy_sensitivity*1000)
		{
		  CurrentlyLeftPressed = TRUE;
		  CurrentlyRightPressed = FALSE;
		}
	      else
		{
		  CurrentlyLeftPressed = FALSE;
		  CurrentlyRightPressed= FALSE;
		}
	    }
	  else if ((axis == 1) || ((joy_num_axes >=5) && (axis == 4))) /* y-axis */
	    {
	      input_axis.y = event.jaxis.value;

	      if (event.jaxis.value > joy_sensitivity*1000)  
		{
		  CurrentlyDownPressed = TRUE;
		  CurrentlyUpPressed = FALSE;
		}
	      else if (event.jaxis.value < -joy_sensitivity*1000)
		{
		  CurrentlyUpPressed = TRUE;
		  CurrentlyDownPressed = FALSE;
		}
	      else
		{
		  CurrentlyUpPressed = FALSE;
		  CurrentlyDownPressed= FALSE;
		}
	    }
		
	  break;
	  
	case SDL_JOYBUTTONDOWN:
	  CurrentlySpacePressed = TRUE;
	  axis_is_active = TRUE;
	  break;

	case SDL_JOYBUTTONUP:
	  CurrentlySpacePressed = FALSE;
	  axis_is_active = FALSE;
	  break;

	case SDL_MOUSEMOTION:
	  if (mouse_control)
	    {
	      //
	      // Since the new mouse cursor does have it's tip at the top left corner
	      // of the mouse cursor, but rather in the center of the 32x32 pixel mouse
	      // cursor, we need to correct the given axis a little (16 pixels) bit.
	      //
	      input_axis.x = event.button.x - UserCenter_x + 16; 
	      input_axis.y = event.button.y - UserCenter_y + 16; 	  
	      CurrentMouseAbsPos.x = event.button.x;
	      CurrentMouseAbsPos.y = event.button.y;
	    }
	  break;
	  
	  /* Mouse control */
	case SDL_MOUSEBUTTONDOWN:
	  if (event.button.button == SDL_BUTTON_LEFT)
	    {
	      CurrentlySpacePressed = TRUE;
	      axis_is_active = TRUE;
	    }

	  if (event.button.button == SDL_BUTTON_RIGHT)
	    CurrentlyMouseRightPressed = TRUE;
	  
	  break;

        case SDL_MOUSEBUTTONUP:
	  if (event.button.button == SDL_BUTTON_LEFT)
	    {
	      CurrentlySpacePressed = FALSE;
	      axis_is_active = FALSE;
	    }

	  if (event.button.button == SDL_BUTTON_RIGHT)
	    CurrentlyMouseRightPressed = FALSE;
	
	  break;

 	default:
 	  break;
 	}

    }

  return 0;
}

/*-----------------------------------------------------------------
 * Desc: should do roughly what getchar() does, but in raw 
 * 	 (SLD) keyboard mode. 
 * 
 * Return: the (SDLKey) of the next key-pressed event cast to (int)
 *
 *-----------------------------------------------------------------*/
int
getchar_raw (void)
{
  SDL_Event event;
  int Returnkey;

  //  keyboard_update ();   /* treat all pending keyboard-events */

  while (1)
    {
      SDL_WaitEvent (&event);    /* wait for next event */
      
      if (event.type == SDL_KEYDOWN)
	{
	  /* 
	   * here we use the fact that, I cite from SDL_keyboard.h:
	   * "The keyboard syms have been cleverly chosen to map to ASCII"
	   * ... I hope that this design feature is portable, and durable ;)  
	   */
	  Returnkey = (int) event.key.keysym.sym;
	  if ( event.key.keysym.mod & KMOD_SHIFT ) Returnkey = toupper( (int)event.key.keysym.sym );
	  return ( Returnkey );
	}
      else if (event.type == SDL_KEYUP)
	{

	  UnsetAllKeys ();  // we don't want to get any 'stuck' keys, and after entering
	                    // text, I'm sure no key still has to be pressed...

	  // do nothing here, but don't push this event either
	}
      else
	{
	  SDL_PushEvent (&event);  /* put this event back into the queue */
	  keyboard_update ();  /* and treat it the usual way */
	  continue;
	}

    } /* while(1) */

} /* getchar_raw() */


int 
KP_PLUS_Pressed (void)
{
  keyboard_update();
  return CurrentlyKP_PLUS_Pressed;
}

int 
KP_MULTIPLY_Pressed (void)
{
  keyboard_update();
  return CurrentlyKP_MULTIPLY_Pressed;
}

int 
KP_MINUS_Pressed (void)
{
  keyboard_update();
  return CurrentlyKP_MINUS_Pressed;
}

int 
KP_DIVIDE_Pressed (void)
{
  keyboard_update();
  return CurrentlyKP_DIVIDE_Pressed;
}

int 
KP_ENTER_Pressed (void)
{
  keyboard_update();
  return CurrentlyKP_ENTER_Pressed;
}

int
Number0Pressed (void)
{
  keyboard_update ();
  return Currently0Pressed;
}				// int KP0Pressed(void)

int
Number1Pressed (void)
{
  keyboard_update ();
  return Currently1Pressed;
}				// int KP1Pressed(void)

int
Number2Pressed (void)
{
  keyboard_update ();
  return Currently2Pressed;
}				// int KP2Pressed(void)

int
Number3Pressed (void)
{
  keyboard_update ();
  return Currently3Pressed;
}				// int KP3Pressed(void)

int
Number4Pressed (void)
{
  keyboard_update ();
  return Currently4Pressed;
}				// int KP4Pressed(void)

int
Number5Pressed (void)
{
  keyboard_update ();
  return Currently5Pressed;
}				// int KP5Pressed(void)

int
Number6Pressed (void)
{
  keyboard_update ();
  return Currently6Pressed;
}				// int KP6Pressed(void)

int
Number7Pressed (void)
{
  keyboard_update ();
  return Currently7Pressed;
}				// int KP7Pressed(void)

int
Number8Pressed (void)
{
  keyboard_update ();
  return Currently8Pressed;
}				// int KP8Pressed(void)

int
Number9Pressed (void)
{
  keyboard_update ();
  return Currently9Pressed;
}				// int KP9Pressed(void)

int
KP0Pressed (void)
{
  keyboard_update ();
  return CurrentlyKP0Pressed;
}				// int KP0Pressed(void)

int
KP1Pressed (void)
{
  keyboard_update ();
  return CurrentlyKP1Pressed;
}				// int KP1Pressed(void)

int
KP2Pressed (void)
{
  keyboard_update ();
  return CurrentlyKP2Pressed;
}				// int KP2Pressed(void)

int
KP3Pressed (void)
{
  keyboard_update ();
  return CurrentlyKP3Pressed;
}				// int KP3Pressed(void)

int
KP4Pressed (void)
{
  keyboard_update ();
  return CurrentlyKP4Pressed;
}				// int KP4Pressed(void)

int
KP5Pressed (void)
{
  keyboard_update ();
  return CurrentlyKP5Pressed;
}				// int KP5Pressed(void)

int
KP6Pressed (void)
{
  keyboard_update ();
  return CurrentlyKP6Pressed;
}				// int KP6Pressed(void)

int
KP7Pressed (void)
{
  keyboard_update ();
  return CurrentlyKP7Pressed;
}				// int KP7Pressed(void)

int
KP8Pressed (void)
{
  keyboard_update ();
  return CurrentlyKP8Pressed;
}				// int KP8Pressed(void)

int
KP9Pressed (void)
{
  keyboard_update ();
  return CurrentlyKP9Pressed;
}				// int KP9Pressed(void)

int
F1Pressed (void)
{
  keyboard_update ();
  return CurrentlyF1Pressed;
}				// int F1Pressed(void)

int
F2Pressed (void)
{
  keyboard_update ();
  return CurrentlyF2Pressed;
}				// int F2Pressed(void)

int
F3Pressed (void)
{
  keyboard_update ();
  return CurrentlyF3Pressed;
}				// int F3Pressed(void)

int
F4Pressed (void)
{
  keyboard_update ();
  return CurrentlyF4Pressed;
}				// int F4Pressed(void)

int
F5Pressed (void)
{
  keyboard_update ();
  return CurrentlyF5Pressed;
}				// int F5Pressed(void)

int
F6Pressed (void)
{
  keyboard_update ();
  return CurrentlyF6Pressed;
}				// int F6Pressed(void)

int
F7Pressed (void)
{
  keyboard_update ();
  return CurrentlyF7Pressed;
}				// int F7Pressed(void)

int
F8Pressed (void)
{
  keyboard_update ();
  return CurrentlyF8Pressed;
}				// int F8Pressed(void)

int
F9Pressed (void)
{
  keyboard_update ();
  return CurrentlyF9Pressed;
}				// int F9Pressed(void)

int
F10Pressed (void)
{
  keyboard_update ();
  return CurrentlyF10Pressed;
}				// int F10Pressed(void)

int
F11Pressed (void)
{
  keyboard_update ();
  return CurrentlyF11Pressed;
}				// int F11Pressed(void)

int
F12Pressed (void)
{
  keyboard_update ();
  return CurrentlyF12Pressed;
}				// int F12Pressed(void)

int
LeftPressed (void)
{
  keyboard_update ();
  return CurrentlyLeftPressed;
}				// int LeftPressed(void)

int
RightPressed (void)
{
  keyboard_update ();
  return CurrentlyRightPressed;
}				// int RightPressed(void)

int
UpPressed (void)
{
  keyboard_update ();
  return CurrentlyUpPressed;
}				// int UpPressed(void)

int
DownPressed (void)
{
  keyboard_update ();
  return CurrentlyDownPressed;
}				// int DownPressed(void)

int
SpacePressed (void)
{
  keyboard_update ();
  return CurrentlySpacePressed;
}				// int SpacePressed(void)

int
EnterPressed (void)
{
  keyboard_update ();
  return CurrentlyEnterPressed;
}				// int SpacePressed(void)

int
BackspacePressed (void)
{
  keyboard_update ();
  return CurrentlyBackspacePressed;
}				// int SpacePressed(void)

int
APressed (void)
{
  keyboard_update ();
  return CurrentlyAPressed;
}				// int PPressed(void)

int
BPressed (void)
{
  keyboard_update ();
  return CurrentlyBPressed;
}				// int PPressed(void)

int
EPressed (void)
{
  keyboard_update ();
  return CurrentlyEPressed;
}				// int PPressed(void)

int
FPressed (void)
{
  keyboard_update ();
  return CurrentlyFPressed;
}				// int PPressed(void)

int
GPressed (void)
{
  keyboard_update ();
  return CurrentlyGPressed;
}				// int PPressed(void)

int
HPressed (void)
{
  keyboard_update ();
  return CurrentlyHPressed;
}				// int PPressed(void)

int
JPressed (void)
{
  keyboard_update ();
  return CurrentlyJPressed;
}				// int PPressed(void)

int
KPressed (void)
{
  keyboard_update ();
  return CurrentlyKPressed;
}				// int PPressed(void)

int
MPressed (void)
{
  keyboard_update ();
  return CurrentlyMPressed;
}				// int PPressed(void)

int
NPressed (void)
{
  keyboard_update ();
  return CurrentlyNPressed;
}				// int PPressed(void)

int
OPressed (void)
{
  keyboard_update ();
  return CurrentlyOPressed;
}				// int PPressed(void)

int
PPressed (void)
{
  keyboard_update ();
  return CurrentlyPPressed;
}				// int PPressed(void)

int
RPressed (void)
{
  keyboard_update ();
  return CurrentlyRPressed;
}				// int PPressed(void)

int
SPressed (void)
{
  keyboard_update ();
  return CurrentlySPressed;
}				// int PPressed(void)

int
TPressed (void)
{
  keyboard_update ();
  return CurrentlyTPressed;
}				// int PPressed(void)

int
QPressed (void)
{
  keyboard_update ();
  return CurrentlyQPressed;
}				// int QPressed(void)

int
WPressed (void)
{
  keyboard_update ();
  return CurrentlyWPressed;
}				// int WPressed(void)

int
DPressed (void)
{
  keyboard_update ();
  return CurrentlyDPressed;
}				// int WPressed(void)

int
LPressed (void)
{
  keyboard_update ();
  return CurrentlyLPressed;
}				// int WPressed(void)

int
IPressed (void)
{
  keyboard_update ();
  return CurrentlyIPressed;
}				// int WPressed(void)

int
VPressed (void)
{
  keyboard_update ();
  return CurrentlyVPressed;
}				// int WPressed(void)

int
CPressed (void)
{
  keyboard_update ();
  return CurrentlyCPressed;
}				// int WPressed(void)

int
UPressed (void)
{
  keyboard_update ();
  return CurrentlyUPressed;
}				// int WPressed(void)

int
XPressed (void)
{
  keyboard_update ();
  return CurrentlyXPressed;
}				// int WPressed(void)

int
YPressed (void)
{
  keyboard_update ();
  return CurrentlyYPressed;
}				// int WPressed(void)

int
ZPressed (void)
{
  keyboard_update ();
  return CurrentlyZPressed;
}				// int WPressed(void)

int
EscapePressed (void)
{
  keyboard_update ();
  return CurrentlyEscapePressed;
}				// int WPressed(void)

/*@Function============================================================
  @Desc: Diese Funktion ermittelt, ob irgend eine Richtungstaste gedrueckt ist
  
  @Ret: wenn eine Richtungstaste gedrueckt ist FALSE
  ansonsten TRUE 
* $Function----------------------------------------------------------*/

int
NoDirectionPressed (void)
{
  if ( (axis_is_active && (input_axis.x || input_axis.y)) ||
      DownPressed () || UpPressed() || LeftPressed() || RightPressed() )
    return ( FALSE );
  else
    return ( TRUE );
} // int NoDirectionPressed(void)


int
MouseRightPressed(void)
{
  keyboard_update();
  return CurrentlyMouseRightPressed;
}





#undef _intput_c
