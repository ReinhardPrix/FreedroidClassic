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
/*----------------------------------------------------------------------
 *
 * Desc:  all functions dealing with sound are contained in this file.
 *
 *----------------------------------------------------------------------*/

#ifndef _sound_c
#define _sound_c
#endif

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"


// The following is the definition of the sound file names used in freedroid
// DO NOT EVER CHANGE THE ORDER OF APPEARENCE IN THIS LIST PLEASE!!!!!
// The order of appearance here should match the order of appearance 
// in the enum-Environment located in defs.h!

#define ALL_SOUNDS 63
char *SoundSampleFilenames[ALL_SOUNDS] = {
   "ERRORSOUND_NILL.NOWAV",
   "Combat_Background_Music.wav",
   "Takeover_Background_Music.wav",
   "Console_Background_Music.wav",
   "Classical_Beep_Beep_Background_Music.wav",
   "Blast_Sound_0.wav",
   "Collision_Sound_0.wav",
   "GotIntoBlast_Sound_0.wav",
   "MoveElevator_Sound_0.wav",
   "Refresh_Sound_0.wav",
   "LeaveElevator_Sound_0.wav",
   "EnterElevator_Sound_0.wav",
   "ThouArtDefeated_Sound_0.wav",
   "Got_Hit_Sound_0.wav",
   "Enemy_Got_Hit_Sound_0.wav",
   "TakeoverSetCapsule_Sound_0.wav",
   "Menu_Item_Selected_Sound_1.wav",
   "Menu_Item_Deselected_Sound_0.wav",
   "Move_Menu_Position_Sound_0.wav",
   "Takeover_Game_Won_Sound_0.wav",
   "Takeover_Game_Deadlock_Sound_0.wav",
   "Takeover_Game_Lost_Sound_0.wav",
   "Fire_Bullet_Pulse_Sound_0.wav",
   "Fire_Bullet_Single_Pulse_Sound_0.wav",
   "Fire_Bullet_Military_Sound_0.wav",
   "Fire_Bullet_Flash_Sound_0.wav",
   "Fire_Bullet_Exterminator_Sound_0.wav",
   // "Fire_Bullet_Laser_Rifle_Sound_0.wav",
   "phaser.wav",
   "Fire_Bullet_Single_Laser_Sound_0.wav",
   "Fire_Bullet_Plasma_Pistol_Sound_0.wav",
   "Fire_Bullet_Sword_Sound_0.wav",
   "Bullet_Reflected_Sound_0.wav",
   "Cry_Sound_0.wav",
   "Takeover_Sound_0.wav",
   "Mission_Status_Change_Sound_0.wav",
   "Item_Taken_Sound_0.wav",
   "ICantCarryAnyMore_Sound_0.wav",
   "MSMachinesClose_0.wav",
   "Item_Drop_Sound_0.wav",
   "Item_Drop_Sound_1.wav",
   "Item_Drop_Sound_2.wav",
   "Item_Drop_Sound_3.wav",
   "Item_Drop_Sound_4.wav",
   "Item_Drop_Sound_5.wav",
   "Item_Armour_Put_Sound_0.wav",
   "Item_Wheels_Put_Sound_0.wav",
   "Item_Range_Weapon_Put_Sound_0.wav",
   "First_Contact_Sound_0.wav",
   "First_Contact_Sound_1.wav",
   "Not_Enough_Power_Sound_0.wav",
   "Not_Enough_Dist_Sound_0.wav",
   "Not_Enough_Mana_0.wav",
   "Influencer_Scream_Sound_0.wav",
   "Influencer_Scream_Sound_1.wav",
   "Influencer_Scream_Sound_2.wav",
   "Influencer_Scream_Sound_3.wav",
   "Influencer_Scream_Sound_4.wav",
   "Spell_ForceToEnergy_Sound_0.wav",
   "Spell_DetectItems_Sound_0.wav",
   "Shop_ItemSoldSound_0.wav",
   "Shop_ItemBoughtSound_0.wav",
   "Shop_ItemRepairedSound_0.wav",
   "Shop_ItemIdentifiedSound_0.wav"
};

#ifdef HAVE_LIBSDL_MIXER
Mix_Chunk *Loaded_WAV_Files[ALL_SOUNDS];
#endif

#define ALL_MOD_MUSICS 2
char *MOD_Music_SampleFilenames[ALL_MOD_MUSICS] = {
  "ERRORSOUND_NILL.NOMOD",
  "A_City_at_Night.mod"
};

#define MAX_SOUND_CHANNELS 5000

char SoundChannelList[ MAX_SOUND_CHANNELS ];
#ifdef HAVE_LIBSDL_MIXER
Mix_Chunk* List_Of_Sustained_Release_WAV_Files[ MAX_SOUND_CHANNELS ];
Mix_Music *Loaded_MOD_Files[ALL_MOD_MUSICS] =
{
  NULL,
  NULL
};
#endif


//----------------------------------------------------------------------
// We want to know in certain cases if a channel has finished playback
// or not.  For this we keep track of channel finished messages.
// This works as follows:
//
// * When a channel is starting playback via the function 
//   PlayOnceNeededSoundSample, then we mark the channel as in use.
//
// * A callback function is set via SDL to THIS FUNCTION.
//
// * This function cancels any channel bits that have finished playback.
//
// ---------------------------------------------------------------------
void channelDone(int channel) {

  DebugPrintf( 0 , "\nCALLBACK FUNCTION INVOKED:  channel %d finished playback.\n" , channel );

  //--------------------
  // Maybe the PlayOnceNeededSoundSample function was called with argument 'non-wait',
  // which means that it allocated a chunk and started playing, but was unable to free
  // this chunk again, since it of course was not done playing by then.
  //
  // In this case we must do the unallocation work here, since now we
  // know that the appropriate channel has stopped playing.
  //
  if ( SoundChannelList[ channel ] == 2 ) 
    {

#ifdef HAVE_LIBSDL_MIXER
      DebugPrintf( 0 , "\nCALLBACK FUNCTION:  Detected soundchannel for sustained release.... freeing chunk..." );
      Mix_FreeChunk ( List_Of_Sustained_Release_WAV_Files[ channel ] );
#endif

    }

  //--------------------
  // Now we can safely mark the channel as unused again
  //
  SoundChannelList[ channel ] = 0;

}; // void channelDone(int channel) {

//----------------------------------------------------------------------
// This function should play a sound sample, that is NOT needed within
// the action part of the game but only in menus or dialogs and can 
// therefore be loaded and dumped on demand while the other sound samples
// for the action parts of the game will be kept in memory all the time.
// ----------------------------------------------------------------------
void
PlayOnceNeededSoundSample( char* SoundSampleFileName , int With_Waiting) 
{
  //--------------------
  // These variables will always be needed!
  //
  int simulated_playback_starting_time;

  //--------------------
  // These variables will only be needed when compiling with sound!
  //
#ifdef HAVE_LIBSDL_MIXER
  int Newest_Sound_Channel=0;
  Mix_Chunk *One_Shot_WAV_File;
  char Temp_Filename[5000];
  char* fpath;
#endif

  //--------------------
  // In case there are no sound capabilities on this machine, we 
  // wait if that is appropriate or otherwise terminate immediately.
  //
#ifndef HAVE_LIBSDL_MIXER
  if ( With_Waiting )
    {
      simulated_playback_starting_time = SDL_GetTicks() ;
      
      while ( ( SDL_GetTicks() - simulated_playback_starting_time < 7 * 1000 ) && 
	      !EscapePressed() && !SpacePressed() );
      
      while ( EscapePressed() || SpacePressed() );
    }

  return;
#else

  //--------------------
  // In case sound has been disabled, we don't do anything here...
  //
  if ( !sound_on ) 
    {
      //--------------------
      // Maybe this sound sample was intended to be hooking the CPU and the
      // program flow, so that nothing happens until the sample has been
      // played fully.  In this case we must introduce a waiting time even
      // now that no sound sample is played.  A default of 7 seconds seems to
      // be appropriate.  On pressing the left button or space or escape
      // the waiting time will be cancelled anyway.
      //
      if ( With_Waiting )
	{
	  simulated_playback_starting_time = SDL_GetTicks() ;
	  
	  while ( ( SDL_GetTicks() - simulated_playback_starting_time < 7 * 1000 ) && 
		  !EscapePressed() && !SpacePressed() );
	  
	}
      while ( EscapePressed() || SpacePressed() );

      //--------------------
      // Since sound is disabled otherwise we MUST return here and not
      // try to do any sound operations on this machine with perhaps no sound
      // modules and no SDL sound initialized.
      //
      return;
    }

  //--------------------
  // Now we set a callback function, that should be called by SDL
  // as soon as ANY other sound channel finishes playing...
  //
  Mix_ChannelFinished( channelDone );

  //--------------------
  // Now we try to load the requested sound file into memory...
  //
  One_Shot_WAV_File=NULL;

  strcpy ( Temp_Filename , "speeches/" );
  strcat ( Temp_Filename , SoundSampleFileName );
  fpath = find_file ( Temp_Filename , SOUND_DIR, FALSE);
  One_Shot_WAV_File = Mix_LoadWAV( fpath );
  if ( One_Shot_WAV_File == NULL )
    {
      fprintf (stderr,
	       "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
The SDL MIXER WAS UNABLE TO LOAD A CERTAIN SOUND FILE INTO MEMORY.\n\
\n\
The name of the problematic file is:\n\
%s \n\
\n\
The reason for this is as follows:  Speech files are stored in wav format\n\
for technical reasons in conjunction with the SDL and the background music.\n\
This tends to use up much space on disk, i.e. several megabytes which would\n\
make the archive a bit large for download via 56K modem.  Therefore not all\n\
sound samples featuring dialog speeches are in the freedroid repository.\n\
\n\
But to ensure smooth gameplay even with missing sound files, there is an option\n\
to have freedroid either ignore the missing dialog sound samples or to terminate\n\
on encountering a missing sound sample.\n\
This option is set to " 
	       , fpath );

      if ( GameConfig.terminate_on_missing_speech_sample )
	{
	  fprintf (stderr, " TERMINATE ON MISSING SPEECH SAMPLE=TRUE\n\
which will cause freedroid to terminate now.  You can use the menu to change\n\
this setting if you wish to play smoothly or you could download the rest of the\n\
sound samples, that are missing in the game from our web page (soon).\n\
But for now Freedroid will terminate to draw attention to the sound problem it \n\
could not resolve, as you requested via the option mentioned above. Sorry...\n\
----------------------------------------------------------------------\n\
\n" );
	  Terminate( ERR );
	}
      else
	{
	  fprintf (stderr, " TERMINATE ON MISSING SPEECH SAMPLE=FALSE\n\
which will cause freedroid to continue running and do nothing otherwise.\n\
If you wish to have the sound samples, you could try to download and install\n\
the rest of the speech samples from our web page (hopefully soon).\n\
----------------------------------------------------------------------\n\
\n" );
	  //--------------------
	  // Maybe this sound sample was intended to be hooking the CPU and the
	  // program flow, so that nothing happens until the sample has been
	  // played fully.  In this case we must introduce a waiting time even
	  // if no sound sample is played.  A default of 7 seconds seems to
	  // be appropriate.  On pressing the left button or space or escape
	  // the waiting time will be cancelled anyway.
	  //
	  if ( With_Waiting )
	    {
	      simulated_playback_starting_time = SDL_GetTicks() ;

	      while ( ( SDL_GetTicks() - simulated_playback_starting_time < 7 * 1000 ) && 
		      !EscapePressed() && !SpacePressed() );

	      while ( EscapePressed() || SpacePressed() );
	    }

	  //--------------------
	  // Now we must return, since we do not want to 'free' the sound sample, that
	  // hasn't been loaded successfully and produce a segfault, do we?
	  //
	  return;

	}
    } // if ( !Loaded_WAV...
  else
    {
      DebugPrintf ( 0 , "\nSuccessfully loaded file %s into memory for playing once." , SoundSampleFileName );
    }

  //--------------------
  // Now we try to play the sound file that has just been successfully
  // loaded into memory...
  //
  // In case of an error, we will of course print an error message
  // and quit...
  //
  Newest_Sound_Channel = Mix_PlayChannel( -1 , One_Shot_WAV_File , 0 );
  if ( Newest_Sound_Channel <= -1 )
    {
      fprintf (stderr,
	       "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
The a SDL MIXER WAS UNABLE TO PLAY A CERTAIN FILE LOADED INTO MEMORY FOR PLAYING ONCE.\n\
\n\
The name of the problematic file is:\n\
%s \n\
\n\
Analysis of the error has returned the following explanation through SDL:\n\
%s \n\
Freedroid will be terminated now to draw attention to this sound problem,\n\
it could not resolve.  Please inform the developers about it.\n\
Sorry for interrupting your game.  \n\
----------------------------------------------------------------------\n\
\n" , SoundSampleFileName , Mix_GetError() );
      Terminate (ERR);
    } // if ( ... = -1
  else
    {
      SoundChannelList[ Newest_Sound_Channel ] = 1;
      DebugPrintf( 0 , "\nSuccessfully playing the 'ONCE NEEDED' file %s.", SoundSampleFileName ) ;
    }

  //--------------------
  // Maybe this sound sample is intended to be hooking the CPU and the
  // program flow, so that nothing happens until the sample has been
  // played fully...
  //
  if ( With_Waiting )
    {
      while ( SoundChannelList[ Newest_Sound_Channel ] && !EscapePressed() && !SpacePressed() );
      //--------------------
      // In case escape was pressed, the currently playing voice sample must
      // be terminated immediately.
      //
      if ( EscapePressed() || SpacePressed() )
	{
	  Mix_HaltChannel( Newest_Sound_Channel );
	  while ( EscapePressed() || SpacePressed() );

	  //--------------------
	  // Now the channel has finished playing (or we have stopped it) and
	  // now we can unallocate the resources used by it...
	  //
	  Mix_FreeChunk ( One_Shot_WAV_File );
	  
	}
    }
  else
    {
      //--------------------
      // Otherwise, if there was no 'With_Waiting' flag set, 
      // we do nothing here, cause we can't halt the channel and 
      // we also can't free the channel, that is still playing.
      //
      // All we will do is set the channels flag to 2, so that the
      // callback function will know what to do when called:  TO
      // FREE THE SOUND CHUNK!
      //
      SoundChannelList[ Newest_Sound_Channel ] = 2;
      List_Of_Sustained_Release_WAV_Files[ Newest_Sound_Channel ] = One_Shot_WAV_File ;
    }


#endif // HAVE_LIBSDL_MIXER

};

// ----------------------------------------------------------------------
// This function shall initialize the SDL Audio subsystem.  It is called
// as soon as Freedroid is started.  It does ONLY work with SDL and no
// longer with any form of sound engine like the YIFF.
// ----------------------------------------------------------------------
void 
Init_Audio(void)
{
#ifndef HAVE_LIBSDL_MIXER  
  return;
#else
  char *fpath;
  int i;
  int audio_rate = 22050;
  Uint16 audio_format = AUDIO_S16; 
  int audio_channels = 2;
  //  int audio_buffers = 4096;
  int audio_buffers = 2048;
  char Temp_Filename[5000];

  DebugPrintf(1, "\nInitializing SDL Audio Systems....\n");

  if ( !sound_on ) return;


  // Now SDL_AUDIO is initialized here:

  if ( SDL_InitSubSystem ( SDL_INIT_AUDIO ) == -1 ) 
    {
      fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
The SDL AUDIO SUBSYSTEM COULD NOT BE INITIALIZED.\n\
\n\
Please check that your sound card is properly configured,\n\
i.e. if other applications are able to play sounds.\n\
\n\
If you for some reason cannot get your sound card ready, \n\
you can choose to play without sound.\n\
\n\
If you want this, use the appropriate command line option and Freedroid will \n\
not complain any more.  But for now Freedroid will terminate to draw attention \n\
to the sound problem it could not resolve.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" );
      Terminate(ERR);
    } else
      {
	printf("\nSDL Audio initialisation successful.\n");
      }

  // Now that we have initialized the audio SubSystem, we must open
  // an audio channel.  This will be done here (see code from Mixer-Tutorial):

  if ( Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) ) 
    {
      fprintf (stderr,
	       "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
The a SDL AUDIO CHANNEL COULD NOT BE OPEND.\n\
\n\
Please check that your sound card is properly configured,\n\
i.e. if other applications are able to play sounds.\n\
\n\
If you for some reason cannot get your sound card ready, \n\
you can choose to play without sound.\n\
\n\
If you want this, use the appropriate command line option and Freedroid will \n\
not complain any more.  But for now Freedroid will terminate to draw attention \n\
to the sound problem it could not resolve.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" );
      Terminate (ERR);
    }
  else 
    {
      DebugPrintf (1, "\nSuccessfully opened SDL audio channel." );
    }

  //--------------------
  // Now that the audio channel is opend, its time to load all the
  // WAV files into memory, something we NEVER did while using the yiff,
  // because the yiff did all the loading, analyzing and playing...
  //
  Loaded_WAV_Files[0]=NULL;
  for (i = 1; i < ALL_SOUNDS; i++)
    {
      strcpy ( Temp_Filename , "effects/" );
      strcat ( Temp_Filename , SoundSampleFilenames[ i ] );
      fpath = find_file ( Temp_Filename , SOUND_DIR, FALSE);
      Loaded_WAV_Files [ i ] = Mix_LoadWAV ( fpath ) ;
      if ( Loaded_WAV_Files [ i ] == NULL )
	{
	  fprintf (stderr,
		   "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
The SDL MIXER WAS UNABLE TO LOAD A CERTAIN SOUND EFFECT FILE INTO MEMORY.\n\
\n\
The name of the problematic file is:\n\
%s \n\
\n\
If the problem persists and you do not find this sound file in the\n\
Freedroid archive, please inform the developers about the problem.\n\
\n\
In the meantime you can choose to play without sound.\n\
\n\
If you want this, use the appropriate command line option and Freedroid will \n\
not complain any more.  But for now Freedroid will terminate to draw attention \n\
to the sound problem it could not resolve.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" , fpath );
	  Terminate (ERR);
	} // if ( !Loaded_WAV...
      else
	{
	  DebugPrintf (1, "\nSuccessfully loaded file %s.", SoundSampleFilenames[i]);
	}
    } // for (i=0; ...


  
  Loaded_MOD_Files[0]=NULL;
  for (i = 1; i < ALL_MOD_MUSICS; i++)
    {
      strcpy ( Temp_Filename , "music/" );
      strcat ( Temp_Filename , MOD_Music_SampleFilenames[ i ] );
      fpath = find_file ( Temp_Filename , SOUND_DIR, FALSE);
      Loaded_MOD_Files [ i ] = Mix_LoadMUS( fpath );
      if ( Loaded_MOD_Files[ i ] == NULL )
	{
	  fprintf (stderr,
		   "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
The a SDL MIXER WAS UNABLE TO LOAD A CERTAIN MOD FILE INTO MEMORY.\n\
\n\
The name of the problematic file is:\n\
%s \n\
\n\
The SDL says the reason for this would be the following:\n\
%s \n\
\n\
If the problem persists and you do not find this sound file in the\n\
Freedroid archive, please inform the developers about the problem.\n\
\n\
In the meantime you can choose to play without sound.\n\
\n\
If you want this, use the appropriate command line option and Freedroid will \n\
not complain any more.  But for now Freedroid will terminate to draw attention \n\
to the sound problem it could not resolve.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" , fpath , Mix_GetError() );
	  Terminate (ERR);
	} // if ( !Loaded_MOD...
      else
	{
	  DebugPrintf ( 1 , "\nSuccessfully loaded file %s.", MOD_Music_SampleFilenames[ i ]);
	}

    } // for (i=1, ... MOD_FILES...

  //--------------------
  // Now that the music files have been loaded successfully, it's time to set
  // the music and sound volumes accoridingly, i.e. as specifies by the users
  // configuration.
  //
  Set_Sound_FX_Volume( GameConfig.Current_Sound_FX_Volume );

  // DebugPrintf (1, "done.");
  // fflush(stdout);
#endif // HAVE_SDL_MIXER
} // void InitAudio(void)


int i;
unsigned char *ptr;
unsigned char v = 128;
int SampleLaenge;

void 
Set_BG_Music_Volume(float NewVolume)
{

#ifndef HAVE_LIBSDL_MIXER
  return;
#else
  if ( !sound_on ) return;

  // Set the volume IN the loaded files, if SDL is used...
  /*
    That is old and way used, when music was technically still a sound
  for ( i=1 ; i<5 ; i++ )
    {
      Mix_VolumeChunk( Loaded_WAV_Files[i], (int) rintf(NewVolume* MIX_MAX_VOLUME) );
    }
  */

  Mix_VolumeMusic( (int) rintf( NewVolume * MIX_MAX_VOLUME ) );

  // Switch_Background_Music_To ( COMBAT_BACKGROUND_MUSIC_SOUND );
#endif // HAVE_LIBSDL_MIXER
} // void Set_BG_Music_Volume(float NewVolume)

void 
Set_Sound_FX_Volume(float NewVolume)
{
#ifndef HAVE_LIBSDL_MIXER
  return;
#else
  if ( !sound_on ) return;

  // Set the volume IN the loaded files, if SDL is used...
  // This is done here for the Files 1,2,3 and 4, since these
  // are background music files.
  for ( i=5 ; i<ALL_SOUNDS ; i++ )
    {
      Mix_VolumeChunk( Loaded_WAV_Files[i], (int) rintf(NewVolume* MIX_MAX_VOLUME) );
    }

#endif // HAVE_LIBSDL_MIXER

} // void Set_BG_Music_Volume(float NewVolume)


/*@Function============================================================
@Desc: Starts a Tune.

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
StartSound (int Tune)
{

} /* void StartSound(int Tune) */

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
CrySound (void)
{
  Play_Sound ( CRY_SOUND );
}

/* ----------------------------------------------------------------------
 * When an Item is sold, this voice sample will be played...
 * ---------------------------------------------------------------------- */
void
Play_Shop_ItemSoldSound( )
{
  Play_Sound ( SHOP_ITEM_SOLD_SOUND_0 ) ;
};  // void Play_Shop_ItemSoldSound( )

/* ----------------------------------------------------------------------
 * When an Item is bought, this voice sample will be played...
 * ---------------------------------------------------------------------- */
void
Play_Shop_ItemBoughtSound( )
{
  Play_Sound ( SHOP_ITEM_BOUGHT_SOUND_0 ) ;
};  // void Play_Shop_ItemBoughtSound( )

/* ----------------------------------------------------------------------
 * When an Item is repaired, this voice sample will be played...
 * ---------------------------------------------------------------------- */
void
Play_Shop_ItemRepairedSound( )
{
  Play_Sound ( SHOP_ITEM_REPAIRED_SOUND_0 ) ;
};  // void Play_Shop_ItemRepairedSound( )

/* ----------------------------------------------------------------------
 * When an Item is identified, this voice sample will be played...
 * ---------------------------------------------------------------------- */
void
Play_Shop_ItemIdentifiedSound( )
{
  Play_Sound ( SHOP_ITEM_IDENTIFIED_SOUND_0 ) ;
};  // void Play_Shop_ItemIdentifiedSound( )

/* ----------------------------------------------------------------------
 * When a Force-To-Energy Spell is invoked successfully, then this sound
 * should be played...
 * ---------------------------------------------------------------------- */
void
Play_Spell_ForceToEnergy_Sound( )
{
  Play_Sound ( SPELL_FORCETOENERGY_SOUND_0 ) ;
};  // void Play_Spell_ForceToEnergy_Sound( )

/* ----------------------------------------------------------------------
 * When a DetectItems Spell is invoked successfully, then this sound
 * should be played...
 * ---------------------------------------------------------------------- */
void
Play_Spell_DetectItems_Sound( )
{
  Play_Sound ( SPELL_DETECTITEMS_SOUND_0 ) ;
};  // void Play_Spell_ForceToEnergy_Sound( )

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
Not_Enough_Power_Sound ( void )
{
  Play_Sound ( NOT_ENOUGH_POWER_SOUND );
}

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
Not_Enough_Dist_Sound ( void )
{
  Play_Sound ( NOT_ENOUGH_DIST_SOUND );
}

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
Not_Enough_Mana_Sound ( void )
{
  Play_Sound ( NOT_ENOUGH_FORCE_SOUND );
};

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
PlayGreetingSound ( int SoundCode )
{
  switch ( SoundCode )
    {
    case -1:
      return;
      break;
    case 0:
      Play_Sound( FIRST_CONTACT_SOUND_0 );
      break;
    case 1:
      Play_Sound( FIRST_CONTACT_SOUND_1 );
      break;
    default:
      DebugPrintf( 0 , "\nUnknown Greeting sound!!! Terminating...");
      Terminate( ERR );
      break;
    }
}; // void PlayGreetingSound ( int SoundCode )

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
PlayItemSound ( int SoundCode )
{
  switch ( SoundCode )
    {
    case 0:
      Play_Sound( ITEM_DROP_SOUND_0 );
      break;
    case 1:
      Play_Sound( ITEM_DROP_SOUND_1 );
      break;
    case 2:
      Play_Sound( ITEM_DROP_SOUND_2 );
      break;
    case 3:
      Play_Sound( ITEM_DROP_SOUND_3 );
      break;
    case 4:
      Play_Sound( ITEM_DROP_SOUND_4 );
      break;
    case 5:
      Play_Sound( ITEM_ARMOUR_PUT_SOUND );
      break;
    case 6:
      Play_Sound( ITEM_WHEELS_PUT_SOUND );
      break;
    case 7:
      Play_Sound( ITEM_RANGE_WEAPON_PUT_SOUND );
      break;
    default:
      break;
    }
};

void
ItemTakenSound (void)
{
  Play_Sound ( ITEM_TAKEN_SOUND );
}

/* ----------------------------------------------------------------------
 * This function generates a voice output stating that the influencer 
 * can't carry any more right now.  Also this function will see to it,
 * that the sentence is not repeated until 4 seconds after the previous
 * cant-carry-sentence have passed.
 * ---------------------------------------------------------------------- */
void
CantCarrySound (void)
{
  static long CurrentTicks = 0;

  if ( ( SDL_GetTicks() - CurrentTicks ) > 4000 )
    {
      Play_Sound ( CANT_CARRY_SOUND );
      CurrentTicks = SDL_GetTicks();
    }
}; // void CantCarrySound (void)

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
TransferSound (void)
{
  Play_Sound ( TRANSFER_SOUND );
}

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
Mission_Status_Change_Sound (void)
{
  Play_Sound ( MISSION_STATUS_CHANGE_SOUND );
}

/* ----------------------------------------------------------------------
 * This function is intended to provide a convenient way of switching
 * between different backround sounds in freedroid.
 * If no background sound was yet running, the function should start playing
 * the given background music.
 * If some background sound was already playing, the function should shut down
 * the old background music and start playing the new one.
 *
 * ---------------------------------------------------------------------- */
void
Switch_Background_Music_To ( char* filename_raw_parameter )
{
  static int MOD_Music_Channel = -1;
  char* fpath;
  char filename_raw[5000];

#ifndef HAVE_LIBSDL_MIXER
  return;
#else

  if ( !sound_on ) return;


  if ( filename_raw == SILENCE ) // SILENCE is defined as -1 I think
    {
      //printf("\nOld Background music channel has been halted.");
      // fflush(stdout);
      Mix_HaltMusic( ); // this REALLY is a VOID-argument function!!
      MOD_Music_Channel = -1;
      return;
    }

  //--------------------
  // Now we LOAD the music file from disk into memory!!
  // But before we free the old music.  This is not a danger, cause the music
  // is first initialized in Init_Audio with some dummy mod files, so that there
  // is always something allocated, that we can free here.
  //
  // The loading of music and sound files is
  // something that was previously done only in the initialisatzion funtion
  // of the audio thing.  But now we want to allow for dynamic specification of
  // music files via the mission files and that.  So we load the music now.
  //

  if ( Loaded_MOD_Files[ 0 ] != NULL )
    {
      Mix_FreeMusic( Loaded_MOD_Files [ 0 ] );  
      Loaded_MOD_Files [ 0 ] = NULL ;
    }

  strcpy ( filename_raw , "music/" );
  strcat ( filename_raw , filename_raw_parameter );
  fpath = find_file ( filename_raw , SOUND_DIR, FALSE);
  Loaded_MOD_Files [ 0 ] = Mix_LoadMUS( fpath );
  if ( Loaded_MOD_Files[ 0 ] == NULL )
    {
      fprintf (stderr,
	       "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
The a SDL MIXER WAS UNABLE TO LOAD A CERTAIN MOD FILE INTO MEMORY ON THE FLY.\n\
\n\
The name of the problematic file is:\n\
%s \n\
\n\
The SDL says the reason for this would be the following:\n\
%s \n\
\n\
If the problem persists and you do not find this sound file in the\n\
Freedroid archive, please inform the developers about the problem.\n\
\n\
In the meantime you can choose to play without sound.\n\
\n\
If you want this, use the appropriate command line option and Freedroid will \n\
not complain any more.  But for now Freedroid will terminate to draw attention \n\
to the sound problem it could not resolve.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" , fpath , Mix_GetError() );
      Terminate (ERR);
    } // if ( !Loaded_MOD...
  else
    {
      DebugPrintf ( 1 , "\nSuccessfully loaded file %s.", fpath );
    }
  

  // MOD_Music_Channel = Mix_PlayMusic ( Loaded_MOD_Files[ Tune ] , -1 );
  MOD_Music_Channel = Mix_PlayMusic ( Loaded_MOD_Files[ 0 ] , -1 );

  Mix_VolumeMusic ( (int) rintf( GameConfig.Current_BG_Music_Volume * MIX_MAX_VOLUME ) );

#endif // HAVE_LIBSDL_MIXER

}; // void Switch_Background_Music_To(int Tune)


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
Play_Sound (int Tune)
{
#ifndef HAVE_LIBSDL_MIXER
  return;
#else
  int Newest_Sound_Channel=0;

  if ( !sound_on ) return;

  Newest_Sound_Channel = Mix_PlayChannel(-1, Loaded_WAV_Files[Tune] , 0);
  if ( Newest_Sound_Channel == -1 )
    {
      fprintf (stderr,
	       "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
The a SDL MIXER WAS UNABLE TO PLAY A CERTAIN FILE LOADED INTO MEMORY.\n\
\n\
The name of the problematic file is:\n\
%s \n\
\n\
Analysis of the error has returned the following explanation through SDL:\n\
%s \n\
The most likely cause for the problem however is, that too many sounds\n\
have been played in too rapid succession, which should be caught.\n\
If the problem persists, please inform the developers about it.\n\
\n\
In the meantime you can choose to play without sound.\n\
\n\
If you want this, use the appropriate command line option and Freedroid will \n\
not complain any more.  Freedroid will NOT be terminated now to draw attention \n\
to this sound problem, because the problem is not lethal and will not interfere\n\
with game performance in any way.  I think this is really not dangerous.\n\
----------------------------------------------------------------------\n\
\n" , SoundSampleFilenames[ Tune ] , Mix_GetError() );
      // Terminate (ERR);
    } // if ( ... = -1
  else
    {
      DebugPrintf( 2 , "\nSuccessfully playing file %s.", SoundSampleFilenames[ Tune ]);
      // DebugPrintf (1, "\nSuccessfully playing file %s.", SoundSampleFilenames[ Tune ]);
    }

#endif // HAVE_LIBSDL_MIXER
  
}  // void Play_Sound(int Tune)

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
GotHitSound (void)
{
  if (!sound_on) return;

  Play_Sound (GOT_HIT_SOUND);
}				// void GotHitSound(void)


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
Influencer_Scream_Sound (void)
{
  if (!sound_on) return;

  //--------------------
  // For a test, let's try using not my samples but rather this
  // one and only sound sample, formerly used for got-into-blast
  // occasions.
  Play_Sound (GOT_INTO_BLAST_SOUND);
  return;

  switch( MyRandom( 4 ) )
    {
    case 0 :
      Play_Sound ( INFLUENCER_SCREAM_SOUND_3 );
      break;
    case 1 :
      Play_Sound ( INFLUENCER_SCREAM_SOUND_1 );
      break;
    case 2 :
      Play_Sound ( INFLUENCER_SCREAM_SOUND_3 );
      break;
    case 3 :
      Play_Sound ( INFLUENCER_SCREAM_SOUND_3 );
      break;
    case 4 :
      Play_Sound ( INFLUENCER_SCREAM_SOUND_4 );
      break;
    default:
      break;
    }
}; // void Influencer_Scream_Sound (void)


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
GotIntoBlastSound (void)
{
  if (!sound_on) return;

  // Play_Sound (GOT_INTO_BLAST_SOUND);
  return;
}				// void GotIntoBlastSound(void)

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
RefreshSound (void)
{
  if (!sound_on) return;

  Play_Sound (REFRESH_SOUND);
  return;
}				// void RefreshSound(void)


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
MoveLiftSound (void)
{
  if (!sound_on) return;

  Play_Sound (MOVE_ELEVATOR_SOUND);
}				// void MoveLiftSound(void)

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
MenuItemSelectedSound (void)
{
  if (!sound_on) return;

  Play_Sound ( MENU_ITEM_SELECTED_SOUND );

}; // void MenuItemSelectedSound ( void )

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
MenuItemDeselectedSound (void)
{
  if (!sound_on) return;

  Play_Sound ( MENU_ITEM_DESELECTED_SOUND );

}; // void MenuItemSelectedSound ( void )

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
MoveMenuPositionSound (void)
{
  if (!sound_on) return;

  Play_Sound ( MOVE_MENU_POSITION_SOUND );
}				// void MoveLiftSound(void)


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
ThouArtDefeatedSound (void)
{
  if (!sound_on) return;
  Play_Sound (THOU_ART_DEFEATED_SOUND);
}				// void MoveLiftSound(void)


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
EnterLiftSound (void)
{
  if (!sound_on) return;

  Play_Sound (ENTER_ELEVATOR_SOUND);
  return;
}				// void EnterLiftSound(void)


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
LeaveLiftSound (void)
{
  if (!sound_on) return;

  Play_Sound (LEAVE_ELEVATOR_SOUND);

  return;
}				// void LeaveLiftSound(void)


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
Fire_Bullet_Sound (int BulletType)
{
  if (!sound_on) return;

  switch (BulletType)
    {
    case PULSE:
      Play_Sound ( FIRE_BULLET_PULSE_SOUND );
      break;

    case SINGLE_PULSE:
      Play_Sound ( FIRE_BULLET_SINGLE_PULSE_SOUND );
      break;

    case MILITARY:
      Play_Sound ( FIRE_BULLET_MILITARY_SOUND );
      break;

    case FLASH:
      Play_Sound ( FIRE_BULLET_FLASH_SOUND );
      break;

    case EXTERMINATOR:
      Play_Sound ( FIRE_BULLET_EXTERMINATOR_SOUND );
      break;

    case LASER_RIFLE:
      Play_Sound ( FIRE_BULLET_LASER_RIFLE_SOUND );
      break;

    case SINGLE_LASER:
      Play_Sound ( FIRE_BULLET_SINGLE_LASER_SOUND );
      break;

    case PLASMA_PISTOL:
      Play_Sound ( FIRE_BULLET_PLASMA_PISTOL_SOUND );
      break;

    case LASER_SWORD_1:
    case LASER_AXE:
    case LASER_SWORD_2:
    default:
      Play_Sound ( FIRE_BULLET_SWORD_SOUND );
      break;
    }
}; // void FireBulletSound(void)


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
Takeover_Set_Capsule_Sound (void)
{
  if (!sound_on) return;

  Play_Sound (TAKEOVER_SET_CAPSULE_SOUND);

}; // void Takeover_Set_Capsule_Sound ( void )

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
Takeover_Game_Won_Sound (void)
{
  if (!sound_on) return;

  Play_Sound ( TAKEOVER_GAME_WON_SOUND );

}; // void Takeover_Game_Won_Sound ( void ) 

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
Takeover_Game_Deadlock_Sound (void)
{
  if (!sound_on) return;

  Play_Sound ( TAKEOVER_GAME_DEADLOCK_SOUND );

}; // void Takeover_Game_Deadlock_Sound ( void )

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
Takeover_Game_Lost_Sound (void)
{
  if (!sound_on) return;

  Play_Sound ( TAKEOVER_GAME_LOST_SOUND );
}; // void Takeover_Game_Lost_Sound ( void )


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
BounceSound (void)
{
  if (!sound_on) return;

  Play_Sound ( COLLISIONSOUND );

}; // void BounceSound ( void )

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
DruidBlastSound (void)
{
  if (!sound_on) return;

  Play_Sound (BLASTSOUND);

}; // void DruidBlastSound (void)


/* ----------------------------------------------------------------------
 * 
 *
 * ---------------------------------------------------------------------- */
void 
PlayLevelCommentSound ( int levelnum )
{
  switch ( levelnum )
    {
    case 0:
      // I've been away for a far too long time it seems...
      break;
    case 3:
      // I can feel the MS Machines close now!
      Play_Sound ( MS_MACHINES_CLOSE_NOW_SOUND );
      break;
    default: 
      break;
    };
}; // void PlayLevelCommentSound ( int levelnum )

/* ----------------------------------------------------------------------
 * Whenever an enemy is hit by the tux with a melee weapon, then the
 * following sound is played...
 * ---------------------------------------------------------------------- */
void 
PlayEnemyGotHitSound ( int enemytype )
{
  switch ( enemytype )
    {
    case -1:
      // Don't play anything at all...
      break;
    case 0:
      // Play a grunting enemy got hit sound...
      Play_Sound ( ENEMY_GOT_HIT_SOUND_0 );
      break;
    default: 
      break;
    };
}; // void PlayLevelCommentSound ( int levelnum )

/* ----------------------------------------------------------------------
 * This function plays a sound of a bullet being reflected. It is only
 * used, when a bullets is compensated by the tux armour.
 * ---------------------------------------------------------------------- */
void
BulletReflectedSound (void)
{
  if (!sound_on) return;

  Play_Sound ( BULLET_REFLECTED_SOUND);

}; // void DruidBlastSound (void)




#undef _sound_c
