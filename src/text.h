/*
 *
 *   Copyright (c) 1994, 2002, 2003  Johannes Prix
 *   Copyright (c) 1994, 2002, 2003  Reinhard Prix
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
#ifndef _text_h
#define _text_h

/* Font Dimensionen */
#define FONTBREITE 8
#define FONTHOEHE 12
#define FONTANZAHL 101
#define FONTMEM FONTBREITE*FONTHOEHE
#define ZEILENABSTAND 5

enum _CharLen
{
  SINGLE,
  DOUBLE
};

/* ASCII-CODE of RETURN */
#define RETURN_ASCII 13
#define BACKSPACE_ASCII 8

/* Cursor-Zeichen: */
#define CURSOR_ICON 130

enum _font_colors
{
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
