/* 
 *
 *   Copyright (c) 2003 Johannes Prix
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
#define ITEM_RUNNING_POWER_POTION 57
#define ITEM_TEMP_STRENGTH_POTION 92
#define ITEM_TEMP_DEXTERITY_POTION 93

#define ITEM_VMX_GAS_GRENADE 69
#define ITEM_EMP_SHOCK_GRENADE 70
#define ITEM_PLASMA_GRENADE 72

#define ITEM_SHORT_BOW 22
#define ITEM_HUNTERS_BOW 3
#define ITEM_LONG_BOW 16
#define ITEM_COMPOSITE_BOW 12
#define ITEM_EXTERMINATOR 13

#define ITEM_ANTIGRAV_POD 112
#define ITEM_TRACKS 10
#define ITEM_TRIPEDAL 8
#define ITEM_WHEELS 7

#define ITEM_SHOES 5
#define ITEM_BOOTS 11
#define ITEM_LIGHT_BOOTS 94
#define ITEM_REINFORCED_BOOTS 95
#define ITEM_HIKING_BOOTS 96

#define ITEM_BUCKLER 19
#define ITEM_SMALL_SHIELD 43
#define ITEM_STANDARD_SHIELD 33
#define ITEM_MEDIUM_SHIELD 97
#define ITEM_LARGE_SHIELD 56
/*
#define ITEM_MEDIUM_SHIELD 0
#define ITEM_ROUND_SHIELD 0
#define ITEM_KITE_SHIELD 0
#define ITEM_DOOR_SHIELD 0
#define ITEM_TOWER_SHIELD 0
*/

#define ITEM_ARMOR_SIMPLE_JACKET 25
#define ITEM_ARMOR_REINFORCED_JACKET 29
#define ITEM_ARMOR_PROTECTIVE_JACKET 30
#define ITEM_RED_GUARD_LIGHT_ARMOUR 31
#define ITEM_RED_GUARD_HEAVY_ARMOUR 18

#define ITEM_DAGGER 37       // very low sword
#define ITEM_SHORT_SWORD 27  // lowest sword
#define ITEM_SCIMITAR 34     // low sword
#define ITEM_SABER 35        // medium sword
#define ITEM_FALCHION 36     // good sword
#define ITEM_LONG_SWORD 24  // 
#define ITEM_CLAYMORE 26  // 

#define ITEM_MACE 113  // mace
#define ITEM_CLUB 38 // club

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

#define ITEM_SPELLBOOK_OF_PLASMA_BOLT 83
#define ITEM_SPELLBOOK_OF_ICE_BOLT 84
#define ITEM_SPELLBOOK_OF_POISON_BOLT 85
#define ITEM_SPELLBOOK_OF_PETRIFICATION 86

#define ITEM_SPELLBOOK_OF_RADIAL_EMP_WAVE 87
#define ITEM_SPELLBOOK_OF_RADIAL_VMX_WAVE 88
#define ITEM_SPELLBOOK_OF_RADIAL_PLASMA_WAVE 89
#define ITEM_SPELLBOOK_OF_DETECT_ITEMS 90
#define ITEM_SPELLBOOK_OF_IDENTIFY 91

#define ITEM_SCRIPT_OF_IDENTIFY 98
#define ITEM_SCRIPT_OF_TELEPORT_HOME 99

#define ITEM_START_PLUGIN_WARRIOR 21
#define ITEM_START_PLUGIN_SNIPER 51
#define ITEM_START_PLUGIN_HACKER 52

// This is a special item for a mission....
#define ITEM_ENERGETIC_HEART 4
#define ITEM_ULTIMATE_BOOK_OF_FREE_CODE 57
#define ITEM_ULTIMATE_COFFEE_MACHINE 57
#define ITEM_BLUE_ENERGY_DRINK 58
#define ITEM_DIXONS_TOOLBOX 59

#define ITEM_DROID_PART_1 61
#define ITEM_DROID_PART_2 62
#define ITEM_DROID_PART_3 63
#define ITEM_DROID_PART_4 64
#define ITEM_DROID_PART_5 65

#define ITEM_PLASMA_AMMUNITION 66
#define ITEM_LASER_AMMUNITION 67
#define ITEM_EXTERMINATOR_AMMUNITION 68

#define ITEM_RED_DILITIUM_CRYSTAL 74
#define ITEM_MAP_MAKER_SIMPLE 75

#define ITEM_STRENGTH_PILL 76
#define ITEM_DEXTERITY_PILL 77
#define ITEM_MAGIC_PILL 78

#define ITEM_EATING_FORK 80
#define ITEM_EATING_PLATE 81
#define ITEM_DRINKING_MUG 82

//--------------------
// And now we put here all the rectangle definitions and that...
//

#define INVENTORY_RECT_Y 264
#define INVENTORY_RECT_X 15

#define CURRENT_SKILL_RECT_X 16
#define CURRENT_SKILL_RECT_Y (GameConfig . screen_height - 16 - 64)
#define CURRENT_SKILL_RECT_W 64
#define CURRENT_SKILL_RECT_H 64

#define LEFT_WHOLE_HEALTH_RECT_X 15
#define WHOLE_HEALTH_RECT_Y 1
#define WHOLE_HEALTH_RECT_W 27
#define WHOLE_HEALTH_RECT_H 45
#define RIGHT_WHOLE_HEALTH_RECT_X ( GameConfig . screen_width - LEFT_WHOLE_HEALTH_RECT_X - WHOLE_HEALTH_RECT_W )
#define HEALTH_RECT_COLOR (63*64*64 + 64*8)

#define GOLDEN_SCHNITT ((sqrt(5)-1)/2)

#define WHOLE_RUNNING_POWER_RECT_X 8
#define WHOLE_RUNNING_POWER_RECT_Y ((1.0-GOLDEN_SCHNITT)*GameConfig . screen_height)/2
#define WHOLE_RUNNING_POWER_RECT_W 5
#define WHOLE_RUNNING_POWER_RECT_H (GOLDEN_SCHNITT*GameConfig . screen_height)

// #define WHOLE_EXPERIENCE_COUNTDOWN_RECT_X ((1.0-GOLDEN_SCHNITT)*GameConfig . screen_width)/2
#define WHOLE_EXPERIENCE_COUNTDOWN_RECT_X (1.0)
#define WHOLE_EXPERIENCE_COUNTDOWN_RECT_Y 8
#define WHOLE_EXPERIENCE_COUNTDOWN_RECT_W (GOLDEN_SCHNITT*GameConfig . screen_width)
#define WHOLE_EXPERIENCE_COUNTDOWN_RECT_H 5

#define LEFT_WHOLE_FORCE_RECT_X ( LEFT_WHOLE_HEALTH_RECT_X * 2 + WHOLE_HEALTH_RECT_W )
#define WHOLE_FORCE_RECT_Y ( WHOLE_HEALTH_RECT_Y )
#define WHOLE_FORCE_RECT_W ( WHOLE_HEALTH_RECT_W )
#define WHOLE_FORCE_RECT_H ( WHOLE_HEALTH_RECT_H )
#define RIGHT_WHOLE_FORCE_RECT_X ( GameConfig . screen_width - LEFT_WHOLE_FORCE_RECT_X - WHOLE_FORCE_RECT_W )
#define FORCE_RECT_COLOR ( 0 * 64 * 64 + 64 * 0 + 63 )

//--------------------
// The weapon rectangle is of course for the weapon the Tux is
// using.  When using a 2-handed weapon, the 'shield' rectangle
// will ALSO be used to show this weapon, since the shield-hand
// is also needed to wield such a thing.
//
#define WEAPON_RECT_WIDTH 67
#define WEAPON_RECT_HEIGHT 112
#define WEAPON_RECT_X 21
#define WEAPON_RECT_Y 117

//--------------------
// The armour rectangle is where you can place some armour
// for the body of the Tux.
//
#define ARMOUR_RECT_WIDTH 66
#define ARMOUR_RECT_HEIGHT 92
#define ARMOUR_RECT_X 129
#define ARMOUR_RECT_Y 90

//--------------------
// The shield rectangle is obviously for the shields, but not only
// that:  when using a 2-handed weapon, its also good for the weapon
// again, to indicate, that no shield can be used in conjunction with
// such a 2-handed weapon.
//
#define SHIELD_RECT_WIDTH 67
#define SHIELD_RECT_HEIGHT 112
#define SHIELD_RECT_X 237
#define SHIELD_RECT_Y 116

//--------------------
// The 'drive' rectangle is actually for the footwear.
//
#define DRIVE_RECT_WIDTH 64
#define DRIVE_RECT_HEIGHT 64
#define DRIVE_RECT_X 237
#define DRIVE_RECT_Y 25

//--------------------
// Now this is the 'helmet' rectangle.
//
#define HELMET_RECT_WIDTH 64
#define HELMET_RECT_HEIGHT 64
#define HELMET_RECT_X 22
#define HELMET_RECT_Y 26

//--------------------
// Now these are the 'bracelet' rectangles, where you could put
// some wristbands and the like.  But since there are no such things
// available in the game right now, these slots don't really have a 
// meaning and therefore currently don't need to appear in the inventory
// screen.
//
#define AUX1_RECT_WIDTH 64
#define AUX1_RECT_HEIGHT 64
#define AUX1_RECT_X 32
#define AUX1_RECT_Y 25
#define AUX2_RECT_WIDTH 64
#define AUX2_RECT_HEIGHT 64
#define AUX2_RECT_X 47
#define AUX2_RECT_Y 210

#endif
// Nothing more from here on 
