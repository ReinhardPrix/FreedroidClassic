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
 * This file contains miscellaeous helpful functions for Freedroid.
 * ---------------------------------------------------------------------- */
/*
 * This file has been checked for remains of german comments in the code
 * I you still find some, please just kill it mercilessly.
 */
#define _misc_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"

//--------------------
// The definition of the message structure can stay here,
// because its only needed in this module.
//
typedef struct
{
  void *NextMessage;
  int MessageCreated;
  char *MessageText;
}
message, Message;

mouse_press_button AllMousePressButtons[ MAX_MOUSE_PRESS_BUTTONS ] =
  {
    { NULL , "CHAButton.png"                    , { 600 , 430 ,  38 ,  22 } } ,
    { NULL , "INVButton.png"                    , { 600 , 400 ,  38 ,  22 } } ,
    { NULL , "SKIButton.png"                    , { 600 , 370 ,  38 ,  22 } } ,
    { NULL , "PlusButton.png"                   , { 600 , 430 ,  38 ,  22 } } ,
    { NULL , "UPButton.png"                     , { 580 , 180 ,  50 , 100 } } ,
    { NULL , "DOWNButton.png"                   , { 580 , 330 ,  50 , 100 } } ,
    { NULL , "LEFTButton.png"                   , { 300 , 420 , 100 ,  50 } } ,
    { NULL , "RIGHTButton.png"                  , { 450 , 420 , 100 ,  50 } } ,
    { NULL , "MapExitButton.png"                , {  50 ,   5 , 100 ,  50 } } ,
    { NULL , "MapUnlockDoorButton_gray.png"     , { 200 ,   5 , 100 ,  50 } } ,
    { NULL , "MapUnlockDoorButton_yellow.png"   , { 200 ,   5 , 100 ,  50 } } ,
    { NULL , "MapUnlockDoorButton_red.png"      , { 200 ,   5 , 100 ,  50 } } ,
    { NULL , "MapGunOnOffButton_gray.png"       , { 350 ,   5 , 100 ,  50 } } ,
    { NULL , "MapGunOnOffButton_yellow.png"     , { 350 ,   5 , 100 ,  50 } } ,
    { NULL , "MapGunOnOffButton_red.png"        , { 350 ,   5 , 100 ,  50 } } ,
    { NULL , "MapSecurityButtonMiddle.png"      , {  40 , 425 , 120 ,  50 } } ,
    { NULL , "MapSecurityButtonLeft.png"        , {   5 , 425 ,  30 ,  50 } } ,
    { NULL , "MapSecurityButtonRight.png"       , { 170 , 425 ,  30 ,  50 } } ,
    { NULL , "MapPasswordButtonMiddle.png"      , { 310 , 425 , 120 ,  50 } } ,
    { NULL , "MapSecurityButtonLeft.png"        , { 280 , 425 ,  30 ,  50 } } ,
    { NULL , "MapSecurityButtonRight.png"       , { 440 , 425 ,  30 ,  50 } } ,
    { NULL , "MapRequestEnergyRation_green.png" , { 200 ,  60 , 100 ,  50 } } ,
    { NULL , "MapRequestEnergyRation_red.png"   , { 200 ,  60 , 100 ,  50 } } ,
    { NULL , "MapReadEmail_green.png"           , { 350 ,  60 , 100 ,  50 } } ,
    { NULL , "MapReadEmail_red.png"             , { 350 ,  60 , 100 ,  50 } } ,

    { NULL , "MapGunTypeButton1_red.png"        , { 570 ,  64 ,  64 ,  64 } } ,
    { NULL , "MapGunTypeButton2_red.png"        , { 570 , 128 ,  64 ,  64 } } ,
    { NULL , "MapGunTypeButton3_red.png"        , { 570 , 192 ,  64 ,  64 } } ,
    { NULL , "MapGunTypeButton4_red.png"        , { 570 , 256 ,  64 ,  64 } } ,
    { NULL , "MapGunOnButton_gray.png"          , { 500 ,   5 , 100 ,  50 } } ,
    { NULL , "MapGunOnButton_yellow.png"        , { 500 ,   5 , 100 ,  50 } } ,
    { NULL , "MapGunOnButton_red.png"           , { 500 ,   5 , 100 ,  50 } } ,
    { NULL , "MapGunTypeButton1_yellow.png"     , { 570 ,  64 ,  64 ,  64 } } ,
    { NULL , "MapGunTypeButton2_yellow.png"     , { 570 , 128 ,  64 ,  64 } } ,
    { NULL , "MapGunTypeButton3_yellow.png"     , { 570 , 192 ,  64 ,  64 } } ,
    { NULL , "MapGunTypeButton4_yellow.png"     , { 570 , 256 ,  64 ,  64 } } ,

    { NULL , "ConsoleIdentifyButton_green.png"  , {  50 ,   5 , 100 ,  50 } } ,
    { NULL , "ConsoleIdentifyButton_red.png"    , {  50 ,   5 , 100 ,  50 } } ,
    { NULL , "ConsoleIdentifyButton_yellow.png" , {  50 ,   5 , 100 ,  50 } } 

  }; // AllMousePressButtons[ MAX_MOUSE_PRESS_BUTTONS ] 

/* ----------------------------------------------------------------------
 * This function checks if a given screen position lies within the 
 * inventory screen toggle button or not.
 * ---------------------------------------------------------------------- */
int
CursorIsOnButton( int ButtonIndex , int x , int y )
{
  //--------------------
  // First a sanity check if the button index given does make
  // some sense.
  //
  if ( ( ButtonIndex >= MAX_MOUSE_PRESS_BUTTONS ) || ( ButtonIndex < 0 ) )
    {
      fprintf (stderr, "\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
A Button that should be checked for mouse contact was requested, but the\n\
button index given exceeds the number of buttons defined in freedroid.\n\
\n\
Freedroid will terminate now to draw attention to the problem...\n\
----------------------------------------------------------------------\n" );
      Terminate ( ERR );
    }

  //--------------------
  // Now that we know we have been given a valid button index,
  // we can start to check if the mouse cursor really is on that
  // rectangle or not.
  //
  if ( x > AllMousePressButtons[ ButtonIndex ] . button_rect . x + 
       AllMousePressButtons[ ButtonIndex ] . button_rect . w  ) return ( FALSE );
  if ( x < AllMousePressButtons[ ButtonIndex ] . button_rect . x ) return ( FALSE );
  if ( y > AllMousePressButtons[ ButtonIndex ] . button_rect . y + 
       AllMousePressButtons[ ButtonIndex ] . button_rect . h ) return ( FALSE );
  if ( y < AllMousePressButtons[ ButtonIndex ] . button_rect . y ) return ( FALSE );

  //--------------------
  // So since the cursor is not outside of this rectangle, it must
  // we inside, and so we'll return this answer.
  //
  return ( TRUE );

}; // int CursorIsOnButton( int ButtonIndex , int x , int y )

/* ----------------------------------------------------------------------
 * This function blits a button to the screen.  The button must have been
 * defined prior to this in the above button list.
 * ---------------------------------------------------------------------- */
void 
ShowGenericButtonFromList ( int ButtonIndex )
{ 
  SDL_Surface *tmp;
  char *fpath;
  SDL_Rect Temp_Blitting_Rect;

  //--------------------
  // First a sanity check if the button index given does make
  // some sense.
  //
  if ( ( ButtonIndex >= MAX_MOUSE_PRESS_BUTTONS ) || ( ButtonIndex < 0 ) )
    {
      fprintf (stderr,
	       "\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
A Button that should be displayed on the screen was requested, but the\n\
button index given exceeds the number of buttons defined in freedroid.\n\
\n\
Freedroid will terminate now to draw attention to the problem...\n\
----------------------------------------------------------------------\n" );
      Terminate ( ERR );
    }

  //--------------------
  // Now we check if we have to load the button image still
  // or if it is perhaps already loaded into memory.
  //
  if ( AllMousePressButtons[ ButtonIndex ] . button_surface == NULL )
    {
      fpath = find_file ( AllMousePressButtons[ ButtonIndex ] . button_image_file_name , GRAPHICS_DIR, FALSE);
      tmp = IMG_Load( fpath );
      if ( tmp == NULL )
	{
	  fprintf (stderr,
		   "\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
An image file for a button that should be displayed on the screen couldn't\n\
be successfully loaded into memory.\n\
\n\
The name of the problematic file is: %s\n\
\n\
This is an indication of a severe bug/installation problem of freedroid.\n\
If you encounter this message, please inform the developers about it, as\n\
always, best via e-mail to freedroid-discussion@lists.sourceforge.net.\n\
Thanks a lot.\n\
Now Freedroid will terminate to draw attention to the problem...\n\
----------------------------------------------------------------------\n" , fpath );
	  Terminate ( ERR );
	}
      AllMousePressButtons[ ButtonIndex ] . button_surface = SDL_DisplayFormat ( tmp );
      SDL_FreeSurface ( tmp );
    }

  //--------------------
  // Now that we know we have the button image loaded, we can start
  // to blit the button image to the screen.
  //
  // But in order not to damage the original rect data, we use the
  // temp value as parameter for the SDL_Blit thing..
  //
  Copy_Rect ( AllMousePressButtons[ ButtonIndex ] . button_rect , Temp_Blitting_Rect );
  SDL_BlitSurface( AllMousePressButtons[ ButtonIndex ] . button_surface , NULL , Screen , &Temp_Blitting_Rect );

}; // void ShowGenericButtonFromList ( int ButtonIndex )




#define SETTINGS_STRUCTURE_RAW_DATA_STRING "\nSettings Raw Data:\n"
#define END_OF_SETTINGS_DATA_STRING "\nEnd of Settings File.\n"

#define MESPOSX 0
#define MESPOSY 64
#define MESHOEHE 8
#define MESBARBREITE 320
#define MAX_MESSAGE_LEN 100
#define MESBAR_MEM MESBARBREITE*MESHOEHE+1000

void CreateMessageBar (char *MText);
void AdvanceQueue (void);

unsigned char *MessageBar;
message *Queue = NULL;
// int ThisMessageTime=0;               /* Counter fuer Message-Timing */

struct timeval now, oneframetimestamp, tenframetimestamp,
  onehundredframetimestamp, differenz;
long oneframedelay = 0;
long tenframedelay = 0;
long onehundredframedelay = 0;
float FPSover1 = 10;
float FPSover10 = 10;
float FPSover100 = 10;
Uint32 Now_SDL_Ticks;
Uint32 One_Frame_SDL_Ticks;
Uint32 Ten_Frame_SDL_Ticks;
Uint32 Onehundred_Frame_SDL_Ticks;
int framenr = 0;

char *homedir = NULL;
char ConfigDir[255]="\0";




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
      fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
In function 'char* ReadAndMalocStringFromData ( ... ):\n\
A starter string that was supposed to be in some data, most likely from an external\n\
data file could not be found, which indicates a corrupted data file or \n\
a serious bug in the reading functions.\n\
\n\
The string that couldn't be located was: %s\n\
\n\
Please check that your external text files are properly set up.\n\
\n\
Please also don't forget, that you might have to run 'make install'\n\
again after you've made modifications to the data files in the source tree.\n\
\n\
Freedroid will terminate now to draw attention to the data problem it could\n\
not resolve.... Sorry, if that interrupts a major game of yours.....\n\
----------------------------------------------------------------------\n\
\n" , StartIndicationString );
      Terminate(ERR);
    }
  else
    {
      // Now we move to the beginning
      SearchPointer += strlen ( StartIndicationString );

      // Now we move to the end with the end pointer
      if ( (EndOfStringPointer = strstr( SearchPointer , EndIndicationString ) ) == NULL )
	{
	  fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
In function 'char* ReadAndMalocStringFromData ( ... ):\n\
A terminating string that was supposed to be in some data, most likely from an external\n\
data file could not be found, which indicates a corrupted data file or \n\
a serious bug in the reading functions.\n\
\n\
The string that couldn't be located was: %s\n\
\n\
Please check that your external text files are properly set up.\n\
\n\
Please also don't forget, that you might have to run 'make install'\n\
again after you've made modifications to the data files in the source tree.\n\
\n\
Freedroid will terminate now to draw attention to the data problem it could\n\
not resolve.... Sorry, if that interrupts a major game of yours.....\n\
----------------------------------------------------------------------\n\
\n" , EndIndicationString );
	  Terminate(ERR);
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
      fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
In function 'char* ReadAndMallocAndTerminateFile ( char* filename ):\n\
\n\
Freedroid was unable to open a given text file, that should be there and\n\
should be accessible.\n\
\n\
This might be due to a wrong file name in a mission file, a wrong filename\n\
in the source or a serious bug in the source.\n\
\n\
The file that couldn't be located was: %s\n\
\n\
Please check that your external text files are properly set up.\n\
\n\
Please also don't forget, that you might have to run 'make install'\n\
again after you've made modifications to the data files in the source tree.\n\
\n\
Freedroid will terminate now to draw attention to the data problem it could\n\
not resolve.... Sorry, if that interrupts a major game of yours.....\n\
----------------------------------------------------------------------\n\
\n" , filename );
      Terminate(ERR);
    }
  else
    {
      DebugPrintf ( 1 , "\nchar* ReadAndMallocAndTerminateFile ( char* filename ) : Opening file succeeded...");
    }

  if (fstat (fileno (DataFile), &stbuf) == EOF)
    {
      DebugPrintf ( 0 , "\nchar* ReadAndMallocAndTerminateFile ( char* filename ) : Error fstat-ing File....");
      Terminate(ERR);
    }
  else
    {
      DebugPrintf ( 1 , "\nchar* ReadAndMallocAndTerminateFile ( char* filename ) : fstating file succeeded...");
    }

  MemoryAmount = stbuf.st_size + 64*2 + 10000;
  if ( ( Data = (char *) MyMalloc ( MemoryAmount  ) ) == NULL )
    {
      DebugPrintf ( 0 , "\nchar* ReadAndMallocAndTerminateFile ( char* filename ) : Out of Memory? ");
      Terminate(ERR);
    }

  fread ( Data, (size_t) 64, (size_t) (stbuf.st_size / 64 +1 ), DataFile);

  DebugPrintf ( 1 , "\nchar* ReadAndMallocAndTerminateFile ( char* filename ) : Reading file succeeded...");

  if (fclose ( DataFile ) == EOF)
    {
      DebugPrintf( 0 , "\nchar* ReadAndMallocAndTerminateFile ( char* filename ) : Error while trying to close lift file....Terminating....\n\n");
      Terminate(ERR);
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
      fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
In function 'char* ReadAndMallocAndTerminateFile ( char* filename ):\n\
\n\
Freedroid was unable to find the string, that should terminate the given\n\
file within this file.\n\
\n\
This might be due to a corrupt text file on disk that does not confirm to\n\
the file standards of this version of freedroid or (less likely) to a serious\n\
bug in the reading function.\n\
\n\
The file that is concerned is: %s\n\
The string, that could not be located was: %s\n\
\n\
Please check that your external text files are properly set up.\n\
\n\
Please also don't forget, that you might have to run 'make install'\n\
again after you've made modifications to the data files in the source tree.\n\
\n\
Freedroid will terminate now to draw attention to the data problem it could\n\
not resolve.... Sorry, if that interrupts a major game of yours.....\n\
----------------------------------------------------------------------\n\
\n" , filename , File_End_String );
      Terminate(ERR);
    }
  else
    {
      // ReadPointer+=strlen( File_End_String ) + 1; // no need to destroy the end pointer :-)
      ReadPointer[0]=0; // we want to handle the file like a string, even if it is not zero
                       // terminated by nature.  We just have to add the zero termination.
    }

  DebugPrintf( 1 , "\nchar* ReadAndMallocAndTerminateFile ( char* filename ) : The content of the read file: \n%s" , Data );

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
      fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
In function 'char* LocateStringInData ( char* SearchBeginPointer, char* SearchTextPointer ):\n\
A string that was supposed to be in some data, most likely from an external\n\
data file could not be found, which indicates a corrupted data file or \n\
a serious bug in the reading functions.\n\
\n\
The string that couldn't be located was: %s\n\
\n\
Please check that your external text files are properly set up.\n\
\n\
Please also don't forget, that you might have to run 'make install'\n\
again after you've made modifications to the data files in the source tree.\n\
\n\
Freedroid will terminate now to draw attention to the data problem it could\n\
not resolve.... Sorry, if that interrupts a major game of yours.....\n\
----------------------------------------------------------------------\n\
\n" , SearchTextPointer );
      Terminate(ERR);
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
      fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
In function 'void ReadValueFromString ( .... ):\n\
\n\
Freedroid has tried to read a value conformant to the format string %s.\n\
\n\
But the reading via sscanf didn't work.\n\
\n\
This might be indicating that the data, most likely from an external\n\
data file, was corrupt.\n\
\n\
Also a serious bug in the reading function might (less likely) be a cause for the problem.\n\
\n\
The text that should be preceeding the real value was: %s\n\
\n\
Please check that your external text files are properly set up.\n\
\n\
Please also don't forget, that you might have to run 'make install'\n\
again after you've made modifications to the data files in the source tree.\n\
\n\
Freedroid will terminate now to draw attention to the data problem it could\n\
not resolve.... Sorry, if that interrupts a major game of yours.....\n\
----------------------------------------------------------------------\n\
\n" , FormatString , ValuePreceedText );
      Terminate(ERR);
    }
  else
    {
      DebugPrintf( 2 , "\nvoid ReadValueFromString ( .... ) : value read in successfully.");
    }

  // Now that we are done, we restore the given SearchArea to former glory
  EndOfSearchSectionPointer[0]=OldTerminaterCharValue;
}; // void ReadValueFromString( ... )

/* -----------------------------------------------------------------
 * find a given filename in subdir relative to DATADIR, 
 * using theme subdir if use_theme==TRUE
 *
 * if you pass NULL as subdir, it will be ignored
 *
 * returns pointer to _static_ string array File_Path, which 
 * contains the full pathname of the file.
 *
 * !! do never try to free the returned string !!
 *
 * ----------------------------------------------------------------- */
char *
find_file (char *fname, char *subdir, int use_theme)
{
  static char File_Path[5000];   /* hope this will be enough */
  FILE *fp;  // this is the file we want to find?
  int i;

  if (!fname)
    {
      // printf ("\nError. find_file() called with empty filename!\n");
      fprintf (stderr, "\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
A find_file call has been issued to generate the full path name of a\n\
certain file, but the file name given is an empty string!\n\
This is indicates a severe bug in Freedroid.\n\
\n\
Freedroid will terminate now to draw attention to the problem...\n\
----------------------------------------------------------------------\n" );
      Terminate ( ERR );
      return (NULL);
    }
  if (!subdir)
    subdir = "";

  for (i=0; i < 2; i++)
    {
      if (i==0)
	strcpy (File_Path, "..");   /* first try local subdirs */
      if (i==1)
	strcpy (File_Path, DATADIR); /* then the DATADIR */

      strcat (File_Path, "/");
      strcat (File_Path, subdir);
      strcat (File_Path, "/");

      if (use_theme)
	strcat (File_Path, GameConfig.Theme_SubPath);

      strcat (File_Path, fname);
      
      if ( (fp = fopen (File_Path, "r")) != NULL)  /* found it? */
	{
	  fclose (fp);
	  break;
	}
    } /* for i */

  return (File_Path);
	
}; // char * find_file ( ... )

/* ----------------------------------------------------------------------
 * This function realises the Pause-Mode: the game process is halted,
 * while the graphics and animations are not.  This mode 
 * can further be toggled from PAUSE to CHEESE, which is
 * a feature from the original program that should probably
 * allow for better screenshots.
 * ---------------------------------------------------------------------- */
void
Pause (void)
{
  int Pause = TRUE;

  Activate_Conservative_Frame_Computation();

  Me[0].status = PAUSE;
  Assemble_Combat_Picture ( DO_SCREEN_UPDATE );

  while ( Pause )
    {
      SetNewBigScreenMessage( " Pause " );
      AnimateInfluence ( 0 );
      AnimateRefresh ();
      AnimateEnemys ();
      DisplayBanner (NULL, NULL, 0);
      Assemble_Combat_Picture ( DO_SCREEN_UPDATE );
      
      if (CPressed ())
	{
	  Me[0].status = CHEESE;
	  DisplayBanner (NULL, NULL,  0 );
	  Assemble_Combat_Picture ( DO_SCREEN_UPDATE );

	  while (!SpacePressed ()); /* stay CHEESE until Space pressed */
	  while ( SpacePressed() ); /* then wait for Space released */
	  
	  Me[0].status = PAUSE;       /* return to normal PAUSE */
	} /* if (CPressed) */

      if ( SpacePressed() )
	{
	  Pause = FALSE;
	  while ( SpacePressed() );  /* wait for release */
	}

      //--------------------
      // During the Pause mode, there is again no need to hog the CPU and to 
      // go at full force.  We introduce some rest for the CPU here...
      //
      usleep(50);

    } /* while (Pause) */
  SetNewBigScreenMessage( "" );
  return;
}; // Pause () 

/* ----------------------------------------------------------------------
 * This function starts the time-taking process.  Later the results
 * of this function will be used to calculate the current framerate
 * 
 * Two methods of time-taking are available.  One uses the SDL 
 * ticks.  This seems LESS ACCURATE.  The other one uses the
 * standard ansi c gettimeofday functions and are MORE ACCURATE
 * but less convenient to use.
 * ---------------------------------------------------------------------- */
void 
StartTakingTimeForFPSCalculation(void)
{
  /* This ensures, that 0 is never an encountered framenr,
   * therefore count to 100 here
   * Take the time now for calculating the frame rate
   * (DO NOT MOVE THIS COMMAND PLEASE!) */
  framenr++;
  
#ifdef USE_SDL_FRAMERATE
  One_Frame_SDL_Ticks=SDL_GetTicks();
  if (framenr % 10 == 1)
    Ten_Frame_SDL_Ticks=SDL_GetTicks();
  if (framenr % 100 == 1)
    {
      Onehundred_Frame_SDL_Ticks=SDL_GetTicks();
      // printf("\n%f",1/Frame_Time());
      // printf("Me[0].pos.x: %g Me[0].pos.y: %g Me[0].speed.x: %g Me[0].speed.y: %g \n",
      //Me[0].pos.x, Me[0].pos.y, Me[0].speed.x, Me[0].speed.y );
      //printf("Me[0].maxspeed.x: %g \n",
      //	     Druidmap[Me[0].type].maxspeed );
    }
#else
  gettimeofday (&oneframetimestamp, NULL);
  if (framenr % 10 == 1)
    gettimeofday (&tenframetimestamp, NULL);
  if (framenr % 100 == 1)
    {
      gettimeofday (&onehundredframetimestamp, NULL);
      printf("\n%f",1/Frame_Time());
    }
#endif
  
}; // void StartTakingTimeForFPSCalculation(void)

/* ----------------------------------------------------------------------
 * This function computes the framerate that has been experienced
 * in this frame.  It will be used to correctly calibrate all 
 * movements of game objects.
 * 
 * NOTE:  To query the actual framerate a DIFFERENT function must
 *        be used, namely Frame_Time().
 *
 *        Two methods of time-taking are available.  One uses the SDL 
 *        ticks.  This seems LESS ACCURATE.  The other one uses the
 *        standard ansi c gettimeofday functions and are MORE ACCURATE
 *        but less convenient to use.
 * ---------------------------------------------------------------------- */
void 
ComputeFPSForThisFrame(void)
{

  // In the following paragraph the framerate calculation is done.
  // There are basically two ways to do this:
  // The first way is to use SDL_GetTicks(), a function measuring milliseconds
  // since the initialisation of the SDL.
  // The second way is to use gettimeofday, a standard ANSI C function I guess,
  // defined in time.h or so.
  // 
  // I have arranged for a definition set in defs.h to switch between the two
  // methods of ramerate calculation.  THIS MIGHT INDEED MAKE SENSE, SINCE THERE
  // ARE SOME UNEXPLAINED FRAMERATE PHENOMENA WHICH HAVE TO TO WITH KEYBOARD
  // SPACE KEY, SO PLEASE DO NOT ERASE EITHER METHOD.  PLEASE ASK JP FIRST.
  //

#ifdef USE_SDL_FRAMERATE

  Now_SDL_Ticks=SDL_GetTicks();
  oneframedelay=Now_SDL_Ticks-One_Frame_SDL_Ticks;
  tenframedelay=Now_SDL_Ticks-Ten_Frame_SDL_Ticks;
  onehundredframedelay=Now_SDL_Ticks-Onehundred_Frame_SDL_Ticks;
  
  FPSover1 = 1000 * 1 / (float) oneframedelay;
  FPSover10 = 1000 * 10 / (float) tenframedelay;
  FPSover100 = 1000 * 100 / (float) onehundredframedelay;
  
#else
  
  gettimeofday (&now, NULL);
  
  oneframedelay =
    (now.tv_usec - oneframetimestamp.tv_usec) + (now.tv_sec -
						 oneframetimestamp.
						 tv_sec) * 1000000;
  if (framenr % 10 == 0)
    tenframedelay =
      ((now.tv_usec - tenframetimestamp.tv_usec)) + (now.tv_sec -
						     tenframetimestamp.
						     tv_sec) *
      1000000;
  if ((framenr % 100) == 0)
    {
      onehundredframedelay =
	(now.tv_sec - onehundredframetimestamp.tv_sec) * 1000000 +
	(now.tv_usec - onehundredframetimestamp.tv_usec);
      framenr = 0;
    }
  
  FPSover1 = 1000000 * 1 / (float) oneframedelay;
  FPSover10 = 1000000 * 10 / (float) tenframedelay;
  FPSover100 = 1000000 * 100 / (float) onehundredframedelay;
  
#endif

}; // void ComputeFPSForThisFrame(void)

/* ----------------------------------------------------------------------
 *
 * This function is the key to independence of the framerate for various game elements.
 * It returns the average time needed to draw one frame.
 * Other functions use this to calculate new positions of moving objects, etc..
 *
 * Also there is of course a serious problem when some interuption occurs, like e.g.
 * the options menu is called or the debug menu is called or the console or the elevator
 * is entered or a takeover game takes place.  This might cause HUGE framerates, that could
 * box the influencer out of the ship if used to calculate the new position.
 *
 * To counter unwanted effects after such events we have the SkipAFewFramerates counter,
 * which instructs Rate_To_Be_Returned to return only the overall default framerate since
 * no better substitute exists at this moment.  But on the other hand, this seems to
 * work REALLY well this way.
 *
 * This counter is most conveniently set via the function Activate_Conservative_Frame_Computation,
 * which can be conveniently called from eveywhere.
 *
 * ---------------------------------------------------------------------- */
float
Frame_Time (void)
{
  float Rate_To_Be_Returned;
  
  if ( SkipAFewFrames ) 
    {
      Rate_To_Be_Returned = Overall_Average;
      return Rate_To_Be_Returned;
    }

  Rate_To_Be_Returned = (1.0 / FPSover1);

  return Rate_To_Be_Returned;

}; // float Frame_Time ( void )

int
Get_Average_FPS ( void )
{
  return ( (int) ( 1.0 / Overall_Average ) );
}; // int Get_Average_FPS( void )

/* ----------------------------------------------------------------------
 * 
 * With framerate computation, there is a problem when some interuption occurs, like e.g.
 * the options menu is called or the debug menu is called or the console or the elevator
 * is entered or a takeover game takes place.  This might cause HUGE framerates, that could
 * box the influencer out of the ship if used to calculate the new position.
 *
 * To counter unwanted effects after such events we have the SkipAFewFramerates counter,
 * which instructs Rate_To_Be_Returned to return only the overall default framerate since
 * no better substitute exists at this moment.
 *
 * This counter is most conveniently set via the function Activate_Conservative_Frame_Computation,
 * which can be conveniently called from eveywhere.
 *
 * ---------------------------------------------------------------------- */
void 
Activate_Conservative_Frame_Computation(void)
{
  // SkipAFewFrames=212;
  // SkipAFewFrames=22;
  SkipAFewFrames=3;

  // Now we are in some form of pause.  It can't
  // hurt to have the top status bar redrawn after that,
  // so we set this variable...
  BannerIsDestroyed=TRUE;

}; // void Activate_Conservative_Frame_Computation(void)

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
 * This function is used to generate an integer in range of all
 * numbers from 0 to UpperBound.
 * ---------------------------------------------------------------------- */
int
MyRandom (int UpperBound)
{
  float tmp;
  int PureRandom;
  int dice_val;    /* the result in [0, Obergrenze] */

  PureRandom = rand ();
  tmp = 1.0*PureRandom/RAND_MAX; /* random number in [0;1] */

  /* 
   * we always round OFF for the resulting int, therefore
   * we first add 0.99999 to make sure that Obergrenze has
   * roughly the same probablity as the other numbers 
   */
  dice_val = (int)( tmp * (1.0 * UpperBound + 0.99999) );

  return (dice_val);

}; // int MyRandom ( int UpperBound ) 


/* ----------------------------------------------------------------------
 * This function is kills all enemy robots on the whole ship.
 * It querys the user once for safety.
 * ---------------------------------------------------------------------- */
void
Armageddon (void)
{
  char key =' ';
  int i;

  printf ("\nKill all droids on ship (y/n) ? \n");
  while ((key != 'y') && (key != 'n'))
    key = getchar_raw ();
  if (key == 'n')
    return;
  else
    for (i = 0; i < MAX_ENEMYS_ON_SHIP; i++)
      {
	AllEnemys[i].energy = -10;
	// AllEnemys[i].Status = OUT;
      }
}; // void Armageddon(void)

/* ----------------------------------------------------------------------
 * This function teleports the influencer to a new position on the
 * ship.  THIS CAN BE A POSITION ON A DIFFERENT LEVEL.
 * ---------------------------------------------------------------------- */
void
Teleport (int LNum, int X, int Y, int PlayerNum , int Shuffling )
{
  int curLevel = LNum;
  int array_num = 0;
  Level tmp;
  int i;

  if ( curLevel != Me [ PlayerNum ] . pos . z )
    {	

      //--------------------
      // In case a real level change has happend,
      // we need to do a lot of work:
      //

      // I think this is for the unlikely case of misordered levels in 
      // the ship file used for this game?!
      //
      while ((tmp = curShip.AllLevels[array_num]) != NULL)
	{
	  if (tmp->levelnum == curLevel)
	    break;
	  else
	    array_num++;
	}

      //--------------------
      // We set a new CurLevel.  This is old and depreciated code,
      // that should sooner or later be completely deactivated.
      //
      CurLevel = curShip.AllLevels[array_num];

      // if ( Shuffling ) ShuffleEnemys ( array_num );

      ClearDetectedItemList( PlayerNum );

      Me [ PlayerNum ] . pos . x = X;
      Me [ PlayerNum ] . pos . y = Y;
      Me [ PlayerNum ] . pos . z = array_num; 

      //--------------------
      // We add some sanity check against teleporting to non-allowed
      // locations (like outside of map that is)
      //
      if ( ( LNum < 0 ) || ( Me [ PlayerNum ] . pos . x < 0 ) || ( Me [ PlayerNum ] . pos . y < 0 ) ||
	   ( LNum >= curShip.num_levels ) || 
	   ( Me [ PlayerNum ] . pos . x >= curShip.AllLevels[ array_num ] -> xlen ) ||
	   ( Me [ PlayerNum ] . pos . y >= curShip.AllLevels[ array_num ] -> ylen ) )
	{
	  fprintf (stderr, "\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
A Teleport was requested, but the location to teleport to lies outside\n\
the bounds of this 'ship' which means the current collection of levels.\n\
This is a severe error.  Please report the bug to the Freedroid team\n\
as always best via e-mail to freedroid-discussion@lists.sourceforge.net\n\
Thanks a lot.\n\
\n\
The given target location was: lev=%d x=%d y=%d.\n\
Freedroid will terminate now to draw attention to the problem...\n\
----------------------------------------------------------------------\n" ,
LNum , X , Y );
	  Terminate ( ERR );
	}

      // turn off all blasts and bullets from the old level
      for (i = 0; i < MAXBLASTS; i++)
	AllBlasts[i].type = OUT;
      for (i = 0; i < MAXBULLETS; i++)
	{
	  DeleteBullet ( i , FALSE ); // Don't ever delete bullets any other way!!! SEGFAULTS might result!!!
	                              // in this case, we need no bullet-explosions
	    //AllBullets[i].type = OUT;
	    //AllBullets[i].mine = FALSE;
	}
      
      // clear the automapping information
      // ClearAutomapData();
    }
  else
    {
      //--------------------
      // If no real level change has occured, everything
      // is simple and we just need to set the new coordinates, haha
      //
      Me [ PlayerNum ] . pos . x = X ;
      Me [ PlayerNum ] . pos . y = Y ;
    }

  
  Me [ PlayerNum ] . mouse_move_target . x = ( -1 ) ;
  Me [ PlayerNum ] . mouse_move_target . y = ( -1 ) ;
  Me [ PlayerNum ] . mouse_move_target . z = ( -1 ) ;

  LeaveLiftSound ();

  //--------------------
  // Perhaps the player is visiting this level for the first time.  Then, the
  // tux should make it's initial statement about the location, if there is one.
  //
  if ( ! Me [ PlayerNum ] . HaveBeenToLevel [ CurLevel->levelnum ] )
    {
      PlayLevelCommentSound ( CurLevel->levelnum );
      Me [ PlayerNum ] . HaveBeenToLevel [ CurLevel->levelnum ] = TRUE;
      if ( array_num != 0 ) ShuffleEnemys ( array_num );
    }

  // UnfadeLevel ();

  SwitchBackgroundMusicTo( CurLevel->Background_Song_Name );

  //--------------------
  // Since we've mightily changed position now, we should clear the
  // position history, so that noone get's confused...
  //
  InitInfluPositionHistory ( PlayerNum );

}; // void Teleport( ... ) 

/* ----------------------------------------------------------------------
 * I'd like to use the function strnlen in LoadSettings, but this would
 * be a gnu extension. so I have to write this function myselt in order
 * to keep the code completely portable.
 * ---------------------------------------------------------------------- */
int
mystrnlen ( char* MyString , int MyMaxlen )
{
  int i;

  for ( i = 0 ; i < MyMaxlen ; i ++ )
    {
      if ( MyString [ i ] == 0 ) return i;
    }
  return MyMaxlen;
}; // int mystrnlen ( char* MyString , int MyMaxlen )

/*----------------------------------------------------------------------
 * LoadGameConfig(): load saved options from config-file
 *
 * this should be the first of all load/save functions called
 * as here we read the $HOME-dir and create the config-subdir if neccessary
 *
 *----------------------------------------------------------------------*/
int
LoadGameConfig (void)
{
  char fname[255];
  FILE *config;
  
  struct stat statbuf;

  // first we need the user's homedir for loading/saving stuff
  if ( (homedir = getenv("HOME")) == NULL )
    DebugPrintf ( 0 , "WARNING: Environment does not contain HOME variable...\n\
Cannot Load or Save settings.\n");

  sprintf (ConfigDir, "%s/.freedroid_rpg", homedir);
  
  if (stat(ConfigDir, &statbuf) == -1) 
    {
      DebugPrintf ( 0 , "\n----------------------------------------------------------------------\n\
You seem not to have the directory %s in your home directory.\n\
This directory is used by freedroid to store saved games and your personal settings.\n\
So I'll try to create it now...\n\
----------------------------------------------------------------------\n", ConfigDir);
      if (mkdir (ConfigDir, S_IREAD|S_IWRITE|S_IEXEC) == -1)
	{
	  DebugPrintf ( 0 , "\n----------------------------------------------------------------------\n\
WARNING: Failed to create config-dir: %s. Giving up...\n\
You settings will not be loaded but the default values will be used instead...\n\
----------------------------------------------------------------------\n", ConfigDir);
	  return (ERR);
	}
      else
	{
	  // --------------------
	  // Since we've just created the config dir, there CANT be any useful
	  // config information in there yet.  So we can do nothing sensible but return here.
	  //
	  DebugPrintf ( 1 , "ok\n" );
	  return (OK); 
	}
    }

  sprintf (fname, "%s/config", ConfigDir);
  if( (config = fopen (fname, "r")) == NULL)
    {
      DebugPrintf (0, "WARNING: failed to open config-file: %s\n");
      return (ERR);
    }
  
  //--------------------
  // Now read the actual data
  // ok, this is neither very portable nor very flexible, we just want that working...
  fread ( &(GameConfig), sizeof (configuration_for_freedroid), sizeof(char), config);
  fclose (config);

  //--------------------
  // Now we do some extra security check:  Maybe the old settings file
  // was generated by a different version of freedroid.  Then extra care
  // must be taken:  The misinterpretation of values could case segmentation
  // faults and things like that.
  //
  // For this purpose I'd like to use the function 'strnlen' but this would be 
  // a gnu extension and not standard C, so I have to use the 'mystrlen' function
  // taken from the gnu c manual.
  //
  if ( ( mystrnlen ( GameConfig.freedroid_version_string , 100 ) != strlen ( VERSION ) ) ||
       ( memcmp ( GameConfig.freedroid_version_string , VERSION , strlen ( VERSION ) ) ) )
    {
      DebugPrintf ( 0 , "\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
Settings file %s does not\n\
seem to be from the same version a this installation of freedroid.\n\
This is perfectly normal if you have just upgraded your version of\n\
freedroid.  But the loading of your settings will be cancelled now,\n\
cause the format of the settings file is no longer supported.  \n\
No need to panic.  The default settings will be used instead and a new\n\
settings file will be generated.\n\
\n\
However, if the problem continues, please inform the freedroid developers\n\
about it, as always, best send e-mail to freedroid-discussion@lists.sourceforge.net.\n\
----------------------------------------------------------------------\n" , fname );
      Reset_GameConfig_To_Default_Values (  );
      return (ERR);
    };

  //--------------------
  // We may print out that config was loaded successfully...
  // This might stay in here.
  //
  DebugPrintf ( 0 , "\nSuccessfully loaded and enforced your personal configuration file %s.\n\n" , fname );

  return (OK);

}; // int LoadGameConfig ( void )
    
/*----------------------------------------------------------------------
 * SaveGameConfig: do just that
 *
 *----------------------------------------------------------------------*/
int
SaveGameConfig (void)
{
  char fname[255];
  FILE *config;
  
  if ( ConfigDir[0] == '\0')
    return (ERR);
  
  sprintf (fname, "%s/config", ConfigDir);
  if( (config = fopen (fname, "w")) == NULL)
    {
      DebugPrintf (0, "WARNING: failed to create config-file: %s\n");
      return (ERR);
    }
  
  //--------------------
  // We put the current version number of freedroid into the 
  // version number string.  This will be usefull so that later
  // versions of freedroid can identify old config files and decide
  // not to use them in some cases.
  //
  strcpy ( GameConfig.freedroid_version_string , VERSION );
  
  //--------------------
  // Now write the actual data
  fwrite ( &(GameConfig), sizeof (configuration_for_freedroid), sizeof(char), config);
  fclose (config);

  return (OK);
  
}; // int SaveGameConfig ( void )


/* ----------------------------------------------------------------------
 * This function is used for terminating freedroid.  It will close
 * the SDL submodules and exit.
 * ---------------------------------------------------------------------- */
void
Terminate (int ExitCode)
{
  DebugPrintf (2, "\nvoid Terminate(int ExitStatus) was called....");
  printf("\n----------------------------------------------------------------------");
  printf("\nTermination of Freedroid initiated...");

  // SaveSettings();
  SaveGameConfig();

  if ( ServerMode )
    {
      DebugPrintf ( 0 , "\n\
--------------------\n\
Closing all players connections to this server...\n\
--------------------\n");
      DisconnectAllRemoteClinets ( ) ;
    }

  // printf("\nUnallocation all resouces...");

  // free the allocated surfaces...
  // SDL_FreeSurface( static_blocks );

  // free the mixer channels...
  // Mix_CloseAudio();

  // printf("\nAnd now the final step...\n\n");
  printf("Thank you for playing Freedroid.\n\n");
  SDL_Quit();
  exit (ExitCode);
  return;
}; // void Terminate ( int ExitCode )

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
 * Since numbers are not so very telling and can easily get confusing
 * we do not use numbers to reference the action from a trigger but 
 * rather we use labels already in the mission file.  However internally
 * the game needs numbers as a pointer or index in a list and therefore
 * this functions was added to go from a label to the corresponding 
 * number entry.
 * ---------------------------------------------------------------------- */
int
GiveNumberToThisActionLabel ( char* ActionLabel )
{
  int i;

  // DebugPrintf( 1 , "\nvoid ExecuteEvent ( int EventNumber ) : real function call confirmed. ");
  // DebugPrintf( 1 , "\nvoid ExecuteEvent ( int EventNumber ) : executing event labeld : %s" , ActionLabel );

  // In case of 'none' as action label, we don't do anything and return;
  if ( strcmp ( ActionLabel , "none" ) == 0 ) return ( -1 );

  //--------------------
  // Now we find out which index the desired action has
  //
  for ( i = 0 ; i < MAX_TRIGGERED_ACTIONS_IN_GAME ; i++ )
    {
      if ( strcmp ( AllTriggeredActions[ i ].ActionLabel , ActionLabel ) == 0 ) break;
    }

  if ( i >= MAX_TRIGGERED_ACTIONS_IN_GAME )
    {
      fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
In function 'void GiveNumberToThisActionLabel ( char* ActionLabel ):\n\
\n\
The label that should reference an action for later execution could not\n\
be identified as valid reference to an existing action.\n\
\n\
The label, that caused this problem was: %s\n\
\n\
Please check that your external text files are properly set up.\n\
\n\
Please also don't forget, that you might have to run 'make install'\n\
again after you've made modifications to the data files in the source tree.\n\
\n\
Freedroid will terminate now to draw attention to the data problem it could\n\
not resolve.... Sorry, if that interrupts a major game of yours.....\n\
----------------------------------------------------------------------\n\
\n" , ActionLabel );
      Terminate(ERR);
    }

  return ( i );
}; // int GiveNumberToThisActionLabel ( char* ActionLabel )

/* ----------------------------------------------------------------------
 * This function executes an action with a label.
 * ---------------------------------------------------------------------- */
void 
ExecuteActionWithLabel ( char* ActionLabel , int PlayerNum )
{
  ExecuteEvent( GiveNumberToThisActionLabel ( ActionLabel ) , PlayerNum );
}; // void ExecuteActionWithLabel ( char* ActionLabel )

/* ----------------------------------------------------------------------
 * 
 * ---------------------------------------------------------------------- */
void 
ExecuteEvent ( int EventNumber , int PlayerNum )
{
  // DebugPrintf( 1 , "\nvoid ExecuteEvent ( int EventNumber ) : real function call confirmed. ");
  // DebugPrintf( 1 , "\nvoid ExecuteEvent ( int EventNumber ) : executing event Nr.: %d." , EventNumber );

  // Do nothing in case of the empty action (-1) given.
  if ( EventNumber == (-1) ) return;

  // Does the action include a change of a map tile?
  if ( AllTriggeredActions[ EventNumber ].ChangeMapTo != -1 )
    {
      //YES.  So we need to check, if the location has been supplied fully first.
      if ( ( AllTriggeredActions[ EventNumber ].ChangeMapLocation.x == (-1) ) ||
	   ( AllTriggeredActions[ EventNumber ].ChangeMapLocation.y == (-1) ) ||
	   ( AllTriggeredActions[ EventNumber ].ChangeMapLevel == (-1) ) )
	{
	  DebugPrintf( 0 , "\n\nSorry! There has been a corrupt event specification!\n\nTerminating...\n\n");
	  Terminate(ERR);
	}
      else
	{
	  DebugPrintf( 1 , "\nvoid ExecuteEvent ( int EventNumber ) : Change map Event correctly specified. confirmed.");
	  curShip.AllLevels[ AllTriggeredActions[ EventNumber ].ChangeMapLevel ]->map [ AllTriggeredActions[ EventNumber ].ChangeMapLocation.y ] [ AllTriggeredActions[ EventNumber ].ChangeMapLocation.x ]  = AllTriggeredActions[ EventNumber ].ChangeMapTo ;
	  GetDoors( curShip.AllLevels[ AllTriggeredActions[ EventNumber ].ChangeMapLevel ]  );
	}
    }

  // Does the action include a teleport of the influencer to some other location?
  if ( AllTriggeredActions[ EventNumber ].TeleportTarget.x != (-1) )
    {
      Teleport ( AllTriggeredActions[ EventNumber ].TeleportTargetLevel ,
		 AllTriggeredActions[ EventNumber ].TeleportTarget.x ,
		 AllTriggeredActions[ EventNumber ].TeleportTarget.y ,
		 PlayerNum , FALSE );
    }

  // Does the defined action assign the influencer a mission?
  if ( AllTriggeredActions[ EventNumber ].AssignWhichMission != (-1) )
    {
      AssignMission( AllTriggeredActions[ EventNumber ].AssignWhichMission );
    }

  // Does the defined action make the influencer say something?
  if ( strlen ( AllTriggeredActions[ EventNumber ].InfluencerSayText ) > 0 )
    {
      //YES. So we need to output his sentence as usual
      Me[0].TextVisibleTime=0;
      Me[0].TextToBeDisplayed=AllTriggeredActions[ EventNumber ].InfluencerSayText;
    }
}; // void ExecuteEvent ( int EventNumber )

/* ----------------------------------------------------------------------
 *
 * This function checks for triggered events & statements.  Those events are
 * usually entered via the mission file and read into the apropriate
 * structures via the InitNewMission function.  Here we check, whether
 * the nescessary conditions for an event are satisfied, and in case that
 * they are, we order the apropriate event to be executed.
 *
 * In addition, statements are started, if the influencer is at the 
 * right location for them.
 *
 * ---------------------------------------------------------------------- */
void 
CheckForTriggeredEventsAndStatements ( int PlayerNum )
{
  int i;
  int map_x, map_y;

  Level StatementLevel = curShip.AllLevels[ Me [ PlayerNum ] . pos . z ] ;

  //--------------------
  // Now we check if some statment location is reached
  //
  map_x = (int) rintf( Me [ PlayerNum ] . pos . x ); map_y = (int) rintf( Me [ PlayerNum ] . pos . y ) ;
  for ( i = 0 ; i < MAX_STATEMENTS_PER_LEVEL ; i++ )
    {
      if ( ( map_x == StatementLevel -> StatementList [ i ] . x ) &&
	   ( map_y == StatementLevel -> StatementList [ i ] . y ) )
	{
	  Me [ PlayerNum ] . TextVisibleTime = 0 ;
	  Me [ PlayerNum ] . TextToBeDisplayed = CurLevel -> StatementList [ i ] . Statement_Text ;
	}
    }

  //--------------------
  // Now we check if some event trigger is fullfilled.
  //
  for ( i=0 ; i<MAX_EVENT_TRIGGERS ; i++ )
    {
      // if ( AllEventTriggers[i].EventNumber == (-1) ) continue;  // thats a sure sign this event doesn't need attention
      if ( strcmp (AllEventTriggers[i].TargetActionLabel , "none" ) == 0 ) continue;  // thats a sure sign this event doesn't need attention

      // --------------------
      // So at this point we know, that the event trigger is somehow meaningful. 
      // Fine, so lets check the details, if the event is triggered now
      //

      if ( AllEventTriggers[i].Influ_Must_Be_At_Point.x != (-1) )
	{
	  if ( rintf( AllEventTriggers[i].Influ_Must_Be_At_Point.x ) != rintf( Me [ PlayerNum ] . pos.x ) ) continue;
	}

      if ( AllEventTriggers[i].Influ_Must_Be_At_Point.y != (-1) )
	{
	  if ( rintf( AllEventTriggers[i].Influ_Must_Be_At_Point.y ) != rintf( Me [ PlayerNum ] . pos.y ) ) continue;
	}

      if ( AllEventTriggers[i].Influ_Must_Be_At_Level != (-1) )
	{
	  if ( rintf( AllEventTriggers[i].Influ_Must_Be_At_Level ) != StatementLevel->levelnum ) continue;
	}

      // printf("\nWARNING!! INFLU NOW IS AT SOME TRIGGER POINT OF SOME LOCATION-TRIGGERED EVENT!!!");
      // ExecuteEvent( AllEventTriggers[i].EventNumber );
      ExecuteActionWithLabel ( AllEventTriggers [ i ] . TargetActionLabel , PlayerNum ) ;

      if ( AllEventTriggers[i].DeleteTriggerAfterExecution == 1 )
	{
	  // AllEventTriggers[i].EventNumber = (-1); // That should prevent the event from being executed again.
	  AllEventTriggers[i].TargetActionLabel = "none"; // That should prevent the event from being executed again.
	}
    }

}; // CheckForTriggeredEventsAndStatements (void )

#undef _misc_c
