/* 
 *
 *   Copyright (c) 1994, 2002, 2003 Johannes Prix
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

/*
 * This file has been checked for remaining german comments. However, if you still
 * find some, please either translate them to english or tell me where you found them.
 * Thanks a lot, Johannes Prix.
 */

// This stuff is nescessary, so that global.h and proto.h know that they have been
// included this time from the file map.h
#ifndef _map_h
#define _map_h

/* some defines */


#define CONNECTION_STRING	"connections: "
#define WHITE_SPACE 		" \t"
#define MAP_BEGIN_STRING	"beginning_of_map"
#define WP_SECTION_BEGIN_STRING		"wp"
#define LEVEL_END_STRING	"end_of_level"

#define AREA_NAME_STRING "Area name=\""
#define LEVEL_NAME_STRING "Name of this level="
#define LEVEL_ENTER_COMMENT_STRING "Comment of the Influencer on entering this level=\""
#define BACKGROUND_SONG_NAME_STRING "Name of background song for this level="
#define MAP_END_STRING "End of pure map information for this level"

#define ITEMS_SECTION_BEGIN_STRING "Start of pure items information for this level"
#define ITEMS_SECTION_END_STRING "End of pure items information for this level"
#define ITEM_CODE_STRING "New item: type="
#define ITEM_POS_X_STRING " X="
#define ITEM_POS_Y_STRING " Y="
#define ITEM_AC_BONUS_STRING " AC="
#define ITEM_DAMAGE_STRING " DoDamage="
#define ITEM_DAMAGE_MODIFIER_STRING " ModifyDamage="
#define ITEM_MAX_DURATION_STRING " MaxDur="
#define ITEM_CUR_DURATION_STRING " CurDur="
#define ITEM_GOLD_AMOUNT_STRING " Gold="
#define ITEM_AMMO_CLIP_STRING " Ammoclip="
#define ITEM_MULTIPLICITY_STRING " Multiplicity="
#define ITEM_PREFIX_CODE_STRING " PrefixCode="
#define ITEM_SUFFIX_CODE_STRING " SuffixCode="
#define ITEM_BONUS_TO_STR_STRING " StrBon="
#define ITEM_BONUS_TO_DEX_STRING " StrDex="
#define ITEM_BONUS_TO_VIT_STRING " StrVit="
#define ITEM_BONUS_TO_MAG_STRING " StrMag="
#define ITEM_BONUS_TO_ALLATT_STRING " StrAllAtt="
#define ITEM_BONUS_TO_LIFE_STRING " StrLife="
#define ITEM_BONUS_TO_FORCE_STRING " StrForce="
#define ITEM_BONUS_TO_HEALTH_RECOVERY_STRING " StrHealthRecovery="
#define ITEM_BONUS_TO_MANA_RECOVERY_STRING " StrManaRecovery="
#define ITEM_BONUS_TO_TOHIT_STRING " StrToHit="
#define ITEM_BONUS_TO_ACDAM_STRING " StrACDam="
#define ITEM_BONUS_TO_RESELE_STRING " ResEle="
#define ITEM_BONUS_TO_RESFIR_STRING " ResFir="
#define ITEM_BONUS_TO_RESFOR_STRING " ResFor="
#define ITEM_IS_IDENTIFIED_STRING " IsIdentif="
#define CHEST_ITEMS_SECTION_BEGIN_STRING "Start of pure chest item information for this level"
#define CHEST_ITEMS_SECTION_END_STRING "End of pure chest item information for this level"

#define STATEMENT_BEGIN_STRING "Start of pure statement information for this level"
#define STATEMENT_END_STRING "End of pure statement information for this level"
#define X_POSITION_OF_STATEMENT_STRING "PosX="
#define Y_POSITION_OF_STATEMENT_STRING "PosY="
#define STATEMENT_ITSELF_ANNOUNCE_STRING "Statement=\""

#define MAP_LABEL_BEGIN_STRING "Start of pure map label information for this level"
#define MAP_LABEL_END_STRING "End of pure map label information for this level"
#define X_POSITION_OF_LABEL_STRING "label.pos.x="
#define Y_POSITION_OF_LABEL_STRING "label.pos.y="
#define LABEL_ITSELF_ANNOUNCE_STRING "label.label_name=\""

#define OBSTACLE_LABEL_BEGIN_STRING "Start of pure obstacle label information for this level"
#define OBSTACLE_LABEL_END_STRING "End of pure obstacle label information for this level"
#define OBSTACLE_LABEL_ANNOUNCE_STRING "obstacle_label_name=\""
#define INDEX_OF_OBSTACLE_NAME "obstacle_label.index="

#define OBSTACLE_DESCRIPTION_BEGIN_STRING "Start of pure obstacle description information for this level"
#define OBSTACLE_DESCRIPTION_END_STRING "End of pure obstacle description information for this level"
#define INDEX_OF_OBSTACLE_DESCRIPTION "obstacle_description.index="
#define OBSTACLE_DESCRIPTION_ANNOUNCE_STRING "obstacle_description_text=\""

#define BIG_MAP_INSERT_SECTION_BEGIN_STRING "Start of big graphics insert information for this level"
#define BIG_MAP_INSERT_SECTION_END_STRING "End of big graphics insert information for this level"
#define POSITION_X_OF_BIG_MAP_INSERT_STRING "BigGraphicsInsertPosX="
#define POSITION_Y_OF_BIG_MAP_INSERT_STRING "BigGraphicsInsertPosY="
#define BIG_MAP_INSERT_TYPE_STRING "BigGraphicsInsertType="

#define SPECIAL_FORCE_INDICATION_STRING "SpecialForce: Type="

#define OBSTACLE_DATA_BEGIN_STRING "Begin of obstacle data"
#define OBSTACLE_DATA_END_STRING "End of obstacle data"
#define OBSTACLE_TYPE_STRING "ob_type="
#define OBSTACLE_X_POSITION_STRING "ob_x="
#define OBSTACLE_Y_POSITION_STRING "ob_y="
#define OBSTACLE_LABEL_INDEX_STRING "ob_na="
#define OBSTACLE_DESCRIPTION_INDEX_STRING "ob_de="



//--------------------
// Distance, where door opens 
//
#define DOOROPENDIST2 (1.0)
#define DOOROPENDIST2_FOR_DROIDS (0.5)

//--------------------
// How big is the margin the walls have as far as collisions are concerned.
//
#define WALLPASS (4.0/64.0)

//--------------------
// How big is the margin in console tiles
//
#define CONSOLEPASS_X 	((INITIAL_BLOCK_WIDTH/2 + 4)/64.0)
#define CONSOLEPASS_Y 	((INITIAL_BLOCK_HEIGHT/2 + 4)/64.0)

//--------------------
// How big is the margin of doors and the passable area in front of doors
//
#define TUERBREITE	(6/64.0)
#define V_RANDSPACE		WALLPASS
#define V_RANDBREITE		(5/64.0)
#define H_RANDSPACE		WALLPASS
#define H_RANDBREITE		(5/64.0)

enum _colornames
{
  PD_RED,
  PD_YELLOW,
  PD_GREEN,
  PD_GRAY,
  PD_BLUE,
  PD_GREENBLUE,
  PD_DARK
};


#endif
