/*=@Header==============================================================
 * $Source$
 *
 * @Desc:
 *	 
 * 	
 * $Revision$
 * $State$
 *
 * $Author$
 *
 * $Log$
 * Revision 1.5  2002/04/08 19:19:09  rp
 * Johannes latest (and last) non-cvs version to be checked in. Added graphics,sound,map-subdirs. Sound support using ALSA started.
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

// static const char RCSid[]=\
// "$Id$";

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
#include "struct.h"
#include "global.h"
#include "proto.h"


// unsigned char* data;

int handle = -1;
int setting = 0x000C000D; // 12 fragments size 8kb
int channels = 0;         // 0=mono 1=stereo
int format = AFMT_U8;
int rate = 8000;

char BlastSoundSampleFilename[]="./sound/BlastSound1.wav";
char CollisionSoundSampleFilename[]="./sound/CollisionSound1.wav";
char FireSoundSampleFilename[]="./sound/FireSound2.wav";

long BlastSoundSampleLength=0;
long CollisionSoundSampleLength=0;
long FireSoundSampleLength=0;

unsigned char *BlastSoundSamplePointer;
unsigned char *CollisionSoundSamplePointer;
unsigned char *FireSoundSamplePointer;



void ExitProc() {
  if (handle != -1) {
    close( handle );
  }
}

unsigned char Kanal;

#define MODDIR		"d:\\mod\\parasnd\\"
#define MY_FIRE	"myfire.mod"

enum _devices {
  PC_SPEAKER = 0,
  SOUNDBLASTER = 7
};

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

tune FireBulletTune2={15,6,15,1,0,FALSE,FALSE,FALSE,FALSE,11,0,0,
		      13,8,8,8,0,FALSE,FALSE,FALSE,FALSE, 1,0,0,
		      700,0,FALSE,4};
tune FireBulletTune={15,4,15,0,0,FALSE,FALSE,FALSE,TRUE,11,0,3,
                     15,7,12,0,2,FALSE,FALSE,FALSE,TRUE, 1,0,3,
                     700,0,FALSE,2};

tune GotHitTune={9,2,4,15,0,FALSE,FALSE,FALSE,FALSE,1,0,0,
                 15,6,15,0,0,FALSE,FALSE,FALSE,FALSE,2,0,0,
                 700,0,FALSE,7};

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

/* Neu ! */
// tune BlastTune={6,6,7,5,0,FALSE,FALSE,FALSE,FALSE,5,0,0,
//		8,8,3,2,1,FALSE,FALSE,FALSE,FALSE,2,0,0,
//		200,3,TRUE,7};
// 
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


int Device = SOUNDBLASTER;		/* The mod-device */

// extern void pascal modvolume(int v1, int v2, int v3, int v4);
// extern void pascal moddevice(int *device);
// extern void pascal modsetup(int *status,
//	int device, int mixspeed, int pro, int loop, char *string);

// extern void pascal modstop(void);
// extern void pascal modinit(void);

void MakeSound(tune* ThisTune);
long FragmentRoundUp(long FormerLength);



long FragmentRoundUp(long FormerLength){
  long NewLength;

  NewLength=FormerLength/(8*1024);
  NewLength=(NewLength+1)*(8*1024);
  return NewLength;
} // long FragmentRoundUp


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

void Play_OSS(int Tune){

  printf("\nvoid Play_OSS(int Tune):  Real function call confirmed.");

  printf("\n Attention!  Playback is about to start!");

  // write(handle, data, 22050*4L);

  //write(handle, data+0x2C, FragmentRoundUp(SampleLaenge));

  if (Tune == FIRESOUND) {
    write(handle, FireSoundSamplePointer, FragmentRoundUp(FireSoundSampleLength));
  }

  if (Tune == COLLISIONSOUND) {
    write(handle, CollisionSoundSamplePointer, FragmentRoundUp(CollisionSoundSampleLength));
  }

  if (Tune == BLASTSOUND) {
    write(handle, BlastSoundSamplePointer, FragmentRoundUp(BlastSoundSampleLength));
  }

  printf("\n Attention!  Data have been written!");

} // void Play_OSS(int Tune)


/*@Function============================================================
@Desc: Funktion zur Ausgabe eines Wertes an einen Registerport.
		 Diese Funktion ist im SB-Profibuch S.66 zu finden.
@Ret: 
@Int:
* $Function----------------------------------------------------------*/

void out_sb(unsigned char sb_reg, unsigned char sb_data){
  //  outportb(0x388,sb_reg); 			
  //  sb_register[sb_reg]=sb_data;	
  //  delay(1);								
  //  outportb(0x389,sb_data);			
  //  delay(1);								
};


/*@Function============================================================
@Desc: Funktion zum Winlesen eines Wertes vom virtuellen Registerport.
		 Diese Funktion ist im SB-Profibuch auf S.66 zu finden.
@Ret: 
@Int:
* $Function----------------------------------------------------------*/

unsigned char in_sb(unsigned char sb_reg)
{
  return(sb_register[sb_reg]);	/* RÅckgabe des gepufferten Wertes */
};




/*@Function============================================================
@Desc: Funktion zur Initialisierung des SB. sollte anfangs augerufen werden.
		 Diese Funktion ist im SB-Profibuch auf S.66 zu finden.
@Ret: 
@Int:
* $Function----------------------------------------------------------*/

int Init_OSS(void){
  FILE* SoundDateihandle;
  struct stat stbuf;

  unsigned char* BeginningOfScreen;
	
  printf("\nAttention!  Opening OSS audio device for playback now!...");

  if ( (handle = open("/dev/dsp",O_WRONLY)) 
       == -1 ) {
    perror("open /dev/dsp");
    return -1;
  }

  printf("\nAttention!  Opening OSS audio device should have worked so far...");

  if ( atexit( ExitProc ) == -1 ) {
    perror("atexit");
    ExitProc();
    return -1;
  }

  printf("\nAttention!  Setting Buffer size, whatever that means....");

  if ( ioctl(handle, SNDCTL_DSP_SETFRAGMENT,
	     &setting) == -1 ) {
    perror("ioctl set fragment");
    return errno;
  }

  printf("\nAttention! Number of Channels will now be set....");

  if ( ioctl(handle, SNDCTL_DSP_STEREO,
	     &channels) == -1 ) {
    perror("ioctl stereo");
    return errno;
  }

  printf("\nAttention!  Now the Sample format is set....");

  if ( ioctl(handle, SNDCTL_DSP_SETFMT,
	     &format) == -1 ) {
    perror("ioctl format");
    return errno;
  }

  printf("\nAttention!  Now the speed for playback is set....");

  if ( ioctl(handle, SNDCTL_DSP_SPEED,
	     &rate) == -1 ) {
    perror("ioctl sample rate");
    return errno;
  }

  // two seconds two channels
  //data = malloc(22050*2*2L);
  //ptr = data;    
  //for (i = 0; i < 22050; ++i) {
    //    *ptr++ = 128;            // set left channel
    //    *ptr++ = 128;            // right silence
  //  *ptr++=0;
  //}
  //for (i = 22050; i < 44100; ++i) {
    //    *ptr++ = 128;
    //    *ptr++ = 128;
  //  *ptr++=0;
  //}

  // Load the sound samples used by paraplus (in wav format)

  // Load the Firesound

  if ((SoundDateihandle=fopen(FireSoundSampleFilename,"rb")) == NULL) {
    printf("\nint Init_OSS(void): Konnte die Datei %s nicht oeffnen !",FireSoundSampleFilename);
    getchar(); Terminate(-1);
  }
	
  if( fstat(fileno(SoundDateihandle), &stbuf) == EOF) Terminate(-1);

  if( (FireSoundSamplePointer = (char*) MyMalloc((size_t)FragmentRoundUp(stbuf.st_size) + 10)) == NULL) {
    FireSoundSampleLength=stbuf.st_size;
    printf("\nint Init_OSS(void): Out of Memory?");
    printf("\nFree: %lu", coreleft() );
    getchar();
    Terminate(-1);
  }

  fread(FireSoundSamplePointer, 1, (size_t)stbuf.st_size, SoundDateihandle);	
  SampleLaenge=stbuf.st_size;

  if (fclose(SoundDateihandle) == EOF) {
    printf("\nint Init_OSS: Konnte die Datei %s nicht schlie·en !",FireSoundSampleFilename);
    getchar(); Terminate(-1);
  }

  // Load the Collisionsound

  if ((SoundDateihandle=fopen(CollisionSoundSampleFilename,"rb")) == NULL) {
    printf("\nint Init_OSS(void): Konnte die Datei %s nicht oeffnen !",CollisionSoundSampleFilename);
    getchar(); Terminate(-1);
  }
	
  if( fstat(fileno(SoundDateihandle), &stbuf) == EOF) Terminate(-1);

  if( (CollisionSoundSamplePointer = (char*) MyMalloc((size_t)FragmentRoundUp(stbuf.st_size) + 10)) == NULL) {
    CollisionSoundSampleLength=stbuf.st_size;
    printf("\nint Init_OSS(void): Out of Memory?");
    printf("\nFree: %lu", coreleft() );
    getchar();
    Terminate(-1);
  }
	
  fread(CollisionSoundSamplePointer, 1, (size_t)stbuf.st_size, SoundDateihandle);	
  SampleLaenge=stbuf.st_size;

  if (fclose(SoundDateihandle) == EOF) {
    printf("\nint Init_OSS: Konnte die Datei %s nicht schlie·en !",CollisionSoundSampleFilename);
    getchar(); Terminate(-1);
  }

  // Load the Blastsound

  if ((SoundDateihandle=fopen(BlastSoundSampleFilename,"rb")) == NULL) {
    printf("\nint Init_OSS(void): Konnte die Datei %s nicht oeffnen !",BlastSoundSampleFilename);
    getchar(); Terminate(-1);
  }
	
  if( fstat(fileno(SoundDateihandle), &stbuf) == EOF) Terminate(-1);

  if( (BlastSoundSamplePointer = (char*) MyMalloc((size_t)FragmentRoundUp(stbuf.st_size) + 10)) == NULL) {
    BlastSoundSampleLength=stbuf.st_size;
    printf("\nint Init_OSS(void): Out of Memory?");
    printf("\nFree: %lu", coreleft() );
    getchar();
    Terminate(-1);
  }
	
  fread(BlastSoundSamplePointer, 1, (size_t)stbuf.st_size, SoundDateihandle);	
  SampleLaenge=stbuf.st_size;

  if (fclose(SoundDateihandle) == EOF) {
    printf("\nint Init_OSS: Konnte die Datei %s nicht schlie·en !",BlastSoundSampleFilename);
    getchar(); Terminate(-1);
  }






} // void Init_OSS(void)


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void GotHitSound(void){

	/* Sound "uber FM-Generatoren */
	MakeSound(&GotHitTune);
	/* oder "uber MOD-Abspielroutine */
	return;
}


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
	char tmp[200];		/* tmp - string */


	Play_OSS(FIRESOUND);

	//MakeSound(&FireBulletTune);
	//	/* Sound "uber FM-Generatoren */
	//	if( !ModPlayerOn ) MakeSound(&FireBulletTune);
	//	/* oder "uber MOD-Abspielroutine */
	//	else {
	//		StopModPlayer();
	//		strcpy(tmp, MODDIR);
	//		strcat(tmp, MY_FIRE);
	//		PlayMod(tmp);
	//	}
	//	return;
}


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void BounceSound(void){

  Play_OSS(COLLISIONSOUND);
  
}

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
  //	return OK;
}

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
