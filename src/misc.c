/*----------------------------------------------------------------------
 *
 * Desc: miscellaeous helpful functions for Freedroid
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
#define _misc_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"

// these are needed for stat()
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

// The definition of the message structure can stay here,
// because its only needed in this module.
typedef struct
{
  void *NextMessage;
  int MessageCreated;
  char *MessageText;
}
message, Message;
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

char *homedir = NULL;
char ConfigDir[255]="\0";

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

// ------------------------------------------------------------
// just a plain old sign-function
// ------------------------------------------------------------
int sign (float x)
{
  if (x == 0.0)
    return (0);
  else if (x > 0.0)
    return (1);
  else 
    return (-1);
}

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

  sprintf (ConfigDir, "%s/.freedroidClassic", homedir);
  
  if (stat(ConfigDir, &statbuf) == -1) {
    DebugPrintf (1, "Couldn't stat Config-dir %s, I'll try to create it...", ConfigDir);
    if (mkdir (ConfigDir, S_IREAD|S_IWRITE|S_IEXEC) == -1)
      {
	DebugPrintf (0, "WARNING: Failed to create config-dir: %s. Giving up...\n", ConfigDir);
	return (ERR);
      }
    else
      {
	DebugPrintf (1, "ok\n");
	return (OK);
      }
  }

  sprintf (fname, "%s/config", ConfigDir);
  if( (config = fopen (fname, "r")) == NULL)
    {
      DebugPrintf (0, "WARNING: failed to open config-file: %s\n");
      return (ERR);
    }
  
  // Now read the actual data
  // ok, this is neither very portable nor very flexible, we just want that working...
  fread ( &(GameConfig), sizeof (configuration_for_freedroid), sizeof(char), config);

  fclose (config);

  return (OK);

} // LoadGameConfig
    
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
  
  // Now write the actual data
  fwrite ( &(GameConfig), sizeof (configuration_for_freedroid), sizeof(char), config);

  fclose (config);
  return (OK);
  
} // SaveGameConfig()


/*----------------------------------------------------------------------
This function looks for a sting begin indicator and takes the string
from after there up to a sting end indicator and mallocs memory for
it, copys it there and returns it.
The original source string specified should in no way be modified.
----------------------------------------------------------------------*/
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
};


/*
----------------------------------------------------------------------
This function counts the number of occurences of a string in a given
other string.
----------------------------------------------------------------------
*/
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

/*
----------------------------------------------------------------------
This function read in a file with the specified name, allocated 
memory for it of course, looks for the file end string and then
terminates the whole read in file with a 0 character, so that it
can easily be treated like a common string.
----------------------------------------------------------------------
*/
char* 
ReadAndMallocAndTerminateFile( char* filename , char* File_End_String ) 
{
  struct stat stbuf;
  FILE *DataFile;
  char *Data;
  char *ReadPointer;
  // char *fpath;

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

  if ((Data = (char *) MyMalloc (stbuf.st_size + 64*2 + 10000 )) == NULL)
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

  if ( (ReadPointer = strstr( Data , File_End_String ) ) == NULL )
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

/*
----------------------------------------------------------------------
This function tries to locate a string in some given data string.
The data string is assumed to be null terminated.  Otherwise SEGFAULTS
might happen.

The return value is a pointer to the first instance where the substring
we are searching is found in the main text.
----------------------------------------------------------------------
*/
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

};


/*
----------------------------------------------------------------------
This function should analyze a given passage of text, locate an 
indicator for a value, and read in the value.

----------------------------------------------------------------------
*/
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
};

/*-----------------------------------------------------------------
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
 * if file is not found, return NULL pointer
 *-----------------------------------------------------------------*/
char *
find_file (char *fname, char *subdir, int use_theme)
{
  static char File_Path[5000];   /* hope this will be enough */
  FILE *fp;  // this is the file we want to find?
  int i;
  bool found = FALSE;

  if (!fname)
    {
      printf ("\nError. find_file() called with empty filename!\n");
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
	  found = TRUE;
	  break;
	}
    } /* for i */

  if (found)
    return (File_Path);
  else
    return (NULL);
	
} /* find_file */

/*@Function============================================================
@Desc: realise Pause-Mode: the game process is halted,
       while the graphics and animations are not.  This mode 
       can further be toggled from PAUSE to CHEESE, which is
       a feature from the original program that should probably
       allow for better screenshots.
       
@Ret: 
* $Function----------------------------------------------------------*/
void
Pause (void)
{
  int Pause = TRUE;
  int Cheese = FALSE;

  Me.status = PAUSE;
  Assemble_Combat_Picture ( DO_SCREEN_UPDATE );

  while ( Pause )
    {
      StartTakingTimeForFPSCalculation();

      if (!Cheese)
	{
	  AnimateInfluence ();
	  AnimateRefresh ();
	  AnimateEnemys ();
	}
      
      DisplayBanner (NULL, NULL, 0);
      Assemble_Combat_Picture ( DO_SCREEN_UPDATE );

      usleep (50);

      ComputeFPSForThisFrame();

      if (CPressed ())
	{
	  while (CPressed());
	  if (Me.status != CHEESE) Me.status = CHEESE;
	  else Me.status = PAUSE;
	  Cheese = !Cheese;

	} /* if (CPressed) */

      if ( SpacePressed() )
	{
	  Pause = FALSE;
	  while ( SpacePressed() );  /* wait for release */
	}

    } /* while (Pause) */

  return;
} // Pause () 


/*@Function============================================================
@Desc: This function starts the time-taking process.  Later the results
       of this function will be used to calculate the current framerate

       Two methods of time-taking are available.  One uses the SDL 
       ticks.  This seems LESS ACCURATE.  The other one uses the
       standard ansi c gettimeofday functions and are MORE ACCURATE
       but less convenient to use.
@Ret: 
* $Function----------------------------------------------------------*/
void 
StartTakingTimeForFPSCalculation(void)
{
  /* This ensures, that 0 is never an encountered framenr,
   * therefore count to 100 here
   * Take the time now for calculating the frame rate
   * (DO NOT MOVE THIS COMMAND PLEASE!) */
  framenr++;
  
  One_Frame_SDL_Ticks = SDL_GetTicks();
  if (framenr % 10 == 1)
    Ten_Frame_SDL_Ticks = SDL_GetTicks();
  if (framenr % 100 == 1)
    {
      Onehundred_Frame_SDL_Ticks=SDL_GetTicks();
      // printf("\n%f",1/Frame_Time());
      // printf("Me.pos.x: %g Me.pos.y: %g Me.speed.x: %g Me.speed.y: %g \n",
      //Me.pos.x, Me.pos.y, Me.speed.x, Me.speed.y );
      //printf("Me.maxspeed.x: %g \n",
      //	     Druidmap[Me.type].maxspeed );
    }
  
} // void StartTakingTimeForFPSCalculation(void)


/*@Function============================================================
@Desc: This function computes the framerate that has been experienced
       in this frame.  It will be used to correctly calibrate all 
       movements of game objects.

       NOTE:  To query the actual framerate a DIFFERENT function must
       be used, namely Frame_Time().

       Two methods of time-taking are available.  One uses the SDL 
       ticks.  This seems LESS ACCURATE.  The other one uses the
       standard ansi c gettimeofday functions and are MORE ACCURATE
       but less convenient to use.
@Ret: 
* $Function----------------------------------------------------------*/
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

  if (SkipAFewFrames) return;

  Now_SDL_Ticks=SDL_GetTicks();
  oneframedelay=Now_SDL_Ticks-One_Frame_SDL_Ticks;
  tenframedelay=Now_SDL_Ticks-Ten_Frame_SDL_Ticks;
  onehundredframedelay=Now_SDL_Ticks-Onehundred_Frame_SDL_Ticks;
  
  FPSover1 = 1000 * 1 / (float) oneframedelay;
  FPSover10 = 1000 * 10 / (float) tenframedelay;
  FPSover100 = 1000 * 100 / (float) onehundredframedelay;
  
} // void ComputeFPSForThisFrame(void)

/*@Function============================================================
  @Desc: 

 * This function is the key to independence of the framerate for various game elements.
 * It returns the average time needed to draw one frame.
 * Other functions use this to calculate new positions of moving objects, etc..
 *

 * Also there is of course a serious problem when some interuption occurs, like e.g.
 * the options menu is called or the debug menu is called or the console or the elevator
 * is entered or a takeover game takes place.  This might cause HUGE framerates, that could
 * box the influencer out of the ship if used to calculate the new position.

 * To counter unwanted effects after such events we have the SkipAFewFramerates counter,
 * which instructs Rate_To_Be_Returned to return only the overall default framerate since
 * no better substitute exists at this moment.  But on the other hand, this seems to
 * work REALLY well this way.

 * This counter is most conveniently set via the function Activate_Conservative_Frame_Computation,
 * which can be conveniently called from eveywhere.

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
float
Frame_Time (void)
{
  static float previous_time;
  
  if ( SkipAFewFrames ) 
    return previous_time;

  previous_time = (1.0 / FPSover1);

  return (previous_time);

} // float Frame_Time(void)


/*@Function============================================================
@Desc: 

 * With framerate computation, there is a problem when some interuption occurs, like e.g.
 * the options menu is called or the debug menu is called or the console or the elevator
 * is entered or a takeover game takes place.  This might cause HUGE framerates, that could
 * box the influencer out of the ship if used to calculate the new position.

 * To counter unwanted effects after such events we have the SkipAFewFramerates counter,
 * which instructs Rate_To_Be_Returned to return only the overall default framerate since
 * no better substitute exists at this moment.

 * This counter is most conveniently set via the function Activate_Conservative_Frame_Computation,
 * which can be conveniently called from eveywhere.

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void 
Activate_Conservative_Frame_Computation(void)
{
  SkipAFewFrames=TRUE;

  // Now we are in some form of pause.  It can't
  // hurt to have the top status bar redrawn after that,
  // so we set this variable...
  BannerIsDestroyed=TRUE;

} // void Activate_Conservative_Frame_Computation(void)


/*@Function============================================================
@Desc: This function is used for debugging purposes.  It writes the
       given string either into a file, on the screen, or simply does
       nothing according to currently set debug level.

@Ret: none
* $Function----------------------------------------------------------*/
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
}

/*@Function============================================================
@Desc: This function is used to generate an integer in range of all
       numbers from 0 to UpperBound.

@Ret:  the generated integer
* $Function----------------------------------------------------------*/
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
} /* MyRandom () */


/*@Function============================================================
@Desc: This function is kills all enemy robots on the whole ship.
       It querys the user once for safety.

@Ret:  none
* $Function----------------------------------------------------------*/
void
Armageddon (void)
{
  int i;

  for (i = 0; i < NumEnemys; i++)
    {
      AllEnemys[i].energy = 0;
      AllEnemys[i].status = OUT;
    }
} // void Armageddon(void)

/*@Function============================================================
@Desc: This function teleports the influencer to a new position on the
       ship.  THIS CAN BE A POSITION ON A DIFFERENT LEVEL.

@Ret:  none
* $Function----------------------------------------------------------*/
void
Teleport (int LNum, int X, int Y)
{
  int curLevel = LNum;
  int array_num = 0;
  Level tmp;
  int i;

  if (curLevel != CurLevel->levelnum)
    {	

      //--------------------
      // In case a real level change has happend,
      // we need to do a lot of work:

      while ((tmp = curShip.AllLevels[array_num]) != NULL)
	{
	  if (tmp->levelnum == curLevel)
	    break;
	  else
	    array_num++;
	}

      CurLevel = curShip.AllLevels[array_num];

      ShuffleEnemys ();

      Me.pos.x = X;
      Me.pos.y = Y;

      // turn off all blasts and bullets from the old level
      for (i = 0; i < MAXBLASTS; i++)
	AllBlasts[i].type = OUT;
      for (i = 0; i < MAXBULLETS; i++)
	{
	  DeleteBullet ( i ); // Don't ever delete bullets any other way!!! SEGFAULTS might result!!!
	    //AllBullets[i].type = OUT;
	    //AllBullets[i].mine = FALSE;
	}
    }
  else
    {
      //--------------------
      // If no real level change has occured, everything
      // is simple and we just need to set the new coordinates, haha
      //
      Me.pos.x = X;
      Me.pos.y = Y;
    }

  LeaveLiftSound ();

  // UnfadeLevel ();

} /* Teleport() */


/*@Function============================================================
@Desc: 	This function is used for terminating freedroid.  It will close
        the SDL submodules and exit.

@Ret: 
* $Function----------------------------------------------------------*/
void
Terminate (int ExitCode)
{
  printf("\n----------------------------------------------------------------------");
  printf("\nTermination of Freedroid initiated...");

  if (ExitCode == OK)
    {
      DebugPrintf (2, "Writing config file\n");
      SaveGameConfig ();
      DebugPrintf (2, "Writing highscores to disk\n");
      SaveHighscores ();
    }

  printf("Thank you for playing Freedroid.\n\n");
  SDL_Quit();
  exit (ExitCode);
  return;
}  // void Terminate(int ExitCode)


/*@Function============================================================
@Desc: This functin deletes the currently displayed message and
       advances to the next message.

@Ret: none
* $Function----------------------------------------------------------*/
void
AdvanceQueue (void)
{
  message *tmp;

  DebugPrintf (2, "\nvoid AdvanceQueue(void): Funktion wurde echt aufgerufen.");

  if (Queue == NULL)
    return;

  if (Queue->MessageText)
    free (Queue->MessageText);
  tmp = Queue;

  Queue = Queue->NextMessage;

  free (tmp);

  DebugPrintf (2, "\nvoid AdvanceQueue(void): Funktion hat ihr natuerliches Ende erfolgreich erreicht....");
} // void AdvanceQueue(void)


/*@Function============================================================
@Desc: This function works a malloc, except that it also checks for
       success and terminates in case of "out of memory", so we dont
       need to do this always in the code.

@Ret: 
* $Function----------------------------------------------------------*/
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
}				// void* MyMalloc(long Mamount)

#undef _misc_c
