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
 * This file contains all the console and lift functions (mostly)
 * ---------------------------------------------------------------------- */

/*
 * This file has been checked for remains of german in the documentation.
 * They should be all out by now, and if you still find any, please do not
 * hesitate to remove them.
 */

#define _ship_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"
#include "text.h"
#include "ship.h"
#include "SDL_rotozoom.h"

enum 
  {
    NO_FUNCTION,
    UNLOCK_FUNCTION,
    GUNON_FUNCTION,
    GUNOFF_FUNCTION
  };


int NoKeyPressed (void);
void GreatItemShow (void);
void show_item_info ( item* ShowItem , int page , char ShowArrows );


// EXTERN SDL_Surface *console_pic;
SDL_Surface *console_pic = NULL ;

char *EmailText1 = "Freedroid Installation 
======================

[ 0) if you have downloaded your source from the cvs-archive 
   you have to start by typing `./autogen.sh']

1) autogen will run `./configure' - you don't need to
2) type `make' to compile the package

3)a) type `make install'. By default this will try to install in 
   /usr/local/, in which case you have to be root to do that.
   You can instead specify `--prefix=<yourdir>' to `configure', in
   which case `make install' will install in <yourdir>.

ALTERNATIVELY:

3)b) you can run the game locally by typing: 
   `cd src' followed by `./freedroid'

If you don't have sound or SDL-sound installed, you might have to run
freedroid with the command-line option `-q'.

Have fun!
Reinhard

If you have problems with the compilation/installation, please send me an email:
rprix@users.sourceforge.net
or report bugs on the sourceforge website:
http://sourceforge.net/projects/freedroid/";

char *EmailText2 = "
 The freedroid project is looking for a content writer, who could set up additional characters, plot and multiple-choice styled dialogs for the freedroid RPG branch.

The story is a science-fiction one and in short goes like this:
The MS has enslaved a far distant universe. The Tux, not natively belonging into this world arrives accidentially during a magnetic storm. He finds himself in a former resistance camp, having lost his memory. He should try to get in contact with the resistance to help him return to his native home plane of existence. (or somewhat like this). Adventures and missions will involve fighting the MS machines but also talking to other characters and helping the resistance (or the MS?).

Not much expertise needed, but perhaps experience with (computer) role playing games. Some familiarity with C would help, since the dialogs will be integrated directly into the C code unless we decide to take them out into separate structures and files. Maybe some idea of open source and the MS in the real world would also be good.

If you are interested, please email to
freedroid-discussion@lists.sourceforge.net. (You can expect approval of your message shortly.) You also might check out the cvs source code directly. The latest release of the rpg branch does not contain appropriate examples of what the dialogs look like.

";

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
PutPasswordButtonsAndPassword ( int PasswordIndex )
{
  ShowGenericButtonFromList ( MAP_PASSWORDMIDDLE_BUTTON );
  ShowGenericButtonFromList ( MAP_PASSWORDRIGHT_BUTTON );
  ShowGenericButtonFromList ( MAP_PASSWORDLEFT_BUTTON );
  if ( PasswordIndex >= 0 )
    {
      PutString ( Screen , 440 , 440 , Me [ 0 ] . password_list [ PasswordIndex ] );      
    }
  else
    {
      PutString ( Screen , 440 , 440 , "-------" );      
    }
}; // void PutPasswordButtonsAndPassword ( int ClearanceIndex )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
PutSecurityButtonsAndClearance ( int ClearanceIndex )
{
  ShowGenericButtonFromList ( MAP_SECURITYMIDDLE_BUTTON );
  ShowGenericButtonFromList ( MAP_SECURITYRIGHT_BUTTON );
  ShowGenericButtonFromList ( MAP_SECURITYLEFT_BUTTON );
  
  if ( ClearanceIndex >= 0 )
    {
      PutString ( Screen , 210 , 440 , Druidmap [ Me [ 0 ] . clearance_list [ ClearanceIndex ] ] . druidname );      
    }
  else
    {
      PutString ( Screen , 210 , 440 , "---" );      
    }
}; // void PutSecurityButtonsAndClearance ( int ClearanceIndex )



/*-----------------------------------------------------------------
 * This function does all the work when we enter a lift
 *-----------------------------------------------------------------*/
void
EnterLift (void)
{
  int i;
  int curLevel;
  int curLift, upLift, downLift, liftrow;

  DebugPrintf (2, "\nvoid EnterLiftator(void): Function call confirmed.");

  //--------------------
  // Prevent distortion of framerate by the delay coming from 
  // the time spend in the menu. 
  //
  Activate_Conservative_Frame_Computation();

  //--------------------
  // Prevent the influ from coming out of the lift in transfer mode
  // by turning off transfer mode as soon as the influ enters the lift 
  Me[0].status= ELEVATOR;

  curLevel = CurLevel->levelnum;

  if ((curLift = GetCurrentLift ()) == -1)
    {
      printf ("Lift out of order, I'm so sorry !");
      return;
    }

  EnterLiftSound ();

  upLift = curShip.AllLifts[curLift].up;
  downLift = curShip.AllLifts[curLift].down;

  liftrow = curShip.AllLifts[curLift].lift_row;

  ShowLifts (curLevel, liftrow);

  while (SpacePressed ()) ; // wait, so the loop afterwards is not exited immediately


  while (!SpacePressed ())
    {
      if (UpPressed () && !DownPressed ())
	if (upLift != -1)
	  {			// see if there is still a way up...
	    if (curShip.AllLifts[upLift].x == 99)
	      {
		printf ("Out of order, so sorry ..");
	      }
	    else
	      {
		downLift = curLift;
		curLift = upLift;
		curLevel = curShip.AllLifts[curLift].level;
		upLift = curShip.AllLifts[curLift].up;

		ShowLifts (curLevel, liftrow);

		
		MoveLiftSound ();
		while (UpPressed ()) ; 
	      }
	  }   // if uplevel 

      if (DownPressed () && !UpPressed ())
	if (downLift != -1)
	  {			// see if there is still a way down...
	    if (curShip.AllLifts[downLift].x == 99)
	      {
		printf ("Out of order, so sorry ..");
	      }
	    else
	      {
		upLift = curLift;
		curLift = downLift;
		curLevel = curShip.AllLifts[curLift].level;
		downLift = curShip.AllLifts[curLift].down;

		ShowLifts (curLevel, liftrow);

		MoveLiftSound ();
		while (DownPressed ()) ;
	      }
	  } // if downlevel 
    }	// while !SpaceReleased 

  //--------------------
  // It might happen, that the influencer enters the elevator, but then decides to
  // come out on the same level where he has been before.  In this case of course there
  // is no need to reshuffle enemys or to reset influencers position.  Therefore, only
  // when a real level change has occured, we need to do real changes as below, where
  // we set the new level and set new position and initiate timers and all that...
  //
  if (curLevel != CurLevel->levelnum)
    {				// see if we really changed the level or not...
      int array_num = 0;
      Level tmp;

      // set the current level
      while ((tmp = curShip.AllLevels[array_num]) != NULL)
	{
	  if (tmp->levelnum == curLevel)
	    break;
	  else
	    array_num++;
	}

      
      CurLevel = curShip.AllLevels[array_num];

      // redistribute the enemys around the level
      ShuffleEnemys ( array_num );

      // set the position of the influencer to the correct locatiohn
      Me[0].pos.x =
	curShip.AllLifts[curLift].x; 
      Me[0].pos.y =
	curShip.AllLifts[curLift].y; 

      // We reset the time on this level and the position history
      Me[0].FramesOnThisLevel=0;
      // for ( i = 0 ; i < MAX_INFLU_POSITION_HISTORY ; i++ ) 
      // {
      // Me[0].Position_History[ i ].x = Me[0].pos.x;
      // Me[0].Position_History[ i ].y = Me[0].pos.y;
      // }

      // delete all bullets and blasts
      for (i = 0; i < MAXBLASTS; i++)
	// AllBlasts[i].type = OUT;
	DeleteBlast( i );
      for (i = 0; i < MAXBULLETS; i++)
	{
	  // AllBullets[i].type = OUT;
	  // AllBullets[i].mine = FALSE;
	  // Never remove bullets any other way than via DeleteBullet or you will
	  // get SEGFAULTS!!!!!!!!!!
	  DeleteBullet ( i , FALSE ) ;
	}
    } // if real level change has occured

  LeaveLiftSound ( );
  ClearGraphMem ( );
  DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE );

  // UnfadeLevel ();

  // if the level is empty, let's color it in gray
  if (CurLevel->empty)
    LevelGrauFaerben ();

  InitBars = TRUE;

  while (SpacePressed ()) ;

  Me[0].status = MOBILE;
  Me[0].TextVisibleTime=0;
  Me[0].TextToBeDisplayed=CurLevel->Level_Enter_Comment;

  DebugPrintf (2, "\nvoid EnterLift(void): Usual end of function reached.");
}; // void EnterLift( void )

/* -----------------------------------------------------------------
 * This function should show a side-view of the ship, and hightlight 
 * the current level and lift
 *
 *  if level==-1: don't highlight any level
 *  if liftrow==-1: dont' highlight any liftrows
 *
 * ----------------------------------------------------------------- */
void
ShowLifts (int level, int liftrow)
{
  SDL_Rect src, dst;
  int i;
  SDL_Color lift_bg_color = {0,0,0};  /* black... */
  int xoffs = User_Rect.x + (User_Rect.w - SCALE_FACTOR*300)/2;
  int yoffs = User_Rect.y + (User_Rect.h - SCALE_FACTOR*180)/2;

  ship_off_pic= IMG_Load (find_file (ship_off_filename, GRAPHICS_DIR, TRUE));
  ship_on_pic = IMG_Load (find_file (ship_on_filename, GRAPHICS_DIR, TRUE));

  // clear the whole screen
  //ClearGraphMem();
  // fill the user fenster with some color
  FillRect (User_Rect, lift_bg_color);
  // DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE );      

  /* First blit ship "lights off" */
  Copy_Rect (User_Rect, dst);
  SDL_SetClipRect (Screen, &dst);
  Copy_Rect (User_Rect, dst);
  dst.x += xoffs;
  dst.y += yoffs;
  SDL_BlitSurface (ship_off_pic, NULL, Screen, &dst);
  
  if (level >= 0)
    for (i=0; i<curShip.num_level_rects[level]; i++)
      {
	Copy_Rect (curShip.Level_Rects[level][i], src);
	Copy_Rect (src, dst);
	dst.x += User_Rect.x + xoffs;   /* offset respective to User-Rectangle */
	dst.y += User_Rect.y + yoffs; 
	SDL_BlitSurface (ship_on_pic, &src, Screen, &dst);
      }

  if (liftrow >=0)
    {
      Copy_Rect (curShip.LiftRow_Rect[liftrow], src);
      Copy_Rect (src, dst);
      dst.x += User_Rect.x + xoffs;   /* offset respective to User-Rectangle */
      dst.y += User_Rect.y + yoffs; 
      SDL_BlitSurface (ship_on_pic, &src, Screen, &dst);
    }

  SDL_Flip (Screen);

  SDL_FreeSurface( ship_off_pic );
  SDL_FreeSurface( ship_on_pic );

  return;

}; // void ShowLifts( ... ) 

/* ----------------------------------------------------------------------
 * This function does all console duties.
 * This means the following:
 * 	2	* Show a small-scale plan of the current deck
 *	3	* Show a side-elevation on the ship
 *	1	* Give all available data on lower druid types
 *	0	* Reenter the game without squashing the colortable
 * ---------------------------------------------------------------------- */
void
EnterConsole (void)
{
  int finished = FALSE;
  int menu_pos = 0;
  char* fpath;

  //--------------------
  // Console picture need not be rendered fast or something.  This
  // really has time, so we load it as a surface and do not take the
  // elements apart (they dont have typical block format either)
  //
  if ( console_pic == NULL )
    {
      fpath = find_file ( NE_CONSOLE_PIC_FILE , GRAPHICS_DIR , FALSE );
      console_pic = IMG_Load ( fpath ); 
    }

  //--------------------
  // Prevent distortion of framerate by the delay coming from 
  // the time spend in the menu.
  Activate_Conservative_Frame_Computation();

  Me[0].status = CONSOLE;

  SwitchBackgroundMusicTo (CONSOLE_BACKGROUND_MUSIC_SOUND);

  SetCurrentFont( Para_BFont );
  SDL_SetClipRect ( Screen , NULL );

  while (SpacePressed ());  /* wait for user to release Space */

  while (!finished)
    {
      PaintConsoleMenu (menu_pos);
      SDL_Flip (Screen);
      usleep(2);

      if ( DownPressed() || MouseWheelDownPressed() ) 
	{
	  while (DownPressed());
	  if (menu_pos < 3) menu_pos++;

	  SDL_WarpMouse ( 47 , 265 + ( menu_pos -1 ) * 66 ) ;
	}
      if ( UpPressed() || MouseWheelUpPressed() ) 
	{
	  if (menu_pos > 0) menu_pos--;
	  while (UpPressed());

	  SDL_WarpMouse ( 47 , 265 + ( menu_pos -1 ) * 66 ) ;
	}

      if ( GetMousePos_y () <= 265 - 33 )
	menu_pos = 0;
      else if ( GetMousePos_y () <= 265 - 33 + 1 * 66 )
	menu_pos = 1;
      else if ( GetMousePos_y () <= 265 - 33 + 2 * 66 )
	menu_pos = 2;
      else
	menu_pos = 3;

      if ( EscapePressed() ) 
	{
	  finished = TRUE ;
	  while (EscapePressed());
	}
      if ( SpacePressed() || EnterPressed() ) 
	{
	  while ( SpacePressed() || EnterPressed() );
	  switch (menu_pos)
	    {
	    case 0:
	      finished = TRUE;
	      break;
	    case 1:
	      GreatDruidShow ();
	      break;
	    case 2:
	      ClearGraphMem();
	      ShowDeckMap (CurLevel);
	      SetCombatScaleTo( 1 );
	      break;
	    case 3:
	      ClearGraphMem();
	      GreatItemShow ();
	      break;
	    default:
	      DebugPrintf(0,"\nError in Console: menu-pos out of bounds \n");
	      Terminate(-1);
	      break;
	    } // switch menu_pos 
	}

    } // while (!finished) 

  Me[0].status = MOBILE;
  ClearGraphMem ( );
  DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE );
  SDL_Flip( Screen );

  while (SpacePressed ());

  // SwitchBackgroundMusicTo ( COMBAT_BACKGROUND_MUSIC_SOUND );
  SwitchBackgroundMusicTo ( CurLevel->Background_Song_Name );

  return;

} // void EnterConsole(void)

/* -----------------------------------------------------------------
 * This function shows the selectable menu items.
 *
 *  NOTE: this function does not actually _display_ anything yet,
 *        it just prepares the display, so you need
 *        to call SDL_Flip() to display the result!
 *
 * ----------------------------------------------------------------- */
void
PaintConsoleMenu (int menu_pos)
{
  char MenuText[1000];

  SDL_Rect SourceRectangle;
  SDL_Rect TargetRectangle;

  static SDL_Surface *image = NULL ;

  // DisplayImage ( find_file( NE_CONSOLE_BG_PIC1_FILE , GRAPHICS_DIR, FALSE) );
  // DisplayBanner (NULL, NULL,  BANNER_NO_SDL_UPDATE |BANNER_FORCE_UPDATE);

  //--------------------
  // If this has not happend yet, we load the console menu image
  // once and for all to be kept until the end of the game.
  //
  if ( image == NULL )
    {
      image = IMG_Load( find_file ( NE_CONSOLE_BG_PIC1_FILE , GRAPHICS_DIR , FALSE ) );
      if ( image == NULL ) {
	fprintf(stderr, "Couldn't load image %s: %s\n",
		find_file ( NE_CONSOLE_BG_PIC1_FILE , GRAPHICS_DIR , FALSE ) , IMG_GetError ( ) );
	Terminate(ERR);
      }
    }

  //--------------------
  // At this point we can safely display the image from memory.
  //
  SDL_BlitSurface( image , NULL , Screen , NULL );

  strcpy (MenuText, "Unit type ");
  strcat (MenuText, Druidmap[Me[0].type].druidname);
  strcat (MenuText, " - ");
  strcat (MenuText, Classname[Druidmap[Me[0].type].class]);
  strcat (MenuText, "\nAccess granted.\nArea : ");
  strcat (MenuText, curShip.AreaName ); // Shipnames[ThisShip]);
  strcat (MenuText, "\nDeck : ");
  strcat (MenuText, CurLevel->Levelname );
  strcat (MenuText, "\n\nAlert: ");
  strcat (MenuText, Alertcolor[Alert]);

  DisplayText (MenuText, Cons_Text_Rect.x, Cons_Text_Rect.y, &Cons_Text_Rect);

  /*
   * display the console menu-bar
   */
  SourceRectangle.x=(CONS_MENU_LENGTH+2)*menu_pos;
  SourceRectangle.y=0;
  SourceRectangle.w=CONS_MENU_LENGTH;
  SourceRectangle.h=CONS_MENU_HEIGHT;
  Copy_Rect (Cons_Menu_Rect, TargetRectangle);
  SDL_BlitSurface( console_pic , &SourceRectangle , Screen , &TargetRectangle );

  return;
}; // void PaintConsoleMenu ( int MenuPos )

/* ----------------------------------------------------------------------
 * This function is intended to show some buttons (with an arrow pointing
 * left and with an arrow pointing right and perhaps also with arrows
 * pointing up or down) so that one can easily navigate through the droid
 * show in the console most conveniently never needing anything but the
 * mouse.
 * ---------------------------------------------------------------------- */
void
ShowLeftRightDroidshowButtons ( void )
{
  ShowGenericButtonFromList ( UP_BUTTON );
  ShowGenericButtonFromList ( DOWN_BUTTON );
  ShowGenericButtonFromList ( LEFT_BUTTON );
  ShowGenericButtonFromList ( RIGHT_BUTTON );
}; // void ShowLeftRightDroidshowButtons ( void )

/* ----------------------------------------------------------------------
 * This function does the robot show when the user has selected robot
 * show from the console menu.
 * ---------------------------------------------------------------------- */
void
GreatDruidShow (void)
{
  int droidtype;
  int page;
  bool finished = FALSE;
  static int WasPressed = FALSE ;
  int ClearanceIndex = 0;
  int NumberOfClearances = 0;
  int i;

  //--------------------
  // First we find out how many clearances the Tux has gained
  // so far.
  //
  for ( i = 0 ; i < MAX_CLEARANCES ; i ++ )
    {
      if ( Me [ 0 ] . clearance_list [ i ] == 0 ) break;
    }
  NumberOfClearances = i;

  // droidtype = Me[0].type;
  droidtype = Me [ 0 ] . clearance_list [ ClearanceIndex ] ;

  page = 0;

  while (!finished)
    {
      usleep ( 2 );

      droidtype = Me [ 0 ] . clearance_list [ ClearanceIndex ] ;

      show_droid_info (droidtype, page , TRUE );
      ShowGenericButtonFromList ( MAP_EXIT_BUTTON );
      SDL_Flip ( Screen );

      if (SpacePressed() || EscapePressed() || axis_is_active )
	{
	  if ( CursorIsOnButton( UP_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) && axis_is_active && !WasPressed )
	    {
	      if ( ClearanceIndex < NumberOfClearances -1 ) 
		{
		  ClearanceIndex ++;	    
		  MoveMenuPositionSound();
		}
	    }
	  else if ( CursorIsOnButton( DOWN_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) && axis_is_active && !WasPressed )
	    {
	      if ( ClearanceIndex > 0) 
		{
		  ClearanceIndex --;	      
		  MoveMenuPositionSound();
		}
	    }
	  else if ( CursorIsOnButton( RIGHT_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) && axis_is_active && !WasPressed )
	    {
	      MoveMenuPositionSound();
	      if (page < 2) page ++;
	    }
	  else if ( CursorIsOnButton( LEFT_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) && axis_is_active && !WasPressed )
	    {
	      MoveMenuPositionSound();
	      if (page > 0) page --;
	    }
	  else if ( CursorIsOnButton( MAP_EXIT_BUTTON , GetMousePos_x ( ) + 16 , GetMousePos_y ( ) + 16 ) && axis_is_active && !WasPressed )
	    {
	      finished = TRUE;
	      while (SpacePressed() ||EscapePressed());
	    }

	  if ( ! CursorIsOnButton( UP_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) &&
	       ! CursorIsOnButton( DOWN_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) &&
	       ! CursorIsOnButton( LEFT_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) &&
	       ! CursorIsOnButton( RIGHT_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) )
	    {
	      // finished = TRUE;
	      // while (SpacePressed() ||EscapePressed());
	    }

	}

      WasPressed = axis_is_active;

      if (UpPressed() || MouseWheelUpPressed())
	{
	  MoveMenuPositionSound();
	  while (UpPressed());
	  if ( droidtype < Me[0].type) droidtype ++;
	}
      if (DownPressed() || MouseWheelDownPressed())
	{
	  MoveMenuPositionSound();
	  while (DownPressed());
	  if (droidtype > 0) droidtype --;
	}
      if (RightPressed() )
	{
	  MoveMenuPositionSound();
	  while (RightPressed());
	  if (page < 2) page ++;
	}
      if (LeftPressed() )
	{
	  MoveMenuPositionSound();
	  while (LeftPressed());
	  if (page > 0) page --;
	}

    } /* while !finished */

  return;
}; // void GreatDroidShow( void ) 

/* ----------------------------------------------------------------------
 * 
 *
 * ---------------------------------------------------------------------- */
int
AssemblePointerListForItemShow ( item** ItemPointerListPointer , int PlayerNum )
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
  // currently equipped
  //
  CurrentItemPointer = ItemPointerListPointer;
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
 * This function does the item show when the user has selected item
 * show from the console menu.
 * ---------------------------------------------------------------------- */
void
GreatItemShow (void)
{
  int ItemType;
  int page;
  bool finished = FALSE;
  static int WasPressed = FALSE ;
  item* ShowPointerList[ MAX_ITEMS_IN_INVENTORY ];
  int NumberOfItems;
  int ItemIndex=0;
  int PasswordIndex = (-1) ;
  int ClearanceIndex = (-1) ;
  int IdentifyAllowed = FALSE ;
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

  NumberOfItems = AssemblePointerListForItemShow ( &(ShowPointerList[0]), 0 );

  if ( ShowPointerList[0] == NULL )
    {
      MenuTexts[0]=" BACK ";
      MenuTexts[1]="";
      DoMenuSelection ( " YOU DONT HAVE ANYTHING IN INVENTORY, THAT COULD BE VIEWED. " , 
			MenuTexts, 1 , NULL , NULL );
      return;
    }

  ItemType = ShowPointerList [ ItemIndex ] -> type ;

  page = 0;

  while (!finished)
    {
      usleep ( 2 );

      //--------------------
      // We show all the info and the buttons that should be in this
      // interface...
      //
      show_item_info ( ShowPointerList [ ItemIndex ] , page , TRUE );
      PutPasswordButtonsAndPassword ( PasswordIndex );
      PutSecurityButtonsAndClearance ( ClearanceIndex );
      ShowGenericButtonFromList ( MAP_EXIT_BUTTON );
      if ( ShowPointerList [ ItemIndex ] -> is_identified ) 
	ShowGenericButtonFromList ( CONSOLE_IDENTIFY_BUTTON_YELLOW );
      else 
	{
	  if ( IdentifyAllowed )
	    ShowGenericButtonFromList ( CONSOLE_IDENTIFY_BUTTON_GREEN );
	  else
	    ShowGenericButtonFromList ( CONSOLE_IDENTIFY_BUTTON_RED );
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
      // ItemType = Me [ 0 ] . clearance_list [ ClearanceIndex ] ;

      if (SpacePressed() || EscapePressed() || axis_is_active )
	{
	  if ( CursorIsOnButton( UP_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) && axis_is_active && !WasPressed )
	    {
	      if ( ItemIndex < NumberOfItems -1 ) 
		{
		  ItemIndex ++;	    
		  MoveMenuPositionSound();
		}
	    }
	  else if ( CursorIsOnButton( DOWN_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) && axis_is_active && !WasPressed )
	    {
	      if ( ItemIndex > 0) 
		{
		  ItemIndex --;	      
		  MoveMenuPositionSound();
		}
	    }
	  else if ( CursorIsOnButton( RIGHT_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) && axis_is_active && !WasPressed )
	    {
	      MoveMenuPositionSound();
	      if (page < 2) page ++;
	    }
	  else if ( CursorIsOnButton( LEFT_BUTTON , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) && axis_is_active && !WasPressed )
	    {
	      MoveMenuPositionSound();
	      if (page > 0) page --;
	    }
	  else if ( CursorIsOnButton( CONSOLE_IDENTIFY_BUTTON_GREEN , GetMousePos_x() + 16 , GetMousePos_y() + 16 ) && axis_is_active && !WasPressed )
	    {
	      MoveMenuPositionSound();
	      ShowPointerList [ ItemIndex ] ->is_identified = TRUE;
	    }
	  else if ( CursorIsOnButton( MAP_SECURITYLEFT_BUTTON , GetMousePos_x ( ) + 16 , GetMousePos_y ( ) + 16 ) && axis_is_active && !WasPressed )
	    {
	      if ( ClearanceIndex > 0 ) 
		{
		  ClearanceIndex --;
		  MenuItemSelectedSound ( ) ;
		  PasswordIndex = (-1) ;
		  // SelectedFunction = NO_FUNCTION ;
		}
	    }
	  else if ( CursorIsOnButton( MAP_SECURITYRIGHT_BUTTON , GetMousePos_x ( ) + 16 , GetMousePos_y ( ) + 16 ) && axis_is_active && !WasPressed )
	    {
	      if ( ClearanceIndex < MAX_CLEARANCES - 1 )
		{
		  if ( Me [ 0 ] . clearance_list [ ClearanceIndex + 1 ] ) 
		    {
		      ClearanceIndex ++;
		      MenuItemSelectedSound();
		      PasswordIndex = (-1) ;
		      // SelectedFunction = NO_FUNCTION ;
		    }
		}
	    }
	  else if ( CursorIsOnButton( MAP_PASSWORDLEFT_BUTTON , GetMousePos_x ( ) + 16 , GetMousePos_y ( ) + 16 ) && axis_is_active && !WasPressed )
	    {
	      if ( PasswordIndex > 0 ) 
		{
		  PasswordIndex --;
		  MenuItemSelectedSound ( ) ;
		  ClearanceIndex = (-1) ;
		  // SelectedFunction = NO_FUNCTION ;
		}
	    }
	  else if ( CursorIsOnButton( MAP_PASSWORDRIGHT_BUTTON , GetMousePos_x ( ) + 16 , GetMousePos_y ( ) + 16 ) && axis_is_active && !WasPressed )
	    {
	      if ( PasswordIndex < MAX_PASSWORDS - 1 )
		{
		  if ( strlen ( Me [ 0 ] . password_list [ PasswordIndex + 1 ] ) > 0 ) 
		    {
		      PasswordIndex ++;
		      MenuItemSelectedSound();
		      ClearanceIndex = (-1) ;
		      // SelectedFunction = NO_FUNCTION ;
		    }
		}
	    }
	  else if ( CursorIsOnButton( MAP_EXIT_BUTTON , GetMousePos_x ( ) + 16 , GetMousePos_y ( ) + 16 ) && axis_is_active && !WasPressed )
	    {
	      finished = TRUE;
	      while (SpacePressed() ||EscapePressed());
	    }

	}

      WasPressed = axis_is_active;

      if (UpPressed() || MouseWheelUpPressed())
	{
	  MoveMenuPositionSound();
	  while (UpPressed());
	  if ( ItemType < Me[0].type) ItemType ++;
	}
      if (DownPressed() || MouseWheelDownPressed())
	{
	  MoveMenuPositionSound();
	  while (DownPressed());
	  if (ItemType > 0) ItemType --;
	}
      if (RightPressed() )
	{
	  MoveMenuPositionSound();
	  while (RightPressed());
	  if (page < 2) page ++;
	}
      if (LeftPressed() )
	{
	  MoveMenuPositionSound();
	  while (LeftPressed());
	  if (page > 0) page --;
	}

    } /* while !finished */

  return;
}; // void GreatItemShow( void ) 

/* ------------------------------------------------------------
 * display infopage page of droidtype
 * does update the screen, no SDL_Flip() necesary !
 * ------------------------------------------------------------ */
void 
show_droid_info (int droidtype, int page , char ShowArrows )
{
  char InfoText[1000];
  char None[20] = "none";
  char *item_name;
  int type;
  SDL_SetClipRect ( Screen , NULL );
  DisplayImage ( find_file( NE_CONSOLE_BG_PIC2_FILE , GRAPHICS_DIR, FALSE) );
  // DisplayBanner (NULL, NULL,  BANNER_NO_SDL_UPDATE | BANNER_FORCE_UPDATE );

  ShowRobotPicture (Cons_Menu_Rect.x, Cons_Menu_Rect.y,  droidtype );

  switch (page)
    {
    case 0:
      sprintf( InfoText, "\
Unit type %s - %s\n\
Entry : %d\n\
Class : %s\n\
Height : %f\n\
Weight: %f \n\
Drive : %s \n\
Brain : %s", Druidmap[droidtype].druidname, Classname[Druidmap[ droidtype ].class],
	       droidtype+1, Classes[Druidmap[droidtype].class],
	       Druidmap[droidtype].height, Druidmap[droidtype].weight,
	       ItemMap [ Druidmap[ droidtype ].drive_item.type ].item_name,
	       Brainnames[ Druidmap[droidtype].brain ]);
      break;
    case 1:
      if ( (type = Druidmap[droidtype].weapon_item.type) >= 0) /* make sure item=-1 */
	item_name = ItemMap[type].item_name;                     /* does not segfault */
      else 
	item_name = None;

      sprintf( InfoText , "\
Unit type %s - %s\n\
Armamant : %s\n\
Sensors  1: %s\n          2: %s\n          3: %s", Druidmap[droidtype].druidname,
	       Classname[Druidmap[droidtype].class],
	       item_name,
	       Sensornames[ Druidmap[droidtype].sensor1 ],
	       Sensornames[ Druidmap[droidtype].sensor2 ],
	       Sensornames[ Druidmap[droidtype].sensor3 ]);
      break;
    case 2:
      sprintf (InfoText, "Unit type %s - %s\n\
Notes: %s", Druidmap[droidtype].druidname , Classname[Druidmap[droidtype].class],
	       Druidmap[droidtype].notes);
      break;
    default:
      sprintf (InfoText, "ERROR: Page not implemented!! \nPlease report bug!");
      break;
    } /* switch (page) */

  SetCurrentFont( Para_BFont );
  DisplayText (InfoText, Cons_Text_Rect.x, Cons_Text_Rect.y, &Cons_Text_Rect);

  if ( ShowArrows ) ShowLeftRightDroidshowButtons (  );

  // SDL_Flip (Screen);

} /* show_droid_info */


/* ----------------------------------------------------------------------
 * This function displays an item picture. 
 * ---------------------------------------------------------------------- */
void
ShowItemPicture (int PosX, int PosY, int Number )
{
  SDL_Surface *tmp;
  SDL_Rect target;
  char ConstructedFileName[5000];
  char* fpath;
  static int LastImageSeriesLoaded = ( -1 );
  int CurrentImageSeries;
#define NUMBER_OF_IMAGES_IN_ITEM_ROTATION 40
  static SDL_Surface *ItemRotationSurfaces[ NUMBER_OF_IMAGES_IN_ITEM_ROTATION ] = { NULL } ;
  SDL_Surface *Whole_Image;
  int i;
  int RotationIndex;

  DebugPrintf (2, "\nvoid ShowItemPicture(...): Function call confirmed.");

  CurrentImageSeries = ItemMap[ Number ] . rotation_model_number ;

  if ( CurrentImageSeries == (-1) ) return;

  //--------------------
  // Maybe we have to reload the whole image series
  //
  if ( LastImageSeriesLoaded != CurrentImageSeries )
    {
      //--------------------
      // Maybe we have to free the series from an old item display first
      //
      if ( ItemRotationSurfaces[ 0 ] != NULL )
	{
	  for ( i = 1 ; i < NUMBER_OF_IMAGES_IN_ITEM_ROTATION ; i ++ )
	    {
	      SDL_FreeSurface ( ItemRotationSurfaces[ i ] ) ;
	    }
	}

      //--------------------
      // Now we can start to load the whole series into memory
      //
      for ( i=0 ; i < NUMBER_OF_IMAGES_IN_ITEM_ROTATION ; i++ )
	{
	  sprintf ( ConstructedFileName , "rotation_models/item_%02d_%04d.png" , CurrentImageSeries , i+1 );
	  DebugPrintf ( 1 , "\nConstructedFileName = %s " , ConstructedFileName );
	  // fpath = find_file ( "rotation_models/anim0001.png" , GRAPHICS_DIR, FALSE );
	  fpath = find_file ( ConstructedFileName , GRAPHICS_DIR, FALSE );
	  
	  Whole_Image = IMG_Load( fpath ); // This is a surface with alpha channel, since the picture is one of this type
	  if ( Whole_Image == NULL )
	    {
	      fprintf( stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
Freedroid was unable to load a rotated image of an item into memory.\n\
\n\
The full path name of the file, that could not be loaded was : \n\
%s\n\
\n\
This error indicates some installation problem with freedroid.\n\
Please contact the developers, as always freedroid-discussion@lists.sourceforge.net.\n\
Thanks a lot.\n\
\n\
But for now Freedroid will terminate to draw attention \n\
to the graphics loading problem it could not resolve.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" , fpath );
	      Terminate(ERR);
	    }
	  
	  SDL_SetAlpha( Whole_Image , 0 , SDL_ALPHA_OPAQUE );
	  
	  ItemRotationSurfaces[i] = SDL_DisplayFormatAlpha( Whole_Image ); // now we have an alpha-surf of right size
	  SDL_SetColorKey( ItemRotationSurfaces[i] , 0 , 0 ); // this should clear any color key in the dest surface
	  
	  SDL_FreeSurface( Whole_Image );
	  
	}

    }

  // strcpy( fname, Druidmap[Number].druidname );

  /*
  strcpy( fname, "droids/" );
  strcat( fname, Druidmap[Number].portrait_filename_without_ext );
  strcat( fname , ".png" );

  fpath = find_file (fname, GRAPHICS_DIR, FALSE);

  if ( (tmp=IMG_Load (fpath)) == NULL )
    {
      fprintf (stderr,
	     "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
The image file named %s could not be read by SDL.\n\
\n\
The error reason as specified by SDL is : %s.\n\
\n\
Please check that the file is present and not corrupted\n\
in your distribution of Freedroid.\n\
\n\
Freedroid will terminate now to point at the error.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" , fpath , SDL_GetError() );
      Terminate (ERR);
    }
  */

  RotationIndex = ( SDL_GetTicks() / 50 ) ;

  RotationIndex = RotationIndex - ( RotationIndex / NUMBER_OF_IMAGES_IN_ITEM_ROTATION ) * NUMBER_OF_IMAGES_IN_ITEM_ROTATION ;

  // tmp = ItemImageList[ ItemMap[ Number ].picture_number ].Surface ;

  tmp = ItemRotationSurfaces[ RotationIndex ] ;

  SDL_SetClipRect( Screen , NULL );
  Set_Rect ( target, PosX, PosY, SCREEN_WIDTH, SCREEN_HEIGHT);
  SDL_BlitSurface( tmp , NULL, Screen , &target);

  // SDL_FreeSurface(tmp);

  DebugPrintf (2, "\nvoid ShowItemPicture(...): Usual end of function reached.");
}; // void ShowItemPicture ( ... )


/* ------------------------------------------------------------
 * display infopage page of droidtype
 * does update the screen, no SDL_Flip() necesary !
 * ------------------------------------------------------------ */
void 
show_item_info ( item* ShowItem , int page , char ShowArrows )
{
  char InfoText[1000];
  // char None[20] = "none";
  // char *item_name;
  // int type;
  SDL_SetClipRect ( Screen , NULL );
  DisplayImage ( find_file( NE_CONSOLE_BG_PIC2_FILE , GRAPHICS_DIR, FALSE) );
  // DisplayBanner (NULL, NULL,  BANNER_NO_SDL_UPDATE | BANNER_FORCE_UPDATE );

  ShowItemPicture (Cons_Menu_Rect.x, Cons_Menu_Rect.y, ShowItem->type );

  switch (page)
    {
    case 0:
      /*
      sprintf( InfoText, "\
Unit type %s - %s\n\
Entry : %d\n\
Class : %s\n\
Height : %f\n\
Weight: %f \n\
Drive : %s \n\
Brain : %s", ItemMap [ ShowItem->type ] . item_name, Classname[Druidmap[ droidtype ].class],
	       droidtype+1, Classes[Druidmap[droidtype].class],
	       Druidmap[droidtype].height, Druidmap[droidtype].weight,
	       ItemMap [ Druidmap[ droidtype ].drive_item.type ].item_name,
	       Brainnames[ Druidmap[droidtype].brain ]);
      */
      sprintf( InfoText, "Item: %s \nClass: %s\n\
Duration: %d / %d\n\
Required Str: %d\n\
Required Dex: %d\n\
Required Mag: %d\n\
Base list price: %d\n", 
	       ItemMap [ ShowItem->type ] . item_name, 
	       ItemMap [ ShowItem->type ] . item_class,
	       (int)ShowItem->current_duration, 
	       ShowItem->max_duration ,
	       ItemMap [ ShowItem->type ] . item_require_strength,
	       ItemMap [ ShowItem->type ] . item_require_dexterity,
	       ItemMap [ ShowItem->type ] . item_require_magic,
	       ItemMap [ ShowItem->type ] . base_list_price );

      break;
    case 1:
      /*
      if ( (type = Druidmap[droidtype].weapon_item.type) >= 0) 
	item_name = ItemMap[type].item_name;                   
      else 
	item_name = None;

      sprintf( InfoText , "\
Unit type %s - %s\n\
Armamant : %s\n\
Sensors  1: %s\n          2: %s\n          3: %s", Druidmap[droidtype].druidname,
	       Classname[Druidmap[droidtype].class],
	       item_name,
	       Sensornames[ Druidmap[droidtype].sensor1 ],
	       Sensornames[ Druidmap[droidtype].sensor2 ],
	       Sensornames[ Druidmap[droidtype].sensor3 ]);
*/
      //--------------------
      // On this page we show the weapon properties (in case of a weapon)
      //
      sprintf( InfoText, "Item: %s \nClass: %s\n\
Damage: %d - %d\n\
Recharge time: %f\n\
Defence bonus: %d\n\
Speed / Acceleration: %d / %d \n", 
	       ItemMap [ ShowItem->type ] . item_name, 
	       ItemMap [ ShowItem->type ] . item_class,
	       ItemMap [ ShowItem->type ] . base_item_gun_damage,
	       ItemMap [ ShowItem->type ] . base_item_gun_damage +
	       ItemMap [ ShowItem->type ] . item_gun_damage_modifier,
	       ItemMap [ ShowItem->type ] . item_gun_recharging_time,
	       ShowItem->ac_bonus,
	       (int)ItemMap [ ShowItem->type ] . item_drive_maxspeed,
	       (int)ItemMap [ ShowItem->type ] . item_drive_accel );




      break;
    case 2:
      sprintf (InfoText, "Item: %s \nClass: %s\n\
Notes: %s", 
	       ItemMap [ ShowItem->type ] . item_name, 
	       ItemMap [ ShowItem->type ] . item_class,
	       ItemMap [ ShowItem->type ] . item_description );
      break;
    default:
      // sprintf (InfoText, "ERROR: Page not implemented!! \nPlease report bug!");
      break;
    } // switch (page) 

  SetCurrentFont( Para_BFont );
  DisplayText (InfoText, Cons_Text_Rect.x, Cons_Text_Rect.y, &Cons_Text_Rect);

  if ( ShowArrows ) ShowLeftRightDroidshowButtons (  );

  // SDL_Flip (Screen);

}; // void show_item_info ( ... )

/* ----------------------------------------------------------------------
 * This function should delect a certain security clearance and reorder 
 * the security clearances array afterwards.
 * ---------------------------------------------------------------------- */
void 
DeleteSecurityClearances( int PlayerNum , int ClearanceIndex )
{
  int i;
  
  for ( i = ClearanceIndex ; i < MAX_CLEARANCES-1 ; i ++ )
    {
      Me [ PlayerNum ] . clearance_list [ i ] = Me [ PlayerNum ] . clearance_list [ i + 1 ] ;
    }
}; // void DeleteSecurityClearances( int PlayerNum , int ClearanceIndex )


/* -----------------------------------------------------------------
 * This function displays the map of the current level and also 
 * affers some menu choices that do some functions of this console:
 *  * You can unlock doors
 *  * You can turn on/off autoguns
 * ----------------------------------------------------------------- */
void
ShowDeckMap (Level deck)
{
  finepoint tmp;
  static char LeftMouseWasPressed = FALSE;
  int ExitNow;
  int SelectedFunction = NO_FUNCTION ;
  grob_point TargetSquare;
  char MapValue;
  int ClearanceIndex = -1 ;
  int PasswordIndex = -1 ;
  int GunTypeSelected = -1 ;
  int UnlockAllowed = FALSE ;
  int GunOnAllowed = FALSE ;
  int GunOffAllowed = FALSE ;
  int ReadEmailAllowed = FALSE ;
  int EnergyRate;
  char EnergyRationString[100];
  char* MenuTexts[ 10 ];
  int MenuPosition;

  MenuTexts[0]="";
  MenuTexts[1]="";
  MenuTexts[2]="";
  MenuTexts[3]="";
  MenuTexts[4]="";
  MenuTexts[5]="";
  MenuTexts[8]="";
  MenuTexts[6]="";
  MenuTexts[7]="";
  MenuTexts[9]="";


  tmp.x=Me[0].pos.x;
  tmp.y=Me[0].pos.y;

  ClearUserFenster ();

  SetCombatScaleTo( 0.25 );

  ExitNow = FALSE ;

  while ( ! ExitNow )
    {
      //--------------------
      // First we see what operations are allowed with the
      // current login configuration of the Tux.
      //
      UnlockAllowed = FALSE ;
      GunOnAllowed = FALSE ;
      GunOffAllowed = FALSE ;
      ReadEmailAllowed = FALSE ;
      if ( PasswordIndex != (-1) )
	{
	  if ( ! strcmp ( Me [ 0 ] . password_list [ PasswordIndex ] , "Tux Dummy1" )  )
	    {
	      UnlockAllowed = TRUE ;
	    } 
	  if ( ! strcmp ( Me [ 0 ] . password_list [ PasswordIndex ] , "Tux Dummy2" )  )
	    {
	      GunOffAllowed = TRUE ;
	    } 
	  if ( ! strcmp ( Me [ 0 ] . password_list [ PasswordIndex ] , "Tux Himself" )  )
	    {
	      ReadEmailAllowed = TRUE ;
	      GunOnAllowed = TRUE ;
	    }
	}
      if ( ClearanceIndex < 0 ) EnergyRate = 0 ; // no energy except with clearance
      else EnergyRate = (int)Druidmap [ Me [ 0 ] . clearance_list [ ClearanceIndex ] ] . maxenergy ;
      if ( ! strcmp ( Me [ 0 ] . password_list [ PasswordIndex ] , "Tux Energy" ) ) EnergyRate = 10 ;


      ExitNow = EscapePressed();

      if ( UpPressed() )
	{
	  if ( Me[0].pos.y > 1 ) Me[0].pos.y -- ;
	  while ( UpPressed ( ) );
	}
      if ( DownPressed() )
	{
	  if ( Me[0].pos.y < curShip.AllLevels[Me[0].pos.z]->ylen-2 ) Me[0].pos.y ++;
	  while ( DownPressed ( ) );
	}
      if ( RightPressed() )
	{
	  if ( Me[0].pos.x < curShip.AllLevels[Me[0].pos.z]->xlen-2 ) Me[0].pos.x ++;
	  while ( RightPressed ( ) );
	}
      if ( LeftPressed() )
	{
	  if ( Me[0].pos.x > 1 ) Me[0].pos.x --;
	  while ( LeftPressed ( ) );
	}

      //--------------------
      // Pressing the mouse button should allow to move about over the small
      // map displayed in the console screen.
      //
      if ( !LeftMouseWasPressed && axis_is_active )
	{
	  //--------------------
	  // Maybe that click went right onto the exit button.  Then
	  // of course nothing else will be done but an exit performed.
	  //
	  if ( CursorIsOnButton( MAP_EXIT_BUTTON , GetMousePos_x ( ) + 16 , GetMousePos_y ( ) + 16 ) )
	    {
	      ExitNow = TRUE;
	    }
	  //--------------------
	  // Maybe that click went right onto the exit button.  Then
	  // of course nothing else will be done but an exit performed.
	  //
	  else if ( CursorIsOnButton( MAP_UNLOCK_BUTTON_GREEN , GetMousePos_x ( ) + 16 , GetMousePos_y ( ) + 16 ) )
	    {
	      if ( UnlockAllowed )
		{
		  if ( SelectedFunction == UNLOCK_FUNCTION ) SelectedFunction = NO_FUNCTION;
		  else 
		    {
		      SelectedFunction = UNLOCK_FUNCTION;
		      PlayOnceNeededSoundSample ( "../effects/CONSOLE_Select_Door_To_Unlock_0.wav" , FALSE );
		    }
		}
	      else
		{
		  PlayOnceNeededSoundSample ( "../effects/CONSOLE_Permission_Denied_0.wav" , FALSE );
		}
	    }
	  else if ( CursorIsOnButton( MAP_GUNOFF_BUTTON_GREEN , GetMousePos_x ( ) + 16 , GetMousePos_y ( ) + 16 ) )
	    {
	      if ( GunOffAllowed )
		{
		  if ( SelectedFunction == GUNOFF_FUNCTION ) SelectedFunction = NO_FUNCTION;
		  else 
		    {
		      SelectedFunction = GUNOFF_FUNCTION;
		      PlayOnceNeededSoundSample ( "../effects/CONSOLE_Select_Gun_To_Switch_0.wav" , FALSE );
		    }
		}
	      else
		{
		  PlayOnceNeededSoundSample ( "../effects/CONSOLE_Permission_Denied_0.wav" , FALSE );		  
		}
	    }
	  else if ( CursorIsOnButton( MAP_GUNON_BUTTON_GREEN , GetMousePos_x ( ) + 16 , GetMousePos_y ( ) + 16 ) )
	    {
	      if ( GunOnAllowed )
		{
		  if ( SelectedFunction == GUNON_FUNCTION ) SelectedFunction = NO_FUNCTION;
		  else 
		    {
		      SelectedFunction = GUNON_FUNCTION;
		      PlayOnceNeededSoundSample ( "../effects/CONSOLE_Select_Gun_Type_0.wav" , FALSE );
		    }
		}
	      else
		{
		  PlayOnceNeededSoundSample ( "../effects/CONSOLE_Permission_Denied_0.wav" , FALSE );		  
		}
	    }
	  else if ( CursorIsOnButton( MAP_REQUEST_ENERGY_RATION_GREEN_BUTTON , 
				      GetMousePos_x ( ) + 16 , GetMousePos_y ( ) + 16 ) )
	    {
	      if ( EnergyRate > 0 )
		{
		  PlayOnceNeededSoundSample ( "../effects/CONSOLE_Energy_Transferred_0.wav" , FALSE );		  
		  Me [ 0 ] . energy += EnergyRate ;

		  //--------------------
		  // If this was done via a clearance, we will delete this
		  // clearance now, cause it was used up.
		  //
		  if ( ClearanceIndex != (-1) )
		    {
		      Me [ 0 ] . clearance_list [ ClearanceIndex ] = 0; 
		      DeleteSecurityClearances( 0 , ClearanceIndex );
		      ClearanceIndex = (-1) ; // do not ever leave the allowed range, so do this to be safe!
		    }
		}
	    }
	  else if ( CursorIsOnButton( MAP_READ_EMAIL_GREEN_BUTTON , 
				      GetMousePos_x ( ) + 16 , GetMousePos_y ( ) + 16 ) )
	    {
	      if ( ReadEmailAllowed )
		{
		  PlayOnceNeededSoundSample ( "../effects/CONSOLE_Browsing_Information_0.wav" , FALSE );		  
		  if ( ! strcmp ( Me [ 0 ] . password_list [ PasswordIndex ] , "Tux Himself" )  )
		    {
		      MenuTexts[ 0 ] = "Welcome Tux!" ;
		      MenuTexts[ 1 ] = "Hi guys!" ;
		      MenuTexts[ 2 ] = "BACK" ;
		      while ( SpacePressed() );
		      MenuPosition = DoMenuSelection ( "\n    Select message to read " , MenuTexts , 1 , NULL , NULL );
		      switch ( MenuPosition )
			{
			case 0:
			case 1:
			  ScrollText ( EmailText1 , SCROLLSTARTX, SCROLLSTARTY, User_Rect.y , NE_TITLE_PIC_FILE );
			  break;
			case 2:
			  ScrollText ( EmailText2 , SCROLLSTARTX, SCROLLSTARTY, User_Rect.y , NE_TITLE_PIC_FILE );
			  break;
			default:
			  break;
			}
		    }
		}
	      else
		PlayOnceNeededSoundSample ( "../effects/CONSOLE_Permission_Denied_0.wav" , FALSE );		  
	    }
	  else if ( CursorIsOnButton( MAP_SECURITYLEFT_BUTTON , GetMousePos_x ( ) + 16 , GetMousePos_y ( ) + 16 ) )
	    {
	      if ( ClearanceIndex > 0 ) 
		{
		  ClearanceIndex --;
		  MenuItemSelectedSound ( ) ;
		  PasswordIndex = (-1) ;
		  SelectedFunction = NO_FUNCTION ;
		}
	    }
	  else if ( CursorIsOnButton( MAP_SECURITYRIGHT_BUTTON , GetMousePos_x ( ) + 16 , GetMousePos_y ( ) + 16 ) )
	    {
	      if ( ClearanceIndex < MAX_CLEARANCES - 1 )
		{
		  if ( Me [ 0 ] . clearance_list [ ClearanceIndex + 1 ] ) 
		    {
		      ClearanceIndex ++;
		      MenuItemSelectedSound();
		      PasswordIndex = (-1) ;
		      SelectedFunction = NO_FUNCTION ;
		    }
		}
	    }
	  else if ( CursorIsOnButton( MAP_PASSWORDLEFT_BUTTON , GetMousePos_x ( ) + 16 , GetMousePos_y ( ) + 16 ) )
	    {
	      if ( PasswordIndex > 0 ) 
		{
		  PasswordIndex --;
		  MenuItemSelectedSound ( ) ;
		  ClearanceIndex = (-1) ;
		  SelectedFunction = NO_FUNCTION ;
		}
	    }
	  else if ( CursorIsOnButton( MAP_PASSWORDRIGHT_BUTTON , GetMousePos_x ( ) + 16 , GetMousePos_y ( ) + 16 ) )
	    {
	      if ( PasswordIndex < MAX_PASSWORDS - 1 )
		{
		  if ( strlen ( Me [ 0 ] . password_list [ PasswordIndex + 1 ] ) > 0 ) 
		    {
		      PasswordIndex ++;
		      MenuItemSelectedSound();
		      ClearanceIndex = (-1) ;
		      SelectedFunction = NO_FUNCTION ;
		    }
		}
	    }
	  //--------------------
	  // The remaining case is that no particular button but rather some
	  // place in the map was clicked on.
	  //
	  else
	    {

	      //--------------------
	      // First we find out which map square the player wishes us to operate on
	      // 
	      TargetSquare.x = rintf ( Me [ 0 ] . pos . x + (float)( GetMousePos_x ( ) + 16 - ( SCREEN_WIDTH / 2 ) ) / ( INITIAL_BLOCK_WIDTH * 0.25 ) ) ;
	      TargetSquare.y = rintf ( Me [ 0 ] . pos . y + (float)( GetMousePos_y ( ) + 16 - ( SCREEN_HEIGHT / 2 ) ) / ( INITIAL_BLOCK_HEIGHT * 0.25 ) ) ;

	      //--------------------
	      // If no function was selected, then a plain move on the map is
	      // what we need to do.
	      //
	      if ( SelectedFunction == NO_FUNCTION )
		{
		  Me [ 0 ] . pos . x += ( GetMousePos_x ( ) + 16 - ( SCREEN_WIDTH / 2 ) ) / ( INITIAL_BLOCK_WIDTH * 0.25 ) ;
		  if ( Me [ 0 ] . pos . x >= curShip.AllLevels[Me[0].pos.z]->xlen-2 )
		    Me [ 0 ] . pos . x = curShip.AllLevels[Me[0].pos.z]->xlen-2 ;
		  if ( Me [ 0 ] . pos . x <= 2 ) Me [ 0 ] . pos . x = 2;
		  
		  Me [ 0 ] . pos . y += ( GetMousePos_y ( ) + 16 - ( SCREEN_HEIGHT / 2 ) ) / ( INITIAL_BLOCK_WIDTH * 0.25 ) ;
		  if ( Me [ 0 ] . pos . y >= curShip.AllLevels[Me[0].pos.z]->ylen-2 )
		    Me [ 0 ] . pos . y = curShip.AllLevels[Me[0].pos.z]->ylen-2 ;
		  if ( Me [ 0 ] . pos . y <= 2 ) Me [ 0 ] . pos . y = 2;
		}
	      else if ( SelectedFunction == UNLOCK_FUNCTION )
		{
		  //--------------------
		  // Now we try to unlock the LOCKED door that should be present at the
		  // location currently pointed at via the mouse cursor.
		  //

		  //--------------------
		  // Some sanity check again against clicks ouside of the bounds of the map...
		  //
		  if ( ! ( ( TargetSquare.x < 0 ) || ( TargetSquare.y < 0 ) ||
			   ( TargetSquare.x + 1 >= curShip . AllLevels [ Me [ 0 ] . pos . z ] -> xlen ) ||
			   ( TargetSquare.y + 1 >= curShip . AllLevels [ Me [ 0 ] . pos . z ] -> ylen ) ) )
		    {
		      MapValue = curShip . AllLevels [ Me [ 0 ] . pos . z ] -> map [ TargetSquare.y ] [ TargetSquare.x ] ;
		      DebugPrintf ( 0 , "Map value found at click location: %d. " , MapValue );
		      if ( MapValue == LOCKED_H_SHUT_DOOR )
			{
			  curShip . AllLevels [ Me [ 0 ] . pos . z ] -> map [ TargetSquare.y ] [ TargetSquare.x ] = H_SHUT_DOOR ;
			  PlayOnceNeededSoundSample ( "../effects/CONSOLE_Door_Successfully_Unlocked_0.wav" , FALSE );
			  SelectedFunction = NO_FUNCTION;
			  GetDoors( curShip.AllLevels[ Me [ 0 ] . pos . z ]  );
			}
		      if ( MapValue == LOCKED_V_SHUT_DOOR )
			{
			  curShip . AllLevels [ Me [ 0 ] . pos . z ] -> map [ TargetSquare.y ] [ TargetSquare.x ] = V_SHUT_DOOR ;
			  PlayOnceNeededSoundSample ( "../effects/CONSOLE_Door_Successfully_Unlocked_0.wav" , FALSE );
			  SelectedFunction = NO_FUNCTION;
			  GetDoors( curShip.AllLevels[ Me [ 0 ] . pos . z ]  );
			}                                         
		    }
		}
	      else if ( SelectedFunction == GUNOFF_FUNCTION )
		{
		  //--------------------
		  // Now we try to turn off the gun turret that should be present at the
		  // location currently pointed at via the mouse cursor.
		  //

		  //--------------------
		  // Some sanity check again against clicks ouside of the bounds of the map...
		  //
		  if ( ! ( ( TargetSquare.x < 0 ) || ( TargetSquare.y < 0 ) ||
			   ( TargetSquare.x + 1 >= curShip . AllLevels [ Me [ 0 ] . pos . z ] -> xlen ) ||
			   ( TargetSquare.y + 1 >= curShip . AllLevels [ Me [ 0 ] . pos . z ] -> ylen ) ) )
		    {
		      MapValue = curShip . AllLevels [ Me [ 0 ] . pos . z ] -> map [ TargetSquare.y ] [ TargetSquare.x ] ;
		      DebugPrintf ( 0 , "Map value found at click location: %d. " , MapValue );
		      if ( ( MapValue == AUTOGUN_R ) || ( MapValue == AUTOGUN_L ) || 
			   ( MapValue == AUTOGUN_D ) || ( MapValue == AUTOGUN_U ) )
			{
			  curShip . AllLevels [ Me [ 0 ] . pos . z ] -> map [ TargetSquare.y ] [ TargetSquare.x ] = BLOCK1 ;
			  PlayOnceNeededSoundSample ( "../effects/CONSOLE_Gun_Successfully_Deactivated_0.wav" , FALSE );
			  SelectedFunction = NO_FUNCTION;
			  GetAutoguns( curShip.AllLevels[ Me [ 0 ] . pos . z ]  );
			}
		    }
		}
	      else if ( SelectedFunction == GUNON_FUNCTION )
		{
		  //--------------------
		  // If the mouse button is just over a gun turret type, then
		  // this turret type will be the new seceted turret type.
		  //
		  if ( CursorIsOnButton( MAP_GUN_TYPE_1_BUTTON_RED , GetMousePos_x ( ) + 16 , GetMousePos_y ( ) + 16 ) )
		    {
		      GunTypeSelected = 1 ;
		    }
		  else if ( CursorIsOnButton( MAP_GUN_TYPE_2_BUTTON_RED , GetMousePos_x ( ) + 16 , GetMousePos_y ( ) + 16 ) )
		    {
		      GunTypeSelected = 2 ;
		    }
		  else if ( CursorIsOnButton( MAP_GUN_TYPE_3_BUTTON_RED , GetMousePos_x ( ) + 16 , GetMousePos_y ( ) + 16 ) )
		    {
		      GunTypeSelected = 3 ;
		    }
		  else if ( CursorIsOnButton( MAP_GUN_TYPE_4_BUTTON_RED , GetMousePos_x ( ) + 16 , GetMousePos_y ( ) + 16 ) )
		    {
		      GunTypeSelected = 4 ;
		    }
		  else
		    {

		      //--------------------
		      // Some sanity check again against clicks ouside of the bounds of the map...
		      //
		      if ( ! ( ( TargetSquare.x < 0 ) || ( TargetSquare.y < 0 ) ||
			       ( TargetSquare.x + 1 >= curShip . AllLevels [ Me [ 0 ] . pos . z ] -> xlen ) ||
			       ( TargetSquare.y + 1 >= curShip . AllLevels [ Me [ 0 ] . pos . z ] -> ylen ) ) )
			{
			  MapValue = curShip . AllLevels [ Me [ 0 ] . pos . z ] -> map [ TargetSquare.y ] [ TargetSquare.x ] ;
			  DebugPrintf ( 0 , "Map value found at click location: %d. " , MapValue );
			  
			  // if ( ( MapValue == BLOCK1 ) || ( GunTypeSelected > 0 ) )
			  if ( MapValue == BLOCK1 ) 
			    {
			      if ( GunTypeSelected > 0 ) 
				{
				  switch ( GunTypeSelected )
				    {
				    case 1:
				      curShip . AllLevels [ Me [ 0 ] . pos . z ] -> map [ TargetSquare.y ] [ TargetSquare.x ] = AUTOGUN_R ;
				      break;
				    case 2:
				      curShip . AllLevels [ Me [ 0 ] . pos . z ] -> map [ TargetSquare.y ] [ TargetSquare.x ] = AUTOGUN_D ;
				      break;
				    case 3:
				      curShip . AllLevels [ Me [ 0 ] . pos . z ] -> map [ TargetSquare.y ] [ TargetSquare.x ] = AUTOGUN_L ;
				      break;
				    case 4:
				      curShip . AllLevels [ Me [ 0 ] . pos . z ] -> map [ TargetSquare.y ] [ TargetSquare.x ] = AUTOGUN_U ;
				      break;
				    default:
				      break;
				    }
				  PlayOnceNeededSoundSample ( "../effects/CONSOLE_Gun_Successfully_Installed_0.wav" , FALSE );			      
				  SelectedFunction = NO_FUNCTION;
				}
			      else
				{
				  PlayOnceNeededSoundSample ( "../effects/CONSOLE_Please_Specify_Gun_0.wav" , FALSE );
				}


			      GetAutoguns( curShip.AllLevels[ Me [ 0 ] . pos . z ]  );
			    }
			}
		    }
		}
	    }
	}

      //--------------------
      // From here on we only do display work, no more checking for which
      // buttons pressed or so.
      //

      ClearUserFenster();
      Assemble_Combat_Picture( ONLY_SHOW_MAP );
      
      ShowGenericButtonFromList ( MAP_EXIT_BUTTON );

      if ( SelectedFunction != UNLOCK_FUNCTION ) 
	{
	  if ( UnlockAllowed ) ShowGenericButtonFromList ( MAP_UNLOCK_BUTTON_GREEN );
	  else ShowGenericButtonFromList ( MAP_UNLOCK_BUTTON_RED );
	}
      else
	{
	  ShowGenericButtonFromList ( MAP_UNLOCK_BUTTON_YELLOW );
	}

      if ( SelectedFunction != GUNOFF_FUNCTION ) 
	{
	  if ( GunOffAllowed ) 
	    {
	      ShowGenericButtonFromList ( MAP_GUNOFF_BUTTON_GREEN );
	    }
	  else 
	    {
	      ShowGenericButtonFromList ( MAP_GUNOFF_BUTTON_RED );
	    }
	}
      else
	{
	  ShowGenericButtonFromList ( MAP_GUNOFF_BUTTON_YELLOW );
	}

      if ( SelectedFunction != GUNON_FUNCTION ) 
	{
	  if ( GunOnAllowed ) 
	    {
	      ShowGenericButtonFromList ( MAP_GUNON_BUTTON_GREEN );
	    }
	  else 
	    {
	      ShowGenericButtonFromList ( MAP_GUNON_BUTTON_RED );
	    }
	}
      else
	{
	  ShowGenericButtonFromList ( MAP_GUNON_BUTTON_YELLOW );
	}

      if ( ReadEmailAllowed ) ShowGenericButtonFromList ( MAP_READ_EMAIL_GREEN_BUTTON );
      else ShowGenericButtonFromList ( MAP_READ_EMAIL_RED_BUTTON );

      //--------------------
      // Now we print out the autocannon type buttons to allow for selection 
      // of a new autocannon direction setting.
      //
      if ( SelectedFunction == GUNON_FUNCTION )
	{
	  ShowGenericButtonFromList ( MAP_GUN_TYPE_1_BUTTON_RED );
	  ShowGenericButtonFromList ( MAP_GUN_TYPE_2_BUTTON_RED );
	  ShowGenericButtonFromList ( MAP_GUN_TYPE_3_BUTTON_RED );
	  ShowGenericButtonFromList ( MAP_GUN_TYPE_4_BUTTON_RED );
	  switch ( GunTypeSelected )
	    {
	    case 1:
	      ShowGenericButtonFromList ( MAP_GUN_TYPE_1_BUTTON_YELLOW );
	      break;
	    case 2:
	      ShowGenericButtonFromList ( MAP_GUN_TYPE_2_BUTTON_YELLOW );
	      break;
	    case 3:
	      ShowGenericButtonFromList ( MAP_GUN_TYPE_3_BUTTON_YELLOW );
	      break;
	    case 4:
	      ShowGenericButtonFromList ( MAP_GUN_TYPE_4_BUTTON_YELLOW );
	      break;
	    default:
	    case (-1):
	      break;
	    }
	}

      //--------------------
      // Now we print out the energy ration request button and energy amout
      // that can be obtained.
      //
      if ( !EnergyRate ) ShowGenericButtonFromList ( MAP_REQUEST_ENERGY_RATION_RED_BUTTON );
      else ShowGenericButtonFromList ( MAP_REQUEST_ENERGY_RATION_GREEN_BUTTON );
      sprintf( EnergyRationString , "%d" , EnergyRate );
      PutString ( Screen , AllMousePressButtons [ MAP_REQUEST_ENERGY_RATION_GREEN_BUTTON ] . button_rect . x +
		  AllMousePressButtons [ MAP_REQUEST_ENERGY_RATION_GREEN_BUTTON ] . button_rect . w + 10 , 
		  AllMousePressButtons [ MAP_REQUEST_ENERGY_RATION_GREEN_BUTTON ] . button_rect . y + 
		  ( ( AllMousePressButtons [ MAP_REQUEST_ENERGY_RATION_GREEN_BUTTON ] . button_rect . h - 
		      FontHeight ( GetCurrentFont() ) ) / 2 ) , EnergyRationString ); 


      //--------------------
      // Now we print out the currently selected password AND
      // the currently selected security clearance.
      //
      
      PutSecurityButtonsAndClearance ( ClearanceIndex ) ;

      PutPasswordButtonsAndPassword ( PasswordIndex ) ;

      SDL_Flip (Screen);

      LeftMouseWasPressed = axis_is_active;

    }
  while (EscapePressed());

  Me [ 0 ] . pos . x = tmp . x ;
  Me [ 0 ] . pos . y = tmp . y ;

}; // void ShowDeckMap( ... )

/* ---------------------------------------------------------------------- 
 * This function checks if a given level is already empty.  If this is 
 * the case, it will be changed to the 'lights off' tileset, cause in
 * the original game the lights went off when a level was cleared.
 * ---------------------------------------------------------------------- */
int
LevelEmpty (void)
{
  int i;
  int levelnum = CurLevel->levelnum;

  if (CurLevel->empty)
    return TRUE;

  for (i = 0; i < NumEnemys; i++)
    {
      if ((AllEnemys[i].Status != OUT)
	  && (AllEnemys[i].pos.z == levelnum))
	return FALSE;
    }

  CurLevel->empty = TRUE;
  Me[0].Experience += DECKCOMPLETEBONUS;
  ShowScore += DECKCOMPLETEBONUS;

  if (ShipEmpty ())
    ThouArtVictorious ();

  return TRUE;
}

/* ----------------------------------------------------------------------
 * This function checks if the current ship is empty, cause in this case
 * in the old paradroid game, you had won.
 * ---------------------------------------------------------------------- */
int
ShipEmpty (void)
{
  int i;

  for (i = 0; i < curShip.num_levels; i++)
    {
      if (curShip.AllLevels[i] == NULL)
	continue;

      if (!((curShip.AllLevels[i])->empty))
	return (FALSE);
    }
  return (TRUE);
}; // int ShipEmpty (void)

/* ----------------------------------------------------------------------
 * This function fills the whole User_Rect with color 0 = black.
 * ---------------------------------------------------------------------- */
void
ClearUserFenster (void)
{
  SDL_Rect tmp;
  
  Copy_Rect (User_Rect, tmp)

  SDL_FillRect( Screen , &tmp, 0 );

}; // void ClearUserFenster( void )

#undef _ship_c
