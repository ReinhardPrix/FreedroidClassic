/* 
 *
 *   Copyright (c) 2002, 2003 Johannes Prix
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
 * All functions that have to do with loading and saving of games.
 * ---------------------------------------------------------------------- */

/*
 * This file should not contain any german, since it originates from
 * a time where we didn't use german for comments any more.
 *
 */

#define _saveloadgame_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"
#include "SDL_rotozoom.h"
#include "sys/stat.h"

#define INFLUENCER_STRUCTURE_RAW_DATA_STRING "\nNow the raw data of the influencer structure:\n"
#define ALLENEMYS_RAW_DATA_STRING "\nNow the raw AllEnemys data:\n"
#define ALLBULLETS_RAW_DATA_STRING "\nNow the raw AllBullets data:\n"
#define DROID001_RAW_DATA_STRING "\nNow the raw DROID001 data:\n"
#define END_OF_SAVEDGAME_DATA_STRING "\nEnd of saved game data file.\n"
#define LEVELNUM_EXPL_STRING "\nExplicit number of the level the influ currently is in="

#define SAVEDGAME_EXT ".savegame"
#define SAVE_GAME_THUMBNAIL_EXT ".thumbnail.bmp"

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
ShowSaveLoadGameProgressMeter( int Percentage , int IsSavegame ) 
{
  SDL_Rect TargetRect;

  TargetRect.x = AllMousePressButtons[ SAVE_GAME_BANNER ] . button_rect . x + ( AllMousePressButtons[ SAVE_GAME_BANNER ] . button_rect . w - 100 ) / 2 + 2 ;
  TargetRect.y = AllMousePressButtons[ SAVE_GAME_BANNER ] . button_rect . y + 20 ;
  TargetRect.w = Percentage ;
  TargetRect.h = 20 ;

  if ( IsSavegame) 
    ShowGenericButtonFromList ( SAVE_GAME_BANNER );
  else
    ShowGenericButtonFromList ( LOAD_GAME_BANNER );
  SDL_FillRect ( Screen , &TargetRect , SDL_MapRGB ( Screen->format , 0x0FF , 0x0FF , 0x0FF ) ) ;
  UpdateScreenOverButtonFromList ( SAVE_GAME_BANNER );

}; // void ShowSaveGameProgressMeter( int Percentage ) 

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
LoadAndShowThumbnail ( char* CoreFilename )
{
  char Saved_Games_Dir[1000];
  char* homedir = NULL ;
  char filename[1000];
  SDL_Surface* NewThumbnail;
  SDL_Rect TargetRectangle;

  DebugPrintf ( 2 , "\nTrying to load thumbnail for character '%s'. " , CoreFilename );

  //--------------------
  // get home-directory to save in
  if ( ( homedir = getenv("HOME")) == NULL ) 
    {
      DebugPrintf ( 0 , "ERROR: Environment does not contain HOME variable... \n\
I need to know that for saving. Abort.\n");
      Terminate( ERR );
      // return (ERR);
    }

  sprintf ( Saved_Games_Dir , "%s/.freedroid_rpg" , homedir );

  //--------------------
  // First we save the full ship information, same as with the level editor
  //
  sprintf( filename , "%s/%s%s", Saved_Games_Dir, CoreFilename , SAVE_GAME_THUMBNAIL_EXT );

  NewThumbnail = IMG_Load ( filename );
  if ( NewThumbnail == NULL ) return;

  // TargetRectangle.x = SCREEN_WIDTH - NewThumbnail ->w ;
  TargetRectangle.x = 10 ;
  TargetRectangle.y = SCREEN_HEIGHT - NewThumbnail ->h - 10 ;
  
  SDL_BlitSurface ( NewThumbnail , NULL , Screen , &TargetRectangle );

  SDL_FreeSurface( NewThumbnail );

}; // void LoadAndShowThumbnail ( char* CoreFilename )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
LoadAndShowStats ( char* CoreFilename )
{
  char Saved_Games_Dir[1000];
  char* homedir = NULL ;
  char filename[1000];
  struct stat FileInfoBuffer;
  char InfoString[5000];
  struct tm *LocalTimeSplitup;
  long int FileSize;

  DebugPrintf ( 2 , "\nTrying to get file stats for character '%s'. " , CoreFilename );

  //--------------------
  // get home-directory to save in
  if ( ( homedir = getenv("HOME")) == NULL ) 
    {
      DebugPrintf ( 0 , "ERROR: Environment does not contain HOME variable... \n\
I need to know that for saving. Abort.\n");
      Terminate( ERR );
      // return (ERR);
    }

  sprintf ( Saved_Games_Dir , "%s/.freedroid_rpg" , homedir );

  //--------------------
  // First we save the full ship information, same as with the level editor
  //
  sprintf( filename , "%s/%s%s", Saved_Games_Dir, CoreFilename , SAVEDGAME_EXT );

  if ( stat ( filename , & ( FileInfoBuffer) ) )
    {
      fprintf( stderr, "\n\nfilename: %s. \n" , filename );
      GiveStandardErrorMessage ( "LoadAndShowStats(...)" , "\
Freedroid was unable to determine the time of the last modification on\n\
your saved game file.\n\
This is either a bug in Freedroid or an indication, that the directory\n\
or file permissions of ~/.freedroid_rpg are somehow not right.",
				 NO_NEED_TO_INFORM, IS_FATAL );
    };

  LocalTimeSplitup = localtime ( & ( FileInfoBuffer.st_mtime ) ) ;

  sprintf( InfoString , "%d/%02d/%02d %02d:%02d" , 
	   1900 + LocalTimeSplitup->tm_year ,
	   LocalTimeSplitup->tm_mon ,
	   LocalTimeSplitup->tm_mday ,
	   LocalTimeSplitup->tm_hour ,
	   LocalTimeSplitup->tm_min );

  PutString ( Screen , 240 , SCREEN_HEIGHT - 3 * FontHeight ( GetCurrentFont () ) , "Last Modified:" );
  PutString ( Screen , 240 , SCREEN_HEIGHT - 2 * FontHeight ( GetCurrentFont () ) , InfoString );

  //--------------------
  // Now that the modification time has been set up, we can start to compute
  // the overall disk space of all files in question.
  //
  FileSize = FileInfoBuffer.st_size;

  //--------------------
  // The saved ship must exist.  On not, it's a sever error!
  //
  sprintf( filename , "%s/%s%s", Saved_Games_Dir, CoreFilename , ".shp" );
  if ( stat ( filename , & ( FileInfoBuffer) ) )
    {
      fprintf( stderr, "\n\nfilename: %s. \n" , filename );
      GiveStandardErrorMessage ( "LoadAndShowStats(...)" , "\
Freedroid was unable to determine the time of the last modification on\n\
your saved game file.\n\
This is either a bug in Freedroid or an indication, that the directory\n\
or file permissions of ~/.freedroid_rpg are somehow not right.",
				 NO_NEED_TO_INFORM, IS_FATAL );
    };
  FileSize += FileInfoBuffer.st_size;

  //--------------------
  // A thumbnail may not yet exist.  We won't make much fuss if it doesn't.
  //
  sprintf( filename , "%s/%s%s", Saved_Games_Dir, CoreFilename , SAVE_GAME_THUMBNAIL_EXT );
  if ( ! stat ( filename , & ( FileInfoBuffer) ) )
    {
        FileSize += FileInfoBuffer.st_size;
    }

  sprintf( InfoString , "File Size: %2.3f MB" , 
	   ((float)FileSize) / ( 1024.0 * 1024.0 ) );

  // PutString ( Screen , 240 , SCREEN_HEIGHT - 2 * FontHeight ( GetCurrentFont () ) , "File Size:" );
  PutString ( Screen , 240 , SCREEN_HEIGHT - 1 * FontHeight ( GetCurrentFont () ) , InfoString );

}; // void LoadAndShowStats ( char* filename );

/* ----------------------------------------------------------------------
 * This function stores a thumbnail of the currently running game, so that
 * these thumbnails can be browsed when choosing which game to load.
 * ---------------------------------------------------------------------- */
void
SaveThumbnailOfGame ( void )
{
  char Saved_Games_Dir[1000];
  char* homedir = NULL ;
  char filename[1000];
  SDL_Surface* NewThumbnail;

  //--------------------
  // get home-directory to save in
  if ( ( homedir = getenv("HOME")) == NULL ) 
    {
      DebugPrintf ( 0 , "ERROR: Environment does not contain HOME variable... \n\
I need to know that for saving. Abort.\n");
      Terminate( ERR );
      // return (ERR);
    }

  sprintf ( Saved_Games_Dir , "%s/.freedroid_rpg" , homedir );

  //--------------------
  // First we save the full ship information, same as with the level editor
  //
  sprintf( filename , "%s/%s%s", Saved_Games_Dir, Me[0].character_name, SAVE_GAME_THUMBNAIL_EXT );

  AssembleCombatPicture ( SHOW_ITEMS );

  NewThumbnail = zoomSurface( Screen , 0.32 , 0.32 , 0 );

  SDL_SaveBMP( NewThumbnail , filename );

  SDL_FreeSurface( NewThumbnail );

}; // void SaveThumbnailOfGame ( void )

/* ----------------------------------------------------------------------
 * This function saves the current game of Freedroid to a file.
 * ---------------------------------------------------------------------- */
int 
SaveGame( void )
{
  char *SaveGameHeaderString;
  FILE *SaveGameFile;  // to this file we will save all the ship data...
  char filename[1000];
  char linebuf[10000];
  char Saved_Games_Dir[1000];
  char* homedir = NULL ;
  char* MenuTexts[10]={ "Back" , "" , "" , "" , "" ,
			"" , "" , "" , "" , "" };

  //--------------------
  // Saving might take a while, therefore we activate the conservative
  // frame rate, just to be sure, so that no sudden jumps occur, perhaps
  // placing the influencer or some bullets outside the map even!
  //
  Activate_Conservative_Frame_Computation();

  DebugPrintf ( SAVE_LOAD_GAME_DEBUG , "\nint SaveGame( void ): real function call confirmed.");

  //--------------------
  // Now we add a security question to prevent the player from (accidentially)
  // saving a game where his character is already dead
  //
  if ( Me[0].energy <= 0 )
    {
      DoMenuSelection( "\n\n    Surely you do not really want do save a game \n\n    where your tux is dead, do you?" , 
		       MenuTexts , 1 , NULL , NULL );
      return ( OK );
    }

  //--------------------
  // Now that we really start to save the game, it's time to display the save
  // game progress meter...
  //
  ShowSaveLoadGameProgressMeter( 0 , TRUE ) ;

  //--------------------
  // get home-directory to save in
  if ( ( homedir = getenv("HOME")) == NULL ) 
    {
      DebugPrintf ( 0 , "ERROR: Environment does not contain HOME variable... \n\
I need to know that for saving. Abort.\n");
      Terminate( ERR );
      // return (ERR);
    }

  sprintf ( Saved_Games_Dir , "%s/.freedroid_rpg" , homedir );

  //--------------------
  // First we save the full ship information, same as with the level editor
  //
  sprintf( filename , "%s/%s%s", Saved_Games_Dir, Me[0].character_name, SHIP_EXT );
  if ( SaveShip( filename ) != OK )
    {
      GiveStandardErrorMessage ( "SaveGame(...)" , "\
The SAVING OF THE SHIP DATA FOR THE SAVED GAME FAILED!\n\
This is either a bug in Freedroid or an indication, that the directory\n\
or file permissions of ~/.freedroid_rpg are somehow not right.",
				 PLEASE_INFORM, IS_FATAL );
    } 
  else
    {
      DebugPrintf( SAVE_LOAD_GAME_DEBUG , "\nShip data for saved game seems to have been saved correctly.\n");
    }

  // ShowSaveGameProgressMeter( 30 ) ;

  //--------------------
  // First, we must determine the save game file name
  //
  sprintf (filename, "%s/%s%s", Saved_Games_Dir, Me[0].character_name, ".savegame");
  
  DebugPrintf ( SAVE_LOAD_GAME_DEBUG , "\nint SaveShip(char *shipname): now opening the savegame file for writing ..."); 

  //--------------------
  // Now that we know which filename to use, we can open the save file for writing
  //
  if( ( SaveGameFile = fopen(filename, "w")) == NULL) {
    printf("\n\nError opening save game file for writing...\n\nTerminating...\n\n");
    Terminate(ERR);
    // return ERR;
  }
  
  //--------------------
  // Now that the file is opend for writing, we can start writing.  And the first thing
  // we will write to the file will be a fine header, indicating what this file is about
  // and things like that...
  //
  SaveGameHeaderString="\n\
----------------------------------------------------------------------\n\
 *\n\
 *   Copyright (c) 1994, 2002 Johannes Prix\n\
 *   Copyright (c) 1994, 2002 Reinhard Prix\n\
 *\n\
 *\n\
 *  This file is part of Freedroid\n\
 *\n\
 *  Freedroid is free software; you can redistribute it and/or modify\n\
 *  it under the terms of the GNU General Public License as published by\n\
 *  the Free Software Foundation; either version 2 of the License, or\n\
 *  (at your option) any later version.\n\
 *\n\
 *  Freedroid is distributed in the hope that it will be useful,\n\
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of\n\
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n\
 *  GNU General Public License for more details.\n\
 *\n\
 *  You should have received a copy of the GNU General Public License\n\
 *  along with Freedroid; see the file COPYING. If not, write to the \n\
 *  Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, \n\
 *  MA  02111-1307  USA\n\
 *\n\
----------------------------------------------------------------------\n\
\n\
This file was generated using the Freedroid save level option.\n\
If you have questions concerning Freedroid, please send mail to:\n\
\n\
freedroid-discussion@lists.sourceforge.net\n\
\n";
  fwrite ( SaveGameHeaderString , strlen( SaveGameHeaderString), sizeof(char), SaveGameFile);  

  //--------------------
  // Since no other information than the dynamic pointer CurLevel is available
  // to tell the current level number, we must write out that information explicitly
  //
  fwrite ( LEVELNUM_EXPL_STRING , strlen ( LEVELNUM_EXPL_STRING ) , sizeof(char), SaveGameFile);  
  sprintf( linebuf , "%d\n", CurLevel->levelnum );
  fwrite ( linebuf , strlen ( linebuf ) , sizeof(char), SaveGameFile);  

  // ShowSaveGameProgressMeter( 40 ) ;

  // --------------------
  // Now we write the influencer raw data start string out to the file and of course
  // then the real raw influencer data follow suit afterwards.
  //
  fwrite ( INFLUENCER_STRUCTURE_RAW_DATA_STRING , strlen( INFLUENCER_STRUCTURE_RAW_DATA_STRING ), 
	   sizeof(char), SaveGameFile );  
  fwrite ( &(Me) , sizeof( influence_t ) , sizeof( char ) , SaveGameFile );  
  // fwrite ( DROID001_RAW_DATA_STRING , strlen( DROID001_RAW_DATA_STRING ), 
  // sizeof(char), SaveGameFile );  
  // fwrite ( &( Druidmap[ DRUID001 ]) , sizeof( druidspec ) , sizeof( char ) , SaveGameFile );  

  // ShowSaveGameProgressMeter( 50 ) ;

  // --------------------
  // Now we write the enemy raw data start string out to the file and of course
  // then the real raw enemy data follow suit afterwards.
  //
  fwrite ( ALLENEMYS_RAW_DATA_STRING , strlen( ALLENEMYS_RAW_DATA_STRING ), 
	   sizeof(char), SaveGameFile );  
  fwrite ( &(AllEnemys) , sizeof( enemy ) * MAX_ENEMYS_ON_SHIP , sizeof( char ) , SaveGameFile );  

  // ShowSaveGameProgressMeter( 60 ) ;

  // --------------------
  // Now we write the bullet raw data start string out to the file and of course
  // then the real raw enemy data follow suit afterwards.
  //
  fwrite ( ALLBULLETS_RAW_DATA_STRING , strlen( ALLBULLETS_RAW_DATA_STRING ), 
	   sizeof(char), SaveGameFile );  
  fwrite ( & ( AllBullets ) , sizeof( bullet ) * MAXBULLETS , sizeof( char ) , SaveGameFile );  

  // ShowSaveGameProgressMeter( 70 ) ;

  //--------------------
  // Now that all the nescessary information has been written to the save game file
  // (hopefully), we can finally add the 'end of saved game file'-marker string, that
  // will be needed by the loading function to detect the end of the file and that the
  // file is really there and complete.  So we add this last string to the file:
  //
  fwrite ( END_OF_SAVEDGAME_DATA_STRING , strlen( END_OF_SAVEDGAME_DATA_STRING ), 
	   sizeof(char), SaveGameFile );

  if( fclose( SaveGameFile ) == EOF) 
    {
      printf("\n\nClosing of save game file failed in SaveGame...\n\nTerminating\n\n");
      Terminate(ERR);
      // return ERR;
    }
  
  ShowSaveLoadGameProgressMeter( 99 , TRUE ); 

  SaveThumbnailOfGame ( );

  ShowSaveLoadGameProgressMeter( 100 , TRUE ) ;

  DebugPrintf ( SAVE_LOAD_GAME_DEBUG , "\nint SaveGame( void ): end of function reached.");
  
  return OK;
}; // int SaveGame( void )


/* ----------------------------------------------------------------------
 * This function loads an old saved game of Freedroid from a file.
 * ---------------------------------------------------------------------- */
int 
DeleteGame( void )
{
  char Saved_Games_Dir[1000];
  char* homedir = NULL ;
  char filename[1000];

  if ( ( homedir = getenv("HOME")) == NULL ) 
    {
      DebugPrintf (0, "\n----------------------------------------------------------------------\n\
ERROR: Environment does not contain HOME variable... \n\
I need to know that for loading.  Giving up loading of games....\n\
Freedroid will not be terminated now, but continue running.\n\
The game however could NOT be saved.\n\
----------------------------------------------------------------------\n");
      return (ERR);
    }

  //--------------------
  // Now we generate the right directory for saving from the home
  // directory.
  //
  sprintf ( Saved_Games_Dir , "%s/.freedroid_rpg" , homedir );

  //--------------------
  // First we save the full ship information, same as with the level editor
  //
  sprintf( filename , "%s/%s%s", Saved_Games_Dir, Me[0].character_name, SHIP_EXT);

  remove ( filename ) ;

  //--------------------
  // First, we must determine the savedgame data file name
  //
  sprintf (filename, "%s/%s%s", Saved_Games_Dir, Me[0].character_name, SAVEDGAME_EXT);

  remove ( filename );

  sprintf( filename , "%s/%s%s", Saved_Games_Dir, Me[0].character_name , SAVE_GAME_THUMBNAIL_EXT );

  remove ( filename );

  return ( OK );

}; // int DeleteGame( void )

/* ----------------------------------------------------------------------
 * This function loads an old saved game of Freedroid from a file.
 * ---------------------------------------------------------------------- */
int 
LoadGame( void )
{
  char Saved_Games_Dir[1000];
  char* homedir = NULL ;
  char *LoadGameData;
  char filename[1000];
  unsigned char* InfluencerRawDataPointer;
  unsigned char* EnemyRawDataPointer;
  unsigned char* BulletRawDataPointer;
  int i;
  int current_geographics_levelnum;
  FILE *DataFile;

  DebugPrintf ( SAVE_LOAD_GAME_DEBUG , "\nint LoadGame( void ): function call confirmed....");
  DebugPrintf ( SAVE_LOAD_GAME_DEBUG , "\nint LoadGame( void ): determining file name....");

  //--------------------
  // Loading might take a while, therefore we activate the conservative
  // frame rate, just to be sure, so that no sudden jumps occur, perhaps
  // placing the influencer or some bullets outside the map even!
  //
  Activate_Conservative_Frame_Computation();

  //--------------------
  // Maybe someone just lost in the game and has then pressed the load
  // button.  Then a new game is loaded and the game-over status has
  // to be restored as well of course.
  //
  GameOver = FALSE; 

  ShowSaveLoadGameProgressMeter( 0 , FALSE )  ;

  //--------------------
  // Before we decode the details of the old game, we load the map
  // information for the old game.  This is not very difficult, since
  // we have very old and proven functions to do it.
  //

  // get home-directory to load from
  if ( ( homedir = getenv("HOME")) == NULL ) 
    {
      DebugPrintf (0, "\n----------------------------------------------------------------------\n\
ERROR: Environment does not contain HOME variable... \n\
I need to know that for loading.  Giving up loading of games....\n\
Freedroid will not be terminated now, but continue running.\n\
The game however could NOT be saved.\n\
----------------------------------------------------------------------\n");
      return (ERR);
    }

  //--------------------
  // Now we generate the right directory for saved games from the home
  // directory.
  //
  sprintf ( Saved_Games_Dir , "%s/.freedroid_rpg" , homedir );

  //--------------------
  // First we load the full ship information, same as with the level editor
  //
  sprintf( filename , "%s/%s%s", Saved_Games_Dir, Me[0].character_name, SHIP_EXT);

  //--------------------
  // Maybe there isn't any saved game by that name.  This case must be checked for
  // and handled...
  //
  if ((DataFile = fopen ( filename , "r")) == NULL)
    {
      GiveMouseAlertWindow ( "\nW A R N I N G !\n\nFreedroidRPG was unable to locate the saved game file you requested to load.\nThis might mean that it really isn't there cause you tried to load a game without ever having saved the game before.  \nThe other explanation of this error might be a severe error in FreedroidRPG.\nNothing will be done about it." );
      /*
      fprintf( stderr, "\n\nfilename: '%s'\n" , filename );
      GiveStandardErrorMessage ( "ReadAndMallocAndTerminateFile(...)" , "\
Freedroid was unable to open a given text file, that should be there and\n\
should be accessible.\n\
This indicates a serious bug in this installation of Freedroid.",
				 PLEASE_INFORM, IS_FATAL );
      */
      return ( ERR ) ;
    }
  else
    {
      DebugPrintf ( 1 , "\nThe saved game file (.shp file) seems to be there at least.....");
    }

  LoadShip( filename );

  //--------------------
  // Now we must determine the savedgame data file name
  //
  sprintf (filename, "%s/%s%s", Saved_Games_Dir, Me[0].character_name, SAVEDGAME_EXT);

  DebugPrintf ( SAVE_LOAD_GAME_DEBUG , "\nint LoadGame( void ): starting to read savegame data....");

  //--------------------
  // Now we can read the whole savegame data into memory with one big flush
  //
  LoadGameData = ReadAndMallocAndTerminateFile( filename , END_OF_SAVEDGAME_DATA_STRING ) ;

  DebugPrintf ( SAVE_LOAD_GAME_DEBUG , "\nint LoadGame( void ): starting to decode savegame data....");

  //--------------------
  // Before we start decoding the details, we get the former level-number where the
  // influencer was walking and construct a new 'CurLevel' out of it.
  //
  ReadValueFromString( LoadGameData ,  LEVELNUM_EXPL_STRING , "%d" , 
		       &current_geographics_levelnum , LoadGameData + 30000 );
  CurLevel = curShip.AllLevels[ current_geographics_levelnum ];


  //--------------------
  // Now we start decoding our new game information and fill it into the apropriate structs
  // We assume, that our target strings will be found, so we give 3000000 as the search area
  // length, since we do not know it exactly
  //
  InfluencerRawDataPointer = MyMemmem( LoadGameData , 3000000 , INFLUENCER_STRUCTURE_RAW_DATA_STRING , 
				       strlen ( INFLUENCER_STRUCTURE_RAW_DATA_STRING ) );
  InfluencerRawDataPointer += strlen ( INFLUENCER_STRUCTURE_RAW_DATA_STRING ) ;
  memcpy( &Me , InfluencerRawDataPointer , sizeof ( influence_t ) );
  InfluencerRawDataPointer += sizeof ( influence_t );

  //--------------------
  // Now we decode the enemy information.
  // We assume, that our target strings will be found, so we give 3000000 as the search area
  // length, since we do not know it exactly
  //
  EnemyRawDataPointer = MyMemmem( LoadGameData , 30000000 , ALLENEMYS_RAW_DATA_STRING , 
				  strlen ( ALLENEMYS_RAW_DATA_STRING ) );
  EnemyRawDataPointer += strlen ( ALLENEMYS_RAW_DATA_STRING ) ;
  memcpy( &(AllEnemys) , EnemyRawDataPointer , sizeof ( enemy ) * MAX_ENEMYS_ON_SHIP );

  //--------------------
  // Now we decode the bullet information.
  // We assume, that our target strings will be found, so we give 10000000 as the search area
  // length, since we do not know it exactly
  //
  BulletRawDataPointer = MyMemmem( LoadGameData , 30000000 , ALLBULLETS_RAW_DATA_STRING , 
				   strlen ( ALLBULLETS_RAW_DATA_STRING ) );
  BulletRawDataPointer += strlen ( ALLBULLETS_RAW_DATA_STRING ) ;
  memcpy( &(AllBullets) , BulletRawDataPointer , sizeof ( bullet ) * MAXBULLETS );

  //--------------------
  // When the original game was still going on, some dynamic things like pointers to some
  // constructed text field might have been used.  Now these dynamic things somewhere in 
  // memory of course do not exist, so any pointer previously refering to them, must be
  // set to acceptable values before an accident (SEGFAULT) occurs!
  //
  DebugPrintf ( SAVE_LOAD_GAME_DEBUG , "\nint LoadGame( void ): now correcting dangerous pointers....");
  Me[0].TextToBeDisplayed = "";
  for ( i = 0 ; i < MAX_ENEMYS_ON_SHIP ; i++ )
    {
      AllEnemys[ i ].TextToBeDisplayed = "" ;
      AllEnemys[ i ].TextVisibleTime = 0;
    }
  for ( i = 0 ; i < MAXBULLETS ; i++ )
    {
      AllBullets[ i ].Surfaces_were_generated = FALSE;
      if ( AllBullets[ i ].angle_change_rate != 0 ) DeleteBullet( i , FALSE );
    }
  


  //--------------------
  // Now that we have loaded the game, we must count and initialize the number
  // of droids used in this ship.  Otherwise we might ignore some robots.
  //
  CountNumberOfDroidsOnShip (  ) ;

  SwitchBackgroundMusicTo( curShip.AllLevels[ Me[0].pos.z ]->Background_Song_Name );

  free ( LoadGameData );

  DebugPrintf ( SAVE_LOAD_GAME_DEBUG , "\nint LoadGame( void ): end of function reached.");

  return OK;
}; // int LoadGame ( void ) 

#undef _saveloadgame_c
