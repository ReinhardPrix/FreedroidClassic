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

#define MAX_BIG_SCREEN_MESSAGES 5

int BigScreenMessageIndex = 0 ;
char BigScreenMessage[ MAX_BIG_SCREEN_MESSAGES ] [ 5000 ];
float BigScreenMessageDuration[ MAX_BIG_SCREEN_MESSAGES ] = { 10000, 10000, 10000, 10000, 10000 } ;

SDL_Surface* Background;

#define MAX_DIALOGUE_OPTIONS_IN_ROSTER 100
#define MAX_REPLIES_PER_OPTION 100
#define MAX_SUBTITLES_N_SAMPLES_PER_DIALOGUE_OPTION 20
#define MAX_EXTRAS_PER_OPTION 10
#define CHAT_DEBUG_LEVEL 1

typedef struct
{
  char* option_text;
  char* option_sample_file_name;

  char* reply_sample_list[ MAX_REPLIES_PER_OPTION ] ;
  char* reply_subtitle_list[ MAX_REPLIES_PER_OPTION ];

  char* extra_list[ MAX_EXTRAS_PER_OPTION ];

  char* on_goto_condition;
  int on_goto_first_target;
  int on_goto_second_target;

  int change_option_nr [ MAX_DIALOGUE_OPTIONS_IN_ROSTER ];
  int change_option_to_value [ MAX_DIALOGUE_OPTIONS_IN_ROSTER ];
}
dialogue_option, *Dialogue_option;

dialogue_option ChatRoster[MAX_DIALOGUE_OPTIONS_IN_ROSTER];

/* ----------------------------------------------------------------------
 * In some cases it will be nescessary to inform the user of something in
 * a big important style.  Then a popup window is suitable, with a mouse
 * button to confirm and make it go away again.
 * ---------------------------------------------------------------------- */
void
GiveMouseAlertWindow( char* WindowText )
{
  SDL_Rect TargetRect;

  TargetRect . w = 440 ; 
  TargetRect . h = 340 ; 
  TargetRect . x = ( 640 - TargetRect . w ) / 2 ; 
  TargetRect . y = ( 480 - TargetRect . h ) / 2 ; 
  SDL_FillRect ( Screen , &TargetRect , 
		 SDL_MapRGB ( Screen->format, 0 , 0 , 0 ) ) ;
  
#define IN_WINDOW_TEXT_OFFSET 15
  TargetRect . w -= IN_WINDOW_TEXT_OFFSET;
  TargetRect . h -= IN_WINDOW_TEXT_OFFSET;
  TargetRect . x += IN_WINDOW_TEXT_OFFSET;
  TargetRect . y += IN_WINDOW_TEXT_OFFSET;

  SetCurrentFont ( FPS_Display_BFont );

  DisplayText ( WindowText, TargetRect . x, TargetRect . y , &TargetRect )  ;

  SDL_Flip ( Screen );

  while (!EnterPressed() && !SpacePressed() ) ;
  while (EnterPressed() || SpacePressed() ) ;

}; // void GiveMouseAlertWindow( char* WindowText )

/* ----------------------------------------------------------------------
 * This function finds the index of the array where the chat flags for
 * this person are stored.  It does this by exploiting on the (unique?)
 * dialog section to use entry of each (friendly) droid.
 * ---------------------------------------------------------------------- */
int
ResolveDialogSectionToChatFlagsIndex ( char* SectionName )
{
  if ( strcmp ( SectionName , "Chandra" ) == 0 ) return PERSON_CHA ;
  if ( strcmp ( SectionName , "Sorenson" ) == 0 ) return PERSON_SORENSON;
  if ( strcmp ( SectionName , "614" ) == 0 ) return PERSON_614;
  if ( strcmp ( SectionName , "STO" ) == 0 ) return PERSON_STONE;
  if ( strcmp ( SectionName , "PEN" ) == 0 ) return PERSON_PENDRAGON;
  if ( strcmp ( SectionName , "DIX" ) == 0 ) return PERSON_DIXON;
  if ( strcmp ( SectionName , "RMS" ) == 0 ) return PERSON_RMS;
  if ( strcmp ( SectionName , "MER" ) == 0 ) return PERSON_MER;
  if ( strcmp ( SectionName , "Francis" ) == 0 ) return PERSON_FRANCIS;
  if ( strcmp ( SectionName , "Ernie" ) == 0 ) return PERSON_ERNIE;
  if ( strcmp ( SectionName , "Bruce" ) == 0 ) return PERSON_BRUCE;
  if ( strcmp ( SectionName , "Benjamin" ) == 0 ) return PERSON_BENJAMIN;
  if ( strcmp ( SectionName , "Bender" ) == 0 ) return PERSON_BENDER;
  if ( strcmp ( SectionName , "Spencer" ) == 0 ) return PERSON_SPENCER;
  if ( strcmp ( SectionName , "Butch" ) == 0 ) return PERSON_BUTCH;
  if ( strcmp ( SectionName , "Darwin" ) == 0 ) return PERSON_DARWIN;
  if ( strcmp ( SectionName , "Duncan" ) == 0 ) return PERSON_DUNCAN;
  if ( strcmp ( SectionName , "Doc Moore" ) == 0 ) return PERSON_DOC_MOORE;
  if ( strcmp ( SectionName , "Melfis" ) == 0 ) return PERSON_MELFIS;
  if ( strcmp ( SectionName , "Michelangelo" ) == 0 ) return PERSON_MICHELANGELO;
  if ( strcmp ( SectionName , "Skippy" ) == 0 ) return PERSON_SKIPPY;
  if ( strcmp ( SectionName , "StandardOldTownGateGuard" ) == 0 ) return PERSON_STANDARD_OLD_TOWN_GATE_GUARD;
  if ( strcmp ( SectionName , "StandardNewTownGateGuard" ) == 0 ) return PERSON_STANDARD_NEW_TOWN_GATE_GUARD;
  if ( strcmp ( SectionName , "OldTownGateGuardLeader" ) == 0 ) return PERSON_OLD_TOWN_GATE_GUARD_LEADER;
  if ( strcmp ( SectionName , "StandardMSFacilityGateGuard" ) == 0 ) return PERSON_STANDARD_MS_FACILITY_GATE_GUARD;
  if ( strcmp ( SectionName , "MSFacilityGateGuardLeader" ) == 0 ) return PERSON_MS_FACILITY_GATE_GUARD_LEADER;
  if ( strcmp ( SectionName , "HEA" ) == 0 ) return PERSON_HEA;

  GiveStandardErrorMessage ( "ResolveDialogSectionToChatFlagsIndex(...)" , "\
There was a dialogue section to be used with a droid, that does not have a \n\
corresponding chat flags array index." ,
			     PLEASE_INFORM, IS_FATAL );
  return (-1);

}; // int ResolveDialogSectionToChatFlagsIndex ( Enemy ChatDroid )

/* ----------------------------------------------------------------------
 * This function plants a cookie, i.e. sets a new text string with the
 * purpose of serving as a flag.  These flags can be set from the dialog
 * file and used from within there and they get stored and loaded with
 * every gave via the influence_t structure.
 * ---------------------------------------------------------------------- */
void
PlantCookie ( char* CookieString , int PlayerNum )
{
  int i;

  //--------------------
  // First a security check against attempts to plant too long cookies...
  //
  if ( strlen ( CookieString ) >= MAX_COOKIE_LENGTH -1 )
    {
      fprintf( stderr, "\n\nCookieString: %s\n" , CookieString );
      GiveStandardErrorMessage ( "PlantCookie(...)" , "\
There was a cookie given that exceeds the maximal length allowed for a\n\
cookie to be set in FreedroidRPG.",
				 PLEASE_INFORM, IS_FATAL );
    }

  //--------------------
  // Check if maybe the cookie has already been set.  In this case we would
  // not have to do anything...
  //
  for ( i = 0 ; i < MAX_COOKIES ; i ++ )
    {
      if ( strlen ( Me [ PlayerNum ] . cookie_list [ i ] ) <= 0 )
	{
	  if ( !strcmp ( Me [ PlayerNum ] . cookie_list [ i ] , CookieString ) )
	    {
	      DebugPrintf ( 0 , "\n\nTHAT COOKIE WAS ALREADY SET... DOING NOTHING...\n\n" );
	      return;
	    }
	}
    }

  //--------------------
  // Now we find a good new and free position for our new cookie...
  //
  for ( i = 0 ; i < MAX_COOKIES ; i ++ )
    {
      if ( strlen ( Me [ PlayerNum ] . cookie_list [ i ] ) <= 0 )
	{
	  break;
	}
    }

  //--------------------
  // Maybe the position we have found is the last one.  That would mean too
  // many cookies, a case that should never occur in FreedroidRPG and that is
  // a considered a fatal error...
  //
  if ( i >= MAX_COOKIES ) 
    {
      fprintf( stderr, "\n\nCookieString: %s\n" , CookieString );
      GiveStandardErrorMessage ( "PlantCookie(...)" , "\
There were no more free positions available to store this cookie.\n\
This should not be possible without a severe bug in FreedroidRPG.",
				 PLEASE_INFORM, IS_FATAL );
    }

  //--------------------
  // Now that we know that we have found a good position for storing our
  // new cookie, we can do it.
  //
  strcpy ( Me [ PlayerNum ] . cookie_list [ i ] , CookieString );
  DebugPrintf ( 0 , "\n\nNEW COOKIE STORED:  Position=%d Text='%s'.\n\n" , 
		i , CookieString );


}; // void PlantCookie ( char* CookieString , int PlayerNum )

/* ----------------------------------------------------------------------
 * 
 * ---------------------------------------------------------------------- */
void 
CutDownStringToMaximalSize ( char* StringToCut , int LengthInPixels )
{
  int StringIndex=0;
  int LengthByNow=0;
  int i;

  if ( TextWidth ( StringToCut ) <= LengthInPixels ) return;

  while ( StringToCut[ StringIndex ] != 0 )
    {
      LengthByNow += CharWidth ( GetCurrentFont() , StringToCut [ StringIndex ] ) ;
      if ( LengthByNow >= LengthInPixels )
	{
	  for ( i = 0 ; i < 3 ; i ++ )
	    {
	      if ( StringToCut [ StringIndex + i ] != 0 )
		{
		  StringToCut [ StringIndex + i ] = '.';
		}
	      else
		return;
	    }
	  StringToCut [ StringIndex + 3 ] = 0 ;
	  return;
	}
      StringIndex ++;
    }

}; // void CutDownStringToMaximalSize ( char* StringToCut , int LengthInPixels )

/* ----------------------------------------------------------------------
 * This function should init the chat roster with empty values and thereby
 * clean out the remnants of the previous chat dialogue.
 * ---------------------------------------------------------------------- */
void
InitChatRosterForNewDialogue( void )
{
  int i;
  int j;
  static int FirstInitialisation = TRUE;

  for ( i = 0 ; i < MAX_DIALOGUE_OPTIONS_IN_ROSTER ; i ++ )
    {

      //--------------------
      // If this is not the first initialisation, we have to free the allocated
      // strings first, or we'll be leaking memory otherwise...
      //
      if ( !FirstInitialisation )
	{
	  if ( strlen ( ChatRoster[i].option_text ) ) free ( ChatRoster[i].option_text );
	  if ( strlen ( ChatRoster[i].option_sample_file_name ) ) free ( ChatRoster[i].option_sample_file_name );
	}
      ChatRoster[i].option_text="";
      ChatRoster[i].option_sample_file_name="";

      for ( j = 0 ; j < MAX_REPLIES_PER_OPTION ; j++ )
	{
	  //--------------------
	  // If this is not the first initialisation, we have to free the allocated
	  // strings first, or we'll be leaking memory otherwise...
	  //
	  if ( !FirstInitialisation )
	    {
	      if ( strlen ( ChatRoster [ i ] . reply_sample_list [ j ] ) ) 
		free ( ChatRoster [ i ] . reply_sample_list [ j ] );
	      if ( strlen ( ChatRoster [ i ] . reply_subtitle_list [ j ] ) ) 
		free ( ChatRoster [ i ] . reply_subtitle_list [ j ] );
	    }
	  ChatRoster [ i ] . reply_sample_list [ j ] = "";
	  ChatRoster [ i ] . reply_subtitle_list [ j ] = "";
	}

      for ( j = 0 ; j < MAX_EXTRAS_PER_OPTION ; j++ )
	{
	  //--------------------
	  // If this is not the first initialisation, we have to free the allocated
	  // strings first, or we'll be leaking memory otherwise...
	  //
	  if ( !FirstInitialisation )
	    {
	      if ( strlen ( ChatRoster [ i ] . extra_list [ j ] ) ) 
		free ( ChatRoster [ i ] . extra_list [ j ] );
	    }
	  ChatRoster [ i ] . extra_list [ j ] = "";
	}

      //--------------------
      // If this is not the first initialisation, we have to free the allocated
      // strings first, or we'll be leaking memory otherwise...
      //
      if ( !FirstInitialisation )
	{
	  if ( strlen ( ChatRoster [ i ] . on_goto_condition ) ) 
	    free ( ChatRoster [ i ] . on_goto_condition );
	}
      ChatRoster [ i ] . on_goto_condition = "";
      ChatRoster [ i ] . on_goto_first_target = (-1);
      ChatRoster [ i ] . on_goto_second_target = (-1);

      for ( j = 0 ; j < MAX_DIALOGUE_OPTIONS_IN_ROSTER ; j++ )
	{
	  ChatRoster [ i ] . change_option_nr [ j ] = (-1); 
	  ChatRoster [ i ] . change_option_to_value [ j ] = (-1); 
	}
    }

  //--------------------
  // Next time, we WILL have to free every used entry before cleaning it
  // out, or we will be leaking memory...
  //
  FirstInitialisation = FALSE ;

}; // void InitChatRosterForNewDialogue( void )

/* ----------------------------------------------------------------------
 * This function should load new chat dialogue information from the 
 * chat info file 'Freedroid.ruleset' into the chat roster.
 *
 * The chat information will be taken from the section with the given
 * Sequence code in the Freedroid.ruleset file.
 *
 * ---------------------------------------------------------------------- */
void
LoadChatRosterWithChatSequence ( char* SequenceCode )
{
  char *ChatData;
  char *SectionPointer;
  char *EndOfSectionPointer;
  char *NextChatSectionCode;
  int i , j ;
  int OptionSectionsLeft;
  char* fpath;
  int OptionIndex;
  int NumberOfOptionsInSection;
  char TempSavedCharacter;
  char *TempEndPointer;
  int NumberOfReplySubtitles;
  int NumberOfReplySamples;
  int NumberOfOptionChanges;
  int NumberOfNewOptionValues;
  int NumberOfExtraEntries;

  int RestoreTempDamage;
  char* ReplyPointer;
  char* OptionChangePointer;
  char* ExtraPointer;

  fpath = find_file ( "Freedroid.dialogues" , MAP_DIR, FALSE);

#define END_OF_DIALOGUE_FILE_STRING "*** End of Dialogue File Information ***"
#define CHAT_CHARACTER_BEGIN_STRING "Beginning of new chat dialog for character=\""
#define CHAT_CHARACTER_END_STRING "End of chat dialog for character=\""
#define NEW_OPTION_BEGIN_STRING "New Option Nr="

  //--------------------
  // At first we read the whole chat file information into memory
  //
  ChatData = ReadAndMallocAndTerminateFile( fpath , END_OF_DIALOGUE_FILE_STRING ) ;
  SectionPointer = ChatData ;

  //--------------------
  // Now we search for the desired chat section, cause most likely
  // there will be more than one person to chat in this chat file soon
  //
#define UNINITIALIZED_SECTION_CODE "NOPERSONATALL"
  NextChatSectionCode = UNINITIALIZED_SECTION_CODE ;
  SectionPointer = ChatData;

  while ( strcmp ( NextChatSectionCode , SequenceCode ) )
    {
      SectionPointer = LocateStringInData ( SectionPointer, CHAT_CHARACTER_BEGIN_STRING );
      if ( strcmp ( NextChatSectionCode , UNINITIALIZED_SECTION_CODE ) )
	free ( NextChatSectionCode );
      NextChatSectionCode = ReadAndMallocStringFromData ( SectionPointer , CHAT_CHARACTER_BEGIN_STRING , "\"" ) ;
      DebugPrintf( CHAT_DEBUG_LEVEL , "\nChat section beginning found.  Chat code given: %s." , NextChatSectionCode );
      SectionPointer++;
    }
  DebugPrintf( CHAT_DEBUG_LEVEL , "\nThat seems to be the chat section we're looking for.  Great!" ) ;
  free ( NextChatSectionCode );

  //--------------------
  // Now we locate the end of this chat section and put a 
  // termination character there, so we can use string functions
  // conveniently on the given section.
  //
  EndOfSectionPointer = LocateStringInData ( SectionPointer , CHAT_CHARACTER_END_STRING );
  *EndOfSectionPointer = 0;

  //--------------------
  // At first we go take a look on how many options we have
  // to decode from this section.
  //
  NumberOfOptionsInSection = CountStringOccurences ( SectionPointer , NEW_OPTION_BEGIN_STRING ) ;
  DebugPrintf( CHAT_DEBUG_LEVEL , "\nWe have counted %d Option entries in this section." , NumberOfOptionsInSection ) ;

  //--------------------
  // Now we see which option index is assigned to this option.
  // It may happen, that some numbers are OMITTED here!  This
  // should be perfectly ok and allowed as far as the code is
  // concerned in order to give the content writers more freedom.
  //
  for ( i = 0 ; i < NumberOfOptionsInSection; i ++ )
    {
      SectionPointer = LocateStringInData ( SectionPointer, NEW_OPTION_BEGIN_STRING );
      ReadValueFromString( SectionPointer , NEW_OPTION_BEGIN_STRING, "%d" , 
			   &OptionIndex , EndOfSectionPointer );
      DebugPrintf( CHAT_DEBUG_LEVEL , "\nFound New Option entry.  Index found is: %d. " , OptionIndex ) ;
      SectionPointer++;

      //--------------------
      // Now that we have the actual option index, we can start to
      // fill the roster with real information.  At first, this will be only
      // the Option string and sample
      //
      // Anything that is loaded into the chat roster doesn't need to be freed,
      // cause this will be done by the next 'InitChatRoster' function anyway.
      //
      ChatRoster[ OptionIndex ] . option_text = 
	ReadAndMallocStringFromData ( SectionPointer , "OptionText=\"" , "\"" ) ;
      DebugPrintf( CHAT_DEBUG_LEVEL , "\nOptionText found : \"%s\"." , ChatRoster[ OptionIndex ] . option_text );
      ChatRoster[ OptionIndex ] . option_sample_file_name = 
	ReadAndMallocStringFromData ( SectionPointer , "OptionSample=\"" , "\"" ) ;
      DebugPrintf( CHAT_DEBUG_LEVEL , "\nOptionSample found : \"%s\"." , ChatRoster[ OptionIndex ] . option_sample_file_name );


      //--------------------
      // Now we can start to add all given Sample and Subtitle combinations
      // But first we must add a termination character in order to not use
      // the combinations of the next option section.
      // 
      if ( ( OptionSectionsLeft = CountStringOccurences ( SectionPointer , NEW_OPTION_BEGIN_STRING ) ) )
	{
	  DebugPrintf ( CHAT_DEBUG_LEVEL , "\nThere are still %d option sections in the file.  \n\
Therefore we must add a new temporary termination character in between." , OptionSectionsLeft );
	  TempEndPointer = LocateStringInData ( SectionPointer, NEW_OPTION_BEGIN_STRING );
	  TempSavedCharacter = *TempEndPointer;
	  *TempEndPointer = 0 ;
	  RestoreTempDamage = TRUE;
	}
      else
	{
	  DebugPrintf ( CHAT_DEBUG_LEVEL , "\nThere is no more option section left in the file.  \n\
Therefore we need not add an additional termination character now." );
	  RestoreTempDamage = FALSE;
	}

#define NEW_REPLY_SAMPLE_STRING "ReplySample=\""
#define NEW_REPLY_SUBTITLE_STRING "Subtitle=\""

      //--------------------
      // We count the number of Subtitle and Sample combinations and then
      // we will read them out
      //
      NumberOfReplySamples = CountStringOccurences ( SectionPointer , NEW_REPLY_SAMPLE_STRING ) ;
      NumberOfReplySubtitles = CountStringOccurences ( SectionPointer , NEW_REPLY_SUBTITLE_STRING ) ;
      if ( NumberOfReplySamples != NumberOfReplySubtitles )
	{
	  fprintf( stderr, "\n\nNumberOfReplySamples: %d NumberOfReplySubtitles: %d \n" , NumberOfReplySamples , NumberOfReplySubtitles );
	  GiveStandardErrorMessage ( "LoadChatRosterWithChatSequence(...)" , "\
There were an unequal number of reply samples and subtitles specified\n\
within a section of the Freedroid.dialogues file.\n\
This is currently not allowed in Freedroid and therefore indicates a\n\
severe error.",
				     PLEASE_INFORM, IS_FATAL );
	}
      else
	{
	  DebugPrintf ( CHAT_DEBUG_LEVEL , "\nThere were %d reply samples and an equal number of subtitles\n\
found in this option of the dialogue, which is fine.", NumberOfReplySamples );
	}

      //--------------------
      // Now that we know exactly how many Sample and Subtitle sections 
      // to read out, we can well start reading exactly that many of them.
      // 
      ReplyPointer = SectionPointer;
      for ( j = 0 ; j < NumberOfReplySamples ; j ++ )
	{
	  ChatRoster[ OptionIndex ] . reply_subtitle_list [ j ] =
	    ReadAndMallocStringFromData ( ReplyPointer , "Subtitle=\"" , "\"" ) ;
	  DebugPrintf( CHAT_DEBUG_LEVEL , "\nReplySubtitle found : \"%s\"." , ChatRoster[ OptionIndex ] . reply_subtitle_list [ j ] );
	  ChatRoster[ OptionIndex ] . reply_sample_list [ j ] =
	    ReadAndMallocStringFromData ( ReplyPointer , "ReplySample=\"" , "\"" ) ;
	  DebugPrintf( CHAT_DEBUG_LEVEL , "\nReplySample found : \"%s\"." , ChatRoster[ OptionIndex ] . reply_sample_list [ j ] );

	  //--------------------
	  // Now we must move the reply pointer to after the previous combination.
	  //
	  ReplyPointer = LocateStringInData ( ReplyPointer, "ReplySample" );
	  ReplyPointer ++;

	}

      //--------------------
      // We count the number of Option changes and new values and then
      // we will read them out
      //
      NumberOfOptionChanges = CountStringOccurences ( SectionPointer , "ChangeOption" ) ;
      NumberOfNewOptionValues = CountStringOccurences ( SectionPointer , "ChangeToValue" ) ;
      if ( NumberOfOptionChanges != NumberOfNewOptionValues )
	{
	  fprintf( stderr, "\n\nNumberOfOptionChanges: %d NumberOfNewOptionValues: %d \n" , NumberOfOptionChanges , NumberOfNewOptionValues );
	  GiveStandardErrorMessage ( "LoadChatRosterWithChatSequence(...)" , "\
There was number of option changes but an unequal number of new option\n\
values specified in a section within the Freedroid.dialogues file.\n\
This is currently not allowed in Freedroid and therefore indicates a\n\
severe error.",
				     PLEASE_INFORM, IS_FATAL );
	}
      else
	{
	  DebugPrintf ( CHAT_DEBUG_LEVEL , "\nThere were %d option changes and an equal number of new option values\n\
found in this option of the dialogue, which is fine.", NumberOfOptionChanges );
	}

      //--------------------
      // Now that we know exactly how many option changes and new option values 
      // to read out, we can well start reading exactly that many of them.
      // 
      OptionChangePointer = SectionPointer;
      for ( j = 0 ; j < NumberOfOptionChanges ; j ++ )
	{
	  ReadValueFromString( OptionChangePointer , "ChangeOption=" , "%d" , 
			       & ( ChatRoster[ OptionIndex ] . change_option_nr [ j ] ) , TempEndPointer );
	  ReadValueFromString( OptionChangePointer , "ChangeToValue=" , "%d" , 
			       & ( ChatRoster[ OptionIndex ] . change_option_to_value [ j ] ) , TempEndPointer );
	  DebugPrintf( CHAT_DEBUG_LEVEL , "\nOption Nr. %d will change to value %d. " , 
		       ChatRoster[ OptionIndex ] . change_option_nr [ j ] ,
		       ChatRoster[ OptionIndex ] . change_option_to_value [ j ] );

	  //--------------------
	  // Now we must move the option change pointer to after the previous combination.
	  //
	  OptionChangePointer = LocateStringInData ( OptionChangePointer, "ChangeToValue" );
	  OptionChangePointer ++;
	}

      //--------------------
      // We count the number of Extras to be done then
      // we will read them out
      //
      NumberOfExtraEntries = CountStringOccurences ( SectionPointer , "DoSomethingExtra" ) ;
      DebugPrintf( CHAT_DEBUG_LEVEL , "\nThere were %d 'Extras' specified in this option." , 
		   NumberOfExtraEntries );
      
      //--------------------
      // Now that we know exactly how many extra entries 
      // to read out, we can well start reading exactly that many of them.
      // 
      ExtraPointer = SectionPointer;
      for ( j = 0 ; j < NumberOfExtraEntries ; j ++ )
	{
	  // ExtraPointer = LocateStringInData ( ExtraPointer, "DoSomethingExtra" );

	  ChatRoster[ OptionIndex ] . extra_list [ j ] =
	    ReadAndMallocStringFromData ( ExtraPointer , "DoSomethingExtra=\"" , "\"" ) ;

	  DebugPrintf( CHAT_DEBUG_LEVEL , "\nOption will execute this extra: %s. " , 
		       ChatRoster[ OptionIndex ] . extra_list [ j ] );

	  //--------------------
	  // Now we must move the option change pointer to after the previous combination.
	  //
	  ExtraPointer = LocateStringInData ( ExtraPointer, "DoSomethingExtra" );
	  ExtraPointer ++;
	}


      //--------------------
      // Next thing we do will be to look whether there is maybe a on-goto-command
      // included in this option section.  If so, we'll read it out.
      //
      if ( CountStringOccurences ( SectionPointer , "OnCondition" ) ) 
	{
	  DebugPrintf( CHAT_DEBUG_LEVEL , "\nWe've found an ON-GOTO-CONDITION IN THIS OPTION!" );
	  ChatRoster[ OptionIndex ] . on_goto_condition = 
	    ReadAndMallocStringFromData ( SectionPointer , "OnCondition=\"" , "\"" ) ;
	  DebugPrintf( CHAT_DEBUG_LEVEL , "\nOnCondition text found : \"%s\"." , ChatRoster[ OptionIndex ] . on_goto_condition );
	  ReadValueFromString( SectionPointer , "JumpToOption=" , "%d" , 
			       & ( ChatRoster[ OptionIndex ] . on_goto_first_target ) , TempEndPointer );
	  ReadValueFromString( SectionPointer , "ElseGoto=" , "%d" , 
			       & ( ChatRoster[ OptionIndex ] . on_goto_second_target ) , TempEndPointer );
	  DebugPrintf( CHAT_DEBUG_LEVEL , "\nOnCondition jump targets: TRUE--> %d FALSE-->%d." , 
		       ChatRoster[ OptionIndex ] . on_goto_first_target ,
		       ChatRoster[ OptionIndex ] . on_goto_second_target  );
	}
      else
	{
	  DebugPrintf( CHAT_DEBUG_LEVEL , "\nThere seems to be NO ON-GOTO-CONDITION AT ALL IN THIS OPTION." );
	}

      //--------------------
      // Now that the whole section has been read out into the ChatRoster, we can
      // restore the original form of the Text again and the next option section
      // can be read out in the next run of this loop 
      //
      if ( RestoreTempDamage )
	{
	  DebugPrintf ( CHAT_DEBUG_LEVEL , "\nWe have now restored the damage from the temporary termination character." );
	  *TempEndPointer = TempSavedCharacter ;
	}
      else
	{
	  DebugPrintf ( CHAT_DEBUG_LEVEL , "\nSince we didn't add any temp termination character, there's nothing to restore now." );
	}
    }

  //--------------------
  // Now we've got all the information we wanted from the dialogues file.
  // We can now free the loaded file again.  Upon a new character dialogue
  // being initiated, we'll just reload the file.  This is very conveninet,
  // for it allows making and testing changes to the dialogues without even
  // having to restart Freedroid!  Very cool!
  //
  free( ChatData );

}; // void LoadChatRosterWithChatSequence ( char* SequenceCode )

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
  // First we define our subtitle window.  We formerly had a small
  // narrow subtitle window of a format like this:
  //
  // Subtitle_Window.x= 15; Subtitle_Window.y=435; Subtitle_Window.w=600; Subtitle_Window.h=44;
  // 
  // But now we have a new subtitle window for Bastians reshaped chat interface:
  //
  Subtitle_Window . x = 260; Subtitle_Window . y = 42; Subtitle_Window . w = 337 ; Subtitle_Window . h = 216;

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

  if ( strcmp ( SubtitleText , "NO_SUBTITLE_AND_NO_WAITING_EITHER" ) )
    {
      DisplaySubtitle ( SubtitleText , Background );
      PlayOnceNeededSoundSample( SampleFilename , TRUE );
    }
  else
    {
      PlayOnceNeededSoundSample( SampleFilename , FALSE );
    }
}; // void GiveSubtitleNSample( char* SubtitleText , char* SampleFilename )

/* ----------------------------------------------------------------------
 * Chat options may contain some extra commands, that specify things that
 * the engine is supposed to do, like open a shop interface, drop some
 * extra item to the inventory, remove an item from inventory, assign a
 * mission, mark a mission as solved and such things.
 *
 * This function is supposed to decode such extra commands and then to
 * execute the desired effect as well.
 *
 * ---------------------------------------------------------------------- */
void
ExecuteChatExtra ( char* ExtraCommandString , Enemy ChatDroid )
{
  int TempValue;
  char WorkString[5000];
  char *TempMessage;
  item NewItem;

  if ( ! strcmp ( ExtraCommandString , "Buy_Basic_Items" ) )
    {
      InitTradeWithCharacter( PERSON_STONE );
    }
  else if ( ! strcmp ( ExtraCommandString , "BreakOffAndBecomeHostile" ) )
    {
      ChatDroid -> is_friendly = FALSE ;
      ChatDroid -> combat_state = MAKE_ATTACK_RUN ;
    }
  else if ( ! strcmp ( ExtraCommandString , "IncreaseMeleeWeaponSkill" ) )
    {
      Me [ 0 ] . melee_weapon_skill ++; 
      SetNewBigScreenMessage( "Melee fighting ability improved!" );
    }
  else if ( ! strcmp ( ExtraCommandString , "IncreaseRangedWeaponSkill" ) )
    {
      Me [ 0 ] . ranged_weapon_skill ++; 
      SetNewBigScreenMessage( "Ranged combat ability improved!" );
    }
  else if ( ! strcmp ( ExtraCommandString , "IncreaseSpellcastingSkill" ) )
    {
      Me [ 0 ] . spellcasting_skill ++; 
      SetNewBigScreenMessage( "Spellcasting ability improved!" );
    }

  else if ( ! strcmp ( ExtraCommandString , "IncreaseExtractPlasmaTransistorSkill" ) )
    {
      Me [ 0 ] . base_skill_level [ SPELL_EXTRACT_PLASMA_TRANSISTORS ] ++; 
      SetNewBigScreenMessage( "Extraction of plasma" );
      SetNewBigScreenMessage( "transistors learned!" );
    }

  else if ( ! strcmp ( ExtraCommandString , "IncreaseExtractSuperconductorSkill" ) )
    {
      Me [ 0 ] . base_skill_level [ SPELL_EXTRACT_SUPERCONDUCTORS ] ++; 
      SetNewBigScreenMessage( "Extraction of superconductors learned!" );
    }

  else if ( ! strcmp ( ExtraCommandString , "IncreaseExtractConverterSkill" ) )
    {
      Me [ 0 ] . base_skill_level [ SPELL_EXTRACT_ANTIMATTER_CONVERTERS ] ++; 
      SetNewBigScreenMessage( "Extraction of antimatter-matter" );
      SetNewBigScreenMessage( "converters learned!" );
    }

  else if ( ! strcmp ( ExtraCommandString , "IncreaseExtractInverterSkill" ) )
    {
      Me [ 0 ] . base_skill_level [ SPELL_EXTRACT_ENTROPY_INVERTERS ] ++; 
      SetNewBigScreenMessage( "Extraction of entropy inverters learned!" );
    }

  else if ( ! strcmp ( ExtraCommandString , "IncreaseExtractCondensatorSkill" ) )
    {
      Me [ 0 ] . base_skill_level [ SPELL_EXTRACT_TACHYON_CONDENSATORS ] ++; 
      SetNewBigScreenMessage( "Extraction of tachyon " );
      SetNewBigScreenMessage( "condensators learned!" );
    }

  else if ( CountStringOccurences ( ExtraCommandString , "GiveItem:" ) )
    {
      DebugPrintf( CHAT_DEBUG_LEVEL , "\nExtra invoked giving an item to the Tux. --> have to decode... " );
      ReadValueFromString( ExtraCommandString , "GiveItem:" , "%d" , 
			   &TempValue , ExtraCommandString + strlen ( ExtraCommandString ) + 0 );
      DebugPrintf( CHAT_DEBUG_LEVEL , "\n...decoding...item to give is: %d." , TempValue );
      NewItem.type = TempValue  ;
      NewItem.prefix_code = (-1);
      NewItem.suffix_code = (-1);
      FillInItemProperties ( &NewItem , TRUE , 0 );
      //--------------------
      // Either we put the new item directly into inventory or we issue a warning
      // that there is no room and then drop the item to the floor directly under 
      // the current Tux position.  That can't fail, right?
      //
      if ( !TryToIntegrateItemIntoInventory ( & NewItem , 1 ) )
	{
	  DropItemToTheFloor ( &NewItem , Me [ 0 ] . pos . x , Me [ 0 ] . pos . y , Me [ 0 ] . pos. z ) ;
	  SetNewBigScreenMessage( "1 Item received (on floor)" );
	}
      else
	{
	  SetNewBigScreenMessage( "1 Item received!" );
	}
    }
  else if ( CountStringOccurences ( ExtraCommandString , "ExecuteActionWithLabel:" ) )
    {
      DebugPrintf( CHAT_DEBUG_LEVEL , "\nExtra invoked execution of action with label: %s. Doing it... " ,
		   ExtraCommandString + strlen ( "ExecuteActionWithLabel:" ) );
      ExecuteActionWithLabel ( ExtraCommandString + strlen ( "ExecuteActionWithLabel:" ) , 0 ) ;
    }
  else if ( CountStringOccurences ( ExtraCommandString , "PlantCookie:" ) )
    {
      DebugPrintf( CHAT_DEBUG_LEVEL , "\nExtra invoked planting of a cookie: %s. Doing it... " ,
		   ExtraCommandString + strlen ( "PlantCookie:" ) );
      PlantCookie ( ExtraCommandString + strlen ( "PlantCookie:" ) , 0 ) ;
    }
  else if ( CountStringOccurences ( ExtraCommandString , "InitTradeWithCharacter:" ) )
    {
      TempValue = ResolveDialogSectionToChatFlagsIndex ( ExtraCommandString + strlen ( "InitTradeWithCharacter:" ) ) ;
      InitTradeWithCharacter( TempValue );
    }
  else if ( CountStringOccurences ( ExtraCommandString , "AssignMission:" ) )
    {
      DebugPrintf( CHAT_DEBUG_LEVEL , "\nExtra invoked assigning of mission. --> have to decode... " );
      ReadValueFromString( ExtraCommandString , "AssignMission:" , "%d" , 
			   &TempValue , ExtraCommandString + strlen ( ExtraCommandString ) + 0 );
      DebugPrintf( CHAT_DEBUG_LEVEL , "\n...decoding...Mission to assign is: %d." , TempValue );
      AssignMission ( TempValue );
    }
  else if ( CountStringOccurences ( ExtraCommandString , "MarkMissionComplete:" ) )
    {
      DebugPrintf( CHAT_DEBUG_LEVEL , "\nExtra invoked marking a mission as completed. --> have to decode... " );
      ReadValueFromString( ExtraCommandString , "MarkMissionComplete:" , "%d" , 
			   &TempValue , ExtraCommandString + strlen ( ExtraCommandString ) + 0 );
      DebugPrintf( CHAT_DEBUG_LEVEL , "\n...decoding...Mission to mark as complete is: %d." , TempValue );
      Me [ 0 ] . AllMissions[ TempValue ] . MissionIsComplete = TRUE;
    }
  else if ( CountStringOccurences ( ExtraCommandString , "AddExperienceBonus:" ) )
    {
      DebugPrintf( CHAT_DEBUG_LEVEL , "\nExtra invoked adding an exerpience bonus. --> have to decode... " );
      ReadValueFromString( ExtraCommandString , "AddExperienceBonus:" , "%d" , 
			   &TempValue , ExtraCommandString + strlen ( ExtraCommandString ) + 0 );
      DebugPrintf( CHAT_DEBUG_LEVEL , "\n...decoding...bonus to add is: %d." , TempValue );
      Me [ 0 ] . Experience += TempValue;
      sprintf( WorkString , "+%d Experience Points" , TempValue );
      SetNewBigScreenMessage ( WorkString );
    }
  else if ( CountStringOccurences ( ExtraCommandString , "AddBigScreenMessageBUT_WITH_TERMINATION_CHARACTER_PLEASE:" ) )
    {
      DebugPrintf( CHAT_DEBUG_LEVEL , "\nExtra invoked adding a big screen message. --> have to decode... " );
      TempMessage = ReadAndMallocStringFromData ( ExtraCommandString , "AddBigScreenMessageBUT_WITH_TERMINATION_CHARACTER_PLEASE:" , ":" ) ;
      DebugPrintf( CHAT_DEBUG_LEVEL , "\n...decoding...message is: %s." , TempMessage );
      SetNewBigScreenMessage ( TempMessage );
    }
  else if ( CountStringOccurences ( ExtraCommandString , "AddBaseMagic:" ) )
    {
      DebugPrintf( CHAT_DEBUG_LEVEL , "\nExtra invoked adding some base magic points. --> have to decode... " );
      ReadValueFromString( ExtraCommandString , "AddBaseMagic:" , "%d" , 
			   &TempValue , ExtraCommandString + strlen ( ExtraCommandString ) + 0 );
      DebugPrintf( CHAT_DEBUG_LEVEL , "\n...decoding... amount of magic points mentioned is: %d." , TempValue );
      Me [ 0 ] . base_magic += TempValue;
    }
  else if ( CountStringOccurences ( ExtraCommandString , "AddBaseDexterity:" ) )
    {
      DebugPrintf( CHAT_DEBUG_LEVEL , "\nExtra invoked adding some base dexteritypoints. --> have to decode... " );
      ReadValueFromString( ExtraCommandString , "AddBaseDexterity:" , "%d" , 
			   &TempValue , ExtraCommandString + strlen ( ExtraCommandString ) + 0 );
      DebugPrintf( CHAT_DEBUG_LEVEL , "\n...decoding... amount of dexterity points mentioned is: %d." , TempValue );
      Me [ 0 ] . base_dexterity += TempValue;
    }
  else if ( CountStringOccurences ( ExtraCommandString , "SubtractPointsToDistribute:" ) )
    {
      DebugPrintf( CHAT_DEBUG_LEVEL , "\nExtra invoked subtracting points to distribute. --> have to decode... " );
      ReadValueFromString( ExtraCommandString , "SubtractPointsToDistribute:" , "%d" , 
			   &TempValue , ExtraCommandString + strlen ( ExtraCommandString ) + 0 );
      DebugPrintf( CHAT_DEBUG_LEVEL , "\n...decoding... amount of points mentioned is: %d." , TempValue );
      Me [ 0 ] . points_to_distribute -= TempValue;
    }
  else if ( CountStringOccurences ( ExtraCommandString , "SubtractGold:" ) )
    {
      DebugPrintf( CHAT_DEBUG_LEVEL , "\nExtra invoked subtracting gold. --> have to decode... " );
      ReadValueFromString( ExtraCommandString , "SubtractGold:" , "%d" , 
			   &TempValue , ExtraCommandString + strlen ( ExtraCommandString ) + 0 );
      DebugPrintf( CHAT_DEBUG_LEVEL , "\n...decoding... amount of gold mentioned is: %d." , TempValue );
      Me [ 0 ] . Gold -= TempValue;
      sprintf ( WorkString , "%d bucks given" , TempValue );
      SetNewBigScreenMessage ( WorkString );
    }
  else if ( CountStringOccurences ( ExtraCommandString , "AddGold:" ) )
    {
      DebugPrintf( CHAT_DEBUG_LEVEL , "\nExtra invoked adding gold. --> have to decode... " );
      ReadValueFromString( ExtraCommandString , "AddGold:" , "%d" , 
			   &TempValue , ExtraCommandString + strlen ( ExtraCommandString ) + 0 );
      DebugPrintf( CHAT_DEBUG_LEVEL , "\n...decoding... amount of gold mentioned is: %d." , TempValue );
      Me [ 0 ] . Gold += TempValue;
      sprintf ( WorkString , "%d bucks received" , TempValue );
      SetNewBigScreenMessage ( WorkString );
    }
  else if ( CountStringOccurences ( ExtraCommandString , "DeleteAllInventoryItemsWithCode:" ) )
    {
      DebugPrintf( CHAT_DEBUG_LEVEL , "\nExtra invoked deletion of all inventory items with a certain code. --> have to decode... " );
      ReadValueFromString( ExtraCommandString , "DeleteAllInventoryItemsWithCode:" , "%d" , 
			   &TempValue , ExtraCommandString + strlen ( ExtraCommandString ) + 0 );
      DebugPrintf( CHAT_DEBUG_LEVEL , "\n...decoding... code of item to delete found is: %d." , TempValue );
      DeleteAllInventoryItemsOfType( TempValue , 0 );      
    }
  else if ( CountStringOccurences ( ExtraCommandString , "DeleteAllInventoryItemsOfType:" ) )
    {
      DebugPrintf( CHAT_DEBUG_LEVEL , "\nExtra invoked deletion of all inventory items of type '%s'. --> have to decode... " ,
		   ExtraCommandString + strlen ( "DeleteAllInventoryItemsOfType:" ) );

      if ( !strcmp ( ExtraCommandString + strlen ( "DeleteAllInventoryItemsOfType:" ) , "ITEM_DIXONS_TOOLBOX" ) )
	{
	  TempValue = ITEM_DIXONS_TOOLBOX;
	}
      else if ( !strcmp ( ExtraCommandString + strlen ( "DeleteAllInventoryItemsOfType:" ) , "ITEM_RED_DILITIUM_CRYSTAL" ) )
	{
	  TempValue = ITEM_RED_DILITIUM_CRYSTAL;
	}
      else
	{
	  fprintf( stderr, "\n\nErrorneous string: %s \n" , 
		   ExtraCommandString + strlen ( "DeleteAllInventoryItemsOfType:" ) );
		   GiveStandardErrorMessage ( "ExecuteChatExtra(...)" , "\
ERROR:  UNKNOWN ITEM STRING GIVEN AS ITEM TO DELETE FROM INVENTORY!",
					      PLEASE_INFORM, IS_FATAL );
	}

      DebugPrintf( CHAT_DEBUG_LEVEL , "\n...decoding...item to remove is: %d." , TempValue );
      DeleteAllInventoryItemsOfType( TempValue , 0 );      

    }
  else 
    {
      fprintf( stderr, "\n\nExtraCommandString: %s \n" , ExtraCommandString );
      GiveStandardErrorMessage ( "ExecuteChatExtra(...)" , "\
ERROR:  UNKNOWN COMMAND STRING GIVEN!",
				 PLEASE_INFORM, IS_FATAL );
    }
}; // void ExecuteChatExtra ( char* ExtraCommandString )

/* ----------------------------------------------------------------------
 * This function prepares the chat background window and displays the
 * image of the dialog partner and also sets the right font.
 * ---------------------------------------------------------------------- */
void
PrepareMultipleChoiceDialog ( Enemy ChatDroid )
{
  SDL_Rect Droid_Image_Window;
  SDL_Surface* Small_Droid;
  SDL_Surface* Large_Droid;
  // SDL_Surface* Background;
  char *fpath;
  char fname[500];

  // #define CHAT_BACKGROUND_IMAGE_FILE "backgrounds/chat_test.jpg" 
#define CHAT_BACKGROUND_IMAGE_FILE "backgrounds/chat_test2.jpg" 

  //--------------------
  // This is the droid window we used before Bastian reshaped
  // the chat interface:
  //
  // Droid_Image_Window.x=15; Droid_Image_Window.y=82; Droid_Image_Window.w=215; Droid_Image_Window.h=330;
  // 
  // But now we have a different chat window format:
  //
  Droid_Image_Window . x = 48; Droid_Image_Window . y = 44; Droid_Image_Window . w = 130; Droid_Image_Window . h = 172;

  Activate_Conservative_Frame_Computation( );

  //--------------------
  // Next we prepare the whole background for all later text operations
  //
  if ( Background == NULL )
    Background = IMG_Load( find_file ( CHAT_BACKGROUND_IMAGE_FILE , GRAPHICS_DIR, FALSE ) );
  else
    {
      //--------------------
      // when there is still an old surface present, it might be tainted and we 
      // therefore generate a new fresh one without the image of the previous
      // discussion partner on it.
      //
      SDL_FreeSurface( Background );
      Background = IMG_Load( find_file ( CHAT_BACKGROUND_IMAGE_FILE , GRAPHICS_DIR, FALSE ) );
    }
  if ( Background == NULL )
    {
      fprintf( stderr, "\n\nSDL_GetError: %s \n" , SDL_GetError() );
      GiveStandardErrorMessage ( "PrepareMultipleChoiceDialog(...)" , "\
ERROR LOADING BACKGROUND IMAGE FILE!",
				 PLEASE_INFORM, IS_FATAL );
    }

  //--------------------
  // We select small font for the menu interaction...
  //
  SetCurrentFont( FPS_Display_BFont );

  //--------------------
  // At first we try to load the image, that is named after this
  // chat section.  If that succeeds, perfect.  If not, we'll revert
  // to a default image.
  //
  strcpy( fname, "droids/" );
  // strcat( fname, Druidmap[ ChatDroid -> type ].portrait_filename_without_ext );
  strcat( fname, ChatDroid -> dialog_section_name );
  strcat( fname , ".png" );
  fpath = find_file (fname, GRAPHICS_DIR, FALSE);
  Small_Droid = IMG_Load (fpath) ;
  if ( Small_Droid == NULL )
    {
      strcpy( fname, "droids/" );
      strcat( fname, "DefaultPortrait.png" );
      fpath = find_file (fname, GRAPHICS_DIR, FALSE);
      Small_Droid = IMG_Load (fpath) ;
    }
  if ( Small_Droid == NULL )
    {
      fprintf( stderr, "\n\nfpath: %s \n" , fpath );
      GiveStandardErrorMessage ( "PrepareMultipleChoiceDialog(...)" , "\
It wanted to load a small portrait file in order to display it in the \n\
chat interface of Freedroid.  But:  Loading this file has failed.",
				 PLEASE_INFORM, IS_FATAL );
    }
  // Large_Droid = zoomSurface( Small_Droid , 1.8 , 1.8 , 0 );

  Large_Droid = zoomSurface( Small_Droid , (float)Droid_Image_Window.w / (float)Small_Droid->w , (float)Droid_Image_Window.w / (float)Small_Droid->w , 0 );
  SDL_BlitSurface( Large_Droid , NULL , Background , &Droid_Image_Window );
  SDL_BlitSurface( Background , NULL , Screen , NULL );
  SDL_Flip( Screen );

  SDL_FreeSurface( Small_Droid );
  SDL_FreeSurface( Large_Droid );

}; // void PrepareMultipleChoiceDialog ( int Enum )

/* ----------------------------------------------------------------------
 * It is possible to specify a conditional goto command from the chat
 * information file 'Freedroid.dialogues'.  But in order to execute this
 * conditional jump, we need to know whether a statment given as pure text
 * string is true or not.  This function is intended to find out whether
 * it is true or not.
 * ---------------------------------------------------------------------- */
int
TextConditionIsTrue ( char* ConditionString )
{
  int TempValue;
  char* CookieText;
  int i ;

  if ( CountStringOccurences ( ConditionString , "MissionComplete" ) )
    {
      DebugPrintf ( CHAT_DEBUG_LEVEL , "\nCondition String identified as question for mission complete." );
      ReadValueFromString( ConditionString , ":", "%d" , 
			   &TempValue , ConditionString + strlen ( ConditionString ) );
      DebugPrintf ( CHAT_DEBUG_LEVEL , "\nCondition String referred to mission number: %d." , TempValue );

      if ( Me [ 0 ] . AllMissions [ TempValue ] . MissionIsComplete )
	return ( TRUE );
      else
	return ( FALSE );
    }
  else if ( CountStringOccurences ( ConditionString , "PointsToDistributeAtLeast" ) )
    {
      DebugPrintf ( CHAT_DEBUG_LEVEL , "\nCondition String identified as question for available skill points to distribute." );
      ReadValueFromString( ConditionString , ":", "%d" , 
			   &TempValue , ConditionString + strlen ( ConditionString ) );
      DebugPrintf ( CHAT_DEBUG_LEVEL , "\nCondition String mentioned number of points: %d." , TempValue );

      if ( Me [ 0 ] . points_to_distribute >= TempValue )
	return ( TRUE );
      else
	return ( FALSE );
    }
  else if ( CountStringOccurences ( ConditionString , "GoldIsLessThan" ) )
    {
      DebugPrintf ( CHAT_DEBUG_LEVEL , "\nCondition String identified as question for amount of gold Tux has on him." );
      ReadValueFromString( ConditionString , ":", "%d" , 
			   &TempValue , ConditionString + strlen ( ConditionString ) );
      DebugPrintf ( CHAT_DEBUG_LEVEL , "\nCondition String mentioned concrete amout of gold: %d." , TempValue );

      if ( Me [ 0 ] . Gold < TempValue )
	return ( TRUE );
      else
	return ( FALSE );
    }
  else if ( CountStringOccurences ( ConditionString , "MeleeSkillLesserThan" ) )
    {
      DebugPrintf ( CHAT_DEBUG_LEVEL , "\nCondition String identified as question for melee skill lesser than value." );
      ReadValueFromString( ConditionString , ":", "%d" , 
			   &TempValue , ConditionString + strlen ( ConditionString ) );
      DebugPrintf ( CHAT_DEBUG_LEVEL , "\nCondition String mentioned level: %d." , TempValue );

      if ( Me [ 0 ] . melee_weapon_skill < TempValue )
	return ( TRUE );
      else
	return ( FALSE );
    }
  else if ( CountStringOccurences ( ConditionString , "CookieIsPlanted" ) )
    {
      DebugPrintf ( 0 , "\nCondition String identified as question for cookie planted." );

      CookieText = 
	ReadAndMallocStringFromData ( ConditionString , "CookieIsPlanted:" , ":" ) ;
      DebugPrintf ( 0 , "\nCookieText mentioned: '%s'." , CookieText );

      for ( i = 0 ; i < MAX_COOKIES ; i ++ )
	{
	  if ( ! strlen ( Me [ 0 ] . cookie_list [ i ] ) ) continue;
	  if ( ! strcmp ( Me [ 0 ] . cookie_list [ i ] , CookieText ) ) 
	    return ( TRUE );
	}

      free ( CookieText );

      return ( FALSE );

    }

  fprintf( stderr, "\n\nConditionString: %s. \n" , ConditionString );
  GiveStandardErrorMessage ( "TextConditionIsTrue(...)" , "\
There were was a Condition string (most likely used for an on-goto-command\n\
in the Freedroid.dialogues file, that contained a seemingly bogus condition.\n\
Freedroid was unable to determine the type of said condition.",
			     PLEASE_INFORM, IS_FATAL );

  return ( TRUE );
}; // int TextConditionIsTrue ( char* ConditionString )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
DoChatFromChatRosterData( int PlayerNum , int ChatPartnerCode , Enemy ChatDroid )
{
  int i ;
  SDL_Rect Chat_Window;
  int MenuSelection = (-1) ;
  char* DialogMenuTexts[ MAX_ANSWERS_PER_PERSON ];

  PrepareMultipleChoiceDialog ( ChatDroid );


  Chat_Window.x=242; Chat_Window.y=100; Chat_Window.w=380; Chat_Window.h=314;

  //--------------------
  // We load the option texts into the dialog options variable..
  //
  for ( i = 0 ; i < MAX_ANSWERS_PER_PERSON ; i ++ )
    {
      if ( strlen ( ChatRoster [ i ] . option_text ) )
	{
	  DialogMenuTexts [ i ] = ChatRoster [ i ] . option_text ;
	}
    }
  DialogMenuTexts [ MAX_ANSWERS_PER_PERSON - 1 ] = " END ";

  while (1)
    {
      //--------------------
      // Now maybe this is one of the bots that is rushing the Tux!  Then of course
      // we won't do the first selection, but instead immediately call the very first
      // dialog option and then continue with normal dialog.
      //
      if ( ChatDroid -> will_rush_tux )
	{
	  MenuSelection = 1 ;
	  ChatDroid -> will_rush_tux = FALSE ;
	}
      else
	MenuSelection = ChatDoMenuSelectionFlagged ( "What will you say?" , DialogMenuTexts , Me [ PlayerNum ] . Chat_Flags [ ChatPartnerCode ]  , 1 , NULL , FPS_Display_BFont );

      //--------------------
      // We do some correction of the menu selection variable:
      // The first entry of the menu will give a 1 and so on and therefore
      // we need to correct this to more C style.
      //
      MenuSelection --;
      if ( ( MenuSelection >= MAX_ANSWERS_PER_PERSON - 2 ) || ( MenuSelection < 0 ) )
	{
	  MenuSelection = MAX_REPLIES_PER_OPTION -1 ;
	}

      //--------------------
      // Now a menu section has been made.  We do the reaction:
      // say the samples and the replies, later we'll set the new option values
      //
      // But it might be the case that this option is more technical and not accompanied
      // by any reply.  This case must also be caught.
      //
      if ( strcmp ( ChatRoster [ MenuSelection ] . option_sample_file_name , "NO_SAMPLE_HERE_AND_DONT_WAIT_EITHER" ) )
	{
	  // PlayOnceNeededSoundSample( ChatRoster [ MenuSelection ] . option_sample_file_name , TRUE );
	  GiveSubtitleNSample ( ChatRoster [ MenuSelection ] . option_text ,
			        ChatRoster [ MenuSelection ] . option_sample_file_name ) ;
	}
      
      //--------------------
      // Maybe there was an ON-GOTO-CONDITION specified for this option.
      // Then of course we have to jump to the new location!!!
      //
      while ( strlen ( ChatRoster [ MenuSelection ] . on_goto_condition ) )
	{
	  DebugPrintf( CHAT_DEBUG_LEVEL , "\nON-GOTO-CONDITION ENCOUNTERED... CHECKING... " );
	  if ( TextConditionIsTrue ( ChatRoster [ MenuSelection ] . on_goto_condition ) )
	    {
	      DebugPrintf( CHAT_DEBUG_LEVEL , "...SEEMS TRUE... CONTINUING AT OPTION: %d. " , 
			   ChatRoster [ MenuSelection ] . on_goto_first_target );
	      MenuSelection = ChatRoster [ MenuSelection ] . on_goto_first_target ;
	    }
	  else
	    {
	      DebugPrintf( CHAT_DEBUG_LEVEL , "...SEEMS FALSE... CONTINUING AT OPTION: %d. " , 
			   ChatRoster [ MenuSelection ] . on_goto_second_target );
	      MenuSelection = ChatRoster [ MenuSelection ] . on_goto_second_target ;
	    }
	}

      //--------------------
      // Now that any eventual jump has been done, we can proceed to execute
      // the rest of the reply that has been set up for this (the now maybe modified)
      // dialog option.
      //
      for ( i = 0 ; i < MAX_REPLIES_PER_OPTION ; i ++ )
	{
	  //--------------------
	  // Once we encounter an empty string here, we're done with the reply...
	  //
	  if ( ! strlen ( ChatRoster [ MenuSelection ] . reply_subtitle_list [ i ] ) ) 
	    break;

	  GiveSubtitleNSample ( ChatRoster [ MenuSelection ] . reply_subtitle_list [ i ] ,
			        ChatRoster [ MenuSelection ] . reply_sample_list [ i ]    ) ;
	}

      //--------------------
      // Now that all the replies have been made, we can start on changing
      // the option flags to their new values
      //
      for ( i = 0 ; i < MAX_ANSWERS_PER_PERSON ; i ++ )
	{
	  //--------------------
	  // Maybe all nescessary changes were made by now.  Then it's time
	  // to quit...
	  //
	  if ( ChatRoster [ MenuSelection ] . change_option_nr [ i ] == (-1) ) 
	    break;

	  Me [ PlayerNum ] . Chat_Flags [ ChatPartnerCode ] [ ChatRoster [ MenuSelection ] . change_option_nr [ i ] ] =
	    ChatRoster [ MenuSelection ] . change_option_to_value [ i ]  ;
	  DebugPrintf ( CHAT_DEBUG_LEVEL , "\nChanged chat flag nr. %d to new value %d." ,
			ChatRoster [ MenuSelection ] . change_option_nr[i] ,
			ChatRoster [ MenuSelection ] . change_option_to_value[i] );
	}

      //--------------------
      // Maybe this option should also invoke some extra function like opening
      // a shop interface or something.  So we do this here.
      //
      for ( i = 0 ; i < MAX_EXTRAS_PER_OPTION ; i ++ )
	{
	  //--------------------
	  // Maybe all nescessary extras were executed by now.  Then it's time
	  // to quit...
	  //
	  if ( !strlen ( ChatRoster [ MenuSelection ] . extra_list [ i ] ) )
	    break;

	  DebugPrintf ( CHAT_DEBUG_LEVEL , "\nWARNING!  Starting to invoke extra.  Text is: %s." ,
			ChatRoster [ MenuSelection ] . extra_list[i] );

	  ExecuteChatExtra ( ChatRoster [ MenuSelection ] . extra_list[i] , ChatDroid );

	  //--------------------
	  // Maybe the chat extra has annoyed the chat partner and he is now
	  // suddenly hostile and breaks off the chat.  This is handled here.
	  //
	  if ( ! ChatDroid -> is_friendly ) return ;

	  //--------------------
	  // It can't hurt to have the overall background redrawn after each extra command
	  // which could have destroyed the background by drawing e.g. a shop interface
	  PrepareMultipleChoiceDialog ( ChatDroid ) ;
	}

      if ( ( MenuSelection >= MAX_ANSWERS_PER_PERSON - 1 ) || ( MenuSelection < 0 ) )
	{
	  return;
	}
    }
  
}; // void DoChatFromChatRosterData( void )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
DialogPartnersTurnToEachOther ( Enemy ChatDroid )
{
  int TurningDone = FALSE;
  float AngleInBetween;
  float WaitBeforeTurningTime = 0.5 ;
  float WaitAfterTurningTime = 0.9 ;
  int TurningStartTime;
  float OldAngle;
  float RightAngle;
  float TurningDirection;

#define TURN_SPEED 90.0

  Activate_Conservative_Frame_Computation();

  //--------------------
  // At first do some waiting before the turning around starts...
  //
  TurningStartTime = SDL_GetTicks();  TurningDone = FALSE ;
  while ( !TurningDone )
    {
      StartTakingTimeForFPSCalculation();       

      AssembleCombatPicture ( 0 ); 
      SDL_Flip ( Screen );
      
      if ( ( SDL_GetTicks() - TurningStartTime ) >= 1000.0 * WaitBeforeTurningTime )
	TurningDone = TRUE;
      
      ComputeFPSForThisFrame();
    }

  //--------------------
  // Now we find out what the final target direction of facing should
  // be.
  //
  // For this we use the atan2, which gives angles from -pi to +pi.
  // 
  // Attention must be paid, since 'y' in our coordinates ascends when
  // moving down and descends when moving 'up' on the scren.  So that
  // one sign must be corrected, so that everything is right again.
  //
  RightAngle = ( atan2 ( - ( Me [ 0 ] . pos . y - ChatDroid -> pos . y ) ,  
			 + ( Me [ 0 ] . pos . x - ChatDroid -> pos . x ) ) * 180.0 / M_PI ) ;
  //
  // Another thing there is, that must also be corrected:  '0' begins
  // with facing 'down' in the current rotation models.  Therefore angle
  // 0 corresponds to that.  We need to shift again...
  //
  RightAngle += 90 ;

  //--------------------
  // Now it's time do determine which direction to move, i.e. if to 
  // turn to the left or to turn to the right...  For this purpose
  // we convert the current angle, which is between 270 and -90 degrees
  // to one between -180 and +180 degrees...
  //
  if ( RightAngle > 180.0 ) RightAngle -= 360.0 ; 

  // DebugPrintf ( 0 , "\nRightAngle: %f." , RightAngle );
  // DebugPrintf ( 0 , "\nCurrent angle: %f." , ChatDroid -> current_angle );

  //--------------------
  // Having done these preparations, it's now easy to determine the right
  // direction of rotation...
  //
  AngleInBetween = RightAngle - ChatDroid -> current_angle ;
  if ( AngleInBetween > 180 ) AngleInBetween -= 360;
  if ( AngleInBetween <= -180 ) AngleInBetween += 360;

  if ( AngleInBetween > 0 )
    TurningDirection = +1 ; 
  else 
    TurningDirection = -1 ; 

  //--------------------
  // Now we turn and show the image until both chat partners are
  // facing each other, mostly the chat partner is facing the Tux,
  // since the Tux may still turn around to somewhere else all the 
  // while, if the chose so
  //
  TurningStartTime = SDL_GetTicks();  TurningDone = FALSE ;
  while ( !TurningDone )
    {
      StartTakingTimeForFPSCalculation();       

      AssembleCombatPicture ( 0 ); 
      SDL_Flip ( Screen );

      OldAngle = ChatDroid -> current_angle;

      ChatDroid -> current_angle = OldAngle + TurningDirection * Frame_Time() * TURN_SPEED ;

      //--------------------
      // In case of positive turning direction, we wait, till our angle is greater
      // than the right angle.
      // Otherwise we wait till our angle is lower than the right angle.
      //
      AngleInBetween = RightAngle - ChatDroid -> current_angle ;
      if ( AngleInBetween > 180 ) AngleInBetween -= 360;
      if ( AngleInBetween <= -180 ) AngleInBetween += 360;
      
      if ( ( TurningDirection > 0 ) && ( AngleInBetween < 0 ) ) TurningDone = TRUE;
      if ( ( TurningDirection < 0 ) && ( AngleInBetween > 0 ) ) TurningDone = TRUE;

      ComputeFPSForThisFrame();
    }

  //--------------------
  // Now that turning around is basically done, we still wait a few frames
  // until we start the dialog...
  //
  TurningStartTime = SDL_GetTicks();  TurningDone = FALSE ;
  while ( !TurningDone )
    {
      StartTakingTimeForFPSCalculation();       

      AssembleCombatPicture ( 0 ); 
      SDL_Flip ( Screen );
      
      if ( ( SDL_GetTicks() - TurningStartTime ) >= 1000.0 * WaitAfterTurningTime )
	TurningDone = TRUE;
      
      ComputeFPSForThisFrame();
    }

}; // void DialogPartnersTurnToEachOther ( Enemy ChatDroid )

/* ----------------------------------------------------------------------
 * This function does the communication routine when the influencer in
 * transfer mode touched a friendly droid.
 * ---------------------------------------------------------------------- */
void 
ChatWithFriendlyDroid( Enemy ChatDroid )
{
  int i ;
  SDL_Rect Chat_Window;
  char* DialogMenuTexts[ MAX_ANSWERS_PER_PERSON ];
  int ChatFlagsIndex = (-1);

  //--------------------
  // Now that we know, that a chat with a friendly droid is planned, the 
  // friendly droid and the Tux should first turn to each other before the
  // real dialog is started...
  //
  DialogPartnersTurnToEachOther ( ChatDroid );

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
  
  //--------------------
  // We clean out the chat roster from any previous use
  //
  InitChatRosterForNewDialogue(  );

  ChatFlagsIndex = ResolveDialogSectionToChatFlagsIndex ( ChatDroid -> dialog_section_name ) ;

  //--------------------
  // Now we do the dialog with Dixon, the teleporter service man...
  //
  if ( Me [ 0 ] . AllMissions [ 1 ] . MissionWasAssigned ) 
    {
      Me [ 0 ] . Chat_Flags [ PERSON_DIXON ]  [ 1 ] = FALSE ; // we allow to ask naively...
      Me [ 0 ] . Chat_Flags [ PERSON_DIXON ]  [ 2 ] = FALSE ; // we allow to ask naively...
      Me [ 0 ] . Chat_Flags [ PERSON_DIXON ]  [ 3 ] = FALSE ; // we allow to ask naively...
      Me [ 0 ] . Chat_Flags [ PERSON_DIXON ]  [ 4 ] = FALSE ; // we allow to ask naively...
      Me [ 0 ] . Chat_Flags [ PERSON_DIXON ]  [ 6 ] = FALSE ; // we allow to ask naively...
      Me [ 0 ] . Chat_Flags [ PERSON_DIXON ]  [ 7 ] = FALSE ; // we allow to ask naively...
    }
  if ( ( Me [ 0 ] . AllMissions [ 1 ] . MissionWasAssigned ) &&
       ( !Me [ 0 ] . AllMissions [ 1 ] . MissionIsComplete ) )
    {
      if ( CountItemtypeInInventory( ITEM_DIXONS_TOOLBOX , 0 ) )
	{
	  Me [ 0 ] . Chat_Flags [ PERSON_DIXON ] [ 5 ] = 1 ; // allow to give back the toolset
	  Me [ 0 ] . Chat_Flags [ PERSON_DIXON ]  [ 7 ] = FALSE ; // we disallow to ask directly for the toolset...
	}
      else
	{
	  Me [ 0 ] . Chat_Flags [ PERSON_DIXON ] [ 5 ] = 0 ; // disallow to give back the toolset
	  Me [ 0 ] . Chat_Flags [ PERSON_DIXON ]  [ 7 ] = TRUE ; // we allow to ask directly for the toolset...
	}
    }
  if ( Me [ 0 ] . AllMissions [ 1 ] . MissionIsComplete ) 
    {
      Me [ 0 ] . Chat_Flags [ PERSON_DIXON ]  [ 1 ] = TRUE ; // we reallow to ask for the status of teleport...
    }

  //--------------------
  // Now we prepare the dialog with Michelangelo, the colony cook...
  //
  if ( ( Me [ 0 ] . AllMissions [ 2 ] . MissionWasAssigned ) &&
       ( !Me [ 0 ] . AllMissions [ 2 ] . MissionIsComplete ) )
    {
      if ( CountItemtypeInInventory( ITEM_RED_DILITIUM_CRYSTAL , 0 ) )
	{
	  Me [ 0 ] . Chat_Flags [ PERSON_MICHELANGELO ]  [ 7 ] = TRUE ; // we allow to give the crystals...
	}
      else
	{
	  Me [ 0 ] . Chat_Flags [ PERSON_MICHELANGELO ]  [ 6 ] = TRUE ; // we allow to report no success yet...
	}
    }
  if ( Me [ 0 ] . AllMissions [ 2 ] . MissionIsComplete )
    {
      Me [ 0 ] . Chat_Flags [ PERSON_MICHELANGELO ]  [ 8 ] = TRUE ; // we allow to ask for reward again...
    }

  //--------------------
  // Now we prepare the dialog with Doc Moore, the old town medic...
  //
  if ( ( Me [ 0 ] . AllMissions [ 0 ] . MissionWasAssigned ) &&
       ( !Me [ 0 ] . AllMissions [ 0 ] . MissionIsComplete ) )
    {
      Me [ 0 ] . Chat_Flags [ PERSON_DOC_MOORE ]  [ 2 ] = TRUE ; // we allow to give the crystals...
      Me [ 0 ] . Chat_Flags [ PERSON_DOC_MOORE ]  [ 3 ] = TRUE ; // we allow to report no success yet...
    }

  //--------------------
  // Now we prepare the dialog with Bender, the strong man who ate brain enlargement pills...
  //
  if ( ( Me [ 0 ] . AllMissions [ 0 ] . MissionWasAssigned ) &&
       ( !Me [ 0 ] . AllMissions [ 0 ] . MissionIsComplete ) )
    {
      if ( CountItemtypeInInventory( 79 , 0 ) )
	{
	  Me [ 0 ] . Chat_Flags [ PERSON_BENDER ]  [ 9  ] = TRUE ; // we allow to give the crystals...
	  Me [ 0 ] . Chat_Flags [ PERSON_BENDER ]  [ 12 ] = FALSE ; // we allow to give the crystals...
	}
      else
	{
	  Me [ 0 ] . Chat_Flags [ PERSON_BENDER ]  [ 12 ] = TRUE ; // we allow to report no success yet...
	  Me [ 0 ] . Chat_Flags [ PERSON_BENDER ]  [ 9  ] = FALSE ; // we allow to report no success yet...
	}
    }
  if ( Me [ 0 ] . AllMissions [ 0 ] . MissionIsComplete ) 
    {
      Me [ 0 ] . Chat_Flags [ PERSON_BENDER ]  [ 12 ] = FALSE ; // we disallow to talk more about the mission...
      Me [ 0 ] . Chat_Flags [ PERSON_BENDER ]  [ 9  ] = FALSE ; // we disallow to talk more about the mission...
      Me [ 0 ] . Chat_Flags [ PERSON_BENDER ]  [ 13 ] = TRUE ; // we disallow to talk more about the mission...
    }

  //--------------------
  // Now we prepare dialog with RMS, the programmer...
  //
  if ( ( Me [ 0 ] . AllMissions [ 1 ] . MissionWasAssigned == TRUE ) &&
       ( Me [ 0 ] . AllMissions [ 1 ] . MissionIsComplete == FALSE ) )
    {
      Me [ 0 ] . Chat_Flags [ PERSON_RMS ]  [ 3 ] = TRUE ; // we allow to ask directly for the coffee machine...
      Me [ 0 ] . Chat_Flags [ PERSON_RMS ]  [ 0 ] = FALSE ; // we disallow to ask about the job naively...
    }
  
  
  LoadChatRosterWithChatSequence ( ChatDroid -> dialog_section_name );
  DoChatFromChatRosterData( 0 , ChatFlagsIndex , ChatDroid );

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
  strcpy ( BigScreenMessage[ BigScreenMessageIndex] , ScreenMessageText );
  BigScreenMessageDuration[ BigScreenMessageIndex] = 0 ;
  BigScreenMessageIndex ++ ;
  if ( BigScreenMessageIndex >= MAX_BIG_SCREEN_MESSAGES )
    BigScreenMessageIndex = 0 ;
}; // void SetNewBigScreenMessage( char* ScreenMessageText )

/* ----------------------------------------------------------------------
 * This function displays the currently defined Bigscreenmessage on the
 * screen.  It will be called by AssembleCombatWindow.
 * ---------------------------------------------------------------------- */
void
DisplayBigScreenMessage( void )
{
  int i;

  for ( i = 0 ; i < MAX_BIG_SCREEN_MESSAGES ; i ++ )
    {
      if ( BigScreenMessageDuration [ i ] < 6.5 )
	{
	  SDL_SetClipRect ( Screen , NULL );
	  CenteredPutStringFont ( Screen , Menu_Filled_BFont , 100 + i * FontHeight ( Menu_Filled_BFont ) , BigScreenMessage [ i ]  );
	  if ( !GameConfig.Inventory_Visible &&
	       !GameConfig.SkillScreen_Visible &&
	       !GameConfig.CharacterScreen_Visible )
	    BigScreenMessageDuration [ i ]  += Frame_Time();
	  
	}
    }

}; // void DisplayBigScreenMessage( void )

/*-----------------------------------------------------------------
 * This function prints *Text beginning at positions startx/starty,
 * respecting the text-borders set by clip_rect.  This includes 
 * clipping but also automatic line-breaks when end-of-line is 
 * reached.  If clip_rect==NULL, no clipping is performed.
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
      fprintf ( stderr , "\nIllegal char passed to DisplayChar(): %d \n", c);
      GiveStandardErrorMessage ( "DisplayChar(...)" , "\
There was an illegal character passed to DisplayChar for printing.\n\
This indicates some error within Freedroid or within one of the dialog\n\
files of Freedroid.",
				 PLEASE_INFORM, IS_WARNING_ONLY );
      return;
    }

  PutChar ( Screen, MyCursorX, MyCursorY, c );

  // DebugPrintf( 0 , "%c" , c );

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
    for ( i = 1 ; i < MAX_WORD_LENGTH ; i ++ ) 
      {
	if ( (Resttext[i] != ' ') && (Resttext[i] != '\n') && (Resttext[i] != 0) )
	  { 
	    NeededSpace += CharWidth ( GetCurrentFont() , Resttext [ i ] );
	    if ( MyCursorX + NeededSpace > clip->x + clip->w - 10 )
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
