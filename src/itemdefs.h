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

#ifndef _itemdefs_h
#define _itemdefs_h

//--------------------
// At first we give here all the definitions of (regular) items
// appearing in the game.
//

#define ITEM_MONEY 6

#define ITEM_SMALL_HEALTH_POTION 1
#define ITEM_SMALL_MANA_POTION 41
#define ITEM_MEDIUM_HEALTH_POTION 48
#define ITEM_MEDIUM_MANA_POTION 49
#define ITEM_FULL_HEALTH_POTION 2
#define ITEM_FULL_MANA_POTION 42

#define ITEM_VMX_GAS_GRENADE 69
#define ITEM_EMP_SHOCK_GRENADE 70
#define ITEM_PLASMA_GRENADE 72

#define ITEM_SHORT_BOW 22
#define ITEM_HUNTERS_BOW 03
#define ITEM_LONG_BOW 16
#define ITEM_COMPOSITE_BOW 12

#define ITEM_ANTIGRAV_ALPHA 5
#define ITEM_ANTIGRAV_BETA 11
#define ITEM_TRACKS 10
#define ITEM_TRIPEDAL 8
#define ITEM_WHEELS 07

#define ITEM_BUCKLER 19
#define ITEM_SMALL_SHIELD 43
#define ITEM_STANDARD_SHIELD 33
#define ITEM_MEDIUM_SHIELD 56
#define ITEM_ROUND_SHIELD 0
#define ITEM_KITE_SHIELD 0
#define ITEM_DOOR_SHIELD 0
#define ITEM_TOWER_SHIELD 0

#define ITEM_CLOAK 25
#define ITEM_ROBE 29
#define ITEM_QUILTED_ARMOUR 30
#define ITEM_LEATHER_ARMOUR 31
#define ITEM_DROID_PLATING 18

#define ITEM_DAGGER 37       // very low sword
#define ITEM_SHORT_SWORD 27  // lowest sword
#define ITEM_SCIMITAR 34     // low sword
#define ITEM_SABER 35        // medium sword
#define ITEM_FALCHION 36     // good sword

#define ITEM_CAP 20
#define ITEM_SMALL_HELM 50
#define ITEM_IRON_HAT 53
#define ITEM_IRON_HELM 54
#define ITEM_POTHELM 55

#define ITEM_STAFF 39

#define ITEM_SPELLBOOK_OF_HEALING 44
#define ITEM_SPELLBOOK_OF_EXPLOSION_CIRCLE 46
#define ITEM_SPELLBOOK_OF_EXPLOSION_RAY 47
#define ITEM_SPELLBOOK_OF_TELEPORT_HOME 45

#define ITEM_START_PLUGIN_WARRIOR 21
#define ITEM_START_PLUGIN_SNIPER 51
#define ITEM_START_PLUGIN_HACKER 52

// This is a special item for a mission....
#define ITEM_ENERGETIC_HEART 4
#define ITEM_ULTIMATE_COFFEE_MACHINE 57
#define ITEM_ULTIMATE_BOOK_OF_FREE_CODE 57
#define ITEM_DIXONS_TOOLBOX 59
#define ITEM_RED_DILITIUM_CRYSTAL 74

#define ITEM_DROID_PART_1 61
#define ITEM_DROID_PART_2 62
#define ITEM_DROID_PART_3 63
#define ITEM_DROID_PART_4 64
#define ITEM_DROID_PART_5 65

#define ITEM_PLASMA_AMMUNITION 66
#define ITEM_LASER_AMMUNITION 67
#define ITEM_EXTERMINATOR_AMMUNITION 68

#define ITEM_MAP_MAKER_SIMPLE 75

#define ITEM_STRENGTH_PILL 76
#define ITEM_DEXTERITY_PILL 77
#define ITEM_MAGIC_PILL 78

#define ITEM_MAP_MAKER_SIMPLE 75

//--------------------
// And now we put here all the rectangle definitions and that...
//

#define INVENTORY_RECT_Y 256
#define INVENTORY_RECT_X 16

#define CURRENT_SKILL_RECT_X 16
#define CURRENT_SKILL_RECT_Y (480 - 16 - 64)
#define CURRENT_SKILL_RECT_W 64
#define CURRENT_SKILL_RECT_H 64

#define UPPER_BANNER_TEXT_RECT_X 160
#define UPPER_BANNER_TEXT_RECT_Y 1
#define UPPER_BANNER_TEXT_RECT_W 320
#define UPPER_BANNER_TEXT_RECT_H 95
#define LOWER_BANNER_TEXT_RECT_X UPPER_BANNER_TEXT_RECT_X
#define LOWER_BANNER_TEXT_RECT_Y ( SCREEN_HEIGHT - UPPER_BANNER_TEXT_RECT_Y - UPPER_BANNER_TEXT_RECT_H )
#define LOWER_BANNER_TEXT_RECT_W UPPER_BANNER_TEXT_RECT_W
#define LOWER_BANNER_TEXT_RECT_H UPPER_BANNER_TEXT_RECT_H
#define BANNER_TEXT_REC_BACKGROUNDCOLOR 0x00

#define LEFT_WHOLE_HEALTH_RECT_X 15
#define WHOLE_HEALTH_RECT_Y 1
#define WHOLE_HEALTH_RECT_W 27
#define WHOLE_HEALTH_RECT_H 45
#define RIGHT_WHOLE_HEALTH_RECT_X ( SCREEN_WIDTH - LEFT_WHOLE_HEALTH_RECT_X - WHOLE_HEALTH_RECT_W )
#define HEALTH_RECT_COLOR (63*64*64 + 64*8)

#define LEFT_WHOLE_FORCE_RECT_X ( LEFT_WHOLE_HEALTH_RECT_X * 2 + WHOLE_HEALTH_RECT_W )
#define WHOLE_FORCE_RECT_Y ( WHOLE_HEALTH_RECT_Y )
#define WHOLE_FORCE_RECT_W ( WHOLE_HEALTH_RECT_W )
#define WHOLE_FORCE_RECT_H ( WHOLE_HEALTH_RECT_H )
#define RIGHT_WHOLE_FORCE_RECT_X ( SCREEN_WIDTH - LEFT_WHOLE_FORCE_RECT_X - WHOLE_FORCE_RECT_W )
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

#define SPECIAL_RECT_WIDTH 64
#define SPECIAL_RECT_HEIGHT 64
#define SPECIAL_POS_X 240
#define SPECIAL_POS_Y 16

#define AUX1_RECT_WIDTH 64
#define AUX1_RECT_HEIGHT 64
#define AUX1_POS_X 32
#define AUX1_POS_Y 25

#define AUX2_RECT_WIDTH 64
#define AUX2_RECT_HEIGHT 64
#define AUX2_POS_X 47
#define AUX2_POS_Y 210

#endif
// Nothing more from here on 
