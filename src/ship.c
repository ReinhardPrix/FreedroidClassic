/*----------------------------------------------------------------------
 *
 * Desc: the konsole- and lift functions
 *
 *----------------------------------------------------------------------*/

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
#define _ship_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"
#include "text.h"
#include "ship.h"
#include "SDL_rotozoom.h"

int NoKeyPressed (void);

int ConsoleMenuPos=0;


/*-----------------------------------------------------------------
 * @Desc: does all the work when we enter a lift
 * 
 *-----------------------------------------------------------------*/
void
EnterLift (void)
{
  int i;
  int curLevel;
  int curLift, upLift, downLift, liftrow;

  DebugPrintf (2, "\nvoid EnterLiftator(void): Function call confirmed.");

  /* Prevent distortion of framerate by the delay coming from 
   * the time spend in the menu. */
  Activate_Conservative_Frame_Computation();

  /* Prevent the influ from coming out of the lift in transfer mode
   * by turning off transfer mode as soon as the influ enters the lift */
  Me.status= ELEVATOR;

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

  /* Warten, bis User Feuer auslaesst */
  while (SpacePressed ()) ;


  while (!SpacePressed ())
    {
      if (UpPressed () && !DownPressed ())
	if (upLift != -1)
	  {			/* gibt es noch einen Lift hoeher ? */
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

		/* Warten, bis user Taste auslaesst */
		MoveLiftSound ();
		while (UpPressed ()) ;
	      }
	  }			/* if uplevel */


      if (DownPressed () && !UpPressed ())
	if (downLift != -1)
	  {			/* gibt es noch einen Lift tiefer ? */
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

		/* Warten, bis User Taste auslaesst */
		MoveLiftSound ();
		while (DownPressed ()) ;
	      }
	  }			/* if downlevel */
    }				/* while !SpaceReleased */

  //--------------------
  // It might happen, that the influencer enters the elevator, but then decides to
  // come out on the same level where he has been before.  In this case of course there
  // is no need to reshuffle enemys or to reset influencers position.  Therefore, only
  // when a real level change has occured, we need to do real changes as below, where
  // we set the new level and set new position and initiate timers and all that...
  //
  if (curLevel != CurLevel->levelnum)
    {				/* wirklich neu ??? */
      int array_num = 0;
      Level tmp;

      /* Aktuellen Level setzen */
      while ((tmp = curShip.AllLevels[array_num]) != NULL)
	{
	  if (tmp->levelnum == curLevel)
	    break;
	  else
	    array_num++;
	}

      
      CurLevel = curShip.AllLevels[array_num];

      // redistribute the enemys around the level
      ShuffleEnemys ();

      // set the position of the influencer to the correct locatiohn
      Me.pos.x =
	curShip.AllLifts[curLift].x; 
      Me.pos.y =
	curShip.AllLifts[curLift].y; 

      // We reset the time on this level and the position history
      Me.FramesOnThisLevel=0;
      // for ( i = 0 ; i < MAX_INFLU_POSITION_HISTORY ; i++ ) 
      // {
      // Me.Position_History[ i ].x = Me.pos.x;
      // Me.Position_History[ i ].y = Me.pos.y;
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

  Me.status = MOBILE;
  Me.TextVisibleTime=0;
  Me.TextToBeDisplayed=CurLevel->Level_Enter_Comment;

  DebugPrintf (2, "\nvoid EnterLift(void): Usual end of function reached.");
}	/* EnterLift */

/*-----------------------------------------------------------------
 * @Desc: show side-view of the ship, and hightlight the current 
 *        level + lift
 *
 *  if level==-1: don't highlight any level
 *  if liftrow==-1: dont' highlight any liftrows
 *
 *-----------------------------------------------------------------*/
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

} /* ShowLifts() */

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
      if ( ( ( (int) rintf( Me.pos.x ) ) == CurLevel->CodepanelList[ Codepanel_Index ].x ) &&
	   ( ( (int) rintf( Me.pos.y ) ) == CurLevel->CodepanelList[ Codepanel_Index ].y ) )
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
	  Me.TextVisibleTime=0;
	  Me.TextToBeDisplayed="Logging out.  Bye...";
	  return;
	}

      if ( !strcmp ( RequestString , CurLevel->CodepanelList[ Codepanel_Index ].Secret_Code ) )
	{
	  Me.TextVisibleTime=0;
	  Me.TextToBeDisplayed="Wow! I've hacked this terminal.  Cool!";
	  map_x = (int) rintf( Me.pos.x );
	  map_y = (int) rintf( Me.pos.y );
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
      

  // while ( !SpacePressed() );

}; // void EnterCodepanel (void)

/*@Function============================================================
@Desc: EnterKonsole(): does all konsole- duties
This function runs the consoles. This means the following duties:
	2	* Show a small-scale plan of the current deck
	3	* Show a side-elevation on the ship
	1	* Give all available data on lower druid types
	0	* Reenter the game without squashing the colortable
@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
EnterKonsole (void)
{
  int ReenterGame = 0;
  int TasteOK;
  // Prevent distortion of framerate by the delay coming from 
  // the time spend in the menu.
  Activate_Conservative_Frame_Computation();

  Me.status = CONSOLE;

  SetCurrentFont( Para_BFont );

  Switch_Background_Music_To (CONSOLE_BACKGROUND_MUSIC_SOUND);

  while (SpacePressed ());  /* wait for user to release Space */

  ConsoleMenuPos=0;

  /* Gesamtkonsolenschleife */

  while (!ReenterGame)
    {

      PaintConsoleMenu ();
      SDL_Flip (Screen);

      /* Nichts tun bis eine vern"unftige Taste gedr"uckt wurde */
      TasteOK = 0;
      while (!TasteOK)
	{
	  if (UpPressed ())
	    {
	      ConsoleMenuPos--;
	      TasteOK = 1;
	    }
	  if (DownPressed ())
	    {
	      ConsoleMenuPos++;
	      TasteOK = 1;
	    }
	  if ( SpacePressed () )
	    TasteOK = 1;
	  if ( EscapePressed() )
	    {
	      TasteOK = 1;
	      ReenterGame = TRUE;
	      while ( EscapePressed() );
	    }
	}

      /* Verhindern, da"s der Menucursor das Menu verl"a"st */
      if (ConsoleMenuPos < 0)
	ConsoleMenuPos = 0;
      if (ConsoleMenuPos > 3)
	ConsoleMenuPos = 3;

      /* gew"ahlte Menupunkte betreten */
      if ((ConsoleMenuPos == 0) & (SpacePressed ()))
	ReenterGame = TRUE;
      if ((ConsoleMenuPos == 1) & (SpacePressed ()))
	GreatDruidShow ();
      if ((ConsoleMenuPos == 2) & (SpacePressed ()))
	{
	  ShowDeckMap (CurLevel);
	  /* this is not very elegant at the moment, but it works ok.. */
	  while ( SpacePressed() );  /* wait for space-release */
	  while (!SpacePressed () ); /* and wait for another space before leaving */
	  // Now that we leave, we restore the combat screen scaling factor..
	  // if ( CurrentCombatScaleFactor == 1 ) ReInitPictures();
	  // else {
	  // if ( CurrentCombatScaleFactor != 0.5 ) SetCombatScaleTo( CurrentCombatScaleFactor );
	  // }
	  SetCombatScaleTo( 1 );
	  while ( SpacePressed() ); /* but also wait for the release before going on..*/
	}
      if ((ConsoleMenuPos == 3) & (SpacePressed ()))
	{
	  while (SpacePressed ());
	  ClearGraphMem();
	  ShowLifts (CurLevel->levelnum, -1);
	  while (!SpacePressed ());
	  while (SpacePressed ());
	}

      while (DownPressed ());
      while (UpPressed ());
    }				/* (while !ReenterGane) */

  Me.status = MOBILE;
  ClearGraphMem ( );
  DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE );
  SDL_Flip( Screen );

  while (SpacePressed ());

  // Switch_Background_Music_To ( COMBAT_BACKGROUND_MUSIC_SOUND );
  Switch_Background_Music_To ( CurLevel->Background_Song_Name );

  return;

} // void EnterKonsole(void)

/*-----------------------------------------------------------------
 * @Desc: diese Funktion zeigt die m"oglichen Auswahlpunkte des Menus
 *    Sie soll die Schriftfarben nicht ver"andern
 *
 *  NOTE: this function does not actually _display_ anything yet,
 *        it just prepares the display, so you need
 *        to call SDL_Flip() to display the result!
 *
 *
 *-----------------------------------------------------------------*/
void
PaintConsoleMenu (void)
{
  char MenuText[1000];

  SDL_Rect SourceRectangle;
  SDL_Rect TargetRectangle;

  ClearGraphMem ();

  SDL_SetClipRect ( Screen , NULL );
  DisplayImage ( find_file( NE_CONSOLE_BG_PIC1_FILE , GRAPHICS_DIR, FALSE) );

  DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE );

  strcpy (MenuText, "Unit type ");
  strcat (MenuText, Druidmap[Me.type].druidname);
  strcat (MenuText, " - ");
  strcat (MenuText, Classname[Druidmap[Me.type].class]);
  strcat (MenuText, "\nAccess granted.\nArea : ");
  strcat (MenuText, curShip.AreaName ); // Shipnames[ThisShip]);
  strcat (MenuText, "\nDeck : ");
  strcat (MenuText, CurLevel->Levelname );
  strcat (MenuText, "\n\nAlert: ");
  strcat (MenuText, Alertcolor[Alert]);

  DisplayText (MenuText, Cons_Text_Rect.x, Cons_Text_Rect.y, &Cons_Text_Rect);

  /*
   * Hier werden die Icons des Menus ausgegeben
   *
   */

  SourceRectangle.x=(CONS_MENU_LENGTH+2)*ConsoleMenuPos;
  SourceRectangle.y=0;
  SourceRectangle.w=CONS_MENU_LENGTH;
  SourceRectangle.h=CONS_MENU_HEIGHT;
  Copy_Rect (Cons_Menu_Rect, TargetRectangle);
  SDL_BlitSurface( console_pic , &SourceRectangle , Screen , &TargetRectangle );

  return;
}	// PaintConsoleMenu ()

/*@Function============================================================
@Desc: Zeigt alle erlaubten Roboter.

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
GreatDruidShow (void)
{
  char InfoText[1000];
  int Infodroid;
  int LeaveThisInformationPart;
  char PassOn = 0;

  DebugPrintf (2, "\nvoid GreadDruidShow(void): Function call confirmed.");

  // wait for user to release the space, that has been pressed for transfer-mode
  while (SpacePressed ()) ;

  for (Infodroid = Me.type; Infodroid > -1;)
    {

      //--------------------
      // At first we print out the most general information about this droid
      //
      
      LeaveThisInformationPart = FALSE;
      while ( !LeaveThisInformationPart )
	{

	  SDL_SetClipRect ( Screen , NULL );
	  DisplayImage ( find_file( NE_CONSOLE_BG_PIC2_FILE , GRAPHICS_DIR, FALSE) );
	  DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE );

	  ShowRobotPicture (Cons_Menu_Rect.x, Cons_Menu_Rect.y,  Infodroid );

	  sprintf( InfoText, 
		   "Unit type %s - %s\n\
Entry : %d\nClass : %s\nHeight : %f\nWeight: %f \nDrive : %s \nBrain : %s",  
		   Druidmap[Me.type].druidname,
		   Classname[Druidmap[Me.type].class],
		   Infodroid+1, 
		   Classes[Druidmap[Infodroid].class],
		   Druidmap[Infodroid].height,
		   Druidmap[Infodroid].weight,
		   ItemMap [ Druidmap[ Infodroid ].drive_item ].ItemName,
		   Brainnames[ Druidmap[Infodroid].brain ]);

	  DisplayText (InfoText, Cons_Text_Rect.x, Cons_Text_Rect.y, &Cons_Text_Rect);
	  SDL_Flip (Screen);

	  PassOn = 0;
	  while (!PassOn)
	    {
	      if ( EscapePressed () )
		{
		  PassOn = 1;
		  while ( EscapePressed() );
		  return;
		}

	      if (UpPressed ())
		{
		  PassOn = 1;
		  Infodroid--;
		  // Einem zu schnellen Weiterbl"attern vorbeugen
		  while (UpPressed () || DownPressed ()) ;
		}
	      
	      if (DownPressed ())
		{
		  Infodroid++;
		  PassOn = 1;
		  // Einem zu schnellen Weiterbl"attern vorbeugen
		  while (UpPressed () || DownPressed ()) ;
		}

	      if (Infodroid > Me.type)
		Infodroid = DRUID001;
	      if (Infodroid < DRUID001)
		Infodroid = Me.type;
	      
	      if ( (LeftPressed ()) || (RightPressed ()))
		{
		  PassOn = 1;
		  while ((RightPressed ()) || (LeftPressed ()) || (UpPressed ())
			 || (DownPressed ())) ;
		  LeaveThisInformationPart = TRUE;
		}
	      if (SpacePressed ())
		{
		  while (SpacePressed ()) ;
		  DebugPrintf (2, "\nvoid GreadDruidShow(void): Alternate end of function reached via Space1.");
		  return;
		}
	    }
	}; // while !LeaveThisInformationPart


      //--------------------
      // Next we print the equipment this droid comes with, which includes the sensors he has,
      // and the fake 'armament' from the original game.
      //

      LeaveThisInformationPart = FALSE;
      while ( !LeaveThisInformationPart )
	{
	  ClearUserFenster( );
	  SDL_SetClipRect ( Screen , NULL );
	  DisplayImage ( find_file( NE_CONSOLE_BG_PIC2_FILE , GRAPHICS_DIR, FALSE) );
	  DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE );
	  
	  sprintf( InfoText , "Unit type %s - %s" , Druidmap[Infodroid].druidname , Classname[Druidmap[Infodroid].class] );
	  
	  DisplayText (InfoText, Cons_Rect.x, Cons_Rect.y, &Cons_Rect);
	  
	  ShowRobotPicture (Cons_Rect.x, Cons_Rect.y + 2 * FONTHOEHE, Infodroid);
	  
	  strcpy (InfoText, "Armamant : ");
	  // strcat (InfoText, Weaponnames[ Druidmap[Infodroid].armament ]);
	  strcat (InfoText, ItemMap[ Druidmap[Infodroid].weapon_item ].ItemName );
	  strcat (InfoText, "\nSensors  1: ");
	  strcat (InfoText, Sensornames[ Druidmap[Infodroid].sensor1 ]);
	  strcat (InfoText, "\n          2: ");
	  strcat (InfoText, Sensornames[ Druidmap[Infodroid].sensor2 ]);
	  strcat (InfoText, "\n          3: ");
	  strcat (InfoText, Sensornames[ Druidmap[Infodroid].sensor3 ]);
	  
	  DisplayText (InfoText, Cons_Rect.x, Cons_Rect.y + FontHeight (Menu_BFont),
		       &Cons_Rect);
	  
	  SDL_Flip (Screen);
	  
	  PassOn = 0;
	  while (!PassOn)
	    {
	      if ( EscapePressed () )
		{
		  PassOn = 1;
		  while ( EscapePressed() );
		  return;
		}

	      if (UpPressed ())
		{
		  PassOn = 1;
		  Infodroid--;
		  // Einem zu schnellen Weiterbl"attern vorbeugen
		  while (UpPressed () || DownPressed ()) ;
		}
	      
	      if (DownPressed ())
		{
		  Infodroid++;
		  PassOn = 1;
		  // Einem zu schnellen Weiterbl"attern vorbeugen
		  while (UpPressed () || DownPressed ()) ;
		}
	      
	      if (Infodroid > Me.type)
		Infodroid = DRUID001;
	      if (Infodroid < DRUID001)
		Infodroid = Me.type;
	      
	      if ( (RightPressed ()) || (LeftPressed ()) )
		{
		  LeaveThisInformationPart = TRUE;
		  PassOn = 1;
		  while ((RightPressed ()) || (LeftPressed ()) || (UpPressed ())
			 || (DownPressed ())) ;
		}
	      if (SpacePressed ())
		{
		  while (SpacePressed ()) ;
		  return;
		}
	    }
	}; // while !LeaveThisInformationPart
	  
	  
      /*
       * Ausgabe der Informationen bezuglich des Druidhintergrundes
       *
       */

      LeaveThisInformationPart = FALSE;
      while ( !LeaveThisInformationPart )
	{
	  SDL_SetClipRect ( Screen , NULL );
	  DisplayImage ( find_file( NE_CONSOLE_BG_PIC2_FILE , GRAPHICS_DIR, FALSE) );
	  DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE );
	  

	  sprintf( InfoText , "Unit type %s - %s" , Druidmap[Infodroid].druidname , Classname[Druidmap[Infodroid].class] );

	  DisplayText (InfoText, Cons_Rect.x, Cons_Rect.y, &Cons_Rect);

	  ShowRobotPicture (Cons_Rect.x, Cons_Rect.y + 2 * FONTHOEHE, Infodroid );

	  strcpy (InfoText, "Notes: ");
	  strcat (InfoText, Druidmap[Infodroid].notes);

	  DisplayText (InfoText, Cons_Rect.x, Cons_Rect.y + FontHeight (Menu_BFont),
		       &Cons_Rect);
	  
	  SDL_Flip (Screen);
      
	  PassOn = 0;
	  while (!PassOn)
	    {
	      if ( EscapePressed () )
		{
		  PassOn = 1;
		  while ( EscapePressed() );
		  return;
		}

	      if (UpPressed ())
		{
		  PassOn = 1;
		  Infodroid--;
		  // Einem zu schnellen Weiterbl"attern vorbeugen
		  while (UpPressed () || DownPressed ()) ;
		}
	      
	      if (DownPressed ())
		{
		  Infodroid++;
		  PassOn = 1;
		  // Einem zu schnellen Weiterbl"attern vorbeugen
		  while (UpPressed () || DownPressed ()) ;
		}
	      
	      if (Infodroid > Me.type)
		Infodroid = DRUID001;
	      if (Infodroid < DRUID001)
		Infodroid = Me.type;
	      
	      if ( (RightPressed ()) || (LeftPressed ()) )
		{
		  PassOn = 1;
		  LeaveThisInformationPart = TRUE;
		  while ((RightPressed ()) || (LeftPressed ()) || (UpPressed ())
			 || (DownPressed ())) ;
		}
	      if (SpacePressed ())
		{
		  while (SpacePressed ()) ;
		  return;
		}
	    }
	}				/* for */
      
    }; // while !LeaveThisInformationPart
  DebugPrintf (2, "\nvoid GreadDruidShow(void): End of function reached.");
}; // GreatDruidShow() 


/*-----------------------------------------------------------------
 * @Desc: Displays the concept view of Level "deck" in Userfenster
 * 	  
 *	Note: we no longer wait here for a key-press, but return
 *            immediately 
 *-----------------------------------------------------------------*/
void
ShowDeckMap (Level deck)
{
  finepoint tmp;
  tmp.x=Me.pos.x;
  tmp.y=Me.pos.y;

  ClearUserFenster ();
  Me.pos.x = CurLevel->xlen/2;
  Me.pos.y = CurLevel->ylen/2;

  SetCombatScaleTo( 0.25 );

  Assemble_Combat_Picture( ONLY_SHOW_MAP );

  SDL_Flip (Screen);

  Me.pos.x=tmp.x;
  Me.pos.y=tmp.y;

} /* ShowDeckMap() */

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
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
	  && (AllEnemys[i].levelnum == levelnum))
	return FALSE;
    }

  CurLevel->empty = TRUE;
  RealScore += DECKCOMPLETEBONUS;
  ShowScore += DECKCOMPLETEBONUS;

  if (ShipEmpty ())
    ThouArtVictorious ();

  return TRUE;
}

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/

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
}


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
int
NoKeyPressed (void)
{
  if (SpacePressed ())
    return (FALSE);
  if (LeftPressed ())
    return (FALSE);
  if (RightPressed ())
    return (FALSE);
  if (UpPressed ())
    return (FALSE);
  if (DownPressed ())
    return (FALSE);
  return (TRUE);
}				// int NoKeyPressed(void)


/*@Function============================================================
@Desc: l"oscht das Userfenster

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
ClearUserFenster (void)
{
  SDL_Rect tmp;
  
  Copy_Rect (User_Rect, tmp)

  SDL_FillRect( Screen , &tmp, 0 );
  return;

} // void ClearUserFenster(void)

#undef _ship_c
