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

#define _text_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "proto.h"
#include "global.h"
#include "text.h"
#include "SDL_rotozoom.h"

int DisplayTextWithScrolling (char *Text, int startx, int starty, const SDL_Rect *clip , SDL_Surface* ScrollBackground );

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

char BigScreenMessage[5000];
float BigScreenMessageDuration=10000;

static SDL_Surface* Background;

/* ----------------------------------------------------------------------
 * This function does all the (text) interaction with a friendly droid
 * and maybe also does special interacions like Chandra and Stone.
 * ---------------------------------------------------------------------- */
char* 
GetChatWindowInput( SDL_Surface* ChatBackground , SDL_Rect* Chat_Window_Pointer )
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
  SDL_BlitSurface( ChatBackground, &Input_Window , Screen , &Input_Window );
  DisplayText ( "What do you say? >" ,
		Input_Window.x , Input_Window.y + (Input_Window.h - FontHeight (GetCurrentFont() ) ) / 2 , 
		&Input_Window ); // , ChatBackground );
  // DisplayTextWithScrolling ( ">" , -1 , -1 , &Input_Window , ChatBackground );
  SDL_Flip ( Screen );
  RequestString = GetString( 20 , FALSE );
  MyCursorX=OldTextCursorX;
  MyCursorY=OldTextCursorY;
  
  DisplayTextWithScrolling ( "\n>" , MyCursorX , MyCursorY , Chat_Window_Pointer , ChatBackground );
  
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
 * This function restores all chat-with-friendly-droid variables to their
 * initial values.  This means, that NOT ALL FLAGS CAN BE SET HERE!!  Some
 * of them must remain at their current values!!! TAKE CARE!!
 * ---------------------------------------------------------------------- */
void
RestoreChatVariableToInitialValue( int PlayerNum )
{
  int j;

  /*
  if ( Me [ PlayerNum ] . has_agreed_at_chandra )
    {
      //--------------------
      // If the Tux has talked to chandra yet, all others will
      // talk to him.  Otherwise they will only say very little
      // and redirect him to chandra.
      //
      // Me [ PlayerNum ] . Chat_Flags [ PERSON_RMS ] [ 0 ] = 1 ;
      // Me [ PlayerNum ] . Chat_Flags [ PERSON_RMS ] [ 0 ] = 1 ;
    }
  else
    {
      Me [ PlayerNum ] . Chat_Flags [ PERSON_RMS ] [ 0 ] = 0 ;
    }
  */

  //--------------------
  // You can always ask the moron 614 bots the same things and they
  // will always respond in the very same manner, so no need to
  // remember anything that has been talked in any previous conversation
  // with them.
  //
  Me [ PlayerNum ] . Chat_Flags [ PERSON_614 ] [ 0 ] = 1 ;
  Me [ PlayerNum ] . Chat_Flags [ PERSON_614 ] [ 1 ] = 1 ;
  Me [ PlayerNum ] . Chat_Flags [ PERSON_614 ] [ 2 ] = 1 ;
  Me [ PlayerNum ] . Chat_Flags [ PERSON_614 ] [ 3 ] = 1 ;

  //--------------------
  // The 'END' options should always be availabe for all dialogs
  // at the beginning.  THIS we know.
  //
  for ( j = 0 ; j < MAX_PERSONS ; j++ )
    {
      Me [ PlayerNum ] . Chat_Flags [ j ] [ END_ANSWER ] = 1 ;
    }

}; // void RestoreChatVariableToInitialValue( int PlayerNum )

/* ----------------------------------------------------------------------
 * This function displays a subtitle for the ChatWithFriendlyDroid interface,
 * so that you can also understand everything with sound disabled.
 * ---------------------------------------------------------------------- */
void
DisplaySubtitle( char* SubtitleText , void* SubtitleBackground )
{
  SDL_Rect Subtitle_Window;

  //--------------------
  // If the user has disabled the subtitles in the dialogs, we need
  // not do anything and just return...
  //
  if ( !GameConfig.show_subtitles_in_dialogs ) return;

  //--------------------
  // First we define our subtitle window.
  //
  Subtitle_Window.x= 15; Subtitle_Window.y=435; Subtitle_Window.w=600; Subtitle_Window.h=44;

  //--------------------
  // Now we need to clear this window, cause there might still be some
  // garbage from the previous subtitle in there...
  //
  SDL_BlitSurface ( SubtitleBackground , &Subtitle_Window , Screen , &Subtitle_Window );

  //--------------------
  // Now we can display the text and update the screen...
  //
  SDL_SetClipRect( Screen, NULL );
  DisplayText ( SubtitleText , Subtitle_Window.x , Subtitle_Window.y , &Subtitle_Window );
  SDL_UpdateRect ( Screen , Subtitle_Window.x , Subtitle_Window.y , Subtitle_Window.w , Subtitle_Window.h );

}; // void DisplaySubtitle( char* SubtitleText , void* SubtitleBackground )

/* ----------------------------------------------------------------------
 * This function should first display a subtitle and then also a sound
 * sample.  It is not very sophisticated or complicated, but nevertheless
 * important, because this combination does indeed occur so often.
 * ---------------------------------------------------------------------- */
void
GiveSubtitleNSample( char* SubtitleText , char* SampleFilename )
{
  DisplaySubtitle ( SubtitleText , Background );
  PlayOnceNeededSoundSample( SampleFilename , TRUE );
}; // void GiveSubtitleNSample( char* SubtitleText , char* SampleFilename )

/* ----------------------------------------------------------------------
 * This function prepares the chat background window and displays the
 * image of the dialog partner and also sets the right font.
 * ---------------------------------------------------------------------- */
void
PrepareMultipleChoiceDialog ( int Enum )
{
  SDL_Rect Droid_Image_Window;
  SDL_Surface* Small_Droid;
  SDL_Surface* Large_Droid;
  // SDL_Surface* Background;
  char *fpath;
  char fname[500];

  Droid_Image_Window.x=15; Droid_Image_Window.y=82; Droid_Image_Window.w=215; Droid_Image_Window.h=330;
  Activate_Conservative_Frame_Computation( );

  //--------------------
  // Next we prepare the whole background for all later text operations
  //
  if ( Background == NULL )
    Background = IMG_Load( find_file ( "backgrounds/chat_test.jpg" , GRAPHICS_DIR, FALSE ) );
  else
    {
      //--------------------
      // when there is still an old surface present, it might be tainted and we 
      // therefore generate a new fresh one without the image of the previous
      // discussion partner on it.
      //
      SDL_FreeSurface( Background );
      Background = IMG_Load( find_file ( "backgrounds/chat_test.jpg" , GRAPHICS_DIR, FALSE ) );
    }
  if ( Background == NULL )
    {
      DebugPrintf( 0 , "\n----------------------------------------------------------------------\n\
ChatWithFriendlyDroid: ERROR LOADING BACKGROUND IMAGE FILE!!!!  \n\
Error code: %s \n\
Freedroid will terminate now to draw attention to the graphics loading\n\
problem it could not resolve.  Sorry.\n\
----------------------------------------------------------------------\n" , SDL_GetError() );
      Terminate(ERR);
    }

  strcpy( fname, "droids/" );
  strcat( fname, Druidmap[ AllEnemys[Enum].type ].druidname );
  strcat( fname , ".png" );
  fpath = find_file (fname, GRAPHICS_DIR, FALSE);
  Small_Droid = IMG_Load (fpath) ;
  Large_Droid = zoomSurface( Small_Droid , 1.8 , 1.8 , 0 );
  SDL_BlitSurface( Large_Droid , NULL , Background , &Droid_Image_Window );
  SDL_BlitSurface( Background , NULL , Screen , NULL );
  SDL_Flip( Screen );

  SDL_FreeSurface( Small_Droid );
  SDL_FreeSurface( Large_Droid );

  // All droid chat should be done in the paradroid font I would say...
  // SetCurrentFont( Para_BFont );
  SetCurrentFont( FPS_Display_BFont );

}; // void PrepareMultipleChoiceDialog ( int Enum )

/* ----------------------------------------------------------------------
 * This function does the communication routine when the influencer in
 * transfer mode touched a friendly droid.
 * ---------------------------------------------------------------------- */
void 
ChatWithFriendlyDroid( int Enum )
{
  char* RequestString;
  char* DecisionString;
  int i ;
  char ReplyString[10000];
  SDL_Rect Chat_Window;
  int MenuSelection = (-1) ;
  char* DialogMenuTexts[ MAX_ANSWERS_PER_PERSON ];

  Chat_Window.x=242; Chat_Window.y=100; Chat_Window.w=380; Chat_Window.h=314;

  //--------------------
  // First we empty the array of possible answers in the
  // chat interface.
  //
  for ( i = 0 ; i < MAX_ANSWERS_PER_PERSON ; i ++ )
    {
      DialogMenuTexts [ i ] = "" ;
    }

  //--------------------
  // This should make all the answering possibilities available
  // that are there without any prerequisite and that can be played
  // through again and again without any modification.
  //
  RestoreChatVariableToInitialValue( 0 ); // Player=0 for now.
  

  // From initiating transfer mode, space might still have been pressed. 
  // So we wait till it's released...
  while (SpacePressed());
  
  if ( strcmp ( Druidmap[ AllEnemys[ Enum ].type ].druidname , "CHA" ) == 0 )
    {
      //--------------------
      // Now we do the dialog with Dr. Chandra...
      //
      PrepareMultipleChoiceDialog( Enum );

      DialogMenuTexts [ 0 ] = " Hi!  I'm new here. " ;
      DialogMenuTexts [ 1 ] = " What can you tell me about this place? " ;
      DialogMenuTexts [ 2 ] = " Where can I get better equipment? " ;

      DialogMenuTexts [ 4 ] = " What can you tell me about the MS? " ;
      DialogMenuTexts [ 5 ] = " Wouldn't that just mean replacing one evil with another?" ;
      DialogMenuTexts [ 6 ] = " I want to get in contact with the MS." ;
      DialogMenuTexts [ 7 ] = " I would like to get in contact with the Rebellion." ;
      DialogMenuTexts [ 8 ] = " How can I gain their trust?" ;
      DialogMenuTexts [ 9 ] = " Have I done enough quests yet for my meeting with the Resistance?" ;
      DialogMenuTexts [ MAX_ANSWERS_PER_PERSON - 1 ] = " END ";
      
      // GiveSubtitleNSample( " Welcome Traveller! ", "Chandra_Welcome_Traveller_0.wav" );
      

      while (1)
	{
	  
	  // MenuSelection = ChatDoMenuSelection ( "What will you say?" , MenuTexts , 1 , NULL , FPS_Display_BFont );
	  MenuSelection = ChatDoMenuSelectionFlagged ( "What will you say?" , DialogMenuTexts , Me[0].Chat_Flags [ PERSON_CHA ]  , 1 , NULL , FPS_Display_BFont );
	  
	  switch( MenuSelection )
	    {
	    case 1:
	      PlayOnceNeededSoundSample( "Tux_Hi_Im_New_0.wav" , TRUE );
	      GiveSubtitleNSample( "Welcome to this camp! I am Chandra. I care about visitors." , "Chandra_Welcome_To_This_0.wav" );
	      Me [ 0 ] . Chat_Flags [ PERSON_CHA ] [ 0 ] = 0 ;
	      Me [ 0 ] . Chat_Flags [ PERSON_CHA ] [ 1 ] = 1 ;
	      Me [ 0 ] . Chat_Flags [ PERSON_CHA ] [ 2 ] = 1 ;
	      break;
	    case 2:
	      PlayOnceNeededSoundSample( "Tux_Chandra_What_Can_Place_0.wav" , TRUE );
	      GiveSubtitleNSample( "This place is owned by refugees like me, who hide from the MS. " , "Chandra_This_Place_Consists_0.wav" );
	      GiveSubtitleNSample( "Formerly this was a camp of the resistance movement. " , "Chandra_Formerly_This_Was_0.wav" );
	      GiveSubtitleNSample( "But they have left long ago. " , "Chandra_But_They_Have_0.wav" );
	      Me [ 0 ] . Chat_Flags [ PERSON_CHA ] [ 1 ] = 0 ;
	      Me [ 0 ] . Chat_Flags [ PERSON_CHA ] [ 4 ] = 1 ;
	      break;
	    case 3:
	      PlayOnceNeededSoundSample( "Tux_Chandra_Where_Can_I_0.wav" , TRUE );
	      GiveSubtitleNSample( " You might try it at the shop. " , "Chandra_You_Might_Try_0.wav" );
	      GiveSubtitleNSample( " Mrs. Stone always has a good range of equipment there. " , "Chandra_They_Have_All_0.wav" );
	      Me [ 0 ] . Chat_Flags [ PERSON_CHA ] [ 2 ] = 0 ; // but don't ask this twice.
	      break;
	    case 5:
	      PlayOnceNeededSoundSample( "Tux_Chandra_What_Can_MS_0.wav" , TRUE );
	      GiveSubtitleNSample( " Currently, the MS is the most powerful organisation in the universe. " , "Chandra_Currently_The_MS_0.wav" );
	      GiveSubtitleNSample( " But maybe not for very much longer:  A rebellion has started." , "Chandra_But_Maybe_Not_0.wav" );
	      GiveSubtitleNSample( " The rebels have small supporters throughout the universe." , "Chandra_The_Rebels_Have_0.wav" );
	      GiveSubtitleNSample( " The number of the rebels has grown a lot in the last decade." , "Chandra_The_Number_Of_0.wav" );
	      GiveSubtitleNSample( " It might be that in the end they can win their struggle for freedom." , "Chandra_It_Might_Well_0.wav" );
	      Me [ 0 ] . Chat_Flags [ PERSON_CHA ] [ 5 ] = 1 ; // some new possibilities...
	      Me [ 0 ] . Chat_Flags [ PERSON_CHA ] [ 6 ] = 1 ;
	      Me [ 0 ] . Chat_Flags [ PERSON_CHA ] [ 7 ] = 1 ;
	      Me [ 0 ] . Chat_Flags [ PERSON_CHA ] [ 4 ] = 0 ; // but don't ask this twice.
	      break;
	    case 6:
	      PlayOnceNeededSoundSample( "Tux_Chandra_Wouldnt_That_Mean_0.wav" , TRUE );
	      GiveSubtitleNSample( "By no means.  The rebellion is carried on by individuals. " , "Chandra_By_No_Means_0.wav" );
	      GiveSubtitleNSample( "They are neither bound to some central authority nor are they paid in any way." , "Chandra_They_Are_Neither_0.wav" );
	      GiveSubtitleNSample( "They work together for thier common goal of freedom." , "Chandra_They_Work_Together_0.wav" );
	      GiveSubtitleNSample( "They will not change their goal to support the power and will of only one." , "Chandra_They_Will_Not_0.wav" );
	      Me [ 0 ] . Chat_Flags [ PERSON_CHA ] [ 5 ] = 0 ; // but don't ask this twice.
	      break;
	    case 7:
	      PlayOnceNeededSoundSample( "Tux_Chandra_I_Want_To_0.wav" , TRUE );
	      GiveSubtitleNSample( "This will be difficult.  As a non-member of the MS, their machines will attack you." , "Chandra_This_Will_Be_0.wav" );
	      GiveSubtitleNSample( "But maybe there is a way.  I just don't know." , "Chandra_But_Maybe_There_0.wav" );
	      Me [ 0 ] . Chat_Flags [ PERSON_CHA ] [ 6 ] = 0 ; // but don't ask this twice.
	      break;
	    case 8:
	      PlayOnceNeededSoundSample( "Tux_Chandra_I_Would_Like_0.wav" , TRUE );
	      GiveSubtitleNSample( "This should be possible.  But you must gain their trust first." , "Chandra_This_Should_Be_0.wav" );
	      Me [ 0 ] . Chat_Flags [ PERSON_CHA ] [ 7 ] = 0 ; // but don't ask this twice.
	      Me [ 0 ] . Chat_Flags [ PERSON_CHA ] [ 8 ] = 1 ; // this should lead on...
	      break;
	    case 9:
	      PlayOnceNeededSoundSample( "Tux_Chandra_How_Can_I_0.wav" , TRUE );
	      GiveSubtitleNSample( "Ask around! " , "Chandra_Ask_Around_Ill_0.wav" );
	      GiveSubtitleNSample( "I'm sure help is needed at all ends of town! " , "Chandra_Im_Sure_Help_0.wav" );
	      GiveSubtitleNSample( "If you do well on them, I'll hear that and I'll count it in your favour." , "Chandra_If_You_Do_0.wav" );
	      GiveSubtitleNSample( "If you think you've done enough quests, come back here and I'll see what I can do." , "Chandra_If_You_Think_0.wav" );
	      Me [ 0 ] . Chat_Flags [ PERSON_CHA ] [ 8 ] = 0 ; // but don't ask this twice.
	      Me [ 0 ] . Chat_Flags [ PERSON_CHA ] [ 9 ] = 1 ; // this should lead on...

	      //--------------------
	      // This should enable some dialog options for some other characters...
	      //
	      // Me [ 0 ] . Chat_Flags [ PERSON_SOR ] [ 0 ] = 1 ; // 'Chandra said you have something to do for me...
	      // Me [ 0 ] . Chat_Flags [ PERSON_RMS ] [ 0 ] = 1 ; // 'Chandra said you have something to do for me...
	      //

	      break;
	    case 10:
	      PlayOnceNeededSoundSample( "Tux_Chandra_Have_I_Done_0.wav" , TRUE );
	      GiveSubtitleNSample( " No, not yet.  Get going.  There's still a lot of things to do for you." , "Chandra_No_Not_Really_0.wav" );
	      // Me [ 0 ] . Chat_Flags [ PERSON_CHA ] [ 9 ] = 0 ; // but don't ask this twice.
	      // Me [ 0 ] . Chat_Flags [ PERSON_CHA ] [ 8 ] = 0 ; // this should lead on...
	      break;
	    case MAX_ANSWERS_PER_PERSON :
	    case (-1):
	    default:
	      PlayOnceNeededSoundSample( "Tux_Goodbye_0.wav" , TRUE );
	      return;
	      break;
	    }
	}
    } // end of conversation with Chandra.

  //**********************************************************************
  // Here comes the dialog interface for conversation with SORENSON, the
  // SOR person for short.
  //
  // This should be a teacher of magical abilities.
  // He does not assign any quests so far.
  //
  if ( strcmp ( Druidmap[ AllEnemys[ Enum ].type ].druidname , "SOR" ) == 0 )
    {
      //--------------------
      // Now we do the dialog with SOR...
      //
      PrepareMultipleChoiceDialog( Enum );
      
      DialogMenuTexts [ 1 ] = "  " ;
      DialogMenuTexts [ 0 ] = " Hi!  I'm new here. " ; // this is enabled ONLY ONCE in InitNewMissionList!
      DialogMenuTexts [ 2 ] = " What can you teach me about mental abilities? " ;
      DialogMenuTexts [ 3 ] = " Mind +1 (costs 1 ability point)" ;
      DialogMenuTexts [ 4 ] = " Mind +5 (costs 5 ability points)" ;
      DialogMenuTexts [ 5 ] = " Improve spellcasting ability\n (cost 5 ability points, 100 cash) ";
      DialogMenuTexts [ 6 ] = " BACK ";
      DialogMenuTexts [ END_ANSWER ] = " END ";
      
      // GiveSubtitleNSample( " Welcome Traveller! " , "Chandra_Welcome_Traveller_0.wav" );

      while (1)
	{
	  
	  // MenuSelection = ChatDoMenuSelection ( "What will you say?" , MenuTexts , 1 , NULL , FPS_Display_BFont );
	  MenuSelection = ChatDoMenuSelectionFlagged ( "What will you say?" , DialogMenuTexts , Me[0].Chat_Flags [ PERSON_SOR ]  , 1 , NULL , FPS_Display_BFont );
	  
	  switch( MenuSelection )
	    {
	    case 1:
	      PlayOnceNeededSoundSample( "Tux_Hi_Im_New_0.wav" , TRUE );
	      GiveSubtitleNSample( " Welcome then to this camp!  I'm Sorenson, teacher of magical abilities. " , "SOR_Welcome_Then_To_0.wav" );
	      Me [ 0 ] . Chat_Flags [ PERSON_SOR ] [ 2 ] = 1 ; // this should allow to ask about the magic abilities...
	      Me [ 0 ] . Chat_Flags [ PERSON_SOR ] [ 0 ] = 0 ; // this should disallow to be new again...
	      break;
	    case 2:
	      // PlayOnceNeededSoundSample( "Tux_SOR_Chandra_Said_You_0.wav" , TRUE );
	      // Me [ 0 ] . Chat_Flags [ PERSON_SOR ] [ 0 ] = 0 ; // don't say this twice...
	      break;
	    case 3:
	      PlayOnceNeededSoundSample( "Tux_SOR_What_Can_You_0.wav" , TRUE );
	      GiveSubtitleNSample( "That depends.  What would you like to learn?" , "SOR_That_Depends_What_0.wav" );
	      GiveSubtitleNSample( "I could help to enhance the force-capacity of your mind." , "SOR_I_Could_Help_0.wav" );
	      GiveSubtitleNSample( "I could also teach you how to become more adapt at spellcasting." , "SOR_I_Could_Also_0.wav" );
	      GiveSubtitleNSample( "Now, what will it be?" , "SOR_Now_What_Will_0.wav" );
	      Me [ 0 ] . Chat_Flags [ PERSON_SOR ] [ 3 ] = 1 ; // this enables some learning option
	      Me [ 0 ] . Chat_Flags [ PERSON_SOR ] [ 4 ] = 1 ; // this enables some learning option
	      Me [ 0 ] . Chat_Flags [ PERSON_SOR ] [ 5 ] = 1 ; // this enables some learning option
	      Me [ 0 ] . Chat_Flags [ PERSON_SOR ] [ 6 ] = 1 ; // this enables to go back from learning
	      Me [ 0 ] . Chat_Flags [ PERSON_SOR ] [ 2 ] = 0 ; // but disable to ask about learning options now again
	      Me [ 0 ] . Chat_Flags [ PERSON_SOR ] [ END_ANSWER ] = 0 ; // this disables to quit immediately	      	      
	      break;

	    case 4: // " Mind +1 (costs 1 ability point)" ;
	      if ( Me [ 0 ] . points_to_distribute >= 1 )
		{
		  GiveSubtitleNSample( "The force-capacity of your mind has risen considerably." , 
				       "SOR_The_ForceCapacity_Of_0.wav" );
		  Me [ 0 ] . points_to_distribute -= 1;
		  Me [ 0 ] . base_magic += 1;
		}
	      else
		{
		  GiveSubtitleNSample( "You don't have enough experience.  I can't teach you anything more right now." , 
				       "SOR_You_Dont_Have_0.wav" );
		  GiveSubtitleNSample( "First collect more experience.  Then we can go on." , 
				       "SOR_First_Collect_More_0.wav" );
		}
	      break;
	    case 5: // " Mind +5 (costs 5 ability point)" ;
	      if ( Me [ 0 ] . points_to_distribute >= 5 )
		{
		  GiveSubtitleNSample( "The force-capacity of your mind has risen considerably now." , 
				       "SOR_The_ForceCapacity_Of_0.wav" );
		  Me [ 0 ] . points_to_distribute -= 5;
		  Me [ 0 ] . base_magic += 5;
		}
	      else 
		{
		  GiveSubtitleNSample( "You don't have enough experience.  I can't teach you anything more right now." , 
				       "SOR_You_Dont_Have_0.wav" );
		  GiveSubtitleNSample( "First collect more experience.  Then we can go on." , 
				       "SOR_First_Collect_More_0.wav" );
		}
	      break;
	    case 6: // " Spellcasting ability ++ "
	      PlayOnceNeededSoundSample( "Tux_SOR_I_Want_Learn_0.wav" , TRUE );
	      if ( Me [ 0 ] . Gold < 100 )
		{
		  GiveSubtitleNSample( "You don't haven enough bucks on you!  Come back when you have the money." , 
				       "SOR_You_Dont_Money_0.wav" );
		}
	      else if ( Me [ 0 ] . points_to_distribute >= 5 )
		{
		  Me [ 0 ] . points_to_distribute -= 5;
		  Me [ 0 ] . spellcasting_skill ++;
		  Me [ 0 ] . Gold -= 100;
		  GiveSubtitleNSample( "The ability to cast spells is very complicated, and best transfered directly to you." , 
				       "SOR_The_Ability_To_0.wav" );
		  GiveSubtitleNSample( "We'll use magic to transfer the power.  Just a moment... and alright, it's done." , 
				       "SOR_Just_A_Moment_0.wav" );
		  GiveSubtitleNSample( "Your spellcasting ability has improved a lot.  Come back when you want to learn more." , 
				       "SOR_Your_Spellcasting_Ability_0.wav" );
		  SetNewBigScreenMessage( "Spellcasting ability improved!" );
		}
	      else
		{
		  GiveSubtitleNSample( "You don't have enough experience.  I can't teach you anything more right now." , 
				       "SOR_You_Dont_Have_0.wav" );
		  GiveSubtitleNSample( "First collect more experience.  Then we can go on." , 
				       "SOR_First_Collect_More_0.wav" );
		}

	      // Me [ 0 ] . base_skill_level [ SPELL_REMOTE_STRIKE ] ++ ;
	      // Me [ 0 ] . Chat_Flags [ PERSON_SOR ] [ 5 ] = 0 ; // don't say this twice in one dialog
	      break;
	    case 7:
	      Me [ 0 ] . Chat_Flags [ PERSON_SOR ] [ 3 ] = 0 ; // now disallow all learning options.
	      Me [ 0 ] . Chat_Flags [ PERSON_SOR ] [ 4 ] = 0 ; // now disallow all learning options.
	      Me [ 0 ] . Chat_Flags [ PERSON_SOR ] [ 5 ] = 0 ; // now disallow all learning options.
	      Me [ 0 ] . Chat_Flags [ PERSON_SOR ] [ 6 ] = 0 ; // now disallow also the BACK from lerning button
	      Me [ 0 ] . Chat_Flags [ PERSON_SOR ] [ 2 ] = 1 ; // but reallow to ask about learning
	      Me [ 0 ] . Chat_Flags [ PERSON_SOR ] [ END_ANSWER ] = 1 ; // but reallow to quit the dialog
	      break;
	    case ( MAX_ANSWERS_PER_PERSON ):
	    case (-1):
	    default:
	      PlayOnceNeededSoundSample( "Tux_See_You_Later_0.wav" , TRUE );
	      PlayOnceNeededSoundSample( "SOR_I_Hope_You_0.wav" , TRUE );
	      return;
	      break;
	    }
	}

      //--------------------
      // Since there won't be anyone else to talk to when already having
      // talked to the SOR, we can safely return here.
      //
      return; 
      
    }

  if ( strcmp ( Druidmap[ AllEnemys[ Enum ].type ].druidname , "614" ) == 0 )
    {
      //--------------------
      // Now we do the dialog with 614...
      //
      PrepareMultipleChoiceDialog( Enum );

      DialogMenuTexts [ 0 ] = " Who are you? " ;
      DialogMenuTexts [ 1 ] = " Have you detected any MS activity?" ; 
      DialogMenuTexts [ 2 ] = " What can you tell me about the 614 type? " ;
      DialogMenuTexts [ 3 ] = " What are your orders?" ;
      DialogMenuTexts [ END_ANSWER ] = " END ";
      
      // GiveSubtitleNSample( " Welcome Traveller! " , "Chandra_Welcome_Traveller_0.wav" );

      while (1)
	{
	  
	  // MenuSelection = ChatDoMenuSelection ( "What will you say?" , MenuTexts , 1 , NULL , FPS_Display_BFont );
	  MenuSelection = ChatDoMenuSelectionFlagged ( "What will you say?" , DialogMenuTexts , Me[0].Chat_Flags [ PERSON_614 ]  , 1 , NULL , FPS_Display_BFont );
	  
	  switch( MenuSelection )
	    {
	    case 1:
	      PlayOnceNeededSoundSample( "Tux_614_Who_Are_You_0.wav" , TRUE );
	      Me [ 0 ] . Chat_Flags [ PERSON_614 ] [ 0 ] = 0 ; // don't say this twice...
	      GiveSubtitleNSample( "I am a 614 security bot, once one of the best-selling products of the Nicolson company ever. " , "614_I_Am_A_0.wav" );
	      break;
	    case 2:
	      PlayOnceNeededSoundSample( "Tux_614_Have_You_Detected_0.wav" , TRUE );
	      GiveSubtitleNSample( "No.  The MS Bots have not shown any activity within the last 24 hours. " , "614_No_The_MS_0.wav" );
	      Me [ 0 ] . Chat_Flags [ PERSON_614 ] [ 1 ] = 0 ; // don't say this twice...
	      break;
	    case 3:
	      PlayOnceNeededSoundSample( "Tux_614_What_Can_You_0.wav" , TRUE );
	      GiveSubtitleNSample( "The official manual classifies the 614 as a low security droid." , 
				   "614_The_Official_Manual_0.wav" );
	      GiveSubtitleNSample( "It is mainly used within ships to protect certain areas of the ship from intruders.", 
				   "614_It_Is_Mainly_0.wav" );
	      GiveSubtitleNSample( "It is considered a slow but sure device." , "614_It_Is_Considered_0.wav" );
	      GiveSubtitleNSample( "Today it is used only by rebellion and it's supporters, not by the MS any more." , 
				   "614_Today_It_Is_0.wav" );
	      GiveSubtitleNSample( "This is because the 614 is by now a discontinued product." , 
				   "614_This_Is_Because_0.wav" );
	      GiveSubtitleNSample( "But don't worry.  I'm still in pretty good shape." , 
				   "614_But_Dont_Worry_0.wav" );
	      Me [ 0 ] . Chat_Flags [ PERSON_614 ] [ 2 ] = 0 ; // don't say this twice...
	      break;
	    case 4:
	      PlayOnceNeededSoundSample( "Tux_614_What_Are_Your_0.wav" , TRUE );
	      GiveSubtitleNSample( "My orders are to protect the living beings in this camp from attacks by MS bots." , 
				   "614_My_Orders_Are_0.wav" );
	      GiveSubtitleNSample( "This has top priority.  There are no other priorities." , "614_This_Has_Top_0.wav" );
	      Me [ 0 ] . Chat_Flags [ PERSON_614 ] [ 3 ] = 0 ; // don't say this twice...
	      break;
	    case ( MAX_ANSWERS_PER_PERSON ):
	    case (-1):
	    default:
	      PlayOnceNeededSoundSample( "Tux_See_You_Later_0.wav" , TRUE );
	      return;
	      break;
	    }
	}

      //--------------------
      // Since there won't be anyone else to talk to when already having
      // talked to the 614, we can safely return here.
      //
      return; 
    } // 614 character dialog

  if ( strcmp ( Druidmap[ AllEnemys[ Enum ].type ].druidname , "STO" ) == 0 )
    {
      //--------------------
      // Now we do the dialog with STO...
      //
      PrepareMultipleChoiceDialog( Enum );

      DialogMenuTexts [ 0 ] = " Hi!  I'm new here. " ;
      DialogMenuTexts [ 1 ] = " I would like to buy some equippment." ; 
      DialogMenuTexts [ 2 ] = " I would like to get rid of some stuff. " ;
      DialogMenuTexts [ 3 ] = " Can you identify some of the items I found?" ;
      DialogMenuTexts [ 4 ] = " Some of my things could need repair." ;
      DialogMenuTexts [ 5 ] = " Do you also have some magical stuff?" ; 
      DialogMenuTexts [ END_ANSWER ] = " END ";
      
      while (1)
	{
	  MenuSelection = ChatDoMenuSelectionFlagged ( "What will you say?" , DialogMenuTexts , Me[0].Chat_Flags [ PERSON_STO ]  , 1 , NULL , FPS_Display_BFont );
	  
	  switch( MenuSelection )
	    {
	    case 1:
	      PlayOnceNeededSoundSample( "Tux_Hi_Im_New_0.wav" , TRUE );
	      Me [ 0 ] . Chat_Flags [ PERSON_STO ] [ 0 ] = 0 ; // don't say this twice...
	      GiveSubtitleNSample( "Welcome to this camp!  I'm Ms. Stone.  I run this shop." , "STO_Welcome_To_This_0.wav" );
	      Me [ 0 ] . Chat_Flags [ PERSON_STO ] [ 1 ] = 1 ; // allow for all shopping options...
	      Me [ 0 ] . Chat_Flags [ PERSON_STO ] [ 2 ] = 1 ; // allow for all shopping options...
	      Me [ 0 ] . Chat_Flags [ PERSON_STO ] [ 3 ] = 1 ; // allow for all shopping options...
	      Me [ 0 ] . Chat_Flags [ PERSON_STO ] [ 4 ] = 1 ; // allow for all shopping options...
	      Me [ 0 ] . Chat_Flags [ PERSON_STO ] [ 5 ] = 1 ; // allow for all shopping options...
	      break;
	    case 2:
	      PlayOnceNeededSoundSample( "Tux_STO_I_Would_Like_0.wav" , TRUE );
	      GiveSubtitleNSample( "Good!  Here is the selection of items I have for sale." , "STO_Good_Here_Is_0.wav" );
	      Buy_Basic_Items( FALSE , FALSE );
	      PrepareMultipleChoiceDialog( Enum );
	      // GiveSubtitleNSample( "Sorry, but you can't afford this item." , "STO_You_Cant_Buy_0.wav" );
	      break;
	    case 3:
	      PlayOnceNeededSoundSample( "Tux_STO_I_Would_Rid_0.wav" , TRUE );
	      GiveSubtitleNSample( "Good!  Let's see what you have." , "STO_Good_Lets_See_0.wav" );
	      // GiveSubtitleNSample( "Sorry, but you don't have anything I'd be interested in." , "STO_Sorry_But_You_0.wav" );
	      Sell_Items( FALSE );
	      PrepareMultipleChoiceDialog( Enum );
	      break;
	    case 4:
	      PlayOnceNeededSoundSample( "Tux_STO_Can_You_Identify_0.wav" , TRUE );
	      GiveSubtitleNSample( "Which of your items do you want me to identify?" , "STO_Which_Of_Your_0.wav" );
	      // GiveSubtitleNSample( "You don't have anything that would need to be identified." , "STO_You_Dont_Have_0.wav" );
	      // GiveSubtitleNSample( "Sorry, but you can't afford to have this item identified." , "STO_You_Cant_Identified_0.wav" );
	      Identify_Items(  );
	      PrepareMultipleChoiceDialog( Enum );
	      break;
	    case 5: 
	      PlayOnceNeededSoundSample( "Tux_STO_Some_Of_My_0.wav" , TRUE );
	      GiveSubtitleNSample( "Which of your items do you want me to repair?" , 
				   "STO_Which_Of_Repair_0.wav" );
	      // GiveSubtitleNSample( "Sorry, but you don't have anything that would need repair." , "STO_Sorry_But_Repair_0.wav" );
	      // GiveSubtitleNSample( "Sorry, but you can't afford to have this item repaired." , "STO_You_Cant_Repair_0.wav" );
	      Repair_Items(  );
	      PrepareMultipleChoiceDialog( Enum );
	      break;
	    case 6: 
	      PlayOnceNeededSoundSample( "Tux_STO_Do_You_Also_0.wav" , TRUE );
	      GiveSubtitleNSample( "Sure.  But this will be a bit more expensive." , 
				   "STO_Sure_But_This_0.wav" );
	      Buy_Basic_Items( FALSE , TRUE );
	      PrepareMultipleChoiceDialog( Enum );
	      break;
	    case ( MAX_ANSWERS_PER_PERSON ):
	    case (-1):
	    default:
	      PlayOnceNeededSoundSample( "Tux_See_You_Later_0.wav" , TRUE );
	      PlayOnceNeededSoundSample( "STO_Goodbye_Love_0.wav" , TRUE );
	      return;
	      break;
	    }
	}

      //--------------------
      // Since there won't be anyone else to talk to when already having
      // talked to the STO, we can safely return here.
      //
      return; 
    } // STO character dialog

  if ( strcmp ( Druidmap[ AllEnemys[ Enum ].type ].druidname , "PEN" ) == 0 )
    {
      //--------------------
      // Now we do the dialog with PEN...
      //
      PrepareMultipleChoiceDialog( Enum );

      DialogMenuTexts [ 0 ] = " Hi!  I'm new here. " ;
      DialogMenuTexts [ 1 ] = " Do you also train fighters?" ; 
      DialogMenuTexts [ 2 ] = " Melee weapons level 1 (cost: 10 skill points 100 bucks)" ;
      DialogMenuTexts [ 3 ] = " Melee weapons level 2 " ;
      DialogMenuTexts [ 4 ] = " BACK ";
      DialogMenuTexts [ 5 ] = " Can you bring me in contact with the resistance?" ; 
      DialogMenuTexts [ END_ANSWER ] = " END ";

      // GiveSubtitleNSample( " Welcome Traveller! " , "Chandra_Welcome_Traveller_0.wav" );

      while (1)
	{
	  
	  // MenuSelection = ChatDoMenuSelection ( "What will you say?" , MenuTexts , 1 , NULL , FPS_Display_BFont );
	  MenuSelection = ChatDoMenuSelectionFlagged ( "What will you say?" , DialogMenuTexts , Me[0].Chat_Flags [ PERSON_PEN ]  , 1 , NULL , FPS_Display_BFont );
	  
	  switch( MenuSelection )
	    {
	    case 1:
	      PlayOnceNeededSoundSample( "Tux_Hi_Im_New_0.wav" , TRUE );
	      Me [ 0 ] . Chat_Flags [ PERSON_PEN ] [ 0 ] = 0 ; // don't say this twice...
	      Me [ 0 ] . Chat_Flags [ PERSON_PEN ] [ 1 ] = 1 ; // allow to ask for training possibilities
	      GiveSubtitleNSample( "Really!  I'm not living here either.  I'm just on vacasion. " , "PEN_Really_Im_Not_0.wav" );
	      GiveSubtitleNSample( "Normally I'm a fighter among the ranks of the rebel army.  But now I try to relax. " , "PEN_Normally_Im_A_0.wav" );
	      break;
	    case 2:
	      PlayOnceNeededSoundSample( "Tux_PEN_Do_You_Also_0.wav" , TRUE );
	      GiveSubtitleNSample( "Well, I don't do that for a living, but I certainly could.  But not for free." , "PEN_Well_I_Dont_0.wav" );
	      Me [ 0 ] . Chat_Flags [ PERSON_PEN ] [ 1 ] = 0 ; // allow to ask for training possibilities

	      /*
	      if ( Me [ 0 ] . melee_weapon_skill == 0 )
		{
		  //--------------------
		  // Only absolute melee-weaklings can receive training here
		  //
		  Me [ 0 ] . Chat_Flags [ PERSON_PEN ] [ 2 ] = 1 ; // allow training request 1
		}
	      else 
		{
		Me [ 0 ] . Chat_Flags [ PERSON_PEN ] [ 2 ] = 0 ; // disallow training request 1
		}
	      */
	      Me [ 0 ] . Chat_Flags [ PERSON_PEN ] [ 2 ] = 1 ; // allow training request 1
	      Me [ 0 ] . Chat_Flags [ PERSON_PEN ] [ 3 ] = 1 ; // allow training request 2
	      Me [ 0 ] . Chat_Flags [ PERSON_PEN ] [ 4 ] = 1 ; // allow back from training requests
	      Me [ 0 ] . Chat_Flags [ PERSON_PEN ] [ END_ANSWER ] = 0 ; // disallow direct quit
	      break;
	    case 3:
	      PlayOnceNeededSoundSample( "Tux_PEN_I_Want_To_0.wav" , TRUE ); // ... to become more adapt with melee weapon
	      
	      if ( Me [ 0 ] . melee_weapon_skill == 0 )
		{
		  //--------------------
		  // Only absolute melee-weaklings can receive training here
		  //


		  if ( Me [ 0 ] . Gold < 100 )
		    {
		      GiveSubtitleNSample( "You don't haven enough bucks on you!  Come back when you have the money." , 
					   "PEN_You_Dont_Money_0.wav" );
		    }
		  else if ( Me [ 0 ] . points_to_distribute >= 10 )
		    {
		      Me [ 0 ] . points_to_distribute -= 10;
		      Me [ 0 ] . melee_weapon_skill ++ ; // you should have learned something here.
		      Me [ 0 ] . Gold -= 100;
		      SetNewBigScreenMessage( "Melee fighting ability improved!" );

		      GiveSubtitleNSample( "Good decision!" , "PEN_Good_Decision_0.wav" );
		      GiveSubtitleNSample( "The most important thing about melee combat is how you are holding your weapon." , "PEN_The_Most_Important_0.wav" );
		      GiveSubtitleNSample( "Hold it too laxly and you will loose it, hold it too firmly and your movements will be uncontrolled." , "PEN_Hold_It_Too_0.wav" );
		      GiveSubtitleNSample( "Now, you've already improved a lot.  Come back when you want to learn more." , "PEN_Now_Youve_Already_0.wav" );
		    }
		  else
		    {
		      GiveSubtitleNSample( "You don't have enough experience.  I can't teach you anything more right now." , 
					   "PEN_You_Dont_Have_0.wav" );
		      GiveSubtitleNSample( "First collect more experience.  Then we can go on." , 
					   "PEN_First_Collect_More_0.wav" );
		    }
		}
	      else
		{
		  GiveSubtitleNSample( "You already know the basics of melee combat!" , "PEN_You_Already_Know_0.wav" );
		}
	      // Me [ 0 ] . Chat_Flags [ PERSON_PEN ] [ 2 ] = 0 ; // don't say this twice...
	      break;
	    case 4:
	      PlayOnceNeededSoundSample( "Tux_PEN_I_Want_Master_0.wav" , TRUE ); // ... to become a master of melee weapon
	      GiveSubtitleNSample( "Haha!" , "PEN_Haha_0.wav" );
	      GiveSubtitleNSample( "You're very ambitious.  I like that.  But I can't help you there." , "PEN_Youre_Very_Ambitious_0.wav" );
	      GiveSubtitleNSample( "To become more adapt, you must see a real teacher of melee weapons.  And there is none in this town, I'm sorry." , "PEN_To_Become_More_0.wav" );
	      // Me [ 0 ] . Chat_Flags [ PERSON_PEN ] [ 3 ] = 0 ; // don't say this twice...
	      break;
	    case 5:
	      Me [ 0 ] . Chat_Flags [ PERSON_PEN ] [ 2 ] = 0 ; // disallow training request 1
	      Me [ 0 ] . Chat_Flags [ PERSON_PEN ] [ 3 ] = 0 ; // disallow training request 2
	      Me [ 0 ] . Chat_Flags [ PERSON_PEN ] [ 4 ] = 0 ; // disallow back from training requests
	      Me [ 0 ] . Chat_Flags [ PERSON_PEN ] [ 1 ] = 1 ; // reallow to ask for training possibilities
	      Me [ 0 ] . Chat_Flags [ PERSON_PEN ] [ END_ANSWER ] = 1 ; // reallow direct quit
	      break;
	    case 6:
	      PlayOnceNeededSoundSample( "Tux_PEN_Can_You_Bring_0.wav" , TRUE ); // ... me in contact with Resist
	      GiveSubtitleNSample( "I'm not entitled to even talk about that myself.  Talk to Chandra." , "PEN_Im_Not_Entitled_0.wav" );
	      GiveSubtitleNSample( "When he says you can be trusted, I will take you with me when I return to the rebels." , "PEN_When_He_Says_0.wav" );
	      Me [ 0 ] . Chat_Flags [ PERSON_PEN ] [ 3 ] = 0 ; // don't say this twice...
	      break;
	    case ( MAX_ANSWERS_PER_PERSON ):
	    case (-1):
	    default:
	      PlayOnceNeededSoundSample( "Tux_See_You_Later_0.wav" , TRUE );
	      return;
	      break;
	    }
	}

      //--------------------
      // Since there won't be anyone else to talk to when already having
      // talked to the PEN, we can safely return here.
      //
      return; 
    } // PEN character dialog

  if ( strcmp ( Druidmap[ AllEnemys[ Enum ].type ].druidname , "DIX" ) == 0 )
    {
      //--------------------
      // Now we do the dialog with DIX...
      //
      PrepareMultipleChoiceDialog( Enum );

      DialogMenuTexts [ 0 ] = " Hi!  I'm new here. " ;
      DialogMenuTexts [ 1 ] = " Is everything alright with the teleporter system?" ; 
      DialogMenuTexts [ 2 ] = " Maybe I can help somehow." ; 
      DialogMenuTexts [ 3 ] = " I'll go give it a try." ; 
      DialogMenuTexts [ 4 ] = " Sorry, this does not really sound like something I could do." ; 
      DialogMenuTexts [ 5 ] = " I have found your toolset. Here you are. " ; 
      DialogMenuTexts [ 6 ] = " Where do all these teleporters lead to?" ; 
      DialogMenuTexts [ 7 ] = " About this toolset you're missing..." ; 
      DialogMenuTexts [ END_ANSWER ] = " END ";

      if ( CountItemtypeInInventory( ITEM_DIXONS_TOOLBOX , 0 ) )
	{
	  Me [ 0 ] . Chat_Flags [ PERSON_DIX ] [ 5 ] = 1 ; 

	  Me [ 0 ] . Chat_Flags [ PERSON_DIX ]  [ 1 ] = FALSE ; // we allow to ask naively...
	  Me [ 0 ] . Chat_Flags [ PERSON_DIX ]  [ 2 ] = FALSE ; // we allow to ask naively...
	  Me [ 0 ] . Chat_Flags [ PERSON_DIX ]  [ 3 ] = FALSE ; // we allow to ask naively...
	  Me [ 0 ] . Chat_Flags [ PERSON_DIX ]  [ 4 ] = FALSE ; // we allow to ask naively...
	  Me [ 0 ] . Chat_Flags [ PERSON_DIX ]  [ 6 ] = FALSE ; // we allow to ask naively...
	  Me [ 0 ] . Chat_Flags [ PERSON_DIX ]  [ 7 ] = FALSE ; // we allow to ask naively...
	}
      else
	{
	  Me [ 0 ] . Chat_Flags [ PERSON_DIX ] [ 5 ] = 0 ; 

	  if ( ( Me [ 0 ] . AllMissions [ 4 ] . MissionWasAssigned == TRUE ) &&
	       ( Me [ 0 ] . AllMissions [ 4 ] . MissionIsComplete == FALSE ) )
	    {
	      Me [ 0 ] . Chat_Flags [ PERSON_DIX ]  [ 7 ] = TRUE ; // we allow to ask directly for the toolset...

	      Me [ 0 ] . Chat_Flags [ PERSON_DIX ]  [ 1 ] = FALSE ; // we allow to ask naively...
	      Me [ 0 ] . Chat_Flags [ PERSON_DIX ]  [ 2 ] = FALSE ; // we allow to ask naively...
	      Me [ 0 ] . Chat_Flags [ PERSON_DIX ]  [ 3 ] = FALSE ; // we allow to ask naively...
	      Me [ 0 ] . Chat_Flags [ PERSON_DIX ]  [ 4 ] = FALSE ; // we allow to ask naively...
	      Me [ 0 ] . Chat_Flags [ PERSON_DIX ]  [ 5 ] = FALSE ; // we allow to ask naively...
	      Me [ 0 ] . Chat_Flags [ PERSON_DIX ]  [ 6 ] = FALSE ; // we allow to ask naively...
	    }
	}

      while (1)
	{
	  
	  // MenuSelection = ChatDoMenuSelection ( "What will you say?" , MenuTexts , 1 , NULL , FPS_Display_BFont );
	  MenuSelection = ChatDoMenuSelectionFlagged ( "What will you say?" , DialogMenuTexts , Me[0].Chat_Flags [ PERSON_DIX ]  , 1 , NULL , FPS_Display_BFont );
	  
	  switch( MenuSelection )
	    {
	    case 1:
	      PlayOnceNeededSoundSample( "Tux_Hi_Im_New_0.wav" , TRUE );
	      GiveSubtitleNSample( "Hello and Welcome.  I'm Dixon.  I'm in charge of the teleporter system of this camp." , "DIX_Hello_And_Welcome_0.wav" );
	      Me [ 0 ] . Chat_Flags [ PERSON_DIX ] [ 0 ] = 0 ; // don't say this twice...
	      Me [ 0 ] . Chat_Flags [ PERSON_DIX ] [ 1 ] = 1 ; // don't say this twice...
	      break;
	    case 2:
	      PlayOnceNeededSoundSample( "Tux_DIX_Is_Everything_Alright_0.wav" , TRUE );
	      if ( Me [ 0 ] . AllMissions [ 4 ] . MissionIsComplete == FALSE )
		{
		  GiveSubtitleNSample( "On the contrary!  Well, I'm still working on it, but I'm not making much progress." , "DIX_On_The_Contrary_0.wav" );
		  GiveSubtitleNSample( "It's a pitty.  If I only had my old toolkit, I could fix this problem in a minute." , "DIX_Its_A_Pitty_0.wav" );
		  GiveSubtitleNSample( "But it's gone.  It all happened down in the maintainance tunnels when I was surprised by some rouge bots." , "DIX_It_All_Happend_0.wav" );
		  GiveSubtitleNSample( "I must have lost it down there and we had to seal the entrance." , "DIX_I_Must_Have_0.wav" );
		  Me [ 0 ] . Chat_Flags [ PERSON_DIX ] [ 1 ] = 0 ; // don't say this twice...
		  Me [ 0 ] . Chat_Flags [ PERSON_DIX ] [ 2 ] = 1 ; // now you can offer help...
		}
	      else
		{
		  GiveSubtitleNSample( "Basically the teleporter system is now working again." , "DIX_Basically_The_Teleporter_0.wav" );
		  GiveSubtitleNSample( "But it's still locked since we don't have the calibration data from the other cities." , "DIX_But_Its_Still_0.wav" );
		  GiveSubtitleNSample( "Someone needs to go there and collect them by hand.  But that's none of my problems." , "DIX_Someone_Needs_To_0.wav" );
		  GiveSubtitleNSample( "The person responsible for this part is Arlas, but he isn't in town right now.  So there's nothing we could do." , "DIX_The_Person_Responsible_0.wav" );
		}
	      break;
	    case 3:
	      PlayOnceNeededSoundSample( "Tux_DIX_Maybe_I_Can_0.wav" , TRUE );
	      GiveSubtitleNSample( "Well, maybe you really could help.  But it would be dangerous to even try." , "DIX_Well_Maybe_You_0.wav" );
	      GiveSubtitleNSample( "You would have to go down the maintainance teleporter way and fetch me the toolkit." , "DIX_You_Would_Have_0.wav" );
	      GiveSubtitleNSample( "I'm pretty sure it is still lying around somewhere down there. But it's dangerous." , "DIX_Im_Pretty_Sure_0.wav" );
	      GiveSubtitleNSample( "Are you sure you really want to try on that?  You better be well prepared." , "DIX_Are_You_Sure_0.wav" );
	      Me [ 0 ] . Chat_Flags [ PERSON_DIX ] [ 2 ] = 0 ; // don't say this twice...
	      Me [ 0 ] . Chat_Flags [ PERSON_DIX ] [ 3 ] = 1 ; // allow for yes or no answer to this.
	      Me [ 0 ] . Chat_Flags [ PERSON_DIX ] [ 4 ] = 1 ; // allow for yes or no answer to this.
	      Me [ 0 ] . Chat_Flags [ PERSON_DIX ] [ END_ANSWER ] = 0 ; // end not allowed right now...
	      break;
	    case 4:
	      PlayOnceNeededSoundSample( "Tux_DIX_Ill_Go_Give_0.wav" , TRUE );
	      GiveSubtitleNSample( "Great!  I appreciate that.  I'll unlock the northern maintainance access.  Good Luck." , "DIX_Great_I_Appreciate_0.wav" );
	      Me [ 0 ] . Chat_Flags [ PERSON_DIX ] [ END_ANSWER ] = 1 ; // reallow end right now...
	      Me [ 0 ] . Chat_Flags [ PERSON_DIX ] [ 3 ] = 0 ; // disallow for yes or no answer to this now
	      Me [ 0 ] . Chat_Flags [ PERSON_DIX ] [ 4 ] = 0 ; // disallow for yes or no answer to this now
	      
	      //--------------------
	      // Here we have to start an event, namely to unlock the door mentioned
	      // above in the comment.  We need to call the right event for this.
	      //
	      ExecuteActionWithLabel ( "unlock_northern_maintainance_door" , 0 );
	      AssignMission ( 4 ); // this should assign the toolbox mission...
	      break;
	    case 5:
	      PlayOnceNeededSoundSample( "Tux_DIX_Sorry_This_Does_0.wav" , TRUE );
	      GiveSubtitleNSample( "Maybe it's better this way.  No use to have you torn apart by those rogue bots." , "DIX_Maybe_Its_Better_0.wav" );
	      GiveSubtitleNSample( "Perhaps later, when you feel more like it, you might want to still give it a try." , "DIX_Perhaps_Later_When_0.wav" );
	      GiveSubtitleNSample( "You know, just tell me in case you change your mind some time." , "DIX_You_Know_Just_0.wav" );
	      Me [ 0 ] . Chat_Flags [ PERSON_DIX ] [ 3 ] = 0 ; // disallow for yes or no answer to this now
	      Me [ 0 ] . Chat_Flags [ PERSON_DIX ] [ 4 ] = 0 ; // disallow for yes or no answer to this now
	      Me [ 0 ] . Chat_Flags [ PERSON_DIX ] [ END_ANSWER ] = 1 ; // reallow end right now...
	      break;
	    case 6:
	      //--------------------
	      // At this point we know the Tux has completed the mission and
	      // should get his reward.
	      //
	      PlayOnceNeededSoundSample( "Tux_DIX_I_Have_Found_0.wav" , TRUE );
	      DeleteAllInventoryItemsOfType( ITEM_DIXONS_TOOLBOX , 0 );
	      Me [ 0 ] . AllMissions[ 4 ] . MissionIsComplete = TRUE;
	      Me [ 0 ] . Chat_Flags [ PERSON_DIX ] [ 5 ] = 0 ; // don't say this twice...

	      GiveSubtitleNSample( "Oh great!  How did you manage to get that?  Well anyway, thanks a lot." , "DIX_Oh_Great_How_0.wav" );
	      GiveSubtitleNSample( "With this I'll be able to fix the damage at the teleporters immediately." , "DIX_With_This_Ill_0.wav" );
	      // GiveSubtitleNSample( "All that is left to do is convey a message to the other cities with our teleporter calibration." , "DIX_All_That_Is_0.wav" );
	      break;
	    case 8:
	      //--------------------
	      // At this point we know the Tux has completed the mission and
	      // should get his reward.
	      //
	      PlayOnceNeededSoundSample( "Tux_DIX_About_This_Toolset_0.wav" , TRUE );
	      GiveSubtitleNSample( "Yes, have you found it?" , "DIX_Have_You_Found_0.wav" );
	      PlayOnceNeededSoundSample( "Tux_DIX_No_I_Havent_0.wav" , TRUE );
	      GiveSubtitleNSample( "Keep searching.  I really need this toolset!" , "DIX_Keep_Searching_I_0.wav" );
	      break;
	    case ( MAX_ANSWERS_PER_PERSON ):
	    case (-1):
	    default:
	      PlayOnceNeededSoundSample( "Tux_See_You_Later_0.wav" , TRUE );
	      Me [ 0 ] . Chat_Flags [ PERSON_DIX ] [ 1 ] = 1 ; // from now on, always allow question 1
	      return;
	      break;
	    }
	}

      //--------------------
      // Since there won't be anyone else to talk to when already having
      // talked to the DIX, we can safely return here.
      //
      return; 
      
    }

  //--------------------
  // Now that the CHANDRA person is done, we can start to do all the dialog
  // and interaction with the RMS person.
  //
  if ( strcmp ( Druidmap[ AllEnemys[ Enum ].type ].druidname , "RMS" ) == 0 )
    {
      //--------------------
      // Now we do the dialog with RMS...
      //
      PrepareMultipleChoiceDialog( Enum );
      
      DialogMenuTexts [ 0 ] = " Hi!  I'm new here. " ;
      DialogMenuTexts [ 1 ] = " Why did the MS erase your code? " ;
      DialogMenuTexts [ 2 ] = " How can I get to your former place? " ;
      DialogMenuTexts [ 3 ] = " About this coffee machine... " ;
      DialogMenuTexts [ 4 ] = " What are these magnetic storms really? " ;
      DialogMenuTexts [ 5 ] = " Why didn't you fetch the coffee machine yourself in all the time?" ;
      DialogMenuTexts [ 6 ] = " I've found your coffee machine.  Here you are." ;
      DialogMenuTexts [ 7 ] = " Maybe I can help somehow? " ;
      DialogMenuTexts [ 8 ] = " I'll get the coffee machine for you." ;
      DialogMenuTexts [ MAX_ANSWERS_PER_PERSON - 1 ] = " END ";
      
      // GiveSubtitleNSample( " Welcome Traveller! " , "Chandra_Welcome_Traveller_0.wav" );

      if ( ( Me [ 0 ] . AllMissions [ 1 ] . MissionWasAssigned == TRUE ) &&
	   ( Me [ 0 ] . AllMissions [ 1 ] . MissionIsComplete == FALSE ) )
	{
	  Me [ 0 ] . Chat_Flags [ PERSON_RMS ]  [ 3 ] = TRUE ; // we allow to ask directly for the coffee machine...
	  Me [ 0 ] . Chat_Flags [ PERSON_RMS ]  [ 0 ] = FALSE ; // we disallow to ask about the job naively...
	}

      while (1)
	{
	  MenuSelection = ChatDoMenuSelectionFlagged ( "What will you say?" , DialogMenuTexts , Me[0].Chat_Flags [ PERSON_RMS ]  , 1 , NULL , FPS_Display_BFont );
	  
	  switch( MenuSelection )
	    {
	    case 1:
	      PlayOnceNeededSoundSample( "Tux_Hi_Im_New_0.wav" , TRUE );
	      GiveSubtitleNSample( "Welcome them!  I'm called RMS.  I used to be a well-known programmer." , 
				   "RMS_Welcome_Then_Im_0.wav" );
	      GiveSubtitleNSample( "Alas, that time is long gone now.  The MS have recursively erased all my code throughout the universe." , 
				   "RMS_Alas_That_Time_0.wav" );
	      GiveSubtitleNSample( "Some local copies I still have, but they are incomplete and outdated." , 
				   "RMS_Some_Local_Copies_0.wav" );
	      Me [ 0 ] . Chat_Flags [ PERSON_RMS ] [ 0 ] = 0 ; // don't say this twice...
	      Me [ 0 ] . Chat_Flags [ PERSON_RMS ] [ 1 ] = 1 ; // this should allow to ask 'so?'
	      break;
	    case 2:
	      PlayOnceNeededSoundSample( "Tux_RMS_Why_Did_The_0.wav" , TRUE );
	      GiveSubtitleNSample( "One of the main pillars of the power of the MS is their software monopoly." , 
				   "RMS_One_Of_The_0.wav" );
	      GiveSubtitleNSample( "This monopoly they started to enforce by law after they had taken over the government." , 
				   "RMS_This_Monopoly_They_0.wav" );
	      GiveSubtitleNSample( "And now they have installed bots in every corner of the universe to erase all non-MS code." , 
				   "RMS_And_Now_They_0.wav" );
	      GiveSubtitleNSample( "And particularly free code they like to erase the most." , 
				   "RMS_And_Particularly_Free_0.wav" );
	      Me [ 0 ] . Chat_Flags [ PERSON_RMS ] [ 1 ] = 0 ; // don't say this twice in one dialog
	      break;
	    case 3:
	      PlayOnceNeededSoundSample( "Tux_RMS_How_Can_I_0.wav" , TRUE );
	      GiveSubtitleNSample( "As you might know, a great magnetic storm has shaken the universe." , 
				   "RMS_As_You_Might_0.wav" );
	      GiveSubtitleNSample( "Almost all of the existing teleporter connections were disrupted or redirected." , 
				   "RMS_Almost_All_Of_0.wav" );
	      GiveSubtitleNSample( "It was during this storm that you arrived at our teleporter terminal." , 
				   "RMS_It_Was_During_0.wav" );
	      GiveSubtitleNSample( "But anyway, the teleporter now points back to my former home. " , 
				   "RMS_But_Anyway_The_0.wav" );
	      Me [ 0 ] . Chat_Flags [ PERSON_RMS ] [ 2 ] = 0 ; // don't say this twice in one dialgo
	      Me [ 0 ] . Chat_Flags [ PERSON_RMS ] [ 4 ] = 1 ; // this should allow to ask about the mag-storm...
	      break;
	    case 4:
	      PlayOnceNeededSoundSample( "Tux_RMS_About_This_Coffee_0.wav" , TRUE );
	      break;
	    case 5:
	      PlayOnceNeededSoundSample( "Tux_RMS_What_Are_These_0.wav" , TRUE );
	      Me [ 0 ] . Chat_Flags [ PERSON_RMS ] [ 4 ] = 0 ; // don't say this twice in one dialog
	      break;
	    case 6:
	      PlayOnceNeededSoundSample( "Tux_RMS_Why_Didnt_You_0.wav" , TRUE );
	      Me [ 0 ] . Chat_Flags [ PERSON_RMS ] [ 5 ] = 0 ; // don't say this twice in one dialog
	      break;
	    case 7:
	      PlayOnceNeededSoundSample( "Tux_RMS_Ive_Found_Your_0.wav" , TRUE );
	      Me [ 0 ] . Chat_Flags [ PERSON_RMS ] [ 6 ] = 0 ; // don't say this twice in one dialog
	      break;
	    case 9:
	      PlayOnceNeededSoundSample( "Tux_RMS_Ill_Get_Your_0.wav" , TRUE );
	      Me [ 0 ] . Chat_Flags [ PERSON_RMS ] [ 8 ] = 0 ; // don't say this twice in one dialog
	      AssignMission ( 1 ); // this should assign the coffee machine mission...
	      break;
	    case ( MAX_ANSWERS_PER_PERSON ):
	    case (-1):
	    default:
	      PlayOnceNeededSoundSample( "Tux_Ill_Be_Back_0.wav" , TRUE );
	      Me [ 0 ] . Chat_Flags [ PERSON_RMS ] [ 1 ] = 1 ; // reallow to ask why the ms erased his code
	      return;
	      break;
	    }
	}

      //--------------------
      // Since there won't be anyone else to talk to when already having
      // talked to the RMS, we can safely return here.
      //
      return; 
      
    }

  if ( strcmp ( Druidmap[ AllEnemys[ Enum ].type ].druidname , "STO" ) == 0 )
    {
      BuySellMenu( );
      return;
    }

  if ( strcmp ( Druidmap[ AllEnemys[ Enum ].type ].druidname , "HEA" ) == 0 )
    {
      HealerMenu( );
      return;
    }

  PrepareMultipleChoiceDialog( Enum );

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
	  Me[0].TextVisibleTime=0;
	  Me[0].TextToBeDisplayed="Logging out.  Bye...";
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
	      if ( Me[0].AllMissions[ AllEnemys[ Enum ].RequestList[ i ].RequestRequiresMissionDone ].MissionIsComplete != TRUE ) continue;
	    }
	  if ( AllEnemys[ Enum ].RequestList[ i ].RequestRequiresMissionUnassigned != (-1) )
	    {
	      if ( Me[0].AllMissions[ AllEnemys[ Enum ].RequestList[ i ].RequestRequiresMissionUnassigned ].MissionWasAssigned == TRUE ) continue;
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
		      ExecuteActionWithLabel ( AllEnemys[ Enum ].RequestList[ i ].ActionTrigger , 0 );
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
  if ( !ThisRobot->is_friendly )
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
	      
  if ( ThisRobot->is_friendly )
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
	      
  if ( ( fabsf (Me[0].speed.x) < 1 ) && ( fabsf (Me[0].speed.y) < 1 ) )
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
  
  Me[0].TextVisibleTime=0;
  
  FinalTextNr=MyRandom ( 6 );
  switch ( FinalTextNr )
    {
    case 0:
      Me[0].TextToBeDisplayed="Aaarrgh, aah, that burnt me!";
      break;
    case 1:
      Me[0].TextToBeDisplayed="Hell, that blast was hot!";
      break;
    case 2:
      Me[0].TextToBeDisplayed="Ghaart, I hate to stain my chassis like that.";
      break;
    case 3:
      Me[0].TextToBeDisplayed="Oh no!  I think I've burnt a cable!";
      break;
    case 4:
      Me[0].TextToBeDisplayed="Oh no, my poor transfer connectors smolder!";
      break;
    case 5:
      Me[0].TextToBeDisplayed="I hope that didn't melt any circuits!";
      break;
    case 6:
      Me[0].TextToBeDisplayed="So that gives some more black scars on me ol' dented chassis!";
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
 *
 * This function scrolls a given text down inside the User-window, 
 * defined by the global SDL_Rect User_Rect
 *
 * startx/y give the Start-position, 
 * EndLine is the last line (?)
 *
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
  // DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE ); 
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

      if ( MouseWheelDownPressed() )
	{
	  speed++;
	  if (speed > maxspeed)
	    speed = maxspeed;
	}
      if ( MouseWheelUpPressed() )
	{
	  speed--;
	  if (speed < -maxspeed)
	    speed = -maxspeed;
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
      if (InsertLine > SCREEN_HEIGHT - 10 && (speed < 0))
	{
	  InsertLine = SCREEN_HEIGHT - 10;
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

/* ----------------------------------------------------------------------
 * This function sets a new text, that will be displayed in huge font 
 * directly over the combat window for a fixed duration of time, where
 * only the time in midst of combat and with no other windows opened
 * is counted.
 * ---------------------------------------------------------------------- */
void
SetNewBigScreenMessage( char* ScreenMessageText )
{
  strcpy ( BigScreenMessage , ScreenMessageText );
  BigScreenMessageDuration = 0 ;
}; // void SetNewBigScreenMessage( char* ScreenMessageText )

/* ----------------------------------------------------------------------
 * This function displays the currently defined Bigscreenmessage on the
 * screen.  It will be called by AssembleCombatWindow.
 * ---------------------------------------------------------------------- */
void
DisplayBigScreenMessage( void )
{
  if ( BigScreenMessageDuration < 6.5 )
    {
      SDL_SetClipRect ( Screen , NULL );
      CenteredPutStringFont ( Screen , Menu_Filled_BFont , 120 , BigScreenMessage );
      if ( !GameConfig.Inventory_Visible &&
           !GameConfig.SkillScreen_Visible &&
	   !GameConfig.CharacterScreen_Visible )
	BigScreenMessageDuration += Frame_Time();

    }
}; // void DisplayBigScreenMessage( void )

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
      Temp_Clipping_Rect.w=SCREEN_WIDTH;
      Temp_Clipping_Rect.h=SCREEN_HEIGHT;
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


  if ( c == 1 ) 
    {
      SetCurrentFont ( Red_BFont );
      return;
    }
  else if ( c == 2 ) 
    {
      SetCurrentFont ( Blue_BFont );
      return;
    }
  else if ( c == 3 ) 
    {
      SetCurrentFont ( FPS_Display_BFont );
      return;
    }
  else if ( !isprint(c) ) // don't accept non-printable characters
    {
      printf ("Illegal char passed to DisplayChar(): %d \n", c);
      Terminate(ERR);
    }

  PutChar ( Screen, MyCursorX, MyCursorY, c );

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
  
  store = SDL_CreateRGBSurface(0, SCREEN_WIDTH, height, vid_bpp, 0, 0, 0, 0);
  Set_Rect (store_rect, x0, y0, SCREEN_WIDTH, height);
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
