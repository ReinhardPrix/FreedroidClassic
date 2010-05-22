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

#define INFLUENCER_STRUCTURE_RAW_DATA_STRING "\nNow the raw data of the influencer structure:\n"
#define ALLENEMYS_RAW_DATA_STRING "\nNow the raw AllEnemys data:\n"
#define ALLBULLETS_RAW_DATA_STRING "\nNow the raw AllBullets data:\n"
#define DROID001_RAW_DATA_STRING "\nNow the raw DROID001 data:\n"
#define END_OF_SAVEDGAME_DATA_STRING "\nEnd of saved game data file.\n"
#define LEVELNUM_EXPL_STRING "\nExplicit number of the level the influ currently is in="

#define SAVEDGAME_EXT ".savegame"

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
  char *homedir;
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
  // get home-directory to save in
  if ( (homedir = getenv("HOME")) == NULL )
    {
      DebugPrintf ( 0 , "ERROR: Environment does not contain HOME variable... \n\
I need to know that for saving. Abort.\n");
      Terminate( ERR );
      // return (ERR);
    }

  //--------------------
  // First we save the full ship information, same as with the level editor
  //
  sprintf( filename , "%s/%s%s", homedir, Me[0].character_name, SHIP_EXT );
  if ( SaveShip( filename ) != OK )
    {
      fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
The SAVING OF THE SHIP DATA FOR THE SAVED GAME FAILED!!!\n\
\n\
This is either a bug in Freedroid or an indication, that the directory\n\
permissions are somehow not right.\n\
\n\
Freedroid will terminate now to draw attention to the problem it could not resolve.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" );
      Terminate(ERR);
    }
  else
    {
      DebugPrintf( SAVE_LOAD_GAME_DEBUG , "\nShip data for saved game seems to have been saved correctly.\n");
    }

  //--------------------
  // First, we must determine the save game file name
  //
  sprintf (filename, "%s/%s%s", homedir, Me[0].character_name, ".savegame");

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

  // --------------------
  // Now we write the influencer raw data start string out to the file and of course
  // then the real raw influencer data follow suit afterwards.
  //
  fwrite ( INFLUENCER_STRUCTURE_RAW_DATA_STRING , strlen( INFLUENCER_STRUCTURE_RAW_DATA_STRING ),
	   sizeof(char), SaveGameFile );
  fwrite ( &(Me) , sizeof( influence_t ) , sizeof( char ) , SaveGameFile );
  // fwrite ( DROID001_RAW_DATA_STRING , strlen( DROID001_RAW_DATA_STRING ),
  // sizeof(char), SaveGameFile );
  fwrite ( &( Druidmap[ DRUID001 ]) , sizeof( druidspec ) , sizeof( char ) , SaveGameFile );

  // --------------------
  // Now we write the enemy raw data start string out to the file and of course
  // then the real raw enemy data follow suit afterwards.
  //
  fwrite ( ALLENEMYS_RAW_DATA_STRING , strlen( ALLENEMYS_RAW_DATA_STRING ),
	   sizeof(char), SaveGameFile );
  fwrite ( &(AllEnemys) , sizeof( enemy ) * MAX_ENEMYS_ON_SHIP , sizeof( char ) , SaveGameFile );

  // --------------------
  // Now we write the bullet raw data start string out to the file and of course
  // then the real raw enemy data follow suit afterwards.
  //
  fwrite ( ALLBULLETS_RAW_DATA_STRING , strlen( ALLBULLETS_RAW_DATA_STRING ),
	   sizeof(char), SaveGameFile );
  fwrite ( & ( AllBullets ) , sizeof( bullet ) * MAXBULLETS , sizeof( char ) , SaveGameFile );


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
      printf("\n\nClosing of ship file failed in SaveGame...\n\nTerminating\n\n");
      Terminate(ERR);
      // return ERR;
    }

  DebugPrintf ( SAVE_LOAD_GAME_DEBUG , "\nint SaveGame( void ): end of function reached.");

  return OK;
}; // void SaveGame( void )


/* ----------------------------------------------------------------------
 * This function saves the current game of Freedroid to a file.
 * ---------------------------------------------------------------------- */
int
LoadGame( void )
{
  char *homedir;
  char *LoadGameData;
  char filename[1000];
  unsigned char* InfluencerRawDataPointer;
  unsigned char* EnemyRawDataPointer;
  unsigned char* BulletRawDataPointer;
  int i;
  int current_geographics_levelnum;

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

  //--------------------
  // Before we decode the details of the old game, we load the map
  // information for the old game.  This is not very difficult, since
  // we have very old and proven functions to do it.
  //

  // get home-directory to load from
  if ( (homedir = getenv("HOME")) == NULL )
    {
      DebugPrintf (0, "ERROR: Environment does not contain HOME variable... \n\
I need to know that for loading. Abort.\n");
      return (ERR);
    }
  //--------------------
  // First we save the full ship information, same as with the level editor
  //
  sprintf( filename , "%s/%s%s", homedir, Me[0].character_name, SHIP_EXT);
  LoadShip( filename );

  //--------------------
  // First, we must determine the savedgame data file name
  //
  sprintf (filename, "%s/%s%s", homedir, Me[0].character_name, SAVEDGAME_EXT);

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
  memcpy( & ( Druidmap[ DRUID001 ] ) , InfluencerRawDataPointer , sizeof ( druidspec ) );
  Druidmap [ DRUID001 ].druidname = "001";
  Druidmap [ DRUID001 ].portrait_filename_without_ext = "001";

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


  DebugPrintf ( SAVE_LOAD_GAME_DEBUG , "\nint LoadGame( void ): end of function reached.");
  return OK;
};

#undef _saveloadgame_c
