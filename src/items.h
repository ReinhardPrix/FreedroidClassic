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

/* ----------------------------------------------------------------------
 * This file contains all the definitions nescessary for the inventory 
 * screen.
 * ---------------------------------------------------------------------- */

#define INVENTORY_RECT_Y 256
#define INVENTORY_RECT_X 16

#define UPPER_BANNER_TEXT_RECT_X 160
#define UPPER_BANNER_TEXT_RECT_Y 1
#define UPPER_BANNER_TEXT_RECT_W 320
#define UPPER_BANNER_TEXT_RECT_H 95
#define LOWER_BANNER_TEXT_RECT_X UPPER_BANNER_TEXT_RECT_X
#define LOWER_BANNER_TEXT_RECT_Y ( SCREENHEIGHT - UPPER_BANNER_TEXT_RECT_Y - UPPER_BANNER_TEXT_RECT_H )
#define LOWER_BANNER_TEXT_RECT_W UPPER_BANNER_TEXT_RECT_W
#define LOWER_BANNER_TEXT_RECT_H UPPER_BANNER_TEXT_RECT_H
#define BANNER_TEXT_REC_BACKGROUNDCOLOR 0x00

#define LEFT_WHOLE_HEALTH_RECT_X 15
#define WHOLE_HEALTH_RECT_Y 1
#define WHOLE_HEALTH_RECT_W 27
#define WHOLE_HEALTH_RECT_H 45
#define RIGHT_WHOLE_HEALTH_RECT_X ( SCREENLEN - LEFT_WHOLE_HEALTH_RECT_X - WHOLE_HEALTH_RECT_W )
#define HEALTH_RECT_COLOR (63*64*64 + 64*8)

#define LEFT_WHOLE_FORCE_RECT_X ( LEFT_WHOLE_HEALTH_RECT_X * 2 + WHOLE_HEALTH_RECT_W )
#define WHOLE_FORCE_RECT_Y ( WHOLE_HEALTH_RECT_Y )
#define WHOLE_FORCE_RECT_W ( WHOLE_HEALTH_RECT_W )
#define WHOLE_FORCE_RECT_H ( WHOLE_HEALTH_RECT_H )
#define RIGHT_WHOLE_FORCE_RECT_X ( SCREENLEN - LEFT_WHOLE_FORCE_RECT_X - WHOLE_FORCE_RECT_W )
#define FORCE_RECT_COLOR ( 0 * 64 * 64 + 64 * 0 + 63 )

#define WEAPON_RECT_WIDTH 64
#define WEAPON_RECT_HEIGHT 64
#define WEAPON_RECT_X 16
#define WEAPON_RECT_Y 97

#define ARMOUR_RECT_WIDTH 64
#define ARMOUR_RECT_HEIGHT 64
#define ARMOUR_POS_X 128
#define ARMOUR_POS_Y 96

#define SHIELD_RECT_WIDTH 64
#define SHIELD_RECT_HEIGHT 64
#define SHIELD_POS_X 240
#define SHIELD_POS_Y 97

#define DRIVE_RECT_WIDTH 64
#define DRIVE_RECT_HEIGHT 64
#define DRIVE_RECT_X 241
#define DRIVE_RECT_Y 180

#define SPECIAL_RECT_WIDTH 32
#define SPECIAL_RECT_HEIGHT 32
#define SPECIAL_POS_X 240
#define SPECIAL_POS_Y 16

#define AUX1_RECT_WIDTH 32
#define AUX1_RECT_HEIGHT 32
#define AUX1_POS_X 99
#define AUX1_POS_Y 104

#define AUX2_RECT_WIDTH 32
#define AUX2_RECT_HEIGHT 32
#define AUX2_POS_X 191
#define AUX2_POS_Y 108

