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

#define INFLUENCER_STRUCTURE_RAW_DATA_STRING "Now the raw data of the influencer structure:"
#define ALLENEMYS_RAW_DATA_STRING "Now the raw AllEnemys data:"
#define ALLBULLETS_RAW_DATA_STRING "Now the raw AllBullets data:"
#define DROID001_RAW_DATA_STRING "Now the raw DROID001 data:"
#define END_OF_SAVEDGAME_DATA_STRING "End of saved game data file."
#define LEVELNUM_EXPL_STRING "Explicit number of the level the influ currently is in="

#define SAVEDGAME_EXT ".savegame"
#define SAVE_GAME_THUMBNAIL_EXT ".thumbnail.bmp"

int load_game_command_came_from_inside_running_game = FALSE ;

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
    our_SDL_fill_rect_wrapper ( Screen , &TargetRect , SDL_MapRGB ( Screen->format , 0x0FF , 0x0FF , 0x0FF ) ) ;
    UpdateScreenOverButtonFromList ( SAVE_GAME_BANNER );

}; // void ShowSaveGameProgressMeter( int Percentage ) 

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
LoadAndShowThumbnail ( char* CoreFilename )
{
  char filename[1000];
  SDL_Surface* NewThumbnail;
  SDL_Rect TargetRectangle;

  if (!ConfigDir)
    return;

  DebugPrintf ( 2 , "\nTrying to load thumbnail for character '%s'. " , CoreFilename );

  //--------------------
  // First we save the full ship information, same as with the level editor
  //
  
  sprintf( filename , "%s/%s%s", ConfigDir, CoreFilename , SAVE_GAME_THUMBNAIL_EXT );

  NewThumbnail = our_IMG_load_wrapper ( filename );
  if ( NewThumbnail == NULL ) return;

  // TargetRectangle.x = GameConfig . screen_width - NewThumbnail ->w ;
  TargetRectangle.x = 10 ;
  TargetRectangle.y = GameConfig . screen_height - NewThumbnail ->h - 10 ;

  if ( use_open_gl ) swap_red_and_blue_for_open_gl ( NewThumbnail );  
  our_SDL_blit_surface_wrapper ( NewThumbnail , NULL , Screen , &TargetRectangle );

  SDL_FreeSurface( NewThumbnail );

}; // void LoadAndShowThumbnail ( char* CoreFilename )

/* ----------------------------------------------------------------------
 * 
 *
 * ---------------------------------------------------------------------- */
void
LoadAndShowStats ( char* CoreFilename )
{
    char filename[1000];
    struct stat FileInfoBuffer;
    char InfoString[5000];
    struct tm *LocalTimeSplitup;
    long int FileSize;
    
    if ( ! ConfigDir )
	return;
    
    DebugPrintf ( 2 , "\nTrying to get file stats for character '%s'. " , CoreFilename );
    
    //--------------------
    // First we save the full ship information, same as with the level editor
    //
    
    sprintf( filename , "%s/%s%s", ConfigDir, CoreFilename , SAVEDGAME_EXT );
    
    if ( stat ( filename , & ( FileInfoBuffer) ) )
    {
	fprintf( stderr, "\n\nfilename: %s. \n" , filename );
	GiveStandardErrorMessage ( __FUNCTION__  , "\
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
    
    PutString ( Screen , 240 , GameConfig . screen_height - 3 * FontHeight ( GetCurrentFont () ) , "Last Modified:" );
    PutString ( Screen , 240 , GameConfig . screen_height - 2 * FontHeight ( GetCurrentFont () ) , InfoString );
    
    //--------------------
    // Now that the modification time has been set up, we can start to compute
    // the overall disk space of all files in question.
    //
    FileSize = FileInfoBuffer.st_size;
    
    //--------------------
    // The saved ship must exist.  On not, it's a sever error!
    //
    sprintf( filename , "%s/%s%s", ConfigDir, CoreFilename , ".shp" );
    if ( stat ( filename , & ( FileInfoBuffer) ) )
    {
	fprintf( stderr, "\n\nfilename: %s. \n" , filename );
	GiveStandardErrorMessage ( __FUNCTION__  , "\
Freedroid was unable to determine the time of the last modification on\n\
your saved game file.\n\
This is either a bug in Freedroid or an indication, that the directory\n\
or file permissions of ~/.freedroid_rpg are somehow not right.",
				   NO_NEED_TO_INFORM, IS_FATAL );
    }
    FileSize += FileInfoBuffer.st_size;
    
    //--------------------
    // A thumbnail may not yet exist.  We won't make much fuss if it doesn't.
    //
    sprintf( filename , "%s/%s%s", ConfigDir, CoreFilename , SAVE_GAME_THUMBNAIL_EXT );
    if ( ! stat ( filename , & ( FileInfoBuffer) ) )
    {
        FileSize += FileInfoBuffer.st_size;
    }
    
    sprintf( InfoString , "File Size: %2.3f MB" , 
	     ((float)FileSize) / ( 1024.0 * 1024.0 ) );
    
    // PutString ( Screen , 240 , GameConfig . screen_height - 2 * FontHeight ( GetCurrentFont () ) , "File Size:" );
    PutString ( Screen , 240 , GameConfig . screen_height - 1 * FontHeight ( GetCurrentFont () ) , InfoString );
    
}; // void LoadAndShowStats ( char* filename );

/* ----------------------------------------------------------------------
 * This function stores a thumbnail of the currently running game, so that
 * these thumbnails can be browsed when choosing which game to load.
 * ---------------------------------------------------------------------- */
void
SaveThumbnailOfGame ( void )
{
    char filename[1000];
    SDL_Surface* NewThumbnail;
    SDL_Surface* FullView;
    
    if (!ConfigDir)
	return;
    
    //--------------------
    // First we save the full ship information, same as with the level editor
    //
    sprintf( filename , "%s/%s%s", ConfigDir, Me [ 0  ] . character_name , SAVE_GAME_THUMBNAIL_EXT );
    
    AssembleCombatPicture ( SHOW_ITEMS );
    
    if ( use_open_gl )
    {
	//--------------------
	// We need to make a copy in processor memory.  This has already
	// been implemented once, and we can just reuse the old code here.
	//
	StoreMenuBackground ( 1 ) ;
	
	//--------------------
	// Now we need to make a real SDL surface from the raw image data we
	// have just extracted.
	//
	FullView = SDL_CreateRGBSurfaceFrom( StoredMenuBackground [ 1 ] , GameConfig . screen_width , GameConfig . screen_height, 24, 3 * GameConfig . screen_width, 0x0FF0000, 0x0FF00, 0x0FF , 0 );
	
	NewThumbnail = zoomSurface( FullView , 0.32 * 640.0f / GameConfig . screen_width , 0.32 * 640.0f / GameConfig . screen_width , 0 );
	
	//--------------------
	// Of course, since we used OpenGL for generating the raw image data, the data is
	// upside down again.  Now that won't be much of a problem, since we've already
	// dealt with is several times, using the following flipping code.
	//
	flip_image_horizontally ( NewThumbnail );
	swap_red_and_blue_for_open_gl ( NewThumbnail );
	
	SDL_FreeSurface ( FullView ) ;
    }
    else
    {
	NewThumbnail = zoomSurface( Screen , 0.32 , 0.32 , 0 );
    }
    
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
    char* MenuTexts[10]={ "Back" , "" , "" , "" , "" , "" , "" , "" , "" , "" };
    
    if (!ConfigDir)
	return (OK);
    
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
			 MenuTexts , 1 , -1 , NULL );
	return ( OK );
    }
    
    //--------------------
    // Now that we really start to save the game, it's time to display the save
    // game progress meter...
    //
    ShowSaveLoadGameProgressMeter( 0 , TRUE ) ;
    
    //--------------------
    // We must make sure the version string in the Me struct is set 
    // correctly for later loading...
    // But not only the version, but also some struct sizes and array
    // lengthes should be checked as well, so that we have some extra
    // protection against saved game data not fitting into some structs
    // any more...
    //
    sprintf ( Me [ 0 ] . freedroid_version_string , 
	      "%s;sizeof(tux_t)=%d;sizeof(enemy)=%d;sizeof(bullet)=%d;MAXBULLETS=%d;MAX_ENEMYS_ON_SHIP=%d\n", 
	      VERSION , 
	      (int) sizeof(tux_t) , 
	      (int) sizeof(enemy) ,
	      (int) sizeof(bullet) ,
	      (int) MAXBULLETS ,
	      (int) MAX_ENEMYS_ON_SHIP );
    
    //--------------------
    sprintf( filename , "%s/%s%s", ConfigDir, Me[0].character_name, SHIP_EXT );
    if ( SaveShip( filename ) != OK )
    {
	GiveStandardErrorMessage ( __FUNCTION__  , "\
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
    sprintf (filename, "%s/%s%s", ConfigDir, Me[0].character_name, ".savegame");
    
    DebugPrintf ( SAVE_LOAD_GAME_DEBUG , "\nint SaveShip(char *shipname): now opening the savegame file for writing ..."); 
    
    //--------------------
    // Now that we know which filename to use, we can open the save file for writing
    //
    if( ( SaveGameFile = fopen(filename, "wb")) == NULL) {
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
    fwrite ( &(Me) , sizeof( tux_t ) , sizeof( char ) , SaveGameFile );  
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
    
    append_new_game_message ( "Game saved." );

    DebugPrintf ( SAVE_LOAD_GAME_DEBUG , "\nint SaveGame( void ): end of function reached.");
    
    return OK;

}; // int SaveGame( void )


/* ----------------------------------------------------------------------
 * This function loads an old saved game of Freedroid from a file.
 * ---------------------------------------------------------------------- */
int 
DeleteGame( void )
{
  char filename[1000];

  if (!ConfigDir)
    return (OK);

  //--------------------
  // First we save the full ship information, same as with the level editor
  //
  sprintf( filename , "%s/%s%s", ConfigDir, Me[0].character_name, SHIP_EXT);

  remove ( filename ) ;

  //--------------------
  // First, we must determine the savedgame data file name
  //
  sprintf (filename, "%s/%s%s", ConfigDir, Me[0].character_name, SAVEDGAME_EXT);

  remove ( filename );

  sprintf( filename , "%s/%s%s", ConfigDir, Me[0].character_name , SAVE_GAME_THUMBNAIL_EXT );

  remove ( filename );

  return ( OK );

}; // int DeleteGame( void )

/* ----------------------------------------------------------------------
 * This function loads an old saved game of Freedroid from a file.
 * ---------------------------------------------------------------------- */
int 
LoadGame( void )
{
    char version_check_string[1000];
    char *LoadGameData;
    char filename[1000];
    unsigned char* InfluencerRawDataPointer;
    unsigned char* EnemyRawDataPointer;
    unsigned char* BulletRawDataPointer;
    int i;
    int current_geographics_levelnum;
    FILE *DataFile;
    
    if (!ConfigDir)
    {
	DebugPrintf (0, "No Config-directory, cannot load any games\n");
	return (OK);
    }
    
    DebugPrintf ( SAVE_LOAD_GAME_DEBUG , "\n%s(): function call confirmed...." , __FUNCTION__ );
    
    //--------------------
    // Loading might take a while, therefore we activate the conservative
    // frame rate, just to be sure, so that no sudden jumps occur, perhaps
    // placing the influencer or some bullets outside the map even!
    //
    Activate_Conservative_Frame_Computation();
    global_ingame_mode = GLOBAL_INGAME_MODE_NORMAL ;
    
    ShowSaveLoadGameProgressMeter( 0 , FALSE )  ;
    
    //--------------------
    // First we load the full ship information, same as with the level editor
    
    sprintf( filename , "%s/%s%s", ConfigDir, Me [ 0 ] . character_name, SHIP_EXT);
    
    //--------------------
    // Maybe there isn't any saved game by that name.  This case must be checked for
    // and handled...
    //
    if ((DataFile = fopen ( filename , "rb")) == NULL )
    {
	GiveMouseAlertWindow ( "\nW A R N I N G !\n\nFreedroidRPG was unable to locate the saved game file you requested to load.\nThis might mean that it really isn't there cause you tried to load a game without ever having saved the game before.  \nThe other explanation of this error might be a severe error in FreedroidRPG.\nNothing will be done about it." );
	/*
	  fprintf( stderr, "\n\nfilename: '%s'\n" , filename );
	  GiveStandardErrorMessage ( __FUNCTION__  , "\
	  Freedroid was unable to open a given text file, that should be there and\n\
	  should be accessible.\n\
	  This indicates a serious bug in this installation of Freedroid.",
	  PLEASE_INFORM, IS_FATAL );
	*/
	append_new_game_message ( "Failed to load old game." );
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
    sprintf (filename, "%s/%s%s", ConfigDir, Me[0].character_name, SAVEDGAME_EXT);
    
    DebugPrintf ( SAVE_LOAD_GAME_DEBUG , "\n%s(): starting to read savegame data...." , __FUNCTION__ );
    
    //--------------------
    // Now we can read the whole savegame data into memory with one big flush
    //
    LoadGameData = ReadAndMallocAndTerminateFile( filename , END_OF_SAVEDGAME_DATA_STRING ) ;
    
    DebugPrintf ( SAVE_LOAD_GAME_DEBUG , "\n%s(): starting to decode savegame data...." , __FUNCTION__ );
    
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
    memcpy( &Me , InfluencerRawDataPointer , sizeof ( tux_t ) );
    InfluencerRawDataPointer += sizeof ( tux_t );
    
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
    DebugPrintf ( SAVE_LOAD_GAME_DEBUG , "\n%s(): now correcting dangerous pointers...." , __FUNCTION__ );
    Me [ 0 ] . TextToBeDisplayed = "";
    for ( i = 0 ; i < MAX_ENEMYS_ON_SHIP ; i++ )
    {
	AllEnemys[ i ].TextToBeDisplayed = "" ;
	AllEnemys[ i ].TextVisibleTime = 0;
    }
    for ( i = 0 ; i < MAXBULLETS ; i++ )
    {
	//--------------------
	// This might mean a slight memory loss, but I guess we can live with that...
	//
	AllBullets [ i ] . Surfaces_were_generated = FALSE;
	if ( AllBullets[ i ].angle_change_rate != 0 ) DeleteBullet( i , FALSE );
    }
    
    //--------------------
    // Now we check if the loaded game is from a compatible version of FreedroidRPG, or
    // if we maybe have a saved game from a different version, which would mean trouble
    //
    // NOTE:  WE MUST DO THIS BEFORE ANY REFERENCE TO THE LOADED GAME IS MADE, EVEN IF
    //        THAT REFERENCE IS SO SMALL AS TO JUST SET BACKGROUND MUSIC ACCORDING TO
    //        CURRENT TUX POSITION IN THE MAPS AS FOUND BELOW
    //
    //
    //
    sprintf ( version_check_string , "%s;sizeof(tux_t)=%d;sizeof(enemy)=%d;sizeof(bullet)=%d;MAXBULLETS=%d;MAX_ENEMYS_ON_SHIP=%d\n", 
	      VERSION , 
	      (int) sizeof(tux_t) , 
	      (int) sizeof(enemy) ,
	      (int) sizeof(bullet) ,
	      (int) MAXBULLETS ,
	      (int) MAX_ENEMYS_ON_SHIP );
    
    if ( strcmp ( Me [ 0 ] . freedroid_version_string , version_check_string ) != 0 )
    {
	show_button_tooltip ( "Error: Version or structsize mismatch! The saved game in question appears to be from a (slightly?) different version of FreedroidRPG.\nSorry, but I refuse to load it for safety/stability reasons...\nFor Recovery, a blank game will be loaded...(please disregard)\n" );
	our_SDL_flip_wrapper( Screen );
	while ( SpacePressed() ) SDL_Delay ( 3 );
	while ( !SpacePressed() ) SDL_Delay ( 3 );
	while ( SpacePressed() ) SDL_Delay ( 3 );

	//--------------------
	// Now at this point the current Tux data has been junked
	// around with from the failed loading attempt.  We must
	// clear out the data and then safely exit out of the current
	// game!
	//
	// WARNING!  If the game is already running, this really
	// dangerous.  In that case the damaged data should best be
	// completely overwritten with something sensible...
	// (Otherwise Floating Point Exceptions and the like are likely...)
	//
	if ( load_game_command_came_from_inside_running_game )
	{
	    clear_player_inventory_and_stats ( ) ;
	    UpdateAllCharacterStats ( 0 ) ;
	    LoadShip ( find_file ( "Asteroid.maps" , MAP_DIR, FALSE) ) ;
	    PrepareStartOfNewCharacter (  ) ;
	}
	return ( ERR ) ;
    }
    
    //--------------------
    // To prevent cheating, we remove all active spells, that might still be there
    // from other games just played before.
    //
    clear_active_spells();
    
    //--------------------
    // Now that we have loaded the game, we must count and initialize the number
    // of droids used in this ship.  Otherwise we might ignore some robots.
    //
    CountNumberOfDroidsOnShip (  ) ;
    
    SwitchBackgroundMusicTo( curShip.AllLevels[ Me[0].pos.z ]->Background_Song_Name );
    
    free ( LoadGameData );
    
    //--------------------
    // Maybe someone just lost in the game and has then pressed the load
    // button.  Then a new game is loaded and the game-over status has
    // to be restored as well of course.
    //
    GameOver = FALSE; 
    
    DebugPrintf ( SAVE_LOAD_GAME_DEBUG , "\n%s(): end of function reached." , __FUNCTION__ );
    
    //--------------------
    // Now we know that right after loading an old saved game, the Tux might have
    // to 'change clothes' i.e. a lot of tux images need to be updated which can
    // take a little time.  Therefore we print some message so the user will not
    // panic and push the reset button :)
    //
    PutStringFont ( Screen , FPS_Display_BFont , 75 , 150 , "Updating Tux images (this may take a little while...)" );
    our_SDL_flip_wrapper ( Screen );
    
    //--------------------
    // Now that the whole character information (and that included the automap
    // information) has been restored, we can use the old automap data to rebuild
    // the automap texture in the open_gl case...
    //
    insert_old_map_info_into_texture (  );

    load_game_command_came_from_inside_running_game = TRUE ;

    append_new_game_message ( "Game Loaded." );

    return OK;
}; // int LoadGame ( void ) 

#undef _saveloadgame_c
