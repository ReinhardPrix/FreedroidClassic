/* 
 *
 *   Copyright (c) 1994, 2002, 2003  Johannes Prix
 *   Copyright (c) 1994, 2002, 2003  Reinhard Prix
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

/*----------------------------------------------------------------------
 *
 * Desc: miscellaeous helpful functions for Freedroid
 *	 
 *----------------------------------------------------------------------*/

#define _misc_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"

int read_variable (char *data, char *var_name, char *fmt, void *var);

char *homedir = NULL;
char ConfigDir[255]="\0";

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


// ----------------------------------------------------------------------
// we need our own value-parser for the config-file, as ReadValueFromString()
// terminates game with error-msg if string is not found...
// returns ERR if not found or could not be read, OK if found&read
// ----------------------------------------------------------------------
int
read_variable (char *data, char *var_name, char *fmt, void *var)
{
  char *found = NULL;
  int ret;

  if ( (found = strstr(data, var_name)) == NULL)
    {
      DebugPrintf (1, "WARNING: variable %s was not found!\n", var_name);
      return (ERR);
    }

  found += strlen (var_name);
  
  // skip whitespace,tab, =, : 
  found += strspn(found, " \t=:");

  ret = sscanf (found, fmt, var);
  if ( (ret == 0) && (ret == EOF) )
    {
      DebugPrintf (0, "WARNING: Variable %s was not readable using the format '%s'\n",
		   var_name, fmt);
      return (ERR);
    }

  return (OK);

} // read_variable


// ----------------------------------------------------------------------
// Game-config maker-strings for config-file:

#define VERSION_STRING               "Freedroid Version"
#define DRAW_FRAMERATE               "Draw_Framerate"
#define DRAW_ENERGY                  "Draw_Energy"
#define DRAW_POSITION                "Draw_Position"
#define DRAW_DEATHCOUNT              "Draw_DeathCount"
#define DROID_TALK                   "Droid_Talk"
#define WANTED_TEXT_VISIBLE_TIME     "WantedTextVisibleTime"
#define CURRENT_BG_MUSIC_VOLUME      "Current_BG_Music_Volume"
#define CURRENT_SOUND_FX_VOLUME      "Current_Sound_FX_Volume"
#define CURRENT_GAMMA_CORRECTION     "Current_Gamma_Correction"
#define THEME_NAME                   "Theme_Name"
#define FULL_USER_RECT               "FullUserRect"
#define USE_FULLSCREEN               "UseFullscreen"
#define TAKEOVER_ACTIVATES           "TakeoverActivates"
#define SHOW_DECALS                  "ShowDecals"
#define ALL_MAP_VISIBLE              "AllMapVisible"
#define VID_SCALE_FACTOR             "Vid_ScaleFactor"

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
  FILE *fp;
  char *data;
  off_t size, read_size;
  struct stat statbuf;
  char version_string[100];

  // first we need the user's homedir for loading/saving stuff
  if ( (homedir = getenv("HOME")) == NULL )
    DebugPrintf ( 0 , "WARNING: Environment does not contain HOME variable...\n\
Cannot Load or Save settings.\n");

  sprintf (ConfigDir, "%s/.freedroidClassic", homedir);
  
  if (stat(ConfigDir, &statbuf) == -1) {
    DebugPrintf (1, "Couldn't stat Config-dir %s, I'll try to create it...", ConfigDir);
#if __WIN32__
    _mkdir (ConfigDir);
#else
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
#endif
  }

  sprintf (fname, "%s/config", ConfigDir);
  
  if ( stat (fname, &statbuf) == -1 )
    {
      DebugPrintf (0, "Couldn't stat config-file: %s\n", fname);
      return (ERR);
    }
  size = statbuf.st_size;

  if( (fp = fopen (fname, "r")) == NULL)
    {
      DebugPrintf (0, "WARNING: failed to open config-file: %s\n");
      return (ERR);
    }
  
  // Now read the raw data
  data = MyMalloc (size+10);
  read_size = fread ( data, 1, size, fp);
  data [read_size] = '\0';  // properly terminate as string!

  DebugPrintf (2, "Wanted to read %d bytes, got %d bytes\n", size, read_size);
  if ( read_size != size )
    {
      DebugPrintf (0, "WARNING: error in reading config-file %s\n Giving up...", fname);
      fclose (fp);
      free (data);
      return (ERR);
    }
  fclose (fp);

  if ( read_variable (data, VERSION_STRING, "%s", version_string) == ERR)
    {
      DebugPrintf (0, "Version string could not be read in config-file...\n");
      free (data);
      return (ERR);
    }
  
  read_variable (data, DRAW_FRAMERATE,           "%d", &GameConfig.Draw_Framerate);
  read_variable (data, DRAW_ENERGY,              "%d", &GameConfig.Draw_Energy);
  read_variable (data, DRAW_POSITION,            "%d", &GameConfig.Draw_Position);
  read_variable (data, DRAW_DEATHCOUNT,          "%d", &GameConfig.Draw_DeathCount);
  read_variable (data, DROID_TALK,               "%d", &GameConfig.Droid_Talk);
  read_variable (data, WANTED_TEXT_VISIBLE_TIME, "%f", &GameConfig.WantedTextVisibleTime);
  read_variable (data, CURRENT_BG_MUSIC_VOLUME,  "%f", &GameConfig.Current_BG_Music_Volume);
  read_variable (data, CURRENT_SOUND_FX_VOLUME,  "%f", &GameConfig.Current_Sound_FX_Volume);
  read_variable (data, CURRENT_GAMMA_CORRECTION, "%f", &GameConfig.Current_Gamma_Correction);
  read_variable (data, THEME_NAME,               "%s", &GameConfig.Theme_Name);
  read_variable (data, FULL_USER_RECT,           "%d", &GameConfig.FullUserRect);
  read_variable (data, USE_FULLSCREEN,           "%d", &GameConfig.UseFullscreen);
  read_variable (data, TAKEOVER_ACTIVATES,       "%d", &GameConfig.TakeoverActivates);
  read_variable (data, SHOW_DECALS,              "%d", &GameConfig.ShowDecals);
  read_variable (data, ALL_MAP_VISIBLE,          "%d", &GameConfig.AllMapVisible);
  read_variable (data, VID_SCALE_FACTOR,         "%f", &GameConfig.scale);

  free (data);

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
  FILE *fp;
  
  if ( ConfigDir[0] == '\0')
    return (ERR);
  
  sprintf (fname, "%s/config", ConfigDir);
  if( (fp = fopen (fname, "w")) == NULL)
    {
      DebugPrintf (0, "WARNING: failed to create config-file: %s\n");
      return (ERR);
    }
  
  // Now write the actual data, line by line
  fprintf (fp, "%s = %s\n", VERSION_STRING, VERSION);
  fprintf (fp, "%s = %d\n", DRAW_FRAMERATE, GameConfig.Draw_Framerate);
  fprintf (fp, "%s = %d\n", DRAW_ENERGY, GameConfig.Draw_Energy);
  fprintf (fp, "%s = %d\n", DRAW_POSITION, GameConfig.Draw_Position);
  fprintf (fp, "%s = %d\n", DRAW_DEATHCOUNT, GameConfig.Draw_DeathCount);
  fprintf (fp, "%s = %d\n", DROID_TALK, GameConfig.Droid_Talk);
  fprintf (fp, "%s = %f\n", WANTED_TEXT_VISIBLE_TIME, GameConfig.WantedTextVisibleTime);
  fprintf (fp, "%s = %f\n", CURRENT_BG_MUSIC_VOLUME, GameConfig.Current_BG_Music_Volume);
  fprintf (fp, "%s = %f\n", CURRENT_SOUND_FX_VOLUME, GameConfig.Current_Sound_FX_Volume);
  fprintf (fp, "%s = %f\n", CURRENT_GAMMA_CORRECTION, GameConfig.Current_Gamma_Correction);
  fprintf (fp, "%s = %s\n", THEME_NAME, GameConfig.Theme_Name);
  fprintf (fp, "%s = %d\n", FULL_USER_RECT, GameConfig.FullUserRect);
  fprintf (fp, "%s = %d\n", USE_FULLSCREEN, GameConfig.UseFullscreen);
  fprintf (fp, "%s = %d\n", TAKEOVER_ACTIVATES, GameConfig.TakeoverActivates);
  fprintf (fp, "%s = %d\n", SHOW_DECALS, GameConfig.ShowDecals);
  fprintf (fp, "%s = %d\n", ALL_MAP_VISIBLE, GameConfig.AllMapVisible);
  fprintf (fp, "%s = %f\n", VID_SCALE_FACTOR, GameConfig.scale);

  fclose (fp);
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
      DebugPrintf (0, "\n\
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
	  DebugPrintf (0, "\n\
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
      DebugPrintf (0, "\n\
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
      DebugPrintf (0, "\n\
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
      DebugPrintf (0, "\n\
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


/*----------------------------------------------------------------------
 *  find label in data and read stuff after label into dst using the FormatString
 *
 * NOTE!!: be sure dst is large enough for data read by FormatString, or
 * sscanf will crash!!
 *
 *----------------------------------------------------------------------*/
void
ReadValueFromString (char* data, char* label, char* FormatString, void* dst)
{
  char *pos;

  // Now we locate the label in data and position pointer right after the label
  pos = LocateStringInData (data, label); // ..will Terminate itself if not found...
  pos += strlen (label);

  if ( sscanf (pos, FormatString, dst) == EOF )
    {
      DebugPrintf (0, "\n ERROR: ReadValueFromString(): could not read value %s of label %s with format %s\n", 
		   pos, FormatString, label ); 
      Terminate(ERR);
    }
  else
    DebugPrintf( 2 , "\nvoid ReadValueFromString ( .... ) : value read in successfully.");

  return;
}

/*-----------------------------------------------------------------
 * find a given filename in subdir relative to FD_DATADIR, 
 *
 * if you pass NULL as "subdir", it will be ignored
 *
 * use current-theme subdir if "use_theme" == USE_THEME, otherwise NO_THEME
 *
 * behavior on file-not-found depends on parameter "critical"
 *  IGNORE: just return NULL
 *  WARNONLY: warn and return NULL
 *  CRITICAL: Error-message and Terminate
 *
 * returns pointer to _static_ string array File_Path, which 
 * contains the full pathname of the file.
 *
 * !! do never try to free the returned string !! 
 * or to keep using it after a new call to find_file!
 *
 *-----------------------------------------------------------------*/
char *
find_file (char *fname, char *subdir, int use_theme, int critical)
{
  static char File_Path[500];   /* hope this will be enough */
  FILE *fp;  // this is the file we want to find?
  int i;
  bool found = FALSE;

  if ( (critical != IGNORE) && (critical != WARNONLY) && (critical != CRITICAL) )
    {
      DebugPrintf (0, "WARNING: unknown critical-value passed to find_file(): %d. Assume CRITICAL\n", 
		   critical);
      critical = CRITICAL;
    }
    
  if (!fname)
    {
      DebugPrintf (0, "\nError: find_file() called with empty filename!\n");
      return (NULL);
    }
  if (!subdir)
    subdir = "";

  for (i=0; i < 2; i++)
    {
      if (i==0)
	strcpy (File_Path, "..");   /* first try local subdirs */
      if (i==1)
	strcpy (File_Path, FD_DATADIR); /* then the FD_DATADIR */

      strcat (File_Path, "/");
      strcat (File_Path, subdir);
      strcat (File_Path, "/");

      if (use_theme == USE_THEME)
	{
	  strcat (File_Path, GameConfig.Theme_Name);
	  strcat (File_Path, "_theme/");
	}

      strcat (File_Path, fname);
      
      if ( (fp = fopen (File_Path, "r")) != NULL)  /* found it? */
	{
	  fclose (fp);
	  found = TRUE;
	  DebugPrintf (1, "find_file() found %s in %s\n", fname, File_Path);
	  break;
	}
    } /* for i */

  if (!found)
    { // how critical is this file for the game:
      switch (critical)
	{
	case WARNONLY:
	  DebugPrintf (0, "WARNING: file %s not found ", fname);
	  if (use_theme == USE_THEME)
	    DebugPrintf (0, " in theme-dir: graphics/%s_theme/ \n", GameConfig.Theme_Name);
	  else DebugPrintf (0, "\n");
	  return (NULL);
	case IGNORE:
	  return (NULL);
	case CRITICAL:
	  DebugPrintf (0, "ERROR: file %s not found ", fname);
	  if (use_theme == USE_THEME)
	    DebugPrintf (0, " in theme-dir: graphics/%s_theme/ \n", GameConfig.Theme_Name);
	  DebugPrintf (0, "...cannot run without it!\n");
	  Terminate (ERR);
	default:
	  DebugPrintf (0, "ERROR in find_file(): Code should never reach this line!! Harakiri\n");
	  Terminate (ERR);
	}
    }

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

      SDL_Delay (1);

      ComputeFPSForThisFrame();

      if (KeyIsPressedR ('c'))
	{
	  if (Me.status != CHEESE) Me.status = CHEESE;
	  else Me.status = PAUSE;
	  Cheese = !Cheese;
	} /* if (CPressed) */

      if ( SpacePressedR() )
	Pause = FALSE;
      
    } /* while (Pause) */

  return;
} // Pause () 


/*@Function============================================================
@Desc: This function starts the time-taking process.  Later the results
       of this function will be used to calculate the current framerate
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
  
  FPSover1 = 1000.0/(float)oneframedelay;
  FPSover10 = 1000.0* 10.0 / (float) tenframedelay;
  FPSover100 = 1000.0 * 100.0 / (float) onehundredframedelay;
  
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
  static char buffer[5000+1];
  va_start (args, fmt);

  if (db_level <= debug_level)
    {
      vsnprintf (buffer, 5000, fmt, args);
      fprintf (stderr, buffer);
      fflush (stderr);
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
  int dice_val;    /* the result in [0, Obergrenze] */

  tmp = 1.0* rand() / RAND_MAX; /* random number in [0;1] */

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
@Desc: This function works a malloc, except that it also checks for
       success and terminates in case of "out of memory", so we dont
       need to do this always in the code.

@Ret: 
* $Function----------------------------------------------------------*/
void *
MyMalloc (long Mamount)
{
  void *Mptr = NULL;

  // make Gnu-compatible even if on a broken system:
  if (Mamount == 0)
    Mamount = 1;

  if ((Mptr = calloc (1, (size_t) Mamount)) == NULL)
    {
      DebugPrintf (0, " MyMalloc(%ld) did not succeed!\n", Mamount);
      Terminate(ERR);
    }

  return Mptr;
}				// void* MyMalloc(long Mamount)

#undef _misc_c
