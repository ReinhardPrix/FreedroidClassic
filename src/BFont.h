/************************************************************/
/*                                                          */
/*   BFONT.h v. 1.0.3 - Billi Font Library by Diego Billi   */
/*                                                          */
/************************************************************/

#include "system.h"

typedef struct
{
  /* font height */
  int h;
  /* font surface */
  SDL_Surface *Surface;
  /* characters width */
  SDL_Rect Chars[256];
}
BFont_Info;


/* Load and store le font in the BFont_Info structure */
BFont_Info *LoadFont (char *filename, float scale);

/* Free memory */
void FreeFont (BFont_Info * Font);

/* Returns a pointer to the current font structure */
BFont_Info *GetCurrentFont (void);

/* Set the current font */
void SetCurrentFont (BFont_Info * Font);

/* Returns the font height */
int FontHeight (BFont_Info * Font);

/* Change the font height */
void SetFontHeight (BFont_Info * Font, int height);

/* Returns the character width of the specified font */
int CharWidth (BFont_Info * Font, int c);

/* Write a single character on the "Surface" with the current font */
int PutChar (SDL_Surface * Surface, int x, int y, int c);

/* Write a single character on the "Surface" with the specified font */
int PutCharFont (SDL_Surface * Surface, BFont_Info * Font, int x, int y,
		 int c);

/* Returns the width, in pixels, of the text calculated with the current font*/
int TextWidth (char *text);

/* Returns the width, in pixels, of the text calculated with the specified font*/
int TextWidthFont (BFont_Info * Font, char *text);

/* Write a string on the "Surface" with the current font */
void PutString (SDL_Surface * Surface, int x, int y, char *text);

/* Write a string on the "Surface" with the specified font */
void PutStringFont (SDL_Surface * Surface, BFont_Info * Font, int x, int y,
		    char *text);

/* Write a left-aligned string on the "Surface" with the current font */
void LeftPutString (SDL_Surface * Surface, int y, char *text);

/* Write a left-aligned string on the "Surface" with the specified font */
void LeftPutStringFont (SDL_Surface * Surface, BFont_Info * Font, int y,
			char *text);

/* Write a center-aligned string on the "Surface" with the current font */
void CenteredPutString (SDL_Surface * Surface, int y, char *text);

/* Write a center-aligned string on the "Surface" with the specified font */
void CenteredPutStringFont (SDL_Surface * Surface, BFont_Info * Font, int y,
			    char *text);

/* Write a right-aligned string on the "Surface" with the specified font */
void RightPutString (SDL_Surface * Surface, int y, char *text);

/* Write a right-aligned string on the "Surface" with the specified font */
void RightPutStringFont (SDL_Surface * Surface, BFont_Info * Font, int y,
			 char *text);

/* Write a justify-aligned string on the "Surface" with the specified font */
void JustifiedPutString (SDL_Surface * Surface, int y, char *text);

/* Write a justify-aligned string on the "Surface" with the specified font */
void JustifiedPutStringFont (SDL_Surface * Surface, BFont_Info * Font,
			     int y, char *text);


/* The following functions do the same task but have the classic "printf" sintax */

void PrintString (SDL_Surface * Surface, int x, int y, char *fmt, ...);
void PrintStringFont (SDL_Surface * Surface, BFont_Info * Font, int x,
		      int y, char *fmt, ...);

void CenteredPrintString (SDL_Surface * Surface, int y, char *fmt, ...);
void CenteredPrintStringFont (SDL_Surface * Surface, BFont_Info * Font,
			      int y, char *fmt, ...);

void RightPrintString (SDL_Surface * Surface, int y, char *fmt, ...);
void RightPrintStringFont (SDL_Surface * Surface, BFont_Info * Font, int y,
			   char *fmt, ...);

void LeftPrintString (SDL_Surface * Surface, int y, char *fmt, ...);
void LeftPrintStringFont (SDL_Surface * Surface, BFont_Info * Font, int y,
			  char *fmt, ...);

void JustifiedPrintString (SDL_Surface * Surface, int y, char *fmt, ...);
void JustifiedPrintStringFont (SDL_Surface * Surface, BFont_Info * Font,
			       int y, char *fmt, ...);

/* Returns a new font colored with the color (r,g,b) */
BFont_Info *SetFontColor (BFont_Info * Font, Uint8 r, Uint8 g, Uint8 b);
