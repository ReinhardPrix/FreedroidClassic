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
#include "SDL_rotozoom.h"

extern int TimerFlag;

/*
----------------------------------------------------------------------
@Desc: 

This function draws a "grid" on the screen, that means every
"second" pixel is blacked out, thereby generation a fading 
effect.  This function was created to fade the background of the 
Escape menu and its submenus.

@Ret: none
----------------------------------------------------------------------
*/
void 
MakeGridOnScreen(void){
  int x,y;

  DebugPrintf("\nvoid MakeGridOnScreen(...): real function call confirmed.");
  SDL_LockSurface( ne_screen );
  for (y=0; y<SCREENHOEHE; y++) 
    {
      for (x=0; x<SCREENBREITE; x++) 
	{
	  if ((x+y)%2 == 0) 
	    {
	      putpixel( ne_screen, x, y, 0 );
	    }
	}
    }
  
  SDL_UnlockSurface( ne_screen );
  DebugPrintf("\nvoid MakeGridOnScreen(...): end of function reached.");
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

/*
----------------------------------------------------------------------
@Desc: 

This function load an image and displays it directly to the ne_screen
but without updating it.
This might be very handy, especially in the Title() function to 
display the title image and perhaps also for displaying the ship
and that.

@Ret: none
----------------------------------------------------------------------
*/
void DisplayImage(char *datafile)
{
  SDL_Surface *image;
  
  image = IMG_Load(datafile);
  if ( image == NULL ) {
    fprintf(stderr, "Couldn't load image %s: %s\n",
	    datafile, IMG_GetError());
    Terminate(ERR);
  }

  SDL_BlitSurface(image, NULL, ne_screen, NULL);

  SDL_FreeSurface(image);
}


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

/*
----------------------------------------------------------------------
@Desc: This function initializes ALL the graphics again, propably after 
they have been destroyed by resizing operations.
This is done via freeing the old structures and starting the classical
allocations routine again.

@Ret: TRUE/FALSE, same as InitPictures() returns

----------------------------------------------------------------------
*/
int
ReInitPictures (void)
{
  SDL_FreeSurface( ne_blocks );
  SDL_FreeSurface( ne_static );

  return (InitPictures());
} // int ReInitPictures(void)


void 
SetCombatScaleTo(float ResizeFactor)
{
  int i, j;
  SDL_Surface *tmp;
  SDL_Surface *zwisch;

  // just to be sure, reset the size of the graphics
  ReInitPictures();

  //--------------------
  // now to the resizing of all combat elements
  // and the corresponding rectangle entries...

  // SDL_SetColorKey( ne_blocks , 0 , 0 );
  tmp=zoomSurface( ne_blocks , ResizeFactor , ResizeFactor , 0 );
  SDL_FreeSurface( ne_blocks );
  ne_blocks=tmp;

  for (i=0; i< NUM_MAP_BLOCKS ; i++)
    {
      ne_map_block[i].x *= ResizeFactor;
      ne_map_block[i].y *= ResizeFactor;
      ne_map_block[i].w *= ResizeFactor;
      ne_map_block[i].h *= ResizeFactor;
    }

  for (i=0; i< DROID_PHASES ; i++)
    {
      ne_influ_block[i].x *= ResizeFactor;
      ne_influ_block[i].y *= ResizeFactor;
      ne_influ_block[i].w *= ResizeFactor;
      ne_influ_block[i].h *= ResizeFactor;
    }

  for (i=0; i< DROID_PHASES ; i++)
    {
      ne_droid_block[i].x *= ResizeFactor;
      ne_droid_block[i].y *= ResizeFactor;
      ne_droid_block[i].w *= ResizeFactor;
      ne_droid_block[i].h *= ResizeFactor;
    }

  for (i=0; i < ALLBULLETTYPES; i++)
    for (j=0; j < Bulletmap[i].phases; j++)
      {
	Bulletmap[i].block[j].x *= ResizeFactor;
	Bulletmap[i].block[j].y *= ResizeFactor;
	Bulletmap[i].block[j].w *= ResizeFactor; 
	Bulletmap[i].block[j].h *= ResizeFactor;
      }

  for (i=0; i < ALLBLASTTYPES; i++)
    for (j=0; j < Blastmap[i].phases; j++)
      {
	Blastmap[i].block[j].x *= ResizeFactor;
	Blastmap[i].block[j].y *= ResizeFactor;
	Blastmap[i].block[j].w *= ResizeFactor; 
	Blastmap[i].block[j].h *= ResizeFactor;
      }

  for (i=0; i< DIGITNUMBER ; i++)
    {
      ne_digit_block[i].x *= ResizeFactor;
      ne_digit_block[i].y *= ResizeFactor;
      ne_digit_block[i].w *= ResizeFactor;
      ne_digit_block[i].h *= ResizeFactor;
    }

  Block_Width *= ResizeFactor;
  Block_Height *= ResizeFactor;

  // printf("\nDigit_Length: %d " , Digit_Length );
  // printf("\nDigit_Pos_X: %d " , Digit_Pos_X );

  Digit_Length *= ResizeFactor;
  Digit_Height *= ResizeFactor;
  Digit_Pos_X *= ResizeFactor;
  Digit_Pos_Y *= ResizeFactor;

  // printf("\nDigit_Length: %d " , Digit_Length );
  //  printf("\nDigit_Pos_X: %d " , Digit_Pos_X );

  SDL_SaveBMP ( tmp, "../graphics/debugSmall.bmp");

} // void SetCombatScaleTo(float new_scale);

/*-----------------------------------------------------------------
 * @Desc: get the pics for: druids, bullets, blasts
 * 				
 * 	reads all blocks and puts the right pointers into
 * 	the various structs
 *
 * @Ret: TRUE/FALSE
 *
 *-----------------------------------------------------------------*/

int
InitPictures (void)
{
  int i;
  SDL_Surface *tmp;
  SDL_Surface *tmp2;
  int block_line = 0;   /* keep track of line in ne_blocks we're writing */

  Block_Width=INITIAL_BLOCK_WIDTH;
  Block_Height=INITIAL_BLOCK_HEIGHT;


  /* 
     create the internal storage for all our blocks 
  */
  tmp = SDL_CreateRGBSurface(0, NUM_MAP_BLOCKS*Block_Width,
			     12*Block_Height, ne_bpp, 0, 0, 0, 0);
  tmp2 = SDL_CreateRGBSurface(0, SCREENBREITE, SCREENHOEHE, ne_bpp, 0, 0, 0, 0);
  if ( (tmp == NULL) || (tmp2 == NULL) )
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

  ne_static = SDL_DisplayFormat(tmp2);  /* the second surface is copied !*/
  if (ne_static == NULL) 
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
  if (SDL_SetColorKey(ne_static, SDL_SRCCOLORKEY, ne_transp_key) == -1 )
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

  ne_rahmen_block = ne_get_rahmen_block ( NE_RAHMEN_BLOCK_FILE );
  
  // console picture need not be rendered fast or something.  This
  // really has time, so we load it as a surface and do not take the
  // elements apart (they dont have typical block format either)
  ne_console_surface=SDL_LoadBMP( NE_CONSOLEN_PIC_FILE );


  // For debuggin purposes of the image loading procedure an
  // image of the filled ne_blocks surface is saved to a file...

  SDL_SaveBMP (ne_blocks, "../graphics/debug.bmp");

  return (TRUE);
}

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
@Desc: InitPalette(): 
This should load the right palette that is used troughout Freedroid.
But now we dont use a palette any more and the function is now
rather obsolete.

@Ret: OK | ERR
@Int:
* $Function----------------------------------------------------------*/
int
InitPalette (void)
{

  return (OK);

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

  // Now SDL_TIMER is initialized here:

  if ( SDL_InitSubSystem ( SDL_INIT_TIMER ) == -1 ) 
    {
      fprintf(stderr, "Couldn't initialize SDL: %s\n",SDL_GetError());
      Terminate(ERR);
    } else
      printf("\nSDL Timer initialisation successful.\n");

  /* clean up on exit */
  atexit (SDL_Quit);

  if ( ( Menu_BFont = LoadFont("../graphics/para_font_for_BFont_01.png") ) == NULL )
    //  if ( ( Menu_BFont = LoadFont("../graphics/font01.png") ) == NULL )
      {
        fprintf(stderr, "\n\nCouldn't initialize Font.\n\nTerminating...\n\n");
        Terminate(ERR);
  } else
  printf("\nSDL Menu Font initialisation successful.\n");

  if ( ( Para_BFont = LoadFont("../graphics/para_font_for_BFont_01.png") ) == NULL )
    {
      fprintf(stderr, "\n\nCouldn't initialize Font.\n\nTerminating...\n\n");
      Terminate(ERR);
    } else
      printf("\nSDL Para Font initialisation successful.\n");

  SetCurrentFont(Menu_BFont);

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


  /* 
   * currently only the simple 320x200 mode is supported for 
   * simplicity, as all our graphics are in this format
   * once this is up and running, we'll provide others modes
   * as well.
   */
  ne_bpp = 16; /* start with the simplest */

  #define SCALE_FACTOR 2

  if( !(ne_screen = SDL_SetVideoMode ( 320*SCALE_FACTOR, 200*SCALE_FACTOR , 0 , flags)) )
    {
      fprintf(stderr, "Couldn't set 320x200*SCALE_FACTOR video mode: %s\n", SDL_GetError());
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

  // NONSENSE FROM THE OLD ENGINE GetView ();
  Assemble_Combat_Picture ( DO_SCREEN_UPDATE );

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
ClearGraphMem ( void )
{
  // One this function is done, the rahmen at the
  // top of the screen surely is destroyed.  We inform the
  // DisplayRahmen function of the matter...
  RahmenIsDestroyed=TRUE;

  // Now we fill the screen with black color...
  SDL_FillRect( ne_screen , NULL , 0 );
} // ClearGraphMem( void )


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

  return;

  // DebugPrintf("\nvoid SetPalCol(...): Real function called.");
  // vga_setpalette (palpos, rot, gruen, blau);

  SDL_SetColors( ne_screen , &ThisOneColor, palpos, 1 );
  // SDL_SetColors( ne_blocks , &ThisOneColor, palpos, 1 );

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
