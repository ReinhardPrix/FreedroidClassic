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
 * Revision 1.2  1994/06/19 16:41:10  prix
 * Sat Jun 04 08:42:14 1994: ??
 *
 * Revision 1.1  1993/05/23  21:05:16  prix
 * Initial revision
 *
 *
 *-@Header------------------------------------------------------------*/

static const char RCSid[]=\
"$Id$";

#ifndef _sound_c
#define _sound_c
#endif

#include <stdio.h>
#include <string.h>
#include <dos.h>
#include <conio.h>

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"
#include "fm_hrd.h"

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

tune BounceTune={12,6,15, 1,0,FALSE,FALSE,FALSE,TRUE,11,0,0,
                 12,2,15,15,0,FALSE,FALSE,FALSE,TRUE, 1,0,0,
                 700,0,FALSE,6};

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
tune BlastTune={6,6,7,5,0,FALSE,FALSE,FALSE,FALSE,5,0,0,
					 8,8,3,2,1,FALSE,FALSE,FALSE,FALSE,2,0,0,
					 200,3,TRUE,7};
					 
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
                       
int Device = SOUNDBLASTER;		/* The mod-device */

extern void pascal modvolume(int v1, int v2, int v3, int v4);
extern void pascal moddevice(int *device);
extern void pascal modsetup(int *status,
	int device, int mixspeed, int pro, int loop, char *string);

extern void pascal modstop(void);
extern void pascal modinit(void);

void MakeSound(tune* ThisTune);

/*@Function============================================================
@Desc: Starts a Tune.

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void StartSound(int Tune){
	MakeSound(&(AllSounds[Tune]));
}

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void CrySound(void)
{
/*	static ThisSound=0;

	if (Me.status == TRANSFERMODE) {
		printf(" Neuer Wert :");
		scanf("%d",&ThisSound);
	}
	MakeSound(&(AllSounds[ThisSound]));
*/

	MakeSound(&CryTune1);
}


/* **********************************************************************
   Diese Funktion erzeugt einen Ton mittels FM-Soundgeneratoren.
   Der Parameter ist ein Pointer auf eine Struktur, die die Tondaten
   enth"alt.
   ********************************************************************** */
void MakeSound(tune* ThisTune){
	unsigned char car,mod,kan;

	kan=Kanal;
	Kanal++;
	if (Kanal>SBCHANNELS) Kanal=0;
	car=fm_carrier[kan];
	mod=fm_modulator[kan];

/* Sound zuerst einmal abschalten */
	sbfm_ton(kan,FALSE);

/* Tremolo ausschalten */
	sbfm_tremolo(car,ThisTune->car_tre);
	sbfm_tremolo(mod,ThisTune->mod_tre);

/* Vibrato ausschalten */
	sbfm_vibrato(car,ThisTune->car_vib);
	sbfm_vibrato(mod,ThisTune->mod_vib);

/* Huellkurvenart festleben */
	sbfm_tonart(car,ThisTune->car_ton);
	sbfm_tonart(mod,ThisTune->mod_ton);

/* Huellkurvendaempfung festlegen */
	sbfm_huelldaempf(car,ThisTune->car_hue);
	sbfm_huelldaempf(mod,ThisTune->mod_hue);

/*	Multiplikationsfaktor festlegen */
	sbfm_zeitfakt(car,ThisTune->car_zei);
	sbfm_zeitfakt(mod,ThisTune->mod_zei);
	
/* Daempfung einstellen */
	sbfm_daempfung(car,ThisTune->car_dae);
	sbfm_daempfung(mod,ThisTune->mod_dae);

/* Amplitudendaempfung einstellen */
	sbfm_ampldaempf(car,ThisTune->car_amp);
	sbfm_ampldaempf(mod,ThisTune->mod_amp);

/* Attackwerte festlegen */
	sbfm_attack(car,ThisTune->car_att);
	sbfm_attack(mod,ThisTune->mod_att);

/* decaywerte festlegen */
	sbfm_decay(car,ThisTune->car_dec);
	sbfm_decay(mod,ThisTune->mod_dec);
	
/* Sustainwerte festlegen */
	sbfm_sustain(car,ThisTune->car_sus);
	sbfm_sustain(mod,ThisTune->mod_sus);

/* Releasewerte festlegen */
	sbfm_release(car,ThisTune->car_rel);
	sbfm_release(mod,ThisTune->mod_rel);

/* Wellenform festlegen */
	sbfm_welle(car,ThisTune->car_wel);
	sbfm_welle(mod,ThisTune->mod_wel);
	
/* Frequenz fuer diesen Kanal festlegen */
	sbfm_frequ(kan,ThisTune->freq);

/* Oktave festlegen */
	sbfm_oktave(kan,ThisTune->okt);

/* Zellenverknuepfung festlegen */
	sbfm_verbind(kan,ThisTune->verb);
	
/* Rueckkopplungsgrad festlegen */
	sbfm_rueckkoppl(kan,ThisTune->rueck);
	
/* Ton aktivieren */
	sbfm_ton(kan,TRUE);
}

/*@Function============================================================
@Desc: Funktion zur Ausgabe eines Wertes an einen Registerport.
		 Diese Funktion ist im SB-Profibuch S.66 zu finden.
@Ret: 
@Int:
* $Function----------------------------------------------------------*/

void out_sb(unsigned char sb_reg, unsigned char sb_data){
  outportb(0x388,sb_reg); 			
  sb_register[sb_reg]=sb_data;	
  delay(1);								
  outportb(0x389,sb_data);			
  delay(1);								
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

void init_sb(void){
	if (sbfm_init() == 2) {
		printf(" Keine Soundblasterkarte installiert !");
		getch();
	}
	if (sbfm_init() == 1) {
		/*
		printf(" BLASTER ist nicht gesetzt ! ");
		getch();
		*/
	}
};


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

	/* Sound "uber FM-Generatoren */
	if( !ModPlayerOn ) MakeSound(&FireBulletTune);
	/* oder "uber MOD-Abspielroutine */
	else {
		StopModPlayer();
		strcpy(tmp, MODDIR);
		strcat(tmp, MY_FIRE);
		PlayMod(tmp);
	}
	return;
}


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void BounceSound(void){

	/* Sound "uber FM-Generatoren */
	MakeSound(&BounceTune);
	/* oder "uber MOD-Abspielroutine */
	return;
}

/*@Function============================================================
@Desc: InitModPlayer():		initialisiert die mod-play Funktionen

@Ret: int ERR/OK
@Int:
* $Function----------------------------------------------------------*/
int InitModPlayer(void)
{
	static AllreadyInitialized=0;
	
	if (!AllreadyInitialized) modinit(); else {
		printf(" MODPLAYER ALLREADY INITIALIZED.\n");
		getch();
		return OK;
	}
	AllreadyInitialized=1;
	return OK;
}

/*@Function============================================================
@Desc: PlayMod();

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void PlayMod(char *modfile)
{
	int mix, stat, pro, loop;
	char ch;
	char pasc_md[200];		/* Pascal has other strings than C !! */

	if( !ModPlayerOn ) return;
	
	mix = 10000;
	pro = 0;
	loop = 0;
	
	modvolume(255, 255, 255, 255);

	pasc_md[0] = strlen(modfile);
	strcpy(&(pasc_md[1]), modfile);
	
	modsetup(&stat, Device, mix, pro, loop, pasc_md);
	if( stat != 0) printf("Mod: %s	stat: %d", pasc_md, stat);
	
	if( stat != 0 ) {
		printf("\nModfile-error !!");
		return;
	}
	
	return;
}

/*@Function============================================================
@Desc: StopModPlayer();		stoppt den Mod-Player

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void StopModPlayer(void)
{
	if( ModPlayerOn ) modstop();

} /* StopModPlayer */

#undef _sound_c
