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
 * This file contains all functions dealing with the HUGE, BIG font used for
 * the top status line, the score and the text displayed during briefing
 * and highscore inverview.  This has NOTHING to do with the fonts
 * of the SVGALIB or the fonts used for the horizontal srolling
 * message line!
 * ---------------------------------------------------------------------- */

/*
 * This file has been checked for remnants of german comments and shouldn't be
 * containing any of these any more.  If you still find any, please let me know. jp.
 */

#define _text_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "proto.h"
#include "global.h"
#include "text.h"
#include "SDL_rotozoom.h"

int DisplayTextWithScrolling (char *Text, int startx, int starty, const SDL_Rect *clip , SDL_Surface* Background );

char *Wordpointer;
unsigned char *Fontpointer;
unsigned char *Zeichenpointer[110];	  // Pointer-array to the letter bitmaps
unsigned int CurrentFontFG = FIRST_FONT_FG;	// current color of the font
unsigned int CurrentFontBG = FIRST_FONT_BG;

int CharsPerLine;		// line length in chars:  obsolete

// curent text insertion position
int MyCursorX;
int MyCursorY;

// buffer for text environment
int StoreCursorX;
int StoreCursorY;

unsigned int StoreTextBG;
unsigned int StoreTextFG;

/* ----------------------------------------------------------------------
 * This function does all the (text) interaction with a friendly droid
 * and maybe also does special interacions like Chandra and Stone.
 * ---------------------------------------------------------------------- */
char* 
GetChatWindowInput( SDL_Surface* Background , SDL_Rect* Chat_Window_Pointer )
{
  int OldTextCursorX, OldTextCursorY;
  int j;
  char* RequestString;
  SDL_Rect Input_Window;
  Input_Window.x=15;
  Input_Window.y=434;
  Input_Window.w=606;
  Input_Window.h=37;


  OldTextCursorX=MyCursorX;
  OldTextCursorY=MyCursorY;
  
  // Now we clear the text window, since the old text is still there
  SDL_BlitSurface( Background, &Input_Window , Screen , &Input_Window );
  DisplayText ( "What do you say? >" ,
		Input_Window.x , Input_Window.y + (Input_Window.h - FontHeight (GetCurrentFont() ) ) / 2 , 
		&Input_Window ); // , Background );
  // DisplayTextWithScrolling ( ">" , -1 , -1 , &Input_Window , Background );
  SDL_Flip ( Screen );
  RequestString = GetString( 20 , FALSE );
  MyCursorX=OldTextCursorX;
  MyCursorY=OldTextCursorY;
  
  DisplayTextWithScrolling ( "\n>" , MyCursorX , MyCursorY , Chat_Window_Pointer , Background );
  
  //--------------------
  // Cause we do not want to deal with upper and lower case difficulties, we simpy convert 
  // the given input string into lower cases.  That will make pattern matching afterwards
  // much more reliable.
  //
  // Also leading spaces should be removed
  j=0;
  while ( RequestString[j] != 0 ) { RequestString[j]=tolower( RequestString[j] ); j++; }
  while ( RequestString[0] == ' ' ) 
    {
      j=0;
      while ( RequestString [ j + 1 ] != 0 )
	{
	  RequestString[ j ] = RequestString[ j + 1 ];
	  j++;
	}
      RequestString[ j ] = 0;
    }



  return ( RequestString );
}; // char* GetChatWindowInput( void )


/* ----------------------------------------------------------------------
 * This function does the communication routine when the influencer in
 * transfer mode touched a friendly droid.
 * ---------------------------------------------------------------------- */
void 
ChatWithFriendlyDroid( int Enum )
{
  char* RequestString;
  char* DecisionString;
  int i;
  int InventoryIndex;
  char *fpath;
  char fname[500];
  char ReplyString[10000];
  char TextAddition[10000];
  SDL_Surface* Small_Droid;
  SDL_Surface* Large_Droid;
  SDL_Surface* Background;
  SDL_Rect Chat_Window;
  SDL_Rect Droid_Image_Window;
  int SelectedItemNumber;
  char* MyText;
	MyText = "Ah, influence device.  It's good that you have returned.  Dark proceedings are going on in
the universe.\n\
\n\
Recently it has come out that the MS have started a project to upload even humans with their new OS brand, transforming them into their mindless zombies.\n\
\n\
When the government tried to stop them, they were taken out and a new MS government installed.  All the military and police droids running their OS are under their command and have gone berserk against anybody and anything not belonging to their organisatzion or their prefered person list.\n\
\n\
Someone must go out and try to stop these barbaric deeds.  \n\
\n\
Long ago, there was the socalled influence device project, which had to be abandoned due to MS legal action.  The former participants of the project have sent a distress call, that they are under attack and all the MS machines in their current location have turned against them.  Many of them were slain already, but their distress call has automatically
revived you and sent you to help.\n\
\n\
I hope it is not too late, but the teleporter has been set up to transfer you directly to the Asteroid 232 research installation.\n\
\n\
If you need help, you can come to me.  I can identify many of the items you will find. And also Mr. Stone, a sales representative of the United Machines Corporation will offer you equippment and repairs.\n\
\n\
Please, you must go and try to save who you can!  Our best wishes will accompany you.\n\
\n";

  // From initiating transfer mode, space might still have been pressed. 
  // So we wait till it's released...
  while (SpacePressed());
  
  if ( strcmp ( Druidmap[ AllEnemys[ Enum ].type ].druidname , "CHA" ) == 0 )
    {
      Switch_Background_Music_To ( "Chandra01.ogg" );

      ScrollText ( MyText , SCROLLSTARTX, SCROLLSTARTY, User_Rect.y , NULL );
      // AnarchyMenu1.mod
      Switch_Background_Music_To ( CurLevel->Background_Song_Name );

      return;
    }

  if ( strcmp ( Druidmap[ AllEnemys[ Enum ].type ].druidname , "STO" ) == 0 )
    {
      // Switch_Background_Music_To ( "Chandra01.ogg" );

      // ScrollText ( MyText , SCROLLSTARTX, SCROLLSTARTY, User_Rect.y , NULL );
      // AnarchyMenu1.mod
      // Switch_Background_Music_To ( CurLevel->Background_Song_Name );
      
      BuySellMenu( );

      return;
    }



  // We define our input and image windows...
  Chat_Window.x=242; Chat_Window.y=100; Chat_Window.w=380; Chat_Window.h=314;
  Droid_Image_Window.x=15; Droid_Image_Window.y=82; Droid_Image_Window.w=215; Droid_Image_Window.h=330;


  Activate_Conservative_Frame_Computation( );

  //--------------------
  // Next we prepare the whole background for all later text operations
  //
  Background = IMG_Load( find_file ( "chat_test.jpg" , GRAPHICS_DIR, FALSE ) );
  if ( Background == NULL )
    {
      printf("\n\nChatWithFriendlyDroid: ERROR LOADING FILE!!!!  Error code: %s " , SDL_GetError() );
      Terminate(ERR);
    }
  strcpy( fname, Druidmap[ AllEnemys[Enum].type ].druidname );
  strcat( fname , ".png" );
  fpath = find_file (fname, GRAPHICS_DIR, FALSE);
  Small_Droid = IMG_Load (fpath) ;
  Large_Droid = zoomSurface( Small_Droid , 1.8 , 1.8 , 0 );
  SDL_BlitSurface( Large_Droid , NULL , Background , &Droid_Image_Window );
  SDL_BlitSurface( Background , NULL , Screen , NULL );
  SDL_Flip( Screen );

  // All droid chat should be done in the paradroid font I would say...
  SetCurrentFont( Para_BFont );

  // We print out a little greeting message...
  DisplayTextWithScrolling ( 
			    "Transfer channel protocol set up for text transfer...\n\n" , 
			    Chat_Window.x , Chat_Window.y , &Chat_Window , Background );

  //--------------------
  // If the droid has a visual desciption in his question-response-list, then we print
  // out this visual description first, so the player get's a better feeling for the
  // chat partner he's facing...
  //
  for ( i = 0 ; i < MAX_CHAT_KEYWORDS_PER_DROID ; i++ )
    {
      if ( !strcmp ( "DESCRIPTION" , AllEnemys[ Enum ].QuestionResponseList[ i * 2 ] ) ) // even entries = questions
	{
	  DisplayTextWithScrolling ( AllEnemys[ Enum ].QuestionResponseList[ i * 2 + 1 ] , 
				     -1 , -1 , &Chat_Window , Background );
	  break;
	}
    }

  while (1)
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
	  AllEnemys[ Enum ].TextToBeDisplayed="Connection closed.  See ya...";
	  AllEnemys[ Enum ].TextVisibleTime=0;
	  return;
	}

      //--------------------
      // At this point we examine the entered string and see if it is perhaps the
      // trigger for some request for a decision to the influencer by this droid.
      //
      // If this is the case, then we may ask for influs answer to the request
      // of course with a certain text again.
      //
      // Then we get the answer and see if it is a valid answer.
      //
      // And if this whole procedure was applied, then we need not match for other
      // things and can continue the chat immediately
      //
      for ( i = 0 ; i < MAX_REQUESTS_PER_DROID ; i++ )
	{
	  //--------------------
	  // First we see, if the mission situation is right for this request to be
	  // imposed upon the player
	  //
	  if ( AllEnemys[ Enum ].RequestList[ i ].RequestRequiresMissionDone != (-1) )
	    {
	      if ( Me.AllMissions[ AllEnemys[ Enum ].RequestList[ i ].RequestRequiresMissionDone ].MissionIsComplete != TRUE ) continue;
	    }
	  if ( AllEnemys[ Enum ].RequestList[ i ].RequestRequiresMissionUnassigned != (-1) )
	    {
	      if ( Me.AllMissions[ AllEnemys[ Enum ].RequestList[ i ].RequestRequiresMissionUnassigned ].MissionWasAssigned == TRUE ) continue;
	    }

	  // So if the word matches, then the request is really proper
	  if ( !strcmp ( RequestString , AllEnemys[ Enum ].RequestList[ i ].RequestTrigger ) ) 
	    {

	      // This asks the influ for a decision to make
	      DisplayTextWithScrolling ( AllEnemys[ Enum ].RequestList[ i ].RequestText , 
					 -1 , -1 , &Chat_Window , Background );

	      // Now we read in his answer and we do this as long as often as it takes for the influ
	      // to give a valid answer
	      DecisionString = "XASDFASDF";

	      while ( ( strcmp( DecisionString , AllEnemys[ Enum ].RequestList[ i ].AnswerYes ) != 0 ) &&
		      ( strcmp( DecisionString , AllEnemys[ Enum ].RequestList[ i ].AnswerNo ) != 0 ) )
		{
		  DecisionString = GetChatWindowInput( Background , &Chat_Window );
		  if ( ( strcmp( DecisionString , AllEnemys[ Enum ].RequestList[ i ].AnswerYes ) != 0 ) &&
		       ( strcmp( DecisionString , AllEnemys[ Enum ].RequestList[ i ].AnswerNo  ) != 0 ) )
		    DisplayTextWithScrolling ( "Please answer only yes or no." , 
					       -1 , -1 , &Chat_Window , Background );
		}
	      
	      // Now we respond to the decision the infuencer has made
		  if ( strcmp( DecisionString , AllEnemys[ Enum ].RequestList[ i ].AnswerYes ) == 0 )
		    {
		      DisplayTextWithScrolling ( AllEnemys[ Enum ].RequestList[ i ].ResponseYes , 
						 -1 , -1 , &Chat_Window , Background );
		      ExecuteActionWithLabel ( AllEnemys[ Enum ].RequestList[ i ].ActionTrigger );
		    }
		  else
		    {
		      DisplayTextWithScrolling ( AllEnemys[ Enum ].RequestList[ i ].ResponseNo , 
						 -1 , -1 , &Chat_Window , Background );
		    }
	      

	      break;
	    }
	}
      //--------------------
      // If a request trigger matched already, we do not process the default keywords any more
      // so that some actions can be caught!
      //
      if ( i != MAX_REQUESTS_PER_DROID ) continue;

      //--------------------
      // In some cases we will not want the default answers to be given,
      // cause they are the same for all droids.
      //
      // We therefore will search this robots question-answer-list FIRST
      // and look for a 
      // match in the question entries and if applicable print out the
      // matching answer of course, and if that is wo, we will continue
      // and not proceed to the default answers.
      //
      for ( i = 0 ; i < MAX_CHAT_KEYWORDS_PER_DROID ; i++ )
	{
	  if ( !strcmp ( RequestString , AllEnemys[ Enum ].QuestionResponseList[ i * 2 ] ) ) // even entries = questions
	    {
	      DisplayTextWithScrolling ( AllEnemys[ Enum ].QuestionResponseList[ i * 2 + 1 ] , 
					 -1 , -1 , &Chat_Window , Background );
	      break;
	    }
	}
      //--------------------
      // If a keyword matched already, we do not process the default keywords any more
      // so that some actions can be caught!
      //
      if ( i != MAX_CHAT_KEYWORDS_PER_DROID ) continue;

      //--------------------
      // the help command is always simple and clear.  We just need to print out the 
      // following help text describing common command and keyword options and that's it.
      //
      if ( !strcmp ( RequestString , "help" ) ) 
	{
	  DisplayTextWithScrolling("You can enter command phrases or ask about some keyword.\n\
Most useful command phrases are: FOLLOW STAY STATUS CLOSER DISTANT INSTALL \n\
Often useful information requests are: JOB NAME MS HELLO \n\
Of course you can ask the droid about anything else it has told you or about what you have heard somewhere else." , 
				   -1 , -1 , &Chat_Window , Background );
	  continue;
	}
      
      //--------------------
      // If the player requested the robot to follow him, this robot should switch to 
      // following mode and follow the 001 robot.  But this is at the moment not implemented.
      // Instead the robot will just print out the message, that he would follow, but don't
      // do anything at this time.
      //
      // Same holds true for the 'stay' command
      //
      if ( !strcmp ( RequestString , "follow" ) ) 
	{
	  DisplayTextWithScrolling( 
		      "Ok.  I'm on your tail.  I go where you go.  I will rest where you have rested.  I will follow your every step.  I try to do it at your speed.  You lead and I follow.  I hope you know where you're going.  I'll do my best to keep up." , 
		      -1 , -1 , &Chat_Window , Background );
	  AllEnemys[ Enum ].CompletelyFixed = FALSE;
	  AllEnemys[ Enum ].FollowingInflusTail = TRUE;
	  AllEnemys[ Enum ].StayHowManyFramesBehind = Get_Average_FPS ( ) * AllEnemys[ Enum ].StayHowManySecondsBehind;
	  AllEnemys[ Enum ].warten = AllEnemys[ Enum ].StayHowManySecondsBehind;
	  // printf(" Staying %d Frames behind.  Should be 5 seconds." , AllEnemys[ Enum ].StayHowManyFramesBehind );
	  // fflush( stdout );
	  continue;
	}

      if ( !strcmp ( RequestString , "closer" ) )
	{
	  if ( AllEnemys[ Enum ].StayHowManySecondsBehind > 1 ) AllEnemys[ Enum ].StayHowManySecondsBehind--;
	  sprintf( ReplyString , "Ok.  I'll stay closer to you, lets say %d seconds back." , 
		   AllEnemys[ Enum ].StayHowManySecondsBehind );
	  DisplayTextWithScrolling( ReplyString , -1 , -1 , &Chat_Window , Background );
	  AllEnemys[ Enum ].StayHowManyFramesBehind = Get_Average_FPS( ) * AllEnemys[ Enum ].StayHowManySecondsBehind;
	  continue;
	}

      if ( !strcmp ( RequestString , "distant" ) )
	{
	  if ( AllEnemys[ Enum ].StayHowManySecondsBehind < 10 ) AllEnemys[ Enum ].StayHowManySecondsBehind++;
	  sprintf( ReplyString , "Ok.  I'll stay farther away from you, lets say %d seconds back." , 
		   AllEnemys[ Enum ].StayHowManySecondsBehind );
	  DisplayTextWithScrolling( ReplyString , -1 , -1 , &Chat_Window , Background );
	  AllEnemys[ Enum ].StayHowManyFramesBehind = Get_Average_FPS( ) * AllEnemys[ Enum ].StayHowManySecondsBehind;
	  continue;
	}

      if ( !strcmp ( RequestString , "stay" ) )
	{
	  DisplayTextWithScrolling( 
				   "Ok.  I'll stay here and not move a bit.  I will do so until I receive further instructions from you.  I hope you will come back sooner or later." , 
		      -1 , -1 , &Chat_Window , Background );
	  AllEnemys[ Enum ].CompletelyFixed = TRUE;
	  AllEnemys[ Enum ].FollowingInflusTail = FALSE;
	  continue;
	}
      if ( !strcmp ( RequestString , "status" ) )
	{
	  sprintf( ReplyString , "Here's my status report:\nEnergy: %d/%d.\n" , 
		   (int) AllEnemys[ Enum ].energy , 
		   (int) Druidmap[ AllEnemys[Enum].type ].maxenergy );
	  if ( AllEnemys[ Enum ].FollowingInflusTail )
	      strcat( ReplyString , "I'm currently following you.\n" );
	  else
	    strcat( ReplyString , "I'm currently not following you.\n" );
	  if ( AllEnemys[ Enum ].CompletelyFixed )
	    strcat( ReplyString , "I am instructed to wait here for your return.\n" );
	  else
	    strcat( ReplyString , "I'm free to move.\n" );

	  DisplayTextWithScrolling( ReplyString , -1 , -1 , &Chat_Window , Background );

	  continue;
	}
      if ( !strcmp ( RequestString , "install" ) )
	{
	  sprintf( ReplyString , "Let's see which of your items I can install into your system:" );
	  
	  for ( InventoryIndex = 0 ; InventoryIndex < MAX_ITEMS_IN_INVENTORY ; InventoryIndex++ )
	    {
	      // Some items don't exist or can't be installed.  These will be skipped...
	      if ( Me.Inventory[ InventoryIndex ].type == (-1) ) continue;
	      if ( ItemMap[ Me.Inventory[ InventoryIndex ].type ].item_can_be_installed_in_influ == FALSE ) continue;

	      // At this point we know, that it's an installable item
	      sprintf( TextAddition , "\nSlot %d: %s could be installed." , InventoryIndex , 
		       ItemMap[ Me.Inventory[ InventoryIndex ].type ].ItemName );
	      strcat ( ReplyString, TextAddition );
	    }

	  strcat( ReplyString , "\nWhich slot shall be installed? (0-9 , q for none)" );
	  DisplayTextWithScrolling( ReplyString , -1 , -1 , &Chat_Window , Background );
	  
	  //--------------------
	  // Now we read in his answer and we do this as long as often as it takes for the influ
	  // to give a valid answer
	  DecisionString = "XASDFASDF";

	  while ( 1 )
	    {
	      DecisionString = GetChatWindowInput( Background , &Chat_Window );

	      if ( strcmp ( DecisionString , "q" ) == 0 ) 
		{
		  DisplayTextWithScrolling ( "Nothing then.  Good.  How else may I be of assistance?" , 
					     -1 , -1 , &Chat_Window , Background );
		  break;
		}


	      SelectedItemNumber = ( -1 );
	      sscanf( DecisionString , "%d" , &SelectedItemNumber );
	      
	      if ( ( SelectedItemNumber >= 0 ) && ( SelectedItemNumber <= 9 ) )
		{
		  if ( InstallItem( SelectedItemNumber ) == OK )
		    {
		      DisplayTextWithScrolling ( "As you wish.  The item has been installed into your system and should be usable immediately.  How else may I be of assistance?" , -1 , -1 , &Chat_Window , Background );
		      break;
		    }
		  else
		    {
		      DisplayTextWithScrolling ( "Sorry, but the item you selected is not designed for installation into your system." , -1 , -1 , &Chat_Window , Background );
		      break;
		    }
		}


	      DisplayTextWithScrolling ( "Please answer '0'-'9' or 'q'." , -1 , -1 , &Chat_Window , Background );
	    }
	      
	  // DisplayTextWithScrolling( ReplyString , -1 , -1 , &Chat_Window , Background );

	  continue;
	}

      //--------------------
      // In case non of the default keywords was said and also none of the
      // special keywords this droid would understand were said, then the
      // droid obviously hasn't understood the message and should also say
      // so.
      //
      if ( i == MAX_CHAT_KEYWORDS_PER_DROID )
	{
	  DisplayTextWithScrolling ( "Sorry, but of that I know entirely nothing." , 
				     -1 , -1 , &Chat_Window , Background );
	}
    }

}; // void ChatWithFriendlyDroid( int Enum );

/* ----------------------------------------------------------------------
 * This function assigns a text comment to say for an enemy right after
 * is has been hit.  This can be turned off via a switch in GameConfig.
 * ---------------------------------------------------------------------- */
void 
EnemyHitByBulletText( int Enum )
{
  Enemy ThisRobot=&AllEnemys[ Enum ];

  if ( !GameConfig.Enemy_Hit_Text ) return;
  
  ThisRobot->TextVisibleTime=0;
  if ( !ThisRobot->Friendly )
    switch (MyRandom(4))
    {
    case 0:
      ThisRobot->TextToBeDisplayed="Unhandled exception fault.  Press ok to reboot.";
      break;
    case 1:
      ThisRobot->TextToBeDisplayed="System fault. Please buy a newer version.";
      break;
    case 2:
      ThisRobot->TextToBeDisplayed="System error. Might be a virus.";
      break;
    case 3:
      ThisRobot->TextToBeDisplayed="System error. Pleae buy an upgrade from MS.";
      break;
    case 4:
      ThisRobot->TextToBeDisplayed="System error. Press any key to reboot.";
      break;
    }
  else
    ThisRobot->TextToBeDisplayed="Aargh, I got hit.  Ugh, I got a bad feeling...";
}; // void EnemyHitByBullet( int Enum );

/* ----------------------------------------------------------------------
 * This function assigns a text comment to say for an enemy right after
 * it has bumped into the player.  This can be turned off via a switch in GameConfig.
 * ---------------------------------------------------------------------- */
void 
EnemyInfluCollisionText ( int Enum )
{
  Enemy ThisRobot=&AllEnemys[ Enum ];

  if ( !GameConfig.Enemy_Bump_Text ) return;
  
  ThisRobot->TextVisibleTime=0;
	      
  if ( ThisRobot->Friendly )
    {
      ThisRobot->TextToBeDisplayed="Ah, good, that we have an open collision avoiding standard, isn't it.";
    }
  else
    {
      switch (MyRandom(1))
	{
	case 0:
	  ThisRobot->TextToBeDisplayed="Hey, I'm from MS! Walk outa my way!";
	  break;
	case 1:
	  ThisRobot->TextToBeDisplayed="Hey, I know the big MS boss! You better go.";
	  break;
	}
    }

}; // void EnemyInfluCollisionText ( int Enum )

/* ----------------------------------------------------------------------
 * This function assigns a text comment to say for an enemy while it is
 * standing and aiming for player.  This can be turned off via a switch in GameConfig.
 * ---------------------------------------------------------------------- */
void 
AddStandingAndAimingText ( int Enum )
{
  Enemy ThisRobot=&AllEnemys[ Enum ];

  if ( !GameConfig.Enemy_Aim_Text ) return;
  
  ThisRobot->TextVisibleTime=0;
	      
  if ( ( fabsf (Me.speed.x) < 1 ) && ( fabsf (Me.speed.y) < 1 ) )
    {
      ThisRobot->TextToBeDisplayed="Yeah, stay like that, haha.";
    }
  else
    {
      ThisRobot->TextToBeDisplayed="Stand still while I aim at you.";
    }

}; // void AddStandingAndAimingText( int Enum )

/* ----------------------------------------------------------------------
 * This function assigns a text comment to say for the influ right after
 * it has ran into an explosion.  This can be turned off via a switch in GameConfig.
 * ---------------------------------------------------------------------- */
void
AddInfluBurntText( void )
{
  int FinalTextNr;

  if ( !GameConfig.Influencer_Blast_Text ) return;
  
  Me.TextVisibleTime=0;
  
  FinalTextNr=MyRandom ( 6 );
  switch ( FinalTextNr )
    {
    case 0:
      Me.TextToBeDisplayed="Aaarrgh, aah, that burnt me!";
      break;
    case 1:
      Me.TextToBeDisplayed="Hell, that blast was hot!";
      break;
    case 2:
      Me.TextToBeDisplayed="Ghaart, I hate to stain my chassis like that.";
      break;
    case 3:
      Me.TextToBeDisplayed="Oh no!  I think I've burnt a cable!";
      break;
    case 4:
      Me.TextToBeDisplayed="Oh no, my poor transfer connectors smolder!";
      break;
    case 5:
      Me.TextToBeDisplayed="I hope that didn't melt any circuits!";
      break;
    case 6:
      Me.TextToBeDisplayed="So that gives some more black scars on me ol' dented chassis!";
      break;
    default:
      printf("\nError in AddInfluBurntText! That shouldn't be happening.");
      Terminate(ERR);
      break;
    }
}; // void AddInfluBurntText( void )

/* ----------------------------------------------------------------------
 * This function sets the text cursor used in DisplayText.
 * ---------------------------------------------------------------------- */
void
SetTextCursor ( int x , int y )
{
  MyCursorX = x;
  MyCursorY = y;

  return;
}; // void SetTextCursor ( int x , int y )

/* -----------------------------------------------------------------
 * This function scrolls a given text down inside the User-window, 
 * defined by the global SDL_Rect User_Rect
 *
 * startx/y give the Start-position, 
 * EndLine is the last line (?)
 * ----------------------------------------------------------------- */
int
ScrollText (char *Text, int startx, int starty, int EndLine , char* TitlePictureName )
{
  int Number_Of_Line_Feeds = 0;	// number of lines used for the text
  char *textpt;			// mobile pointer to the text
  int InsertLine = starty;
  int speed = +4;
  int maxspeed = 8;
  SDL_Surface* Background;

  Activate_Conservative_Frame_Computation( );

  if ( TitlePictureName != NULL )
    DisplayImage ( find_file( TitlePictureName , GRAPHICS_DIR, FALSE) );

  MakeGridOnScreen( (SDL_Rect*) &Full_Screen_Rect );
  DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE ); 
  Background = SDL_DisplayFormat( Screen );

  SetCurrentFont( Para_BFont );

  // printf("\nScrollTest should be starting to scroll now...");

  // getchar();

  // count the number of lines in the text
  textpt = Text;
  while (*textpt++)
    if (*textpt == '\n')
      Number_Of_Line_Feeds++;

  while ( !SpacePressed () )
    {
      if (UpPressed ())
	{
	  speed--;
	  if (speed < -maxspeed)
	    speed = -maxspeed;
	}
      if (DownPressed ())
	{
	  speed++;
	  if (speed > maxspeed)
	    speed = maxspeed;
	}

      usleep (30000);

      // DisplayImage ( find_file(TitlePictureName,GRAPHICS_DIR, FALSE) );
      // MakeGridOnScreen( (SDL_Rect*) &Full_Screen_Rect );
      // DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE ); 
      // ClearUserFenster(); 

      SDL_BlitSurface ( Background , NULL , Screen , NULL );

      if (!DisplayText (Text, startx, InsertLine, &User_Rect))
	{
	  // JP: I've disabled this, since with this enabled we won't even
	  // see a single line of the first section of the briefing.
	  // But this leads to that we currently NEVER can see the second 
	  // or third part of the briefing text, cause it will not start
	  // the new text part when the lower end of the first text part
	  // is reached.  I don't consider this bug release-critical.
	  //
	  // break;  /* Text has been scrolled outside User_Rect */
	}

      InsertLine -= speed;

      SDL_Flip (Screen);

      /* Nicht bel. nach unten wegscrollen */
      if (InsertLine > SCREENHEIGHT - 10 && (speed < 0))
	{
	  InsertLine = SCREENHEIGHT - 10;
	  speed = 0;
	}

    } /* while !Space_Pressed */

  SDL_FreeSurface( Background );

  while ( SpacePressed() ); // so that we don't touch again immediately.

  return OK;
}				// void ScrollText(void)

/*-----------------------------------------------------------------
 * This function is much like DisplayText, but with the main difference,
 * that in case of the whole clipping window filled, the function will
 * display a ---more--- line, wait for a key and then scroll the text
 * further up.
 *
 * @Desc: prints *Text beginning at positions startx/starty, 
 * 
 *	and respecting the text-borders set by clip_rect
 *      -> this includes clipping but also automatic line-breaks
 *      when end-of-line is reached
 * 
 *      if clip_rect==NULL, no clipping is performed
 *      
 *      NOTE: the previous clip-rectange is restored before
 *            the function returns!
 *
 *      NOTE2: this function _does not_ update the screen
 *
 * @Ret: TRUE if some characters where written inside the clip rectangle
 *       FALSE if not (used by ScrollText to know if Text has been scrolled
 *             out of clip-rect completely)
 *-----------------------------------------------------------------*/
int
DisplayTextWithScrolling (char *Text, int startx, int starty, const SDL_Rect *clip , SDL_Surface* Background )
{
  char *tmp;	// mobile pointer to the current position as the text is drawn
  // SDL_Rect Temp_Clipping_Rect; // adding this to prevent segfault in case of NULL as parameter

  SDL_Rect store_clip;

  if ( startx != -1 ) MyCursorX = startx;		
  if ( starty != -1 ) MyCursorY = starty;

  SDL_GetClipRect (Screen, &store_clip);  /* store previous clip-rect */
  if (clip)
    SDL_SetClipRect (Screen, clip);
  else
    {
      clip = & User_Rect;
    }


  tmp = Text;			/* running text-pointer */

  while ( *tmp && (MyCursorY < clip->y + clip->h) )
    {
      if ( *tmp == '\n' )
	{
	  MyCursorX = clip->x;
	  MyCursorY += FontHeight ( GetCurrentFont() ) * TEXT_STRETCH;

	}
      else
	DisplayChar (*tmp);
      tmp++;

      //--------------------
      // Here we plant in the question for ---more--- and a key to be pressed,
      // before we clean the screen and restart displaying text from the top
      // of the given Clipping rectangle
      //
      // if ( ( clip->h + clip->y - MyCursorY ) <= 2 * FontHeight ( GetCurrentFont() ) * TEXT_STRETCH )
      if ( ( clip->h + clip->y - MyCursorY ) <= 1 * FontHeight ( GetCurrentFont() ) * TEXT_STRETCH )
	{
	  DisplayText( "--- more --- more --- \n" , MyCursorX , MyCursorY , clip );
	  SDL_Flip( Screen );
	  while ( !SpacePressed() );
	  while (  SpacePressed() );
	  SDL_BlitSurface( Background , NULL , Screen , NULL );
	  MyCursorY = clip->y;
	  SDL_Flip( Screen );
	};
      
      if (clip)
	ImprovedCheckUmbruch(tmp, clip);   /* dont write over right border */

    } // while !FensterVoll()

   SDL_SetClipRect (Screen, &store_clip); /* restore previous clip-rect */

  /*
   * ScrollText() wants to know if we still wrote something inside the
   * clip-rectangle, of if the Text has been scrolled out
   */
   if ( clip && ((MyCursorY < clip->y) || (starty > clip->y + clip->h) ))
     return FALSE;  /* no text was written inside clip */
   else
     return TRUE; 

};

/*-----------------------------------------------------------------
 * @Desc: prints *Text beginning at positions startx/starty, 
 * 
 *	and respecting the text-borders set by clip_rect
 *      -> this includes clipping but also automatic line-breaks
 *      when end-of-line is reached
 * 
 *      if clip_rect==NULL, no clipping is performed
 *      
 *      NOTE: the previous clip-rectange is restored before
 *            the function returns!
 *
 *      NOTE2: this function _does not_ update the screen
 *
 * @Ret: TRUE if some characters where written inside the clip rectangle
 *       FALSE if not (used by ScrollText to know if Text has been scrolled
 *             out of clip-rect completely)
 *-----------------------------------------------------------------*/
int
DisplayText (char *Text, int startx, int starty, const SDL_Rect *clip)
{
  char *tmp;	// mobile pointer to the current position in the string to be printed
  SDL_Rect Temp_Clipping_Rect; // adding this to prevent segfault in case of NULL as parameter

  SDL_Rect store_clip;

  if ( startx != -1 ) MyCursorX = startx;		
  if ( starty != -1 ) MyCursorY = starty;


  SDL_GetClipRect (Screen, &store_clip);  /* store previous clip-rect */
  if ( clip != NULL )
    SDL_SetClipRect ( Screen , clip );
  else
    {
      clip = & Temp_Clipping_Rect;
      Temp_Clipping_Rect.x=0;
      Temp_Clipping_Rect.y=0;
      Temp_Clipping_Rect.w=SCREENLEN;
      Temp_Clipping_Rect.h=SCREENHEIGHT;
    }


  tmp = Text;			/* running text-pointer */

  while ( *tmp && (MyCursorY < clip->y + clip->h) )
    {
      if ( *tmp == '\n' )
	{
	  MyCursorX = clip->x;
	  MyCursorY += FontHeight ( GetCurrentFont() ) * TEXT_STRETCH;
	}
      else
	DisplayChar (*tmp);

      tmp++;

      if (clip)
	ImprovedCheckUmbruch(tmp, clip);   /* dont write over right border */

    } // while !FensterVoll()

   SDL_SetClipRect (Screen, &store_clip); /* restore previous clip-rect */



  /*
   * ScrollText() wants to know if we still wrote something inside the
   * clip-rectangle, of if the Text has been scrolled out
   */
   if ( clip && ((MyCursorY < clip->y) || (starty > clip->y + clip->h) ))
     return FALSE;  /* no text was written inside clip */
   else
     return TRUE; 

} // DisplayText(...)

/* -----------------------------------------------------------------
 * This function displays a char.  It uses Menu_BFont now
 * to do this.  MyCursorX is  updated to new position.
 * ----------------------------------------------------------------- */
void
DisplayChar (unsigned char c)
{

  if ( !isprint(c) ) // don't accept non-printable characters
    {
      printf ("Illegal char passed to DisplayChar(): %d \n", c);
      Terminate(ERR);
    }

  PutChar (Screen, MyCursorX, MyCursorY, c);

  // After the char has been displayed, we must move the cursor to its
  // new position.  That depends of course on the char displayed.
  //
  MyCursorX += CharWidth ( GetCurrentFont() , c);
  
} // void DisplayChar(...)


/* ----------------------------------------------------------------------
 * This function checks if the next word still fits in this line
 * of text and initiates a carriage return/line feed if not.
 * Very handy and convenient, for that means it is no longer nescessary
 * to enter \n in the text every time its time for a newline. cool.
 *  
 * The function could perhaps still need a little improvement.  But for
 * now its good enough and improvement enough in comparison to the old
 * CheckUmbruch function.
 *
 * rp: added argument clip, which contains the text-window we're writing in
 *     (formerly known as "TextBorder")
 *
 * ---------------------------------------------------------------------- */
void
ImprovedCheckUmbruch (char* Resttext, const SDL_Rect *clip)
{
  int i;
  int NeededSpace=0;
#define MAX_WORD_LENGTH 100

  // In case of a space, see if the next word will still fit on the line
  // and do a carriage return/line feed if not
  if ( *Resttext == ' ' ) {
    for (i=1;i<MAX_WORD_LENGTH;i++) 
      {
	if ( (Resttext[i] != ' ') && (Resttext[i] != '\n') && (Resttext[i] != 0) )
	  { 
	    NeededSpace+=CharWidth( GetCurrentFont() , Resttext[i] );
	    if ( MyCursorX+NeededSpace > clip->x + clip->w - 10 )
	      {
		MyCursorX = clip->x;
		MyCursorY += FontHeight ( GetCurrentFont() ) * TEXT_STRETCH;
		return;
	      }
	  }
	else 
	  return;
      }
  }
}; // void ImprovedCheckUmbruch(void)


/* -----------------------------------------------------------------
 * This function reads a string of "MaxLen" from User-input, and echos it 
 * either to stdout or using graphics-text, depending on the
 * parameter "echo":	echo=0    no echo
 *                      echo=1    print using printf
 *                      echo=2    print using graphics-text
 *
 * values of echo > 2 are ignored and treated like echo=0
 *
 * NOTE: MaxLen is the maximal _strlen_ of the string (excl. \0 !)
 * 
 * @Ret: char *: String is allocated _here_!!!
 *       (dont forget to free it !)
 * 
 * ----------------------------------------------------------------- */
char *
GetString (int MaxLen, int echo)
{
  char *input;		// pointer to the string entered by the user
  int key;          // last 'character' entered 
  int curpos;		// counts the characters entered so far
  int finished;
  int x0, y0, height;
  SDL_Rect store_rect, tmp_rect;
  SDL_Surface *store = NULL;

  if (echo == 1)		/* echo to stdout */
    {
      printf ("\nGetString(): sorry, echo=1 currently not implemented!\n");
      return NULL;
    }

  x0 = MyCursorX;
  y0 = MyCursorY;
  height = FontHeight (GetCurrentFont());
  
  store = SDL_CreateRGBSurface(0, SCREENLEN, height, vid_bpp, 0, 0, 0, 0);
  Set_Rect (store_rect, x0, y0, SCREENLEN, height);
  SDL_BlitSurface (Screen, &store_rect, store, NULL);

  // allocate memory for the users input
  input     = MyMalloc (MaxLen + 5);

  memset (input, '.', MaxLen);
  input[MaxLen] = 0;
  
  finished = FALSE;
  curpos = 0;

  while ( !finished  )
    {
      Copy_Rect( store_rect, tmp_rect);
      SDL_BlitSurface (store, NULL, Screen, &tmp_rect);
      PutString (Screen, x0, y0, input);
      SDL_Flip (Screen);
      
      key = getchar_raw ();  
      
      if (key == SDLK_RETURN) 
	{
	  input[curpos] = 0;
	  finished = TRUE;
	}
      else if (isprint (key) && (curpos < MaxLen) )  
	{
	  /* printable characters are entered in string */
	  input[curpos] = (char) key;   
	  curpos ++;
	}
      else if (key == SDLK_BACKSPACE)
	{
	  if ( curpos > 0 ) curpos --;
	  input[curpos] = '.';
	}
      
    } /* while(!finished) */

  DebugPrintf (2, "\n\nchar *GetString(..):  The final string is:\n");
  DebugPrintf (2,  input );
  DebugPrintf (2, "\n\n");


  return (input);

}; // char* GetString( ... ) 

/* -----------------------------------------------------------------
 * This function is similar to putchar(), but uses the SDL via the 
 * BFont-fct PutChar() instead.
 *
 * Is sets MyCursor[XY], and allows passing (-1,-1) as coords to indicate
 * using the current cursor position.
 * ----------------------------------------------------------------- */
int
putchar_SDL (SDL_Surface *Surface, int x, int y, int c)
{
  int ret;
  if (x == -1) x = MyCursorX;
  if (y == -1) y = MyCursorY;
  
  MyCursorX = x + CharWidth (GetCurrentFont(), c);
  MyCursorY = y;

  ret = PutChar (Surface, x, y, c);

  SDL_Flip (Surface);

  return (ret);
}; // int putchar_SDL (SDL_Surface *Surface, int x, int y, int c)


/* -----------------------------------------------------------------
 * behaves similarly as gl_printf() of svgalib, using the BFont
 * print function PrintString().
 *  
 *  sets current position of MyCursor[XY],  
 *     if last char is '\n': to same x, next line y
 *     to end of string otherwise
 *
 * Added functionality to PrintString() is: 
 *  o) passing -1 as coord uses previous x and next-line y for printing
 *  o) Screen is updated immediatly after print, using SDL_flip()                       
 *
 * ----------------------------------------------------------------- */
void
printf_SDL (SDL_Surface *screen, int x, int y, char *fmt, ...)
{
  va_list args;
  int i;

  char *tmp;
  va_start (args, fmt);

  if (x == -1) x = MyCursorX;
  else MyCursorX = x;

  if (y == -1) y = MyCursorY;
  else MyCursorY = y;

  tmp = (char *) MyMalloc (10000 + 1);
  vsprintf (tmp, fmt, args);
  PutString (screen, x, y, tmp);

  SDL_Flip (screen);

  if (tmp[strlen(tmp)-1] == '\n')
    {
      MyCursorX = x;
      MyCursorY = y+ 1.1* (GetCurrentFont()->h);
    }
  else
    {
      for (i=0; i < strlen(tmp); i++)
	MyCursorX += CharWidth (GetCurrentFont(), tmp[i]);
      MyCursorY = y;
    }

  free (tmp);
  va_end (args);
}; // void printf_SDL (SDL_Surface *screen, int x, int y, char *fmt, ...)


#undef _text_c
