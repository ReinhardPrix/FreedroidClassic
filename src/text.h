/*=@Header==============================================================
 * $Source$
 *
 * @Desc:
 * 	
 * $Revision$
 * $State$
 *
 * $Author$
 *
 * $Log$
 * Revision 1.3  2002/04/08 09:53:13  rp
 * Johannes' initial linux PORT
 *
 * Revision 1.1  1994/06/19  16:38:40  prix
 * Initial revision
 *
 *
 *-@Header------------------------------------------------------------*/
#ifndef _paratext_h
#define _paratext_h

/* Font Dimensionen */
#define FONTBREITE 8
#define FONTHOEHE 12
#define FONTANZAHL 101
#define FONTMEM FONTBREITE*FONTHOEHE
#define ZEILENABSTAND 5

enum _CharLen{
	SINGLE,
	DOUBLE
};

/* ASCII-CODE of RETURN */
#define RETURN_ASCII 13
#define BACKSPACE_ASCII 8

/* Cursor-Zeichen: */
#define CURSOR_ICON 130

enum _font_colors {
	FONT_YELLOW = 6,
	FONT_GREEN,
	FONT_BLUE,
	FONT_WHITE,
	FONT_RED,
	FONT_BLUEGREEN,
	FONT_BLACK
};

/* Startfarben */
#define FIRST_FONT_FG		FONT_GREEN
#define FIRST_FONT_BG		FONT_BLUE

#endif
