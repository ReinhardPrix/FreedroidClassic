/*=@Header==============================================================
 * $Source$
 *
 * @Desc: Graphics primitived, such as functions to load LBM or PCX images, to change the vga color table,
 *        to activate or deachtivate monitor signal, to set video modes etc.
 * 	
 * $Revision$
 *
 * $State$
 *
 * $Author$
 *
 * $Log: graphics.c,v 
 * Revision 1.5  1997/05/31 13:30:31  rpri
 * Further update by johannes. (sent to me in tar.gz
 *
 * Revision 1.2  1994/06/19  16:21:33  prix
 * Sat May 21 07:37:55 1994: Neue Funktion: TransparentLadeLBMBild
 * Sat May 21 09:57:45 1994: Transp.LBMLaden kann auch in den Speicher laden.
 * Sat May 21 10:07:25 1994: LadeLBMBild kann auch in den Speicher laden.
 * Sat May 21 11:19:57 1994: :LBMBIlder ver"andern nicht unbedingt die Pallete
 * Sat May 21 13:24:56 1994: UnfadeLevel eingef"uhrt
 *
 * Revision 1.1  1993/05/22  20:53:58  rp
 * Initial revision
 *
 *
 *-@Header------------------------------------------------------------*/

/* static const char RCSid[]=\
   "$Id$"; */

#define _graphics_c


#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/msg.h>
#include <errno.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <vga.h>
#include <vgagl.h>
#include <vgakeyboard.h>

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"
#include "map.h"
#include "paratext.h"
#include "colodefs.h"

extern int TimerFlag;


/*@Function============================================================
@Desc: 	int InitPictures(void):
				get the pics for: druids, bullets, blasts
				
				reads all blocks and puts the right pointers into
				the various structs

@Ret: FALSE: ERROR  	TRUE: OK
@Int:
* $Function----------------------------------------------------------*/
int InitPictures(void) {
  int i;

  /* First read the map blocks */
  GetMapBlocks();

  /* Get the enemy-blocks */
  GetBlocks(ENEMYBILD, 0, 0);
  Enemypointer = GetBlocks(NULL, 0, ENEMYPHASES);

  /* Get the influence-blocks */
  GetBlocks(INFLUENCEBILD, 0, 0);
  Influencepointer = GetBlocks(NULL, 0, ENEMYPHASES);
	
  /* the same game for the bullets */
  GetBlocks(BULLETBILD, 0, 0);
  for (i=0; i<ALLBULLETTYPES; i++) {
    Bulletmap[i].picpointer = GetBlocks(NULL, i, Bulletmap[i].phases);
  }

  /* ...and the blasts */
  GetBlocks(BLASTBILD, 0, 0);
  for (i=0; i<ALLBLASTTYPES; i++) {
    Blastmap[i].picpointer = GetBlocks(NULL, i, Blastmap[i].phases);
  }

  /* Get the Frame */
  LadeLBMBild(RAHMENBILD1,InternalScreen,FALSE); 
  RahmenPicture = (unsigned char *)MyMalloc(RAHMENBREITE*RAHMENHOEHE+10);
  IsolateBlock(InternalScreen, RahmenPicture, 0, 0, RAHMENBREITE, RAHMENHOEHE);

  /* get the Elevator-Blocks */
  ElevatorBlocks = (unsigned char*)MyMalloc(NUM_EL_BLOCKS*EL_BLOCK_MEM+100);
  LadeLBMBild(EL_BLOCKS_FILE,InternalScreen,FALSE);
  for( i=0; i<NUM_EL_BLOCKS; i++)
    IsolateBlock(
		 InternalScreen, 
		 ElevatorBlocks+i*EL_BLOCK_MEM,
		 1+i*(EL_BLOCK_LEN+1), 1,
		 EL_BLOCK_LEN, EL_BLOCK_HEIGHT);
		

  /* get Menublocks */
  LadeLBMBild(CONSOLENBILD,InternalScreen,FALSE);
  MenuItemPointer=MyMalloc(MENUITEMMEM);
  IsolateBlock(InternalScreen, MenuItemPointer, 0, 0, MENUITEMLENGTH, MENUITEMHEIGHT);	

  return TRUE;
}  // int InitPictures(void)

/*@Function============================================================
@Desc: MemSearch(): Sucht Binary-Area zwischen SStart und SEnd nach
SString ab. liefer Pointer auf gef. String oder NULL

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void SwapScreen(void)
{
  int y;

  for(y=0;y<SCREENHOEHE;y++) {
    vga_drawscanline(y,InternalScreen+SCREENBREITE*y);
  }
} // void SwapScreen(void)

/*@Function============================================================
@Desc: MemSearch(): Sucht Binary-Area zwischen SStart und SEnd nach
SString ab. liefer Pointer auf gef. String oder NULL

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void ClearVGAScreen(void)
{
  char* LocalBlackLinePointer;
  int y;

  LocalBlackLinePointer=malloc(SCREENBREITE+10);
  memset(LocalBlackLinePointer,0,SCREENBREITE);
  
  for(y=0;y<SCREENHOEHE;y++) {
    vga_drawscanline(y,LocalBlackLinePointer);
  }

  free(LocalBlackLinePointer);
} // void ClearVGAScreen(void)

unsigned char* MemSearch(
		unsigned char* SStart,

		unsigned char* SEnd,
		unsigned char *SString);
		
void PlusDrawEnergyBar(void);

/*@Function============================================================
@Desc: MemSearch(): Sucht Binary-Area zwischen SStart und SEnd nach
SString ab. liefer Pointer auf gef. String oder NULL

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
unsigned char* MemSearch(
			 unsigned char *SStart,
			 unsigned char *SEnd,
			 unsigned char *SString)
{
  register unsigned char *tmp;		// bewegl. Zeiger auf Search-Area 
  register unsigned char *string;	// bewegl. Zeiger auf ges. STring 
  unsigned char *firstchar;			// Zeiger auf Beginn der Uebereinstimmung 
  tmp = SStart;

  while(1)
    {
      string = SString;
      
      // Erste "Ubereinstimmung finden
      while( (*tmp != *string) && (tmp < SEnd) ) tmp++;
      if( tmp >= SEnd ) return NULL;	// Suchbereich schon verlassen
      
      firstchar = tmp;					// Beginn der Uebereinst. merken
		
      // Rest vergleichen
		string++;
		tmp ++;
		while(*string && (tmp < SEnd) && (*string == *tmp)) {
			string ++;
			tmp++;
		}

		// Falls Ende des String erreicht ---> gefunden !!!
		if( *string == '\0') return firstchar;

		// Falls Ende des Suchbereichs erreicht --> nix gefunden !!
		if( tmp >= SEnd ) return NULL;

		// sonst: weitersuchen
		tmp = firstchar + 1;		// nach voriger Uebereinst. weiter
		continue;
	} // while
	
} /* MemSearch */

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void SetColors(int FirstCol, int PalAnz, char* PalPtr)
{
  char* MyPalPtr;
  int i;

  MyPalPtr=PalPtr;
  // IM ZUGE DES PORTS MUESSEN DIE FOLGENDEN ANWEISUNGEN RAUS:
  //
  //unsigned int PalOfs,PalSeg;
  //	PalOfs=FP_OFF(PalPtr);
  //	PalSeg=FP_SEG(PalPtr);
  //	asm{
  //		push es
  //		mov ax,1012h
  //	mov bx,FirstCol
  //	mov cx,PalAnz
  //	mov dx,PalSeg
  //	mov es,dx
  //	mov dx,PalOfs
  //	int 10h
  //	pop es
  // }

  // IM ZUGE DES PORTS KOMMEN DAFUER DIE FOLGENDEN ANWEISUNGEN:

  for (i=FirstCol; i<FirstCol+PalAnz; i++) {
    vga_setpalette(i,MyPalPtr[0],MyPalPtr[1],MyPalPtr[2]);
    MyPalPtr+=3;
  }
} // void SetColors(...)

/*@Function============================================================
@Desc: Monitorsignalunterbrechung(int): steuert das Monitorsignal

@Ret:
@Int:
* $Function----------------------------------------------------------*/
void Monitorsignalunterbrechung(int Signal){
  //	asm{
  //		mov ax,Signal
  //		mov ah,12h
  //		mov bl,36h
  //		int 10h
  //	}
}

/*@Function============================================================
@Desc: InitPalette(): laedt die allgemein gueltige Palette fuer die
						LBM- Bilder 

@Ret: OK | ERR
@Int:
* $Function----------------------------------------------------------*/
int InitPalette(void) {
  /* Hier sollte die Palette geladen werden */
  LadeLBMBild(PALBILD,InternalScreen,TRUE);
  return OK;
} // int InitPalette(void)


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
int InitLevelColorTable(void){
  FILE* ColorFile;
  int i;

  LevelColorArray=MyMalloc(ALLLEVELCOLORS*3*FARBENPROLEVEL+10);	// NICHT FREIGEBEN
  if ( (ColorFile=fopen(COLORFILE,"r")) == NULL ) return (FALSE);
  for (i=0;i<ALLLEVELCOLORS*FARBENPROLEVEL;i++) {
    if (fscanf(ColorFile,"%hhd %hhd %hhd", &LevelColorArray[i*3],
	       &LevelColorArray[3*i+1],&LevelColorArray[3*i+2]) == EOF)
      return (FALSE);
  }
  fclose(ColorFile);
  return (TRUE);
} // int InitLevelColorTable(void)

/*@Function============================================================
@Desc: 

 @Ret: 
@Int:
* $Function----------------------------------------------------------*/
void SetLevelColor(int ColorEntry){

  SetColors(FIRSTBLOCKCOLOR,FARBENPROLEVEL,LevelColorArray+ColorEntry*3*FARBENPROLEVEL);

} // void SetLevelColor(int ColorEntry)

/*@Function============================================================
@Desc: LadeLBMBild(char*,unsigned char*,int):

Diese Prozedur ist fuer das laden ganzer IFF/ILBM-Bilder zust„ndig. Zuerst
wird ausreichend Speicher reserviert, dann die Bilddatei in den Speicher
geladen. Das decodiieren der eigentlichen Bilddaten uebernimmt dann eine
Assemblerroutine. Erst wenn das Bild fertig im Bildschirmspeicher steht
werden die Farbinformationen eingetragen.
	
@Ret: none
@Int:
* $Function----------------------------------------------------------*/
void LadeLBMBild(char* LBMDateiname,unsigned char* Screen,int LoadPal)
{
  // MODIFIED FOR THE PORT!!!!
  // Variables used by the function
  char* BodyPtr;
  long BytesWritten=0;
  FILE* BildDateihandle;
  char* BildDateiPointer;
  struct stat stbuf;
  int i;
  int y;

  unsigned char* BeginningOfScreen;
	
  BeginningOfScreen=Screen;

  /* *******************  ILBM-Bild laden und anzeigen  ******************** */

  /* Speicher fuer die Bildbearbeitung reservieren */

  if ((BildDateihandle=fopen(LBMDateiname,"rb")) == NULL) {
    printf("\nLadeLBM: Konnte die Datei %s nicht oeffnen !",LBMDateiname);
    getchar(); Terminate(-1);
  }
	
  if( fstat(fileno(BildDateihandle), &stbuf) == EOF) Terminate(-1);

  if( (BildDateiPointer = (char*) MyMalloc((size_t)stbuf.st_size + 10)) == NULL) {
    printf("\nOut of Memory in LadeLBMBild()");
    getchar();
    Terminate(-1);
  }

	
  /* File von Diskette in den reservierten Speicher laden */

  fread(BildDateiPointer, 1, (size_t)stbuf.st_size, BildDateihandle);	
  if (fclose(BildDateihandle) == EOF) {
    printf("\nLadeLBM: Konnte die Datei %s nicht schlieáen !",LBMDateiname);
    getchar(); Terminate(-1);
  }
	

  // First Part: Decode the Body of the LBM-File
  BodyPtr=(char *)MemSearch(
			    (unsigned char*)BildDateiPointer,
			    (unsigned char*)(BildDateiPointer + (int)stbuf.st_size),
			    (unsigned char*)"BODY");		
  BodyPtr+=strlen("BODY")+4;

	
  while(BytesWritten < 63999)
    {
      if (*BodyPtr < 0)
	{
	  memset(Screen,BodyPtr[1],abs(BodyPtr[0])+1);
	  BytesWritten+=abs(BodyPtr[0])+1;
	  Screen+=abs(BodyPtr[0])+1;
	  BodyPtr+=2;
	} else {
	  memcpy(Screen,BodyPtr+1,BodyPtr[0]+1);
	  BytesWritten+=BodyPtr[0]+1;
	  Screen+=BodyPtr[0]+1;
	  BodyPtr+=BodyPtr[0]+2;
	}
    }


  if( LoadPal ) {			
    // Second Part: Decode the Color-Information		
    BodyPtr=(char*)MemSearch(
			     (unsigned char*)BildDateiPointer,
			     (unsigned char*)(BildDateiPointer + (int)stbuf.st_size),
			     (unsigned char*)"CMAP");    
    BodyPtr+=strlen("CMAP")+4;
    for(i=0;i<(256*3);i++)
      {
	BodyPtr[i]=BodyPtr[i]>>2;
      }
    SetColors(0,255,BodyPtr);
  } /* if LoadPal */

  free(BildDateiPointer);
  printf("\nLadeLBM: Die Datei %s sollte nun erfolgreich geladen worden sein!",LBMDateiname);

  for (y=0;y<199;y++) {
    vga_drawscanline(y,BeginningOfScreen);
    BeginningOfScreen+=320;
  }

  printf("\nLadeLBM: Die Datei %s sollte nun erfolgreich geladen worden sein!",LBMDateiname);
} // LadeLBMBild 



/* *********************************************************************** */

void Set_SVGALIB_Video_ON(void) {
  int vgamode;

  printf("\n\n    Die SVGALIB wird nun initialisiert.... gleich gehts los.... \n\n");
  //  getchar();
  vga_init();
  vgamode=vga_getdefaultmode();
  if ((vgamode == -1) || (vga_getmodeinfo(vgamode)->bytesperpixel != 1)) vgamode = G320x200x256;
  
  if (!vga_hasmode(vgamode)) {
    printf("Mode not available.\n");
    exit(1);
  }

  vga_setmode(vgamode);
  gl_setcontextvga(vgamode);
  gl_enableclipping();
  // Initiate fonts of the svgalib (gl-part)!
  gl_setfont(8, 8, gl_font8x8);
  gl_setwritemode(FONT_COMPRESSED + WRITEMODE_OVERWRITE);
  gl_setfontcolors(0, vga_white());
  // Initiate raw keyboard access...
  printf("\n\n    Die Tastatur wird nun fuer die svgalib initialisiert.... gleich gehts los!\n");
  if (keyboard_init()) {
    printf("FEHLER! FEHLER! Keyboard konnte nicht initialisiert werden!!!!!");
    Terminate(ERR);
  }
  // Translate to 4 keypad cursor keys, and unify enter key. 
  keyboard_translatekeys(TRANSLATE_CURSORKEYS | TRANSLATE_KEYPADENTER |
			 TRANSLATE_DIAGONAL);       
  /* (TRANSLATE_DIAGONAL seems to give problems.) Michael: No doesn't...
     but might not do what you expect.. */

} // void Set_SVGALIB_Video_ON(void)

void Set_SVGALIB_Video_OFF(void) {

  printf("\nvoid Set_SVGALIB_Video_OFF(void): shutting svga-keyboard back to normal.....\n");
  keyboard_close();

  printf("\nvoid Set_SVGALIB_Video_OFF(void): shutting console back to text mode....\n");
  vga_setmode(TEXT);

} // Set_SVGALIB_Video_OFF(void)

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void UnfadeLevel(void)
{
  char* CMAPBuffer;
  int i,j,Color;

  Color=CurLevel->color;
  if (CurLevel->empty) Color=PD_DARK;
	
  GetView();
  GetInternFenster();
  PutInternFenster();
	
  /* Speicher reservieren */
  CMAPBuffer=MyMalloc(257*3);	// wird wieder freigegeben
  memset(CMAPBuffer,0,256*3);

  for(j=0;j<64;j++){
    for(i=3;i<((1+FARBENPROLEVEL)*3+1);i++){
      if (CMAPBuffer[i]<LevelColorArray[(i-3)+(Color*3*FARBENPROLEVEL)]) CMAPBuffer[i]++;
    }
    SetColors(1,FARBENPROLEVEL,CMAPBuffer+3);
  }	
  free(CMAPBuffer);
}  // void UnfadeLevel(void)

/*@Function============================================================
@Desc: Wie FadeColors1 nur auf die FARBENPROLEVEL Farben beschr"ankt.

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void FadeLevel(void)
{
  // char* CMAPBuffer;
  // int CMAPOffset,CMAPSegment,i,ii;

  // 
  // THIS CRAP WAS IN BEFORE THE PORT!!!!!!!!!!
  //
  /* Speicherplatz fuer die Farbregister reservieren  */
  // CMAPBuffer=MyMalloc(257*3);		// wird wieder freigegeben 
  // CMAPOffset=FP_OFF(CMAPBuffer);
  // CMAPSegment=FP_SEG(CMAPBuffer);

  /* Farbregisterwerte aus den DAC-Registern auslesen */
  //	asm{
  //	mov ax,1017h
  //	mov bx,1
  //	mov dx,CMAPOffset
  //	mov cx,CMAPSegment
  //	mov es,cx
  //	mov cx,FARBENPROLEVEL
  //	int 10h
  // }

  /* Farbregisterwerte an einen absteigenden Hoechstwert anpassen */
  //for(i=64;i>0;i--)
  //{
  //	for(ii=0;ii<(256*3);ii++)
  //	{
  //		if (CMAPBuffer[ii]>i) CMAPBuffer[ii]--;
  //	}
  
  /* Farbregisterwerte in die DAC-Register eintragen */
  //	asm{
  //		mov ax,1012h
  //		mov bx,1
  //		mov cx,FARBENPROLEVEL
  //		mov dx,CMAPSegment
  //		mov es,dx
  //		mov dx,CMAPOffset
  //		int 10h
  //	}
  //}
  //free(CMAPBuffer);
} // void FadeLevel(void)

void FadeColors1(void)
{
/*
	Diese Prozedur blendet das momentan angezeigte Bild aus. Dabei werden die
   Farbregister des DAC-Converters an einen absteigenden Hoechstwert angepasst.

   Parameter: keine
   Returnwert: keiner
*/

/* lokale Variablen der Funktion */
  // char* CMAPBuffer;
  // int CMAPOffset=0;
  // int CMAPSegment=0;
  // int i;
  // int ii;

/* Speicherplatz fuer die Farbregister reservieren  */
//	CMAPBuffer=MyMalloc(257*3);		// wird wieder freigegeben 
//	CMAPOffset=FP_OFF(CMAPBuffer);
//	CMAPSegment=FP_SEG(CMAPBuffer);

/* Farbregisterwerte aus den DAC-Registern auslesen */
//	asm{
//		mov ax,1017h
//		mov bx,0
//		mov dx,CMAPOffset
//		mov cx,CMAPSegment
//		mov es,cx
//		mov cx,256
//		int 10h
//	}

/* Farbregisterwerte an einen absteigenden Hoechstwert anpassen */
//	for(i=64;i>0;i--)
//	{
//		for(ii=0;ii<(256*3);ii++)
//		{
//			if (CMAPBuffer[ii]>i) CMAPBuffer[ii]--;
//		}

/* Farbregisterwerte in die DAC-Register eintragen */
//		asm{
//			mov ax,1012h
//			mov bx,0
//			mov cx,256
//			mov dx,CMAPSegment
//			mov es,dx
//			mov dx,CMAPOffset
//			int 10h
//		}
//	}
//	free(CMAPBuffer);
}


/* *********************************************************************** */

void FadeColors2(void)
{
/*
	Diese Prozedur blendet das momentan angezeigte Bild aus. Dabei werden die
	DAC-Farbregister der Grafikkarte immer dekrementiert, solange sie nicht
	mit null identisch sind.

	Parameter: keine
	Returnwert: keiner
*/

/* lokale Variablen der Funktion */
  // char* CMAPBuffer;
  // int CMAPOffset=0;
  // int CMAPSegment=0;
  // int i;
  // int ii;

/* Speicherplatz fuer die Farbregister reservieren  */
//	CMAPBuffer=MyMalloc(257*3);			// wird wieder freigegeben
//	CMAPOffset=FP_OFF(CMAPBuffer);
//	CMAPSegment=FP_SEG(CMAPBuffer);

/* Farbregisterwerte aus den DAC-Registern auslesen */
//	asm{
//		mov ax,1017h
//		mov bx,0
//		mov dx,CMAPOffset
//		mov cx,CMAPSegment
//		mov es,cx
//		mov cx,256
//		int 10h
//	}

/* Farbregisterwerte an einen absteigenden Hoechstwert anpassen */
//	for(i=64;i>0;i--)
//	{
//		for(ii=0;ii<(256*3);ii++)
//		{
//			if (CMAPBuffer[ii]>0) CMAPBuffer[ii]--;
//		}

/* Farbregisterwerte in die DAC-Register eintragen */
//		asm{
//			mov ax,1012h
//			mov bx,0
//			mov cx,256
//			mov dx,CMAPSegment
//			mov es,dx
//			mov dx,CMAPOffset
//			int 10h
//		}
//	}

/* Reservierten Speicher wieder freigeben */
//	free(CMAPBuffer);
}


/* *********************************************************************** */

void WaitVRetrace(void)
{
/*
	Diese Prozedur wartet darauf, daá der Elektronenstrahl des Monitors den
   naechsten vertikalen Strahlruecklauf antritt. Sollte der Elektronenstrahl
   zu Beginn der Funktion gerade mit einem Ruecklauf beschaeftigt sein, so
   wird die naechste Ruecklaufperiode abgewartet.

   Parameter: keine
*/

  //	while ((inport(*CRTC+6) & 0x08) != 0);
  //	while ((inport(*CRTC+6) & 0x08) <= 0);
}


/* *********************************************************************** */

void LadeZeichensatz(char* Zeichensatzname)
{
/*
  Diese Prozedur laedt einen Zeichensatz in den Standardzeichensatzbereich
  und verwendet dazu das BIOS.

  Parameter sind ein Zeiger auf den Zeichensatznamen
  Returnwert: keiner
*/

  /* lokale Variablen der Funktion */
  unsigned char* Zeichensatzpointer;
  FILE* CharDateiHandle;
  int i,j,k;


  /* Speicher fuer die zu ladende Datei reservieren */
  Zeichensatzpointer=MyMalloc(256*8+10);

  /* Datei in den Speicher laden */
  if ((CharDateiHandle=fopen(Zeichensatzname,"rb")) == NULL) {
    printf("\nvoid LadeZeichensatz(char* Zeichensatzname):  Konnte die Datei %s nicht oeffnen !\n",Zeichensatzname);
    getchar();
    Terminate(-1);
  }
  fread(Zeichensatzpointer,1,30000,CharDateiHandle);
  if (fclose(CharDateiHandle) == EOF) {
    printf("\nvoid LadeZeichensatz(char* Zeichensatzname): Konnte die Datei %s nicht schlie3en !\n",Zeichensatzname);
    getchar();
    Terminate(-1);
  }

  //
  // CRAP DISABLED FOR THE PORT!
  //

  /*	Zeichensatz aktivieren */
  //	SatzSegment=FP_SEG(Zeichensatzpointer);
  //  SatzOffset=FP_OFF(Zeichensatzpointer);
  //  asm{
  //  	push es
  //     push bp
  //  	mov ax,SatzSegment
  //     mov es,ax
  //  	mov ah,11h
  //     mov al,21h
  //     mov bl,2
  //     mov cx,8
  //     mov bp,SatzOffset
  //     int 10h
  //     pop bp
  //     pop es
  //  }

  /* Eventuell Report erstatten das der Zeichensatz installiert ist */
#ifdef REPORTDEBUG
  printf("\nvoid LadeZeichensatz(char* Zeichensatzname): Der Zeichensatz ist installiert ! ");
#endif

  if (Data70Pointer) {
    printf(" Der Zeichensatz war schon installiert !.\n");
    getchar();
    Terminate(-1);
  }
  Data70Pointer=malloc(256*8*8+10);
  for(i=0;i<256;i++) {
    for(j=0;j<8;j++){
      for(k=0;k<8;k++) {
	if ( ((int)Zeichensatzpointer[i*8+j]) & (1 << (7-k)) )
	  Data70Pointer[i*8*8+j*8+k]=DATA70FONTCOLOR;
	else Data70Pointer[i*8*8+j*8+k]=DATA70BGCOLOR;
      }
    }
  }
}  // void LadeZeichensatz(char* Zeichensatzname)


/* ********************************************************************** */

void RotateColors(register int ColNum1,register int ColNum2)
{
  /*
  Diese Prozedur rotiert einen fest vorgegebenen Bereich der DAC-Register
  um eins aufsteigend. Ein Zwischenspeicher wird reserviert und wieder
  freigegeben.
  
  Parameter : die Werte des ersten und lesten Registers: ColNum1 und ColNum2
  */

  //	static unsigned char *FarbFeldPointer = NULL;
  //	register unsigned char Zwisch[3];
  
  //	register int AnzColors = ColNum2 - ColNum1 +1;
  // int ColOfs;
  // int ColSeg;
  // int i;

  //	if( FarbFeldPointer == NULL ) FarbFeldPointer = MyMalloc(1000);
  
  //	ColOfs=FP_OFF(FarbFeldPointer);
  //	ColSeg=FP_SEG(FarbFeldPointer);
  
  /* Farben aus den Registern in den Speicher uebertragen */
  //	asm{
  //	push es
  //	mov ax,1017h
  //	mov bx,ColNum1
  //	mov cx,AnzColors
  //	mov dx,ColSeg
  //	mov es,dx
  //	mov dx,ColOfs
  //	int 10h
  //	pop es
  //}
  
  /* Farben im Speicher um eins rotieren */
  //memcpy(Zwisch,FarbFeldPointer, 3);
	
  //memcpy(FarbFeldPointer,FarbFeldPointer+3, (AnzColors-1)*3 );
  
  //memcpy(FarbFeldPointer+(AnzColors-1)*3, Zwisch, 3);
  
  /* Farben aus dem Speicher zurueck in die DAC-Register schreiben */
  //asm{
  //		push es
  //	mov ax,1012h
  //	mov bx,ColNum1
  //	mov cx,AnzColors
  //	mov dx,ColSeg
  //	mov es,dx
  //	mov dx,ColOfs
  //	int 10h
  //	pop es
  //}
  
}  // void RotateColors(...)

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void LevelGrauFaerben(void){ SetLevelColor(PD_DARK); }


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void ClearGraphMem(unsigned char* screen){
  printf("\nvoid ClearGraphMem(unsigned char* screen): Real function called.");

  memset(screen,SCREENBREITE*SCREENHOEHE,0);

  printf("\nvoid ClearGraphMem(unsigned char* screen): Usual end of function reached.");
} // void ClearGraphMem(unsigned char* screen)


/*@Function============================================================
@Desc: This function sets a selected colorregister to a specified
		RGB value

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void SetPalCol(unsigned int palpos, unsigned char rot, unsigned char gruen, unsigned char blau ){
  // printf("\nvoid SetPalCol(...): Real function called.");
  vga_setpalette(palpos,rot,gruen,blau);
  // printf("\nvoid SetPalCol(...): Usual end of function reached.");
} // void SetPalCol(...)

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void SetPalCol2(unsigned int palpos, color Farbwert)
{
  vga_setpalette(palpos,Farbwert.rot,Farbwert.gruen,Farbwert.blau);
} // void SetPalCol2(...)

		
/* **********************************************************************
   Diese Funktion bringt ein Flimmer auf den Schirm
**********************************************************************/


#define BLANKBREITE 20
#define FLIMMERN4

void Flimmern(void){
  int i;
  unsigned char* Screenptr;
  unsigned char* Junkptr;
  
  printf("\nvoid Flimmern(void): Real function call confirmed.");

  Screenptr=RealScreen;

#ifdef FLIMMERN1
  Screenptr+=USERFENSTERPOSY*SCREENBREITE+USERFENSTERPOSX;
  randomize();
  for (i=0;i<USERFENSTERHOEHE;i++){
    for (j=0;j<USERFENSTERBREITE;j++){
      *Screenptr=*Junkptr;
      Screenptr++;
      Junkptr++;
    }
    Screenptr+=SCREENBREITE-USERFENSTERBREITE;
  }
  return;
#endif

#ifdef FLIMMERN2
  for(i=0;i<BLANKBREITE;i++){
    memset(Screenptr+(i+USERFENSTERPOSY)*SCREENBREITE+USERFENSTERPOSX,0,USERFENSTERBREITE);
    memset(Screenptr+(USERFENSTERPOSY-i+USERFENSTERHOEHE)*SCREENBREITE+USERFENSTERPOSX,0,USERFENSTERBREITE);
    for(j=0;j<USERFENSTERHOEHE;j++) {
      *(Screenptr+(USERFENSTERPOSY+j)*SCREENBREITE+USERFENSTERPOSX+i)=0;
      *(Screenptr+(USERFENSTERPOSY+j)*SCREENBREITE+USERFENSTERPOSX+USERFENSTERBREITE-i)=0;
    }
  }
  return;
#endif
	
#ifdef FLIMMERN3
  /* vertical close Userfenster */
  for(i=0; i<USERFENSTERHOEHE/(2); i++) {
    memset(Screenptr+(i+USERFENSTERPOSY)*SCREENBREITE+USERFENSTERPOSX,
	   0,USERFENSTERBREITE);
    memset(Screenptr+(USERFENSTERPOSY+USERFENSTERHOEHE-i)*SCREENBREITE
	   +USERFENSTERPOSX,
	   0, USERFENSTERBREITE);
    usleep(200);
  }
  
  /* make the central line white */
  Junkptr = Screenptr +
    (USERFENSTERPOSY+USERFENSTERHOEHE/2)*SCREENBREITE+USERFENSTERPOSX;
  memset( Junkptr, FONT_WHITE, USERFENSTERBREITE);
  
  usleep(50000);


  /* horizontal close userfenster */
  
  for(i=0; i<USERFENSTERBREITE/2-2; i++) {
    *(Junkptr + i) = 0;
    *(Junkptr + USERFENSTERBREITE - i) = 0;
    usleep(100);
  }
  
  usleep(30000);
  /* Clear the rest */
  memset(Junkptr, 0, USERFENSTERBREITE);
  
  return;
#endif

#ifdef FLIMMERN4
  /* vertical close Userfenster */
  for(i=0; i<(USERFENSTERHOEHE/2); i++) {
    memset(Screenptr+(i+USERFENSTERPOSY)*SCREENBREITE+USERFENSTERPOSX,
	   0,USERFENSTERBREITE);
    memset(Screenptr+(USERFENSTERPOSY+USERFENSTERHOEHE-i)*SCREENBREITE
	   +USERFENSTERPOSX,
	   0, USERFENSTERBREITE);
    usleep(200);
  }
  
	
  /* make the central line white */
  
  // This is off in Flimmern4
  Junkptr = Screenptr +
    (USERFENSTERPOSY+USERFENSTERHOEHE/2)*SCREENBREITE+USERFENSTERPOSX;
  /*	memset( Junkptr, FONT_WHITE, USERFENSTERBREITE);
   */
  usleep(50000);
  
  /* horizontal close userfenster */
  
  for(i=0; i<USERFENSTERBREITE/2-2; i++) {
    *(Junkptr + i) = 0;
    *(Junkptr + USERFENSTERBREITE - i) = 0;
    usleep(100);
  }
  
  usleep(30000);
  /* Clear the rest */
  memset(Junkptr, 0, USERFENSTERBREITE);
  
  return;
#endif

#ifndef FLIMMERN4
#ifndef FLIMMERN3
#ifndef FLIMMERN2
#ifndef FLIMMERN1
  /* Wenn "uberhaupt keine der angebotenen Varianten genommen wurde */
  printf(" Warning: No Flimmern at all !\n");
  getchar();
#endif
#endif
#endif
#endif
}
#undef _graphics_c








