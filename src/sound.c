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

#define FD_SFX_TRACKS 20

#ifdef HAVE_LIBSDL_MIXER
static MIX_Mixer *fd_mixer;
static MIX_Audio *Loaded_WAV_Files[ALL_SOUNDS];
static MIX_Audio *MusicSongs[NUM_COLORS];
static MIX_Audio *Tmp_MOD_File;
static MIX_Track *fd_music_track;
static MIX_Track *fd_sfx_tracks[FD_SFX_TRACKS];
static size_t fd_next_sfx_track;

static bool FD_LoadAudioFile(MIX_Audio **audio, const char *filename, bool predecode);
static MIX_Track *FD_GetNextSfxTrack(void);
static bool FD_PlayTrack(MIX_Track *track, MIX_Audio *audio, int loops);
static void FD_StopBackgroundMusic(void);
#endif

// The following is the definition of the sound file names used in freedroid
// DO NOT CHANGE THE ORDER OF APPEARENCE IN THIS LIST unless you
// also adjust the order of appearance in defs.h!

const char *SoundSampleFilenames[ALL_SOUNDS] = {
   "ERRORSOUND_NILL.NOWAV",
   "Blast_Sound_0.wav",
   // "Collision_Sound_0.wav", // replaced by damage-dependent-sounds:  Collision_[Neutral|GotDamaged|DamagedEnemy]
   "Collision_Neutral.wav",
   "Collision_GotDamaged.wav",
   "Collision_DamagedEnemy.wav",
   //"GotIntoBlast_Sound_0.wav", // replaced by GotIntoBlast_Sound_1.wav
   "GotIntoBlast_Sound_1.wav",
   "MoveElevator_Sound_0.wav",
   "Refresh_Sound_0.wav",
   "LeaveElevator_Sound_0.wav",
   "EnterElevator_Sound_0.wav",
   "ThouArtDefeated_Sound_0.wav",
   "Got_Hit_Sound_0.wav",
   "TakeoverSetCapsule_Sound_0.wav",
   "Menu_Item_Selected_Sound_0.wav",
   "Move_Menu_Position_Sound_0.wav",
   "Takeover_Game_Won_Sound_0.wav",
   "Takeover_Game_Deadlock_Sound_0.wav",
   "Takeover_Game_Lost_Sound_0.wav",
   "Fire_Bullet_Pulse_Sound_0.wav",
   "Fire_Bullet_Single_Pulse_Sound_0.wav",
   "Fire_Bullet_Military_Sound_0.wav",
   "Fire_Bullet_Flash_Sound_0.wav",
   "Fire_Bullet_Exterminator_Sound_0.wav",
   "Fire_Bullet_Laser_Rifle_Sound.wav",
   "Cry_Sound_0.wav",
   "Takeover_Sound_0.wav",
   "Countdown_Sound.wav",
   "EndCountdown_Sound.wav",
   "InfluExplosion.wav",
   "WhiteNoise.wav",
   "Alert.wav",
   "Screenshot.wav"
};

const char *MusicFiles [NUM_COLORS] = {  // we have a background song per color now
#ifdef ANDROID
  "AnarchyMenu1.mod.ogg",                 // RED
  "starpaws.mod.ogg",                     // YELLOW
  "The_Last_V8.mod.ogg",                  // GREEN
  "dreamfish-green_beret.mod.ogg",        // GRAY
  "dreamfish-sanxion.mod.ogg",            // BLUE // CRASHES the GCW0 ???
  "kollaps-tron.mod.ogg",                 // GREENBLUE
  "dreamfish-uridium2_loader.mod.ogg"     // DARK
#else
  "AnarchyMenu1.mod",                 // RED
  "starpaws.mod",                     // YELLOW
  "The_Last_V8.mod",                  // GREEN
  "dreamfish-green_beret.mod",        // GRAY
#ifdef GCW0
  "dreamfish-green_beret.mod",        // GRAY
#else
  "dreamfish-sanxion.mod",            // BLUE // CRASHES the GCW0 ???
#endif
  "kollaps-tron.mod",                 // GREENBLUE
  "dreamfish-uridium2_loader.mod"     // DARK
#endif // ANDROID
};

#ifdef HAVE_LIBSDL_MIXER
static bool
FD_LoadAudioFile(MIX_Audio **audio, const char *filename, bool predecode)
{
  char *fpath;

  *audio = NULL;
  fpath = find_file(filename, SOUND_DIR, NO_THEME, WARNONLY);
  if (fpath == NULL) {
    DebugPrintf(0, "\nError loading sound-file: %s\n", filename);
    return false;
  }

  *audio = MIX_LoadAudio(fd_mixer, fpath, predecode);
  if (*audio == NULL) {
    DebugPrintf(0, "SDL Mixer Error: %s\nContinuing with sound disabled\n", SDL_GetError());
    return false;
  }

  return true;
}

static MIX_Track *
FD_GetNextSfxTrack(void)
{
  size_t i;

  for (i = 0; i < NUM_ELEM(fd_sfx_tracks); i++) {
    size_t index = (fd_next_sfx_track + i) % NUM_ELEM(fd_sfx_tracks);
    MIX_Track *track = fd_sfx_tracks[index];

    if (track != NULL && !MIX_TrackPlaying(track) && !MIX_TrackPaused(track)) {
      fd_next_sfx_track = (index + 1) % NUM_ELEM(fd_sfx_tracks);
      return track;
    }
  }

  fd_next_sfx_track = (fd_next_sfx_track + 1) % NUM_ELEM(fd_sfx_tracks);
  return fd_sfx_tracks[fd_next_sfx_track];
}

static bool
FD_PlayTrack(MIX_Track *track, MIX_Audio *audio, int loops)
{
  SDL_PropertiesID options = 0;
  bool success = false;

  if (track == NULL || audio == NULL) {
    return false;
  }

  if (!MIX_SetTrackAudio(track, audio)) {
    return false;
  }

  if (loops != 0) {
    options = SDL_CreateProperties();
    if (options == 0) {
      return false;
    }
    if (!SDL_SetNumberProperty(options, MIX_PROP_PLAY_LOOPS_NUMBER, loops)) {
      SDL_DestroyProperties(options);
      return false;
    }
  }

  success = MIX_PlayTrack(track, options);
  if (options != 0) {
    SDL_DestroyProperties(options);
  }

  return success;
}

static void
FD_StopBackgroundMusic(void)
{
  if (fd_music_track == NULL) {
    return;
  }

  MIX_StopTrack(fd_music_track, 0);
}
#endif

void
Init_Audio(void)
{
#ifndef HAVE_LIBSDL_MIXER
  return;
#else
  int i;
  SDL_AudioSpec mixer_spec;

  DebugPrintf(1, "\nInitializing SDL Audio Systems....\n");

  if ( !sound_on ) return;


  // Now SDL_AUDIO is initialized here:

  if ( !SDL_InitSubSystem ( SDL_INIT_AUDIO ) )
    {
      DebugPrintf (0, "WARNING: SDL Sound subsystem could not be initialized.\n\
Continuing with sound disabled\n");
      sound_on = FALSE;
      return;
    }
  else
    DebugPrintf(1, "SDL Audio initialisation successful.\n");

  if (!MIX_Init()) {
    DebugPrintf(0, "WARNING: SDL_mixer could not be initialized.\n");
    DebugPrintf(0, "SDL Mixer Error: %s\nContinuing with sound disabled\n", SDL_GetError());
    sound_on = FALSE;
    return;
  }

  SDL_zero(mixer_spec);
  mixer_spec.freq = 44100;
  mixer_spec.format = SDL_AUDIO_S16;
  mixer_spec.channels = 2;

  fd_mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &mixer_spec);
  if (fd_mixer == NULL) {
    DebugPrintf(0, "WARNING: SDL audio mixer could not be opened.\n");
    DebugPrintf(0, "SDL Mixer Error: %s\nContinuing with sound disabled\n", SDL_GetError());
    sound_on = FALSE;
    MIX_Quit();
    return;
  }
  DebugPrintf(1, "\nSuccessfully opened SDL audio mixer.");

  fd_music_track = MIX_CreateTrack(fd_mixer);
  if (fd_music_track == NULL) {
    DebugPrintf(0, "WARNING: could not create SDL music track.\n");
    DebugPrintf(0, "SDL Mixer Error: %s\nContinuing with sound disabled\n", SDL_GetError());
    sound_on = FALSE;
    FreeSounds();
    return;
  }

  for (i = 0; i < FD_SFX_TRACKS; i++) {
    fd_sfx_tracks[i] = MIX_CreateTrack(fd_mixer);
    if (fd_sfx_tracks[i] == NULL) {
      DebugPrintf(0, "WARNING: could not create all SDL sound-effect tracks.\n");
      DebugPrintf(0, "SDL Mixer Error: %s\nContinuing with sound disabled\n", SDL_GetError());
      sound_on = FALSE;
      FreeSounds();
      return;
    }
  }

  // Now that the audio channel is opend, its time to load all the
  // WAV files into memory, something we NEVER did while using the yiff,
  // because the yiff did all the loading, analyzing and playing...

  Loaded_WAV_Files[0]=NULL;
  for (i = 1; i < ALL_SOUNDS; i++)
    {

      if (!FD_LoadAudioFile(&Loaded_WAV_Files[i], SoundSampleFilenames[i], true))
	{
	  DebugPrintf (0, "Could not load Sound-sample: %s\n", SoundSampleFilenames[ i ]);
	  sound_on = FALSE;
	  FreeSounds();
	  return;
	} // if ( !Loaded_WAV...
      else
	DebugPrintf (1, "\nSuccessfully loaded file %s.", SoundSampleFilenames[i]);
    } // for (i=1; ...

  for (i = 0; i < NUM_COLORS; i++)
    {

      if (!FD_LoadAudioFile(&MusicSongs[i], MusicFiles[i], false))
	{
	  DebugPrintf ( 0, "\nError loading sound-file: %s\n", MusicFiles[ i ]);
	  sound_on = FALSE;
	  FreeSounds();
	  return;
	} // if ( !Loaded_WAV...
      else
	DebugPrintf ( 1 , "\nSuccessfully loaded file %s.", MusicFiles[ i ]);
    } // for


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

void
Set_BG_Music_Volume(float NewVolume)
{
#ifndef HAVE_LIBSDL_MIXER
  (void)NewVolume;
#endif

#ifndef HAVE_LIBSDL_MIXER
  return;
#else
  if ( !sound_on ) return;

  MIX_SetTrackGain(fd_music_track, NewVolume);

#endif // HAVE_LIBSDL_MIXER
} // void Set_BG_Music_Volume(float NewVolume)

void
Set_Sound_FX_Volume(float NewVolume)
{
#ifndef HAVE_LIBSDL_MIXER
  (void)NewVolume;
#endif
#ifdef HAVE_LIBSDL_MIXER
  int i;
#endif
#ifndef HAVE_LIBSDL_MIXER
  return;
#else
  if ( !sound_on ) return;

  // Set the volume IN the loaded files, if SDL is used...
  // This is done here for the Files 1,2,3 and 4, since these
  // are background music files.
  for ( i=0 ; i<FD_SFX_TRACKS ; i++ )
    MIX_SetTrackGain(fd_sfx_tracks[i], NewVolume);

#endif // HAVE_LIBSDL_MIXER

} // void Set_BG_Music_Volume(float NewVolume)


void
CountdownSound (void)
{
  Play_Sound (COUNTDOWN_SOUND);
}

void
EndCountdownSound (void)
{
  Play_Sound (ENDCOUNTDOWN_SOUND);
}

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

This function is intended to provide a convenient way of switching
between different backround sounds in freedroid.
If no background sound was yet running, the function should start playing
the given background music.
If some background sound was already playing, the function should shut down
the old background music and start playing the new one.

Technical details:



@Ret:
@Int:
* $Function----------------------------------------------------------*/
void
Switch_Background_Music_To ( const char* filename_raw )
{
#ifndef HAVE_LIBSDL_MIXER
  (void)filename_raw;
#endif
#ifdef HAVE_LIBSDL_MIXER
  static int prev_color = -1;
  static bool paused = FALSE;
#endif

#ifndef HAVE_LIBSDL_MIXER
  return;
#else

  if ( !sound_on ) return;

  if ( filename_raw == NULL )
    {
      MIX_PauseTrack(fd_music_track);
      paused = TRUE;
      return;
    }

  // rp: lets cheat when using Mingw32
#if (!defined SDL_PLATFORM_WIN32  && !defined HAVE_LIBVORBIS)
  if (strstr (filename_raw, ".ogg"))
    {
      DebugPrintf (1, "\n\nWARNING: no ogg vorbis libs were found when configuring,\n\
 can't play %s!\n", filename_raw);
      return;
    }

#endif

  // New feature: choose background music by level-color:
  // if filename_raw==BYCOLOR then chose bg_music[color]
  // NOTE: if new level-color is the same as before, just resume paused music!
  if (!strcmp( filename_raw, BYCOLOR))
    {
      if (paused && (prev_color == CurLevel->color) )  // current level-song was just paused
	{
	  MIX_ResumeTrack(fd_music_track);
	  paused = FALSE;
	}
      else
	{
	  if (!FD_PlayTrack(fd_music_track, MusicSongs[CurLevel->color], -1)) {
	    DebugPrintf(0, "SDL Mixer Error: %s\n Continuing with sound disabled\n", SDL_GetError());
	    return;
	  }
	  paused = FALSE;
	  prev_color = CurLevel->color;
	}
    }
  else  // not using BYCOLOR mechanism: just play specified song
    {
      if (Tmp_MOD_File) {
        MIX_DestroyAudio(Tmp_MOD_File);
        Tmp_MOD_File = NULL;
      }

      if (!FD_LoadAudioFile(&Tmp_MOD_File, filename_raw, false)) {
        return;
      }

      if (!FD_PlayTrack(fd_music_track, Tmp_MOD_File, -1)) {
	DebugPrintf(0, "SDL Mixer Error: %s\n Continuing with sound disabled\n", SDL_GetError());
	return;
      }
      paused = FALSE;
    }

  MIX_SetTrackGain(fd_music_track, GameConfig.Current_BG_Music_Volume);

#endif // HAVE_LIBSDL_MIXER

}; // void Switch_Background_Music_To(int Tune)

void
Stop_Background_Music(void)
{
#ifndef HAVE_LIBSDL_MIXER
  return;
#else
  if (!sound_on) return;

  FD_StopBackgroundMusic();
#endif
}


/*@Function============================================================
@Desc:

@Ret:
@Int:
* $Function----------------------------------------------------------*/
void
Play_Sound (int Tune)
{
#ifndef HAVE_LIBSDL_MIXER
  (void)Tune;
#endif
#ifndef HAVE_LIBSDL_MIXER
  return;
#else
  MIX_Track *track;

  if ( !sound_on ) return;

  track = FD_GetNextSfxTrack();
  if (track == NULL || !FD_PlayTrack(track, Loaded_WAV_Files[Tune], 0))
    {
      DebugPrintf (0, "WARNING: Could not play sound-sample: %s Error: %s\n\
This usually just means that too many samples where played at the same time\n",
		   SoundSampleFilenames[ Tune ] , SDL_GetError() );
    } // if ( ... = -1
  else
    DebugPrintf( 2 , "\nSuccessfully playing file %s.", SoundSampleFilenames[ Tune ]);

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
GotIntoBlastSound (void)
{
  if (!sound_on) return;

  Play_Sound (GOT_INTO_BLAST_SOUND);
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

  Play_Sound (MENU_ITEM_SELECTED_SOUND);
}				// void MoveLiftSound(void)

/*@Function============================================================
@Desc:

@Ret:
@Int:
* $Function----------------------------------------------------------*/
void
MoveMenuPositionSound (void)
{
  if (!sound_on) return;

  Play_Sound (MOVE_MENU_POSITION_SOUND);
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

    }
}				// void FireBulletSound(void)


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
}				// void FireBulletSound(void)

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
}				// void FireBulletSound(void)

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
}				// void FireBulletSound(void)

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
}				// void FireBulletSound(void)

void
CollisionGotDamagedSound ( void )
{
  if ( !sound_on ) return;

  Play_Sound ( COLLISION_GOT_DAMAGED_SOUND );
}

void
CollisionDamagedEnemySound ( void )
{
  if ( !sound_on ) return;

  Play_Sound ( COLLISION_DAMAGED_ENEMY_SOUND );
}

/*@Function============================================================
@Desc:

@Ret:
@Int:
* $Function----------------------------------------------------------*/
void
BounceSound (void)
{
  if (!sound_on) return;

  // Play_Sound (COMBAT_BACKGROUND_MUSIC_SOUND );
  Play_Sound ( COLLISIONSOUND );

}				// void BounceSound(void)

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

}				// void BounceSound(void)

void
FreeSounds ( void )
{
#ifdef HAVE_LIBSDL_MIXER
  FD_StopBackgroundMusic();

  for (size_t i = 0; i < NUM_ELEM(Loaded_WAV_Files); i++) {
    if ( Loaded_WAV_Files[i] != NULL ) { MIX_DestroyAudio ( Loaded_WAV_Files[i] ); }
    Loaded_WAV_Files[i] = NULL;
  }

  for ( size_t i = 0; i < NUM_ELEM(MusicSongs); i ++ ) {
    if ( MusicSongs[i] != NULL )  { MIX_DestroyAudio ( MusicSongs[i] ); }
    MusicSongs[i] = NULL;
  }

  if ( Tmp_MOD_File ) {
    MIX_DestroyAudio ( Tmp_MOD_File );
    Tmp_MOD_File = NULL;
  }

  for ( size_t i = 0; i < NUM_ELEM(fd_sfx_tracks); i++ ) {
    if ( fd_sfx_tracks[i] != NULL ) {
      MIX_DestroyTrack(fd_sfx_tracks[i]);
      fd_sfx_tracks[i] = NULL;
    }
  }

  if (fd_music_track != NULL) {
    MIX_DestroyTrack(fd_music_track);
    fd_music_track = NULL;
  }

  if (fd_mixer != NULL) {
    MIX_DestroyMixer(fd_mixer);
    fd_mixer = NULL;
  }

  MIX_Quit();

#endif
  return;
}
#undef _sound_c
