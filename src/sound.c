/* 
 *
 *   Copyright (c) 1994, 2002 Johannes Prix
 *   Copyright (c) 1994, 2002 Reinhard Prix
 *
 *
 *  This file is part of FreeDroid
 *
 *  FreeDroid is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  FreeDroid is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with FreeDroid; see the file COPYING. If not, write to the 
 *  Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
 *  MA  02111-1307  USA
 *
 */

/*----------------------------------------------------------------------
 *
 * Desc:  all functions dealing with sound are contained in this file.
 *
 *----------------------------------------------------------------------*/

#include <config.h>

#ifndef _sound_c
#define _sound_c
#endif

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/soundcard.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>

#include "defs.h"


// Leave the following lines in.  They are for the yiff sound server!!
#if HAVE_LIBY2
#include <Y2/Y.h>         //  Basic Y types and constants. 
#include <Y2/Ylib.h>      //  YLib functions and structs. 
#endif 

/* Change this to the address and port of the Y server you want
 * to connect to. Note that 127.0.0.1 is a symbolic address
 * meaning `localhost'.
 */
#define CON_ARG             "127.0.0.1:9433"
// Thanks a lot for leaving the above lines in.  They are for the yiff sound server!!


#include "struct.h"
#include "global.h"
#include "proto.h"



int handle = -1;
int setting = 0x000C000D; // 12 fragments size 8kb
int channels = 0;         // 0=mono 1=stereo
int format = AFMT_U8;
int rate = 8000;


// The following is the definition of the sound file names used in freedroid
// DO NOT EVER CHANGE THE ORDER OF APPEARENCE IN THIS LIST PLEASE!!!!!
// The order of appearance here should match the order of appearance 
// in the enum-Environment located in defs.h!

#define ALL_SOUNDS 15
char *SoundSampleFilenames[ALL_SOUNDS] = {
  "/../sound/ERRORSOUND_NILL",
  "/../sound/Combat_Background_Music.wav",
  "/../sound/Takeover_Background_Music.wav",
  "/../sound/Console_Background_Music.wav",
  "/../sound/Classical_Beep_Beep_Background_Music.wav",
  "/../sound/BlastSound1.wav",
  "/../sound/CollisionSound1.wav",
  "/../sound/FireSound1.wav",
  "/../sound/GotIntoBlastSound.wav",
  "/../sound/MoveElevatorSound1.wav",
  "/../sound/RefreshSound.wav",
  "/../sound/LeaveElevatorSound3.wav",
  "/../sound/EnterElevatorSound2.wav",
  "/../sound/ThouArtDefeatedSound2.wav",
  "/../sound/Got_Hit_Sound_1.wav"
};

char *ExpandedSoundSampleFilenames[ALL_SOUNDS];

long BlastSoundSampleLength=0;
long CollisionSoundSampleLength=0;
long FireSoundSampleLength=0;

unsigned char *BlastSoundSamplePointer;
unsigned char *CollisionSoundSamplePointer;
unsigned char *FireSoundSamplePointer;


#if HAVE_LIBY2
// The following Lines define several channels for sound output to the yiff sound server!!!
// Background Music Cannel 
YConnection *BackgroundMusic_con;
YEventSoundObjectAttributes BackgroundMusic_sndobj_attrib;
YID BackgroundMusic_play_id;
YEvent BackgroundMusic_event;
// Cannel 1
YConnection *con;
YEventSoundObjectAttributes sndobj_attrib;
YID play_id;
YEvent event;
// Cannel 2
YConnection *con2;
YEventSoundObjectAttributes sndobj_attrib2;
YID play_id2;
YEvent event2;
// Now some YIDs one for each Sound Played
YID BlastSoundSampleYID;
YID FireSoundSampleYID;
YID CollisionSoundSampleYID;
YID BackgroundMusicSampleYID;
#endif /* HAVE_LIBY2 */
// The above Lines define several channels for sound output to the yiff sound server!!!

void ExitProc() {
  if (handle != -1) {
    close( handle );
  }
}

int i;
unsigned char* ptr;
unsigned char v = 128;
int SampleLaenge;

// void MakeSound(tune* ThisTune);
long FragmentRoundUp(long FormerLength);

long FragmentRoundUp(long FormerLength){
  long NewLength;

  NewLength=FormerLength/(8*1024);
  NewLength=(NewLength+1)*(8*1024);
  return NewLength;
} // long FragmentRoundUp



void YIFF_Server_Check_Events(void){

  // This function can and should only be compiled on machines, that have the YIFF sound
  // server installed.  Compilation therefore is optional and can be toggled with the following
  // definition.
#if HAVE_LIBY2
  if(YGetNextEvent( BackgroundMusic_con , &event , False ) > 0)
    {
      // Sound object stopped playing? 
      if( (event.type == YSoundObjectKill) && (event.kill.yid == play_id) )
	{
	  // Our play has stopped. 
	  printf("Done playing.\n");
	}
      // Server disconnected us? 
      else if(event.type == YDisconnect)
	{
	  // Got disconnected.
	  printf(
		 "Y server disconnected us, reason %i.\n",
		 event.disconnect.reason
		 );
	  Terminate(ERR);
	}
      // Server shutdown? 
      else if(event.type == YShutdown)
	{
	  // Server shutdown. 
	  printf( "Y server shutdown, reason %i.\n", event.shutdown.reason );
	  Terminate(ERR);
	}
      else
	{
	  // Some other Y event, ignore. 
	}
    }
 
#endif /* HAVE_LIBY2 */
} // void YIFF_Server_Check_Events(void)

char *ExpandFilename(char *LocalFilename){
  char *tmp;

  tmp=malloc(strlen(LocalFilename)+strlen(getcwd(NULL,0)) + 1);
  strcpy(tmp,getcwd(NULL,0));

  // cut out the "/src" at the end of the sting, that is, make it 4 chars shorter
  // tmp[strlen(tmp)-4]=0;

  strcat(tmp,LocalFilename);
  return(tmp);
} // char *ExpandFilename(char *LocalFilename){


void YIFF_Server_Close_Connections(void){

  // This function can and should only be compiled on machines, that have the YIFF sound
  // server installed.  Compilation therefore is optional and can be toggled with the following
  // definition.
#if HAVE_LIBY2

  /* Disconnect from the Y server. We need to pass the
   * original connection pointer con to close that
   * connection to the Y server. Note that con may be
   * NULL at this point if the Y server disconnected us
   * already, passing NULL is okay.
   *
   * The second argument asks us do we want to leave the
   * Y server up when we disconnect. If we were the
   * program that started the Y erver and the second
   * argument is set to False then the Y server will
   * be automatically shut down.  To ensure that the Y
   * server stays running, you can pass True instead.
   */

  YCloseConnection(BackgroundMusic_con, False);
  BackgroundMusic_con = NULL;

#endif /* HAVE_LIBY2 */

} // void YIFF_Server_Close_Connections(void)


/*@Function============================================================
@Desc: Starts a Tune.

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void StartSound(int Tune){
    
} // void StartSound(int Tune)

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void CrySound(void)
{

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
Switch_Background_Music_To(int Tune)
{
#if HAVE_LIBY2

  static int Current_Tune=SILENCE;
  YEventSoundPlay Music_Parameters;


  DebugPrintf("\nvoid Switch_Background_Music_To(int Tune):  Real function call confirmed.\n");

  DebugPrintf("\nvoid Switch_Background_Music_To(int Tune):  Shutting down old background music track...\n");
  
  if ( Current_Tune == SILENCE ) 
    {

      DebugPrintf("\nvoid Switch_Background_Music_To(int Tune):  No old music there to be shut down...\n");

    } 
  else
    {

      YDestroyPlaySoundObject( BackgroundMusic_con , BackgroundMusic_play_id );

      DebugPrintf("\nvoid Switch_Background_Music_To(int Tune):  Old music track detected...\n");
      
    }

  Current_Tune=SILENCE;

  if( YGetSoundObjectAttributes( BackgroundMusic_con, ExpandedSoundSampleFilenames[ Tune ], 
				 &BackgroundMusic_sndobj_attrib ) )
    {
      // Can't get sound object attributes.
      fprintf( stderr, "\nvoid Play_YIFF_BackgroundMusic(int Tune): %s: Error: Missing or corrupt.\n", 
	       ExpandedSoundSampleFilenames[ Tune ] );
      printf(" CWD: %s \n\n",getcwd(NULL,0));
      Terminate(ERR);
    }
  else
    {
      BackgroundMusic_play_id = YStartPlaySoundObjectSimple( BackgroundMusic_con, 
							     ExpandedSoundSampleFilenames[ Tune ] );

      Current_Tune = Tune;

      Music_Parameters.repeats=0;
      Music_Parameters.total_repeats=-1; // -1 here means to repeat indefinately
      Music_Parameters.left_volume=1;
      Music_Parameters.right_volume=1;
      Music_Parameters.sample_rate=BackgroundMusic_sndobj_attrib.sample_rate;
      Music_Parameters.length=BackgroundMusic_sndobj_attrib.sample_size;
      Music_Parameters.position=0;
      Music_Parameters.yid=BackgroundMusic_play_id;
      Music_Parameters.flags=0xFFFFFFFF;
      YSetPlaySoundObjectValues( BackgroundMusic_con, BackgroundMusic_play_id, &Music_Parameters );
    }

  DebugPrintf("\nvoid Switch_Background_Music_To(int Tune):  end of function reached.\n");

#endif  /* HAVE_LIBY2 */
} // void Switch_Background_Music_To(int Tune)


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void Play_YIFF_Server_Sound(int Tune){

  // This function can and should only be compiled on machines, that have the YIFF sound
  // server installed.  Compilation therefore is optional and can be toggled with the following
  // definition.
#if HAVE_LIBY2

  printf("\nvoid Play_YIFF_Server_Sound(int Tune):  Real function call confirmed.");
  printf("\nvoid Play_YIFF_Server_Sound(int Tune):  Playback is about to start!");


  play_id = YStartPlaySoundObjectSimple( BackgroundMusic_con, ExpandedSoundSampleFilenames[Tune] );

  return;


  if (Tune == FIRESOUND) {
    // TEST   if( YGetSoundObjectAttributes( con, ExpandedFireSoundSampleFilename, &sndobj_attrib ) )
    //    if( YGetSoundObjectAttributes( BackgroundMusic_con, ExpandedFireSoundSampleFilename, &sndobj_attrib ) )
    //      {
	// Can't get sound object attributes.
    //  	fprintf( stderr, "\nvoid Play_YIFF_Server_Sound(int Tune): %s: Error: Missing or corrupt.\n", 
    //  		 ExpandedFireSoundSampleFilename );
    //	printf(" CWD: %s \n\n",getcwd(NULL,0));
    //  	Terminate(ERR);
    //      }
    //    else
    //      {
    //TEST	play_id = YStartPlaySoundObjectSimple( con, ExpandedFireSoundSampleFilename );
    // play_id = YStartPlaySoundObjectSimple( BackgroundMusic_con, ExpandedFireSoundSampleFilename );
    //      }
  }


#endif /* HAVE_LIBY2 */

} // void Play_YIFF_Server_Sound(int Tune)

/*@Function============================================================
@Desc: 

Connect to the Y server. (That is the yiff.) 
We pass NULL as the start argument, this means the Y server will not be started if it was detected to be not running. 
The connection argument is CON_ARG which is defined above as a constant. 
The connection argument is a string of the format ":".

@Ret: 
@Int:
* $Function----------------------------------------------------------*/

int Init_YIFF_Sound_Server(void){

  // This function can and should only be compiled on machines, that have the YIFF sound
  // server installed.  Compilation therefore is optional and can be toggled with the following
  // definition.
#if HAVE_LIBY2
  
  int i;

  // Because the yiff does not nescessarily have the same origin for relative paths as paradroid does,
  // is is nescessary to first translate our path names to absolute pathnames.  This is done here:

  for ( i=0; i < ALL_SOUNDS ; i++ ) {
    ExpandedSoundSampleFilenames[i] = ExpandFilename( SoundSampleFilenames[i] );
  }

  // Now a new connection to the yiff server can be opend.  The first argument to open is not NULL,
  // therefore a yiff server will be started even if none is running!!  great!!
  BackgroundMusic_con = YOpenConnection(
			"/usr/sbin/yiff",
			CON_ARG
			);

  // Attention!! First channel is to be opend now!
  if(BackgroundMusic_con == NULL) {
    // Failed to connect to the Y server. 
    fprintf(
	      stderr,
	      "%s: Cannot connect to YIFF server for background music.\n",
	      CON_ARG
	      );
      Terminate(ERR);
    }

  // Now a new connection to the yiff server can be opend.  The first argument to open is not NULL,
  // therefore a yiff server will be started even if none is running!!  great!!
  /* TEST
    con = YOpenConnection(
  			"yiff",
  			CON_ARG
  			);

  // Attention!! First channel is to be opend now!
  if(con == NULL) {
    // Failed to connect to the Y server. 
    fprintf(
	      stderr,
	      "%s: Cannot connect to YIFF server for first channel.\n",
	      CON_ARG
	      );
      Terminate(ERR);
    }

  // Attention!! Second channel is to be opend now!
  con2 = YOpenConnection(
			 NULL,
			 CON_ARG
			 );
  if(con2 == NULL) {
    // Failed to connect to the Y server. 
    fprintf( stderr, "%s: Cannot connect the YIFF server for second channel.\n", CON_ARG );
      Terminate(ERR);

    }

  */

  // The connection to the sound server should now be established...
  // Printing debug message and going on...

  printf("\nint Init_YIFF_Sound_Server(void): The connection to the sound server was established successfully...");

  // Load the Firesound

  //  if( YGetSoundObjectAttributes( con, FireSoundSampleFilename, &sndobj_attrib ) ) {
    // Can't get sound object attributes.
  //    fprintf( stderr, "\nvoid Init_YIFF_Sound_Server(void): %s: Error: Missing or corrupt.\n", 
  //	     FireSoundSampleFilename );
  //    Terminate(ERR);
  //  }

  //  
  //  if ((SoundDateihandle=fopen(FireSoundSampleFilename,"rb")) == NULL) {
  //    printf("\nint Init_OSS(void): Konnte die Datei %s nicht oeffnen !",FireSoundSampleFilename);
  //    getchar(); Terminate(-1);
  //  }
  //	
  //  if( fstat(fileno(SoundDateihandle), &stbuf) == EOF) Terminate(-1);
  //
  //  if( (FireSoundSamplePointer = (char*) MyMalloc((size_t)FragmentRoundUp(stbuf.st_size) + 10)) == NULL) {
  //    FireSoundSampleLength=stbuf.st_size;
  //    printf("\nint Init_OSS(void): Out of Memory?");
  //    printf("\nFree: %lu", coreleft() );
  //    getchar();
  //    Terminate(-1);
  //  }
  //
  //  fread(FireSoundSamplePointer, 1, (size_t)stbuf.st_size, SoundDateihandle);	
  //  SampleLaenge=stbuf.st_size;
  //
  //  if (fclose(SoundDateihandle) == EOF) {
  //    printf("\nint Init_OSS: Konnte die Datei %s nicht schlieáen !",FireSoundSampleFilename);
  //    getchar(); Terminate(-1);
  //  }
  //
  //  // Load the Collisionsound
  //
  //  if ((SoundDateihandle=fopen(CollisionSoundSampleFilename,"rb")) == NULL) {
  //    printf("\nint Init_OSS(void): Konnte die Datei %s nicht oeffnen !",CollisionSoundSampleFilename);
  //    getchar(); Terminate(-1);
  //  }
  //	
  //  if( fstat(fileno(SoundDateihandle), &stbuf) == EOF) Terminate(-1);
  //
  //  if( (CollisionSoundSamplePointer = (char*) MyMalloc((size_t)FragmentRoundUp(stbuf.st_size) + 10)) == NULL) {
  //    CollisionSoundSampleLength=stbuf.st_size;
  //    printf("\nint Init_OSS(void): Out of Memory?");
  //    printf("\nFree: %lu", coreleft() );
  //    getchar();
  //    Terminate(-1);
  //  }
  //	
  //  fread(CollisionSoundSamplePointer, 1, (size_t)stbuf.st_size, SoundDateihandle);	
  //  SampleLaenge=stbuf.st_size;
  //
  //  if (fclose(SoundDateihandle) == EOF) {
  //    printf("\nint Init_OSS: Konnte die Datei %s nicht schlieáen !",CollisionSoundSampleFilename);
  //    getchar(); Terminate(-1);
  //  }
  //
  //  // Load the Blastsound
  //
  //  if ((SoundDateihandle=fopen(BlastSoundSampleFilename,"rb")) == NULL) {
  //    printf("\nint Init_OSS(void): Konnte die Datei %s nicht oeffnen !",BlastSoundSampleFilename);
  //    getchar(); Terminate(-1);
  //  }
  //	
  //  if( fstat(fileno(SoundDateihandle), &stbuf) == EOF) Terminate(-1);
  //
  //  if( (BlastSoundSamplePointer = (char*) MyMalloc((size_t)FragmentRoundUp(stbuf.st_size) + 10)) == NULL) {
  //    BlastSoundSampleLength=stbuf.st_size;
  //    printf("\nint Init_OSS(void): Out of Memory?");
  //    printf("\nFree: %lu", coreleft() );
  //    getchar();
  //    Terminate(-1);
  //  }
  //	
  //  fread(BlastSoundSamplePointer, 1, (size_t)stbuf.st_size, SoundDateihandle);	
  //  SampleLaenge=stbuf.st_size;
  //
  //  if (fclose(SoundDateihandle) == EOF) {
  //    printf("\nint Init_OSS: Konnte die Datei %s nicht schlieáen !",BlastSoundSampleFilename);
  //    getchar(); Terminate(-1);
  //  }
  //
  //

#endif
  return(OK);
} // void Init_YIFF_Sound_Server(void)


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void GotHitSound(void){

  Play_YIFF_Server_Sound(GOT_HIT_SOUND);

}  // void GotHitSound(void)


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void GotIntoBlastSound(void){

  Play_YIFF_Server_Sound(GOT_INTO_BLAST_SOUND);
  
  return;
} // void GotIntoBlastSound(void)

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void RefreshSound(void){

  Play_YIFF_Server_Sound(REFRESH_SOUND);

  return;
} // void RefreshSound(void)


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void MoveElevatorSound(void){
  Play_YIFF_Server_Sound(MOVE_ELEVATOR_SOUND);
} // void MoveElevatorSound(void)


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void ThouArtDefeatedSound(void){
  Play_YIFF_Server_Sound( THOU_ART_DEFEATED_SOUND);
} // void MoveElevatorSound(void)


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void EnterElevatorSound(void){

  Play_YIFF_Server_Sound(ENTER_ELEVATOR_SOUND);

  return;
} // void EnterElevatorSound(void)


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void LeaveElevatorSound(void){

  Play_YIFF_Server_Sound(LEAVE_ELEVATOR_SOUND);

  return;
} // void LeaveElevatorSound(void)


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void FireBulletSound(void){

	Play_YIFF_Server_Sound(FIRESOUND);

} // void FireBulletSound(void)


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void BounceSound(void){

  Play_YIFF_Server_Sound(COLLISIONSOUND);
  
} // void BounceSound(void)

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void DruidBlastSound(void){

  Play_YIFF_Server_Sound(BLASTSOUND);
  
} // void BounceSound(void)


#undef _sound_c
