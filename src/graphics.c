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
 * Desc: Graphics primitived, such as functions to load LBM or PCX images,
 *	to change the vga color table, to activate or deachtivate monitor
 *	signal, to set video modes etc.
 *
 *----------------------------------------------------------------------*/
#include <config.h>

#define _graphics_c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <vga.h>
// #include <vgagl.h>
#include <vgakeyboard.h>

#include "SDL.h"
// #include "SDL_mixer.h"
#include "SDL_image.h"


#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"
#include "map.h"
#include "paratext.h"
#include "colodefs.h"

extern int TimerFlag;


void 
MakeGridOnScreen(void){
  int x,y;

  // vga_setcolor(0);

  Lock_SDL_Screen();

  for (y=0; y<SCREENHOEHE; y++) 
    {
      for (x=0; x<SCREENBREITE; x++) 
	{
	  if ((x+y)%2 == 0) 
	    {
	      putpixel(screen, x, y, 0);
	    }
	}
    }

  Unlock_SDL_Screen();

} // void MakeGridOnSchreen(void)

unsigned char *MemSearch (unsigned char *, unsigned char *, unsigned char *);

SDL_Surface *LoadImage(char *datafile, int transparent)
{
  SDL_Surface *image, *surface;
  
  image = IMG_Load(datafile);
  if ( image == NULL ) {
    fprintf(stderr, "Couldn't load image %s: %s\n",
	    datafile, IMG_GetError());
    return(NULL);
  }
  if ( transparent ) {
    /* Assuming 8-bit BMP image */
    SDL_SetColorKey(image, (SDL_SRCCOLORKEY|SDL_RLEACCEL),
		    *(Uint8 *)image->pixels);
  }
  surface = SDL_DisplayFormat(image);
  SDL_FreeSurface(image);
  return(surface);
}

void display_bmp(char *file_name)
{
  SDL_Surface *image;

  DebugPrintf
    ("\nvoid display_bmp(char *file_name):  Real function call confirmed...");

  /* Load the BMP file into a surface */
  image = SDL_LoadBMP(file_name);
  if (image == NULL) {
    fprintf(stderr, "Couldn't load %s: %s\n", file_name, SDL_GetError());
    return;
  }
  
  /*
   * Palettized screen modes will have a default palette (a standard
   * 8*8*4 colour cube), but if the image is palettized as well we can
   * use that palette for a nicer colour matching
   */
  if (image->format->palette && screen->format->palette) 
    {
      SDL_SetColors( screen , image->format->palette->colors, 0,
		     image->format->palette->ncolors);
      SDL_SetColors( ScaledSurface , image->format->palette->colors, 0,
		     image->format->palette->ncolors);
      /*
	printf("\n\n\nFarbpalette wurde erkannt...\n\n\nTerminiere...\n\n\n");
	Terminate(0);
      */
    }
  
  if ( SDL_SetColorKey(image, SDL_SRCCOLORKEY, 252) == (-1) )
    {
      printf("\n\nvoid display_bmp(char* file_name): ERROR in SDL_SetColorKey.\n\nTerminating...\n\n");
      Terminate(ERR);
    }
  
  /* Blit onto the screen surface */
  if(SDL_BlitSurface(image, NULL, screen, NULL) < 0)
    fprintf(stderr, "BlitSurface error: %s\n", SDL_GetError());
  
  SDL_UpdateRect(screen, 0, 0, image->w, image->h);
  
  /* Free the allocated BMP surface */
  SDL_FreeSurface(image);

  DebugPrintf
    ("\nvoid display_bmp(char *file_name):  end of function reached...");
} 


void
Load_PCX_Image (char *PCX_Filename, unsigned char *Parameter_Screen, int LoadPal)
{
  FILE *file;
  void *image;
  int i;
  int j;
  unsigned short int length, height;
  unsigned char palette[768];
  SDL_Surface *LocalImage;

  DebugPrintf
    ("\nvoid Load_PCX_Image(...):  Real function call confirmed...");

  display_bmp(PCX_Filename);

  SDL_UpdateRect(screen, 0, 0, SCREENBREITE, SCREENHOEHE);

  Lock_SDL_Screen();

  for (i=0;i<SCREENHOEHE;i++)
    for (j=0;j<SCREENBREITE;j++)
      {
	*(Parameter_Screen+i*SCREENBREITE+j)=getpixel(screen, j, i);
      }

  Unlock_SDL_Screen();

  DebugPrintf ("\nvoid Load_PCX_Image(...):  end of function reached.");

} // void Load_PCX_Image(char* PCX_Filename,unsigned char* Screen,int LoadPal)

/*-----------------------------------------------------------------
 * @Desc: get the pics for: druids, bullets, blasts
 * 				
 * 	reads all blocks and puts the right pointers into
 * 	the various structs
 *
 * @Ret: FALSE: ERROR  	TRUE: OK
 *
 *-----------------------------------------------------------------*/
int
InitPictures (void)
{
  int i;
  char *DruidFilename;

  /* First read the map blocks */
  GetMapBlocks ();

  /* Get the enemy-blocks */
  GetBlocks (ENEMYBILD_PCX, 0, 0);
  Enemypointer = GetBlocks (NULL, 0, ENEMYPHASES);

  /* Get the influence-blocks */
  GetBlocks (INFLUENCEBILD_PCX, 0, 0);
  Influencepointer = GetBlocks (NULL, 0, ENEMYPHASES);

  /* the same game for the bullets */
  GetBlocks (BULLETBILD_PCX, 0, 0);
  for (i = 0; i < ALLBULLETTYPES; i++)
    {
      Bulletmap[i].picpointer = GetBlocks (NULL, i, Bulletmap[i].phases);
    }

  /* ...and the blasts */
  GetBlocks (BLASTBILD_PCX, 0, 0);
  for (i = 0; i < ALLBLASTTYPES; i++)
    {
      Blastmap[i].picpointer = GetBlocks (NULL, i, Blastmap[i].phases);
    }

  /* Get the Frame */
  Load_PCX_Image (RAHMENBILD1_PCX, InternalScreen, FALSE);
  RahmenPicture =
    (unsigned char *) MyMalloc (RAHMENBREITE * RAHMENHOEHE + 10);
  IsolateBlock (InternalScreen, RahmenPicture, 0, 0, RAHMENBREITE,
		RAHMENHOEHE);

  /* get the Elevator-Blocks */
  ElevatorBlocks =
    (unsigned char *) MyMalloc (NUM_EL_BLOCKS * EL_BLOCK_MEM + 100);
  Load_PCX_Image (EL_BLOCKS_FILE_PCX, InternalScreen, FALSE);
  for (i = 0; i < NUM_EL_BLOCKS; i++)
    IsolateBlock (InternalScreen,
		  ElevatorBlocks + i * EL_BLOCK_MEM,
		  1 + i * (EL_BLOCK_LEN + 1), 1,
		  EL_BLOCK_LEN, EL_BLOCK_HEIGHT);


  /* get Menublocks for the In-game Consoles, not the Options menu! */
  Load_PCX_Image ( CONSOLENBILD_PCX, InternalScreen, FALSE);
  MenuItemPointer = MyMalloc (MENUITEMMEM);
  IsolateBlock (InternalScreen, MenuItemPointer, 0, 0, MENUITEMLENGTH,
		MENUITEMHEIGHT);

  /* get Menublocks for the In-game Consoles, not the Options menu! */
  Load_PCX_Image ( MENU_PICTURE_PCX_FILENAME , InternalScreen, FALSE);
  OptionsMenuPointer = MyMalloc ( SCREENBREITE * SCREENHOEHE +10 );
  IsolateBlock (InternalScreen, OptionsMenuPointer, 0, 0, SCREENBREITE ,
		SCREENHOEHE );

  /* get robotpictures */
  DruidFilename = malloc (1000);
  for (i = 0; i < ALLDRUIDTYPES; i++)
    {
      DruidFilename[0] = 0;
      DruidFilename = strcat (DruidFilename, "../graphics/");
      DruidFilename = strcat (DruidFilename, Druidmap[i].druidname);
      DruidFilename = strcat (DruidFilename, ".bmp");
      DebugPrintf ("\nint InitPictures(void): Loading Druidpicture: ");
      DebugPrintf (DruidFilename);
      Load_PCX_Image (DruidFilename, InternalScreen, FALSE);
      Druidmap[i].image = malloc (DRUIDIMAGE_LENGTH * DRUIDIMAGE_HEIGHT + 1);
      IsolateBlock (InternalScreen, Druidmap[i].image, 0, 0,
		    DRUIDIMAGE_LENGTH, DRUIDIMAGE_HEIGHT);
    }
  free (DruidFilename);
  return TRUE;
} // int InitPictures(void)

/*-----------------------------------------------------------------
 * @Desc: doesnt really _swap_ anything, but copies InternalScreen
 *        onto the Real vga screen
 *
 *-----------------------------------------------------------------*/
void
SwapScreen (void)
{
  int x;
  int y;

  Lock_SDL_Screen();

  for (y = 0; y < SCREENHOEHE; y++)
    {
      for (x = 0; x< SCREENBREITE; x++)
	{
	  putpixel ( screen, x, y, *(InternalScreen + SCREENBREITE * y + x) );
	}
    }

  Unlock_SDL_Screen();

  Update_SDL_Screen();

} /* SwapScreen() */

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
CopyScreenToInternalScreen(void)
{
  int y, x;

  Lock_SDL_Screen();

  for (y = 0; y < SCREENHOEHE; y++)
    {
      for (x=0; x<SCREENBREITE; x++) 
	InternalScreen[y*SCREENBREITE+x]=getpixel(screen, x,y);
    }

  Unlock_SDL_Screen();

} // void CopyScreenToInternalScreen(void)

/*-----------------------------------------------------------------
 * @Desc: 
 * @Ret: 
 *
 *-----------------------------------------------------------------*/
void
ClearVGAScreen (void)
{
  char *LocalBlackLinePointer;
  int y;
  int x;

  LocalBlackLinePointer = malloc (SCREENBREITE + 10);
  memset (LocalBlackLinePointer, 0, SCREENBREITE);

  Lock_SDL_Screen();

  for (y = 0; y < SCREENHOEHE; y++)
    {
      for (x=0; x<SCREENBREITE; x++) 
	{
	  putpixel ( screen , x , y , 0 );
	}
    }

  Unlock_SDL_Screen();

  free (LocalBlackLinePointer);
}				// void ClearVGAScreen(void)

unsigned char *MemSearch (unsigned char *SStart,
			  unsigned char *SEnd, unsigned char *SString);

void PlusDrawEnergyBar (void);

/*@Function============================================================
@Desc: MemSearch(): Sucht Binary-Area zwischen SStart und SEnd nach
SString ab. liefer Pointer auf gef. String oder NULL

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
unsigned char *
MemSearch (unsigned char *SStart, unsigned char *SEnd, unsigned char *SString)
{
  register unsigned char *tmp;	// bewegl. Zeiger auf Search-Area 
  register unsigned char *string;	// bewegl. Zeiger auf ges. STring 
  unsigned char *firstchar;	// Zeiger auf Beginn der Uebereinstimmung 
  tmp = SStart;

  while (1)
    {
      string = SString;

      // Erste "Ubereinstimmung finden
      while ((*tmp != *string) && (tmp < SEnd))
	tmp++;
      if (tmp >= SEnd)
	return NULL;		// Suchbereich schon verlassen

      firstchar = tmp;		// Beginn der Uebereinst. merken

      // Rest vergleichen
      string++;
      tmp++;
      while (*string && (tmp < SEnd) && (*string == *tmp))
	{
	  string++;
	  tmp++;
	}

      // Falls Ende des String erreicht ---> gefunden !!!
      if (*string == '\0')
	return firstchar;

      // Falls Ende des Suchbereichs erreicht --> nix gefunden !!
      if (tmp >= SEnd)
	return NULL;

      // sonst: weitersuchen
      tmp = firstchar + 1;	// nach voriger Uebereinst. weiter
      continue;
    }				// while

}				/* MemSearch */

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
SetColors (int FirstCol, int PalAnz, char *PalPtr)
{
  char *MyPalPtr;
  int i;

  MyPalPtr = PalPtr;

  for (i = FirstCol; i < FirstCol + PalAnz; i++)
    {
      SetPalCol(i, MyPalPtr[0], MyPalPtr[1], MyPalPtr[2]);
      MyPalPtr += 3;
    }
}				// void SetColors(...)

/*@Function============================================================
@Desc: Monitorsignalunterbrechung(int): steuert das Monitorsignal

@Ret:
@Int:
* $Function----------------------------------------------------------*/
void
Monitorsignalunterbrechung (int Signal)
{

}

/*@Function============================================================
@Desc: InitPalette(): laedt die allgemein gueltige Palette fuer die
						LBM- Bilder 

@Ret: OK | ERR
@Int:
* $Function----------------------------------------------------------*/
int
InitPalette (void)
{
  /* Hier sollte die Palette geladen werden */
  Load_PCX_Image (PALBILD_PCX, InternalScreen, TRUE);
  return OK;
}				// int InitPalette(void)


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
int
InitLevelColorTable (void)
{
  FILE *ColorFile;
  int i;

  LevelColorArray = MyMalloc (ALLLEVELCOLORS * 3 * FARBENPROLEVEL + 10);	// NICHT FREIGEBEN
  if ((ColorFile = fopen (COLORFILE, "r")) == NULL)
    return (FALSE);
  for (i = 0; i < ALLLEVELCOLORS * FARBENPROLEVEL; i++)
    {
      if (fscanf (ColorFile, "%hhd %hhd %hhd", &LevelColorArray[i * 3],
		  &LevelColorArray[3 * i + 1],
		  &LevelColorArray[3 * i + 2]) == EOF)
	return (FALSE);
    }
  fclose (ColorFile);
  return (TRUE);
}				// int InitLevelColorTable(void)

/*@Function============================================================
@Desc: 

 @Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
SetLevelColor (int ColorEntry)
{

  SetColors (FIRSTBLOCKCOLOR, FARBENPROLEVEL,
	     LevelColorArray + ColorEntry * 3 * FARBENPROLEVEL);

}				// void SetLevelColor(int ColorEntry)

/* *********************************************************************** */

void
Set_SVGALIB_Video_ON (void)
{
  int vgamode;

  DebugPrintf
    ("\n\n    Die SVGALIB wird nun initialisiert.... gleich gehts los.... \n\n");
  //  getchar();
  vga_init ();
  vgamode = vga_getdefaultmode ();
  // SDL if ((vgamode == -1) || (vga_getmodeinfo (vgamode)->bytesperpixel != 1))
  // SDL vgamode = G320x200x256;

  if (!vga_hasmode (vgamode))
    {
      DebugPrintf ("Mode not available.\n");
      exit (1);
    }

  vga_setmode (vgamode);
  gl_setcontextvga (vgamode);
  gl_enableclipping ();
  // Initiate fonts of the svgalib (gl-part)!
  // SDL gl_setfont (8, 8, gl_font8x8);
  // SDL gl_setwritemode (FONT_COMPRESSED + WRITEMODE_OVERWRITE);
  gl_setfontcolors (0, vga_white ());
  // Initiate raw keyboard access...
  DebugPrintf
    ("\n\n    Die Tastatur wird nun fuer die svgalib initialisiert.... gleich gehts los!\n");

  Init_SDL_Keyboard();
  /*
  if (keyboard_init ())
    {
      DebugPrintf
	("FEHLER! FEHLER! Keyboard konnte nicht initialisiert werden!!!!!");
      Terminate (ERR);
    }
  */

  // Translate to 4 keypad cursor keys, and unify enter key. 
  keyboard_translatekeys (TRANSLATE_CURSORKEYS | TRANSLATE_KEYPADENTER |
			  TRANSLATE_DIAGONAL);
  /* (TRANSLATE_DIAGONAL seems to give problems.) Michael: No doesn't...
     but might not do what you expect.. */

}				// void Set_SVGALIB_Video_ON(void)

void
Set_SVGALIB_Video_OFF (void)
{

  DebugPrintf
    ("\nvoid Set_SVGALIB_Video_OFF(void): shutting svga-keyboard back to normal.....\n");
  keyboard_close ();

  DebugPrintf
    ("\nvoid Set_SVGALIB_Video_OFF(void): shutting console back to text mode....\n");
  vga_setmode (TEXT);

}				// Set_SVGALIB_Video_OFF(void)

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
UnfadeLevel (void)
{
  char *CMAPBuffer;
  int i, j, Color;

  Color = CurLevel->color;
  if (CurLevel->empty)
    Color = PD_DARK;

  GetView ();
  GetInternFenster (SHOW_ALL);
  PutInternFenster ();

  /* Speicher reservieren */
  CMAPBuffer = MyMalloc (257 * 3);	// wird wieder freigegeben
  memset (CMAPBuffer, 0, 256 * 3);

  for (j = 0; j < 64; j++)
    {
      for (i = 3; i < ((1 + FARBENPROLEVEL) * 3 + 1); i++)
	{
	  if (CMAPBuffer[i] <
	      LevelColorArray[(i - 3) + (Color * 3 * FARBENPROLEVEL)])
	    CMAPBuffer[i]++;
	}
      //SetColors (1, FARBENPROLEVEL, CMAPBuffer + 3);
    }
  SetColors (1, FARBENPROLEVEL, CMAPBuffer + 3);
  free (CMAPBuffer);
}				// void UnfadeLevel(void)

/*@Function============================================================
@Desc: Wie FadeColors1 nur auf die FARBENPROLEVEL Farben beschr"ankt.

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
FadeLevel (void)
{
  // char* CMAPBuffer;
  // int CMAPOffset,CMAPSegment,i,ii;

  // 
  // THIS CRAP WAS IN BEFORE THE PORT!!!!!!!!!!
  //
  /* Speicherplatz fuer die Farbregister reservieren  */
  // CMAPBuffer=MyMalloc(257*3);                // wird wieder freigegeben 
  // CMAPOffset=FP_OFF(CMAPBuffer);
  // CMAPSegment=FP_SEG(CMAPBuffer);

  /* Farbregisterwerte aus den DAC-Registern auslesen */
  //    asm{
  //    mov ax,1017h
  //    mov bx,1
  //    mov dx,CMAPOffset
  //    mov cx,CMAPSegment
  //    mov es,cx
  //    mov cx,FARBENPROLEVEL
  //    int 10h
  // }

  /* Farbregisterwerte an einen absteigenden Hoechstwert anpassen */
  //for(i=64;i>0;i--)
  //{
  //    for(ii=0;ii<(256*3);ii++)
  //    {
  //            if (CMAPBuffer[ii]>i) CMAPBuffer[ii]--;
  //    }

  /* Farbregisterwerte in die DAC-Register eintragen */
  //    asm{
  //            mov ax,1012h
  //            mov bx,1
  //            mov cx,FARBENPROLEVEL
  //            mov dx,CMAPSegment
  //            mov es,dx
  //            mov dx,CMAPOffset
  //            int 10h
  //    }
  //}
  //free(CMAPBuffer);
}				// void FadeLevel(void)

void
FadeColors1 (void)
{
/*
  Diese Prozedur blendet das momentan angezeigte Bild aus. Dabei werden die
  Farbregister des DAC-Converters an einen absteigenden Hoechstwert angepasst.
  
  Parameter: keine
  Returnwert: keiner
*/

/* lokale Variablen der Funktion */
  int *CMAPBuffer;
  int i;
  int ii;

/* Speicherplatz fuer die Farbregister reservieren  */
  CMAPBuffer = MyMalloc (2 * 257 * 3);	// wird wieder freigegeben 

/* Farbregisterwerte auslesen */
  vga_getpalvec (0, 256, CMAPBuffer);

/* Farbregisterwerte an einen absteigenden Hoechstwert anpassen */
  for (i = 64; i > 0; i--)
    {
      for (ii = 0; ii < (256 * 3); ii++)
	{
	  if (CMAPBuffer[ii] > i)
	    CMAPBuffer[ii]--;
	}
      /* Farbregisterwerte in die DAC-Register eintragen */
      vga_setpalvec (0, 256, CMAPBuffer);
    }				// for(i=64,...)


  free (CMAPBuffer);
}				// void FadeColors1(void)


/* *********************************************************************** */

void
FadeColors2 (void)
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
//      CMAPBuffer=MyMalloc(257*3);                     // wird wieder freigegeben
//      CMAPOffset=FP_OFF(CMAPBuffer);
//      CMAPSegment=FP_SEG(CMAPBuffer);

/* Farbregisterwerte aus den DAC-Registern auslesen */
//      asm{
//              mov ax,1017h
//              mov bx,0
//              mov dx,CMAPOffset
//              mov cx,CMAPSegment
//              mov es,cx
//              mov cx,256
//              int 10h
//      }

/* Farbregisterwerte an einen absteigenden Hoechstwert anpassen */
//      for(i=64;i>0;i--)
//      {
//              for(ii=0;ii<(256*3);ii++)
//              {
//                      if (CMAPBuffer[ii]>0) CMAPBuffer[ii]--;
//              }

/* Farbregisterwerte in die DAC-Register eintragen */
//              asm{
//                      mov ax,1012h
//                      mov bx,0
//                      mov cx,256
//                      mov dx,CMAPSegment
//                      mov es,dx
//                      mov dx,CMAPOffset
//                      int 10h
//              }
//      }

/* Reservierten Speicher wieder freigeben */
//      free(CMAPBuffer);
}


/* *********************************************************************** */

void
WaitVRetrace (void)
{
  /*
     Diese Prozedur wartet darauf, daá der Elektronenstrahl des Monitors den
     naechsten vertikalen Strahlruecklauf antritt. Sollte der Elektronenstrahl
     zu Beginn der Funktion gerade mit einem Ruecklauf beschaeftigt sein, so
     wird die naechste Ruecklaufperiode abgewartet.

     Parameter: keine
   */

  vga_waitretrace ();

}				// void WaitVRetrace(void)


/* *********************************************************************** */

void
LadeZeichensatz (char *Zeichensatzname)
{
/*
  Diese Prozedur laedt einen Zeichensatz in den Standardzeichensatzbereich
  und verwendet dazu das BIOS.

  Parameter sind ein Zeiger auf den Zeichensatznamen
  Returnwert: keiner
*/

  /* lokale Variablen der Funktion */
  unsigned char *Zeichensatzpointer;
  FILE *CharDateiHandle;
  int i, j, k;


  /* Speicher fuer die zu ladende Datei reservieren */
  Zeichensatzpointer = MyMalloc (256 * 8 + 10);

  /* Datei in den Speicher laden */
  if ((CharDateiHandle = fopen (Zeichensatzname, "rb")) == NULL)
    {
      printf
	("\nvoid LadeZeichensatz(char* Zeichensatzname):  Konnte die Datei %s nicht oeffnen !\n",
	 Zeichensatzname);
      getchar ();
      Terminate (-1);
    }
  fread (Zeichensatzpointer, 1, 30000, CharDateiHandle);
  if (fclose (CharDateiHandle) == EOF)
    {
      printf
	("\nvoid LadeZeichensatz(char* Zeichensatzname): Konnte die Datei %s nicht schlie3en !\n",
	 Zeichensatzname);
      getchar ();
      Terminate (-1);
    }

  //
  // CRAP DISABLED FOR THE PORT!
  //

  /*    Zeichensatz aktivieren */
  //    SatzSegment=FP_SEG(Zeichensatzpointer);
  //  SatzOffset=FP_OFF(Zeichensatzpointer);
  //  asm{
  //    push es
  //     push bp
  //    mov ax,SatzSegment
  //     mov es,ax
  //    mov ah,11h
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
  DebugPrintf
    ("\nvoid LadeZeichensatz(char* Zeichensatzname): Der Zeichensatz ist installiert ! ");
#endif

  if (Data70Pointer)
    {
      DebugPrintf (" Der Zeichensatz war schon installiert !.\n");
      getchar ();
      Terminate (-1);
    }
  Data70Pointer = malloc (256 * 8 * 8 + 10);
  for (i = 0; i < 256; i++)
    {
      for (j = 0; j < 8; j++)
	{
	  for (k = 0; k < 8; k++)
	    {
	      if (((int) Zeichensatzpointer[i * 8 + j]) & (1 << (7 - k)))
		Data70Pointer[i * 8 * 8 + j * 8 + k] = DATA70FONTCOLOR;
	      else
		Data70Pointer[i * 8 * 8 + j * 8 + k] = DATA70BGCOLOR;
	    }
	}
    }
}				// void LadeZeichensatz(char* Zeichensatzname)


/* ********************************************************************** */

void
RotateColors (register int ColNum1, register int ColNum2)
{
  /*
     Diese Prozedur rotiert einen fest vorgegebenen Bereich der DAC-Register
     um eins aufsteigend. Ein Zwischenspeicher wird reserviert und wieder
     freigegeben.

     Parameter : die Werte des ersten und lesten Registers: ColNum1 und ColNum2
   */

  //    static unsigned char *FarbFeldPointer = NULL;
  //    register unsigned char Zwisch[3];

  //    register int AnzColors = ColNum2 - ColNum1 +1;
  // int ColOfs;
  // int ColSeg;
  // int i;

  //    if( FarbFeldPointer == NULL ) FarbFeldPointer = MyMalloc(1000);

  //    ColOfs=FP_OFF(FarbFeldPointer);
  //    ColSeg=FP_SEG(FarbFeldPointer);

  /* Farben aus den Registern in den Speicher uebertragen */
  //    asm{
  //    push es
  //    mov ax,1017h
  //    mov bx,ColNum1
  //    mov cx,AnzColors
  //    mov dx,ColSeg
  //    mov es,dx
  //    mov dx,ColOfs
  //    int 10h
  //    pop es
  //}

  /* Farben im Speicher um eins rotieren */
  //memcpy(Zwisch,FarbFeldPointer, 3);

  //memcpy(FarbFeldPointer,FarbFeldPointer+3, (AnzColors-1)*3 );

  //memcpy(FarbFeldPointer+(AnzColors-1)*3, Zwisch, 3);

  /* Farben aus dem Speicher zurueck in die DAC-Register schreiben */
  //asm{
  //            push es
  //    mov ax,1012h
  //    mov bx,ColNum1
  //    mov cx,AnzColors
  //    mov dx,ColSeg
  //    mov es,dx
  //    mov dx,ColOfs
  //    int 10h
  //    pop es
  //}

}				// void RotateColors(...)

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
LevelGrauFaerben (void)
{
  SetLevelColor (PD_DARK);
}


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
ClearGraphMem (unsigned char *Parameter_screen)
{
  SDL_Rect ThisRectangle;

  DebugPrintf
    ("\nvoid ClearGraphMem(unsigned char* screen): Real function called.");

  if (screen == RealScreen) 
    {
      ThisRectangle.x=0;
      ThisRectangle.y=0;
      ThisRectangle.w=SCREENBREITE;
      ThisRectangle.h=SCREENHOEHE;
      SDL_FillRect( screen , & ThisRectangle , 0 );
      vga_clear ();
    }
  else
    memset (Parameter_screen, 0, SCREENBREITE * SCREENHOEHE);

  DebugPrintf
    ("\nvoid ClearGraphMem(unsigned char* screen): Usual end of function reached.");

}				// void ClearGraphMem(unsigned char* screen)


/*@Function============================================================
@Desc: This function sets a selected colorregister to a specified
		RGB value

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
SetPalCol (unsigned int palpos, unsigned char rot, unsigned char gruen,
	   unsigned char blau)
{
  SDL_Color ThisOneColor;

  ThisOneColor.r=rot;
  ThisOneColor.g=gruen;
  ThisOneColor.b=blau;
  ThisOneColor.unused=0;

  // DebugPrintf("\nvoid SetPalCol(...): Real function called.");
  // vga_setpalette (palpos, rot, gruen, blau);

  SDL_SetColors( ScaledSurface , &ThisOneColor, palpos, 1 );

  // SDL_SetColors( screen , &ThisOneColor, palpos, 1 );
  // DebugPrintf("\nvoid SetPalCol(...): Usual end of function reached.");
}				// void SetPalCol(...)

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
SetPalCol2 (unsigned int palpos, color Farbwert)
{
  vga_setpalette (palpos, Farbwert.rot, Farbwert.gruen, Farbwert.blau);
}				// void SetPalCol2(...)


/* **********************************************************************
   Diese Funktion bringt ein Flimmer auf den Schirm
**********************************************************************/


#define BLANKBREITE 20
#define FLIMMERN4

void
Flimmern (void)
{
  int i;
  unsigned char *Screenptr;
  SDL_Rect LocalRectangle;

  //  unsigned char* Junkptr;

  DebugPrintf ("\nvoid Flimmern(void): Real function call confirmed.");

  Screenptr = RealScreen;

#ifdef FLIMMERN1
  Screenptr += USERFENSTERPOSY * SCREENBREITE + USERFENSTERPOSX;
  randomize ();
  for (i = 0; i < USERFENSTERHOEHE; i++)
    {
      for (j = 0; j < USERFENSTERBREITE; j++)
	{
	  *Screenptr = *Junkptr;
	  Screenptr++;
	  Junkptr++;
	}
      Screenptr += SCREENBREITE - USERFENSTERBREITE;
    }
  return;
#endif

#ifdef FLIMMERN2
  for (i = 0; i < BLANKBREITE; i++)
    {
      memset (Screenptr + (i + USERFENSTERPOSY) * SCREENBREITE +
	      USERFENSTERPOSX, 0, USERFENSTERBREITE);
      memset (Screenptr +
	      (USERFENSTERPOSY - i + USERFENSTERHOEHE) * SCREENBREITE +
	      USERFENSTERPOSX, 0, USERFENSTERBREITE);
      for (j = 0; j < USERFENSTERHOEHE; j++)
	{
	  *(Screenptr + (USERFENSTERPOSY + j) * SCREENBREITE +
	    USERFENSTERPOSX + i) = 0;
	  *(Screenptr + (USERFENSTERPOSY + j) * SCREENBREITE +
	    USERFENSTERPOSX + USERFENSTERBREITE - i) = 0;
	}
    }
  return;
#endif

#ifdef FLIMMERN3
  /* vertical close Userfenster */
  for (i = 0; i < USERFENSTERHOEHE / (2); i++)
    {
      memset (Screenptr + (i + USERFENSTERPOSY) * SCREENBREITE +
	      USERFENSTERPOSX, 0, USERFENSTERBREITE);
      memset (Screenptr +
	      (USERFENSTERPOSY + USERFENSTERHOEHE - i) * SCREENBREITE +
	      USERFENSTERPOSX, 0, USERFENSTERBREITE);
      usleep (200);
    }

  /* make the central line white */
  Junkptr = Screenptr +
    (USERFENSTERPOSY + USERFENSTERHOEHE / 2) * SCREENBREITE + USERFENSTERPOSX;
  memset (Junkptr, FONT_WHITE, USERFENSTERBREITE);

  usleep (50000);


  /* horizontal close userfenster */

  for (i = 0; i < USERFENSTERBREITE / 2 - 2; i++)
    {
      *(Junkptr + i) = 0;
      *(Junkptr + USERFENSTERBREITE - i) = 0;
      usleep (100);
    }

  usleep (30000);
  /* Clear the rest */
  memset (Junkptr, 0, USERFENSTERBREITE);

  return;
#endif

#ifdef FLIMMERN4

  LocalRectangle.x=USERFENSTERPOSX;
  LocalRectangle.w=USERFENSTERBREITE;
  LocalRectangle.h=1;

  /* vertical close Userfenster */
  for (i = 0; i < (USERFENSTERHOEHE / 2); i++)
    {
      LocalRectangle.x=USERFENSTERPOSX;
      LocalRectangle.w=USERFENSTERBREITE;
      LocalRectangle.h=1;
      LocalRectangle.y=USERFENSTERPOSY+i;
      SDL_FillRect( screen , &LocalRectangle, 0);
      LocalRectangle.x=USERFENSTERPOSX;
      LocalRectangle.w=USERFENSTERBREITE;
      LocalRectangle.h=1;
      LocalRectangle.y=USERFENSTERPOSY+USERFENSTERHOEHE-i;
      SDL_FillRect( screen , &LocalRectangle, 0);
      // usleep (200);
      PrepareScaledSurface();
    }

  /* make the central line white */
  Lock_SDL_Screen();
  for (i = 0; i < USERFENSTERBREITE / 2 + 1; i++)
    {
      putpixel (screen, USERFENSTERPOSX + i, USERFENSTERPOSY + USERFENSTERHOEHE / 2, 0);
      putpixel (screen, USERFENSTERPOSX + USERFENSTERBREITE - i, USERFENSTERPOSY + USERFENSTERHOEHE / 2, 0);
      // usleep (20);
      PrepareScaledSurface();
    }
  Unlock_SDL_Screen();

  return;
#endif

#ifndef FLIMMERN4
#ifndef FLIMMERN3
#ifndef FLIMMERN2
#ifndef FLIMMERN1
  /* Wenn "uberhaupt keine der angebotenen Varianten genommen wurde */
  DebugPrintf (" Warning: No Flimmern at all !\n");
  getchar ();
#endif
#endif
#endif
#endif

}

#undef _graphics_c
