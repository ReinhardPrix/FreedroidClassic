/*=@Header==============================================================
 * $Source$
 *
 * @Desc:  all functions dealing with sound are contained in this file.
 *	 
 * 	
 * $Revision$
 *
 * $State$
 *
 * $Author$
 *
 * $Log$
 * Revision 1.11  1997/06/08 18:46:11  jprix
 * Sound server should be working perfectly now.  Background music was also activated.
 * Background music file and new fire sound added.
 * Soundserver is currently activated by default. (rp shound install a yiff soon! it's great.)
 *
 * Revision 1.10  1997/06/08 16:33:10  jprix
 * Eliminated all warnings, that resulted from the new -Wall gcc flag.
 *
 *
 * Revision 1.9  1997/06/08 14:49:40  jprix
 * Added file FILES describing the files of this project.
 * Added more doku while writing the files description.
 * Added -Wall compilerflag for maximal generation of sane warnings.
 *
 * Revision 1.8  1997/06/05 23:47:38  jprix
 * added some doku.  cleaned out some old doku and old code, that was allready commented out.
 *
 * Revision 1.7  1997/06/05 23:09:01  jprix
 * Project can now be compiled and run entirely without any yiff installation.
 *
 * Revision 1.6  1997/06/05 09:24:15  jprix
 * yiff server access introduced to the project. (This version was not compilable without YIFF. SORRY!)
 *
 * Revision 1.5  2002/04/08 19:19:09  rp
 * Johannes latest (and last) non-cvs version to be checked in. 
 * Added graphics,sound,map-subdirs. Sound support using ALSA started.
 *
 * Revision 1.5  1997/05/31 13:30:32  rprix
 * Further update by johannes. (sent to me in tar.gz)
 *
 * Revision 1.2  1994/06/19  16:41:10  prix
 * Sat Jun 04 08:42:14 1994: ??
 *
 * Revision 1.1  1993/05/23  21:05:16  prix
 * Initial revision
 *
 *
 *-@Header------------------------------------------------------------*/

/* static const char RCSid[]=\
   "$Id$"; */

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
#ifdef PARADROID_SOUND_SUPPORT_ON
#include <Y2/Y.h>         //  Basic Y types and constants. 
#include <Y2/Ylib.h>      //  YLib functions and structs. 
/* Change this to the address and port of the Y server you want
 * to connect to. Note that 127.0.0.1 is a symbolic address
 * meaning `localhost'.
 */
#define CON_ARG             "127.0.0.1:9433"
#endif
// Thanks a lot for leaving the above lines in.  They are for the yiff sound server!!

#include "struct.h"
#include "global.h"
#include "proto.h"


// unsigned char* data;

int handle = -1;
int setting = 0x000C000D; // 12 fragments size 8kb
int channels = 0;         // 0=mono 1=stereo
int format = AFMT_U8;
int rate = 8000;

char BlastSoundSampleFilename[]="/sound/BlastSound1.wav";
char CollisionSoundSampleFilename[]="/sound/CollisionSound1.wav";
char FireSoundSampleFilename[]="/sound/FireSound1.wav";
char BackgroundMusicSampleFilename[]="/sound/BackgroundMusic1.wav";
char* ExpandedBlastSoundSampleFilename;
char* ExpandedCollisionSoundSampleFilename;
char* ExpandedFireSoundSampleFilename;
char* ExpandedBackgroundMusicSampleFilename;

long BlastSoundSampleLength=0;
long CollisionSoundSampleLength=0;
long FireSoundSampleLength=0;

unsigned char *BlastSoundSamplePointer;
unsigned char *CollisionSoundSamplePointer;
unsigned char *FireSoundSamplePointer;


// The following Lines define several channels for sound output to the yiff sound server!!!
#ifdef PARADROID_SOUND_SUPPORT_ON
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
#endif
// The above Lines define several channels for sound output to the yiff sound server!!!

void ExitProc() {
  if (handle != -1) {
    close( handle );
  }
}

typedef struct{
  int car_att;
  int car_dec;
  int car_sus;
  int car_rel;
  int car_wel;
  int car_tre;
  int car_vib;
  int car_ton;
  int car_hue;
  int car_zei;
  int car_dae;
  int car_amp;
  
  int mod_att;
  int mod_dec;
  int mod_sus;
  int mod_rel;
  int mod_wel;
  int mod_tre;
  int mod_vib;
  int mod_ton;
  int mod_hue;
  int mod_zei;
  int mod_dae;
  int mod_amp;
  
  int freq;
  int okt;
  int verb;
  int rueck;
}tune;	

tune GotIntoBlastTune={12,6,15, 1,0,FALSE,FALSE,FALSE,TRUE,11,0,0,
		       12,2,15,15,0,FALSE,FALSE,FALSE,TRUE, 1,0,0,
		       100,0,TRUE,6};

tune MoveElevatorTune={15,3,8,3,0,TRUE,TRUE,TRUE,TRUE,11,0,0,
                       8,8,8,8,0,TRUE,TRUE,TRUE,TRUE,1,0,0,
                       500,1,0,4};

tune OvertakeTune={5,4,12,4,3,FALSE,FALSE,FALSE,FALSE,9,0,0,
		   13,3,8,2,2,TRUE,TRUE,TRUE,FALSE,1,10,0,
		   300,1,0,0};

tune CryTune1={3,4,6,3,3,TRUE,TRUE,FALSE,FALSE,3,0,0,
	       5,5,7,9,3,TRUE,TRUE,FALSE,TRUE,11,0,0,
	       400,0,TRUE,5};

/* Neu ! */
tune CryTune2={7,8,4,5,2,TRUE,TRUE,FALSE,FALSE,4,5,0,
	       3,6,7,3,0,TRUE,TRUE,FALSE,FALSE,7,0,0,
	       300,3,FALSE,5};

/* Neu ! */
tune StartOrLiftverlTune={13,4,8,5,2,TRUE,TRUE,FALSE,FALSE,9,0,0,
			  2,5,7,3,1,TRUE,TRUE,FALSE,FALSE,4,0,0,
			  400,2,FALSE,4};

/* Neu ! Nr. 8 */								  
tune TankenTune={7,4,8,5,1,TRUE,TRUE,FALSE,FALSE,2,0,0,
		 13,5,7,3,0,TRUE,TRUE,FALSE,FALSE,5,0,0,
		 400,2,TRUE,2};


tune HitHimTune1={6,6,7,5,0,FALSE,FALSE,FALSE,FALSE,5,0,0,
		  8,8,3,2,1,FALSE,FALSE,FALSE,FALSE,2,0,0,
		  200,3,TRUE,7};

tune AllSounds[]={ { 12,6,15, 1,0,FALSE,FALSE,FALSE,TRUE,11,0,0,
		     12,2,15,15,0,FALSE,FALSE,FALSE,TRUE, 1,0,0,
		     700,0,FALSE,6},
		   {	15,6,15,1,0,FALSE,FALSE,FALSE,FALSE,11,0,0,
			13,8,8,8,0,FALSE,FALSE,FALSE,FALSE, 1,0,0,
			700,0,FALSE,4},
                   {	15,4,15,0,0,FALSE,FALSE,FALSE,TRUE,11,0,3,
			15,7,12,0,2,FALSE,FALSE,FALSE,TRUE, 1,0,3,
			700,0,FALSE,2},
		   { 9,2,4,15,0,FALSE,FALSE,FALSE,FALSE,1,0,0,
		     15,6,15,0,0,FALSE,FALSE,FALSE,FALSE,2,0,0,
		     700,0,FALSE,7},
		   {	12,6,15, 1,0,FALSE,FALSE,FALSE,TRUE,11,0,0,
         	       	12,2,15,15,0,FALSE,FALSE,FALSE,TRUE, 1,0,0,
			100,0,TRUE,6},
                   {	15,3,8,3,0,TRUE,TRUE,TRUE,TRUE,11,0,0,
			8,8,8,8,0,TRUE,TRUE,TRUE,TRUE,1,0,0,
			500,1,0,4},
                   { 5,4,12,4,3,FALSE,FALSE,FALSE,FALSE,9,0,0,
		     13,3,8,2,2,TRUE,TRUE,TRUE,FALSE,1,10,0,
		     300,1,0,0},
		   {	3,4,6,3,3,TRUE,TRUE,FALSE,FALSE,3,0,0,
			5,5,7,9,3,TRUE,TRUE,FALSE,TRUE,11,0,0,
			400,0,TRUE,5},
		   { 7,8,4,5,2,TRUE,TRUE,FALSE,FALSE,4,5,0,
		     3,6,7,3,0,TRUE,TRUE,FALSE,FALSE,7,0,0,
		     300,3,FALSE,5},
		   { 13,4,8,5,2,TRUE,TRUE,FALSE,FALSE,9,0,0,
		     2,5,7,3,1,TRUE,TRUE,FALSE,FALSE,4,0,0,
		     400,2,FALSE,4},
		   { 7,4,8,5,1,TRUE,TRUE,FALSE,FALSE,2,0,0,
		     13,5,7,3,0,TRUE,TRUE,FALSE,FALSE,5,0,0,
		     400,2,TRUE,2},
		   { 6,6,7,5,0,FALSE,FALSE,FALSE,FALSE,5,0,0,
		     8,8,3,2,1,FALSE,FALSE,FALSE,FALSE,2,0,0,
		     200,3,TRUE,7}
};


int i;
unsigned char* ptr;
unsigned char v = 128;
int SampleLaenge;

void MakeSound(tune* ThisTune);
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
#ifdef PARADROID_SOUND_SUPPORT_ON

  // FIRST CHECK EVENTS FOR THE FIRST CHANNEL

  // Get the next event (if any) in the first Channel 

  if(YGetNextEvent( con, &event, False ) > 0)
    {
                                /* Sound object stopped playing? */
      if( (event.type == YSoundObjectKill) && (event.kill.yid == play_id) )
	{
	  /* Our play has stopped. */
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
	  /* Server shutdown. */
	  printf( "Y server shutdown, reason %i.\n", event.shutdown.reason );
	  Terminate(ERR);
	}
      else
	{
	  /* Some other Y event, ignore. */
	}
    }
  

  // NOW CHECK FOR EVENTS OF THE SECOND CHANNEL
  
  // Get the next event (if any) in the second channel 

  if(YGetNextEvent( con2, &event2, False ) > 0)
    {
      // Sound object stopped playing? 
      if((event2.type == YSoundObjectKill) &&
	 (event2.kill.yid == play_id2)
	 )
	{
	  /* Our play has stopped. */
	  printf("Done playing in the second channel.\n");
	}
      // Server disconnected us? 
      else if(event2.type == YDisconnect)
	{
	  // Got disconnected. 
	  printf( "Y server disconnected us, channel 2, reason %i.\n", event2.disconnect.reason );
	  Terminate(ERR);
	}
      // Server shutdown? 
      else if(event2.type == YShutdown)
	{
	  // Server shutdown. 
	  printf("Y server shutdown, reason %i.\n", event2.shutdown.reason );
	  Terminate(ERR);
	}
      else
	{
	  // Some other Y event, ignore. 
	}
    }
    
#endif

} // void YIFF_Server_Check_Events(void)

char *ExpandFilename(char *LocalFilename){
  char *tmp;

  tmp=malloc(strlen(LocalFilename)+strlen(getcwd(NULL,0)) + 1);
  strcpy(tmp,getcwd(NULL,0));
  strcat(tmp,LocalFilename);
  return(tmp);
} // char *ExpandFilename(char *LocalFilename){


void YIFF_Server_Close_Connections(void){

  // This function can and should only be compiled on machines, that have the YIFF sound
  // server installed.  Compilation therefore is optional and can be toggled with the following
  // definition.
#ifdef PARADROID_SOUND_SUPPORT_ON

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

  YCloseConnection(con, False);
  con = NULL;

  YCloseConnection(con2, False);
  con2 = NULL;

#endif

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

	MakeSound(&CryTune1);
}


/* **********************************************************************
   Diese Funktion erzeugt einen Ton mittels FM-Soundgeneratoren.
   Der Parameter ist ein Pointer auf eine Struktur, die die Tondaten
   enth"alt.
   ********************************************************************** */

void MakeSound(tune* ThisTune){

  printf("\nvoid MakeSound(tune* ThisTune):  Real function call confirmed.");

  // printf("\n Attention!  Playback is about to start!");

  // write(handle, data, 22050*4L);

  //write(handle, data+0x2C, FragmentRoundUp(SampleLaenge));

  // write(handle, data, FragmentRoundUp(SampleLaenge));

  // printf("\n Attention!  Data have been written!");

} // void MakeSound(tune* ThisTune)

void Play_YIFF_BackgroundMusic(int Tune){
  YEventSoundPlay Music_Parameters;

  printf("\nvoid Play_YIFF_BackgroundMusic(int Tune):  Real function call confirmed.\n");
  if( YGetSoundObjectAttributes( BackgroundMusic_con, ExpandedBackgroundMusicSampleFilename, 
				 &BackgroundMusic_sndobj_attrib ) )
    {
      // Can't get sound object attributes.
      fprintf( stderr, "\nvoid Play_YIFF_BackgroundMusic(int Tune): %s: Error: Missing or corrupt.\n", 
	       ExpandedBackgroundMusicSampleFilename );
      printf(" CWD: %s \n\n",getcwd(NULL,0));
      Terminate(ERR);
    }
  else
    {
      BackgroundMusic_play_id = YStartPlaySoundObjectSimple( BackgroundMusic_con, ExpandedBackgroundMusicSampleFilename );

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
} // Play_YIFF_BackgroundMusic(int Tune)

void Play_YIFF_Server_Sound(int Tune){
  //  static int previous_channel;
#define NUMBER_OF_CHANNELS 2


  // This function can and should only be compiled on machines, that have the YIFF sound
  // server installed.  Compilation therefore is optional and can be toggled with the following
  // definition.
#ifdef PARADROID_SOUND_SUPPORT_ON

  printf("\nvoid Play_YIFF_Server_Sound(int Tune):  Real function call confirmed.");

  printf("\nvoid Play_YIFF_Server_Sound(int Tune):  Playback is about to start!");

  if (Tune == FIRESOUND) {
    if( YGetSoundObjectAttributes( con, ExpandedFireSoundSampleFilename, &sndobj_attrib ) )
      {
	// Can't get sound object attributes.
  	fprintf( stderr, "\nvoid Play_YIFF_Server_Sound(int Tune): %s: Error: Missing or corrupt.\n", 
  		 ExpandedFireSoundSampleFilename );
	printf(" CWD: %s \n\n",getcwd(NULL,0));
  	Terminate(ERR);
      }
    else
      {
	play_id = YStartPlaySoundObjectSimple( con, ExpandedFireSoundSampleFilename );
      }
  }



  if (Tune == COLLISIONSOUND) {
    play_id = YStartPlaySoundObjectSimple( con, ExpandedCollisionSoundSampleFilename );
    // write(handle, CollisionSoundSamplePointer, FragmentRoundUp(CollisionSoundSampleLength));
  }

  if (Tune == BLASTSOUND) {
    play_id = YStartPlaySoundObjectSimple( con, ExpandedBlastSoundSampleFilename );
  }

#endif

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
#ifdef PARADROID_SOUND_SUPPORT_ON
  
  // Because the yiff does not nescessarily have the same origin for relative paths as paradroid does,
  // is is nescessary to first translate our path names to absolute pathnames.  This is done here:
  ExpandedBlastSoundSampleFilename=ExpandFilename(BlastSoundSampleFilename);
  ExpandedCollisionSoundSampleFilename=ExpandFilename(CollisionSoundSampleFilename);
  ExpandedFireSoundSampleFilename=ExpandFilename(FireSoundSampleFilename);
  ExpandedBackgroundMusicSampleFilename=ExpandFilename(BackgroundMusicSampleFilename);

  // Now a new connection to the yiff server can be opend.  The first argument to open is not NULL,
  // therefore a yiff server will be started even if none is running!!  great!!
  BackgroundMusic_con = YOpenConnection(
			"yiff",
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

}  // void GotHitSound(void)


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void GotIntoBlastSound(void){

	/* Sound "uber FM-Generatoren */
	MakeSound(&GotIntoBlastTune);
	/* oder "uber MOD-Abspielroutine */
	return;
}

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void RefreshSound(void){

	/* Sound "uber FM-Generatoren */
//	MakeSound(&MoveElevatorTune);
	MakeSound(&TankenTune);
	/* oder "uber MOD-Abspielroutine */
	return;
}


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void MoveElevatorSound(void){

	/* Sound "uber FM-Generatoren */
	MakeSound(&MoveElevatorTune);
	/* oder "uber MOD-Abspielroutine */
	return;
}


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void EnterElevatorSound(void){

	/* Sound "uber FM-Generatoren */
	MakeSound(&MoveElevatorTune);
	/* oder "uber MOD-Abspielroutine */
	return;
}


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void LeaveElevatorSound(void){

	/* Sound "uber FM-Generatoren */
	MakeSound(&MoveElevatorTune);
	/* oder "uber MOD-Abspielroutine */
	return;
}


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
@Desc: InitModPlayer():		initialisiert die mod-play Funktionen

@Ret: int ERR/OK
@Int:
* $Function----------------------------------------------------------*/
int InitModPlayer(void)
{
  //	static AllreadyInitialized=0;
  //	
  //	if (!AllreadyInitialized) modinit(); else {
  //		printf(" MODPLAYER ALLREADY INITIALIZED.\n");
  //		getchar();
  //		return OK;
  //	}
  //	AllreadyInitialized=1;
  return OK;
} // void InitModPlayer(void)

/*@Function============================================================
@Desc: PlayMod();

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void PlayMod(char *modfile)
{
  //	int mix, stat, pro, loop;
  //	char ch;
  //	char pasc_md[200];		/* Pascal has other strings than C !! */
  //
  //	if( !ModPlayerOn ) return;
  //	
  //	mix = 10000;
  //	pro = 0;
  //	loop = 0;
  //	
  //	modvolume(255, 255, 255, 255);
  //
  //	pasc_md[0] = strlen(modfile);
  //	strcpy(&(pasc_md[1]), modfile);
  //	
  //	modsetup(&stat, Device, mix, pro, loop, pasc_md);
  //	if( stat != 0) printf("Mod: %s	stat: %d", pasc_md, stat);
  //	
  //	if( stat != 0 ) {
  //		printf("\nModfile-error !!");
  //		return;
  //	}
  //	
  //	return;
}

/*@Function============================================================
@Desc: StopModPlayer();		stoppt den Mod-Player

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void StopModPlayer(void)
{
  //	if( ModPlayerOn ) modstop();

} /* StopModPlayer */

#undef _sound_c
