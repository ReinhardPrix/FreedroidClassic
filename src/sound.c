/* 
 *
 *   Copyright (c) 1994, 2002, 2003 Johannes Prix
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

#define ALL_SOUNDS 59
char *SoundSampleFilenames[ALL_SOUNDS] = {
   "ERRORSOUND_NILL.NOWAV",
   "Blast_Sound_0.wav",
   "Collision_Sound_0.wav",
   "GotIntoBlast_Sound_0.wav",
   "MoveElevator_Sound_0.wav",
   "Refresh_Sound_0.wav",
   "LeaveElevator_Sound_0.wav",
   "EnterElevator_Sound_0.wav",
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
   "phaser.wav",
   "Fire_Bullet_Single_Laser_Sound_0.wav",
   "Fire_Bullet_Plasma_Pistol_Sound_0.wav",
   "swing_then_hit_1.wav",
   "swing_then_hit_2.wav",
   "swing_then_hit_3.wav",
   "swing_then_hit_4.wav",
   "swing_then_nohit_1.wav",
   "swing_then_nohit_2.wav",
   "swing_then_nohit_3.wav",
   "swing_then_nohit_4.wav",
   "Bullet_Reflected_Sound_0.wav",
   "Cry_Sound_0.wav",
   "Takeover_Sound_0.wav",
   "Mission_Status_Change_Sound_0.wav",
   "ICantCarryAnyMore_Sound_0.wav",
   "ICantCarryAnyMore_Sound_1.wav",
   "ICantCarryAnyMore_Sound_2.wav",
   "MSMachinesClose_0.wav",
   "bot_sounds/First_Contact_Sound_0.wav",
   "bot_sounds/First_Contact_Sound_1.wav",
   "bot_sounds/First_Contact_Sound_2.wav",
   "bot_sounds/First_Contact_Sound_3.wav",
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
   "No_Ammo_Sound_0.wav",
   "bot_sounds/123_death_sound.wav",
   "bot_sounds/247_death_sound.wav",
   "bot_sounds/302_death_sound.wav"
};

#ifdef HAVE_LIBSDL_MIXER
Mix_Chunk *Loaded_WAV_Files[ALL_SOUNDS];
#endif

#define ALL_MOD_MUSICS 1
char *MOD_Music_SampleFilenames[ALL_MOD_MUSICS] = {
  "ERRORSOUND_NILL.NOMOD"
};

#define MAX_SOUND_CHANNELS 5000

char SoundChannelList[ MAX_SOUND_CHANNELS ];
#ifdef HAVE_LIBSDL_MIXER
Mix_Chunk* List_Of_Sustained_Release_WAV_Files[ MAX_SOUND_CHANNELS ];
Mix_Music *Loaded_MOD_Files[ALL_MOD_MUSICS] =
{
  NULL
};
#endif

enum 
  {
    NOTHING_PLAYING_AT_ALL = 3 ,
    FADING_IN = 4 ,
    FADING_OUT = 5 
  };

char NewMusicTargetFileName[5000];
int BackgroundMusicStateMachineState = NOTHING_PLAYING_AT_ALL ;

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

  DebugPrintf( 1 , "\nCALLBACK FUNCTION INVOKED:  channel %d finished playback.\n" , channel );

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
      DebugPrintf( 1 , "\nCALLBACK FUNCTION:  Detected soundchannel for sustained release.... freeing chunk..." );
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
PlayOnceNeededSoundSample( char* SoundSampleFileName , int With_Waiting , int no_double_catching ) 
{
  //--------------------
  // These variables will always be needed!
  //
  int simulated_playback_starting_time;
  static char PreviousFileName [1000]="HalloHallo";
  static Uint32 PreviousStartTicks = 0 ;
  Uint32 TicksNow;

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
  // Some voice samples need to be played rather fast, but still
  // they are a bit config-file dependent, so we check for some
  // special file names we know and handle these differenty...
  //
  if ( ! strcmp ( "../effects/bot_sounds/123_death_sound.wav" , SoundSampleFileName ) )
    {
      PlaySound ( DEATH_SOUND_123 );
      DebugPrintf ( 1 , "\nPrematurely caught 123 death sound for safe playing..." );
      return;
    }
  if ( ! strcmp ( "../effects/bot_sounds/247_death_sound.wav" , SoundSampleFileName ) )
    {
      PlaySound ( DEATH_SOUND_247 );
      DebugPrintf ( 1 , "\nPrematurely caught 247 death sound for safe playing..." );
      return;
    }
  if ( ! strcmp ( "../effects/bot_sounds/302_death_sound.wav" , SoundSampleFileName ) )
    {
      PlaySound ( DEATH_SOUND_302 );
      DebugPrintf ( 1 , "\nPrematurely caught 302 death sound for safe playing..." );
      return;
    }


  //--------------------
  // In case the same sample is played again and again in a very
  // short time, we might refuse operation here, since this could
  // lead to non-loadability errors with the sound files.
  //
  TicksNow = SDL_GetTicks();
  if ( ( strcmp ( "Sorry_No_Voice_Sample_Yet_0.wav" , SoundSampleFileName ) ) && ( ! strcmp ( PreviousFileName , SoundSampleFileName ) ) && ( ( TicksNow - PreviousStartTicks ) < 2.5 * 1000 ) && ( ! no_double_catching ) ) return;

  //--------------------
  // For now, we disable the bombardment with 'no voice sample yet...'
  //
  // if ( ! strcmp ( "Sorry_No_Voice_Sample_Yet_0.wav" , SoundSampleFileName ) ) return;

  PreviousStartTicks = TicksNow;
  strcpy ( PreviousFileName , SoundSampleFileName );

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
      
      if ( EscapePressed() ) while ( EscapePressed() );
      if ( SpacePressed() ) while ( SpacePressed() );
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
	  
	  if ( EscapePressed() ) while ( EscapePressed() );
	  if ( SpacePressed() ) while ( SpacePressed() );
	}

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

      fprintf( stderr, "\n\nfpath: '%s'\n" , fpath );
      GiveStandardErrorMessage ( "PlayOnceNeededSoundSample(...)" , "\
The SDL MIXER WAS UNABLE TO LOAD A CERTAIN SOUND FILE INTO MEMORY.\n\
The reason for this is as follows:  Speech files are stored in wav format\n\
for technical reasons in conjunction with the SDL and the background music.\n\
This tends to use up much space on disk, i.e. several megabytes which would\n\
make the archive a bit large for download via 56K modem.  Therefore not all\n\
sound samples featuring dialog speeches are in the freedroid repository.\n\
\n\
But to ensure smooth gameplay even with missing sound files, there is an option\n\
to have freedroid either ignore the missing dialog sound samples or to terminate\n\
on encountering a missing sound sample.  According to this option, Freedroid will\n\
either terminate or continue running now.",
				 NO_NEED_TO_INFORM, GameConfig.terminate_on_missing_speech_sample );

      if ( !GameConfig.terminate_on_missing_speech_sample )
	{
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
      DebugPrintf ( 1 , "\nSuccessfully loaded file %s into memory for playing once, filename is %s ." , 
		    SoundSampleFileName , fpath );
    }

  //--------------------
  // Hoping, that this will not take up too much processor speed, we'll
  // now change the volume of the sound sample in question to what is normal
  // for sound effects right now...
  //
  Mix_VolumeChunk( One_Shot_WAV_File , (int) rintf( GameConfig.Current_Sound_FX_Volume * MIX_MAX_VOLUME ) );

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
      fprintf( stderr, "\n\nSoundSampleFileName: '%s' Mix_GetError(): %s \n" , SoundSampleFileName , Mix_GetError() );
      GiveStandardErrorMessage ( "PlayOnceNeededSoundSample(...)" , "\
The SDL MIXER WAS UNABLE TO PLAY A CERTAIN FILE LOADED INTO MEMORY FOR PLAYING ONCE.\n",
				 PLEASE_INFORM, IS_WARNING_ONLY );

      //--------------------
      // If we receive an error playing a sound file here, this is very inconvenient.
      // We must see to it that the callback code and allocation there and all that doesn't
      // get touched.  I hope that the following fix does already what we want here...
      // But it should :->
      //
      Mix_FreeChunk ( One_Shot_WAV_File );
      return;

    } // if ( ... = -1
  else
    {
      SoundChannelList[ Newest_Sound_Channel ] = 1;
      DebugPrintf( 1 , "\nSuccessfully playing the 'ONCE NEEDED' file %s.", SoundSampleFileName ) ;
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

}; // void PlayOnceNeededSoundSample( char* SoundSampleFileName , int With_Waiting) 

/* ---------------------------------------------------------------------- 
 *
 *
 * ---------------------------------------------------------------------- */
void
LoadAllStaticWavFiles( void )
{
#ifndef HAVE_LIBSDL_MIXER  
  return;
#else
  char *fpath;
  int i;
  char Temp_Filename[5000];

  if ( !sound_on ) return;

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
	  DebugPrintf (-1, "Sound sample %s not found. \n\
Will continue with sound disabled.\n\
If you want sound, please install the sound-samples.\n", Temp_Filename);
	  sound_on = FALSE; // simply switch off sound
	  return;
	} // if ( !Loaded_WAV...
      else
	{
	  DebugPrintf (1, "\nSuccessfully loaded file %s.", SoundSampleFilenames[i]);
	}
    } // for (i=0; ...

#endif // HAVE_SDL_MIXER

}; // void LoadAllStaticWavFiles( void )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
LoadAllStaticModFiles( void )
{
#ifndef HAVE_LIBSDL_MIXER  
  return;
#else
  char *fpath;
  int i;
  char Temp_Filename[5000];

  if ( !sound_on ) return;

  Loaded_MOD_Files[0]=NULL;
  for (i = 1; i < ALL_MOD_MUSICS; i++)
    {
      strcpy ( Temp_Filename , "music/" );
      strcat ( Temp_Filename , MOD_Music_SampleFilenames[ i ] );
      fpath = find_file ( Temp_Filename , SOUND_DIR, FALSE);
      Loaded_MOD_Files [ i ] = Mix_LoadMUS( fpath );
      if ( Loaded_MOD_Files[ i ] == NULL )
	{
	  fprintf (stderr, "\n\nfpath: '%s' Mix_GetError(): %s.\n" , fpath , Mix_GetError() );
	  GiveStandardErrorMessage ( "InitAudio(...)" , "\
The SDL MIXER WAS UNABLE TO LOAD A CERTAIN MOD FILE INTO MEMORY.\n\
\n\
Please check that the file is present within your Freedroid installation.\n\
\n\
If you for some reason cannot get sound output ready, \n\
you can choose to play without sound.\n\
\n\
If you want this, use the appropriate command line option and Freedroid will \n\
not complain any more.",
				     NO_NEED_TO_INFORM, IS_FATAL );
	} // if ( !Loaded_MOD...
      else
	{
	  DebugPrintf ( 1 , "\nSuccessfully loaded file %s.", MOD_Music_SampleFilenames[ i ]);
	}

    } // for (i=1, ... MOD_FILES...

#endif // HAVE_SDL_MIXER

}; // void LoadAllStaticModFiles( void )

// ----------------------------------------------------------------------
// This function shall initialize the SDL Audio subsystem.  It is called
// as soon as Freedroid is started.  It does ONLY work with SDL and no
// longer with any form of sound engine like the YIFF.
// ----------------------------------------------------------------------
void 
InitAudio(void)
{
#ifndef HAVE_LIBSDL_MIXER  
  return;
#else
  int audio_rate = 22050;
  Uint16 audio_format = AUDIO_S16; 
  int audio_channels = 2;
  //  int audio_buffers = 4096;
  int audio_buffers = 2048;

  DebugPrintf(1, "\nInitializing SDL Audio Systems....\n");

  if ( !sound_on ) return;

  // Now SDL_AUDIO is initialized here:

  if ( SDL_InitSubSystem ( SDL_INIT_AUDIO ) == -1 ) 
    {
      GiveStandardErrorMessage ( "InitAudio(...)" , "\
The SDL AUDIO SUBSYSTEM COULD NOT BE INITIALIZED.\n\
\n\
Please check that your sound card is properly configured,\n\
i.e. if other applications are able to play sounds.\n\
\n\
If you for some reason cannot get your sound card ready, \n\
you can choose to play without sound.\n\
\n\
If you want this, use the appropriate command line option and Freedroid will \n\
not complain any more.",
				 NO_NEED_TO_INFORM, IS_FATAL );
    } 
  else
    {
      DebugPrintf ( 1 , "\nSDL Audio initialisation successful.\n");
    }

  // Now that we have initialized the audio SubSystem, we must open
  // an audio channel.  This will be done here (see code from Mixer-Tutorial):

  if ( Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) ) 
    {
      GiveStandardErrorMessage ( "InitAudio(...)" , "\
The SDL AUDIO CHANNEL COULD NOT BE OPEND.\n\
\n\
Please check that your sound card is properly configured,\n\
i.e. if other applications are able to play sounds.\n\
\n\
If you for some reason cannot get your sound card ready, \n\
you can choose to play without sound.\n\
\n\
If you want this, use the appropriate command line option and Freedroid will \n\
not complain any more.",
				 NO_NEED_TO_INFORM, IS_FATAL );
    }
  else 
    {
      DebugPrintf (1, "\nSuccessfully opened SDL audio channel." );
    }

  //--------------------
  // Since we don't want some sounds to be omitted due to lack of mixing
  // channels, we select to have some at our disposal.  The SDL will do this
  // for a small increase in memory appetite as the price.  Whether this will
  // really resolve the problem however is unsure.
  //
  DebugPrintf( 1 , "\nChannels allocated: %d. " , Mix_AllocateChannels( 200 ) );

#endif // HAVE_SDL_MIXER
}; // void InitAudio(void)


int i;
unsigned char *ptr;
unsigned char v = 128;
int SampleLaenge;

void 
SetBGMusicVolume(float NewVolume)
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

  // SwitchBackgroundMusicTo ( COMBAT_BACKGROUND_MUSIC_SOUND );
#endif // HAVE_LIBSDL_MIXER
} // void SetBGMusicVolume(float NewVolume)

void 
SetSoundFXVolume(float NewVolume)
{
#ifndef HAVE_LIBSDL_MIXER
  return;
#else
  if ( !sound_on ) return;

  // Set the volume IN the loaded files, if SDL is used...
  // This is done here for the Files 1,2,3 and 4, since these
  // are background music files.
  for ( i=1 ; i<ALL_SOUNDS ; i++ )
    {
      Mix_VolumeChunk( Loaded_WAV_Files[i], (int) rintf(NewVolume* MIX_MAX_VOLUME) );
    }

#endif // HAVE_LIBSDL_MIXER

} // void SetBGMusicVolume(float NewVolume)

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
CrySound (void)
{
  PlaySound ( CRY_SOUND );
}

/* ----------------------------------------------------------------------
 * When a chest is entered (for the chest take/put dialog) or when a
 * chest is just looted by left-click, we always play this sound...
 * ---------------------------------------------------------------------- */
void
play_open_chest_sound( void )
{
  PlayOnceNeededSoundSample ( "../effects/enter_chest_sound.wav" , FALSE , TRUE );
}; // void play_open_chest_sound( void )

/* ----------------------------------------------------------------------
 * When a Force-To-Energy Spell is invoked successfully, then this sound
 * should be played...
 * ---------------------------------------------------------------------- */
void
Play_Spell_ForceToEnergy_Sound( )
{
  PlaySound ( SPELL_FORCETOENERGY_SOUND_0 ) ;
};  // void Play_Spell_ForceToEnergy_Sound( )

/* ----------------------------------------------------------------------
 * When a DetectItems Spell is invoked successfully, then this sound
 * should be played...
 * ---------------------------------------------------------------------- */
void
Play_Spell_DetectItems_Sound( )
{
  PlaySound ( SPELL_DETECTITEMS_SOUND_0 ) ;
};  // void Play_Spell_ForceToEnergy_Sound( )

/* ----------------------------------------------------------------------
 * This function plays a voice sample, stating that not enough power
 * (strength) is available to use a certain item.
 * The sample must of course only be played, if it hasn't been played just
 * milliseconds before, so a check is made to see that the file is played
 * with at least a certain interval in between to the last occasion of the
 * file being played.
 * ---------------------------------------------------------------------- */
void
No_Ammo_Sound ( void )
{
  static Uint32 PreviousSound = (-1) ;
  Uint32 now;

  now = SDL_GetTicks() ;
  if ( SDL_GetTicks() - PreviousSound >= 0.25 * 1000 )
    {
      PlaySound ( NO_AMMO_SOUND_0 );
      PreviousSound = now;
    }
}; // void Not_Enough_Power_Sound ( void )

/* ----------------------------------------------------------------------
 * This function plays a voice sample, stating that not enough power
 * (strength) is available to use a certain item.
 * The sample must of course only be played, if it hasn't been played just
 * milliseconds before, so a check is made to see that the file is played
 * with at least a certain interval in between to the last occasion of the
 * file being played.
 * ---------------------------------------------------------------------- */
void
Not_Enough_Power_Sound ( void )
{
  static Uint32 PreviousSound = (-1) ;
  Uint32 now;

  now = SDL_GetTicks() ;
  if ( SDL_GetTicks() - PreviousSound >= 1.15 * 1000 )
    {
      PlaySound ( NOT_ENOUGH_POWER_SOUND );
      PreviousSound = now;
    }
}; // void Not_Enough_Power_Sound ( void )

/* ----------------------------------------------------------------------
 * This function plays a voice sample, stating that not enough power
 * distribution (dexterity) is available to use a certain item.
 * The sample must of course only be played, if it hasn't been played just
 * milliseconds before, so a check is made to see that the file is played
 * with at least a certain interval in between to the last occasion of the
 * file being played.
 * ---------------------------------------------------------------------- */
void
Not_Enough_Dist_Sound ( void )
{
  static Uint32 PreviousSound = (-1) ;
  Uint32 now;

  now = SDL_GetTicks() ;
  if ( SDL_GetTicks() - PreviousSound >= 1.15 * 1000 )
    {
      PlaySound ( NOT_ENOUGH_DIST_SOUND );
      PreviousSound = now;
    }
}

/* ----------------------------------------------------------------------
 * This function plays a voice sample, stating that not enough magical
 * energy (force) is available to cast a certain spell.
 * The sample must of course only be played, if it hasn't been played just
 * milliseconds before, so a check is made to see that the file is played
 * with at least a certain interval in between to the last occasion of the
 * file being played.
 * ---------------------------------------------------------------------- */
void
Not_Enough_Mana_Sound ( void )
{

  static Uint32 PreviousNotEnoughForceSound = (-1) ;
  Uint32 now;

  now = SDL_GetTicks() ;
  if ( SDL_GetTicks() - PreviousNotEnoughForceSound >= 1.15 * 1000 )
    {
      PlaySound ( NOT_ENOUGH_FORCE_SOUND );
      PreviousNotEnoughForceSound = now;
    }

}; // void Not_Enough_Mana_Sound ( void )

/* ----------------------------------------------------------------------
 * Whenever the Tux meets someone in the game for the very first time, 
 * this enemy or friend will issue the first-time greeting.
 * Depending on whether that is typically a hostile unit or not, we play
 * the corresponding sound from preloaded memory or, if there is no combat
 * to expect, by loading it freshly from disk and then forgeting about it
 * again.
 * ---------------------------------------------------------------------- */
void
PlayGreetingSound ( int SoundCode )
{
  switch ( SoundCode )
    {
    case -1:
      return;
      break;
    case 0:
      PlaySound( FIRST_CONTACT_SOUND_0 );
      break;
    case 1:
      PlaySound( FIRST_CONTACT_SOUND_1 );
      break;
    case 2:
      PlaySound( FIRST_CONTACT_SOUND_2 );
      break;
    case 3:
      PlaySound( FIRST_CONTACT_SOUND_3 );  // the dark apprentice.
      break;
    case 4:
      PlayOnceNeededSoundSample ( "../effects/bot_sounds/First_Contact_Sound_4.wav" , FALSE , FALSE );
      break;
    case 5:
      PlayOnceNeededSoundSample ( "../effects/bot_sounds/First_Contact_Sound_5.wav" , FALSE , FALSE );
      break;
    case 6:
      PlayOnceNeededSoundSample ( "../effects/bot_sounds/First_Contact_Sound_6.wav" , FALSE , FALSE );
      break;
    case 7:
      PlayOnceNeededSoundSample ( "../effects/bot_sounds/First_Contact_Sound_7.wav" , FALSE , FALSE );
      break;
    case 8:
      PlayOnceNeededSoundSample ( "../effects/bot_sounds/First_Contact_Sound_8.wav" , FALSE , FALSE );
      break;
    case 9:
      PlayOnceNeededSoundSample ( "../effects/bot_sounds/First_Contact_Sound_9.wav" , FALSE , FALSE );
      break;
    case 10:
      PlayOnceNeededSoundSample ( "../effects/bot_sounds/First_Contact_Sound_10.wav" , FALSE , FALSE );
      break;
    case 11:
      PlayOnceNeededSoundSample ( "../effects/bot_sounds/First_Contact_Sound_11.wav" , FALSE , FALSE );
      break;
    case 12:
      PlayOnceNeededSoundSample ( "../effects/bot_sounds/First_Contact_Sound_12.wav" , FALSE , FALSE );
      break;
    case 13:
      PlayOnceNeededSoundSample ( "../effects/bot_sounds/First_Contact_Sound_13.wav" , FALSE , FALSE );
      break;
    case 14:
      PlayOnceNeededSoundSample ( "../effects/bot_sounds/First_Contact_Sound_14.wav" , FALSE , FALSE );
      break;
    case 15:
      PlayOnceNeededSoundSample ( "../effects/bot_sounds/First_Contact_Sound_15.wav" , FALSE , FALSE );
      break;
    case 16:
      PlayOnceNeededSoundSample ( "../effects/bot_sounds/First_Contact_Sound_16.wav" , FALSE , FALSE );
      break;
    case 17:
      PlayOnceNeededSoundSample ( "../effects/bot_sounds/First_Contact_Sound_17.wav" , FALSE , FALSE );
      break;
    case 18:
      PlayOnceNeededSoundSample ( "../effects/bot_sounds/First_Contact_Sound_18.wav" , FALSE , FALSE );
      break;
    default:
      DebugPrintf( 0 , "\nUnknown Greeting sound!!! Terminating...");
      Terminate( ERR );
      break;
    }
}; // void PlayGreetingSound ( int SoundCode )


/* ----------------------------------------------------------------------
 * Whenever a bot dies, that should create a dying sound.  But so far,
 * this will be done only for fully animated bots, since the other bots
 * just explode and that has a sound of it's own.
 * ---------------------------------------------------------------------- */
void
play_death_sound_for_bot ( enemy* ThisRobot )
{
  char filename[5000];

  //--------------------
  // If the keyword 'none' for the death sound file name is encountered,
  // nothing will be done...
  //.
  if ( ! strcmp ( Druidmap [ ThisRobot -> type ] . droid_death_sound_file_name , "none" ) )
    return;

  //--------------------
  // Now we play the given death sound, looking for the file in the
  // appropriate sound folder.
  //
  strcpy ( filename , "../effects/bot_sounds/" );
  strcat ( filename , Druidmap [ ThisRobot -> type ] . droid_death_sound_file_name );
  PlayOnceNeededSoundSample ( filename , FALSE , FALSE );

}; // void play_death_sound_for_bot ( enemy* ThisRobot )

/* ----------------------------------------------------------------------
 * Whenever a bot dies, that should create a dying sound.  But so far,
 * this will be done only for fully animated bots, since the other bots
 * just explode and that has a sound of it's own.
 * ---------------------------------------------------------------------- */
void
play_attack_animation_sound_for_bot ( enemy* ThisRobot )
{
  char filename[5000];

  //--------------------
  // If the keyword 'none' for the death sound file name is encountered,
  // nothing will be done...
  //
  if ( ! strcmp ( Druidmap [ ThisRobot -> type ] . droid_attack_animation_sound_file_name , "none" ) )
    return;

  //--------------------
  // Now we play the given death sound, looking for the file in the
  // appropriate sound folder.
  //
  strcpy ( filename , "../effects/bot_sounds/" );
  strcat ( filename , Druidmap [ ThisRobot -> type ] . droid_attack_animation_sound_file_name );
  PlayOnceNeededSoundSample ( filename , FALSE , FALSE );

}; // play_attack_animation_sound_for_bot ( enemy* ThisRobot )


/* ----------------------------------------------------------------------
 * Whenever a bot starts to attack the Tux, he'll issue the attack cry.
 * This is done here, and no respect to loading time issues for now...
 * ---------------------------------------------------------------------- */
void
play_enter_attack_run_state_sound ( int SoundCode )
{
  switch ( SoundCode )
    {
    case -1:
      return;
      break;
    case 0:
      PlayOnceNeededSoundSample ( "../effects/bot_sounds/Start_Attack_Sound_0.wav" , FALSE , FALSE );
      break;
    case 1:
      PlayOnceNeededSoundSample ( "../effects/bot_sounds/Start_Attack_Sound_1.wav" , FALSE , FALSE );
      break;
    case 2:
      PlayOnceNeededSoundSample ( "../effects/bot_sounds/Start_Attack_Sound_2.wav" , FALSE , FALSE );
      break;
    case 9:
      PlayOnceNeededSoundSample ( "../effects/bot_sounds/Start_Attack_Sound_9.wav" , FALSE , FALSE );
      break;
    case 10:
      PlayOnceNeededSoundSample ( "../effects/bot_sounds/Start_Attack_Sound_10.wav" , FALSE , FALSE );
      break;
    case 11:
      PlayOnceNeededSoundSample ( "../effects/bot_sounds/Start_Attack_Sound_11.wav" , FALSE , FALSE );
      break;
    case 12:
      PlayOnceNeededSoundSample ( "../effects/bot_sounds/Start_Attack_Sound_12.wav" , FALSE , FALSE );
      break;
    case 13:
      PlayOnceNeededSoundSample ( "../effects/bot_sounds/Start_Attack_Sound_13.wav" , FALSE , FALSE );
      break;
    case 14:
      PlayOnceNeededSoundSample ( "../effects/bot_sounds/Start_Attack_Sound_14.wav" , FALSE , FALSE );
      break;
    case 15:
      PlayOnceNeededSoundSample ( "../effects/bot_sounds/Start_Attack_Sound_15.wav" , FALSE , FALSE );
      break;
    case 16:
      PlayOnceNeededSoundSample ( "../effects/bot_sounds/Start_Attack_Sound_16.wav" , FALSE , FALSE );
      break;
    case 17:
      PlayOnceNeededSoundSample ( "../effects/bot_sounds/Start_Attack_Sound_17.wav" , FALSE , FALSE );
      break;
    case 18:
      PlayOnceNeededSoundSample ( "../effects/bot_sounds/Start_Attack_Sound_18.wav" , FALSE , FALSE );
      break;
    default:
      DebugPrintf( 0 , "\nUnknown Start Attack sound!!! NOT TERMINATING CAUSE OF THIS...");
      // Terminate( ERR );
      break;
    }
}; // void PlayStartAttackSound ( int SoundCode )

/* ----------------------------------------------------------------------
 * Whenever an item is placed or taken, we'll issue a sound attached to
 * that item. 
 * ---------------------------------------------------------------------- */
void
play_item_sound ( int item_type )
{
  char final_file_name[5000];
  //--------------------
  // First some sanity check...
  //
  if ( item_type < 0 )
    {
      fprintf( stderr, "\n\nitem_type %d \n" , item_type );
      GiveStandardErrorMessage ( "play_item_sound(...)" , "\
negative item type received!\n",
				 PLEASE_INFORM, IS_FATAL );
    }

  //--------------------
  // Now we can proceed and just play the sound associated with that item...
  //
  strcpy ( final_file_name , "../effects/item_sounds/" );
  strcat ( final_file_name , ItemMap [ item_type ] . item_drop_sound_file_name ); 
  PlayOnceNeededSoundSample ( final_file_name , FALSE , TRUE );

}; // void play_item_sound ( int item_type )

/* ----------------------------------------------------------------------
 * This function generates a voice output stating that the influencer 
 * can't carry any more right now.  Also this function will see to it,
 * that the sentence is not repeated until 4 seconds after the previous
 * cant-carry-sentence have passed.
 * ---------------------------------------------------------------------- */
void
CantCarrySound (void)
{
  static Uint32 CurrentTicks = 0;

  if ( ( SDL_GetTicks() - CurrentTicks ) > 2 * 1000 )
    {
      CurrentTicks = SDL_GetTicks();
      switch( MyRandom( 2 ) )
	{
	case 0 :
	  PlaySound ( CANT_CARRY_SOUND_0 );
	  break;
	case 1 :
	  PlaySound ( CANT_CARRY_SOUND_1 );
	  break;
	case 2 :
	  PlaySound ( CANT_CARRY_SOUND_2 );
	  break;
	default:
	  break;
	}
    }
}; // void CantCarrySound (void)

/* ---------------------------------------------------------------------- 
 *
 *
 * ---------------------------------------------------------------------- */
void
TransferSound (void)
{
  PlaySound ( TRANSFER_SOUND );
}; // void TransferSound (void)

/* ---------------------------------------------------------------------- 
 *
 *
 * ---------------------------------------------------------------------- */
void
Mission_Status_Change_Sound (void)
{
  PlaySound ( MISSION_STATUS_CHANGE_SOUND );
}; // void Mission_Status_Change_Sound (void)

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
LoadAndFadeInBackgroundMusic ( void )
{
#ifndef HAVE_LIBSDL_MIXER
  return;
#else

  static int MOD_Music_Channel = -1;
  char* fpath;
  char filename_raw[5000];

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
  // is first initialized in InitAudio with some dummy mod files, so that there
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
  strcat ( filename_raw , NewMusicTargetFileName );
  fpath = find_file_for_callbacks ( filename_raw , SOUND_DIR, FALSE);
  Loaded_MOD_Files [ 0 ] = Mix_LoadMUS( fpath );
  if ( Loaded_MOD_Files[ 0 ] == NULL )
    {
      DebugPrintf (0, "The music file %s could not be loaded!\n" , NewMusicTargetFileName );
      return;
    }
  else
    DebugPrintf ( 1 , "\nSuccessfully loaded file %s.", fpath );

  // MOD_Music_Channel = Mix_PlayMusic ( Loaded_MOD_Files[ Tune ] , -1 );
  // MOD_Music_Channel = Mix_PlayMusic ( Loaded_MOD_Files[ 0 ] , -1 );
  MOD_Music_Channel = Mix_FadeInMusic ( Loaded_MOD_Files[ 0 ] , -1 , 5000 );

  Mix_VolumeMusic ( (int) rintf( GameConfig.Current_BG_Music_Volume * MIX_MAX_VOLUME ) );

#endif // HAVE_LIBSDL_MIXER
}; // void LoadAndFadeInBackgroundMusic ( void )

/* ----------------------------------------------------------------------
 * This function will be invoked by the callback of the SDL mixer when
 * the fading out effect with the background music is completed.
 * ---------------------------------------------------------------------- */
void OldMusicHasFinishedFadingOut ( void )
{

  DebugPrintf( 1 , "\nOld music has now completely faded out and stopped... Can proceed to fade in new music... \n");
  LoadAndFadeInBackgroundMusic (  );

}; // void OldMusicHasFinishedFadingOut ( void )


/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
SwitchBackgroundMusicTo ( char* filename_raw_parameter )
{

#ifdef HAVE_LIBSDL_MIXER

  static char PreviousFileParameter[5000]="NONE_AT_ALL";

  if ( !sound_on ) return;

  //--------------------
  // Maybe the background music switch command given instructs us to initiate
  // the same background music that has been playing all the while anyway in 
  // an endless loop.  So in this case, we need not touch anything at all and
  // just return.
  //
  if ( !strcmp ( PreviousFileParameter , filename_raw_parameter ) )
    {
      DebugPrintf ( 0 , "\nSwitch BG music instruction just repeats running music... nothing done.\n");
      return;
    }
  else
    {
      strcpy ( PreviousFileParameter , filename_raw_parameter );
    }

  strcpy ( NewMusicTargetFileName , filename_raw_parameter );


  //--------------------
  // Now we can start to get some new music going, either directly or
  // by issuing a fade out instruction, that will then launch a callback
  // to get the new music going...
  //

  if ( BackgroundMusicStateMachineState == NOTHING_PLAYING_AT_ALL )
    {
      LoadAndFadeInBackgroundMusic (    );
      BackgroundMusicStateMachineState = FADING_IN ;
    }
  else
    {
      Mix_FadeOutMusic( 5000 );
      BackgroundMusicStateMachineState = FADING_OUT ;
      
      //--------------------
      // We set up a function to be invoked by the SDL automatically as
      // soon as the fading out effect is completed (and the new music
      // can start to get going that is...)
      //
      Mix_HookMusicFinished( OldMusicHasFinishedFadingOut );

    }

#endif

}; // void SwitchBackgroundMusicTo ( char* filename_raw_parameter )


/* ---------------------------------------------------------------------- 
 *
 *
 * ---------------------------------------------------------------------- */
void
PlaySound (int Tune)
{
#ifndef HAVE_LIBSDL_MIXER
  return;
#else
  int Newest_Sound_Channel=0;

  if ( !sound_on ) return;

  Newest_Sound_Channel = Mix_PlayChannel(-1, Loaded_WAV_Files[Tune] , 0);
  if ( Newest_Sound_Channel == -1 )
    {
      fprintf (stderr, "\n\nSoundSampleFilenames[ Tune ]: '%s' Mix_GetError(): %s.\n" , SoundSampleFilenames[ Tune ] , Mix_GetError() );
      GiveStandardErrorMessage ( "PlaySound(...)" , "\
The SDL MIXER WAS UNABLE TO PLAY A CERTAIN FILE LOADED INTO MEMORY.\n\
The most likely cause for the problem however is, that too many sounds\n\
have been played in too rapid succession, which should be caught.\n\
If the problem persists, please inform the developers about it.\n\
\n\
In the meantime you can choose to play without sound.\n\
\n\
If you want this, use the appropriate command line option and Freedroid will \n\
not complain any more.  Freedroid will NOT be terminated now to draw attention \n\
to this sound problem, because the problem is not lethal and will not interfere\n\
with game performance in any way.  I think this is really not dangerous.",
				 NO_NEED_TO_INFORM, IS_WARNING_ONLY );
    } // if ( ... = -1
  else
    {
      DebugPrintf( 2 , "\nSuccessfully playing file %s.", SoundSampleFilenames[ Tune ]);
      // DebugPrintf (1, "\nSuccessfully playing file %s.", SoundSampleFilenames[ Tune ]);
    }

#endif // HAVE_LIBSDL_MIXER
  
}  // void PlaySound(int Tune)

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
GotHitSound (void)
{
  if (!sound_on) return;

  PlaySound (GOT_HIT_SOUND);
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
  // PlaySound (GOT_INTO_BLAST_SOUND);
  // return;

  switch( MyRandom( 4 ) )
    {
    case 0 :
      PlaySound ( INFLUENCER_SCREAM_SOUND_3 );
      break;
    case 1 :
      PlaySound ( INFLUENCER_SCREAM_SOUND_1 );
      break;
    case 2 :
      PlaySound ( INFLUENCER_SCREAM_SOUND_3 );
      break;
    case 3 :
      PlaySound ( INFLUENCER_SCREAM_SOUND_3 );
      break;
    case 4 :
      PlaySound ( INFLUENCER_SCREAM_SOUND_4 );
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

  // PlaySound (GOT_INTO_BLAST_SOUND);
  return;
}				// void GotIntoBlastSound(void)

/* ---------------------------------------------------------------------- 
 *
 * ---------------------------------------------------------------------- */
void
RefreshSound (void)
{
  if (!sound_on) return;

  // PlaySound (REFRESH_SOUND);
  return;
}; // void RefreshSound(void)


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
MoveLiftSound (void)
{
  if (!sound_on) return;

  PlaySound (MOVE_ELEVATOR_SOUND);
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

  PlaySound ( MENU_ITEM_SELECTED_SOUND );

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

  PlaySound ( MENU_ITEM_DESELECTED_SOUND );

}; // void MenuItemSelectedSound ( void )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
MoveMenuPositionSound (void)
{
  if (!sound_on) return;
  PlaySound ( MOVE_MENU_POSITION_SOUND );
}; // void MoveMenuPositionSound (void)


/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
ThouArtDefeatedSound (void)
{
  if (!sound_on) return;
  PlayOnceNeededSoundSample ( "../effects/ThouArtDefeated_Sound_0.wav" , FALSE , FALSE );
}; // void ThouArtDefeatedSound (void)


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
EnterLiftSound (void)
{
  if (!sound_on) return;

  PlaySound (ENTER_ELEVATOR_SOUND);
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

  PlaySound (LEAVE_ELEVATOR_SOUND);

  return;
}; // void LeaveLiftSound(void)

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
play_melee_weapon_hit_something_sound ( void )
{
  switch( MyRandom( 3 ) )
    {
    case 0 :
      PlaySound ( SWING_THEN_HIT_2_SOUND ) ;
      break;
    case 1 :
      PlaySound ( SWING_THEN_HIT_1_SOUND ) ;
      break;
    case 2 :
      PlaySound ( SWING_THEN_HIT_3_SOUND ) ;
      break;
    case 3 :
      PlaySound ( SWING_THEN_HIT_4_SOUND ) ;
      break;
    default:
      break;
    }
}; // void play_melee_weapon_hit_something_sound ( void )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
play_melee_weapon_missed_sound ( void )
{
  switch( MyRandom( 3 ) )
    {
    case 0 :
      PlaySound ( SWING_THEN_NOHIT_1_SOUND ) ;
      break;
    case 1 :
      PlaySound ( SWING_THEN_NOHIT_2_SOUND ) ;
      break;
    case 2 :
      PlaySound ( SWING_THEN_NOHIT_3_SOUND ) ;
      break;
    case 3 :
      PlaySound ( SWING_THEN_NOHIT_4_SOUND ) ;
      break;
    default:
      break;
    }
}; // void play_melee_weapon_missed_sound ( void )

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
Fire_Bullet_Sound (int BulletType)
{
  if (!sound_on) return;

  DebugPrintf ( -10, "\nFireBulletSound called..." );

  switch (BulletType)
    {
    case PULSE:
      PlaySound ( FIRE_BULLET_PULSE_SOUND );
      break;

    case SINGLE_PULSE:
      PlaySound ( FIRE_BULLET_SINGLE_PULSE_SOUND );
      break;

    case MILITARY:
      PlaySound ( FIRE_BULLET_MILITARY_SOUND );
      break;

    case FLASH:
      PlaySound ( FIRE_BULLET_FLASH_SOUND );
      break;

    case EXTERMINATOR:
      PlaySound ( FIRE_BULLET_EXTERMINATOR_SOUND );
      break;

    case LASER_RIFLE:
      PlaySound ( FIRE_BULLET_LASER_RIFLE_SOUND );
      break;

    case SINGLE_LASER:
      PlaySound ( FIRE_BULLET_SINGLE_LASER_SOUND );
      break;

    case PLASMA_PISTOL:
      PlaySound ( FIRE_BULLET_PLASMA_PISTOL_SOUND );
      break;

    case LASER_SWORD_1:
    case LASER_AXE:
    case LASER_SWORD_2:
    default:
      PlaySound ( SWING_THEN_NOHIT_1_SOUND );
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

  PlaySound (TAKEOVER_SET_CAPSULE_SOUND);

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

  PlaySound ( TAKEOVER_GAME_WON_SOUND );

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

  PlaySound ( TAKEOVER_GAME_DEADLOCK_SOUND );

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

  PlaySound ( TAKEOVER_GAME_LOST_SOUND );
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

  PlaySound ( COLLISIONSOUND );

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

  PlaySound ( BLASTSOUND );

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
      PlaySound ( MS_MACHINES_CLOSE_NOW_SOUND );
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
      PlaySound ( ENEMY_GOT_HIT_SOUND_0 );
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

  PlaySound ( BULLET_REFLECTED_SOUND);

}; // void DruidBlastSound (void)




#undef _sound_c
