/* ----------------------------------------------------------------------
 * This function works a malloc, except that it also checks for
 * success and terminates in case of "out of memory", so we dont
 * need to do this always in the code.
 * ---------------------------------------------------------------------- */
void *
MyMalloc (long Mamount)
{
  void *Mptr = NULL;

  if ((Mptr = malloc ((size_t) Mamount)) == NULL)
    {
      printf (" MyMalloc(%ld) did not succeed!\n", Mamount);
      Terminate(ERR);
    }

  return Mptr;
}; // void* MyMalloc ( long Mamount )

/* ----------------------------------------------------------------------
 * This function is used for debugging purposes.  It writes the
 * given string either into a file, on the screen, or simply does
 * nothing according to currently set debug level.
 * ---------------------------------------------------------------------- */
void
DebugPrintf (int db_level, char *fmt, ...)
{
  va_list args;
  char *tmp;
  va_start (args, fmt);

  if (db_level <= debug_level)
    {
      tmp = (char *) MyMalloc (1000000 + 1);
      vsprintf (tmp, fmt, args);
      fprintf (stderr, tmp);

      free (tmp);
    }

  va_end (args);
}; // void DebugPrintf (int db_level, char *fmt, ...)

/* ----------------------------------------------------------------------
 * This function should help to simplify and standardize the many error
 * messages possible in Freedroid RPG.
 * ---------------------------------------------------------------------- */
void
GiveStandardErrorMessage ( char* FunctionName , char* ProblemDescription, int InformDevelopers , int IsFatal )
{
  fprintf (stderr, "\n----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n" );
  fprintf (stderr, "In Function: %s.\n\n" , FunctionName );
  fprintf (stderr, "Problem Description: %s.\n\n" , ProblemDescription );

  if ( InformDevelopers )
    {
      fprintf (stderr, 
"If you encounter this message, please inform the Freedroid developers\n\
about the problem, best by sending e-mail to \n\
\n\
freedroid-discussion@lists.sourceforge.net\n\
\n\
Thanks a lot!\n\n" );
    }

  if ( IsFatal )
    {
      fprintf (stderr, 
"Freedroid will terminate now to draw attention to the problems it could\n\
not sesolve.  Sorry if that interrupts a major game of yours...\n" );
    }
  else
    {
      fprintf (stderr, 
"The error mentioned above is not a big problem for Freedroid.  Therefore\n\
we'll continue execution right now.\n" );
    }

  fprintf (stderr, "----------------------------------------------------------------------\n" );

  if ( IsFatal ) Terminate ( ERR );
  
}; // void GiveStandardErrorMessage ( ... )



/* ----------------------------------------------------------------------
 * This function does something similar to memmem.  Indeed, it would be
 * best perhaps if it did exactly the same as memmem, but since we do not
 * use gnu extensions for compatibility reasons, we go this way.
 *
 * Be careful with using this function for searching just one byte!!
 *
 * Be careful with using this function for non-string NEEDLE!!
 *
 * Haystack can of course have ANY form!!!
 *
 * ---------------------------------------------------------------------- */
void *
MyMemmem ( unsigned char *haystack, size_t haystacklen, unsigned char *needle, size_t needlelen)
{
  unsigned char* NextFoundPointer;
  void* MatchPointer;
  size_t SearchPos=0;

  while ( haystacklen - SearchPos > 0 )
    {
      //--------------------
      // We search for the first match OF THE FIRST CHARACTER of needle
      //
      NextFoundPointer = memchr ( haystack+SearchPos , needle[0] , haystacklen-SearchPos );
      
      //--------------------
      // if not even that was found, we can immediately return and report our failure to find it
      //
      if ( NextFoundPointer == NULL ) return ( NULL );

      //--------------------
      // Otherwise we see, if also the rest of the strings match this time ASSUMING THEY ARE STRINGS!
      // In case of a match, we can return immediately
      //
      MatchPointer = strstr( NextFoundPointer , needle );
      if ( MatchPointer != NULL ) return ( MatchPointer );

      //--------------------
      // At this point, we know that we had no luck with this one occasion of a first-character-match
      // and must continue after this one occasion with our search
      SearchPos = NextFoundPointer - haystack + 1;
    }

  return( NULL );
}; // void *MyMemmem ( ... );


/* ----------------------------------------------------------------------
 * This function looks for a string begin indicator and takes the string
 * from after there up to a sting end indicator and mallocs memory for
 * it, copys it there and returns it.
 * The original source string specified should in no way be modified.
 * ---------------------------------------------------------------------- */
char*
ReadAndMallocStringFromData ( char* SearchString , char* StartIndicationString , char* EndIndicationString ) 
{
  char* SearchPointer;
  char* EndOfStringPointer;
  char* ReturnString;
  int StringLength;

  if ( (SearchPointer = strstr ( SearchString , StartIndicationString )) == NULL )
    {
      fprintf( stderr, "\n\nStartIndicationString: '%s'\n" , StartIndicationString );
      GiveStandardErrorMessage ( "ReadAndMalocStringFromData(...)" , "\
The string that is supposed to prefix an entry in a text data file\n\
of Freedroid was not found within this text data file.\n\
This indicates some corruption in the data file in question.",
				 PLEASE_INFORM, IS_FATAL );
    }
  else
    {
      // Now we move to the beginning
      SearchPointer += strlen ( StartIndicationString );

      // Now we move to the end with the end pointer
      if ( (EndOfStringPointer = strstr( SearchPointer , EndIndicationString ) ) == NULL )
	{
	  fprintf( stderr, "\n\nEndIndicationString: '%s'\n" , EndIndicationString );
	  GiveStandardErrorMessage ( "ReadAndMalocStringFromData(...)" , "\
The string that is supposed to terminate an entry in a text data file\n\
of Freedroid was not found within this text data file.\n\
This indicates some corruption in the data file in question.",
				     PLEASE_INFORM, IS_FATAL );
	}

      // Now we allocate memory and copy the string...
      StringLength = EndOfStringPointer - SearchPointer ;
      
      ReturnString = MyMalloc ( StringLength + 1 );
      strncpy ( ReturnString , SearchPointer , StringLength );
      ReturnString[ StringLength ] = 0;

      DebugPrintf( 2 , "\nchar* ReadAndMalocStringFromData (...): Successfully identified string : %s." , ReturnString );
    }
  return ( ReturnString );
}; // char* ReadAndMallocStringFromData ( ... )

/* ----------------------------------------------------------------------
 * This function counts the number of occurences of a string in a given
 * other string.
 * ---------------------------------------------------------------------- */
int
CountStringOccurences ( char* SearchString , char* TargetString ) 
{
  int Counter=0;
  char* CountPointer;

  CountPointer = SearchString;

  while ( ( CountPointer = strstr ( CountPointer, TargetString ) ) != NULL)
    {
      CountPointer += strlen ( TargetString );
      Counter++;
    }
  return ( Counter );
}; // CountStringOccurences ( char* SearchString , char* TargetString ) 

/* ----------------------------------------------------------------------
 * This function read in a file with the specified name, allocated 
 * memory for it of course, looks for the file end string and then
 * terminates the whole read in file with a 0 character, so that it
 * can easily be treated like a common string.
 * ---------------------------------------------------------------------- */
char* 
ReadAndMallocAndTerminateFile( char* filename , char* File_End_String ) 
{
  struct stat stbuf;
  FILE *DataFile;
  char *Data;
  char *ReadPointer;
  long MemoryAmount;

  DebugPrintf ( 1 , "\nchar* ReadAndMallocAndTerminateFile ( char* filename ) : The filename is: %s" , filename );

  // Read the whole theme data to memory 
  if ((DataFile = fopen ( filename , "r")) == NULL)
    {
      fprintf( stderr, "\n\nfilename: '%s'\n" , filename );
      GiveStandardErrorMessage ( "ReadAndMallocAndTerminateFile(...)" , "\
Freedroid was unable to open a given text file, that should be there and\n\
should be accessible.\n\
This indicates a serious bug in this installation of Freedroid.",
				 PLEASE_INFORM, IS_FATAL );
    }
  else
    {
      DebugPrintf ( 1 , "\nchar* ReadAndMallocAndTerminateFile ( char* filename ) : Opening file succeeded...");
    }

  if (fstat (fileno (DataFile), &stbuf) == EOF)
    {
      fprintf( stderr, "\n\nfilename: '%s'\n" , filename );
      GiveStandardErrorMessage ( "ReadAndMallocAndTerminateFile(...)" , "\
Freedroid was unable to fstat a given text file, that should be there and\n\
should be accessible.\n\
This indicates a strange bug in this installation of Freedroid, that is\n\
very likely a problem with the file/directory permissions of the files\n\
belonging to Freedroid.",
				 PLEASE_INFORM, IS_FATAL );
    }
  else
    {
      DebugPrintf ( 1 , "\nchar* ReadAndMallocAndTerminateFile ( char* filename ) : fstating file succeeded...");
    }

  MemoryAmount = stbuf.st_size + 64*2 + 10000;
  Data = (char *) MyMalloc ( MemoryAmount );

  fread ( Data, (size_t) 64, (size_t) (stbuf.st_size / 64 +1 ), DataFile);

  DebugPrintf ( 1 , "\nchar* ReadAndMallocAndTerminateFile ( char* filename ) : Reading file succeeded...");

  if (fclose ( DataFile ) == EOF)
    {
      fprintf( stderr, "\n\nfilename: '%s'\n" , filename );
      GiveStandardErrorMessage ( "ReadAndMallocAndTerminateFile(...)" , "\
Freedroid was unable to close a given text file, that should be there and\n\
should be accessible.\n\
This indicates a strange bug in this installation of Freedroid, that is\n\
very likely a problem with the file/directory permissions of the files\n\
belonging to Freedroid.",
				 PLEASE_INFORM, IS_FATAL );
    }
  else
    {
      DebugPrintf( 1 , "\nchar* ReadAndMallocAndTerminateFile ( char* filename ) : file closed successfully...");
    }

  DebugPrintf ( 1 , "\nchar* ReadAndMallocAndTerminateFile ( char* filename ) : Adding a 0 at the end of read data....");

  // NOTE: Since we do not assume to always have pure text files here, we switched to
  // MyMemmem, so that we can handle 0 entries in the middle of the file content as well
  //
  // if ( (ReadPointer = strstr( Data , File_End_String ) ) == NULL )
  if ( ( ReadPointer = MyMemmem ( Data , (size_t) MemoryAmount , File_End_String , (size_t) strlen( File_End_String ) ) ) == NULL )
    {
      fprintf( stderr, "\n\nfilename: '%s'\n" , filename );
      fprintf( stderr, "File_End_String: '%s'\n" , File_End_String );
      GiveStandardErrorMessage ( "ReadAndMallocAndTerminateFile(...)" , "\
Freedroid was unable to find the string, that should indicate the end of\n\
the given text file within this file.\n\
This indicates a corrupt or outdated data or saved game file.",
				 PLEASE_INFORM, IS_FATAL );
    }
  else
    {
      // ReadPointer+=strlen( File_End_String ) + 1; // no need to destroy the end pointer :-)
      ReadPointer[0]=0; // we want to handle the file like a string, even if it is not zero
                       // terminated by nature.  We just have to add the zero termination.
    }

  // DebugPrintf( 1 , "\nchar* ReadAndMallocAndTerminateFile ( char* filename ) : The content of the read file: \n%s" , Data );

  return ( Data );
}; // char* ReadAndMallocAndTerminateFile( char* filename) 

/* ----------------------------------------------------------------------
 * This function tries to locate a string in some given data string.
 * The data string is assumed to be null terminated.  Otherwise SEGFAULTS
 * might happen.
 * 
 * The return value is a pointer to the first instance where the substring
 * we are searching is found in the main text.
 * ---------------------------------------------------------------------- */
char* 
LocateStringInData ( char* SearchBeginPointer, char* SearchTextPointer )
{
  char* temp;

  if ( ( temp = strstr ( SearchBeginPointer , SearchTextPointer ) ) == NULL)
    {
      fprintf( stderr, "\n\nSearchTextPointer: '%s'\n" , SearchTextPointer );
      GiveStandardErrorMessage ( "LocateStringInData(...)" , "\
The string that was supposed to be in the text data file could not be found.\n\
This indicates a corrupted or seriously outdated game data or saved game file.",
				 PLEASE_INFORM, IS_FATAL );
    }
  else
    {
      DebugPrintf( 2 , "\nchar* LocateStringInDate ( char* SearchBeginText , char* SearchTextPointer ) : String %s successfully located within data. " , SearchTextPointer );
    }
  
  return ( temp );

}; // char* LocateStringInData ( ... )

/* ----------------------------------------------------------------------
 * This function should analyze a given passage of text, locate an 
 * indicator for a value, and read in the value.
 * ---------------------------------------------------------------------- */
void
ReadValueFromString( char* SearchBeginPointer , char* ValuePreceedText , char* FormatString , void* TargetValue , char* EndOfSearchSectionPointer )
{
  char OldTerminaterCharValue;
  char* SourceLocation;

  // We shortly make a termination char into the string.
  OldTerminaterCharValue=EndOfSearchSectionPointer[0];
  EndOfSearchSectionPointer[0]=0;

  // Now we locate the spot, where we finally will find our value
  SourceLocation = LocateStringInData ( SearchBeginPointer , ValuePreceedText );
  SourceLocation += strlen ( ValuePreceedText );

  //--------------------
  // Attention!!! 
  // Now we try to read in the value!!!
  //
  if ( sscanf ( SourceLocation , FormatString , TargetValue ) == EOF )
    {
      fprintf( stderr, "\n\nFormatString: '%s'\n" , FormatString );
      fprintf( stderr, "ValuePreceedText: '%s'\n" , ValuePreceedText );
      GiveStandardErrorMessage ( "ReadValueFromString(...)" , "\
sscanf using a certain format string failed!\n\
This indicates a corrupted or seriously outdated game data or saved game file.",
				 PLEASE_INFORM, IS_FATAL );
    }
  else
    {
      DebugPrintf( 2 , "\nvoid ReadValueFromString ( .... ) : value read in successfully.");
    }

  // Now that we are done, we restore the given SearchArea to former glory
  EndOfSearchSectionPointer[0]=OldTerminaterCharValue;
}; // void ReadValueFromString( ... )
