
/* ----------------------------------------------------------------------
 * This function does the rotation of a given vector by a given angle.
 * The vector is a vector.
 * The angle given is in DEGREE MEASURE, i.e. 0-360 or so, maybe more or
 * less than that, but you get what I mean...
 * ---------------------------------------------------------------------- */
void
RotateVectorByAngle ( moderately_finepoint* vector , float rot_angle )
{
  moderately_finepoint new_vect;
  float rad_angle;

  rad_angle = rot_angle * ( M_PI / 180.0 ) ; 
  
  DebugPrintf( 2 , "\n RAD_ANGLE : %f " , rad_angle );
  new_vect.x =  sin( rad_angle ) * vector->y + cos( rad_angle ) * vector->x;
  new_vect.y =  cos( rad_angle ) * vector->y - sin( rad_angle ) * vector->x;
  vector->x = new_vect.x;
  vector->y = new_vect.y;

}; // void RotateVectorByAngle ( ... )

/* ----------------------------------------------------------------------
 *
 * ---------------------------------------------------------------------- */
void
delete_one_dialog_option ( int i , int FirstInitialisation )
{
  int j;

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
  
  //--------------------
  // Now we can set the positions of the dialog boxes within the dialog editor
  // to 'empty' values.  This will remain completely without effect in FreedroidRPG.
  // The only one caring about these positions is the Dialog Editor.
  //
  ChatRoster[i].position_x = -1;
  ChatRoster[i].position_y = -1;
  
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
}; // void delete_one_dialog_option ( int i , int FirstInitialisation )

/* ----------------------------------------------------------------------
 * This function should init the chat roster with empty values and thereby
 * clean out the remnants of the previous chat dialogue.
 * ---------------------------------------------------------------------- */
void
InitChatRosterForNewDialogue( void )
{
  int i;
  static int FirstInitialisation = TRUE;

  for ( i = 0 ; i < MAX_DIALOGUE_OPTIONS_IN_ROSTER ; i ++ )
    {
      delete_one_dialog_option ( i , FirstInitialisation );
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
LoadChatRosterWithChatSequence ( char* FullPathAndFullFilename )
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

  fpath = FullPathAndFullFilename;

  // #define END_OF_DIALOGUE_FILE_STRING "*** End of Dialogue File Information ***"
#define CHAT_CHARACTER_BEGIN_STRING "Beginning of new chat dialog for character=\""
#define CHAT_CHARACTER_END_STRING "End of chat dialog for character=\""
#define NEW_OPTION_BEGIN_STRING "New Option Nr="

  //--------------------
  // At first we read the whole chat file information into memory
  //
  ChatData = ReadAndMallocAndTerminateFile( fpath , CHAT_CHARACTER_END_STRING ) ;
  SectionPointer = ChatData ;

  //--------------------
  // Now we search for the desired chat section, cause most likely
  // there will be more than one person to chat in this chat file soon
  //
#define UNINITIALIZED_SECTION_CODE "NOPERSONATALL"
  NextChatSectionCode = UNINITIALIZED_SECTION_CODE ;
  SectionPointer = ChatData;

  //--------------------
  // Now we locate the end of this chat section and put a 
  // termination character there, so we can use string functions
  // conveniently on the given section.
  //
  // EndOfSectionPointer = LocateStringInData ( SectionPointer , CHAT_CHARACTER_END_STRING );
  // *EndOfSectionPointer = 0;
  //
  EndOfSectionPointer = SectionPointer + strlen ( SectionPointer );

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
      // For now we will just add some 'default' position here.  Later we might
      // really load the current position of each dialog box, some information
      // that will only be needed in the Dialog Editor anyway.
      //
      if ( strstr ( SectionPointer , "PositionX=" ) != NULL )
	{
	  ReadValueFromString( SectionPointer , "PositionX=" , "%d" , 
			       & ( ChatRoster[ OptionIndex ] . position_x ) , TempEndPointer );
	  ReadValueFromString( SectionPointer , "PositionY=" , "%d" , 
			       & ( ChatRoster[ OptionIndex ] . position_y ) , TempEndPointer );
	}
      else
	{
	  ChatRoster [ OptionIndex ] . position_x = ( 2 + ( OptionIndex % 10 ) ) * 30 ;
	  ChatRoster [ OptionIndex ] . position_y = ( 2 + ( OptionIndex / 10 ) ) * 30 ;
	}

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

