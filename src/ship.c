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


#define UP_BUTTON_WIDTH 50
#define DOWN_BUTTON_WIDTH UP_BUTTON_WIDTH
#define LEFT_BUTTON_WIDTH 100
#define RIGHT_BUTTON_WIDTH LEFT_BUTTON_WIDTH
#define UP_BUTTON_HEIGHT 100
#define DOWN_BUTTON_HEIGHT UP_BUTTON_HEIGHT
#define LEFT_BUTTON_HEIGHT 50
#define RIGHT_BUTTON_HEIGHT LEFT_BUTTON_HEIGHT

#define UP_BUTTON_X 580
#define UP_BUTTON_Y 180
#define DOWN_BUTTON_X UP_BUTTON_X
#define DOWN_BUTTON_Y (UP_BUTTON_Y + UP_BUTTON_HEIGHT * 1.5 )
#define LEFT_BUTTON_X 300
#define LEFT_BUTTON_Y 420
#define RIGHT_BUTTON_X 450
#define RIGHT_BUTTON_Y LEFT_BUTTON_Y

int NoKeyPressed (void);

// EXTERN SDL_Surface *console_pic;
SDL_Surface *console_pic = NULL ;


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
  Fill_Rect (User_Rect, lift_bg_color);
  DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE );      

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
 * This function does all the codepanel duties, like bring up the 
 * codepanel screen, ask for a number, compare it to the real codepanel
 * number and unlock the door/perform the desired action upon right
 * code, else just say no and logoff.
 * ----------------------------------------------------------------------*/
void
EnterItemIdentificationBooth (void)
{
  int Weiter=FALSE;
  char* RequestString;
  SDL_Surface* Background;
  SDL_Rect Chat_Window;

  while (SpacePressed());
  
  Chat_Window.x=242;
  Chat_Window.y=100;
  Chat_Window.w=380;
  Chat_Window.h=314;

  //--------------------
  // First we arrange the background, so that everything looks fine,
  // similar as in the DroidChat.
  //

  Activate_Conservative_Frame_Computation( );
  Background = IMG_Load( find_file ( "chat_test.jpg" , GRAPHICS_DIR, FALSE ) );
  if ( Background == NULL )
    {
      printf("\n\nChatWithFriendlyDroid: ERROR LOADING FILE!!!!  Error code: %s " , SDL_GetError() );
      Terminate(ERR);
    }

  SDL_BlitSurface( Background , NULL , Screen , NULL );
  SDL_Flip( Screen );
  
  SetCurrentFont( Para_BFont );

  DisplayTextWithScrolling ( 
			    "United Machines Corporation Item Identification Booth\n\nItems: " , 
			    Chat_Window.x , Chat_Window.y , &Chat_Window , Background );

  //--------------------
  // Now we read in the code the user has.
  //

  while ( !Weiter )
    {
      RequestString = GetChatWindowInput( Background , &Chat_Window );
      Weiter = TRUE;

    }
}; // void EnterItemIdentificationBooth( void );

/* ----------------------------------------------------------------------
 * This function does all the codepanel duties, like bring up the 
 * codepanel screen, ask for a number, compare it to the real codepanel
 * number and unlock the door/perform the desired action upon right
 * code, else just say no and logoff.
 * ----------------------------------------------------------------------*/
void
EnterCodepanel (void)
{
  int map_x;
  int map_y;
  int Codepanel_Index;
  int Weiter=FALSE;
  char* RequestString;
  SDL_Surface* Background;
  SDL_Rect Chat_Window;

  while (SpacePressed());
  
  Chat_Window.x=242;
  Chat_Window.y=100;
  Chat_Window.w=380;
  Chat_Window.h=314;

  //--------------------
  // A codepanel has been entered.  Therefore we try to find the appropriate
  // code word specified in the code panel array.  Let's see if we can find
  // a matching index:
  //
  for ( Codepanel_Index = 0 ; Codepanel_Index < MAX_CODEPANELS_PER_LEVEL ; Codepanel_Index ++ )
    {
      if ( ( ( (int) rintf( Me[0].pos.x ) ) == CurLevel->CodepanelList[ Codepanel_Index ].x ) &&
	   ( ( (int) rintf( Me[0].pos.y ) ) == CurLevel->CodepanelList[ Codepanel_Index ].y ) )
	break;
    }
  if ( Codepanel_Index == MAX_CODEPANELS_PER_LEVEL )
    {
      DisplayText ( "\nLast codepanel entry used or no codepanel entry present!!" , 100 , 100 , &User_Rect );
      SDL_Flip ( Screen );
      getchar_raw();
      Codepanel_Index = 0;
    }

  //--------------------
  // First we arrange the background, so that everything looks fine,
  // similar as in the DroidChat.
  //

  Activate_Conservative_Frame_Computation( );
  Background = IMG_Load( find_file ( "chat_test.jpg" , GRAPHICS_DIR, FALSE ) );
  if ( Background == NULL )
    {
      printf("\n\nChatWithFriendlyDroid: ERROR LOADING FILE!!!!  Error code: %s " , SDL_GetError() );
      Terminate(ERR);
    }

  SDL_BlitSurface( Background , NULL , Screen , NULL );
  SDL_Flip( Screen );
  
  SetCurrentFont( Para_BFont );

  DisplayTextWithScrolling ( 
			    "MegaSoft Security Access Control System\n\nEnter Code: " , 
			    Chat_Window.x , Chat_Window.y , &Chat_Window , Background );

  //--------------------
  // Now we read in the code the user has.
  //

  while ( !Weiter )
    {
      RequestString = GetChatWindowInput( Background , &Chat_Window );

      //--------------------
      // the quit command is always simple and clear.  We just need to end
      // the communication function. hehe.
      //
      if ( ( !strcmp ( RequestString , "quit" ) ) || 
	   ( !strcmp ( RequestString , "bye" ) ) ||
	   ( !strcmp ( RequestString , "logout" ) ) ||
	   ( !strcmp ( RequestString , "logoff" ) ) ||
	   ( !strcmp ( RequestString , "" ) ) ) 
	{
	  Me[0].TextVisibleTime=0;
	  Me[0].TextToBeDisplayed="Logging out.  Bye...";
	  return;
	}

      if ( !strcmp ( RequestString , CurLevel->CodepanelList[ Codepanel_Index ].Secret_Code ) )
	{
	  Me[0].TextVisibleTime=0;
	  Me[0].TextToBeDisplayed="Wow! I've hacked this terminal.  Cool!";
	  map_x = (int) rintf( Me[0].pos.x );
	  map_y = (int) rintf( Me[0].pos.y );
	  switch( CurLevel->map[ map_y ] [ map_x + 1] )
	    {
	    case LOCKED_H_ZUTUERE:
	      CurLevel->map[ map_y ] [ map_x + 1 ] = H_ZUTUERE;
	      break;
	    case LOCKED_V_ZUTUERE:
	      CurLevel->map[ map_y ] [ map_x + 1 ] = V_ZUTUERE;
	      break;
	    default:
	      break;
	    }
	  switch( CurLevel->map[ map_y ] [ map_x - 1 ] )
	    {
	    case LOCKED_H_ZUTUERE:
	      CurLevel->map[ map_y ] [ map_x - 1 ] = H_ZUTUERE;
	      break;
	    case LOCKED_V_ZUTUERE:
	      CurLevel->map[ map_y ] [ map_x - 1 ] = V_ZUTUERE;
	      break;
	    default:
	      break;
	    }
	  switch( CurLevel->map[ map_y + 1 ] [ map_x ] )
	    {
	    case LOCKED_H_ZUTUERE:
	      CurLevel->map[ map_y + 1 ] [ map_x ] = H_ZUTUERE;
	      break;
	    case LOCKED_V_ZUTUERE:
	      CurLevel->map[ map_y + 1 ] [ map_x ] = V_ZUTUERE;
	      break;
	    default:
	      break;
	    }
	  switch( CurLevel->map[ map_y - 1 ] [ map_x ] )
	    {
	    case LOCKED_H_ZUTUERE:
	      CurLevel->map[ map_y - 1 ] [ map_x ] = H_ZUTUERE;
	      break;
	    case LOCKED_V_ZUTUERE:
	      CurLevel->map[ map_y - 1 ] [ map_x ] = V_ZUTUERE;
	      break;
	    default:
	      break;
	    }
	  GetDoors( CurLevel );
	  DisplayTextWithScrolling ( 
				    "\nAccess granted ! ! " , 
				    -1 , -1 , &Chat_Window , Background );
	  SDL_Flip( Screen );
	  while (!SpacePressed());
	  while (SpacePressed());
	  
	  return;
	}

      DisplayTextWithScrolling ( 
				"Access denied." , 
				-1 , -1 , &Chat_Window , Background );

    }

}; // void EnterCodepanel (void)

/* ----------------------------------------------------------------------
 * This function does all console duties.
 * This means the following:
 * 	2	* Show a small-scale plan of the current deck
 *	3	* Show a side-elevation on the ship
 *	1	* Give all available data on lower druid types
 *	0	* Reenter the game without squashing the colortable
 * ---------------------------------------------------------------------- */
void
EnterKonsole (void)
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

  Switch_Background_Music_To (CONSOLE_BACKGROUND_MUSIC_SOUND);

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
	}
      if ( UpPressed() || MouseWheelUpPressed() ) 
	{
	  if (menu_pos > 0) menu_pos--;
	  while (UpPressed());
	}
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
	      getchar_raw();
	      SetCombatScaleTo( 1 );
	      break;
	    case 3:
	      ClearGraphMem();
	      ShowLifts (CurLevel->levelnum, -1);
	      getchar_raw();
	      break;
	    default:
	      DebugPrintf(0,"\nError in Console: menu-pos out of bounds \n");
	      Terminate(-1);
	      break;
	    } // switch menu_pos 
	}
      

      /*

      switch (key)
	{
	case SDLK_DOWN:
	  if (menu_pos < 3) menu_pos++;
	  break;
	case SDLK_UP:
	  if (menu_pos > 0) menu_pos--;
	  break;

	case SDLK_SPACE:
	case SDLK_RETURN:
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
	      getchar_raw();
	      SetCombatScaleTo( 1 );
	      break;
	    case 3:
	      ClearGraphMem();
	      ShowLifts (CurLevel->levelnum, -1);
	      getchar_raw();
	      break;
	    default:
	      DebugPrintf(0,"\nError in Console: menu-pos out of bounds \n");
	      Terminate(-1);
	      break;
	    } // switch menu_pos 
	  break;

	case SDLK_ESCAPE:
	  finished = TRUE;
	  break;

	default:
	  break;
	} // switch key 

      */

    } /* while (!finished) */

  Me[0].status = MOBILE;
  ClearGraphMem ( );
  DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE );
  SDL_Flip( Screen );

  while (SpacePressed ());

  // Switch_Background_Music_To ( COMBAT_BACKGROUND_MUSIC_SOUND );
  Switch_Background_Music_To ( CurLevel->Background_Song_Name );

  return;

} // void EnterKonsole(void)

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
 * This function checks if a given screen position lies within the 
 * UP button of the droid show or not.
 * ---------------------------------------------------------------------- */
int
CursorIsOnUPButton( int x , int y )
{
  if ( x > UP_BUTTON_X + UP_BUTTON_WIDTH  ) return ( FALSE );
  if ( x < UP_BUTTON_X                     ) return ( FALSE );
  if ( y > UP_BUTTON_Y + UP_BUTTON_HEIGHT ) return ( FALSE );
  if ( y < UP_BUTTON_Y                     ) return ( FALSE );
  return ( TRUE );
}; // int CursorIsOnStrButton( int x , int y )

/* ----------------------------------------------------------------------
 * This function checks if a given screen position lies within the 
 * DOWN button of the droid show or not.
 * ---------------------------------------------------------------------- */
int
CursorIsOnDOWNButton( int x , int y )
{
  if ( x > DOWN_BUTTON_X + DOWN_BUTTON_WIDTH  ) return ( FALSE );
  if ( x < DOWN_BUTTON_X                     ) return ( FALSE );
  if ( y > DOWN_BUTTON_Y + DOWN_BUTTON_HEIGHT ) return ( FALSE );
  if ( y < DOWN_BUTTON_Y                     ) return ( FALSE );
  return ( TRUE );
}; // int CursorIsOnStrButton( int x , int y )

/* ----------------------------------------------------------------------
 * This function checks if a given screen position lies within the 
 * LEFT button of the droid show or not.
 * ---------------------------------------------------------------------- */
int
CursorIsOnLEFTButton( int x , int y )
{
  if ( x > LEFT_BUTTON_X + LEFT_BUTTON_WIDTH  ) return ( FALSE );
  if ( x < LEFT_BUTTON_X                     ) return ( FALSE );
  if ( y > LEFT_BUTTON_Y + LEFT_BUTTON_HEIGHT ) return ( FALSE );
  if ( y < LEFT_BUTTON_Y                     ) return ( FALSE );
  return ( TRUE );
}; // int CursorIsOnStrButton( int x , int y )

/* ----------------------------------------------------------------------
 * This function checks if a given screen position lies within the 
 * UP button of the droid show or not.
 * ---------------------------------------------------------------------- */
int
CursorIsOnRIGHTButton( int x , int y )
{
  if ( x > RIGHT_BUTTON_X + RIGHT_BUTTON_WIDTH  ) return ( FALSE );
  if ( x < RIGHT_BUTTON_X                     ) return ( FALSE );
  if ( y > RIGHT_BUTTON_Y + RIGHT_BUTTON_HEIGHT ) return ( FALSE );
  if ( y < RIGHT_BUTTON_Y                     ) return ( FALSE );
  return ( TRUE );
}; // int CursorIsOnStrButton( int x , int y )


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
  static SDL_Surface *UP_ButtonImage = NULL;
  static SDL_Surface *DOWN_ButtonImage = NULL;
  static SDL_Surface *LEFT_ButtonImage = NULL;
  static SDL_Surface *RIGHT_ButtonImage = NULL;
  SDL_Surface *tmp;
  static SDL_Rect UP_Button_Rect;
  static SDL_Rect DOWN_Button_Rect;
  static SDL_Rect LEFT_Button_Rect;
  static SDL_Rect RIGHT_Button_Rect;
  char* fpath;

  // --------------------
  // Some things like the loading of the button images
  // need to be done only once at the first call of this
  // function. 
  //
  if ( UP_ButtonImage == NULL )
    {
      fpath = find_file ( "UPButton.png" , GRAPHICS_DIR, FALSE);
      tmp = IMG_Load( fpath );
      UP_ButtonImage = SDL_DisplayFormat( tmp );
      SDL_FreeSurface( tmp );

      fpath = find_file ( "DOWNButton.png" , GRAPHICS_DIR, FALSE);
      tmp = IMG_Load( fpath );
      DOWN_ButtonImage = SDL_DisplayFormat( tmp );
      SDL_FreeSurface( tmp );

      fpath = find_file ( "LEFTButton.png" , GRAPHICS_DIR, FALSE);
      tmp = IMG_Load( fpath );
      LEFT_ButtonImage = SDL_DisplayFormat( tmp );
      SDL_FreeSurface( tmp );

      fpath = find_file ( "RIGHTButton.png" , GRAPHICS_DIR, FALSE);
      tmp = IMG_Load( fpath );
      RIGHT_ButtonImage = SDL_DisplayFormat( tmp );
      SDL_FreeSurface( tmp );
    }

  UP_Button_Rect.x = UP_BUTTON_X;
  UP_Button_Rect.y = UP_BUTTON_Y;
  
  DOWN_Button_Rect.x = DOWN_BUTTON_X;
  DOWN_Button_Rect.y = DOWN_BUTTON_Y;
  
  LEFT_Button_Rect.x = LEFT_BUTTON_X;
  LEFT_Button_Rect.y = LEFT_BUTTON_Y;
  
  RIGHT_Button_Rect.x = RIGHT_BUTTON_X;
  RIGHT_Button_Rect.y = RIGHT_BUTTON_Y;
  
  SDL_BlitSurface( UP_ButtonImage , NULL , Screen , &UP_Button_Rect );
  SDL_BlitSurface( DOWN_ButtonImage , NULL , Screen , &DOWN_Button_Rect );
  SDL_BlitSurface( LEFT_ButtonImage , NULL , Screen , &LEFT_Button_Rect );
  SDL_BlitSurface( RIGHT_ButtonImage , NULL , Screen , &RIGHT_Button_Rect );

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
  bool key_pressed = FALSE;
  static int WasPressed = FALSE ;

  droidtype = Me[0].type;
  page = 0;

  show_droid_info (droidtype, page , TRUE );

  while (!finished)
    {
      usleep ( 2 );
      if (key_pressed)
	{
	  show_droid_info (droidtype, page , TRUE );
	  key_pressed = FALSE;
	}

      if (SpacePressed() || EscapePressed() || axis_is_active )
	{
	  if ( CursorIsOnUPButton( GetMousePos_x() + 16 , GetMousePos_y() + 16 ) && axis_is_active && !WasPressed )
	    {
	      if ( droidtype < Me[0].type) droidtype ++;	    
	      key_pressed = TRUE;
	      MoveMenuPositionSound();
	    }
	  else if ( CursorIsOnDOWNButton( GetMousePos_x() + 16 , GetMousePos_y() + 16 ) && axis_is_active && !WasPressed )
	    {
	      if (droidtype > 0) droidtype --;	      
	      key_pressed = TRUE;
	      MoveMenuPositionSound();
	    }
	  else if ( CursorIsOnRIGHTButton( GetMousePos_x() + 16 , GetMousePos_y() + 16 ) && axis_is_active && !WasPressed )
	    {
	      MoveMenuPositionSound();
	      if (page < 2) page ++;
	      key_pressed = TRUE;
	    }
	  else if ( CursorIsOnLEFTButton( GetMousePos_x() + 16 , GetMousePos_y() + 16 ) && axis_is_active && !WasPressed )
	    {
	      MoveMenuPositionSound();
	      if (page > 0) page --;
	      key_pressed = TRUE;
	    }

	  if ( ! CursorIsOnUPButton( GetMousePos_x() + 16 , GetMousePos_y() + 16 ) &&
	       ! CursorIsOnDOWNButton( GetMousePos_x() + 16 , GetMousePos_y() + 16 ) &&
	       ! CursorIsOnLEFTButton( GetMousePos_x() + 16 , GetMousePos_y() + 16 ) &&
	       ! CursorIsOnRIGHTButton( GetMousePos_x() + 16 , GetMousePos_y() + 16 ) )
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
	  if ( droidtype < Me[0].type) droidtype ++;
	  key_pressed = TRUE;
	}
      if (DownPressed() || MouseWheelDownPressed())
	{
	  MoveMenuPositionSound();
	  while (DownPressed());
	  if (droidtype > 0) droidtype --;
	  key_pressed = TRUE;
	}
      if (RightPressed() )
	{
	  MoveMenuPositionSound();
	  while (RightPressed());
	  if (page < 2) page ++;
	  key_pressed = TRUE;
	}
      if (LeftPressed() )
	{
	  MoveMenuPositionSound();
	  while (LeftPressed());
	  if (page > 0) page --;
	  key_pressed = TRUE;
	}

    } /* while !finished */

  return;
}; // void GreatDroidShow( void ) 

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
  DisplayBanner (NULL, NULL,  BANNER_NO_SDL_UPDATE | BANNER_FORCE_UPDATE );

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
Brain : %s", Druidmap[droidtype].druidname, Classname[Druidmap[Me[0].type].class],
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

  SDL_Flip (Screen);

} /* show_droid_info */


/* -----------------------------------------------------------------
 * Displays the concept view of Level "deck" in Userfenster
 * 	  
 * Note: we no longer wait here for a key-press, but return
 *       immediately 
 * ----------------------------------------------------------------- */
void
ShowDeckMap (Level deck)
{
  finepoint tmp;
  tmp.x=Me[0].pos.x;
  tmp.y=Me[0].pos.y;

  ClearUserFenster ();
  Me[0].pos.x = CurLevel->xlen/2;
  Me[0].pos.y = CurLevel->ylen/2;

  SetCombatScaleTo( 0.25 );

  Assemble_Combat_Picture( ONLY_SHOW_MAP );

  SDL_Flip (Screen);

  Me[0].pos.x=tmp.x;
  Me[0].pos.y=tmp.y;

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
