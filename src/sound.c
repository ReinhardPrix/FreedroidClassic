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

//--------------------
// We'll use a new sound concept now:  Don't load all the sound samples
// we might be using again and again at game startup (which would require
// that we know all the sound sample file names in advance and hard-code
// them into the game) but rather use a dynamic cache, i.e. sounds are
// loaded once they are first needed and then either droped again (like 
// in Play...Once() or they are kept and cached and when next time a call
// comes in, we use the sample from the dynamic cache.
// The gain is shorter startup time and less hard-coding and maybe more
// efficiency.
//
#define MAX_SOUNDS_IN_DYNAMIC_WAV_CACHE 100
#ifdef HAVE_LIBSDL_MIXER
Mix_Chunk *dynamic_WAV_cache[ MAX_SOUNDS_IN_DYNAMIC_WAV_CACHE ];
char* sound_names_in_dynamic_wav_chache [ MAX_SOUNDS_IN_DYNAMIC_WAV_CACHE ];
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

//--------------------
// This variable refers to the next free position inside the WAV file
// cache.
//
static int next_free_position_in_cache = 0 ;

void play_sample_using_WAV_cache( char* SoundSampleFileName , int With_Waiting , int no_double_catching ) ;
void remove_all_samples_from_WAV_cache( void );

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
  char* fpath="no_fpath_has_been_set";
#endif

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

  //--------------------
  // Only if the file name wasn't 'no_voice_sample', we really
  // try to load anything...
  //  
  if ( strcmp ( SoundSampleFileName , "Sorry_No_Voice_Sample_Yet_0.wav" ) )
    {
      // This code searches for different kinds of 
      int i;
      int pathlen;
      char *extension;
      char *extensions[] = { ".spx", ".ogg", ".wav", NULL };  // Extensions to try for audio
      
      pathlen = strlen(Temp_Filename);

      if(strcmp(Temp_Filename+pathlen-4, ".wav") == 0)
      {
          extension = Temp_Filename + pathlen - 4;
      }
      else
      {
          extension = Temp_Filename + pathlen;
      }
      
      i = 0;
      while(extensions[i] != NULL)
      {
        strcpy(extension, extensions[i]);
        fpath = find_file ( Temp_Filename , SOUND_DIR, FALSE);
        One_Shot_WAV_File = Mix_LoadWAV( fpath );
        if(One_Shot_WAV_File != NULL)
        {
            break;
        }

        i++;
      }
    }
  else
    One_Shot_WAV_File = NULL ;

  //--------------------
  // Now some error checking against failed/missing sound samples...
  //
  if ( One_Shot_WAV_File == NULL )
    {
      //--------------------
      // A warning message about a missing speech file should only be issued,
      // if it wasn't the 'no_voice_sample' dummy entry anyway...
      //
      if ( strcmp ( SoundSampleFileName , "Sorry_No_Voice_Sample_Yet_0.wav" ) )
	{
	  fprintf( stderr, "\n\nfpath: '%s'\n" , fpath );
	  GiveStandardErrorMessage ( __FUNCTION__  , "\
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
	}

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
      GiveStandardErrorMessage ( __FUNCTION__  , "\
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
	  GiveStandardErrorMessage ( __FUNCTION__  , "\
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
      GiveStandardErrorMessage ( __FUNCTION__  , "\
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
      GiveStandardErrorMessage ( __FUNCTION__  , "\
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

  remove_all_samples_from_WAV_cache(  ) ;

#endif // HAVE_LIBSDL_MIXER

} // void SetBGMusicVolume(float NewVolume)

/* ----------------------------------------------------------------------
 *
 * ---------------------------------------------------------------------- */
void
CrySound (void)
{
  play_sample_using_WAV_cache( "../effects/Cry_Sound_0.wav", FALSE , FALSE );
}; // void CrySound (void)

/* ----------------------------------------------------------------------
 * When a chest is entered (for the chest take/put dialog) or when a
 * chest is just looted by left-click, we always play this sound...
 * ---------------------------------------------------------------------- */
void
play_open_chest_sound( void )
{
  PlayOnceNeededSoundSample ( "../effects/open_chest_sound.wav" , FALSE , TRUE );
}; // void play_open_chest_sound( void )

/* ----------------------------------------------------------------------
 * When a Force-To-Energy Spell is invoked successfully, then this sound
 * should be played...
 * ---------------------------------------------------------------------- */
void
Play_Spell_ForceToEnergy_Sound( )
{
  play_sample_using_WAV_cache( "../effects/Spell_ForceToEnergy_Sound_0.wav", FALSE , FALSE );
};  // void Play_Spell_ForceToEnergy_Sound( )

/* ----------------------------------------------------------------------
 * When a DetectItems Spell is invoked successfully, then this sound
 * should be played...
 * ---------------------------------------------------------------------- */
void
Play_Spell_DetectItems_Sound( )
{
  play_sample_using_WAV_cache( "../effects/Spell_DetectItems_Sound_0.wav", FALSE , FALSE );
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
      play_sample_using_WAV_cache( "../effects/No_Ammo_Sound_0.wav", FALSE , FALSE );
      PreviousSound = now;
    }
}; // void No_Ammo_Sound ( void )

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
      play_sample_using_WAV_cache( "../effects/tux_ingame_comments/Not_Enough_Power_Sound_0.wav", FALSE , FALSE );
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
      play_sample_using_WAV_cache( "../effects/tux_ingame_comments/Not_Enough_Dist_Sound_0.wav", FALSE , FALSE );
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
      play_sample_using_WAV_cache( "../effects/tux_ingame_comments/Not_Enough_Mana_0.wav", FALSE , FALSE );
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
      play_sample_using_WAV_cache( "../effects/bot_sounds/First_Contact_Sound_0.wav" , FALSE , FALSE );
      break;
    case 1:
      play_sample_using_WAV_cache( "../effects/bot_sounds/First_Contact_Sound_1.wav" , FALSE , FALSE );
      break;
    case 2:
      play_sample_using_WAV_cache( "../effects/bot_sounds/First_Contact_Sound_2.wav" , FALSE , FALSE );
      break;
    case 3:
      play_sample_using_WAV_cache( "../effects/bot_sounds/First_Contact_Sound_3.wav" , FALSE , FALSE );
      break;
    case 4:
      play_sample_using_WAV_cache( "../effects/bot_sounds/First_Contact_Sound_4.wav" , FALSE , FALSE );
      break;
    case 5:
      play_sample_using_WAV_cache( "../effects/bot_sounds/First_Contact_Sound_5.wav" , FALSE , FALSE );
      break;
    case 6:
      play_sample_using_WAV_cache( "../effects/bot_sounds/First_Contact_Sound_6.wav" , FALSE , FALSE );
      break;
    case 7:
      play_sample_using_WAV_cache( "../effects/bot_sounds/First_Contact_Sound_7.wav" , FALSE , FALSE );
      break;
    case 8:
      play_sample_using_WAV_cache( "../effects/bot_sounds/First_Contact_Sound_8.wav" , FALSE , FALSE );
      break;
    case 9:
      play_sample_using_WAV_cache( "../effects/bot_sounds/First_Contact_Sound_9.wav" , FALSE , FALSE );
      break;
    case 10:
      play_sample_using_WAV_cache( "../effects/bot_sounds/First_Contact_Sound_10.wav" , FALSE , FALSE );
      break;
    case 11:
      play_sample_using_WAV_cache( "../effects/bot_sounds/First_Contact_Sound_11.wav" , FALSE , FALSE );
      break;
    case 12:
      play_sample_using_WAV_cache( "../effects/bot_sounds/First_Contact_Sound_12.wav" , FALSE , FALSE );
      break;
    case 13:
      play_sample_using_WAV_cache( "../effects/bot_sounds/First_Contact_Sound_13.wav" , FALSE , FALSE );
      break;
    case 14:
      play_sample_using_WAV_cache( "../effects/bot_sounds/First_Contact_Sound_14.wav" , FALSE , FALSE );
      break;
    case 15:
      play_sample_using_WAV_cache( "../effects/bot_sounds/First_Contact_Sound_15.wav" , FALSE , FALSE );
      break;
    case 16:
      play_sample_using_WAV_cache( "../effects/bot_sounds/First_Contact_Sound_16.wav" , FALSE , FALSE );
      break;
    case 17:
      play_sample_using_WAV_cache( "../effects/bot_sounds/First_Contact_Sound_17.wav" , FALSE , FALSE );
      break;
    case 18:
      play_sample_using_WAV_cache( "../effects/bot_sounds/First_Contact_Sound_18.wav" , FALSE , FALSE );
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
  // PlayOnceNeededSoundSample ( filename , FALSE , FALSE );
  play_sample_using_WAV_cache( filename , FALSE , FALSE );

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
      GiveStandardErrorMessage ( __FUNCTION__  , "\
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
	  play_sample_using_WAV_cache ( "../effects/tux_ingame_comments/ICantCarryAnyMore_Sound_0.wav", FALSE , FALSE );
	  break;
	case 1 :
	  play_sample_using_WAV_cache ( "../effects/tux_ingame_comments/ICantCarryAnyMore_Sound_1.wav", FALSE , FALSE );
	  break;
	case 2 :
	  play_sample_using_WAV_cache ( "../effects/tux_ingame_comments/ICantCarryAnyMore_Sound_2.wav", FALSE , FALSE );
	  break;
	default:
	  break;
	}
    }
}; // void CantCarrySound (void)

/* ---------------------------------------------------------------------- 
 *
 * ---------------------------------------------------------------------- */
void
TransferSound (void)
{
  play_sample_using_WAV_cache ( "../effects/Takeover_Sound_0.wav", FALSE , FALSE );
}; // void TransferSound (void)

/* ---------------------------------------------------------------------- 
 *
 * ---------------------------------------------------------------------- */
void
Mission_Status_Change_Sound (void)
{
  play_sample_using_WAV_cache ( "../effects/Mission_Status_Change_Sound_0.wav", FALSE , FALSE );
}; // void Mission_Status_Change_Sound (void)

/* ----------------------------------------------------------------------
 *
 * ---------------------------------------------------------------------- */
void
teleport_arrival_sound ( void )
{
  play_sample_using_WAV_cache ( "../effects/LeaveElevator_Sound_0.wav", FALSE , FALSE );
}; // void teleport_arrival_sound ( void )

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
 * ---------------------------------------------------------------------- */
void
GotHitSound (void)
{
  play_sample_using_WAV_cache( "../effects/Got_Hit_Sound_0.wav" , FALSE , FALSE );
}; // void GotHitSound(void)


/* ----------------------------------------------------------------------
 *
 * ---------------------------------------------------------------------- */
void
tux_scream_sound (void)
{
  if ( MyRandom ( 7 ) <= 5 ) return;

  switch( MyRandom( 4 ) )
    {
    case 0 :
      // the 0 sound is bad.  we use the 3 for now
      play_sample_using_WAV_cache( "../effects/Influencer_Scream_Sound_3.wav", FALSE , FALSE );
      break;
    case 1 :
      play_sample_using_WAV_cache( "../effects/Influencer_Scream_Sound_1.wav", FALSE , FALSE );
      break;
    case 2 :
      // the 2 sound is bad.  we use the 3 for now
      play_sample_using_WAV_cache( "../effects/Influencer_Scream_Sound_3.wav", FALSE , FALSE );
      break;
    case 3 :
      play_sample_using_WAV_cache( "../effects/Influencer_Scream_Sound_3.wav", FALSE , FALSE );
      break;
    case 4 :
      play_sample_using_WAV_cache( "../effects/Influencer_Scream_Sound_4.wav", FALSE , FALSE );
      break;
    default:
      break;
    }
}; // void tux_scream_sound (void)

/* ----------------------------------------------------------------------
 * We add a matching group of sounds here for the menu movements.  It's
 * a 'ping-ping' sound, well, not super, but where do we get a better one?
 * ---------------------------------------------------------------------- */
void
MenuItemSelectedSound (void)
{
  play_sample_using_WAV_cache( "../effects/Menu_Item_Selected_Sound_1.wav", FALSE , FALSE );
}; // void MenuItemSelectedSound ( void )
void
MenuItemDeselectedSound (void)
{
  play_sample_using_WAV_cache( "../effects/Menu_Item_Deselected_Sound_0.wav", FALSE , FALSE );
}; // void MenuItemSelectedSound ( void )
void
MoveMenuPositionSound (void)
{
  play_sample_using_WAV_cache( "../effects/Move_Menu_Position_Sound_0.wav", FALSE , FALSE );
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

/* ----------------------------------------------------------------------
 * When the Tux makes a weapon swing, this will either cause a swinging
 * sound and then a 'hit' sound or it will just be a swinging sound.  The
 * following functions do this, also creating some variation in the choice
 * of sample used.
 * ---------------------------------------------------------------------- */
void
play_melee_weapon_hit_something_sound ( void )
{
  switch( MyRandom( 3 ) )
    {
    case 0 :
      play_sample_using_WAV_cache( "../effects/swing_then_hit_1.wav", FALSE , FALSE );
      break;
    case 1 :
      play_sample_using_WAV_cache( "../effects/swing_then_hit_2.wav", FALSE , FALSE );
      break;
    case 2 :
      play_sample_using_WAV_cache( "../effects/swing_then_hit_3.wav", FALSE , FALSE );
      break;
    case 3 :
      play_sample_using_WAV_cache( "../effects/swing_then_hit_4.wav", FALSE , FALSE );
      break;
    default:
      break;
    }
}; // void play_melee_weapon_hit_something_sound ( void )
void
play_melee_weapon_missed_sound ( void )
{
  switch( MyRandom( 3 ) )
    {
    case 0 :
      play_sample_using_WAV_cache( "../effects/swing_then_nohit_1.wav", FALSE , FALSE );
      break;
    case 1 :
      play_sample_using_WAV_cache( "../effects/swing_then_nohit_2.wav", FALSE , FALSE );
      break;
    case 2 :
      play_sample_using_WAV_cache( "../effects/swing_then_nohit_3.wav", FALSE , FALSE );
      break;
    case 3 :
      play_sample_using_WAV_cache( "../effects/swing_then_nohit_4.wav", FALSE , FALSE );
      break;
    default:
      break;
    }
}; // void play_melee_weapon_missed_sound ( void )

/* ----------------------------------------------------------------------
 * This function should generate the sound that belongs to a certain
 * (ranged) weapon.  This does not include the Tux swinging/swinging_and_hit
 * sounds, when Tux is using melee weapons, but it does include ranged
 * weapons and the non-animated bot weapons too.
 * ---------------------------------------------------------------------- */
void
Fire_Bullet_Sound (int BulletType)
{
  if (!sound_on) return;

  DebugPrintf ( 1, "\nFireBulletSound called..." );

  switch (BulletType)
    {
    case PULSE:
      play_sample_using_WAV_cache( "../effects/Fire_Bullet_Pulse_Sound_0.wav", FALSE , FALSE );
      break;

    case SINGLE_PULSE:
      play_sample_using_WAV_cache( "../effects/Fire_Bullet_Single_Pulse_Sound_0.wav", FALSE , FALSE );
      break;

    case MILITARY:
      play_sample_using_WAV_cache( "../effects/Fire_Bullet_Military_Sound_0.wav", FALSE , FALSE );
      break;

    case FLASH:
      play_sample_using_WAV_cache( "../effects/Fire_Bullet_Flash_Sound_0.wav", FALSE , FALSE );
      break;

    case EXTERMINATOR:
      play_sample_using_WAV_cache( "../effects/Fire_Bullet_Exterminator_Sound_0.wav", FALSE , FALSE );
      break;

    case LASER_RIFLE:
      play_sample_using_WAV_cache( "../effects/phaser.wav", FALSE , FALSE );
      break;

    case SINGLE_LASER:
      play_sample_using_WAV_cache( "../effects/Fire_Bullet_Single_Laser_Sound_0.wav", FALSE , FALSE );
      break;

    case PLASMA_PISTOL:
      play_sample_using_WAV_cache( "../effects/Fire_Bullet_Plasma_Pistol_Sound_0.wav", FALSE , FALSE );
      break;

    case LASER_SWORD_1:
    case LASER_AXE:
    case LASER_SWORD_2:
    default:
      play_melee_weapon_missed_sound (  ) ;
      break;
    }
}; // void FireBulletSound(void)

/* ----------------------------------------------------------------------
 * For the takeover game, there are 4 main sounds.  We handle them from
 * the cache, even if that might also be possible as 'once_needed' type
 * sound samples...
 * ---------------------------------------------------------------------- */
void
Takeover_Set_Capsule_Sound (void)
{
  play_sample_using_WAV_cache( "../effects/TakeoverSetCapsule_Sound_0.wav", FALSE , FALSE );
}; // void Takeover_Set_Capsule_Sound ( void )
void
Takeover_Game_Won_Sound (void)
{
  play_sample_using_WAV_cache( "../effects/Takeover_Game_Won_Sound_0.wav", FALSE , FALSE );
}; // void Takeover_Game_Won_Sound ( void ) 
void
Takeover_Game_Deadlock_Sound (void)
{
  play_sample_using_WAV_cache( "../effects/Takeover_Game_Deadlock_Sound_0.wav", FALSE , FALSE );
}; // void Takeover_Game_Deadlock_Sound ( void )
void
Takeover_Game_Lost_Sound (void)
{
  play_sample_using_WAV_cache( "../effects/Takeover_Game_Lost_Sound_0.wav", FALSE , FALSE );
}; // void Takeover_Game_Lost_Sound ( void )


/* ----------------------------------------------------------------------
 * 
 * ---------------------------------------------------------------------- */
void
BounceSound (void)
{
  play_sample_using_WAV_cache( "../effects/Collision_Sound_0.wav" , FALSE , FALSE );
}; // void BounceSound ( void )

/* ----------------------------------------------------------------------
 *
 * ---------------------------------------------------------------------- */
void
DruidBlastSound (void)
{
  play_sample_using_WAV_cache( "../effects/Blast_Sound_0.wav" , FALSE , FALSE );
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
      play_sample_using_WAV_cache( "../effects/MSMachinesClose_0.wav" , FALSE , FALSE );
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
      play_sample_using_WAV_cache( "../effects/Enemy_Got_Hit_Sound_0.wav" , FALSE , FALSE );
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
  play_sample_using_WAV_cache( "../effects/Bullet_Reflected_Sound_0.wav" , FALSE , FALSE );
}; // void DruidBlastSound (void)

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
//----------------------------------------------------------------------
// This function should play a sound sample, that is NOT needed within
// the action part of the game but only in menus or dialogs and can 
// therefore be loaded and dumped on demand while the other sound samples
// for the action parts of the game will be kept in memory all the time.
// ----------------------------------------------------------------------
void
play_sample_using_WAV_cache( char* SoundSampleFileName , int With_Waiting , int no_double_catching ) 
{
#ifdef HAVE_LIBSDL_MIXER
  int Newest_Sound_Channel=0;
  char Temp_Filename[5000];
  char* fpath;
  int index_of_sample_to_be_played = 0 ;
  int sound_must_be_loaded = TRUE ;
  int i;

  //--------------------
  // In case sound has been disabled, we don't do anything here...
  //
  if ( !sound_on ) return;
  
  //--------------------
  // First we go take a look if maybe the sound sample file name in question
  // has been given to this function (at least) once before.  Then we can
  // assume, that the corresponding sound sample is already loaded and still
  // in the cache and just need to play it and then we can safely return
  // immediately, without setting up any callbacks or the like, like the
  // PlayOnceNeededSoundSample function has to.
  //
  for ( i = 0 ; i < MAX_SOUNDS_IN_DYNAMIC_WAV_CACHE ; i ++ )
    {
      if ( i >= next_free_position_in_cache )
	{
	  sound_must_be_loaded = TRUE ;
	  break;
	}

      if ( ! strcmp ( sound_names_in_dynamic_wav_chache [ i ] , SoundSampleFileName ) )
	{
	  sound_must_be_loaded = FALSE ;
	  index_of_sample_to_be_played = i ;
	  DebugPrintf( 0 , "\nFound the sound sample '%s' already in the cache!  Good.", SoundSampleFileName ) ;
	  break;
	}
    }

  //--------------------
  // So if the sound sample isn't in the cache and therefore must still be loaded,
  // we do so here before any playing takes place...
  //
  if ( sound_must_be_loaded )
    {
      //----------------------------------------------------------------------
      // So now we know, that the sound sample in question has not yet ever been
      // used before.  We must load it, play it and keep it in cache memory.
      //----------------------------------------------------------------------
      
      //--------------------
      // Now we try to load the requested sound file into memory...
      //
      dynamic_WAV_cache[ next_free_position_in_cache ] = NULL ;
      strcpy ( Temp_Filename , "speeches/" );
      strcat ( Temp_Filename , SoundSampleFileName );
      fpath = find_file ( Temp_Filename , SOUND_DIR, FALSE);
      dynamic_WAV_cache[ next_free_position_in_cache ] = Mix_LoadWAV( fpath );
      if ( dynamic_WAV_cache [ next_free_position_in_cache ]  == NULL )
	{
	  fprintf( stderr, "\n\nfpath: '%s'\n" , fpath );
	  GiveStandardErrorMessage ( __FUNCTION__  , "\
The SDL MIXER WAS UNABLE TO LOAD A CERTAIN SOUND FILE INTO MEMORY.\n\
This should not happen for samples that are supposed to be cached...",
				     NO_NEED_TO_INFORM , IS_WARNING_ONLY );
	  //--------------------
	  // If the sample couldn't be loaded, we just quit, not marking anything
	  // as loaded and inside the cache and also not trying to play anything...
	  //
	  return;
	}
  
      //--------------------
      // Hoping, that this will not take up too much processor speed, we'll
      // now change the volume of the sound sample in question to what is normal
      // for sound effects right now...
      //
      Mix_VolumeChunk( dynamic_WAV_cache [ next_free_position_in_cache ] , 
		       (int) rintf( GameConfig.Current_Sound_FX_Volume * MIX_MAX_VOLUME ) );

      //--------------------
      // We note the position of the sound file to be played
      //
      index_of_sample_to_be_played = next_free_position_in_cache ;

      //--------------------
      // Now we store the corresponding file name as well.
      //
      sound_names_in_dynamic_wav_chache [ next_free_position_in_cache ] = MyMalloc ( strlen ( SoundSampleFileName ) + 1 );
      strcpy ( sound_names_in_dynamic_wav_chache [ next_free_position_in_cache ] , SoundSampleFileName );
      DebugPrintf ( 1 , "\nSuccessfully added sample '%s' to sound cache at new position %d." ,
		   sound_names_in_dynamic_wav_chache [ next_free_position_in_cache ] , next_free_position_in_cache );

      //--------------------
      // Now we increase the 'next_sample' index and are done.
      //
      next_free_position_in_cache ++ ;
      if ( next_free_position_in_cache >= MAX_SOUNDS_IN_DYNAMIC_WAV_CACHE )
	{
	  fprintf( stderr, "\n\nnext_free_position_in_cache: %d,\n" , next_free_position_in_cache );
	  GiveStandardErrorMessage ( __FUNCTION__  , "\
ALERT!  Ran out of space in the dynamic wav sample cache!  Cache size too small?",
				     PLEASE_INFORM, IS_FATAL );
	}
    }
  
  //--------------------
  // Now we try to play the sound file that has just been successfully
  // loaded into memory or has resided in memory already for some time...
  //
  // In case of an error, we will of course print an error message
  // and quit...
  //
  Newest_Sound_Channel = Mix_PlayChannel( -1 , dynamic_WAV_cache [ index_of_sample_to_be_played ] , 0 );
  if ( Newest_Sound_Channel <= -1 )
    {
      fprintf( stderr, "\n\nSoundSampleFileName: '%s' Mix_GetError(): %s \n" , SoundSampleFileName , Mix_GetError() );
      GiveStandardErrorMessage ( __FUNCTION__  , "\
The SDL mixer was unable to play a certain sound sample file, that was supposed to be cached for later.\n",
				 NO_NEED_TO_INFORM, IS_WARNING_ONLY );
    } // if ( ... = -1
  else
    {
      // SoundChannelList[ Newest_Sound_Channel ] = 1;
      DebugPrintf( 1 , "\nSuccessfully playing the 'to be cached' file %s.", SoundSampleFileName ) ;
    }

#endif // HAVE_LIBSDL_MIXER

}; // void play_sample_using_WAV_cache(...)

/* ----------------------------------------------------------------------
 * When the sound sample volume is changed via the in-game controls, we 
 * need to re-sample everything, which is done only upon loading of the
 * sound samples.  Instead of RE-MIXING SOUND SAMPLES AGAIN AND AGAIN to
 * adapt the sound sample volume to the users needs, we will just clear
 * out the current wav file cache, so that when needed again, the sound
 * files will be loaded again and then automatically re-sampled to proper
 * volume from the play/load function itself.  Perfect solution.
 * ---------------------------------------------------------------------- */
void 
remove_all_samples_from_WAV_cache( void )
{

#ifdef HAVE_LIBSDL_MIXER

  for ( i = 0 ; i < next_free_position_in_cache ; i ++ )
    {
      //--------------------
      // We free the allocated memory for the file name.  This is overly 'clean'
      // code.  The amount of memory in question would be neglectable...
      //
      free ( sound_names_in_dynamic_wav_chache [ i ] ) ;

      //--------------------
      // We free the allocated music chunk.  This is more important since the
      // music chunks in question can consume more memory...
      //
      Mix_FreeChunk ( dynamic_WAV_cache [ i ] ) ;
    }

  //--------------------
  // Now that the cache has been emptied, it must be marked as such, or
  // the next play function will search it and produce a segfault.
  //
  next_free_position_in_cache = 0 ;

  DebugPrintf ( 1 , "\nremove_all_samples_from_WAV_cache(...): Successfully cleared the whole WAV cache." );

#endif // HAVE_LIBSDL_MIXER

}; // void remove_all_samples_from_WAV_cache( void )

#undef _sound_c
