/*----------------------------------------------------------------------
 *
 * Desc: Graphics primitived, such as functions to load LBM or PCX images,
 *	to change the vga color table, to activate or deachtivate monitor
 *	signal, to set video modes etc.
 *
 *----------------------------------------------------------------------*/

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
#define _graphics_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"
#include "map.h"
#include "text.h"
#include "colodefs.h"

extern int TimerFlag;


void 
MakeGridOnScreen(unsigned char* Parameter_Screen){
  int x,y;

  for (y=0; y<SCREENHOEHE; y++) 
    {
      for (x=0; x<SCREENBREITE; x++) 
	{
	  if ((x+y)%2 == 0) 
	    {
	      Parameter_Screen[x+y*SCREENBREITE]=0;
	    }
	}
    }
  
} // void MakeGridOnSchreen(void)

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
#ifdef NEW_ENGINE
  return;
#else
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

  return;
#endif // !NEW_ENGINE
}  /* display_bmp() */


/*-----------------------------------------------------------------
 * 
 * 
 * 
 *-----------------------------------------------------------------*/
void
Load_PCX_Image (char *PCX_Filename, unsigned char *Parameter_Screen, int LoadPal)
{
#ifdef NEW_ENGINE
  return;
#else
  int i,j;

  display_bmp(PCX_Filename);

  SDL_UpdateRect(screen, 0, 0, SCREENBREITE, SCREENHOEHE);

  Lock_SDL_Screen();

  for (i=0;i<SCREENHOEHE;i++)
    for (j=0;j<SCREENBREITE;j++)
      {
	*(Parameter_Screen+i*SCREENBREITE+j)=getpixel(screen, j, i);
      }

  Unlock_SDL_Screen();


#endif // !NEW_ENGINE

} // void Load_PCX_Image(char* PCX_Filename,unsigned char* Screen,int LoadPal)

/*
 * replace every occurance of color src by dst in Surface surf
 */
void replace_color (SDL_Surface *surf, SDL_Color src, SDL_Color dst)
{
  int i, j;
    
  for (i=0; i < surf->w; i++)
    for (j=0; j < surf->h; i++)
      ; /* ok, I'll do that later ; */

  return;
}


/*-----------------------------------------------------------------
 * @Desc: get the pics for: druids, bullets, blasts
 * 				
 * 	reads all blocks and puts the right pointers into
 * 	the various structs
 *
 * @Ret: TRUE/FALSE
 *
 *-----------------------------------------------------------------*/
#ifdef NEW_ENGINE /* new experimental graphics engine */
int
InitPictures (void)
{
  int i, j;
  SDL_Surface *tmp;
  int block_line = 0;   /* keep track of line in ne_blocks we're writing */

  /* 
     create the internal storage for all our blocks 
  */
  tmp = SDL_CreateRGBSurface(0, NUM_MAP_BLOCKS*BLOCK_WIDTH,
			     12*BLOCK_HEIGHT, ne_bpp, 0, 0, 0, 0);
  if (tmp == NULL)
    {
      printf ("\nCould not create ne_blocks surface: %s\n", SDL_GetError());
      return (FALSE);
    }


  /* 
   * convert this to display format for fast blitting 
   */
  ne_blocks = SDL_DisplayFormat(tmp);  /* the surface is copied !*/
  if (ne_blocks == NULL) 
    {
      printf ("\nSDL_DisplayFormat() has failed: %s\n", SDL_GetError());
      return (FALSE);
    }
  SDL_FreeSurface (tmp); /* and free the old one */

  /* set the transparent color */
  if (SDL_SetColorKey(ne_blocks, SDL_SRCCOLORKEY, ne_transp_key) == -1 )
    {
      fprintf (stderr, "Transp setting by SDL_SetColorKey() failed: %s \n",
	       SDL_GetError());
      return (FALSE);
    }

  /* 
   * and now read in the blocks from various files into ne_blocks
   * and initialise the block-coordinates 
   */

  ne_map_block =
    ne_get_blocks (NE_MAP_BLOCK_FILE, NUM_MAP_BLOCKS, 9, 0, block_line++);

  ne_influ_block =
    ne_get_blocks (NE_DROID_BLOCK_FILE, DROID_PHASES, 0, 0, block_line++);

  ne_droid_block =
    ne_get_blocks (NE_DROID_BLOCK_FILE, DROID_PHASES, 0, 1, block_line++);

  for (i=0; i < ALLBULLETTYPES; i++)
    Bulletmap[i].block =
      ne_get_blocks (NE_BULLET_BLOCK_FILE, Bulletmap[i].phases, 0, i, block_line++);

  for (i=0; i < ALLBLASTTYPES; i++)
    Blastmap[i].block =
      ne_get_blocks (NE_BLAST_BLOCK_FILE, Blastmap[i].phases, 0, i, block_line++);

  ne_digit_block =
    ne_get_digit_blocks (NE_DIGIT_BLOCK_FILE, DIGITNUMBER, DIGITNUMBER, 0, block_line++);

  ne_rahmen_block = ne_get_rahmen_block ( NE_RAHMEN_BLOCK_FILE, 1, 1, 0, block_line++);
  
  // console picture need not be rendered fast or something.  This
  // really has time, so we load it as a surface and do not take the
  // elements apart (they dont have typical block format either)
  ne_console_surface=SDL_LoadBMP( NE_CONSOLEN_PIC_FILE );

  /* 
     TEST: show those successively on the screen 
  */

  SDL_SaveBMP (ne_blocks, "../graphics/debug.bmp");

  /*
  for (i=0; i<ALLBLASTTYPES; i++)
    for (j=0; j<Blastmap[i].phases; j++)
      {
	SDL_BlitSurface (ne_blocks, &ne_map_block[0], ne_screen, NULL);
	SDL_BlitSurface (ne_blocks, &Blastmap[i].block[j], ne_screen, NULL);
	SDL_UpdateRect (ne_screen, 0,0,0,0);
	getchar_raw ();
      }
  */
  // Terminate(OK);
  
  return (TRUE);
}

#else /* the old working engine */
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

  /* new: get Elevator-ship picture */
  ElevatorPicture = (unsigned char *) 
    MyMalloc (USERFENSTERBREITE*USERFENSTERHOEHE + 10);
  Load_PCX_Image (SEITENANSICHTBILD_PCX, InternalScreen, FALSE);
  IsolateBlock (InternalScreen, ElevatorPicture, 0, 0,
		USERFENSTERBREITE, USERFENSTERHOEHE);

  /* get Menublocks for the In-game Consoles, not the Options menu! */
  Load_PCX_Image ( CONSOLENBILD_PCX, InternalScreen, FALSE);
  MenuItemPointer = MyMalloc (MENUITEMMEM);
  IsolateBlock (InternalScreen, MenuItemPointer, 0, 0, MENUITEMLENGTH,
		MENUITEMHEIGHT);

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

#endif // !NEW_ENGINE

/*-----------------------------------------------------------------
 * @Desc: 
 * @Ret: 
 *
 *-----------------------------------------------------------------*/
void
ClearVGAScreen (void)
{
#ifdef NEW_ENGINE
  return;
#endif

  memset( Outline320x200, 0, SCREENBREITE * SCREENHOEHE );

} // void ClearVGAScreen(void)


void PlusDrawEnergyBar (void);

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
@Desc: InitPalette(): laedt die allgemein gueltige Palette fuer die
						LBM- Bilder 

@Ret: OK | ERR
@Int:
* $Function----------------------------------------------------------*/
int
InitPalette (void)
{
#ifdef NEW_ENGINE
  return (OK);
#else

  /* Hier sollte die Palette geladen werden */
  Load_PCX_Image (PALBILD_PCX, InternalScreen, TRUE);
  return OK;

#endif
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

  LevelColorArray = MyMalloc (ALLLEVELCOLORS * 3 * FARBENPROLEVEL + 10);
	// NICHT FREIGEBEN !
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


/*-----------------------------------------------------------------
 * Initialise the Video display and graphics engine
 *
 *
 *-----------------------------------------------------------------*/
void
Init_Video (void)
{
  const SDL_VideoInfo *vid_info;
  SDL_Rect **vid_modes;
  char vid_driver[81];
  Uint32 flags;  /* flags for SDL video mode */

  /* Initialize the SDL library */
  // if ( SDL_Init (SDL_INIT_VIDEO | SDL_INIT_TIMER) == -1 ) 

  if ( SDL_Init (SDL_INIT_VIDEO) == -1 ) 
    {
      fprintf(stderr, "Couldn't initialize SDL: %s\n",SDL_GetError());
      Terminate(ERR);
    } else
      printf("\nSDL Video initialisation successful.\n");

  /* clean up on exit */
  atexit (SDL_Quit);

  if ( ( Font1 = LoadFont("../graphics/font01.png") ) == NULL )
    {
      fprintf(stderr, "\n\nCouldn't initialize Font.\n\nTerminating...\n\n");
      Terminate(ERR);
    } else
      printf("\nSDL Font initialisation successful.\n");

  vid_info = SDL_GetVideoInfo (); /* just curious */
  SDL_VideoDriverName (vid_driver, 80);
  
  flags = SDL_SWSURFACE | SDL_HWPALETTE ;
  if (fullscreen_on) flags |= SDL_FULLSCREEN;

  vid_modes = SDL_ListModes (NULL, SDL_SWSURFACE);

  if (vid_info->wm_available)  /* if there's a window-manager */
    {
      SDL_WM_SetCaption("Freedroid", "");
      SDL_WM_SetIcon(SDL_LoadBMP("../graphics/paraicon.bmp"), NULL);
    }


#ifdef NEW_ENGINE  /* new experimental graphics engine */
  /* 
   * currently only the simple 320x200 mode is supported for 
   * simplicity, as all our graphics are in this format
   * once this is up and running, we'll provide others modes
   * as well.
   */
  ne_bpp = 8; /* start with the simplest */
  if( !(ne_screen = SDL_SetVideoMode (320, 200, ne_bpp, flags)) )
    {
      fprintf(stderr, "Couldn't set 320x200 video mode: %s\n", SDL_GetError());
      exit(-1);
    }
  ne_vid_info = SDL_GetVideoInfo (); /* info about current video mode */
  /* RGB of transparent color in our pics */
  ne_transp_rgb.rot   = 199; 
  ne_transp_rgb.gruen =  43; 
  ne_transp_rgb.blau  =  43; 
  /* and corresponding key: */
  ne_transp_key = SDL_MapRGB(ne_screen->format, ne_transp_rgb.rot,
			     ne_transp_rgb.gruen, ne_transp_rgb.blau);

#else     /* use the old but working graphics engine */
  ScaledSurface = SDL_SetVideoMode(320*2 , 200*2, 8, flags);
  SDL_ShowCursor (SDL_DISABLE);  /* turn off display of mouse cursor */
  if ( ScaledSurface == NULL ) {
    fprintf(stderr, "Couldn't set 320x200 video mode: %s\n",
	    SDL_GetError());
    exit(2);
  } 
  screen = SDL_CreateRGBSurface( SDL_SWSURFACE , 320, 200, 8, 0, 0, 0, 0 );

#endif  /* !NEW_ENGINE */

  SDL_SetGamma( 2 , 2 , 2 );
  Current_Gamma_Correction=2;

  return;

} /* InitVideo () */

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
  Assemble_Combat_Picture (SHOW_ALL);
  PutInternFenster (TRUE);

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

  printf ("\nLadeZeichensatz() called... is that not obsolete?\n");

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
#ifdef NEW_ENGINE
  return;
#else
  SDL_Rect ThisRectangle;

  if (Parameter_screen == RealScreen) 
    {
      ThisRectangle.x=0;
      ThisRectangle.y=0;
      ThisRectangle.w=SCREENBREITE;
      ThisRectangle.h=SCREENHOEHE;
      SDL_FillRect( screen , & ThisRectangle , 0 );
    }
  else
    memset (Parameter_screen, 0, SCREENBREITE * SCREENHOEHE);
#endif // !NEW_ENGINE
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

#ifdef NEW_ENGINE
  SDL_SetColors( ne_screen , &ThisOneColor, palpos, 1 );
  // SDL_SetColors( ne_blocks , &ThisOneColor, palpos, 1 );
#else
  SDL_SetColors( ScaledSurface , &ThisOneColor, palpos, 1 );
#endif // !NEW_ENGINE

  // SDL_SetColors( screen , &ThisOneColor, palpos, 1 );
  // DebugPrintf("\nvoid SetPalCol(...): Usual end of function reached.");
}				// void SetPalCol(...)

/*-----------------------------------------------------------------
 *   Diese Funktion bringt ein Flimmer auf den Schirm
 *
 * Param: type = 0   no flimmer
 *               1   flimmer type 1 
 *               etc...
 *
 *  Note: Type 1-3 are not really implemented, just kept from earlier
 *        versions. Only type 4 works currently with SDL
 * 
 *-----------------------------------------------------------------*/

#define BLANKBREITE 20

void
Flimmern (int type)
{
  int i, j;
  unsigned char *Screenptr;
  unsigned char *Junkptr = 0x0;
  SDL_Rect LocalRectangle;

  DebugPrintf ("\nvoid Flimmern(void): Real function call confirmed.");

  //Screenptr = RealScreen;
  Screenptr = Outline320x200;

  switch (type)
    {
    case 0:
      break;
      
    case 1:
      Screenptr += USERFENSTERPOSY * SCREENBREITE + USERFENSTERPOSX;
      //      randomize ();
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
      break;

    case 2:
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
      break;

    case 3:
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

  for (i = 0; i < USERFENSTERBREITE / 2 - 2; i++)
    {
      *(Junkptr + i) = 0;
      *(Junkptr + USERFENSTERBREITE - i) = 0;
      usleep (100);
    }

  usleep (30000);
  /* Clear the rest */
  memset (Junkptr, 0, USERFENSTERBREITE);
  break;

case 4:	

  LocalRectangle.x=USERFENSTERPOSX;
  LocalRectangle.w=USERFENSTERBREITE;
  LocalRectangle.h=1;

  /* vertical close Userfenster */
  for (i = 0; i < (USERFENSTERHOEHE / 2); i++)
    {
      memset( Outline320x200 + USERFENSTERPOSX + (USERFENSTERPOSY+i) * SCREENBREITE , 
	      0 , USERFENSTERBREITE );
      memset( Outline320x200 + USERFENSTERPOSX + (USERFENSTERPOSY+USERFENSTERHOEHE-i) * SCREENBREITE , 
	      0 , USERFENSTERBREITE );
      PrepareScaledSurface(TRUE);
    }

  /* now also close the last line in the middle */

  for (i = 0; i < USERFENSTERBREITE / 2 + 1; i++)
    {

      Outline320x200[ USERFENSTERPOSX + (USERFENSTERPOSY+USERFENSTERHOEHE/2) * SCREENBREITE + i ]=0;
      Outline320x200[ USERFENSTERPOSX + (USERFENSTERPOSY+USERFENSTERHOEHE/2) * SCREENBREITE + USERFENSTERBREITE - i ]=0;

      PrepareScaledSurface(TRUE);
    }



    default:
      break;
    } /* switch (type of flimmer) */

  return;

} /* Flimmern() */

#undef _graphics_c
