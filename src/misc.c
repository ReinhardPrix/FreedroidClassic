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

/*
----------------------------------------------------------------------
This function looks for a sting begin indicator and takes the string
from after there up to a sting end indicator and mallocs memory for
it, copys it there and returns it.
The original source string specified should in no way be modified.
----------------------------------------------------------------------
*/
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
 *-----------------------------------------------------------------*/
char *
find_file (char *fname, char *subdir, int use_theme)
{
  static char File_Path[5000];   /* hope this will be enough */
  FILE *fp;  // this is the file we want to find?
  int i;

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
	  break;
	}
    } /* for i */


  return (File_Path);
	
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

  Activate_Conservative_Frame_Computation();

  Me.status = PAUSE;
  Assemble_Combat_Picture ( DO_SCREEN_UPDATE );

  while ( Pause )
    {
      // usleep(10);
      AnimateInfluence ();
      AnimateRefresh ();
      AnimateEnemys ();
      DisplayBanner (NULL, NULL, 0);
      Assemble_Combat_Picture ( DO_SCREEN_UPDATE );
      
      if (CPressed ())
	{
	  Me.status = CHEESE;
	  DisplayBanner (NULL, NULL,  0 );
	  Assemble_Combat_Picture ( DO_SCREEN_UPDATE );

	  while (!SpacePressed ()); /* stay CHEESE until Space pressed */
	  while ( SpacePressed() ); /* then wait for Space released */
	  
	  Me.status = PAUSE;       /* return to normal PAUSE */
	} /* if (CPressed) */

      if ( SpacePressed() )
	{
	  Pause = FALSE;
	  while ( SpacePressed() );  /* wait for release */
	}

    } /* while (Pause) */
  return;
}; // Pause () 


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
  
#ifdef USE_SDL_FRAMERATE
  One_Frame_SDL_Ticks=SDL_GetTicks();
  if (framenr % 10 == 1)
    Ten_Frame_SDL_Ticks=SDL_GetTicks();
  if (framenr % 100 == 1)
    {
      Onehundred_Frame_SDL_Ticks=SDL_GetTicks();
      // printf("\n%f",1/Frame_Time());
      // printf("Me.pos.x: %g Me.pos.y: %g Me.speed.x: %g Me.speed.y: %g \n",
      //Me.pos.x, Me.pos.y, Me.speed.x, Me.speed.y );
      //printf("Me.maxspeed.x: %g \n",
      //	     Druidmap[Me.type].maxspeed );
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
  float Rate_To_Be_Returned;
  
  if ( SkipAFewFrames ) 
    {
      Rate_To_Be_Returned = Overall_Average;
      return Rate_To_Be_Returned;
    }

  Rate_To_Be_Returned = (1.0 / FPSover1);

  return Rate_To_Be_Returned;

} // float Frame_Time(void)

int
Get_Average_FPS ( void )
{
  return ( (int) ( 1.0 / Overall_Average ) );
}; // int Get_Average_FPS( void )

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
  // SkipAFewFrames=212;
  // SkipAFewFrames=22;
  SkipAFewFrames=3;

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
	AllEnemys[i].energy = 0;
	AllEnemys[i].Status = OUT;
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
	  DeleteBullet ( i , FALSE ); // Don't ever delete bullets any other way!!! SEGFAULTS might result!!!
	                              // in this case, we need no bullet-explosions
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
@Desc: This is a test function for InsertMessage()

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
InsertNewMessage (void)
{
  static int counter = 0;
  char testmessage[100];

  DebugPrintf (2, "\nvoid InsertNewMessage(void): real function call confirmed...");

  counter++;
  sprintf (testmessage, "Das ist die %d .te Message !!", counter);
  InsertMessage (testmessage);

  DebugPrintf (2, "\nvoid InsertNewMessage(void): end of function reached...");
  return;
}				// void InsertNewMessage(void)

/*@Function============================================================
@Desc: 	This function is used for terminating freedroid.  It will close
        the SDL submodules and exit.

@Ret: 
* $Function----------------------------------------------------------*/
void
Terminate (int ExitCode)
{
  DebugPrintf (2, "\nvoid Terminate(int ExitStatus) was called....");
  printf("\n----------------------------------------------------------------------");
  printf("\nTermination of Freedroid initiated...");
  // printf("\nUnallocation all resouces...");

  // free the allocated surfaces...
  // SDL_FreeSurface( ne_blocks );
  // SDL_FreeSurface( ne_static );

  // free the mixer channels...
  // Mix_CloseAudio();

  // printf("\nAnd now the final step...\n\n");
  printf("Thank you for playing Freedroid.\n\n");
  SDL_Quit();
  exit (ExitCode);
  return;
}  // void Terminate(int ExitCode)


/*@Function============================================================
@Desc: This function empties the message queue of messages to be
       displayed in a moving font on screen.

@Ret: none
* $Function----------------------------------------------------------*/
void
KillQueue (void)
{
  while (Queue)
    AdvanceQueue ();
}

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
@Desc: This function should put a message from the queue to the scren.
       It surely does not work now and it also needs not work now, 
       since this is a feature not incorporated into the original game
       from the C64 and therefore has less priority.

@Ret: 
* $Function----------------------------------------------------------*/
void
PutMessages (void)
{
  static int MesPos = 0;	// X-position of the message bar 
  static int Working = FALSE;	// is a message beeing processed? 
  message *LQueue;		// mobile queue pointer
  int i;

  if (!PlusExtentionsOn)
    return;

  DebugPrintf (2, "\nvoid PutMessages(void): Funktion wurde echt aufgerufen.");

  if (!Queue)
    return;			// nothing to be done
  if (!Working)
    ThisMessageTime = 0;	// inactive, but Queue->reset time

  printf ("Time: %d", ThisMessageTime);

// display the current list:

  LQueue = Queue;
  i = 0;
  DebugPrintf (2, "\nvoid PutMessages(void): This is the Queue of Messages:\n");
  while (LQueue != NULL)
    {
      if ((LQueue->MessageText) == NULL)
	{
	  DebugPrintf (2, "\nvoid PutMessages(void): ERROR: Textpointer is NULL !!!!!!\n");
	  Terminate(ERR);
	}
      printf ("%d. '%s' %d\n", i, LQueue->MessageText,
	      LQueue->MessageCreated);
      i++;
      LQueue = LQueue->NextMessage;
    }
  DebugPrintf (2, " NULL reached !\n");

  // if the message is very old, it can be deleted...
  if (Working && (ThisMessageTime > MaxMessageTime))
    {
      AdvanceQueue ();
      //      CleanMessageLine ();
      Working = FALSE;		// inactive
      ThisMessageTime = 0;	// Counter init.
      return;
    }


  // old message has lived for MinTime, new one is waiting
  if ((ThisMessageTime > MinMessageTime) && (Queue->NextMessage))
    {
      AdvanceQueue ();		/* Queue weiterbewegen */
      Working = FALSE;		/* inaktiv setzen */
      ThisMessageTime = 0;	/* counter neu init. */
      return;
    }

  // function currenlty inactive and new message waiting --> activate it
  if ((!Working) && Queue)
    {

      // if message not yet generated, generate it
      if (!Queue->MessageCreated)
	{
	  CreateMessageBar (Queue->MessageText);
	  Queue->MessageCreated = TRUE;
	}

      ThisMessageTime = 0;	/* initialize counter  */
      //      CleanMessageLine ();	/* delete line */
      Working = TRUE;		/* activated */
    }

  // function currently inactive --> move and display
  if (Working && Queue)
    {

      MesPos = 10 * ThisMessageTime;	/* move synchronized this time */

      /* don't go beyond the left border!! */
      if (MesPos > (MESBARBREITE - 2))
	MesPos = MESBARBREITE - 2;

      for (i = 0; i < MESHOEHE; i++)
	{
	  ;
	}
    }	/* if aktiv + Message there */
}	/* Put Messages */


/*@Function============================================================
@Desc: This function prepares the graphics for the next message to
       be displayed

@Ret: 
* $Function----------------------------------------------------------*/
void
CreateMessageBar (char *MText)
{
  char Worktext[42];
  int i, j;

  DebugPrintf (2, "\nvoid CreateMessageBar(char* MText): real function call confirmed.");

  // check for too long message
  if (strlen (MText) > 40)
    {
      DebugPrintf (2, "\nvoid CreateMessageBar(char* MText): Message hat mehr als 40 Zeichen !.\n");
      Terminate (ERR);
    }

  // allocate memory if this hasn't happened yet
  if (MessageBar == NULL)
    if ((MessageBar = MyMalloc (MESBAR_MEM)) == NULL)
      {
	DebugPrintf (2, "\nvoid CreateMessageBar(char* MText): Bekomme keinen Speicher fuer MessageBar !!\n");
	Terminate (ERR);
      }

  // fill in spaces to get 40 chars as message length
  strcpy (Worktext, MText);
  while (strlen (Worktext) < 40)
    strcat (Worktext, " ");

  // display the current message to the internal screen and then cut it out from there
  for (i = 0; i < 40; i++)
    {
      for (j = 0; j < 8; j++)
	{
	  memcpy ((MessageBar + i * 8 + j * SCREENLEN),
		  (Data70Pointer + Worktext[i] * 8 * 8 + j * 8), 8);
	}
    }

  DebugPrintf (2, "\nvoid CreateMessageBar(char* MText): end of function reached.");
} // void CreateMessageBar(char* MText)

/*@Function============================================================
@Desc: This function insers a new message for the user to be 
       displayed into the message queue

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
InsertMessage (char *MText)
{
  message *LQueue = Queue;

  DebugPrintf (2, "\nvoid InsertMessage(char* MText): real function call confirmed...");

  if (LQueue)
    {
      // move to the next free position in the message queue
      while (LQueue->NextMessage != NULL)
	LQueue = LQueue->NextMessage;
      LQueue->NextMessage = MyMalloc (sizeof (message) + 1);
      LQueue = LQueue->NextMessage;
    }
  else
    {
      Queue = MyMalloc (sizeof (message) + 1);
      LQueue = Queue;
    }

  LQueue->MessageText = MyMalloc (MAX_MESSAGE_LEN + 1);
  strcpy (LQueue->MessageText, MText);
  LQueue->NextMessage = NULL;
  LQueue->MessageCreated = FALSE;
} // void InsertMessage(char* MText)

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

/* ----------------------------------------------------------------------
 *
 *
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
In function 'void ExecuteActionWithLabel ( char* ActionLabel ):\n\
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



/*@Function============================================================
@Desc: 

@Ret: 
* $Function----------------------------------------------------------*/
void 
ExecuteActionWithLabel ( char* ActionLabel )
{
  ExecuteEvent( GiveNumberToThisActionLabel ( ActionLabel ) );
}; // void ExecuteActionWithLabel ( char* ActionLabel )

/*@Function============================================================
@Desc: 

@Ret: 
* $Function----------------------------------------------------------*/
void 
ExecuteEvent ( int EventNumber )
{
  // DebugPrintf( 1 , "\nvoid ExecuteEvent ( int EventNumber ) : real function call confirmed. ");
  // DebugPrintf( 1 , "\nvoid ExecuteEvent ( int EventNumber ) : executing event Nr.: %d." , EventNumber );

  // Do nothing in case of the empty action (-1) given.
  if ( EventNumber == (-1) ) return;

  // Does the trigger include a change of a map tile?
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

  // Does the defined action assign the influencer a mission?
  if ( AllTriggeredActions[ EventNumber ].AssignWhichMission != (-1) )
    {
      AssignMission( AllTriggeredActions[ EventNumber ].AssignWhichMission );
    }

  // Does the defined action make the influencer say something?
  if ( strlen ( AllTriggeredActions[ EventNumber ].InfluencerSayText ) > 0 )
    {
      //YES. So we need to output his sentence as usual
      Me.TextVisibleTime=0;
      Me.TextToBeDisplayed=AllTriggeredActions[ EventNumber ].InfluencerSayText;
    }
}; // void ExecuteEvent ( int EventNumber )

/*@Function============================================================
@Desc: This function checks for triggered events.  Those events are
       usually entered via the mission file and read into the apropriate
       structures via the InitNewMission function.  Here we check, whether
       the nescessary conditions for an event are satisfied, and in case that
       they are, we order the apropriate event to be executed.

@Ret: 
* $Function----------------------------------------------------------*/
void 
CheckForTriggeredEvents ( void )
{
  int i;

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
	  if ( rintf( AllEventTriggers[i].Influ_Must_Be_At_Point.x ) != rintf( Me.pos.x ) ) continue;
	}

      if ( AllEventTriggers[i].Influ_Must_Be_At_Point.y != (-1) )
	{
	  if ( rintf( AllEventTriggers[i].Influ_Must_Be_At_Point.y ) != rintf( Me.pos.y ) ) continue;
	}

      if ( AllEventTriggers[i].Influ_Must_Be_At_Level != (-1) )
	{
	  if ( rintf( AllEventTriggers[i].Influ_Must_Be_At_Level ) != CurLevel->levelnum ) continue;
	}

      // printf("\nWARNING!! INFLU NOW IS AT SOME TRIGGER POINT OF SOME LOCATION-TRIGGERED EVENT!!!");
      // ExecuteEvent( AllEventTriggers[i].EventNumber );
      ExecuteActionWithLabel ( AllEventTriggers[i].TargetActionLabel );

      if ( AllEventTriggers[i].DeleteTriggerAfterExecution == 1 )
	{
	  // AllEventTriggers[i].EventNumber = (-1); // That should prevent the event from being executed again.
	  AllEventTriggers[i].TargetActionLabel = "none"; // That should prevent the event from being executed again.
	}
    }

}; // CheckForTriggeredEvents (void )

#undef _misc_c
